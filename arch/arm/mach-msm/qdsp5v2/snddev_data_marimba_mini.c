/* Copyright (c) 2009-2010, Code Aurora Forum. All rights reserved.
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
#include <linux/platform_device.h>
#include <linux/debugfs.h>
#include <linux/mfd/msm-adie-codec.h>
#include <linux/uaccess.h>
#include <mach/qdsp5v2/snddev_icodec.h>
#include <mach/qdsp5v2/marimba_profile.h>
#include <mach/qdsp5v2/aux_pcm.h>
#include <mach/qdsp5v2/snddev_ecodec.h>
#include <mach/qdsp5v2/audio_dev_ctl.h>
#include <mach/qdsp5v2/snddev_virtual.h>
#include <mach/board.h>
#include <asm/mach-types.h>
#include <mach/gpio.h>
#include <mach/qdsp5v2/snddev_mi2s.h>
#include <mach/qdsp5v2/mi2s.h>

#include <mach/qdsp5v2/audio_acdb_def_mini.h>

#ifdef CONFIG_PANTECH_AUDIO_FAB2200
#include "snd_sub_fab2200_mini.h"
#endif

#ifdef CONFIG_PANTECH_MINI_AUDIO_AUDIENCE1028 // jmlee
#include "snd_audience_a1028_mini.h"
#endif

/* define the value for BT_SCO */
#define BT_SCO_PCM_CTL_VAL (PCM_CTL__RPCM_WIDTH__LINEAR_V |\
				PCM_CTL__TPCM_WIDTH__LINEAR_V)
#define BT_SCO_DATA_FORMAT_PADDING (DATA_FORMAT_PADDING_INFO__RPCM_FORMAT_V |\
				DATA_FORMAT_PADDING_INFO__TPCM_FORMAT_V)
#define BT_SCO_AUX_CODEC_INTF   AUX_CODEC_INTF_CTL__PCMINTF_DATA_EN_V

#ifdef CONFIG_DEBUG_FS
static struct dentry *debugfs_hsed_config;
static void snddev_hsed_config_modify_setting(int type);
static void snddev_hsed_config_restore_setting(void);
#endif

static struct adie_codec_action_unit iearpiece_48KHz_osr256_actions[] =
	HANDSET_RX_48000_OSR_256;

static struct adie_codec_hwsetting_entry iearpiece_settings[] = {
	{
		.freq_plan = 48000,
		.osr = 256,
		.actions = iearpiece_48KHz_osr256_actions,
		.action_sz = ARRAY_SIZE(iearpiece_48KHz_osr256_actions),
	}
};

static struct adie_codec_dev_profile iearpiece_profile = {
	.path_type = ADIE_CODEC_RX,
	.settings = iearpiece_settings,
	.setting_sz = ARRAY_SIZE(iearpiece_settings),
};

static struct snddev_icodec_data snddev_iearpiece_data = {
	.capability = (SNDDEV_CAP_RX | SNDDEV_CAP_VOICE),
	.name = "handset_rx",
	.copp_id = 0,
	.acdb_id = ACDB_ID_HANDSET_SPKR,
	.profile = &iearpiece_profile,
	.channel_mode = 1,
	.pmctl_id = NULL,
	.pmctl_id_sz = 0,
	.default_sample_rate = 48000,
#ifdef CONFIG_PANTECH_MINI_AUDIO_AUDIENCE1028  // jmlee CONFIG_SND_PAN_AUDIO
	.pamp_on = &msm_snddev_nr_out_sel_qtrReceiver_or_A1028Speaker,  //20110301 jhsong : abnormal work after set path.....cause by pointer address so add "&" before func
	.pamp_off = &msm_snddev_nr_out_sel_A1028Reciever,  //20110301 jhsong : abnormal work after set path.....cause by pointer address so add "&" before func
#else
	.pamp_on = NULL,
	.pamp_off = NULL,
#endif	
	.property = SIDE_TONE_MASK,        
	.max_voice_rx_vol[VOC_NB_INDEX] = -200,
	.min_voice_rx_vol[VOC_NB_INDEX] = -1700,
	.max_voice_rx_vol[VOC_WB_INDEX] = -200,
	.min_voice_rx_vol[VOC_WB_INDEX] = -1700
};

static struct platform_device msm_iearpiece_device = {
	.name = "snddev_icodec",
	.id = 0,
	.dev = { .platform_data = &snddev_iearpiece_data },
};

static struct adie_codec_action_unit imic_8KHz_osr256_actions[] =
	HANDSET_TX_8000_OSR_256;

static struct adie_codec_action_unit imic_16KHz_osr256_actions[] =
	HANDSET_TX_16000_OSR_256;

static struct adie_codec_action_unit imic_48KHz_osr256_actions[] =
	HANDSET_TX_48000_OSR_256;

static struct adie_codec_hwsetting_entry imic_settings[] = {
	{
		.freq_plan = 8000,
		.osr = 256,
		.actions = imic_8KHz_osr256_actions,
		.action_sz = ARRAY_SIZE(imic_8KHz_osr256_actions),
	},
	{
		.freq_plan = 16000,
		.osr = 256,
		.actions = imic_16KHz_osr256_actions,
		.action_sz = ARRAY_SIZE(imic_16KHz_osr256_actions),
	},
	{
		.freq_plan = 48000,
		.osr = 256,
		.actions = imic_48KHz_osr256_actions,
		.action_sz = ARRAY_SIZE(imic_48KHz_osr256_actions),
	}
};

static struct adie_codec_dev_profile imic_profile = {
	.path_type = ADIE_CODEC_TX,
	.settings = imic_settings,
	.setting_sz = ARRAY_SIZE(imic_settings),
};

static enum hsed_controller imic_pmctl_id[] = {PM_HSED_CONTROLLER_0};

static struct snddev_icodec_data snddev_imic_data = {
	.capability = (SNDDEV_CAP_TX | SNDDEV_CAP_VOICE),
	.name = "handset_tx",
	.copp_id = 0,
	.acdb_id = ACDB_ID_HANDSET_MIC,
	.profile = &imic_profile,
	.channel_mode = 1,
	.pmctl_id = imic_pmctl_id,
	.pmctl_id_sz = ARRAY_SIZE(imic_pmctl_id),
	.default_sample_rate = 48000,
#ifdef CONFIG_PANTECH_MINI_AUDIO_AUDIENCE1028 // jmlee QTR Route NR_OFF CT TALK TX/RX
	.pamp_on = &msm_snddev_mic_sel_QTR,  //20110301 jhsong : abnormal work after set path.....cause by pointer address so add "&" before func
	.pamp_off = NULL,
#else
	.pamp_on = NULL,
	.pamp_off = NULL,
#endif
};

static struct platform_device msm_imic_device = {
	.name = "snddev_icodec",
	.id = 1,
	.dev = { .platform_data = &snddev_imic_data },
};

#ifdef CONFIG_PANTECH_ASR_PATH    //20110428 jhsong : feature clearup
static struct adie_codec_action_unit asr_handset_mic_48KHz_osr256_actions[] =
	ASR_HANDSET_TX_48000_OSR_256;

static struct adie_codec_hwsetting_entry asr_handset_mic_settings[] = {
	{
		.freq_plan = 48000,
		.osr = 256,
		.actions = asr_handset_mic_48KHz_osr256_actions,
		.action_sz = ARRAY_SIZE(asr_handset_mic_48KHz_osr256_actions),
	}
};

static struct adie_codec_dev_profile asr_handset_mic_profile = {
	.path_type = ADIE_CODEC_TX,
	.settings = asr_handset_mic_settings,
	.setting_sz = ARRAY_SIZE(asr_handset_mic_settings),
};

static struct snddev_icodec_data snddev_asr_handset_mic_data = {
	.capability = (SNDDEV_CAP_TX | SNDDEV_CAP_VOICE),
	.name = "asr_handset_tx",
	.copp_id = 0,
	.acdb_id = ACDB_ID_ASR_HANDSET_TX,
	.profile = &asr_handset_mic_profile,
	.channel_mode = 1,
	.pmctl_id = imic_pmctl_id,
	.pmctl_id_sz = ARRAY_SIZE(imic_pmctl_id),
	.default_sample_rate = 48000,
#ifdef CONFIG_PANTECH_MINI_AUDIO_AUDIENCE1028 // jmlee QTR Route NR_OFF CT TALK TX/RX
	.pamp_on = &msm_snddev_mic_sel_QTR,  //20110301 jhsong : abnormal work after set path.....cause by pointer address so add "&" before func
	.pamp_off = NULL,
#else
	.pamp_on = NULL,
	.pamp_off = NULL,
#endif
};

static struct platform_device msm_asr_handset_tx_device = {
	.name = "snddev_icodec",
	.id = 33,
	.dev = { .platform_data = &snddev_asr_handset_mic_data },
};
#endif

#ifdef CONFIG_PANTECH_MINI_AUDIO_AUDIENCE1028 // jmlee QTR Route NR_OFF CT TALK TX/RX
///////////////////////////////////////////////////////////////////////////
// NR_OFF_CT_RX

static struct adie_codec_action_unit i_nr_off_ct_rx_48KHz_osr256_actions[] =
	NR_OFF_CT_RX_48000_OSR_256;

static struct adie_codec_hwsetting_entry i_nr_off_ct_rx_settings[] = {
	{
		.freq_plan = 48000,
		.osr = 256,
		.actions = i_nr_off_ct_rx_48KHz_osr256_actions,
		.action_sz = ARRAY_SIZE(i_nr_off_ct_rx_48KHz_osr256_actions),
	}
};

static struct adie_codec_dev_profile i_nr_off_ct_rx_profile = {
	.path_type = ADIE_CODEC_RX,
	.settings = i_nr_off_ct_rx_settings,
	.setting_sz = ARRAY_SIZE(i_nr_off_ct_rx_settings),
};

static struct snddev_icodec_data snddev_i_nr_off_ct_rx_data = {
	.capability = (SNDDEV_CAP_RX | SNDDEV_CAP_VOICE),
	.name = "nr_off_ct_rx",
	.copp_id = 0,
	.acdb_id = ACDB_ID_NR_OFF_CT_RX,
	.profile = &i_nr_off_ct_rx_profile,
	.channel_mode = 1,
	.pmctl_id = NULL,
	.pmctl_id_sz = 0,
	.default_sample_rate = 48000,
	.pamp_on = &msm_snddev_nr_out_sel_qtrReceiver_or_A1028Speaker,  //20110301 jhsong : abnormal work after set path.....cause by pointer address so add "&" before func
	.pamp_off = &msm_snddev_nr_out_sel_A1028Reciever,  // default switch statue by H/W recommand  //20110301 jhsong : abnormal work after set path.....cause by pointer address so add "&" before func
	.property = SIDE_TONE_MASK,        
	.max_voice_rx_vol[VOC_NB_INDEX] = -200,
	.min_voice_rx_vol[VOC_NB_INDEX] = -1700,
	.max_voice_rx_vol[VOC_WB_INDEX] = -200,
	.min_voice_rx_vol[VOC_WB_INDEX] = -1700
};

static struct platform_device msm_nr_off_ct_rx_device = {
	.name = "snddev_icodec",
	.id = 27, //
	.dev = { .platform_data = &snddev_i_nr_off_ct_rx_data },
};

///////////////////////////////////////////////////////////////////////////
// NR_OFF_FT_RX
// refer : msm_ispeaker_rx_device

static struct adie_codec_action_unit i_nr_off_ft_rx_48KHz_osr256_actions[] =
	NR_OFF_FT_RX_48000_OSR_256;

static struct adie_codec_hwsetting_entry i_nr_off_ft_rx_settings[] = {
	{
		.freq_plan = 48000,
		.osr = 256,
		.actions = i_nr_off_ft_rx_48KHz_osr256_actions,
		.action_sz = ARRAY_SIZE(i_nr_off_ft_rx_48KHz_osr256_actions),
	}
};

static struct adie_codec_dev_profile i_nr_off_ft_rx_profile = {
	.path_type = ADIE_CODEC_RX,
	.settings = i_nr_off_ft_rx_settings,
	.setting_sz = ARRAY_SIZE(i_nr_off_ft_rx_settings),
};

static struct snddev_icodec_data snddev_i_nr_off_ft_rx_data = {
	.capability = (SNDDEV_CAP_RX | SNDDEV_CAP_VOICE),
	.name = "nr_off_ft_rx",
	.copp_id = 0,
	.acdb_id = ACDB_ID_NR_OFF_FT_RX,
	.profile = &i_nr_off_ft_rx_profile,
	.channel_mode = 2,
	.pmctl_id = NULL,
	.pmctl_id_sz = 0,
	.default_sample_rate = 48000,
#ifdef CONFIG_PANTECH_AUDIO_FAB2200    //20110428 jhsong : feature clearup
	.pamp_on = &snd_subsystem_nr_off_ft_poweron,
	.pamp_off = &snd_subsystem_nr_off_ft_powerdown,
#endif
	.max_voice_rx_vol[VOC_NB_INDEX] = -200,
	.min_voice_rx_vol[VOC_NB_INDEX] = -1700,
	.max_voice_rx_vol[VOC_WB_INDEX] = -200,
	.min_voice_rx_vol[VOC_WB_INDEX] = -1700
};

static struct platform_device msm_nr_off_ft_rx_device = {
	.name = "snddev_icodec",
	.id = 28, //
	.dev = { .platform_data = &snddev_i_nr_off_ft_rx_data },
};

///////////////////////////////////////////////////////////////////////////
// NR_OFF_DV_RX
// refer : msm_ispeaker_rx_device

static struct adie_codec_action_unit i_nr_off_dv_rx_48KHz_osr256_actions[] =
	NR_OFF_DV_RX_48000_OSR_256;

static struct adie_codec_hwsetting_entry i_nr_off_dv_rx_settings[] = {
	{
		.freq_plan = 48000,
		.osr = 256,
		.actions = i_nr_off_dv_rx_48KHz_osr256_actions,
		.action_sz = ARRAY_SIZE(i_nr_off_dv_rx_48KHz_osr256_actions),
	}
};

static struct adie_codec_dev_profile i_nr_off_dv_rx_profile = {
	.path_type = ADIE_CODEC_RX,
	.settings = i_nr_off_dv_rx_settings,
	.setting_sz = ARRAY_SIZE(i_nr_off_dv_rx_settings),
};

static struct snddev_icodec_data snddev_i_nr_off_dv_rx_data = {
	.capability = (SNDDEV_CAP_RX | SNDDEV_CAP_VOICE),
	.name = "nr_off_dv_rx",
	.copp_id = 0,
	.acdb_id = ACDB_ID_NR_OFF_DV_RX,
	.profile = &i_nr_off_dv_rx_profile,
	.channel_mode = 2,
	.pmctl_id = NULL,
	.pmctl_id_sz = 0,
	.default_sample_rate = 48000,
#ifdef CONFIG_PANTECH_AUDIO_FAB2200    //20110428 jhsong : feature clearup
	.pamp_on = &snd_subsystem_nr_off_dv_poweron,
	.pamp_off = &snd_subsystem_nr_off_dv_powerdown,
#endif
	.max_voice_rx_vol[VOC_NB_INDEX] = -200,
	.min_voice_rx_vol[VOC_NB_INDEX] = -1700,
	.max_voice_rx_vol[VOC_WB_INDEX] = -200,
	.min_voice_rx_vol[VOC_WB_INDEX] = -1700
};

static struct platform_device msm_nr_off_dv_rx_device = {
	.name = "snddev_icodec",
	.id = 29, //
	.dev = { .platform_data = &snddev_i_nr_off_dv_rx_data },
};

///////////////////////////////////////////////////////////////////////////
// NR_OFF_CT_TX

static struct adie_codec_action_unit i_nr_off_ct_tx_8KHz_osr256_actions[] =
	NR_OFF_CT_TX_8000_OSR_256;

static struct adie_codec_action_unit i_nr_off_ct_tx_16KHz_osr256_actions[] =
	NR_OFF_CT_TX_16000_OSR_256;

static struct adie_codec_action_unit i_nr_off_ct_tx_48KHz_osr256_actions[] =
	NR_OFF_CT_TX_48000_OSR_256;

static struct adie_codec_hwsetting_entry i_nr_off_ct_tx_settings[] = {
	{
		.freq_plan = 8000,
		.osr = 256,
		.actions = i_nr_off_ct_tx_8KHz_osr256_actions,
		.action_sz = ARRAY_SIZE(i_nr_off_ct_tx_8KHz_osr256_actions),
	},
	{
		.freq_plan = 16000,
		.osr = 256,
		.actions = i_nr_off_ct_tx_16KHz_osr256_actions,
		.action_sz = ARRAY_SIZE(i_nr_off_ct_tx_16KHz_osr256_actions),
	},
	{
		.freq_plan = 48000,
		.osr = 256,
		.actions = i_nr_off_ct_tx_48KHz_osr256_actions,
		.action_sz = ARRAY_SIZE(i_nr_off_ct_tx_48KHz_osr256_actions),
	}
};

static struct adie_codec_dev_profile i_nr_off_ct_tx_profile = {
	.path_type = ADIE_CODEC_TX,
	.settings = i_nr_off_ct_tx_settings,
	.setting_sz = ARRAY_SIZE(i_nr_off_ct_tx_settings),
};

