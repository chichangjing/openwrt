/*************************************************************************
* luaCLIEngine.h
*
* DESCRIPTION:
*       luaCLI engine
*
* DEPENDENCIES:
*
* COMMENTS:
*
* FILE REVISION NUMBER:
*       $Revision: 1 $
**************************************************************************/

#ifndef __luaCLIEngineCLI_h__
#define __luaCLIEngineCLI_h__

/***** Includes  ********************************************************/
#include <extUtils/luaCLI/luaCLIEngine.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/*******************************************************************************
* luaCLIEngine_init
*
* DESCRIPTION:
*       Init Lua CLI
*
* INPUTS:
*       IOStream
*       memstat     - pointer to memstat structure or NULL
*
* OUTPUTS:
*       luaPtr
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on error
*
* COMMENTS:
*       None
*
*******************************************************************************/
int luaCLIEngine_init
(
    IN LUA_CLI_MALLOC_STC      *memMgr,
    IN IOStreamPTR              IOStream,
    IN genFS_PTR                fs,
    IN LUA_CLI_GETFUNCBYNAME_T  getFunctionByName,
    lua_State                 **luaPtr
);

/*******************************************************************************
* luaCLIEngine_EventLoop
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
int luaCLIEngine_EventLoop
(
    IN LUA_CLI_MALLOC_STC      *memMgr,
    IN IOStreamPTR              IOStream,
    IN genFS_PTR                fs,
    IN LUA_CLI_GETFUNCBYNAME_T  getFunctionByName,
    IN GT_STATUS (*osShell)(IOStreamPTR IOStream)
);


#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __luaCLIEngineCLI_h__ */
