/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* wrapNetIfTxCpss.c
*
* DESCRIPTION:
*       Wrapper functions for NetworkIf cpss functions
*
* FILE REVISION NUMBER:
 *       $Revision: 26 $
*
******************************************************************************/

/* Feature specific includes. */

#ifdef SHARED_MEMORY

#include <gtOs/gtOsSharedUtil.h>
#include <gtOs/gtOsSharedIPC.h>

#endif

#include <cmdShell/common/cmdCommon.h>


#include <cpss/generic/cpssTypes.h>
#include <cpss/generic/trunk/cpssGenTrunkTypes.h>
#include <cpss/generic/bridge/cpssGenBrgFdb.h>
#include <cpss/generic/events/cpssGenEventUnifyTypes.h>
#include <cpss/generic/events/cpssGenEventRequests.h>
#include <cpss/dxCh/dxChxGen/networkIf/cpssDxChNetIfTypes.h>
#include <cpss/dxCh/dxChxGen/networkIf/cpssDxChNetIf.h>
#include <cpss/dxCh/dxChxGen/bridge/cpssDxChBrgFdb.h>
#include <cpss/dxCh/dxChxGen/trunk/cpssDxChTrunk.h>
#include <galtisAgent/wrapCpss/dxCh/networkIf/cmdCpssDxChNetTransmit.h>
#include <cpss/extServices/private/prvCpssBindFunc.h>
#include <cpssCommon/private/prvCpssMemLib.h>
#include <galtisAgent/wrapUtil/cmdCpssPresteraUtils.h>

#ifdef CHX_FAMILY
#include <cpss/dxCh/dxChxGen/version/cpssDxChVersion.h>
#include <cpss/dxCh/dxChxGen/config/private/prvCpssDxChInfo.h>
#endif  /* CHX_FAMILY */

#ifdef SHARED_MEMORY

extern int multiProcessAppDemo;

typedef struct BufHead
{
    struct BufHead  *pNext;

}GT_ETH_PORT_POOL_FREE_BUF_HEAD;

/*
 * Typedef: struct gt_pool_head
 *
 * Description: Defines the pool ID
 *
 * Fields:
 *      poolBlock_PTR           - Pointer to the allocated memory block.
 *      alignedBlockStart_PTR   - Pointer to the aligned start address
 *                                of the pool's memory block.
 *      alignedBlockEnd_PTR     - Pointer to the aligned end address
 *                                of the pool's memory block.
 *      listHead_PTR            - Head of the buffers' list.
 *      poolSem                 - the pool semaphore
 *      memBlocSize             - Memory block size.
 *      reqBufferSize           - requested Buffer's size.
 *      neededBufferSize        - Actual buffer's size (after alignment).
 *      numOfBuffers            - Number of buffers in the pool.
 *      numOfFreeBuffers        - number of free buffers in the pool.
 */
typedef struct gt_pool_head
{
    GT_VOID                        *poolBlock_PTR;
    GT_VOID                        *alignedBlockStart_PTR;
    GT_VOID                        *alignedBlockEnd_PTR;
    GT_SEM                          poolSem;
    GT_ETH_PORT_POOL_FREE_BUF_HEAD *listHead_PTR;
    GT_U32                          memBlocSize;
    GT_U32                          reqBufferSize;
    GT_U32                          neededBufferSize;
    GT_U32                          numOfBuffers;
    GT_U32                          numOfFreeBuffers;
}GT_ETH_PORT_POOL_HEAD;


#ifdef APP_DEMO_CPU_ETH_PORT_MANAGE
extern GT_ETH_PORT_POOL_HEAD * poolArrPtr;
#endif

#endif


/*******************************************************************************
 * Internal definitions
 ******************************************************************************/
#define GT_GALTIS_TRANSMITE_TASK_PRIOR_CNS            100

/******************************** Globals *************************************/

/* Transmit Table descriptors */
GT_U32 wrCpssDxChNetIfTxDxChTblCapacity = 0;
DXCH_PKT_DESC_STC * cpssDxChTxPacketDescTbl = NULL;

/* Traffic generator transmit table descriptors */
GT_U32 wrCpssDxChNetIfTxGenDxChTblCapacity[8][8] = {{0}};
DXCH_PKT_GEN_DESC_STC * cpssDxChTxGenPacketDescTbl[8][8] = {{0}};

/* Transmit task ID */
static GT_TASK taskId = 0;
static GT_SEM  txCmdSemId;                      /* Module semaphore id  */
static GT_BOOL taskBusyFlag = GT_FALSE;

/* Transmit Task operation mode  */
/*static MODE_SETINGS_STC modeSettings = {0, 1};*/
static DXCH_MODE_SETINGS_STC modeSettings = {1, 1};
static GT_BOOL flagStopTransmit = GT_FALSE;
static GT_U8 txCookie = 0x10;

/******************************** Externals ***********************************/
extern GT_POOL_ID txBuffersPoolId;

GT_UINTPTR  cpssDxChTxGenPacketEvReqHndl = 0;


/*============================
          TX Transsmit
=============================*/

