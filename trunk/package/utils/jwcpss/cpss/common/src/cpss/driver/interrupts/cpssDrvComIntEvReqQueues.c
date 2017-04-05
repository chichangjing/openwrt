/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* cpssDrvComIntEvReqQueues.c
*
* DESCRIPTION:
*       This file includes functions for managing and configuring the interrupt
*       queues structure.
*
*
* FILE REVISION NUMBER:
*       $Revision: 12$
*
*******************************************************************************/
/* get the OS , extDrv functions*/
#include <cpss/extServices/private/prvCpssBindFunc.h>
/* get common defs */
#include <cpssCommon/cpssPresteraDefs.h>

/* get the common deriver info */
#include <cpss/driver/interrupts/cpssDrvComIntEvReqQueues.h>
#include <cpss/driver/interrupts/cpssDrvComIntSvcRtn.h>
#include <cpss/generic/events/private/prvCpssGenEvReq.h>

/*
 * Typedef: structure PRV_CPSS_DRV_COMMON_INT_EVENT_REQUEST_QUEUES_STC
 *
 * Description: the DB of this file
 *
 * Fields:
 *
 *      uniEvQArr - Array of queues (one for every event)
 *      uniEvQArrSize - The size of the above array
 *
 */
typedef struct{
    PRV_CPSS_DRV_EV_REQ_Q_CTRL_STC  *uniEvQArr;
    GT_U32                          uniEvQArrSize;
}PRV_CPSS_DRV_COMMON_INT_EVENT_REQUEST_QUEUES_STC;

/* the DB of this file */
static PRV_CPSS_DRV_COMMON_INT_EVENT_REQUEST_QUEUES_STC
                prvCpssDrvComIntEvReqQueuesDb = {NULL , 0};

/* The interrupt locking is needed for mutual exclusion prevention between   */
/* ISR and user task on the event queues.                                    */
#define INT_LOCK(key)                                                   \
    key = 0;                                                            \
    PRV_CPSS_LOCK_BY_MUTEX;                                        \
    cpssExtDrvSetIntLockUnlock(CPSS_OS_INTR_MODE_LOCK_E, &(key))

#define INT_UNLOCK(key)                                                 \
    cpssExtDrvSetIntLockUnlock(CPSS_OS_INTR_MODE_UNLOCK_E, &(key));     \
    PRV_CPSS_UNLOCK_BY_MUTEX

/* #define  EV_DEBUG */

#ifdef EV_DEBUG
#define DUMP(_x)  cpssOsPrintSync _x
#else
#define DUMP(_x)
#endif
/*******************************************************************************
* prvCpssDrvEvReqQReInit
*
* DESCRIPTION:
*       This function re-initializes the specified interrupts queues structure,
*       according to the user defined parameters, or according to the default
*       parameters.
*
* INPUTS:
*       numOfEvents - Number of interrupt events.
*
* OUTPUTS:
*
* RETURNS:
*       GT_OK on success, or
*       GT_FAIL if failed.
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvCpssDrvEvReqQReInit
(
    IN  GT_U32      numOfEvents
)
{
    GT_U32      i;

    if (prvCpssDrvComIntEvReqQueuesDb.uniEvQArrSize < numOfEvents)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_FAIL, LOG_ERROR_NO_MSG);
    }

    for(i = 0; i < numOfEvents; i++)
    {
        prvCpssDrvComIntEvReqQueuesDb.uniEvQArr[i].evNodeList     = NULL;
        prvCpssDrvComIntEvReqQueuesDb.uniEvQArr[i].userHndlPtr    = NULL;
        prvCpssDrvComIntEvReqQueuesDb.uniEvQArr[i].nextPtr        = NULL;
        prvCpssDrvComIntEvReqQueuesDb.uniEvQArr[i].uniEvCause     = i;
    }

    return GT_OK;
}


/*******************************************************************************
* prvCpssDrvEvReqQInit
*
* DESCRIPTION:
*       This function initializes the interrupts queues structure, according to
*       the user defined parameters, or according to the default parameters.
*
* INPUTS:
*       numOfEvents - the Maximal number of different events that this module
*                   will support.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK on success, or
*       GT_FAIL if failed.
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvCpssDrvEvReqQInit
(
    IN GT_U32       numOfEvents
)
{
    /* Check if Initialization has already been performed.
       Should be performed once per system.
       Exist in PP,FA and XBAR modules */
    if(prvCpssDrvComIntEvReqQueuesDb.uniEvQArr != NULL)
    {
        return GT_OK;
    }

    prvCpssDrvComIntEvReqQueuesDb.uniEvQArr =
        cpssOsMalloc(sizeof(PRV_CPSS_DRV_EV_REQ_Q_CTRL_STC) * numOfEvents);

    if(prvCpssDrvComIntEvReqQueuesDb.uniEvQArr == NULL)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_OUT_OF_CPU_MEM, LOG_ERROR_NO_MSG);
    }

    prvCpssDrvComIntEvReqQueuesDb.uniEvQArrSize = numOfEvents;

    return prvCpssDrvEvReqQReInit(numOfEvents);
}


