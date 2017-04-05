/*******************************************************************************
* HttpParser.c
*
* DESCRIPTION:
*       Web server implementation.
*       HTTP parser
*
* DEPENDENCIES:
*
* FILE REVISION NUMBER:
*       $Revision: 1$
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

#include <lua.h>
#include <lualib.h>

/***** Defines  ********************************************************/

/***** Global macros ***************************************************/

/***** Private Types ***************************************************/
/*
typedef struct tsubstr {
    const char *ptr;
    int len;
} tsubstr;
*/
typedef struct  {
    const char *ptr;
    int len;
} tsubstr;


/***** Private Data ****************************************************/

/***** Private Functions ***********************************************/

/***********************************************************
* parser engine
*
* These function operate with strings represented by pointer and length
*
* The memory for them allocated somewhere else, so they are valid
* till object unallocated
*
* For example:
* char a[] = "The sample string";
* tsubstr t = { a, sizeof(a)-1 }  - t == "The sample string"
* t = tsubstr_tail(t, 4);         - t == "sample string"
*
***********************************************************/
/* new tsubstr */
static tsubstr tsubstr_new(const char *ptr, int len)
{
    tsubstr t = { ptr, len };
    return t;
}
/* remove last chat if it match ch */
static tsubstr tsubstr_cutlastchar(tsubstr *t, char ch)
{
    if (t->len && t->ptr[t->len-1] == ch)
        t->len--;
    return *t;
}
/* remove first chat if it match ch */
static tsubstr tsubstr_cutfirstchar(tsubstr *t, char ch)
{
    if (t->len && t->ptr[0] == ch)
    {
        t->ptr++;
        t->len--;
    }
    return *t;
}
/* find char, return index or -1 */
static int tsubstr_find_char(tsubstr t, char ch)
{
    int p = 0;
    for (p = 0; p < t.len; p++)
        if (t.ptr[p] == ch)
            return p;
    return -1;
}
/* find string */
static int tsubstr_find_tsubstr(tsubstr t, tsubstr s)
{
    const char *p, *e;
    if (s.len == 0 || s.len > t.len)
        return -1;
    e = t.ptr + (t.len - s.len);
    p = t.ptr;
    for (p = t.ptr; p <= e; p++)
    {
        int i;
        if (*p != s.ptr[0])
            continue;
        for (i = 0; i < s.len; i++)
            if (p[i] != s.ptr[i])
                break;
        if (i == s.len)
            return (p - t.ptr);
    }
    return -1;
}
/* find string */
static int tsubstr_find_str(tsubstr t, const char *s)
{
    tsubstr a;
    a.ptr = s;
    a.len = cmdOsStrlen(s);
    return tsubstr_find_tsubstr(t, a);
}
/* cut first nChars from string */
static tsubstr tsubstr_cutb(tsubstr *t, int nChars)
{
    if (nChars > t->len)
        nChars = t->len;
    t->ptr += nChars;
    t->len -= nChars;
    return *t;
}
/* truncate string from given position */
static tsubstr tsubstr_cute(tsubstr *t, int start)
{
    if (t->len > start)
        t->len = start;
    return *t;
}
/* return the tail of the string starting at given position */
static tsubstr tsubstr_tail(tsubstr t,int start)
{
    tsubstr t1 = t;
    return tsubstr_cutb(&t1, start);
}
/* substring */
static tsubstr tsubstr_substr(tsubstr t, int start, int length)
{
    tsubstr t1 = t;
    tsubstr_cutb(&t1, start);
    return tsubstr_cute(&t1, length);
}
/* head of string */
static tsubstr tsubstr_head(tsubstr t, int length)
{
    return tsubstr_substr(t, 0, length);
}
/* remove spaces at begin and end of string */
#define IS_SPC(a)   ((a)==' ' || (a)=='\t' || (a)=='\r' || (a)=='\n')
static tsubstr tsubstr_stripspaces(tsubstr t)
{
    while (t.len && IS_SPC(*(t.ptr))) {
        t.ptr++;
        t.len--;
    }
    while (t.len && IS_SPC(t.ptr[t.len-1]))
        t.len--;
    return t;
}
/* the part of string after given substring */
static tsubstr tsubstr_after(tsubstr t, tsubstr str)
{
    tsubstr r = { NULL, 0 };
    if (t.ptr > str.ptr || (t.ptr + t.len) < (str.ptr + str.len))
    {
        return r;
    }
    r = t;
    tsubstr_cutb(&r, str.ptr-t.ptr);
    tsubstr_cutb(&r, str.len);
    return r;
}

