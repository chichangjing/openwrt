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
* mvHwsPortCtrlApLogDefs.h
*
* DESCRIPTION:
*       Port Control Log Definitions
*
* FILE REVISION NUMBER:
*       $Revision: 1 $
*
*******************************************************************************/

#ifndef __mvHwsPortCtrlApLogDefs_H
#define __mvHwsPortCtrlApLogDefs_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include <mvHwsPortCtrlApDefs.h>

#define LOG_ALL_PORT_DUMP (0xFF)
#define LOG_PARAM_UNUSED  (0x1F)
#define LOG_RESULT_MASK   (0x1)

#define LOG_MOD_MASK      (0x7)
#define LOG_MOD_SHIFT     (0)
#define LOG_MOD_SET(mod)  (mod & LOG_MOD_MASK)
#define LOG_MOD_GET(info) (info & LOG_MOD_MASK)

/*
** Reg Log Info
** +====================================================+===========+============+=======+========+
** + 31:16                                              + 15:11     + 10:06      + 05:03 + 02:00  +
** +====================================================+===========+============+=======+========+
** + Reg Value                                          + Reg Index + Port Index + State + Module +
** +====================================================+===========+============+=======+========+
*/
#define REG_LOG_STATE_MASK         (0x7)
#define REG_LOG_STATE_SHIFT        (3)
#define REG_LOG_STATE_SET(state)   (state << REG_LOG_STATE_SHIFT)
#define REG_LOG_STATE_GET(info)   ((info  >> REG_LOG_STATE_SHIFT) & REG_LOG_STATE_MASK)

#define REG_LOG_PORT_MASK          (0x1F)
#define REG_LOG_PORT_SHIFT         (6)
#define REG_LOG_PORT_SET(port)     (port << REG_LOG_PORT_SHIFT)
#define REG_LOG_PORT_GET(info)    ((info >> REG_LOG_PORT_SHIFT) & REG_LOG_PORT_MASK)

#define REG_LOG_IDX_MASK           (0x1F)
#define REG_LOG_IDX_SHIFT          (11)
#define REG_LOG_IDX_SET(reg)       (reg  << REG_LOG_IDX_SHIFT)
#define REG_LOG_IDX_GET(info)     ((info >> REG_LOG_IDX_SHIFT) & REG_LOG_IDX_MASK)

#define REG_LOG_VAL_MASK           (0xFFFF)
#define REG_LOG_VAL_SHIFT          (16)
#define REG_LOG_VAL_SET(val)       (val  << REG_LOG_VAL_SHIFT)
#define REG_LOG_VAL_GET(info)     ((info >> REG_LOG_VAL_SHIFT) & REG_LOG_VAL_MASK)

#define REG_LOG(state, port, reg, val) \
    (M0_DEBUG | \
     REG_LOG_STATE_SET(state) | \
     REG_LOG_PORT_SET(port)   | \
     REG_LOG_IDX_SET(reg)     | \
     REG_LOG_VAL_SET(val))

/*
** Supervisor Log Info
** +======================================================+======+=======+=======+=======+========+
** + 31:20                                                +  19  + 18:12 + 11:06 + 05:03 + 02:00  +
** +======================================================+======+=======+=======+=======+========+
** + Reserved                                             + Type + Port  + Msg   + State + Module +
** +======================================================+======+=======+=======+=======+========+
*/
#define SPV_LOG_STATE_MASK         (0x7)
#define SPV_LOG_STATE_SHIFT        (3)
#define SPV_LOG_STATE_SET(state)   (state << SPV_LOG_STATE_SHIFT)
#define SPV_LOG_STATE_GET(info)   ((info  >> SPV_LOG_STATE_SHIFT) & SPV_LOG_STATE_MASK)

