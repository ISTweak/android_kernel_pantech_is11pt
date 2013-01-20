/* drivers/misc/apanic_pantech.c
 *
 * Copyright (C) 2011 PANTECH, Co. Ltd.
 * based on drivers/misc/apanic.c
 * This software is licensed under the terms of the GNU General Public
 * License version 2, as published by the Free Software Foundation, and
 * may be copied, distributed, and modified under those terms.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.      See the
 * GNU General Public License for more details.
 *
 */

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/string.h>
#include <linux/errno.h>
#include <linux/init.h>
#include <linux/interrupt.h>
#include <linux/device.h>
#include <linux/types.h>
#include <linux/delay.h>
#include <linux/sched.h>
#include <linux/wait.h>
#include <linux/wakelock.h>
#include <linux/platform_device.h>
#include <linux/uaccess.h>
#include <linux/notifier.h>
#include <linux/debugfs.h>
#include <linux/fs.h>
#include <linux/proc_fs.h>
#include <linux/mutex.h>
#include <linux/workqueue.h>
#include <linux/preempt.h>
#include <asm/cacheflush.h>
#include <asm/system.h>

#include "smd_private.h"
#include "proc_comm.h"
#include "modem_notifier.h"


#define POS(x) (x > 0 ? x : 0)
#define WRITE_LOG(...) \
  if(bufsize != 0) { \
    n = snprintf(s, POS(bufsize), __VA_ARGS__); \
    s+=n; \
    total +=n; \
    bufsize-=n;\
  }

#define MMU_SCRIPT_BUF_SIZE 512
#define EXCP_SCRIPT_BUF_SIZE 512
extern void ram_console_enable_console(int);

#define PANIC_MAGIC      0xDAEDDAED
#define PHDR_VERSION   0x01
struct panic_info {
      unsigned int magic;
      unsigned int version;

      char mmu_cmm_script[MMU_SCRIPT_BUF_SIZE];
       int mmu_cmm_size;

      unsigned int console_offset;
      unsigned int console_length;

      unsigned int threads_offset;
      unsigned int threads_length;
      
      unsigned int logcat_offset;
      unsigned int logcat_length;
      unsigned int total_size;
};

struct apanic_config {
      unsigned int initialized;
      unsigned buf_size;
      unsigned int writesize;
      void *bounce; 
};

static struct apanic_config driver_context;

static int in_panic = 0;

extern int log_buf_copy(char *dest, int idx, int len);
extern void log_buf_clear(void);

extern int logcat_buf_copy(char *dest,int idx,int len);
extern void logcat_set_log(int index);


static inline unsigned get_ctrl(void)
{
      unsigned int val;
      asm("mrc p15,0,%0,c1,c0,0" : "=r" (val));
      return val;
}

static inline unsigned get_transbase0(void)
{
      unsigned int val;
      asm("mrc p15,0,%0,c2,c0,0" : "=r" (val));
      return val;
}

static inline unsigned get_transbase1(void)
{
      unsigned int val;
      asm("mrc p15,0,%0,c2,c0,1" : "=r" (val));
      return val;
}

static inline unsigned get_dac(void)
{
      unsigned int val;
      asm("mrc p15,0,%0,c3,c0,0" : "=r" (val));
      return val;
}

static inline unsigned get_prrr(void)
{
      unsigned int val;
      asm("mrc p15,0,%0,c10,c2,0" : "=r" (val));
      return val;
}

static inline unsigned get_nmrr(void)
{
      unsigned int val;
      asm("mrc p15,0,%0,c10,c2,1" : "=r" (val));
      return val;
}

