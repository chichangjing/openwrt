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
* mvHwsPortCtrlApDefs.h
*
* DESCRIPTION:
*       AP Global definitions
*
* DEPENDENCIES:
*
* FILE REVISION NUMBER:
*       $Revision: 17 $
******************************************************************************/

#ifndef __mvHwServicesPortCtrlApDefs_H
#define __mvHwServicesPortCtrlApDefs_H

#ifdef __cplusplus
extern "C" {
#endif

/* Port Control modules definition */
typedef enum
{
    M0_DEBUG           = 0,
    M1_SUPERVISOR      = 1,
    M2_PORT_MNG        = 2,
    M3_PORT_SM         = 3,
    M4_AP_PORT_MNG     = 4,
    M5_AP_PORT_DET     = 5,
    M5_AP_PORT_DET_EXT = 6,
    M6_GENERAL         = 7,
    MAX_MODULE         = 8

}MV_PORT_CTRL_MODULE;

typedef enum
{
    PORT_CTRL_AP_REG_CFG_0                                   = 1,
    PORT_CTRL_AP_REG_CFG_1                                   = 2,
    PORT_CTRL_AP_REG_ST_0                                    = 3,
    PORT_CTRL_AP_INT_REG_802_3_AP_CTRL                       = 4,
    PORT_CTRL_AP_INT_REG_802_3_AP_ST                         = 5,
    PORT_CTRL_AP_INT_REG_802_3_AP_ADV_REG_1                  = 6,
    PORT_CTRL_AP_INT_REG_802_3_AP_ADV_REG_2                  = 7,
    PORT_CTRL_AP_INT_REG_802_3_AP_ADV_REG_3                  = 8,
    PORT_CTRL_AP_INT_REG_802_3_LP_BASE_ABILITY_REG_1         = 9,
    PORT_CTRL_AP_INT_REG_802_3_LP_BASE_ABILITY_REG_2         = 10,
    PORT_CTRL_AP_INT_REG_802_3_LP_BASE_ABILITY_REG_3         = 11,
    PORT_CTRL_AP_INT_REG_ANEG_CTRL_0                         = 12,
    PORT_CTRL_AP_INT_REG_ANEG_CTRL_1                         = 13,
    PORT_CTRL_AP_INT_REG_802_3_NEXT_PG_TX_REG                = 14,
    PORT_CTRL_AP_INT_REG_802_3_NEXT_PG_TX_CODE_0_15          = 15,
    PORT_CTRL_AP_INT_REG_802_3_NEXT_PG_TX_CODE_16_31         = 16,
    PORT_CTRL_AP_INT_REG_802_3_LP_NEXT_PG_ABILITY_REG        = 17,
    PORT_CTRL_AP_INT_REG_802_3_LP_NEXT_PG_ABILITY_CODE_0_15  = 18,
    PORT_CTRL_AP_INT_REG_802_3_LP_NEXT_PG_ABILITY_CODE_16_31 = 19,
    PORT_CTRL_AP_INT_REG_802_3_BP_ETH_ST_REG                 = 20,
    PORT_CTRL_AP_INT_REG_802_3_LP_ADV_REG                    = 21,
    PORT_CTRL_MAX_AP_REGS

}MV_PORT_CTRL_AP_REGS;


#define AP_CTRL_GET(param, mask, shift) (((param) >> (shift)) & (mask))
#define AP_CTRL_SET(param, mask, shift, val) \
{ \
    param &= (~((mask) << (shift))); \
    param |= ((val) << (shift)); \
}

/*
** AP Configuration
** ================
*/

/*
** Interface Number
** [00:07] Lane Number
** [08:15] PCS Number
** [16:23] MAC Number
** [24:31] Reserved
*/
#define AP_CTRL_LANE_MASK                  (0x7F)
#define AP_CTRL_LANE_SHIFT                 (0)
#define AP_CTRL_LANE_GET(ifNum)            AP_CTRL_GET(ifNum, AP_CTRL_LANE_MASK, AP_CTRL_LANE_SHIFT)
#define AP_CTRL_LANE_SET(ifNum, val)       AP_CTRL_SET(ifNum, AP_CTRL_LANE_MASK, AP_CTRL_LANE_SHIFT, val)

#define AP_CTRL_PCS_MASK                   (0x7F)
#define AP_CTRL_PCS_SHIFT                  (8)
#define AP_CTRL_PCS_GET(ifNum)             AP_CTRL_GET(ifNum, AP_CTRL_PCS_MASK, AP_CTRL_PCS_SHIFT)
#define AP_CTRL_PCS_SET(ifNum, val)        AP_CTRL_SET(ifNum, AP_CTRL_PCS_MASK, AP_CTRL_PCS_SHIFT, val)

#define AP_CTRL_MAC_MASK                   (0x7F)
#define AP_CTRL_MAC_SHIFT                  (16)
#define AP_CTRL_MAC_GET(ifNum)             AP_CTRL_GET(ifNum, AP_CTRL_MAC_MASK, AP_CTRL_MAC_SHIFT)
#define AP_CTRL_MAC_SET(ifNum, val)        AP_CTRL_SET(ifNum, AP_CTRL_MAC_MASK, AP_CTRL_MAC_SHIFT, val)

/*
** Capability
** [00:00] Advertisement 40GBase KR4
** [01:01] Advertisement 10GBase KR
** [02:02] Advertisement 10GBase KX4
** [03:03] Advertisement 1000Base KX
** [04:04] Advertisement 20GBase KR2
** [05:05] Advertisement 10GBase KX2
** [06:15] Reserved
*/
#define AP_CTRL_ADV_MASK                   (0x1)
#define AP_CTRL_40GBase_KR4_SHIFT          (0)
#define AP_CTRL_40GBase_KR4_GET(adv)       AP_CTRL_GET(adv, AP_CTRL_ADV_MASK, AP_CTRL_40GBase_KR4_SHIFT)
#define AP_CTRL_40GBase_KR4_SET(adv, val)  AP_CTRL_SET(adv, AP_CTRL_ADV_MASK, AP_CTRL_40GBase_KR4_SHIFT, val)

#define AP_CTRL_10GBase_KR_SHIFT           (1)
#define AP_CTRL_10GBase_KR_GET(adv)        AP_CTRL_GET(adv, AP_CTRL_ADV_MASK, AP_CTRL_10GBase_KR_SHIFT)
#define AP_CTRL_10GBase_KR_SET(adv, val)   AP_CTRL_SET(adv, AP_CTRL_ADV_MASK, AP_CTRL_10GBase_KR_SHIFT, val)

#define AP_CTRL_10GBase_KX4_SHIFT          (2)
#define AP_CTRL_10GBase_KX4_GET(adv)       AP_CTRL_GET(adv, AP_CTRL_ADV_MASK, AP_CTRL_10GBase_KX4_SHIFT)
#define AP_CTRL_10GBase_KX4_SET(adv, val)  AP_CTRL_SET(adv, AP_CTRL_ADV_MASK, AP_CTRL_10GBase_KX4_SHIFT, val)

#define AP_CTRL_1000Base_KX_SHIFT          (3)
#define AP_CTRL_1000Base_KX_GET(adv)       AP_CTRL_GET(adv, AP_CTRL_ADV_MASK, AP_CTRL_1000Base_KX_SHIFT)
#define AP_CTRL_1000Base_KX_SET(adv, val)  AP_CTRL_SET(adv, AP_CTRL_ADV_MASK, AP_CTRL_1000Base_KX_SHIFT, val)

#define AP_CTRL_20GBase_KR2_SHIFT          (4)
#define AP_CTRL_20GBase_KR2_GET(adv)       AP_CTRL_GET(adv, AP_CTRL_ADV_MASK, AP_CTRL_20GBase_KR2_SHIFT)
#define AP_CTRL_20GBase_KR2_SET(adv, val)  AP_CTRL_SET(adv, AP_CTRL_ADV_MASK, AP_CTRL_20GBase_KR2_SHIFT, val)

#define AP_CTRL_10GBase_KX2_SHIFT          (5)
#define AP_CTRL_10GBase_KX2_GET(adv)       AP_CTRL_GET(adv, AP_CTRL_ADV_MASK, AP_CTRL_10GBase_KX2_SHIFT)
#define AP_CTRL_10GBase_KX2_SET(adv, val)  AP_CTRL_SET(adv, AP_CTRL_ADV_MASK, AP_CTRL_10GBase_KX2_SHIFT, val)

#define AP_CTRL_ADV_ALL_MASK               (0xF)
#define AP_CTRL_ADV_ALL_SHIFT              (0)
#define AP_CTRL_ADV_ALL_GET(adv)           AP_CTRL_GET(adv, AP_CTRL_ADV_ALL_MASK, AP_CTRL_ADV_ALL_SHIFT)
#define AP_CTRL_ADV_ALL_SET(adv, val)      AP_CTRL_SET(adv, AP_CTRL_ADV_ALL_MASK, AP_CTRL_ADV_ALL_SHIFT, val)

/*
** Options
** [00:00] Flow Control Pause Enable
** [01:01] Flow Control Asm Direction
** [02:02] Remote Flow Control Pause Enable
** [03:03] Remote Flow Control Asm Direction
** [04:04] FEC Suppress En
** [05:05] FEC Request
** [06:07] Reserved
** [09:09] loopback Enable
** [10:15] Reserved
*/
#define AP_CTRL_FC_PAUSE_MASK              (0x1)
#define AP_CTRL_FC_PAUSE_SHIFT             (0)
#define AP_CTRL_FC_PAUSE_GET(opt)          AP_CTRL_GET(opt, AP_CTRL_FC_PAUSE_MASK, AP_CTRL_FC_PAUSE_SHIFT)
#define AP_CTRL_FC_PAUSE_SET(opt, val)     AP_CTRL_SET(opt, AP_CTRL_FC_PAUSE_MASK, AP_CTRL_FC_PAUSE_SHIFT, val)

#define AP_CTRL_FC_ASM_MASK                (0x1)
#define AP_CTRL_FC_ASM_SHIFT               (1)
#define AP_CTRL_FC_ASM_GET(opt)            AP_CTRL_GET(opt, AP_CTRL_FC_ASM_MASK, AP_CTRL_FC_ASM_SHIFT)
#define AP_CTRL_FC_ASM_SET(opt, val)       AP_CTRL_SET(opt, AP_CTRL_FC_ASM_MASK, AP_CTRL_FC_ASM_SHIFT, val)

#define AP_CTRL_REM_FC_PAUSE_MASK          (0x1)
#define AP_CTRL_REM_FC_PAUSE_SHIFT         (2)
#define AP_CTRL_REM_FC_PAUSE_GET(opt)      AP_CTRL_GET(opt, AP_CTRL_REM_FC_PAUSE_MASK, AP_CTRL_REM_FC_PAUSE_SHIFT)
#define AP_CTRL_REM_FC_PAUSE_SET(opt, val) AP_CTRL_SET(opt, AP_CTRL_REM_FC_PAUSE_MASK, AP_CTRL_REM_FC_PAUSE_SHIFT, val)

#define AP_CTRL_REM_FC_ASM_MASK            (0x1)
#define AP_CTRL_REM_FC_ASM_SHIFT           (3)
#define AP_CTRL_REM_FC_ASM_GET(opt)        AP_CTRL_GET(opt, AP_CTRL_REM_FC_ASM_MASK, AP_CTRL_REM_FC_ASM_SHIFT)
#define AP_CTRL_REM_FC_ASM_SET(opt, val)   AP_CTRL_SET(opt, AP_CTRL_REM_FC_ASM_MASK, AP_CTRL_REM_FC_ASM_SHIFT, val)

#define AP_CTRL_FEC_ABIL_MASK              (0x1)
#define AP_CTRL_FEC_ABIL_SHIFT             (4)
#define AP_CTRL_FEC_ABIL_GET(opt)          AP_CTRL_GET(opt, AP_CTRL_FEC_ABIL_MASK, AP_CTRL_FEC_ABIL_SHIFT)
#define AP_CTRL_FEC_ABIL_SET(opt, val)     AP_CTRL_SET(opt, AP_CTRL_FEC_ABIL_MASK, AP_CTRL_FEC_ABIL_SHIFT, val)

#define AP_CTRL_FEC_REQ_MASK               (0x1)
#define AP_CTRL_FEC_REQ_SHIFT              (5)
#define AP_CTRL_FEC_REQ_GET(opt)           AP_CTRL_GET(opt, AP_CTRL_FEC_REQ_MASK, AP_CTRL_FEC_REQ_SHIFT)
#define AP_CTRL_FEC_REQ_SET(opt, val)      AP_CTRL_SET(opt, AP_CTRL_FEC_REQ_MASK, AP_CTRL_FEC_REQ_SHIFT, val)

#define AP_CTRL_LB_EN_MASK                 (0x1)
#define AP_CTRL_LB_EN_SHIFT                (9)
#define AP_CTRL_LB_EN_GET(opt)             AP_CTRL_GET(opt, AP_CTRL_LB_EN_MASK, AP_CTRL_LB_EN_SHIFT)
#define AP_CTRL_LB_EN_SET(opt, val)        AP_CTRL_SET(opt, AP_CTRL_LB_EN_MASK, AP_CTRL_LB_EN_SHIFT, val)

/*
** AP Status Info
** ==============
**/

/*
** HCD Status
** [03:03] Found
** [04:04] ARBSmError
** [05:05] FEC Result
** [06:06] Flow Control Rx Result
** [07:07] Flow Control Tx Result
** [08:15] HCD Type ------------> [00:00] 40GBase KR4
** [16:16] HCD Interrupt Trigger  [01:01] 10GBase KR
** [17:17] Link Interrupt Trigger [02:02] 10GBase KX4
** [18:30] Reserved               [03:03] 1000Base KX
** [31:31] Link                   [04:04] 20GBase KR2
**                                [05:05] 10GBase KX2
*/
#define AP_ST_HCD_FOUND_MASK                 (0x1)
#define AP_ST_HCD_FOUND_SHIFT                (3)
#define AP_ST_HCD_FOUND_GET(hcd)             AP_CTRL_GET(hcd, AP_ST_HCD_FOUND_MASK, AP_ST_HCD_FOUND_SHIFT)
#define AP_ST_HCD_FOUND_SET(hcd, val)        AP_CTRL_SET(hcd, AP_ST_HCD_FOUND_MASK, AP_ST_HCD_FOUND_SHIFT, val)

#define AP_ST_AP_ERR_MASK                    (0x1)
#define AP_ST_AP_ERR_SHIFT                   (4)
#define AP_ST_AP_ERR_GET(hcd)                AP_CTRL_GET(hcd, AP_ST_AP_ERR_MASK, AP_ST_AP_ERR_SHIFT)
#define AP_ST_AP_ERR_SET(hcd, val)           AP_CTRL_SET(hcd, AP_ST_AP_ERR_MASK, AP_ST_AP_ERR_SHIFT, val)

#define AP_ST_HCD_FEC_RES_MASK               (0x1)
#define AP_ST_HCD_FEC_RES_SHIFT              (5)
#define AP_ST_HCD_FEC_RES_GET(hcd)           AP_CTRL_GET(hcd, AP_ST_HCD_FEC_RES_MASK, AP_ST_HCD_FEC_RES_SHIFT)
#define AP_ST_HCD_FEC_RES_SET(hcd, val)      AP_CTRL_SET(hcd, AP_ST_HCD_FEC_RES_MASK, AP_ST_HCD_FEC_RES_SHIFT, val)

#define AP_ST_HCD_FC_RX_RES_MASK             (0x1)
#define AP_ST_HCD_FC_RX_RES_SHIFT            (6)
#define AP_ST_HCD_FC_RX_RES_GET(hcd)         AP_CTRL_GET(hcd, AP_ST_HCD_FC_RX_RES_MASK, AP_ST_HCD_FC_RX_RES_SHIFT)
#define AP_ST_HCD_FC_RX_RES_SET(hcd, val)    AP_CTRL_SET(hcd, AP_ST_HCD_FC_RX_RES_MASK, AP_ST_HCD_FC_RX_RES_SHIFT, val)

#define AP_ST_HCD_FC_TX_RES_MASK             (0x1)
#define AP_ST_HCD_FC_TX_RES_SHIFT            (7)
#define AP_ST_HCD_FC_TX_RES_GET(hcd)         AP_CTRL_GET(hcd, AP_ST_HCD_FC_TX_RES_MASK, AP_ST_HCD_FC_TX_RES_SHIFT)
#define AP_ST_HCD_FC_TX_RES_SET(hcd, val)    AP_CTRL_SET(hcd, AP_ST_HCD_FC_TX_RES_MASK, AP_ST_HCD_FC_TX_RES_SHIFT, val)

#define AP_ST_HCD_TYPE_MASK                  (0xFF)
#define AP_ST_HCD_TYPE_SHIFT                 (8)
#define AP_ST_HCD_TYPE_GET(hcd)              AP_CTRL_GET(hcd, AP_ST_HCD_TYPE_MASK, AP_ST_HCD_TYPE_SHIFT)
#define AP_ST_HCD_TYPE_SET(hcd, val)         AP_CTRL_SET(hcd, AP_ST_HCD_TYPE_MASK, AP_ST_HCD_TYPE_SHIFT, val)

#define AP_ST_HCD_INT_TRIG_MASK              (0x1)
#define AP_ST_HCD_INT_TRIG_SHIFT             (16)
#define AP_ST_HCD_INT_TRIG_GET(hcd)          AP_CTRL_GET(hcd, AP_ST_HCD_INT_TRIG_MASK, AP_ST_HCD_INT_TRIG_SHIFT)
#define AP_ST_HCD_INT_TRIG_SET(hcd, val)     AP_CTRL_SET(hcd, AP_ST_HCD_INT_TRIG_MASK, AP_ST_HCD_INT_TRIG_SHIFT, val)

#define AP_ST_LINK_INT_TRIG_MASK             (0x1)
#define AP_ST_LINK_INT_TRIG_SHIFT            (17)
#define AP_ST_LINK_INT_TRIG_GET(hcd)         AP_CTRL_GET(hcd, AP_ST_LINK_INT_TRIG_MASK, AP_ST_LINK_INT_TRIG_SHIFT)
#define AP_ST_LINK_INT_TRIG_SET(hcd, val)    AP_CTRL_SET(hcd, AP_ST_LINK_INT_TRIG_MASK, AP_ST_LINK_INT_TRIG_SHIFT, val)

#define AP_ST_HCD_LINK_MASK                  (0x1)
#define AP_ST_HCD_LINK_SHIFT                 (31)
#define AP_ST_HCD_LINK_GET(hcd)              AP_CTRL_GET(hcd, AP_ST_HCD_LINK_MASK, AP_ST_HCD_LINK_SHIFT)
#define AP_ST_HCD_LINK_SET(hcd, val)         AP_CTRL_SET(hcd, AP_ST_HCD_LINK_MASK, AP_ST_HCD_LINK_SHIFT, val)

/*
** ARBSmStatus
** [00:00] ST_AN_ENABLE
** [01:01] ST_TX_DISABLE
** [02:02] ST_LINK_STAT_CK
** [03:03] ST_PARALLEL_FLT
** [04:04] ST_ABILITY_DET
** [05:05] ST_ACK_DETECT
** [06:06] ST_COMPLETE_ACK
** [07:07] ST_NP_WAIT
** [08:08] ST_AN_GOOD_CK
** [09:09] ST_AN_GOOD
** [10:10] ST_SERDES_WAIT
*/
#define AP_ST_ARB_FSM_MASK                   (0x7FF)
#define AP_ST_ARB_FSM_SHIFT                  (0)
#define AP_ST_ARB_FSM_GET(arb)               AP_CTRL_GET(arb, AP_ST_ARB_FSM_MASK, AP_ST_ARB_FSM_SHIFT)
#define AP_ST_ARB_FSM_SET(arb, val)          AP_CTRL_SET(arb, AP_ST_ARB_FSM_MASK, AP_ST_ARB_FSM_SHIFT, val)

/*
** AP Status
** [00:00] Signal Detect
** [01:01] CDR Lock
** [02:02] PCS Lock
** [03:07] Reserved
*/
#define AP_ST_SD_MASK                        (0x1)
#define AP_ST_SD_SHIFT                       (0)
#define AP_ST_SD_GET(status)                 AP_CTRL_GET(arb, AP_ST_SD_MASK, AP_ST_SD_SHIFT)
#define AP_ST_SD_SET(status, val)            AP_CTRL_SET(arb, AP_ST_SD_MASK, AP_ST_SD_SHIFT, val)

#define AP_ST_CDR_LOCK_MASK                  (0x1)
#define AP_ST_CDR_LOCK_SHIFT                 (1)
#define AP_ST_CDR_LOCK_GET(status)           AP_CTRL_GET(arb, AP_ST_CDR_LOCK_MASK, AP_ST_CDR_LOCK_SHIFT)
#define AP_ST_CDR_LOCK_SET(status, val)      AP_CTRL_SET(arb, AP_ST_CDR_LOCK_MASK, AP_ST_CDR_LOCK_SHIFT, val)

#define AP_ST_PCS_LOCK_MASK                  (0x1)
#define AP_ST_PCS_LOCK_SHIFT                 (2)
#define AP_ST_PCS_LOCK_GET(status)           AP_CTRL_GET(arb, AP_ST_PCS_LOCK_MASK, AP_ST_PCS_LOCK_SHIFT)
#define AP_ST_PCS_LOCK_SET(status, val)      AP_CTRL_SET(arb, AP_ST_PCS_LOCK_MASK, AP_ST_PCS_LOCK_SHIFT, val)

#ifdef __cplusplus
}
#endif

#endif /* __mvHwServicesPortCtrlApDefs_H */


