

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

#include "snd_audience_a1028_mini.h"
#include "snd_sub_fab2200_mini.h"
#include "sky_snd_extampctrl_mini.h"

#if 1 //20110620 jhsong
int log_enable = 0;
#endif

#if 0
#include <linux/jiffies.h>
static void aud_a1028_mdelay(int ms)
{
    u64 endtime = get_jiffies_64();
    endtime = endtime + (ms*HZ)/1000;
#if 0
    printk( "[Snd_audience_a1028][-=AUDIO=-] Delay time FROM ====>> MS : [%d] !!!\n",ms);
    while(jiffies < endtime)
    schedule();	
    printk( "[Snd_audience_a1028][-=AUDIO=-] Delay time TO     ====>> MS : [%d] !!!\n",ms);
#else
    //printk( "[Snd_audience_a1028][-=AUDIO=-] Delay time FROM ====>> MS : [%d] !!!\n",ms);
    while(jiffies < endtime);
    //printk( "[Snd_audience_a1028][-=AUDIO=-] Delay time TO     ====>> MS : [%d] !!!\n",ms);
#endif
}
#endif

static struct i2c_client *audience_a1028_i2c_client = NULL;

#define POLLING_LOOP_COUNT 	3

#define GPIO_NR_WAKEUP_N 	147  // jmlee
#define GPIO_NR_RESET_N 	64  // jmlee

#if 0
#define GPIO_NR_AMP_EN 84 // jmlee pull down
#define GPIO_NR_OUT_SEL 85 // jmlee pull up
#endif

#ifndef byte // jmlee
#define byte u8
#endif
#if 0 //ndef boolean //jmlee
#define int s32
#endif

// A1028_CMD_BOOTLOAD_INITIATE is needed when reinitialize the system without resetting.
/* === not used === */
//SW Reset
#define A1028_CMD_SW_RESET 0x8002
#define A1028_CMD_SW_RESET_IMMEDIATE 0x0000
#define A1028_CMD_SW_RESET_DELAYED 0x0001
/* === not used === */
//Bootload Initiate
#define A1028_CMD_BOOTLOAD_INITIATE 0x8003
#define A1028_CMD_BOOTLOAD_INITIATE_ACK 0x80030000

#define A1028_CMD_BOOT 0x0001
#define A1028_CMD_BOOT_ACK 0x0101
#define A1028_CMD_BOOT_ACK_A1028 0x01

//Sync Control
#define A1028_CMD_SYNC 0x8000
#define A1028_CMD_SYNC_ACK 0x80000000L
#define A1028_CMD_SYNC_ONLY_ACK 0x80000005L
#define A1028_CMD_SYNC_MODE_POLLING 0x0000
#define A1028_CMD_SYNC_MODE_ACTIVE_LOW 0x0001
#define A1028_CMD_SYNC_MODE_ACTIVE_HIGH 0x0002
#define A1028_CMD_SYNC_MODE_FALLING_EDGE 0x0003
#define A1028_CMD_SYNC_MODE_RISING_EDGE 0x0004

//Set Power State
#define A1028_CMD_SLEEP 0x8010
#define A1028_CMD_SLEEP_VALUE 0x0001
#define A1028_CMD_SLEEP_ACK 0x80100001L

//Get/Set Device Parameter
#define A1028_CMD_GET_DEVICE_PARAMID 0x800B
#define A1028_CMD_DEVICE_PARAMID 0x800C
#define A1028_CMD_DEVICE_PARAM 0x800D

//Get/Set Algorithm Parameter
#define A1028_CMD_GET_ALGO_PARAMID 0x8016
#define A1028_CMD_ALGO_PARAMID 0x8017
#define A1028_CMD_ALGO_PARAM 0x8018

//Get/Set Digital Input Gain
#define A1028_CMD_SET_DIGI_INPUT_GAIN 0x801B
#define A1028_CMD_GET_DIGI_INPUT_GAIN 0x801A

//Get/Set Digital Output Gain
#define A1028_CMD_SET_DIGI_OUTPUT_GAIN 0x8015
#define A1028_CMD_GET_DIGI_OUTPUT_GAIN 0x801D

//Select Audio Route ID
#define A1028_CMD_SET_AUDIO_ROUTING 0x8026
#define A1028_CMD_GET_AUDIO_ROUTING 0x8027

//Enable/Disable Voice Processing
#define A1028_CMD_VOICE_PROCESSING 0x801C  // A100_msg_Bypass

#define ABS(a) (((a)>=(0))?(a):-(a))

//static const byte a1028_firmware_image[] =
static const u8 a1028_firmware_image[] =
//byte a1028_firmware_image[] =
{
//0x00,0x00  // jmlee compile temp
//#include "M11.5.1.5_B4668_Pantech_Mini_test.txt" // jmlee 20110131
//#include "M11.5.1.5_B4841_Pantech_Mini_test.txt" // jmlee 20110131
//#include "M11.5.1.5_B4932_Pantech_Mini_test.txt" // jhsong 20110226
//#include "M11.5.1.5_B5064_Pantech_Mini_test.txt" // jhsong 20110315
#include "Audience_bin_mini.txt"
};

int bBootSequence = 0;
int bVPonoff = 0;
a1028_talk_mode curr_talk_mode = MAX_TALK;
a1028_talk_mode prev_talk_mode = MAX_TALK;
a1028_mic_mode curr_mic_mode = BOTH_MIC_ON;
	
static a1028_status_type a1028_mode = A1028_MAX_MODE;
//static a1028_gpio_status_type a1028_gpio_mode = A1028_GPIO_MAX_MODE;
static a1028_sleep_status sleep_status = A1028_SLEEP_NONE;

//static byte * a1028_write_buf = NULL;
//static i2c_rw_cmd_type a1028_i2c_read_write_value;

typedef struct 
{
    u16 cmd;
    u16 value;
} a1028_reset_param;

//Software Reset
static a1028_reset_param sw_reset_param[] = 
{
    { A1028_CMD_SW_RESET, A1028_CMD_SW_RESET_DELAYED }, //Software Delayed Reset
    { 0xFFFF, 0xFFFF },
};

typedef struct 
{
    u16 cmd1;
    u16 id;
    u16 cmd2;
    u16 value;
} a1028_device_param;

//Device Param
static a1028_device_param device_param[] = 
{
    //Set Device Parameter
    { A1028_CMD_DEVICE_PARAMID, 0x0300, A1028_CMD_DEVICE_PARAM, 0x0001 }, //ADC0 : 6dB
    { A1028_CMD_DEVICE_PARAMID, 0x0400, A1028_CMD_DEVICE_PARAM, 0x0001 }, //ADC1 : 6dB
    { 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF },
};

typedef struct 
{
    u16 cmd1;
    u16 id;
    u16 cmd2;
    u16 value;
} a1028_algo_param;

//DV
#if 1  // jmlee 1028 CT init data
static a1028_algo_param algo_dv_param[] = 
{
    //Set Algorithm Parameter    
#if 1 //20110603 jhsong : audience cal
    { A1028_CMD_ALGO_PARAMID, 0x0002, A1028_CMD_ALGO_PARAM, 0x0002 }, // 0x8017:SetAlgorithmParmID, 0x0002:Microphone Configuration, 0x8018:SetAlgorithmParm, 0x0002:1-mic Desktop/Vehicle (DV)
    { A1028_CMD_ALGO_PARAMID, 0x0000, A1028_CMD_ALGO_PARAM, 0x0004 },
    { A1028_CMD_ALGO_PARAMID, 0x0003, A1028_CMD_ALGO_PARAM, 0x0001 },
    { A1028_CMD_ALGO_PARAMID, 0x0012, A1028_CMD_ALGO_PARAM, 0x000a/*0xfff6*//*0x0000*/ }, // 0x8017:SetAlgorithmParmID, 0x0012:   Downlink Speaker Volume, 0x8018:SetAlgorithmParm, 0xfff6:(-10 dB)
    { A1028_CMD_ALGO_PARAMID, 0x0023, A1028_CMD_ALGO_PARAM, 0x0000/*0x0001*/ }, // 0x8017:SetAlgorithmParmID, 0x0023:   Use AEC Comfort Noise Fill, 0x8018:SetAlgorithmParm, 0x0000:No
    	
    { A1028_CMD_ALGO_PARAMID, 0x0034, A1028_CMD_ALGO_PARAM, 0x000a/*0x0005*//*0x0000*/ },//  Echo Suppression Enhancement, 0x8018:SetAlgorithmParm,  0x0005 : (5dB)
    { A1028_CMD_ALGO_PARAMID, 0x0009, A1028_CMD_ALGO_PARAM, 0x0001 }, // 0x8017:SetAlgorithmParmID, 0x0009:Voice Equalization Mode, 0x8018:SetAlgorithmParm, 0x0001:SE On (auto select SE mode)
    { A1028_CMD_ALGO_PARAMID, 0x0025, A1028_CMD_ALGO_PARAM, 0xfff6/*0x0000*/ },
    { A1028_CMD_ALGO_PARAMID, 0x003D, A1028_CMD_ALGO_PARAM, 0x000B }, // jmlee add one line  0x8017:SetAlgorithmParmID, 0x003D:   VEQ Max Gain (dB), 0x8018:SetAlgorithmParm, 0x000B:(11 dB)
    { A1028_CMD_ALGO_PARAMID, 0x000E, A1028_CMD_ALGO_PARAM, 0x0001 },
    { A1028_CMD_ALGO_PARAMID, 0x000F, A1028_CMD_ALGO_PARAM, 0x000F }, // SetAlgorithmParmID, 0x000F:   FE NS Amount (dB), 0x8018:SetAlgorithmParm, 0x000F:(15 dB)

    { A1028_CMD_ALGO_PARAMID, 0x0004, A1028_CMD_ALGO_PARAM, 0x0000 },
    { A1028_CMD_ALGO_PARAMID, 0x0028, A1028_CMD_ALGO_PARAM, 0x0000 },

    { A1028_CMD_ALGO_PARAMID, 0x0040, A1028_CMD_ALGO_PARAM, 0xFFEC },  //20110622 jhsong case1

    { A1028_CMD_ALGO_PARAMID, 0x0020, A1028_CMD_ALGO_PARAM, 0x0000 },
    { A1028_CMD_ALGO_PARAMID, 0x001F, A1028_CMD_ALGO_PARAM, 0x0000/*0x0002*//*0x0000*/ }, // 0x8017:SetAlgorithmParmID, 0x001F:Rx PostEq Mode, 0x8018:SetAlgorithmParm, 0x0000:off  -> 0x0002 (en)
    { A1028_CMD_ALGO_PARAMID, 0x001A, A1028_CMD_ALGO_PARAM, 0x0000 }, // 0x8017:SetAlgorithmParmID, 0x001A:Use ComfortNoise, 0x8018:SetAlgorithmParm, 0x0000:No

    { A1028_CMD_ALGO_PARAMID, 0x001B, A1028_CMD_ALGO_PARAM, 0xFFAC }, // 0x8017:SetAlgorithmParmID, 0x001B:   ComfortNoise gain (dB), 0x8018:SetAlgorithmParm, 0xFFAC:(-84 dB)

    { A1028_CMD_ALGO_PARAMID, 0x0015, A1028_CMD_ALGO_PARAM, 0x0000 }, // Side Tone Mode, 0x8018:SetAlgorithmParm, 0x0000:Off
    { A1028_CMD_ALGO_PARAMID, 0x0016, A1028_CMD_ALGO_PARAM, 0xFFF7 },// Side Tone Gain (dB), 0x8018:SetAlgorithmParm, 0xFFF7:(-9 dB)

    { 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF },

#else
    { A1028_CMD_ALGO_PARAMID, 0x0002, A1028_CMD_ALGO_PARAM, 0x0002 }, // 0x8017:SetAlgorithmParmID, 0x0002:Microphone Configuration, 0x8018:SetAlgorithmParm, 0x0002:1-mic Desktop/Vehicle (DV)
    { A1028_CMD_ALGO_PARAMID, 0x0000, A1028_CMD_ALGO_PARAM, 0x0004 },
    { A1028_CMD_ALGO_PARAMID, 0x0003, A1028_CMD_ALGO_PARAM, 0x0001 },
    { A1028_CMD_ALGO_PARAMID, 0x0012, A1028_CMD_ALGO_PARAM, 0xfff6/*0x0000*/ }, // 0x8017:SetAlgorithmParmID, 0x0012:   Downlink Speaker Volume, 0x8018:SetAlgorithmParm, 0xfff6:(-10 dB)
    { A1028_CMD_ALGO_PARAMID, 0x0023, A1028_CMD_ALGO_PARAM, 0x0000/*0x0001*/ }, // 0x8017:SetAlgorithmParmID, 0x0023:   Use AEC Comfort Noise Fill, 0x8018:SetAlgorithmParm, 0x0000:No
    	
    { A1028_CMD_ALGO_PARAMID, 0x0034, A1028_CMD_ALGO_PARAM, 0x0005/*0x0000*/ },//  Echo Suppression Enhancement, 0x8018:SetAlgorithmParm,  0x0005 : (5dB)
    { A1028_CMD_ALGO_PARAMID, 0x0009, A1028_CMD_ALGO_PARAM, 0x0001 }, // 0x8017:SetAlgorithmParmID, 0x0009:Voice Equalization Mode, 0x8018:SetAlgorithmParm, 0x0001:SE On (auto select SE mode)
    { A1028_CMD_ALGO_PARAMID, 0x0025, A1028_CMD_ALGO_PARAM, 0x0000 },
    { A1028_CMD_ALGO_PARAMID, 0x003D, A1028_CMD_ALGO_PARAM, 0x000B }, // jmlee add one line  0x8017:SetAlgorithmParmID, 0x003D:   VEQ Max Gain (dB), 0x8018:SetAlgorithmParm, 0x000B:(11 dB)
    { A1028_CMD_ALGO_PARAMID, 0x000E, A1028_CMD_ALGO_PARAM, 0x0001 },
    { A1028_CMD_ALGO_PARAMID, 0x000F, A1028_CMD_ALGO_PARAM, 0x000F }, // SetAlgorithmParmID, 0x000F:   FE NS Amount (dB), 0x8018:SetAlgorithmParm, 0x000F:(15 dB)

    { A1028_CMD_ALGO_PARAMID, 0x0004, A1028_CMD_ALGO_PARAM, 0x0000 },
    { A1028_CMD_ALGO_PARAMID, 0x0028, A1028_CMD_ALGO_PARAM, 0x0000 },
    { A1028_CMD_ALGO_PARAMID, 0x0020, A1028_CMD_ALGO_PARAM, 0x0000 },
    { A1028_CMD_ALGO_PARAMID, 0x001F, A1028_CMD_ALGO_PARAM, 0x0000 }, // 0x8017:SetAlgorithmParmID, 0x001F:Rx PostEq Mode, 0x8018:SetAlgorithmParm, 0x0000:off
    { A1028_CMD_ALGO_PARAMID, 0x001A, A1028_CMD_ALGO_PARAM, 0x0000 }, // 0x8017:SetAlgorithmParmID, 0x001A:Use ComfortNoise, 0x8018:SetAlgorithmParm, 0x0000:No

    { A1028_CMD_ALGO_PARAMID, 0x001B, A1028_CMD_ALGO_PARAM, 0xFFAC }, // 0x8017:SetAlgorithmParmID, 0x001B:   ComfortNoise gain (dB), 0x8018:SetAlgorithmParm, 0xFFAC:(-84 dB)
    { 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF },
#endif    
};

//DV VP Off
static a1028_algo_param algo_dv_VPOFF_param[] =  
{
    //Set Algorithm Parameter    
    { A1028_CMD_ALGO_PARAMID, 0x0002, A1028_CMD_ALGO_PARAM, 0x0002 },
    { A1028_CMD_ALGO_PARAMID, 0x0000, A1028_CMD_ALGO_PARAM, 0x0001 },
    { A1028_CMD_ALGO_PARAMID, 0x0003, A1028_CMD_ALGO_PARAM, 0x0001 },
    { A1028_CMD_ALGO_PARAMID, 0x0012, A1028_CMD_ALGO_PARAM, 0x0003 },
    { A1028_CMD_ALGO_PARAMID, 0x0023, A1028_CMD_ALGO_PARAM, 0x0001 },
    	
    { A1028_CMD_ALGO_PARAMID, 0x0034, A1028_CMD_ALGO_PARAM, 0x0000 },
    { A1028_CMD_ALGO_PARAMID, 0x0009, A1028_CMD_ALGO_PARAM, 0x0000 },
    { A1028_CMD_ALGO_PARAMID, 0x0025, A1028_CMD_ALGO_PARAM, 0x0000 },
    { A1028_CMD_ALGO_PARAMID, 0x000E, A1028_CMD_ALGO_PARAM, 0x0001 },
    { A1028_CMD_ALGO_PARAMID, 0x000F, A1028_CMD_ALGO_PARAM, 0x000A },

    { A1028_CMD_ALGO_PARAMID, 0x0004, A1028_CMD_ALGO_PARAM, 0x0000 },
    { A1028_CMD_ALGO_PARAMID, 0x0028, A1028_CMD_ALGO_PARAM, 0x0000 },
    { A1028_CMD_ALGO_PARAMID, 0x0020, A1028_CMD_ALGO_PARAM, 0x0000 },
    { A1028_CMD_ALGO_PARAMID, 0x001F, A1028_CMD_ALGO_PARAM, 0x0002 },
    { A1028_CMD_ALGO_PARAMID, 0x001A, A1028_CMD_ALGO_PARAM, 0x0001 },

    { A1028_CMD_ALGO_PARAMID, 0x001B, A1028_CMD_ALGO_PARAM, 0xFFBA },
    { 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF },
};
#endif

