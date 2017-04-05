/*******************************************************************************
*              (c), Copyright 2011, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* xmlparser.c
*
* DESCRIPTION:
*       Simple XML parser
*
* DEPENDENCIES:
*
* COMMENTS:
*
* FILE REVISION NUMBER:
*       $Revision: 4 $
**************************************************************************/

#include <string.h>
#include <generic/cider/xmlparser.h>


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
tbuf xml_GetNextTag(tbuf t)
{
    int k;
    int st = 0;
    tbuf p = t;
    while (p.len && p.ptr[0] != '<')
        p = tbuf_cutb(p);
    if (p.len == 0)
        return p;
    for (k = 0; k < p.len; k++)
		switch (st) {
			case 0:
				if (p.ptr[k] == '>') {
					p.len = k+1;
					return p;
				}
				if (p.ptr[k] == '"')
					st = 1;
				break;
			case 1:
				if (p.ptr[k] == '"')
					st = 0;
				if (p.ptr[k] == '\\')
					st = 2;
				break;
			case 2:
				st = 1;
				break;
		}
    p.ptr += p.len;
    p.len = 0;
    return p;
}
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
tbuf xml_GetTagName(tbuf tag)
{
    int k;
    if (tag.len < 2 || tag.ptr[0] != '<' || tag.ptr[tag.len-1] != '>')
    {
        tag.len = 0;
        return tag;
    }
    tag.ptr++;
    tag.len-=2;
    if (tag.len && tag.ptr[tag.len-1] == '/')
        tag.len--;
    for (k = 0; k < tag.len; k++)
        if (is_spc(tag.ptr[k]))
        {
            tag.len = k;
            break;
        }
    return tag;
}

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
tbuf xmlParser_gettag(tbuf src, tbuf prev)
{
    tbuf tg, tagname;
    tbuf outline;
    if (prev.ptr)
        src = tbuf_after(src, prev);
    src = tbuf_strip_head(src);
    if (!src.len)
        return src;
    tg = xml_GetNextTag(src);
    if (!tg.len) {
        src.ptr += src.len;
        src.len = 0;
        return src;
    }
    if (tg.len > 2 && tg.ptr[tg.len-2] == '/')
        return tg; /* <tag /> */
    
    /* Parse tag name */;
    tagname = xml_GetTagName(tg);

    outline = tbuf_starting_from(src, tg);
    src = tbuf_after(src,tg);
    
    while (src.len) {
        tbuf ntg, ntagname;
        ntg = xml_GetNextTag(src);
        if (ntg.len == 0)
            break;
        ntagname = xml_GetTagName(ntg);
        if (ntagname.len && ntagname.ptr[0] == '/' &&
                tbuf_equal(tbuf_cutb(ntagname), tagname))
        {
            outline.len = (int)(ntg.ptr - outline.ptr) +ntg.len;
            return outline;
        }
        ntg = xmlParser_gettag(src,tbuf_init());
        src =tbuf_after(src,ntg);
    }
    return outline;
}

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
tbuf xmlParser_tagname(tbuf outline)
{
    return xml_GetTagName(xml_GetNextTag(outline));
}

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
*   tagattrs = xmlParser_tagattrs(tag)
*   // tagattrs == "a=\"10\""
********************************************************************/
tbuf xmlParser_tagattrs(tbuf outline)
{
    tbuf tagname;
    tbuf tag = xml_GetNextTag(outline);
    if (tag.len < 2)
    {
        tag.len = 0;
        return tag;
    }
    tagname = xml_GetTagName(tag);
    tag.len--; /* remove trailing > */
	/* remove engine slash or '?' in <?xml ... ?> */
    if (tag.len && tag.ptr[tag.len-1] == '/') /* remove trailing / */
        tag.len--;
    if (tagname.len && tagname.ptr[0] == '?' && tag.len && tag.ptr[tag.len-1] == '?')
        tag.len--;
    /* now return attributes */
    return tbuf_strip(tbuf_after(tag, tagname));
}

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
*   // tag == "<single a=\"10\" />"
*   tagattrs = xmlParser_tagattrs(tag)
*   // tagattrs == "a=\"10\""
********************************************************************/
tbuf xmlParser_attrs_getnext(tbuf attrs, tbuf iterator, tbuf *name, tbuf *value)
{
    int k;
    tbuf val;
    if (iterator.ptr)
        attrs = tbuf_after(attrs, iterator);
    attrs = tbuf_strip(attrs);
    if (!attrs.len)
        return attrs;

    for (k = 0; k < attrs.len; k++)
        if (is_spc(attrs.ptr[k]) || attrs.ptr[k] == '=')
            break;
    name->ptr = attrs.ptr;
    name->len = k;
    if (k == attrs.len || attrs.ptr[k] != '=')
    {
        value->ptr = name->ptr + k;
        value->len = 0;
        attrs.len = k;
        return attrs;
    }
    val = tbuf_cutb(tbuf_after(attrs,*name));
    if (val.len && val.ptr[0] == '"')
    {
        int st = 0;
        val = tbuf_cutb(val);
        /* quoted */
        for (k = 0; k < val.len; k++)
        {
            switch(st) {
                case 0:
                    if (val.ptr[k] == '"') {
                        iterator = val;
                        iterator.len = k+1;
                        value->ptr = val.ptr;
                        value->len = k;
                        return iterator;
                    }
                    if (val.ptr[k] == '\\')
                        st = 1;
                    break;
                case 1:
                    st = 0;
                    break;
            }
        }
        *value = val;
        return val;

    }
    for (k = 0; k < val.len; k++)
        if (is_spc(val.ptr[k]))
            break;
    iterator = val;
    iterator.len = k;
    *value = iterator;
    return iterator;
}

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
tbuf xmlParser_inline(tbuf outline)
{
    tbuf tag=xml_GetNextTag(outline);
    if (tag.len > 2 && tag.ptr[tag.len-2] == '/')
        return tbuf_init(); /* <tag /> */
    outline = tbuf_after(outline,tag);
    while (outline.len && outline.ptr[outline.len-1] != '<')
        outline.len--;
    if (outline.len && outline.ptr[outline.len-1] == '<')
        outline.len--;
    return outline;
}

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
tbuf xmlParser_get(tbuf src, const char *path_s)
{
    tbuf path = tbuf_init_s(path_s);
    tbuf result = tbuf_init();
    while (path.len)
    {
        const char *p;
        tbuf tag = tbuf_init();
        tbuf tagname = path;
        p = memchr(tagname.ptr, '.', tagname.len);
        if (p != NULL)
            tagname.len = (int)(p - tagname.ptr) + 1;
        path = tbuf_after(path, tagname);
        if (p != NULL)
            tagname.len--;
        do {
            tag = xmlParser_gettag(src, tag);
            if (tag.len)
            {
                if (tbuf_equal(tagname, xmlParser_tagname(tag)))
                {
                    result = tag;
                    src = xmlParser_inline(tag);
                    break;
                }
            }
        } while (tag.len);
        if (!tag.len)
            return tbuf_init();
    }
    return result;
}

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
tbuf xmlParser_get_attr(tbuf tag, const char *name)
{
    tbuf aname, value;
    tbuf attrs = xmlParser_tagattrs(tag);
    tbuf iterator = tbuf_init();
    do {
        iterator = xmlParser_attrs_getnext(attrs, iterator, &aname, &value);
        if (aname.len)
        {
            if (tbuf_equal_s(aname, name))
                return value;
        }
    } while (iterator.len);
    return tbuf_init();
}

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
int xmlParser_defined(tbuf data)
{
    return (data.ptr == NULL) ? 0 : 1;
}

