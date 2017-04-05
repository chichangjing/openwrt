/*******************************************************************************
*              (c), Copyright 2007, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* cmdStream.c
*
* DESCRIPTION:
*       This file contains common functions for mainCmd I/O streams
*
* FILE REVISION NUMBER:
*       $Revision: 9 $
*
*******************************************************************************/
#include <cmdShell/os/cmdStreamImpl.h>

/*******************************************************************************
* cmdStreamGrabSystemOutput
*
* DESCRIPTION:
*       cmdStreamRedirectStdout() will grab stdout and stderr streams
*       if this flags set to GT_TRUE
*
*******************************************************************************/
GT_BOOL cmdStreamGrabSystemOutput = GT_TRUE;

/*******************************************************************************
* cmdStreamRedirectStdout
*
* DESCRIPTION:
*       Redirect stdout/stderr to given stream
*
* INPUTS:
*       stream - destination stream
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK
*       GT_FAIL
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS cmdStreamRedirectStdout
(
    IN IOStreamPTR stream
)
{
    if (!stream)
    {
        /* dup2... */
        return cmdOsBindStdOut(NULL, NULL);
    }
    if (cmdStreamGrabSystemOutput == GT_FALSE)
        return GT_OK;
    if (stream->grabStd)
        stream->grabStd(stream);
    return cmdOsBindStdOut((CPSS_OS_BIND_STDOUT_FUNC_PTR)stream->writeBuf, stream);
}

/************* NULL stream implementation *************************************/


/*******************************************************************************
* cmdStreamStartEventLoopHandler
*
* DESCRIPTION:
*       Create new task to run stream handler
*
* INPUTS:
*       taskName   - task name for handler
*       handler    - pointer to handler function
*       stream     - stream
*       isConsole  - application flag
*
* OUTPUTS:
*       taskId     - pointer for task ID
*
* RETURNS:
*       GT_OK
*       GT_FAIL
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS cmdStreamStartEventLoopHandler
(
    IN  char *taskName,
    IN  eventLoopHandlerPTR handler,
    IN  IOStreamPTR stream,
    IN  GT_BOOL isConsole,
    OUT GT_TASK *taskId
)
{
    if (!stream)
        return GT_FAIL;

    stream->isConsole = isConsole;

    return cmdOsTaskCreate(
                taskName,               /* thread name              */
                STREAM_THREAD_PRIO+1,   /* thread priority          */
                65536,                  /* use default stack size   */
                (unsigned (__TASKCONV *)(void*))handler,
                stream,
                taskId);
}
