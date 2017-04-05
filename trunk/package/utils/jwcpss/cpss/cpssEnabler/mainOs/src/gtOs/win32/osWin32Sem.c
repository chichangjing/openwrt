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
* osWin32Sem.c
*
* DESCRIPTION:
*       Win32 Operating System Simulation. Semaphore facility.
*
* DEPENDENCIES:
*       Win32, CPU independed.
*
* FILE REVISION NUMBER:
*       $Revision: 6 $
*******************************************************************************/
#define OS_WIN32_MTX_AS_CRITICAL_SECTION

#include <windows.h>
#include <process.h>
#include <time.h>

#include <stdio.h>
#include <assert.h>

#include <winbase.h>

#include <gtOs/gtOsSem.h>
#include <gtOs/gtOsMem.h>
#include <gtOs/gtOsIo.h>
#ifdef OS_WIN32_MTX_AS_CRITICAL_SECTION
#include <gtOs/osObjIdLib.h>
#endif


/************ Internal Typedefs ***********************************************/
#define OS_USERSPACE_MUTEX_STAT
#ifdef OS_WIN32_MTX_AS_CRITICAL_SECTION
typedef struct _osMutex
{
    OS_OBJECT_HEADER_STC    header;
    CRITICAL_SECTION        mtx;
#ifdef  OS_USERSPACE_MUTEX_STAT
    int                     lockcount;
    int                     unlockcount;
#endif
}GT_WIN32_MUTEX;

static OS_OBJECT_LIST_STC userspaceMutexes = {
    NULL,                   /* list */
    0,                      /* allocated */
    128,                    /* allocChunk */
    sizeof(GT_WIN32_MUTEX), /* objSize */
    malloc,                 /* allocFuncPtr */
    free                    /* freeFuncPtr */
};

#define CHECK_MTX(mtxId) \
    GT_WIN32_MUTEX *mtx; \
    if ((int)mtxId < 1 || (int)mtxId >= userspaceMutexes.allocated) \
        return GT_FAIL; \
    mtx = (GT_WIN32_MUTEX*)(userspaceMutexes.list[(int)mtxId]); \
    if (!mtx || !mtx->header.type) \
        return GT_FAIL;

static CRITICAL_SECTION userspaceMutexes_mtx;
#endif /* defined(OS_WIN32_MTX_AS_CRITICAL_SECTION) */


/*
 * Typedef: struct GT_WIN_RW_LOCK
 *
 * Description: Defines the Read / Write lock type for Win32 implementation.
 *
 * Fields:
 *      numOfReaders    - Number of readers currently accessing the data-base.
 *      cntSem          - A binary semaphore to protect the access to
 *                        numOfReaders.
 *      writerSem       - A binary semaphore used to prevent further readers
 *                        from getting the lock after the first write arrives.
 *      dbSem           - A semaphore to protect the data-base we are accessing.
 *
 */
typedef struct _winRwLock
{
    GT_U32  numOfReaders;
    GT_SEM  cntSem;
    GT_SEM  writerSem;
    GT_SEM  dbSem;
}GT_WIN_RW_LOCK;

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
static unsigned long scount = 0;

