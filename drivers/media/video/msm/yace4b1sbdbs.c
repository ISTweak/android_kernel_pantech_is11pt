/* Copyright (c) 2009, Code Aurora Forum. All rights reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 and
 * only version 2 as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
 * 02110-1301, USA.
 *
 */

#include <linux/delay.h>
#include <linux/types.h>
#include <linux/i2c.h>
#include <linux/uaccess.h>
#include <linux/miscdevice.h>
#include <media/msm_camera.h>
#include <mach/gpio.h>
#include <mach/vreg.h>
#include "yace4b1sbdbs.h"
#include <linux/jiffies.h>
#include <linux/slab.h>

/* Micron YACE4B1SBDBS Registers and their values */
/* Sensor Core Registers */
#define  REG_YACE4B1SBDBS_MODEL_ID 0x3000
#define  YACE4B1SBDBS_MODEL_ID     0x1580

/*  SOC Registers Page 1  */
#define  REG_YACE4B1SBDBS_SENSOR_RESET     0x301A
#define  REG_YACE4B1SBDBS_STANDBY_CONTROL  0x3202
#define  REG_YACE4B1SBDBS_MCU_BOOT         0x3386

#define SENSOR_DEBUG 0
#define ERROR_DEBUG 1
#define YACE4B1SBDBS_DEBUG 1

#define YACE4B1SBDBS_CAM_SDA_GPIO 17
#define YACE4B1SBDBS_CAM_SCL_GPIO 16

#define YACE4B1SBDBS_CAM_RESET_GPIO 121
#define YACE4B1SBDBS_CAM_STANDBY_GPIO 123

//100816 Do. layer1

#if YACE4B1SBDBS_DEBUG
#define CMSG(fmt, arg...) printk(KERN_INFO "Camera: " fmt "\n" , ## arg )
#else
#define CMSG(fmt, arg...) do {} while (0)
#endif

#if ERROR_DEBUG
#define EMSG(fmt, arg...) printk(KERN_INFO "Camera: " fmt "\n" , ## arg )
#else
#define EMSG(fmt, arg...) do {} while (0)
#endif

/* 101020 Do. layer1 for division shot */

#define FOCUS_WIN_WIDTH	    0xFF
#define FOCUS_WIN_HEIGHT	0xC0

#define NEG_X_AXIS -100
#define POS_X_AXIS 100
#define NEG_Y_AXIS -100
#define POS_Y_AXIS 100

#define WEIGHT_R0 0XC010
#define WEIGHT_R1 0X3008
#define WEIGHT_R3 0X0302
#define WEIGHT_R2 0X0C04

#define AF_SUCCESS 1
#define AF_FAIL 0
#define AF_FAIL_RETURN  -(0xFFFF)

#define DIVISION_SHOT_IMPLEMENTED 1
#define DIVISION_SHOT_NOT_IMPLEMENTED 0



struct yace4b1sbdbs_work {
	struct work_struct work;
};
//100622 Do. layer1
struct vregs
{
struct vreg *vreg_wlan2;	/*wlan2 cam_af 2.8v*/
struct vreg *vreg_xo_out; /* lvsw0 cam_a 2.8v */
struct vreg *vreg_lvsw0;  /* lvsw0 cam_io 1.8v */
};
static struct vregs vgs;

static struct  yace4b1sbdbs_work *yace4b1sbdbs_sensorw;
static struct  i2c_client *yace4b1sbdbs_client;


static DECLARE_WAIT_QUEUE_HEAD(yace4b1sbdbs_wait_queue);
DECLARE_MUTEX(yace4b1sbdbs_sem);


/*=============================================================
	EXTERNAL DECLARATIONS
==============================================================*/
extern struct yace4b1sbdbs_reg yace4b1sbdbs_regs;
extern int32_t is_division_shot;
/*=============================================================*/

static int32_t yace4b1sbdbs_i2c_txdata(unsigned short saddr,
	unsigned char *txdata, int length)
{
	struct i2c_msg msg[] = {
		{
			.addr = saddr,
			.flags = 0,
			.len = length,
			.buf = txdata,
		},
	};

#if SENSOR_DEBUG
	if (length == 2)
		CDBG("msm_io_i2c_w: 0x%04x 0x%04x\n",
			*(u16 *) txdata, *(u16 *) (txdata + 2));
	else if (length == 4)
	{
		CDBG("buf[0] is 0x%x\n", msg[0].buf[0]);
		CDBG("buf[1] is 0x%x\n", msg[0].buf[1]);
		CDBG("buf[2] is 0x%x\n", msg[0].buf[2]);
		CDBG("buf[3] is 0x%x\n", msg[0].buf[3]);
		CDBG("msm_io_i2c_w: 0x%04x\n", *(u16 *) txdata);
	}
	else
		CDBG("msm_io_i2c_w: length = %d\n", length);
#endif
	if (i2c_transfer(yace4b1sbdbs_client->adapter, msg, 1) < 0) {
		CDBG("yace4b1sbdbs_i2c_txdata failed\n");
		return -EIO;
	}

	return 0;
}

static int32_t yace4b1sbdbs_i2c_write(unsigned short saddr,
	unsigned short waddr, unsigned short wdata, enum yace4b1sbdbs_width width)
{
	int32_t rc = -EIO;
	unsigned char buf[4];
#if SENSOR_DEBUG
	CDBG("waddr is 0x%x\n ",waddr);
	CDBG("wdata is 0x%x\n ",wdata);
#endif
	memset(buf, 0, sizeof(buf));
	switch (width) {
	case WORD_LEN: {
		buf[0] = (waddr & 0xFF00)>>8;
		buf[1] = (waddr & 0x00FF);
		buf[2] = (wdata & 0xFF00)>>8;
		buf[3] = (wdata & 0x00FF);

		rc = yace4b1sbdbs_i2c_txdata(saddr, buf, 4);
	}
		break;

	case BYTE_LEN: {
		buf[0] = waddr;
		buf[1] = wdata;
		rc = yace4b1sbdbs_i2c_txdata(saddr, buf, 2);
	}
		break;

	default:
		break;
	}

	if (rc < 0)
		CDBG(
		"i2c_write failed, addr = 0x%x, val = 0x%x!\n",
		waddr, wdata);
	return rc;
}


