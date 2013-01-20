

/************************************************************************************************
** Includes
*************************************************************************************************/


#include <mach/board.h>
#include <mach/vreg.h>

#include <linux/kernel.h>
#include <linux/i2c.h>
#include <linux/delay.h>
#include <linux/fs.h>
#include <linux/miscdevice.h>
#include <mach/gpio.h>
#include <asm/ioctls.h>
#include <linux/uaccess.h>
#include <linux/gpio.h>

#include "snd_audience_a2020_laser2.h"
#include "snd_sub_fab2200_laser2.h"
#include "sky_snd_extampctrl_laser2.h"

#undef FEATURE_SKY_SND_EXTAMPCTRL //added by elecjang

static struct i2c_client *audience_a2020_i2c_client = NULL;

#define POLLING_LOOP_COUNT 		3

#define GPIO_NR_WAKEUP_N 		87
#define GPIO_NR_RESET_N 		64
#define GPIO_AUDIENCE_PLL 		85	// from 12Mhz to 26Mhz, UART_SOUT PIN

#ifndef byte
#define byte u8
#endif

// A2020_CMD_BOOTLOAD_INITIATE is needed when reinitialize the system without resetting.
/* === not used === */
//SW Reset
#define A2020_CMD_SW_RESET 					0x8002
#define A2020_CMD_SW_RESET_IMMEDIATE 		0x0000
#define A2020_CMD_SW_RESET_DELAYED 			0x0001
/* === not used === */
//Bootload Initiate
#define A2020_CMD_BOOTLOAD_INITIATE 		0x8003
#define A2020_CMD_BOOTLOAD_INITIATE_ACK 	0x80030000

#define A2020_CMD_BOOT 						0x0001
#define A2020_CMD_BOOT_ACK 					0x0101
#define A2020_CMD_BOOT_ACK_A2020 			0x01

//Sync Control
#define A2020_CMD_SYNC 						0x8000
#define A2020_CMD_SYNC_ACK 					0x80000000L
#define A2020_CMD_SYNC_ONLY_ACK 			0x80000005L
#define A2020_CMD_SYNC_MODE_POLLING 		0x0000
#define A2020_CMD_SYNC_MODE_ACTIVE_LOW 		0x0001
#define A2020_CMD_SYNC_MODE_ACTIVE_HIGH 	0x0002
#define A2020_CMD_SYNC_MODE_FALLING_EDGE 	0x0003
#define A2020_CMD_SYNC_MODE_RISING_EDGE 	0x0004

//Set Power State
#define A2020_CMD_SLEEP 					0x8010
#define A2020_CMD_SLEEP_VALUE 				0x0001
#define A2020_CMD_SLEEP_ACK 				0x80100001L

//Get/Set Device Parameter
#define A2020_CMD_GET_DEVICE_PARAMID 		0x800B
#define A2020_CMD_DEVICE_PARAMID 			0x800C
#define A2020_CMD_DEVICE_PARAM 				0x800D

//Get/Set Algorithm Parameter
#define A2020_CMD_GET_ALGO_PARAMID 			0x8016
#define A2020_CMD_ALGO_PARAMID 				0x8017
#define A2020_CMD_ALGO_PARAM 				0x8018

//Get/Set Digital Input Gain
#define A2020_CMD_SET_DIGI_INPUT_GAIN 		0x801B
#define A2020_CMD_GET_DIGI_INPUT_GAIN 		0x801A

//Get/Set Digital Output Gain
#define A2020_CMD_SET_DIGI_OUTPUT_GAIN 		0x8015
#define A2020_CMD_GET_DIGI_OUTPUT_GAIN 		0x801D

//Select Audio Route ID
#define A2020_CMD_SET_AUDIO_ROUTING 		0x8026
#define A2020_CMD_GET_AUDIO_ROUTING 		0x8027

// Voice Processing Enable/Disable
#define A2020_CMD_VOICE_PROCESSING 			0x801C

#define ABS(a) (((a)>=(0))?(a):-(a))


#if 0 //by elecjang 20110711, for large size firmware, it must be separated
static const u8 a2020_firmware_image[] =
{
#include "Audience_bin_laser2.txt"
};
#else
static const u8 a2020_firmware_image1[] =
{
#include "Audience_bin_laser2_1st.txt"
};

static const u8 a2020_firmware_image2[] =
{
#include "Audience_bin_laser2_2st.txt"
};
#endif

int bBootSequence = 0;
int bVPonoff = 0;
a2020_talk_mode curr_talk_mode 			= MAX_TALK;
a2020_talk_mode prev_talk_mode 			= MAX_TALK;
a2020_mic_mode curr_mic_mode 			= BOTH_MIC_ON;
	
static a2020_status_type a2020_mode 	= A2020_MAX_MODE;
static a2020_sleep_status sleep_status 	= A2020_SLEEP_NONE;

typedef struct 
{
    u16 cmd;
    u16 value;
} a2020_reset_param;

//Software Reset
static a2020_reset_param sw_reset_param[] = 
{
    { A2020_CMD_SW_RESET, A2020_CMD_SW_RESET_DELAYED }, //Software Delayed Reset
    { 0xFFFF, 0xFFFF },
};

typedef struct 
{
    u16 cmd1;
    u16 id;
    u16 cmd2;
    u16 value;
} a2020_device_param;

//Device Param	//by elecjang 20110711 firmware
static a2020_device_param device_param[] = {
    { A2020_CMD_DEVICE_PARAMID	,0x1001	,A2020_CMD_DEVICE_PARAM	,0x0002 }, //0x800C:SetDeviceParmID, 0x10:ADC0, 0x01:ADC PGA Gain, 0x800D:SetDeviceParm, 0x0002: +6.02dB
    { A2020_CMD_DEVICE_PARAMID	,0x1101	,A2020_CMD_DEVICE_PARAM	,0x0002 }, //0x800C:SetDeviceParmID, 0x11:ADC1, 0x01:ADC PGA Gain, 0x800D:SetDeviceParm, 0x0002: +6.02dB
    { A2020_CMD_DEVICE_PARAMID	,0x0A02	,A2020_CMD_DEVICE_PARAM	,0x0001 }, //0x800C:SetDeviceParmID, 0x0A:PCM0, 0x02:PCM DelFromFsTx, 0x800D:SetDeviceParm, 0x0001:(1 clock)
    { A2020_CMD_DEVICE_PARAMID	,0x0A03	,A2020_CMD_DEVICE_PARAM	,0x0001 }, //0x800C:SetDeviceParmID, 0x0A:PCM0, 0x03:PCM DelFromFsRx, 0x800D:SetDeviceParm, 0x0001:(1 clock)
    { 0xFFFF					,0xFFFF	,0xFFFF					,0xFFFF },
};


typedef struct 
{
    u16 cmd1;
    u16 id;
    u16 cmd2;
    u16 value;
} a2020_algo_param;


//Desktop Vehicular(DV), Set Algorithm Parameter 	//by elecjang 20110716 Macro
static a2020_algo_param algo_dv_param[] = {
    { A2020_CMD_ALGO_PARAMID	,0x0002		,A2020_CMD_ALGO_PARAM	,0x0002 },	//0x8017:SetAlgorithmParmID, 0x0002:Microphone Configuration, 0x8018:SetAlgorithmParm, 0x0002:1-mic Desktop/Vehicle (DV)
    { A2020_CMD_ALGO_PARAMID	,0x004B		,A2020_CMD_ALGO_PARAM	,0x0006 },	//0x8017:SetAlgorithmParmID, 0x004B:Tx Noise Suppression Level, 0x8018:SetAlgorithmParm, 0x0006:Level 6
    { A2020_CMD_ALGO_PARAMID	,0x0015		,A2020_CMD_ALGO_PARAM	,0x0000 },	//0x8017:SetAlgorithmParmID, 0x0015:Side Tone Mode, 0x8018:SetAlgorithmParm, 0x0000:Off
    { A2020_CMD_ALGO_PARAMID	,0x0016		,A2020_CMD_ALGO_PARAM	,0xFFEB },	//0x8017:SetAlgorithmParmID, 0x0016:   Side Tone Gain (dB), 0x8018:SetAlgorithmParm, 0xFFEB:(-21 dB)
    { A2020_CMD_ALGO_PARAMID	,0x0003		,A2020_CMD_ALGO_PARAM	,0x0001 },	//0x8017:SetAlgorithmParmID, 0x0003:AEC Mode, 0x8018:SetAlgorithmParm, 0x0001:AEC On (auto select mode)
    	
    { A2020_CMD_ALGO_PARAMID	,0x0012		,A2020_CMD_ALGO_PARAM	,0x0005 },	//0x8017:SetAlgorithmParmID, 0x0012:   Downlink Speaker Volume, 0x8018:SetAlgorithmParm, 0x0005:(5 dB)
    { A2020_CMD_ALGO_PARAMID	,0x0023		,A2020_CMD_ALGO_PARAM	,0x0000 },	//0x8017:SetAlgorithmParmID, 0x0023:   Use AEC Comfort Noise Fill, 0x8018:SetAlgorithmParm, 0x0000:No
    { A2020_CMD_ALGO_PARAMID	,0x0034		,A2020_CMD_ALGO_PARAM	,0x0016 },	//0x8017:SetAlgorithmParmID, 0x0034:   Echo Suppression Enhancement, 0x8018:SetAlgorithmParm, 0x0016:(22 dB)
    { A2020_CMD_ALGO_PARAMID	,0x0004		,A2020_CMD_ALGO_PARAM	,0x0000 },	//0x8017:SetAlgorithmParmID, 0x0004:Use AGC, 0x8018:SetAlgorithmParm, 0x0000:No
    { A2020_CMD_ALGO_PARAMID	,0x0028		,A2020_CMD_ALGO_PARAM	,0x0000 },	//0x8017:SetAlgorithmParmID, 0x0028:Use Rx AGC, 0x8018:SetAlgorithmParm, 0x0000:No

    { A2020_CMD_ALGO_PARAMID	,0x0009		,A2020_CMD_ALGO_PARAM	,0x0001 },	//0x8017:SetAlgorithmParmID, 0x0009:Speaker Enhancement Mode, 0x8018:SetAlgorithmParm, 0x0001:SE On (auto select SE mode)
    { A2020_CMD_ALGO_PARAMID	,0x0025		,A2020_CMD_ALGO_PARAM	,0xFFF7 },	//0x8017:SetAlgorithmParmID, 0x0025:   SE Noise Estimate Adj, 0x8018:SetAlgorithmParm, 0xFFF7:(-9 dB)
    { A2020_CMD_ALGO_PARAMID	,0x003D		,A2020_CMD_ALGO_PARAM	,0x000B },	//0x8017:SetAlgorithmParmID, 0x003D:   SE Max Gain (dB), 0x8018:SetAlgorithmParm, 0x000B:(11 dB)
    { A2020_CMD_ALGO_PARAMID	,0x000E		,A2020_CMD_ALGO_PARAM	,0x0001 },	//0x8017:SetAlgorithmParmID, 0x000E:Far End Noise Suppression, 0x8018:SetAlgorithmParm, 0x0001:On (auto select mode)
    { A2020_CMD_ALGO_PARAMID	,0x004C		,A2020_CMD_ALGO_PARAM	,0x0006 },	//0x8017:SetAlgorithmParmID, 0x004C:   Rx Noise Suppression Level, 0x8018:SetAlgorithmParm, 0x0006:Level 6

    { A2020_CMD_ALGO_PARAMID	,0x0040		,A2020_CMD_ALGO_PARAM	,0xFFF7 },	//0x8017:SetAlgorithmParmID, 0x0040:Tx-out Limiter Max Level (dB), 0x8018:SetAlgorithmParm, 0xFFF7:(-9 dB)
    { A2020_CMD_ALGO_PARAMID	,0x0020		,A2020_CMD_ALGO_PARAM	,0x0000 },	//0x8017:SetAlgorithmParmID, 0x0020:Tx PostEq Mode, 0x8018:SetAlgorithmParm, 0x0000:Off
    { A2020_CMD_ALGO_PARAMID	,0x001F		,A2020_CMD_ALGO_PARAM	,0x0000 },	//0x8017:SetAlgorithmParmID, 0x001F:Rx PostEq Mode, 0x8018:SetAlgorithmParm, 0x0000:Off
	{ A2020_CMD_ALGO_PARAMID	,0x0030 	,A2020_CMD_ALGO_PARAM	,0x0000 },	//0x8017:SetAlgorithmParmID, 0x0030:Tx MBC Mode, 0x8018:SetAlgorithmParm, 0x0000:Off
	{ A2020_CMD_ALGO_PARAMID	,0x0031 	,A2020_CMD_ALGO_PARAM	,0x0001 },	//0x8017:SetAlgorithmParmID, 0x0031:Rx MBC Mode, 0x8018:SetAlgorithmParm, 0x0001:On
    { A2020_CMD_ALGO_PARAMID	,0x001A		,A2020_CMD_ALGO_PARAM	,0x0000 },	//0x8017:SetAlgorithmParmID, 0x001A:Use Tx ComfortNoise, 0x8018:SetAlgorithmParm, 0x0000:No

    { A2020_CMD_ALGO_PARAMID	,0x001B		,A2020_CMD_ALGO_PARAM	,0xFFAC },	//0x8017:SetAlgorithmParmID, 0x001B:   Tx ComfortNoise gain (dB), 0x8018:SetAlgorithmParm, 0xFFAC:(-84 dB)
    { A2020_CMD_ALGO_PARAMID	,0x004F		,A2020_CMD_ALGO_PARAM	,0x0000 },	//0x8017:SetAlgorithmParmID, 0x004F:Use Bandwidth Expansion, 0x8018:SetAlgorithmParm, 0x0000:No
    { A2020_CMD_ALGO_PARAMID	,0x0050		,A2020_CMD_ALGO_PARAM	,0x0000 },	//0x8017:SetAlgorithmParmID, 0x0050:  Gain High Band (dB), 0x8018:SetAlgorithmParm, 0x0000:(0 dB)
    { A2020_CMD_ALGO_PARAMID	,0x0051		,A2020_CMD_ALGO_PARAM	,0x0000 },	//0x8017:SetAlgorithmParmID, 0x0051:  Max SNR (dB), 0x8018:SetAlgorithmParm, 0x0000:(0 dB)
    { A2020_CMD_ALGO_PARAMID	,0x0052		,A2020_CMD_ALGO_PARAM	,0x0000 },	//0x8017:SetAlgorithmParmID, 0x0052:  Use BWE Post EQ, 0x8018:SetAlgorithmParm, 0x0000:No

    { 0xFFFF					,0xFFFF		,0xFFFF					,0xFFFF },
};


//DV VP Off, Set Algorithm Parameter 
static a2020_algo_param algo_dv_VPOFF_param[] = {
    { A2020_CMD_ALGO_PARAMID	, 0x0002	, A2020_CMD_ALGO_PARAM	, 0x0002 },
    { A2020_CMD_ALGO_PARAMID	, 0x0000	, A2020_CMD_ALGO_PARAM	, 0x0001 },
    { A2020_CMD_ALGO_PARAMID	, 0x0003	, A2020_CMD_ALGO_PARAM	, 0x0001 },
    { A2020_CMD_ALGO_PARAMID	, 0x0012	, A2020_CMD_ALGO_PARAM	, 0x0003 },
    { A2020_CMD_ALGO_PARAMID	, 0x0023	, A2020_CMD_ALGO_PARAM	, 0x0001 },
    	
    { A2020_CMD_ALGO_PARAMID	, 0x0034	, A2020_CMD_ALGO_PARAM	, 0x0000 },
    { A2020_CMD_ALGO_PARAMID	, 0x0009	, A2020_CMD_ALGO_PARAM	, 0x0000 },
    { A2020_CMD_ALGO_PARAMID	, 0x0025	, A2020_CMD_ALGO_PARAM	, 0x0000 },
    { A2020_CMD_ALGO_PARAMID	, 0x000E	, A2020_CMD_ALGO_PARAM	, 0x0001 },
    { A2020_CMD_ALGO_PARAMID	, 0x000F	, A2020_CMD_ALGO_PARAM	, 0x000A },

    { A2020_CMD_ALGO_PARAMID	, 0x0004	, A2020_CMD_ALGO_PARAM	, 0x0000 },
    { A2020_CMD_ALGO_PARAMID	, 0x0028	, A2020_CMD_ALGO_PARAM	, 0x0000 },
    { A2020_CMD_ALGO_PARAMID	, 0x0020	, A2020_CMD_ALGO_PARAM	, 0x0000 },
    { A2020_CMD_ALGO_PARAMID	, 0x001F	, A2020_CMD_ALGO_PARAM	, 0x0002 },
    { A2020_CMD_ALGO_PARAMID	, 0x001A	, A2020_CMD_ALGO_PARAM	, 0x0001 },

    { A2020_CMD_ALGO_PARAMID	, 0x001B	, A2020_CMD_ALGO_PARAM	, 0xFFBA },
    { 0xFFFF					, 0xFFFF	, 0xFFFF				, 0xFFFF },
};