static enum hsed_controller i_nr_off_ct_tx_pmctl_id[] = {PM_HSED_CONTROLLER_0};

static struct snddev_icodec_data snddev_i_nr_off_ct_tx_data = {
	.capability = (SNDDEV_CAP_TX | SNDDEV_CAP_VOICE),
	.name = "nr_off_ct_tx",
	.copp_id = 0,
	.acdb_id = ACDB_ID_NR_OFF_CT_TX,
	.profile = &i_nr_off_ct_tx_profile,
	.channel_mode = 1,
	.pmctl_id = i_nr_off_ct_tx_pmctl_id,
	.pmctl_id_sz = ARRAY_SIZE(i_nr_off_ct_tx_pmctl_id),
	.default_sample_rate = 48000,
	.pamp_on = &msm_snddev_mic_sel_QTR,  //20110301 jhsong : abnormal work after set path.....cause by pointer address so add "&" before func
	.pamp_off = NULL,
};

static struct platform_device msm_nr_off_ct_tx_device = {
	.name = "snddev_icodec",
	.id = 30,
	.dev = { .platform_data = &snddev_i_nr_off_ct_tx_data },
};

///////////////////////////////////////////////////////////////////////////
// NR_OFF_FT_TX

static struct adie_codec_action_unit i_nr_off_ft_tx_speaker_tx_8KHz_osr256_actions[] =
	NR_OFF_FT_TX_8000_OSR_256;

static struct adie_codec_action_unit i_nr_off_ft_tx_48KHz_osr256_actions[] =
	NR_OFF_FT_TX_48000_OSR_256;

static struct adie_codec_hwsetting_entry i_nr_off_ft_tx_settings[] = {
	{
		.freq_plan = 8000,
		.osr = 256,
		.actions = i_nr_off_ft_tx_speaker_tx_8KHz_osr256_actions,
		.action_sz = ARRAY_SIZE(i_nr_off_ft_tx_speaker_tx_8KHz_osr256_actions),
	},
	{ /* 8KHz profile is good for 16KHz */
		.freq_plan = 16000,
		.osr = 256,
		.actions = i_nr_off_ft_tx_speaker_tx_8KHz_osr256_actions,
		.action_sz = ARRAY_SIZE(i_nr_off_ft_tx_speaker_tx_8KHz_osr256_actions),
	},
	{
		.freq_plan = 48000,
		.osr = 256,
		.actions = i_nr_off_ft_tx_48KHz_osr256_actions,
		.action_sz = ARRAY_SIZE(i_nr_off_ft_tx_48KHz_osr256_actions),
	}
};

static struct adie_codec_dev_profile i_nr_off_ft_tx_profile = {
	.path_type = ADIE_CODEC_TX,
	.settings = i_nr_off_ft_tx_settings,
	.setting_sz = ARRAY_SIZE(i_nr_off_ft_tx_settings),
};

static enum hsed_controller i_nr_off_ft_tx_pmctl_id[] = {PM_HSED_CONTROLLER_0};

static struct snddev_icodec_data snddev_i_nr_off_ft_tx_data = {
	.capability = (SNDDEV_CAP_TX | SNDDEV_CAP_VOICE),
	.name = "nr_off_ft_tx",
	.copp_id = 0,
	.acdb_id = ACDB_ID_NR_OFF_FT_TX,
	.profile = &i_nr_off_ft_tx_profile,
	.channel_mode = 1,
	.pmctl_id = i_nr_off_ft_tx_pmctl_id,
	.pmctl_id_sz = ARRAY_SIZE(i_nr_off_ft_tx_pmctl_id),
	.default_sample_rate = 48000,
#ifdef CONFIG_PANTECH_AUDIO_FAB2200    //20110428 jhsong : feature clearup
	.pamp_on = &msm_snddev_mic_sel_QTR,  //20110301 jhsong : abnormal work after set path.....cause by pointer address so add "&" before func
	.pamp_off = NULL,
#else
	.pamp_on = msm_snddev_tx_route_config,
	.pamp_off = msm_snddev_tx_route_deconfig,
#endif	
};

static struct platform_device msm_nr_off_ft_tx_device = {
	.name = "snddev_icodec",
	.id = 31,
	.dev = { .platform_data = &snddev_i_nr_off_ft_tx_data },
};


///////////////////////////////////////////////////////////////////////////
// NR_OFF_DV_TX

static struct adie_codec_action_unit i_nr_off_dv_tx_speaker_tx_8KHz_osr256_actions[] =
	NR_OFF_DV_TX_8000_OSR_256;

static struct adie_codec_action_unit i_nr_off_dv_tx_48KHz_osr256_actions[] =
	NR_OFF_DV_TX_48000_OSR_256;

static struct adie_codec_hwsetting_entry i_nr_off_dv_tx_settings[] = {
	{
		.freq_plan = 8000,
		.osr = 256,
		.actions = i_nr_off_dv_tx_speaker_tx_8KHz_osr256_actions,
		.action_sz = ARRAY_SIZE(i_nr_off_dv_tx_speaker_tx_8KHz_osr256_actions),
	},
	{ /* 8KHz profile is good for 16KHz */
		.freq_plan = 16000,
		.osr = 256,
		.actions = i_nr_off_dv_tx_speaker_tx_8KHz_osr256_actions,
		.action_sz = ARRAY_SIZE(i_nr_off_dv_tx_speaker_tx_8KHz_osr256_actions),
	},
	{
		.freq_plan = 48000,
		.osr = 256,
		.actions = i_nr_off_dv_tx_48KHz_osr256_actions,
		.action_sz = ARRAY_SIZE(i_nr_off_dv_tx_48KHz_osr256_actions),
	}
};

static struct adie_codec_dev_profile i_nr_off_dv_tx_profile = {
	.path_type = ADIE_CODEC_TX,
	.settings = i_nr_off_dv_tx_settings,
	.setting_sz = ARRAY_SIZE(i_nr_off_dv_tx_settings),
};

static enum hsed_controller i_nr_off_dv_tx_pmctl_id[] = {PM_HSED_CONTROLLER_0};

static struct snddev_icodec_data snddev_i_nr_off_dv_tx_data = {
	.capability = (SNDDEV_CAP_TX | SNDDEV_CAP_VOICE),
	.name = "nr_off_dv_tx",
	.copp_id = 0,
	.acdb_id = ACDB_ID_NR_OFF_DV_TX,
	.profile = &i_nr_off_dv_tx_profile,
	.channel_mode = 1,
	.pmctl_id = i_nr_off_dv_tx_pmctl_id,
	.pmctl_id_sz = ARRAY_SIZE(i_nr_off_dv_tx_pmctl_id),
	.default_sample_rate = 48000,
#ifdef CONFIG_PANTECH_AUDIO_FAB2200    //20110428 jhsong : feature clearup
	.pamp_on = &msm_snddev_mic_sel_QTR,  //20110301 jhsong : abnormal work after set path.....cause by pointer address so add "&" before func
	.pamp_off = NULL,
#else
	.pamp_on = msm_snddev_tx_route_config,
	.pamp_off = msm_snddev_tx_route_deconfig,
#endif	
};

static struct platform_device msm_nr_off_dv_tx_device = {
	.name = "snddev_icodec",
	.id = 32,
	.dev = { .platform_data = &snddev_i_nr_off_dv_tx_data },
};

#endif // CONFIG_PANTECH_MINI_AUDIO_AUDIENCE1028  //jmlee QTR Route NR_OFF CT TALK TX/RX

static struct adie_codec_action_unit ihs_stereo_rx_48KHz_osr256_actions[] =
	HEADSET_STEREO_RX_LEGACY_48000_OSR_256;

static struct adie_codec_hwsetting_entry ihs_stereo_rx_settings[] = {
	{
		.freq_plan = 48000,
		.osr = 256,
		.actions = ihs_stereo_rx_48KHz_osr256_actions,
		.action_sz = ARRAY_SIZE(ihs_stereo_rx_48KHz_osr256_actions),
	}
};

static struct adie_codec_dev_profile ihs_stereo_rx_profile = {
	.path_type = ADIE_CODEC_RX,
	.settings = ihs_stereo_rx_settings,
	.setting_sz = ARRAY_SIZE(ihs_stereo_rx_settings),
};

static struct snddev_icodec_data snddev_ihs_stereo_rx_data = {
	.capability = (SNDDEV_CAP_RX | SNDDEV_CAP_VOICE),
	.name = "headset_stereo_rx",
	.copp_id = 0,
	.acdb_id = ACDB_ID_HEADSET_SPKR_STEREO,
	.profile = &ihs_stereo_rx_profile,
	.channel_mode = 2,
	.default_sample_rate = 48000,
#ifdef CONFIG_PANTECH_AUDIO_FAB2200    //20110428 jhsong : feature clearup
	.pamp_on = &snd_subsystem_hp_poweron,
	.pamp_off = &snd_subsystem_hp_powerdown,
	.property = 0x00,
#else
	.pamp_on = NULL,
	.pamp_off = NULL,
	.property = SIDE_TONE_MASK,
#endif
	.max_voice_rx_vol[VOC_NB_INDEX] = -700,
	.min_voice_rx_vol[VOC_NB_INDEX] = -2200,
	.max_voice_rx_vol[VOC_WB_INDEX] = -900,
	.min_voice_rx_vol[VOC_WB_INDEX] = -2400
};

static struct platform_device msm_ihs_stereo_rx_device = {
	.name = "snddev_icodec",
	.id = 2,
	.dev = { .platform_data = &snddev_ihs_stereo_rx_data },
};

static struct adie_codec_action_unit ihs_mono_rx_48KHz_osr256_actions[] =
	HEADSET_RX_LEGACY_48000_OSR_256;

static struct adie_codec_hwsetting_entry ihs_mono_rx_settings[] = {
	{
		.freq_plan = 48000,
		.osr = 256,
		.actions = ihs_mono_rx_48KHz_osr256_actions,
		.action_sz = ARRAY_SIZE(ihs_mono_rx_48KHz_osr256_actions),
	}
};

static struct adie_codec_dev_profile ihs_mono_rx_profile = {
	.path_type = ADIE_CODEC_RX,
	.settings = ihs_mono_rx_settings,
	.setting_sz = ARRAY_SIZE(ihs_mono_rx_settings),
};

static struct snddev_icodec_data snddev_ihs_mono_rx_data = {
	.capability = (SNDDEV_CAP_RX | SNDDEV_CAP_VOICE),
	.name = "headset_mono_rx",
	.copp_id = 0,
	.acdb_id = ACDB_ID_HEADSET_SPKR_MONO,
	.profile = &ihs_mono_rx_profile,
	.channel_mode = 1,
	.default_sample_rate = 48000,
#ifdef CONFIG_PANTECH_AUDIO_FAB2200    //20110428 jhsong : feature clearup
	.pamp_on = &snd_subsystem_hp_poweron,
	.pamp_off = &snd_subsystem_hp_powerdown,
#else
	.pamp_on = NULL,
	.pamp_off = NULL,
#endif
	.property = SIDE_TONE_MASK,
	.max_voice_rx_vol[VOC_NB_INDEX] = -700,
	.min_voice_rx_vol[VOC_NB_INDEX] = -2200,
	.max_voice_rx_vol[VOC_WB_INDEX] = -900,
	.min_voice_rx_vol[VOC_WB_INDEX] = -2400,

};

static struct platform_device msm_ihs_mono_rx_device = {
	.name = "snddev_icodec",
	.id = 3,
	.dev = { .platform_data = &snddev_ihs_mono_rx_data },
};

static struct adie_codec_action_unit ihs_ffa_stereo_rx_48KHz_osr256_actions[] =
	HEADSET_STEREO_RX_CAPLESS_48000_OSR_256;

static struct adie_codec_hwsetting_entry ihs_ffa_stereo_rx_settings[] = {
	{
		.freq_plan = 48000,
		.osr = 256,
		.actions = ihs_ffa_stereo_rx_48KHz_osr256_actions,
		.action_sz = ARRAY_SIZE(ihs_ffa_stereo_rx_48KHz_osr256_actions),
	}
};

#ifdef CONFIG_DEBUG_FS
static struct adie_codec_action_unit
	ihs_ffa_stereo_rx_class_d_legacy_48KHz_osr256_actions[] =
	HEADSET_STEREO_RX_CLASS_D_LEGACY_48000_OSR_256;

static struct adie_codec_hwsetting_entry
	ihs_ffa_stereo_rx_class_d_legacy_settings[] = {
	{
		.freq_plan = 48000,
		.osr = 256,
		.actions =
		ihs_ffa_stereo_rx_class_d_legacy_48KHz_osr256_actions,
		.action_sz = ARRAY_SIZE
		(ihs_ffa_stereo_rx_class_d_legacy_48KHz_osr256_actions),
	}
};

static struct adie_codec_action_unit
	ihs_ffa_stereo_rx_class_ab_legacy_48KHz_osr256_actions[] =
	HEADSET_STEREO_RX_LEGACY_48000_OSR_256;

static struct adie_codec_hwsetting_entry
	ihs_ffa_stereo_rx_class_ab_legacy_settings[] = {
	{
		.freq_plan = 48000,
		.osr = 256,
		.actions =
		ihs_ffa_stereo_rx_class_ab_legacy_48KHz_osr256_actions,
		.action_sz = ARRAY_SIZE
		(ihs_ffa_stereo_rx_class_ab_legacy_48KHz_osr256_actions),
	}
};
#endif

static struct adie_codec_dev_profile ihs_ffa_stereo_rx_profile = {
	.path_type = ADIE_CODEC_RX,
	.settings = ihs_ffa_stereo_rx_settings,
	.setting_sz = ARRAY_SIZE(ihs_ffa_stereo_rx_settings),
};

static struct snddev_icodec_data snddev_ihs_ffa_stereo_rx_data = {
	.capability = (SNDDEV_CAP_RX | SNDDEV_CAP_VOICE),
	.name = "headset_stereo_rx",
	.copp_id = 0,
	.acdb_id = ACDB_ID_HEADSET_SPKR_STEREO,
	.profile = &ihs_ffa_stereo_rx_profile,
	.channel_mode = 2,
	.default_sample_rate = 48000,
	.voltage_on = msm_snddev_hsed_voltage_on,
	.voltage_off = msm_snddev_hsed_voltage_off,
	.max_voice_rx_vol[VOC_NB_INDEX] = -700,
	.min_voice_rx_vol[VOC_NB_INDEX] = -2200,
	.max_voice_rx_vol[VOC_WB_INDEX] = -900,
	.min_voice_rx_vol[VOC_WB_INDEX] = -2400,
};

static struct platform_device msm_ihs_ffa_stereo_rx_device = {
	.name = "snddev_icodec",
	.id = 4,
	.dev = { .platform_data = &snddev_ihs_ffa_stereo_rx_data },
};

static struct adie_codec_action_unit ihs_ffa_mono_rx_48KHz_osr256_actions[] =
	HEADSET_RX_CAPLESS_48000_OSR_256;

static struct adie_codec_hwsetting_entry ihs_ffa_mono_rx_settings[] = {
	{
		.freq_plan = 48000,
		.osr = 256,
		.actions = ihs_ffa_mono_rx_48KHz_osr256_actions,
		.action_sz = ARRAY_SIZE(ihs_ffa_mono_rx_48KHz_osr256_actions),
	}
};

static struct adie_codec_dev_profile ihs_ffa_mono_rx_profile = {
	.path_type = ADIE_CODEC_RX,
	.settings = ihs_ffa_mono_rx_settings,
	.setting_sz = ARRAY_SIZE(ihs_ffa_mono_rx_settings),
};

static struct snddev_icodec_data snddev_ihs_ffa_mono_rx_data = {
	.capability = (SNDDEV_CAP_RX | SNDDEV_CAP_VOICE),
	.name = "headset_mono_rx",
	.copp_id = 0,
	.acdb_id = ACDB_ID_HEADSET_SPKR_MONO,
	.profile = &ihs_ffa_mono_rx_profile,
	.channel_mode = 1,
	.default_sample_rate = 48000,
	.pamp_on = msm_snddev_hsed_voltage_on,
	.pamp_off = msm_snddev_hsed_voltage_off,
	.max_voice_rx_vol[VOC_NB_INDEX] = -700,
	.min_voice_rx_vol[VOC_NB_INDEX] = -2200,
	.max_voice_rx_vol[VOC_WB_INDEX] = -900,
	.min_voice_rx_vol[VOC_WB_INDEX] = -2400,
};

static struct platform_device msm_ihs_ffa_mono_rx_device = {
	.name = "snddev_icodec",
	.id = 5,
	.dev = { .platform_data = &snddev_ihs_ffa_mono_rx_data },
};

static struct adie_codec_action_unit ihs_mono_tx_8KHz_osr256_actions[] =
	HEADSET_MONO_TX_8000_OSR_256;

static struct adie_codec_action_unit ihs_mono_tx_16KHz_osr256_actions[] =
	HEADSET_MONO_TX_16000_OSR_256;

static struct adie_codec_action_unit ihs_mono_tx_48KHz_osr256_actions[] =
	HEADSET_MONO_TX_48000_OSR_256;

