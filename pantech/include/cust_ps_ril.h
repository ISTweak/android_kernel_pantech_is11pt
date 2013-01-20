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

  #undef FEATURE_FTM_UICC_MENU_RIL /* FTM - UICC �޴� : c, h ������ ���� */

  #undef FEATURE_PS_DEBUG_MENU_CIQ_RIL /* FTM - CIQ �޴� */

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
2011.03.04. kdhyun : qcril_arb_init�� network preference�� GSM/WCDMA only�� ������
*/
#undef FEATURE_PS_RIL_NET_PREF

/*
2010.11.09. kdhyun : MCC/MNC 001/01, 001/02, 002/01�� ��� network name�� �÷����� �ʵ��� ��
*/
#undef FEATURE_PS_NOT_USED_QUALCOMM_DEFAULT

/*
2011.04.01. kdhyun : Manual selection�� ����� RAT ������ �߰�
*/
#define FEATURE_PS_MANUAL_SELECTION_RAT

/*
2011.04.12. kdhyun : Network selection optimization
  - HPLMN�� ��� �ִ� ���¿��� automatic -> automatic �������� ��� QCRIL_EVT_CM_SS_SRV_CHANGED event �߻����� �ʱ� ������ timeout �߻��ϴ� ���� ����
    -> QCRIL_EVT_CM_SS_SRV_CHANGED event ��ٸ��� �ʰ� �ٷ� success�� ó��
  - Manual selection���� ���õ� network���κ��� reject msg�� �޾��� ��� �ٷ� fail ó���ϵ��� ��
  - SIM state�� invalid/not available�� ��� �ٷ� fail ó���ϵ��� ��
*/
#undef FEATURE_PS_NETWORK_SELECTION_OPTIMIZATION

/*
2011.4.14. kdhyun : NV_PREF_MODE_I�� default ���� automatic���� �����ϵ��� ��
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
 - port_bridge.h SINGLE_PDP value ����. 
*/
#undef FEATURE_DATA_MULTIPDP

/* 20110422_PS11_DATA_PKH
 - init.rc ���� 
 - net.tcp.buffersize.default 4096,87380,262140,4096,16384,262140
 - tcp advertisement window scals value  3 : init.qcom.rc
*/
#undef FEATURE_DATA_CHANGE_TCP_CONFIGRATION

/* 20110422_PS11_DATA_PKH 
 - DunService.java /dev/pantech/dun ����.
  port_bridge �� ��� ������ code ���Ǿ�� �Ѵ�.
 - init.rc Ȥ�� init_�𵨸�.rc ���Ͽ��� /dev/pantech directory �� system �������� ����������Ѵ�. 
*/
#undef FEATURE_DATA_CTS_ROOT_PROCESS

/* 20110422_PS11_DATA_PKH
 - Ư�� VPN ���� ���� �ȵǴ� ���� (���� : android ��  SSL VPN �� Cisco VPN ������)
 - external\mtpd\L2tp.c �� kernel config ( kernel\arch\arm\config\ ���� �̹� define�� ������ �ּ�ó���ϰ� y�� ���� )
*/
#define FEATURE_DATA_VPN_FIX

/* 20110422_PS11_DATA_PKH
 - no service event �� �߻��� ��� ���� data ������� �ʰ� disconnected �� broadcast �Ǿ� default route �����ǰ� 
   �ٷ� in service ���ԵǸ� �Ʒ������δ� data ����Ǿ� ������ �������� ������� ���� ������ ���̴� ���� 
 - dataconnectiontracker.java , BSP ���� �ּ�ó���Ǿ� �ʿ� ���� ��� ����. 
*/
#undef FEATURE_DATA_NO_SERVICE_BUG_FIX

/* 20110422_PS11_DATA_PKH
 - CTS testTrafficStatsForLocalhost test �� ���� kernel config �� CONFIG_UID_STAT=y �� ����. 
*/
#undef FEATURE_DATA_CONFIG_UID_STAT

/* 20110422_PS11_DATA_PKH
 - MTU size ���� - ����� �䱸���� (1410)
 - msm_rmnet.c RMNET_DATA_LEN ������ ����.
 - usb0 interface mtu size ���� : init.MINI.rc
*/
#undef FEATURE_DATA_CHANGE_MTU_SIZE

/* 2011/02/07 by kwanghee
    for Disable DUN
*/
#undef FEATURE_PS_DISABLE_DUN

/*
20110517 PS11 PKH : FAST DORMANCY
Screen state�� �𵩴ܿ� �˷���.
*/
#undef FEATURE_PS_DATA_FAST_DORMANCY

/* 2011/06/12 by jrdragon
   Attached Icon �߰��� Service Domain ������ Framework �� �����ϴ� �κ� �߰�
   android\vendor\qcom\proprietary\qcril\qcril_fusion\qcril_cm.c
   android\vendor\qcom\proprietary\qcril\qcril_fusion\qcril_cm_util.c
*/
#undef FEATURE_PS_DATA_REG_STATE_SRV_DOMAIN

/*
20110621 PS11 PKH : CR 273010
smd_tty ����.
*/
#undef FEATURE_PS_DATA_SMD_TTY


/***************************************************************************
                                 SIM
****************************************************************************/
/*
2011.03.31 TJKIM
���� �� PIN/PUK Retry Count �� ���� �ÿ� Framework�� ������Ʈ ���� �ʴ� ���� ����.
RIL_REQUEST_GET_SIM_STATUS ó�� �� PIN1/PUK1/PIN2/PUK2 retry count ���޵ǵ��� ���� ��.
*/
#undef FEATURE_PS_PIN_RETRY_INIT

/*
 TJKIM 2011.04.06
 Anroid Framework���� Virtual SIM ���θ� Ȯ�� �ϱ� ���� Card Status ������ Virtual SIM ���θ� �Ǻ��ϴ� �ʵ带 �߰� ��.
*/
#undef FEATURE_PS_IS_VIRTUAL_SIM

/*
 TJKIM 2011.04.07
 RIL �� Framework�� Illegal SIM State Enum ���� �ٸ��� ���ǵǾ� �־� ������.
 RIL�� -1�� ���ǵǾ� �־� Card Status Response�� ������ �� Execption�� �߻� ��. Modem, Framework�� �����ϰ� 6���� ���� ��.
*/
#undef FEATURE_PS_ILLEGAL_SIM_STATE_MISMATCH

/*
TJKIM 2011.04.11
Airplane Mode On �ÿ� Card Down ���� �ʵ��� ���� ��. qcril_cm �� apm_sim_not_pwdn�� default ���� TRUE�� ���� ��.
*/
#undef FEATURE_PS_NO_CARD_DOWN

/*
TJKIM 2011.04.14
UICC Card Type Check ��� �߰�. FTM Mode ���� ISIM Application�� ���� ��� ISIM���� ǥ�� ��.
*/
#undef FEATURE_PS_CARD_TYPE

/*
TJKIM 2011.04.15
ISIM Read ��� �߰�. FTM Mode���� ISIM�� IMPU�� ���� �� �ֵ��� ���� ��.
*/
#undef FEATURE_PS_ISIM_READ

/* 20100712 ES7 - TJKIM Add*/
#undef FEATURE_PS_CHECK_SIM_TYPE

/* 20100722 ES7 - TJKIM Add */
#undef FEATURE_PS_IMPU_READ

#endif
