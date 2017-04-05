/******************************************************************************
*              Copyright (c) Marvell International Ltd. and its affiliates
*
* This software file (the "File") is owned and distributed by Marvell
* International Ltd. and/or its affiliates ("Marvell") under the following
* alternative licensing terms.
* If you received this File from Marvell, you may opt to use, redistribute
* and/or modify this File under the following licensing terms.
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following conditions are met:
*  -   Redistributions of source code must retain the above copyright notice,
*       this list of conditions and the following disclaimer.
*  -   Redistributions in binary form must reproduce the above copyright
*       notice, this list of conditions and the following disclaimer in the
*       documentation and/or other materials provided with the distribution.
*  -    Neither the name of Marvell nor the names of its contributors may be
*       used to endorse or promote products derived from this software without
*       specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
* AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
* IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
* ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
* LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
* CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
* SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
* OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
* WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
* OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
* ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*******************************************************************************
* mvHwsPortCtrlAp.h
*
* DESCRIPTION:
*       Port Control AP Definitions
*
* FILE REVISION NUMBER:
*       $Revision: 1 $
*
*******************************************************************************/

#ifndef __mvHwsPortCtrlAp_H
#define __mvHwsPortCtrlAp_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include <mvHwsPortCtrlApDefs.h>

/*******************************************************************************
*                         AP Register Definition                               *
* ============================================================================ *
*******************************************************************************/
#if defined(BC2_DEV_SUPPORT) || defined(AC3_DEV_SUPPORT) || defined(BOBK_DEV_SUPPORT)

/* AP Control Regsiters */
/* ==================== */

/* PCS40G Common Control */
#define PCS40G_COMMON_CTRL_REG                                (0x0014)
/* PCS40G Common Control Field definition */
#define PCS40G_COMMON_CTRL_REG_PCS_MODE_MASK                  (0x0300)
#define PCS40G_COMMON_CTRL_REG_PCS_MODE_SHIFT                 (8)

/* PCS40G Common Status */
#define PCS40G_COMMON_STATUS_REG                              (0x0030)
/* PCS40G Common Status Field definition */
#define PCS40G_COMMON_STATUS_REG_PCS_MODE_MASK                (0x0001)

/* AP configuration reg 0 */
#define AP_REG_CFG_0                                          (0x013C)
/* AP configuration reg 0 Field definition */
#define AP_REG_CFG_0_ADV_MASK                                 (0xF3F0)
#define AP_REG_CFG_0_ADV_SHIFT                                (4)
#define AP_REG_CFG_0_ADV_40G_KR4_MASK                         (0x4000)
#define AP_REG_CFG_0_ADV_1G_KX_MASK                           (0x8000)

/* AP configuration reg 1 */
#define AP_REG_CFG_1                                          (0x0140)
/* AP configuration reg 1 Field definition */
#define AP_REG_CFG_1_ADV_MASK                                 (0x0300)
#define AP_REG_CFG_1_ADV_10G_KR_MASK                          (0x0200)
#define AP_REG_CFG_1_ADV_10G_KX4_MASK                         (0x0100)

/* AP status reg 0 */
#define AP_REG_ST_0                                           (0x0144)
/* AP status reg 0 Field definition */
#define AP_REG_ST_0_AP_PWT_UP_MASK                            (0x000F)
#define AP_REG_ST_0_AP_PWT_UP_10G_KX4_MASK                    (0x0008)
#define AP_REG_ST_0_AP_PWT_UP_1G_KX_MASK                      (0x0004)
#define AP_REG_ST_0_AP_PWT_UP_40G_KR4_MASK                    (0x0002)
#define AP_REG_ST_0_AP_PWT_UP_10G_KR_MASK                     (0x0001)
#define AP_REG_ST_0_HCD_RESOLVED_MASK                         (0x0001)
#define AP_REG_ST_0_HCD_RESOLVED_SHIFT                        (13)

