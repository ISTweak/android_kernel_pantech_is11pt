/* kernel/power/earlysuspend.c
 *
 * Copyright (C) 2005-2008 Google, Inc.
 *
 * This software is licensed under the terms of the GNU General Public
 * License version 2, as published by the Free Software Foundation, and
 * may be copied, distributed, and modified under those terms.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 */

#include <linux/earlysuspend.h>
#include <linux/module.h>
#include <linux/mutex.h>
#include <linux/rtc.h>
#include <linux/wakelock.h>
#include <linux/workqueue.h>
#include <linux/kallsyms.h>
#include "power.h"

#ifdef CONFIG_DEBUG_EARLYSUSPEND
#include <linux/proc_fs.h>
#define MODULE_NAME "procfs_earlysuspend"
#define EARLYSUSPEND_START 0xAA
#define EARLYSUSPEND_END     0xEE
#define LATESUME_START           0xBB
#define LATESUME_END               0xFF
static struct proc_dir_entry *earlysuspend_dir,*earlysuspend_info,*lateresume_info;

#endif

enum {
	DEBUG_USER_STATE = 1U << 0,
	DEBUG_SUSPEND = 1U << 2,
};
static int debug_mask = DEBUG_USER_STATE | DEBUG_SUSPEND;
module_param_named(debug_mask, debug_mask, int, S_IRUGO | S_IWUSR | S_IWGRP);

static DEFINE_MUTEX(early_suspend_lock);
static LIST_HEAD(early_suspend_handlers);
static void early_suspend(struct work_struct *work);
static void late_resume(struct work_struct *work);
static DECLARE_WORK(early_suspend_work, early_suspend);
static DECLARE_WORK(late_resume_work, late_resume);
static DEFINE_SPINLOCK(state_lock);
enum {
	SUSPEND_REQUESTED = 0x1,
	SUSPENDED = 0x2,
	SUSPEND_REQUESTED_AND_SUSPENDED = SUSPEND_REQUESTED | SUSPENDED,
};
static int state;

void register_early_suspend(struct early_suspend *handler)
{
	struct list_head *pos;

	mutex_lock(&early_suspend_lock);
	list_for_each(pos, &early_suspend_handlers) {
		struct early_suspend *e;
		e = list_entry(pos, struct early_suspend, link);
		if (e->level > handler->level)
			break;
	}
	list_add_tail(&handler->link, pos);
	if ((state & SUSPENDED) && handler->suspend)
		handler->suspend(handler);
	mutex_unlock(&early_suspend_lock);
}
EXPORT_SYMBOL(register_early_suspend);

void unregister_early_suspend(struct early_suspend *handler)
{
	mutex_lock(&early_suspend_lock);
	list_del(&handler->link);
	mutex_unlock(&early_suspend_lock);
}
EXPORT_SYMBOL(unregister_early_suspend);

//xsemiyas_debug
static struct early_suspend *xsemiyas_curr_suspend = NULL;
static struct early_suspend *xsemiyas_curr_resume = NULL;
static int count_s = 0;
static int count_r = 0;

