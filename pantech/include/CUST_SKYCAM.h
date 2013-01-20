#ifndef __CUST_SKYCAM_H__
#define __CUST_SKYCAM_H__

/*
Modified files which have "SKYCAM" tag:
	android/build/core/Makefile
	android/kernel/arch/arm/configs/qsd8650_defconfig
	android/kernel/drivers/media/video/msm/Kconfig
	android/kernel/drivers/media/video/msm/Makefile
	android/vendor/qcom/android-open/libcamera2/Android.mk
	android/vendor/qcom-proprietary/mm-camera/Android.mk
	android/vendor/qcom-proprietary/mm-camera/camera.mk
	android/vendor/qcom-proprietary/mm-camera/targets/Android.mk

Modified files which don't have "SKYCAM" tag:
	android/kernel/drivers/media/video/msm/mt9p111.h
	android/kernel/drivers/media/video/msm/mt9p111_reg.c
	android/vendor/qcom-proprietary/mm-camera/targets/tgtcommon/sensor/mt9p111/mt9p111.h
	android/kernel/drivers/media/video/msm/yacbac1sddas.h
	android/kernel/drivers/media/video/msm/yacbac1sddas_reg.c
	android/vendor/qcom-proprietary/mm-camera/targets/tgtcommon/sensor/yacbac1sddas/yacbac1sddas.h

Local features:
	CONFIG_MSM_CAMERA_DEBUG (MSM_CAMERA_DEBUG)
	CONFIG_SKYCAM_MT9P111 (SKYCAM_MT9P111)
	F_SKYCAM_SENSOR_MT9P111
	CONFIG_SKYCAM_YACBAC1SDDAS (SKYCAM_YACBAC1SDDAS)
	F_SKYCAM_SENSOR_YACBAC1SDDAS
	F_SKYCAM_LOG_VERBOSE (enable LOGV/LOGD/LOGI in userspace)
	F_SKYCAM_LOG_CDBG (enable CDBG in userspace)
	F_SKYCAM_LOG_OEM (enable SKYCDBG/SKYCERR in userspace)

How to turn off all camera logs (kernel/userspace):
	android/kernel/arch/arm/config/qsd8650-defconfig
		- CONFIG_MSM_CAMERA_DEBUG -> no (default)
	Disable F_SKYCAM_LOG_PRINTK (default enabled)
	Find all F_SKYCAM_LOG_OEM, 
		- comment out (default)
	Find all F_SKYCAM_LOG_CDBG
		- comment out (default)
	Find all F_SKYCAM_LOG_VERBOSE
		- comment out (default)

How to exclude module "MT9P111":
	android/kernel/arch/arm/config/msm7630_defconfig
		- disable CONFIG_MT9P111
	android/vendor/qcom-proprietary/mm-camera/Android.mk
	android/vendor/qcom-proprietary/mm-camera/camera.mk
		- CONFIG_MT9P111=no (default yes)

How to exclude module "OLAWORKS":



(2)  Ä«¸Þ¶ó °ü·Ã ¸ðµç kernel/userspace/android ·Î±×¸¦ Á¦°Å

kernel/arch/arm/config/qsd8650-perf_defconfig ¸¦ ¼öÁ¤ÇÑ´Ù.

	# CONFIG_MSM_CAMERA_DEBUG is not set (default)

CUST_SKYCAM.h ¿¡¼­ F_SKYCAM_LOG_PRINTK À» #undef ÇÑ´Ù. 

	#define F_SKYCAM_LOG_PRINTK (default)

¸ðµç ¼Ò½º ÆÄÀÏ¿¡¼­ F_SKYCAM_LOG_OEM À» Ã£¾Æ ÁÖ¼® Ã³¸®ÇÑ´Ù. 
	¼±¾ð µÈ °æ¿ì, ÇØ´ç ÆÄÀÏ¿¡ »ç¿ëµÈ SKYCDBG/SKYCERR ¸ÅÅ©·Î¸¦ ÀÌ¿ëÇÑ 
	¸Þ½ÃÁöµéÀ» È°¼ºÈ­ ½ÃÅ²´Ù. (user-space only)

¸ðµç ¼Ò½º ÆÄÀÏ¿¡¼­ F_SKYCAM_LOG_CDBG ¸¦ Ã£¾Æ ÁÖ¼® Ã³¸®ÇÑ´Ù. 
	¼±¾ð µÈ °æ¿ì, ÇØ´ç ÆÄÀÏ¿¡ »ç¿ëµÈ CDBG ¸ÅÅ©·Î¸¦ ÀÌ¿ëÇÑ ¸Þ½ÃÁöµéÀ» 
	È°¼ºÈ­ ½ÃÅ²´Ù. (user-space only)

¸ðµç ¼Ò½º ÆÄÀÏ¿¡¼­ F_SKYCAM_LOG_VERBOSE ¸¦ Ã£¾Æ ÁÖ¼® Ã³¸®ÇÑ´Ù.
	¼±¾ð µÈ °æ¿ì, ÇØ´ç ÆÄÀÏ¿¡ »ç¿ëµÈ LOGV/LOGD/LOGI ¸ÅÅ©·Î¸¦ ÀÌ¿ëÇÑ 
	¸Þ½ÃÁöµéÀ» È°¼ºÈ­ ½ÃÅ²´Ù. (user-space only)




(4)  ¾È¸éÀÎ½Ä °ü·Ã ±â´É ºôµå È¯°æ

vendor/qcom/android-open/libcamera2/Android.mk ¸¦ ¼öÁ¤ÇÑ´Ù.
	3rd PARTY ¼Ö·ç¼Ç »ç¿ë ¿©ºÎ¸¦ °áÁ¤ÇÑ´Ù.

	SKYCAM_FD_ENGINE := 0		¹ÌÆ÷ÇÔ
	SKYCAM_FD_ENGINE := 1		¿Ã¶ó¿÷½º ¼Ö·ç¼Ç »ç¿ë
	SKYCAM_FD_ENGINE := 2		±âÅ¸ ¼Ö·ç¼Ç »ç¿ë

CUST_SKYCAM.h ¿¡¼­ F_SKYCAM_ADD_CFG_FACE_FILTER ¸¦ #undef ÇÑ´Ù.
	¾È¸éÀÎ½Ä ±â´É °ü·Ã ÀÎÅÍÆäÀÌ½º Æ÷ÇÔ ¿©ºÎ¸¦ °áÁ¤ÇÑ´Ù.

libOlaEngine.so ¸¦ ±âÁ¸ libcamera.so ¿¡ ¸µÅ©ÇÏ¹Ç·Î ±âÁ¸ ´ëºñ libcamera.so ÀÇ
Å©±â°¡ Áõ°¡ÇÏ¿© ¸µÅ© ¿À·ù°¡ ¹ß»ý °¡´ÉÇÏ¸ç, ÀÌ °æ¿ì ¾Æ·¡ ÆÄÀÏµé¿¡¼­ 
liboemcamera.so ÀÇ ¿µ¿ªÀ» ÁÙ¿© libcamera.so ÀÇ ¿µ¿ªÀ» È®º¸ÇÒ ¼ö ÀÖ´Ù.

build/core/prelink-linux-arm-2G.map (for 2G/2G)
build/core/prelink-linux-arm.map (for 1G/3G)	

2010-10-04  ¿Ã¶ó¿÷½º ¶óÀÌºê·¯¸® ¹× ¼Ò½ºÄÚµå ¾÷µ¥ÀÌÆ®

*/	



