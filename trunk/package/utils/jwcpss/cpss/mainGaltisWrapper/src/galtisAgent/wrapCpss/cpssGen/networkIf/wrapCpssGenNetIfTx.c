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
*       $Revision: 31 $
*
******************************************************************************/

#ifdef SHARED_MEMORY
#  include <gtOs/gtOsSharedUtil.h>
#  include <gtOs/gtOsSharedIPC.h>
#endif

/* Common galtis includes */
#include <cmdShell/common/cmdCommon.h>
#include <galtisAgent/wrapUtil/cmdCpssPresteraUtils.h>
#include <cmdShell/cmdDb/cmdBase.h>
#include <cmdShell/common/cmdWrapUtils.h>

/* Feature specific includes. */
#include <cpss/generic/cpssTypes.h>
#include <cpss/generic/trunk/cpssGenTrunkTypes.h>
#include <cpss/generic/bridge/cpssGenBrgFdb.h>
#include <cpss/generic/events/cpssGenEventUnifyTypes.h>
#include <cpss/generic/events/cpssGenEventRequests.h>
#include <cpss/generic/config/private/prvCpssConfigTypes.h>

#include <galtisAgent/wrapCpss/Gen/networkIf/wrapCpssGenNetIf.h>

#include <cpss/extServices/private/prvCpssBindFunc.h>

#include <cpssCommon/private/prvCpssMemLib.h>

/*******************************************************************************
 * Internal definitions
 ******************************************************************************/
#define GT_GALTIS_TRANSMITE_TASK_PRIOR_CNS            100

/******************************** Variables ***********************************/

#ifdef SHARED_MEMORY
    extern int multiProcessAppDemo;
#endif

/* Transmit Table descriptors */
#define MAX_NUM_OF_PCAKETS_TO_SEND 100
static GT_U32 txTblCapacity = 0;
static CPSS_TX_PKT_DESC* txPacketDescTbl = NULL;

#if defined EXMXPM_FAMILY
    static CPSS_EXMXPM_SPECIFIC_TX_PKT_INFO_STC specificDeviceTypeInfo[MAX_NUM_OF_PCAKETS_TO_SEND];
#endif

/* Transmit task ID */
static GT_TASK taskId = 0;
static GT_SEM  txCmdSemId;                      /* Module semaphore id  */
static GT_BOOL taskBusyFlag = GT_FALSE;

/* Transmit Task operation mode  */
/*static MODE_SETINGS_STC modeSettings = {0, 1};*/
struct
{
    GT_U32  cyclesNum;
    GT_U32  gap;
}modeSettings = {1, 0};

static GT_BOOL flagStopTransmit = GT_FALSE;


#if defined EXMXPM_FAMILY || defined CHX_FAMILY || defined EX_FAMILY
/*============================
          TX Transsmit
=============================*/

static GT_U32  sdmaTxPacketSendCount = 0; /* Packets sent, and need to wait
                                             for TX_BUFF_Q event */
static GT_U32  sdmaTxPacketGetCount = 0;  /* Packet whos info got after receiving
                                             TX_BUFF_Q event. */
#endif

static GT_U8 txCookie = 0x10;
/******************************** Externals ***********************************/
static GT_POOL_ID genTxBuffersPoolId;

static const char hexcode[] = "0123456789ABCDEF";
static GT_BOOL  poolCreated = GT_FALSE;


static GT_UINTPTR  evReqHndl = 0, rx2TxEvReqHndl = 0;

#define INT_LOCK(key)                                                   \
    key = 0;                                                            \
    cmdOsTaskLock();                                                    \
    cmdExtDrvSetIntLockUnlock(CPSS_OS_INTR_MODE_LOCK_E, &(key))

#define INT_UNLOCK(key)                                                 \
    cmdExtDrvSetIntLockUnlock(CPSS_OS_INTR_MODE_UNLOCK_E, &(key));      \
    cmdOsTaskUnLock()


/*******************************************************************************
* init_txPacketDescTbl
*
* DESCRIPTION:
*       dynamic memory allocation for txPacketDescTbl table
*
* INPUTS:
*       None
*
* OUTPUTS:
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on error
*
* COMMENTS:
*
*
*******************************************************************************/
static GT_STATUS init_txPacketDescTbl(void)
{
    if(txPacketDescTbl == NULL)
    {
            txPacketDescTbl = (CPSS_TX_PKT_DESC*)
                cmdOsMalloc(sizeof(*txPacketDescTbl) * MAX_NUM_OF_PCAKETS_TO_SEND);
            txTblCapacity = MAX_NUM_OF_PCAKETS_TO_SEND;

            cmdOsMemSet(txPacketDescTbl, 0, sizeof(*txPacketDescTbl) * txTblCapacity);
    }
    if(txPacketDescTbl != NULL)
    {
        return GT_OK;
        }
    else
        {
        return GT_FAIL;
        }
}


#if defined EXMXPM_FAMILY
    /*******************************************************************************
    * prvWrCpssExMxPmSpecificTxPktInfoClear
    *
    * DESCRIPTION:
    *       clear specific tx pkt info(set all fields zero)
    *
    * INPUTS:
    *       pktInfoPtr (pointer to) pkt specific info
    *
    * OUTPUTS:
    *
    *
    * RETURNS:
    *       GT_OK      - on success
    *       GT_BAD_PTR - on bad ptr
    *
    * COMMENTS:
    *
    *
    *******************************************************************************/
    static GT_STATUS prvWrCpssExMxPmSpecificTxPktInfoClear
    (
        CPSS_EXMXPM_SPECIFIC_TX_PKT_INFO_STC * pktInfoPtr
    )
    {
        if (pktInfoPtr == NULL)
        {
            return GT_BAD_PTR;
        }
        pktInfoPtr->vpt = 0;
        pktInfoPtr->cfiBit = 0;
        pktInfoPtr->egrFilterEn = GT_FALSE;
        pktInfoPtr->egrFilterRegistered = GT_FALSE;
        pktInfoPtr->srcId = 0;
        pktInfoPtr->isDit = GT_FALSE;
        pktInfoPtr->ditPtr = 0;

        return GT_OK;
    }

    #if 0
        /*******************************************************************************
        * prvWrCpssExMxPmSpecificTxPktInfoTblClear
        *
        * DESCRIPTION:
        *       clear the specific tx pkt info table (set all fields zero)
        *
        * INPUTS:
        *       pktInfoTblPtr (pointer to) pkt specific info  tbl
        *       soze          table size
        *
        * OUTPUTS:
        *
        *
        * RETURNS:
        *       GT_OK      - on success
        *       GT_BAD_PTR - on bad ptr
        *
        * COMMENTS:
        *
        *
        *******************************************************************************/

        static GT_STATUS prvWrCpssExMxPmSpecificTxPktInfoTblClear
        (
            IN CPSS_EXMXPM_SPECIFIC_TX_PKT_INFO_STC * pktInfoTblPtr,
            IN GT_U32 size
        )
        {
            GT_STATUS rc;
            GT_U32 i;
            CPSS_EXMXPM_SPECIFIC_TX_PKT_INFO_STC * pktInfoPtr;

            if (pktInfoTblPtr == NULL)
            {
                return GT_BAD_PTR;
            }

            pktInfoPtr = pktInfoTblPtr;
            for (i = 0 ; i < size ; i++)
            {
                rc = prvWrCpssExMxPmSpecificTxPktInfoClear(pktInfoPtr);
                if (GT_OK != rc)
                {
                    return rc;
                }
                pktInfoPtr++;
            }
            return GT_OK;
        }
    #endif
#endif

#if defined CHX_FAMILY
    #include <cpss/dxCh/dxChxGen/networkIf/cpssDxChNetIfTypes.h>
    #include <cpss/dxCh/dxChxGen/networkIf/cpssDxChNetIf.h>
    #include <cpss/dxCh/dxChxGen/networkIf/cpssDxChNetIfMii.h>

    /*******************************************************************************
    * dxChSendPacket
    *
    * DESCRIPTION:
    *       Function for transmitting to Cheetah devices.
    *
    * INPUTS:
    *       None
    *
    * OUTPUTS:
    *       index - Entry index to send
    *
    * RETURNS:
    *       GT_OK   - on success
    *       GT_FAIL - on error
    *
    * COMMENTS:
    *
    *
    *******************************************************************************/
    static GT_STATUS dxChSendPacket
    (
        GT_U32 index
    )
    {
        GT_U32 j;
        GT_U8  devNum;
        CPSS_DXCH_NET_TX_PARAMS_STC cpssPcktParams;
        GT_STATUS   status = GT_OK;

        /* check txPacketDescTbl table init */
        if(txPacketDescTbl == NULL)
        {
            status = init_txPacketDescTbl();
                if(status != GT_OK)
                {
                    cmdOsPrintf("txPacketDescTbl init failed\n");
                    return GT_FAIL;
                }
            }

        cmdOsMemSet(&cpssPcktParams, 0, sizeof(cpssPcktParams));
        cpssPcktParams.packetIsTagged      = txPacketDescTbl[index].packetIsTagged;
        cpssPcktParams.cookie              = &txCookie;
        cpssPcktParams.sdmaInfo.evReqHndl  = evReqHndl;
        cpssPcktParams.sdmaInfo.recalcCrc  = txPacketDescTbl[index].recalcCrc;
        cpssPcktParams.sdmaInfo.txQueue    = txPacketDescTbl[index].txQueue;
        cpssPcktParams.sdmaInfo.invokeTxBufferQueueEvent = txPacketDescTbl[index].invokeTxBufferQueueEvent;


        cpssPcktParams.dsaParam.commonParams.dsaTagType = CPSS_DXCH_NET_DSA_TYPE_EXTENDED_E;
        cpssPcktParams.dsaParam.commonParams.vpt    = 0;
        cpssPcktParams.dsaParam.commonParams.cfiBit = 0;
        cpssPcktParams.dsaParam.commonParams.vid    = txPacketDescTbl[index].vid;

        /* This function treats only from CPU DSA type. */
        cpssPcktParams.dsaParam.dsaType = CPSS_DXCH_NET_DSA_CMD_FROM_CPU_E;

        cpssPcktParams.dsaParam.dsaInfo.fromCpu.tc          = txPacketDescTbl[index].trafficClass;
        cpssPcktParams.dsaParam.dsaInfo.fromCpu.dp          = txPacketDescTbl[index].dropPrecedence;
        cpssPcktParams.dsaParam.dsaInfo.fromCpu.egrFilterEn = GT_FALSE;
        cpssPcktParams.dsaParam.dsaInfo.fromCpu.cascadeControl = GT_FALSE;
        cpssPcktParams.dsaParam.dsaInfo.fromCpu.egrFilterRegistered = GT_FALSE;
        cpssPcktParams.dsaParam.dsaInfo.fromCpu.srcId          = 0;
        cpssPcktParams.dsaParam.dsaInfo.fromCpu.srcHwDev         = 0;
        cpssPcktParams.dsaParam.dsaInfo.fromCpu.dstInterface = txPacketDescTbl[index].dstInterface;

        if( (txPacketDescTbl[index].dstInterface.type == CPSS_INTERFACE_VIDX_E) ||
            (txPacketDescTbl[index].dstInterface.type == CPSS_INTERFACE_VID_E)    )
        {
            cpssPcktParams.dsaParam.dsaInfo.fromCpu.extDestInfo.multiDest.excludeInterface =
                txPacketDescTbl[index].excludeInterface;
            cpssPcktParams.dsaParam.dsaInfo.fromCpu.extDestInfo.multiDest.excludedInterface =
                txPacketDescTbl[index].excludedInterface;
        }
        else/* CPSS_INTERFACE_PORT_E */
        {
            cpssPcktParams.dsaParam.dsaInfo.fromCpu.extDestInfo.devPort.dstIsTagged =
                txPacketDescTbl[index].dstIsTagged;
        }


        devNum = txPacketDescTbl[index].devNum;
        for (j = 0; j < txPacketDescTbl[index].numPacketsToSend; j++)
        {/*For packet sent by this descriptor*/
            /* If the transmition was disabled */

            if (flagStopTransmit == GT_TRUE)
            {
                break;
            }

            if(cmdIsCpuEtherPortUsed() == GT_TRUE)
            {
                /* add support for native mii */
                CPSS_TBD_BOOKMARK
                return GT_NOT_SUPPORTED;
            }
            else
            {
                if(txPacketDescTbl[index].txSyncMode == GT_TRUE)
                 {
                     status = cpssDxChNetIfSdmaSyncTxPacketSend(devNum, &cpssPcktParams, txPacketDescTbl[index].pcktData,
                                                    txPacketDescTbl[index].pcktDataLen, txPacketDescTbl[index].numOfBuffers);
                 }
                 else
                 {
                     status = cpssDxChNetIfSdmaTxPacketSend(devNum, &cpssPcktParams, txPacketDescTbl[index].pcktData,
                                                    txPacketDescTbl[index].pcktDataLen, txPacketDescTbl[index].numOfBuffers);

                     if(status == GT_OK)
                         sdmaTxPacketSendCount++;
                 }
            }

            if((status != GT_OK) && (status != GT_NO_RESOURCE))
            {
                return status;
            }

            if (status == GT_OK)
            {
                /* in case transmit succeed */
                txPacketDescTbl[index].numSentPackets++;
            }

            if (status == GT_NO_RESOURCE)
            {
                /* packet wasn't transmitted */
                j--;
            }

            /* if we get GT_NO_RESOURCE and the sdmaTxPacketSendCount
               is bigger then PRV_MAX_NUM_OF_BUFFERS_AT_POOL
               it means no more buffer available in the FIFO */
            if ((cmdIsCpuEtherPortUsed() == GT_TRUE) ||
                (status == GT_NO_RESOURCE)    ||
                ((txPacketDescTbl[index].txSyncMode==GT_FALSE) && (txPacketDescTbl[index].invokeTxBufferQueueEvent==GT_TRUE))
               )
            {
                GT_U8           devNum;
                GT_U8           queueIdx;
                GT_PTR          cookie;

                /* wait for the Tx-End event */
                status = cmdCpssEventSelect(evReqHndl,NULL,NULL,0);

                if (status != GT_OK)
                {
                    return status;
                }

                /* get all Tx end events for the packet */

                if(cmdIsCpuEtherPortUsed() == GT_TRUE)
                {
                    /* add support for native mii */
                    CPSS_TBD_BOOKMARK
                    return GT_NOT_SUPPORTED;
                }
                else
                {
                    /* Even though we don't do anything with the returned value,
                       we need  to call cpssExMxPmNetIfSdmaTxBufferQueueInfoGet
                       to free the TX-queue-pool resources created for this task.
                     */
                    while(1)
                    {
                        status = cpssDxChNetIfTxBufferQueueGet(evReqHndl,&devNum,
                                                               &cookie,&queueIdx,&status);
                        if (status == GT_FAIL)
                        {
                            return status;
                        }
                        if(status == GT_NO_MORE)
                        {
                            break;
                        }
                        sdmaTxPacketGetCount++;
                    }

                    if(sdmaTxPacketSendCount != sdmaTxPacketGetCount)
                    {
                        cmdOsPrintf("Number of sent packets NOT equal to number of packets get \n\n");
                        cmdOsPrintf("Total sent packets: %d, sdmaTxPacketSendCount: %d, sdmaTxPacketGetCount: %d\n",j,sdmaTxPacketSendCount,sdmaTxPacketGetCount);
                    }

                    sdmaTxPacketSendCount = 0;
                    sdmaTxPacketGetCount = 0;

                }
            }

            /* wait n milliseconds before sending next packet */
            if(txPacketDescTbl[index].gap != 0)
            {
                cmdOsTimerWkAfter(txPacketDescTbl[index].gap);
            }

        }

        return GT_OK;
    }
#else
    static GT_STATUS dxChSendPacket
    (
        GT_U32 index
    )
    {
        return GT_OK;
    }
#endif


