/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* cpssGenEventRequests.c
*
* DESCRIPTION:
*       Includes unified event routine. The routines allow a user application
*       to bind, select and receive events from PP, XBAR and FA devices.
*
* FILE REVISION NUMBER:
*       $Revision: 15 $
*
*******************************************************************************/

#include <cpss/extServices/private/prvCpssBindFunc.h>
#include <cpssCommon/cpssPresteraDefs.h>

#include <cpss/generic/events/cpssGenEventUnifyTypes.h>

#include <cpss/driver/interrupts/cpssDrvComIntEvReqQueues.h>
#include <cpss/generic/events/private/prvCpssGenEvReq.h>
#include <cpss/generic/events/cpssGenEventCtrl.h>
#include <cpss/driver/interrupts/cpssDrvComIntSvcRtn.h>


static GT_STATUS prvCpssGenEventGppIsrConnect(void);

static PRV_CPSS_EVENT_MASK_DEVICE_SET_FUNC eventMaskDeviceFuncArray[PRV_CPSS_DEVICE_TYPE_LAST_E] =
                                                                        {NULL , NULL , NULL, NULL};

static PRV_CPSS_EVENT_GENERATE_DEVICE_SET_FUNC eventGenerateDeviceFuncArray[PRV_CPSS_DEVICE_TYPE_LAST_E] =
                                                                        {NULL , NULL , NULL, NULL};

static PRV_CPSS_EVENT_GPP_ISR_CONNECT_FUNC eventGppIsrConnectFunc =
                                (PRV_CPSS_EVENT_GPP_ISR_CONNECT_FUNC)prvCpssGenEventGppIsrConnect;

/*******************************************************************************
* Internal definitions
*******************************************************************************/

/*#define EV_REQ_DEBUG*/

#ifdef EV_REQ_DEBUG
    #define DBG_INFO(x)     cpssOsPrintf x

    static char * uniEvName[CPSS_UNI_EVENT_COUNT_E] = {UNI_EV_NAME};
    #ifdef ASIC_SIMULATION
        #define DBG_LOG(x)  cpssOsPrintf x
    #else
        extern int logMsg(char *, int, int, int, int, int, int);
        #define DBG_LOG(x)  logMsg x
    #endif
#else
    #define DBG_INFO(x)
    #define DBG_LOG(x)
#endif

static GT_STATUS uniEvHndlSet(GT_U32 uniEvent, PRV_CPSS_DRV_EVENT_HNDL_STC *evHndlPtr);

/*******************************************************************************
* Internal usage variables
*******************************************************************************/

/*******************************************************************************
* prvCpssEventBind
*
* DESCRIPTION:
*       MODE PRV_CPSS_DRV_HANDLE_BIND_USE_SIGNAL_TYPE_E :
*       This routine binds a user process to unified event. The routine returns
*       a handle that is used when the application wants to wait for the event
*       (cpssEventSelect), receive the event(cpssEventRecv) or transmit a packet
*       using the Network Interface.
*
*       MODE PRV_CPSS_DRV_HANDLE_BIND_USE_APPL_CB_TYPE_E :
*       This routine binds a user process to unified event. The routine returns
*       a handle that is used when the application wants to check if there are
*       waiting events (cpssEventWaitingEventsGet), clear the already served
*       events (cpssEventTreatedEventsClear) , receive the event(cpssEventRecv)
*
*       This is alternative way of treating events as opposed of the other
*       Select mechanism used with function cpssEventBind(...) (where
*       Application had to wait on binary semaphore, which was released by the
*       relevant interrupt).
*
*       In the new scheme, the application provides an ISR function which would
*       be called whenever the relevant interrupt(s) have been occurred.
*       Thus, allowing the application to use any synchronism mechanism which
*       suits its needs.
*       The application ISR function would be called at Interrupt context !!!
*       NOTE : the function does not mask/unmask the HW events in any device.
*              This is Application responsibility to unmask the relevant events
*              on the needed devices , using function cpssEventDeviceMaskSet or
*              cpssEventDeviceMaskWithEvExtDataSet
* APPLICABLE DEVICES:  All devices (PP / FA / XBAR)
*
* INPUTS:
*       handleBindType - handle bind type.
*
*       uniEventArr - The unified event list.
*       arrLength   - The unified event list length.
*       isrCbPtr  - (pointer for) application ISR function to be called when
*                     events are received
*                     relevant only when PRV_CPSS_DRV_HANDLE_BIND_USE_APPL_CB_TYPE_E
*       cookie  - storing application data (cookie)
*                     relevant only when PRV_CPSS_DRV_HANDLE_BIND_USE_APPL_CB_TYPE_E
*
* OUTPUTS:
*       hndlPtr     - The user handle for the bounded events.
*
* RETURNS:
*       GT_OK  - success
*       GT_FAIL - general failure
*       GT_BAD_PTR  - one of the parameters is NULL pointer
*       GT_OUT_OF_CPU_MEM - failed to allocate CPU memory,
*       GT_FULL - when trying to set the "tx buffer queue unify event"
*                 (CPSS_PP_TX_BUFFER_QUEUE_E) with other events in the same
*                 handler
*       GT_ALREADY_EXIST - one of the unified events already bound to another
*                 handler (not applicable to CPSS_PP_TX_BUFFER_QUEUE_E)
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS prvCpssEventBind
(
    IN  PRV_CPSS_DRV_HANDLE_BIND_TYPE_ENT   handleBindType,
    IN  CPSS_UNI_EV_CAUSE_ENT               uniEventArr[],
    IN  GT_U32                              arrLength,
    IN  CPSS_EVENT_ISR_CB_FUNC              isrCbPtr,
    IN  GT_VOID*                            cookie,
    OUT GT_UINTPTR                          *hndlPtr
)
{
    PRV_CPSS_DRV_EVENT_HNDL_STC *evHndlPtr; /* The event handle pointer     */
    CPSS_OS_SIG_SEM         semId=0;        /* The signalling semaphore Id  */
    char                    semName[30];    /* The semaphore name           */
    static GT_U32           semCnt = 0;     /* The semaphore counter        */
    GT_STATUS               rc=GT_OK;       /* The return code              */
    GT_U32                  i;              /* Iterator                     */

    /* validate input */
    CPSS_NULL_PTR_CHECK_MAC(hndlPtr);
    if(arrLength)
    {
        CPSS_NULL_PTR_CHECK_MAC(uniEventArr);
    }

    /* allocate a user handle */
    evHndlPtr = cpssOsMalloc(sizeof(PRV_CPSS_DRV_EVENT_HNDL_STC));

    if (NULL == evHndlPtr)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_OUT_OF_CPU_MEM, LOG_ERROR_NO_MSG);
    }

    cpssOsBzero((char *)evHndlPtr, sizeof(PRV_CPSS_DRV_EVENT_HNDL_STC));

    evHndlPtr->hndlBindType = handleBindType;

    if(handleBindType == PRV_CPSS_DRV_HANDLE_BIND_USE_SIGNAL_TYPE_E)
    {
        cpssOsSprintf(semName, "EvBind_%d", semCnt++);

        /* create semaphore for signalling the user process a new event arrived */
        rc = cpssOsSigSemBinCreate(semName, CPSS_OS_SEMB_EMPTY_E, &semId);
        if(rc != GT_OK)
        {
            goto exit_cleanly_lbl;
        }

        /* save the semaphore Id in the user handle */
        PRV_CPSS_DRV_HANDEL_SEM_MAC(evHndlPtr) = (GT_UINTPTR)semId;
        PRV_CPSS_DRV_HANDEL_SEM_SIGNAL_MAC(evHndlPtr) = GT_FALSE;
    }
    else if(handleBindType == PRV_CPSS_DRV_HANDLE_BIND_USE_APPL_CB_TYPE_E)
    {
        /* check that application gave non-NULL pointer */
        CPSS_NULL_PTR_CHECK_MAC(isrCbPtr);

        /* save Application's CB function and cookie */
        PRV_CPSS_DRV_HANDEL_APPL_CB_MAC(evHndlPtr) = isrCbPtr;
        PRV_CPSS_DRV_HANDEL_APPL_COOKIE_MAC(evHndlPtr) = cookie;
    }
    else
    {
        /* should not happen */
        rc = GT_BAD_PARAM;
        goto exit_cleanly_lbl;
    }


    evHndlPtr->evType = PRV_CPSS_DRV_REGULAR_EVENT_E;

    /* set the new handle in the unified event queue and unmask the interrupt */
    for (i = 0; i < arrLength; i++)
    {
        if (uniEventArr[i] == CPSS_PP_TX_BUFFER_QUEUE_E)
        {
            /* Tx buffer queue event type is unique, more than one process can bind  */
            /* the event and the notifying of the event arrival is per      */
            /* process. Allocate a TxBufferQueue FIFO for the ISR usage.    */
            if (i != 0)
            {
                /* TxBufferQueue event MUST be bound alone, without any other events */
                rc = GT_FULL;
                goto exit_cleanly_lbl;
            }

            evHndlPtr->evType     = PRV_CPSS_DRV_TX_BUFF_QUEUE_EVENT_E;
            evHndlPtr->extDataPtr = cpssOsMalloc(sizeof(PRV_CPSS_TX_BUF_QUEUE_FIFO_STC));

            if (NULL == evHndlPtr->extDataPtr)
            {
                rc = GT_OUT_OF_CPU_MEM;
                goto exit_cleanly_lbl;
            }

            ((PRV_CPSS_TX_BUF_QUEUE_FIFO_STC *)evHndlPtr->extDataPtr)->headPtr = NULL;
            ((PRV_CPSS_TX_BUF_QUEUE_FIFO_STC *)evHndlPtr->extDataPtr)->tailPtr = NULL;
        }
        else
        {
            if (PRV_CPSS_DRV_TX_BUFF_QUEUE_EVENT_E == evHndlPtr->evType)
            {
                /* TxBufferQueue event MUST be bound alone, without any other events */
                rc = GT_FULL;
                goto exit_cleanly_lbl;
            }
        }

        /* save the user handle in unified queue control block */
        rc = uniEvHndlSet(uniEventArr[i], evHndlPtr);
        if(rc != GT_OK)
        {
            goto exit_cleanly_lbl;
        }
    }