GT_STATUS osSemBinCreate
(
    IN  const char    *name,
    IN  GT_SEMB_STATE init,
    OUT GT_SEM        *smid
)
{
    char sname[64];

    if (scount == 0)
        scount =  GetCurrentThreadId();

    _snprintf(sname,sizeof(sname),"%s%u",name,scount++);
    sname[sizeof(sname)-1] = 0;
    *smid = (GT_SEM)CreateSemaphore(NULL,       /* no security attributes   */
                          (unsigned long)init,  /* initial count            */
                          (unsigned long)1,     /* maximum count            */
                           sname);              /* semaphore name           */

    if (*smid == 0)
      return GT_FAIL;

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
    if (CloseHandle((HANDLE)smid) == 0)
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
    if (timeOut == OS_WAIT_FOREVER)
        timeOut = INFINITE;
    else if(timeOut == OS_NO_WAIT)
        timeOut = 0;

    switch (WaitForSingleObject((HANDLE)smid, timeOut))
    {
        case WAIT_FAILED:
            return GT_FAIL;

        case WAIT_TIMEOUT:
            return GT_TIMEOUT;

        case WAIT_OBJECT_0:
            return GT_OK;
    }

  return GT_FAIL;
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
    ReleaseSemaphore((HANDLE)smid,1,NULL);

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
#ifndef OS_WIN32_MTX_AS_CRITICAL_SECTION
    return osSemBinCreate(name, OS_SEMB_FULL, (GT_SEM*)mtxid);
#else
    int m;
    OS_OBJECT_HEADER_STC *h;
    GT_WIN32_MUTEX *mtx;
    static GT_BOOL facInit  =   GT_FALSE;

    if (facInit == GT_FALSE)
    {
        InitializeCriticalSection(&userspaceMutexes_mtx);
        facInit = GT_TRUE;
    }

    EnterCriticalSection(&userspaceMutexes_mtx);
    m = osObjLibGetNewObject(&userspaceMutexes, 1, name, &h);
    mtx = (GT_WIN32_MUTEX*)h;
    if (m <= 0)
    {
        LeaveCriticalSection(&userspaceMutexes_mtx);
        return GT_FAIL;
    }
    /* initialize recursive mutex */
    InitializeCriticalSection(&(mtx->mtx));

    LeaveCriticalSection(&userspaceMutexes_mtx);

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
#ifndef OS_WIN32_MTX_AS_CRITICAL_SECTION
    return osSemDelete((GT_SEM)mtxid);
#else
    CHECK_MTX(mtxid);
    DeleteCriticalSection(&(mtx->mtx));
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
#ifndef OS_WIN32_MTX_AS_CRITICAL_SECTION
    return osSemWait((GT_SEM)mtxid, OS_WAIT_FOREVER);
#else
    CHECK_MTX(mtxid);
    EnterCriticalSection(&(mtx->mtx));
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
#ifndef OS_WIN32_MTX_AS_CRITICAL_SECTION
    return osSemSignal((GT_SEM)mtxid);
#else
    CHECK_MTX(mtxid);
    LeaveCriticalSection(&(mtx->mtx));
#ifdef  OS_USERSPACE_MUTEX_STAT
    mtx->unlockcount++;
#endif
    return GT_OK;
#endif
}

#if defined(OS_WIN32_MTX_AS_CRITICAL_SECTION) && defined(OS_USERSPACE_MUTEX_STAT)
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
        GT_WIN32_MUTEX *mtx;
        mtx = (GT_WIN32_MUTEX*)(userspaceMutexes.list[m]);
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
#endif /* defined(OS_WIN32_MTX_AS_CRITICAL_SECTION) && defined(OS_USERSPACE_MUTEX_STAT) */

/*******************************************************************************
* osRwLockCreate
*
* DESCRIPTION:
*       Create and initialize a Read / Write lock.
*
* INPUTS:
*       name   - Lock Name.
*
* OUTPUTS:
*       lockId - semaphore Id
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on error
*
* COMMENTS:
*       1.  The lock is created in empty state.
*
*******************************************************************************/
GT_STATUS osRwLockCreate
(
    IN  const char  *name,
    OUT GT_RW_LOCK  *lockId
)
{
    GT_WIN_RW_LOCK  *rwLock;
    GT_U8           lockName[100];

    rwLock = (GT_WIN_RW_LOCK*)osMalloc(sizeof(GT_WIN_RW_LOCK));
    if(rwLock == NULL)
    {
        return GT_FAIL;
    }

    osSprintf(lockName,"%s-cntSem",name);
    rwLock->numOfReaders = 0;
    if(osSemBinCreate(lockName,OS_SEMB_FULL,&(rwLock->cntSem)) != GT_OK)
    {
        osFree(rwLock);
        return GT_FAIL;
    }

    osSprintf(lockName,"%s-wrSem",name);
    if(osSemBinCreate(lockName,OS_SEMB_FULL,&(rwLock->writerSem)) != GT_OK)
    {
        osSemDelete(rwLock->cntSem);
        osFree(rwLock);
        return GT_FAIL;
    }

    if(osSemBinCreate(lockName,OS_SEMB_FULL,&(rwLock->dbSem)) != GT_OK)
    {
        osSemDelete(rwLock->cntSem);
        osSemDelete(rwLock->writerSem);
        osFree(rwLock);
        return GT_FAIL;
    }

    *lockId = (GT_RW_LOCK)rwLock;

    return GT_OK;
}


