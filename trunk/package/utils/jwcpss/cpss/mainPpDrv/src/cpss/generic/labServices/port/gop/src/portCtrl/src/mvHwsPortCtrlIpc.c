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
*******************************************************************************
* mvHwsPortCtrlGeneral.c
*
* DESCRIPTION:
*       Port Control General State Machine
*
* FILE REVISION NUMBER:
*       $Revision: 1 $
*
*******************************************************************************/
#include <mvHwsPortCtrlInc.h>
#include <mvHwsPortCtrlIpc.h>
#include "common.h"
#include <mv_ipc.h>
#include <ic.h>

/* IPC channel enable table */
char ipc_enabled_chn[MV_IPC_CHN_NUM_MAX];
static mv_ipc_chn_info *chnl_info_array[MV_IPC_CHN_NUM_MAX] = {0};
MV_HWS_IPC_CTRL_MSG_STRUCT ipcMsg;

extern GT_U32 portSerdesNum[];

#ifndef CONFIG_MV_IPC_FREERTOS_DRIVER
#ifndef BOBK_DEV_SUPPORT
/*******************************************************************************
* host2servicecpu_isr
*
* DESCRIPTION: This interrupt handling only works when servicecpu is not working in ipc mode
*              It's trigged when host sends a doorbell irq to servicecpu
*              This interrupt handling only works for MSYS family boards
* INPUTS:
*       unsigned int linkId
*       void *dev_id
*
* OUTPUTS:
*       None
*
* RETURNS:
*       0  - on success
*       1  - on error
*
*******************************************************************************/
static long host2servicecpu_isr(void *data)
{
    MV_U32 reg;

    mv_os_printf("Interrupt from host CPU\n");

    MV_REG_BIT_RESET(CPU_DOORBELL_IN_MASK_REG, 1 << IPC_BASE_DOORBELL);
    MV_REG_WRITE(CPU_DOORBELL_IN_REG, ~(1 << IPC_BASE_DOORBELL));

    /* Generate a doorbell irq to host CPU */
    mv_os_printf("Generate a doorbell irq to host CPU back\n");
    MV_REG_WRITE(CPU_DOORBELL_OUT_REG, 1 << IPC_BASE_DOORBELL);

    MV_REG_BIT_SET(CPU_DOORBELL_IN_MASK_REG, (1));

    return 0;
}
#endif /* BOBK_DEV_SUPPORT */
#endif /* CONFIG_MV_IPC_FREERTOS_DRIVER */

/*******************************************************************************
* ipc_request_irq
*
* DESCRIPTION: This API register polling task handler
*              It is implemented via Port Control Supervisour task
*              The API is for compatibility with IPC link structure
*
* INPUTS:
*       unsigned int linkId
*       void *dev_id
*
* OUTPUTS:
*       None
*
* RETURNS:
*       0  - on success
*       1  - on error
*
*******************************************************************************/
static int ipc_request_irq(unsigned int link_id, void *dev_id)
{
    /* This API register polling task handler */
    /* It is implemented via Port Control Supervisour task */
    return 0;
}

/***********************************************************************************
* mvIpcSendTriggerPolling
*
* DESCRIPTION: Send an polling signal to target CPU
*
* INPUT:
*       linkId - link Id
*       chnId - channel ID
* OUTPUT:
*       None
* RETURN:
*       MV_OK or MV_ERROR
*
************************************************************************************/
MV_VOID mvIpcSendTriggerPolling(MV_U32 linkId, MV_U32 chnId)
{
    /* Do nothing for polling */
    return;
}