//Cross Talk(CT), Set Algorithm Parameter 	//by elecjang 20110711 firmware
static a2020_algo_param algo_ct_param[] = {
    { A2020_CMD_ALGO_PARAMID	,0x0002		,A2020_CMD_ALGO_PARAM	,0x0000 },	//0x8017:SetAlgorithmParmID, 0x0002:Microphone Configuration, 0x8018:SetAlgorithmParm, 0x0000:2-mic Close Talk (CT)
    { A2020_CMD_ALGO_PARAMID	,0x004B		,A2020_CMD_ALGO_PARAM	,0x0006 },	//0x8017:SetAlgorithmParmID, 0x004B:Tx Noise Suppression Level, 0x8018:SetAlgorithmParm, 0x0006:Level 6
    { A2020_CMD_ALGO_PARAMID	,0x0015		,A2020_CMD_ALGO_PARAM	,0x0000 },	//0x8017:SetAlgorithmParmID, 0x0015:Side Tone Mode, 0x8018:SetAlgorithmParm, 0x0000:Off
    { A2020_CMD_ALGO_PARAMID	,0x0016		,A2020_CMD_ALGO_PARAM	,0xFFEB },	//0x8017:SetAlgorithmParmID, 0x0016:   Side Tone Gain (dB), 0x8018:SetAlgorithmParm, 0xFFEB:(-21 dB)
    { A2020_CMD_ALGO_PARAMID	,0x0003		,A2020_CMD_ALGO_PARAM	,0x0001 },	//0x8017:SetAlgorithmParmID, 0x0003:AEC Mode, 0x8018:SetAlgorithmParm, 0x0001:AEC On (auto select mode)
    	
    { A2020_CMD_ALGO_PARAMID	,0x0012		,A2020_CMD_ALGO_PARAM	,0xFFEC },	//0x8017:SetAlgorithmParmID, 0x0012:   Downlink Speaker Volume, 0x8018:SetAlgorithmParm, 0xFFEC:(-20 dB)
    { A2020_CMD_ALGO_PARAMID	,0x0023		,A2020_CMD_ALGO_PARAM	,0x0000 },	//0x8017:SetAlgorithmParmID, 0x0023:   Use AEC Comfort Noise Fill, 0x8018:SetAlgorithmParm, 0x0000:No
    { A2020_CMD_ALGO_PARAMID	,0x0034		,A2020_CMD_ALGO_PARAM	,0x0007 },	//0x8017:SetAlgorithmParmID, 0x0034:   Echo Suppression Enhancement, 0x8018:SetAlgorithmParm, 0x0007:(7 dB)
    { A2020_CMD_ALGO_PARAMID	,0x0004		,A2020_CMD_ALGO_PARAM	,0x0000 },	//0x8017:SetAlgorithmParmID, 0x0004:Use AGC, 0x8018:SetAlgorithmParm, 0x0000:No
    { A2020_CMD_ALGO_PARAMID	,0x0028		,A2020_CMD_ALGO_PARAM	,0x0000 },	//0x8017:SetAlgorithmParmID, 0x0028:Use Rx AGC, 0x8018:SetAlgorithmParm, 0x0000:No

    { A2020_CMD_ALGO_PARAMID	,0x0009		,A2020_CMD_ALGO_PARAM	,0x0001 },	//0x8017:SetAlgorithmParmID, 0x0009:Speaker Enhancement Mode, 0x8018:SetAlgorithmParm, 0x0001:SE On (auto select SE mode)
    { A2020_CMD_ALGO_PARAMID	,0x0025		,A2020_CMD_ALGO_PARAM	,0x0000 },	//0x8017:SetAlgorithmParmID, 0x0025:   SE Noise Estimate Adj, 0x8018:SetAlgorithmParm, 0x0000:(0 dB)
    { A2020_CMD_ALGO_PARAMID	,0x003D		,A2020_CMD_ALGO_PARAM	,0x000B },	//0x8017:SetAlgorithmParmID, 0x003D:   SE Max Gain (dB), 0x8018:SetAlgorithmParm, 0x000B:(11 dB)
    { A2020_CMD_ALGO_PARAMID	,0x000E		,A2020_CMD_ALGO_PARAM	,0x0001 },	//0x8017:SetAlgorithmParmID, 0x000E:Far End Noise Suppression, 0x8018:SetAlgorithmParm, 0x0001:On (auto select mode)
    { A2020_CMD_ALGO_PARAMID	,0x004C		,A2020_CMD_ALGO_PARAM	,0x0006 },	//0x8017:SetAlgorithmParmID, 0x004C:   Rx Noise Suppression Level, 0x8018:SetAlgorithmParm, 0x0006:Level 6

    { A2020_CMD_ALGO_PARAMID	,0x0040		,A2020_CMD_ALGO_PARAM	,0xFFF7 },	//0x8017:SetAlgorithmParmID, 0x0040:Tx-out Limiter Max Level (dB), 0x8018:SetAlgorithmParm, 0xFFF7:(-9 dB)
    { A2020_CMD_ALGO_PARAMID	,0x0020		,A2020_CMD_ALGO_PARAM	,0x0001 },	//0x8017:SetAlgorithmParmID, 0x0020:Tx PostEq Mode, 0x8018:SetAlgorithmParm, 0x0001:On
    { A2020_CMD_ALGO_PARAMID	,0x001F		,A2020_CMD_ALGO_PARAM	,0x0001 },	//0x8017:SetAlgorithmParmID, 0x001F:Rx PostEq Mode, 0x8018:SetAlgorithmParm, 0x0001:On
	{ A2020_CMD_ALGO_PARAMID	,0x0030 	,A2020_CMD_ALGO_PARAM	,0x0000 },	//0x8017:SetAlgorithmParmID, 0x0030:Tx MBC Mode, 0x8018:SetAlgorithmParm, 0x0000:Off
	{ A2020_CMD_ALGO_PARAMID	,0x0031 	,A2020_CMD_ALGO_PARAM	,0x0000 },	//0x8017:SetAlgorithmParmID, 0x0031:Rx MBC Mode, 0x8018:SetAlgorithmParm, 0x0000:Off
	{ A2020_CMD_ALGO_PARAMID	,0x001A		,A2020_CMD_ALGO_PARAM	,0x0000 },	//0x8017:SetAlgorithmParmID, 0x001A:Use Tx ComfortNoise, 0x8018:SetAlgorithmParm, 0x0000:No

    { A2020_CMD_ALGO_PARAMID	,0x001B		,A2020_CMD_ALGO_PARAM	,0xFFAC },	//0x8017:SetAlgorithmParmID, 0x001B:   Tx ComfortNoise gain (dB), 0x8018:SetAlgorithmParm, 0xFFAC:(-84 dB)
    { A2020_CMD_ALGO_PARAMID	,0x004F		,A2020_CMD_ALGO_PARAM	,0x0000 },	//0x8017:SetAlgorithmParmID, 0x004F:Use Bandwidth Expansion, 0x8018:SetAlgorithmParm, 0x0000:No
    { A2020_CMD_ALGO_PARAMID	,0x0050		,A2020_CMD_ALGO_PARAM	,0x0000 },	//0x8017:SetAlgorithmParmID, 0x0050:  Gain High Band (dB), 0x8018:SetAlgorithmParm, 0x0000:(0 dB)
    { A2020_CMD_ALGO_PARAMID	,0x0051		,A2020_CMD_ALGO_PARAM	,0x0000 },	//0x8017:SetAlgorithmParmID, 0x0051:  Max SNR (dB), 0x8018:SetAlgorithmParm, 0x0000:(0 dB)
    { A2020_CMD_ALGO_PARAMID	,0x0052		,A2020_CMD_ALGO_PARAM	,0x0000 },	//0x8017:SetAlgorithmParmID, 0x0052:  Use BWE Post EQ, 0x8018:SetAlgorithmParm, 0x0000:No

    { 0xFFFF					,0xFFFF		,0xFFFF					,0xFFFF },
};


//CT 1MIC
static a2020_algo_param algo_ct_1MIC_param[] =  
{
    //Set Algorithm Parameter    
    { A2020_CMD_ALGO_PARAMID, 0x0002, A2020_CMD_ALGO_PARAM, 0x0003 }, //MIC Configuration
    { A2020_CMD_ALGO_PARAMID, 0x0000, A2020_CMD_ALGO_PARAM, 0x0005 }, //AIS Suppression Level
    { A2020_CMD_ALGO_PARAMID, 0x0003, A2020_CMD_ALGO_PARAM, 0x0001 },
    { A2020_CMD_ALGO_PARAMID, 0x0012, A2020_CMD_ALGO_PARAM, 0xFFD3 },
    { A2020_CMD_ALGO_PARAMID, 0x0023, A2020_CMD_ALGO_PARAM, 0x0001 },

    { A2020_CMD_ALGO_PARAMID, 0x0034, A2020_CMD_ALGO_PARAM, 0xFFFB },
    { A2020_CMD_ALGO_PARAMID, 0x0009, A2020_CMD_ALGO_PARAM, 0x0001 },
    { A2020_CMD_ALGO_PARAMID, 0x0025, A2020_CMD_ALGO_PARAM, 0x0000 },
    { A2020_CMD_ALGO_PARAMID, 0x003D, A2020_CMD_ALGO_PARAM, 0x000B },
    { A2020_CMD_ALGO_PARAMID, 0x000E, A2020_CMD_ALGO_PARAM, 0x0001 },

    { A2020_CMD_ALGO_PARAMID, 0x000F, A2020_CMD_ALGO_PARAM, 0x000F },
    { A2020_CMD_ALGO_PARAMID, 0x0004, A2020_CMD_ALGO_PARAM, 0x0000 },
    { A2020_CMD_ALGO_PARAMID, 0x0028, A2020_CMD_ALGO_PARAM, 0x0000 },
    { A2020_CMD_ALGO_PARAMID, 0x0020, A2020_CMD_ALGO_PARAM, 0x0002 },
    { A2020_CMD_ALGO_PARAMID, 0x001F, A2020_CMD_ALGO_PARAM, 0x0002 },

    { A2020_CMD_ALGO_PARAMID, 0x001A, A2020_CMD_ALGO_PARAM, 0x0001 },
    { A2020_CMD_ALGO_PARAMID, 0x001B, A2020_CMD_ALGO_PARAM, 0xFFAC },
    { 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF },
};

//CT VP Off
static a2020_algo_param algo_ct_VPOFF_param[] =  
{
    //Set Algorithm Parameter    
    { A2020_CMD_ALGO_PARAMID, 0x0002, A2020_CMD_ALGO_PARAM, 0x0003 }, //MIC Configuration
    { A2020_CMD_ALGO_PARAMID, 0x0000, A2020_CMD_ALGO_PARAM, 0x0001 }, //AIS Suppression Level
    { A2020_CMD_ALGO_PARAMID, 0x0003, A2020_CMD_ALGO_PARAM, 0x0001 },
    { A2020_CMD_ALGO_PARAMID, 0x0012, A2020_CMD_ALGO_PARAM, 0xFFD3 },
    { A2020_CMD_ALGO_PARAMID, 0x0023, A2020_CMD_ALGO_PARAM, 0x0001 },

    { A2020_CMD_ALGO_PARAMID, 0x0034, A2020_CMD_ALGO_PARAM, 0xFFFB },
    { A2020_CMD_ALGO_PARAMID, 0x0009, A2020_CMD_ALGO_PARAM, 0x0001 },
    { A2020_CMD_ALGO_PARAMID, 0x0025, A2020_CMD_ALGO_PARAM, 0x0000 },
    { A2020_CMD_ALGO_PARAMID, 0x003D, A2020_CMD_ALGO_PARAM, 0x000B },
    { A2020_CMD_ALGO_PARAMID, 0x000E, A2020_CMD_ALGO_PARAM, 0x0001 },

    { A2020_CMD_ALGO_PARAMID, 0x000F, A2020_CMD_ALGO_PARAM, 0x000F },
    { A2020_CMD_ALGO_PARAMID, 0x0004, A2020_CMD_ALGO_PARAM, 0x0000 },
    { A2020_CMD_ALGO_PARAMID, 0x0028, A2020_CMD_ALGO_PARAM, 0x0000 },
    { A2020_CMD_ALGO_PARAMID, 0x0020, A2020_CMD_ALGO_PARAM, 0x0002 },
    { A2020_CMD_ALGO_PARAMID, 0x001F, A2020_CMD_ALGO_PARAM, 0x0002 },

    { A2020_CMD_ALGO_PARAMID, 0x001A, A2020_CMD_ALGO_PARAM, 0x0001 },
    { A2020_CMD_ALGO_PARAMID, 0x001B, A2020_CMD_ALGO_PARAM, 0xFFAC },
    { 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF },
};

//CT 1MIC VP Off close talk
static a2020_algo_param algo_ct_1MIC_VPOFF_param[] =  
{
    //Set Algorithm Parameter    
    { A2020_CMD_ALGO_PARAMID, 0x0002, A2020_CMD_ALGO_PARAM, 0x0003 }, //MIC Configuration
    { A2020_CMD_ALGO_PARAMID, 0x0000, A2020_CMD_ALGO_PARAM, 0x0001 }, //AIS Suppression Level
    { A2020_CMD_ALGO_PARAMID, 0x0003, A2020_CMD_ALGO_PARAM, 0x0001 },
    { A2020_CMD_ALGO_PARAMID, 0x0012, A2020_CMD_ALGO_PARAM, 0xFFD3 },
    { A2020_CMD_ALGO_PARAMID, 0x0023, A2020_CMD_ALGO_PARAM, 0x0001 },

    { A2020_CMD_ALGO_PARAMID, 0x0034, A2020_CMD_ALGO_PARAM, 0xFFFB },
    { A2020_CMD_ALGO_PARAMID, 0x0009, A2020_CMD_ALGO_PARAM, 0x0001 },
    { A2020_CMD_ALGO_PARAMID, 0x0025, A2020_CMD_ALGO_PARAM, 0x0000 },
    { A2020_CMD_ALGO_PARAMID, 0x003D, A2020_CMD_ALGO_PARAM, 0x000B },
    { A2020_CMD_ALGO_PARAMID, 0x000E, A2020_CMD_ALGO_PARAM, 0x0001 },

    { A2020_CMD_ALGO_PARAMID, 0x000F, A2020_CMD_ALGO_PARAM, 0x000F },
    { A2020_CMD_ALGO_PARAMID, 0x0004, A2020_CMD_ALGO_PARAM, 0x0000 },
    { A2020_CMD_ALGO_PARAMID, 0x0028, A2020_CMD_ALGO_PARAM, 0x0000 },
    { A2020_CMD_ALGO_PARAMID, 0x0020, A2020_CMD_ALGO_PARAM, 0x0002 },
    { A2020_CMD_ALGO_PARAMID, 0x001F, A2020_CMD_ALGO_PARAM, 0x0002 },

    { A2020_CMD_ALGO_PARAMID, 0x001A, A2020_CMD_ALGO_PARAM, 0x0001 },
    { A2020_CMD_ALGO_PARAMID, 0x001B, A2020_CMD_ALGO_PARAM, 0xFFAC },
    { 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF },
};


