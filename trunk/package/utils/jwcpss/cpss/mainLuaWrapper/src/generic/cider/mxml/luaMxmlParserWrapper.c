/*******************************************************************************
*              (c), Copyright 2011, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* luaMxmlParserWrapper.c
*
* DESCRIPTION:
*       Lua <=> C interface Mxml
*
* DEPENDENCIES:
*
* COMMENTS:
*
* FILE REVISION NUMBER:
*       $Revision: 7 $
**************************************************************************/
#include <extUtils/luaCLI/luaCLIEngine.h>
#include "mxml.h"
#include "mxml_config.h"


/* Forward declarations */
static const char * prvLuaXmlNewLineCallback(mxml_node_t *node,int where);

/* static data */
struct xml_opened_data_t {
    mxml_node_t                 *root;
    char                        *filename;
    struct xml_opened_data_t    *next;
};
static struct xml_opened_data_t *xml_opened_data = NULL;

/*******************************************************************************
* prvLuaXmlTagName
*
* DESCRIPTION:
*       Return XML node tag name
*
* APPLICABLE DEVICES:
*       ALL
*
* NOT APPLICABLE DEVICES:
*       None.
*
* INPUTS:
*       L                   - lua state
*           node            - XML node object(mxml_node_t*)
*       
* OUTPUTS:
*       None
*
* RETURNS:
*       tag name (string)
*       empty string if error
*
* COMMENTS:
*       Example:
*           -- node = mxml node object (<mytag xxx>fgdsf</mytag>)
*           print(xml_tagname(node)) -- mytag
*
*******************************************************************************/
static int prvLuaXmlTagName(lua_State *L)
{
    mxml_node_t *node;
    if (!lua_islightuserdata(L, 1))
    {
        lua_pushliteral(L, "");
        return 1;
    }
    node = (mxml_node_t*)lua_touserdata(L, 1);
    if (node->type == MXML_ELEMENT)
        lua_pushstring(L, node->value.element.name);
    else
        lua_pushliteral(L, "");
    return 1;
}

/*******************************************************************************
* prvLuaXmlTagChilds
*
* DESCRIPTION:
*       return object to iterate XML node childs
*
* APPLICABLE DEVICES:
*       ALL
*
* NOT APPLICABLE DEVICES:
*       None.
*
* INPUTS:
*       L                   - lua state
*           node            - XML node object(mxml_node_t*)
*       
* OUTPUTS:
*       None
*
* RETURNS:
*       object to iterate XML node childs(mxml_node_t*, the same)
*       nil if error
*
* COMMENTS:
*       Example:
*           -- node = xml object ("<second>bbb<third>xxx</third><fourth>a</fourth></second>"
*          xml_childs(node) -- to walk for third, fourth
*
*******************************************************************************/
static int prvLuaXmlTagChilds(lua_State *L)
{
    mxml_node_t *node;
    if (!lua_islightuserdata(L, 1))
    {
        lua_pushnil(L);
        return 1;
    }
    node = (mxml_node_t*)lua_touserdata(L, 1);
    if (node)
        lua_pushlightuserdata(L, node);
    else
        lua_pushnil(L);
    return 1;
}

/*******************************************************************************
* prvLuaXmlGetChild
*
* DESCRIPTION:
*       walk through XML node childs
*
* APPLICABLE DEVICES:
*       ALL
*
* NOT APPLICABLE DEVICES:
*       None.
*
* INPUTS:
*       L                   - lua state
*           childs          - child set (returned by xml_childs(node))
*           iterator        - nil or value returned by previous call
*       
* OUTPUTS:
*       None
*
* RETURNS:
*       XML node object(tag outline), should be used as next iterator
*       nil if no tags left
*
* COMMENTS:
*       Example:
*           -- node = xml object ("<second>bbb<third>xxx</third><fourth>a</fourth></second>"
*           childs = xml_childs(node)
*           iterator = nil
*           repeat
*               iterator = xml_getchild(childs, iterator)
*               if iterator ~= nil then
*                   print(xml_tagname(iterator))
*               end
*           until iterator == nil
*
*******************************************************************************/
static int prvLuaXmlGetChild(lua_State *L)
{
    mxml_node_t *node;
    mxml_node_t *iterator = NULL;
    if (!lua_islightuserdata(L, 1))
    {
        lua_pushnil(L);
        return 1;
    }
    node = (mxml_node_t*)lua_touserdata(L, 1);
    if (lua_islightuserdata(L, 2))
        iterator = (mxml_node_t*)lua_touserdata(L, 2);
    if (iterator == NULL)
        iterator = node->child;
    else
        iterator = iterator->next;
    while (iterator && iterator->type != MXML_ELEMENT)
        iterator = iterator->next;
    if (iterator)
        lua_pushlightuserdata(L, iterator);
    else
        lua_pushnil(L);
    return 1;
}

