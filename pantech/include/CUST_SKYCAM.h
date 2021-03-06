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



(2)  카메라 관련 모든 kernel/userspace/android 로그를 제거

kernel/arch/arm/config/qsd8650-perf_defconfig 를 수정한다.

	# CONFIG_MSM_CAMERA_DEBUG is not set (default)

CUST_SKYCAM.h 에서 F_SKYCAM_LOG_PRINTK 을 #undef 한다. 

	#define F_SKYCAM_LOG_PRINTK (default)

모든 소스 파일에서 F_SKYCAM_LOG_OEM 을 찾아 주석 처리한다. 
	선언 된 경우, 해당 파일에 사용된 SKYCDBG/SKYCERR 매크로를 이용한 
	메시지들을 활성화 시킨다. (user-space only)

모든 소스 파일에서 F_SKYCAM_LOG_CDBG 를 찾아 주석 처리한다. 
	선언 된 경우, 해당 파일에 사용된 CDBG 매크로를 이용한 메시지들을 
	활성화 시킨다. (user-space only)

모든 소스 파일에서 F_SKYCAM_LOG_VERBOSE 를 찾아 주석 처리한다.
	선언 된 경우, 해당 파일에 사용된 LOGV/LOGD/LOGI 매크로를 이용한 
	메시지들을 활성화 시킨다. (user-space only)




(4)  안면인식 관련 기능 빌드 환경

vendor/qcom/android-open/libcamera2/Android.mk 를 수정한다.
	3rd PARTY 솔루션 사용 여부를 결정한다.

	SKYCAM_FD_ENGINE := 0		미포함
	SKYCAM_FD_ENGINE := 1		올라웍스 솔루션 사용
	SKYCAM_FD_ENGINE := 2		기타 솔루션 사용

CUST_SKYCAM.h 에서 F_SKYCAM_ADD_CFG_FACE_FILTER 를 #undef 한다.
	안면인식 기능 관련 인터페이스 포함 여부를 결정한다.

libOlaEngine.so 를 기존 libcamera.so 에 링크하므로 기존 대비 libcamera.so 의
크기가 증가하여 링크 오류가 발생 가능하며, 이 경우 아래 파일들에서 
liboemcamera.so 의 영역을 줄여 libcamera.so 의 영역을 확보할 수 있다.

build/core/prelink-linux-arm-2G.map (for 2G/2G)
build/core/prelink-linux-arm.map (for 1G/3G)	

2010-10-04  올라웍스 라이브러리 및 소스코드 업데이트

*/	



/* 내수 CS 부서에서는 소비자 시료 분석을 위해 별도 PC 프로그램을 사용하여 
 * 카메라 구동 시간 정보를 PC 로 추출한다. 
 *
 * 구현 방법은 공정 커맨드 사양서에 명시되어 있으므로 관련 코드들은 공정 커맨드 
 * 관련 모듈에 포함되어 있으나, 공정 커맨드 용 PC 프로그램을 사용하지 않고 별도
 * 프로그램을 사용하여, 시료의 DIAG 포트로부터 구동 시간 정보를 확인할 수 있다.
 *
 * 공정 커맨드 사양서 v10.35 기반 구현
 * PhoneInfoDisplay v4.0 프로그램으로 확인
 * 사양서와 프로그램은 DS2팀 박경호 선임에게 문의 */

#define F_SKYCAM_FACTORY_PROC_CMD