static struct adie_codec_hwsetting_entry ihs_mono_tx_settings[] = {
	{
		.freq_plan = 8000,
		.osr = 256,
		.actions = ihs_mono_tx_8KHz_osr256_actions,
		.action_sz = ARRAY_SIZE(ihs_mono_tx_8KHz_osr256_actions),
	},
	{
		.freq_plan = 16000,
		.osr = 256,
		.actions = ihs_mono_tx_16KHz_osr256_actions,
		.action_sz = ARRAY_SIZE(ihs_mono_tx_16KHz_osr256_actions),
	},
	{
		.freq_plan = 48000,
		.osr = 256,
		.actions = ihs_mono_tx_48KHz_osr256_actions,
		.action_sz = ARRAY_SIZE(ihs_mono_tx_48KHz_osr256_actions),
	}
};

#ifdef CONFIG_PANTECH_AUDIO_FAB2200    //20110428 jhsong : feature clearup
//static enum hsed_controller i_hs_mono_tx_pmctl_id[] = {PM_HSED_CONTROLLER_1};
#endif

static struct adie_codec_dev_profile ihs_mono_tx_profile = {
	.path_type = ADIE_CODEC_TX,
	.settings = ihs_mono_tx_settings,
	.setting_sz = ARRAY_SIZE(ihs_mono_tx_settings),
};

static struct snddev_icodec_data snddev_ihs_mono_tx_data = {
	.capability = (SNDDEV_CAP_TX | SNDDEV_CAP_VOICE),
	.name = "headset_mono_tx",
	.copp_id = 0,
	.acdb_id = ACDB_ID_HEADSET_MIC,
	.profile = &ihs_mono_tx_profile,
	.channel_mode = 1,
#ifdef CONFIG_PANTECH_AUDIO_FAB2200    //20110428 jhsong : feature clearup
//	.pmctl_id = i_hs_mono_tx_pmctl_id,
//	.pmctl_id_sz = ARRAY_SIZE(i_hs_mono_tx_pmctl_id),
	.default_sample_rate = 48000,
	.pamp_on = NULL,
	.pamp_off = NULL,
#else
	.pmctl_id = NULL,
	.pmctl_id_sz = 0,
	.default_sample_rate = 48000,
	.pamp_on = msm_snddev_tx_route_config,
	.pamp_off = msm_snddev_tx_route_deconfig,
#endif	
};

static struct platform_device msm_ihs_mono_tx_device = {
	.name = "snddev_icodec",
	.id = 6,
	.dev = { .platform_data = &snddev_ihs_mono_tx_data },
};

#ifdef CONFIG_PANTECH_ASR_PATH    //20110428 jhsong : feature clearup

static struct adie_codec_action_unit ihs_asr_mono_tx_48KHz_osr256_actions[] =
	ASR_HEADSET_MONO_TX_48000_OSR_256;

static struct adie_codec_hwsetting_entry ihs_asr_mono_tx_settings[] = {
	{
		.freq_plan = 8000,
		.osr = 256,
		.actions = ihs_mono_tx_8KHz_osr256_actions,
		.action_sz = ARRAY_SIZE(ihs_mono_tx_8KHz_osr256_actions),
	},
	{
		.freq_plan = 16000,
		.osr = 256,
		.actions = ihs_mono_tx_16KHz_osr256_actions,
		.action_sz = ARRAY_SIZE(ihs_mono_tx_16KHz_osr256_actions),
	},
	{
		.freq_plan = 48000,
		.osr = 256,
		.actions = ihs_asr_mono_tx_48KHz_osr256_actions,
		.action_sz = ARRAY_SIZE(ihs_asr_mono_tx_48KHz_osr256_actions),
	}
};

static struct adie_codec_dev_profile ihs_asr_mono_tx_profile = {
	.path_type = ADIE_CODEC_TX,
	.settings = ihs_asr_mono_tx_settings,
	.setting_sz = ARRAY_SIZE(ihs_asr_mono_tx_settings),
};

static struct snddev_icodec_data snddev_asr_ihs_mono_tx_data = {
	.capability = (SNDDEV_CAP_TX | SNDDEV_CAP_VOICE),
	.name = "asr_headset_tx",
	.copp_id = 0,
	.acdb_id = ACDB_ID_ASR_HEADSET_TX,
	.profile = &ihs_asr_mono_tx_profile,
	.channel_mode = 1,
#ifdef CONFIG_PANTECH_AUDIO_FAB2200    //20110428 jhsong : feature clearup
//	.pmctl_id = i_hs_mono_tx_pmctl_id,
//	.pmctl_id_sz = ARRAY_SIZE(i_hs_mono_tx_pmctl_id),
	.default_sample_rate = 48000,
	.pamp_on = NULL,
	.pamp_off = NULL,
#else
	.pmctl_id = NULL,
	.pmctl_id_sz = 0,
	.default_sample_rate = 48000,
	.pamp_on = msm_snddev_tx_route_config,
	.pamp_off = msm_snddev_tx_route_deconfig,
#endif	
};

static struct platform_device msm_asr_headset_tx_device = {
	.name = "snddev_icodec",
	.id = 34,
	.dev = { .platform_data = &snddev_asr_ihs_mono_tx_data },
};
#endif

static struct adie_codec_action_unit ifmradio_handset_osr64_actions[] =
	FM_HANDSET_OSR_64;

static struct adie_codec_hwsetting_entry ifmradio_handset_settings[] = {
	{
		.freq_plan = 8000,
		.osr = 256,
		.actions = ifmradio_handset_osr64_actions,
		.action_sz = ARRAY_SIZE(ifmradio_handset_osr64_actions),
	}
};

static struct adie_codec_dev_profile ifmradio_handset_profile = {
	.path_type = ADIE_CODEC_RX,
	.settings = ifmradio_handset_settings,
	.setting_sz = ARRAY_SIZE(ifmradio_handset_settings),
};

static struct snddev_icodec_data snddev_ifmradio_handset_data = {
	.capability = (SNDDEV_CAP_RX | SNDDEV_CAP_FM),
	.name = "fmradio_handset_rx",
	.copp_id = 0,
	.acdb_id = ACDB_ID_LP_FM_SPKR_PHONE_STEREO_RX,
	.profile = &ifmradio_handset_profile,
	.channel_mode = 1,
	.default_sample_rate = 8000,
	.pamp_on = NULL,
	.pamp_off = NULL,
	.dev_vol_type = SNDDEV_DEV_VOL_DIGITAL,
};

static struct platform_device msm_ifmradio_handset_device = {
	.name = "snddev_icodec",
	.id = 7,
	.dev = { .platform_data = &snddev_ifmradio_handset_data },
};


#if 0 //def CONFIG_PANTECH_MINI_AUDIO_FAB2200  //20110126 jhsong : FAB2200 AMP
#define _SPEAKER_RX_48000_OSR_256 \
	{{ADIE_CODEC_ACTION_STAGE_REACHED, ADIE_CODEC_FLASH_IMAGE}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x80, 0x02, 0x02)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x80, 0x02, 0x00)}, \
	{ADIE_CODEC_ACTION_STAGE_REACHED, ADIE_CODEC_DIGITAL_READY}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x24, 0x6F, 0x64)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x25, 0x0F, 0x0B)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x26, 0xfc, 0xfc)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x37, 0xe2, 0x82/*, 0xe2, 0xa2*/)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3A, 0xFF, 0x2B)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3d, 0xFF, 0x55)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x83, 0x23, 0x23)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x23, 0xff, 0x20)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x33, 0x8a, 0x8a)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x33, 0x05, 0x05)}, \
	{ADIE_CODEC_ACTION_DELAY_WAIT, 0x7530}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x84, 0xff, 0xfe/*0x00*/)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x85, 0xff, 0xfe/*0x00*/)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x8A, 0x0f, 0x0c/*, 0x05, 0x04*/)}, \
	{ADIE_CODEC_ACTION_STAGE_REACHED, ADIE_CODEC_DIGITAL_ANALOG_READY}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x8a, 0x03, 0x03/*, 0x01, 0x01*/)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x33, 0x40, 0x00)}, \
	{ADIE_CODEC_ACTION_STAGE_REACHED, ADIE_CODEC_ANALOG_OFF}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x33, 0x80, 0x00)}, \
	{ADIE_CODEC_ACTION_STAGE_REACHED, ADIE_CODEC_DIGITAL_OFF} }

static struct adie_codec_action_unit ispeaker_rx_48KHz_osr256_actions[] =
   _SPEAKER_RX_48000_OSR_256;
#else
static struct adie_codec_action_unit ispeaker_rx_48KHz_osr256_actions[] =
   SPEAKER_STEREO_RX_48000_OSR_256;
#endif

static struct adie_codec_hwsetting_entry ispeaker_rx_settings[] = {
	{
		.freq_plan = 48000,
		.osr = 256,
		.actions = ispeaker_rx_48KHz_osr256_actions,
		.action_sz = ARRAY_SIZE(ispeaker_rx_48KHz_osr256_actions),
	}
};

static struct adie_codec_dev_profile ispeaker_rx_profile = {
	.path_type = ADIE_CODEC_RX,
	.settings = ispeaker_rx_settings,
	.setting_sz = ARRAY_SIZE(ispeaker_rx_settings),
};

static struct snddev_icodec_data snddev_ispeaker_rx_data = {
	.capability = (SNDDEV_CAP_RX | SNDDEV_CAP_VOICE),
	.name = "speaker_stereo_rx",
	.copp_id = 0,
	.acdb_id = ACDB_ID_SPKR_PHONE_STEREO,
	.profile = &ispeaker_rx_profile,
	.channel_mode = 2,
	.pmctl_id = NULL,
	.pmctl_id_sz = 0,
	.default_sample_rate = 48000,
#ifdef CONFIG_PANTECH_AUDIO_FAB2200    //20110428 jhsong : feature clearup
	.pamp_on = &snd_subsystem_sp_poweron,
	.pamp_off = &snd_subsystem_sp_powerdown,
#else
	.pamp_on = &msm_snddev_poweramp_on,
	.pamp_off = &msm_snddev_poweramp_off,
#endif
	.max_voice_rx_vol[VOC_NB_INDEX] = 1000,
	.min_voice_rx_vol[VOC_NB_INDEX] = -500,
	.max_voice_rx_vol[VOC_WB_INDEX] = 1000,
	.min_voice_rx_vol[VOC_WB_INDEX] = -500,
};

static struct platform_device msm_ispeaker_rx_device = {
	.name = "snddev_icodec",
	.id = 8,
	.dev = { .platform_data = &snddev_ispeaker_rx_data },

};

static struct adie_codec_action_unit ifmradio_speaker_osr64_actions[] =
	FM_SPEAKER_OSR_64;

static struct adie_codec_hwsetting_entry ifmradio_speaker_settings[] = {
	{
		.freq_plan = 8000,
		.osr = 256,
		.actions = ifmradio_speaker_osr64_actions,
		.action_sz = ARRAY_SIZE(ifmradio_speaker_osr64_actions),
	}
};

static struct adie_codec_dev_profile ifmradio_speaker_profile = {
	.path_type = ADIE_CODEC_RX,
	.settings = ifmradio_speaker_settings,
	.setting_sz = ARRAY_SIZE(ifmradio_speaker_settings),
};

static struct snddev_icodec_data snddev_ifmradio_speaker_data = {
	.capability = (SNDDEV_CAP_RX | SNDDEV_CAP_FM),
	.name = "fmradio_speaker_rx",
	.copp_id = 0,
	.acdb_id = ACDB_ID_LP_FM_SPKR_PHONE_STEREO_RX,
	.profile = &ifmradio_speaker_profile,
	.channel_mode = 1,
	.default_sample_rate = 8000,
#ifdef CONFIG_PANTECH_AUDIO_FAB2200    //20110428 jhsong : feature clearup
	.pamp_on = NULL,
	.pamp_off = NULL,
#else
	.pamp_on = &msm_snddev_poweramp_on,
	.pamp_off = &msm_snddev_poweramp_off,
#endif	
	.dev_vol_type = SNDDEV_DEV_VOL_DIGITAL,
};

static struct platform_device msm_ifmradio_speaker_device = {
	.name = "snddev_icodec",
	.id = 9,
	.dev = { .platform_data = &snddev_ifmradio_speaker_data },
};

static struct adie_codec_action_unit ifmradio_headset_osr64_actions[] =
	FM_HEADSET_STEREO_CLASS_D_LEGACY_OSR_64;

static struct adie_codec_hwsetting_entry ifmradio_headset_settings[] = {
	{
		.freq_plan = 8000,
		.osr = 256,
		.actions = ifmradio_headset_osr64_actions,
		.action_sz = ARRAY_SIZE(ifmradio_headset_osr64_actions),
	}
};

static struct adie_codec_dev_profile ifmradio_headset_profile = {
	.path_type = ADIE_CODEC_RX,
	.settings = ifmradio_headset_settings,
	.setting_sz = ARRAY_SIZE(ifmradio_headset_settings),
};

static struct snddev_icodec_data snddev_ifmradio_headset_data = {
	.capability = (SNDDEV_CAP_RX | SNDDEV_CAP_FM),
	.name = "fmradio_headset_rx",
	.copp_id = 0,
	.acdb_id = ACDB_ID_LP_FM_HEADSET_SPKR_STEREO_RX,
	.profile = &ifmradio_headset_profile,
	.channel_mode = 1,
	.default_sample_rate = 8000,
	.pamp_on = NULL,
	.pamp_off = NULL,
	.dev_vol_type = SNDDEV_DEV_VOL_DIGITAL,
};

static struct platform_device msm_ifmradio_headset_device = {
	.name = "snddev_icodec",
	.id = 10,
	.dev = { .platform_data = &snddev_ifmradio_headset_data },
};


static struct adie_codec_action_unit ifmradio_ffa_headset_osr64_actions[] =
	FM_HEADSET_CLASS_AB_STEREO_CAPLESS_OSR_64;

static struct adie_codec_hwsetting_entry ifmradio_ffa_headset_settings[] = {
	{
		.freq_plan = 8000,
		.osr = 256,
		.actions = ifmradio_ffa_headset_osr64_actions,
		.action_sz = ARRAY_SIZE(ifmradio_ffa_headset_osr64_actions),
	}
};

static struct adie_codec_dev_profile ifmradio_ffa_headset_profile = {
	.path_type = ADIE_CODEC_RX,
	.settings = ifmradio_ffa_headset_settings,
	.setting_sz = ARRAY_SIZE(ifmradio_ffa_headset_settings),
};

static struct snddev_icodec_data snddev_ifmradio_ffa_headset_data = {
	.capability = (SNDDEV_CAP_RX | SNDDEV_CAP_FM),
	.name = "fmradio_headset_rx",
	.copp_id = 0,
	.acdb_id = ACDB_ID_LP_FM_HEADSET_SPKR_STEREO_RX,
	.profile = &ifmradio_ffa_headset_profile,
	.channel_mode = 1,
	.default_sample_rate = 8000,
	.pamp_on = msm_snddev_hsed_voltage_on,
	.pamp_off = msm_snddev_hsed_voltage_off,
	.dev_vol_type = SNDDEV_DEV_VOL_DIGITAL,
};

static struct platform_device msm_ifmradio_ffa_headset_device = {
	.name = "snddev_icodec",
	.id = 11,
	.dev = { .platform_data = &snddev_ifmradio_ffa_headset_data },
};

static struct snddev_ecodec_data snddev_bt_sco_earpiece_data = {
	.capability = (SNDDEV_CAP_RX | SNDDEV_CAP_VOICE),
	.name = "bt_sco_rx",
	.copp_id = 1,
	.acdb_id = ACDB_ID_BT_SCO_SPKR,
	.channel_mode = 1,
	.conf_pcm_ctl_val = BT_SCO_PCM_CTL_VAL,
	.conf_aux_codec_intf = BT_SCO_AUX_CODEC_INTF,
	.conf_data_format_padding_val = BT_SCO_DATA_FORMAT_PADDING,
	.max_voice_rx_vol[VOC_NB_INDEX] = 400,
	.min_voice_rx_vol[VOC_NB_INDEX] = -1100,
	.max_voice_rx_vol[VOC_WB_INDEX] = 400,
	.min_voice_rx_vol[VOC_WB_INDEX] = -1100,
#ifdef CONFIG_PANTECH_MINI_AUDIO_AUDIENCE1028  // jmlee
	.pamp_on = NULL,
	.pamp_off = NULL,
#endif	
};

static struct snddev_ecodec_data snddev_bt_sco_mic_data = {
	.capability = (SNDDEV_CAP_TX | SNDDEV_CAP_VOICE),
	.name = "bt_sco_tx",
	.copp_id = 1,
	.acdb_id = ACDB_ID_BT_SCO_MIC,
	.channel_mode = 1,
	.conf_pcm_ctl_val = BT_SCO_PCM_CTL_VAL,
	.conf_aux_codec_intf = BT_SCO_AUX_CODEC_INTF,
	.conf_data_format_padding_val = BT_SCO_DATA_FORMAT_PADDING,
#ifdef CONFIG_PANTECH_MINI_AUDIO_AUDIENCE1028 // jmlee
	.pamp_on = NULL,
	.pamp_off = NULL,
#endif		
};

