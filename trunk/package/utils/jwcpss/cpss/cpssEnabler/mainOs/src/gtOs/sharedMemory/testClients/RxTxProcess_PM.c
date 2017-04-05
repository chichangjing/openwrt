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
* RxTxProcess_PM.c
*
* DESCRIPTION:
*       This file includes the RxTx process code for multi process appDemo.
*
* DEPENDENCIES:
*
*
* COMMENTS:
*		The process includes :
* 			- event handler for Rx packets events
*			        CPSS_PP_RX_BUFFER_QUEUE0_E
*        			CPSS_PP_RX_BUFFER_QUEUE1_E
*        			CPSS_PP_RX_BUFFER_QUEUE2_E
*        			CPSS_PP_RX_BUFFER_QUEUE3_E
*        			CPSS_PP_RX_BUFFER_QUEUE4_E
*        			CPSS_PP_RX_BUFFER_QUEUE5_E
*        			CPSS_PP_RX_BUFFER_QUEUE6_E
*        			CPSS_PP_RX_BUFFER_QUEUE7_E
*
*			- Rx packet receive treatment includes the following steps:
*				> copy received packet from DMA buffer to the static local
*				   buffer	
*				> get Rx Control Parameters from CPSS Shared Lib buffer;
*				   			the appDemo process send this Rx Control Parameters 
*                  			to special CPSS Shared Lib static buffer;
*							this Rx Control Parameters include the pointer to Packet Capture Table
*                 			allocated by appDemo process 
*				> allocate Packet Buffer in Shared Heap 
*				> copy the packet from the static local buffer the Packet Buffer in Shared Heap
*				> fill Packet Capture Table Entry including the packet buffer pointer  
*    			> bind  the Packet Buffer in Shared Heap to Packet Capture Table Entry buffer pointer
*
*			- Tx packet treatment includes:
*				> Tx thread
*				> TxPacketTransmit function that receives the Tx parameters from appDemo process
*					and sens packets
*
*
* FILE REVISION NUMBER:
*       $Revision: 4 $
*******************************************************************************/

#include <gtOs/shrMemTestClients/RxTxProcess.h>

#include <gtOs/gtOsSharedUtil.h>
#include <gtOs/gtOsSharedIPC.h>
#include <gtOs/gtOsSharedData.h>
#include <gtOs/gtOsInit.h>



static GT_U32  sdmaTxPacketSendCount = 0; /* Packets sent, and need to wait
                                             for TX_BUFF_Q event */
static GT_U32  sdmaTxPacketGetCount = 0;  /* Packet whos info got after receiving
                                             TX_BUFF_Q event. */

#if (defined EXMXPM_FAMILY)
    #define PM_CODE
#endif 

#include <galtisAgent/wrapCpss/Gen/networkIf/wrapCpssGenNetIf.h>
/*
#include <cpss/exMxPm/exMxPmGen/networkIf/cpssExMxPmNetIfTypes.h>
*/

typedef struct
{
    GT_U32  cyclesNum;
    GT_U32  gap;
} MODE_SETTINGS_STC;


static MODE_SETTINGS_STC modeSettings = {1, 1};

static GT_U32 txTblCapacity = 0;

static CPSS_TX_PKT_DESC * cpssTxPacketDescTbl = NULL;


static GT_BOOL flagStopTransmit = GT_FALSE;

static GT_UINTPTR  evReqHndl = 0;
static GT_U8 txCookie = 0x10;

/* Transmit task ID */
static GT_TASK taskId = 0;
static GT_SEM  txCmdSemId;                      /* Module semaphore id  */
static GT_BOOL taskBusyFlag = GT_FALSE;


#ifdef IMPL_GALTIS
static  GT_STATUS cmdGenRxPktReceive
(
    IN GT_U8      devNum,
    IN GT_U8      queueIdx,
    IN GT_U32     numOfBuff,
    IN GT_U8     *packetBuffs[],
    IN GT_U32     buffLen[],
    IN void      *rxParamsPtr
);
#endif /* IMPL_GALTIS */

static void rxInfoFree
(
IN void*  specificDeviceFormatPtr
);


#include <cpss/exMxPm/exMxPmGen/networkIf/cpssExMxPmNetIfTypes.h>
#include <cpss/exMxPm/exMxPmGen/networkIf/cpssExMxPmNetIf.h>


static CMD_RX_ENTRY *rxEntriesArr;
static GALTIS_RX_MODE_ENT rxTableMode;
static GT_U32  maxRxBufferSize;
static GT_U32  maxNumOfRxEntries;
static GT_U32  indexToInsertPacket;
static GT_U32  doCapture;
static GT_BOOL wasInitDone;


/******* RX 2 TX ****************/

#define MAX_NUM_OF_PCAKETS_TO_SEND 100

static GT_U32 rx2TxParamIndex = MAX_NUM_OF_PCAKETS_TO_SEND;
static GT_BOOL rx2TxEnable = GT_FALSE;
static GT_BOOL rx2TxSyncMode = GT_FALSE;
static GT_BOOL rx2TxInvokeIntBufQueue = GT_FALSE;


/*************************************************************************************/
CPSS_EXMXPM_NET_TX_PARAMS_STC cpssExMxPmPcktParams;


static GT_STATUS packetSend
(
    IN GT_U8                                    *buffList[],
    IN GT_U32                                   buffLenList[],
    IN GT_U32                                   numOfBufs
)
{
    GT_STATUS status = GT_OK;
    IN GT_U8  devNum;


      devNum = cpssTxPacketDescTbl[rx2TxParamIndex].devNum;
      if(cpssTxPacketDescTbl[rx2TxParamIndex].txSyncMode==GT_TRUE)
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

    return status;
}

static GT_U32  rx2TxEvReqHndl = 0;

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
    status = cpssEventSelect(rx2TxEvReqHndl, NULL, NULL, 0);

    if (status != GT_OK)
    {
        return status;
    }

    /* get all Tx end events for the packet */
        /*
           . */
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

    return status;
}



