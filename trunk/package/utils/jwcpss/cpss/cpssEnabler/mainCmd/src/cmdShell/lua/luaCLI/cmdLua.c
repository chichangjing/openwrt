#error temporary disabled code

/****************************************************
* cmdLua.c
*
* DESCRIPTION:
*       mainCmd communication module for Lua
*
* DEPENDENCIES:
*
* COMMENTS:
*
* FILE REVISION NUMBER:
*       $Revision: 2 $
**************************************************************************/

/***** Include files ***************************************************/
/* required for strncmp */
#include <string.h>
#include <cmdShell/common/cmdCommon.h>

#include "lua.h"
#include "lualib.h"

#include "cmdLua.h"


/***** Defines  ********************************************************/

#define   CMD_LUA_STRNCMP(a,b,l)  strncmp(a,b,l)

#define CPSS_RESULT         "cmdLuaResult:"
#define CPSS_RESULT_L       13
#define CPSS_RESULT_TBL     "cmdLuaResultTbl:"
#define CPSS_RESULT_TBL_L   16
#define CPSS_END_RESULT     "cmdLuaEndResult"
#define CPSS_END_RESULT_L   15

#define CPSS_LUA_OUTSTRING(str) \
    lua_getglobal(L, "print"); \
    lua_pushstring(L, str); \
    lua_call(L, 1, 0);

#define CPSS_LUA_OUTVAL(index) \
    lua_getglobal(L, "print"); \
    lua_pushvalue(L, index); \
    lua_call(L, 1, 0);


/***** Private Functions ***********************************************/

#ifdef  CMD_LUA_SRV
/*******************************************************************************
* cmdLuaExecWrapper
*
* DESCRIPTION:
*       cmdLua wrapper: calls function taken as first parameter
*       and then serialize all return values
*
* INPUTS:
*       function    - string with function name
*       ...         - parameters
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
static int cmdLuaExecWrapper(lua_State *L)
{
    int n,i;

    n = lua_gettop(L);
    if (n < 1)
    {
        /* cmdLuaExecWrapper(): at least one parameter required */
        goto end_results;
    }
    if (!lua_isstring(L,1))
    {
        /* cmdLuaExecWrapper(): first parameter must be string */
        goto end_results;
    }
    lua_getglobal(L, lua_tostring(L,1));
    if (!lua_isfunction(L, -1))
    {
        /* bad function */
        goto end_results;
    }
    lua_replace(L, 1);
    i = lua_pcall(L, n-1, LUA_MULTRET, 0);
    if (i != 0)
    {
        /* error while executing function */
        lua_getglobal(L, "print");
        lua_insert(L,-2);
        lua_call(L, 1, 0);
        goto end_results;
    }
    n = lua_gettop(L);
    /* export return values */
    for (i = 1; i <= n; i++)
    {
        lua_getglobal(L, "cmdLuaPushResult");
        lua_pushvalue(L,i);
        lua_call(L, 1, 0);
    }
end_results:
    lua_getglobal(L, "cmdLuaEndResults");
    lua_call(L, 0, 0);
    return 0;
}

/*******************************************************************************
* cmdLuaPushResult
*
* DESCRIPTION:
*       Push parameter variable as result: serialize it and then output
*
* INPUTS:
*       variable    - variable 
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
static int cmdLuaPushResult(lua_State *L)
{
    if (lua_gettop(L) < 1)
    {
        /* cmdLuaPushResult(): parameter required */
        return 0;
    }
    lua_pushliteral(L, "\n");
    lua_pushliteral(L, CPSS_RESULT);
    lua_getglobal(L, "cmdLuaSerializeValue");
    lua_pushvalue(L,1);
    lua_call(L, 1, 1);
    lua_concat(L, 3);
    CPSS_LUA_OUTVAL(-2);
    return 0;
}

/*******************************************************************************
* cmdLuaPushResultTblItem
*
* DESCRIPTION:
*       Push parameter variable as table item.
*       This function required for big arrays - they will be represented in
*       many lines.
*
* INPUTS:
*       item        - table item
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
static int cmdLuaPushResultTblItem(lua_State *L)
{
    if (lua_gettop(L) < 1)
    {
        /* cmdLuaPushResultTblItem(): parameter required */
        return 0;
    }
    lua_pushliteral(L, "\n");
    lua_pushliteral(L, CPSS_RESULT_TBL);
    lua_concat(L, 2);
    lua_getglobal(L, "cmdLuaSerializeValue");
    lua_pushvalue(L,1);
    lua_call(L, 1, 1);
    lua_concat(L, 2);
    CPSS_LUA_OUTVAL(-2);
    return 0;
}