#ifdef CONFIG_PANTECH_MINI_AUDIO_AUDIENCE1028 // jmlee
///////////////////////////////////////////////////////////////////////////
// NR_CT_RX
///////////////////////////////////////////////////////////////////////////
// NR_FT_RX
///////////////////////////////////////////////////////////////////////////
// NR_DV_RX
#ifdef CONFIG_PANTECH_MINI_AUDIO_AUDIENCE1028 //20110224 jhsong
static enum hsed_controller i_nr_tx_pmctl_id[] = {PM_HSED_CONTROLLER_0};
#endif

static struct snddev_ecodec_data snddev_nr_ct_rx_data = {
	.capability = (SNDDEV_CAP_RX | SNDDEV_CAP_VOICE),
	.name = "nr_ct_rx",
	.copp_id = 1,
	.acdb_id = ACDB_ID_NR_CT_RX, //
	.channel_mode = 1,
	.conf_pcm_ctl_val = BT_SCO_PCM_CTL_VAL,
	.conf_aux_codec_intf = BT_SCO_AUX_CODEC_INTF,
	.conf_data_format_padding_val = BT_SCO_DATA_FORMAT_PADDING,
	.max_voice_rx_vol[VOC_NB_INDEX] = 400,
	.min_voice_rx_vol[VOC_NB_INDEX] = -1100,
	.max_voice_rx_vol[VOC_WB_INDEX] = 400,
	.min_voice_rx_vol[VOC_WB_INDEX] = -1100,
#ifdef CONFIG_PANTECH_MINI_AUDIO_AUDIENCE1028 // jmlee
	.pamp_on = &audience_a1028_ct_on,    //20110301 jhsong : abnormal work after set path.....cause by pointer address so add "&" before func
	.pamp_off = &audience_a1028_off,  //20110301 jhsong : abnormal work after set path.....cause by pointer address so add "&" before func
#endif		
};

static struct snddev_ecodec_data snddev_nr_ct_tx_data = {
	.capability = (SNDDEV_CAP_TX | SNDDEV_CAP_VOICE),
	.name = "nr_ct_tx",
	.copp_id = 1,
	.acdb_id = ACDB_ID_NR_CT_TX, //
	.channel_mode = 1,
	.conf_pcm_ctl_val = BT_SCO_PCM_CTL_VAL,
	.conf_aux_codec_intf = BT_SCO_AUX_CODEC_INTF,
	.conf_data_format_padding_val = BT_SCO_DATA_FORMAT_PADDING,
#ifdef CONFIG_PANTECH_MINI_AUDIO_AUDIENCE1028 // jmlee
	.pmctl_id = i_nr_tx_pmctl_id,  //20110224 jhsong
	.pmctl_id_sz = ARRAY_SIZE(i_nr_tx_pmctl_id),  //20110224 jhsong

	.pamp_on = &audience_a1028_ct_on,  //20110301 jhsong : abnormal work after set path.....cause by pointer address so add "&" before func
	.pamp_off = &audience_a1028_off, //20110301 jhsong : abnormal work after set path.....cause by pointer address so add "&" before func
#endif		
};

static struct snddev_ecodec_data snddev_nr_ft_rx_data = {
	.capability = (SNDDEV_CAP_RX | SNDDEV_CAP_VOICE),
	.name = "nr_ft_rx",
	.copp_id = 1,
	.acdb_id = ACDB_ID_NR_FT_RX, //
	.channel_mode = 1,
	.conf_pcm_ctl_val = BT_SCO_PCM_CTL_VAL,
	.conf_aux_codec_intf = BT_SCO_AUX_CODEC_INTF,
	.conf_data_format_padding_val = BT_SCO_DATA_FORMAT_PADDING,
	.max_voice_rx_vol[VOC_NB_INDEX] = 400,
	.min_voice_rx_vol[VOC_NB_INDEX] = -1100,
	.max_voice_rx_vol[VOC_WB_INDEX] = 400,
	.min_voice_rx_vol[VOC_WB_INDEX] = -1100,
#ifdef CONFIG_PANTECH_MINI_AUDIO_AUDIENCE1028 // jmlee
	.pamp_on = &audience_a1028_ft_on,  //20110301 jhsong : abnormal work after set path.....cause by pointer address so add "&" before func
	.pamp_off = &audience_a1028_off,  //20110301 jhsong : abnormal work after set path.....cause by pointer address so add "&" before func
#endif		
};

static struct snddev_ecodec_data snddev_nr_ft_tx_data = {
	.capability = (SNDDEV_CAP_TX | SNDDEV_CAP_VOICE),
	.name = "nr_ft_tx",
	.copp_id = 1,
	.acdb_id = ACDB_ID_NR_FT_TX, //
	.channel_mode = 1,
	.conf_pcm_ctl_val = BT_SCO_PCM_CTL_VAL,
	.conf_aux_codec_intf = BT_SCO_AUX_CODEC_INTF,
	.conf_data_format_padding_val = BT_SCO_DATA_FORMAT_PADDING,
#ifdef CONFIG_PANTECH_MINI_AUDIO_AUDIENCE1028 // jmlee
	.pmctl_id = i_nr_tx_pmctl_id,  //20110224 jhsong
	.pmctl_id_sz = ARRAY_SIZE(i_nr_tx_pmctl_id),  //20110224 jhsong

	.pamp_on = &audience_a1028_ft_on,  //20110301 jhsong : abnormal work after set path.....cause by pointer address so add "&" before func
	.pamp_off = &audience_a1028_off,  //20110301 jhsong : abnormal work after set path.....cause by pointer address so add "&" before func
#endif		
};

static struct snddev_ecodec_data snddev_nr_dv_rx_data = {
	.capability = (SNDDEV_CAP_RX | SNDDEV_CAP_VOICE),
	.name = "nr_dv_rx",
	.copp_id = 1,
	.acdb_id = ACDB_ID_NR_DV_RX, //
	.channel_mode = 1,
	.conf_pcm_ctl_val = BT_SCO_PCM_CTL_VAL,
	.conf_aux_codec_intf = BT_SCO_AUX_CODEC_INTF,
	.conf_data_format_padding_val = BT_SCO_DATA_FORMAT_PADDING,
	.max_voice_rx_vol[VOC_NB_INDEX] = 400,
	.min_voice_rx_vol[VOC_NB_INDEX] = -1100,
	.max_voice_rx_vol[VOC_WB_INDEX] = 400,
	.min_voice_rx_vol[VOC_WB_INDEX] = -1100,
#ifdef CONFIG_PANTECH_MINI_AUDIO_AUDIENCE1028 // jmlee
	.pamp_on = &audience_a1028_dv_on,  //20110301 jhsong : abnormal work after set path.....cause by pointer address so add "&" before func
	.pamp_off = &audience_a1028_off,  //20110301 jhsong : abnormal work after set path.....cause by pointer address so add "&" before func
#endif		
};

static struct snddev_ecodec_data snddev_nr_dv_tx_data = {
	.capability = (SNDDEV_CAP_TX | SNDDEV_CAP_VOICE),
	.name = "nr_dv_tx",
	.copp_id = 1,
	.acdb_id = ACDB_ID_NR_DV_TX, //
	.channel_mode = 1,
	.conf_pcm_ctl_val = BT_SCO_PCM_CTL_VAL,
	.conf_aux_codec_intf = BT_SCO_AUX_CODEC_INTF,
	.conf_data_format_padding_val = BT_SCO_DATA_FORMAT_PADDING,
#ifdef CONFIG_PANTECH_MINI_AUDIO_AUDIENCE1028 // jmlee
	.pmctl_id = i_nr_tx_pmctl_id,  //20110224 jhsong
	.pmctl_id_sz = ARRAY_SIZE(i_nr_tx_pmctl_id),  //20110224 jhsong

	.pamp_on = &audience_a1028_dv_on,  //20110301 jhsong : abnormal work after set path.....cause by pointer address so add "&" before func
	.pamp_off = &audience_a1028_off,  //20110301 jhsong : abnormal work after set path.....cause by pointer address so add "&" before func
#endif		
};

#if 0 //ndef CONFIG_PANTECH_MINI_AUDIO_AUDIENCE1028 // jmlee Audience Route NR_OFF
static struct snddev_ecodec_data snddev_nr_off_ct_rx_data = {
	.capability = (SNDDEV_CAP_RX | SNDDEV_CAP_VOICE),
	.name = "nr_off_ct_rx",
	.copp_id = 1,
	.acdb_id = ACDB_ID_NR_OFF_CT_RX, //
	.channel_mode = 1,
	.conf_pcm_ctl_val = BT_SCO_PCM_CTL_VAL,
	.conf_aux_codec_intf = BT_SCO_AUX_CODEC_INTF,
	.conf_data_format_padding_val = BT_SCO_DATA_FORMAT_PADDING,
	.max_voice_rx_vol[VOC_NB_INDEX] = 400,
	.min_voice_rx_vol[VOC_NB_INDEX] = -1100,
	.max_voice_rx_vol[VOC_WB_INDEX] = 400,
	.min_voice_rx_vol[VOC_WB_INDEX] = -1100,
#ifdef CONFIG_PANTECH_MINI_AUDIO_AUDIENCE1028 // jmlee
	.pamp_on = audience_a1028_ct_vpoff_on,
	.pamp_off = audience_a1028_off,
#endif		
};

static struct snddev_ecodec_data snddev_nr_off_ct_tx_data = {
	.capability = (SNDDEV_CAP_TX | SNDDEV_CAP_VOICE),
	.name = "nr_ct_off_tx",
	.copp_id = 1,
	.acdb_id = ACDB_ID_NR_OFF_CT_TX, //
	.channel_mode = 1,
	.conf_pcm_ctl_val = BT_SCO_PCM_CTL_VAL,
	.conf_aux_codec_intf = BT_SCO_AUX_CODEC_INTF,
	.conf_data_format_padding_val = BT_SCO_DATA_FORMAT_PADDING,
#ifdef CONFIG_PANTECH_MINI_AUDIO_AUDIENCE1028 // jmlee
	.pamp_on = audience_a1028_ct_vpoff_on,
	.pamp_off = audience_a1028_off,
#endif		
};

static struct snddev_ecodec_data snddev_nr_off_ft_rx_data = {
	.capability = (SNDDEV_CAP_RX | SNDDEV_CAP_VOICE),
	.name = "nr_off_ft_rx",
	.copp_id = 1,
	.acdb_id = ACDB_ID_NR_OFF_FT_RX, //
	.channel_mode = 1,
	.conf_pcm_ctl_val = BT_SCO_PCM_CTL_VAL,
	.conf_aux_codec_intf = BT_SCO_AUX_CODEC_INTF,
	.conf_data_format_padding_val = BT_SCO_DATA_FORMAT_PADDING,
	.max_voice_rx_vol[VOC_NB_INDEX] = 400,
	.min_voice_rx_vol[VOC_NB_INDEX] = -1100,
	.max_voice_rx_vol[VOC_WB_INDEX] = 400,
	.min_voice_rx_vol[VOC_WB_INDEX] = -1100,
#ifdef CONFIG_PANTECH_MINI_AUDIO_AUDIENCE1028 // jmlee
	.pamp_on = audience_a1028_ft_vpoff_on,
	.pamp_off = audience_a1028_off,
#endif		
};

static struct snddev_ecodec_data snddev_nr_off_ft_tx_data = {
	.capability = (SNDDEV_CAP_TX | SNDDEV_CAP_VOICE),
	.name = "nr_ft_off_tx",
	.copp_id = 1,
	.acdb_id = ACDB_ID_NR_OFF_FT_TX, //
	.channel_mode = 1,
	.conf_pcm_ctl_val = BT_SCO_PCM_CTL_VAL,
	.conf_aux_codec_intf = BT_SCO_AUX_CODEC_INTF,
	.conf_data_format_padding_val = BT_SCO_DATA_FORMAT_PADDING,
#ifdef CONFIG_PANTECH_MINI_AUDIO_AUDIENCE1028 // jmlee
	.pamp_on = audience_a1028_ft_vpoff_on,
	.pamp_off = audience_a1028_off,
#endif		
};

static struct snddev_ecodec_data snddev_nr_off_dv_rx_data = {
	.capability = (SNDDEV_CAP_RX | SNDDEV_CAP_VOICE),
	.name = "nr_off_dv_rx",
	.copp_id = 1,
	.acdb_id = ACDB_ID_NR_OFF_DV_RX, //
	.channel_mode = 1,
	.conf_pcm_ctl_val = BT_SCO_PCM_CTL_VAL,
	.conf_aux_codec_intf = BT_SCO_AUX_CODEC_INTF,
	.conf_data_format_padding_val = BT_SCO_DATA_FORMAT_PADDING,
	.max_voice_rx_vol[VOC_NB_INDEX] = 400,
	.min_voice_rx_vol[VOC_NB_INDEX] = -1100,
	.max_voice_rx_vol[VOC_WB_INDEX] = 400,
	.min_voice_rx_vol[VOC_WB_INDEX] = -1100,
#ifdef CONFIG_PANTECH_MINI_AUDIO_AUDIENCE1028 // jmlee
	.pamp_on = audience_a1028_dv_vpoff_on,
	.pamp_off = audience_a1028_off,
#endif		
};

static struct snddev_ecodec_data snddev_nr_off_dv_tx_data = {
	.capability = (SNDDEV_CAP_TX | SNDDEV_CAP_VOICE),
	.name = "nr_off_dv_tx",
	.copp_id = 1,
	.acdb_id = ACDB_ID_NR_OFF_DV_TX, //
	.channel_mode = 1,
	.conf_pcm_ctl_val = BT_SCO_PCM_CTL_VAL,
	.conf_aux_codec_intf = BT_SCO_AUX_CODEC_INTF,
	.conf_data_format_padding_val = BT_SCO_DATA_FORMAT_PADDING,
#ifdef CONFIG_PANTECH_MINI_AUDIO_AUDIENCE1028 // jmlee
	.pamp_on = audience_a1028_dv_vpoff_on,
	.pamp_off = audience_a1028_off,
#endif		
};
#endif
#endif  // CONFIG_PANTECH_MINI_AUDIO_AUDIENCE1028 // jmlee

struct platform_device msm_bt_sco_earpiece_device = {
	.name = "msm_snddev_ecodec",
	.id = 0,
	.dev = { .platform_data = &snddev_bt_sco_earpiece_data },
};

struct platform_device msm_bt_sco_mic_device = {
	.name = "msm_snddev_ecodec",
	.id = 1,
	.dev = { .platform_data = &snddev_bt_sco_mic_data },
};

#ifdef CONFIG_PANTECH_MINI_BT_GROUPING  //20110421 jhsong : bt grouping for att

static struct snddev_ecodec_data snddev_bt_sco_a_earpiece_data = {
	.capability = (SNDDEV_CAP_RX | SNDDEV_CAP_VOICE),
	.name = "bt_sco_class_a_rx",
	.copp_id = 1,
	.acdb_id = ACDB_ID_BT_SCO_A_SPKR,
	.channel_mode = 1,
	.conf_pcm_ctl_val = BT_SCO_PCM_CTL_VAL,
	.conf_aux_codec_intf = BT_SCO_AUX_CODEC_INTF,
	.conf_data_format_padding_val = BT_SCO_DATA_FORMAT_PADDING,
	.max_voice_rx_vol[VOC_NB_INDEX] = 400,
	.min_voice_rx_vol[VOC_NB_INDEX] = -1100,
	.max_voice_rx_vol[VOC_WB_INDEX] = 400,
	.min_voice_rx_vol[VOC_WB_INDEX] = -1100,
#ifdef CONFIG_PANTECH_MINI_AUDIO_AUDIENCE1028  // jmlee
	.pamp_on = NULL,
	.pamp_off = NULL,
#endif	
};

static struct snddev_ecodec_data snddev_bt_sco_a_mic_data = {
	.capability = (SNDDEV_CAP_TX | SNDDEV_CAP_VOICE),
	.name = "bt_sco_class_a_tx",
	.copp_id = 1,
	.acdb_id = ACDB_ID_BT_SCO_A_MIC,
	.channel_mode = 1,
	.conf_pcm_ctl_val = BT_SCO_PCM_CTL_VAL,
	.conf_aux_codec_intf = BT_SCO_AUX_CODEC_INTF,
	.conf_data_format_padding_val = BT_SCO_DATA_FORMAT_PADDING,
#ifdef CONFIG_PANTECH_MINI_AUDIO_AUDIENCE1028 // jmlee
	.pamp_on = NULL,
	.pamp_off = NULL,
#endif		
};

struct platform_device msm_bt_sco_a_earpiece_device = {
	.name = "msm_snddev_ecodec",
	.id = 8,
	.dev = { .platform_data = &snddev_bt_sco_a_earpiece_data },
};

struct platform_device msm_bt_sco_a_mic_device = {
	.name = "msm_snddev_ecodec",
	.id = 9,
	.dev = { .platform_data = &snddev_bt_sco_a_mic_data },
};