static GT_STATUS prvCpssGenNetIfRx2Tx
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

    if  ( (rx2TxSyncMode==GT_FALSE) && (rx2TxInvokeIntBufQueue == GT_TRUE))
    {
        status = waitEndEventAndClean();
    }

    return status;
}





extern GT_STATUS prvCpssGenNetIfRx2Tx
(
    IN GT_U8  *buffList[],
    IN GT_U32 buffLenList[],
    IN GT_U32 numOfBufs
);

static void rxInfoCopy
(
    IN  const void*  srcSpecificDeviceFormatPtr,
    OUT void*        dstSpecificDeviceFormatPtr
)
{
    GT_U32 size = 0;


#if defined CHX_FAMILY
    size = sizeof(CPSS_DXCH_NET_RX_PARAMS_STC);
#endif
#if defined EXMXPM_FAMILY
    if(size < sizeof(CPSS_EXMXPM_NET_RX_PARAMS_STC))
        size = sizeof(CPSS_EXMXPM_NET_RX_PARAMS_STC);
#endif
#if defined EX_FAMILY
    if(size < sizeof(CPSS_EXMX_NET_RX_PARAMS_STC))
        size = sizeof(CPSS_EXMX_NET_RX_PARAMS_STC);
#endif


    if(srcSpecificDeviceFormatPtr == NULL || dstSpecificDeviceFormatPtr == NULL)
    {
        return;
    }

    osMemCpy(dstSpecificDeviceFormatPtr,
             srcSpecificDeviceFormatPtr,
             size);

    return;
}

GT_BOOL cmdCpssRxPktIsCaptureReady(void)
{
    return (doCapture == GT_TRUE && wasInitDone == GT_TRUE) ? GT_TRUE : GT_FALSE;
}


