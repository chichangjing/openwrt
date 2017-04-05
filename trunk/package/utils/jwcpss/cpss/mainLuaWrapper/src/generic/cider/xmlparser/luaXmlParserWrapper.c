/*******************************************************************************
*              (c), Copyright 2011, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* luaXmlParserWrapper.c
*
* DESCRIPTION:
*       Lua <=> C interface xmlparser
*
* DEPENDENCIES:
*
* COMMENTS:
*
* FILE REVISION NUMBER:
*       $Revision: 3 $
**************************************************************************/
#include <lua.h>
#include <generic/cider/xmlparser.h>
#include <generic/cider/luaTbufWrapper.h>

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
*           node            - XML node object(tbuf outline)
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
*           -- node = tbuf object (<mytag xxx>fgdsf</mytag>)
*           print(xml_tagname(node)) -- mytag
*
*******************************************************************************/
static int prvLuaXmlTagName(lua_State *L)
{
    tbuf tag = prvLuaToTbuf(L, 1);
    prvLuaPushTbufAsString(L, xmlParser_tagname(tag));
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
*           node            - XML node object(tag outline)
*       
* OUTPUTS:
*       None
*
* RETURNS:
*       object to iterate XML node childs(tag inline)
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
    tbuf tag = prvLuaToTbuf(L, 1);
    tag = xmlParser_inline(tag);
    if (tag.len == 0)
        lua_pushnil(L);
    else
        prvLuaPushTbuf(L, tag);
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
    tbuf xml;
    tbuf iterator;
    xml = prvLuaToTbuf(L, 1);
    if (xml.len == 0)
    {
        lua_pushnil(L);
        return 1;
    }
    iterator = prvLuaToTbuf(L, 2);
    iterator = xmlParser_gettag(xml, iterator);
    if (iterator.len != 0)
        prvLuaPushTbuf(L, iterator);
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
    tbuf tag = prvLuaToTbuf(L, 1);
    prvLuaPushTbufAsString(L, xmlParser_inline(tag));
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
    tbuf tag = prvLuaToTbuf(L, 1);
    tbuf attr;
    if (!lua_isstring(L, 2))
    {
        lua_pushnil(L);
        return 1;
    }
    attr = xmlParser_get_attr(tag, lua_tostring(L, 2));
    if (attr.ptr != NULL)
    {
        prvLuaPushTbufAsString(L, attr);
    }
    else
    {
        lua_pushnil(L);
    }
    return 1;
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
    tbuf xml, tag;

    xml = prvLuaToTbuf(L, 1);
    xml = xmlParser_inline(xml);
    if (!lua_isstring(L, 2))
    {
        lua_pushnil(L);
        return 1;
    }
    tag = xmlParser_get(xml, lua_tostring(L, 2));
    if (tag.len)
        prvLuaPushTbuf(L, tag);
    else
        lua_pushnil(L);
    return 1;
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
int prvLuaXmlParser_register(lua_State *L)
{
    lua_register(L, "xml_tagname", prvLuaXmlTagName);
    lua_register(L, "xml_childs", prvLuaXmlTagChilds);
    lua_register(L, "xml_getchild", prvLuaXmlGetChild);
    lua_register(L, "xml_inlinetext", prvLuaXmlTagInlineText);
    lua_register(L, "xml_get_attr", prvLuaXmlTagAttr);
    lua_register(L, "xml_lookup", prvLuaXmlLookup);
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
 */
 