//Far Talk(FT), Set Algorithm Parameter 	//by elecjang 20110711 firmware
static a2020_algo_param algo_ft_param[] = {  
    { A2020_CMD_ALGO_PARAMID	,0x0002		,A2020_CMD_ALGO_PARAM	,0x0001 },	//0x8017:SetAlgorithmParmID, 0x0002:Microphone Configuration, 0x8018:SetAlgorithmParm, 0x0001:2-mic Far Talk (FT/HHS)
    { A2020_CMD_ALGO_PARAMID	,0x004B		,A2020_CMD_ALGO_PARAM	,0x0006 },	//0x8017:SetAlgorithmParmID, 0x004B:Tx Noise Suppression Level, 0x8018:SetAlgorithmParm, 0x0006:Level 6
    { A2020_CMD_ALGO_PARAMID	,0x0015		,A2020_CMD_ALGO_PARAM	,0x0000 },	//0x8017:SetAlgorithmParmID, 0x0015:Side Tone Mode, 0x8018:SetAlgorithmParm, 0x0000:Off
    { A2020_CMD_ALGO_PARAMID	,0x0016		,A2020_CMD_ALGO_PARAM	,0xFFEB },	//0x8017:SetAlgorithmParmID, 0x0016:   Side Tone Gain (dB), 0x8018:SetAlgorithmParm, 0xFFEB:(-21 dB)
    { A2020_CMD_ALGO_PARAMID	,0x0003		,A2020_CMD_ALGO_PARAM	,0x0001 },	//0x8017:SetAlgorithmParmID, 0x0003:AEC Mode, 0x8018:SetAlgorithmParm, 0x0001:AEC On (auto select mode)
    	
    { A2020_CMD_ALGO_PARAMID	,0x0012		,A2020_CMD_ALGO_PARAM	,0x0005 },	//0x8017:SetAlgorithmParmID, 0x0012:   Downlink Speaker Volume, 0x8018:SetAlgorithmParm, 0x0005:(5 dB)
    { A2020_CMD_ALGO_PARAMID	,0x0023		,A2020_CMD_ALGO_PARAM	,0x0000 },	//0x8017:SetAlgorithmParmID, 0x0023:   Use AEC Comfort Noise Fill, 0x8018:SetAlgorithmParm, 0x0000:No
    { A2020_CMD_ALGO_PARAMID	,0x0034		,A2020_CMD_ALGO_PARAM	,0x0016 },	//0x8017:SetAlgorithmParmID, 0x0034:   Echo Suppression Enhancement, 0x8018:SetAlgorithmParm, 0x0016:(22 dB)
    { A2020_CMD_ALGO_PARAMID	,0x0004		,A2020_CMD_ALGO_PARAM	,0x0000 },	//0x8017:SetAlgorithmParmID, 0x0004:Use AGC, 0x8018:SetAlgorithmParm, 0x0000:No
    { A2020_CMD_ALGO_PARAMID	,0x0028		,A2020_CMD_ALGO_PARAM	,0x0000 },	//0x8017:SetAlgorithmParmID, 0x0028:Use Rx AGC, 0x8018:SetAlgorithmParm, 0x0000:No

    { A2020_CMD_ALGO_PARAMID	,0x0009		,A2020_CMD_ALGO_PARAM	,0x0001 },	//0x8017:SetAlgorithmParmID, 0x0009:Speaker Enhancement Mode, 0x8018:SetAlgorithmParm, 0x0001:SE On (auto select SE mode)
    { A2020_CMD_ALGO_PARAMID	,0x0025		,A2020_CMD_ALGO_PARAM	,0xFFF7 },	//0x8017:SetAlgorithmParmID, 0x0025:   SE Noise Estimate Adj, 0x8018:SetAlgorithmParm, 0xFFF7:(-9 dB)
    { A2020_CMD_ALGO_PARAMID	,0x003D		,A2020_CMD_ALGO_PARAM	,0x000B },	//0x8017:SetAlgorithmParmID, 0x003D:   SE Max Gain (dB), 0x8018:SetAlgorithmParm, 0x000B:(11 dB)
    { A2020_CMD_ALGO_PARAMID	,0x000E		,A2020_CMD_ALGO_PARAM	,0x0001 },	//0x8017:SetAlgorithmParmID, 0x000E:Far End Noise Suppression, 0x8018:SetAlgorithmParm, 0x0001:On (auto select mode)
    { A2020_CMD_ALGO_PARAMID	,0x004C		,A2020_CMD_ALGO_PARAM	,0x0006 },	//0x8017:SetAlgorithmParmID, 0x004C:   Rx Noise Suppression Level, 0x8018:SetAlgorithmParm, 0x0006:Level 6

    { A2020_CMD_ALGO_PARAMID	,0x0040		,A2020_CMD_ALGO_PARAM	,0xFFF7 },	//0x8017:SetAlgorithmParmID, 0x0040:Tx-out Limiter Max Level (dB), 0x8018:SetAlgorithmParm, 0xFFF7:(-9 dB)
    { A2020_CMD_ALGO_PARAMID	,0x0020		,A2020_CMD_ALGO_PARAM	,0x0000 },	//0x8017:SetAlgorithmParmID, 0x0020:Tx PostEq Mode, 0x8018:SetAlgorithmParm, 0x0000:Off
    { A2020_CMD_ALGO_PARAMID	,0x001F		,A2020_CMD_ALGO_PARAM	,0x0000 },	//0x8017:SetAlgorithmParmID, 0x001F:Rx PostEq Mode, 0x8018:SetAlgorithmParm, 0x0000:Off
	{ A2020_CMD_ALGO_PARAMID	,0x0030 	,A2020_CMD_ALGO_PARAM	,0x0000 },	//0x8017:SetAlgorithmParmID, 0x0030:Tx MBC Mode, 0x8018:SetAlgorithmParm, 0x0000:Off
	{ A2020_CMD_ALGO_PARAMID	,0x0031 	,A2020_CMD_ALGO_PARAM	,0x0001 },	//0x8017:SetAlgorithmParmID, 0x0031:Rx MBC Mode, 0x8018:SetAlgorithmParm, 0x0001:On
	{ A2020_CMD_ALGO_PARAMID	,0x001A		,A2020_CMD_ALGO_PARAM	,0x0000 },	//0x8017:SetAlgorithmParmID, 0x001A:Use Tx ComfortNoise, 0x8018:SetAlgorithmParm, 0x0000:No

    { A2020_CMD_ALGO_PARAMID	,0x001B		,A2020_CMD_ALGO_PARAM	,0xFFAC },	//0x8017:SetAlgorithmParmID, 0x001B:   Tx ComfortNoise gain (dB), 0x8018:SetAlgorithmParm, 0xFFAC:(-84 dB)
    { A2020_CMD_ALGO_PARAMID	,0x004F		,A2020_CMD_ALGO_PARAM	,0x0000 },	//0x8017:SetAlgorithmParmID, 0x004F:Use Bandwidth Expansion, 0x8018:SetAlgorithmParm, 0x0000:No
    { A2020_CMD_ALGO_PARAMID	,0x0050		,A2020_CMD_ALGO_PARAM	,0x0000 },	//0x8017:SetAlgorithmParmID, 0x0050:  Gain High Band (dB), 0x8018:SetAlgorithmParm, 0x0000:(0 dB)
    { A2020_CMD_ALGO_PARAMID	,0x0051		,A2020_CMD_ALGO_PARAM	,0x0000 },	//0x8017:SetAlgorithmParmID, 0x0051:  Max SNR (dB), 0x8018:SetAlgorithmParm, 0x0000:(0 dB)
    { A2020_CMD_ALGO_PARAMID	,0x0052		,A2020_CMD_ALGO_PARAM	,0x0000 },	//0x8017:SetAlgorithmParmID, 0x0052:  Use BWE Post EQ, 0x8018:SetAlgorithmParm, 0x0000:No

    { 0xFFFF					,0xFFFF		,0xFFFF					,0xFFFF },
};


//FT VP Off
static a2020_algo_param algo_ft_VPOFF_param[] = 
{
    //Set Algorithm Parameter    
    { A2020_CMD_ALGO_PARAMID, 0x0002, A2020_CMD_ALGO_PARAM, 0x0002 },
    { A2020_CMD_ALGO_PARAMID, 0x0000, A2020_CMD_ALGO_PARAM, 0x0001 },
    { A2020_CMD_ALGO_PARAMID, 0x0003, A2020_CMD_ALGO_PARAM, 0x0001 },
    { A2020_CMD_ALGO_PARAMID, 0x0012, A2020_CMD_ALGO_PARAM, 0x0003 },
    { A2020_CMD_ALGO_PARAMID, 0x0023, A2020_CMD_ALGO_PARAM, 0x0001 },
    	
    { A2020_CMD_ALGO_PARAMID, 0x0034, A2020_CMD_ALGO_PARAM, 0x0000 },
    { A2020_CMD_ALGO_PARAMID, 0x0009, A2020_CMD_ALGO_PARAM, 0x0000 },
    { A2020_CMD_ALGO_PARAMID, 0x0025, A2020_CMD_ALGO_PARAM, 0x0000 },
    { A2020_CMD_ALGO_PARAMID, 0x000E, A2020_CMD_ALGO_PARAM, 0x0001 },
    { A2020_CMD_ALGO_PARAMID, 0x000F, A2020_CMD_ALGO_PARAM, 0x000A },

    { A2020_CMD_ALGO_PARAMID, 0x0004, A2020_CMD_ALGO_PARAM, 0x0000 },
    { A2020_CMD_ALGO_PARAMID, 0x0028, A2020_CMD_ALGO_PARAM, 0x0000 },
    { A2020_CMD_ALGO_PARAMID, 0x0020, A2020_CMD_ALGO_PARAM, 0x0000 },
    { A2020_CMD_ALGO_PARAMID, 0x001F, A2020_CMD_ALGO_PARAM, 0x0002 },
    { A2020_CMD_ALGO_PARAMID, 0x001A, A2020_CMD_ALGO_PARAM, 0x0001 },

    { A2020_CMD_ALGO_PARAMID, 0x001B, A2020_CMD_ALGO_PARAM, 0xFFBA },
    { 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF },
};

//Voice LoopBack Primary/Secondary MIC VP Off
static a2020_algo_param algo_ct_vl_MIC_param[] =  
{
    //Set Algorithm Parameter    
    { A2020_CMD_ALGO_PARAMID, 0x0002, A2020_CMD_ALGO_PARAM, 0x0003 }, //MIC Configuration
    { A2020_CMD_ALGO_PARAMID, 0x0000, A2020_CMD_ALGO_PARAM, 0x0005 }, //AIS Suppression Level
    { A2020_CMD_ALGO_PARAMID, 0x0003, A2020_CMD_ALGO_PARAM, 0x0001 },
    { A2020_CMD_ALGO_PARAMID, 0x0012, A2020_CMD_ALGO_PARAM, 0xFFD3 },
    { A2020_CMD_ALGO_PARAMID, 0x0023, A2020_CMD_ALGO_PARAM, 0x0001 },

    { A2020_CMD_ALGO_PARAMID, 0x0034, A2020_CMD_ALGO_PARAM, 0xFFFB },
    { A2020_CMD_ALGO_PARAMID, 0x0009, A2020_CMD_ALGO_PARAM, 0x0001 },
    { A2020_CMD_ALGO_PARAMID, 0x0025, A2020_CMD_ALGO_PARAM, 0x0000 },
    { A2020_CMD_ALGO_PARAMID, 0x003D, A2020_CMD_ALGO_PARAM, 0x000B },
    { A2020_CMD_ALGO_PARAMID, 0x000E, A2020_CMD_ALGO_PARAM, 0x0001 },

    { A2020_CMD_ALGO_PARAMID, 0x000F, A2020_CMD_ALGO_PARAM, 0x000F },
    { A2020_CMD_ALGO_PARAMID, 0x0004, A2020_CMD_ALGO_PARAM, 0x0000 },
    { A2020_CMD_ALGO_PARAMID, 0x0028, A2020_CMD_ALGO_PARAM, 0x0000 },
    { A2020_CMD_ALGO_PARAMID, 0x0020, A2020_CMD_ALGO_PARAM, 0x0002 },
    { A2020_CMD_ALGO_PARAMID, 0x001F, A2020_CMD_ALGO_PARAM, 0x0002 },

    { A2020_CMD_ALGO_PARAMID, 0x001A, A2020_CMD_ALGO_PARAM, 0x0001 },
    { A2020_CMD_ALGO_PARAMID, 0x001B, A2020_CMD_ALGO_PARAM, 0xFFAC },
    { 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF },
};

//Call Primary/Secondary MIC Test VP On
static a2020_algo_param algo_ct_call_MIC_param[] =  
{
    //Set Algorithm Parameter
    { A2020_CMD_ALGO_PARAMID, 0x0002, A2020_CMD_ALGO_PARAM, 0x0003 }, //MIC Configuration
    { A2020_CMD_ALGO_PARAMID, 0x0000, A2020_CMD_ALGO_PARAM, 0x0005 }, //AIS Suppression Level
    { A2020_CMD_ALGO_PARAMID, 0x0003, A2020_CMD_ALGO_PARAM, 0x0001 },
    { A2020_CMD_ALGO_PARAMID, 0x0012, A2020_CMD_ALGO_PARAM, 0xFFD3 },
    { A2020_CMD_ALGO_PARAMID, 0x0023, A2020_CMD_ALGO_PARAM, 0x0001 },

    { A2020_CMD_ALGO_PARAMID, 0x0034, A2020_CMD_ALGO_PARAM, 0xFFFB },
    { A2020_CMD_ALGO_PARAMID, 0x0009, A2020_CMD_ALGO_PARAM, 0x0001 },
    { A2020_CMD_ALGO_PARAMID, 0x0025, A2020_CMD_ALGO_PARAM, 0x0000 },
    { A2020_CMD_ALGO_PARAMID, 0x003D, A2020_CMD_ALGO_PARAM, 0x000B },
    { A2020_CMD_ALGO_PARAMID, 0x000E, A2020_CMD_ALGO_PARAM, 0x0001 },

    { A2020_CMD_ALGO_PARAMID, 0x000F, A2020_CMD_ALGO_PARAM, 0x000F },
    { A2020_CMD_ALGO_PARAMID, 0x0004, A2020_CMD_ALGO_PARAM, 0x0000 },
    { A2020_CMD_ALGO_PARAMID, 0x0028, A2020_CMD_ALGO_PARAM, 0x0000 },
    { A2020_CMD_ALGO_PARAMID, 0x0020, A2020_CMD_ALGO_PARAM, 0x0002 },
    { A2020_CMD_ALGO_PARAMID, 0x001F, A2020_CMD_ALGO_PARAM, 0x0002 },

    { A2020_CMD_ALGO_PARAMID, 0x001A, A2020_CMD_ALGO_PARAM, 0x0001 },
    { A2020_CMD_ALGO_PARAMID, 0x001B, A2020_CMD_ALGO_PARAM, 0xFFAC },
    { 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF },
};

typedef struct 
{
    u16 cmd;
    u16 value;
} a2020_path_param;

//==========Voice Processing ON/OFF=================================================================
static a2020_path_param path_vp_on_param[] = {
    { A2020_CMD_VOICE_PROCESSING, 0x0001 },
    { 0xFFFF, 0xFFFF },
};

static a2020_path_param path_vp_off_param[] = {
    { A2020_CMD_VOICE_PROCESSING, 0x0000 },
    { 0xFFFF, 0xFFFF },
};
//===========================================================================================




//CT SelectRouting
static a2020_path_param path_routing_param[] = {
    { A2020_CMD_SET_AUDIO_ROUTING		,0x0001 },
    { 0xFFFF							,0xFFFF },
};

//FT SelectRouting
static a2020_path_param path_ct_A_routing_param[] = {
    { A2020_CMD_SET_AUDIO_ROUTING		,0x0002 },	//0x8026:SelectRouting, 0x0002:Pri,Sec,Zro,Fei,Zro,Zro,Zro - Snk,Feo,Csp,Snk,Snk,Snk
    { 0xFFFF							,0xFFFF },
};

//Voice LoopBack Primary MIC Audio Routing
static a2020_path_param path_vl_pri_MIC_routing_param[] = {
    { A2020_CMD_SET_AUDIO_ROUTING		,0x0007 },
    { 0xFFFF							,0xFFFF },
};

//Voice LoopBack Secondary MIC Audio Routing
static a2020_path_param path_vl_sec_MIC_routing_param[] = {
    { A2020_CMD_SET_AUDIO_ROUTING		,0x0015 },
    { 0xFFFF							,0xFFFF },
};

//Call Primary MIC Audio Routing
static a2020_path_param path_call_pri_MIC_routing_param[] = {
    { A2020_CMD_SET_AUDIO_ROUTING		,0x000A },
    { 0xFFFF							,0xFFFF },
};

//Call Secondary MIC Audio Routing
static a2020_path_param path_call_sec_MIC_routing_param[] = {
    { A2020_CMD_SET_AUDIO_ROUTING		,0x0008 },
    { 0xFFFF							,0xFFFF },
};

//DV SelectRouting
static a2020_path_param path_call_sec_DV_MIC_routing_param[] = {
    { A2020_CMD_SET_AUDIO_ROUTING		,0x000A },	//0x8026:SelectRouting, 0x0009:Pri,Zro,Zro,Fei,Zro,Zro,Zro - Snk,Feo,Csp,Snk,Snk,Snk
    { 0xFFFF							,0xFFFF },
};


//KeyTone Digital Gain
static a2020_path_param path_keytone_gain_param[] =
{
    { A2020_CMD_SET_DIGI_INPUT_GAIN, 0x0203 },
    { 0xFFFF, 0xFFFF },
};


//CT Digital Gain	//by elecjang 20110716 macro // skkim 20110719 macro
static a2020_path_param path_ct_gain_param[] = 
{
    { A2020_CMD_SET_DIGI_INPUT_GAIN		,0x0007 },		//0x801B:SetDigitalInputGain, 0x00:ADC0, 0x07:(7 dB)
    { A2020_CMD_SET_DIGI_INPUT_GAIN		,0x0107 },		//0x801B:SetDigitalInputGain, 0x01:ADC1, 0x07:(7 dB)
    { A2020_CMD_SET_DIGI_INPUT_GAIN		,0x03F2 },		//0x801B:SetDigitalInputGain, 0x03:PCM-A left, 0xF2:(-14 dB)
    { A2020_CMD_SET_DIGI_OUTPUT_GAIN	,0x0005 },		//0x8015:SetDigitalOutputGain, 0x00:DAC0, 0x05:(5 dB)
    { A2020_CMD_SET_DIGI_OUTPUT_GAIN	,0x0204 },		//0x8015:SetDigitalOutputGain, 0x02:PCM-A left,  0x04:(4 dB)
    { 0xFFFF							,0xFFFF },
};


//CT 1MIC Digital Gain
static a2020_path_param path_ct_1MIC_gain_param[] =
{
    { A2020_CMD_SET_DIGI_INPUT_GAIN, 0x0004 }, //Primary MIC
    { A2020_CMD_SET_DIGI_INPUT_GAIN, 0x0104 }, //Secondary MIC
    { A2020_CMD_SET_DIGI_OUTPUT_GAIN, 0x0004 }, //Tx
    { A2020_CMD_SET_DIGI_INPUT_GAIN, 0x0202 }, //Far End(Rx)
    { A2020_CMD_SET_DIGI_OUTPUT_GAIN, 0x0100 }, //Rx
    { 0xFFFF, 0xFFFF },
};

//CT VP Off Digital Gain
static a2020_path_param path_ct_VPOFF_gain_param[] = 
{
    { A2020_CMD_SET_DIGI_INPUT_GAIN, 0x0002 }, //Primary MIC
    { A2020_CMD_SET_DIGI_INPUT_GAIN, 0x0102 }, //Secondary MIC
    { A2020_CMD_SET_DIGI_OUTPUT_GAIN, 0x0002 }, //Tx
    { A2020_CMD_SET_DIGI_INPUT_GAIN, 0x0200 }, //Far End(Rx)
    { A2020_CMD_SET_DIGI_OUTPUT_GAIN, 0x01FD }, //Rx
    { 0xFFFF, 0xFFFF },
};

