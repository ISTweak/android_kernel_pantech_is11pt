#ifndef EXTERNAL_AMP_CONTROLLER_H
#define EXTERNAL_AMP_CONTROLLER_H
/************************************************************************************************
**
**    AUDIO EXTERNAL AMP CONTROLLER
**
**    FILE
**        extampctrl.c
**
**    DESCRIPTION
**        This file contains audio external amp api//
**          
**          void snd_extamp_api_Init()//
**          void snd_extamp_api_SetPath()//
**          void snd_extamp_api_SetVolume()//
**          void snd_extamp_api_Sleep()//
**
**    Copyright (c) 2010 by PANTECH Incorporated.  All Rights Reserved.
*************************************************************************************************/


/************************************************************************************************
** Includes
*************************************************************************************************/
#include <linux/kernel.h>

#ifdef CONFIG_PANTECH_AUDIO_SUBCHIP_CAL_TOOL
#define FEATURE_SKY_SND_EXTAMPCTRL
#define FEATURE_SKY_SND_EXTAMPCTRL_FOR_APP
#endif

#define EXTAMP_SPKR_PHONE                      		0x01
#define EXTAMP_HEADPHONE_STEREO      			    0x02
#define EXTAMP_SPKR_PHONE_AND_HP_STEREO            	0x03
#ifdef CONFIG_PANTECH_LASER2_AUDIENCE2020
#define EXTAMP_NR_ON_CT 						    0X04
#define EXTAMP_NR_ON_FT 						    0X05
#define EXTAMP_NR_OFF_FT 					        0x06
#define EXTAMP_NR_ON_DV 					        0x07
#define EXTAMP_NR_OFF_DV					        0x08
#endif
#define EXTAMP_TTY_HEADPHONE					    0x09

#ifdef FEATURE_SKY_SND_EXTAMPCTRL_FOR_APP
#define EXTAMP_TEST_ID                      		0xFF
#endif

#ifdef FEATURE_SKY_SND_EXTAMPCTRL
struct extamp_device_info {
	uint32_t extamp_dev_id;
	uint32_t pga_info43;	//0x02
	uint32_t pga_info21;	//0x02
	uint32_t hp_ramp_speed;			//0x03
	uint32_t hp_ramp_en;			//0x03
	uint32_t hpl_vol_info;			//0x05
	uint32_t hpr_vol_info;			//0x06
	uint32_t hp_ng_zra_info;		//0x07
	uint32_t hp_ng_thresh_info;		//0x07
	uint32_t hp_ng_zrr_info;		//0x07
	uint32_t hp_ng_time_info;		//0x07
	uint32_t erc_info;				//0x08
	uint32_t spk_ramp_speed; 		//0x008
	uint32_t spk_ramp_en; 			//0x008
	uint32_t spk_ng_zra_info;		//0x09
	uint32_t spk_ng_thresh_info;	//0x09
	uint32_t spk_ng_zrr_info;		//0x09
	uint32_t spk_ng_time_info;		//0x09
	uint32_t thresh_max_info;		//0x0a
	uint32_t thresh_batt_info;		//0x0a
	uint32_t agc_attack_info;		//0x0b
	uint32_t agc_release_info;		//0x0b
	uint32_t agc_min_info;			//0x0c
	uint32_t start_gain_info;		//0x0e
	uint32_t mc_ssmt_info;			//0x0f 
	uint32_t ssmt_info;				//0x0f 
    uint32_t rx_voice_vol_0;		//call voice rx volume
	uint32_t rx_voice_vol_1;		//call voice rx volume
    uint32_t rx_voice_vol_2;		//call voice rx volume
    uint32_t rx_voice_vol_3;		//call voice rx volume
    uint32_t rx_voice_vol_4;		//call voice rx volume
    uint32_t rx_voice_vol_5;		//call voice rx volume
#if 0//def CONFIG_PANTECH_LASER2_AUDIENCE2020
       uint32_t audience_digital_out_tx_gain;					//audience digital output tx gain (0x8015)
       uint32_t audience_digital_out_rx_gain;					//audience digital output rx gain (0x8015)
       uint32_t audience_input_gain_id_prim_mic_gain;			//audience input gain primary mic gain(0x801B)       
       uint32_t audience_input_gain_id_sec_mic_gain;			//audience input gain secondary mic gain(0x801B)       
       uint32_t audience_input_gain_id_far_end_gain;			//audience input gain Far End (Rx) Gain(0x801B)       
       uint32_t audience_algo_param_id_aec_mode;				//audience algorithm param ID AEC Mode on/off(0x8017 id : 0x03)
       uint32_t audience_algo_param_id_se_mode;					//audience algorithm param ID Voice EQ mode SE on/off(0x8017 id : 0x09)
       uint32_t audience_algo_param_id_downlink_spk_vol_1;		//audience algorithm param ID DownLink Spk Volume(0x8017 id : 0x12)
       uint32_t audience_algo_param_id_downlink_spk_vol_2;		//audience algorithm param ID DownLink Spk Volume(0x8017 id : 0x12)
       uint32_t audience_algo_param_id_aec_comfort_noise_mode;	//audience algorithm param ID AEC Comfort Noise on/off(0x8017 id : 0x23)
       uint32_t audience_algo_param_id_ec_supp_enhanced_1;		//audience algorithm param ID echo suppression enhanced(0x8017 id : 0x34)
       uint32_t audience_algo_param_id_ec_supp_enhanced_2;		//audience algorithm param ID echo suppression enhanced(0x8017 id : 0x34)
       uint32_t audience_algo_param_id_veq_noise_estimate_adj_1;//audience algorithm param ID veq_noise_estimate adj(0x8017 id : 0x25)
       uint32_t audience_algo_param_id_veq_noise_estimate_adj_2;//audience algorithm param ID veq_noise_estimate adj(0x8017 id : 0x25)
       uint32_t audience_algo_param_id_veq_max_gain_1;			//audience algorithm param ID veq max gain(0x8017 id : 0x3d)
       uint32_t audience_algo_param_id_veq_max_gain_2;			//audience algorithm param ID veq max gain(0x8017 id : 0x3d)       
       uint32_t audience_algo_param_id_far_end_noise_supp;		//audience algorithm param ID far end noise suppression(0x8017 id : 0x0e)       
       uint32_t audience_algo_param_id_fens_amount_1;			//audience algorithm param ID FENS amount(0x8017 id : 0x0f)       
       uint32_t audience_algo_param_id_fens_amount_2;			//audience algorithm param ID FENS amount(0x8017 id : 0x0f)       
#endif
};

struct extamp_device_info *extamp_lookup_device(uint32_t extamp_dev_id);
uint32_t snd_extamp_api_GetGainTable(uint32_t DeviceId, uint32_t Register);
void snd_extamp_api_SetGainTableForTest(uint32_t Register, uint32_t value);
void snd_extamp_api_SetVolume_ById(uint32_t extamp_dev_id);

#ifdef FEATURE_SKY_SND_EXTAMPCTRL_FOR_APP
extern bool LockForTest;

struct bitmask {
  uint32_t min;
  uint32_t bitmask;
};

void snd_extamp_api_AudioCmd_ForInt(unsigned int cmd, unsigned long arg);
void snd_extamp_api_AudioCmd_ForString(unsigned int cmd, unsigned long arg);
#endif /* FEATURE_SKY_SND_EXTAMPCTRL_FOR_APP */

#endif /* FEATURE_SKY_SND_EXTAMPCTRL */
#endif /* EXTERNAL_AMP_CONTROLLER_H */