/* PCS Clock Reset */
#define PCS_CLOCK_RESET_REG                                   (0x014C)
/* PCS Clock Reset Field definition */
#define PCS_CLOCK_RESET_REG_AP_RESET_MASK                     (0x0008)
#define PCS_CLOCK_RESET_REG_AP_RESET_SHIFT                    (3)

/* AP Internal Registers */
/* ===================== */

/* 802.3ap Auto-Negotiation Control */
#define AP_INT_REG_802_3_AP_CTRL                              (0x0200)
/* 802.3ap Auto-Negotiation Control Field definition */
#define AP_INT_REG_802_3_AP_CTRL_RST_MASK                     (0x8000)
#define AP_INT_REG_802_3_AP_CTRL_RST_SHIFT                    (15)
#define AP_INT_REG_802_3_AP_CTRL_AN_ENA_MASK                  (0x1000)
#define AP_INT_REG_802_3_AP_CTRL_AN_ENA_SHIFT                 (12)
#define AP_INT_REG_802_3_AP_CTRL_AN_RST_MASK                  (0x0200)
#define AP_INT_REG_802_3_AP_CTRL_AN_RST_SHIFT                 (9)

/* 802.3ap Auto-Negotiation Status */
#define AP_INT_REG_802_3_AP_ST                                (0x0204)

/* 802.3ap Auto-Negotiation Advertisement Register 1 */
#define AP_INT_REG_802_3_AP_ADV_REG_1                         (0x0240)
/* 802.3ap Auto-Negotiation Advertisement Register 1 Field definition */
#define AP_INT_REG_802_3_AP_ADV_REG_1_FC_PAUSE_MASK           (0x0C00)
#define AP_INT_REG_802_3_AP_ADV_REG_1_FC_PAUSE_SHIFT          (10)

/* 802.3ap Auto-Negotiation Advertisement Register 2 */
#define AP_INT_REG_802_3_AP_ADV_REG_2                         (0x0244)

/* 802.3ap Auto-Negotiation Advertisement Register 3 */
#define AP_INT_REG_802_3_AP_ADV_REG_3                         (0x0248)
/* 802.3ap Auto-Negotiation Advertisement Register 3 Field definition */
#define AP_INT_REG_802_3_AP_ADV_REG_3_FEC_MASK                (0xC000)
#define AP_INT_REG_802_3_AP_ADV_REG_3_FEC_SHIFT               (14)

/* 802.3ap Link Partner Base Page Ability Register 1 */
#define AP_INT_REG_802_3_LP_BASE_ABILITY_REG_1                (0x024C)
/* 802.3ap Link Partner Base Page Ability Register 1 Field definition */
#define AP_INT_REG_802_3_LP_BASE_ABILITY_REG_1_FC_PAUSE_SHIFT (10)
#define AP_INT_REG_802_3_LP_BASE_ABILITY_REG_1_FC_DIR_SHIFT   (11)

/* 802.3ap Link Partner Base Page Ability Register 2 */
#define AP_INT_REG_802_3_LP_BASE_ABILITY_REG_2                (0x0250)

/* 802.3ap Link Partner Base Page Ability Register 3 */
#define AP_INT_REG_802_3_LP_BASE_ABILITY_REG_3                (0x0254)

/* 802.3ap Next Page Transmit Register / Extended Next Page Transmit Register */
#define AP_INT_REG_802_3_NEXT_PG_TX_REG                       (0x0258)

/* 802.3ap Extended Next Page Transmit Register Unformatted Code Field U0 to U15 */
#define AP_INT_REG_802_3_NEXT_PG_TX_CODE_0_15                 (0x025C)

/* 802.3ap Extended Next Page Transmit Register Unformatted Code Field U16 to U31 */
#define AP_INT_REG_802_3_NEXT_PG_TX_CODE_16_31                (0x0260)

/* 802.3ap Link Partner Next Page Register / Link Partner Extended Next Page Ability Register */
#define AP_INT_REG_802_3_LP_NEXT_PG_ABILITY_REG               (0x0264)

