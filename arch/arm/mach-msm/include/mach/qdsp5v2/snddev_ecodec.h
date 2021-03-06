/* Copyright (c) 2009, Code Aurora Forum. All rights reserved.
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
#ifndef __MACH_QDSP5_V2_SNDDEV_ECODEC_H
#define __MACH_QDSP5_V2_SNDDEV_ECODEC_H
#include <mach/qdsp5v2/audio_def.h>
#include <mach/pmic.h>  //20110224 jhsong

struct snddev_ecodec_data {
	u32 capability; /* RX or TX */
	const char *name;
	u32 copp_id; /* audpp routing */
	u32 acdb_id; /* Audio Cal purpose */
	u8 channel_mode;
	u32 conf_pcm_ctl_val;
	u32 conf_aux_codec_intf;
	u32 conf_data_format_padding_val;
	s32 max_voice_rx_vol[VOC_RX_VOL_ARRAY_NUM]; /* [0]:NB, [1]:WB */
	s32 min_voice_rx_vol[VOC_RX_VOL_ARRAY_NUM];
#if defined(CONFIG_PANTECH_MINI_AUDIO_AUDIENCE1028) || defined(CONFIG_PANTECH_LASER2_AUDIENCE2020)
	enum hsed_controller *pmctl_id; /* tx only enable mic bias */  //20110224 jhsong
	u32 pmctl_id_sz;  //20110224 jhsong
	void (*pamp_on) (void);
	void (*pamp_off) (void);
#endif	
};
#endif
