#error "not use"
/*******************************************************************************
*              (c), Copyright 2007, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* cmdStreamTelnetProto.c
*
* DESCRIPTION:
*       This file contains Telnet protocol implementation for cmdStream
*
* FILE REVISION NUMBER:
*       $Revision: 3 $
*
* COMMENTS:
*       See RFC 854, 855, 856, 857, 858
*******************************************************************************/
#include <cmdShell/os/cmdStreamImpl.h>

/*********** telnet protocol implementation *****************************/

#define TELNET_IAC  255 /*  */
#define TELNET_DONT 254 /*  */
#define TELNET_DO   253 /*  */
#define TELNET_WONT 252 /*  */
#define TELNET_WILL 251 /*  */
#define TELNET_SB   250 /* subnegotiation of the indicated option begin */
#define TELNET_GA   249 /* Go ahead */
#define TELNET_EL   248 /* Erase Line */
#define TELNET_EC   247 /* Erase character */
#define TELNET_AYT  246 /* Are You There */
#define TELNET_AO   245 /* Abort output */
#define TELNET_IP   244 /* Interrupt Process */
#define TELNET_BRK  243 /* Break */
#define TELNET_DM   242 /* Data mark */
#define TELNET_NOP  241 /* No operation */
#define TELNET_SE   240 /* End of subnegotiation parameters */

#define TELNET_OPT_TRANSMIT_BINARY  0
#define TELNET_OPT_ECHO             1
#define TELNET_OPT_SGA              3

typedef enum {
    OPT_DISABLED,
    OPT_ENABLED,
    OPT_DISABLED_WAIT_ACK,
    OPT_ENABLED_WAIT_ACK
} TELNET_OPTION_STATUS_T;

typedef struct {
    cmdStreamSTC common;
    cmdStreamPTR transport;
    enum {
        RSTATE_NONE,
        RSTATE_IAC,
        RSTATE_IACDONT,
        RSTATE_IACDO,
        RSTATE_IACWONT,
        RSTATE_IACWILL
    } readState;
    TELNET_OPTION_STATUS_T opt_TRANSMIT_BINARY;
    TELNET_OPTION_STATUS_T opt_ECHO;
    TELNET_OPTION_STATUS_T opt_SGA;

} telnetStreamSTC, *telnetStreamPTR;

static int telnetStreamDestroy(cmdStreamPTR streamP)
{
    telnetStreamPTR stream = (telnetStreamPTR)streamP;

    if (!stream)
        return -1;

    stream->transport->destroy(stream->transport);
    cmdOsFree(stream);

    return 0;
}

#define SEND_CMD3(a1,a2,a3) { \
        cmdBuf[0] = a1; \
        cmdBuf[1] = a2; \
        cmdBuf[2] = a3; \
        stream->transport->write(stream->transport, cmdBuf, 3); \
    }

