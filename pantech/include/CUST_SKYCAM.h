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



(2)  ī�޶� ���� ��� kernel/userspace/android �α׸� ����

kernel/arch/arm/config/qsd8650-perf_defconfig �� �����Ѵ�.

	# CONFIG_MSM_CAMERA_DEBUG is not set (default)

CUST_SKYCAM.h ���� F_SKYCAM_LOG_PRINTK �� #undef �Ѵ�. 

	#define F_SKYCAM_LOG_PRINTK (default)

��� �ҽ� ���Ͽ��� F_SKYCAM_LOG_OEM �� ã�� �ּ� ó���Ѵ�. 
	���� �� ���, �ش� ���Ͽ� ���� SKYCDBG/SKYCERR ��ũ�θ� �̿��� 
	�޽������� Ȱ��ȭ ��Ų��. (user-space only)

��� �ҽ� ���Ͽ��� F_SKYCAM_LOG_CDBG �� ã�� �ּ� ó���Ѵ�. 
	���� �� ���, �ش� ���Ͽ� ���� CDBG ��ũ�θ� �̿��� �޽������� 
	Ȱ��ȭ ��Ų��. (user-space only)

��� �ҽ� ���Ͽ��� F_SKYCAM_LOG_VERBOSE �� ã�� �ּ� ó���Ѵ�.
	���� �� ���, �ش� ���Ͽ� ���� LOGV/LOGD/LOGI ��ũ�θ� �̿��� 
	�޽������� Ȱ��ȭ ��Ų��. (user-space only)




(4)  �ȸ��ν� ���� ��� ���� ȯ��

vendor/qcom/android-open/libcamera2/Android.mk �� �����Ѵ�.
	3rd PARTY �ַ�� ��� ���θ� �����Ѵ�.

	SKYCAM_FD_ENGINE := 0		������
	SKYCAM_FD_ENGINE := 1		�ö���� �ַ�� ���
	SKYCAM_FD_ENGINE := 2		��Ÿ �ַ�� ���

CUST_SKYCAM.h ���� F_SKYCAM_ADD_CFG_FACE_FILTER �� #undef �Ѵ�.
	�ȸ��ν� ��� ���� �������̽� ���� ���θ� �����Ѵ�.

libOlaEngine.so �� ���� libcamera.so �� ��ũ�ϹǷ� ���� ��� libcamera.so ��
ũ�Ⱑ �����Ͽ� ��ũ ������ �߻� �����ϸ�, �� ��� �Ʒ� ���ϵ鿡�� 
liboemcamera.so �� ������ �ٿ� libcamera.so �� ������ Ȯ���� �� �ִ�.

build/core/prelink-linux-arm-2G.map (for 2G/2G)
build/core/prelink-linux-arm.map (for 1G/3G)	

2010-10-04  �ö���� ���̺귯�� �� �ҽ��ڵ� ������Ʈ

*/	



/* ���� CS �μ������� �Һ��� �÷� �м��� ���� ���� PC ���α׷��� ����Ͽ� 
 * ī�޶� ���� �ð� ������ PC �� �����Ѵ�. 
 *
 * ���� ����� ���� Ŀ�ǵ� ��缭�� ��õǾ� �����Ƿ� ���� �ڵ���� ���� Ŀ�ǵ� 
 * ���� ��⿡ ���ԵǾ� ������, ���� Ŀ�ǵ� �� PC ���α׷��� ������� �ʰ� ����
 * ���α׷��� ����Ͽ�, �÷��� DIAG ��Ʈ�κ��� ���� �ð� ������ Ȯ���� �� �ִ�.
 *
 * ���� Ŀ�ǵ� ��缭 v10.35 ��� ����
 * PhoneInfoDisplay v4.0 ���α׷����� Ȯ��
 * ��缭�� ���α׷��� DS2�� �ڰ�ȣ ���ӿ��� ���� */

#define F_SKYCAM_FACTORY_PROC_CMD