/*========================================================
 * SKYCAM Lee Jitae 2010-11-09
 * 카메라 ANR발생이나 Device이상으로 open fail이 일어 날 경우 정상적인 재진입을 위한 
 * cameraservice connect처리 및 vfe reset코드 작업
 *F_SKYCAM_INVALIDATE_CAMERA_CLIENT feature는 이미 들어가 있었음
======================================================== */
/* 카메라 장치 파일 OPEN 에 실패한 경우 (단순 I2C 버스 R/W 오류, 카메라 미장착) 
 * 오류 처리를 위해 수정한다. 
 *
 * 장치 파일을 OPEN 하는 과정에서 VFE 초기화 이후 카메라 HW 초기화가 이루어 
 * 지는데, HW 초기화에 실패할 경우 VFE 는 초기화 된 상태로 남게되고 다음
 * OPEN 시도 시 HW 초기화에 성공한다 하더라도 이미 VFE 가 초기화된 상태이므로 
 * VFE 초기화 시 오류가 발생한다.
 * 
 * 호출순서 : vfefn.vfe_init() -> sctrl.s_init()
 *
 * HW 초기화에 실패할 경우, 이미 초기화된 VFE 를 RELEASE (vfe_release) 시켜 
 * 다음 열기 시도 시 정상 동작하도록 수정한다. 
 *
 * ECLAIR 버전에서는 위와 같은 에러 처리에도 불구하고 센서가 연결되어 있지
 * 않거나 센서 하드웨어에 이상이 발생한 경우 카메라 응용이 ANR 오류로 인해 
 * 비정상 종료되고 이후 재부팅 전까지는 지속하여 재진입이 불가능하다.
 *
 * 센서가 비 정상인 경우, ISP 초기화 시 ISP 와 센서를 프리뷰 모드로 설정하는 
 * 과정에서 3초 간 POLLING 수행하며, 이로 인해 타임아웃 발생하고 ANR 오류로 
 * 이어진다. 비 정상 종료 이후 카메라 재진입 시 센서가 정상이라 하더라도 ANR 
 * 오류 이후 응용이 비 정상적으로 종료되었으므로 FRAMEWORK 내부는 비 정상인 
 * 상태로 유지되고, 이로 인해 재부팅 전까지는 카메라 응용 진입 시 "Unable to 
 * connect camera device" 팝업과 함께 무조건 진입에 실패한다.
 *
 * ISP 초기화 시 프리뷰 모드 설정 이전에, ISP 를 통해 센서의 특정 레지스터를 
 * 1회 READ 하고 비 정상일 경우, 이를 FRAMWORK 을 통해 응용으로 전달하여 
 * 정상적으로 종료되도록 수정한다. 
 *
 * 또한 ISP 자체에 이상이 발생한 경우에도, PROBE 시에 오류 발생하여 해당 
 * 디바이스 파일들을 생성할 수 없으므로 FRAMWORK 내부에서 함께 처리 가능하다. */

#define F_SKYCAM_INVALIDATE_CAMERA_CLIENT


/* SKYCAM Lee Jitae 2010-11-12 
 *  Antina sensor Mt9p111에서 열화 현상 발생
 * OTP렌즈쉐이딩 값을 1이라고 인식하는 레퍼런스 볼티지를 조절하는 레지스터
 * 정확한 값은 Antina에서 가르쳐 주지 않음
 * 적용 되지 않으면 열화 현상이 발생하는 모듈이 나온다 (EF13S에서 많이 나왔음) EF18K에서는 한개 발견해서 먼저 적용을 함
 */

#define F_SKYCAM_SENSOR_BUG

/* 카메라 관련 GPIO 들을 설정한다. 
 *
 * 총 12 라인의 데이터 버스에서 상위 8비트만 사용한다. 
 *
 * 카메라 HW 종료 시점과 SLEEP 진입 시점 충돌 시 HW 이상 동작을 방지하기 위해
 * SLEEP_CONFIGS[] 에서 카메라 관련 GPIO 들은 모두 제외시킨다.
 *
 * VCM/FLUID 관련 GPIO 들은 사용하지 않으므로 설정 부분을 삭제한다.
 *
 * 관련 파일 참고
 * AMSS/products/7x30/core/systemdrivers/tlmm/src/bsp/7x30/TLMMBsp.c
 * AMSS/products/7x30/core/systemdrivers/tlmm/src/bsp/7x30/TLMMBspMsm.h */

#define F_SKYCAM_HW_GPIO

/* HW I2C 버스의 클럭을 수정한다. 
 *
 * MT9P111 의 경우, 초기화 및 각종 설정 변경 시 WRITE 해야 할 레지스터의 수가 
 * 많으므로 성능 향상을 위해 최대 클럭을 사용한다. */