#define lua_pushtsubstr(L, t) lua_pushlstring(L, t.ptr, t.len);
#define lua_totsubstr(L, index) tsubstr_new(lua_tostring(L, index), lua_objlen(L, index))

#define TO_UPPER(ch) (((ch)>='a' && (ch) <= 'z') ? (ch) - ('a' - 'A') : (ch))
static int pars_strcasecmp(const char *s1, const char *s2)
{
    for (;*s1 && *s2; s1++, s2++)
    {
        if (TO_UPPER(*s1) != TO_UPPER(*s2))
            break;
    }
    if (*s1 == 0 && *s2 == 0)
        return 0;
    if (*s1 > *s2)
        return 1;
    return -1;
}
int lua_equal_ignorecase(lua_State *L, int a, int b)
{
    return (pars_strcasecmp(lua_tostring(L, a), lua_tostring(L, b)) == 0) ? 1 : 0;
}

/*******************************************************************************
* HttpParser_DecodeUrlEncoded
*
* DESCRIPTION:
*       Decode URL encoded string, Fx sss%2Fttt decoded to sss/ttt
*
* INPUTS:
*       L       - Lua state
*       str     - string to decode
*
* OUTPUTS:
*       decoded string at the top of stack
*
* RETURNS:
*       None
*
* COMMENTS:
*
*******************************************************************************/
static void HttpParser_DecodeUrlEncoded(lua_State *L, tsubstr str)
{
    int     p;
    char    o[64];

    lua_pushliteral(L, "");
    for (p = 0; str.len; tsubstr_cutb(&str, 1))
    {
        if (p == sizeof(o))
        {
            lua_pushlstring(L, o, p);
            lua_concat(L, 2);
            p = 0;
        }
        if (*str.ptr == '%')
        {
            tsubstr hex = { "0123456789ABCDEF", 16 };
            int d1, d2;
            if (str.len < 3)
                break;
            d1 = tsubstr_find_char(hex, str.ptr[1]);
            d2 = tsubstr_find_char(hex, str.ptr[2]);
            if (d1 >= 0 && d2 >= 0)
            {
                o[p++] = d1*16+d2;
                tsubstr_cutb(&str, 2);
                continue;
            }
        }
        /* ?? only form data? */
        if (*str.ptr == '+')
        {
            o[p++] = ' ';
            continue;
        }
        o[p++] = *str.ptr;
    }
    if (p)
    {
        lua_pushlstring(L, o, p);
        lua_concat(L, 2);
    }
}

/*******************************************************************************
* HttpParser_parse_x_www_form_urlencoded
*
* DESCRIPTION:
*       Decode
*           get varialbles,
*           application/x-www-form-urlencoded
*       Syntax:
*           data := vardecl[&vardecl[&vardecl ... ]]
*           vardecl := varname | varname '=' value
*
* INPUTS:
*       L       - Lua state
*       table   - table index to save data to
*       data    - data to decode
*
* OUTPUTS:
*       L       - Lua table filled
*
* RETURNS:
*       None
*
* COMMENTS:
*
*******************************************************************************/
static void HttpParser_parse_x_www_form_urlencoded(lua_State *L, int table, tsubstr data)
{
    int     p;
    tsubstr var, val;

    while (data.len)
    {
        if ((p = tsubstr_find_char(data,'&')) < 0)
        {
            var = data;
            tsubstr_cutb(&data, data.len);
        }
        else
        {
            var = tsubstr_head(data, p);
            tsubstr_cutb(&data, p + 1);
        }
        /* var = vardecl now */
        if ((p = tsubstr_find_char(var, '=')) >= 0)
        {
            val = tsubstr_tail(var, p+1);
            tsubstr_cute(&var, p);
        }
        else
        {
            val = tsubstr_tail(var, var.len);
        }
        /* vars[var] = decode_urlencoded(val); */
        lua_pushtsubstr(L, var);
        HttpParser_DecodeUrlEncoded(L, val);
        lua_setfield(L, table, lua_tostring(L, -2));
        lua_pop(L, 1);
    }
}

#define SET_HTTP_VAR(name, t) \
    { \
        tsubstr tmp; \
        tmp = t; \
        lua_pushtsubstr(L, tmp); \
        lua_setfield(L, http, name); \
    }