/*========================================================
 * SKYCAM Lee Jitae 2010-11-09
 * ī�޶� ANR�߻��̳� Device�̻����� open fail�� �Ͼ� �� ��� �������� �������� ���� 
 * cameraservice connectó�� �� vfe reset�ڵ� �۾�
 *F_SKYCAM_INVALIDATE_CAMERA_CLIENT feature�� �̹� �� �־���
======================================================== */
/* ī�޶� ��ġ ���� OPEN �� ������ ��� (�ܼ� I2C ���� R/W ����, ī�޶� ������) 
 * ���� ó���� ���� �����Ѵ�. 
 *
 * ��ġ ������ OPEN �ϴ� �������� VFE �ʱ�ȭ ���� ī�޶� HW �ʱ�ȭ�� �̷�� 
 * ���µ�, HW �ʱ�ȭ�� ������ ��� VFE �� �ʱ�ȭ �� ���·� ���Եǰ� ����
 * OPEN �õ� �� HW �ʱ�ȭ�� �����Ѵ� �ϴ��� �̹� VFE �� �ʱ�ȭ�� �����̹Ƿ� 
 * VFE �ʱ�ȭ �� ������ �߻��Ѵ�.
 * 
 * ȣ����� : vfefn.vfe_init() -> sctrl.s_init()
 *
 * HW �ʱ�ȭ�� ������ ���, �̹� �ʱ�ȭ�� VFE �� RELEASE (vfe_release) ���� 
 * ���� ���� �õ� �� ���� �����ϵ��� �����Ѵ�. 
 *
 * ECLAIR ���������� ���� ���� ���� ó������ �ұ��ϰ� ������ ����Ǿ� ����
 * �ʰų� ���� �ϵ��� �̻��� �߻��� ��� ī�޶� ������ ANR ������ ���� 
 * ������ ����ǰ� ���� ����� �������� �����Ͽ� �������� �Ұ����ϴ�.
 *
 * ������ �� ������ ���, ISP �ʱ�ȭ �� ISP �� ������ ������ ���� �����ϴ� 
 * �������� 3�� �� POLLING �����ϸ�, �̷� ���� Ÿ�Ӿƿ� �߻��ϰ� ANR ������ 
 * �̾�����. �� ���� ���� ���� ī�޶� ������ �� ������ �����̶� �ϴ��� ANR 
 * ���� ���� ������ �� ���������� ����Ǿ����Ƿ� FRAMEWORK ���δ� �� ������ 
 * ���·� �����ǰ�, �̷� ���� ����� �������� ī�޶� ���� ���� �� "Unable to 
 * connect camera device" �˾��� �Բ� ������ ���Կ� �����Ѵ�.
 *
 * ISP �ʱ�ȭ �� ������ ��� ���� ������, ISP �� ���� ������ Ư�� �������͸� 
 * 1ȸ READ �ϰ� �� ������ ���, �̸� FRAMWORK �� ���� �������� �����Ͽ� 
 * ���������� ����ǵ��� �����Ѵ�. 
 *
 * ���� ISP ��ü�� �̻��� �߻��� ��쿡��, PROBE �ÿ� ���� �߻��Ͽ� �ش� 
 * ����̽� ���ϵ��� ������ �� �����Ƿ� FRAMWORK ���ο��� �Բ� ó�� �����ϴ�. */

#define F_SKYCAM_INVALIDATE_CAMERA_CLIENT


/* SKYCAM Lee Jitae 2010-11-12 
 *  Antina sensor Mt9p111���� ��ȭ ���� �߻�
 * OTP����̵� ���� 1�̶�� �ν��ϴ� ���۷��� ��Ƽ���� �����ϴ� ��������
 * ��Ȯ�� ���� Antina���� ������ ���� ����
 * ���� ���� ������ ��ȭ ������ �߻��ϴ� ����� ���´� (EF13S���� ���� ������) EF18K������ �Ѱ� �߰��ؼ� ���� ������ ��
 */

#define F_SKYCAM_SENSOR_BUG

/* ī�޶� ���� GPIO ���� �����Ѵ�. 
 *
 * �� 12 ������ ������ �������� ���� 8��Ʈ�� ����Ѵ�. 
 *
 * ī�޶� HW ���� ������ SLEEP ���� ���� �浹 �� HW �̻� ������ �����ϱ� ����
 * SLEEP_CONFIGS[] ���� ī�޶� ���� GPIO ���� ��� ���ܽ�Ų��.
 *
 * VCM/FLUID ���� GPIO ���� ������� �����Ƿ� ���� �κ��� �����Ѵ�.
 *
 * ���� ���� ����
 * AMSS/products/7x30/core/systemdrivers/tlmm/src/bsp/7x30/TLMMBsp.c
 * AMSS/products/7x30/core/systemdrivers/tlmm/src/bsp/7x30/TLMMBspMsm.h */

#define F_SKYCAM_HW_GPIO

