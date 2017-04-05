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
* mvHwsPortCtrlIpc.h
*
* DESCRIPTION:
*       Port Control Internal IPC Message Definitions
*
* FILE REVISION NUMBER:
*       $Revision: 1 $
*
*******************************************************************************/

#ifndef __mvHwsPortCtrlIpc_H
#define __mvHwsPortCtrlIpc_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#ifdef CONFIG_MV_IPC_FREERTOS_DRIVER
#define IPC_REMOTE_FREERTOS_NODE_ID   MV_IPC_NODE_ID_MASTER

#ifndef BOBK_DEV_SUPPORT

#define IPC_BASE_DOORBELL             12
#define IPC_DOORBELL_NUM(chnId)       (IPC_BASE_DOORBELL + chnId )

/* ARM Doorbell Registers Map */
#define CPU_SW_TRIG_IRQ               (MV_MBUS_REGS_OFFSET + 0xA04)
#define CPU_INT_SRC_CTRL( i )         (MV_MBUS_REGS_OFFSET + ICREG_SRC_CTRL(i))
#define CPU_DOORBELL_IN_REG           (MV_CPUIF_LOCAL_REGS_OFFSET + 0x78)
#define CPU_DOORBELL_IN_MASK_REG      (MV_CPUIF_LOCAL_REGS_OFFSET + 0x7C)
#define CPU_DOORBELL_OUT_REG          (MV_CPUIF_LOCAL_REGS_OFFSET + 0x70)
#define CPU_DOORBELL_OUT_MASK_REG     (MV_CPUIF_LOCAL_REGS_OFFSET + 0x74)
#define CPU_HOST_TO_ARM_DRBL_REG(cpu) (MV_CPUIF_REGS_BASE(cpu) + 0x78)
#define CPU_HOST_TO_ARM_MASK_REG(cpu) (MV_CPUIF_REGS_BASE(cpu) + 0x7C)
#define CPU_ARM_TO_HOST_DRBL_REG(cpu) (MV_CPUIF_REGS_BASE(cpu) + 0x70)
#define CPU_ARM_TO_HOST_MASK_REG(cpu) (MV_CPUIF_REGS_BASE(cpu) + 0x74)

#endif /* BOBK_DEV_SUPPORT */
#endif /* CONFIG_MV_IPC_FREERTOS_DRIVER */

#define MV_PORT_CTRL_NO_MSG_REPLY     (0xFF)

typedef struct 
{
    unsigned int link;
    unsigned int chn;
}mv_ipc_chn_info;

/*******************************************************************************
* mvPortCtrlIpcInit
*
* DESCRIPTION: Initialize Port control IPC infrastructure
*******************************************************************************/
GT_STATUS mvPortCtrlIpcInit(void);

/*******************************************************************************
* mvPortCtrlIpcActivate
*
* DESCRIPTION: Activate Port control IPC infrastructure
*******************************************************************************/
GT_STATUS mvPortCtrlIpcActivate(void);

/*******************************************************************************
* mvHwsIpcRequestGet
*
* DESCRIPTION: The API is called from Supervisour process to retrive message
*              from IPC driver
*******************************************************************************/
GT_STATUS mvHwsIpcRequestGet(GT_U32 priority, GT_U32 msgSize, char *msg);

/*******************************************************************************
* mvHwsIpcReplyMsg
*
* DESCRIPTION: The API is called from Port / General process to build and send
*              IPC message to the Host
*******************************************************************************/
GT_STATUS mvHwsIpcReplyMsg(GT_U32 queueId, MV_HWS_IPC_REPLY_MSG_STRUCT *txReplyMsg);

/*******************************************************************************
* mvHwsIpcRequestSet
*
* DESCRIPTION: IPC command simulation
*******************************************************************************/
GT_STATUS mvHwsIpcRequestSet(GT_U32 type, GT_U32 port, GT_U32 mode);

/*******************************************************************************
* mvPortCtrlProcessPendMsgNum
*
* DESCRIPTION: Receive number of messages pending in one of the system message
*              queues
*******************************************************************************/
GT_STATUS mvPortCtrlProcessPendMsgNum(MV_PORT_CTRL_MODULE module, GT_U32 *msgNum);

/*******************************************************************************
* mvPortCtrlProcessMsgRecv
*
* DESCRIPTION: Receive a message from on of the system message queues
*              duration defines the time to wait until the message is received
*              in case the queue is empty.
*              duration == 0, return immediately
*******************************************************************************/
GT_STATUS mvPortCtrlProcessMsgRecv(MV_PORT_CTRL_MODULE module,
                                   GT_U32 duration,
                                   MV_HWS_IPC_CTRL_MSG_STRUCT *msgPtr);

/*******************************************************************************
* mvPortCtrlProcessMsgSend
*
* DESCRIPTION: Send a message to a system message queues
*******************************************************************************/
GT_STATUS mvPortCtrlProcessMsgSend(MV_PORT_CTRL_MODULE module,
                                   MV_HWS_IPC_CTRL_MSG_STRUCT *msgPtr);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __mvHwsPortCtrlIpc_H */