/*******************************************************************************
* startPacketTransmission
*
* DESCRIPTION:
*       Thi is thread function that performs trunsmition of packets defined
*       in the table.
*
* INPUTS:
*       None
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on error
*
* COMMENTS:
*
* GalTis:
*       None
*
*******************************************************************************/
static GT_STATUS startPacketTransmission(void)
{
    GT_U32      i;
    GT_U32      cycles;
    GT_STATUS   status = GT_OK;

    CPSS_DXCH_NET_TX_PARAMS_STC cpssPcktParams;
    GT_U8                       devNum=0;
    GT_PORT_NUM                 dummyPort = 0;/* dummy port num for conversion */
    GT_HW_DEV_NUM               dummyHwDev = 0;/* dummy dev num for conversion */
    GT_U8                       dev; /* device iterator */

    if (cpssDxChTxGenPacketEvReqHndl == 0)
    {
        CPSS_UNI_EV_CAUSE_ENT     evCause[1] = { CPSS_PP_TX_BUFFER_QUEUE_E };

        if (cmdCpssEventBind(evCause, 1, &cpssDxChTxGenPacketEvReqHndl) != GT_OK)
        {
            return GT_FAIL;
        }

        for(i = 0 ; i < 32 ; i++)
        {
            /* unmask this event with all HW devices */
            status = cmdCpssEventDeviceMaskSet((GT_U8)i,CPSS_PP_TX_BUFFER_QUEUE_E,CPSS_EVENT_UNMASK_E);
            if(status != GT_OK)
            {
                /* there are FA/XBAR events that not relevant to PP device,
                vice versa */
                continue;
            }
        }

    }


    cycles = 0;
    while (((cycles < modeSettings.cyclesNum) ||
            (modeSettings.cyclesNum == -1)) && (flagStopTransmit == GT_FALSE))
    {
        for(i = 0; i < wrCpssDxChNetIfTxDxChTblCapacity; i++) /*For each descriptor*/
        {
            GT_U32 j;

            /* If the transmition was disabled */
            if (flagStopTransmit == GT_TRUE)
            {
                break;
            }


            if (GT_TRUE != cpssDxChTxPacketDescTbl[i].valid)
            {
                /* entry is not valid */
                continue;
            }

            for (j = 0; j < cpssDxChTxPacketDescTbl[i].pcktsNum; j++)
            {/*For packet sent by this descriptor*/
                /* If the transmition was disabled */
                if (flagStopTransmit == GT_TRUE)
                {
                    break;
                }
                /* start by copy all the fields*/
                cpssPcktParams.dsaParam = cpssDxChTxPacketDescTbl[i].dsaParam;
                cpssPcktParams.sdmaInfo = cpssDxChTxPacketDescTbl[i].sdmaInfo;


                cpssPcktParams.packetIsTagged = cpssDxChTxPacketDescTbl[i].packetIsTagged;
                cpssPcktParams.cookie = &txCookie;
                cpssPcktParams.sdmaInfo.evReqHndl  = cpssDxChTxGenPacketEvReqHndl;
                cpssPcktParams.sdmaInfo.recalcCrc = cpssDxChTxPacketDescTbl[i].sdmaInfo.recalcCrc;
                cpssPcktParams.sdmaInfo.txQueue = cpssDxChTxPacketDescTbl[i].sdmaInfo.txQueue;
                cpssPcktParams.sdmaInfo.invokeTxBufferQueueEvent = cpssDxChTxPacketDescTbl[i].sdmaInfo.invokeTxBufferQueueEvent;


                cpssPcktParams.dsaParam.commonParams.dsaTagType = cpssDxChTxPacketDescTbl[i].dsaParam.commonParams.dsaTagType;
                cpssPcktParams.dsaParam.commonParams.vpt = cpssDxChTxPacketDescTbl[i].dsaParam.commonParams.vpt;
                cpssPcktParams.dsaParam.commonParams.cfiBit = cpssDxChTxPacketDescTbl[i].dsaParam.commonParams.cfiBit;
                cpssPcktParams.dsaParam.commonParams.vid = cpssDxChTxPacketDescTbl[i].dsaParam.commonParams.vid;
                cpssPcktParams.dsaParam.dsaType = cpssDxChTxPacketDescTbl[i].dsaParam.dsaType;

                switch(cpssDxChTxPacketDescTbl[i].dsaParam.dsaType)
                {
                    case CPSS_DXCH_NET_DSA_CMD_FROM_CPU_E:

                        cpssPcktParams.dsaParam.dsaInfo.fromCpu.tc = cpssDxChTxPacketDescTbl[i].dsaParam.dsaInfo.fromCpu.tc;
                        cpssPcktParams.dsaParam.dsaInfo.fromCpu.dp = cpssDxChTxPacketDescTbl[i].dsaParam.dsaInfo.fromCpu.dp;
                        cpssPcktParams.dsaParam.dsaInfo.fromCpu.egrFilterEn = cpssDxChTxPacketDescTbl[i].dsaParam.dsaInfo.fromCpu.egrFilterEn;
                        cpssPcktParams.dsaParam.dsaInfo.fromCpu.cascadeControl = cpssDxChTxPacketDescTbl[i].dsaParam.dsaInfo.fromCpu.cascadeControl;
                        cpssPcktParams.dsaParam.dsaInfo.fromCpu.egrFilterRegistered = cpssDxChTxPacketDescTbl[i].dsaParam.dsaInfo.fromCpu.egrFilterRegistered;
                        cpssPcktParams.dsaParam.dsaInfo.fromCpu.srcId = cpssDxChTxPacketDescTbl[i].dsaParam.dsaInfo.fromCpu.srcId;
                        cpssPcktParams.dsaParam.dsaInfo.fromCpu.srcHwDev = cpssDxChTxPacketDescTbl[i].dsaParam.dsaInfo.fromCpu.srcHwDev;
                        cpssPcktParams.dsaParam.dsaInfo.fromCpu.dstInterface.devPort.hwDevNum = cpssDxChTxPacketDescTbl[i].dsaParam.dsaInfo.fromCpu.dstInterface.devPort.hwDevNum;

                        dummyHwDev = cpssPcktParams.dsaParam.dsaInfo.fromCpu.dstInterface.devPort.hwDevNum;

                        /* the value of cpssPcktParams.dsaParam.dsaInfo.fromCpu.dstInterface.devPort.hwDevNum
                           was converted to 'hwDevNum' .

                           so to apply as 'sw devNum' we need to 'Convert it back'
                        */
                        CONVERT_BACK_DEV_PORT_DATA_MAC(dummyHwDev,dummyPort);
                        devNum = (GT_U8)dummyHwDev;


                        if(cpssDxChTxPacketDescTbl[i].cmdType == DXCH_TX_BY_VIDX)
                        {
                            cpssPcktParams.dsaParam.dsaInfo.fromCpu.dstInterface.type = cpssDxChTxPacketDescTbl[i].dsaParam.dsaInfo.fromCpu.dstInterface.type;
                            if(cpssPcktParams.dsaParam.dsaInfo.fromCpu.dstInterface.type == CPSS_INTERFACE_VIDX_E)
                            {
                                cpssPcktParams.dsaParam.dsaInfo.fromCpu.dstInterface.vidx = cpssDxChTxPacketDescTbl[i].dsaParam.dsaInfo.fromCpu.dstInterface.vidx;
                            }
                            else /*CPSS_INTERFACE_VID_E*/
                            {
                                cpssPcktParams.dsaParam.dsaInfo.fromCpu.dstInterface.vlanId = cpssDxChTxPacketDescTbl[i].dsaParam.dsaInfo.fromCpu.dstInterface.vlanId;
                            }

                            cpssPcktParams.dsaParam.dsaInfo.fromCpu.extDestInfo.multiDest.excludeInterface = cpssDxChTxPacketDescTbl[i].dsaParam.dsaInfo.fromCpu.extDestInfo.multiDest.excludeInterface;
                            cpssPcktParams.dsaParam.dsaInfo.fromCpu.extDestInfo.multiDest.excludedInterface.type = cpssDxChTxPacketDescTbl[i].dsaParam.dsaInfo.fromCpu.extDestInfo.multiDest.excludedInterface.type;

                            if(cpssPcktParams.dsaParam.dsaInfo.fromCpu.extDestInfo.multiDest.excludedInterface.type == 0) /*CPSS_INTERFACE_PORT_E*/
                            {
                                cpssPcktParams.dsaParam.dsaInfo.fromCpu.extDestInfo.multiDest.excludedInterface.devPort.hwDevNum = cpssDxChTxPacketDescTbl[i].dsaParam.dsaInfo.fromCpu.extDestInfo.multiDest.excludedInterface.devPort.hwDevNum;
                                cpssPcktParams.dsaParam.dsaInfo.fromCpu.extDestInfo.multiDest.excludedInterface.devPort.portNum = cpssDxChTxPacketDescTbl[i].dsaParam.dsaInfo.fromCpu.extDestInfo.multiDest.excludedInterface.devPort.portNum;
                            }
                            else/*CPSS_INTERFACE_TRUNK_E*/
                            {
                                cpssPcktParams.dsaParam.dsaInfo.fromCpu.extDestInfo.multiDest.excludedInterface.trunkId = cpssDxChTxPacketDescTbl[i].dsaParam.dsaInfo.fromCpu.extDestInfo.multiDest.excludedInterface.trunkId;
                                CONVERT_TRUNK_ID_TEST_TO_CPSS_MAC(cpssPcktParams.dsaParam.dsaInfo.fromCpu.extDestInfo.multiDest.excludedInterface.trunkId);
                            }
                        }
                        else/*DXCH_TX_BY_PORT*/
                        {
                            cpssPcktParams.dsaParam.dsaInfo.fromCpu.dstInterface.type = cpssDxChTxPacketDescTbl[i].dsaParam.dsaInfo.fromCpu.dstInterface.type;
                            cpssPcktParams.dsaParam.dsaInfo.fromCpu.dstInterface.devPort.hwDevNum = cpssDxChTxPacketDescTbl[i].dsaParam.dsaInfo.fromCpu.dstInterface.devPort.hwDevNum;
                            cpssPcktParams.dsaParam.dsaInfo.fromCpu.dstInterface.devPort.portNum = cpssDxChTxPacketDescTbl[i].dsaParam.dsaInfo.fromCpu.dstInterface.devPort.portNum;

                            cpssPcktParams.dsaParam.dsaInfo.fromCpu.extDestInfo.devPort.dstIsTagged = cpssDxChTxPacketDescTbl[i].dsaParam.dsaInfo.fromCpu.extDestInfo.devPort.dstIsTagged;
                            cpssPcktParams.dsaParam.dsaInfo.fromCpu.extDestInfo.devPort.mailBoxToNeighborCPU = cpssDxChTxPacketDescTbl[i].dsaParam.dsaInfo.fromCpu.extDestInfo.devPort.mailBoxToNeighborCPU;

                        }
                        break;

                    case CPSS_DXCH_NET_DSA_CMD_FORWARD_E:

                        cpssPcktParams.dsaParam.dsaInfo.fromCpu.tc = cpssDxChTxPacketDescTbl[i].dsaParam.dsaInfo.fromCpu.tc;
                        cpssPcktParams.dsaParam.dsaInfo.forward.srcIsTagged = cpssDxChTxPacketDescTbl[i].dsaParam.dsaInfo.forward.srcIsTagged;
                        cpssPcktParams.dsaParam.dsaInfo.forward.srcHwDev = cpssDxChTxPacketDescTbl[i].dsaParam.dsaInfo.forward.srcHwDev;
                        cpssPcktParams.dsaParam.dsaInfo.forward.srcIsTrunk = cpssDxChTxPacketDescTbl[i].dsaParam.dsaInfo.forward.srcIsTrunk;
                        cpssPcktParams.dsaParam.dsaInfo.forward.srcId = cpssDxChTxPacketDescTbl[i].dsaParam.dsaInfo.forward.srcId;
                        if(cpssPcktParams.dsaParam.dsaInfo.forward.srcIsTrunk == GT_TRUE)
                        {
                            cpssPcktParams.dsaParam.dsaInfo.forward.source.trunkId = cpssDxChTxPacketDescTbl[i].dsaParam.dsaInfo.forward.source.trunkId;
                            CONVERT_TRUNK_ID_TEST_TO_CPSS_MAC(cpssPcktParams.dsaParam.dsaInfo.forward.source.trunkId);
                        }
                        else
                            cpssPcktParams.dsaParam.dsaInfo.forward.source.portNum = cpssDxChTxPacketDescTbl[i].dsaParam.dsaInfo.forward.source.portNum;

                        cpssPcktParams.dsaParam.dsaInfo.forward.egrFilterRegistered = cpssDxChTxPacketDescTbl[i].dsaParam.dsaInfo.forward.egrFilterRegistered;
                        cpssPcktParams.dsaParam.dsaInfo.forward.wasRouted = cpssDxChTxPacketDescTbl[i].dsaParam.dsaInfo.forward.wasRouted;
                        cpssPcktParams.dsaParam.dsaInfo.forward.qosProfileIndex = cpssDxChTxPacketDescTbl[i].dsaParam.dsaInfo.forward.qosProfileIndex;

                        cpssPcktParams.dsaParam.dsaInfo.forward.dstInterface.devPort.hwDevNum = cpssDxChTxPacketDescTbl[i].dsaParam.dsaInfo.forward.dstInterface.devPort.hwDevNum;

                        dummyHwDev = cpssPcktParams.dsaParam.dsaInfo.forward.dstInterface.devPort.hwDevNum;
                        /* the value of cpssPcktParams.dsaParam.dsaInfo.fromCpu.dstInterface.devPort.hwDevNum
                           was converted to 'hwDevNum' .

                           so to apply as 'sw devNum' we need to 'Convert it back'
                        */
                        CONVERT_BACK_DEV_PORT_DATA_MAC(dummyHwDev,dummyPort);
                        devNum = (GT_U8)dummyHwDev;

                        if(cpssDxChTxPacketDescTbl[i].cmdType == DXCH_TX_BY_VIDX)
                        {
                            cpssPcktParams.dsaParam.dsaInfo.forward.dstInterface.type = cpssDxChTxPacketDescTbl[i].dsaParam.dsaInfo.forward.dstInterface.type;
                            if(cpssPcktParams.dsaParam.dsaInfo.forward.dstInterface.type == CPSS_INTERFACE_VIDX_E)
                                cpssPcktParams.dsaParam.dsaInfo.forward.dstInterface.vidx = cpssDxChTxPacketDescTbl[i].dsaParam.dsaInfo.forward.dstInterface.vidx;
                            else /* CPSS_INTERFACE_VID_E */
                                cpssPcktParams.dsaParam.dsaInfo.forward.dstInterface.vlanId = cpssDxChTxPacketDescTbl[i].dsaParam.dsaInfo.forward.dstInterface.vlanId;
                        }
                        else/*DXCH_TX_BY_PORT*/
                        {
                            cpssPcktParams.dsaParam.dsaInfo.forward.dstInterface.type = cpssDxChTxPacketDescTbl[i].dsaParam.dsaInfo.forward.dstInterface.type;
                            cpssPcktParams.dsaParam.dsaInfo.forward.dstInterface.devPort.hwDevNum = cpssDxChTxPacketDescTbl[i].dsaParam.dsaInfo.forward.dstInterface.devPort.hwDevNum;
                            cpssPcktParams.dsaParam.dsaInfo.forward.dstInterface.devPort.portNum = cpssDxChTxPacketDescTbl[i].dsaParam.dsaInfo.forward.dstInterface.devPort.portNum;
                        }
                        break;

                    default:
                        return GT_BAD_PARAM;
                }
                /* find the first active device */
                if(0 == PRV_CPSS_IS_DEV_EXISTS_MAC(devNum))
                {
                    for(dev = 0 ; dev < PRV_CPSS_MAX_PP_DEVICES_CNS; dev++)
                    {
                        if(prvCpssPpConfig[dev] != NULL)
                        {
                            devNum = dev;
                            break;
                        }
                    }
                }


                status = GT_EMPTY;
                while (status == GT_EMPTY)
                  {

                    if(cpssDxChTxPacketDescTbl[i].txSyncMode==GT_TRUE)
                      {
                        if(cmdIsCpuEtherPortUsed() == GT_TRUE) /* native mii ? */
                          {
                            status =
                              cpssDxChNetIfMiiSyncTxPacketSend
                              (devNum,
                               &cpssPcktParams,
                               cpssDxChTxPacketDescTbl[i].pcktData,
                               cpssDxChTxPacketDescTbl[i].pcktDataLen,
                               cpssDxChTxPacketDescTbl[i].numOfBuffers);
                          }
                        else
                          {
                            status =
                              cpssDxChNetIfSdmaSyncTxPacketSend
                              (devNum,
                               &cpssPcktParams,
                               cpssDxChTxPacketDescTbl[i].pcktData,
                               cpssDxChTxPacketDescTbl[i].pcktDataLen,
                               cpssDxChTxPacketDescTbl[i].numOfBuffers);
                          }
                      }
                    else /* async mode */
                      {
                        if(cmdIsCpuEtherPortUsed() == GT_TRUE) /* native mii ? */
                          {
                            status =
                              cpssDxChNetIfMiiTxPacketSend
                              (devNum,
                               &cpssPcktParams,
                               cpssDxChTxPacketDescTbl[i].pcktData,
                               cpssDxChTxPacketDescTbl[i].pcktDataLen,
                               cpssDxChTxPacketDescTbl[i].numOfBuffers);
                          }
                        else
                          {
                            status = cpssDxChNetIfSdmaTxPacketSend
                              (devNum,
                               &cpssPcktParams,
                               cpssDxChTxPacketDescTbl[i].pcktData,
                               cpssDxChTxPacketDescTbl[i].pcktDataLen,
                               cpssDxChTxPacketDescTbl[i].numOfBuffers);
                          }
                      }

                    /* Not enough descriptors to do the sending */
                    if (status == GT_EMPTY)
                      {
                        /* wait and try to send the packet again */
                        cmdOsTimerWkAfter(1);
                      }
                  }

                if((status != GT_OK) && (status != GT_NO_RESOURCE))
                {
                    cmdOsPrintf("Failed to send the packet, status = %d \n", status);
                    return status;
                }

                /* If returned status is GT_NO_RESOURCE then we should free */
                /* the elements in the fifo and then try to send the packet again */
                if ((((cpssDxChTxPacketDescTbl[i].txSyncMode == GT_FALSE) &&
                    (cpssDxChTxPacketDescTbl[i].sdmaInfo.invokeTxBufferQueueEvent == GT_TRUE)) ||
                    (status == GT_NO_RESOURCE)))
                {
                    GT_U8           _devNum=0;
                    GT_U8           queueIdx;
                    GT_STATUS       retVal;
                    GT_PTR          cookie;


                    /* wait for the Tx-End event */
                    retVal = cmdCpssEventSelect(cpssDxChTxGenPacketEvReqHndl,NULL,NULL,0);

                    if (retVal != GT_OK)
                    {
                        return retVal;
                    }

                    /* get all Tx end events for the packet */
                    if(cmdIsCpuEtherPortUsed() == GT_TRUE)
                    {
                        while(1)
                        {
                            retVal = cpssDxChNetIfMiiTxBufferQueueGet(cpssDxChTxGenPacketEvReqHndl,&_devNum,
                                                            &cookie,&queueIdx,&retVal);
                            if(retVal == GT_NO_MORE)
                            {
                                break;
                            }
                            if (retVal != GT_OK)
                            {
                                cmdOsPrintf("cpssDxChNetIfMiiTxBufferQueueGet - Failed , retVal = %d \n", retVal);
                                return retVal;
                            }
                        }
                    }
                    else
                    {
                        while(1)
                        {
                            retVal = cpssDxChNetIfTxBufferQueueGet(cpssDxChTxGenPacketEvReqHndl,&_devNum,
                                                            &cookie,&queueIdx,&retVal);
                            if(retVal == GT_NO_MORE)
                            {
                                break;
                            }
                            if (retVal != GT_OK)
                            {
                                cmdOsPrintf("cpssDxChNetIfTxBufferQueueGet - Failed , retVal = %d \n", retVal);
                                return retVal;
                            }
                        }
                    }

                  /* try to send the packet again*/
                  if(status == GT_NO_RESOURCE)
                  {
                        if(cpssDxChTxPacketDescTbl[i].txSyncMode==GT_TRUE)
                        {
                          if(cmdIsCpuEtherPortUsed() == GT_TRUE) /* native mii ? */
                          {
                              status =
                                cpssDxChNetIfMiiSyncTxPacketSend
                                (devNum,
                                 &cpssPcktParams,
                                 cpssDxChTxPacketDescTbl[i].pcktData,
                                 cpssDxChTxPacketDescTbl[i].pcktDataLen,
                                 cpssDxChTxPacketDescTbl[i].numOfBuffers);
                          }
                          else
                          {
                            status =
                              cpssDxChNetIfSdmaSyncTxPacketSend
                              (devNum,
                               &cpssPcktParams,
                               cpssDxChTxPacketDescTbl[i].pcktData,
                               cpssDxChTxPacketDescTbl[i].pcktDataLen,
                               cpssDxChTxPacketDescTbl[i].numOfBuffers);
                          }
                        }
                        else
                        {
                          if(cmdIsCpuEtherPortUsed() == GT_TRUE) /* native mii ? */
                          {
                                status =
                                      cpssDxChNetIfMiiTxPacketSend
                                      (devNum,
                                       &cpssPcktParams,
                                       cpssDxChTxPacketDescTbl[i].pcktData,
                                       cpssDxChTxPacketDescTbl[i].pcktDataLen,
                                       cpssDxChTxPacketDescTbl[i].numOfBuffers);
                          }
                          else
                          {
                            status =
                            cpssDxChNetIfSdmaTxPacketSend
                            (devNum,
                             &cpssPcktParams,
                             cpssDxChTxPacketDescTbl[i].pcktData,
                             cpssDxChTxPacketDescTbl[i].pcktDataLen,
                             cpssDxChTxPacketDescTbl[i].numOfBuffers);
                          }
                        }

                         if (status != GT_OK)
                         {
                             cmdOsPrintf("Failed to send the packet (2), status = %d \n", status);
                             return status;
                         }
                    }
                }

                /* in case transmit succeed */
                cpssDxChTxPacketDescTbl[i].numSentPackets++;


                /* wait n milliseconds before sending next packet */
                if(cpssDxChTxPacketDescTbl[i].gap != 0)
                {
                    cmdOsTimerWkAfter(cpssDxChTxPacketDescTbl[i].gap);
                }
            }

            /* wait n milliseconds before moving to the next entry */
            if(cpssDxChTxPacketDescTbl[i].waitTime != 0)
            {
                cmdOsTimerWkAfter(cpssDxChTxPacketDescTbl[i].waitTime);
            }
        }


        /* wait n milliseconds before starting the next cycle */
        if(modeSettings.gap != 0)
        {
            cmdOsTimerWkAfter(modeSettings.gap);
        }
        /* move to the next cycle */
        cycles++;
    }

    return GT_OK;
}

