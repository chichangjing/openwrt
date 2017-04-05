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
* userEventHandler.c
*
* DESCRIPTION:
*       This module defines the Application Demo requested event (polling) mode
*       process creation and event handling.
*
* DEPENDENCIES:
*
* FILE REVISION NUMBER:
*       $Revision: 50 $
*******************************************************************************/

#include <gtOs/gtOsExc.h>
#include <gtOs/gtOsMsgQ.h>

#include <appDemo/os/appOs.h>
#include <appDemo/userExit/userEventHandler.h>
#include <appDemo/sysHwConfig/appDemoDb.h>
#include <appDemo/sysHwConfig/gtAppDemoSysConfig.h>

#include <cpssCommon/cpssPresteraDefs.h>
#include <cpss/generic/events/private/prvCpssGenEvReq.h>
#include <cpss/generic/config/private/prvCpssConfigTypes.h>

#include <cpssDriver/pp/interrupts/dxExMx/prvCpssDrvDxExMxInterrupts.h>

#include <gtExtDrv/drivers/gtIntDrv.h>
#include <extUtils/rxEventHandler/rxEventHandler.h>
#include <extUtils/auEventHandler/auEventHandler.h>

#include <cmdShell/cmdDb/cmdBase.h>

#include <cpss/dxCh/dxChxGen/port/macCtrl/prvCpssDxChGEMacCtrl.h>

#include <cpss/dxCh/dxChxGen/port/cpssDxChPortAp.h>

#ifdef SHARED_MEMORY

        extern int         multiProcessAppDemo;

#endif

/* debug flag to open trace of events */
static GT_U32 wrapCpssTraceEvents = 0;
/* indication the we do 'system reset' */
static GT_U32   eventRequestDrvnModeReset = 0;
#define FREE_TASK_CNS   0xFFFFFFFF
GT_U32   *uniEventCounters[MAX_NUM_DEVICES];
GT_UINTPTR rxEventHanderHnd = 0;
GT_UINTPTR auEventHandlerHnd = 0;

/* event handler counter update protection */
static CPSS_OS_MUTEX appDemoEventHandlerCounterUpdateLockMtx;

/* global variables used in single Rx task mode */
/* is single Rx task mode enabled */
static GT_BOOL singleTaskRxEnable = GT_FALSE;
/* is random distribution of packets to msgQs enabled */
static GT_BOOL singleTaskRxRandomEnable = GT_FALSE;
/* number of treat Rx tasks */
static GT_U32  treatTasksNum = 2;
static CPSS_OS_MSGQ_ID singleRxMsgQIds[8];
CPSS_OS_MUTEX rxMutex;
/* treat RX task id array for reset - size should be equal to "treatTasksNum" */
GT_TASK treatRxEventHandlerTid[2];

/* task id array for reset */
GT_TASK eventHandlerTidArr[APP_DEMO_PROCESS_NUMBER];

#ifdef EX_FAMILY
#include <cpss/exMx/exMxGen/bridge/cpssExMxBrgFdb.h>
#endif

#if defined(CHX_FAMILY) && defined(IMPL_GALTIS)
#include <galtisAgent/wrapCpss/dxCh/diag/wrapCpssDxChDiagDataIntegrity.h>
#endif /* defined(CHX_FAMILY) && defined(IMPL_GALTIS) */

/* The next define can be uncommented for debug use only !! */
/* #define APPDEMO_REQ_MODE_DEBUG */

#ifdef APPDEMO_REQ_MODE_DEBUG
  static GT_CHAR * uniEvName[CPSS_UNI_EVENT_COUNT_E] = {UNI_EV_NAME};
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
/* indication that task created */
static GT_U32   taskCreated = 0;
/* Local routines */
static unsigned __TASKCONV appDemoEvHndlr(GT_VOID * param);
static unsigned __TASKCONV appDemoSingleRxEvHndlr(GT_VOID * param);
static unsigned __TASKCONV appDemoSingleRxTreatTask(GT_VOID * param);
#ifdef GM_USED
static unsigned __TASKCONV gm_appDemoEmulateLinkChangeTask(GT_VOID * param);
#endif /*GM_USED*/

/* Local Defines */
#define EV_HANDLER_MAX_PRIO 200

#if (defined CHX_FAMILY)
    #define DXCH_CODE
#endif /* (defined CHX_FAMILY) */

#if (defined EXMXPM_FAMILY)
    #define PM_CODE
#endif /* (defined EXMXPM_FAMILY) */

#ifdef DXCH_CODE
#include <cpss/dxCh/dxChxGen/networkIf/cpssDxChNetIf.h>
#include <cpss/dxCh/dxChxGen/cpssHwInit/cpssDxChHwInit.h>
#include <cpss/dxCh/dxChxGen/vnt/cpssDxChVnt.h>
#include <appDemo/userExit/dxCh/appDemoDxChEventHandle.h>

#include <cpss/dxCh/dxChxGen/port/cpssDxChPortCtrl.h>
#include <cpss/dxCh/dxChxGen/config/private/prvCpssDxChInfo.h>

#include <cpss/dxCh/dxChxGen/diag/cpssDxChDiagDataIntegrity.h>
#include <cpss/dxCh/dxChxGen/config/private/prvCpssDxChInfoEnhanced.h>

#endif /*DXCH_CODE*/

#ifdef PM_CODE
#include <cpss/exMxPm/exMxPmGen/networkIf/cpssExMxPmNetIf.h>
#include <cpss/exMxPm/exMxPmGen/port/cpssExMxPmPortCtrl.h>

GT_STATUS appDemoExMxPmNetRxPktHandle(
    IN GT_UINTPTR                           evHandler,
    IN RX_EV_HANDLER_EV_TYPE_ENT            evType,
    IN GT_U8 devNum,
    IN GT_U8 queueIdx);
GT_STATUS appDemoExMxPmNetRxPktGet(IN GT_U8 devNum,
                                   IN GT_U8 queueIdx,
                                   IN CPSS_OS_MSGQ_ID msgQId);
GT_STATUS appDemoExMxPmNetRxPktTreat(IN APP_DEMO_RX_PACKET_PARAMS *rxParamsPtr);


extern GT_STATUS appDemoExMxPmPuma3PortLinkWa
(
    IN   GT_U8  devNum,
    IN   GT_U8  portNum
);

GT_STATUS appDemoExMxPmNetRxPacketCbRegister
(
    IN  RX_PACKET_RECEIVE_CB_FUN  rxPktReceiveCbFun
);
#endif /*PM_CODE*/

extern GT_STATUS AN_WA_Task_SyncChangeEventBuildAndSend
(
    GT_U8    devNum,
    GT_U32   portMacNum
);

extern GT_STATUS AN_WA_Task_AnCompleteEventBuildAndSend
(
    GT_U8    devNum,
    GT_U32   portMacNum
);


extern GT_STATUS appDemoGenNetRxPktHandle(IN GT_U8 devNum,
                                          IN GT_U8 queueIdx);

extern GT_STATUS cpssEnGenAuMsgHandle(IN GT_UINTPTR auEvHandler,
                                      IN GT_U8  devNum,
                                      IN GT_U32 evExtData);

/*
 * typedef: struct EV_HNDLR_PARAM
 *
 * Description:
 *
 * Fields:
 *   evHndl         - The process unified event handle (got from appDemoCpssEventBind).
 *   hndlrIndex     - The process number
 */
typedef struct
{
    GT_UINTPTR          evHndl;
    GT_U32              hndlrIndex;
} EV_HNDLR_PARAM;




static GT_STATUS prvUniEvMaskAllSet
(
    IN  CPSS_UNI_EV_CAUSE_ENT       cpssUniEventArr[],
    IN  GT_U32                      arrLength,
    IN  CPSS_EVENT_MASK_SET_ENT     operation
);

static GT_STATUS cpssEnRxPacketGet
(
    IN RX_EV_HANDLER_EV_TYPE_ENT evType,
    IN GT_U8                devNum,
    IN GT_U8                queue
);
static GT_STATUS appDemoGetMaxPackets
(
    IN GT_U8                devNum,
    IN GT_U8                queue,
    IN GT_U32               maxPacket
);


/* Event Counter increment function */
EVENT_COUNTER_INCREMENT_FUNC *eventIncrementFunc = NULL;


#if defined(DXCH_CODE) && defined(IMPL_GALTIS)
/* Event Counter increment function. Implemented in Galtis*/
DXCH_DATA_INTEGRITY_EVENT_COUNTER_INCREMENT_FUNC *dxChDataIntegrityEventIncrementFunc = NULL;
#endif /* defined(DXCH_CODE) && defined(IMPL_GALTIS) */

#ifdef DXCH_CODE
extern GT_U32   trainingTrace;/* option to disable the print in runtime*/
#define TRAINING_DBG_PRINT_MAC(x) if(trainingTrace) cpssOsPrintSync x
#ifndef ASIC_SIMULATION
CPSS_OS_SIG_SEM tuneSmid = 0; /* semaphore to manage serdes tuning selector task */
CPSS_PORTS_BMP_STC todoTuneBmp; /* bitmap of ports to run tuning on */
extern unsigned __TASKCONV appDemoDxChLion2SerdesAutoTuningStartTask
(
    GT_VOID * param
);
extern unsigned __TASKCONV lion2WaTask
(
    GT_VOID * param
);
extern CPSS_OS_SIG_SEM waTSmid;
extern CPSS_PORTS_BMP_STC todoWaBmp; /* bitmap of ports to run WA's on */
extern GT_U32 locksTimeoutArray[CPSS_MAX_PORTS_NUM_CNS];
#endif /* ndef ASIC_SIMULATION */
#endif /* DXCH_CODE */


/* flag to enable the printings of 'link change' --
    by default not enabled because it may cause RDE tests to fail due to the
    printings , tests that cause link change.
*/
#ifdef ASIC_SIMULATION
static GT_U32   printLinkChangeEnabled = 1;
#else  /*ASIC_SIMULATION*/
static GT_U32   printLinkChangeEnabled = 0;
#endif /*ASIC_SIMULATION*/

static GT_U32   printEeeInterruptInfo = 0;/* WM not supports this interrupt */


/* flag that state the tasks may process events that relate to AUQ messages.
    this flag allow us to stop processing those messages , by that the AUQ may
    be full, or check AU storm prevention, and other.

    use function appDemoAllowProcessingOfAuqMessages(...) to set this flag
*/
static GT_BOOL  allowProcessingOfAuqMessages = GT_TRUE;

/* The interrupt locking is needed for mutual exclusion prevention between   */
/* ISR and user task on the event queues.                                   */

#if !defined(_linux) && !defined(_FreeBSD)

#define INT_LOCK(key)                                                   \
    key = 0;                                                            \
    extDrvSetIntLockUnlock(INTR_MODE_LOCK, &(key))

#define INT_UNLOCK(key)                                                 \
    extDrvSetIntLockUnlock(INTR_MODE_UNLOCK, &(key))

#else /* Linux and FreeBSD */

#define INT_LOCK(key)                                                   \
    key = 0;                                                            \
    osTaskLock();                                                       \
    extDrvSetIntLockUnlock(INTR_MODE_LOCK, &(key))

#define INT_UNLOCK(key)                                                 \
    extDrvSetIntLockUnlock(INTR_MODE_UNLOCK, &(key));                   \
    osTaskUnLock()

#endif /* !defined(_linux) && !defined(_FreeBSD) */


/* fatal error handling type */
static APP_DEMO_FATAL_ERROR_TYPE prvAppDemoFatalErrorType = APP_DEMO_FATAL_ERROR_EXECUTE_TYPE_E;

/* flag meaning appDemoEnPpEvTreat will catch <Signal Detect triggered> interrupt.
Warning: <Signal Detect triggered> is new interrupt in XCAT2 B1, if enabled using
    cpssDxChPortGePrbsIntReplaceEnableSet it overrides old but not obsolete
    <QSGMII PRBS error> interrupt, so in appDemoEnPpEvTreat it's treated as
    CPSS_PP_PORT_PRBS_ERROR_QSGMII_E case */
GT_BOOL catchSignalDetectInterrupt = GT_FALSE;

/*******************************************************************************
* appDemoPrintLinkChangeFlagSet
*
* DESCRIPTION:
*       function to allow set the flag of : printLinkChangeEnabled
*
* INPUTS:
*       enable - enable/disable the printings of 'link change'
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK   - on success
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS appDemoPrintLinkChangeFlagSet
(
    IN GT_U32   enable
)
{
    printLinkChangeEnabled = enable;
    return GT_OK;
}
/*******************************************************************************
* appDemoPrintPortEeeInterruptInfoSet
*
* DESCRIPTION:
*       function to allow set the flag of : printEeeInterruptInfo
*
* INPUTS:
*       enable - enable/disable the printings of ports 'EEE interrupts info'
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK   - on success
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS appDemoPrintPortEeeInterruptInfoSet
(
    IN GT_U32   enable
)
{
    printEeeInterruptInfo = enable;
    return GT_OK;
}

/*******************************************************************************
* appDemoAllowProcessingOfAuqMessages
*
* DESCRIPTION:
*       function to allow set the flag of : allowProcessingOfAuqMessages
*
* INPUTS:
*       enable - enable/disable the processing of the AUQ messages
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK   - on success
*
* COMMENTS:
*    flag that state the tasks may process events that relate to AUQ messages.
*    this flag allow us to stop processing those messages , by that the AUQ may
*    be full, or check AU storm prevention, and other.
*
*******************************************************************************/
GT_STATUS   appDemoAllowProcessingOfAuqMessages
(
    GT_BOOL     enable
)
{
    GT_U8   dev;

    if(enable == GT_TRUE && allowProcessingOfAuqMessages == GT_FALSE)
    {
        /* move from no learn to learn */
        /* start processing the messages stuck in the queue */

        for(dev = SYSTEM_DEV_NUM_MAC(0); dev < SYSTEM_DEV_NUM_MAC(appDemoPpConfigDevAmount); dev++)
        {
            if(appDemoPpConfigList[dev].valid == GT_FALSE)
            {
                continue;
            }

            cpssEnGenAuMsgHandle(auEventHandlerHnd,appDemoPpConfigList[dev].devNum,
                0);/*unused param*/
        }
    }

    allowProcessingOfAuqMessages = enable;

    return GT_OK;
}