//CT
#if 1  // jmlee 1028 CT init data
static a1028_algo_param algo_ct_param[] = 
{
    //Set Algorithm Parameter    
#if 1 //20110408 jhsong : audience cal
    { A1028_CMD_ALGO_PARAMID, 0x0002, A1028_CMD_ALGO_PARAM, 0x0000 }, //MIC Configuration
    { A1028_CMD_ALGO_PARAMID, 0x0000, A1028_CMD_ALGO_PARAM, 0x0004 }, //AIS Suppression Level AIS 4
    { A1028_CMD_ALGO_PARAMID, 0x0003, A1028_CMD_ALGO_PARAM, 0x0001 },
    { A1028_CMD_ALGO_PARAMID, 0x0012, A1028_CMD_ALGO_PARAM, /*0xfffc*//*0x0000*/0xffec }, // Downlink Speaker Volume 0dB  //20110608 jhsong : 0dB -> -20dB
    { A1028_CMD_ALGO_PARAMID, 0x0023, A1028_CMD_ALGO_PARAM, /*0x0001*/0x0000 }, // Comfort Noise Fill 0x0000 : No

    { A1028_CMD_ALGO_PARAMID, 0x0034, A1028_CMD_ALGO_PARAM, /*0x0000*/0x0007 }, // Echo Suppression Enhancement 0x0007:(7 dB)
    { A1028_CMD_ALGO_PARAMID, 0x0009, A1028_CMD_ALGO_PARAM, 0x0001 },
    { A1028_CMD_ALGO_PARAMID, 0x0025, A1028_CMD_ALGO_PARAM, 0x0000 },
    { A1028_CMD_ALGO_PARAMID, 0x003D, A1028_CMD_ALGO_PARAM, 0x000B }, // VEQ Max Gain (dB) 0x000B:(11 dB)
    { A1028_CMD_ALGO_PARAMID, 0x000E, A1028_CMD_ALGO_PARAM, 0x0001 },

    { A1028_CMD_ALGO_PARAMID, 0x000F, A1028_CMD_ALGO_PARAM, 0x000F },
    { A1028_CMD_ALGO_PARAMID, 0x0004, A1028_CMD_ALGO_PARAM, 0x0000 },
    { A1028_CMD_ALGO_PARAMID, 0x0028, A1028_CMD_ALGO_PARAM, 0x0000 },

    { A1028_CMD_ALGO_PARAMID, 0x0040, A1028_CMD_ALGO_PARAM, 0xFFF7 }, //20110622 jhsong case1

    { A1028_CMD_ALGO_PARAMID, 0x0020, A1028_CMD_ALGO_PARAM, 0x0002 },
    { A1028_CMD_ALGO_PARAMID, 0x001F, A1028_CMD_ALGO_PARAM, 0x0002 },

    { A1028_CMD_ALGO_PARAMID, 0x001A, A1028_CMD_ALGO_PARAM, 0x0000 }, // Use ComfortNoise, 0x0000:No
    { A1028_CMD_ALGO_PARAMID, 0x001B, A1028_CMD_ALGO_PARAM, 0xFFAC },

    { A1028_CMD_ALGO_PARAMID, 0x0015, A1028_CMD_ALGO_PARAM, 0x0000 }, // Side Tone Mode, 0x8018:SetAlgorithmParm, 0x0000:Off
    { A1028_CMD_ALGO_PARAMID, 0x0016, A1028_CMD_ALGO_PARAM, 0xFFF7 },// Side Tone Gain (dB), 0x8018:SetAlgorithmParm, 0xFFF7:(-9 dB)
    { 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF },

#else
    { A1028_CMD_ALGO_PARAMID, 0x0002, A1028_CMD_ALGO_PARAM, 0x0000 }, //MIC Configuration
    { A1028_CMD_ALGO_PARAMID, 0x0000, A1028_CMD_ALGO_PARAM, 0x0004 }, //AIS Suppression Level AIS 4
    { A1028_CMD_ALGO_PARAMID, 0x0003, A1028_CMD_ALGO_PARAM, 0x0001 },
    { A1028_CMD_ALGO_PARAMID, 0x0012, A1028_CMD_ALGO_PARAM, 0xfffc/*0x0000*/ }, // Downlink Speaker Volume -20dB
    { A1028_CMD_ALGO_PARAMID, 0x0023, A1028_CMD_ALGO_PARAM, 0x0001 }, // Comfort Noise Fill 0x0000 : No

    { A1028_CMD_ALGO_PARAMID, 0x0034, A1028_CMD_ALGO_PARAM, 0x0000 }, // Echo Suppression Enhancement 0x0000:(0 dB)
    { A1028_CMD_ALGO_PARAMID, 0x0009, A1028_CMD_ALGO_PARAM, 0x0001 },
    { A1028_CMD_ALGO_PARAMID, 0x0025, A1028_CMD_ALGO_PARAM, 0x0000 },
    { A1028_CMD_ALGO_PARAMID, 0x003D, A1028_CMD_ALGO_PARAM, 0x000B }, // VEQ Max Gain (dB) 0x000B:(11 dB)
    { A1028_CMD_ALGO_PARAMID, 0x000E, A1028_CMD_ALGO_PARAM, 0x0001 },

    { A1028_CMD_ALGO_PARAMID, 0x000F, A1028_CMD_ALGO_PARAM, 0x000F },
    { A1028_CMD_ALGO_PARAMID, 0x0004, A1028_CMD_ALGO_PARAM, 0x0000 },
    { A1028_CMD_ALGO_PARAMID, 0x0028, A1028_CMD_ALGO_PARAM, 0x0000 },
    { A1028_CMD_ALGO_PARAMID, 0x0020, A1028_CMD_ALGO_PARAM, 0x0002 },
    { A1028_CMD_ALGO_PARAMID, 0x001F, A1028_CMD_ALGO_PARAM, 0x0002 },

    { A1028_CMD_ALGO_PARAMID, 0x001A, A1028_CMD_ALGO_PARAM, 0x0001 }, // Use ComfortNoise, 0x0000:No
    { A1028_CMD_ALGO_PARAMID, 0x001B, A1028_CMD_ALGO_PARAM, 0xFFAC },
    { 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF },
#endif    
};
#endif

//CT 1MIC
static a1028_algo_param algo_ct_1MIC_param[] =  
{
    //Set Algorithm Parameter    
    { A1028_CMD_ALGO_PARAMID, 0x0002, A1028_CMD_ALGO_PARAM, 0x0003 }, //MIC Configuration
    { A1028_CMD_ALGO_PARAMID, 0x0000, A1028_CMD_ALGO_PARAM, 0x0005 }, //AIS Suppression Level
    { A1028_CMD_ALGO_PARAMID, 0x0003, A1028_CMD_ALGO_PARAM, 0x0001 },
    { A1028_CMD_ALGO_PARAMID, 0x0012, A1028_CMD_ALGO_PARAM, 0xFFD3 },
    { A1028_CMD_ALGO_PARAMID, 0x0023, A1028_CMD_ALGO_PARAM, 0x0001 },

    { A1028_CMD_ALGO_PARAMID, 0x0034, A1028_CMD_ALGO_PARAM, 0xFFFB },
    { A1028_CMD_ALGO_PARAMID, 0x0009, A1028_CMD_ALGO_PARAM, 0x0001 },
    { A1028_CMD_ALGO_PARAMID, 0x0025, A1028_CMD_ALGO_PARAM, 0x0000 },
    { A1028_CMD_ALGO_PARAMID, 0x003D, A1028_CMD_ALGO_PARAM, 0x000B },
    { A1028_CMD_ALGO_PARAMID, 0x000E, A1028_CMD_ALGO_PARAM, 0x0001 },

    { A1028_CMD_ALGO_PARAMID, 0x000F, A1028_CMD_ALGO_PARAM, 0x000F },
    { A1028_CMD_ALGO_PARAMID, 0x0004, A1028_CMD_ALGO_PARAM, 0x0000 },
    { A1028_CMD_ALGO_PARAMID, 0x0028, A1028_CMD_ALGO_PARAM, 0x0000 },
    { A1028_CMD_ALGO_PARAMID, 0x0020, A1028_CMD_ALGO_PARAM, 0x0002 },
    { A1028_CMD_ALGO_PARAMID, 0x001F, A1028_CMD_ALGO_PARAM, 0x0002 },

    { A1028_CMD_ALGO_PARAMID, 0x001A, A1028_CMD_ALGO_PARAM, 0x0001 },
    { A1028_CMD_ALGO_PARAMID, 0x001B, A1028_CMD_ALGO_PARAM, 0xFFAC },
    { 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF },
};

//CT VP Off
static a1028_algo_param algo_ct_VPOFF_param[] =  
{
    //Set Algorithm Parameter    
    { A1028_CMD_ALGO_PARAMID, 0x0002, A1028_CMD_ALGO_PARAM, 0x0003 }, //MIC Configuration
    { A1028_CMD_ALGO_PARAMID, 0x0000, A1028_CMD_ALGO_PARAM, 0x0001 }, //AIS Suppression Level
    { A1028_CMD_ALGO_PARAMID, 0x0003, A1028_CMD_ALGO_PARAM, 0x0001 },
    { A1028_CMD_ALGO_PARAMID, 0x0012, A1028_CMD_ALGO_PARAM, 0xFFD3 },
    { A1028_CMD_ALGO_PARAMID, 0x0023, A1028_CMD_ALGO_PARAM, 0x0001 },

    { A1028_CMD_ALGO_PARAMID, 0x0034, A1028_CMD_ALGO_PARAM, 0xFFFB },
    { A1028_CMD_ALGO_PARAMID, 0x0009, A1028_CMD_ALGO_PARAM, 0x0001 },
    { A1028_CMD_ALGO_PARAMID, 0x0025, A1028_CMD_ALGO_PARAM, 0x0000 },
    { A1028_CMD_ALGO_PARAMID, 0x003D, A1028_CMD_ALGO_PARAM, 0x000B },
    { A1028_CMD_ALGO_PARAMID, 0x000E, A1028_CMD_ALGO_PARAM, 0x0001 },

    { A1028_CMD_ALGO_PARAMID, 0x000F, A1028_CMD_ALGO_PARAM, 0x000F },
    { A1028_CMD_ALGO_PARAMID, 0x0004, A1028_CMD_ALGO_PARAM, 0x0000 },
    { A1028_CMD_ALGO_PARAMID, 0x0028, A1028_CMD_ALGO_PARAM, 0x0000 },
    { A1028_CMD_ALGO_PARAMID, 0x0020, A1028_CMD_ALGO_PARAM, 0x0002 },
    { A1028_CMD_ALGO_PARAMID, 0x001F, A1028_CMD_ALGO_PARAM, 0x0002 },

    { A1028_CMD_ALGO_PARAMID, 0x001A, A1028_CMD_ALGO_PARAM, 0x0001 },
    { A1028_CMD_ALGO_PARAMID, 0x001B, A1028_CMD_ALGO_PARAM, 0xFFAC },
    { 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF },
};

//CT 1MIC VP Off close talk
static a1028_algo_param algo_ct_1MIC_VPOFF_param[] =  
{
    //Set Algorithm Parameter    
    { A1028_CMD_ALGO_PARAMID, 0x0002, A1028_CMD_ALGO_PARAM, 0x0003 }, //MIC Configuration
    { A1028_CMD_ALGO_PARAMID, 0x0000, A1028_CMD_ALGO_PARAM, 0x0001 }, //AIS Suppression Level
    { A1028_CMD_ALGO_PARAMID, 0x0003, A1028_CMD_ALGO_PARAM, 0x0001 },
    { A1028_CMD_ALGO_PARAMID, 0x0012, A1028_CMD_ALGO_PARAM, 0xFFD3 },
    { A1028_CMD_ALGO_PARAMID, 0x0023, A1028_CMD_ALGO_PARAM, 0x0001 },

    { A1028_CMD_ALGO_PARAMID, 0x0034, A1028_CMD_ALGO_PARAM, 0xFFFB },
    { A1028_CMD_ALGO_PARAMID, 0x0009, A1028_CMD_ALGO_PARAM, 0x0001 },
    { A1028_CMD_ALGO_PARAMID, 0x0025, A1028_CMD_ALGO_PARAM, 0x0000 },
    { A1028_CMD_ALGO_PARAMID, 0x003D, A1028_CMD_ALGO_PARAM, 0x000B },
    { A1028_CMD_ALGO_PARAMID, 0x000E, A1028_CMD_ALGO_PARAM, 0x0001 },

    { A1028_CMD_ALGO_PARAMID, 0x000F, A1028_CMD_ALGO_PARAM, 0x000F },
    { A1028_CMD_ALGO_PARAMID, 0x0004, A1028_CMD_ALGO_PARAM, 0x0000 },
    { A1028_CMD_ALGO_PARAMID, 0x0028, A1028_CMD_ALGO_PARAM, 0x0000 },
    { A1028_CMD_ALGO_PARAMID, 0x0020, A1028_CMD_ALGO_PARAM, 0x0002 },
    { A1028_CMD_ALGO_PARAMID, 0x001F, A1028_CMD_ALGO_PARAM, 0x0002 },

    { A1028_CMD_ALGO_PARAMID, 0x001A, A1028_CMD_ALGO_PARAM, 0x0001 },
    { A1028_CMD_ALGO_PARAMID, 0x001B, A1028_CMD_ALGO_PARAM, 0xFFAC },
    { 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF },
};

//FT
#if 1  // jmlee 1028 CT init data
static a1028_algo_param algo_ft_param[] = 
{
    //Set Algorithm Parameter    
#if 1  //20110603 jhsong : audience cal
    { A1028_CMD_ALGO_PARAMID, 0x0002, A1028_CMD_ALGO_PARAM, 0x0001 }, // 0x8017:SetAlgorithmParmID, 0x0002:Microphone Configuration, 0x8018:SetAlgorithmParm, 0x0001:2-mic Far Talk (FT)
    { A1028_CMD_ALGO_PARAMID, 0x0000, A1028_CMD_ALGO_PARAM, 0x0004 },
    { A1028_CMD_ALGO_PARAMID, 0x0003, A1028_CMD_ALGO_PARAM, 0x0001 },
    { A1028_CMD_ALGO_PARAMID, 0x0012, A1028_CMD_ALGO_PARAM, 0x000a/*0xfff6*//*0x0000*/ }, // 0x8017:SetAlgorithmParmID, 0x0012:   Downlink Speaker Volume, 0x8018:SetAlgorithmParm, 0xfff6:(-10 dB)
    { A1028_CMD_ALGO_PARAMID, 0x0023, A1028_CMD_ALGO_PARAM, 0x0000 }, // 0x8017:SetAlgorithmParmID, 0x0023:   Use AEC Comfort Noise Fill, 0x8018:SetAlgorithmParm, 0x0000:No
    	
    { A1028_CMD_ALGO_PARAMID, 0x0034, A1028_CMD_ALGO_PARAM, 0x000a/*0x0005*//*0x0000*/ },// Echo Suppression Enhancement, 0x8018:SetAlgorithmParm, 0x0005:(5 dB)
    { A1028_CMD_ALGO_PARAMID, 0x0009, A1028_CMD_ALGO_PARAM, 0x0001 }, // 0x8017:SetAlgorithmParmID, 0x0009:Voice Equalization Mode, 0x8018:SetAlgorithmParm, 0x0001:SE On (auto select SE mode)
    { A1028_CMD_ALGO_PARAMID, 0x0025, A1028_CMD_ALGO_PARAM, 0xfff6/*0x0000 */},
    { A1028_CMD_ALGO_PARAMID, 0x003D, A1028_CMD_ALGO_PARAM, 0x000B }, // jmlee add one line  0x8017:SetAlgorithmParmID, 0x003D:   VEQ Max Gain (dB), 0x8018:SetAlgorithmParm, 0x000B:(11 dB)
    { A1028_CMD_ALGO_PARAMID, 0x000E, A1028_CMD_ALGO_PARAM, 0x0001 },
    { A1028_CMD_ALGO_PARAMID, 0x000F, A1028_CMD_ALGO_PARAM, 0x000F }, // SetAlgorithmParmID, 0x000F:   FE NS Amount (dB), 0x8018:SetAlgorithmParm, 0x000F:(15 dB)

    { A1028_CMD_ALGO_PARAMID, 0x000D, A1028_CMD_ALGO_PARAM, 0xFFFE}, // 20110622 jhsong :  0x000D:Rx-out Limiter Max Level (dB), 0x8018:SetAlgorithmParm, 0xFFFE:(-2 dB)

    { A1028_CMD_ALGO_PARAMID, 0x0004, A1028_CMD_ALGO_PARAM, 0x0000 },
    { A1028_CMD_ALGO_PARAMID, 0x0028, A1028_CMD_ALGO_PARAM, 0x0000 },

    { A1028_CMD_ALGO_PARAMID, 0x0040, A1028_CMD_ALGO_PARAM, 0xFFF7 },  //20110622 jhsong case1

    { A1028_CMD_ALGO_PARAMID, 0x0020, A1028_CMD_ALGO_PARAM, 0x0000 },
    { A1028_CMD_ALGO_PARAMID, 0x001F, A1028_CMD_ALGO_PARAM, 0x0002/*0x0000 */}, // 0x8017:SetAlgorithmParmID, 0x001F:Rx PostEq Mode, 0x8018:SetAlgorithmParm, 0x0000:off -> 0x0002 (en)
    { A1028_CMD_ALGO_PARAMID, 0x001A, A1028_CMD_ALGO_PARAM, 0x0000 }, // 0x8017:SetAlgorithmParmID, 0x001A:Use ComfortNoise, 0x8018:SetAlgorithmParm, 0x0000:No

    { A1028_CMD_ALGO_PARAMID, 0x001B, A1028_CMD_ALGO_PARAM, 0xFFAC }, // 0x8017:SetAlgorithmParmID, 0x001B:   ComfortNoise gain (dB), 0x8018:SetAlgorithmParm, 0xFFAC:(-84 dB)

    { A1028_CMD_ALGO_PARAMID, 0x0015, A1028_CMD_ALGO_PARAM, 0x0000 }, // Side Tone Mode, 0x8018:SetAlgorithmParm, 0x0000:Off
    { A1028_CMD_ALGO_PARAMID, 0x0016, A1028_CMD_ALGO_PARAM, 0xFFF7 },// Side Tone Gain (dB), 0x8018:SetAlgorithmParm, 0xFFF7:(-9 dB)
    { 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF },

#else
    { A1028_CMD_ALGO_PARAMID, 0x0002, A1028_CMD_ALGO_PARAM, 0x0001 }, // 0x8017:SetAlgorithmParmID, 0x0002:Microphone Configuration, 0x8018:SetAlgorithmParm, 0x0001:2-mic Far Talk (FT)
    { A1028_CMD_ALGO_PARAMID, 0x0000, A1028_CMD_ALGO_PARAM, 0x0004 },
    { A1028_CMD_ALGO_PARAMID, 0x0003, A1028_CMD_ALGO_PARAM, 0x0001 },
    { A1028_CMD_ALGO_PARAMID, 0x0012, A1028_CMD_ALGO_PARAM, 0xfff6/*0x0000*/ }, // 0x8017:SetAlgorithmParmID, 0x0012:   Downlink Speaker Volume, 0x8018:SetAlgorithmParm, 0xfff6:(-10 dB)
    { A1028_CMD_ALGO_PARAMID, 0x0023, A1028_CMD_ALGO_PARAM, 0x0000 }, // 0x8017:SetAlgorithmParmID, 0x0023:   Use AEC Comfort Noise Fill, 0x8018:SetAlgorithmParm, 0x0000:No
    	
    { A1028_CMD_ALGO_PARAMID, 0x0034, A1028_CMD_ALGO_PARAM, 0x0005/*0x0000*/ },// Echo Suppression Enhancement, 0x8018:SetAlgorithmParm, 0x0005:(5 dB)
    { A1028_CMD_ALGO_PARAMID, 0x0009, A1028_CMD_ALGO_PARAM, 0x0001 }, // 0x8017:SetAlgorithmParmID, 0x0009:Voice Equalization Mode, 0x8018:SetAlgorithmParm, 0x0001:SE On (auto select SE mode)
    { A1028_CMD_ALGO_PARAMID, 0x0025, A1028_CMD_ALGO_PARAM, 0x0000 },
    { A1028_CMD_ALGO_PARAMID, 0x003D, A1028_CMD_ALGO_PARAM, 0x000B }, // jmlee add one line  0x8017:SetAlgorithmParmID, 0x003D:   VEQ Max Gain (dB), 0x8018:SetAlgorithmParm, 0x000B:(11 dB)
    { A1028_CMD_ALGO_PARAMID, 0x000E, A1028_CMD_ALGO_PARAM, 0x0001 },
    { A1028_CMD_ALGO_PARAMID, 0x000F, A1028_CMD_ALGO_PARAM, 0x000F }, // SetAlgorithmParmID, 0x000F:   FE NS Amount (dB), 0x8018:SetAlgorithmParm, 0x000F:(15 dB)

    { A1028_CMD_ALGO_PARAMID, 0x0004, A1028_CMD_ALGO_PARAM, 0x0000 },
    { A1028_CMD_ALGO_PARAMID, 0x0028, A1028_CMD_ALGO_PARAM, 0x0000 },
    { A1028_CMD_ALGO_PARAMID, 0x0020, A1028_CMD_ALGO_PARAM, 0x0000 },
    { A1028_CMD_ALGO_PARAMID, 0x001F, A1028_CMD_ALGO_PARAM, 0x0000 }, // 0x8017:SetAlgorithmParmID, 0x001F:Rx PostEq Mode, 0x8018:SetAlgorithmParm, 0x0000:off
    { A1028_CMD_ALGO_PARAMID, 0x001A, A1028_CMD_ALGO_PARAM, 0x0000 }, // 0x8017:SetAlgorithmParmID, 0x001A:Use ComfortNoise, 0x8018:SetAlgorithmParm, 0x0000:No

    { A1028_CMD_ALGO_PARAMID, 0x001B, A1028_CMD_ALGO_PARAM, 0xFFAC }, // 0x8017:SetAlgorithmParmID, 0x001B:   ComfortNoise gain (dB), 0x8018:SetAlgorithmParm, 0xFFAC:(-84 dB)
    { 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF },
#endif    
};
#endif