#ifndef BOBK_DEV_SUPPORT
/***********************************************************************************
* mvIpcSendPCIDrbl
*
* DESCRIPTION: Send an IPC doorbell to target CPU
*
* INPUT:
*       cpuId - the id of the target CPU
*       chnId - The channel ID
* OUTPUT:
*       None
* RETURN:
*       MV_OK or MV_ERROR
*
************************************************************************************/
MV_VOID mvIpcSendTriggerPCIDrbl(MV_U32 linkId, MV_U32 chnId)
{
    MV_U32 doorbellNum;

    /* now all ipc channels share one doorbell - IPC_BASE_DOORBELL*/
    /* so we do not use chnId now, but keep it as an input parameter*/
    doorbellNum  = IPC_BASE_DOORBELL;

    /* Use outbound doorbell IPC_BASE_DOORBELL(12)  for IPC */
    MV_REG_WRITE(CPU_DOORBELL_OUT_REG, 1 << doorbellNum);

    return;
}
#endif /* BOBK_DEV_SUPPORT */

/*******************************************************************************
* ipc_trigger_irq
*
* DESCRIPTION: Trigger Doorbell interrupt from MSYS to A385
*
* INPUTS:
*       unsigned int linkId
*       unsigned int chnId
*       void *dev_id
*
* OUTPUTS:
*       None
*
* RETURNS:
*       None
*
*******************************************************************************/
static void ipc_trigger_irq(unsigned int linkId, unsigned int chnId, void *dev_id)
{
#if defined BC2_DEV_SUPPORT

    mvIpcSendTriggerPCIDrbl(linkId, chnId);

#elif defined BOBK_DEV_SUPPORT

    cm3_to_msys_doorbell();

#endif
}

/*******************************************************************************
* ipc_enable_doorbell
*
* DESCRIPTION: This API enable channel RX execution
*
* INPUTS:
*       unsigned int linkId
*       unsigned int chnId
*       bool enable
*       void *dev_id
*
* OUTPUTS:
*       None
*
* RETURNS:
*       None
*
*******************************************************************************/
static void ipc_enable_doorbell(unsigned int linkId, unsigned int chnId,
                                bool enable, void *dev_id)
{
    /* now all ipc channels share one doorbell - IPC_BASE_DOORBELL, do not
    ** amsk/unmask it per-channel
    */
    if (enable == true)
    {
        ipc_enabled_chn[chnId] = 1;
    }
    else
    {
        ipc_enabled_chn[chnId] = 0;
    }
}

#ifndef BOBK_DEV_SUPPORT
/***********************************************************************************
* mvIpcPCIDrblInit
*
* DESCRIPTION: Init the structures
*
* INPUT:
*       None
* OUTPUT:
*       None
* RETURN:
*       None
*
************************************************************************************/
MV_VOID mvIpcPCIDrblInit(void)
{
    /* Forward Outbound doorbell to PCIe Host */
    /* now all ipc channels share one doorbell - IPC_BASE_DOORBELL*/
    MV_REG_WRITE(CPU_DOORBELL_OUT_MASK_REG, 1 << IPC_BASE_DOORBELL);

    MV_REG_BIT_SET(CPU_DOORBELL_IN_MASK_REG, 1 << IPC_BASE_DOORBELL);

    MV_REG_WRITE(CPU_INT_SRC_CTRL(IRQ_OUT_DOORBELL_SUM),
                 (1 << ICBIT_SRC_CTRL_EP_MASK)  |
                 (1 << ICBIT_SRC_CTRL_INT_EN)   |
                 (1 << ICBIT_SRC_CTRL_PRIORITY) |
                 (1 << ICBIT_SRC_CTRL_EP_SEL));

#ifndef CONFIG_MV_IPC_FREERTOS_DRIVER
    /* Register Doorbell interrupt handler */
    iICRegisterHandler(IRQ_IN_DOORBELL_LOW, host2servicecpu_isr, (void *)0,
                       IRQ_ENABLE, IRQ_PRIO_DEFAULT);
    iICRegisterHandler(IRQ_IN_DOORBELL_HIGH, host2servicecpu_isr, (void *)1,
                       IRQ_ENABLE, IRQ_PRIO_DEFAULT);
#endif /* CONFIG_MV_IPC_FREERTOS_DRIVER */

    return;
}
#endif /* BOBK_DEV_SUPPORT */

