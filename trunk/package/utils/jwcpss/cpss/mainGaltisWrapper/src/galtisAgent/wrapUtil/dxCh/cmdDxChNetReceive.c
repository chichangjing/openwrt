/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* cmdDxChNetReceive.c
*
* DESCRIPTION:
*       galtis agent interface for packet capture using networkIf -- DXCH
*
* FILE REVISION NUMBER:
*       $Revision: 2 $
*******************************************************************************/

#include <cmdShell/common/cmdCommon.h>
#include <galtisAgent/wrapUtil/cmdCpssPresteraUtils.h>
#include <galtisAgent/wrapUtil/cmdCpssNetReceive.h>
#include <cpss/dxCh/dxChxGen/networkIf/cpssDxChNetIfTypes.h>

/******************************** Locals *************************************/

/*******************************************************************************
* rxInfoFree
*
* DESCRIPTION:
*       function to free the specific rx info format.
*
* INPUTS:
*      specificDeviceFormatPtr - (pointer to) the specific device Rx info format.
*                             format of DXCH / EXMX /DXSAL ...
*
* OUTPUTS:
*       None
*
* RETURNS:
*       None
*
* COMMENTS:
*
*
*******************************************************************************/
static void rxInfoFree
(
    IN void*  specificDeviceFormatPtr
)
{
    if(specificDeviceFormatPtr)
    {
        cmdOsFree(specificDeviceFormatPtr);
    }
}

/*******************************************************************************
* rxInfoCopy
*
* DESCRIPTION:
*       function to COPY the specific rx info format.
*
* INPUTS:
*      srcSpecificDeviceFormatPtr - (pointer to) the SOURCE specific device Rx
*                                   info format.
*                                   format of DXCH / EXMX /DXSAL ...
*
* OUTPUTS:
*      dstSpecificDeviceFormatPtr - (pointer to) the DESTINATION specific device Rx
*                                   info format.
*                                   format of DXCH / EXMX /DXSAL ...
*
*
* RETURNS:
*       None
*
* COMMENTS:
*
*
*******************************************************************************/
static void rxInfoCopy
(
    IN  const void*  srcSpecificDeviceFormatPtr,
    OUT void*        dstSpecificDeviceFormatPtr
)
{
    if(srcSpecificDeviceFormatPtr == NULL || dstSpecificDeviceFormatPtr == NULL)
    {
        return;
    }

    cmdOsMemCpy(dstSpecificDeviceFormatPtr,
             srcSpecificDeviceFormatPtr,
             sizeof(CPSS_DXCH_NET_RX_PARAMS_STC));

    return;
}

/*******************************************************************************
* cmdDxChRxPktReceive
*
* DESCRIPTION:
*       This function returns packets from PP destined to the CPU port.
*        -- SDMA relate.
*
* APPLICABLE DEVICES: ALL PCI/SDMA DXCH Devices
*
* INPUTS:
*       devNum      - Device number.
*       queueIdx    - The queue from which this packet was received.
*       numOfBuffPtr- Num of buffs in packetBuffs.
*       packetBuffs  - The received packet buffers list.
*       buffLen      - List of buffer lengths for packetBuffs.
*       rxParamsPtr  - (Pointer to)information parameters of received packets
*
* RETURNS:
*
*
* COMMENTS:
*
*
*******************************************************************************/
extern GT_STATUS prvCpssGenNetIfRx2Tx
(
    IN GT_U8  *buffList[],
    IN GT_U32 buffLenList[],
    IN GT_U32 numOfBufs
);

GT_STATUS cmdDxChRxPktReceive
(
    IN GT_U8                                devNum,
    IN GT_U8                                queueIdx,
    IN GT_U32                               numOfBuff,
    IN GT_U8                               *packetBuffs[],
    IN GT_U32                               buffLen[],
    IN CPSS_DXCH_NET_RX_PARAMS_STC         *rxParamsPtr
)
{
    GT_STATUS rc;
    CPSS_DXCH_NET_RX_PARAMS_STC *tmpRxParamPtr;

    /* If Rx to Tx disabled the function prvCpssGenNetIfRx2Tx returns
    without sending the packet. */
    prvCpssGenNetIfRx2Tx(packetBuffs,buffLen,numOfBuff);


    if(GT_FALSE == cmdCpssRxPktIsCaptureReady())
    {
        return GT_OK;
    }

    tmpRxParamPtr = cmdOsMalloc(sizeof(CPSS_DXCH_NET_RX_PARAMS_STC));
    if(tmpRxParamPtr == NULL)
    {
        return GT_OUT_OF_CPU_MEM;
    }

    cmdOsMemCpy(tmpRxParamPtr,
             rxParamsPtr,
             sizeof(CPSS_DXCH_NET_RX_PARAMS_STC));

    rc = cmdCpssRxPktReceive(devNum,queueIdx,tmpRxParamPtr,
                         &rxInfoFree,&rxInfoCopy,
                         numOfBuff,packetBuffs,buffLen);
    if(rc != GT_OK)
    {
        cmdOsFree(tmpRxParamPtr);
    }

    return rc;
}
