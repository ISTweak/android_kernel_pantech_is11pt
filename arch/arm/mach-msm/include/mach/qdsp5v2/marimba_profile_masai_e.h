/* Copyright (c) 2009-2010,  Code Aurora Forum. All rights reserved.
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
#ifndef __MACH_QDSP5_V2_MARIMBA_PROFILE_H__
#define __MACH_QDSP5_V2_MARIMBA_PROFILE_H__


///* ------------------------------------------------------------------
// | ### MIC AMP ### (HW Tuning Table name)
// * ------------------------------------------------------------------
// | 0x0D | TXFE LEFT GAIN SETTING(Codec Tx front end control register 1)
// | 6:5  | 00 : 0dB
// |      | 01 : 4.5dB
// |      | 1x : 24dB 
// * ------------------------------------------------------------------
// | ### Codec ST Gain ### (HW Tuning Table name)
// * ------------------------------------------------------------------
// | 0x8B | SIDETONE_GAIN(CDC_STG)
// | 7:0  | Sidetone gain (dB) in 2s complement; valid supported range is -84 to 0 dB
// |      | Negative numbers <7:0>: calculate as follows:
// |      | . Find positive opposite value in decimal: p
// |      | . Find binary equivalent of positive value: b
// |      | . Invert the binary number: binv
// |      | . Increment number by 1: value = binv+1.
// |      | For example: -84dB
// |      | . p = 84.
// |      | . b = 1010100
// |      | . binv = 0101011
// |      | . value = 0101011+1 = 10101100 = 0xAC
// |      | Special number
// |      | 0dB = 00000000 = 0x0
// * ------------------------------------------------------------------
// | ### Codec Tx Gain ### (HW Tuning Table name)
// * ------------------------------------------------------------------
// | 0x86 | DIGITAL_TX_LEFT_GAIN(CDC_LTXG)
// | 7:0  | TX left channel digital gain(dB) in 2s complement; valid range is -66 to +30 dB.
// |      | Positive numbers <7>=0, <6:0> value: binary equivalent of number
// |      | For example: +30dB = 00011110 = 0x1E
// |      | . Find positive opposite value in decimal: p
// |      | . Find binary equivalent of positive value: b
// |      | . Invert the binary number: binv
// |      | . Increment number by 1: value = binv+1.
// |      | Example: -66dB
// |      | . p= 66
// |      | . b= 1000010
// |      | . binv = 0111101
// |      | . value = 0111101 + 1 =10111110 = 0xBE
// |      | Special number
// |      | 0dB = 00000000 = 0x0
// * ------------------------------------------------------------------ */



///* ------------------------------------------------------------------
// | ### Codec RX Gain left ### (HW Tuning Table name)
// * ------------------------------------------------------------------
// | 0x84 | DIGITAL_RX_LEFT_GAIN(CDC_LRXG)
// | 7:0  | RX left channel digital gain (dB) in 2s complement;
// |      | Valid supported range is -84 to +20 dB;
// |      | Positive numbers <7>=0, <6:0> value: binary equivalent of
// |      | number.
// |      | For example: +20dB=00010100=0x14
// |      | Negative numbers <7:0>; calculate as follows:
// |      | . Find positive opposite value in decimal: p
// |      | . Find binary equivalent of positive value: b
// |      | . Invert the binary number: binv
// |      | . Increment number by 1: value=binv+1.
// |      | For example: -84dB
// |      | . p=84
// |      | . b=1010100
// |      | . binv=0101011
// |      | . value=0101011+1=10101100=0xAC
// |      | Special number
// |      | 0dB=00000000=0x0
// * ------------------------------------------------------------------
// | ### Codec RX Gain right ### (HW Tuning Table name)
// * ------------------------------------------------------------------
// | 0x85 | DIGITAL_RX_RIGHT_GAIN(CDC_RRXG)
// | 7:0  | Valid range is -84 to +20 dB;
// |      | Positive numbers <7>= 0, <6:0> value: binary equivalent of
// |      | number.
// |      | For example: +20dB=00010100=0x14
// |      | Negative numbers <7:0>; calculate as follows:
// |      | . Find positive opposite value in decimal: p
// |      | . Find binary equivalent of positive value: b
// |      | . Invert the binary number: binv
// |      | . Increment number by 1: value = binv+1.
// |      | For example: -84dB
// |      | . p = 84
// |      | . b = 1010100
// |      | . binv = 0101011
// |      | . value = 0101011+1=10101100=0xAC
// |      | Special number
// |      | 0dB=00000000=0x0 
// * ------------------------------------------------------------------ */


/* ******************************************************************
 *                                                                  *
 *                        HANDSET(IN CALL)                          *
 *                                                                  *
 * ****************************************************************** */
 
/* ------------------------------------------------------------------
 | "handset_rx"
 * ------------------------------------------------------------------
 | Codec RX Gain left  | 0x84  | DIGITAL_RX_LEFT_GAIN(CDC_LRXG)
                       | (7:0) | RX left channel digital gain (dB) in 2s complement;
 * ------------------------------------------------------------------
 | Codec RX Gain right | 0x85  | DIGITAL_RX_RIGHT_GAIN(CDC_RRXG)
 |                     | (7:0) | Valid range is -84 to +20 dB;
 * ------------------------------------------------------------------ */
#define HANDSET_RX_48000_OSR_256\
	{{ADIE_CODEC_ACTION_STAGE_REACHED, ADIE_CODEC_FLASH_IMAGE}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x80, 0x02, 0x02)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x80, 0x02, 0x00)}, \
	{ADIE_CODEC_ACTION_STAGE_REACHED, ADIE_CODEC_DIGITAL_READY}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x24, 0x6F, 0x44)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x04, 0xfF, 0x8C)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x81, 0xFF, /*0x4e*/0x6e)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x25, 0x0F, 0x0b)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x26, 0xfc, 0xfc)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x36, 0xc0, 0x80)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3A, 0xFF, 0x2B)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x23, 0xff, 0x20)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3d, 0xFF, 0x55)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x83, 0x21, 0x21)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x33, 0x80, 0x80)}, \
	{ADIE_CODEC_ACTION_DELAY_WAIT, 0x2710}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x33, 0x40, 0x40)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x84, 0xff, 0xfc)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x8A, 0x05, 0x04)}, \
	{ADIE_CODEC_ACTION_STAGE_REACHED, ADIE_CODEC_DIGITAL_ANALOG_READY}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x8a, 0x01, 0x01)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x36, 0xc0, 0x00)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x33, 0x40, 0x00)}, \
	{ADIE_CODEC_ACTION_STAGE_REACHED, ADIE_CODEC_ANALOG_OFF}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x33, 0x80, 0x00)}, \
	{ADIE_CODEC_ACTION_STAGE_REACHED, ADIE_CODEC_DIGITAL_OFF} }
 
#define HANDSET_RX_16000_OSR_256 HANDSET_RX_8000_OSR_256


/* ------------------------------------------------------------------
 | NAME ### "handset_tx"
 * ------------------------------------------------------------------
 | MIC AMP       | 0x0D  | TXFE LEFT GAIN SETTING(Codec Tx front end control register 1)
 |               | (6:5) | 00 : 0dB
 |               |       | 01 : 4.5dB
 |               |       | 1x : 24dB
 * ------------------------------------------------------------------
 | Codec ST Gain | 0x8B  | SIDETONE_GAIN(CDC_STG)
 |               | (7:0) | Sidetone gain (dB) in 2s complement; valid supported range is -84 to 0 dB.
 * ------------------------------------------------------------------
 | Codec Tx Gain | 0x86  | DIGITAL_TX_LEFT_GAIN(CDC_LTXG)
 |               | (7:0) | TX left channel digital gain(dB) in 2s complement; valid range is -66 to +30 dB.
 * ------------------------------------------------------------------ */
#define HANDSET_TX_48000_OSR_256 \
	{{ADIE_CODEC_ACTION_STAGE_REACHED, ADIE_CODEC_FLASH_IMAGE}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x80, 0x01, 0x01)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x80, 0x01, 0x00)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x8A, 0x30, 0x30)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x11, 0xfc, 0xfc)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x13, 0xfc, 0x58)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x14, 0xff, 0x65)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x15, 0xff, 0x64)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x82, 0xff, 0x5A)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x10, 0xFF, 0x68)}, \
	{ADIE_CODEC_ACTION_STAGE_REACHED, ADIE_CODEC_DIGITAL_READY}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x0D, 0xF0, 0xd0)}, \
	{ADIE_CODEC_ACTION_DELAY_WAIT, 0xbb8}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x83, 0x14, 0x14)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x8b, 0xff, /*0xE6*/0xac)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x8c, 0x03, 0x02)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x86, 0xff, 0x05)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x8A, 0x50, 0x40)}, \
	{ADIE_CODEC_ACTION_STAGE_REACHED, ADIE_CODEC_DIGITAL_ANALOG_READY}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x8A, 0x10, 0x30)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x0D, 0xFF, 0x00)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x83, 0x14, 0x00)}, \
	{ADIE_CODEC_ACTION_STAGE_REACHED, ADIE_CODEC_ANALOG_OFF}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x11, 0xff, 0x00)}, \
	{ADIE_CODEC_ACTION_STAGE_REACHED, ADIE_CODEC_DIGITAL_OFF} }



/* ******************************************************************
 *                                                                  *
 *                        HEADSET(INCALL)                           *
 *                                                                  *
 * ****************************************************************** */
 
/* ------------------------------------------------------------------
 | "voice_headset_rx"
 * ------------------------------------------------------------------
 | Codec RX Gain left  | 0x84  | DIGITAL_RX_LEFT_GAIN(CDC_LRXG)
 |                     | (7:0) | RX left channel digital gain (dB) in 2s complement;
 * ------------------------------------------------------------------
 | Codec RX Gain right | 0x85  | DIGITAL_RX_RIGHT_GAIN(CDC_RRXG)
 |                     | (7:0) | Valid range is -84 to +20 dB;
 * ------------------------------------------------------------------ */

#define HEADSET_VOICE_STEREO_RX_LEGACY_48000_OSR_256 \
	{{ADIE_CODEC_ACTION_STAGE_REACHED, ADIE_CODEC_FLASH_IMAGE}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x80, 0x02, 0x02)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x80, 0x02, 0x00)}, \
	{ADIE_CODEC_ACTION_STAGE_REACHED, ADIE_CODEC_DIGITAL_READY}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3B, 0xFF, 0xac)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3C, 0xFF, 0xac)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x81, 0xFF, /*0x4e*/0x0e)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x04, 0xff, 0xBC)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x24, 0x6F, 0x64)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x25, 0x0F, 0x0B)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x26, 0xfc, 0xfc)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x38, 0xff, 0x80)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3A, 0xFF, 0x2b)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x83, 0x23, 0x23)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x33, 0x80, 0x80)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x34, 0xf0, 0xa0)}, \
	{ADIE_CODEC_ACTION_DELAY_WAIT, 0x186A0}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x34, 0xf0, 0xf0)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x23, 0xff, 0x20)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3B, 0xFF, 0x98)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3C, 0xFF, 0x98)}, \
	{ADIE_CODEC_ACTION_DELAY_WAIT, 0x3E8}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3B, 0xFF, 0x88)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3C, 0xFF, 0x88)}, \
	{ADIE_CODEC_ACTION_DELAY_WAIT, 0x3E8}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3B, 0xFF, 0x78)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3C, 0xFF, 0x78)}, \
	{ADIE_CODEC_ACTION_DELAY_WAIT, 0x3E8}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3B, 0xFF, 0x68)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3C, 0xFF, 0x68)}, \
	{ADIE_CODEC_ACTION_DELAY_WAIT, 0x3E8}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3B, 0xFF, 0x58)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3C, 0xFF, 0x58)}, \
	{ADIE_CODEC_ACTION_DELAY_WAIT, 0x3E8}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3B, 0xFF, 0x48)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3C, 0xFF, 0x48)}, \
	{ADIE_CODEC_ACTION_DELAY_WAIT, 0x3E8}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3B, 0xFF, 0x38)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3C, 0xFF, 0x38)}, \
	{ADIE_CODEC_ACTION_DELAY_WAIT, 0x3E8}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3B, 0xFF, 0x28)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3C, 0xFF, 0x28)}, \
	{ADIE_CODEC_ACTION_DELAY_WAIT, 0x3E8}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3B, 0xFF, 0x18)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3C, 0xFF, 0x18)}, \
	{ADIE_CODEC_ACTION_DELAY_WAIT, 0x3E8}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3B, 0xFF, 0x10)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3C, 0xFF, 0x10)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x84, 0xff, 0xfb)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x85, 0xff, 0xfb)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x8A, 0x0f, 0x0c)}, \
	{ADIE_CODEC_ACTION_STAGE_REACHED, ADIE_CODEC_DIGITAL_ANALOG_READY}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x8a, 0x03, 0x03)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3b, 0xFF, 0xAC)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3c, 0xFF, 0xAC)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x34, 0xf0, 0x00)}, \
	{ADIE_CODEC_ACTION_STAGE_REACHED, ADIE_CODEC_ANALOG_OFF}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x33, 0x80, 0x00)}, \
	{ADIE_CODEC_ACTION_STAGE_REACHED, ADIE_CODEC_DIGITAL_OFF} }

/* ------------------------------------------------------------------
 | NAME ### "voice_headset_tx"
 * ------------------------------------------------------------------
 | MIC AMP       | 0x0D  | TXFE LEFT GAIN SETTING(Codec Tx front end control register 1)
 |               | (6:5) | 00 : 0dB
 |                       | 01 : 4.5dB
 |                       | 1x : 24dB
 * ------------------------------------------------------------------
 | Codec ST Gain | 0x8B  | SIDETONE_GAIN(CDC_STG)
 |               | (7:0) | Sidetone gain (dB) in 2s complement; valid supported range is -84 to 0 dB.
 * ------------------------------------------------------------------
 | Codec Tx Gain | 0x86  | DIGITAL_TX_LEFT_GAIN(CDC_LTXG)
 |               | (7:0) | TX left channel digital gain(dB) in 2s complement; valid range is -66 to +30 dB.
 * ------------------------------------------------------------------ */
 
/*20110518 jhsong : audio cal side tone : -84dB*/
#define HEADSET_VOICE_MONO_TX_48000_OSR_256 \
	{{ADIE_CODEC_ACTION_STAGE_REACHED, ADIE_CODEC_FLASH_IMAGE}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x80, 0x01, 0x01)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x80, 0x01, 0x00)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x8A, 0x30, 0x30)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x11, 0xfc, 0xfc)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x13, 0xfc, 0x58)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x14, 0xff, 0x65)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x15, 0xff, 0x64)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x82, 0xff, 0x5A)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x10, 0xFF, 0x64)}, \
	{ADIE_CODEC_ACTION_STAGE_REACHED, ADIE_CODEC_DIGITAL_READY}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x0D, 0xFF, 0xC8)}, \
	{ADIE_CODEC_ACTION_DELAY_WAIT, 0xbb8}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x83, 0x14, 0x14)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x8b, 0xff, /*0xE7*/0xac)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x8c, 0x03, 0x02)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x86, 0xff, 0x0c)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x8A, 0x50, 0x40)}, \
	{ADIE_CODEC_ACTION_STAGE_REACHED, ADIE_CODEC_DIGITAL_ANALOG_READY}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x8A, 0x10, 0x10)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x0D, 0xFF, 0x00)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x83, 0x14, 0x00)}, \
	{ADIE_CODEC_ACTION_STAGE_REACHED, ADIE_CODEC_ANALOG_OFF}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x11, 0xff, 0x00)}, \
	{ADIE_CODEC_ACTION_STAGE_REACHED, ADIE_CODEC_DIGITAL_OFF} }



/* ******************************************************************
 *                                                                  *
 *                        HEADSET                                   *
 *                                                                  *
 * ****************************************************************** */
 
/* ------------------------------------------------------------------
 | "headset_stereo_rx"
 * ------------------------------------------------------------------
 | Codec RX Gain left  | 0x84  | DIGITAL_RX_LEFT_GAIN(CDC_LRXG)
 |                     | (7:0) | RX left channel digital gain (dB) in 2s complement;
 * ------------------------------------------------------------------
 | Codec RX Gain right | 0x85  | DIGITAL_RX_RIGHT_GAIN(CDC_RRXG)
 |                     | (7:0) | Valid range is -84 to +20 dB;
 * ------------------------------------------------------------------ */
 
#define HEADSET_STEREO_RX_LEGACY_48000_OSR_256 \
	{{ADIE_CODEC_ACTION_STAGE_REACHED, ADIE_CODEC_FLASH_IMAGE}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x80, 0x02, 0x02)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x80, 0x02, 0x00)}, \
	{ADIE_CODEC_ACTION_STAGE_REACHED, ADIE_CODEC_DIGITAL_READY}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3B, 0xFF, 0xac)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3C, 0xFF, 0xac)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x81, 0xFF, 0x4e)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x04, 0xff, 0xBC)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x24, 0x6F, 0x64)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x25, 0x0F, 0x0B)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x26, 0xfc, 0xfc)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x38, 0xff, 0x80)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3A, 0xFF, 0x2b)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x83, 0x23, 0x23)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x33, 0x80, 0x80)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x34, 0xf0, 0xa0)}, \
	{ADIE_CODEC_ACTION_DELAY_WAIT, 0x186A0}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x34, 0xf0, 0xf0)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x23, 0xff, 0x20)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3B, 0xFF, 0x98)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3C, 0xFF, 0x98)}, \
	{ADIE_CODEC_ACTION_DELAY_WAIT, 0x3E8}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3B, 0xFF, 0x88)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3C, 0xFF, 0x88)}, \
	{ADIE_CODEC_ACTION_DELAY_WAIT, 0x3E8}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3B, 0xFF, 0x78)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3C, 0xFF, 0x78)}, \
	{ADIE_CODEC_ACTION_DELAY_WAIT, 0x3E8}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3B, 0xFF, 0x68)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3C, 0xFF, 0x68)}, \
	{ADIE_CODEC_ACTION_DELAY_WAIT, 0x3E8}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3B, 0xFF, 0x58)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3C, 0xFF, 0x58)}, \
	{ADIE_CODEC_ACTION_DELAY_WAIT, 0x3E8}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3B, 0xFF, 0x48)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3C, 0xFF, 0x48)}, \
	{ADIE_CODEC_ACTION_DELAY_WAIT, 0x3E8}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3B, 0xFF, 0x38)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3C, 0xFF, 0x38)}, \
	{ADIE_CODEC_ACTION_DELAY_WAIT, 0x3E8}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3B, 0xFF, 0x28)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3C, 0xFF, 0x28)}, \
	{ADIE_CODEC_ACTION_DELAY_WAIT, 0x3E8}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3B, 0xFF, 0x18)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3C, 0xFF, 0x18)}, \
	{ADIE_CODEC_ACTION_DELAY_WAIT, 0x3E8}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3B, 0xFF, 0x10)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3C, 0xFF, 0x10)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x84, 0xff, 0xfb)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x85, 0xff, 0xfb)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x8A, 0x0f, 0x0c)}, \
	{ADIE_CODEC_ACTION_STAGE_REACHED, ADIE_CODEC_DIGITAL_ANALOG_READY}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x8a, 0x03, 0x03)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3b, 0xFF, 0xAC)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3c, 0xFF, 0xAC)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x34, 0xf0, 0x00)}, \
	{ADIE_CODEC_ACTION_STAGE_REACHED, ADIE_CODEC_ANALOG_OFF}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x33, 0x80, 0x00)}, \
	{ADIE_CODEC_ACTION_STAGE_REACHED, ADIE_CODEC_DIGITAL_OFF} }

/* ------------------------------------------------------------------
 | "headset_mono_tx"
 * ------------------------------------------------------------------
 | MIC AMP         | 0x0D  | TXFE LEFT GAIN SETTING(Codec Tx front end control register 1)
 |                 | (6:5) | 00 : 0dB
 |                         | 01 : 4.5dB
 |                         | 1x : 24dB
 * ------------------------------------------------------------------
 | Codec ST Gain   | 0x8B  | SIDETONE_GAIN(CDC_STG)
 |                 | (7:0) | Sidetone gain (dB) in 2s complement; valid supported range is -84 to 0 dB. 
 * ------------------------------------------------------------------
 | Codec Tx Gain   | 0x86  | DIGITAL_TX_LEFT_GAIN(CDC_LTXG)
 |                 | (7:0) | TX left channel digital gain(dB) in 2s complement; valid range is -66 to +30 dB.
 * ------------------------------------------------------------------ */

#if defined (CONFIG_MACH_MSM8X55_J_MASAI_E)
#define HEADSET_MONO_TX_48000_OSR_256 \
	{{ADIE_CODEC_ACTION_STAGE_REACHED, ADIE_CODEC_FLASH_IMAGE}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x80, 0x01, 0x01)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x80, 0x01, 0x00)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x8A, 0x30, 0x30)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x11, 0xfc, 0xfc)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x13, 0xfc, 0x58)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x14, 0xff, 0x65)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x15, 0xff, 0x64)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x82, 0xff, 0x5A)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x10, 0xFF, 0x64)}, \
	{ADIE_CODEC_ACTION_STAGE_REACHED, ADIE_CODEC_DIGITAL_READY}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x0D, 0xFF, 0xC8)}, \
	{ADIE_CODEC_ACTION_DELAY_WAIT, 0xbb8}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x83, 0x14, 0x14)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x8b, 0xff, 0xac)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x8c, 0x03, 0x02)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x86, 0xff, 0x0e)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x8A, 0x50, 0x40)}, \
	{ADIE_CODEC_ACTION_STAGE_REACHED, ADIE_CODEC_DIGITAL_ANALOG_READY}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x8A, 0x10, 0x10)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x0D, 0xFF, 0x00)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x83, 0x14, 0x00)}, \
	{ADIE_CODEC_ACTION_STAGE_REACHED, ADIE_CODEC_ANALOG_OFF}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x11, 0xff, 0x00)}, \
	{ADIE_CODEC_ACTION_STAGE_REACHED, ADIE_CODEC_DIGITAL_OFF} }
#endif



/* ******************************************************************
 *                                                                  *
 *                        SPEAKER                                   *
 *                                                                  *
 * ****************************************************************** */
 
/* ------------------------------------------------------------------
 | "speaker_stereo_rx"
 * ------------------------------------------------------------------
 | Codec RX Gain left  | 0x84  | DIGITAL_RX_LEFT_GAIN(CDC_LRXG)
 |                     | (7:0) | RX left channel digital gain (dB) in 2s complement;
 * ------------------------------------------------------------------
 | Codec RX Gain right | 0x85  | DIGITAL_RX_RIGHT_GAIN(CDC_RRXG)
 |                     | (7:0) | Valid range is -84 to +20 dB;
 * ------------------------------------------------------------------ */
 
#ifdef CONFIG_PANTECH_AUDIO_FAB2200    //20110428 jhsong : feature clearup
#define SPEAKER_STEREO_RX_48000_OSR_256 \
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
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x84, 0xff, 0xfb)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x85, 0xff, 0xfb)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x8A, 0x0f, 0x0c/*, 0x05, 0x04*/)}, \
	{ADIE_CODEC_ACTION_STAGE_REACHED, ADIE_CODEC_DIGITAL_ANALOG_READY}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x8a, 0x03, 0x03/*, 0x01, 0x01*/)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x33, 0x40, 0x00)}, \
	{ADIE_CODEC_ACTION_STAGE_REACHED, ADIE_CODEC_ANALOG_OFF}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x33, 0x80, 0x00)}, \
	{ADIE_CODEC_ACTION_STAGE_REACHED, ADIE_CODEC_DIGITAL_OFF} }
#else
#define SPEAKER_STEREO_RX_48000_OSR_256 \
	{{ADIE_CODEC_ACTION_STAGE_REACHED, ADIE_CODEC_FLASH_IMAGE}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x80, 0x02, 0x02)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x80, 0x02, 0x00)}, \
	{ADIE_CODEC_ACTION_STAGE_REACHED, ADIE_CODEC_DIGITAL_READY}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x24, 0x6F, 0x64)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x25, 0x0F, 0x0B)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x26, 0xfc, 0xfc)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x37, 0xe6, 0x80)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3A, 0xFF, 0x2B)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3d, 0xFF, 0x55)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x83, 0x23, 0x23)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x23, 0xff, 0x20)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x33, 0x8a, 0x8a)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x33, 0x05, 0x05)}, \
	{ADIE_CODEC_ACTION_DELAY_WAIT, 0x7530}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x84, 0xff, 0x00)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x85, 0xff, 0x00)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x8A, 0x0f, 0x0c)}, \
	{ADIE_CODEC_ACTION_STAGE_REACHED, ADIE_CODEC_DIGITAL_ANALOG_READY}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x8a, 0x0f, 0x03)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x33, 0x40, 0x00)}, \
	{ADIE_CODEC_ACTION_STAGE_REACHED, ADIE_CODEC_ANALOG_OFF}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x33, 0x80, 0x00)}, \
	{ADIE_CODEC_ACTION_STAGE_REACHED, ADIE_CODEC_DIGITAL_OFF} }
#endif


/* ------------------------------------------------------------------
 | NAME ### "speaker_mono_tx"
 * ------------------------------------------------------------------
 | 20110413 | jhsong    | codec tx gain 0 -> 13dB (0x86)
 |          | 0x86      | 0x00 >>> 0x13
 * ------------------------------------------------------------------ */
 