exit_cleanly_lbl:

    if (rc != GT_OK)
    {
        if(evHndlPtr->extDataPtr)
        {
            cpssOsFree(evHndlPtr->extDataPtr);
        }
        cpssOsFree(evHndlPtr);

        *hndlPtr = 0;
        if(semId)
        {
            cpssOsSigSemDelete(semId);
        }

        return rc;
    }

    /* and update the user handle */
    *hndlPtr = (GT_UINTPTR)evHndlPtr;

    return GT_OK;
}


/*******************************************************************************
* cpssEventBind
*
* DESCRIPTION:
*       This routine binds a user process to unified event. The routine returns
*       a handle that is used when the application wants to wait for the event
*       (cpssEventSelect), receive the event(cpssEventRecv) or transmit a packet
*       using the Network Interface.
*
*       NOTE : the function does not mask/unmask the HW events in any device.
*              This is Application responsibility to unmask the relevant events
*              on the needed devices , using function cpssEventDeviceMaskSet or
*              cpssEventDeviceMaskWithEvExtDataSet
* APPLICABLE DEVICES:  All devices (PP / FA / XBAR)
*
* INPUTS:
*       uniEventArr - The unified event list.
*       arrLength   - The unified event list length.
*
* OUTPUTS:
*       hndlPtr     - The user handle for the bounded events.
*
* RETURNS:
*       GT_OK  - success
*       GT_FAIL - general failure
*       GT_BAD_PTR  - one of the parameters is NULL pointer
*       GT_OUT_OF_CPU_MEM - failed to allocate CPU memory,
*       GT_FULL - when trying to set the "tx buffer queue unify event"
*                 (CPSS_PP_TX_BUFFER_QUEUE_E) with other events in the same
*                 handler
*       GT_ALREADY_EXIST - one of the unified events already bound to another
*                 handler (not applicable to CPSS_PP_TX_BUFFER_QUEUE_E)
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssEventBind
(
    IN  CPSS_UNI_EV_CAUSE_ENT      uniEventArr[],
    IN  GT_U32               arrLength,
    OUT GT_UINTPTR           *hndlPtr
)
{
    return prvCpssEventBind(PRV_CPSS_DRV_HANDLE_BIND_USE_SIGNAL_TYPE_E,
                            uniEventArr,arrLength,
                            NULL,0,/* 2 don't care parameters */
                            hndlPtr);
}

