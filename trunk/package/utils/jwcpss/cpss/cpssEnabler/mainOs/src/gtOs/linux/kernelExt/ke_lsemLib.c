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
* semLib.c
*
* DESCRIPTION:
 *            Defines the wrapper functions and data structures needed
 *            to implement semaphore API 
*
* DEPENDENCIES:
*       None
*
* FILE REVISION NUMBER:
*       $Revision: 6 $
*
*******************************************************************************/

#define _GNU_SOURCE
#include "kernelExt.h"
#include <gtOs/gtOsSem.h>
#include <gtOs/gtOsIo.h>

#include <string.h>
#include <pthread.h>
#include <stdlib.h>
#include <errno.h>
#include <gtOs/osObjIdLib.h>


#define OS_USERSPACE_MUTEX_STAT
#ifndef SHARED_MEMORY
#  define OS_USERSPACE_MUTEXES
#endif

#if defined(OS_USERSPACE_MUTEXES)

typedef struct _osMutex
{
    OS_OBJECT_HEADER_STC    header;
    pthread_mutex_t         mtx;
#ifdef  OS_USERSPACE_MUTEX_STAT
    int                     lockcount;
    int                     unlockcount;
#endif
}GT_LINUX_MUTEX;
#define LMUTEX_FLAG 0x8000000
#define IS_LOCAL_MUTEX(m) ((int)m & LMUTEX_FLAG)
#define SEM_TO_MTX(m) ((GT_MUTEX)((int)m & ~LMUTEX_FLAG))
#define MTX_TO_SEM(m) ((GT_SEM)((int)m | LMUTEX_FLAG))

static OS_OBJECT_LIST_STC userspaceMutexes = {
    NULL,                   /* list */
    0,                      /* allocated */
    128,                    /* allocChunk */
    sizeof(GT_LINUX_MUTEX), /* objSize */
    malloc,                 /* allocFuncPtr */
    free                    /* freeFuncPtr */
};

#define CHECK_MTX(mtxId) \
    GT_LINUX_MUTEX *mtx; \
    if ((int)mtxId < 1 || (int)mtxId >= userspaceMutexes.allocated) \
        return GT_FAIL; \
    mtx = (GT_LINUX_MUTEX*)(userspaceMutexes.list[(int)mtxId]); \
    if (!mtx || !mtx->header.type) \
        return GT_FAIL;

static pthread_mutex_t userspaceMutexes_mtx = PTHREAD_MUTEX_INITIALIZER;
#endif /* defined(OS_USERSPACE_MUTEXES) */


/************ Public Functions ************************************************/

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
    int ret;
    mv_sem_create_stc rparam;

    rparam.flags = (init == OS_SEMB_FULL) ? 1 : 0;
    rparam.flags |= MV_SEMAPTHORE_F_BINARY;
    if (name)
        strncpy(rparam.name, name, sizeof(rparam.name));
    else
        rparam.name[0] = 0;

    ret = mv_ctrl(MVKERNELEXT_IOC_SEMCREATE, &rparam);
    if (ret < 0)
    {
        *smid = 0;
        return GT_FAIL;
    }

    *smid = (GT_SEM)ret;

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
#if defined(OS_USERSPACE_MUTEXES)
    GT_STATUS   rc;
    GT_MUTEX    mtx;

    /* fix 'const' warning */
    char        tname[32];
    strncpy(tname, name?name:"", sizeof(tname));
    tname[sizeof(tname)-1] = 0;

    rc = osMutexCreate(tname, &mtx);
    if (rc == GT_OK)
        *smid = MTX_TO_SEM(mtx);
    else
        *smid = 0;
    return rc;
#else
    int ret;
    mv_sem_create_stc rparam;

    rparam.flags = MV_SEMAPTHORE_F_MTX;
    if (name)
        strncpy(rparam.name, name, sizeof(rparam.name));
    else
        rparam.name[0] = 0;

    ret = mv_ctrl(MVKERNELEXT_IOC_SEMCREATE, &rparam);
    if (ret < 0)
    {
        *smid = 0;
        return GT_FAIL;
    }

    *smid = (GT_SEM)ret;

    return GT_OK;
#endif
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
    int ret;
    mv_sem_create_stc rparam;

    rparam.flags = MV_SEMAPTHORE_F_COUNT | (init & MV_SEMAPTHORE_F_COUNT_MASK);
    if (name)
        strncpy(rparam.name, name, sizeof(rparam.name));
    else
        rparam.name[0] = 0;

    ret = mv_ctrl(MVKERNELEXT_IOC_SEMCREATE, &rparam);
    if (ret < 0)
    {
        *smid = 0;
        return GT_FAIL;
    }

    *smid = (GT_SEM)ret;

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
    int ret;
    mv_sem_create_stc rparam;

    rparam.flags = MV_SEMAPTHORE_F_MTX | MV_SEMAPTHORE_F_OPENEXIST;
    if (name)
        strncpy(rparam.name, name, sizeof(rparam.name));
    else
        rparam.name[0] = 0;

    ret = mv_ctrl(MVKERNELEXT_IOC_SEMCREATE, &rparam);
    if (ret < 0)
    {
        *smid = 0;
        return GT_FAIL;
    }

    *smid = (GT_SEM)ret;

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
    int ret;
    mv_sem_create_stc rparam;

    rparam.flags = (init == OS_SEMB_FULL) ? 1 : 0;
    rparam.flags |= MV_SEMAPTHORE_F_MTX | MV_SEMAPTHORE_F_OPENEXIST;
    if (name)
        strncpy(rparam.name, name, sizeof(rparam.name));
    else
        rparam.name[0] = 0;

    ret = mv_ctrl(MVKERNELEXT_IOC_SEMCREATE, &rparam);
    if (ret < 0)
    {
        *smid = 0;
        return GT_FAIL;
    }

    *smid = (GT_SEM)ret;

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
    int ret;
    mv_sem_create_stc rparam;

    rparam.flags = (init & MV_SEMAPTHORE_F_COUNT_MASK);
    rparam.flags |= MV_SEMAPTHORE_F_COUNT | MV_SEMAPTHORE_F_OPENEXIST;
    if (name)
        strncpy(rparam.name, name, sizeof(rparam.name));
    else
        rparam.name[0] = 0;

    ret = mv_ctrl(MVKERNELEXT_IOC_SEMCREATE, &rparam);
    if (ret < 0)
    {
        *smid = 0;
        return GT_FAIL;
    }

    *smid = (GT_SEM)ret;

    return GT_OK;
}

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
    int rc;

