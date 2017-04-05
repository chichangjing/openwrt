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
* lsemLib.c
*
* DESCRIPTION:
*       Pthread implementation of mainOs semaphores
*
* DEPENDENCIES:
*       POSIX pthreads
*
* FILE REVISION NUMBER:
*       $Revision: 9 $
*
*******************************************************************************/

#define _GNU_SOURCE
#include <gtOs/gtOsSem.h>

#include <errno.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <limits.h>
#include <pthread.h>

#include <gtOs/osObjIdLib.h>
#include <gtOs/gtOsSharedData.h>

#include "mainOsPthread.h"

/************* Defines ********************************************************/

#ifndef CPSS_CLOCK
#define CPSS_CLOCK CLOCK_REALTIME
#endif

#define SEM_ALLOC_CHUNK 32
#define SEM_STAT

/************* Internal types *************************************************/
typedef enum {
    SEMTYPE_NONE = 0,
    SEMTYPE_MUTEX,
    SEMTYPE_COUNTING,
    SEMTYPE_BINARY
} semType;

typedef struct _V2L_semSTC
{
    OS_OBJECT_HEADER_STC    header;
    int                     count;
    pthread_mutex_t         mtx;
    pthread_cond_t          cond;
#ifdef  SEM_STAT
    int                     sig_count;
    int                     wait_count;
    int                     waiting;
    int                     timeout_count;
#endif
} _V2L_semSTC;


/************* Internal data **************************************************/

static OS_OBJECT_LIST_STC semaphores __SHARED_DATA_MAINOS = {
    NULL,                   /* list */
    0,                      /* allocated */
    SEM_ALLOC_CHUNK,        /* allocChunk */
    sizeof(_V2L_semSTC),    /* objSize */
    V2L_ts_malloc,          /* allocFuncPtr */
    V2L_ts_free             /* freeFuncPtr */
};

#ifndef SHARED_MEMORY
static pthread_mutex_t semaphores_mtx = PTHREAD_MUTEX_INITIALIZER;
#else
static int initialized __SHARED_DATA_MAINOS = 0;
static pthread_mutex_t semaphores_mtx __SHARED_DATA_MAINOS;
#endif


/************ Forward declararions ********************************************/
static int V2L_semOpenExisting
(
    IN  const char  *name,
    IN  semType     type
);


/************ Public Functions ************************************************/

/*******************************************************************************
* V2L_lsemInit
*
* DESCRIPTION:
*       Initialize semaphores
*
* INPUTS:
*       None
*
* OUTPUTS:
*       None
*
* RETURNS:
*       OK         - on success
*       ERROR      - on error
*
* COMMENTS:
*       This function called from osStartEngine()
*
*******************************************************************************/
int V2L_lsemInit(void)
{
#ifdef SHARED_MEMORY
    if (!initialized)
    {
        CREATE_MTX(&semaphores_mtx);
        initialized = 1;
    }
#endif
    return 0;
}


/*******************************************************************************
* CREATE_SEM
*
* DESCRIPTION:
*       creates a semaphore.
*       Open an existing one if openExisting != 0
*
* INPUTS:
*       type       - semaphore type
*       name       - semaphore name
*       openExisting - flag
*
* OUTPUTS:
*       semId      - semaphore Id
*       sem        - semaphore struct pointer
*
* RETURNS:
*       ERROR      - on error
*       semaphoreId if already exists
*
* COMMENTS:
*       None
*
*******************************************************************************/
#define CREATE_SEM(type, openExisting) \
    int semId; \
    OS_OBJECT_HEADER_STC *h; \
    _V2L_semSTC *sem; \
    pthread_mutex_lock(&semaphores_mtx); \
    if (openExisting) \
    { \
        semId = V2L_semOpenExisting(name, type); \
        if (semId > 0) \
        { \
            pthread_mutex_unlock(&semaphores_mtx); \
            *smid = (GT_SEM)semId; \
            return GT_OK; \
        } \
        if (semId < 0) \
        { \
            return GT_FAIL; \
        } \
    } \
    semId = osObjLibGetNewObject(&semaphores, type, name, &h); \
    sem = (_V2L_semSTC*)h; \
    pthread_mutex_unlock(&semaphores_mtx);





/*******************************************************************************
* osSemBinCreate
*
* DESCRIPTION:
*       Create and initialize a binary semaphore.
*
* INPUTS:
*       name   - semaphore Name
*       init   - init value of semaphore (full or empty)
*
* OUTPUTS:
*       smid - semaphore Id
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on error
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS osSemBinCreate
(
    IN  const char    *name,
    IN  GT_SEMB_STATE init,
    OUT GT_SEM        *smid
)
{
    CREATE_SEM(SEMTYPE_BINARY, 0);

    /* initialize */
    CREATE_MTX(&(sem->mtx));
    CREATE_COND(&(sem->cond));

    sem->count = (init == OS_SEMB_FULL) ? 1 : 0;
    *smid = (GT_SEM)semId;
    return GT_OK;
}