/*******************************************************************************
* osRwLockDelete
*
* DESCRIPTION:
*       Delete a read / write lock.
*
* INPUTS:
*       lockId - Lock Id
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
GT_STATUS osRwLockDelete
(
    IN GT_RW_LOCK lockId
)
{
    GT_WIN_RW_LOCK  *rwLock;
    GT_STATUS       retVal;

    rwLock = (GT_WIN_RW_LOCK*)lockId;

    if((retVal = osSemDelete(rwLock->cntSem)) != GT_OK)
    {
        return retVal;
    }

    if((retVal = osSemDelete(rwLock->dbSem)) != GT_OK)
    {
        return retVal;
    }

    return osSemDelete(rwLock->writerSem);
}


/*******************************************************************************
* osRwLockWait
*
* DESCRIPTION:
*       Wait on a read / write lock.
*
* INPUTS:
*       lockId      - Lock Id.
*       lockType    - The lock type.
*       timeOut     - time out in miliseconds or 0 to wait forever.
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
*       None.
*
*******************************************************************************/
GT_STATUS osRwLockWait
(
    IN GT_RW_LOCK       lockId,
    IN GT_RW_LOCK_TYPE  lockType,
    IN GT_U32           timeOut
)
{
    GT_WIN_RW_LOCK  *rwLock;
    GT_STATUS       retVal;

    rwLock = (GT_WIN_RW_LOCK*)lockId;

    /* Prevent any new readers / writers from getting in    */
    if((retVal = osSemWait(rwLock->writerSem,timeOut)) != GT_OK)
    {
        return retVal;
    }

    if(lockType == OS_READ_LOCK)
    {
        /* Lock the access to the readers counter               */
        if((retVal = osSemWait(rwLock->cntSem,timeOut)) != GT_OK)
        {
            return retVal;
        }

        rwLock->numOfReaders++;
        if(rwLock->numOfReaders == 1)
        {
            if((retVal = osSemWait(rwLock->dbSem,timeOut)) != GT_OK)
            {
                return retVal;
            }
        }

        osSemSignal(rwLock->cntSem);
        osSemSignal(rwLock->writerSem);
    }
    else
    {
        /* lockType == OS_WRITE_LOCK    */
        if((retVal = osSemWait(rwLock->dbSem,timeOut)) != GT_OK)
        {
            return retVal;
        }
    }

    return GT_OK;
}


/*******************************************************************************
* osRwLockSignal
*
* DESCRIPTION:
*       Signal a Read / Write lock.
*
* INPUTS:
*       lockId      - Lock Id.
*       lockType    - The lock type this lock was taken for, read / write.
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
GT_STATUS osRwLockSignal
(
    IN GT_RW_LOCK       lockId,
    IN GT_RW_LOCK_TYPE  lockType
)
{
    GT_WIN_RW_LOCK  *rwLock;
    GT_STATUS       retVal;

    rwLock = (GT_WIN_RW_LOCK*)lockId;

    if(lockType == OS_READ_LOCK)
    {
        if((retVal = osSemWait(rwLock->cntSem,OS_WAIT_FOREVER)) != GT_OK)
        {
            return retVal;
        }

        rwLock->numOfReaders--;
        if(rwLock->numOfReaders == 0)
        {
            /* Free waiting writers */
            osSemSignal(rwLock->dbSem);
        }

        osSemSignal(rwLock->cntSem);
    }
    else
    {
        /* lockType == OS_WRITE_LOCK    */
        osSemSignal(rwLock->dbSem);
        osSemSignal(rwLock->writerSem);
    }

    return GT_OK;
}