#define SPV_LOG_MSG_MASK           (0x3F)
#define SPV_LOG_MSG_SHIFT          (6)
#define SPV_LOG_MSG_SET(msg)       (msg  << SPV_LOG_MSG_SHIFT)
#define SPV_LOG_MSG_GET(info)     ((info >> SPV_LOG_MSG_SHIFT) & SPV_LOG_MSG_MASK)

#define SPV_LOG_PORT_MASK          (0x7F)
#define SPV_LOG_PORT_SHIFT         (12)
#define SPV_LOG_PORT_SET(port)     (port << SPV_LOG_PORT_SHIFT)
#define SPV_LOG_PORT_GET(info)    ((info >> SPV_LOG_PORT_SHIFT) & SPV_LOG_PORT_MASK)

#define SPV_LOG_TYPE_MASK          (0x1)
#define SPV_LOG_TYPE_SHIFT         (19)
#define SPV_LOG_TYPE_SET(type)     (type << SPV_LOG_TYPE_SHIFT)
#define SPV_LOG_TYPE_GET(info)    ((info >> SPV_LOG_TYPE_SHIFT) & SPV_LOG_TYPE_MASK)

#define SPV_LOG(state, msg, port, type) \
    (M1_SUPERVISOR | \
     SPV_LOG_STATE_SET(state) | \
     SPV_LOG_MSG_SET(msg)     | \
     SPV_LOG_PORT_SET(port)   | \
     SPV_LOG_TYPE_SET(type))

/*
** Port Management Log Info
** +=============================================================+=======+=======+=======+========+
** + 31:17                                                       + 16:12 + 11:05 + 04:03 + 02:00  +
** +=============================================================+=======+=======+=======+========+
** + Reserved                                                    + Msg   + Port  + State + Module +
** +=============================================================+=======+=======+=======+========+
*/
#define PORT_MNG_LOG_STATE_MASK        (0x3)
#define PORT_MNG_LOG_STATE_SHIFT       (3)
#define PORT_MNG_LOG_STATE_SET(state)  (state << PORT_MNG_LOG_STATE_SHIFT)
#define PORT_MNG_LOG_STATE_GET(info)  ((info  >> PORT_MNG_LOG_STATE_SHIFT) & PORT_MNG_LOG_STATE_MASK)

#define PORT_MNG_LOG_PORT_MASK         (0x7F)
#define PORT_MNG_LOG_PORT_SHIFT        (5)
#define PORT_MNG_LOG_PORT_SET(port)    (port  << PORT_MNG_LOG_PORT_SHIFT)
#define PORT_MNG_LOG_PORT_GET(info)   ((info  >> PORT_MNG_LOG_PORT_SHIFT) & PORT_MNG_LOG_PORT_MASK)

#define PORT_MNG_MSG_MASK              (0x1F)
#define PORT_MNG_MSG_SHIFT             (12)
#define PORT_MNG_MSG_SET(msg)          (msg  << PORT_MNG_MSG_SHIFT)
#define PORT_MNG_MSG_GET(info)        ((info >> PORT_MNG_MSG_SHIFT) & PORT_MNG_MSG_MASK)

#define PORT_MNG_LOG(state, port, msg) \
    (M2_PORT_MNG | \
     PORT_MNG_LOG_STATE_SET(state) | \
     PORT_MNG_LOG_PORT_SET(port)   | \
     PORT_MNG_MSG_SET(msg))

/*
** Port State Machine Log Info
** +============================================================+=======+========+=======+========+
** + 31:18                                                      + 17:11 + 10:06  + 05:03 + 02:00  +
** +============================================================+=======+========+=======+========+
** + Reserved                                                   + Port  + Status + State + Module +
** +============================================================+=======+========+=======+========+
*/
#define PORT_SM_LOG_STATE_MASK          (0x7)
#define PORT_SM_LOG_STATE_SHIFT         (3)
#define PORT_SM_LOG_STATE_SET(state)    (state << PORT_SM_LOG_STATE_SHIFT)
#define PORT_SM_LOG_STATE_GET(info)    ((info  >> PORT_SM_LOG_STATE_SHIFT) & PORT_SM_LOG_STATE_MASK)