static int yace4b1sbdbs_i2c_rxdata(unsigned short saddr,
	unsigned char *rxdata, int length)
{
	struct i2c_msg msgs[] = {
	{
		.addr   = saddr,
		.flags = 0,
		.len   = 1,
		.buf   = rxdata,
	},
	{
		.addr   = saddr,
		.flags = I2C_M_RD,
		.len   = length,
		.buf   = rxdata,
	},
	};

#if SENSOR_DEBUG
	if (length == 2)
		CDBG("msm_io_i2c_r: 0x%04x 0x%04x\n",
			*(u16 *) rxdata, *(u16 *) (rxdata + 2));
	else if (length == 4)
		CDBG("msm_io_i2c_r: 0x%04x\n", *(u16 *) rxdata);
	else
		CDBG("msm_io_i2c_r: length = %d\n", length);
#endif

	if (i2c_transfer(yace4b1sbdbs_client->adapter, msgs, 2) < 0) {
		CDBG("yace4b1sbdbs_i2c_rxdata failed!\n");
		return -EIO;
	}

	return 0;
}


static int32_t yace4b1sbdbs_i2c_read(unsigned short   saddr,
	unsigned short raddr, unsigned short *rdata, enum yace4b1sbdbs_width width)
{
	int32_t rc = 0;
	unsigned char buf[4];

	if (!rdata)
		return -EIO;

	memset(buf, 0, sizeof(buf));

	switch (width) {
	case WORD_LEN: {
		buf[0] = (raddr & 0xFF00)>>8;
		buf[1] = (raddr & 0x00FF);

		rc = yace4b1sbdbs_i2c_rxdata(saddr, buf, 2);
		if (rc < 0)
			return rc;

		*rdata = buf[0] << 8 | buf[1];
	}
		break;
	case BYTE_LEN:{
		buf[0] = (raddr & 0x00FF);
		rc = yace4b1sbdbs_i2c_rxdata(saddr, buf, 1);

		if (rc < 0)
			return rc;

		*rdata = buf[0] ;
		}
		break;
	default:
		break;
	}

	if (rc < 0)
		CDBG("yace4b1sbdbs_i2c_read failed!\n");

	return rc;
}


static long yace4b1sbdbs_reg_init(void)
{
// 100721 Do. layer1
	int32_t N;
	long rc = 0 ;
CMSG("init time start %lu",jiffies);
	/* Configure sensor for initiate camera */
	N = yace4b1sbdbs_regs.initial_sensor_mode_reg_settings_size;
	rc = yace4b1sbdbs_i2c_write_loop(N, yace4b1sbdbs_regs.init_reg_settings);
CMSG("init time end %lu",jiffies);	
	return rc;
}

static long yace4b1sbdbs_reg_mode(int mode)
{
 // 100721 Do. layer1
	int32_t N;
	int32_t rc = 0;

	switch(mode)
	{
		case SENSOR_PREVIEW_MODE :
			{
			 	CMSG("preview in\n");
				N = yace4b1sbdbs_regs.preview_mode_contextA_size;
				rc = yace4b1sbdbs_i2c_write_loop(N, yace4b1sbdbs_regs.preview_reg_settings);
			}
		break;
		case SENSOR_SNAPSHOT_MODE :
		{
				CMSG("snapshot in\n");
				N = yace4b1sbdbs_regs.snapshot_mode_contextB_size_1;
				rc = yace4b1sbdbs_i2c_write_loop(N, yace4b1sbdbs_regs.snapshot_reg_settings_1);
		}
		break;
	}
	return rc;
}

#ifdef  F_SKYCAM_FIX_CFG_WB
 long yace4b1sbdbs_set_whitebalance(int mode, int wb)
{
	

	long rc = 0;

	uint32_t N;

	switch (wb) {
	case CAMERA_WB_AUTO: {
			CMSG("wb auto in\n");

			N = yace4b1sbdbs_regs.whitebalance_auto_setting_size;
			rc = yace4b1sbdbs_i2c_write_loop(N, yace4b1sbdbs_regs.whitebalance_auto_setting);

			}
		break;

	case CAMERA_WB_DAYLIGHT: {
			CMSG("wb daylight in\n");

			N = yace4b1sbdbs_regs.whitebalance_daylight_setting_size;
			rc = yace4b1sbdbs_i2c_write_loop(N, yace4b1sbdbs_regs.whitebalance_daylight_setting);

			}
		break;

	case CAMERA_WB_CLOUDY_DAYLIGHT: {
			CMSG("wb cloudy in\n");

			N = yace4b1sbdbs_regs.whitebalance_cloudy_setting_size;
			rc = yace4b1sbdbs_i2c_write_loop(N, yace4b1sbdbs_regs.whitebalance_cloudy_setting);


			}
		break;

	case CAMERA_WB_INCANDESCENT: {
			CMSG("wb tungsten in\n");

			N = yace4b1sbdbs_regs.whitebalance_tungsten_setting_size;
			rc = yace4b1sbdbs_i2c_write_loop(N, yace4b1sbdbs_regs.whitebalance_tungsten_setting);	

			}
		break;

	case CAMERA_WB_FLUORESCENT: {
			CMSG("wb flourescent in\n");

			N = yace4b1sbdbs_regs.whitebalance_flourescent_setting_size;
			rc = yace4b1sbdbs_i2c_write_loop(N, yace4b1sbdbs_regs.whitebalance_flourescent_setting);	

	}
		break;

	default: {

		return -EINVAL;
	}
	}

	return rc;
	}
