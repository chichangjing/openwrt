/*******************************************************************************
*              (c), Copyright 2015, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* luaCLIEngine.c
*
* DESCRIPTION:
*       Common function for luaCLIEngine
*
* DEPENDENCIES:
*
* COMMENTS:
*
* FILE REVISION NUMBER:
*       $Revision: 1 $
**************************************************************************/

/***** Include files ***************************************************/


#include <cpss/extServices/private/prvCpssBindFunc.h>
#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>
#include <string.h>
#include <extUtils/luaCLI/luaCLIEngine.h>


/***** Externs  ********************************************************/

int luaEngineGenWrapper_init(lua_State *L);
int luaEngineFSregisterLoader(lua_State *L);
int luaEngineFSregisterAPI(lua_State *L);


#ifdef CMD_LUA_NET
int luaCLIEngineNetRegisterAPI(lua_State *L);
#endif /* CMD_LUA_NET */

#ifdef CMD_LUA_GLOBAL
/* global storage for Lua data */
int luaGlobalStore(lua_State *L);
int luaGlobalGet(lua_State *L);
#endif /* CMD_LUA_GLOBAL */
/***** Private declarations ********************************************/

/***** Public Functions ************************************************/

/*************** Global ******************************************************/

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
void* luaCLIEngineGetData(lua_State* L, const char *name, int printErr)
{
    void *ptr = NULL;

    lua_getglobal(L, name);
    if (lua_islightuserdata(L, -1))
        ptr = lua_touserdata(L, -1);
    lua_pop(L, 1);
    if (ptr != NULL || printErr == 0)
        return ptr;
    lua_getglobal(L, "print");
    lua_pushstring(L, "Failure: ");
    lua_pushstring(L, name);
    lua_pushstring(L, " is not defined or NULL");
    lua_concat(L, 3);
    lua_call(L, 1, 0);
    return ptr;
}

/*******************************************************************************
* luaCLIEnginePrint
*
* DESCRIPTION:
*       lua print function
*
* INPUTS:
*       L       - Lua state
*
* OUTPUTS:
*       None
*
* RETURNS:
*       0
*
* COMMENTS:
*       None
*
*******************************************************************************/
static int luaCLIEnginePrint(
    lua_State *L
)
{
    IOStreamPTR IOStream;
    int n;
    int i;

    IOStream = (IOStreamPTR)luaCLIEngineGetData(L, "_IOStream", 0);
    if (IOStream == NULL)
        return 0;

    n=lua_gettop(L);
    for (i=1; i<=n; i++)
    {
        if (i>1)
            IOStream->writeLine(IOStream, "\t");
        if (lua_isstring(L,i))
            IOStream->writeLine(IOStream, lua_tostring(L,i));
        else if (lua_isnil(L,i))
            IOStream->writeLine(IOStream, "nil");
        else if (lua_isboolean(L,i))
            IOStream->writeLine(IOStream, lua_toboolean(L,i) ? "true" : "false");
        else if (lua_islightuserdata(L,i))
        {
            char buf[40];
            cpssOsSprintf(buf, "<pointer=%p>", lua_touserdata(L, i));
            IOStream->writeLine(IOStream, buf);
        }
        else
        {
            /* printf("%s:%p",luaL_typename(L,i),lua_topointer(L,i)); */
            IOStream->writeLine(IOStream, "<");
            IOStream->writeLine(IOStream, luaL_typename(L,i));
            IOStream->writeLine(IOStream, ">");
        }
    }
    IOStream->writeLine(IOStream, "\n");
    return 0;
}

/*******************************************************************************
* luaCLI_splitline
*
* DESCRIPTION:
*       execute command
*
* INPUTS:
*       L       - lua_State
*        1st    - line
*
* OUTPUTS:
*       luaPtr
*
* RETURNS:
*       table of words
*
* COMMENTS:
*       None
*
*******************************************************************************/
static int luaCLI_splitline(lua_State *L)
{
    int was_space = 1;
    int s, p;
    const char *line;
    int linelen;
    if (lua_type(L, 1) != LUA_TSTRING)
    {
        return 0;
    }
    lua_settop(L, 1);
    line = lua_tostring(L, 1);
    linelen = (int)lua_objlen(L, 1);
    lua_newtable(L); /* 2 */
    for (s = p = 0; p < linelen; p++)
    {
        int is_space = 0;
        switch(line[p])
        {
            case 0:
            case ' ':
            case '\t':
            case '\r':
            case '\n':
            case '\v':
                is_space = 1;
                break;

        }
        if (was_space == is_space)
            continue;
        if (was_space == 1)
        {
            s = p;
        }
        else
        {
            lua_pushlstring(L, line + s, p - s);
            lua_rawseti(L, 2, (int)lua_objlen(L, 2) + 1);
        }
        was_space = is_space;
    }
    if (was_space == 0 && s < p)
    {
        lua_pushlstring(L, line + s, p - s);
        lua_rawseti(L, 2, (int)lua_objlen(L, 2) + 1);
    }
    return 1;
}

