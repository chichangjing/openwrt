/*******************************************************************************
* osLinuxMsgQ.c
*
* DESCRIPTION:
*       Linux User Mode Operating System wrapper. Message queues
*
* DEPENDENCIES:
*       Linux, CPU independed , and posix threads implementation.
*
* FILE REVISION NUMBER:
*       $Revision: 5 $
*******************************************************************************/

#define _GNU_SOURCE
#include "kernelExt.h"
#include <gtOs/gtOsMsgQ.h>
#include <gtOs/gtOsIo.h>


#include <string.h>
#include <stdlib.h>
#include <errno.h>

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
    int ret;
    mv_msgq_create_stc rparam;

    if (name)
        strncpy(rparam.name, name, sizeof(rparam.name));
    else
        rparam.name[0] = 0;
    rparam.maxMsgs = maxMsgs;
    rparam.maxMsgSize = maxMsgSize;

    ret = mv_ctrl(MVKERNELEXT_IOC_MSGQCREATE, &rparam);
    if (ret < 0)
    {
        *msgqId = 0;
        return GT_FAIL;
    }

    *msgqId = (GT_MSGQ_ID)ret;

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
    int rc;

    rc = mv_ctrl(MVKERNELEXT_IOC_MSGQDELETE, msgqId);

    if (rc != 0)
        return GT_FAIL;

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
    int ret;
    mv_msgq_sr_stc params;
    
    params.msgqId = msgqId;
    params.message = message;
    params.messageSize = messageSize;
    if (timeOut == OS_MSGQ_NO_WAIT)
    {
        params.timeOut = 0;
    }
    else if (timeOut == OS_MSGQ_WAIT_FOREVER)
    {
        params.timeOut = -1;
    }
    else
    {
        params.timeOut = timeOut;
    }

    do {
        ret = mv_ctrl(MVKERNELEXT_IOC_MSGQSEND, &params);
    } while (ret < 0 && errno == MVKERNELEXT_EINTR);

    if (ret < 0 && errno == MVKERNELEXT_ETIMEOUT)
        return GT_TIMEOUT;

    if (ret < 0)
        return GT_FAIL;
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
    int ret;
    mv_msgq_sr_stc params;
    
    params.msgqId = msgqId;
    params.message = message;
    params.messageSize = *messageSize;
    if (timeOut == OS_MSGQ_NO_WAIT)
    {
        params.timeOut = 0;
    }
    else if (timeOut == OS_MSGQ_WAIT_FOREVER)
    {
        params.timeOut = -1;
    }
    else
    {
        params.timeOut = timeOut;
    }

    do {
        ret = mv_ctrl(MVKERNELEXT_IOC_MSGQRECV, &params);
    } while (ret < 0 && errno == MVKERNELEXT_EINTR);

    if (ret < 0 && errno == MVKERNELEXT_ETIMEOUT)
        return GT_TIMEOUT;

    if (ret < 0)
        return GT_FAIL;

    *messageSize = (GT_U32)ret;
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
    int ret;

    ret = mv_ctrl(MVKERNELEXT_IOC_MSGQNUMMSGS, msgqId);

    if (ret < 0)
        return GT_FAIL;

    *numMessages = (GT_U32)ret;
    return GT_OK;
}