static CPSS_UNI_EV_CAUSE_ENT evHndlr0CauseDefaultArr[] = APP_DEMO_P0_UNI_EV_DEFAULT;
static CPSS_UNI_EV_CAUSE_ENT evHndlr1CauseDefaultArr[] = APP_DEMO_P1_UNI_EV_DEFAULT;
static CPSS_UNI_EV_CAUSE_ENT evHndlr2CauseDefaultArr[] = APP_DEMO_P2_UNI_EV_DEFAULT;
static CPSS_UNI_EV_CAUSE_ENT evHndlr3CauseDefaultArr[] = APP_DEMO_P3_UNI_EV_DEFAULT;
static CPSS_UNI_EV_CAUSE_ENT evHndlr4CauseDefaultArr[] = APP_DEMO_P4_UNI_EV_DEFAULT;
static CPSS_UNI_EV_CAUSE_ENT evHndlr5CauseDefaultArr[] = APP_DEMO_P5_UNI_EV_DEFAULT;
static CPSS_UNI_EV_CAUSE_ENT evHndlr6CauseDefaultArr[] = APP_DEMO_P6_UNI_EV_DEFAULT;
static CPSS_UNI_EV_CAUSE_ENT evHndlr7CauseDefaultArr[] = APP_DEMO_P7_UNI_EV_DEFAULT;
static CPSS_UNI_EV_CAUSE_ENT evHndlr8CauseDefaultArr[] = APP_DEMO_P8_UNI_EV_DEFAULT;
static CPSS_UNI_EV_CAUSE_ENT evHndlr9CauseDefaultArr[] = APP_DEMO_P9_UNI_EV_DEFAULT;

static GT_U32 evHndlr0CauseDefaultArrSize = sizeof(evHndlr0CauseDefaultArr)/sizeof(evHndlr0CauseDefaultArr[0]);
static GT_U32 evHndlr1CauseDefaultArrSize = sizeof(evHndlr1CauseDefaultArr)/sizeof(evHndlr1CauseDefaultArr[0]);
static GT_U32 evHndlr2CauseDefaultArrSize = sizeof(evHndlr2CauseDefaultArr)/sizeof(evHndlr2CauseDefaultArr[0]);
static GT_U32 evHndlr3CauseDefaultArrSize = sizeof(evHndlr3CauseDefaultArr)/sizeof(evHndlr3CauseDefaultArr[0]);
static GT_U32 evHndlr4CauseDefaultArrSize = sizeof(evHndlr4CauseDefaultArr)/sizeof(evHndlr4CauseDefaultArr[0]);
static GT_U32 evHndlr5CauseDefaultArrSize = sizeof(evHndlr5CauseDefaultArr)/sizeof(evHndlr5CauseDefaultArr[0]);
static GT_U32 evHndlr6CauseDefaultArrSize = sizeof(evHndlr6CauseDefaultArr)/sizeof(evHndlr6CauseDefaultArr[0]);
static GT_U32 evHndlr7CauseDefaultArrSize = sizeof(evHndlr7CauseDefaultArr)/sizeof(evHndlr7CauseDefaultArr[0]);
static GT_U32 evHndlr8CauseDefaultArrSize = sizeof(evHndlr8CauseDefaultArr)/sizeof(evHndlr8CauseDefaultArr[0]);
static GT_U32 evHndlr9CauseDefaultArrSize = sizeof(evHndlr9CauseDefaultArr)/sizeof(evHndlr9CauseDefaultArr[0]);


static CPSS_UNI_EV_CAUSE_ENT evHndlr6CauseSingleTaskRxArr[] = APP_DEMO_P6_UNI_EV_SINGLE_RX_TASK_MODE;
static GT_U32 evHndlr6CauseSingleTaskRxArrSize = sizeof(evHndlr6CauseSingleTaskRxArr)/sizeof(evHndlr6CauseSingleTaskRxArr[0]);

static CPSS_UNI_EV_CAUSE_ENT evHndlr7CauseNoAuqPendingArr[] = APP_DEMO_P7_UNI_EV_NO_AUQ_PENDING;
static GT_U32 evHndlr7CauseNoAuqPendingArrSize = sizeof(evHndlr7CauseNoAuqPendingArr)/sizeof(evHndlr7CauseNoAuqPendingArr[0]);

static CPSS_UNI_EV_CAUSE_ENT *evHndlrCauseAllArr[APP_DEMO_PROCESS_NUMBER];
static GT_U32 evHndlrCauseAllArrSize[APP_DEMO_PROCESS_NUMBER];

/* Note - should be only static (global), cannot resides in stack! */
static EV_HNDLR_PARAM taskParamArr[APP_DEMO_PROCESS_NUMBER];

#ifdef DXCH_CODE
/*******************************************************************************
* appDemoDxChLion2DataIntegrityScan
*
* DESCRIPTION:
*       This routine treats Data Integrity events.
*
* INPUTS:
*       devNum - device number
*       evExtData - event external data
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
#define APP_DEMO_DXCH_LION2_DATA_INTEGRITY_EVENTS_NUM_CNS 20

GT_STATUS appDemoDxChLion2DataIntegrityScan
(
    GT_U8   devNum,
    GT_U32  evExtData
)
{
    GT_STATUS   rc;                 /* return code */
    GT_U32      eventsNum = APP_DEMO_DXCH_LION2_DATA_INTEGRITY_EVENTS_NUM_CNS;     /* initial number of ECC/parity events */
    CPSS_DXCH_DIAG_DATA_INTEGRITY_EVENT_STC     eventsArr[APP_DEMO_DXCH_LION2_DATA_INTEGRITY_EVENTS_NUM_CNS];
    GT_BOOL     isScanFinished = GT_FALSE;     /* events scan finish status */

#ifdef IMPL_GALTIS
    GT_U32 i; /*loop iterator*/
#endif /* IMPL_GALTIS */


    while(isScanFinished != GT_TRUE)
    {
        rc = cpssDxChDiagDataIntegrityEventsGet(devNum, evExtData, &eventsNum, eventsArr, &isScanFinished);
        if(GT_OK != rc)
        {
            return rc;
        }

#ifdef IMPL_GALTIS
        for(i = 0; i < eventsNum; i++)
        {
            /* counting the event */
            if(dxChDataIntegrityEventIncrementFunc != NULL)
            {
                rc = dxChDataIntegrityEventIncrementFunc(devNum, &eventsArr[i]);
                if(GT_OK != rc)
                {
                    cpssOsPrintf("appDemoDxChLion2DataIntegrityScan: failed to increment event counter for memType %d\r\n", eventsArr[i].memType);
                    return rc;
                }
            }
        }
#endif /* IMPL_GALTIS */

    }

    return GT_OK;
}
#endif /* DXCH_CODE */

/*******************************************************************************
* appDemoEventRequestDrvnModeInit
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
GT_STATUS appDemoEventRequestDrvnModeInit
(
    IN GT_VOID
)
{
    GT_STATUS rc;              /* The returned code            */
    GT_U32    i;               /* Iterator                     */
    GT_CHAR   name[30];        /* The task/msgQ name           */
    GT_32     intKey;          /* The interrupt lock key       */
    GT_U32    value;
    unsigned (__TASKCONV *start_addr)(GT_VOID*);
    GT_U8   dev;

    rc = cpssOsMutexCreate("appDemoEventHandlerCounterUpdateLockMtx",
                            &appDemoEventHandlerCounterUpdateLockMtx);
    if (rc != GT_OK)
    {
        return rc;
    }

    if (appDemoDbEntryGet("singleRxTask",&value) == GT_OK)
    {
        singleTaskRxEnable = (value == 1) ? GT_TRUE : GT_FALSE;
    }
    if (appDemoDbEntryGet("singleRxTaskRandom",&value) == GT_OK)
    {
        singleTaskRxRandomEnable = (value == 1) ? GT_TRUE : GT_FALSE;
    }

    /* set default arrays and sizes */
    evHndlrCauseAllArr[0] = evHndlr0CauseDefaultArr;
    evHndlrCauseAllArr[1] = evHndlr1CauseDefaultArr;
    evHndlrCauseAllArr[2] = evHndlr2CauseDefaultArr;
    evHndlrCauseAllArr[3] = evHndlr3CauseDefaultArr;
    evHndlrCauseAllArr[4] = evHndlr4CauseDefaultArr;
    evHndlrCauseAllArr[5] = evHndlr5CauseDefaultArr;
    evHndlrCauseAllArr[6] = evHndlr6CauseDefaultArr;
    evHndlrCauseAllArr[7] = evHndlr7CauseDefaultArr;
    evHndlrCauseAllArr[8] = evHndlr8CauseDefaultArr;
    evHndlrCauseAllArr[9] = evHndlr9CauseDefaultArr;

    evHndlrCauseAllArrSize[0] = evHndlr0CauseDefaultArrSize;
    evHndlrCauseAllArrSize[1] = evHndlr1CauseDefaultArrSize;
    evHndlrCauseAllArrSize[2] = evHndlr2CauseDefaultArrSize;
    evHndlrCauseAllArrSize[3] = evHndlr3CauseDefaultArrSize;
    evHndlrCauseAllArrSize[4] = evHndlr4CauseDefaultArrSize;
    evHndlrCauseAllArrSize[5] = evHndlr5CauseDefaultArrSize;
    evHndlrCauseAllArrSize[6] = evHndlr6CauseDefaultArrSize;
    evHndlrCauseAllArrSize[7] = evHndlr7CauseDefaultArrSize;
    evHndlrCauseAllArrSize[8] = evHndlr8CauseDefaultArrSize;
    evHndlrCauseAllArrSize[9] = evHndlr9CauseDefaultArrSize;

    /* override defaults */
    rxEventHandlerLibInit();
    auEventHandlerLibInit();
#ifdef SHARED_MEMORY
    if(multiProcessAppDemo == 1)
    {
        evHndlrCauseAllArrSize[1] = 0;
        evHndlrCauseAllArrSize[2] = 0;
        evHndlrCauseAllArrSize[3] = 0;
        evHndlrCauseAllArrSize[4] = 0;
        evHndlrCauseAllArrSize[5] = 0;
        evHndlrCauseAllArrSize[6] = 0;
        evHndlrCauseAllArrSize[7] = 0;
        evHndlrCauseAllArrSize[8] = 0;
    }
    else
#endif
    {
    rxEventHandlerInitHandler(
            RX_EV_HANDLER_DEVNUM_ALL,
            RX_EV_HANDLER_QUEUE_ALL,
            RX_EV_HANDLER_TYPE_ANY_E,
            &rxEventHanderHnd);

    if (singleTaskRxEnable == GT_TRUE)
    {
        evHndlrCauseAllArrSize[1] = evHndlrCauseAllArrSize[2] = evHndlrCauseAllArrSize[3] =
        evHndlrCauseAllArrSize[4] = evHndlrCauseAllArrSize[5] = 0;

        evHndlrCauseAllArr[APP_DEMO_SINGLE_RX_TASK_NUMBER] = evHndlr6CauseSingleTaskRxArr;
        evHndlrCauseAllArrSize[APP_DEMO_SINGLE_RX_TASK_NUMBER] = evHndlr6CauseSingleTaskRxArrSize;
    }
    auEventHandlerInitHandler(
            AU_EV_HANDLER_DEVNUM_ALL,
            AU_EV_HANDLER_TYPE_ANY_E,
            &auEventHandlerHnd);
    }

    if(appDemoDbEntryGet("NoBindAuqPendingEvent",&value) == GT_OK)
    {
        evHndlrCauseAllArr[7] = evHndlr7CauseNoAuqPendingArr;
        evHndlrCauseAllArrSize[7] = evHndlr7CauseNoAuqPendingArrSize;
    }


#if (defined EX_FAMILY) || (defined MX_FAMILY)
    /* re-size array[7] that hold as last event the :
        CPSS_PP_MAC_AGE_VIA_TRIGGER_ENDED_E
        --> because on ExMx the cpss do internal bind to the event
    */
    if(evHndlrCauseAllArrSize[7])
    {
        evHndlrCauseAllArrSize[7]--;
    }
