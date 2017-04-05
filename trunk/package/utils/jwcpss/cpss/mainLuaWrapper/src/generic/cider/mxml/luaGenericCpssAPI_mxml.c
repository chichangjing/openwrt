/*******************************************************************************
*              (c), Copyright 2011, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* luaGenericCpssAPI_mxml.c
*
* DESCRIPTION:
*       Lua <=> C interface for CPSS API function descriptions
*
* DEPENDENCIES:
*
* COMMENTS:
*
* FILE REVISION NUMBER:
*       $Revision: 4 $
**************************************************************************/
#include <extUtils/luaCLI/luaCLIEngine.h>
#include "mxml.h"
#include "mxml_config.h"

static mxml_node_t *cpssapi_root = NULL;
static mxml_node_t *cpssapi_functions = NULL;
static mxml_node_t *cpssapi_structures = NULL;

mxml_node_t* prvLuaMxmlLookupNode(mxml_node_t *node, const char *path);
int prvLuaXml_lookup_tag_with_attr_common(
    lua_State *L,
    mxml_node_t *parent,
    const char  *tagname,
    const char  *attrname,
    const char  *value,
    int         ignorecase
);
int prvLuaXml_compl_by_tag_with_attr_common(
    lua_State *L,
    mxml_node_t *parent,
    const char  *tagname,
    const char  *attrname,
    const char  *prefix,
    size_t      prefix_len,
    int         ignorecase
);

/*******************************************************************************
* prvLuaCpssAPI_load_mxml
*
* DESCRIPTION:
*       Load cpss API xml
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
static int prvLuaCpssAPI_load_mxml(lua_State *L)
{
    GEN_FS_STAT_STC st;
    int fd;
    genFS_PTR fs = (genFS_PTR)luaCLIEngineGetData(L, "_genFS", 0);

    if (cpssapi_root != NULL)
    {
        lua_pushboolean(L, 1);
        return 1;
    }

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

    if (cpssapi_root != NULL)
    {
        mxmlDelete(cpssapi_root);
        cpssapi_root = NULL;
        cpssapi_functions = NULL;
        cpssapi_structures = NULL;
    }
    fd = fs->open(lua_tostring(L, 1), GEN_FS_O_RDONLY);
    if (fd < 0)
    {
        lua_pushboolean(L, 0);
        return 1;
    }
    cpssapi_root = mxmlLoadFd(NULL, fd, fs, MXML_NO_CALLBACK);
    fs->close(fd);

    if (!cpssapi_root)
    {
        lua_pushboolean(L, 0);
        return 1;
    }

    cpssapi_functions = prvLuaMxmlLookupNode(cpssapi_root, "CpssFunctionPrototypes.Functions");
    cpssapi_structures = prvLuaMxmlLookupNode(cpssapi_root, "CpssFunctionPrototypes.Structures");

    lua_pushboolean(L, 1);
    return 1;
}

/*******************************************************************************
* luaCpssAPI_reset_mxml
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
void luaCpssAPI_reset_mxml(void)
{
    cpssapi_root = NULL;
    cpssapi_functions = NULL;
    cpssapi_structures = NULL;
}

/*******************************************************************************
* prvLuaCpssAPI_GetList
*
* DESCRIPTION:
*       return cpss API root node
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
static int prvLuaCpssAPI_GetList(lua_State *L)
{
    if (cpssapi_functions)
        lua_pushlightuserdata(L, cpssapi_functions);
    else
        lua_pushnil(L);
    return 1;
}

/*******************************************************************************
* prvLuaCpssAPI_lookup
*
* DESCRIPTION:
*       return cpss API function node node
*
* INPUTS:
*       L                   - lua state
*           funcname        - function name
*           ignorecase      - ignore case in function name (optional)
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
static int prvLuaCpssAPI_lookup(lua_State *L)
{
    int ignorecase = 0;
    if (!cpssapi_functions)
    {
        lua_pushnil(L);
        return 1;
    }
    if (!lua_isstring(L, 1))
    {
        lua_pushnil(L);
        return 1;
    }
    if (lua_isboolean(L, 2))
    {
        ignorecase = lua_toboolean(L, 2);
    }
    return prvLuaXml_lookup_tag_with_attr_common(L,
            cpssapi_functions,
            "Function", "name", lua_tostring(L, 1), ignorecase);
}

/*******************************************************************************
* prvLuaCpssAPI_list
*
* DESCRIPTION:
*       List all functions which match prefix
*
* INPUTS:
*       L                   - lua state
*           prefix          - prefix or empty string
*           ignorecase      - ignore case in function name (optional)
*       
* OUTPUTS:
*       None
*
* RETURNS:
*       compl, help
*
* COMMENTS:
*
*******************************************************************************/
static int prvLuaCpssAPI_list(lua_State *L)
{
    const char *prefix;
    size_t prefix_len;
    int ignorecase = 0;
    if (lua_isboolean(L, 2))
    {
        ignorecase = lua_toboolean(L, 2);
    }
    if (!cpssapi_functions)
    {
        return 2;
    }
    if (!lua_isstring(L, 1))
    {
        return 2;
    }
    prefix = lua_tolstring(L, 1, &prefix_len);
    return prvLuaXml_compl_by_tag_with_attr_common(L,
            cpssapi_functions,
            "Function", "name", 
            prefix, prefix_len,
            ignorecase);
}