static GT_STATUS cmdGenRxPktReceive
(
    IN GT_U8      devNum,
    IN GT_U8      queueIdx,
    IN GT_U32     numOfBuff,
    IN GT_U8     *packetBuffs[],
    IN GT_U32     buffLen[],
    IN void      *rxParamsPtr
)
{
    GT_STATUS rc;
    void  *tmpRxParamPtr;
    GT_U32 size = 0;

#if defined EXMXPM_FAMILY
    size = sizeof(CPSS_EXMXPM_NET_RX_PARAMS_STC);
#endif

#if defined CHX_FAMILY
    if(size < sizeof(CPSS_DXCH_NET_RX_PARAMS_STC))
       size = sizeof(CPSS_DXCH_NET_RX_PARAMS_STC);
#endif

#if defined EX_FAMILY
    if(size < sizeof(CPSS_EXMX_NET_RX_PARAMS_STC))
        size = sizeof(CPSS_EXMX_NET_RX_PARAMS_STC);
#endif

    /* If Rx to Tx disabled the function prvCpssGenNetIfRx2Tx returns
    without sending the packet. */
    prvCpssGenNetIfRx2Tx(packetBuffs,buffLen,numOfBuff);


    if(GT_FALSE == cmdCpssRxPktIsCaptureReady())
    {
        return GT_OK;
    }

    tmpRxParamPtr = osMalloc(size);
    if(tmpRxParamPtr == NULL)
    {
        return GT_OUT_OF_CPU_MEM;
    }

    osMemCpy(tmpRxParamPtr, rxParamsPtr, size);

    rc = cmdCpssRxPktReceive(devNum,queueIdx,tmpRxParamPtr,
                         &rxInfoFree,&rxInfoCopy,
                         numOfBuff,packetBuffs,buffLen);
    if(rc != GT_OK)
    {
        osFree(tmpRxParamPtr);
    }

    return rc;
}





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
static GT_STATUS exMxPmSendPacket
(
    GT_U32 index
)
{
    GT_U32 j;
    GT_U8  devNum;
    CPSS_EXMXPM_NET_TX_PARAMS_STC cpssPcktParams;
    GT_STATUS   status = GT_OK;

    osMemSet(&cpssPcktParams, 0, sizeof(cpssPcktParams));
    cpssPcktParams.packetIsTagged      = cpssTxPacketDescTbl[index].packetIsTagged;
    cpssPcktParams.cookie              = &txCookie;
    cpssPcktParams.sdmaInfo.evReqHndl  = evReqHndl;
    cpssPcktParams.sdmaInfo.recalcCrc  = cpssTxPacketDescTbl[index].recalcCrc;
    cpssPcktParams.sdmaInfo.txQueue    = cpssTxPacketDescTbl[index].txQueue;
    cpssPcktParams.sdmaInfo.invokeTxBufferQueueEvent = cpssTxPacketDescTbl[index].invokeTxBufferQueueEvent;


    cpssPcktParams.dsaParam.commonParams.dsaTagType = CPSS_EXMXPM_NET_DSA_TYPE_EXTENDED_E;

    cpssPcktParams.dsaParam.commonParams.vpt    = ((CPSS_EXMXPM_SPECIFIC_TX_PKT_INFO_STC *)(cpssTxPacketDescTbl[index].specificDeviceTypeInfo))->vpt;
    cpssPcktParams.dsaParam.commonParams.cfiBit = ((CPSS_EXMXPM_SPECIFIC_TX_PKT_INFO_STC *)(cpssTxPacketDescTbl[index].specificDeviceTypeInfo))->cfiBit;
    cpssPcktParams.dsaParam.commonParams.vid    = cpssTxPacketDescTbl[index].vid;

    /* This function treats only from CPU DSA type. */
    cpssPcktParams.dsaParam.dsaCmd = CPSS_EXMXPM_NET_DSA_CMD_FROM_CPU_E;

    cpssPcktParams.dsaParam.dsaInfo.fromCpu.tc          = cpssTxPacketDescTbl[index].trafficClass;
    cpssPcktParams.dsaParam.dsaInfo.fromCpu.dp          = cpssTxPacketDescTbl[index].dropPrecedence;
    cpssPcktParams.dsaParam.dsaInfo.fromCpu.egrFilterEn = ((CPSS_EXMXPM_SPECIFIC_TX_PKT_INFO_STC *)(cpssTxPacketDescTbl[index].specificDeviceTypeInfo))->egrFilterEn;
    cpssPcktParams.dsaParam.dsaInfo.fromCpu.packetType  = CPSS_EXMXPM_TS_PASSENGER_PACKET_TYPE_ETHERNET_E;
    cpssPcktParams.dsaParam.dsaInfo.fromCpu.egrFilterRegistered = ((CPSS_EXMXPM_SPECIFIC_TX_PKT_INFO_STC *)(cpssTxPacketDescTbl[index].specificDeviceTypeInfo))->egrFilterRegistered;
    cpssPcktParams.dsaParam.dsaInfo.fromCpu.srcId          = ((CPSS_EXMXPM_SPECIFIC_TX_PKT_INFO_STC *)(cpssTxPacketDescTbl[index].specificDeviceTypeInfo))->srcId;
    cpssPcktParams.dsaParam.dsaInfo.fromCpu.dstInterface = cpssTxPacketDescTbl[index].dstInterface;

    if( (cpssTxPacketDescTbl[index].dstInterface.type == CPSS_INTERFACE_VIDX_E) ||
        (cpssTxPacketDescTbl[index].dstInterface.type == CPSS_INTERFACE_VID_E)    )
    {
        cpssPcktParams.dsaParam.dsaInfo.fromCpu.extDestInfo.multiDest.excludeInterface =
            cpssTxPacketDescTbl[index].excludeInterface;
        cpssPcktParams.dsaParam.dsaInfo.fromCpu.extDestInfo.multiDest.excludedInterface =
            cpssTxPacketDescTbl[index].excludedInterface;
    }
    else/* CPSS_INTERFACE_PORT_E */
    {
        cpssPcktParams.dsaParam.dsaInfo.fromCpu.extDestInfo.devPort.dstIsTagged =
            cpssTxPacketDescTbl[index].dstIsTagged;
    }


    devNum = cpssTxPacketDescTbl[index].devNum;
    for (j = 0; j < cpssTxPacketDescTbl[index].numPacketsToSend; j++)
    {/*For packet sent by this descriptor*/
        /* If the transmition was disabled */

        if (flagStopTransmit == GT_TRUE)
        {
            break;
        }

        if(cpssTxPacketDescTbl[index].txSyncMode == GT_TRUE)
             {
                 status = cpssExMxPmNetIfSdmaTxSyncPacketSend(devNum, &cpssPcktParams, cpssTxPacketDescTbl[index].pcktData,
                                                cpssTxPacketDescTbl[index].pcktDataLen, cpssTxPacketDescTbl[index].numOfBuffers);
             }
             else
             {
                 status = cpssExMxPmNetIfSdmaTxPacketSend(devNum, &cpssPcktParams, cpssTxPacketDescTbl[index].pcktData,
                                                cpssTxPacketDescTbl[index].pcktDataLen, cpssTxPacketDescTbl[index].numOfBuffers);

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
            cpssTxPacketDescTbl[index].numSentPackets++;
        }

        if (status == GT_NO_RESOURCE)
        {
            /* packet wasn't transmitted */
            j--;
        }

        /* if we get GT_NO_RESOURCE and the sdmaTxPacketSendCount
           is bigger then MAX_NUM_OF_BUFFERS_AT_POOL
           it means no more buffer available in the FIFO */
        if ((status == GT_NO_RESOURCE)    ||
            ((cpssTxPacketDescTbl[index].txSyncMode==GT_FALSE) && (cpssTxPacketDescTbl[index].invokeTxBufferQueueEvent==GT_TRUE))
           )
        {
            GT_U8           devNum;
            GT_U8           queueIdx;
            GT_PTR          cookie;

            /* wait for the Tx-End event */
            status = cpssEventSelect(evReqHndl,NULL,NULL,0);

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
                    osPrintf("Number of sent packets NOT equal to number of packets get \n\n");
                    osPrintf("Total sent packets: %d, sdmaTxPacketSendCount: %d, sdmaTxPacketGetCount: %d\n",j,sdmaTxPacketSendCount,sdmaTxPacketGetCount);
                }

                sdmaTxPacketSendCount = 0;
                sdmaTxPacketGetCount = 0;

            }

        /* wait n milliseconds before sending next packet */
        if(cpssTxPacketDescTbl[index].gap != 0)
        {
            osTimerWkAfter(cpssTxPacketDescTbl[index].gap);
        }

    }

    return GT_OK;
}


#if (defined EXMXPM_FAMILY)
#include <cpss/exMxPm/exMxPmGen/networkIf/cpssExMxPmNetIf.h>

static  CPSS_EXMXPM_NET_RX_PARAMS_STC     pmRxParams;

/*******************************************************************************
* appDemoNetRxPktHandlePmPCI
*
* DESCRIPTION:
*       application routine to receive frames . -- Gen function
*
* INPUTS:
*       devNum      - Device number.
*       queueIdx    - The queue from which this packet was received.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       None.
*
* COMMENTS:
*     The user is responsible to free the gtBuf structures and rx data buffers.
*     The lport that needs to be transferred to the function netFreeRxGtbuf()
*     is the intLport and not srcLport.
*
*
*******************************************************************************/
static GT_STATUS rxTxNetRxPktHandlePmPCI
(
    IN    GT_U8      devNum,
    IN    GT_U8      queueIdx,
    INOUT GT_U32    *numOfBuffPtr,
    OUT   GT_U8     *packetBuffs[],
    OUT   GT_U32     buffLen[],
    OUT   void     **rxParams
)
{
    GT_STATUS                       rc;

    /* get the packet from the device */
    rc = cpssExMxPmNetIfSdmaRxPacketGet(devNum, queueIdx, numOfBuffPtr,
                        packetBuffs, buffLen, &pmRxParams);

    if (rc != GT_OK)  /* GT_NO_MORE is good and not an error !!! */
    {
        /* no need to free buffers because not got any */
        return rc;
    }
    *rxParams = &pmRxParams;




    /* call GalTis Agent for counting and capturing */
#ifdef IMPL_GALTIS
    /* ignore the return code from this function because we want to keep on
       getting RX buffers , regardless to the CMD manager */
    (void) cmdGenRxPktReceive(devNum,queueIdx, *numOfBuffPtr,
                              packetBuffs, buffLen, *rxParams);
#endif /* IMPL_GALTIS */


    /* now you need to free the buffers */
    rc = cpssExMxPmNetIfSdmaRxBufFree(devNum, queueIdx, packetBuffs, *numOfBuffPtr);

    return rc;
}
#endif


