/*******************************************************************************
*              (c), Copyright 2011, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* luaCider_mxml.c
*
* DESCRIPTION:
*       Lua <=> C interface Mxml
*
* DEPENDENCIES:
*
* COMMENTS:
*
* FILE REVISION NUMBER:
*       $Revision: 6 $
**************************************************************************/
#include <extUtils/luaCLI/luaCLIEngine.h>
#include "mxml.h"
#include "mxml_config.h"

static mxml_node_t *cider_root = NULL;
static mxml_node_t *cider_units = NULL;

mxml_node_t* prvLuaMxmlLookupNode(mxml_node_t *node, const char *path);
void luaCider_reset_mxml(void);
/*******************************************************************************
* prvLuaCider_load_mxml
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
static int prvLuaCider_load_mxml(lua_State *L)
{
    GEN_FS_STAT_STC st;
    int fd;
    genFS_PTR fs = (genFS_PTR)luaCLIEngineGetData(L, "_genFS", 0);

    if (!lua_isstring(L, 1) || fs == NULL)
    {
        lua_pushboolean(L, 0);
        return 1;
    }

    if (fs->stat(lua_tostring(L, 1), &st))
    {
        lua_pushboolean(L, 0);
        return 1;
    }

    
    fd = fs->open(lua_tostring(L, 1), GEN_FS_O_RDONLY);
    if (fd < 0)
    {
        lua_pushboolean(L, 0);
        return 0;
    }


    luaCider_reset_mxml();

    cider_root = mxmlLoadFd(NULL, fd, fs, MXML_NO_CALLBACK);
    fs->close(fd);

    if (!cider_root)
    {
        lua_pushboolean(L, 0);
        return 1;
    }

    cider_units = prvLuaMxmlLookupNode(cider_root, "SILICON.UNITS.UNIT");

    lua_pushboolean(L, 1);
    return 1;
}

/*******************************************************************************
* luaCider_reset_mxml
*
* DESCRIPTION:
*       Reset mxml state (used with cliMemLib reset)
*
* INPUTS:
*       None
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
void luaCider_reset_mxml(void)
{
    if (cider_root != NULL)
    {
        mxmlDelete(cider_root);     
    }
    cider_root = NULL;
    cider_units = NULL;
}


/*******************************************************************************
* luaCider_is_mxml
*
* DESCRIPTION:
*       checks if xml is already loaded*
* INPUTS:
*       None
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
static int luaCider_is_mxml(lua_State *L)
{
    if (cider_root != NULL)
    {
        lua_pushboolean(L, 1);
        return 1;
    }

    lua_pushboolean(L, 0);
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
*       xml node
*
* COMMENTS:
*
*******************************************************************************/
static int prvLuaCider_RootNode(lua_State *L)
{
    if (cider_units)
        lua_pushlightuserdata(L, cider_units);
    else
        lua_pushnil(L);
    return 1;
}

/*******************************************************************************
* is_spc
*
* DESCRIPTION:
*       Return true if char is space (TAB,CR,LF, etc)
*
* INPUTS:
*       ch      - char to test
*       
* OUTPUTS:
*       None
*
* RETURNS:
*       1 if ch is space
*
* COMMENTS:
*
*******************************************************************************/
static int is_spc(char ch)
{
    if (ch == ' ' || ch == '\t' || ch == '\r' || ch == '\n')
        return 1;
    return 0;
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
static int add_to_name(char *namebuf, int namelen, const char *str, int do_xmlentities)
{
    int was_spc = 0;

    while (is_spc(*str))
        str++;
    if (namelen)
        was_spc = 1;
    for (;*str && namelen < 256; str++)
    {
        char ch = *str;
        if (ch == '&' && do_xmlentities)
        {
            if (_mxml_strncmp(str, "&lt;", 4) == 0)
            {
                ch = '<';
                str += 3;
            } else
            if (_mxml_strncmp(str, "&gt;", 4) == 0)
            {
                ch = '<';
                str += 3;
            } else
            if (_mxml_strncmp(str, "&amp;", 5) == 0)
            {
                ch = '&';
                str += 4;
            } else
            if (_mxml_strncmp(str, "&#x", 3) == 0)
            {
                char *e;
                long n = _mxml_strtol((char*)(GT_UINTPTR)(str + 3), &e, 16);
                if (*e == ';')
                {
                    str = e;
                    ch = (char)n;
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
    return namelen;
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
static int prvLuaCider_getname_mxml(lua_State *L)
{
    mxml_node_t *node;
    char        namebuf[256];
    int         namelen = 0;
    int         i;
    
    if (!lua_islightuserdata(L, 1))
    {
        lua_pushnil(L);
        return 1; /* nil */
    }
    node = (mxml_node_t*)lua_touserdata(L, 1);
    if (node->type != MXML_ELEMENT)
    {
        lua_pushnil(L);
        return 1; /* nil */
    }

    if (_mxml_strcmp(node->value.element.name, "UNIT") == 0
            || _mxml_strcmp(node->value.element.name, "MEM") == 0
            || _mxml_strcmp(node->value.element.name, "REGISTER") == 0)
    {
        mxml_node_t *n;
        for (n = node->child; n; n = n->next)
        {
            if (n->type != MXML_ELEMENT)
                continue;
            if (_mxml_strcmp(n->value.element.name, "swName") != 0)
                continue;
            /* found */
            for (n = n->child; n; n = n->next)
            {
                if (n->type == MXML_TEXT)
                {
                    namelen = add_to_name(namebuf, namelen, n->value.text.string, 0);
                }
            }
            lua_pushlstring(L, namebuf, namelen);
            return 1;
        }
    }

    for (i = 0; i < node->value.element.num_attrs; i++)
    {
        if (!_mxml_strcmp(node->value.element.attrs[i].name, "name"))
        {
            namelen = add_to_name(namebuf, namelen, node->value.element.attrs[i].value, 1);
            lua_pushlstring(L, namebuf, namelen);
            return 1;
        }
    }
    lua_pushnil(L);
    return 1; /* nil */
}

int prvLuaMxmlParser_register(lua_State *L);
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
    prvLuaMxmlParser_register(L);
    prvCiderPpAccess_register(L);
    lua_register(L, "cider_load", prvLuaCider_load_mxml);
    lua_register(L, "cider_RootNode", prvLuaCider_RootNode);
    lua_register(L, "cider_getname", prvLuaCider_getname_mxml);
    lua_register(L, "cider_is_loaded", luaCider_is_mxml);
    
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
 *      Return cider root node (XML node)
 * 
 */