/*******************************************************************************
* prvLuaXmlTagInlineText
*
* DESCRIPTION:
*       return XML node inline text
*
* APPLICABLE DEVICES:
*       ALL
*
* NOT APPLICABLE DEVICES:
*       None.
*
* INPUTS:
*       L                   - lua state
*           node            - XML node object(tag outline)
*       
* OUTPUTS:
*       None
*
* RETURNS:
*       inline text (string)
*
* COMMENTS:
*       Example:
*           -- node = xml object (<mytag xxx>fgdsf</mytag>)
*           print(xml_inlinetext(node)) -- fgdsf
*
*******************************************************************************/
static int prvLuaXmlTagInlineText(lua_State *L)
{
    mxml_node_t *node;
    mxml_node_t *iterator;
    if (!lua_islightuserdata(L, 1))
    {
        lua_pushnil(L);
        return 1;
    }
    node = (mxml_node_t*)lua_touserdata(L, 1);
    lua_pushliteral(L, "");
    for(iterator = node->child; iterator; iterator = iterator->next)
    {
        if (iterator->type == MXML_TEXT)
        {
            if (lua_objlen(L, -1))
            {
                if (iterator->value.text.whitespace)
                {
                    lua_pushliteral(L, " ");
                    lua_concat(L, 2);
                }
            }
            lua_pushstring(L, iterator->value.text.string);
            lua_concat(L, 2);
        }
        if (iterator->type == MXML_ELEMENT)
        {
            if (_mxml_strncmp(iterator->value.element.name, "![CDATA[",8) == 0)
            {
                int l = _mxml_strlen(iterator->value.element.name);
                if (_mxml_strcmp(iterator->value.element.name+l-2,"]]") == 0)
                {
                    l -= 2;
                }
                lua_pushlstring(L,iterator->value.element.name+8,l-8);
                lua_concat(L, 2);
            }
        }
    }
    return 1;
}

/*******************************************************************************
* prvLuaXmlTagAttr
*
* DESCRIPTION:
*       return XML tag attribute
*
* APPLICABLE DEVICES:
*       ALL
*
* NOT APPLICABLE DEVICES:
*       None.
*
* INPUTS:
*       L                   - lua state
*           node            - XML node object(tag outline)
*           attrname        - attribute name
*       
* OUTPUTS:
*       None
*
* RETURNS:
*       tag attribute (string) or nil
*
* COMMENTS:
*       Example:
*           -- node = tbuf object (<mytag myattr="10">fgdsf</mytag>)
*           print(xml_get_attr(node, "myattr")) -- 10
*
*******************************************************************************/
static int prvLuaXmlTagAttr(lua_State *L)
{
    mxml_node_t *node;
    const char *tagname;
    int i;
    if (!lua_islightuserdata(L, 1))
    {
        lua_pushnil(L);
        return 1;
    }
    node = (mxml_node_t*)lua_touserdata(L, 1);
    if (!lua_isstring(L, 2))
    {
        lua_pushnil(L);
        return 1;
    }
    tagname = lua_tostring(L, 2);
    for (i = 0; i < node->value.element.num_attrs; i++)
    {
        if (!_mxml_strcmp(node->value.element.attrs[i].name,tagname))
        {
            lua_pushstring(L, node->value.element.attrs[i].value);
            return 1;
        }
    }
    lua_pushnil(L);
    return 1;
}