/*******************************************************************************
* cpssDxChPacketTransminitionTask
*
* DESCRIPTION:
*       galtis task for packet transmission.
*
* INPUTS:
*       None
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on error
*
* COMMENTS:
*
* GalTis:
*       None
*
*******************************************************************************/
GT_STATUS __TASKCONV cpssDxChPacketTransminitionTask()
{
    GT_STATUS rc;

    /* while forever */
    while(1)
    {
        /* wait on the TX command semaphore */
        taskBusyFlag = GT_FALSE;
        cmdOsSigSemWait(txCmdSemId, CPSS_OS_SEM_WAIT_FOREVER_CNS);
        taskBusyFlag = GT_TRUE;

        rc = startPacketTransmission();
        if (rc == GT_ABORTED)
        {
            break;
        }
    }
    return GT_OK;
}

/*******************************************************************************
* cmdCpssDxChTxSetPacketDesc
*
* DESCRIPTION:
*       Creates new transmit parameters description entry in the table.
*
* INPUTS:
*       entryID      - Entry ID of packet descriptor to return.
*       packetDesc   - Packet descriptor fro new entry in the table
*       extended     - flag indicates if this is the extended table
*
* OUTPUTS:
*       newEntryIndex   - The index of the new entry in the table.
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on error
*
* COMMENTS:
*       The creation is always for one segment. The pointer to the data is
*       allocated by the wrapper and always arrived in index 0 of packetDesc
*
* GalTis:
*       Table - cmdTxPacketDescs
*
*******************************************************************************/
GT_STATUS cmdCpssDxChTxSetPacketDesc
(
    IN  GT_U32                  entryId,
    IN  DXCH_PKT_DESC_STC       *packetDesc,
    IN  GT_BOOL                 extended,
    OUT GT_U32                  *newEntryIndex
)
{
    GT_U32  entryIndex;


    /* Prepare local variables and check whether valid packet */
    if(packetDesc->pcktDataLen[0] == 0)
    {
        return GT_FAIL;
    }

    for(entryIndex = 0; entryIndex < wrCpssDxChNetIfTxDxChTblCapacity; entryIndex++)
    {
        if (GT_FALSE == cpssDxChTxPacketDescTbl[entryIndex].valid)
        {
            /* we found a free space */
            break;
        }

        if(cpssDxChTxPacketDescTbl[entryIndex].entryId == entryId)
        {
            /* Non-extended mode does not permit to change entry data */
            if (extended == GT_FALSE)
                return GT_ALREADY_EXIST ;

            /* Extended mode allows to add packet segments */
            break;

        }
    }

    if(entryIndex == wrCpssDxChNetIfTxDxChTblCapacity)
    {
        /* Entry not found - creates new one */
        wrCpssDxChNetIfTxDxChTblCapacity++;

        /* allocate new TX packet descriptor*/
        if(cpssDxChTxPacketDescTbl == NULL)
        {
            cpssDxChTxPacketDescTbl = cmdOsMalloc(wrCpssDxChNetIfTxDxChTblCapacity * sizeof(DXCH_PKT_DESC_STC));
        }
        else
        {
            cpssDxChTxPacketDescTbl = cmdOsRealloc(cpssDxChTxPacketDescTbl,
                                        wrCpssDxChNetIfTxDxChTblCapacity * sizeof(DXCH_PKT_DESC_STC));
        }
        if(cpssDxChTxPacketDescTbl == NULL)
        {
            wrCpssDxChNetIfTxDxChTblCapacity = 0;
            return GT_OUT_OF_CPU_MEM;
        }
        cmdOsMemSet(&cpssDxChTxPacketDescTbl[entryIndex], 0, sizeof(DXCH_PKT_DESC_STC));
    }

    cmdOsMemCpy(&cpssDxChTxPacketDescTbl[entryIndex], packetDesc, sizeof(DXCH_PKT_DESC_STC));

    cpssDxChTxPacketDescTbl[entryIndex].valid = GT_TRUE;

    *newEntryIndex = entryIndex;

    return GT_OK;
}