/*******************************************************************************
* HttpParser_ParseURL
*
* DESCRIPTION:
*       Parse URL = http://user:pass@host.com:8081/path?var1=val1&var2=val2
*
* INPUTS:
*       L       - Lua state
*       http    - Lua index of global table "http"
*
* OUTPUTS:
*       L       - fields added to http
*
* RETURNS:
*       None
*
*******************************************************************************/
static void HttpParser_ParseURL(lua_State *L, int http)
{
    int p;
    tsubstr turi;

    /* turi = http.url */
    lua_getfield(L, http, "url");
    turi = lua_totsubstr(L, -1);

    if ((p = tsubstr_find_char(turi, '?')) >= 0)
    {
        lua_getfield(L, http, "get_vars");
        /* HttpParser_parse_x_www_form_urlencoded(L, http.get_vars, "var1=val1&var2=val2"); */
        HttpParser_parse_x_www_form_urlencoded(L, lua_gettop(L), tsubstr_tail(turi, p+1));
        lua_pop(L, 1);
        tsubstr_cute(&turi, p);
        /* turi = "http://user:pass@host.com:8081/path" */
    }
    if ((p = tsubstr_find_str(turi,"://")) >= 0)
    {
        tsubstr thost = tsubstr_tail(turi, p + 3);
        /* thost = "user:pass@host.com:8081/path" */
        SET_HTTP_VAR("scheme", tsubstr_head(turi,p));
        /* http.scheme = "http" */
        
        if ((p = tsubstr_find_char(thost,'/')) >= 0)
        {
            HttpParser_DecodeUrlEncoded(L, tsubstr_tail(thost, p));
            lua_setfield(L, http, "/path");
            /* http.path = "path" */
            tsubstr_cute(&thost,p);
            /* thost = user:pass@host.com:8081 */
        }
        else
        {
            lua_pushliteral(L, "/");
            lua_setfield(L, http, "path");
        }
        
        if ((p = tsubstr_find_char(thost,'@')) >= 0)
        {
            tsubstr tuser = tsubstr_head(thost,p);
            /* tsuser = user:pass */
            tsubstr_cutb(&thost,p+1);
            /* thost = host.com:8081 */
            if ((p = tsubstr_find_char(tuser,':')) >= 0)
            {
                HttpParser_DecodeUrlEncoded(L, tsubstr_tail(tuser, p+1));
                lua_setfield(L, http, "pass");
                /* http.pass = "pass" */
                tsubstr_cute(&tuser,p);
                /* tuser = "user */
            }
            HttpParser_DecodeUrlEncoded(L, tuser);
            lua_setfield(L, http, "user");
            /* http.user = "user" */
        }
        if ((p = tsubstr_find_char(thost,':')) >= 0)
        {
            HttpParser_DecodeUrlEncoded(L, tsubstr_tail(thost,p+1));
            lua_setfield(L, http, "port");
            /* http.port = "8081" */
            tsubstr_cute(&thost,p);
            /* thost = "host.com" */
        }
        HttpParser_DecodeUrlEncoded(L, thost);
        lua_setfield(L, http, "host");
        /* http.host = "host.com" */
    }
    else
    {
        HttpParser_DecodeUrlEncoded(L, turi);
        lua_setfield(L, http, "path");
        /* http.path = "/path" */
    }
    HttpParser_DecodeUrlEncoded(L, turi);
    lua_setfield(L, http, "uri");
    /* http.uri = "http://user:pass@host.com:8081/path" */
}

/*******************************************************************************
* HttpParser_ParseQuery
*
* DESCRIPTION:
*       Parse HTTP query string
*       Query example:
*           GET http://host/aa HTTP/1.0
*       Where
*           GET         - method := GET | POST
*           HTTP/1.0    - protocol := HTTP/1.*
*
* INPUTS:
*       L       - Lua state
*       data    - query string pointer
*       len     - query string length
*
* OUTPUTS:
*       L       - the global table "http" filled
*
* RETURNS:
*       Non-zero if error
*
*******************************************************************************/
int HttpParser_ParseQuery(lua_State *L, const char *data, int len)
{
    tsubstr query = { data, len };
    tsubstr u;
    int http;
    int p;
    int last_error_code = 0;
    const char *last_error = "";

    lua_getglobal(L, "http");
    http = lua_gettop(L);

    tsubstr_cutlastchar(&query, '\n');
    tsubstr_cutlastchar(&query, '\r');
    
    /* extract method */
    /* query = "GET http://host/aa HTTP/1.0" */
    if ((p = tsubstr_find_char(query, ' ')) < 0)
    {
        last_error_code = 400;
        last_error = "Bad request";
        goto return_error;
    }
    u = tsubstr_tail(query,p+1);
    /* u = "http://host/aa HTTP/1.0" */
    SET_HTTP_VAR("method", tsubstr_head(query, p));

    if ((p = tsubstr_find_char(u, ' ')) < 0)
    {
        last_error = "Bad request";
        last_error_code = 400;
        goto return_error;
    }
    SET_HTTP_VAR("proto", tsubstr_tail(u, p+1))
    SET_HTTP_VAR("url", tsubstr_head(u, p))

    /*!! check method */
    lua_getfield(L, http, "method");
    if (cmdOsStrCmp(lua_tostring(L, -1), "GET") != 0 && 
        cmdOsStrCmp(lua_tostring(L, -1), "POST") != 0)
    {
        last_error = "Method Not Acceptable";
        last_error_code = 406;
        goto return_error;
    }
    lua_pop(L, 1);

    /*!! check protocol */
    lua_getfield(L, http, "proto");
    if (tsubstr_find_str(lua_totsubstr(L, -1), "HTTP/1.") != 0)
    {
        last_error = "Bad protocol";
        last_error_code = 406;
        goto return_error;
    }
    lua_pop(L, 1);

    /* Now parse URL */
    HttpParser_ParseURL(L, http);

    lua_settop(L, http);
    lua_pop(L, 1);
    return 0;

return_error:
    lua_pushfstring(L, "%03d", last_error_code);
    lua_setfield(L, http, "responce_code");
    lua_pushstring(L, last_error);
    lua_setfield(L, http, "responce");
    lua_settop(L, http);
    lua_pop(L, 1);
    return 1;
}

