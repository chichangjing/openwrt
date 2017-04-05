/*******************************************************************************
*              Copyright 2001, GALILEO TECHNOLOGY, LTD.
*
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL. NO RIGHTS ARE GRANTED
* HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT OF MARVELL OR ANY THIRD
* PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE DISCRETION TO REQUEST THAT THIS
* CODE BE IMMEDIATELY RETURNED TO MARVELL. THIS CODE IS PROVIDED "AS IS".
* MARVELL MAKES NO WARRANTIES, EXPRESS, IMPLIED OR OTHERWISE, REGARDING ITS
* ACCURACY, COMPLETENESS OR PERFORMANCE. MARVELL COMPRISES MARVELL TECHNOLOGY
* GROUP LTD. (MTGL) AND ITS SUBSIDIARIES, MARVELL INTERNATIONAL LTD. (MIL),
* MARVELL TECHNOLOGY, INC. (MTI), MARVELL SEMICONDUCTOR, INC. (MSI), MARVELL
* ASIA PTE LTD. (MAPL), MARVELL JAPAN K.K. (MJKK), GALILEO TECHNOLOGY LTD. (GTL)
* AND GALILEO TECHNOLOGY, INC. (GTI).
********************************************************************************
* gtOsIo.h
*
* DESCRIPTION:
*       Operating System wrapper. Input/output facility.
*
* DEPENDENCIES:
*       None.
*
* FILE REVISION NUMBER:
*       $Revision: 9 $
*******************************************************************************/

#ifndef __gtOsIoh
#define __gtOsIoh