#if defined EX_FAMILY
    #include <cpss/exMx/exMxGen/networkIf/cpssExMxGenNetIfTypes.h>
    #include <cpss/exMx/exMxGen/networkIf/cpssExMxNetIf.h>
    #include <cpss/exMx/exMxGen/trunk/cpssExMxTrunk.h>
    /*******************************************************************************
    * exMxSendPacket
    *
    * DESCRIPTION:
    *       Function for transmitting to Tiger devices.
    *
    * INPUTS:
    *       None
    *
    * OUTPUTS:
    *       index - Entry index to send
    *
    * RETURNS:
    *       GT_OK   - on success
    *       GT_FAIL - on error
    *
    * COMMENTS:
    *
    *
    *******************************************************************************/
    static GT_STATUS exMxSendPacket
    (
        GT_U32 index
    )
    {
        GT_U32 j;
        GT_U8  devNum;
        CPSS_EXMX_NET_TX_PARAMS_STC cpssPcktParams;
        GT_TRUNK_ID                 trunkId;
        GT_U32                      numMembersPtr;
        CPSS_TRUNK_MEMBER_STC       membersArray[CPSS_TRUNK_MAX_NUM_OF_MEMBERS_CNS];
        GT_HW_DEV_NUM               tgtHwDev;
        GT_U32                      tgtPort;

        GT_STATUS   status = GT_OK;

            /* check txPacketDescTbl table init */
            if(txPacketDescTbl == NULL)
            {
                status = init_txPacketDescTbl();
                if(status != GT_OK)
                {
                    cmdOsPrintf("txPacketDescTbl init failed\n");
                    return GT_FAIL;
                }
            }

        cmdOsMemSet(&cpssPcktParams, 0, sizeof(cpssPcktParams));
        cpssPcktParams.txParams.cookie          = &txCookie;
        cpssPcktParams.txParams.dropPrecedence  = txPacketDescTbl[index].dropPrecedence;
        cpssPcktParams.txParams.evReqHndl       = evReqHndl;

        if (txPacketDescTbl[index].dstInterface.type == CPSS_INTERFACE_VIDX_E)
        {
            cpssPcktParams.useVidx      = GT_TRUE;
            cpssPcktParams.txParams.vid = txPacketDescTbl[index].dstInterface.vlanId;
            cpssPcktParams.dest.vidx    = txPacketDescTbl[index].dstInterface.vidx;

            /* dsaInfo is relevant only when addDsa = GT_TRUE and
               useVidx = GT_FALSE. if cmdType == TX_BY_VIDX then
               useVidx = GT_TRUE ==> we set addDsa = GT_FALSE */
            cpssPcktParams.addDsa = GT_FALSE;
        }
        else{
            cpssPcktParams.useVidx = GT_FALSE;
            cpssPcktParams.txParams.vid = txPacketDescTbl[index].dstInterface.vlanId;

            if(txPacketDescTbl[index].dstInterface.type == CPSS_INTERFACE_TRUNK_E)
            {

                trunkId = txPacketDescTbl[index].dstInterface.trunkId;
                devNum  = txPacketDescTbl[index].devNum;

                status = cpssExMxTrunkTableEntryGet(devNum, trunkId, &numMembersPtr, membersArray);
                if(status != GT_OK)
                {
                    return status;
                }

                if(numMembersPtr==0)
                {
                    return GT_FAIL;/* just like in empty vlan --- packet was not sent */
                }

                /* The packet should be send throw any of the trunk members */
                tgtHwDev  = membersArray[0].hwDevice;
                tgtPort = membersArray[0].port;
            }
            else /* port */
            {
                tgtHwDev  = txPacketDescTbl[index].dstInterface.devPort.hwDevNum;
                tgtPort = txPacketDescTbl[index].dstInterface.devPort.portNum;
            }

            cpssPcktParams.dest.devPort.tgtDev = (GT_U8)tgtHwDev;
            cpssPcktParams.dest.devPort.tgtPort = (GT_U8)tgtPort;

            cpssPcktParams.addDsa = GT_FALSE; /* addDsa ia always FALSE */
        }

        if ((*txPacketDescTbl[index].pcktData[0] == 0xFF) &&
            (*txPacketDescTbl[index].pcktData[1] == 0xFF) &&
            (*txPacketDescTbl[index].pcktData[2] == 0xFF) &&
            (*txPacketDescTbl[index].pcktData[3] == 0xFF) &&
            (*txPacketDescTbl[index].pcktData[4] == 0xFF) &&
            (*txPacketDescTbl[index].pcktData[5] == 0xFF))
        {
            cpssPcktParams.txParams.macDaType = CPSS_EXMX_NET_BROADCAST_MAC_E;
        }
        else if (*txPacketDescTbl[index].pcktData[0] == 0x01)
        {
            cpssPcktParams.txParams.macDaType = CPSS_EXMX_NET_MULTICAST_MAC_E;
        }
        else
        {
            cpssPcktParams.txParams.macDaType = CPSS_EXMX_NET_UNICAST_MAC_E;
        }

        cpssPcktParams.txParams.packetEncap = CPSS_EXMX_NET_REGULAR_PCKT_E;
        cpssPcktParams.txParams.recalcCrc   = txPacketDescTbl[index].recalcCrc;
        cpssPcktParams.txParams.tagged      = txPacketDescTbl[index].packetIsTagged;
        cpssPcktParams.txParams.txQueue     = txPacketDescTbl[index].txQueue;
        cpssPcktParams.txParams.userPrioTag = 0;
        cpssPcktParams.invokeTxBufferQueueEvent = txPacketDescTbl[index].invokeTxBufferQueueEvent;

        devNum = txPacketDescTbl[index].devNum;
        for (j = 0; j < txPacketDescTbl[index].numPacketsToSend; j++)
        {/*For packet sent by this descriptor*/
            /* If the transmition was disabled */

            if (flagStopTransmit == GT_TRUE)
            {
                break;
            }

            if(txPacketDescTbl[index].txSyncMode == GT_TRUE)
             {
                 status = cpssExMxNetIfSyncTxPacketSend(devNum, &cpssPcktParams, txPacketDescTbl[index].pcktData,
                                                txPacketDescTbl[index].pcktDataLen, txPacketDescTbl[index].numOfBuffers);
             }
             else
             {
                 status = cpssExMxNetIfTxPacketSend(devNum, &cpssPcktParams, txPacketDescTbl[index].pcktData,
                                                txPacketDescTbl[index].pcktDataLen, txPacketDescTbl[index].numOfBuffers);

                if(status == GT_OK)
                    sdmaTxPacketSendCount++;
             }

            if((status != GT_OK) && (status != GT_NO_RESOURCE))
            {
                return status;
            }

            if (status == GT_OK)
            {
                /* in case transmit succeed */
                txPacketDescTbl[index].numSentPackets++;
            }

            if (status == GT_NO_RESOURCE)
            {
                /* packet wasn't transmitted */
                j--;
            }

            /* if we get GT_NO_RESOURCE and the sdmaTxPacketSendCount
               is bigger then PRV_MAX_NUM_OF_BUFFERS_AT_POOL
               it means no more buffer available in the FIFO */
            if ((cmdIsCpuEtherPortUsed() == GT_TRUE) ||
                (status == GT_NO_RESOURCE)    ||
                ((txPacketDescTbl[index].txSyncMode==GT_FALSE) && (txPacketDescTbl[index].invokeTxBufferQueueEvent==GT_TRUE))
               )
            {
                GT_U8           devNum;
                GT_U8           queueIdx;
                GT_PTR          cookie;

                /* wait for the Tx-End event */
                status = cmdCpssEventSelect(evReqHndl,NULL,NULL,0);

                if (status != GT_OK)
                {
                    return status;
                }

                /* get all Tx end events for the packet */


                    /* Even though we don't do anything with the returned value,
                       we need  to call cpssExMxPmNetIfSdmaTxBufferQueueInfoGet
                       to free the TX-queue-pool resources created for this task.
                     */
                    while(1)
                    {
                        status = cpssExMxNetIfTxBufferQueueGet(evReqHndl,&devNum,
                                                               &cookie,&queueIdx,&status);
                        if (status == GT_FAIL)
                        {
                            return status;
                        }
                        if(status == GT_NO_MORE)
                        {
                            break;
                        }
                        sdmaTxPacketGetCount++;
                    }

                    if(sdmaTxPacketSendCount != sdmaTxPacketGetCount)
                    {
                        cmdOsPrintf("Number of sent packets NOT equal to number of packets get \n\n");
                        cmdOsPrintf("Total sent packets: %d, sdmaTxPacketSendCount: %d, sdmaTxPacketGetCount: %d\n",j,sdmaTxPacketSendCount,sdmaTxPacketGetCount);
                    }

                    sdmaTxPacketSendCount = 0;
                    sdmaTxPacketGetCount = 0;

            }

            /* wait n milliseconds before sending next packet */
            if(txPacketDescTbl[index].gap != 0)
            {
                cmdOsTimerWkAfter(txPacketDescTbl[index].gap);
            }

        }

        return GT_OK;
    }
#else
    static GT_STATUS exMxSendPacket
    (
        GT_U32 index
    )
    {
        return GT_OK;
    }
#endif


#if defined EXMXPM_FAMILY
    #include <cpss/exMxPm/exMxPmGen/networkIf/cpssExMxPmNetIfTypes.h>
    #include <cpss/exMxPm/exMxPmGen/networkIf/cpssExMxPmNetIf.h>

    /*******************************************************************************
    * exMxPmSendPacket
    *
    * DESCRIPTION:
    *       Function for transmitting to Puma devices.
    *
    * INPUTS:
    *       None
    *
    * OUTPUTS:
    *       txPacketDescTbl - table to be used
    *       index           - Entry index to send
    *
    * RETURNS:
    *       GT_OK   - on success
    *       GT_FAIL - on error
    *
    * COMMENTS:
    *
    *
    *******************************************************************************/
    static GT_STATUS exMxPmSendPacket
    (
        GT_U32 index
    )
    {
        GT_U32 j;
        GT_U8  devNum;
        CPSS_EXMXPM_NET_TX_PARAMS_STC cpssPcktParams;
        GT_STATUS   status = GT_OK;

        /* check txPacketDescTbl table init */
        if(txPacketDescTbl == NULL)
        {
                status = init_txPacketDescTbl();
            if(status != GT_OK)
            {
                cmdOsPrintf("txPacketDescTbl init failed\n");
                return GT_FAIL;
            }
        }


        cmdOsMemSet(&cpssPcktParams, 0, sizeof(cpssPcktParams));
        cpssPcktParams.packetIsTagged      = txPacketDescTbl[index].packetIsTagged;
        cpssPcktParams.cookie              = &txCookie;
        cpssPcktParams.sdmaInfo.evReqHndl  = evReqHndl;
        cpssPcktParams.sdmaInfo.recalcCrc  = txPacketDescTbl[index].recalcCrc;
        cpssPcktParams.sdmaInfo.txQueue    = txPacketDescTbl[index].txQueue;
        cpssPcktParams.sdmaInfo.invokeTxBufferQueueEvent = txPacketDescTbl[index].invokeTxBufferQueueEvent;


        cpssPcktParams.dsaParam.commonParams.dsaTagType = CPSS_EXMXPM_NET_DSA_TYPE_EXTENDED_E;

        cpssPcktParams.dsaParam.commonParams.vpt    = ((CPSS_EXMXPM_SPECIFIC_TX_PKT_INFO_STC *)(txPacketDescTbl[index].specificDeviceTypeInfo))->vpt;
        cpssPcktParams.dsaParam.commonParams.cfiBit = ((CPSS_EXMXPM_SPECIFIC_TX_PKT_INFO_STC *)(txPacketDescTbl[index].specificDeviceTypeInfo))->cfiBit;
        cpssPcktParams.dsaParam.commonParams.vid    = txPacketDescTbl[index].vid;

        switch(txPacketDescTbl[index].dsaTagCmd)
        {
            case PRV_CPSS_NET_DSA_CMD_FROM_CPU_E:
                cpssPcktParams.dsaParam.dsaCmd                      = CPSS_EXMXPM_NET_DSA_CMD_FROM_CPU_E;
                cpssPcktParams.dsaParam.dsaInfo.fromCpu.tc          = txPacketDescTbl[index].trafficClass;
                cpssPcktParams.dsaParam.dsaInfo.fromCpu.dp          = txPacketDescTbl[index].dropPrecedence;
                cpssPcktParams.dsaParam.dsaInfo.fromCpu.egrFilterEn = ((CPSS_EXMXPM_SPECIFIC_TX_PKT_INFO_STC *)(txPacketDescTbl[index].specificDeviceTypeInfo))->egrFilterEn;
                cpssPcktParams.dsaParam.dsaInfo.fromCpu.packetType  = CPSS_EXMXPM_TS_PASSENGER_PACKET_TYPE_ETHERNET_E;
                cpssPcktParams.dsaParam.dsaInfo.fromCpu.egrFilterRegistered = ((CPSS_EXMXPM_SPECIFIC_TX_PKT_INFO_STC *)(txPacketDescTbl[index].specificDeviceTypeInfo))->egrFilterRegistered;
                cpssPcktParams.dsaParam.dsaInfo.fromCpu.srcId               = ((CPSS_EXMXPM_SPECIFIC_TX_PKT_INFO_STC *)(txPacketDescTbl[index].specificDeviceTypeInfo))->srcId;
                cpssPcktParams.dsaParam.dsaInfo.fromCpu.dstInterface        = txPacketDescTbl[index].dstInterface;
                cpssPcktParams.dsaParam.dsaInfo.fromCpu.dstIsTagged         = txPacketDescTbl[index].dstIsTagged;
                cpssPcktParams.dsaParam.dsaInfo.fromCpu.isDit               = ((CPSS_EXMXPM_SPECIFIC_TX_PKT_INFO_STC *)(txPacketDescTbl[index].specificDeviceTypeInfo))->isDit;
                cpssPcktParams.dsaParam.dsaInfo.fromCpu.ditPtr              = ((CPSS_EXMXPM_SPECIFIC_TX_PKT_INFO_STC *)(txPacketDescTbl[index].specificDeviceTypeInfo))->ditPtr;


                if( (txPacketDescTbl[index].dstInterface.type == CPSS_INTERFACE_VIDX_E) ||
                    (txPacketDescTbl[index].dstInterface.type == CPSS_INTERFACE_VID_E)    )
                {
                    cpssPcktParams.dsaParam.dsaInfo.fromCpu.extDestInfo.multiDest.excludeInterface =
                        txPacketDescTbl[index].excludeInterface;
                    cpssPcktParams.dsaParam.dsaInfo.fromCpu.extDestInfo.multiDest.excludedInterface =
                        txPacketDescTbl[index].excludedInterface;
                }
                break;
            case PRV_CPSS_NET_DSA_CMD_FORWARD_E:
                cpssPcktParams.dsaParam.dsaCmd                      = CPSS_EXMXPM_NET_DSA_CMD_FORWARD_E;
                cpssPcktParams.dsaParam.dsaInfo.forward.srcIsTagged = txPacketDescTbl[index].packetIsTagged;
                cpssPcktParams.dsaParam.dsaInfo.forward.srcDev      = txPacketDescTbl[index].srcDevice;
                cpssPcktParams.dsaParam.dsaInfo.forward.tc          = txPacketDescTbl[index].trafficClass;
                cpssPcktParams.dsaParam.dsaInfo.forward.srcIsTrunk  = txPacketDescTbl[index].srcIsTrunk;
                if(cpssPcktParams.dsaParam.dsaInfo.forward.srcIsTrunk == GT_TRUE)
                {
                    cpssPcktParams.dsaParam.dsaInfo.forward.source.trunkId = txPacketDescTbl[index].source.trunkId;
                }
                else
                {
                    cpssPcktParams.dsaParam.dsaInfo.forward.source.portNum = txPacketDescTbl[index].source.portNum;
                }
                cpssPcktParams.dsaParam.dsaInfo.forward.egrFilterRegistered = ((CPSS_EXMXPM_SPECIFIC_TX_PKT_INFO_STC *)(txPacketDescTbl[index].specificDeviceTypeInfo))->egrFilterRegistered;
                cpssPcktParams.dsaParam.dsaInfo.forward.srcId        = ((CPSS_EXMXPM_SPECIFIC_TX_PKT_INFO_STC *)(txPacketDescTbl[index].specificDeviceTypeInfo))->srcId;
                cpssPcktParams.dsaParam.dsaInfo.forward.wasRouted    = txPacketDescTbl[index].wasRouted;
                cpssPcktParams.dsaParam.dsaInfo.forward.dstInterface.type
                                                                     = txPacketDescTbl[index].dstInterface.type;
                cpssPcktParams.dsaParam.dsaInfo.forward.dstInterface = txPacketDescTbl[index].dstInterface;
                break;
            default:
                cmdOsPrintf("ERROR-DSA command not valid\n\n");
                return GT_BAD_PARAM;
        }

        devNum = txPacketDescTbl[index].devNum;
        for (j = 0; j < txPacketDescTbl[index].numPacketsToSend; j++)
        {/*For packet sent by this descriptor*/
            /* If the transmition was disabled */

            if (flagStopTransmit == GT_TRUE)
            {
                break;
            }

            if(txPacketDescTbl[index].txSyncMode == GT_TRUE)
            {
                status = cpssExMxPmNetIfSdmaTxSyncPacketSend(devNum, &cpssPcktParams, txPacketDescTbl[index].pcktData,
                                                             txPacketDescTbl[index].pcktDataLen, txPacketDescTbl[index].numOfBuffers);
            }
            else
            {
                status = cpssExMxPmNetIfSdmaTxPacketSend(devNum, &cpssPcktParams, txPacketDescTbl[index].pcktData,
                                                         txPacketDescTbl[index].pcktDataLen, txPacketDescTbl[index].numOfBuffers);

                if(status == GT_OK)
                    sdmaTxPacketSendCount++;
            }

            if((status != GT_OK) && (status != GT_NO_RESOURCE))
            {
                return status;
            }

            if (status == GT_OK)
            {
                /* in case transmit succeed */
                txPacketDescTbl[index].numSentPackets++;
            }

            if (status == GT_NO_RESOURCE)
            {
                /* packet wasn't transmitted */
                j--;
            }

            /* if we get GT_NO_RESOURCE and the sdmaTxPacketSendCount
               is bigger then PRV_MAX_NUM_OF_BUFFERS_AT_POOL
               it means no more buffer available in the FIFO */
            if ((cmdIsCpuEtherPortUsed() == GT_TRUE) ||
                (status == GT_NO_RESOURCE)    ||
                ((txPacketDescTbl[index].txSyncMode==GT_FALSE) && (txPacketDescTbl[index].invokeTxBufferQueueEvent==GT_TRUE))
               )
            {
                GT_U8           devNum;
                GT_U8           queueIdx;
                GT_PTR          cookie;

                /* wait for the Tx-End event */
                status = cmdCpssEventSelect(evReqHndl,NULL,NULL,0);

                if (status != GT_OK)
                {
                    return status;
                }

                /* get all Tx end events for the packet */

                /* Even though we don't do anything with the returned value,
                   we need  to call cpssExMxPmNetIfSdmaTxBufferQueueInfoGet
                   to free the TX-queue-pool resources created for this task.
                 */
                while(1)
                {
                    status = cpssExMxPmNetIfSdmaTxBufferQueueInfoGet(evReqHndl,&devNum,
                                                                     &cookie,&queueIdx,&status);
                    if (status == GT_FAIL)
                    {
                        return status;
                    }
                    if(status == GT_NO_MORE)
                    {
                        break;
                    }
                    sdmaTxPacketGetCount++;
                }

                if(sdmaTxPacketSendCount != sdmaTxPacketGetCount)
                {
                    cmdOsPrintf("Number of sent packets NOT equal to number of packets get \n\n");
                    cmdOsPrintf("Total sent packets: %d, sdmaTxPacketSendCount: %d, sdmaTxPacketGetCount: %d\n",j,sdmaTxPacketSendCount,sdmaTxPacketGetCount);
                }

                sdmaTxPacketSendCount = 0;
                sdmaTxPacketGetCount = 0;

            }

            /* wait n milliseconds before sending next packet */
            if(txPacketDescTbl[index].gap != 0)
            {
                cmdOsTimerWkAfter(txPacketDescTbl[index].gap);
            }

        }

        return GT_OK;
    }
#else
    static GT_STATUS exMxPmSendPacket
    (
        GT_U32 index
    )
    {
        return GT_OK;
    }