/*20110413 jhsong : codec tx gain 0 -> 13dB (0x86)*/
#define SPEAKER_TX_48000_OSR_256 \
	{{ADIE_CODEC_ACTION_STAGE_REACHED, ADIE_CODEC_FLASH_IMAGE}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x80, 0x01, 0x01)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x80, 0x01, 0x00)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x8A, 0x30, 0x30)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x11, 0xfc, 0xfc)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x13, 0xfc, 0x58)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x14, 0xff, 0x65)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x15, 0xff, 0x64)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x82, 0xff, 0x5A)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x10, 0xFF, 0x68)}, \
	{ADIE_CODEC_ACTION_STAGE_REACHED, ADIE_CODEC_DIGITAL_READY}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x0D, 0xF0, 0xD0)}, \
	{ADIE_CODEC_ACTION_DELAY_WAIT, 0xbb8}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x83, 0x14, 0x14)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x8c, 0x03, 0x03)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x86, 0xff, 0x13/*0x00*/)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x8A, 0x50, 0x40)}, \
	{ADIE_CODEC_ACTION_STAGE_REACHED, ADIE_CODEC_DIGITAL_ANALOG_READY}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x8A, 0x10, 0x30)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x0D, 0xFF, 0x00)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x83, 0x14, 0x00)}, \
	{ADIE_CODEC_ACTION_STAGE_REACHED, ADIE_CODEC_ANALOG_OFF}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x11, 0xff, 0x00)}, \
	{ADIE_CODEC_ACTION_STAGE_REACHED, ADIE_CODEC_DIGITAL_OFF} }



/* ******************************************************************
 *                                                                  *
 *                        SPEAKER & HEADSET                         *
 *                                                                  *
 * ****************************************************************** */
 
/* ------------------------------------------------------------------
 | NAME ### "headset_stereo_speaker_stereo_rx"
 * ------------------------------------------------------------------
 | ### Codec RX Gain left ### (HW Tuning Table name)
 * ------------------------------------------------------------------
 | 0x84 | DIGITAL_RX_LEFT_GAIN(CDC_LRXG)
 | 7:0  | RX left channel digital gain (dB) in 2s complement;
 
 * ------------------------------------------------------------------
 | ### Codec RX Gain right ### (HW Tuning Table name)
 * ------------------------------------------------------------------
 | 0x85 | DIGITAL_RX_RIGHT_GAIN(CDC_RRXG)
 | 7:0  | Valid range is -84 to +20 dB;
 
 * ------------------------------------------------------------------ */
 
#ifdef CONFIG_PANTECH_AUDIO_FAB2200    //20110428 jhsong : feature clearup
#define HEADSET_STEREO_SPEAKER_STEREO_RX_CAPLESS_48000_OSR_256 \
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
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x37, 0xe4, 0x80/*0x82*/)/*(0x37, 0xe6, 0xa0)*/}, \
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
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x84, 0xff, 0xfb)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x85, 0xff, 0xfb)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x8A, 0x0f, 0x0c)}, \
	{ADIE_CODEC_ACTION_STAGE_REACHED, ADIE_CODEC_DIGITAL_ANALOG_READY}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x8a, 0x03, 0x03)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3b, 0xFF, 0xaC)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3C, 0xFF, 0xaC)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x34, 0xf0, 0x00)}, \
	{ADIE_CODEC_ACTION_STAGE_REACHED, ADIE_CODEC_ANALOG_OFF}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x33, 0x80, 0x00)}, \
	{ADIE_CODEC_ACTION_STAGE_REACHED, ADIE_CODEC_DIGITAL_OFF} }
#else
#define HEADSET_STEREO_SPEAKER_STEREO_RX_CAPLESS_48000_OSR_256 \
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
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x38, 0xff, 0x82)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x37, 0xe6, 0xa0)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3A, 0xFF, 0x2b)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x83, 0x23, 0x23)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x33, 0x8A, 0x8A)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x34, 0xf0, 0xa0)}, \
	{ADIE_CODEC_ACTION_DELAY_WAIT, 0x7530}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x34, 0xf0, 0xf0)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x23, 0xff, 0x20)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x33, 0x05, 0x05)}, \
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
#endif






/* ******************************************************************
 *                                                                  *
 *                        ASR                                       *
 *                                                                  *
 * ****************************************************************** */
 
/* ------------------------------------------------------------------
 | NAME ### "asr_handset_tx"
 * ------------------------------------------------------------------
 | ### MIC AMP ### (HW Tuning Table name)
 * ------------------------------------------------------------------
 | 0x0D | TXFE LEFT GAIN SETTING(Codec Tx front end control register 1)
 | 6:5  | 00 : 0dB
 |      | 01 : 4.5dB
 |      | 1x : 24dB
 * ------------------------------------------------------------------
 | ### Codec ST Gain ### (HW Tuning Table name)
 * ------------------------------------------------------------------
 | 0x8B | SIDETONE_GAIN(CDC_STG)
 | 7:0  | Sidetone gain (dB) in 2s complement; valid supported range is -84 to 0 dB.
 * ------------------------------------------------------------------
 | ### Codec Tx Gain ### (HW Tuning Table name)
 * ------------------------------------------------------------------
 | 0x86 | DIGITAL_TX_LEFT_GAIN(CDC_LTXG)
 | 7:0  | TX left channel digital gain(dB) in 2s complement; valid range is -66 to +30 dB.
 * ------------------------------------------------------------------ */
 
#ifdef CONFIG_PANTECH_ASR_PATH    //20110428 jhsong : feature clearup
#define ASR_HANDSET_TX_48000_OSR_256 \
	{{ADIE_CODEC_ACTION_STAGE_REACHED, ADIE_CODEC_FLASH_IMAGE}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x80, 0x01, 0x01)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x80, 0x01, 0x00)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x8A, 0x30, 0x30)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x11, 0xfc, 0xfc)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x13, 0xfc, 0x58)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x14, 0xff, 0x65)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x15, 0xff, 0x64)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x82, 0xff, 0x5A)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x10, 0xFF, 0x68)}, \
	{ADIE_CODEC_ACTION_STAGE_REACHED, ADIE_CODEC_DIGITAL_READY}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x0D, 0xF0, 0xd0)}, \
	{ADIE_CODEC_ACTION_DELAY_WAIT, 0xbb8}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x83, 0x14, 0x14)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x8b, 0xff, 0xE6)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x8c, 0x03, 0x02)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x86, 0xff, 0x0c)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x8A, 0x50, 0x40)}, \
	{ADIE_CODEC_ACTION_STAGE_REACHED, ADIE_CODEC_DIGITAL_ANALOG_READY}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x8A, 0x10, 0x30)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x0D, 0xFF, 0x00)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x83, 0x14, 0x00)}, \
	{ADIE_CODEC_ACTION_STAGE_REACHED, ADIE_CODEC_ANALOG_OFF}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x11, 0xff, 0x00)}, \
	{ADIE_CODEC_ACTION_STAGE_REACHED, ADIE_CODEC_DIGITAL_OFF} }
#endif


/* ------------------------------------------------------------------
 | NAME ### "asr_headset_tx"
 * ------------------------------------------------------------------
 | ### MIC AMP ### (HW Tuning Table name)
 * ------------------------------------------------------------------
 | 0x0D | TXFE LEFT GAIN SETTING(Codec Tx front end control register 1)
 | 6:5  | 00 : 0dB
 |      | 01 : 4.5dB
 |      | 1x : 24dB
 * ------------------------------------------------------------------
 | ### Codec ST Gain ### (HW Tuning Table name)
 * ------------------------------------------------------------------
 | 0x8B | SIDETONE_GAIN(CDC_STG)
 | 7:0  | Sidetone gain (dB) in 2s complement; valid supported range is -84 to 0 dB.
 * ------------------------------------------------------------------
 | ### Codec Tx Gain ### (HW Tuning Table name)
 * ------------------------------------------------------------------
 | 0x86 | DIGITAL_TX_LEFT_GAIN(CDC_LTXG)
 | 7:0  | TX left channel digital gain(dB) in 2s complement; valid range is -66 to +30 dB.
 * ------------------------------------------------------------------ */
#if defined (CONFIG_MACH_MSM8X55_J_MASAI_E)
#define ASR_HEADSET_MONO_TX_48000_OSR_256 \
	{{ADIE_CODEC_ACTION_STAGE_REACHED, ADIE_CODEC_FLASH_IMAGE}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x80, 0x01, 0x01)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x80, 0x01, 0x00)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x8A, 0x30, 0x30)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x11, 0xfc, 0xfc)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x13, 0xfc, 0x58)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x14, 0xff, 0x65)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x15, 0xff, 0x64)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x82, 0xff, 0x5A)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x10, 0xFF, 0x64)}, \
	{ADIE_CODEC_ACTION_STAGE_REACHED, ADIE_CODEC_DIGITAL_READY}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x0D, 0xFF, 0xC8)}, \
	{ADIE_CODEC_ACTION_DELAY_WAIT, 0xbb8}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x83, 0x14, 0x14)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x8b, 0xff, 0xac)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x8c, 0x03, 0x02)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x86, 0xff, 0x0e)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x8A, 0x50, 0x40)}, \
	{ADIE_CODEC_ACTION_STAGE_REACHED, ADIE_CODEC_DIGITAL_ANALOG_READY}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x8A, 0x10, 0x10)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x0D, 0xFF, 0x00)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x83, 0x14, 0x00)}, \
	{ADIE_CODEC_ACTION_STAGE_REACHED, ADIE_CODEC_ANALOG_OFF}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x11, 0xff, 0x00)}, \
	{ADIE_CODEC_ACTION_STAGE_REACHED, ADIE_CODEC_DIGITAL_OFF} }
#endif


#ifdef CONFIG_PANTECH_TEST_CODE_PATH  //20110711 jhsong
#define TESTCODE_HANDSET_RX_48000_OSR_256\
	{{ADIE_CODEC_ACTION_STAGE_REACHED, ADIE_CODEC_FLASH_IMAGE}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x80, 0x02, 0x02)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x80, 0x02, 0x00)}, \
	{ADIE_CODEC_ACTION_STAGE_REACHED, ADIE_CODEC_DIGITAL_READY}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x24, 0x6F, 0x44)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x04, 0xfF, 0x8C)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x81, 0xFF, /*0x4e*/0x0e)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x25, 0x0F, 0x0b)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x26, 0xfc, 0xfc)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x36, 0xc0, 0x80)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3A, 0xFF, 0x2B)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x23, 0xff, 0x20)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3d, 0xFF, 0x55)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x83, 0x21, 0x21)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x33, 0x80, 0x80)}, \
	{ADIE_CODEC_ACTION_DELAY_WAIT, 0x2710}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x33, 0x40, 0x40)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x84, 0xff, 0x0f)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x8A, 0x05, 0x04)}, \
	{ADIE_CODEC_ACTION_STAGE_REACHED, ADIE_CODEC_DIGITAL_ANALOG_READY}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x8a, 0x01, 0x01)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x36, 0xc0, 0x00)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x33, 0x40, 0x00)}, \
	{ADIE_CODEC_ACTION_STAGE_REACHED, ADIE_CODEC_ANALOG_OFF}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x33, 0x80, 0x00)}, \
	{ADIE_CODEC_ACTION_STAGE_REACHED, ADIE_CODEC_DIGITAL_OFF} }


#define TESTCODE_HANDSET_TX_48000_OSR_256 \
	{{ADIE_CODEC_ACTION_STAGE_REACHED, ADIE_CODEC_FLASH_IMAGE}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x80, 0x01, 0x01)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x80, 0x01, 0x00)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x8A, 0x30, 0x30)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x11, 0xfc, 0xfc)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x13, 0xfc, 0x58)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x14, 0xff, 0x65)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x15, 0xff, 0x64)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x82, 0xff, 0x5A)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x10, 0xFF, 0x68)}, \
	{ADIE_CODEC_ACTION_STAGE_REACHED, ADIE_CODEC_DIGITAL_READY}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x0D, 0xF0, 0xd0)}, \
	{ADIE_CODEC_ACTION_DELAY_WAIT, 0xbb8}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x83, 0x14, 0x14)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x8b, 0xff, /*0xE6*/0xac)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x8c, 0x03, 0x02)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x86, 0xff, 0x00)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x8A, 0x50, 0x40)}, \
	{ADIE_CODEC_ACTION_STAGE_REACHED, ADIE_CODEC_DIGITAL_ANALOG_READY}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x8A, 0x10, 0x30)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x0D, 0xFF, 0x00)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x83, 0x14, 0x00)}, \
	{ADIE_CODEC_ACTION_STAGE_REACHED, ADIE_CODEC_ANALOG_OFF}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x11, 0xff, 0x00)}, \
	{ADIE_CODEC_ACTION_STAGE_REACHED, ADIE_CODEC_DIGITAL_OFF} }

#endif

/***************************************************************************\
				Handset
\***************************************************************************/


#define HANDSET_RX_8000_OSR_256 \
	{{ADIE_CODEC_ACTION_STAGE_REACHED, ADIE_CODEC_FLASH_IMAGE},  \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x80,  0x02,  0x02)},  \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x80, 0x02, 0x00)}, \
	{ADIE_CODEC_ACTION_STAGE_REACHED, ADIE_CODEC_DIGITAL_READY}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x24, 0x6F, 0x44)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x04, 0xff, 0x8C)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x81, 0xFF, 0x4E)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x25, 0x0F, 0x0b)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x26, 0xfc, 0xfc)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x36, 0xc0, 0x80)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3A, 0xFF, 0x2B)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x23, 0xff, 0x20)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3d, 0xFF, 0xD5)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x83, 0x21, 0x21)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x33, 0x80, 0x80)}, \
	{ADIE_CODEC_ACTION_DELAY_WAIT, 0x2710}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x33, 0x40, 0x40)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x84, 0xff, 0x00)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x8A, 0x05, 0x04)}, \
	{ADIE_CODEC_ACTION_STAGE_REACHED, ADIE_CODEC_DIGITAL_ANALOG_READY}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x8a, 0x01, 0x01)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x36, 0xc0, 0x00)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x33, 0x40, 0x00)}, \
	{ADIE_CODEC_ACTION_STAGE_REACHED, ADIE_CODEC_ANALOG_OFF}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x33, 0x80, 0x00)}, \
	{ADIE_CODEC_ACTION_STAGE_REACHED, ADIE_CODEC_DIGITAL_OFF} }

#define HANDSET_RX_16000_OSR_256 HANDSET_RX_8000_OSR_256

#define HANDSET_RX_48000_OSR_64\
	{{ADIE_CODEC_ACTION_STAGE_REACHED, ADIE_CODEC_FLASH_IMAGE}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x80, 0x02, 0x02)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x80, 0x02, 0x00)}, \
	{ADIE_CODEC_ACTION_STAGE_REACHED, ADIE_CODEC_DIGITAL_READY}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x24, 0x6F, 0x47)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x04, 0xfF, 0x8C)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x81, 0xFF, 0x42)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x25, 0x0F, 0x0b)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x26, 0xfc, 0xfc)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x36, 0xc0, 0x80)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3A, 0xFF, 0x2B)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x23, 0xff, 0x20)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3d, 0xFF, 0xD5)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x83, 0x21, 0x21)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x33, 0x80, 0x80)}, \
	{ADIE_CODEC_ACTION_DELAY_WAIT, 0x2710}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x33, 0x40, 0x40)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x84, 0xff, 0x00)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x8A, 0x05, 0x04)}, \
	{ADIE_CODEC_ACTION_STAGE_REACHED, ADIE_CODEC_DIGITAL_ANALOG_READY}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x8a, 0x01, 0x01)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x36, 0xc0, 0x00)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x33, 0x40, 0x00)}, \
	{ADIE_CODEC_ACTION_STAGE_REACHED, ADIE_CODEC_ANALOG_OFF}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x33, 0x80, 0x00)}, \
	{ADIE_CODEC_ACTION_STAGE_REACHED, ADIE_CODEC_DIGITAL_OFF} }


#define HANDSET_TX_16000_OSR_256 HANDSET_TX_8000_OSR_256

#define HANDSET_TX_8000_OSR_256 \
	{{ADIE_CODEC_ACTION_STAGE_REACHED, ADIE_CODEC_FLASH_IMAGE}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x80, 0x01, 0x01)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x80, 0x01, 0x00)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x8A, 0x30, 0x30)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x11, 0xfc, 0xfc)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x13, 0xfc, 0x58)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x14, 0xff, 0x65)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x15, 0xff, 0x64)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x82, 0xff, 0x5E)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x10, 0xFF, 0x68)}, \
	{ADIE_CODEC_ACTION_STAGE_REACHED, ADIE_CODEC_DIGITAL_READY}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x0D, 0xF0, 0xd0)}, \
	{ADIE_CODEC_ACTION_DELAY_WAIT, 0xbb8}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x83, 0x14, 0x14)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x8b, 0xff, 0xE6)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x8c, 0x03, 0x02)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x86, 0xff, 0x00)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x8A, 0x50, 0x40)}, \
	{ADIE_CODEC_ACTION_STAGE_REACHED, ADIE_CODEC_DIGITAL_ANALOG_READY}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x8A, 0x10, 0x30)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x0D, 0xFF, 0x00)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x83, 0x14, 0x00)}, \
	{ADIE_CODEC_ACTION_STAGE_REACHED, ADIE_CODEC_ANALOG_OFF}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x11, 0xff, 0x00)}, \
	{ADIE_CODEC_ACTION_STAGE_REACHED, ADIE_CODEC_DIGITAL_OFF} }

#define HANDSET_TX_16000_OSR_256 HANDSET_TX_8000_OSR_256



/***************************************************************************\
				Headset
\***************************************************************************/



#define HEADSET_STEREO_TX_8000_OSR_256\
	{{ADIE_CODEC_ACTION_STAGE_REACHED, ADIE_CODEC_FLASH_IMAGE}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x80, 0x01, 0x01)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x80, 0x01, 0x00)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x8A, 0x30, 0x30)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x11, 0xfc, 0xfc)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x12, 0xfc, 0xfc)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x13, 0xfc, 0x58)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x14, 0xfd, 0x65)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x15, 0xff, 0x64)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x82, 0xFF, 0x5E)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x10, 0xFF, 0x64)}, \
	{ADIE_CODEC_ACTION_STAGE_REACHED, ADIE_CODEC_DIGITAL_READY}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x0E, 0xE8, 0xE8)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x0D, 0xF0, 0xD0)}, \
	{ADIE_CODEC_ACTION_DELAY_WAIT, 0xbb8}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x83, 0x1c, 0x1c)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x8b, 0xff, 0xE7)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x8c, 0x03, 0x02)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x87, 0xff, 0x00)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x86, 0xff, 0x00)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x8A, 0xF0, 0xF0)}, \
	{ADIE_CODEC_ACTION_STAGE_REACHED, ADIE_CODEC_DIGITAL_ANALOG_READY}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x8A, 0x30, 0x30)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x0e, 0xFF, 0x00)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x0d, 0xFF, 0x00)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x83, 0x1c, 0x00)}, \
	{ADIE_CODEC_ACTION_STAGE_REACHED, ADIE_CODEC_ANALOG_OFF}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x12, 0xff, 0x00)}, \
	{ADIE_CODEC_ACTION_STAGE_REACHED, ADIE_CODEC_DIGITAL_OFF} }

#define HEADSET_STEREO_TX_16000_OSR_256 HEADSET_STEREO_TX_8000_OSR_256

#define HEADSET_STEREO_TX_48000_OSR_64\
	{{ADIE_CODEC_ACTION_STAGE_REACHED, ADIE_CODEC_FLASH_IMAGE}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x80, 0x01, 0x01)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x80, 0x01, 0x00)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x8A, 0x30, 0x30)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x11, 0xfc, 0xfc)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x12, 0xfc, 0xfc)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x13, 0xfc, 0x58)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x14, 0xfd, 0x65)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x15, 0xfc, 0x64)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x82, 0xFF, 0x46)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x10, 0xFF, 0x64)}, \
	{ADIE_CODEC_ACTION_STAGE_REACHED, ADIE_CODEC_DIGITAL_READY}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x0E, 0xE8, 0xE8)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x0D, 0xF0, 0xD0)}, \
	{ADIE_CODEC_ACTION_DELAY_WAIT, 0xbb8}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x83, 0x1c, 0x1c)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x8b, 0xff, 0xE7)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x8c, 0x03, 0x02)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x87, 0xff, 0x00)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x86, 0xff, 0x00)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x8A, 0xF0, 0xF0)}, \
	{ADIE_CODEC_ACTION_STAGE_REACHED, ADIE_CODEC_DIGITAL_ANALOG_READY}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x8A, 0x30, 0x30)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x0e, 0xFF, 0x00)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x0d, 0xFF, 0x00)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x83, 0x1c, 0x00)}, \
	{ADIE_CODEC_ACTION_STAGE_REACHED, ADIE_CODEC_ANALOG_OFF}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x12, 0xff, 0x00)}, \
	{ADIE_CODEC_ACTION_STAGE_REACHED, ADIE_CODEC_DIGITAL_OFF} }

#define HEADSET_STEREO_TX_48000_OSR_256\
	{{ADIE_CODEC_ACTION_STAGE_REACHED, ADIE_CODEC_FALSH_IMAGE}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x80, 0x01, 0x01)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x80, 0x01, 0x00)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x8A, 0x30, 0x30)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x11, 0xfc, 0xfc)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x12, 0xfc, 0xfc)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x13, 0xfc, 0x58)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x14, 0xfd, 0x65)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x15, 0xfc, 0x64)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x82, 0xff, 0x5A)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x10, 0xFF, 0x64)}, \
	{ADIE_CODEC_ACTION_STAGE_REACHED, ADIE_CODEC_DIGITAL_READY}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x0E, 0xE8, 0xE8)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x0D, 0xF0, 0xD0)}, \
	{ADIE_CODEC_ACTION_DELAY_WAIT, 0xbb8}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x83, 0x1c, 0x1c)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x8b, 0xff, 0xE7)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x8c, 0x03, 0x02)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x87, 0xff, 0x00)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x86, 0xff, 0x00)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x8A, 0xF0, 0xF0)}, \
	{ADIE_CODEC_ACTION_STAGE_REACHED, ADIE_CODEC_DIGITAL_ANALOG_READY}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x8A, 0x30, 0x30)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x0e, 0xFF, 0x00)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x0d, 0xFF, 0x00)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x83, 0x1c, 0x00)}, \
	{ADIE_CODEC_ACTION_STAGE_REACHED, ADIE_CODEC_ANALOG_OFF}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x12, 0xff, 0x00)}, \
	{ADIE_CODEC_ACTION_STAGE_REACHED, ADIE_CODEC_DIGITAL_OFF} }

#define HEADSET_MONO_TX_16000_OSR_256\
	{{ADIE_CODEC_ACTION_STAGE_REACHED,  ADIE_CODEC_FLASH_IMAGE}, \
	{ADIE_CODEC_ACTION_ENTRY,  ADIE_CODEC_PACK_ENTRY(0x80, 0x01, 0x01)}, \
	{ADIE_CODEC_ACTION_ENTRY,  ADIE_CODEC_PACK_ENTRY(0x80, 0x01, 0x00)}, \
	{ADIE_CODEC_ACTION_ENTRY,  ADIE_CODEC_PACK_ENTRY(0x8A, 0x30, 0x30)}, \
	{ADIE_CODEC_ACTION_ENTRY,  ADIE_CODEC_PACK_ENTRY(0x11, 0xfc, 0xfc)}, \
	{ADIE_CODEC_ACTION_ENTRY,  ADIE_CODEC_PACK_ENTRY(0x13, 0xfc, 0x58)}, \
	{ADIE_CODEC_ACTION_ENTRY,  ADIE_CODEC_PACK_ENTRY(0x14, 0xff, 0x65)}, \
	{ADIE_CODEC_ACTION_ENTRY,  ADIE_CODEC_PACK_ENTRY(0x15, 0xfc, 0x64)}, \
	{ADIE_CODEC_ACTION_ENTRY,  ADIE_CODEC_PACK_ENTRY(0x82, 0xFF, 0x5E)}, \
	{ADIE_CODEC_ACTION_ENTRY,  ADIE_CODEC_PACK_ENTRY(0x10, 0xFF, 0x64)}, \
	{ADIE_CODEC_ACTION_STAGE_REACHED,  ADIE_CODEC_DIGITAL_READY}, \
	{ADIE_CODEC_ACTION_ENTRY,  ADIE_CODEC_PACK_ENTRY(0x0D, 0xFf, 0xc8)}, \
	{ADIE_CODEC_ACTION_DELAY_WAIT,  0xbb8}, \
	{ADIE_CODEC_ACTION_ENTRY,  ADIE_CODEC_PACK_ENTRY(0x83, 0x14, 0x14)}, \
	{ADIE_CODEC_ACTION_ENTRY,  ADIE_CODEC_PACK_ENTRY(0x8b, 0xff, 0xE7)}, \
	{ADIE_CODEC_ACTION_ENTRY,  ADIE_CODEC_PACK_ENTRY(0x8c, 0x03, 0x02)}, \
	{ADIE_CODEC_ACTION_ENTRY,  ADIE_CODEC_PACK_ENTRY(0x86, 0xff, 0x00)}, \
	{ADIE_CODEC_ACTION_ENTRY,  ADIE_CODEC_PACK_ENTRY(0x8A, 0x50, 0x40)}, \
	{ADIE_CODEC_ACTION_STAGE_REACHED,  ADIE_CODEC_DIGITAL_ANALOG_READY}, \
	{ADIE_CODEC_ACTION_ENTRY,  ADIE_CODEC_PACK_ENTRY(0x8A, 0x10, 0x10)}, \
	{ADIE_CODEC_ACTION_ENTRY,  ADIE_CODEC_PACK_ENTRY(0x0D, 0xFF, 0x00)}, \
	{ADIE_CODEC_ACTION_ENTRY,  ADIE_CODEC_PACK_ENTRY(0x83, 0x14, 0x00)}, \
	{ADIE_CODEC_ACTION_STAGE_REACHED,  ADIE_CODEC_ANALOG_OFF}, \
	{ADIE_CODEC_ACTION_ENTRY,  ADIE_CODEC_PACK_ENTRY(0x11, 0xff, 0x00)}, \
	{ADIE_CODEC_ACTION_STAGE_REACHED,  ADIE_CODEC_DIGITAL_OFF} }

