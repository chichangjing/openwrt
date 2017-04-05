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
* osFreeBsdSem.c
*
* DESCRIPTION:
*       FreeBsd User Mode Operating System wrapper. Semaphore facility.
*
* DEPENDENCIES:
*       FreeBsd, CPU independed , and posix threads implementation.
*
* FILE REVISION NUMBER:
*       $Revision: 13 $
*******************************************************************************/

#include <gtOs/gtOsMem.h>
#include <gtOs/gtOsSem.h>
#include <gtOs/gtOsIo.h>


/* enable statistic collection for userspace mutexes */
#define OS_USERSPACE_MUTEX_STAT

/************ Internal Typedefs ***********************************************/
/*
 * Typedef: struct GT_LINUX_RW_LOCK
 *
 * Description: Defines the Read / Write lock type for Linux implementation.
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
typedef struct _linuxRwLock
{
    GT_U32  numOfReaders;
    GT_SEM  cntSem;
    GT_SEM  writerSem;
    GT_SEM  dbSem;
}GT_LINUX_RW_LOCK;

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
    GT_LINUX_RW_LOCK    *rwLock;
    GT_U8               lockName[100];

    rwLock = (GT_LINUX_RW_LOCK*)osMalloc(sizeof(GT_LINUX_RW_LOCK));
    if(rwLock == NULL)
    {
        return GT_FAIL;
    }

    osSprintf((char*)lockName,"%s-cntSem",name);
    rwLock->numOfReaders = 0;
    if(osSemBinCreate((char*)lockName,OS_SEMB_FULL,&(rwLock->cntSem)) != GT_OK)
    {
        osFree(rwLock);
        return GT_FAIL;
    }

    osSprintf((char*)lockName,"%s-wrSem",name);
    if(osSemBinCreate((char*)lockName,OS_SEMB_FULL,&(rwLock->writerSem)) != GT_OK)
    {
        osSemDelete(rwLock->cntSem);
        osFree(rwLock);
        return GT_FAIL;
    }

    if(osSemBinCreate((char*)lockName,OS_SEMB_FULL,&(rwLock->dbSem)) != GT_OK)
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
    GT_LINUX_RW_LOCK    *rwLock;
    GT_STATUS           retVal;

    rwLock = (GT_LINUX_RW_LOCK*)lockId;

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
    GT_LINUX_RW_LOCK    *rwLock;
    GT_STATUS           retVal;

    rwLock = (GT_LINUX_RW_LOCK*)lockId;

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
    GT_LINUX_RW_LOCK    *rwLock;
    GT_STATUS           retVal;

    rwLock = (GT_LINUX_RW_LOCK*)lockId;

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