static struct snddev_ecodec_data snddev_bt_sco_b_earpiece_data = {
	.capability = (SNDDEV_CAP_RX | SNDDEV_CAP_VOICE),
	.name = "bt_sco_class_b_rx",
	.copp_id = 1,
	.acdb_id = ACDB_ID_BT_SCO_B_SPKR,
	.channel_mode = 1,
	.conf_pcm_ctl_val = BT_SCO_PCM_CTL_VAL,
	.conf_aux_codec_intf = BT_SCO_AUX_CODEC_INTF,
	.conf_data_format_padding_val = BT_SCO_DATA_FORMAT_PADDING,
	.max_voice_rx_vol[VOC_NB_INDEX] = 400,
	.min_voice_rx_vol[VOC_NB_INDEX] = -1100,
	.max_voice_rx_vol[VOC_WB_INDEX] = 400,
	.min_voice_rx_vol[VOC_WB_INDEX] = -1100,
#ifdef CONFIG_PANTECH_MINI_AUDIO_AUDIENCE1028  // jmlee
	.pamp_on = NULL,
	.pamp_off = NULL,
#endif	
};

static struct snddev_ecodec_data snddev_bt_sco_b_mic_data = {
	.capability = (SNDDEV_CAP_TX | SNDDEV_CAP_VOICE),
	.name = "bt_sco_class_b_tx",
	.copp_id = 1,
	.acdb_id = ACDB_ID_BT_SCO_B_MIC,
	.channel_mode = 1,
	.conf_pcm_ctl_val = BT_SCO_PCM_CTL_VAL,
	.conf_aux_codec_intf = BT_SCO_AUX_CODEC_INTF,
	.conf_data_format_padding_val = BT_SCO_DATA_FORMAT_PADDING,
#ifdef CONFIG_PANTECH_MINI_AUDIO_AUDIENCE1028 // jmlee
	.pamp_on = NULL,
	.pamp_off = NULL,
#endif		
};

struct platform_device msm_bt_sco_b_earpiece_device = {
	.name = "msm_snddev_ecodec",
	.id = 10,
	.dev = { .platform_data = &snddev_bt_sco_b_earpiece_data },
};

struct platform_device msm_bt_sco_b_mic_device = {
	.name = "msm_snddev_ecodec",
	.id = 11,
	.dev = { .platform_data = &snddev_bt_sco_b_mic_data },
};

static struct snddev_ecodec_data snddev_bt_sco_c_earpiece_data = {
	.capability = (SNDDEV_CAP_RX | SNDDEV_CAP_VOICE),
	.name = "bt_sco_class_c_rx",
	.copp_id = 1,
	.acdb_id = ACDB_ID_BT_SCO_C_SPKR,
	.channel_mode = 1,
	.conf_pcm_ctl_val = BT_SCO_PCM_CTL_VAL,
	.conf_aux_codec_intf = BT_SCO_AUX_CODEC_INTF,
	.conf_data_format_padding_val = BT_SCO_DATA_FORMAT_PADDING,
	.max_voice_rx_vol[VOC_NB_INDEX] = 400,
	.min_voice_rx_vol[VOC_NB_INDEX] = -1100,
	.max_voice_rx_vol[VOC_WB_INDEX] = 400,
	.min_voice_rx_vol[VOC_WB_INDEX] = -1100,
#ifdef CONFIG_PANTECH_MINI_AUDIO_AUDIENCE1028  // jmlee
	.pamp_on = NULL,
	.pamp_off = NULL,
#endif	
};

static struct snddev_ecodec_data snddev_bt_sco_c_mic_data = {
	.capability = (SNDDEV_CAP_TX | SNDDEV_CAP_VOICE),
	.name = "bt_sco_class_c_tx",
	.copp_id = 1,
	.acdb_id = ACDB_ID_BT_SCO_C_MIC,
	.channel_mode = 1,
	.conf_pcm_ctl_val = BT_SCO_PCM_CTL_VAL,
	.conf_aux_codec_intf = BT_SCO_AUX_CODEC_INTF,
	.conf_data_format_padding_val = BT_SCO_DATA_FORMAT_PADDING,
#ifdef CONFIG_PANTECH_MINI_AUDIO_AUDIENCE1028 // jmlee
	.pamp_on = NULL,
	.pamp_off = NULL,
#endif		
};

struct platform_device msm_bt_sco_c_earpiece_device = {
	.name = "msm_snddev_ecodec",
	.id = 12,
	.dev = { .platform_data = &snddev_bt_sco_c_earpiece_data },
};

struct platform_device msm_bt_sco_c_mic_device = {
	.name = "msm_snddev_ecodec",
	.id = 13,
	.dev = { .platform_data = &snddev_bt_sco_c_mic_data },
};

static struct snddev_ecodec_data snddev_bt_sco_d_earpiece_data = {
	.capability = (SNDDEV_CAP_RX | SNDDEV_CAP_VOICE),
	.name = "bt_sco_class_d_rx",
	.copp_id = 1,
	.acdb_id = ACDB_ID_BT_SCO_D_SPKR,
	.channel_mode = 1,
	.conf_pcm_ctl_val = BT_SCO_PCM_CTL_VAL,
	.conf_aux_codec_intf = BT_SCO_AUX_CODEC_INTF,
	.conf_data_format_padding_val = BT_SCO_DATA_FORMAT_PADDING,
	.max_voice_rx_vol[VOC_NB_INDEX] = 400,
	.min_voice_rx_vol[VOC_NB_INDEX] = -1100,
	.max_voice_rx_vol[VOC_WB_INDEX] = 400,
	.min_voice_rx_vol[VOC_WB_INDEX] = -1100,
#ifdef CONFIG_PANTECH_MINI_AUDIO_AUDIENCE1028  // jmlee
	.pamp_on = NULL,
	.pamp_off = NULL,
#endif	
};

static struct snddev_ecodec_data snddev_bt_sco_d_mic_data = {
	.capability = (SNDDEV_CAP_TX | SNDDEV_CAP_VOICE),
	.name = "bt_sco_class_d_tx",
	.copp_id = 1,
	.acdb_id = ACDB_ID_BT_SCO_D_MIC,
	.channel_mode = 1,
	.conf_pcm_ctl_val = BT_SCO_PCM_CTL_VAL,
	.conf_aux_codec_intf = BT_SCO_AUX_CODEC_INTF,
	.conf_data_format_padding_val = BT_SCO_DATA_FORMAT_PADDING,
#ifdef CONFIG_PANTECH_MINI_AUDIO_AUDIENCE1028 // jmlee
	.pamp_on = NULL,
	.pamp_off = NULL,
#endif		
};

struct platform_device msm_bt_sco_d_earpiece_device = {
	.name = "msm_snddev_ecodec",
	.id = 14,
	.dev = { .platform_data = &snddev_bt_sco_d_earpiece_data },
};

struct platform_device msm_bt_sco_d_mic_device = {
	.name = "msm_snddev_ecodec",
	.id = 15,
	.dev = { .platform_data = &snddev_bt_sco_d_mic_data },
};

static struct snddev_ecodec_data snddev_bt_sco_e_earpiece_data = {
	.capability = (SNDDEV_CAP_RX | SNDDEV_CAP_VOICE),
	.name = "bt_sco_class_e_rx",
	.copp_id = 1,
	.acdb_id = ACDB_ID_BT_SCO_E_SPKR,
	.channel_mode = 1,
	.conf_pcm_ctl_val = BT_SCO_PCM_CTL_VAL,
	.conf_aux_codec_intf = BT_SCO_AUX_CODEC_INTF,
	.conf_data_format_padding_val = BT_SCO_DATA_FORMAT_PADDING,
	.max_voice_rx_vol[VOC_NB_INDEX] = 400,
	.min_voice_rx_vol[VOC_NB_INDEX] = -1100,
	.max_voice_rx_vol[VOC_WB_INDEX] = 400,
	.min_voice_rx_vol[VOC_WB_INDEX] = -1100,
#ifdef CONFIG_PANTECH_MINI_AUDIO_AUDIENCE1028  // jmlee
	.pamp_on = NULL,
	.pamp_off = NULL,
#endif	
};

static struct snddev_ecodec_data snddev_bt_sco_e_mic_data = {
	.capability = (SNDDEV_CAP_TX | SNDDEV_CAP_VOICE),
	.name = "bt_sco_class_e_tx",
	.copp_id = 1,
	.acdb_id = ACDB_ID_BT_SCO_E_MIC,
	.channel_mode = 1,
	.conf_pcm_ctl_val = BT_SCO_PCM_CTL_VAL,
	.conf_aux_codec_intf = BT_SCO_AUX_CODEC_INTF,
	.conf_data_format_padding_val = BT_SCO_DATA_FORMAT_PADDING,
#ifdef CONFIG_PANTECH_MINI_AUDIO_AUDIENCE1028 // jmlee
	.pamp_on = NULL,
	.pamp_off = NULL,
#endif		
};

struct platform_device msm_bt_sco_e_earpiece_device = {
	.name = "msm_snddev_ecodec",
	.id = 16,
	.dev = { .platform_data = &snddev_bt_sco_e_earpiece_data },
};

struct platform_device msm_bt_sco_e_mic_device = {
	.name = "msm_snddev_ecodec",
	.id = 17,
	.dev = { .platform_data = &snddev_bt_sco_e_mic_data },
};

static struct snddev_ecodec_data snddev_bt_sco_f_earpiece_data = {
	.capability = (SNDDEV_CAP_RX | SNDDEV_CAP_VOICE),
	.name = "bt_sco_class_f_rx",
	.copp_id = 1,
	.acdb_id = ACDB_ID_BT_SCO_F_SPKR,
	.channel_mode = 1,
	.conf_pcm_ctl_val = BT_SCO_PCM_CTL_VAL,
	.conf_aux_codec_intf = BT_SCO_AUX_CODEC_INTF,
	.conf_data_format_padding_val = BT_SCO_DATA_FORMAT_PADDING,
	.max_voice_rx_vol[VOC_NB_INDEX] = 400,
	.min_voice_rx_vol[VOC_NB_INDEX] = -1100,
	.max_voice_rx_vol[VOC_WB_INDEX] = 400,
	.min_voice_rx_vol[VOC_WB_INDEX] = -1100,
#ifdef CONFIG_PANTECH_MINI_AUDIO_AUDIENCE1028  // jmlee
	.pamp_on = NULL,
	.pamp_off = NULL,
#endif	
};

static struct snddev_ecodec_data snddev_bt_sco_f_mic_data = {
	.capability = (SNDDEV_CAP_TX | SNDDEV_CAP_VOICE),
	.name = "bt_sco_class_f_tx",
	.copp_id = 1,
	.acdb_id = ACDB_ID_BT_SCO_F_MIC,
	.channel_mode = 1,
	.conf_pcm_ctl_val = BT_SCO_PCM_CTL_VAL,
	.conf_aux_codec_intf = BT_SCO_AUX_CODEC_INTF,
	.conf_data_format_padding_val = BT_SCO_DATA_FORMAT_PADDING,
#ifdef CONFIG_PANTECH_MINI_AUDIO_AUDIENCE1028 // jmlee
	.pamp_on = NULL,
	.pamp_off = NULL,
#endif		
};

struct platform_device msm_bt_sco_f_earpiece_device = {
	.name = "msm_snddev_ecodec",
	.id = 18,
	.dev = { .platform_data = &snddev_bt_sco_f_earpiece_data },
};

struct platform_device msm_bt_sco_f_mic_device = {
	.name = "msm_snddev_ecodec",
	.id = 19,
	.dev = { .platform_data = &snddev_bt_sco_f_mic_data },
};

static struct snddev_ecodec_data snddev_bt_sco_g_earpiece_data = {
	.capability = (SNDDEV_CAP_RX | SNDDEV_CAP_VOICE),
	.name = "bt_sco_class_g_rx",
	.copp_id = 1,
	.acdb_id = ACDB_ID_BT_SCO_G_SPKR,
	.channel_mode = 1,
	.conf_pcm_ctl_val = BT_SCO_PCM_CTL_VAL,
	.conf_aux_codec_intf = BT_SCO_AUX_CODEC_INTF,
	.conf_data_format_padding_val = BT_SCO_DATA_FORMAT_PADDING,
	.max_voice_rx_vol[VOC_NB_INDEX] = 400,
	.min_voice_rx_vol[VOC_NB_INDEX] = -1100,
	.max_voice_rx_vol[VOC_WB_INDEX] = 400,
	.min_voice_rx_vol[VOC_WB_INDEX] = -1100,
#ifdef CONFIG_PANTECH_MINI_AUDIO_AUDIENCE1028  // jmlee
	.pamp_on = NULL,
	.pamp_off = NULL,
#endif	
};

static struct snddev_ecodec_data snddev_bt_sco_g_mic_data = {
	.capability = (SNDDEV_CAP_TX | SNDDEV_CAP_VOICE),
	.name = "bt_sco_class_g_tx",
	.copp_id = 1,
	.acdb_id = ACDB_ID_BT_SCO_G_MIC,
	.channel_mode = 1,
	.conf_pcm_ctl_val = BT_SCO_PCM_CTL_VAL,
	.conf_aux_codec_intf = BT_SCO_AUX_CODEC_INTF,
	.conf_data_format_padding_val = BT_SCO_DATA_FORMAT_PADDING,
#ifdef CONFIG_PANTECH_MINI_AUDIO_AUDIENCE1028 // jmlee
	.pamp_on = NULL,
	.pamp_off = NULL,
#endif		
};

struct platform_device msm_bt_sco_g_earpiece_device = {
	.name = "msm_snddev_ecodec",
	.id = 20,
	.dev = { .platform_data = &snddev_bt_sco_g_earpiece_data },
};

struct platform_device msm_bt_sco_g_mic_device = {
	.name = "msm_snddev_ecodec",
	.id = 21,
	.dev = { .platform_data = &snddev_bt_sco_g_mic_data },
};

static struct snddev_ecodec_data snddev_bt_sco_h_earpiece_data = {
	.capability = (SNDDEV_CAP_RX | SNDDEV_CAP_VOICE),
	.name = "bt_sco_class_h_rx",
	.copp_id = 1,
	.acdb_id = ACDB_ID_BT_SCO_H_SPKR,
	.channel_mode = 1,
	.conf_pcm_ctl_val = BT_SCO_PCM_CTL_VAL,
	.conf_aux_codec_intf = BT_SCO_AUX_CODEC_INTF,
	.conf_data_format_padding_val = BT_SCO_DATA_FORMAT_PADDING,
	.max_voice_rx_vol[VOC_NB_INDEX] = 400,
	.min_voice_rx_vol[VOC_NB_INDEX] = -1100,
	.max_voice_rx_vol[VOC_WB_INDEX] = 400,
	.min_voice_rx_vol[VOC_WB_INDEX] = -1100,
#ifdef CONFIG_PANTECH_MINI_AUDIO_AUDIENCE1028  // jmlee
	.pamp_on = NULL,
	.pamp_off = NULL,
#endif	
};

static struct snddev_ecodec_data snddev_bt_sco_h_mic_data = {
	.capability = (SNDDEV_CAP_TX | SNDDEV_CAP_VOICE),
	.name = "bt_sco_class_h_tx",
	.copp_id = 1,
	.acdb_id = ACDB_ID_BT_SCO_H_MIC,
	.channel_mode = 1,
	.conf_pcm_ctl_val = BT_SCO_PCM_CTL_VAL,
	.conf_aux_codec_intf = BT_SCO_AUX_CODEC_INTF,
	.conf_data_format_padding_val = BT_SCO_DATA_FORMAT_PADDING,
#ifdef CONFIG_PANTECH_MINI_AUDIO_AUDIENCE1028 // jmlee
	.pamp_on = NULL,
	.pamp_off = NULL,
#endif		
};

struct platform_device msm_bt_sco_h_earpiece_device = {
	.name = "msm_snddev_ecodec",
	.id = 22,
	.dev = { .platform_data = &snddev_bt_sco_h_earpiece_data },
};

struct platform_device msm_bt_sco_h_mic_device = {
	.name = "msm_snddev_ecodec",
	.id = 23,
	.dev = { .platform_data = &snddev_bt_sco_h_mic_data },
};

static struct snddev_ecodec_data snddev_bt_sco_i_earpiece_data = {
	.capability = (SNDDEV_CAP_RX | SNDDEV_CAP_VOICE),
	.name = "bt_sco_class_i_rx",
	.copp_id = 1,
	.acdb_id = ACDB_ID_BT_SCO_I_SPKR,
	.channel_mode = 1,
	.conf_pcm_ctl_val = BT_SCO_PCM_CTL_VAL,
	.conf_aux_codec_intf = BT_SCO_AUX_CODEC_INTF,
	.conf_data_format_padding_val = BT_SCO_DATA_FORMAT_PADDING,
	.max_voice_rx_vol[VOC_NB_INDEX] = 400,
	.min_voice_rx_vol[VOC_NB_INDEX] = -1100,
	.max_voice_rx_vol[VOC_WB_INDEX] = 400,
	.min_voice_rx_vol[VOC_WB_INDEX] = -1100,
#ifdef CONFIG_PANTECH_MINI_AUDIO_AUDIENCE1028  // jmlee
	.pamp_on = NULL,
	.pamp_off = NULL,
#endif	
};