/*******************************************************************************
* prvCpssDrvEvReqQUserHndlGet
*
* DESCRIPTION:
*       This function returns the user handle pointer for the event.
*
* INPUTS:
*       uniEvCause - The event number.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       !=NULL on success, or
*       NULL if failed.
*
* COMMENTS:
*
*******************************************************************************/
PRV_CPSS_DRV_EVENT_HNDL_STC  *prvCpssDrvEvReqQUserHndlGet
(
    IN GT_U32       uniEvCause
)
{
    if (prvCpssDrvComIntEvReqQueuesDb.uniEvQArrSize < uniEvCause)
    {
        return NULL;
    }

    return prvCpssDrvComIntEvReqQueuesDb.uniEvQArr[uniEvCause].userHndlPtr;
}


/*******************************************************************************
* prvCpssDrvEvReqQUserHndlSet
*
* DESCRIPTION:
*       This function sets the user handle pointer for the event.
*
* INPUTS:
*       uniEvCause      - The event number.
*       evQNewHndlPtr   - The new user handler
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK on success, or
*       GT_FAIL if failed.
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS  prvCpssDrvEvReqQUserHndlSet
(
    IN GT_U32               uniEvCause,
    IN PRV_CPSS_DRV_EVENT_HNDL_STC        *evQNewHndlPtr
)
{
    if (prvCpssDrvComIntEvReqQueuesDb.uniEvQArrSize < uniEvCause)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_FAIL, LOG_ERROR_NO_MSG);
    }

    /* link the user handle the new queue event control */
    prvCpssDrvComIntEvReqQueuesDb.uniEvQArr[uniEvCause].nextPtr = evQNewHndlPtr->evListPtr;
    evQNewHndlPtr->evListPtr      = &prvCpssDrvComIntEvReqQueuesDb.uniEvQArr[uniEvCause];

    /* save the user handle in queue control */
    prvCpssDrvComIntEvReqQueuesDb.uniEvQArr[uniEvCause].userHndlPtr = evQNewHndlPtr;

    return GT_OK;
}