/*******************************************************************************
* osSemMCreate
*
* DESCRIPTION:
*       Create and initialize a mutext semaphore.
*
* INPUTS:
*       name   - semaphore Name
*
* OUTPUTS:
*       smid - semaphore Id
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on error
*
* COMMENTS:
*       This is not API. Should be used as development tool only.
*       Will be removed.
*
*******************************************************************************/
GT_STATUS osSemMCreate
(
    IN  const char    *name,
    OUT GT_SEM        *smid
)
{
    pthread_mutexattr_t mattr;
    CREATE_SEM(SEMTYPE_MUTEX, 0);

    /* initialize recursive mutex */
    pthread_mutexattr_init(&mattr);
#ifdef SHARED_MEMORY
    pthread_mutexattr_setpshared(&mattr, PTHREAD_PROCESS_SHARED);
#endif
    pthread_mutexattr_settype(&mattr, PTHREAD_MUTEX_RECURSIVE);
    pthread_mutex_init(&(sem->mtx), &mattr);
    pthread_mutexattr_destroy(&mattr);
    *smid = (GT_SEM)semId;
    return GT_OK;
}

/*******************************************************************************
* osSemCCreate
*
* DESCRIPTION:
*       Create counting semaphore.
*
* INPUTS:
*       name   - semaphore Name
*       init   - init value of semaphore
*
* OUTPUTS:
*       smid - semaphore Id
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on error
*
*******************************************************************************/
GT_STATUS osSemCCreate
(
    IN  const char    *name,
    IN  GT_U32        init,
    OUT GT_SEM        *smid
)
{
    CREATE_SEM(SEMTYPE_COUNTING, 0);

    /* initialize */
    CREATE_MTX(&(sem->mtx));
    CREATE_COND(&(sem->cond));

    sem->count = init;
    *smid = (GT_SEM)semId;
    return GT_OK;
}

/*******************************************************************************
* osSemMOpenNamed
*
* DESCRIPTION:
*       Create or open an existing named mutex semaphore.
*
* INPUTS:
*       name   - semaphore Name
*
* OUTPUTS:
*       smid - semaphore Id
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on error
*
* COMMENTS:
*       Works only with kernel module semaphores
*
*******************************************************************************/
GT_STATUS osSemMOpenNamed
(
    IN  const char    *name,
    OUT GT_SEM        *smid
)
{
    CREATE_SEM(SEMTYPE_MUTEX, 1);

    /* initialize */
    CREATE_MTX(&(sem->mtx));

    *smid = (GT_SEM)semId;
    return GT_OK;
}

/*******************************************************************************
* osSemBinOpenNamed
*
* DESCRIPTION:
*       Create or open existing named binary semaphore.
*
* INPUTS:
*       name   - semaphore Name
*       init   - init value of semaphore (full or empty)
*
* OUTPUTS:
*       smid - semaphore Id
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on error
*
* COMMENTS:
*       Works only with kernel module semaphores
*
*******************************************************************************/
GT_STATUS osSemBinOpenNamed
(
    IN  const char    *name,
    IN  GT_SEMB_STATE init,
    OUT GT_SEM        *smid
)
{
    CREATE_SEM(SEMTYPE_BINARY, 1);

    /* initialize */
    CREATE_MTX(&(sem->mtx));
    CREATE_COND(&(sem->cond));

    sem->count = (init == OS_SEMB_FULL) ? 1 : 0;
    *smid = (GT_SEM)semId;
    return GT_OK;
}

/*******************************************************************************
* osSemCOpenNamed
*
* DESCRIPTION:
*       Create or open existing named counting semaphore.
*
* INPUTS:
*       name   - semaphore Name
*       init   - init value of semaphore
*
* OUTPUTS:
*       smid - semaphore Id
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on error
*
* COMMENTS:
*       Works only with kernel module semaphores
*
*******************************************************************************/
GT_STATUS osSemCOpenNamed
(
    IN  const char    *name,
    IN  GT_U32        init,
    OUT GT_SEM        *smid
)
{
    CREATE_SEM(SEMTYPE_COUNTING, 0);

    /* initialize */
    CREATE_MTX(&(sem->mtx));
    CREATE_COND(&(sem->cond));

    sem->count = init;
    *smid = (GT_SEM)semId;
    return GT_OK;
}

