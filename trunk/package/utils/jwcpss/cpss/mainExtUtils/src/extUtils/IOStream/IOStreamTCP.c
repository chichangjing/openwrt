/*******************************************************************************
*              (c), Copyright 2007, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* IOStreamSocket.c
*
* DESCRIPTION:
*       This file contains common functions for mainCmd socket streams
*
* FILE REVISION NUMBER:
*       $Revision: 6 $
*
*******************************************************************************/
#include <extUtils/IOStream/IOStreamTCP.h>
#include <cpss/extServices/private/prvCpssBindFunc.h>

/*********** generic socket stream implementation *****************************/
typedef struct {
    IOStreamSTC common;
    CPSS_SOCKET_FD socket;
    GT_BOOL isConnected;
} socketStreamSTC, *socketStreamPTR;

static int socketStreamDestroy(IOStreamPTR streamP)
{
    socketStreamPTR stream = (socketStreamPTR)streamP;

    if (!stream)
        return -1;

    cpssOsSocketTcpDestroy(stream->socket);

    cpssOsFree(stream);

    return 0;
}
static int socketStreamRead(IOStreamPTR streamP, void* bufferPtr, int bufferLength)
{
    socketStreamPTR stream = (socketStreamPTR)streamP;
    GT_SSIZE_T k;

    if (!stream)
        return -1;

    if (!stream->isConnected)
        return -1;

    /* set input mode */
    k = cpssOsSocketRecv(stream->socket, bufferPtr, bufferLength, GT_TRUE);
    if (k <= 0)
    {
        stream->isConnected = GT_FALSE;
    }

    return (int)k;
}

static int socketStreamReadChar(IOStreamPTR streamP, char* charPtr, int timeOut)
{
    socketStreamPTR stream = (socketStreamPTR)streamP;
    GT_VOID    *rdSet;
    GT_SSIZE_T  k;

    if (!stream)
        return -1;

    if (!stream->isConnected)
        return -1;

    rdSet = cpssOsSelectCreateSet();
    cpssOsSelectZeroSet(rdSet);
    cpssOsSelectAddFdToSet(rdSet, stream->socket);

    k = cpssOsSelect(stream->socket+1, rdSet, NULL, NULL,
            timeOut < 0 ? GT_INFINITE: (GT_U32)timeOut);

    if (k <= 0) /* error or timeout */
    {
        cpssOsSelectEraseSet(rdSet);
        return (int)k;
    }

    if (cpssOsSelectIsFdSet(rdSet, stream->socket) == GT_FALSE)
    {
        cpssOsSelectEraseSet(rdSet);
        return -1; /* unexpected error */
    }

    cpssOsSelectEraseSet(rdSet);
    /* set input mode */
    k = cpssOsSocketRecv(stream->socket, charPtr, 1, GT_TRUE);
    if (k <= 0)
    {
        stream->isConnected = GT_FALSE;
        return -1;
    }

    return (int)k;
}

static int socketStreamWrite(IOStreamPTR streamP, const void* bufferPtr, int bufferLength)
{
    socketStreamPTR stream = (socketStreamPTR)streamP;

    if (!stream)
        return -1;

    if (!stream->isConnected)
        return -1;

    return (int)cpssOsSocketSend(stream->socket, bufferPtr, bufferLength);
}
static int socketStreamConnected(IOStreamPTR streamP)
{
    socketStreamPTR stream = (socketStreamPTR)streamP;

    if (!stream)
        return -1;

    return stream->isConnected;
}

#ifdef UNIX
#include <unistd.h>
#endif
#ifdef WIN32
#include <windows.h>
#include <winsock.h>
#endif
static int socketGrabStd(IOStreamPTR streamP)
{
#if defined(UNIX) || defined(WIN32)
    socketStreamPTR stream = (socketStreamPTR)streamP;

    if (!stream)
        return -1;
#endif /* defined(UNIX) || defined(WIN32) */

#ifdef UNIX
    dup2(stream->socket, 1);
    dup2(stream->socket, 2);
#endif
#ifdef WIN32
    SetStdHandle(STD_OUTPUT_HANDLE, (HANDLE)stream->socket);
    SetStdHandle(STD_ERROR_HANDLE, (HANDLE)stream->socket);
#endif
    return 1;
}

static int socketSetTtyMode(IOStreamPTR streamP, int mode)
{
    socketStreamPTR stream = (socketStreamPTR)streamP;

    if (!stream)
        return -1;

    if (mode == 0)
    {
        /* raw mode */
        stream->common.flags &= ~(
                IO_STREAM_FLAG_O_CRLF |
                IO_STREAM_FLAG_I_CANON |
                IO_STREAM_FLAG_I_ECHO
            );
    }
    else
    {
        /* line edit mode */
        stream->common.flags |= 
            IO_STREAM_FLAG_O_CRLF |
            IO_STREAM_FLAG_I_CANON |
            IO_STREAM_FLAG_I_ECHO;
    }
    return 0;
}

static int socketGetFd(IOStreamPTR streamP)
{
    socketStreamPTR stream = (socketStreamPTR)streamP;

    if (!stream)
        return -1;

    return (int)stream->socket;
}

/*******************************************************************************
* IOStreamCreateSocket
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
IOStreamPTR IOStreamCreateSocket
(
    IN CPSS_SOCKET_FD socket
)
{
    socketStreamPTR stream;

    stream = (socketStreamPTR)cpssOsMalloc(sizeof(*stream));
    if (!stream)
    {
        return NULL;
    }

    cpssOsMemSet(stream, 0, sizeof(*stream));
    stream->common.destroy = socketStreamDestroy;
    stream->common.read = socketStreamRead;
    stream->common.readChar = socketStreamReadChar;
    stream->common.readLine = IOStreamGenericReadLine;
    stream->common.write = socketStreamWrite;
    stream->common.writeBuf = IOStreamGenericWriteBuf;
    stream->common.writeLine = IOStreamGenericWriteLine;
    stream->common.connected = socketStreamConnected;
    stream->common.grabStd = socketGrabStd;
    stream->common.setTtyMode = socketSetTtyMode;
    stream->common.getFd = socketGetFd;
    stream->common.isConsole = GT_FALSE;
    stream->common.flags = 
        IO_STREAM_FLAG_O_CRLF |
        IO_STREAM_FLAG_I_CANON |
        IO_STREAM_FLAG_I_ECHO;
    stream->socket = socket;
    stream->isConnected = GT_TRUE;

    cpssOsSocketSetBlock(socket);

    return (IOStreamPTR)stream;
}