/**************** static func declaration *****************************/
 
static GT_STATUS prvUniEvMaskAllSet
(
    IN  CPSS_UNI_EV_CAUSE_ENT       cpssUniEventArr[],
    IN  GT_U32                      arrLength,
    IN  CPSS_EVENT_MASK_SET_ENT     operation
);
static GT_STATUS rxTxEnRxPacketGet
(
    IN GT_U8                devNum,
    IN GT_U8                queue
);
static GT_STATUS rxTxNetRxPktHandle
	(
		IN GT_U8                                devNum,
		IN GT_U8                                queueIdx
	);


static GT_STATUS rxTxEnPpEvTreat
(
    GT_U8                   devNum,
    GT_U32                  uniEv,
    GT_U32                  evExtData
);

/* Tx */
static GT_STATUS startPacketTransmission(void);

static unsigned __TASKCONV rxTxEventsHndlr(GT_VOID * param);
static unsigned __TASKCONV rxTxCmdTask(GT_VOID * param);



static CMD_RX_ENTRY *rxEntriesArr;
static GALTIS_RX_MODE_ENT rxTableMode;
static GT_U32  maxRxBufferSize;
static GT_U32  maxNumOfRxEntries;
static GT_U32  indexToInsertPacket;
static GT_U32  doCapture;
static GT_BOOL wasInitDone;


CPSS_RX_TX_MP_MODE_PARAMS_STC rxTxParams;


APP_DEMO_SYS_CONFIG_STC appDemoSysConfig = {
    GT_FALSE  /*cpuEtherPortUsed*/
};



APP_DEMO_PP_CONFIG appDemoPpConfigList[PRV_CPSS_MAX_PP_DEVICES_CNS];



int multiProcessAppDemo = 1;


GT_STATUS appDemoEvReqNodeGet_cpssEmulate
(
    IN GT_U8                   devNum,
    IN CPSS_UNI_EV_CAUSE_ENT         uniEvent,
    OUT EV_REQ_NODE        **evHndlPtr
);





/* by default the pointers set directly to the CPSS API */
/* but on some systems we need the dispatcher to handle */


APP_DEMO_CPSS_EVENT_BIND_FUNC            rxTxCpssEventBind = cpssEventBind;
APP_DEMO_CPSS_EVENT_SELECT_FUNC          rxTxCpssEventSelect = cpssEventSelect;
APP_DEMO_CPSS_EVENT_RECV_FUNC            rxTxCpssEventRecv = cpssEventRecv;
APP_DEMO_CPSS_EVENT_DEVICE_MASK_SET_FUNC rxTxCpssEventDeviceMaskSet = cpssEventDeviceMaskSet;




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
		osFree(specificDeviceFormatPtr);
	}
}

/*******************************************************************************
* rxTxEnPpEvTreat
*
* DESCRIPTION:
*       This routine handles PP events.
*
* INPUTS:
*       devNum      - the device number.
*       uniEv       - Unified event number
*       evExtData   - Unified event additional information
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK   - on success,
*       GT_FAIL - otherwise.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS rxTxEnPpEvTreat
(
GT_U8                   devNum,
GT_U32                  uniEv,
GT_U32                  evExtData
)
{
	GT_STATUS   rc = GT_OK;
	GT_U8       queue;


	/* Event handler */
	switch(uniEv)
	{
	case CPSS_PP_RX_BUFFER_QUEUE0_E:
	case CPSS_PP_RX_BUFFER_QUEUE1_E:
	case CPSS_PP_RX_BUFFER_QUEUE2_E:
	case CPSS_PP_RX_BUFFER_QUEUE3_E:
	case CPSS_PP_RX_BUFFER_QUEUE4_E:
	case CPSS_PP_RX_BUFFER_QUEUE5_E:
	case CPSS_PP_RX_BUFFER_QUEUE6_E:
	case CPSS_PP_RX_BUFFER_QUEUE7_E:


		queue = (GT_U8)(uniEv - CPSS_PP_RX_BUFFER_QUEUE0_E);
		/* implemented if defined DXCH_CODE || defined EXMXPM_FAMILY */
		rc = rxTxEnRxPacketGet(devNum, queue);
		break;

	default:
		break;
	}
	return rc;
}

/*******************************************************************************
* rxTxEventRequestDrvnModeInit
*
* DESCRIPTION:
*       This routine spawns the App Demo event handlers.
*
* INPUTS:
*       None.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK   - on success,
*       GT_FAIL - otherwise.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS rxTxEventRequestDrvnModeInit
(
IN GT_VOID
)
{
	GT_STATUS           rc;				 /* The returned code            */
	GT_TASK             eventHandlerTid; /* The task Tid                 */
	GT_U32                  i = 1;				 /* Iterator                     */
	GT_U8                       taskName[30];	 /* The task name                */
	GT_TASK         txPacketTransmitTid; /* The task Tid                 */


	GT_U32 evHndlrCauseArrSize = sizeof(evHndlrCauseArr)/sizeof(evHndlrCauseArr[0]);
	static EV_HNDLR_PARAM taskParamArr;

	/* bind the events for all the App-Demo event handlers */
	taskParamArr.hndlrIndex = i;

	/* call CPSS to bind the events under single handler */
	rc = rxTxCpssEventBind(evHndlrCauseArr,
					   evHndlrCauseArrSize,
					   &taskParamArr.evHndl);
	switch(rc)
	{
	case GT_FAIL:
		printf("General failure\n");
		break;
	case GT_BAD_PTR:
		printf("One of the parameters is NULL pointer\n");
		break;
	case GT_OUT_OF_CPU_MEM:
		printf("Failed to allocate CPU memory\n");
		break;
	case GT_FULL:
		printf("When trying to set the 'tx buffer queue unify event' \
(CPSS_PP_TX_BUFFER_QUEUE_E) with other events in the same handler\n");
		break;
	case GT_ALREADY_EXIST:
		printf("One of the unified events already bound to another \
handler (not applicable to CPSS_PP_TX_BUFFER_QUEUE_E)\n");
		break;
	}

	if(GT_OK != rc)
	{
		return GT_FAIL;
	}

	/* call the CPSS to enable those interrupts in the HW of the device */
	rc = prvUniEvMaskAllSet(evHndlrCauseArr,
							evHndlrCauseArrSize,
							CPSS_EVENT_UNMASK_E);
	if(GT_OK != rc)
	{
		return GT_FAIL;
	}

