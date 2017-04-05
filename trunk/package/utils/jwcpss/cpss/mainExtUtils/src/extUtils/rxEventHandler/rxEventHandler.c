/*******************************************************************************
*                Copyright 2014, Marvell International Ltd.
* This code contains confidential information of Marvell semiconductor, inc.
* no rights are granted herein under any patent, mask work right or copyright
* of Marvell or any third party.
* Marvell reserves the right at its sole discretion to request that this code
* be immediately returned to Marvell. This code is provided "as is".
* Marvell makes no warranties, express, implied or otherwise, regarding its
* accuracy, completeness or performance.
********************************************************************************
* rxEventHandler.h
*
* DESCRIPTION:
*       This module defines the API for CPSS_PP_RX_BUFFER_QUEUE*_E and
*       CPSS_PP_RX_ERR_QUEUE*_E event handling.
*
* DEPENDENCIES:
*
* FILE REVISION NUMBER:
*       $Revision: 2 $
*******************************************************************************/
#include <extUtils/rxEventHandler/rxEventHandler.h>
#include <cpss/extServices/private/prvCpssBindFunc.h>
/*TODO*/
/*#include <extUtils/trafficEngine/tgfTrafficTable.h>*/
#ifdef SHARED_MEMORY
#  include <sys/types.h>
#  include <unistd.h>
#  ifdef CHX_FAMILY
#    include <cpss/dxCh/dxChxGen/networkIf/cpssDxChNetIfTypes.h>
     /* CPSS_DXCH_NET_RX_PARAMS_STC */
#  endif
#  ifdef EXMXPM_FAMILY
#    include <cpss/exMxPm/exMxPmGen/networkIf/cpssExMxPmNetIfTypes.h>
     /* CPSS_EXMXPM_NET_RX_PARAMS_STC */
#  endif
#  ifdef EX_FAMILY
#    include <cpss/exMx/exMxGen/networkIf/cpssExMxGenNetIfTypes.h>
     /* CPSS_EXMX_NET_RX_PARAMS_STC */
#  endif

#endif /* SHARED_MEMORY */




#define BUFF_LEN    20

/*
 * typedef: enum RX_EV_HANDLER_CB_TYPE_ENT
 *
 * Description:
 *      Callback method
 *
 * Enumerations:
 *      RX_EV_HANDLER_CB_TYPE_UNKNOWN_E - not detected yet
 *      RX_EV_HANDLER_CB_TYPE_DIRECT_E  - direct callback, just execute
 *                                        callback function
 *      RX_EV_HANDLER_CB_TYPE_REMOTE_E  - callback function in external
 *                                        process, use remote call
 */
typedef enum
{
    RX_EV_HANDLER_CB_TYPE_UNKNOWN_E,
    RX_EV_HANDLER_CB_TYPE_DIRECT_E,
    RX_EV_HANDLER_CB_TYPE_REMOTE_E
} RX_EV_HANDLER_CB_TYPE_ENT;

#ifdef SHARED_MEMORY
#define BUFF_LEN 20
/*
 * typedef: RX_EV_HANDLER_CB_PARAMS_STC
 *
 * Description:
 *      Parameters for remote callback
 *
 * Fields:
 *      devNum      - Device number.
 *      queue       - The queue from which this packet was received.
 *      handle      - the handle of CPSS event handler
 *      evType      - event type
 *      numOfBuff   - Num of used buffs in packetBuffs.
 *      packetBuffs - The received packet buffers list.
 *      buffLenArr  - List of buffer lengths for packetBuffs.
 *      rxParams    - information parameters of received packets
 *
 */
typedef struct RX_EV_HANDLER_CB_PARAMS_STCT {
    GT_U8                       devNum;
    GT_U8                       queue;
    RX_EV_HANDLER_EV_TYPE_ENT   evType;
    GT_U32                      numOfBuff;
    GT_U8                       *packetBuffs[BUFF_LEN];
    GT_U32                      buffLen[BUFF_LEN];
    /* reserve memory for all types of rxParams */
    union   {
        char rawdata;
#ifdef CHX_FAMILY
        CPSS_DXCH_NET_RX_PARAMS_STC     dxCh;
#endif
#ifdef EXMXPM_FAMILY
        CPSS_EXMXPM_NET_RX_PARAMS_STC   exMxPm;
#endif
#ifdef EX_FAMILY
        CPSS_EXMX_NET_RX_PARAMS_STC     exMx;
#endif
    } rxParams;
} RX_EV_HANDLER_CB_PARAMS_STC;
#endif

