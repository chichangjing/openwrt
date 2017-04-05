/*******************************************************************************
*              (c), Copyright 2011, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* xmlparser.h
*
* DESCRIPTION:
*       Simple XML parser
*
* DEPENDENCIES:
*
* COMMENTS:
*
* FILE REVISION NUMBER:
*       $Revision: 2 $
**************************************************************************/

#ifndef __xmlparser_h__
#define __xmlparser_h__

#include <generic/cider/tbuf.h>

#ifdef  __cplusplus
extern "C" {
#endif

/********************************************************************
* xml_GetNextTag
*
* Description:
*   (implementation)
*   Scan buffer for the next tag
*
* Inputs:
*   t       - buffer to scan
*
* Returns:
*   The tag
*
* Example:
*   tbuf src = tbuf_init_s(
*       "<first>aaa</first>   <second>bbb<third>xxx</third><fourth>a</fourth></second>  ");
*   tag = xml_GetNextTag(src);
*   // tag == "<first>"
********************************************************************/
tbuf xml_GetNextTag(tbuf t);

/********************************************************************
* xml_GetTagName
*
* Description:
*   (implementation)
*   Return tag name
*
* Inputs:
*   t       - buffer to scan
*
* Returns:
*   The tag
*
* Example:
*   // tag == "<first>"
*   tagname = xml_GetTagName(tag);
*   // tagname == "first"
********************************************************************/
tbuf xml_GetTagName(tbuf tag);

/********************************************************************
* xmlParser_gettag
*
* Description:
*   Iterate source for tags
*
* Inputs:
*   src     - source
*   prev    - previous XML tag (tbuf_init() for the first)
*
* Returns:
*   The tags outline
*
* Example:
*   tbuf src = tbuf_init_s(
*       "<first>aaa</first> <single x=\"10\" />  <second>bbb<third>xxx</third><fourth>a</fourth></second>  ");
*   tbuf tag = tbuf_init();
*
*   tag = xmlParser_gettag(src, tag);
*   // tag == "<first>aaa</first>"
*
*   tag = xmlParser_gettag(src, tag);
*   // tag == "<single x=\"10\" />"
*
*   tag = xmlParser_gettag(src, tag);
*   // tag == "<second>bbb<third>xxx</third><fourth>a</fourth></second>"
*
********************************************************************/
tbuf xmlParser_gettag(tbuf src, tbuf prev);

/********************************************************************
* xmlParser_tagname
*
* Description:
*   Returns tag name
*
* Inputs:
*   outline - the tags outline returned by xmlParser_gettag()
*
* Returns:
*   The tag name
*
* Example:
*   // tag == "<first>aaa</first>"
*   tagname = xmlParser_tagname(tag)
*   // tagname == "first"
********************************************************************/
tbuf xmlParser_tagname(tbuf outline);

/********************************************************************
* xmlParser_tagattrs
*
* Description:
*   Returns tag attributes
*
* Inputs:
*   outline - the tags outline returned by xmlParser_gettag()
*
* Returns:
*   The tag attributes
*
* Example:
*   // tag == "<single a=\"10\" />"
*   tagattrs = xmlParser_tagattr(tag)
*   // tagattrs == "a=\"10\""
********************************************************************/
tbuf xmlParser_tagattrs(tbuf outline);

/********************************************************************
* xmlParser_inline
*
* Description:
*   Returns tag's inline
*
* Inputs:
*   outline - the tags outline returned by xmlParser_gettag()
*
* Returns:
*   The tag inline
*
* Example:
*   // tag == "<second>bbb<third>xxx</third><fourth>a</fourth></second>"
*   taginline = xmlParser_tagname(tag)
*   // taginline == "bbb<third>xxx</third><fourth>a</fourth>"
********************************************************************/
tbuf xmlParser_inline(tbuf outline);

/********************************************************************
* xmlParser_attrs_getnext
*
* Description:
*   Iterate through attributes
*
* Inputs:
*   attrs       - the attrs returned by xmlParser_tagattrs()
*   iterator    - iterator, first should be set to tbuf_init()
*
* Outputs:
*   name
*   value
*
* Returns:
*   iterator
*
* Example:
*   // tag == "<single a=\"10\" yy z=5/>"
*   attrs = xmlParser_tagattrs(tag)
*   iterator = xmlParser_attrs_getnext(attrs, iterator, &name, &value)
*   // name == "a" value == "10"
*   iterator = xmlParser_attrs_getnext(attrs, iterator, &name, &value)
*   // name == "yy" value == ""
*   iterator = xmlParser_attrs_getnext(attrs, iterator, &name, &value)
*   // name == "z" value == "5"
*   iterator = xmlParser_attrs_getnext(attrs, iterator, &name, &value)
*   // iterator.len == 0
********************************************************************/
tbuf xmlParser_attrs_getnext(tbuf attrs, tbuf iterator, tbuf *name, tbuf *value);

/********************************************************************
* xmlParser_get
*
* Description:
*   Returns content by string path
*
* Inputs:
*   src         - the XML text
*   path_s      - the string pointing to tag
*
* Returns:
*   The tag outline
*
* Example:
*   tbuf src = tbuf_init_s(
*       "<first>aaa</first> <single x=\"10\" />  <second>bbb<third>xxx</third><fourth>a</fourth></second>  ");
*   data = xmlParser_get(src, "first");
*   // data == "<first>aaa</first>"
*   data = xmlParser_get(src, "second.third")
*   // data == "<third>xxx</third>"
*   data == xmlParser_get(src, "second")
*   // data == "<second>bbb<third>xxx</third><fourth>a</fourth></second>"
*   xmlParser_get(src, "second.zzz")
*   // xmlParser_defined(data) == 0
********************************************************************/
tbuf xmlParser_get(tbuf src, const char *path_s);

/********************************************************************
* xmlParser_get_attr
*
* Description:
*   Returns tags attribute by value
*
* Inputs:
*   tag         - the XML tag
*   name        - the attribute name
*
* Returns:
*   The tag inline
*
* Example:
*   // tag == "<single x=\"10\" yy z=5/>"
*   x = xmlParser_get_attr(tag, "x")
*   // x == "10"
*   
********************************************************************/
tbuf xmlParser_get_attr(tbuf tag, const char *name);

/********************************************************************
* xmlParser_defined
*
* Description:
*   check xmlParser_get() result
*
* Inputs:
*   data        - the xmlParser_get() result
*
* Returns:
*   Non zero if data is defined
********************************************************************/
int xmlParser_defined(tbuf data);
 
#ifdef  __cplusplus
}
#endif