/*******************************************************************************
* cmdCpssDxChTxGetPacketDesc
*
* DESCRIPTION:
*       Returns specific packet descriptor entry from the table.
*
* INPUTS:
*       entryIndex   - Entry index in packet descriptor table.
*       dsaCmd       - DSA tag commands
*
* OUTPUTS:
*     packetDesc   - Packet descriptor from the table.
*
* RETURNS:
*       GT_OK    - on success
*       GT_FAIL  - on error
*       GT_EMPTY - on invalid index
*
* COMMENTS:
*
* GalTis:
*       Table - cmdTxPacketDescs
*
*******************************************************************************/
GT_STATUS cmdCpssDxChTxGetPacketDesc
(
    INOUT GT_U32            *entryIndex,
    IN CPSS_DXCH_NET_DSA_CMD_ENT  dsaCmd,
    OUT DXCH_PKT_DESC_STC   *packetDesc
)
{
    GT_U8   i;

    /* check if index is valid for the table */
    if(*entryIndex >= wrCpssDxChNetIfTxDxChTblCapacity)
        return GT_NOT_FOUND;


    for (; *entryIndex < wrCpssDxChNetIfTxDxChTblCapacity; (*entryIndex)++)
    {
        if ((GT_TRUE == cpssDxChTxPacketDescTbl[*entryIndex].valid) &&
            (cpssDxChTxPacketDescTbl[*entryIndex].dsaParam.dsaType == dsaCmd))
        {
            /* Entry found */
            break;
        }
    }

    if (*entryIndex == wrCpssDxChNetIfTxDxChTblCapacity)
    {
        /* entry not found */
        return GT_NOT_FOUND;
    }

    /* copy packet descriptor */
    cmdOsMemSet(packetDesc, 0, sizeof(DXCH_PKT_DESC_STC));
    cmdOsMemCpy(packetDesc, &cpssDxChTxPacketDescTbl[*entryIndex], sizeof(DXCH_PKT_DESC_STC));

    /* allocate memory for the pkt data */
    for( i = 0; i < MAX_NUM_OF_SDMA_BUFFERS_PER_CHAIN; i++)
    {
        if(cpssDxChTxPacketDescTbl[*entryIndex].pcktDataLen[i] == 0)
            break; /*End of the chain */

        packetDesc->pcktData[i] =
            cmdOsMalloc(cpssDxChTxPacketDescTbl[*entryIndex].pcktDataLen[i]);

        if(packetDesc->pcktData[i] == NULL)
        {
            return GT_OUT_OF_CPU_MEM;
        }

        /* copy pkt data */
        cmdOsMemCpy(packetDesc->pcktData[i],
                     cpssDxChTxPacketDescTbl[*entryIndex].pcktData[i],
                     cpssDxChTxPacketDescTbl[*entryIndex].pcktDataLen[i]);
    }

    return GT_OK;
}