/*******************************************************************************
* CHECK_SEMID
*
* DESCRIPTION:
*       Check semaphore Id
*
* INPUTS:
*       semId      - semaphore Id
*
* OUTPUTS:
*       sem        - semaphore struct pointer
*
* RETURNS:
*       ERROR      - on error
*
* COMMENTS:
*       None
*
*******************************************************************************/
#define CHECK_SEMID(semId) \
    _V2L_semSTC *sem; \
    if ((int)semId < 1 || (int)semId >= semaphores.allocated) \
        return GT_FAIL; \
    sem = (_V2L_semSTC*)(semaphores.list[(int)semId]); \
    if (!sem || !sem->header.type) \
        return GT_FAIL;

/*******************************************************************************
* osSemDelete
*
* DESCRIPTION:
*       Delete semaphore.
*
* INPUTS:
*       smid - semaphore Id
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on error
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS osSemDelete
(
    IN GT_SEM smid
)
{
    CHECK_SEMID(smid);
    switch (sem->header.type)
    {
        case SEMTYPE_NONE:
            break;
        case SEMTYPE_MUTEX:
            sem->header.type = SEMTYPE_NONE;
            pthread_mutex_unlock(&(sem->mtx));
            pthread_mutex_destroy(&(sem->mtx));
            break;
        case SEMTYPE_BINARY:
        case SEMTYPE_COUNTING:
            sem->header.type = SEMTYPE_NONE;
            pthread_cond_broadcast(&(sem->cond));
            pthread_mutex_destroy(&(sem->mtx));
            pthread_cond_destroy(&(sem->cond));
            break; 
    }
    sem->header.type = SEMTYPE_NONE;
    return GT_OK;
}

/*******************************************************************************
* osSemWait
*
* DESCRIPTION:
*       Wait on semaphore.
*
* INPUTS:
*       smid    - semaphore Id
*       timeOut - time out in miliseconds or OS_WAIT_FOREVER to wait forever
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on error
*       OS_TIMEOUT - on time out
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS osSemWait
(
    IN GT_SEM smid,
    IN GT_U32 timeOut
)
{
    CHECK_SEMID(smid);

    if (sem->header.type == SEMTYPE_MUTEX)
    {
        pthread_mutex_lock(&(sem->mtx));
#ifdef SEM_STAT
        sem->wait_count++;
#endif
        return GT_OK;
    }


    pthread_mutex_lock(&(sem->mtx));
    if (sem->count)
    {
        /* wait successful */
        sem->count--;
#ifdef SEM_STAT
        sem->wait_count++;
#endif
        pthread_mutex_unlock(&(sem->mtx));
        return GT_OK;
    }
    if (timeOut == OS_NO_WAIT)
    {
        /* no wait */
#ifdef SEM_STAT
        sem->wait_count++;
#endif
        pthread_mutex_unlock(&(sem->mtx));
        return GT_FAIL;
    }

#ifdef SEM_STAT
    sem->waiting++;
#endif
    if (timeOut == OS_WAIT_FOREVER)
    {
        /* wait forever */
        while (sem->count == 0)
        {
            pthread_cond_wait(&(sem->cond), &(sem->mtx));
            if (!sem->header.type)
            {
                pthread_mutex_unlock(&(sem->mtx));
                return GT_FAIL;
            }
        }
    }
    else
    {
        /* wait no more than timeOut milliseconds */
        struct timespec timeout, now;

        milliseconds2timespec(timeOut, &timeout);
        clock_gettime(CPSS_CLOCK, &now);
        timespec_add(&timeout, &now);

        while (sem->count == 0 && timespec_gt(&timeout, &now))
        {
            pthread_cond_timedwait(&(sem->cond), &(sem->mtx), &timeout);
            if (!sem->header.type)
            {
                pthread_mutex_unlock(&(sem->mtx));
                return GT_FAIL;
            }
            if (sem->count == 0)
                clock_gettime(CPSS_CLOCK, &now);
        }
        if (sem->count == 0)
        {
            /* wait failed (timeout) */
#ifdef SEM_STAT
            sem->waiting--;
            sem->timeout_count++;
#endif
            pthread_mutex_unlock(&(sem->mtx));
            return GT_TIMEOUT;
        }
    }
    /* wait successful */
#ifdef SEM_STAT
    sem->waiting--;
#endif
    sem->count--;
    pthread_mutex_unlock(&(sem->mtx));
    return GT_OK;
}