#endif /*(defined EX_FAMILY) || (defined MX_FAMILY)*/

    {
        /* silent the next events that are part of the ' controlled learning' .
           this event cause when we add/delete mac from the FDB and the FDB notify
           us , that the action done ,
           we get those from 4 port groups and it slows the operations */
        if ((appDemoPpConfigList[SYSTEM_DEV_NUM_MAC(0)].devFamily ==
            CPSS_PP_FAMILY_DXCH_LION_E) ||
            (appDemoPpConfigList[SYSTEM_DEV_NUM_MAC(0)].devFamily ==
            CPSS_PP_FAMILY_DXCH_LION2_E))
        {
            for(i = 0 ; i < evHndlrCauseAllArrSize[0];i++)
            {
                if(evHndlr0CauseDefaultArr[i] == CPSS_PP_MAC_NA_LEARNED_E)
                {
                    evHndlr0CauseDefaultArr[i] = evHndlr0CauseDefaultArr[evHndlrCauseAllArrSize[0] - 1];
                    break;
                }
            }

            if(i != evHndlrCauseAllArrSize[0])
            {
                /* we removed CPSS_PP_MAC_NA_LEARNED_E */
                /* so need to decrement the number of elements */
                evHndlrCauseAllArrSize[0] --;
            }

        }
    }

    /* Get event increment. */
    eventIncrementFunc = appDemoGenEventCounterIncrement;

    /* Reset Event Counter DB */
    osMemSet(uniEventCounters, 0, sizeof(uniEventCounters));

    if(eventIncrementFunc != NULL)
    {
        /* just trigger to allocate the needed memory for the device (during initialization)
           and not to wait till the first event.

           because the 'Enhanced UT' will check for memory leakage , and if first
           event will come only after the test started , then the test will fail .
        */

        for(dev = SYSTEM_DEV_NUM_MAC(0); dev < SYSTEM_DEV_NUM_MAC(appDemoPpConfigDevAmount); dev++)
        {
            if(appDemoPpConfigList[dev].valid == GT_FALSE)
            {
                continue;
            }
            /* don't care about the return value */
            (GT_VOID)eventIncrementFunc(appDemoPpConfigList[dev].devNum, 0xFFFFFFFF, 0);
        }
    }
#if defined(DXCH_CODE) && defined(IMPL_GALTIS)

    /* Get event increment CB routine. */
    wrCpssDxChDiagDataIntegrityCountersCbGet(&dxChDataIntegrityEventIncrementFunc);

    if(dxChDataIntegrityEventIncrementFunc != NULL)
    {
        /* trigger to allocate the needed memory for the device (during initialization)*/
        for(dev = SYSTEM_DEV_NUM_MAC(0); dev < SYSTEM_DEV_NUM_MAC(appDemoPpConfigDevAmount); dev++)
        {
            if(appDemoPpConfigList[dev].valid == GT_FALSE)
            {
                continue;
            }
            /* don't care about the return value */
            (GT_VOID)dxChDataIntegrityEventIncrementFunc(appDemoPpConfigList[dev].devNum, NULL);
        }
    }
#endif /* defined(DXCH_CODE) && defined(IMPL_GALTIS) */


    /* bind the events for all the App-Demo event handlers */

    /* NOTE : MUST lock interrupts from first bind and unmask till end
             of bind and unmask , so we will not loose any event.

       explanation on option to loose interrupt if not locking:
       I will describe loose interrupt of 'link change' for a port that when
       'power up to board' is in state of 'up'.

       assume that the loop in not under 'lock interrupts'

       the event of 'link change' CPSS_PP_PORT_LINK_STATUS_CHANGED_E is in taskParamArr[9]
       and the event of CPSS_PP_PORT_RX_FIFO_OVERRUN_E is in taskParamArr[0]

       BUT those to events are actually in the same register of 'per port' ,
       so if the HW initialized also the 'rx fifo overrun' interrupts , then
       the after the 'bind and unmask' of this event the ISR will read (and clear)
       the register of the 'per port' and will not care about the 'link change'
       event that is now LOST !
    */
    /* lock section to disable interruption of ISR while unmasking events */
    INT_LOCK(intKey);
    for (i = 0; i < APP_DEMO_PROCESS_NUMBER; i++)
    {
        if(evHndlrCauseAllArrSize[i] == 0 || evHndlrCauseAllArr[i][0] == CPSS_UNI_RSRVD_EVENT_E)
            continue;
        taskParamArr[i].hndlrIndex = i;

        DBG_LOG(("appDemoEventRequestDrvnModeInit: subscribed group #%d for %d events\n", i, evHndlrCauseAllArrSize[i], 3, 4, 5, 6));
        /* call CPSS to bind the events under single handler */
        rc = cpssEventBind(evHndlrCauseAllArr[i],
                           evHndlrCauseAllArrSize[i],
                           &taskParamArr[i].evHndl);
        if (GT_OK != rc)
        {
            INT_UNLOCK(intKey);
            return GT_FAIL;
        }

        /* call the CPSS to enable those interrupts in the HW of the device */
        rc = prvUniEvMaskAllSet(evHndlrCauseAllArr[i],
                                evHndlrCauseAllArrSize[i],
                                CPSS_EVENT_UNMASK_E);
        if (GT_OK != rc)
        {
            INT_UNLOCK(intKey);
            return GT_FAIL;
        }
    }

#ifdef DXCH_CODE

    for(dev = SYSTEM_DEV_NUM_MAC(0); dev < SYSTEM_DEV_NUM_MAC(appDemoPpConfigDevAmount); dev++)
    {
        if(GT_FALSE == appDemoPpConfigList[dev].valid)
        {
            continue;
        }

        if(GT_TRUE == PRV_CPSS_DXCH_ERRATA_GET_MAC(appDemoPpConfigList[dev].devNum,
            PRV_CPSS_DXCH_BOBCAT2_GPP_INTERRUPT_WA_E))
        {
            (GT_VOID)cpssEventDeviceMaskSet(appDemoPpConfigList[dev].devNum,
                                        CPSS_PP_GPP_E,
                                        CPSS_EVENT_MASK_E);
        }

        switch (appDemoPpConfigList[dev].deviceId)
        {/* in existing Hooper RD boards there is mistake with default state of GPP interrupt,
            mask it to prevent not needed interrupts storming */
            case CPSS_LION2_HOOPER_PORT_GROUPS_0123_DEVICES_CASES_MAC:
                (GT_VOID)cpssEventDeviceMaskSet(appDemoPpConfigList[dev].devNum,
                                                CPSS_PP_GPP_E,
                                                CPSS_EVENT_MASK_E);
                break;
            case CPSS_BOBK_ALL_DEVICES_CASES_MAC:
                /* BobK device has GPP#5 muxed with LED interface.
                   Need to disable event to avoid storming from LED related data. */
                (GT_VOID)cpssEventDeviceMaskWithEvExtDataSet(appDemoPpConfigList[dev].devNum,
                                                CPSS_PP_GPP_E, 5, CPSS_EVENT_MASK_E);
                break;
            default:
                break;
        }
        (GT_VOID)cpssEventDeviceMaskSet(appDemoPpConfigList[dev].devNum,
                                        CPSS_PP_PORT_MMPCS_SIGNAL_DETECT_CHANGE_E,
                                        CPSS_EVENT_MASK_E);

        if(appDemoPpConfigList[dev].devFamily != CPSS_PP_FAMILY_DXCH_LION2_E)
        {
            (GT_VOID)cpssEventDeviceMaskSet(appDemoPpConfigList[dev].devNum,
                                        CPSS_PP_PORT_LANE_SYNC_STATUS_CHANGED_E,
                                            CPSS_EVENT_MASK_E);
        }
        else
        {
            GT_PHYSICAL_PORT_NUM            portNum;
            CPSS_PORT_INTERFACE_MODE_ENT    ifMode;

            /* loop over all ports */
            for (portNum = 0; portNum < (appDemoPpConfigList[dev].maxPortNumber); portNum++)
            {
                CPSS_ENABLER_PORT_SKIP_CHECK(appDemoPpConfigList[dev].devNum, portNum);

                rc = cpssDxChPortInterfaceModeGet(appDemoPpConfigList[dev].devNum, portNum, &ifMode);
                if (rc != GT_OK)
                {
                    return rc;
                }

                if((CPSS_PORT_INTERFACE_MODE_KR_E == ifMode)
                   || (CPSS_PORT_INTERFACE_MODE_SR_LR_E == ifMode)
                   || (CPSS_PORT_INTERFACE_MODE_XHGS_E == ifMode)
                   || (CPSS_PORT_INTERFACE_MODE_XHGS_SR_E == ifMode))
                {
                    (GT_VOID)cpssEventDeviceMaskWithEvExtDataSet(appDemoPpConfigList[dev].devNum,
                                 CPSS_PP_PORT_MMPCS_SIGNAL_DETECT_CHANGE_E,
                                 portNum, CPSS_EVENT_UNMASK_E);
                }
                else if(CPSS_PORT_INTERFACE_MODE_RXAUI_E == ifMode)
                {
                    (GT_VOID)cpssEventDeviceMaskWithEvExtDataSet(appDemoPpConfigList[dev].devNum,
                        CPSS_PP_PORT_LANE_SYNC_STATUS_CHANGED_E,
                        ((portNum<<8)|0x1),
                        CPSS_EVENT_UNMASK_E);
                    (GT_VOID)cpssEventDeviceMaskWithEvExtDataSet(appDemoPpConfigList[dev].devNum,
                        CPSS_PP_PORT_LANE_SYNC_STATUS_CHANGED_E,
                        ((portNum<<8)|0x2),
                        CPSS_EVENT_UNMASK_E);
                }
            }
        }

        if(appDemoPpConfigList[dev].devFamily != CPSS_PP_FAMILY_DXCH_XCAT2_E)
        {
            (GT_VOID)cpssEventDeviceMaskSet(appDemoPpConfigList[dev].devNum,
                                            CPSS_PP_PORT_PRBS_ERROR_QSGMII_E,
                                            CPSS_EVENT_MASK_E);
        }
CPSS_TBD_BOOKMARK /* must define PEX window range to prevent this interrupt on all
                    new devices */
        (GT_VOID)cpssEventDeviceMaskSet(appDemoPpConfigList[dev].devNum,
                                        CPSS_PP_PEX_HIT_DEFAULT_WIN_ERR_E,
                                        CPSS_EVENT_MASK_E);
    }
#endif


    INT_UNLOCK(intKey);


    if (singleTaskRxEnable == GT_TRUE)
    {
        for (i = 0; i < treatTasksNum; i++)
        {
            /* create msgQ */
            osSprintf(name, "msgQ_%d", i);
            rc = cpssOsMsgQCreate(name,APP_DEMO_MSGQ_SIZE,
                                  sizeof(APP_DEMO_RX_PACKET_PARAMS),
                                  &(singleRxMsgQIds[i]));
            if (rc != GT_OK)
            {
                return rc;
            }

            if(i == 0)/* create mutex only once */
            {
                    rc = cpssOsMutexCreate("rxMutex",&rxMutex);
                    if (rc != GT_OK)
                    {
                        return rc;
                    }
            }

            /* spawn rx treat task */
            osSprintf(name, "treatRx_%d", i);
            rc = osTaskCreate(name,
                          EV_HANDLER_MAX_PRIO - APP_DEMO_SINGLE_RX_TASK_NUMBER - 1,
                          _8KB,
                          appDemoSingleRxTreatTask,
                          &(singleRxMsgQIds[i]),
                          &treatRxEventHandlerTid[i]);
            if (rc != GT_OK)
            {
                return rc;
            }
        }
    }


    /* spawn all the event handler processes */
    for (i = 0; i < APP_DEMO_PROCESS_NUMBER; i++)
    {
        if(evHndlrCauseAllArrSize[i] == 0 || evHndlrCauseAllArr[i][0] == CPSS_UNI_RSRVD_EVENT_E)
            continue;
        osSprintf(name, "evHndl_%d", i);

        if (singleTaskRxEnable == GT_TRUE && i == APP_DEMO_SINGLE_RX_TASK_NUMBER)
        {
            start_addr = appDemoSingleRxEvHndlr;
        }
        else
        {
            start_addr = appDemoEvHndlr;
        }

        taskCreated = 0;
        rc = osTaskCreate(name,
                          EV_HANDLER_MAX_PRIO - i,
                          _32KB,
                          start_addr,
                          &taskParamArr[i],
                          &eventHandlerTidArr[i]);
        if (rc != GT_OK)
        {
            return GT_FAIL;
        }


        while(taskCreated == 0)
        {
            /* wait for indication that task created */
            osTimerWkAfter(1);
        }
    }

#ifdef GM_USED

    /* the GM not hold MAC and therefor not generate next event :
        CPSS_PP_PORT_LINK_STATUS_CHANGED_E

        still we have WA using simulation on top of the GM.
    */
    {
        GT_TASK gmTid;
        osSprintf(name, "GM_link_status");
        start_addr = gm_appDemoEmulateLinkChangeTask;

        taskCreated = 0;
        rc = osTaskCreate(name,
                          EV_HANDLER_MAX_PRIO - i,
                          _32KB,
                          start_addr,
                          NULL,
                          &gmTid);
        if (rc != GT_OK)
        {
            return GT_FAIL;
        }


        while(taskCreated == 0)
        {
            /* wait for indication that task created */
            osTimerWkAfter(1);
        }
    }