mxml_node_t* prvLuaMxmlLookupNode(mxml_node_t *node, const char *path)
{
    for (node = node->child; node; )
    {
        int l;
        if (node->type != MXML_ELEMENT)
        {
            node = node->next;
            continue;
        }
        l = _mxml_strlen(node->value.element.name);
        if (_mxml_strncmp(path, node->value.element.name, l) != 0)
        {
            node = node->next;
            continue;
        }
        if (path[l] == 0)
        {
            /* found */
            return node;
        }
        if (path[l] == '.')
        {
            /* step matched */
            path += l+1;
            node = node->child;
            continue;
        }
        /* not matched */
        node = node->next;
    }
    return NULL;
}
/*******************************************************************************
* prvLuaXmlLookup
*
* DESCRIPTION:
*       Lookup for XML node by path
*
* APPLICABLE DEVICES:
*       ALL
*
* NOT APPLICABLE DEVICES:
*       None.
*
* INPUTS:
*       L                   - lua state
*           node            - parent XML node(tbuf object)
*           path            - path
*       
* OUTPUTS:
*       None
*
* RETURNS:
*       XML node object (tag outline)
*       nil if not found
*
* COMMENTS:
*       Example:
*           -- xml = tbuf object ("<PARENT><first>aaa</first> <single x=\"10\" /> " ..
*           --              " <second>bbb<third>xxx</third><fourth>a</fourth></second></PARENT>"
*           print(tbuf_tostring(xml_lookup(xml, "second.third"))) -- <third>xxx</third>
*
*******************************************************************************/
static int prvLuaXmlLookup(lua_State *L)
{
    mxml_node_t *node;
    if (!lua_islightuserdata(L, 1))
    {
        lua_pushnil(L);
        return 1;
    }
    node = (mxml_node_t*)lua_touserdata(L, 1);
    if (!lua_isstring(L, 2))
    {
        lua_pushnil(L);
        return 1;
    }
    node = prvLuaMxmlLookupNode(node, lua_tostring(L, 2));
    if (node)
        lua_pushlightuserdata(L, node);
    else
        lua_pushnil(L);
    return 1;
}

/*******************************************************************************
* prvLuaXml_streqx
*
* DESCRIPTION:
*       check if two strings are equal
*
* INPUTS:
*       s1          - first string to compare
*       s2          - second string to compare
*       n           - number of chars to compare
*       ignorecase  - ignore case in comparison flag
*       
* OUTPUTS:
*       None
*
* RETURNS:
*       1 if strings are equal otherwise 0
*
* COMMENTS:
*
*******************************************************************************/
static int prvLuaXml_streqx(const char* s1, const char *s2, int n, int ignorecase)
{
    if (s1 == NULL || s2 == NULL)
        return 1;
    for (;*s1 && *s2 && n; s1++, s2++, n--)
    {
        char c1 = *s1;
        char c2 = *s2;
        if (ignorecase)
        {
            c1 = (c1 >= 'a' && c1 <= 'z') ? c1 - 'a' + 'A' : c1;
            c2 = (c2 >= 'a' && c2 <= 'z') ? c2 - 'a' + 'A' : c2;
        }
        if (c1 != c2)
            return 0;
    }
    if (!n)
        return 1;
    return (*s1 == *s2) ? 1 : 0;
}