/*******************************************************************************
* cmdCpssDxChTxBeginGetPacketDesc
*
* DESCRIPTION:
*       Indicates for cmdCpssDxChTxGetPacketDesc function that user go to get many
*       records in one session. This API make exact copy of table so that
*       user will get table without changes that can be madce during get
*       operation. This mechanism is best suitable for Refresh table
*       operation.
*
* INPUTS:
*       None
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK    - on success
*       GT_FAIL  - on error
*
* COMMENTS:
*
* GalTis:
*       Table - cmdTxPacketDescs
*
*******************************************************************************/
GT_STATUS cmdCpssDxChTxBeginGetPacketDesc
(
)
{
    return GT_OK;
}

/*******************************************************************************
* cmdCpssDxChTxEndGetPacketDesc
*
* DESCRIPTION:
*       Finish operation began by cmdCpssDxChTxBeginGetPacketDesc API function
*
* INPUTS:
*       None
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK    - on success
*       GT_FAIL  - on error
*
* COMMENTS:
*
* GalTis:
*       Table - cmdTxPacketDescs
*
*******************************************************************************/
GT_STATUS cmdCpssDxChTxEndGetPacketDesc
(
)
{
    return GT_OK;
}

/*******************************************************************************
* cmdCpssDxChConvertPacketDesc
*
* DESCRIPTION:
*       Convert packet desciptor structure to CPSS_DXCH_NET_TX_PARAMS_STC
*
* INPUTS:
*       netTxPacketDescPtr      - pointer to TX packet descriptor structure
*
* OUTPUTS:
*       cpssNetTxPacketDescPtr  - pointer to CPSS TX packet descriptor structure
*
* RETURNS:
*       None
*
* COMMENTS:
*
*******************************************************************************/
GT_VOID cmdCpssDxChConvertPacketDesc
(
    IN DXCH_PKT_DESC_STC                * netTxPacketDescPtr,
    OUT CPSS_DXCH_NET_TX_PARAMS_STC     * cpssNetTxPacketDescPtr
)
{
    /* start by copy all the fields*/
    cpssNetTxPacketDescPtr->dsaParam = netTxPacketDescPtr->dsaParam;
    cpssNetTxPacketDescPtr->sdmaInfo = netTxPacketDescPtr->sdmaInfo;


    cpssNetTxPacketDescPtr->packetIsTagged = netTxPacketDescPtr->packetIsTagged;
    cpssNetTxPacketDescPtr->cookie = &txCookie;
    cpssNetTxPacketDescPtr->sdmaInfo.evReqHndl  = cpssDxChTxGenPacketEvReqHndl;
    cpssNetTxPacketDescPtr->sdmaInfo.recalcCrc = netTxPacketDescPtr->sdmaInfo.recalcCrc;
    cpssNetTxPacketDescPtr->sdmaInfo.txQueue = netTxPacketDescPtr->sdmaInfo.txQueue;
    cpssNetTxPacketDescPtr->sdmaInfo.invokeTxBufferQueueEvent = netTxPacketDescPtr->sdmaInfo.invokeTxBufferQueueEvent;

    return;
}