/* HW I2C ������ Ŭ���� �����Ѵ�. 
 *
 * MT9P111 �� ���, �ʱ�ȭ �� ���� ���� ���� �� WRITE �ؾ� �� ���������� ���� 
 * �����Ƿ� ���� ����� ���� �ִ� Ŭ���� ����Ѵ�. */

#define F_SKYCAM_CUST_I2C_CLK_FREQ

/*----------------------------------------------------------------------------*/
/*  MODEL-SPECIFIC                                                            */
/*  EF18K ���� ����Ǵ� �Ǵ� EF18K ������ ������ FEATURE ���                 */
/*----------------------------------------------------------------------------*/

/*
 * ���� VREG ������� ���԰� i2c�� ���õǾ� ����  
 *  F_SKYCAM_VREG�� F_SKYCAM_I2C_INIT�� �ٲ� ����
 */

#define F_SKYCAM

/* "ro.product.device" ���� �� �������� ���� �ڵ� �������� ���� ���� ī�޶�
 * ���� �ڵ�鿡���� �� ���� ���� �ʰų� 'TARGET_QSD8250' ���� �����Ѵ�. 
 * �ý��� ���� "ro.product.device" �� ������ ������ ��� "qsd8250_surf" �̰�,
 * EF18K �� ��� "ef18k" ���� �����Ǿ� �ִ�. */

#define F_SKYCAM_CUST_TARGET_TYPE_7X30


/* SKYCAM Lee Jitae 2010-12-11 
 * ���� ������ LOGV/LOGD/LOGI �޼��� on off
*/

#define F_SKYCAM_LOG_VERBOSE

/* Ŀ�� ���� �ڵ忡�� SKYCDBG/SKYCERR ��ũ�θ� ����Ͽ� ��µǴ� �޽�������
 * Ȱ��ȭ ��Ų��. 
 * kernel/arch/arm/mach-msm/include/mach/camera.h */

#define F_SKYCAM_LOG_PRINTK

/* ���� ������ SKYCDBG/SKYCERR �޼��� on off	
  */

 #define F_SKYCAM_LOG_OEM  

 /* QUALCOMM ������ �ڵ忡 ����� �� �޽����� �߰��Ѵ�. */

#define F_SKYCAM_LOG_DEBUG



/* ��ȭ ����/���� ��, ȿ������ �ش� ������ ���Ͽ� ��ȭ�Ǵ� ������ �ӽ� �����Ѵ�.
 *
 * ��ȭ ���� �ÿ��� ȿ���� ��� �Ϸ� �ñ��� POLLING �� ��ȭ�� �����ϰ�,
 * �Ϸ� �ÿ��� ��ȭ �Ϸ� ���� 800ms ���� DELAY ���� �� ȿ������ ����Ѵ�.
 *
 * R4070 ������ ��ȭ ���� �� ����� Ŭ���� �ʱ�ȭ�Ǵ� �ð��� ���� �ɷ�
 * �ټ��� �������� DROP �Ǹ鼭 ��ȭ �������� ���� �� ���ɼ��� ���������,
 * R4075 ���� �� �̽��� �����Ǹ鼭 ��ȭ ���� �� ȿ������ 100% Ȯ���� �����ǰ�, 
 * ���� �� 80% �̻� Ȯ���� �����ȴ�.
 *
 * F_SKYCAM_TODO, QUALCOMM ���� Ȯ�� �� �����ؾ� �Ѵ�. (SR#307114) */

#define F_SKYCAM_QBUG_REC_BEEP_IS_RECORDED


/* �ٿ� ����� �ϱ� ���� �Լ��� ����ϱ� ���ؼ�,  ĸ�� ȭ���� �����ִ� ���� ĸ�� ȭ����  Ʋ�� ���� �����ϱ� ���ؼ�
 * ������� ������ �����������並 �����.
 * IPL ���̺귯���� �ִ� downsize�� �̿��ؼ� ���δ�.
 * ���� SR���� �� -> ���� ��ġ�� ���� �� �� �ֵ��� ���� �ޱ� ���ؼ� ���� ��
 */

/* #define F_SKYCAM_SNAPSHOT_PREVIEW  */


/* EF18K ���� ���Ǵ� ī�޶� ���� ���̺� (�ִ� ��� ���� �ػ�, �ִ� ������ 
 * �ػ�, AF ���� ����) �� �����Ѵ�. */

#define F_SKYCAM_CUST_SENSOR_TYPE


