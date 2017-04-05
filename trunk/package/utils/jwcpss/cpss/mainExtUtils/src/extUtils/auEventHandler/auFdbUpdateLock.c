/*******************************************************************************
*              (c), Copyright 2015, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
********************************************************************************
* auFdbUpdateLock.c
*
* DESCRIPTION:
*       Address update lock for Fdb update actions
*
* FILE REVISION NUMBER:
*       $Revision: 1 $
*******************************************************************************/
#include <extUtils/auEventHandler/auFdbUpdateLock.h>

CPSS_OS_MUTEX auFdbUpdateLockMtx = (CPSS_OS_MUTEX)0;

/*******************************************************************************
* auFdbUpdateLockInit
*
* DESCRIPTION:
*       Initialize lock
*
* INPUTS:
*       None
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK on success
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS auFdbUpdateLockInit(void)
{
    if (auFdbUpdateLockMtx != ((CPSS_OS_MUTEX)0))
        return GT_OK;
    return cpssOsMutexCreate("fdbUpdate", &auFdbUpdateLockMtx);
}

/*******************************************************************************
* auFdbUpdateLockReset
*
* DESCRIPTION:
*       Uninit lock
*
* INPUTS:
*       None
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK on success
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS auFdbUpdateLockReset(void)
{
    if (auFdbUpdateLockMtx == ((CPSS_OS_MUTEX)0))
        return GT_OK;

    cpssOsMutexDelete(auFdbUpdateLockMtx);
    auFdbUpdateLockMtx = (CPSS_OS_MUTEX)0;
    return 0;
}
