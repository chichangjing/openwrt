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
* auEventHandler.c
*
* DESCRIPTION:
*       This module defines the API for CPSS FDB AU events
*
* DEPENDENCIES:
*
* FILE REVISION NUMBER:
*       $Revision: 2 $
*******************************************************************************/
#include <extUtils/auEventHandler/auEventHandler.h>
#include <cpss/extServices/private/prvCpssBindFunc.h>
/*TODO*/
/*#include <extUtils/trafficEngine/tgfTrafficTable.h>*/
#ifdef SHARED_MEMORY
#  include <sys/types.h>
#  include <unistd.h>
#endif /* SHARED_MEMORY */




#define BUFF_LEN    20

/*
 * typedef: enum AU_EV_HANDLER_CB_TYPE_ENT
 *
 * Description:
 *      Callback method
 *
 * Enumerations:
 *      AU_EV_HANDLER_CB_TYPE_UNKNOWN_E - not detected yet
 *      AU_EV_HANDLER_CB_TYPE_DIRECT_E  - direct callback, just execute
 *                                        callback function
 *      AU_EV_HANDLER_CB_TYPE_REMOTE_E  - callback function in external
 *                                        process, use remote call
 */
typedef enum
{
    AU_EV_HANDLER_CB_TYPE_UNKNOWN_E,
    AU_EV_HANDLER_CB_TYPE_DIRECT_E,
    AU_EV_HANDLER_CB_TYPE_REMOTE_E
} AU_EV_HANDLER_CB_TYPE_ENT;

#ifdef SHARED_MEMORY
#define BUFF_LEN 20
/*
 * typedef: AU_EV_HANDLER_CB_PARAMS_STC
 *
 * Description:
 *      Parameters for remote callback
 *
 * Fields:
 *      devNum      - Device number.
 *      evType      - event type
*       auMessage   - AUQ message
 *
 */
typedef struct AU_EV_HANDLER_CB_PARAMS_STCT {
    GT_U8                       devNum;
    AU_EV_HANDLER_EV_TYPE_ENT   evType;
    CPSS_MAC_UPDATE_MSG_EXT_STC auMessage;
} AU_EV_HANDLER_CB_PARAMS_STC;
#endif

/*
 * Typedef: AU_EV_HANDLER_CB_STC
 *
 * Description:
 *      Callback info structure
 *
 * Fields:
 *      next            - pointer to next item in chain
 *      matchDevNum     - applicable devNum or AU_EV_HANDLER_DEVNUM_ALL
 *      matchQueue      - applicable queue or AU_EV_HANDLER_QUEUE_ALL
 *      matchEvType     - event type, can be AU_EV_HANDLER_TYPE_ANY_E
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
typedef struct AU_EV_HANDLER_CB_STCT {
    struct AU_EV_HANDLER_CB_STCT *next;
    GT_U8                       matchDevNum;
    AU_EV_HANDLER_EV_TYPE_ENT   matchEvType;
    AU_EV_CALLBACK_FUNC         cbFunc;
    GT_UINTPTR                  cookie;
    GT_U32                      numRefs;
    AU_EV_HANDLER_CB_TYPE_ENT   cbType;
#ifdef SHARED_MEMORY
    pid_t                       pid;

    /* data for communication */
    CPSS_TASK                   tid;
    CPSS_OS_SIG_SEM             handleSem;
    CPSS_OS_SIG_SEM             handleDoneSem;
    AU_EV_HANDLER_CB_PARAMS_STC *paramsPtr;
#endif
} AU_EV_HANDLER_CB_STC;