/*******************************************************************************
* luaCLI_registerCfunction
*
* DESCRIPTION:
*       Add commands
*
* INPUTS:
*       L       - lua_State
*        1st    - C function name
*        2nd    - lua prefix (optional)
*
* OUTPUTS:
*       luaPtr
*
* RETURNS:
*       None
*
* COMMENTS:
*       None
*
*******************************************************************************/
static int luaCLI_registerCfunction(lua_State *L)
{
    lua_CFunction funcPtr;
    LUA_CLI_GETFUNCBYNAME_T  getFunctionByName;

    if (!lua_isstring(L, 1))
    {
        lua_getglobal(L, "print");
        lua_pushstring(L, "luaCLI_registerCfunction() error: "
            "first parameter must be string");
        lua_call(L, 1, 0);
        return 0;
    }
    getFunctionByName = (LUA_CLI_GETFUNCBYNAME_T)
            luaCLIEngineGetData(L, "_getFunctionByName", 1);
    if (getFunctionByName == NULL)
        return 0;
    if (lua_gettop(L) > 1)
    {
        lua_settop(L, 2);
        if (!lua_isstring(L, 2))
        {
            lua_getglobal(L, "print");
            lua_pushstring(L, "luaCLI_registerCfunction() error: "
                "second parameter must be string");
            lua_call(L, 1, 0);
            return 0;
        }
    }
    else
    {
        lua_pushstring(L, "");
    }
    lua_pushvalue(L, 1);
    lua_concat(L, 2);

    funcPtr = (lua_CFunction) getFunctionByName(lua_tostring(L, 1));
    if (!funcPtr)
    {
        lua_getglobal(L, "print");
        lua_pushstring(L, "luaCLI_registerCfunction() error: "
            "symbol not found: ");
        lua_pushvalue(L, 1);
        lua_concat(L, 2);
        lua_call(L, 1, 0);
        return 0;
    }
    lua_register(L, lua_tostring(L, 2), funcPtr);
    /* add function to registry */
    lua_getglobal(L, "cmdLuaCLI_CfunctionsRegistry");
    lua_pushcfunction(L, funcPtr);
    lua_setfield(L, -2, lua_tostring(L, 2));
    return 0;
}

/*******************************************************************************
* luaCLI_callCfunction
*
* DESCRIPTION:
*       Call Lua C function without registering
*
* INPUTS:
*       L       - lua_State
*        1st    - C function name
*        ...
*
* OUTPUTS:
*       luaPtr
*
* RETURNS:
*       None
*
* COMMENTS:
*       None
*
*******************************************************************************/
static int luaCLI_callCfunction(lua_State *L)
{
    lua_CFunction funcPtr;
    LUA_CLI_GETFUNCBYNAME_T  getFunctionByName;
    if (!lua_isstring(L, 1))
    {
        lua_getglobal(L, "print");
        lua_pushstring(L, "luaCLI_callCfunction() error: "
            "first parameter must be string");
        lua_call(L, 1, 0);
        return 0;
    }
    getFunctionByName = (LUA_CLI_GETFUNCBYNAME_T)
            luaCLIEngineGetData(L, "_getFunctionByName", 1);
    if (getFunctionByName == NULL)
        return 0;

    funcPtr = (lua_CFunction) getFunctionByName(lua_tostring(L, 1));
    if (!funcPtr)
    {
        lua_getglobal(L, "print");
        lua_pushstring(L, "luaCLI_callCfunction() error: "
            "symbol not found: ");
        lua_pushvalue(L, 1);
        lua_concat(L, 2);
        lua_call(L, 1, 0);
        return 0;
    }
    lua_pushcfunction(L, funcPtr);
    lua_replace(L, 1);
    lua_call(L, lua_gettop(L) - 1, LUA_MULTRET);
    return lua_gettop(L);
}

/*******************************************************************************
* luaCLI_require
*
* DESCRIPTION:
*       Builtin command: require
*
* INPUTS:
*       L       - lua_State
*        1st    - module name
*
* OUTPUTS:
*       luaPtr
*
* RETURNS:
*       None
*
* COMMENTS:
*       None
*
*******************************************************************************/
static int luaCLI_require(lua_State *L)
{
    if (lua_istable(L, 1))
    {
        lua_rawgeti(L, 1, 1);
        lua_replace(L, 1);
    }
    if (!lua_isstring(L, 1))
    {
        lua_getglobal(L, "print");
        lua_pushstring(L, "luaCLI_require() error: "
            "parameter must be a string");
        lua_call(L, 1, 0);
        return 0;
    }
    /* load libLuaCliDefs.lua */
    lua_getglobal(L, "require");
    lua_pushvalue(L, 1);
    if (lua_pcall(L, 1, 0, 0) != 0)
    {
        lua_getglobal(L, "print");
        lua_pushstring(L, "Failed to load ");
        lua_pushvalue(L, 1);
        lua_pushstring(L, ": ");
        lua_pushvalue(L, -5);
        lua_concat(L, 4);
        lua_call(L, 1, 0);
    }
    return 0;
}