/*******************************************************************************
* parse_rfc822hdr
*
* DESCRIPTION:
*       Parse RFC 822 type header
*
* INPUTS:
*       L       - Lua state
*       data    - data to parse
*       headers - Lua index of parsed headers table
*
* OUTPUTS:
*       L       - table filled
*
* RETURNS:
*       The rest of data (body)
*
*******************************************************************************/
static tsubstr parse_rfc822hdr(lua_State *L, tsubstr data, int headers)
{
    int p;
    tsubstr line, nline, header_name, header_content;

    while ((p = tsubstr_find_char(data, '\n')) >= 0)
    {
        line = tsubstr_head(data, p); /* line = current line */
        tsubstr_cutb(&data, p+1); /* remove this line from start of data */
        tsubstr_cutlastchar(&line, '\r'); /* remove optional \r at end of line */
        if (line.len == 0)
            break; /* line empty, header finished */
        
        /* while next line starts with SPACE or TAB add it to this header */
        while (data.len && (data.ptr[0] == ' ' || data.ptr[0] == '\t'))
        {
            if ((p = tsubstr_find_char(data, '\n')) < 0)
                break;
            nline = tsubstr_head(data, p);
            tsubstr_cutb(&data, p+1);
            tsubstr_cutlastchar(&nline, '\r');
            line.len = (nline.ptr + nline.len) - line.ptr;
        }

        /* header name delimited by ':' */
        if ((p = tsubstr_find_char(line, ':')) < 0)
            continue;
        
        header_name = tsubstr_stripspaces(tsubstr_head(line, p));
        header_content = tsubstr_stripspaces(tsubstr_tail(line, p+1));
        /* headers[#headers+1] = { header_name, header_content } */
        lua_newtable(L);
        lua_pushtsubstr(L, header_name);
        lua_rawseti(L, -2, 1);
        lua_pushtsubstr(L, header_content);
        lua_rawseti(L, -2, 2);
        lua_rawseti(L, headers, lua_objlen(L, headers)+1);
    }
    return data;
}

/*******************************************************************************
* get_token
*
* DESCRIPTION:
*       get token from begin of string
*
* INPUTS:
*       ss      - string to get token from
*
* OUTPUTS:
*       None
*
* RETURNS:
*       token
*
*******************************************************************************/
static tsubstr get_token(tsubstr ss)
{
    int k;

    tsubstr s = ss;
    s = tsubstr_stripspaces(s);
    for (k = 0; k < s.len; k++) {
        char ch = s.ptr[k];
        if (ch >= 0 && ch <= ' ')
            break;
        if (cmdOsStrChr("()<>@,;:\\\"/[]?=", ch) != NULL)
            break;
    }
    return tsubstr_head(s, k);
}