//CT 1MIC VP Off Digital Gain
static a2020_path_param path_ct_1MIC_VPOFF_gain_param[] = 
{
    { A2020_CMD_SET_DIGI_INPUT_GAIN		, 0x0002 }, //Primary MIC
    { A2020_CMD_SET_DIGI_INPUT_GAIN		, 0x0102 }, //Secondary MIC
    { A2020_CMD_SET_DIGI_OUTPUT_GAIN	, 0x0002 }, //Tx
    { A2020_CMD_SET_DIGI_INPUT_GAIN		, 0x0200 }, //Far End(Rx)
    { A2020_CMD_SET_DIGI_OUTPUT_GAIN	, 0x01FD }, //Rx
    { 0xFFFF							, 0xFFFF },
};


//FT Digital Gain	//by elecjang 20110716 macro 
static a2020_path_param path_ft_gain_param[] = {
    { A2020_CMD_SET_DIGI_INPUT_GAIN		,0x000A },	//0x801B:SetDigitalInputGain, 0x00:ADC0, 0x0A:(10 dB)
    { A2020_CMD_SET_DIGI_INPUT_GAIN		,0x010A },	//0x801B:SetDigitalInputGain, 0x01:ADC1, 0x0A:(10 dB)
	{ A2020_CMD_SET_DIGI_INPUT_GAIN		,0x03FC },	//0x801B:SetDigitalInputGain, 0x03:PCM-A left, 0xFC:(-4 dB)
    { A2020_CMD_SET_DIGI_OUTPUT_GAIN	,0x0103 },	//0x8015:SetDigitalOutputGain, 0x01:DAC1, 0x03:(3 dB)
    { A2020_CMD_SET_DIGI_OUTPUT_GAIN	,0x020A },	//0x8015:SetDigitalOutputGain, 0x02:PCM-A left,  0x0A:(10 dB)
    { 0xFFFF							,0xFFFF },
};


//FT VP Off Digital Gain
static a2020_path_param path_ft_VPOFF_gain_param[] = 
{
    { A2020_CMD_SET_DIGI_INPUT_GAIN		, 0x00FB }, //Primary MIC
    { A2020_CMD_SET_DIGI_INPUT_GAIN		, 0x01FB }, //Secondary MIC
    { A2020_CMD_SET_DIGI_OUTPUT_GAIN	, 0x0000 }, //Tx
    { A2020_CMD_SET_DIGI_INPUT_GAIN		, 0x02FD }, //Far End(Rx)
    { A2020_CMD_SET_DIGI_OUTPUT_GAIN	, 0x0101 }, //Rx
    { 0xFFFF							, 0xFFFF },
};

//DV Digital Gain //by elecjang 20110716 macro
static a2020_path_param path_dv_gain_param[] = {
    { A2020_CMD_SET_DIGI_INPUT_GAIN		, 0x000A },	//0x801B:SetDigitalInputGain, 0x00:ADC0, 0x0A:(10 dB)
    { A2020_CMD_SET_DIGI_INPUT_GAIN		, 0x010A },	//0x801B:SetDigitalInputGain, 0x01:ADC1, 0x0A:(10 dB)
	{ A2020_CMD_SET_DIGI_INPUT_GAIN		, 0x03FC },	//0x801B:SetDigitalInputGain, 0x03:PCM-A left, 0xFA:(-6 dB)
    { A2020_CMD_SET_DIGI_OUTPUT_GAIN	, 0x020A },	//0x8015:SetDigitalOutputGain, 0x02:PCM-A left,  0x0A:(10 dB)
    { A2020_CMD_SET_DIGI_OUTPUT_GAIN	, 0x0103 },	//0x8015:SetDigitalOutputGain, 0x01:DAC1, 0x03:(3 dB)
    { 0xFFFF							, 0xFFFF },
};


//DV VP Off Digital Gain
static a2020_path_param path_dv_VPOFF_gain_param[] = 
{
    { A2020_CMD_SET_DIGI_INPUT_GAIN		, 0x00FB }, //Primary MIC
    { A2020_CMD_SET_DIGI_INPUT_GAIN		, 0x01FB }, //Secondary MIC
    { A2020_CMD_SET_DIGI_OUTPUT_GAIN	, 0x0000 }, //Tx
    { A2020_CMD_SET_DIGI_INPUT_GAIN		, 0x02FD }, //Far End(Rx)
    { A2020_CMD_SET_DIGI_OUTPUT_GAIN	, 0x0101 }, //Rx
    { 0xFFFF							, 0xFFFF },
};


//Voice LoopBack Primary/Secondary MIC Digital Gain
static a2020_path_param path_vl_MIC_gain_param[] = 
{
    { A2020_CMD_SET_DIGI_INPUT_GAIN, 0x0004 }, //Primary MIC
    { A2020_CMD_SET_DIGI_INPUT_GAIN, /*0x0104 */0x0113}, //Secondary MIC 4dB -> 19dB
    { A2020_CMD_SET_DIGI_OUTPUT_GAIN, 0x0004}, //Tx
    { A2020_CMD_SET_DIGI_INPUT_GAIN, 0x0202 }, //Far End(Rx)
    { A2020_CMD_SET_DIGI_OUTPUT_GAIN, 0x0100 }, //Rx
    { 0xFFFF, 0xFFFF },
};

//InCall Primary/Secondary MIC Digital Gain
static a2020_path_param path_call_MIC_gain_param[] =
{
    { A2020_CMD_SET_DIGI_INPUT_GAIN, 0x0004 }, //Primary MIC
    { A2020_CMD_SET_DIGI_INPUT_GAIN, 0x0104 }, //Secondary MIC
    { A2020_CMD_SET_DIGI_OUTPUT_GAIN, 0x0004 }, //Tx
    { A2020_CMD_SET_DIGI_INPUT_GAIN, 0x0202 }, //Far End(Rx)
    { A2020_CMD_SET_DIGI_OUTPUT_GAIN, 0x0100 }, //Rx
    { 0xFFFF, 0xFFFF },
};

#ifdef FEATURE_SKY_SND_EXTAMPCTRL  //20110519 jhsong : sub chip cal tool by youngsoo Lim
static uint32_t extamp_cal_dev_id = 0;
extern bool LockForTest;
static a2020_path_param a2020_gain_cal[6] = {{0,0},{0,0},{0,0},{0,0},{0,0},{0,0}};
static a2020_algo_param a2020_algo_cal[10] = {{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0}};
static uint32_t ext_audience_device = 0x00;
static int ext_audience_pth_on = 0;

void audience_set_cal_value_data(struct extamp_device_info *dev_info)
{
//	tSubsystemInfo_cal.power_info = (u8)DEFAULT_DATA_REG;
	extamp_cal_dev_id = dev_info->extamp_dev_id;

	a2020_gain_cal[0].value= (u16) (0x0000) | (dev_info->audience_input_gain_id_prim_mic_gain);
	a2020_gain_cal[1].value= (u16) (0x0100) | (dev_info->audience_input_gain_id_sec_mic_gain);
	a2020_gain_cal[2].value= (u16) (0x0000) | (dev_info->audience_digital_out_tx_gain);
	a2020_gain_cal[3].value= (u16) (0x0200) | (dev_info->audience_input_gain_id_far_end_gain);
	a2020_gain_cal[4].value= (u16) (0x0100) | (dev_info->audience_digital_out_rx_gain);
	a2020_gain_cal[5].value= (u16) 0xffff;

	a2020_algo_cal[0].value = (u16) (dev_info->audience_algo_param_id_aec_mode);
	a2020_algo_cal[1].value = (u16) (dev_info->audience_algo_param_id_se_mode);
	a2020_algo_cal[2].value = (u16) ((dev_info->audience_algo_param_id_downlink_spk_vol_1)<<2) | ((dev_info->audience_algo_param_id_downlink_spk_vol_2));
	a2020_algo_cal[3].value = (u16) (dev_info->audience_algo_param_id_aec_comfort_noise_mode);
	a2020_algo_cal[4].value = (u16) ((dev_info->audience_algo_param_id_ec_supp_enhanced_1)<<2) | ((dev_info->audience_algo_param_id_ec_supp_enhanced_2));
	a2020_algo_cal[5].value = (u16) ((dev_info->audience_algo_param_id_veq_noise_estimate_adj_1)<<2) | ((dev_info->audience_algo_param_id_veq_noise_estimate_adj_2));
	a2020_algo_cal[6].value = (u16) ((dev_info->audience_algo_param_id_veq_max_gain_1)<<2) | ((dev_info->audience_algo_param_id_veq_max_gain_2));
	a2020_algo_cal[7].value = (u16) (dev_info->audience_algo_param_id_far_end_noise_supp);
	a2020_algo_cal[8].value = (u16) ((dev_info->audience_algo_param_id_fens_amount_1)<<2) | ((dev_info->audience_algo_param_id_fens_amount_2));
	a2020_algo_cal[9].value= (u16) 0xffff;

	
//	printk("@#@#fab2200_set_cal_value_data.......0x02 : 0x%x,    0x05 : 0x%x,     0x06 : 0x%x\n",tSubsystemInfo_cal.diff_pga_info,tSubsystemInfo_cal.hpl_vol_info,tSubsystemInfo_cal.hpr_vol_info );
//	printk("@#@#fab2200_set_cal_value_data.......0x07 : 0x%x,    0x08 : 0x%x,     0x09 : 0x%x\n",tSubsystemInfo_cal.hp_ng_info,tSubsystemInfo_cal.spk_ramp_zc_info,tSubsystemInfo_cal.spk_ng_info );
//	printk("@#@#fab2200_set_cal_value_data.......0x0a : 0x%x,    0x0b : 0x%x,     0x0c : 0x%x\n",tSubsystemInfo_cal.thresh_info,tSubsystemInfo_cal.agc_info,tSubsystemInfo_cal.agc_min_info );
//	printk("@#@#fab2200_set_cal_value_data.......0x0e : 0x%x,    0x0f : 0x%x\n",tSubsystemInfo_cal.start_gain_info,tSubsystemInfo_cal.ssmt_info);
}

void snd_extamp_api_audience_SetVolume_ById(uint32_t extamp_dev_id) {
//	extamp_info_t tCurrentExtampInfo;
	struct extamp_device_info *di;
//	int status = 0;

	printk(KERN_ERR "[[ MAX97001 ]] ### extamp_dev_id(0x%x)\n", extamp_dev_id);


	ext_audience_device = extamp_dev_id;
	
	di = extamp_lookup_device(extamp_dev_id);

/*************************************************************************************/
/* Need to modify each Audio sub system.                                             */
/*************************************************************************************/
	audience_set_cal_value_data(di);



/* Debug session */
//	printk(KERN_ERR "[[ FAB2200 ]] ### ID(0x%d) \n",
//	          di->extamp_dev_id);
//	printk(KERN_ERR "[[ FAB2200 ]] ### \n",
//              );
/*************************************************************************************/
}

uint32_t ext_audience_get_device(void)
{
	return ext_audience_device;
}

int ext_audience_get_path_on(void)
{
	return ext_audience_pth_on;
}
#endif


void A2020_msgCB_Chip_AckDataFlush(void)
{
    byte read_buf[4];
    struct i2c_msg msg;
    static int ret_value = 0;
    int check_count = 0;
	
    u32 get_value = 0;
    u32 swap_value = 0;

	while(1)
	{
	    if(!audience_a2020_i2c_client){
	        break;
	    }

	    memset((void*)read_buf, 0x00, 4);
		
	    msg.addr = audience_a2020_i2c_client->addr;
	    msg.flags = I2C_M_RD;
	    msg.len = 4;
	    msg.buf = read_buf;
	    
	    ret_value = i2c_transfer(audience_a2020_i2c_client->adapter, &msg, 1);

	    if (ret_value < 0) 
	    {
	       printk( "[Snd_audience_a2020] A2020_msgCB_Chip_AckDataFlush I2C ERROR OCCURE~~   I2C Error Num%d   !!!\n",ret_value);
	    }

	    memcpy((byte*)&get_value,read_buf,4);
	    swap_value = SWAP32(get_value);
		
	    printk( "[Snd_audience_a2020] A2020_msgCB_Chip_AckDataFlush 0x%x !!!\n",swap_value);
		
	    if(swap_value == 0x0000)
	    {
	        break;
	    }
		
	    if(swap_value == 0xffff)
	    {
	        check_count++;
	        if(check_count > 4)
	        {
                   printk( "[Snd_audience_a2020] A2020_msgCB_Chip_AckDataFlush swap_value IS 0xffff 4 times 0x%x !!!\n",swap_value);
		     break;
	        }
	    }
		
	}

}

int aud_a2020_is_active_mode(void)
{
    if(a2020_mode == A2020_ACTIVE_MODE)
        return 1;
    else
        return 0;
}

int aud_a2020_is_sleep_mode(void)
{
    if(a2020_mode == A2020_SLEEP_MODE)
        return 1;
    else
        return 0;
}

// length : byte num for sending (cmd + value)
// lengh limit 4 
// error num #define	ENXIO		 6	/* No such device or address */
int A2020_msg(u16 cmd, u16 value, u16 length)
{
    static int ret_value = 0;
    u16 len = 0;
    u16 swap_value = 0;

    unsigned char buf[4];	
    struct i2c_msg msg;

    if(len > 4) return 0;
	
    if(!audience_a2020_i2c_client){
        return 0;
    }
	
    len = length;
	
    swap_value = SWAP16(cmd);
    memcpy((byte*)&buf[0],(byte*)&swap_value, 2);
    swap_value = SWAP16(value);
    memcpy((byte*)&buf[2],(byte*)&swap_value, 2);
	

    msg.addr = audience_a2020_i2c_client->addr;
    msg.flags = 0;
    msg.len = len;
    msg.buf = (char *)buf;
    
    ret_value = i2c_transfer(audience_a2020_i2c_client->adapter, &msg, 1);
	
    printk( "[Snd_audience_a2020] A2020_msg cmd : 0x%x  value : 0x%x   !!!\n",cmd, value);

    if (ret_value < 0) 
    {
       printk( "[Snd_audience_a2020] A2020_msg I2C ERROR OCCURE~~   I2C Error Num%d   !!!\n",ret_value);    
        return 0;
    }

    return 1;	
}

int A2020_msgCB(u16 read_byte, byte * read_buf)
{
    static int ret_value = 0;
    struct i2c_msg msg;
	
    u32 get_value = 0;
    u32 swap_value = 0;

    if(!audience_a2020_i2c_client){
        return 0;
    }

    memset((void*)read_buf, 0x00, read_byte);
	
    msg.addr = audience_a2020_i2c_client->addr;
    msg.flags = I2C_M_RD;
    msg.len = read_byte;
    msg.buf = read_buf;
    
    ret_value = i2c_transfer(audience_a2020_i2c_client->adapter, &msg, 1);

    if (ret_value < 0) 
    {
       printk( "[Snd_audience_a2020] A2020_msgCB I2C ERROR OCCURE~~   I2C Error Num%d   !!!\n",ret_value);
       return 0;
    }

    memcpy((byte*)&get_value,read_buf,4);
    swap_value = SWAP32(get_value);
	
    printk( "[Snd_audience_a2020] A2020_msgCB 0x%x !!!\n",swap_value);
	
    if(swap_value == 0x00)
    {
        //A2020 not ready
        return 0;
    }

    return 1;
}

int aud_boot_cmd_to_2020(void)
{
    int ret_val = 0;
    byte count = 0;

    while(1)
    {
        ret_val = A2020_msg(A2020_CMD_BOOT, 0, 2); //send 2 bytes to the A2020

        mdelay(1); //sleep needed for firmware to process

        if(ret_val == 1)
        {
            byte read_buf[1];
            ret_val = A2020_msgCB(1, read_buf); //read the 1 byte ACK back

            if(ret_val == 1)
            {
                if(read_buf[0] != A2020_CMD_BOOT_ACK_A2020)					
                {
                    count++;
                    mdelay(1);	

                    if(count > POLLING_LOOP_COUNT)
                    {
                        printk( "[Snd_audience_a2020] A2020 Boot CMD ACK NOK !!!\n");
                        break;
                    }
                }
                else
                {
                    printk( "[Snd_audience_a2020] A2020 Boot CMD ACK OK !!!\n");
                    break;
                }            
            }
            else
            {
                count++;
                mdelay(1);

                if(count > POLLING_LOOP_COUNT)
                {
                   printk( "[Snd_audience_a2020] A2020 Boot CMD I2C ACK NOK !!!\n");
                   break;
                }            
            }
        }
        else
        {
            count++;            
            mdelay(1);
            
            if(count > (POLLING_LOOP_COUNT * 6))
            {
                printk( "[Snd_audience_a2020] A2020 Boot CMD I2C Write I2C NOK !!!\n");				
                break;
            }
        }
    }

    return ret_val;	
}

#if 0 //by elecjang 20110711, for large size firmware, it must be separated
int aud_bootimage_download(void)
{

    struct i2c_msg msg;
    u16 len = 0;
    int ret_val = 0;
	
    if(!audience_a2020_i2c_client){
        return 0;
    }
   
    len = sizeof(a2020_firmware_image);
    
    msg.addr = audience_a2020_i2c_client->addr;
    msg.flags = 0;
    msg.len = len;
    msg.buf = (char *)a2020_firmware_image;
    
    ret_val = i2c_transfer(audience_a2020_i2c_client->adapter, &msg, 1);
	
    if (ret_val < 0) 
    {
        printk( "[Snd_audience_a2020] A2020 Boot DL I2C NOK !!!\n");   
        return 0;
    }
    
    printk( "[Snd_audience_a2020] A2020 Boot DL OK !!!\n");
    return 1;
}
#else
int aud_bootimage_download_1st(void)
{

    struct i2c_msg msg;
    u16 len = 0;
    int ret_val = 0;
	
    if(!audience_a2020_i2c_client){
        return 0;
    }
   
    len = sizeof(a2020_firmware_image1);
    
    msg.addr = audience_a2020_i2c_client->addr;
    msg.flags = 0;
    msg.len = len;
    msg.buf = (char *)a2020_firmware_image1;
    
    ret_val = i2c_transfer(audience_a2020_i2c_client->adapter, &msg, 1);
	
    if (ret_val < 0){
        printk( "[Snd_audience_a2020] aud_bootimage_download_1st NOK !!!\n");   
        return 0;
    }
    
    //printk( "[Snd_audience_a2020] aud_bootimage_download_1st OK !!!\n");
    return 1;
}