#endif
 long yace4b1sbdbs_set_effect(int mode, int effect)
{
	uint32_t N;

	long rc = 0;
	CMSG("filter  in %d", effect);	

	switch (effect) {
	case CAMERA_EFFECT_OFF: {
			CMSG("filter normal in\n");
	
			N = yace4b1sbdbs_regs.filter_normal_setting_size;
			rc = yace4b1sbdbs_i2c_write_loop(N, yace4b1sbdbs_regs.filter_normal_setting);

	}
			break;

	case CAMERA_EFFECT_MONO: {
			CMSG("filter mono in\n");

			N = yace4b1sbdbs_regs.filter_mono_setting_size;
			rc = yace4b1sbdbs_i2c_write_loop(N, yace4b1sbdbs_regs.filter_mono_setting);

	}
		break;

	case CAMERA_EFFECT_NEGATIVE: {
			CMSG("filter negative in\n");
	
			N = yace4b1sbdbs_regs.filter_negative_setting_size;
			rc = yace4b1sbdbs_i2c_write_loop(N, yace4b1sbdbs_regs.filter_negative_setting);

		
	}
		break;

	case CAMERA_EFFECT_SEPIA: {
			CMSG("filter sepia in\n");

			N = yace4b1sbdbs_regs.filter_sepia_setting_size;
			rc = yace4b1sbdbs_i2c_write_loop(N, yace4b1sbdbs_regs.filter_sepia_setting);

	}
		break;
	case CAMERA_EFFECT_SOLARIZE: {

	}
		break;

	

	default: {

		return -EINVAL;
	}
	}
			return rc;
	}

 long yace4b1sbdbs_set_brightness(int bright)
{
	

	long rc = 0;

	uint32_t N;

	switch (bright) {

	case CAMERA_BRIGHTNESS_POSITIVE_4: {
			CMSG("brightness +4 in");
			N = yace4b1sbdbs_regs.brightness_positive_4_setting_size;
			rc = yace4b1sbdbs_i2c_write_loop(N, yace4b1sbdbs_regs.brightness_positive_4_setting);
				}
		break;

	case CAMERA_BRIGHTNESS_POSITIVE_3: {
			CMSG("brightness +3 in");
			N = yace4b1sbdbs_regs.brightness_positive_3_setting_size;
			rc = yace4b1sbdbs_i2c_write_loop(N, yace4b1sbdbs_regs.brightness_positive_3_setting);
				}
	break;

	case CAMERA_BRIGHTNESS_POSITIVE_2: {
			CMSG("brightness +2 in");
			N = yace4b1sbdbs_regs.brightness_positive_2_setting_size;
			rc = yace4b1sbdbs_i2c_write_loop(N, yace4b1sbdbs_regs.brightness_positive_2_setting);
				}
	break;

	case CAMERA_BRIGHTNESS_POSITIVE_1: {
			CMSG("brightness +1 in");
			N = yace4b1sbdbs_regs.brightness_positive_1_setting_size;
			rc = yace4b1sbdbs_i2c_write_loop(N, yace4b1sbdbs_regs.brightness_positive_1_setting);
	}
	break;

	case CAMERA_BRIGHTNESS_NORMAL_0: {
			CMSG("brightness 0 in");
			N = yace4b1sbdbs_regs.brightness_normal_0_setting_size;
			rc = yace4b1sbdbs_i2c_write_loop(N, yace4b1sbdbs_regs.brightness_normal_0_setting);
	}
	break;

	case CAMERA_BRIGHTNESS_NEGATIVE_1: {
			CMSG("brightness -1 in");
			N = yace4b1sbdbs_regs.brightness_negative_1_setting_size;
			rc = yace4b1sbdbs_i2c_write_loop(N, yace4b1sbdbs_regs.brightness_negative_1_setting);
				}
	break;

	case CAMERA_BRIGHTNESS_NEGATIVE_2: {
			CMSG("brightness -2 in");
			N = yace4b1sbdbs_regs.brightness_negative_2_setting_size;
			rc = yace4b1sbdbs_i2c_write_loop(N, yace4b1sbdbs_regs.brightness_negative_2_setting);
				}
	break;

	case CAMERA_BRIGHTNESS_NEGATIVE_3: {
			CMSG("brightness -3 in");
			N = yace4b1sbdbs_regs.brightness_negative_3_setting_size;
			rc = yace4b1sbdbs_i2c_write_loop(N, yace4b1sbdbs_regs.brightness_negative_3_setting);
				}
	break;

	case CAMERA_BRIGHTNESS_NEGATIVE_4: {
			CMSG("brightness -4 in");
			N = yace4b1sbdbs_regs.brightness_negative_4_setting_size;
			rc = yace4b1sbdbs_i2c_write_loop(N, yace4b1sbdbs_regs.brightness_negative_4_setting);
				}
	break;
		default:
		return -EINVAL;
	}

	return rc;
}

 long yace4b1sbdbs_set_focus_step(int mode, int32_t focus_step)
{
	
	long rc = 0;

	

	uint32_t N;
	 CMSG("yace4b1sbdbs_set_focus_step function  in");
	CMSG("focus step is %d", focus_step);
	switch(focus_step)
		{
		#if 0
		  	case CAMERA_AF :
				{
			
			N = yace4b1sbdbs_regs.focus_off_setting_size;
			yace4b1sbdbs_i2c_write_loop(N, yace4b1sbdbs_regs.focus_off_setting);
		
			
				}
			break;
		#endif
			case C_SKYCAM_MAX_FOCUS_STEP :
					{

			N = yace4b1sbdbs_regs.macro_focus_on_setting_size;
			rc = yace4b1sbdbs_i2c_write_loop(N, yace4b1sbdbs_regs.macro_focus_on_setting);

					}
			break;
			
			case C_SKYCAM_MIN_FOCUS_STEP :
					{
				N = yace4b1sbdbs_regs.infinity_focus_on_setting_size;
				rc = yace4b1sbdbs_i2c_write_loop(N, yace4b1sbdbs_regs.infinity_focus_on_setting);
					}
			break;
			default:
		return -EINVAL;
		}

	return rc;
}

/*===========================================================================
 * FUNCTION    - yace4b1sbdbs_set_focus_rect -
 *
 * DESCRIPTION:
 * it's for division shot with spot focus
 * the display size is 430X320
 * the resolution of preview is 1024X768
 * the AF window full size is [0xFF*2] X [0xC0*2]
 * in this function, the AF window is divides by 6X6 block.
  -------------------
 |	     	|		 |
 |	R0		|	R1	 |
 |			|		 |
 ---------------------
 |			|		 |
 |  R2		|	R3	 |
 |			|		 |
 ---------------------
 <  each AF weight area of AF wiondow >

 *==========================================================================*/