#define CALLBACKS_MAX 10
/*
 * Typedef: AU_EV_HANDLER_STC
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
typedef struct AU_EV_HANDLER_STCT
{
    struct AU_EV_HANDLER_STCT   *next;
    GT_U8                       devNum;
    AU_EV_HANDLER_EV_TYPE_ENT   evType;
#ifdef SHARED_MEMORY
    pid_t                       pid;
    AU_EV_HANDLER_CB_PARAMS_STC params;
#endif
    AU_EV_HANDLER_CB_STC        *callBacks[CALLBACKS_MAX];

} AU_EV_HANDLER_STC;

static AU_EV_HANDLER_STC     *prvAuEventHandlersList = NULL;
static AU_EV_HANDLER_CB_STC  *prvAuEventCallBacksList = NULL;
#ifdef SHARED_MEMORY
static int prvAuEvSemSeq = 0;
#endif







/*******************************************************************************
* prvAuEvHandlerMatch
*
* DESCRIPTION:
*       Initialize library
*
* INPUTS:
*       evHandler - pointer to event handler
*       devNum  - the device number or AU_EV_HANDLER_DEVNUM_ALL
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
static GT_BOOL prvAuEvHandlerMatch(
  IN  AU_EV_HANDLER_STC        *evHandler,
  IN  GT_U8                     devNum,
  IN  AU_EV_HANDLER_EV_TYPE_ENT evType
)
{
    if (    (evHandler->devNum != devNum) && 
            (evHandler->devNum != AU_EV_HANDLER_DEVNUM_ALL) &&
            (devNum != AU_EV_HANDLER_DEVNUM_ALL))
    {
        return GT_FALSE;
    }
    if (    (evHandler->evType != evType) && 
            (evHandler->evType != AU_EV_HANDLER_TYPE_ANY_E) &&
            (evType != AU_EV_HANDLER_TYPE_ANY_E))
    {
        return GT_FALSE;
    }
    return GT_TRUE;
}

/*******************************************************************************
* prvAuEvCbHandlerMatch
*
* DESCRIPTION:
*       Initialize library
*
* INPUTS:
*       cbHandler - pointer to event handler
*       devNum  - the device number or AU_EV_HANDLER_DEVNUM_ALL
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
static GT_BOOL prvAuEvCbHandlerMatch(
  IN  AU_EV_HANDLER_CB_STC     *cbHandler,
  IN  GT_U8                     devNum,
  IN  AU_EV_HANDLER_EV_TYPE_ENT evType
)
{
    if (    (cbHandler->matchDevNum != devNum) && 
            (cbHandler->matchDevNum != AU_EV_HANDLER_DEVNUM_ALL) &&
            (devNum != AU_EV_HANDLER_DEVNUM_ALL))
    {
        return GT_FALSE;
    }
    if (    (cbHandler->matchEvType != evType) && 
            (cbHandler->matchEvType != AU_EV_HANDLER_TYPE_ANY_E) &&
            (evType != AU_EV_HANDLER_TYPE_ANY_E))
    {
        return GT_FALSE;
    }
    return GT_TRUE;
}


/*******************************************************************************
* auEventHandlerLibInit
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
GT_STATUS auEventHandlerLibInit(GT_VOID)
{
    /*TODO: init protection mutex */
    return GT_OK;
}

#ifdef SHARED_MEMORY
static unsigned __TASKCONV prvAuEventHandlerCbTaskFun(void *param)
{
    AU_EV_HANDLER_CB_STC *c = (AU_EV_HANDLER_CB_STC*)param;
    AU_EV_HANDLER_CB_PARAMS_STC *p;

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
        c->cbFunc(c->cookie, p->evType, p->devNum, &(p->auMessage));
        /* callback done */
        cpssOsSigSemSignal(c->handleDoneSem);
    }
    /* exit */
    cpssOsSigSemSignal(c->handleDoneSem);
    return 0;
}

static GT_STATUS prvAuEvInitRemoteCbHandler(
  IN  AU_EV_HANDLER_CB_STC *cbHandler
)
{
    char name[64];

    cbHandler->cbType = AU_EV_HANDLER_CB_TYPE_REMOTE_E;
    cpssOsSprintf(name,"auEvHS%d", prvAuEvSemSeq);
    cpssOsSigSemBinCreate(name, CPSS_OS_SEMB_EMPTY_E, &(cbHandler->handleSem));
    cpssOsSprintf(name,"auEvHR%d", prvAuEvSemSeq);
    cpssOsSigSemBinCreate(name, CPSS_OS_SEMB_EMPTY_E, &(cbHandler->handleDoneSem));
    cpssOsSprintf(name, "auEvCbTask%d", prvAuEvSemSeq);

    cpssOsTaskCreate(name, 10/*prio*/,0x10000/*stack*/,
            prvAuEventHandlerCbTaskFun, cbHandler, &(cbHandler->tid));

    prvAuEvSemSeq++;
    return GT_OK;
}

static GT_STATUS prvAuEvInitStopCbHandler(
  IN  AU_EV_HANDLER_CB_STC *cbHandler
)
{
    AU_EV_CALLBACK_FUNC saveFunc = cbHandler->cbFunc;
    cbHandler->cbFunc = NULL;
    cpssOsSigSemSignal(cbHandler->handleSem);
    /* TODO: handle timeout */
    cpssOsSigSemWait(cbHandler->handleDoneSem, CPSS_OS_SEM_WAIT_FOREVER_CNS);
    cbHandler->cbFunc = saveFunc;

    cpssOsSigSemDelete(cbHandler->handleSem);
    cpssOsSigSemDelete(cbHandler->handleDoneSem);
    cbHandler->cbType = AU_EV_HANDLER_CB_TYPE_UNKNOWN_E;
    return GT_OK;
}
#endif