#define HEADSET_MONO_TX_8000_OSR_256 HEADSET_MONO_TX_16000_OSR_256


#ifdef CONFIG_PANTECH_MASAI_E_SEPERATE_CALL_MEDIA_PATH  //20110420 jhsong : seperate call and media path
#define SPEAKER_VOICE_STEREO_RX_48000_OSR_256 \
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
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x84, 0xff, 0x00)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x85, 0xff, 0x00)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x8A, 0x0f, 0x0c/*, 0x05, 0x04*/)}, \
	{ADIE_CODEC_ACTION_STAGE_REACHED, ADIE_CODEC_DIGITAL_ANALOG_READY}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x8a, 0x03, 0x03/*, 0x01, 0x01*/)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x33, 0x40, 0x00)}, \
	{ADIE_CODEC_ACTION_STAGE_REACHED, ADIE_CODEC_ANALOG_OFF}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x33, 0x80, 0x00)}, \
	{ADIE_CODEC_ACTION_STAGE_REACHED, ADIE_CODEC_DIGITAL_OFF} }

#define SPEAKER_VOICE_TX_48000_OSR_256 \
	{{ADIE_CODEC_ACTION_STAGE_REACHED, ADIE_CODEC_FLASH_IMAGE}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x80, 0x01, 0x01)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x80, 0x01, 0x00)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x8A, 0x30, 0x30)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x11, 0xfc, 0xfc)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x13, 0xfc, 0x58)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x14, 0xff, 0x65)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x15, 0xff, 0x64)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x82, 0xff, 0x5A)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x10, 0xFF, 0x68)}, \
	{ADIE_CODEC_ACTION_STAGE_REACHED, ADIE_CODEC_DIGITAL_READY}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x0D, 0xF0, 0xD0)}, \
	{ADIE_CODEC_ACTION_DELAY_WAIT, 0xbb8}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x83, 0x14, 0x14)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x8c, 0x03, 0x03)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x86, 0xff, 0x00)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x8A, 0x50, 0x40)}, \
	{ADIE_CODEC_ACTION_STAGE_REACHED, ADIE_CODEC_DIGITAL_ANALOG_READY}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x8A, 0x10, 0x30)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x0D, 0xFF, 0x00)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x83, 0x14, 0x00)}, \
	{ADIE_CODEC_ACTION_STAGE_REACHED, ADIE_CODEC_ANALOG_OFF}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x11, 0xff, 0x00)}, \
	{ADIE_CODEC_ACTION_STAGE_REACHED, ADIE_CODEC_DIGITAL_OFF} }
#endif



#define HEADSET_RX_CAPLESS_8000_OSR_256\
	{{ADIE_CODEC_ACTION_STAGE_REACHED, ADIE_CODEC_FLASH_IMAGE}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x80, 0x02, 0x02)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x80, 0x02, 0x00)}, \
	{ADIE_CODEC_ACTION_STAGE_REACHED, ADIE_CODEC_DIGITAL_READY}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x81, 0xFF, 0x4E)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x04, 0xff, 0xBC)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x24, 0x6F, 0x64)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x25, 0x0F, 0x0b)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x26, 0xfc, 0xfc)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x38, 0xff, 0xa2)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3A, 0xFF, 0xeb)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x83, 0x23, 0x23)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x33, 0x80, 0x80)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x34, 0xf0, 0xf0)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x23, 0xff, 0x20)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3B, 0xFF, 0x04)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3C, 0xFF, 0x04)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x84, 0xff, 0x00)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x85, 0xff, 0x00)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x8A, 0x0f, 0x0c)}, \
	{ADIE_CODEC_ACTION_STAGE_REACHED, ADIE_CODEC_DIGITAL_ANALOG_READY}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x8a, 0x03, 0x03)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3b, 0xFF, 0xAC)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3C, 0xFF, 0xAC)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x34, 0xf0, 0x00)}, \
	{ADIE_CODEC_ACTION_STAGE_REACHED, ADIE_CODEC_ANALOG_OFF}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x33, 0x80, 0x00)}, \
	{ADIE_CODEC_ACTION_STAGE_REACHED, ADIE_CODEC_DIGITAL_OFF} }

#define HEADSET_RX_CAPLESS_48000_OSR_64 \
	{{ADIE_CODEC_ACTION_STAGE_REACHED, ADIE_CODEC_FLASH_IMAGE}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x80, 0x02, 0x02)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x80, 0x02, 0x00)}, \
	{ADIE_CODEC_ACTION_STAGE_REACHED, ADIE_CODEC_DIGITAL_READY}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x81, 0xFF, 0x42)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x04, 0xff, 0xBC)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x24, 0x6F, 0x67)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x25, 0x0F, 0x0b)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x26, 0xfc, 0xfc)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x38, 0xff, 0xa2)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3A, 0xFF, 0xab)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x83, 0x23, 0x23)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x33, 0x80, 0x80)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x34, 0xf0, 0xf0)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x23, 0xff, 0x20)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3B, 0xFF, 0x04)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3c, 0xFF, 0x04)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x84, 0xff, 0x00)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x85, 0xff, 0x00)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x8A, 0x0f, 0x0c)}, \
	{ADIE_CODEC_ACTION_STAGE_REACHED, ADIE_CODEC_DIGITAL_ANALOG_READY}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x8a, 0x03, 0x03)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3b, 0xFF, 0xAC)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3c, 0xFF, 0xAC)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x34, 0xf0, 0x00)}, \
	{ADIE_CODEC_ACTION_STAGE_REACHED, ADIE_CODEC_ANALOG_OFF}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x33, 0x80, 0x00)}, \
	{ADIE_CODEC_ACTION_STAGE_REACHED, ADIE_CODEC_DIGITAL_OFF} }

#define HEADSET_RX_CAPLESS_48000_OSR_256 \
	{{ADIE_CODEC_ACTION_STAGE_REACHED, ADIE_CODEC_FLASH_IMAGE}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x80, 0x02, 0x02)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x80, 0x02, 0x00)}, \
	{ADIE_CODEC_ACTION_STAGE_REACHED, ADIE_CODEC_DIGITAL_READY}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x81, 0xFF, 0x4e)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x04, 0xff, 0xBC)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x24, 0x6F, 0x64)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x25, 0x0F, 0x0B)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x26, 0xfc, 0xfc)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x38, 0xff, 0xa2)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3A, 0xFF, 0xab)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x83, 0x23, 0x23)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x33, 0x80, 0x80)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x34, 0xf0, 0xf0)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x23, 0xff, 0x20)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3B, 0xFF, 0x04)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3c, 0xFF, 0x04)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x84, 0xff, 0x00)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x85, 0xff, 0x00)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x8A, 0x0f, 0x0c)}, \
	{ADIE_CODEC_ACTION_STAGE_REACHED, ADIE_CODEC_DIGITAL_ANALOG_READY}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x8a, 0x03, 0x03)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3b, 0xFF, 0xAC)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3c, 0xFF, 0xAC)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x34, 0xf0, 0x00)}, \
	{ADIE_CODEC_ACTION_STAGE_REACHED, ADIE_CODEC_ANALOG_OFF}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x33, 0x80, 0x00)}, \
	{ADIE_CODEC_ACTION_STAGE_REACHED, ADIE_CODEC_DIGITAL_OFF} }


#define HEADSET_RX_LEGACY_8000_OSR_256 \
	{{ADIE_CODEC_ACTION_STAGE_REACHED, ADIE_CODEC_FLASH_IMAGE}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x80, 0x02, 0x02)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x80, 0x02, 0x00)}, \
	{ADIE_CODEC_ACTION_STAGE_REACHED, ADIE_CODEC_DIGITAL_READY}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3B, 0xFF, 0xac)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3C, 0xFF, 0xac)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x81, 0xFF, 0x4E)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x04, 0xff, 0xBC)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x24, 0x6F, 0x64)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x25, 0x0F, 0x0b)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x26, 0xfc, 0xfc)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x38, 0xff, 0xa0)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3A, 0xFF, 0x2b)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x83, 0x23, 0x23)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x33, 0x80, 0x80)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x34, 0xf0, 0xa0)}, \
	{ADIE_CODEC_ACTION_DELAY_WAIT, 0x186A0}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x34, 0xf0, 0xf0)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x23, 0xff, 0x20)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3B, 0xFF, 0x98)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3C, 0xFF, 0x98)}, \
	{ADIE_CODEC_ACTION_DELAY_WAIT, 0x3E8}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3B, 0xFF, 0x88)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3C, 0xFF, 0x88)}, \
	{ADIE_CODEC_ACTION_DELAY_WAIT, 0x3E8}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3B, 0xFF, 0x78)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3C, 0xFF, 0x78)}, \
	{ADIE_CODEC_ACTION_DELAY_WAIT, 0x3E8}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3B, 0xFF, 0x68)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3C, 0xFF, 0x68)}, \
	{ADIE_CODEC_ACTION_DELAY_WAIT, 0x3E8}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3B, 0xFF, 0x58)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3C, 0xFF, 0x58)}, \
	{ADIE_CODEC_ACTION_DELAY_WAIT, 0x3E8}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3B, 0xFF, 0x48)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3C, 0xFF, 0x48)}, \
	{ADIE_CODEC_ACTION_DELAY_WAIT, 0x3E8}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3B, 0xFF, 0x38)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3C, 0xFF, 0x38)}, \
	{ADIE_CODEC_ACTION_DELAY_WAIT, 0x3E8}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3B, 0xFF, 0x28)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3C, 0xFF, 0x28)}, \
	{ADIE_CODEC_ACTION_DELAY_WAIT, 0x3E8}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3B, 0xFF, 0x18)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3C, 0xFF, 0x18)}, \
	{ADIE_CODEC_ACTION_DELAY_WAIT, 0x3E8}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3B, 0xFF, 0x10)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3C, 0xFF, 0x10)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x84, 0xff, 0x00)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x85, 0xff, 0x00)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x8A, 0x0f, 0x0c)}, \
	{ADIE_CODEC_ACTION_STAGE_REACHED, ADIE_CODEC_DIGITAL_ANALOG_READY}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x8a, 0x03, 0x03)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3b, 0xFF, 0xAC)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3C, 0xFF, 0xAC)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x34, 0xf0, 0x00)}, \
	{ADIE_CODEC_ACTION_STAGE_REACHED, ADIE_CODEC_ANALOG_OFF}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x33, 0x80, 0x00)}, \
	{ADIE_CODEC_ACTION_STAGE_REACHED, ADIE_CODEC_DIGITAL_OFF} }

#define HEADSET_RX_LEGACY_16000_OSR_256 HEADSET_RX_LEGACY_8000_OSR_256

#define HEADSET_RX_LEGACY_48000_OSR_64 \
	{{ADIE_CODEC_ACTION_STAGE_REACHED, ADIE_CODEC_FLASH_IMAGE}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x80, 0x02, 0x02)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x80, 0x02, 0x00)}, \
	{ADIE_CODEC_ACTION_STAGE_REACHED, ADIE_CODEC_DIGITAL_READY}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3B, 0xFF, 0xac)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3C, 0xFF, 0xac)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x81, 0xFF, 0x42)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x04, 0xff, 0xBC)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x24, 0x6F, 0x67)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x25, 0x0F, 0x0b)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x26, 0xfc, 0xfc)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x38, 0xff, 0xa0)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3A, 0xFF, 0x2b)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x83, 0x23, 0x23)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x33, 0x80, 0x80)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x34, 0xf0, 0xa0)}, \
	{ADIE_CODEC_ACTION_DELAY_WAIT, 0x186A0}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x34, 0xf0, 0xf0)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x23, 0xff, 0x20)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3B, 0xFF, 0x98)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3C, 0xFF, 0x98)}, \
	{ADIE_CODEC_ACTION_DELAY_WAIT, 0x3E8}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3B, 0xFF, 0x88)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3C, 0xFF, 0x88)}, \
	{ADIE_CODEC_ACTION_DELAY_WAIT, 0x3E8}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3B, 0xFF, 0x78)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3C, 0xFF, 0x78)}, \
	{ADIE_CODEC_ACTION_DELAY_WAIT, 0x3E8}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3B, 0xFF, 0x68)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3C, 0xFF, 0x68)}, \
	{ADIE_CODEC_ACTION_DELAY_WAIT, 0x3E8}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3B, 0xFF, 0x58)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3C, 0xFF, 0x58)}, \
	{ADIE_CODEC_ACTION_DELAY_WAIT, 0x3E8}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3B, 0xFF, 0x48)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3C, 0xFF, 0x48)}, \
	{ADIE_CODEC_ACTION_DELAY_WAIT, 0x3E8}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3B, 0xFF, 0x38)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3C, 0xFF, 0x38)}, \
	{ADIE_CODEC_ACTION_DELAY_WAIT, 0x3E8}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3B, 0xFF, 0x28)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3C, 0xFF, 0x28)}, \
	{ADIE_CODEC_ACTION_DELAY_WAIT, 0x3E8}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3B, 0xFF, 0x18)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3C, 0xFF, 0x18)}, \
	{ADIE_CODEC_ACTION_DELAY_WAIT, 0x3E8}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3B, 0xFF, 0x10)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3C, 0xFF, 0x10)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x84, 0xff, 0x00)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x85, 0xff, 0x00)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x8A, 0x0f, 0x0c)}, \
	{ADIE_CODEC_ACTION_STAGE_REACHED, ADIE_CODEC_DIGITAL_ANALOG_READY}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x8a, 0x03, 0x03)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3b, 0xFF, 0xAC)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3c, 0xFF, 0xAC)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x34, 0xf0, 0x00)}, \
	{ADIE_CODEC_ACTION_STAGE_REACHED, ADIE_CODEC_ANALOG_OFF}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x33, 0x80, 0x00)}, \
	{ADIE_CODEC_ACTION_STAGE_REACHED, ADIE_CODEC_DIGITAL_OFF} }


#define HEADSET_RX_LEGACY_48000_OSR_256 \
	{{ADIE_CODEC_ACTION_STAGE_REACHED, ADIE_CODEC_FLASH_IMAGE}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x80, 0x02, 0x02)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x80, 0x02, 0x00)}, \
	{ADIE_CODEC_ACTION_STAGE_REACHED, ADIE_CODEC_DIGITAL_READY}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3B, 0xFF, 0xac)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3C, 0xFF, 0xac)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x81, 0xFF, 0x4e)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x04, 0xff, 0xBC)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x24, 0x6F, 0x64)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x25, 0x0F, 0x0B)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x26, 0xfc, 0xfc)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x38, 0xff, 0xa0)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3A, 0xFF, 0x2b)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x83, 0x23, 0x23)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x33, 0x80, 0x80)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x34, 0xf0, 0xa0)}, \
	{ADIE_CODEC_ACTION_DELAY_WAIT, 0x186A0}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x34, 0xf0, 0xf0)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x23, 0xff, 0x20)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3B, 0xFF, 0x98)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3C, 0xFF, 0x98)}, \
	{ADIE_CODEC_ACTION_DELAY_WAIT, 0x3E8}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3B, 0xFF, 0x88)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3C, 0xFF, 0x88)}, \
	{ADIE_CODEC_ACTION_DELAY_WAIT, 0x3E8}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3B, 0xFF, 0x78)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3C, 0xFF, 0x78)}, \
	{ADIE_CODEC_ACTION_DELAY_WAIT, 0x3E8}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3B, 0xFF, 0x68)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3C, 0xFF, 0x68)}, \
	{ADIE_CODEC_ACTION_DELAY_WAIT, 0x3E8}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3B, 0xFF, 0x58)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3C, 0xFF, 0x58)}, \
	{ADIE_CODEC_ACTION_DELAY_WAIT, 0x3E8}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3B, 0xFF, 0x48)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3C, 0xFF, 0x48)}, \
	{ADIE_CODEC_ACTION_DELAY_WAIT, 0x3E8}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3B, 0xFF, 0x38)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3C, 0xFF, 0x38)}, \
	{ADIE_CODEC_ACTION_DELAY_WAIT, 0x3E8}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3B, 0xFF, 0x28)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3C, 0xFF, 0x28)}, \
	{ADIE_CODEC_ACTION_DELAY_WAIT, 0x3E8}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3B, 0xFF, 0x18)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3C, 0xFF, 0x18)}, \
	{ADIE_CODEC_ACTION_DELAY_WAIT, 0x3E8}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3B, 0xFF, 0x10)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3C, 0xFF, 0x10)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x84, 0xff, 0x00)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x85, 0xff, 0x00)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x8A, 0x0f, 0x0c)}, \
	{ADIE_CODEC_ACTION_STAGE_REACHED, ADIE_CODEC_DIGITAL_ANALOG_READY}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x8a, 0x03, 0x03)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3b, 0xFF, 0xAC)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3c, 0xFF, 0xAC)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x34, 0xf0, 0x00)}, \
	{ADIE_CODEC_ACTION_STAGE_REACHED, ADIE_CODEC_ANALOG_OFF}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x33, 0x80, 0x00)}, \
	{ADIE_CODEC_ACTION_STAGE_REACHED, ADIE_CODEC_DIGITAL_OFF} }

#define HEADSET_RX_CLASS_D_LEGACY_8000_OSR_256 \
	{{ADIE_CODEC_ACTION_STAGE_REACHED, ADIE_CODEC_FLASH_IMAGE}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3B, 0xFF, 0xac)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3C, 0xFF, 0xac)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x80, 0x02, 0x02)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x80, 0x02, 0x00)}, \
	{ADIE_CODEC_ACTION_STAGE_REACHED, ADIE_CODEC_DIGITAL_READY}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x81, 0xFF, 0x0e)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x23, 0xf8, 0xA8)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x28, 0xfF, 0xCA)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x29, 0xfF, 0xCA)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x39, 0x50, 0x50)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x40, 0xFF, 0x08)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x41, 0xFF, 0x08)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x42, 0xFF, 0xFF)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x43, 0xFF, 0xc4)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x44, 0xFF, 0x37)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3E, 0xFF, 0xDD)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3F, 0xFF, 0x0f)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x45, 0xFF, 0xff)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x46, 0xFF, 0x77)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x47, 0xFF, 0xc4)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x48, 0xFF, 0x37)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x49, 0xFF, 0xff)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x4a, 0xFF, 0x77)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x83, 0x23, 0x23)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x33, 0x80, 0x80)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x34, 0x0a, 0x0a)}, \
	{ADIE_CODEC_ACTION_DELAY_WAIT, 0x186A0}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x34, 0x05, 0x05)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3B, 0xFF, 0x98)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3C, 0xFF, 0x98)}, \
	{ADIE_CODEC_ACTION_DELAY_WAIT, 0x3E8}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3B, 0xFF, 0x88)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3C, 0xFF, 0x88)}, \
	{ADIE_CODEC_ACTION_DELAY_WAIT, 0x3E8}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3B, 0xFF, 0x78)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3C, 0xFF, 0x78)}, \
	{ADIE_CODEC_ACTION_DELAY_WAIT, 0x3E8}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3B, 0xFF, 0x68)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3C, 0xFF, 0x68)}, \
	{ADIE_CODEC_ACTION_DELAY_WAIT, 0x3E8}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3B, 0xFF, 0x58)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3C, 0xFF, 0x58)}, \
	{ADIE_CODEC_ACTION_DELAY_WAIT, 0x3E8}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3B, 0xFF, 0x48)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3C, 0xFF, 0x48)}, \
	{ADIE_CODEC_ACTION_DELAY_WAIT, 0x3E8}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3B, 0xFF, 0x38)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3C, 0xFF, 0x38)}, \
	{ADIE_CODEC_ACTION_DELAY_WAIT, 0x3E8}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3B, 0xFF, 0x28)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3C, 0xFF, 0x28)}, \
	{ADIE_CODEC_ACTION_DELAY_WAIT, 0x3E8}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3B, 0xFF, 0x18)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3C, 0xFF, 0x18)}, \
	{ADIE_CODEC_ACTION_DELAY_WAIT, 0x3E8}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3B, 0xFF, 0x08)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3C, 0xFF, 0x08)}, \
	{ADIE_CODEC_ACTION_DELAY_WAIT, 0x3E8}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3B, 0xFF, 0x04)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3C, 0xFF, 0x04)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x84, 0xff, 0x00)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x85, 0xff, 0x00)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x8A, 0x0f, 0x0c)}, \
	{ADIE_CODEC_ACTION_STAGE_REACHED, ADIE_CODEC_DIGITAL_ANALOG_READY}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x8a, 0x03, 0x03)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3b, 0xFC, 0xAC)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3c, 0xFC, 0xAC)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x34, 0x0f, 0x00)}, \
	{ADIE_CODEC_ACTION_STAGE_REACHED, ADIE_CODEC_ANALOG_OFF}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x33, 0x80, 0x00)}, \
	{ADIE_CODEC_ACTION_STAGE_REACHED, ADIE_CODEC_DIGITAL_OFF} }


#define HEADSET_RX_CLASS_D_LEGACY_11025_OSR_256 \
	{{ADIE_CODEC_ACTION_STAGE_REACHED, ADIE_CODEC_FLASH_IMAGE}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3B, 0xFF, 0xac)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3C, 0xFF, 0xac)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x80, 0x02, 0x02)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x80, 0x02, 0x00)}, \
	{ADIE_CODEC_ACTION_STAGE_REACHED, ADIE_CODEC_DIGITAL_READY}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x81, 0xFF, 0x0e)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x23, 0xf8, 0xA8)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x28, 0xfF, 0xCA)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x29, 0xfF, 0xCA)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x39, 0x50, 0x50)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x40, 0xFF, 0x08)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x41, 0xFF, 0x08)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x42, 0xFF, 0xbb)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x43, 0xFF, 0xc2)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x44, 0xFF, 0x37)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3E, 0xFF, 0xDD)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3F, 0xFF, 0x0f)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x45, 0xFF, 0xff)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x46, 0xFF, 0x77)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x47, 0xFF, 0xc2)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x48, 0xFF, 0x37)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x49, 0xFF, 0xff)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x4a, 0xFF, 0x77)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x83, 0x23, 0x23)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x33, 0x80, 0x80)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x34, 0x0a, 0x0a)}, \
	{ADIE_CODEC_ACTION_DELAY_WAIT, 0x186A0}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x34, 0x05, 0x05)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3B, 0xFF, 0x98)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3C, 0xFF, 0x98)}, \
	{ADIE_CODEC_ACTION_DELAY_WAIT, 0x3E8}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3B, 0xFF, 0x88)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3C, 0xFF, 0x88)}, \
	{ADIE_CODEC_ACTION_DELAY_WAIT, 0x3E8}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3B, 0xFF, 0x78)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3C, 0xFF, 0x78)}, \
	{ADIE_CODEC_ACTION_DELAY_WAIT, 0x3E8}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3B, 0xFF, 0x68)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3C, 0xFF, 0x68)}, \
	{ADIE_CODEC_ACTION_DELAY_WAIT, 0x3E8}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3B, 0xFF, 0x58)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3C, 0xFF, 0x58)}, \
	{ADIE_CODEC_ACTION_DELAY_WAIT, 0x3E8}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3B, 0xFF, 0x48)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3C, 0xFF, 0x48)}, \
	{ADIE_CODEC_ACTION_DELAY_WAIT, 0x3E8}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3B, 0xFF, 0x38)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3C, 0xFF, 0x38)}, \
	{ADIE_CODEC_ACTION_DELAY_WAIT, 0x3E8}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3B, 0xFF, 0x28)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3C, 0xFF, 0x28)}, \
	{ADIE_CODEC_ACTION_DELAY_WAIT, 0x3E8}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3B, 0xFF, 0x18)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3C, 0xFF, 0x18)}, \
	{ADIE_CODEC_ACTION_DELAY_WAIT, 0x3E8}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3B, 0xFF, 0x08)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3C, 0xFF, 0x08)}, \
	{ADIE_CODEC_ACTION_DELAY_WAIT, 0x3E8}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3B, 0xFF, 0x04)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3C, 0xFF, 0x04)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x84, 0xff, 0x00)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x85, 0xff, 0x00)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x8A, 0x0f, 0x0c)}, \
	{ADIE_CODEC_ACTION_STAGE_REACHED, ADIE_CODEC_DIGITAL_ANALOG_READY}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x8a, 0x03, 0x03)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3b, 0xFC, 0xAC)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3c, 0xFC, 0xAC)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x34, 0x0f, 0x00)}, \
	{ADIE_CODEC_ACTION_STAGE_REACHED, ADIE_CODEC_ANALOG_OFF}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x33, 0x80, 0x00)}, \
	{ADIE_CODEC_ACTION_STAGE_REACHED, ADIE_CODEC_DIGITAL_OFF} }