/*******************************************************************************
* cmdLuaEndResults
*
* DESCRIPTION:
*       Signal caller function on client(host) side that function finished and
*       there will be no more results
*
* INPUTS:
*       None
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
static int cmdLuaEndResults(lua_State *L)
{
    lua_pushliteral(L, "\n");
    lua_pushliteral(L, CPSS_END_RESULT);
    lua_concat(L, 2);
    CPSS_LUA_OUTVAL(-2);
    return 0;
}

/*******************************************************************************
* cmdLuaSerializeValue
*
* DESCRIPTION:
*       Serialize value
*
* INPUTS:
*       value   - a data to serialize
*
* OUTPUTS:
*       None
*
* RETURNS:
*       serialized data (string)
*
* COMMENTS:
*
*******************************************************************************/
int cmdLuaSerializeValue(lua_State *L)
{
    char buf[16]; /* enough for 32 bit decimal */
    lua_pushliteral(L, "");
#define CONCAT_STR(str) { \
        lua_pushstring(L, str); \
        lua_concat(L, 2); \
}
    switch (lua_type(L,1))
    {
        case LUA_TNONE: /* for a non-valid index */
            break;
        case LUA_TNIL:
            CONCAT_STR("n:");
            break;
        case LUA_TBOOLEAN:
            CONCAT_STR(lua_toboolean(L,1) ? "b:t" : "b:f")
            break;
        case LUA_TNUMBER:
            sprintf(buf,"i:%u",(unsigned)lua_tointeger(L,1));
            CONCAT_STR(buf);
            break;
        case LUA_TSTRING:
            CONCAT_STR("s:\"");
            {
                size_t len;
                int i, b;
                const char *s;
                s = lua_tolstring(L,1,&len);
                for (i = b = 0; i < len; i++)
                {
                    unsigned char ch = (unsigned char)s[i];
                    if (ch >= 0x20 && ch < 0x7f && ch != '"' && ch != '%')
                        continue;
                    if (i > b)
                    {
                         lua_pushlstring(L, s+b, i-b);
                         lua_concat(L, 2);
                    }
                    sprintf(buf,"%%%02x",ch);
                    CONCAT_STR(buf);
                    b = i + 1;
                }
                if (i > b)
                {
                     lua_pushlstring(L, s+b, i-b);
                     lua_concat(L, 2);
                }
            }
            CONCAT_STR("\"");
            break;
        case LUA_TTABLE:
            /* table is in the stack at index '1' */
            {
                int f = 0;
                CONCAT_STR("t:{");
                lua_pushnil(L);  /* first key */
                while (lua_next(L, 1) != 0)
                {
                    /* uses 'key' (at index -2) and 'value' (at index -1) */
                    lua_pushvalue(L, 2);
                    if (f)
                        CONCAT_STR(",");
                    f = 1;
                    CONCAT_STR("[");
                    lua_getglobal(L,"cmdLuaSerializeValue");
                    lua_pushvalue(L,-4);
                    lua_call(L,1,1);
                    lua_concat(L, 2);
                    CONCAT_STR("]=");
                    lua_getglobal(L,"cmdLuaSerializeValue");
                    lua_pushvalue(L,-3);
                    lua_call(L,1,1);
                    lua_concat(L, 2);
                    lua_replace(L, 2);
                    /* removes 'value'; keeps 'key' for next iteration */
                    lua_pop(L, 1);
                }
                CONCAT_STR("}");
            }
            break;
        case LUA_TUSERDATA:
            CONCAT_STR("B:?");
            break;
        case LUA_TLIGHTUSERDATA:
            CONCAT_STR("P:?");
            break;
        case LUA_TFUNCTION:
            CONCAT_STR("F:?");
            break;
        case LUA_TTHREAD:
            CONCAT_STR("T:?");
            break;
    }
    return 1;
}
#endif /* CMD_LUA_SRV */