#endif

/*******************************************************************************
* setBufferArraysOfEqualSize
*
* DESCRIPTION:
*       build GT_BYTE_ARRY from string expanding or trunkating to size
*
* INPUTS:
*       sourceDataPtr       - byte array buffer (hexadecimal string)
*       totalSize           - exact total size of the buffer in bytes
*       bufferSize          - wanted size of each data buffer
*
* OUTPUTS:
*       pcktData    - array of byte array data
*       pcktDataLen - array of byte array size
*       numOfBuffs  - number of buffers the data was splited to.
*
* RETURNS:
*       None
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS setBufferArraysOfEqualSize
(
    IN GT_U8   *sourceDataPtr,
    IN GT_U32   totalSize,
    IN GT_U32   bufferSize,
    OUT GT_U8   *pcktData[PRV_MAX_NUM_OF_SDMA_BUFFERS_PER_CHAIN],
    OUT GT_U32  pcktDataLen[PRV_MAX_NUM_OF_SDMA_BUFFERS_PER_CHAIN],
    OUT GT_U32  *numOfBuffs

)
{
    GT_U32 element;
    GT_U32 len = totalSize;
    GT_U32 numOfBuffers = len / bufferSize;
    GT_U32 sizeOfLastBuff = len % bufferSize;
    GT_U32 i;

    numOfBuffers = (sizeOfLastBuff > 0) ? numOfBuffers + 1 : numOfBuffers;

    if (numOfBuffers > PRV_MAX_NUM_OF_SDMA_BUFFERS_PER_CHAIN)
        return GT_NO_RESOURCE;

    if(cmdOsPoolGetBufFreeCnt(genTxBuffersPoolId) < numOfBuffers)
        return GT_NO_RESOURCE;

    /* if the length of the data is bigger than the length given by the user,
       the data is truncate otherwise it is expanded */

    for(i = 0; i < numOfBuffers ; i++)
    {
        if(len < bufferSize)
            bufferSize = len;
        else
            len = len - bufferSize;

        /* Since update can be done, then check that buffer is already allocated.*/
        if(pcktData[i] == NULL)
            pcktData[i] = cmdOsPoolGetBuf(genTxBuffersPoolId);

        /*for every buffer, we will copy the data*/
        for (element = 0; element < bufferSize; element++)
        {
            pcktData[i][element] = (int)(cmdOsStrChr(hexcode, cmdOsToUpper(*sourceDataPtr++)) - (GT_CHAR*)hexcode)<< 4;

            pcktData[i][element]+= (int)(cmdOsStrChr(hexcode, cmdOsToUpper(*sourceDataPtr++)) - (GT_CHAR*)hexcode);
        }

        pcktDataLen[i] = bufferSize;

    }

    *numOfBuffs = numOfBuffers;

    return GT_OK;
}

/*******************************************************************************
* freeBufferArraysOfEqualSize
*
* DESCRIPTION:
*       Free GT_BYTE_ARRY from string
*
* INPUTS:
*       pcktData    - array of byte array data
*       pcktDataLen - array of byte array size
*
* OUTPUTS:
*       None
*
* RETURNS:
*       None
*
* COMMENTS:
*
* Toolkit:
*
*******************************************************************************/
static GT_VOID freeBufferArraysOfEqualSize
(
    IN GT_U8   *pcktData[PRV_MAX_NUM_OF_SDMA_BUFFERS_PER_CHAIN],
    IN GT_U32  pcktDataLen[PRV_MAX_NUM_OF_SDMA_BUFFERS_PER_CHAIN]
)
{

    GT_U32 i;
    for (i=0; i<PRV_MAX_NUM_OF_SDMA_BUFFERS_PER_CHAIN; i++)
    {

        /*nothing to free */
        if((pcktData[i] == NULL) || (pcktDataLen[i] == 0))
            break;

         cmdOsPoolFreeBuf(genTxBuffersPoolId, pcktData[i]);
         pcktData[i] = NULL;
   }
}


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
    GT_BOOL     transmitted;


    /* check txPacketDescTbl table init */
    if(txPacketDescTbl == NULL)
    {
        status = init_txPacketDescTbl();
        if(status != GT_OK)
        {
            cmdOsPrintf("txPacketDescTbl init failed\n");
            return GT_FAIL;
        }
    }

    if (evReqHndl == 0)
    {
        CPSS_UNI_EV_CAUSE_ENT     evCause[1] = { CPSS_PP_TX_BUFFER_QUEUE_E };

        if (cmdCpssEventBind(evCause, 1, &evReqHndl) != GT_OK)
        {
            return GT_FAIL;
        }

        /* convert pss events to those of cpss */
        for(i = 0 ; i < 32 ; i++)
        {
            /*extern CMD_PP_CONFIG *ppConfigList;
            if(ppConfigList[i].valid == GT_FALSE)
            {
                continue;
            }
            */

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

    transmitted = GT_FALSE;
    cycles = 0;
    while (((cycles < modeSettings.cyclesNum) ||
            (modeSettings.cyclesNum == -1)) && (flagStopTransmit == GT_FALSE))
    {

        for(i = 0; i < MAX_NUM_OF_PCAKETS_TO_SEND; i++) /*For each descriptor*/
        {
            /* If the transmition was disabled */
            if (flagStopTransmit == GT_TRUE)
            {
                return GT_OK;
            }

            if (GT_TRUE != txPacketDescTbl[i].valid)
            {
                /* entry is not valid */
                continue;
            }

            if(CPSS_IS_EXMXPM_FAMILY_MAC(PRV_CPSS_PP_MAC(txPacketDescTbl[i].devNum)->devFamily))
            {
                status = exMxPmSendPacket(i);
            }
            else if(CPSS_IS_DXCH_FAMILY_MAC(PRV_CPSS_PP_MAC(txPacketDescTbl[i].devNum)->devFamily))
            {
                status = dxChSendPacket(i);
            }
            else if(CPSS_IS_EXMX_FAMILY_MAC(PRV_CPSS_PP_MAC(txPacketDescTbl[i].devNum)->devFamily))
            {
                status = exMxSendPacket(i);
            }
            else
            {
                status = GT_FAIL;
            }

            if (status != GT_OK)
            {
                cmdOsPrintf("ERROR-in transmitting packets\n\n");
                return GT_FAIL;
            }

            transmitted = GT_TRUE;

            /* wait n milliseconds before moving to the next entry */
            if(txPacketDescTbl[i].waitTime != 0)
            {
                cmdOsTimerWkAfter(txPacketDescTbl[i].waitTime);
            }
        }

        /* No Packets to send */
        if(transmitted == GT_FALSE)
            break;

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
* genPacketTransminitionTask
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
GT_STATUS __TASKCONV genPacketTransminitionTask()
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
* txGetValidDescIndex
*   return the index of the descriptor that include the entryId if found.
*   If not found return
*
* DESCRIPTION:
*
*
* INPUTS:
*       devNum    - device number
*       entryId   - Entry ID of packet descriptor to return.
*
* OUTPUTS:
*       indexPtr  - The index of the valid entry in the table.
*
* RETURNS:
*       GT_OK        - on success
*       GT_NOT_FOUND - if entry not found
*
* COMMENTS:
*
* GalTis:
*       Table - cmdTxPacketDescs
*
*******************************************************************************/
static GT_STATUS txGetValidDescIndex
(
    IN  GT_U8             devNum,
    IN  GT_BOOL           checkDevNum,
    IN  GT_U32            entryId,
    OUT GT_U32            *indexPtr
)
{
    GT_U32  entryIndex;
    GT_STATUS status;

    /* check txPacketDescTbl table init */
    if(txPacketDescTbl == NULL)
    {
        status = init_txPacketDescTbl();
        if(status != GT_OK)
        {
            cmdOsPrintf("txPacketDescTbl init failed\n");
            return GT_FAIL;
        }
    }

    for(entryIndex = 0; entryIndex < MAX_NUM_OF_PCAKETS_TO_SEND; entryIndex++)
    {
        if((GT_TRUE == txPacketDescTbl[entryIndex].valid) &&
           (txPacketDescTbl[entryIndex].entryId == entryId))
        {
            if((devNum != txPacketDescTbl[entryIndex].devNum) &&
               (checkDevNum == GT_TRUE))
            {
                continue;
            }

            /* we found a free space */
            *indexPtr = entryIndex;
            break;
        }
    }

    if(entryIndex ==  MAX_NUM_OF_PCAKETS_TO_SEND)
        return GT_NOT_FOUND;

    return GT_OK;
}

/*******************************************************************************
* txGetValidOrEmptyDescIndex
*   return the index of the descriptor that include the entryId or non valid
*   entry if not found.
*
* DESCRIPTION:
*
*
* INPUTS:
*       entryId   - Entry ID of packet descriptor to return.
*
* OUTPUTS:
*       indexPtr  - The index of the valid entry in the table.
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
static GT_STATUS txGetValidOrEmptyDescIndex
(
    IN  GT_U8             devNum,
    IN  GT_BOOL           checkDevNum,
    IN  GT_U32            entryId,
    OUT GT_U32            *indexPtr
)
{
    GT_U32  entryIndex;
    GT_STATUS status;

    /* check txPacketDescTbl table init */
    if(txPacketDescTbl == NULL)
    {
        status = init_txPacketDescTbl();
        if(status != GT_OK)
        {
            cmdOsPrintf("txPacketDescTbl init failed\n");
            return GT_FAIL;
        }
    }


    if( GT_OK == txGetValidDescIndex(devNum, checkDevNum, entryId, indexPtr))
        return GT_OK;

    /* Search for empty place */
    for(entryIndex = 0; entryIndex < MAX_NUM_OF_PCAKETS_TO_SEND; entryIndex++)
    {
        if (GT_FALSE == txPacketDescTbl[entryIndex].valid)
        {
            /* we found a free space */
            *indexPtr = entryIndex;
            break;
        }
    }

    if(entryIndex ==  MAX_NUM_OF_PCAKETS_TO_SEND)
        return GT_NO_RESOURCE;

    return GT_OK;
}

#ifdef SHARED_MEMORY
static CPSS_MP_REMOTE_COMMAND remoteCmd;
#endif

/*******************************************************************************
* wrCpssGenTxStart
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
*       Command     - cmdCpssGenTxStart
*
*******************************************************************************/
static CMD_STATUS wrCpssGenTxStart
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    static GT_BOOL   enterOnce = GT_FALSE;
    GT_STATUS status;

    /* check txPacketDescTbl table init */
    if(txPacketDescTbl == NULL)
    {
        status = init_txPacketDescTbl();
        if(status != GT_OK)
        {
            cmdOsPrintf("txPacketDescTbl init failed\n");
            return GT_FAIL;
        }
    }

    /* check if there are element in the transmit table */
    if(txTblCapacity == 0)
    {
        return GT_OK;
    }

#if defined SHARED_MEMORY && defined EXMXPM_FAMILY
    /* if compiled with SHARED_MAMORY defined
        and if variable multiProcessAppDemo == 1 the multiprocess
        appDemo will be run */

    if(multiProcessAppDemo != 0)
    {
        remoteCmd.remote = CPSS_MP_REMOTE_RXTX_E;
        remoteCmd.command = CPSS_MP_CMD_RXTX_START_TRANSMIT_E;

        /* Fill TX parameters for RxTxProcess packet transmission */
        remoteCmd.data.rxTxStartTransmit.cpssExMxPmTxPacketDescTbl = txPacketDescTbl;
        remoteCmd.data.rxTxStartTransmit.modeSettings.cyclesNum = modeSettings.cyclesNum;
        remoteCmd.data.rxTxStartTransmit.modeSettings.gap = modeSettings.gap;
        remoteCmd.data.rxTxStartTransmit.txExMxPmTxTblCapacity = txTblCapacity;

        return cpssMultiProcComExecute(&remoteCmd);
    }

#endif
    /* creat the task only once */
    if(enterOnce == GT_FALSE)
    {
        if(cmdOsSigSemBinCreate("txCmd",CPSS_OS_SEMB_EMPTY_E,&txCmdSemId) != GT_OK)
        {
            return CMD_AGENT_ERROR;
        }
        if(cmdOsTaskCreate("GalPktTx",                           /* Task Name                      */
                        GT_GALTIS_TRANSMITE_TASK_PRIOR_CNS,   /* Task Priority                  */
                        0x1000,                               /* Stack Size _4KB                */
                        (unsigned (__TASKCONV *)(void*))genPacketTransminitionTask, /* Starting Point */
                        (GT_VOID*)NULL,                       /* there is no arguments */
                        &taskId) != GT_OK)                    /* returned task ID */
        {
            return CMD_AGENT_ERROR;
        }

        enterOnce = GT_TRUE;
    }

    /* check if the last transition is done */
    if(taskBusyFlag == GT_TRUE)
    {
        return CMD_AGENT_ERROR;
    }

    flagStopTransmit = GT_FALSE;

    /* send a signal for the task to start the transmission */
    cmdOsSigSemSignal(txCmdSemId);

    return  CMD_OK;
}

/*******************************************************************************
* wrCpssGenTxStop
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
*       Command     - cmdCpssGenTxStop
*
*******************************************************************************/
static GT_STATUS wrCpssGenTxStop
(
    void
)
{
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
    return GT_OK;
}

/*******************************************************************************
* wrCpssGenTxStop_
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
*       Command     - cmdCpssGenTxStop
*
*******************************************************************************/
static CMD_STATUS wrCpssGenTxStop_
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    wrCpssGenTxStop();
    galtisOutput(outArgs, GT_OK, "");
    return CMD_OK;
}

/*******************************************************************************
* wrCpssGenTxSetMode
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
static CMD_STATUS wrCpssGenTxSetMode
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    /* map input arguments to locals */
    modeSettings.cyclesNum = (GT_U32)inArgs[0];
    modeSettings.gap = (GT_U32)inArgs[1];

    return CMD_OK;
}


/*******************************************************************************
* wrCpssGenTxGetMode
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
*       Command - cmdCpssGenTxGetMode
*
*******************************************************************************/
static CMD_STATUS wrCpssGenTxGetMode
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, GT_OK, "%d%d", modeSettings.cyclesNum, modeSettings.gap);

    return CMD_OK;
}


/*******************************************************************************
* wrCpssGenTxGetTxBufferQueueEventHandle
*
* DESCRIPTION:
*       This command will get the the tx buffer queue event handle.
*
* INPUTS:
*       None
*
* OUTPUTS:
*       evReqHndl - tx buffer queue event  handle
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on error
*
* COMMENTS:
*
* GalTis:
*
*
*******************************************************************************/
static CMD_STATUS wrCpssGenTxGetTxBufferQueueEventHandle
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, GT_OK, "%d" ,evReqHndl);
    return CMD_OK;
}


/*******************************************************************************
* wrCpssGenTxBufferQueueEventHandling
*
* DESCRIPTION:
*       This command bind tx buffer queue event, get tx buffer queue event
*       handle and unmask corresponding interrupt.
*
* INPUTS:
*       None
*
* OUTPUTS:
*       evReqHndl - tx buffer queue event  handle
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on error
*
* COMMENTS:
*
* GalTis:
*
*
*******************************************************************************/
static CMD_STATUS wrCpssGenTxBufferQueueEventHandling
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
     CPSS_UNI_EV_CAUSE_ENT     evCause[1] = { CPSS_PP_TX_BUFFER_QUEUE_E };
     GT_STATUS status = GT_OK;
    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    if (evReqHndl == 0)
    {
        status = cmdCpssEventBind(evCause, 1, &evReqHndl);
        if ( status!= GT_OK)
        {
            galtisOutput(outArgs, status, "");
            return CMD_OK;
        }
            /* unmask this event */
            status = cmdCpssEventDeviceMaskSet(0, CPSS_PP_TX_BUFFER_QUEUE_E,CPSS_EVENT_UNMASK_E);
    }

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, status, "%d" ,evReqHndl);
    return CMD_OK;
}

/*******************************************************************************
* wrTxSystemReset
*
* DESCRIPTION:
*       Preparation for system reset
*
* INPUTS:
*       None
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK        - on success
*       GT_FAIL      - on error
*       GT_HW_ERROR  - on hardware error
*       GT_BAD_PARAM - wrong interface
*       GT_BAD_PTR   - on NULL pointer
*
* COMMENTS:
*
*******************************************************************************/
static GT_VOID wrTxSystemReset
(
    GT_VOID
)
{
    GT_32     intKey;          /* The interrupt lock key       */

    if(genTxBuffersPoolId)
    {
        cmdOsPoolDeletePool(genTxBuffersPoolId);
        genTxBuffersPoolId = 0;
    }

    if(txPacketDescTbl)
    {
        cmdOsFree(txPacketDescTbl);
        txPacketDescTbl = 0;
    }

    txTblCapacity = 0;

    poolCreated = GT_FALSE;


    INT_LOCK(intKey);
    cpssEventDestroy(evReqHndl);
    evReqHndl = 0;
    INT_UNLOCK(intKey);
}
/*
* Table: TxNetIf
*
* Description:
*     Transmited packet descriptor table.
*
* Comments:
*/
/*******************************************************************************
* cpssGenNetIfTxFromCpuSet (table command)
*
* DESCRIPTION:
*     Creates new transmit parameters description entry in the table.
*
* INPUTS:
*     [0] GT_U32 lIndex -
*          Entry ID of packet descriptor.
*
*     [1] DXCH_TX_CMD_ENT lCmdType -
*          Defines type of transmition (VIDX, PORT).
*
*
* OUTPUTS:
*     none
*
* RETURNS:
*     CMD_OK            - on success.
*     CMD_AGENT_ERROR   - on failure.
*     CMD_FIELD_UNDERFLOW - not enough field arguments.
*     CMD_FIELD_OVERFLOW  - too many field arguments.
*
* COMMENTS:
*     none
*
*******************************************************************************/
static CMD_STATUS wrCpssGenNetIfTxFromCpuEntrySet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS status;
    GT_U32    entryIndex=0, entryId;
    GT_32     sizes[2] = {22, 18};
#if defined EXMXPM_FAMILY
    GT_32     i=0;
