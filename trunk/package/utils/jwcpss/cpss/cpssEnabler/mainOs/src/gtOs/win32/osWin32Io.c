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
* osWin32Io.c
*
* DESCRIPTION:
*       Win32 Operating System Simulation. Input/output facility.
*
* DEPENDENCIES:
*       Win32, CPU independed.
*
* FILE REVISION NUMBER:
*       $Revision: 6 $
*******************************************************************************/

#include <windows.h>
#include <process.h>

#include <winbase.h>

#include <stdio.h>
#include <stdarg.h>
#include <errno.h>

#include <gtOs/gtOsIo.h>

#include <asicSimulation/SCIB/scib.h>

/* the lock,unlock of the 'scib sem' must be only after the scib initialized
   so before that we not take the semaphore , and not release it.
*/
#define PROTECT_TASK_DOING_IO_ACTION_START_MAC      \
    if(simulationInitReady)                         \
    {                                               \
        SCIB_SEM_TAKE;                              \
        protectionDone = 1;                         \
    }

/*unlock semaphore only if we know that we locked it ! */
#define PROTECT_TASK_DOING_IO_ACTION_END_MAC        \
    if(protectionDone)                              \
        SCIB_SEM_SIGNAL

extern GT_U32   simulationInitReady;
static GT_U32   protectionDone = 0;

/************* Global Arguments ***********************************************/

GT_STATUS (*osPrintSyncUartBindFunc)(char *, GT_U32) = NULL;

/************* Static variables ***********************************************/
static OS_BIND_STDOUT_FUNC_PTR writeFunctionPtr = NULL;
static GT_VOID_PTR             writeFunctionParam = NULL;

/************ Public Functions ************************************************/

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
*       0
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
)
{
    return 0;
}

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
GT_STATUS osGetStdOutFunction
(
    OUT OS_BIND_STDOUT_FUNC_PTR     *writeFunctionPtrPtr,
    OUT GT_VOID_PTR                 *writeFunctionParamPtrPtr
)
{
    *writeFunctionPtrPtr        = writeFunctionPtr;
    *writeFunctionParamPtrPtr   = writeFunctionParam;

    return GT_OK;
}

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
GT_STATUS osBindStdOut
(
    IN OS_BIND_STDOUT_FUNC_PTR writeFunction,
    IN void* userPtr
)
{
    writeFunctionPtr = writeFunction;
    writeFunctionParam = userPtr;
    return GT_OK;
}

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
int osPrintf(const char* format, ...)
{
    char buff[2048];
    va_list args;
    int i;

    va_start(args, format);
    i = vsprintf(buff, format, args);
    va_end(args);

    PROTECT_TASK_DOING_IO_ACTION_START_MAC;

    if (writeFunctionPtr != NULL)
    {
        i =  writeFunctionPtr(writeFunctionParam, buff, i);
    }
    else
    {
        i =  printf("%s", buff);
    }

    PROTECT_TASK_DOING_IO_ACTION_END_MAC;


    return i;
}


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
int osSprintf(char * buffer, const char* format, ...)
{
    va_list args;
    int i;

    va_start(args, format);
    i = vsprintf(buffer, format, args);
    va_end(args);

    return i;
}

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
int osVsprintf(char * buffer, const char* format, va_list args)
{
    return vsprintf(buffer, format, args);
}

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
int osSnprintf(char * buffer, int size, const char* format, ...)
{
    va_list args;
    int i;

    va_start(args, format);
    i = _vsnprintf(buffer, (size_t)size, format, args);
    va_end(args);

    if (i >= size)
    {
        buffer[size-1] = 0;
        i = size-1;
    }

    return i;
}

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
int osVsnprintf(char * buffer, int size, const char* format, va_list args)
{
    int i;
    i = _vsnprintf(buffer, (size_t)size, format, args);
    if (i >= size)
    {
        buffer[size-1] = 0;
        i = size-1;
    }
    return i;
}

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
char * osGets(char * buffer)
{
    return gets(buffer);
}

/*******************************************************************************
* osPrintSync
*
* DESCRIPTION:
*       Write a formatted string to the standard output stream, in polling mode
*                The device driver will print the string in the same instance this function
*       is called, with no delay.
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
int osPrintSync(const char* format, ...)
{
    char buff[2048];
    va_list args;
    int i, retVal = 0;


    va_start(args, format);
    i = vsprintf(buff, format, args);
    va_end(args);

    PROTECT_TASK_DOING_IO_ACTION_START_MAC;
    if ( osPrintSyncUartBindFunc != NULL )
    {
      retVal = (int)((osPrintSyncUartBindFunc)(buff, (GT_U32)strlen(buff)));
    }
    else /* similar to osPrintf(...) */
    {
        if (writeFunctionPtr != NULL)
        {
            i =  writeFunctionPtr(writeFunctionParam, buff, i);
        }
        else
        {
            i =  printf("%s", buff);
        }
    }
    PROTECT_TASK_DOING_IO_ACTION_END_MAC;


    return (retVal);
}

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
const char * osFlastErrorStr(void)
{
    return strerror(errno);
}

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
OS_FILE osFopen(const char * fileNamePtr, const char * modePtr)
{
    FILE *fd = fopen(fileNamePtr, modePtr);
    if (!fd)
        return OS_FILE_INVALID;
    return (OS_FILE)fd;
}

#define OS_FILE_TO_FD(_stream) \
    FILE *fd = (FILE *)(_stream); \
    if ((_stream) == OS_FILE_STDOUT) \
        fd = stdout; \
    if ((_stream) == OS_FILE_STDERR) \
        fd = stderr;

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
int osFclose(OS_FILE stream)
{
    OS_FILE_TO_FD(stream);
    return fclose(fd);
}

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
void osRewind(OS_FILE stream)
{
    OS_FILE_TO_FD(stream);
    rewind(fd);
}

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
int osFprintf(OS_FILE stream, const char* format, ...)
{
    va_list args;
    char buffer[2048];
    OS_FILE_TO_FD(stream);

    va_start(args, format);
    vsprintf(buffer, format, args);
    va_end(args);

    return fprintf(fd, "%s", buffer);
}

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
char * osFgets(char * bufferPtr, int num, OS_FILE stream)
{
    OS_FILE_TO_FD(stream);
    return fgets(bufferPtr, num, fd);
}

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
)
{
    OS_FILE_TO_FD(stream);
    return (int)fwrite(ptr,size,nmemb,fd);
}

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
)
{
    OS_FILE_TO_FD(stream);
    return (int)fread(ptr,size,nmemb,fd);
}

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
)
{
    long curr, length;
    OS_FILE_TO_FD(stream);
    curr = ftell(fd);
    fseek(fd, 0, SEEK_END);
    length = ftell(fd);
    fseek(fd, curr, SEEK_SET);
    return (GT_UINTPTR)length;
}

