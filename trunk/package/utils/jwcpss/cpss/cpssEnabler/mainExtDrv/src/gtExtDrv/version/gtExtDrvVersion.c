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
* gtExtDrvVersion.c
*
* DESCRIPTION:
*       Includes software version information for the External drivers part of
*       the Prestera software suite.
*
* DEPENDENCIES:
*       None.
*
* FILE REVISION NUMBER:
*       $Revision: 1.1.2.1 $
*
*******************************************************************************/

#include <gtExtDrv/version/gtExtDrvVersion.h>


#ifdef EXT_DRV_DEBUG
#define DBG_INFO(x) osPrintf x
#else
#define DBG_INFO(x)
#endif


#define EXT_DRV_VERSION   "v1.2a3"

/*******************************************************************************
* gtExtDrvVersion
*
* DESCRIPTION:
*       This function returns the version of the External driver part of the
*       Prestera SW suite.
*
* INPUTS:
*       None.
*
* OUTPUTS:
*       version     - External driver software version.
*
* RETURNS:
*       GT_OK on success,
*       GT_BAD_PARAM on bad parameters,
*       GT_FAIL otherwise.
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS gtExtDrvVersion
(
    OUT GT_EXT_DRV_VERSION   *version
)
{
    if(version == NULL)
        return GT_BAD_PARAM;

    if(osStrlen(EXT_DRV_VERSION) > EXT_DRV_VERSION_MAX_LEN)
    {
        DBG_INFO(("EXT_DRV_VERSION exceeded max len\n"));
        return GT_FAIL;
    }

    osMemCpy(version->version,EXT_DRV_VERSION,osStrlen(EXT_DRV_VERSION) + 1);
    return GT_OK;
}