/*
 * Typedef: RX_EV_HANDLER_CB_STC
 *
 * Description:
 *      Callback info structure
 *
 * Fields:
 *      next            - pointer to next item in chain
 *      matchDevNum     - applicable devNum or RX_EV_HANDLER_DEVNUM_ALL
 *      matchQueue      - applicable queue or RX_EV_HANDLER_QUEUE_ALL
 *      matchEvType     - event type, can be RX_EV_HANDLER_TYPE_ANY_E
 *      cbFunc          - callback function pointer
 *      cookie          - userdefined cookie value to be passed to cbFunc()
 *      numRefs         - ref count
 *      cbType          - callback method
 *
 *      pid             - process id of current process (shlib only)
 *      tid             - task id of event waiting task (remote callback)
 *      handleSem       - semaphore to wakeup event waiting task
 *      handleDoneSem   - semaphore to ack callback is done
 *      paramsPtr       - callback parameters
 *
 */
typedef struct RX_EV_HANDLER_CB_STCT {
    struct RX_EV_HANDLER_CB_STCT *next;
    GT_U8                       matchDevNum;
    GT_U8                       matchQueue;
    RX_EV_HANDLER_EV_TYPE_ENT   matchEvType;
    RX_EV_PACKET_RECEIVE_CB_FUN cbFunc;
    GT_UINTPTR                  cookie;
    GT_U32                      numRefs;
    RX_EV_HANDLER_CB_TYPE_ENT   cbType;
#ifdef SHARED_MEMORY
    pid_t                       pid;

    /* data for communication */
    CPSS_TASK                   tid;
    CPSS_OS_SIG_SEM             handleSem;
    CPSS_OS_SIG_SEM             handleDoneSem;
    RX_EV_HANDLER_CB_PARAMS_STC *paramsPtr;
#endif
} RX_EV_HANDLER_CB_STC;

#define CALLBACKS_MAX 10
/*
 * Typedef: RX_EV_HANDLER_STC
 *
 * Description:
 *      Event handler info. The types of events to be handled and
 *      a list of callbacks bound to
 *
 * Fields:
 *      next            - pointer to next item in chain
 *      devNum          - device number
 *      queue           - queue number
 *      evType          - event types
 *
 *      pid             - process id (shlib only)
 *      params          - placeholder for callback parameters
 *
 *      callBacks       - array of matched callbacks
 *
 */
typedef struct RX_EV_HANDLER_STCT
{
    struct RX_EV_HANDLER_STCT   *next;
    GT_U8                       devNum;
    GT_U8                       queue;
    RX_EV_HANDLER_EV_TYPE_ENT   evType;
#ifdef SHARED_MEMORY
    pid_t                       pid;
    RX_EV_HANDLER_CB_PARAMS_STC params;
#endif
    RX_EV_HANDLER_CB_STC        *callBacks[CALLBACKS_MAX];

} RX_EV_HANDLER_STC;

static RX_EV_HANDLER_STC     *prvRxEventHandlersList = NULL;
static RX_EV_HANDLER_CB_STC  *prvRxEventCallBacksList = NULL;
#ifdef SHARED_MEMORY
static int prvRxEvSemSeq = 0;
#endif







