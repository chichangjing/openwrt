/*******************************************************************************
*              (c), Copyright 2015, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* IOStream.h
*
* DESCRIPTION:
*       This file contains types and routines for I/O streams
*
* FILE REVISION NUMBER:
*       $Revision: 1 $
*
*******************************************************************************/
#ifndef __IOStreamh
#define __IOStreamh

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/*
 * Typedef: struct IOStreamSTC
 *
 * Description:
 *      This structure represent a I/O stream.
 *
 * Fields:
 *      destroy()     - Destroy stream
 *      read()        - Raw read from stream (blocking)
 *      readChar()    - Read character from stream (blocking)
 *      readLine()    - Read line from stream
 *      write()       - Raw write to stream
 *      writeBuf()    - Write to stream with output filtering
 *      writeLine()   - Write line to stream
 *      connected()   - stream connection status
 *      grabStd()     - Redirect system stdout/stderr to stream
 *      setTtyMode()  - Set terminal mode to raw(0) or linemode(1)
 *      getFd()       - Return file descriptor for this stream
 *      isConsole     - console flag. Line editing/echoing/prompting required
 *      flags         - I/O flags for generic methods
 *      wasCR         - internal flag for generic readLine method
 *
 */
struct IOStreamSTC {
    /***************************************************************************
    * destroy
    *
    * DESCRIPTION:
    *       Destroy stream
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
    ***************************************************************************/
    int (*destroy)(struct IOStreamSTC* stream);

    /***************************************************************************
    * read
    *
    * DESCRIPTION:
    *       Raw read from stream (blocking)
    *       Used by readLine method
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
    ***************************************************************************/
    int (*read)(struct IOStreamSTC* stream, void* bufferPtr, int bufferLength);

    /***************************************************************************
    * readChar
    *
    * DESCRIPTION:
    *       Read character from stream (blocking)
    *
    * INPUTS:
    *       stream       - destination stream
    *       timeOut      - timeout in milliseconds, -1 means wait forever
    *
    * OUTPUTS:
    *       charPtr    - pointer to output buffer
    *
    * RETURNS:
    *       < 0 if error occured
    *       0 if timeout
    *       1 if character read
    *       number of received bytes
    *
    ***************************************************************************/
    int (*readChar)(struct IOStreamSTC* stream, char* charPtr, int timeOut);

    /***************************************************************************
    * readLine
    *
    * DESCRIPTION:
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
    ***************************************************************************/
    int (*readLine)(struct IOStreamSTC* stream, char* bufferPtr, int bufferLength);

    /***************************************************************************
    * write
    *
    * DESCRIPTION:
    *       Raw write to stream
    *       Used by writeBuf method
    *
    * INPUTS:
    *       stream       - destination stream
    *       bufferPtr    - pointer to buffer
    *       bufferLength - number of bytes to send
    *
    * OUTPUTS:
    *       None
    *
    * RETURNS:
    *       < 0 if error occured
    *       number of bytes was sent
    *
    ***************************************************************************/
    int (*write)(struct IOStreamSTC* stream, const void* bufferPtr, int bufferLength);

    /***************************************************************************
    * writeBuf
    *
    * DESCRIPTION:
    *       Write to stream. Perform output translations
    *
    * INPUTS:
    *       stream       - destination stream
    *       bufferPtr    - pointer to buffer
    *       bufferLength - number of bytes to send
    *
    * OUTPUTS:
    *       None
    *
    * RETURNS:
    *       < 0 if error occured
    *       number of bytes was sent
    *
    ***************************************************************************/
    int (*writeBuf)(struct IOStreamSTC* stream, const char* s, int len);

    /***************************************************************************
    * writeLine
    *
    * DESCRIPTION:
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
    int (*writeLine)(struct IOStreamSTC* stream, const char* s);

    /***************************************************************************
    * connected
    *
    * DESCRIPTION:
    *       Return stream connected status
    *
    * INPUTS:
    *       stream       - destination stream
    *
    * OUTPUTS:
    *       None
    *
    * RETURNS:
    *       Non zero if stream connected
    *       Zero if not connected (connection lost)
    *
    ***************************************************************************/
    int (*connected)(struct IOStreamSTC* stream);

    /***************************************************************************
    * grabStd
    *
    * DESCRIPTION:
    *       Redirect system stdout/stderr to stream
    *
    * INPUTS:
    *       stream       - destination stream
    *
    * OUTPUTS:
    *       None
    *
    * RETURNS:
    *       Non zero if stream connected
    *       Zero if not connected (connection lost)
    *
    ***************************************************************************/
    int (*grabStd)(struct IOStreamSTC* stream);

    /***************************************************************************
    * setTtyMode
    *
    * DESCRIPTION:
    *       Set terminal mode to raw(0) or linemode(1)
    *
    * INPUTS:
    *       stream       - destination stream
    *
    * OUTPUTS:
    *       None
    *
    * RETURNS:
    *       Non zero if stream connected
    *       Zero if not connected (connection lost)
    *
    ***************************************************************************/
    int (*setTtyMode)(struct IOStreamSTC* stream, int mode);

    /***************************************************************************
    * getFd
    *
    * DESCRIPTION:
    *       Return file descriptor for this stream
    *
    * INPUTS:
    *       stream       - stream
    *
    * OUTPUTS:
    *       None
    *
    * RETURNS:
    *       -1 on error
    *       file descriptor
    *
    ***************************************************************************/
    int (*getFd)(struct IOStreamSTC* stream);

    /***************************************************************************
    * isConsole
    *
    * DESCRIPTION:
    *       Application flag
    *
    ***************************************************************************/
    int isConsole;

    /***************************************************************************
    * customPtr
    *
    * DESCRIPTION:
    *       Application specific data
    *
    ***************************************************************************/
    void* customPtr;

    /***************************************************************************
    * flags
    *
    * DESCRIPTION:
    *       Stream I/O filtering/editing flags
    *
    ***************************************************************************/
#define IO_STREAM_FLAG_O_CRLF   1
#define IO_STREAM_FLAG_I_CANON  2
#define IO_STREAM_FLAG_I_ECHO   4
#define IO_STREAM_FLAG_C_FIRSTSHELL   0x40000000
    int flags;

    /***************************************************************************
    * wasCR
    *
    * DESCRIPTION:
    *       Input editing flags (internal)
    *
    ***************************************************************************/
    int wasCR; /* flag for readline */
};
typedef struct IOStreamSTC IOStreamSTC;
typedef struct IOStreamSTC *IOStreamPTR;

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
int IOStreamGenericReadLine(IOStreamPTR streamP, char* bufferPtr, int bufferLength);

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
int IOStreamGenericWriteBuf(IOStreamPTR streamP, const char *s, int len);

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
int IOStreamGenericWriteLine(IOStreamPTR streamP, const char *s);

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
*       stream - null stream
*
* COMMENTS:
*       None
*
*******************************************************************************/
IOStreamPTR IOStreamCreateNULL(void);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __IOStreamh */