/*******************************************************************************
* cpssEventDestroy
*
* DESCRIPTION:
*       This routine destroy the handle , and this handle is not valid any more.
*       The handle could have been create either by cpssEventIsrBind or cpssEventBind
*       API implementation sequence:
*       1. Signal any locked semaphores (so application's task can continue)
*       2. Release dynamic allocations relate to this event.
*
*       NOTE : the function does not mask/unmask the HW events in any device.
*              This is Application responsibility to unmask the relevant events
*              on the needed devices , using function cpssEventDeviceMaskSet or
*              cpssEventDeviceMaskWithEvExtDataSet
*
* APPLICABLE DEVICES:  All devices (PP / FA / XBAR)
*
* INPUTS:
*       hndl                - The user handle for the bounded events.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK  - success
*       GT_FAIL - general failure
*       GT_BAD_PARAM - bad hndl parameter , the hndl parameter is not legal
*                     (was not returned by cpssEventBind(...)/cpssEventIsrBind(...))
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssEventDestroy
(
    IN  GT_UINTPTR          hndl
)
{
    PRV_CPSS_DRV_EVENT_HNDL_STC *evHndlPtr; /* The event handle pointer     */

    /* validate the handle */
    if (0 == hndl)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    evHndlPtr = (PRV_CPSS_DRV_EVENT_HNDL_STC*)hndl;

    if(evHndlPtr->hndlBindType != PRV_CPSS_DRV_HANDLE_BIND_USE_SIGNAL_TYPE_E &&
       evHndlPtr->hndlBindType != PRV_CPSS_DRV_HANDLE_BIND_USE_APPL_CB_TYPE_E)
    {
        /* the hndl parameter is not legal --> was not created by
           cpssEventBind(...) / cpssEventIsrBind(...) */
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }
    /*check if semaphore was created*/
    if(PRV_CPSS_DRV_HANDEL_SEM_MAC(evHndlPtr))
    {
#if !defined(LINUX_SIM) /* cause linux sim to hang here (after task that wait for this semaphore was killed) */
        cpssOsSigSemSignal((CPSS_OS_SIG_SEM)PRV_CPSS_DRV_HANDEL_SEM_MAC(evHndlPtr));
        /* allow application to finish 'last moment' issues with the handler */
        cpssOsTimerWkAfter(10);
#endif
        cpssOsSigSemDelete((CPSS_OS_SIG_SEM)PRV_CPSS_DRV_HANDEL_SEM_MAC(evHndlPtr));
    }
    FREE_PTR_MAC(evHndlPtr->extDataPtr);
    FREE_PTR_MAC(evHndlPtr);

    return GT_OK;
}


/*******************************************************************************
* cpssEventSelect
*
* DESCRIPTION:
*       This function waiting for one of the events ,relate to the handler,
*       to happen , and gets a list of events (in array of bitmaps format) that
*       occurred .
*
* APPLICABLE DEVICES:  All devices (PP / FA / XBAR)
*
* INPUTS:
*       hndl                - The user handle for the bounded events.
*       timeoutPtr          - (pointer to) Wait timeout in milliseconds
*                             NULL pointer means wait forever.
*       evBitmapArrLength   - The bitmap array length (in words).
*
* OUTPUTS:
*       evBitmapArr         - The bitmap array.
*
* RETURNS:
*       GT_OK  - success
*       GT_FAIL - general failure
*       GT_BAD_PARAM - bad hndl parameter , the hndl parameter is not legal
*                     (was not returned by cpssEventBind(...))
*       GT_BAD_PTR  - evBitmapArr parameter is NULL pointer
*                     (and evBitmapArrLength != 0)
*       GT_EMPTY - there are no events to retrieve (relevant when timeoutPtr != NULL).
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssEventSelect
(
    IN  GT_UINTPTR          hndl,
    IN  GT_U32              *timeoutPtr,
    OUT GT_U32              evBitmapArr[],
    IN  GT_U32              evBitmapArrLength
)
{
    PRV_CPSS_DRV_EVENT_HNDL_STC *evHndlPtr; /* The event handle pointer     */
    GT_STATUS               rc;             /* Return Code                  */

    /* validate the handle */
    if (0 == hndl)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    /* validate bitmap parameters */
    if(evBitmapArrLength != 0)
    {
        CPSS_NULL_PTR_CHECK_MAC(evBitmapArr);
    }

    evHndlPtr = (PRV_CPSS_DRV_EVENT_HNDL_STC*)hndl;

    if(evHndlPtr->hndlBindType != PRV_CPSS_DRV_HANDLE_BIND_USE_SIGNAL_TYPE_E)
    {
        /* the hndl parameter is not legal --> was not created by
           cpssEventBind(...) */
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    /* Remove all events that were received by user and did not */
    /* occure since last select.                                */
    prvCpssDrvEvReqQRcvedEvRemoveAll(hndl);

    /* get the event bitmap that occurred */
    if (prvCpssDrvEvReqQBitmapGet(hndl, evBitmapArr, evBitmapArrLength) > 0)
    {
        /* clear the semaphore if signalled */
        cpssOsSigSemWait((CPSS_OS_SIG_SEM)PRV_CPSS_DRV_HANDEL_SEM_MAC(evHndlPtr), CPSS_OS_SEM_NO_WAIT_CNS);

        /* To prevent loss of signal for events which might occur after waiting */
        /* for the semaphore ended until the next waiting period, the "semSignal" */
        /* must be reset here. This prohibit the situation that events are waiting */
        /* in the queue but no signaling was done. The opposite scenario can occur, */
        /* not all events cause signaling, but due to the order of proccessing at */
        /* least one of the waiting events will cause signaling, and as a result */
        /* all other waiting events will be dealt.*/
        PRV_CPSS_DRV_HANDEL_SEM_SIGNAL_MAC(evHndlPtr) = GT_FALSE;

        return GT_OK;
    }

waitForSignal_lbl:
    /* wait on semaphore for event to happen */
    if (NULL == timeoutPtr)
    {
        /* wait forever */
        rc = cpssOsSigSemWait((CPSS_OS_SIG_SEM)PRV_CPSS_DRV_HANDEL_SEM_MAC(evHndlPtr),
                            CPSS_OS_SEM_WAIT_FOREVER_CNS);
        if(rc != GT_OK)
        {
            return rc;
        }
    }
    else
    {
        if (*timeoutPtr > 0)
        {
            /* wait for the defined timeout */
            rc = cpssOsSigSemWait((CPSS_OS_SIG_SEM)PRV_CPSS_DRV_HANDEL_SEM_MAC(evHndlPtr), *timeoutPtr);

            if(rc != GT_OK)
            {
                return rc;
            }
        }
        else
        {
            /* no wait for timeout, clear the semaphore */
            cpssOsSigSemWait((CPSS_OS_SIG_SEM)PRV_CPSS_DRV_HANDEL_SEM_MAC(evHndlPtr), CPSS_OS_SEM_NO_WAIT_CNS);
        }
    }

    /* To prevent loss of signal for events which might occur after waiting */
    /* for the semaphore ended untill the next waiting period, the "semSignal" */
    /* must be reset here. This prohibit the situation that events are waiting */
    /* in the queue but no signaling was done. The opposite scenario can occur, */
    /* not all events cause signaling, but due to the order of proccessing at */
    /* least one of the waiting events will cause signaling, and as a result */
    /* all other waiting events will be dealt.*/
    PRV_CPSS_DRV_HANDEL_SEM_SIGNAL_MAC(evHndlPtr) = GT_FALSE;

    /* we got new events, get the new bitmap */
    if (prvCpssDrvEvReqQBitmapGet(hndl, evBitmapArr, evBitmapArrLength) == 0)
    {
        if(timeoutPtr == NULL)
        {
            /*
                in that case we got indication that there are no events to handle.
                even that the handle was signaled.

                explanation : how did it happened ?

                assume that current handle bound event that represent several
                interrupt bits in HW --> like 'link change' that each port has
                it's own interrupt bit
                assume:
                1. link change on one port --> the ISR will signal the waiting task.
                2. the waiting task will start to work (after ISR finish)
                    a. will start handle the event
                -- BUT during that the link change on other port --> the ISR will signal the waiting task.
                    b. will handle also the second event (before waiting to the semaphore)
                3. finally when coreEventSelect will be called after the events
                   where handle , the function will wait for the semaphore
                -- BUT the semaphore is 'ON' it was signaled already !!!
                --> meaning that the coreEventSelect will continue to run when
                there are no actual event waiting !!!


                that is the reason that we return to wait for the signal.


                NOTE:
                also in the handle of 'tx buffer queue' ('tx ended') may get to
                similar situation (even the handle hold only single event type)

                explanation : how did it happened -- for 'tx buffer queue' ?

                assume that the task handle the 'tx buffer queue'
                1. the event occur due to first TX packet from CPU --> ISR signal the waiting task.
                2. the waiting task will start to work (after ISR finish)
                    a. will start handle the event
                -- BUT during that the same event happen again (due to other TX from cpu)
                    --> the ISR will signal the waiting task.
                    b. will handle also the second event (before waiting to the semaphore)
                3. finally when coreEventSelect will be called after the events
                   where handle , the function will wait for the semaphore
                -- BUT the semaphore is 'ON' it was signaled already !!!
                --> meaning that the coreEventSelect will continue to run when
                there are no actual event waiting !!!

            */

            goto waitForSignal_lbl;
        }
        else
        {
            /* there are no current events to handle */
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_EMPTY, LOG_ERROR_NO_MSG);
        }
    }

    return GT_OK;
}