int aud_bootimage_download_2st(void)
{

    struct i2c_msg msg;
    u16 len = 0;
    int ret_val = 0;
	
    if(!audience_a2020_i2c_client){
        return 0;
    }
   
    len = sizeof(a2020_firmware_image2);
    
    msg.addr = audience_a2020_i2c_client->addr;
    msg.flags = 0;
    msg.len = len;
    msg.buf = (char *)a2020_firmware_image2;
    
    ret_val = i2c_transfer(audience_a2020_i2c_client->adapter, &msg, 1);
	
    if (ret_val < 0){
        printk( "[Snd_audience_a2020] aud_bootimage_download_2st NOK !!!\n");   
        return 0;
    }
    
    //printk( "[Snd_audience_a2020] aud_bootimage_download_2st OK !!!\n");
    return 1;
}
#endif


int aud_bootload_initiate(void)
{
    int ret_val = 0;

#if 0 //by elecjang 20110711, for large size firmware, it must be separated
    ret_val = aud_bootimage_download();	
#else
    ret_val = aud_bootimage_download_1st();
	if (ret_val < 0){
		printk( "[Snd_audience_a2020] aud_bootimage_download_1st NOK !!!\n");	 
		return 0;
	}
	
	mdelay(1);
	
    ret_val = aud_bootimage_download_2st();
#endif
    return ret_val;
}

int aud_sync_cmd_to_2020(void)
{
    int ret_val = 0;
    u32 get_value = 0;
    u32 swap_value = 0;
    byte count = 0;
	
    printk( "[Snd_audience_a2020] aud_sync_cmd_to_2020 !!!\n");
	
    while(1)
    {
        ret_val = A2020_msg(A2020_CMD_SYNC, A2020_CMD_SYNC_MODE_POLLING, 4);

        if(ret_val == 1)
        {
            byte read_buf[4];
            if(bBootSequence)
                mdelay(20); //need to wait 20ms for firmware to respond
            else
                mdelay(1); //wait 1ms to try another read attempt
			
            ret_val = A2020_msgCB(4,read_buf);
    
            if(ret_val == 1)
            {
                memcpy((byte*)&get_value, read_buf, 4);
                swap_value = SWAP32(get_value);
                    		
                if(swap_value != A2020_CMD_SYNC_ACK)
                {
                    count++;
                    mdelay(1);
					
                    if(count > POLLING_LOOP_COUNT)
                    {
                        printk( "[Snd_audience_a2020] A2020 Boot Sync CMD ACK NOK @@@@ !!!\n");				
                        break;
                    }
                }
                else
                {
                    printk( "[Snd_audience_a2020] A2020 Boot Sync CMD ACK OK !!!\n");  // -2147483648				
                    break;
                }                
            }
            else
            {
                count++;
                mdelay(1);
                printk( "[Snd_audience_a2020] A2020 Boot Sync POLLING_LOOP_COUNT %d !!!\n",count);					
                if(count > POLLING_LOOP_COUNT)
                {
                    printk( "[Snd_audience_a2020] A2020 Boot Sync POLLING_LOOP_COUNT %d !!!\n",count);					
                    break;
                }
            }			
        }
        else
        {
            count++;   
            mdelay(1);
            printk( "[Snd_audience_a2020] A2020 Boot Sync CMD I2C Write NOK !!!POLLING_LOOP_COUNT : %d !!!\n",count);		

            if(count > POLLING_LOOP_COUNT)
            {
                printk( "[Snd_audience_a2020] A2020 Boot Sync CMD I2C Write NOK ~~ POLLING_LOOP_COUNT : %d !!!\n",count);		
                break;
            }
        }
    }
    printk( "[Snd_audience_a2020] A2020 Boot Sync CMD ret_val : %d !!!\n",ret_val);	
    return ret_val;
}

int aud_sleep_cmd_to_2020(void)
{
    int ret_val = 0;
#ifndef CONFIG_PANTECH_LASER2_AUDIENCE2020	
    u32 get_value = 0;
    u32 swap_value = 0;
#endif
    byte count = 0;
    
    if(aud_a2020_is_sleep_mode())
        return 1;
	
        A2020_msgCB_Chip_AckDataFlush();
        printk( "[Snd_audience_a2020] ######### aud_sync_cmd_to_2020() Test IN  aud_sleep_cmd_to_2020() !!!\n");

    while(1)
    {
        ret_val = A2020_msg(A2020_CMD_SLEEP, A2020_CMD_SLEEP_VALUE, 4);

// if sleep command with eS310, it will enter idle and lowpower state
// so, i2c will be fail '-6'
#ifdef CONFIG_PANTECH_LASER2_AUDIENCE2020
        if(ret_val == 1)
        {
            printk( "[Snd_audience_a2020] A2020 Sleep CMD ACK OK !!!\n");
            break;
        }
        else
        {
            count++;                
            mdelay(1);

            if(count > POLLING_LOOP_COUNT)
            {
                printk( "[Snd_audience_a2020] A2020 Sleep CMD I2C ACK NOK !!!\n");	
                break;
            }
         }			
 #else
        if(ret_val == 1)
        {
            byte read_buf[4];
            mdelay(20); //After sending the sleep command
			
            ret_val = A2020_msgCB(4,read_buf);

            if(ret_val == 1)
            {
                memcpy((byte*)&get_value,read_buf,4);
                swap_value = SWAP32(get_value);
                		
                if(swap_value != A2020_CMD_SLEEP_ACK)
                {
                    count++;                    
                    mdelay(1);	

                    if(count > POLLING_LOOP_COUNT)
                    {
                        printk( "[Snd_audience_a2020] A2020 Sleep CMD ACK NOK !!!\n");			
                        break;
                    }
                }
                else
                {
                    printk( "[Snd_audience_a2020] A2020 Sleep CMD ACK OK !!!\n");
                    break;
                }
            }
            else
            {
                count++;                
                mdelay(1);

                if(count > POLLING_LOOP_COUNT)
                {
                    printk( "[Snd_audience_a2020] A2020 Sleep CMD I2C ACK NOK !!!\n");	
                    break;
                }
            }			
        }
        else
        {
            count++;            
            if(count > POLLING_LOOP_COUNT)
            {
                printk( "[Snd_audience_a2020] A2020 Sleep CMD I2C Write NOK !!!\n");	
                break;
            }
        }
#endif	

    }

    return ret_val;
}

void aud_a2020_clk_on(void)
{
    //Clock Stablize
    //Wait 4 Clock Cycles
    mdelay(4);
}

void aud_a2020_clk_off(void)
{
    if(a2020_mode == A2020_SLEEP_MODE)
    {

    }
}

int aud_a2020_wakeup_sequence(void)
{
    int ret_val = 0;


    printk( "[Snd_audience_a2020] A2020 Wake Up Start !!!\n");	

    aud_a2020_clk_on();

    //First toggle the wake up pin to start the sequence 
    gpio_set_value(GPIO_NR_WAKEUP_N,0);
	

    mdelay(80); //need to wait 30ms for eS310 //by elejang

    ret_val = aud_sync_cmd_to_2020();
    printk( "[Snd_audience_a2020] ===================== A2020 SYNC CHECK 5 ]]]]>>> !!!\n");	
	
    //Put the wake up pin in the default state for the next wake up sequence	
    gpio_set_value(GPIO_NR_WAKEUP_N,1);

    if(ret_val == 1)
    {
        a2020_mode = A2020_WAKEUP_MODE;
        printk( "[Snd_audience_a2020][-=AUDIO=-] A2020 Wake Up Sequence OK !!!\n");
    }
    else
    {
        printk( "[Snd_audience_a2020][-=AUDIO=-] A2020 Wake Up Sequence NOK !!!\n");			
        a2020_mode = A2020_MAX_MODE;
    }
	
    return ret_val;
}


int aud_config_sw_reset_cmd_to_2020(a2020_talk_mode talk_mode)
{
    int ret_val = 0;
    byte count = 0;
    byte index = 0;
    u16 size = 0;

    if(a2020_mode != A2020_ACTIVE_MODE)
        return 0;

    size = sizeof(sw_reset_param);    
    size = size/sizeof(a2020_reset_param);
    
    while(1)
    {
        if(sw_reset_param[index].cmd == 0xFFFF)
            break;

        switch(talk_mode)
        {
            case CLOSE_TALK:
            case CLOSE_1MIC_TALK:
            case CLOSE_VPOFF_TALK:
            case CLOSE_1MIC_VPOFF_TALK:
            case FAR_TALK:
            case FAR_VPOFF_TALK:
            case DV_TALK:  
            case DV_VPOFF_TALK:  
            case VL_PRI_MIC_TALK:
            case VL_SEC_MIC_TALK:
            case CALL_PRI_MIC_TALK:
            case CALL_SEC_MIC_TALK:
                ret_val = A2020_msg(sw_reset_param[index].cmd, sw_reset_param[index].value, 4);	
                break;

           default:
                ret_val = A2020_msg(sw_reset_param[index].cmd, sw_reset_param[index].value, 4);
                break;                
        }

        if(ret_val == 1)
        {
            printk( "[Snd_audience_a2020] A2020 SW Reset Write OK !!!\n");		
            mdelay(200); //by elejang
            index++;

            if(index >= size)
            {
                break;
            }	        
        }
        else
        {
            count++;
            				
            if(count > POLLING_LOOP_COUNT)
            {
                printk( "[Snd_audience_a2020] A2020 SW Reset Write NOK !!!\n");
                break;
            }        
        }
    }

    return ret_val;	
}

int aud_config_device_cmd_to_2020(a2020_talk_mode talk_mode)
{
    int ret_val = 0;
    byte count = 0;
    byte index = 0;
    u16 size = 0;
	
    if(a2020_mode != A2020_ACTIVE_MODE)
        return 0;

    size = sizeof(device_param);
    size = size/sizeof(a2020_device_param);
    
    while(1)
    {
        while(1)
        {
            if(device_param[index].cmd1 == 0xFFFF && device_param[index].cmd2 == 0xFFFF)
                break;

            mdelay(1);

            ret_val = A2020_msg(device_param[index].cmd1, device_param[index].id, 4);				

            if(ret_val == 1)
            {
//                printk( "[Snd_audience_a2020] A2020 Device Param Write OK_1 !!!\n");

                mdelay(1);
        
                if(device_param[index].cmd2 == 0xFFFF && device_param[index].value == 0xFFFF)
                    break;

                ret_val = A2020_msg(device_param[index].cmd2, device_param[index].value, 4);					
                
                if(ret_val == 1)
                {
//                    printk( "[Snd_audience_a2020] A2020 Device Param Write ACK OK_2 !!!\n");
                    break;                
                }
                else
                {
                    count++;
                    			
                    if(count > POLLING_LOOP_COUNT)
                    {
                        printk( "[Snd_audience_a2020] A2020 Device Param Write NOK_2 !!!\n");
                        break;
                    }
                }
            }
            else
            {
                count++;
                				
                if(count > POLLING_LOOP_COUNT)
                {
                    printk( "[Snd_audience_a2020] A2020 Device Param Write NOK_1 !!!\n");	
                    break;
                }            
            }
        }
    
        if(ret_val == 0)
            break;
        else
            index++;
    
        if(index >= size)  
        {
            break;
        }
    }

    return ret_val;
}

