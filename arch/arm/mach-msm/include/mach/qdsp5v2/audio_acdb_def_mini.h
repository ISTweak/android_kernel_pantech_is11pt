/* Copyright (c) 2010, Code Aurora Forum. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above
 *       copyright notice, this list of conditions and the following
 *       disclaimer in the documentation and/or other materials provided
 *       with the distribution.
 *     * Neither the name of Code Aurora Forum, Inc. nor the names of its
 *       contributors may be used to endorse or promote products derived
 *       from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED "AS IS" AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS
 * BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR
 * BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE
 * OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN
 * IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 */
#ifndef _MACH_QDSP5_V2_AUDIO_ACDB_DEF_H
#define _MACH_QDSP5_V2_AUDIO_ACDB_DEF_H

/* Define ACDB device ID */
#define ACDB_ID_HANDSET_SPKR				1
#define ACDB_ID_HANDSET_MIC				2
#define ACDB_ID_HEADSET_MIC				3
#define ACDB_ID_HEADSET_SPKR_MONO			4
#define ACDB_ID_HEADSET_SPKR_STEREO			5
#define ACDB_ID_SPKR_PHONE_MIC				6
#define ACDB_ID_SPKR_PHONE_MONO				7
#define ACDB_ID_SPKR_PHONE_STEREO			8
#define ACDB_ID_BT_SCO_MIC				9
#define ACDB_ID_BT_SCO_SPKR				0x0A
#define ACDB_ID_BT_A2DP_SPKR				0x0B
#define ACDB_ID_BT_A2DP_TX				0x10
#define ACDB_ID_TTY_HEADSET_MIC				0x0C
#define ACDB_ID_TTY_HEADSET_SPKR			0x0D
#define ACDB_ID_HEADSET_MONO_PLUS_SPKR_MONO_RX		0x11
#define ACDB_ID_HEADSET_STEREO_PLUS_SPKR_STEREO_RX	0x14
#define ACDB_ID_FM_TX_LOOPBACK				0x17
#define ACDB_ID_FM_TX					0x18
#define ACDB_ID_LP_FM_SPKR_PHONE_STEREO_RX		0x19
#define ACDB_ID_LP_FM_HEADSET_SPKR_STEREO_RX		0x1A
#define ACDB_ID_I2S_RX					0x20
#define ACDB_ID_SPKR_PHONE_MIC_BROADSIDE		0x2B
#define ACDB_ID_HANDSET_MIC_BROADSIDE			0x2C
#define ACDB_ID_SPKR_PHONE_MIC_ENDFIRE			0x2D
#define ACDB_ID_HANDSET_MIC_ENDFIRE			0x2E
#define ACDB_ID_I2S_TX					0x30
#define ACDB_ID_HDMI					0x40


#ifdef CONFIG_PANTECH_MINI_AUDIO_AUDIENCE1028 //jmlee
#define ACDB_ID_NR_CT_RX					0x41
#define ACDB_ID_NR_CT_TX					0x42
#define ACDB_ID_NR_FT_RX					0x43
#define ACDB_ID_NR_FT_TX					0x44
#define ACDB_ID_NR_DV_RX					0x45
#define ACDB_ID_NR_DV_TX					0x46
#define ACDB_ID_NR_OFF_CT_RX					0x47
#define ACDB_ID_NR_OFF_CT_TX					0x48
#define ACDB_ID_NR_OFF_FT_RX					0x49
#define ACDB_ID_NR_OFF_FT_TX					0x4A
#define ACDB_ID_NR_OFF_DV_RX					0x4B
#define ACDB_ID_NR_OFF_DV_TX					0x4C
#define ACDB_ID_NR_CT_2MIC_TX					0x4F
#endif

#ifdef CONFIG_PANTECH_ASR_PATH    //20110428 jhsong : feature clearup
#define ACDB_ID_ASR_HANDSET_TX					0x4D
#define ACDB_ID_ASR_HEADSET_TX					0x4E
#endif

#ifdef CONFIG_PANTECH_MINI_BT_GROUPING  //20110421 jhsong : bt grouping for att
#define ACDB_ID_BT_SCO_A_MIC				0x50
#define ACDB_ID_BT_SCO_A_SPKR				0x51
#define ACDB_ID_BT_SCO_B_MIC				0x52
#define ACDB_ID_BT_SCO_B_SPKR				0x53
#define ACDB_ID_BT_SCO_C_MIC				0x54
#define ACDB_ID_BT_SCO_C_SPKR				0x55
#define ACDB_ID_BT_SCO_D_MIC				0x56
#define ACDB_ID_BT_SCO_D_SPKR				0x57
#define ACDB_ID_BT_SCO_E_MIC				0x58
#define ACDB_ID_BT_SCO_E_SPKR				0x59
#define ACDB_ID_BT_SCO_F_MIC				0x5A
#define ACDB_ID_BT_SCO_F_SPKR				0x5B
#define ACDB_ID_BT_SCO_G_MIC				0x5C
#define ACDB_ID_BT_SCO_G_SPKR				0x5D
#define ACDB_ID_BT_SCO_H_MIC				0x5E
#define ACDB_ID_BT_SCO_H_SPKR				0x5F
#define ACDB_ID_BT_SCO_I_MIC				0x60
#define ACDB_ID_BT_SCO_I_SPKR				0x61
#endif

#ifdef CONFIG_PANTECH_AUDIO_RX_VOLUME   //20110428 jhsong : feature clearup
//#define ACDB_ID_MAX		ACDB_ID_NR_OFF_DV_TX + 1					
#ifdef CONFIG_PANTECH_MINI_BT_GROUPING  //20110421 jhsong : bt grouping for att
#define ACDB_ID_MAX		ACDB_ID_BT_SCO_I_SPKR + 1					
#else
#define ACDB_ID_MAX		ACDB_ID_ASR_HEADSET_TX + 1					
#endif

#endif

/* ID used for virtual devices */
#define PSEUDO_ACDB_ID 					0xFFFF

#endif /* _MACH_QDSP5_V2_AUDIO_ACDB_DEF_H */