/* ī�޶� IOCTL Ŀ�ǵ� MSM_CAM_IOCTL_SENSOR_IO_CFG �� Ȯ���Ѵ�. 
 *
 * EF10S ���� �߰��� ��� �� SOC ī�޶� �������� �ʰ� �̱����� �κе��� 
 * ���� �� �߰� �����Ѵ�. */

#define F_SKYCAM_CUST_MSM_CAMERA_CFG

/* ZOOM LEVEL �� 51 �̻��ΰ�� wide size ���� preview frame �� width/height ratio�� �޶�����.
 * zoom level 0~50 : 1.6666
 * zoom level 51 �̻� : width �� Ȯ�� ��.
 * zoom level 60 : 1.3333
 * R5040���� Qualcomm SBA �޾Ƽ� patch ����. ���� release�� ���� ���� Ȯ�� �� ���� �Ѵ�.
 */

#define F_SKYCAM_QBUG_ZOOM_MORE51



/* SKYCAM Lee Jitae 2010-10-05 
 * QualcommCameraHardware.cpp���� setParameters�� VT�� ī�޶� ���õ� ������ �ȴ�. VT���� �ʿ���� ������ ���ϱ� ���ؼ�
 * ī�޶��϶��� �Ķ���� ������ �ϱ� ���ؼ� ������ �κ�
 * EF18K�ǰ�� �ʿ����� ���� ������ ifndef�� ���� �ξ���.
 */


#define F_SKYCAM_CAMERA_SETTING_ONLY

/* SKYCAM_PSJ_0100331 : ī�޶� Ʃ�� ���� �ε带 ����... */
/* #define F_SKYCAM_SENSOR_TUNEUP */
#ifdef F_SKYCAM_SENSOR_TUNEUP
/* #define F_SKYCAM_TUP_LOAD_FILE */
/* #define MT9P111_TUP_FNAME       "/sdcard/camsensor_mt9p111_tuneup_reg.txt" */
#endif

/* SKYCAM 2010-10-21
 * 2010-10-26 Qualcomm���� ���� �ڵ带 �޾Ƽ� ����
 * GPS, JPEG THUMBNAIL, JPEG_EXIF ���� fail�� �߻��Ѵ�.
 * ���� MT9P111�������� ������ ���� ��쿡�� checkpreview�� failed�� �߻�. 
 * Timeout Error�� �߻��Ѵ�. �̷��� ������WAIT_FOR_COMMAND_TO_COMPLETE�� 1.5�ʷ� �����Ǿ�̴�.
 * WAIT_FOR_COMMAND_TO_COMPLETE�� ��� EF13S, EF13L�� ���� 5�ʷ� �����Ǿ�  �ִ�.
 * ������� �������δ� ������������ ���� ����ȭ ������ �ð��� ���� �ɸ��� �����̶�� �ľ��ϰ� �ִ�.
 */

#define F_SKYCAM_QBUG_CTS



/* SKYCAM Lee Jitae 2010-11-27
 * �Կ��� ������� ���� ��Ȳ���� Stop preview�� �Ͽ� �޸𸮰� �����Ǵ� ��Ȳ�� �߻�, NULL POINTER REFERENCE������ ���ؼ� ���� ��������
 * �װ� ���Ŀ��� �̻� ȭ�� ǥ��, ���� �ȳ� ���� ������ �߻��մϴ�.
 */

#define F_SKYCAM_QBUG_SNAPSHOT_RELEASE_INTERRUPT


/* SKYCAM_PSJ_110302
 * ���� FEATURE������ ����ϴ� ������ IPL�Լ��� ����ϱ� ���� IPL LIB�ε�
 * ipl_reflect �׽�Ʈ �Ϸ�
*/

#define F_SKYCAM_USE_IPLLIB


/*----------------------------------------------------------------------------*/
/*  SENSOR CONFIGURATION                                                      */
/*  �� �� ��� ����(ISP)�� ���� ����/�߰��� FEATURE ���                    */
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
 *  Camcoder�� ��� CAF�� ON, OFF�� �� �� �ִ�
 * EF18K�� ��� �ػ�� ������ �߻��ؼ� ����
 */

/* #define F_SKYCAM_FIX_CFG_CAF */
#else