/*******************************************************************************
* prvCpssDrvEvReqQInsert
*
* DESCRIPTION:
*       This function inserts a new event node to the tail of the event queue.
*       The active event counter is incremented.
*
* INPUTS:
*       evNodePool  - Pointer to the interrupt nodes pool the interrupt belongs
*                     to.
*       intIndex    - The interrupt index.
*       masked      - Indicates if the interrupt was received while it was
*                     masked.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK on success, or
*       GT_FAIL if failed.
*
* COMMENTS:
*       This routine is invoked from ISR context !! If Application process needs
*       to invoke the routine, be shure to protect the call with task and
*       inerrupt lock.
*
*******************************************************************************/
GT_STATUS prvCpssDrvEvReqQInsert
(
    IN PRV_CPSS_DRV_EV_REQ_NODE_STC      evNodePool[],
    IN GT_U32           intIndex,
    IN GT_BOOL          masked
)
{
    PRV_CPSS_DRV_EV_REQ_NODE_STC     *newNode;        /* The new node to be inserted.     */
    PRV_CPSS_DRV_EV_REQ_Q_CTRL_STC   *evQueue;       /* The unified event queue          */

    /* set the node to be inserted */
    newNode = &(evNodePool[intIndex]);

    DUMP((" ISR QInsert int 0x%x stat %d masked %d\n",intIndex, newNode->intRecStatus, masked ));

    switch (newNode->intRecStatus)
    {
        case PRV_CPSS_DRV_EV_DRVN_INT_RCVD_E:
            /* int is allready masked, update the record status */
            newNode->intRecStatus = PRV_CPSS_DRV_EV_DRVN_INT_MASKED_E;
            return GT_OK;

        case PRV_CPSS_DRV_EV_DRVN_INT_MASKED_E:
            /* int is allready masked, do nothing */
            return GT_OK;

        default:
            break;
    }

    /* Insert a new node only if it was unmasked. */
    if (GT_TRUE == masked)
    {
        /* handle IDLE_READY that was not handled in the switch case above */
        if(newNode->intRecStatus == PRV_CPSS_DRV_EV_DRVN_INT_IDLE_READY_E)
        {
            /* int is allready masked, update the record status */
            newNode->intRecStatus = PRV_CPSS_DRV_EV_DRVN_INT_MASKED_E;
        }
        return GT_OK;
    }

    evQueue = &(prvCpssDrvComIntEvReqQueuesDb.uniEvQArr[newNode->uniEvCause]);

    /* don't insert IDLE_READY interrupt to the queue. It already in queue.
      we need only send signal for IDLE_READY interrupt to avoid missing
      of the interrupt  */
    if(newNode->intRecStatus != PRV_CPSS_DRV_EV_DRVN_INT_IDLE_READY_E)
    {
        newNode->intRecStatus = PRV_CPSS_DRV_EV_DRVN_INT_RCVD_E;

        if (NULL == evQueue->evNodeList)
        {
            /* queue is empty */
            evQueue->evNodeList = newNode;
            newNode->nextPtr    = newNode;
            newNode->prevPtr    = newNode;
        }
        else
        {
            /* queue isn`t empty */
            evQueue->evNodeList->prevPtr->nextPtr   = newNode;
            newNode->nextPtr                        = evQueue->evNodeList;
            newNode->prevPtr                        = evQueue->evNodeList->prevPtr;
            evQueue->evNodeList->prevPtr            = newNode;
        }
    }
    else
    {
        /* notify Event Select procedure that event need to be handled
           one more time but not removed from queue */
        newNode->intRecStatus = PRV_CPSS_DRV_EV_DRVN_INT_MASKED_E;
    }

    /* and signal the user process */
    if (NULL != evQueue->userHndlPtr)
    {
        /* and signal the user process ,or */
        /* call the application's CB function */
        prvCpssDrvEvReqNotify(evQueue->userHndlPtr);
    }

    return GT_OK;
}