/* 802.3ap Link Partner Extended Next Page Ability Register Unformatted Code Field U0 to U15 */
#define AP_INT_REG_802_3_LP_NEXT_PG_ABILITY_CODE_0_15         (0x0268)

/* 802.3ap Link Partner Extended Next Page Ability Register Unformatted Code Field U16 to U31 */
#define AP_INT_REG_802_3_LP_NEXT_PG_ABILITY_CODE_16_31        (0x026C)

/* Backplane Ethernet Status Register */
#define AP_INT_REG_802_3_BP_ETH_ST_REG                        (0x0270)

/* EEE Link Partner Advertisement Register */
#define AP_INT_REG_802_3_LP_ADV_REG                           (0x0278)

/* ANEG Control Register 0 */
#define AP_INT_REG_ANEG_CTRL_0                                (0x0280)
/* ANEG Control Register 0  Field definition */
#define AP_INT_REG_ANEG_CTRL_0_ANEG_MASK                      (0x7FFF)
#define AP_INT_REG_ANEG_CTRL_0_ANEG_STATE_MASK                (0x07FF)
#define AP_INT_REG_ANEG_CTRL_0_ANEG_STATE_SHIFT               (4)

/* ANEG Control Register 1 */
#define AP_INT_REG_ANEG_CTRL_1                                (0x0284)
/* ANEG Control Register 1  Field definition */
#define AP_INT_REG_ANEG_CTRL_1_PD_CAP_MASK                    (0x03C0)
#define AP_INT_REG_ANEG_CTRL_1_LB_NONCE_MATCH_MASK            (0x0400)
#define AP_INT_REG_ANEG_CTRL_1_LB_NONCE_MATCH_SHIFT           (10)

#endif


/*******************************************************************************
*                         AP Global Definition                                 *
* ============================================================================ *
*******************************************************************************/

/* AP Port Numbering Definition */
#ifdef BC2_DEV_SUPPORT

#define MV_PORT_CTRL_MAX_AP_PORT_NUM  (24)
#define MV_PORT_CTRL_AP_PORT_NUM_BASE (48)

#elif defined AC3_DEV_SUPPORT

#define MV_PORT_CTRL_MAX_AP_PORT_NUM  (6)
#define MV_PORT_CTRL_AP_PORT_NUM_BASE (24)

#elif defined BOBK_DEV_SUPPORT

#define MV_PORT_CTRL_MAX_AP_PORT_NUM  (16)
#define MV_PORT_CTRL_AP_PORT_NUM_BASE (56)

#endif

#define MV_HWS_PORT_CTRL_AP_LOGICAL_PORT_GET(phyPort) \
                                            (phyPort - MV_PORT_CTRL_AP_PORT_NUM_BASE)
#define MV_HWS_PORT_CTRL_AP_PHYSICAL_PORT_GET(phyPort) \
                                             (phyPort + MV_PORT_CTRL_AP_PORT_NUM_BASE)

/* AP ARB State Machine Numbering Definition */
typedef enum
{
    ST_AN_ENABLE    = 0x001,
    ST_TX_DISABLE   = 0x002,
    ST_LINK_STAT_CK = 0x004,
    ST_PARALLEL_FLT = 0x008,
    ST_ABILITY_DET  = 0x010,
    ST_ACK_DETECT   = 0x020,
    ST_COMPLETE_ACK = 0x040,
    ST_NP_WAIT      = 0x080,
    ST_AN_GOOD_CK   = 0x100,
    ST_AN_GOOD      = 0x200,
    ST_SERDES_WAIT  = 0x400

}MV_HWS_AP_SM_STATE;

/* AP Port Mode Definition */
typedef enum
{
     Port_1000Base_KX,
     Port_10GBase_KX4,
     Port_10GBase_R,
     Port_40GBase_R,
     Port_AP_LAST
}MV_HWA_AP_PORT_MODE;

