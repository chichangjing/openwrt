/*******************************************************************************
* cmdWebServer.c
*
* DESCRIPTION:
*       Web server implementation
*
* DEPENDENCIES:
*
* FILE REVISION NUMBER:
*       $Revision: 2$
*
* COMMENTS:
*       for info see:
*       RFC 1945: Hypertext Transfer Protocol -- HTTP/1.0
*       RFC 2616: Hypertext Transfer Protocol -- HTTP/1.1
*       RFC 1630: Universal Resource Identifiers in WWW
*       RFC 1738: Uniform Resource Locators (URL)
*       RFC 1867: Form-based File Upload in HTML
*       ??? MIME
*
*       Cookies: (see RFC 2109)
*       from server:
*           Set-Cookie: Name=cookie_value; expires=exp_date; path=/; domain=www.com
*           exp_date := Wed, 21-Jan-04 13:22:23 GMT
*       from client:
*           Cookie: name1=value[; name2=value...]
*
*******************************************************************************/

/***** Include files ***************************************************/

#include <cmdShell/common/cmdCommon.h>
#include <cmdShell/os/cmdStream.h>
#include <cmdShell/FS/cmdFS.h>

#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>

#include <cmdShell/lua/cmdLuaCommon.h>

/************* Externs ********************************************************/

/***** Defines  ********************************************************/
#define     HTTP_MAX_LINE_LENGTH     1024

/***** Global macros ***************************************************/

/***** Private Types ***************************************************/
/***** Private Data ****************************************************/

/***** Private Functions ***********************************************/
/* externs */
int HttpParser_ParseQuery(lua_State *L, const char *data, int len);
int HttpParser_ParseHeader(lua_State *L, const char *data, int len);
int HttpParser_getHeader(lua_State *L);
int HttpParser_ParseData(lua_State *L, const char *data, int len);

/*******************************************************************************
* l_http_set_responce
*
* DESCRIPTION:
*       registeread as
*           http.set_responce()
*
* INPUTS:
*       L       - Lua state
*        1st    - responce code
*        2nd    - responce string
*
* OUTPUTS:
*       luaPtr
*
* RETURNS:
*       false if headers already sent
*
* COMMENTS:
*       None
*
*******************************************************************************/
static int l_http_set_responce(
    lua_State *L
)
{
    if (!lua_isstring(L, 1) || !lua_isstring(L, 2))
    {
        /* at least two string parameters required */
        lua_pushboolean(L, 0);
        return 1;
    }
    lua_settop(L, 2);

    lua_getglobal(L, "http");
    lua_getfield(L, -1, "header_sent");
    if (lua_toboolean(L, -1))
    {
        /* headers already sent */
        /* return false */
        lua_pushboolean(L, 0);
        return 1;
    }
    lua_pop(L, 1);

    /* check responce code */
    lua_getglobal(L, "string");
    lua_getfield(L, -1, "match");
    lua_pushvalue(L, 1);
    lua_pushliteral(L, "[1-5]%d%d");
    lua_call(L, 2, 1);
    if (!lua_equal(L, 1, -1))
    {
        lua_pushboolean(L, 1);
        return 1;
    }
    
    lua_setfield(L, 3, "responce_code");
    lua_pushvalue(L, 2);
    lua_setfield(L, 3, "responce");

    lua_pushboolean(L, 1);
    return 1;
}

