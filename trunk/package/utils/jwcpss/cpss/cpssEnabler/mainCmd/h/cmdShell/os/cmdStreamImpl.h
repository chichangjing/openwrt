/*******************************************************************************
*              (c), Copyright 2007, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* cmdStreamImpl.h
*
* DESCRIPTION:
*       This file contains types and routines for mainCmd I/O streams
*
* FILE REVISION NUMBER:
*       $Revision: 2 $
*
*******************************************************************************/
#ifndef __cmdStreamImplh
#define __cmdStreamImplh

#include <cmdShell/common/cmdCommon.h>
#include <extUtils/IOStream/IOStream.h>
#include <extUtils/IOStream/IOStreamTCP.h>

#define STREAM_THREAD_PRIO 5

/*******************************************************************************
* cmdStreamGrabSystemOutput
*
* DESCRIPTION:
*       cmdStreamRedirectStdout() will grab stdout and stderr streams
*       if this flags set to GT_TRUE
*
*******************************************************************************/
extern GT_BOOL cmdStreamGrabSystemOutput;

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
);

#if 0

/*******************************************************************************
* cmdStreamCreateNULL
*
* DESCRIPTION:
*       Create null stream. This stream equals to /dev/null
*
* INPUTS:
*       None
*
* OUTPUTS:
*       None
*
* RETURNS:
*       stream - null stream
*
* COMMENTS:
*       None
*
*******************************************************************************/
IOStreamPTR cmdStreamCreateNULL(void);
#endif

/*******************************************************************************
* cmdStreamSerialInit
*
* DESCRIPTION:
*       Initialize serial engine
*
* INPUTS:
*       None
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
GT_STATUS cmdStreamSerialInit(void);

/*******************************************************************************
* cmdStreamSerialFinish
*
* DESCRIPTION:
*       Close serial engine
*
* INPUTS:
*       None
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
GT_STATUS cmdStreamSerialFinish(void);

/*******************************************************************************
* cmdStreamCreateSerial
*
* DESCRIPTION:
*       Create serial port stream
*
* INPUTS:
*       devNum  - the serial device port number (0 = COM1, ...)
*
* OUTPUTS:
*       None
*
* RETURNS:
*       stream - serial stream
*       NULL if error
*
* COMMENTS:
*       None
*
*******************************************************************************/
IOStreamPTR cmdStreamCreateSerial
(
    IN GT_U32 devNum
);

/*******************************************************************************
* cmdStreamSocketInit
*
* DESCRIPTION:
*       Initialize TCP/IP socket engine
*
* INPUTS:
*       None
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
GT_STATUS cmdStreamSocketInit(void);

/*******************************************************************************
* cmdStreamSocketFinish
*
* DESCRIPTION:
*       Close socket engine
*
* INPUTS:
*       None
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
GT_STATUS cmdStreamSocketFinish(void);

#if 0
/*******************************************************************************
* cmdStreamCreateSocket
*
* DESCRIPTION:
*       Create socket stream
*
* INPUTS:
*       socket - socket descriptor
*
* OUTPUTS:
*       None
*
* RETURNS:
*       stream - socket stream
*       NULL if error
*
* COMMENTS:
*       None
*
*******************************************************************************/
IOStreamPTR cmdStreamCreateSocket
(
    IN GT_SOCKET_FD socket
);

/*******************************************************************************
* cmdStreamCreateTelnet
*
* DESCRIPTION:
*       Create Telnet protocol stream
*
* INPUTS:
*       socket - socket I/O stream
*
* OUTPUTS:
*       None
*
* RETURNS:
*       stream - socket stream
*       NULL if error
*
* COMMENTS:
*       None
*
*******************************************************************************/
IOStreamPTR cmdStreamCreateTelnet
(
    IN IOStreamPTR socket
);

#endif

