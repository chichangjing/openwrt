/*******************************************************************************
*              (c), Copyright 2015, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* cpssGenericSrvCpuIpc.h
*
* DESCRIPTION:
*       Service CPU IPC APIs
*
* FILE REVISION NUMBER:
*       $Revision: 1 $
*
*******************************************************************************/

#include <cpss/generic/srvCpu/prvCpssGenericSrvCpuIpc.h>
#include <cpssDriver/pp/prvCpssDrvPpDefs.h>
#include <cpssDriver/pp/hardware/cpssDriverPpHw.h>
#include "prvCpssGenericSrvCpuIpcDevCfg.h"

/*******************************************************************************
* prvCpssGenericSrvCpuIpcInit
*
* DESCRIPTION:
*       Configure IPC, open and attach all channels
*
* APPLICABLE DEVICES:
*       Bobcat2
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2; Lion2;
*       Puma2; Puma3; ExMx; Caelum; Bobcat3.
*
* INPUTS:
*       devNum                   - device number.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_FAIL                  - on error.
*       GT_BAD_PARAM             - wrong devNum or flowId.
*       GT_BAD_PTR               - one of the parameters is NULL pointer.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device.
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvCpssGenericSrvCpuIpcInit
(
    IN GT_U8    devNum
)
{
    int rc, chn_id;
    PRV_CPSS_IPC_DEV_CFG_STC *ipc;

    if (devNum >= PRV_CPSS_MAX_PP_DEVICES_CNS)
        return GT_BAD_PARAM;
    ipc = prvIpcDevCfg[devNum];
    if (ipc == NULL)
        return GT_NOT_INITIALIZED;
    if (ipc->linkConfigured == GT_TRUE)
        return GT_ALREADY_EXIST;

    for (chn_id = 0; (unsigned)chn_id < ipc->link_cfg.num_of_chn; chn_id++)
    {
        int attached = 0;
        cpssOsPrintf("Configuring channel %d...\n", chn_id);
        rc = ipc_open_chn(ipc->link_id, chn_id,
                NULL /*dev*/, NULL /*rx_callback*/, NULL/*set_chn_status*/);
        if (rc != 0)
        {
            cpssOsPrintf("ipc_open_chn(%d, %d, ...) failed rc=%d\n",
                        ipc->link_id, chn_id, rc);
            continue;
        }
        rc = ipc_attach_chn(ipc->link_id, chn_id, ipc->link_cfg.remote_node_id, &attached);
        if (rc != 0)
        {
            cpssOsPrintf("ipc_attach_chn(%d, %d, %d, ...)=%d\n",
                        ipc->link_id, chn_id, ipc->link_cfg.remote_node_id, rc);
            continue;

        }
        if (attached)
            cpssOsPrintf("Dev %d link %d chn %d attached to %d\n",
                        devNum, ipc->link_id, chn_id, ipc->link_cfg.remote_node_id);
#if 0
        /* Unmasks the given channel in ISR */
        ipc_enable_chn_rx(ipc->link_id, chn_id);
#else
        if (ipc->link_id == IPC_SCPU_FREERTOS_LINK_ID)
        {
            /* Reset the pcidrbl corresponding to channel */
            GT_U32 reg;
            rc = cpssDrvPpHwInternalPciRegRead(
                    devNum, CPSS_PORT_GROUP_UNAWARE_MODE_CNS,
                    0x21874, /* IPC_PCI_OUTDRBL_CPU0_MASK_REG */
                    &reg);
            if (rc != GT_OK)
            {
                cpssOsPrintf("Failed to read IPC_PCI_OUTDRBL_CPU0_MASK_REG\n");
                continue;
            }
            reg &= (GT_U32)(~(1 << (12/*IPC_PCI_DRBL_BASE*/ + chn_id)));
            rc = cpssDrvPpHwInternalPciRegWrite(
                    devNum, CPSS_PORT_GROUP_UNAWARE_MODE_CNS,
                    0x21874, /* IPC_PCI_OUTDRBL_CPU0_MASK_REG */
                    reg);
            cpssOsDelay(10);
        }
#endif
    }
    ipc->linkConfigured = GT_TRUE;

    return GT_OK;

}

