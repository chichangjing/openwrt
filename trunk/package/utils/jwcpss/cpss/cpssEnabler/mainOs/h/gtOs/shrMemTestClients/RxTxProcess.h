/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* RxTxProcess.h
*
* DESCRIPTION:
*       This file contains function declarations, typedefs and defines for RxTxProcess.c 
*       
*       
*
* FILE REVISION NUMBER:
*       $Revision: 10 $
*
*******************************************************************************/
#ifndef __RxTxProcessh
#define __RxTxProcessh

#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <gtOs/gtOsSharedMalloc.h>
#include <gtOs/gtOsSharedMemory.h>
#include <gtOs/gtOsMem.h>
#include <gtOs/gtOsIo.h>
#include <cpss/generic/events/cpssGenEventRequests.h>
#include <cpss/generic/events/private/prvCpssGenEvReq.h>
#include <gtExtDrv/drivers/gtIntDrv.h>
#include <cpss/generic/config/private/prvCpssConfigTypes.h>
#include <cpssCommon/cpssPresteraDefs.h>

#ifdef CHX_FAMILY
#include <cpss/dxCh/dxChxGen/networkIf/cpssDxChNetIfTypes.h>
#endif
#include <cpss/driver/interrupts/cpssDrvComIntEvReqQueues.h>

#include <cpss/generic/events/cpssGenEventRequests.h>
#include <cpss/generic/events/private/prvCpssGenEvReq.h>
#include <gtExtDrv/drivers/gtEthPortCtrl.h>

#ifdef CHX_FAMILY
#include <cpss/dxCh/dxChxGen/cpssHwInit/cpssDxChHwInit.h>
#endif
#if 0
#include <appDemo/userExit/userEventHandler.h>
#endif



#ifndef IMPL_GALTIS
#define IMPL_GALTIS
#endif


#define EV_HANDLER_MAX_PRIO     200
#define     NUM_OF_INT_QUEUES           8

#define TAKE_TX_SEM(useSem, id) if (useSem) osSemWait(id, OS_WAIT_FOREVER);
#define GIVE_TX_SEM(useSem, id) if (useSem) osSemSignal(id);

#define ALIGN_4B_MASK_CNS   0x3
#define ALIGN_4B_CNS        4

#define LOCK(semId)     osSemWait(semId, OS_WAIT_FOREVER);
#define UNLOCK(semId)   osSemSignal(semId);


/* Suppose to have AT LEAST 1 valid device! */
#define _PRV_CPSS_MAX_PP_DEVICES_CNS 1

#ifdef RXTX_REQ_MODE_DEBUG
  static char * uniEvName[CPSS_UNI_EVENT_COUNT_E] = {UNI_EV_NAME};
  #ifdef ASIC_SIMULATION
    #define DBG_LOG(x)  osPrintf x
  #else
    extern int  logMsg (char *fmt, int arg1, int arg2, int arg3,
                        int arg4, int arg5, int arg6);
    #define DBG_LOG(x)  osPrintf x
  #endif
  #define DBG_INFO(x)   osPrintf x
#else
  #define DBG_INFO(x)
  #define DBG_LOG(x)
#endif


/* event array */
CPSS_UNI_EV_CAUSE_ENT       evHndlrCauseArr[] =  
	{
        CPSS_PP_RX_BUFFER_QUEUE0_E, \
        CPSS_PP_RX_BUFFER_QUEUE1_E, \
        CPSS_PP_RX_BUFFER_QUEUE2_E, \
        CPSS_PP_RX_BUFFER_QUEUE3_E, \
        CPSS_PP_RX_BUFFER_QUEUE4_E, \
        CPSS_PP_RX_BUFFER_QUEUE5_E, \
        CPSS_PP_RX_BUFFER_QUEUE6_E, \
        CPSS_PP_RX_BUFFER_QUEUE7_E
	};

/**************** type defs *****************************/

	typedef struct
	{
		GT_UINTPTR          evHndl;
		GT_U32              hndlrIndex;
	} EV_HNDLR_PARAM;

typedef struct BufHead
{
    struct BufHead  *pNext;

}GT_ETH_PORT_POOL_FREE_BUF_HEAD;


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


GT_STATUS rxTxEthPrtInitNetIfDev
(
    void
);
GT_STATUS extDrvEthPortTxInit
(
    IN GT_U32        numOfTxBufs
);

GT_STATUS extDrvEthPortTx
(
    IN GT_U8_PTR        segmentList[],
    IN GT_U32           segmentLen[],
    IN GT_U32           numOfSegments,
    IN GT_U32           txQueue
);

GT_STATUS extDrvEthPortRxCb
(
    IN GT_U8_PTR        segmentList[],
    IN GT_U32           segmentLen[],
    IN GT_U32           numOfSegments,
    IN GT_U32           queueNum
);

GT_STATUS extDrvEthPortTxEndCb
(
    IN GT_U8_PTR        segmentList[],
    IN GT_U32           numOfSegments
);


GT_STATUS extDrvEthRxPacketFree
(
    IN GT_U8_PTR        segmentList[],
    IN GT_U32           numOfSegments,
    IN GT_U32           queueNum
);


GT_STATUS interruptEthPortRxSR
(
    IN GT_U8_PTR        segmentList[],
    IN GT_U32           segmentLen[],
    IN GT_U32           numOfSegments,
    IN GT_U32           rxQueue
);

GT_STATUS interruptEthPortTxEndSR
(
    IN GT_U8_PTR        segmentList[],
    IN GT_U32           numOfSegments
);


GT_VOID shrMemPrintMapsDebugInfo(GT_VOID);


#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */


#ifdef __cplusplus
}
#endif  /* __cplusplus */

#endif /* __RxTxProcessh */
