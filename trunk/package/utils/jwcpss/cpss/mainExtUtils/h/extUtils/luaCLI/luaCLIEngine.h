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

#ifndef __luaCLIEngine_h__
#define __luaCLIEngine_h__

/***** Includes  ********************************************************/
#include <cpss/generic/cpssTypes.h>
#include <cpss/generic/cpssCommonDefs.h>
#include <extUtils/IOStream/IOStream.h>
#include <extUtils/FS/genFS.h>
#include <cpss/extServices/os/gtOs/gtGenTypes.h>
#include <lua.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/***** Defines  ********************************************************/

#define LUACLI_MEMSTAT

typedef void (*LUA_CLI_FUNC_PTR_T) (void); /* ptr to function returning void */
typedef LUA_CLI_FUNC_PTR_T (*LUA_CLI_GETFUNCBYNAME_T)(const char *name);

typedef struct LUA_CLI_MALLOC_STCT {
    /***************************************************************************
    * malloc
    *
    * DESCRIPTION:
    *       malloc() implementation
    *
    * INPUTS:
    *       size - number of bytes to allocate
    *
    * OUTPUTS:
    *       None
    *
    * RETURNS:
    *       Pointer to allocated memory or NULL
    *       GT_FAIL
    *
    ***************************************************************************/
    void* (*malloc)(int size);

    /***************************************************************************
    * free
    *
    * DESCRIPTION:
    *       free() implementation
    *
    * INPUTS:
    *       ptr - pointer to memory allocated my .malloc()
    *
    * OUTPUTS:
    *       None
    *
    * RETURNS:
    *       None
    *
    ***************************************************************************/
    void (*free)(void *ptr);
#ifdef LUACLI_MEMSTAT
    int     totalUsed;
    int     nChunks;
#endif /* LUACLI_MEMSTAT */
} LUA_CLI_MALLOC_STC;

/***** Public Functions ************************************************/



/*******************************************************************************
* luaCLIEngine_newstate
*
* DESCRIPTION:
*       Init new Lua state, enable minimal functionality
*       Should be used to create luaCLI and for new Lua task
*
* INPUTS:
*       memMgr            - malloc/free struct pointer. Mandatory
*       IOStream          - (optional)
*       fs                - FS API pointer (optional)
*       getFunctionByName - (optional)
*
* OUTPUTS:
*       None
*
* RETURNS:
*       luaPtr
*
* COMMENTS:
*       None
*
*******************************************************************************/
lua_State* luaCLIEngine_newstate
(
    IN LUA_CLI_MALLOC_STC      *memMgr,
    IN IOStreamPTR              IOStream,
    IN genFS_PTR                fs,
    IN LUA_CLI_GETFUNCBYNAME_T  getFunctionByName
);

/*******************************************************************************
* luaCLIEngineGetData
*
* DESCRIPTION:
*       Returns pointer to lightuserdata, print error if not defined
*
* INPUTS:
*       L        - lua state
*       name     - name of data
*       printErr - print error if any
*
* OUTPUTS:
*       None
*
* RETURNS:
*       pointer or NULL
*
* COMMENTS:
*       None
*
*******************************************************************************/
void* luaCLIEngineGetData(lua_State* L, const char *name, int printErr);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __luaCLIEngine_h__ */
