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
* os_w32IO.c
*
* DESCRIPTION:
*       win32 extended system wrapper layer
*
*       this module implements pipe io and standard io redirection routines
*
* DEPENDENCIES:
*
* FILE REVISION NUMBER:
*       $Revision: 4 $
*******************************************************************************/

/***** Include files ***************************************************/

/* WA to avoid next warning :
   due to include to : #include <windows.h>
    c:\program files\microsoft visual studio\vc98\include\rpcasync.h(45) :
    warning C4115: '_RPC_ASYNC_STATE' : named type definition in parentheses
*/
struct _RPC_ASYNC_STATE;

#include <cmdShell/os/cmdExecute.h>
#include <windows.h>
#include <stdio.h>
#include <gtOs/gtGenTypes.h>

typedef GT_STATUS (*SHELL_FUNC)(char* command);
/* global, defined cmdConsole.c */
extern SHELL_FUNC osSlellFunction;


/*******************************************************************************
* osShellExecute
*
* DESCRIPTION:
*       execute command through OS shell
*
* INPUTS:
*       command - null terminated command string
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK       - on success
*       GT_FAIL     - on error
*       GT_TIMEOUT  - on timeout
*
* COMMENTS:
*       NOT IMPLEMENTED (WIN32)
*
*******************************************************************************/
GT_STATUS   osShellExecute
(
    IN  char*   command
)
{
    if (osSlellFunction != NULL)
    {
        return osSlellFunction(command);
    }
    return GT_FAIL;
}

/*******************************************************************************
* osShellPrintf
*
* description:
*       Write a formatted string to the shell's output stream.
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
int osShellPrintf(const char* format, ...)
{
    char buff[2048];
    va_list args;
    int i;

    va_start(args, format);
    i = vsprintf(buff, format, args);
    va_end(args);

	return cmdOsPrintf("%s", buff);
}