/*******************************************************************************
* cpssEventRecv
*
* DESCRIPTION:
*       This function gets general information about the selected unified event.
*       The function retrieve information about the device number that relate to
*       the event , and extra info about port number / priority queue that
*       relate to the event.
*       The function retrieve the info about the first occurrence of this event
*       in the queue.
*
* APPLICABLE DEVICES:  All devices (PP / FA / XBAR)
*
* INPUTS:
*       hndl        - The user handle for the bounded events.
*       evCause     - The specified unify event that info about it's first
*                     waiting occurrence required.
* OUTPUTS:
*       evExtDataPtr- (pointer to)The additional date (port num / priority
*                     queue number) the event was received upon.
*       evDevPtr    - (pointer to)The device the event was received upon
*
* RETURNS:
*       GT_OK  - success
*       GT_FAIL - general failure
*       GT_NO_MORE   - There is no more info to retrieve about the specified
*                      event .
*       GT_BAD_PARAM - bad hndl parameter ,
*                      or hndl bound to CPSS_PP_TX_BUFFER_QUEUE_E --> not allowed
*                      use dedicated "get tx ended info" function instead
*       GT_BAD_PTR  - one of the parameters is NULL pointer
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssEventRecv
(
    IN  GT_UINTPTR          hndl,
    IN  CPSS_UNI_EV_CAUSE_ENT     evCause,
    OUT GT_U32              *evExtDataPtr,
    OUT GT_U8               *evDevPtr
)
{
    PRV_CPSS_DRV_EVENT_HNDL_STC *evHndlPtr; /* The event handle pointer     */
    PRV_CPSS_DRV_EV_REQ_NODE_STC*evNodePtr; /* The event node pointer       */

    /* validate the handle */
    if (0 == hndl)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    CPSS_NULL_PTR_CHECK_MAC(evExtDataPtr);
    CPSS_NULL_PTR_CHECK_MAC(evDevPtr);

    evHndlPtr = (PRV_CPSS_DRV_EVENT_HNDL_STC*)hndl;

    if (PRV_CPSS_DRV_TX_BUFF_QUEUE_EVENT_E == evHndlPtr->evType)
    {
        /* Tx End event type does not support this API */
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    /* get an event from queue */
    evNodePtr = prvCpssDrvEvReqQGet(evCause);

    if (NULL == evNodePtr)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NO_MORE, LOG_ERROR_NO_MSG);
    }

    *evExtDataPtr  = evNodePtr->uniEvExt;
    *evDevPtr      = evNodePtr->devNum;

    return GT_OK;
}


/*******************************************************************************
* uniEvHndlSet
*
* DESCRIPTION:
*       This routine sets the new user event handle in the queue control.
*
* APPLICABLE DEVICES:  All devices (PP / FA / XBAR)
*
* INPUTS:
*       uniEvent    - The unified event index.
*       evHndlPtr   - The user event handle.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK  - success
*       GT_FAIL - general failure
*       GT_ALREADY_EXIST - one of the unified events already bound to another
*                 handler (not applicable to CPSS_PP_TX_BUFFER_QUEUE_E)
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS uniEvHndlSet
(
    IN GT_U32                       uniEvent,
    IN PRV_CPSS_DRV_EVENT_HNDL_STC  *evHndlPtr
)
{
    /* regular events can be bound only once, validate it */
    if (PRV_CPSS_DRV_REGULAR_EVENT_E == evHndlPtr->evType)
    {

        if(NULL != prvCpssDrvEvReqQUserHndlGet(uniEvent))
        {
            DBG_LOG(("uniEvent %s already bound\n",
                     (GT_U32)uniEvName[uniEvent],2,3,4,5,6));
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_ALREADY_EXIST, LOG_ERROR_NO_MSG);
        }
    }

    return prvCpssDrvEvReqQUserHndlSet(uniEvent, evHndlPtr);
}