#endif

    /* check for valid arguments */
    if(inArgs == NULL || outArgs == NULL)
        return CMD_AGENT_ERROR;

    if(numFields < sizes[inArgs[1]])
        return CMD_FIELD_UNDERFLOW;

        /* check txPacketDescTbl table init */
    if(txPacketDescTbl == NULL)
    {
        status = init_txPacketDescTbl();
        if(status != GT_OK)
        {
            cmdOsPrintf("txPacketDescTbl init failed\n");
            return GT_FAIL;
        }
    }


    /* create the pool only once */
    if(poolCreated == GT_FALSE)
    {
        /* register function to reset DB so after system reset we can send traffic again */
        wrCpssRegisterResetCb(wrTxSystemReset);
        /* create pool of buffers from Cache */
        status = cmdOsPoolCreateDmaPool(
                                  PRV_TX_BUFFER_SIZE + 64 /* ALIGN_ADDR_CHUNK */,
                                  GT_4_BYTE_ALIGNMENT,
                                  PRV_MAX_NUM_OF_BUFFERS_AT_POOL,
                                  GT_TRUE,
                                  &genTxBuffersPoolId);

        if (status != GT_OK)
        {
                cmdOsPrintf("ERROR-PoolCreateDmaPool\n\n");
                return status;
        }

        if(txPacketDescTbl == NULL)
        {
            txPacketDescTbl = (CPSS_TX_PKT_DESC*)
                cmdOsMalloc(sizeof(*txPacketDescTbl) * MAX_NUM_OF_PCAKETS_TO_SEND);
            txTblCapacity = MAX_NUM_OF_PCAKETS_TO_SEND;

            cmdOsMemSet(txPacketDescTbl, 0, sizeof(*txPacketDescTbl) * txTblCapacity);
        }

        /* if the device is puma, we support setting extra parameters to the packet
           description table. When we create the pool we reset the specificDeviceTypeInfo*/
        #if defined EXMXPM_FAMILY
            cmdOsMemSet(specificDeviceTypeInfo, 0, sizeof(specificDeviceTypeInfo));
        #endif

        poolCreated = GT_TRUE;
    }

    /* When calling wrCpssGenNetIfTxFromCpuEntrySet all specificDeviceTypeInfo
       fields will be 0. In order to update the value of those fields
       wrCpssExMxPm2NetIfFromCpuExtFieldsEntry should be called.
       Here we bind the predefined specificDeviceTypeInfo structure to the
       sw shadow - txPacketDescTbl */
    #if defined EXMXPM_FAMILY
        for(i=0; i<MAX_NUM_OF_PCAKETS_TO_SEND; i++)
        {
            if(txPacketDescTbl[i].specificDeviceTypeInfo==NULL)
                txPacketDescTbl[i].specificDeviceTypeInfo = &specificDeviceTypeInfo[i];
        }
    #endif

    /* Get entry in the table */
    entryId = (GT_U32)inFields[0];
    status = txGetValidOrEmptyDescIndex((GT_U8)inArgs[0], GT_TRUE, entryId, &entryIndex);
    if( status != GT_OK)
    {
        galtisOutput(outArgs, GT_NO_RESOURCE, "");
        return CMD_OK;
    }


    /*Non zero data len */
    if ((GT_U32)inFields[13] == 0)
    {
        galtisOutput(outArgs, GT_BAD_PARAM, "");
        return CMD_OK;
    }

    txPacketDescTbl[entryIndex].dsaTagCmd = PRV_CPSS_NET_DSA_CMD_FROM_CPU_E;
    txPacketDescTbl[entryIndex].entryId = entryId;
    txPacketDescTbl[entryIndex].devNum = (GT_U8)inArgs[0];
    txPacketDescTbl[entryIndex].packetIsTagged = (GT_BOOL)inFields[1];
    txPacketDescTbl[entryIndex].vid = (GT_U16)inFields[2];
    txPacketDescTbl[entryIndex].dropPrecedence = (CPSS_DP_LEVEL_ENT)inFields[3];
    txPacketDescTbl[entryIndex].trafficClass = (GT_U8)inFields[4];
    txPacketDescTbl[entryIndex].recalcCrc = (GT_BOOL)inFields[5];;
    txPacketDescTbl[entryIndex].txSyncMode = (GT_BOOL)inFields[6];
    txPacketDescTbl[entryIndex].txQueue = (GT_U8)inFields[7];;
    txPacketDescTbl[entryIndex].invokeTxBufferQueueEvent = inFields[8];
    txPacketDescTbl[entryIndex].numPacketsToSend = (GT_U32)inFields[9];
    txPacketDescTbl[entryIndex].gap = (GT_U32)inFields[10];
    txPacketDescTbl[entryIndex].waitTime = (GT_U32)inFields[11];

    switch(inArgs[1])
    {

        case 0: /* TX to MULTI DESTINATION */
            txPacketDescTbl[entryIndex].dstInterface.type =
                (CPSS_INTERFACE_TYPE_ENT)inFields[15];

            if(txPacketDescTbl[entryIndex].dstInterface.type == CPSS_INTERFACE_VID_E)
                txPacketDescTbl[entryIndex].dstInterface.vlanId = (GT_U16)inFields[16];
            else
                txPacketDescTbl[entryIndex].dstInterface.vidx = (GT_U16)inFields[16];

            txPacketDescTbl[entryIndex].excludeInterface = (GT_BOOL)inFields[17];
            /* type can be CPSS_INTERFACE_PORT_E = 0 or CPSS_INTERFACE_TRUNK_E = 1*/

            if(txPacketDescTbl[entryIndex].excludeInterface == GT_TRUE)
            {
                if(inFields[18] == 0) /*CPSS_INTERFACE_PORT_E*/
                {
                    txPacketDescTbl[entryIndex].excludedInterface.type = CPSS_INTERFACE_PORT_E;
                    txPacketDescTbl[entryIndex].excludedInterface.devPort.hwDevNum = (GT_U8)inFields[19];
                    txPacketDescTbl[entryIndex].excludedInterface.devPort.portNum = (GT_U8)inFields[20];

                    /* Override Device and Port */
                    CONVERT_DEV_PORT_DATA_MAC(txPacketDescTbl[entryIndex].excludedInterface.devPort.hwDevNum,
                                         txPacketDescTbl[entryIndex].excludedInterface.devPort.portNum);

                }
                else if (inFields[18] == 1)/*CPSS_INTERFACE_TRUNK_E*/
                {
                    txPacketDescTbl[entryIndex].excludedInterface.type = CPSS_INTERFACE_TRUNK_E;
                    txPacketDescTbl[entryIndex].excludedInterface.trunkId = (GT_U16)inFields[21];
                }
                else
                    status = GT_BAD_PARAM;
            }

            break;

        case 1: /* TX to SINGLE DESTINATION */
            txPacketDescTbl[entryIndex].dstInterface.type = CPSS_INTERFACE_PORT_E;
            txPacketDescTbl[entryIndex].dstInterface.devPort.hwDevNum = (GT_U8)inFields[15];
            txPacketDescTbl[entryIndex].dstInterface.devPort.portNum = (GT_U8)inFields[16];

            /* Override Device and Port */
            CONVERT_DEV_PORT_DATA_MAC(txPacketDescTbl[entryIndex].dstInterface.devPort.hwDevNum,
                                 txPacketDescTbl[entryIndex].dstInterface.devPort.portNum);

            txPacketDescTbl[entryIndex].dstIsTagged = (GT_BOOL)inFields[17];
            break;

        default:
            status = GT_BAD_PARAM;
            break;
    }

    if( status != GT_OK)
    {
        galtisOutput(outArgs, status, "");
        return CMD_OK;
    }

    /* Copy the packet data to DB. */
    status = setBufferArraysOfEqualSize((GT_U8*)inFields[12],
                               (GT_U32)inFields[13],
                               PRV_TX_BUFFER_SIZE,
                               txPacketDescTbl[entryIndex].pcktData,
                               txPacketDescTbl[entryIndex].pcktDataLen,
                               &txPacketDescTbl[entryIndex].numOfBuffers);


    if (status != GT_OK)
    {
        /* Still need to free, because of partial allocation. */
        freeBufferArraysOfEqualSize(txPacketDescTbl[entryIndex].pcktData,
                                    txPacketDescTbl[entryIndex].pcktDataLen);

        galtisOutput(outArgs, GT_NO_RESOURCE, "");
        return CMD_OK;
    }

    /* The entry is valid. */
    txPacketDescTbl[entryIndex].valid = GT_TRUE;

    galtisOutput(outArgs, GT_OK, "");

    return CMD_OK;
}

static GT_U32    netIfTblEntryId=0;
/*******************************************************************************
* netGetTxNetIfTbl (table command)
*
* DESCRIPTION:
*     Retruns specific packet descriptor entry from the table.
*
* INPUTS:
*     [0] GT_U32 lIndex -
*          Entry ID of packet descriptor.
*
*     [1] DXCH_TX_CMD_ENT lCmdType -
*          Defines type of transmition (VIDX, PORT).
*
*
* OUTPUTS:
*      Table fields as describe in the table description are written into
*      outArgs parameter.
*
* RETURNS:
*     CMD_OK            - on success.
*     CMD_AGENT_ERROR   - on failure.
*     CMD_FIELD_UNDERFLOW - not enough field arguments.
*     CMD_FIELD_OVERFLOW  - too many field arguments.
*
* COMMENTS:
*     none
*
*******************************************************************************/
static GT_STATUS getTxNetIfTblEntry
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_U32       index=0;
    GT_BYTE_ARRY lGtBuff;
    GT_STATUS    status = GT_OK;
    GT_HW_DEV_NUM  __HwDev; /* Dummy for converting. */
    GT_PORT_NUM    __Port; /* Dummy for converting. */


        /* check txPacketDescTbl table init */
    if(txPacketDescTbl == NULL)
    {
        status = init_txPacketDescTbl();
        if(status != GT_OK)
        {
            cmdOsPrintf("txPacketDescTbl init failed\n");
            return GT_FAIL;
        }
    }


    for( ; netIfTblEntryId < MAX_NUM_OF_PCAKETS_TO_SEND ; netIfTblEntryId++)
    {
        status = txGetValidDescIndex((GT_U8)inArgs[0], GT_TRUE, netIfTblEntryId, &index);

        if(status == GT_NOT_FOUND)
            continue;

        if(status == GT_OK)
        {
            netIfTblEntryId++;
            break;
        }
    }
    if(netIfTblEntryId == MAX_NUM_OF_PCAKETS_TO_SEND)
    {
        return GT_NOT_FOUND;
    }

    /* Mapping structure to fields */
    inFields[0 ] =  txPacketDescTbl[index].entryId;
    inFields[1 ] =  txPacketDescTbl[index].packetIsTagged;
    inFields[2 ] =  txPacketDescTbl[index].vid;
    inFields[3 ] =  txPacketDescTbl[index].dropPrecedence;
    inFields[4 ] =  txPacketDescTbl[index].trafficClass;
    inFields[5 ] =  txPacketDescTbl[index].recalcCrc;
    inFields[6 ] =  txPacketDescTbl[index].txSyncMode;
    inFields[7 ] =  txPacketDescTbl[index].txQueue;
    inFields[8 ] =  txPacketDescTbl[index].invokeTxBufferQueueEvent;
    inFields[9 ] =  txPacketDescTbl[index].numPacketsToSend;
    inFields[10] =  txPacketDescTbl[index].gap;
    inFields[11] =  txPacketDescTbl[index].waitTime;
    inFields[13] =  *(txPacketDescTbl[index].pcktDataLen);
    inFields[14] =  txPacketDescTbl[index].numSentPackets;

    switch( txPacketDescTbl[index].dstInterface.type)
    {
        case CPSS_INTERFACE_VID_E:
        case CPSS_INTERFACE_VIDX_E:

            inFields[15] =  txPacketDescTbl[index].dstInterface.type;
            if(txPacketDescTbl[index].dstInterface.type == CPSS_INTERFACE_VID_E)
                inFields[16] =  txPacketDescTbl[index].dstInterface.vlanId;
            else
                inFields[16] =  txPacketDescTbl[index].dstInterface.vidx;

            inFields[17] =  txPacketDescTbl[index].excludeInterface;
            inFields[18] =  txPacketDescTbl[index].excludedInterface.type;
            if(inFields[18] == 0) /*CPSS_INTERFACE_PORT_E*/
            {
                __HwDev  = txPacketDescTbl[index].excludedInterface.devPort.hwDevNum;
                __Port = txPacketDescTbl[index].excludedInterface.devPort.portNum;

                CONVERT_BACK_DEV_PORT_DATA_MAC(__HwDev, __Port) ;
                inFields[19] = __HwDev  ;
                inFields[20] = __Port ;
                inFields[21] = 0;
            }
            else/*CPSS_INTERFACE_TRUNK_E*/
            {
                inFields[19] = 0;
                inFields[20] = 0;
                inFields[21] = txPacketDescTbl[index].excludedInterface.trunkId;
            }

            break;

    case CPSS_INTERFACE_PORT_E:
            __HwDev  =  txPacketDescTbl[index].dstInterface.devPort.hwDevNum;
            __Port =  txPacketDescTbl[index].dstInterface.devPort.portNum;

            CONVERT_BACK_DEV_PORT_DATA_MAC(__HwDev, __Port);
            inFields[15] = __HwDev  ;
            inFields[16] = __Port ;

            inFields[17] =  (GT_32)txPacketDescTbl[index].dstIsTagged;

            break;

        default:
            break;
    }


    /* Send to Galtis only the first buffer of the packet */
    lGtBuff.data = txPacketDescTbl[index].pcktData[0];
    lGtBuff.length =  txPacketDescTbl[index].pcktDataLen[0];

    switch( txPacketDescTbl[index].dstInterface.type)
    {
        case CPSS_INTERFACE_VID_E:
        case CPSS_INTERFACE_VIDX_E:

            /* pack and output table fields */
            fieldOutput("%d%d%d%d%d%d%d%d%d%d%d%d%s%d%d%d%d%d%d%d%d%d",
                        inFields[0], inFields[1], inFields[2], inFields[3],
                        inFields[4], inFields[5], inFields[6], inFields[7],
                        inFields[8], inFields[9], inFields[10], inFields[11],
                        galtisBArrayOut(&lGtBuff),inFields[13], inFields[14],
                        inFields[15], inFields[16], inFields[17],inFields[18],
                        inFields[19], inFields[20], inFields[21] );
            galtisOutput(outArgs, status, "%d%f",  0);
            break;

        case CPSS_INTERFACE_PORT_E:
            fieldOutput("%d%d%d%d%d%d%d%d%d%d%d%d%s%d%d%d%d%d",
                        inFields[0], inFields[1], inFields[2], inFields[3],
                        inFields[4], inFields[5], inFields[6], inFields[7],
                        inFields[8], inFields[9], inFields[10], inFields[11],
                        galtisBArrayOut(&lGtBuff),inFields[13], inFields[14],
                        inFields[15], inFields[16], inFields[17]);
            galtisOutput(outArgs, status, "%d%f",  1);

            break;

        default:
            break;
    }

    return GT_OK;

}

/*******************************************************************************
* wrCpssGenNetIfTxFromCpuEntryGet (table command)
*
* DESCRIPTION:
*     Retruns specific packet descriptor entry from the table.
*
* INPUTS:
*     [0] GT_U32 lIndex -
*          Entry ID of packet descriptor.
*
*     [1] DXCH_TX_CMD_ENT lCmdType -
*          Defines type of transmition (VIDX, PORT).
*
*
* OUTPUTS:
*      Table fields as describe in the table description are written into
*      outArgs parameter.
*
* RETURNS:
*     CMD_OK            - on success.
*     CMD_AGENT_ERROR   - on failure.
*     CMD_FIELD_UNDERFLOW - not enough field arguments.
*     CMD_FIELD_OVERFLOW  - too many field arguments.
*
* COMMENTS:
*     none
*
*******************************************************************************/
static CMD_STATUS wrCpssGenNetIfTxFromCpuEntryGet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{

    /* check for valid arguments */
    if(inArgs == NULL || outArgs == NULL)
        return CMD_AGENT_ERROR;

    if(GT_NOT_FOUND == getTxNetIfTblEntry(inArgs ,inFields ,numFields ,outArgs))
    {
        galtisOutput(outArgs, GT_EMPTY, "");
    }

    return CMD_OK;
}

/*******************************************************************************
* wrCpssGenNetIfTxFromCpuEntryGetFirst (table command)
*
* DESCRIPTION:
*     Get first entry in the table.
*
* INPUTS:
*     [0] GT_U32 lIndex -
*          Entry ID of packet descriptor.
*
*     [1] DXCH_TX_CMD_ENT lCmdType -
*          Defines type of transmition (VIDX, PORT).
*
*
* OUTPUTS:
*      Table fields as describe in the table description are written into
*      outArgs parameter.
*
* RETURNS:
*     CMD_OK            - on success.
*     CMD_AGENT_ERROR   - on failure.
*     CMD_FIELD_UNDERFLOW - not enough field arguments.
*     CMD_FIELD_OVERFLOW  - too many field arguments.
*
* COMMENTS:
*     none
*
*******************************************************************************/
static CMD_STATUS wrCpssGenNetIfTxFromCpuEntryGetFirst
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS rc;
    /* check for valid arguments */
    if(inArgs == NULL || outArgs == NULL)
        return CMD_AGENT_ERROR;

    netIfTblEntryId = 0;

    rc = getTxNetIfTblEntry(inArgs, inFields, numFields, outArgs);
    if(rc == GT_NOT_FOUND)
    {
        galtisOutput(outArgs, GT_EMPTY, "%d", -1);
    }

    return CMD_OK;
}

