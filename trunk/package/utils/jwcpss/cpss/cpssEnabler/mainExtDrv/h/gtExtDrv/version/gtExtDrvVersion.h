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
* gtExtDrvVersion.h
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

#ifndef __gtExtDrvVersionh
#define __gtExtDrvVersionh

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include <gtExtDrv/os/extDrvOs.h>

#define EXT_DRV_VERSION_MAX_LEN 30
/*
 * Typedef: struct GT_APP_DEMO_VERSION
 *
 * Description: This struct holds the package version.
 *
 * Fields:
 *      version - string array holding the version.
 *
 */
typedef struct
{
    GT_U8   version[EXT_DRV_VERSION_MAX_LEN];
}GT_EXT_DRV_VERSION;


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
);


#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __gtExtDrvVersionh */