/*******************************************************************************
* prvLuaXml_lookup_tag_with_attr_common
*
* DESCRIPTION:
*       Search node by attribute value
*
* INPUTS:
*       L           - lua state
*       parent      - parent node
*       tagname     - 
*       attrname    -
*       value       - Search <tagname attrname="value">
*       ignorecase  - ignore case in value
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
int prvLuaXml_lookup_tag_with_attr_common(
    lua_State *L,
    mxml_node_t *parent,
    const char  *tagname,
    const char  *attrname,
    const char  *value,
    int         ignorecase
)
{
    mxml_node_t* node;
    int i;

    for (node = parent->child; node; )
    {
        if (node->type != MXML_ELEMENT)
        {
            node = node->next;
            continue;
        }
        if (_mxml_strcmp(node->value.element.name, tagname) != 0)
        {
            node = node->next;
            continue;
        }
        for (i = 0; i < node->value.element.num_attrs; i++)
        {
            mxml_attr_t *attr = node->value.element.attrs + i;
            if (_mxml_strcmp(attr->name, attrname) == 0 &&
                prvLuaXml_streqx(attr->value, value, -1, ignorecase))
            {
                lua_pushlightuserdata(L, node);
                return 1;
            }
        }
        node = node->next;
    }
    lua_pushnil(L);
    return 1;
}

/*******************************************************************************
* prvLuaXml_lookup_tag_with_attr
*
* DESCRIPTION:
*       Search node by attribute value
*
* INPUTS:
*       L           - lua state
*           parent      - parent node
*           tagname     - 
*           attrname    -
*           value       - Search <tagname attrname="value">
*           ignorecase  - ignore case in value
*       
* OUTPUTS:
*       None
*
* RETURNS:
*       XML node object (tag outline)
*       nil if not found
*
* COMMENTS:
*
*******************************************************************************/
static int prvLuaXml_lookup_tag_with_attr(lua_State *L)
{
    int ignorecase = 0;
    if (!lua_islightuserdata(L, 1))
    {
        lua_pushnil(L);
        return 1;
    }
    if (!lua_isstring(L, 2))
    {
        lua_pushnil(L);
        return 1;
    }
    if (!lua_isstring(L, 3))
    {
        lua_pushnil(L);
        return 1;
    }
    if (!lua_isstring(L, 4))
    {
        lua_pushnil(L);
        return 1;
    }
    if (lua_isboolean(L, 5))
    {
        ignorecase = lua_toboolean(L, 5);
    }
    return prvLuaXml_lookup_tag_with_attr_common(L,
            (mxml_node_t*)lua_touserdata(L, 1), /* parent */
            lua_tostring(L, 2), /* tagname */
            lua_tostring(L, 3), /* attrname */
            lua_tostring(L, 4), /* value */
            ignorecase);
}


/*******************************************************************************
* prvLuaXml_compl_by_tag_with_attr_common
*
* DESCRIPTION:
*       Complete by attribute value
*
* INPUTS:
*       L           - lua state
*       parent      - parent node
*       tagname     - 
*       attrname    -
*       prefix      -
*       prefix_len  - Search <tagname attrname="prefix*">
*       ignorecase  - ignore case in value
*       
* OUTPUTS:
*       None
*
* RETURNS:
*       two lua tables: compl, help
*
* COMMENTS:
*
*******************************************************************************/
int prvLuaXml_compl_by_tag_with_attr_common(
    lua_State *L,
    mxml_node_t *parent,
    const char  *tagname,
    const char  *attrname,
    const char  *prefix,
    size_t      prefix_len,
    int         ignorecase
)
{
    int     compl_idx;
    mxml_node_t* node;
    int i;

    lua_newtable(L); /* compl */
    compl_idx = lua_gettop(L);
    lua_newtable(L); /* help */

    for (node = parent->child; node; )
    {
        if (node->type != MXML_ELEMENT)
        {
            node = node->next;
            continue;
        }
        if (_mxml_strcmp(node->value.element.name, tagname) != 0)
        {
            node = node->next;
            continue;
        }
        for (i = 0; i < node->value.element.num_attrs; i++)
        {
            mxml_attr_t *attr = node->value.element.attrs + i;
            if (_mxml_strcmp(attr->name, attrname) == 0 &&
                prvLuaXml_streqx(attr->value, prefix, prefix_len, ignorecase))
            {
                lua_pushstring(L, attr->value);
                lua_rawseti(L, compl_idx, (int)lua_objlen(L, compl_idx) + 1);
                /* TODO: handle help */
            }
        }
        node = node->next;
    }
    return 2;
}