/*******************************************************************************
* osSemSignal
*
* DESCRIPTION:
*       Signal a semaphore.
*
* INPUTS:
*       smid    - semaphore Id
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on error
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS osSemSignal
(
    IN GT_SEM smid
)
{
    CHECK_SEMID(smid);
    if (sem->header.type == SEMTYPE_MUTEX)
    {
#ifdef SEM_STAT
        sem->sig_count++;
#endif
        pthread_mutex_unlock(&(sem->mtx));
        return GT_OK;
    }
    pthread_mutex_lock(&(sem->mtx));
#ifdef SEM_STAT
    sem->sig_count++;
#endif
    sem->count++;
    if (sem->header.type == SEMTYPE_BINARY && sem->count > 1)
        sem->count = 1;
    pthread_cond_signal(&(sem->cond));
    pthread_mutex_unlock(&(sem->mtx));

    return GT_OK;
}


/*******************************************************************************
* osMutexCreate
*
* DESCRIPTION:
*       Create and initialize a Mutex object.
*
* INPUTS:
*       name   - mutex Name
*
* OUTPUTS:
*       mtxid - mutex Id
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on error
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS osMutexCreate
(
    IN  const char      *name,
    OUT GT_MUTEX        *mtxid
)
{
    return osSemMCreate(name, (GT_SEM*)mtxid);
}

/*******************************************************************************
* osMutexDelete
*
* DESCRIPTION:
*       Delete mutex.
*
* INPUTS:
*       mtxid - mutex Id
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on error
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS osMutexDelete
(
    IN GT_MUTEX mtxid
)
{
    return osSemDelete((GT_SEM)mtxid);
}

/*******************************************************************************
* osMutexLock
*
* DESCRIPTION:
*       Lock a mutex.
*
* INPUTS:
*       mtxid - mutex Id
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on error
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS osMutexLock
(
    IN GT_MUTEX mtxid
)
{
    return osSemWait((GT_SEM)mtxid, OS_WAIT_FOREVER);
}

/*******************************************************************************
* osMutexUnlock
*
* DESCRIPTION:
*       Unlock a mutex.
*
* INPUTS:
*       mtxid - mutex Id
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on error
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS osMutexUnlock
(
    IN GT_MUTEX mtxid
)
{
    return osSemSignal((GT_SEM)mtxid);
}

#ifdef SEM_STAT
/*******************************************************************************
* V2L_semStat
*
* DESCRIPTION:
*       Prints semaphore statistics
*
* INPUTS:
*       None
*
* OUTPUTS:
*       None
*
* RETURNS:
*       0 == GT_OK
*
* COMMENTS:
*       None
*
*******************************************************************************/
int V2L_semStat(void)
{
    int semId;
    for (semId = 1; semId < semaphores.allocated; semId++)
    {
        _V2L_semSTC *sem;
        sem = (_V2L_semSTC*)(semaphores.list[semId]);
        if (!sem)
            continue;
        if (!sem->header.type)
            continue;
        printf("id=%d name=\"%s\" type=%c sig=%d waits=%d timeouts=%d waiting=%d\n",
                semId, sem->header.name, "nmcb"[sem->header.type],
                sem->sig_count, sem->wait_count,
                sem->timeout_count, sem->waiting);
    }
    return 0;
}
#endif

/*******************************************************************************
* osMutexStat
*
* DESCRIPTION:
*       Prints userspace mutex statistics
*
* INPUTS:
*       None
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK   - on success
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS osMutexStat(void)
{
#ifdef SEM_STAT
    V2L_semStat();
#endif
    return GT_OK;
}


/************ Internal Functions **********************************************/

/*******************************************************************************
* V2L_semOpenExisting
*
* DESCRIPTION:
*       Try to open an existing semaphore of given type
*
* INPUTS:
*       name        - semaphore name
*       type        - semaphore type
*
* OUTPUTS:
*       None
*
* RETURNS:
*       semaphore Id
*       0 if semaphore doesn't exists
*       < 0 if semaphore with give name has different type
*
* COMMENTS:
*       None
*
*******************************************************************************/
static int V2L_semOpenExisting
(
    IN  const char  *name,
    IN  semType     type
)
{
    int semId;
    _V2L_semSTC *sem = NULL;

    /* try to find existing semaphore first */
    for (semId = 1; semId < semaphores.allocated; semId++)
    {
        sem = (_V2L_semSTC*)(semaphores.list[semId]);
        if (!sem || !sem->header.type)
            continue;

        if (!strncmp(sem->header.name, name, sizeof(sem->header.name)))
            break;
    }

    if (semId < semaphores.allocated)
    {
        /* found */
        return (sem->header.type != type) ? -1 : semId;
    }
    return 0;
}