/* ³»¼ö CS ºÎ¼­¿¡¼­´Â ¼ÒºñÀÚ ½Ã·á ºÐ¼®À» À§ÇØ º°µµ PC ÇÁ·Î±×·¥À» »ç¿ëÇÏ¿© 
 * Ä«¸Þ¶ó ±¸µ¿ ½Ã°£ Á¤º¸¸¦ PC ·Î ÃßÃâÇÑ´Ù. 
 *
 * ±¸Çö ¹æ¹ýÀº °øÁ¤ Ä¿¸Çµå »ç¾ç¼­¿¡ ¸í½ÃµÇ¾î ÀÖÀ¸¹Ç·Î °ü·Ã ÄÚµåµéÀº °øÁ¤ Ä¿¸Çµå 
 * °ü·Ã ¸ðµâ¿¡ Æ÷ÇÔµÇ¾î ÀÖÀ¸³ª, °øÁ¤ Ä¿¸Çµå ¿ë PC ÇÁ·Î±×·¥À» »ç¿ëÇÏÁö ¾Ê°í º°µµ
 * ÇÁ·Î±×·¥À» »ç¿ëÇÏ¿©, ½Ã·áÀÇ DIAG Æ÷Æ®·ÎºÎÅÍ ±¸µ¿ ½Ã°£ Á¤º¸¸¦ È®ÀÎÇÒ ¼ö ÀÖ´Ù.
 *
 * °øÁ¤ Ä¿¸Çµå »ç¾ç¼­ v10.35 ±â¹Ý ±¸Çö
 * PhoneInfoDisplay v4.0 ÇÁ·Î±×·¥À¸·Î È®ÀÎ
 * »ç¾ç¼­¿Í ÇÁ·Î±×·¥Àº DS2ÆÀ ¹Ú°æÈ£ ¼±ÀÓ¿¡°Ô ¹®ÀÇ */

#define F_SKYCAM_FACTORY_PROC_CMD

/*========================================================
 * SKYCAM Lee Jitae 2010-11-09
 * Ä«¸Þ¶ó ANR¹ß»ýÀÌ³ª DeviceÀÌ»óÀ¸·Î open failÀÌ ÀÏ¾î ³¯ °æ¿ì Á¤»óÀûÀÎ ÀçÁøÀÔÀ» À§ÇÑ 
 * cameraservice connectÃ³¸® ¹× vfe resetÄÚµå ÀÛ¾÷
 *F_SKYCAM_INVALIDATE_CAMERA_CLIENT feature´Â ÀÌ¹Ì µé¾î°¡ ÀÖ¾úÀ½
======================================================== */
/* Ä«¸Þ¶ó ÀåÄ¡ ÆÄÀÏ OPEN ¿¡ ½ÇÆÐÇÑ °æ¿ì (´Ü¼ø I2C ¹ö½º R/W ¿À·ù, Ä«¸Þ¶ó ¹ÌÀåÂø) 
 * ¿À·ù Ã³¸®¸¦ À§ÇØ ¼öÁ¤ÇÑ´Ù. 
 *
 * ÀåÄ¡ ÆÄÀÏÀ» OPEN ÇÏ´Â °úÁ¤¿¡¼­ VFE ÃÊ±âÈ­ ÀÌÈÄ Ä«¸Þ¶ó HW ÃÊ±âÈ­°¡ ÀÌ·ç¾î 
 * Áö´Âµ¥, HW ÃÊ±âÈ­¿¡ ½ÇÆÐÇÒ °æ¿ì VFE ´Â ÃÊ±âÈ­ µÈ »óÅÂ·Î ³²°ÔµÇ°í ´ÙÀ½
 * OPEN ½Ãµµ ½Ã HW ÃÊ±âÈ­¿¡ ¼º°øÇÑ´Ù ÇÏ´õ¶óµµ ÀÌ¹Ì VFE °¡ ÃÊ±âÈ­µÈ »óÅÂÀÌ¹Ç·Î 
 * VFE ÃÊ±âÈ­ ½Ã ¿À·ù°¡ ¹ß»ýÇÑ´Ù.
 * 
 * È£Ãâ¼ø¼­ : vfefn.vfe_init() -> sctrl.s_init()
 *
 * HW ÃÊ±âÈ­¿¡ ½ÇÆÐÇÒ °æ¿ì, ÀÌ¹Ì ÃÊ±âÈ­µÈ VFE ¸¦ RELEASE (vfe_release) ½ÃÄÑ 
 * ´ÙÀ½ ¿­±â ½Ãµµ ½Ã Á¤»ó µ¿ÀÛÇÏµµ·Ï ¼öÁ¤ÇÑ´Ù. 
 *
 * ECLAIR ¹öÀü¿¡¼­´Â À§¿Í °°Àº ¿¡·¯ Ã³¸®¿¡µµ ºÒ±¸ÇÏ°í ¼¾¼­°¡ ¿¬°áµÇ¾î ÀÖÁö
 * ¾Ê°Å³ª ¼¾¼­ ÇÏµå¿þ¾î¿¡ ÀÌ»óÀÌ ¹ß»ýÇÑ °æ¿ì Ä«¸Þ¶ó ÀÀ¿ëÀÌ ANR ¿À·ù·Î ÀÎÇØ 
 * ºñÁ¤»ó Á¾·áµÇ°í ÀÌÈÄ ÀçºÎÆÃ Àü±îÁö´Â Áö¼ÓÇÏ¿© ÀçÁøÀÔÀÌ ºÒ°¡´ÉÇÏ´Ù.
 *
 * ¼¾¼­°¡ ºñ Á¤»óÀÎ °æ¿ì, ISP ÃÊ±âÈ­ ½Ã ISP ¿Í ¼¾¼­¸¦ ÇÁ¸®ºä ¸ðµå·Î ¼³Á¤ÇÏ´Â 
 * °úÁ¤¿¡¼­ 3ÃÊ °£ POLLING ¼öÇàÇÏ¸ç, ÀÌ·Î ÀÎÇØ Å¸ÀÓ¾Æ¿ô ¹ß»ýÇÏ°í ANR ¿À·ù·Î 
 * ÀÌ¾îÁø´Ù. ºñ Á¤»ó Á¾·á ÀÌÈÄ Ä«¸Þ¶ó ÀçÁøÀÔ ½Ã ¼¾¼­°¡ Á¤»óÀÌ¶ó ÇÏ´õ¶óµµ ANR 
 * ¿À·ù ÀÌÈÄ ÀÀ¿ëÀÌ ºñ Á¤»óÀûÀ¸·Î Á¾·áµÇ¾úÀ¸¹Ç·Î FRAMEWORK ³»ºÎ´Â ºñ Á¤»óÀÎ 
 * »óÅÂ·Î À¯ÁöµÇ°í, ÀÌ·Î ÀÎÇØ ÀçºÎÆÃ Àü±îÁö´Â Ä«¸Þ¶ó ÀÀ¿ë ÁøÀÔ ½Ã "Unable to 
 * connect camera device" ÆË¾÷°ú ÇÔ²² ¹«Á¶°Ç ÁøÀÔ¿¡ ½ÇÆÐÇÑ´Ù.
 *
 * ISP ÃÊ±âÈ­ ½Ã ÇÁ¸®ºä ¸ðµå ¼³Á¤ ÀÌÀü¿¡, ISP ¸¦ ÅëÇØ ¼¾¼­ÀÇ Æ¯Á¤ ·¹Áö½ºÅÍ¸¦ 
 * 1È¸ READ ÇÏ°í ºñ Á¤»óÀÏ °æ¿ì, ÀÌ¸¦ FRAMWORK À» ÅëÇØ ÀÀ¿ëÀ¸·Î Àü´ÞÇÏ¿© 
 * Á¤»óÀûÀ¸·Î Á¾·áµÇµµ·Ï ¼öÁ¤ÇÑ´Ù. 
 *
 * ¶ÇÇÑ ISP ÀÚÃ¼¿¡ ÀÌ»óÀÌ ¹ß»ýÇÑ °æ¿ì¿¡µµ, PROBE ½Ã¿¡ ¿À·ù ¹ß»ýÇÏ¿© ÇØ´ç 
 * µð¹ÙÀÌ½º ÆÄÀÏµéÀ» »ý¼ºÇÒ ¼ö ¾øÀ¸¹Ç·Î FRAMWORK ³»ºÎ¿¡¼­ ÇÔ²² Ã³¸® °¡´ÉÇÏ´Ù. */

