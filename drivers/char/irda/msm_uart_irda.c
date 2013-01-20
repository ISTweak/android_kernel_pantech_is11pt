/*****************************************************************************
*
* Filename:      msm_uart_irda.c
* Version:       0.1
* Description:   IrDA Setting driver
* Status:        Experimental
* Author:        Katsuhiko Takagaki<k-takagaki@e-globaledge.com>
*
*	Copyright (C) 2011, Katsuhiko Takagaki<k-takagaki@e-globaledge.com>
*
*	This program is free software; you can redistribute it and/or modify
*	it under the terms of the GNU General Public License as published by
*	the Free Software Foundation; either version 2 of the License, or
*	(at your option) any later version.
*
*	This program is distributed in the hope that it will be useful,
*	but WITHOUT ANY WARRANTY; without even the implied warranty of
*	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*	GNU General Public License for more details.
*
*	You should have received a copy of the GNU General Public License
*	along with this program; if not, write to the Free Software
*	Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
*
*****************************************************************************/
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/gpio.h>
#include <linux/platform_device.h>
#include <linux/delay.h>
#include <linux/io.h>
#include <asm/gpio.h>
#include <linux/major.h>
#include <linux/miscdevice.h>
#include <linux/poll.h>
#include <mach/msm_serial_hs.h>

#include "msm_uart_irda.h"

/*20110520 iblee : IrDA Power Test*/
#include <mach/vreg.h>

/* for MSM8650 UARTDM2(IRDA) register setting */
#define	UARTDM2_BASE_REG_ADR 	0xA3200000
#define	UARTDM2_BASE_REG_LEN 	0x100
#define UARTDM2_IRDA_ADDR 		0x38
#define	UARTDM2_MODE_NORMAL 	0x00
#define	UARTDM2_MODE_IRDA 		0x03

#define	GPIO_IRDA_SD			84
#define	GPIO_IRDA_TXD			87


static unsigned char __iomem	*uart_dm2 ;
static struct msm_uart_irda_info *uart_irda_info;

static int uart_irda_fop_open(struct inode *inode, struct file *file) ;
static int uart_irda_fop_release(struct inode *inode, struct file *file) ;

static struct msm_gpio uart2_config_data_init[] = {
	{ GPIO_CFG(84, 0, GPIO_CFG_OUTPUT,   GPIO_CFG_NO_PULL, GPIO_CFG_2MA), "IrDA_SD"},
	{ GPIO_CFG(85, 3, GPIO_CFG_INPUT,   GPIO_CFG_PULL_UP, GPIO_CFG_2MA), "UART2DM_Rx"},
	{ GPIO_CFG(87, 0, GPIO_CFG_OUTPUT,  GPIO_CFG_NO_PULL, GPIO_CFG_2MA), "UART2DM_Tx"},
};

static struct msm_gpio uart2_config_data[] = {
	{ GPIO_CFG(87, 3, GPIO_CFG_OUTPUT,  GPIO_CFG_NO_PULL, GPIO_CFG_2MA), "UART2DM_Tx"},
};

/* ------------------------------------------------------------------------------------
 *		uart irda file operation
 * ------------------------------------------------------------------------------------ */
static int uart_irda_fop_open(struct inode *inode, struct file *file)
{
	int retval;

	printk( "%s:%d\n", __func__, __LINE__ );
	printk( "%s: major %d minor %d (pid %d)\n", __func__, imajor(inode), iminor(inode), current->pid);

	file->private_data = uart_irda_info ;


		
#if 1 /*20110520 iblee : IrDA Power Test*/
	{
		struct vreg *vreg_gp9; /*L12*/
		int rc = 0;
		
		vreg_gp9 = vreg_get(NULL, "gp9");
		if (IS_ERR(vreg_gp9)) 
		{
			rc = PTR_ERR(vreg_gp9);
			pr_err("%s: vreg_xo_out vreg get failed (%d)\n",
			__func__, rc);
//				return -1;
		}
		else
		{
			rc = vreg_set_level(vreg_gp9, 3000);
			rc = vreg_enable(vreg_gp9);
			if (rc) {
			pr_err("%s: vreg_xo_out vreg enable failed (%d)\n",
			__func__, rc);
//				return rc;
			}
		}
	}	
#endif	

	retval = msm_gpios_enable(uart2_config_data,
			ARRAY_SIZE(uart2_config_data));
	printk( "msm_gpios_request_enable retval=%d\n", retval);

	iowrite32(UARTDM2_MODE_IRDA, uart_dm2+UARTDM2_IRDA_ADDR);
	printk( "iowrite32(UARTDM2_MODE_IRDA)=0x%x\n",UARTDM2_MODE_IRDA);

	// Set LOW at the IrDA SD port
	gpio_set_value(GPIO_IRDA_SD, 0);

	return 0;
}

