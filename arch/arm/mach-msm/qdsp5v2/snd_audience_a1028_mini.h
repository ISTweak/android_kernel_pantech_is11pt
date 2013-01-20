#ifndef SNDAUDIENCE_A1028_H
#define SNDAUDIENCE_A1028_H
/************************************************************************************************
** Includes
*************************************************************************************************/
#include <linux/kernel.h>
/************************************************************************************************
** Definition
*************************************************************************************************/

#define CONFIG_SND_AUDIENCE_A1028

#define A1028_SLAVE_ADDR 0x3E  // b' 0111 110
#define A1028_DEVICE_ADDRESS_ID (A1028_SLAVE_ADDR<<1)

#define SWAP16( sw )        ((((sw)>>8) & 0x00ff) | (((sw)<<8) & 0xff00))
#define SWAP32( sw )        ((((sw)>>24) & 0x000000ff) | (((sw)>>8) & 0x0000ff00) | \
                            (((sw)<<24) & 0xff000000) | (((sw)<<8) & 0x00ff0000))

typedef enum
{
    CLOSE_TALK,
    CLOSE_VPOFF_TALK,
    CLOSE_1MIC_TALK,
    CLOSE_1MIC_VPOFF_TALK,    
    FAR_TALK,
    FAR_VPOFF_TALK,
    DV_TALK,
    DV_VPOFF_TALK,
    VL_PRI_MIC_TALK,
    VL_SEC_MIC_TALK,
    CALL_PRI_MIC_TALK,
    CALL_SEC_MIC_TALK,
    MAX_TALK,
} a1028_talk_mode;

typedef enum
{
    PRI_MIC_ON,
    SEC_MIC_ON,
    NR_ON_MIC_TEST,
    NR_OFF_MIC_TEST,
    BOTH_MIC_ON,
    MAX_MIC,
} a1028_mic_mode;

typedef enum
{	
    A1028_BOOTUP_MODE = 0,
    A1028_SLEEP_MODE,
    A1028_WAKEUP_MODE,
    A1028_ACTIVE_MODE,
    A1028_MAX_MODE
} a1028_status_type;

typedef enum
{
    A1028_SLEEP_START = 0,
    A1028_SLEEP_END,
    A1028_SLEEP_NONE,
} a1028_sleep_status;

typedef enum
{	
    A1028_GPIO_NORMAL_MODE = 0,
    A1028_GPIO_AUXPCM_MODE,
    A1028_GPIO_TRISTATE_MODE,
    A1028_GPIO_TEST1_MODE,
    A1028_GPIO_MAX_MODE
} a1028_gpio_status_type;

int aud_a1028_active_mode_sequence(a1028_talk_mode talk_mode);
int aud_a1028_sleep_sequence(void);

///////////////////////////////// >>>>>
// refered by marimba snddev_ecodec_data  [snddev_data_marimba.c]
// called at [snddev_ecodec.c] ==> snddev_ecodec_open/close function.
void audience_a1028_ct_on(void);
void audience_a1028_ct_2mic_on(void);
void audience_a1028_ct_vpoff_on(void);
void audience_a1028_ft_on(void);
void audience_a1028_ft_vpoff_on(void);
void audience_a1028_dv_on(void);
void audience_a1028_dv_vpoff_on(void);
void audience_a1028_off(void);
void audience_a1028_ct_off(void);
///////////////////////////////// <<<<<

uint32_t ext_audience_get_device(void);  //20110523 jhsong
int ext_audience_get_path_on(void);  //20110523 jhsong

extern void snd_audience_a1028_api_Init(void);  // jmlee
extern void snd_audience_a1028_api_DeInit(void); // jmlee
#endif