#define F_SKYCAM_INVALIDATE_CAMERA_CLIENT


/* SKYCAM Lee Jitae 2010-11-12 
 *  Antina sensor Mt9p111¿¡¼­ ¿­È­ Çö»ó ¹ß»ý
 * OTP·»Áî½¦ÀÌµù °ªÀ» 1ÀÌ¶ó°í ÀÎ½ÄÇÏ´Â ·¹ÆÛ·±½º º¼Æ¼Áö¸¦ Á¶ÀýÇÏ´Â ·¹Áö½ºÅÍ
 * Á¤È®ÇÑ °ªÀº Antina¿¡¼­ °¡¸£ÃÄ ÁÖÁö ¾ÊÀ½
 * Àû¿ë µÇÁö ¾ÊÀ¸¸é ¿­È­ Çö»óÀÌ ¹ß»ýÇÏ´Â ¸ðµâÀÌ ³ª¿Â´Ù (EF13S¿¡¼­ ¸¹ÀÌ ³ª¿ÔÀ½) EF18K¿¡¼­´Â ÇÑ°³ ¹ß°ßÇØ¼­ ¸ÕÀú Àû¿ëÀ» ÇÔ
 */

#define F_SKYCAM_SENSOR_BUG

/* Ä«¸Þ¶ó °ü·Ã GPIO µéÀ» ¼³Á¤ÇÑ´Ù. 
 *
 * ÃÑ 12 ¶óÀÎÀÇ µ¥ÀÌÅÍ ¹ö½º¿¡¼­ »óÀ§ 8ºñÆ®¸¸ »ç¿ëÇÑ´Ù. 
 *
 * Ä«¸Þ¶ó HW Á¾·á ½ÃÁ¡°ú SLEEP ÁøÀÔ ½ÃÁ¡ Ãæµ¹ ½Ã HW ÀÌ»ó µ¿ÀÛÀ» ¹æÁöÇÏ±â À§ÇØ
 * SLEEP_CONFIGS[] ¿¡¼­ Ä«¸Þ¶ó °ü·Ã GPIO µéÀº ¸ðµÎ Á¦¿Ü½ÃÅ²´Ù.
 *
 * VCM/FLUID °ü·Ã GPIO µéÀº »ç¿ëÇÏÁö ¾ÊÀ¸¹Ç·Î ¼³Á¤ ºÎºÐÀ» »èÁ¦ÇÑ´Ù.
 *
 * °ü·Ã ÆÄÀÏ Âü°í
 * AMSS/products/7x30/core/systemdrivers/tlmm/src/bsp/7x30/TLMMBsp.c
 * AMSS/products/7x30/core/systemdrivers/tlmm/src/bsp/7x30/TLMMBspMsm.h */

#define F_SKYCAM_HW_GPIO

/* HW I2C ¹ö½ºÀÇ Å¬·°À» ¼öÁ¤ÇÑ´Ù. 
 *
 * MT9P111 ÀÇ °æ¿ì, ÃÊ±âÈ­ ¹× °¢Á¾ ¼³Á¤ º¯°æ ½Ã WRITE ÇØ¾ß ÇÒ ·¹Áö½ºÅÍÀÇ ¼ö°¡ 
 * ¸¹À¸¹Ç·Î ¼º´É Çâ»óÀ» À§ÇØ ÃÖ´ë Å¬·°À» »ç¿ëÇÑ´Ù. */

#define F_SKYCAM_CUST_I2C_CLK_FREQ

/*----------------------------------------------------------------------------*/
/*  MODEL-SPECIFIC                                                            */
/*  EF18K ¿¡¸¸ Àû¿ëµÇ´Â ¶Ç´Â EF18K ¿¡¼­¸¸ °ËÁõµÈ FEATURE ¸ñ·Ï                 */
/*----------------------------------------------------------------------------*/

/*
 * ÇöÀç VREG Çì´õÆÄÀÏ Æ÷ÇÔ°ú i2c¿¡ °ü·ÃµÇ¾î ÀÖÀ½  
 *  F_SKYCAM_VREG¿Í F_SKYCAM_I2C_INIT·Î ¹Ù²Ü ¿¹Á¤
 */