/*******************************************************************************
* prvRxEvHandlerMatch
*
* DESCRIPTION:
*       Initialize library
*
* INPUTS:
*       evHandler - pointer to event handler
*       devNum  - the device number or RX_EV_HANDLER_DEVNUM_ALL
*       queue   - the rx queue number or RX_EV_HANDLER_QUEUE_ALL
*       evType  - event type
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_BOOL
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_BOOL prvRxEvHandlerMatch(
  IN  RX_EV_HANDLER_STC      *evHandler,
  IN  GT_U8                     devNum,
  IN  GT_U8                     queue,
  IN  RX_EV_HANDLER_EV_TYPE_ENT    evType
)
{
    if (    (evHandler->devNum != devNum) && 
            (evHandler->devNum != RX_EV_HANDLER_DEVNUM_ALL) &&
            (devNum != RX_EV_HANDLER_DEVNUM_ALL))
    {
        return GT_FALSE;
    }
    if (    (evHandler->queue != queue) &&
            (evHandler->queue != RX_EV_HANDLER_QUEUE_ALL) &&
            (queue != RX_EV_HANDLER_QUEUE_ALL))
    {
        return GT_FALSE;
    }
    if (    (evHandler->evType != evType) && 
            (evHandler->evType != RX_EV_HANDLER_TYPE_ANY_E) &&
            (evType != RX_EV_HANDLER_TYPE_ANY_E))
    {
        return GT_FALSE;
    }
    return GT_TRUE;
}

/*******************************************************************************
* prvRxEvCbHandlerMatch
*
* DESCRIPTION:
*       Initialize library
*
* INPUTS:
*       cbHandler - pointer to event handler
*       devNum  - the device number or RX_EV_HANDLER_DEVNUM_ALL
*       queue   - the rx queue number or RX_EV_HANDLER_QUEUE_ALL
*       evType  - event type
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_BOOL
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_BOOL prvRxEvCbHandlerMatch(
  IN  RX_EV_HANDLER_CB_STC  *cbHandler,
  IN  GT_U8                     devNum,
  IN  GT_U8                     queue,
  IN  RX_EV_HANDLER_EV_TYPE_ENT    evType
)
{
    if (    (cbHandler->matchDevNum != devNum) && 
            (cbHandler->matchDevNum != RX_EV_HANDLER_DEVNUM_ALL) &&
            (devNum != RX_EV_HANDLER_DEVNUM_ALL))
    {
        return GT_FALSE;
    }
    if (    (cbHandler->matchQueue != queue) &&
            (cbHandler->matchQueue != RX_EV_HANDLER_QUEUE_ALL) &&
            (queue != RX_EV_HANDLER_QUEUE_ALL))
    {
        return GT_FALSE;
    }
    if (    (cbHandler->matchEvType != evType) && 
            (cbHandler->matchEvType != RX_EV_HANDLER_TYPE_ANY_E) &&
            (evType != RX_EV_HANDLER_TYPE_ANY_E))
    {
        return GT_FALSE;
    }
    return GT_TRUE;
}


/*******************************************************************************
* rxEventHandlerLibInit
*
* DESCRIPTION:
*       Initialize library
*
* INPUTS:
*       None
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK on success
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS rxEventHandlerLibInit(GT_VOID)
{
    /*TODO: init protection mutex */
    return GT_OK;
}

#ifdef SHARED_MEMORY
static unsigned __TASKCONV prvRxEventHandlerCbTaskFun(void *param)
{
    RX_EV_HANDLER_CB_STC *c = (RX_EV_HANDLER_CB_STC*)param;
    RX_EV_HANDLER_CB_PARAMS_STC *p;

    while (1)
    {
        cpssOsSigSemWait(c->handleSem, CPSS_OS_SEM_WAIT_FOREVER_CNS);
        p = c->paramsPtr;
        if (c->cbFunc == NULL || p == NULL)
        {
            /* special case: close thread */
            break;
        }
        if (p == NULL)
        {
            /* smth is wrong */
            cpssOsSigSemSignal(c->handleDoneSem);
            continue;
        }

        /* call handler */
        c->cbFunc(c->cookie,
                p->evType, p->devNum, p->queue,
                p->numOfBuff, p->packetBuffs, p->buffLen,
                &(p->rxParams.rawdata));
        /* callback done */
        cpssOsSigSemSignal(c->handleDoneSem);
    }
    /* exit */
    cpssOsSigSemSignal(c->handleDoneSem);
    return 0;
}

static GT_STATUS prvRxEvInitRemoteCbHandler(
  IN  RX_EV_HANDLER_CB_STC *cbHandler
)
{
    char name[64];

    cbHandler->cbType = RX_EV_HANDLER_CB_TYPE_REMOTE_E;
    cpssOsSprintf(name,"rxEvHS%d", prvRxEvSemSeq);
    cpssOsSigSemBinCreate(name, CPSS_OS_SEMB_EMPTY_E, &(cbHandler->handleSem));
    cpssOsSprintf(name,"rxEvHR%d", prvRxEvSemSeq);
    cpssOsSigSemBinCreate(name, CPSS_OS_SEMB_EMPTY_E, &(cbHandler->handleDoneSem));
    cpssOsSprintf(name, "rxEvCbTask%d", prvRxEvSemSeq);

    cpssOsTaskCreate(name, 10/*prio*/,0x10000/*stack*/,
            prvRxEventHandlerCbTaskFun, cbHandler, &(cbHandler->tid));

    prvRxEvSemSeq++;
    return GT_OK;
}