/*******************************************************************************
* mvPortCtrlIpcInit
*
* DESCRIPTION: Initialize Port control IPC infrastructure
*
* INPUTS:
*       None.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       0  - on success
*       1  - on error
*
*******************************************************************************/
GT_STATUS mvPortCtrlIpcInit(void)
{
    struct ipc_link_info link_info;

    link_info = (struct ipc_link_info){
        .num_of_chn = MV_IPC_CHN_NUM_MAX,
        .is_master = false,
        .remote_node_id = IPC_REMOTE_FREERTOS_NODE_ID,
        .shmem_size = SHMEM_SIZE,
        .shmem_virt_addr = (void *)SHMEM_BASE,
        .dev_id = NULL,
        .send_trigger = ipc_trigger_irq,
        .enable_chn_doorbell = ipc_enable_doorbell,
        .irq_init = ipc_request_irq
    };

#if defined BC2_DEV_SUPPORT

    mvIpcPCIDrblInit();
    ipc_shmem_link_setup(IPC_SCPU_FREERTOS_LINK_ID, link_info);

#elif defined BOBK_DEV_SUPPORT

    ipc_shmem_link_setup(IPC_CM3_FREERTOS_LINK_ID, link_info);

#endif

    return GT_OK;
}

/*******************************************************************************
* mvPortCtrlIpcActivate
*
* DESCRIPTION: Initialize Port control IPC infrastructure
*
* INPUTS:
*       None.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       0  - on success
*       1  - on error
*
*******************************************************************************/
GT_STATUS mvPortCtrlIpcActivate(void)
{
    int status;
    int attached;
    int chIdx;
    int chArr[] = 
    {
        MV_HWS_IPC_HI_PRI_RX_CH_ID,
        MV_HWS_IPC_LO_PRI_RX_CH_ID,
        MV_HWS_IPC_TX_0_CH_ID,
        MV_HWS_IPC_TX_1_CH_ID,
        MV_HWS_IPC_TX_2_CH_ID,
        MV_HWS_IPC_TX_3_CH_ID
    };

    mv_ipc_chn_info *chn_info;

    for (chIdx = 0; chIdx < MV_HWS_IPC_CH_NUM; chIdx++)
    {
        chn_info = mv_os_malloc(sizeof(mv_ipc_chn_info));
        memset(chn_info, 0, sizeof(mv_ipc_chn_info));
        
        chn_info->link = HWS_IPC_LINK_ID;
        chn_info->chn  = chIdx;

        /* Initialize IPC driver */
        status = ipc_open_chn(HWS_IPC_LINK_ID, chArr[chIdx], chn_info, NULL, NULL);
        if (status != 0)
        {
            mv_os_printf("IPC: Failed to open IPC channel %d", chArr[chIdx]);
            return GT_FAIL;
        }

        status = ipc_attach_chn(HWS_IPC_LINK_ID, chArr[chIdx],
                                ipc_get_remote_node_id(HWS_IPC_LINK_ID), &attached);
        if (status != 0)
        {
            mv_os_printf("IPC: Attach returned error for target CPU %d\n",
                         ipc_get_remote_node_id(HWS_IPC_LINK_ID));
            return GT_FAIL;
        }

        /* Enable channel for RX */
        ipc_enable_chn_rx(HWS_IPC_LINK_ID, chArr[chIdx]);

        chnl_info_array[chIdx] = chn_info;

        mv_os_printf("IPC: channel %d to cpu %d attached succesfully.\n",
                     chArr[chIdx], ipc_get_remote_node_id(HWS_IPC_LINK_ID));
    }

    return GT_OK;
}

