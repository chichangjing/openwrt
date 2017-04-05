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
*       performance tests
*
* DEPENDENCIES:
*       POSIX pthreads, mvKernelExt
*
* FILE REVISION NUMBER:
*       $Revision: 4 $
*
*******************************************************************************/

#define _GNU_SOURCE
#include "kernelExt.h"
#include <sched.h>
#include <sys/types.h>
#include <time.h>
#include <pthread.h>
#include <sys/ioctl.h>
#include <stdio.h>

#include <gtOs/gtOsSem.h>
#include <gtOs/gtOsTask.h>

#ifndef CPSS_CLOCK
#define CPSS_CLOCK CLOCK_REALTIME
#endif


void timespec_sub(struct timespec *a, struct timespec *b, struct timespec *delta);

#define PERF_COUNT 1000000

#define TIMING_START() \
    clock_gettime(CPSS_CLOCK, &start);

#define TIMING_PRINT(desc) \
    clock_gettime(CPSS_CLOCK, &now); \
    timespec_sub(&now, &start, &delta); \
    fprintf(stderr, "%s: count=%d time=%d.%09d seconds\n", \
            desc, PERF_COUNT, delta.tv_sec, delta.tv_nsec);




extern int gtPpFd;
#ifdef  GT_SMIa
extern unsigned int hwIfSmiReadReg(
        unsigned int devSlvId,
        unsigned int regAddr,
        unsigned int *dataPtr);
extern unsigned int hwIfSmiWriteReg(
        unsigned int devSlvId,
        unsigned int regAddr,
        unsigned int value
);
#endif
int mv_testperf()
{
    int k;
    struct timespec start,now,delta;
    pthread_mutex_t mtx;
    GT_SEM  sem;

    TIMING_START();
    for (k = 0; k < PERF_COUNT; k++)
        ioctl(mvKernelExtFd, MVKERNELEXT_IOC_NOOP);
    TIMING_PRINT("noop ioctl");

#ifdef MVKERNELEXT_SYSCALLS
    TIMING_START();
    for (k = 0; k < PERF_COUNT; k++)
        mv_ctrl(MVKERNELEXT_IOC_NOOP, 0);
    TIMING_PRINT("noop ioctl through syscall");
#endif

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

    osSemBinCreate("testPerf", OS_SEMB_FULL, &sem);
    TIMING_START();
    for (k = 0; k < PERF_COUNT; k++)
    {
        osSemWait(sem, OS_WAIT_FOREVER);
        osSemSignal(sem);
    }
    TIMING_PRINT("osSemWait/osSemSignal on binary semaphore");
    osSemDelete(sem);

    osSemBinCreate("testPerf", OS_SEMB_EMPTY, &sem);
    TIMING_START();
    osSemWait(sem, 1000);
    TIMING_PRINT("wait_timeout semaphore 1000ms");
    osSemDelete(sem);

#ifdef  GT_SMIa
    TIMING_START();
    for (k = 0; k < PERF_COUNT; k++)
    {
        unsigned int value;
        hwIfSmiReadReg(23, 0xb002000, &value);
    }
    TIMING_PRINT("hwIfSmiReadReg(23,0xb002000,&value)");

    TIMING_START();
    for (k = 0; k < PERF_COUNT; k++)
    {
        hwIfSmiWriteReg(23, 0xb002000, 0);
    }
    TIMING_PRINT("hwIfSmiWriteReg(23,0xb002000,0)");
#endif

    return 0;
}
