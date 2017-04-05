/*******************************************************************************
*              (c), Copyright 2011, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* luaCider.c
*
* DESCRIPTION:
*       Lua <=> C interface xmlparser
*
* DEPENDENCIES:
*
* COMMENTS:
*
* FILE REVISION NUMBER:
*       $Revision: 6 $
**************************************************************************/
#include <lua.h>
#include <generic/cider/xmlparser.h>
#include <generic/cider/luaTbufWrapper.h>
#include <cmdShell/FS/cmdFS.h>

static int cider_fd = -1;
static char *xmldata = NULL;
static int xmllen = 0;
static tbuf cider = {NULL, 0};
static tbuf cider_units = {NULL, 0};


/*******************************************************************************
* prvLuaCider_load
*
* DESCRIPTION:
*       Load cider xml
*
* INPUTS:
*       L                   - lua state
*           filename        - filename
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
static int prvLuaCider_load(lua_State *L)
{
    tbuf tg;
    CMD_FS_STAT_STC st;

    if (!lua_isstring(L, 1))
    {
        lua_pushboolean(L, 0);
        return 1;
    }

    if (cmdFSstat(lua_tostring(L, 1), &st))
    {
        lua_pushboolean(L, 0);
        return 1;
    }

    if (cider_fd >= 0)
    {
        cmdFSmunmap(cider_fd, xmldata);
        cmdFSclose(cider_fd);
        cider_fd = -1;
        xmldata = NULL;
        xmllen = 0;
    }
    cider_fd = cmdFSopen(lua_tostring(L, 1), CMD_FS_O_RDONLY);
    if (cider_fd < 0)
    {
        lua_pushboolean(L, 0);
        return 0;
    }
    xmldata = (char*)cmdFSmmap(cider_fd);
    if (!xmldata)
    {
        cmdFSclose(cider_fd);
        cider_fd = -1;
        lua_pushboolean(L, 0);
        return 1;
    }
    xmllen = st.st_size;

    cider.ptr = xmldata;
    cider.len = xmllen;
    tg = xml_GetNextTag(cider);
    if (tbuf_equal_s(xmlParser_tagname(tg), "?xml"))
        cider = tbuf_after(cider, tg);

    cider_units = xmlParser_get(cider, "SILICON.UNITS.UNIT");

    lua_pushboolean(L, 1);
    return 1;
}

/*******************************************************************************
* prvLuaCider_RootNode
*
* DESCRIPTION:
*       return cider root node
*
* INPUTS:
*       L                   - lua state
*       
* OUTPUTS:
*       None
*
* RETURNS:
*       tbuf
*
* COMMENTS:
*
*******************************************************************************/
static int prvLuaCider_RootNode(lua_State *L)
{
    if (cider_units.ptr)
        prvLuaPushTbuf(L, cider_units);
    else
        lua_pushnil(L);
    return 1;
}

/*******************************************************************************
* add_to_name
*
* DESCRIPTION:
*       convert <swName>, name attribute to object name:
*       remove heading and trailing spaces,
*       remove duplicate spaces,
*       replace spaces with unedrscore, replace xml entities
*
* INPUTS:
*       namelen         - length of already collected part
*       str             - string to convert
*       do_xmlentities  - replace xml entities with correct chars 
*       
* OUTPUTS:
*       namebuf
*
* RETURNS:
*       string length
*
* COMMENTS:
*
*******************************************************************************/
static void convert_name(lua_State *L, tbuf name)
{
    char    namebuf[256];
    int     namelen = 0;
    int     was_spc = 0;

    name = tbuf_strip(name);
    for (;name.len && namelen < sizeof(namebuf); name = tbuf_cutb(name))
    {
        char ch = *name.ptr;
        if (ch == '&')
        {
            if (name.len >= 4 &&
                name.ptr[1] == 'l' && name.ptr[2] == 't' &&
                name.ptr[3] == ';')
            {
                ch = '<';
                name = tbuf_cutb(name);
                name = tbuf_cutb(name);
                name = tbuf_cutb(name);
            } else
            if (name.len >= 4 &&
                name.ptr[1] == 'g' && name.ptr[2] == 't' &&
                name.ptr[3] == ';')
            {
                ch = '>';
                name = tbuf_cutb(name);
                name = tbuf_cutb(name);
                name = tbuf_cutb(name);
            } else
            if (name.len >= 5 &&
                name.ptr[1] == 'a' && name.ptr[2] == 'm' && name.ptr[3] == 'p' &&
                name.ptr[4] == ';')
            {
                ch = '&';
                name = tbuf_cutb(name);
                name = tbuf_cutb(name);
                name = tbuf_cutb(name);
                name = tbuf_cutb(name);
            } else
            if (name.len >= 2 && name.ptr[1] == '#')
            {
                tbuf    num = name;
                int     i;

                for (i = 0; i < num.len; i++)
                {
                    if (num.ptr[i] == ';')
                        break;
                }
                if (i < num.len)
                {
                    num.len = i;
                    num = tbuf_cutb(num);
                    num = tbuf_cutb(num);
                    ch = (char)tbuf_touint(num);
                    name = tbuf_after(name, num);
                }
            }
        }
        if (is_spc(ch))
        {
            was_spc = 1;
        } else {
            if (was_spc && namelen)
                namebuf[namelen++] = '_';
            was_spc = 0;
            namebuf[namelen++] = ch;
        }
    }
    lua_pushlstring(L, namebuf, namelen);
}

/*******************************************************************************
* prvLuaCider_getname
*
* DESCRIPTION:
*       return cider name(swName or name attribute)
*
* INPUTS:
*       L                   - lua state
*           node            - XML node
*       
* OUTPUTS:
*       None
*
* RETURNS:
*       xml node
*
* COMMENTS:
*
*******************************************************************************/
static int prvLuaCider_getname(lua_State *L)
{
    tbuf    node, name;
    

    node = prvLuaToTbuf(L, 1);
    if (node.ptr == NULL)
    {
        lua_pushnil(L);
        return 1; /* nil */
    }

    name = xmlParser_tagname(node);
    if (tbuf_equal_s(name, "UNIT")
            || tbuf_equal_s(name, "MEM")
            || tbuf_equal_s(name, "REGISTER"))
    {
        tbuf    swName = xmlParser_get(xmlParser_inline(node),"swName");
        if (swName.ptr)
        {
            swName = xmlParser_inline(swName);
            convert_name(L, swName);
            return 1;
        }
    }
    name = xmlParser_get_attr(node, "name");
    if (name.ptr)
    {
        convert_name(L, name);
        return 1;
    }

    lua_pushnil(L);
    return 1; /* nil */
}

int prvLuaTbuf_register(lua_State *L);
int prvLuaXmlParser_register(lua_State *L);
int prvCiderPpAccess_register(lua_State *L);
/*******************************************************************************
* prvLuaCider_register
*
* DESCRIPTION:
*       register cider functions
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
int prvLuaCider_register(lua_State *L)
{
    prvLuaTbuf_register(L);
    prvLuaXmlParser_register(L);
    prvCiderPpAccess_register(L);
    lua_register(L, "cider_load", prvLuaCider_load);
    lua_register(L, "cider_RootNode", prvLuaCider_RootNode);
    lua_register(L, "cider_getname", prvLuaCider_getname);
    return 0;
}

/*
 * Short reference
 * ===============
 *
 * cider_load(filename)
 *      Load cider xml
 *
 *      Params:
 *          filename        - filename
 *
 *      Returns:
 *          bool status
 *
 * cider_RootNode() -- unit,subunit, ...
 *      Return cider root node (tbuf object)
 *
 */