int lua_equal_ignorecase(lua_State *L, int a, int b);
/*******************************************************************************
* l_http_set_header
*
* DESCRIPTION:
*       registeread as
*           http.set_header()
*
* INPUTS:
*       L       - Lua state
*        1st    - header name
*        2nd    - responceder string
*        3rd    - optional. override flag. default is false
*
* OUTPUTS:
*       luaPtr
*
* RETURNS:
*       false if headers already sent
*
* COMMENTS:
*       None
*
*******************************************************************************/
static int l_http_set_header(
    lua_State *L
)
{
    int headers;
    int override = 0;

    if (!lua_isstring(L, 1) || !lua_isstring(L, 2))
    {
        /* at least two string parameters required */
        lua_pushboolean(L, 0);
        return 1;
    }
    if (lua_gettop(L) > 2)
    {
        override = lua_toboolean(L, 3);
    }
    lua_settop(L, 2);

    lua_getglobal(L, "http");
    lua_getfield(L, -1, "header_sent");
    if (lua_toboolean(L, -1))
    {
        /* headers already sent */
        /* return false */
        lua_pushboolean(L, 0);
        return 1;
    }
    lua_pop(L, 1);

    lua_getfield(L, -1, "responce_headers");
    headers = lua_gettop(L);
    if (override)
    {
        /*TODO search for this header */
        int h;
        
        for (h = 1; h <= lua_objlen(L, headers); h++)
        {
            lua_rawgeti(L, headers, h);
            lua_rawgeti(L, -1, 1);

            if (lua_equal_ignorecase(L, -1, 1))
            {
                lua_pushvalue(L, 2);
                lua_rawseti(L, -3, 2);
                lua_pushboolean(L, 1);
                return 1;
            }

            lua_pop(L, 2);
        }
    }

    /* add new header */
    lua_newtable(L);
    lua_pushvalue(L, 1);
    lua_rawseti(L, -2, 1);
    lua_pushvalue(L, 2);
    lua_rawseti(L, -2, 2);
    lua_rawseti(L, headers, lua_objlen(L, headers)+1);

    lua_pushboolean(L, 1);
    return 1;
}

/*******************************************************************************
* l_http_send_header
*
* DESCRIPTION:
*       registeread as
*           http.send_header()
*
* INPUTS:
*       L       - Lua state
*
* OUTPUTS:
*       luaPtr
*
* RETURNS:
*       false if headers already sent
*
* COMMENTS:
*       None
*
*******************************************************************************/
static int l_http_send_header(
    lua_State *L
)
{
    cmdStreamPTR IOStream;
    int http;
    int h;

    lua_getglobal(L, "http");
    http = lua_gettop(L);
    lua_getfield(L, http, "header_sent");
    if (lua_toboolean(L, -1))
    {
        /* headers already sent */
        /* return false */
        lua_pushboolean(L, 0);
        return 1;
    }

    /* now send header */
    lua_getglobal(L, "IOStream");
    IOStream = (cmdStreamPTR)lua_touserdata(L, -1);
    lua_pop(L, 1);

    /* send responce code */
    lua_pushliteral(L, "HTTP/1.1 ");
    lua_getfield(L, http, "responce_code");
    lua_pushliteral(L, " ");
    lua_getfield(L, http, "responce");
    lua_pushliteral(L, "\n");
    lua_concat(L, 5);
    IOStream->write(IOStream, lua_tostring(L, -1), lua_objlen(L, -1));
    lua_pop(L, 1);

    /* send headers */
    lua_getfield(L, http, "responce_headers");
    for (h = 1; h <= lua_objlen(L, -1); h++)
    {
        int header;
        lua_rawgeti(L, -1, h);
        header = lua_gettop(L);

        lua_rawgeti(L, header, 1);
        lua_pushliteral(L, ": ");
        lua_rawgeti(L, header, 2);
        lua_pushliteral(L, "\n");
        lua_concat(L, 4);
        IOStream->write(IOStream, lua_tostring(L, -1), lua_objlen(L, -1));
        lua_pop(L, 1);

        lua_pop(L, 1);
    }


    IOStream->write(IOStream, "\n", 1);

    /* http.header_sent = true */
    lua_pushboolean(L, 1);
    lua_setfield(L, http, "header_sent");
    /* return true */
    lua_pushboolean(L, 1);
    return 1;
}

/*******************************************************************************
* l_http_data
*
* DESCRIPTION:
*       registeread as
*           http.data()
*
* INPUTS:
*       L       - Lua state
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
static int l_http_data(
    lua_State *L
)
{
    cmdStreamPTR IOStream;
    int n;
    int i;

    /* flush headers */
    lua_pushcfunction(L, l_http_send_header);
    lua_call(L, 0, 0);

    lua_getglobal(L, "IOStream");
    IOStream = (cmdStreamPTR)lua_touserdata(L, -1);
    lua_pop(L, 1);
    if (IOStream == NULL)
        return 0;

    n=lua_gettop(L);
    for (i=1; i<=n; i++)
    {
        if (lua_isstring(L,i))
            IOStream->write(IOStream, lua_tostring(L,i), lua_objlen(L, i));
    }
    return 0;
}