/*******************************************************************************
* parse_attr_value_list
*
* DESCRIPTION:
*       Parse the given string for list of attributes:
*       s = name1=value1[; name2="value 2"[; ...]]
*
* INPUTS:
*       L       - Lua state with table at the top of list
*
* OUTPUTS:
*       None
*
* RETURNS:
*       0 on error
*       1 on success
*
*******************************************************************************/
static int parse_attr_value_list(lua_State *L, tsubstr s)
{
    while (s.len)
    {
        s = tsubstr_stripspaces(s);
        tsubstr a = get_token(s);
        if (a.len == 0)
            break;
        s = tsubstr_stripspaces(tsubstr_after(s, a));
        if (s.len == 0 || s.ptr[0] != '=')
            return 0;
        /* a == "name1",  s == "=value1; name2=...." */
        s = tsubstr_stripspaces(tsubstr_tail(s, 1));
        if (s.len == 0)
        {
            /* table[a] = s */
            lua_pushtsubstr(L, a);
            lua_pushtsubstr(L, s);
            lua_setfield(L, -3, lua_tostring(L, -2));
            lua_pop(L, 1);
            break;
        }
        if (s.ptr[0] != '"')
        {
            /* the value is not quoted, get next token as value */
            tsubstr v = get_token(s);
            /* table[a] = v */
            lua_pushtsubstr(L, a);
            lua_pushtsubstr(L, v);
            lua_setfield(L, -3, lua_tostring(L, -2));
            lua_pop(L, 1);

            s = tsubstr_after(s, v);
            /* s == "; name2=...." */
        }
        else
        {
            /* quoted string */
            char o[64];
            tsubstr v = s; /* v == "\"value 2\";..." */
            int l = 0;
            int k;

            lua_pushtsubstr(L, a);
            lua_pushliteral(L, "");
            for (k = 1; k < v.len; k++)
            {
                if (l >= sizeof(o))
                {
                    lua_pushlstring(L, o, l);
                    lua_concat(L, 2);
                    l = 0;
                }
                if (v.ptr[k] == '"')
                    break;
                if (v.ptr[k] == '\\')
                {
                    if (k+1 < v.len)
                        o[l++] = v.ptr[++k];
                    else
                        break;
                } 
                else
                {
                    o[l++] = v.ptr[k];
                }
            }
            if (l > 0)
            {
                lua_pushlstring(L, o, l);
                lua_concat(L, 2);
            }
            if (v.ptr[k] != '"')
            {
                lua_pop(L, 2);
                break;
            }
            tsubstr_cute(&v, k+1); /* v = "\"value 2\"" */
            /* table[a] = dequoted string */
            lua_setfield(L, -3, lua_tostring(L, -2));
            lua_pop(L, 1);
            s = tsubstr_after(s, v); /* s = ";..." */
        }
        s = tsubstr_stripspaces(s);
        if (s.len == 0 || s.ptr[0] != ';')
            break;
        /* ';' at begin of s, remove it an parse next attribute */
        tsubstr_cutb(&s, 1);
    }
    return 1;
}

/*
 * rfc2045 base64
 */
static char b64chars[256] = {
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 62, -1, -1, -1, 63,
	52, 53, 54, 55, 56, 57, 58, 59, 60, 61, -1, -1, -1, -1, -1, -1,
	-1,  0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 14,
	15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, -1, -1, -1, -1, -1,
	-1, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40,
	41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, -1, -1, -1, -1, -1,
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1
};

/*******************************************************************************
* decode_base64
*
* DESCRIPTION:
*       Decode rfc2045 base64
*       The decoded string is pushed to the top of Lua stack
*
* INPUTS:
*       L       - Lua state
*
* OUTPUTS:
*       L       - decoded string at the top
*
* RETURNS:
*       None
*
*******************************************************************************/
static void decode_base64(lua_State *L, tsubstr str)
{
	int stt = 0, a[4];
	int	p;
	char	t, d[64];

    lua_pushliteral(L, "");

	for (p = 0; str.len; str.ptr++, str.len--) {
        if (p + 4 >= sizeof(d))
        {
            lua_pushlstring(L, d, p);
            lua_concat(L, 2);
            p = 0;
        }
		t = b64chars[(unsigned char)(*str.ptr)];
		if (t != -1)
			a[stt++] = t;
		if (*str.ptr == '=' || stt == 4) {
			d[p+0] = ((a[0] << 2) & 0xfc) | ((a[1] >> 4) & 0x03);
			d[p+1] = ((a[1] << 4) & 0xf0) | ((a[2] >> 2) & 0x0f);
			d[p+2] = ((a[2] << 6) & 0xc0) | (a[3] & 0x3f);
			p += "\0\0\1\2\3"[stt];
			stt = 0;
		}
		if (*str.ptr == '=')
			break;
	}
    if (p > 0)
    {
        lua_pushlstring(L, d, p);
        lua_concat(L, 2);
    }
}