static void early_suspend(struct work_struct *work)
{
	struct early_suspend *pos;
	unsigned long irqflags;
	int abort = 0;
	char buffer[KSYM_SYMBOL_LEN];

	mutex_lock(&early_suspend_lock);
	spin_lock_irqsave(&state_lock, irqflags);
	if (state == SUSPEND_REQUESTED)
		state |= SUSPENDED;
	else
		abort = 1;
	spin_unlock_irqrestore(&state_lock, irqflags);

	if (abort) {
		if (debug_mask & DEBUG_SUSPEND)
			pr_info("early_suspend: abort, state %d\n", state);
		mutex_unlock(&early_suspend_lock);
		goto abort;
	}

	if (debug_mask & DEBUG_SUSPEND)
		pr_info("early_suspend: call handlers\n");
		//xsemiyas_debug
	count_s = 0;
	list_for_each_entry(pos, &early_suspend_handlers, link) {
#ifdef CONFIG_DEBUG_EARLYSUSPEND
		pr_info("[PANTECH_TRACE] early_suspend pos:%d\n",pos->level);
		xsemiyas_curr_suspend = pos;
		printk(KERN_ERR "e_suspend [%d]\n",count_s++);
		if (pos->suspend != NULL){
			pos->suspend_flag = EARLYSUSPEND_START;
			sprint_symbol(buffer, (unsigned int)(pos->suspend));
			printk("## NK [%s:%d] early suspend : 0x%08x ,buffer -> %s\n", __func__, __LINE__, (unsigned int)(pos->suspend), buffer); 
            pos->suspend(pos);
			pos->suspend_flag = EARLYSUSPEND_END;
		}
#else
		if (pos->suspend != NULL){
		    sprint_symbol(buffer, (unsigned int)(pos->suspend));
			printk("## NK [%s:%d] early suspend : 0x%08x ,buffer -> %s\n", __func__, __LINE__, (unsigned int)(pos->suspend), buffer); 
			pos->suspend(pos);
			
			}
#endif
	}
	mutex_unlock(&early_suspend_lock);

	if (debug_mask & DEBUG_SUSPEND)
		pr_info("[PANTECH_TRACE]early_suspend: sync\n");  

	suspend_sys_sync_queue();
abort:
	spin_lock_irqsave(&state_lock, irqflags);
	if (state == SUSPEND_REQUESTED_AND_SUSPENDED)
		wake_unlock(&main_wake_lock);
	spin_unlock_irqrestore(&state_lock, irqflags);
}

static void late_resume(struct work_struct *work)
{
	struct early_suspend *pos;
	unsigned long irqflags;
	int abort = 0;

	mutex_lock(&early_suspend_lock);
	spin_lock_irqsave(&state_lock, irqflags);
	if (state == SUSPENDED)
		state &= ~SUSPENDED;
	else
		abort = 1;
	spin_unlock_irqrestore(&state_lock, irqflags);

	if (abort) {
		if (debug_mask & DEBUG_SUSPEND)
			pr_info("late_resume: abort, state %d\n", state);
		goto abort;
	}
	if (debug_mask & DEBUG_SUSPEND)
		pr_info("late_resume: call handlers\n");
		count_r = 0;
	list_for_each_entry_reverse(pos, &early_suspend_handlers, link){
#ifdef CONFIG_DEBUG_EARLYSUSPEND
		xsemiyas_curr_resume = pos;
		printk(KERN_ERR "l_resume [%d]\n",count_r++);
		pr_info("[PANTECH_TRACE] late_resume pos:%d\n",pos->level);
		if (pos->resume != NULL){
            pos->resume_flag = LATESUME_START;
			pos->resume(pos);
            pos->resume_flag = LATESUME_END;
        }
	}
#else
		if (pos->resume != NULL)
			pos->resume(pos);
#endif
	if (debug_mask & DEBUG_SUSPEND)
		pr_info("[PANTECH_TRACE]late_resume: done\n");
abort:
	mutex_unlock(&early_suspend_lock);
}

void request_suspend_state(suspend_state_t new_state)
{
	unsigned long irqflags;
	int old_sleep;

	spin_lock_irqsave(&state_lock, irqflags);
	old_sleep = state & SUSPEND_REQUESTED;
	if (debug_mask & DEBUG_USER_STATE) {
		struct timespec ts;
		struct rtc_time tm;
		getnstimeofday(&ts);
		rtc_time_to_tm(ts.tv_sec, &tm);
		pr_info("request_suspend_state: %s (%d->%d) at %lld "
			"(%d-%02d-%02d %02d:%02d:%02d.%09lu UTC)\n",
			new_state != PM_SUSPEND_ON ? "sleep" : "wakeup",
			requested_suspend_state, new_state,
			ktime_to_ns(ktime_get()),
			tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday,
			tm.tm_hour, tm.tm_min, tm.tm_sec, ts.tv_nsec);
	}
	if (!old_sleep && new_state != PM_SUSPEND_ON) {
		state |= SUSPEND_REQUESTED;
		pr_info("[PANTECH_TRACE]+early_suspend_work\n");		
		queue_work(suspend_work_queue, &early_suspend_work);
		pr_info("[PANTECH_TRACE]-early_suspend_work\n");
	} else if (old_sleep && new_state == PM_SUSPEND_ON) {
		state &= ~SUSPEND_REQUESTED;
		pr_info("[PANTECH_TRACE]+main_wake_lock\n");
		wake_lock(&main_wake_lock);
		pr_info("[PANTECH_TRACE]-main_wake_lock & +late_resume_work\n");
		queue_work(suspend_work_queue, &late_resume_work);
		pr_info("[PANTECH_TRACE]-late_resume_work\n");
	}
	requested_suspend_state = new_state;
	spin_unlock_irqrestore(&state_lock, irqflags);
}