#ifdef  CMD_LUA_CLI
/*******************************************************************************
* cmdLuaParseValue
*
* DESCRIPTION:
*       Parse serialized value
*
* INPUTS:
*       str     - serialized data
*
* OUTPUTS:
*       None
*
* RETURNS:
*       1st. a number of characters parsed in string
*       2nd. data
*
* COMMENTS:
*
*******************************************************************************/
static int cmdLuaParseValue(lua_State *L)
{
    const char *s;
    size_t len;

    if (lua_type(L,1) != LUA_TSTRING)
        return 0;
    s = lua_tolstring(L,1,&len);
    if (len < 2)
    {
ret_err:
        lua_pushnumber(L, len);
        lua_pushnil(L);
        return 2;
    }


    if (s[1] != ':')
        goto ret_err;
    if (s[0] == 'n')
    {
        /* nil */
        lua_pushnumber(L, 1);
        lua_pushnil(L);
        return 2;
    }
    if (s[0] == 'b')
    {
        /* bool */
        lua_pushnumber(L, 3);
        lua_pushboolean(L,(s[2] == 't')?1:0);
        return 2;
    }
    if (s[0] == 'i')
    {
        /* number */
        unsigned val = 0;
        int p;
        for (p = 2; p < len && s[p] >= '0' && s[p] <='9'; p++)
            val = val * 10 + (s[p] - '0');
        lua_pushnumber(L, p);
        lua_pushnumber(L, val);
        return 2;
    }
    if (s[0] == 's')
    {
        /* string */
        int p,ps,pe;
        if (s[2] != '"')
            goto ret_err;
        pe = 3; 
        while (pe < len && s[pe] != '"')
            pe++;
        lua_pushnumber(L, pe+1);
        p = 3;
        lua_pushliteral(L,"");
        for (ps = p; p < pe; p++)
        {
            if (s[p] == '%')
            {
                int x = 0;
                int k;
                char ch;
                lua_pushlstring(L,s+ps,p-ps);
                lua_concat(L,2);
                for (k = 0; k < 2 && p+1 < pe; k++)
                {
                    p++;
                    if (s[p] >= '0' && s[p] <= '9')
                        x = x * 16 + (s[p] - '0');
                    else
                        if (s[p] >= 'a' && s[p] <= 'f')
                            x = x * 16 + (s[p] - 'a' + 10);
                }
                ch = (char)x;
                lua_pushlstring(L,&ch,1);
                lua_concat(L,2);
                ps = p+1;
            }

        }
        if (p > ps)
        {
            lua_pushlstring(L,s+ps,p-ps);
            lua_concat(L,2);
        }
        return 2;
    }
    if (s[0] == 't')
    {
        /* table */        
        int p, n;
        if (s[2] != '{')
            goto ret_err;
        lua_pushnumber(L,3);
        lua_newtable(L);
        p = 3;
        while(p < len && s[p] != '}')
        {
            while (p < len && (s[p] == ',' || s[p] == ' '))
                p++;
            if (p >= len)
                break;

            if (s[p] != '[')
            {
                p = len;
                break;
            }
            p++;
            /* read key */
            lua_getglobal(L,"cmdLuaParseValue");
            lua_pushlstring(L, s+p, len - p);
            lua_call(L, 1, 2);
            n = lua_tointeger(L, -2);
            lua_remove(L,-2);
            p += n;
            /* key in the stack */
            while (p < len && s[p] == ' ')
                p++;
            if (p >= len)
            {
                lua_pop(L,1);
                break;
            }
            if (s[p] != ']' || s[p+1] != '=')
            {
                lua_pop(L,1);
                p = len;
                break;
            }
            p += 2;
            /* read value */
            lua_getglobal(L,"cmdLuaParseValue");
            lua_pushlstring(L, s+p, len - p);
            lua_call(L, 1, 2);
            n = lua_tointeger(L, -2);
            lua_remove(L,-2);
            p += n;
            /* value in the stack */
            lua_settable(L,-3);
        }
        if (p < len && s[p] == '}')
            p++;
        lua_pushnumber(L,p);
        lua_replace(L,-3);
        return 2;
    }
#if 0
    if (s[0] == 'B')
    {
        /* user data */
    }
#endif
    return 0; 
}