int aud_config_algo_cmd_to_2020(a2020_talk_mode talk_mode)
{
    int ret_val = 0;
    byte count = 0;
    byte index = 0;
    u16 size = 0;
    u32 get_value = 0;
    u32 swap_value1 = 0;
    u32 swap_value2 = 0;
	
    if(a2020_mode != A2020_ACTIVE_MODE)
        return 0;

    switch(talk_mode)
    {
        case CLOSE_TALK:
            size = sizeof(algo_ct_param);
            break;

        case CLOSE_VPOFF_TALK:
            size = sizeof(algo_ct_VPOFF_param);
            break;

        case CLOSE_1MIC_TALK:
            size = sizeof(algo_ct_1MIC_param);
            break;
			
        case CLOSE_1MIC_VPOFF_TALK:
            size = sizeof(algo_ct_1MIC_VPOFF_param);
            break;

        case FAR_TALK:
            size = sizeof(algo_ft_param);
            break;

        case FAR_VPOFF_TALK:
            size = sizeof(algo_ft_VPOFF_param);
            break;

        case DV_TALK: 
            size = sizeof(algo_dv_param);
            break;

        case DV_VPOFF_TALK: 
            size = sizeof(algo_dv_VPOFF_param);
            break;

        case VL_PRI_MIC_TALK:
        case VL_SEC_MIC_TALK:
            size = sizeof(algo_ct_vl_MIC_param);
            break;

        case CALL_PRI_MIC_TALK:
        case CALL_SEC_MIC_TALK:
            size = sizeof(algo_ct_call_MIC_param);
            break;

        default:
            size = sizeof(algo_ct_param);			
            printk( "[Snd_audience_a2020] A2020 Param Size Error !!!\n");
            break;
    }

    size = size/sizeof(a2020_algo_param);
    
    while(1)
    {
        while(1)
        {
            if(talk_mode == CLOSE_TALK)
            {
                if(algo_ct_param[index].cmd1 == 0xFFFF && algo_ct_param[index].cmd2 == 0xFFFF)
                    break;
            }
            else if(talk_mode == CLOSE_VPOFF_TALK)
            {
                if(algo_ct_VPOFF_param[index].cmd1 == 0xFFFF && algo_ct_VPOFF_param[index].cmd2 == 0xFFFF)
                    break;            
            }
            else if(talk_mode == CLOSE_1MIC_TALK)
            {
                if(algo_ct_1MIC_param[index].cmd1 == 0xFFFF && algo_ct_1MIC_param[index].cmd2 == 0xFFFF)
                    break;            
            }
            else if(talk_mode == CLOSE_1MIC_VPOFF_TALK)
            {
                if(algo_ct_1MIC_VPOFF_param[index].cmd1 == 0xFFFF && algo_ct_1MIC_VPOFF_param[index].cmd2 == 0xFFFF)
                    break;            
            }
            else if(talk_mode == FAR_TALK)
            {
                if(algo_ft_param[index].cmd1 == 0xFFFF && algo_ft_param[index].cmd2 == 0xFFFF)
                    break;            
            }
            else if(talk_mode == FAR_VPOFF_TALK)
            {
                if(algo_ft_VPOFF_param[index].cmd1 == 0xFFFF && algo_ft_VPOFF_param[index].cmd2 == 0xFFFF)
                    break;            
            }
            else if(talk_mode == DV_TALK) 
            {
                if(algo_dv_param[index].cmd1 == 0xFFFF && algo_dv_param[index].cmd2 == 0xFFFF)
                    break;            
            }
            else if(talk_mode == DV_VPOFF_TALK) 
            {
                if(algo_dv_VPOFF_param[index].cmd1 == 0xFFFF && algo_dv_VPOFF_param[index].cmd2 == 0xFFFF)
                    break;            
            }			
            else if(talk_mode == VL_PRI_MIC_TALK || talk_mode == VL_SEC_MIC_TALK)
            {
                if(algo_ct_vl_MIC_param[index].cmd1 == 0xFFFF && algo_ct_vl_MIC_param[index].cmd2 == 0xFFFF)
                    break;            
            }
            else if(talk_mode == CALL_PRI_MIC_TALK || talk_mode == CALL_SEC_MIC_TALK)
            {
                if(algo_ct_call_MIC_param[index].cmd1 == 0xFFFF && algo_ct_call_MIC_param[index].cmd2 == 0xFFFF)
                    break;            
            }
            else
            {
                if(algo_ct_param[index].cmd1 == 0xFFFF && algo_ct_param[index].cmd2 == 0xFFFF)
                    break;            
            }

            mdelay(1);

            if(talk_mode == CLOSE_TALK)
            {
                ret_val = A2020_msg(algo_ct_param[index].cmd1, algo_ct_param[index].id, 4);
                //printk( "[Snd_audience_a2020]CMD1 : %x, ID : %x!!!", algo_ct_param[index].cmd1, algo_ct_param[index].id);
            }
            else if(talk_mode == CLOSE_VPOFF_TALK)
            {
                ret_val = A2020_msg(algo_ct_VPOFF_param[index].cmd1, algo_ct_VPOFF_param[index].id, 4);
                //printk( "[Snd_audience_a2020]CMD1 : %x, ID : %x!!!", algo_ct_VPOFF_param[index].cmd1, algo_ct_VPOFF_param[index].id);
            }
            else if(talk_mode == CLOSE_1MIC_TALK)
            {
                ret_val = A2020_msg(algo_ct_1MIC_param[index].cmd1, algo_ct_1MIC_param[index].id, 4);
                //printk( "[Snd_audience_a2020]CMD1 : %x, ID : %x!!!", algo_ct_1MIC_param[index].cmd1, algo_ct_1MIC_param[index].id);
            }
            else if(talk_mode == CLOSE_1MIC_VPOFF_TALK)
            {
                ret_val = A2020_msg(algo_ct_1MIC_VPOFF_param[index].cmd1, algo_ct_1MIC_VPOFF_param[index].id, 4);
                //printk( "[Snd_audience_a2020]CMD1 : %x, ID : %x!!!", algo_ct_1MIC_VPOFF_param[index].cmd1, algo_ct_1MIC_VPOFF_param[index].id);
            }
            else if(talk_mode == FAR_TALK)
            {
                A2020_msgCB_Chip_AckDataFlush(); // for msg CB 

                ret_val = A2020_msg(algo_ft_param[index].cmd1, algo_ft_param[index].id, 4);
                //printk( "[Snd_audience_a2020]CMD1 : %x, ID : %x!!!", algo_ft_param[index].cmd1, algo_ft_param[index].id);
            }
            else if(talk_mode == FAR_VPOFF_TALK)
            {
                ret_val = A2020_msg(algo_ft_VPOFF_param[index].cmd1, algo_ft_VPOFF_param[index].id, 4);
                //printk( "[Snd_audience_a2020]CMD1 : %x, ID : %x!!!", algo_ft_VPOFF_param[index].cmd1, algo_ft_VPOFF_param[index].id);
            }
            else if(talk_mode == DV_TALK)
            {
                ret_val = A2020_msg(algo_dv_param[index].cmd1, algo_dv_param[index].id, 4);
                //printk( "[Snd_audience_a2020]CMD1 : %x, ID : %x!!!", algo_dv_param[index].cmd1, algo_dv_param[index].id);
            }
            else if(talk_mode == DV_VPOFF_TALK)
            {
                ret_val = A2020_msg(algo_dv_VPOFF_param[index].cmd1, algo_dv_VPOFF_param[index].id, 4);
                //printk( "[Snd_audience_a2020]CMD1 : %x, ID : %x!!!", algo_dv_VPOFF_param[index].cmd1, algo_dv_VPOFF_param[index].id);
            }			
            else if(talk_mode == VL_PRI_MIC_TALK || talk_mode == VL_SEC_MIC_TALK)
            {
                ret_val = A2020_msg(algo_ct_vl_MIC_param[index].cmd1, algo_ct_vl_MIC_param[index].id, 4);
                //printk( "[Snd_audience_a2020]CMD1 : %x, ID : %x!!!", algo_ct_vl_MIC_param[index].cmd1, algo_ct_vl_MIC_param[index].id);
            }
            else if(talk_mode == CALL_PRI_MIC_TALK || talk_mode == CALL_SEC_MIC_TALK)
            {
                ret_val = A2020_msg(algo_ct_call_MIC_param[index].cmd1, algo_ct_call_MIC_param[index].id, 4);
                //printk( "[Snd_audience_a2020]CMD1 : %x, ID : %x!!!", algo_ct_call_MIC_param[index].cmd1, algo_ct_call_MIC_param[index].id);
            }
            else
            {
                ret_val = A2020_msg(algo_ct_param[index].cmd1, algo_ct_param[index].id, 4);				
                //printk( "[Snd_audience_a2020]CMD1 : %x, ID : %x!!!", algo_ct_param[index].cmd1, algo_ct_param[index].id);
            }

            if(ret_val == 1)
            {
//                printk( "[Snd_audience_a2020][-=AUDIO=-] A2020 Param Write OK_1 !!!\n");

                mdelay(1);
        
#if 1
                if(talk_mode == FAR_TALK)
                {
                    byte read_buf[4];
                    ret_val = A2020_msgCB(4,read_buf);
                    
                    if(ret_val == 1)
                    {
                        memcpy((byte*)&get_value, read_buf, 2);
                        swap_value1 = SWAP16(get_value);
                        	
                        memcpy((byte*)&get_value, &read_buf[2], 2);
                        swap_value2 = SWAP16(get_value);
        
                        if(swap_value1 != A2020_CMD_ALGO_PARAMID)
                        {
                            count++;
                            mdelay(20);
        
                            if(count > POLLING_LOOP_COUNT)
                            {
                                printk( "[Snd_audience_a2020] A2020 Algo CMD1 ACK NOK !!!\n");
                                break;
                            }
                        }
                        else
                        {
                            printk( "[Snd_audience_a2020]Algo CMD1 ACK OK, Val2 = %x, FT ID = %x !!!", swap_value2, algo_ft_param[index].id);
    
                            if(swap_value2 == algo_ft_param[index].id)
                            {
                                printk( "[Snd_audience_a2020] A2020 Algo Param ID ACK OK !!!\n");                        
                            }
                            else
                            {
                                count++;
                                mdelay(20);
            
                                if(count > POLLING_LOOP_COUNT)
                                {
                                    printk( "[Snd_audience_a2020] A2020 Algo Param ID ACK NOK !!!\n");
                                    break;
                                }                            
                            }
                        }
                    }
                    else
                    {
                        count++;
                        mdelay(20);
    
                        if(count > POLLING_LOOP_COUNT)
                        {
                            printk( "[Snd_audience_a2020] A2020 Algo CMD1 Response ACK NOK !!!\n");
                            break;
                        }
                    }
                }
#endif

                if(talk_mode == CLOSE_TALK)
                {
                    if(algo_ct_param[index].cmd2 == 0xFFFF && algo_ct_param[index].value == 0xFFFF)
                        break;
                }					
                else if(talk_mode == CLOSE_VPOFF_TALK)
                {
                    if(algo_ct_VPOFF_param[index].cmd2 == 0xFFFF && algo_ct_VPOFF_param[index].value == 0xFFFF)
                        break;
                }
                else if(talk_mode == CLOSE_1MIC_TALK)
                {
                    if(algo_ct_1MIC_param[index].cmd2 == 0xFFFF && algo_ct_1MIC_param[index].value == 0xFFFF)
                        break;
                }
                else if(talk_mode == CLOSE_1MIC_VPOFF_TALK)
                {
                    if(algo_ct_1MIC_VPOFF_param[index].cmd2 == 0xFFFF && algo_ct_1MIC_VPOFF_param[index].value == 0xFFFF)
                        break;
                }					
                else if(talk_mode == FAR_TALK)
                {
                    if(algo_ft_param[index].cmd2 == 0xFFFF && algo_ft_param[index].value == 0xFFFF)
                        break;
                }
                else if(talk_mode == FAR_VPOFF_TALK)
                {
                    if(algo_ft_VPOFF_param[index].cmd2 == 0xFFFF && algo_ft_VPOFF_param[index].value == 0xFFFF)
                        break;
                }
                else if(talk_mode == DV_TALK) 
                {
                    if(algo_dv_param[index].cmd2 == 0xFFFF && algo_dv_param[index].value == 0xFFFF)
                        break;
                }
                else if(talk_mode == DV_VPOFF_TALK) 
                {
                    if(algo_dv_VPOFF_param[index].cmd2 == 0xFFFF && algo_dv_VPOFF_param[index].value == 0xFFFF)
                        break;
                }
                else if(talk_mode == VL_PRI_MIC_TALK || talk_mode == VL_SEC_MIC_TALK)
                {
                    if(algo_ct_vl_MIC_param[index].cmd2 == 0xFFFF && algo_ct_vl_MIC_param[index].value == 0xFFFF)
                        break;
                }
                else if(talk_mode == CALL_PRI_MIC_TALK || talk_mode == CALL_SEC_MIC_TALK)
                {
                    if(algo_ct_call_MIC_param[index].cmd2 == 0xFFFF && algo_ct_call_MIC_param[index].value == 0xFFFF)
                        break;
                }
                else
                {
                    if(algo_ct_param[index].cmd2 == 0xFFFF && algo_ct_param[index].value == 0xFFFF)
                        break;
                }

                if(talk_mode == CLOSE_TALK)
                {
#ifdef FEATURE_SKY_SND_EXTAMPCTRL  //20110519 jhsong : sub chip cal tool by youngsoo Lim
		    if(LockForTest){
			if((index == 2) || (index == 3) || (index == 4) || (index == 5) ||(index == 6) || (index == 7) || (index == 8) || (index == 9) ||(index == 10)){
	                    ret_val = A2020_msg(algo_ct_param[index].cmd2, a2020_algo_cal[index].value, 4);					
		       }else{
	                    ret_val = A2020_msg(algo_ct_param[index].cmd2, algo_ct_param[index].value, 4);					
		       }
		    }else
#endif
                    ret_val = A2020_msg(algo_ct_param[index].cmd2, algo_ct_param[index].value, 4);					
                    //printk( "[Snd_audience_a2020]CMD2 : %x, Value : %x!!!", algo_ct_param[index].cmd2, algo_ct_param[index].value);
                }
                else if(talk_mode == CLOSE_VPOFF_TALK)
                {
                    ret_val = A2020_msg(algo_ct_VPOFF_param[index].cmd2, algo_ct_VPOFF_param[index].value, 4);
                    //printk( "[Snd_audience_a2020]CMD2 : %x, Value : %x!!!", algo_ct_VPOFF_param[index].cmd2, algo_ct_VPOFF_param[index].value);
                }
                else if(talk_mode == CLOSE_1MIC_TALK)
                {
                    ret_val = A2020_msg(algo_ct_1MIC_param[index].cmd2, algo_ct_1MIC_param[index].value, 4);
                    //printk( "[Snd_audience_a2020]CMD2 : %x, Value : %x!!!", algo_ct_1MIC_param[index].cmd2, algo_ct_1MIC_param[index].value);
                }
                else if(talk_mode == CLOSE_1MIC_VPOFF_TALK)
                {
                    ret_val = A2020_msg(algo_ct_1MIC_VPOFF_param[index].cmd2, algo_ct_1MIC_VPOFF_param[index].value, 4);					
                    //printk( "[Snd_audience_a2020]CMD2 : %x, Value : %x!!!", algo_ct_1MIC_VPOFF_param[index].cmd2, algo_ct_1MIC_VPOFF_param[index].value);
                }
                else if(talk_mode == FAR_TALK)
                {
                	A2020_msgCB_Chip_AckDataFlush(); // for msg CB 
#ifdef FEATURE_SKY_SND_EXTAMPCTRL  //20110519 jhsong : sub chip cal tool by youngsoo Lim
		    if(LockForTest){
			if((index == 2) || (index == 3) || (index == 4) || (index == 5) ||(index == 6) || (index == 7) || (index == 8) || (index == 9) ||(index == 10)){
	                    ret_val = A2020_msg(algo_ft_param[index].cmd2, a2020_algo_cal[index].value, 4);					
		       }else{
	                    ret_val = A2020_msg(algo_ft_param[index].cmd2, algo_ft_param[index].value, 4);
		       }
		    }else
#endif
                    ret_val = A2020_msg(algo_ft_param[index].cmd2, algo_ft_param[index].value, 4);
                    //printk( "[Snd_audience_a2020]CMD2 : %x, Value : %x!!!", algo_ft_param[index].cmd2, algo_ft_param[index].value);
                }
                else if(talk_mode == FAR_VPOFF_TALK)
                {
                    ret_val = A2020_msg(algo_ft_VPOFF_param[index].cmd2, algo_ft_VPOFF_param[index].value, 4);
                   // printk( "[Snd_audience_a2020]CMD2 : %x, Value : %x!!!", algo_ft_VPOFF_param[index].cmd2, algo_ft_VPOFF_param[index].value);
                }
                else if(talk_mode == DV_TALK)
                {
#ifdef FEATURE_SKY_SND_EXTAMPCTRL  //20110519 jhsong : sub chip cal tool by youngsoo Lim
		    if(LockForTest){
			if((index == 2) || (index == 3) || (index == 4) || (index == 5) ||(index == 6) || (index == 7) || (index == 8) || (index == 9) ||(index == 10)){
	                    ret_val = A2020_msg(algo_dv_param[index].cmd2, a2020_algo_cal[index].value, 4);					
		       }else{
	                   ret_val = A2020_msg(algo_dv_param[index].cmd2, algo_dv_param[index].value, 4);
		       }
		    }else
#endif
                    ret_val = A2020_msg(algo_dv_param[index].cmd2, algo_dv_param[index].value, 4);
                    //printk( "[Snd_audience_a2020]CMD2 : %x, Value : %x!!!", algo_dv_param[index].cmd2, algo_dv_param[index].value);
                }
                else if(talk_mode == DV_VPOFF_TALK)
                {
                    ret_val = A2020_msg(algo_dv_VPOFF_param[index].cmd2, algo_dv_VPOFF_param[index].value, 4);
                    //printk( "[Snd_audience_a2020]CMD2 : %x, Value : %x!!!", algo_dv_VPOFF_param[index].cmd2, algo_dv_VPOFF_param[index].value);
                }
                else if(talk_mode == VL_PRI_MIC_TALK || talk_mode == VL_SEC_MIC_TALK)
                {
                    ret_val = A2020_msg(algo_ct_vl_MIC_param[index].cmd2, algo_ct_vl_MIC_param[index].value, 4);
                    //printk( "[Snd_audience_a2020]CMD2 : %x, Value : %x!!!", algo_ct_vl_MIC_param[index].cmd2, algo_ct_vl_MIC_param[index].value);
                }
                else if(talk_mode == CALL_PRI_MIC_TALK || talk_mode == CALL_SEC_MIC_TALK)
                {
                    ret_val = A2020_msg(algo_ct_call_MIC_param[index].cmd2, algo_ct_call_MIC_param[index].value, 4);
                    //printk( "[Snd_audience_a2020]CMD2 : %x, Value : %x!!!", algo_ct_call_MIC_param[index].cmd2, algo_ct_call_MIC_param[index].value);
                }
                else
                {
                    ret_val = A2020_msg(algo_ct_param[index].cmd2, algo_ct_param[index].value, 4);   
                    //printk( "[Snd_audience_a2020]CMD2 : %x, Value : %x!!!", algo_ct_param[index].cmd2, algo_ct_param[index].value);
                }

                if(ret_val == 1)
                {
//                    printk( "[Snd_audience_a2020] A2020 Param Write ACK OK_2 !!!\n");

#if 0//20110621 by elecjang
                    if(talk_mode == FAR_TALK)
                    {
                        byte read_buf[4];
                        ret_val = A2020_msgCB(4,read_buf);
                        
                        if(ret_val == 1)
                        {
                            memcpy((byte*)&get_value, read_buf, 2);
                            swap_value1 = SWAP16(get_value);
                            	
                            memcpy((byte*)&get_value, &read_buf[2], 2);
                            swap_value2 = SWAP16(get_value);
            
                            if(swap_value1 != A2020_CMD_ALGO_PARAM)
                            {
                                count++;
                                mdelay(20);
            
                                if(count > POLLING_LOOP_COUNT)
                                {
                                    printk( "[Snd_audience_a2020] A2020 Algo CMD2 ACK NOK !!!\n");
                                    break;
                                }
                            }
                            else
                            {
//                                printk( "[Snd_audience_a2020]Algo CMD2 ACK OK, Val2 = %x, FT ID = %x !!!", swap_value2, algo_ft_param[index].value);
        
                                if(swap_value2 == algo_ft_param[index].value)
                                {
//                                    printk( "[Snd_audience_a2020] A2020 Algo Value ACK OK !!!\n");
                                    break;
                                }
                                else
                                {
                                    count++;
                                    mdelay(20);
                
                                    if(count > POLLING_LOOP_COUNT)
                                    {
                                        printk( "[Snd_audience_a2020] A2020 Algo Value ACK NOK !!!\n");
                                        break;
                                    }                            
                                }
                            }
                        }
                        else
                        {
                            count++;
                            mdelay(20);
        
                            if(count > POLLING_LOOP_COUNT)
                            {
                                printk( "[Snd_audience_a2020] A2020 Algo CMD2 Response ACK NOK !!!\n");
                                break;
                            }
                        }
                    }
                    else
#endif
                    break;
                }
                else
                {
                    count++;
                    			
                    if(count > POLLING_LOOP_COUNT)
                    {
                        printk( "[Snd_audience_a2020] A2020 Param Write NOK_2 !!!\n");
                        break;
                    }
                }
            }
            else
            {
                count++;
                				
                if(count > POLLING_LOOP_COUNT)
                {
                    printk( "[Snd_audience_a2020] A2020 Param Write NOK_1 !!!\n");	
                    break;
                }            
            }
        }
    
        if(ret_val == 0)
        {
            printk( "[Snd_audience_a2020] A2020 Algo Param Write Retry !!!\n");
            //break;
        }
        else
            index++;
    
        if(index >= size) 
        {
            break;
        }
    }

    return ret_val;
}

int aud_config_path_vp_cmd_to_2020(a2020_talk_mode talk_mode)
{
    int ret_val = 0;
    byte count = 0;
    byte index = 0;
    u16 size = 0;

    if(a2020_mode != A2020_ACTIVE_MODE)
        return 0;

    size = sizeof(path_vp_on_param);    
    size = size/sizeof(a2020_path_param);
    
    while(1)
    {
        if(path_vp_on_param[index].cmd == 0xFFFF)
            break;

        switch(talk_mode)
        {
            case CLOSE_TALK:
            case CLOSE_1MIC_TALK:
            case FAR_TALK:
            case DV_TALK:
            case VL_PRI_MIC_TALK:
            case VL_SEC_MIC_TALK:
            case CALL_PRI_MIC_TALK:
            case CALL_SEC_MIC_TALK:
                ret_val = A2020_msg(path_vp_on_param[index].cmd, path_vp_on_param[index].value, 4);	
                break;

            case CLOSE_VPOFF_TALK:
            case CLOSE_1MIC_VPOFF_TALK:
            case FAR_VPOFF_TALK:
            case DV_VPOFF_TALK: 
                ret_val = A2020_msg(path_vp_off_param[index].cmd, path_vp_off_param[index].value, 4);	
                break;

            default:
                ret_val = A2020_msg(path_vp_on_param[index].cmd, path_vp_on_param[index].value, 4);
                break;                
        }

        if(ret_val == 1)
        {
            printk( "[Snd_audience_a2020] A2020 Path VP Param Write OK !!!\n");		
            mdelay(1);			
            index++;

            if(index >= size)
            {
                break;
            }	        
        }
        else
        {
            count++;
            				
            if(count > POLLING_LOOP_COUNT)
            {
                printk( "[Snd_audience_a2020] A2020 Path VP Param Write NOK !!!\n");
                break;
            }        
        }
    }

    return ret_val;	
}