/*******************************************************************************
* deviceMaskWithEvExtDataSet
*
* DESCRIPTION:
*       This routine mask/unmasks an unified event on specific device , for
*       specific element in the event associated with extra data.
*       Since Each unified event may be associated with multiple HW interrupts,
*       each HW interrupt has different 'extra data' in the context of the
*       relevant unified event
*
* APPLICABLE DEVICES:  All devices (PP / FA / XBAR)
*
* INPUTS:
*       devNum - device number - PP/FA/Xbar device number -
*                depend on the uniEvent
*                if the uniEvent is in range of PP events , then devNum relate
*                to PP
*                if the uniEvent is in range of FA events , then devNum relate
*                to FA
*                if the uniEvent is in range of XBAR events , then devNum relate
*                to XBAR
*       uniEvent   - The unified event.
*       evExtData - The additional data (port num / priority
*                     queue number / other ) the event was received upon.
*                   may use value PRV_CPSS_DRV_EV_REQ_UNI_EV_EXTRA_DATA_ANY_CNS
*                   to indicate 'ALL interrupts' that relate to this unified
*                   event
*       operation  - the operation : mask / unmask
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK  - on success
*       GT_FAIL - on failure.
*       GT_BAD_PARAM - bad unify event value or bad device number
*       GT_NOT_FOUND - the unified event has no interrupts associated with it in
*                      the device Or the 'extra data' has value that not relate
*                      to the uniEvent
*       GT_NOT_INITIALIZED - the CPSS was not initialized properly to handle
*                   this type of event
*       GT_HW_ERROR - on hardware error
*
* COMMENTS:
*
*       The OS interrupts and the ExtDrv interrupts are locked during the
*       operation of the function.
*
*******************************************************************************/
static GT_STATUS deviceMaskWithEvExtDataSet
(
    IN GT_U8                    devNum,
    IN CPSS_UNI_EV_CAUSE_ENT    uniEvent,
    IN GT_U32                   evExtData,
    IN CPSS_EVENT_MASK_SET_ENT  operation
)
{
    GT_STATUS               rc = GT_BAD_PARAM;/* The Return Code            */
    GT_32                   intKey;         /* The interrupt lock key       */
    PRV_CPSS_DEVICE_TYPE_ENT deviceType;/*device type that the event relate to*/

    if(operation != CPSS_EVENT_MASK_E &&
       operation != CPSS_EVENT_UNMASK_E)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    if (uniEvent >= CPSS_PP_UNI_EV_MIN_E &&
        uniEvent <= CPSS_PP_UNI_EV_MAX_E)
    {
        /* events of PP */
        deviceType = PRV_CPSS_DEVICE_TYPE_PP_E;
    }
    else if(uniEvent >= CPSS_XBAR_UNI_EV_MIN_E &&
            uniEvent <= CPSS_XBAR_UNI_EV_MAX_E)
    {
        /* events of XBAR */
        deviceType = PRV_CPSS_DEVICE_TYPE_XBAR_E;
    }
    else if(uniEvent >= CPSS_FA_UNI_EV_MIN_E &&
            uniEvent <= CPSS_FA_UNI_EV_MAX_E)
    {
        /* events of FA */
        deviceType = PRV_CPSS_DEVICE_TYPE_FA_E;
    }
    else if(uniEvent >= CPSS_DRAGONITE_UNI_EV_MIN_E &&
            uniEvent <= CPSS_DRAGONITE_UNI_EV_MAX_E)
    {
        /* events of Dragonite */
        deviceType = PRV_CPSS_DEVICE_TYPE_DRAGONITE_E;
    }
    else
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    if(eventMaskDeviceFuncArray[deviceType] == NULL)
    {
        /* there is no callback function , relate to those events .
        the initialization was not done for it ... */
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_INITIALIZED, LOG_ERROR_NO_MSG);
    }

    /* lock section to disable interruption of ISR while mask/unmask event */
    intKey = 0;
    PRV_CPSS_LOCK_BY_MUTEX;
    cpssExtDrvSetIntLockUnlock(CPSS_OS_INTR_MODE_LOCK_E, &intKey);

    /* invoke the callback function that relate to this type of event */
    rc = (eventMaskDeviceFuncArray[deviceType])(devNum,uniEvent,
            evExtData,operation);

    /* unlock section to enable interruption of ISR when done */
    cpssExtDrvSetIntLockUnlock(CPSS_OS_INTR_MODE_UNLOCK_E, &intKey);
    PRV_CPSS_UNLOCK_BY_MUTEX;

    return rc;
}

/*******************************************************************************
* cpssEventDeviceMaskSet
*
* DESCRIPTION:
*       This routine mask/unmasks an unified event on specific device - in HW.
*
* APPLICABLE DEVICES:  All devices (PP / FA / XBAR)
*
* INPUTS:
*       devNum - device number - PP/FA/Xbar device number -
*                depend on the uniEvent
*                if the uniEvent is in range of PP events , then devNum relate
*                to PP
*                if the uniEvent is in range of FA events , then devNum relate
*                to FA
*                if the uniEvent is in range of XBAR events , then devNum relate
*                to XBAR
*       uniEvent   - The unified event.
*       operation  - the operation : mask / unmask
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK  - on success
*       GT_FAIL - on failure.
*       GT_BAD_PARAM - bad value on one of the parameters
*       GT_NOT_FOUND - the unified event has no interrupts associated with it in
*                      the device
*       GT_NOT_INITIALIZED - the CPSS was not initialized properly to handle
*                   this type of event
*       GT_HW_ERROR - on hardware error
*
* COMMENTS:
*
*       The OS interrupts and the ExtDrv interrupts are locked during the
*       operation of the function.
*
*******************************************************************************/
GT_STATUS cpssEventDeviceMaskSet
(
    IN GT_U8                    devNum,
    IN CPSS_UNI_EV_CAUSE_ENT    uniEvent,
    IN CPSS_EVENT_MASK_SET_ENT  operation
)
{
    /* Treat all interrupts relate to this unified event */
    return deviceMaskWithEvExtDataSet(devNum,uniEvent,
            PRV_CPSS_DRV_EV_REQ_UNI_EV_EXTRA_DATA_ANY_CNS,
            operation);
}