static void apanic_get_mmu_info(struct panic_info *info)
{
      int  bufsize = MMU_SCRIPT_BUF_SIZE, n = 0,total=0;
      char *s;
      unsigned int mmu_transbase0,mmu_transbase1;
      unsigned int mmu_dac,mmu_control;
      unsigned int mmu_prrr,mmu_nmrr;

      mmu_control =  get_ctrl();
      mmu_transbase0 = get_transbase0();
      mmu_dac = get_dac();
      mmu_transbase1 = get_transbase1();
      mmu_prrr = get_prrr();
      mmu_nmrr = get_nmrr();

      s =(char *)info->mmu_cmm_script;
      WRITE_LOG("PER.S C15:0x1 %%LONG 0x%X\n",mmu_control);
      WRITE_LOG("PER.S C15:0x2 %%LONG 0x%X\n",mmu_transbase0);
      WRITE_LOG("PER.S C15:0x3 %%LONG 0x%X\n",mmu_dac);
      WRITE_LOG("PER.S C15:0x102 %%LONG 0x%X\n",mmu_transbase1);
      WRITE_LOG("PER.S C15:0x2A %%LONG 0x%X\n",mmu_prrr);
      WRITE_LOG("PER.S C15:0x12A %%LONG 0x%X\n",mmu_nmrr);
      WRITE_LOG("MMU.SCAN\n");
      WRITE_LOG("MMU.ON\n");
      WRITE_LOG("\n\n\n"); /* 32bit boundary */
      info->mmu_cmm_size = total;
}
/*
 * Writes the contents of the console to the specified offset in flash.
 * Returns number of bytes written
 */
static int apanic_write_console_log(unsigned int off)
{
      struct apanic_config *ctx = &driver_context;
      int saved_oip;
      int idx = 0;
      int rc;
      unsigned int last_chunk = 0;
      unsigned char *cur_bounce;

      cur_bounce = (unsigned char *)((unsigned int)ctx->bounce + off);

      while (!last_chunk) {
            saved_oip = oops_in_progress;
            oops_in_progress = 1;
            rc = log_buf_copy(cur_bounce+idx, idx, ctx->writesize);
            if (rc < 0)
                  break;

            if (rc != ctx->writesize)
                  last_chunk = rc;

            oops_in_progress = saved_oip;
            if (rc <= 0)
                  break;

            if (rc != ctx->writesize)
                  memset(cur_bounce + idx + rc, 0, ctx->writesize - rc);

            if (!last_chunk)
                  idx += rc;
            else
                  idx += last_chunk;

      }
      return idx;
}

static int apanic_write_logcat_log(unsigned int off)
{
      struct apanic_config *ctx = &driver_context;
      int saved_oip;
      int idx = 0;
      int rc ;
      unsigned int last_chunk = 0;
      unsigned char *cur_bounce;

      cur_bounce = (unsigned char *)((unsigned int)ctx->bounce + off);

      while (!last_chunk) {
            saved_oip = oops_in_progress;
            oops_in_progress = 1;
            rc = logcat_buf_copy(cur_bounce+idx, idx, ctx->writesize);
            if (rc < 0)
                  break;

            if (rc != ctx->writesize)
                  last_chunk = rc;

            oops_in_progress = saved_oip;
            if (rc <= 0)
                  break;

            if (rc != ctx->writesize)
                  memset(cur_bounce + idx + rc, 0, ctx->writesize - rc);

            if (!last_chunk)
                  idx += rc;
            else
                  idx += last_chunk;

      }
      return idx;
}


