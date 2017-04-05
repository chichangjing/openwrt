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
* initDbUserExit.c
*
* DESCRIPTION:
*       The "cmdUserInitDatabase()" function .
*       Overwrites the same named empty function from
*       from [mainCmd] cmdShell/shell/ directory at link build step.
*
* DEPENDENCIES:
*       none
*
* FILE REVISION NUMBER:
*       $Revision: 7 $
*******************************************************************************/

/***** Include files ***************************************************/

#if (defined CPSS_PRODUCT) || (defined PSS_PRODUCT)
#include <prestera/tapi/sysConfig/gtSysConfig.h>
#include <prestera/tapi/logicalIf/gtLif.h>
#include <appDemo/sysHwConfig/gtAppDemoSysConfig.h>
#include <appDemo/boardConfig/appDemoBoardConfig.h>
#endif 

#include <cmdShell/cmdDb/cmdBase.h>
#include <cmdShell/common/cmdWrapUtils.h>
#include <cmdShell/shell/userInitCmdDb.h>


/*******************************************************************************
 * External usage variables
 ******************************************************************************/
#if (defined CPSS_PRODUCT) || (defined PSS_PRODUCT)
extern GT_INTFUNCPTR gtInitSystemFuncPtr;
extern GT_DEVICE internalBase;
#endif

#ifdef IMAGE_HELP_SHELL
extern void imageHelpShellInit(void);
#endif
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
)
{
#if (defined CPSS_PRODUCT) || (defined PSS_PRODUCT)
    /* Make it TWIST - D */
    internalBase = GT_98EX120D;/* was GT_98MX610BS --- 0x006011AB */
    gtInitSystemFuncPtr = (GT_INTFUNCPTR)gtInitSystem;
#endif 

#ifdef IMAGE_HELP_SHELL
    imageHelpShellInit();
#endif
    return GT_OK ;
}
