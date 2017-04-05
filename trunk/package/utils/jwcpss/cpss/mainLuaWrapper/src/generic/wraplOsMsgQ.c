/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* wraplOsMsgQ.c
*
* DESCRIPTION:
*       A lua wrapper for osMsgQ*()
*
* DEPENDENCIES:
*
* COMMENTS:
*
* FILE REVISION NUMBER:
*       $Revision: 2 $
**************************************************************************/
#include <gtOs/gtOsMsgQ.h>
#include <cmdShell/os/cmdExecute.h>
#include <lua.h>


/************* Defines ********************************************************/

/************* Typedefs *******************************************************/

/************* Functions ******************************************************/
static int getfunc(lua_State *L, const char *prefix, int idx, const char *fname)
{
    lua_CFunction funcPtr;

    lua_pushstring(L, prefix);
    lua_pushvalue(L, 2);
    lua_concat(L, 2);

    lua_getglobal(L, lua_tostring(L, -1));
    if (lua_isfunction(L, -1))
    {
        lua_setfield(L, idx, fname);
        lua_pop(L, 1); /* name */
        return 0;
    }

    /* not found, try C function */
    lua_pop(L, 1); /* nil */

    funcPtr = (lua_CFunction) osShellGetFunctionByName(lua_tostring(L, -1));
    if (funcPtr)
    {
        lua_pushcfunction(L, funcPtr);
        lua_setfield(L, idx, fname);
        lua_pop(L, 1); /* name */
        return 0;
    }

    lua_pushfstring(L, "function '%s' is not defined", lua_tostring(L, -1));
    return 1;
}

/*******************************************************************************
* luaMsgQInitStruct: lua function
*
* DESCRIPTION:
*       Initialize structure for a message queue.
*
* INPUTS:
*       name       - message queue name
*       msgType    - message type string
*
* RETURNS:
*       msgqId     - table: { msgType = 'type_name', name='msgqname', to_c=<func>, to_lua=<func> }
*       integer or string on error
*
* COMMENTS:
*       None
*
*******************************************************************************/
static int luaMsgQInitStruct
(
    lua_State *L
)
{
    int retIdx;

    if (lua_gettop(L) < 2)
    {
        lua_pushliteral(L, "no enough parameters");
        return 1;
    }
    if (!lua_isstring(L, 1))
    {
        lua_pushliteral(L, "bad parameter 1");
        return 1;
    }
    if (!lua_isstring(L, 2))
    {
        lua_pushliteral(L, "bad parameter 2");
        return 1;
    }
    lua_newtable(L);
    retIdx = lua_gettop(L);
    lua_pushvalue(L, 1);
    lua_setfield(L, retIdx, "name");
    lua_pushvalue(L, 2);
    lua_setfield(L, retIdx, "msgType");
    /* get to_c and to_lua */
    if (getfunc(L, "mgmType_to_c_", retIdx, "to_c") != 0)
        return 1;
    if (getfunc(L, "mgmType_to_lua_", retIdx, "to_lua") != 0)
        return 1;

    lua_settop(L, retIdx);
    return 1;
}