#ifdef  __cplusplus
class XmlTagAttrs : public Tbuf {
public:
    XmlTagAttrs(Tbuf a) : Tbuf(a) {}
    XmlTagAttrs(tbuf a) : Tbuf(a) {}
    Tbuf getnext(Tbuf i, Tbuf &name, Tbuf &value) {
        return xmlParser_attrs_getnext(*this, i, &name, &value); }
    static bool defined(Tbuf value) { return xmlParser_defined(value) != 0; }
};

class XmlParser : public Tbuf {
public:
    XmlParser(Tbuf a) : Tbuf(a) {}
    XmlParser(tbuf a) : Tbuf(a) {}
    XmlParser(const char *a) : Tbuf(a) {}
    XmlParser() : Tbuf() {}
    
    XmlParser getnext(XmlParser i) { return xmlParser_gettag(*this, i); }
    Tbuf tagname(void) { return xmlParser_tagname(*this); }
    XmlTagAttrs attrs(void) { return xmlParser_tagattrs(*this); }
    XmlParser tag_inline(void) { return xmlParser_inline(*this); }

    XmlParser get(const char *path) { return xmlParser_get(*this, path); }
    Tbuf attr(const char *attrname) { return xmlParser_get_attr(*this, attrname); }

    static bool defined(Tbuf value) { return xmlParser_defined(value) != 0; }
    static bool defined(XmlParser value) { return xmlParser_defined((Tbuf)value) != 0; }
};
#endif

#if undef
/* example */
XmlParser root = 
    "<first>aaa</first> "
    "<first call>bbb</first> "
    "<single x=\"10\" />  "
    "<second>bbb<third>xxx</third><fourth>a</fourth></second>  ";

XmlParser child;
do {
    child = xml.getnext(child);
    if (child.len)
    {
        if (child.tagname == "first")
        {
            printf("first!");
            if (XmlTagAttrs::defined(child.get_attr("call")))
            {
                call_func(child.tag_inline());
            }
        }
    }
} while (child.len);


#endif

#endif /* __xmlparser_h__ */