/*******************************************************************************
* cpssEventDeviceMaskWithEvExtDataSet
*
* DESCRIPTION:
*       This routine mask/unmasks an unified event on specific device , for
*       specific element in the event associated with extra data.
*       Since Each unified event may be associated with multiple HW interrupts,
*       each HW interrupt has different 'extra data' in the context of the
*       relevant unified event
*
* APPLICABLE DEVICES:  All devices (PP / FA / XBAR)
*
* INPUTS:
*       devNum - device number - PP/FA/Xbar device number -
*                depend on the uniEvent
*                if the uniEvent is in range of PP events , then devNum relate
*                to PP
*                if the uniEvent is in range of FA events , then devNum relate
*                to FA
*                if the uniEvent is in range of XBAR events , then devNum relate
*                to XBAR
*       uniEvent   - The unified event.
*       evExtData - The additional data (port num / priority
*                     queue number / other ) the event was received upon.
*       operation  - the operation : mask / unmask
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK  - on success
*       GT_FAIL - on failure.
*       GT_BAD_PARAM - bad unify event value or bad device number
*       GT_NOT_FOUND - the unified event has no interrupts associated with it in
*                      the device Or the 'extra data' has value that not relate
*                      to the uniEvent
*       GT_NOT_INITIALIZED - the CPSS was not initialized properly to handle
*                   this type of event
*       GT_HW_ERROR - on hardware error
*
* COMMENTS:
*
*       The OS interrupts and the ExtDrv interrupts are locked during the
*       operation of the function.
*
*******************************************************************************/
GT_STATUS cpssEventDeviceMaskWithEvExtDataSet
(
    IN GT_U8                    devNum,
    IN CPSS_UNI_EV_CAUSE_ENT    uniEvent,
    IN GT_U32                   evExtData,
    IN CPSS_EVENT_MASK_SET_ENT  operation
)
{
    /* Treat interrupt(s) relate to this unified event and 'extra info' */
    return deviceMaskWithEvExtDataSet(devNum,uniEvent,
            evExtData,operation);
}

/*******************************************************************************
* cpssEventDeviceGenerate
*
* DESCRIPTION:
*       This debug routine configures device to generate unified event for
*       specific element in the event associated with extra data.
*
* APPLICABLE DEVICES:  Bobcat2, Caelum, Bobcat3.
*
* INPUTS:
*       devNum - device number - PP/FA/Xbar device number -
*                depend on the uniEvent
*                if the uniEvent is in range of PP events , then devNum relate
*                to PP
*                if the uniEvent is in range of FA events , then devNum relate
*                to FA
*                if the uniEvent is in range of XBAR events , then devNum relate
*                to XBAR
*       uniEvent   - The unified event.
*       evExtData - The additional data (port num / priority
*                   queue number / other ) the event was received upon.
*                   Use CPSS_PARAM_NOT_USED_CNS to generate events for all supported evExtData.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK  - on success
*       GT_FAIL - on failure.
*       GT_BAD_PARAM - bad unify event value or bad device number
*       GT_NOT_FOUND - the unified event has no interrupts associated with it in
*                      the device Or the 'extra data' has value that not relate
*                      to the uniEvent
*       GT_NOT_INITIALIZED - the CPSS was not initialized properly to handle
*                   this type of event
*       GT_HW_ERROR - on hardware error
*       GT_NOT_SUPPORTED - on not supported unify event
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssEventDeviceGenerate
(
    IN GT_U8                    devNum,
    IN CPSS_UNI_EV_CAUSE_ENT    uniEvent,
    IN GT_U32                   evExtData
)
{
    GT_STATUS                rc = GT_BAD_PARAM;/* The Return Code            */
    GT_32                    intKey;         /* The interrupt lock key       */
    PRV_CPSS_DEVICE_TYPE_ENT deviceType;/*device type that the event relate to*/

    if (uniEvent >= CPSS_PP_UNI_EV_MIN_E &&
            uniEvent <= CPSS_PP_UNI_EV_MAX_E)
    {
        /* events of PP */
        deviceType = PRV_CPSS_DEVICE_TYPE_PP_E;
    }
    else if(uniEvent >= CPSS_XBAR_UNI_EV_MIN_E &&
            uniEvent <= CPSS_XBAR_UNI_EV_MAX_E)
    {
        /* events of XBAR */
        deviceType = PRV_CPSS_DEVICE_TYPE_XBAR_E;
    }
    else if(uniEvent >= CPSS_FA_UNI_EV_MIN_E &&
            uniEvent <= CPSS_FA_UNI_EV_MAX_E)
    {
        /* events of FA */
        deviceType = PRV_CPSS_DEVICE_TYPE_FA_E;
    }
    else if(uniEvent >= CPSS_DRAGONITE_UNI_EV_MIN_E &&
            uniEvent <= CPSS_DRAGONITE_UNI_EV_MAX_E)
    {
        /* events of Dragonite */
        deviceType = PRV_CPSS_DEVICE_TYPE_DRAGONITE_E;
    }
    else
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    if(eventGenerateDeviceFuncArray[deviceType] == NULL)
    {
        /* there is no callback function , relate to those events .
         * the initialization was not done for it ... */
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_INITIALIZED, LOG_ERROR_NO_MSG);
    }

    /* lock section to disable interruption of ISR while mask/unmask event */
    intKey = 0;
    PRV_CPSS_LOCK_BY_MUTEX;
    cpssExtDrvSetIntLockUnlock(CPSS_OS_INTR_MODE_LOCK_E, &intKey);

    /* invoke the callback function that relate to this type of event */
    rc = (eventGenerateDeviceFuncArray[deviceType])(devNum,uniEvent,
            evExtData);

    /* unlock section to enable interruption of ISR when done */
    cpssExtDrvSetIntLockUnlock(CPSS_OS_INTR_MODE_UNLOCK_E, &intKey);
    PRV_CPSS_UNLOCK_BY_MUTEX;

    return rc;
}