/*******************************************************************************
* luaMsgQCreate: lua function
*
* DESCRIPTION:
*       Create and initialize a message queue.
*
* INPUTS:
*       name       - message queue name
*       msgType    - message type string
*       maxMsgs    - max messages in queue
*
* RETURNS:
*       msgqId     - table: { msgType = 'type_name', msgqId = messageId, name='msgqname' }
*       integer or string on error
*
* COMMENTS:
*       None
*
*******************************************************************************/
int luaMsgQCreate
(
    lua_State *L
)
{
    GT_U32        maxMsgs = 1;
    GT_U32        maxMsgSize;
    GT_MSGQ_ID    msgqId;
    GT_STATUS     status;
    int           retIdx;

    if (lua_gettop(L) < 3)
    {
        lua_pushliteral(L, "no enough parameters");
        return 1;
    }
    if (!lua_isstring(L, 1))
    {
        lua_pushliteral(L, "bad parameter 1");
        return 1;
    }
    if (!lua_isstring(L, 2))
    {
        lua_pushliteral(L, "bad parameter 2");
        return 1;
    }
    if (!lua_isnumber(L, 3))
    {
        lua_pushliteral(L, "bad parameter 3");
        return 1;
    }
    maxMsgs = (GT_U32)lua_tonumber(L, 3);

    lua_pushcfunction(L, luaMsgQInitStruct);
    lua_pushvalue(L, 1);
    lua_pushvalue(L, 2);
    lua_call(L, 2, 1);
    if (!lua_istable(L, -1))
    {
        return 1;
    }
    retIdx = lua_gettop(L);

    /* calculate message size */
    lua_getfield(L, retIdx, "to_c");
    lua_pushnil(L);
    lua_call(L, 1, 1);
    if (lua_type(L, -1) == LUA_TNUMBER)
    {
        maxMsgSize = sizeof(GT_U32);
    }
    else if (lua_type(L, -1) == LUA_TUSERDATA)
    {
        maxMsgSize = lua_objlen(L, -1);
    }
    else
    {
        /* wrong type */
        lua_pushliteral(L, "type not supported");
        return 1;
    }
    lua_pop(L, 1);
    /* now create queue */

    status = osMsgQCreate(lua_tostring(L, 1), maxMsgs, maxMsgSize, &msgqId);
    if (status != GT_OK)
    {
        lua_pushinteger(L, status);
        return 1;
    }
    lua_pushnumber(L, msgqId);
    lua_setfield(L, retIdx, "msgqId");

    lua_settop(L, retIdx);
    return 1;
}
/*******************************************************************************
* luaMsgQAttach: lua function
*
* DESCRIPTION:
*       Create a Lua queue connected to C msgq Id
*
* INPUTS:
*       name       - message queue name
*       msgType    - message type string
*       msgqId     - message queue id
*
* RETURNS:
*       msgqId     - table: { msgType = 'type_name', msgId = messageId, name='msgqname' }
*
* COMMENTS:
*       None
*
*******************************************************************************/
int luaMsgQAttach
(
    lua_State *L
)
{
    int           retIdx;

    if (lua_gettop(L) < 3)
    {
        lua_pushliteral(L, "no enough parameters");
        return 1;
    }
    if (!lua_isstring(L, 1))
    {
        lua_pushliteral(L, "bad parameter 1");
        return 1;
    }
    if (!lua_isstring(L, 2))
    {
        lua_pushliteral(L, "bad parameter 2");
        return 1;
    }
    if (!lua_isnumber(L, 3))
    {
        lua_pushliteral(L, "bad parameter 2");
        return 1;
    }

    lua_pushcfunction(L, luaMsgQInitStruct);
    lua_pushvalue(L, 1);
    lua_pushvalue(L, 2);
    lua_call(L, 2, 1);
    if (!lua_istable(L, -1))
    {
        return 1;
    }
    retIdx = lua_gettop(L);

    lua_pushvalue(L, 3);
    lua_setfield(L, retIdx, "msgqId");

    return 1;
}


#define LUA_MSGQ_BEGIN(nparams)    \
    GT_MSGQ_ID  msgqId; \
    if (lua_gettop(L) < nparams) \
    { \
        lua_pushliteral(L, "no enough parameters"); \
        return 1; \
    } \
    if (!lua_istable(L, 1)) \
    { \
        lua_pushliteral(L, "bad parameter 1"); \
        return 1; \
    } \
    lua_getfield(L, 1, "msgqId"); \
    if (!lua_isnumber(L, -1)) \
    { \
        lua_pushliteral(L, "bad parameter 1"); \
        return 1; \
    } \
    msgqId = (GT_MSGQ_ID)lua_tonumber(L, -1); \
    lua_pop(L, 1);

/*******************************************************************************
* luaMsgQDelete
*
* DESCRIPTION:
*       Delete message queue
*
* INPUTS:
*       msgqId - message queue Id table
*
* RETURNS:
*       status or string if error
*
* COMMENTS:
*       None
*
*******************************************************************************/
int luaMsgQDelete
(
    lua_State *L
)
{
    GT_STATUS   status;

    LUA_MSGQ_BEGIN(1);

    status = osMsgQDelete(msgqId);

    lua_pushnumber(L, status);
    return 1;
}