#define F_SKYCAM

/* "ro.product.device" ¼³Á¤ °ª º¯°æÀ¸·Î ÀÎÇÑ ÄÚµå ¿Àµ¿ÀÛÀ» ¸·±â À§ÇØ Ä«¸Þ¶ó
 * °ü·Ã ÄÚµåµé¿¡¼­´Â ÀÌ °ªÀ» ÀÐÁö ¾Ê°Å³ª 'TARGET_QSD8250' À¸·Î °íÁ¤ÇÑ´Ù. 
 * ½Ã½ºÅÛ ¼³Á¤ "ro.product.device" ´Â ¸±¸®Áî ¹öÀüÀÇ °æ¿ì "qsd8250_surf" ÀÌ°í,
 * EF18K ÀÇ °æ¿ì "ef18k" À¸·Î ¼³Á¤µÇ¾î ÀÖ´Ù. */

#define F_SKYCAM_CUST_TARGET_TYPE_7X30


/* SKYCAM Lee Jitae 2010-12-11 
 * À¯Àú ¿µ¿ªÀÇ LOGV/LOGD/LOGI ¸Þ¼¼Áö on off
*/

#define F_SKYCAM_LOG_VERBOSE

/* Ä¿³Î ¿µ¿ª ÄÚµå¿¡¼­ SKYCDBG/SKYCERR ¸ÅÅ©·Î¸¦ »ç¿ëÇÏ¿© Ãâ·ÂµÇ´Â ¸Þ½ÃÁöµéÀ»
 * È°¼ºÈ­ ½ÃÅ²´Ù. 
 * kernel/arch/arm/mach-msm/include/mach/camera.h */

#define F_SKYCAM_LOG_PRINTK

/* À¯Àú ¿µ¿ªÀÇ SKYCDBG/SKYCERR ¸Þ¼¼Áö on off	
  */

 #define F_SKYCAM_LOG_OEM  

 /* QUALCOMM ¸±¸®Áî ÄÚµå¿¡ µð¹ö±× ¿ë ¸Þ½ÃÁö¸¦ Ãß°¡ÇÑ´Ù. */

#define F_SKYCAM_LOG_DEBUG



/* ³ìÈ­ ½ÃÀÛ/Á¾·á ½Ã, È¿°úÀ½ÀÌ ÇØ´ç µ¿¿µ»ó ÆÄÀÏ¿¡ ³ìÈ­µÇ´Â ¹®Á¦¸¦ ÀÓ½Ã ¼öÁ¤ÇÑ´Ù.
 *
 * ³ìÈ­ ½ÃÀÛ ½Ã¿¡´Â È¿°úÀ½ Àç»ý ¿Ï·á ½Ã±îÁö POLLING ÈÄ ³ìÈ­¸¦ ½ÃÀÛÇÏ°í,
 * ¿Ï·á ½Ã¿¡´Â ³ìÈ­ ¿Ï·á ÀÌÈÄ 800ms °íÁ¤ DELAY Àû¿ë ÈÄ È¿°úÀ½À» Àç»ýÇÑ´Ù.
 *
 * R4070 ±îÁö´Â ³ìÈ­ ½ÃÀÛ ½Ã ¿Àµð¿À Å¬·°ÀÌ ÃÊ±âÈ­µÇ´Â ½Ã°£ÀÌ ¿À·¡ °É·Á
 * ´Ù¼öÀÇ ÇÁ·¹ÀÓÀÌ DROP µÇ¸é¼­ ³ìÈ­ ½ÃÀÛÀ½ÀÌ ³ìÀ½ µÉ °¡´É¼ºÀÌ Èñ¹ÚÇßÀ¸³ª,
 * R4075 ¿¡¼­ ÀÌ ÀÌ½´°¡ ¼öÁ¤µÇ¸é¼­ ³ìÈ­ ½ÃÀÛ ½Ã È¿°úÀ½ÀÌ 100% È®·ü·Î ³ìÀ½µÇ°í, 
 * Á¾·á ½Ã 80% ÀÌ»ó È®·ü·Î ³ìÀ½µÈ´Ù.
 *
 * F_SKYCAM_TODO, QUALCOMM ¼öÁ¤ È®ÀÎ ÈÄ »èÁ¦ÇØ¾ß ÇÑ´Ù. (SR#307114) */

#define F_SKYCAM_QBUG_REC_BEEP_IS_RECORDED


/* ´Ù¿î »çÀÌÁî¸¦ ÇÏ±â À§ÇÑ ÇÔ¼ö¸¦ »ç¿ëÇÏ±â À§ÇØ¼­,  Ä¸ÃÄ È­¸éÀ» º¸¿©ÁÖ´Â °÷°ú Ä¸ÃÄ È­¸éÀÌ  Æ²¸° °ÍÀ» ¼öÁ¤ÇÏ±â À§ÇØ¼­
 * ½æ³×ÀÏÀ» °¡Áö°í ½º³À¼¦ÇÁ¸®ºä¸¦ ¸¸µç´Ù.
 * IPL ¶óÀÌºê·¯¸®¿¡ ÀÖ´Â downsize¸¦ ÀÌ¿ëÇØ¼­ ÁÙÀÎ´Ù.
 * ÇöÀç SRÁøÇà Áß -> Ä÷ÄÄ ÆÐÄ¡¿¡ Àû¿ë µÉ ¼ö ÀÖµµ·Ï ¼öÁ¤ ¹Þ±â À§ÇØ¼­ ÁøÇà Áß
 */

/* #define F_SKYCAM_SNAPSHOT_PREVIEW  */


/* EF18K ¿¡¼­ »ç¿ëµÇ´Â Ä«¸Þ¶ó Á¤º¸ Å×ÀÌºí (ÃÖ´ë Ãâ·Â °¡´É ÇØ»óµµ, ÃÖ´ë ½º³À¼¦ 
 * ÇØ»óµµ, AF Áö¿ø ¿©ºÎ) À» ¼öÁ¤ÇÑ´Ù. */

#define F_SKYCAM_CUST_SENSOR_TYPE


/* Ä«¸Þ¶ó IOCTL Ä¿¸Çµå MSM_CAM_IOCTL_SENSOR_IO_CFG ¸¦ È®ÀåÇÑ´Ù. 
 *
 * EF10S ¿¡¼­ Ãß°¡ÇÑ ±â´É ¹× SOC Ä«¸Þ¶ó¸¦ °¨¾ÈÇÏÁö ¾Ê°í ¹Ì±¸ÇöµÈ ºÎºÐµéÀ» 
 * ¼öÁ¤ ¹× Ãß°¡ ±¸ÇöÇÑ´Ù. */

#define F_SKYCAM_CUST_MSM_CAMERA_CFG

