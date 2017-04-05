/*******************************************************************************
*              (c), Copyright 2007, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* IOStreamSerial.c
*
* DESCRIPTION:
*       This file contains IOStream for UNIX terminal (console)
*
* FILE REVISION NUMBER:
*       $Revision: 5 $
*
*******************************************************************************/
#include <sys/select.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <stdio.h>
#include <termios.h>
#include <sys/ttydefaults.h>
#include <sys/time.h>
#include <string.h>
#include <errno.h>

#include <extUtils/IOStream/IOStream.h>

#ifndef  XTABS
#    define XTABS            0
#endif
#ifndef  ECHOKE
#    define ECHOKE           0
#endif
#ifndef  ECHOCTL
#    define ECHOCTL          0
#endif

#define READLINE_COMMON

typedef struct {
    IOStreamSTC common;
    int fd;
} serialStreamSTC, *serialStreamPTR;

static int serialStreamDestroy(IOStreamPTR streamP)
{
    serialStreamPTR stream = (serialStreamPTR)streamP;

    if (!stream)
        return -1;

    close(stream->fd);

    free(stream);

    return 0;
}
static int serialStreamRead(IOStreamPTR streamP, void* bufferPtr, int bufferLength)
{
    serialStreamPTR stream = (serialStreamPTR)streamP;

    if (!stream)
        return -1;

    /* set input mode */
    return read(stream->fd, bufferPtr, bufferLength);
}
static int serialStreamReadChar(IOStreamPTR streamP, char* charPtr, int timeOut)
{
    serialStreamPTR stream = (serialStreamPTR)streamP;
    fd_set rfds;
    int k;
    struct timeval tmo;

    if (!stream)
        return -1;

    FD_ZERO(&rfds);
    FD_SET(stream->fd, &rfds);
    tmo.tv_sec = timeOut / 1000;
    tmo.tv_usec = (timeOut % 1000 ) * 1000;
    k = select(stream->fd + 1, &rfds, NULL, NULL,
            (timeOut < 0) ? NULL : &tmo);

    if (k <= 0)
        return k;
    if (!FD_ISSET(stream->fd, &rfds))
        return -1; /* unexpected error */

    /* set input mode */
    return read(stream->fd, charPtr, 1);
}
#ifndef READLINE_COMMON
static int serialStreamReadLine(IOStreamPTR streamP, char* bufferPtr, int bufferLength)
{
    serialStreamPTR stream = (serialStreamPTR)streamP;
    int k;

    if (!stream)
        return -1;

    /* set input mode */

    k = read(stream->fd, bufferPtr, bufferLength);
    if (k <= 0)
        return k;
    if (bufferPtr[k-1] == '\n')
        bufferPtr[--k] = 0;

    return k;
}
#endif
static int serialStreamWrite(IOStreamPTR streamP, const void* bufferPtr, int bufferLength)
{
    serialStreamPTR stream = (serialStreamPTR)streamP;

    if (!stream)
        return -1;

    return write(stream->fd, bufferPtr, bufferLength);
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

    dup2(stream->fd, 1);
    dup2(stream->fd, 2);

    return 1;
}

static int serialSetTtyMode(IOStreamPTR streamP, int mode)
{
    serialStreamPTR stream = (serialStreamPTR)streamP;
    struct termios settings;

    if (!stream)
        return -1;


    tcgetattr(stream->fd, &settings);

    if (mode == 0)
    {
        /* raw mode */

        /* keep interrupt handling */
        settings.c_iflag &= ~(/* BRKINT | */ INLCR | ICRNL | IXON);
        settings.c_oflag &= ~OPOST;
        settings.c_lflag &= ~(ECHO | ECHONL | ICANON | /* ISIG | */ IEXTEN);
    }
    else
    {
        /* line edit mode */
        settings.c_iflag |= BRKINT | ICRNL | /*INLCR | */ IXON;
        settings.c_oflag |= OPOST;
        settings.c_lflag |= ECHO | /* ECHONL | */ ICANON | ISIG | IEXTEN;
    }
    tcsetattr(stream->fd, TCSANOW, &settings);
    return 0;
}

static int serialGetFd(IOStreamPTR streamP)
{
    serialStreamPTR stream = (serialStreamPTR)streamP;

    if (!stream)
        return -1;

    return stream->fd;
}

/*******************************************************************************
* IOStreamCreateSerial
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
IOStreamPTR IOStreamCreateTTY(void)
{
    serialStreamPTR stream;
    int fd;
    struct termios settings;
    int result;

    fd = open("/dev/tty", O_RDWR, 0);
    if (fd < 0)
    {
        perror("failed to open /dev/tty");
        return NULL;
    }

    /* Configure the stream to replace each LF char with a LF-CR couple.    */
    result = tcgetattr(fd, &settings);
    if (result < 0)
    {
        close(fd);
        return NULL;
    }

    /* settings.c_iflag |= INLCR;*/
    settings.c_oflag |= ONLCR;

#if 0
    result = tcsetattr(fd, TCSANOW, &settings);
#endif
    result = tcsetattr(fd, TCSAFLUSH, &settings);
    if (result < 0)
    {
        close(fd);
        return NULL;
    }

    stream = (serialStreamPTR)malloc(sizeof(*stream));
    if (!stream)
    {
        close(fd);
        return NULL;
    }
    memset(stream, 0, sizeof(*stream));
    stream->common.destroy = serialStreamDestroy;
    stream->common.read = serialStreamRead;
    stream->common.readChar = serialStreamReadChar;
#ifdef  READLINE_COMMON
    stream->common.readLine = IOStreamGenericReadLine;
#else
    stream->common.readLine = serialStreamReadLine;
#endif
    stream->common.write = serialStreamWrite;
    stream->common.writeBuf = IOStreamGenericWriteBuf;
    stream->common.writeLine = IOStreamGenericWriteLine;
    stream->common.connected = serialStreamConnected;
    stream->common.grabStd = serialGrabStd;
    stream->common.setTtyMode = serialSetTtyMode;
    stream->common.getFd = serialGetFd;
    stream->common.isConsole = 1;
    stream->common.flags = 0;
    stream->fd = fd;
    /* force lineedit mode */
    serialSetTtyMode((IOStreamPTR)stream, 1);

    return (IOStreamPTR)stream;
}
