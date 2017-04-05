/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* cmdOsh.h
*
* DESCRIPTION:
*       Operating System wrapper
*
* FILE REVISION NUMBER:
*       $Revision: 2.*******************************************************************************/

#ifndef __cmdOsh
#define __cmdOsh

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#ifndef CMDOS_NO_CMDCOMMON
#include <cmdShell/common/cmdCommon.h>
#endif

/* a pointer to config file name
 * Default is "config.txt"
 */
extern GT_CHAR *cmdOsConfigFileName;
/* a pointer to directory name with trailing slash added
 * usually this is a current(startup) directory 
 * Default is NULL
 */
extern GT_CHAR *cmdOsConfigFilePath;

/*******************************************************************************
 * Typedef:     enum   CMD_STARTUP_SHELL_TYPE_ENT
 *
 * Description: enum that describes default mainCmd shell
 *
 * Fields:
 *     CMD_STARTUP_SHELL_CMDSHELL_E - The default shell is cmdShell
 *                          (GalTis shell)
 *
 *     CMD_STARTUP_SHELL_LUACLI_E   - The default shell is luaCLI
 *
 * Comments:
 *
 *******************************************************************************/
typedef enum
{
    CMD_STARTUP_SHELL_CMDSHELL_E,
    CMD_STARTUP_SHELL_LUACLI_E
} CMD_STARTUP_SHELL_TYPE_ENT;
/* default command shell
 * Default is CMD_STARTUP_SHELL_CMDSHELL_E
 */
extern CMD_STARTUP_SHELL_TYPE_ENT cmdDefaultShellType;

/*******************************************************************************
* cmdOsGetServerAndLocalIP
*
* DESCRIPTION:
*       Returns TFTP server IP address and local IP 
*
* INPUTS:
*       None
*
* OUTPUTS:
*       local_ip
*       server_ip
*
* RETURNS:
*       Status
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS cmdOsGetServerAndLocalIP(
  OUT GT_CHAR *local_ip,
  OUT GT_CHAR *server_ip
);


#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif  /* __cmdOsh */
/* Do Not Add Anything Below This Line */