/* spawn all the event handler processes */

	osSprintf(taskName, "rxTxHndl");

	rc = osTaskCreate(taskName,
					  EV_HANDLER_MAX_PRIO,
					  _32KB,
					  rxTxEventsHndlr,
					  &taskParamArr,
					  &eventHandlerTid);
	if(rc != GT_OK)
	{
		return GT_FAIL;
	}


	osSprintf(taskName, "rxTxCmdTask");

	rc = osTaskCreate(taskName,
					  EV_HANDLER_MAX_PRIO,
					  _32KB,
					  rxTxCmdTask,
					  &taskParamArr,
					  &txPacketTransmitTid);
	if(rc != GT_OK)
	{
		return GT_FAIL;
	}


	return GT_OK;
}

/*******************************************************************************
* prvUniEvMaskAllSet
*
* DESCRIPTION:
*       This routine unmasks all the events according to the unified event list.
*
* INPUTS:
*       cpssUniEventArr - The CPSS unified event list.
*       arrLength   - The unified event list length.
*       operation   - type of operation mask/unmask to do on the events
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK on success, or
*       GT_FAIL if failed.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS prvUniEvMaskAllSet
(
IN  CPSS_UNI_EV_CAUSE_ENT       cpssUniEventArr[],
IN  GT_U32                      arrLength,
IN  CPSS_EVENT_MASK_SET_ENT     operation
)
{
	GT_STATUS rc = GT_OK;
	GT_U32    i;			  /* Iterator                     */
	GT_U8     dev;			  /* Device iterator              */
	GT_32     intKey;		  /* The interrupt lock key       */

	/* lock section to disable interruption of ISR while unmasking events */
	intKey = 0;
/*    osTaskLock(); 
	extDrvSetIntLockUnlock(INTR_MODE_LOCK, &intKey); */

	/* unmask the interrupt */
	for(i = 0; i < arrLength; i++)
	{
		if(cpssUniEventArr[i] >= CPSS_PP_UNI_EV_MIN_E &&
		   cpssUniEventArr[i] <= CPSS_PP_UNI_EV_MAX_E)
		{
			/* unmask the interrupt for all PPs */
			for(dev = 0; dev < _PRV_CPSS_MAX_PP_DEVICES_CNS; dev++)
			{
/*                if(appDemoPpConfigList[dev].valid == GT_FALSE)
				{
					continue;
				} */
				rc = rxTxCpssEventDeviceMaskSet(/*appDemoPpConfigList[dev].devNum*/ dev, cpssUniEventArr[i], operation);
				if(rc != GT_OK)
				{
					goto exit_cleanly_lbl;
				}
			}
		} else if(cpssUniEventArr[i] >= CPSS_XBAR_UNI_EV_MIN_E &&
				  cpssUniEventArr[i] <= CPSS_XBAR_UNI_EV_MAX_E)
		{
		} else if(cpssUniEventArr[i] >= CPSS_FA_UNI_EV_MIN_E &&
				  cpssUniEventArr[i] <= CPSS_FA_UNI_EV_MAX_E)
		{
		} else
		{
			rc = GT_FAIL;
			goto exit_cleanly_lbl;
		}
	}

	exit_cleanly_lbl:
/*    extDrvSetIntLockUnlock(INTR_MODE_UNLOCK, &intKey);
	osTaskUnLock();*/

	return rc;
} /* prvUniEvMaskAllSet */

