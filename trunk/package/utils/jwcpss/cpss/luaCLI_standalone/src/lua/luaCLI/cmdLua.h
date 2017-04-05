#error temporary disabled code

/****************************************************
* cmdLua.h
*
* DESCRIPTION:
*       
*
* DEPENDENCIES:
*
* COMMENTS:
*
* FILE REVISION NUMBER:
*       $Revision: 1 $
**************************************************************************/
#ifndef __cmdLua_h__
#define __cmdLua_h__

/***** Include files ***************************************************/
#include "lua.h"

/***** Defines  ********************************************************/
#define CMD_LUA_NET_FLUSH_TIMEOUT 1
#define CMD_LUA_NET_WAITSTRING_TIMEOUT 60

#define CMD_LUA_TYPE_UNKNOWN    0
#define CMD_LUA_TYPE_BYTEARRAY  1
#define CMD_LUA_TYPE_U16ARRAY   2
#define CMD_LUA_TYPE_U32ARRAY   3
#define CMD_LUA_TYPE_U64        4


/*******************************************************************************
* luaopen_cmdLua
*
* DESCRIPTION:
*       Initialize library, register functions
*
* INPUTS:
*       L       - lua_State
*
* OUTPUTS:
*       None
*
* RETURNS:
*       0
*
* COMMENTS:
*
*******************************************************************************/
int luaopen_cmdLua(lua_State *L);

/*******************************************************************************
* cmdLuaSetCustomType
*
* DESCRIPTION:
*       Set custom type for userdata value
*
* INPUTS:
*       L       - lua_State
*       index   - index of userdata on stack
*       type    - type to set
*
* OUTPUTS:
*       None
*
* RETURNS:
*       0
*
* COMMENTS:
*
*******************************************************************************/
int cmdLuaSetCustomType(lua_State *L, int index, int type);

/*******************************************************************************
* cmdLuaGetCustomType
*
* DESCRIPTION:
*       Get custom type for userdata value
*
* INPUTS:
*       L       - lua_State
*       index   - index of userdata on stack
*
* OUTPUTS:
*       None
*
* RETURNS:
*       type
*
* COMMENTS:
*
*******************************************************************************/
int cmdLuaGetCustomType(lua_State *L, int index);




/*************************************************************************
* cmdLuaGaltis
*************************************************************************/
#ifdef  CMD_LUA_GALTIS

/*******************************************************************************
* cmdLuaRunGaltisWrapper
*
* DESCRIPTION:
*       Wrapper to execute existing Galtis Wrappers
*
* INPUTS:
*       L       - Lua state
*         1st   - Galtis wrapper name
*
* OUTPUTS:
*       None
*
* RETURNS:
*       wrapper output
*
* COMMENTS:
*
*******************************************************************************/
int cmdLuaRunGaltisWrapper(lua_State *L);
#endif /* CMD_LUA_GALTIS */



/*************************************************************************
* cmdLua_net
*************************************************************************/
#ifdef CMD_LUA_CLI

/*******************************************************************************
* cmdLuaNetConnect
*
* DESCRIPTION:
*       Connect to remote host
*
* INPUTS:
*       L       - Lua state
*         1st   - host name
*         2nd   - port
*
* OUTPUTS:
*       None
*
* RETURNS:
*       string or nil if error
*       connection descriptor
*
* COMMENTS:
*
*******************************************************************************/
int cmdLuaNetConnect(lua_State *L);

/*******************************************************************************
* cmdLuaNetDisconnect
*
* DESCRIPTION:
*       disconnect
*
* INPUTS:
*       L       - Lua state
*         1st   - connection descriptor
*
* OUTPUTS:
*       None
*
* RETURNS:
*       None
*
* COMMENTS:
*
*******************************************************************************/
int cmdLuaNetDisconnect(lua_State *L);

/*******************************************************************************
* cmdLuaNetFlushInput
*
* DESCRIPTION:
*       flush input, make sure noting left in buffer (sync)
*
* INPUTS:
*       L       - Lua state
*         1st   - connection descriptor
*
* OUTPUTS:
*       None
*
* RETURNS:
*       None
*
* COMMENTS:
*
*******************************************************************************/
int cmdLuaNetFlushInput(lua_State *L);

/*******************************************************************************
* cmdLuaNetSend
*
* DESCRIPTION:
*       Send string to connection
*
* INPUTS:
*       L       - Lua state
*         1st   - connection descriptor
*         2nd   - string to send
*
* OUTPUTS:
*       None
*
* RETURNS:
*       None
*
* COMMENTS:
*
*******************************************************************************/
int cmdLuaNetSend(lua_State *L);

/*******************************************************************************
* cmdLuaNetWaitString
*
* DESCRIPTION:
*       Wait for a string from connection
*
* INPUTS:
*       L       - Lua state
*         1st   - connection descriptor
*         2nd   - timeout. Default is CMD_LUA_NET_WAITSTRING_TIMEOUT
*
* OUTPUTS:
*       None
*
* RETURNS:
*       string if successful
*       nil if timeout
*       any other on error
*
* COMMENTS:
*
*******************************************************************************/
int cmdLuaNetWaitString(lua_State *L);
#endif /* CMD_LUA_CLI */

#endif
