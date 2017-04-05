/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* wraplTaskCreate.c
*
* DESCRIPTION:
*       luaTaskCreate() implementation
*
* DEPENDENCIES:
*
* COMMENTS:
*
* FILE REVISION NUMBER:
*       $Revision: 5 $
**************************************************************************/
#include <gtOs/gtOsTask.h>
#include <gtOs/gtOsStr.h>
#include <gtOs/gtOsIo.h>
#include <lua.h>
#include <lauxlib.h>
#include <gtOs/gtOsMsgQ.h>
#include <cpss/extServices/private/prvCpssBindFunc.h>
#include <string.h>

#include <extUtils/luaCLI/luaCLIEngine.h>

/************* Externs ********************************************************/
int luaEngineFS_loadfile(lua_State *L, const char *name);

/************* Defines ********************************************************/

/************* Typedefs *******************************************************/

/************* Functions ******************************************************/
/************* Data ******************************************************/
static GT_TASK      receiveTaskId=0;
static GT_MSGQ_ID   receiveTaskQueueId;

/*******************************************************************************
* lua_copy
*
* DESCRIPTION:
*       Copy top value from top of L1 to L2
*
* INPUTS:
*       L1
*       L2
*
* RETURNS:
*       0 if ok
*
* COMMENTS:
*       None
*
*******************************************************************************/
static int lua_copy(lua_State* L1, lua_State* L2)
{
    switch (lua_type(L1, -1))
    {
        case LUA_TNIL:
            lua_pushnil(L2);
            return 0;
        case LUA_TBOOLEAN:
            lua_pushboolean(L2, lua_toboolean(L1, -1));
            return 0;
        case LUA_TLIGHTUSERDATA:
            lua_pushlightuserdata(L2, lua_touserdata(L1, -1));
            return 0;
        case LUA_TNUMBER:
            lua_pushnumber(L2, lua_tonumber(L1, -1));
            return 0;
        case LUA_TSTRING:
            {
                const char *str;
                size_t len = 0;
                str = lua_tolstring(L1, -1, &len);
                lua_pushlstring(L2, str, len);
            }
            return 0;
        case LUA_TUSERDATA:
            lua_newuserdata(L2, lua_objlen(L1, -1));
            memcpy(lua_touserdata(L2,-1), lua_touserdata(L1, -1), lua_objlen(L1, -1));
            return 0;
        case LUA_TFUNCTION:
            if (lua_iscfunction(L1, -1))
            {
                lua_pushcfunction(L2, lua_tocfunction(L1, -1));
                return 0;
            }
            return 1;
        case LUA_TTABLE:
            break;
        default:
            return 1;
    }
    /* table */
    lua_newtable(L2);
    lua_pushnil(L1); /* first key */
    while (lua_next(L1, -2))
    {
        /* L1: tbl key value */
        if (lua_copy(L1, L2))
        {
            lua_pop(L1, 2);
            lua_pop(L2, 1);
            return 1;
        }
        /* L2: tbl value */

        lua_pop(L1, 1); /* remove value, keep key for next iteration */
        if (lua_copy(L1, L2))
        {
            lua_pop(L1, 1);
            lua_pop(L2, 2);
            return 1;
        }
        /* L2: tbl value key */
        lua_insert(L2, -2); /* L2: tbl key value */
        lua_settable(L2, -3);
    }

    return 0;
}

/*******************************************************************************
* luaTaskWrapper
*
* DESCRIPTION:
*       task wrapper
*
* INPUTS:
*       L       - Lua state
*
* OUTPUTS:
*       None
*
* RETURNS:
*       None
*
* COMMENTS:
*       None
*
*******************************************************************************/
static GT_VOID __TASKCONV luaTaskWrapper
(
    lua_State* L
)
{
    int status;
    /* run loaded script */
    if ((status = lua_pcall(L, 0, LUA_MULTRET, 0)) != 0)
    {
        /* handle errors */
        osPrintf("***Lua");
        switch (status)
        {
            case LUA_ERRRUN:
                osPrintf(" runtime");
                break;
            case LUA_ERRMEM:
                osPrintf(" allocation");
                break;
            case LUA_ERRERR:
                osPrintf(" error handling");
                break;
			default:
			     osPrintf(" unknown error");
        }
        if (lua_isstring(L,-1))
        {
            osPrintf(" error: %s\n",lua_tostring(L,-1));
        }
        else
        {
            osPrintf(" error (no more info)\n");
        }
    }
    lua_close(L);
}

/*******************************************************************************
* lua_log_console
*
* DESCRIPTION:
*       A wraper for osPrintf(), should be used from lua tasks
*       Doesn't require CPSS_API_CALL_LOCK() as done in cpssGenWrapper()
*
* INPUTS:
*       L       - Lua state
*
* OUTPUTS:
*       None
*
* RETURNS:
*       None
*
* COMMENTS:
*       None
*
*******************************************************************************/
static int lua_log_console(lua_State *L)
{
    if (lua_isstring(L, 1))
    {
        osPrintf("%s\n",lua_tostring(L, 1));
    }
    return 0;
}

