/*******************************************************************************
*              (c), Copyright 2007, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* IOStream.c
*
* DESCRIPTION:
*       This file contains basic implementation of IOStream
*
* FILE REVISION NUMBER:
*       $Revision: 1 $
*
*******************************************************************************/
#include <extUtils/IOStream/IOStream.h>
#include <cpss/extServices/private/prvCpssBindFunc.h>


/************* NULL stream implementation *************************************/
static int nullStreamDestroy(IOStreamPTR stream GT_UNUSED)
{
    return 0;
}
static int nullStreamRead(IOStreamPTR stream GT_UNUSED, void* bufferPtr GT_UNUSED, int bufferLength GT_UNUSED)
{
    return 0;
}
static int nullStreamReadChar(IOStreamPTR stream GT_UNUSED, char* charPtr GT_UNUSED, int timeOut GT_UNUSED)
{
    return -1;
}
static int nullStreamReadLine(IOStreamPTR stream GT_UNUSED, char* bufferPtr GT_UNUSED, int bufferLength GT_UNUSED)
{
    return 0;
}
static int nullStreamWrite(IOStreamPTR stream GT_UNUSED, const void* bufferPtr GT_UNUSED, int bufferLength)
{
    return bufferLength;
}
static int nullStreamWriteBuf(struct IOStreamSTC* stream GT_UNUSED, const char* s GT_UNUSED, int len)
{
    return len;
}
static int nullStreamWriteLine(IOStreamPTR stream GT_UNUSED, const char *s)
{
    return cpssOsStrlen(s);
}
static int nullStreamConnected(IOStreamPTR stream GT_UNUSED)
{
    return 0;
}
static int nullStreamSetTtyMode(IOStreamPTR stream GT_UNUSED, int mode GT_UNUSED)
{
    return 0;
}

/*******************************************************************************
* IOStreamCreateNULL
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
*       stream - destination stream
*
* COMMENTS:
*       None
*
*******************************************************************************/
IOStreamPTR IOStreamCreateNULL(void)
{
    static IOStreamSTC stream = {
        nullStreamDestroy,
        nullStreamRead,
        nullStreamReadChar,
        nullStreamReadLine,
        nullStreamWrite,
        nullStreamWriteBuf,
        nullStreamWriteLine,
        nullStreamConnected,
        NULL,     /* grabStd */
        nullStreamSetTtyMode,
        NULL,     /* getFd */
        GT_FALSE, /* isConsole */
        NULL,     /* customPtr */
        0,        /* flags */
        GT_FALSE  /* wasCr */
    };

    return &stream;
}



/************* generic methods implementation *********************************/

/* ascii codes */
#define BKSP        (0x08)  /* back space character         */
#define DEL         (0x7f)  /* back space character         */
#define XON_CHAR_CNS   17 /*0x11*/
#define XOFF_CHAR_CNS  19 /*0x13*/