/*******************************************************************************
* prvCpssDrvEvReqQGet
*
* DESCRIPTION:
*       This function gets the first interrupt node information from the
*       selected unified event queue.
*
* INPUTS:
*       uniEvCause  - The unified event queue.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       A pointer to the selected interrupt node, or NULL if no interrupts left.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
PRV_CPSS_DRV_EV_REQ_NODE_STC *prvCpssDrvEvReqQGet
(
    IN GT_U32           uniEvCause
)
{
    PRV_CPSS_DRV_EV_REQ_NODE_STC   *evNodePtr;         /* The event node pointer */
    PRV_CPSS_DRV_EV_REQ_Q_CTRL_STC *evQueuePtr;        /* The event queue pointer*/
    GT_32               intKey;       /* The interrupt lock key       */
    PRV_CPSS_DRV_EV_REQ_NODE_STC   *lastEvNodePtr;     /* The last event in queue*/

    /* perform param and status validity */
    if (prvCpssDrvComIntEvReqQueuesDb.uniEvQArrSize < uniEvCause)
    {
        return NULL;
    }

    evQueuePtr = &(prvCpssDrvComIntEvReqQueuesDb.uniEvQArr[uniEvCause]);

    INT_LOCK(intKey);

    if (NULL == evQueuePtr->evNodeList)
    {
        INT_UNLOCK(intKey);
        /* the queue is empty */
        return NULL;
    }

    /* set the first and last events in queue */
    evNodePtr       = evQueuePtr->evNodeList;
    lastEvNodePtr   = evQueuePtr->evNodeList->prevPtr;

    /* search for the first active event in queue */
    while(1)
    {
        /* treat only received and masked events */
        if (PRV_CPSS_DRV_EV_DRVN_INT_MASKED_E == evNodePtr->intRecStatus ||
            PRV_CPSS_DRV_EV_DRVN_INT_RCVD_E == evNodePtr->intRecStatus)
        {
            /* we found an event, update the record status to */
            /* Idle Ready. This state is used on select to    */
            /* remove event from queue and unmask interrupt.  */
            evNodePtr->intRecStatus = PRV_CPSS_DRV_EV_DRVN_INT_IDLE_READY_E;

            /* update the list to the next event in queue */
            evQueuePtr->evNodeList = evNodePtr->nextPtr;
            break;
        }

        /* check that we did not reach end of queue */
        if (lastEvNodePtr == evNodePtr)
        {
            /* end of a queue */
            evNodePtr = NULL;

            /* make quick finish of the while loop without condition check */
            break;
        }

        /* if we reached here, proceed to the next node */
        evNodePtr = evNodePtr->nextPtr;

    }

    INT_UNLOCK(intKey);

    return evNodePtr;
}


/*******************************************************************************
* prvCpssDrvEvReqQBitmapGet
*
* DESCRIPTION:
*       This function returns a bitmap reflecting the unified events waiting
*       in queue for the user application.
*
* INPUTS:
*       hndl            - The user handle.
*       evBitmapArr     - The bitmap array.
*       evBitmapLength  - The size of bitmap array in words.
*
* OUTPUTS:
*       evBitmapArr     - The updated bitmap array.
*
* RETURNS:
*       The number of new events in bitmap.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_U32 prvCpssDrvEvReqQBitmapGet
(
    IN GT_UINTPTR       hndl,
    INOUT GT_U32        evBitmapArr[],
    IN GT_U32           evBitmapLength
)
{
    GT_U32              evCount;       /* number of new events */
    PRV_CPSS_DRV_EVENT_HNDL_STC *hndlEvPtr;    /* handle event list */
    PRV_CPSS_DRV_EV_REQ_Q_CTRL_STC       *evReqPtr;      /*  */

    evCount             = 0;
    hndlEvPtr           = (PRV_CPSS_DRV_EVENT_HNDL_STC*)hndl;
    evReqPtr            = hndlEvPtr->evListPtr;

    if (NULL != evBitmapArr)
    {
        /* zero out the bitmap */
        cpssOsBzero((GT_VOID*)evBitmapArr, evBitmapLength * sizeof(evBitmapArr[0]));
    }

    if (PRV_CPSS_DRV_TX_BUFF_QUEUE_EVENT_E == hndlEvPtr->evType)
    {
        /* see if there is a TxBufferQueue event waiting in the FIFO for the app */
        if (NULL != ((PRV_CPSS_TX_BUF_QUEUE_FIFO_STC *)hndlEvPtr->extDataPtr)->headPtr)
        {
            /* at least one TxBufferQueue in FIFO */
            if (NULL != evBitmapArr)
            {
                evBitmapArr[evReqPtr->uniEvCause >> 5] |=
                                            (1 << (evReqPtr->uniEvCause & 0x1F));
            }
            evCount = 1;
        }
    }
    else
    {
        if (NULL == evBitmapArr)
        {
            /* count the waiting events only */
            while (evReqPtr)
            {
                if (evReqPtr->evNodeList != NULL)
                {
                    evCount++;
                }
                /* move to the next binded event */
                evReqPtr = evReqPtr->nextPtr;
            }
        }
        else
        {
            /* update the bitmap for all user binded events and count them */
            while (evReqPtr)
            {
                if (evReqPtr->evNodeList != NULL &&
                    (evReqPtr->uniEvCause >> 5) < evBitmapLength)
                {
                    evBitmapArr[evReqPtr->uniEvCause >> 5] |=
                                            (1 << (evReqPtr->uniEvCause & 0x1F));
                    evCount++;
                }
                /* move to the next binded event */
                evReqPtr = evReqPtr->nextPtr;
            }
        }
    }

    return evCount;
}