static int apanic_logging(struct notifier_block *this, unsigned long event,
                  void *ptr)
{
      struct apanic_config *ctx = &driver_context;
      struct panic_info *hdr = (struct panic_info *) ctx->bounce;
      int console_offset = 0;
      int console_len = 0;
      int threads_offset = 0;
      int threads_len = 0;
      int logcat_offset = 0;
      int logcat_len = 0;

      if(!ctx->initialized)
            return -1;

      if (in_panic)
            return NOTIFY_DONE;
      in_panic = 1;
#ifdef CONFIG_PREEMPT
      /* Ensure that cond_resched() won't try to preempt anybody */
      add_preempt_count(PREEMPT_ACTIVE);
#endif

      touch_softlockup_watchdog();
 
      /* this case is kernel panic message send to modem err */
      if(ptr){
            //smem_diag_set_message((char *)ptr);
            printcrash((const char *)ptr);
      }
      console_offset = 4096;

      /*
       * Write out the console
       */
      console_len = apanic_write_console_log(console_offset);
      if (console_len < 0) {
            printk(KERN_EMERG "Error writing console to panic log! (%d)\n",
                   console_len);
            console_len = 0;
      }

      /*
       * Write out all threads
       */
      threads_offset = ALIGN(console_offset + console_len,ctx->writesize);
      if (!threads_offset)
            threads_offset = ctx->writesize;
#ifdef CONFIG_THREAD_STATE_INFO
/*
      logging time issue!!
      do use T32 simulator for thread info about it's schedule.
*/
      ram_console_enable_console(0);

      log_buf_clear();
      show_state_filter(0);

      threads_len = apanic_write_console_log(threads_offset);
      if (threads_len < 0) {
            printk(KERN_EMERG "Error writing threads to panic log! (%d)\n",
                   threads_len);
            threads_len = 0;
      }
#endif
      logcat_offset = ALIGN(threads_offset + threads_len,ctx->writesize);
      if(!logcat_offset)
            logcat_offset = ctx->writesize;

      /*
            TO - DO 
            1.log_main
            2.log_event
            3.log_radio
            4.log_system
      */

       logcat_set_log(4); /* logcat log system */

      logcat_len = apanic_write_logcat_log(logcat_offset);
      if (logcat_len < 0) {
            printk(KERN_EMERG "Error writing logcat to panic log! (%d)\n",
                   logcat_len);
            logcat_len = 0;
      }

      /*
       * Finally write the panic header
       */
      memset(ctx->bounce, 0, PAGE_SIZE);
      hdr->magic = PANIC_MAGIC;
      hdr->version = PHDR_VERSION;
      apanic_get_mmu_info(hdr);
      //apanic_get_regs_info(hdr);

      hdr->console_offset    = console_offset;
      hdr->console_length  = console_len;
      
      hdr->threads_offset = threads_offset;
      hdr->threads_length = threads_len;
      
      hdr->logcat_offset   = logcat_offset;
      hdr->logcat_length  = logcat_len;

      hdr->total_size = logcat_offset + logcat_len;

      printk(KERN_EMERG "pantech_apanic: Panic dump sucessfully written to smem\n");

      flush_cache_all();

#ifdef CONFIG_PREEMPT
      sub_preempt_count(PREEMPT_ACTIVE);
#endif
      in_panic = 0;
      return NOTIFY_DONE;
}
#if defined(CONFIG_PANTECH_APANIC_RESET)
static int apanic_reset(struct notifier_block *this, unsigned long event,void *ptr)
{
      printk(KERN_EMERG "pantech_apanic: reset modem & system download \n");

      smsm_reset_modem(SMSM_SYSTEM_DOWNLOAD);
}

static struct notifier_block next_blk = {
      .notifier_call = apanic_reset,
};
#endif

static struct notifier_block panic_blk = {
      .notifier_call    = apanic_logging,
#if defined(CONFIG_PANTECH_APANIC_RESET)
      .next = &next_blk,
#endif
};

int __init apanic_pantech_init(void)
{
      unsigned size = MAX_CRASH_BUF_SIZE;
      unsigned char *crash_buf;

      /*
            using smem_get_entry function if already memory have allocated. 
            if not AMSS err_init function must be called alloc function for ID.
      */
       memset(&driver_context,0,sizeof(struct apanic_config));

      crash_buf = (unsigned char *)smem_get_entry(SMEM_ID_VENDOR2, &size);
      if(!crash_buf){
            printk(KERN_ERR "pantech_apanic: no available crash buffer , initial failed\n");
            return 0;
      } else {
            atomic_notifier_chain_register(&panic_notifier_list, &panic_blk);
            driver_context.buf_size = size;
            driver_context.writesize = 4096;
            driver_context.bounce    = (void *)crash_buf;
            driver_context.initialized = 1;
      }
      printk(KERN_INFO "Android kernel / Modem panic handler initialized \n");
      return 0;
}

module_init(apanic_pantech_init);

MODULE_AUTHOR("JungSik Jeong <chero@pantech.com>");
MODULE_DESCRIPTION("PANTECH Error Crash misc driver");
MODULE_LICENSE("GPL");