/*******************************************************************************
* IOStreamGenericReadLine
*
* DESCRIPTION:
*       Generic readLine method for IOStreamSTC
*       Read line from stream. Perform input editing/translations
*       Returned line is zero terminated with stripped EOL symbols
*
* INPUTS:
*       stream       - destination stream
*       bufferLength - buffer length
*
* OUTPUTS:
*       bufferPtr    - pointer to output buffer
*
* RETURNS:
*       < 0 if error occured
*       number of received bytes
*
*******************************************************************************/
int IOStreamGenericReadLine(IOStreamPTR streamP, char* bufferPtr, int bufferLength)
{
    int k, p;
    char ch;
    p = 0;

    if (!streamP)
        return -1;

    while (streamP->connected(streamP) && p < bufferLength)
    {
        k = streamP->read(streamP, &ch, 1);
        if (k < 0)
        {
            return -1;
        }
        if (k == 0)
            continue;
        if (ch == '\n')
        {
            if (streamP->flags & IO_STREAM_FLAG_I_CANON)
            {
                if (streamP->wasCR)
                {
                    streamP->wasCR = GT_FALSE;
                    continue;
                }
                if (streamP->flags & IO_STREAM_FLAG_I_ECHO)
                {
                    streamP->writeBuf(streamP, "\r\n", 2);
                }
            }
            break;
        }
        streamP->wasCR = GT_FALSE;
        if (streamP->flags & IO_STREAM_FLAG_I_CANON)
        {
            switch (ch) {
                case '\0':
                    break;

                case BKSP:
                case DEL:
                    if (p)
                    {
                        p--;
                        if (streamP->flags & IO_STREAM_FLAG_I_ECHO)
                        {
                            streamP->write(streamP, "\b \b", 3);
                        }
                    }
                    break;
                case '\r':
                    streamP->wasCR = GT_TRUE;
                    if (streamP->flags & IO_STREAM_FLAG_I_ECHO)
                    {
                        streamP->write(streamP, "\r\n", 2);
                    }
                    break;
                case XON_CHAR_CNS:
                case XOFF_CHAR_CNS:
                    /* characters send from the other size when , we send it
                       large amount of characters during 'osPrintf' */
                    /* we need to ignore those characters */
                    break;
                default:
                    ((char*)bufferPtr)[p++] = ch;
                    if (streamP->flags & IO_STREAM_FLAG_I_ECHO)
                    {
                        streamP->write(streamP, &ch, 1);
                    }
            }
            if (streamP->wasCR)
                break;
        } else {
            if (ch != '\r')
            {
                ((char*)bufferPtr)[p++] = ch;
                if (streamP->flags & IO_STREAM_FLAG_I_ECHO)
                {
                    streamP->write(streamP, &ch, 1);
                }
            }
        }
    }
    if (p < bufferLength)
    {
        bufferPtr[p] = 0;
    }
    return p;

    /* set input mode */
}

/*******************************************************************************
* IOStreamGenericWriteBuf
*
* DESCRIPTION:
*       Generic writeBuf method for IOStreamSTC
*       Write to stream. Perform output translations
*
* INPUTS:
*       stream       - destination stream
*       s            - pointer to buffer
*       bufferLength - number of bytes to send
*
* OUTPUTS:
*       None
*
* RETURNS:
*       < 0 if error occured
*       number of bytes was sent
*
*******************************************************************************/
int IOStreamGenericWriteBuf(IOStreamPTR streamP, const char *s, int len)
{
    const char *p, *e;
    int result = 0;
    int ret;

    if (!streamP)
        return -1;

    if (!(streamP->flags & IO_STREAM_FLAG_O_CRLF))
    {
        return streamP->write(streamP, (char*)s, len);
    }

    e = s + len;
    for (p = s; p < e; p++)
    {
        if (*p == '\n')
        {
            if (p != s)
            {
                ret = streamP->write(streamP, (char*)s, (int)(p-s));
                if (ret < 0)
                    return result ? result : -1;
                result += ret;
            }
            if (streamP->write(streamP, "\r", 1) < 0)
                return result ? result : -1;
            s = p;
        }
    }
    if (p != s)
    {
        ret = streamP->write(streamP, (char*)s, (int)(p-s));
        if (ret < 0)
            return result ? result : -1;
        result += ret;
    }
    return result;
}

/***************************************************************************
* IOStreamGenericWriteLine
*
* DESCRIPTION:
*       Generic writeLine method for IOStreamSTC
*       Write line to stream. Perform output translations
*
* INPUTS:
*       stream       - destination stream
*       s            - pointer to string
*
* OUTPUTS:
*       None
*
* RETURNS:
*       < 0 if error occured
*       number of bytes was sent
*
***************************************************************************/
int IOStreamGenericWriteLine(IOStreamPTR streamP, const char *s)
{
    if (!streamP)
        return -1;

    return streamP->writeBuf(streamP, s, cpssOsStrlen(s));
}