/*******************************************************************************
* cmdLuaNetTimeOut
*
* DESCRIPTION:
*       Returns configured timeout cmdLua.cmdTimeOut
*       or CMD_LUA_NET_WAITSTRING_TIMEOUT as fallback
*
* INPUTS:
*       conn    - connection
*
* OUTPUTS:
*       None
*
* RETURNS:
*       configured timeout
*
* COMMENTS:
*       cmdLua.cmdTimeOut controls timeout
*
*******************************************************************************/
static int cmdLuaNetTimeOut(lua_State *L)
{
    int timeout = CMD_LUA_NET_WAITSTRING_TIMEOUT;

    lua_getfield(L, LUA_GLOBALSINDEX, "cmdLua"); /* table to be indexed */
    if (!lua_istable(L, -1))
    {
        lua_pop(L, 1);
        return timeout;
    }
    lua_getfield(L, -1, "cmdTimeOut"); /* push result of cmdLua.cmdTimeOut */
    if (lua_isnumber(L, -1))
        timeout = lua_tointeger(L, -1);
    lua_pop(L, 2);
    return timeout;
}

/*******************************************************************************
* cmdLuaNetWaitResult
*
* DESCRIPTION:
*       Wait result from remote called function
*
* INPUTS:
*       conn    - connection
*
* OUTPUTS:
*       None
*
* RETURNS:
*       a variable number of result values from called funciton
*
* COMMENTS:
*       cmdLua.cmdTimeOut controls timeout
*
*******************************************************************************/
static int cmdLuaNetWaitResult(lua_State *L)
{
    const char *s;
    int nret = 0;
    int inarray = 0;
    int timeout = cmdLuaNetTimeOut(L);

    while (1)
    {
        lua_getglobal(L, "cmdLuaNetWaitString");
        lua_pushvalue(L, 1);
        lua_pushnumber(L, timeout);
        lua_call(L, 2, 1);
        if (!lua_isstring(L, -1))
        {
            /* timeout or read error, flush input */
            lua_pop(L, 1);
            lua_getglobal(L, "cmdLuaNetFlushInput");
            lua_pushvalue(L, 1);
            lua_call(L, 1, 0);
            return nret;
        }
        s = lua_tostring(L, -1);
#if 0
fprintf(stderr,"got string: %s\n",s);
#endif
        if (!CMD_LUA_STRNCMP(s, CPSS_END_RESULT, CPSS_END_RESULT_L))
        {
            /* end results */
            lua_pop(L, 1);
            return nret;
        }
        if (!CMD_LUA_STRNCMP(s, CPSS_RESULT, CPSS_RESULT_L))
        {
            inarray = 0;
            lua_getglobal(L, "cmdLuaParseValue");
            lua_pushstring(L, s + CPSS_RESULT_L);
            lua_call(L, 1, 2);
            lua_replace(L, -3);
            lua_pop(L, 1);
            nret++;
            continue;
        }
        if (!CMD_LUA_STRNCMP(s, CPSS_RESULT_TBL, CPSS_RESULT_TBL_L))
        {
            if (!inarray)
            {
                lua_newtable(L);
                lua_pushvalue(L, -2);
                lua_remove(L, -3);
                s = lua_tostring(L, -1);
                inarray = 1;
                nret++;
            }
            lua_getglobal(L, "cmdLuaParseValue");
            lua_pushstring(L, s + CPSS_RESULT_TBL_L);
            lua_call(L, 1, 2);
            lua_pushnumber(L, inarray);
            lua_replace(L, -3);
            lua_settable(L, -4);
            inarray++;
            lua_pop(L, 1);
            continue;
        }
        lua_pop(L, 1);
    }
    return 0;
}