/*******************************************************************************
* cmdCpssDxChTxDelPacketDesc
*
* DESCRIPTION:
*       Delete packet descriptor entry from the table.
*
* INPUTS:
*       entryId     - Entry ID of packet descriptor to delete.
*       dsaCmd       - DSA tag commands
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK       - on success
*       GT_FAIL     - on error
*
* COMMENTS:
*       After delete operation all record will be shifted
*       so the table will be not fragmented.
*
* GalTis:
*       Table - cmdTxPacketDescs
*
*******************************************************************************/
GT_STATUS cmdCpssDxChTxDelPacketDesc
(
    IN  GT_U32      entryId,
    IN CPSS_DXCH_NET_DSA_CMD_ENT  dsaCmd
)
{
    GT_U32          j;
    DXCH_PKT_DESC_STC   *txPcktDescPtr;
    GT_U32          entryIndex;

    /* make sure there is no send */
    cmdCpssDxChTxStop();

    if(cpssDxChTxPacketDescTbl != NULL)
    {
        /* find the entry */
        for (entryIndex = 0; entryIndex < wrCpssDxChNetIfTxDxChTblCapacity; entryIndex++)
        {
            if (cpssDxChTxPacketDescTbl[entryIndex].dsaParam.dsaType == dsaCmd)
            {
                if (GT_FALSE == cpssDxChTxPacketDescTbl[entryIndex].valid)
                {
                    /* invalid entry */
                    continue;
                }

                if (cpssDxChTxPacketDescTbl[entryIndex].entryId == entryId)

                {
                    /* Entry found */
                    break;
                }
            }
        }

        if (entryIndex == wrCpssDxChNetIfTxDxChTblCapacity)
        {
            /* entry not found */
            return GT_NOT_FOUND;
        }

        txPcktDescPtr = &cpssDxChTxPacketDescTbl[entryIndex];

        /* release tx buffers */
        for( j=0; j < MAX_NUM_OF_SDMA_BUFFERS_PER_CHAIN ; j++ )
        {
            if (0 == txPcktDescPtr->pcktDataLen[j])
            {
                break;
            }

            if(txPcktDescPtr->pcktData[j] != NULL)
            {

                cmdOsPoolFreeBuf(txBuffersPoolId, txPcktDescPtr->pcktData[j]);
            }
        }/*For all buffers for this descriptor*/

        /* clear the packet descriptor data */
        cmdOsMemSet(txPcktDescPtr, 0, sizeof(DXCH_PKT_DESC_STC));
    }

    return GT_OK;
}

/*******************************************************************************
* cmdCpssDxChTxClearPacketDesc
*
* DESCRIPTION:
*       Clear all packet descriptors entries from the table.
*
* INPUTS:
*     dsaCmd       - DSA tag commands
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on error
*
* COMMENTS:
*
* GalTis:
*       Table - cmdTxPacketDescs
*
*******************************************************************************/
GT_STATUS cmdCpssDxChTxClearPacketDesc
(
    IN CPSS_DXCH_NET_DSA_CMD_ENT  dsaCmd
)
{
    GT_U32 i,j;
    GT_U32 numberOfPacketsDeleted = 0;
    GT_U32 numberOfPacketsNotValid = 0;
    DXCH_PKT_DESC_STC   *txPcktDescPtr;

    /* make sure there is no send */
    cmdCpssDxChTxStop();

    if(cpssDxChTxPacketDescTbl != NULL)
    {
         /* release tx buffers */
         for(i = 0; i < wrCpssDxChNetIfTxDxChTblCapacity;i++)
         {
             if (GT_TRUE != cpssDxChTxPacketDescTbl[i].valid)
             {
                 numberOfPacketsNotValid++;
                 /* entry is not valid */
                 continue;
             }

             if (cpssDxChTxPacketDescTbl[i].dsaParam.dsaType == dsaCmd)
             {
                 txPcktDescPtr = &cpssDxChTxPacketDescTbl[i];

                 for( j=0; j < MAX_NUM_OF_SDMA_BUFFERS_PER_CHAIN ; j++ )
                 {
                     if (0 == txPcktDescPtr->pcktDataLen[j])
                     {
                         break;
                     }

                     if (txPcktDescPtr->pcktData[j] != NULL)
                         cmdOsPoolFreeBuf(txBuffersPoolId, txPcktDescPtr->pcktData[j]);

                 }/*For all buffers for this descriptor*/

                 /* clear the packet descriptor data */
                 cmdOsMemSet(txPcktDescPtr, 0, sizeof(DXCH_PKT_DESC_STC));

                 numberOfPacketsDeleted++;
             }

         }/*For all descriptors*/

         if((numberOfPacketsDeleted + numberOfPacketsNotValid) == wrCpssDxChNetIfTxDxChTblCapacity)
         {
             /* free allocated table */
             cmdOsFree(cpssDxChTxPacketDescTbl);
             cpssDxChTxPacketDescTbl = NULL;
             wrCpssDxChNetIfTxDxChTblCapacity = 0;
         }
    }

    return GT_OK;
}


#ifdef SHARED_MEMORY
static CPSS_MP_REMOTE_COMMAND remoteCmd;
#endif

/*******************************************************************************
* cmdCpssDxChTxStart
*
* DESCRIPTION:
*       Starts transmition of packets
*
* INPUTS:
*       None
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on error
*
* COMMENTS:
*
* GalTis:
*       Command     - cmdCpssDxChTxStart
*
*******************************************************************************/
GT_STATUS cmdCpssDxChTxStart
(
)
{
    GT_STATUS rc = GT_OK;
    static GT_BOOL   enterOnce = GT_FALSE;

    /* check if there are element in the transmit table */
    if(wrCpssDxChNetIfTxDxChTblCapacity == 0)
        return GT_OK;

#ifdef SHARED_MEMORY
    /* if compiled with SHARED_MAMORY defined
        and if variable multiProcessAppDemo == 1 the multiprocess
        appDemo will be run */

    if(multiProcessAppDemo != 0)
    {
        remoteCmd.remote = CPSS_MP_REMOTE_RXTX_E;
        remoteCmd.command = CPSS_MP_CMD_RXTX_START_TRANSMIT_E;

                /* Fill TX parameters for RxTxProcess packet transmission */
                remoteCmd.data.rxTxStartTransmit.cpssDxChTxPacketDescTbl = cpssDxChTxPacketDescTbl;
                remoteCmd.data.rxTxStartTransmit.txDxChTblCapacity = wrCpssDxChNetIfTxDxChTblCapacity;
                remoteCmd.data.rxTxStartTransmit.modeSettings.cyclesNum = modeSettings.cyclesNum;
                remoteCmd.data.rxTxStartTransmit.modeSettings.gap = modeSettings.gap;

                #ifdef APP_DEMO_CPU_ETH_PORT_MANAGE
                CPSS_TBD_BOOKMARK
                /* Shared Memory implementation should be updated to use native MII APIs. */
                remoteCmd.data.rxTxStartTransmit.txPoolPtr = (void *)poolArrPtr;
                #endif


        return cpssMultiProcComExecute(&remoteCmd);
    }
#endif
    /* creat the task only once */
    if(enterOnce == GT_FALSE)
    {

        if(cmdOsSigSemBinCreate("txCmd",CPSS_OS_SEMB_EMPTY_E,&txCmdSemId) != GT_OK)
        {
            return GT_FAIL;
        }
        if(cmdOsTaskCreate("GalPktTx",                           /* Task Name                      */
                        GT_GALTIS_TRANSMITE_TASK_PRIOR_CNS,   /* Task Priority                  */
                        0x1000,                               /* Stack Size _4KB                */
                        (unsigned (__TASKCONV *)(void*))cpssDxChPacketTransminitionTask, /* Starting Point */
                        (GT_VOID*)NULL,                       /* there is no arguments */
                        &taskId) != GT_OK)                    /* returned task ID */
            return GT_FAIL;
        enterOnce = GT_TRUE;
    }

    /* check if the last transmition is done */
    if(taskBusyFlag == GT_TRUE)
        return GT_FAIL;

    flagStopTransmit = GT_FALSE;

    /* send a signal for the task to start the transmission */
    cmdOsSigSemSignal(txCmdSemId);

    return  rc;
}

