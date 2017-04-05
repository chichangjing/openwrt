/*******************************************************************************
*              (c), Copyright 2007, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* cmdOsExecute.h
*
* DESCRIPTION:
*       extended operating system wrapper library
*       implements pipe IO routines and standard io redirection
*
* DEPENDENCIES:
*       gtTypes.h
*
* FILE REVISION NUMBER:
*       $Revision: 1 $
*******************************************************************************/

#ifndef __cmdExecuteh
#define __cmdExecuteh

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include <cmdShell/common/cmdCommon.h>

#define OS_DEF_BUFFSIZE 4096    /* default buffer size  */

/*******************************************************************************
* osShellGetFunctionByName
*
* DESCRIPTION:
*       Lookup executable file symbol table for function
*
* INPUTS:
*       funcName - null terminated command string
*
* OUTPUTS:
*       None
*
* RETURNS:
*       NULL        - if function is not found
*       function pointer
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_VOIDFUNCPTR osShellGetFunctionByName
(
    IN  const char* funcName
);

/*******************************************************************************
* osShellExecute
*
* DESCRIPTION:
*       execute command through OS shell
*
* INPUTS:
*       command - null terminated command string
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK       - on success
*       GT_FAIL     - on error
*       GT_TIMEOUT  - on timeout
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS   osShellExecute
(
    IN  char*   command
);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* ifndef __cmdExecuteh */