/* ZOOM LEVEL ÀÌ 51 ÀÌ»óÀÎ°æ¿ì wide size ¿¡¼­ preview frame ÀÇ width/height ratio°¡ ´Þ¶óÁø´Ù.
 * zoom level 0~50 : 1.6666
 * zoom level 51 ÀÌ»ó : width ¸¸ È®´ë µÊ.
 * zoom level 60 : 1.3333
 * R5040¿¡¼­ Qualcomm SBA ¹Þ¾Æ¼­ patch Àû¿ë. ÀÌÈÄ release¿¡ Àû¿ë ¿©ºÎ È®ÀÎ ÈÄ »èÁ¦ ÇÑ´Ù.
 */

#define F_SKYCAM_QBUG_ZOOM_MORE51



/* SKYCAM Lee Jitae 2010-10-05 
 * QualcommCameraHardware.cpp¿¡¼­ setParameters´Â VT¿Í Ä«¸Þ¶ó °ü·ÃµÈ ¼¼ÆÃÀÌ µÈ´Ù. VT¿¡¼­ ÇÊ¿ä¾ø´Â ¼¼ÆÃÀ» ÇÇÇÏ±â À§ÇØ¼­
 * Ä«¸Þ¶óÀÏ¶§¸¸ ÆÄ¶ó¸ÞÅÍ ¼¼ÆÃÀ» ÇÏ±â À§ÇØ¼­ ±¸ÇöÇÑ ºÎºÐ
 * EF18KÀÇ°æ¿ì ÇÊ¿äÇÏÁö ¾ÊÀº ¼¼ÆÃÀº ifndef·Î ¸·¾Æ µÎ¾ú´Ù.
 */


#define F_SKYCAM_CAMERA_SETTING_ONLY

/* SKYCAM_PSJ_0100331 : Ä«¸Þ¶ó Æ©´× ÆÄÀÏ ·Îµå¸¦ À§ÇØ... */
/* #define F_SKYCAM_SENSOR_TUNEUP */
#ifdef F_SKYCAM_SENSOR_TUNEUP
/* #define F_SKYCAM_TUP_LOAD_FILE */
/* #define MT9P111_TUP_FNAME       "/sdcard/camsensor_mt9p111_tuneup_reg.txt" */
#endif

/* SKYCAM 2010-10-21
 * 2010-10-26 Qualcomm¿¡¼­ ¼öÁ¤ ÄÚµå¸¦ ¹Þ¾Æ¼­ ¼öÁ¤
 * GPS, JPEG THUMBNAIL, JPEG_EXIF ¿¡¼­ failÀÌ ¹ß»ýÇÑ´Ù.
 * ÇöÀç MT9P111¼¾¼­¿¡¼­ Á¶µµ°¡ ³·À» °æ¿ì¿¡´Â checkpreview°¡ failed°¡ ¹ß»ý. 
 * Timeout Error°¡ ¹ß»ýÇÑ´Ù. ÀÌ·¯ÇÑ ÀÌÀ¯´ÂWAIT_FOR_COMMAND_TO_COMPLETE°¡ 1.5ÃÊ·Î ¼³Á¤µÇ¾î¼­ÀÌ´Ù.
 * WAIT_FOR_COMMAND_TO_COMPLETEÀÇ °æ¿ì EF13S, EF13LÀÇ °æ¿ì´Â 5ÃÊ·Î ¼³Á¤µÇ¾î  ÀÖ´Ù.
 * ÇöÀç±îÁö ¿øÀÎÀ¸·Î´Â ÀúÁ¶µµ¿¡¼­´Â ¼¾¼­ ¾ÈÁ¤È­ ±îÁöÀÇ ½Ã°£ÀÌ ¸¹ÀÌ °É¸®±â ¶§¹®ÀÌ¶ó°í ÆÄ¾ÇÇÏ°í ÀÖ´Ù.
 */

#define F_SKYCAM_QBUG_CTS



/* SKYCAM Lee Jitae 2010-11-27
 * ÃÔ¿µÀÌ Á¾·áµÇÁö ¾ÊÀº »óÈ²¿¡¼­ Stop preview¸¦ ÇÏ¿© ¸Þ¸ð¸®°¡ ÇØÁ¦µÇ´Â »óÈ²ÀÌ ¹ß»ý, NULL POINTER REFERENCE¿À·ù·Î ÀÎÇØ¼­ °¢Á¾ ¼­¹öµéÀÌ
 * Á×°í ÀÌÈÄ¿¡´Â ÀÌ»ó È­¸é Ç¥½Ã, »ç¿îµå ¾È³² µîÀÇ ž­µéÀÌ ¹ß»ýÇÕ´Ï´Ù.
 */

#define F_SKYCAM_QBUG_SNAPSHOT_RELEASE_INTERRUPT


/* SKYCAM_PSJ_110302
 * ±âÁ¸ FEATUREÆù¿¡¼­ »ç¿ëÇÏ´ø Ä÷ÄÄÀÇ IPLÇÔ¼ö¸¦ »ç¿ëÇÏ±â À§ÇØ IPL LIB·Îµå
 * ipl_reflect Å×½ºÆ® ¿Ï·á
*/

#define F_SKYCAM_USE_IPLLIB


/*----------------------------------------------------------------------------*/
/*  SENSOR CONFIGURATION                                                      */
/*  ¸ðµ¨ º° »ç¿ë ¼¾¼­(ISP)¸¦ À§ÇØ ¼öÁ¤/Ãß°¡µÈ FEATURE ¸ñ·Ï                    */
/*----------------------------------------------------------------------------*/
/* If you use YUV sensor only, define this feature. If you use two cameras, 
 * and one is YUV and another is BAYER, don't define this. If you define this,
 * some BAYER-codes will not be processed.
 */

#ifdef T_J_MASAI_E
#define F_SKYCAM_TARGET_J_MASAI_E
#endif
	
#ifdef T_MINI
#define F_SKYCAM_TARGET_MINI
#endif

#ifdef T_LASER2
#define F_SKYCAM_TARGET_LASER2
#endif

#define F_SKYCAM_YUV_SENSOR_ONLY
#define F_SKYCAM_MT9P111