/*******************************************************************************
* wrCpssGenNetIfTxFromCpuEntryGetNext (table command)
*
* DESCRIPTION:
*     Get next entry in the table.
*
* INPUTS:
*     [0] GT_U32 lIndex -
*          Entry ID of packet descriptor.
*
*     [1] DXCH_TX_CMD_ENT lCmdType -
*          Defines type of transmition (VIDX, PORT).
*
*
* OUTPUTS:
*      Table fields as describe in the table description are written into
*      outArgs parameter.
*
* RETURNS:
*     CMD_OK            - on success.
*     CMD_AGENT_ERROR   - on failure.
*     CMD_FIELD_UNDERFLOW - not enough field arguments.
*     CMD_FIELD_OVERFLOW  - too many field arguments.
*
* COMMENTS:
*     none
*
*******************************************************************************/
static CMD_STATUS wrCpssGenNetIfTxFromCpuEntryGetNext
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS rc;

    /* check for valid arguments */
    if(inArgs == NULL || outArgs == NULL)
        return CMD_AGENT_ERROR;

    rc = getTxNetIfTblEntry(inArgs, inFields, numFields, outArgs);
    if(rc == GT_NOT_FOUND)
    {
        galtisOutput(outArgs, GT_OK, "%d", -1);
    }

    return CMD_OK;
}

/*******************************************************************************
* wrCpssGenNetIfTxFromCpuEntryDelete (table command)
*
* DESCRIPTION:
*     Delete packet descriptor entry from the table.
*
* INPUTS:
*     [0] GT_U32 lIndex -
*          Entry ID of packet descriptor.
*
*     [1] DXCH_TX_CMD_ENT lCmdType -
*          Defines type of transmition (VIDX, PORT).
*
*
* OUTPUTS:
*     none
*
* RETURNS:
*     CMD_OK            - on success.
*     CMD_AGENT_ERROR   - on failure.
*     CMD_FIELD_UNDERFLOW - not enough field arguments.
*     CMD_FIELD_OVERFLOW  - too many field arguments.
*
* COMMENTS:
*     none
*
*******************************************************************************/
static CMD_STATUS wrCpssGenNetIfTxFromCpuEntryDelete
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS         status;
    GT_U32            j;
    CPSS_TX_PKT_DESC *txPcktDescPtr;
    GT_U32            entryIndex, entryId;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* check txPacketDescTbl table init */
    if(txPacketDescTbl == NULL)
    {
        status = init_txPacketDescTbl();
        if(status != GT_OK)
        {
            cmdOsPrintf("txPacketDescTbl init failed\n");
            return GT_FAIL;
        }
    }

    /* make sure there is no send */
    wrCpssGenTxStop();

    entryId = (GT_U32)inFields[0];

    /* Get the entry index */
    status =  txGetValidDescIndex((GT_U8)inArgs[0], GT_TRUE, entryId, &entryIndex);
    if(status != GT_OK)
        return status;


    txPcktDescPtr = &txPacketDescTbl[entryIndex];

    /* release tx buffers */
    for( j=0; j < PRV_MAX_NUM_OF_SDMA_BUFFERS_PER_CHAIN ; j++ )
    {
        if (0 == txPcktDescPtr->pcktDataLen[j])
        {
            break;
        }

        if(txPcktDescPtr->pcktData[j] != NULL)
        {

            cmdOsPoolFreeBuf(genTxBuffersPoolId, txPcktDescPtr->pcktData[j]);
        }
    }/*For all buffers for this descriptor*/

    /* clear the packet descriptor data */
    cmdOsMemSet(txPcktDescPtr, 0, sizeof(CPSS_TX_PKT_DESC));

    galtisOutput(outArgs, status, "");

    return CMD_OK;
}

/*******************************************************************************
* prvWrCpssGenNetIfTxFromCpuEntryClear
*
* DESCRIPTION:
*       Clear From CPU table
*
* INPUTS:
*       mallocIfNull - indication if need to allocate the table.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       None
*
* COMMENTS:
*
* Toolkit:
*
*******************************************************************************/
static GT_STATUS prvWrCpssGenNetIfTxFromCpuEntryClear
(
    IN GT_BOOL      mallocIfNull
)
{
    GT_U32    i,j;
    CPSS_TX_PKT_DESC *txPcktDescPtr;
    GT_STATUS status;

    /* check txPacketDescTbl table init */
    if(txPacketDescTbl == NULL)
    {
        if(mallocIfNull == GT_FALSE)
        {
            /* the table was not allocated ... we not need to allocate it */
            return GT_OK;
        }

        status = init_txPacketDescTbl();
        if(status != GT_OK)
        {
            cmdOsPrintf("txPacketDescTbl init failed\n");
            return GT_FAIL;
        }
    }

    /* make sure there is no send */
    wrCpssGenTxStop();

        /* release tx buffers */
    for(i = 0 ; i < MAX_NUM_OF_PCAKETS_TO_SEND ; i++)
    {
        if (GT_FALSE == txPacketDescTbl[i].valid)
        {
            /* entry is not valid */
            continue;
        }

        txPcktDescPtr = &txPacketDescTbl[i];

        for( j=0; j < PRV_MAX_NUM_OF_SDMA_BUFFERS_PER_CHAIN ; j++ )
        {
            if (0 == txPcktDescPtr->pcktDataLen[j])
            {
                break;
            }

            if (txPcktDescPtr->pcktData[j] != NULL)
                cmdOsPoolFreeBuf(genTxBuffersPoolId, txPcktDescPtr->pcktData[j]);

        }/*For all buffers for this descriptor*/

        /* clear the packet descriptor data */
        cmdOsMemSet(txPcktDescPtr, 0, sizeof(CPSS_TX_PKT_DESC));

    }/*For all descriptors*/

    return GT_OK;
}

/*******************************************************************************
* cpssDxChNetIfTxFromCpuClear (table command)
*
* DESCRIPTION:
*     Clear all packet descriptors entries from the table.
*
* INPUTS:
*     [0] GT_U32 lIndex -
*          Entry ID of packet descriptor.
*
*     [1] DXCH_TX_CMD_ENT lCmdType -
*          Defines type of transmition (VIDX, PORT).
*
*
* OUTPUTS:
*     none
*
* RETURNS:
*     CMD_OK            - on success.
*     CMD_AGENT_ERROR   - on failure.
*     CMD_FIELD_UNDERFLOW - not enough field arguments.
*     CMD_FIELD_OVERFLOW  - too many field arguments.
*
* COMMENTS:
*     none
*
*******************************************************************************/
static CMD_STATUS wrCpssGenNetIfTxFromCpuEntryClear
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS status;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;


    status = prvWrCpssGenNetIfTxFromCpuEntryClear(GT_TRUE);

    galtisOutput(outArgs, status, "");

    return CMD_OK;
}

/*****************************************************************************/

#if defined EXMXPM_FAMILY

    static GT_U32    netIfTblExtFieldsEntryId=0;

    /*******************************************************************************
    * getExMxPm2NetIfTblExtFieldsEntry (table command)
    *
    * DESCRIPTION:
    *     Retruns specific packet descriptor entry from the table.
    *
    * INPUTS:
    *     iIndex - Entry ID of packet descriptor.
    *
    * OUTPUTS:
    *      Table fields as describe in the table description are written into
    *      outArgs parameter.
    *
    * RETURNS:
    *     CMD_OK            - on success.
    *     CMD_AGENT_ERROR   - on failure.
    *     CMD_FIELD_UNDERFLOW - not enough field arguments.
    *     CMD_FIELD_OVERFLOW  - too many field arguments.
    *
    * COMMENTS:
    *     none
    *
    *******************************************************************************/
    static GT_STATUS getExMxPm2NetIfTblExtFieldsEntry
    (
        IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
        IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
        IN  GT_32 numFields,
        OUT GT_8  outArgs[CMD_MAX_BUFFER]
    )
    {
        GT_U32       entryIndex=0;
        GT_STATUS    status = GT_OK;
        GT_U8        devNum;
        CPSS_EXMXPM_SPECIFIC_TX_PKT_INFO_STC * pktInfoPtr;

            /* check txPacketDescTbl table init */
            if(txPacketDescTbl == NULL)
            {
                status = init_txPacketDescTbl();
                if(status != GT_OK)
                {
                    cmdOsPrintf("txPacketDescTbl init failed\n");
                    return GT_FAIL;
                }
            }

        devNum = (GT_U8)inArgs[0];
        for( ; netIfTblExtFieldsEntryId < MAX_NUM_OF_PCAKETS_TO_SEND ; netIfTblExtFieldsEntryId++)
        {
            status = txGetValidDescIndex(devNum, GT_TRUE, netIfTblExtFieldsEntryId, &entryIndex);

            if(status == GT_NOT_FOUND)
                continue;

            if(status == GT_OK)
            {
                netIfTblExtFieldsEntryId++;
                break;
            }
        }

        if(netIfTblExtFieldsEntryId == MAX_NUM_OF_PCAKETS_TO_SEND)
        {
            return GT_NOT_FOUND;
        }

        /* Mapping structure to fields */
        pktInfoPtr = (CPSS_EXMXPM_SPECIFIC_TX_PKT_INFO_STC *)(txPacketDescTbl[entryIndex].specificDeviceTypeInfo);

        inFields[0] =  txPacketDescTbl[entryIndex].entryId;
        inFields[1] =  pktInfoPtr->vpt;
        inFields[2] =  pktInfoPtr->cfiBit;
        inFields[3] =  pktInfoPtr->egrFilterEn;
        inFields[4] =  pktInfoPtr->egrFilterRegistered;
        inFields[5] =  pktInfoPtr->srcId;

        if (numFields > 6)
        {
            inFields[6] =  pktInfoPtr->isDit;
            inFields[7] =  pktInfoPtr->ditPtr;
        }

        if (numFields == 6)
        {
            fieldOutput("%d%d%d%d%d%d",
                        inFields[0], inFields[1], inFields[2], inFields[3],
                        inFields[4], inFields[5]);
            cpssOsPrintf("\n%d%d%d%d%d%d",
                        inFields[0], inFields[1], inFields[2], inFields[3],
                        inFields[4], inFields[5]);
        }
        else
        {
            fieldOutput("%d%d%d%d%d%d%d%d\n",
                        inFields[0], inFields[1], inFields[2], inFields[3],
                        inFields[4], inFields[5], inFields[6], inFields[7]);
            cpssOsPrintf("\n%d%d%d%d%d%d%d%d\n",
                        inFields[0], inFields[1], inFields[2], inFields[3],
                        inFields[4], inFields[5], inFields[6], inFields[7]);
        }
        galtisOutput(outArgs, status, "%f");

        return GT_OK;

    }

    /*******************************************************************************
    * cpssExMxPmNetIfFromCpuExtFieldsSet(table command)
    *
    * DESCRIPTION:
    *     Add extra parameters description to the entry created in the table in the
    *     call to cpssGenNetIfTxFromCpuSet.
    *
    * INPUTS:
    *     none
    *
    *
    * OUTPUTS:
    *     none
    *
    * RETURNS:
    *     CMD_OK            - on success.
    *     CMD_AGENT_ERROR   - on failure.
    *     CMD_FIELD_UNDERFLOW - not enough field arguments.
    *     CMD_FIELD_OVERFLOW  - too many field arguments.
    *
    * COMMENTS:
    *     none
    *
    *******************************************************************************/
    static CMD_STATUS wrCpssExMxPm2NetIfFromCpuExtFieldsEntrySet
    (
        IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
        IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
        IN  GT_32 numFields,
        OUT GT_8  outArgs[CMD_MAX_BUFFER]
    )
    {
        GT_STATUS status;
        GT_U32    entryIndex=0, entryId;
        CPSS_EXMXPM_SPECIFIC_TX_PKT_INFO_STC * pktInfoPtr;

        if(inArgs == NULL || outArgs == NULL)
        {
            return CMD_AGENT_ERROR;
        }

        /* check txPacketDescTbl table init */
            if(txPacketDescTbl == NULL)
            {
                status = init_txPacketDescTbl();
                if(status != GT_OK)
                {
                    cmdOsPrintf("txPacketDescTbl init failed\n");
                    return GT_FAIL;
                }
            }

        /* creat the pool only once in the call to wrCpssGenNetIfTxFromCpuEntrySet
           if the pool was not created it is not legal to update the extra fields */
        if(poolCreated == GT_FALSE)
        {

            cmdOsPrintf("ERROR-PoolCreateDmaPool was not created yet\n\n");
            return CMD_AGENT_ERROR;

        }

        /* Get entry in the table */
        entryId = (GT_U32)inFields[0];
        status = txGetValidOrEmptyDescIndex((GT_U8)inArgs[0], GT_TRUE, entryId, &entryIndex);
        if( status != GT_OK)
        {
            galtisOutput(outArgs, GT_NO_RESOURCE, "");
            return CMD_OK;
        }


        /* update the fields that were not set in the call to wrCpssGenNetIfTxFromCpuEntrySet*/
        pktInfoPtr = (CPSS_EXMXPM_SPECIFIC_TX_PKT_INFO_STC *)(txPacketDescTbl[entryIndex].specificDeviceTypeInfo);

        pktInfoPtr->vpt         = (GT_U8)inFields[1];
        pktInfoPtr->cfiBit      = (GT_U8)inFields[2];
        pktInfoPtr->egrFilterEn = (GT_BOOL)inFields[3];
        pktInfoPtr->egrFilterRegistered = (GT_BOOL)inFields[4];
        pktInfoPtr->srcId       = (GT_BOOL)inFields[5];
        pktInfoPtr->isDit       = GT_FALSE;
        pktInfoPtr->ditPtr      = 0;

        galtisOutput(outArgs, GT_OK, "");

        return CMD_OK;
    }

    /*******************************************************************************
    * wrCpssExMxPm2NetIfFromCpuExtFieldsEntryGet (table command)
    *
    * DESCRIPTION:
    *       Retruns specific packet parameters descriptor from the table.
    *
    * INPUTS:
    *       None
    *
    * OUTPUTS:
    *      Table fields as describe in the table description are written into
    *      outArgs parameter.
    *
    * RETURNS:
    *     CMD_OK            - on success.
    *     CMD_AGENT_ERROR   - on failure.
    *     CMD_FIELD_UNDERFLOW - not enough field arguments.
    *     CMD_FIELD_OVERFLOW  - too many field arguments.
    *
    * COMMENTS:
    *     none
    *
    *******************************************************************************/
    static CMD_STATUS wrCpssExMxPm2NetIfFromCpuExtFieldsEntryGet
    (
        IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
        IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
        IN  GT_32 numFields,
        OUT GT_8  outArgs[CMD_MAX_BUFFER]
    )
    {
        /* check for valid arguments */
        if(inArgs == NULL || outArgs == NULL)
        {
            return CMD_AGENT_ERROR;
        }

        if(GT_NOT_FOUND == getExMxPm2NetIfTblExtFieldsEntry(inArgs ,inFields ,numFields ,outArgs))
        {
            galtisOutput(outArgs, GT_EMPTY, "");
        }

        galtisOutput(outArgs, GT_NOT_SUPPORTED, "%d", -1);

        return CMD_OK;
    }

    /*******************************************************************************
    * wrCpssExMxPm2NetIfFromCpuExtFieldsEntryGetFirst (table command)
    *
    * DESCRIPTION:
    *     Get first entry specific parameters in the table.
    *
    * INPUTS:
    *       None
    *
    * OUTPUTS:
    *      Table fields as describe in the table description are written into
    *      outArgs parameter.
    *
    * RETURNS:
    *     CMD_OK            - on success.
    *     CMD_AGENT_ERROR   - on failure.
    *     CMD_FIELD_UNDERFLOW - not enough field arguments.
    *     CMD_FIELD_OVERFLOW  - too many field arguments.
    *
    * COMMENTS:
    *     none
    *
    *******************************************************************************/
    static CMD_STATUS wrCpssExMxPm2NetIfFromCpuExtFieldsEntryGetFirst
    (
        IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
        IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
        IN  GT_32 numFields,
        OUT GT_8  outArgs[CMD_MAX_BUFFER]
    )
    {
        GT_STATUS rc;
        /* check for valid arguments */
        if(inArgs == NULL || outArgs == NULL)
        {
            return CMD_AGENT_ERROR;
        }

        netIfTblExtFieldsEntryId = 0;

        rc = getExMxPm2NetIfTblExtFieldsEntry(inArgs, inFields, numFields, outArgs);
        if(rc == GT_NOT_FOUND)
        {
            galtisOutput(outArgs, GT_EMPTY, "%d", -1);
        }

        return CMD_OK;
    }

    /*******************************************************************************
    * wrCpssExMxPm2NetIfFromCpuExtFieldsEntryGetNext (table command)
    *
    * DESCRIPTION:
    *       Get next entry specific parameters in the table.
    *
    * INPUTS:
    *       None
    *
    * OUTPUTS:
    *      Table fields as describe in the table description are written into
    *      outArgs parameter.
    *
    * RETURNS:
    *     CMD_OK            - on success.
    *     CMD_AGENT_ERROR   - on failure.
    *     CMD_FIELD_UNDERFLOW - not enough field arguments.
    *     CMD_FIELD_OVERFLOW  - too many field arguments.
    *
    * COMMENTS:
    *     none
    *
    *******************************************************************************/
    static CMD_STATUS wrCpssExMxPm2NetIfFromCpuExtFieldsEntryGetNext
    (
        IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
        IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
        IN  GT_32 numFields,
        OUT GT_8  outArgs[CMD_MAX_BUFFER]
    )
    {
        GT_STATUS rc;

        /* check for valid arguments */
        if(inArgs == NULL || outArgs == NULL)
        {
            return CMD_AGENT_ERROR;
        }

        rc = getExMxPm2NetIfTblExtFieldsEntry(inArgs, inFields, numFields, outArgs);
        if(rc == GT_NOT_FOUND)
        {
            galtisOutput(outArgs, GT_OK, "%d", -1);
        }

        return CMD_OK;
    }

    /*******************************************************************************
    * wrCpssExMxPm2NetIfFromExtFieldsCpuClear (table command)
    *
    * DESCRIPTION:
    *     Clear all packet descriptors entries from the table.
    *
    * INPUTS:
    *     [0] GT_U32 lIndex -
    *          Entry ID of packet descriptor.
    *
    *     [1] DXCH_TX_CMD_ENT lCmdType -
    *          Defines type of transmition (VIDX, PORT).
    *
    *
    * OUTPUTS:
    *     none
    *
    * RETURNS:
    *     CMD_OK            - on success.
    *     CMD_AGENT_ERROR   - on failure.
    *     CMD_FIELD_UNDERFLOW - not enough field arguments.
    *     CMD_FIELD_OVERFLOW  - too many field arguments.
    *
    * COMMENTS:
    *     none
    *
    *******************************************************************************/
    static CMD_STATUS wrCpssExMxPm2NetIfFromCpuExtFieldsEntryClear
    (
        IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
        IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
        IN  GT_32 numFields,
        OUT GT_8  outArgs[CMD_MAX_BUFFER]
    )
    {
        GT_U32 i=0;
        GT_STATUS status;
        CPSS_EXMXPM_SPECIFIC_TX_PKT_INFO_STC * pktInfoPtr;

        /* check for valid arguments */
        if(inArgs == NULL || outArgs == NULL)
        {
            return CMD_AGENT_ERROR;
        }


            /* check txPacketDescTbl table init */
            if(txPacketDescTbl == NULL)
            {
                status = init_txPacketDescTbl();
                if(status != GT_OK)
                {
                    cmdOsPrintf("txPacketDescTbl init failed\n");
                    return GT_FAIL;
                }
            }

        /* make sure there is no send */
        wrCpssGenTxStop();

        for(i=0; i<MAX_NUM_OF_PCAKETS_TO_SEND; i++)
        {
            pktInfoPtr = (CPSS_EXMXPM_SPECIFIC_TX_PKT_INFO_STC *)(txPacketDescTbl[i].specificDeviceTypeInfo);
            /* if the device is puma, we support setting extra parameters to the packet description table*/
            status = prvWrCpssExMxPmSpecificTxPktInfoClear(pktInfoPtr);
            if (GT_OK != status)
            {
                    cmdOsPrintf("prvWrCpssExMxPmSpecificTxPktInfoClear() failed\n");
                return GT_FAIL;
            }

        }
        galtisOutput(outArgs, GT_OK, "");

        return CMD_OK;
    }



    /*******************************************************************************
    * wrCpssExMxPm2NetIfToDITExtFieldsEntrySet(table command)
    *
    * DESCRIPTION:
    *     Add extra parameters description to the entry created in the table in the
    *     call to cpssGenNetIfTxFromCpuSet.
    *
    * INPUTS:
    *     none
    *
    *
    * OUTPUTS:
    *     none
    *
    * RETURNS:
    *     CMD_OK            - on success.
    *     CMD_AGENT_ERROR   - on failure.
    *     CMD_FIELD_UNDERFLOW - not enough field arguments.
    *     CMD_FIELD_OVERFLOW  - too many field arguments.
    *
    * COMMENTS:
    *     none
    *
    *******************************************************************************/
    static CMD_STATUS wrCpssExMxPm2NetIfToDITExtFieldsEntrySet
    (
        IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
        IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
        IN  GT_32 numFields,
        OUT GT_8  outArgs[CMD_MAX_BUFFER]
    )
    {
        GT_STATUS status;
        GT_U32    entryIndex=0, entryId;
        CPSS_EXMXPM_SPECIFIC_TX_PKT_INFO_STC * pktInfoPtr;

        /* check txPacketDescTbl table init */
            if(txPacketDescTbl == NULL)
            {
                status = init_txPacketDescTbl();
                if(status != GT_OK)
                {
                    cmdOsPrintf("txPacketDescTbl init failed\n");
                    return GT_FAIL;
                }
            }

        /* check for valid arguments */
        if(inArgs == NULL || outArgs == NULL)
            return CMD_AGENT_ERROR;

        /* creat the pool only once in the call to wrCpssGenNetIfTxFromCpuEntrySet
           if the pool was not created it is not legal to update the extra fields */
        if(poolCreated == GT_FALSE)
        {

            cmdOsPrintf("ERROR-PoolCreateDmaPool was not created yet\n\n");
            return CMD_AGENT_ERROR;

        }

        /* Get entry in the table */
        entryId = (GT_U32)inFields[0];
        status = txGetValidOrEmptyDescIndex((GT_U8)inArgs[0], GT_TRUE, entryId, &entryIndex);
        if( status != GT_OK)
        {
            galtisOutput(outArgs, GT_NO_RESOURCE, "");
            return CMD_OK;
        }


        /* update the fields that were not set in the call to wrCpssGenNetIfTxFromCpuEntrySet*/
        pktInfoPtr = (CPSS_EXMXPM_SPECIFIC_TX_PKT_INFO_STC *)(txPacketDescTbl[entryIndex].specificDeviceTypeInfo);

        pktInfoPtr->vpt         = (GT_U8)inFields[1];
        pktInfoPtr->cfiBit      = (GT_U8)inFields[2];
        pktInfoPtr->egrFilterEn = (GT_BOOL)inFields[3];
        pktInfoPtr->egrFilterRegistered = (GT_BOOL)inFields[4];
        pktInfoPtr->srcId       = (GT_BOOL)inFields[5];
        pktInfoPtr->isDit       = (GT_BOOL)inFields[6];
        pktInfoPtr->ditPtr      = (GT_BOOL)inFields[7];

        galtisOutput(outArgs, GT_OK, "");

        return CMD_OK;
    }


    /*******************************************************************************
    * wrCpssExMxPm2NetIfToDITExtFieldsEntryGet (table command)
    *
    * DESCRIPTION:
    *       Retruns specific packet parameters descriptor from the table.
    *
    * INPUTS:
    *       None
    *
    * OUTPUTS:
    *      Table fields as describe in the table description are written into
    *      outArgs parameter.
    *
    * RETURNS:
    *     CMD_OK            - on success.
    *     CMD_AGENT_ERROR   - on failure.
    *     CMD_FIELD_UNDERFLOW - not enough field arguments.
    *     CMD_FIELD_OVERFLOW  - too many field arguments.
    *
    * COMMENTS:
    *     none
    *
    *******************************************************************************/
    static CMD_STATUS wrCpssExMxPm2NetIfToDITExtFieldsEntryGet
    (
        IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
        IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
        IN  GT_32 numFields,
        OUT GT_8  outArgs[CMD_MAX_BUFFER]
    )
    {
        /* check for valid arguments */
        if(inArgs == NULL || outArgs == NULL)
        {
            return CMD_AGENT_ERROR;
        }

        if(GT_NOT_FOUND == getExMxPm2NetIfTblExtFieldsEntry(inArgs ,inFields ,numFields ,outArgs))
        {
            galtisOutput(outArgs, GT_EMPTY, "");
        }

        galtisOutput(outArgs, GT_NOT_SUPPORTED, "%d", -1);

        return CMD_OK;
    }


    /*******************************************************************************
    * wrCpssExMxPm2NetIfToDITExtFieldsEntryGetFirst (table command)
    *
    * DESCRIPTION:
    *     Get first entry specific parameters in the table.
    *
    * INPUTS:
    *       None
    *
    * OUTPUTS:
    *      Table fields as describe in the table description are written into
    *      outArgs parameter.
    *
    * RETURNS:
    *     CMD_OK            - on success.
    *     CMD_AGENT_ERROR   - on failure.
    *     CMD_FIELD_UNDERFLOW - not enough field arguments.
    *     CMD_FIELD_OVERFLOW  - too many field arguments.
    *
    * COMMENTS:
    *     none
    *
    *******************************************************************************/
    static CMD_STATUS wrCpssExMxPm2NetIfToDITExtFieldsEntryGetFirst
    (
        IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
        IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
        IN  GT_32 numFields,
        OUT GT_8  outArgs[CMD_MAX_BUFFER]
    )
    {
        GT_STATUS rc;
        /* check for valid arguments */
        if(inArgs == NULL || outArgs == NULL)
        {
            return CMD_AGENT_ERROR;
        }

        netIfTblExtFieldsEntryId = 0;

        rc = getExMxPm2NetIfTblExtFieldsEntry(inArgs, inFields, numFields, outArgs);
        if(rc == GT_NOT_FOUND)
        {
            galtisOutput(outArgs, GT_EMPTY, "%d", -1);
        }

        return CMD_OK;
    }

    /*******************************************************************************
    * wrCpssExMxPm2NetIfToDITExtFieldsEntryGetNext (table command)
    *
    * DESCRIPTION:
    *       Get next entry specific parameters in the table.
    *
    * INPUTS:
    *       None
    *
    * OUTPUTS:
    *      Table fields as describe in the table description are written into
    *      outArgs parameter.
    *
    * RETURNS:
    *     CMD_OK            - on success.
    *     CMD_AGENT_ERROR   - on failure.
    *     CMD_FIELD_UNDERFLOW - not enough field arguments.
    *     CMD_FIELD_OVERFLOW  - too many field arguments.
    *
    * COMMENTS:
    *     none
    *
    *******************************************************************************/
    static CMD_STATUS wrCpssExMxPm2NetIfToDITExtFieldsEntryGetNext
    (
        IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
        IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
        IN  GT_32 numFields,
        OUT GT_8  outArgs[CMD_MAX_BUFFER]
    )
    {
        GT_STATUS rc;

        /* check for valid arguments */
        if(inArgs == NULL || outArgs == NULL)
        {
            return CMD_AGENT_ERROR;
        }

        rc = getExMxPm2NetIfTblExtFieldsEntry(inArgs, inFields, numFields, outArgs);
        if(rc == GT_NOT_FOUND)
        {
            galtisOutput(outArgs, GT_OK, "%d", -1);
        }

        return CMD_OK;
    }


    /*******************************************************************************
    * wrCpssExMxPm2NetIfToDITExtFieldsEntryClear (table command)
    *
    * DESCRIPTION:
    *     Clear all packet descriptors entries from the table.
    *
    * INPUTS:
    *     [0] GT_U32 lIndex -
    *          Entry ID of packet descriptor.
    *
    *     [1] DXCH_TX_CMD_ENT lCmdType -
    *          Defines type of transmition (VIDX, PORT).
    *
    *
    * OUTPUTS:
    *     none
    *
    * RETURNS:
    *     CMD_OK            - on success.
    *     CMD_AGENT_ERROR   - on failure.
    *     CMD_FIELD_UNDERFLOW - not enough field arguments.
    *     CMD_FIELD_OVERFLOW  - too many field arguments.
    *
    * COMMENTS:
    *     none
    *
    *******************************************************************************/
    static CMD_STATUS wrCpssExMxPm2NetIfToDITExtFieldsEntryClear
    (
        IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
        IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
        IN  GT_32 numFields,
        OUT GT_8  outArgs[CMD_MAX_BUFFER]
    )
    {
        GT_U32 i=0;
        GT_STATUS status;
        CPSS_EXMXPM_SPECIFIC_TX_PKT_INFO_STC * pktInfoPtr;

        /* check txPacketDescTbl table init */
            if(txPacketDescTbl == NULL)
            {
                status = init_txPacketDescTbl();
                if(status != GT_OK)
                {
                    cmdOsPrintf("txPacketDescTbl init failed\n");
                    return GT_FAIL;
                }
            }

        /* check for valid arguments */
        if(!inArgs || !outArgs)
        {
            return CMD_AGENT_ERROR;
        }


        /* make sure there is no send */
        wrCpssGenTxStop();


        for(i=0; i<MAX_NUM_OF_PCAKETS_TO_SEND; i++)
        {
            /* if the device is puma, we support setting extra parameters to the packet description table*/
            pktInfoPtr = (CPSS_EXMXPM_SPECIFIC_TX_PKT_INFO_STC *)(txPacketDescTbl[i].specificDeviceTypeInfo);
            status = prvWrCpssExMxPmSpecificTxPktInfoClear(pktInfoPtr);
            if (GT_OK != status)
            {
                    cmdOsPrintf("prvWrCpssExMxPmSpecificTxPktInfoClear() failed\n");
                return GT_FAIL;
            }
        }
        galtisOutput(outArgs, GT_OK, "");

        return CMD_OK;
    }