#define F_SKYCAM_CUST_I2C_CLK_FREQ

/*----------------------------------------------------------------------------*/
/*  MODEL-SPECIFIC                                                            */
/*  EF18K 에만 적용되는 또는 EF18K 에서만 검증된 FEATURE 목록                 */
/*----------------------------------------------------------------------------*/

/*
 * 현재 VREG 헤더파일 포함과 i2c에 관련되어 있음  
 *  F_SKYCAM_VREG와 F_SKYCAM_I2C_INIT로 바꿀 예정
 */

#define F_SKYCAM

/* "ro.product.device" 설정 값 변경으로 인한 코드 오동작을 막기 위해 카메라
 * 관련 코드들에서는 이 값을 읽지 않거나 'TARGET_QSD8250' 으로 고정한다. 
 * 시스템 설정 "ro.product.device" 는 릴리즈 버전의 경우 "qsd8250_surf" 이고,
 * EF18K 의 경우 "ef18k" 으로 설정되어 있다. */

#define F_SKYCAM_CUST_TARGET_TYPE_7X30


/* SKYCAM Lee Jitae 2010-12-11 
 * 유저 영역의 LOGV/LOGD/LOGI 메세지 on off
*/

#define F_SKYCAM_LOG_VERBOSE

/* 커널 영역 코드에서 SKYCDBG/SKYCERR 매크로를 사용하여 출력되는 메시지들을
 * 활성화 시킨다. 
 * kernel/arch/arm/mach-msm/include/mach/camera.h */

#define F_SKYCAM_LOG_PRINTK

/* 유저 영역의 SKYCDBG/SKYCERR 메세지 on off	
  */

 #define F_SKYCAM_LOG_OEM  

 /* QUALCOMM 릴리즈 코드에 디버그 용 메시지를 추가한다. */

#define F_SKYCAM_LOG_DEBUG



/* 녹화 시작/종료 시, 효과음이 해당 동영상 파일에 녹화되는 문제를 임시 수정한다.
 *
 * 녹화 시작 시에는 효과음 재생 완료 시까지 POLLING 후 녹화를 시작하고,
 * 완료 시에는 녹화 완료 이후 800ms 고정 DELAY 적용 후 효과음을 재생한다.
 *
 * R4070 까지는 녹화 시작 시 오디오 클럭이 초기화되는 시간이 오래 걸려
 * 다수의 프레임이 DROP 되면서 녹화 시작음이 녹음 될 가능성이 희박했으나,
 * R4075 에서 이 이슈가 수정되면서 녹화 시작 시 효과음이 100% 확률로 녹음되고, 
 * 종료 시 80% 이상 확률로 녹음된다.
 *
 * F_SKYCAM_TODO, QUALCOMM 수정 확인 후 삭제해야 한다. (SR#307114) */

#define F_SKYCAM_QBUG_REC_BEEP_IS_RECORDED


/* 다운 사이즈를 하기 위한 함수를 사용하기 위해서,  캡쳐 화면을 보여주는 곳과 캡쳐 화면이  틀린 것을 수정하기 위해서
 * 썸네일을 가지고 스냅샷프리뷰를 만든다.
 * IPL 라이브러리에 있는 downsize를 이용해서 줄인다.
 * 현재 SR진행 중 -> 퀄컴 패치에 적용 될 수 있도록 수정 받기 위해서 진행 중
 */

/* #define F_SKYCAM_SNAPSHOT_PREVIEW  */


/* EF18K 에서 사용되는 카메라 정보 테이블 (최대 출력 가능 해상도, 최대 스냅샷 
 * 해상도, AF 지원 여부) 을 수정한다. */

#define F_SKYCAM_CUST_SENSOR_TYPE


/* 카메라 IOCTL 커맨드 MSM_CAM_IOCTL_SENSOR_IO_CFG 를 확장한다. 
 *
 * EF10S 에서 추가한 기능 및 SOC 카메라를 감안하지 않고 미구현된 부분들을 
 * 수정 및 추가 구현한다. */

#define F_SKYCAM_CUST_MSM_CAMERA_CFG

