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
* userInitCmdDb.h
*
* DESCRIPTION:
*       The "cmdUserInitDatabase()" function placeholder.
*       contains the empty function, dedicated to be overwitten
*       from [main] make/win32 directory at link build step.
*
* DEPENDENCIES:
*       none
*
* FILE REVISION NUMBER:
*       $Revision: 2 $
*******************************************************************************/

#ifndef __userInitCmdDbh
#define __userInitCmdDbh

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */


/*******************************************************************************
* cmdUserInitDatabase
*
* DESCRIPTION:
*       
*       The "cmdUserInitDatabase()" function placeholder.
*       Called by "cmdInit" and "cmdInitDebug" functions.
*       Enables some command shell features .
*
* INPUTS:
*       None
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
GT_STATUS cmdUserInitDatabase
(
	GT_VOID
) ;

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __userInitCmdDbh*/