#endif /*GM_USED*/


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
    GT_U32    i;              /* Iterator                     */
    GT_U8     dev;            /* Device iterator              */
    GT_U32    time=0;

    if ( (PRV_CPSS_SIP_5_CHECK_MAC((appDemoPpConfigList[SYSTEM_DEV_NUM_MAC(0)].devNum))) &&
         (systemRecoveryInfo.systemRecoveryState == CPSS_SYSTEM_RECOVERY_INIT_STATE_E) &&
         (systemRecoveryInfo.systemRecoveryProcess == CPSS_SYSTEM_RECOVERY_PROCESS_FAST_BOOT_E ) )
        time = 200;

    /* unmask the interrupt */
    for (i = 0; i < arrLength; i++)
    {
        osTimerWkAfter(time);

        if (cpssUniEventArr[i] > CPSS_UNI_EVENT_COUNT_E) /* last event */
        {
            return GT_FAIL;
        }

        /* unmask the interrupt for all PPs/XBARs/FAs */

        /* assume PRV_CPSS_MAX_PP_DEVICES_CNS >= PRV_CPSS_MAX_XBAR_DEVICES_CNS */
        for (dev = 0; dev < PRV_CPSS_MAX_PP_DEVICES_CNS; dev++)
        {
            if(GT_FALSE == appDemoPpConfigList[dev].valid)
            {
                continue;
            }
            rc = cpssEventDeviceMaskSet(appDemoPpConfigList[dev].devNum, cpssUniEventArr[i], operation);
            switch(rc)
            {
                case GT_NOT_INITIALIZED:
                    /* assume there are no FA/XBAR devices in the system ,
                       because there was initialization of the 'phase 1' for any FA/XBAR */

                    /* fall through */
                case GT_BAD_PARAM:
                    /* assume that this PP/FA/XBAR device not exists , so no DB of PP/FA/XBAR devices needed .... */

                    /* fall through */
                case GT_NOT_FOUND:
                    /* this event not relevant to this device */
                    rc = GT_OK;
                    break;

                case GT_OK:
                    break;

                default:
                    /* other real error */
                    break;
            }
        }
    }

    return rc;
} /* prvUniEvMaskAllSet */

static  GT_TASK appDemoTestsEventHandlerTid = 0;     /* task ID for test's event handler */

/*******************************************************************************
* appDemoEventsToTestsHandlerBind
*
* DESCRIPTION:
*       This routine creates new event handler for the unified event list.
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
GT_STATUS appDemoEventsToTestsHandlerBind
(
    IN  CPSS_UNI_EV_CAUSE_ENT       cpssUniEventArr[],
    IN  GT_U32                      arrLength,
    IN  CPSS_EVENT_MASK_SET_ENT     operation
)
{
    GT_32           intKey;        /* The interrupt lock key */
    EV_HNDLR_PARAM  taskParam;     /* parameters for task */
    GT_STATUS       rc;            /* return code */
    char    name[30] = "eventGenerationTask"; /* task name */

    if ((operation == CPSS_EVENT_UNMASK_E) && (appDemoTestsEventHandlerTid == 0))
    {
        INT_LOCK(intKey);
        /* call CPSS to bind the events under single handler */
        rc = cpssEventBind(cpssUniEventArr, arrLength, &taskParam.evHndl);
        INT_UNLOCK(intKey);
        if (GT_OK != rc)
        {
            osPrintf("appDemoEventsToTestsHandlerBind: bind fail %d", rc);
            return rc;
        }

        /* Create event handler task */
        taskParam.hndlrIndex = 25;
        taskCreated = 0;
        rc = osTaskCreate(name, EV_HANDLER_MAX_PRIO,  _32KB, appDemoEvHndlr,
                          &taskParam,  &appDemoTestsEventHandlerTid);
        if (rc != GT_OK)
        {
            osPrintf("appDemoEventsToTestsHandlerBind: task create fail %d", rc);
            return rc;
        }

        while(taskCreated == 0)
        {
            /* wait for indication that task created */
            osTimerWkAfter(1);
        }
    }

    /* call the CPSS to enable those interrupts in the HW of the device */
    rc = prvUniEvMaskAllSet(cpssUniEventArr, arrLength, operation);
    if (GT_OK != rc)
    {
        osPrintf("appDemoEventsToTestsHandlerBind: mask/unmask fail %d", rc);
        return rc;
    }
    return GT_OK;
}
/*******************************************************************************
* cpssEnFuMsgGet
*
* DESCRIPTION:
*       This routine handles FDB upload (FU) messages.
*
* INPUTS:
*       devNum      - the device number.
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
static GT_STATUS cpssEnFuMsgGet
(
    IN GT_U8                devNum,
    GT_U32                  evExtData
)
{
    GT_STATUS   rc = GT_NOT_IMPLEMENTED;

    if(IS_API_CH_DEV_MAC(devNum))/* is device support dxch API ? */
    {
#ifdef DXCH_CODE
/*      if(1)*//* 2 queues used *//*
        {
            rc = cpssEnChDxFuMsgHandle(devNum, evExtData);
        }
        else
        {
            rc = cpssEnChDxAuMsgHandle(devNum, evExtData);
        }*/
#endif /*DXCH_CODE*/
    }

    return rc;
}

/*******************************************************************************
* appDemoEventFatalErrorEnable
*
* DESCRIPTION:
*       Set fatal error handling type.
*
* INPUTS:
*       fatalErrorType  - fatal error handling type
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK   - on success
*       GT_BAD_PARAM - on wrong fatalErrorType
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS appDemoEventFatalErrorEnable
(
    APP_DEMO_FATAL_ERROR_TYPE fatalErrorType
)
{
    switch(fatalErrorType)
    {
        case APP_DEMO_FATAL_ERROR_NOTIFY_ONLY_TYPE_E:
        case APP_DEMO_FATAL_ERROR_EXECUTE_TYPE_E:
        case APP_DEMO_FATAL_ERROR_SILENT_TYPE_E:
            prvAppDemoFatalErrorType = fatalErrorType;
            break;
        default:
            return GT_BAD_PARAM;
    }
    return GT_OK;
}

#if (defined DXCH_CODE) && (!defined ASIC_SIMULATION)

/* type of combo port:
   1 - XG;
   0 - GE
*/
static GT_U32 comboType = 1; /* be default XG */
GT_STATUS dbgSetComboType
(
    IN  GT_U32  cType
)
{
    GT_U32 oldType = comboType;

    comboType = cType;

    return oldType;
}

/*******************************************************************************
* prvAppDemoUserHandleDxChComboPort
*
* DESCRIPTION:
*       Set active MAC of combo port.
*
* INPUTS:
*       devNum      - the device number.
*       portNum     - physical port number
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
static GT_STATUS prvAppDemoUserHandleDxChComboPort
(
    GT_U8                   devNum,
    GT_PHYSICAL_PORT_NUM    portNum
)
{
    GT_STATUS               rc;
    GT_U32                  i;
    GT_BOOL                 enable;
    CPSS_DXCH_PORT_COMBO_PARAMS_STC params;
    GT_BOOL                 signalState;
    GT_U32                  portGroupId;
    GT_U32                  localPort;
    GT_U32                  lanesArrayP9[2] = {13, 18};
    GT_U32                  lanesArrayP11[2] = {15, 20};
    GT_U32                  *portLanesArrayPtr;

    rc = cpssDxChPortComboModeEnableGet(devNum, portNum, &enable, &params);
    if(rc != GT_OK)
    {
        cpssOsPrintSync("cpssDxChPortComboModeEnableGet(portNum=%d):rc=%d\n",
                        portNum, rc);
    }

    if(!enable)
    {
        return GT_OK;
    }

    portGroupId = PRV_CPSS_GLOBAL_PORT_TO_PORT_GROUP_ID_CONVERT_MAC(devNum, portNum);
    localPort = PRV_CPSS_GLOBAL_PORT_TO_LOCAL_PORT_CONVERT_MAC(devNum, portNum);
    portLanesArrayPtr = (9 == localPort) ? lanesArrayP9 : lanesArrayP11;
    for(i = 0; i < 2; i++)
    {
        /* check that signal up detected on port - on all its serdes lanes */
        rc = cpssDxChPortSerdesLaneSignalDetectGet(devNum, portGroupId,
                                                   portLanesArrayPtr[i],
                                                   &signalState);
        if (rc != GT_OK)
        {
            cpssOsPrintSync("cpssDxChPortSerdesLaneSignalDetectGet fail:portGroupId=%d,lane=%d,rc=%d\n",
                            portGroupId, portLanesArrayPtr[i], rc);
            return rc;
        }
        if (GT_TRUE == signalState)
        {
            break;
        }
    }

    if (i < 2) /* if signal changed to up */
    {
        rc = cpssDxChPortComboPortActiveMacSet(devNum, portNum,
                                               &(params.macArray[i]));
        if (rc != GT_OK)
        {
            cpssOsPrintSync("cpssDxChPortComboPortActiveMacSet fail:portNum=%d,macArrIdx=%d,rc=%d\n",
                            portNum, i, rc);
            return rc;
        }
    }

    return GT_OK;
}
#endif

#ifdef DXCH_CODE
#include <cpss/dxCh/dxChxGen/port/cpssDxChPortMapping.h>
/*******************************************************************************
* sip5_20_linkChange
*
* DESCRIPTION:
*       function to handle link change in sip5_20 .
*
* INPUTS:
*       devNum      - the device number.
*       macPortNum  - the physical port number that generated the event of
*               CPSS_PP_PORT_LINK_STATUS_CHANGED_E
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
static GT_STATUS   sip5_20_linkChange(
    IN GT_U8 devNum,
    IN GT_U32 macPortNum
)
{
    GT_STATUS rc;
    GT_BOOL linkUp;
    GT_U32  maxPortNum;/* max port iterator value*/
    GT_U32  portNum;/* physical port num (range 0..255/511) */
    CPSS_DXCH_DETAILED_PORT_MAP_STC  portMap;
    CPSS_DXCH_DETAILED_PORT_MAP_STC *portMapPtr = &portMap;
    GT_BOOL isMacPortInBiodirectionalMode;/*is MAC of a 'link down' port in mode of 'Biodirectional' */

    rc = cpssDxChCfgTableNumEntriesGet(devNum,CPSS_DXCH_CFG_TABLE_PHYSICAL_PORT_E,&maxPortNum);
    if (rc != GT_OK)
    {
        DBG_LOG(("cpssDxChCfgTableNumEntriesGet [CPSS_DXCH_CFG_TABLE_PHYSICAL_PORT_E]: error, devNum=%d, port=%d, rc=%d\n",
                devNum, macPortNum, rc, 4, 5, 6));
        return rc;
    }

    for (portNum = 0 ; portNum < maxPortNum; portNum++)
    {
        rc = cpssDxChPortPhysicalPortDetailedMapGet(devNum,portNum,/*OUT*/portMapPtr);
        if (rc != GT_OK)
        {
            DBG_LOG(("cpssDxChPortPhysicalPortDetailedMapGet: error, devNum=%d, port=%d, rc=%d\n",
                    devNum, portNum, rc, 4, 5, 6));
            return rc;
        }
        if (portMapPtr->valid == GT_FALSE)
        {
            continue;
        }

        if(portMapPtr->portMap.macNum != macPortNum)
        {
            continue;
        }

        /* we got valid physical port for our MAC number */
        rc = cpssDxChPortLinkStatusGet(devNum, portNum, &linkUp);
        if (rc != GT_OK)
        {
            DBG_LOG(("cpssDxChPortLinkStatusGet: error, devNum=%d, port=%d, rc=%d\n",
                    devNum, portNum, rc, 4, 5, 6));
            return rc;
        }

        if(printLinkChangeEnabled)
        {
            cpssOsPrintSync("LINK %s : devNum[%ld] MAC port[%ld] (physical port[%ld]) \n",
                                (linkUp == GT_FALSE) ? "DOWN" : "UP",
                                            (GT_U32)devNum, macPortNum , portNum);
        }

        isMacPortInBiodirectionalMode = GT_FALSE;
        if(linkUp == GT_FALSE && PRV_CPSS_DXCH3_FAMILY_CHECK_MAC(devNum))
        {
            rc = cpssDxChVntOamPortUnidirectionalEnableGet(devNum,portNum,
                &isMacPortInBiodirectionalMode);
#ifdef GM_USED
            isMacPortInBiodirectionalMode = GT_FALSE;
            rc = GT_OK;
#endif /*GM_USED*/

            if (rc != GT_OK)
            {
                DBG_LOG(("cpssDxChVntOamPortUnidirectionalEnableGet: error, devNum=%d, port=%d, rc=%d\n",
                        devNum, portNum, rc, 4, 5, 6));
                return rc;
            }
        }

        if(linkUp == GT_FALSE && isMacPortInBiodirectionalMode == GT_TRUE)
        {
            if(printLinkChangeEnabled)
            {
                cpssOsPrintSync("MAC Biodirectional enabled : even though the devNum[%ld] MAC port[%ld] (physical port[%ld]) is DOWN the port can egress packets \n",
                                    (GT_U32)devNum, macPortNum , portNum);
            }

            /* !!! do not modify the filter !!! keep it as 'link up' */
            continue;
        }

        /* need to set the EGF link status filter according to new state of the
           port */
        rc = cpssDxChBrgEgrFltPortLinkEnableSet(devNum, portNum,
            (linkUp == GT_TRUE ? GT_FALSE : GT_TRUE));
        if (rc != GT_OK)
        {
            DBG_LOG(("cpssDxChBrgEgrFltPortLinkEnableSet: error, devNum=%d, port=%d, link state[%d] rc=%d\n",
                    devNum, portNum, linkUp, rc, 5, 6));
            return rc;
        }
    }

    return GT_OK;

}
#endif /*DXCH_CODE*/