/* ZOOM LEVEL 이 51 이상인경우 wide size 에서 preview frame 의 width/height ratio가 달라진다.
 * zoom level 0~50 : 1.6666
 * zoom level 51 이상 : width 만 확대 됨.
 * zoom level 60 : 1.3333
 * R5040에서 Qualcomm SBA 받아서 patch 적용. 이후 release에 적용 여부 확인 후 삭제 한다.
 */

#define F_SKYCAM_QBUG_ZOOM_MORE51



/* SKYCAM Lee Jitae 2010-10-05 
 * QualcommCameraHardware.cpp에서 setParameters는 VT와 카메라 관련된 세팅이 된다. VT에서 필요없는 세팅을 피하기 위해서
 * 카메라일때만 파라메터 세팅을 하기 위해서 구현한 부분
 * EF18K의경우 필요하지 않은 세팅은 ifndef로 막아 두었다.
 */


#define F_SKYCAM_CAMERA_SETTING_ONLY

/* SKYCAM_PSJ_0100331 : 카메라 튜닝 파일 로드를 위해... */
/* #define F_SKYCAM_SENSOR_TUNEUP */
#ifdef F_SKYCAM_SENSOR_TUNEUP
/* #define F_SKYCAM_TUP_LOAD_FILE */
/* #define MT9P111_TUP_FNAME       "/sdcard/camsensor_mt9p111_tuneup_reg.txt" */
#endif

/* SKYCAM 2010-10-21
 * 2010-10-26 Qualcomm에서 수정 코드를 받아서 수정
 * GPS, JPEG THUMBNAIL, JPEG_EXIF 에서 fail이 발생한다.
 * 현재 MT9P111센서에서 조도가 낮을 경우에는 checkpreview가 failed가 발생. 
 * Timeout Error가 발생한다. 이러한 이유는WAIT_FOR_COMMAND_TO_COMPLETE가 1.5초로 설정되어서이다.
 * WAIT_FOR_COMMAND_TO_COMPLETE의 경우 EF13S, EF13L의 경우는 5초로 설정되어  있다.
 * 현재까지 원인으로는 저조도에서는 센서 안정화 까지의 시간이 많이 걸리기 때문이라고 파악하고 있다.
 */

#define F_SKYCAM_QBUG_CTS



/* SKYCAM Lee Jitae 2010-11-27
 * 촬영이 종료되지 않은 상황에서 Stop preview를 하여 메모리가 해제되는 상황이 발생, NULL POINTER REFERENCE오류로 인해서 각종 서버들이
 * 죽고 이후에는 이상 화면 표시, 사운드 안남 등의 옲들이 발생합니다.
 */

#define F_SKYCAM_QBUG_SNAPSHOT_RELEASE_INTERRUPT


/* SKYCAM_PSJ_110302
 * 기존 FEATURE폰에서 사용하던 퀄컴의 IPL함수를 사용하기 위해 IPL LIB로드
 * ipl_reflect 테스트 완료
*/

#define F_SKYCAM_USE_IPLLIB


/*----------------------------------------------------------------------------*/
/*  SENSOR CONFIGURATION                                                      */
/*  모델 별 사용 센서(ISP)를 위해 수정/추가된 FEATURE 목록                    */
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
 *  Camcoder일 경우 CAF를 ON, OFF를 할 수 있다
 * EF18K의 경우 해상력 문제가 발생해서 제외
 */

/* #define F_SKYCAM_FIX_CFG_CAF */
#else

/* SKYCAM Lee Jitae 2010-09-25
 * VPE의사용으로 인해서 비디오 프레임이 올라오지 않는 문제점이 발생
 * mDisEnabled의 미사용으로 문제 해결 
 * mDisEnabled의 경우에는 BayerSensor의 경우에만 Enable한다.
 * mDisEnabled 은 DIS(Digital Image Stabilization) 기능을 ON- OFF하는 것이다.
 */

/* #define F_SKYCAM_VIDEO_FRAME_VPE_DIS */


#endif