/*******************************************************************************
*                         AP API Definition                                    *
* ============================================================================ *
*******************************************************************************/

/*******************************************************************************
* mvHwsApDetectStateSet
*
* DESCRIPTION: AP Detection process enable / disable set
*******************************************************************************/
void mvHwsApDetectStateSet(GT_U32 state);

/*******************************************************************************
* mvHwsApDetectRoutine
*
* DESCRIPTION: AP Detection process execution sequence
*******************************************************************************/
void mvHwsApDetectRoutine(void* pvParameters);


/* AP Detection External State machine States API's */
/* ================================================ */

/*******************************************************************************
* mvPortCtrlApDetectMsg
*
* DESCRIPTION: AP Detection message state execution
*******************************************************************************/
void mvPortCtrlApDetectMsg(void);

/*******************************************************************************
* mvPortCtrlApDetectActive
*
* DESCRIPTION: AP Detection active state execution
*******************************************************************************/
void mvPortCtrlApDetectActive(void);

/*******************************************************************************
* mvPortCtrlApDetectDelay
*
* DESCRIPTION: AP Detection delay state execution
*******************************************************************************/
void mvPortCtrlApDetectDelay(void);


/* AP Detection Port State machine States API's */
/* ============================================ */

/*******************************************************************************
* mvHwsInitialConfiguration
*
* DESCRIPTION: Set DMA in 10G/40G mode (instead of 1G mode)
*******************************************************************************/
GT_STATUS mvHwsInitialConfiguration(GT_U8 portIndex);

/*******************************************************************************
* mvApPortInit
*
* DESCRIPTION: The function exeucte AP Port detection Idle state
*******************************************************************************/
GT_STATUS mvApPortIdle(GT_U8 portIndex);

/*******************************************************************************
* mvApPortInit
*
* DESCRIPTION: The function exeucte AP Port detection Init state
*******************************************************************************/
GT_STATUS mvApPortInit(GT_U8 portIndex);

/*******************************************************************************
* mvApPortTxDisable
*
* DESCRIPTION: The function exeucte AP Port detection Tx Disable state
*******************************************************************************/
GT_STATUS mvApPortTxDisable(GT_U8 portIndex);

/*******************************************************************************
* mvApPortResolution
*
* DESCRIPTION: The function exeucte AP Port detection Resolution state
*******************************************************************************/
GT_STATUS mvApPortResolution(GT_U8 portIndex);

/*******************************************************************************
* mvApPortLinkUp
*
* DESCRIPTION: The function exeucte AP Port detection link check state
*******************************************************************************/
GT_STATUS mvApPortLinkUp(GT_U8 portIndex);

/*******************************************************************************
* mvApPortDeleteValidate
*
* DESCRIPTION: The function exeucte AP Port detete validation
*******************************************************************************/
GT_STATUS mvApPortDeleteValidate(GT_U8 portIndex);

/*******************************************************************************
* mvApPortDeleteMsg
*
* DESCRIPTION: The function exeucte AP Port detection delete sequence
*******************************************************************************/
GT_STATUS mvApPortDeleteMsg(GT_U8 portIndex);

/*******************************************************************************
* mvHwsApSetPortParameters
*
* DESCRIPTION: Update Port mode parameters for AP port
*******************************************************************************/
GT_STATUS mvHwsApSetPortParameters(GT_U32 portNum, MV_HWA_AP_PORT_MODE apPortMode);

/*******************************************************************************
* mvPortCtrlApPortMsgSend
*
* DESCRIPTION: The functions build and send IPC message to AP Port init queue
*******************************************************************************/
GT_STATUS mvPortCtrlApPortMsgSend(GT_U8 portIndex, GT_U8 msgType, GT_U8 queueId,
                                  GT_U8 portNum, GT_U8 portMode, GT_U8 action,
                                  GT_U8 refClk, GT_U8 refClkSrc);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __mvHwsPortCtrlAp_H */