/*******************************************************************************
* cpssEventIsrBind
*
* DESCRIPTION:
*       This routine binds a user process to unified event. The routine returns
*       a handle that is used when the application wants to check if there are
*       waiting events (cpssEventWaitingEventsGet), clear the already served
*       events (cpssEventTreatedEventsClear) , receive the event(cpssEventRecv)
*
*       This is alternative way of treating events as opposed of the other
*       Select mechanism used with function cpssEventBind(...) (where
*       Application had to wait on binary semaphore, which was released by the
*       relevant interrupt).
*
*       In the new scheme, the application provides an ISR function which would
*       be called whenever the relevant interrupt(s) have been occurred.
*       Thus, allowing the application to use any synchronism mechanism which
*       suits its needs.
*       The application ISR function would be called at Interrupt context !!!
*
*       NOTE : 1. the function does not mask/unmask the HW events in any device.
*              This is Application responsibility to unmask the relevant events
*              on the needed devices , using function cpssEventDeviceMaskSet or
*              cpssEventDeviceMaskWithEvExtDataSet
*              2. for this handler the function cpssEventSelect(..) is not
*                 applicable.
*
* APPLICABLE DEVICES:  All devices (PP / FA / XBAR)
*
* INPUTS:
*       uniEventArr - The unified event list.
*       arrLength   - The unified event list length.
*       isrCbPtr  - (pointer for) application ISR function to be called when
*                     events are received
*       cookie  - storing application data (cookie)
*
* OUTPUTS:
*       hndlPtr     - (pointer to) The user handle for the bounded events.
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on failure
*       GT_BAD_PTR  - one of the parameters is NULL pointer
*       GT_OUT_OF_CPU_MEM - failed to allocate CPU memory,
*       GT_FULL - when trying to set the "tx buffer queue unify event"
*                 (CPSS_PP_TX_BUFFER_QUEUE_E) with other events in the same
*                 handler
*       GT_ALREADY_EXIST - one of the unified events already bound to another
*                 handler (not applicable to CPSS_PP_TX_BUFFER_QUEUE_E)
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssEventIsrBind
(
    IN  CPSS_UNI_EV_CAUSE_ENT       uniEventArr[],
    IN  GT_U32                      arrLength,
    IN  CPSS_EVENT_ISR_CB_FUNC      isrCbPtr,
    IN  GT_VOID*                    cookie,
    OUT GT_UINTPTR                  *hndlPtr
)
{
    return prvCpssEventBind(PRV_CPSS_DRV_HANDLE_BIND_USE_APPL_CB_TYPE_E,
                            uniEventArr,arrLength,
                            isrCbPtr,cookie,hndlPtr);
}

/*******************************************************************************
* cpssEventWaitingEventsGet
*
* DESCRIPTION:
*       This function returns a bitmap reflecting the unified events waiting
*       in queue for the user application.
*       the function get the events from the waiting queue.
*       This function should be called after a call to clear already served
*       events (cpssEventTreatedEventsClear).
*       The application receives a bitmap of events that are waiting.
*
* INPUTS:
*       hndl            - The user handle.
*       evBitmapLength  - The size of bitmap array in words.
*
* OUTPUTS:
*       evBitmapArr     - The updated bitmap array.(can be NULL if
*                         evBitmapLength ==0 )
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on failure
*       GT_BAD_PTR    - one of the parameters is NULL pointer
*       GT_BAD_PARAM  - the hndl parameter is not legal (was not returned by
*                       cpssEventIsrBind(...))
*       GT_NO_MORE -  no events are waiting
*
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssEventWaitingEventsGet
(
    IN GT_UINTPTR       hndl,
    OUT GT_U32          evBitmapArr[],
    IN GT_U32           evBitmapLength
)
{
    PRV_CPSS_DRV_EVENT_HNDL_STC *evHndlPtr; /* The event handle pointer     */

    /* validate the handle */
    if (0 == hndl)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    evHndlPtr = (PRV_CPSS_DRV_EVENT_HNDL_STC*)hndl;

    if(evHndlPtr->hndlBindType != PRV_CPSS_DRV_HANDLE_BIND_USE_APPL_CB_TYPE_E)
    {
        /* the hndl parameter is not legal --> was not created by
           cpssEventIsrBind(...) */
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    if(prvCpssDrvEvReqQBitmapGet(hndl,evBitmapArr,evBitmapLength) == 0)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NO_MORE, LOG_ERROR_NO_MSG);
    }

    return GT_OK;
}


/*******************************************************************************
* cpssEventTreatedEventsClear
*
* DESCRIPTION:
*       remove the events from the waiting queue , that already served
*       (Received) and has no new waiting for All Handler event.
*       The events are unified under a Handler received from the CPSS.
*
*       NOTE : the function protect itself by internal "interrupts lock" in
*              the start of function , and "interrupts unlock" in the end of
*              function.
*
* INPUTS:
*       hndl    - The user handle.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on failure
*       GT_BAD_PARAM  - the hndl parameter is not legal
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssEventTreatedEventsClear
(
    IN GT_UINTPTR       hndl
)
{
    /* validate the handle */
    if (0 == hndl)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    prvCpssDrvEvReqQRcvedEvRemoveAll(hndl);

    return GT_OK;
}

