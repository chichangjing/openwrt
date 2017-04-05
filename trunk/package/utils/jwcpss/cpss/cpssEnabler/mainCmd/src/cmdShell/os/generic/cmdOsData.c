/*******************************************************************************
*              (c), Copyright 2015, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* cmdOsData.c
*
* DESCRIPTION:
*       This file contains common functions for mainCmd I/O streams
*
* FILE REVISION NUMBER:
*       $Revision: 8 $
*
*******************************************************************************/
#include <cmdShell/os/cmdOs.h>

/* a pointer to config file name
 * Default is "config.txt"
 */
GT_CHAR *cmdOsConfigFileName = "config.txt";
/* a pointer to directory name with trailing slash added
 * usually this is a current(startup) directory 
 * Default is NULL
 */
GT_CHAR *cmdOsConfigFilePath = NULL;

/* default command shell
 * Default is CMD_STARTUP_SHELL_CMDSHELL_E
 */
CMD_STARTUP_SHELL_TYPE_ENT cmdDefaultShellType = CMD_STARTUP_SHELL_CMDSHELL_E;
