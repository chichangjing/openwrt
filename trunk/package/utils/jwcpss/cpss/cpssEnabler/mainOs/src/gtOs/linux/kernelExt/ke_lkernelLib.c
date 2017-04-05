/*******************************************************************************
*              Copyright 2010, Marvell Israel, LTD.
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
*  kernelLib.c
*
* DESCRIPTION:
*                Defines the functions and data structures needed
 *               to initialize a virtual machine in a POSIX Threads
 *               + mvKernelExt environment.
*
* DEPENDENCIES:
*       POSIX pthreads
*
* FILE REVISION NUMBER:
*       $Revision: 7 $
*
*******************************************************************************/
#define _GNU_SOURCE
#include "kernelExt.h"
#include <gtOs/gtOsInit.h>

#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <string.h>
#include <signal.h>
#include <pthread.h>
#include <errno.h>
#include <sys/ioctl.h>


#if defined(MVKERNELEXT_SYSCALLS)
# if defined(_syscall0)
    _syscall2(int, mv_ctl, unsigned int, cmd, unsigned long, arg)
#   define mv_ctrl(cmd,arg)  mv_ctl((cmd),(unsigned long)(arg))
# endif
#endif

/***** Global Vars ********************************************/

int mvKernelExtFd = -1;
    
/************* Static variables ***********************************************/
static enum {
    E_STATE_UNINITIALIZED,
    E_STATE_INITIALIZED,
    E_STATE_WAITING,
    E_STATE_STOP
} programState = E_STATE_UNINITIALIZED;
static pthread_t rootTask;

/************* Static Function declarations ***********************************/
static void sigStopHandler(int);

/************* Functions ******************************************************/


GT_STATUS osOpenFd
(
)
{
    mv_registertask_stc rparam;

    mvKernelExtFd = open("/dev/mvKernelExt", O_RDONLY);
    if (mvKernelExtFd < 0)
    {
        perror("can't open /dev/mvKernelExt");
        sleep(20);
        exit(1);
    }
    return GT_OK;
}


/*******************************************************************************
* osStartEngine
*
* DESCRIPTION:
*       Initialize mainOs engine, then start entryPoint routine
*       Need to be inplemented for Linux/FreeBSD
*
* INPUTS:
*       argC        - main() parameter
*       argV        - main() parameter
*       name        - task name
*       entryPoint  - entry point function
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK on success
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS osStartEngine
(
    IN int argC,
    IN const char* argV[],
    IN const char* name,
    IN GT_VOIDFUNCPTR entryPoint
)
{
    mv_registertask_stc rparam;

    setbuf(stdout, NULL);


    mvKernelExtFd = open("/dev/mvKernelExt", O_RDONLY);
    if (mvKernelExtFd < 0)
    {
        perror("can't open /dev/mvKernelExt");
        sleep(20);
        exit(1);
    }

#if defined(KERNEL_SYSCALLS_INDIRECTaa)
    printf("0 gettid()=%d\n",syscall(SYS_gettid));
#if 0
    ioctl(mvKernelExtFd, MVKERNELEXT_IOC_NOOP);
    printf("%d mv_ctl(noop)=%d\n",227, syscall(227, MVKERNELEXT_IOC_NOOP));
    perror("x");
#endif
    printf("%d mv_ctl(noop)=%d\n",__NR_mv_ctl, syscall(__NR_mv_ctl, MVKERNELEXT_IOC_NOOP));
    perror("x");
#if 0
    printf("%d mv_ctl(noop)=%d\n",229, syscall(229, MVKERNELEXT_IOC_NOOP));
    perror("x");
#endif
    exit(0);
#endif
    /*
    **  Lock all memory pages associated with this process to prevent delays
    **  due to process (or thread) memory being swapped out to disk and back.
    */
    mlockall( (MCL_CURRENT) );

    /* register task in kernel */
    strncpy(rparam.name, name?name:"tUsrRoot", sizeof(rparam.name));
    rparam.vxw_priority = 0;
    rparam.pthread_id = pthread_self();
    mv_ctrl(MVKERNELEXT_IOC_REGISTER, &rparam);

    rootTask = pthread_self();
#if 1
    programState = E_STATE_INITIALIZED;

    signal(SIGINT, sigStopHandler);
#endif
    if (entryPoint)
    {
        printf( "\r\nStarting System Root Task" );
        entryPoint();
        printf( "\r\n herea !!\n" );
    }
#if 0
    programState = E_STATE_WAITING;
    while (programState != E_STATE_STOP)
    {
        pause();
    }
#endif
    printf( "\r\nExiting...\n" );

    return GT_OK;
}

/*******************************************************************************
* osStopEngine
*
* DESCRIPTION:
*       Stop engine
*       Need to be inplemented for Linux/FreeBSD
*
* INPUTS:
*       None
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS osStopEngine(void)
{
    if (programState == E_STATE_UNINITIALIZED)
    {
        /* already signalled or not started yet */
        return GT_FAIL;
    }
    pthread_kill(rootTask, SIGINT);
    return GT_OK;
}



/************* Static functions ***********************************************/
static void sigStopHandler(int sig)
{
    if (programState == E_STATE_WAITING)
    {
        programState = E_STATE_STOP;
    }
    else
    {
        exit(0);
    }
}
