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

#include <linux/slab.h>
#include <mach/vreg.h>

#include "mt9p111.h"

#ifndef SKYCDBG
#define SKYCDBG(fmt, args...) do{}while(0)
#define SKYCERR(fmt, args...) do{}while(0)
#endif

#ifndef TRUE
#define TRUE	1
#define FALSE 0
#endif

#define ON	1
#define OFF	0

#undef F_SKYCAM_EF18K_ES01

#ifdef F_SKYCAM_FIX_CFG_AF
static int auto_focus = 0;
#endif
/* F_SKYCAM_TODO, move to CUST_SKYCAM.h */
/* The intermittent failure with no image after initialization sequence is 
 * related to the sensor entering an invalid state during power up. 
 * The in-rush current due to charge sharing and gate charge initialization 
 * significantly reduce reset timing margin and cause some Aptina tested parts 
 * to fail at voltages at or below 1.8V (VDD). 
 * The in-rush current is believed to cause a significant internal glitch that 
 * disrupts internal circuitry, results in unsuccessful reset during power up 
 * and leads to no image issue. 
 * Internal nodes not directly shorted to VDD are charged by initial reset and 
 * are more robust by the second reset. 
 * The suggested solution is to have the second reset and also forcing a reset 
 * of the sensor core after initialization. */
#define F_SKYCAM_MT9P111_NO_IMAGE_AFTER_INIT

//#ifdef F_SKYCAM_TUP_LOAD_FILE
#include "camsensor_mt9p111_tuneup.h" 
//#endif
/* Micron MT9P111 Registers and their values */
/* Sensor Core Registers */
#define MT9P111_REG_MCU_ADDR      0x098E
#define MT9P111_REG_MCU_DATA      0x0990
#define  REG_MT9P111_MODEL_ID 0x3000
#define  MT9P111_MODEL_ID     0x1580

/*  SOC Registers Page 1  */
#define  REG_MT9P111_SENSOR_RESET     0x301A
#define  REG_MT9P111_STANDBY_CONTROL  0x3202
#define  REG_MT9P111_MCU_BOOT         0x3386

#ifdef F_SKYCAM_ADD_CFG_OTP_MODULE
/* OTP detecting register */
#define MT9P111_REG_OTP_DETECT_ZONE_ADDR	0x6026
#define MT9P111_REG_OTP_DETECT_ZONE_READ	0xE026

/* OTP setting register */
#define MT9P111_REG_OTP_SET_ZONE_ADDR 		0xD006
#define MT9P111_REG_OTP_SET_ZONE_CURRENT 	0xD005
#define MT9P111_REG_OTP_SET_ZONE_SOLUTION 	0xD004
#define MT9P111_REG_OTP_SET_ZONE_PGA_ALGO 	0xD002
#define MT9P111_REG_OTP_SET_ZONE_LENS_ENABLE 	0x3210

/* OTP zone read variable */
#if !(defined(CONFIG_MACH_MSM8X55_J_MASAI_E) && (BOARD_VER >= WS20)) //charley2
static uint16_t read_otp_zone = 0;
#endif
static uint16_t read_otp_zone_fail = 0;
#endif

#define MT9P111_I2C_RETRY	10
#define MT9P111_I2C_MPERIOD	200
#define MT9P111_SNAPSHOT_RETRY 	200
#define MT9P111_PREVIEW_RETRY 	50
#define MT9P111_POLLING_RETRY	 	30


#define CAM_5MP_RST_N	 	0
#define CAM_5MP_STB 	1
#define CAM_LDO_EN 		2 /* control external LDO (VDD_CAMA_2800) */
#if defined(CONFIG_MACH_MSM8X55_J_MASAI_E) && (BOARD_VER >= WS10)
#define CAM_FLASH_EN 	3	/* enable flash driver IC (power control) */
#endif

#ifdef MT9P111_J_MASAI_E_WBPOINT

#define NUM_PGA_PARA	12
#define OUTDOOR_START_INDEX	211
#define OUTDOOR_END_INDEX	307
#define INCAND_CCM_POSTION 8

enum pga_type_e {
	PGA_DAYLIGHT,
	PGA_FLOURESCENT,
	PGA_INCAND_A,
	PGA_PREVIEW,	
	PGA_MAX_NUM,
};

typedef struct {
	uint16_t	pgaAddr;
	uint16_t	pgaData[PGA_MAX_NUM];
} pga_struct_t;

static pga_struct_t pga_table[NUM_PGA_PARA] = {
	{0x3644, {0,0,0,0} },
	{0x364C, {0,0,0,0} },
	{0x364E, {0,0,0,0} },
	{0x3656, {0,0,0,0} },
	{0x3658, {0,0,0,0} },
	{0x3662, {0,0,0,0} },
	{0x368A, {0,0,0,0} },
	{0x3694, {0,0,0,0} },
	{0x36C0, {0,0,0,0} },
	{0x36CA, {0,0,0,0} },
	{0x36D4, {0,0,0,0} },
	{0x36DE, {0,0,0,0} }
};		

static const int32_t pga_factor_table[NUM_PGA_PARA][PGA_MAX_NUM] = {
	{ -29,  -31,  -70,  -30},
	{  77,  -69,   48,  -25},
	{ -26,  -96,  -39,  -75},
	{ 197,   77, -104,  113},
	{  -1,  -19, -199,  -14},
	{ -29,  -31,  -70,  -30},
	{ -37,  -18,  -23,  -24},
	{-164, -179,  289, -174},
	{ -51,  -26,  -65,  -33},
	{ -61,  -77,  -35,  -72},
	{ -36,    0, -144,  -11},
	{ -51,  -26,  -65,  -33}
};


#define CAL_GREY_FACTOR_M	125000
#define CAL_GREY_FACTOR_N	-50

#define CAL_RED_MIN			93
#define CAL_RED_MAX        128
#define CAL_RED_MIN_BRT    99	
#define CAL_RED_MAX_BRT    103

#define CAL_BLUE_MIN       75
#define CAL_BLUE_MAX       118
#define CAL_BLUE_MIN_BRT	108	
#define CAL_BLUE_MAX_BRT	118	

uint16_t red_green  = 0;
uint16_t blue_green = 0;

typedef struct {
	uint16_t	r_min;
	uint16_t	r_max;
	uint16_t	r_min_br;
	uint16_t	r_max_br;
	uint16_t	b_min;
	uint16_t	b_max;
	uint16_t	b_min_br;
	uint16_t	b_max_br;
} mwb_struct_t;

static mwb_struct_t mwb_setting[MT9P111_CFG_WB_MAX];

#endif /* MT9P111_J_MASAI_E_WBPOINT */


#ifdef F_SKYCAM_CHECK_IS_INIT_SEQ_DONE
bool init_seq_done = false;
#endif

typedef struct {
	int32_t id;
	const char *label;
	uint32_t num;
} gpio_ctrl_t;

static gpio_ctrl_t g_gpios[] = {
	{CAM_5MP_RST_N, "CAM_5MP_RST_N", 	121},
	{CAM_5MP_STB,   "CAM_5MP_STB", 	123},
	{CAM_LDO_EN,    "CAM_LDO_EN", 	43},
#if defined(CONFIG_MACH_MSM8X55_J_MASAI_E) && (BOARD_VER >= WS10)
	{CAM_FLASH_EN,  "CAM_FLASH_EN", 	33}
#endif
};

#ifdef SKY_ORIGINAL
#define VDD_CAM_1800	    0   /* gp7 */
#define VDD_CAM_1800_IO	1   /* gp10 */
#define VDD_CAMAF_2800 	2   /* gp4 */
#define VDD_CAMA_2800	     3   /* external LDO (controlled by CAM_LDO_EN) */
#else
#define VDD_CAM_1800	     0   /* external LDO (controlled by CAM_LDO_EN) */
#define VDD_CAM_1800_IO  	 1   /* VREG_L19 */
#define VDD_CAMAF_2800	     2   /* VREG_L20 */
#define VDD_CAMA_2800	     3   /* external LDO (controlled by CAM_LDO_EN) */
#endif 

typedef struct {
	int32_t id;
	const char *label;
	const char *vname;
	uint32_t mvolt;
} vreg_ctrl_t;


#if (BOARD_VER <  WS10)/* 2011.03.11 Do. layer1 for machine type*/
static vreg_ctrl_t g_vregs[] = {
	{VDD_CAM_1800,    "VDD_CAM_1800",    NULL,  0},
	{VDD_CAM_1800_IO, "VDD_CAM_1800_IO", "gp13", 1900},
	{VDD_CAMAF_2800,  "VDD_CAMAF_2800",  "wlan2",  2800},
	{VDD_CAMA_2800,   "VDD_CAMA_2800",   NULL,      0}
};

#else
static vreg_ctrl_t g_vregs[] = {
	{VDD_CAM_1800,    "VDD_CAM_1800",    NULL,  0},
	{VDD_CAM_1800_IO, "VDD_CAM_1800_IO", "gp10", 1900},
	{VDD_CAMAF_2800,  "VDD_CAMAF_2800",  "wlan2",  2800},
	{VDD_CAMA_2800,   "VDD_CAMA_2800",   NULL,      0}
};
#endif

struct mt9p111_work {
	struct work_struct work;
};

static struct  mt9p111_work *mt9p111_sensorw;
static struct  i2c_client *mt9p111_client;

struct mt9p111_ctrl_t {
	const struct msm_camera_sensor_info *sensordata;
};

static int multi_count = 0;
static struct mt9p111_ctrl_t *mt9p111_ctrl = NULL;

#ifdef F_SKYCAM_SAME_FOCUS_STEP_SKIP
#define AF_IMPLEMENTED 0xFF;
int cur_focus_pos ; 
#endif

#ifdef F_SKYCAM_ADD_CFG_DIMENSION
static struct dimension_cfg mt9p111_dimension;
#endif


static DECLARE_WAIT_QUEUE_HEAD(mt9p111_wait_queue);
DECLARE_MUTEX(mt9p111_sem);

#define LED_DATA1 168
#define LED_DATA2 169

struct mt9p111_vreg_t {
	const char *name;
	unsigned short mvolt;
};

#ifdef F_SKYCAM_TUP_LOAD_FILE
static mt9p111_tune_state_type mt9p111_tup_state = MT9P111_TUNE_STATE_NONE;
static mt9p111_tune_mode_type mt9p111_tup_mode = MT9P111_TUNE_STATE_TUNNING_MODE_OFF;

static mt9p111_params_tbl_t mt9p111_params_tbl;
#endif

/*=============================================================
	EXTERNAL DECLARATIONS
==============================================================*/
extern struct mt9p111_reg mt9p111_regs;

/*
================================================================================
LOCAL API DECLARATIONS
================================================================================
*/
static int32_t mt9p111_i2c_read_word(unsigned short raddr, unsigned short *rdata);

#ifdef MT9P111_J_MASAI_E_WBPOINT
static int32_t mt9p111_cal_pga_init(void);
static int32_t mt9p111_write_pga_table(uint16_t exp_val, uint16_t ccm_pos);
static int32_t mt9p111_AWB_cal_init(void);
#endif

#ifdef F_SKYCAM_TUP_LOAD_FILE
static u_int32_t mt9p111_i2c_write_params
(
    mt9p111_cfg_param_t *params,
    u_int32_t num_params
);
#endif

#ifdef F_SKYCAM_ADD_CFG_RED_LED_MODE
static int32_t mt9p111_set_red_led_mode(int32_t led_mode);
#endif

#ifdef F_SKYCAM_EF18K_ES01
/*
================================================================================
FUNCTION
================================================================================
*/
static int mt9p111_reset(const struct msm_camera_sensor_info *dev,  int set)
{
	int rc = 0;

	rc = gpio_request(dev->sensor_reset, "mt9p111");
		SKYCDBG("[2.5_gpio_request(dev->sensor_reset) = %d]\n", dev->sensor_reset);
		
	if (!rc) 
	{					
		rc = gpio_direction_output(dev->sensor_reset, 0);
		SKYCDBG("[3_reset pin = %d, value = LOW]\n", dev->sensor_reset);

		if(set)
		{
			rc = gpio_direction_output(dev->sensor_reset, set);
			SKYCDBG("[4_reset pin = %d, value = HIGH]\n", dev->sensor_reset);
		}
	}

	gpio_free(dev->sensor_reset);
	return rc;
}
#endif


static int32_t mt9p111_i2c_txdata(unsigned short saddr,
	unsigned char *txdata, int length)
{
	uint32_t i = 0;
	int32_t rc = 0;
	
	struct i2c_msg msg[] = {
		{
			.addr = saddr,
			.flags = 0,
			.len = length,
			.buf = txdata,
		},
	};

#if 0 //charley2
	if (length == 2)
		SKYCDBG("msm_io_i2c_w: 0x%04x 0x%04x\n",
			*(u16 *) txdata, *(u16 *) (txdata + 2));
	else if (length == 4)
		SKYCDBG("msm_io_i2c_w: 0x%04x\n", *(u16 *) txdata);
	else
		SKYCDBG("msm_io_i2c_w: length = %d\n", length);
#endif

#if 0
	if (i2c_transfer(mt9p111_client->adapter, msg, 1) < 0) {
		SKYCERR("mt9p111_i2c_txdata failed\n");
		return -EIO;
	}

	return 0;
#endif
	for (i = 0; i < MT9P111_I2C_RETRY; i++) {
		rc = i2c_transfer(mt9p111_client->adapter, msg, 1);
		if (rc >= 0) {			
			return 0;
		}
		SKYCERR("%s: tx retry. [%02x.%02x.%02x] len=%d rc=%d\n", __func__,saddr, *txdata, *(txdata + 1), length, rc);
		msleep(MT9P111_I2C_MPERIOD);
	}
	return -EIO;
}


static int32_t mt9p111_i2c_write(unsigned short saddr,
	unsigned short waddr, unsigned short wdata, enum mt9p111_width width)
{
	int32_t rc = -EIO;
	unsigned char buf[4];

	memset(buf, 0, sizeof(buf));
	
	switch (width) {
		case WORD_LEN: {
			buf[0] = (waddr & 0xFF00)>>8;
			buf[1] = (waddr & 0x00FF);
			buf[2] = (wdata & 0xFF00)>>8;
			buf[3] = (wdata & 0x00FF);

			rc = mt9p111_i2c_txdata(saddr, buf, 4);
		}
		break;

		case TRIPLE_LEN: {
			buf[0] = (waddr & 0xFF00)>>8;
			buf[1] = (waddr & 0x00FF);
			buf[2] = wdata;
			rc = mt9p111_i2c_txdata(saddr, buf, 3);
		}
		break;

		case BYTE_LEN: {
			buf[0] = waddr;
			buf[1] = wdata;
			rc = mt9p111_i2c_txdata(saddr, buf, 2);
		}
		break;

		default:
		break;
	}

	if (rc < 0)
		SKYCERR(
		"i2c_write failed, saddr= 0x%x, addr = 0x%x, val = 0x%x!\n",
		saddr, waddr, wdata);

	return rc;
}


#ifdef F_SKYCAM_ADD_CFG_OTP_MODULE
static int32_t mt9p111_i2c_write_a2d1(unsigned short waddr, unsigned char wdata)
{
	int32_t rc = -EIO;
	unsigned char buf[3];

	memset(buf, 0, sizeof(buf));

	buf[0] = (waddr & 0xFF00)>>8;
	buf[1] = (waddr & 0x00FF);
	buf[2] = wdata;
	
	rc = mt9p111_i2c_txdata(mt9p111_client->addr, buf, 3);

	if (rc < 0)
		SKYCERR(
		"i2c_write failed, saddr= 0x%x, addr = 0x%x, val = 0x%x!\n",
		mt9p111_client->addr, waddr, wdata);

	return rc;
}
#endif


static int32_t mt9p111_i2c_write_word(unsigned short waddr, unsigned short wdata)
{
	int32_t rc = -EIO;
	unsigned char buf[4];

	memset(buf, 0, sizeof(buf));

	buf[0] = (waddr & 0xFF00)>>8;
	buf[1] = (waddr & 0x00FF);
	buf[2] = (wdata & 0xFF00)>>8;
	buf[3] = (wdata & 0x00FF);

	rc = mt9p111_i2c_txdata(mt9p111_client->addr, buf, 4);

	if (rc < 0)
		SKYCERR(
		"i2c_write failed, saddr= 0x%x, addr = 0x%x, val = 0x%x!\n",
		mt9p111_client->addr, waddr, wdata);

	return rc;
}