/*******************************************************************************
* l_http_print
*
* DESCRIPTION:
*       registeread as
*           print
*
* INPUTS:
*       L       - Lua state
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
static int l_http_print(
    lua_State *L
)
{
    cmdStreamPTR IOStream;
    int n;
    int i;

    /* flush headers */
    lua_pushcfunction(L, l_http_send_header);
    lua_call(L, 0, 0);

    lua_getglobal(L, "IOStream");
    IOStream = (cmdStreamPTR)lua_touserdata(L, -1);
    lua_pop(L, 1);
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
* cmdWebServerInitLuaHTTPEnv
*
* DESCRIPTION:
*       Initialize Lua environment for http services
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
static void cmdWebServerInitLuaHTTPEnv
(
    lua_State *L,
    cmdStreamPTR IOStream
)
{
    lua_pushlightuserdata(L, IOStream);
    lua_setglobal(L, "IOStream");
    /* override print */
    lua_register(L, "print", l_http_print);
    lua_newtable(L);
    lua_setglobal(L, "http");
    lua_getglobal(L, "http");
    lua_pushboolean(L, 0);
    lua_setfield(L, -2, "header_sent");
    lua_pushnumber(L, 200);
    lua_setfield(L, -2, "responce_code");
    lua_pushliteral(L, "OK");
    lua_setfield(L, -2, "responce");
    lua_newtable(L);
    lua_setfield(L, -2, "responce_headers");
    /* functions */
    lua_pushcfunction(L, l_http_set_responce);
    lua_setfield(L, -2, "set_responce");
    lua_pushcfunction(L, l_http_set_header);
    lua_setfield(L, -2, "set_header");
    lua_pushcfunction(L, l_http_send_header);
    lua_setfield(L, -2, "send_header");
    lua_pushcfunction(L, l_http_data);
    lua_setfield(L, -2, "data");
    lua_pushcfunction(L, HttpParser_getHeader);
    lua_setfield(L, -2, "get_header");
    /* tables */
    lua_newtable(L);
    lua_setfield(L, -2, "headers");
    lua_newtable(L);
    lua_setfield(L, -2, "get_vars");
    lua_newtable(L);
    lua_setfield(L, -2, "post_vars");
    lua_newtable(L);
    lua_setfield(L, -2, "cookies");
    lua_pop(L, 1);
}

typedef struct {
    char            buf[1024];
    int             bytes;
    int             pos;
} IOSTREAM_BUFFERED;
/*******************************************************************************
* cmdWebServerHttpReadLine
*
* DESCRIPTION:
*       HTTP protocol implmentation: read line
*       Line must end with \r\n
*
* INPUTS:
*       IOStream    - I/O stream
*       L           - Lua state
*       buf         - IOSTREAM_BUFFERED
*
* OUTPUTS:
*       buf
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on error
*
* COMMENTS:
*       None
*
*******************************************************************************/
static GT_STATUS cmdWebServerHttpReadLine(
    cmdStreamPTR        IOStream,
    lua_State           *L,
    IOSTREAM_BUFFERED   *buf
)
{
    int     i;

    lua_pushliteral(L, "");
    while (IOStream->connected(IOStream))
    {
        if (buf->pos >= buf->bytes)
        {
            buf->bytes = IOStream->read(IOStream, buf->buf, sizeof(buf->buf));
            if (buf->bytes < 0)
            {
                lua_pop(L, 1);
                return GT_FAIL;
            }
            buf->pos = 0;
        }
        else 
        {
            /* pos < bytes */
            for (i = buf->pos; i < buf->bytes; i++)
                if (buf->buf[i] == '\n')
                    break;
            lua_pushlstring(L, buf->buf + buf->pos, i-buf->pos+1);
            lua_concat(L, 2);
            buf->pos = i + 1;
            if (i < buf->bytes)
            {
                return GT_OK;
            }
        }
    }
    lua_pop(L, 1);
    return GT_FAIL;
}

/*******************************************************************************
* cmdWebServerHttpProto
*
* DESCRIPTION:
*       HTTP protocol implmentation
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
static GT_STATUS cmdWebServerHttpProto
(
    cmdStreamPTR IOStream
)
{
    lua_State *L;
#ifdef  LUACLI_MEMSTAT
    LUACLI_MEM_STAT_STC memStat;
#endif /* LUACLI_MEMSTAT */
    IOSTREAM_BUFFERED buf;
    int bodysize = 0;
    int len;

    /* initialize Lua instance */
#ifdef  LUACLI_MEMSTAT
    cmdOsMemSet(&memStat, 0, sizeof(memStat));
    L = cmdLua_newstate(&memStat);
