/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* cmdLuaCLI.c
*
* DESCRIPTION:
*       a LUA CLI implementation
*
* DEPENDENCIES:
*
* COMMENTS:
*
* FILE REVISION NUMBER:
*       $Revision: 37 $
**************************************************************************/

#include <extUtils/luaCLI/luaCLIEngineCLI.h>

#if !defined(ENV_MAINCMD) && !defined(ENV_POSIX)
#  error "No layer defined. Define one of ENV_MAINCMD, ENV_POSIX"
#endif

/***** Include files ***************************************************/

#ifdef ENV_MAINCMD
# include "cmdLua_os_mainCmd.inc"
#endif
#ifdef ENV_POSIX
# include "cmdLua_os_posix.inc"
#endif

#ifdef ENV_MAINCMD
#include <cmdShell/FS/cmdFS.h>
#include <cmdShell/os/cmdShell.h>
#else
#include <cmdFS.h>
#define cmdOsShell NULL
#endif

typedef void* (*LUACLI_MALLOC_T)(int size);

/*******************************************************************************
* luaCLI_EventLoop
*
* DESCRIPTION:
*
*
* INPUTS:
*       stream  - pointer to stream descriptor
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
int luaCLI_EventLoop
(
    IOStreamPTR IOStream
)
{
    LUA_CLI_MALLOC_STC memMgr = {
        (LUACLI_MALLOC_T)LUACLI_MALLOC_F,
        LUACLI_FREE,
        0, 0 };

    return luaCLIEngine_EventLoop(
            &memMgr, IOStream, &cmdFS,
            osShellGetFunctionByName, cmdOsShell);
}