/*******************************************************************************
* prvLuaXml_compl_by_tag_with_attr
*
* DESCRIPTION:
*       Complete by attribute value
*
* INPUTS:
*       L           - lua state
*           parent      - parent node
*           tagname     - 
*           attrname    -
*           prefix      - Search <tagname attrname="prefix*">
*           ignorecase  - ignore case in value
*       
* OUTPUTS:
*       None
*
* RETURNS:
*       two lua tables: compl, help
*
* COMMENTS:
*
*******************************************************************************/
static int prvLuaXml_compl_by_tag_with_attr(lua_State *L)
{
    int ignorecase = 0;
    const char *prefix;
    size_t prefix_len;
    if (!lua_islightuserdata(L, 1))
    {
ret_err:
        lua_newtable(L);
        lua_newtable(L);
        return 2;
    }
    if (!lua_isstring(L, 2))
    {
        goto ret_err;
    }
    if (!lua_isstring(L, 3))
    {
        goto ret_err;
    }
    if (!lua_isstring(L, 4))
    {
        goto ret_err;
    }
    prefix = lua_tolstring(L, 4, &prefix_len);
    if (lua_isboolean(L, 5))
    {
        ignorecase = lua_toboolean(L, 5);
    }
    return prvLuaXml_compl_by_tag_with_attr_common(L,
            (mxml_node_t*)lua_touserdata(L, 1), /* parent */
            lua_tostring(L, 2), /* tagname */
            lua_tostring(L, 3), /* attrname */
            prefix, prefix_len,
            ignorecase);
}

/*******************************************************************************
* prvLuaXmlOpen
*
* DESCRIPTION:
*       Open an existing XML file
*
* APPLICABLE DEVICES:
*       ALL
*
* NOT APPLICABLE DEVICES:
*       None.
*
* INPUTS:
*       L               - lua state
*           filename
*       
* OUTPUTS:
*       None
*
* RETURNS:
*       XML root node 
*       nil if file failed to load
*
* COMMENTS:
*       Example:  xml_open("iDebug.xml")
*
*******************************************************************************/
static int prvLuaXmlOpen(lua_State *L)
{
    struct xml_opened_data_t *opened;
    const char *filename;
    GEN_FS_STAT_STC st;
    mxml_node_t *root;
    int fd;
    int reload = 0;
    genFS_PTR fs = (genFS_PTR)luaCLIEngineGetData(L, "_genFS", 0);

    if (!lua_isstring(L, 1) || fs == NULL)
    {
        lua_pushnil(L);
        return 1;
    }
    filename = lua_tostring(L, 1);
    if (lua_isboolean(L, 2) && lua_toboolean(L, 2) == 1)
    {
        reload = 1;
    }
    /* lookup for previosly opened */
    for (opened = xml_opened_data; opened; opened = opened->next)
    {
        if (_mxml_strcmp(opened->filename, filename) == 0)
            break;
    }
    /* was opened before */
    if (opened && reload == 0)
    {
        lua_pushlightuserdata(L, opened->root);
        return 1;
    }
    if (opened && opened->root != NULL)
    {
        mxmlRemove(opened->root);
        opened->root = NULL;
    }
    /* not opened, first check that file exists */
    if (fs->stat(filename, &st))
    {
        lua_pushnil(L);
        return 1;
    }
    /* open file and load XML */
    fd = fs->open(filename, GEN_FS_O_RDONLY);
    if (fd < 0)
    {
        lua_pushnil(L);
        return 1;
    }
    root = mxmlLoadFd(NULL, fd, fs, MXML_NO_CALLBACK);
    fs->close(fd);
    if (!root)
    {
        lua_pushnil(L);
        return 1;
    }
    /* add to chain */
    if (!opened)
    {
        opened = (struct xml_opened_data_t*)_mxml_malloc(sizeof(*opened));
        opened->filename = _mxml_strdup(filename);
        opened->next = xml_opened_data;
        xml_opened_data = opened;
    }
    opened->root = root;

    /* return root node */
    lua_pushlightuserdata(L, root);
    return 1;
}