/*******************************************************************************
* prvCpssGenericSrvCpuIpcMessageSend
*
* DESCRIPTION:
*       This API allows the application to send a message to other side,
*       using a buffer it was previously fetched from the IPC (by using the
*       Buffer-Get API)
*       It may choose to send asynchrony or synchronic and in case
*       synchronic how much tome to wait for processing.
*
* APPLICABLE DEVICES:
*       Bobcat2
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2; Lion2;
*       Puma2; Puma3; ExMx; Caelum; Bobcat3.
*
* INPUTS:
*       devNum                   - device number.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_FAIL                  - on error.
*       GT_BAD_PARAM             - wrong devNum or flowId.
*       GT_BAD_PTR               - one of the parameters is NULL pointer.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device.
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvCpssGenericSrvCpuIpcMessageSend
(
    IN  GT_U8    devNum,
    IN  GT_U32   channel,
    IN  GT_U8   *msgData,
    IN  GT_U32   msgDataSize
)
{
    PRV_CPSS_IPC_DEV_CFG_STC *ipc;

    if (devNum >= PRV_CPSS_MAX_PP_DEVICES_CNS)
        return GT_BAD_PARAM;
    ipc = prvIpcDevCfg[devNum];
    if (ipc == NULL)
        return GT_NOT_INITIALIZED;

    if (ipc_tx_ready(ipc->link_id, channel) != mv_true)
    {
        return GT_FAIL;
    }
    /* check size */
    if (ipc_tx_queue_send(ipc->link_id, channel, (char*)msgData, msgDataSize) != 0)
        return GT_FAIL;
    return GT_OK;
}

/*******************************************************************************
* prvCpssGenericSrvCpuIpcMessageRecv
*
* DESCRIPTION:
*       This API allows the application to fetch message sent by the other
*       side. Application may choose to wait for incoming message or not
*       (if application wish to use polling it can loop with no waiting).
*
* APPLICABLE DEVICES:
*       Bobcat2
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2; Lion2;
*       Puma2; Puma3; ExMx; Caelum; Bobcat3.
*
* INPUTS:
*       devNum                   - device number.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_FAIL                  - on error.
*       GT_BAD_PARAM             - wrong devNum or flowId.
*       GT_BAD_PTR               - one of the parameters is NULL pointer.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device.
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvCpssGenericSrvCpuIpcMessageRecv
(
    IN  GT_U8    devNum,
    IN  GT_U32   channel,
    OUT GT_U8   *msgData,
    OUT GT_U32  *msgDataSize
)
{
    PRV_CPSS_IPC_DEV_CFG_STC *ipc;
    char    *rxbuf = (char*)msgData;
    int     rc;

    if (devNum >= PRV_CPSS_MAX_PP_DEVICES_CNS)
        return GT_BAD_PARAM;
    ipc = prvIpcDevCfg[devNum];
    if (ipc == NULL)
        return GT_NOT_INITIALIZED;

    rc = ipc_rx_msg(ipc->link_id, channel, rxbuf);
    if (msgDataSize != NULL)
        *msgDataSize = ipc->link_cfg.chn_rx_buffer_info[channel];

    if (rc == 0)
        return GT_OK;
    if (rc == MV_NO_MORE)
        return GT_NO_MORE;

    return GT_FAIL;
}



/*******************************************************************************
* prvCpssGenericSrvCpuWrite
*
* DESCRIPTION:
*       Write to Service CPU "firmware section"
*       TODO: remove, design bug
*
* APPLICABLE DEVICES:
*       Bobcat2
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2; Lion2;
*       Puma2; Puma3; ExMx; Caelum; Bobcat3.
*
* INPUTS:
*       devNum                   - device number.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_FAIL                  - on error.
*       GT_BAD_PARAM             - wrong devNum or flowId.
*       GT_BAD_PTR               - one of the parameters is NULL pointer.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device.
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvCpssGenericSrvCpuWrite
(
    IN  GT_U8    devNum,
    IN  GT_U32   offset,
    IN  void    *data,
    IN  GT_U32   size
)
{
    PRV_CPSS_IPC_DEV_CFG_STC *ipc;

    if (devNum >= PRV_CPSS_MAX_PP_DEVICES_CNS)
        return GT_BAD_PARAM;
    ipc = prvIpcDevCfg[devNum];
    if (ipc == NULL)
        return GT_NOT_INITIALIZED;
    cpssOsMemCpy((void*)(((GT_UINTPTR)ipc->fwAddress) + offset), data, size);
    return GT_OK;
}

/*******************************************************************************
* prvCpssGenericSrvCpuRead
*
* DESCRIPTION:
*       Read from Service CPU "firmware section"
*       TODO: remove, design bug
*
* APPLICABLE DEVICES:
*       Bobcat2
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2; Lion2;
*       Puma2; Puma3; ExMx; Caelum; Bobcat3.
*
* INPUTS:
*       devNum                   - device number.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_FAIL                  - on error.
*       GT_BAD_PARAM             - wrong devNum or flowId.
*       GT_BAD_PTR               - one of the parameters is NULL pointer.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device.
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvCpssGenericSrvCpuRead
(
    IN  GT_U8    devNum,
    IN  GT_U32   offset,
    OUT void    *data,
    IN  GT_U32   size
)
{
    PRV_CPSS_IPC_DEV_CFG_STC *ipc;

    if (devNum >= PRV_CPSS_MAX_PP_DEVICES_CNS)
        return GT_BAD_PARAM;
    ipc = prvIpcDevCfg[devNum];
    if (ipc == NULL)
        return GT_NOT_INITIALIZED;
    cpssOsMemCpy(data, (void*)(((GT_UINTPTR)ipc->fwAddress) + offset), size);
    return GT_OK;
}