/* 101020 Do. layer1 for division shot */
int32_t yace4b1sbdbs_set_focus_rect(uint32_t focus_rect)
{

	uint32_t xleft=0, xright=0, ytop=0, ybottom=0;
	uint32_t af_win_xlf=0, af_win_xrt=0, af_win_ytp=0, af_win_ybt=0;
	uint8_t afrow2=0, afrow3=0, afcol2=0, afcol3=0;
	int32_t rc = 0;
	int32_t N=0;
	int32_t x_axis = 0;
	int32_t y_axis = 0;
	int32_t weight = 0;


CMSG("set_focus_rect function in focus_rect is %d", focus_rect);


/* extract each coordinate value from the focus_rect value.*/
	xleft   = (uint8_t)((focus_rect & 0x0f000000) >> 24);
	xright  = (uint8_t)((focus_rect & 0x000f0000) >> 16);
	ytop    = (uint8_t)((focus_rect & 0x00000f00) >> 8);
	ybottom = (uint8_t)((focus_rect & 0x0000000f));
CMSG("xleft = %d, xright = %d, ytop = %d, ybottom = %d", xleft, xright, ytop, ybottom);;

// if the value is located on a boundary line, adjust it to focus on each weight area.
	if ( ( xleft == 5 ) && ( xright ==7 ) ) xright = 6;
	if ( ( ytop == 5 ) && ( ybottom ==7 ) ) ybottom = 6;

/*if  each coordinate value is zero, set sensor to normal focusing.*/
	if ( ( (xleft == 0) && (xright == 0) && (ytop == 0) && (ybottom == 0))  || ((xleft == 5) && (xright == 6) && (ytop == 5) && (ybottom == 6))  )
	{
			N = yace4b1sbdbs_regs.division_normal_condition_setting_size;
			rc = yace4b1sbdbs_i2c_write_loop(N, yace4b1sbdbs_regs.division_normal_condition_setting);
			is_division_shot = DIVISION_SHOT_NOT_IMPLEMENTED;
	}
	else
	{
	
/* each coordinate value will be changed so that sensor can set the value with  sensor's focus window */
	af_win_xlf = xleft * (FOCUS_WIN_WIDTH*2)/12;
	af_win_xrt = xright *( FOCUS_WIN_WIDTH*2)/12;
	af_win_ytp = ytop  * (FOCUS_WIN_HEIGHT*2)/12;
	af_win_ybt = ybottom * (FOCUS_WIN_HEIGHT*2)/12;

/* from each changed coordinate value, get the each axis value. */
	if ( af_win_xlf < 255 ) x_axis = NEG_X_AXIS;
	else
		{
		x_axis = POS_X_AXIS;
		}
	if( af_win_ytp  < 192 ) y_axis = POS_Y_AXIS;
	else 
		{
		y_axis = NEG_Y_AXIS;
		}


	
	CMSG("%s  xleft = %d, ytop = %d, xright = %d, ybottom = %d\n", __func__, af_win_xlf, af_win_ytp, af_win_xrt, af_win_ybt);
	CMSG("%s  x_axis = %d, y_axis = %d\n", __func__, x_axis, y_axis);

	/* the below is the specific algorithm of the sensor's spot focusing */
	if ( x_axis == NEG_X_AXIS && y_axis == POS_Y_AXIS)
		{
			weight = WEIGHT_R0;
			afcol2 = FOCUS_WIN_WIDTH - af_win_xrt;
			afcol3 = FOCUS_WIN_WIDTH - af_win_xlf;
			afrow2 = FOCUS_WIN_HEIGHT - af_win_ybt;
			afrow3 = FOCUS_WIN_HEIGHT - af_win_ytp;
			}
	else if(x_axis == POS_X_AXIS && y_axis == POS_Y_AXIS)
		{
			weight = WEIGHT_R1;
			afcol2 = af_win_xlf - FOCUS_WIN_WIDTH;
			afcol3 = af_win_xrt - FOCUS_WIN_WIDTH;
			afrow2 = FOCUS_WIN_HEIGHT - af_win_ybt;
			afrow3 = FOCUS_WIN_HEIGHT - af_win_ytp;
		}
	else if(x_axis == POS_X_AXIS && y_axis == NEG_Y_AXIS)
		{
			weight = WEIGHT_R3;
			afcol2 = af_win_xlf - FOCUS_WIN_WIDTH;
			afcol3 = af_win_xrt - FOCUS_WIN_WIDTH;
			afrow2 = af_win_ytp - FOCUS_WIN_HEIGHT;
			afrow3 = af_win_ybt - FOCUS_WIN_HEIGHT;
		}
	else if(x_axis == NEG_X_AXIS && y_axis == NEG_Y_AXIS)
		{
			weight = WEIGHT_R2;
			afcol2 = FOCUS_WIN_WIDTH - af_win_xrt;
			afcol3 = FOCUS_WIN_WIDTH - af_win_xlf;
			afrow2 = af_win_ytp - FOCUS_WIN_HEIGHT;
			afrow3 = af_win_ybt - FOCUS_WIN_HEIGHT;
		}

		if ( (( xleft == 5 ) && ( xright == 6 )) && !(( ytop == 5) && (ybottom == 6)) )
			{
				weight = ybottom <= 6 ? (WEIGHT_R0 | WEIGHT_R1) : (WEIGHT_R2 | WEIGHT_R3) ;
			}	
		else if  ( (( ytop == 5 ) && ( ybottom == 6 )) && !(( xleft  == 5) && (xright == 6)) )
			{
						weight = xright <= 6 ? (WEIGHT_R0 | WEIGHT_R2) : (WEIGHT_R1 | WEIGHT_R3);
			}	
	
		
		CMSG("%s  afcol3 = %d, afcol2 = %d afrow3 = %d afrow2 = %d\n weight = %d\n", __func__, afcol3, afcol2, afrow3, afrow2 , weight);
		rc = yace4b1sbdbs_write_focus_rect(afcol3,afcol2, afrow3, afrow2 , weight);
		is_division_shot = DIVISION_SHOT_IMPLEMENTED;
		}
	
	CMSG("%s is done.\n", __func__); 

	return rc;
}
int32_t  yace4b1sbdbs_write_focus_rect(uint8_t afcol_3, uint8_t afcol_2 , uint8_t afrow_3, uint8_t afrow_2, int16_t weight)
{

int32_t rc=0;	

rc  = yace4b1sbdbs_i2c_write(yace4b1sbdbs_client->addr,
					0x03,
					0x24,
					BYTE_LEN);
	if (rc < 0)
			{
			EMSG("function:  %s line : %d i2c_write ailed!\n", __func__, __LINE__);
			goto 	WRITE_FAIL;
			}

	yace4b1sbdbs_i2c_write(yace4b1sbdbs_client->addr,
					0x10,
					0x65,
					BYTE_LEN);
	if (rc < 0)
			{
			EMSG("function:  %s line : %d i2c_write ailed!\n", __func__, __LINE__);
			goto 	WRITE_FAIL;
			}

	yace4b1sbdbs_i2c_write(yace4b1sbdbs_client->addr,
					0x20,
					0x00,
					BYTE_LEN);
		if (rc < 0)
			{
			EMSG("function:  %s line : %d i2c_write ailed!\n", __func__, __LINE__);
			goto 	WRITE_FAIL;
			}
		
	yace4b1sbdbs_i2c_write(yace4b1sbdbs_client->addr,
					0x21,
					afrow_2,
					BYTE_LEN);
		if (rc < 0)
			{
			EMSG("function:  %s line : %d i2c_write ailed!\n", __func__, __LINE__);
			goto 	WRITE_FAIL;
			}
		
	yace4b1sbdbs_i2c_write(yace4b1sbdbs_client->addr,
					0x22,
					afrow_3,
					BYTE_LEN);
		if (rc < 0)
			{
			EMSG("function:  %s line : %d i2c_write ailed!\n", __func__, __LINE__);
			goto 	WRITE_FAIL;
			}
		

	yace4b1sbdbs_i2c_write(yace4b1sbdbs_client->addr,
					0x24,
					0x00,
					BYTE_LEN);
		if (rc < 0)
			{
			EMSG("function:  %s line : %d i2c_write ailed!\n", __func__, __LINE__);
			goto 	WRITE_FAIL;
			}
		
	yace4b1sbdbs_i2c_write(yace4b1sbdbs_client->addr,
					0x25,
					afcol_2,
					BYTE_LEN);
		if (rc < 0)
			{
			EMSG("function:  %s line : %d i2c_write ailed!\n", __func__, __LINE__);
			goto 	WRITE_FAIL;
			}
		
	yace4b1sbdbs_i2c_write(yace4b1sbdbs_client->addr,
					0x26,
					afcol_3,
					BYTE_LEN);
	if (rc < 0)
			{
			EMSG("function:  %s line : %d i2c_write ailed!\n", __func__, __LINE__);
			goto 	WRITE_FAIL;
			}

	yace4b1sbdbs_i2c_write(yace4b1sbdbs_client->addr,
					0x03,
					0x60,
					BYTE_LEN);
		if (rc < 0)
				{
				EMSG("function:  %s line : %d i2c_write ailed!\n", __func__, __LINE__);
				goto	WRITE_FAIL;
				}
		
	yace4b1sbdbs_i2c_write(yace4b1sbdbs_client->addr,
					0x13,
					weight >> 8,
					BYTE_LEN);
		if (rc < 0)
			{
			EMSG("function:  %s line : %d i2c_write ailed!\n", __func__, __LINE__);
			goto 	WRITE_FAIL;
			}
		
	yace4b1sbdbs_i2c_write(yace4b1sbdbs_client->addr,
					0x14,
					weight & 0xFF,
					BYTE_LEN);
		if (rc < 0)
			{
			EMSG("function:  %s line : %d i2c_write ailed!\n", __func__, __LINE__);
			goto 	WRITE_FAIL;
			}

return rc;

WRITE_FAIL:
rc = -EINVAL;
return rc;

}	