#define PORT_SM_LOG_STATUS_MASK         (0x1F)
#define PORT_SM_LOG_STATUS_SHIFT        (6)
#define PORT_SM_LOG_STATUS_SET(status)  (status   << PORT_SM_LOG_STATUS_SHIFT)
#define PORT_SM_LOG_STATUS_GET(info)   ((info  >> PORT_SM_LOG_STATUS_SHIFT) & PORT_SM_LOG_STATUS_MASK)

#define PORT_SM_LOG_PORT_MASK           (0x7F)
#define PORT_SM_LOG_PORT_SHIFT          (11)
#define PORT_SM_LOG_PORT_SET(port)      (port  << PORT_SM_LOG_PORT_SHIFT)
#define PORT_SM_LOG_PORT_GET(info)     ((info  >> PORT_SM_LOG_PORT_SHIFT) & PORT_SM_LOG_PORT_MASK)

#define PORT_SM_LOG(state, status, port) \
    (M3_PORT_SM | \
     PORT_SM_LOG_STATE_SET(state)   | \
     PORT_SM_LOG_STATUS_SET(status) | \
     PORT_SM_LOG_PORT_SET(port))

/*
** AP Port Management Log Info
** +============================================================+=======+========+=======+========+
** + 31:18                                                      + 17:11 + 10:06  + 05:03 + 02:00  +
** +============================================================+=======+========+=======+========+
** + Reserved                                                   + Port  + Status + State + Module +
** +============================================================+=======+========+=======+========+
*/
#define AP_PORT_MNG_LOG_STATE_MASK          (0x7)
#define AP_PORT_MNG_LOG_STATE_SHIFT         (3)
#define AP_PORT_MNG_LOG_STATE_SET(state)    (state  << AP_PORT_MNG_LOG_STATE_SHIFT)
#define AP_PORT_MNG_LOG_STATE_GET(info)    ((info   >> AP_PORT_MNG_LOG_STATE_SHIFT) & AP_PORT_MNG_LOG_STATE_MASK)

#define AP_PORT_MNG_LOG_STATUS_MASK         (0x1F)
#define AP_PORT_MNG_LOG_STATUS_SHIFT        (6)
#define AP_PORT_MNG_LOG_STATUS_SET(status)  (status << AP_PORT_MNG_LOG_STATUS_SHIFT)
#define AP_PORT_MNG_LOG_STATUS_GET(info)   ((info   >> AP_PORT_MNG_LOG_STATUS_SHIFT) & AP_PORT_MNG_LOG_STATUS_MASK)

#define AP_PORT_MNG_LOG_PORT_MASK           (0x7F)
#define AP_PORT_MNG_LOG_PORT_SHIFT          (11)
#define AP_PORT_MNG_LOG_PORT_SET(port)      (port   << AP_PORT_MNG_LOG_PORT_SHIFT)
#define AP_PORT_MNG_LOG_PORT_GET(info)     ((info   >> AP_PORT_MNG_LOG_PORT_SHIFT) & AP_PORT_MNG_LOG_PORT_MASK)

#define AP_PORT_MNG_LOG(state, status, port) \
    (M4_AP_PORT_MNG | \
     AP_PORT_MNG_LOG_STATE_SET(state)   | \
     AP_PORT_MNG_LOG_STATUS_SET(status) | \
     AP_PORT_MNG_LOG_PORT_SET(port))

