/*************************************************************************
*              (c), Copyright 2013, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* cpssApiCallLock.h
*
* DESCRIPTION:
*       Lock/unlock for CPSS API CALLS
*
* DEPENDENCIES:
*
* COMMENTS:
*
* FILE REVISION NUMBER:
*       $Revision: 1 $
**************************************************************************/

#ifndef __cpssApiCallLockh
#define __cpssApiCallLockh

/************************************************************************
* Lock/unlock for CPSS API CALLS
************************************************************************/
typedef void (*CPSS_API_LOCK_UNLOCK_CALLBACK_FUNCPTR)(void);

#define CPSS_API_CALL_LOCK()    cpssApiCallLock()
#define CPSS_API_CALL_UNLOCK()  cpssApiCallUnlock()				   

/*******************************************************************************
* cpssApiCallLockUnlockBind
*
* DESCRIPTION:
*       Bind functions for lock/unlock before/after call to CPSS API
*
* INPUTS:
*       lockFunc   - pointer to lock functions
*       unlockFunc - pointer to unlock functions
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
void cpssApiCallLockUnlockBind
(
	CPSS_API_LOCK_UNLOCK_CALLBACK_FUNCPTR lockFunc, 
	CPSS_API_LOCK_UNLOCK_CALLBACK_FUNCPTR unlockFunc
);

/*******************************************************************************
* cpssApiCallLock
*
* DESCRIPTION:
*       Acquire lock before call to CPSS API
*
* INPUTS:
*       None
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
void cpssApiCallLock(void);

/*******************************************************************************
* cpssApiCallUnlock
*
* DESCRIPTION:
*       Acquire lock before call to CPSS API
*
* INPUTS:
*       None
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
void cpssApiCallUnlock(void);

#endif /* __cpssApiCallLockh */