/* SKYCAM Lee Jitae 2010-09-25
 * VPE�ǻ������ ���ؼ� ���� �������� �ö���� �ʴ� �������� �߻�
 * mDisEnabled�� �̻������ ���� �ذ� 
 * mDisEnabled�� ��쿡�� BayerSensor�� ��쿡�� Enable�Ѵ�.
 * mDisEnabled �� DIS(Digital Image Stabilization) ����� ON- OFF�ϴ� ���̴�.
 */

/* #define F_SKYCAM_VIDEO_FRAME_VPE_DIS */


#endif


/* �÷��� LED ������ ���� �������̽��� �����Ѵ�.
 *
 * QUALCOMM ������ ������ IOCTL (MSM_CAM_IOCTL_FLASH_LED_CFG) Ŀ�ǵ带 
 * ����Ͽ� �����Ǿ� ������, PMIC ������ ����ϴ� LED ����̹��� �����Ѵ�.
 * EF10S ������ �̸� ������� ������, MSM_CAM_IOCTL_SENSOR_IO_CFG �� Ȯ���Ͽ�
 * �����Ѵ�.
 *
 * AUTO ���� ������ ���, ������ �� ��쿡�� AF ���� �� AF/AE �� ����
 * ��� ON �ǰ�, ���� ������ �������� �� �� �� ON �ȴ�. */

#define F_SKYCAM_FIX_CFG_LED_MODE

/* KDDI ??Mirach_J???�촬방�???RED LED�?ON/OFF ?????�는 ?�터?�이?��? 추�?
 *  startPreview()?�에 ON ?�키�?  stopPreview()?�에 OFF ?�키?�록 변�? */
#if defined(F_SKYCAM_TARGET_J_MASAI_E) && (BOARD_VER >= WS10)
#define F_SKYCAM_ADD_CFG_RED_LED_MODE
#endif

/* SKYCAM Lee Jitae 2010-09-25
 * OTP����� �ҷ����� ���̱� ���� �޸� zone�� 2���� detecting�Ͽ� 
 * ���� ����ؾ� �� zone�� ã�Ƴ� �� �ش� zone�� �޸𸮸� ��� �� �� �ֵ��� �߰�
 * WS02������ ���� ����
*/

#define F_SKYCAM_ADD_CFG_OTP_MODULE

/* 
 * 800*480, 640*480 �� ������ �ٸ� �̹��� ������� ���� ���(90��)�� �ؼ� ���ؼ� ���� �Կ��� �� ��� �̹����� ����
 * �̹��� ���� ������ ��������. ���� �⺻ ī�޶󿡼��� 3M 1.4�迡�� ������ �����ϴ� ���� Ȯ����
 * �̷��� �̹����� ������ ��츦 �۾��� ��
 */

#define F_SKYCAM_QBUG_ROTATION_ZOOM

/* 
 * MEDIA_RECORDER_INFO_FILESIZE_PROGRESS CallBack, Only Use KT Phone 
 * KT�������� ���� ���� ����� ����ϴµ� ���� ����� APP�� �˷��ֱ� ���ؼ�
 * �߰� 
 */

/* #define F_SKYCAM_ADD_EVT_RECSIZE */



/* ES01 ���� ���̳ʸ� ���� Feature  Lee Jitae 2010-10-13 */
/* EF18K��WS02�� �÷� H/W ������ power sequence�� ����  �ʾ���
 *  �׷��� Power sequence���� �ذ��� ���� Feature
 * EF18K WS02�� �÷�� ���ĸ� ���� ES1�� ���Ĵ� �����ش�.
 */

/* #define F_SKYCAM_EF18K_ES01 */


   /* SKYCAM_PSJ_100622
   * BGM�� Ʋ����� ���¿��� ī�޶� �����Կ��� �Կ��� ������ ������ ������ 
   * get_duration�Լ��� �̿��Ͽ� ������ ���̸� ���� �� �׸�ŭ usleep�� �ָ� 
   * ������ ������ �������� �Կ����� �� ���� �Ŀ� ���� ������ ������
   * ������ ������ ������ ���ɻ��� �����̹Ƿ� �ϴ� �츮�� �������� �ʴ� �ɷ� �Ѵ�
  */
  
  /* SKYCAM Lee Jitae 2010-10-22
   * VGA�� ������̵带 ���� �� ���尡 �������� ������ �Ͼ��. EF13���� ���� �Կ����� ������ ���⳪
   * EF18K������ ���� �߻��� ������ �ʾҴ�. �ణ ������ ���� ���� ����.
   * �ϴ� EF13S�� ���� ���� ��⿡ ǥ���Ѵ�.
   */