static struct snddev_ecodec_data snddev_bt_sco_i_mic_data = {
	.capability = (SNDDEV_CAP_TX | SNDDEV_CAP_VOICE),
	.name = "bt_sco_class_i_tx",
	.copp_id = 1,
	.acdb_id = ACDB_ID_BT_SCO_I_MIC,
	.channel_mode = 1,
	.conf_pcm_ctl_val = BT_SCO_PCM_CTL_VAL,
	.conf_aux_codec_intf = BT_SCO_AUX_CODEC_INTF,
	.conf_data_format_padding_val = BT_SCO_DATA_FORMAT_PADDING,
#ifdef CONFIG_PANTECH_MINI_AUDIO_AUDIENCE1028 // jmlee
	.pamp_on = NULL,
	.pamp_off = NULL,
#endif		
};

struct platform_device msm_bt_sco_i_earpiece_device = {
	.name = "msm_snddev_ecodec",
	.id = 25,
	.dev = { .platform_data = &snddev_bt_sco_i_earpiece_data },
};

struct platform_device msm_bt_sco_i_mic_device = {
	.name = "msm_snddev_ecodec",
	.id = 26,
	.dev = { .platform_data = &snddev_bt_sco_i_mic_data },
};
#endif

#ifdef CONFIG_PANTECH_MINI_AUDIO_AUDIENCE1028 // jmlee
// /sys/bus/platform/drivers/msm_snddev_ecodec  check if  exist msm_snddev_ecodec.[number] 

struct platform_device msm_nr_ct_rx_device = {
	.name = "msm_snddev_ecodec",
	.id = 2,
	.dev = { .platform_data = &snddev_nr_ct_rx_data },
};

struct platform_device msm_nr_ct_tx_device = {
	.name = "msm_snddev_ecodec",
	.id = 3,
	.dev = { .platform_data = &snddev_nr_ct_tx_data },
};

struct platform_device msm_nr_ft_rx_device = {
	.name = "msm_snddev_ecodec",
	.id = 4,
	.dev = { .platform_data = &snddev_nr_ft_rx_data },
};

struct platform_device msm_nr_ft_tx_device = {
	.name = "msm_snddev_ecodec",
	.id = 5,
	.dev = { .platform_data = &snddev_nr_ft_tx_data },
};

struct platform_device msm_nr_dv_rx_device = {
	.name = "msm_snddev_ecodec",
	.id = 6,
	.dev = { .platform_data = &snddev_nr_dv_rx_data },
};

struct platform_device msm_nr_dv_tx_device = {
	.name = "msm_snddev_ecodec",
	.id = 7,
	.dev = { .platform_data = &snddev_nr_dv_tx_data },
};

static struct snddev_ecodec_data snddev_nr_ct_2mic_tx_data = {
	.capability = (SNDDEV_CAP_TX | SNDDEV_CAP_VOICE),
	.name = "nr_ct_2mic_tx",
	.copp_id = 1,
	.acdb_id = ACDB_ID_NR_CT_2MIC_TX, //
	.channel_mode = 1,
	.conf_pcm_ctl_val = BT_SCO_PCM_CTL_VAL,
	.conf_aux_codec_intf = BT_SCO_AUX_CODEC_INTF,
	.conf_data_format_padding_val = BT_SCO_DATA_FORMAT_PADDING,
#ifdef CONFIG_PANTECH_MINI_AUDIO_AUDIENCE1028 // jmlee
	.pmctl_id = i_nr_tx_pmctl_id,  //20110224 jhsong
	.pmctl_id_sz = ARRAY_SIZE(i_nr_tx_pmctl_id),  //20110224 jhsong

	.pamp_on = &audience_a1028_ct_2mic_on,  //20110301 jhsong : abnormal work after set path.....cause by pointer address so add "&" before func
	.pamp_off = &audience_a1028_off, //20110301 jhsong : abnormal work after set path.....cause by pointer address so add "&" before func
#endif		
};

struct platform_device msm_nr_ct_2mic_tx_device = {
	.name = "msm_snddev_ecodec",
	.id = 24,
	.dev = { .platform_data = &snddev_nr_ct_2mic_tx_data },
};

#if 0 // jmlee Audience Route NR_OFF
struct platform_device msm_nr_off_ct_rx_device = {
	.name = "msm_snddev_ecodec",
	.id = 8,
	.dev = { .platform_data = &snddev_nr_off_ct_rx_data },
};

struct platform_device msm_nr_off_ct_tx_device = {
	.name = "msm_snddev_ecodec",
	.id = 9,
	.dev = { .platform_data = &snddev_nr_off_ct_tx_data },
};

struct platform_device msm_nr_off_ft_rx_device = {
	.name = "msm_snddev_ecodec",
	.id = 10,
	.dev = { .platform_data = &snddev_nr_off_ft_rx_data },
};

struct platform_device msm_nr_off_ft_tx_device = {
	.name = "msm_snddev_ecodec",
	.id = 11,
	.dev = { .platform_data = &snddev_nr_off_ft_tx_data },
};

struct platform_device msm_nr_off_dv_rx_device = {
	.name = "msm_snddev_ecodec",
	.id = 12,
	.dev = { .platform_data = &snddev_nr_off_dv_rx_data },
};

struct platform_device msm_nr_off_dv_tx_device = {
	.name = "msm_snddev_ecodec",
	.id = 13,
	.dev = { .platform_data = &snddev_nr_off_dv_tx_data },
};
#endif
#endif

static struct adie_codec_action_unit idual_mic_endfire_8KHz_osr256_actions[] =
	MIC1_LEFT_LINE_IN_RIGHT_8000_OSR_256;

static struct adie_codec_hwsetting_entry idual_mic_endfire_settings[] = {
	{
		.freq_plan = 8000,
		.osr = 256,
		.actions = idual_mic_endfire_8KHz_osr256_actions,
		.action_sz = ARRAY_SIZE(idual_mic_endfire_8KHz_osr256_actions),
	}, /* 8KHz profile can be used for 16KHz */
	{
		.freq_plan = 16000,
		.osr = 256,
		.actions = idual_mic_endfire_8KHz_osr256_actions,
		.action_sz = ARRAY_SIZE(idual_mic_endfire_8KHz_osr256_actions),
	}, /* 8KHz profile can be used for 48KHz */
	{
		.freq_plan = 48000,
		.osr = 256,
		.actions = idual_mic_endfire_8KHz_osr256_actions,
		.action_sz = ARRAY_SIZE(idual_mic_endfire_8KHz_osr256_actions),
	}
};

static struct adie_codec_dev_profile idual_mic_endfire_profile = {
	.path_type = ADIE_CODEC_TX,
	.settings = idual_mic_endfire_settings,
	.setting_sz = ARRAY_SIZE(idual_mic_endfire_settings),
};

static enum hsed_controller idual_mic_endfire_pmctl_id[] = {
	PM_HSED_CONTROLLER_0, PM_HSED_CONTROLLER_2
};

static struct snddev_icodec_data snddev_idual_mic_endfire_data = {
	.capability = (SNDDEV_CAP_TX | SNDDEV_CAP_VOICE),
	.name = "handset_dual_mic_endfire_tx",
	.copp_id = 0,
	.acdb_id = ACDB_ID_HANDSET_MIC_ENDFIRE,
	.profile = &idual_mic_endfire_profile,
	.channel_mode = 2,
	.default_sample_rate = 48000,
	.pmctl_id = idual_mic_endfire_pmctl_id,
	.pmctl_id_sz = ARRAY_SIZE(idual_mic_endfire_pmctl_id),
	.pamp_on = NULL,
	.pamp_off = NULL,
};

static struct platform_device msm_idual_mic_endfire_device = {
	.name = "snddev_icodec",
	.id = 12,
	.dev = { .platform_data = &snddev_idual_mic_endfire_data },
};


static struct snddev_icodec_data\
		snddev_idual_mic_endfire_real_stereo_data = {
	.capability = (SNDDEV_CAP_TX | SNDDEV_CAP_VOICE),
	.name = "handset_dual_mic_endfire_tx_real_stereo",
	.copp_id = 0,
	.acdb_id = PSEUDO_ACDB_ID,
	.profile = &idual_mic_endfire_profile,
	.channel_mode = REAL_STEREO_CHANNEL_MODE,
	.default_sample_rate = 48000,
	.pmctl_id = idual_mic_endfire_pmctl_id,
	.pmctl_id_sz = ARRAY_SIZE(idual_mic_endfire_pmctl_id),
	.pamp_on = NULL,
	.pamp_off = NULL,
};

static struct platform_device msm_real_stereo_tx_device = {
	.name = "snddev_icodec",
	.id = 26,
	.dev = { .platform_data =
			&snddev_idual_mic_endfire_real_stereo_data },
};

static struct adie_codec_action_unit idual_mic_bs_8KHz_osr256_actions[] =
	MIC1_LEFT_AUX_IN_RIGHT_8000_OSR_256;

static struct adie_codec_hwsetting_entry idual_mic_broadside_settings[] = {
	{
		.freq_plan = 8000,
		.osr = 256,
		.actions = idual_mic_bs_8KHz_osr256_actions,
		.action_sz = ARRAY_SIZE(idual_mic_bs_8KHz_osr256_actions),
	}, /* 8KHz profile can be used for 16KHz */
	{
		.freq_plan = 16000,
		.osr = 256,
		.actions = idual_mic_bs_8KHz_osr256_actions,
		.action_sz = ARRAY_SIZE(idual_mic_bs_8KHz_osr256_actions),
	}, /* 8KHz profile can be used for 16KHz */
	{
		.freq_plan = 48000,
		.osr = 256,
		.actions = idual_mic_bs_8KHz_osr256_actions,
		.action_sz = ARRAY_SIZE(idual_mic_bs_8KHz_osr256_actions),
	}
};

static struct adie_codec_dev_profile idual_mic_broadside_profile = {
	.path_type = ADIE_CODEC_TX,
	.settings = idual_mic_broadside_settings,
	.setting_sz = ARRAY_SIZE(idual_mic_broadside_settings),
};

static enum hsed_controller idual_mic_broadside_pmctl_id[] = {
	PM_HSED_CONTROLLER_0, PM_HSED_CONTROLLER_2
};

static struct snddev_icodec_data snddev_idual_mic_broadside_data = {
	.capability = (SNDDEV_CAP_TX | SNDDEV_CAP_VOICE),
	.name = "handset_dual_mic_broadside_tx",
	.copp_id = 0,
	.acdb_id = ACDB_ID_HANDSET_MIC_BROADSIDE,
	.profile = &idual_mic_broadside_profile,
	.channel_mode = 2,
	.default_sample_rate = 48000,
	.pmctl_id = idual_mic_broadside_pmctl_id,
	.pmctl_id_sz = ARRAY_SIZE(idual_mic_broadside_pmctl_id),
	.pamp_on = NULL,
	.pamp_off = NULL,
};

static struct platform_device msm_idual_mic_broadside_device = {
	.name = "snddev_icodec",
	.id = 13,
	.dev = { .platform_data = &snddev_idual_mic_broadside_data },
};

static struct adie_codec_action_unit ispk_dual_mic_ef_8KHz_osr256_actions[] =
	SPEAKER_MIC1_LEFT_LINE_IN_RIGHT_8000_OSR_256;

static struct adie_codec_hwsetting_entry ispk_dual_mic_ef_settings[] = {
	{
		.freq_plan = 8000,
		.osr = 256,
		.actions = ispk_dual_mic_ef_8KHz_osr256_actions,
		.action_sz = ARRAY_SIZE(ispk_dual_mic_ef_8KHz_osr256_actions),
	}, /* 8KHz profile can be used for 16Khz */
	{
		.freq_plan = 16000,
		.osr = 256,
		.actions = ispk_dual_mic_ef_8KHz_osr256_actions,
		.action_sz = ARRAY_SIZE(ispk_dual_mic_ef_8KHz_osr256_actions),
	}, /* 8KHz profile can be used for 48KHz */
	{
		.freq_plan = 48000,
		.osr = 256,
		.actions = ispk_dual_mic_ef_8KHz_osr256_actions,
		.action_sz = ARRAY_SIZE(ispk_dual_mic_ef_8KHz_osr256_actions),
	},
};

static struct adie_codec_dev_profile ispk_dual_mic_ef_profile = {
	.path_type = ADIE_CODEC_TX,
	.settings = ispk_dual_mic_ef_settings,
	.setting_sz = ARRAY_SIZE(ispk_dual_mic_ef_settings),
};

static struct snddev_icodec_data snddev_spk_idual_mic_endfire_data = {
	.capability = (SNDDEV_CAP_TX | SNDDEV_CAP_VOICE),
	.name = "speaker_dual_mic_endfire_tx",
	.copp_id = 0,
	.acdb_id = ACDB_ID_SPKR_PHONE_MIC_ENDFIRE,
	.profile = &ispk_dual_mic_ef_profile,
	.channel_mode = 2,
	.default_sample_rate = 48000,
	.pmctl_id = idual_mic_endfire_pmctl_id,
	.pmctl_id_sz = ARRAY_SIZE(idual_mic_endfire_pmctl_id),
	.pamp_on = NULL,
	.pamp_off = NULL,
};

static struct platform_device msm_spk_idual_mic_endfire_device = {
	.name = "snddev_icodec",
	.id = 14,
	.dev = { .platform_data = &snddev_spk_idual_mic_endfire_data },
};

static struct adie_codec_action_unit ispk_dual_mic_bs_8KHz_osr256_actions[] =
	SPEAKER_MIC1_LEFT_AUX_IN_RIGHT_8000_OSR_256;

static struct adie_codec_hwsetting_entry ispk_dual_mic_bs_settings[] = {
	{
		.freq_plan = 8000,
		.osr = 256,
		.actions = ispk_dual_mic_bs_8KHz_osr256_actions,
		.action_sz = ARRAY_SIZE(ispk_dual_mic_bs_8KHz_osr256_actions),
	}, /* 8KHz profile can be used for 16Khz */
	{
		.freq_plan = 16000,
		.osr = 256,
		.actions = ispk_dual_mic_bs_8KHz_osr256_actions,
		.action_sz = ARRAY_SIZE(ispk_dual_mic_bs_8KHz_osr256_actions),
	}, /* 8KHz profile can be used for 48KHz */
	{
		.freq_plan = 48000,
		.osr = 256,
		.actions = ispk_dual_mic_bs_8KHz_osr256_actions,
		.action_sz = ARRAY_SIZE(ispk_dual_mic_bs_8KHz_osr256_actions),
	},
};

static struct adie_codec_dev_profile ispk_dual_mic_bs_profile = {
	.path_type = ADIE_CODEC_TX,
	.settings = ispk_dual_mic_bs_settings,
	.setting_sz = ARRAY_SIZE(ispk_dual_mic_bs_settings),
};
static struct snddev_icodec_data snddev_spk_idual_mic_broadside_data = {
	.capability = (SNDDEV_CAP_TX | SNDDEV_CAP_VOICE),
	.name = "speaker_dual_mic_broadside_tx",
	.copp_id = 0,
	.acdb_id = ACDB_ID_SPKR_PHONE_MIC_BROADSIDE,
	.profile = &ispk_dual_mic_bs_profile,
	.channel_mode = 2,
	.default_sample_rate = 48000,
	.pmctl_id = idual_mic_broadside_pmctl_id,
	.pmctl_id_sz = ARRAY_SIZE(idual_mic_broadside_pmctl_id),
	.pamp_on = NULL,
	.pamp_off = NULL,
};

static struct platform_device msm_spk_idual_mic_broadside_device = {
	.name = "snddev_icodec",
	.id = 15,
	.dev = { .platform_data = &snddev_spk_idual_mic_broadside_data },
};

static struct adie_codec_action_unit itty_hs_mono_tx_8KHz_osr256_actions[] =
	TTY_HEADSET_MONO_TX_8000_OSR_256;

static struct adie_codec_hwsetting_entry itty_hs_mono_tx_settings[] = {
	/* 8KHz, 16KHz, 48KHz TTY Tx devices can shared same set of actions */
	{
		.freq_plan = 8000,
		.osr = 256,
		.actions = itty_hs_mono_tx_8KHz_osr256_actions,
		.action_sz = ARRAY_SIZE(itty_hs_mono_tx_8KHz_osr256_actions),
	},
	{
		.freq_plan = 16000,
		.osr = 256,
		.actions = itty_hs_mono_tx_8KHz_osr256_actions,
		.action_sz = ARRAY_SIZE(itty_hs_mono_tx_8KHz_osr256_actions),
	},
	{
		.freq_plan = 48000,
		.osr = 256,
		.actions = itty_hs_mono_tx_8KHz_osr256_actions,
		.action_sz = ARRAY_SIZE(itty_hs_mono_tx_8KHz_osr256_actions),
	}
};

static struct adie_codec_dev_profile itty_hs_mono_tx_profile = {
	.path_type = ADIE_CODEC_TX,
	.settings = itty_hs_mono_tx_settings,
	.setting_sz = ARRAY_SIZE(itty_hs_mono_tx_settings),
};

