/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* wraplWriteBin.c
*
* DESCRIPTION:
*       A lua wrapper for to write binary data to genFS
*
* DEPENDENCIES:
*
* COMMENTS:
*
* FILE REVISION NUMBER:
*       $Revision: 1 $
**************************************************************************/
#include <extUtils/luaCLI/luaCLIEngine.h>
#include <cpss/generic/cpssTypes.h>

/************* Defines ********************************************************/

/************* Typedefs *******************************************************/

/************* Functions ******************************************************/
static int hexdigit2int(char ch)
{
    if (ch >= '0' && ch <= '9')
        return ch - '0';
    if (ch >= 'a' && ch <= 'f')
        return ch - 'a' + 10;
    if (ch >= 'A' && ch <= 'F')
        return ch - 'A' + 10;
    if (ch == ' ' || ch == '\t' || ch == '\r' || ch == '\n')
        return -2;
    return -1;
}

static int hexstr2bin(const char *str, void *dst)
{
    int dstlen = 0;
    int byte = 0, s, digit;
    s = 0;
    for (; *str; str++)
    {
        digit = hexdigit2int(*str);
        if (digit == -2 || *str == '-') /* space or comment */
        {
            if (s)
            {
                if (dst)
                    ((GT_U8*)dst)[dstlen] = (GT_U8) byte;
                dstlen++;
            }
            s = 0;
            if (*str == '-')
            {
                /* comment, skip to EOL */
                while (*str && *str != '\n')
                    str++;
                if (!*str)
                    return dstlen;
            }
            continue;
        }
        if (digit == -1) /* wrong character */
            return dstlen;
        if (s)
        {
            if (dst)
                ((GT_U8*)dst)[dstlen] = (GT_U8) (byte * 16 + digit);
            dstlen++;
        }
        else
            byte = digit;
        s ^= 1;
    }
    return dstlen;
}

/*******************************************************************************
* wraplWriteHexAsBinary
*
* DESCRIPTION:
*       Write to a file descriptor, convert hex data to binary
*
* INPUTS:
*       L       - Lua state
*        1st    - file descriptor
*        2nd    - hex string to write
*
* OUTPUTS:
*       None
*
* RETURNS:
*       The number of bytes written or nil + error message
*
* COMMENTS:
*
*******************************************************************************/
int wraplWriteHexAsBinary
(
    lua_State *L
)
{
    int     fd;
    int     ret;
    int     len;
    void    *data;
    genFS_PTR fs = (genFS_PTR)luaCLIEngineGetData(L, "_genFS", 0);

    if (!lua_isnumber(L, 1) || !lua_isstring(L, 2))
    {
        lua_pushnil(L);
        lua_pushliteral(L, "Bad parameter");
        return 2;
    }
    if (fs == NULL)
    {
        lua_pushnil(L);
        lua_pushliteral(L, "_genFS not configured");
        return 2;
    }
    fd = lua_tointeger(L, 1);
    len = hexstr2bin(lua_tostring(L, 2), NULL); /* first pass, resolve len */
    if (len < 0)
    {
        lua_pushnil(L);
        lua_pushliteral(L, "Bad string, must be hex data");
        return 2;
    }
    if (!len)
    {
        lua_pushinteger(L, len);
        return 1;
    }
    data = lua_newuserdata(L, (size_t)len);
    if (!data)
    {
        lua_pushnil(L);
        lua_pushfstring(L, "Failed to allocate userdata (%d bytes)", len);
        return 2;
    }
    hexstr2bin(lua_tostring(L, 2), data);
    ret = fs->write(fd, data, len);
    if (ret < 0)
    {
        lua_pop(L, 1);
        lua_pushnil(L);
        lua_pushstring(L, fs->lastError());
        return 2;
    }
    lua_pushinteger(L, ret);
    return 1;
}