/*
** AP Port State Machine Log Info
** +=================================================+==========+=======+========+=======+========+
** + 31:29                                           + 28:18    + 17:11 + 10:06  + 05:03 + 02:00  +
** +=================================================+==========+=======+========+=======+========+
** + Reserved                                        + AP HW SM + Port  + Status + State + Module +
** +=================================================+==========+=======+========+=======+========+
*/
#define AP_PORT_DET_LOG_STATE_MASK             (0x7)
#define AP_PORT_DET_LOG_STATE_SHIFT            (3)
#define AP_PORT_DET_LOG_STATE_SET(state)       (state    << AP_PORT_DET_LOG_STATE_SHIFT)
#define AP_PORT_DET_LOG_STATE_GET(info)       ((info     >> AP_PORT_DET_LOG_STATE_SHIFT) & AP_PORT_DET_LOG_STATE_MASK)

#define AP_PORT_DET_LOG_STATUS_MASK            (0x1F)
#define AP_PORT_DET_LOG_STATUS_SHIFT           (6)
#define AP_PORT_DET_LOG_STATUS_SET(status)     (status   << AP_PORT_DET_LOG_STATUS_SHIFT)
#define AP_PORT_DET_LOG_STATUS_GET(info)      ((info     >> AP_PORT_DET_LOG_STATUS_SHIFT) & AP_PORT_DET_LOG_STATUS_MASK)

#define AP_PORT_DET_LOG_PORT_MASK              (0x7F)
#define AP_PORT_DET_LOG_PORT_SHIFT             (11)
#define AP_PORT_DET_LOG_PORT_SET(port)         (port     << AP_PORT_DET_LOG_PORT_SHIFT)
#define AP_PORT_DET_LOG_PORT_GET(info)        ((info     >> AP_PORT_DET_LOG_PORT_SHIFT) & AP_PORT_DET_LOG_PORT_MASK)

#define AP_PORT_DET_LOG_HW_SM_MASK             (0x7FF)
#define AP_PORT_DET_LOG_HW_SM_SHIFT            (18)
#define AP_PORT_DET_LOG_HW_SM_SET(apHw)        (apHw    << AP_PORT_DET_LOG_HW_SM_SHIFT)
#define AP_PORT_DET_LOG_HW_SM_GET(info)       ((info    >> AP_PORT_DET_LOG_HW_SM_SHIFT) & AP_PORT_DET_LOG_HW_SM_MASK)

#define AP_PORT_SM_LOG(state, status, port, apHw) \
    (M5_AP_PORT_DET | \
     AP_PORT_DET_LOG_STATE_SET(state)   | \
     AP_PORT_DET_LOG_STATUS_SET(status) | \
     AP_PORT_DET_LOG_PORT_SET(port)     | \
     AP_PORT_DET_LOG_HW_SM_SET(apHw))

/*
** AP Port State Machine Extended Log Info - Defined in DB as 32bit HCD Status
** +==========+=======+==========+===========+=======+=======+=========+==========+=======+========+
** + 31       + 30:24 + 16:23    + 08:15     + 07    + 06    + 05      + 04       + 03    + 02:00  +
** +==========+=======+==========+===========+=======+=======+=========+==========+=======+========+
** + HCD Link + Port  + Reserved + Local HCD + FC TX + FC RX + FEC Res + ARBSmErr + Found + Module +
** +==========+=======+======|===+===========+=======+=======+=========+==========+=======+========+
**                           |
**                           | [16:16] HCD Interrupt Trigger
**                           | [17:17] Link Interrupt Trigger
*/
#define AP_PORT_DET_EXT_LOG_FOUND_MASK          (0x1)
#define AP_PORT_DET_EXT_LOG_FOUND_SHIFT         (3)
#define AP_PORT_DET_EXT_LOG_FOUND_GET(info)    ((info >> AP_PORT_DET_EXT_LOG_FOUND_SHIFT) & AP_PORT_DET_EXT_LOG_FOUND_MASK)

#define AP_PORT_DET_EXT_LOG_ARB_ERR_MASK        (0x1)
#define AP_PORT_DET_EXT_LOG_ARB_ERR_SHIFT       (4)
#define AP_PORT_DET_EXT_LOG_ARB_ERR_GET(info)  ((info >> AP_PORT_DET_EXT_LOG_ARB_ERR_SHIFT) & AP_PORT_DET_EXT_LOG_ARB_ERR_MASK)

