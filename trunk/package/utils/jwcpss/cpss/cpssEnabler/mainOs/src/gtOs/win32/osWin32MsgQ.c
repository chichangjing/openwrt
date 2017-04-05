/*******************************************************************************
* osWin32MsgQ.c
*
* DESCRIPTION:
*       Win32 User Mode Operating System wrapper. Message queues
*
* DEPENDENCIES:
*       Win32, CPU independed , and posix threads implementation.
*
* FILE REVISION NUMBER:
*       $Revision: 4 $
*******************************************************************************/

#include <gtOs/gtOsMsgQ.h>
#include <gtOs/gtOsMem.h>
#include <gtOs/gtOsSem.h>
#include <gtOs/gtOsIo.h>
#include <gtOs/gtOsTimer.h>

#include <windows.h>

#include <gtOs/osObjIdLib.h>

/************* Defines ********************************************************/
#ifndef OS_MESSAGE_QUEUES
#  define OS_MESSAGE_QUEUES 10
#endif

/* enable statistic collection for userspace mutexes */
#undef OS_MESSAGE_QUEUES_STAT

/************ Internal Typedefs ***********************************************/
typedef struct _osMsgQ
{
    OS_OBJECT_HEADER_STC    header;
    GT_MUTEX                mtx;
    GT_SEM                  rxSem;
    GT_SEM                  txSem;
    int                     maxMsgs;
    int                     maxMsgSize;
    int                     messages;
    char                    *buffer;
    int                     head;
    int                     tail;
    int                     waitRx;
    int                     waitTx;
} OS_MSGQ_STC;

#ifdef OS_MALLOC_MEMORY_LEAKAGE_DBG
static GT_VOID *myOsMalloc
(
    IN GT_U32 size
)
{
    return osMalloc_MemoryLeakageDbg(size,__FILE__,__LINE__);
}
static GT_VOID myOsFree
(
    IN GT_VOID* const memblock
)
{
    osFree_MemoryLeakageDbg(memblock,__FILE__,__LINE__);
}
#endif /*OS_MALLOC_MEMORY_LEAKAGE_DBG*/

static OS_OBJECT_LIST_STC messageQueues = {
    NULL,                               /* list */
    0,                                  /* allocated */
    64,                                 /* allocChunk */
    sizeof(OS_MSGQ_STC),                /* objSize */
#ifdef OS_MALLOC_MEMORY_LEAKAGE_DBG
    (OS_OBJ_ALLOC_FUNC_TYPE)myOsMalloc,   /* allocFuncPtr */
    myOsFree                              /* freeFuncPtr */
#else  /*!OS_MALLOC_MEMORY_LEAKAGE_DBG*/
    (OS_OBJ_ALLOC_FUNC_TYPE)osMalloc,   /* allocFuncPtr */
    osFree                              /* freeFuncPtr */
#endif /*!OS_MALLOC_MEMORY_LEAKAGE_DBG*/
};
static HANDLE messageQueues_mtx = (HANDLE)NULL;

/************ Implementation defines ******************************************/
#define MSGQ_CHECK() \
    do { \
        if ((int)msgqId < 1 || (int)msgqId >= messageQueues.allocated) \
        { \
            /* bad msgqId */ \
            return GT_FAIL; \
        } \
        q = (OS_MSGQ_STC*)(messageQueues.list[(int)msgqId]); \
        if (!q || q->header.type == 0) \
        { \
            /* uninitialized or deleted */ \
            return GT_FAIL; \
        } \
    } while (0)

/************ Public Functions ************************************************/