//FT VP Off
static a1028_algo_param algo_ft_VPOFF_param[] = 
{
    //Set Algorithm Parameter    
    { A1028_CMD_ALGO_PARAMID, 0x0002, A1028_CMD_ALGO_PARAM, 0x0002 },
    { A1028_CMD_ALGO_PARAMID, 0x0000, A1028_CMD_ALGO_PARAM, 0x0001 },
    { A1028_CMD_ALGO_PARAMID, 0x0003, A1028_CMD_ALGO_PARAM, 0x0001 },
    { A1028_CMD_ALGO_PARAMID, 0x0012, A1028_CMD_ALGO_PARAM, 0x0003 },
    { A1028_CMD_ALGO_PARAMID, 0x0023, A1028_CMD_ALGO_PARAM, 0x0001 },
    	
    { A1028_CMD_ALGO_PARAMID, 0x0034, A1028_CMD_ALGO_PARAM, 0x0000 },
    { A1028_CMD_ALGO_PARAMID, 0x0009, A1028_CMD_ALGO_PARAM, 0x0000 },
    { A1028_CMD_ALGO_PARAMID, 0x0025, A1028_CMD_ALGO_PARAM, 0x0000 },
    { A1028_CMD_ALGO_PARAMID, 0x000E, A1028_CMD_ALGO_PARAM, 0x0001 },
    { A1028_CMD_ALGO_PARAMID, 0x000F, A1028_CMD_ALGO_PARAM, 0x000A },

    { A1028_CMD_ALGO_PARAMID, 0x0004, A1028_CMD_ALGO_PARAM, 0x0000 },
    { A1028_CMD_ALGO_PARAMID, 0x0028, A1028_CMD_ALGO_PARAM, 0x0000 },
    { A1028_CMD_ALGO_PARAMID, 0x0020, A1028_CMD_ALGO_PARAM, 0x0000 },
    { A1028_CMD_ALGO_PARAMID, 0x001F, A1028_CMD_ALGO_PARAM, 0x0002 },
    { A1028_CMD_ALGO_PARAMID, 0x001A, A1028_CMD_ALGO_PARAM, 0x0001 },

    { A1028_CMD_ALGO_PARAMID, 0x001B, A1028_CMD_ALGO_PARAM, 0xFFBA },
    { 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF },
};

//Voice LoopBack Primary/Secondary MIC VP Off
static a1028_algo_param algo_ct_vl_MIC_param[] =  
{
    //Set Algorithm Parameter    
    { A1028_CMD_ALGO_PARAMID, 0x0002, A1028_CMD_ALGO_PARAM, 0x0003 }, //MIC Configuration
    { A1028_CMD_ALGO_PARAMID, 0x0000, A1028_CMD_ALGO_PARAM, 0x0005 }, //AIS Suppression Level
    { A1028_CMD_ALGO_PARAMID, 0x0003, A1028_CMD_ALGO_PARAM, 0x0001 },
    { A1028_CMD_ALGO_PARAMID, 0x0012, A1028_CMD_ALGO_PARAM, 0xFFD3 },
    { A1028_CMD_ALGO_PARAMID, 0x0023, A1028_CMD_ALGO_PARAM, 0x0001 },

    { A1028_CMD_ALGO_PARAMID, 0x0034, A1028_CMD_ALGO_PARAM, 0xFFFB },
    { A1028_CMD_ALGO_PARAMID, 0x0009, A1028_CMD_ALGO_PARAM, 0x0001 },
    { A1028_CMD_ALGO_PARAMID, 0x0025, A1028_CMD_ALGO_PARAM, 0x0000 },
    { A1028_CMD_ALGO_PARAMID, 0x003D, A1028_CMD_ALGO_PARAM, 0x000B },
    { A1028_CMD_ALGO_PARAMID, 0x000E, A1028_CMD_ALGO_PARAM, 0x0001 },

    { A1028_CMD_ALGO_PARAMID, 0x000F, A1028_CMD_ALGO_PARAM, 0x000F },
    { A1028_CMD_ALGO_PARAMID, 0x0004, A1028_CMD_ALGO_PARAM, 0x0000 },
    { A1028_CMD_ALGO_PARAMID, 0x0028, A1028_CMD_ALGO_PARAM, 0x0000 },
    { A1028_CMD_ALGO_PARAMID, 0x0020, A1028_CMD_ALGO_PARAM, 0x0002 },
    { A1028_CMD_ALGO_PARAMID, 0x001F, A1028_CMD_ALGO_PARAM, 0x0002 },

    { A1028_CMD_ALGO_PARAMID, 0x001A, A1028_CMD_ALGO_PARAM, 0x0001 },
    { A1028_CMD_ALGO_PARAMID, 0x001B, A1028_CMD_ALGO_PARAM, 0xFFAC },
    { 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF },
};

//Call Primary/Secondary MIC Test VP On
static a1028_algo_param algo_ct_call_MIC_param[] =  
{
    //Set Algorithm Parameter
    { A1028_CMD_ALGO_PARAMID, 0x0002, A1028_CMD_ALGO_PARAM, 0x0003 }, //MIC Configuration
    { A1028_CMD_ALGO_PARAMID, 0x0000, A1028_CMD_ALGO_PARAM, 0x0005 }, //AIS Suppression Level
    { A1028_CMD_ALGO_PARAMID, 0x0003, A1028_CMD_ALGO_PARAM, 0x0001 },
    { A1028_CMD_ALGO_PARAMID, 0x0012, A1028_CMD_ALGO_PARAM, 0xFFD3 },
    { A1028_CMD_ALGO_PARAMID, 0x0023, A1028_CMD_ALGO_PARAM, 0x0001 },

    { A1028_CMD_ALGO_PARAMID, 0x0034, A1028_CMD_ALGO_PARAM, 0xFFFB },
    { A1028_CMD_ALGO_PARAMID, 0x0009, A1028_CMD_ALGO_PARAM, 0x0001 },
    { A1028_CMD_ALGO_PARAMID, 0x0025, A1028_CMD_ALGO_PARAM, 0x0000 },
    { A1028_CMD_ALGO_PARAMID, 0x003D, A1028_CMD_ALGO_PARAM, 0x000B },
    { A1028_CMD_ALGO_PARAMID, 0x000E, A1028_CMD_ALGO_PARAM, 0x0001 },

    { A1028_CMD_ALGO_PARAMID, 0x000F, A1028_CMD_ALGO_PARAM, 0x000F },
    { A1028_CMD_ALGO_PARAMID, 0x0004, A1028_CMD_ALGO_PARAM, 0x0000 },
    { A1028_CMD_ALGO_PARAMID, 0x0028, A1028_CMD_ALGO_PARAM, 0x0000 },
    { A1028_CMD_ALGO_PARAMID, 0x0020, A1028_CMD_ALGO_PARAM, 0x0002 },
    { A1028_CMD_ALGO_PARAMID, 0x001F, A1028_CMD_ALGO_PARAM, 0x0002 },

    { A1028_CMD_ALGO_PARAMID, 0x001A, A1028_CMD_ALGO_PARAM, 0x0001 },
    { A1028_CMD_ALGO_PARAMID, 0x001B, A1028_CMD_ALGO_PARAM, 0xFFAC },
    { 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF },
};

typedef struct 
{
    u16 cmd;
    u16 value;
} a1028_path_param;

//Voice Processing On
static a1028_path_param path_vp_on_param[] = 
{
    { A1028_CMD_VOICE_PROCESSING, 0x0001 }, //Voice Processing On
    { 0xFFFF, 0xFFFF },
};

//Voice Processing Off
static a1028_path_param path_vp_off_param[] = 
{
    { A1028_CMD_VOICE_PROCESSING, 0x0000 }, //Voice Processing Off
    { 0xFFFF, 0xFFFF },
};

//Audio Routing
static a1028_path_param path_routing_param[] = 
{
    { A1028_CMD_SET_AUDIO_ROUTING, 0x0000 },
    { 0xFFFF, 0xFFFF },
};


//Audio Routing Parameter 0x000A Setting
#if 1  // jmlee 1028 CT init data
static a1028_path_param path_ct_A_routing_param[] = 
{
    { A1028_CMD_SET_AUDIO_ROUTING, 0x0000 },  // 0x8026:SelectRouting, 0x0000:Pri,Sec,Fei,Lp1 - Feo,Csp,Lp1 (PCM0->ADCs)
    { 0xFFFF, 0xFFFF },
};
#else
static a1028_path_param path_ct_A_routing_param[] = 
{
    { A1028_CMD_SET_AUDIO_ROUTING, 0x000A },
    { 0xFFFF, 0xFFFF },
};
#endif

//Voice LoopBack Primary MIC Audio Routing
static a1028_path_param path_vl_pri_MIC_routing_param[] =
{
    { A1028_CMD_SET_AUDIO_ROUTING, 0x0007 },
    { 0xFFFF, 0xFFFF },
};

//Voice LoopBack Secondary MIC Audio Routing
static a1028_path_param path_vl_sec_MIC_routing_param[] =
{
    { A1028_CMD_SET_AUDIO_ROUTING, 0x0015 },
    { 0xFFFF, 0xFFFF },
};

//Call Primary MIC Audio Routing
static a1028_path_param path_call_pri_MIC_routing_param[] =
{
    { A1028_CMD_SET_AUDIO_ROUTING, 0x000A },
    { 0xFFFF, 0xFFFF },
};

//Call Secondary MIC Audio Routing
static a1028_path_param path_call_sec_MIC_routing_param[] =
{
    { A1028_CMD_SET_AUDIO_ROUTING, 0x0008 },
    { 0xFFFF, 0xFFFF },
};

//Call Secondary MIC Audio Routing
static a1028_path_param path_call_sec_DV_MIC_routing_param[] =
{
    { A1028_CMD_SET_AUDIO_ROUTING, 0x000A },  //0x8026:SelectRouting, 0x000A:Pri,Snk,Fei,Lp1 - Feo,Csp,Lp1 (PCM0->ADCs)
    { 0xFFFF, 0xFFFF },
};


//KeyTone Digital Gain
static a1028_path_param path_keytone_gain_param[] =
{
    { A1028_CMD_SET_DIGI_INPUT_GAIN, 0x0203 },
    { 0xFFFF, 0xFFFF },
};


//CT Digital Gain
#if 1  // jmlee 1028 CT init data
static a1028_path_param path_ct_gain_param[] = 
{
    { A1028_CMD_SET_DIGI_INPUT_GAIN, 0x000D }, //Primary MIC 13dB
    { A1028_CMD_SET_DIGI_INPUT_GAIN, 0x010D }, //Secondary MIC 13dB
    { A1028_CMD_SET_DIGI_OUTPUT_GAIN, /*0x00FA*//*0x00f7*/0x00f4}, //20110608 jhsong audience cal : Tx -6dB -> -9dB -> -12dB
    { A1028_CMD_SET_DIGI_INPUT_GAIN, 0x02ff }, //Far End -1dB
    { A1028_CMD_SET_DIGI_OUTPUT_GAIN, /*0x01fe*/0x01ff }, //Rx -2dB -> 0dB
    { 0xFFFF, 0xFFFF },
};
#else
static a1028_path_param path_ct_gain_param[] = 
{
    { A1028_CMD_SET_DIGI_INPUT_GAIN, 0x000D }, //Primary MIC 13dB
    { A1028_CMD_SET_DIGI_INPUT_GAIN, 0x010D }, //Secondary MIC 13dB
    { A1028_CMD_SET_DIGI_OUTPUT_GAIN, 0x00F9 }, //Tx -7dB
    { A1028_CMD_SET_DIGI_INPUT_GAIN, 0x02ff }, //Far End -1dB
    { A1028_CMD_SET_DIGI_OUTPUT_GAIN, 0x01fe }, //Rx -2dB
    { 0xFFFF, 0xFFFF },
};
#endif

//CT 1MIC Digital Gain
static a1028_path_param path_ct_1MIC_gain_param[] =
{
    { A1028_CMD_SET_DIGI_INPUT_GAIN, 0x0004 }, //Primary MIC
    { A1028_CMD_SET_DIGI_INPUT_GAIN, 0x0104 }, //Secondary MIC
    { A1028_CMD_SET_DIGI_OUTPUT_GAIN, 0x0004 }, //Tx
    { A1028_CMD_SET_DIGI_INPUT_GAIN, 0x0202 }, //Far End(Rx)
    { A1028_CMD_SET_DIGI_OUTPUT_GAIN, 0x0100 }, //Rx
    { 0xFFFF, 0xFFFF },
};

//CT VP Off Digital Gain
static a1028_path_param path_ct_VPOFF_gain_param[] = 
{
    { A1028_CMD_SET_DIGI_INPUT_GAIN, 0x0002 }, //Primary MIC
    { A1028_CMD_SET_DIGI_INPUT_GAIN, 0x0102 }, //Secondary MIC
    { A1028_CMD_SET_DIGI_OUTPUT_GAIN, 0x0002 }, //Tx
    { A1028_CMD_SET_DIGI_INPUT_GAIN, 0x0200 }, //Far End(Rx)
    { A1028_CMD_SET_DIGI_OUTPUT_GAIN, 0x01FD }, //Rx
    { 0xFFFF, 0xFFFF },
};

//CT 1MIC VP Off Digital Gain
static a1028_path_param path_ct_1MIC_VPOFF_gain_param[] = 
{
    { A1028_CMD_SET_DIGI_INPUT_GAIN, 0x0002 }, //Primary MIC
    { A1028_CMD_SET_DIGI_INPUT_GAIN, 0x0102 }, //Secondary MIC
    { A1028_CMD_SET_DIGI_OUTPUT_GAIN, 0x0002 }, //Tx
    { A1028_CMD_SET_DIGI_INPUT_GAIN, 0x0200 }, //Far End(Rx)
    { A1028_CMD_SET_DIGI_OUTPUT_GAIN, 0x01FD }, //Rx
    { 0xFFFF, 0xFFFF },
};