static GT_STATUS prvRxEvInitStopCbHandler(
  IN  RX_EV_HANDLER_CB_STC *cbHandler
)
{
    RX_EV_PACKET_RECEIVE_CB_FUN saveFunc = cbHandler->cbFunc;
    cbHandler->cbFunc = NULL;
    cpssOsSigSemSignal(cbHandler->handleSem);
    /* TODO: handle timeout */
    cpssOsSigSemWait(cbHandler->handleDoneSem, CPSS_OS_SEM_WAIT_FOREVER_CNS);
    cbHandler->cbFunc = saveFunc;

    cpssOsSigSemDelete(cbHandler->handleSem);
    cpssOsSigSemDelete(cbHandler->handleDoneSem);
    cbHandler->cbType = RX_EV_HANDLER_CB_TYPE_UNKNOWN_E;
    return GT_OK;
}
#endif

/*******************************************************************************
* prvRxEvAddCallBack
*
* DESCRIPTION:
*       Add callback entry to event handler
*
* INPUTS:
*       evHandler   - event handler ptr
*       cbHandler   - callback entry
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK on success
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS prvRxEvAddCallBack(
  IN  RX_EV_HANDLER_STC* evHandler,
  IN  RX_EV_HANDLER_CB_STC *cbHandler
)
{
    GT_U32 i;

    for (i = 0; i < CALLBACKS_MAX; i++)
    {
        if (evHandler->callBacks[i] == NULL)
            break;
        if (evHandler->callBacks[i] == cbHandler)
            return GT_ALREADY_EXIST;
    }
    if (i == CALLBACKS_MAX)
        return GT_FULL;

    evHandler->callBacks[i] = cbHandler;
    cbHandler->numRefs++;
#ifdef SHARED_MEMORY
    if ((cbHandler->cbType == RX_EV_HANDLER_CB_TYPE_REMOTE_E) && 
            (cbHandler->pid == evHandler->pid))
    {
        prvRxEvInitStopCbHandler(cbHandler);
        cbHandler->cbType = RX_EV_HANDLER_CB_TYPE_DIRECT_E;
    }
#endif
    if (cbHandler->cbType != RX_EV_HANDLER_CB_TYPE_UNKNOWN_E)
        return GT_OK;
    cbHandler->cbType = RX_EV_HANDLER_CB_TYPE_DIRECT_E;
#ifdef SHARED_MEMORY
    if (cbHandler->pid == evHandler->pid)
    {
        return GT_OK;
    }
    /*TODO: cbHandler->cbFunc == tgfTrafficTableRxPcktReceiveCb */

    /* cbType == RX_EV_HANDLER_CB_TYPE_REMOTE_E */
    prvRxEvInitRemoteCbHandler(cbHandler);

#endif
    return GT_OK;
}

/*******************************************************************************
* rxEventHandlerInitHandler
*
* DESCRIPTION:
*       Initialize event handler for Rx/RxErr events
*
* INPUTS:
*       devNum  - the device number or RX_EV_HANDLER_DEVNUM_ALL
*       queue   - the rx queue number or RX_EV_HANDLER_QUEUE_ALL
*       evType  - event type
*
* OUTPUTS:
*       evHandlePtr  - handler
*
* RETURNS:
*       GT_OK on success
*       GT_ALREADY_EXIST success, already initialized
*       GT_BAD_PARAM
*       GT_FAIL
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS rxEventHandlerInitHandler(
  IN  GT_U8                     devNum,
  IN  GT_U8                     queue,
  IN  RX_EV_HANDLER_EV_TYPE_ENT    evType,
  OUT GT_UINTPTR                *evHandlePtr
)
{
    RX_EV_HANDLER_STC* evHandler;
    RX_EV_HANDLER_CB_STC *cbHandler;

    if (evHandlePtr == NULL)
    {
        return GT_BAD_PARAM;
    }
    /* check if already exists */
    evHandler = prvRxEventHandlersList;
    while (evHandler != NULL)
    {
        if (    (evHandler->devNum == devNum) &&
                (evHandler->queue == queue) &&
                (evHandler->evType == evType))
        {
            break;
        }
    }
    if (evHandler != NULL)
    {
        *evHandlePtr = (GT_UINTPTR)evHandler;
        return GT_ALREADY_EXIST;
    }
    evHandler = (RX_EV_HANDLER_STC*)cpssOsMalloc(sizeof(*evHandler));
    if (evHandler == NULL)
    {
        return GT_FAIL;
    }
    cpssOsMemSet(evHandler, 0, sizeof(*evHandler));
    evHandler->devNum = devNum;
    evHandler->queue = queue;
    evHandler->evType = evType;
