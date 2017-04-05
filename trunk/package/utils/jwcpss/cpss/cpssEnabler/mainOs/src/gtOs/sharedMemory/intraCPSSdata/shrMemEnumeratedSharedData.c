/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* shrMemSharedData.c
*
* DESCRIPTION:
*       This file contains common data for OS/SharedMemory modules
*       which should be shared between different processes, thus should
*       be linked into libcpss.so (only that object
*       has shared BSS/DATA sections).
*
* FILE REVISION NUMBER:
*       $Revision: 1  $
*
*******************************************************************************/
#include <gtOs/gtEnvDep.h>
#include <gtOs/gtOsSharedData.h>
#include <gtOs/gtOsSem.h>



#ifdef SHARED_MEMORY

static uintptr_t sharedDataArray[CPSS_SHARED_DATA_MAX_E];
GT_SEM semMtx = 0;

/*******************************************************************************
* cpssMultiProcGetSharedData
*
* DESCRIPTION:
*       return enumerated shared data
*
* INPUTS:
*  		id       - data identifier
*
* OUTPUTS:
*       None
*
* RETURNS:
*       data (integer or pointer)
*
* COMMENTS:
*       None.
*
*******************************************************************************/
uintptr_t cpssMultiProcGetSharedData
(
    IN CPSS_SHARED_DATA_TYPE id
)
{
    if (id < 0 || id >= CPSS_SHARED_DATA_MAX_E)
    {
        return 0;
    }
    return sharedDataArray[id];
}

/*******************************************************************************
* cpssMultiProcSetSharedData
*
* DESCRIPTION:
*       Set enumerated shared data
*
* INPUTS:
*  		id       - data identifier
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK
*       GT_FAIL
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssMultiProcSetSharedData
(
    IN CPSS_SHARED_DATA_TYPE id,
    IN uintptr_t data
)
{
    if (id < 0 || id >= CPSS_SHARED_DATA_MAX_E)
    {
        return GT_BAD_PARAM;
    }
    sharedDataArray[id] = data;
    return GT_OK;
}


/*******************************************************************************
* cpssMultiProcSharedDataLock
*
* DESCRIPTION:
*       Lock shared data for critical operations (data alloc)
*
* INPUTS:
*       None
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssMultiProcSharedDataLock(void)
{
    if (!semMtx)
    {
        /* not initialized yet */
        if (osSemBinCreate("MP_EnumeratedMtx", OS_SEMB_FULL, &semMtx) != GT_OK)
        {
            return GT_FAIL;
        }
    }
    return osSemWait(semMtx, OS_WAIT_FOREVER);
}

/*******************************************************************************
* cpssMultiProcSharedDataUnlock
*
* DESCRIPTION:
*       Unlock shared data
*
* INPUTS:
*       None
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssMultiProcSharedDataUnlock(void)
{
    if (!semMtx)
    {
        return GT_FAIL;
    }
    return osSemSignal(semMtx);
}

#endif