/*******************************************************************************
* prvCpssDrvEvReqQRemoveDev
*
* DESCRIPTION:
*       This function is called upon Hot removal of a device, inorder to remove
*       the interrupt nodes belonging to this device from the interrupts queues.
*
* INPUTS:
*       devNum  - The device number.
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
GT_STATUS prvCpssDrvEvReqQRemoveDev
(
    IN  GT_U8           devNum
)
{
    PRV_CPSS_DRV_EV_REQ_Q_CTRL_STC       *evQueuePtr;        /* The event queue pointer      */
    PRV_CPSS_DRV_EV_REQ_NODE_STC         *evNodePtr;         /* The event node pointer       */
    PRV_CPSS_DRV_EV_REQ_NODE_STC         *lastEvNodePtr;     /* The last event in queue      */
    PRV_CPSS_DRV_EV_REQ_NODE_STC         *nextEvNodePtr;     /* The next event node in queue */
    GT_32               intKey;             /* The interrupt lock key       */
    GT_U32              timeOut;            /* The timeout to wait for app  */
    GT_U32              i;                  /* Iterator                     */

    INT_LOCK(intKey);

    for (i = 0; i < prvCpssDrvComIntEvReqQueuesDb.uniEvQArrSize; i++)
    {
        if (NULL == prvCpssDrvComIntEvReqQueuesDb.uniEvQArr[i].evNodeList)
        {
            continue;
        }

        evQueuePtr = &(prvCpssDrvComIntEvReqQueuesDb.uniEvQArr[i]);

        /* set the first and last events in queue */
        nextEvNodePtr = NULL;
        evNodePtr     = evQueuePtr->evNodeList;
        lastEvNodePtr = evQueuePtr->evNodeList->prevPtr;

        /* set intStatus to pending only for requested event */
        while (evQueuePtr->evNodeList != NULL)
        {
            if (evNodePtr->devNum == devNum)
            {
                /* Set the interrupt to pending state   */
                evNodePtr->intStatus = PRV_CPSS_DRV_EV_DRVN_INT_STOPPED_E;

                /* Wait until the interrupt is handled  */
                timeOut = 1000;

                while (evNodePtr->intRecStatus != PRV_CPSS_DRV_EV_DRVN_INT_IDLE_E)
                {
                    cpssOsTimerWkAfter(1);

                    if (--timeOut == 0)
                    {
                        INT_UNLOCK(intKey);
                        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_FAIL, LOG_ERROR_NO_MSG);
                    }
                }

                /* Remove the int node from the interrupt queue          */
                /* The node to be deleted is the first node in the list. */
                if(evNodePtr == evQueuePtr->evNodeList)
                {
                    /* one element in list */
                    if (evNodePtr->nextPtr == evNodePtr)
                    {
                        evQueuePtr->evNodeList = NULL;
                    }
                    else
                    {
                        evQueuePtr->evNodeList          = evNodePtr->nextPtr;
                        evNodePtr->prevPtr->nextPtr     = evQueuePtr->evNodeList;
                        evQueuePtr->evNodeList->prevPtr = evNodePtr->prevPtr;
                    }
                }
                else
                {
                    /* the element is not the first */
                    evNodePtr->prevPtr->nextPtr = evNodePtr->nextPtr;
                    evNodePtr->nextPtr->prevPtr = evNodePtr->prevPtr;
                }

                /* save the deleted node pointer */
                nextEvNodePtr            = evNodePtr->nextPtr;
                evNodePtr->intRecStatus  = PRV_CPSS_DRV_EV_DRVN_INT_IDLE_E;
                evNodePtr->intStatus     = PRV_CPSS_DRV_EV_DRVN_INT_STOPPED_E;
                evNodePtr->nextPtr       = NULL;
                evNodePtr->prevPtr       = NULL;
            }
            else
            {
                /* goto next event */
                nextEvNodePtr            = evNodePtr->nextPtr;
            }

            if (lastEvNodePtr == evNodePtr)
            {
                /* this is the last element in list */
                break;
            }
            else
            {
                /* proceed to the next element in list */
                evNodePtr = nextEvNodePtr;
            }
        }
    }

    INT_UNLOCK(intKey);

    return GT_OK;
}