/*******************************************************************************
* prvLuaXmlNewXML
*
* DESCRIPTION:
*       Create a new XML root node
*
* APPLICABLE DEVICES:
*       ALL
*
* NOT APPLICABLE DEVICES:
*       None.
*
* INPUTS:
*       L               - lua state
*       version         - XML version 
*       
* OUTPUTS:
*       None
*
* RETURNS:
*       XML root node 
*       nil if the node could not be created
*
* COMMENTS:
*       The version must be a number or a string, otherwise the default
*       value "1.0" is used.
*       Example:  xml_newXml(), xml_newXml("1.0") or xml_newXml(nil)
*
*******************************************************************************/
static int prvLuaXmlNewXML(lua_State *L)
{
    mxml_node_t *node;

    node = mxmlNewXML(lua_tostring(L,1));
    if (node)
        lua_pushlightuserdata(L, node);
    else
        lua_pushnil(L);
    return 1;
}

/*******************************************************************************
* prvLuaXmlNewElement
*
* DESCRIPTION:
*       Create a new XML node and connects it to a parent node
*
* APPLICABLE DEVICES:
*       ALL
*
* NOT APPLICABLE DEVICES:
*       None.
*
* INPUTS:
*       L              - lua state
*       parent         - a mxml_node_t* struct defining the parent node
*       node name      - The name of the node
*       node text      - (Optional) The the node's text
*       
* OUTPUTS:
*       None
*
* RETURNS:
*       XML node 
*       nil if the node could not be created
*
* COMMENTS:
*       Example:
*           xml_newElement(xml_newXml(),"node name")
*
*******************************************************************************/
static int prvLuaXmlNewElement(lua_State *L)
{
    mxml_node_t *node;
    if (!lua_islightuserdata(L, 1) || !lua_isstring(L,2))
    {
        lua_pushnil(L);
        return 1;
    }

    node = mxmlNewElement((mxml_node_t *)lua_touserdata(L,1), lua_tostring(L,2));

    if (!node) 
    {
        lua_pushnil(L);
    }
    else
    {
        if (lua_isstring(L,3)) 
        {
             mxmlNewText(node, 0, lua_tostring(L,3));
        }
    }
    lua_pushlightuserdata(L, node);

    return 1;
}

/*******************************************************************************
* prvLuaXmlSaveXmlFile
*
* DESCRIPTION:
*       Save an xml structure to a file
*
* APPLICABLE DEVICES:
*       ALL
*
* NOT APPLICABLE DEVICES:
*       None.
*
* INPUTS:
*       L              - lua state
*       root node      - a mxml_node_t* struct defining the root node
*       filename       - The name of the file
*       
* OUTPUTS:
*       None
*
* RETURNS:
*       0   - if successful 
*       nil - else
*
* COMMENTS:
*       Example:
*           xml_saveFile(xml_newXml(),"file.xml")
*
*******************************************************************************/
static int prvLuaXmlSaveXmlFile(lua_State *L)
{
    int fp;
    genFS_PTR fs = (genFS_PTR)luaCLIEngineGetData(L, "_genFS", 0);

    if (!lua_islightuserdata(L, 1) || !lua_isstring(L, 2) || fs == NULL)
    {
        lua_pushnil(L);
        return 1;
    }

    fp = fs->open(lua_tostring(L,2) ,GEN_FS_O_RDWR | GEN_FS_O_CREAT);   
    mxmlSaveFd((mxml_node_t *)lua_touserdata(L,1), fp, prvLuaXmlNewLineCallback);
    fs->close(fp); 

    lua_pushinteger(L,0);
    return 1;
}