#ifdef SHARED_MEMORY
    evHandler->pid = getpid();
#endif
    evHandler->next = prvRxEventHandlersList;
    prvRxEventHandlersList = evHandler;

    /* add all matched callbacks */
    for (cbHandler = prvRxEventCallBacksList; cbHandler != NULL; cbHandler = cbHandler->next)
    {
        if (prvRxEvCbHandlerMatch(cbHandler, devNum, queue, evType) == GT_TRUE)
        {
            prvRxEvAddCallBack(evHandler, cbHandler);
        }
    }

    *evHandlePtr = (GT_UINTPTR)evHandler;
    return GT_OK;
}


/*******************************************************************************
* rxEventHandlerAddCallback
*
* DESCRIPTION:
*       Add callback function for Rx events
*
* INPUTS:
*       devNum  - the device number or RX_EV_HANDLER_DEVNUM_ALL
*       queue   - the rx queue number or RX_EV_HANDLER_QUEUE_ALL
*       type    - event type
*       cbFunc  - callback function
*       cookie  - cookie value to be passed to callback function
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
GT_STATUS rxEventHandlerAddCallback(
  IN  GT_U8                             devNum,
  IN  GT_U8                             queue,
  IN  RX_EV_HANDLER_EV_TYPE_ENT        evType,
  IN  RX_EV_PACKET_RECEIVE_CB_FUN   cbFunc,
  IN  GT_UINTPTR                        cookie
)
{
    RX_EV_HANDLER_STC*   evHandler;
    RX_EV_HANDLER_CB_STC *cbHandler;
    GT_STATUS                   rc = GT_OK;
#ifdef SHARED_MEMORY
    pid_t   pid;

    pid = getpid();
#endif

    /* find callback handler */
    for (cbHandler = prvRxEventCallBacksList; cbHandler != NULL; cbHandler = cbHandler->next)
    {
        if (cbHandler->cbFunc != cbFunc)
            continue;
        if (cbHandler->matchDevNum != devNum)
            continue;
        if (cbHandler->matchQueue != queue)
            continue;
        if (cbHandler->matchEvType != evType)
            continue;
        if (cbHandler->cookie != cookie)
            continue;
#ifdef SHARED_MEMORY
        /*TODO: if cbFunc == tgfTrafficTableRxPcktReceiveCb */
        if (pid != cbHandler->pid)
            continue;
#endif
        break;
    }
    if (cbHandler != NULL)
    {
        return GT_ALREADY_EXIST;
    }
    /* create new handler */
    cbHandler = (RX_EV_HANDLER_CB_STC*)cpssOsMalloc(sizeof(*cbHandler));
    if (cbHandler == NULL)
        return GT_FAIL;
    cpssOsMemSet(cbHandler, 0, sizeof(*cbHandler));
    cbHandler->matchDevNum = devNum;
    cbHandler->matchQueue = queue;
    cbHandler->matchEvType = evType;
    cbHandler->cbFunc = cbFunc;
    cbHandler->cookie = cookie;
#ifdef SHARED_MEMORY
    cbHandler->pid = pid;
#endif
    cbHandler->next = prvRxEventCallBacksList;
    prvRxEventCallBacksList = cbHandler;


    /* add to evHandlers */
    evHandler = prvRxEventHandlersList;
    while (evHandler != NULL)
    {
        if (prvRxEvHandlerMatch(evHandler, devNum, queue, evType) == GT_TRUE)
        {
            rc = prvRxEvAddCallBack(evHandler, cbHandler);
            if (rc != GT_OK)
                break;
        }
        evHandler = evHandler->next;
    }
