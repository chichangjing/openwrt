/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* cpssOsMsgQ.h
*
* DESCRIPTION:
*       Operating System wrapper. Message queues
*
* FILE REVISION NUMBER:
*       $Revision: 2 $
*******************************************************************************/

#ifndef __cpssOsMsgQh
#define __cpssOsMsgQh

#ifdef __cplusplus
extern "C" {
#endif

/************* Includes *******************************************************/

#include <cpss/extServices/os/gtOs/gtGenTypes.h>

/************* Defines ********************************************************/
#define CPSS_OS_MSGQ_WAIT_FOREVER 0
#define CPSS_OS_MSGQ_NO_WAIT      0xffffffff

/************* Typedefs *******************************************************/
typedef GT_UINTPTR CPSS_OS_MSGQ_ID;


/************* Functions ******************************************************/

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
typedef GT_STATUS (*CPSS_OS_MSGQ_CREATE_FUNC)
(
    IN  const char      *name,
    IN  GT_U32          maxMsgs,
    IN  GT_U32          maxMsgSize,
    OUT CPSS_OS_MSGQ_ID *msgqId
);

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
typedef GT_STATUS (*CPSS_OS_MSGQ_DELETE_FUNC)
(
    IN CPSS_OS_MSGQ_ID msgqId
);

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
typedef GT_STATUS (*CPSS_OS_MSGQ_SEND_FUNC)
(
    IN CPSS_OS_MSGQ_ID  msgqId,
    IN GT_PTR           message,
    IN GT_U32           messageSize,
    IN GT_U32           timeOut
);

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
typedef GT_STATUS (*CPSS_OS_MSGQ_RECV_FUNC)
(
    IN    CPSS_OS_MSGQ_ID   msgqId,
    OUT   GT_PTR            message,
    INOUT GT_U32            *messageSize,
    IN    GT_U32            timeOut
);

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
typedef GT_STATUS (*CPSS_OS_MSGQ_NUM_MSGS_FUNC)
(
    IN    CPSS_OS_MSGQ_ID   msgqId,
    OUT   GT_U32            *numMessages
);



/* CPSS_OS_MSGQ_BIND_STC -
    structure that hold the "os msgq" functions needed be bound to cpss.

*/
typedef struct{
    CPSS_OS_MSGQ_CREATE_FUNC    osMsgQCreateFunc;
    CPSS_OS_MSGQ_DELETE_FUNC    osMsgQDeleteFunc;
    CPSS_OS_MSGQ_SEND_FUNC      osMsgQSendFunc;
    CPSS_OS_MSGQ_RECV_FUNC      osMsgQRecvFunc;
    CPSS_OS_MSGQ_NUM_MSGS_FUNC  osMsgQNumMsgsFunc;
}CPSS_OS_MSGQ_BIND_STC;

#ifdef __cplusplus
}
#endif

#endif  /* __cpssOsMsgQh */
/* Do Not Add Anything Below This Line */

