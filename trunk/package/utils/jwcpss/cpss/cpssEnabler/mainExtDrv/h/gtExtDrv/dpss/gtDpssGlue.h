      
/*******************************************************************************
*
*                   Copyright 2002, GALILEO TECHNOLOGY, LTD.
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL.
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.
*
* MARVELL COMPRISES MARVELL TECHNOLOGY GROUP LTD. (MTGL) AND ITS SUBSIDIARIES,
* MARVELL INTERNATIONAL LTD. (MIL), MARVELL TECHNOLOGY, INC. (MTI), MARVELL
* SEMICONDUCTOR, INC. (MSI), MARVELL ASIA PTE LTD. (MAPL), MARVELL JAPAN K.K.
* (MJKK), GALILEO TECHNOLOGY LTD. (GTL) AND GALILEO TECHNOLOGY, INC. (GTI).
*
*******************************************************************************
* gtDpssGlue.h
*
* DESCRIPTION: APIs used by the TAPI to configure the DPSS. These function
*              Initialize the DPSS and get information from it.
*
* DEPENDENCIES:
*
* FILE REVISION NUMBER:
*       $Revision: 1.1.2.2 $
*******************************************************************************/

#ifndef __gtDpssGlueh
#define __gtDpssGlueh

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/********* include ***********************************************************/
#include <gtOs/gtGenTypes.h>

 
/******** typedefs ***********************************************************/

/*******************************************************************************
* extDpssIsSysDistributed
*
* DESCRIPTION: If the system is distrebuted then return GT_TRUE other wise 
*              return GT_FALSE.
*
* INPUTS: 
*       None.
*
* OUTPUTS:
*       None.
*
* RETURNS :
*       GT_TRUE  - Disterbuted system
*       GT_FALSE - Non disterbuted system.
*
* COMMENTS:
*
*
*******************************************************************************/
GT_BOOL extDpssIsSysDistributed
(
    GT_VOID
);

/*******************************************************************************
* extDpssInit
*
* DESCRIPTION:  Initialize all the modules in the DPSS.
*
*
* INPUTS:
*       None
*       
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on error.
*
* COMMENTS:
*       
*
* Galtis:
*       None.
*
*******************************************************************************/
GT_STATUS extDpssInit
(
    GT_VOID
);

 
/*******************************************************************************
* extDpssIsUnitMaster
*
* DESCRIPTION: Returns the state of the unit master or slave
*
* INPUTS:
*       None.
*
* OUTPUTS:
*       None.
*
* RETURNS : 
*           GT_TRUE  - The unit is master.
*           GT_FALSE - The unit is slave. 
*
* COMMENTS: If this function is not implemented it should return GT_TRUE.
*       
*
*******************************************************************************/
GT_BOOL extDpssIsUnitMaster
(
    GT_VOID
);

 
/*******************************************************************************
* extDpssWaitForCfg
*
* DESCRIPTION: Wait for the configuration to come from the master. After the 
*              configuration came from the master then we can move to core 
*              phase 2 init. The function is blocking.
*
* INPUTS: 
*       None.
*
* OUTPUTS:
*       None.
*
* RETURNS :
*       GT_OK   - on success
*       GT_FAIL - on error.
*
* COMMENTS: The configuration from the master contains several parameters. One 
*           of the parameters is the new PP dev num, this information is needed
*           for the core phase 2 init.
*           If this function is not implemented it should return GT_OK.
*
*******************************************************************************/
GT_STATUS extDpssWaitForCfg
(
    GT_VOID
);

/*******************************************************************************
* extDpssWaitForMasterCfg
*
* DESCRIPTION: Wait for all the slaves to finish there configuration. After all
*              the slaves finished ther configuration we can move to logical
*              configuration    
*
* INPUTS:
*       None.
*
* OUTPUTS:
*       None.
*
* RETURNS :
*       GT_OK   - on success
*       GT_FAIL - on error.
*
* COMMENTS:
*
*
*******************************************************************************/
GT_STATUS  extDpssWaitForMasterCfg
(
    GT_VOID
);