static int32_t mt9p111_i2c_write_table(
	struct mt9p111_i2c_reg_conf const *reg_conf_tbl,
	int num_of_items_in_table)
{
	int i;
	int32_t rc = -EIO;

	uint16_t poll_delay = 0;
	uint16_t poll_retry = 0;
	uint16_t poll_mcu_var = 0;
	uint16_t poll_data = 0;
	uint16_t poll_mask = 0;
	uint16_t retry_cnt = 0;
	uint16_t read_data = 0;
	//OTP 방어코드 추가

#if !(defined(CONFIG_MACH_MSM8X55_J_MASAI_E) && (BOARD_VER >= WS20)) //charley2
#ifdef F_SKYCAM_ADD_CFG_OTP_MODULE
	uint16_t otp_retry_cnt = 0;
	uint16_t otp_poll_retry = 20;
#endif
#endif

	for (i = 0; i < num_of_items_in_table; i++) 
	{		
		switch(reg_conf_tbl->width )
		{
			case ZERO_LEN:
			{
				SKYCDBG("ZERO_LEN continue ADDR = 0x%x, VALUE = 0x%d\n",reg_conf_tbl->waddr, reg_conf_tbl->wdata);
				reg_conf_tbl++;			
				continue;
			}

			case POLL_MCU_VAR:
			{
				poll_mcu_var = reg_conf_tbl->waddr;
				poll_mask = reg_conf_tbl->wdata;	              
				poll_data = (reg_conf_tbl+1)->waddr;
				poll_delay = ((reg_conf_tbl+1)->wdata & 0xff00) >> 8;
				poll_retry = ((reg_conf_tbl+1)->wdata & 0x00ff);              

				SKYCDBG("POLLING!! poll_delay=%x, poll_retry=%x, poll_mcu_var=%x, poll_data=%x, poll_mask=%x\n",poll_delay, poll_retry, poll_mcu_var, poll_data, poll_mask);

				for (retry_cnt = 0; retry_cnt < poll_retry; retry_cnt++)
				{
#ifdef MT9P111_MODULE_ABNORMAL_OPERATION_DEFENCE                	
					if (mt9p111_i2c_read_word(poll_mcu_var, &read_data) < 0)
					{
						SKYCERR("<<POLL_MCU_VAR mt9p111_i2c_read_word (FALSE)\n");
						return -EIO;
					}
#else
					if (mt9p111_i2c_write_word(MT9P111_REG_MCU_ADDR, poll_mcu_var) < 0)
					{
						SKYCERR("<<POLL_MCU_VAR mt9p111_i2c_write_word (FALSE)\n");
						return -EIO;
					}

					if (mt9p111_i2c_read_word(MT9P111_REG_MCU_DATA, &read_data) < 0)
					{
						SKYCERR("<<POLL_MCU_VAR mt9p111_i2c_read_word (FALSE)\n");
						return -EIO;
					}
#endif
					if ((read_data & poll_mask) != poll_data)
					{
						//SKYCDBG("retry polling MCU variable... after sleeping %d ms, read_data=%2x\n", poll_delay, read_data);//charley2
						mdelay(poll_delay);
					}
					else
					{
						SKYCDBG("stop polling MCU variable... retried %d/%d time(s) (delay = %d ms), read_data=%2x\n", retry_cnt, poll_retry, poll_delay, read_data);
						break;
					}
				}

				if (retry_cnt == poll_retry)
				{
					SKYCDBG("<<RETRY FAIL!! read_data = %x (FALSE)\n", read_data);
#ifdef MT9P111_MODULE_ABNORMAL_OPERATION_DEFENCE						
					return MT9P111_REG_POLLING_ERROR;
#endif
				}
				//2개의 값을 이용하므로 +2를 해준다
				reg_conf_tbl++;
				reg_conf_tbl++;
				i++;
				break;
			}
			
			case POLL_MCU_VAR_NOT:
			{
				poll_mcu_var = reg_conf_tbl->waddr;
				poll_mask = reg_conf_tbl->wdata;	              
				poll_data = (reg_conf_tbl+1)->waddr;
				poll_delay = ((reg_conf_tbl+1)->wdata & 0xff00) >> 8;
				poll_retry = ((reg_conf_tbl+1)->wdata & 0x00ff);              

				SKYCDBG("POLLING!! poll_delay=%x, poll_retry=%x, poll_mcu_var=%x, poll_data=%x, poll_mask=%x\n",poll_delay, poll_retry, poll_mcu_var, poll_data, poll_mask);

				for (retry_cnt = 0; retry_cnt < poll_retry; retry_cnt++)
				{
#ifdef MT9P111_MODULE_ABNORMAL_OPERATION_DEFENCE                	
					if (mt9p111_i2c_read_word(poll_mcu_var, &read_data) < 0)
					{
						SKYCERR("<<POLL_MCU_VAR mt9p111_i2c_read_word (FALSE)\n");
						return -EIO;
					}
#else
					if (mt9p111_i2c_write_word(MT9P111_REG_MCU_ADDR, poll_mcu_var) < 0)
					{
						SKYCERR("<<POLL_MCU_VAR mt9p111_i2c_write_word (FALSE)\n");
						return -EIO;
					}

					if (mt9p111_i2c_read_word(MT9P111_REG_MCU_DATA, &read_data) < 0)
					{
						SKYCERR("<<POLL_MCU_VAR mt9p111_i2c_read_word (FALSE)\n");
						return -EIO;
					}
#endif
					if ((read_data & poll_mask) == poll_data)
					{
						//SKYCDBG("retry polling MCU variable... after sleeping %d ms, read_data=%2x\n", poll_delay, read_data);
						mdelay(poll_delay);
					}
					else
					{
						SKYCDBG("stop polling MCU variable... retried %d/%d time(s) (delay = %d ms), read_data=%2x\n", retry_cnt, poll_retry, poll_delay, read_data);//charley2
						break;
					}
				}

				if (retry_cnt == poll_retry)
				{
					SKYCDBG("<<RETRY FAIL!! read_data = %x (FALSE)\n", read_data);
#ifdef MT9P111_MODULE_ABNORMAL_OPERATION_DEFENCE												
					return MT9P111_REG_POLLING_ERROR;
#endif
				}
				reg_conf_tbl++;
				reg_conf_tbl++;
				i++;
				break;
			}
			
			case BURST_WORD_START:
			{
				uint16_t laddr = 0x886c;

				uint8_t *base = mt9p111_regs.init_patch_burst_settings;
				uint16_t len = mt9p111_regs.init_patch_burst_settings_size;
				uint16_t pos = 0;

				uint8_t  blk_dat[2 + 128]; // address 2 bytes + data 128 bytes
				uint16_t blk_len = 0;

				while (pos < len) {
					blk_len = ((len - pos) >= 128) ? 128 : (len - pos);
					blk_dat[0] = (uint8_t)((laddr & 0xff00) >> 8);
					blk_dat[1] = (uint8_t)(laddr & 0x00ff);
					
					memcpy(&blk_dat[2], &base[pos], blk_len);
					
					rc = mt9p111_i2c_txdata(mt9p111_client->addr, blk_dat, 2 + blk_len);
					
					if (rc < 0) {
						SKYCERR("burst write failed\n");
						break;
					}
					laddr += blk_len;
					pos += blk_len;
				}
			}
				
			if((++reg_conf_tbl)->width == BURST_WORD_END)
			{
				reg_conf_tbl++;
				SKYCDBG("<<BURST_WORD_END SUCCESS!!reg_conf_tbl.waddr=%x, reg_conf_tbl.wdata=%x\n", reg_conf_tbl->waddr, reg_conf_tbl->wdata);								
			}
			else
			{
				reg_conf_tbl++;
				reg_conf_tbl++;
				SKYCDBG("<<BURST_WORD_END FAIL!!reg_conf_tbl.waddr=%x, reg_conf_tbl.wdata=%x\n", reg_conf_tbl->waddr, reg_conf_tbl->wdata);				
			}
			i++;
			break;		
			
#ifdef F_SKYCAM_ADD_CFG_OTP_MODULE
			case OTP_MODULE_ZONE_DETECT_START:		
				mdelay(100); //안정화 delay
	#if defined(CONFIG_MACH_MSM8X55_J_MASAI_E) && (BOARD_VER >= WS20) //charley2
			#ifdef F_SKYCAM_SENSOR_BUG
				 if (mt9p111_i2c_write_word(0x381C, 0x0000) < 0)
	              {
	                  SKYCERR("<<POLL_MCU_VAR mt9p111_i2c_read_word (-EIO)\n");
	                  return -EIO;
	              }
				 
				if (mt9p111_i2c_write_word(0xE02A, 0x0001) < 0)	
				{
					SKYCERR("<<POLL_MCU_VAR mt9p111_i2c_read_word (FALSE)\n");
					return -EIO;
				}				 
			#endif
	#else
			#ifdef F_SKYCAM_SENSOR_BUG		     	
			     if (mt9p111_i2c_write_word(0x3812, 0x2124) < 0)
	              {
	                  SKYCERR("<<POLL_MCU_VAR mt9p111_i2c_read_word (-EIO)\n");
	                  return -EIO;
	              }
			#endif
	#endif
				
				if (mt9p111_i2c_read_word(0xE023, &read_otp_zone_fail) < 0)
				{
					SKYCERR("<<POLL_MCU_VAR mt9p111_i2c_read_word (FALSE)\n");
					return -EIO;
				}

				if ((read_otp_zone_fail & 0xff00) != 0xC100)
				{
					SKYCDBG("DEFALT_LENS_SHADING_MODE!!!, read_data=%2x\n", read_otp_zone_fail);

					mt9p111_i2c_write_word(0x3640, 0x02B0); 	// P_G1_P0Q0
					mt9p111_i2c_write_word(0x3642, 0x07EE); 	// P_G1_P0Q1
					mt9p111_i2c_write_word(0x3644, 0x3D71); 	// P_G1_P0Q2
					mt9p111_i2c_write_word(0x3646, 0xCAEB); 	// P_G1_P0Q3
					mt9p111_i2c_write_word(0x3648, 0xFE50); 	// P_G1_P0Q4
					mt9p111_i2c_write_word(0x364A, 0x02F0); 	// P_R_P0Q0
					mt9p111_i2c_write_word(0x364C, 0xDE8C); 	// P_R_P0Q1
					mt9p111_i2c_write_word(0x364E, 0x0211); 	// P_R_P0Q2
					mt9p111_i2c_write_word(0x3650, 0x6F4D); 	// P_R_P0Q3
					mt9p111_i2c_write_word(0x3652, 0xF070); 	// P_R_P0Q4
					mt9p111_i2c_write_word(0x3654, 0x03B0); 	// P_B_P0Q0
					mt9p111_i2c_write_word(0x3656, 0x6E8D); 	// P_B_P0Q1
					mt9p111_i2c_write_word(0x3658, 0x56D0); 	// P_B_P0Q2
					mt9p111_i2c_write_word(0x365A, 0xF36D); 	// P_B_P0Q3
					mt9p111_i2c_write_word(0x365C, 0x98F0); 	// P_B_P0Q4
					mt9p111_i2c_write_word(0x365E, 0x0150); 	// P_G2_P0Q0
					mt9p111_i2c_write_word(0x3660, 0xAD4D); 	// P_G2_P0Q1
					mt9p111_i2c_write_word(0x3662, 0x3F71); 	// P_G2_P0Q2
					mt9p111_i2c_write_word(0x3664, 0x016D); 	// P_G2_P0Q3
					mt9p111_i2c_write_word(0x3666, 0xFA30); 	// P_G2_P0Q4
					mt9p111_i2c_write_word(0x3680, 0x1FEA); 	// P_G1_P1Q0
					mt9p111_i2c_write_word(0x3682, 0xE0CD); 	// P_G1_P1Q1
					mt9p111_i2c_write_word(0x3684, 0x20AC); 	// P_G1_P1Q2
					mt9p111_i2c_write_word(0x3686, 0x192E); 	// P_G1_P1Q3
					mt9p111_i2c_write_word(0x3688, 0x912E); 	// P_G1_P1Q4
					mt9p111_i2c_write_word(0x368A, 0x4EAD); 	// P_R_P1Q0
					mt9p111_i2c_write_word(0x368C, 0x316D); 	// P_R_P1Q1
					mt9p111_i2c_write_word(0x368E, 0xFA09); 	// P_R_P1Q2
					mt9p111_i2c_write_word(0x3690, 0xCB0E); 	// P_R_P1Q3
					mt9p111_i2c_write_word(0x3692, 0xB90E); 	// P_R_P1Q4
					mt9p111_i2c_write_word(0x3694, 0xA4CE); 	// P_B_P1Q0
					mt9p111_i2c_write_word(0x3696, 0x598D); 	// P_B_P1Q1
					mt9p111_i2c_write_word(0x3698, 0x60AF); 	// P_B_P1Q2
					mt9p111_i2c_write_word(0x369A, 0xF2CE); 	// P_B_P1Q3
					mt9p111_i2c_write_word(0x369C, 0x9330); 	// P_B_P1Q4
					mt9p111_i2c_write_word(0x369E, 0xB82D); 	// P_G2_P1Q0
					mt9p111_i2c_write_word(0x36A0, 0xA86E); 	// P_G2_P1Q1
					mt9p111_i2c_write_word(0x36A2, 0x362F); 	// P_G2_P1Q2
					mt9p111_i2c_write_word(0x36A4, 0x3D0D); 	// P_G2_P1Q3
					mt9p111_i2c_write_word(0x36A6, 0xF6EF); 	// P_G2_P1Q4
					mt9p111_i2c_write_word(0x36C0, 0x5F11); 	// P_G1_P2Q0
					mt9p111_i2c_write_word(0x36C2, 0x0A4F); 	// P_G1_P2Q1
					mt9p111_i2c_write_word(0x36C4, 0xB572); 	// P_G1_P2Q2
					mt9p111_i2c_write_word(0x36C6, 0xA16B); 	// P_G1_P2Q3
					mt9p111_i2c_write_word(0x36C8, 0x0ED3); 	// P_G1_P2Q4
					mt9p111_i2c_write_word(0x36CA, 0x25B1); 	// P_R_P2Q0
					mt9p111_i2c_write_word(0x36CC, 0xF18E); 	// P_R_P2Q1
					mt9p111_i2c_write_word(0x36CE, 0x90D2); 	// P_R_P2Q2
					mt9p111_i2c_write_word(0x36D0, 0x276F); 	// P_R_P2Q3
					mt9p111_i2c_write_word(0x36D2, 0x2313); 	// P_R_P2Q4
					mt9p111_i2c_write_word(0x36D4, 0x2311); 	// P_B_P2Q0
					mt9p111_i2c_write_word(0x36D6, 0x126F); 	// P_B_P2Q1
					mt9p111_i2c_write_word(0x36D8, 0xF8D1); 	// P_B_P2Q2
					mt9p111_i2c_write_word(0x36DA, 0x870E); 	// P_B_P2Q3
					mt9p111_i2c_write_word(0x36DC, 0x12F3); 	// P_B_P2Q4
					mt9p111_i2c_write_word(0x36DE, 0x61F1); 	// P_G2_P2Q0
					mt9p111_i2c_write_word(0x36E0, 0x92AF); 	// P_G2_P2Q1
					mt9p111_i2c_write_word(0x36E2, 0xA1F2); 	// P_G2_P2Q2
					mt9p111_i2c_write_word(0x36E4, 0x1310); 	// P_G2_P2Q3
					mt9p111_i2c_write_word(0x36E6, 0x6872); 	// P_G2_P2Q4
					mt9p111_i2c_write_word(0x3700, 0x364F); 	// P_G1_P3Q0
					mt9p111_i2c_write_word(0x3702, 0xA04B); 	// P_G1_P3Q1
					mt9p111_i2c_write_word(0x3704, 0xBA6D); 	// P_G1_P3Q2
					mt9p111_i2c_write_word(0x3706, 0x6BF0); 	// P_G1_P3Q3
					mt9p111_i2c_write_word(0x3708, 0x91F2); 	// P_G1_P3Q4
					mt9p111_i2c_write_word(0x370A, 0x1E0F); 	// P_R_P3Q0
					mt9p111_i2c_write_word(0x370C, 0x954E); 	// P_R_P3Q1
					mt9p111_i2c_write_word(0x370E, 0xDBAF); 	// P_R_P3Q2
					mt9p111_i2c_write_word(0x3710, 0x1EF1); 	// P_R_P3Q3
					mt9p111_i2c_write_word(0x3712, 0x8A51); 	// P_R_P3Q4
					mt9p111_i2c_write_word(0x3714, 0xF66C); 	// P_B_P3Q0
					mt9p111_i2c_write_word(0x3716, 0xC10E); 	// P_B_P3Q1
					mt9p111_i2c_write_word(0x3718, 0x1BD0); 	// P_B_P3Q2
					mt9p111_i2c_write_word(0x371A, 0x0251); 	// P_B_P3Q3
					mt9p111_i2c_write_word(0x371C, 0xB5F2); 	// P_B_P3Q4
					mt9p111_i2c_write_word(0x371E, 0x736E); 	// P_G2_P3Q0
					mt9p111_i2c_write_word(0x3720, 0xA4CE); 	// P_G2_P3Q1
					mt9p111_i2c_write_word(0x3722, 0x430F); 	// P_G2_P3Q2
					mt9p111_i2c_write_word(0x3724, 0x45B1); 	// P_G2_P3Q3
					mt9p111_i2c_write_word(0x3726, 0xAED2); 	// P_G2_P3Q4
					mt9p111_i2c_write_word(0x3740, 0x9A11); 	// P_G1_P4Q0
					mt9p111_i2c_write_word(0x3742, 0x9350); 	// P_G1_P4Q1
					mt9p111_i2c_write_word(0x3744, 0x8D31); 	// P_G1_P4Q2
					mt9p111_i2c_write_word(0x3746, 0xD04E); 	// P_G1_P4Q3
					mt9p111_i2c_write_word(0x3748, 0x1174); 	// P_G1_P4Q4
					mt9p111_i2c_write_word(0x374A, 0x8811); 	// P_R_P4Q0
					mt9p111_i2c_write_word(0x374C, 0x292F); 	// P_R_P4Q1
					mt9p111_i2c_write_word(0x374E, 0x776F); 	// P_R_P4Q2
					mt9p111_i2c_write_word(0x3750, 0x9E71); 	// P_R_P4Q3
					mt9p111_i2c_write_word(0x3752, 0x21F3); 	// P_R_P4Q4
					mt9p111_i2c_write_word(0x3754, 0xB8D0); 	// P_B_P4Q0
					mt9p111_i2c_write_word(0x3756, 0xA010); 	// P_B_P4Q1
					mt9p111_i2c_write_word(0x3758, 0x9F0B); 	// P_B_P4Q2
					mt9p111_i2c_write_word(0x375A, 0x7190); 	// P_B_P4Q3
					mt9p111_i2c_write_word(0x375C, 0x47F3); 	// P_B_P4Q4
					mt9p111_i2c_write_word(0x375E, 0x89F1); 	// P_G2_P4Q0
					mt9p111_i2c_write_word(0x3760, 0x5D0E); 	// P_G2_P4Q1
					mt9p111_i2c_write_word(0x3762, 0x8C12); 	// P_G2_P4Q2
					mt9p111_i2c_write_word(0x3764, 0xA891); 	// P_G2_P4Q3
					mt9p111_i2c_write_word(0x3766, 0x3D54); 	// P_G2_P4Q4
					mt9p111_i2c_write_word(0x3782, 0x03B4); 	// CENTER_ROW
					mt9p111_i2c_write_word(0x3784, 0x04F4); 	// CENTER_COLUMN
					mt9p111_i2c_write_word(0x3210, 0x49B8); 	// COLOR_PIPELINE_CONTROL      
				}
				else
				{
					SKYCDBG("OTP_SELECT_MODE!!!, read_data=%2x\n", read_data);

		#if defined(CONFIG_MACH_MSM8X55_J_MASAI_E) && (BOARD_VER >= WS20) //charley2

					mt9p111_i2c_write_a2d1(0xD004, 0x04);
					mt9p111_i2c_write_word(0xD006, 0x0008); 
					mt9p111_i2c_write_a2d1(0xD005, 0x00);
					mt9p111_i2c_write_word(0xD002, 0x8002); 
					mt9p111_i2c_write_word(0x3210, 0x49B8); 
		#else

					mt9p111_i2c_write_word(0xE024, 0x0100);
					mt9p111_i2c_write_word(0xE02A, 0xA010);
					mt9p111_i2c_write_a2d1(0x8404, 0x05);

					for (otp_retry_cnt = 0; otp_retry_cnt < otp_poll_retry; otp_retry_cnt++)
					{
						if (mt9p111_i2c_read_word(0x8404, &read_data) < 0)
						{
							SKYCERR("<<POLL_MCU_VAR mt9p111_i2c_read_word (FALSE)\n");
							return -EIO;
						}

						if ((read_data & 0xff00) != 0)
						{
							//SKYCDBG("retry polling MCU variable... after sleeping %d ms, read_data=%2x\n", poll_delay, read_data);
							mdelay(50);
						}
						else
						{
							SKYCDBG("stop polling MCU variable... retried %d/%d time(s) (delay = %d ms), read_data=%2x\n", retry_cnt, poll_retry, poll_delay, read_data);//charley2
							break;
						}
					}

					if (retry_cnt == poll_retry)
					{
						SKYCDBG("<<RETRY FAIL!! read_data = %x (FALSE)\n", read_data);
				#ifdef MT9P111_MODULE_ABNORMAL_OPERATION_DEFENCE												
						return MT9P111_REG_POLLING_ERROR;
				#endif
					}
		#endif
				}

	//===================================================================================			

		#if !(defined(CONFIG_MACH_MSM8X55_J_MASAI_E) && (BOARD_VER >= WS20)) //charley2
				if ((read_otp_zone_fail & 0xff00) == 0xC100)
				{
			       	SKYCERR("<<OTP_ZONE_SELECT!!! read_otp_zone_fail=%x\n", read_otp_zone_fail);
					if (mt9p111_i2c_read_word(MT9P111_REG_OTP_DETECT_ZONE_READ, &read_otp_zone) < 0)
					{
						SKYCERR("<<MT9P111_REG_OTP_DETECT_ZONE_READ FAIL!! mt9p111_i2c_read_word (FALSE)\n");
						return -EIO;
					}

					if(read_otp_zone == 0)
					{
						SKYCDBG("<<OTP_MODULE_ZONE_SET : ZONE 0 [read_otp_zone]=%x\n", read_otp_zone);
	                      if (mt9p111_i2c_write_a2d1(MT9P111_REG_OTP_SET_ZONE_SOLUTION, 0x04) < 0)
						{
							SKYCERR("<<MT9P111_REG_OTP_SET_ZONE_SOLUTION FAIL!! mt9p111_i2c_write_word (FALSE)\n");
							return -EIO;
						}
								 
						if (mt9p111_i2c_write_word(MT9P111_REG_OTP_SET_ZONE_ADDR, 0) < 0)
						{
							SKYCERR("<<MT9P111_REG_OTP_DETECT_ZONE_ADDR FAIL!! mt9p111_i2c_write_word (FALSE)\n");
							return -EIO;
						}
						
						if (mt9p111_i2c_write_a2d1(MT9P111_REG_OTP_SET_ZONE_CURRENT, 0) < 0)
						{
							SKYCERR("<<MT9P111_REG_OTP_SET_ZONE_CURRENT FAIL!! mt9p111_i2c_write_word (FALSE)\n");
							return -EIO;
						}
						
						if (mt9p111_i2c_write_word(MT9P111_REG_OTP_SET_ZONE_PGA_ALGO, 0x8002) < 0)
						{
							SKYCERR("<<MT9P111_REG_OTP_SET_ZONE_PGA_ALGO FAIL!! mt9p111_i2c_write_word (FALSE)\n");
							return -EIO;
						}
						
						if (mt9p111_i2c_write_word(MT9P111_REG_OTP_SET_ZONE_LENS_ENABLE, 0x49B8) < 0)
						{
							SKYCERR("<<MT9P111_REG_OTP_SET_ZONE_LENS_ENABLE FAIL!! mt9p111_i2c_write_word (FALSE)\n");
							return -EIO;
						}				
					}
					else
					{
						SKYCDBG("<<OTP_MODULE_ZONE_SET : ZONE 1 [read_otp_zone]=%x\n", read_otp_zone);
						
	                       if (mt9p111_i2c_write_a2d1(MT9P111_REG_OTP_SET_ZONE_SOLUTION, 0x04) < 0)
						{
							SKYCERR("<<MT9P111_REG_OTP_SET_ZONE_SOLUTION FAIL!! mt9p111_i2c_write_word (FALSE)\n");
							return -EIO;
						}
								  
						if (mt9p111_i2c_write_word(MT9P111_REG_OTP_SET_ZONE_ADDR, 0x0100) < 0)
						{
							SKYCERR("<<MT9P111_REG_OTP_DETECT_ZONE_ADDR FAIL!! mt9p111_i2c_write_word (FALSE)\n");
							return -EIO;
						}
						
						if (mt9p111_i2c_write_a2d1(MT9P111_REG_OTP_SET_ZONE_CURRENT, 0x01) < 0)
						{
							SKYCERR("<<MT9P111_REG_OTP_SET_ZONE_CURRENT FAIL!! mt9p111_i2c_write_word (FALSE)\n");
							return -EIO;
						}
						
						if (mt9p111_i2c_write_word(MT9P111_REG_OTP_SET_ZONE_PGA_ALGO, 0x8002) < 0)
						{
							SKYCERR("<<MT9P111_REG_OTP_SET_ZONE_PGA_ALGO FAIL!! mt9p111_i2c_write_word (FALSE)\n");
							return -EIO;
						}
						
						if (mt9p111_i2c_write_word(MT9P111_REG_OTP_SET_ZONE_LENS_ENABLE, 0x49B8) < 0)
						{
							SKYCERR("<<MT9P111_REG_OTP_SET_ZONE_LENS_ENABLE FAIL!! mt9p111_i2c_write_word (FALSE)\n");
							return -EIO;
						}
					}
				}
		#endif
	
				if((++reg_conf_tbl)->width == OTP_MODULE_ZONE_DETECT_END)
				{
		#ifdef MT9P111_J_MASAI_E_WBPOINT
					rc = mt9p111_AWB_cal_init();
					if (rc < 0)
					{
						SKYCERR("%s :mt9p111_AWB_cal_init. rc = %d\n", __func__, rc);
						return rc;
					}	

					rc = mt9p111_cal_pga_init();
					if (rc < 0)
					{
						SKYCERR("%s : mt9p111_cal_pga_init. rc = %d\n", __func__, rc);
						return rc;
					}	
		#endif /* MT9P111_J_MASAI_E_WBPOINT */
					reg_conf_tbl++;
		#if !(defined(CONFIG_MACH_MSM8X55_J_MASAI_E) && (BOARD_VER >= WS20)) //charley2
					SKYCDBG("<<OTP_MODULE_ZONE_DETECT_END SUCCESS!!read_otp_zone_waddr=%x, read_otp_zone_wdata=%x\n", MT9P111_REG_OTP_DETECT_ZONE_READ, read_otp_zone);	
		#endif
				}
				else
				{
					reg_conf_tbl++;
					reg_conf_tbl++;
		#if !(defined(CONFIG_MACH_MSM8X55_J_MASAI_E) && (BOARD_VER >= WS20)) //charley2
					SKYCDBG("<<OTP_MODULE_ZONE_DETECT_END FAIL!!read_otp_zone_waddr=%x, read_otp_zone_wdata=%x\n", MT9P111_REG_OTP_DETECT_ZONE_READ, read_otp_zone);
		#endif
				}
				i++;				
				break;
#endif /*	F_SKYCAM_ADD_CFG_OTP_MODULE */		

			default:
			{
				rc = mt9p111_i2c_write(mt9p111_client->addr,
						reg_conf_tbl->waddr, reg_conf_tbl->wdata,
						reg_conf_tbl->width);		

				if (rc < 0)
				{
					SKYCERR("mt9p111_i2c_read failed!\n");
					return rc;
				}
				
				if (reg_conf_tbl->mdelay_time != 0)
					mdelay(reg_conf_tbl->mdelay_time);

				reg_conf_tbl++;
				break;
			}			
		}
	}

	return rc;
}