/*******************************************************************************
* mvHwsIpcRequestGet
*
* DESCRIPTION: The API is called from Supervisour process to retrive message
*              from IPC driver
*
* INPUTS:
*       GT_U32 priority
*       char *msg
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       0  - on success
*       1  - on error
*
*******************************************************************************/
GT_STATUS mvHwsIpcRequestGet(GT_U32 priority, GT_U32 msgSize, char *msg)
{
    int channel;
     
    if (priority == MV_HWS_IPC_HIGH_PRI_QUEUE)
    {
        /* Extract message from high priority channel Rx */
        if (ipc_enabled_chn[MV_HWS_IPC_HI_PRI_RX_CH_ID])
        {
            if (ipc_rx_queue_process(HWS_IPC_LINK_ID, MV_HWS_IPC_HI_PRI_RX_CH_ID, msg) == 1)
            {
                return GT_OK;
            }
        }
    }
    else if (priority == MV_HWS_IPC_LOW_PRI_QUEUE)
    {
        /* Extract message from low priority channel Rx */
        if (ipc_enabled_chn[MV_HWS_IPC_LO_PRI_RX_CH_ID])
        {
            if (ipc_rx_queue_process(HWS_IPC_LINK_ID, MV_HWS_IPC_LO_PRI_RX_CH_ID, msg) == 1)
            {
                return GT_OK;
            }
        }

        /* Process IPC control message for Tx queues
        ** The IPC requires that control messages will be replied even in case the queue
        ** is used for Tx direction 
        ** These queues will be polled for control messages each time low prioirty queue is 
        ** processed and no message in received in the queue
        ** It ensure that low priority queue has higher priority than Tx queues 
        */
        for (channel = MV_HWS_IPC_TX_0_CH_ID; channel <= MV_HWS_IPC_TX_3_CH_ID; channel++)
        {
            if (ipc_enabled_chn[channel])
            {
                ipc_rx_queue_process(HWS_IPC_LINK_ID, channel, msg);
            }
        }
    }

    return GT_FAIL;
}

/*******************************************************************************
* mvHwsIpcReplyMsg
*
* DESCRIPTION:
*       Buils and Send reply from HW Services to the Host
*
* INPUTS:
*       queueId    - queue ID
*       txReplyMsg - pointer to message
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       0  - on success
*       1  - on error
*
*******************************************************************************/
GT_STATUS mvHwsIpcReplyMsg
(
    GT_U32                      queueId,
    MV_HWS_IPC_REPLY_MSG_STRUCT *txReplyMsg
)
{
    int status;

    if (queueId == MV_PORT_CTRL_NO_MSG_REPLY)
    {
        return GT_OK;
    }

    mvPortCtrlIpcLock();
    status = ipc_tx_queue_send(HWS_IPC_LINK_ID, queueId, (char*)txReplyMsg, sizeof(MV_HWS_IPC_REPLY_MSG_STRUCT));
    mvPortCtrlIpcUnlock();
    if (status == -1)
    {
        mv_os_printf("IPC: Failed to sent reply message via channel %d\n", queueId);
    }

    return GT_OK;
}