/*******************************************************************************
* luaTaskCreate: lua function
*
* DESCRIPTION:
*       Create a new lua task
*
* INPUTS:
*       name        - task name
*       script_name - script to load
*       param       - (optional) parameter to script
*                     will be set as task_Param
*
* RETURNS:
*       string if error, taskID if successful
*
* COMMENTS:
*       None
*
*******************************************************************************/
int luaTaskCreate
(
    lua_State *L
)
{
    lua_State   *L2;
    int         ret;
    GT_STATUS   status;
    GT_TASK     taskId;
    GT_CHAR     taskName[32];
    LUA_CLI_MALLOC_STC     *memMgr;
    genFS_PTR               fs;
    LUA_CLI_GETFUNCBYNAME_T getFunctionByName;

    /* check params */
    if (lua_gettop(L) < 2)
    {
        lua_pushliteral(L, "not enough parameters");
        return 1;
    }
    if (!lua_isstring(L, 1) || !lua_isstring(L, 2))
    {
        lua_pushliteral(L, "bad parameter");
        return 1;
    }

    memMgr = (LUA_CLI_MALLOC_STC*)luaCLIEngineGetData(L, "_memMgr", 0);
    fs = (genFS_PTR)luaCLIEngineGetData(L, "_genFS", 0);
    getFunctionByName = (LUA_CLI_GETFUNCBYNAME_T)
                luaCLIEngineGetData(L, "_getFunctionByName", 0);

    /* create new state for task */
    L2 = luaCLIEngine_newstate(memMgr, NULL, fs, getFunctionByName);
    if (L2 == NULL)
    {
        lua_pushliteral(L, "failed to create new Lua instance");
        return 1;
    }
    lua_register(L2, "log_console", lua_log_console);

    /* copy task name to L2 */
    lua_pushstring(L2, lua_tostring(L, 1));
    lua_setglobal(L2, "task_Name");
    /* copy script name to L2 */
    lua_pushstring(L2, lua_tostring(L, 2));
    lua_setglobal(L2, "task_Script");
    /* copy param to L2 */
    if (lua_gettop(L) > 2)
    {
        lua_settop(L, 3);
        if (lua_copy(L, L2) != 0)
        {
            lua_pushliteral(L, "failed to copy task param, wrong data type (function, thread)");
            lua_close(L2);
            return 1;
        }
        lua_setglobal(L2, "task_Param");
    }

    /* load script to new state, don't run it now */
    ret = luaEngineFS_loadfile(L2, lua_tostring(L, 2));

    if (ret != 0)
    {
        lua_pushfstring(L, "luaL_loadfile(\"%s\"): ", lua_tostring(L, 2));
        switch (ret)
        {
            case LUA_ERRSYNTAX:
                lua_pushliteral(L, "syntax error");
                break;
            case LUA_ERRMEM:
                lua_pushliteral(L, "memory allocation error");
                break;
            case LUA_ERRFILE:
                lua_pushliteral(L, "cannot open/read file");
                break;
            default:
                lua_pushfstring(L, "unknown error: %d", ret);
        }
        lua_concat(L, 2);
        lua_close(L2);
        return 1;
    }

    osStrNCpy(taskName, lua_tostring(L, 1), sizeof(taskName));
    taskName[sizeof(taskName)-1] = 0;
    status = osTaskCreate(
        taskName,
        5, /* prio */
        32768, /* stack */
        (unsigned (__TASKCONV *)(void*))luaTaskWrapper, /* entry point */
        L2, /* arg */
        &taskId);

    if (status!=GT_OK) 
    {
        lua_pushliteral(L,"Task could not be created");
        return 1;
    }

    if(osStrNCmp(lua_tostring(L, 1),"RxTxTask",8)==0)
    {
        receiveTaskId=taskId;
        receiveTaskQueueId=lua_tointeger(L,3);
    }

    lua_pushnumber(L, taskId);
    return 1;
}

/*TODO: remove after test done */
#undef TEST
#ifdef TEST
/***********************************/
/***** TEST ************************/
/***********************************/
#include <gtOs/gtOsMsgQ.h>

static GT_VOID __TASKCONV cTestTask
(
    GT_MSGQ_ID msgqId
)
{
    GT_U32  msg;
    msg = 201;
    osMsgQSend(msgqId, &msg, sizeof(msg), OS_MSGQ_WAIT_FOREVER);
    msg = 202;
    osMsgQSend(msgqId, &msg, sizeof(msg), OS_MSGQ_WAIT_FOREVER);
}