/*******************************************************************************
* prvAuEvAddCallBack
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
static GT_STATUS prvAuEvAddCallBack(
  IN  AU_EV_HANDLER_STC* evHandler,
  IN  AU_EV_HANDLER_CB_STC *cbHandler
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
    if ((cbHandler->cbType == AU_EV_HANDLER_CB_TYPE_REMOTE_E) && 
            (cbHandler->pid == evHandler->pid))
    {
        prvAuEvInitStopCbHandler(cbHandler);
        cbHandler->cbType = AU_EV_HANDLER_CB_TYPE_DIRECT_E;
    }
#endif
    if (cbHandler->cbType != AU_EV_HANDLER_CB_TYPE_UNKNOWN_E)
        return GT_OK;
    cbHandler->cbType = AU_EV_HANDLER_CB_TYPE_DIRECT_E;
#ifdef SHARED_MEMORY
    if (cbHandler->pid == evHandler->pid)
    {
        return GT_OK;
    }
    /*TODO: cbHandler->cbFunc == tgfTrafficTableRxPcktReceiveCb */

    /* cbType == AU_EV_HANDLER_CB_TYPE_REMOTE_E */
    prvAuEvInitRemoteCbHandler(cbHandler);

#endif
    return GT_OK;
}

/*******************************************************************************
* auEventHandlerInitHandler
*
* DESCRIPTION:
*       Initialize event handler for Rx/RxErr events
*       auEventHandlerDoCallbacks() must be called from the same process
*       (another thread of this process allowed)
*
* INPUTS:
*       devNum  - the device number or AU_EV_HANDLER_DEVNUM_ALL
*       type    - event type
*
* OUTPUTS:
*       evHandlePtr  - handle
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
GT_STATUS auEventHandlerInitHandler(
  IN  GT_U8                     devNum,
  IN  AU_EV_HANDLER_EV_TYPE_ENT evType,
  OUT GT_UINTPTR                *evHandlePtr
)
{
    AU_EV_HANDLER_STC* evHandler;
    AU_EV_HANDLER_CB_STC *cbHandler;

    if (evHandlePtr == NULL)
    {
        return GT_BAD_PARAM;
    }
    /* check if already exists */
    evHandler = prvAuEventHandlersList;
    while (evHandler != NULL)
    {
        if (    (evHandler->devNum == devNum) &&
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
    evHandler = (AU_EV_HANDLER_STC*)cpssOsMalloc(sizeof(*evHandler));
    if (evHandler == NULL)
    {
        return GT_FAIL;
    }
    cpssOsMemSet(evHandler, 0, sizeof(*evHandler));
    evHandler->devNum = devNum;
    evHandler->evType = evType;
#ifdef SHARED_MEMORY
    evHandler->pid = getpid();
#endif
    evHandler->next = prvAuEventHandlersList;
    prvAuEventHandlersList = evHandler;

    /* add all matched callbacks */
    for (cbHandler = prvAuEventCallBacksList; cbHandler != NULL; cbHandler = cbHandler->next)
    {
        if (prvAuEvCbHandlerMatch(cbHandler, devNum, evType) == GT_TRUE)
        {
            prvAuEvAddCallBack(evHandler, cbHandler);
        }
    }

    *evHandlePtr = (GT_UINTPTR)evHandler;
    return GT_OK;
}


/*******************************************************************************
* auEventHandlerAddCallback
*
* DESCRIPTION:
*       Add callback function for Rx events
*
* INPUTS:
*       devNum  - the device number or AU_EV_HANDLER_DEVNUM_ALL
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
GT_STATUS auEventHandlerAddCallback(
  IN  GT_U8                         devNum,
  IN  AU_EV_HANDLER_EV_TYPE_ENT     evType,
  IN  AU_EV_CALLBACK_FUNC           cbFunc,
  IN  GT_UINTPTR                    cookie
)
{
    AU_EV_HANDLER_STC*   evHandler;
    AU_EV_HANDLER_CB_STC *cbHandler;
    GT_STATUS                   rc = GT_OK;
#ifdef SHARED_MEMORY
    pid_t   pid;

    pid = getpid();
#endif

    /* find callback handler */
    for (cbHandler = prvAuEventCallBacksList; cbHandler != NULL; cbHandler = cbHandler->next)
    {
        if (cbHandler->cbFunc != cbFunc)
            continue;
        if (cbHandler->matchDevNum != devNum)
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
    cbHandler = (AU_EV_HANDLER_CB_STC*)cpssOsMalloc(sizeof(*cbHandler));
    if (cbHandler == NULL)
        return GT_FAIL;
    cpssOsMemSet(cbHandler, 0, sizeof(*cbHandler));
    cbHandler->matchDevNum = devNum;
    cbHandler->matchEvType = evType;
    cbHandler->cbFunc = cbFunc;
    cbHandler->cookie = cookie;
#ifdef SHARED_MEMORY
    cbHandler->pid = pid;
#endif
    cbHandler->next = prvAuEventCallBacksList;
    prvAuEventCallBacksList = cbHandler;


    /* add to evHandlers */
    evHandler = prvAuEventHandlersList;
    while (evHandler != NULL)
    {
        if (prvAuEvHandlerMatch(evHandler, devNum, evType) == GT_TRUE)
        {
            rc = prvAuEvAddCallBack(evHandler, cbHandler);
            if (rc != GT_OK)
                break;
        }
        evHandler = evHandler->next;
    }
#ifdef SHARED_MEMORY
    if (cbHandler->cbType == AU_EV_HANDLER_CB_TYPE_UNKNOWN_E)
    {
        rc = prvAuEvInitRemoteCbHandler(cbHandler);
    }
#endif

    return rc;
}

/*******************************************************************************
* auEventHandlerRemoveCallback
*
* DESCRIPTION:
*       Add callback function for Rx events
*
* INPUTS:
*       devNum  - the device number or AU_EV_HANDLER_DEVNUM_ALL
*       evType  - event type
*       cbFunc  - callback function
*       cookie  - cookie passed to auEventHandlerAddCallback()
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
GT_STATUS auEventHandlerRemoveCallback(
  IN  GT_U8                         devNum,
  IN  AU_EV_HANDLER_EV_TYPE_ENT     evType,
  IN  AU_EV_CALLBACK_FUNC           cbFunc,
  IN  GT_UINTPTR                    cookie
)
{
    AU_EV_HANDLER_CB_STC* cbHandler;
    AU_EV_HANDLER_STC* evHandler;
    GT_U32 i;
#ifdef SHARED_MEMORY
    pid_t   pid;

    pid = getpid();
#endif

    /* find callback handler */
    for (cbHandler = prvAuEventCallBacksList; cbHandler != NULL; cbHandler = cbHandler->next)
    {
        if (cbHandler->cbFunc != cbFunc)
            continue;
        if (cbHandler->cookie != cookie)
            continue;
        if (cbHandler->matchDevNum != devNum)
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
    evHandler = prvAuEventHandlersList;
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
        AU_EV_HANDLER_CB_STC *h = prvAuEventCallBacksList;
        AU_EV_HANDLER_CB_STC **prev = &prvAuEventCallBacksList;

#ifdef SHARED_MEMORY
        if (cbHandler->cbType == AU_EV_HANDLER_CB_TYPE_REMOTE_E)
        {
            prvAuEvInitStopCbHandler(cbHandler);
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
* auEventHandlerDoCallbacks
*
* DESCRIPTION:
*       Execute all callback routines for event handler
*
* INPUTS:
*       handle       - the handle of CPSS event handler
*       evType       - event type
*       devNum       - Device number.
*       auMessagePtr - pointer to the AUQ message
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
GT_STATUS auEventHandlerDoCallbacks
(
    IN GT_UINTPTR                   handle,
    IN AU_EV_HANDLER_EV_TYPE_ENT    evType,
    IN GT_U8                        devNum,
    IN CPSS_MAC_UPDATE_MSG_EXT_STC *auMessagePtr
)
{
    GT_U32 i;
    AU_EV_HANDLER_STC *evHandler = (AU_EV_HANDLER_STC*)handle;
#ifdef SHARED_MEMORY
    GT_BOOL paramsFilled = GT_FALSE;
#endif

    for (i = 0; i < CALLBACKS_MAX; i++)
    {
        AU_EV_HANDLER_CB_STC* cbHandler = evHandler->callBacks[i];
        if (cbHandler == NULL)
            break;
        /* first skip entries which are not match */
        if (cbHandler->matchEvType != evType && cbHandler->matchEvType != AU_EV_HANDLER_TYPE_ANY_E)
            continue;
        if (cbHandler->matchDevNum != devNum && cbHandler->matchDevNum != AU_EV_HANDLER_DEVNUM_ALL)
            continue;

        if (cbHandler->cbType == AU_EV_HANDLER_CB_TYPE_DIRECT_E)
        {
            cbHandler->cbFunc(cbHandler->cookie, evType, devNum, auMessagePtr);
        }
#ifdef SHARED_MEMORY
        if (cbHandler->cbType == AU_EV_HANDLER_CB_TYPE_REMOTE_E)
        {
            if (paramsFilled != GT_TRUE)
            {
                /* copy data to shared memory */
                evHandler->params.devNum = devNum;
                evHandler->params.evType = evType;
                cpssOsMemCpy(&(evHandler->params.auMessage), auMessagePtr,
                        sizeof(evHandler->params.auMessage));
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