/*******************************************************************************
* appDemoEnPpEvTreat
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
GT_U32 mmpcsPrint = 0;

extern GT_U32 mmpcsPrintSet(GT_U32 enable)
{
    GT_U32 oldState;

    oldState = mmpcsPrint;
    mmpcsPrint = enable;

    return oldState;
}

static GT_STATUS appDemoEnPpEvTreat
(
    GT_U8                   devNum,
    GT_U32                  uniEv,
    GT_U32                  evExtData
)
{
    GT_STATUS   rc = GT_OK;
    GT_U8       queue;
    GT_BOOL     linkUp;
    GT_PHYSICAL_PORT_NUM    portNum;
    GT_U32                  portMacNum;
#ifdef DXCH_CODE
    CPSS_DXCH_IMPLEMENT_WA_ENT   waArr[1];
    GT_U32  additionalInfoBmpArr[1];
    GT_U32  evExtDataOld;
#endif
   /* printf("uniEv is %d  \n", uniEv);*/
    switch (uniEv)
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

                rc = cpssEnRxPacketGet(RX_EV_HANDLER_TYPE_RX_E, devNum, queue);
            break;
        /* rx_buffer event doesn't arrive on every rx buffer event,
         * so theoretically under heavy load, there could be scenario,
         * when just rx_error events will signal that there are
         * descriptors to treat in chain
         */
        case CPSS_PP_RX_ERR_QUEUE0_E:
        case CPSS_PP_RX_ERR_QUEUE1_E:
        case CPSS_PP_RX_ERR_QUEUE2_E:
        case CPSS_PP_RX_ERR_QUEUE3_E:
        case CPSS_PP_RX_ERR_QUEUE4_E:
        case CPSS_PP_RX_ERR_QUEUE5_E:
        case CPSS_PP_RX_ERR_QUEUE6_E:
        case CPSS_PP_RX_ERR_QUEUE7_E:
                queue = (GT_U8)(uniEv - CPSS_PP_RX_ERR_QUEUE0_E);

                rc = cpssEnRxPacketGet(RX_EV_HANDLER_TYPE_RX_ERR_E, devNum, queue);
            break;
        case CPSS_PP_MAC_MESSAGE_TO_CPU_READY_E:
        case CPSS_PP_EB_AUQ_PENDING_E:

            if(allowProcessingOfAuqMessages == GT_TRUE)
            {
#ifdef DEBUG_LINUX_ISR_LOCK
                /* make the system busy while test of UT(cpssDxChCfgReNumberDevNum)
                   may change the DB of CPSS */

                rc = 0;
                while(1)
                {
                    GT_32 dummy;
                    /*lock*/
                    osSetIntLockUnlock(0,&dummy);
                    rc = cpssEnGenAuMsgHandle(auEventHandlerHnd,devNum, evExtData);
                    /*unlock*/
                    osSetIntLockUnlock(1,&dummy);
                };
#endif /*DEBUG_LINUX_ISR_LOCK*/

                rc = cpssEnGenAuMsgHandle(auEventHandlerHnd,devNum, evExtData);
                if (GT_OK != rc)
                {
                    DBG_LOG(("cpssEnAuMsgGet: error, devNum=%d, uniEv=%d, rc=%d\n", devNum, uniEv, rc, 4, 5, 6));
                }
            }

            break;
        case CPSS_PP_EB_FUQ_PENDING_E:

            rc = cpssEnFuMsgGet(devNum, evExtData);
            if (GT_OK != rc)
            {
                DBG_LOG(("cpssEnFuMsgGet: error, devNum=%d, uniEv=%d, rc=%d\n", devNum, uniEv, rc, 4, 5, 6));
            }
            break;
        case CPSS_PP_PORT_LINK_STATUS_CHANGED_E:
#ifdef DXCH_CODE
                    /* work arround appearing wrong MIB counters after port link down */
            waArr[0] = CPSS_DXCH_IMPLEMENT_WA_WRONG_MIB_COUNTERS_LINK_DOWN_E;
            additionalInfoBmpArr[0] = evExtData;
            rc = cpssDxChHwPpImplementWaInit(devNum,1,&waArr[0],
                                            &additionalInfoBmpArr[0]);
            if (GT_OK != rc)
            {
                DBG_LOG(("cpssDxChHwPpImplementWaInit: wrong MIB counters after port link down error, devNum=%d, port=%d, rc=%d\n",
                        devNum, evExtData, rc, 4, 5, 6));
            }

            evExtDataOld = evExtData;

            if(PRV_CPSS_SIP_5_10_CHECK_MAC(devNum) &&
                PRV_CPSS_PP_MAC(devNum)->devFamily != CPSS_PP_FAMILY_DXCH_BOBCAT3_E)/* api cpssDxChPortApPortConfigGet not supports bc3 ... yet */
            {/* on BC2 for port with AP - disable MAC if link down to prevent
                traffic during link reestablish */
                GT_BOOL                         apEnable;
                CPSS_DXCH_PORT_AP_PARAMS_STC    apParams;

                if (CPSS_IS_DXCH_FAMILY_MAC(PRV_CPSS_PP_MAC(devNum)->devFamily))
                {
                    rc = cpssDxChPortPhysicalPortMapReverseMappingGet(devNum,
                        CPSS_DXCH_PORT_MAPPING_TYPE_ETHERNET_MAC_E, evExtData,
                        &evExtData);
                    if(GT_OK != rc)
                    {
                        DBG_LOG(("prvCpssDxChPortPhysicalPortMapReverseMappingGet:rc=%d,portNum=%d\n",
                                        rc, evExtData,3,4,5,6));
                        return rc;
                    }

                    rc = cpssDxChPortApPortConfigGet(devNum, evExtData, &apEnable,
                        &apParams);
                    if(rc != GT_OK)
                    {
                        DBG_LOG(("cpssDxChPortApPortConfigGet:rc=%d,portNum=%d\n",
                                        rc, evExtData,3,4,5,6));
                        return rc;
                    }

                    if(apEnable)
                    {
                        rc = cpssDxChPortLinkStatusGet(devNum, evExtData, &linkUp);
                        if (rc != GT_OK)
                        {
                            DBG_LOG(("cpssDxChPortLinkStatusGet:rc=%d,portNum=%d\n",
                                            rc, evExtData,3,4,5,6));
                            return rc;
                        }

                        rc = cpssDxChPortEnableSet(devNum, evExtData, linkUp);
                        if(rc != GT_OK)
                        {
                            DBG_LOG(("cpssDxChPortEnableSet:rc=%d,portNum=%d,linkUp=%d\n",
                                            rc, evExtData,linkUp,4,5,6));
                            return rc;
                        }
                    }
                }
            }

            evExtData = evExtDataOld;

            if(PRV_CPSS_SIP_5_20_CHECK_MAC(devNum))
            {
                rc = sip5_20_linkChange(devNum, evExtData/* mac port number*/);
                if (rc != GT_OK)
                {
                    DBG_LOG(("sip5_20_linkChange: error, devNum=%d, port=%d, rc=%d\n",
                            devNum, evExtData, rc, 4, 5, 6));
                }
            }
            else
#endif
            /* use function appDemoPrintLinkChangeFlagSet to modify the : printLinkChangeEnabled */
            if(printLinkChangeEnabled)
            {
#ifdef DXCH_CODE
                if (CPSS_IS_DXCH_FAMILY_MAC(PRV_CPSS_PP_MAC(devNum)->devFamily))
                {
                    rc = cpssDxChPortLinkStatusGet(devNum, evExtData, &linkUp);
                    if (rc != GT_OK)
                    {
                        return rc;
                    }
                    cpssOsPrintSync("LINK %s : devNum[%ld] port[%ld] \n",
                                        (linkUp == GT_FALSE) ? "DOWN" : "UP",
                                                    (GT_U32)devNum, evExtData);
                }
                else
#endif
                    osPrintf(" LINK CHANGED : devNum[%ld] port[%ld] \n",
                                                    (GT_U32)devNum, evExtData);
            }

#ifdef DXCH_CODE
            if ((PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_DXCH_XCAT_E) ||
                (PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_DXCH_XCAT2_E))
            {
                waArr[0] = CPSS_DXCH_IMPLEMENT_WA_2_5G_SGMII_LINK_UP_E;
                additionalInfoBmpArr[0] = evExtData;
                rc = cpssDxChHwPpImplementWaInit(devNum,1,&waArr[0],
                                                 &additionalInfoBmpArr[0]);
                if (GT_OK != rc)
                {
                    DBG_LOG(("cpssDxChHwPpImplementWaInit: error, devNum=%d, port=%d, rc=%d\n",
                            devNum, evExtData, rc, 4, 5, 6));
                }
            }
#ifndef ASIC_SIMULATION
            else if(PRV_CPSS_DXCH_BOBCAT2_A0_CHECK_MAC(devNum))
            {

                /*****************************************************************************/
                /* Important note: for BC2 evExtData -> MAC port number (NOT physical) */
                /*****************************************************************************/

                waArr[0] = CPSS_DXCH_IMPLEMENT_WA_TRI_SPEED_PORT_AN_FC_E;
                additionalInfoBmpArr[0] = evExtData;
                rc = cpssDxChHwPpImplementWaInit(devNum,1,&waArr[0],
                                                 &additionalInfoBmpArr[0]);
                if (GT_OK != rc)
                {
                    DBG_LOG(("cpssDxChHwPpImplementWaInit: error, devNum=%d, port=%d, rc=%d\n",
                            devNum, evExtData, rc, 4, 5, 6));
                }
            }
#endif
#endif
            break;
        case CPSS_PP_BM_MISC_E: if( evExtData > 2 )
                                {
                                    break;
                                }
        case CPSS_PP_BM_PORT_RX_BUFFERS_CNT_UNDERRUN_E:
        case CPSS_PP_BM_PORT_RX_BUFFERS_CNT_OVERRUN_E:
        case CPSS_PP_BM_WRONG_SRC_PORT_E:  if(IS_API_EXMXPM_DEV_MAC(devNum))/* for ExMxPm those event haven't */
                                           {
                                               /* for ExMxPm those event haven't yet been defined as HW critical */
                                               break;
                                           }
        case CPSS_PP_CRITICAL_HW_ERROR_E:
        case CPSS_PP_PORT_PCS_PPM_FIFO_UNDERRUN_E:
        case CPSS_PP_PORT_PCS_PPM_FIFO_OVERRUN_E:

            if(prvAppDemoFatalErrorType != APP_DEMO_FATAL_ERROR_SILENT_TYPE_E)
            {
                osPrintf(" Critical HW Error : devNum[%d], uniEv=[%d], extraData[%d]\n", devNum, uniEv, evExtData);
            }

            if(prvAppDemoFatalErrorType == APP_DEMO_FATAL_ERROR_EXECUTE_TYPE_E)
            {
                osFatalError(OS_FATAL_WARNING, "");
            }
            break;

#ifdef DXCH_CODE
        case CPSS_PP_PORT_PRBS_ERROR_QSGMII_E:
            /* in XCAT2 B1 this interrupt means Signal Detect change */
            if((GT_TRUE == catchSignalDetectInterrupt)
                && (PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_DXCH_XCAT2_E)
                && (PRV_CPSS_PP_MAC(devNum)->revision >= 3))
            {
                GT_U8 portNum;

                portNum = (GT_U8)evExtData;

                if(printLinkChangeEnabled)
                {/* use function appDemoPrintLinkChangeFlagSet to modify the:
                    printLinkChangeEnabled */
                    osPrintf(" Signal Detect change : devNum[%ld] port[%ld] \n",(GT_U32)devNum,portNum);
                }

                if(PRV_CPSS_PORT_GE_E == PRV_CPSS_DXCH_PORT_TYPE_MAC(devNum,portNum))
                {
                    GT_U8     optionsArrayLen = 2;
                    CPSS_PORT_MODE_SPEED_STC currentMode;
                    CPSS_PORT_MODE_SPEED_STC testPortModeSpeedOptionsArray[] =
                    {
                        {CPSS_PORT_INTERFACE_MODE_SGMII_E,      CPSS_PORT_SPEED_1000_E},
                        {CPSS_PORT_INTERFACE_MODE_1000BASE_X_E, CPSS_PORT_SPEED_1000_E}
                    };

                    portNum = (GT_U8)evExtData;

                    rc = cpssDxChPortModeSpeedAutoDetectAndConfig(devNum,
                            portNum, testPortModeSpeedOptionsArray,
                            optionsArrayLen, &currentMode);
                }
            }
            break;