/*******************************************************************************
* valignorecase
*
* DESCRIPTION:
*       Search table of headers. Ignore name case
*       The index points the table in form:
*       table = {
*           { name, value },
*           { name, value },
*           ...
*       }
*
* INPUTS:
*       L       - Lua state with name on top of stack
*
* OUTPUTS:
*       L       - name removed from stack, value or nil pushed
*
* RETURNS:
*       None
*
*******************************************************************************/
static void valignorecase(lua_State *L, int index)
{
    int parm;
    int h;

    parm = lua_gettop(L);

    for (h = 1; h <= lua_objlen(L, index); h++)
    {
        lua_rawgeti(L, index, h);
        /* header at the top */

        lua_rawgeti(L, -1, 1);
        /* header name at the top */
        if (lua_equal_ignorecase(L, -1, parm))
        {
            lua_rawgeti(L, -2, 2);
            lua_replace(L, parm);
            lua_settop(L, parm);
            return;
        }

        lua_pop(L, 2);
    }

    lua_pushnil(L);
    lua_replace(L, parm);
    lua_settop(L, parm);
}

/*******************************************************************************
* HttpParser_parse_ct_cd
*
* DESCRIPTION:
*       Parse Content-Type and Content-Disposition headers
*       They have the same syntax:
*       Content-Type := value [ ';' ATTRIB_LIST ]
*       ATTRIB_LIST := ATTRIB [ ';' ATTRIB ....]
*       ATTRIB := NAME '=' VALUE
*       NAME := token
*       value := token | quoted string
*
* INPUTS:
*       L       - Lua state
*
* OUTPUTS:
*       L       - The value string and attrib_list pushed to stack
*
* RETURNS:
*       1 on success
*
*******************************************************************************/
static int HttpParser_parse_ct_cd(lua_State *L, tsubstr s)
{
    int p;
    int mpart;
    tsubstr t;

    lua_pushliteral(L, "");
    mpart = lua_gettop(L);
    lua_newtable(L); /* parms */
    if ((p = tsubstr_find_char(s, ';')) < 0)
    {
        /* no attributes */
        s = tsubstr_stripspaces(s);
        lua_pushtsubstr(L, s);
        lua_replace(L, mpart); /* mpart = s */
        return 1;
    }
    t = tsubstr_stripspaces(tsubstr_head(s, p));
    lua_pushtsubstr(L, t);
    lua_replace(L, mpart); /* mpart = s */
    tsubstr_cutb(&s,p+1);
    return parse_attr_value_list(L, s);
}

/*******************************************************************************
* HttpParser_parse_multipart_form_data
*
* DESCRIPTION:
*       Parse multipart/form-data
*
* INPUTS:
*       L       - Lua state
*
* OUTPUTS:
*       L       - The value string and attrib_list pushed to stack
*
* RETURNS:
*       1 on success
*
*******************************************************************************/
static void HttpParser_parse_multipart_form_data(lua_State *L, tsubstr data, int boundary, int http)
{
    int p;
    tsubstr bound, bbound;
    int ltop;

    ltop = lua_gettop(L);

    /* bound = "--" + boundary */
    lua_pushliteral(L, "--");
    lua_pushvalue(L, boundary);
    lua_concat(L, 2);
    bound = lua_totsubstr(L, -1);

    /* bbound = "\r\n--" + boundary */
    lua_pushliteral(L, "\r\n--");
    lua_pushvalue(L, boundary);
    lua_concat(L, 2);
    bbound = lua_totsubstr(L, -1);

    while ((p = tsubstr_find_tsubstr(data, bound)) >= 0) {
        tsubstr part, partdata;
        int parthdr, cdparms;

        /* get part from the begin of data */
        tsubstr_cutb(&data, p+bound.len);
        if (data.len >= 2 && data.ptr[0] == '-' && data.ptr[1] == '-')
            break;
        tsubstr_cutfirstchar(&data, '\r');
        tsubstr_cutfirstchar(&data, '\n');
        if ((p = tsubstr_find_tsubstr(data, bbound)) < 0)
            break;
        part = tsubstr_head(data, p);
        /* remove this part from data */
        tsubstr_cutb(&data, p + 2);

        /* parse part... */
        lua_newtable(L);
        parthdr = lua_gettop(L);
        partdata = parse_rfc822hdr(L, part, parthdr);

        lua_pushliteral(L, "Content-Disposition");
        valignorecase(L, parthdr);
        if (lua_isnil(L, -1))
        {
            lua_pop(L, 2);
            continue;
        }
        if (!HttpParser_parse_ct_cd(L, lua_totsubstr(L, -1)))
        {
            lua_pop(L, 4);
            continue;
        }
        /* Content-Disposition at stack-2, attrs at stack - 1 */
        cdparms = lua_gettop(L);

        /* if Content-Disposition == form-data */
        lua_pushliteral(L, "form-data");
        p = lua_equal(L, -1, -3);
        lua_pop(L, 1);
        if (p)
        {
            lua_getfield(L, cdparms, "name");
            if (lua_isstring(L, -1))
            {
                int post_vars;
                int name;
                name = lua_gettop(L);
                lua_getfield(L, http, "post_vars");
                post_vars = lua_gettop(L);

                /* http.post_vars[cdparms.name] = partdata */
                lua_pushtsubstr(L, partdata);
                lua_setfield(L, post_vars, lua_tostring(L, name));

                lua_getfield(L, cdparms, "filename");
                if (lua_isstring(L, -1))
                {
                    lua_pushvalue(L, name);
                    lua_pushliteral(L, "_filename");
                    lua_concat(L, 2);
                    lua_insert(L, -2);
                    lua_setfield(L, post_vars, lua_tostring(L, -2));
                }
                lua_pop(L, 1);

                lua_pushliteral(L, "Content-Type");
                valignorecase(L, parthdr);
                if (lua_isstring(L, -1))
                {
                    lua_pushvalue(L, name);
                    lua_pushliteral(L, "_content_type");
                    lua_concat(L, 2);
                    lua_insert(L, -2);
                    lua_setfield(L, post_vars, lua_tostring(L, -2));
                    lua_pop(L, 1);
                }
                lua_pop(L, 1);
            }
            lua_pop(L, 1);
        }

        lua_pop(L, 4);
    }
    /* restore Lua stack */
    lua_settop(L, ltop);
}

