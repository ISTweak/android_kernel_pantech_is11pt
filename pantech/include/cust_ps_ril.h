#ifndef _CUST_PS_RIL_
#define _CUST_PS_RIL_

/***************************************************************************
                                 TARGET
****************************************************************************/
#if defined (T_SATURN)
#undef FEATURE_PS_TME_SPECIFIC /* Spain - Telefonica Moviles Espana */
#else
#undef FEATURE_PS_ATT_SPECIFIC /* USA - AT&T */
#endif


/***************************************************************************
                                 COMMON
****************************************************************************/
/* 20110324 PS10 SUNI : FTM Mode/Debug Screen in LINUX */
#undef FEATURE_PS_DEBUG_MENU_RPC_RIL

#ifdef FEATURE_PS_DEBUG_MENU_RPC_RIL

  #undef FEATURE_FTM_UICC_MENU_RIL /* FTM - UICC 메뉴 : c, h 구현은 없음 */

  #undef FEATURE_PS_DEBUG_MENU_CIQ_RIL /* FTM - CIQ 메뉴 */

#endif

/* Soobeen 20100911 Add For System Dubug Mode */
/* FEATURE_PS_SYSTEM_DEBUGMODE */ 
#undef FEATURE_PS_SYSTEM_DEBUGMODE

/* 20110428 PS10 PKH, Soobeen 20110419 Add For DNS Setting For CIQ */
#undef  FEATURE_PS_DNS_Setting


/***************************************************************************
                                 AS
****************************************************************************/
#if defined FEATURE_PS_ATT_SPECIFIC
/* 20101101 PSTeam Sungoh AT&T RSSI Requirement implementation
13340 V_42 <CDR-RBP-1030>*/
#undef FEATURE_ATNT_RSSI_BAR
#endif


/***************************************************************************
                                 MM
****************************************************************************/
/*
2011.03.04. kdhyun : qcril_arb_init시 network preference를 GSM/WCDMA only로 수정함
*/
#undef FEATURE_PS_RIL_NET_PREF

/*
2010.11.09. kdhyun : MCC/MNC 001/01, 001/02, 002/01인 경우 network name을 올려주지 않도록 함
*/
#undef FEATURE_PS_NOT_USED_QUALCOMM_DEFAULT

/*
2011.04.01. kdhyun : Manual selection의 결과에 RAT 정보도 추가
*/
#define FEATURE_PS_MANUAL_SELECTION_RAT

/*
2011.04.12. kdhyun : Network selection optimization
  - HPLMN을 잡고 있는 상태에서 automatic -> automatic 선택했을 경우 QCRIL_EVT_CM_SS_SRV_CHANGED event 발생하지 않기 때문에 timeout 발생하는 문제 수정
    -> QCRIL_EVT_CM_SS_SRV_CHANGED event 기다리지 않고 바로 success로 처리
  - Manual selection으로 선택된 network으로부터 reject msg를 받았을 경우 바로 fail 처리하도록 함
  - SIM state가 invalid/not available인 경우 바로 fail 처리하도록 함
*/
#undef FEATURE_PS_NETWORK_SELECTION_OPTIMIZATION

/*
2011.4.14. kdhyun : NV_PREF_MODE_I의 default 값을 automatic으로 설정하도록 함
*/
#undef FEATURE_PS_DEFAULT_PREF_MODE_AUTOMATIC


/*2011.7.24. kimyg : ONS table update */
#define FEATURE_NETWORK_TABLE_EXT

/*2011.8.10 kimyg: KDDI ONS table */
#define FEATURE_KDDI_NETWORK_TABLE
/***************************************************************************
                                 CC
****************************************************************************/
/* 
20110331 PS SUNI : PANTECH MIGRATION
For detailed explanation, please refer the pantech migration document. */
/*
When qcril cm is initialzed, TTY value is set as OFF unconditionally. */
#undef FEATURE_CC_TTY_MODE_INIT_VALUE

/* 
For auto redial function, qcril informs the cause value to UI without converting value of modem. */
#undef FEATURE_CC_AUTO_REDIAL_CAUSE

/*
20110418 PS SUNI : FDN CHECK ERROR
FDN check routine is moved to modem from qcril.
But modem cannot return the error cause to qcril.
*/
#undef FEATURE_CC_FDN_ERROR


/***************************************************************************
                                 Data
****************************************************************************/
/* 20110422_PS11_DATA_PKH 
 - port_bridge.h SINGLE_PDP value 변경. 
*/
#undef FEATURE_DATA_MULTIPDP

/* 20110422_PS11_DATA_PKH
 - init.rc 파일 
 - net.tcp.buffersize.default 4096,87380,262140,4096,16384,262140
 - tcp advertisement window scals value  3 : init.qcom.rc
*/
#undef FEATURE_DATA_CHANGE_TCP_CONFIGRATION