#define AP_PORT_DET_EXT_LOG_FEC_RES_MASK        (0x1)
#define AP_PORT_DET_EXT_LOG_FEC_RES_SHIFT       (5)
#define AP_PORT_DET_EXT_LOG_FEC_RES_GET(info)  ((info >> AP_PORT_DET_EXT_LOG_FEC_RES_SHIFT) & AP_PORT_DET_EXT_LOG_FEC_RES_MASK)

#define AP_PORT_DET_EXT_LOG_RX_FC_MASK          (0x1)
#define AP_PORT_DET_EXT_LOG_RX_FC_SHIFT         (6)
#define AP_PORT_DET_EXT_LOG_RX_FC_GET(info)    ((info >> AP_PORT_DET_EXT_LOG_RX_FC_SHIFT) & AP_PORT_DET_EXT_LOG_RX_FC_MASK)

#define AP_PORT_DET_EXT_LOG_TX_FC_MASK          (0x1)
#define AP_PORT_DET_EXT_LOG_TX_FC_SHIFT         (7)
#define AP_PORT_DET_EXT_LOG_TX_FC_GET(info)    ((info >> AP_PORT_DET_EXT_LOG_TX_FC_SHIFT) & AP_PORT_DET_EXT_LOG_TX_FC_MASK)

#define AP_PORT_DET_EXT_LOG_HCD_MASK            (0xFF)
#define AP_PORT_DET_EXT_LOG_HCD_SHIFT           (8)
#define AP_PORT_DET_EXT_LOG_HCD_GET(info)      ((info >> AP_PORT_DET_EXT_LOG_HCD_SHIFT) & AP_PORT_DET_EXT_LOG_HCD_MASK)

#define AP_PORT_DET_EXT_LOG_HCD_INT_MASK        (0x1)
#define AP_PORT_DET_EXT_LOG_HCD_INT_SHIFT       (16)
#define AP_PORT_DET_EXT_LOG_HCD_INT_GET(info)  ((info >> AP_PORT_DET_EXT_LOG_HCD_INT_SHIFT) & AP_PORT_DET_EXT_LOG_HCD_INT_MASK)

#define AP_PORT_DET_EXT_LOG_LINK_INT_MASK       (0x1)
#define AP_PORT_DET_EXT_LOG_LINK_INT_SHIFT      (17)
#define AP_PORT_DET_EXT_LOG_LINK_INT_GET(info) ((info >> AP_PORT_DET_EXT_LOG_LINK_INT_SHIFT) & AP_PORT_DET_EXT_LOG_LINK_INT_MASK)

#define AP_PORT_DET_EXT_LOG_PORT_MASK           (0x7F)
#define AP_PORT_DET_EXT_LOG_PORT_SHIFT          (24)
#define AP_PORT_DET_EXT_LOG_PORT_SET(port)      (port << AP_PORT_DET_EXT_LOG_PORT_SHIFT)
#define AP_PORT_DET_EXT_LOG_PORT_GET(info)     ((info >> AP_PORT_DET_EXT_LOG_PORT_SHIFT) & AP_PORT_DET_EXT_LOG_PORT_MASK)

#define AP_PORT_DET_EXT_LOG_LINK_MASK           (0x1)
#define AP_PORT_DET_EXT_LOG_LINK_SHIFT          (31)
#define AP_PORT_DET_EXT_LOG_LINK_GET(info)     ((info >> AP_PORT_DET_EXT_LOG_LINK_SHIFT) & AP_PORT_DET_EXT_LOG_LINK_MASK)

#define AP_PORT_DET_LOG_EXT(hcdStatus, port) \
    (M5_AP_PORT_DET_EXT | \
     hcdStatus | \
     AP_PORT_DET_EXT_LOG_PORT_SET(port))