/*******************************************************************************
* osMsgQCreate
*
* DESCRIPTION:
*       Create and initialize a message queue.
*
* INPUTS:
*       name       - message queue name
*       maxMsgs    - max messages in queue
*       maxMsgSize - max length of single message
*
* OUTPUTS:
*       msgqId - message queue id
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on error
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS osMsgQCreate
(
    IN  const char    *name,
    IN  GT_U32        maxMsgs,
    IN  GT_U32        maxMsgSize,
    OUT GT_MSGQ_ID    *msgqId
)
{
    int qnum;
    OS_MSGQ_STC *q;
    char tmpName[100];
    GT_U32 retVal;

    if (messageQueues_mtx == (HANDLE)NULL)
    {
        messageQueues_mtx = CreateMutex(NULL, FALSE, "Local\\msgqmtx");
    }

    retVal = WaitForSingleObject(messageQueues_mtx, INFINITE);
    if(retVal != WAIT_OBJECT_0)
    {
        goto ret_fail;
    }

    qnum = osObjLibGetNewObject(&messageQueues, 1, name, (OS_OBJECT_HEADER_STC**)&q);
    if (qnum <= 0)
    {
ret_fail:
        ReleaseMutex(messageQueues_mtx);
        return GT_FAIL;
    }


    /* align max message size by 4 bytes */
    maxMsgSize = (maxMsgSize+3) & ~3;
    q->buffer = (char*)osMalloc((maxMsgSize + sizeof(GT_U32))*maxMsgs);
    if (q->buffer == NULL)
    {
        goto ret_fail;
    }
    osSprintf(tmpName,"mQ-M-%s",name);
    if (osMutexCreate(tmpName, &(q->mtx)) != GT_OK)
    {
ret_fail_1:
        osFree(q->buffer);
        goto ret_fail;
    }
    osSprintf(tmpName,"mQ-R-%s",name);
    if (osSemBinCreate(tmpName, OS_SEMB_EMPTY, &(q->rxSem)) != GT_OK)
    {
ret_fail_2:
        osMutexDelete(q->mtx);
        goto ret_fail_1;
    }
    osSprintf(tmpName,"mQ-T-%s",name);
    if (osSemBinCreate(tmpName, OS_SEMB_EMPTY, &(q->txSem)) != GT_OK)
    {
        osMutexDelete(q->rxSem);
        goto ret_fail_2;
    }

    q->maxMsgs = maxMsgs;
    q->maxMsgSize = maxMsgSize;
    ReleaseMutex(messageQueues_mtx);

    *msgqId = (GT_MSGQ_ID)qnum;
    return GT_OK;
}

/*******************************************************************************
* osMsgQDelete
*
* DESCRIPTION:
*       Delete message queue
*
* INPUTS:
*       msgqId - message queue Id
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on error
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS osMsgQDelete
(
    IN GT_MSGQ_ID msgqId
)
{
    int timeOut;
    OS_MSGQ_STC *q;

    MSGQ_CHECK();

    osMutexLock(q->mtx);
    q->header.type = 2; /* deleting */
    osMutexUnlock(q->mtx);

    for (timeOut = 1000; q->waitRx && timeOut; timeOut--)
    {
        osSemSignal(q->rxSem);
        osTimerWkAfter(1);
    }
    for (timeOut = 1000; q->waitTx && timeOut; timeOut--)
    {
        osSemSignal(q->txSem);
        osTimerWkAfter(1);
    }
    osFree(q->buffer);
    osMutexDelete(q->mtx);
    osSemDelete(q->rxSem);
    osSemDelete(q->txSem);

    q->header.type = 0;

    return GT_OK;
}