/* 101020 Do. layer1 */

 long yace4b1sbdbs_set_preview_fps(uint32_t  fps)
{

extern struct  fps_arr fps_array[13];

uint32_t N;
uint32_t fps_array_order;
long rc = 0;

CMSG ("fps in kernel is %d",fps);	

if ( fps == C_SKYCAM_VARIABLE_FPS  )
{
		/* variable fps 8~22.62fps */
								CMSG("camera preview normal fps");
								N =fps_array[0].size;
								rc = yace4b1sbdbs_i2c_write_loop(N,fps_array[0].value_set);
								
}
else if( fps >= C_SKYCAM_MIN_PREVIEW_FPS && fps <= C_SKYCAM_MAX_PREVIEW_FPS  )
{
	/* fixed fps 8~19.6fps */
	fps = fps>C_SKYCAM_MAX_SUPPORT_FPS ? C_SKYCAM_MAX_SUPPORT_FPS : fps;
	
	fps_array_order = fps - 7 ;
									N =  fps_array[fps_array_order].size;
							rc = yace4b1sbdbs_i2c_write_loop(N, fps_array[fps_array_order].value_set);	
}
else
{
	EMSG(" %s:%d : %d fps value is abnomal ", __func__, __LINE__, fps);
	return -EINVAL;
}

	/* 5ms delay is needed after changing fps */
	msleep(5);

	return rc;
	}

 long yace4b1sbdbs_move_focus_mode(int mode, int8_t autofocus)
{

	long rc = 0;


	uint32_t N;

	uint16_t af_done_value = 0;
	uint16_t af_success_value = 0;
	uint16_t is_af_success = 0; /* 101029 Do. layer1. flag for AF success */
	unsigned long exposure_time = 0;
	uint32_t delay_time = 0;
	unsigned short temp = 0;
	unsigned long endtime;

      CMSG("AF in");
	CMSG("af mode is %d", autofocus);

	switch(autofocus)
	{
	case CAMERA_AF :
			{
		/* AF initialize */
			N = yace4b1sbdbs_regs.focus_off_setting_size;
			rc = yace4b1sbdbs_i2c_write_loop(N, yace4b1sbdbs_regs.focus_off_setting);
		if (rc < 0 )
					{
					EMSG("function:  %s line : %d i2c write : AF init fail!\n", __func__, __LINE__);
					goto 	WRITE_FAIL;
					}
		/*
		the below is the sequence of auto focus.  
		*/
	
	/* 101020 Do. layer1.  read exposure time and define frame rate to set delay time  */
	yace4b1sbdbs_i2c_write(yace4b1sbdbs_client->addr,
								0x03,
								0x20,
								BYTE_LEN);
	yace4b1sbdbs_i2c_read(yace4b1sbdbs_client->addr,
					0x80, &temp, BYTE_LEN);
	exposure_time = temp << 16;
	yace4b1sbdbs_i2c_read(yace4b1sbdbs_client->addr,
					0x81, &temp, BYTE_LEN);
	exposure_time = exposure_time | (temp << 8);
	yace4b1sbdbs_i2c_read(yace4b1sbdbs_client->addr,
					0x82, &temp, BYTE_LEN);
	exposure_time = exposure_time | temp ;

	if ( exposure_time >= 0x000138 && exposure_time <=0x01e780) delay_time = 700 ;  /* 22.62 fps , 700ms */
	else if ( exposure_time >= 0x01e77f && exposure_time <=0x030c00) delay_time = 1200 ;  /* 19.04 ~ 14.46 fps , 1200ms */
	else if ( exposure_time >= 0x030bff && exposure_time <=0x05b680 )  delay_time = 2200 ;  /* 12.91~8fps  , 2200ms */
	
	/* AF run */
			N = yace4b1sbdbs_regs.auto_focus_on_setting_size;
			rc = yace4b1sbdbs_i2c_write_loop(N, yace4b1sbdbs_regs.auto_focus_on_setting);
	msleep(delay_time);
			if (rc < 0 )
					{
					EMSG("function:  %s line : %d i2c write : AF run FAIL!\n", __func__, __LINE__);
					goto 	WRITE_FAIL;
					}
				
				endtime =  jiffies+ 4000*HZ/1000;				
				CMSG("endtime is %lu and before jiffies = %lu HZ = %d",endtime,  jiffies, HZ);
				do{
				// read the valueof whether focus is done or not
				yace4b1sbdbs_i2c_write(yace4b1sbdbs_client->addr,
								0x03,
								0x60,
								BYTE_LEN);
				yace4b1sbdbs_i2c_read(yace4b1sbdbs_client->addr,
					0x39, &af_done_value, BYTE_LEN);
				
			// check the value of success and time delay of 4s
				}while(af_done_value != 0x0C &&   jiffies < endtime);
				CMSG("after jiffies = %lu, af_done_value = 0x%x, HZ = %d", jiffies, af_done_value, HZ);

			// read the value of af_success
			
	
		yace4b1sbdbs_i2c_write(yace4b1sbdbs_client->addr,
							0x03,
							0x60,
							BYTE_LEN);
			yace4b1sbdbs_i2c_read(yace4b1sbdbs_client->addr,
							0x35, &temp, BYTE_LEN);
						af_success_value = temp<<8;

						yace4b1sbdbs_i2c_write(yace4b1sbdbs_client->addr,
										0x03,
										0x60,
										BYTE_LEN);
						 yace4b1sbdbs_i2c_read(yace4b1sbdbs_client->addr,
							0x36, &temp, BYTE_LEN);  
						af_success_value = af_success_value | temp ;

						CMSG(" af_success_value = 0x%x",  af_success_value);
	
	
			// if focusing is failed, then the sensor run infinity focus mode.

				if ( is_division_shot ) 
					{
						is_af_success = af_success_value >= 0X45 ? AF_SUCCESS : AF_FAIL;
					}
				else
					{
						is_af_success = af_success_value >= 0X78 ? AF_SUCCESS : AF_FAIL;
					}

				if (!is_af_success)
				{
			      
				N = yace4b1sbdbs_regs.infinity_focus_on_setting_size;
				
				rc = yace4b1sbdbs_i2c_write_loop(N, yace4b1sbdbs_regs.infinity_focus_on_setting);
				if (rc < 0 )
					{
					EMSG("function:  %s line : %d i2c write : move focus infinity fail!\n", __func__, __LINE__);
					goto 	WRITE_FAIL;
					}
			
				rc = -AF_FAIL_RETURN; //( return value of fail)
					}
						
				// 1frame skip for AF complete, 125ms time delay (Minimun) is needed to be completed for focusing
				msleep(200);
				}
		break;
case CAMERA_INFINITY :
					{
					
				//	nothing to do

		}
					break;
case CAMERA_MACRO : 
					{
					
				//	nothing to do
	

					}
					break;
	}



	return rc;
	
WRITE_FAIL:

return rc;
}
 long yace4b1sbdbs_set_sensor_mode(int mode)
{
	long rc = 0;

	switch (mode) {
	case SENSOR_PREVIEW_MODE:
	{	
		rc = yace4b1sbdbs_reg_mode(SENSOR_PREVIEW_MODE);
		if(rc<0)
		return rc;
	}
		break;

	case SENSOR_SNAPSHOT_MODE:
	{	/* Switch to lower fps for Snapshot */
		rc = yace4b1sbdbs_reg_mode(SENSOR_SNAPSHOT_MODE);
		if(rc<0)
		return rc;
	}
		break;

	case SENSOR_RAW_SNAPSHOT_MODE:
	{	/* Setting the effect to CAMERA_EFFECT_OFF */
		if(rc<0)
		return rc;
	}       
		break;

	default:
		return -EINVAL;
	}

	return rc;
}

 int yace4b1sbdbs_sensor_init_probe(void)
{
// configure gpio and vreg for power-up
 int32_t rc = 0;

 vgs.vreg_wlan2 = NULL;	
 vgs.vreg_xo_out = NULL;
 vgs.vreg_lvsw0 = NULL; 




// gpio config
rc = gpio_request(YACE4B1SBDBS_CAM_RESET_GPIO, "sensor reset");
if(rc<0)
{
	CDBG("gpio sensor reset request fail\n");
	return rc;
}

rc = gpio_request(YACE4B1SBDBS_CAM_STANDBY_GPIO, "sensor standby");
if(rc<0)
{
	CDBG("gpio sensor stanby request fail\n");
	return rc;
}
// make sure if gpio is low
rc = gpio_direction_output(YACE4B1SBDBS_CAM_RESET_GPIO, 0);
if( rc < 0 )
{
	CDBG("gpio sensor reset low fail\n");
	return rc;
}

rc = gpio_direction_output(YACE4B1SBDBS_CAM_STANDBY_GPIO, 0);
if( rc < 0 )
{
	CDBG("gpio sensor standby low fail\n");
	return rc;
}

// vreg config
vgs.vreg_wlan2 = vreg_get(NULL, "wlan2");
rc = IS_ERR(vgs.vreg_wlan2);
if(rc<0)
{
	CDBG("vreg_wlan2 get failed \n");
	return rc;
}

vgs.vreg_xo_out = vreg_get(NULL, "xo_out");
rc = IS_ERR(vgs.vreg_xo_out);
if(rc<0)
{
	CDBG("vreg_xo_out get failed \n");
	return rc;
}

vgs.vreg_lvsw0= vreg_get(NULL, "lvsw0");
rc = IS_ERR(vgs.vreg_lvsw0);
if(rc<0)
{
	CDBG("vreg_gp3 get failed \n");
	return rc;
}


// vreg set level


rc = vreg_set_level(vgs.vreg_wlan2, 2800);
if(rc<0)
{
	CDBG("vreg_wlan2 set level failed \n");
	return rc;
}

rc = vreg_set_level(vgs.vreg_xo_out, 2800);
if(rc<0)
{
	CDBG("vreg_xo_out set level failed \n");
	return rc;
}

rc = vreg_set_level(vgs.vreg_lvsw0, 1800);
if(rc<0)
{
	CDBG("vreg_lvsw0 set level failed \n");
	return rc;
}

/* make sure vreg power down */

		// 5. 2.8V_CAM_A down

	rc = vreg_disable(vgs.vreg_xo_out);
	if(rc<0)
	{
	CDBG("vreg_xo_out disable is failed\n");
		return rc;
	}	
		udelay(100);

		// 6. 1.8V_CAM_IO down

	rc = vreg_disable(vgs.vreg_lvsw0);
	if(rc<0)
	{
	CDBG("vreg vreg_lvsw0  set disable is failed\n");
		return rc;
	}	

		udelay(100);
		
		// 7. AF power down	

	rc = vreg_disable(vgs.vreg_wlan2);
	if(rc<0)
	{
	CDBG("vreg vreg_wlan2  disable is failed\n");
		return rc;
	}




	return rc;
}

