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
* ltest.c
*
* DESCRIPTION:
*       Pthread implementation of mainOs tasks and semaphores
*       performance tests
*
* DEPENDENCIES:
*       POSIX pthreads
*
* FILE REVISION NUMBER:
*       $Revision: 4 $
*
*******************************************************************************/

#define _GNU_SOURCE
#include <sched.h>
#include <time.h>
#include <pthread.h>
#include <stdio.h>

#include "mainOsPthread.h"
#include <gtOs/gtOsSem.h>
#include <gtOs/gtOsTask.h>

#ifndef CPSS_CLOCK
#define CPSS_CLOCK CLOCK_REALTIME
#endif


#define PERF_COUNT 1000000

#define TIMING_START() \
    clock_gettime(CPSS_CLOCK, &start);

#define TIMING_PRINT(desc) \
    clock_gettime(CPSS_CLOCK, &now); \
    timespec_sub(&now, &start, &delta); \
    fprintf(stderr, "%s: count=%d time=%d.%09d seconds\n", \
            desc, PERF_COUNT, delta.tv_sec, delta.tv_nsec);

int V2L_testperf()
{
    int k;
    struct timespec start, now, delta;
    pthread_mutex_t mtx;
    GT_SEM      sem;
    GT_MUTEX    osmtx;

    TIMING_START();
    for (k = 0; k < PERF_COUNT; k++)
    {
        osTaskLock();
        osTaskUnLock();
    }
    TIMING_PRINT("osTaskLock/osTaskUnLock");

    pthread_mutex_init(&mtx,NULL);
    TIMING_START();
    for (k = 0; k < PERF_COUNT; k++)
    {
        pthread_mutex_lock(&mtx);
        pthread_mutex_unlock(&mtx);
    }
    TIMING_PRINT("pthread mutex lock/unlock (single thread)");
    pthread_mutex_destroy(&mtx);

    osMutexCreate("testPerf", &osmtx);
    TIMING_START();
    for (k = 0; k < PERF_COUNT; k++)
    {
        osMutexLock(osmtx);
        osMutexUnlock(osmtx);
    }
    TIMING_PRINT("osMutexLock/osMutexUnlock (single thread)");
    osMutexDelete(osmtx);

    osSemMCreate("testPerf", &sem);
    TIMING_START();
    for (k = 0; k < PERF_COUNT; k++)
    {
        osSemWait(sem, OS_WAIT_FOREVER);
        osSemSignal(sem);
    }
    TIMING_PRINT("mutex semaphore wait/signal (single thread)");
    osSemDelete(sem);

    osSemBinCreate("testPerf",OS_SEMB_FULL, &sem);
    TIMING_START();
    for (k = 0; k < PERF_COUNT; k++)
    {
        osSemWait(sem, OS_WAIT_FOREVER);
        osSemSignal(sem);
    }
    TIMING_PRINT("binary semaphore wait/signal (single thread)");
    osSemDelete(sem);

    osSemBinCreate("testPerf", OS_SEMB_EMPTY, &sem);
    TIMING_START();
    osSemWait(sem, 1000);
    TIMING_PRINT("wait_timeout semaphore 1000ms");
    osSemDelete(sem);


    return 0;
}