#ifdef SHARED_MEMORY
    if (cbHandler->cbType == RX_EV_HANDLER_CB_TYPE_UNKNOWN_E)
    {
        rc = prvRxEvInitRemoteCbHandler(cbHandler);
    }
#endif

    return rc;
}

/*******************************************************************************
* rxEventHandlerRemoveCallback
*
* DESCRIPTION:
*       Add callback function for Rx events
*
* INPUTS:
*       devNum  - the device number or RX_EV_HANDLER_DEVNUM_ALL
*       queue   - the rx queue number or RX_EV_HANDLER_QUEUE_ALL
*       evType  - event type
*       cbFunc  - callback function
*       cookie  - cookie passed to rxEventHandlerAddCallback()
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
GT_STATUS rxEventHandlerRemoveCallback(
  IN  GT_U8                             devNum,
  IN  GT_U8                             queue,
  IN  RX_EV_HANDLER_EV_TYPE_ENT        evType,
  IN  RX_EV_PACKET_RECEIVE_CB_FUN   cbFunc,
  IN  GT_UINTPTR                        cookie
)
{
    RX_EV_HANDLER_CB_STC* cbHandler;
    RX_EV_HANDLER_STC* evHandler;
    GT_U32 i;
#ifdef SHARED_MEMORY
    pid_t   pid;

    pid = getpid();
#endif

    /* find callback handler */
    for (cbHandler = prvRxEventCallBacksList; cbHandler != NULL; cbHandler = cbHandler->next)
    {
        if (cbHandler->cbFunc != cbFunc)
            continue;
        if (cbHandler->cookie != cookie)
            continue;
        if (cbHandler->matchDevNum != devNum)
            continue;
        if (cbHandler->matchQueue != queue)
            continue;
        if (cbHandler->matchEvType != evType)
            continue;
#ifdef SHARED_MEMORY
        /* TODO: if cbFunc == tgfTrafficTableRxPcktReceiveCb */
        if (pid != cbHandler->pid)
            continue;
#endif
        break;
    }
    if (cbHandler == NULL)
    {
        return GT_NOT_FOUND;
    }

    /* remove from evHandlers */
    evHandler = prvRxEventHandlersList;
    while (evHandler != NULL)
    {
        for (i = 0; i < CALLBACKS_MAX; i++)
        {
            if (evHandler->callBacks[i] == cbHandler)
            {
                /* cleanup */
                if (cbHandler->numRefs > 0)
                    cbHandler->numRefs--;
                /* shift array */
                for(;i + 1 < CALLBACKS_MAX; i++)
                    /*TODO: memcpy??? */
                    evHandler->callBacks[i] = evHandler->callBacks[i+1];
                evHandler->callBacks[i] = NULL;
                break;
            }
        }
        evHandler = evHandler->next;
    }

    if (cbHandler->numRefs == 0)
    {
        /* remove from list */
        RX_EV_HANDLER_CB_STC *h = prvRxEventCallBacksList;
        RX_EV_HANDLER_CB_STC **prev = &prvRxEventCallBacksList;

#ifdef SHARED_MEMORY
        if (cbHandler->cbType == RX_EV_HANDLER_CB_TYPE_REMOTE_E)
        {
            prvRxEvInitStopCbHandler(cbHandler);
        }
#endif
        while (h != NULL)
        {
            if (h == cbHandler)
            {
                *prev = h->next;
                break;
            }
            prev = &h->next;
            h = h->next;
        }
        cpssOsFree(cbHandler);
    }
    return GT_OK;
}