//FT Digital Gain
#if 1  // jmlee 1028 CT init data
static a1028_path_param path_ft_gain_param[] = 
{
    { A1028_CMD_SET_DIGI_INPUT_GAIN, 0x0013}, //Primary MIC  0x13(18dB)
    { A1028_CMD_SET_DIGI_INPUT_GAIN, 0x0113 }, //Secondary MIC 0x13(19dB)
    { A1028_CMD_SET_DIGI_OUTPUT_GAIN, 0x00fa }, //Tx -6dB
    { A1028_CMD_SET_DIGI_INPUT_GAIN, 0x0201/*0x0200*//*0x0206*/ }, //Far End(Rx) 0x06:(6 dB) //20110622 jhsong : audio cal case2
    { A1028_CMD_SET_DIGI_OUTPUT_GAIN, 0x0105/*0x0104*//*0x0106 */}, //Rx 0x06:(6 dB) //20110622 jhsong : audio cal case 2
    { 0xFFFF, 0xFFFF },
};
#else

static a1028_path_param path_ft_gain_param[] = 
{
    { A1028_CMD_SET_DIGI_INPUT_GAIN, 0x0012 }, //Primary MIC  0x12(18dB)
    { A1028_CMD_SET_DIGI_INPUT_GAIN, 0x0112 }, //Secondary MIC 0x12(18dB)
    { A1028_CMD_SET_DIGI_OUTPUT_GAIN, 0x0000 }, //Tx
    { A1028_CMD_SET_DIGI_INPUT_GAIN, 0x0200 }, //Far End(Rx) 0x00:(0 dB)
    { A1028_CMD_SET_DIGI_OUTPUT_GAIN, 0x0100 }, //Rx 0x00:(0 dB)
    { 0xFFFF, 0xFFFF },
};
#endif

//FT VP Off Digital Gain
static a1028_path_param path_ft_VPOFF_gain_param[] = 
{
    { A1028_CMD_SET_DIGI_INPUT_GAIN, 0x00FB }, //Primary MIC
    { A1028_CMD_SET_DIGI_INPUT_GAIN, 0x01FB }, //Secondary MIC
    { A1028_CMD_SET_DIGI_OUTPUT_GAIN, 0x0000 }, //Tx
    { A1028_CMD_SET_DIGI_INPUT_GAIN, 0x02FD }, //Far End(Rx)
    { A1028_CMD_SET_DIGI_OUTPUT_GAIN, 0x0101 }, //Rx
    { 0xFFFF, 0xFFFF },
};

//DV Digital Gain
#if 1  // jmlee 1028 DV init data
static a1028_path_param path_dv_gain_param[] = 
{
    { A1028_CMD_SET_DIGI_INPUT_GAIN, 0x0013/*0x0012*/ }, //Primary MIC  0x13(19dB)  //20110325 jhsong : audio cal
    { A1028_CMD_SET_DIGI_INPUT_GAIN, 0x0113/*0x0112*/ }, //Secondary MIC 0x13(19dB)  //20110325 jhsong : audio cal
    { A1028_CMD_SET_DIGI_OUTPUT_GAIN, 0x00fa/*0x00fe*//*0x00fa*/ }, //Tx  0xfe : (-2dB) //20110622 jhsong : audio cal case1
    { A1028_CMD_SET_DIGI_INPUT_GAIN ,0x201/*0x0200*/ /*0x0206*//*0x0200*/ }, //Far End(Rx) 0x00:(0 dB)  //20110622 jhsong : audio cal case2
    { A1028_CMD_SET_DIGI_OUTPUT_GAIN, 0x0105/*0x0104*//*0x0106*//*0x0100*/ }, //Rx 0x04:(4 dB)  //20110622 jhsong : audio cal case2
    { 0xFFFF, 0xFFFF },
};
#endif

//DV VP Off Digital Gain // jmlee
static a1028_path_param path_dv_VPOFF_gain_param[] = 
{
    { A1028_CMD_SET_DIGI_INPUT_GAIN, 0x00FB }, //Primary MIC
    { A1028_CMD_SET_DIGI_INPUT_GAIN, 0x01FB }, //Secondary MIC
    { A1028_CMD_SET_DIGI_OUTPUT_GAIN, 0x0000 }, //Tx
    { A1028_CMD_SET_DIGI_INPUT_GAIN, 0x02FD }, //Far End(Rx)
    { A1028_CMD_SET_DIGI_OUTPUT_GAIN, 0x0101 }, //Rx
    { 0xFFFF, 0xFFFF },
};


//Voice LoopBack Primary/Secondary MIC Digital Gain
static a1028_path_param path_vl_MIC_gain_param[] = 
{
    { A1028_CMD_SET_DIGI_INPUT_GAIN, 0x0004 }, //Primary MIC
    { A1028_CMD_SET_DIGI_INPUT_GAIN, /*0x0104 */0x0113}, //Secondary MIC 4dB -> 19dB
    { A1028_CMD_SET_DIGI_OUTPUT_GAIN, 0x0004}, //Tx
    { A1028_CMD_SET_DIGI_INPUT_GAIN, 0x0202 }, //Far End(Rx)
    { A1028_CMD_SET_DIGI_OUTPUT_GAIN, 0x0100 }, //Rx
    { 0xFFFF, 0xFFFF },
};

//InCall Primary/Secondary MIC Digital Gain
static a1028_path_param path_call_MIC_gain_param[] =
{
    { A1028_CMD_SET_DIGI_INPUT_GAIN, 0x0004 }, //Primary MIC
    { A1028_CMD_SET_DIGI_INPUT_GAIN, 0x0104 }, //Secondary MIC
    { A1028_CMD_SET_DIGI_OUTPUT_GAIN, 0x0004 }, //Tx
    { A1028_CMD_SET_DIGI_INPUT_GAIN, 0x0202 }, //Far End(Rx)
    { A1028_CMD_SET_DIGI_OUTPUT_GAIN, 0x0100 }, //Rx
    { 0xFFFF, 0xFFFF },
};

#ifdef FEATURE_SKY_SND_EXTAMPCTRL  //20110519 jhsong : sub chip cal tool by youngsoo Lim
static uint32_t extamp_cal_dev_id = 0;
extern bool LockForTest;
static a1028_path_param a1028_gain_cal[6] = {{0,0},{0,0},{0,0},{0,0},{0,0},{0,0}};
static a1028_algo_param a1028_algo_cal[10] = {{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0}};
static uint32_t ext_audience_device = 0x00;
static int ext_audience_pth_on = 0;

void audience_set_cal_value_data(struct extamp_device_info *dev_info)
{
//	tSubsystemInfo_cal.power_info = (u8)DEFAULT_DATA_REG;
	extamp_cal_dev_id = dev_info->extamp_dev_id;

	a1028_gain_cal[0].value= (u16) (0x0000) | (dev_info->audience_input_gain_id_prim_mic_gain);
	a1028_gain_cal[1].value= (u16) (0x0100) | (dev_info->audience_input_gain_id_sec_mic_gain);
	a1028_gain_cal[2].value= (u16) (0x0000) | (dev_info->audience_digital_out_tx_gain);
	a1028_gain_cal[3].value= (u16) (0x0200) | (dev_info->audience_input_gain_id_far_end_gain);
	a1028_gain_cal[4].value= (u16) (0x0100) | (dev_info->audience_digital_out_rx_gain);
	a1028_gain_cal[5].value= (u16) 0xffff;

	a1028_algo_cal[0].value = (u16) (dev_info->audience_algo_param_id_aec_mode);
	a1028_algo_cal[1].value = (u16) (dev_info->audience_algo_param_id_se_mode);
	a1028_algo_cal[2].value = (u16) ((dev_info->audience_algo_param_id_downlink_spk_vol_1)<<2) | ((dev_info->audience_algo_param_id_downlink_spk_vol_2));
	a1028_algo_cal[3].value = (u16) (dev_info->audience_algo_param_id_aec_comfort_noise_mode);
	a1028_algo_cal[4].value = (u16) ((dev_info->audience_algo_param_id_ec_supp_enhanced_1)<<2) | ((dev_info->audience_algo_param_id_ec_supp_enhanced_2));
	a1028_algo_cal[5].value = (u16) ((dev_info->audience_algo_param_id_veq_noise_estimate_adj_1)<<2) | ((dev_info->audience_algo_param_id_veq_noise_estimate_adj_2));
	a1028_algo_cal[6].value = (u16) ((dev_info->audience_algo_param_id_veq_max_gain_1)<<2) | ((dev_info->audience_algo_param_id_veq_max_gain_2));
	a1028_algo_cal[7].value = (u16) (dev_info->audience_algo_param_id_far_end_noise_supp);
	a1028_algo_cal[8].value = (u16) ((dev_info->audience_algo_param_id_fens_amount_1)<<2) | ((dev_info->audience_algo_param_id_fens_amount_2));
	a1028_algo_cal[9].value= (u16) 0xffff;

	
//	printk("@#@#fab2200_set_cal_value_data.......0x02 : 0x%x,    0x05 : 0x%x,     0x06 : 0x%x\n",tSubsystemInfo_cal.diff_pga_info,tSubsystemInfo_cal.hpl_vol_info,tSubsystemInfo_cal.hpr_vol_info );
//	printk("@#@#fab2200_set_cal_value_data.......0x07 : 0x%x,    0x08 : 0x%x,     0x09 : 0x%x\n",tSubsystemInfo_cal.hp_ng_info,tSubsystemInfo_cal.spk_ramp_zc_info,tSubsystemInfo_cal.spk_ng_info );
//	printk("@#@#fab2200_set_cal_value_data.......0x0a : 0x%x,    0x0b : 0x%x,     0x0c : 0x%x\n",tSubsystemInfo_cal.thresh_info,tSubsystemInfo_cal.agc_info,tSubsystemInfo_cal.agc_min_info );
//	printk("@#@#fab2200_set_cal_value_data.......0x0e : 0x%x,    0x0f : 0x%x\n",tSubsystemInfo_cal.start_gain_info,tSubsystemInfo_cal.ssmt_info);
}