/*******************************************************************************
* prvLuaXmlAddAttrib
*
* DESCRIPTION:
*       Add an attribute to a node
*
* APPLICABLE DEVICES:
*       ALL
*
* NOT APPLICABLE DEVICES:
*       None.
*
* INPUTS:
*       L         - lua state
*       node      - a mxml_node_t* struct defining the node
*       key       - The key of the attribute
*       value     - The value of the attribute
*       
* OUTPUTS:
*       None
*
* RETURNS:
*       0   - if successful 
*       nil - else
*
* COMMENTS:
*       Example:
*           xml_addAttrib(node,"DevID","0")		
*
*******************************************************************************/
static int prvLuaXmlAddAttrib(lua_State *L)
{
    if (!lua_islightuserdata(L, 1) || !lua_isstring(L, 2) || !lua_isstring(L, 3))
    {
        lua_pushnil(L);
        return 1;
    }
    
    mxmlElementSetAttr(lua_touserdata(L, 1),lua_tostring(L,2),lua_tostring(L,3));
    lua_pushinteger(L,0);
    return 1;
}

/*******************************************************************************
* prvLuaXmlDelete
*
* DESCRIPTION:
*       Delete an xml node and all of its children dettaching it from its parents
*
* APPLICABLE DEVICES:
*       ALL
*
* NOT APPLICABLE DEVICES:
*       None.
*
* INPUTS:
*       L              - lua state
*       xml node      - a mxml_node_t* struct to be deleted
*       
* OUTPUTS:
*       None
*
* RETURNS:
*       0   - if successful 
*       nil - else
*
* COMMENTS:
*       Example:
*           xml_delete(xml_newXml())
*
*******************************************************************************/
static int prvLuaXmlDelete(lua_State *L)
{
    if (!lua_islightuserdata(L, 1))
    {
        lua_pushnil(L);
        return 1;
    }
    
    mxmlDelete((mxml_node_t *)lua_touserdata(L, 1));
    lua_pushinteger(L,0);
    return 1;
}