/*******************************************************************************
* rxEventHandlerDoCallbacks
*
* DESCRIPTION:
*       Execute all callback routines for event handler
*
* INPUTS:
*       handle      - the handle of CPSS event handler
*       evType      - event type
*       devNum      - Device number.
*       queueIdx    - The queue from which this packet was received.
*       numOfBuff   - Num of used buffs in packetBuffs.
*       packetBuffs - The received packet buffers list.
*       buffLen     - List of buffer lengths for packetBuffs.
*       rxParamsPtr - (pointer to)information parameters of received packets
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
GT_STATUS rxEventHandlerDoCallbacks
(
    IN GT_UINTPTR                   handle,
    IN RX_EV_HANDLER_EV_TYPE_ENT    evType,
    IN GT_U8                        devNum,
    IN GT_U8                        queue,
    IN GT_U32                       numOfBuff,
    IN GT_U8                        *packetBuffs[],
    IN GT_U32                       buffLen[],
    IN GT_VOID                      *rxParamsPtr
)
{
    GT_U32 i;
    RX_EV_HANDLER_STC *evHandler = (RX_EV_HANDLER_STC*)handle;
#ifdef SHARED_MEMORY
    GT_BOOL paramsFilled = GT_FALSE;
#endif

    for (i = 0; i < CALLBACKS_MAX; i++)
    {
        RX_EV_HANDLER_CB_STC* cbHandler = evHandler->callBacks[i];
        if (cbHandler == NULL)
            break;
        /* first skip entries which are not match */
        if (cbHandler->matchEvType != evType && cbHandler->matchEvType != RX_EV_HANDLER_TYPE_ANY_E)
            continue;
        if (cbHandler->matchDevNum != devNum && cbHandler->matchDevNum != RX_EV_HANDLER_DEVNUM_ALL)
            continue;
        if (cbHandler->matchQueue != queue && cbHandler->matchQueue != RX_EV_HANDLER_QUEUE_ALL)
            continue;

        if (cbHandler->cbType == RX_EV_HANDLER_CB_TYPE_DIRECT_E)
        {
            cbHandler->cbFunc(cbHandler->cookie, evType,
                    devNum, queue, numOfBuff, packetBuffs, buffLen, rxParamsPtr);
        }
#ifdef SHARED_MEMORY
        if (cbHandler->cbType == RX_EV_HANDLER_CB_TYPE_REMOTE_E)
        {
            if (paramsFilled != GT_TRUE)
            {
                GT_U32 rxParamsSize = 0;
                /* copy data to shared memory */
                evHandler->params.devNum = devNum;
                evHandler->params.queue = queue;
                evHandler->params.evType = evType;
                if (numOfBuff > BUFF_LEN)
                {
                    /* TODO: handle err */
                    numOfBuff = BUFF_LEN;
                }
                evHandler->params.numOfBuff = numOfBuff;
                cpssOsMemCpy(evHandler->params.packetBuffs, packetBuffs,
                        sizeof(packetBuffs[0])*numOfBuff);
                cpssOsMemCpy(evHandler->params.buffLen, buffLen,
                        sizeof(buffLen[0])*numOfBuff);

#ifdef CHX_FAMILY
#  if defined(EXMXPM_FAMILY) || defined(EX_FAMILY)
                if (CPSS_IS_DXCH_FAMILY_MAC(PRV_CPSS_PP_MAC(devNum)->devFamily))
#  endif
                {
                    rxParamsSize = sizeof(CPSS_DXCH_NET_RX_PARAMS_STC);
                }
#endif /* CHX_FAMILY */
#ifdef EXMXPM_FAMILY
#  if defined(CHX_FAMILY) || defined(EX_FAMILY)
                if (CPSS_IS_EXMXPM_FAMILY_MAC(PRV_CPSS_PP_MAC(devNum)->devFamily))
#  endif
                {
                    rxParamsSize = sizeof(CPSS_EXMXPM_NET_RX_PARAMS_STC);
                }
#endif /* EXMXPM_FAMILY */
#ifdef EX_FAMILY
#  if defined(CHX_FAMILY) || defined(EXMXPM_FAMILY)
                if (CPSS_IS_EXMX_FAMILY_MAC(PRV_CPSS_PP_MAC(devNum)->devFamily))
#  endif
                {
                    rxParamsSize = sizeof(CPSS_EXMX_NET_RX_PARAMS_STC);
                }
#endif /* EX_FAMILY */
                if (rxParamsSize != 0)
                {
                    cpssOsMemCpy(&(evHandler->params.rxParams.rawdata),
                                rxParamsPtr, rxParamsSize);
                }
                paramsFilled = GT_TRUE;
            }
            cbHandler->paramsPtr = &(evHandler->params);

            /* signal to cb handler func */
            cpssOsSigSemSignal(cbHandler->handleSem);
            /* wait ack */
            /* TODO: handle timeout */
            cpssOsSigSemWait(cbHandler->handleDoneSem, CPSS_OS_SEM_WAIT_FOREVER_CNS);
        }
#endif
    }
    return GT_OK;
}