#ifndef ASIC_SIMULATION
        case CPSS_PP_PORT_LANE_SYNC_STATUS_CHANGED_E:
            if (PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_DXCH_LION2_E)
            {
                CPSS_PORT_INTERFACE_MODE_ENT    ifMode;
                GT_PHYSICAL_PORT_NUM            portNum;

                portNum = evExtData>>8;
                TRAINING_DBG_PRINT_MAC(("get LANE_SYNC_STATUS_CHANGED:portNum=%d\n", portNum));
                rc = cpssDxChPortInterfaceModeGet(devNum, portNum, &ifMode);
                TRAINING_DBG_PRINT_MAC(("cpssDxChPortInterfaceModeGet:portNum=%d,ifMode=%d,rc=%d\n",
                        portNum, ifMode, rc));
                if((GT_OK == rc) && (CPSS_PORT_INTERFACE_MODE_RXAUI_E == ifMode))
                {
                    if(0 == waTSmid)
                    {
                        GT_U32   waTaskId;

                        CPSS_PORTS_BMP_PORT_CLEAR_ALL_MAC(&todoWaBmp);
                        if (cpssOsSigSemBinCreate("waTSem", CPSS_OS_SEMB_EMPTY_E, &waTSmid) != GT_OK)
                        {
                            return GT_NO_RESOURCE;
                        }
                        TRAINING_DBG_PRINT_MAC(("create waTask\n"));
                        rc = cpssOsTaskCreate("waTask",    /* Task Name      */
                                              210,              /* Task Priority  */
                                              _32K,             /* Stack Size     */
                                              lion2WaTask,      /* Starting Point */
                                              (GT_VOID*)((GT_UINTPTR)devNum), /* Arguments list */
                                              &waTaskId);       /* task ID        */
                        if(rc != GT_OK)
                        {
                            cpssOsPrintSync("LANE_SYNC_STATUS_CHANGED(%d):failed to create links WA task:rc=%d\n",
                                                                    portNum, rc);
                        }
                    } /* if(0 == waTSmid) */

                    if(0 == CPSS_PORTS_BMP_IS_PORT_SET_MAC(&todoWaBmp, portNum))
                    {
                        locksTimeoutArray[portNum] = 0;
                        CPSS_PORTS_BMP_PORT_SET_MAC(&todoWaBmp, portNum);
                        rc = cpssOsSigSemSignal(waTSmid);
                        if(rc != GT_OK)
                        {
                            cpssOsPrintSync("LANE_SYNC_STATUS_CHANGED:cpssOsSigSemSignal:rc=%d\n", rc);
                        }
                        TRAINING_DBG_PRINT_MAC(("set todoWaBmp portNum=%d\n", portNum));
                    }
                }
            }
            break;

        case CPSS_PP_PORT_MMPCS_SIGNAL_DETECT_CHANGE_E:
            if (PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_DXCH_LION2_E)
            {
                GT_PHYSICAL_PORT_NUM    portNum;
                GT_U32                  serdesTraining;

                portNum = (GT_U8)evExtData;
                TRAINING_DBG_PRINT_MAC(("get MMPCS_SIGNAL_DETECT_CHANGE:portNum=%d\n", portNum));
                rc = prvAppDemoUserHandleDxChComboPort(devNum, portNum);
                if(rc != GT_OK)
                {
                    cpssOsPrintSync("prvAppDemoUserHandleDxChComboPort(portNum=%d):rc=%d\n",
                                    portNum, rc);
                }

                if(appDemoDbEntryGet("serdesTraining", &serdesTraining) != GT_OK)
                {
                    serdesTraining = 1;
                }

                if(serdesTraining)
                {
                    if(0 == tuneSmid)
                    {
                        GT_U32   tuneStartTaskId;

                        if(cpssOsSigSemBinCreate("tuneSem", CPSS_OS_SEMB_EMPTY_E, &tuneSmid) != GT_OK)
                        {
                            cpssOsPrintSync("MMPCS_SIGNAL_DETECT(portNum=%d) tuneSem create:rc=%d\n",
                                            portNum, rc);
                        }
                        CPSS_PORTS_BMP_PORT_CLEAR_ALL_MAC(&todoTuneBmp);
                        rc = cpssOsTaskCreate("tuneStrt",               /* Task Name      */
                                              210,                        /* Task Priority  */
                                              _64K,                     /* Stack Size     */
                                              appDemoDxChLion2SerdesAutoTuningStartTask,   /* Starting Point */
                                              (GT_VOID*)((GT_UINTPTR)devNum),         /* Arguments list */
                                              &tuneStartTaskId);               /* task ID        */
                        if (rc != GT_OK)
                        {
                            cpssOsPrintSync("MMPCS_SIGNAL_DETECT(portNum=%d) tuneStrt create:rc=%d\n",
                                            portNum, rc);
                        }
                    }

                    CPSS_PORTS_BMP_PORT_SET_MAC(&todoTuneBmp, portNum);
                    rc = cpssOsSigSemSignal(tuneSmid);
                    if(rc != GT_OK)
                    {
                        cpssOsPrintSync("MMPCS_SIGNAL_DETECT(portNum=%d) cpssOsSigSemSignal(tuneSmid):rc=%d\n",
                                        portNum, rc);
                    }
                }
                else
                {
                    if(0 == waTSmid)
                    {
                        GT_U32   waTaskId;

                        CPSS_PORTS_BMP_PORT_CLEAR_ALL_MAC(&todoWaBmp);
                        if (cpssOsSigSemBinCreate("waTSem", CPSS_OS_SEMB_EMPTY_E, &waTSmid) != GT_OK)
                        {
                            return GT_NO_RESOURCE;
                        }
                        rc = cpssOsTaskCreate("waTask",    /* Task Name      */
                                              210,              /* Task Priority  */
                                              _32K,             /* Stack Size     */
                                              lion2WaTask,      /* Starting Point */
                                              (GT_VOID*)((GT_UINTPTR)devNum), /* Arguments list */
                                              &waTaskId);       /* task ID        */
                        if(rc != GT_OK)
                        {
                            cpssOsPrintSync("failed to create links WA task:rc=%d\n", rc);
                        }
                    }

                    if(0 == CPSS_PORTS_BMP_IS_PORT_SET_MAC(&todoWaBmp, portNum))
                    {
                        locksTimeoutArray[portNum] = 0;
                        CPSS_PORTS_BMP_PORT_SET_MAC(&todoWaBmp, portNum);
                        rc = cpssOsSigSemSignal(waTSmid);
                        if(rc != GT_OK)
                        {
                            cpssOsPrintSync("MMPCS_SIGNAL_DETECT(portNum=%d) cpssOsSigSemSignal(waTSmid):rc=%d\n",
                                            portNum, rc);
                        }
                    }
                }
            }
            else if(PRV_CPSS_SIP_5_CHECK_MAC(devNum) && (1 == mmpcsPrint))
            {
                GT_PHYSICAL_PORT_NUM  portNum;
                GT_U32                laneNum;

                portNum = evExtData>>8;
                laneNum = evExtData&0xff;
                /* printf preferable, because printSync not implemented for every CPU */
                cpssOsPrintf("MMPCS_SIGNAL_DETECT:devNum=%d,portNum=%d,laneNum=%d\n",
                    devNum, portNum, laneNum);
            }
#endif /* ASIC_SIMULATION */
#endif /* DXCH_CODE */

#ifdef PM_CODE
        case CPSS_PP_PORT_MMPCS_SIGNAL_DETECT_CHANGE_E:
            if((PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_PUMA3_E) &&
                                (PRV_CPSS_PP_MAC(devNum)->revision > 0))
            {
                GT_U8 portNum;
                portNum = (GT_U8)evExtData;
                /* Execute WA */
                rc = appDemoExMxPmPuma3PortLinkWa(devNum,portNum);
            }
#endif
            break;

#ifdef DXCH_CODE
        case CPSS_PP_DATA_INTEGRITY_ERROR_E:
            if(PRV_CPSS_DXCH_LION2_FAMILY_CHECK_MAC(devNum))
            {
                rc = appDemoDxChLion2DataIntegrityScan(devNum, evExtData);
            }
            break;
#endif
        case CPSS_PP_PORT_EEE_E:
            if (printEeeInterruptInfo)
            {
                GT_PHYSICAL_PORT_NUM    portNum;
                GT_U32                  subEvent = (evExtData & 0xFF);

                portNum = evExtData>>8;
                osPrintf("EEE Interrupt: devNum[%ld] port[%ld] subEvent [%ld] \n",
                                                (GT_U32)devNum, portNum , subEvent);
            }
            break;
        case CPSS_PP_PORT_SYNC_STATUS_CHANGED_E:
            if (PRV_CPSS_DXCH_CETUS_CHECK_MAC(devNum) || PRV_CPSS_DXCH_CAELUM_CHECK_MAC(devNum))
            {                                                                                       
                portMacNum = evExtData;

                rc = cpssDxChPortPhysicalPortMapReverseMappingGet(devNum, CPSS_DXCH_PORT_MAPPING_TYPE_ETHERNET_MAC_E, portMacNum,
                                                                  /*OUT*/&portNum);
                if(GT_OK != rc)
                {
                    DBG_LOG(("prvCpssDxChPortPhysicalPortMapReverseMappingGet:rc=%d,portMacNum=%d\n",
                                    rc, portMacNum,3,4,5,6));
                    return rc;
                }

                rc = AN_WA_Task_SyncChangeEventBuildAndSend(devNum,portMacNum);
            }
            break;
        case CPSS_PP_PORT_AN_COMPLETED_E:
            if (PRV_CPSS_DXCH_CETUS_CHECK_MAC(devNum) || PRV_CPSS_DXCH_CAELUM_CHECK_MAC(devNum))
            {
                portMacNum = evExtData;
                rc = cpssDxChPortPhysicalPortMapReverseMappingGet(devNum, CPSS_DXCH_PORT_MAPPING_TYPE_ETHERNET_MAC_E, portMacNum,
                                                                  /*OUT*/&portNum);
                if(GT_OK != rc)
                {
                    DBG_LOG(("prvCpssDxChPortPhysicalPortMapReverseMappingGet:rc=%d,portMacNum=%d\n",
                                    rc, portMacNum,3,4,5,6));
                    return rc;
                }
                rc = AN_WA_Task_AnCompleteEventBuildAndSend(devNum,portMacNum);
            }
            break;
        default:
            break;
    }

    return rc;
}


/*******************************************************************************
* appDemoXbarEvTreat
*
* DESCRIPTION:
*       This routine handles XBAR events.
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
static GT_STATUS appDemoXbarEvTreat
(
    GT_U8                   devNum,
    GT_U32                  uniEv,
    GT_U32                  evExtData
)
{

        /* TODO: add XBar event handlers here */

        return GT_OK;
}
/*******************************************************************************
* appDemoFaEvTreat
*
* DESCRIPTION:
*       This routine handles FA events.
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
static GT_STATUS appDemoFaEvTreat
(
    GT_U8                   devNum,
    GT_U32                  uniEv,
    GT_U32                  evExtData
)
{

        /* TODO: add XBar event handlers here */

        return GT_OK;
}

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
static unsigned __TASKCONV appDemoEvHndlr
(
    GT_VOID * param
)
{
    GT_STATUS           rc;                                         /* return code         */
    GT_U32              i;                                          /* iterator            */
    GT_UINTPTR          evHndl;                                     /* event handler       */
    GT_U32              evBitmapArr[CPSS_UNI_EV_BITMAP_SIZE_CNS];   /* event bitmap array  */
    GT_U32              evBitmap;                                   /* event bitmap 32 bit */
    GT_U32              evExtData;                                  /* event extended data */
    GT_U8               devNum;                                     /* device number       */
    GT_U32              uniEv;                                      /* unified event cause */
    GT_U32              evCauseIdx;                                 /* event index         */
    EV_HNDLR_PARAM      *hndlrParamPtr;                             /* bind event array    */
#ifdef APPDEMO_REQ_MODE_DEBUG
    GT_U32 tid, prio;
#endif

    hndlrParamPtr = (EV_HNDLR_PARAM*)param;
    evHndl        = hndlrParamPtr->evHndl;


    /*osPrintf("appDemoEvHndlr[%d]: created \n",
        hndlrParamPtr->hndlrIndex);*/

    /* indicate that task start running */
    taskCreated = 1;
    osTimerWkAfter(1);

    while (1)
    {
        rc = cpssEventSelect(evHndl, NULL, evBitmapArr, (GT_U32)CPSS_UNI_EV_BITMAP_SIZE_CNS);
        if(eventRequestDrvnModeReset)
        {
            break;
        }

        if (GT_OK != rc)
        {
            DBG_LOG(("appDemoCpssEventSelect: err [%d]\n", rc, 2, 3, 4, 5, 6));
            continue;
        }

        for (evCauseIdx = 0; evCauseIdx < CPSS_UNI_EV_BITMAP_SIZE_CNS; evCauseIdx++)
        {
            if (evBitmapArr[evCauseIdx] == 0)
            {
                continue;
            }

            evBitmap = evBitmapArr[evCauseIdx];

            for (i = 0; evBitmap; evBitmap >>= 1, i++)
            {
                if ((evBitmap & 1) == 0)
                {
                    continue;
                }

                uniEv = (evCauseIdx << 5) + i;

                if (cpssEventRecv(evHndl, uniEv, &evExtData, &devNum) == GT_OK)
                {
#ifdef APPDEMO_REQ_MODE_DEBUG
                    osTaskGetSelf(&tid);
                    osGetTaskPrior(tid, &prio);
                    DBG_LOG(("cpssEventRecv: %d <dev %d, %s, extData %d> tid 0x%x prio %d\n",
                             (GT_U32)hndlrParamPtr->hndlrIndex, devNum,
                             (GT_U32)uniEvName[uniEv], evExtData, tid, prio));
#endif
                    if (/*(uniEv >= CPSS_PP_UNI_EV_MIN_E) &&*/
                        (uniEv <= CPSS_PP_UNI_EV_MAX_E))
                    {
                        rc = appDemoEnPpEvTreat(devNum, uniEv, evExtData);
                    }
                    else if ((uniEv >= CPSS_XBAR_UNI_EV_MIN_E) &&
                             (uniEv <= CPSS_XBAR_UNI_EV_MAX_E))
                    {
                        rc = appDemoXbarEvTreat(devNum, uniEv, evExtData);
                    }
                    else if ((uniEv >= CPSS_FA_UNI_EV_MIN_E) &&
                             (uniEv <= CPSS_FA_UNI_EV_MAX_E))
                    {
                        rc = appDemoFaEvTreat(devNum, uniEv, evExtData);
                    }

                    /* counting the event */
                    if(eventIncrementFunc != NULL)
                    {
                        /* don't care about the return value */
                        (GT_VOID)eventIncrementFunc(devNum, uniEv, evExtData);
                    }

                    if (GT_OK != rc)
                    {
                        DBG_LOG(("appDemoEnPpEvTreat: error - 0x%x, uniEv [%d]\n", rc, uniEv, 3, 4, 5, 6));
                    }
                }
            }
        }
    }

    /* give indication that the thread exit */
    /*osPrintf("appDemoEvHndlr[%d]: thread exit during system reset \n",
        hndlrParamPtr->hndlrIndex);*/

    hndlrParamPtr->hndlrIndex = FREE_TASK_CNS;

    return 0;
}