void snd_extamp_api_audience_SetVolume_ById(uint32_t extamp_dev_id) {
//	extamp_info_t tCurrentExtampInfo;
	struct extamp_device_info *di;
//	int status = 0;

	if(log_enable) printk(KERN_ERR "[[ MAX97001 ]] ### extamp_dev_id(0x%x)\n", extamp_dev_id);

#if 0 //def FEATURE_SKY_SND_EXTAMPCTRL_FOR_APP
	if (LockForTest == 1) return;
#endif /* FEATURE_SKY_SND_EXTAMPCTRL_FOR_APP */

	ext_audience_device = extamp_dev_id;
	
	di = extamp_lookup_device(extamp_dev_id);

/*************************************************************************************/
/* Need to modify each Audio sub system.                                             */
/*************************************************************************************/
	audience_set_cal_value_data(di);

#if 0
//	status|=snd_subsystem_i2c_write(POWER_REG, tSubsystemInfo.power_info); 					  // 0x01 register set default param
	status|=snd_subsystem_i2c_write(DIFF_PGA_REG, tSubsystemInfo_cal.diff_pga_info);				  // 0x02 register set default param
	status|=snd_subsystem_i2c_write(INPUT_SPK_HP_ZC_REG, tSubsystemInfo_cal.input_spk_hp_zc_info);  // 0x03 register set default param
	status|=snd_subsystem_i2c_write(INPUT_HP_REG, tSubsystemInfo_cal.input_hp_info);				  // 0x04 register set default param
	status|=snd_subsystem_i2c_write(HPL_VOL_REG, tSubsystemInfo_cal.hpl_vol_info);					  // 0x05 register set default param
	status|=snd_subsystem_i2c_write(HPR_VOL_REG, tSubsystemInfo_cal.hpr_vol_info);				  // 0x06 register set default param
	status|=snd_subsystem_i2c_write(HP_NG_REG, tSubsystemInfo_cal.hp_ng_info);					  // 0x07 register set default param
	status|=snd_subsystem_i2c_write(SPK_RAMP_ZC_REG, tSubsystemInfo_cal.spk_ramp_zc_info);  		  // 0x08 register set default param
	status|=snd_subsystem_i2c_write(SPK_NG_REG, tSubsystemInfo_cal.spk_ng_info);					  // 0x09 register set default param
	status|=snd_subsystem_i2c_write(THRESH_REG, tSubsystemInfo_cal.thresh_info);  					  // 0x0a register set default param
	status|=snd_subsystem_i2c_write(AGC_REG, tSubsystemInfo_cal.agc_info); 						  // 0x0b register set default param
	status|=snd_subsystem_i2c_write(AGC_MIN_REG, tSubsystemInfo_cal.agc_min_info);				  // 0x0c register set default param
//	status|=snd_subsystem_i2c_write(PRESENT_GAIN_REG, tSubsystemInfo.present_gain_info);		  // 0x0d register set default param
	status|=snd_subsystem_i2c_write(START_GAIN_REG, tSubsystemInfo_cal.start_gain_info);			  // 0x0e register set default param
	status|=snd_subsystem_i2c_write(SSMT_REG, tSubsystemInfo_cal.ssmt_info);						  // 0x0f register set default param
#endif

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

#if 1
void A1028_msgCB_Chip_AckDataFlush(void)
{
    byte read_buf[4];
    struct i2c_msg msg;
    static int ret_value = 0;
    int check_count = 0;
	
    u32 get_value = 0;
    u32 swap_value = 0;

	while(1)
	{
	    if(!audience_a1028_i2c_client){
	        break;
	    }

	    memset((void*)read_buf, 0x00, 4);
		
	    msg.addr = audience_a1028_i2c_client->addr;
	    msg.flags = I2C_M_RD;
	    msg.len = 4;
	    msg.buf = read_buf;
	    
	    ret_value = i2c_transfer(audience_a1028_i2c_client->adapter, &msg, 1);

	    if (ret_value < 0) 
	    {
	       printk( "[Snd_audience_a1028][-=AUDIO=-] A1028_msgCB_Chip_AckDataFlush I2C ERROR OCCURE~~   I2C Error Num%d   !!!\n",ret_value);
	    }

	    memcpy((byte*)&get_value,read_buf,4);
	    swap_value = SWAP32(get_value);
		
	    if(log_enable) printk( "[Snd_audience_a1028][-=AUDIO=-] A1028_msgCB_Chip_AckDataFlush 0x%x !!!\n",swap_value);
		
	    if(swap_value == 0x0000)
	    {
	        break;
	    }
		
	    if(swap_value == 0xffff)
	    {
	        check_count++;
	        if(check_count > 4)
	        {
                   printk( "[Snd_audience_a1028][-=AUDIO=-] A1028_msgCB_Chip_AckDataFlush swap_value IS 0xffff 4 times 0x%x !!!\n",swap_value);
		     break;
	        }
	    }
		
	}

}
#endif

int aud_a1028_is_active_mode(void)
{
    if(a1028_mode == A1028_ACTIVE_MODE)
        return 1;
    else
        return 0;
}

int aud_a1028_is_sleep_mode(void)
{
    if(a1028_mode == A1028_SLEEP_MODE)
        return 1;
    else
        return 0;
}

// length : byte num for sending (cmd + value)
// lengh limit 4 
// error num #define	ENXIO		 6	/* No such device or address */
int A1028_msg(u16 cmd, u16 value, u16 length)
{
    static int ret_value = 0;
    u16 len = 0;
    u16 swap_value = 0;

    unsigned char buf[4];	
    struct i2c_msg msg;

    if(len > 4) return 0;
	
    if(!audience_a1028_i2c_client){
        return 0;
    }
	
    len = length;
	
    swap_value = SWAP16(cmd);
    memcpy((byte*)&buf[0],(byte*)&swap_value, 2);
    swap_value = SWAP16(value);
    memcpy((byte*)&buf[2],(byte*)&swap_value, 2);
	

    msg.addr = audience_a1028_i2c_client->addr;
    msg.flags = 0;
    msg.len = len;
    msg.buf = (char *)buf;
    
    ret_value = i2c_transfer(audience_a1028_i2c_client->adapter, &msg, 1);
	
    if(log_enable) printk( "[Snd_audience_a1028][-=AUDIO=-] A1028_msg cmd : 0x%x  value : 0x%x  I2C Return Value : [%d] !!!\n",cmd, value,ret_value);

    if (ret_value < 0) 
    {
        return 0;
    }

    return 1;	
}

int A1028_msgCB(u16 read_byte, byte * read_buf)
{
    static int ret_value = 0;
    struct i2c_msg msg;
	
    u32 get_value = 0;
    u32 swap_value = 0;

    if(!audience_a1028_i2c_client){
        return 0;
    }

    memset((void*)read_buf, 0x00, read_byte);
	
    msg.addr = audience_a1028_i2c_client->addr;
    msg.flags = I2C_M_RD;
    msg.len = read_byte;
    msg.buf = read_buf;
    
    ret_value = i2c_transfer(audience_a1028_i2c_client->adapter, &msg, 1);

    if (ret_value < 0) 
    {
       printk( "[Snd_audience_a1028][-=AUDIO=-] A1028_msgCB I2C ERROR OCCURE~~   I2C Error Num%d   !!!\n",ret_value);
       return 0;
    }

    memcpy((byte*)&get_value,read_buf,4);
    swap_value = SWAP32(get_value);
	
    if(log_enable) printk( "[Snd_audience_a1028][-=AUDIO=-] A1028_msgCB 0x%x !!!\n",swap_value);
	
    if(swap_value == 0x00)
    {
        //A1028 not ready
        return 0;
    }

    return 1;
}

int aud_boot_cmd_to_1028(void)
{
    int ret_val = 0;
    byte count = 0;

    while(1)
    {
        ret_val = A1028_msg(A1028_CMD_BOOT, 0, 2); //send 2 bytes to the A1028

        mdelay(1); //sleep needed for firmware to process

        if(ret_val == 1)
        {
            byte read_buf[1];
            ret_val = A1028_msgCB(1, read_buf); //read the 1 byte ACK back

            if(ret_val == 1)
            {
                if(read_buf[0] != A1028_CMD_BOOT_ACK_A1028)					
                {
                    count++;
                    mdelay(1);	

                    if(count > POLLING_LOOP_COUNT)
                    {
                        printk( "[Snd_audience_a1028][-=AUDIO=-] A1028 Boot CMD ACK NOK !!!\n");
                        break;
                    }
                }
                else
                {
                    if(log_enable) printk( "[Snd_audience_a1028][-=AUDIO=-] A1028 Boot CMD ACK OK !!!\n");
                    break;
                }            
            }
            else
            {
                count++;
                mdelay(1);

                if(count > POLLING_LOOP_COUNT)
                {
                   printk( "[Snd_audience_a1028][-=AUDIO=-] A1028 Boot CMD I2C ACK NOK !!!\n");
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
                printk( "[Snd_audience_a1028][-=AUDIO=-] A1028 Boot CMD I2C Write I2C NOK !!!\n");				
                break;
            }
        }
    }

    return ret_val;	
}

int aud_bootimage_download(void)
{

    struct i2c_msg msg;
    u16 len = 0;
    int ret_val = 0;
	
    if(!audience_a1028_i2c_client){
        return 0;
    }
   
    len = sizeof(a1028_firmware_image);
    
    msg.addr = audience_a1028_i2c_client->addr;
    msg.flags = 0;
    msg.len = len;
    msg.buf = (char *)a1028_firmware_image;
    
    ret_val = i2c_transfer(audience_a1028_i2c_client->adapter, &msg, 1);
	
    if (ret_val < 0) 
    {
        printk( "[Snd_audience_a1028][-=AUDIO=-] A1028 Boot DL I2C NOK !!!\n");   
        return 0;
    }
    
    if(log_enable) printk( "[Snd_audience_a1028][-=AUDIO=-] A1028 Boot DL OK !!!\n");
    return 1;
}

int aud_bootload_initiate(void)
{
    int ret_val = 0;

    ret_val = aud_bootimage_download();	
    return ret_val;
}

int aud_sync_cmd_to_1028(void)
{
    int ret_val = 0;
    u32 get_value = 0;
    u32 swap_value = 0;
    byte count = 0;

    while(1)
    {
        ret_val = A1028_msg(A1028_CMD_SYNC, A1028_CMD_SYNC_MODE_POLLING, 4);

        if(ret_val == 1)
        {
            byte read_buf[4];
            if(bBootSequence)
                mdelay(20); //need to wait 20ms for firmware to respond
            else
                mdelay(1); //wait 1ms to try another read attempt
			
            ret_val = A1028_msgCB(4,read_buf);
    
            if(ret_val == 1)
            {
                memcpy((byte*)&get_value, read_buf, 4);
                swap_value = SWAP32(get_value);
                    		
                if(swap_value != A1028_CMD_SYNC_ACK)
                {
                    count++;
                    mdelay(1);
					
                    if(count > POLLING_LOOP_COUNT)
                    {
                        printk( "[Snd_audience_a1028][-=AUDIO=-] A1028 Boot Sync CMD ACK NOK !!!\n");				
                        break;
                    }
                }
                else
                {
                    if(log_enable) printk( "[Snd_audience_a1028][-=AUDIO=-] A1028 Boot Sync CMD ACK OK !!!\n");  // -2147483648				
                    break;
                }                
            }
            else
            {
                count++;
                mdelay(1);
        
                if(count > POLLING_LOOP_COUNT)
                {
                    if(log_enable) printk( "[Snd_audience_a1028][-=AUDIO=-] A1028 Boot Sync POLLING_LOOP_COUNT %d !!!\n",count);					
                    break;
                }
            }			
        }
        else
        {
            count++;   
            mdelay(1);
            printk( "[Snd_audience_a1028][-=AUDIO=-] A1028 Boot Sync CMD I2C Write NOK !!!\n");		

            if(count > POLLING_LOOP_COUNT)
            {
                printk( "[Snd_audience_a1028][-=AUDIO=-] A1028 Boot Sync CMD I2C Write NOK !!!\n");		
                break;
            }
        }
    }
    
    return ret_val;
}

int aud_sleep_cmd_to_1028(void)
{
    int ret_val = 0;
    u32 get_value = 0;
    u32 swap_value = 0;
    byte count = 0;
    
    if(aud_a1028_is_sleep_mode())
        return 1;
#if 1   // jmlee 
        A1028_msgCB_Chip_AckDataFlush();
        if(log_enable) printk( "[Snd_audience_a1028][-=AUDIO=-] ######### aud_sync_cmd_to_1028() Test IN  aud_sleep_cmd_to_1028() !!!\n");
#endif        		
    while(1)
    {
        ret_val = A1028_msg(A1028_CMD_SLEEP, A1028_CMD_SLEEP_VALUE, 4);
        
        if(ret_val == 1)
        {
            byte read_buf[4];
            mdelay(20); //After sending the sleep command
            //aud_a1028_mdelay(30); // jmlee
			
            ret_val = A1028_msgCB(4,read_buf);

            if(ret_val == 1)
            {
                memcpy((byte*)&get_value,read_buf,4);
                swap_value = SWAP32(get_value);
                		
                if(swap_value != A1028_CMD_SLEEP_ACK)
                {
                    count++;                    
                    mdelay(1);	

                    if(count > POLLING_LOOP_COUNT)
                    {
                        printk( "[Snd_audience_a1028][-=AUDIO=-] A1028 Sleep CMD ACK NOK !!!\n");			
                        break;
                    }
                }
                else
                {
                    if(log_enable) printk( "[Snd_audience_a1028][-=AUDIO=-] A1028 Sleep CMD ACK OK !!!\n");
                    break;
                }
            }
            else
            {
                count++;                
                mdelay(1);

                if(count > POLLING_LOOP_COUNT)
                {
                    printk( "[Snd_audience_a1028][-=AUDIO=-] A1028 Sleep CMD I2C ACK NOK !!!\n");	
                    break;
                }
            }			
        }
        else
        {
            count++;            
            if(count > POLLING_LOOP_COUNT)
            {
                printk( "[Snd_audience_a1028][-=AUDIO=-] A1028 Sleep CMD I2C Write NOK !!!\n");	
                break;
            }
        }		
    }

    return ret_val;
}

void aud_a1028_clk_on(void)
{
    //pm_vreg_control(PM_ON_CMD, PM_VREG_WLAN_M);

    //Clock Stablize
    //Wait 4 Clock Cycles
    mdelay(4);
}

void aud_a1028_clk_off(void)
{
    if(a1028_mode == A1028_SLEEP_MODE)
    {
        //pm_vreg_control(PM_OFF_CMD, PM_VREG_WLAN_M);
    }
}

int aud_a1028_wakeup_sequence(void)
{
    int ret_val = 0;

#if 0
    if(bBootSequence == 0)
    {
        //Booting Ï¥àÍ∏∞??Audio ControlÎ∂ÄÎ∂ÑÏù¥ ?ôÏûë???òÍ∏∞??
        //Booting???ïÌôï???¥Î£®?¥Ï?Í∏??ÑÏóê???§Î•∏ ModeÎ°?ÏßÑÏûÖ???†Ïàò ?ÜÎèÑÎ°?Ï°∞Ïπò
        return 1;
    }
#endif

    if(log_enable) printk( "[Snd_audience_a1028][-=AUDIO=-] A1028 Wake Up Start !!!\n");	

    aud_a1028_clk_on();

    //First toggle the wake up pin to start the sequence 
    gpio_set_value(GPIO_NR_WAKEUP_N,0);
	
    mdelay(10); //need to wait 10ms for warm boot to finish xxx --- jmlee : more than 10ms needed with MINI
    mdelay(10);  // jmlee add more time
    mdelay(10);  // jmlee add more time    
    
    ret_val = aud_sync_cmd_to_1028();
    if(log_enable) printk( "[Snd_audience_a1028][-=AUDIO=-] ============================================ A1028 SYNC CHECK 5 ]]]]>>> !!!\n");	
	
    //Put the wake up pin in the default state for the next wake up sequence	
    gpio_set_value(GPIO_NR_WAKEUP_N,1);

    if(ret_val == 1)
    {
        a1028_mode = A1028_WAKEUP_MODE;
        if(log_enable) printk( "[Snd_audience_a1028][-=AUDIO=-] A1028 Wake Up Sequence OK !!!\n");
    }
    else
    {
        printk( "[Snd_audience_a1028][-=AUDIO=-] A1028 Wake Up Sequence NOK !!!\n");			
        a1028_mode = A1028_MAX_MODE;
    }
	
    return ret_val;
}

void set_gpio_aux_config(void)
{ 
#if 0 //[CHD BB08]
    if(a1028_gpio_mode !=  A1028_GPIO_AUXPCM_MODE)
    {
        //One Minute & ?µÌôîÏ§?SMS?êÏÑú SyncÍ∞Ä Î¨¥ÎÑàÏßÑÎã§.
        //?¨ÌòÑ?Ä ???àÎêòÏßÄÎß??ÑÏÉÅ???ïÏù∏???àÍ∏∞??Î∞©Ïñ¥Code?ΩÏûÖ

        gpio_tlmm_config(AUX_PCM_SYNC);
        gpio_tlmm_config(AUX_PCM_CLK);	
        gpio_tlmm_config(AUX_PCM_DOUT);
        gpio_tlmm_config(AUX_PCM_DIN);
        
        a1028_gpio_mode =  A1028_GPIO_AUXPCM_MODE;        
    }
#endif	
}

int aud_config_sw_reset_cmd_to_1028(a1028_talk_mode talk_mode)
{
    int ret_val = 0;
    byte count = 0;
    byte index = 0;
    u16 size = 0;

    if(a1028_mode != A1028_ACTIVE_MODE)
        return 0;

    size = sizeof(sw_reset_param);    
    size = size/sizeof(a1028_reset_param);
    
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
            case DV_TALK:  // jmlee
            case DV_VPOFF_TALK:   // jmlee         				
            case VL_PRI_MIC_TALK:
            case VL_SEC_MIC_TALK:
            case CALL_PRI_MIC_TALK:
            case CALL_SEC_MIC_TALK:
                ret_val = A1028_msg(sw_reset_param[index].cmd, sw_reset_param[index].value, 4);	
                break;

           default:
                ret_val = A1028_msg(sw_reset_param[index].cmd, sw_reset_param[index].value, 4);
                break;                
        }

        if(ret_val == 1)
        {
            if(log_enable) printk( "[Snd_audience_a1028][-=AUDIO=-] A1028 SW Reset Write OK !!!\n");		
            mdelay(120);
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
                printk( "[Snd_audience_a1028][-=AUDIO=-] A1028 SW Reset Write NOK !!!\n");
                break;
            }        
        }
    }

    return ret_val;	
}

int aud_config_device_cmd_to_1028(a1028_talk_mode talk_mode)
{
    int ret_val = 0;
    byte count = 0;
    byte index = 0;
    u16 size = 0;
	
    if(a1028_mode != A1028_ACTIVE_MODE)
        return 0;

    size = sizeof(device_param);
    size = size/sizeof(a1028_device_param);
    
    while(1)
    {
        while(1)
        {
            if(device_param[index].cmd1 == 0xFFFF && device_param[index].cmd2 == 0xFFFF)
                break;

            mdelay(1);

            ret_val = A1028_msg(device_param[index].cmd1, device_param[index].id, 4);				

            if(ret_val == 1)
            {
                if(log_enable) printk( "[Snd_audience_a1028][-=AUDIO=-] A1028 Device Param Write OK_1 !!!\n");

                mdelay(1);
        
                if(device_param[index].cmd2 == 0xFFFF && device_param[index].value == 0xFFFF)
                    break;

                ret_val = A1028_msg(device_param[index].cmd2, device_param[index].value, 4);					
                
                if(ret_val == 1)
                {
                    if(log_enable) printk( "[Snd_audience_a1028][-=AUDIO=-] A1028 Device Param Write ACK OK_2 !!!\n");
                    break;                
                }
                else
                {
                    count++;
                    			
                    if(count > POLLING_LOOP_COUNT)
                    {
                        printk( "[Snd_audience_a1028][-=AUDIO=-] A1028 Device Param Write NOK_2 !!!\n");
                        break;
                    }
                }
            }
            else
            {
                count++;
                				
                if(count > POLLING_LOOP_COUNT)
                {
                    printk( "[Snd_audience_a1028][-=AUDIO=-] A1028 Device Param Write NOK_1 !!!\n");	
                    break;
                }            
            }
        }
    
        if(ret_val == 0)
            break;
        else
            index++;
    
        if(index >= size) //?ÑÏû¨ Size = 17, Index = 16
        {
            break;
        }
    }

    return ret_val;
}