/*******************************************************************************
* luaTaskCreate: lua function
*
* DESCRIPTION:
*       Create a new C task with msgqId passed as parameter
*
* INPUTS:
*       name        - task name
*       script_name - script to load
*       param
*
* RETURNS:
*       status
*
* COMMENTS:
*       None
*
*******************************************************************************/
int luaTestTaskCreate
(
    lua_State *L
)
{
    GT_MSGQ_ID  msgqId;
    GT_TASK     taskId;
    if (lua_gettop(L) < 1)
    {
        lua_getglobal(L, "print");
        lua_pushliteral(L, "luaTestTaskCreate(): no required parameter");
        lua_call(L, 1, 0);
        return 0;
    }
    if (!lua_istable(L, 1))
    {
bad_param:
        lua_getglobal(L, "print");
        lua_pushliteral(L, "luaTestTaskCreate(): wrong parameter");
        lua_call(L, 1, 0);
        return 0;
    }
    lua_getfield(L, 1, "msgqId");
    if (!lua_isnumber(L, -1))
        goto bad_param;
    msgqId = (GT_MSGQ_ID)lua_tonumber(L, -1);

    osTaskCreate(
        "ctest", /* name */
        5, /* prio */
        32768, /* stack */
        (unsigned (__TASKCONV *)(void*))cTestTask, /* entry point */
        (void*)msgqId, /* arg */
        &taskId);
    return 0;
}
#endif /* TEST */


/*******************************************************************************
* luaTaskSuspend: lua function
*
* DESCRIPTION:
*       Suspend a thread running a lua script
*
* INPUTS:
*       taskID        - the id of the task to suspend
*
* RETURNS:
*       string if error, GT_OK if successful
*
* COMMENTS:
*       None
*
*******************************************************************************/
int luaTaskSuspend
(
    lua_State *L
)
{
    GT_TASK taskID;

    if (lua_gettop(L) != 1)
    {
        lua_pushliteral(L, "Incorrect parameters, only task ID is needed");
        return 1;
    }

    if (!lua_isnumber(L,1))
    {
        lua_pushliteral(L, "Parameter must be a task ID");
        return 1;
    }

    taskID= lua_tointeger(L,1);

    if (osTaskSuspend(taskID)!=GT_OK)
    {
        lua_pushliteral(L, "Could not suspend task");
        return 1;
    }

    lua_pushnumber(L,GT_OK);
    return 1;
}


/*******************************************************************************
* luaTaskResume: lua function
*
* DESCRIPTION:
*       Resume a thread running a lua script
*
* INPUTS:
*       taskID        - the id of the task to resume
*
* RETURNS:
*       string if error, GT_OK if successful
*
* COMMENTS:
*       None
*
*******************************************************************************/
int luaTaskResume
(
    lua_State *L
)
{
    GT_TASK taskID;

    if (lua_gettop(L) != 1)
    {
        lua_pushliteral(L, "Incorrect parameters, only task ID is needed");
        return 1;
    }

    if (!lua_isnumber(L,1))
    {
        lua_pushliteral(L, "Parameter must be a task ID");
        return 1;
    }

    taskID= lua_tointeger(L,1);

    if (osTaskResume(taskID)!=GT_OK)
    {
        lua_pushliteral(L, "Could not resume task");
        return 1;
    }

    lua_pushnumber(L,GT_OK);
    return 1;
}

/*******************************************************************************
* luaTaskResume: lua function
*
* DESCRIPTION:
*       Returns the status of the receive task
*
* INPUTS:
*       none
*
* RETURNS:
*       nil if error,  taskID and taskQueueID if successful
*
* COMMENTS:
*       None
*
*******************************************************************************/
int luaReceiveTaskStatus
(
    lua_State *L
)
{
    if (receiveTaskId) 
    {
        lua_pushnumber(L,receiveTaskId);
        lua_pushnumber(L,receiveTaskQueueId);
        return 2;
    }
    lua_pushnil(L);
    return 1;
}

/*******************************************************************************
* luaTaskDelay: lua function
*
* DESCRIPTION:
*       Delays the task for the specified time in milliseconds.
*
* INPUTS:
*       mils    - Delay time in milliseconds
*
* RETURNS:
*       string if error, GT_OK if successful
*
* COMMENTS:
*       None
*
*******************************************************************************/
int luaTaskDelay
(
    lua_State *L
)
{
    GT_U32 mils;
    if (lua_gettop(L) != 1)
    {
        lua_pushliteral(L, "Incorrect parameters, only time in milliseconds is required");
        return 1;
    }

    if (!lua_isnumber(L,1))
    {
        lua_pushliteral(L, "Parameter must be time in milliseconds");
        return 1;
    }

    mils= lua_tointeger(L,1);

    if (cpssOsTimerWkAfter(mils)!=GT_OK)
    {
        lua_pushliteral(L, "Could not delay task");
        return 1;
    }

    lua_pushnumber(L,GT_OK);
    return 1;
}