int yace4b1sbdbs_sensor_init(const struct msm_camera_sensor_info *data)
{
	int rc = 0;

	msm_camio_camif_pad_reg_reset();
	//power up sensor
	yace4b1sbdbs_power_up();
	if(rc<0)
	{
	CDBG("yace4b1sbdbs_power_up failed!\n");
	return rc;
	}
	//initiate sensor
	yace4b1sbdbs_reg_init();
	if(rc<0)
	{
	CDBG("yace4b1sbdbs_reg_init failed!\n");
	return rc;
	}

	CDBG("sensor init is done  \n");
	return rc;

}

static int yace4b1sbdbs_init_client(struct i2c_client *client)
{
	/* Initialize the MSM_CAMI2C Chip */
	init_waitqueue_head(&yace4b1sbdbs_wait_queue);
	return 0;
}

int yace4b1sbdbs_sensor_config(void __user *argp)
{
	struct sensor_cfg_data cfg_data;
	long   rc = 0;

	if (copy_from_user(&cfg_data,
			(void *)argp,
			sizeof(struct sensor_cfg_data)))
		return -EFAULT;

	/* down(&yace4b1sbdbs_sem); */

	CDBG("yace4b1sbdbs_ioctl, cfgtype = %d, mode = %d\n",
		cfg_data.cfgtype, cfg_data.mode);

		switch (cfg_data.cfgtype) {
		case CFG_SET_MODE:
			rc = yace4b1sbdbs_set_sensor_mode(
						cfg_data.mode);
			break;

		case CFG_SET_WB:
			rc = yace4b1sbdbs_set_whitebalance(SENSOR_PREVIEW_MODE,
						cfg_data.cfg.wb);
			break;

		case CFG_SET_EFFECT:
			rc = yace4b1sbdbs_set_effect(cfg_data.mode,
						cfg_data.cfg.effect);
			break;

		case CFG_SET_BRIGHTNESS:
			rc = yace4b1sbdbs_set_brightness(cfg_data.cfg.brightness);
			break;

		case CFG_AUTO_FOCUS:
			rc = yace4b1sbdbs_move_focus_mode(cfg_data.mode, cfg_data.cfg.focus.dir);
			break;
			/*100816 Do. layer1*/
		case CFG_SET_FOCUS_STEP :
			rc = yace4b1sbdbs_set_focus_step(cfg_data.mode, cfg_data.cfg.focus_step);
			break;
			/*101020 Do. layer1*/
         case CFG_SET_PREVIEW_FPS:
			rc = yace4b1sbdbs_set_preview_fps(cfg_data.cfg.preview_fps);
			break;		
		case CFG_SET_FOCUS_RECT:
			rc = yace4b1sbdbs_set_focus_rect(cfg_data.cfg.focus_rect);
			break;			
		default:
			rc = -EINVAL;
			break;
		}

	/* up(&yace4b1sbdbs_sem); */

	return rc;
}