static struct snddev_icodec_data snddev_itty_hs_mono_tx_data = {
	.capability = (SNDDEV_CAP_TX | SNDDEV_CAP_VOICE | SNDDEV_CAP_TTY),
	.name = "tty_headset_mono_tx",
	.copp_id = 0,
	.acdb_id = ACDB_ID_TTY_HEADSET_MIC,
	.profile = &itty_hs_mono_tx_profile,
	.channel_mode = 1,
	.default_sample_rate = 48000,
#ifdef CONFIG_PANTECH_AUDIO_FAB2200    //20110428 jhsong : feature clearup
//	.pmctl_id = i_hs_mono_tx_pmctl_id,
//	.pmctl_id_sz = ARRAY_SIZE(i_hs_mono_tx_pmctl_id),
#else
	.pmctl_id = NULL,
	.pmctl_id_sz = 0,
#endif	
	.pamp_on = NULL,
	.pamp_off = NULL,
};

static struct platform_device msm_itty_hs_mono_tx_device = {
	.name = "snddev_icodec",
	.id = 16,
	.dev = { .platform_data = &snddev_itty_hs_mono_tx_data },
};

static struct adie_codec_action_unit itty_hs_mono_rx_8KHz_osr256_actions[] =
	TTY_HEADSET_MONO_RX_CLASS_D_8000_OSR_256;

static struct adie_codec_action_unit itty_hs_mono_rx_16KHz_osr256_actions[] =
	TTY_HEADSET_MONO_RX_CLASS_D_16000_OSR_256;

static struct adie_codec_action_unit itty_hs_mono_rx_48KHz_osr256_actions[] =
	TTY_HEADSET_MONO_RX_CLASS_D_48000_OSR_256;

static struct adie_codec_hwsetting_entry itty_hs_mono_rx_settings[] = {
	{
		.freq_plan = 8000,
		.osr = 256,
		.actions = itty_hs_mono_rx_8KHz_osr256_actions,
		.action_sz = ARRAY_SIZE(itty_hs_mono_rx_8KHz_osr256_actions),
	},
	{
		.freq_plan = 16000,
		.osr = 256,
		.actions = itty_hs_mono_rx_16KHz_osr256_actions,
		.action_sz = ARRAY_SIZE(itty_hs_mono_rx_16KHz_osr256_actions),
	},
	{
		.freq_plan = 48000,
		.osr = 256,
		.actions = itty_hs_mono_rx_48KHz_osr256_actions,
		.action_sz = ARRAY_SIZE(itty_hs_mono_rx_48KHz_osr256_actions),
	}
};

static struct adie_codec_dev_profile itty_hs_mono_rx_profile = {
	.path_type = ADIE_CODEC_RX,
	.settings = itty_hs_mono_rx_settings,
	.setting_sz = ARRAY_SIZE(itty_hs_mono_rx_settings),
};

static struct snddev_icodec_data snddev_itty_hs_mono_rx_data = {
	.capability = (SNDDEV_CAP_RX | SNDDEV_CAP_VOICE | SNDDEV_CAP_TTY),
	.name = "tty_headset_mono_rx",
	.copp_id = 0,
	.acdb_id = ACDB_ID_TTY_HEADSET_SPKR,
	.profile = &itty_hs_mono_rx_profile,
	.channel_mode = 1,
	.default_sample_rate = 48000,
#ifdef CONFIG_PANTECH_AUDIO_FAB2200    //20110428 jhsong : feature clearup
	.pamp_on = &snd_subsystem_tty_hp_poweron,
	.pamp_off = &snd_subsystem_tty_hp_powerdown,
#else
	.pamp_on = NULL,
	.pamp_off = NULL,
#endif	
	.max_voice_rx_vol[VOC_NB_INDEX] = 0,
	.min_voice_rx_vol[VOC_NB_INDEX] = 0,
	.max_voice_rx_vol[VOC_WB_INDEX] = 0,
	.min_voice_rx_vol[VOC_WB_INDEX] = 0,
};

static struct platform_device msm_itty_hs_mono_rx_device = {
	.name = "snddev_icodec",
	.id = 17,
	.dev = { .platform_data = &snddev_itty_hs_mono_rx_data },
};

static struct adie_codec_action_unit ispeaker_tx_8KHz_osr256_actions[] =
	SPEAKER_TX_8000_OSR_256;

static struct adie_codec_action_unit ispeaker_tx_48KHz_osr256_actions[] =
	SPEAKER_TX_48000_OSR_256;

static struct adie_codec_hwsetting_entry ispeaker_tx_settings[] = {
	{
		.freq_plan = 8000,
		.osr = 256,
		.actions = ispeaker_tx_8KHz_osr256_actions,
		.action_sz = ARRAY_SIZE(ispeaker_tx_8KHz_osr256_actions),
	},
	{ /* 8KHz profile is good for 16KHz */
		.freq_plan = 16000,
		.osr = 256,
		.actions = ispeaker_tx_8KHz_osr256_actions,
		.action_sz = ARRAY_SIZE(ispeaker_tx_8KHz_osr256_actions),
	},
	{
		.freq_plan = 48000,
		.osr = 256,
		.actions = ispeaker_tx_48KHz_osr256_actions,
		.action_sz = ARRAY_SIZE(ispeaker_tx_48KHz_osr256_actions),
	}
};

static struct adie_codec_dev_profile ispeaker_tx_profile = {
	.path_type = ADIE_CODEC_TX,
	.settings = ispeaker_tx_settings,
	.setting_sz = ARRAY_SIZE(ispeaker_tx_settings),
};

static enum hsed_controller ispk_pmctl_id[] = {PM_HSED_CONTROLLER_0};

static struct snddev_icodec_data snddev_ispeaker_tx_data = {
	.capability = (SNDDEV_CAP_TX | SNDDEV_CAP_VOICE),
	.name = "speaker_mono_tx",
	.copp_id = 0,
	.acdb_id = ACDB_ID_SPKR_PHONE_MIC,
	.profile = &ispeaker_tx_profile,
	.channel_mode = 1,
	.pmctl_id = ispk_pmctl_id,
	.pmctl_id_sz = ARRAY_SIZE(ispk_pmctl_id),
	.default_sample_rate = 48000,
#ifdef CONFIG_PANTECH_MINI_AUDIO_AUDIENCE1028  //20110126 jmlee : Audiance AMP  
	.pamp_on = &msm_snddev_mic_sel_QTR,  //20110301 jhsong : abnormal work after set path.....cause by pointer address so add "&" before func
	.pamp_off = NULL,
#else
	.pamp_on = msm_snddev_tx_route_config,
	.pamp_off = msm_snddev_tx_route_deconfig,
#endif	
};

static struct platform_device msm_ispeaker_tx_device = {
	.name = "snddev_icodec",
	.id = 18,
	.dev = { .platform_data = &snddev_ispeaker_tx_data },
};

static struct adie_codec_action_unit iearpiece_ffa_48KHz_osr256_actions[] =
	HANDSET_RX_48000_OSR_256_FFA;

static struct adie_codec_hwsetting_entry iearpiece_ffa_settings[] = {
	{
		.freq_plan = 48000,
		.osr = 256,
		.actions = iearpiece_ffa_48KHz_osr256_actions,
		.action_sz = ARRAY_SIZE(iearpiece_ffa_48KHz_osr256_actions),
	}
};

static struct adie_codec_dev_profile iearpiece_ffa_profile = {
	.path_type = ADIE_CODEC_RX,
	.settings = iearpiece_ffa_settings,
	.setting_sz = ARRAY_SIZE(iearpiece_ffa_settings),
};

static struct snddev_icodec_data snddev_iearpiece_ffa_data = {
	.capability = (SNDDEV_CAP_RX | SNDDEV_CAP_VOICE),
	.name = "handset_rx",
	.copp_id = 0,
	.acdb_id = ACDB_ID_HANDSET_SPKR,
	.profile = &iearpiece_ffa_profile,
	.channel_mode = 1,
	.pmctl_id = NULL,
	.pmctl_id_sz = 0,
	.default_sample_rate = 48000,
	.pamp_on = NULL,
	.pamp_off = NULL,
	.max_voice_rx_vol[VOC_NB_INDEX] = -700,
	.min_voice_rx_vol[VOC_NB_INDEX] = -2200,
	.max_voice_rx_vol[VOC_WB_INDEX] = -1400,
	.min_voice_rx_vol[VOC_WB_INDEX] = -2900,
};

static struct platform_device msm_iearpiece_ffa_device = {
	.name = "snddev_icodec",
	.id = 19,
	.dev = { .platform_data = &snddev_iearpiece_ffa_data },
};

static struct adie_codec_action_unit imic_ffa_8KHz_osr256_actions[] =
	HANDSET_TX_8000_OSR_256_FFA;

static struct adie_codec_action_unit imic_ffa_16KHz_osr256_actions[] =
	HANDSET_TX_16000_OSR_256_FFA;

static struct adie_codec_action_unit imic_ffa_48KHz_osr256_actions[] =
	HANDSET_TX_48000_OSR_256_FFA;

static struct adie_codec_hwsetting_entry imic_ffa_settings[] = {
	{
		.freq_plan = 8000,
		.osr = 256,
		.actions = imic_ffa_8KHz_osr256_actions,
		.action_sz = ARRAY_SIZE(imic_ffa_8KHz_osr256_actions),
	},
	{
		.freq_plan = 16000,
		.osr = 256,
		.actions = imic_ffa_16KHz_osr256_actions,
		.action_sz = ARRAY_SIZE(imic_ffa_16KHz_osr256_actions),
	},
	{
		.freq_plan = 48000,
		.osr = 256,
		.actions = imic_ffa_48KHz_osr256_actions,
		.action_sz = ARRAY_SIZE(imic_ffa_48KHz_osr256_actions),
	}
};

static struct adie_codec_dev_profile imic_ffa_profile = {
	.path_type = ADIE_CODEC_TX,
	.settings = imic_ffa_settings,
	.setting_sz = ARRAY_SIZE(imic_ffa_settings),
};

static struct snddev_icodec_data snddev_imic_ffa_data = {
	.capability = (SNDDEV_CAP_TX | SNDDEV_CAP_VOICE),
	.name = "handset_tx",
	.copp_id = 0,
	.acdb_id = ACDB_ID_HANDSET_MIC,
	.profile = &imic_ffa_profile,
	.channel_mode = 1,
	.pmctl_id = imic_pmctl_id,
	.pmctl_id_sz = ARRAY_SIZE(imic_pmctl_id),
	.default_sample_rate = 48000,
	.pamp_on = NULL,
	.pamp_off = NULL,
};

static struct platform_device msm_imic_ffa_device = {
	.name = "snddev_icodec",
	.id = 20,
	.dev = { .platform_data = &snddev_imic_ffa_data },
};

#if 0 //def CONFIG_PANTECH_MINI_AUDIO_FAB2200  //20110126 jhsong : FAB2200 AMP
#define _HEADSET_STEREO_SPEAKER_STEREO_RX_CAPLESS_48000_OSR_256 \
	{{ADIE_CODEC_ACTION_STAGE_REACHED, ADIE_CODEC_FLASH_IMAGE}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3B, 0xFF, 0xac)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3C, 0xFF, 0xac)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x80, 0x02, 0x02)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x80, 0x02, 0x00)}, \
	{ADIE_CODEC_ACTION_STAGE_REACHED, ADIE_CODEC_DIGITAL_READY}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x81, 0xFF, 0x4E)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x04, 0xff, 0xBC)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x24, 0x6F, 0x64)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x25, 0x0F, 0x0b)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x26, 0xfc, 0xfc)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x38, 0xff, 0x80)/*(0x38, 0xff, 0x82)*/}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x37, 0xe4, 0x82)/*(0x37, 0xe6, 0xa0)*/}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3A, 0xFF, 0x2b)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x83, 0x23, 0x23)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x33, 0x8A, 0x8A)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x34, 0xf0, 0xa0)}, \
	{ADIE_CODEC_ACTION_DELAY_WAIT, 0x7530}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x34, 0xf0, 0xf0)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x23, 0xff, 0x20)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x33, 0x05, 0x85)/*(0x33, 0x05, 0x05)*/}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3B, 0xFF, 0x98)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3C, 0xFF, 0x98)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3B, 0xFF, 0x88)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3C, 0xFF, 0x88)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3B, 0xFF, 0x78)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3C, 0xFF, 0x78)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3B, 0xFF, 0x68)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3C, 0xFF, 0x68)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3B, 0xFF, 0x58)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3C, 0xFF, 0x58)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3B, 0xFF, 0x48)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3C, 0xFF, 0x48)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3B, 0xFF, 0x38)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3C, 0xFF, 0x38)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3B, 0xFF, 0x28)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3C, 0xFF, 0x28)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3B, 0xFF, 0x18)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3C, 0xFF, 0x18)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3B, 0xFF, 0x10)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3C, 0xFF, 0x10)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x84, 0xff, 0x00)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x85, 0xff, 0x00)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x8A, 0x0f, 0x0c)}, \
	{ADIE_CODEC_ACTION_STAGE_REACHED, ADIE_CODEC_DIGITAL_ANALOG_READY}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x8a, 0x03, 0x03)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3b, 0xFF, 0xaC)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3C, 0xFF, 0xaC)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x34, 0xf0, 0x00)}, \
	{ADIE_CODEC_ACTION_STAGE_REACHED, ADIE_CODEC_ANALOG_OFF}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x33, 0x80, 0x00)}, \
	{ADIE_CODEC_ACTION_STAGE_REACHED, ADIE_CODEC_DIGITAL_OFF} }

static struct adie_codec_action_unit
	ihs_stereo_speaker_stereo_rx_48KHz_osr256_actions[] =
	_HEADSET_STEREO_SPEAKER_STEREO_RX_CAPLESS_48000_OSR_256;
#else
static struct adie_codec_action_unit
	ihs_stereo_speaker_stereo_rx_48KHz_osr256_actions[] =
	HEADSET_STEREO_SPEAKER_STEREO_RX_CAPLESS_48000_OSR_256;
#endif

static struct adie_codec_hwsetting_entry
	ihs_stereo_speaker_stereo_rx_settings[] = {
	{
		.freq_plan = 48000,
		.osr = 256,
		.actions = ihs_stereo_speaker_stereo_rx_48KHz_osr256_actions,
		.action_sz =
		ARRAY_SIZE(ihs_stereo_speaker_stereo_rx_48KHz_osr256_actions),
	}
};

static struct adie_codec_dev_profile ihs_stereo_speaker_stereo_rx_profile = {
	.path_type = ADIE_CODEC_RX,
	.settings = ihs_stereo_speaker_stereo_rx_settings,
	.setting_sz = ARRAY_SIZE(ihs_stereo_speaker_stereo_rx_settings),
};

static struct snddev_icodec_data snddev_ihs_stereo_speaker_stereo_rx_data = {
	.capability = (SNDDEV_CAP_RX | SNDDEV_CAP_VOICE),
	.name = "headset_stereo_speaker_stereo_rx",
	.copp_id = 0,
	.acdb_id = ACDB_ID_HEADSET_STEREO_PLUS_SPKR_STEREO_RX,
	.profile = &ihs_stereo_speaker_stereo_rx_profile,
	.channel_mode = 2,
	.default_sample_rate = 48000,
#ifdef CONFIG_PANTECH_AUDIO_FAB2200    //20110428 jhsong : feature clearup
	.pamp_on = &snd_subsystem_sp_hp_poweron,
	.pamp_off = &snd_subsystem_sp_hp_powerdown,	
	.voltage_on = &snd_subsystem_pmic_vreg_l2_on,
	.voltage_off = &snd_subsystem_pmic_vreg_l2_off,
#else
	.pamp_on = msm_snddev_poweramp_on,
	.pamp_off = msm_snddev_poweramp_off,
	.voltage_on = msm_snddev_hsed_voltage_on,
	.voltage_off = msm_snddev_hsed_voltage_off,
#endif	
	.max_voice_rx_vol[VOC_NB_INDEX] = -500,
	.min_voice_rx_vol[VOC_NB_INDEX] = -2000,
	.max_voice_rx_vol[VOC_WB_INDEX] = -500,
	.min_voice_rx_vol[VOC_WB_INDEX] = -2000,
};

static struct platform_device msm_ihs_stereo_speaker_stereo_rx_device = {
	.name = "snddev_icodec",
	.id = 21,
	.dev = { .platform_data = &snddev_ihs_stereo_speaker_stereo_rx_data },
};

static struct snddev_mi2s_data snddev_mi2s_stereo_rx_data = {
	.capability = SNDDEV_CAP_RX ,
	.name = "hdmi_stereo_rx",
	.copp_id = 3,
	.acdb_id = ACDB_ID_HDMI,
	.channel_mode = 2,
	.sd_lines = MI2S_SD_0,
#ifdef CONFIG_PANTECH_AUDIO_FAB2200    //20110428 jhsong : feature clearup
	.route = NULL,
	.deroute = NULL,
#else
	.route = msm_snddev_tx_route_config,
	.deroute = msm_snddev_tx_route_deconfig,
#endif	
	.default_sample_rate = 48000,
};

static struct platform_device msm_snddev_mi2s_stereo_rx_device = {
	.name = "snddev_mi2s",
	.id = 0,
	.dev = { .platform_data = &snddev_mi2s_stereo_rx_data },
};