#endif

/******************************************************************************
 ***************************** RX to Tx Handling *****************************
 ******************************************************************************/

static GT_U32 rx2TxParamIndex = MAX_NUM_OF_PCAKETS_TO_SEND;
static GT_BOOL rx2TxEnable = GT_FALSE;
static GT_BOOL rx2TxSyncMode = GT_FALSE;
static GT_BOOL rx2TxInvokeIntBufQueue = GT_FALSE;

#if defined CHX_FAMILY
    CPSS_DXCH_NET_TX_PARAMS_STC cpssDxChPcktParams;

/*******************************************************************************
* setDxChRx2TxParams
*
*
*******************************************************************************/
static GT_STATUS setDxChRx2TxParams
(
    IN  GT_32 index
)
{
    GT_STATUS status;

        /* check txPacketDescTbl table init */
    if(txPacketDescTbl == NULL)
    {
        status = init_txPacketDescTbl();
        if(status != GT_OK)
        {
            cmdOsPrintf("txPacketDescTbl init failed\n");
            return GT_FAIL;
        }
    }

    cmdOsMemSet(&cpssDxChPcktParams, 0, sizeof(cpssDxChPcktParams));
    cpssDxChPcktParams.packetIsTagged      = txPacketDescTbl[index].packetIsTagged;
    cpssDxChPcktParams.cookie              = &txCookie;
    cpssDxChPcktParams.sdmaInfo.evReqHndl  = rx2TxEvReqHndl;
    cpssDxChPcktParams.sdmaInfo.recalcCrc  = txPacketDescTbl[index].recalcCrc;
    cpssDxChPcktParams.sdmaInfo.txQueue    = txPacketDescTbl[index].txQueue;
    cpssDxChPcktParams.sdmaInfo.invokeTxBufferQueueEvent = txPacketDescTbl[index].invokeTxBufferQueueEvent;


    cpssDxChPcktParams.dsaParam.commonParams.dsaTagType = CPSS_DXCH_NET_DSA_TYPE_EXTENDED_E;
    cpssDxChPcktParams.dsaParam.commonParams.vpt    = 0;
    cpssDxChPcktParams.dsaParam.commonParams.cfiBit = 0;
    cpssDxChPcktParams.dsaParam.commonParams.vid    = txPacketDescTbl[index].vid;

    /* This function treats only from CPU DSA type. */
    cpssDxChPcktParams.dsaParam.dsaType = CPSS_DXCH_NET_DSA_CMD_FROM_CPU_E;

    cpssDxChPcktParams.dsaParam.dsaInfo.fromCpu.tc          = txPacketDescTbl[index].trafficClass;
    cpssDxChPcktParams.dsaParam.dsaInfo.fromCpu.dp          = txPacketDescTbl[index].dropPrecedence;
    cpssDxChPcktParams.dsaParam.dsaInfo.fromCpu.egrFilterEn = GT_FALSE;
    cpssDxChPcktParams.dsaParam.dsaInfo.fromCpu.cascadeControl = GT_FALSE;
    cpssDxChPcktParams.dsaParam.dsaInfo.fromCpu.egrFilterRegistered = GT_FALSE;
    cpssDxChPcktParams.dsaParam.dsaInfo.fromCpu.srcId          = 0;

    cpssDxChPcktParams.dsaParam.dsaInfo.fromCpu.dstInterface = txPacketDescTbl[index].dstInterface;

    if( (txPacketDescTbl[index].dstInterface.type == CPSS_INTERFACE_VIDX_E) ||
        (txPacketDescTbl[index].dstInterface.type == CPSS_INTERFACE_VID_E)    )
    {
        cpssDxChPcktParams.dsaParam.dsaInfo.fromCpu.extDestInfo.multiDest.excludeInterface =
            txPacketDescTbl[index].excludeInterface;
        cpssDxChPcktParams.dsaParam.dsaInfo.fromCpu.extDestInfo.multiDest.excludedInterface =
            txPacketDescTbl[index].excludedInterface;
    }
    else/* CPSS_INTERFACE_PORT_E */
    {
        cpssDxChPcktParams.dsaParam.dsaInfo.fromCpu.extDestInfo.devPort.dstIsTagged =
            txPacketDescTbl[index].dstIsTagged;
    }

    return GT_OK;
}
#elif defined EXMXPM_FAMILY
    CPSS_EXMXPM_NET_TX_PARAMS_STC cpssExMxPmPcktParams;
    /*******************************************************************************
    * setExMxPmRx2TxParams
    *
    *
    *******************************************************************************/
    static GT_STATUS setExMxPmRx2TxParams
    (
        IN  GT_32 index
    )
    {
        GT_STATUS status;
            if(txPacketDescTbl == NULL)
            {
                status = init_txPacketDescTbl();
                if(status != GT_OK)
                {
                    cmdOsPrintf("txPacketDescTbl init failed\n");
                    return GT_FAIL;
                }
            }

        cmdOsMemSet(&cpssExMxPmPcktParams, 0, sizeof(cpssExMxPmPcktParams));
        cpssExMxPmPcktParams.packetIsTagged      = txPacketDescTbl[index].packetIsTagged;
        cpssExMxPmPcktParams.cookie              = &txCookie;
        cpssExMxPmPcktParams.sdmaInfo.evReqHndl  = rx2TxEvReqHndl;
        cpssExMxPmPcktParams.sdmaInfo.recalcCrc  = txPacketDescTbl[index].recalcCrc;
        cpssExMxPmPcktParams.sdmaInfo.txQueue    = txPacketDescTbl[index].txQueue;
        cpssExMxPmPcktParams.sdmaInfo.invokeTxBufferQueueEvent = txPacketDescTbl[index].invokeTxBufferQueueEvent;


        cpssExMxPmPcktParams.dsaParam.commonParams.dsaTagType = CPSS_EXMXPM_NET_DSA_TYPE_EXTENDED_E;
        cpssExMxPmPcktParams.dsaParam.commonParams.vpt    = ((CPSS_EXMXPM_SPECIFIC_TX_PKT_INFO_STC *)(txPacketDescTbl[index].specificDeviceTypeInfo))->vpt;
        cpssExMxPmPcktParams.dsaParam.commonParams.cfiBit = ((CPSS_EXMXPM_SPECIFIC_TX_PKT_INFO_STC *)(txPacketDescTbl[index].specificDeviceTypeInfo))->cfiBit;
        cpssExMxPmPcktParams.dsaParam.commonParams.vid    = txPacketDescTbl[index].vid;

        /* This function treats only from CPU DSA type. */
        cpssExMxPmPcktParams.dsaParam.dsaCmd = CPSS_EXMXPM_NET_DSA_CMD_FROM_CPU_E;

        cpssExMxPmPcktParams.dsaParam.dsaInfo.fromCpu.tc          = txPacketDescTbl[index].trafficClass;
        cpssExMxPmPcktParams.dsaParam.dsaInfo.fromCpu.dp          = txPacketDescTbl[index].dropPrecedence;
        cpssExMxPmPcktParams.dsaParam.dsaInfo.fromCpu.egrFilterEn = ((CPSS_EXMXPM_SPECIFIC_TX_PKT_INFO_STC *)(txPacketDescTbl[index].specificDeviceTypeInfo))->egrFilterEn;
        cpssExMxPmPcktParams.dsaParam.dsaInfo.fromCpu.packetType  = CPSS_EXMXPM_TS_PASSENGER_PACKET_TYPE_ETHERNET_E;
        cpssExMxPmPcktParams.dsaParam.dsaInfo.fromCpu.egrFilterRegistered = ((CPSS_EXMXPM_SPECIFIC_TX_PKT_INFO_STC *)(txPacketDescTbl[index].specificDeviceTypeInfo))->egrFilterRegistered;
        cpssExMxPmPcktParams.dsaParam.dsaInfo.fromCpu.srcId          = ((CPSS_EXMXPM_SPECIFIC_TX_PKT_INFO_STC *)(txPacketDescTbl[index].specificDeviceTypeInfo))->srcId;

        cpssExMxPmPcktParams.dsaParam.dsaInfo.fromCpu.dstInterface = txPacketDescTbl[index].dstInterface;
        cpssExMxPmPcktParams.dsaParam.dsaInfo.fromCpu.dstIsTagged = txPacketDescTbl[index].dstIsTagged;

        if( (txPacketDescTbl[index].dstInterface.type == CPSS_INTERFACE_VIDX_E) ||
            (txPacketDescTbl[index].dstInterface.type == CPSS_INTERFACE_VID_E)    )
        {
            cpssExMxPmPcktParams.dsaParam.dsaInfo.fromCpu.extDestInfo.multiDest.excludeInterface =
                txPacketDescTbl[index].excludeInterface;
            cpssExMxPmPcktParams.dsaParam.dsaInfo.fromCpu.extDestInfo.multiDest.excludedInterface =
                txPacketDescTbl[index].excludedInterface;
        }


        return GT_OK;
    }

