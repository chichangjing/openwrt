/*******************************************************************************
*              (c), Copyright 2011, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* luaTbufWrapper.c
*
* DESCRIPTION:
*       Lua <=> C interface for tbuf
*
* DEPENDENCIES:
*
* COMMENTS:
*
* FILE REVISION NUMBER:
*       $Revision: 3 $
**************************************************************************/
#include <lua.h>
#include <generic/cider/luaTbufWrapper.h>

/*******************************************************************************
* prvLuaToTbuf
*
* DESCRIPTION:
*       Get tbuf stored in Lua object placed on stack at L_index
*
* INPUTS:
*       L                   - lua state
*       L_index             - tbuf object index in lua stack 
* 
* OUTPUTS:
*       None
*
* RETURNS:
*       tbuf        (tbuf.ptr == NULL if error)
*
* COMMENTS:
*
*******************************************************************************/
tbuf prvLuaToTbuf(
    lua_State       *L,
    int             L_index
)
{
    if (!lua_isuserdata(L, L_index))
    {
        return tbuf_init();
    }
    if (lua_objlen(L, L_index) != sizeof(tbuf))
    {
        return tbuf_init();
    }
    return *((tbuf*)lua_touserdata(L, L_index));
}

/*******************************************************************************
* prvLuaPushTbuf
*
* DESCRIPTION:
*       Push tbuf to Lua stack
*
* INPUTS:
*       L                   - lua state
*       buf                 - tbuf object
*       
* 
* OUTPUTS:
*       None
*
* RETURNS:
*       tbuf        (tbuf.ptr == NULL if error)
*
* COMMENTS:
*
*******************************************************************************/
void prvLuaPushTbuf(
    lua_State       *L,
    tbuf            buf
)
{
    if (buf.ptr == NULL)
    {
        lua_pushnil(L);
    }
    else
    {
        tbuf *ptr;
        ptr = (tbuf*)lua_newuserdata(L, sizeof(tbuf));
        if (ptr != NULL)
        {
            *ptr = buf;
        }
    }
}

/*******************************************************************************
* prvLuaTbufEmpty
*
* DESCRIPTION:
*       Returns true if tbuf.len != NULL
*
* INPUTS:
*       L                   - lua state
*       
* OUTPUTS:
*       None
*
* RETURNS:
*       Lua's boolean
*
* COMMENTS:
*
*******************************************************************************/
static int prvLuaTbufEmpty(lua_State *L)
{
    tbuf buf = prvLuaToTbuf(L, 1);
    lua_pushboolean(L, (buf.len == 0) ? 1 : 0);
    return 1;
}

/*******************************************************************************
* prvLuaPushTbufAsString
*
* DESCRIPTION:
*       Convert tbuf object to Lua string
*
* INPUTS:
*       L                   - lua state
*           buf             - tbuf object
*       
* OUTPUTS:
*       None
*
* RETURNS:
*       String at the top of Lua stack
*
* COMMENTS:
*
*******************************************************************************/
void prvLuaPushTbufAsString(lua_State *L, tbuf buf)
{
    if (buf.ptr != NULL)
    {
        lua_pushlstring(L, buf.ptr, buf.len);
    }
    else
    {
        /* Not sure lua_pushlstring will works with buf.ptr == NULL */
        lua_pushliteral(L, "");
    }
}

/*******************************************************************************
* prvLuaTbufToString
*
* DESCRIPTION:
*       Convert tbuf object to string
*
* INPUTS:
*       L                   - lua state
*           buf             - tbuf object
*       
* OUTPUTS:
*       None
*
* RETURNS:
*       String
*
* COMMENTS:
*
*******************************************************************************/
static int prvLuaTbufToString(lua_State *L)
{
    tbuf buf = prvLuaToTbuf(L, 1);
    prvLuaPushTbufAsString(L, buf);
    return 1;
}

/*******************************************************************************
* prvLuaTbuf_register
*
* DESCRIPTION:
*       register tbuf functions
*
* INPUTS:
*       L                   - lua state
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
int prvLuaTbuf_register(lua_State *L)
{
    lua_register(L, "tbuf_empty", prvLuaTbufEmpty);
    lua_register(L, "tbuf_tostring", prvLuaTbufToString);
    return 0;
}

/*
 * Short reference
 * ===============
 *
 * tbuf_empty(tbuf_object)
 *      Return true if tbuf length is zero
 *
 * tbuf_tostring(tbuf_object)
 *      Convert tbuf object to Lua string (duplicate data)
 *
 */
 