/*******************************************************************************
* HttpParser_getHeader
*
* DESCRIPTION:
*       Get HTTP query header (case insensetive)
*
* INPUTS:
*       L       - Lua state
*        1st    - header name
*
* OUTPUTS:
*       L       - Lua state
*
* RETURNS:
*       header value or nil
*
*******************************************************************************/
int HttpParser_getHeader(lua_State *L)
{
    int     headers;
    if (lua_gettop(L) < 1)
    {
        lua_pushnil(L);
        return 1;
    }
    lua_settop(L, 1);
    if (!lua_isstring(L, 1))
    {
        lua_pushnil(L);
        return 1;
    }
    /* search http.headers for specified header, return header text or nil */
    lua_getglobal(L, "http");
    lua_getfield(L, -1, "headers");
    lua_remove(L, -2);
    headers = lua_gettop(L);

    lua_pushvalue(L, 1);
    valignorecase(L, headers);

    return 1;
}

/*******************************************************************************
* HttpParser_ParseHeader
*
* DESCRIPTION:
*       Parse HTTP query header
*
* INPUTS:
*       L       - Lua state
*       data    - pointer to header start
*       len     - header length
*
* OUTPUTS:
*       L       - the global table "http" filled
*
* RETURNS:
*       0 on success
*       1 on error
*
*******************************************************************************/
int HttpParser_ParseHeader(lua_State *L, const char *data, int len)
{
    int http;
    int h;
    int last_error_code = 0;
    const char *last_error = "";
    tsubstr ret;
    tsubstr header = { data, len };

    lua_getglobal(L, "http");
    http = lua_gettop(L);

    /* store this for degug only */
    lua_pushlstring(L, data, len);
    lua_setfield(L, http, "header");

    lua_getfield(L, http, "headers");
    ret = parse_rfc822hdr(L, header, lua_gettop(L));
    if (ret.len)
    {
        last_error = "Bad request";
        last_error_code = 400;
        goto return_error;
    }
    lua_pop(L, 1);

    lua_pushcfunction(L, HttpParser_getHeader);
    lua_pushliteral(L, "Authorization");
    lua_call(L, 1, 1);
    if (lua_isstring(L, -1))
    {
        /* Parse authorization field */
        tsubstr auth = lua_totsubstr(L, -1);
        if (tsubstr_find_str(auth, "Basic ") == 0)
        {
            int     p;
            tsubstr user, pass;

            decode_base64(L, tsubstr_tail(auth, 6));
            user = lua_totsubstr(L, -1);
            if ((p = tsubstr_find_char(user, ':')) >= 0)
            {
                pass = tsubstr_tail(user, p+1);
                user = tsubstr_head(user, p);
                lua_pushtsubstr(L, user);
                lua_setfield(L, http, "user");
                lua_pushtsubstr(L, pass);
                lua_setfield(L, http, "pass");
            }
            lua_pop(L, 1);
        }
    }
    lua_pop(L, 1);


    lua_pushcfunction(L, HttpParser_getHeader);
    lua_pushliteral(L, "Host");
    lua_call(L, 1, 1);
    if (lua_isstring(L, -1))
    {
        /* Parse Host field */
        tsubstr host;
        int     p;

        host = lua_totsubstr(L, -1);
        if ((p = tsubstr_find_char(host, ':')) >= 0)
        {
            tsubstr port = tsubstr_tail(host, p + 1);
            host = tsubstr_head(host, p);
            if (port.len)
            {
                lua_pushtsubstr(L, port);
                lua_setfield(L, http, "port");
            }
        }
        if (host.len)
        {
            lua_pushtsubstr(L, host);
            lua_setfield(L, http, "host");
        }
    }
    lua_pop(L, 1);

    /* handle cookies */
    lua_getfield(L, http, "headers");
    for (h = 1; h <= lua_objlen(L, -1); h++)
    {
        lua_rawgeti(L, -1, h);
        /* header at the top */

        lua_rawgeti(L, -1, 1);
        /* header name at the top */
        if (pars_strcasecmp(lua_tostring(L, -1), "Cookie") == 0)
        {
            lua_rawgeti(L, -2, 2);
            
            lua_getfield(L, http, "cookies");
            parse_attr_value_list(L, lua_totsubstr(L, -2));

            lua_pop(L, 2);
        }

        lua_pop(L, 2);
    }
    lua_pop(L, 1);

    lua_settop(L, http);
    lua_pop(L, 1);
    return 0;

return_error:
    lua_pushfstring(L, "%03d", last_error_code);
    lua_setfield(L, http, "responce_code");
    lua_pushstring(L, last_error);
    lua_setfield(L, http, "responce");
    lua_settop(L, http);
    lua_pop(L, 1);

    return 1;
}