/*******************************************************************************
* appDemoEvHndlr
*
* DESCRIPTION:
*       This routine is the event handler for PSS Event-Request-Driven mode
*       (polling mode).
*
* INPUTS:
*       param - The process data structure.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK   - on success,
*       GT_FAIL - otherwise.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static unsigned __TASKCONV rxTxEventsHndlr
(
GT_VOID * param
)
{
	GT_STATUS       rc;					/* return code         */
	GT_U32          i;						/* iterator            */
	GT_UINTPTR      evHndl;				/* event handler       */
	GT_U32          evBitmapArr[CPSS_UNI_EV_BITMAP_SIZE_CNS];	/* event bitmap array  */
	GT_U32          evBitmap;									/* event bitmap 32 bit */
	GT_U32          evExtData;									/* event extended data */
	GT_U8           devNum;										/* device number       */
	GT_U32          uniEv;										/* unified event cause */
	GT_U32          evCauseIdx;									/* event index         */
	EV_HNDLR_PARAM      *hndlrParamPtr;								/* bind event array    */

	hndlrParamPtr = (EV_HNDLR_PARAM*)param;
	evHndl        = hndlrParamPtr->evHndl;

	while(1)
	{
		rc = rxTxCpssEventSelect(evHndl, NULL, evBitmapArr, (GT_U32)CPSS_UNI_EV_BITMAP_SIZE_CNS);
		if(GT_OK != rc)
		{
			/* If seems like this result is not unusual... */
			/* DBG_LOG(("rxTxCpssEventSelect: err\n", 1, 2, 3, 4, 5, 6));*/
			continue;
		}

		for(evCauseIdx = 0; evCauseIdx < CPSS_UNI_EV_BITMAP_SIZE_CNS; evCauseIdx++)
		{
			if(evBitmapArr[evCauseIdx] == 0)
			{
				continue;
			}

			evBitmap = evBitmapArr[evCauseIdx];

			for(i = 0; evBitmap; evBitmap >>= 1, i++)
			{
				if((evBitmap & 1) == 0)
				{
					continue;
				}
				uniEv = (evCauseIdx << 5) + i;

				if((rc=rxTxCpssEventRecv(evHndl, uniEv, &evExtData, &devNum)) == GT_OK)
				{
					DBG_LOG(("cpssEventRecv: %08x <dev=%d, uniEv=%d(%s), extData=0x%0x>\n",
							 (GT_U32)hndlrParamPtr->hndlrIndex, devNum, uniEv,
							 (GT_U32)uniEvName[uniEv], evExtData, 6));


					/* 
					*    Get rxTxParams from CPSS Shared Lib buffer 
					*    These parameters set by appDemo process
					*/
					rc = cpssMultiProcComGetRxTxParams(&rxTxParams);

					if(rxTxParams.valid == GT_TRUE)
					{
						rxTxParams.valid = GT_FALSE;

						rxTxParams.doCapture = 1;

						rxEntriesArr = rxTxParams.rxEntriesArr;
						rxTableMode = rxTxParams.rxTableMode;
						maxRxBufferSize = rxTxParams.maxRxBufferSize; 
						maxNumOfRxEntries = rxTxParams.maxNumOfRxEntries;
						indexToInsertPacket = rxTxParams.indexToInsertPacket;
						doCapture = rxTxParams.doCapture;
						wasInitDone = rxTxParams.wasInitDone;

						rc = cpssMultiProcComSetRxTxParams(&rxTxParams);

					}

					/* Treat packet transmit */
					rc = rxTxEnPpEvTreat(devNum, uniEv, evExtData);

				} else
				{
					DBG_LOG(("cpssEventRecv: error %d\n", rc, 2, 3, 4, 5, 6));
				}
			}
		}
	}
}


/*******************************************************************************
* cpssEnRxPacketGet
*
* DESCRIPTION:
*       This routine handles the packet Rx event.
*
* INPUTS:
*       devNum  - the device number.
*       queue    - the queue the rx event occurred upon
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK   - on success,
*       GT_FAIL - otherwise.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS rxTxEnRxPacketGet
(
IN GT_U8                devNum,
IN GT_U8                queue
)
{
	GT_STATUS       rc = GT_NOT_IMPLEMENTED;

	do
	{

		/*new packet treatment*/
		rc = rxTxNetRxPktHandle(devNum, queue);

	}while(rc == GT_OK);

	return rc;
}


/*******************************************************************************
* appDemoDxChNetRxPktHandle
*
* DESCRIPTION:
*       application routine to receive frames . -- DXCH function
*
* INPUTS:
*       devNum      - Device number.
*       queueIdx    - The queue from which this packet was received.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       None.
*
* COMMENTS:
*     The user is responsible to free the gtBuf structures and rx data buffers.
*     The lport that needs to be transferred to the function netFreeRxGtbuf()
*     is the intLport and not srcLport.
*
*
*******************************************************************************/
#define BUFF_LEN    5


static GT_STATUS rxTxNetRxPktHandle
(
IN GT_U8                                devNum,
IN GT_U8                                queueIdx
)
{
	GT_STATUS                           rc;
	GT_U32                              numOfBuff = BUFF_LEN;
	GT_U8*                              packetBuffs[BUFF_LEN];
	GT_U32                              buffLenArr[BUFF_LEN];
	void                                 *rxParams;
	/*
	CPSS_DXCH_NET_RX_PARAMS_STC         rxParams;
*/
	/* get the packet from the device */
	if(appDemoPpConfigList[devNum].channel == CPSS_CHANNEL_PCI_E)
	{

#if defined DXCH_CODE
            rc = rxTxNetRxPktHandleChPCI(devNum, queueIdx, &numOfBuff,
                                            packetBuffs, buffLenArr, &rxParams);
#endif /*DXCH_CODE*/

#if defined PM_CODE
            rc = rxTxNetRxPktHandlePmPCI(devNum, queueIdx, &numOfBuff,
                                            packetBuffs, buffLenArr, &rxParams);
#endif /*PM_CODE*/

/*

		rc = cpssDxChNetIfSdmaRxPacketGet(devNum, queueIdx,&numOfBuff,
							packetBuffs,buffLenArr,&rxParams);
*/
	}
	else
	{
#ifdef APP_DEMO_CPU_ETH_PORT_MANAGE
        CPSS_TBD_BOOKMARK
        /* Shared Memory implementation should be updated to use native MII APIs. */
		APP_DEMO_ETH_PORT_NET_RX_PARAMS_STC appDemoRxParams;
			   
		rc = appDemoEthPrtEthPrtRxPacketGet(queueIdx,&numOfBuff,
							packetBuffs,buffLenArr,&appDemoRxParams);

		osMemCpy(&rxParams,&appDemoRxParams.dsa.dxChDsaParam,
				 sizeof(CPSS_DXCH_NET_RX_PARAMS_STC));
#else  /*APP_DEMO_CPU_ETH_PORT_MANAGE*/
		rc = GT_NOT_IMPLEMENTED;
#endif /*APP_DEMO_CPU_ETH_PORT_MANAGE*/
	}

	if (rc != GT_OK)  /* GT_NO_MORE is good and not an error !!! */
	{
		/* no need to free buffers because not got any */
		return rc;
	}

	return rc;
}


