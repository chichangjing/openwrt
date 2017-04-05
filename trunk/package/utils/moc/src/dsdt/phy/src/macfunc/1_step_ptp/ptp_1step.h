#include <mvlPtpCopyright.h>

/********************************************************************************
* ptp_1step.h
*
* DESCRIPTION:
*       Definitions of internal functions for new PTP control API's
*
* DEPENDENCIES:
*
* FILE REVISION NUMBER:
*
*******************************************************************************/

#ifndef __PTP_1STEP_H
#define __PTP_1STEP_H
#ifdef __cplusplus
extern "C" {
#endif

/*#define PTP_ADDR_ADJ 2   1 */
#define PTP_ADDR_ADJ 1  /* 2 */

/* status / error codes */
typedef unsigned int DSDT_STATUS;
#define DSDT_ERROR           (-1)
#define DSDT_OK               (0x00)    /* Operation succeeded */
#define DSDT_FAIL               (0x01)    /* Operation failed  */

typedef enum {
    DSDT_FALSE = 0,
    DSDT_TRUE  = 1
} DSDT_BOOL;

#define PTP_PORT_OFF(port) ((port%4)*0x800)

#define MAD_APP 1

#if 0
/*  PTP block structure */
typedef struct {
    union
    {
        GT_QD_DEV *qddev;
        MAD_DEV   *maddev;
    } device;
} PTP_BLK;
extern MAD_DEV       *maddev;
#endif

/*#define PTP_DBG_INFO MAD_DBG_ERROR  */
#ifdef DEBUG_MAD
#define DEBUG_PTP  1
#else
#undef DEBUG_PTP
#endif
#define DEBUG_PTP  1

#define PTP_DBG_INFO MAD_DBG_INFO
/*#define PTP_DBG_DEMO MAD_DBG_ERROR */
#define PTP_DBG_DEMO MAD_CRITIC_INFO
#define PTP_DBG_TRACE MAD_DBG_INFO
#define PTP_DBG_VERB MAD_DBG_INFO
/*  Data structure for Time of Day */
typedef struct {
  unsigned long subnano; /* Sun-nanoseconds */
  unsigned long nanosec; /* nanoseconds */
  unsigned long lowsec; /* low seconds */
  unsigned long highsec; /* high seconds */
} NPTP_TIME_STRUCT;

#if 0 /* In msec_defines.h */
/* For new PTP */
  #define VN_GLOBAL_PTP        0xB000 /*/< Global  */
  #define VN_TAI_CFG       0xBC00 /*/< TAI configuration  */
  #define VN_EG0_PTP       0xA000 /*/< PTP configuration for Egress port 0 */
  #define VN_IG0_PTP       0xA800 /*/< PTP configuration for Ingress port 0 */
  #define VN_EG1_PTP       0xA000 /*/< PTP configuration for Egress port 1 */
  #define VN_IG1_PTP       0xA800 /*/< PTP configuration for Ingress port 1 */
  #define VN_EG2_PTP       0xA000 /*/< PTP configuration for Egress port 2 */
  #define VN_IG2_PTP       0xA800 /*/< PTP configuration for Ingress port 2 */
  #define VN_EG3_PTP       0xA000 /*/< PTP configuration for Egress port 3 */
  #define VN_IG3_PTP       0xA800 /*/< PTP configuration for Ingress port 3 */
/* */

#endif

#define  MSEC_PTP_GLOBAL		VN_GLOBAL_PTP
/* MacSec/PTP Global register Defines */
#define MSEC_PTP_RESERVED0              MSEC_PTP_GLOBAL+0x00
#define MSEC_PTP_RESERVED1              MSEC_PTP_GLOBAL+0x01
#define MSEC_PTP_ENCAP_DA0              MSEC_PTP_GLOBAL+0x02
#define MSEC_PTP_ENCAP_DA_ET            MSEC_PTP_GLOBAL+0x03
#define MSEC_PTP_IMB_CTL                MSEC_PTP_GLOBAL+0x04
#define MSEC_PTP_IMB_DATA               MSEC_PTP_GLOBAL+0x05
#define MSEC_PTP_FIPS_VEC0              MSEC_PTP_GLOBAL+0x06
#define MSEC_PTP_FIPS_VEC1              MSEC_PTP_GLOBAL+0x07
#define MSEC_PTP_FIPS_VEC2              MSEC_PTP_GLOBAL+0x08
#define MSEC_PTP_FIPS_VEC3              MSEC_PTP_GLOBAL+0x09
#define MSEC_PTP_STAT_GLOBAL            MSEC_PTP_GLOBAL+0x0a
#define MSEC_PTP_INTR_SOURCE            MSEC_PTP_GLOBAL+0x0b
#define MSEC_PTP_GLOBAL_INTR            MSEC_PTP_GLOBAL+0x0c
#define MSEC_PTP_GLOBAL_INTR_MSK        MSEC_PTP_GLOBAL+0x0d
#define MSEC_PTP_SVEC_CFG_GLOBAL        MSEC_PTP_GLOBAL+0x0e
#define MSEC_PTP_SVEC_CFG_ADD           MSEC_PTP_GLOBAL+0x0f
#define MSEC_PTP_MIB_PORT_TO_COPY       MSEC_PTP_GLOBAL+0x10
#define MSEC_PTP_POWER_CTL              MSEC_PTP_GLOBAL+0x11
#define MSEC_PTP_MIB_STATUS              MSEC_PTP_GLOBAL+0x12

/* PTP address Memory Map */
#define  PTP_PMEU_PAM		0x0000                    /* pped */
#define  PTP_UMEU_PAM		0x0040*PTP_ADDR_ADJ       /* pued */
#define  PTP_RF     		0x0080*PTP_ADDR_ADJ       /* rf */
#define  PTP_STATS     		0x00c0*PTP_ADDR_ADJ       /* stat */
#define  PTP_PMEU_UDATA		0x0100*PTP_ADDR_ADJ       /* ppec */
#define  PTP_UMEU_UDATA		0x0200*PTP_ADDR_ADJ       /* ppdc */
#define  PTP_LUT_ACTION		0x0300*PTP_ADDR_ADJ       /* plud */
#define  PTP_LUT_KEYMASK	0x0380*PTP_ADDR_ADJ       /* pluc */

      
/* New PTP Register oFFSET Defines */
typedef enum  {
PTP_CFG_GEN =0*PTP_ADDR_ADJ,
PTP_CFG_GEN2 =1*PTP_ADDR_ADJ,
PTP_INTR_SET =2*PTP_ADDR_ADJ,
PTP_INTR_SET_MSK =3*PTP_ADDR_ADJ,
PTP_TIMEOUT_CTL =4*PTP_ADDR_ADJ,
PTP_STT_CTL =5*PTP_ADDR_ADJ,
PTP_DISP =6*PTP_ADDR_ADJ,
PTP_DISP_RMA_S =7*PTP_ADDR_ADJ,
PTP_DISP_RMA_M =8*PTP_ADDR_ADJ,
PTP_TS_QUEUE =9*PTP_ADDR_ADJ,
PTP_TS_USAGE =10*PTP_ADDR_ADJ,
PTP_TSQL_DATA =11*PTP_ADDR_ADJ,
PTP_TSQH_DATA =12*PTP_ADDR_ADJ,
PTP_BIST_CTL0 =13*PTP_ADDR_ADJ,
BIST_STATUS =14*PTP_ADDR_ADJ,
PTP_CFG_STAT =15*PTP_ADDR_ADJ,
PTP_CFG_TIMEOUT_CTL2 =16*PTP_ADDR_ADJ,
PTP_CFG_TIMEOUT_CTL =17*PTP_ADDR_ADJ,
PTP_CFG_LUT_CTRL =18*PTP_ADDR_ADJ
} PTP_1STEP_RF_NAME;

/* Base addresses  */
typedef enum {
  PTP_igr,
  PTP_egr
 } PTP_Blk;

 typedef enum {
  GLOBAL_PTP,
  TAI_CFG,
  EG0_PTP,
  IG0_PTP,
  EG1_PTP,
  IG1_PTP,
  EG2_PTP,
  IG2_PTP,
  EG3_PTP,
  IG3_PTP
} PTP_TYPE;

#if 0
typedef enum {
  PTP_ADDR_PMEU_PAM,
  PTP_ADDR_UMEU_PAM,
  PTP_ADDR_RF,
  PTP_ADDR_STATS,
  PTP_ADDR_PMEU_UDATA,
  PTP_ADDR_UMEU_UDATA,
  PTP_ADDR_LUT_ACTION,
  PTP_ADDR_LUT_KEYMASK
} PTP_Addr;
#endif

/* Set bits by ANDing inverse of new value with old value */
typedef enum {
	MacName_RxGoodOctLo,
	MacName_RxGoodOctHi,
	MacName_RxBadOct,
	MacName_TxFifoUnder,
	MacName_RxUnicast,
    MacName_SentDeferred,
	MacName_BroadRx,
	MacName_MultiRx,
	MacName_Fr64,
	MacName_Fr65_127,
	MacName_Fr128_255,
	MacName_Fr256_511,
    MacName_Fr512_1023,
	MacName_Fr1024_Max,
	MacName_GoodTxOctLo,
	MacName_GoodTxOctHi,
	MacName_TxUnicast,
	MacName_ExCol,
	MacName_TxMulti,
    MacName_TxBroad,
	MacName_SentMultiple,
	MacName_FCSent,
	MacName_FCReceived,
	MacName_RxFifoOver,
	MacName_Undersize,
	MacName_Fragments,
    MacName_Oversize,
	MacName_Jabber,
	MacName_RxError,
	MacName_BadCrc,
	MacName_Collisions,
	MacName_LateCol,
	MacName_MacBameEnd
} MacName;

/* CAM match name. Associative array for LUT CAM match fields */
typedef enum {
  M_FLAGS,          
  M_PTP_VERSIONS,   
  M_PTP_MSGTYPE,    
  M_PTP_TRANS_SPEC, 
  M_TST_E,          
  M_TST_ACTION,     
  M_PTP_FLAGS      
}PTP_MATCH;

typedef enum {
  A_RX_TST_ACTION,
  A_RX_TST_OFFSETTYPE,
  A_RX_TST_U,
  A_RX_INSERTTST,
  A_RX_DROP,
  A_RX_INIPIGGYBACK,
  A_RX_STAMPLOW,
  A_RX_STAMPHIGH,
  A_RX_TST_E
}PTP_RX_ACTION;

typedef enum {
  A_TX_UPDATETXTIME,
  A_TX_REMOVETST,
  A_TX_DROP,
  A_TX_UPDATERESIDENCE,
  A_TX_CLEAR_PIGGYBACK,
  A_TX_STAMPLOW,
  A_TX_STAMPHIGH,
  A_TX_HOLD_FOR_TRANS
}PTP_TX_ACTION;

typedef enum {
  K_FlagPTPv2,
  K_FlagUDP,
  K_FlagIPv4,
  K_FlagIPv6,
  K_FlagTST,
  K_FlagGRE,
  K_FlagY1731,
  K_FlagNTP,
  K_FlagPTPv1,
  K_FlagDOT1q,
  K_FlagSTAG,
  K_FlagITAG,
  K_FlagSNAP,
  K_FlagMPLS,
  K_FlagMACSEC,
  K_AlwaysZero,
  K_VersionPTP,
  K_MessageType,
  K_TransportSpecific,
  K_TST_action,
  K_TST_E,
  K_FlagField,
  K_DomainNumber
}PTP_KEY;

typedef enum {
  PTP_Act_UpdateTxTime,      /* Tx action */
  PTP_Act_RemoveTST,         /* Tx action */
  PTP_Act_Drop,              /* Tx/Rx action */
  PTP_Act_UpdateResidence,   /* Tx action */
  PTP_Act_StampLow,          /* Tx/Rx action */
  PTP_Act_StampHigh,         /* Tx/Rx action */
  PTP_Act_TST_action,           /* Rx action */
  PTP_Act_TST_mode,             /* Rx action */
  PTP_Act_TST_E,                /* Rx action */
  PTP_Act_InsertTST,            /* Rx action */
  PTP_Act_IniPiggyback,         /* Rx action */
  PTP_Act_HoldForTransmission,  /* Tx action */
  PTP_Act_ClearPiggyback,       /* Tx action */
  PTP_Act_AddAsymmetry,	        /* no use action */
  PTP_Act_AddLinkDelay,         /* no use action */
  PTP_Act_No_Action = -1
}PTP_1STEP_LUT_ACT;

typedef enum {
	REG_TOD_0,             /* 0 */
	REG_TOD_1,
	REG_TOD_2,
	REG_TOD_3,
	REG_FRC_0,
	REG_FRC_1,
	REG_FRC_2,
	REG_FRC_3,
	REG_DRIFT_ADJ_CFG,     /* 8 */
	REG_TOD_STEP_NS,
	REG_TOD_STEP_FS,
	REG_TOD_CFG_GEN,
	REG_PULSE_DELAY,       /* 12 */
	REG_TRIG_GEN_TOD_0,    /* 13 */
	REG_TRIG_GEN_TOD_1,
	REG_TRIG_GEN_TOD_2,
	REG_TRIG_GEN_TOD_3,
	REG_TRIG_GEN_MSK_0,
	REG_TRIG_GEN_MSK_1,
	REG_TRIG_GEN_MSK_2,
	REG_TRIG_GEN_MSK_3,
	REG_TOD_LOAD_0,        /* 21 */
	REG_TOD_LOAD_1,
	REG_TOD_LOAD_2,
	REG_TOD_LOAD_3,
	REG_TOD_CAP_0,
	REG_TOD_CAP_1,
	REG_TOD_CAP_2,
	REG_TOD_CAP_3,
	REG_TOD_CAP1_0,         /* 29 */
	REG_TOD_CAP1_1,
	REG_TOD_CAP1_2,
	REG_TOD_CAP1_3,
	REG_TOD_CAP_CFG,        /* 33 */
	REG_PULSE_IN_CNT,
	REG_TOD_FUNC_CFG,       /* 35(0x23) */
	REG_PCLK_CFG,
	REG_PCLK_CNT_0,
	REG_PCLK_CNT_1,
	REG_DRIFT_THR_CFG,
	REG_NEXT_RT_0,          /* 40(0x28) */
	REG_NEXT_RT_1,
	REG_NEXT_RT_2,
	REG_NEXT_RT_3,
	REG_CLOCK_CYC,
	RSVD,
	REG_CLK_IN_CNT,
	REG_TAI_INTR,
	REG_TAI_INTR_MASK       /* 48 (0x30) */
} PTP_1STEP_TAI_REGS;

/* TAI register list */
#define TOD_0           VN_TAI_CFG +REG_TOD_0*PTP_ADDR_ADJ
#define TOD_1           VN_TAI_CFG +REG_TOD_1*PTP_ADDR_ADJ
#define TOD_2           VN_TAI_CFG +REG_TOD_2*PTP_ADDR_ADJ
#define TOD_3           VN_TAI_CFG +REG_TOD_3*PTP_ADDR_ADJ
#define FRC_0           VN_TAI_CFG +REG_FRC_0*PTP_ADDR_ADJ
#define FRC_1           VN_TAI_CFG +REG_FRC_1*PTP_ADDR_ADJ
#define FRC_2           VN_TAI_CFG +REG_FRC_2*PTP_ADDR_ADJ
#define FRC_3           VN_TAI_CFG +REG_FRC_3*PTP_ADDR_ADJ
#define DRIFT_ADJ_CFG   VN_TAI_CFG +REG_DRIFT_ADJ_CFG*PTP_ADDR_ADJ
#define TOD_STEP_NS     VN_TAI_CFG +REG_TOD_STEP_NS*PTP_ADDR_ADJ
#define TOD_STEP_FS     VN_TAI_CFG +REG_TOD_STEP_FS*PTP_ADDR_ADJ
#define TOD_CFG_GEN     VN_TAI_CFG +REG_TOD_CFG_GEN*PTP_ADDR_ADJ
#define PULSE_DELAY     VN_TAI_CFG +REG_PULSE_DELAY*PTP_ADDR_ADJ
#define TRIG_GEN_TOD_0  VN_TAI_CFG +REG_TRIG_GEN_TOD_0*PTP_ADDR_ADJ
#define TRIG_GEN_TOD_1  VN_TAI_CFG +REG_TRIG_GEN_TOD_1*PTP_ADDR_ADJ
#define TRIG_GEN_TOD_2  VN_TAI_CFG +REG_TRIG_GEN_TOD_2*PTP_ADDR_ADJ
#define TRIG_GEN_TOD_3  VN_TAI_CFG +REG_TRIG_GEN_TOD_3*PTP_ADDR_ADJ
#define TRIG_GEN_MASK_0  VN_TAI_CFG +REG_TRIG_GEN_MASK_0*PTP_ADDR_ADJ
#define TRIG_GEN_MASK_1  VN_TAI_CFG +REG_TRIG_GEN_MASK_1*PTP_ADDR_ADJ
#define TRIG_GEN_MASK_2  VN_TAI_CFG +REG_TRIG_GEN_MASK_2*PTP_ADDR_ADJ
#define TRIG_GEN_MASK_3  VN_TAI_CFG +REG_TRIG_GEN_MASK_3*PTP_ADDR_ADJ
#define TOD_LOAD_0       VN_TAI_CFG +REG_TOD_LOAD_0*PTP_ADDR_ADJ
#define TOD_LOAD_1       VN_TAI_CFG +REG_TOD_LOAD_1*PTP_ADDR_ADJ
#define TOD_LOAD_2       VN_TAI_CFG +REG_TOD_LOAD_2*PTP_ADDR_ADJ
#define TOD_LOAD_3       VN_TAI_CFG +REG_TOD_LOAD_3*PTP_ADDR_ADJ
#define TOD_CAP_0        VN_TAI_CFG +REG_TOD_CAP_0*PTP_ADDR_ADJ
#define TOD_CAP_1        VN_TAI_CFG +REG_TOD_CAP_1*PTP_ADDR_ADJ
#define TOD_CAP_2        VN_TAI_CFG +REG_TOD_CAP_2*PTP_ADDR_ADJ
#define TOD_CAP_3        VN_TAI_CFG +REG_TOD_CAP_3*PTP_ADDR_ADJ
#define TOD_CAP1_0       VN_TAI_CFG +REG_TOD_CAP1_0*PTP_ADDR_ADJ
#define TOD_CAP1_1       VN_TAI_CFG +REG_TOD_CAP1_1*PTP_ADDR_ADJ
#define TOD_CAP1_2       VN_TAI_CFG +REG_TOD_CAP1_2*PTP_ADDR_ADJ
#define TOD_CAP1_3       VN_TAI_CFG +REG_TOD_CAP1_3*PTP_ADDR_ADJ
#define TOD_CAP_CFG      VN_TAI_CFG +REG_TOD_CAP_CFG*PTP_ADDR_ADJ
#define PULSE_IN_CNT     VN_TAI_CFG +REG_PULSE_IN_CNT*PTP_ADDR_ADJ
#define TOD_FUNC_CFG     VN_TAI_CFG +REG_TOD_FUNC_CFG*PTP_ADDR_ADJ
#define PCLK_CFG         VN_TAI_CFG +REG_PCLK_CFG*PTP_ADDR_ADJ
#define PCLK_CNT_0       VN_TAI_CFG +REG_PCLK_CNT_0*PTP_ADDR_ADJ
#define PCLK_CNT_1       VN_TAI_CFG +REG_PCLK_CNT_1*PTP_ADDR_ADJ
#define DRIFT_THR_CFG    VN_TAI_CFG +REG_DRIFT_THR_CFG*PTP_ADDR_ADJ
#define NEXT_RT_0        VN_TAI_CFG +REG_NEXT_RT_0*PTP_ADDR_ADJ
#define NEXT_RT_1        VN_TAI_CFG +REG_NEXT_RT_1*PTP_ADDR_ADJ
#define NEXT_RT_2        VN_TAI_CFG +REG_NEXT_RT_2*PTP_ADDR_ADJ
#define NEXT_RT_3        VN_TAI_CFG +REG_NEXT_RT_3*PTP_ADDR_ADJ
#define CLOCK_CYC        VN_TAI_CFG +REG_CLOCK_CYC*PTP_ADDR_ADJ
#define CLK_IN_CNT            VN_TAI_CFG +REG_CLK_IN_CNT*PTP_ADDR_ADJ
#define TAI_INTR         VN_TAI_CFG +REG_TAI_INTR*PTP_ADDR_ADJ
#define TAI_INTR_MASK    VN_TAI_CFG +REG_TAI_INTR_MASK*PTP_ADDR_ADJ
#define OLD_TOD_0        VN_TAI_CFG +0x31*PTP_ADDR_ADJ
#define OLD_TOD_1        VN_TAI_CFG +0x32*PTP_ADDR_ADJ
#define OLD_TOD_2        VN_TAI_CFG +0x33*PTP_ADDR_ADJ
#define OLD_TOD_3        VN_TAI_CFG +0x34*PTP_ADDR_ADJ
#define TIME_UPD_CNT     VN_TAI_CFG +0x38*PTP_ADDR_ADJ


typedef enum  {
  PTPv2_MSG_Sync = 0x0,
  PTPv2_MSG_Delay_Req = 0x1,
  PTPv2_MSG_Pdelay_Req = 0x2,
  PTPv2_MSG_Pdelay_Resp = 0x3,
  PTPv2_MSG_Follow_Up = 0x8,
  PTPv2_MSG_Delay_Resp = 0x9,
  PTPv2_MSG_Pdelay_Resp_Follow_Up = 0xa,
  PTPv2_MSG_Announce = 0xb,
  PTPv2_MSG_Signaling = 0xc,
  PTPv2_MSG_Management = 0xd
}PTPv2_message_type;


typedef enum  {
  PTP_1STEP_VER_V1,
  PTP_1STEP_VER_V2
}PTP_1STEP_VERSION;

typedef int PTP_1STEP_TRANS_SPEC;

typedef enum  {
  PTP_1STEP_OVER_L2,
  PTP_1STEP_OVER_IP4_UDP,
  PTP_1STEP_OVER_IP6_UDP,
  PTP_1STEP_OVER_NTP,
  PTP_1STEP_OVER_MPLS
}PTP_1STEP_OVER_LAYER;

typedef enum  {
  PTP_1STEP_ING_ACT_TS_TAG		= 0x1,
  PTP_1STEP_ING_ACT_TS_TAG_EX   = 0x2,
  PTP_1STEP_ING_ACT_PIGGY       = 0x4,
  PTP_1STEP_ING_ACT_LOW_QUEUE   = 0x8,
  PTP_1STEP_ING_ACT_HIGH_QUEUE  = 0x10
}PTP_1STEP_ING_ACTION;

typedef enum  {
  PTP_1STEP_EG_ACT_LOW_QUEUE    = 0x1,
  PTP_1STEP_EG_ACT_HIGH_QUEUE   = 0x2,
  PTP_1STEP_EG_ACT_UPDATE_TS    = 0x4,
  PTP_1STEP_EG_ACT_SET_RES_TS   = 0x8,
  PTP_1STEP_EG_ACT_REMOVE_TST   = 0x10
}PTP_1STEP_EG_ACTION;

typedef enum  {
  PTP_1STEP_MSG_SYNC			= 1<<PTPv2_MSG_Sync,
  PTP_1STEP_MSG_DELAY_REQ		= 1<<PTPv2_MSG_Delay_Req,
  PTP_1STEP_MSG_PDELAY_REQ		= 1<<PTPv2_MSG_Pdelay_Req,
  PTP_1STEP_MSG_PDELAY_RESP		= 1<<PTPv2_MSG_Pdelay_Resp,
  PTP_1STEP_MSG_FOLLOWUP		= 1<<PTPv2_MSG_Follow_Up,
  PTP_1STEP_MSG_DELAY_RESP		= 1<<PTPv2_MSG_Delay_Resp,
  PTP_1STEP_MSG_PDELAY_RESP_FOLLOWUP	= 1<<PTPv2_MSG_Pdelay_Resp_Follow_Up,
  PTP_1STEP_MSG_ANNOUNCE				= 1<<PTPv2_MSG_Announce,
  PTP_1STEP_MSG_SIGNALING				= 1<<PTPv2_MSG_Signaling,
  PTP_1STEP_MSG_MANAGEMENT				= 1<<PTPv2_MSG_Management
}PTP_1STEP_MSG_TYPE;

typedef enum  {
  PTP_1STEP_QUEUE_HIGH,
  PTP_1STEP_QUEUE_LOW
} PTP_1STEP_QUEUE_TYPE;

typedef struct {
	PTP_1STEP_VERSION version;
	PTP_1STEP_TRANS_SPEC transSpec ;
    PTP_1STEP_MSG_TYPE   msgTypes;
	PTP_1STEP_OVER_LAYER overLayer;
	PTP_1STEP_ING_ACTION ingaction;
	PTP_1STEP_EG_ACTION egaction;
} PTP_1STEP_CFG;

/* PPT time structure in register : tod_0, frc_0, trig_gen_tod_0, trig_gen_msk_0, tod_load_0, tod_cap_0, tod_cap1_0, next_rt_0 */
typedef struct {
    unsigned int      sec_hi;  /* High seconds */
    unsigned int      sec_lo;  /* Low seconds */
    unsigned int      nanosec; /* nanoseconds */ 
    unsigned short    frec;    /* sub-nanoseconds */ 
} PtpTimeStruct; 


/* Time stamp struct */
typedef union {
  struct {
    unsigned int      nanoseconds;  
    unsigned short    topseconds; 
    unsigned int      seconds;
  }  extTs;
  unsigned int      ts;  
} Ptp1StepTsStruct; 

typedef struct {
    unsigned short      seqId; 
    unsigned char       domainNum; 
    unsigned char       transpSpec; 
    unsigned char       msgId; 
    Ptp1StepTsStruct     ts; 
} PtpTsQueueStruct; 


extern unsigned short ptp_addr[];

/*  */
extern unsigned short ptp_1step_base_addr[];
extern char * ptp_1step_macNmList[];
extern char *lut_match_fld_pos_name[];
extern int lut_match_fld_pos[];
extern char *lut_match_fld_sz_name[];
extern int lut_match_fld_sz[];
extern char * lut_tx_action_fld_pos_name[];
extern int lut_tx_action_fld_pos[];
extern int lut_tx_action_fld_sz[];
extern char * lut_rx_action_fld_pos_name[];
extern int lut_rx_action_fld_pos[];
extern int lut_rx_action_fld_sz[];

extern char * lut_action_fld_pos_name[];
extern int lut_action_fld_pos[];
extern int lut_action_fld_sz[];

extern char * ptp_stat_name [];
extern char * ptp_rf_names_list [];
extern char * ptp_egr_stats_names_list[];
extern char * ptp_igr_stats_names_list[];
extern int lut_key_pos[];
extern int lut_key_size[]; 
extern int lut_act_pos[];
extern char * ptp_tai_regs_list_name [];

typedef enum {
  CLK_MODE,
  IN_CLK_EN,
  EXT_CLK_EN,
  EN_SYNCE_TAI_CLK,
  SYNCE_CLK_SEL,
  TAI_CLKOUT_SEL,
  PULSE_IN_SEL,
  CLK_IN_SEL,
  NTP_TOD_MODE
} TAI_TOD_CFG_GEN;

extern char *tai_tod_cfg_gen_name[];
extern int tai_tod_cfg_gen_pos[];
extern int tai_tod_cfg_gen_sz[];

typedef enum {
  TAI_TOD_PULSE_WIDTH,
  TAI_TOD_CNT_SEL,
  TAI_TOD_TOD_FUNC_TRIG,
  TAI_TOD_TRIG_GEN_EN,
  TAI_TOD_TOD_FUNC 
} TAI_TOD_FUNC_CFG;

typedef enum {
  TOD_FUNC_TIMER_UPDATE,
  TOD_FUNC_TIMER_INC,
  TOD_FUNC_TIMER_DEC,
  TOD_FUNC_TIMER_CAPT 
} TOD_FUNC_TIMER_OP;

#define PTP_CYCLE 8 /* 125M clock */

extern char *tai_tod_func_cfg_name[];
extern int tai_tod_func_cfg_pos[];
extern int tai_tod_func_cfg_sz[];

/* functions */
DSDT_STATUS ptp_dev_init(void * dev);
PTP_TYPE get_ptp_base_addr_id ( int portnum, PTP_Blk path );

DSDT_STATUS clear_bits (int port, unsigned short addr, unsigned int data);

/* write a 1540 register */
unsigned int ptp_read_reg (int port, unsigned short addr);

unsigned int ptp_readcheck_reg (int port, unsigned short addr);

DSDT_STATUS ptp_write_reg (int port, unsigned short addr, unsigned int data);

DSDT_STATUS ptp_writecheck_reg(int port, unsigned short addr, unsigned int data);

void ptp_1step_print_reg (int port, unsigned short addr);
/* Set bits by ORing new value with old value */
DSDT_STATUS set_bits (int port, unsigned short addr, unsigned int data);

DSDT_STATUS ptp_1step_dump_mac_statistics (int port, unsigned int *macValList);
void ptp_1step_print_mac_statistics (int port, unsigned int *macValList);
DSDT_STATUS ptp_1step_dump_sysmac_statistics (int port, unsigned int *sysmacValList);
void ptp_1step_print_sysmac_statistics (int port, unsigned int *sysmacValList);
void get_mac_stats (int port, unsigned int *macValList);
void get_sysmac_stats (int port, unsigned int *sysmacValList);

DSDT_STATUS ptp_1step_set_smac_loopback (int port);
/* Enable PTP by clearing PTP bypass */
DSDT_STATUS enable_ptp (int port);
/* Disable PTP by setting PTP bypass */
DSDT_STATUS disable_ptp (int port);

/* Enable RX CRC check */
DSDT_STATUS ptp_1step_enable_rx_crc_check (int port);
/* Disable RX CRC check */
DSDT_STATUS ptp_1step_disable_rx_crc_check (int port);
DSDT_STATUS set_wiremac_ipg (int port, int ipg);
DSDT_STATUS set_cutthrough_mode (int port);
DSDT_STATUS ptp_set_store_forward_mode (int port);
DSDT_STATUS ptp_macsec_workaround_1 (void);

void ptp_1step_print_rumi_stats (int port);

/* Print TOD */
void ptp_1step_print_tod (int port);
/* Print FRC */
void ptp_1step_print_frc (int port);

/* common PTP init  */
DSDT_STATUS ptp_common_init (int port);

/* PTP parser init */
DSDT_STATUS ptp_parser_init (int port);

DSDT_STATUS ptp_parser_set_udata (int port);

DSDT_STATUS ptp_parser_check_udata (int port);

/* PTP update init */
DSDT_STATUS ptp_test_clk_in_out_init (int port, int isMaster);
DSDT_STATUS ptp_update_init (int port);

DSDT_STATUS ptp_update_set_udata (int port);
DSDT_STATUS ptp_update_check_udata (int port);

/* Set PTP udata into udata space */
DSDT_STATUS ptp_set_udata (int port, unsigned int * filedata, unsigned int baseaddr );
/* Check PTP udata from file against udata space */
DSDT_STATUS ptp_check_udata (int port,  unsigned int * filedata, unsigned int baseaddr );

/* Write DONE to udata spaces to just make PTP pass packet along */
/* (Enric modified this as follows) */
/* for parser (both ingress and egress): addr 0 & 1 => HALT */
/* for update: for ingress: addr 0 => HALT, addr 1 => OUT EOP, addr 2 => HALT */
/*             for egress : addr 0 => HALT, addr 1 => OUT HFT, addr 2 => OUT EOP, addr 3 => HALT  */
DSDT_STATUS ptp_1step_setup_simple_udata (int port);

/* return LUT value adjusted for field position */
long long lut_match_fld_val(PTP_MATCH field, long long value );
/* return LUT value extracted from vector */
long long  lut_match_fld_extract( PTP_MATCH field, long long vector);

/* return TX LUT value adjusted for field position */
unsigned short lut_tx_action_fld_val( PTP_TX_ACTION field, unsigned short value );

/* return TX LUT value extracted from vector  */
unsigned long long lut_tx_action_fld_extract (PTP_TX_ACTION field, unsigned long long vector );

/* return RX LUT value adjusted for field position */
unsigned short lut_rx_action_fld_val( PTP_RX_ACTION field, unsigned short value );

/* return RX LUT value extracted from vector  */
unsigned long long lut_rx_action_fld_extract (PTP_RX_ACTION field, unsigned long long vector );

/* return flag value adjusted for flag position */
unsigned long long lut_flag_fld_val(int flag, unsigned long long value );
/* program lut entry */
/* lut_match: 64 bit match fields */
/* lut_valid: 64 bit valid fields (correspond to lut_match) */
/* lut_action: 64 bit action fields */
/* lut_index: index of LUT entry to program */
/* path: eg0_ptp for egress or ig0_ptp for ingress */
DSDT_STATUS prgm_lut_entry (int port, unsigned long long lut_key, unsigned long long lut_valid, unsigned long lut_key_upper, unsigned long lut_valid_upper, unsigned long long lut_action,  short lut_index, int path );
/* Read lut entry */
DSDT_STATUS read_lut_entry (int port, unsigned long long *lut_key, unsigned long long *lut_valid, unsigned long *lut_key_upper, unsigned long *lut_valid_upper, unsigned long long *lut_action, short lut_index, int path );
/* Check lut entry in memory */
DSDT_STATUS check_lut_entry (int port,  int lut_index, int path );
/* find entry, that has required action */
DSDT_STATUS ptp_find_lut_action(int port, PTP_1STEP_LUT_ACT action, PTP_Blk path, int *entryNum);
/* Enable/Disable lut action */
DSDT_STATUS ptp_enable_lut_action(int port, int idx, PTP_1STEP_LUT_ACT action, PTP_Blk path,DSDT_BOOL en ) ;
/* init LUT */
DSDT_STATUS lut_init(void);
/* print PTP stats */
DSDT_STATUS get_ptp_stats1 (int port);
/* dump memory */
DSDT_STATUS ptp_1step_dump_mem (int port, unsigned int addr, int range);
/* memory test */
DSDT_STATUS ptp_1step_write_fs_check (int port, unsigned int addr, int range, unsigned int mask);
/* memory test */
DSDT_STATUS ptp_1step_write_unique_check (int port, unsigned short addr, int range, unsigned int mask);

/* udata mem dump */
DSDT_STATUS ptp_1step_mem_dump_update_udata (int port);
/* udata mem test */
DSDT_STATUS ptp_1step_mem_test_update_udata (int port);
/* udata mem test */
DSDT_STATUS ptp_1step_mem_test_parser_udata (int port);

/* prints out the ptp registers (portnumber 'portnum') */
DSDT_STATUS get_ptp_regs ( int portnum, PTP_Blk path );
/* prints out the ptp statistics (portnumber 'portnum' and path 'egr|igr') */
DSDT_STATUS get_ptp_stats ( int portnum, PTP_Blk path );

/* prints out the ptp pam (parser and update) contents (portnumber 'portnum' and path 'egr|igr') */
DSDT_STATUS get_ptp_pam ( int portnum, PTP_Blk path );

/* prints out the ptp lut (key, mask and action) contents (portnumber 'portnum' and path 'egr|igr') */
DSDT_STATUS get_ptp_lut ( int portnum, PTP_Blk path );
/*get_ptp_regs 0 igr */
/*get_ptp_regs 0 egr */
/*get_ptp_stats 0 igr */
/*get_ptp_stats 0 egr */
/*get_ptp_pam 0 igr */
/*get_ptp_pam 0 egr */
/*get_ptp_lut 0 igr */
/*get_ptp_lut 0 egr */

unsigned long long lut_key_adjust ( PTP_KEY name, unsigned long long value );
unsigned long long lut_vld_adjust ( PTP_KEY name );
/*  global lut_key_pos */
/*  global lut_key_size */

unsigned long long lut_act_adjust ( PTP_1STEP_LUT_ACT name, unsigned long long value );

DSDT_STATUS ptp_lut_clear (int port);
/**demo LUT */
DSDT_STATUS ptp_lut_init (int port);

/* init example */

DSDT_STATUS get_ptp_tai(void);

DSDT_STATUS get_ptp_tai_tod(int port, unsigned int *_ns, unsigned int *_sec_lo, unsigned int *_sec_hi);
DSDT_STATUS _set_ptp_tai_tod(int port, unsigned int ns, unsigned int sec_lo, unsigned int sec_hi);
DSDT_STATUS get_ptp_tai_frc(int port, unsigned int *_ns, unsigned int *_sec_lo, unsigned int *_sec_hi);
DSDT_STATUS set_ptp_tai_frc(int port, unsigned int ns, unsigned int sec_lo, unsigned int sec_hi);
DSDT_STATUS get_ptp_tai_drift_adjust (int port, long *adj);
DSDT_STATUS set_ptp_tai_drift_adjust (int port, long adj);
DSDT_STATUS get_ptp_tai_step(int port, long *nanosec, long *fracnanosec);
DSDT_STATUS set_ptp_tai_step(int port, long nanosec, long fracnanosec);

int get_ptp_tsql_usage(int portnum, PTP_Blk path);
int get_ptp_tsqh_usage(int portnum, PTP_Blk path);

DSDT_STATUS ptp_get_tsql_part(int portnum, PTP_Blk path, int count, PtpTsQueueStruct *tsQueue);
DSDT_STATUS ptp_get_tsqh_part(int portnum, PTP_Blk path, int count, PtpTsQueueStruct *tsQueue);

DSDT_STATUS ptp_get_tsql(int portnum, PTP_Blk path, PtpTsQueueStruct *tsQueue);
DSDT_STATUS ptp_get_tsqh(int portnum, PTP_Blk path, PtpTsQueueStruct *tsQueue);

DSDT_STATUS get_ptp_tsql_data (int portnum,  PTP_Blk path, int numhwords);

DSDT_STATUS get_ptp_tsqh_data (int portnum,  PTP_Blk path, int numhwords);
DSDT_STATUS get_ptp_tsqh_drain (int portnum,  PTP_Blk path);
DSDT_STATUS get_ptp_tsql_drain (int portnum,  PTP_Blk path);

DSDT_STATUS get_ptp_latadj (int portnum );
DSDT_STATUS set_ptp_latadj (int portnum, int ns );

DSDT_STATUS get_ptp_stt_timer (int portnum );

DSDT_STATUS set_ptp_stt_timer (int portnum, unsigned int cycles);
DSDT_STATUS set_ptp_disp_timer (int portnum, unsigned int cycles);

DSDT_STATUS get_ptp_stuff (int port);

DSDT_STATUS ptp_lut_update (int port, unsigned long long delete_lut_key , 
                     unsigned long long delete_lut_vld,
                     unsigned long long delete_lut_act,
                     unsigned long long add_lut_key , 
                     unsigned long long add_lut_vld,
                     unsigned long long add_lut_act,
					 short lut_index, int path );

DSDT_STATUS set_ptp_tai_reg (int port, PTP_1STEP_TAI_REGS regname, unsigned int value );
/* Set PTP register */
DSDT_STATUS get_ptp_tai_reg (int port, PTP_1STEP_TAI_REGS regname, unsigned int *value  ); 

/* set drift adjustment (frac nsec) */
DSDT_STATUS set_drift_adj (int port, unsigned int frac_ns);
/* get drift adjustment (frac nsec) */
DSDT_STATUS get_drift_adj (int port, unsigned int *frac_ns);
/* set drift adjustment via step size (frac nsec) */
DSDT_STATUS set_drift_adj_step (int port, unsigned int frac_ns);
/* get drift adjustment via step size (frac nsec) */
DSDT_STATUS get_drift_adj_step (int port, unsigned int *frac_ns);
/* Clear ELU entry */
DSDT_STATUS clr_elu_entry (int portnum, int num);
/* Clear ILU entry */
DSDT_STATUS clr_ilu_entry (int portnum, int num);
/* programs igr replay table entry to 1 */
DSDT_STATUS set_igr_rply (int portnum, int num);
DSDT_STATUS disable_macsec_both (int port);
/* TAI / TOD */
/* Get PTP time register  */
/* Valid regname is one of: tod_0, frc_0, trig_gen_tod_0, trig_gen_msk_0, tod_load_0, tod_cap_0, tod_cap1_0, next_rt_0 */
DSDT_STATUS get_ptp_tai_timereg (int port, PTP_1STEP_TAI_REGS regname,unsigned int *sec_hi,unsigned int *sec_lo,unsigned int *ns, unsigned int *fs);
/* Set PTP time register  */
/* Valid regname is one of: tod_0, frc_0, trig_gen_tod_0, trig_gen_msk_0, tod_load_0, tod_cap_0, tod_cap1_0, next_rt_0 */
DSDT_STATUS set_ptp_tai_timereg (int port, PTP_1STEP_TAI_REGS regname,unsigned int sec_hi,unsigned int sec_lo,unsigned int ns, unsigned int fs);
/* return value with modified field  */
unsigned int tai_tod_cfg_gen_set_field (unsigned int oldvalue, int field, unsigned int value);
/* return value extracted from vector */
unsigned int tai_tod_cfg_gen_extract(int field, int vector);

/* return value with modified field  */
unsigned int tai_tod_func_cfg_set_field(unsigned int oldvalue, int field, unsigned int value) ;

/* return value extracted from vector */
unsigned int tai_tod_func_cfg_extract(int field, int vector);
/* Show TAI TOD cfg gen */
void ptp_1step_print_tai_tod_func_cfg (int port);

/* ------------------------------------------------------------------------------------ zzz */
DSDT_STATUS set_tai_pulse_in_cnt (int port, unsigned int val);

DSDT_STATUS get_tai_pulse_in_cnt (int port);
DSDT_STATUS set_tai_pulse_delay (int port, unsigned int val);

DSDT_STATUS hard_reset (int port);
DSDT_STATUS msec_reset (int port);
DSDT_STATUS ptp_lut_default (int port);
/* entry 14 */
/* if parser is interrupted => do nothing */
DSDT_STATUS ptp_lut_interrupt (int port);
DSDT_STATUS set_hard_drop_size (int port, unsigned int size);
DSDT_STATUS set_tai_ntp_mode (int port);
unsigned int get_version_number (int port);

DSDT_STATUS set_wmac_ipg (int port, unsigned int ipg );
DSDT_STATUS set_smac_ipg (int port, unsigned int ipg );
unsigned int get_wmac_ipg (int port);
unsigned int get_smac_ipg (int port);
DSDT_STATUS set_wiremac_ipg_10 (int port);
DSDT_STATUS set_wiremac_ipg_11 (int port);
DSDT_STATUS set_wiremac_ipg_12 (int port);

DSDT_STATUS set_sysmac_ipg_11 (int port);
DSDT_STATUS set_sysmac_ipg_12 (int port);



/* tod_func value: 0 - update, 1 -increment, 2- decrement, 3-capture */
/* trig - tod_func_trig, default 0, triggers tod function when set to 1 */
DSDT_STATUS set_tai_tod_func (int port, unsigned int tod_func, unsigned int trig);
/* set tod_cap_cfg bits cap_ */
DSDT_STATUS set_tod_cap_cfg (int port, unsigned int val);
DSDT_STATUS set_elu_entry_match_da (int portnum, int en, int da);

DSDT_STATUS set_egr_def_drop (int port, int flag, int portnum);
DSDT_STATUS ptp_update_set_udata_ntp (int port);
DSDT_STATUS ptp_update_check_udata_ntp (int port);
DSDT_STATUS ptp_parser_set_udata_ntp (int port);
DSDT_STATUS ptp_parser_check_udata_ntp (int port);
DSDT_STATUS ptp_lut_init_ntp (int port);
DSDT_STATUS ptp_lut_init_ntp_2 (int port, int udp_src_port, int udp_dst_port);
DSDT_STATUS set_ntp_tod_mode (int port, unsigned int ntp_mode);

unsigned long long  prgm_ptp_lut_acts (PTP_1STEP_LUT_ACT action);
DSDT_STATUS prgm_ptp_lut_ent_msgType(int port, int idx, PTPv2_message_type msgId, PTP_1STEP_LUT_ACT actions, PTP_Blk path);
/* */
 
DSDT_STATUS set_ptp_timeout_ctrl(int cycles, int portnum);
/* set ingress or egress asymmetry */
DSDT_STATUS set_asymmetry(unsigned int asym, PTP_Blk path, int port);
unsigned int get_asymmetry(PTP_Blk path, int port);
/* set ingress linkdelay */
DSDT_STATUS set_link_delay(int ldelay, int port);

#ifdef __cplusplus
}
#endif

#endif /* __PTP_1STEP_H */