/*******************************************************************************
* evReqQRcvedEvRemoveOne
*
* DESCRIPTION:
*       This routine removes one user binded event that is active and record
*       status is Idle Ready. All removed events are unmasked.
*
* INPUTS:
*       uniEvCause - The event number.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       None.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS evReqQRcvedEvRemoveOne
(
    IN GT_U32           uniEvCause
)
{
    PRV_CPSS_DRV_EV_REQ_Q_CTRL_STC       *evQueuePtr;        /* The event queue pointer      */
    PRV_CPSS_DRV_EV_REQ_NODE_STC         *evNodePtr;         /* The event node pointer       */
    PRV_CPSS_DRV_EV_REQ_NODE_STC         *lastEvNodePtr;     /* The last event in queue      */
    PRV_CPSS_DRV_EV_REQ_NODE_STC         *nextEvNodePtr;     /* The next event node in queue */

    evQueuePtr = &(prvCpssDrvComIntEvReqQueuesDb.uniEvQArr[uniEvCause]);

    evNodePtr     = evQueuePtr->evNodeList;
    if (evNodePtr == NULL)
    {
        return GT_OK;
    }
    lastEvNodePtr = evQueuePtr->evNodeList->prevPtr;
    nextEvNodePtr = NULL;

    while (1)
    {
        /* If an interrupt was received while it was masked,    */
        /* re-handle this interrupt and dont remove it.         */
        if (PRV_CPSS_DRV_EV_DRVN_INT_MASKED_E == evNodePtr->intRecStatus ||
            PRV_CPSS_DRV_EV_DRVN_INT_RCVD_E   == evNodePtr->intRecStatus)
        {
            if (NULL == evQueuePtr->evNodeList || lastEvNodePtr == evNodePtr)
            {
                /* this is last event in a queue */
                break;
            }
            evNodePtr = evNodePtr->nextPtr;
            continue;
        }

        /* if we reached here, we need to remove node from list and unmask event */

        /* The node to be deleted is the first node in the list. */
        if(evNodePtr == evQueuePtr->evNodeList)
        {
            /* one element in list */
            if (evNodePtr->nextPtr == evNodePtr)
            {
                evQueuePtr->evNodeList = NULL;
            }
            else
            {
                evQueuePtr->evNodeList          = evNodePtr->nextPtr;
                evNodePtr->prevPtr->nextPtr     = evQueuePtr->evNodeList;
                evQueuePtr->evNodeList->prevPtr = evNodePtr->prevPtr;
            }
        }
        else
        {
            /* the element is not the first */
            evNodePtr->prevPtr->nextPtr = evNodePtr->nextPtr;
            evNodePtr->nextPtr->prevPtr = evNodePtr->prevPtr;
        }

        /* save next node of the deleted node pointer */
        nextEvNodePtr            = evNodePtr->nextPtr;

        /* update fields of deleted event */
        evNodePtr->prevPtr       = NULL;
        evNodePtr->nextPtr       = NULL;
        evNodePtr->intRecStatus  = PRV_CPSS_DRV_EV_DRVN_INT_IDLE_E;

        /* unmask event if it not stopped */
        if (evNodePtr->intStatus != PRV_CPSS_DRV_EV_DRVN_INT_STOPPED_E)
        {
            evNodePtr->intMaskSetFptr(evNodePtr, CPSS_EVENT_UNMASK_E);
        }

        if (NULL == evQueuePtr->evNodeList || lastEvNodePtr == evNodePtr)
        {
            /* this is the last element in list */
            break;
        }
        else
        {
            /* proceed to the next element in list */
            evNodePtr = nextEvNodePtr;
        }
    }

    return GT_OK;
}

