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
* cmdVersion.c
*
* DESCRIPTION:
*       Includes software version information for the command shell part of
*       the Prestera software suite.
*
* DEPENDENCIES:
*       None.
*
* FILE REVISION NUMBER:
*       $Revision: 3 $
*
*******************************************************************************/

#include <cmdShell/common/cmdVersion.h>
#include <cmdShell/common/cmdCommon.h>

#define CMD_SHELL_VERSION "v1.2a3"

#ifdef CMD_SHELL_DEBUG
#define DBG_INFO(x) cmdOsPrintf x
#else
#define DBG_INFO(x)
#endif


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
)
{
    if(version == NULL)
        return GT_BAD_PARAM;

    if(cmdOsStrlen(CMD_SHELL_VERSION) > CMD_SHELL_VERSION_MAX_LEN)
    {
        DBG_INFO(("CMD_SHELL_VERSION exceeded max len\n"));
        return GT_FAIL;
    }

    cmdOsMemCpy(version->version,CMD_SHELL_VERSION,cmdOsStrlen(CMD_SHELL_VERSION)+ 1);
    return GT_OK;
}
