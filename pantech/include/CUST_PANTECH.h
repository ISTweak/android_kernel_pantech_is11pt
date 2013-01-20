#ifndef CUST_PANTECH_H
#define CUST_PANTECH_H

#include "BOARD_REV.h"

#ifdef __MODEMBUILD__
/*
    ONLY MODEM BUILD FEATURE
*/

/*******************************************************************************
  Pantech MODEM System
*******************************************************************************/

/*
      !!!!!!!!!!!!!!!!!! MUST BE DEFINED AS FOLLOWS (ONLY MODEM)!!!!!!!!!!!!!!!!!!
      FEATURE_{COMPANY_NAME}_{FUNCTION_NAME}_{ACTION_NAME}
      ex) PMIC function.

      #define FEATURE_PANTECH_PMIC
*/


/*******************************************************************************
  PMIC
*******************************************************************************/
#if defined(T_MINI) || defined(T_J_MASAI_E) || defined(T_LASER2)
#define FEATURE_PANTECH_PMIC
#endif // #if defined(T_MINI) || defined(T_J_MASAI_E) || defined(T_LASER2)

#if defined(FEATURE_PANTECH_PMIC)
#define FEATURE_PANTECH_PMIC_DEBUG
#define FEATURE_PANTECH_PMIC_OUTPUT_REGULATOR
#define FEATURE_PANTECH_PMIC_INPUT_POWER
#define FEATURE_PANTECH_PMIC_GPIO_KEYPAD
#define FEATURE_PANTECH_PMIC_GENERAL_HOUSEKEEPING
#define FEATURE_PANTECH_PMIC_RTC
#define FEATURE_PANTECH_PMIC_AUTO_PUSH_POWER_ON_OFF
#define FEATURE_PANTECH_PMIC_FUEL_GAUGE
#define FEATURE_PANTECH_PMIC_HW_RESET_DETECT
#endif

#if defined(T_MINI) || defined(T_J_MASAI_E) || defined(T_LASER2)
#define FEATURE_PANTECH_ERR_CRASH_LOGGING
#endif
#ifdef FEATURE_PANTECH_ERR_CRASH_LOGGING
#define FEATURE_SKY_ASSERT_LOG
#define FEATURE_SKY_ERROR_LOGGING_EXTENDED
#endif /* FEATURE_PANTECH_ERR_CRASH_LOGGING */

/*******************************************************************************
**  UIM
*******************************************************************************/
#define FEATURE_SKY_UIM_TESTMENU

#if defined(T_MINI) || defined(T_J_MASAI_E) || defined(T_LASER2)
#define FEATURE_PANTECH_WIFI	// 20110511 khlee_wifi for wifi gpio settings
#endif

#endif /* __MODEMBUILD__ */

#define CONFIG_PANTECH_FB_24BPP_RGB888 
#define SKY_LCD_LOADBAR_ANDROID 
#undef F_SKYDISP_NO_USE_BACKLIGHT_EARLYSUSPEND
#define F_SKYDISP_ADDITIONAL_MDP_LCDC_DMA_PATCH
/* 20100916, kkcho, MDP초기화시 noise화면제거를 위해 lcd를 off한다. */
#if defined (T_MINI) || defined(T_J_MASAI_E)
#define F_SKYDISP_REMOVE_NOISE_MDP_INIT
#endif
#if !defined(__KERNELBUILD__) && !defined(__MODEMBUILD__)
/*
      !!!!!!!!!!!!!!!!!! MUST BE DEFINED AS FOLLOWS (ANDROID)!!!!!!!!!!!!!!!!!!
      CONFIG_{COMPANY_NAME}_{FUNCTION_NAME}_{ACTION_NAME}
      ex) PMIC function.
      #define CONFIG_PANTECH_PMIC

      for using BOTH (android & kernel) definition, please read engineer note about chapter 5 Arm Linux Kernel.

      IF YOU ADD FEATURE IN KERNEL , YOU CHECK THE RELEASE ENGINNER NOTE

      __KERNELBUILD__ :  for avoid redefined , this is predefined name in kernel makefile.

*/

#define CONFIG_PANTECH_SDCARD
#define CONFIG_PANTECH_CAMREA
#define CONFIG_PANTECH_PMIC8058
#if defined(CONFIG_PANTECH_PMIC8058)
#define CONFIG_PANTECH_PMIC_INPUT_POWER_A
#define CONFIG_PANTECH_PMIC_POWER_ON_REASON_A
#endif
#define CONFIG_PANTECH_BT
#ifndef CONFIG_PANTECH_SKY_FACTORY_COMMAND /* msy_sky_factory_command */
#define CONFIG_PANTECH_SKY_FACTORY_COMMAND
#endif
#define CONFIG_PANTECH_WIFI_TEST_MODE // 20110302 khlee_wifi for wifi test mode
#define CONFIG_PANTECH_WIFI_FC // 20110303 khlee_wifi for wifi factory command
#define CONFIG_PANTECH_WIFI_ADDR_BACKUP // 20110316 khlee_wifi for wifi mac address backup
#define FEATURE_PANTECH_VOLUME_CTL