/*******************************************************************************
* cmdCpssDxChTxStop
*
* DESCRIPTION:
*       Stop transmition of packets.
*
* INPUTS:
*       None
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on error
*
* COMMENTS:
*
* GalTis:
*       Command     - cmdCpssDxChTxStop
*
*******************************************************************************/
GT_STATUS cmdCpssDxChTxStop
(
    void
)
{
    GT_U32 timeOutCnt = 10;
#ifdef SHARED_MEMORY
    /* if compiled with SHARED_MAMORY defined
        and if variable multiProcessAppDemo == 1 the multiprocess
        appDemo will be run */

    if(multiProcessAppDemo != 0)
    {
        remoteCmd.remote = CPSS_MP_REMOTE_RXTX_E;
        remoteCmd.command = CPSS_MP_CMD_RXTX_STOP_TRANSMIT_E;

        return cpssMultiProcComExecute(&remoteCmd);
    }
#endif
    /* there is no send task running */
    if(taskId == 0)
    {
        return GT_NO_CHANGE;
    }

    flagStopTransmit = GT_TRUE;
    while (taskBusyFlag == GT_TRUE && timeOutCnt > 0)
    {
        cmdOsTimerWkAfter(50);
        timeOutCnt--;
    }
    if (taskBusyFlag == GT_TRUE)
    {
        return GT_FAIL;
    }
    return GT_OK;
}


/*******************************************************************************
* cmdCpssDxChTxSetMode
*
* DESCRIPTION:
*       This command will set the the transmit parameters.
*
* INPUTS:
*       cyclesNum - The maximum number of loop cycles (-1 = forever)
*       gap       - The time to wit between two cycles
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on error
*
* COMMENTS:
*
* GalTis:
*       Command     - cmdTxSetMode
*       Interface   - <prestera/tapi/networkif/commands.api>
*
*******************************************************************************/
GT_STATUS cmdCpssDxChTxSetMode
(
    IN GT_U32   cyclesNum,
    IN GT_U32   gap
)
{
    modeSettings.cyclesNum = cyclesNum;
    modeSettings.gap = gap;

    return GT_OK;
}


/*******************************************************************************
* cmdCpssDxChTxGetMode
*
* DESCRIPTION:
*       This command will get the the transmit parameters.
*
* INPUTS:
*       None
*
* OUTPUTS:
*       cyclesNum - The maximum number of loop cycles (-1 = forever)
*       gap       - The time to wit between two cycles.
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on error
*
* COMMENTS:
*
* GalTis:
*       Command - cmdCpssDxChTxGetMode
*
*******************************************************************************/
GT_STATUS cmdCpssDxChTxGetMode
(
    OUT GT_U32 *cyclesNum,
    OUT GT_U32 *gap
)
{
    *cyclesNum = modeSettings.cyclesNum;
    *gap = modeSettings.gap;

    return GT_OK;
}

/*******************************************************************************
* cmdCpssDxChTxGenPacketDescSet
*
* DESCRIPTION:
*       Bind TX generator transmit parameters description entry to DB.
*
* INPUTS:
*       packetGenDescPtr        - pointer to TX Packet generator descriptor
*       packetGenDescTblEntryPtrPtr - pointer to pointer to TX packet generator descriptor table entry
*       packetGenDescTblSizePtr - pointer to TX packet generator descriptor tale size
*
* OUTPUTS:
*       packetDescTblEntryPtr - pointer to updated TX packet generator descriptor table entry
*       packetDescTblSizePtr - pointer to updated TX packet generator descriptor tale size
*       newEntryIndex   - The index of the new entry in the table.
*
* RETURNS:
*       GT_OK               - on success
*       GT_OUT_OF_CPU_MEM   - on memory allocation fail
*       GT_ALREADY_EXIST    - on already exist entry
*
* COMMENTS:
*       None
*
*
*******************************************************************************/
GT_STATUS cmdCpssDxChTxGenPacketDescSet
(
    IN  DXCH_PKT_GEN_DESC_STC       * packetGenDescPtr,
    INOUT DXCH_PKT_GEN_DESC_STC     ** packetGenDescTblEntryPtrPtr,
    INOUT GT_U32                    * packetGenDescTblSizePtr,
    OUT GT_U32                      * newEntryIndexPtr
)
{
    GT_U32  entryIndex;
    GT_U32  packetDescTblSize;
    DXCH_PKT_GEN_DESC_STC  *packetGenDescTblEntryPtr;

    packetDescTblSize = *packetGenDescTblSizePtr;
    packetGenDescTblEntryPtr = *packetGenDescTblEntryPtrPtr;

    for(entryIndex = 0; entryIndex < packetDescTblSize; entryIndex++)
    {
        /* Entry alreday exists in database */
        if(packetGenDescTblEntryPtr[entryIndex].packetId == packetGenDescPtr->packetId)
        {
            if (packetGenDescPtr->actionType == DXCH_TX_GEN_ACTION_ADD_E)
            {
                return GT_ALREADY_EXIST;
            }
            /* Update entry */
            break;
        }
    }

    if(entryIndex == packetDescTblSize)
    {
        /* Entry not found - creates new one */
        packetDescTblSize++;

        /* Allocate new TX generator packet descriptor */
        if(packetGenDescTblEntryPtr == NULL)
        {
            packetGenDescTblEntryPtr =
                cmdOsMalloc(packetDescTblSize * sizeof(DXCH_PKT_GEN_DESC_STC));
        }
        else
        {
            packetGenDescTblEntryPtr =
                cmdOsRealloc(packetGenDescTblEntryPtr,
                             packetDescTblSize * sizeof(DXCH_PKT_GEN_DESC_STC));
        }
        if(packetGenDescTblEntryPtr == NULL)
        {
            packetDescTblSize = 0;
            return GT_OUT_OF_CPU_MEM;
        }
        cmdOsMemSet(&packetGenDescTblEntryPtr[entryIndex], 0, sizeof(DXCH_PKT_GEN_DESC_STC));
    }

    /* Set table entry in database */
    cmdOsMemCpy(&packetGenDescTblEntryPtr[entryIndex], packetGenDescPtr,
                sizeof(DXCH_PKT_GEN_DESC_STC));

    *newEntryIndexPtr = entryIndex;
    *packetGenDescTblSizePtr = packetDescTblSize;
    *packetGenDescTblEntryPtrPtr = packetGenDescTblEntryPtr;

    return GT_OK;
}