static GT_U32 rx2cpu_queue_wrr[] = {APP_DEMO_SINGLE_RX_TASK_P0_WEIGHT,
                                    APP_DEMO_SINGLE_RX_TASK_P1_WEIGHT,
                                    APP_DEMO_SINGLE_RX_TASK_P2_WEIGHT,
                                    APP_DEMO_SINGLE_RX_TASK_P3_WEIGHT,
                                    APP_DEMO_SINGLE_RX_TASK_P4_WEIGHT,
                                    APP_DEMO_SINGLE_RX_TASK_P5_WEIGHT,
                                    APP_DEMO_SINGLE_RX_TASK_P6_WEIGHT,
                                    APP_DEMO_SINGLE_RX_TASK_P7_WEIGHT};

/*******************************************************************************
* appDemoSingleRxEvHndlr
*
* DESCRIPTION:
*       This routine is the event handler for all RX events for all queues.
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
static unsigned __TASKCONV appDemoSingleRxEvHndlr
(
    GT_VOID * param
)
{
    GT_STATUS           rc;                                         /* return code         */
    GT_UINTPTR          evHndl;                                     /* event handler       */
    GT_U32              evBitmapArr[CPSS_UNI_EV_BITMAP_SIZE_CNS];   /* event bitmap array  */
    GT_U32              evExtData;                                  /* event extended data */
    GT_U8               devNum;                                     /* device number       */
    EV_HNDLR_PARAM      *hndlrParamPtr;                             /* bind event array    */

    GT_BOOL cpuRxNotEmpty[PRV_CPSS_MAX_PP_DEVICES_CNS][8] = {{0}};
    GT_U32 queue;
    GT_U32 maxPacket;
    GT_U8 dev;
    GT_U32 counter =0;
    GT_U32 fullQueues =0;
    hndlrParamPtr = (EV_HNDLR_PARAM*)param;
    evHndl        = hndlrParamPtr->evHndl;


    /*osPrintf("appDemoSingleRxEvHndlr[%d]: created \n",
        hndlrParamPtr->hndlrIndex);*/

    /* indicate that task start running */
    taskCreated = 1;

    while (1)
    {
        rc = cpssEventSelect(evHndl, NULL, evBitmapArr, (GT_U32)CPSS_UNI_EV_BITMAP_SIZE_CNS);
        if(eventRequestDrvnModeReset)
        {
            break;
        }
        if (GT_OK != rc)
        {
            DBG_LOG(("cpssEventSelect: err [%d]\n", rc, 2, 3, 4, 5, 6));
            continue;
        }
        do
        {
            for (queue = 0; queue < 8; queue++)
            {
                do
                {
                    /* get all the events for current queue*/
                    rc = cpssEventRecv(evHndl,CPSS_PP_RX_BUFFER_QUEUE0_E+queue,&evExtData,&devNum);
                    if (rc == GT_OK)
                    {
                        /* counting the event */
                        if(eventIncrementFunc != NULL)
                        {
                                (GT_VOID)eventIncrementFunc(devNum, CPSS_PP_RX_BUFFER_QUEUE0_E+queue, 0);
                        }
                        cpuRxNotEmpty[devNum][queue] = GT_TRUE;
                    }
                    rc = cpssEventRecv(evHndl,CPSS_PP_RX_ERR_QUEUE0_E+queue,&evExtData,&devNum);
                    if (rc == GT_OK)
                    {
                        /* counting the event */
                        if(eventIncrementFunc != NULL)
                        {
                                (GT_VOID)eventIncrementFunc(devNum, CPSS_PP_RX_ERR_QUEUE0_E+queue, 0);
                        }
                        cpuRxNotEmpty[devNum][queue] = GT_TRUE;
                    }
                }
                while (rc == GT_OK);

                maxPacket = rx2cpu_queue_wrr[queue];
                for(dev = SYSTEM_DEV_NUM_MAC(0); dev < SYSTEM_DEV_NUM_MAC(appDemoPpConfigDevAmount); dev++)
                {
                    if(appDemoPpConfigList[dev].valid == GT_FALSE || cpuRxNotEmpty[dev][queue] == GT_FALSE)
                    {
                        counter++;
                        continue;
                    }
                    /* get up to maxPacket */
                    rc = appDemoGetMaxPackets((GT_U8)appDemoPpConfigList[dev].devNum,(GT_U8)queue,maxPacket);
                    if (rc == GT_NO_MORE)
                    {
                        counter++;
                        cpssEventTreatedEventsClear(evHndl);
                        cpuRxNotEmpty[dev][queue] = GT_FALSE;
                    }
                }
                if (counter >= appDemoPpConfigDevAmount)
                {
                    U32_SET_FIELD_MAC(fullQueues,queue,1,0);
                }
                else
                {
                    U32_SET_FIELD_MAC(fullQueues,queue,1,1);
                }
                counter = 0;
            }
        } while ( fullQueues );
    }

    /* give indication that the thread exit */
    /*osPrintf("appDemoSingleRxEvHndlr[%d]: thread exit during system reset \n",
        hndlrParamPtr->hndlrIndex);*/
    hndlrParamPtr->hndlrIndex = FREE_TASK_CNS;

    return 0;
}


/*******************************************************************************
* appDemoSingleRxTreatTask
*
* DESCRIPTION:
*       This routine treats Rx packets in single Rx task mode.
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
static unsigned __TASKCONV appDemoSingleRxTreatTask
(
    GT_VOID * param
)
{
    GT_STATUS           rc; /* return code         */
    GT_U32              size; /* size of buffer pointed by message */
    GT_U8               devNum;
    CPSS_OS_MSGQ_ID    *msgQIdPtr;
    APP_DEMO_RX_PACKET_PARAMS  rxParams;
    msgQIdPtr = (CPSS_OS_MSGQ_ID*)param;

    size = sizeof(APP_DEMO_RX_PACKET_PARAMS);

    while (1)
    {
        rc = cpssOsMsgQRecv(*msgQIdPtr,&rxParams,&size,CPSS_OS_MSGQ_WAIT_FOREVER);
        if (rc != GT_OK)
        {
            DBG_LOG(("cpssOsMsgQRecv: err [%d]\n", rc, 2, 3, 4, 5, 6));
            continue;
        }
        devNum = rxParams.devNum;
        rc = GT_NOT_IMPLEMENTED;
        if(IS_API_CH_DEV_MAC(devNum))/* is device support dxch API ? */
        {
#ifdef DXCH_CODE
            /* apply the App Demo treatment for the new packet */
            rc = appDemoDxChNetRxPktTreat(&rxParams);
#endif /*DXCH_CODE*/
        }
        else if(IS_API_EXMXPM_DEV_MAC(devNum))/* is device support ExMxPm API ? */
        {
#ifdef PM_CODE
            /* apply the App Demo treatment for the new packet */
            rc = appDemoExMxPmNetRxPktTreat(&rxParams);
#endif /*PM_CODE*/
        }

        if (rc != GT_OK)
        {
            DBG_LOG(("XXX_NetRxPktTreat: err [%d]\n", rc, 2, 3, 4, 5, 6));
        }
    }

#ifdef __GNUC__
    /* to avoid warnings in gcc */
    return 0;
#endif

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
static GT_STATUS cpssEnRxPacketGet
(
    IN RX_EV_HANDLER_EV_TYPE_ENT evType,
    IN GT_U8                devNum,
    IN GT_U8                queue
)
{
    GT_STATUS       rc = GT_NOT_IMPLEMENTED;

    do
    {
        if(IS_API_CH_DEV_MAC(devNum))/* is device support dxch API ? */
        {
#ifdef DXCH_CODE
            /* apply the App Demo treatment for the new packet */
            rc = appDemoDxChNetRxPktHandle(rxEventHanderHnd, evType, devNum, queue);
#endif /*DXCH_CODE*/
        }

        if(IS_API_EXMXPM_DEV_MAC(devNum))/* is device support ExMxPm API ? */
        {
#ifdef PM_CODE
            /* apply the App Demo treatment for the new packet */
            rc = appDemoExMxPmNetRxPktHandle(rxEventHanderHnd, evType, devNum, queue);
#endif /*PM_CODE*/
        }

        if(IS_API_EXMX_DEV_MAC(devNum)) /* is device support ExMx API ? */
        {
            rc = appDemoGenNetRxPktHandle(devNum, queue);
        }

    }while(rc == GT_OK);

    return rc;
}

/*******************************************************************************
* appDemoGetMaxPackets
*
* DESCRIPTION:
*       This routine get the RX packets from given device and queue
*       up to maxPacket.
*
* INPUTS:
*       devNum  - the device number.
*       queue    - the queue the rx event occurred upon
*       maxPacket - maximum packets to get from the queue
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
static GT_STATUS appDemoGetMaxPackets
(
    IN GT_U8                devNum,
    IN GT_U8                queue,
    IN GT_U32               maxPacket
)
{
    GT_STATUS       rc = GT_NOT_IMPLEMENTED; /* The returned code            */
    GT_U32          counter = 0; /* counter */
    CPSS_OS_MSGQ_ID msgQId;
    GT_U32 number;

    do
    {
        /* choose msgQ */
        number = (singleTaskRxRandomEnable == GT_TRUE) ?
                    cpssOsRand() : queue;
        msgQId = singleRxMsgQIds[number % treatTasksNum];

        if(IS_API_CH_DEV_MAC(devNum))/* is device support dxch API ? */
        {
#ifdef DXCH_CODE
            /* apply the App Demo treatment for the new packet */
            rc = appDemoDxChNetRxPktGet(devNum, queue,msgQId);
#endif /*DXCH_CODE*/
        }
        else if(IS_API_EXMXPM_DEV_MAC(devNum))/* is device support ExMxPm API ? */
        {
#ifdef PM_CODE
            /* apply the App Demo treatment for the new packet */
            rc = appDemoExMxPmNetRxPktGet(devNum, queue,msgQId);
#endif /*PM_CODE*/
        }
        else
        {
            rc = GT_NOT_IMPLEMENTED;
        }

        counter++;

    }while(rc == GT_OK && counter < maxPacket);

    return rc;
}

/*******************************************************************************
* appDemoEventsDataBaseGet
*
* DESCRIPTION:
*       this method is a getter to the events-table database
*
*
*
* INPUTS:
*       eventCounterBlockGet - a pointer to a database which the method will fill in
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK   - on success
*
* COMMENTS:
*       None.
*
*******************************************************************************/

GT_STATUS appDemoEventsDataBaseGet
(
        GT_U32 ***eventCounterBlockGet
)
{
    *eventCounterBlockGet = uniEventCounters;
    return GT_OK;
}