#if defined(CMD_LUA_FS)
/*******************************************************************************
* cmdLuaNetUpload
*
* DESCRIPTION:
*       Upload file to remote host
*
* INPUTS:
*       conn    - connection
*       name    - file name
*
* OUTPUTS:
*       None
*
* RETURNS:
*       "OK"
*       error string
*
* COMMENTS:
*       cmdLua.cmdTimeOut controls timeout
*
*******************************************************************************/
static int cmdLuaNetUpload(lua_State *L)
{
    void    *f = NULL;
    int     filesize;
    int timeout = cmdLuaNetTimeOut(L);

    /* open file and get file size */
    f = cmdLuaFSopen(lua_tostring(L, 2));
    if (f != NULL)
    {
        void    *ftmp = f;
        size_t  size;
        filesize = 0;
        while (cmdLuaFSReader(L, &ftmp, &size) != NULL)
            filesize += size;
    } else {
        lua_pushfstring(L, "cmdLuaFS: file not found: %s", lua_tostring(L, 2));

        return 1;
    }

    /* send lua_pushfile _length_ _filename_ */
    lua_getglobal(L, "cmdLuaNetSend");
    lua_pushvalue(L, 1);
    lua_pushfstring(L, "luapushfile %d %s\n", filesize,
            lua_tostring(L, 2));
    lua_call(L, 2, 0);

    /* wait ack */
    lua_getglobal(L, "cmdLuaNetWaitString");
    lua_pushvalue(L, 1);
    lua_pushnumber(L, timeout);
    lua_call(L, 2, 1);
    if (!lua_isstring(L, -1))
    {
        /* timeout waiting ack */
        lua_pop(L, 1);
        lua_pushliteral(L, "timeout waiting ack");
ret_err:
        return 1;
    }
    /* 200 xxx   expected as correct ack */
    if (lua_tostring(L, -1)[0] != '2')
    {
        lua_pushliteral(L, "bad ack: ");
        lua_insert(L, -2);
        lua_concat(L, 2);
        goto ret_err;
    }

    /* now push file data */
    {
        size_t  size;
        const char  *data;
        while ((data = cmdLuaFSReader(L, &f, &size)) != NULL)
        {
            lua_getglobal(L, "cmdLuaNetSend");
            lua_pushvalue(L, 1);
            lua_pushlstring(L, data, size);
            lua_call(L, 2, 0);
        }
    }

    /* wait final ack */
    lua_getglobal(L, "cmdLuaNetWaitString");
    lua_pushvalue(L, 1);
    lua_pushnumber(L, timeout);
    lua_call(L, 2, 1);
    if (!lua_isstring(L, -1))
    {
        /* timeout waiting ack */
        lua_pop(L, 1);
        lua_pushliteral(L, "timeout waiting final ack");
        return 1;
    }
    /* 200 xxx   expected as correct ack */
    if (lua_tostring(L, -1)[0] != '2')
    {
        lua_pushliteral(L, "bad final ack: ");
        lua_insert(L, -2);
        lua_concat(L, 2);
        return 1;
    }

    lua_pushliteral(L, "OK");
    return 1;
}
#endif /* defined(CMD_LUA_FS) */
#endif /* CMD_LUA_CLI */

/*******************************************************************************
* cmdLuaEncodeHex
*
* DESCRIPTION:
*       Encode userdata into Hex 
*
* INPUTS:
*       L       - lua_State
*        1st    - userdata
*
* OUTPUTS:
*       None
*
* RETURNS:
*       nil if input parameter is not userdata
*       string
*
* COMMENTS:
*
*******************************************************************************/
static int cmdLuaEncodeHex(lua_State *L)
{
    unsigned char *data;
    size_t size;
    if (lua_type(L, 1) != LUA_TUSERDATA)
    {
        return 0;
    }
    data = (unsigned char*)lua_touserdata(L, 1);
    if (!data)
    {
        lua_pushnil(L);
        return 1;
    }
    lua_pushliteral(L, "");
    for (size = lua_objlen(L, 1); size; size--, data++)
    {
        lua_pushfstring(L, "%02x", *data);
        lua_concat(L, 2);
    }
    return 1;
}

/*******************************************************************************
* cmdLuaGetType
*
* DESCRIPTION:
*       get type of data including cmdLua custom ones
*
* INPUTS:
*       L       - lua_State
*        1st    - value
*
* OUTPUTS:
*       None
*
* RETURNS:
*       string
*
* COMMENTS:
*
*******************************************************************************/
static int cmdLuaGetType(lua_State *L)
{
    int type;

    type = lua_type(L, 1);
    if (type != LUA_TUSERDATA)
    {
        lua_pushstring(L, lua_typename(L, type));
        return 1;
    }
    type = cmdLuaGetCustomType(L, 1);
    switch (type)
    {
        case CMD_LUA_TYPE_UNKNOWN:
            lua_pushstring(L, lua_typename(L, LUA_TUSERDATA));
            break;
        case CMD_LUA_TYPE_BYTEARRAY:
            lua_pushliteral(L, "bytearray");
            break;
        case CMD_LUA_TYPE_U16ARRAY:
            lua_pushliteral(L, "u16array");
            break;
        case CMD_LUA_TYPE_U32ARRAY:
            lua_pushliteral(L, "u32array");
            break;
        case CMD_LUA_TYPE_U64:
            lua_pushliteral(L, "u64");
            break;
        default:
            lua_pushliteral(L, "unknown");
    }
    return 1;
}