/*******************************************************************************
* prvCpssDrvEvReqQRcvedEvRemoveAll
*
* DESCRIPTION:
*       This routine removes all user binded events that are active and record
*       status is Idle Ready. All removed events are unmasked.
*
* INPUTS:
*       hndl    - The user handle.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       None.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_VOID prvCpssDrvEvReqQRcvedEvRemoveAll
(
    IN GT_UINTPTR       hndl
)
{
    GT_32               intKey;        /* The interrupt lock key            */
    PRV_CPSS_DRV_EV_REQ_Q_CTRL_STC       *hndlEvPtr;    /* handle event list                 */


    hndlEvPtr   = ((PRV_CPSS_DRV_EVENT_HNDL_STC*)hndl)->evListPtr;

    INT_LOCK(intKey);

    while (hndlEvPtr)
    {
        if (hndlEvPtr->evNodeList != NULL)
        {
            evReqQRcvedEvRemoveOne(hndlEvPtr->uniEvCause);
        }
        hndlEvPtr = hndlEvPtr->nextPtr;
    }

    INT_UNLOCK(intKey);
}


/*******************************************************************************
* prvCpssDrvEvReqUniEvMaskSet
*
* DESCRIPTION:
*       This routine mask/unmask the selected unified event.
*
* INPUTS:
*       intNodesPool    - The event node array for the device.
*       intNodesPoolSize- The number of entries in the intNodesPool.
*       uniEvent        - The unified event to be unmasked.
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
*       GT_OK - on success
*       GT_NOT_FOUND - the unified event or the evExtData within the unified
*                      event are not found in this device interrupts
*       GT_HW_ERROR - on hardware error
*
* COMMENTS:
*       This function called when OS interrupts and the ExtDrv interrupts are
*       locked !
*
*******************************************************************************/
#ifdef CHX_FAMILY
extern GT_STATUS prvCpssDxChPortInterruptDisambiguation
(
    IN  GT_U8     devNum,
    IN  GT_U32    portNum,
    OUT GT_BOOL   *extendedModePtr
);
#endif 
GT_STATUS prvCpssDrvEvReqUniEvMaskSet
(
    IN PRV_CPSS_DRV_EV_REQ_NODE_STC             intNodesPool[],
    IN GT_U32               intNodesPoolSize,
    IN GT_U32               uniEvent,
    IN GT_U32               evExtData,
    IN CPSS_EVENT_MASK_SET_ENT operation
)
{
    PRV_CPSS_DRV_EV_REQ_NODE_STC  *nodePoolArr;   /* The event node array   */
    GT_U32                  event;          /* The event index              */
    GT_STATUS               rc;             /* The Return Code              */
    GT_U32                  found = 0;/* indication whether the event was found */
                                      /* also support event with specific extra data*/
#ifdef CHX_FAMILY
    GT_BOOL                 extendedMode;
#endif 

    nodePoolArr = intNodesPool;

    for (event = 0; event < intNodesPoolSize; event++)
    {
        if (nodePoolArr[event].uniEvCause != uniEvent)
        {
            continue;
        }

        if(evExtData != PRV_CPSS_DRV_EV_REQ_UNI_EV_EXTRA_DATA_ANY_CNS)
        {
            if(nodePoolArr[event].uniEvExt != evExtData)
            {
                /* we need to mask/unmask only the specific interrupts and not all
                   interrupts that connected to the unified event */
                continue;
            }
#ifdef CHX_FAMILY
            if(CPSS_EVENT_UNMASK_E == operation)
            {
                switch(uniEvent)
                {
                    case CPSS_PP_PORT_LINK_STATUS_CHANGED_E:
                    case CPSS_PP_PORT_AN_COMPLETED_E:
                    case CPSS_PP_PORT_RX_FIFO_OVERRUN_E:
                    case CPSS_PP_PORT_TX_FIFO_UNDERRUN_E:
                    case CPSS_PP_PORT_TX_FIFO_OVERRUN_E:
                    case CPSS_PP_PORT_TX_UNDERRUN_E:
                    case CPSS_PP_PORT_ADDRESS_OUT_OF_RANGE_E:
                    case CPSS_PP_PORT_PRBS_ERROR_E:
                    case CPSS_PP_PORT_SYNC_STATUS_CHANGED_E:
                    case CPSS_PP_PORT_ILLEGAL_SEQUENCE_E:
                    case CPSS_PP_PORT_FAULT_TYPE_CHANGE_E:
                    case CPSS_PP_PORT_FC_STATUS_CHANGED_E:
                    case CPSS_PP_PORT_NO_BUFF_PACKET_DROP_E:
                    case CPSS_PP_PORT_COUNT_COPY_DONE_E:
                    case CPSS_PP_PORT_COUNT_EXPIRED_E:
                    case CPSS_PP_PORT_MMPCS_SIGNAL_DETECT_CHANGE_E:
                        rc = prvCpssDxChPortInterruptDisambiguation(nodePoolArr[event].devNum, 
                                                                    evExtData, &extendedMode);
                        if(rc != GT_BAD_STATE)
                        {
                            if(((0 == found%2) && extendedMode)
                               || ((found%2 != 0) && !extendedMode))
                            {
                                found ++;
                                continue;
                            }
                        }

                        if((rc != GT_OK) && (rc != GT_BAD_STATE))
                        {
                            return rc;
                        }

                        break;
                    default:
                        break;
                }
            }
#endif 
        }

        /* we found at least one interrupt associated with the event and extra info */
        found ++;

        /* mask/unmask the HW interrupt */
        rc = nodePoolArr[event].intMaskSetFptr(&nodePoolArr[event], operation);

        if (rc != GT_OK)
        {
            return rc;
        }
    }

    if(found == 0)
    {
        /* the unified event or the evExtData within the unified event are not
           found in this device interrupts */
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_FOUND, LOG_ERROR_NO_MSG);
    }

    return GT_OK;
}