/*******************************************************************************
* cmdCpssRxPktReceive
*
* DESCRIPTION:
*       Receive packet callback function . the caller wants to register its
*       packet info with the manager array.
*
*       This function give services to all the rx packets info formats.
*
* INPUTS:
*      devNum  - The device number in which the packet was received.
*      queue   - The Rx queue in which the packet was received.
*      specificDeviceFormatPtr - (pointer to) the specific device Rx info format.
*                             format of DXCH / EXMX /DXSAL ...
*                             NOTE : this pointer is allocated by the specific
*                             device "C" file.
*      freeRxInfoFunc - callback function to free the specific rx info format,
*      copyRxInfoFunc - callback function to copy the specific rx info format,
*      numOfBuff    - Num of used buffs in packetBuffs
*      packetBuffs  - The received packet buffers list
*      buffLen      - List of buffer lengths for packetBuffs
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK - on success
*       GT_FULL - when buffer is full
*       GT_BAD_STATE - the galtis-cmd not ready/set to "save" the packet with info
*       GT_BAD_PARAM - on bad parameter
*       GT_OUT_OF_CPU_MEM - on CPU memory allocation failure
* COMMENTS:
*
* GalTis:
*       None
*
*******************************************************************************/
GT_STATUS cmdCpssRxPktReceive
(
IN GT_U8           devNum,
IN GT_U8           queue,
IN void*           specificDeviceFormatPtr,
IN FREE_RX_INFO_FUNC freeRxInfoFunc,
IN COPY_RX_INFO_FUNC copyRxInfoFunc,
IN GT_U32          numOfBuff,
IN GT_U8           *packetBuffs[],
IN GT_U32          buffLen[]
)
{
	GT_STATUS   rc;
	GT_U32 tmpBuffSize;
	GT_U32 toCopyBufSize;
	GT_U32 rxPcktBufSize;
	GT_U32 ii;

	if(doCapture != GT_TRUE)
		return GT_BAD_STATE;

	if(wasInitDone == GT_FALSE)
		return GT_BAD_STATE;


	if((rxTableMode == GALTIS_RX_ONCE_E && indexToInsertPacket < maxNumOfRxEntries - 1)
	   || (rxTableMode == GALTIS_RX_CYCLIC_E))
	{
		if(rxEntriesArr[indexToInsertPacket].isEmpty != GT_TRUE)
		{
			rxInfoFree(rxEntriesArr[indexToInsertPacket].specificDeviceFormatPtr);
		}


		/* get the packet actual length */
		rxPcktBufSize = 0;
		for(ii = 0; ii < numOfBuff; ii++)
		{
			rxPcktBufSize += buffLen[ii];
		}

		/* set the number of buffer bytes to copy */
		if(rxPcktBufSize > maxRxBufferSize)
		{
			/* packet length is bigger than destination buffer size. */
			toCopyBufSize = maxRxBufferSize;
		} else
		{
			toCopyBufSize = rxPcktBufSize;
		}

		/* allocate memory for the packet data */
		rxEntriesArr[indexToInsertPacket].packetBufferPtr = osMalloc(toCopyBufSize +1);

		if(NULL == rxEntriesArr[indexToInsertPacket].packetBufferPtr)
		{
			return GT_OUT_OF_CPU_MEM;
		}

		rxEntriesArr[indexToInsertPacket].isEmpty = GT_FALSE;
		rxEntriesArr[indexToInsertPacket].devNum = devNum;
		rxEntriesArr[indexToInsertPacket].queue = queue;
		rxEntriesArr[indexToInsertPacket].specificDeviceFormatPtr = specificDeviceFormatPtr;
		rxEntriesArr[indexToInsertPacket].freeRxInfoFunc = freeRxInfoFunc;
		rxEntriesArr[indexToInsertPacket].copyRxInfoFunc = copyRxInfoFunc;
		rxEntriesArr[indexToInsertPacket].packetOriginalLen = rxPcktBufSize;
		rxEntriesArr[indexToInsertPacket].bufferLen = toCopyBufSize + 1;

		osMemSet(rxEntriesArr[indexToInsertPacket].packetBufferPtr,0,toCopyBufSize +1);

		tmpBuffSize = 0;
		/* copy packet data to allocated buffer */
		for(ii = 0; ii < numOfBuff; ii++)
		{
			if(tmpBuffSize + buffLen[ii] > toCopyBufSize)
			{
				/* packet length is bigger than destination buffer size */
				osMemCpy(rxEntriesArr[indexToInsertPacket].packetBufferPtr + tmpBuffSize,
						 packetBuffs[ii], toCopyBufSize - tmpBuffSize);
				break;
			}
			/* copy the packet buffer to destination buffer */
			osMemCpy(rxEntriesArr[indexToInsertPacket].packetBufferPtr + tmpBuffSize,
					 packetBuffs[ii], buffLen[ii]);
			tmpBuffSize += buffLen[ii];
		}

		if((indexToInsertPacket == maxNumOfRxEntries -1))
		{
			indexToInsertPacket = 0;
		} else
		{
			indexToInsertPacket++;
		}
		rc = GT_OK;
	} else
	{
		rc = GT_FULL;
	}

	return rc;
}

/************************                TX                   ***************************/



static GT_STATUS __TASKCONV cpssPacketTransminitionTask()
{
    GT_STATUS rc = GT_OK;

    /* while forever */
    while(1)
    {
        /* wait on the TX command semaphore */
        taskBusyFlag = GT_FALSE;
        osSemWait(txCmdSemId, OS_WAIT_FOREVER);
        taskBusyFlag = GT_TRUE;

    	rc = startPacketTransmission();
    }
}