/*******************************************************************************
* prvLuaCpssAPI_structures
*
* DESCRIPTION:
*       return cpss API structures root node
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
static int prvLuaCpssAPI_structures(lua_State *L)
{
    if (cpssapi_structures)
        lua_pushlightuserdata(L, cpssapi_structures);
    else
        lua_pushnil(L);
    return 1;
}

/*******************************************************************************
* prvLuaCpssAPI_lookup_struct
*
* DESCRIPTION:
*       return cpss API structure node
*
* INPUTS:
*       L                   - lua state
*           structname        - function name
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
static int prvLuaCpssAPI_lookup_struct(lua_State *L)
{
    if (!cpssapi_structures)
    {
        lua_pushnil(L);
        return 1;
    }
    if (!lua_isstring(L, 1))
    {
        lua_pushnil(L);
        return 1;
    }
    return prvLuaXml_lookup_tag_with_attr_common(L,
            cpssapi_structures,
            "Structure", "name", lua_tostring(L, 1), 0);
}


int prvLuaMxmlParser_register(lua_State *L);
/*******************************************************************************
* prvLuaCpssAPI_register
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
int prvLuaCpssAPI_register(lua_State *L)
{
    prvLuaMxmlParser_register(L);
    lua_register(L, "cpssAPI_load", prvLuaCpssAPI_load_mxml);
    lua_register(L, "cpssAPI_getlist", prvLuaCpssAPI_GetList);
    lua_register(L, "cpssAPI_lookup", prvLuaCpssAPI_lookup);
    lua_register(L, "cpssAPI_list", prvLuaCpssAPI_list);
    lua_register(L, "cpssAPI_structures", prvLuaCpssAPI_structures);
    lua_register(L, "cpssAPI_getstruct", prvLuaCpssAPI_lookup_struct);
    return 0;
}

/*
 * Short reference
 * ===============
 *
 * cpssAPI_load(filename)
 *      Load CPSS API descriptions xml
 *
 *      Params:
 *          filename        - filename
 *
 *      Returns:
 *          bool status
 *
 *
 * cpssAPI_getlist() -- unit,subunit, ...
 *      Return CPSS API root node (XML node)
 *
 *
 * cpssAPI_lookup(funcname)
 *      Search for function description
 *
 *      Params:
 *          funcname        - function name
 *          ignorecase      - ignore case in function name (optional)
 *
 *      Returns:
 *          XML node
 * 
 *
 * cpssAPI_list(prefix)
 *      List all functions which match prefix
 *
 *      Params:
 *          prefix          - prefix or empty string
 *          ignorecase      - ignore case in function name (optional)
 *
 *      Returns:
 *          compl, help
 * 
 *
 * cpssAPI_structures() -- 
 *      Return CPSS API structures root node (XML node)
 *
 *
 * cpssAPI_getstruct(structname)
 *      Search for structure description
 *
 *      Params:
 *          structname       - structure name
 *
 *      Returns:
 *          XML node
 * 
 */
