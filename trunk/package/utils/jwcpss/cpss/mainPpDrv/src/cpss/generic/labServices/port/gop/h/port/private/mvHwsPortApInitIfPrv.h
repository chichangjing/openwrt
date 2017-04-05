/*******************************************************************************
*                Copyright 2001, Marvell International Ltd.
* This code contains confidential information of Marvell semiconductor, inc.
* no rights are granted herein under any patent, mask work right or copyright
* of Marvell or any third party.
* Marvell reserves the right at its sole discretion to request that this code
* be immediately returned to Marvell. This code is provided "as is".
* Marvell makes no warranties, express, implied or otherwise, regarding its
* accuracy, completeness or performance.
********************************************************************************
* mvHwsPortApInitIfPrv.h
*
* DESCRIPTION: Internal definition.
*
*
* DEPENDENCIES:
*
* FILE REVISION NUMBER:
*       $Revision: 22 $
******************************************************************************/

#ifndef __mvHwSPortApIfPrv_H
#define __mvHwSPortApIfPrv_H

#ifdef __cplusplus
extern "C" {
#endif

#include <common/siliconIf/mvSiliconIf.h>
#ifdef  Z80_COMP
#include <port/private/mvZ80EnvDep.h>
#endif
#define AP_Ports_Num (12)
#define CO_PROC_MEMORY_BASE (0x30000)
#define CO_PROC_GP_SRAM_BASE (0x30000) /* change to 0x20000 for bobcat2 */

/*
 * Registers used by AP.
 */
typedef enum
{
    PCS40G_Common_Control = 0x088C0414,
    PCS_reset_reg = 0x088C054C,
    AP_configuration_reg_0 = 0x088C053C,
    AP_configuration_reg_1 = 0x088C0540,
    AP_status_reg_0 = 0x088C0544,
    AP_status_reg_1 = 0x088C0548,
    AP_internal_regisers = 0x088C0600

}MV_HWS_AP_REGISTERS;

/*
 * bit map for the arbiter FSM states
*/
typedef enum
{
  ST_AN_ENABLE    = 1,
  ST_TX_DISABLE   = 2,
  ST_LINK_STAT_CK = 4,
  ST_PARALLEL_FLT = 8,
  ST_ABILITY_DET  = 16,
  ST_ACK_DETECT   = 32,
  ST_COMPLETE_ACK = 64,
  ST_NP_WAIT      = 128,
  ST_AN_GOOD_CK   = 256,
  ST_AN_GOOD      = 512,
  ST_SERDES_WAIT  = 1024

}MV_HWS_AP_SM_STATE;

/* timer names */
enum
{
    ArbFsmPolling = 1,
    BreakLink,
    AbilityDetecPolling,
    AutoNegWait,
    LinkFailInhibit

};

/* AP port request states */
enum
{
    Idle = 0,
    Prepare_AP,
    Arb_Fsm_Wait,
    Arb_Fsm_Wait_Done,
    Tx_Disable_Wait,
    Tx_Disable_Wait_Done,
    Ability_Detect_Wait,
    Ability_Detect_Wait_Done,
    An_Wait,
    An_Wait_Done,
    Power_Up_Link,
    Link_Fail_Inh_Wait,
    Link_Fail_Inh_Done,
    Link_Up
};

/*  HCDPortType  [11:12]
                0 = 1000Base_KX
                1 = 10GBase_KX4
                2 = 10GBase_R
                3 = 40GBase_R */
enum
{
  Port_1000Base_KX,
  Port_10GBase_KX4,
  Port_10GBase_R,
  Port_40GBase_R
};

/* Timers link list entry for each timer */
typedef struct
{
    unsigned char  timerName;
    unsigned char  isActive;
    unsigned short period;
}MV_HWS_AP_TIMER_INFO;

/* ALL Global variables for reference must be packed into structure */
/* AP control register: (write by Host, read by Z80)
  Enable       [0:0]
  LaneNumber   [1:6]
  FCPauseEn    [7:7]
  FCAsmDir     [8:8]
  FECSupEn     [9:9]
  FECReq       [10:10]
  LoopBackEn   [11:11]
  PcsNumber    [12:15]
  MacNumber    [16:19]
  Adv_40GBase_KR4 [20:20]
  Adv_10GBase_KR    [21:21]
  Adv_10GBase_KX4   [22:22]
  Adv_1000Base_KX   [23:23]
  Adv_20GBase_KR2   [24:24]
  Adv_10GBase_KX2   [25:25]
  Reserved     [26:31]

*/

/* AP status register: (write by Z80, read by Host)
  Active       [0:0]
  State        [1:4] -  AP request state:
                0 = Idle
                1 = Prepare_AP
                2 = Tx_Disable_Wait
                3 = Tx_Disable_Done
                4 = An_Wait
                5 = An_Wait_Done
                6 = Link_Fail_Inh_Wait
                7 = Link_Fail_Inh_Done
                8 = Link_Up

  ActiveError  [5:5] (no resolution; indication from ARB SM)
  HCDFound     [6:6]
  HCDPortType  [7:8]
                0 = 1000Base_KX
                1 = 10GBase_KX4
                2 = 10GBase_R
                3 = 40GBase_R

  HCDFecRes    [9:9]
  HCDLink      [10:10]
  PCSLock      [11:12]
  ARBSmStatus  [13:23]
                ST_AN_ENABLE    = 1,
                ST_TX_DISABLE   = 2,
                ST_LINK_STAT_CK = 4,
                ST_PARALLEL_FLT = 8,
                ST_ABILITY_DET  = 16,
                ST_ACK_DETECT   = 32,
                ST_COMPLETE_ACK = 64,
                ST_NP_WAIT      = 128,
                ST_AN_GOOD_CK   = 256,
                ST_AN_GOOD      = 512,
                ST_SERDES_WAIT  = 1024
  Reserved     [24:31]

*/
typedef struct
{
    unsigned int  checkCounter;
    unsigned int  apPortControl[AP_Ports_Num];
    unsigned int  apPortStatus[AP_Ports_Num];
    MV_HWS_AP_TIMER_INFO apTimerList[AP_Ports_Num];
    unsigned int  apPortActiveLanes[AP_Ports_Num];
    unsigned char minVersion;
    unsigned char fwVersion;
    unsigned char buff[2];
    unsigned int  portResolutionMask;
    unsigned int  portInitMask;
    unsigned int  reserved;
    unsigned int  flag;
    unsigned int  debug[6];
    unsigned int  portDebug[AP_Ports_Num];

}MV_HWS_GLOBALS;


/************************************************************************/
/*			Control Macros                                              */
/************************************************************************/

/* Get / Set fields of the AP port control entry */
#define AP_CONTROL_AP_ENABLE_GET(apPortControl)  (apPortControl & 0x1)
#define AP_CONTROL_AP_ENABLE_SET(apPortControl,val)                     \
            (U32_SET_FIELD(apPortControl, 0, 1,(val & 1)))

#define AP_CONTROL_LANE_NUM_GET(apPortControl)                         \
            (U32_GET_FIELD(apPortControl, 1, 6))
#define AP_CONTROL_LANE_NUM_SET(apPortControl,val)                     \
            (U32_SET_FIELD(apPortControl, 1, 6,(val & 0x3F)))

#define AP_CONTROL_FC_ENABLE_GET(apPortControl)  ((apPortControl >> 7) & 0x1)
#define AP_CONTROL_FC_ENABLE_SET(apPortControl,val)                     \
            (U32_SET_FIELD(apPortControl, 7, 1,(val & 1)))

#define AP_CONTROL_FC_ASM_GET(apPortControl)  ((apPortControl >> 8) & 0x1)
#define AP_CONTROL_FC_ASM_SET(apPortControl,val)                     \
            (U32_SET_FIELD(apPortControl, 8, 1,(val & 1)))

#define AP_CONTROL_FEC_ABIL_GET(apPortControl)  ((apPortControl >> 9) & 0x1)
#define AP_CONTROL_FEC_ABIL_SET(apPortControl,val)                     \
            (U32_SET_FIELD(apPortControl, 9, 1,(val & 1)))

#define AP_CONTROL_FEC_REQ_GET(apPortControl)  ((apPortControl >> 10) & 0x1)
#define AP_CONTROL_FEC_REQ_SET(apPortControl,val)                     \
            (U32_SET_FIELD(apPortControl, 10, 1,(val & 1)))

#define AP_CONTROL_LB_EN_GET(apPortControl)  ((apPortControl >> 11) & 0x1)
#define AP_CONTROL_LB_EN_SET(apPortControl,val)                     \
            (U32_SET_FIELD(apPortControl, 11, 1,(val & 1)))

#define AP_CONTROL_PCS_NUM_GET(apPortControl)                         \
            (U32_GET_FIELD(apPortControl, 12, 4))
#define AP_CONTROL_PCS_NUM_SET(apPortControl,val)                     \
            (U32_SET_FIELD(apPortControl, 12, 4,(val & 0xF)))

#define AP_CONTROL_MAC_NUM_GET(apPortControl)                         \
            (U32_GET_FIELD(apPortControl, 16, 4))
#define AP_CONTROL_MAC_NUM_SET(apPortControl,val)                     \
            (U32_SET_FIELD(apPortControl, 16, 4,(val & 0xF)))

#define AP_CONTROL_ADV_40GBase_KR4_GET(apPortControl)  ((apPortControl >> 20) & 0x1)
#define AP_CONTROL_ADV_40GBase_KR4_SET(apPortControl,val)                     \
            (U32_SET_FIELD(apPortControl, 20, 1,(val & 1)))

#define AP_CONTROL_ADV_10GBase_KX4_GET(apPortControl)  ((apPortControl >> 21) & 0x1)
#define AP_CONTROL_ADV_10GBase_KX4_SET(apPortControl,val)                     \
            (U32_SET_FIELD(apPortControl, 21, 1,(val & 1)))

#define AP_CONTROL_ADV_10GBase_KR_GET(apPortControl)  ((apPortControl >> 22) & 0x1)
#define AP_CONTROL_ADV_10GBase_KR_SET(apPortControl,val)                     \
            (U32_SET_FIELD(apPortControl, 22, 1,(val & 1)))

#define AP_CONTROL_ADV_1000Base_KX_GET(apPortControl)  ((apPortControl >> 23) & 0x1)
#define AP_CONTROL_ADV_1000Base_KX_SET(apPortControl,val)                     \
            (U32_SET_FIELD(apPortControl, 23, 1,(val & 1)))

#define AP_CONTROL_ALL_ADV_MODE_GET(apPortControl)  ((apPortControl >> 20) & 0xf)
#define AP_CONTROL_ALL_ADV_MODE_SET(apPortControl,val)                     \
            (U32_SET_FIELD(apPortControl, 20, 4,(val & 0xf)))

/* on lane swap this macro is used to get the port's PCS (while AP_CONTROL_PCS_NUM_GET
   is used to get the AP PCS num */
#define AP_CONTROL_PORT_PCS_NUM_GET(apPortControl)                         \
			(U32_GET_FIELD(apPortControl, 24, 4))
#define AP_CONTROL_PORT_PCS_NUM_SET(apPortControl,val)                     \
            (U32_SET_FIELD(apPortControl, 24, 4,(val & 0xF)))

#define AP_CONTROL_HOST_LOCK_GET(apPortControl)                \
			(U32_GET_FIELD(apPortControl, 31, 1))
#define AP_CONTROL_HOST_LOCK_SET(apPortControl,val)            \
            (U32_SET_FIELD(apPortControl, 31, 1,(val & 0x1)))


/************************************************************************/
/*			Status Macros                                               */
/************************************************************************/


/* Get / Set fields of the AP port status entry */
#define AP_STATUS_AP_ACTIVE_GET(apPortControl)  (apPortControl & 0x1)
#define AP_STATUS_AP_ACTIVE_SET(apPortControl,val)                     \
            (U32_SET_FIELD(apPortControl, 0, 1,(val & 1)))

#define AP_STATUS_REQ_STATE_GET(apPortControl)                         \
            (U32_GET_FIELD(apPortControl, 1, 4))
#define AP_STATUS_REQ_STATE_SET(apPortControl,val)                     \
            ((apPortControl) = ((apPortControl & 0xFFFFFFE1) | ((val & 0xF) << 1)))
     /*0x1e*/       /*(U32_SET_FIELD(apPortControl, 1, 4,(val & 0xF)))*/

#define AP_STATUS_AP_ERROR_GET(apPortControl) (U32_GET_FIELD(apPortControl, 5, 1))
#define AP_STATUS_AP_ERROR_SET(apPortControl,val)                     \
            (U32_SET_FIELD((apPortControl, 5, 1,(val & 1)))

#define AP_STATUS_HCD_FOUND_GET(apPortControl)                         \
            (U32_GET_FIELD(apPortControl, 6, 1))
#define AP_STATUS_HCD_FOUND_SET(apPortControl,val)                     \
            (U32_SET_FIELD(apPortControl, 6, 1,(val & 0x1)))

#define AP_STATUS_HCD_PORT_TYPE_GET(apPortControl)                         \
            (U32_GET_FIELD(apPortControl, 7, 2))
#define AP_STATUS_HCD_PORT_TYPE_SET(apPortControl,val)                     \
            ((apPortControl) = ((apPortControl & 0xFFFFFE7F) | (val & 0x3) << 7))
    /*0x180*/       /*(U32_SET_FIELD(apPortControl, 7, 2,(val & 0x3)))*/

#define AP_STATUS_HCD_FEC_RES_GET(apPortControl)                         \
            (U32_GET_FIELD(apPortControl, 9, 1))
#define AP_STATUS_HCD_FEC_RES_SET(apPortControl,val)                     \
            (U32_SET_FIELD(apPortControl, 9, 1,(val & 0x1)))

#define AP_STATUS_HCD_LINK_GET(apPortControl)                         \
            (U32_GET_FIELD(apPortControl, 10, 1))
#define AP_STATUS_HCD_LINK_SET(apPortControl,val)                     \
            (U32_SET_FIELD(apPortControl, 10, 1,(val & 0x1)))

#define AP_STATUS_PCS_LOCK_GET(apPortControl)                         \
            (U32_GET_FIELD(apPortControl, 11, 2))
#define AP_STATUS_PCS_LOCK_SET(apPortControl,val)                     \
            (U32_SET_FIELD(apPortControl, 11, 2,(val & 0x3)))

#define AP_STATUS_ARB_FSM_STATE_GET(apPortControl)                         \
            (U32_GET_FIELD(apPortControl, 13, 11))
#define AP_STATUS_ARB_FSM_STATE_SET(apPortControl,val)                     \
            (U32_SET_FIELD(apPortControl, 13, 11,(val & 0x7FF)))

#define AP_STATUS_HCD_FC_RX_RES_GET(apPortControl)                         \
            (U32_GET_FIELD(apPortControl, 24, 1))

#define AP_STATUS_HCD_FC_RX_RES_SET(apPortControl,val)                     \
            (U16_SET_FIELD(apPortControl, 8, 1,(val & 0x1)))
            /*(U32_SET_FIELD(apPortControl, 24, 1,(val & 0x1)))*/
            /* Z80 can't handle shift for more than 16 bits */

#define AP_STATUS_HCD_FC_TX_RES_GET(apPortControl)                         \
            (U32_GET_FIELD(apPortControl, 25, 1))

#define AP_STATUS_HCD_FC_TX_RES_SET(apPortControl,val)                     \
            (U16_SET_FIELD(apPortControl, 9, 1,(val & 0x1)))
            /*(U32_SET_FIELD(apPortControl, 25, 1,(val & 0x1)))*/
            /* Z80 can't handle shift for more than 16 bits */

#define AP_STATUS_PENDING_DELETE_GET(apPortControl)                \
			(U32_GET_FIELD(apPortControl, 30, 1))
#define AP_STATUS_PENDING_DELETE_SET(apPortControl,val)            \
            (U32_SET_FIELD(apPortControl, 30, 1,(val & 0x1)))

#define AP_STATUS_AP_LOCK_GET(apPortControl)                \
			(U32_GET_FIELD(apPortControl, 31, 1))
#define AP_STATUS_AP_LOCK_SET(apPortControl,val)            \
            (U32_SET_FIELD(apPortControl, 31, 1,(val & 0x1)))


#define AP_STATUS_RESET(apPortControl) \
            {                                               \
            AP_STATUS_HCD_LINK_SET(apPortControl, 0);       \
            AP_STATUS_HCD_FOUND_SET(apPortControl, 0);      \
            AP_STATUS_HCD_FEC_RES_SET(apPortControl, 0);    \
            AP_STATUS_HCD_FC_RX_RES_SET(apPortControl, 0);  \
            AP_STATUS_HCD_FC_TX_RES_SET(apPortControl, 0);  \
            AP_STATUS_HCD_PORT_TYPE_SET(apPortControl, 0);  \
            }


#ifdef __cplusplus
}
#endif

#endif /* __mvHwSPortApIfPrv_H */