#ifdef F_SKYCAM_YUV_SENSOR_ONLY
#define F_SKYCAM_ADD_CFG_ANTISHAKE
#define F_SKYCAM_FIX_CFG_EXPOSURE
#define F_SKYCAM_FIX_CFG_SCENE_MODE
#define F_SKYCAM_FIX_CFG_FOCUS_RECT
/* #define F_SKYCAM_FIX_CFG_REFLECT */
#define F_SKYCAM_FIX_CFG_FOCUS_STEP
#define F_SKYCAM_FIX_CFG_ROTATION
#define F_SKYCAM_ADD_CFG_MULTISHOT
#define F_SKYCAM_CUST_PICTURE_SIZES
#define F_SKYCAM_FIX_CFG_BRIGHTNESS
#define F_SKYCAM_FIX_CFG_EFFECT
#define F_SKYCAM_FIX_CFG_PREVIEW_FPS
#define F_SKYCAM_FIX_CFG_ANTIBANDING
#define F_SKYCAM_FIX_CFG_AF
#define F_SKYCAM_CUST_PREVIEW_SIZES
#define F_SKYCAM_FIX_CFG_WB
/* Do. layer1 for focus mode setting at QC HAL*/
#define F_SKYCAM_FIX_CFG_FOCUS_MODE
/* #define F_SKYCAM_ADD_CFG_DIMENSION */
/* F_SKYCAM Lee Jitae 2010-11-15
 *  CamcoderÀÏ °æ¿ì CAF¸¦ ON, OFF¸¦ ÇÒ ¼ö ÀÖ´Ù
 * EF18KÀÇ °æ¿ì ÇØ»ó·Â ¹®Á¦°¡ ¹ß»ýÇØ¼­ Á¦¿Ü
 */

/* #define F_SKYCAM_FIX_CFG_CAF */
#else

/* SKYCAM Lee Jitae 2010-09-25
 * VPEÀÇ»ç¿ëÀ¸·Î ÀÎÇØ¼­ ºñµð¿À ÇÁ·¹ÀÓÀÌ ¿Ã¶ó¿ÀÁö ¾Ê´Â ¹®Á¦Á¡ÀÌ ¹ß»ý
 * mDisEnabledÀÇ ¹Ì»ç¿ëÀ¸·Î ¹®Á¦ ÇØ°á 
 * mDisEnabledÀÇ °æ¿ì¿¡´Â BayerSensorÀÇ °æ¿ì¿¡¸¸ EnableÇÑ´Ù.
 * mDisEnabled Àº DIS(Digital Image Stabilization) ±â´ÉÀ» ON- OFFÇÏ´Â °ÍÀÌ´Ù.
 */

/* #define F_SKYCAM_VIDEO_FRAME_VPE_DIS */


#endif


/* ÇÃ·¡½¬ LED ¼³Á¤À» À§ÇÑ ÀÎÅÍÆäÀÌ½º¸¦ ¼öÁ¤ÇÑ´Ù.
 *
 * QUALCOMM ¿¡¼­´Â º°µµÀÇ IOCTL (MSM_CAM_IOCTL_FLASH_LED_CFG) Ä¿¸Çµå¸¦ 
 * »ç¿ëÇÏ¿© ±¸ÇöµÇ¾î ÀÖÀ¸¸ç, PMIC Àü¿øÀ» »ç¿ëÇÏ´Â LED µå¶óÀÌ¹ö¸¦ Á¦°øÇÑ´Ù.
 * EF10S ¿¡¼­´Â ÀÌ¸¦ »ç¿ëÇÏÁö ¾ÊÀ¸¸ç, MSM_CAM_IOCTL_SENSOR_IO_CFG À» È®ÀåÇÏ¿©
 * ±¸ÇöÇÑ´Ù.
 *
 * AUTO ¸ðµå·Î ¼³Á¤ÇÒ °æ¿ì, ÀúÁ¶µµ ÀÏ °æ¿ì¿¡¸¸ AF ¼öÇà Áß AF/AE ¸¦ À§ÇØ
 * Àá½Ã ON µÇ°í, ½ÇÁ¦ ½º³À¼¦ ½ÃÁ¡¿¡¼­ ÇÑ ¹ø ´õ ON µÈ´Ù. */

#define F_SKYCAM_FIX_CFG_LED_MODE

/* KDDI ??Mirach_J???„ì´¬ë°©ì???RED LEDë¥?ON/OFF ?????ˆëŠ” ?¸í„°?˜ì´?¤ë? ì¶”ê?
 *  startPreview()?œì— ON ?œí‚¤ê³?  stopPreview()?œì— OFF ?œí‚¤?„ë¡ ë³€ê²? */
#if defined(F_SKYCAM_TARGET_J_MASAI_E) && (BOARD_VER >= WS10)
#define F_SKYCAM_ADD_CFG_RED_LED_MODE
#endif

/* SKYCAM Lee Jitae 2010-09-25
 * OTP¸ðµâÀÇ ºÒ·®·üÀ» ÁÙÀÌ±â À§ÇØ ¸Þ¸ð¸® zoneÀÇ 2°³¸¦ detectingÇÏ¿© 
 * ÇöÀç »ç¿ëÇØ¾ß µÉ zoneÀ» Ã£¾Æ³½ ÈÄ ÇØ´ç zoneÀÇ ¸Þ¸ð¸®¸¦ »ç¿ë ÇÒ ¼ö ÀÖµµ·Ï Ãß°¡
 * WS02Â÷¿¡¼­ Àû¿ë ¿¹Á¤
*/

#define F_SKYCAM_ADD_CFG_OTP_MODULE

/* 
 * 800*480, 640*480 À» Á¦¿ÜÇÑ ´Ù¸¥ ÀÌ¹ÌÁö »çÀÌÁî¿¡¼­ ¼¼·Î ¸ðµå(90µµ)·Î ÇØ¼­ ÁÜÇØ¼­ »çÁø ÃÔ¿µÀ» ÇÑ °æ¿ì ÀÌ¹ÌÁö°¡ ±úÁü
 * ÀÌ¹ÌÁö ¿ÞÂÊ ³¡ÂÊÀÌ ±ú¾îÁø´Ù. ±¸±Û ±âº» Ä«¸Þ¶ó¿¡¼­´Â 3M 1.4¹è¿¡¼­ ±úÁö±â ½ÃÀÛÇÏ´Â °ÍÀ» È®ÀÎÇÔ
 * ÀÌ·¯ÇÑ ÀÌ¹ÌÁö°¡ ±úÁö´Â °æ¿ì¸¦ ÀÛ¾÷ÇÑ °Í
 */

#define F_SKYCAM_QBUG_ROTATION_ZOOM

/* 
 * MEDIA_RECORDER_INFO_FILESIZE_PROGRESS CallBack, Only Use KT Phone 
 * KTÆù¿¡¼­´Â ºñµð¿À ÆÄÀÏ »çÀÌÁî¸¦ ±â·ÏÇÏ´Âµ¥ ÆÄÀÏ »çÀÌÁî¸¦ APP¿¡ ¾Ë·ÁÁÖ±â À§ÇØ¼­
 * Ãß°¡ 
 */

/* #define F_SKYCAM_ADD_EVT_RECSIZE */