#else /* !LUACLI_MEMSTAT */
    L = cmdLua_newstate(NULL);
#endif /* LUACLI_MEMSTAT */
    cmdWebServerInitLuaHTTPEnv(L, IOStream);

    IOStream->flags &= ~STREAM_FLAG_I_ECHO;
    IOStream->isConsole = GT_FALSE;

    buf.bytes = 0;
    buf.pos = 0;

    /* read HTTP query string:
     * METHOD URL PROTOCOL
     */
    if (cmdWebServerHttpReadLine(IOStream, L, &buf) != GT_OK)
    {
        /* input faled, connection failed */
        goto stream_error_exit;
    }
    if (HttpParser_ParseQuery(L, lua_tostring(L, -1), lua_objlen(L, -1)) != 0)
    {
        goto error_exit;
    }
    lua_pop(L, 1);

    /* read http request header till empty line */
    lua_pushliteral(L,"");
    while (IOStream->connected(IOStream))
    {
        const char *line;
        if (cmdWebServerHttpReadLine(IOStream, L, &buf) != GT_OK)
        {
            /* input faled, connection failed */
            goto stream_error_exit;
        }
        line = lua_tostring(L, -1);
        if (line[0] == '\n' || (line[0] == '\r' && line[1] == '\n'))
        {
            lua_pop(L, 1);
            break;
        }
        lua_concat(L, 2);
    }
    if (HttpParser_ParseHeader(L, lua_tostring(L, -1), lua_objlen(L, -1)) != 0)
    {
        goto error_exit;
    }
    lua_pop(L, 1);
    /* resolve body size */
    bodysize = 0;
    lua_pushcfunction(L, HttpParser_getHeader);
    lua_pushliteral(L, "Content-Length");
    lua_call(L, 1, 1);
    if (lua_isnumber(L, -1))
    {
        bodysize = lua_tointeger(L, -1);
    }
    lua_pop(L, 1);

    /* read http request body */
    /* put buffered data first */
    len = buf.bytes - buf.pos;
    if (len > bodysize)
        len = bodysize;
    lua_pushlstring(L, buf.buf + buf.pos, len);
    bodysize -= len;
    while (bodysize)
    {
        len = IOStream->read(IOStream, buf.buf,
            bodysize > sizeof(buf.buf)
            ? sizeof(buf.buf)
            : bodysize);
        if (len < 0)
        {
            /* input faled, connection failed */
            goto stream_error_exit;
        }
        if (len)
        {
            lua_pushlstring(L, buf.buf, len);
            lua_concat(L, 2);
        }
        bodysize -= len;
    }
    if (lua_objlen(L, -1) > 0)
    {
        if (HttpParser_ParseData(L, lua_tostring(L, -1), lua_objlen(L, -1)) != 0)
        {
            goto error_exit;
        }
    }
    lua_pop(L, 1);

    /* execute query */
    /* set default content-type, will be done by script */
    lua_pushcfunction(L, l_http_set_header);
    lua_pushliteral(L, "Content-Type");
    lua_pushliteral(L, "text/html");
    lua_call(L, 2, 0);

    /* load cmdWebServer.lua */
    lua_getglobal(L, "require");
    lua_pushliteral(L, "cmdWebServer");
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

    /* flush data */


error_exit:
    lua_pushcfunction(L, l_http_send_header);
    lua_call(L, 0, 0);
/*    cmdOsTimerWkAfter(100);*/

stream_error_exit:
    IOStream->destroy(IOStream);
    lua_close(L);

    return GT_OK;
}


/***** Public Functions ************************************************/

/*******************************************************************************
* cmdWebServerServiceStart
*
* DESCRIPTION:
*       Start Web server
*
* INPUTS:
*       None
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
GT_STATUS cmdWebServerServiceStart(void)
{
    GT_STATUS rc;

    rc = cmdStreamSocketServiceCreate(
                "WEBSERVER",
                NULL, 8080/*port*/,
                cmdWebServerHttpProto,
                GT_TRUE/*multipleInstances*/,
                GT_FALSE/*isConsole*/);
    if (rc == GT_OK)
    {
        rc = cmdStreamSocketServiceStart("WEBSERVER");
    }
    if (rc != GT_OK)
    {
        cmdOsPrintf("commander: failed to create Web server\n");
    }
    return rc;
}
