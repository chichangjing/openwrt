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
* cmdVersion.h
*
* DESCRIPTION:
*       Includes software version information for the command shell part of
*       the Prestera software suite.
*
* DEPENDENCIES:
*       None.
*
* FILE REVISION NUMBER:
*       $Revision: 6 $
*
*******************************************************************************/

#ifndef __cmdVersionh
#define __cmdVersionh

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include <cmdShell/os/cmdOs.h>

#define CMD_SHELL_VERSION_MAX_LEN 30
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
    GT_U8   version[CMD_SHELL_VERSION_MAX_LEN];
}GT_CMD_SHELL_VERSION;


/*******************************************************************************
* gtCmdShellVersion
*
* DESCRIPTION:
*       This function returns the version of the Command shell part of the
*       Prestera SW suite.
*
* INPUTS:
*       None.
*
* OUTPUTS:
*       version     - Command shell software version.
*
* RETURNS:
*       GT_OK on success,
*       GT_BAD_PARAM on bad parameters,
*       GT_FAIL otherwise.
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS gtCmdShellVersion
(
    OUT GT_CMD_SHELL_VERSION   *version
);


#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __cmdVersionh */