#define HEADSET_RX_CLASS_D_LEGACY_16000_OSR_256 \
	{{ADIE_CODEC_ACTION_STAGE_REACHED, ADIE_CODEC_FLASH_IMAGE}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3B, 0xFF, 0xac)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3C, 0xFF, 0xac)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x80, 0x02, 0x02)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x80, 0x02, 0x00)}, \
	{ADIE_CODEC_ACTION_STAGE_REACHED, ADIE_CODEC_DIGITAL_READY}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x81, 0xFF, 0x0e)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x23, 0xf8, 0xA8)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x28, 0xfF, 0xCA)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x29, 0xfF, 0xCA)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x39, 0x50, 0x50)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x40, 0xFF, 0x08)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x41, 0xFF, 0x08)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x42, 0xFF, 0xff)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x43, 0xFF, 0xd4)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x44, 0xFF, 0x37)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3E, 0xFF, 0xDD)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3F, 0xFF, 0x0f)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x45, 0xFF, 0xff)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x46, 0xFF, 0x77)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x47, 0xFF, 0xd4)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x48, 0xFF, 0x37)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x49, 0xFF, 0xff)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x4a, 0xFF, 0x77)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x83, 0x23, 0x23)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x33, 0x80, 0x80)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x34, 0x0a, 0x0a)}, \
	{ADIE_CODEC_ACTION_DELAY_WAIT, 0x186A0}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x34, 0x05, 0x05)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3B, 0xFF, 0x98)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3C, 0xFF, 0x98)}, \
	{ADIE_CODEC_ACTION_DELAY_WAIT, 0x3E8}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3B, 0xFF, 0x88)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3C, 0xFF, 0x88)}, \
	{ADIE_CODEC_ACTION_DELAY_WAIT, 0x3E8}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3B, 0xFF, 0x78)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3C, 0xFF, 0x78)}, \
	{ADIE_CODEC_ACTION_DELAY_WAIT, 0x3E8}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3B, 0xFF, 0x68)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3C, 0xFF, 0x68)}, \
	{ADIE_CODEC_ACTION_DELAY_WAIT, 0x3E8}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3B, 0xFF, 0x58)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3C, 0xFF, 0x58)}, \
	{ADIE_CODEC_ACTION_DELAY_WAIT, 0x3E8}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3B, 0xFF, 0x48)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3C, 0xFF, 0x48)}, \
	{ADIE_CODEC_ACTION_DELAY_WAIT, 0x3E8}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3B, 0xFF, 0x38)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3C, 0xFF, 0x38)}, \
	{ADIE_CODEC_ACTION_DELAY_WAIT, 0x3E8}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3B, 0xFF, 0x28)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3C, 0xFF, 0x28)}, \
	{ADIE_CODEC_ACTION_DELAY_WAIT, 0x3E8}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3B, 0xFF, 0x18)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3C, 0xFF, 0x18)}, \
	{ADIE_CODEC_ACTION_DELAY_WAIT, 0x3E8}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3B, 0xFF, 0x08)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3C, 0xFF, 0x08)}, \
	{ADIE_CODEC_ACTION_DELAY_WAIT, 0x3E8}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3B, 0xFF, 0x04)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3C, 0xFF, 0x04)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x84, 0xff, 0x00)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x85, 0xff, 0x00)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x8A, 0x0f, 0x0c)}, \
	{ADIE_CODEC_ACTION_STAGE_REACHED, ADIE_CODEC_DIGITAL_ANALOG_READY}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x8a, 0x03, 0x03)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3b, 0xFC, 0xAC)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3c, 0xFC, 0xAC)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x34, 0x0f, 0x00)}, \
	{ADIE_CODEC_ACTION_STAGE_REACHED, ADIE_CODEC_ANALOG_OFF}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x33, 0x80, 0x00)}, \
	{ADIE_CODEC_ACTION_STAGE_REACHED, ADIE_CODEC_DIGITAL_OFF} }


#define HEADSET_RX_CLASS_D_LEGACY_22050_OSR_256 \
	{{ADIE_CODEC_ACTION_STAGE_REACHED, ADIE_CODEC_FLASH_IMAGE}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3B, 0xFF, 0xac)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3C, 0xFF, 0xac)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x80, 0x02, 0x02)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x80, 0x02, 0x00)}, \
	{ADIE_CODEC_ACTION_STAGE_REACHED, ADIE_CODEC_DIGITAL_READY}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x81, 0xFF, 0x0e)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x23, 0xf8, 0xA8)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x28, 0xfF, 0xCA)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x29, 0xfF, 0xCA)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x39, 0x50, 0x50)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x40, 0xFF, 0x08)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x41, 0xFF, 0x08)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x42, 0xFF, 0xbb)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x43, 0xFF, 0xd2)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x44, 0xFF, 0x37)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3E, 0xFF, 0xDD)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3F, 0xFF, 0x0f)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x45, 0xFF, 0xff)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x46, 0xFF, 0x77)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x47, 0xFF, 0xd2)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x48, 0xFF, 0x37)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x49, 0xFF, 0xff)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x4a, 0xFF, 0x77)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x83, 0x23, 0x23)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x33, 0x80, 0x80)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x34, 0x0a, 0x0a)}, \
	{ADIE_CODEC_ACTION_DELAY_WAIT, 0x186A0}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x34, 0x05, 0x05)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3B, 0xFF, 0x98)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3C, 0xFF, 0x98)}, \
	{ADIE_CODEC_ACTION_DELAY_WAIT, 0x3E8}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3B, 0xFF, 0x88)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3C, 0xFF, 0x88)}, \
	{ADIE_CODEC_ACTION_DELAY_WAIT, 0x3E8}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3B, 0xFF, 0x78)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3C, 0xFF, 0x78)}, \
	{ADIE_CODEC_ACTION_DELAY_WAIT, 0x3E8}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3B, 0xFF, 0x68)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3C, 0xFF, 0x68)}, \
	{ADIE_CODEC_ACTION_DELAY_WAIT, 0x3E8}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3B, 0xFF, 0x58)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3C, 0xFF, 0x58)}, \
	{ADIE_CODEC_ACTION_DELAY_WAIT, 0x3E8}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3B, 0xFF, 0x48)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3C, 0xFF, 0x48)}, \
	{ADIE_CODEC_ACTION_DELAY_WAIT, 0x3E8}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3B, 0xFF, 0x38)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3C, 0xFF, 0x38)}, \
	{ADIE_CODEC_ACTION_DELAY_WAIT, 0x3E8}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3B, 0xFF, 0x28)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3C, 0xFF, 0x28)}, \
	{ADIE_CODEC_ACTION_DELAY_WAIT, 0x3E8}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3B, 0xFF, 0x18)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3C, 0xFF, 0x18)}, \
	{ADIE_CODEC_ACTION_DELAY_WAIT, 0x3E8}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3B, 0xFF, 0x08)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3C, 0xFF, 0x08)}, \
	{ADIE_CODEC_ACTION_DELAY_WAIT, 0x3E8}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3B, 0xFF, 0x04)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3C, 0xFF, 0x04)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x84, 0xff, 0x00)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x85, 0xff, 0x00)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x8A, 0x0f, 0x0c)}, \
	{ADIE_CODEC_ACTION_STAGE_REACHED, ADIE_CODEC_DIGITAL_ANALOG_READY}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x8a, 0x03, 0x03)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3b, 0xFC, 0xAC)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3c, 0xFC, 0xAC)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x34, 0x0f, 0x00)}, \
	{ADIE_CODEC_ACTION_STAGE_REACHED, ADIE_CODEC_ANALOG_OFF}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x33, 0x80, 0x00)}, \
	{ADIE_CODEC_ACTION_STAGE_REACHED, ADIE_CODEC_DIGITAL_OFF} }


#define HEADSET_CLASS_D_LEGACY_32000_OSR_256 \
	{{ADIE_CODEC_ACTION_STAGE_REACHED, ADIE_CODEC_FLASH_IMAGE}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3B, 0xFF, 0xac)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3C, 0xFF, 0xac)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x80, 0x02, 0x02)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x80, 0x02, 0x00)}, \
	{ADIE_CODEC_ACTION_STAGE_REACHED, ADIE_CODEC_DIGITAL_READY}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x81, 0xFF, 0x0e)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x23, 0xf8, 0xA8)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x28, 0xfF, 0xCA)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x29, 0xfF, 0xCA)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x39, 0x50, 0x50)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x40, 0xFF, 0x08)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x41, 0xFF, 0x08)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x42, 0xFF, 0xff)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x43, 0xFF, 0xf4)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x44, 0xFF, 0x37)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3E, 0xFF, 0xDD)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3F, 0xFF, 0x0f)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x45, 0xFF, 0xff)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x46, 0xFF, 0x77)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x47, 0xFF, 0xf4)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x48, 0xFF, 0x37)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x49, 0xFF, 0xff)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x4a, 0xFF, 0x77)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x83, 0x23, 0x23)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x33, 0x80, 0x80)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x34, 0x0a, 0x0a)}, \
	{ADIE_CODEC_ACTION_DELAY_WAIT, 0x186A0}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x34, 0x05, 0x05)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3B, 0xFF, 0x98)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3C, 0xFF, 0x98)}, \
	{ADIE_CODEC_ACTION_DELAY_WAIT, 0x3E8}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3B, 0xFF, 0x88)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3C, 0xFF, 0x88)}, \
	{ADIE_CODEC_ACTION_DELAY_WAIT, 0x3E8}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3B, 0xFF, 0x78)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3C, 0xFF, 0x78)}, \
	{ADIE_CODEC_ACTION_DELAY_WAIT, 0x3E8}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3B, 0xFF, 0x68)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3C, 0xFF, 0x68)}, \
	{ADIE_CODEC_ACTION_DELAY_WAIT, 0x3E8}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3B, 0xFF, 0x58)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3C, 0xFF, 0x58)}, \
	{ADIE_CODEC_ACTION_DELAY_WAIT, 0x3E8}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3B, 0xFF, 0x48)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3C, 0xFF, 0x48)}, \
	{ADIE_CODEC_ACTION_DELAY_WAIT, 0x3E8}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3B, 0xFF, 0x38)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3C, 0xFF, 0x38)}, \
	{ADIE_CODEC_ACTION_DELAY_WAIT, 0x3E8}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3B, 0xFF, 0x28)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3C, 0xFF, 0x28)}, \
	{ADIE_CODEC_ACTION_DELAY_WAIT, 0x3E8}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3B, 0xFF, 0x18)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3C, 0xFF, 0x18)}, \
	{ADIE_CODEC_ACTION_DELAY_WAIT, 0x3E8}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3B, 0xFF, 0x08)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3C, 0xFF, 0x08)}, \
	{ADIE_CODEC_ACTION_DELAY_WAIT, 0x3E8}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3B, 0xFF, 0x04)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3C, 0xFF, 0x04)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x84, 0xff, 0x00)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x85, 0xff, 0x00)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x8A, 0x0f, 0x0c)}, \
	{ADIE_CODEC_ACTION_STAGE_REACHED, ADIE_CODEC_DIGITAL_ANALOG_READY}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x8a, 0x03, 0x03)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3b, 0xFC, 0xAC)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3c, 0xFC, 0xAC)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x34, 0x0f, 0x00)}, \
	{ADIE_CODEC_ACTION_STAGE_REACHED, ADIE_CODEC_ANALOG_OFF}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x33, 0x80, 0x00)}, \
	{ADIE_CODEC_ACTION_STAGE_REACHED, ADIE_CODEC_DIGITAL_OFF} }

#define HEADSET_RX_CLASS_D_LEGACY_48000_OSR_64 \
	{{ADIE_CODEC_ACTION_STAGE_REACHED, ADIE_CODEC_FLASH_IMAGE}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3B, 0xFF, 0xac)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3C, 0xFF, 0xac)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x80, 0x02, 0x02)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x80, 0x02, 0x00)}, \
	{ADIE_CODEC_ACTION_STAGE_REACHED, ADIE_CODEC_DIGITAL_READY}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x81, 0xFF, 0x02)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x23, 0xf8, 0xA8)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x28, 0xfF, 0xCA)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x29, 0xfF, 0xCA)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x39, 0x50, 0x50)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x40, 0xFF, 0x08)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x41, 0xFF, 0x08)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x42, 0xFF, 0x55)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x43, 0xFF, 0xc5)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x44, 0xFF, 0x37)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3E, 0xFF, 0xDD)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3F, 0xFF, 0x0f)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x45, 0xFF, 0xff)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x46, 0xFF, 0x77)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x47, 0xFF, 0xc2)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x48, 0xFF, 0x37)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x49, 0xFF, 0xff)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x4a, 0xFF, 0x77)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x83, 0x23, 0x23)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x33, 0x80, 0x80)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x34, 0x0a, 0x0a)}, \
	{ADIE_CODEC_ACTION_DELAY_WAIT, 0x186A0}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x34, 0x05, 0x05)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3B, 0xFF, 0x98)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3C, 0xFF, 0x98)}, \
	{ADIE_CODEC_ACTION_DELAY_WAIT, 0x3E8}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3B, 0xFF, 0x88)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3C, 0xFF, 0x88)}, \
	{ADIE_CODEC_ACTION_DELAY_WAIT, 0x3E8}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3B, 0xFF, 0x78)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3C, 0xFF, 0x78)}, \
	{ADIE_CODEC_ACTION_DELAY_WAIT, 0x3E8}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3B, 0xFF, 0x68)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3C, 0xFF, 0x68)}, \
	{ADIE_CODEC_ACTION_DELAY_WAIT, 0x3E8}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3B, 0xFF, 0x58)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3C, 0xFF, 0x58)}, \
	{ADIE_CODEC_ACTION_DELAY_WAIT, 0x3E8}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3B, 0xFF, 0x48)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3C, 0xFF, 0x48)}, \
	{ADIE_CODEC_ACTION_DELAY_WAIT, 0x3E8}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3B, 0xFF, 0x38)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3C, 0xFF, 0x38)}, \
	{ADIE_CODEC_ACTION_DELAY_WAIT, 0x3E8}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3B, 0xFF, 0x28)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3C, 0xFF, 0x28)}, \
	{ADIE_CODEC_ACTION_DELAY_WAIT, 0x3E8}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3B, 0xFF, 0x18)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3C, 0xFF, 0x18)}, \
	{ADIE_CODEC_ACTION_DELAY_WAIT, 0x3E8}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3B, 0xFF, 0x08)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3C, 0xFF, 0x08)}, \
	{ADIE_CODEC_ACTION_DELAY_WAIT, 0x3E8}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3B, 0xFF, 0x04)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3C, 0xFF, 0x04)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x84, 0xff, 0x00)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x85, 0xff, 0x00)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x8A, 0x0f, 0x0c)}, \
	{ADIE_CODEC_ACTION_STAGE_REACHED, ADIE_CODEC_DIGITAL_ANALOG_READY}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x8a, 0x03, 0x03)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3b, 0xFC, 0xAC)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3c, 0xFC, 0xAC)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x34, 0x0f, 0x00)}, \
	{ADIE_CODEC_ACTION_STAGE_REACHED, ADIE_CODEC_ANALOG_OFF}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x33, 0x80, 0x00)}, \
	{ADIE_CODEC_ACTION_STAGE_REACHED, ADIE_CODEC_DIGITAL_OFF} }

#define HEADSET_RX_CLASS_D_LEGACY_48000_OSR_256 \
	{{ADIE_CODEC_ACTION_STAGE_REACHED, ADIE_CODEC_FLASH_IMAGE}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3B, 0xFF, 0xac)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3C, 0xFF, 0xac)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x80, 0x02, 0x02)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x80, 0x02, 0x00)}, \
	{ADIE_CODEC_ACTION_STAGE_REACHED, ADIE_CODEC_DIGITAL_READY}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x81, 0xFF, 0x0e)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x23, 0xf8, 0xA8)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x28, 0xfF, 0xCA)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x29, 0xfF, 0xCA)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x39, 0x50, 0x50)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x40, 0xFF, 0x08)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x41, 0xFF, 0x08)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x42, 0xFF, 0xBB)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x43, 0xFF, 0xF2)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x44, 0xFF, 0x37)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3E, 0xFF, 0xDD)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3F, 0xFF, 0x0f)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x45, 0xFF, 0xff)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x46, 0xFF, 0x77)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x47, 0xFF, 0xF2)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x48, 0xFF, 0x37)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x49, 0xFF, 0xff)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x4a, 0xFF, 0x77)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x83, 0x23, 0x23)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x33, 0x80, 0x80)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x34, 0x0a, 0x0a)}, \
	{ADIE_CODEC_ACTION_DELAY_WAIT, 0x186A0}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x34, 0x05, 0x05)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3B, 0xFF, 0x98)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3C, 0xFF, 0x98)}, \
	{ADIE_CODEC_ACTION_DELAY_WAIT, 0x3E8}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3B, 0xFF, 0x88)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3C, 0xFF, 0x88)}, \
	{ADIE_CODEC_ACTION_DELAY_WAIT, 0x3E8}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3B, 0xFF, 0x78)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3C, 0xFF, 0x78)}, \
	{ADIE_CODEC_ACTION_DELAY_WAIT, 0x3E8}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3B, 0xFF, 0x68)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3C, 0xFF, 0x68)}, \
	{ADIE_CODEC_ACTION_DELAY_WAIT, 0x3E8}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3B, 0xFF, 0x58)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3C, 0xFF, 0x58)}, \
	{ADIE_CODEC_ACTION_DELAY_WAIT, 0x3E8}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3B, 0xFF, 0x48)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3C, 0xFF, 0x48)}, \
	{ADIE_CODEC_ACTION_DELAY_WAIT, 0x3E8}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3B, 0xFF, 0x38)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3C, 0xFF, 0x38)}, \
	{ADIE_CODEC_ACTION_DELAY_WAIT, 0x3E8}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3B, 0xFF, 0x28)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3C, 0xFF, 0x28)}, \
	{ADIE_CODEC_ACTION_DELAY_WAIT, 0x3E8}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3B, 0xFF, 0x18)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3C, 0xFF, 0x18)}, \
	{ADIE_CODEC_ACTION_DELAY_WAIT, 0x3E8}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3B, 0xFF, 0x08)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3C, 0xFF, 0x08)}, \
	{ADIE_CODEC_ACTION_DELAY_WAIT, 0x3E8}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3B, 0xFF, 0x04)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3C, 0xFF, 0x04)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x84, 0xff, 0x00)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x85, 0xff, 0x00)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x8A, 0x0f, 0x0c)}, \
	{ADIE_CODEC_ACTION_STAGE_REACHED, ADIE_CODEC_DIGITAL_ANALOG_READY}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x8a, 0x03, 0x03)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3b, 0xFC, 0xAC)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3c, 0xFC, 0xAC)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x34, 0x0f, 0x00)}, \
	{ADIE_CODEC_ACTION_STAGE_REACHED, ADIE_CODEC_ANALOG_OFF}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x33, 0x80, 0x00)}, \
	{ADIE_CODEC_ACTION_STAGE_REACHED, ADIE_CODEC_DIGITAL_OFF} }

#define HEADSET_STEREO_RX_CAPLESS_8000_OSR_256 \
	{{ADIE_CODEC_ACTION_STAGE_REACHED, ADIE_CODEC_FLASH_IMAGE}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x80, 0x02, 0x02)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x80, 0x02, 0x00)}, \
	{ADIE_CODEC_ACTION_STAGE_REACHED, ADIE_CODEC_DIGITAL_READY}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x81, 0xFF, 0x4E)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x04, 0xff, 0xBC)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x24, 0x6F, 0x64)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x25, 0x0F, 0x0b)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x26, 0xfc, 0xfc)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x38, 0xff, 0x82)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3A, 0xFF, 0xeb)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x83, 0x23, 0x23)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x33, 0x80, 0x80)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x34, 0xf0, 0xf0)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x23, 0xff, 0x20)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3B, 0xFF, 0x04)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3C, 0xFF, 0x04)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x84, 0xff, 0x00)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x85, 0xff, 0x00)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x8A, 0x0f, 0x0c)}, \
	{ADIE_CODEC_ACTION_STAGE_REACHED, ADIE_CODEC_DIGITAL_ANALOG_READY}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x8a, 0x03, 0x03)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3b, 0xFF, 0xAC)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3C, 0xFF, 0xAC)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x34, 0xf0, 0x00)}, \
	{ADIE_CODEC_ACTION_STAGE_REACHED, ADIE_CODEC_ANALOG_OFF}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x33, 0x80, 0x00)}, \
	{ADIE_CODEC_ACTION_STAGE_REACHED, ADIE_CODEC_DIGITAL_OFF} }

#define HEADSET_STEREO_RX_CAPLESS_48000_OSR_64 \
	{{ADIE_CODEC_ACTION_STAGE_REACHED, ADIE_CODEC_FLASH_IMAGE}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x80, 0x02, 0x02)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x80, 0x02, 0x00)}, \
	{ADIE_CODEC_ACTION_STAGE_REACHED, ADIE_CODEC_DIGITAL_READY}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x81, 0xFF, 0x42)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x04, 0xff, 0xBC)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x24, 0x6F, 0x67)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x25, 0x0F, 0x0b)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x26, 0xfc, 0xfc)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x38, 0xff, 0x82)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3A, 0xFF, 0xab)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x83, 0x23, 0x23)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x33, 0x80, 0x80)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x34, 0xf0, 0xf0)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x23, 0xff, 0x20)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3B, 0xFF, 0x04)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3c, 0xFF, 0x04)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x84, 0xff, 0x00)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x85, 0xff, 0x00)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x8A, 0x0f, 0x0c)}, \
	{ADIE_CODEC_ACTION_STAGE_REACHED, ADIE_CODEC_DIGITAL_ANALOG_READY}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x8a, 0x03, 0x03)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3b, 0xFF, 0xAC)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3c, 0xFF, 0xAC)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x34, 0xf0, 0x00)}, \
	{ADIE_CODEC_ACTION_STAGE_REACHED, ADIE_CODEC_ANALOG_OFF}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x33, 0x80, 0x00)}, \
	{ADIE_CODEC_ACTION_STAGE_REACHED, ADIE_CODEC_DIGITAL_OFF} }



#define HEADSET_STEREO_RX_CAPLESS_48000_OSR_256 \
	{{ADIE_CODEC_ACTION_STAGE_REACHED, ADIE_CODEC_FLASH_IMAGE}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x80, 0x02, 0x02)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x80, 0x02, 0x00)}, \
	{ADIE_CODEC_ACTION_STAGE_REACHED, ADIE_CODEC_DIGITAL_READY}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x81, 0xFF, 0x4e)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x04, 0xff, 0xBC)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x24, 0x6F, 0x64)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x25, 0x0F, 0x0B)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x26, 0xfc, 0xfc)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x38, 0xff, 0x82)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3A, 0xFF, 0xab)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x83, 0x23, 0x23)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x33, 0x80, 0x82)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x34, 0xf0, 0xf0)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x23, 0xff, 0x20)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3B, 0xFF, 0x04)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3c, 0xFF, 0x04)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x84, 0xff, 0x00)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x85, 0xff, 0x00)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x8A, 0x0f, 0x0c)}, \
	{ADIE_CODEC_ACTION_STAGE_REACHED, ADIE_CODEC_DIGITAL_ANALOG_READY}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x8a, 0x03, 0x03)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3b, 0xFF, 0xAC)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3c, 0xFF, 0xAC)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x34, 0xf0, 0x00)}, \
	{ADIE_CODEC_ACTION_STAGE_REACHED, ADIE_CODEC_ANALOG_OFF}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x33, 0x80, 0x00)}, \
	{ADIE_CODEC_ACTION_STAGE_REACHED, ADIE_CODEC_DIGITAL_OFF} }

#define HEADSET_STEREO_RX_LEGACY_8000_OSR_256 \
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
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x38, 0xff, 0x80)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3A, 0xFF, 0x2b)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x83, 0x23, 0x23)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x33, 0x80, 0x80)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x34, 0xf0, 0xa0)}, \
	{ADIE_CODEC_ACTION_DELAY_WAIT, 0x186A0}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x34, 0xf0, 0xf0)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x23, 0xff, 0x20)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3B, 0xFF, 0x98)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3C, 0xFF, 0x98)}, \
	{ADIE_CODEC_ACTION_DELAY_WAIT, 0x3E8}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3B, 0xFF, 0x88)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3C, 0xFF, 0x88)}, \
	{ADIE_CODEC_ACTION_DELAY_WAIT, 0x3E8}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3B, 0xFF, 0x78)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3C, 0xFF, 0x78)}, \
	{ADIE_CODEC_ACTION_DELAY_WAIT, 0x3E8}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3B, 0xFF, 0x68)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3C, 0xFF, 0x68)}, \
	{ADIE_CODEC_ACTION_DELAY_WAIT, 0x3E8}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3B, 0xFF, 0x58)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3C, 0xFF, 0x58)}, \
	{ADIE_CODEC_ACTION_DELAY_WAIT, 0x3E8}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3B, 0xFF, 0x48)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3C, 0xFF, 0x48)}, \
	{ADIE_CODEC_ACTION_DELAY_WAIT, 0x3E8}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3B, 0xFF, 0x38)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3C, 0xFF, 0x38)}, \
	{ADIE_CODEC_ACTION_DELAY_WAIT, 0x3E8}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3B, 0xFF, 0x28)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3C, 0xFF, 0x28)}, \
	{ADIE_CODEC_ACTION_DELAY_WAIT, 0x3E8}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3B, 0xFF, 0x18)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3C, 0xFF, 0x18)}, \
	{ADIE_CODEC_ACTION_DELAY_WAIT, 0x3E8}, \
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