int yace4b1sbdbs_sensor_release(void)
{
	int rc = 0;

	/* down(&yace4b1sbdbs_sem); */
	rc = yace4b1sbdbs_power_down();
	if(rc<0)
	CDBG("yace4b1sbdbs sensor power down fail\n");
/*the belows aren't needed because those are not allocated.*/
/*	kfree(yace4b1sbdbs_ctrl); */
	/* up(&yace4b1sbdbs_sem); */

	return rc;
}

static int yace4b1sbdbs_i2c_probe(struct i2c_client *client,
	const struct i2c_device_id *id)
{
	int rc = 0;
	if (!i2c_check_functionality(client->adapter, I2C_FUNC_I2C)) {
		rc = -ENOTSUPP;
		goto probe_failure;
	}

	yace4b1sbdbs_sensorw =
		(struct yace4b1sbdbs_work*)kzalloc(sizeof(struct yace4b1sbdbs_work), GFP_KERNEL);
	
	if (!yace4b1sbdbs_sensorw) {
		rc = -ENOMEM;
		goto probe_failure;
	}

	i2c_set_clientdata(client, yace4b1sbdbs_sensorw);
	yace4b1sbdbs_init_client(client);
	yace4b1sbdbs_client = client;

	CDBG("yace4b1sbdbs_probe succeeded!\n");

	return 0;

probe_failure:
	kfree(yace4b1sbdbs_sensorw);
	yace4b1sbdbs_sensorw = NULL;
	CDBG("yace4b1sbdbs_probe failed!\n");
	return rc;
}

static const struct i2c_device_id yace4b1sbdbs_i2c_id[] = {
	{ "yace4b1sbdbs", 0},
	{ },
};

static struct i2c_driver yace4b1sbdbs_i2c_driver = {
	.id_table = yace4b1sbdbs_i2c_id,
	.probe  = yace4b1sbdbs_i2c_probe,
	.remove = __exit_p(yace4b1sbdbs_i2c_remove),
	.driver = {
		.name = "yace4b1sbdbs",
	},
};