/*******************************************************************************
* luaCLIEngine_alloc_wrapper
*
* DESCRIPTION:
*       The type of the memory-allocation function used by Lua states.
*       This allocator function provides a functionality similar to realloc,
*       but not exactly the same
*
* INPUTS:
*       ud      - user defined pointer, ignored
*       ptr     - a pointer to the block being allocated/reallocated/freed
*       osize   - the original size of the block
*       nsize   - the new size of the block
*
* OUTPUTS:
*       None
*
* RETURNS:
*       pointer to allocated/reallocated block
*
* COMMENTS:
*       (From Lua documentation)
*       ptr is NULL if and only if osize is zero.
*       When nsize is zero, the allocator must return NULL;
*       if osize is not zero, it should free the block pointed to by ptr.
*       When nsize is not zero, the allocator returns NULL if and only if
*         it cannot fill the request.
*       When nsize is not zero and osize is zero, the allocator should
*         behave like malloc.
*       When nsize and osize are not zero, the allocator behaves like realloc.
*       Lua assumes that the allocator never fails when osize >= nsize
*
*******************************************************************************/
static void* luaCLIEngine_alloc_wrapper
(
    LUA_CLI_MALLOC_STC  *memMgr,
    void                *ptr,
    size_t              osize,
    size_t              nsize
)
{
    void    *nptr;
    if (nsize == 0)
    {
        if (ptr != NULL)
        {
            memMgr->free(ptr);
#ifdef LUACLI_MEMSTAT
            memMgr->totalUsed -= (int)osize;
            memMgr->nChunks--;
#endif /* LUACLI_MEMSTAT */
        }
        return NULL;
    }

    nptr = memMgr->malloc(nsize);
    if (nptr == NULL && osize >= nsize)
    {
        return ptr;
    }
#ifdef LUACLI_MEMSTAT
    memMgr->totalUsed += (int)nsize;
    memMgr->nChunks++;
#endif /* LUACLI_MEMSTAT */
    if (nptr != NULL && ptr != NULL)
    {
        cpssOsMemCpy(nptr, ptr, (osize < nsize) ? osize : nsize);
    }
    if (ptr != NULL)
    {
        memMgr->free(ptr);
#ifdef LUACLI_MEMSTAT
        memMgr->totalUsed -= (int)osize;
        memMgr->nChunks--;
#endif /* LUACLI_MEMSTAT */
    }
    return nptr;
}

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
)
{
    lua_State *L;

    L = lua_newstate((lua_Alloc)luaCLIEngine_alloc_wrapper, memMgr);
    if (L == NULL)
        return NULL;

    lua_pushlightuserdata(L, memMgr);
    lua_setglobal(L, "_memMgr");
    lua_pushlightuserdata(L, IOStream);
    lua_setglobal(L, "_IOStream");
    lua_pushlightuserdata(L, fs);
    lua_setglobal(L, "_genFS");
    lua_pushlightuserdata(L, getFunctionByName);
    lua_setglobal(L, "_getFunctionByName");

    luaL_openlibs(L);
    /* register(override) print function */
    lua_register(L, "print", luaCLIEnginePrint);
    lua_register(L, "splitline", luaCLI_splitline);
    lua_register(L, "cmdLuaCLI_require", luaCLI_require);
    /* create C functions registry */
    lua_newtable(L);
    lua_setglobal(L, "cmdLuaCLI_CfunctionsRegistry");
    lua_register(L, "cmdLuaCLI_registerCfunction", luaCLI_registerCfunction);
    lua_register(L, "cmdLuaCLI_callCfunction", luaCLI_callCfunction);

#ifdef CMD_LUA_GLOBAL
    /* register global storage for Lua data */
    lua_register(L, "globalStore", luaGlobalStore);
    lua_register(L, "globalGet", luaGlobalGet);
#endif /* CMD_LUA_GLOBAL */

    if (fs != NULL)
    {
        luaEngineFSregisterLoader(L);
        luaEngineFSregisterAPI(L);
    }

#ifdef CMD_LUA_NET
    luaCLIEngineNetRegisterAPI(L);
#endif /* CMD_LUA_NET */

    /* register generic wrapper and generic types */
    lua_pushcfunction(L, luaEngineGenWrapper_init);
    lua_call(L, 0, 0);

    return L;
}