/*******************************************************************************
* HttpParser_ParseData
*
* DESCRIPTION:
*       Parse HTTP query data (POST variables, files)
*
* INPUTS:
*       L       - Lua state
*       data    - pointer to header start
*       len     - header length
*
* OUTPUTS:
*       L       - the global table "http" filled
*
* RETURNS:
*       0 on success
*       1 on error
*
*******************************************************************************/
int HttpParser_ParseData(lua_State *L, const char *data, int len)
{
    int http;
    tsubstr s;
    int ret;
#if 0
    int last_error_code = 0;
    const char *last_error = "";
#endif

    lua_getglobal(L, "http");
    http = lua_gettop(L);

    /* store this for degug only */
    lua_pushlstring(L, data, len);
    lua_setfield(L, http, "data");

    /* if (method != "POST") return true; */
    lua_getfield(L, http, "method");
    lua_pushliteral(L, "POST");
    if (!lua_equal(L, -1, -2))
    {
        goto return_ok;
    }
    lua_pop(L, 2);

    lua_pushcfunction(L, HttpParser_getHeader);
    lua_pushliteral(L, "Content-Type");
    lua_call(L, 1, 1);
    if (lua_isnil(L, -1))
    {
        /* Content-Type not forund, assume this is not an error */
        goto return_ok;
    }
    /* parse Content-Type header */
    s = lua_totsubstr(L, -1);
    ret = HttpParser_parse_ct_cd(L, s);
    /* now in stack: content-type, params{} */
    lua_pushliteral(L, "application/x-www-form-urlencoded");
    if (lua_equal(L, -1, -3))
    {
        /* Content-Type: application/x-www-form-urlencoded */
        lua_pop(L, 3);
        s.ptr = data;
        s.len = len;
        lua_getfield(L, http, "post_vars");
        HttpParser_parse_x_www_form_urlencoded(L, lua_gettop(L), s);
        goto return_ok;
    }
    lua_pop(L, 1);

    lua_pushliteral(L, "multipart/form-data");
    if (lua_equal(L, -1, -3))
    {
        /* Content-Type: multipart/form-data */
        lua_pop(L, 1);
        lua_getfield(L, -1, "boundary");
        if (lua_isstring(L, -1))
        {
            s.ptr = data;
            s.len = len;
            HttpParser_parse_multipart_form_data(L, s, lua_gettop(L), http);
        }
    }

return_ok:
    lua_settop(L, http);
    lua_pop(L, 1);
    return 0;

#if 0
return_error:
    lua_pushfstring(L, "%03d", last_error_code);
    lua_setfield(L, http, "responce_code");
    lua_pushstring(L, last_error);
    lua_setfield(L, http, "responce");
    lua_settop(L, http);
    lua_pop(L, 1);

    return 1;
#endif
}

/***** Public Functions ************************************************/