#endif

/*******************************************************************************
* wrCpssGenRx2TxSet
*
* DESCRIPTION:
*     Library database initialization function.
*
* INPUTS:
*     none
*
* OUTPUTS:
*     none
*
* RETURNS:
*     GT_OK   - on success.
*     GT_FAIL - on failure.
*
* COMMENTS:
*     none
*
*******************************************************************************/
static CMD_STATUS wrCpssGenRx2TxSet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS status = GT_OK;
    GT_U32    entryId;

    /* check for valid arguments */
    if(inArgs == NULL || outArgs == NULL)
        return CMD_AGENT_ERROR;

        /* check txPacketDescTbl table init */
    if(txPacketDescTbl == NULL)
    {
        status = init_txPacketDescTbl();
        if(status != GT_OK)
        {
            cmdOsPrintf("txPacketDescTbl init failed\n");
            return GT_FAIL;
        }
    }

    entryId = (GT_U32)inArgs[0];
    status = txGetValidDescIndex((GT_U8)inArgs[0], GT_FALSE, entryId, &rx2TxParamIndex);
    if(status != GT_OK)
    {
        galtisOutput(outArgs, GT_NOT_INITIALIZED, "\nTx entry wasn't configured.\n");
        return CMD_OK;
    }

    rx2TxEnable = (GT_BOOL)inArgs[1];
    if(rx2TxEnable == GT_FALSE)
    {
        galtisOutput(outArgs, GT_OK, "");
        return CMD_OK;
    }

    if (rx2TxEvReqHndl == 0)
    {
        CPSS_UNI_EV_CAUSE_ENT     evCause[1] = { CPSS_PP_TX_BUFFER_QUEUE_E };
        GT_U32 i;

        if (cmdCpssEventBind(evCause, 1, &rx2TxEvReqHndl) != GT_OK)
        {
            galtisOutput(outArgs, GT_FAIL, "FAIL to create txQ Handle\n");
            return CMD_OK;
        }

        /* convert pss events to those of cpss */
        for(i = 0 ; i < 32 ; i++)
        {
            /* unmask this event with all HW devices */
            status = cmdCpssEventDeviceMaskSet((GT_U8)i,
                                                   CPSS_PP_TX_BUFFER_QUEUE_E,
                                                   CPSS_EVENT_UNMASK_E);
            if(status != GT_OK)
            {
                /* there are FA/XBAR events that not relevant to PP device,
                vice versa */
                continue;
            }
        }

    }

#if defined CHX_FAMILY
    status = setDxChRx2TxParams(rx2TxParamIndex);
#elif defined EXMXPM_FAMILY
    status = setExMxPmRx2TxParams(rx2TxParamIndex);
#endif

    rx2TxInvokeIntBufQueue = txPacketDescTbl[rx2TxParamIndex].invokeTxBufferQueueEvent;
    rx2TxSyncMode = txPacketDescTbl[rx2TxParamIndex].txSyncMode;

    galtisOutput(outArgs, status, "");
    return CMD_OK;
}


/*******************************************************************************
* prvCpssGenNetIfRx2Tx
*
* DESCRIPTION:
*     Library database initialization function.
*
* INPUTS:
*     none
*
* OUTPUTS:
*     none
*
* RETURNS:
*     GT_OK   - on success.
*     GT_FAIL - on failure.
*
* COMMENTS:
*     none
*
*******************************************************************************/

/*******************************************************************************
 packetSend - Routine  for sending packet.
*******************************************************************************/
static GT_STATUS packetSend
(
    IN GT_U8                                    *buffList[],
    IN GT_U32                                   buffLenList[],
    IN GT_U32                                   numOfBufs
)
{
    GT_STATUS status = GT_OK;
    IN GT_U8  devNum;

    /* check txPacketDescTbl table init */
    if(txPacketDescTbl == NULL)
    {
        status = init_txPacketDescTbl();
        if(status == GT_OK)
        {
            cmdOsPrintf("txPacketDescTbl is empty - no transmit\n");
            return GT_OK;
        }
        else
        {
            cmdOsPrintf("txPacketDescTbl init failed\n");
            return GT_FAIL;
        }
    }


    if(cmdIsCpuEtherPortUsed() == GT_TRUE)
    {
      devNum = txPacketDescTbl[rx2TxParamIndex].devNum;
      if(txPacketDescTbl[rx2TxParamIndex].txSyncMode==GT_TRUE)
      {
          #if defined CHX_FAMILY
              status = cpssDxChNetIfMiiSyncTxPacketSend(devNum, &cpssDxChPcktParams,buffList, buffLenList, numOfBufs);
          #elif defined EXMXPM_FAMILY
              status = GT_NOT_SUPPORTED;
          #endif
      }
      else
      {
          #if defined CHX_FAMILY
              status = cpssDxChNetIfMiiTxPacketSend(devNum, &cpssDxChPcktParams,buffList,buffLenList,numOfBufs);
          #elif defined EXMXPM_FAMILY
              status = GT_NOT_SUPPORTED;
          #endif
      }
    }
    else
    {
      devNum = txPacketDescTbl[rx2TxParamIndex].devNum;
      if(txPacketDescTbl[rx2TxParamIndex].txSyncMode==GT_TRUE)
      {
          #if defined CHX_FAMILY
              status = cpssDxChNetIfSdmaSyncTxPacketSend(devNum, &cpssDxChPcktParams,
                                                               buffList, buffLenList, numOfBufs);
          #elif defined EXMXPM_FAMILY
               status = cpssExMxPmNetIfSdmaTxSyncPacketSend(devNum, &cpssExMxPmPcktParams,
                                                                buffList, buffLenList, numOfBufs);
          #endif
      }
      else
      {
          #if defined CHX_FAMILY
              status = cpssDxChNetIfSdmaTxPacketSend(devNum, &cpssDxChPcktParams,buffList,buffLenList,numOfBufs);
          #elif defined EXMXPM_FAMILY
              status = cpssExMxPmNetIfSdmaTxPacketSend(devNum, &cpssExMxPmPcktParams,buffList,buffLenList,numOfBufs);
          #endif
      }
    }
    return status;
}

/*******************************************************************************
 waitEndEventAndClean - Routine waiting end event and cleaning the instances.
*******************************************************************************/
static GT_STATUS waitEndEventAndClean( void )
{
    /* Wait for packet queued/sent indication*/
    GT_STATUS status = GT_OK;
    GT_U8           devNum;
    GT_U8           queueIdx;
    GT_PTR          cookie;

    devNum = queueIdx = 0;
    cookie = NULL;

    /* wait for the Tx-End/Tx-BufferQueue event */
    status = cmdCpssEventSelect(rx2TxEvReqHndl, NULL, NULL, 0);

    if (status != GT_OK)
    {
        return status;
    }

    /* get all Tx end events for the packet */
    if(cmdIsCpuEtherPortUsed() == GT_TRUE)
    {
#if defined CHX_FAMILY
        while(1)
        {
          status = cpssDxChNetIfMiiTxBufferQueueGet(rx2TxEvReqHndl, &devNum,
                                                 &cookie, &queueIdx, &status);
          if (status == GT_FAIL)
          {
            return status;
          }
          if(status == GT_NO_MORE)
          {
            break;
          }
        }
#else
        status = GT_NOT_SUPPORTED;
#endif
    }
    else
    {
        while(1)
        {
#if defined CHX_FAMILY
          status = cpssDxChNetIfTxBufferQueueGet(rx2TxEvReqHndl, &devNum,
                                                 &cookie, &queueIdx, &status);
#elif defined EXMXPM_FAMILY
          status = cpssExMxPmNetIfSdmaTxBufferQueueInfoGet(rx2TxEvReqHndl, &devNum,
                                                      &cookie, &queueIdx, &status);
#endif
          if (status == GT_FAIL)
          {
            return status;
          }
          if(status == GT_NO_MORE)
          {
            break;
          }
        }
    }

    return status;
}

GT_STATUS prvCpssGenNetIfRx2Tx
(
    IN GT_U8                                    *buffList[],
    IN GT_U32                                   buffLenList[],
    IN GT_U32                                   numOfBufs
)
{
    GT_STATUS status = GT_OK;

    if(rx2TxEnable == GT_FALSE)
        return GT_OK;

    /* Send the packet */
    status = packetSend(buffList,buffLenList,numOfBufs);
    if((status != GT_OK) && (status != GT_NO_RESOURCE))
    {
        return status;
    }

    if( ( (rx2TxSyncMode==GT_FALSE) &&
          (cpssExtDrvEthRawSocketModeGet() == GT_FALSE) &&
          (rx2TxInvokeIntBufQueue == GT_TRUE))
        ||
        (cmdIsCpuEtherPortUsed() == GT_TRUE))
    {
        status = waitEndEventAndClean();
    }

    return status;
}
#if defined EXMXPM_FAMILY
/*******************************************************************************
* cpssGenNetIfTxForwardSet (table command)
*
* DESCRIPTION:
*     Creates new transmit parameters description entry in the "forward" DSA Tag table.
*
* INPUTS:
*     [0] GT_U32 lIndex -
*          Entry ID of packet descriptor.
*
*     [1] DXCH_TX_CMD_ENT lCmdType -
*          Defines type of transmition (VIDX, PORT).
*
*
* OUTPUTS:
*     none
*
* RETURNS:
*     CMD_OK            - on success.
*     CMD_AGENT_ERROR   - on failure.
*     CMD_FIELD_UNDERFLOW - not enough field arguments.
*     CMD_FIELD_OVERFLOW  - too many field arguments.
*
* COMMENTS:
*     Relevant for Puma3 and above devices
*
*******************************************************************************/
static CMD_STATUS wrCpssGenNetIfTxForwardEntrySet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS status;
    GT_U32    entryIndex = 0, entryId;
    GT_32     sizes[2] = {24, 24};
    GT_32     i = 0;

    /* Check for valid arguments */
    if(inArgs == NULL || outArgs == NULL)
        {
        return CMD_AGENT_ERROR;
        }

    if(numFields < sizes[inArgs[1]])
        {
        return CMD_FIELD_UNDERFLOW;
        }

    /* Check txPacketDescTbl table init */
    if(txPacketDescTbl == NULL)
    {
        status = init_txPacketDescTbl();
        if(status != GT_OK)
        {
            cmdOsPrintf("txPacketDescTbl init failed\n");
            return GT_FAIL;
        }
    }


    /* Create the pool only once */
    if(poolCreated == GT_FALSE)
    {
        /* register function to reset DB so after system reset we can send traffic again */
        wrCpssRegisterResetCb(wrTxSystemReset);
        /* Create pool of buffers from cache */
        status = cmdOsPoolCreateDmaPool(
                                  PRV_TX_BUFFER_SIZE + 64 /* ALIGN_ADDR_CHUNK */,
                                  GT_4_BYTE_ALIGNMENT,
                                  PRV_MAX_NUM_OF_BUFFERS_AT_POOL,
                                  GT_TRUE,
                                  &genTxBuffersPoolId);

        if (status != GT_OK)
        {
            cmdOsPrintf("ERROR-PoolCreateDmaPool\n\n");
            return status;
        }

        if(txPacketDescTbl == NULL)
        {
            txPacketDescTbl =
                (CPSS_TX_PKT_DESC*) cmdOsMalloc(sizeof(*txPacketDescTbl) * MAX_NUM_OF_PCAKETS_TO_SEND);
            txTblCapacity = MAX_NUM_OF_PCAKETS_TO_SEND;

            cmdOsMemSet(txPacketDescTbl, 0, sizeof(*txPacketDescTbl) * txTblCapacity);
        }

        cmdOsMemSet(specificDeviceTypeInfo, 0, sizeof(specificDeviceTypeInfo));
        poolCreated = GT_TRUE;
    }


    /* When calling wrCpssGenNetIfTxForwardEntrySet all specificDeviceTypeInfo
       fields will be 0. Here we bind the predefined specificDeviceTypeInfo structure to the
       sw shadow - txPacketDescTbl */
    for(i=0; i<MAX_NUM_OF_PCAKETS_TO_SEND; i++)
    {
        if(txPacketDescTbl[i].specificDeviceTypeInfo==NULL)
        {
            txPacketDescTbl[i].specificDeviceTypeInfo = &specificDeviceTypeInfo[i];
        }
    }

    /* Get entry in the table */
    entryId = (GT_U32)inFields[0];
    status = txGetValidOrEmptyDescIndex((GT_U8)inArgs[0], GT_TRUE, entryId, &entryIndex);
    if( status != GT_OK)
    {
        galtisOutput(outArgs, GT_NO_RESOURCE, "");
        return CMD_OK;
    }


    /*  Non zero packet data length */
    if ((GT_U8)inFields[23] == 0)
    {
        galtisOutput(outArgs, GT_BAD_PARAM, "");
        return CMD_OK;
    }

    txPacketDescTbl[entryIndex].dsaTagCmd = PRV_CPSS_NET_DSA_CMD_FORWARD_E;
    txPacketDescTbl[entryIndex].entryId = entryId;
    txPacketDescTbl[entryIndex].devNum = (GT_U8)inArgs[0];
    txPacketDescTbl[entryIndex].packetIsTagged = (GT_BOOL)inFields[1];
    txPacketDescTbl[entryIndex].vid = (GT_U16)inFields[2];
    txPacketDescTbl[entryIndex].dropPrecedence = (CPSS_DP_LEVEL_ENT)inFields[3];
    txPacketDescTbl[entryIndex].trafficClass = (GT_U8)inFields[4];
    txPacketDescTbl[entryIndex].recalcCrc = (GT_BOOL)inFields[5];
    txPacketDescTbl[entryIndex].txSyncMode = (GT_BOOL)inFields[6];
    txPacketDescTbl[entryIndex].txQueue = (GT_U8)inFields[7];
    txPacketDescTbl[entryIndex].invokeTxBufferQueueEvent = inFields[8];
    txPacketDescTbl[entryIndex].numPacketsToSend = (GT_U32)inFields[9];
    txPacketDescTbl[entryIndex].gap = (GT_U32)inFields[10];
    txPacketDescTbl[entryIndex].waitTime = (GT_U32)inFields[11];
    txPacketDescTbl[entryIndex].srcDevice = (GT_U8)inFields[12];
    txPacketDescTbl[entryIndex].srcIsTrunk = (GT_BOOL)inFields[13];
    if(txPacketDescTbl[entryIndex].srcIsTrunk == GT_TRUE) /* Source is trunk */
    {
        txPacketDescTbl[entryIndex].source.trunkId = (GT_U16)inFields[14];
    }
    else
    {
        txPacketDescTbl[entryIndex].source.portNum = (GT_U8)inFields[14];
    }

    specificDeviceTypeInfo[entryIndex].vpt = (GT_U8)inFields[15];
    specificDeviceTypeInfo[entryIndex].cfiBit = (GT_U8)inFields[16];
    specificDeviceTypeInfo[entryIndex].srcId = inFields[17];
    specificDeviceTypeInfo[entryIndex].egrFilterRegistered = inFields[18];
    txPacketDescTbl[entryIndex].wasRouted = (GT_BOOL)inFields[19];

    switch(inArgs[1])
    {

        case 0: /* TX to MULTI DESTINATION */
            txPacketDescTbl[entryIndex].dstInterface.type = (CPSS_INTERFACE_TYPE_ENT)inFields[20];
            if(txPacketDescTbl[entryIndex].dstInterface.type == CPSS_INTERFACE_VID_E)
            {
                txPacketDescTbl[entryIndex].dstInterface.vlanId = (GT_U16)inFields[21];
            }
            else
            {
                txPacketDescTbl[entryIndex].dstInterface.vidx = (GT_U16)inFields[21];
            }

            break;

        case 1: /* TX to SINGLE DESTINATION */
            txPacketDescTbl[entryIndex].dstInterface.type = CPSS_INTERFACE_PORT_E;
            txPacketDescTbl[entryIndex].dstInterface.devPort.hwDevNum = (GT_U8)inFields[20];
            txPacketDescTbl[entryIndex].dstInterface.devPort.portNum = (GT_U8)inFields[21];

            /* Override Device and Port */
            CONVERT_DEV_PORT_DATA_MAC(
                txPacketDescTbl[entryIndex].dstInterface.devPort.hwDevNum,
                txPacketDescTbl[entryIndex].dstInterface.devPort.portNum);

            break;

        default:
            status = GT_BAD_PARAM;
            break;
    }

    if( status != GT_OK)
    {
        galtisOutput(outArgs, status, "");
        return CMD_OK;
    }

    /* Copy the packet data to DB */
    status = setBufferArraysOfEqualSize((GT_U8*)inFields[22],
                               (GT_U32)inFields[23],
                               PRV_TX_BUFFER_SIZE,
                               txPacketDescTbl[entryIndex].pcktData,
                               txPacketDescTbl[entryIndex].pcktDataLen,
                               &txPacketDescTbl[entryIndex].numOfBuffers);


    if (status != GT_OK)
    {
        /* Still need to free, because of partial allocation. */
        freeBufferArraysOfEqualSize(txPacketDescTbl[entryIndex].pcktData,
                                    txPacketDescTbl[entryIndex].pcktDataLen);

        galtisOutput(outArgs, GT_NO_RESOURCE, "");
        return CMD_OK;
    }

    /* The entry is valid */
    txPacketDescTbl[entryIndex].valid = GT_TRUE;

    galtisOutput(outArgs, GT_OK, "");

    return CMD_OK;
}