int aud_config_algo_cmd_to_1028(a1028_talk_mode talk_mode)
{
    int ret_val = 0;
    byte count = 0;
    byte index = 0;
    u16 size = 0;
#if 0 //20110624 jhsong : remove retry
    u32 get_value = 0;
    u32 swap_value1 = 0;
    u32 swap_value2 = 0;
#endif

    if(a1028_mode != A1028_ACTIVE_MODE)
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

        case DV_TALK: // jmlee
            size = sizeof(algo_dv_param);
            break;

        case DV_VPOFF_TALK:  // jmlee
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
            printk( "[Snd_audience_a1028][-=AUDIO=-] A1028 Param Size Error !!!\n");
            break;
    }

    size = size/sizeof(a1028_algo_param);
    
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
            else if(talk_mode == DV_TALK) // jmlee
            {
                if(algo_dv_param[index].cmd1 == 0xFFFF && algo_dv_param[index].cmd2 == 0xFFFF)
                    break;            
            }
            else if(talk_mode == DV_VPOFF_TALK) // jmlee
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
                ret_val = A1028_msg(algo_ct_param[index].cmd1, algo_ct_param[index].id, 4);
                if(log_enable) printk( "[Snd_audience_a1028]CMD1 : %x, ID : %x!!!", algo_ct_param[index].cmd1, algo_ct_param[index].id);
            }
            else if(talk_mode == CLOSE_VPOFF_TALK)
            {
                ret_val = A1028_msg(algo_ct_VPOFF_param[index].cmd1, algo_ct_VPOFF_param[index].id, 4);
                if(log_enable) printk( "[Snd_audience_a1028]CMD1 : %x, ID : %x!!!", algo_ct_VPOFF_param[index].cmd1, algo_ct_VPOFF_param[index].id);
            }
            else if(talk_mode == CLOSE_1MIC_TALK)
            {
                ret_val = A1028_msg(algo_ct_1MIC_param[index].cmd1, algo_ct_1MIC_param[index].id, 4);
                if(log_enable) printk( "[Snd_audience_a1028]CMD1 : %x, ID : %x!!!", algo_ct_1MIC_param[index].cmd1, algo_ct_1MIC_param[index].id);
            }
            else if(talk_mode == CLOSE_1MIC_VPOFF_TALK)
            {
                ret_val = A1028_msg(algo_ct_1MIC_VPOFF_param[index].cmd1, algo_ct_1MIC_VPOFF_param[index].id, 4);
                if(log_enable) printk( "[Snd_audience_a1028]CMD1 : %x, ID : %x!!!", algo_ct_1MIC_VPOFF_param[index].cmd1, algo_ct_1MIC_VPOFF_param[index].id);
            }
            else if(talk_mode == FAR_TALK)
            {
#if 0 // jmlee  //20110624 jhsong
                A1028_msgCB_Chip_AckDataFlush(); // for msg CB 
#endif
                ret_val = A1028_msg(algo_ft_param[index].cmd1, algo_ft_param[index].id, 4);
                if(log_enable) printk( "[Snd_audience_a1028]CMD1 : %x, ID : %x!!!", algo_ft_param[index].cmd1, algo_ft_param[index].id);
            }
            else if(talk_mode == FAR_VPOFF_TALK)
            {
                ret_val = A1028_msg(algo_ft_VPOFF_param[index].cmd1, algo_ft_VPOFF_param[index].id, 4);
                if(log_enable) printk( "[Snd_audience_a1028]CMD1 : %x, ID : %x!!!", algo_ft_VPOFF_param[index].cmd1, algo_ft_VPOFF_param[index].id);
            }
            else if(talk_mode == DV_TALK)
            {
                ret_val = A1028_msg(algo_dv_param[index].cmd1, algo_dv_param[index].id, 4);
                if(log_enable) printk( "[Snd_audience_a1028]CMD1 : %x, ID : %x!!!", algo_dv_param[index].cmd1, algo_dv_param[index].id);
            }
            else if(talk_mode == DV_VPOFF_TALK)
            {
                ret_val = A1028_msg(algo_dv_VPOFF_param[index].cmd1, algo_dv_VPOFF_param[index].id, 4);
                if(log_enable) printk( "[Snd_audience_a1028]CMD1 : %x, ID : %x!!!", algo_dv_VPOFF_param[index].cmd1, algo_dv_VPOFF_param[index].id);
            }			
            else if(talk_mode == VL_PRI_MIC_TALK || talk_mode == VL_SEC_MIC_TALK)
            {
                ret_val = A1028_msg(algo_ct_vl_MIC_param[index].cmd1, algo_ct_vl_MIC_param[index].id, 4);
                if(log_enable) printk( "[Snd_audience_a1028]CMD1 : %x, ID : %x!!!", algo_ct_vl_MIC_param[index].cmd1, algo_ct_vl_MIC_param[index].id);
            }
            else if(talk_mode == CALL_PRI_MIC_TALK || talk_mode == CALL_SEC_MIC_TALK)
            {
                ret_val = A1028_msg(algo_ct_call_MIC_param[index].cmd1, algo_ct_call_MIC_param[index].id, 4);
                if(log_enable) printk( "[Snd_audience_a1028]CMD1 : %x, ID : %x!!!", algo_ct_call_MIC_param[index].cmd1, algo_ct_call_MIC_param[index].id);
            }
            else
            {
                ret_val = A1028_msg(algo_ct_param[index].cmd1, algo_ct_param[index].id, 4);				
                if(log_enable) printk( "[Snd_audience_a1028]CMD1 : %x, ID : %x!!!", algo_ct_param[index].cmd1, algo_ct_param[index].id);
            }

            if(ret_val == 1)
            {
                if(log_enable) printk( "[Snd_audience_a1028][-=AUDIO=-] A1028 Param Write OK_1 !!!\n");

                mdelay(1);
        
#if 0 //20110624 jhsong : remove retry
                if(talk_mode == FAR_TALK)
                {
                    byte read_buf[4];
                    ret_val = A1028_msgCB(4,read_buf);
                    
                    if(ret_val == 1)
                    {
                        memcpy((byte*)&get_value, read_buf, 2);
                        swap_value1 = SWAP16(get_value);
                        	
                        memcpy((byte*)&get_value, &read_buf[2], 2);
                        swap_value2 = SWAP16(get_value);
        
                        if(swap_value1 != A1028_CMD_ALGO_PARAMID)
                        {
                            count++;
                            mdelay(20);
        
                            if(count > POLLING_LOOP_COUNT)
                            {
                                printk( "[Snd_audience_a1028][-=AUDIO=-] A1028 Algo CMD1 ACK NOK !!!\n");
                                break;
                            }
                        }
                        else
                        {
                            if(log_enable) printk( "[Snd_audience_a1028]Algo CMD1 ACK OK, Val2 = %x, FT ID = %x !!!", swap_value2, algo_ft_param[index].id);
    
                            if(swap_value2 == algo_ft_param[index].id)
                            {
                                if(log_enable) printk( "[Snd_audience_a1028][-=AUDIO=-] A1028 Algo Param ID ACK OK !!!\n");                        
                            }
                            else
                            {
                                count++;
                                mdelay(20);
            
                                if(count > POLLING_LOOP_COUNT)
                                {
                                    printk( "[Snd_audience_a1028][-=AUDIO=-] A1028 Algo Param ID ACK NOK !!!\n");
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
                            printk( "[Snd_audience_a1028][-=AUDIO=-] A1028 Algo CMD1 Response ACK NOK !!!\n");
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
                else if(talk_mode == DV_TALK) // jmlee
                {
                    if(algo_dv_param[index].cmd2 == 0xFFFF && algo_dv_param[index].value == 0xFFFF)
                        break;
                }
                else if(talk_mode == DV_VPOFF_TALK) // jmlee
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
	                    ret_val = A1028_msg(algo_ct_param[index].cmd2, a1028_algo_cal[index].value, 4);					
		       }else{
	                    ret_val = A1028_msg(algo_ct_param[index].cmd2, algo_ct_param[index].value, 4);					
		       }
		    }else
#endif
		    {
                    ret_val = A1028_msg(algo_ct_param[index].cmd2, algo_ct_param[index].value, 4);					
                    if(log_enable) printk( "[Snd_audience_a1028]CMD2 : %x, Value : %x!!!", algo_ct_param[index].cmd2, algo_ct_param[index].value);
		    }
                }
                else if(talk_mode == CLOSE_VPOFF_TALK)
                {
                    ret_val = A1028_msg(algo_ct_VPOFF_param[index].cmd2, algo_ct_VPOFF_param[index].value, 4);
                    if(log_enable) printk( "[Snd_audience_a1028]CMD2 : %x, Value : %x!!!", algo_ct_VPOFF_param[index].cmd2, algo_ct_VPOFF_param[index].value);
                }
                else if(talk_mode == CLOSE_1MIC_TALK)
                {
                    ret_val = A1028_msg(algo_ct_1MIC_param[index].cmd2, algo_ct_1MIC_param[index].value, 4);
                    if(log_enable) printk( "[Snd_audience_a1028]CMD2 : %x, Value : %x!!!", algo_ct_1MIC_param[index].cmd2, algo_ct_1MIC_param[index].value);
                }
                else if(talk_mode == CLOSE_1MIC_VPOFF_TALK)
                {
                    ret_val = A1028_msg(algo_ct_1MIC_VPOFF_param[index].cmd2, algo_ct_1MIC_VPOFF_param[index].value, 4);					
                    if(log_enable) printk( "[Snd_audience_a1028]CMD2 : %x, Value : %x!!!", algo_ct_1MIC_VPOFF_param[index].cmd2, algo_ct_1MIC_VPOFF_param[index].value);
                }
                else if(talk_mode == FAR_TALK)
                {
#if 0 // jmlee  //20110624 jhsong
                A1028_msgCB_Chip_AckDataFlush(); // for msg CB 
#endif                
#ifdef FEATURE_SKY_SND_EXTAMPCTRL  //20110519 jhsong : sub chip cal tool by youngsoo Lim
		    if(LockForTest){
			if((index == 2) || (index == 3) || (index == 4) || (index == 5) ||(index == 6) || (index == 7) || (index == 8) || (index == 9) ||(index == 10)){
	                    ret_val = A1028_msg(algo_ft_param[index].cmd2, a1028_algo_cal[index].value, 4);					
		       }else{
	                    ret_val = A1028_msg(algo_ft_param[index].cmd2, algo_ft_param[index].value, 4);
		       }
		    }else
#endif
		    {
                    ret_val = A1028_msg(algo_ft_param[index].cmd2, algo_ft_param[index].value, 4);
                    if(log_enable) printk( "[Snd_audience_a1028]CMD2 : %x, Value : %x!!!", algo_ft_param[index].cmd2, algo_ft_param[index].value);
		    }
                }
                else if(talk_mode == FAR_VPOFF_TALK)
                {
                    ret_val = A1028_msg(algo_ft_VPOFF_param[index].cmd2, algo_ft_VPOFF_param[index].value, 4);
                   if(log_enable)  printk( "[Snd_audience_a1028]CMD2 : %x, Value : %x!!!", algo_ft_VPOFF_param[index].cmd2, algo_ft_VPOFF_param[index].value);
                }
                else if(talk_mode == DV_TALK)
                {
#ifdef FEATURE_SKY_SND_EXTAMPCTRL  //20110519 jhsong : sub chip cal tool by youngsoo Lim
		    if(LockForTest){
			if((index == 2) || (index == 3) || (index == 4) || (index == 5) ||(index == 6) || (index == 7) || (index == 8) || (index == 9) ||(index == 10)){
	                    ret_val = A1028_msg(algo_dv_param[index].cmd2, a1028_algo_cal[index].value, 4);					
		       }else{
	                   ret_val = A1028_msg(algo_dv_param[index].cmd2, algo_dv_param[index].value, 4);
		       }
		    }else
#endif
		    {
                    ret_val = A1028_msg(algo_dv_param[index].cmd2, algo_dv_param[index].value, 4);
                    if(log_enable) printk( "[Snd_audience_a1028]CMD2 : %x, Value : %x!!!", algo_dv_param[index].cmd2, algo_dv_param[index].value);
		    }
                }
                else if(talk_mode == DV_VPOFF_TALK)
                {
                    ret_val = A1028_msg(algo_dv_VPOFF_param[index].cmd2, algo_dv_VPOFF_param[index].value, 4);
                    if(log_enable) printk( "[Snd_audience_a1028]CMD2 : %x, Value : %x!!!", algo_dv_VPOFF_param[index].cmd2, algo_dv_VPOFF_param[index].value);
                }
                else if(talk_mode == VL_PRI_MIC_TALK || talk_mode == VL_SEC_MIC_TALK)
                {
                    ret_val = A1028_msg(algo_ct_vl_MIC_param[index].cmd2, algo_ct_vl_MIC_param[index].value, 4);
                    if(log_enable) printk( "[Snd_audience_a1028]CMD2 : %x, Value : %x!!!", algo_ct_vl_MIC_param[index].cmd2, algo_ct_vl_MIC_param[index].value);
                }
                else if(talk_mode == CALL_PRI_MIC_TALK || talk_mode == CALL_SEC_MIC_TALK)
                {
                    ret_val = A1028_msg(algo_ct_call_MIC_param[index].cmd2, algo_ct_call_MIC_param[index].value, 4);
                    if(log_enable) printk( "[Snd_audience_a1028]CMD2 : %x, Value : %x!!!", algo_ct_call_MIC_param[index].cmd2, algo_ct_call_MIC_param[index].value);
                }
                else
                {
                    ret_val = A1028_msg(algo_ct_param[index].cmd2, algo_ct_param[index].value, 4);   
                    if(log_enable) printk( "[Snd_audience_a1028]CMD2 : %x, Value : %x!!!", algo_ct_param[index].cmd2, algo_ct_param[index].value);
                }

                if(ret_val == 1)
                {
                    if(log_enable) printk( "[Snd_audience_a1028][-=AUDIO=-] A1028 Param Write ACK OK_2 !!!\n");

#if 0 //20110624 jhsong : remove retry
                    if(talk_mode == FAR_TALK)
                    {
                        byte read_buf[4];
                        ret_val = A1028_msgCB(4,read_buf);
                        
                        if(ret_val == 1)
                        {
                            memcpy((byte*)&get_value, read_buf, 2);
                            swap_value1 = SWAP16(get_value);
                            	
                            memcpy((byte*)&get_value, &read_buf[2], 2);
                            swap_value2 = SWAP16(get_value);
            
                            if(swap_value1 != A1028_CMD_ALGO_PARAM)
                            {
                                count++;
                                mdelay(20);
            
                                if(count > POLLING_LOOP_COUNT)
                                {
                                    printk( "[Snd_audience_a1028][-=AUDIO=-] A1028 Algo CMD2 ACK NOK !!!\n");
                                    break;
                                }
                            }
                            else
                            {
                                if(log_enable) printk( "[Snd_audience_a1028]Algo CMD2 ACK OK, Val2 = %x, FT ID = %x !!!", swap_value2, algo_ft_param[index].value);
        
                                if(swap_value2 == algo_ft_param[index].value)
                                {
                                    if(log_enable) printk( "[Snd_audience_a1028][-=AUDIO=-] A1028 Algo Value ACK OK !!!\n");
                                    break;
                                }
                                else
                                {
                                    count++;
                                    mdelay(20);
                
                                    if(count > POLLING_LOOP_COUNT)
                                    {
                                        printk( "[Snd_audience_a1028][-=AUDIO=-] A1028 Algo Value ACK NOK !!!\n");
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
                                printk( "[Snd_audience_a1028][-=AUDIO=-] A1028 Algo CMD2 Response ACK NOK !!!\n");
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
                        printk( "[Snd_audience_a1028][-=AUDIO=-] A1028 Param Write NOK_2 !!!\n");
                        break;
                    }
                }
            }
            else
            {
                count++;
                				
                if(count > POLLING_LOOP_COUNT)
                {
                    printk( "[Snd_audience_a1028][-=AUDIO=-] A1028 Param Write NOK_1 !!!\n");	
                    break;
                }            
            }
        }
    
        if(ret_val == 0)
        {
            printk( "[Snd_audience_a1028][-=AUDIO=-] A1028 Algo Param Write Retry !!!\n");
            //break;
        }
        else
            index++;
    
        if(index >= size) //?ÑÏû¨ Size = 17, Index = 16
        {
            break;
        }
    }

    return ret_val;
}

int aud_config_path_vp_cmd_to_1028(a1028_talk_mode talk_mode)
{
    int ret_val = 0;
    byte count = 0;
    byte index = 0;
    u16 size = 0;

    if(a1028_mode != A1028_ACTIVE_MODE)
        return 0;

    size = sizeof(path_vp_on_param);    
    size = size/sizeof(a1028_path_param);
    
    while(1)
    {
        if(path_vp_on_param[index].cmd == 0xFFFF)
            break;

        switch(talk_mode)
        {
            case CLOSE_TALK:
            case CLOSE_1MIC_TALK:
            case FAR_TALK:
            case DV_TALK: // jmlee
            case VL_PRI_MIC_TALK:
            case VL_SEC_MIC_TALK:
            case CALL_PRI_MIC_TALK:
            case CALL_SEC_MIC_TALK:
                ret_val = A1028_msg(path_vp_on_param[index].cmd, path_vp_on_param[index].value, 4);	
                break;

            case CLOSE_VPOFF_TALK:
            case CLOSE_1MIC_VPOFF_TALK:
            case FAR_VPOFF_TALK:
            case DV_VPOFF_TALK: // jmlee
                ret_val = A1028_msg(path_vp_off_param[index].cmd, path_vp_off_param[index].value, 4);	
                break;

            default:
                ret_val = A1028_msg(path_vp_on_param[index].cmd, path_vp_on_param[index].value, 4);
                break;                
        }

        if(ret_val == 1)
        {
            if(log_enable) printk( "[Snd_audience_a1028][-=AUDIO=-] A1028 Path VP Param Write OK !!!\n");		
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
                printk( "[Snd_audience_a1028][-=AUDIO=-] A1028 Path VP Param Write NOK !!!\n");
                break;
            }        
        }
    }

    return ret_val;	
}

int aud_config_path_keytone_gain_cmd_to_1028(void)
{
    int ret_val = 0;
    byte count = 0;
    byte index = 0;
    u16 size = 0;

    if(a1028_mode != A1028_ACTIVE_MODE)
        return 0;

    size = sizeof(path_keytone_gain_param);
	
    size = size/sizeof(a1028_path_param);
    
    while(1)
    {
        if(path_keytone_gain_param[index].cmd == 0xFFFF)
            break;

        ret_val = A1028_msg(path_keytone_gain_param[index].cmd, path_keytone_gain_param[index].value, 4);

        if(ret_val == 1)
        {
            if(log_enable) printk( "[Snd_audience_a1028][-=AUDIO=-] A1028 Path Digital KeyTone Gain Param Write OK !!!\n");		
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
                printk( "[Snd_audience_a1028][-=AUDIO=-] A1028 Path Digital KeyTone Gain Param Write NOK !!!\n");
                break;
            }        
        }
    }

    return ret_val;	
}

int aud_config_path_gain_cmd_to_1028(a1028_talk_mode talk_mode)
{
    int ret_val = 0;
    byte count = 0;
    byte index = 0;
    u16 size = 0;

    if(a1028_mode != A1028_ACTIVE_MODE)
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

        case DV_TALK: // jmlee
            size = sizeof(path_dv_gain_param);
            break;
			
        case DV_VPOFF_TALK: // jmlee
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
            printk( "[Snd_audience_a1028][-=AUDIO=-] A1028 Path Digital Gain Param Size Error !!!\n");			
            break;		
    }
	
    size = size/sizeof(a1028_path_param);
    
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
        else if(talk_mode == DV_TALK) // jmlee
        {
            if(path_dv_gain_param[index].cmd == 0xFFFF)
                break;
        }
        else if(talk_mode == DV_VPOFF_TALK) // jmlee
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
	            ret_val = A1028_msg(path_ct_gain_param[index].cmd, a1028_gain_cal[index].value, 4);
	        else if(talk_mode == CLOSE_VPOFF_TALK)
	            ret_val = A1028_msg(path_ct_VPOFF_gain_param[index].cmd, path_ct_VPOFF_gain_param[index].value, 4);
	        else if(talk_mode == CLOSE_1MIC_TALK)
	            ret_val = A1028_msg(path_ct_1MIC_gain_param[index].cmd, path_ct_1MIC_gain_param[index].value, 4);
	        else if(talk_mode == CLOSE_1MIC_VPOFF_TALK)
	            ret_val = A1028_msg(path_ct_1MIC_VPOFF_gain_param[index].cmd, path_ct_1MIC_VPOFF_gain_param[index].value, 4);
	        else if(talk_mode == FAR_TALK)
	            ret_val = A1028_msg(path_ft_gain_param[index].cmd, a1028_gain_cal[index].value, 4);
	        else if(talk_mode == FAR_VPOFF_TALK)
	            ret_val = A1028_msg(path_ft_VPOFF_gain_param[index].cmd, path_ft_VPOFF_gain_param[index].value, 4);
	        else if(talk_mode == DV_TALK) // jmlee
	            ret_val = A1028_msg(path_dv_gain_param[index].cmd, a1028_gain_cal[index].value, 4);
	        else if(talk_mode == DV_VPOFF_TALK) // jmlee
	            ret_val = A1028_msg(path_dv_VPOFF_gain_param[index].cmd, path_dv_VPOFF_gain_param[index].value, 4);		
	        else if(talk_mode == VL_PRI_MIC_TALK || talk_mode == VL_SEC_MIC_TALK)
	            ret_val = A1028_msg(path_vl_MIC_gain_param[index].cmd, path_vl_MIC_gain_param[index].value, 4);
	        else if(talk_mode == CALL_PRI_MIC_TALK || talk_mode == CALL_SEC_MIC_TALK)
	            ret_val = A1028_msg(path_call_MIC_gain_param[index].cmd, path_call_MIC_gain_param[index].value, 4);
	        else
	            ret_val = A1028_msg(path_ct_gain_param[index].cmd, path_ct_gain_param[index].value, 4);		
	}else
