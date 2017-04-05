/*******************************************************************************
*              (c), Copyright 2007, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* cmdConsole.h
*
* DESCRIPTION:
*       commander console api header
*
*
* DEPENDENCIES:
*       gtOs.h, cmdStream.h
*
* FILE REVISION NUMBER:
*       $Revision: 8 $
*******************************************************************************/

#ifndef __cmdConsole_h__
#define __cmdConsole_h__

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/***** Include files ***************************************************/
#include <cmdShell/common/cmdCommon.h>
#include <cmdShell/os/cmdStreamImpl.h>

/*******************************************************************************
* cmdTakeGlobalStdio
*
* DESCRIPTION:
*       Set the global stdio streams. No task can set unless the one took has
*       released it (Critical Section).
*
* INPUTS:
*       IOStream - the task io stream. This stream will be used as the global io
*       stream. Each task in the context of that task wiil use this io.

*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK   - on success
*
* COMMENTS:
*       Each console uses the stdio must set the global stdio stream in order to
*       redirect all other task outputs into the current stream
*
*******************************************************************************/
GT_STATUS cmdTakeGlobalStdio(IOStreamPTR IOStream);

/*******************************************************************************
* cmdReleaseGlobalStdio
*
* DESCRIPTION:
*       Release the global stdio for other taks used.
*
* INPUTS:
*       None.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK   - on success
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS cmdReleaseGlobalStdio(GT_VOID);

/*******************************************************************************
* cmdPrintf
*
* DESCRIPTION:
*       Write a formatted string to the current I/O stream.
*
* INPUTS:
*       format  - format string to write
*
* OUTPUTS:
*       None
*
* RETURNS:
*       The number of characters written, or a negative value if
*       an output error occurs.
*
* COMMENTS:
*       None
*
*******************************************************************************/
int cmdPrintf(const char* format, ...);

/***************************************************************************
* cmdGets
*
* DESCRIPTION:
*       Read line from current stream.
*       Returned line is zero terminated with stripped EOL symbols
*       Zero termination is added if there is a space in a buffer.
*       Otherwise returned value should be used as line length
*
* INPUTS:
*       bufferLength - buffer length
*
* OUTPUTS:
*       bufferPtr    - pointer to output buffer
*
* RETURNS:
*       < 0 if error occured
*       number of received bytes
*
***************************************************************************/
int cmdGets(char* bufferPtr, int bufferLen);

/*******************************************************************************
* cmdEventLoop
*
* DESCRIPTION:
*       command line shell pipeline; should be spawned as an independent thread
*
* INPUTS:
*       stream  - pointer to stream descriptor
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
GT_STATUS cmdEventLoop
(
    IOStreamPTR IOStream
);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __cmdConsole_h__ */