#define HEADSET_STEREO_RX_LEGACY_48000_OSR_64 \
	{{ADIE_CODEC_ACTION_STAGE_REACHED, ADIE_CODEC_FLASH_IMAGE}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3B, 0xFF, 0xac)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3C, 0xFF, 0xac)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x80, 0x02, 0x02)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x80, 0x02, 0x00)}, \
	{ADIE_CODEC_ACTION_STAGE_REACHED, ADIE_CODEC_DIGITAL_READY}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x81, 0xFF, 0x42)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x04, 0xff, 0xBC)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x24, 0x6F, 0x67)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x25, 0x0F, 0x0b)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x26, 0xfc, 0xfc)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x38, 0xff, 0x80)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3A, 0xFF, 0x2b)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x83, 0x23, 0x23)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x33, 0x80, 0x80)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x34, 0xf0, 0xa0)}, \
	{ADIE_CODEC_ACTION_DELAY_WAIT, 0x186A0}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x34, 0xf0, 0xf0)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x23, 0xff, 0x20)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3B, 0xFF, 0x98)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3C, 0xFF, 0x98)}, \
	{ADIE_CODEC_ACTION_DELAY_WAIT, 0x3E8}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3B, 0xFF, 0x88)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3C, 0xFF, 0x88)}, \
	{ADIE_CODEC_ACTION_DELAY_WAIT, 0x3E8}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3B, 0xFF, 0x78)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3C, 0xFF, 0x78)}, \
	{ADIE_CODEC_ACTION_DELAY_WAIT, 0x3E8}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3B, 0xFF, 0x68)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3C, 0xFF, 0x68)}, \
	{ADIE_CODEC_ACTION_DELAY_WAIT, 0x3E8}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3B, 0xFF, 0x58)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3C, 0xFF, 0x58)}, \
	{ADIE_CODEC_ACTION_DELAY_WAIT, 0x3E8}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3B, 0xFF, 0x48)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3C, 0xFF, 0x48)}, \
	{ADIE_CODEC_ACTION_DELAY_WAIT, 0x3E8}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3B, 0xFF, 0x38)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3C, 0xFF, 0x38)}, \
	{ADIE_CODEC_ACTION_DELAY_WAIT, 0x3E8}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3B, 0xFF, 0x28)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3C, 0xFF, 0x28)}, \
	{ADIE_CODEC_ACTION_DELAY_WAIT, 0x3E8}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3B, 0xFF, 0x18)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3C, 0xFF, 0x18)}, \
	{ADIE_CODEC_ACTION_DELAY_WAIT, 0x3E8}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3B, 0xFF, 0x10)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3C, 0xFF, 0x10)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x84, 0xff, 0x00)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x85, 0xff, 0x00)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x8A, 0x0f, 0x0c)}, \
	{ADIE_CODEC_ACTION_STAGE_REACHED, ADIE_CODEC_DIGITAL_ANALOG_READY}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x8a, 0x03, 0x03)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3b, 0xFF, 0xAC)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3c, 0xFF, 0xAC)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x34, 0xf0, 0x00)}, \
	{ADIE_CODEC_ACTION_STAGE_REACHED, ADIE_CODEC_ANALOG_OFF}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x33, 0x80, 0x00)}, \
	{ADIE_CODEC_ACTION_STAGE_REACHED, ADIE_CODEC_DIGITAL_OFF} }
/*  //404010 build error
#define HEADSET_STEREO_RX_LEGACY_48000_OSR_256 \
	{{ADIE_CODEC_ACTION_STAGE_REACHED, ADIE_CODEC_FLASH_IMAGE}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x80, 0x02, 0x02)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x80, 0x02, 0x00)}, \
	{ADIE_CODEC_ACTION_STAGE_REACHED, ADIE_CODEC_DIGITAL_READY}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3B, 0xFF, 0xac)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3C, 0xFF, 0xac)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x81, 0xFF, 0x4e)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x04, 0xff, 0xBC)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x24, 0x6F, 0x64)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x25, 0x0F, 0x0B)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x26, 0xfc, 0xfc)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x38, 0xff, 0x80)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3A, 0xFF, 0x2b)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x83, 0x23, 0x23)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x33, 0x80, 0x80)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x34, 0xf0, 0xa0)}, \
	{ADIE_CODEC_ACTION_DELAY_WAIT, 0x186A0}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x34, 0xf0, 0xf0)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x23, 0xff, 0x20)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3B, 0xFF, 0x98)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3C, 0xFF, 0x98)}, \
	{ADIE_CODEC_ACTION_DELAY_WAIT, 0x3E8}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3B, 0xFF, 0x88)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3C, 0xFF, 0x88)}, \
	{ADIE_CODEC_ACTION_DELAY_WAIT, 0x3E8}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3B, 0xFF, 0x78)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3C, 0xFF, 0x78)}, \
	{ADIE_CODEC_ACTION_DELAY_WAIT, 0x3E8}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3B, 0xFF, 0x68)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3C, 0xFF, 0x68)}, \
	{ADIE_CODEC_ACTION_DELAY_WAIT, 0x3E8}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3B, 0xFF, 0x58)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3C, 0xFF, 0x58)}, \
	{ADIE_CODEC_ACTION_DELAY_WAIT, 0x3E8}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3B, 0xFF, 0x48)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3C, 0xFF, 0x48)}, \
	{ADIE_CODEC_ACTION_DELAY_WAIT, 0x3E8}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3B, 0xFF, 0x38)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3C, 0xFF, 0x38)}, \
	{ADIE_CODEC_ACTION_DELAY_WAIT, 0x3E8}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3B, 0xFF, 0x28)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3C, 0xFF, 0x28)}, \
	{ADIE_CODEC_ACTION_DELAY_WAIT, 0x3E8}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3B, 0xFF, 0x18)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3C, 0xFF, 0x18)}, \
	{ADIE_CODEC_ACTION_DELAY_WAIT, 0x3E8}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3B, 0xFF, 0x10)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3C, 0xFF, 0x10)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x84, 0xff, 0x00)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x85, 0xff, 0x00)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x8A, 0x0f, 0x0c)}, \
	{ADIE_CODEC_ACTION_STAGE_REACHED, ADIE_CODEC_DIGITAL_ANALOG_READY}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x8a, 0x03, 0x03)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3b, 0xFF, 0xAC)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3c, 0xFF, 0xAC)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x34, 0xf0, 0x00)}, \
	{ADIE_CODEC_ACTION_STAGE_REACHED, ADIE_CODEC_ANALOG_OFF}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x33, 0x80, 0x00)}, \
	{ADIE_CODEC_ACTION_STAGE_REACHED, ADIE_CODEC_DIGITAL_OFF} }
*/

#define HEADSET_STEREO_RX_CLASS_D_LEGACY_8000_OSR_256 \
	{{ADIE_CODEC_ACTION_STAGE_REACHED, ADIE_CODEC_FLASH_IMAGE}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3B, 0xFF, 0xac)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3C, 0xFF, 0xac)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x80, 0x02, 0x02)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x80, 0x02, 0x00)}, \
	{ADIE_CODEC_ACTION_STAGE_REACHED, ADIE_CODEC_DIGITAL_READY}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x81, 0xFF, 0x0e)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x23, 0xf8, 0xA8)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x28, 0xfF, 0xCA)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x29, 0xfF, 0xCA)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x39, 0x40, 0x40)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x40, 0xFF, 0x08)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x41, 0xFF, 0x08)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x42, 0xFF, 0xFF)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x43, 0xFF, 0xc4)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x44, 0xFF, 0x37)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3E, 0xFF, 0xDD)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3F, 0xFF, 0x0f)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x45, 0xFF, 0xff)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x46, 0xFF, 0x77)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x47, 0xFF, 0xc4)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x48, 0xFF, 0x37)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x49, 0xFF, 0xff)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x4a, 0xFF, 0x77)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x83, 0x23, 0x23)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x33, 0x80, 0x80)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x34, 0x0a, 0x0a)}, \
	{ADIE_CODEC_ACTION_DELAY_WAIT, 0x186A0}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x34, 0x05, 0x05)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3B, 0xFF, 0x98)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3C, 0xFF, 0x98)}, \
	{ADIE_CODEC_ACTION_DELAY_WAIT, 0x3E8}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3B, 0xFF, 0x88)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3C, 0xFF, 0x88)}, \
	{ADIE_CODEC_ACTION_DELAY_WAIT, 0x3E8}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3B, 0xFF, 0x78)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3C, 0xFF, 0x78)}, \
	{ADIE_CODEC_ACTION_DELAY_WAIT, 0x3E8}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3B, 0xFF, 0x68)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3C, 0xFF, 0x68)}, \
	{ADIE_CODEC_ACTION_DELAY_WAIT, 0x3E8}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3B, 0xFF, 0x58)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3C, 0xFF, 0x58)}, \
	{ADIE_CODEC_ACTION_DELAY_WAIT, 0x3E8}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3B, 0xFF, 0x48)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3C, 0xFF, 0x48)}, \
	{ADIE_CODEC_ACTION_DELAY_WAIT, 0x3E8}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3B, 0xFF, 0x38)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3C, 0xFF, 0x38)}, \
	{ADIE_CODEC_ACTION_DELAY_WAIT, 0x3E8}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3B, 0xFF, 0x28)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3C, 0xFF, 0x28)}, \
	{ADIE_CODEC_ACTION_DELAY_WAIT, 0x3E8}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3B, 0xFF, 0x18)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3C, 0xFF, 0x18)}, \
	{ADIE_CODEC_ACTION_DELAY_WAIT, 0x3E8}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3B, 0xFF, 0x08)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3C, 0xFF, 0x08)}, \
	{ADIE_CODEC_ACTION_DELAY_WAIT, 0x3E8}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3B, 0xFF, 0x04)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3C, 0xFF, 0x04)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x84, 0xff, 0x00)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x85, 0xff, 0x00)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x8A, 0x0f, 0x0c)}, \
	{ADIE_CODEC_ACTION_STAGE_REACHED, ADIE_CODEC_DIGITAL_ANALOG_READY}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x8a, 0x03, 0x03)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3b, 0xFC, 0xAC)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3c, 0xFC, 0xAC)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x34, 0x0f, 0x00)}, \
	{ADIE_CODEC_ACTION_STAGE_REACHED, ADIE_CODEC_ANALOG_OFF}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x33, 0x80, 0x00)}, \
	{ADIE_CODEC_ACTION_STAGE_REACHED, ADIE_CODEC_DIGITAL_OFF} }


#define HEADSET_STEREO_RX_CLASS_D_LEGACY_11025_OSR_256 \
	{{ADIE_CODEC_ACTION_STAGE_REACHED, ADIE_CODEC_FLASH_IMAGE}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3B, 0xFF, 0xac)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3C, 0xFF, 0xac)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x80, 0x02, 0x02)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x80, 0x02, 0x00)}, \
	{ADIE_CODEC_ACTION_STAGE_REACHED, ADIE_CODEC_DIGITAL_READY}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x81, 0xFF, 0x0e)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x23, 0xf8, 0xA8)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x28, 0xfF, 0xCA)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x29, 0xfF, 0xCA)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x39, 0x40, 0x40)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x40, 0xFF, 0x08)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x41, 0xFF, 0x08)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x42, 0xFF, 0xbb)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x43, 0xFF, 0xc2)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x44, 0xFF, 0x37)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3E, 0xFF, 0xDD)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3F, 0xFF, 0x0f)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x45, 0xFF, 0xff)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x46, 0xFF, 0x77)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x47, 0xFF, 0xc2)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x48, 0xFF, 0x37)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x49, 0xFF, 0xff)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x4a, 0xFF, 0x77)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x83, 0x23, 0x23)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x33, 0x80, 0x80)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x34, 0x0a, 0x0a)}, \
	{ADIE_CODEC_ACTION_DELAY_WAIT, 0x186A0}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x34, 0x05, 0x05)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3B, 0xFF, 0x98)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3C, 0xFF, 0x98)}, \
	{ADIE_CODEC_ACTION_DELAY_WAIT, 0x3E8}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3B, 0xFF, 0x88)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3C, 0xFF, 0x88)}, \
	{ADIE_CODEC_ACTION_DELAY_WAIT, 0x3E8}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3B, 0xFF, 0x78)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3C, 0xFF, 0x78)}, \
	{ADIE_CODEC_ACTION_DELAY_WAIT, 0x3E8}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3B, 0xFF, 0x68)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3C, 0xFF, 0x68)}, \
	{ADIE_CODEC_ACTION_DELAY_WAIT, 0x3E8}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3B, 0xFF, 0x58)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3C, 0xFF, 0x58)}, \
	{ADIE_CODEC_ACTION_DELAY_WAIT, 0x3E8}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3B, 0xFF, 0x48)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3C, 0xFF, 0x48)}, \
	{ADIE_CODEC_ACTION_DELAY_WAIT, 0x3E8}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3B, 0xFF, 0x38)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3C, 0xFF, 0x38)}, \
	{ADIE_CODEC_ACTION_DELAY_WAIT, 0x3E8}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3B, 0xFF, 0x28)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3C, 0xFF, 0x28)}, \
	{ADIE_CODEC_ACTION_DELAY_WAIT, 0x3E8}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3B, 0xFF, 0x18)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3C, 0xFF, 0x18)}, \
	{ADIE_CODEC_ACTION_DELAY_WAIT, 0x3E8}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3B, 0xFF, 0x08)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3C, 0xFF, 0x08)}, \
	{ADIE_CODEC_ACTION_DELAY_WAIT, 0x3E8}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3B, 0xFF, 0x04)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3C, 0xFF, 0x04)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x84, 0xff, 0x00)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x85, 0xff, 0x00)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x8A, 0x0f, 0x0c)}, \
	{ADIE_CODEC_ACTION_STAGE_REACHED, ADIE_CODEC_DIGITAL_ANALOG_READY}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x8a, 0x03, 0x03)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3b, 0xFC, 0xAC)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3c, 0xFC, 0xAC)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x34, 0x0f, 0x00)}, \
	{ADIE_CODEC_ACTION_STAGE_REACHED, ADIE_CODEC_ANALOG_OFF}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x33, 0x80, 0x00)}, \
	{ADIE_CODEC_ACTION_STAGE_REACHED, ADIE_CODEC_DIGITAL_OFF} }

#define HEADSET_STEREO_RX_CLASS_D_LEGACY_16000_OSR_256 \
	{{ADIE_CODEC_ACTION_STAGE_REACHED, ADIE_CODEC_FLASH_IMAGE}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3B, 0xFF, 0xac)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3C, 0xFF, 0xac)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x80, 0x02, 0x02)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x80, 0x02, 0x00)}, \
	{ADIE_CODEC_ACTION_STAGE_REACHED, ADIE_CODEC_DIGITAL_READY}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x81, 0xFF, 0x0e)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x23, 0xf8, 0xA8)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x28, 0xfF, 0xCA)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x29, 0xfF, 0xCA)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x39, 0x40, 0x40)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x40, 0xFF, 0x08)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x41, 0xFF, 0x08)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x42, 0xFF, 0xff)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x43, 0xFF, 0xd4)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x44, 0xFF, 0x37)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3E, 0xFF, 0xDD)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3F, 0xFF, 0x0f)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x45, 0xFF, 0xff)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x46, 0xFF, 0x77)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x47, 0xFF, 0xd4)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x48, 0xFF, 0x37)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x49, 0xFF, 0xff)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x4a, 0xFF, 0x77)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x83, 0x23, 0x23)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x33, 0x80, 0x80)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x34, 0x0a, 0x0a)}, \
	{ADIE_CODEC_ACTION_DELAY_WAIT, 0x186A0}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x34, 0x05, 0x05)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3B, 0xFF, 0x98)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3C, 0xFF, 0x98)}, \
	{ADIE_CODEC_ACTION_DELAY_WAIT, 0x3E8}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3B, 0xFF, 0x88)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3C, 0xFF, 0x88)}, \
	{ADIE_CODEC_ACTION_DELAY_WAIT, 0x3E8}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3B, 0xFF, 0x78)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3C, 0xFF, 0x78)}, \
	{ADIE_CODEC_ACTION_DELAY_WAIT, 0x3E8}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3B, 0xFF, 0x68)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3C, 0xFF, 0x68)}, \
	{ADIE_CODEC_ACTION_DELAY_WAIT, 0x3E8}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3B, 0xFF, 0x58)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3C, 0xFF, 0x58)}, \
	{ADIE_CODEC_ACTION_DELAY_WAIT, 0x3E8}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3B, 0xFF, 0x48)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3C, 0xFF, 0x48)}, \
	{ADIE_CODEC_ACTION_DELAY_WAIT, 0x3E8}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3B, 0xFF, 0x38)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3C, 0xFF, 0x38)}, \
	{ADIE_CODEC_ACTION_DELAY_WAIT, 0x3E8}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3B, 0xFF, 0x28)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3C, 0xFF, 0x28)}, \
	{ADIE_CODEC_ACTION_DELAY_WAIT, 0x3E8}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3B, 0xFF, 0x18)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3C, 0xFF, 0x18)}, \
	{ADIE_CODEC_ACTION_DELAY_WAIT, 0x3E8}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3B, 0xFF, 0x08)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3C, 0xFF, 0x08)}, \
	{ADIE_CODEC_ACTION_DELAY_WAIT, 0x3E8}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3B, 0xFF, 0x04)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3C, 0xFF, 0x04)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x84, 0xff, 0x00)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x85, 0xff, 0x00)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x8A, 0x0f, 0x0c)}, \
	{ADIE_CODEC_ACTION_STAGE_REACHED, ADIE_CODEC_DIGITAL_ANALOG_READY}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x8a, 0x03, 0x03)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3b, 0xFC, 0xAC)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3c, 0xFC, 0xAC)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x34, 0x0f, 0x00)}, \
	{ADIE_CODEC_ACTION_STAGE_REACHED, ADIE_CODEC_ANALOG_OFF}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x33, 0x80, 0x00)}, \
	{ADIE_CODEC_ACTION_STAGE_REACHED, ADIE_CODEC_DIGITAL_OFF} }


#define HEADSET_STEREO_RX_CLASS_D_LEGACY_22050_OSR_256 \
	{{ADIE_CODEC_ACTION_STAGE_REACHED, ADIE_CODEC_FLASH_IMAGE}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3B, 0xFF, 0xac)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3C, 0xFF, 0xac)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x80, 0x02, 0x02)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x80, 0x02, 0x00)}, \
	{ADIE_CODEC_ACTION_STAGE_REACHED, ADIE_CODEC_DIGITAL_READY}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x81, 0xFF, 0x0e)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x23, 0xf8, 0xA8)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x28, 0xfF, 0xCA)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x29, 0xfF, 0xCA)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x39, 0x40, 0x40)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x40, 0xFF, 0x08)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x41, 0xFF, 0x08)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x42, 0xFF, 0xbb)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x43, 0xFF, 0xd2)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x44, 0xFF, 0x37)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3E, 0xFF, 0xDD)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3F, 0xFF, 0x0f)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x45, 0xFF, 0xff)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x46, 0xFF, 0x77)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x47, 0xFF, 0xd2)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x48, 0xFF, 0x37)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x49, 0xFF, 0xff)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x4a, 0xFF, 0x77)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x83, 0x23, 0x23)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x33, 0x80, 0x80)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x34, 0x0a, 0x0a)}, \
	{ADIE_CODEC_ACTION_DELAY_WAIT, 0x186A0}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x34, 0x05, 0x05)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3B, 0xFF, 0x98)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3C, 0xFF, 0x98)}, \
	{ADIE_CODEC_ACTION_DELAY_WAIT, 0x3E8}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3B, 0xFF, 0x88)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3C, 0xFF, 0x88)}, \
	{ADIE_CODEC_ACTION_DELAY_WAIT, 0x3E8}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3B, 0xFF, 0x78)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3C, 0xFF, 0x78)}, \
	{ADIE_CODEC_ACTION_DELAY_WAIT, 0x3E8}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3B, 0xFF, 0x68)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3C, 0xFF, 0x68)}, \
	{ADIE_CODEC_ACTION_DELAY_WAIT, 0x3E8}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3B, 0xFF, 0x58)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3C, 0xFF, 0x58)}, \
	{ADIE_CODEC_ACTION_DELAY_WAIT, 0x3E8}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3B, 0xFF, 0x48)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3C, 0xFF, 0x48)}, \
	{ADIE_CODEC_ACTION_DELAY_WAIT, 0x3E8}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3B, 0xFF, 0x38)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3C, 0xFF, 0x38)}, \
	{ADIE_CODEC_ACTION_DELAY_WAIT, 0x3E8}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3B, 0xFF, 0x28)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3C, 0xFF, 0x28)}, \
	{ADIE_CODEC_ACTION_DELAY_WAIT, 0x3E8}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3B, 0xFF, 0x18)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3C, 0xFF, 0x18)}, \
	{ADIE_CODEC_ACTION_DELAY_WAIT, 0x3E8}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3B, 0xFF, 0x08)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3C, 0xFF, 0x08)}, \
	{ADIE_CODEC_ACTION_DELAY_WAIT, 0x3E8}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3B, 0xFF, 0x04)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3C, 0xFF, 0x04)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x84, 0xff, 0x00)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x85, 0xff, 0x00)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x8A, 0x0f, 0x0c)}, \
	{ADIE_CODEC_ACTION_STAGE_REACHED, ADIE_CODEC_DIGITAL_ANALOG_READY}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x8a, 0x03, 0x03)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3b, 0xFC, 0xAC)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3c, 0xFC, 0xAC)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x34, 0x0f, 0x00)}, \
	{ADIE_CODEC_ACTION_STAGE_REACHED, ADIE_CODEC_ANALOG_OFF}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x33, 0x80, 0x00)}, \
	{ADIE_CODEC_ACTION_STAGE_REACHED, ADIE_CODEC_DIGITAL_OFF} }


#define HEADSET_STEREO_RX_CLASS_D_LEGACY_32000_OSR_256 \
	{{ADIE_CODEC_ACTION_STAGE_REACHED, ADIE_CODEC_FLASH_IMAGE}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3B, 0xFF, 0xac)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3C, 0xFF, 0xac)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x80, 0x02, 0x02)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x80, 0x02, 0x00)}, \
	{ADIE_CODEC_ACTION_STAGE_REACHED, ADIE_CODEC_DIGITAL_READY}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x81, 0xFF, 0x0e)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x23, 0xf8, 0xA8)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x28, 0xfF, 0xCA)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x29, 0xfF, 0xCA)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x39, 0x40, 0x40)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x40, 0xFF, 0x08)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x41, 0xFF, 0x08)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x42, 0xFF, 0xff)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x43, 0xFF, 0xf4)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x44, 0xFF, 0x37)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3E, 0xFF, 0xDD)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3F, 0xFF, 0x0f)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x45, 0xFF, 0xff)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x46, 0xFF, 0x77)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x47, 0xFF, 0xf4)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x48, 0xFF, 0x37)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x49, 0xFF, 0xff)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x4a, 0xFF, 0x77)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x83, 0x23, 0x23)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x33, 0x80, 0x80)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x34, 0x0a, 0x0a)}, \
	{ADIE_CODEC_ACTION_DELAY_WAIT, 0x186A0}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x34, 0x05, 0x05)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3B, 0xFF, 0x98)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3C, 0xFF, 0x98)}, \
	{ADIE_CODEC_ACTION_DELAY_WAIT, 0x3E8}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3B, 0xFF, 0x88)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3C, 0xFF, 0x88)}, \
	{ADIE_CODEC_ACTION_DELAY_WAIT, 0x3E8}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3B, 0xFF, 0x78)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3C, 0xFF, 0x78)}, \
	{ADIE_CODEC_ACTION_DELAY_WAIT, 0x3E8}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3B, 0xFF, 0x68)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3C, 0xFF, 0x68)}, \
	{ADIE_CODEC_ACTION_DELAY_WAIT, 0x3E8}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3B, 0xFF, 0x58)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3C, 0xFF, 0x58)}, \
	{ADIE_CODEC_ACTION_DELAY_WAIT, 0x3E8}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3B, 0xFF, 0x48)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3C, 0xFF, 0x48)}, \
	{ADIE_CODEC_ACTION_DELAY_WAIT, 0x3E8}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3B, 0xFF, 0x38)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3C, 0xFF, 0x38)}, \
	{ADIE_CODEC_ACTION_DELAY_WAIT, 0x3E8}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3B, 0xFF, 0x28)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3C, 0xFF, 0x28)}, \
	{ADIE_CODEC_ACTION_DELAY_WAIT, 0x3E8}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3B, 0xFF, 0x18)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3C, 0xFF, 0x18)}, \
	{ADIE_CODEC_ACTION_DELAY_WAIT, 0x3E8}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3B, 0xFF, 0x08)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3C, 0xFF, 0x08)}, \
	{ADIE_CODEC_ACTION_DELAY_WAIT, 0x3E8}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3B, 0xFF, 0x04)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3C, 0xFF, 0x04)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x84, 0xff, 0x00)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x85, 0xff, 0x00)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x8A, 0x0f, 0x0c)}, \
	{ADIE_CODEC_ACTION_STAGE_REACHED, ADIE_CODEC_DIGITAL_ANALOG_READY}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x8a, 0x03, 0x03)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3b, 0xFC, 0xAC)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3c, 0xFC, 0xAC)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x34, 0x0f, 0x00)}, \
	{ADIE_CODEC_ACTION_STAGE_REACHED, ADIE_CODEC_ANALOG_OFF}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x33, 0x80, 0x00)}, \
	{ADIE_CODEC_ACTION_STAGE_REACHED, ADIE_CODEC_DIGITAL_OFF} }

