/*************************************************************************
*              (c), Copyright 2013, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* cpssApiCallLock.c
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

#include <cmdShell/common/cmdCommon.h>
#include <cmdShell/common/cpssApiCallLock.h>

/* Pointer to lock/unlock functions for calling CPSS APIs */
CPSS_API_LOCK_UNLOCK_CALLBACK_FUNCPTR cpssApiCallLockFunc = NULL;
CPSS_API_LOCK_UNLOCK_CALLBACK_FUNCPTR cpssApiCallUnlockFunc = NULL;

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
)
{
	cpssApiCallLockFunc   = lockFunc;
	cpssApiCallUnlockFunc = unlockFunc;
}
							   
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
void cpssApiCallLock(void)
{
	if(NULL != cpssApiCallLockFunc)
		cpssApiCallLockFunc();
}

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
void cpssApiCallUnlock(void)
{
	if(NULL != cpssApiCallLockFunc)
		cpssApiCallUnlockFunc();
}

