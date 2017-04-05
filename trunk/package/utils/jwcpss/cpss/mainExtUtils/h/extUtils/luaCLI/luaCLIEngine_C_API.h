/*******************************************************************************
*              (c), Copyright 2015, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* luaCLIEngine_C_API.h
*
* DESCRIPTION:
*       a C API for LUA CLI Engine
*
* DEPENDENCIES:
*
* COMMENTS:
*
* FILE REVISION NUMBER:
*       $Revision: 1 $
**************************************************************************/
#ifndef __luaCLIEngine_C_API_h__
#define __luaCLIEngine_C_API_h__

/***** Includes  ********************************************************/
#include <extUtils/luaCLI/luaCLIEngine.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/*******************************************************************************
* luaCLIEngineNewInstance
*
* DESCRIPTION:
*       Init new Lua state, enable minimal functionality
*       Should be used to create luaCLI and for new Lua task
*
* INPUTS:
*       fs                - FS API pointer (optional)
*       getFunctionByName - (optional)
*
* OUTPUTS:
*       luaPtr            - Lua instance
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on error
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS luaCLIEngineNewInstance
(
    IN  genFS_PTR                fs,
    IN  LUA_CLI_GETFUNCBYNAME_T  getFunctionByName,
    OUT lua_State              **luaPtr
);

/*******************************************************************************
* luaCLIEngineDestroy
*
* DESCRIPTION:
*       Destroy luaCLI instance
*
* INPUTS:
*       L   - Lua instance
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
GT_STATUS luaCLIEngineDestroy
(
    IN lua_State    *L
);

/*******************************************************************************
* luaCLIEngineExec
*
* DESCRIPTION:
*       Execute luaCLI command
*
* INPUTS:
*       L               - Lua instance
*       cmd             - luaCLI command
*                         For example: "show version"
*       outBufferLength - output buffer length
*
* OUTPUTS:
*       outBuffer       - output buffer
*       outBufferLength - output buffer length
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on error
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS luaCLIEngineExec
(
    IN  lua_State  *L,
    IN  const char *cmd,
    OUT char       *outBuffer,
    INOUT GT_U32   *outBufferLength
);

/*******************************************************************************
* luaCLIEngineDoLua
*
* DESCRIPTION:
*       Execute lua script
*
* INPUTS:
*       L               - Lua instance
*       luaScript       - lua script
*                         For example: "print(to_string({1,2,3}))"
*       outBufferLength - output buffer length
*
* OUTPUTS:
*       outBuffer       - output buffer
*       outBufferLength - output buffer length
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on error
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS luaCLIEngineDoLua
(
    IN  lua_State  *L,
    IN  const char *luaScript,
    OUT char       *outBuffer,
    INOUT GT_U32   *outBufferLength
);

/*******************************************************************************
* luaCLIEngineLoadModule
*
* DESCRIPTION:
*       Load new luaCLI module
*       This command executes require("module_name")
*       module_name.lua must be in genFS
*
* INPUTS:
*       L               - Lua instance
*       moduleName      - Module name (w/o .lua extension)
*       outBufferLength - output buffer length
*
* OUTPUTS:
*       outBuffer       - output buffer
*       outBufferLength - output buffer length
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on error
*
* COMMENTS:
*       None
*
* EXAMPLE:
*       luaCLIEngineLoadModule(L, "examples/run", NULL, 0);
*
*******************************************************************************/
GT_STATUS luaCLIEngineLoadModule
(
    IN  lua_State  *L,
    IN  const char *moduleName,
    OUT char       *outBuffer,
    INOUT GT_U32   *outBufferLength
);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __luaCLIEngine_C_API_h__ */