#if defined(_linux) || defined(__FreeBSD__)
/*******************************************************************************
* cmdStreamCreatePipe
*
* DESCRIPTION:
*       Create pipe stream
*
* INPUTS:
*       None
*
* OUTPUTS:
*       None
*
* RETURNS:
*       stream - pipe stream
*       NULL if error
*
* COMMENTS:
*       Input and output file descriptors must be specified in environment
*       variables PIPE_STREAM_INFD and PIPE_STREAM_OUTFD
*
*******************************************************************************/
IOStreamPTR cmdStreamCreatePipe(void);
#endif /* defined(_linux) || defined(__FreeBSD__) */


/*
 * Typedef: eventLoopHandlerPTR
 *
 * Description:
 *      Pointer to application stream handler
 */
typedef GT_STATUS (*eventLoopHandlerPTR)(
        IOStreamPTR stream
);

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
);

#if 0
/*******************************************************************************
* cmdStreamSocketCreateListenerTask
*
* DESCRIPTION:
*       Create socket listenet task
*       This task will accept incoming socket connections and then start
*       handler for each connection
*
* INPUTS:
*       listenerTaskName  - name for listener task
*       ip                - ip address to listen on
*                           NULL means listen on all interfaces
*       port                tcp port for incoming connections
*       streamTaskName    - task name for handler
*       handler           - pointer to handler function
*       multipleInstances - Allow more than one handler at time
*       isConsole         - application flag
*
* OUTPUTS:
*       taskId     - pointer for listener task ID
*
* RETURNS:
*       GT_OK
*       GT_FAIL
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS cmdStreamSocketCreateListenerTask(
    IN  char *listenerTaskName,
    IN  char *ip,
    IN  int port,
    IN  char *streamTaskName,
    IN  eventLoopHandlerPTR handler,
    IN  GT_BOOL multipleInstances,
    IN  GT_BOOL isConsole,
    OUT GT_TASK *taskId
);
#endif

/*******************************************************************************
* cmdStreamSocketServiceCreate
*
* DESCRIPTION:
*       Create socket listener service
*
* INPUTS:
*       serviceName       - service name
*       ip                - ip address to listen on
*                           NULL means listen on all interfaces
*       port                tcp port for incoming connections
*       handler           - pointer to handler function
*       multipleInstances - Allow more than one handler at time
*       isConsole         - application flag
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
GT_STATUS cmdStreamSocketServiceCreate(
    IN  const char *serviceName,
    IN  char *ip,
    IN  GT_U16 port,
    IN  eventLoopHandlerPTR handler,
    IN  GT_BOOL multipleInstances,
    IN  GT_BOOL isConsole
);

/*******************************************************************************
* cmdStreamSocketServiceStart
*
* DESCRIPTION:
*       Start socket service
*
* INPUTS:
*       serviceName
*
* OUTPUTS:
*       None
*
* RETURNS:
*       status
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS cmdStreamSocketServiceStart(
    IN  const char *serviceName
);

/*******************************************************************************
* cmdStreamSocketServiceStop
*
* DESCRIPTION:
*       Stop socket service
*
* INPUTS:
*       serviceName
*
* OUTPUTS:
*       None
*
* RETURNS:
*       status
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS cmdStreamSocketServiceStop(
    IN  const char *serviceName
);

/*******************************************************************************
* cmdStreamSocketServiceIsRunning
*
* DESCRIPTION:
*       Return service running status
*
* INPUTS:
*       serviceName
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_TRUE or GT_FALSE
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_BOOL cmdStreamSocketServiceIsRunning(
    IN  const char *serviceName
);

/*******************************************************************************
* cmdStreamSocketServiceGetPort
*
* DESCRIPTION:
*       Get service port number
*
* INPUTS:
*       serviceName
*
* OUTPUTS:
*       portNumberPtr
*
* RETURNS:
*       status
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS cmdStreamSocketServiceGetPort(
    IN  const char *serviceName,
    OUT GT_U16     *portNumberPtr
);

/*******************************************************************************
* cmdStreamSocketServiceSetPort
*
* DESCRIPTION:
*       Get service port number
*
* INPUTS:
*       serviceName
*       portNumber
*
* OUTPUTS:
*
* RETURNS:
*       status
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS cmdStreamSocketServiceSetPort(
    IN  const char *serviceName,
    IN  GT_U16     portNumber
);


#endif /* __cmdStreamImplh */