/*******************************************************************************
* osMsgQSend
*
* DESCRIPTION:
*       Send message to queue
*
* INPUTS:
*       msgqId       - Message queue Idsemaphore Id
*       message      - message data pointer
*       messageSize  - message size
*       timeOut      - time out in miliseconds or
*                      OS_MSGQ_WAIT_FOREVER or OS_MSGQ_NO_WAIT
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK        - on success
*       GT_FAIL      - on error
*       GT_TIMEOUT   - on time out
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS osMsgQSend
(
    IN GT_MSGQ_ID   msgqId,
    IN GT_PTR       message,
    IN GT_U32       messageSize,
    IN GT_U32       timeOut
)
{
    GT_STATUS ret;
    char    *msg;
    OS_MSGQ_STC *q;

    MSGQ_CHECK();

    osMutexLock(q->mtx);
    while (q->messages == q->maxMsgs)
    {
        /* queue full */
        if (timeOut == OS_MSGQ_NO_WAIT)
        {
            osMutexUnlock(q->mtx);
            return GT_FAIL; /* ??? GT_TIMEOUT */
        }
        q->waitTx++;
        osMutexUnlock(q->mtx);

        ret = osSemWait(q->txSem, timeOut);
        if (ret != GT_OK)
            return ret;

        osMutexLock(q->mtx);
        q->waitTx--;
        if (q->header.type != 1)
        {
            /* deleting */
            osMutexUnlock(q->mtx);
            return GT_FAIL;
        }
    }

    /* put message */
    msg = q->buffer + q->head * (q->maxMsgSize + sizeof(GT_U32));
    if (messageSize > (GT_U32)(q->maxMsgSize))
        messageSize = q->maxMsgSize;

    *((GT_U32*)msg) = messageSize;
    osMemCpy(msg+sizeof(GT_U32), message, messageSize);
    q->head++;
    if (q->head >= q->maxMsgs) /* round up */
        q->head = 0;
    q->messages++;

    /* signal to Recv thread if any */
    if (q->waitRx)
    {
        osSemSignal(q->rxSem);
    }

    osMutexUnlock(q->mtx);
    return GT_OK;
}

/*******************************************************************************
* osMsgQRecv
*
* DESCRIPTION:
*       Receive message from queuee
*
* INPUTS:
*       msgqId       - Message queue Idsemaphore Id
*       messageSize  - size of buffer pointed by message
*       timeOut      - time out in miliseconds or
*                      OS_MSGQ_WAIT_FOREVER or OS_MSGQ_NO_WAIT
*
* OUTPUTS:
*       message      - message data pointer
*       messageSize  - actual message size
*
* RETURNS:
*       GT_OK        - on success
*       GT_FAIL      - on error
*       GT_TIMEOUT   - on time out
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS osMsgQRecv
(
    IN    GT_MSGQ_ID   msgqId,
    OUT   GT_PTR       message,
    INOUT GT_U32       *messageSize,
    IN    GT_U32       timeOut
)
{
    GT_STATUS ret;
    char    *msg;
    GT_U32  msgSize;
    OS_MSGQ_STC *q;

    MSGQ_CHECK();

    osMutexLock(q->mtx);
    while (q->messages == 0)
    {
        /* queue empty */
        if (timeOut == OS_MSGQ_NO_WAIT)
        {
            osMutexUnlock(q->mtx);
            return GT_FAIL; /* ??? GT_TIMEOUT */
        }
        q->waitRx++;
        osMutexUnlock(q->mtx);

        ret = osSemWait(q->rxSem, timeOut);
        if (ret != GT_OK)
            return ret;

        osMutexLock(q->mtx);
        q->waitRx--;
        if (q->header.type != 1)
        {
            /* deleting */
            osMutexUnlock(q->mtx);
            return GT_FAIL;
        }
    }
    /* get message */
    msg = q->buffer + q->tail * (q->maxMsgSize + sizeof(GT_U32));
    msgSize = *((GT_U32*)msg);
    if (msgSize > *messageSize)
        msgSize = *messageSize;

    osMemCpy(message, msg+sizeof(GT_U32), msgSize);
    *messageSize = msgSize;
    q->tail++;
    if (q->tail >= q->maxMsgs) /* round up */
        q->tail = 0;
    q->messages--;

    /* signal to Recv thread if any */
    if (q->waitTx)
    {
        osSemSignal(q->txSem);
    }

    osMutexUnlock(q->mtx);
    return GT_OK;
}

/*******************************************************************************
* osMsgQNumMsgs
*
* DESCRIPTION:
*       Return number of messages pending in queue
*
* INPUTS:
*       msgqId       - Message queue Idsemaphore Id
*
* OUTPUTS:
*       numMessages  - number of messages pending in queue
*
* RETURNS:
*       GT_OK        - on success
*       GT_FAIL      - on error
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS osMsgQNumMsgs
(
    IN    GT_MSGQ_ID   msgqId,
    OUT   GT_U32       *numMessages
)
{
    OS_MSGQ_STC *q;

    MSGQ_CHECK();

    osMutexLock(q->mtx);
    *numMessages = q->messages;
    osMutexUnlock(q->mtx);

    return GT_OK;
}