/*
** Port General Log Info
** +===========================================+========+========+=======+=======+=======+========+
** + 31:27                                     + 26:19  +   18   + 17:11 + 10:05 + 04:03 + 02:00  +
** +===========================================+========+========+=======+=======+=======+========+
** + Reserved                                  + Detail + Result + Port  + Msg   + State + Module +
** +===========================================+========+========+=======+=======+=======+========+
*/
#define PORT_GENERAL_LOG_STATE_MASK         (0x3)
#define PORT_GENERAL_LOG_STATE_SHIFT        (3)
#define PORT_GENERAL_LOG_STATE_SET(state)   (state << PORT_GENERAL_LOG_STATE_SHIFT)
#define PORT_GENERAL_LOG_STATE_GET(info)   ((info  >> PORT_GENERAL_LOG_STATE_SHIFT) & PORT_GENERAL_LOG_STATE_MASK)

#define PORT_GENERAL_LOG_MSG_MASK           (0x3F)
#define PORT_GENERAL_LOG_MSG_SHIFT          (5)
#define PORT_GENERAL_LOG_MSG_SET(msg)       (msg   << PORT_GENERAL_LOG_MSG_SHIFT)
#define PORT_GENERAL_LOG_MSG_GET(info)     ((info  >> PORT_GENERAL_LOG_MSG_SHIFT) & PORT_GENERAL_LOG_MSG_MASK)

#define PORT_GENERAL_LOG_PORT_MASK          (0x7F)
#define PORT_GENERAL_LOG_PORT_SHIFT         (11)
#define PORT_GENERAL_LOG_PORT_SET(port)     (port  << PORT_GENERAL_LOG_PORT_SHIFT)
#define PORT_GENERAL_LOG_PORT_GET(info)    ((info  >> PORT_GENERAL_LOG_PORT_SHIFT) & PORT_GENERAL_LOG_PORT_MASK)

#define PORT_GENERAL_LOG_RES_MASK           (0x1)
#define PORT_GENERAL_LOG_RES_SHIFT          (18)
#define PORT_GENERAL_LOG_RES_SET(result)    (result << PORT_GENERAL_LOG_RES_SHIFT)
#define PORT_GENERAL_LOG_RES_GET(info)     ((info   >> PORT_GENERAL_LOG_RES_SHIFT) & PORT_GENERAL_LOG_RES_MASK)

#define PORT_GENERAL_LOG_DETAIL_MASK        (0xFF)
#define PORT_GENERAL_LOG_DETAIL_SHIFT       (19)
#define PORT_GENERAL_LOG_DETAIL_SET(detail) (detail << PORT_GENERAL_LOG_DETAIL_SHIFT)
#define PORT_GENERAL_LOG_DETAIL_GET(info)  ((info   >> PORT_GENERAL_LOG_DETAIL_SHIFT) & PORT_GENERAL_LOG_DETAIL_MASK)

#define PORT_GENERAL_LOG(state, msg, port) \
    (M6_GENERAL | \
     PORT_GENERAL_LOG_STATE_SET(state) | \
     PORT_GENERAL_LOG_MSG_SET(msg)     | \
     PORT_GENERAL_LOG_PORT_SET(port))

#define PORT_GENERAL_LOG_RES(state, msg, port, result, detail) \
    (M6_GENERAL | \
     PORT_GENERAL_LOG_STATE_SET(state) | \
     PORT_GENERAL_LOG_MSG_SET(msg)     | \
     PORT_GENERAL_LOG_PORT_SET(port)   | \
     PORT_GENERAL_LOG_RES_SET(result)  | \
     PORT_GENERAL_LOG_DETAIL_SET(detail))

/* LOG Entry Structure Definition */
typedef struct
{
    unsigned long timestamp;
    unsigned long info;
}MV_HWS_PORT_CTRL_LOG_ENTRY;

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __mvHwsPortCtrlApLogDefs_H */
