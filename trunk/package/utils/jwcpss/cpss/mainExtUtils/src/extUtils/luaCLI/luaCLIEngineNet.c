/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* luaCLIEngineNet.c
*
* DESCRIPTION:
*       network API Lua:
*       - tcp.*
*
* DEPENDENCIES:
*
* COMMENTS:
*
* FILE REVISION NUMBER:
*       $Revision: 2 $
*******************************************************************************/

/***** Include files ***************************************************/

#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>
#include <cpss/extServices/private/prvCpssBindFunc.h>


/***** Defines  ********************************************************/
#define CMD_LUA_NET_FLUSH_TIMEOUT 10

/***** Private Types ***************************************************/

/***** Private Data ****************************************************/

/***** Public Functions ************************************************/

/*******************************************************************************
* l_cpssNetTcp_Connect
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
*       Lua: connection descriptor or nil + error_message
*
* COMMENTS:
*   tcp.connect(
*       addr,
*       port
*   )
*                       Connect to host using TCP stream
*                       Returns connection descriptor or nil + error_message
*
*******************************************************************************/
static int l_cpssNetTcp_Connect(lua_State *L)
{
    GT_U8*          ipAddr;
    CPSS_SOCKET_PORT  ipPort;
    GT_VOID *       sockAddr;
    GT_SIZE_T       sockAddrLen;
    GT_STATUS       ret;
    CPSS_SOCKET_FD    sock;

    if (lua_gettop(L) < 2 || !lua_isstring(L, 1))
    {
        lua_pushnil(L);
        lua_pushliteral(L, "Bad parameters. Must be ipaddr(string),port_num(number)");
        return 2;
    }
    ipAddr = (GT_U8*)lua_tostring(L, 1);
    ipPort = (CPSS_SOCKET_PORT)lua_tointeger(L, 2);
    if (ipPort == 0)
    {
        lua_pushnil(L);
        lua_pushfstring(L, "Bad port: %s must be a number", lua_tostring(L, 2));
        return 2;
    }

    /* resolve address */
    ret = cpssOsSocketCreateAddr((GT_CHAR*)ipAddr, ipPort, &sockAddr, &sockAddrLen);
    if (ret != GT_OK)
    {
        lua_pushnil(L);
        lua_pushfstring(L, "failed to parse ip address: %s", lua_tostring(L, 1));
        return 2;
    }
    /* create socket */
    sock = cpssOsSocketTcpCreate(-1/* default buffers */);
    if (sock < 0)
    {
        lua_pushnil(L);
        lua_pushliteral(L, "failed to create TCP socket");
        cpssOsSocketDestroyAddr(sockAddr);
        return 2;
    }
    /* connect */
    ret = cpssOsSocketConnect(sock, sockAddr, sockAddrLen);
    if (ret != GT_OK)
    {
        lua_pushnil(L);
        lua_pushfstring(L, "failed to connect to %s:%s",
                lua_tostring(L, 1), lua_tostring(L, 2));
        cpssOsSocketTcpDestroy(sock);
        cpssOsSocketDestroyAddr(sockAddr);
        return 2;

    }

    /* sockAddr no more required */ 
    cpssOsSocketDestroyAddr(sockAddr);

    /* return sock */
    lua_pushinteger(L, sock);
    return 1;
}

/*******************************************************************************
* l_cpssNetTcp_Close
*
* DESCRIPTION:
*       close connection
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
*   tcp.close(descr)    Close connection
*
*******************************************************************************/
static int l_cpssNetTcp_Close(lua_State *L)
{
    CPSS_SOCKET_FD    sock;

    if (!lua_isnumber(L, 1))
        return 0;
    sock = lua_tointeger(L, 1);

    cpssOsSocketTcpDestroy(sock);
    return 0;
}

/*******************************************************************************
* l_cpssNetTcp_FlushInput
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
*       Lua: true/false
*
* COMMENTS:
*   tcp.flushinput(
*       descr
*   )
*                       Flush(drop) all pending incoming data on connection
*                       Returns result: true/false
*
*******************************************************************************/
static int l_cpssNetTcp_FlushInput(lua_State *L)
{
    CPSS_SOCKET_FD    sock;
    GT_VOID*        rfds;

    if (!lua_isnumber(L, 1))
    {
        /* bad parameter */
        lua_pushboolean(L, 0);
        return 1;
    }
    sock = lua_tointeger(L, 1);

    rfds = cpssOsSelectCreateSet();
    if (!rfds)
    {
        lua_pushboolean(L, 0);
        return 1;
    }

    /* while data avaible on for immediate input read it */
    while (1)
    {
        int     k;
        GT_8    buf[64];

        cpssOsSelectZeroSet(rfds);
        cpssOsSelectAddFdToSet(rfds, sock);

        k = cpssOsSelect(sock + 1, rfds, NULL, NULL, CMD_LUA_NET_FLUSH_TIMEOUT);
        if (k <= 0)
            break;
        cpssOsSocketRecv(sock, buf, sizeof(buf), GT_TRUE);
    }
    cpssOsSelectEraseSet(rfds);

    lua_pushboolean(L, 1);
    return 1;
}