/*******************************************************************************
* luaMsgQSend
*
* DESCRIPTION:
*       Send message to queue
*
* INPUTS:
*       msgqId       - message queue Id table
*       message      - message data pointer
*       timeOut      - (optional) timeout in miliseconds
*                      default OS_MSGQ_WAIT_FOREVER
*
* RETURNS:
*       status or error string
*
* COMMENTS:
*       None
*
*******************************************************************************/
int luaMsgQSend
(
    lua_State *L
)
{
    GT_STATUS status;
    GT_PTR msgPtr = NULL;
    GT_U32 msgSize = 0;
    GT_U32 intMsg = 0;
    GT_U32 timeOut = OS_MSGQ_WAIT_FOREVER;

    LUA_MSGQ_BEGIN(2);

    if (lua_gettop(L) > 2)
    {
        if (lua_isnumber(L, 3))
        {
            timeOut = (GT_U32)lua_tonumber(L, 3);
            if (timeOut == 0)
                timeOut = OS_MSGQ_NO_WAIT;
        }
    }
    /* convert data */
    lua_getfield(L, 1, "to_c");
    lua_pushvalue(L, 2);
    lua_call(L, 1, 1);
    if (lua_type(L, -1) == LUA_TNUMBER)
    {
        intMsg = (GT_U32)lua_tonumber(L, -1);
        msgSize = sizeof(GT_U32);
        msgPtr = &intMsg;
    }
    else if (lua_type(L, -1) == LUA_TUSERDATA)
    {
        msgSize = lua_objlen(L, -1);
        msgPtr = lua_touserdata(L, -1);
    }
    else
    {
        /* wrong type */
        lua_pushliteral(L, "type not supported");
        return 1;
    }

    status = osMsgQSend(msgqId, msgPtr, msgSize, timeOut);
    lua_pop(L, 1); /* converted message */
    lua_pushnumber(L, status);
    return 1;
}

/*******************************************************************************
* luaMsgQRecv
*
* DESCRIPTION:
*       Receive message from queuee
*
* INPUTS:
*       msgqId       - message queue Id table
*       timeOut      - (optional) timeout in miliseconds
*                      default OS_MSGQ_WAIT_FOREVER
*
* RETURNS:
*       status or error string
*       message      - message
*
* COMMENTS:
*       None
*
*******************************************************************************/
int luaMsgQRecv
(
    lua_State *L
)
{
    GT_STATUS status;
    GT_PTR msgPtr = NULL;
    GT_U32 msgSize = 0;
    GT_U32 intMsg = 0;
    GT_U32 timeOut = OS_MSGQ_WAIT_FOREVER;

    LUA_MSGQ_BEGIN(1);

    if (lua_gettop(L) > 1)
    {
        if (lua_isnumber(L, 2))
        {
            timeOut = (GT_U32)lua_tonumber(L, 2);
            if (timeOut == 0)
                timeOut = OS_MSGQ_NO_WAIT;
        }
    }
    /* allocate for message */
    lua_getfield(L, 1, "to_c");
    lua_pushnil(L);
    lua_call(L, 1, 1);
    if (lua_type(L, -1) == LUA_TNUMBER)
    {
        intMsg = (GT_U32)lua_tonumber(L, -1);
        msgSize = sizeof(GT_U32);
        msgPtr = &intMsg;
    }
    else if (lua_type(L, -1) == LUA_TUSERDATA)
    {
        msgSize = lua_objlen(L, -1);
        msgPtr = lua_touserdata(L, -1);
    }
    else
    {
        /* wrong type */
        lua_pushliteral(L, "type not supported");
        return 1;
    }

    status = osMsgQRecv(msgqId, msgPtr, &msgSize, timeOut);

    if (status != GT_OK)
    {
        lua_pushnumber(L, status);
        return 1;
    }
    /* convert to lua */
    if (lua_type(L, -1) == LUA_TNUMBER)
    {
        lua_pop(L, 1);
        lua_pushnumber(L, intMsg);
    }
    lua_getfield(L, 1, "to_lua");
    lua_insert(L, -2);
    lua_call(L, 1, 1);
    /* return status, message */
    lua_pushnumber(L, status);
    lua_insert(L, -2);
    return 2;
}