#endif
	{
	        if(talk_mode == CLOSE_TALK)
	            ret_val = A1028_msg(path_ct_gain_param[index].cmd, path_ct_gain_param[index].value, 4);
	        else if(talk_mode == CLOSE_VPOFF_TALK)
	            ret_val = A1028_msg(path_ct_VPOFF_gain_param[index].cmd, path_ct_VPOFF_gain_param[index].value, 4);
	        else if(talk_mode == CLOSE_1MIC_TALK)
	            ret_val = A1028_msg(path_ct_1MIC_gain_param[index].cmd, path_ct_1MIC_gain_param[index].value, 4);
	        else if(talk_mode == CLOSE_1MIC_VPOFF_TALK)
	            ret_val = A1028_msg(path_ct_1MIC_VPOFF_gain_param[index].cmd, path_ct_1MIC_VPOFF_gain_param[index].value, 4);
	        else if(talk_mode == FAR_TALK)
	            ret_val = A1028_msg(path_ft_gain_param[index].cmd, path_ft_gain_param[index].value, 4);
	        else if(talk_mode == FAR_VPOFF_TALK)
	            ret_val = A1028_msg(path_ft_VPOFF_gain_param[index].cmd, path_ft_VPOFF_gain_param[index].value, 4);
	        else if(talk_mode == DV_TALK) // jmlee
	            ret_val = A1028_msg(path_dv_gain_param[index].cmd, path_dv_gain_param[index].value, 4);
	        else if(talk_mode == DV_VPOFF_TALK) // jmlee
	            ret_val = A1028_msg(path_dv_VPOFF_gain_param[index].cmd, path_dv_VPOFF_gain_param[index].value, 4);		
	        else if(talk_mode == VL_PRI_MIC_TALK || talk_mode == VL_SEC_MIC_TALK)
	            ret_val = A1028_msg(path_vl_MIC_gain_param[index].cmd, path_vl_MIC_gain_param[index].value, 4);
	        else if(talk_mode == CALL_PRI_MIC_TALK || talk_mode == CALL_SEC_MIC_TALK)
	            ret_val = A1028_msg(path_call_MIC_gain_param[index].cmd, path_call_MIC_gain_param[index].value, 4);
	        else
	            ret_val = A1028_msg(path_ct_gain_param[index].cmd, path_ct_gain_param[index].value, 4);
	}
        if(log_enable) printk( "[Snd_audience_a1028][-=AUDIO=-] A1028 Path Digital Gain Param !!!\n");

        if(ret_val == 1)
        {
            if(log_enable) printk( "[Snd_audience_a1028][-=AUDIO=-] A1028 Path Digital Gain Param Write OK !!!\n");		
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
                printk( "[Snd_audience_a1028][-=AUDIO=-] A1028 Path Digital Gain Param Write NOK !!!\n");
                break;
            }        
        }
    }

    return ret_val;	
}

int aud_config_path_routing_cmd_to_1028(a1028_talk_mode talk_mode)
{
    int ret_val = 0;
    byte count = 0;
    byte index = 0;
    u16 size = 0;

    if(a1028_mode != A1028_ACTIVE_MODE)
        return 0;

    size = sizeof(path_routing_param);    
    size = size/sizeof(a1028_path_param);
    
    while(1)
    {
        if(path_routing_param[index].cmd == 0xFFFF)
            break;

        switch(talk_mode)
        {
            case CLOSE_TALK:
                ret_val = A1028_msg(path_routing_param[index].cmd, path_routing_param[index].value, 4);	
                break;

            case CLOSE_1MIC_TALK:
            case CLOSE_VPOFF_TALK:
            case CLOSE_1MIC_VPOFF_TALK:
            case FAR_TALK:
            case FAR_VPOFF_TALK:
                ret_val = A1028_msg(path_ct_A_routing_param[index].cmd, path_ct_A_routing_param[index].value, 4);	
                break;
            case DV_TALK: // jmlee	
            case DV_VPOFF_TALK: // jmlee
                ret_val = A1028_msg(path_call_sec_DV_MIC_routing_param[index].cmd, path_call_sec_DV_MIC_routing_param[index].value, 4);	
                break;
            case VL_PRI_MIC_TALK:
                ret_val = A1028_msg(path_vl_pri_MIC_routing_param[index].cmd, path_vl_pri_MIC_routing_param[index].value, 4);	
                break;

            case VL_SEC_MIC_TALK:
                ret_val = A1028_msg(path_vl_sec_MIC_routing_param[index].cmd, path_vl_sec_MIC_routing_param[index].value, 4);	
                break;

            case CALL_PRI_MIC_TALK:
                ret_val = A1028_msg(path_call_pri_MIC_routing_param[index].cmd, path_call_pri_MIC_routing_param[index].value, 4);	
                break;

            case CALL_SEC_MIC_TALK:
                ret_val = A1028_msg(path_call_sec_MIC_routing_param[index].cmd, path_call_sec_MIC_routing_param[index].value, 4);	
                break;

            default:
                ret_val = A1028_msg(path_routing_param[index].cmd, path_routing_param[index].value, 4);
                break;                
        }

        if(ret_val == 1)
        {
            if(log_enable) printk( "[Snd_audience_a1028][-=AUDIO=-] A1028 Path Audio Routing Param Write OK !!!\n");		
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
                printk( "[Snd_audience_a1028][-=AUDIO=-] A1028 Path Audio Routing Param Write NOK !!!\n");
                break;
            }        
        }
    }

    return ret_val;	
}

int aud_config_algo_path_cmd_to_1028(a1028_talk_mode talk_mode)
{
    int ret_val = 0;

    if(log_enable) printk( "[Snd_audience_a1028][-=AUDIO=-] prev_talk_mode = %d, curr_talk_mode = %d \n", prev_talk_mode, talk_mode);	
#if 0 // jmlee temp remove
    if(prev_talk_mode == talk_mode)
    {
        if((!bMVSAquire && !voc_check_is_incall()) && (snd_crnt_path_out.device == SND_DEVICE_HANDSET)
           && (snd_crnt_path_out.method == SND_METHOD_KEY_BEEP))
        {
            aud_config_path_keytone_gain_cmd_to_1028();
            printk( "[Snd_audience_a1028][-=AUDIO=-] prev_talk = %d, curr_talk = %d, A1028 KeyTone Gain OK1", 
                              prev_talk_mode, talk_mode);
        }
    
        return 1;    
    }

//#ifndef PHONE_SIMULATOR // for simulator build 100220
    if(!bMVSAquire && !voc_check_is_incall())
    {
        if(talk_mode != VL_PRI_MIC_TALK && talk_mode != VL_SEC_MIC_TALK
           && curr_mic_mode != NR_ON_MIC_TEST && curr_mic_mode != NR_OFF_MIC_TEST)
        {
            if((voc_check_is_incall() == 0) && (snd_crnt_path_out.device == SND_DEVICE_HANDSET)
               && (snd_crnt_path_out.method == SND_METHOD_KEY_BEEP))
            {
                aud_config_path_keytone_gain_cmd_to_1028();
                printk( "[Snd_audience_a1028][-=AUDIO=-] prev_talk = %d, curr_talk = %d, A1028 KeyTone Gain OK2",
                                  prev_talk_mode, talk_mode, 0);
            }

            return 1;
        }
    }
//#endif // PHONE_SIMULATOR
#endif

#if 0  //20110627 jhsong : remove reset for path change
    ret_val = aud_config_sw_reset_cmd_to_1028(talk_mode);
    if(ret_val == 0)
    {
        printk( "[Snd_audience_a1028][-=AUDIO=-] A1028 Path SW Reset Param Configration NOK !!!\n");
    }
#endif

    ret_val = aud_config_path_vp_cmd_to_1028(talk_mode);
    if(ret_val == 0)
    {
        printk( "[Snd_audience_a1028][-=AUDIO=-] A1028 Path VP Param Configration NOK !!!\n");
    }

    ret_val = aud_config_path_gain_cmd_to_1028(talk_mode);

    if(ret_val == 0)
    {
        printk( "[Snd_audience_a1028][-=AUDIO=-] A1028 Path Digital Gain Param Configration NOK !!!\n");
    }

    ret_val = aud_config_algo_cmd_to_1028(talk_mode);

    if(ret_val == 0)
    {
        printk( "[Snd_audience_a1028][-=AUDIO=-] A1028 Algo Param Configration NOK !!!\n");
    }

    ret_val = aud_config_path_routing_cmd_to_1028(talk_mode);
    if(ret_val == 0)
    {
        printk( "[Snd_audience_a1028][-=AUDIO=-] A1028 Path Audio Routing Param Configration NOK !!!\n");
    }

    prev_talk_mode = talk_mode;

    return ret_val;
}

#if 0
int aud_a1028_amp_switch_control(a1028_talk_mode talk_mode)
{
    int ret_val = 0;

// jmlee add FAB2200 CONTROL + switch gpio control + receiver amp control  ==>>
        switch(talk_mode)
        {
            case CLOSE_TALK:
            case CLOSE_VPOFF_TALK:
                gpio_set_value(GPIO_NR_AMP_EN,1);	
                gpio_set_value(GPIO_NR_OUT_SEL,0);				
                break;
            case FAR_TALK:
            case FAR_VPOFF_TALK:
            case DV_TALK:
            case DV_VPOFF_TALK:				
                gpio_set_value(GPIO_NR_AMP_EN,0);	
                gpio_set_value(GPIO_NR_OUT_SEL,1);	
		  // todo : fab2200 speaker on
                break;

            case CLOSE_1MIC_TALK:
            case CLOSE_1MIC_VPOFF_TALK:
            case VL_PRI_MIC_TALK:
            case VL_SEC_MIC_TALK:
            case CALL_PRI_MIC_TALK:
            case CALL_SEC_MIC_TALK:
            default:
                break;                
        }
    return ret_val;

}
#endif
int aud_a1028_active_mode_sequence(a1028_talk_mode talk_mode)
{
    int ret_val = 0;

    if(bBootSequence == 0)
    {
        //Booting Ï¥àÍ∏∞??Audio ControlÎ∂ÄÎ∂ÑÏù¥ ?ôÏûë???òÍ∏∞??
        //Booting???ïÌôï???¥Î£®?¥Ï?Í∏??ÑÏóê???§Î•∏ ModeÎ°?ÏßÑÏûÖ???†Ïàò ?ÜÎèÑÎ°?Ï°∞Ïπò
        return ret_val;
    }

    if(a1028_mode == A1028_ACTIVE_MODE)
    {
        ret_val = aud_config_algo_path_cmd_to_1028(talk_mode);
#if 0		
	 aud_a1028_amp_switch_control(talk_mode); // jmlee
#endif
        return ret_val;
    }

    if(log_enable) printk( "[Snd_audience_a1028][-=AUDIO=-] A1028 Active Sequence Start !!!\n");	

    ret_val = aud_a1028_wakeup_sequence();

/*================= End of Wake up Sequence ==================*/

    if(ret_val == 1)
    {
        a1028_mode = A1028_ACTIVE_MODE;
		
        aud_config_algo_path_cmd_to_1028(talk_mode);
#if 0
	 aud_a1028_amp_switch_control(talk_mode); // jmlee
#endif
        if(log_enable) printk( "[Snd_audience_a1028][-=AUDIO=-] A1028 Active Sequence OK !!!\n");
    }
    else
    {
        a1028_mode = A1028_MAX_MODE;
        printk( "[Snd_audience_a1028][-=AUDIO=-] A1028 Active Sequence NOK !!!\n");		
    }
	
    if(log_enable) printk( "[Snd_audience_a1028][-=AUDIO=-] A1028 Active Sequence End !!!\n");	

    return ret_val;
}

int aud_a1028_sleep_sequence(void)
{
    int ret_val = 1;
    //static int is_init = 0;
	
    if((a1028_mode == A1028_SLEEP_MODE) || (bBootSequence == 0) 
        /*|| (sleep_status == A1028_SLEEP_START)*/)
    {
        return 1;
    }
#if 0
    gpio_set_value(GPIO_NR_AMP_EN,0);
    gpio_set_value(GPIO_NR_OUT_SEL,1);
#endif
    sleep_status = A1028_SLEEP_START;
    ret_val = aud_sleep_cmd_to_1028();		

    if(ret_val)
    {
#if 1 //[CHD BE10]
        //Wait 120msec
        mdelay(120);
        //mdelay(200);

        prev_talk_mode = MAX_TALK;
        a1028_mode = A1028_SLEEP_MODE;
        sleep_status = A1028_SLEEP_END;
        aud_a1028_clk_off();

        if(log_enable) printk( "[Snd_audience_a1028][-=AUDIO=-] A1028 Sleep Sequence OK !!!\n");
#else
        if(!is_init)
        {
            //Wait 120msec
            mdelay(100);

            a1028_mode = A1028_SLEEP_MODE;
            sleep_status = A1028_SLEEP_END;
            aud_a1028_clk_off();

            printk( "[Snd_audience_a1028][-=AUDIO=-] A1028 First Sleep Sequence OK !!!\n");            		

            //is_init = 1;			
        }
        else
        {
            //Booting?úÍ? ?ÑÎãå WorkingÏ§ëÏóê Ï≤òÎ¶¨
            aud_a1028_set_mode(A1028_SLEEP_MODE);
            aud_a1028_clk_off_sleep_end();
            printk( "[Snd_audience_a1028][-=AUDIO=-] A1028 Sleep Sequence OK !!!\n");			
        }
#endif		
    }
    else
    {
        aud_a1028_clk_off();
        a1028_mode = A1028_MAX_MODE;
        sleep_status = A1028_SLEEP_NONE;
        printk( "[Snd_audience_a1028][-=AUDIO=-] A1028 Sleep Sequence NOK !!!\n");
    }

    return ret_val;
}

#if 0
int aud_a1028_set_mode(a1028_status_type set_a1028_mode)
{
    a1028_mode = set_a1028_mode;
    return 1;
}

void aud_a1028_clk_off_sleep_end(void)
{
    aud_a1028_clk_off();
    sleep_status = A1028_SLEEP_END;
}
#endif

int aud_a1028_boot_sequence(void)
{
    int ret_val = 0;
    byte count = 0;

AGAIN_BOOT:

    a1028_mode = A1028_MAX_MODE;
    bBootSequence = 0;

    //Reset Low
#if 1
#if 0
    gpio_tlmm_config(GPIO_CFG(GPIO_NR_AMP_EN, 0, GPIO_CFG_OUTPUT,	GPIO_CFG_NO_PULL, GPIO_CFG_2MA), GPIO_CFG_ENABLE);  //84
    gpio_tlmm_config(GPIO_CFG(GPIO_NR_OUT_SEL, 0, GPIO_CFG_OUTPUT,	GPIO_CFG_PULL_UP, GPIO_CFG_2MA), GPIO_CFG_ENABLE);  //85
    gpio_set_value(GPIO_NR_AMP_EN,0);
    gpio_set_value(GPIO_NR_OUT_SEL,1);
#endif	
    gpio_tlmm_config(GPIO_CFG(GPIO_NR_WAKEUP_N, 0, GPIO_CFG_OUTPUT,	GPIO_CFG_NO_PULL, GPIO_CFG_2MA), GPIO_CFG_ENABLE);
    gpio_set_value(GPIO_NR_WAKEUP_N,1);

    //gpio_tlmm_config(GPIO_CFG(GPIO_NR_RESET_N,0,GPIO_OUT,GPIO_NO_PULL,GPIO_2MA), GPIO_ENABLE);
    //gpio_tlmm_config(GPIO_NR_RESET_N, GPIO_ENABLE);
    gpio_tlmm_config(GPIO_CFG(GPIO_NR_RESET_N, 0, GPIO_CFG_OUTPUT,	GPIO_CFG_NO_PULL, GPIO_CFG_2MA), GPIO_CFG_ENABLE);
    gpio_set_value(GPIO_NR_RESET_N,0);
#else    
    gpio_tlmm_config(NR_RST_N);
    gpio_out(NR_RST_N, GPIO_LOW_VALUE);
#endif
    //Power On (may be NULL function if power always on)
    
    //Clock On 
    aud_a1028_clk_on();
	    
    //Reset High      
#if 1
    //gpio_tlmm_config(GPIO_CFG(GPIO_NR_RESET_N,0,GPIO_OUT,GPIO_NO_PULL,GPIO_2MA), GPIO_ENABLE);
    //gpio_tlmm_config(GPIO_NR_RESET_N, GPIO_ENABLE);
    gpio_tlmm_config(GPIO_CFG(GPIO_NR_RESET_N, 0, GPIO_CFG_OUTPUT,	GPIO_CFG_NO_PULL, GPIO_CFG_2MA), GPIO_CFG_ENABLE);    
    gpio_set_value(GPIO_NR_RESET_N,1);
#else        
    gpio_tlmm_config(NR_RST_N);
    gpio_out(NR_RST_N, GPIO_HIGH_VALUE);
#endif
    //Wait 50ms
    mdelay(50);
	
    ret_val = aud_boot_cmd_to_1028();

    if(ret_val == 1)
    {
        ret_val = aud_bootload_initiate();

        if(ret_val == 1)
        {
            mdelay(10); //need to wait 10ms before firmware is ready

            ret_val = aud_sync_cmd_to_1028();

           if(ret_val == 1)
           {
               a1028_mode = A1028_BOOTUP_MODE;
               bBootSequence = 1;

               ret_val = aud_a1028_sleep_sequence();
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
	
    if(log_enable) printk( "[Snd_audience_a1028][-=AUDIO=-] A1028 Boot Up OK !!!\n");

    if(ret_val == 0)
    {
        a1028_mode = A1028_MAX_MODE;
        aud_a1028_clk_off();

        printk( "[Snd_audience_a1028][-=AUDIO=-] A1028 Boot Up NOK !!!\n");
    }

    return ret_val;
}

///////////////////////////////// >>>>>
// refered by marimba snddev_ecodec_data  [snddev_data_marimba.c]
// called at [snddev_ecodec.c] ==> snddev_ecodec_open/close function.

/*==========================================================================
** snd_subsystem_pmic_vreg_l2_on
**=========================================================================*/
static struct vreg *vreg_s4_audience_system;  // s4 vreg control (RF_2.2V)
static struct vreg *vreg_s3_audience_system;  //20110513 jhsong : s3 vreg control (MSMP_1.8V)

int audience_pmic_vreg_switch_on(void)
{
	int rc;

	vreg_s4_audience_system = vreg_get(NULL, "s4");  // s4 vreg.c
	vreg_s3_audience_system = vreg_get(NULL, "s3");  // s3 vreg.c

	rc = vreg_set_level(vreg_s4_audience_system, 2200);

	if (rc){
		printk(KERN_ERR "%s: vreg_set_level failed(%s) = %d \n",
					__func__, "s4", rc);
		return rc;
	}

	rc = vreg_enable(vreg_s4_audience_system);
	if (rc){
		printk(KERN_ERR "%s: vreg_enable failed(%s) = %d \n",
					__func__, "s4", rc);
		return rc;
	}
	if(log_enable) printk("[fab2200] audience_pmic_vreg_switch_on rc:%d\n", rc);

	rc = vreg_set_level(vreg_s3_audience_system, 1800);

	if (rc){
		printk(KERN_ERR "%s: vreg_set_level failed(%s) = %d \n",
					__func__, "s3", rc);
		return rc;
	}

	rc = vreg_enable(vreg_s3_audience_system);
	if (rc){
		printk(KERN_ERR "%s: vreg_enable failed(%s) = %d \n",
					__func__, "s3", rc);
		return rc;
	}

#ifdef FEATURE_SKY_SND_EXTAMPCTRL  //20110519 jhsong : sub chip cal tool by youngsoo Lim
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

	if (IS_ERR(vreg_s4_audience_system)) {
		printk("%s: vreg_get(%s) failed (%ld)\n",
		__func__, "s4", PTR_ERR(vreg_s4_audience_system));
		return -1;
	}

	rc = vreg_disable(vreg_s4_audience_system);

	if (rc){
		printk("%s: vreg_disable(%s) failed (%d)\n", __func__, "s4", rc);
		return rc;
	}

#ifdef FEATURE_SKY_SND_EXTAMPCTRL  //20110519 jhsong : sub chip cal tool by youngsoo Lim
	ext_audience_pth_on = 0;
#endif

	vreg_put(vreg_s4_audience_system);

	if(log_enable) printk("[[Snd_audience_a1028]] audience_pmic_vreg_switch_off rc:%d \n", rc);

	if (IS_ERR(vreg_s3_audience_system)) {
		printk("%s: vreg_get(%s) failed (%ld)\n",
		__func__, "s3", PTR_ERR(vreg_s4_audience_system));
		return -1;
	}

	rc = vreg_disable(vreg_s3_audience_system);

	if (rc){
		printk("%s: vreg_disable(%s) failed (%d)\n", __func__, "s3", rc);
		return rc;
	}

	vreg_put(vreg_s3_audience_system);

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
				printk(KERN_ERR "%s: gpio_tlmm_config(%#x)=%d\n",
				__func__, audience_config_gpio_on[pin], rc);
				return -EIO;
			}
		}
	} else {
		for (pin = 0; pin < ARRAY_SIZE(audience_config_gpio_off); pin++) {
			rc = gpio_tlmm_config(audience_config_gpio_off[pin], GPIO_CFG_ENABLE);
			if (rc) {
				printk(KERN_ERR "%s: gpio_tlmm_config(%#x)=%d\n",
				__func__, audience_config_gpio_off[pin], rc);
				return -EIO;
			}
		}
	}
	return 0;
}

