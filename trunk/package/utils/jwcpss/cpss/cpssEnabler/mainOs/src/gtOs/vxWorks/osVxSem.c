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
* osVxSem.c
*
* DESCRIPTION:
*       VxWorks Operating System wrapper. Semaphore facility.
*
* DEPENDENCIES:
*       VxWorks, CPU independed.
*
* FILE REVISION NUMBER:
*       $Revision: 6 $
*******************************************************************************/

#include <vxWorks.h>
#include <objLib.h>
#include <semLib.h>
#include <sysLib.h>

#include <gtOs/gtOsSem.h>
#include <gtOs/gtOsMem.h>
#include <gtOs/gtOsGen.h>


/************ Internal Typedefs ***********************************************/
/*
 * Typedef: struct GT_VX_RW_LOCK
 *
 * Description: Defines the Read / Write lock type for VxWorks implementation.
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
typedef struct _vxRwLock
{
    GT_U32  numOfReaders;
    GT_SEM  cntSem;
    GT_SEM  writerSem;
    GT_SEM  dbSem;
}GT_VX_RW_LOCK;


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
  SEM_ID s_id;

  IS_WRAPPER_OPEN_STATUS;
  /* create semaphore binary */
  if (init == OS_SEMB_EMPTY)
      s_id = semBCreate(SEM_Q_FIFO, SEM_EMPTY);
  else
      s_id = semBCreate (SEM_Q_FIFO, SEM_FULL);

  if (s_id == NULL)
  {
      *smid = 0;
      return GT_FAIL;
  }

  *smid = (GT_SEM)s_id;

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
  STATUS rc;

  IS_WRAPPER_OPEN_STATUS;
  rc = semDelete((SEM_ID) smid);

  if (rc != OK)
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
    STATUS rc;

    IS_WRAPPER_OPEN_STATUS;
    if (timeOut == OS_WAIT_FOREVER)
        rc = semTake ((SEM_ID) smid, WAIT_FOREVER);
    else if(timeOut == OS_NO_WAIT)
        rc = semTake ((SEM_ID) smid, NO_WAIT);
    else
    {
        int num, delay;

        num = sysClkRateGet();
        delay = (num * timeOut) / 1000;
        if (delay < 1)
            rc = semTake ((SEM_ID) smid, 1);
        else
            rc = semTake ((SEM_ID) smid, delay);
    }

    if (rc != OK)
    {
        if (errno == S_objLib_OBJ_TIMEOUT)
            return GT_TIMEOUT;
        else
            return GT_FAIL;
    }

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
    STATUS rc;

    IS_WRAPPER_OPEN_STATUS;
    rc = semGive ((SEM_ID) smid);

    if (rc != OK)
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
  SEM_ID s_id;

  IS_WRAPPER_OPEN_STATUS;
  /* create mutex semaphore */
  s_id = semMCreate (SEM_Q_FIFO);

  if (s_id == NULL)
  {
      *mtxid = 0;
      return GT_FAIL;
  }

  *mtxid = (GT_SEM)s_id;

  return GT_OK;
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
    STATUS rc;

    IS_WRAPPER_OPEN_STATUS;
    rc = semDelete((SEM_ID) mtxid);

    if (rc != OK)
        return GT_FAIL;

    return GT_OK;
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
    STATUS rc;

    rc = semTake ((SEM_ID) mtxid, WAIT_FOREVER);

    if (rc != OK)
    {
        return GT_FAIL;
    }

    return GT_OK;
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
    STATUS rc;

    rc = semGive ((SEM_ID) mtxid);

    if (rc != OK)
        return GT_FAIL;

    return GT_OK;
}

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
    GT_VX_RW_LOCK   *rwLock;

    IS_WRAPPER_OPEN_STATUS;
    rwLock = (GT_VX_RW_LOCK*)osMalloc(sizeof(GT_VX_RW_LOCK));
    if(rwLock == NULL)
    {
        return GT_FAIL;
    }

    rwLock->numOfReaders = 0;
    if(osSemBinCreate(name,OS_SEMB_FULL,&(rwLock->cntSem)) != GT_OK)
    {
        osFree(rwLock);
        return GT_FAIL;
    }

    if(osSemBinCreate(name,OS_SEMB_FULL,&(rwLock->writerSem)) != GT_OK)
    {
        osSemDelete(rwLock->cntSem);
        osFree(rwLock);
        return GT_FAIL;
    }

    if(osSemBinCreate(name,OS_SEMB_FULL,&(rwLock->dbSem)) != GT_OK)
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
    GT_VX_RW_LOCK   *rwLock;
    GT_STATUS       retVal;

    IS_WRAPPER_OPEN_STATUS;
    rwLock = (GT_VX_RW_LOCK*)lockId;

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
    GT_VX_RW_LOCK   *rwLock;
    GT_STATUS       retVal;

    IS_WRAPPER_OPEN_STATUS;
    rwLock = (GT_VX_RW_LOCK*)lockId;

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
    GT_VX_RW_LOCK   *rwLock;
    GT_STATUS       retVal;

    IS_WRAPPER_OPEN_STATUS;
    rwLock = (GT_VX_RW_LOCK*)lockId;

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
