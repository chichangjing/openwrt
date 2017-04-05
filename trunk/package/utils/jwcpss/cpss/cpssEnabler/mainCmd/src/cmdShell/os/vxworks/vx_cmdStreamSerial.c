/*******************************************************************************
*              (c), Copyright 2007, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* vx_IOStreamSerial.c
*
* DESCRIPTION:
*       This file contains mainCmd serial stream implementation for vxWorks
*
* FILE REVISION NUMBER:
*       $Revision: 8 $
*
*******************************************************************************/
#include <ioLib.h>
#include <sioLib.h>
#include <ttyLib.h>
#include <sysLib.h>
#include <selectLib.h>
#include <string.h>

#include <cmdShell/os/cmdStreamImpl.h>

#define DEVICE_SER      "/tyCo/serial%lu"

typedef struct {
    IOStreamSTC common;
    int handle;
} serialStreamSTC, *serialStreamPTR;

static int serialStreamDestroy(IOStreamPTR streamP)
{
    serialStreamPTR stream = (serialStreamPTR)streamP;

    if (!stream)
        return -1;

    close(stream->handle);

    cmdOsFree(stream);

    return 0;
}
static int serialStreamRead(IOStreamPTR streamP, void* bufferPtr, int bufferLength)
{
    serialStreamPTR stream = (serialStreamPTR)streamP;
    fd_set rfds;
    int ret;

    if (!stream)
        return -1;

    /* wait for input */
    FD_ZERO(&rfds);
    FD_SET(stream->handle, &rfds);
    ret = select(stream->handle+1, &rfds, NULL, NULL, NULL);
    if (ret <= 0)
        return ret;

    if (!FD_ISSET(stream->handle, &rfds))
        return 0;

    /* set input mode */
    return read(stream->handle, bufferPtr, bufferLength);
}

static int serialStreamReadChar(IOStreamPTR streamP, char* charPtr, int timeOut)
{
    serialStreamPTR stream = (serialStreamPTR)streamP;
    fd_set rfds;
    struct timeval tmo;
    int ret;

    if (!stream)
        return -1;

    /* wait for input */
    FD_ZERO(&rfds);
    FD_SET(stream->handle, &rfds);
    tmo.tv_sec = timeOut / 1000;
    tmo.tv_usec = timeOut % 1000;
    ret = select(stream->handle+1, &rfds, NULL, NULL,
            (timeOut < 0) ? NULL : &tmo);
    if (ret <= 0)
        return ret;

    if (!FD_ISSET(stream->handle, &rfds))
        return -1;

    /* set input mode */
    return read(stream->handle, charPtr, 1);
}

static int serialStreamWrite(IOStreamPTR streamP, const void* bufferPtr, int bufferLength)
{
    serialStreamPTR stream = (serialStreamPTR)streamP;

    if (!stream)
        return -1;

    return write(stream->handle, (void*)bufferPtr, bufferLength);
}
static int serialStreamConnected(IOStreamPTR streamP)
{
    serialStreamPTR stream = (serialStreamPTR)streamP;

    if (!stream)
        return -1;

    return 1;
}

static int serialGrabStd(IOStreamPTR streamP)
{
    serialStreamPTR stream = (serialStreamPTR)streamP;

    if (!stream)
        return -1;

    /* ioGlobalStdSet(STD_IN, stream->handle); */
    ioGlobalStdSet(STD_OUT, stream->handle);
    ioGlobalStdSet(STD_ERR, stream->handle);

    return 1;
}

static int serialSetTtyMode(IOStreamPTR streamP, int mode)
{
    serialStreamPTR stream = (serialStreamPTR)streamP;

    if (mode == 0)
    {
        /* raw mode */
        stream->common.flags &= ~(
                STREAM_FLAG_I_CANON |
                STREAM_FLAG_I_ECHO
            );
    }
    else
    {
        /* line edit mode */
        stream->common.flags |= 
            STREAM_FLAG_I_CANON |
            STREAM_FLAG_I_ECHO;
    }
    return 0;
}

static int serialStreamGetFd(IOStreamPTR streamP)
{
    serialStreamPTR stream = (serialStreamPTR)streamP;

    if (!stream)
        return -1;

    return stream->handle;
}
/*******************************************************************************
* IOStreamSerialInit
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
GT_STATUS IOStreamSerialInit(void)
{
    return GT_OK;
}

/*******************************************************************************
* IOStreamSerialFinish
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
GT_STATUS IOStreamSerialFinish(void)
{
    return GT_OK;
}

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
)
{
    int pHandle;
    SIO_CHAN *sio = sysSerialChanGet(devNum);
    GT_U8 deviceName[128];
    serialStreamPTR stream;

    if (sio == NULL)
        return NULL;

    /*  attempt to create serial device; this code fragment
        is kind of a vxworks hack and should be avoided;
        it is the only way to get control over single
        serial port where it is already assigned as
        default stdio console.
    */
    cmdOsSprintf(deviceName, DEVICE_SER, devNum);
    if (ttyDevCreate(deviceName, sio, 128, 128) != OK)
        return NULL;

    pHandle = open(deviceName, O_RDWR, 0);

    /* check for successful operation */
    if (pHandle < 0)
        return NULL;

    /* Configure the stream to replace each LF char with a LF-CR couple.    */
    ioctl(pHandle,FIOOPTIONS,OPT_CRMOD);

    stream = (serialStreamPTR)cmdOsMalloc(sizeof(*stream));
    if (!stream)
    {
        close(pHandle);
        return NULL;
    }
    memset(stream, 0, sizeof(*stream));
    stream->common.destroy = serialStreamDestroy;
    stream->common.read = serialStreamRead;
    stream->common.readChar = serialStreamReadChar;
    stream->common.readLine = streamGenericReadLine;
    stream->common.write = serialStreamWrite;
    stream->common.writeBuf = streamGenericWriteBuf;
    stream->common.writeLine = streamGenericWriteLine;
    stream->common.connected = serialStreamConnected;
    stream->common.grabStd = serialGrabStd;
    stream->common.setTtyMode = serialSetTtyMode;
    stream->common.getFd = serialStreamGetFd;
    stream->common.isConsole = GT_FALSE;
    stream->common.flags = STREAM_FLAG_I_CANON | STREAM_FLAG_I_ECHO;
    stream->handle = pHandle;

    return (IOStreamPTR)stream;
}