static int mt9p111_i2c_rxdata(unsigned short saddr, int slength,
	unsigned char *rxdata, int length)
{
	uint32_t i = 0;
	int32_t rc = 0;
	
	struct i2c_msg msgs[] = {
		{
			.addr   = saddr,
			.flags = 0,
			.len   = slength,//   2, //읽고 싶은 레지스터의 주소 길이
			.buf   = rxdata,// 읽고 싶은 레지스터의 데이터 길이
		},
		{
			.addr   = saddr,
			.flags = I2C_M_RD,
			.len   = length,
			.buf   = rxdata,
		},
	};

#if 0//charley2
	if (length == 2)
		SKYCDBG("msm_io_i2c_r: 0x%04x 0x%04x\n",
			*(u16 *) rxdata, *(u16 *) (rxdata + 2));
	else if (length == 4)
		SKYCDBG("msm_io_i2c_r: 0x%04x\n", *(u16 *) rxdata);
	else
		SKYCDBG("msm_io_i2c_r: length = %d\n", length);
#endif

#if 0
	if (i2c_transfer(mt9p111_client->adapter, msgs, 2) < 0) {
		SKYCERR("mt9p111_i2c_rxdata failed!\n");
		return -EIO;
	}

	return 0;
#endif
	for (i = 0; i < MT9P111_I2C_RETRY; i++) {
		rc = i2c_transfer(mt9p111_client->adapter, msgs, 2);
		if (rc >= 0) {			
			return 0;
		}
		SKYCERR("%s: tx retry. [%02x.%02x] len=%d rc=%d\n", __func__,saddr, *rxdata, length, rc);
		msleep(MT9P111_I2C_MPERIOD);
	}
	
	SKYCDBG("%s end\n",__func__);
	return -EIO;//0;
}


static int32_t mt9p111_i2c_read(unsigned short   saddr,
	unsigned short raddr, unsigned short *rdata, enum mt9p111_width width)
{
	int32_t rc = 0;
	unsigned char buf[4];

	if (!rdata)
		return -EIO;

	memset(buf, 0, sizeof(buf));

	switch (width) {
		case WORD_LEN: { //주소와 데이터가 모두 2바이트 일 경우
			buf[0] = (raddr & 0xFF00)>>8;
			buf[1] = (raddr & 0x00FF);

			rc = mt9p111_i2c_rxdata(saddr, 2, buf, 2);
			if (rc < 0)
				return rc;

			*rdata = buf[0] << 8 | buf[1];
		}
		break;

		case TRIPLE_LEN: { //주소는 2바이트 데이터는 1바이트 일 경우
			buf[0] = (raddr & 0xFF00)>>8;
			buf[1] = (raddr & 0x00FF);

			rc = mt9p111_i2c_rxdata(saddr, 2, buf, 1);
			if (rc < 0)
				return rc;

			*rdata = buf[0];
		}
		break;

		default:
		break;
	}

	if (rc < 0)
		SKYCERR("mt9p111_i2c_read failed!\n");

	return rc;
}


static int32_t mt9p111_i2c_read_word(unsigned short raddr, unsigned short *rdata)
{
	int32_t rc = 0;
	unsigned char buf[4];

	if (!rdata)
		return -EIO;

	memset(buf, 0, sizeof(buf));

	buf[0] = (raddr & 0xFF00)>>8;
	buf[1] = (raddr & 0x00FF);

	rc = mt9p111_i2c_rxdata(mt9p111_client->addr, 2, buf, 2);
	if (rc < 0)
		return rc;

	*rdata = buf[0] << 8 | buf[1];

	if (rc < 0)
		SKYCERR("mt9p111_i2c_read_word failed!\n");

	return rc;
}


static int32_t mt9p111_gpio_out(int32_t id, int32_t val)
{
	int32_t rc = 0;


#if defined(CONFIG_MACH_MSM8X55_J_MASAI_E) && (BOARD_VER >= WS10)
	if ((id < CAM_5MP_RST_N) || (id > CAM_FLASH_EN)) {
		SKYCERR("%s#%d: -EINVAL\n", __func__, __LINE__);
		return -EINVAL;
	}
#else
	if ((id < CAM_5MP_RST_N) || (id > CAM_LDO_EN)) {
		SKYCERR("%s#%d: -EINVAL\n", __func__, __LINE__);
		return -EINVAL;
	}
#endif

	rc = gpio_request(g_gpios[id].num, g_gpios[id].label);
	if (rc) {
		SKYCERR("%s#%d: error. rc=%d\n", __func__, __LINE__, rc);
		goto gpio_out_fail;
	}

	rc = gpio_direction_output(g_gpios[id].num, val);
	if (rc) {
		SKYCERR("%s#%d: error. rc=%d\n", __func__, __LINE__, rc);
		goto gpio_out_fail;
	}

	gpio_free(g_gpios[id].num);

	SKYCDBG("%s: set %s to %d\n", __func__, g_gpios[id].label, val);
	return 0;

gpio_out_fail:
	return rc;
}


static int32_t mt9p111_vreg_out(int32_t id, int32_t on) 
{
	struct vreg *vreg = NULL;
	int32_t rc = 0;

	if ((id < VDD_CAM_1800) || (id > VDD_CAMAF_2800)){
		SKYCERR("%s#%d: -EINVAL\n", __func__, __LINE__);
		return -EINVAL;
	}

	vreg = vreg_get(NULL, g_vregs[id].vname);
	if (IS_ERR(vreg)) {
		rc = -ENOENT;
		SKYCERR("%s#%d: -ENOENT\n", __func__, __LINE__);
		goto vreg_out_fail;
	}

	if (on) {
		rc = vreg_set_level(vreg, g_vregs[id].mvolt);
		if (rc) {
			SKYCERR("%s#%d: error. rc=%d\n", __func__, __LINE__, rc);
			goto vreg_out_fail;
		}
		rc = vreg_enable(vreg);
		if (rc) {
			SKYCERR("%s#%d: error. rc=%d\n", __func__, __LINE__, rc);
			goto vreg_out_fail;
		}
		SKYCDBG("%s: turn ON %s\n", __func__, g_vregs[id].label);
	} else {
		rc = vreg_disable(vreg);
		if (rc) {
			SKYCERR("%s#%d: error. rc=%d\n", __func__, __LINE__, rc);
			goto vreg_out_fail;
		}
		SKYCDBG("%s: turn OFF %s\n", __func__, g_vregs[id].label);
	}

	return 0;

vreg_out_fail:
	return rc;
}


static int32_t mt9p111_power_on(void)
{
	int32_t rc = 0;
	SKYCDBG ("mt9p111_power_on \n");

	if (mt9p111_vreg_out(VDD_CAM_1800_IO, 1)) 	rc = -EIO;
	mdelay(5);
	if (mt9p111_gpio_out(CAM_5MP_STB, 0))		rc = -EIO;
	mdelay(5);	
	/* prevent 5MP/VGA RESET signals from becoming HIGH state just after 
	 * supplying CORE/IO voltages. (sensor HW issue) */
	if (mt9p111_gpio_out(CAM_5MP_RST_N, 0))		rc = -EIO;
	mdelay(5);
	/* turn on VDD_CAMA_2800 */
	if (mt9p111_gpio_out(CAM_LDO_EN, 1))		    rc = -EIO;
	mdelay(5);
	if (mt9p111_vreg_out(VDD_CAMAF_2800, 1))	    rc = -EIO;
	mdelay(15);
	if (mt9p111_gpio_out(CAM_5MP_RST_N, 1))		rc = -EIO;
	mdelay(10);
#ifdef F_SKYCAM_MT9P111_NO_IMAGE_AFTER_INIT
	if (mt9p111_i2c_write_word(0x0020, 0x0000) < 0) rc = -EIO;
	mdelay(1);
	if (mt9p111_gpio_out(CAM_5MP_RST_N, 0))	     	rc = -EIO;
	udelay(3);
	if (mt9p111_gpio_out(CAM_5MP_RST_N, 1))		     rc = -EIO;
	udelay(40);
	if (mt9p111_i2c_write_word(0x0020, 0x0000) < 0) rc = -EIO;
#endif

#if defined(CONFIG_MACH_MSM8X55_J_MASAI_E) && (BOARD_VER >= WS10)
	if (mt9p111_set_red_led_mode(ON))				rc = -EIO;
#endif

	return rc;
}

static int32_t mt9p111_power_off(void)
{
	int32_t rc = 0;
 
	/* VDD_CAM_1800_IO is opened and merged with VDD_CAM_1800 in ET15
	 * to correct 'no image' and 'burnt image' issues. 
	 * We should not remove VDD_CAM_1800_IO for backward compatibility. */

	if (mt9p111_gpio_out(CAM_5MP_RST_N, 0))		rc = -EIO;
	mdelay(3);
	if (mt9p111_vreg_out(VDD_CAMAF_2800, 0)) 	rc = -EIO;
	/* turn off VDD_CAMA_2800 */
	if (mt9p111_gpio_out(CAM_LDO_EN, 0))		    rc = -EIO;
	mdelay(4);
	if (mt9p111_vreg_out(VDD_CAM_1800_IO, 0)) 	rc = -EIO;
	mdelay(1);

#if defined(CONFIG_MACH_MSM8X55_J_MASAI_E) && (BOARD_VER >= WS10)
	if (mt9p111_gpio_out(CAM_FLASH_EN, 0))       rc = -EIO;
	if (mt9p111_set_red_led_mode(OFF))			 rc = -EIO;
#endif

	return rc;
}


static int mt9p111_reg_init(void)
{
	int32_t rc = 0;
	int32_t i =0;
	SKYCDBG("%s start\n",__func__);

#ifdef F_SKYCAM_TUP_LOAD_FILE
	if((mt9p111_tup_state == MT9P111_TUNE_STATE_LOAD_VALUE) && (mt9p111_params_tbl.init.num_params > 0))	
	{
		rc = mt9p111_i2c_write_params(mt9p111_params_tbl.init.params,
								mt9p111_params_tbl.init.num_params);
	}
	else
#endif
	{
		rc = mt9p111_i2c_write_table(&mt9p111_regs.init_settings[0],
					mt9p111_regs.init_settings_size);
	}
#ifdef MT9P111_MODULE_ABNORMAL_OPERATION_DEFENCE
	if(rc == MT9P111_REG_POLLING_ERROR)
	{
		for( i =0; i <5; i++ )
		{
	
			SKYCERR("POLLING FAIL!!!RETRY INIT SEQUENCE~~~\n");
			gpio_request(121, "mt9p111");
			gpio_direction_output(121, 0);
			mdelay(5);
			gpio_direction_output(121, 1);
			gpio_free(121);
			mdelay(5);

#ifdef F_SKYCAM_MT9P111_NO_IMAGE_AFTER_INIT
			if (mt9p111_i2c_write_word(0x0020, 0x0000) < 0) rc = -EIO;
			mdelay(1);
			if (mt9p111_gpio_out(CAM_5MP_RST_N, 0))		rc = -EIO;
			udelay(3);
			if (mt9p111_gpio_out(CAM_5MP_RST_N, 1))		rc = -EIO;
			udelay(40);
			if (mt9p111_i2c_write_word(0x0020, 0x0000) < 0) rc = -EIO;
#endif

#ifdef F_SKYCAM_TUP_LOAD_FILE
			if((mt9p111_tup_state == MT9P111_TUNE_STATE_LOAD_VALUE) && (mt9p111_params_tbl.init.num_params > 0))	
			{
				rc = mt9p111_i2c_write_params(mt9p111_params_tbl.init.params,
										mt9p111_params_tbl.init.num_params);
			}
			else
#endif
			{
				rc = mt9p111_i2c_write_table(&mt9p111_regs.init_settings[0],
								mt9p111_regs.init_settings_size);
			}

			if((i==4) && (rc == MT9P111_REG_POLLING_ERROR))
			{
				SKYCERR("5 times RETRY POLLING FAIL!!!OUT OF INIT~~~\n");
				return 0;
			}
			if(rc == MT9P111_REG_POLLING_ERROR)
			{
				SKYCERR("RETRY POLLING_ERROR!!!retry time : %d\n", i+1);
				continue; //return 0;
			}
			else
			{
				SKYCERR("RETRY INIT SUCCESS!!!retry time : %d\n", i+1);
				return 0;
			}
		}
	}
#endif	
	
	if (rc < 0)
	{
		SKYCERR("mt9p111_i2c_write_table FAIL!!! return~~\n");
		return rc;
	}

#if 0
	mt9p111_i2c_read_table(&mt9p111_regs.init_settings[0],
					mt9p111_regs.init_settings_size);
#endif
	SKYCDBG("%s END\n",__func__);
	return 0;
}