suspend_state_t get_suspend_state(void)
{
	return requested_suspend_state;
}

#ifdef CONFIG_DEBUG_EARLYSUSPEND

static int proc_read_earlysuspend(char *page, char **start,
                                                 off_t off, int count,
                                                 int *eof, void *data)
{
	int len = 0;
	char buf[KSYM_SYMBOL_LEN];
	struct early_suspend *pos;

	list_for_each_entry(pos, &early_suspend_handlers, link) {
		if (pos->suspend != NULL){
			memset(buf,0,KSYM_SYMBOL_LEN);
			sprint_symbol(buf,(unsigned long)pos->suspend);
			len += sprintf(page+len, "[%03d:%02X] = %s \n",pos->level,pos->suspend_flag,buf);
		}
	}

	return len;
}

static int proc_read_lateresume(char *page,char **start,
                                              off_t off, int count,
                                              int *eof, void *data)
{
	int len = 0;
	char buf[KSYM_SYMBOL_LEN];
	struct early_suspend *pos;

	list_for_each_entry_reverse(pos, &early_suspend_handlers, link) {
		if (pos->resume != NULL){
			memset(buf,0,KSYM_SYMBOL_LEN);
			sprint_symbol(buf,(unsigned long)pos->resume);
			len += sprintf(page+len, "[%03d:%02X] = %s \n",pos->level,pos->resume_flag,buf);
		}
	}

	return len;
}

static int __init init_procfs_earlysuspend(void)
{
	int ret_val = 0;
	earlysuspend_dir = proc_mkdir(MODULE_NAME,NULL);
	if(earlysuspend_dir == NULL){
		ret_val = -ENOMEM;
		goto exit;
	}

	earlysuspend_info = create_proc_read_entry("early_suspend_info",
                                                                    0444,earlysuspend_dir,
                                                                    proc_read_earlysuspend,
                                                                    NULL);
	if(earlysuspend_info == NULL){
		ret_val = -ENOMEM;
		goto no_earlysuspend;
	}

	lateresume_info = create_proc_read_entry("late_resume_info",
                                                                 0444,earlysuspend_dir,
                                                                 proc_read_lateresume,
                                                                 NULL);
	if(lateresume_info == NULL){
		ret_val = -ENOMEM;
		goto no_lateresume_info;
	}

	printk(KERN_INFO "%s %s initialized\n",__func__,MODULE_NAME);
	return 0;

no_lateresume_info:
	remove_proc_entry("early_suspend_info",earlysuspend_dir);

no_earlysuspend:
	remove_proc_entry(MODULE_NAME,NULL);
exit:
	printk(KERN_INFO "%s %s initialize failed\n",__func__,MODULE_NAME);
	return ret_val;
}

static void __exit cleanup_procfs_earlysuspend(void)
{
	remove_proc_entry("early_suspend_info",earlysuspend_dir);
	remove_proc_entry("late_resume_info",earlysuspend_dir);

	printk(KERN_INFO "%s_removed\n",__func__);
}


module_init(init_procfs_earlysuspend);
module_exit(cleanup_procfs_earlysuspend);

MODULE_AUTHOR("Chero Jeong chero@pantech.com");
MODULE_DESCRIPTION("earlysuspend procfs info");

#endif