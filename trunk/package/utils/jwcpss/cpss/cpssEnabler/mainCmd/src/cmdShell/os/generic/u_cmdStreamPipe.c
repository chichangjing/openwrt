/*******************************************************************************
*              (c), Copyright 2007, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* u_cmdStreamPipe.c
*
* DESCRIPTION:
*       This file contains mainCmd pipe stream implementation for UNIX OSes
*
* FILE REVISION NUMBER:
*       $Revision: 3 $
*
*******************************************************************************/
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>

#include <cmdShell/os/cmdStreamImpl.h>


typedef struct {
    IOStreamSTC common;
    int inFd;
    int outFd;
} pipeStreamSTC, *pipeStreamPTR;

static int pipeStreamDestroy(IOStreamPTR streamP)
{
    pipeStreamPTR stream = (pipeStreamPTR)streamP;

    if (!stream)
        return -1;

    close(stream->inFd);
    close(stream->outFd);

    cmdOsFree(stream);

    return 0;
}
static int pipeStreamRead(IOStreamPTR streamP, void* bufferPtr, int bufferLength)
{
    pipeStreamPTR stream = (pipeStreamPTR)streamP;

    if (!stream)
        return -1;

    /* set input mode */
    return read(stream->inFd, bufferPtr, bufferLength);
}
static int pipeStreamWrite(IOStreamPTR streamP,const void* bufferPtr, int bufferLength)
{
    pipeStreamPTR stream = (pipeStreamPTR)streamP;

    if (!stream)
        return -1;

    return write(stream->outFd, bufferPtr, bufferLength);
}
static int pipeStreamConnected(IOStreamPTR streamP)
{
    pipeStreamPTR stream = (pipeStreamPTR)streamP;

    if (!stream)
        return -1;

    return 1;
}

static int pipeGrabStd(IOStreamPTR streamP)
{
    pipeStreamPTR stream = (pipeStreamPTR)streamP;

    if (!stream)
        return -1;

    dup2(stream->outFd, 1);
    dup2(stream->outFd, 2);

    return 1;
}

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
IOStreamPTR cmdStreamCreatePipe(void)
{
    pipeStreamPTR stream;
    int inFd;
    int outFd;
    char *env;

    /* read environment variables */
    if ((env = getenv("PIPE_STREAM_INFD")) == NULL)
    {
        return NULL;
    }
    inFd = atoi(env);
    if ((env = getenv("PIPE_STREAM_OUTFD")) == NULL)
    {
        return NULL;
    }
    outFd = atoi(env);

    stream = (pipeStreamPTR)cmdOsMalloc(sizeof(*stream));
    if (!stream)
    {
        return NULL;
    }
    memset(stream, 0, sizeof(*stream));
    stream->common.destroy = pipeStreamDestroy;
    stream->common.read = pipeStreamRead;
    stream->common.readLine = IOStreamGenericReadLine;
    stream->common.write = pipeStreamWrite;
    stream->common.writeBuf = IOStreamGenericWriteBuf;
    stream->common.writeLine = IOStreamGenericWriteLine;
    stream->common.connected = pipeStreamConnected;
    stream->common.grabStd = pipeGrabStd;
    stream->common.isConsole = GT_FALSE;
    stream->common.flags = 0;
    stream->inFd = inFd;
    stream->outFd = outFd;

    return (IOStreamPTR)stream;
}