void audience_a1028_ct_on(void)
{
     printk("[Snd_audience_a1028] #@#@#@#@#@#@#@#@#  [[ CLOSE_TALK ]] BY ECODEC  \n");
     audience_pmic_vreg_switch_on();   //20110301 jhsong : rf 2.2VReg Control
     Audience_Config_GPIO_onoff(1); // Aux Pcm(about PCM Block <--> BT)
     snd_extamp_api_audience_SetVolume_ById(EXTAMP_NR_ON_CT);
     aud_a1028_active_mode_sequence(CLOSE_TALK);
     msm_snddev_mic_sel_a1028();
     msm_snddev_nr_out_sel_A1028Reciever_nr_receiver_amp_on();
}

void audience_a1028_ct_2mic_on(void)
{
     printk("[Snd_audience_a1028] #@#@#@#@#@#@#@#@#  [[ 2mic only ]] BY ECODEC  \n");
     audience_pmic_vreg_switch_on();   //20110301 jhsong : rf 2.2VReg Control
     Audience_Config_GPIO_onoff(1); // Aux Pcm(about PCM Block <--> BT)
     aud_a1028_active_mode_sequence(VL_SEC_MIC_TALK);
     msm_snddev_mic_sel_a1028();
     msm_snddev_nr_out_sel_A1028Reciever_nr_receiver_amp_on();
}

// not used
void audience_a1028_ct_vpoff_on(void)
{
     printk("[Snd_audience_a1028] #@#@#@#@#@#@#@#@#  [[ CLOSE_VPOFF_TALK ]] BY ECODEC  \n");
     audience_pmic_vreg_switch_on();   //20110301 jhsong : rf 2.2VReg Control
     Audience_Config_GPIO_onoff(0); // Aux Pcm(about PCM Block <--> BT)
     aud_a1028_active_mode_sequence(CLOSE_VPOFF_TALK);
     msm_snddev_mic_sel_a1028();	 
}

void audience_a1028_ft_on(void)
{
     printk("[Snd_audience_a1028] #@#@#@#@#@#@#@#@#  [[ FAR_TALK ]] BY ECODEC  \n");
     audience_pmic_vreg_switch_on();   //20110301 jhsong : rf 2.2VReg Control
     Audience_Config_GPIO_onoff(1); // Aux Pcm(about PCM Block <--> BT)
     snd_extamp_api_audience_SetVolume_ById(EXTAMP_NR_ON_FT);
     aud_a1028_active_mode_sequence(FAR_TALK);
     msm_snddev_mic_sel_a1028();
     msm_snddev_nr_receiver_amp_off();
     msm_snddev_nr_out_sel_qtrReceiver_or_A1028Speaker();
     // FAB2200 ON Insert !!
     snd_subsystem_nr_ft_poweron();  
}

// not used
void audience_a1028_ft_vpoff_on(void)
{
     printk("[Snd_audience_a1028] #@#@#@#@#@#@#@#@#  [[ FAR_VPOFF_TALK ]] BY ECODEC  \n");
     audience_pmic_vreg_switch_on();   //20110301 jhsong : rf 2.2VReg Control
     Audience_Config_GPIO_onoff(0); // Aux Pcm(about PCM Block <--> BT)	 
     aud_a1028_active_mode_sequence(FAR_VPOFF_TALK);
     msm_snddev_mic_sel_a1028();	 
}

void audience_a1028_dv_on(void)
{
     printk("[Snd_audience_a1028] #@#@#@#@#@#@#@#@#  [[ DV_TALK ]] BY ECODEC  \n");
     audience_pmic_vreg_switch_on();   //20110301 jhsong : rf 2.2VReg Control
     Audience_Config_GPIO_onoff(1); // Aux Pcm(about PCM Block <--> BT)	 
     snd_extamp_api_audience_SetVolume_ById(EXTAMP_NR_ON_DV);
     aud_a1028_active_mode_sequence(DV_TALK);
     msm_snddev_mic_sel_a1028();
     msm_snddev_nr_receiver_amp_off();
     msm_snddev_nr_out_sel_qtrReceiver_or_A1028Speaker();
     // FAB2200 ON Insert !!
     snd_subsystem_nr_dv_poweron(); 
}

// not used
void audience_a1028_dv_vpoff_on(void)
{
     printk("[Snd_audience_a1028] #@#@#@#@#@#@#@#@#  [[ DV_VPOFF_TALK ]] BY ECODEC  \n");
     audience_pmic_vreg_switch_on();   //20110301 jhsong : rf 2.2VReg Control
     Audience_Config_GPIO_onoff(0); // Aux Pcm(about PCM Block <--> BT)	 
     aud_a1028_active_mode_sequence(DV_VPOFF_TALK);
     msm_snddev_mic_sel_a1028();	 
}

void audience_a1028_off(void)
{
     printk("[Snd_audience_a1028] #@#@#@#@#@#@#@#@#  [[ AUDIENCE OFF ]] BY ECODEC  \n");
     // FAB2200 OFF Insert !!
     snd_subsystem_sp_powerdown(); // temp setting
     Audience_Config_GPIO_onoff(1); // Aux Pcm(about PCM Block <--> BT)	   //20110614 jhsong
     aud_a1028_sleep_sequence();
     msm_snddev_nr_receiver_amp_off();
     audience_pmic_vreg_switch_off();	   //20110301 jhsong : rf 2.2VReg Control
}

void audience_a1028_ct_off(void)
{
     printk("[Snd_audience_a1028] #@#@#@#@#@#@#@#@#  [[ AUDIENCE NR CT OFF ]] BY ECODEC  \n");
     Audience_Config_GPIO_onoff(1); // Aux Pcm(about PCM Block <--> BT)	   //20110614 jhsong
     aud_a1028_sleep_sequence();
     msm_snddev_nr_receiver_amp_off();
     audience_pmic_vreg_switch_off();	  //20110301 jhsong : rf 2.2VReg Control
}
///////////////////////////////// <<<<<


static int audience_a1028_probe(struct i2c_client *client, const struct i2c_device_id *id)
{
 	int rc = 0;
	
       audience_pmic_vreg_switch_on(); // switch power up.
	   
	if (!i2c_check_functionality(client->adapter, I2C_FUNC_I2C)) {
		audience_a1028_i2c_client = NULL;
		rc = -1;
	}
	else {
		audience_a1028_i2c_client = client;
		
#if 0  // jmlee for test code i2c device check for AUDIENCE DEBUG TOOL
    gpio_tlmm_config(GPIO_CFG(67, 0, GPIO_CFG_INPUT,	GPIO_CFG_NO_PULL, GPIO_CFG_2MA), GPIO_CFG_ENABLE);    
    gpio_tlmm_config(GPIO_CFG(65, 0, GPIO_CFG_INPUT,	GPIO_CFG_NO_PULL, GPIO_CFG_2MA), GPIO_CFG_ENABLE);    
#endif

		aud_a1028_boot_sequence(); 
	}

	audience_pmic_vreg_switch_off();    //20110301 jhsong : rf 2.2VReg Control
		
	if(log_enable) printk("[Snd_audience_a1028] audience_a1028_probe %d \n",rc);
#if 0
      mdelay(5);
	//printk("[Snd_audience_a1028] audience_a1028_probe TEST : FIRST ACK TEST IN SLEEP ~~!!  ==>> \n");
       //aud_sync_cmd_to_1028();

	printk("[Snd_audience_a1028] audience_a1028_probe TEST : CLOSE_TALK Active ~~!!  ==>> \n");
       aud_a1028_active_mode_sequence(CLOSE_TALK);
       //aud_sync_cmd_to_1028();
       printk( "[Snd_audience_a1028][-=AUDIO=-] ============================================ A1028 SYNC CHECK 4 ]]]]>>> !!!\n");	
       printk( "[Snd_audience_a1028][-=AUDIO=-] ====================================================================\n");	
       printk( "[Snd_audience_a1028][-=AUDIO=-] ====================================================================\n");	
       printk( "[Snd_audience_a1028][-=AUDIO=-] ====================================================================\n");	
       printk( "[Snd_audience_a1028][-=AUDIO=-] ====================================================================\n");	

      mdelay(5);
	printk("[Snd_audience_a1028] audience_a1028_probe TEST : CLOSE_TALK Sleep ~~!!  ==>> \n");
       aud_a1028_sleep_sequence();

      mdelay(5);
	printk("[Snd_audience_a1028] audience_a1028_probe TEST : CLOSE_VPOFF_TALK Active ~~!!  ==>> \n");
       aud_a1028_active_mode_sequence(CLOSE_VPOFF_TALK);
       //aud_sync_cmd_to_1028();
       printk( "[Snd_audience_a1028][-=AUDIO=-] ============================================ A1028 SYNC CHECK 6 ]]]]>>> !!!\n");	
       printk( "[Snd_audience_a1028][-=AUDIO=-] ====================================================================\n");	
       printk( "[Snd_audience_a1028][-=AUDIO=-] ====================================================================\n");	
       printk( "[Snd_audience_a1028][-=AUDIO=-] ====================================================================\n");	
       printk( "[Snd_audience_a1028][-=AUDIO=-] ====================================================================\n");	

      mdelay(5);	   
	printk("[Snd_audience_a1028] audience_a1028_probe TEST : CLOSE_VPOFF_TALK Sleep ~~!!  ==>> \n");
       aud_a1028_sleep_sequence();

	printk("[Snd_audience_a1028] audience_a1028_probe TEST : FAR_TALK Active ~~!!  ==>> \n");
       aud_a1028_active_mode_sequence(FAR_TALK);
       //aud_sync_cmd_to_1028();
       printk( "[Snd_audience_a1028][-=AUDIO=-] ============================================ A1028 SYNC CHECK 7 ]]]]>>> !!!\n");	
       printk( "[Snd_audience_a1028][-=AUDIO=-] ====================================================================\n");	
       printk( "[Snd_audience_a1028][-=AUDIO=-] ====================================================================\n");	
       printk( "[Snd_audience_a1028][-=AUDIO=-] ====================================================================\n");	
       printk( "[Snd_audience_a1028][-=AUDIO=-] ====================================================================\n");		   

	printk("[Snd_audience_a1028] audience_a1028_probe TEST : FAR_TALK Sleep ~~!!  ==>> \n");
       aud_a1028_sleep_sequence();

	printk("[Snd_audience_a1028] audience_a1028_probe TEST : FAR_VPOFF_TALK Active ~~!!  ==>> \n");
       aud_a1028_active_mode_sequence(FAR_VPOFF_TALK);
       //aud_sync_cmd_to_1028();
       printk( "[Snd_audience_a1028][-=AUDIO=-] ============================================ A1028 SYNC CHECK 8 ]]]]>>> !!!\n");	
       printk( "[Snd_audience_a1028][-=AUDIO=-] ====================================================================\n");	
       printk( "[Snd_audience_a1028][-=AUDIO=-] ====================================================================\n");	
       printk( "[Snd_audience_a1028][-=AUDIO=-] ====================================================================\n");	
       printk( "[Snd_audience_a1028][-=AUDIO=-] ====================================================================\n");		   
	   
	printk("[Snd_audience_a1028] audience_a1028_probe TEST : FAR_VPOFF_TALK Sleep ~~!!  ==>> \n");
       aud_a1028_sleep_sequence();
	   
	printk("[Snd_audience_a1028] audience_a1028_probe TEST : DV_TALK Active ~~!!  ==>> \n");
       aud_a1028_active_mode_sequence(DV_TALK);
       //aud_sync_cmd_to_1028();
       printk( "[Snd_audience_a1028][-=AUDIO=-] ============================================ A1028 SYNC CHECK 9 ]]]]>>> !!!\n");	
       printk( "[Snd_audience_a1028][-=AUDIO=-] ====================================================================\n");	
       printk( "[Snd_audience_a1028][-=AUDIO=-] ====================================================================\n");	
       printk( "[Snd_audience_a1028][-=AUDIO=-] ====================================================================\n");	
       printk( "[Snd_audience_a1028][-=AUDIO=-] ====================================================================\n");		   
	   	   
	printk("[Snd_audience_a1028] audience_a1028_probe TEST : DV_TALK Sleep ~~!!  ==>> \n");
       aud_a1028_sleep_sequence();

	printk("[Snd_audience_a1028] audience_a1028_probe TEST : DV_VPOFF_TALK Active ~~!!  ==>> \n");
       aud_a1028_active_mode_sequence(DV_VPOFF_TALK);
       //aud_sync_cmd_to_1028();
       printk( "[Snd_audience_a1028][-=AUDIO=-] ============================================ A1028 SYNC CHECK 10 ]]]]>>> !!!\n");	
       printk( "[Snd_audience_a1028][-=AUDIO=-] ====================================================================\n");	
       printk( "[Snd_audience_a1028][-=AUDIO=-] ====================================================================\n");	
       printk( "[Snd_audience_a1028][-=AUDIO=-] ====================================================================\n");	
       printk( "[Snd_audience_a1028][-=AUDIO=-] ====================================================================\n");		   

       printk( "[Snd_audience_a1028][-=AUDIO=-] JMLEE IN AWAKE ACK TEST START ***********************************************************\n");		   	   
       printk( "[Snd_audience_a1028][-=AUDIO=-] JMLEE IN AWAKE ACK TEST START ***********************************************************\n");		   	   
#if 1 // jmlee
                A1028_msgCB_Chip_AckDataFlush(); // for msg CB 
#endif                
       aud_sync_cmd_to_1028();
       aud_sync_cmd_to_1028();
	mdelay(5);
	mdelay(5);
	mdelay(5);
	mdelay(5);	
       aud_sync_cmd_to_1028();
       aud_sync_cmd_to_1028();	
       printk( "[Snd_audience_a1028][-=AUDIO=-] JMLEE IN AWAKE ACK TEST END  ***********************************************************\n");		   	   
       printk( "[Snd_audience_a1028][-=AUDIO=-] JMLEE IN AWAKE ACK TEST END  ***********************************************************\n");		   	   
	   
	   
	printk("[Snd_audience_a1028] audience_a1028_probe TEST : DV_VPOFF_TALK Sleep ~~!!  ==>> \n");
       aud_a1028_sleep_sequence();

#endif
	return rc;
}

static int __exit audience_a1028_remove(struct i2c_client *client)
{
	int rc = 0;
	audience_a1028_i2c_client = NULL;
	return rc;
}

/*static*/ int audience_a1028_suspend(struct i2c_client *client, pm_message_t mesg)
{
	if(log_enable) printk("[Snd_audience_a1028] audience_a1028_suspend  \n");
	return 0;
}

static int audience_a1028_resume(struct i2c_client *client)
{
	if(log_enable) printk("[Snd_audience_a1028] audience_a1028_resume  \n");

	return 0;
}

static void audience_a1028_shutdown(struct i2c_client *client)
{
	if(log_enable) printk("[Snd_audience_a1028] audience_a1028_shutdown \n");

}

static const struct i2c_device_id audience_a1028_id[] = {
	{ "audience-a1028", 0},
};

static struct i2c_driver audience_a1028_driver = {
	.id_table = audience_a1028_id,
	.probe  = audience_a1028_probe,
	.remove = __exit_p(audience_a1028_remove),
	.suspend = audience_a1028_suspend,
	.resume = audience_a1028_resume,
	.shutdown = audience_a1028_shutdown,
	.driver = {
		.name = "audience-a1028",
	},
};

void snd_audience_a1028_api_Init(void)
{
	int result = 0;

	if(log_enable) printk( "[Snd_audience_a1028] snd_audience_a1028_api_Init()\n");
	result = i2c_add_driver(&audience_a1028_driver);
	if(result){
	printk(KERN_ERR "[Snd_audience_a1028] snd_audience_a1028_api_Init  Fail\n");
	}
}

void snd_audience_a1028_api_DeInit(void)
{
	//misc_deregister(&miscdev);
	i2c_del_driver(&audience_a1028_driver);
}