/* ES01 Â÷¿ë ¹ÙÀÌ³Ê¸® Á¦¾î Feature  Lee Jitae 2010-10-13 */
/* EF18KÀÇWS02Â÷ ½Ã·á H/W ¹®Á¦¹× power sequence°¡ ¸ÂÁö  ¾Ê¾ÒÀ½
 *  ±×·¯ÇÑ Power sequence¹®Á¦ ÇØ°áÀ» À§ÇÑ Feature
 * EF18K WS02Â÷ ½Ã·á´Â ÇÇÃÄ¸¦ ¸·°í ES1Â÷ ÀÌÈÄ´Â ¿­¾îÁØ´Ù.
 */

/* #define F_SKYCAM_EF18K_ES01 */


   /* SKYCAM_PSJ_100622
   * BGMÀ» Æ²¾î³õÀº »óÅÂ¿¡¼­ Ä«¸Þ¶ó ¿¬¼ÓÃÔ¿µ½Ã ÃÔ¿µÀ½ ±úÁö´Â Çö»óÀÌ ÀÖÀ¸¸ç 
   * get_durationÇÔ¼ö¸¦ ÀÌ¿ëÇÏ¿© À½¿øÀÇ ±æÀÌ¸¦ ¾òÀº ÈÄ ±×¸¸Å­ usleepÀ» ÁÖ¸é 
   * ±úÁö´Â Çö»óÀÌ ¾ø¾îÁö³ª ÃÔ¿µÀ½ÀÌ ´Ù ³¡³­ ÈÄ¿¡ ÂïÀº »çÁøÀÌ º¸¿©Áü
   * À½¿øÀÌ ±úÁö´Â Çö»óÀº ¼º´É»óÀÇ ¹®Á¦ÀÌ¹Ç·Î ÀÏ´Ü ¿ì¸®°¡ ¼öÁ¤ÇÏÁö ¾Ê´Â °É·Î ÇÑ´Ù
  */
  
  /* SKYCAM Lee Jitae 2010-10-22
   * VGA·Î Æú¶ó·ÎÀÌµå¸¦ ÂïÀ» ¶§ »ç¿îµå°¡ ¹¶°³Áö´Â Çö»óÀÌ ÀÏ¾î³­´Ù. EF13¿¡¼­ ¿¬¼Ó ÃÔ¿µ¿¡¼­ ¹®Á¦°¡ »ý±â³ª
   * EF18K¿¡¼­´Â ¹®Á¦ ¹ß»ýÀÌ º¸ÀÌÁö ¾Ê¾Ò´Ù. ¾à°£ ´À·ÁÁø µíÇÑ °¨Àº ÀÖÀ½.
   * ÀÏ´Ü EF13S¿¡ ³ª¿Â °ÍÀº »ó±â¿¡ Ç¥±âÇÑ´Ù.
   */

/* #define F_SKYCAM_FIX_SNAPSHOT_BEEP_SPLIT */


/* SKYCAM Lee Jita3 2010-11-23 
 * 720P¿¡¼­ ÇÁ¸®ºä°¡ 1280*720ÀÌ µÇÁö ¾Ê´Â´Ù.
 * Ç¥Çö µÉ ¼ö ÀÖ´Â ÇÁ¸®ºä »çÀÌÁî·Î º¯°æÇØÁØ´Ù.
 */

#define F_SKYCAM_720P_PREVIEW

/* SKYCAM Lee Jitae 2010-12-10
 * MARKET CAMERA APP³ª I'M IN APPÀÇ POSTVIEW°¡ ±úÁø´Ù.
 * ¿øÀÎÀº Set_demensionÀ» ÇÏ¸é POSTVIEW»çÀÌÁî°¡ º¯ÇÏ´Â ¾îÇÃµéÀÌ ÀÖ´Ù.
 * ÀÌ·±  ¾îÇÃµéÀº CAMERA_SET_PARM_DIMENSION ÀÌÈÄ °ªÀÌ º¯°æµÈ´Ù.
 * SKYCAM¿¡¼­´Â 1.3333À¸·Î ºñÀ²¿¡ º¯È­°¡ ¾øÀÌ ³ª¿À±â ¶§¹®¿¡ ¹®Á¦¾ø´Ù.
 */

 #define F_SKYCAM_QBUG_MARKET_APP


 /* SKYCAM Lee JItae 2010-11-23
  * WVGAÀÏ °æ¿ì ÁÜÀ» ÇÏ¸é POSTVIEW°¡ Ä¿Áø´Ù.
  * QualcommÀ¸·Î ºÎÅÍ ÆÐÄ¡ ¹ÞÀº ÄÚµå
  */

  #define F_SKYCAM_QBUG_WVGA_ZOOM


  /* ÃÔ¿µÀ½/³ìÈ­À½ Àç»ý Áß¿¡ ÀÀ¿ëÀÌ Á¾·áµÉ °æ¿ì, CLIENT ¼Ò¸ê ½Ã¿¡ ÇØ´ç ÃÔ¿µÀ½/
   * ³ìÈ­À½ ¿ÀºêÁ§Æ®°¡ °­Á¦·Î disconnect/clear µÇ¸é¼­ MEDIA SERVER °¡ Á×´Â °ÍÀ»
   * ¹æÁöÇÑ´Ù. */

#define F_SKYCAM_QBUG_STOP_CAMERA_SOUND


/* 2010-10-06 
 *Continous AF ¸¦ Å×½ºÆ® ÇÏ±â À§ÇØ ÀÛ¾÷ ÇÒ ³»¿ë 
 */
/* #define F_SKYCAM_CAF
*/

/* oen exif tag ¼öÁ¤ */

#define F_SKYCAM_OEM_EXIF_TAG


/* µ¿¿µ»ó ³ìÈ­ ½ÃÀÛ/Á¾·á¸¦ ºü¸£°Ô ¹Ýº¹ÇÏ°Å³ª, ÀÌ¾îÀèÀ» ÀåÂøÇÑ »óÅÂ¿¡¼­ ¿¬¼ÓÃÔ¿µ
 * ¸ðµå·Î ÃÔ¿µÇÒ °æ¿ì, MediaPlayer °¡ ¿Àµ¿ÀÛÇÏ¸é¼­ HALT ¹ß»ýÇÑ´Ù.
 *
 * MediaPlayer ÀÇ °æ¿ì, µ¿ÀÏÇÑ À½¿øÀ» Àç»ý Áß¿¡ ¶Ç ´Ù½Ã Àç»ý ½ÃµµÇÒ °æ¿ì 100%
 * ¿Àµ¿ÀÛÇÏ¹Ç·Î µ¿ÀÏ À½¿øÀ» ¿¬¼ÓÇÏ¿© Àç»ýÇØ¾ß ÇÒ °æ¿ì, ¹Ýµå½Ã ÀÌÀü Àç»ýÀÌ ¿Ï·á
 * µÇ¾ú´ÂÁö ¿©ºÎ¸¦ È®ÀÎ ÈÄ Àç»ýÇØ¾ß ÇÑ´Ù. */