int aud_config_path_keytone_gain_cmd_to_2020(void)
{
    int ret_val = 0;
    byte count = 0;
    byte index = 0;
    u16 size = 0;

    if(a2020_mode != A2020_ACTIVE_MODE)
        return 0;

    size = sizeof(path_keytone_gain_param);
	
    size = size/sizeof(a2020_path_param);
    
    while(1)
    {
        if(path_keytone_gain_param[index].cmd == 0xFFFF)
            break;

        ret_val = A2020_msg(path_keytone_gain_param[index].cmd, path_keytone_gain_param[index].value, 4);

        if(ret_val == 1)
        {
//            printk( "[Snd_audience_a2020] A2020 Path Digital KeyTone Gain Param Write OK !!!\n");		
            index++;

            if(index >= size)
            {
                break;
            }	        
        }
        else
        {
            count++;
            				
            if(count > POLLING_LOOP_COUNT)
            {
                printk( "[Snd_audience_a2020] A2020 Path Digital KeyTone Gain Param Write NOK !!!\n");
                break;
            }        
        }
    }

    return ret_val;	
}

int aud_config_path_gain_cmd_to_2020(a2020_talk_mode talk_mode)
{
    int ret_val = 0;
    byte count = 0;
    byte index = 0;
    u16 size = 0;

    if(a2020_mode != A2020_ACTIVE_MODE)
        return 0;

    switch(talk_mode)
    {
        case CLOSE_TALK:
            size = sizeof(path_ct_gain_param);
            break;

        case CLOSE_VPOFF_TALK:
            size = sizeof(path_ct_VPOFF_gain_param);
            break;

        case CLOSE_1MIC_TALK:
            size = sizeof(path_ct_1MIC_gain_param);
            break;

        case CLOSE_1MIC_VPOFF_TALK:
            size = sizeof(path_ct_1MIC_VPOFF_gain_param);
            break;

        case FAR_TALK:
            size = sizeof(path_ft_gain_param);
            break;
			
        case FAR_VPOFF_TALK:
            size = sizeof(path_ft_VPOFF_gain_param);
            break;

        case DV_TALK: 
            size = sizeof(path_dv_gain_param);
            break;
			
        case DV_VPOFF_TALK: 
            size = sizeof(path_dv_VPOFF_gain_param);
            break;


        case VL_PRI_MIC_TALK:
        case VL_SEC_MIC_TALK:
            size = sizeof(path_vl_MIC_gain_param);
            break;

        case CALL_PRI_MIC_TALK:
        case CALL_SEC_MIC_TALK:
            size = sizeof(path_call_MIC_gain_param);
            break;

        default:
            size = sizeof(path_ct_gain_param);
            printk( "[Snd_audience_a2020] A2020 Path Digital Gain Param Size Error !!!\n");			
            break;		
    }
	
    size = size/sizeof(a2020_path_param);
    
    while(1)
    {
        if(talk_mode == CLOSE_TALK)
        {
            if(path_ct_gain_param[index].cmd == 0xFFFF)
                break;
        }
        else if(talk_mode == CLOSE_VPOFF_TALK)
        {
            if(path_ct_VPOFF_gain_param[index].cmd == 0xFFFF)
                break;
        }
        else if(talk_mode == CLOSE_1MIC_TALK)
        {
            if(path_ct_1MIC_gain_param[index].cmd == 0xFFFF)
                break;
        }
        else if(talk_mode == CLOSE_1MIC_VPOFF_TALK)
        {
            if(path_ct_1MIC_VPOFF_gain_param[index].cmd == 0xFFFF)
                break;
        }
        else if(talk_mode == FAR_TALK)
        {
            if(path_ft_gain_param[index].cmd == 0xFFFF)
                break;
        }
        else if(talk_mode == FAR_VPOFF_TALK)
        {
            if(path_ft_VPOFF_gain_param[index].cmd == 0xFFFF)
                break;
        }
        else if(talk_mode == DV_TALK) 
        {
            if(path_dv_gain_param[index].cmd == 0xFFFF)
                break;
        }
        else if(talk_mode == DV_VPOFF_TALK) 
        {
            if(path_dv_VPOFF_gain_param[index].cmd == 0xFFFF)
                break;
        }		
        else if(talk_mode == VL_PRI_MIC_TALK || talk_mode == VL_SEC_MIC_TALK)
        {
            if(path_vl_MIC_gain_param[index].cmd == 0xFFFF)
                break;
        }
        else if(talk_mode == CALL_PRI_MIC_TALK || talk_mode == CALL_SEC_MIC_TALK)
        {
            if(path_call_MIC_gain_param[index].cmd == 0xFFFF)
                break;
        }			
        else
        {
            if(path_ct_gain_param[index].cmd == 0xFFFF)
                break;
        }

#ifdef FEATURE_SKY_SND_EXTAMPCTRL  //20110519 jhsong : sub chip cal tool by youngsoo Lim
	if(LockForTest){
	        if(talk_mode == CLOSE_TALK)
	            ret_val = A2020_msg(path_ct_gain_param[index].cmd, a2020_gain_cal[index].value, 4);
	        else if(talk_mode == CLOSE_VPOFF_TALK)
	            ret_val = A2020_msg(path_ct_VPOFF_gain_param[index].cmd, path_ct_VPOFF_gain_param[index].value, 4);
	        else if(talk_mode == CLOSE_1MIC_TALK)
	            ret_val = A2020_msg(path_ct_1MIC_gain_param[index].cmd, path_ct_1MIC_gain_param[index].value, 4);
	        else if(talk_mode == CLOSE_1MIC_VPOFF_TALK)
	            ret_val = A2020_msg(path_ct_1MIC_VPOFF_gain_param[index].cmd, path_ct_1MIC_VPOFF_gain_param[index].value, 4);
	        else if(talk_mode == FAR_TALK)
	            ret_val = A2020_msg(path_ft_gain_param[index].cmd, a2020_gain_cal[index].value, 4);
	        else if(talk_mode == FAR_VPOFF_TALK)
	            ret_val = A2020_msg(path_ft_VPOFF_gain_param[index].cmd, path_ft_VPOFF_gain_param[index].value, 4);
	        else if(talk_mode == DV_TALK) // jmlee
	            ret_val = A2020_msg(path_dv_gain_param[index].cmd, a2020_gain_cal[index].value, 4);
	        else if(talk_mode == DV_VPOFF_TALK) // jmlee
	            ret_val = A2020_msg(path_dv_VPOFF_gain_param[index].cmd, path_dv_VPOFF_gain_param[index].value, 4);		
	        else if(talk_mode == VL_PRI_MIC_TALK || talk_mode == VL_SEC_MIC_TALK)
	            ret_val = A2020_msg(path_vl_MIC_gain_param[index].cmd, path_vl_MIC_gain_param[index].value, 4);
	        else if(talk_mode == CALL_PRI_MIC_TALK || talk_mode == CALL_SEC_MIC_TALK)
	            ret_val = A2020_msg(path_call_MIC_gain_param[index].cmd, path_call_MIC_gain_param[index].value, 4);
	        else
	            ret_val = A2020_msg(path_ct_gain_param[index].cmd, path_ct_gain_param[index].value, 4);		
	}else
#endif
	        if(talk_mode == CLOSE_TALK)
	            ret_val = A2020_msg(path_ct_gain_param[index].cmd, path_ct_gain_param[index].value, 4);
	        else if(talk_mode == CLOSE_VPOFF_TALK)
	            ret_val = A2020_msg(path_ct_VPOFF_gain_param[index].cmd, path_ct_VPOFF_gain_param[index].value, 4);
	        else if(talk_mode == CLOSE_1MIC_TALK)
	            ret_val = A2020_msg(path_ct_1MIC_gain_param[index].cmd, path_ct_1MIC_gain_param[index].value, 4);
	        else if(talk_mode == CLOSE_1MIC_VPOFF_TALK)
	            ret_val = A2020_msg(path_ct_1MIC_VPOFF_gain_param[index].cmd, path_ct_1MIC_VPOFF_gain_param[index].value, 4);
	        else if(talk_mode == FAR_TALK)
	            ret_val = A2020_msg(path_ft_gain_param[index].cmd, path_ft_gain_param[index].value, 4);
	        else if(talk_mode == FAR_VPOFF_TALK)
	            ret_val = A2020_msg(path_ft_VPOFF_gain_param[index].cmd, path_ft_VPOFF_gain_param[index].value, 4);
	        else if(talk_mode == DV_TALK) // jmlee
	            ret_val = A2020_msg(path_dv_gain_param[index].cmd, path_dv_gain_param[index].value, 4);
	        else if(talk_mode == DV_VPOFF_TALK) // jmlee
	            ret_val = A2020_msg(path_dv_VPOFF_gain_param[index].cmd, path_dv_VPOFF_gain_param[index].value, 4);		
	        else if(talk_mode == VL_PRI_MIC_TALK || talk_mode == VL_SEC_MIC_TALK)
	            ret_val = A2020_msg(path_vl_MIC_gain_param[index].cmd, path_vl_MIC_gain_param[index].value, 4);
	        else if(talk_mode == CALL_PRI_MIC_TALK || talk_mode == CALL_SEC_MIC_TALK)
	            ret_val = A2020_msg(path_call_MIC_gain_param[index].cmd, path_call_MIC_gain_param[index].value, 4);
	        else
	            ret_val = A2020_msg(path_ct_gain_param[index].cmd, path_ct_gain_param[index].value, 4);

//        printk( "[Snd_audience_a2020][-=AUDIO=-] A2020 Path Digital Gain Param !!!\n");

        if(ret_val == 1)
        {
            printk( "[Snd_audience_a2020] A2020 Path Digital Gain Param Write OK !!!\n");		
            mdelay(1);
            index++;

            if(index >= size)
            {
                break;
            }	        
        }
        else
        {
            count++;
            				
            if(count > POLLING_LOOP_COUNT)
            {
                printk( "[Snd_audience_a2020] A2020 Path Digital Gain Param Write NOK !!!\n");
                break;
            }        
        }
    }

    return ret_val;	
}

int aud_config_path_routing_cmd_to_2020(a2020_talk_mode talk_mode)
{
    int ret_val = 0;
    byte count = 0;
    byte index = 0;
    u16 size = 0;

    if(a2020_mode != A2020_ACTIVE_MODE)
        return 0;

    size = sizeof(path_routing_param);    
    size = size/sizeof(a2020_path_param);
    
    while(1)
    {
        if(path_routing_param[index].cmd == 0xFFFF)
            break;

        switch(talk_mode)
        {
            case CLOSE_TALK:
                ret_val = A2020_msg(path_routing_param[index].cmd, path_routing_param[index].value, 4);	
                break;

            case CLOSE_1MIC_TALK:
            case CLOSE_VPOFF_TALK:
            case CLOSE_1MIC_VPOFF_TALK:
            case FAR_TALK:
            case FAR_VPOFF_TALK:
                ret_val = A2020_msg(path_ct_A_routing_param[index].cmd, path_ct_A_routing_param[index].value, 4);	
                break;
            case DV_TALK: 
            case DV_VPOFF_TALK: 
                ret_val = A2020_msg(path_call_sec_DV_MIC_routing_param[index].cmd, path_call_sec_DV_MIC_routing_param[index].value, 4);	
                break;
            case VL_PRI_MIC_TALK:
                ret_val = A2020_msg(path_vl_pri_MIC_routing_param[index].cmd, path_vl_pri_MIC_routing_param[index].value, 4);	
                break;

            case VL_SEC_MIC_TALK:
                ret_val = A2020_msg(path_vl_sec_MIC_routing_param[index].cmd, path_vl_sec_MIC_routing_param[index].value, 4);	
                break;

            case CALL_PRI_MIC_TALK:
                ret_val = A2020_msg(path_call_pri_MIC_routing_param[index].cmd, path_call_pri_MIC_routing_param[index].value, 4);	
                break;

            case CALL_SEC_MIC_TALK:
                ret_val = A2020_msg(path_call_sec_MIC_routing_param[index].cmd, path_call_sec_MIC_routing_param[index].value, 4);	
                break;

            default:
                ret_val = A2020_msg(path_routing_param[index].cmd, path_routing_param[index].value, 4);
                break;                
        }

        if(ret_val == 1)
        {
            printk( "[Snd_audience_a2020] A2020 Path Audio Routing Param Write OK !!!\n");		
            mdelay(1);			
            index++;

            if(index >= size)
            {
                break;
            }	        
        }
        else
        {
            count++;
            				
            if(count > POLLING_LOOP_COUNT)
            {
                printk( "[Snd_audience_a2020] A2020 Path Audio Routing Param Write NOK !!!\n");
                break;
            }        
        }
    }

    return ret_val;	
}

int aud_config_algo_path_cmd_to_2020(a2020_talk_mode talk_mode)
{
    int ret_val = 0;

    printk( "[Snd_audience_a2020][-=AUDIO=-] prev_talk_mode = %d, curr_talk_mode = %d \n", prev_talk_mode, talk_mode);	

#if 0//deleted by elecjang 20110630
    ret_val = aud_config_sw_reset_cmd_to_2020(talk_mode);
    if(ret_val == 0)
    {
        printk( "[Snd_audience_a2020][-=AUDIO=-] A2020 Path SW Reset Param Configration NOK !!!\n");
    }
#endif

    ret_val = aud_config_path_vp_cmd_to_2020(talk_mode);
    if(ret_val == 0)
    {
        printk( "[Snd_audience_a2020][-=AUDIO=-] A2020 Path VP Param Configration NOK !!!\n");
    }

#if 1 // jmlee  todo for presto
    ret_val = aud_config_device_cmd_to_2020(talk_mode);
    if(ret_val == 0)
    {
        printk( "[Snd_audience_a2020] A2020 Set Device Gain Param Configration NOK !!!\n");
    }
#endif

    ret_val = aud_config_path_gain_cmd_to_2020(talk_mode);

    if(ret_val == 0)
    {
        printk( "[Snd_audience_a2020] A2020 Path Digital Gain Param Configration NOK !!!\n");
    }

    ret_val = aud_config_algo_cmd_to_2020(talk_mode);

    if(ret_val == 0)
    {
        printk( "[Snd_audience_a2020] A2020 Algo Param Configration NOK !!!\n");
    }

    ret_val = aud_config_path_routing_cmd_to_2020(talk_mode);
    if(ret_val == 0)
    {
        printk( "[Snd_audience_a2020] A2020 Path Audio Routing Param Configration NOK !!!\n");
    }

    prev_talk_mode = talk_mode;

    return ret_val;
}

int aud_a2020_active_mode_sequence(a2020_talk_mode talk_mode)
{
    int ret_val = 0;

    if(bBootSequence == 0)
    {
        return ret_val;
    }

    if(a2020_mode == A2020_ACTIVE_MODE)
    {
        ret_val = aud_config_algo_path_cmd_to_2020(talk_mode);
        return ret_val;
    }

    printk( "[Snd_audience_a2020] A2020 Active Sequence Start !!!\n");	

    ret_val = aud_a2020_wakeup_sequence();

/*================= End of Wake up Sequence ==================*/

    if(ret_val == 1)
    {
        a2020_mode = A2020_ACTIVE_MODE;
		
        aud_config_algo_path_cmd_to_2020(talk_mode);
        printk( "[Snd_audience_a2020] A2020 Active Sequence OK !!!\n");
    }
    else
    {
        a2020_mode = A2020_MAX_MODE;
        printk( "[Snd_audience_a2020] A2020 Active Sequence NOK !!!\n");		
    }
	
    printk( "[Snd_audience_a2020] A2020 Active Sequence End !!!\n");	

    return ret_val;
}

int aud_a2020_sleep_sequence(void)
{
    int ret_val = 1;

        printk( "[Snd_audience_a2020] A2020 Sleep Sequence !!!\n");
		
	
    if((a2020_mode == A2020_SLEEP_MODE) || (bBootSequence == 0) )
    {
        return 1;
    }
    sleep_status = A2020_SLEEP_START;
    ret_val = aud_sleep_cmd_to_2020();		

    if(ret_val)
    {
        mdelay(120);

        prev_talk_mode = MAX_TALK;
        a2020_mode = A2020_SLEEP_MODE;
        sleep_status = A2020_SLEEP_END;
        aud_a2020_clk_off();
//        printk( "[Snd_audience_a2020] A2020 Sleep Sequence OK !!!\n");
    }
    else
    {
        aud_a2020_clk_off();
        a2020_mode = A2020_MAX_MODE;
        sleep_status = A2020_SLEEP_NONE;
        printk( "[Snd_audience_a2020] A2020 Sleep Sequence NOK !!!\n");
    }

    return ret_val;
}