#define HEADSET_STEREO_RX_CLASS_D_LEGACY_48000_OSR_64 \
	{{ADIE_CODEC_ACTION_STAGE_REACHED, ADIE_CODEC_FLASH_IMAGE}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3B, 0xFF, 0xac)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3C, 0xFF, 0xac)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x80, 0x02, 0x02)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x80, 0x02, 0x00)}, \
	{ADIE_CODEC_ACTION_STAGE_REACHED, ADIE_CODEC_DIGITAL_READY}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x81, 0xFF, 0x02)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x23, 0xf8, 0xA8)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x28, 0xfF, 0xCA)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x29, 0xfF, 0xCA)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x39, 0x40, 0x40)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x40, 0xFF, 0x08)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x41, 0xFF, 0x08)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x42, 0xFF, 0x55)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x43, 0xFF, 0xc5)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x44, 0xFF, 0x37)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3E, 0xFF, 0xDD)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3F, 0xFF, 0x0f)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x45, 0xFF, 0xff)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x46, 0xFF, 0x77)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x47, 0xFF, 0xc2)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x48, 0xFF, 0x37)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x49, 0xFF, 0xff)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x4a, 0xFF, 0x77)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x83, 0x23, 0x23)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x33, 0x80, 0x80)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x34, 0x0a, 0x0a)}, \
	{ADIE_CODEC_ACTION_DELAY_WAIT, 0x186A0}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x34, 0x05, 0x05)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3B, 0xFF, 0x98)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3C, 0xFF, 0x98)}, \
	{ADIE_CODEC_ACTION_DELAY_WAIT, 0x3E8}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3B, 0xFF, 0x88)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3C, 0xFF, 0x88)}, \
	{ADIE_CODEC_ACTION_DELAY_WAIT, 0x3E8}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3B, 0xFF, 0x78)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3C, 0xFF, 0x78)}, \
	{ADIE_CODEC_ACTION_DELAY_WAIT, 0x3E8}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3B, 0xFF, 0x68)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3C, 0xFF, 0x68)}, \
	{ADIE_CODEC_ACTION_DELAY_WAIT, 0x3E8}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3B, 0xFF, 0x58)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3C, 0xFF, 0x58)}, \
	{ADIE_CODEC_ACTION_DELAY_WAIT, 0x3E8}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3B, 0xFF, 0x48)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3C, 0xFF, 0x48)}, \
	{ADIE_CODEC_ACTION_DELAY_WAIT, 0x3E8}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3B, 0xFF, 0x38)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3C, 0xFF, 0x38)}, \
	{ADIE_CODEC_ACTION_DELAY_WAIT, 0x3E8}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3B, 0xFF, 0x28)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3C, 0xFF, 0x28)}, \
	{ADIE_CODEC_ACTION_DELAY_WAIT, 0x3E8}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3B, 0xFF, 0x18)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3C, 0xFF, 0x18)}, \
	{ADIE_CODEC_ACTION_DELAY_WAIT, 0x3E8}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3B, 0xFF, 0x08)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3C, 0xFF, 0x08)}, \
	{ADIE_CODEC_ACTION_DELAY_WAIT, 0x3E8}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3B, 0xFF, 0x04)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3C, 0xFF, 0x04)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x84, 0xff, 0x00)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x85, 0xff, 0x00)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x8A, 0x0f, 0x0c)}, \
	{ADIE_CODEC_ACTION_STAGE_REACHED, ADIE_CODEC_DIGITAL_ANALOG_READY}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x8a, 0x03, 0x03)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3b, 0xFC, 0xAC)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3c, 0xFC, 0xAC)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x34, 0x0f, 0x00)}, \
	{ADIE_CODEC_ACTION_STAGE_REACHED, ADIE_CODEC_ANALOG_OFF}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x33, 0x80, 0x00)}, \
	{ADIE_CODEC_ACTION_STAGE_REACHED, ADIE_CODEC_DIGITAL_OFF} }

#define HEADSET_STEREO_RX_CLASS_D_LEGACY_48000_OSR_256 \
	{{ADIE_CODEC_ACTION_STAGE_REACHED, ADIE_CODEC_FLASH_IMAGE}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3B, 0xFF, 0xac)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3C, 0xFF, 0xac)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x80, 0x02, 0x02)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x80, 0x02, 0x00)}, \
	{ADIE_CODEC_ACTION_STAGE_REACHED, ADIE_CODEC_DIGITAL_READY}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x81, 0xFF, 0x0e)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x23, 0xf8, 0xA8)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x28, 0xfF, 0xCA)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x29, 0xfF, 0xCA)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x39, 0x40, 0x40)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x40, 0xFF, 0x08)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x41, 0xFF, 0x08)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x42, 0xFF, 0xBB)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x43, 0xFF, 0xF2)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x44, 0xFF, 0x37)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3E, 0xFF, 0xDD)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3F, 0xFF, 0x0f)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x45, 0xFF, 0xff)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x46, 0xFF, 0x77)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x47, 0xFF, 0xF2)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x48, 0xFF, 0x37)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x49, 0xFF, 0xff)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x4a, 0xFF, 0x77)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x83, 0x23, 0x23)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x33, 0x80, 0x80)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x34, 0x0a, 0x0a)}, \
	{ADIE_CODEC_ACTION_DELAY_WAIT, 0x186A0}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x34, 0x05, 0x05)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3B, 0xFF, 0x98)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3C, 0xFF, 0x98)}, \
	{ADIE_CODEC_ACTION_DELAY_WAIT, 0x3E8}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3B, 0xFF, 0x88)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3C, 0xFF, 0x88)}, \
	{ADIE_CODEC_ACTION_DELAY_WAIT, 0x3E8}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3B, 0xFF, 0x78)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3C, 0xFF, 0x78)}, \
	{ADIE_CODEC_ACTION_DELAY_WAIT, 0x3E8}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3B, 0xFF, 0x68)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3C, 0xFF, 0x68)}, \
	{ADIE_CODEC_ACTION_DELAY_WAIT, 0x3E8}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3B, 0xFF, 0x58)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3C, 0xFF, 0x58)}, \
	{ADIE_CODEC_ACTION_DELAY_WAIT, 0x3E8}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3B, 0xFF, 0x48)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3C, 0xFF, 0x48)}, \
	{ADIE_CODEC_ACTION_DELAY_WAIT, 0x3E8}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3B, 0xFF, 0x38)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3C, 0xFF, 0x38)}, \
	{ADIE_CODEC_ACTION_DELAY_WAIT, 0x3E8}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3B, 0xFF, 0x28)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3C, 0xFF, 0x28)}, \
	{ADIE_CODEC_ACTION_DELAY_WAIT, 0x3E8}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3B, 0xFF, 0x18)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3C, 0xFF, 0x18)}, \
	{ADIE_CODEC_ACTION_DELAY_WAIT, 0x3E8}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3B, 0xFF, 0x08)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3C, 0xFF, 0x08)}, \
	{ADIE_CODEC_ACTION_DELAY_WAIT, 0x3E8}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3B, 0xFF, 0x04)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3C, 0xFF, 0x04)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x84, 0xff, 0x00)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x85, 0xff, 0x00)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x8A, 0x0f, 0x0c)}, \
	{ADIE_CODEC_ACTION_STAGE_REACHED, ADIE_CODEC_DIGITAL_ANALOG_READY}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x8a, 0x03, 0x03)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3b, 0xFC, 0xAC)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3c, 0xFC, 0xAC)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x34, 0x0f, 0x00)}, \
	{ADIE_CODEC_ACTION_STAGE_REACHED, ADIE_CODEC_ANALOG_OFF}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x33, 0x80, 0x00)}, \
	{ADIE_CODEC_ACTION_STAGE_REACHED, ADIE_CODEC_DIGITAL_OFF} }

#define SPEAKER_RX_8000_OSR_256 \
	{{ADIE_CODEC_ACTION_STAGE_REACHED, ADIE_CODEC_FLASH_IMAGE}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x80, 0x02, 0x02)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x80, 0x02, 0x00)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x81, 0xFF, 0x4E)}, \
	{ADIE_CODEC_ACTION_STAGE_REACHED, ADIE_CODEC_DIGITAL_READY}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x24, 0x6F, 0x64)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x25, 0x0F, 0x0b)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x26, 0xfc, 0xfc)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x37, 0xe2, 0xa2)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3A, 0xFF, 0x2B)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3d, 0xFF, 0x55)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x83, 0x23, 0x23)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x23, 0xff, 0x20)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x33, 0x8a, 0x8a)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x33, 0x05, 0x05)}, \
	{ADIE_CODEC_ACTION_DELAY_WAIT, 0x7530}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x85, 0xff, 0x00)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x84, 0xff, 0x00)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x8A, 0x05, 0x04)}, \
	{ADIE_CODEC_ACTION_STAGE_REACHED, ADIE_CODEC_DIGITAL_ANALOG_READY}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x8a, 0x01, 0x01)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x33, 0x40, 0x00)}, \
	{ADIE_CODEC_ACTION_STAGE_REACHED, ADIE_CODEC_ANALOG_OFF}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x33, 0x80, 0x00)}, \
	{ADIE_CODEC_ACTION_STAGE_REACHED, ADIE_CODEC_DIGITAL_OFF} }


#define SPEAKER_RX_48000_OSR_64 \
	{{ADIE_CODEC_ACTION_STAGE_REACHED, ADIE_CODEC_FLASH_IMAGE}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x80, 0x02, 0x02)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x80, 0x02, 0x00)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x81, 0xFF, 0x42)}, \
	{ADIE_CODEC_ACTION_STAGE_REACHED, ADIE_CODEC_DIGITAL_READY}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x24, 0x6F, 0x67)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x25, 0x0F, 0x0b)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x26, 0xfc, 0xfc)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x37, 0xe2, 0xa2)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3A, 0xFF, 0x2B)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3d, 0xFF, 0x55)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x83, 0x23, 0x23)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x23, 0xff, 0x20)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x33, 0x8a, 0x8a)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x33, 0x05, 0x05)}, \
	{ADIE_CODEC_ACTION_DELAY_WAIT, 0x7530}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x85, 0xff, 0x00)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x84, 0xff, 0x00)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x8A, 0x05, 0x04)}, \
	{ADIE_CODEC_ACTION_STAGE_REACHED, ADIE_CODEC_DIGITAL_ANALOG_READY}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x8a, 0x01, 0x01)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x33, 0x40, 0x00)}, \
	{ADIE_CODEC_ACTION_STAGE_REACHED, ADIE_CODEC_ANALOG_OFF}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x33, 0x80, 0x00)}, \
	{ADIE_CODEC_ACTION_STAGE_REACHED, ADIE_CODEC_DIGITAL_OFF} }

#define SPEAKER_RX_48000_OSR_256 \
	{{ADIE_CODEC_ACTION_STAGE_REACHED, ADIE_CODEC_FLASH_IMAGE}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x80, 0x02, 0x02)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x80, 0x02, 0x00)}, \
	{ADIE_CODEC_ACTION_STAGE_REACHED, ADIE_CODEC_DIGITAL_READY}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x24, 0x6F, 0x64)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x25, 0x0F, 0x0B)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x26, 0xfc, 0xfc)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x37, 0xe2, 0xa2)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3A, 0xFF, 0x2B)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3d, 0xFF, 0x55)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x83, 0x23, 0x23)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x23, 0xff, 0x20)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x33, 0x8a, 0x8a)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x33, 0x05, 0x05)}, \
	{ADIE_CODEC_ACTION_DELAY_WAIT, 0x7530}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x84, 0xff, 0x00)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x85, 0xff, 0x00)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x8A, 0x05, 0x04)}, \
	{ADIE_CODEC_ACTION_STAGE_REACHED, ADIE_CODEC_DIGITAL_ANALOG_READY}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x8a, 0x01, 0x01)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x33, 0x40, 0x00)}, \
	{ADIE_CODEC_ACTION_STAGE_REACHED, ADIE_CODEC_ANALOG_OFF}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x33, 0x80, 0x00)}, \
	{ADIE_CODEC_ACTION_STAGE_REACHED, ADIE_CODEC_DIGITAL_OFF} }

#define SPEAKER_STEREO_RX_8000_OSR_256 \
	{{ADIE_CODEC_ACTION_STAGE_REACHED, ADIE_CODEC_FLASH_IMAGE}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x80, 0x02, 0x02)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x80, 0x02, 0x00)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x81, 0xFF, 0x4E)}, \
	{ADIE_CODEC_ACTION_STAGE_REACHED, ADIE_CODEC_DIGITAL_READY}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x24, 0x6F, 0x64)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x25, 0x0F, 0x0E)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x26, 0xfc, 0xfc)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x37, 0xe6, 0x80)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3A, 0xFF, 0x2B)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3d, 0xFF, 0x55)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x83, 0x23, 0x23)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x23, 0xff, 0x20)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x33, 0x8a, 0x8a)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x33, 0x05, 0x05)}, \
	{ADIE_CODEC_ACTION_DELAY_WAIT, 0x7530}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x84, 0xff, 0x00)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x85, 0xff, 0x00)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x8A, 0x0f, 0x0c)}, \
	{ADIE_CODEC_ACTION_STAGE_REACHED, ADIE_CODEC_DIGITAL_ANALOG_READY}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x8a, 0x0f, 0x03)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x33, 0x40, 0x00)}, \
	{ADIE_CODEC_ACTION_STAGE_REACHED, ADIE_CODEC_ANALOG_OFF}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x33, 0x80, 0x00)}, \
	{ADIE_CODEC_ACTION_STAGE_REACHED, ADIE_CODEC_DIGITAL_OFF} }


#define SPEAKER_STEREO_RX_48000_OSR_64 \
	{{ADIE_CODEC_ACTION_STAGE_REACHED, ADIE_CODEC_FLASH_IMAGE}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x80, 0x02, 0x02)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x80, 0x02, 0x00)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x81, 0xFF, 0x42)}, \
	{ADIE_CODEC_ACTION_STAGE_REACHED, ADIE_CODEC_DIGITAL_READY}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x24, 0x6F, 0x67)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x25, 0x0F, 0x0b)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x26, 0xfc, 0xfc)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x37, 0xe6, 0x80)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3A, 0xFF, 0x2B)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3d, 0xFF, 0x55)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x83, 0x23, 0x23)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x23, 0xff, 0x20)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x33, 0x8a, 0x8a)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x33, 0x05, 0x05)}, \
	{ADIE_CODEC_ACTION_DELAY_WAIT, 0x7530}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x84, 0xff, 0x00)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x85, 0xff, 0x00)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x8A, 0x0f, 0x0c)}, \
	{ADIE_CODEC_ACTION_STAGE_REACHED, ADIE_CODEC_DIGITAL_ANALOG_READY}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x8a, 0x0f, 0x03)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x33, 0x40, 0x00)}, \
	{ADIE_CODEC_ACTION_STAGE_REACHED, ADIE_CODEC_ANALOG_OFF}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x33, 0x80, 0x00)}, \
	{ADIE_CODEC_ACTION_STAGE_REACHED, ADIE_CODEC_DIGITAL_OFF} }
/*   /404010 build error
#define SPEAKER_STEREO_RX_48000_OSR_256 \
	{{ADIE_CODEC_ACTION_STAGE_REACHED, ADIE_CODEC_FLASH_IMAGE}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x80, 0x02, 0x02)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x80, 0x02, 0x00)}, \
	{ADIE_CODEC_ACTION_STAGE_REACHED, ADIE_CODEC_DIGITAL_READY}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x24, 0x6F, 0x64)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x25, 0x0F, 0x0B)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x26, 0xfc, 0xfc)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x37, 0xe6, 0x80)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3A, 0xFF, 0x2B)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3d, 0xFF, 0x55)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x83, 0x23, 0x23)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x23, 0xff, 0x20)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x33, 0x8a, 0x8a)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x33, 0x05, 0x05)}, \
	{ADIE_CODEC_ACTION_DELAY_WAIT, 0x7530}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x84, 0xff, 0x00)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x85, 0xff, 0x00)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x8A, 0x0f, 0x0c)}, \
	{ADIE_CODEC_ACTION_STAGE_REACHED, ADIE_CODEC_DIGITAL_ANALOG_READY}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x8a, 0x0f, 0x03)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x33, 0x40, 0x00)}, \
	{ADIE_CODEC_ACTION_STAGE_REACHED, ADIE_CODEC_ANALOG_OFF}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x33, 0x80, 0x00)}, \
	{ADIE_CODEC_ACTION_STAGE_REACHED, ADIE_CODEC_DIGITAL_OFF} }
*/

#define SPEAKER_TX_8000_OSR_256 \
	{{ADIE_CODEC_ACTION_STAGE_REACHED, ADIE_CODEC_FLASH_IMAGE}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x80, 0x01, 0x01)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x80, 0x01, 0x00)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x8A, 0x30, 0x30)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x11, 0xfc, 0xfc)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x13, 0xfc, 0x58)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x14, 0xff, 0x65)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x15, 0xff, 0x64)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x82, 0xff, 0x5E)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x10, 0xFF, 0x68)}, \
	{ADIE_CODEC_ACTION_STAGE_REACHED, ADIE_CODEC_DIGITAL_READY}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x0D, 0xF0, 0xD0)}, \
	{ADIE_CODEC_ACTION_DELAY_WAIT, 0xbb8}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x83, 0x14, 0x14)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x8c, 0x03, 0x03)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x86, 0xff, 0x00)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x8A, 0x50, 0x40)}, \
	{ADIE_CODEC_ACTION_STAGE_REACHED, ADIE_CODEC_DIGITAL_ANALOG_READY}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x8A, 0x10, 0x30)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x0D, 0xFF, 0x00)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x83, 0x14, 0x00)}, \
	{ADIE_CODEC_ACTION_STAGE_REACHED, ADIE_CODEC_ANALOG_OFF}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x11, 0xff, 0x00)}, \
	{ADIE_CODEC_ACTION_STAGE_REACHED, ADIE_CODEC_DIGITAL_OFF} }

#define SPEAKER_TX_48000_OSR_64 \
	{{ADIE_CODEC_ACTION_STAGE_REACHED, ADIE_CODEC_FLASH_IMAGE}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x80, 0x01, 0x01)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x80, 0x01, 0x00)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x8A, 0x30, 0x30)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x11, 0xfc, 0xfc)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x13, 0xfc, 0x58)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x14, 0xff, 0x65)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x15, 0xff, 0x64)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x82, 0xff, 0x46)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x10, 0xFF, 0x68)}, \
	{ADIE_CODEC_ACTION_STAGE_REACHED, ADIE_CODEC_DIGITAL_READY}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x0D, 0xF0, 0xD0)}, \
	{ADIE_CODEC_ACTION_DELAY_WAIT, 0xbb8}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x83, 0x14, 0x14)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x8c, 0x03, 0x03)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x86, 0xff, 0x00)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x8A, 0x50, 0x40)}, \
	{ADIE_CODEC_ACTION_STAGE_REACHED, ADIE_CODEC_DIGITAL_ANALOG_READY}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x8A, 0x10, 0x30)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x0D, 0xFF, 0x00)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x83, 0x14, 0x00)}, \
	{ADIE_CODEC_ACTION_STAGE_REACHED, ADIE_CODEC_ANALOG_OFF}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x11, 0xff, 0x00)}, \
	{ADIE_CODEC_ACTION_STAGE_REACHED, ADIE_CODEC_DIGITAL_OFF} }

/*   //404010 build error
#define SPEAKER_TX_48000_OSR_256 \
	{{ADIE_CODEC_ACTION_STAGE_REACHED, ADIE_CODEC_FLASH_IMAGE}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x80, 0x01, 0x01)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x80, 0x01, 0x00)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x8A, 0x30, 0x30)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x11, 0xfc, 0xfc)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x13, 0xfc, 0x58)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x14, 0xff, 0x65)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x15, 0xff, 0x64)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x82, 0xff, 0x5A)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x10, 0xFF, 0x68)}, \
	{ADIE_CODEC_ACTION_STAGE_REACHED, ADIE_CODEC_DIGITAL_READY}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x0D, 0xF0, 0xD0)}, \
	{ADIE_CODEC_ACTION_DELAY_WAIT, 0xbb8}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x83, 0x14, 0x14)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x8c, 0x03, 0x03)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x86, 0xff, 0x00)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x8A, 0x50, 0x40)}, \
	{ADIE_CODEC_ACTION_STAGE_REACHED, ADIE_CODEC_DIGITAL_ANALOG_READY}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x8A, 0x10, 0x30)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x0D, 0xFF, 0x00)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x83, 0x14, 0x00)}, \
	{ADIE_CODEC_ACTION_STAGE_REACHED, ADIE_CODEC_ANALOG_OFF}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x11, 0xff, 0x00)}, \
	{ADIE_CODEC_ACTION_STAGE_REACHED, ADIE_CODEC_DIGITAL_OFF} }
*/
#define FM_HANDSET_OSR_64 \
	{{ADIE_CODEC_ACTION_STAGE_REACHED, ADIE_CODEC_FLASH_IMAGE}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x80, 0x02, 0x02)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x80, 0x02, 0x00)}, \
	{ADIE_CODEC_ACTION_STAGE_REACHED, ADIE_CODEC_DIGITAL_READY}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x24, 0x6F, 0x47)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x04, 0xff, 0x8C)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x81, 0xFF, 0x92)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x25, 0x0F, 0x0b)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x26, 0xfc, 0xfc)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x36, 0xc0, 0x80)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3A, 0xFF, 0x2B)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x23, 0xff, 0x20)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3d, 0xFF, 0xD5)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x83, 0x01, 0x01)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x33, 0x80, 0x80)}, \
	{ADIE_CODEC_ACTION_DELAY_WAIT, 0x2710}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x33, 0x40, 0x40)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x84, 0xff, 0x00)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x8A, 0x05, 0x04)}, \
	{ADIE_CODEC_ACTION_STAGE_REACHED, ADIE_CODEC_DIGITAL_ANALOG_READY}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x8a, 0x03, 0x03)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x36, 0xc0, 0x00)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x33, 0x40, 0x00)}, \
	{ADIE_CODEC_ACTION_STAGE_REACHED, ADIE_CODEC_ANALOG_OFF}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x33, 0x80, 0x00)}, \
	{ADIE_CODEC_ACTION_STAGE_REACHED, ADIE_CODEC_DIGITAL_OFF} }

#define FM_HEADSET_STEREO_CLASS_D_LEGACY_OSR_64 \
	{{ADIE_CODEC_ACTION_STAGE_REACHED, ADIE_CODEC_FLASH_IMAGE}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3B, 0xFF, 0xac)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3C, 0xFF, 0xac)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x80, 0x02, 0x02)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x80, 0x02, 0x00)}, \
	{ADIE_CODEC_ACTION_STAGE_REACHED, ADIE_CODEC_DIGITAL_READY}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x81, 0xFF, 0x92)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x23, 0xf8, 0xA8)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x28, 0xfF, 0xCA)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x29, 0xfF, 0xCA)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x39, 0x40, 0x40)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x40, 0xFF, 0x08)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x41, 0xFF, 0x08)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x42, 0xFF, 0x55)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x43, 0xFF, 0xD5)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x44, 0xFF, 0x37)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3E, 0xFF, 0xDD)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3F, 0xFF, 0x0f)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x45, 0xFF, 0xff)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x46, 0xFF, 0x77)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x47, 0xFF, 0xD5)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x48, 0xFF, 0x37)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x49, 0xFF, 0xff)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x4a, 0xFF, 0x77)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x83, 0x03, 0x03)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x33, 0x80, 0x80)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x34, 0x0a, 0x0a)}, \
	{ADIE_CODEC_ACTION_DELAY_WAIT, 0x186A0}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x34, 0x05, 0x05)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3B, 0xFF, 0x98)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3C, 0xFF, 0x98)}, \
	{ADIE_CODEC_ACTION_DELAY_WAIT, 0x3E8}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3B, 0xFF, 0x88)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3C, 0xFF, 0x88)}, \
	{ADIE_CODEC_ACTION_DELAY_WAIT, 0x3E8}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3B, 0xFF, 0x78)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3C, 0xFF, 0x88)}, \
	{ADIE_CODEC_ACTION_DELAY_WAIT, 0x3E8}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3B, 0xFF, 0x68)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3C, 0xFF, 0x88)}, \
	{ADIE_CODEC_ACTION_DELAY_WAIT, 0x3E8}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3B, 0xFF, 0x58)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3C, 0xFF, 0x88)}, \
	{ADIE_CODEC_ACTION_DELAY_WAIT, 0x3E8}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3B, 0xFF, 0x48)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3C, 0xFF, 0x88)}, \
	{ADIE_CODEC_ACTION_DELAY_WAIT, 0x3E8}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3B, 0xFF, 0x38)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3C, 0xFF, 0x88)}, \
	{ADIE_CODEC_ACTION_DELAY_WAIT, 0x3E8}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3B, 0xFF, 0x28)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3C, 0xFF, 0x88)}, \
	{ADIE_CODEC_ACTION_DELAY_WAIT, 0x3E8}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3B, 0xFF, 0x18)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3C, 0xFF, 0x88)}, \
	{ADIE_CODEC_ACTION_DELAY_WAIT, 0x3E8}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3B, 0xFF, 0x08)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3C, 0xFF, 0x88)}, \
	{ADIE_CODEC_ACTION_DELAY_WAIT, 0x3E8}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3B, 0xFF, 0x04)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3C, 0xFF, 0x04)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x84, 0xff, 0x00)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x85, 0xff, 0x00)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x8A, 0x0f, 0x0c)}, \
	{ADIE_CODEC_ACTION_STAGE_REACHED, ADIE_CODEC_DIGITAL_ANALOG_READY}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x8a, 0x03, 0x03)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3b, 0xFC, 0xAC)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3c, 0xFC, 0xAC)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x34, 0x0f, 0x00)}, \
	{ADIE_CODEC_ACTION_STAGE_REACHED, ADIE_CODEC_ANALOG_OFF}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x33, 0x80, 0x00)}, \
	{ADIE_CODEC_ACTION_STAGE_REACHED, ADIE_CODEC_DIGITAL_OFF} }

