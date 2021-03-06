/*
 * Copyright (C) 2009 PANTECH, Co. Ltd.
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
#include <linux/kernel.h>
#include <linux/platform_device.h>
#include <linux/err.h>
#include <linux/hrtimer.h>
#include <linux/sched.h>

#include <mach/msm_rpcrouter.h>

#include <linux/pansw.h>
#include <mach/rpc_nv.h>
#include "smd_private.h"

// 20110301 khlee_wifi for wifi rpc to read nv mac address 

typedef	union {
	uint32_t	bytes;
	struct {
		// LSB
		uint32_t	conloglvl:4;	// console printk (console_loglevel)
								// 10 : debug kernel
								// 4  : quiet kernel
								// 7  : default value
								// 0  : console silent
								// 15 : console verbose
		uint32_t	alog:1;		// android UART log on/off
		uint32_t	reserved:11;
		uint32_t	useless:16;
		// MSB
	} bits;
} debug_option_type;


static debug_option_type	debug_flag;

void init_pantech_debug_option(void);
static void set_pantech_debug_option(struct pansw_classdev *sw_dev, int value);
static int pantech_debug_probe(struct platform_device *pdev);
static int pantech_debug_remove(struct platform_device *pdev);
static int __init pantech_debug_opt_init(void);
static void __exit pantech_debug_opt_exit(void);

extern void logger_set_android_uart_log(int flag);


static struct pansw_classdev pantech_debug_opt = {
	.name		= "dip",
	.enable_set = set_pantech_debug_option,
	// .enable : storage for setting value
};

void init_pantech_debug_option(void)
{
// 20110301 khlee_wifi commented out for build error
//	int	r;
	
	printk(KERN_INFO "%s is called\n", __func__);
#if 0  // 20100926 jylee
	r = msm_nv_read(NV_SPARE_1_I, (nv_cmd_item_type *)&debug_flag.bytes);
	if (r != NV_DONE_S) { 
		// minus : RPC error, plus : NV error
		printk(KERN_ERR "%s: msm_nv_read error(%d)\n", __func__, r);
		return;
	}

	// TODO : set current status
	pantech_debug_opt.enable = debug_flag.bytes;

	if (debug_flag.bits.alog) {
		printk(KERN_INFO "%s: android UART log ON\n", __func__);
		logger_set_android_uart_log(1);
	}
	else {
		printk(KERN_INFO "%s: android UART log OFF\n", __func__);
		logger_set_android_uart_log(0);
	}

	if (console_loglevel == 0)
		printk(KERN_INFO "%s: console_loglevel (0x%x)\n", 
				__func__, debug_flag.bits.conloglvl);

	console_loglevel = debug_flag.bits.conloglvl;

	if (console_loglevel)
		printk(KERN_INFO "%s: console_loglevel (0x%x)\n", 
				__func__, debug_flag.bits.conloglvl);
#endif	
}
EXPORT_SYMBOL(init_pantech_debug_option);

static void set_pantech_debug_option(struct pansw_classdev *sw_dev, int value)
{
// 20110301 khlee_wifi commented out for build error
//	int	r;

	debug_flag.bytes = (uint32_t)value;

	if (debug_flag.bits.alog) {
		printk(KERN_INFO "%s: android UART log ON\n", __func__);
		logger_set_android_uart_log(1);
	}
	else {
		printk(KERN_INFO "%s: android UART log OFF\n", __func__);
		logger_set_android_uart_log(0);
	}

	if (console_loglevel == 0)
		printk(KERN_INFO "%s: console_loglevel (0x%x)\n", 
				__func__, debug_flag.bits.conloglvl);

	console_loglevel = debug_flag.bits.conloglvl;

	if (console_loglevel)
		printk(KERN_INFO "%s: console_loglevel (0x%x)\n", 
				__func__, debug_flag.bits.conloglvl);
#if 0  // 20100926 jylee
	r = msm_nv_write(NV_SPARE_1_I, (nv_cmd_item_type *)&debug_flag.bytes);
	if (r != NV_DONE_S) { 
		// minus : RPC error, plus : NV error
		printk(KERN_ERR "%s: msm_nv_write error(%d)\n", __func__, r);
		return;
	}
#endif 	
}

static int pantech_debug_probe(struct platform_device *pdev)
{
	int ret;

	printk(KERN_ERR "%s is called\n", __func__);
	ret = pansw_classdev_register(&pdev->dev, &pantech_debug_opt);
	if (ret)
		goto err_debug;

	return 0;

err_debug:
	return	ret;
}

static int pantech_debug_remove(struct platform_device *pdev)
{
	pansw_classdev_unregister(&pantech_debug_opt);

	return 0;
}

static struct platform_driver pantech_debug_opt_driver = {
	.driver = {
		.name = "pantech-debug",
		.owner = THIS_MODULE,
	},
	.probe	= pantech_debug_probe,
	.remove = pantech_debug_remove,
};

static int __init pantech_debug_opt_init(void)
{
	int	rc;

	printk(KERN_INFO "%s is called\n", __func__);

	rc = platform_driver_register(&pantech_debug_opt_driver);
	if (rc < 0) {
		printk(KERN_ERR "%s: platform_driver_register rc = %d\n",
			__func__, rc);
	}

	rc = msm_nv_rpc_connect();
	if (rc == 0) {
		init_pantech_debug_option();
	}
	else {
		printk(KERN_ERR "%s : msm_nv_rpc_connect error(%d)\n", 
			__func__, rc);
	}

	return	rc;
}

static void __exit pantech_debug_opt_exit(void)
{
	platform_driver_unregister(&pantech_debug_opt_driver);
}

late_initcall(pantech_debug_opt_init);

MODULE_AUTHOR("Jinwoo Cha <jwcha@pantech.com>");
MODULE_DESCRIPTION("PANTECH debug option device");
MODULE_LICENSE("GPL");

