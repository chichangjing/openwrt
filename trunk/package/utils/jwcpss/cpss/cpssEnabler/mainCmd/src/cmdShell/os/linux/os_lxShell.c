/*******************************************************************************
*              (c), Copyright 2007, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* os_lxShell.c
*
* DESCRIPTION:
*       This file contains OS shell function
*
* FILE REVISION NUMBER:
*       $Revision: 2$
*
*******************************************************************************/
#include <cmdShell/shell/cmdConsole.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/wait.h>

extern pid_t vfork(void);

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
GT_STATUS cmdOsShell(IOStreamPTR IOStream)
{
    int fd;
    pid_t cpid;
    int status;

    /* run on tty device only (UART, PTY) */
    fd = (IOStream->getFd) ? IOStream->getFd(IOStream) : -1;
    if (fd < 0 || !isatty(fd))
    {
        cmdPrintf("cmdOsShell will run on a TTY only\n");
        return GT_FAIL;
    }

    cpid = vfork();
    if (cpid < 0)
    {
        cmdPrintf("vfork() failed: %s\n", strerror(errno));
    }
    if (cpid == 0)
    {
        /* child */

        /* redirect std */
        dup2(fd, 0);
        dup2(fd, 1);
        dup2(fd, 2);

        execl("/bin/sh", "sh", NULL);
        if (write(1, "Failed to exec /bin/sh\n", 23) < 0)
        {
            fprintf(stderr, "write() : %s\n", strerror(errno));
        }
        _exit(1);
    }
    do {
        if (waitpid(cpid, &status, 0) < 0)
        {
            perror("waitpid()");
            return GT_FAIL;
        }
    } while (!WIFEXITED(status) && !WIFSIGNALED(status));

    return GT_OK;
}
