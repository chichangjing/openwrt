/*******************************************************************************
*              Copyright 2002, GALILEO TECHNOLOGY, LTD.
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
* gtHitlessUpgrd.h
*
* DESCRIPTION:
*       Definitions for hitless SW upgrade.
*
* DEPENDENCIES:
*       None.
*
* FILE REVISION NUMBER:
*       $Revision: 2 $
*
*******************************************************************************/

#ifndef __gtOsHitlessUpgrd_h
#define __gtOsHitlessUpgrd_h

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include <gtOs/gtOsMem.h>

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
GT_STATUS osHitlessUpgradeDetach (void);


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
);

/*******************************************************************************
* osMemBindMemAllocFunc
*
* DESCRIPTION:
*        Binds the the memory allocation function
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
GT_STATUS osMemBindMemAllocFunc
(    
    IN   GT_MEMORY_ALLOC_FUNC    memAllocFuncPtr
);




#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __gtOsHitlessUpgrd_h */