int aud_a2020_boot_sequence(void)
{
    int ret_val = 0;
    byte count = 0;

AGAIN_BOOT:

    a2020_mode = A2020_MAX_MODE;
    bBootSequence = 0;

    gpio_tlmm_config(GPIO_CFG(GPIO_NR_WAKEUP_N, 0, GPIO_CFG_OUTPUT,	GPIO_CFG_PULL_UP, GPIO_CFG_2MA), GPIO_CFG_ENABLE);
    gpio_set_value(GPIO_NR_WAKEUP_N,1);

    gpio_tlmm_config(GPIO_CFG(GPIO_NR_RESET_N, 0, GPIO_CFG_OUTPUT,	GPIO_CFG_PULL_DOWN, GPIO_CFG_2MA), GPIO_CFG_ENABLE);
    gpio_set_value(GPIO_NR_RESET_N,0);

    //Clock On 
    aud_a2020_clk_on();
	    
    //Reset High       
    gpio_set_value(GPIO_NR_RESET_N,1);

    //Wait 50ms
    mdelay(50);
	
    ret_val = aud_boot_cmd_to_2020();

    if(ret_val == 1)
    {
        ret_val = aud_bootload_initiate();

        if(ret_val == 1)
        {
            mdelay(10); //need to wait 10ms before firmware is ready

            ret_val = aud_sync_cmd_to_2020();

           if(ret_val == 1)
           {
               a2020_mode = A2020_BOOTUP_MODE;
               bBootSequence = 1;

               ret_val = aud_a2020_sleep_sequence();
           }
           else
           {
               count++;
               if(count < (POLLING_LOOP_COUNT))
               {
                   goto AGAIN_BOOT;
               }
           }		   
       }
       else
       {
           count++;
           if(count < (POLLING_LOOP_COUNT))
           {
               goto AGAIN_BOOT;
           }
        }
    }
    else
    {
        count++;
        if(count < (POLLING_LOOP_COUNT))
        {
            goto  AGAIN_BOOT;
        }
    }
	
//    printk( "[Snd_audience_a2020] A2020 Boot Up OK !!!\n");

    if(ret_val == 0)
    {
        a2020_mode = A2020_MAX_MODE;
        aud_a2020_clk_off();

        printk( "[Snd_audience_a2020] A2020 Boot Up NOK !!!\n");
    }

    return ret_val;
}

///////////////////////////////// >>>>>
// refered by marimba snddev_ecodec_data  [snddev_data_marimba.c]
// called at [snddev_ecodec.c] ==> snddev_ecodec_open/close function.

/*==========================================================================
** snd_subsystem_pmic_vreg_l2_on
**=========================================================================*/
static struct vreg *vreg_wlan_audience_system;  // wlan vreg control ( VDD CODEC: 2.7V)
#if (BOARD_VER <= WS20)
static struct vreg *vreg_gp6_audience_system;   // gp6 vreg control (VDD_IO, VDD_LDO: 1.8V)
#endif

int audience_pmic_vreg_switch_on(void)
{
	int rc;

	vreg_wlan_audience_system = vreg_get(NULL, "wlan");  // wlan vreg.c
#if (BOARD_VER <= WS20)
	vreg_gp6_audience_system = vreg_get(NULL, "gp6");    // gp6 vreg.c
#endif

	//printk(" audience_pmic_vreg_switch_on rc:%d\n", rc);

	rc = vreg_set_level(vreg_wlan_audience_system, 2700);
	if (rc){
		printk(KERN_ERR "%s: vreg_set_level failed(%s) = %d \n", __func__, "wlan", rc);
		return rc;
	}

	rc = vreg_enable(vreg_wlan_audience_system);
	if (rc){
		printk(KERN_ERR "%s: vreg_enable failed(%s) = %d \n", __func__, "wlan", rc);
		return rc;
	}

#if (BOARD_VER <= WS20)
	rc = vreg_set_level(vreg_gp6_audience_system, 1800);
	if (rc){
		printk(KERN_ERR "%s: vreg_set_level failed(%s) = %d \n", __func__, "gp6", rc);
		return rc;
	}

	rc = vreg_enable(vreg_gp6_audience_system);
	if (rc){
		printk(KERN_ERR "%s: vreg_enable failed(%s) = %d \n", __func__, "gp6", rc);
		return rc;
	}
#endif

#ifdef FEATURE_SKY_SND_EXTAMPCTRL
	ext_audience_pth_on = 1;
#endif
	return rc;
}


/*==========================================================================
** snd_subsystem_pmic_vreg_l2_off
**=========================================================================*/
int audience_pmic_vreg_switch_off(void)
{

	int rc;

	//printk("audience_pmic_vreg_switch_off rc:%d \n", rc);

	if (IS_ERR(vreg_wlan_audience_system)) {
		printk("%s: vreg_get(%s) failed (%ld)\n",
		__func__, "wlan", PTR_ERR(vreg_wlan_audience_system));
		return -1;
	}

	rc = vreg_disable(vreg_wlan_audience_system);

	if (rc){
		printk("%s: vreg_disable(%s) failed (%d)\n", __func__, "wlan", rc);
		return rc;
	}

#ifdef FEATURE_SKY_SND_EXTAMPCTRL
	ext_audience_pth_on = 0;
#endif

	vreg_put(vreg_wlan_audience_system);

#if (BOARD_VER <= WS20)
	if (IS_ERR(vreg_gp6_audience_system)) {
		printk("%s: vreg_get(%s) failed (%ld)\n",
		__func__, "gp6", PTR_ERR(vreg_gp6_audience_system));
		return -1;
	}

	rc = vreg_disable(vreg_gp6_audience_system);

	if (rc){
		printk("%s: vreg_disable(%s) failed (%d)\n", __func__, "gp6", rc);
		return rc;
	}

	vreg_put(vreg_gp6_audience_system);
#endif

	return rc;
}


static unsigned audience_config_gpio_on[] = {
	GPIO_CFG(138, 1, GPIO_CFG_OUTPUT, GPIO_CFG_NO_PULL, GPIO_CFG_2MA),	/* PCM_DOUT */
	GPIO_CFG(139, 1, GPIO_CFG_INPUT,  GPIO_CFG_NO_PULL, GPIO_CFG_2MA),	/* PCM_DIN */
	GPIO_CFG(140, 1, GPIO_CFG_OUTPUT, GPIO_CFG_NO_PULL, GPIO_CFG_2MA),	/* PCM_SYNC */
	GPIO_CFG(141, 1, GPIO_CFG_OUTPUT, GPIO_CFG_NO_PULL, GPIO_CFG_2MA),	/* PCM_CLK */
};

static unsigned audience_config_gpio_off[] = {
	GPIO_CFG(138, 0, GPIO_CFG_INPUT, GPIO_CFG_PULL_DOWN, GPIO_CFG_2MA),	/* PCM_DOUT */
	GPIO_CFG(139, 0, GPIO_CFG_INPUT, GPIO_CFG_PULL_DOWN, GPIO_CFG_2MA),	/* PCM_DIN */
	GPIO_CFG(140, 0, GPIO_CFG_INPUT, GPIO_CFG_PULL_DOWN, GPIO_CFG_2MA),	/* PCM_SYNC */
	GPIO_CFG(141, 0, GPIO_CFG_INPUT, GPIO_CFG_PULL_DOWN, GPIO_CFG_2MA),	/* PCM_CLK */};

static int Audience_Config_GPIO_onoff(int on)
{
	int pin, rc;

	if (on) {
		for (pin = 0; pin < ARRAY_SIZE(audience_config_gpio_on); pin++) {
			rc = gpio_tlmm_config(audience_config_gpio_on[pin], GPIO_CFG_ENABLE);
			if (rc) {
				printk(KERN_ERR "%s: gpio_tlmm_config(%#x)=%d\n",	__func__, audience_config_gpio_on[pin], rc);
				return -EIO;
			}
		}
	} else {
		for (pin = 0; pin < ARRAY_SIZE(audience_config_gpio_off); pin++) {
			rc = gpio_tlmm_config(audience_config_gpio_off[pin], GPIO_CFG_ENABLE);
			if (rc) {
				printk(KERN_ERR "%s: gpio_tlmm_config(%#x)=%d\n",	__func__, audience_config_gpio_off[pin], rc);
				return -EIO;
			}
		}
	}
	return 0;
}

void audience_a2020_ct_on(void)
{

     printk("[Snd_audience_a2020] #@#@#@#@#@#@#@#@#  [[ CLOSE_TALK ]] BY ECODEC  \n");
     audience_pmic_vreg_switch_on();   //20110301 jhsong : rf 2.2VReg Control
     Audience_Config_GPIO_onoff(1); // Aux Pcm(about PCM Block <--> BT)
#ifdef FEATURE_SKY_SND_EXTAMPCTRL
	 snd_extamp_api_audience_SetVolume_ById(EXTAMP_NR_ON_CT);
#endif
     aud_a2020_active_mode_sequence(CLOSE_TALK);
     msm_snddev_mic_sel_a2020();
     msm_snddev_nr_out_sel_A2020Reciever_nr_receiver_amp_on();
}

void audience_a2020_ct_2mic_on(void)
{
     printk("[Snd_audience_a2020] #@#@#@#@#@#@#@#@#  [[ 2mic only ]] BY ECODEC  \n");
     audience_pmic_vreg_switch_on();   //20110301 jhsong : rf 2.2VReg Control
     Audience_Config_GPIO_onoff(1); // Aux Pcm(about PCM Block <--> BT)
     aud_a2020_active_mode_sequence(VL_SEC_MIC_TALK);
     msm_snddev_mic_sel_a2020();
     msm_snddev_nr_out_sel_A2020Reciever_nr_receiver_amp_on();
}

// not used
void audience_a2020_ct_vpoff_on(void)
{
     printk("[Snd_audience_a2020] #@#@#@#@#@#@#@#@#  [[ CLOSE_VPOFF_TALK ]] BY ECODEC  \n");
     audience_pmic_vreg_switch_on();   //20110301 jhsong : rf 2.2VReg Control
     Audience_Config_GPIO_onoff(1); // Aux Pcm(about PCM Block <--> BT)
     aud_a2020_active_mode_sequence(CLOSE_VPOFF_TALK);
     msm_snddev_mic_sel_a2020();	 
     msm_snddev_nr_out_sel_A2020Reciever_nr_receiver_amp_on();	
}


void audience_a2020_ft_on(void)
{
     printk("[Snd_audience_a2020] #@#@#@#@#@#@#@#@#  [[ FAR_TALK ]] BY ECODEC  \n");
     audience_pmic_vreg_switch_on();   //20110301 jhsong : rf 2.2VReg Control
     Audience_Config_GPIO_onoff(1); // Aux Pcm(about PCM Block <--> BT)
#ifdef FEATURE_SKY_SND_EXTAMPCTRL
     snd_extamp_api_audience_SetVolume_ById(EXTAMP_NR_ON_FT);
#endif
     aud_a2020_active_mode_sequence(FAR_TALK);
     msm_snddev_mic_sel_a2020();
     //msm_snddev_nr_receiver_amp_off();
     msm_snddev_nr_out_sel_qtrReceiver_or_A2020Speaker();
     mdelay(1);
     msm_snddev_nr_receiver_amp_off();
     // FAB2200 ON Insert !!
     snd_subsystem_nr_ft_poweron();  
}

// not used
void audience_a2020_ft_vpoff_on(void)
{
     printk("[Snd_audience_a2020] #@#@#@#@#@#@#@#@#  [[ FAR_VPOFF_TALK ]] BY ECODEC  \n");
     audience_pmic_vreg_switch_on();   //20110301 jhsong : rf 2.2VReg Control
     Audience_Config_GPIO_onoff(1); // Aux Pcm(about PCM Block <--> BT)	 
     aud_a2020_active_mode_sequence(FAR_VPOFF_TALK);
     msm_snddev_mic_sel_a2020();	 
     msm_snddev_nr_out_sel_qtrReceiver_or_A2020Speaker();
     mdelay(1);
     msm_snddev_nr_receiver_amp_off();	
     snd_subsystem_nr_off_ft_poweron();
}

void audience_a2020_dv_on(void)
{
     printk("[Snd_audience_a2020] #@#@#@#@#@#@#@#@#  [[ DV_TALK ]] BY ECODEC  \n");
     audience_pmic_vreg_switch_on();   //20110301 jhsong : rf 2.2VReg Control
     Audience_Config_GPIO_onoff(1); // Aux Pcm(about PCM Block <--> BT)	 
#ifdef FEATURE_SKY_SND_EXTAMPCTRL
     snd_extamp_api_audience_SetVolume_ById(EXTAMP_NR_ON_DV);
#endif
     aud_a2020_active_mode_sequence(DV_TALK);
     msm_snddev_mic_sel_a2020();
     //msm_snddev_nr_receiver_amp_off();
     msm_snddev_nr_out_sel_qtrReceiver_or_A2020Speaker();
     mdelay(1);
     msm_snddev_nr_receiver_amp_off();
     // FAB2200 ON Insert !!
     snd_subsystem_nr_dv_poweron(); 
}

// not used
void audience_a2020_dv_vpoff_on(void)
{
     printk("[Snd_audience_a2020] #@#@#@#@#@#@#@#@#  [[ DV_VPOFF_TALK ]] BY ECODEC  \n");
     audience_pmic_vreg_switch_on();   //20110301 jhsong : rf 2.2VReg Control
     Audience_Config_GPIO_onoff(0); // Aux Pcm(about PCM Block <--> BT)	 
     aud_a2020_active_mode_sequence(DV_VPOFF_TALK);
     msm_snddev_mic_sel_a2020();	 
}

void audience_a2020_off(void)
{
     printk("[Snd_audience_a2020] #@#@#@#@#@#@#@#@#  [[ AUDIENCE OFF ]] BY ECODEC  \n");
     // FAB2200 OFF Insert !!
     snd_subsystem_sp_powerdown(); // temp setting
     Audience_Config_GPIO_onoff(1); // Aux Pcm(about PCM Block <--> BT)	 
     aud_a2020_sleep_sequence();
     msm_snddev_nr_receiver_amp_off();
     audience_pmic_vreg_switch_off();	   //20110301 jhsong : rf 2.2VReg Control
}

void audience_a2020_ct_off(void)
{
     printk("[Snd_audience_a2020] #@#@#@#@#@#@#@#@#  [[ AUDIENCE NR CT OFF ]] BY ECODEC  \n");

     msm_snddev_nr_out_sel_qtrReceiver_or_A2020Speaker();
     mdelay(1);
     Audience_Config_GPIO_onoff(1); // Aux Pcm(about PCM Block <--> BT)	 
     aud_a2020_sleep_sequence();
     msm_snddev_nr_receiver_amp_off();
     audience_pmic_vreg_switch_off();	  //20110301 jhsong : rf 2.2VReg Control
}
///////////////////////////////// <<<<<


static int audience_a2020_probe(struct i2c_client *client, const struct i2c_device_id *id)
{
 	int rc = 0;
	
    gpio_tlmm_config(GPIO_CFG(GPIO_AUDIENCE_PLL, 0, GPIO_CFG_OUTPUT, GPIO_CFG_PULL_DOWN, GPIO_CFG_16MA), GPIO_CFG_ENABLE);
    gpio_set_value(GPIO_AUDIENCE_PLL,0);

    audience_pmic_vreg_switch_on(); //1.8V(VDD_IO, VDD_LDO), 2.7V(VDD_CODEC)

	if (!i2c_check_functionality(client->adapter, I2C_FUNC_I2C)) {
		audience_a2020_i2c_client = NULL;
		rc = -1;
	}
	else {
		audience_a2020_i2c_client = client;
		
		aud_a2020_boot_sequence(); 
	}

	audience_pmic_vreg_switch_off();
		
	printk("[Snd_audience_a2020] audience_a2020_probe %d \n",rc);
	return rc;
}

static int __exit audience_a2020_remove(struct i2c_client *client)
{
	int rc = 0;
	audience_a2020_i2c_client = NULL;
	return rc;
}

/*static*/ int audience_a2020_suspend(struct i2c_client *client, pm_message_t mesg)
{
//	printk("[Snd_audience_a2020] audience_a2020_suspend  \n");
	return 0;
}

static int audience_a2020_resume(struct i2c_client *client)
{
//	printk("[Snd_audience_a2020] audience_a2020_resume  \n");

	return 0;
}

static void audience_a2020_shutdown(struct i2c_client *client)
{
//	printk("[Snd_audience_a2020] audience_a2020_shutdown \n");

}

static const struct i2c_device_id audience_a2020_id[] = {
	{ "audience-a2020", 0},
};

static struct i2c_driver audience_a2020_driver = {
	.id_table 	= audience_a2020_id,
	.probe  	= audience_a2020_probe,
	.remove 	= __exit_p(audience_a2020_remove),
	.suspend 	= audience_a2020_suspend,
	.resume 	= audience_a2020_resume,
	.shutdown 	= audience_a2020_shutdown,
	.driver 	= {
	.name 		= "audience-a2020",
	},
};

void snd_audience_a2020_api_Init(void)
{
	int result = 0;

	printk( "[Snd_audience_a2020] snd_audience_a2020_api_Init()\n");
	result = i2c_add_driver(&audience_a2020_driver);
	if(result){
	printk(KERN_ERR "[Snd_audience_a2020] snd_audience_a2020_api_Init  Fail\n");
	}
}

void snd_audience_a2020_api_DeInit(void)
{
	//misc_deregister(&miscdev);
	i2c_del_driver(&audience_a2020_driver);
}