static int telnetStreamReadChar(cmdStreamPTR streamP, char* charPtr, int timeOut)
{
    telnetStreamPTR stream = (telnetStreamPTR)streamP;
    int k = 1;
    unsigned char cmdBuf[3];

    if (!stream)
        return -1;
    if (charPtr == NULL)
        return -1;

    if (!stream->transport->connected(stream->transport))
        return -1;

    /* telnet protocol read implementation */
    while (k == 1)
    {
        unsigned char ch;
        k = stream->transport->readChar(stream->transport, (char*)(&ch), timeOut);
        if (k <= 0)
            break;
        switch (stream->readState)
        {
            case RSTATE_NONE:
                if (ch != TELNET_IAC)
                {
                    *charPtr = (char)ch;
                    return 1;
                }
                stream->readState = RSTATE_IAC;
                break;
            case RSTATE_IAC:
                switch (ch)
                {
                    case TELNET_IAC:
                        stream->readState = RSTATE_NONE;
                        *charPtr = (char)ch;
                        return 1;
                    case TELNET_DONT:
                        stream->readState = RSTATE_IACDONT;
                        break;
                    case TELNET_DO:
                        stream->readState = RSTATE_IACDO;
                        break;
                    case TELNET_WONT:
                        stream->readState = RSTATE_IACWONT;
                        break;
                    case TELNET_WILL:
                        stream->readState = RSTATE_IACWILL;
                        break;
                    case TELNET_NOP:
                        stream->readState = RSTATE_NONE;
                        break;
                    case TELNET_IP:
                    case TELNET_AYT:
                    case TELNET_AO:
                    case TELNET_EC:
                    case TELNET_EL:
                    case TELNET_BRK:
                    case TELNET_GA:
                        /* ignore */
                        stream->readState = RSTATE_NONE;
                        break;

                    /* should never happen: initiated by server */
                    case TELNET_DM: /* Data mark */
                    /* should never happen - options require this is not supported */
                    case TELNET_SB: /* subnegotiation of the indicated option begin */
                    case TELNET_SE: /* End of subnegotiation parameters */
                        /* ignore */
                        stream->readState = RSTATE_NONE;
                        break;

                    default:
                        stream->readState = RSTATE_NONE;
                        *charPtr = (char)ch;
                        return 1;
                }
                break;
            case RSTATE_IACDONT:
#define DO_IACDONT_FOR(code, opt) \
                if (ch == code) \
                { \
                    if (stream->opt_ ## opt == OPT_DISABLED || stream->opt_ ## opt == OPT_ENABLED) \
                    { \
                        SEND_CMD3(TELNET_IAC, TELNET_WONT, code); \
                    } \
                    stream->opt_ ## opt = OPT_DISABLED; \
                }
                DO_IACDONT_FOR(TELNET_OPT_TRANSMIT_BINARY, TRANSMIT_BINARY);
                DO_IACDONT_FOR(TELNET_OPT_ECHO, ECHO);
                DO_IACDONT_FOR(TELNET_OPT_SGA, SGA);

                stream->readState = RSTATE_NONE;
                break;
            case RSTATE_IACDO:
#define DO_IACDO_FOR(code, opt) \
                if (ch == code) \
                { \
                    if (stream->opt_ ## opt == OPT_DISABLED || stream->opt_ ## opt == OPT_ENABLED) \
                    { \
                        SEND_CMD3(TELNET_IAC, TELNET_WILL, code); \
                    } \
                    stream->opt_ ## opt = OPT_ENABLED; \
                }
                DO_IACDO_FOR(TELNET_OPT_TRANSMIT_BINARY, TRANSMIT_BINARY);
                DO_IACDO_FOR(TELNET_OPT_ECHO, ECHO);
                DO_IACDO_FOR(TELNET_OPT_SGA, SGA);

                stream->readState = RSTATE_NONE;
                break;
            case RSTATE_IACWONT:
                SEND_CMD3(TELNET_IAC, TELNET_DONT, ch);
                stream->readState = RSTATE_NONE;
                break;
            case RSTATE_IACWILL:
                SEND_CMD3(TELNET_IAC, TELNET_DONT, ch);
                stream->readState = RSTATE_NONE;
                break;
        }
    }
    return k;
}

static int telnetStreamRead(cmdStreamPTR streamP, void* bufferPtr, int bufferLength)
{
    int     k;
    int     bytes_read;
    char    *charPtr = (char*)bufferPtr;

    if (!streamP)
        return -1;

    if (bufferPtr == NULL || bufferLength < 1)
        return -1;

    for (bytes_read = 0; bytes_read < bufferLength; bytes_read++)
    {
        k = telnetStreamReadChar(streamP, charPtr + bytes_read, 0);
        if (k <= 0)
        {
            return bytes_read ? bytes_read : k;
        }
    }
    return bytes_read;
}

static int telnetStreamWrite(cmdStreamPTR streamP, const void* bufferPtr, int bufferLength)
{
    telnetStreamPTR stream = (telnetStreamPTR)streamP;
    const unsigned char *charPtr = (const unsigned char*)bufferPtr;
    int start, pos;
    int k, sent = 0;

    /* Replace IAC wit IAC IAC */

    if (!stream)
        return -1;

    if (bufferPtr == NULL || bufferLength < 0)
        return -1;
    sent = 0;
    for (start = pos = 0; pos < bufferLength; pos++)
    {
        if (charPtr[pos] == TELNET_IAC)
        {
            k = stream->transport->write(stream->transport, charPtr + start, pos - start + 1);
            if (k <= 0)
            {
                return sent > 0 ? sent : k;
            }
            sent += k;
            if (k < pos - start + 1)
            {              
                start += k;
                pos--;
            }
            else
            {
                k = stream->transport->write(stream->transport, charPtr + pos, 1);
                if (k <= 0)
                {
                    return sent;
                }
                start = pos + 1;
            }
        }
    }

    while (pos > start)
    {
        k = stream->transport->write(stream->transport, charPtr + start, pos - start);
        if (k <= 0)
        {
            return sent > 0 ? sent : k;
        }
        sent += k;
        start += k;
    }
    return sent;
}

static int telnetStreamConnected(cmdStreamPTR streamP)
{
    telnetStreamPTR stream = (telnetStreamPTR)streamP;

    if (!stream)
        return -1;

    return stream->transport->connected(stream->transport);
}

static int telnetGrabStd(cmdStreamPTR streamP GT_UNUSED)
{
    /* not supported. Workaround: grab to socket stream */
    return -1;
}

static int telnetSetTtyMode(cmdStreamPTR streamP, int mode)
{
    telnetStreamPTR stream = (telnetStreamPTR)streamP;

    if (mode == 0)
    {
        /* raw mode */
        stream->common.flags &= ~(
                STREAM_FLAG_O_CRLF |
                STREAM_FLAG_I_CANON |
                STREAM_FLAG_I_ECHO
            );
    }
    else
    {
        /* line edit mode */
        stream->common.flags |= 
            STREAM_FLAG_O_CRLF |
            STREAM_FLAG_I_CANON |
            STREAM_FLAG_I_ECHO;
    }
    return 0;
}

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
cmdStreamPTR cmdStreamCreateTelnet
(
    IN cmdStreamPTR socket
)
{
    telnetStreamPTR stream;
    unsigned char cmdBuf[3];

    stream = (telnetStreamPTR)cmdOsMalloc(sizeof(*stream));
    if (!stream)
    {
        return NULL;
    }

    cmdOsMemSet(stream, 0, sizeof(*stream));
    stream->common.destroy = telnetStreamDestroy;
    stream->common.read = telnetStreamRead;
    stream->common.readChar = telnetStreamReadChar;
    stream->common.readLine = streamGenericReadLine;
    stream->common.write = telnetStreamWrite;
    stream->common.writeBuf = streamGenericWriteBuf;
    stream->common.writeLine = streamGenericWriteLine;
    stream->common.connected = telnetStreamConnected;
    stream->common.grabStd = telnetGrabStd;
    stream->common.setTtyMode = telnetSetTtyMode;
    stream->common.isConsole = GT_FALSE;
    stream->common.flags = 
        STREAM_FLAG_O_CRLF |
        STREAM_FLAG_I_CANON |
        STREAM_FLAG_I_ECHO;
    stream->transport = socket;

    /* set telnet protocol options */
    stream->readState = RSTATE_NONE;
    stream->opt_TRANSMIT_BINARY = OPT_ENABLED;

    SEND_CMD3(TELNET_IAC, TELNET_WILL, TELNET_OPT_ECHO);
    stream->opt_ECHO = OPT_ENABLED_WAIT_ACK;

    SEND_CMD3(TELNET_IAC, TELNET_WILL, TELNET_OPT_SGA);
    stream->opt_SGA = OPT_ENABLED_WAIT_ACK;

    return (cmdStreamPTR)stream;
}