/***** Public Functions ************************************************/

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
int luaopen_cmdLua(lua_State *L)
{
    lua_settop(L, 1);  /* _LOADED table will be at index 2 */
    lua_getfield(L, LUA_REGISTRYINDEX, "_LOADED");
    lua_getfield(L, 2, "cmdLua)");
    if (lua_toboolean(L, -1)) {
        /* already loaded */
        return 0;
    }

    /* register functions */
    lua_register(L, "cmdLuaEncodeHex", cmdLuaEncodeHex);
    lua_register(L, "cmdLuaGetType", cmdLuaGetType);
#ifdef  CMD_LUA_SRV
    lua_register(L, "cmdLuaExec", cmdLuaExecWrapper);
    lua_register(L, "cmdLuaPushResult", cmdLuaPushResult);
    lua_register(L, "cmdLuaPushResultTblItem", cmdLuaPushResultTblItem);
    lua_register(L, "cmdLuaEndResults", cmdLuaEndResults);
    lua_register(L, "cmdLuaSerializeValue", cmdLuaSerializeValue);
#ifdef  CMD_LUA_CLI
    lua_register(L, "cmdLuaParseValue", cmdLuaParseValue);

    lua_register(L, "cmdLuaNetWaitResult", cmdLuaNetWaitResult);
#ifdef  CMD_LUA_FS
    lua_register(L, "cmdLuaNetUpload", cmdLuaNetUpload);
#endif /* CMD_LUA_FS */
    /* set cmdLua.cmdTimeOut */
    lua_getfield(L, LUA_GLOBALSINDEX, "cmdLua");
    if (!lua_istable(L, -1))
    {
        lua_newtable(L);
        lua_setfield(L, LUA_GLOBALSINDEX, "cmdLua");
        lua_getfield(L, LUA_GLOBALSINDEX, "cmdLua");
    }
    lua_getfield(L, -1, "cmdTimeOut");
    if (lua_isnil(L, -1)) /* cmdLua.cmdTimeOut not defined yet */
    {
        lua_pushnumber(L, CMD_LUA_NET_WAITSTRING_TIMEOUT);
        lua_setfield(L, -3, "cmdTimeOut"); /* cmdLua.cmdTimeOut = CMD_LUA_NET_WAITSTRING_TIMEOUT */
    }
    lua_pop(L, 2);

    lua_register(L, "cmdLuaNetConnect", cmdLuaNetConnect);
    lua_register(L, "cmdLuaNetDisconnect", cmdLuaNetDisconnect);
    lua_register(L, "cmdLuaNetFlushInput", cmdLuaNetFlushInput);
    lua_register(L, "cmdLuaNetSend", cmdLuaNetSend);
    lua_register(L, "cmdLuaNetWaitString", cmdLuaNetWaitString);
#endif /* CMD_LUA_CLI */

#ifdef  CMD_LUA_FS
    cmdLuaFSregisterLoader(L);
#endif /* CMD_LUA_FS */
#endif /* CMD_LUA_SRV */

    /* mark module as loaded */
    lua_pushboolean(L, 1); /* use true */
    lua_setfield(L, 2, "cmdLua");  /* _LOADED["cmdLua"] = true */
    return 0;
}

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
int cmdLuaSetCustomType(lua_State *L, int index, int type)
{
    if (index < 0)
        index = lua_gettop(L) + 1 + index;
    if (lua_type(L, index) == LUA_TNONE)
        return 0;
    if (lua_getmetatable(L, index) == 0)
    {
        /* no metatable, create */
        lua_newtable(L);
    }
    lua_pushnumber(L, type);
    lua_setfield(L, -2, "cmdLuaType");
    lua_setmetatable(L, index);
    return 1;
}

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
int cmdLuaGetCustomType(lua_State *L, int index)
{
    int type = CMD_LUA_TYPE_UNKNOWN;

    if (lua_type(L, index) == LUA_TNONE)
        return type;
    if (lua_getmetatable(L, index) == 0)
    {
        /* no metatable, unknown type */
        return type;
    }
    lua_getfield(L, -1, "cmdLuaType");
    if (lua_isnumber(L, -1))
        type = lua_tointeger(L, -1);
    lua_pop(L, 2);
    return type;
}