/*******************************************************************************
* l_cpssNetTcp_Send
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
*       Lua: true/false
*
* COMMENTS:
*   tcp.send(
*       descr,
*       string_to_send
*   )
*                       Send string to connection
*                       Returns result: true/false
*
*******************************************************************************/
static int l_cpssNetTcp_Send(lua_State *L)
{
    CPSS_SOCKET_FD    sock;
    const char      *s;
    size_t          len;
    GT_SSIZE_T      bytes_sent;


    if (lua_gettop(L) < 2 || !lua_isnumber(L, 1) || !lua_isstring(L, 2))
    {
        /* bad parameters */
        lua_pushboolean(L, 0);
        return 1;
    }
    if (!lua_isnumber(L, 1))
        return 0;
    sock = lua_tointeger(L, 1);

    s = lua_tolstring(L, 2, &len);

    bytes_sent = (GT_SSIZE_T)len;
    if (s)
        bytes_sent = cpssOsSocketSend(sock, (GT_8*)s, (GT_SIZE_T)len);
    lua_pushboolean(L, bytes_sent == (GT_SSIZE_T)len);
    return 1;
}

/*******************************************************************************
* l_cpssNetTcp_Wait
*
* DESCRIPTION:
*       Wait for data available on connection
*
* INPUTS:
*       L       - Lua state
*         1st   - connection descriptor
*         2nd   - timeout in milliseconds. Default is wait forever
*
* OUTPUTS:
*       None
*
* RETURNS:
*         1 - data available
*         0 - no data available (timeout)
*        -1 - error
*
* COMMENTS:
*   tcp.wait(
*       descr
*       [, timeout]
*   )
*                       Wait for data available on connection
*                       The timeout == WAIT_FOREVER if not specified
*                       Returns:
*                        1 - data available
*                        0 - no data available (timeout)
*                       -1 - error
*
*******************************************************************************/
static int l_cpssNetTcp_Wait(lua_State *L)
{
    CPSS_SOCKET_FD    sock;
    GT_U32          timeout = 0xffffffff;
    int             k;
    GT_VOID*        rfds;

    if (!lua_isnumber(L, 1))
    {
        /* bad descritor */
        lua_pushinteger(L, -1);
        return 1;
    }
    sock = lua_tointeger(L, 1);

    if (lua_isnumber(L, 2))
        timeout = (GT_U32)lua_tointeger(L, 2);

    rfds = cpssOsSelectCreateSet();
    if (!rfds)
    {
        /* can't create fd_set */
        lua_pushinteger(L, -1);
        return 1;
    }
    
    cpssOsSelectZeroSet(rfds);
    cpssOsSelectAddFdToSet(rfds, sock);

    k = cpssOsSelect(sock + 1, rfds, NULL, NULL, timeout);

    cpssOsSelectEraseSet(rfds);

    /* (k == 0): timeout */
    /* (k < 0): select() returned error */
    /* (k == 1): data available */
    lua_pushinteger(L, k);
    return 1;
}

/*******************************************************************************
* l_cpssNetTcp_Recv
*
* DESCRIPTION:
*       Wait for data available on connection
*
* INPUTS:
*       L       - Lua state
*         1st   - connection descriptor
*         2nd   - max bytes to read. Default is 1024
*
* OUTPUTS:
*       None
*
* RETURNS:
*       Lua: string
*       Lua: nil + error_message if error
*
* COMMENTS:
*   tcp.recv(
*       descr
*       [, max_bytes]
*   )
*                       Receive data
*                       max_bytes = 1024 if not specified
*                       returns string or nil + error_message if error
*
*******************************************************************************/
static int l_cpssNetTcp_Recv(lua_State *L)
{
    CPSS_SOCKET_FD    sock;
    int             max_bytes = 1024;
    char            *buf;
    int             k;

    if (!lua_isnumber(L, 1))
    {
        lua_pushnil(L);
        lua_pushliteral(L, "bad descritor");
        return 2;
    }
    sock = lua_tointeger(L, 1);

    if (lua_isnumber(L, 2))
        max_bytes = (int)lua_tointeger(L, 2);

    buf = (char*)cpssOsMalloc(max_bytes);
    if (!buf)
    {
        lua_pushnil(L);
        lua_pushliteral(L, "failed to allocate buffer");
        return 2;
    }
    
    k = cpssOsSocketRecv(sock, buf, max_bytes, GT_TRUE);
    if (k < 0)
    {
        cpssOsFree(buf);
        lua_pushnil(L);
        lua_pushliteral(L, "recv() returned error");
        return 2;
    }
    lua_pushlstring(L, buf, k);
    cpssOsFree(buf);
    return 1;
}