static struct snddev_mi2s_data snddev_mi2s_fm_tx_data = {
	.capability = SNDDEV_CAP_TX ,
	.name = "fmradio_stereo_tx",
	.copp_id = 2,
	.acdb_id = ACDB_ID_FM_TX,
	.channel_mode = 2,
	.sd_lines = MI2S_SD_3,
	.route = NULL,
	.deroute = NULL,
	.default_sample_rate = 48000,
};

static struct platform_device  msm_snddev_mi2s_fm_tx_device = {
	.name = "snddev_mi2s",
	.id = 1,
	.dev = { .platform_data = &snddev_mi2s_fm_tx_data},
};

static struct snddev_icodec_data snddev_fluid_imic_tx_data = {
	.capability = (SNDDEV_CAP_TX | SNDDEV_CAP_VOICE),
	.name = "handset_tx",
	.copp_id = 0,
	.acdb_id = ACDB_ID_SPKR_PHONE_MIC,
	.profile = &ispeaker_tx_profile,
	.channel_mode = 1,
	.pmctl_id = ispk_pmctl_id,
	.pmctl_id_sz = ARRAY_SIZE(ispk_pmctl_id),
	.default_sample_rate = 48000,
#ifdef CONFIG_PANTECH_AUDIO_FAB2200    //20110428 jhsong : feature clearup
	.pamp_on = NULL,
	.pamp_off = NULL,
#else
	.pamp_on = msm_snddev_tx_route_config,
	.pamp_off = msm_snddev_tx_route_deconfig,
#endif	
};

static struct platform_device msm_fluid_imic_tx_device = {
	.name = "snddev_icodec",
	.id = 22,
	.dev = { .platform_data = &snddev_fluid_imic_tx_data },
};

static struct snddev_icodec_data snddev_fluid_iearpiece_rx_data = {
	.capability = (SNDDEV_CAP_RX | SNDDEV_CAP_VOICE),
	.name = "handset_rx",
	.copp_id = 0,
	.acdb_id = ACDB_ID_SPKR_PHONE_STEREO,
	.profile = &ispeaker_rx_profile,
	.channel_mode = 2,
	.pmctl_id = NULL,
	.pmctl_id_sz = 0,
	.default_sample_rate = 48000,
#ifdef CONFIG_PANTECH_AUDIO_FAB2200    //20110428 jhsong : feature clearup
	.pamp_on = NULL,
	.pamp_off = NULL,
#else
	.pamp_on = &msm_snddev_poweramp_on,
	.pamp_off = &msm_snddev_poweramp_off,
#endif
	.max_voice_rx_vol[VOC_NB_INDEX] = -500,
	.min_voice_rx_vol[VOC_NB_INDEX] = -1000,
	.max_voice_rx_vol[VOC_WB_INDEX] = -500,
	.min_voice_rx_vol[VOC_WB_INDEX] = -1000,
};

static struct platform_device msm_fluid_iearpeice_rx_device = {
	.name = "snddev_icodec",
	.id = 23,
	.dev = { .platform_data = &snddev_fluid_iearpiece_rx_data },
};

static struct adie_codec_action_unit fluid_idual_mic_ef_8KHz_osr256_actions[] =
	MIC1_LEFT_AUX_IN_RIGHT_8000_OSR_256;

static struct adie_codec_hwsetting_entry fluid_idual_mic_endfire_settings[] = {
	{
		.freq_plan = 8000,
		.osr = 256,
		.actions = fluid_idual_mic_ef_8KHz_osr256_actions,
		.action_sz = ARRAY_SIZE(fluid_idual_mic_ef_8KHz_osr256_actions),
	}, /* 8KHz profile can be used for 16KHz */
	{
		.freq_plan = 16000,
		.osr = 256,
		.actions = fluid_idual_mic_ef_8KHz_osr256_actions,
		.action_sz = ARRAY_SIZE(fluid_idual_mic_ef_8KHz_osr256_actions),
	}, /* 8KHz profile can also be used for 48KHz */
	{
		.freq_plan = 48000,
		.osr = 256,
		.actions = fluid_idual_mic_ef_8KHz_osr256_actions,
		.action_sz = ARRAY_SIZE(fluid_idual_mic_ef_8KHz_osr256_actions),
	}
};

static struct adie_codec_dev_profile fluid_idual_mic_endfire_profile = {
	.path_type = ADIE_CODEC_TX,
	.settings = fluid_idual_mic_endfire_settings,
	.setting_sz = ARRAY_SIZE(fluid_idual_mic_endfire_settings),
};

static enum hsed_controller fluid_idual_mic_endfire_pmctl_id[] = {
	PM_HSED_CONTROLLER_0, PM_HSED_CONTROLLER_2
};

static struct snddev_icodec_data snddev_fluid_idual_mic_endfire_data = {
	.capability = (SNDDEV_CAP_TX | SNDDEV_CAP_VOICE),
	.name = "handset_dual_mic_endfire_tx",
	.copp_id = 0,
	.acdb_id = ACDB_ID_SPKR_PHONE_MIC_ENDFIRE,
	.profile = &fluid_idual_mic_endfire_profile,
	.channel_mode = 2,
	.default_sample_rate = 48000,
	.pmctl_id = fluid_idual_mic_endfire_pmctl_id,
	.pmctl_id_sz = ARRAY_SIZE(fluid_idual_mic_endfire_pmctl_id),
#ifdef CONFIG_PANTECH_AUDIO_FAB2200    //20110428 jhsong : feature clearup
	.pamp_on = NULL,
	.pamp_off = NULL,
#else
	.pamp_on = msm_snddev_tx_route_config,
	.pamp_off = msm_snddev_tx_route_deconfig,
#endif
};

static struct platform_device msm_fluid_idual_mic_endfire_device = {
	.name = "snddev_icodec",
	.id = 24,
	.dev = { .platform_data = &snddev_fluid_idual_mic_endfire_data },
};

static struct snddev_icodec_data snddev_fluid_spk_idual_mic_endfire_data = {
	.capability = (SNDDEV_CAP_TX | SNDDEV_CAP_VOICE),
	.name = "speaker_dual_mic_endfire_tx",
	.copp_id = 0,
	.acdb_id = ACDB_ID_SPKR_PHONE_MIC_ENDFIRE,
	.profile = &fluid_idual_mic_endfire_profile,
	.channel_mode = 2,
	.default_sample_rate = 48000,
	.pmctl_id = fluid_idual_mic_endfire_pmctl_id,
	.pmctl_id_sz = ARRAY_SIZE(fluid_idual_mic_endfire_pmctl_id),
#ifdef CONFIG_PANTECH_AUDIO_FAB2200    //20110428 jhsong : feature clearup
	.pamp_on = NULL,
	.pamp_off = NULL,
#else
	.pamp_on = msm_snddev_tx_route_config,
	.pamp_off = msm_snddev_tx_route_deconfig,
#endif	
};

static struct platform_device msm_fluid_spk_idual_mic_endfire_device = {
	.name = "snddev_icodec",
	.id = 25,
	.dev = { .platform_data = &snddev_fluid_spk_idual_mic_endfire_data },
};

static struct snddev_virtual_data snddev_a2dp_tx_data = {
	.capability = SNDDEV_CAP_TX,
	.name = "a2dp_tx",
	.copp_id = 5,
	.acdb_id = PSEUDO_ACDB_ID,
};

static struct snddev_virtual_data snddev_a2dp_rx_data = {
	.capability = SNDDEV_CAP_RX,
	.name = "a2dp_rx",
	.copp_id = 2,
	.acdb_id = PSEUDO_ACDB_ID,
};

static struct platform_device msm_a2dp_rx_device = {
	.name = "snddev_virtual",
	.id = 0,
	.dev = { .platform_data = &snddev_a2dp_rx_data },
};

static struct platform_device msm_a2dp_tx_device = {
	.name = "snddev_virtual",
	.id = 1,
	.dev = { .platform_data = &snddev_a2dp_tx_data },
};

static struct snddev_virtual_data snddev_uplink_rx_data = {
	.capability = SNDDEV_CAP_RX,
	.name = "uplink_rx",
	.copp_id = 5,
	.acdb_id = PSEUDO_ACDB_ID,
};

static struct platform_device msm_uplink_rx_device = {
	.name = "snddev_virtual",
	.id = 2,
	.dev = { .platform_data = &snddev_uplink_rx_data },
};

static struct platform_device *snd_devices_ffa[] __initdata = {
	&msm_iearpiece_ffa_device,
	&msm_imic_ffa_device,
	&msm_ifmradio_handset_device,
	&msm_ihs_ffa_stereo_rx_device,
	&msm_ihs_ffa_mono_rx_device,
	&msm_ihs_mono_tx_device,
	&msm_bt_sco_earpiece_device,
	&msm_bt_sco_mic_device,
	&msm_ispeaker_rx_device,
	&msm_ifmradio_speaker_device,
	&msm_ifmradio_ffa_headset_device,
	&msm_idual_mic_endfire_device,
	&msm_idual_mic_broadside_device,
	&msm_spk_idual_mic_endfire_device,
	&msm_spk_idual_mic_broadside_device,
	&msm_itty_hs_mono_tx_device,
	&msm_itty_hs_mono_rx_device,
	&msm_ispeaker_tx_device,
	&msm_ihs_stereo_speaker_stereo_rx_device,
	&msm_a2dp_rx_device,
	&msm_a2dp_tx_device,
	&msm_snddev_mi2s_stereo_rx_device,
	&msm_snddev_mi2s_fm_tx_device,
	&msm_uplink_rx_device,
	&msm_real_stereo_tx_device,
};

static struct platform_device *snd_devices_surf[] __initdata = {
	&msm_iearpiece_device,
	&msm_imic_device,
	&msm_ihs_stereo_rx_device,
	&msm_ihs_mono_rx_device,
	&msm_ihs_mono_tx_device,
	&msm_bt_sco_earpiece_device,
	&msm_bt_sco_mic_device,
	&msm_ifmradio_handset_device,
	&msm_ispeaker_rx_device,
	&msm_ifmradio_speaker_device,
	&msm_ifmradio_headset_device,
	&msm_itty_hs_mono_tx_device,
	&msm_itty_hs_mono_rx_device,
	&msm_ispeaker_tx_device,
	&msm_ihs_stereo_speaker_stereo_rx_device,
	&msm_a2dp_rx_device,
	&msm_a2dp_tx_device,
	&msm_snddev_mi2s_stereo_rx_device,
	&msm_snddev_mi2s_fm_tx_device,
	&msm_uplink_rx_device,
#ifdef CONFIG_PANTECH_MINI_AUDIO_AUDIENCE1028 // jmlee
	&msm_nr_ct_rx_device,
	&msm_nr_ct_tx_device,
	&msm_nr_ft_rx_device,
	&msm_nr_ft_tx_device,
	&msm_nr_dv_rx_device,
	&msm_nr_dv_tx_device,
	&msm_nr_off_ct_rx_device,
	&msm_nr_off_ct_tx_device,
	&msm_nr_off_ft_rx_device,
	&msm_nr_off_ft_tx_device,
	&msm_nr_off_dv_rx_device,
	&msm_nr_off_dv_tx_device,
	&msm_nr_ct_2mic_tx_device,
#endif
#ifdef CONFIG_PANTECH_ASR_PATH    //20110428 jhsong : feature clearup
	&msm_asr_handset_tx_device,
	&msm_asr_headset_tx_device,
#endif
#ifdef CONFIG_PANTECH_MINI_BT_GROUPING  //20110421 jhsong : bt grouping for att
	&msm_bt_sco_a_earpiece_device,
	&msm_bt_sco_a_mic_device,
	&msm_bt_sco_b_earpiece_device,
	&msm_bt_sco_b_mic_device,
	&msm_bt_sco_c_earpiece_device,
	&msm_bt_sco_c_mic_device,
	&msm_bt_sco_d_earpiece_device,
	&msm_bt_sco_d_mic_device,
	&msm_bt_sco_e_earpiece_device,
	&msm_bt_sco_e_mic_device,
	&msm_bt_sco_f_earpiece_device,
	&msm_bt_sco_f_mic_device,
	&msm_bt_sco_g_earpiece_device,
	&msm_bt_sco_g_mic_device,
	&msm_bt_sco_h_earpiece_device,
	&msm_bt_sco_h_mic_device,
	&msm_bt_sco_i_earpiece_device,
	&msm_bt_sco_i_mic_device,
#endif
};

static struct platform_device *snd_devices_fluid[] __initdata = {
	&msm_ihs_stereo_rx_device,
	&msm_ihs_mono_rx_device,
	&msm_ihs_mono_tx_device,
	&msm_ispeaker_rx_device,
	&msm_ispeaker_tx_device,
	&msm_fluid_imic_tx_device,
	&msm_fluid_iearpeice_rx_device,
	&msm_fluid_idual_mic_endfire_device,
	&msm_fluid_spk_idual_mic_endfire_device,
	&msm_a2dp_rx_device,
	&msm_a2dp_tx_device,
	&msm_snddev_mi2s_stereo_rx_device,
	&msm_uplink_rx_device,
	&msm_ifmradio_speaker_device,
	&msm_ifmradio_headset_device,
};

#ifdef CONFIG_DEBUG_FS
static void snddev_hsed_config_modify_setting(int type)
{
	struct platform_device *device;
	struct snddev_icodec_data *icodec_data;

	device = &msm_ihs_ffa_stereo_rx_device;
	icodec_data = (struct snddev_icodec_data *)device->dev.platform_data;

	if (icodec_data) {
		if (type == 1) {
			icodec_data->voltage_on = NULL;
			icodec_data->voltage_off = NULL;
			icodec_data->profile->settings =
				ihs_ffa_stereo_rx_class_d_legacy_settings;
			icodec_data->profile->setting_sz =
			ARRAY_SIZE(ihs_ffa_stereo_rx_class_d_legacy_settings);
		} else if (type == 2) {
			icodec_data->voltage_on = NULL;
			icodec_data->voltage_off = NULL;
			icodec_data->profile->settings =
				ihs_ffa_stereo_rx_class_ab_legacy_settings;
			icodec_data->profile->setting_sz =
			ARRAY_SIZE(ihs_ffa_stereo_rx_class_ab_legacy_settings);
		}
	}
}

static void snddev_hsed_config_restore_setting(void)
{
	struct platform_device *device;
	struct snddev_icodec_data *icodec_data;

	device = &msm_ihs_ffa_stereo_rx_device;
	icodec_data = (struct snddev_icodec_data *)device->dev.platform_data;

	if (icodec_data) {
#ifdef CONFIG_PANTECH_AUDIO_FAB2200    //20110428 jhsong : feature clearup
		icodec_data->voltage_on = &snd_subsystem_pmic_vreg_l2_on;
		icodec_data->voltage_off = &snd_subsystem_pmic_vreg_l2_off;
#else
		icodec_data->voltage_on = msm_snddev_hsed_voltage_on;
		icodec_data->voltage_off = msm_snddev_hsed_voltage_off;
#endif
		icodec_data->profile->settings = ihs_ffa_stereo_rx_settings;
		icodec_data->profile->setting_sz =
			ARRAY_SIZE(ihs_ffa_stereo_rx_settings);
	}
}

static ssize_t snddev_hsed_config_debug_write(struct file *filp,
	const char __user *ubuf, size_t cnt, loff_t *ppos)
{
	char *lb_str = filp->private_data;
	char cmd;

	if (get_user(cmd, ubuf))
		return -EFAULT;

	if (!strcmp(lb_str, "msm_hsed_config")) {
		switch (cmd) {
		case '0':
			snddev_hsed_config_restore_setting();
			break;

		case '1':
			snddev_hsed_config_modify_setting(1);
			break;

		case '2':
			snddev_hsed_config_modify_setting(2);
			break;

		default:
			break;
		}
	}
	return cnt;
}

static int snddev_hsed_config_debug_open(struct inode *inode, struct file *file)
{
	file->private_data = inode->i_private;
	return 0;
}

static const struct file_operations snddev_hsed_config_debug_fops = {
	.open = snddev_hsed_config_debug_open,
	.write = snddev_hsed_config_debug_write
};
#endif

void __ref msm_snddev_init(void)
{
	if (machine_is_msm7x30_ffa() || machine_is_msm8x55_ffa() ||
		machine_is_msm8x55_svlte_ffa()) {
		platform_add_devices(snd_devices_ffa,
		ARRAY_SIZE(snd_devices_ffa));
#ifdef CONFIG_DEBUG_FS
		debugfs_hsed_config = debugfs_create_file("msm_hsed_config",
					S_IFREG | S_IRUGO, NULL,
		(void *) "msm_hsed_config", &snddev_hsed_config_debug_fops);
#endif
	} else if (machine_is_msm7x30_surf() || machine_is_msm8x55_surf() ||
		machine_is_msm8x55_svlte_surf() || machine_is_msm8x55_mini() ||machine_is_msm8x55_j_masai_e())
		platform_add_devices(snd_devices_surf,
		ARRAY_SIZE(snd_devices_surf));
	else if (machine_is_msm7x30_fluid())
		platform_add_devices(snd_devices_fluid,
		ARRAY_SIZE(snd_devices_fluid));
	else
		pr_err("%s: Unknown machine type\n", __func__);
}