/* msy_usb */
#if defined(CONFIG_PANTECH_HSUSB_OBEX)
#define CONFIG_PANTECH_HSUSB_TEST
#endif
#define CONFIG_PANTECH_MSG_ONOFF

#if defined (T_MINI) || defined (T_J_MASAI_E) || defined(T_LASER2)
#define CONFIG_PANTECH_ASR_PATH
#define CONFIG_PANTECH_AUDIO_FAB2200  //jhsong
#define CONFIG_PANTECH_3POLE_HEADSET  //20110407 jhsong
#define CONFIG_PANTECH_CAMERA_SHUTTER  //20110407 jhsong
#endif

/* jmlee add */
#ifdef T_MINI
#define CONFIG_PANTECH_MINI_AUDIO
#define CONFIG_PANTECH_MINI_AUDIO_AUDIENCE1028  //jmlee
#define CONFIG_PANTECH_EMERSYS_EQ
/* #define CONFIG_PANTECH_MINI_AUDIO_HDRADIO  */
#define CONFIG_PANTECH_ERR_CRASH_LOGGING
#define CONFIG_PANTECH_MINI_BT_GROUPING  //20110421 jhsong
#define CONFIG_PANTECH_AUDIO_POLICY
#endif

/* jmlee add */
/* dictionary.com site voice recognizing err */
#define CONFIG_PANTECH_MINI_DICTIONARYDOTCOM
/* #define CONFIG_PANTECH_MINI_DICTIONARYDOTCOM_PANTECHMODIFY */

#ifdef T_J_MASAI_E
#define CONFIG_PANTECH_MASAI_E_AUDIO  //20110504 jhsong
#define CONFIG_PANTECH_ERR_CRASH_LOGGING
#define CONFIG_PANTECH_MASAI_E_SEPERATE_CALL_MEDIA_PATH  //20110419 jhsong
#define CONFIG_PANTECH_EMERSYS_EQ
#define CONFIG_PANTECH_AUDIO_POLICY
#define CONFIG_PANTECH_MIRACHJ_RXMUTE
#define CONFIG_PANTECH_TEST_CODE_PATH  //20110711 jhsong
#define CONFIG_PANTECH_EMERSYS_EQ_TRACK_BASE  //20110713 jhsong
#define CONFIG_PANTECH_MIRACHJ_VOCREC_INCALL_ERR //20110727 YSLEEM
#endif

#ifdef T_LASER2
#define CONFIG_PANTECH_ERR_CRASH_LOGGING
#define CONFIG_PANTECH_LASER2_AUDIENCE2020
#define CONFIG_PANTECH_EMERSYS_EQ
#define CONFIG_PANTECH_MINI_BT_GROUPING
#define CONFIG_PANTECH_LASER2_BT_GROUPING
#define CONFIG_PANTECH_AUDIO_POLICY
#define CONFIG_PANTECH_AUDIO_SUBCHIP_CAL_TOOL
#endif

/* #ifdef T_MINI, gieil, 20110608 */
#if defined (T_MINI) || defined (T_LASER2)
#define FEATURE_FOTA_FS_REDBEND
#endif

/* PS2.P12095 SENSOR UTIL */
#define FEATURE_PANTECH_SENSOR_UTIL

#define FEATURE_SKY_PATTERN_RESET //ps1 team shs : 2011-07-11

#endif /* __KERNELBUILD__ */

/*******************************************************************************
  Pantech CAMERA System
*******************************************************************************/

/*SYS_LJH add 20110208 */
/* Global features for SKY camera framework. */
#include "CUST_SKYCAM.h"

/*******************************************************************************
  Pantech MMP(Codec) System
*******************************************************************************/

/* P11157 add 20110303 */
/* Global features for PANTECH MMP(Codec). */
#include "CUST_PANTECH_MMP.h"

/*[LSH] add 20110315*/
/* Global features for UIM for KDDI */
#include "cust_ps_uim.h"

/* kjkong 20110707 dynamic surfaceview for video player. */
#define FEATURE_VIDEO_DYNAMIC_SURFACEVIEW
#undef FEATURE_PANTECH_MEDIAFRAMEWORK_DEBUG


/*[PS10Team]20110713 */
#include "cust_ps_ril.h"

#endif/* CUST_PANTECH_H */


