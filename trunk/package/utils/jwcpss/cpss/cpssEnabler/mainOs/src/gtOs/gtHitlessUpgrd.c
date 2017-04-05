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
* gtHitlessUpgrd.c
*
* DESCRIPTION:
*       Implementation for hitless SW upgrade.
*
* DEPENDENCIES:
*       None.
*
* FILE REVISION NUMBER:
*       $Revision: 1.1.2.1 $
*
*******************************************************************************/

#include <gtOs/gtHitlessUpgrd.h>

/* osAttachStageDone - determines if the OS already performed attach. Needed  */ 
/*                     to prevent duplicate executions due to calls from tapi */ 
/*                     attach and core attach.                                */
static GT_BOOL osAttachStageDone = GT_FALSE; 


/*******************************************************************************
* osHitlessUpgradeDetach
*
* DESCRIPTION:
*        Detaches the mainOs lib
*
* INPUTS:
*       None.       
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK - operation succeeded
*       GT_FAIL - operation failed
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS osHitlessUpgradeDetach (void)
{
    osAttachStageDone = GT_FALSE;
    return GT_OK;
}


/*******************************************************************************
* osHitlessUpgradeAttach
*
* DESCRIPTION:
*        Attaches the mainOs lib to the stored system state.
*
* INPUTS:
*        memAllocFuncPtr - the function to be used in memory allocation
*                          (refer to osMemLibInit)                     
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK - operation succeeded
*       GT_FAIL - operation failed
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS osHitlessUpgradeAttach
(    
    IN   GT_MEMORY_ALLOC_FUNC    memAllocFuncPtr
)
{
    GT_STATUS rc = GT_OK;
    if (osAttachStageDone == GT_FALSE)
    {
        rc = osMemBindMemAllocFunc(memAllocFuncPtr);
        if (rc != GT_OK)
        {
            return rc;
        }
        osAttachStageDone = GT_TRUE;
    }

    return rc;
}


