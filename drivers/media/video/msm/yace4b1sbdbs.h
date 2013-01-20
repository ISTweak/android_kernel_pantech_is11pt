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
 
#ifndef YACE4B1SBDBS_H
#define YACE4B1SBDBS_H

#include <linux/types.h>
#include <mach/camera.h>

enum
{
CAMERA_BRIGHTNESS_NEGATIVE_4,
CAMERA_BRIGHTNESS_NEGATIVE_3,
CAMERA_BRIGHTNESS_NEGATIVE_2,
CAMERA_BRIGHTNESS_NEGATIVE_1,
CAMERA_BRIGHTNESS_NORMAL_0,
CAMERA_BRIGHTNESS_POSITIVE_1,
CAMERA_BRIGHTNESS_POSITIVE_2,
CAMERA_BRIGHTNESS_POSITIVE_3,
CAMERA_BRIGHTNESS_POSITIVE_4
};

enum
{
CAMERA_WB_AUTO = 1,  /* This list must match aeecamera.h */
CAMERA_WB_CUSTOM,
CAMERA_WB_INCANDESCENT,
CAMERA_WB_FLUORESCENT,
CAMERA_WB_DAYLIGHT,
CAMERA_WB_CLOUDY_DAYLIGHT,

};

enum
{
CAMERA_INFINITY,
CAMERA_MACRO ,	
CAMERA_AF
};


extern struct yace4b1sbdbs_reg yace4b1sbdbs_regs;

enum yace4b1sbdbs_width {
	WORD_LEN,
	BYTE_LEN
};

struct yace4b1sbdbs_i2c_reg_conf {
	unsigned short waddr;
	unsigned short wdata;
	enum yace4b1sbdbs_width width;
	unsigned short mdelay_time;

};


struct reg_addr_value_pair
{
	uint32_t  register_address;
	uint32_t register_value;
	union
		{
			uint32_t delay_time;
			uint32_t mode;
		};
};

/*101020 Do. layer1. for fixed fps option */

struct fps_arr
 {
	struct reg_addr_value_pair const *value_set;
	int32_t size;
 };

struct yace4b1sbdbs_reg {
	const struct reg_addr_value_pair *init_ARM_reg_settings;
	uint16_t initial_ARM_mode_reg_settings_size;
	const struct reg_addr_value_pair *init_reg_settings;
	uint16_t initial_sensor_mode_reg_settings_size;
	const struct reg_addr_value_pair *preview_reg_settings;
	uint16_t preview_mode_contextA_size;
	const struct reg_addr_value_pair *snapshot_reg_settings_1;
	uint16_t snapshot_mode_contextB_size_1;
	/* focus mode */
/* auto */
	const struct reg_addr_value_pair *auto_focus_on_setting;
	uint16_t auto_focus_on_setting_size;
/* infinity */
	const struct reg_addr_value_pair *infinity_focus_on_setting;
	uint16_t infinity_focus_on_setting_size;
/* macro */
	const struct reg_addr_value_pair *macro_focus_on_setting;
	uint16_t macro_focus_on_setting_size;
/* focus initial */
	const struct reg_addr_value_pair *focus_off_setting;
	uint16_t focus_off_setting_size;

	// whitebalance
	
	const struct reg_addr_value_pair *whitebalance_auto_setting;
	uint16_t whitebalance_auto_setting_size;
	const struct reg_addr_value_pair *whitebalance_daylight_setting;
	uint16_t whitebalance_daylight_setting_size;
	const struct reg_addr_value_pair *whitebalance_cloudy_setting;
	uint16_t whitebalance_cloudy_setting_size;
	const struct reg_addr_value_pair *whitebalance_flourescent_setting;
	uint16_t whitebalance_flourescent_setting_size;
	const struct reg_addr_value_pair *whitebalance_tungsten_setting;
	uint16_t whitebalance_tungsten_setting_size;

	const struct reg_addr_value_pair *filter_normal_setting;
	uint16_t filter_normal_setting_size;
	const struct reg_addr_value_pair *filter_mono_setting;
	uint16_t filter_mono_setting_size;
	const struct reg_addr_value_pair *filter_sepia_setting;
	uint16_t filter_sepia_setting_size;
	const struct reg_addr_value_pair *filter_negative_setting;
	uint16_t filter_negative_setting_size;

	//brightness set
	const struct reg_addr_value_pair *brightness_positive_4_setting;
	uint16_t brightness_positive_4_setting_size;
	const struct reg_addr_value_pair *brightness_positive_3_setting;
	uint16_t brightness_positive_3_setting_size;
	const struct reg_addr_value_pair *brightness_positive_2_setting;
	uint16_t brightness_positive_2_setting_size;
	const struct reg_addr_value_pair *brightness_positive_1_setting;
	uint16_t brightness_positive_1_setting_size;
	const struct reg_addr_value_pair *brightness_normal_0_setting;
	uint16_t brightness_normal_0_setting_size;
	const struct reg_addr_value_pair *brightness_negative_1_setting;
	uint16_t brightness_negative_1_setting_size;
	const struct reg_addr_value_pair *brightness_negative_2_setting;
	uint16_t brightness_negative_2_setting_size;
	const struct reg_addr_value_pair *brightness_negative_3_setting;
	uint16_t brightness_negative_3_setting_size;
	const struct reg_addr_value_pair *brightness_negative_4_setting;
	uint16_t brightness_negative_4_setting_size;
	const struct reg_addr_value_pair *division_normal_condition_setting;
	uint16_t division_normal_condition_setting_size ;

	/* 101020 Do. layer1 for fixed fps option */
	const struct reg_addr_value_pair *preview_fps_setting ;
	uint16_t preview_fps_setting_size ;


};

 int32_t yace4b1sbdbs_power_up(void);
 int32_t yace4b1sbdbs_power_down(void);
 long yace4b1sbdbs_i2c_write_loop(int32_t i, const struct reg_addr_value_pair reg_pair[]);
 long yace4b1sbdbs_set_whitebalance(int mode, int wb);
 long yace4b1sbdbs_move_focus_mode(int mode, int8_t autofocus);
 long yace4b1sbdbs_set_focus_step(int mode, int32_t focus_step);

/* 101020 Do. layer1 for division shot */

int32_t yace4b1sbdbs_set_focus_rect(uint32_t focus_rect);
int32_t  yace4b1sbdbs_write_focus_rect(uint8_t xleft, uint8_t xright, uint8_t ytop, uint8_t ybottom, int16_t weight);

/*101020 Do. layer1. for fixed fps option */
#ifdef FEATURE_MOON_SET_FPS
 long yace4b1sbdbs_set_preview_fps(uint32_t fps);
#endif
#endif /* YACE4B1SBDBS_H */