/*******************************************************************************
* l_cpssNetTcp_WaitString
*
* DESCRIPTION:
*       Read line from connection. The EOL is \r?\n
*
* INPUTS:
*       L       - Lua state
*         1st   - connection descriptor
*         2nd   - timeout in milliseconds. Default is wait forever
*
* OUTPUTS:
*       None
*
* RETURNS:
*       Lua: string if successful
*       Lua: 0 if timeout
*       Lua: nil + error_message  on error
*
* COMMENTS:
*   tcp.waitline(
*       descr
*       [, timeout]
*   )
*                       Read line from connection. The EOL is \r?\n
*                       Return string or 0 if timeout, nil + error_message if error
*
*******************************************************************************/
static int l_cpssNetTcp_WaitString(lua_State *L)
{
    CPSS_SOCKET_FD    sock;
    GT_U32          timeout = 0xffffffff;
    char            buf[64];
    int             p;
    GT_VOID*        rfds;

    if (!lua_isnumber(L, 1))
    {
        lua_pushnil(L);
        lua_pushliteral(L, "bad descritor");
        return 2;
    }
    sock = lua_tointeger(L, 1);

    if (lua_isnumber(L, 2))
        timeout = (GT_U32)lua_tointeger(L, 2);

    rfds = cpssOsSelectCreateSet();
    if (!rfds)
    {
        lua_pushnil(L);
        lua_pushliteral(L, "can't create fd_set");
        return 2;
    }
    
    /* begin waitstring */
    lua_pushstring(L, "");
    p = 0;
    while (1)
    {
        int k;

        cpssOsSelectZeroSet(rfds);
        cpssOsSelectAddFdToSet(rfds, sock);

        k = cpssOsSelect(sock + 1, rfds, NULL, NULL, timeout);
        /* this cannot be handled due to lag in API:
        if (k < 0 && errno == EINTR)
            continue;
        */
        if (k == 0)
        {
            lua_pop(L, 1);
            lua_pushinteger(L, 0);
            break;
        }
        if (k < 0)
        {
            cpssOsSelectEraseSet(rfds);
            lua_pop(L, 1);
            lua_pushnil(L);
            lua_pushliteral(L, "select() returned error");
            return 2;
        }
        k = cpssOsSocketRecv(sock, buf + p, 1, GT_TRUE);
        if (k == 0)
        {
            /* eof */
            break;
        }
        if (k < 0)
        {
            cpssOsSelectEraseSet(rfds);
            lua_pop(L, 1);
            lua_pushnil(L);
            lua_pushliteral(L, "select flags input ready but recv returned error");
            return 2;
        }
        if (buf[p] == '\n')
        {
            if (p) {
                lua_pushlstring(L, buf, p);
                lua_concat(L, 2);
                break;
            }
        }
        p++;
        if (p == sizeof(buf))
        {
            lua_pushlstring(L, buf, p);
            lua_concat(L, 2);
            p = 0;
        }
    }
    cpssOsSelectEraseSet(rfds);
    return 1;
}


static const luaL_Reg luatcplib[] = {
  {"connect",   l_cpssNetTcp_Connect },
  {"close",     l_cpssNetTcp_Close },
  {"flushinput",l_cpssNetTcp_FlushInput },
  {"send",      l_cpssNetTcp_Send },
  {"wait",      l_cpssNetTcp_Wait },
  {"recv",      l_cpssNetTcp_Recv },
  {"waitline",  l_cpssNetTcp_WaitString },
  {NULL, NULL}
};

/*******************************************************************************
* luaCLIEngineNetRegisterAPI
*
* DESCRIPTION:
*       register Network APIs in Lua instance
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
*
*******************************************************************************/
int luaCLIEngineNetRegisterAPI(lua_State *L)
{
    luaL_register(L, "tcp", luatcplib);
    lua_pop(L, 1); /* pop the table from stack */
    return 0;
}

/***** Lua API Documentation **************************************************
*
*   tcp.connect(
*       addr,
*       port
*   )
*                       Connect to host using TCP stream
*                       Returns connection descriptor or nil + error_message
*
*   tcp.close(descr)    Close connection
*
*   tcp.flushinput(
*       descr
*   )
*                       Flush(drop) all pending incoming data on connection
*                       Returns result: true/false
*
*   tcp.send(
*       descr,
*       string_to_send
*   )
*                       Send string to connection
*                       Returns result: true/false
*
*   tcp.wait(
*       descr
*       [, timeout]
*   )
*                       Wait for data available on connection
*                       The timeout == WAIT_FOREVER if not specified
*                       Returns:
*                        1 - data available
*                        0 - no data available (timeout)
*                       -1 - error
*
*   tcp.recv(
*       descr
*       [, max_bytes]
*   )
*                       Receive data
*                       max_bytes = 1024 if not specified
*                       returns string or nil + error_message if error
*
*   tcp.waitline(
*       descr
*       [, timeout]
*   )
*                       Read line from connection. The EOL is \r?\n
*                       The timeout == WAIT_FOREVER if not specified
*                       Return string or 0 if timeout, nil + error_message if error
*                       
*
******************************************************************************/