/*******************************************************************************
* getTxNetIfForwardTblEntry (table command)
*
* DESCRIPTION:
*     Retruns specific packet descriptor entry from the "forward" DSA Tag table.
*
* INPUTS:
*     [0] GT_U32 lIndex -
*          Entry ID of packet descriptor.
*
*     [1] DXCH_TX_CMD_ENT lCmdType -
*          Defines type of transmition (VIDX, PORT).
*
*
* OUTPUTS:
*      Table fields as describe in the table description are written into
*      outArgs parameter.
*
* RETURNS:
*     CMD_OK            - on success.
*     CMD_AGENT_ERROR   - on failure.
*     CMD_FIELD_UNDERFLOW - not enough field arguments.
*     CMD_FIELD_OVERFLOW  - too many field arguments.
*
* COMMENTS:
*     Relevant for Puma3 and above devices
*
*******************************************************************************/
static GT_STATUS getTxNetIfForwardTblEntry
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_U32       index=0;
    GT_BYTE_ARRY lGtBuff;
    GT_STATUS    status = GT_OK;
    GT_HW_DEV_NUM  __HwDev; /* Dummy for converting. */
    GT_PORT_NUM    __Port; /* Dummy for converting. */


        /* check txPacketDescTbl table init */
    if(txPacketDescTbl == NULL)
    {
        status = init_txPacketDescTbl();
        if(status != GT_OK)
        {
            cmdOsPrintf("txPacketDescTbl init failed\n");
            return GT_FAIL;
        }
    }


    for( ; netIfTblEntryId < MAX_NUM_OF_PCAKETS_TO_SEND ; netIfTblEntryId++)
    {
        status = txGetValidDescIndex((GT_U8)inArgs[0], GT_TRUE, netIfTblEntryId, &index);

        if(status == GT_NOT_FOUND)
            continue;

        if(status == GT_OK)
        {
            netIfTblEntryId++;
            break;
        }
    }
    if(netIfTblEntryId == MAX_NUM_OF_PCAKETS_TO_SEND)
    {
        return GT_NOT_FOUND;
    }

    /* Mapping structure to fields */
    inFields[0] = txPacketDescTbl[index].entryId;
    inFields[1] = txPacketDescTbl[index].packetIsTagged;
    inFields[2] = txPacketDescTbl[index].vid;
    inFields[3] = txPacketDescTbl[index].dropPrecedence;
    inFields[4] = txPacketDescTbl[index].trafficClass;
    inFields[5] = txPacketDescTbl[index].recalcCrc;
    inFields[6] = txPacketDescTbl[index].txSyncMode;
    inFields[7] = txPacketDescTbl[index].txQueue;
    inFields[8] = txPacketDescTbl[index].invokeTxBufferQueueEvent;
    inFields[9] = txPacketDescTbl[index].numPacketsToSend;
    inFields[10] = txPacketDescTbl[index].gap;
    inFields[11] = txPacketDescTbl[index].waitTime;
    inFields[12] = txPacketDescTbl[index].srcDevice;
    inFields[13] = txPacketDescTbl[index].srcIsTrunk;
    if(inFields[13] == GT_TRUE) /* Source is trunk */
    {
        inFields[14] = txPacketDescTbl[index].source.trunkId;
    }
    else
    {
        inFields[14] = txPacketDescTbl[index].source.portNum;
    }

    inFields[15] = specificDeviceTypeInfo[index].vpt;
    inFields[16] = specificDeviceTypeInfo[index].cfiBit;
    inFields[17] = specificDeviceTypeInfo[index].srcId;
    inFields[18] = specificDeviceTypeInfo[index].egrFilterRegistered;
    inFields[19] = txPacketDescTbl[index].wasRouted;

    switch( txPacketDescTbl[index].dstInterface.type)
    {
        case CPSS_INTERFACE_VID_E:
        case CPSS_INTERFACE_VIDX_E:
            inFields[20] =  txPacketDescTbl[index].dstInterface.type;
            if(txPacketDescTbl[index].dstInterface.type == CPSS_INTERFACE_VID_E)
                inFields[21] =  txPacketDescTbl[index].dstInterface.vlanId;
            else
                inFields[21] =  txPacketDescTbl[index].dstInterface.vidx;

            break;

        case CPSS_INTERFACE_PORT_E:
            __HwDev = txPacketDescTbl[index].dstInterface.devPort.hwDevNum;
            __Port = txPacketDescTbl[index].dstInterface.devPort.portNum;
            /* Convert back device and port */
            CONVERT_BACK_DEV_PORT_DATA_MAC(__HwDev, __Port);

            inFields[20] = __HwDev;
            inFields[21] = __Port;

            break;

        default:
            break;
    }

    /* Send to Galtis only the first buffer of the packet */
    lGtBuff.data = txPacketDescTbl[index].pcktData[0];
    lGtBuff.length =  txPacketDescTbl[index].pcktDataLen[0];

    inFields[23] = *(txPacketDescTbl[index].pcktDataLen);

    switch( txPacketDescTbl[index].dstInterface.type)
    {
        case CPSS_INTERFACE_VID_E:
        case CPSS_INTERFACE_VIDX_E:

            /* pack and output table fields */
            fieldOutput("%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%s%d",
                        inFields[0], inFields[1], inFields[2], inFields[3],
                        inFields[4], inFields[5], inFields[6], inFields[7],
                        inFields[8], inFields[9], inFields[10], inFields[11],
                        inFields[12],inFields[13],inFields[14], inFields[15],
                        inFields[16], inFields[17],inFields[18], inFields[19],
                        inFields[20], inFields[21],
                        galtisBArrayOut(&lGtBuff), inFields[23]);
            galtisOutput(outArgs, status, "%d%f",  0);
            break;

        case CPSS_INTERFACE_PORT_E:
            fieldOutput("%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%s%d",
                        inFields[0], inFields[1], inFields[2], inFields[3],
                        inFields[4], inFields[5], inFields[6], inFields[7],
                        inFields[8], inFields[9], inFields[10], inFields[11],
                        inFields[12],inFields[13],inFields[14], inFields[15],
                        inFields[16], inFields[17],inFields[18], inFields[19],
                        inFields[20], inFields[21],
                        galtisBArrayOut(&lGtBuff), inFields[23]);
            galtisOutput(outArgs, status, "%d%f",  1);

            break;

        default:
            break;
    }

    return GT_OK;
}

/*******************************************************************************
* wrCpssGenNetIfTxForwardEntryGet (table command)
*
* DESCRIPTION:
*     Retruns specific packet descriptor entry from the "forward" DSA Tag table.
*
* INPUTS:
*     [0] GT_U32 lIndex -
*          Entry ID of packet descriptor.
*
*     [1] DXCH_TX_CMD_ENT lCmdType -
*          Defines type of transmition (VIDX, PORT).
*
*
* OUTPUTS:
*      Table fields as describe in the table description are written into
*      outArgs parameter.
*
* RETURNS:
*     CMD_OK            - on success.
*     CMD_AGENT_ERROR   - on failure.
*     CMD_FIELD_UNDERFLOW - not enough field arguments.
*     CMD_FIELD_OVERFLOW  - too many field arguments.
*
* COMMENTS:
*     Relevant for Puma3 and above devices
*
*******************************************************************************/
static CMD_STATUS wrCpssGenNetIfTxForwardEntryGet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{

    /* check for valid arguments */
    if(inArgs == NULL || outArgs == NULL)
        return CMD_AGENT_ERROR;

    if(GT_NOT_FOUND == getTxNetIfForwardTblEntry(inArgs ,inFields ,numFields ,outArgs))
        galtisOutput(outArgs, GT_EMPTY, "");

    return CMD_OK;
}

/*******************************************************************************
* wrCpssGenNetIfTxForwardEntryGetFirst (table command)
*
* DESCRIPTION:
*     Get first entry in the "forward" DSA Tag table.
*
* INPUTS:
*     [0] GT_U32 lIndex -
*          Entry ID of packet descriptor.
*
*     [1] DXCH_TX_CMD_ENT lCmdType -
*          Defines type of transmition (VIDX, PORT).
*
*
* OUTPUTS:
*      Table fields as describe in the table description are written into
*      outArgs parameter.
*
* RETURNS:
*     CMD_OK            - on success.
*     CMD_AGENT_ERROR   - on failure.
*     CMD_FIELD_UNDERFLOW - not enough field arguments.
*     CMD_FIELD_OVERFLOW  - too many field arguments.
*
* COMMENTS:
*     Relevant for Puma3 and above devices
*
*******************************************************************************/
static CMD_STATUS wrCpssGenNetIfTxForwardEntryGetFirst
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS rc;
    /* check for valid arguments */
    if(inArgs == NULL || outArgs == NULL)
        return CMD_AGENT_ERROR;

    netIfTblEntryId = 0;

    rc = getTxNetIfForwardTblEntry(inArgs, inFields, numFields, outArgs);
    if(rc == GT_NOT_FOUND)
        galtisOutput(outArgs, GT_EMPTY, "%d", -1);

    return CMD_OK;
}

/*******************************************************************************
* wrCpssGenNetIfTxForwardEntryGetNext (table command)
*
* DESCRIPTION:
*     Get next entry in the "forward" DSA Tag table.
*
* INPUTS:
*     [0] GT_U32 lIndex -
*          Entry ID of packet descriptor.
*
*     [1] DXCH_TX_CMD_ENT lCmdType -
*          Defines type of transmition (VIDX, PORT).
*
*
* OUTPUTS:
*      Table fields as describe in the table description are written into
*      outArgs parameter.
*
* RETURNS:
*     CMD_OK            - on success.
*     CMD_AGENT_ERROR   - on failure.
*     CMD_FIELD_UNDERFLOW - not enough field arguments.
*     CMD_FIELD_OVERFLOW  - too many field arguments.
*
* COMMENTS:
*     Relevant for Puma3 and above devices
*
*******************************************************************************/
static CMD_STATUS wrCpssGenNetIfTxForwardEntryGetNext
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS rc;

    /* check for valid arguments */
    if(inArgs == NULL || outArgs == NULL)
        return CMD_AGENT_ERROR;

    rc = getTxNetIfForwardTblEntry(inArgs, inFields, numFields, outArgs);
    if(rc == GT_NOT_FOUND)
        galtisOutput(outArgs, GT_OK, "%d", -1);

    return CMD_OK;
}
#endif

/**** database initialization **************************************/

static CMD_COMMAND dbCommands[] =
{

    {"cpssGenNetIfTxFromCpuEntrySet",
        &wrCpssGenNetIfTxFromCpuEntrySet,
        2, 22},

    {"cpssGenNetIfTxFromCpuEntryGet",
        &wrCpssGenNetIfTxFromCpuEntryGet,
        1, 0},

    {"cpssGenNetIfTxFromCpuEntryGetFirst",
        &wrCpssGenNetIfTxFromCpuEntryGetFirst,
        1, 0},

    {"cpssGenNetIfTxFromCpuEntryGetNext",
        &wrCpssGenNetIfTxFromCpuEntryGetNext,
        1, 0},

    {"cpssGenNetIfTxFromCpuEntryDelete",
        &wrCpssGenNetIfTxFromCpuEntryDelete,
        1, 22},

    {"cpssGenNetIfTxFromCpuEntryClear",
        &wrCpssGenNetIfTxFromCpuEntryClear,
        0, 0},

#if defined EXMXPM_FAMILY
    /*----------------------------------------------*/
    /* table cpssExMxPm2NetIfFromCpuExtFieldsEntry  */
    /*----------------------------------------------*/
    {"cpssExMxPm2NetIfFromCpuExtFieldsEntrySet",
        &wrCpssExMxPm2NetIfFromCpuExtFieldsEntrySet,
        1, 6},

    {"cpssExMxPm2NetIfFromCpuExtFieldsEntryGet",
        &wrCpssExMxPm2NetIfFromCpuExtFieldsEntryGet,
        1, 0},

    {"cpssExMxPm2NetIfFromCpuExtFieldsEntryGetFirst",
        &wrCpssExMxPm2NetIfFromCpuExtFieldsEntryGetFirst,
        1, 0},

    {"cpssExMxPm2NetIfFromCpuExtFieldsEntryGetNext",
        &wrCpssExMxPm2NetIfFromCpuExtFieldsEntryGetNext,
        1, 0},

    {"cpssExMxPm2NetIfFromCpuExtFieldsEntryClear",
        &wrCpssExMxPm2NetIfFromCpuExtFieldsEntryClear,
        0, 0},
    /*----------------------------------------------*/
    /* table cpssExMxPm2NetIToDITExtFieldsEntry     */
    /*----------------------------------------------*/
    {"cpssExMxPm2NetIfToDITExtFieldsEntrySet",
        &wrCpssExMxPm2NetIfToDITExtFieldsEntrySet,
        1, 8},

    {"cpssExMxPm2NetIfToDITExtFieldsEntryGet",
        &wrCpssExMxPm2NetIfToDITExtFieldsEntryGet,
        1, 0},

    {"cpssExMxPm2NetIfToDITExtFieldsEntryGetFirst",
        &wrCpssExMxPm2NetIfToDITExtFieldsEntryGetFirst,
        1, 0},

    {"cpssExMxPm2NetIfToDITExtFieldsEntryGetNext",
        &wrCpssExMxPm2NetIfToDITExtFieldsEntryGetNext,
        1, 0},

    {"cpssExMxPm2NetIfToDITExtFieldsEntryClear",
        &wrCpssExMxPm2NetIfToDITExtFieldsEntryClear,
        0, 0},
    /*----------------------------------------------*/
    /*    forward entry                             */
    /*----------------------------------------------*/

    {"cpssGenNetIfTxForwardEntrySet",
        &wrCpssGenNetIfTxForwardEntrySet,
        2, 24},

    {"cpssGenNetIfTxForwardEntryGet",
        &wrCpssGenNetIfTxForwardEntryGet,
        1, 0},

    {"cpssGenNetIfTxForwardEntryGetFirst",
        &wrCpssGenNetIfTxForwardEntryGetFirst,
        1, 0},

    {"cpssGenNetIfTxForwardEntryGetNext",
        &wrCpssGenNetIfTxForwardEntryGetNext,
        1, 0},

    {"cpssGenNetIfTxForwardEntryDelete",
        &wrCpssGenNetIfTxFromCpuEntryDelete,
        1, 0},

    {"cpssGenNetIfTxForwardEntryClear",
        &wrCpssGenNetIfTxFromCpuEntryClear,
        0, 0},

#endif

    {"cpssGenTxSetMode",
        &wrCpssGenTxSetMode,
        2, 0},

    {"cpssGenTxGetMode",
        &wrCpssGenTxGetMode,
        0, 0},

    {"cpssGenTxStart",
        &wrCpssGenTxStart,
        0, 0},

    {"cpssGenTxStop",
        &wrCpssGenTxStop_,
        0, 0},

    {"cpssGenRx2TxSet",
        &wrCpssGenRx2TxSet,
        2, 0},

    {"cpssGenTxGetTxBufferEventHandle",
        &wrCpssGenTxGetTxBufferQueueEventHandle,
        0, 0},

    {"cpssGenTxBufferQueueEventHandling",
        &wrCpssGenTxBufferQueueEventHandling,
        0, 0},
};

#define numCommands (sizeof(dbCommands) / sizeof(CMD_COMMAND))

/*******************************************************************************
* cmdLibResetCpssGenTxNetIf
*
* DESCRIPTION:
*     Library database reset function.
*
* INPUTS:
*     none
*
* OUTPUTS:
*     none
*
* RETURNS:
*     none
*
* COMMENTS:
*     none
*
*******************************************************************************/
static GT_VOID cmdLibResetCpssGenTxNetIf
(
    GT_VOID
)
{
    GT_STATUS rc;

    rc = prvWrCpssGenNetIfTxFromCpuEntryClear(GT_FALSE);
    if(rc != GT_OK)
    {
        cmdOsPrintf("cmdLibResetCpssGenTxNetIf: txPacketDescTbl init failed\n");
    }

    return;
}

/*******************************************************************************
* cmdLibInitCpssDxChNetIf
*
* DESCRIPTION:
*     Library database initialization function.
*
* INPUTS:
*     none
*
* OUTPUTS:
*     none
*
* RETURNS:
*     GT_OK   - on success.
*     GT_FAIL - on failure.
*
* COMMENTS:
*     none
*
*******************************************************************************/
GT_STATUS cmdLibInitCpssGenTxNetIf
(
    GT_VOID
)
{
    GT_STATUS rc;

    rc = wrCpssRegisterResetCb(cmdLibResetCpssGenTxNetIf);
    if(rc != GT_OK)
    {
        return rc;
    }

    return cmdInitLibrary(dbCommands, numCommands);
}