/*******************************************************************************
* extDpssConvTblLock
*
* DESCRIPTION: Locks the conversion table for reading.
*
* INPUTS:
*       None.
*
* OUTPUTS:
*       None.
*
* RETURNS :
*       GT_OK   - on success
*       GT_FAIL - on error.
*
* COMMENTS: If this function is not implemented it should return GT_OK.
*
*
*******************************************************************************/
GT_STATUS extDpssConvTblLock
(
    GT_VOID
);

/*******************************************************************************
* extDpssConvTblUnLock
*
* DESCRIPTION: Unlocks the conversion table for reading.
*
* INPUTS:
*       None.
*
* OUTPUTS:
*       None.
*
* RETURNS :
*       GT_OK   - on success
*       GT_FAIL - on error.
*
* COMMENTS: If this function is not implemented it should return GT_OK.
*
*
*******************************************************************************/
GT_STATUS extDpssConvTblUnLock
(
    GT_VOID
);


/*******************************************************************************
* extDpssUpdateConvTbl
*
* DESCRIPTION: Update the conversion table with the new values
*
* INPUTS:
*       None.
*
* OUTPUTS:
*       None.
*
* RETURNS :
*       GT_OK   - on success
*       GT_FAIL - on error.
*
* COMMENTS: The data of the new values is stored in the USS and this function
*           will only trigger the USS to save the new data in the conversion
*           table.
*           If this function is not implemented it should return GT_OK.
*       
*******************************************************************************/
GT_STATUS extDpssUpdateConvTbl
(
    GT_VOID
);

 
/*******************************************************************************
* extDpssGetPpConvTbl
*
* DESCRIPTION: Get the dev num conversion table from physical to logical.
*              This data is returned from the USS. The USS received the data
*              from the configuration message that came from the master.
* INPUTS:
*       None.
*
* OUTPUTS:
*       devConvTbl   : Pointer to an array. The index of the array is the 
*                      physical dev number of the PP, the value in each cell is 
*                      the logical dev number of the PP.
*       devConvTblLen : The number of elements in the array.
*
* RETURNS :
*       GT_OK   - on success
*       GT_FAIL - on error.
*
* COMMENTS: If this function is not implemented then it should return an array.
*           of the new PP dev nums. The array should be allocated and not a
*           local function array.
*
*******************************************************************************/
GT_STATUS extDpssGetPpConvTbl
(
    OUT GT_U8 **devConvTbl,
    OUT GT_U8 *devConvTblLen
);

/*******************************************************************************
* extDpssIsIpcEnabled
*
* DESCRIPTION: If the IPC is enabled return GT_TRUE other wise 
*              return GT_FALSE.
*
* INPUTS: 
*       None.
*
* OUTPUTS:
*       None.
*
* RETURNS :
*       GT_TRUE  - IPC enabled
*       GT_FALSE - IPC disabled.
*
* COMMENTS:
*
*
*******************************************************************************/
GT_BOOL extDpssIsIpcEnabled
(
    GT_VOID
);

/*******************************************************************************
* extDpssIpcSlaveInit
*
* DESCRIPTION: Initialize the IPC slave
*
* INPUTS:
*       None.
*
* OUTPUTS:
*       None.
*
* RETURNS :
*       GT_OK        - on success,
*       GT_FAIL      - Operation failed.
*
* COMMENTS:
*
*
*******************************************************************************/
GT_STATUS extDpssIpcSlaveInit
(
    GT_VOID
);

/*******************************************************************************
* extDpssIpcMasterInit
*
* DESCRIPTION: Initialize the IPC master
*
* INPUTS:
*       None.
*   
* OUTPUTS:
*       None.
*
* RETURNS :
*       GT_OK        - on success,
*       GT_FAIL      - Operation failed.
*
* COMMENTS:
*
*
*******************************************************************************/
GT_STATUS extDpssIpcMasterInit
(
    GT_VOID
);

/*******************************************************************************
* extDpssInitSysPpParams
*
* DESCRIPTION: Initializes all the PPs logical device numbers and unit IDs.
*			   This is needes before TAPI init in the master.
*
* INPUTS:
*		None.
*
* OUTPUTS:
*		None.
*
* RETURNS :
*		GT_OK        - on success,
*       GT_FAIL      - Operation failed.
*
* COMMENTS:
*
*
*******************************************************************************/
GT_STATUS extDpssInitSysPpParams
(
	GT_VOID
);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __gtDpssGlueh */