static int uart_irda_fop_release(struct inode *inode, struct file *file)
{
	printk( "%s: major %d minor %d (pid %d)\n", __func__, imajor(inode), iminor(inode), current->pid);

/* Set LOW at the IrDA SD port */
	gpio_set_value(GPIO_IRDA_SD, 1);

	printk( "%s:%d\n", __func__, __LINE__ );

#if 1 /*20110520 iblee : IrDA Power Test*/
	{
		struct vreg *vreg_gp9; /*L12*/
		int rc = 0;
		
		vreg_gp9 = vreg_get(NULL, "gp9");
		if (IS_ERR(vreg_gp9)) 
		{
			rc = PTR_ERR(vreg_gp9);
			pr_err("%s: vreg_xo_out vreg get failed (%d)\n",
			__func__, rc);
//				return -1;
		}
		else
		{
			rc = vreg_disable(vreg_gp9);
			if (rc) {
			pr_err("%s: vreg_xo_out vreg enable failed (%d)\n",
			__func__, rc);
//				return rc;
			}
		}
	}	
#endif	

	return 0;
}


static struct class *uart_irda_class;

struct file_operations uart_irda_fops =
{
	.owner		= THIS_MODULE,
	.open		= uart_irda_fop_open,
	.release	= uart_irda_fop_release,
};

static char *uart_irda_devnode(struct device *dev, mode_t *mode)
{
	printk("uart_irda_devnode\n");

	if (mode)
		*mode = 0666;
	return kasprintf(GFP_KERNEL,"%s", dev_name(dev));
}

static int init_uart_irda( void )
{
	int ret = 0;
	int retval = 0;
	
	printk( KERN_NOTICE"IRDA module is beeing initialized.\n" ) ;
	uart_irda_info = kzalloc(sizeof(*uart_irda_info), GFP_KERNEL);
	if (uart_irda_info == NULL) {
		pr_err(MODULE_NAME ":kzalloc err.\n");
		return -ENOMEM;
	}

	uart_irda_class = class_create(THIS_MODULE, MODULE_NAME);

	ret = alloc_chrdev_region(&uart_irda_info->dev_num, 0, 1, MODULE_NAME);
	if (ret) {
		printk(MODULE_NAME "alloc_chrdev_region err.\n");
		return -ENODEV;
	}
	uart_irda_class->devnode = uart_irda_devnode;
	uart_irda_info->dev = device_create(uart_irda_class, NULL, uart_irda_info->dev_num,
				      uart_irda_info, MODULE_NAME);
	if (IS_ERR(uart_irda_info->dev)) {
		printk(MODULE_NAME ":device_create err.\n");
		return -ENODEV;
	}

	uart_irda_info->cdev = cdev_alloc();
	if (uart_irda_info->cdev == NULL) {
		printk(MODULE_NAME ":cdev_alloc err.\n");
		return -ENODEV;
	}
	cdev_init(uart_irda_info->cdev, &uart_irda_fops);
	uart_irda_info->cdev->owner = THIS_MODULE;

	ret = cdev_add(uart_irda_info->cdev, uart_irda_info->dev_num, 1);
	if (ret) {
		printk(MODULE_NAME ":cdev_add err=%d\n", -ret);
	} else {
		printk(MODULE_NAME ":uart_irda init OK..\n");
	}

	/* setting momory mapping I/O */
	uart_dm2 = ioremap( UARTDM2_BASE_REG_ADR, UARTDM2_BASE_REG_LEN );

	retval = msm_gpios_enable(uart2_config_data_init,
			ARRAY_SIZE(uart2_config_data_init));
	printk( "msm_gpios_request_enable retval=%d\n", retval);

	// Set Low at the IrDA TXD port
	gpio_set_value(GPIO_IRDA_TXD, 0);
	// Set High at the IrDA SD port
	gpio_set_value(GPIO_IRDA_SD, 1);

	printk( " %s driver installed.\n", MODULE_NAME );

	return ret;

}

static void exit_uart_irda( void )
{
	iounmap( uart_dm2 );
	cdev_del(uart_irda_info->cdev);
	device_destroy(uart_irda_class, uart_irda_info->dev_num);
	unregister_chrdev_region(uart_irda_info->dev_num, 1);

	kfree(uart_irda_info);
	printk( "IRDA module is removed.\n" ) ;
}

module_init( init_uart_irda ) ;
module_exit( exit_uart_irda ) ;