#ifdef TEST
#include <stdio.h>
int main(void)
{
    tbuf tagname, content, attrs, z;
    tbuf tag=tbuf_init();
    const char *path;
    tbuf src = tbuf_init_s(
       "<first>aaa</first> <single x=\"10\" yy z=5/>  <second>bbb<third>xxx</third><fourth>a</fourth></second>  ");
    do {
        tag = xmlParser_gettag(src, tag);
        if (tag.len)
        {
            tagname = xmlParser_tagname(tag);
            content= xmlParser_inline(tag);
            attrs = xmlParser_tagattrs(tag);
            z = xmlParser_get_attr(tag, "z");
            printf("Got tag '%.*s' attrs='%.*s' outline='%.*s' content='%.*s'\n",
                    tagname.len, tagname.ptr,
                    attrs.len, attrs.ptr,
                    tag.len, tag.ptr,
                    content.len, content.ptr);
            printf("\tz='%.*s'\n", z.len, z.ptr);
            if (attrs.len)
            {
                tbuf name, value, iterator;
                iterator = tbuf_init();
                do {
                    iterator = xmlParser_attrs_getnext(attrs, iterator, &name, &value);
                    if (iterator.len)
                    {
                        printf("\tname='%.*s' value='%.*s'\n",
                            name.len, name.ptr,
                            value.len, value.ptr);
                    }
                } while (iterator.len);
            }
        }
    } while (tag.len);
    path="first";
    tag = xmlParser_get(src, path);
    printf("%s='%.*s'\n", path,tag.len,tag.ptr);
    path="second.third";
    tag = xmlParser_get(src, path);
    printf("%s='%.*s'\n", path,tag.len,tag.ptr);
    path="second";
    tag = xmlParser_get(src, path);
    printf("%s='%.*s'\n", path,tag.len,tag.ptr);
    path="second.zzz";
    tag = xmlParser_get(src, path);
    printf("%s='%.*s'\n", path,tag.len,tag.ptr);
    return 0;
}
#endif /* TEST */
