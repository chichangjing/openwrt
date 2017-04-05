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
* lkernelLib.c
*
* DESCRIPTION:
*       Pthread implementation of mainOs tasks and semaphores
*       Initialization and common functions
*
* DEPENDENCIES:
*       POSIX pthreads
*
* FILE REVISION NUMBER:
*       $Revision: 4 $
*
*******************************************************************************/
#define _GNU_SOURCE

#include <gtOs/gtOsInit.h>

#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#if 0
#include <sys/mman.h>
#endif

#ifdef SHARED_MEMORY
#include <gtOs/gtOsSharedMalloc.h>
#endif
#include "mainOsPthread.h"

#ifndef PTHREAD_ADAPTIVE_MUTEX_INITIALIZER_NP
#define PTHREAD_ADAPTIVE_MUTEX_INITIALIZER_NP PTHREAD_MUTEX_INITIALIZER
#endif

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

/************ Public Functions ************************************************/

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
    setbuf(stdout, NULL);


#if 0
    /*
    **  Lock all memory pages associated with this process to prevent delays
    **  due to process (or thread) memory being swapped out to disk and back.
    */
    mlockall( (MCL_CURRENT) );
#endif

    V2L_ltaskInit(name);
    V2L_lsemInit();
    V2L_lmsgQInit();

    rootTask = pthread_self();
    programState = E_STATE_INITIALIZED;
    signal(SIGINT, sigStopHandler);

    if (entryPoint)
    {
        printf( "\r\nStarting System Root Task" );
        entryPoint();
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


/************ Private Functions ***********************************************/

/*******************************************************************************
* V2L_ts_malloc
*
* DESCRIPTION:
*       thread-safe malloc
*
* INPUTS:
*       blksize    - number of bytes to allocate
*
* OUTPUTS:
*       None
*
* RETURNS:
*       Pointer to allocated buffer or NULL
*
* COMMENTS:
*       None
*
*******************************************************************************/
void* V2L_ts_malloc
(
    IN  size_t blksize
)
{
    void *blkaddr;
    static pthread_mutex_t
        malloc_lock = PTHREAD_ADAPTIVE_MUTEX_INITIALIZER_NP;

    pthread_cleanup_push( (void(*)(void *))pthread_mutex_unlock,
                          (void *)&malloc_lock );
    pthread_mutex_lock( &malloc_lock );

#ifdef SHARED_MEMORY
    blkaddr = SHARED_MALLOC_MAC( blksize );
#else
    blkaddr = malloc( blksize );
#endif

    pthread_cleanup_pop( 1 );

    return( blkaddr );
}
    
/*******************************************************************************
* V2L_ts_free
*
* DESCRIPTION:
*       thread-safe free
*
* INPUTS:
*       blkaddr    - address previosly allocated buffer
*
* OUTPUTS:
*       None
*
* RETURNS:
*       None
*
* COMMENTS:
*       None
*
*******************************************************************************/
void V2L_ts_free
(
    IN  void *blkaddr
)
{
    static pthread_mutex_t
        free_lock = PTHREAD_ADAPTIVE_MUTEX_INITIALIZER_NP;

    pthread_cleanup_push( (void(*)(void *))pthread_mutex_unlock,
                          (void *)&free_lock );
    pthread_mutex_lock( &free_lock );

#ifdef SHARED_MEMORY
    SHARED_FREE_MAC( blkaddr );
#else
    free( blkaddr );
#endif

    pthread_cleanup_pop( 1 );
}