#define FM_HEADSET_CLASS_AB_STEREO_LEGACY_OSR_64 \
	{{ADIE_CODEC_ACTION_STAGE_REACHED, ADIE_CODEC_FLASH_IMAGE}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3B, 0xFF, 0xac)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3C, 0xFF, 0xac)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x80, 0x02, 0x02)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x80, 0x02, 0x00)}, \
	{ADIE_CODEC_ACTION_STAGE_REACHED, ADIE_CODEC_DIGITAL_READY}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x81, 0xFF, 0x92)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x04, 0xff, 0xBC)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x24, 0x6F, 0x67)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x25, 0x0F, 0x0b)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x26, 0xfc, 0xfc)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x38, 0xff, 0x80)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3A, 0xFF, 0x2b)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x83, 0x03, 0x03)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x33, 0x80, 0x80)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x34, 0xf0, 0xa0)}, \
	{ADIE_CODEC_ACTION_DELAY_WAIT, 0x186A0}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x34, 0xf0, 0xf0)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x23, 0xff, 0x20)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3B, 0xFF, 0x98)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3C, 0xFF, 0x98)}, \
	{ADIE_CODEC_ACTION_DELAY_WAIT, 0x3E8}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3B, 0xFF, 0x88)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3C, 0xFF, 0x88)}, \
	{ADIE_CODEC_ACTION_DELAY_WAIT, 0x3E8}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3B, 0xFF, 0x78)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3C, 0xFF, 0x78)}, \
	{ADIE_CODEC_ACTION_DELAY_WAIT, 0x3E8}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3B, 0xFF, 0x68)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3C, 0xFF, 0x68)}, \
	{ADIE_CODEC_ACTION_DELAY_WAIT, 0x3E8}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3B, 0xFF, 0x58)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3C, 0xFF, 0x58)}, \
	{ADIE_CODEC_ACTION_DELAY_WAIT, 0x3E8}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3B, 0xFF, 0x48)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3C, 0xFF, 0x48)}, \
	{ADIE_CODEC_ACTION_DELAY_WAIT, 0x3E8}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3B, 0xFF, 0x38)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3C, 0xFF, 0x38)}, \
	{ADIE_CODEC_ACTION_DELAY_WAIT, 0x3E8}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3B, 0xFF, 0x28)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3C, 0xFF, 0x28)}, \
	{ADIE_CODEC_ACTION_DELAY_WAIT, 0x3E8}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3B, 0xFF, 0x18)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3C, 0xFF, 0x18)}, \
	{ADIE_CODEC_ACTION_DELAY_WAIT, 0x3E8}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3B, 0xFF, 0x10)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3C, 0xFF, 0x10)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x84, 0xff, 0x00)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x85, 0xff, 0x00)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x8A, 0x0f, 0x0c)}, \
	{ADIE_CODEC_ACTION_STAGE_REACHED, ADIE_CODEC_DIGITAL_ANALOG_READY}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x8a, 0x03, 0x03)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3b, 0xFF, 0xAC)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3c, 0xFF, 0xAC)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x34, 0xf0, 0x00)}, \
	{ADIE_CODEC_ACTION_STAGE_REACHED, ADIE_CODEC_ANALOG_OFF}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x33, 0x80, 0x00)}, \
	{ADIE_CODEC_ACTION_STAGE_REACHED, ADIE_CODEC_DIGITAL_OFF} }

#define FM_HEADSET_CLASS_AB_STEREO_CAPLESS_OSR_64 \
	{{ADIE_CODEC_ACTION_STAGE_REACHED, ADIE_CODEC_FLASH_IMAGE}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x80, 0x02, 0x02)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x80, 0x02, 0x00)}, \
	{ADIE_CODEC_ACTION_STAGE_REACHED, ADIE_CODEC_DIGITAL_READY}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x81, 0xFF, 0x92)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x04, 0xff, 0xBC)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x24, 0x6F, 0x67)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x25, 0x0F, 0x0b)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x26, 0xfc, 0xfc)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x38, 0xff, 0x82)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3A, 0xFF, 0xeb)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x83, 0x23, 0x23)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x33, 0x80, 0x80)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x34, 0xf0, 0xf0)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x23, 0xff, 0x20)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3B, 0xFF, 0x04)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3c, 0xFF, 0x04)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x84, 0xff, 0x00)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x85, 0xff, 0x00)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x8A, 0x0f, 0x0c)}, \
	{ADIE_CODEC_ACTION_STAGE_REACHED, ADIE_CODEC_DIGITAL_ANALOG_READY}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x8a, 0x03, 0x03)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3b, 0xFF, 0xAC)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3c, 0xFF, 0xAC)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x34, 0xf0, 0x00)}, \
	{ADIE_CODEC_ACTION_STAGE_REACHED, ADIE_CODEC_ANALOG_OFF}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x33, 0x80, 0x00)}, \
	{ADIE_CODEC_ACTION_STAGE_REACHED, ADIE_CODEC_DIGITAL_OFF} }

#define FM_SPEAKER_OSR_64 \
	{{ADIE_CODEC_ACTION_STAGE_REACHED, ADIE_CODEC_FLASH_IMAGE}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x80, 0x02, 0x02)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x80, 0x02, 0x00)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x81, 0xFF, 0x92)}, \
	{ADIE_CODEC_ACTION_STAGE_REACHED, ADIE_CODEC_DIGITAL_READY}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x24, 0x6F, 0x67)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x25, 0x0F, 0x0E)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x26, 0xfc, 0xfc)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x37, 0xe2, 0xa2)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3A, 0xFF, 0x2B)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x23, 0xff, 0x20)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3d, 0xFF, 0x55)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x83, 0x23, 0x23)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x33, 0x8a, 0x8a)}, \
	{ADIE_CODEC_ACTION_DELAY_WAIT, 0x2710}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x33, 0x05, 0x05)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x85, 0xff, 0x00)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x84, 0xff, 0x00)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x8A, 0x05, 0x04)}, \
	{ADIE_CODEC_ACTION_STAGE_REACHED, ADIE_CODEC_DIGITAL_ANALOG_READY}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x8a, 0x01, 0x01)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x33, 0x40, 0x00)}, \
	{ADIE_CODEC_ACTION_STAGE_REACHED, ADIE_CODEC_ANALOG_OFF}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x33, 0x80, 0x00)}, \
	{ADIE_CODEC_ACTION_STAGE_REACHED, ADIE_CODEC_DIGITAL_OFF} }


#define AUXPGA_HEADSET_STEREO_RX_LEGACY \
	{{ADIE_CODEC_ACTION_STAGE_REACHED, ADIE_CODEC_FLASH_IMAGE}, \
	{ADIE_CODEC_ACTION_STAGE_REACHED, ADIE_CODEC_DIGITAL_READY}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x2B, 0xff, 0x09)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x2C, 0xff, 0x09)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3A, 0xFF, 0x2b)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x33, 0x80, 0x80)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x2B, 0xff, 0x89)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x2C, 0xff, 0x89)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x38, 0xff, 0x10)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x34, 0xf0, 0xa0)}, \
	{ADIE_CODEC_ACTION_DELAY_WAIT, 0x186A0}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x34, 0xf0, 0xf0)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3B, 0xFF, 0x10)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3c, 0xFF, 0x10)}, \
	{ADIE_CODEC_ACTION_STAGE_REACHED, ADIE_CODEC_DIGITAL_ANALOG_READY}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3b, 0xFF, 0xAC)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3c, 0xFF, 0xAC)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x38, 0x18, 0x00)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x34, 0x40, 0x00)}, \
	{ADIE_CODEC_ACTION_STAGE_REACHED, ADIE_CODEC_ANALOG_OFF}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x33, 0x80, 0x00)}, \
	{ADIE_CODEC_ACTION_STAGE_REACHED, ADIE_CODEC_DIGITAL_OFF} }

#define AUXPGA_HEADSET_MONO_RX_LEGACY \
	{{ADIE_CODEC_ACTION_STAGE_REACHED, ADIE_CODEC_FLASH_IMAGE}, \
	{ADIE_CODEC_ACTION_STAGE_REACHED, ADIE_CODEC_DIGITAL_READY}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x2B, 0xff, 0x09)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x2C, 0xff, 0x09)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3A, 0xFF, 0x2b)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x33, 0x80, 0x80)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x2B, 0xff, 0x89)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x2C, 0xff, 0x89)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x38, 0xff, 0x18)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x34, 0xf0, 0xa0)}, \
	{ADIE_CODEC_ACTION_DELAY_WAIT, 0x186A0}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x34, 0xf0, 0xf0)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3B, 0xFF, 0x10)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3c, 0xFF, 0x10)}, \
	{ADIE_CODEC_ACTION_STAGE_REACHED, ADIE_CODEC_DIGITAL_ANALOG_READY}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3b, 0xFF, 0xAC)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3c, 0xFF, 0xAC)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x38, 0x18, 0x00)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x34, 0x40, 0x00)}, \
	{ADIE_CODEC_ACTION_STAGE_REACHED, ADIE_CODEC_ANALOG_OFF}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x33, 0x80, 0x00)}, \
	{ADIE_CODEC_ACTION_STAGE_REACHED, ADIE_CODEC_DIGITAL_OFF} }

#define AUXPGA_HEADSET_STEREO_RX_CAPLESS \
	{{ADIE_CODEC_ACTION_STAGE_REACHED, ADIE_CODEC_FLASH_IMAGE}, \
	{ADIE_CODEC_ACTION_STAGE_REACHED, ADIE_CODEC_DIGITAL_READY}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3A, 0xFF, 0xeb)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x33, 0x80, 0x80)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x2B, 0xff, 0x89)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x2C, 0xff, 0x89)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x38, 0xff, 0x10)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x34, 0xf0, 0xa0)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x34, 0xf0, 0xf0)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3B, 0xFF, 0x04)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3c, 0xFF, 0x04)}, \
	{ADIE_CODEC_ACTION_STAGE_REACHED, ADIE_CODEC_DIGITAL_ANALOG_READY}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3b, 0xFF, 0xAC)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3c, 0xFF, 0xAC)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x38, 0x18, 0x00)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x34, 0x40, 0x00)}, \
	{ADIE_CODEC_ACTION_STAGE_REACHED, ADIE_CODEC_ANALOG_OFF}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x33, 0x80, 0x00)}, \
	{ADIE_CODEC_ACTION_STAGE_REACHED, ADIE_CODEC_DIGITAL_OFF} }

#define AUXPGA_HEADSET_MONO_RX_CAPLESS \
	{{ADIE_CODEC_ACTION_STAGE_REACHED, ADIE_CODEC_FLASH_IMAGE}, \
	{ADIE_CODEC_ACTION_STAGE_REACHED, ADIE_CODEC_DIGITAL_READY}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x2B, 0xff, 0x09)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x2C, 0xff, 0x09)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3A, 0xFF, 0xeb)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x33, 0x80, 0x80)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x2B, 0xff, 0x89)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x2C, 0xff, 0x89)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x38, 0xff, 0x18)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x34, 0xf0, 0xa0)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x34, 0xf0, 0xf0)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3B, 0xFF, 0x04)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3c, 0xFF, 0x04)}, \
	{ADIE_CODEC_ACTION_STAGE_REACHED, ADIE_CODEC_DIGITAL_ANALOG_READY}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3b, 0xFF, 0xAC)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3c, 0xFF, 0xAC)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x38, 0x18, 0x00)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x34, 0x40, 0x00)}, \
	{ADIE_CODEC_ACTION_STAGE_REACHED, ADIE_CODEC_ANALOG_OFF}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x33, 0x80, 0x00)}, \
	{ADIE_CODEC_ACTION_STAGE_REACHED, ADIE_CODEC_DIGITAL_OFF} }

#define AUXPGA_HEADSET_STEREO_RX_CLASS_D \
	{{ADIE_CODEC_ACTION_STAGE_REACHED, ADIE_CODEC_FLASH_IMAGE}, \
	{ADIE_CODEC_ACTION_STAGE_REACHED, ADIE_CODEC_DIGITAL_READY}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x2B, 0xff, 0x09)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x2C, 0xff, 0x09)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x2B, 0xff, 0x89)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x2C, 0xff, 0x89)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x39, 0x20, 0x20)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x40, 0xFF, 0x08)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x41, 0xFF, 0x08)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x42, 0xFF, 0x55)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x43, 0xFF, 0xD5)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x44, 0xFF, 0x37)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3E, 0xFF, 0xDD)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3F, 0xFF, 0x0f)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x45, 0xFF, 0xff)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x46, 0xFF, 0x77)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x47, 0xFF, 0xD5)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x48, 0xFF, 0x37)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x49, 0xFF, 0xff)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x4a, 0xFF, 0x77)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x33, 0x80, 0x80)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x34, 0x0a, 0x0a)}, \
	{ADIE_CODEC_ACTION_DELAY_WAIT, 0x186A0}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x34, 0x05, 0x05)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3B, 0xFC, 0x04)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3c, 0xFC, 0x04)}, \
	{ADIE_CODEC_ACTION_STAGE_REACHED, ADIE_CODEC_DIGITAL_ANALOG_READY}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x39, 0x20, 0x00)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3b, 0xFC, 0xAC)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3c, 0xFC, 0xAC)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x34, 0x05, 0x00)}, \
	{ADIE_CODEC_ACTION_STAGE_REACHED, ADIE_CODEC_ANALOG_OFF}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x33, 0x80, 0x00)}, \
	{ADIE_CODEC_ACTION_STAGE_REACHED, ADIE_CODEC_DIGITAL_OFF} }


#define AUXPGA_HEADSET_MONO_RX_CLASS_D \
	{{ADIE_CODEC_ACTION_STAGE_REACHED, ADIE_CODEC_FLASH_IMAGE}, \
	{ADIE_CODEC_ACTION_STAGE_REACHED, ADIE_CODEC_DIGITAL_READY}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x2B, 0xff, 0x09)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x2C, 0xff, 0x09)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x2B, 0xff, 0x89)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x2C, 0xff, 0x89)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x39, 0x30, 0x30)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x40, 0xFF, 0x08)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x41, 0xFF, 0x08)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x42, 0xFF, 0x55)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x43, 0xFF, 0xD5)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x44, 0xFF, 0x37)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3E, 0xFF, 0xDD)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3F, 0xFF, 0x0f)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x45, 0xFF, 0xff)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x46, 0xFF, 0x77)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x47, 0xFF, 0xD5)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x48, 0xFF, 0x37)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x49, 0xFF, 0xff)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x4a, 0xFF, 0x77)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x33, 0x80, 0x80)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x34, 0x0a, 0x0a)}, \
	{ADIE_CODEC_ACTION_DELAY_WAIT, 0x186A0}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x34, 0x05, 0x05)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3B, 0xFC, 0x04)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3c, 0xFC, 0x04)}, \
	{ADIE_CODEC_ACTION_STAGE_REACHED, ADIE_CODEC_DIGITAL_ANALOG_READY}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x39, 0x20, 0x00)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3b, 0xFC, 0xAC)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3c, 0xFC, 0xAC)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x34, 0x05, 0x00)}, \
	{ADIE_CODEC_ACTION_STAGE_REACHED, ADIE_CODEC_ANALOG_OFF}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x33, 0x80, 0x00)}, \
	{ADIE_CODEC_ACTION_STAGE_REACHED, ADIE_CODEC_DIGITAL_OFF} }

#define AUXPGA_EAR \
	{{ADIE_CODEC_ACTION_STAGE_REACHED, ADIE_CODEC_FLASH_IMAGE}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x2B, 0xff, 0x09)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x2C, 0xff, 0x09)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x2B, 0xff, 0x89)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x2C, 0xff, 0x89)}, \
	{ADIE_CODEC_ACTION_STAGE_REACHED, ADIE_CODEC_DIGITAL_READY}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x36, 0x20, 0x20)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x33, 0x80, 0x80)}, \
	{ADIE_CODEC_ACTION_DELAY_WAIT, 0x2710}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x33, 0x40, 0x40)}, \
	{ADIE_CODEC_ACTION_STAGE_REACHED, ADIE_CODEC_DIGITAL_ANALOG_READY}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x36, 0x20, 0x00)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x33, 0x40, 0x00)}, \
	{ADIE_CODEC_ACTION_STAGE_REACHED, ADIE_CODEC_ANALOG_OFF}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x33, 0x80, 0x00)}, \
	{ADIE_CODEC_ACTION_STAGE_REACHED, ADIE_CODEC_DIGITAL_OFF} }


/***************************************************************************\
				DigitalMicprofile
\***************************************************************************/
#define DIGITAL_MIC \
	{{ADIE_CODEC_ACTION_STAGE_REACHED, ADIE_CODEC_FLASH_IMAGE}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x80, 0x01, 0x01)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x80, 0x01, 0x00)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x8A, 0x30, 0x30)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x14, 0xfd, 0x65)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x1A, 0xff, 0xc0)}, \
	{ADIE_CODEC_ACTION_STAGE_REACHED, ADIE_CODEC_DIGITAL_READY}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x82, 0xFF, 0x66)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x83, 0x1c, 0x1c)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x87, 0xff, 0x00)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x86, 0xff, 0x00)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x8A, 0xF0, 0xF0)}, \
	{ADIE_CODEC_ACTION_STAGE_REACHED, ADIE_CODEC_DIGITAL_ANALOG_READY}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x8A, 0x30, 0x30)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x83, 0x1c, 0x00)}, \
	{ADIE_CODEC_ACTION_STAGE_REACHED, ADIE_CODEC_ANALOG_OFF}, \
	{ADIE_CODEC_ACTION_STAGE_REACHED, ADIE_CODEC_DIGITAL_OFF} }

/***************************************************************************\
				DualMicprofile
\***************************************************************************/
#define SPEAKER_MIC1_LEFT_LINE_IN_RIGHT_8000_OSR_256 \
	{{ADIE_CODEC_ACTION_STAGE_REACHED, ADIE_CODEC_FLASH_IMAGE}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x80, 0x01, 0x01)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x80, 0x01, 0x00)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x8A, 0x30, 0x30)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x11, 0xfc, 0xfc)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x12, 0xfc, 0xfc)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x13, 0xfc, 0x58)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x14, 0xfd, 0x65)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x15, 0xff, 0x64)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x82, 0xFF, 0x5E)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x10, 0xFF, 0x68)}, \
	{ADIE_CODEC_ACTION_STAGE_REACHED, ADIE_CODEC_DIGITAL_READY}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x0E, 0xE2, 0xE2)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x0D, 0xF0, 0xD0)}, \
	{ADIE_CODEC_ACTION_DELAY_WAIT, 0xbb8}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x83, 0x1c, 0x1c)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x8c, 0x03, 0x03)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x87, 0xff, 0x00)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x86, 0xff, 0x00)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x8A, 0xF0, 0xc0)}, \
	{ADIE_CODEC_ACTION_STAGE_REACHED, ADIE_CODEC_DIGITAL_ANALOG_READY}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x8A, 0x30, 0x30)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x0e, 0xFF, 0x00)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x0d, 0xFF, 0x00)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x83, 0x1c, 0x00)}, \
	{ADIE_CODEC_ACTION_STAGE_REACHED, ADIE_CODEC_ANALOG_OFF}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x12, 0xff, 0x00)}, \
	{ADIE_CODEC_ACTION_STAGE_REACHED, ADIE_CODEC_DIGITAL_OFF} }

#define SPEAKER_MIC1_LEFT_AUX_IN_RIGHT_8000_OSR_256 \
	{{ADIE_CODEC_ACTION_STAGE_REACHED, ADIE_CODEC_FLASH_IMAGE}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x80, 0x01, 0x01)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x80, 0x01, 0x00)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x8A, 0x30, 0x30)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x11, 0xfc, 0xfc)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x12, 0xfc, 0xfc)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x13, 0xfc, 0x58)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x14, 0xfd, 0x65)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x15, 0xff, 0x64)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x82, 0xFF, 0x5E)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x10, 0xFF, 0x68)}, \
	{ADIE_CODEC_ACTION_STAGE_REACHED, ADIE_CODEC_DIGITAL_READY}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x0E, 0xE2, 0xE1)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x0D, 0xF0, 0xD0)}, \
	{ADIE_CODEC_ACTION_DELAY_WAIT, 0xbb8}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x83, 0x1c, 0x1c)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x8c, 0x03, 0x03)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x87, 0xff, 0x00)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x86, 0xff, 0x00)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x8A, 0xF0, 0xc0)}, \
	{ADIE_CODEC_ACTION_STAGE_REACHED, ADIE_CODEC_DIGITAL_ANALOG_READY}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x8A, 0x30, 0x30)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x0e, 0xFF, 0x00)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x0d, 0xFF, 0x00)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x83, 0x1c, 0x00)}, \
	{ADIE_CODEC_ACTION_STAGE_REACHED, ADIE_CODEC_ANALOG_OFF}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x12, 0xff, 0x00)}, \
	{ADIE_CODEC_ACTION_STAGE_REACHED, ADIE_CODEC_DIGITAL_OFF} }

#define MIC1_LEFT_LINE_IN_RIGHT_8000_OSR_256 \
	{{ADIE_CODEC_ACTION_STAGE_REACHED, ADIE_CODEC_FLASH_IMAGE}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x80, 0x01, 0x01)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x80, 0x01, 0x00)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x8A, 0x30, 0x30)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x11, 0xfc, 0xfc)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x12, 0xfc, 0xfc)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x13, 0xfc, 0x58)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x14, 0xfd, 0x65)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x15, 0xff, 0x64)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x82, 0xFF, 0x5E)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x10, 0xFF, 0x68)}, \
	{ADIE_CODEC_ACTION_STAGE_REACHED, ADIE_CODEC_DIGITAL_READY}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x0E, 0xE2, 0xE2)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x0D, 0xF0, 0xD0)}, \
	{ADIE_CODEC_ACTION_DELAY_WAIT, 0xbb8}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x83, 0x1c, 0x1c)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x8b, 0xff, 0xCE)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x8c, 0x03, 0x02)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x87, 0xff, 0x00)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x86, 0xff, 0x00)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x8A, 0xF0, 0xc0)}, \
	{ADIE_CODEC_ACTION_STAGE_REACHED, ADIE_CODEC_DIGITAL_ANALOG_READY}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x8A, 0x30, 0x30)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x0e, 0xFF, 0x00)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x0d, 0xFF, 0x00)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x83, 0x1c, 0x00)}, \
	{ADIE_CODEC_ACTION_STAGE_REACHED, ADIE_CODEC_ANALOG_OFF}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x12, 0xff, 0x00)}, \
	{ADIE_CODEC_ACTION_STAGE_REACHED, ADIE_CODEC_DIGITAL_OFF} }

#define MIC1_LEFT_AUX_IN_RIGHT_8000_OSR_256 \
	{{ADIE_CODEC_ACTION_STAGE_REACHED, ADIE_CODEC_FLASH_IMAGE}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x80, 0x01, 0x01)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x80, 0x01, 0x00)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x8A, 0x30, 0x30)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x11, 0xfc, 0xfc)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x12, 0xfc, 0xfc)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x13, 0xfc, 0x58)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x14, 0xfd, 0x65)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x15, 0xff, 0x64)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x82, 0xFF, 0x5E)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x10, 0xFF, 0x68)}, \
	{ADIE_CODEC_ACTION_STAGE_REACHED, ADIE_CODEC_DIGITAL_READY}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x0E, 0xE1, 0xE1)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x0D, 0xF0, 0xD0)}, \
	{ADIE_CODEC_ACTION_DELAY_WAIT, 0xbb8}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x83, 0x1c, 0x1c)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x8b, 0xff, 0xCE)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x8c, 0x03, 0x02)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x87, 0xff, 0x00)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x86, 0xff, 0x00)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x8A, 0xF0, 0xc0)}, \
	{ADIE_CODEC_ACTION_STAGE_REACHED, ADIE_CODEC_DIGITAL_ANALOG_READY}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x8A, 0x30, 0x30)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x0e, 0xFF, 0x00)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x0d, 0xFF, 0x00)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x83, 0x1c, 0x00)}, \
	{ADIE_CODEC_ACTION_STAGE_REACHED, ADIE_CODEC_ANALOG_OFF}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x12, 0xff, 0x00)}, \
	{ADIE_CODEC_ACTION_STAGE_REACHED, ADIE_CODEC_DIGITAL_OFF} }


