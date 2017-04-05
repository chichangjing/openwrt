/*******************************************************************************
* osVxMsgQ.c
*
* DESCRIPTION:
*       vxWorks Operating System wrapper. Message queues
*
* DEPENDENCIES:
*       VxWorks, CPU independed.
*
* FILE REVISION NUMBER:
*       $Revision: 1 $
*******************************************************************************/

#include <vxWorks.h>
#include <objLib.h>
#include <msgQLib.h>
#include <sysLib.h>
#include <gtOs/gtOsMsgQ.h>

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
    MSG_Q_ID    id;

    id = msgQCreate(maxMsgs, maxMsgSize, MSG_Q_FIFO);

    if (!id)
        return GT_FAIL;

    if (msgqId)
        *msgqId = (GT_MSGQ_ID)id;

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
    STATUS rc;

    rc = msgQDelete((MSG_Q_ID)msgqId);

    return (rc == OK) ? GT_OK : GT_FAIL;
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
    int     tmo;
    STATUS  rc;

    if (timeOut == OS_MSGQ_NO_WAIT)
        tmo = NO_WAIT;
    else if (timeOut == OS_MSGQ_WAIT_FOREVER)
        tmo = WAIT_FOREVER;
    else
    {
        int num;

        num = sysClkRateGet();
        tmo = (num * timeOut) / 1000;
        if (tmo < 1)
            tmo = 1;
    }
    rc = msgQSend((MSG_Q_ID)msgqId,
            (char*)message, (UINT)messageSize,
            tmo, MSG_PRI_NORMAL);

    if (rc == OK)
        return GT_OK;

    if (errno == S_objLib_OBJ_TIMEOUT)
        return GT_TIMEOUT;
    return GT_FAIL;
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
    int     rc;
    int     tmo;

    if (timeOut == OS_MSGQ_NO_WAIT)
        tmo = NO_WAIT;
    else if (timeOut == OS_MSGQ_WAIT_FOREVER)
        tmo = WAIT_FOREVER;
    else
    {
        int num;

        num = sysClkRateGet();
        tmo = (num * timeOut) / 1000;
        if (tmo < 1)
            tmo = 1;
    }

    rc = msgQReceive((MSG_Q_ID)msgqId,
            (char*)message, (UINT)*messageSize,
            tmo);

    if (rc != ERROR)
    {
        *messageSize = (GT_U32)rc;
        return GT_OK;
    }

    if (errno == S_objLib_OBJ_TIMEOUT)
        return GT_TIMEOUT;
    return GT_FAIL;
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
    int rc;

    rc = msgQNumMsgs((MSG_Q_ID)msgqId);

    if (rc != ERROR)
    {
        if (numMessages)
            *numMessages = (GT_U32)rc;
        return GT_OK;
    }

    return GT_FAIL;
}