/*******************************************************************************
* appDemoGenEventCounterIncrement
*
* DESCRIPTION:
*       increment the event counter according to the device number and the event type.
*                Allocate memory for the database on first call
*
*
*
* INPUTS:
*       devNum - device number
*       uniEvCounter - number which represent the event type
*       evExtData    - extended data of event
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK   - on success
*                GT_BAD_PARAM - in case of bad parameters
*                GT_OUT_OF_CPU_MEM - in case of running out of memory when initialize the database
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS appDemoGenEventCounterIncrement
(
    IN  GT_U8   devNum,
    IN GT_U32   uniEvCounter,
    IN GT_U32   evExtData
)
{
    GT_U32 eventsNum = CPSS_UNI_EVENT_COUNT_E; /* number of events */
    GT_BOOL wasNullBeforeCall = GT_FALSE;

    if(devNum >= MAX_NUM_DEVICES)
    {
        /* check device out of range */
        return GT_BAD_PARAM;
    }

    /* Counting the event */
    if(uniEventCounters[devNum] == NULL)
    {
        wasNullBeforeCall = GT_TRUE;
        uniEventCounters[devNum] =  (GT_U32 *)osMalloc((eventsNum) * sizeof(GT_U32));

        if(uniEventCounters[devNum] == NULL)
            return GT_OUT_OF_CPU_MEM;

        osMemSet(uniEventCounters[devNum], 0, ((eventsNum) * sizeof(GT_U32)));
    }

    if(uniEvCounter == 0xFFFFFFFF && wasNullBeforeCall == GT_TRUE)
    {
        /* just trigger to allocate the needed memory for the device (during initialization)
           and not to wait till the first event.

           because the 'Enhanced UT' will check for memory leakage , and if first
           event will come only after the test started , then the test will fail .
            */
        return GT_OK;
    }

    if (uniEvCounter >= eventsNum)
    {
        return GT_BAD_PARAM;
    }

    osMutexLock(appDemoEventHandlerCounterUpdateLockMtx);

    uniEventCounters[devNum][uniEvCounter]++;

    if (wrapCpssTraceEvents)
    {
        osPrintf(" Event %d evExtData %d amount %d\n", uniEvCounter, evExtData, uniEventCounters[devNum][uniEvCounter]);
    }

    osMutexUnlock(appDemoEventHandlerCounterUpdateLockMtx);

    return GT_OK;
}



GT_U32 wrapCpssTraceEventsEnable
(
    IN GT_U32 enable
)
{
    GT_U32 tmp = wrapCpssTraceEvents;
    wrapCpssTraceEvents = enable;
    return tmp;
}

/*******************************************************************************
* appDemoGenEventCounterGet
*
* DESCRIPTION:
*     get the number of times that specific event happened.
*
* INPUTS:
*     devNum      - device number
*     uniEvent    - unified event
*     clearOnRead - do we 'clear' the counter after 'read' it
*               GT_TRUE - set counter to 0 after get it's value
*               GT_FALSE - don't update the counter (only read it)
* OUTPUTS:
*     counterPtr  - (pointer to)the counter (the number of times that specific event happened)
*
* RETURNS:
*     GT_OK              - on success.
*     GT_BAD_PARAM       - on bad devNum or uniEvent.
*     GT_BAD_PTR         - on NULL pointer.
*     GT_NOT_INITIALIZED - the counters DB not initialized for the device.
*
* COMMENTS:
*     none
*
*******************************************************************************/
GT_STATUS appDemoGenEventCounterGet
(
    IN GT_U8                    devNum,
    IN CPSS_UNI_EV_CAUSE_ENT    uniEvent,
    IN GT_BOOL                  clearOnRead,
    OUT GT_U32                 *counterPtr
)
{
    if(devNum >= MAX_NUM_DEVICES || uniEvent >= CPSS_UNI_EVENT_COUNT_E)
    {
        /* check device out of range */
        return GT_BAD_PARAM;
    }

    if(counterPtr == NULL && clearOnRead != GT_TRUE)
    {
        return GT_BAD_PTR;
    }

    /* Counting the event */
    if(uniEventCounters[devNum] == NULL)
    {
        return GT_NOT_INITIALIZED;
    }

    osMutexLock(appDemoEventHandlerCounterUpdateLockMtx);

    if (counterPtr != NULL)
    {
        *counterPtr = uniEventCounters[devNum][uniEvent];
    }

    if(clearOnRead == GT_TRUE)
    {

        uniEventCounters[devNum][uniEvent] = 0;
    }

    osMutexUnlock(appDemoEventHandlerCounterUpdateLockMtx);

    return GT_OK;
}
#ifdef GM_USED
extern GT_VOID snetGmPortBmpLinkInfoGet
(
    IN  GT_U8    deviceNumber,
    IN  GT_U32   linkUpPortsBmpArr[4],
    IN  GT_U32   changedPortsBmpArr[4]
);
extern GT_STATUS   getSimDevIdFromSwDevNum
(
    IN GT_U8    swDevNum,
    IN  GT_U32  portGroupId,
    OUT GT_U32  *simDeviceIdPtr
);

static GT_U32 gm_appDemoEmulateLinkChangeTask_sleepTime = 250;
void gm_appDemoEmulateLinkChangeTask_sleepTimeSet(IN GT_U32 newTime)
{
    gm_appDemoEmulateLinkChangeTask_sleepTime = newTime + 1;/*to avoid 0*/
}

/*******************************************************************************
* gm_appDemoEmulateLinkChangeTask
*
* DESCRIPTION:
*       the GM not hold MAC and therefor not generate next event :
*        CPSS_PP_PORT_LINK_STATUS_CHANGED_E
*
*        still we have WA using simulation on top of the GM.
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
static unsigned __TASKCONV gm_appDemoEmulateLinkChangeTask
(
    GT_VOID * param
)
{
    GT_STATUS rc;        /* The returned code   */
    GT_U8     devNum;    /* device number       */
    GT_U32   linkUpPortsBmpArr[4];
    GT_U32   changedPortsBmpArr[4];
    GT_U32   macPortNum;
    GT_U32   portGroupId = 0;
    GT_U32   sim_deviceId;

    /* indicate that task start running */
    taskCreated = 1;
    osTimerWkAfter(1);

    while (1)
    {
        for(devNum = SYSTEM_DEV_NUM_MAC(0); devNum < SYSTEM_DEV_NUM_MAC(appDemoPpConfigDevAmount); devNum++)
        {
            if(appDemoPpConfigList[devNum].valid == GT_FALSE)
            {
                continue;
            }

            osTimerWkAfter(gm_appDemoEmulateLinkChangeTask_sleepTime);

            /* convert cpss devNum to simulation deviceId */
            rc = getSimDevIdFromSwDevNum(appDemoPpConfigList[devNum].devNum,portGroupId,&sim_deviceId);
            if(rc != GT_OK)
            {
                continue;
            }

            /* call the simulation to get the info */
            snetGmPortBmpLinkInfoGet((GT_U8)sim_deviceId,linkUpPortsBmpArr,changedPortsBmpArr);

            for(macPortNum = 0 ; macPortNum < 128 ; macPortNum++)
            {
                if(0 == (changedPortsBmpArr[macPortNum >> 5] & (1<<(macPortNum&0x1f))))
                {
                    continue;
                }

                /* the MAC changed it's status from last time called */
                /* so emulate the event handling */

                (void)appDemoEnPpEvTreat(appDemoPpConfigList[devNum].devNum,
                    CPSS_PP_PORT_LINK_STATUS_CHANGED_E,
                    macPortNum);

                /* counting the event */
                if(eventIncrementFunc != NULL)
                {
                    /* don't care about the return value */
                    (GT_VOID)eventIncrementFunc(devNum, CPSS_PP_PORT_LINK_STATUS_CHANGED_E, macPortNum);
                }
            }
        }

        if(gm_appDemoEmulateLinkChangeTask_sleepTime == 0)
        {
            /* avoid warning of unreachable code on the 'return 0' at the end of function */
            break;
        }
    }

    /* avoid next warning : no return statement in function returning non-void [-Werror=return-type] */
    return 0;

}
#endif /*GM_USED*/

/*******************************************************************************
* appDemoEventRequestDrvnModeReset
*
* DESCRIPTION:
*       This routine destroys event handlers tasks, delete message queues, clean
*       bindings for App Demo event handlers.
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
GT_STATUS appDemoEventRequestDrvnModeReset
(
    IN GT_VOID
)
{
    GT_STATUS rc;              /* The returned code            */
    GT_U32    i;               /* Iterator                     */
    GT_32     intKey;          /* The interrupt lock key       */

    eventRequestDrvnModeReset = 1;

    for (i = 0; i < APP_DEMO_PROCESS_NUMBER; i++)
    {
        if(evHndlrCauseAllArrSize[i] == 0 || evHndlrCauseAllArr[i][0] == CPSS_UNI_RSRVD_EVENT_E)
        {
            /*osPrintf("appDemoEvHndlr[%d]: not exists \n",
                i);*/
            continue;
        }

        /* call the CPSS to disable those interrupts in the HW of the device */
        rc = prvUniEvMaskAllSet(evHndlrCauseAllArr[i],
                                evHndlrCauseAllArrSize[i],
                                CPSS_EVENT_MASK_E);
        if (GT_OK != rc)
        {
            return GT_FAIL;
        }

#if defined(LINUX_SIM)
        /* the linux simulation during cpssEventDestroy(...) is not able to
           signal the waiting thread !!!
           so we need to first kill the thread and then to destroy the event
        */
        rc = osTaskDelete(eventHandlerTidArr[i]);
        if (rc != GT_OK)
        {
            return GT_FAIL;
        }
        taskParamArr[i].hndlrIndex = FREE_TASK_CNS;
#endif /*LINUX_SIM*/

        /* call CPSS to destroy the events */
        INT_LOCK(intKey);
        rc = cpssEventDestroy(taskParamArr[i].evHndl);
        INT_UNLOCK(intKey);
        if (GT_OK != rc)
        {
            return GT_FAIL;
        }

        while(taskParamArr[i].hndlrIndex != FREE_TASK_CNS)
        {
            /* this is indication that the thread is exited properly ... no need to kill it */
            osTimerWkAfter(1);
            /*osPrintf("8");*/
        }
        /*osPrintf("\n");*/

        taskParamArr[i].hndlrIndex = 0;
        taskParamArr[i].evHndl     = 0;
        eventHandlerTidArr[i]      = 0;
    }


    if (singleTaskRxEnable == GT_TRUE)
    {
        for (i = 0; i < treatTasksNum; i++)
        {
            /* delete rx treat task -- before deleting the mutex/semaphore */
            rc = osTaskDelete(treatRxEventHandlerTid[i]);
            if (rc != GT_OK)
            {
                return rc;
            }

            /* delete msgQ */
            rc = osMsgQDelete(singleRxMsgQIds[i]);
            if (rc != GT_OK)
            {
                return rc;
            }
            singleRxMsgQIds[i] = 0;
        }

        rc = osMutexDelete(rxMutex);
        if (rc != GT_OK)
        {
            return rc;
        }

        rxMutex = 0;

    }

    /* clean main DB */
    for (i = 0; i < APP_DEMO_PROCESS_NUMBER; i++)
    {
        evHndlrCauseAllArrSize[i] = 0;
        evHndlrCauseAllArr[i] = NULL;
    }

    /* debug flag to open trace of events */
    wrapCpssTraceEvents = 0;
    singleTaskRxEnable = GT_FALSE;
    /* is random distribution of packets to msgQs enabled */
    singleTaskRxRandomEnable = GT_FALSE;
    /* number of treat Rx tasks */
    treatTasksNum = 2;

    /* flag meaning appDemoEnPpEvTreat will catch <Signal Detect triggered> interrupt.
    Warning: <Signal Detect triggered> is new interrupt in XCAT2 B1, if enabled using
        cpssDxChPortGePrbsIntReplaceEnableSet it overrides old but not obsolete
        <QSGMII PRBS error> interrupt, so in appDemoEnPpEvTreat it's treated as
        CPSS_PP_PORT_PRBS_ERROR_QSGMII_E case */
    catchSignalDetectInterrupt = GT_FALSE;

    /* flag to enable the printings of 'link change' --
        by default not enabled because it may cause RDE tests to fail due to the
        printings , tests that cause link change.
    */
    #ifdef ASIC_SIMULATION
    printLinkChangeEnabled = 1;
    #else  /*ASIC_SIMULATION*/
    printLinkChangeEnabled = 0;
    #endif /*ASIC_SIMULATION*/

    /* flag that state the tasks may process events that relate to AUQ messages.
        this flag allow us to stop processing those messages , by that the AUQ may
        be full, or check AU storm prevention, and other.

        use function appDemoAllowProcessingOfAuqMessages(...) to set this flag
    */
    allowProcessingOfAuqMessages = GT_TRUE;

    /* fatal error handling type */
    prvAppDemoFatalErrorType = APP_DEMO_FATAL_ERROR_EXECUTE_TYPE_E;

    mmpcsPrint = 0;

    {/* destroy the counters DB + related semaphore */
        if(appDemoEventHandlerCounterUpdateLockMtx)
        {
            osMutexLock(appDemoEventHandlerCounterUpdateLockMtx);
        }

        /* free and Reset Event Counter DB */
        for(i = 0 ; i < MAX_NUM_DEVICES; i ++)
        {
            if(uniEventCounters[i] == NULL)
            {
                continue;
            }
            osFree(uniEventCounters[i]);
            uniEventCounters[i] = NULL;
        }

        if(appDemoEventHandlerCounterUpdateLockMtx)
        {
            osMutexUnlock(appDemoEventHandlerCounterUpdateLockMtx);

            osMutexDelete(appDemoEventHandlerCounterUpdateLockMtx);
            appDemoEventHandlerCounterUpdateLockMtx = 0;
        }
    }

    /* state that reset of appDemo events finished */
    eventRequestDrvnModeReset = 0;

    return GT_OK;
}