/***************************************************************************\
				AnalogDualMicProfile
\***************************************************************************/
#define ANALOG_DUAL_MIC \
	{{ADIE_CODEC_ACTION_STAGE_REACHED, ADIE_CODEC_FLASH_IMAGE}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x80, 0x01, 0x01)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x80, 0x01, 0x00)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x8A, 0x30, 0x30)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x11, 0xfc, 0xfc)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x12, 0xfc, 0xfc)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x13, 0xfc, 0x58)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x14, 0xfd, 0x65)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x15, 0xff, 0x64)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x82, 0xFF, 0x5E)}, \
	{ADIE_CODEC_ACTION_STAGE_REACHED, ADIE_CODEC_DIGITAL_READY}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x0E, 0xE2, 0xE2)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x0D, 0xF0, 0xD0)}, \
	{ADIE_CODEC_ACTION_DELAY_WAIT, 0xbb8}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x83, 0x1c, 0x1c)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x87, 0xff, 0x00)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x86, 0xff, 0x00)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x8A, 0xF0, 0xF0)}, \
	{ADIE_CODEC_ACTION_STAGE_REACHED, ADIE_CODEC_DIGITAL_ANALOG_READY}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x8A, 0x30, 0x30)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x0e, 0xFF, 0x00)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x0d, 0xFF, 0x00)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x83, 0x1c, 0x00)}, \
	{ADIE_CODEC_ACTION_STAGE_REACHED, ADIE_CODEC_ANALOG_OFF}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x12, 0xff, 0x00)}, \
	{ADIE_CODEC_ACTION_STAGE_REACHED, ADIE_CODEC_DIGITAL_OFF} }
/***************************************************************************\
				TTY
\***************************************************************************/
#define TTY_HEADSET_MONO_TX_8000_OSR_256 \
	{{ADIE_CODEC_ACTION_STAGE_REACHED, ADIE_CODEC_FLASH_IMAGE}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x80, 0x01, 0x01)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x80, 0x01, 0x00)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x8A, 0x30, 0x30)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x11, 0xfc, 0xfc)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x13, 0xfc, 0x58)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x14, 0xff, 0x65)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x15, 0xfc, 0x64)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x82, 0xFF, 0x5E)}, \
	{ADIE_CODEC_ACTION_STAGE_REACHED, ADIE_CODEC_DIGITAL_READY}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x0D, 0xFf, 0xA1)/*(0x0D, 0xFf, 0xA8)*/}, \
	{ADIE_CODEC_ACTION_DELAY_WAIT, 0xbb8}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x83, 0x14, 0x14)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x8c, 0x03, 0x03)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x86, 0xff, 0x0A)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x8A, 0x50, 0x40)}, \
	{ADIE_CODEC_ACTION_STAGE_REACHED, ADIE_CODEC_DIGITAL_ANALOG_READY}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x8A, 0x10, 0x10)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x0D, 0xFF, 0x00)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x83, 0x14, 0x00)}, \
	{ADIE_CODEC_ACTION_STAGE_REACHED, ADIE_CODEC_ANALOG_OFF}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x11, 0xff, 0x00)}, \
	{ADIE_CODEC_ACTION_STAGE_REACHED, ADIE_CODEC_DIGITAL_OFF} }

#define TTY_HEADSET_MONO_TX_16000_OSR_256 TTY_HEADSET_MONO_TX_8000_OSR_256

#define TTY_HEADSET_MONO_RX_CLASS_D_8000_OSR_256 \
	{{ADIE_CODEC_ACTION_STAGE_REACHED, ADIE_CODEC_FLASH_IMAGE}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3B, 0xFF, 0xac)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3C, 0xFF, 0xac)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x80, 0x02, 0x02)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x80, 0x02, 0x00)}, \
	{ADIE_CODEC_ACTION_STAGE_REACHED, ADIE_CODEC_DIGITAL_READY}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x81, 0xFF, 0x0e)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x23, 0xf8, 0xA8)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x28, 0xfF, 0xCA)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x29, 0xfF, 0xCA)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x39, 0x40, 0x40)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x40, 0xFF, 0x08)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x41, 0xFF, 0x08)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x42, 0xFF, 0xFF)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x43, 0xFF, 0xc4)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x44, 0xFF, 0x37)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3E, 0xFF, 0xD5)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3F, 0xFF, 0x0f)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x45, 0xFF, 0xff)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x46, 0xFF, 0x77)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x47, 0xFF, 0xc4)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x48, 0xFF, 0x37)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x49, 0xFF, 0xff)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x4a, 0xFF, 0x77)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x83, 0x23, 0x23)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x33, 0x80, 0x80)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x34, 0x0a, 0x0a)}, \
	{ADIE_CODEC_ACTION_DELAY_WAIT, 0x186A0}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x34, 0x05, 0x04)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3B, 0xFF, 0x98)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3C, 0xFF, 0x98)}, \
	{ADIE_CODEC_ACTION_DELAY_WAIT, 0x3E8}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3B, 0xFF, 0x88)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3C, 0xFF, 0x88)}, \
	{ADIE_CODEC_ACTION_DELAY_WAIT, 0x3E8}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3B, 0xFF, 0x78)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3C, 0xFF, 0x78)}, \
	{ADIE_CODEC_ACTION_DELAY_WAIT, 0x3E8}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3B, 0xFF, 0x68)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3C, 0xFF, 0x68)}, \
	{ADIE_CODEC_ACTION_DELAY_WAIT, 0x3E8}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3B, 0xFF, 0x58)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3C, 0xFF, 0x58)}, \
	{ADIE_CODEC_ACTION_DELAY_WAIT, 0x3E8}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3B, 0xFF, 0x48)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3C, 0xFF, 0x48)}, \
	{ADIE_CODEC_ACTION_DELAY_WAIT, 0x3E8}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3B, 0xFF, 0x38)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3C, 0xFF, 0x38)}, \
	{ADIE_CODEC_ACTION_DELAY_WAIT, 0x3E8}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3B, 0xFF, 0x28)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3C, 0xFF, 0x28)}, \
	{ADIE_CODEC_ACTION_DELAY_WAIT, 0x3E8}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3B, 0xFF, 0x18)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3C, 0xFF, 0x18)}, \
	{ADIE_CODEC_ACTION_DELAY_WAIT, 0x3E8}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3B, 0xFF, 0x08)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3C, 0xFF, 0x08)}, \
	{ADIE_CODEC_ACTION_DELAY_WAIT, 0x3E8}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3B, 0xFF, 0x01)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3C, 0xFF, 0x01)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x84, 0xff, 0xF4)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x85, 0xff, 0x00)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x8A, 0x0f, 0x0c)}, \
	{ADIE_CODEC_ACTION_STAGE_REACHED, ADIE_CODEC_DIGITAL_ANALOG_READY}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x8a, 0x03, 0x03)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3b, 0xFC, 0xAC)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3c, 0xFC, 0xAC)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x34, 0x0f, 0x00)}, \
	{ADIE_CODEC_ACTION_STAGE_REACHED, ADIE_CODEC_ANALOG_OFF}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x33, 0x80, 0x00)}, \
	{ADIE_CODEC_ACTION_STAGE_REACHED, ADIE_CODEC_DIGITAL_OFF} }

#define TTY_HEADSET_MONO_RX_CLASS_D_16000_OSR_256 \
	{{ADIE_CODEC_ACTION_STAGE_REACHED, ADIE_CODEC_FLASH_IMAGE}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3B, 0xFF, 0xac)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3C, 0xFF, 0xac)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x80, 0x02, 0x02)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x80, 0x02, 0x00)}, \
	{ADIE_CODEC_ACTION_STAGE_REACHED, ADIE_CODEC_DIGITAL_READY}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x81, 0xFF, 0x0e)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x23, 0xf8, 0xA8)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x28, 0xfF, 0xCA)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x29, 0xfF, 0xCA)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x39, 0x40, 0x40)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x40, 0xFF, 0x08)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x41, 0xFF, 0x08)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x42, 0xFF, 0xFF)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x43, 0xFF, 0xd4)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x44, 0xFF, 0x37)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3E, 0xFF, 0xD5)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3F, 0xFF, 0x0f)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x45, 0xFF, 0xff)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x46, 0xFF, 0x77)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x47, 0xFF, 0xd4)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x48, 0xFF, 0x37)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x49, 0xFF, 0xff)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x4a, 0xFF, 0x77)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x83, 0x23, 0x23)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x33, 0x80, 0x80)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x34, 0x0a, 0x0a)}, \
	{ADIE_CODEC_ACTION_DELAY_WAIT, 0x186A0}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x34, 0x05, 0x04)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3B, 0xFF, 0x98)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3C, 0xFF, 0x98)}, \
	{ADIE_CODEC_ACTION_DELAY_WAIT, 0x3E8}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3B, 0xFF, 0x88)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3C, 0xFF, 0x88)}, \
	{ADIE_CODEC_ACTION_DELAY_WAIT, 0x3E8}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3B, 0xFF, 0x78)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3C, 0xFF, 0x78)}, \
	{ADIE_CODEC_ACTION_DELAY_WAIT, 0x3E8}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3B, 0xFF, 0x68)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3C, 0xFF, 0x68)}, \
	{ADIE_CODEC_ACTION_DELAY_WAIT, 0x3E8}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3B, 0xFF, 0x58)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3C, 0xFF, 0x58)}, \
	{ADIE_CODEC_ACTION_DELAY_WAIT, 0x3E8}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3B, 0xFF, 0x48)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3C, 0xFF, 0x48)}, \
	{ADIE_CODEC_ACTION_DELAY_WAIT, 0x3E8}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3B, 0xFF, 0x38)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3C, 0xFF, 0x38)}, \
	{ADIE_CODEC_ACTION_DELAY_WAIT, 0x3E8}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3B, 0xFF, 0x28)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3C, 0xFF, 0x28)}, \
	{ADIE_CODEC_ACTION_DELAY_WAIT, 0x3E8}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3B, 0xFF, 0x18)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3C, 0xFF, 0x18)}, \
	{ADIE_CODEC_ACTION_DELAY_WAIT, 0x3E8}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3B, 0xFF, 0x08)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3C, 0xFF, 0x08)}, \
	{ADIE_CODEC_ACTION_DELAY_WAIT, 0x3E8}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3B, 0xFF, 0x01)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3C, 0xFF, 0x01)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x84, 0xff, 0xF4)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x85, 0xff, 0x00)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x8A, 0x0f, 0x0c)}, \
	{ADIE_CODEC_ACTION_STAGE_REACHED, ADIE_CODEC_DIGITAL_ANALOG_READY}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x8a, 0x03, 0x03)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3b, 0xFC, 0xAC)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3c, 0xFC, 0xAC)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x34, 0x0f, 0x00)}, \
	{ADIE_CODEC_ACTION_STAGE_REACHED, ADIE_CODEC_ANALOG_OFF}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x33, 0x80, 0x00)}, \
	{ADIE_CODEC_ACTION_STAGE_REACHED, ADIE_CODEC_DIGITAL_OFF} }

#if 0  //20110419 jhsong : tty config mono -> stereo
#define TTY_HEADSET_MONO_RX_CLASS_D_48000_OSR_256 \
	{{ADIE_CODEC_ACTION_STAGE_REACHED, ADIE_CODEC_FLASH_IMAGE}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3B, 0xFF, 0xac)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3C, 0xFF, 0xac)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x80, 0x02, 0x02)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x80, 0x02, 0x00)}, \
	{ADIE_CODEC_ACTION_STAGE_REACHED, ADIE_CODEC_DIGITAL_READY}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x81, 0xFF, 0x0e)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x23, 0xf8, 0xA8)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x28, 0xfF, 0xCA)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x29, 0xfF, 0xCA)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x39, 0x40, 0x40)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x40, 0xFF, 0x08)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x41, 0xFF, 0x08)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x42, 0xFF, 0xBB)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x43, 0xFF, 0xF2)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x44, 0xFF, 0x37)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3E, 0xFF, 0xD5)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3F, 0xFF, 0x0f)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x45, 0xFF, 0xff)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x46, 0xFF, 0x77)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x47, 0xFF, 0xF2)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x48, 0xFF, 0x37)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x49, 0xFF, 0xff)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x4a, 0xFF, 0x77)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x83, 0x23, 0x23)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x33, 0x80, 0x80)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x34, 0x0a, 0x0a)}, \
	{ADIE_CODEC_ACTION_DELAY_WAIT, 0x186A0}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x34, 0x05, 0x04)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3B, 0xFF, 0x98)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3C, 0xFF, 0x98)}, \
	{ADIE_CODEC_ACTION_DELAY_WAIT, 0x3E8}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3B, 0xFF, 0x88)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3C, 0xFF, 0x88)}, \
	{ADIE_CODEC_ACTION_DELAY_WAIT, 0x3E8}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3B, 0xFF, 0x78)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3C, 0xFF, 0x78)}, \
	{ADIE_CODEC_ACTION_DELAY_WAIT, 0x3E8}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3B, 0xFF, 0x68)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3C, 0xFF, 0x68)}, \
	{ADIE_CODEC_ACTION_DELAY_WAIT, 0x3E8}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3B, 0xFF, 0x58)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3C, 0xFF, 0x58)}, \
	{ADIE_CODEC_ACTION_DELAY_WAIT, 0x3E8}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3B, 0xFF, 0x48)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3C, 0xFF, 0x48)}, \
	{ADIE_CODEC_ACTION_DELAY_WAIT, 0x3E8}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3B, 0xFF, 0x38)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3C, 0xFF, 0x38)}, \
	{ADIE_CODEC_ACTION_DELAY_WAIT, 0x3E8}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3B, 0xFF, 0x28)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3C, 0xFF, 0x28)}, \
	{ADIE_CODEC_ACTION_DELAY_WAIT, 0x3E8}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3B, 0xFF, 0x18)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3C, 0xFF, 0x18)}, \
	{ADIE_CODEC_ACTION_DELAY_WAIT, 0x3E8}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3B, 0xFF, 0x08)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3C, 0xFF, 0x08)}, \
	{ADIE_CODEC_ACTION_DELAY_WAIT, 0x3E8}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3B, 0xFF, 0x01)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3C, 0xFF, 0x01)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x84, 0xff, 0xF4)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x85, 0xff, 0x00)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x8A, 0x0f, 0x0c)}, \
	{ADIE_CODEC_ACTION_STAGE_REACHED, ADIE_CODEC_DIGITAL_ANALOG_READY}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x8a, 0x03, 0x03)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3b, 0xFC, 0xAC)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3c, 0xFC, 0xAC)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x34, 0x0f, 0x00)}, \
	{ADIE_CODEC_ACTION_STAGE_REACHED, ADIE_CODEC_ANALOG_OFF}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x33, 0x80, 0x00)}, \
	{ADIE_CODEC_ACTION_STAGE_REACHED, ADIE_CODEC_DIGITAL_OFF} }
#else
#define TTY_HEADSET_MONO_RX_CLASS_D_48000_OSR_256 \
	{{ADIE_CODEC_ACTION_STAGE_REACHED, ADIE_CODEC_FLASH_IMAGE}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x80, 0x02, 0x02)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x80, 0x02, 0x00)}, \
	{ADIE_CODEC_ACTION_STAGE_REACHED, ADIE_CODEC_DIGITAL_READY}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3B, 0xFF, 0xac)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3C, 0xFF, 0xac)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x81, 0xFF, 0x4e)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x04, 0xff, 0xBC)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x24, 0x6F, 0x64)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x25, 0x0F, 0x0B)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x26, 0xfc, 0xfc)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x38, 0xff, 0x80)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3A, 0xFF, 0x2b)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x83, 0x23, 0x23)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x33, 0x80, 0x80)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x34, 0xf0, 0xa0)}, \
	{ADIE_CODEC_ACTION_DELAY_WAIT, 0x186A0}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x34, 0xf0, 0xf0)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x23, 0xff, 0x20)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3B, 0xFF, 0x98)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3C, 0xFF, 0x98)}, \
	{ADIE_CODEC_ACTION_DELAY_WAIT, 0x3E8}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3B, 0xFF, 0x88)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3C, 0xFF, 0x88)}, \
	{ADIE_CODEC_ACTION_DELAY_WAIT, 0x3E8}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3B, 0xFF, 0x78)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3C, 0xFF, 0x78)}, \
	{ADIE_CODEC_ACTION_DELAY_WAIT, 0x3E8}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3B, 0xFF, 0x68)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3C, 0xFF, 0x68)}, \
	{ADIE_CODEC_ACTION_DELAY_WAIT, 0x3E8}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3B, 0xFF, 0x58)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3C, 0xFF, 0x58)}, \
	{ADIE_CODEC_ACTION_DELAY_WAIT, 0x3E8}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3B, 0xFF, 0x48)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3C, 0xFF, 0x48)}, \
	{ADIE_CODEC_ACTION_DELAY_WAIT, 0x3E8}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3B, 0xFF, 0x38)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3C, 0xFF, 0x38)}, \
	{ADIE_CODEC_ACTION_DELAY_WAIT, 0x3E8}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3B, 0xFF, 0x28)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3C, 0xFF, 0x28)}, \
	{ADIE_CODEC_ACTION_DELAY_WAIT, 0x3E8}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3B, 0xFF, 0x18)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3C, 0xFF, 0x18)}, \
	{ADIE_CODEC_ACTION_DELAY_WAIT, 0x3E8}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3B, 0xFF, 0x10)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3C, 0xFF, 0x10)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x84, 0xff, 0x00)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x85, 0xff, 0x00)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x8A, 0x0f, 0x0c)}, \
	{ADIE_CODEC_ACTION_STAGE_REACHED, ADIE_CODEC_DIGITAL_ANALOG_READY}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x8a, 0x03, 0x03)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3b, 0xFF, 0xAC)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3c, 0xFF, 0xAC)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x34, 0xf0, 0x00)}, \
	{ADIE_CODEC_ACTION_STAGE_REACHED, ADIE_CODEC_ANALOG_OFF}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x33, 0x80, 0x00)}, \
	{ADIE_CODEC_ACTION_STAGE_REACHED, ADIE_CODEC_DIGITAL_OFF} }
#endif

/***************************************************************************\
				FFA
\***************************************************************************/
#define HANDSET_RX_8000_OSR_256_FFA \
	{{ADIE_CODEC_ACTION_STAGE_REACHED, ADIE_CODEC_FLASH_IMAGE},  \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x80,  0x02,  0x02)},  \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x80, 0x02, 0x00)}, \
	{ADIE_CODEC_ACTION_STAGE_REACHED, ADIE_CODEC_DIGITAL_READY}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x24, 0x6F, 0x44)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x04, 0xff, 0x8C)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x81, 0xFF, 0x4E)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x25, 0x0F, 0x0b)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x26, 0xfc, 0xfc)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x36, 0xc0, 0x80)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3A, 0xFF, 0x2B)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x23, 0xff, 0x20)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3d, 0xFF, 0xD5)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x83, 0x21, 0x21)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x33, 0x80, 0x80)}, \
	{ADIE_CODEC_ACTION_DELAY_WAIT, 0x2710}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x33, 0x40, 0x40)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x84, 0xff, 0x00)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x8A, 0x05, 0x04)}, \
	{ADIE_CODEC_ACTION_STAGE_REACHED, ADIE_CODEC_DIGITAL_ANALOG_READY}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x8a, 0x01, 0x01)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x36, 0xc0, 0x00)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x33, 0x40, 0x00)}, \
	{ADIE_CODEC_ACTION_STAGE_REACHED, ADIE_CODEC_ANALOG_OFF}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x33, 0x80, 0x00)}, \
	{ADIE_CODEC_ACTION_STAGE_REACHED, ADIE_CODEC_DIGITAL_OFF} }

#define HANDSET_RX_16000_OSR_256_FFA HANDSET_RX_8000_OSR_256_FFA

#define HANDSET_RX_48000_OSR_64_FFA \
	{{ADIE_CODEC_ACTION_STAGE_REACHED, ADIE_CODEC_FLASH_IMAGE}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x80, 0x02, 0x02)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x80, 0x02, 0x00)}, \
	{ADIE_CODEC_ACTION_STAGE_REACHED, ADIE_CODEC_DIGITAL_READY}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x24, 0x6F, 0x47)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x04, 0xfF, 0x8C)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x81, 0xFF, 0x42)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x25, 0x0F, 0x0b)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x26, 0xfc, 0xfc)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x36, 0xc0, 0x80)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3A, 0xFF, 0x2B)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x23, 0xff, 0x20)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3d, 0xFF, 0xD5)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x83, 0x21, 0x21)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x33, 0x80, 0x80)}, \
	{ADIE_CODEC_ACTION_DELAY_WAIT, 0x2710}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x33, 0x40, 0x40)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x84, 0xff, 0x00)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x8A, 0x05, 0x04)}, \
	{ADIE_CODEC_ACTION_STAGE_REACHED, ADIE_CODEC_DIGITAL_ANALOG_READY}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x8a, 0x01, 0x01)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x36, 0xc0, 0x00)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x33, 0x40, 0x00)}, \
	{ADIE_CODEC_ACTION_STAGE_REACHED, ADIE_CODEC_ANALOG_OFF}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x33, 0x80, 0x00)}, \
	{ADIE_CODEC_ACTION_STAGE_REACHED, ADIE_CODEC_DIGITAL_OFF} }

#define HANDSET_RX_48000_OSR_256_FFA \
	{{ADIE_CODEC_ACTION_STAGE_REACHED, ADIE_CODEC_FLASH_IMAGE}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x80, 0x02, 0x02)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x80, 0x02, 0x00)}, \
	{ADIE_CODEC_ACTION_STAGE_REACHED, ADIE_CODEC_DIGITAL_READY}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x24, 0x6F, 0x44)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x04, 0xfF, 0x8C)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x81, 0xFF, 0x4e)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x25, 0x0F, 0x0b)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x26, 0xfc, 0xfc)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x36, 0xc0, 0x80)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3A, 0xFF, 0x2B)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x23, 0xff, 0x20)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x3d, 0xFF, 0x55)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x83, 0x21, 0x21)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x33, 0x80, 0x80)}, \
	{ADIE_CODEC_ACTION_DELAY_WAIT, 0x2710}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x33, 0x40, 0x40)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x84, 0xff, 0x00)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x8A, 0x05, 0x04)}, \
	{ADIE_CODEC_ACTION_STAGE_REACHED, ADIE_CODEC_DIGITAL_ANALOG_READY}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x8a, 0x01, 0x01)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x36, 0xc0, 0x00)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x33, 0x40, 0x00)}, \
	{ADIE_CODEC_ACTION_STAGE_REACHED, ADIE_CODEC_ANALOG_OFF}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x33, 0x80, 0x00)}, \
	{ADIE_CODEC_ACTION_STAGE_REACHED, ADIE_CODEC_DIGITAL_OFF} }

#define HANDSET_TX_8000_OSR_256_FFA \
	{{ADIE_CODEC_ACTION_STAGE_REACHED, ADIE_CODEC_FLASH_IMAGE}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x80, 0x01, 0x01)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x80, 0x01, 0x00)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x8A, 0x30, 0x30)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x11, 0xfc, 0xfc)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x13, 0xfc, 0x58)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x14, 0xff, 0x65)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x15, 0xff, 0x64)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x82, 0xff, 0x5E)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x10, 0xFF, 0x68)}, \
	{ADIE_CODEC_ACTION_STAGE_REACHED, ADIE_CODEC_DIGITAL_READY}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x0D, 0xF0, 0xd0)}, \
	{ADIE_CODEC_ACTION_DELAY_WAIT, 0xbb8}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x83, 0x14, 0x14)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x8b, 0xff, 0xCE)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x8c, 0x03, 0x02)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x86, 0xff, 0x00)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x8A, 0x50, 0x40)}, \
	{ADIE_CODEC_ACTION_STAGE_REACHED, ADIE_CODEC_DIGITAL_ANALOG_READY}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x8A, 0x10, 0x30)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x0D, 0xFF, 0x00)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x83, 0x14, 0x00)}, \
	{ADIE_CODEC_ACTION_STAGE_REACHED, ADIE_CODEC_ANALOG_OFF}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x11, 0xff, 0x00)}, \
	{ADIE_CODEC_ACTION_STAGE_REACHED, ADIE_CODEC_DIGITAL_OFF} }

#define HANDSET_TX_16000_OSR_256_FFA HANDSET_TX_8000_OSR_256_FFA

#define HANDSET_TX_48000_OSR_256_FFA \
	{{ADIE_CODEC_ACTION_STAGE_REACHED, ADIE_CODEC_FLASH_IMAGE}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x80, 0x01, 0x01)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x80, 0x01, 0x00)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x8A, 0x30, 0x30)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x11, 0xfc, 0xfc)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x13, 0xfc, 0x58)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x14, 0xff, 0x65)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x15, 0xff, 0x64)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x82, 0xff, 0x5A)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x10, 0xFF, 0x68)}, \
	{ADIE_CODEC_ACTION_STAGE_REACHED, ADIE_CODEC_DIGITAL_READY}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x0D, 0xF0, 0xd0)}, \
	{ADIE_CODEC_ACTION_DELAY_WAIT, 0xbb8}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x83, 0x14, 0x14)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x8b, 0xff, 0xCE)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x8c, 0x03, 0x02)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x86, 0xff, 0x00)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x8A, 0x50, 0x40)}, \
	{ADIE_CODEC_ACTION_STAGE_REACHED, ADIE_CODEC_DIGITAL_ANALOG_READY}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x8A, 0x10, 0x30)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x0D, 0xFF, 0x00)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x83, 0x14, 0x00)}, \
	{ADIE_CODEC_ACTION_STAGE_REACHED, ADIE_CODEC_ANALOG_OFF}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x11, 0xff, 0x00)}, \
	{ADIE_CODEC_ACTION_STAGE_REACHED, ADIE_CODEC_DIGITAL_OFF} }



#endif /* __MARIMBA_PROFILE_H__ */