/* #define F_SKYCAM_FIX_SNAPSHOT_BEEP_SPLIT */


/* SKYCAM Lee Jita3 2010-11-23 
 * 720P���� �����䰡 1280*720�� ���� �ʴ´�.
 * ǥ�� �� �� �ִ� ������ ������� �������ش�.
 */

#define F_SKYCAM_720P_PREVIEW

/* SKYCAM Lee Jitae 2010-12-10
 * MARKET CAMERA APP�� I'M IN APP�� POSTVIEW�� ������.
 * ������ Set_demension�� �ϸ� POSTVIEW����� ���ϴ� ���õ��� �ִ�.
 * �̷�  ���õ��� CAMERA_SET_PARM_DIMENSION ���� ���� ����ȴ�.
 * SKYCAM������ 1.3333���� ������ ��ȭ�� ���� ������ ������ ��������.
 */

 #define F_SKYCAM_QBUG_MARKET_APP


 /* SKYCAM Lee JItae 2010-11-23
  * WVGA�� ��� ���� �ϸ� POSTVIEW�� Ŀ����.
  * Qualcomm���� ���� ��ġ ���� �ڵ�
  */

  #define F_SKYCAM_QBUG_WVGA_ZOOM


  /* �Կ���/��ȭ�� ��� �߿� ������ ����� ���, CLIENT �Ҹ� �ÿ� �ش� �Կ���/
   * ��ȭ�� ������Ʈ�� ������ disconnect/clear �Ǹ鼭 MEDIA SERVER �� �״� ����
   * �����Ѵ�. */

#define F_SKYCAM_QBUG_STOP_CAMERA_SOUND


/* 2010-10-06 
 *Continous AF �� �׽�Ʈ �ϱ� ���� �۾� �� ���� 
 */
/* #define F_SKYCAM_CAF
*/

/* oen exif tag ���� */

#define F_SKYCAM_OEM_EXIF_TAG


/* ������ ��ȭ ����/���Ḧ ������ �ݺ��ϰų�, �̾����� ������ ���¿��� �����Կ�
 * ���� �Կ��� ���, MediaPlayer �� �������ϸ鼭 HALT �߻��Ѵ�.
 *
 * MediaPlayer �� ���, ������ ������ ��� �߿� �� �ٽ� ��� �õ��� ��� 100%
 * �������ϹǷ� ���� ������ �����Ͽ� ����ؾ� �� ���, �ݵ�� ���� ����� �Ϸ�
 * �Ǿ����� ���θ� Ȯ�� �� ����ؾ� �Ѵ�. */
/* #define F_SKYCAM_QBUG_SKIP_CAMERA_SOUND */


/*
AT&T's model has a option that can be turn shutter sound on or off.
*/

#define F_SKYCAM_SET_SHUTTER_SOUND
/*
ķ�ڴ� �������� ���������� �÷��� �� �� �������ִ� ������ �߰�.
*/
#define F_SKYCAM_DELAY_ON_REC_STOP_SOUND 


/*
	QE1�� �� ���� ���� ���� (2010/10/18)

	Camera preview ȭ�鿡���� ������ �ȸ����� ���� �� ȭ���� ��������
	-->  Auto focus ���� ���� snapshot mode ������ 100 msec ������ �߰�
*/


#define F_SKYCAM_BUG_PATCH_POSTVIEW_FOCUS


/*#define F_SKYCAM_CHECK_IS_INIT_SEQ_DONE*/  /* it skip preview mode process when initiating sensor */
#define F_SKYCAM_SAME_FOCUS_STEP_SKIP /* it skip focus mode setting when previous and current focus mode are same */

/*----------------------------------------------------------------------------*/
/*  MODEL-SPECIFIC CONSTANTS                                                  */
/*  �� ���� ��� ����                                       */
/*----------------------------------------------------------------------------*/


#ifdef F_SKYCAM_FACTORY_PROC_CMD
#define C_SKYCAM_FNAME_FACTORY_PROC_CMD	"/data/.factorycamera.dat"
#endif

/* ���� ������ �ּ�/�ִ� ��Ŀ�� �����̴�. */
#ifdef F_SKYCAM_FIX_CFG_FOCUS_STEP
#define C_SKYCAM_MIN_FOCUS_STEP 0	/* ���Ѵ� (default) */
#define C_SKYCAM_MAX_FOCUS_STEP 9	/* ��ũ�� */
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


/* Brightness ���� */

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