/*******************************************************************************
* prvCpssGenEventMaskDeviceBind
*
* DESCRIPTION:
*       This routine mask/unmasks an unified event on specific device.
*
* APPLICABLE DEVICES:  All devices (PP / FA / XBAR)
*
* INPUTS:
*       deviceType - device Type - PP/FA/Xbar device
*       funcPtr    - (pointer to) function that will be called when the
*                    Application wants to mask/unmask events relate to the
*                    deviceType
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK  - on success
*       GT_FAIL - on failure.
*       GT_BAD_PARAM - bad deviceType value or bad device number
*       GT_BAD_PTR  - funcPtr is NULL pointer
*
* COMMENTS:
*
*
*******************************************************************************/
GT_STATUS prvCpssGenEventMaskDeviceBind
(
    IN PRV_CPSS_DEVICE_TYPE_ENT    deviceType,
    IN PRV_CPSS_EVENT_MASK_DEVICE_SET_FUNC funcPtr
)
{
    /* check that the caller gave non-NULL function pointer ,
       if caller not want to handle events it must give pointer to dummy
       function and not NULL pointer */
    CPSS_NULL_PTR_CHECK_MAC(funcPtr);

    switch(deviceType)
    {
        case PRV_CPSS_DEVICE_TYPE_PP_E:
        case PRV_CPSS_DEVICE_TYPE_FA_E:
        case PRV_CPSS_DEVICE_TYPE_XBAR_E:
        case PRV_CPSS_DEVICE_TYPE_DRAGONITE_E:
            break;
        default:
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    eventMaskDeviceFuncArray[deviceType] = funcPtr;

    return GT_OK;
}

/*******************************************************************************
* prvCpssGenEventGenerateDeviceBind
*
* DESCRIPTION:
*       This routine binds function that configure device to generate unified event for
*       specific element in the event associated with extra data.
*
* APPLICABLE DEVICES:  All devices (PP / FA / XBAR)
*
* INPUTS:
*       deviceType - device Type - PP/FA/Xbar device
*       funcPtr    - pointer to function that will be called when the
*                    Application wants to generate events relate to the
*                    deviceType
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK  - on success
*       GT_FAIL - on failure.
*       GT_BAD_PARAM - bad deviceType value or bad device number
*       GT_BAD_PTR  - funcPtr is NULL pointer
*
* COMMENTS:
*
*
*******************************************************************************/
GT_STATUS prvCpssGenEventGenerateDeviceBind
(
    IN PRV_CPSS_DEVICE_TYPE_ENT    deviceType,
    IN PRV_CPSS_EVENT_GENERATE_DEVICE_SET_FUNC funcPtr
)
{
    /* check that the caller gave non-NULL function pointer ,
       if caller not want to handle events it must give pointer to dummy
       function and not NULL pointer */
    CPSS_NULL_PTR_CHECK_MAC(funcPtr);

    switch(deviceType)
    {
        case PRV_CPSS_DEVICE_TYPE_PP_E:
        case PRV_CPSS_DEVICE_TYPE_FA_E:
        case PRV_CPSS_DEVICE_TYPE_XBAR_E:
        case PRV_CPSS_DEVICE_TYPE_DRAGONITE_E:
            break;
        default:
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    eventGenerateDeviceFuncArray[deviceType] = funcPtr;

    return GT_OK;
}

/*******************************************************************************
* prvCpssGenEventGppIsrConnectBind
*
* DESCRIPTION:
*       This function binds function that connects an Isr for a given Gpp
*       interrupt.
*
* APPLICABLE DEVICES:  All devices (PP / FA / XBAR) with GPP
*
* INPUTS:
*       devNum      - The Pp device number at which the Gpp device is connected.
*       gppId       - The Gpp Id to be connected.
*       isrFuncPtr  - A pointer to the function to be called on Gpp interrupt
*                     reception.
*       cookie      - A cookie to be passed to the isrFuncPtr when its called.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK   - on success,
*       GT_FAIL - otherwise.
*       GT_BAD_PTR  - funcPtr is NULL pointer
*
* COMMENTS:
*       To disconnect a Gpp Isr, call this function with a NULL parameter in
*       the isrFuncPtr param.
*
*******************************************************************************/
GT_STATUS prvCpssGenEventGppIsrConnectBind
(
    IN PRV_CPSS_EVENT_GPP_ISR_CONNECT_FUNC funcPtr
)
{
    /* check that the caller gave non-NULL function pointer ,
       if caller not want to handle events it must give pointer to dummy
       function and not NULL pointer */
    CPSS_NULL_PTR_CHECK_MAC(funcPtr);

    eventGppIsrConnectFunc = funcPtr;

    return GT_OK;
}

/*******************************************************************************
* prvCpssGenEventGppIsrConnect
*
* DESCRIPTION:
*       This function connects an Isr for a given Gpp interrupt.
*
* APPLICABLE DEVICES:  All devices (PP / FA / XBAR) with GPP
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
*       To disconnect a Gpp Isr, call this function with a NULL parameter in
*       the isrFuncPtr param.
*
*******************************************************************************/
static GT_STATUS prvCpssGenEventGppIsrConnect
(
    void
)
{
    CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_SUPPORTED, LOG_ERROR_NO_MSG);
}


/*******************************************************************************
* cpssGenEventGppIsrConnect
*
* DESCRIPTION:
*       This function connects an Isr for a given Gpp interrupt.
*
* APPLICABLE DEVICES:  All devices (PP / FA / XBAR) with GPP
*
* INPUTS:
*       devNum      - The Pp device number at which the Gpp device is connected.
*       gppId       - The Gpp Id to be connected.
*       isrFuncPtr  - A pointer to the function to be called on Gpp interrupt
*                     reception.
*       cookie      - A cookie to be passed to the isrFuncPtr when its called.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK   - on success,
*       GT_FAIL - otherwise.
*
* COMMENTS:
*       To disconnect a Gpp Isr, call this function with a NULL parameter in
*       the isrFuncPtr param.
*
*******************************************************************************/
GT_STATUS cpssGenEventGppIsrConnect
(
    IN  GT_U8                   devNum,
    IN  CPSS_EVENT_GPP_ID_ENT   gppId,
    IN  CPSS_EVENT_ISR_FUNC     isrFuncPtr,
    IN  void                    *cookie
)
{
    return eventGppIsrConnectFunc(devNum,gppId,isrFuncPtr,cookie);
}

/*******************************************************************************
* cpssGenEventInterruptConnect
*
* DESCRIPTION:
*       Connect a given interrupt vector to an interrupt routine.
*
* APPLICABLE DEVICES:  All devices (PP / FA / XBAR)
*
* INPUTS:
*       intVecNum   - The interrupt vector number this device is connected to.
*       intMask     - The interrupt mask to enable/disable interrupts on
*                     this device.
*       intRoutine  - A pointer to the interrupt routine to be connected to the
*                     given interrupt line.
*       cookie      - A user defined cookie to be passed to the isr on interrupt
*                     reception.
*       connectionId- Id to be used for future access to the connected isr.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK on success, or
*       GT_FAIL if failed.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssGenEventInterruptConnect
(
    IN  GT_U32          intVecNum,
    IN  GT_U32          intMask,
    IN  CPSS_EVENT_ISR_FUNC    intRoutine,
    IN  void            *cookie,
    OUT GT_UINTPTR      *connectionId
)
{
    GT_STATUS retVal = GT_OK;
    retVal =  prvCpssDrvInterruptConnect(intVecNum,
                                         intMask,intRoutine,
                                         cookie,connectionId);
    if (GT_OK != retVal)
    {
        return retVal;
    }

    retVal =  prvCpssDrvInterruptEnable(intMask);
    return retVal;
}

/*******************************************************************************
* prvCpssGenEventRequestsDbRelease
*
* DESCRIPTION:
*       private (internal) function to release the DB of the event related.
*       NOTE: function  'free' the allocated memory.
*             and restore DB to 'pre-init' state
*
*  APPLICABLE DEVICES: ALL
*
* INPUTS:
*       none
*
* OUTPUTS:
*
* RETURNS:
*       GT_OK                    - on success.
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvCpssGenEventRequestsDbRelease
(
    void
)
{
    cpssOsMemSet(&eventMaskDeviceFuncArray,0,sizeof(eventMaskDeviceFuncArray));

    /* NOTE: this is application ability (and responsibility) to 'destroy' the
     'handlers' that hold allocated memory and semaphore that created during
     cpssEventBind(...)/cpssEventIsrBind(...)

     ---> see function cpssEventDestroy(...)
     */

    eventGppIsrConnectFunc = (PRV_CPSS_EVENT_GPP_ISR_CONNECT_FUNC)prvCpssGenEventGppIsrConnect;

    return GT_OK;
}