static int mt9p111_set_effect(int mode, int effect)
{
	int rc = 0;

	SKYCDBG("%s start\n",__func__);

	if(effect < CAMERA_EFFECT_OFF || effect >= CAMERA_EFFECT_MAX){
		SKYCERR("%s error. effect=%d\n", __func__, effect);
		return -EINVAL;
	}

	rc = mt9p111_i2c_write_table(mt9p111_regs.effect_cfg_settings[effect],
			mt9p111_regs.effect_cfg_settings_size);
	
	if (rc < 0)
	{
		SKYCERR("CAMERA_WB I2C FAIL!!! return~~\n");
		return rc;
	}		

	/* Refresh Sequencer */	
	SKYCDBG("%s end\n",__func__);
	
	return rc;
}


#ifdef F_SKYCAM_FIX_CFG_WB
#ifndef MT9P111_J_MASAI_E_WBPOINT
static int32_t mt9p111_set_whitebalance (int mode, int32_t whitebalance)
{
	
	int32_t rc = 0;
		
	SKYCDBG("%s start  mode=%d, whitebalance=%d\n",__func__, mode, whitebalance);

#ifdef F_SKYCAM_TUP_LOAD_FILE
	if((mt9p111_tup_state == MT9P111_TUNE_STATE_LOAD_VALUE) && (mt9p111_params_tbl.wb[whitebalance-1].num_params > 0))	
	{
		mt9p111_i2c_write_params(mt9p111_params_tbl.wb[whitebalance-1].params,
								mt9p111_params_tbl.wb[whitebalance-1].num_params);
	}
	else
#endif		
	{
		rc = mt9p111_i2c_write_table(mt9p111_regs.wb_cfg_settings[whitebalance-1],
						mt9p111_regs.wb_cfg_settings_size);
	}

	if (rc < 0)
	{
		SKYCERR("CAMERA_WB I2C FAIL!!! return~~\n");
		return rc;
	}		

	SKYCDBG("%s end\n",__func__);

	return rc;
}

#else /* MT9P111_J_MASAI_E_WBPOINT */

static int32_t mt9p111_set_whitebalance (int mode, int32_t whitebalance)
{
	
	int32_t rc = 0;
		
	SKYCDBG("%s start  mode=%d, whitebalance=%d\n",__func__, mode, whitebalance);

	rc = mt9p111_i2c_write_word(0x098E, 0xACB0);
	if (rc < 0)
	{
		SKYCERR("%s : 0x098E CAMERA_WB I2C fail. rc = %d\n", __func__, rc);
		return rc;
	}	
	
	rc = mt9p111_i2c_write_a2d1(0xACB0, (unsigned char)mwb_setting[whitebalance-1].r_min);
	if (rc < 0)
	{
		SKYCERR("%s : 0xACB0 CAMERA_WB I2C fail. rc = %d\n", __func__, rc);
		return rc;
	}	

	rc = mt9p111_i2c_write_a2d1(0xACB1, (unsigned char)mwb_setting[whitebalance-1].r_max);
	if (rc < 0)
	{
		SKYCERR("%s : 0xACB1 CAMERA_WB I2C fail. rc = %d\n", __func__, rc);
		return rc;
	}	

	rc = mt9p111_i2c_write_a2d1(0xACB2, (unsigned char)mwb_setting[whitebalance-1].r_min_br);
	if (rc < 0)
	{
		SKYCERR("%s : 0xACB2 CAMERA_WB I2C fail. rc = %d\n", __func__, rc);
		return rc;
	}

	rc = mt9p111_i2c_write_a2d1(0xACB3, (unsigned char)mwb_setting[whitebalance-1].r_max_br);
	if (rc < 0)
	{
		SKYCERR("%s : 0xACB3 CAMERA_WB I2C fail. rc = %d\n", __func__, rc);
		return rc;
	}

	rc = mt9p111_i2c_write_a2d1(0xACB4, (unsigned char)mwb_setting[whitebalance-1].b_min);
	if (rc < 0)
	{
		SKYCERR("%s : 0xACB4 CAMERA_WB I2C fail. rc = %d\n", __func__, rc);
		return rc;
	}

	rc = mt9p111_i2c_write_a2d1(0xACB5, (unsigned char)mwb_setting[whitebalance-1].b_max);
	if (rc < 0)
	{
		SKYCERR("%s : 0xACB5 CAMERA_WB I2C fail. rc = %d\n", __func__, rc);
		return rc;
	}

	rc = mt9p111_i2c_write_a2d1(0xACB6, (unsigned char)mwb_setting[whitebalance-1].b_min_br);
	if (rc < 0)
	{
		SKYCERR("%s : 0xACB6 CAMERA_WB I2C fail. rc = %d\n", __func__, rc);
		return rc;
	}

	rc = mt9p111_i2c_write_a2d1(0xACB7, (unsigned char)mwb_setting[whitebalance-1].b_max_br);
	if (rc < 0)
	{
		SKYCERR("%s : 0xACB7 CAMERA_WB I2C fail. rc = %d\n", __func__, rc);
		return rc;
	}	
	
	rc = mt9p111_i2c_write_a2d1(0xAC04, (unsigned char)mwb_setting[whitebalance-1].r_min_br);
	if (rc < 0)
	{
		SKYCERR("%s : 0xAC04 CAMERA_WB I2C fail. rc = %d\n", __func__, rc);
		return rc;
	}	

	rc = mt9p111_i2c_write_a2d1(0xAC05, (unsigned char)mwb_setting[whitebalance-1].b_min_br);
	if (rc < 0)
	{
		SKYCERR("%s : 0xAC05 CAMERA_WB I2C fail. rc = %d\n", __func__, rc);
		return rc;
	}	

	SKYCDBG("%s end\n",__func__);

	return rc;
}
#endif /* MT9P111_J_MASAI_E_WBPOINT */
#endif


#ifdef F_SKYCAM_FIX_CFG_BRIGHTNESS
static int32_t mt9p111_set_brightness(int32_t brightness)
{
	int32_t rc = 0;
	SKYCDBG("%s start~ receive brightness = %d\n",__func__, brightness);
	
	if ((brightness < 0) || (brightness >= 9)) {
		SKYCERR("%s error. brightness=%d\n", __func__, brightness);
		return -EINVAL;
	}
#ifdef F_SKYCAM_TUP_LOAD_FILE
	if((mt9p111_tup_state == MT9P111_TUNE_STATE_LOAD_VALUE) && (mt9p111_params_tbl.bright[brightness].num_params > 0))	
	{
		SKYCDBG("mt9p111_tup_state = %d, mt9p111_params_tbl.bright[0].num_params=%d\n",
			mt9p111_tup_state, brightness);
		mt9p111_i2c_write_params(mt9p111_params_tbl.bright[brightness].params,
								mt9p111_params_tbl.bright[brightness].num_params);
	}
	else
#endif		
	{
		rc = mt9p111_i2c_write_table(mt9p111_regs.bright_cfg_settings[brightness],	
					mt9p111_regs.bright_cfg_settings_size);
	}		

	if (rc < 0)
	{
		SKYCERR("CAMERA_BRIGHTNESS I2C FAIL!!! return~~\n");
		return rc;
	}		
	
	SKYCDBG("%s end\n",__func__);

	return rc;
}
#endif


#ifdef F_SKYCAM_FIX_CFG_LED_MODE
static int32_t mt9p111_set_led_mode(int32_t led_mode)
{

#if defined(CONFIG_MACH_MSM8X55_J_MASAI_E) && (BOARD_VER >= WS10)
		/* off, auto, on, torch */ 
		int rc = 0;
	
		SKYCDBG("%s: led_mode=%d\n", __func__, led_mode);
		if ((led_mode < 0) || (led_mode >= 4)) {
			SKYCERR("%s: -EINVAL, led_mode=%d\n", __func__, led_mode);
			return -EINVAL;
		}
	
		switch(led_mode)
		{
			case 0:
				// Flash off
				rc = mt9p111_gpio_out(CAM_FLASH_EN, 0);
				break;

			case 1:
			case 2:
			case 3:				
				// Flash on
				rc = mt9p111_gpio_out(CAM_FLASH_EN, 1);
				break;
				
			default:
				break;
		}
	
#endif

	return 0;
}
#endif

#ifdef F_SKYCAM_ADD_CFG_RED_LED_MODE
static int32_t mt9p111_set_red_led_mode(int32_t led_mode)
{

	/* off, auto, on, torch */ 
	int rc = 0;
	
	SKYCDBG("%s: red_led_mode=%d\n", __func__, led_mode);
	if ((led_mode < 0) || (led_mode >= 4)) {
		SKYCERR("%s: -EINVAL, red_led_mode=%d\n", __func__, led_mode);
		return -EINVAL;
	}
	
	switch(led_mode)
	{
		case 0:
			// RED led off
			rc = pm8058_set_led_current(PMIC8058_ID_LED_2, 0);
			break;

		case 1:
		case 2:
		case 3:				
			// RED led on
			rc = pm8058_set_led_current(PMIC8058_ID_LED_2, 4);
			break;
			
		default:
			break;
	}

	return 0;
}
#endif

#ifdef F_SKYCAM_FIX_CFG_AF
static int32_t mt9p111_set_auto_focus(int mode, int8_t autofocus)
{
#define AF_POLL_PERIOD	50
#define AF_POLL_RETRY	90

	int32_t rc = 0;
	int32_t i = 0;
	unsigned short read_buf = 0;

	if( autofocus == 0 || autofocus ==2)
		auto_focus = 1;
	else
		auto_focus = 0;

	
	if ((autofocus < 0) || (autofocus > 3))
	{
		SKYCERR("%s FAIL!!! return~~  autofocus = %d\n",__func__,autofocus);
		return 0;//-EINVAL;
	}

	{
		static uint16_t read_data = 0;
		int i = 0;

		// AF debug - check af mode : if mode is manual, change to "continuous mode"
		rc = mt9p111_i2c_read(mt9p111_client->addr,	0x36DE, &read_data, TRIPLE_LEN);
		if (rc < 0)
		{
			SKYCERR("AUTO FOCUS READ I2C FAIL!!! return~~\n");
			return rc;
		}

		if (read_data != 5 /* full scan mode*/)
		{
			SKYCDBG("%s current af_mode = %d\n",__func__, rc);
			rc = mt9p111_i2c_write_a2d1(0x8419, 0x05);	// SEQ_STATE_CFG_1_AF [Full_Scan_mode]
			if (rc < 0)
			{
				SKYCERR("AUTO FOCUS READ I2C FAIL!!! return~~\n");
				return rc;
			}

			rc = mt9p111_i2c_write_word(0xB004, 0x0002); // AF_ALGO [Full_Scan_mode]
			if (rc < 0)
			{
				SKYCERR("AUTO FOCUS READ I2C FAIL!!! return~~\n");
				return rc;
			}

			rc = mt9p111_i2c_write_a2d1(0x8404, 0x06);	// Refresh_mode
			if (rc < 0)
			{
				SKYCERR("AUTO FOCUS READ I2C FAIL!!! return~~\n");
				return rc;
			}

			i = 0;		// polling start

			do {
				msleep(40);
				mt9p111_i2c_read(mt9p111_client->addr, 0x8404, &read_data, TRIPLE_LEN);
				i++;
			} while ((i < 20) && (read_data));
		}
	}
	
	SKYCDBG("%s start~  mode = %d, autofocust = %d\n",__func__,mode ,autofocus);

	rc = mt9p111_i2c_write_table(&mt9p111_regs.autofocus_trigger_cfg_settings[0],
					mt9p111_regs.autofocus_trigger_cfg_settings_size);
	if (rc < 0)
	{
		SKYCERR("CAMERA_AUTO_FOCUS I2C FAIL_1!!! return~~\n");
		return rc;
	}		

	rc =	mt9p111_i2c_write(mt9p111_client->addr,
		0x098E, 0xB006,
		WORD_LEN);
	
	if (rc < 0)
	{
		SKYCERR("CAMERA_AUTO_FOCUS I2C FAIL_2!!! return~~\n");
		return rc;
	}	

	for (i = 0; i < AF_POLL_RETRY; i++) 
	{
		mdelay(AF_POLL_PERIOD);
		
		rc = mt9p111_i2c_read(mt9p111_client->addr,
				0xB006, &read_buf, 
				TRIPLE_LEN);//charlry2
				
		if (rc < 0)
		{
			SKYCERR("AUTO FOCUS READ I2C FAIL!!! return~~\n");
			return rc;
		}

		SKYCDBG("AF polling num = %d,  read_buf = %x\n", i, read_buf);

		if (read_buf == 0)
		{
			SKYCDBG("Focus done.\n");
			goto FOCUS_DONE;
		}
	}

	/* Last preview frame is not focused for some reason,
	 * so add delay here to view several frames after AF is done. */
	SKYCERR("Focus failed. read_buf = 0x%x\n", read_buf);
	/* set focus to infinity if failed */

	rc = mt9p111_i2c_write_table(mt9p111_regs.focus_step_cfg_settings[C_SKYCAM_MIN_FOCUS_STEP],	
					mt9p111_regs.focus_step_cfg_settings_size); //charley2
	
	if (rc < 0)
	{
		SKYCERR("%s FAIL!!! return~~\n",__func__);
		return rc;
	}		

#ifdef F_SKYCAM_BUG_PATCH_POSTVIEW_FOCUS
	msleep(100);
#endif
	return -1;	/* FOCUS FAIL (auto) */


FOCUS_DONE:

    rc = mt9p111_i2c_write(mt9p111_client->addr, 0x098E, 0x3000, WORD_LEN);
	if (rc < 0)
	{
		SKYCERR("polling failed. autofocus = %d, rc = %d\n", autofocus, rc);
		return rc;
	}	

	rc = mt9p111_i2c_read(mt9p111_client->addr, 0xB000, &read_buf, WORD_LEN);
	if (rc < 0)
	{
		SKYCERR("focus done. result failed. autofocus = %d, read_buf = 0x%x, rc = %d\n", autofocus, read_buf, rc);
		return rc;
	}

	if ( ( read_buf & (0x1 << 15) ) != 0 )
	{
		SKYCDBG("focus done. result failed. read_buf = 0x%x\n", read_buf);

		/* set focus to infinity if failed */
		rc = mt9p111_i2c_write_table(mt9p111_regs.focus_step_cfg_settings[C_SKYCAM_MIN_FOCUS_STEP],	
						mt9p111_regs.focus_step_cfg_settings_size); //charley2		

		if (rc < 0)
		{
			SKYCERR("%s FAIL!!! return~~\n",__func__);
			return rc;
		}		

	#ifdef F_SKYCAM_SAME_FOCUS_STEP_SKIP
		cur_focus_pos = C_SKYCAM_MIN_FOCUS_STEP;
	#endif

	#ifdef F_SKYCAM_BUG_PATCH_POSTVIEW_FOCUS
		msleep(100);
	#endif
		return -1;	/* FOCUS FAIL (auto) */
	}

	SKYCDBG("%s is done. Focus pass. read_buf = 0x%x, rc = %d\n", __func__, read_buf, rc);

#ifdef F_SKYCAM_SAME_FOCUS_STEP_SKIP
	cur_focus_pos = AF_IMPLEMENTED;
#endif

#ifdef F_SKYCAM_BUG_PATCH_POSTVIEW_FOCUS
	msleep(100);
#endif
	return 0;		/* FOCUS PASS (auto) */
	
	SKYCDBG("%s end\n",__func__);
	return rc;
}
#endif


#ifdef F_SKYCAM_FIX_CFG_ISO
static int32_t mt9p111_set_iso(int32_t iso)
{
	/* auto, N/A, ISO100, ISO200, ISO400, ISO800, ISO1600, ISO3200 */
	uint8_t data[] = {0x00, 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06};
	int32_t rc = 0;

#if 0	
//	if(mode == SENSOR_SNAPSHOT_MODE) return 0;

	if ((iso < 0) || (iso >= sizeof(data)) || (iso == 1))
		return -EINVAL;

	SKYCDBG("%s  iso = %d\n",__func__, iso);

	rc = mt9p111_i2c_write(mt9p111_client->addr, 0x17, data[iso],WORD_LEN);
#endif
	
	return rc;
}
#endif


#ifdef F_SKYCAM_FIX_CFG_SCENE_MODE
static int32_t mt9p111_set_scene_mode(int mode, int32_t scene_mode)
{
	int32_t rc = 0;

	if ((scene_mode < 0) || (scene_mode > MT9P111_CFG_SCENE_MODE_MAX))
	{
		SKYCERR("%s not support scene mode  = %d\n",__func__,scene_mode);
		return 0;//-EINVAL;
	}
		
	rc = mt9p111_i2c_write_table(mt9p111_regs.scene_mode_cfg_settings[scene_mode],	
				mt9p111_regs.scene_mode_cfg_settings_size);

	if (rc < 0)
	{
		SKYCERR("%s FAIL!!! return~~\n",__func__);
		return rc;
	}		

	SKYCDBG("%s end\n",__func__);

	return rc;
}
#endif


#ifdef F_SKYCAM_FIX_CFG_FOCUS_STEP
static int32_t mt9p111_set_focus_step(int mode, int32_t focus_step)
{
	int32_t rc = 0;

	if ((focus_step < 0) || (focus_step >= MT9P111_CFG_FOCUS_STEP_MAX))
	{
		SKYCERR("%s FAIL!!! return~~  focus_step = %d\n",__func__,focus_step);
		return 0;//-EINVAL;
	}

#ifdef F_SKYCAM_SAME_FOCUS_STEP_SKIP
		if( focus_step != cur_focus_pos)
#endif

	rc = mt9p111_i2c_write_table(mt9p111_regs.focus_step_cfg_settings[focus_step],	
					mt9p111_regs.focus_step_cfg_settings_size);

#ifdef F_SKYCAM_SAME_FOCUS_STEP_SKIP
		if(!rc) cur_focus_pos = focus_step;
#endif

	if (rc < 0)
	{
		SKYCERR("%s FAIL!!! return~~\n",__func__);
		return rc;
	}		

	SKYCDBG("%s end\n",__func__);

	return rc;
}
#endif