/* Supported for osSemMCreate . Should be removed */
#if defined(OS_USERSPACE_MUTEXES)
    if (IS_LOCAL_MUTEX((int)smid))
    {
        return osMutexDelete(SEM_TO_MTX(smid));
    }
#endif
    rc = mv_ctrl(MVKERNELEXT_IOC_SEMDELETE, smid);

    if (rc != 0)
        return GT_FAIL;

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
    int ret;

#if defined(OS_USERSPACE_MUTEXES)
    if (IS_LOCAL_MUTEX(smid))
    {
        return osMutexLock(SEM_TO_MTX(smid));
    }
#endif
    if (timeOut == OS_WAIT_FOREVER)
    {
        do {
            ret = mv_sem_wait((int)smid);
        } while (ret < 0 && errno == MVKERNELEXT_EINTR);
        return (ret == 0) ? GT_OK : GT_FAIL;
    }
    if (timeOut == OS_NO_WAIT)
    {
        return mv_sem_trywait((int)smid);
    }

    /* wait with timeout */
    do {
        mv_sem_timedwait_stc params;
        params.semid = (int)smid;
        params.timeout = timeOut;
        do {
            ret = mv_ctrl(MVKERNELEXT_IOC_SEMWAITTMO, &params);
        } while (ret < 0 && errno == MVKERNELEXT_EINTR);
    } while (ret < 0 && errno == MVKERNELEXT_EINTR);

    if (ret < 0 && errno == MVKERNELEXT_ETIMEOUT)
        return GT_TIMEOUT;

    return (ret == 0) ? GT_OK : GT_FAIL;
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
    int ret;

#if defined(OS_USERSPACE_MUTEXES)
    if (IS_LOCAL_MUTEX(smid))
    {
        return osMutexUnlock(SEM_TO_MTX(smid));
    }
#endif
    ret = mv_sem_signal((int)smid);
    
    if (ret != 0)
        return GT_FAIL;
    
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
/* OS_USERSPACE_MUTEXES not defined in shared-lib mode */
#ifndef OS_USERSPACE_MUTEXES
    return osSemMCreate(name, (GT_SEM*)mtxid);
#else
    int m;
    pthread_mutexattr_t mattr;
    OS_OBJECT_HEADER_STC *h;
    GT_LINUX_MUTEX *mtx;

    pthread_mutex_lock(&userspaceMutexes_mtx);
    m = osObjLibGetNewObject(&userspaceMutexes, 1, name, &h);
    mtx = (GT_LINUX_MUTEX*)h;
    if (m <= 0)
    {
        pthread_mutex_unlock(&userspaceMutexes_mtx);
        return GT_FAIL;
    }
    /* initialize recursive mutex */
    pthread_mutexattr_init(&mattr);
    pthread_mutexattr_settype(&mattr, PTHREAD_MUTEX_RECURSIVE);
    pthread_mutex_init(&(mtx->mtx), &mattr);
    pthread_mutexattr_destroy(&mattr);

    pthread_mutex_unlock(&userspaceMutexes_mtx);

    *mtxid = (GT_MUTEX)m;
    return GT_OK;
#endif
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
#ifndef OS_USERSPACE_MUTEXES
    return osSemDelete((GT_SEM)mtxid);
#else
    CHECK_MTX(mtxid);
    pthread_mutex_destroy(&(mtx->mtx));
    mtx->header.type = 0;
    return GT_OK;
#endif
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
#ifndef OS_USERSPACE_MUTEXES
    return osSemWait((GT_SEM)mtxid, OS_WAIT_FOREVER);
#else
    CHECK_MTX(mtxid);
    pthread_mutex_lock(&(mtx->mtx));
#ifdef  OS_USERSPACE_MUTEX_STAT
    mtx->lockcount++;
#endif
    return GT_OK;
#endif
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
#ifndef OS_USERSPACE_MUTEXES
    return osSemSignal((GT_SEM)mtxid);
#else
    CHECK_MTX(mtxid);
    pthread_mutex_unlock(&(mtx->mtx));
#ifdef  OS_USERSPACE_MUTEX_STAT
    mtx->unlockcount++;
#endif
    return GT_OK;
#endif
}

#if defined(OS_USERSPACE_MUTEXES) && defined(OS_USERSPACE_MUTEX_STAT)
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
    int m;

    for (m = 1; m < userspaceMutexes.allocated; m++)
    {
        GT_LINUX_MUTEX *mtx;
        mtx = (GT_LINUX_MUTEX*)(userspaceMutexes.list[m]);
        if (!mtx || mtx->header.type == 0)
            continue;
        osPrintf("id=%d name=\"%s\" lcount=%d ulcount=%d\n",
                m,
                mtx->header.name,
                mtx->lockcount,
                mtx->unlockcount);
    }
    return GT_OK;
}
#endif /* defined(OS_USERSPACE_MUTEXES) && defined(OS_USERSPACE_MUTEX_STAT) */
