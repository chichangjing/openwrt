/*******************************************************************************
*              (c), Copyright 2007, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* os_vxShell.c
*
* DESCRIPTION:
*       This file contains OS shell function
*
* FILE REVISION NUMBER:
*       $Revision: 5$
*
*******************************************************************************/
#include <cmdShell/shell/cmdConsole.h>
#include <cmdShell/os/cmdShell.h>
#include <cmdShell/os/cmdStream.h>

#if (CPU != PPC604) && (CPU != PPC603)
#include <ioLib.h>
#include <shellLib.h>
#include <taskLib.h>
#include <fcntl.h>
#if (defined (_WRS_VXWORKS_MAJOR) && (_WRS_VXWORKS_MAJOR >= 6))
#include <private/excLibP.h>
#else
#include <remLib.h>
#include <sysLib.h>
#endif

#ifndef REMOTE_START
# define REMOTE_START 0
#endif
#ifndef REMOTE_STOP
# define REMOTE_STOP 1
#endif

#ifdef  CMD_LUA_CLI
extern GT_STATUS luaCLI_EventLoop(cmdStreamPTR IOStream);
#endif

static int shellStatus = 0;
static GT_SEM shellExitSem;
static cmdStreamPTR iostream = NULL;
static int fdoptions = 0;

static void shellExitJob(void)
{
    /* This routine is called as a result of the logout() command
     * being issued from the shell.  It is therefore run in the context
     * of the shell.
     */
#if (!defined (_WRS_VXWORKS_MAJOR) || (_WRS_VXWORKS_MAJOR < 6))
    shellParserControl(REMOTE_STOP, 0, 0);
#else
    shellParserControl(REMOTE_STOP, 0, 0, NULL);
#endif
    shellLogoutInstall ((FUNCPTR) NULL, 0);  /* uninstall logout function */

    /* unblock cmdOsShell() */
    cmdOsSigSemSignal(shellExitSem);
}


static void shellExit(void)
{
#if (!defined (_WRS_VXWORKS_MAJOR) || (_WRS_VXWORKS_MAJOR < 6))
    excJobAdd ((VOIDFUNCPTR) shellExitJob, 0, 0, 0, 0, 0, 0);
    taskDelay (sysClkRateGet());
#else
    jobAdd ((FUNCPTR) shellExitJob, 0, 0, 0, 0, 0, 0, NULL);
#endif
}

#ifdef  CMD_LUA_CLI
int luaCLI(void)
{
    int fdoptions_shell;
    int fd;
    if (!iostream)
    {
        cmdPrintf("Shell is not executed\n");
        return 1;
    }
    fd = iostream->getFd(iostream);
    fdoptions_shell = ioctl (fd, FIOGETOPTIONS, 0);
    ioctl (fd, FIOSETOPTIONS, fdoptions);
    luaCLI_EventLoop(iostream);
    ioctl (fd, FIOSETOPTIONS, fdoptions_shell);
    return 0;
}
#endif

/*******************************************************************************
* cmdOsShell
*
* DESCRIPTION:
*       Start OS shell
*       It is assumed that stdout is redirected to current stream
*
* INPUTS:
*       IOStream  - I/O stream
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
GT_STATUS cmdOsShell(cmdStreamPTR IOStream)
{
    int fd;
    int result;

    if (shellStatus != 0)
    {
        cmdPrintf("shell is already executed\n");
        shellStatus = 0;
        return GT_FAIL;
    }
    shellStatus = 1;
    if (cmdOsSigSemBinCreate("osShellExitSem", CPSS_OS_SEMB_EMPTY_E, &shellExitSem) != GT_OK)
    {
        cmdPrintf("failed to create semaphore\n");
        shellStatus = 0;
        return GT_FAIL;
    }

    /* run on tty device only (UART, PTY) */
    fd = (IOStream->getFd) ? IOStream->getFd(IOStream) : -1;
    if (fd < 0 || !isatty(fd))
    {
        cmdPrintf("cmdOsShell will run on a TTY only\n");
err_exit:
        cmdOsSigSemDelete(shellExitSem);
        shellStatus = 0;
        iostream = NULL;
        return GT_FAIL;
    }
    iostream = IOStream;

    cmdPrintf("starting shell()...\n");
    /* save TTY options */
    fdoptions = ioctl (fd, FIOGETOPTIONS, 0);

    shellLoginInstall((FUNCPTR)NULL, 0);
#if (!defined (_WRS_VXWORKS_MAJOR) || (_WRS_VXWORKS_MAJOR < 6))
    result = shellParserControl(REMOTE_START, 0, fd);
#else
    result = shellParserControl(REMOTE_START, 0, fd, shellExit);
#endif

    if (result == ERROR)
    {
        cmdPrintf("Failure in shellParserControl(REMOTE_START,...)\n");
        ioctl (fd, FIOSETOPTIONS, fdoptions);
        goto err_exit;
    }

#if (!defined (_WRS_VXWORKS_MAJOR) || (_WRS_VXWORKS_MAJOR < 6))
    shellLogoutInstall((FUNCPTR)shellExit, 0);
#endif
    
    cmdOsSigSemWait(shellExitSem, CPSS_OS_SEM_WAIT_FOREVER_CNS);
    cmdOsSigSemDelete(shellExitSem);
    /* restore TTY options */
    ioctl (fd, FIOSETOPTIONS, fdoptions);

    iostream = NULL;
    shellStatus = 0;

    cmdPrintf("shell() done, return to GalTis shell\n");


    return GT_OK;
}
#else /* (CPU == PPC604) || (CPU == PPC603) */
/*******************************************************************************
* cmdOsShell
*
* DESCRIPTION:
*       Start OS shell
*       It is assumed that stdout is redirected to current stream
*
* INPUTS:
*       IOStream  - I/O stream
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
GT_STATUS cmdOsShell(cmdStreamPTR IOStream)
{
    cmdPrintf("cmdOsShell is disabled for this CPU\n");
    return GT_FAIL;
}
#endif /* (CPU == PPC604) || (CPU == PPC603) */