#ifdef F_SKYCAM_ADD_CFG_ANTISHAKE
static int32_t mt9p111_set_antishake(int32_t antishake)
{
	int32_t rc = 0;

	if ((antishake < 0) || (antishake >= MT9P111_CFG_ANTISHAKE_MAX))
	{
		SKYCERR("%s FAIL!!! return~~  antishake = %d\n",__func__,antishake);
		return 0;//-EINVAL;
	}
	
	rc = mt9p111_i2c_write_table(mt9p111_regs.antishake_cfg_settings[antishake],	
				mt9p111_regs.antishake_cfg_settings_size);

	if (rc < 0)
	{
		SKYCERR("%s FAIL!!! return~~\n",__func__);
		return rc;
	}		

	SKYCDBG("%s end\n",__func__);
	
	return rc;
}
#endif


#ifdef F_SKYCAM_FIX_CFG_EXPOSURE
static int32_t mt9p111_set_exposure_mode(int32_t exposure)
{
	int32_t rc = 0;

	SKYCDBG("%s  exposure = %d\n",__func__, exposure);

	if(auto_focus == 1)
		return 0;

	if ((exposure < 0) || (exposure >= MT9P111_CFG_EXPOSURE_MAX))
	{
		SKYCERR("%s FAIL!!! return~~  exposure = %d\n",__func__,exposure);
		return 0;//-EINVAL;
	}
	
	rc = mt9p111_i2c_write_table(mt9p111_regs.exposure_cfg_settings[exposure],
				mt9p111_regs.exposure_cfg_settings_size);
 
	if (rc < 0)
	{
		SKYCERR("CAMERA_EFFECT_SEPIA I2C FAIL!!! return~~\n");
		return rc;
	}		
	
	SKYCDBG("%s end\n",__func__);

	return rc;
}
#endif


#ifdef F_SKYCAM_FIX_CFG_FOCUS_RECT
static int32_t mt9p111_set_focus_rect(uint32_t focus_rect)
{
	uint8_t xleft, xright, ytop, ybottom;
	uint8_t nwidth, nheight;
	int32_t rc = 0;

	SKYCDBG("%s  focus_rect = %d\n",__func__, focus_rect);

	xleft   = (uint8_t)((focus_rect & 0x0f000000) >> 24);
	xright  = (uint8_t)((focus_rect & 0x000f0000) >> 16);
	ytop    = (uint8_t)((focus_rect & 0x00000f00) >> 8);
	ybottom = (uint8_t)((focus_rect & 0x0000000f));

	if ((xleft == 0) && (xright == 0) && (ytop == 0) && (ybottom == 0)) {
		xleft = 0x20;
		ytop = 0x20;
		nwidth = 0xBF;
		nheight = 0xBF;
	}
	else
	{
		xleft   = xleft*16;
		xright  = ((xright+1)*16 )- 1;
		ytop    = ytop*16;
		ybottom = ((ybottom+1)*16 )- 1;

		nwidth = xright - xleft;
		nheight = ybottom - ytop;
	}
	
	SKYCDBG("%s  xleft = %d, ytop = %d, xright = %d, ybottom = %d\n",__func__, xleft, ytop, xright, ybottom);
	SKYCDBG("%s  nwidth = %d, nheight = %d\n",__func__, nwidth, nheight);
	
	rc =	mt9p111_i2c_write(mt9p111_client->addr,	0xB854, xleft, TRIPLE_LEN);
	if (rc < 0) {
		SKYCERR("CAMERA_AUTO_FOCUS I2C FAIL_1!!! return~~\n");
		return rc;
	}
	rc =	mt9p111_i2c_write(mt9p111_client->addr,	0xB855, ytop, TRIPLE_LEN);
	if (rc < 0) {
		SKYCERR("CAMERA_AUTO_FOCUS I2C FAIL_2!!! return~~\n");
		return rc;
	}
	rc =	mt9p111_i2c_write(mt9p111_client->addr,	0xB856, nwidth, TRIPLE_LEN);
	if (rc < 0) {
		SKYCERR("CAMERA_AUTO_FOCUS I2C FAIL_3!!! return~~\n");
		return rc;
	}
	rc =	mt9p111_i2c_write(mt9p111_client->addr,	0xB857, nheight, TRIPLE_LEN);
	if (rc < 0) {
		SKYCERR("CAMERA_AUTO_FOCUS I2C FAIL_4!!! return~~\n");
		return rc;
	}
	
	rc = mt9p111_i2c_write_table(&mt9p111_regs.autofocus_rect_cfg_settings[0],
					mt9p111_regs.autofocus_rect_cfg_settings_size);
	if (rc < 0)
	{
		SKYCERR("CAMERA_AUTO_FOCUS I2C FAIL_1!!! return~~\n");
		return rc;
	}		

	return 0;
}
#endif


#ifdef F_SKYCAM_ADD_CFG_DIMENSION
static int32_t mt9p111_set_dimension(struct dimension_cfg *dimension)
{
	memcpy(&mt9p111_dimension, dimension, sizeof(struct dimension_cfg));
	SKYCDBG("%s: preview=%dx%d, snapshot=%dx%d\n", __func__,
		dimension->prev_dx, dimension->prev_dy,
		dimension->pict_dx, dimension->pict_dy);
	return 0;
}
#endif


#ifdef F_SKYCAM_FIX_CFG_ANTIBANDING
static int32_t mt9p111_set_antibanding(int32_t antibanding)
{
	int32_t rc = 0;

	if ((antibanding < 0) || (antibanding >= MT9P111_CFG_FLICKER_MAX))
	{
		SKYCERR("%s FAIL!!! return~~  antibanding = %d\n",__func__,antibanding);
		return 0;//-EINVAL;
	}

	rc = mt9p111_i2c_write_table(mt9p111_regs.flicker_cfg_settings[antibanding],	
			mt9p111_regs.flicker_cfg_settings_size);

	if (rc < 0)
	{
		SKYCERR("%s FAIL!!! return~~\n",__func__);
		return rc;
	}		

	SKYCDBG("%s end\n",__func__);
	return rc;
}
#endif


#ifdef F_SKYCAM_FIX_CFG_PREVIEW_FPS
static int32_t mt9p111_set_frame_rate(int32_t preview_fps)
{
	/*  fixed FPS : 5 ~ 20 fps */
	/* variable FPS : 8 ~ 20 fps */
	int32_t rc = 0;
	uint8_t set_fps = 0;
	
	if ( preview_fps == C_SKYCAM_VARIABLE_FPS ) 
	{
		set_fps = C_SKYCAM_MAX_PREVIEW_FPS - C_SKYCAM_MIN_PREVIEW_FPS;
		
		SKYCDBG("%s: variable_fps=%d\n", __func__, set_fps);
		rc = mt9p111_i2c_write_table(mt9p111_regs.preview_fps_cfg_settings[set_fps],
					mt9p111_regs.preview_fps_cfg_settings_size);
	}
	else
	{
		if ((preview_fps < C_SKYCAM_MIN_PREVIEW_FPS) || 
			(preview_fps > C_SKYCAM_MAX_PREVIEW_FPS)) {
			SKYCERR("%s: -EINVAL, preview_fps=%d\n", 
				__func__, preview_fps);
			return -EINVAL;
		}

		set_fps = (uint8_t)(preview_fps & 0x000000ff);

		if( set_fps > C_SKYCAM_MAX_SUPPORT_FPS) 
		{
			set_fps = C_SKYCAM_MAX_SUPPORT_FPS;
		}
		else
		{
			set_fps = set_fps - C_SKYCAM_MIN_PREVIEW_FPS;
		}	

		SKYCDBG("%s: set_fps=%d\n", __func__, set_fps);

		rc = mt9p111_i2c_write_table(mt9p111_regs.preview_fps_cfg_settings[set_fps],
					mt9p111_regs.preview_fps_cfg_settings_size);

	}

	SKYCDBG("%s end rc = %d\n",__func__, rc);

	return 0;
}
#endif


#ifdef F_SKYCAM_TUP_LOAD_FILE
static u_int32_t mt9p111_i2c_write_params
(
    mt9p111_cfg_param_t *params,
    u_int32_t num_params
)
{
	u_int16_t read_data = 0;
	u_int16_t write_data = 0;
	u_int8_t write_data_byte = 0;	
	u_int32_t i = 0;

	SKYCDBG(">>%s ", __func__);
	SKYCDBG("- params = %p, num_params = %d", params, num_params);

	if ((params == NULL) || (num_params == 0))
	{
		SKYCERR("Invalid param! [params = 0x%08x][num_params = %d]", (u_int32_t)params, num_params);
		return -EIO;    
	}

#if 0	//for serial output without i2c error
	for (i = 0; i < num_params; i++)
	{
		if (params[i].op == CAMOP_REG_BURST_MODE)
		{
			SKYCDBG("!!! BUSRST MODE !!!\n");
			{
				volatile u_int8_t *pArray = NULL;
				volatile int count = 0;

				pArray = (u_int8_t *)( (params[i].data << 16) | params[i].addr );

				while( (*(pArray + count) != 0xAB) || ( *(pArray + (count + 1) ) != 0xCD) )
				{				
					SKYCDBG("0x%x", (unsigned int)*(pArray + count));
					count++;
				}
				SKYCDBG("Bust Mode End\n"); 
			}
		}
		else
		{
			SKYCDBG("op(%d) : 0x%04x = 0x%04x\n", params[i].op, params[i].addr, params[i].data);
		}			
	}
#endif

	for (i = 0; i < num_params; i++)
	{
		switch(params[i].op)
		{
			case CAMOP_NOP:
				SKYCDBG("WRITE: NOP");
				break;

			case CAMOP_DELAY:
				SKYCDBG("WRITE: DELAY (%dms)", params[i].data);
				mdelay(params[i].data);
				break;
				
			case CAMOP_WRITE:
				if (mt9p111_i2c_write_word(params[i].addr, params[i].data) < 0)
				{
					SKYCERR("<<%s (-EIO)", __func__);
					return -EIO;
				}
				break;				

			case CAMOP_WRITE_DATA1:
				//배열의 2바이트 데이터를 1바이트로 캐스팅
				write_data_byte = (unsigned char)params[i].data;
				if (mt9p111_i2c_write_a2d1(params[i].addr, write_data_byte) < 0)
				{
					SKYCERR("<<%s (-EIO)", __func__);
					return -EIO;
				}
				break;

			case CAMOP_WRITE_BIT:
			{
				u_int16_t bit_pos = 0;
				u_int16_t bit_val = 0;

				if (mt9p111_i2c_read_word(params[i].addr, &read_data) < 0)
				{
					SKYCERR("<<%s (-EIO)", __func__);
					return -EIO;
				}

				bit_pos = (params[i].data & 0xff00) >> 8;
				bit_val = (params[i].data & 0x00ff);

				if (bit_val != 0) // set to 1
				{
					write_data = read_data | (0x0001 << bit_pos);
					SKYCDBG("set 0x%04x[%d] to 1", params[i].addr, bit_pos);
				}
				else // set to 0
				{
					write_data = read_data & (~(0x0001 << bit_pos));
					SKYCDBG("set 0x%04x[%d] to 0", params[i].addr, bit_pos);
				}

				if (mt9p111_i2c_write_word(params[i].addr, write_data) < 0)
				{
					SKYCERR("<<%s (-EIO)", __func__);
					return -EIO;
				}
				break;
			}

			case CAMOP_POLL_REG:
			{
				u_int16_t poll_delay = 0;
				u_int16_t poll_retry = 0;
				u_int16_t poll_reg = 0;
				u_int16_t poll_data = 0;
				u_int16_t poll_mask = 0;
				u_int16_t retry_cnt = 0;

				if (params[i+1].op != CAMOP_POLL_REG)
				{
					SKYCERR("<<%s (-EIO)", __func__);
					return -EIO;
				}

				poll_delay = (params[i+1].data & 0xff00) >> 8;
				poll_retry = (params[i+1].data & 0x00ff);
				poll_reg = params[i].addr;
				poll_data = params[i+1].addr;
				poll_mask = params[i].data;;

				SKYCDBG("start polling register... if [0x%04x] AND 0x%04x equals 0x%04x, then stop polling", poll_reg, poll_mask, poll_data);

				for (retry_cnt = 0; retry_cnt < poll_retry; retry_cnt++)
				{
					if (mt9p111_i2c_read_word(poll_reg, &read_data) < 0)
					{
						SKYCERR("<<%s (-EIO)", __func__);
						return -EIO;
					}

					if ((read_data & poll_mask) != poll_data)
					{
						SKYCDBG("retry polling register... after sleeping %d ms", poll_delay);
						mdelay(poll_delay);
					}
					else
					{
						SKYCDBG("stop polling register... retried %d/%d time(s) (delay = %d ms)", retry_cnt, poll_retry, poll_delay);
						break;
					}
				}

				if (retry_cnt == poll_retry)
				{
					SKYCERR("<<%s (-EIO)", __func__);
					return -EIO;
				}

				++i;
				break;
			}

			case CAMOP_POLL_MCU_VAR:
			{
				u_int16_t poll_delay = 0;
				u_int16_t poll_retry = 0;
				u_int16_t poll_mcu_var = 0;
				u_int16_t poll_data = 0;
				u_int16_t poll_mask = 0;
				u_int16_t retry_cnt = 0;

				if (params[i+1].op != CAMOP_POLL_MCU_VAR)
				{
					SKYCERR("<<%s (-EIO)", __func__);
					return -EIO;
				}

				poll_delay = (params[i+1].data & 0xff00) >> 8;
				poll_retry = (params[i+1].data & 0x00ff);
				poll_mcu_var = params[i].addr;
				poll_data = params[i+1].addr;
				poll_mask = params[i].data;;

				SKYCDBG("start polling MCU variable... if [0x%04x] AND 0x%04x equals 0x%04x, then stop polling", poll_mcu_var, poll_mask, poll_data);


				for (retry_cnt = 0; retry_cnt < poll_retry; retry_cnt++)
				{
#ifdef MT9P111_MODULE_ABNORMAL_OPERATION_DEFENCE                	
					if (mt9p111_i2c_read_word(poll_mcu_var, &read_data) < 0)
					{
						SKYCERR("<<POLL_MCU_VAR mt9p111_i2c_read_word (FALSE)\n");
						return -EIO;
					}
#else               
					if (mt9p111_i2c_write_word(MT9P111_REG_MCU_ADDR, poll_mcu_var) < 0)
					{
						SKYCERR("<<%s (-EIO)", __func__);
						return -EIO;
					}

					if (mt9p111_i2c_read_word(MT9P111_REG_MCU_DATA, &read_data) < 0)
					{
						SKYCERR("<<%s (-EIO)", __func__);
						return -EIO;
					}
#endif

					if ((read_data & poll_mask) != poll_data)
					{
						//SKYCDBG("retry polling MCU variable... after sleeping %d ms", poll_delay);//charley2
						mdelay(poll_delay);
					}
					else
					{
						SKYCDBG("stop polling MCU variable... retried %d/%d time(s) (delay = %d ms)", retry_cnt, poll_retry, poll_delay);
						break;
					}
				}

				if (retry_cnt == poll_retry)
				{
#ifdef MT9P111_MODULE_ABNORMAL_OPERATION_DEFENCE						
					SKYCDBG("<<RETRY FAIL!! read_data = %x (FALSE)\n", read_data);
					return MT9P111_REG_POLLING_ERROR;
#else
					SKYCERR("<<%s (-EIO)", __func__);
					return -EIO;
#endif						  
				}
				++i;
				break;
			}

			case CAMOP_REG_BURST_MODE:
			{
				volatile u_int8_t *pArray = NULL;
				volatile int count = 0;

				pArray = (u_int8_t *)( (params[i].data << 16) | params[i].addr );

				while( (*(pArray + count) != 0xAB) || ( *(pArray + (count + 1) ) != 0xCD) )
				{
					count++;
				}

				//if (mt9p111_i2c_txdata(mt9p111_client->addr, (unsigned char *)pArray, (uint16_t)(count - 1)) < 0)
				//{
				//	SKYCERR(": CAMOP_REG_BURST_MODE failed!\n");
				//	return -EIO;
				//}

				{
					uint16_t laddr = 0x886c;

					uint8_t *base = (uint8_t *)pArray;	//mt9p111_regs.init_patch_burst_settings;
					uint16_t len = (uint16_t)(count - 1);	//mt9p111_regs.init_patch_burst_settings_size;
					uint16_t pos = 0;

					uint8_t	blk_dat[2 + 128]; 	// address 2 bytes + data 128 bytes
					uint16_t blk_len = 0;

					while (pos < len) 
					{
						blk_len = ((len - pos) >= 128) ? 128 : (len - pos);
						blk_dat[0] = (uint8_t)((laddr & 0xff00) >> 8);
						blk_dat[1] = (uint8_t)(laddr & 0x00ff);

						memcpy(&blk_dat[2], &base[pos], blk_len);
						if (mt9p111_i2c_txdata(mt9p111_client->addr, blk_dat, 2 + blk_len) < 0) {
							SKYCERR("burst write failed\n");
							break;
						}

						laddr += blk_len;
						pos += blk_len;
					}
				}

				SKYCDBG("CAMOP_REG_BURST_MODE done. count = %d\n", (count - 1));
				//++i; //bug
				break;
			}

#ifdef F_SKYCAM_ADD_CFG_OTP_MODULE
			case CAMOP_REG_OTP_MODULE_ZONE_DETECT_MODE:
			{
	#if !(defined(CONFIG_MACH_MSM8X55_J_MASAI_E) && (BOARD_VER >= WS20)) //charley2
				uint16_t otp_retry_cnt = 0;
				uint16_t otp_poll_retry = 20;
	#endif
				mdelay(100); //안정화 delay
	#if defined(CONFIG_MACH_MSM8X55_J_MASAI_E) && (BOARD_VER >= WS20) //charley2
		#ifdef F_SKYCAM_SENSOR_BUG
				if (mt9p111_i2c_write_word(0x381C, 0x0000) < 0)
				{
					SKYCERR("<<POLL_MCU_VAR mt9p111_i2c_read_word (-EIO)\n");
					return -EIO;
				}

				if (mt9p111_i2c_write_word(0xE02A, 0x0001) < 0) 
				{
					SKYCERR("<<POLL_MCU_VAR mt9p111_i2c_read_word (FALSE)\n");
					return -EIO;
				}				 
		#endif
	#else
		#ifdef F_SKYCAM_SENSOR_BUG
				if (mt9p111_i2c_write_word(0x3812, 0x2124) < 0)
				{
					SKYCERR("<<POLL_MCU_VAR mt9p111_i2c_read_word (-EIO)\n");
					return -EIO;
				}
		#endif
	#endif

				if (mt9p111_i2c_read_word(0xE023, &read_otp_zone_fail) < 0)
				{
					SKYCERR("<<POLL_MCU_VAR mt9p111_i2c_read_word (FALSE)\n");
					return -EIO;
				}

				if ((read_otp_zone_fail & 0xff00) != 0xC100)
				{
					SKYCDBG("OTP ERROR !!!, read_data=%2x\n", read_otp_zone_fail);
					return MT9P111_REG_POLLING_ERROR;			
				}
				else
				{
	#if defined(CONFIG_MACH_MSM8X55_J_MASAI_E) && (BOARD_VER >= WS20) //charley2
					mt9p111_i2c_write_a2d1(0xD004, 0x04);
					mt9p111_i2c_write_word(0xD006, 0x0008); 
					mt9p111_i2c_write_a2d1(0xD005, 0x00);
					mt9p111_i2c_write_word(0xD002, 0x8002); 
					mt9p111_i2c_write_word(0x3210, 0x49B8); 
	#else

					SKYCDBG("OTP_SELECT_MODE!!!, read_data=%2x\n", read_data);
					mt9p111_i2c_write_word(0xE024, 0x0100);
					mt9p111_i2c_write_word(0xE02A, 0xA010);
					mt9p111_i2c_write_a2d1(0x8404, 0x05);

					for (otp_retry_cnt = 0; otp_retry_cnt < otp_poll_retry; otp_retry_cnt++)
					{
						if (mt9p111_i2c_read_word(0x8404, &read_data) < 0)
						{
							SKYCERR("<<POLL_MCU_VAR mt9p111_i2c_read_word (FALSE)\n");
							return -EIO;
						}

						if ((read_data & 0xff00) != 0)
						{
							//SKYCDBG("retry polling MCU variable... after sleeping %d ms, read_data=%2x\n", 50, read_data);
							mdelay(50);
						}
						else
						{
							SKYCDBG("stop polling MCU variable... retried %d/%d time(s) (delay = %d ms), read_data=%2x\n", otp_retry_cnt, otp_poll_retry, 50, read_data);
							break;
						}
					}

					if (otp_retry_cnt == otp_poll_retry)
					{
						SKYCDBG("<<RETRY FAIL!! read_data = %x (FALSE)\n", read_data);
		#ifdef MT9P111_MODULE_ABNORMAL_OPERATION_DEFENCE												
						return MT9P111_REG_POLLING_ERROR;
		#endif
					}
	#endif						
				}

				//===================================================================================			
	#if !(defined(CONFIG_MACH_MSM8X55_J_MASAI_E) && (BOARD_VER >= WS20)) //charley2

				if ((read_otp_zone_fail & 0xff00) == 0xC100)
				{
					SKYCERR("<<OTP_ZONE_SELECT!!! read_otp_zone_fail=%x\n", read_otp_zone_fail);
					if (mt9p111_i2c_read_word(MT9P111_REG_OTP_DETECT_ZONE_READ, &read_otp_zone) < 0)
					{
						SKYCERR("<<MT9P111_REG_OTP_DETECT_ZONE_READ FAIL!! mt9p111_i2c_read_word (FALSE)\n");
						return -EIO;
					}

					if(read_otp_zone == 0)
					{
						SKYCDBG("<<OTP_MODULE_ZONE_SET : ZONE 0 [read_otp_zone]=%x\n", read_otp_zone);
						if (mt9p111_i2c_write_a2d1(MT9P111_REG_OTP_SET_ZONE_SOLUTION, 0x04) < 0)
						{
							SKYCERR("<<MT9P111_REG_OTP_SET_ZONE_SOLUTION FAIL!! mt9p111_i2c_write_word (FALSE)\n");
							return -EIO;
						}
						
						if (mt9p111_i2c_write_word(MT9P111_REG_OTP_SET_ZONE_ADDR, 0) < 0)
						{
							SKYCERR("<<MT9P111_REG_OTP_DETECT_ZONE_ADDR FAIL!! mt9p111_i2c_write_word (FALSE)\n");
							return -EIO;
						}
						
						if (mt9p111_i2c_write_a2d1(MT9P111_REG_OTP_SET_ZONE_CURRENT, 0) < 0)
						{
							SKYCERR("<<MT9P111_REG_OTP_SET_ZONE_CURRENT FAIL!! mt9p111_i2c_write_word (FALSE)\n");
							return -EIO;
						}
						
						if (mt9p111_i2c_write_word(MT9P111_REG_OTP_SET_ZONE_PGA_ALGO, 0x8002) < 0)
						{
							SKYCERR("<<MT9P111_REG_OTP_SET_ZONE_PGA_ALGO FAIL!! mt9p111_i2c_write_word (FALSE)\n");
							return -EIO;
						}
						
						if (mt9p111_i2c_write_word(MT9P111_REG_OTP_SET_ZONE_LENS_ENABLE, 0x49B8) < 0)
						{
							SKYCERR("<<MT9P111_REG_OTP_SET_ZONE_LENS_ENABLE FAIL!! mt9p111_i2c_write_word (FALSE)\n");
							return -EIO;
						}
					}
					else
					{

						SKYCDBG("<<OTP_MODULE_ZONE_SET : ZONE 1 [read_otp_zone]=%x\n", read_otp_zone);
						if (mt9p111_i2c_write_a2d1(MT9P111_REG_OTP_SET_ZONE_SOLUTION, 0x04) < 0)
						{
							SKYCERR("<<MT9P111_REG_OTP_SET_ZONE_SOLUTION FAIL!! mt9p111_i2c_write_word (FALSE)\n");
							return -EIO;
						}
						
						if (mt9p111_i2c_write_word(MT9P111_REG_OTP_SET_ZONE_ADDR, 0x0100) < 0)
						{
							SKYCERR("<<MT9P111_REG_OTP_DETECT_ZONE_ADDR FAIL!! mt9p111_i2c_write_word (FALSE)\n");
							return -EIO;
						}
						
						if (mt9p111_i2c_write_a2d1(MT9P111_REG_OTP_SET_ZONE_CURRENT, 0x01) < 0)
						{
							SKYCERR("<<MT9P111_REG_OTP_SET_ZONE_CURRENT FAIL!! mt9p111_i2c_write_word (FALSE)\n");
							return -EIO;
						}
						
						if (mt9p111_i2c_write_word(MT9P111_REG_OTP_SET_ZONE_PGA_ALGO, 0x8002) < 0)
						{
							SKYCERR("<<MT9P111_REG_OTP_SET_ZONE_PGA_ALGO FAIL!! mt9p111_i2c_write_word (FALSE)\n");
							return -EIO;
						}
						
						if (mt9p111_i2c_write_word(MT9P111_REG_OTP_SET_ZONE_LENS_ENABLE, 0x49B8) < 0)
						{
							SKYCERR("<<MT9P111_REG_OTP_SET_ZONE_LENS_ENABLE FAIL!! mt9p111_i2c_write_word (FALSE)\n");
							return -EIO;
						}
					}
				}
	#endif
				break;
			}
#endif/*F_SKYCAM_ADD_CFG_OTP_MODULE*/

			default:
				SKYCERR("Invalid op code! [op = %d]", params[i].op);
				return -EIO;
				break;
		}
	}

	SKYCDBG("<<%s (TRUE)", __func__);
	return TRUE;
}