#ifdef __cplusplus
extern "C" {
#endif

/************* Includes *******************************************************/

#include <gtOs/gtGenTypes.h>
#if (!defined(__KERNEL__) && !defined(KERNEL) && !defined(INKERNEL) && !defined(_KERNEL))
#include <stdarg.h>
#endif

/************* Definitions ****************************************************/

/* a type for file */
typedef GT_UINTPTR OS_FILE;
#define OS_FILE_INVALID 0
#define OS_FILE_STDOUT  1
#define OS_FILE_STDERR  2

/************* Global Arguments ***********************************************/

extern GT_STATUS (*osPrintSyncUartBindFunc)(char *, GT_U32);

/************* Functions ******************************************************/
typedef int (*OS_BIND_STDOUT_FUNC_PTR)(void* userPtr, const char* buffer, int length);

/*******************************************************************************
* osNullStdOutFunction
*
* DESCRIPTION:
*       Get Stdout handler
*
* INPUTS:
*
* OUTPUTS:
*       writeFunctionPtrPtr         - poiter to saved output function 
*       writeFunctionParamPtrPtr    - poiter to saved output function parameter
*
* RETURNS:
*       GT_OK
*
* COMMENTS:
*       None
* 
*******************************************************************************/
int osNullStdOutFunction
(
    GT_VOID_PTR         userPtr, 
    const char*         buffer, 
    int                 length
);

/*******************************************************************************
* osGetStdOutFunction
*
* DESCRIPTION:
*       Get Stdout handler
*
* INPUTS:
*
* OUTPUTS:
*       writeFunctionPtrPtr         - poiter to saved output function 
*       writeFunctionParamPtrPtr    - poiter to saved output function parameter
*
* RETURNS:
*       GT_OK
*
* COMMENTS:
*       None
* 
*******************************************************************************/
GT_STATUS osGetStdOutFunction(
    OUT OS_BIND_STDOUT_FUNC_PTR     *writeFunctionPtrPtr,
    OUT GT_VOID_PTR                 *writeFunctionParamPtrPtr
);

/*******************************************************************************
* osBindStdOut
*
* DESCRIPTION:
*       Bind Stdout to handler
*
* INPUTS:
*       writeFunction - function to call for output
*       userPtr       - first parameter to pass to write function
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS osBindStdOut(
    IN OS_BIND_STDOUT_FUNC_PTR writeFunction,
    IN void* userPtr
);

/*******************************************************************************
* osPrintf
*
* DESCRIPTION:
*       Write a formatted string to the standard output stream.
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
int osPrintf(const char* format, ...);

/*******************************************************************************
* osVprintf
*
* DESCRIPTION:
*       Write a formatted string to the standard output stream.
*
* INPUTS:
*       format  - format string to write
*       args    - va_list
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
*******************************************************************************/
int osVprintf(const char* format, va_list args);

/*******************************************************************************
* osSprintf
*
* DESCRIPTION:
*       Write a formatted string to a buffer.
*
* INPUTS:
*       buffer  - buffer to write to
*       format  - format string
*
* OUTPUTS:
*       None
*
* RETURNS:
*       The number of characters copied to buffer, not including
*       the NULL terminator.
*
* COMMENTS:
*       None
*
*******************************************************************************/
int osSprintf(char * buffer, const char* format, ...);

/*******************************************************************************
* osVsprintf
*
* DESCRIPTION:
*       Write a formatted string to a buffer.
*
* INPUTS:
*       buffer  - buffer to write to
*       format  - format string
*       args    - va_list
*
* OUTPUTS:
*       None
*
* RETURNS:
*       The number of characters copied to buffer, not including
*       the NULL terminator.
*
* COMMENTS:
*       None
*
*******************************************************************************/
int osVsprintf(char * buffer, const char* format, va_list args);

/*******************************************************************************
* osSnprintf
*
* DESCRIPTION:
*       Write a formatted string to a buffer.
*
* INPUTS:
*       buffer  - buffer to write to
*       size    - buffer size
*       format  - format string
*
* OUTPUTS:
*       None
*
* RETURNS:
*       The number of characters copied to buffer, not including
*       the NULL terminator.
*
* COMMENTS:
*       None
*
*******************************************************************************/
int osSnprintf(char * buffer, int size, const char* format, ...);

/*******************************************************************************
* osVsnprintf
*
* DESCRIPTION:
*       Write a formatted string to a buffer.
*
* INPUTS:
*       buffer  - buffer to write to
*       size    - buffer size
*       format  - format string
*       args    - va_list
*
* OUTPUTS:
*       None
*
* RETURNS:
*       The number of characters copied to buffer, not including
*       the NULL terminator.
*
* COMMENTS:
*       None
*
*******************************************************************************/
int osVsnprintf(char * buffer, int size, const char* format, va_list args);

/*******************************************************************************
* osPrintSync
*
* DESCRIPTION:
*       Write a formatted string to the standard output stream.
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
int osPrintSync(const char* format, ...);

/*******************************************************************************
* osGets
*
* DESCRIPTION:
*       Reads characters from the standard input stream into the array
*       'buffer' until end-of-file is encountered or a new-line is read.
*       Any new-line character is discarded, and a null character is written
*       immediately after the last character read into the array.
*
* INPUTS:
*       buffer  - pointer to buffer to write to
*
* OUTPUTS:
*       buffer  - buffer with read data
*
* RETURNS:
*       A pointer to 'buffer', or a null pointer if end-of-file is
*       encountered and no characters have been read, or there is a read error.
*
* COMMENTS:
*       None
*
*******************************************************************************/
char * osGets(char * buffer);

/*******************************************************************************
* osFlastErrorStr
*
* DESCRIPTION:
*       returns error string for last error
*
* INPUTS:
*       None
*
* OUTPUTS:
*       None
*
* RETURNS:
*       string
*
* COMMENTS:
*       None
*
*******************************************************************************/
const char * osFlastErrorStr(void);

/*******************************************************************************
* osFopen
*
* DESCRIPTION:
*       Opens the file whose name is specified in the parameter filename and associates it with a stream
*       that can be identified in future operations by the OS_FILE pointer returned.
*       
*
* INPUTS:
*       fileNamePtr  - (pointer to) string containing the name of the file to be opened
*       modePtr - (pointer to) string containing a file access mode
*
* OUTPUTS:
*       None
*
* RETURNS:
*       OS_FILE object that can be used to identify the stream on future operations
*       or OS_FILE_INVALID if opening fails
*
* COMMENTS:
*       None
*
*******************************************************************************/
OS_FILE osFopen(const char * fileNamePtr, const char * modePtr);

/*******************************************************************************
* osFclose
*
* DESCRIPTION:
*       Closes the file associated with the stream and disassociates it.
*
* INPUTS:
*       stream  - OS_FILE object that specifies the stream.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       If the stream is successfully closed, a zero value is returned.
*       On failure, EOF is returned.
*
* COMMENTS:
*       Nome
*
*******************************************************************************/
int osFclose(OS_FILE stream);

/*******************************************************************************
* osRewind
*
* DESCRIPTION:
*       Sets the position indicator associated with stream to the beginning of the file.
*
* INPUTS:
*       stream  - OS_FILE object that specifies the stream.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       None
*
* COMMENTS:
*       Nome
*
*******************************************************************************/
void osRewind(OS_FILE stream);

/*******************************************************************************
* osFprintf
*
* DESCRIPTION:
*       Write a formatted string to the stream.
*
* INPUTS:
*       stream  - OS_FILE object that specifies the stream.
*       format  - format string to write
*       ... - parameters of the 'format'
*
* OUTPUTS:
*       None
*
* RETURNS:
*       On success, the total number of characters written is returned.
*       If a writing error occurs, the error indicator (ferror) is set and a negative number is returned.
*
* COMMENTS:
*       None
*
*******************************************************************************/
int osFprintf(OS_FILE stream, const char* format, ...);

/*******************************************************************************
* osFgets
*
* DESCRIPTION:
*       Reads characters from stream and stores them as a string into buffer until (num-1) characters have been read
*       or either a newline or the end-of-file is reached, whichever happens first.
*
* INPUTS:
*       bufferPtr  - (pointer to) buffer to write to
*       num - maximum number of characters to be copied into str (including the terminating null-character)
*       stream  - OS_FILE object that specifies the stream.
*
* OUTPUTS:
*       bufferPtr  - (pointer to)buffer with read data
*
* RETURNS:
*       A pointer to 'bufferPtr', or a null pointer if end-of-file is
*       encountered and no characters have been read, or there is a read error.
*
* COMMENTS:
*       None
*
*******************************************************************************/
char * osFgets(char * bufferPtr, int num, OS_FILE stream);

/*******************************************************************************
* osFwrite
*
* DESCRIPTION:
*       Writes nmemb elements of data, each size bytes long, to the stream,
*       obtaining them from the location given by ptr.
*
* INPUTS:
*       ptr   - (pointer from) buffer to write from
*       size  - element size
*       nmemb - number of elements to write
*       stream - OS_FILE object that specifies the stream.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       Number of items written or less than 0
*
* COMMENTS:
*       None
*
*******************************************************************************/
int osFwrite(
    IN const void *ptr,
    IN int         size,
    IN int         nmemb,
    IN OS_FILE     stream
);

/*******************************************************************************
* osFread
*
* DESCRIPTION:
*       Reads nmemb elements of data, each size bytes long, from the stream,
*       to the location given by ptr.
*
* INPUTS:
*       size  - element size
*       nmemb - number of elements to write
*       stream - OS_FILE object that specifies the stream.
*
* OUTPUTS:
*       ptr   - (pointer to) buffer to read to
*
* RETURNS:
*       Number of items read or less than 0
*
* COMMENTS:
*       None
*
*******************************************************************************/
int osFread(
    OUT void        *ptr,
    IN  int          size,
    IN  int          nmemb,
    IN  OS_FILE      stream
);

/*******************************************************************************
* osFgetLength
*
* DESCRIPTION:
*       Returns lenhth of current file
*
* INPUTS:
*       stream - OS_FILE object that specifies the stream.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       file length or 0
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_UINTPTR osFgetLength(
    IN  OS_FILE stream
);

#ifdef __cplusplus
}
#endif

#endif  /* __gtOsIoh */
/* Do Not Add Anything Below This Line */