/* #define F_SKYCAM_QBUG_SKIP_CAMERA_SOUND */


/*
AT&T's model has a option that can be turn shutter sound on or off.
*/

#define F_SKYCAM_SET_SHUTTER_SOUND
/*
Ä·ÄÚ´õ Á¾·áÀ½À» Á¤»óÀûÀ¸·Î ÇÃ·¹ÀÌ ÇÒ ¼ö º¸ÀåÇØÁÖ´Â µô·¹ÀÌ Ãß°¡.
*/
#define F_SKYCAM_DELAY_ON_REC_STOP_SOUND 


/*
	QE1Â÷ Æò°¡ ¹®Á¦ »çÇ× ¼öÁ¤ (2010/10/18)

	Camera preview È­¸é¿¡¼­´Â ÃÊÁ¡ÀÌ ¾È¸ÂÀ¸³ª ÂïÀº ÈÄ È­¸éÀº Á¤»óÀûÀÓ
	-->  Auto focus µ¿ÀÛ ÀÌÈÄ snapshot mode ÀÌÀü¿¡ 100 msec µô·¹ÀÌ Ãß°¡
*/


#define F_SKYCAM_BUG_PATCH_POSTVIEW_FOCUS


/*#define F_SKYCAM_CHECK_IS_INIT_SEQ_DONE*/  /* it skip preview mode process when initiating sensor */
#define F_SKYCAM_SAME_FOCUS_STEP_SKIP /* it skip focus mode setting when previous and current focus mode are same */

/*----------------------------------------------------------------------------*/
/*  MODEL-SPECIFIC CONSTANTS                                                  */
/*  ¸ðµ¨ °ü·Ã »ó¼ö ¼±¾ð                                       */
/*----------------------------------------------------------------------------*/


#ifdef F_SKYCAM_FACTORY_PROC_CMD
#define C_SKYCAM_FNAME_FACTORY_PROC_CMD	"/data/.factorycamera.dat"
#endif

/* ¼³Á¤ °¡´ÉÇÑ ÃÖ¼Ò/ÃÖ´ë Æ÷Ä¿½º ·¹º§ÀÌ´Ù. */
#ifdef F_SKYCAM_FIX_CFG_FOCUS_STEP
#define C_SKYCAM_MIN_FOCUS_STEP 0	/* ¹«ÇÑ´ë (default) */
#define C_SKYCAM_MAX_FOCUS_STEP 9	/* ¸ÅÅ©·Î */
#endif

/*  FPS setting value*/
#ifdef F_SKYCAM_FIX_CFG_PREVIEW_FPS


#ifndef F_SKYCAM_TARGET_LASER2
#define C_SKYCAM_MIN_PREVIEW_FPS	5
#define C_SKYCAM_MAX_PREVIEW_FPS	30
#define C_SKYCAM_VARIABLE_FPS	31
#define C_SKYCAM_MAX_SUPPORT_FPS 20
/* #define C_SKYCAM_VT_PREVIEW_FPS		7 */
#else
#define C_SKYCAM_MIN_PREVIEW_FPS	8
#define C_SKYCAM_MAX_PREVIEW_FPS	30
#define C_SKYCAM_VARIABLE_FPS	31
#define C_SKYCAM_MAX_SUPPORT_FPS 19

#endif

#endif


/* Brightness ¼¼ÆÃ */

#ifdef F_SKYCAM_FIX_CFG_BRIGHTNESS
#define C_SKYCAM_MIN_BRIGHTNESS 0
#define C_SKYCAM_MAX_BRIGHTNESS 8
#endif

#ifdef F_SKYCAM_FIX_CFG_FOCUS_RECT

#ifndef F_SKYCAM_TARGET_LASER2
#define SENSOR_PREVIEW_WIDTH 1280
#define SENSOR_PREVIEW_HEIGHT 960
#define NUM_BLOCKS 16
#else
#define SENSOR_PREVIEW_WIDTH 1024
#define SENSOR_PREVIEW_HEIGHT 768
#define NUM_BLOCKS 12
#endif

#endif


#ifdef F_SKYCAM_OEM_EXIF_TAG

#ifdef F_SKYCAM_TARGET_J_MASAI_E
#define C_SKYCAM_EXIF_MAKE		"KDDI-PT"
#define C_SKYCAM_EXIF_MAKE_LEN		8		/* including NULL */

#ifdef EIS01PT
#define C_SKYCAM_EXIF_MODEL		"EIS01PT"
#define C_SKYCAM_EXIF_MODEL_LEN		8		/* including NULL */

#else
#define C_SKYCAM_EXIF_MODEL		"IS11PT"
#define C_SKYCAM_EXIF_MODEL_LEN		7		/* including NULL */

#endif

#define F_SKYCAM_J_MASAI_E_ANTIBANDING

#if (BOARD_VER >= WS20) 
#define MT9P111_J_MASAI_E_WBPOINT
#ifdef F_SKYCAM_CHECK_IS_INIT_SEQ_DONE
#undef F_SKYCAM_CHECK_IS_INIT_SEQ_DONE
#endif
#endif

#else /* F_SKYCAM_TARGET_J_MASAI_E */

#define C_SKYCAM_EXIF_MAKE		"PANTECH"
#define C_SKYCAM_EXIF_MAKE_LEN		8		/* including NULL */


#ifdef F_SKYCAM_TARGET_MINI
#define C_SKYCAM_EXIF_MODEL		"MINI"
#define C_SKYCAM_EXIF_MODEL_LEN		5		/* including NULL */
#undef F_SKYCAM_FIX_CFG_LED_MODE /* MINI hasn't flash mode. */
/* to satisfy at&t's spec ( max 30s on MMS recording) bit-rate should be below 192k*/
#define F_SKYCAM_CFG_MMS_REC_FOR_ATT
#endif

#ifdef F_SKYCAM_TARGET_LASER2
#define C_SKYCAM_EXIF_MODEL     "LASER2"
#define C_SKYCAM_EXIF_MODEL_LEN     7       /* including NULL */
#undef F_SKYCAM_FIX_CFG_LED_MODE	/* LASER2 hasn't flash mode. */
/* to satisfy at&t's spec ( max 30s on MMS recording) bit-rate should be below 192k*/
#define F_SKYCAM_CFG_MMS_REC_FOR_ATT
#define FEATURE_AF_INIT_AT_PREVIEW_START
#endif
#endif

#endif

#endif /* CUST_SKYCAM.h */


