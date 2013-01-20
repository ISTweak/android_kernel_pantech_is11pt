/*****************************************************************************
*
* Filename:      msm_uart_irda.h
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
#if !defined(__irda_eg_h__)
#define	__irda_eg_h__

#include <linux/spinlock.h>
#include <linux/skbuff.h>
#include <linux/module.h>
#include <linux/cdev.h>

MODULE_AUTHOR("Katsuhiko Takagaki<k-takagaki@e-globaledge.com>");
MODULE_DESCRIPTION("MSM UART IrDA Setting Driver");
MODULE_LICENSE("GPL");


#define MODULE_NAME "msm_uart_irda"


struct msm_uart_irda_info {
	dev_t dev_num;
	struct device *dev;
	struct cdev *cdev;
};



#endif