static void mt9p111_init_params_tbl (int8_t *stream)
{
#if defined(F_SKYCAM_SENSOR_TUNEUP)
	mt9p111_cfg_param_t *params = NULL;
	u_int32_t num_params = 0;
#endif /* F_SKYCAM_SENSOR_TUNEUP */
	u_int32_t i = 0;

	SKYCDBG(">>%s ()", __func__);

#if defined(F_SKYCAM_SENSOR_TUNEUP)
	camsensor_mt9p111_tup_init(stream);
	num_params = camsensor_mt9p111_tup_get_init_params(&params);
	if ((num_params > 0) && (params != NULL))
	{
		mt9p111_params_tbl.init.num_params = num_params;
		mt9p111_params_tbl.init.params = params;
	}
	else
#endif /* F_SKYCAM_SENSOR_TUNEUP */
	{
		mt9p111_params_tbl.init.num_params = num_params;
		mt9p111_params_tbl.init.params = params;

	}

	for (i = 0; i < MT9P111_CFG_WB_MAX; i++)
	{
#if defined(F_SKYCAM_SENSOR_TUNEUP)
		num_params = camsensor_mt9p111_tup_get_wb_params(i, &params);
		if ((num_params > 0) && (params != NULL))
		{
			mt9p111_params_tbl.wb[i].num_params = num_params;
			mt9p111_params_tbl.wb[i].params = params;
		}
		else
#endif /* F_SKYCAM_SENSOR_TUNEUP */
		{        
			mt9p111_params_tbl.wb[i].num_params = num_params;
			mt9p111_params_tbl.wb[i].params = params;
		}
	}

	for (i = 0; i < MT9P111_CFG_BRIGHT_MAX; i++)
	{
#if defined(F_SKYCAM_SENSOR_TUNEUP)
		num_params = camsensor_mt9p111_tup_get_bright_params(i, &params);
		if ((num_params > 0) && (params != NULL))
		{
			mt9p111_params_tbl.bright[i].num_params = num_params;
			mt9p111_params_tbl.bright[i].params = params;
		}
		else
#endif /* F_SKYCAM_SENSOR_TUNEUP */
		{
			mt9p111_params_tbl.bright[i].num_params = num_params;
			mt9p111_params_tbl.bright[i].params = params;		
		}
	}

	for (i = 0; i < MT9P111_CFG_EXPOSURE_MAX; i++)
	{
		mt9p111_params_tbl.exposure[i].num_params = num_params;
		mt9p111_params_tbl.exposure[i].params = params;
	}

	for (i = MT9P111_CFG_FFPS_MIN; i <= MT9P111_CFG_FFPS_MAX; i++)
	{
		mt9p111_params_tbl.ffps[i - MT9P111_CFG_FFPS_MIN].num_params = num_params;
		mt9p111_params_tbl.ffps[i - MT9P111_CFG_FFPS_MIN].params = params;	
	}

	for (i = 0; i < MT9P111_CFG_REFLECT_MAX; i++)
	{
		mt9p111_params_tbl.reflect[i].num_params = num_params;
		mt9p111_params_tbl.reflect[i].params = params;
	}

	for (i = 0; i < MT9P111_CFG_EFFECT_MAX; i++)
	{
		mt9p111_params_tbl.effect[i].num_params = num_params;
		mt9p111_params_tbl.effect[i].params = params;
	}

	for (i = 0; i < MT9P111_CFG_FLICKER_MAX; i++)
	{
		mt9p111_params_tbl.flicker[i].num_params = num_params;
		mt9p111_params_tbl.flicker[i].params = params;
	}

	for (i = 0; i < MT9P111_CFG_ANTISHAKE_MAX; i++)
	{
		mt9p111_params_tbl.antishake[i].num_params = num_params;
		mt9p111_params_tbl.antishake[i].params = params;
	}

	for (i = 0; i < MT9P111_CFG_FOCUS_STEP_MAX; i++)
	{
		mt9p111_params_tbl.focus_step[i].num_params = num_params;
		mt9p111_params_tbl.focus_step[i].params = params;
	}

	for (i = 0; i < MT9P111_CFG_SCENE_MODE_MAX; i++)
	{
		mt9p111_params_tbl.scene[i].num_params = num_params;
		mt9p111_params_tbl.scene[i].params = params;
	}    

	SKYCDBG("<<%s ()", __func__);
	return;
}
#endif


#ifdef F_SKYCAM_TUP_LOAD_FILE
static int mt9p111_set_tune_value(const struct sensor_cfg_data *scfg)
{
	int32_t rc = 0;
	int32_t loop_count = 0;
	int32_t i = 0;
	int32_t last_size = 0;
	int32_t copy_size = 0;
	uint8_t * pKTune_stream = NULL;
	uint8_t * pUTune_stream = NULL;
	
	if(mt9p111_tup_state != MT9P111_TUNE_STATE_LOAD_VALUE)
	{
		SKYCDBG("%s start\n",__func__);
		
		pKTune_stream = kmalloc(scfg->cfg.tune_value.file_size, GFP_KERNEL);
		SKYCDBG("pKTune_stream =%p\n", pKTune_stream);
		if (!pKTune_stream) {
			SKYCERR("failed to malloc.\n");
			rc = -ENOMEM;
			goto update_fw_user_fail;
		}

		//유저인터페이스에서 튜닝 파일 스트림을 얻어옴	
		pUTune_stream = scfg->cfg.tune_value.pfile_stream;
		//페이지 단위 사이즈로 나누어 loop_count 저장
		loop_count = (scfg->cfg.tune_value.file_size / 4096) + 1; 
		last_size = scfg->cfg.tune_value.file_size % 4096;
		copy_size = 4096;

		SKYCDBG("scfg->cfg.tune_value.pfile_stream=%p, scfg->cfg.tune_value.file_size=%d\n", scfg->cfg.tune_value.pfile_stream, scfg->cfg.tune_value.file_size);
		for(i = 0; i < loop_count; i++)
		{
			//마지막 루프일 경우 남은 사이즈로 변경
			if(i == (loop_count-1))
				copy_size = last_size;
			else
				copy_size = 4096;

			//20110413_ysgeum_ps3  마지막 루프일 경우 copy_size가 4096보다 작아지므로 페이지 단위 사이즈는 상수 값으로 고정
			//if (copy_from_user(pKTune_stream + i*copy_size, (void *)pUTune_stream +	i*copy_size, copy_size))
			if (copy_from_user(pKTune_stream + i*4096, (void *)pUTune_stream +  i*4096, copy_size))			
			{                
				rc = -EFAULT;
				goto update_fw_user_fail;
	        	}
			//SKYCDBG(" i =%d, loop_count=%d, copy_size=%d, pKTune_stream=%x\n", i, loop_count, copy_size, pKTune_stream);
		}

		//파일 스트림에서 개별 튜닝값을 배열에 로딩
		mt9p111_init_params_tbl(pKTune_stream);

		mt9p111_tup_state = MT9P111_TUNE_STATE_LOAD_VALUE;
	}
		
#if 0	
	if (pKTune_stream)
		kfree(pKTune_stream);
#endif
	SKYCDBG("%s done.\n", __func__);
	return 0;

update_fw_user_fail:
	if (pKTune_stream)
	{
		kfree(pKTune_stream);
		pKTune_stream = NULL;
	}
	SKYCERR("%s error. rc=%d\n", __func__, rc);
	return rc;
}
#endif


static int32_t mt9p111_video_config(void)
{
	int32_t rc = 0;
#if 0 //charley2
	uint16_t npolling = 0;
	int i;
#endif

#ifdef F_SKYCAM_CHECK_IS_INIT_SEQ_DONE
	if (init_seq_done == false)
	{
		init_seq_done = true;
		SKYCDBG("%s end rc = %d\n",__func__, rc);
		return 0;
	}
	else
	{
#endif
		/* set preview resolution to 1280x960 */
		SKYCDBG("%s start\n",__func__);

#ifdef F_SKYCAM_TUP_LOAD_FILE
		if(mt9p111_tup_state == MT9P111_TUNE_STATE_LOAD_VALUE)
		{		
		/*20110420_ysgeum_ps3 for initialize multi setting error on J-MASAI_E tunning binary*/
			if (mt9p111_tup_mode == MT9P111_TUNE_STATE_TUNNING_MODE_OFF)
			{
		
				rc = mt9p111_reg_init();
				if (rc < 0)
				{
					SKYCERR("mt9p111_i2c_write_table FAIL!!! return~~\n");
					return rc;
				}		
			}
			mt9p111_tup_mode = MT9P111_TUNE_STATE_TUNNING_MODE_ON;			
		
			SKYCDBG("!!! /* MT9P111_INIT */ tunning file setting. num_params = %d !!!\n", mt9p111_params_tbl.init.num_params);
		}
		else
		{		
#endif		

			rc = mt9p111_i2c_write_table(&mt9p111_regs.preview_cfg_settings[0],
							mt9p111_regs.preview_cfg_settings_size);

#if 0//charley2
			for (i = 0; i < MT9P111_PREVIEW_RETRY; i++) 
			{
				rc = mt9p111_i2c_read(mt9p111_client->addr,	0x8404, &npolling,TRIPLE_LEN);
				if (rc < 0)
				{
					SKYCERR("mt9p111_snapshot_config read  FAIL!!! return~~\n");
					//return rc;
				}
				SKYCDBG("%s: retry npolling = %x, count = %d\n", __func__,npolling, i);

				if (npolling == 0) {	
					return 0;
				}		
				msleep(20);
			}
#endif

#ifdef F_SKYCAM_TUP_LOAD_FILE
		}
#endif

		if (rc < 0)
		{
			SKYCERR("mt9p111_i2c_write_table FAIL!!! return~~\n");
			return rc;
		}
		SKYCDBG("%s end rc = %d\n",__func__, rc);

#ifdef MT9P111_J_MASAI_E_WBPOINT
		rc =	mt9p111_write_pga_table(0, 0);
		if (rc < 0)
		{
			SKYCERR("%s : mt9p111_cal_pga_init. rc = %d\n", __func__, rc);
			return rc;
		}	
#endif

#ifdef F_SKYCAM_CHECK_IS_INIT_SEQ_DONE
	}
#endif

	return 0;
}


static int32_t mt9p111_snapshot_config(void)
{
	int32_t rc = 0;
#if 0 //charley2	
	uint16_t npolling = 0;
	int i;
#endif

#ifdef MT9P111_J_MASAI_E_WBPOINT
	uint16_t exp_index, ccm_pos;
	
	rc = mt9p111_i2c_read(mt9p111_client->addr, 0x3012, &exp_index, WORD_LEN);
	if (rc < 0)
	{
		SKYCERR("%s : 0x3012 fail. rc = %d\n", __func__, rc);
		return rc;
	}	
	
	rc = mt9p111_i2c_write_word(0x098E, 0xAC08);
	if (rc < 0)
	{
		SKYCERR("%s : 0x098E fail. rc = %d\n", __func__, rc);
		return rc;
	}	
	
	rc = mt9p111_i2c_read(mt9p111_client->addr, 0xAC08, &ccm_pos, WORD_LEN);
	if (rc < 0)
	{
		SKYCERR("%s : 0xAC08 fail. rc = %d\n", __func__, rc);
		return rc;
	}	
	
	ccm_pos = (ccm_pos >> 8) & 0xFF;	
	rc = mt9p111_write_pga_table(exp_index, ccm_pos);
	if (rc < 0)
	{
		SKYCERR("%s : mt9p111_write_pga_table fail. rc = %d\n", __func__, rc);
		return rc;
	}	
#endif /* MT9P111_J_MASAI_E_WBPOINT */

	/* set snapshot resolution to 2592x1944 */
	SKYCDBG("%s start\n",__func__);

	rc = mt9p111_i2c_write_table(&mt9p111_regs.snapshot_cfg_settings[0],
					mt9p111_regs.snapshot_cfg_settings_size);
	
#if 0//charley2
	for (i = 0; i < MT9P111_SNAPSHOT_RETRY; i++)
    {
		rc = mt9p111_i2c_read(mt9p111_client->addr,
		0x8404, &npolling,TRIPLE_LEN);
		if (rc < 0)
		{
			SKYCERR("mt9p111_snapshot_config read  FAIL!!! return~~\n");
			//return rc;
		}
		SKYCDBG("%s: retry npolling = %x, count = %d\n", __func__,npolling, i);
		if (npolling == 0) {	
			return 0;
		}		
		msleep(20);
	}	
#endif	
#if 0
	mt9p111_i2c_read_table(&mt9p111_regs.snapshot_cfg_settings[0],
					mt9p111_regs.snapshot_cfg_settings_size);
#endif
	if (rc < 0)
	{
		SKYCERR("mt9p111_i2c_write_table FAIL!!! return~~\n");
		return rc;
	}

	SKYCDBG("%s end rc = %d\n",__func__, rc);
	
	return 0;
}