/* 20110422_PS11_DATA_PKH 
 - DunService.java /dev/pantech/dun 변경.
  port_bridge 의 경우 수정된 code 사용되어야 한다.
 - init.rc 혹은 init_모델명.rc 파일에서 /dev/pantech directory 를 system 권한으로 생성해줘야한다. 
*/
#undef FEATURE_DATA_CTS_ROOT_PROCESS

/* 20110422_PS11_DATA_PKH
 - 특정 VPN 서버 접속 안되는 문제 (참고 : android 는  SSL VPN 과 Cisco VPN 미지원)
 - external\mtpd\L2tp.c 및 kernel config ( kernel\arch\arm\config\ 에서 이미 define된 내용은 주석처리하고 y로 설정 )
*/
#define FEATURE_DATA_VPN_FIX

/* 20110422_PS11_DATA_PKH
 - no service event 가 발생할 경우 실제 data 종료되지 않고 disconnected 로 broadcast 되어 default route 삭제되고 
   바로 in service 오게되면 아래단으로는 data 연결되어 있지만 상위에는 연결되지 않은 것으로 보이는 문제 
 - dataconnectiontracker.java , BSP 별로 주석처리되어 필요 없는 경우 잇음. 
*/
#undef FEATURE_DATA_NO_SERVICE_BUG_FIX

/* 20110422_PS11_DATA_PKH
 - CTS testTrafficStatsForLocalhost test 를 위한 kernel config 에 CONFIG_UID_STAT=y 로 수정. 
*/
#undef FEATURE_DATA_CONFIG_UID_STAT

/* 20110422_PS11_DATA_PKH
 - MTU size 변경 - 사업자 요구사항 (1410)
 - msm_rmnet.c RMNET_DATA_LEN 사이즈 변경.
 - usb0 interface mtu size 변경 : init.MINI.rc
*/
#undef FEATURE_DATA_CHANGE_MTU_SIZE

/* 2011/02/07 by kwanghee
    for Disable DUN
*/
#undef FEATURE_PS_DISABLE_DUN

/*
20110517 PS11 PKH : FAST DORMANCY
Screen state를 모뎀단에 알려줌.
*/
#undef FEATURE_PS_DATA_FAST_DORMANCY

/* 2011/06/12 by jrdragon
   Attached Icon 추가로 Service Domain 정보를 Framework 에 전달하는 부분 추가
   android\vendor\qcom\proprietary\qcril\qcril_fusion\qcril_cm.c
   android\vendor\qcom\proprietary\qcril\qcril_fusion\qcril_cm_util.c
*/
#undef FEATURE_PS_DATA_REG_STATE_SRV_DOMAIN

/*
20110621 PS11 PKH : CR 273010
smd_tty 수정.
*/
#undef FEATURE_PS_DATA_SMD_TTY


/***************************************************************************
                                 SIM
****************************************************************************/
/*
2011.03.31 TJKIM
부팅 시 PIN/PUK Retry Count 가 부팅 시에 Framework에 업데이트 되지 않는 문제 수정.
RIL_REQUEST_GET_SIM_STATUS 처리 시 PIN1/PUK1/PIN2/PUK2 retry count 전달되도록 수정 함.
*/
#undef FEATURE_PS_PIN_RETRY_INIT

/*
 TJKIM 2011.04.06
 Anroid Framework에서 Virtual SIM 여부를 확인 하기 위해 Card Status 정보에 Virtual SIM 여부를 판별하는 필드를 추가 함.
*/
#undef FEATURE_PS_IS_VIRTUAL_SIM

/*
 TJKIM 2011.04.07
 RIL 과 Framework의 Illegal SIM State Enum 값이 다르게 정의되어 있어 수정함.
 RIL에 -1로 정의되어 있어 Card Status Response를 저장할 때 Execption이 발생 함. Modem, Framework과 동일하게 6으로 수정 함.
*/
#undef FEATURE_PS_ILLEGAL_SIM_STATE_MISMATCH

/*
TJKIM 2011.04.11
Airplane Mode On 시에 Card Down 되지 않도록 수정 함. qcril_cm 의 apm_sim_not_pwdn의 default 값을 TRUE로 변경 함.
*/
#undef FEATURE_PS_NO_CARD_DOWN

/*
TJKIM 2011.04.14
UICC Card Type Check 기능 추가. FTM Mode 에서 ISIM Application이 있을 경우 ISIM으로 표시 함.
*/
#undef FEATURE_PS_CARD_TYPE

/*
TJKIM 2011.04.15
ISIM Read 기능 추가. FTM Mode에서 ISIM의 IMPU를 읽을 수 있도록 수정 함.
*/
#undef FEATURE_PS_ISIM_READ

/* 20100712 ES7 - TJKIM Add*/
#undef FEATURE_PS_CHECK_SIM_TYPE

/* 20100722 ES7 - TJKIM Add */
#undef FEATURE_PS_IMPU_READ

#endif