/*******************************************************************************
* prvLuaXmlNewLineCallback
*
* DESCRIPTION:
*       XML save file callback, adds newline before and after elements
*
* APPLICABLE DEVICES:
*       ALL
*
* NOT APPLICABLE DEVICES:
*       None.
*
* INPUTS:
*       node     - The element which is saved
*       where    - The place where the file saving is at 
*                  (Before/After element, etc...)
*       
* OUTPUTS:
*       None
*
* RETURNS:
*       "\n"- if it is called before close/open
*       NULL - else
*
* COMMENTS:
*       The callback function is used as the third parameter of the
*       mxmlSaveFd() function.
*
*******************************************************************************/
static const char * prvLuaXmlNewLineCallback
(
    mxml_node_t *node,
    int where
)
{
	if (((where == MXML_WS_BEFORE_OPEN) &&(node->parent!=NULL)) ||
            (where == MXML_WS_BEFORE_CLOSE))
        return ("\n");

    return (NULL);
}
/*******************************************************************************
* prvLuaXmlParser_register
*
* DESCRIPTION:
*       register xmlparser functions
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
int prvLuaMxmlParser_register(lua_State *L)
{
    lua_register(L, "xml_tagname", prvLuaXmlTagName);
    lua_register(L, "xml_childs", prvLuaXmlTagChilds);
    lua_register(L, "xml_getchild", prvLuaXmlGetChild);
    lua_register(L, "xml_inlinetext", prvLuaXmlTagInlineText);
    lua_register(L, "xml_get_attr", prvLuaXmlTagAttr);
    lua_register(L, "xml_lookup", prvLuaXmlLookup);
    lua_register(L, "xml_lookup_by_attr", prvLuaXml_lookup_tag_with_attr);
    lua_register(L, "xml_compl_by_attr", prvLuaXml_compl_by_tag_with_attr);

    lua_register(L, "xml_open", prvLuaXmlOpen);
    lua_register(L, "xml_newXml", prvLuaXmlNewXML);
    lua_register(L, "xml_newElement", prvLuaXmlNewElement);
    lua_register(L, "xml_saveFile", prvLuaXmlSaveXmlFile);
    lua_register(L, "xml_addAttrib", prvLuaXmlAddAttrib);
    lua_register(L, "xml_delete", prvLuaXmlDelete);

    
    return 0;
}

/*
 * Short reference
 * ===============
 *
 * xml_tagname(node)
 *      Return XML node tag name
 *
 *      Params:
 *          node            - XML node object
 *
 *      Returns:
 *          tag name (string)
 *          empty string if error
 *
 *      Example:
 *          -- node = xml object (<mytag xxx>fgdsf</mytag>)
 *          print(xml_tagname(node)) -- mytag
 *
 *
 * xml_childs(node)
 *      Return object to iterate XML node childs
 *
 *      Params:
 *          node            - XML node object
 *          
 *      Returns:
 *          childs          - the object for child scan
 *          nil if error
 *
 *      Example:
 *          -- node = xml object ("<second>bbb<third>xxx</third><fourth>a</fourth></second>"
 *          xml_childs(node) -- to walk for third, fourth
 *
 *
 * xml_getchild(childs, iterator)
 *      Walk through childs in xml node
 *
 *      Params:
 *          childs          - object to walk in (returned by xml_childs)
 *          iterator        - nil or previous tag
 *
 *      Returns:
 *          XML node object, should be used as next iterator
 *          nil if no tags left
 *
 *      Example:
 *          -- node = xml object ("<second>bbb<third>xxx</third><fourth>a</fourth></second>"
 *          childs = xml_childs(node)
 *          iterator = nil
 *          repeat
 *              iterator = xml_getchild(childs, iterator)
 *              if iterator ~= nil then
 *                  print(xml_tagname(iterator))
 *              end
 *          until iterator == nil
 *
 *
 * xml_inlinetext(node)
 *      Return XML node inline text
 *
 *      Params:
 *          node            - xml none object
 *
 *      Returns:
 *          inline text (string)
 *
 *      Example:
 *          -- node = xml object (<mytag xxx>fgdsf</mytag>)
 *          print(xml_inlinetext(node)) -- fgdsf
 *
 *
 * xml_get_attr(node,attrname)
 *      return XML tag attribute
 *
 *      Params:
 *          node            - XML node object
 *          attrname        - attribute name (string)
 *
 *      Returns:
 *          tag attribute (string) or nil
 *
 *      Example:
 *          -- node = xml object (<mytag myattr="10">fgdsf</mytag>)
 *          print(xml_get_attr(node, "myattr")) -- 10
 *
 *
 * xml_lookup(node,pathattrname)
 *      Lookup for XML node by path
 *
 *      Params:
 *          node            - parent XML node
 *          path            - path
 *
 *      Returns:
 *          XML node object
 *          nil if not found
 *
 *      Example:
 *          -- parent = tbuf object ("<PARENT><first>aaa</first> <single x=\"10\" /> " ..
 *          --              " <second>bbb<third>xxx</third><fourth>a</fourth></second></PARENT>"
 *          print(tbuf_tostring(xml_lookup(parent, "second.third"))) -- <third>xxx</third>
 *
 *
 * xml_lookup_by_attr(parent, tagname, attrname, value [, ignorecase])
 *      Search node by attribute value
 *
 *      Params:
 *          parent      - parent node
 *          tagname     - 
 *          attrname    -
 *          value       - Search <tagname attrname="value">
 *          ignorecase  - ignore case in value
 *
 *      Returns:
 *          XML node object
 *          nil if not found
 *
 *
 * xml_compl_by_attr(parent, tagname, attrname, prefix [, ignorecase])
 *      Search node by attribute value
 *
 *      Params:
 *          parent      - parent node
 *          tagname     - 
 *          attrname    -
 *          prefix       - Search <tagname attrname="prefix*">
 *          ignorecase  - ignore case in value
 *
 *      Returns:
 *          two lua tables: compl, help
 *
 *
 * xml_open(filename [, forcereload])
 *      Open an existing XML file
 *
 *      Params:
 *          filename
 *          forcereload - reload xml
 *
 *      Returns:
 *          XML node object
 *          nil if not found
 *
 *      Example:
 *          iDebug = xml_open("iDebug.xml")
 *
 */
 