static int32_t mt9p111_set_sensor_mode(int mode)
{
	int32_t rc = 0;
	SKYCDBG("%s start\n",__func__);

	switch (mode) {
		
		case SENSOR_PREVIEW_MODE:
			rc = mt9p111_video_config();
			multi_count = 0;
			SKYCDBG("mt9p111_video_config end multi count =%d, rc = %d\n",  multi_count, rc);
			break;

		case SENSOR_SNAPSHOT_MODE:
			/* Switch to lower fps for Snapshot */
			if(multi_count ==0)
			rc = mt9p111_snapshot_config();
			
			SKYCDBG("mt9p111_snapshot_config end multi count =%d, rc = %d\n", multi_count, rc);
			multi_count++;
			break;

		case SENSOR_RAW_SNAPSHOT_MODE:
			/* Setting the effect to CAMERA_EFFECT_OFF */
			rc = mt9p111_snapshot_config();		
			break;

		default:
			SKYCERR("mt9p111_set_sensor_mode DEFAULT FAIL[EINVAL] = %d\n", EINVAL);
			return -EINVAL;
	}

	SKYCDBG("%s end rc = %d\n",__func__, rc);
	return rc;
}


static int mt9p111_sensor_init_probe(const struct msm_camera_sensor_info *data)
{
	int rc = 0;

	SKYCDBG("init entry \n");

#ifdef F_SKYCAM_EF18K_ES01
	rc = mt9p111_reset(data, ON);
	if (rc < 0) {
		SKYCERR("reset failed!\n");
		goto init_probe_fail;
	}
	mdelay(5);
#endif

#ifdef F_SKYCAM_TUP_LOAD_FILE
	if(mt9p111_tup_mode == MT9P111_TUNE_STATE_TUNNING_MODE_OFF)
	{
		SKYCDBG("Open probe initialize setting skip. (MT9P111_TUNNING_MODE)\n");
		return rc;
	}
#endif	

	rc = mt9p111_reg_init();
	if (rc < 0)
		goto init_probe_fail;

	return rc;

init_probe_fail:
	SKYCERR("%s failed! (%d)\n", __func__, rc);
	return rc;
}


static int mt9p111_sensor_init(const struct msm_camera_sensor_info *data)
{
	int rc = 0;

#ifdef F_SKYCAM_CHECK_IS_INIT_SEQ_DONE
	init_seq_done = false;
#endif

	rc = mt9p111_power_on();
	if (rc) {
		rc = -ENOENT;
		SKYCERR(" mt9p111_power failed rc=%d\n",rc);
		goto init_fail;
	}
	
	mt9p111_ctrl = (struct mt9p111_ctrl_t *)kzalloc(sizeof(struct mt9p111_ctrl_t), GFP_KERNEL);

	if (!mt9p111_ctrl) {
		SKYCERR("mt9p111_init failed!\n");
		rc = -ENOMEM;
		goto init_done;
	}

	if (data)
		mt9p111_ctrl->sensordata = data;

	/* Input MCLK = 24MHz */
	msm_camio_clk_rate_set(24000000);
	mdelay(5);

	msm_camio_camif_pad_reg_reset();

	rc = mt9p111_sensor_init_probe(data);
	SKYCDBG(" mt9p111_sensor_init_probe(data); rc =%d\n",rc);
	if (rc < 0) {
		SKYCERR("mt9p111_sensor_init failed!\n");
		goto init_fail;
	}

#ifdef F_SKYCAM_SAME_FOCUS_STEP_SKIP
	cur_focus_pos = C_SKYCAM_MIN_FOCUS_STEP;
#endif

	multi_count = 0;

init_done:
	SKYCDBG("%s init_done\n",__func__);
	return rc;

init_fail:
	SKYCERR("%s failed! (%d)\n", __func__, rc);
	(void)mt9p111_power_off();
	kfree(mt9p111_ctrl);
	return rc;
}


static int mt9p111_init_client(struct i2c_client *client)
{
	/* Initialize the MSM_CAMI2C Chip */
	init_waitqueue_head(&mt9p111_wait_queue);
	return 0;
}


static int mt9p111_sensor_config(void __user *argp)
{
	struct sensor_cfg_data cfg_data;
	int32_t   rc = 0;

	if (copy_from_user(&cfg_data, (void *)argp,
			sizeof(struct sensor_cfg_data))) {
		SKYCERR("%s#%d: -EFAULT.\n", __func__, __LINE__);			
		return -EFAULT;
	}

	SKYCDBG("%s (%d): mt9p111_sensor_config() \n", __func__, __LINE__);

	/* down(&mt9p111_sem); */

	switch (cfg_data.cfgtype) {
		case CFG_SET_MODE:
			rc = mt9p111_set_sensor_mode(cfg_data.mode);
			SKYCDBG("mt9p111_set_sensor_mode OK! rc = [%d], cfg_data.mode = [%d]\n", rc, cfg_data.mode);
			break;

		case CFG_SET_EFFECT:
			rc = mt9p111_set_effect(cfg_data.mode, cfg_data.cfg.effect);
			SKYCDBG("mt9p111_set_effect OK! rc = [%d], cfg_data.mode = [%d], cfg_data.cfg.effect =[%d]\n", rc, cfg_data.mode, cfg_data.cfg.effect);			
			break;
			
#ifdef F_SKYCAM_FIX_CFG_WB
		case CFG_SET_WB:			
			rc = mt9p111_set_whitebalance(cfg_data.mode, cfg_data.cfg.wb);
			SKYCDBG("mt9p111_set_whitebalance OK! rc = [%d], cfg_data.mode = [%d], cfg_data.cfg.whitebalance =[%d]\n", rc, cfg_data.mode, cfg_data.cfg.wb);
			break;
#endif					
#ifdef F_SKYCAM_FIX_CFG_BRIGHTNESS
		case CFG_SET_BRIGHTNESS:
			rc = mt9p111_set_brightness(cfg_data.cfg.brightness);
			SKYCDBG("mt9p111_set_brightness OK! rc = [%d], cfg_data.cfg.brightness =[%d]\n", rc, cfg_data.cfg.brightness);
			break;
#endif			
#ifdef F_SKYCAM_FIX_CFG_LED_MODE
		case CFG_SET_LED_MODE:
			rc = mt9p111_set_led_mode(cfg_data.cfg.led_mode);
			SKYCDBG("mt9p111_set_led_mode OK! rc = [%d], cfg_data.cfg.led_mode =[%d]\n", rc, cfg_data.cfg.led_mode);
			break;
#endif
#ifdef F_SKYCAM_ADD_CFG_RED_LED_MODE
		case CFG_SET_RED_LED_MODE:
			rc = mt9p111_set_red_led_mode(cfg_data.cfg.red_led_mode);
			SKYCDBG("mt9p111_set_red_led_mode OK! rc = [%d], cfg_data.cfg.red_led_mode =[%d]\n", rc, cfg_data.cfg.red_led_mode);
			break;
#endif
#ifdef F_SKYCAM_FIX_CFG_AF
		case CFG_AUTO_FOCUS:		
			rc = mt9p111_set_auto_focus(cfg_data.mode, cfg_data.cfg.focus.dir);
			SKYCDBG("mt9p111_auto_focus OK! rc = [%d], cfg_data.mode = [%d], cfg_data.cfg.focus.dir =[%d]\n", rc, cfg_data.mode, cfg_data.cfg.focus.dir);
			break;
#endif
#ifdef F_SKYCAM_FIX_CFG_ISO
	case CFG_SET_ISO:
			rc = mt9p111_set_iso(cfg_data.cfg.iso);
			SKYCDBG("mt9p111_set_scene_mode OK! rc = [%d], cfg_data.cfg.iso = [%d]\n", rc, cfg_data.cfg.iso);
			break;	
#endif
#ifdef F_SKYCAM_FIX_CFG_SCENE_MODE
	case CFG_SET_SCENE_MODE:
			rc = mt9p111_set_scene_mode(cfg_data.mode, cfg_data.cfg.scene_mode);
			SKYCDBG("mt9p111_set_scene_mode OK! rc = [%d], cfg_data.mode = [%d], cfg_data.cfg.scene_mode =[%d]\n", rc, cfg_data.mode, cfg_data.cfg.scene_mode);
			break;	
#endif
#ifdef F_SKYCAM_FIX_CFG_FOCUS_STEP
	case CFG_SET_FOCUS_STEP:
			rc = mt9p111_set_focus_step(cfg_data.mode, cfg_data.cfg.focus_step);
			SKYCDBG("mt9p111_set_focus_step OK! rc = [%d], cfg_data.mode = [%d], cfg_data.cfg.focus_step =[%d]\n", rc, cfg_data.mode, cfg_data.cfg.focus_step);
			break;
#endif
#ifdef F_SKYCAM_ADD_CFG_ANTISHAKE
	case CFG_SET_ANTISHAKE:
			rc = mt9p111_set_antishake(cfg_data.cfg.antishake);
			SKYCDBG("mt9p111_set_antishake OK! rc = [%d], cfg_data.cfg.antishake = [%d]\n", rc, cfg_data.cfg.antishake);			
			break;
#endif
#ifdef F_SKYCAM_FIX_CFG_EXPOSURE
		case CFG_SET_EXPOSURE_MODE:
			rc = mt9p111_set_exposure_mode(cfg_data.cfg.exposure);
			SKYCDBG("mt9p111_set_exposure_mode OK! rc = [%d], cfg_data.cfg.exposure =[%d]\n", rc, cfg_data.cfg.exposure);
			break;
#endif
#ifdef F_SKYCAM_FIX_CFG_FOCUS_RECT
	case CFG_SET_FOCUS_RECT:
			rc = mt9p111_set_focus_rect(cfg_data.cfg.focus_rect);
			SKYCDBG("mt9p111_set_focus_rect OK! rc = [%d], cfg_data.cfg.focus_rect = [%d]\n", rc, cfg_data.cfg.focus_rect);
			break;
#endif
#ifdef F_SKYCAM_FIX_CFG_PREVIEW_FPS
	case CFG_SET_PREVIEW_FPS:
			rc = mt9p111_set_frame_rate(cfg_data.cfg.preview_fps); 
			SKYCDBG("mt9p111_set_frame_rate OK! rc = [%d], cfg_data.cfg.preview_fps =[%d]\n", rc, cfg_data.cfg.preview_fps);
			break;
#endif
#ifdef F_SKYCAM_FIX_CFG_ANTIBANDING
	case CFG_SET_ANTIBANDING:
			rc = mt9p111_set_antibanding(cfg_data.cfg.antibanding);
			SKYCDBG("mt9p111_set_antibanding OK! rc = [%d], cfg_data.cfg.antibanding =[%d]\n", rc, cfg_data.cfg.antibanding);			
			break;
#endif
#ifdef F_SKYCAM_TUP_LOAD_FILE
		case CFG_SET_TUNE_VALUE:
			rc = mt9p111_set_tune_value(&cfg_data);
			SKYCDBG("mt9p111_set_tune_value OK! rc = [%d]\n", rc);
			break;
#endif
#ifdef F_SKYCAM_ADD_CFG_DIMENSION
		case CFG_SET_DIMENSION:
			rc = mt9p111_set_dimension(&cfg_data.cfg.dimension);
			SKYCDBG("mt9p111_set_dimension OK! rc = [%d]\n", rc);			
			break;
#endif

		default:
			rc = 0; // -EINVAL;
			SKYCERR("mt9p111_video_config DEFAULT FAIL!!![EINVAL] rc = [%d]\n", EINVAL);
			break;
	}

	/* up(&mt9p111_sem); */
	SKYCDBG("%s end rc = %d\n",__func__, rc);
	return rc;
}


static int mt9p111_sensor_release(void)
{	
	int32_t rc = 0;
#ifdef F_SKYCAM_FIX_CFG_LED_MODE	
	mt9p111_set_led_mode(0);
#endif

#ifdef F_SKYCAM_EF18K_ES01
    mt9p111_reset(mt9p111_ctrl->sensordata, OFF);
#endif

	rc = mt9p111_power_off();
	if (rc)
		goto sensor_release_fail;

#ifdef F_SKYCAM_SAME_FOCUS_STEP_SKIP
	if (!rc) cur_focus_pos = C_SKYCAM_MIN_FOCUS_STEP;
#endif

	/* down(&mt9p111_sem); */
	kfree(mt9p111_ctrl);
	mt9p111_ctrl = NULL;
	/* up(&mt9p111_sem); */
	
#ifdef F_SKYCAM_TUP_LOAD_FILE
	mt9p111_tup_state = MT9P111_TUNE_STATE_NONE;
	mt9p111_tup_mode = MT9P111_TUNE_STATE_TUNNING_MODE_OFF;
#endif

#ifdef F_SKYCAM_CHECK_IS_INIT_SEQ_DONE
	 init_seq_done = false;
#endif

	SKYCDBG("%s end rc = %d\n",__func__, rc);
	return rc;

sensor_release_fail:
	SKYCERR("%s failed! (%d)\n", __func__, rc);
	return rc;
}


static int mt9p111_i2c_probe(struct i2c_client *client,
	const struct i2c_device_id *id)
{
	int rc = 0;
	if (!i2c_check_functionality(client->adapter, I2C_FUNC_I2C)) {
		rc = -ENOTSUPP;
		goto probe_failure;
	}

	mt9p111_sensorw = (struct mt9p111_work *)kzalloc(sizeof(struct mt9p111_work), GFP_KERNEL);
	if (!mt9p111_sensorw) {
		rc = -ENOMEM;
		goto probe_failure;
	}

	i2c_set_clientdata(client, mt9p111_sensorw);
	mt9p111_init_client(client);
	mt9p111_client = client;

	SKYCDBG("mt9p111_probe succeeded!\n");

	return 0;

probe_failure:
	kfree(mt9p111_sensorw);
	mt9p111_sensorw = NULL;
	SKYCERR("mt9p111_probe failed!\n");
	return rc;
}


static const struct i2c_device_id mt9p111_i2c_id[] = {
	{ "mt9p111", 0},
	{ },
};


static struct i2c_driver mt9p111_i2c_driver = {
	.id_table = mt9p111_i2c_id,
	.probe  = mt9p111_i2c_probe,
	.remove = __exit_p(mt9p111_i2c_remove),
	.driver = {
		.name = "mt9p111",
	},
};


static int32_t mt9p111_init_i2c(void)
{
	int32_t rc = 0;

	SKYCDBG("%s start\n",__func__);

	rc = i2c_add_driver(&mt9p111_i2c_driver);
	SKYCDBG("%s mt9p111_i2c_driver rc = %d\n",__func__, rc);
	if (IS_ERR_VALUE(rc))
		goto init_i2c_fail;

	SKYCDBG("%s end\n",__func__);
	return 0;

init_i2c_fail:
	SKYCERR("%s failed! (%d)\n", __func__, rc);
	return rc;
}


static int mt9p111_sensor_probe(const struct msm_camera_sensor_info *info, struct msm_sensor_ctrl *s)
{
	int rc = 0;
	SKYCDBG("%s start\n",__func__);
	
	rc = mt9p111_init_i2c();	
	if (rc < 0 || mt9p111_client == NULL)
	{
		SKYCERR("%s#%d: -ENOTSUPP.\n", __func__, __LINE__);
		goto probe_init_fail;
	}

	s->s_init = mt9p111_sensor_init;
	s->s_release = mt9p111_sensor_release;
	s->s_config  = mt9p111_sensor_config;
	return 0;

probe_init_fail:
	SKYCERR("%s Failed!\n", __func__);
	return 0;
}


static int __mt9p111_probe(struct platform_device *pdev)
{
	return msm_camera_drv_start(pdev, mt9p111_sensor_probe);
}

static struct platform_driver msm_camera_driver = {
	.probe = __mt9p111_probe,
	.driver = {
		.name = "msm_camera_mt9p111",
		.owner = THIS_MODULE,
	},
};


static int __init mt9p111_init(void)
{
	return platform_driver_register(&msm_camera_driver);
}

module_init(mt9p111_init);


#ifdef MT9P111_J_MASAI_E_WBPOINT
static unsigned short float16_to_register(int32_t f16val)
{
	uint16_t bit4to0, bit14to5, bit15, reg16;

	if (f16val < 0)
	{ 
		f16val*= -1;
		bit15 = 1;
	}
	else
		bit15 = 0;

	bit4to0 = 10;
	
	while(f16val >= 2048)
	{
		f16val=f16val >> 1;
		bit4to0++;
	}

	bit14to5 = (unsigned short)(f16val -1024);
	reg16 = ((bit15<<15)&0x8000) | ((bit14to5<<5) & 0x7FE0) | (bit4to0 & 0x001F);
	
	return reg16;
}


static int32_t register_to_float16(uint16_t reg16)
{
	int32_t fbit15, fbit14to5, fbit4to0, f16val;

	if(reg16&0x8000)
		fbit15 = -1;
	else fbit15 = 1;

	fbit14to5 = ((reg16>>5)&0x3FF) + 1024;
	fbit4to0 = (reg16&0x1F) - 10;

	if (fbit4to0 < 0)
		return 0;
	else
	{
		f16val = (fbit14to5 << fbit4to0) * fbit15;
		return f16val;
	}
}


static int32_t mt9p111_cal_pga_init(void)
{	
	int32_t rc = 0, real_data;
	uint16_t	read_data;
	int i = 0, j = 0;

	for (i = 0; i < NUM_PGA_PARA; i++)	{
		if (mt9p111_i2c_read_word(pga_table[i].pgaAddr, &read_data) < 0) {
			SKYCERR("<<%s (-EIO)", __func__);
			return -EIO;
		}

		real_data = register_to_float16(read_data);
		SKYCDBG("%s (%d) real_data=%d\n",__func__, i, real_data);
		for (j = 0; j < PGA_MAX_NUM; j++)	{
			pga_table[i].pgaData[j] = float16_to_register(real_data + ((((real_data * pga_factor_table[i][j]) + 256) >> 9)));
			SKYCDBG("%s (%d, %d) pgaData=%x\n",__func__, i,j,pga_table[i].pgaData[j] );
		}
	}	

	return rc;
}