/*******************************************************************************
* prvCpssDrvEvReqNotify
*
* DESCRIPTION:
*       This function notify the waiting layer about the occurrence of event.
*       this function will "Signal" the waiting task or will call the
*       Application's call back that is bound to the handler
*
* INPUTS:
*       hndlEvPtr  - (Pointer to) the handler info.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       None.
*
* COMMENTS:
*       This routine is invoked from ISR context !!
*
*******************************************************************************/
GT_VOID prvCpssDrvEvReqNotify
(
    IN PRV_CPSS_DRV_EVENT_HNDL_STC      *hndlEvPtr
)
{
    if(hndlEvPtr->hndlBindType == PRV_CPSS_DRV_HANDLE_BIND_USE_SIGNAL_TYPE_E)
    {
        /* signal the user process if not already signalled by this routine */
        if(GT_FALSE == PRV_CPSS_DRV_HANDEL_SEM_SIGNAL_MAC(hndlEvPtr))
        {
             PRV_CPSS_DRV_HANDEL_SEM_SIGNAL_MAC(hndlEvPtr) = GT_TRUE;
            /* and signal the user process */
            cpssOsSigSemSignal((CPSS_OS_SIG_SEM)PRV_CPSS_DRV_HANDEL_SEM_MAC(hndlEvPtr));
        }
    }
    else
    {
        /* call the application's CB function */
        PRV_CPSS_DRV_HANDEL_APPL_CB_MAC(hndlEvPtr)(  /* CB */
            PRV_CPSS_DRV_HANDEL_MAC(hndlEvPtr),      /* handle */
            PRV_CPSS_DRV_HANDEL_APPL_COOKIE_MAC(hndlEvPtr));/* cookie */
    }

    return;
}

/*******************************************************************************
* prvCpssDrvComIntEvReqQueuesDbRelease
*
* DESCRIPTION:
*       private (internal) function to release the DB of the common driver for request driven.
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
GT_STATUS prvCpssDrvComIntEvReqQueuesDbRelease
(
    void
)
{
    GT_STATUS rc;

    FREE_PTR_MAC(prvCpssDrvComIntEvReqQueuesDb.uniEvQArr);

    cpssOsMemSet(&prvCpssDrvComIntEvReqQueuesDb,0,sizeof(prvCpssDrvComIntEvReqQueuesDb));

    rc = prvCpssGenEventRequestsDbRelease();

    return rc;
}