/* 플래쉬 LED 설정을 위한 인터페이스를 수정한다.
 *
 * QUALCOMM 에서는 별도의 IOCTL (MSM_CAM_IOCTL_FLASH_LED_CFG) 커맨드를 
 * 사용하여 구현되어 있으며, PMIC 전원을 사용하는 LED 드라이버를 제공한다.
 * EF10S 에서는 이를 사용하지 않으며, MSM_CAM_IOCTL_SENSOR_IO_CFG 을 확장하여
 * 구현한다.
 *
 * AUTO 모드로 설정할 경우, 저조도 일 경우에만 AF 수행 중 AF/AE 를 위해
 * 잠시 ON 되고, 실제 스냅샷 시점에서 한 번 더 ON 된다. */

#define F_SKYCAM_FIX_CFG_LED_MODE

/* KDDI ??Mirach_J???꾩눋諛⑹???RED LED瑜?ON/OFF ?????덈뒗 ?명꽣?섏씠?ㅻ? 異붽?
 *  startPreview()?쒖뿉 ON ?쒗궎怨?  stopPreview()?쒖뿉 OFF ?쒗궎?꾨줉 蹂�寃? */
#if defined(F_SKYCAM_TARGET_J_MASAI_E) && (BOARD_VER >= WS10)
#define F_SKYCAM_ADD_CFG_RED_LED_MODE
#endif

/* SKYCAM Lee Jitae 2010-09-25
 * OTP모듈의 불량률을 줄이기 위해 메모리 zone의 2개를 detecting하여 
 * 현재 사용해야 될 zone을 찾아낸 후 해당 zone의 메모리를 사용 할 수 있도록 추가
 * WS02차에서 적용 예정
*/

#define F_SKYCAM_ADD_CFG_OTP_MODULE

/* 
 * 800*480, 640*480 을 제외한 다른 이미지 사이즈에서 세로 모드(90도)로 해서 줌해서 사진 촬영을 한 경우 이미지가 깨짐
 * 이미지 왼쪽 끝쪽이 깨어진다. 구글 기본 카메라에서는 3M 1.4배에서 깨지기 시작하는 것을 확인함
 * 이러한 이미지가 깨지는 경우를 작업한 것
 */

#define F_SKYCAM_QBUG_ROTATION_ZOOM

/* 
 * MEDIA_RECORDER_INFO_FILESIZE_PROGRESS CallBack, Only Use KT Phone 
 * KT폰에서는 비디오 파일 사이즈를 기록하는데 파일 사이즈를 APP에 알려주기 위해서
 * 추가 
 */

/* #define F_SKYCAM_ADD_EVT_RECSIZE */



/* ES01 차용 바이너리 제어 Feature  Lee Jitae 2010-10-13 */
/* EF18K의WS02차 시료 H/W 문제및 power sequence가 맞지  않았음
 *  그러한 Power sequence문제 해결을 위한 Feature
 * EF18K WS02차 시료는 피쳐를 막고 ES1차 이후는 열어준다.
 */

/* #define F_SKYCAM_EF18K_ES01 */


   /* SKYCAM_PSJ_100622
   * BGM을 틀어놓은 상태에서 카메라 연속촬영시 촬영음 깨지는 현상이 있으며 
   * get_duration함수를 이용하여 음원의 길이를 얻은 후 그만큼 usleep을 주면 
   * 깨지는 현상이 없어지나 촬영음이 다 끝난 후에 찍은 사진이 보여짐
   * 음원이 깨지는 현상은 성능상의 문제이므로 일단 우리가 수정하지 않는 걸로 한다
  */
  
  /* SKYCAM Lee Jitae 2010-10-22
   * VGA로 폴라로이드를 찍을 때 사운드가 뭉개지는 현상이 일어난다. EF13에서 연속 촬영에서 문제가 생기나
   * EF18K에서는 문제 발생이 보이지 않았다. 약간 느려진 듯한 감은 있음.
   * 일단 EF13S에 나온 것은 상기에 표기한다.
   */

/* #define F_SKYCAM_FIX_SNAPSHOT_BEEP_SPLIT */


