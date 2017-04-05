/*******************************************************************************
*              (c), Copyright 2007, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* u_cmdStreamSerial.c
*
* DESCRIPTION:
*       This file contains mainCmd serial stream implementation for UNIX OSes
*
* FILE REVISION NUMBER:
*       $Revision: 6 $
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

#include <cmdShell/os/cmdStreamImpl.h>

#ifdef  __FreeBSD__
    #if (OS_TARGET_RELEASE) == 4
        #define DEVICE_SER      "/dev/cuaa%lu"
    #else
    #ifdef _FreeBSD /* black mode */
        #define DEVICE_SER      "/dev/ttyu%lu"
    #else
        #define DEVICE_SER      "/dev/cuad%lu"
    #endif
    #endif
#else
#   define DEVICE_SER       "/dev/ttyS%lu"
#   ifdef UCLINUX
#       define DEVFS                "/dev/.devfsd"
#       define DEVFS_DEVICE_SER     "/dev/tts/%lu"
#   endif
#endif

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

#ifndef  __FreeBSD__
extern GT_BOOL osPrintSyncEnableFlag;
#endif

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

    cmdOsFree(stream);

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
    int ret;
    serialStreamPTR stream = (serialStreamPTR)streamP;

    if (!stream)
        return -1;

    ret= write(stream->fd, bufferPtr, bufferLength);
#ifndef  __FreeBSD__
    if (osPrintSyncEnableFlag == GT_TRUE)
    {
        tcdrain(stream->fd);
    }
#endif
    return ret;
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
        /*settings.c_oflag &= ~OPOST;*/
        settings.c_lflag &= ~(ECHO | ECHONL | ICANON | /* ISIG | */ IEXTEN);
    }
    else
    {
        /* line edit mode */
        settings.c_iflag |= BRKINT | ICRNL | /*INLCR | */ IXON;
        settings.c_oflag |= OPOST;
        settings.c_lflag |= ECHO | /* ECHONL | */ ICANON | ISIG | IEXTEN;
    }
    tcsetattr(stream->fd, TCSAFLUSH, &settings);
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
GT_STATUS cmdStreamSerialInit(void)
{
    return GT_OK;
}

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
GT_STATUS cmdStreamSerialFinish(void)
{
    return GT_OK;
}

#ifdef CPU_ARMADAXP
extern int usleep(__useconds_t __useconds);
#endif
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
    serialStreamPTR stream;
    int fd;
    struct termios settings;
    int result;
    char *pseudoTerm;

    /* check for valid function arguments */
    if (devNum > 6)
    {
        return NULL;
    }

#ifdef CPU_ARMADAXP
    /* There is some race condition between console and /dev/ttyS0 */
    usleep(10000); /* sleep 10ms */
#endif
    pseudoTerm = getenv("PSEUDO");
    if (0 != pseudoTerm)
    {
        fd = open(pseudoTerm, O_RDWR, 0);
        if (fd < 0)
        {
            fprintf(stdout, "failed to open %s: %s\n",
                    pseudoTerm, strerror(errno));
            return NULL;
        }
    }
    else
    {
        char deviceName[128];
#ifdef  UCLINUX
        /* Check DEVFS */
        if (access(DEVFS, F_OK) < 0)
        {
            /* No DEVFS */
            cmdOsSprintf(deviceName, DEVICE_SER, devNum);
        }
        else
        {
            /* DEVFS case */
            cmdOsSprintf(deviceName, DEVFS_DEVICE_SER, devNum);
        }
#else
        sprintf(deviceName, DEVICE_SER, devNum);
#endif
        fd = open(deviceName, O_RDWR, 0);
        if (fd < 0)
        {
            fprintf(stdout, "failed to open %s: %s\n",
                    deviceName, strerror(errno));
            return NULL;
        }
    }

    /* Configure the stream to replace each LF char with a LF-CR couple.    */
    result = tcgetattr(fd, &settings);
    if (result < 0)
    {
        close(fd);
        return NULL;
    }

    if (0 != pseudoTerm)
    {
        /* settings.c_iflag |= INLCR;*/
        settings.c_oflag |= ONLCR;
    }
    else
    {
        settings.c_iflag = TTYDEF_IFLAG;
        settings.c_oflag = TTYDEF_OFLAG;
        settings.c_lflag = TTYDEF_LFLAG;
        settings.c_cc[VERASE] = CERASE;
#ifdef  __FreeBSD__
        /* disable input SW flow control */
        settings.c_iflag &= ~(IXON | IXOFF | IXANY);   
        /* 8 BITS, No parity, one stop bit, No HW flow control */ 
        settings.c_cflag &= ~(CSIZE | PARENB | CSTOPB | CRTSCTS);
        settings.c_cflag |= CS8;
        /* enable receiver - not mandatory in all systems */
        settings.c_cflag |= CLOCAL | CREAD;
        /* set speed */
        cfsetispeed(&settings, B115200); 
        cfsetospeed(&settings, B115200);
#endif
    }

    if (getenv("SERIAL_SPEED"))
    {
        speed_t speed = B0;
        static const struct {
            speed_t speed;
            int     rate;
        } rates[] = {
            { B2400, 2400 },
            { B4800, 4800 },
            { B9600, 9600 },
            { B19200, 19200 },
            { B38400, 38400 },
            { B57600, 57600 },
            { B115200, 115200 },
            { B230400, 230400 },
            { B0, 0 }
        };
        int i;
        int rate = atoi(getenv("SERIAL_SPEED"));

        for (i = 0; rates[i].rate; i++)
            if (rates[i].rate == rate)
                speed = rates[i].speed;

        if (speed != B0)
        {
            cfsetispeed(&settings, speed);
            cfsetospeed(&settings, speed);
        }
    }

    result = tcsetattr(fd, TCSAFLUSH, &settings);
    if (result < 0)
    {
        close(fd);
        return NULL;
    }

    stream = (serialStreamPTR)cmdOsMalloc(sizeof(*stream));
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
    stream->common.isConsole = GT_FALSE;
    stream->common.flags = 0;
    stream->fd = fd;
    /* force lineedit mode */
    serialSetTtyMode((IOStreamPTR)stream, 1);

    return (IOStreamPTR)stream;
}