/*******************************************************************************
* rxTxCpssExMxPmTxStart
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
*******************************************************************************/
static GT_STATUS rxTxCpssExMxPmTxStart
(
)
{
    GT_STATUS rc = GT_OK;
    static GT_BOOL   enterOnce = GT_FALSE;
	  
    /* check if there are element in the transmit table */
    if(txTblCapacity == 0)
        return GT_OK;

    /* creat the task only once */
    if(enterOnce == GT_FALSE)
    {

        if(osSemBinCreate("RxTxTransmit",OS_SEMB_EMPTY,&txCmdSemId) != GT_OK)
        {
            return GT_FAIL;
        }
        if(osTaskCreate("RxTxPktTx",                          /* Task Name                      */
                        EV_HANDLER_MAX_PRIO,                  /* Task Priority                  */
                        0xa000,                               /* Stack Size _40KB               */
                        (unsigned (__TASKCONV *)(void*))cpssPacketTransminitionTask, /* Starting Point */
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
    osSemSignal(txCmdSemId);

    return  rc;
}

/*******************************************************************************
* rxTxCpssDxChTxStop
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
*******************************************************************************/
static GT_STATUS rxTxCpssDxChTxStop
(
    void
)
{
    /* there is no send task running */
    if(taskId == 0)
    {
        return GT_NO_CHANGE;
    }

    flagStopTransmit = GT_TRUE;
    return GT_OK;
}

/*******************************************************************************
* rxTxCmdTask
*
* DESCRIPTION:
*       
*       
* INPUTS:
*       param - The process data structure.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK   - on success,
*       GT_FAIL - otherwise.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static unsigned __TASKCONV rxTxCmdTask
(
GT_VOID * param
)
{
	GT_STATUS           rc;											/* return code         */
    CPSS_MP_REMOTE_COMMAND cmd;

	while(cpssMultiProcComWait(CPSS_MP_REMOTE_RXTX_E, &cmd) == GT_OK)
	{
        switch (cmd.command)
        {
            case CPSS_MP_CMD_RXTX_RXPKT_CLEAR_TBL_E:
                indexToInsertPacket = 0;
                cpssMultiProcComComplete(&cmd, GT_OK);
                break;
		case CPSS_MP_CMD_RXTX_START_TRANSMIT_E:

		/*
                cpssTxPacketDescTbl = cmd.data.rxTxStartTransmit.cpssDxChTxPacketDescTbl;
			*/

				cpssTxPacketDescTbl = cmd.data.rxTxStartTransmit.cpssExMxPmTxPacketDescTbl;

				/*
                txTblCapacity = cmd.data.rxTxStartTransmit.txDxChTblCapacity;
				*/
				txTblCapacity = cmd.data.rxTxStartTransmit.txExMxPmTxTblCapacity;
				
                modeSettings.cyclesNum = cmd.data.rxTxStartTransmit.modeSettings.cyclesNum;
                modeSettings.gap = cmd.data.rxTxStartTransmit.modeSettings.gap;
#ifdef APP_DEMO_CPU_ETH_PORT_MANAGE
        CPSS_TBD_BOOKMARK
        /* Shared Memory implementation should be updated to use native MII APIs. */
                poolArrPtr = (GT_ETH_PORT_POOL_HEAD *)cmd.data.rxTxStartTransmit.txPoolPtr;
#endif
                printf("TX_PROCESS_START : DescTbl - %x  cyc - %d gap - %d cap - %d\n",
                       cpssTxPacketDescTbl,
                       modeSettings.cyclesNum,
                       modeSettings.gap,
                       txTblCapacity);
                rc = rxTxCpssExMxPmTxStart();
                cpssMultiProcComComplete(&cmd, rc);
                break;
            case CPSS_MP_CMD_RXTX_STOP_TRANSMIT_E:
                rc = rxTxCpssDxChTxStop();
                cpssMultiProcComComplete(&cmd, rc);
                break;
            default:
                cpssMultiProcComComplete(&cmd, GT_FAIL);

        }
	}
	return GT_OK;
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
    GT_U32      cycles, pcktNum;
    GT_STATUS   status = GT_OK;
    GT_BOOL     transmitted;


    if (evReqHndl == 0)
    {
        CPSS_UNI_EV_CAUSE_ENT     evCause[1] = { CPSS_PP_TX_BUFFER_QUEUE_E };

        if (rxTxCpssEventBind(evCause, 1, &evReqHndl) != GT_OK)
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
            status = rxTxCpssEventDeviceMaskSet((GT_U8)i,CPSS_PP_TX_BUFFER_QUEUE_E,CPSS_EVENT_UNMASK_E);
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
    pcktNum = 0;
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

            if (GT_TRUE != cpssTxPacketDescTbl[i].valid)
            {
                /* entry is not valid */
                continue;
            }

            if(CPSS_IS_EXMXPM_FAMILY_MAC(PRV_CPSS_PP_MAC(cpssTxPacketDescTbl[i].devNum)->devFamily))
            {
                status = exMxPmSendPacket(i);
            }
            else
            {
                status = GT_FAIL;
            }

            if (status != GT_OK)
            {
				osPrintf("ERROR-in transmitting packets\n\n");
                return GT_FAIL;
            }

            transmitted = GT_TRUE;

            /* wait n milliseconds before moving to the next entry */
            if(cpssTxPacketDescTbl[i].waitTime != 0)
            {
				osTimerWkAfter(cpssTxPacketDescTbl[i].waitTime);
            }
        }

        /* No Packets to send */
        if(transmitted == GT_FALSE)
            break;

        /* wait n milliseconds before starting the next cycle */
        if(modeSettings.gap != 0)
        {
			osTimerWkAfter(modeSettings.gap);
        }
        /* move to the next cycle */
        cycles++;
    }

    return GT_OK;
}

/****************************************************
* entryPoint:
*             - run event Handler
*****************************************************/

static void entryPoint(void)
{

	appDemoPpConfigList[0].channel = CPSS_CHANNEL_PCI_E;

	if(rxTxEventRequestDrvnModeInit() == GT_FAIL)
	{
		fprintf(stderr, "rxTxEventRequestDrvnModeInit() error\n");
        osStopEngine();
		return;
	}

}

/****************************************************
*	Main function :
*            - init of CPSS Shared Lib 
*            - start mainOs engine
*****************************************************/

int main(int argc, const char * argv[])
{
    setbuf(stdout, NULL);

	if(shrMemInitSharedLibrary() != GT_OK)
	{
		fprintf(stderr, "shrMemInitSharedLibrary() failed\n");
		return -1;
	}
	shrMemPrintMapsDebugInfo();

    if (osStartEngine(argc, argv, "rxTxProcess", entryPoint) != GT_OK)
        return 1;

	return 0;
}