/* SKYCAM Lee Jita3 2010-11-23 
 * 720P에서 프리뷰가 1280*720이 되지 않는다.
 * 표현 될 수 있는 프리뷰 사이즈로 변경해준다.
 */

#define F_SKYCAM_720P_PREVIEW

/* SKYCAM Lee Jitae 2010-12-10
 * MARKET CAMERA APP나 I'M IN APP의 POSTVIEW가 깨진다.
 * 원인은 Set_demension을 하면 POSTVIEW사이즈가 변하는 어플들이 있다.
 * 이런  어플들은 CAMERA_SET_PARM_DIMENSION 이후 값이 변경된다.
 * SKYCAM에서는 1.3333으로 비율에 변화가 없이 나오기 때문에 문제없다.
 */

 #define F_SKYCAM_QBUG_MARKET_APP


 /* SKYCAM Lee JItae 2010-11-23
  * WVGA일 경우 줌을 하면 POSTVIEW가 커진다.
  * Qualcomm으로 부터 패치 받은 코드
  */

  #define F_SKYCAM_QBUG_WVGA_ZOOM


  /* 촬영음/녹화음 재생 중에 응용이 종료될 경우, CLIENT 소멸 시에 해당 촬영음/
   * 녹화음 오브젝트가 강제로 disconnect/clear 되면서 MEDIA SERVER 가 죽는 것을
   * 방지한다. */

#define F_SKYCAM_QBUG_STOP_CAMERA_SOUND


/* 2010-10-06 
 *Continous AF 를 테스트 하기 위해 작업 할 내용 
 */
/* #define F_SKYCAM_CAF
*/

/* oen exif tag 수정 */

#define F_SKYCAM_OEM_EXIF_TAG


/* 동영상 녹화 시작/종료를 빠르게 반복하거나, 이어잭을 장착한 상태에서 연속촬영
 * 모드로 촬영할 경우, MediaPlayer 가 오동작하면서 HALT 발생한다.
 *
 * MediaPlayer 의 경우, 동일한 음원을 재생 중에 또 다시 재생 시도할 경우 100%
 * 오동작하므로 동일 음원을 연속하여 재생해야 할 경우, 반드시 이전 재생이 완료
 * 되었는지 여부를 확인 후 재생해야 한다. */
/* #define F_SKYCAM_QBUG_SKIP_CAMERA_SOUND */


/*
AT&T's model has a option that can be turn shutter sound on or off.
*/

#define F_SKYCAM_SET_SHUTTER_SOUND
/*
캠코더 종료음을 정상적으로 플레이 할 수 보장해주는 딜레이 추가.
*/
#define F_SKYCAM_DELAY_ON_REC_STOP_SOUND 


/*
	QE1차 평가 문제 사항 수정 (2010/10/18)

	Camera preview 화면에서는 초점이 안맞으나 찍은 후 화면은 정상적임
	-->  Auto focus 동작 이후 snapshot mode 이전에 100 msec 딜레이 추가
*/


#define F_SKYCAM_BUG_PATCH_POSTVIEW_FOCUS


/*#define F_SKYCAM_CHECK_IS_INIT_SEQ_DONE*/  /* it skip preview mode process when initiating sensor */
#define F_SKYCAM_SAME_FOCUS_STEP_SKIP /* it skip focus mode setting when previous and current focus mode are same */

/*----------------------------------------------------------------------------*/
/*  MODEL-SPECIFIC CONSTANTS                                                  */
/*  모델 관련 상수 선언                                       */
/*----------------------------------------------------------------------------*/


#ifdef F_SKYCAM_FACTORY_PROC_CMD
#define C_SKYCAM_FNAME_FACTORY_PROC_CMD	"/data/.factorycamera.dat"
#endif

/* 설정 가능한 최소/최대 포커스 레벨이다. */
#ifdef F_SKYCAM_FIX_CFG_FOCUS_STEP
#define C_SKYCAM_MIN_FOCUS_STEP 0	/* 무한대 (default) */
#define C_SKYCAM_MAX_FOCUS_STEP 9	/* 매크로 */
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


/* Brightness 세팅 */

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


