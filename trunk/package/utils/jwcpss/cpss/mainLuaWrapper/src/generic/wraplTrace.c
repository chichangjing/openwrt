/*******************************************************************************
*              (c), Copyright 2013, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* wraplTrace.c
*
* DESCRIPTION:
*       wrlTrace() implementation
*
* DEPENDENCIES:
*
* COMMENTS:
*
* FILE REVISION NUMBER:
*       $Revision: 1 $
**************************************************************************/
#include <cpss/generic/cpssTypes.h>
#include <cpss/extServices/private/prvCpssBindFunc.h>
#include <extUtils/luaCLI/luaCLIEngine.h>

/************* Defines ********************************************************/
#define TRC_CALLING_HDR "<<<call "
#define TRC_CALLING_SEP ", "
#define TRC_CALLING_TAIL ")\n>>>\n"
#define TRC_RETURNING_HDR "<<<ret "
#define TRC_RETURNING_SEP ", "
#define TRC_RETURNING_TAIL "\n>>>\n"

/************* Data ******************************************************/
static int traceEnabled = GT_FALSE;
static int traceFd = -1;

static const char *ignoreFuncs[] = {
    "wrlDevList", "wrlIsDevIdValid", "wrlCpssDevicePortNumberGet",
    NULL
};


/************* Functions ******************************************************/
/*******************************************************************************
* traceHook
*
* DESCRIPTION:
*       A trace hook function for Lua calls
*
* INPUTS:
*       L       - lua state
*       ar      - debug information filled by Lua
*
* RETURNS:
*       None
*
* COMMENTS:
*       None
*
*******************************************************************************/
static void traceHook(lua_State *L, lua_Debug *ar)
{
    lua_CFunction funcPtr;
    int         n, c, i;
    int         found = 0;
    genFS_PTR fs = (genFS_PTR)luaCLIEngineGetData(L, "_genFS", 0);

    if (!traceEnabled)
    {
        return;
    }
    /* get pointer to function */
    lua_getinfo(L, "nf", ar);
    if (!lua_iscfunction(L, -1))
    {
        lua_pop(L, 1);
        return;
    }
    funcPtr = lua_tocfunction(L, -1);
    lua_pop(L, 1);

    lua_pushstring(L, ar->name);
    n = lua_gettop(L);

    /* search function in registry */
    lua_getglobal(L, "cmdLuaCLI_CfunctionsRegistry");
    if (!lua_istable(L, -1))
    {
        lua_pop(L, 1);
        return;
    }
    lua_pushnil(L);  /* first key */
    while (lua_next(L, -2) != 0)
    {
        /* uses 'key' (at index -2) and 'value' (at index -1) */
        if (lua_isstring(L, -2) && lua_iscfunction(L, -1))
        {
            if (funcPtr == lua_tocfunction(L, -1))
            {
                found = 1;
                lua_pop(L, 1);
                lua_replace(L, n);
                break;
            }
        }
        /* removes 'value'; keeps 'key' for next iteration */
        lua_pop(L, 1);
    }
    lua_settop(L, n);
    if (!found)
    {
        lua_pop(L, 1);
        return;
    }

    /* function found, stack[n] == function_name */
    /* ignore some functions */
    for (i = 0; ignoreFuncs[i]; i++)
    {
        lua_pushstring(L, ignoreFuncs[i]);
        if (lua_equal(L, n, -1))
        {
            lua_pop(L,2);
            return;
        }
        lua_pop(L,1);
    }
    /* Handle event */
    switch (ar->event)
    {
        case LUA_HOOKCALL:
            lua_pushliteral(L, TRC_CALLING_HDR);
            lua_insert(L, -2);
            lua_pushliteral(L, "(");
            c = 3;
            for (i = 1; i < n; i++)
            {
                if (i > 1)
                {
                    lua_pushliteral(L, TRC_CALLING_SEP);
                    c++;
                }
                lua_getglobal(L, "to_string");
                lua_pushvalue(L, i);
                lua_call(L, 1, 1);
                c++;
            }
            lua_pushliteral(L, TRC_CALLING_TAIL);
            lua_concat(L, c+1);
            break;
        case LUA_HOOKRET:
            lua_pushliteral(L, TRC_RETURNING_HDR);
            lua_insert(L, -2);
            lua_pushliteral(L, " ");
            c = 3;
            for (i = 0; i < ar->currentline; i++)
            {
                if (i > 0)
                {
                    lua_pushliteral(L, TRC_RETURNING_SEP);
                    c++;
                }
                lua_getglobal(L, "to_string");
                lua_pushvalue(L, n - ar->currentline + i);
                lua_call(L, 1, 1);
                c++;
            }
            lua_pushliteral(L, TRC_RETURNING_TAIL);
            lua_concat(L, c+1);
            break;
        default:
            lua_pop(L, 1);
            return;
    }
    if (fs != NULL)
    {
        fs->write(traceFd, lua_tostring(L, -1), lua_strlen(L, -1));
    }
    lua_pop(L, 1);

    return;
}

/*******************************************************************************
* wrlTraceEnable: lua function
*
* DESCRIPTION:
*       Enable disable Lua -> C trace calls to file
*
* INPUTS:
*       enable      - integer value
*                     0: disable trace
*                     1: enable call hooks
*                     2: enable return hooks
*                     3: enable both call and return hooks
*       fd          - descriptor to write trace to (see fd.open() )
*
* RETURNS:
*       bool
*
* COMMENTS:
*       None
*
*******************************************************************************/
int wrlTraceEnable(lua_State *L)
{
    lua_Integer enable;

    if (!lua_isnumber(L, 1))
    {
        lua_pushboolean(L, 0);
        return 1;
    }
    enable = (int)lua_tointeger(L, 1);
    if (enable != 0)
    {
        if (!lua_isnumber(L, 2))
        {
            lua_pushboolean(L, 0);
            return 1;
        }
        traceFd = (int)lua_tointeger(L, 2);
    }
    traceEnabled = (int)enable;
    if (enable != 0)
    {
        int flags = 0;
        flags = 0;
        if (enable & 1)
            flags |= LUA_MASKCALL;
        if (enable & 2)
            flags |= LUA_MASKRET;
        lua_sethook(L, traceHook, flags, 0);
    }
    else
    {
        lua_sethook(L, (lua_Hook)NULL, 0, 0);
    }
    lua_pushboolean(L, 1);
    return 1;
}
