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
* gtOsSem.h
*
* DESCRIPTION:
*       Operating System wrapper. Semaphore facility.
*
* DEPENDENCIES:
*       None.
*
* FILE REVISION NUMBER:
*       $Revision: 11 $
*******************************************************************************/

#ifndef __gtOsSemh
#define __gtOsSemh

#ifdef __cplusplus
extern "C" {
#endif

/************* Includes *******************************************************/

#include <gtOs/gtGenTypes.h>

/************* Defines ********************************************************/

#define OS_WAIT_FOREVER             0
/* try to take semaphore, return immediately even if semaphore still busy */
#define OS_NO_WAIT                  0xFFFFFFFF


/************* Typedefs *******************************************************/
#if (!defined __cmdExtServices_h_) || (defined PSS_PRODUCT)
typedef GT_UINTPTR GT_SEM;
typedef GT_UINTPTR GT_MUTEX;
#endif

typedef enum
{
    OS_SEMB_EMPTY = 0,
    OS_SEMB_FULL
}GT_SEMB_STATE;

/* typedefs for Read / Write locks  */
typedef void * GT_RW_LOCK;

typedef enum
{
    OS_READ_LOCK = 0,
    OS_WRITE_LOCK
}GT_RW_LOCK_TYPE;


/************* Functions ******************************************************/

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
);

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
);

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
);

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
);

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
);

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
);


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
);

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
);

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
);

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
);


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
);

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
);


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
);


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
);


#ifdef __cplusplus
}
#endif

#endif  /* __gtOsSemh */
/* Do Not Add Anything Below This Line */