/*******************************************************************************
* mvHwsIpcRequestSet
*
* DESCRIPTION: IPC command simulation
*
* INPUTS:
*       GT_U32 type
*       GT_U32 port
*       GT_U32 mode
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       0  - on success
*       1  - on error
*
*******************************************************************************/
GT_STATUS mvHwsIpcRequestSet(GT_U32 type, GT_U32 port, GT_U32 mode)
{
    if (type == MV_HWS_IPC_PORT_INIT_MSG)
    {
        ipcMsg.devNum = 0;
        ipcMsg.ctrlMsgType = MV_HWS_IPC_PORT_INIT_MSG;
        ipcMsg.msgQueueId = MV_PORT_CTRL_NO_MSG_REPLY;
        ipcMsg.msgLength = sizeof(MV_HWS_IPC_PORT_INIT_DATA_STRUCT);
        ipcMsg.msgData.portInit.portGroup = 0;
        ipcMsg.msgData.portInit.phyPortNum = port;
        ipcMsg.msgData.portInit.portMode = mode;
        ipcMsg.msgData.portInit.lbPort = GT_FALSE;
        ipcMsg.msgData.portInit.refClock = MHz_156;
        ipcMsg.msgData.portInit.refClockSource = PRIMARY_LINE_SRC;

        mvPortCtrlProcessMsgSend(M2_PORT_MNG, &ipcMsg);
    }
    else if (type == MV_HWS_IPC_PORT_LINK_STATUS_GET_MSG)
    {
        ipcMsg.devNum = 0;
        ipcMsg.ctrlMsgType = MV_HWS_IPC_PORT_LINK_STATUS_GET_MSG;
        ipcMsg.msgQueueId = MV_PORT_CTRL_NO_MSG_REPLY;
        ipcMsg.msgLength = sizeof(MV_HWS_IPC_PORT_INFO_STRUCT);
        ipcMsg.msgData.portLinkStatus.portGroup = 0;
        ipcMsg.msgData.portLinkStatus.phyPortNum = port;
        ipcMsg.msgData.portLinkStatus.portMode = mode;

        mvPortCtrlProcessMsgSend(M6_GENERAL, &ipcMsg);
    }
    else if (type == MV_HWS_IPC_PORT_AUTO_TUNE_SET_EXT_MSG)
    {
        ipcMsg.devNum = 0;
        ipcMsg.ctrlMsgType = MV_HWS_IPC_PORT_AUTO_TUNE_SET_EXT_MSG;
        ipcMsg.msgQueueId = MV_PORT_CTRL_NO_MSG_REPLY;
        ipcMsg.msgLength = sizeof(MV_HWS_IPC_PORT_AUTO_TUNE_SET_EXT_DATA_STRUCT);
        ipcMsg.msgData.portAutoTuneSetExt.portGroup = 0;
        ipcMsg.msgData.portAutoTuneSetExt.phyPortNum = port;
        ipcMsg.msgData.portAutoTuneSetExt.portMode = (mode & 0xFFFF);
        ipcMsg.msgData.portAutoTuneSetExt.portTuningMode = ((mode >> 16) & 0xFFFF);

        mvPortCtrlProcessMsgSend(M6_GENERAL, &ipcMsg);
    }
    else if (type == MV_HWS_IPC_PORT_AP_ENABLE_MSG)
    {
        ipcMsg.devNum = 0;
        ipcMsg.ctrlMsgType = MV_HWS_IPC_PORT_AP_ENABLE_MSG;
        ipcMsg.msgQueueId = MV_PORT_CTRL_NO_MSG_REPLY;
        ipcMsg.msgLength = sizeof(MV_HWS_IPC_PORT_AP_DATA_STRUCT);
        ipcMsg.msgData.apConfig.portGroup = 0;
        ipcMsg.msgData.apConfig.phyPortNum = port;
        ipcMsg.msgData.apConfig.laneNum = portSerdesNum[port - MV_PORT_CTRL_AP_PORT_NUM_BASE];
        ipcMsg.msgData.apConfig.pcsNum = port;
        ipcMsg.msgData.apConfig.macNum = port;
        ipcMsg.msgData.apConfig.advMode = 0xE;
        ipcMsg.msgData.apConfig.options |= 0x200; /* [09:09] loopback Enable */

        mvPortCtrlProcessMsgSend(M5_AP_PORT_DET, &ipcMsg);
    }
    else if (type == MV_HWS_IPC_PORT_AP_DISABLE_MSG)
    {
        ipcMsg.devNum = 0;
        ipcMsg.ctrlMsgType = MV_HWS_IPC_PORT_AP_DISABLE_MSG;
        ipcMsg.msgQueueId = MV_PORT_CTRL_NO_MSG_REPLY;
        ipcMsg.msgLength = sizeof(MV_HWS_IPC_PORT_AP_DATA_STRUCT);
        ipcMsg.msgData.apConfig.portGroup = 0;
        ipcMsg.msgData.apConfig.phyPortNum = port;

        mvPortCtrlProcessMsgSend(M5_AP_PORT_DET, &ipcMsg);
    }

    return GT_OK;
}