static int yace4b1sbdbs_sensor_probe(const struct msm_camera_sensor_info *info,
				struct msm_sensor_ctrl *s)
{
	int rc = i2c_add_driver(&yace4b1sbdbs_i2c_driver);
	if (rc < 0 || yace4b1sbdbs_client == NULL) {
		rc = -ENOTSUPP;
		goto probe_fail;
	}

	rc = yace4b1sbdbs_sensor_init_probe();
	if (rc < 0)
		goto probe_fail;
	
	s->s_init = yace4b1sbdbs_sensor_init;
	s->s_release = yace4b1sbdbs_sensor_release;
	s->s_config  = yace4b1sbdbs_sensor_config;
/*
	TO DO : add your API
*/

	return rc;

probe_fail:
	CDBG("yace4b1sbdbs probe fail\n %s %s:%d\n", __FILE__, __func__, __LINE__);
	return rc;
}

static int __yace4b1sbdbs_probe(struct platform_device *pdev)
{
	return msm_camera_drv_start(pdev, yace4b1sbdbs_sensor_probe);
}

//TODO add the function is defined in header file.

 int32_t yace4b1sbdbs_power_up(void)
{

	int32_t rc =0;

	
/*	the followings is  power up sequence of the yace4b1sbdbs 3M sensor. */
	// 1. 1.8V_CAM_IO on & 1.8V_CAM_D on
	
	rc = vreg_enable(vgs.vreg_lvsw0);
	if(rc<0)
	{
	CDBG("vreg lvsw0  set eable is failed\n");
		return rc;
	}

    udelay(100);
	
	// 2. 2.8V_CAM_A on

	rc = vreg_enable(vgs.vreg_xo_out);
	if(rc<0)
	{
	CDBG("vreg xo_out enable is failed\n");
		return rc;
	}


	 udelay(100);
	 
	// 4. standby on

	rc = gpio_direction_output(YACE4B1SBDBS_CAM_STANDBY_GPIO, 1);
	if( rc < 0 )
	{
		CDBG("gpio sensor standby high fail\n");
		return rc;
	}
	
	 udelay(100);
	 
	// 5. Mclk on
	
	msm_camio_clk_rate_set(24000000);
	
	

	msleep(10);

	// 6. CAM_RESET on

	rc = gpio_direction_output(YACE4B1SBDBS_CAM_RESET_GPIO, 1);
	if(rc<0)
	{
	CDBG("cam reset gpio high is failed\n");
		return rc;
	}	

	// delay 16,000,000 cycles = 1/24,000,000 * 16 = 0.66 us 

	udelay(1);
	
	// AF power on this has no relationship with power sequence.

	rc = vreg_enable(vgs.vreg_wlan2);
	if(rc<0)
	{
	CDBG("vreg gp1 enable is failed\n");
		return rc;
	}

	return rc;
}


 int32_t yace4b1sbdbs_power_down(void)
{
	int32_t rc = 0;
/*	the followings is  power down sequence of the yace4b1sbdbs 3M sensor. */
	//delay 20 cycles. 1/24,000,000 * 20 = 0.8 usec

	

		// 1. SCL, SDA set to low
	 rc = gpio_direction_output(YACE4B1SBDBS_CAM_SCL_GPIO, 0);
	if(rc<0)
	{
		CDBG("gpio sensor standby low fail\n");
		return rc;
	}
	
		rc = gpio_direction_output(YACE4B1SBDBS_CAM_SDA_GPIO, 0);
	if(rc<0)
	{
		CDBG("gpio sensor standby low fail\n");
		return rc;
	}

		
		udelay(1);

        // 2. CAM_RESET down.

	rc = gpio_direction_output(YACE4B1SBDBS_CAM_RESET_GPIO, 0);
	if(rc<0)
	{
	CDBG("cam reset gpio low is failed\n");
		return rc;
	}
        msleep(10);

		// 3. Mclk down.

	//msm_camio_clk_disable(CAMIO_CAM_MCLK_CLK);	

	
        // 4. standby off

	rc = gpio_direction_output(YACE4B1SBDBS_CAM_STANDBY_GPIO, 0);
	if(rc<0)
	{
		CDBG("gpio sensor standby low fail\n");
		return rc;
	}


        udelay(100);

		// 5. 2.8V_CAM_A down

	rc = vreg_disable(vgs.vreg_xo_out);
	if(rc<0)
	{
	CDBG("vreg_xo_out disable is failed\n");
		return rc;
	}	
		udelay(100);

		// 6. 1.8V_CAM_IO down

	rc = vreg_disable(vgs.vreg_lvsw0);
	if(rc<0)
	{
	CDBG("vreg vreg_lvsw0  set disable is failed\n");
		return rc;
	}	

	udelay(100);
		// 7. AF power down	

	rc = vreg_disable(vgs.vreg_wlan2);
	if(rc<0)
	{
	CDBG("vreg vreg_wlan2  disable is failed\n");
		return rc;
	}

  

 return rc;
}

 long yace4b1sbdbs_i2c_write_loop(int32_t i, const struct reg_addr_value_pair reg_pair[])
{			
	int32_t max;
	int32_t rc = 0;
	max = i ;
				do {

				 	rc = yace4b1sbdbs_i2c_write(yace4b1sbdbs_client->addr,
					reg_pair[max-i].register_address,
					reg_pair[max-i].register_value,
					BYTE_LEN);
				
					 if( reg_pair[max-i].delay_time != 0x00 ) /* if the value set has delay value, then set delay time after setting value. */
					{
					msleep(reg_pair[max-i].delay_time);
					//CMSG("function:  %s line : %d delay time %d \n", __func__, __LINE__, reg_pair[max-i].delay_time);
					}
				
					if (rc < 0)
					{
					EMSG("function:  %s line : %d i2c_write ailed!\n", __func__, __LINE__);
					goto 	WRITE_LOOP_FAIL;
					}
				
				i -= 1;
				}while ( i != 0);

return rc;

WRITE_LOOP_FAIL:
return rc;

}


static struct platform_driver msm_camera_driver = {
	.probe = __yace4b1sbdbs_probe,
	.driver = {
		.name = "msm_camera_yace4b1sbdbs",
		.owner = THIS_MODULE,
	},
};

static int __init yace4b1sbdbs_init(void)
{
	return platform_driver_register(&msm_camera_driver);
}

module_init(yace4b1sbdbs_init);