/*******************************************************************************
* cmdCpssDxChTxGenPacketDescDelete
*
* DESCRIPTION:
*       Delete TX generator transmit parameters description entry.
*
* INPUTS:
*       devNum                      - Device number.
*       txQueue                     - Tx queue.
*       portGroup                   - Port group ID.
*       packetId                    - Packet ID to delete.
*       packetGenDescTblEntryPtrPtr - pointer to pointer to TX packet generator table
*
* OUTPUTS:
*       packetGenDescTblEntryPtrPtr - pointer to TX packet generator table
*
* RETURNS:
*       GT_OK           - on success
*       GT_NOT_FOUND    - on entry not found
*       GT_BAD_PTR      - on bad entry pointer
*       GT_EMPTY        - on empty table
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS cmdCpssDxChTxGenPacketDescDelete
(
    IN GT_U8                            devNum,
    IN GT_U8                            txQueue,
    IN GT_U32                           portGroup,
    IN GT_U32                           packetId,
    INOUT DXCH_PKT_GEN_DESC_STC      ** packetGenDescTblEntryPtrPtr,
    INOUT GT_U32                      * packetGenDescTblSizePtr
)
{
    DXCH_PKT_GEN_DESC_STC * packetGenDescTblEntryPtr;
    DXCH_PKT_GEN_DESC_STC * txPcktGenDescPtr;       /* Pointer to TX queue generator entry */
    GT_U32                  entryIndex;             /* Table entry index */
    GT_PORT_GROUPS_BMP      portGroupBmp;           /* Port group bitmap */
    GT_STATUS               result;                 /* Return status */
    GT_U32                  packetDescTblSize;

    packetDescTblSize = *packetGenDescTblSizePtr;
    packetGenDescTblEntryPtr = *packetGenDescTblEntryPtrPtr;

    if(packetGenDescTblEntryPtr == 0)
    {
        return GT_EMPTY;
    }

    if(packetGenDescTblSizePtr == 0)
    {
        return GT_BAD_PTR;
    }
    portGroupBmp = (1 << portGroup);

    /* find the entry */
    for (entryIndex = 0; entryIndex < packetDescTblSize; entryIndex++)
    {
        if (packetGenDescTblEntryPtr[entryIndex].packetId == packetId)
        {
            /* Entry found */
            txPcktGenDescPtr = &packetGenDescTblEntryPtr[entryIndex];
            result = cpssDxChNetIfSdmaTxGeneratorPacketRemove(devNum,
                                              portGroupBmp, txQueue, packetId);
            if(result != GT_OK)
            {
                return result;
            }

            if(entryIndex < packetDescTblSize - 1)
            {
                /* Copy data to avoid memory fragmentation */
                cmdOsMemCpy(txPcktGenDescPtr, txPcktGenDescPtr + 1,
                           (packetDescTblSize - entryIndex - 1) * sizeof(DXCH_PKT_GEN_DESC_STC));
            }

            break;
        }
    }

    if (entryIndex == packetDescTblSize)
    {
        /* Entry not found */
        return GT_NOT_FOUND;
    }

    packetDescTblSize--;
    if(packetDescTblSize)
    {
        cmdOsRealloc(packetGenDescTblEntryPtr,
                     packetDescTblSize * sizeof(DXCH_PKT_GEN_DESC_STC));
    }

    *packetGenDescTblEntryPtrPtr = packetGenDescTblEntryPtr;
    *packetGenDescTblSizePtr = packetDescTblSize;

    return GT_OK;
}

/*******************************************************************************
* cmdCpssDxChTxGenPacketDescClearAll
*
* DESCRIPTION:
*       Clear TX generator transmit parameters DB.
*
* INPUTS:
*       devNum                      - Device number.
*       txQueue                     - Tx queue.
*       portGroup                   - Port group ID.
*       packetGenDescTblEntryPtrPtr - pointer to pointer to TX packet generator table
*
* OUTPUTS:
*       packetGenDescTblEntryPtrPtr - pointer to pointer to TX packet generator table
*
* RETURNS:
*       GT_OK           - on success
*       GT_NOT_FOUND    - on entry not found
*       GT_EMPTY        - on empty table
*
* COMMENTS:
*       None
*
*
*******************************************************************************/
GT_STATUS cmdCpssDxChTxGenPacketDescClearAll
(
    IN GT_U8                            devNum,
    IN GT_U8                            txQueue,
    IN GT_U32                           portGroup,
    INOUT DXCH_PKT_GEN_DESC_STC      ** packetGenDescTblEntryPtrPtr
)
{
    GT_STATUS               result;                 /* Return status */
    DXCH_PKT_GEN_DESC_STC   txPcktGenDesc;          /* Pointer to TX queue generator entry */
    GT_U32                  packetId;               /* Packet ID */
    DXCH_PKT_GEN_DESC_STC * packetGenDescTblEntryPtr;/* Pointer to TX packet generator table */
    GT_U32                  packetGenDescTblSize;   /* Packet generator table size */

    packetGenDescTblEntryPtr = *packetGenDescTblEntryPtrPtr;

    if(packetGenDescTblEntryPtr == NULL)
    {
        return GT_EMPTY;
    }

    packetGenDescTblSize = wrCpssDxChNetIfTxGenDxChTblCapacity[portGroup][txQueue];
    while(packetGenDescTblSize)
    {
        /* Get current entry */
        result = cmdCpssDxChTxGenPacketDescGet(0,
                                               packetGenDescTblEntryPtr,
                                               packetGenDescTblSize,
                                               &txPcktGenDesc);
        if(result != GT_OK)
        {
            return result;
        }

        packetId = txPcktGenDesc.packetId;
        /* Delete TX packet generator packet */
        result = cmdCpssDxChTxGenPacketDescDelete(devNum, txQueue,
                                            portGroup, packetId,
                                            &packetGenDescTblEntryPtr,
                                            &wrCpssDxChNetIfTxGenDxChTblCapacity[portGroup][txQueue]);
        if(result != GT_OK)
        {
            return result;
        }
    }

    /* Free allocated table */
    cmdOsFree(packetGenDescTblEntryPtr);
    *packetGenDescTblEntryPtrPtr = 0;

    return GT_OK;
}

/*******************************************************************************
* cmdCpssDxChTxGenPacketDescGet
*
* DESCRIPTION:
*       Returns TX generator packet descriptor entry from the table.
*
* INPUTS:
*       entryIndex          - table entry index
*       packetGenDescTblEntryPtr
*                           - pointer to TX packet generator table
*       packetGenDescTblSize
*                           - TX packet generator table size
*
* OUTPUTS:
*       packetGenDescPtr    - pointer to entry in TX packet generator descriptor table.
*
* RETURNS:
*       GT_OK    - on success
*       GT_BAD_PTR - on bad entry pointer
*       GT_OUT_OF_RANGE - on invalid index
*       GT_EMPTY - on empty table
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS cmdCpssDxChTxGenPacketDescGet
(
    IN  GT_U32                      entryIndex,
    IN  DXCH_PKT_GEN_DESC_STC       * packetGenDescTblEntryPtr,
    IN  GT_U32                      packetGenDescTblSize,
    OUT DXCH_PKT_GEN_DESC_STC       * packetGenDescPtr
)
{
    if(packetGenDescTblEntryPtr == 0)
    {
        return GT_EMPTY;
    }

    if(packetGenDescPtr == 0)
    {
        return GT_BAD_PTR;
    }

    /* Check if index is valid for the table */
    if(entryIndex >= packetGenDescTblSize)
    {
        return GT_OUT_OF_RANGE;
    }

    /* Copy packet descriptor */
    cmdOsMemCpy(packetGenDescPtr, &packetGenDescTblEntryPtr[entryIndex],
                sizeof(DXCH_PKT_GEN_DESC_STC));

    return GT_OK;
}