static int32_t mt9p111_write_pga_table(uint16_t exp_val, uint16_t ccm_pos)
{	
	int32_t rc = 0;
	int i = 0;


	if (exp_val == 0) {
		SKYCDBG("%s pga PREVIEW(exp:%d, ccm:%d)\n",__func__, exp_val, ccm_pos );		
		for (i = 0; i < NUM_PGA_PARA; i++) {
			if (mt9p111_i2c_write_word(	pga_table[i].pgaAddr, pga_table[i].pgaData[PGA_PREVIEW]) < 0) {
				SKYCERR("<<%s (-EIO:%d)", __func__, __LINE__);
				return -EIO;
			}
		}
		mt9p111_i2c_write_word(0x098E, 0xAC9A);
		mt9p111_i2c_write_a2d1(0xAC97, 0x82);
		mt9p111_i2c_write_a2d1(0xAC98, 0x7E);
		mt9p111_i2c_write_a2d1(0xAC99, 0x84);
		
		mt9p111_i2c_write_a2d1(0xAC9A, 0x7A);
		mt9p111_i2c_write_a2d1(0xAC9B, 0x82);
		mt9p111_i2c_write_a2d1(0xAC9C, 0x86);
		
	} 
	else if (exp_val <= OUTDOOR_START_INDEX) {
		SKYCDBG("%s pga DAYLIGHT(exp:%d, ccm:%d)\n",__func__, exp_val, ccm_pos );
		for (i = 0; i < NUM_PGA_PARA; i++) {
			if (mt9p111_i2c_write_word(pga_table[i].pgaAddr, pga_table[i].pgaData[PGA_DAYLIGHT]) < 0) {
				SKYCERR("<<%s (-EIO:%d)", __func__, __LINE__);
				return -EIO;
			}
		}
		mt9p111_i2c_write_word(0x098E, 0xAC9A);

		mt9p111_i2c_write_a2d1(0xAC9A, 0x6F);
		mt9p111_i2c_write_a2d1(0xAC9B, 0x88);
		mt9p111_i2c_write_a2d1(0xAC9C, 0x8A);
		
	}
	else if (ccm_pos <= INCAND_CCM_POSTION) {
		SKYCDBG("%s pga INCAND(exp:%d, ccm:%d)\n",__func__, exp_val, ccm_pos );
		for (i = 0; i < NUM_PGA_PARA; i++) {
			if (mt9p111_i2c_write_word(pga_table[i].pgaAddr, pga_table[i].pgaData[PGA_INCAND_A]) < 0) {
				SKYCERR("<<%s (-EIO:%d)", __func__, __LINE__);
				return -EIO;
			}
		}
		mt9p111_i2c_write_word(0x098E, 0xAC9A);
		mt9p111_i2c_write_a2d1(0xAC97, 0x78);	
		mt9p111_i2c_write_a2d1(0xAC98, 0x84);
		mt9p111_i2c_write_a2d1(0xAC99, 0x9C);
	}
	else if (exp_val > OUTDOOR_END_INDEX) {
		SKYCDBG("%s pga FLOURESCENT(exp:%d, ccm:%d)\n",__func__, exp_val, ccm_pos );
		for (i = 0; i < NUM_PGA_PARA; i++) {
			if (mt9p111_i2c_write_word(pga_table[i].pgaAddr, pga_table[i].pgaData[PGA_FLOURESCENT]) < 0) {
				SKYCERR("<<%s (-EIO:%d)", __func__, __LINE__);
				return -EIO;
			}
		}

		mt9p111_i2c_write_word(0x098E, 0xAC9A);
		mt9p111_i2c_write_a2d1(0xAC9A, 0x80);	
		mt9p111_i2c_write_a2d1(0xAC9B, 0x80);
		mt9p111_i2c_write_a2d1(0xAC9C, 0x8C);

	}
	else SKYCDBG("%s pga PREVIEW-for capture(exp:%d, ccm:%d)\n",__func__, exp_val, ccm_pos );

	return rc;
}


static int32_t mt9p111_AWB_cal_init(void)
{
	int32_t rc = 0, i = 0;
	int32_t greyXY;	
	uint16_t ccmleft, ccmright, read_data;


	red_green = 689;	
	blue_green = 583;

	mt9p111_i2c_write_word(0x098E, 0x6024);	
	mt9p111_i2c_write_word(0xE024, 0x0004);
	mt9p111_i2c_write_word(0xE02A, 0xA010);

	do {
		msleep(10);
		mt9p111_i2c_read(mt9p111_client->addr, 0xE02A, &read_data, WORD_LEN);
		i++;
	} while ((i < 20) && (read_data & 0x0010));

	if ((read_data & 0x0010) == 0)  {
		rc = mt9p111_i2c_read(mt9p111_client->addr, 0xE02A, &read_data, WORD_LEN);
		if ((rc >= 0) && (read_data < red_green + 150) && (read_data > red_green - 150))
			red_green = read_data;
	}

	i = 0;

	mt9p111_i2c_write_word(0x098E, 0x6024);	
	mt9p111_i2c_write_word(0xE024, 0x0006);
	mt9p111_i2c_write_word(0xE02A, 0xA010);

	do {
		msleep(10);
		mt9p111_i2c_read(mt9p111_client->addr, 0xE02A, &read_data, WORD_LEN);
		i++;
	} while ((i < 20) && (read_data & 0x0010));

	if ((read_data & 0x0010) == 0)  {
		rc = mt9p111_i2c_read(mt9p111_client->addr, 0xE02A, &read_data, WORD_LEN);
		if ((rc >= 0) && (read_data < blue_green + 150) && (read_data > blue_green - 150)) 
			blue_green = read_data;
	}


	SKYCDBG("%s : red_green(0x%x), blue_green(0x%x) \n", __func__, red_green, blue_green);

	mwb_setting[MT9P111_CFG_WB_AUTO].r_min = 
			(uint16_t) ((CAL_RED_MIN * red_green + 500) / 1000);
	mwb_setting[MT9P111_CFG_WB_AUTO].r_max = 
			(uint16_t) ((CAL_RED_MAX * red_green + 500) / 1000);
	mwb_setting[MT9P111_CFG_WB_AUTO].r_min_br = 
			(uint16_t) ((CAL_RED_MIN_BRT * red_green + 500) / 1000);
	mwb_setting[MT9P111_CFG_WB_AUTO].r_max_br = 
 			(uint16_t) ((CAL_RED_MAX_BRT * red_green + 500) / 1000);

	mwb_setting[MT9P111_CFG_WB_AUTO].b_min = 
 			(uint16_t) ((CAL_BLUE_MIN * blue_green + 500) / 1000);
	mwb_setting[MT9P111_CFG_WB_AUTO].b_max = 
 			(uint16_t) ((CAL_BLUE_MAX * blue_green + 500) / 1000);
	mwb_setting[MT9P111_CFG_WB_AUTO].b_min_br = 
 			(uint16_t) ((CAL_BLUE_MIN_BRT * blue_green + 500) / 1000);
	mwb_setting[MT9P111_CFG_WB_AUTO].b_max_br = 
 			(uint16_t) ((CAL_BLUE_MAX_BRT * blue_green + 500) / 1000);

	greyXY = CAL_GREY_FACTOR_M / ((int32_t)red_green + (int32_t)blue_green) + CAL_GREY_FACTOR_N;
	ccmleft = (mwb_setting[MT9P111_CFG_WB_AUTO].r_max * 100 
				+ (mwb_setting[MT9P111_CFG_WB_AUTO].b_min>>1)) 
				/ mwb_setting[MT9P111_CFG_WB_AUTO].b_min;
	ccmright = (mwb_setting[MT9P111_CFG_WB_AUTO].r_min_br * 100 
				+ (mwb_setting[MT9P111_CFG_WB_AUTO].b_max>>1)) 
				/ mwb_setting[MT9P111_CFG_WB_AUTO].b_max;	

	SKYCDBG("%s : red_min(0x%x), red_max(0x%x), red_min_bright(0x%x), red_max_bright(0x%x)\n",
		__func__, mwb_setting[MT9P111_CFG_WB_AUTO].r_min, 
		mwb_setting[MT9P111_CFG_WB_AUTO].r_max, 
		mwb_setting[MT9P111_CFG_WB_AUTO].r_min_br, 
		mwb_setting[MT9P111_CFG_WB_AUTO].r_max_br);
	SKYCDBG("%s : blue_min(0x%x), blue_max(0x%x), greyXY(0x%x), ccmleft(0x%x)\n",
		__func__, mwb_setting[MT9P111_CFG_WB_AUTO].b_min, 
		mwb_setting[MT9P111_CFG_WB_AUTO].b_max, 
		greyXY, ccmleft);


	mt9p111_i2c_write_a2d1(0xACB0, (unsigned char)mwb_setting[MT9P111_CFG_WB_AUTO].r_min);
	mt9p111_i2c_write_a2d1(0xACB1, (unsigned char)mwb_setting[MT9P111_CFG_WB_AUTO].r_max);
	mt9p111_i2c_write_a2d1(0xACB2, (unsigned char)mwb_setting[MT9P111_CFG_WB_AUTO].r_min_br);
	mt9p111_i2c_write_a2d1(0xACB3, (unsigned char)mwb_setting[MT9P111_CFG_WB_AUTO].r_max_br);

	mt9p111_i2c_write_a2d1(0xACB4, (unsigned char)mwb_setting[MT9P111_CFG_WB_AUTO].b_min);
	mt9p111_i2c_write_a2d1(0xACB5, (unsigned char)mwb_setting[MT9P111_CFG_WB_AUTO].b_max);
	mt9p111_i2c_write_a2d1(0xACB6, (unsigned char)mwb_setting[MT9P111_CFG_WB_AUTO].b_min_br);
	mt9p111_i2c_write_a2d1(0xACB7, (unsigned char)mwb_setting[MT9P111_CFG_WB_AUTO].b_max_br);

	mt9p111_i2c_write_word(0xAC58, ccmleft);
	mt9p111_i2c_write_word(0xAC6E, ccmright);

	mt9p111_i2c_write_word(0xB842, greyXY - 2);
	mt9p111_i2c_write_word(0xB844, greyXY + 2);



	mwb_setting[MT9P111_CFG_WB_CUSTOM].r_min = mwb_setting[MT9P111_CFG_WB_AUTO].r_min;
	mwb_setting[MT9P111_CFG_WB_CUSTOM].r_max = mwb_setting[MT9P111_CFG_WB_AUTO].r_max;
	mwb_setting[MT9P111_CFG_WB_CUSTOM].r_min_br = mwb_setting[MT9P111_CFG_WB_AUTO].r_min_br;
	mwb_setting[MT9P111_CFG_WB_CUSTOM].r_max_br = mwb_setting[MT9P111_CFG_WB_AUTO].r_max_br;
	
	mwb_setting[MT9P111_CFG_WB_CUSTOM].b_min = mwb_setting[MT9P111_CFG_WB_AUTO].b_min;
	mwb_setting[MT9P111_CFG_WB_CUSTOM].b_max = mwb_setting[MT9P111_CFG_WB_AUTO].b_max;
	mwb_setting[MT9P111_CFG_WB_CUSTOM].b_min_br =  mwb_setting[MT9P111_CFG_WB_AUTO].b_min_br;
	mwb_setting[MT9P111_CFG_WB_CUSTOM].b_max_br = mwb_setting[MT9P111_CFG_WB_AUTO].b_max_br;

	SKYCDBG("MWB-custom: r_min(0x%x), r_max(0x%x), b_min(0x%x), b_max(0x%x)\n",
		mwb_setting[MT9P111_CFG_WB_CUSTOM].r_min, mwb_setting[MT9P111_CFG_WB_CUSTOM].r_max,
		mwb_setting[MT9P111_CFG_WB_CUSTOM].b_min, mwb_setting[MT9P111_CFG_WB_CUSTOM].b_max);


	mwb_setting[MT9P111_CFG_WB_INCANDESCENT].r_min = mwb_setting[MT9P111_CFG_WB_AUTO].r_max - 4;
	mwb_setting[MT9P111_CFG_WB_INCANDESCENT].r_max = mwb_setting[MT9P111_CFG_WB_AUTO].r_max;
	mwb_setting[MT9P111_CFG_WB_INCANDESCENT].r_min_br = mwb_setting[MT9P111_CFG_WB_INCANDESCENT].r_min;
	mwb_setting[MT9P111_CFG_WB_INCANDESCENT].r_max_br = mwb_setting[MT9P111_CFG_WB_INCANDESCENT].r_max;
	
	mwb_setting[MT9P111_CFG_WB_INCANDESCENT].b_min = mwb_setting[MT9P111_CFG_WB_AUTO].b_min;
	mwb_setting[MT9P111_CFG_WB_INCANDESCENT].b_max = mwb_setting[MT9P111_CFG_WB_AUTO].b_min + 3;
	mwb_setting[MT9P111_CFG_WB_INCANDESCENT].b_min_br =  mwb_setting[MT9P111_CFG_WB_INCANDESCENT].b_min;
	mwb_setting[MT9P111_CFG_WB_INCANDESCENT].b_max_br = mwb_setting[MT9P111_CFG_WB_INCANDESCENT].b_max;

	SKYCDBG("MWB-Incand: r_min(0x%x), r_max(0x%x), b_min(0x%x), b_max(0x%x)\n",
		mwb_setting[MT9P111_CFG_WB_INCANDESCENT].r_min, mwb_setting[MT9P111_CFG_WB_INCANDESCENT].r_max,
		mwb_setting[MT9P111_CFG_WB_INCANDESCENT].b_min, mwb_setting[MT9P111_CFG_WB_INCANDESCENT].b_max);
	

	mwb_setting[MT9P111_CFG_WB_FLUORESCENT].r_min 
		= (mwb_setting[MT9P111_CFG_WB_AUTO].r_min + mwb_setting[MT9P111_CFG_WB_AUTO].r_min_br + 1) >> 1;
	mwb_setting[MT9P111_CFG_WB_FLUORESCENT].r_max 
		= (mwb_setting[MT9P111_CFG_WB_AUTO].r_max + 2*mwb_setting[MT9P111_CFG_WB_AUTO].r_max_br + 1) / 3;
	mwb_setting[MT9P111_CFG_WB_FLUORESCENT].r_min_br = mwb_setting[MT9P111_CFG_WB_FLUORESCENT].r_min;
	mwb_setting[MT9P111_CFG_WB_FLUORESCENT].r_max_br = mwb_setting[MT9P111_CFG_WB_FLUORESCENT].r_max;
	
	mwb_setting[MT9P111_CFG_WB_FLUORESCENT].b_min 
		= (mwb_setting[MT9P111_CFG_WB_AUTO].b_min + mwb_setting[MT9P111_CFG_WB_AUTO].b_min_br +1 ) >> 1;
	mwb_setting[MT9P111_CFG_WB_FLUORESCENT].b_max 
		= (mwb_setting[MT9P111_CFG_WB_AUTO].b_max + mwb_setting[MT9P111_CFG_WB_AUTO].b_max_br +1 ) >>1;
	mwb_setting[MT9P111_CFG_WB_FLUORESCENT].b_min_br =  mwb_setting[MT9P111_CFG_WB_FLUORESCENT].b_min;
	mwb_setting[MT9P111_CFG_WB_FLUORESCENT].b_max_br = mwb_setting[MT9P111_CFG_WB_FLUORESCENT].b_max;
	
	SKYCDBG("MWB-fluorescent: r_min(0x%x), r_max(0x%x), b_min(0x%x), b_max(0x%x)\n",
		mwb_setting[MT9P111_CFG_WB_FLUORESCENT].r_min, mwb_setting[MT9P111_CFG_WB_FLUORESCENT].r_max,
		mwb_setting[MT9P111_CFG_WB_FLUORESCENT].b_min, mwb_setting[MT9P111_CFG_WB_FLUORESCENT].b_max);
	

	mwb_setting[MT9P111_CFG_WB_DAYLIGHT].r_min = mwb_setting[MT9P111_CFG_WB_AUTO].r_min_br - 1;
	mwb_setting[MT9P111_CFG_WB_DAYLIGHT].r_max = mwb_setting[MT9P111_CFG_WB_AUTO].r_max_br - 1;
	mwb_setting[MT9P111_CFG_WB_DAYLIGHT].r_min_br = mwb_setting[MT9P111_CFG_WB_DAYLIGHT].r_min;
	mwb_setting[MT9P111_CFG_WB_DAYLIGHT].r_max_br = mwb_setting[MT9P111_CFG_WB_DAYLIGHT].r_max;
	
	mwb_setting[MT9P111_CFG_WB_DAYLIGHT].b_min = mwb_setting[MT9P111_CFG_WB_AUTO].b_min_br + 1;
	mwb_setting[MT9P111_CFG_WB_DAYLIGHT].b_max = mwb_setting[MT9P111_CFG_WB_AUTO].b_max_br + 1;
	mwb_setting[MT9P111_CFG_WB_DAYLIGHT].b_min_br =  mwb_setting[MT9P111_CFG_WB_DAYLIGHT].b_min;
	mwb_setting[MT9P111_CFG_WB_DAYLIGHT].b_max_br = mwb_setting[MT9P111_CFG_WB_DAYLIGHT].b_max;

	SKYCDBG("MWB-fluorescent: r_min(0x%x), r_max(0x%x), b_min(0x%x), b_max(0x%x)\n",
		mwb_setting[MT9P111_CFG_WB_DAYLIGHT].r_min, mwb_setting[MT9P111_CFG_WB_DAYLIGHT].r_max,
		mwb_setting[MT9P111_CFG_WB_DAYLIGHT].b_min, mwb_setting[MT9P111_CFG_WB_DAYLIGHT].b_max);
	

	mwb_setting[MT9P111_CFG_WB_CLOUDY].r_min = mwb_setting[MT9P111_CFG_WB_AUTO].r_min_br + 1;
	mwb_setting[MT9P111_CFG_WB_CLOUDY].r_max = mwb_setting[MT9P111_CFG_WB_AUTO].r_max_br + 1;
	mwb_setting[MT9P111_CFG_WB_CLOUDY].r_min_br = mwb_setting[MT9P111_CFG_WB_CLOUDY].r_min;
	mwb_setting[MT9P111_CFG_WB_CLOUDY].r_max_br = mwb_setting[MT9P111_CFG_WB_CLOUDY].r_max;
	
	mwb_setting[MT9P111_CFG_WB_CLOUDY].b_min = mwb_setting[MT9P111_CFG_WB_AUTO].b_min_br - 1;
	mwb_setting[MT9P111_CFG_WB_CLOUDY].b_max = mwb_setting[MT9P111_CFG_WB_AUTO].b_max_br - 1;
	mwb_setting[MT9P111_CFG_WB_CLOUDY].b_min_br =  mwb_setting[MT9P111_CFG_WB_CLOUDY].b_min;
	mwb_setting[MT9P111_CFG_WB_CLOUDY].b_max_br = mwb_setting[MT9P111_CFG_WB_CLOUDY].b_max;

	SKYCDBG("MWB-fluorescent: r_min(0x%x), r_max(0x%x), b_min(0x%x), b_max(0x%x)\n",
		mwb_setting[MT9P111_CFG_WB_CLOUDY].r_min, mwb_setting[MT9P111_CFG_WB_CLOUDY].r_max,
		mwb_setting[MT9P111_CFG_WB_CLOUDY].b_min, mwb_setting[MT9P111_CFG_WB_CLOUDY].b_max);

	return rc;
}
#endif
