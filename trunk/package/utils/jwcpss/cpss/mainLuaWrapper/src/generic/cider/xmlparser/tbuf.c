/*******************************************************************************
*              (c), Copyright 2011, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* tbuf.c
*
* DESCRIPTION:
*       tbuf: a library to parse strings
*
* DEPENDENCIES:
*
* COMMENTS:
*
* FILE REVISION NUMBER:
*       $Revision: 4 $
**************************************************************************/

#include <string.h>
#include <generic/cider/tbuf.h>

/********************************************************************
* is_spc
*
* Description:
*   return non-zero if character is space
********************************************************************/
int is_spc(char ch)
{
    if (ch == ' ' || ch == '\t' || ch == '\r' || ch == '\n')
        return 1;
    return 0;
}

/********************************************************************
* tbuf_init
*
* Description:
*   return empty tbuf
********************************************************************/
tbuf tbuf_init(void)
{
    tbuf b = { NULL, 0 };
    return b;
}

/********************************************************************
* tbuf_init_s
*
* Description:
*   return tbuf associated with character string
*   String must be not freed while tbuf is used
*
* Inputs:
*   s       - string to associate with tbuf
********************************************************************/
tbuf tbuf_init_s(const char *s)
{
    tbuf b;
    b.ptr = s;
    b.len = (int)strlen(s);
    return b;
}

/********************************************************************
* tbuf_cutb
*
* Description:
*   Remove first character of tbuf
*
* Inputs:
*   d       - source
*
* Returns:
*   tbuf with first character removed
********************************************************************/
tbuf tbuf_cutb(tbuf d)
{
    if (d.len && d.ptr) {
        d.ptr++;
        d.len--;
    }
    return d;
}

/********************************************************************
* tbuf_strip_head
*
* Description:
*   Remove spaces from the head of tbuf
*
* Inputs:
*   d       - source
*
* Returns:
*   tbuf with spaces at the begin removed
********************************************************************/
tbuf tbuf_strip_head(tbuf d)
{
    while (d.len && is_spc(d.ptr[0])) 
        d = tbuf_cutb(d);
    return d;
}

/********************************************************************
* tbuf_strip_tail
*
* Description:
*   Remove spaces from the tail of tbuf
*
* Inputs:
*   d       - source
*
* Returns:
*   tbuf with spaces at the end removed
********************************************************************/
tbuf tbuf_strip_tail(tbuf d)
{
    while (d.len && is_spc(d.ptr[d.len-1]))
        d.len--;
    return d;
}

/********************************************************************
* tbuf_strip
*
* Description:
*   Remove spaces from the head and tail of tbuf
*
* Inputs:
*   d       - source
*
* Returns:
*   tbuf with spaces at the begin and end removed
********************************************************************/
tbuf tbuf_strip(tbuf d)
{
    d = tbuf_strip_head(d);
    return tbuf_strip_tail(d);
}

/********************************************************************
* tbuf_after
*
* Description:
*   Return part of tbuf d from the end of frag till the end of d
*
* Inputs:
*   d       - source
*   frag    - fragment of d
*
* Returns:
*   tbuf
********************************************************************/
tbuf tbuf_after(tbuf d, tbuf frag)
{
    if (frag.ptr < d.ptr || frag.ptr + frag.len >= d.ptr + d.len) {
        d.ptr += d.len;
        d.len = 0;
    } else {
        d.len -= (int)((frag.ptr + frag.len) - d.ptr);
        d.ptr = frag.ptr + frag.len;
    }
    return d;
}

/********************************************************************
* tbuf_starting_from
*
* Description:
*   Return part of tbuf d from the begin of frag till the end of d
*
* Inputs:
*   d       - source
*   frag    - fragment of d
*
* Returns:
*   tbuf
********************************************************************/
tbuf tbuf_starting_from(tbuf d, tbuf frag)
{
    if (frag.ptr < d.ptr || frag.ptr + frag.len >= d.ptr + d.len)
        return d;
    d.len -= (int)(frag.ptr - d.ptr);
    d.ptr = frag.ptr;
    return d;
}

/********************************************************************
* tbuf_equal
*
* Description:
*   conmpare two tbuf s
*
* Inputs:
*   a       - tbuf
*   b       - tbuf
*
* Returns:
*   non zero if a == b
********************************************************************/
int tbuf_equal(tbuf a, tbuf b)
{
    if (a.len != b.len)
        return 0;
    if (a.ptr == b.ptr || a.len == 0)
        return 1;
    if (a.ptr == NULL || b.ptr == NULL)
        return 0;
    return memcmp(a.ptr, b.ptr, a.len) == 0 ? 1 : 0;
}

/********************************************************************
* tbuf_equal_s
*
* Description:
*   conmpare two tbuf s
*
* Inputs:
*   a       - tbuf
*   s       - string to compare with
*
* Returns:
*   non zero if a == s
********************************************************************/
int tbuf_equal_s(tbuf a, const char *s)
{
    int len;
    if (a.ptr == NULL && s == NULL)
        return 1;
    if (s == NULL)
        return (a.len == 0) ? 1 : 0;
    len = (int)strlen(s);
    if (a.len != len)
        return 0;
    if (a.ptr == s || a.len == 0)
        return 1;
    if (a.ptr == NULL)
        return 0;
    return memcmp(a.ptr, s, a.len) == 0 ? 1 : 0;
}

/********************************************************************
* tbuf_strcpy
*
* Description:
*   copy tbuf to string
*
* Inputs:
*   a       - tbuf
*
* Outputs:
*   s       - string to copy to
*
* Returns:
*   s
********************************************************************/
char* tbuf_strcpy(char *s, tbuf a)
{
    if (s)
    {
        if (a.ptr && a.len)
            memcpy(s, a.ptr, a.len);
        s[a.len] = 0;
    }
    return s;
}

/********************************************************************
* tbuf_touint
*
* Description:
*   Convert tbuf (decimal or hex) to integer value
*
* Inputs:
*   a       - tbuf
*
* Returns:
*   integer
********************************************************************/
unsigned int tbuf_touint(tbuf a)
{
    unsigned int ret = 0;
    int     is_hex = 0;

    if (!a.ptr)
        return 0;
    a = tbuf_strip(a);
    while (a.len)
    {
        int digit = -1;
        if (a.ptr[0] == 'x')
        {
            if (is_hex || ret != 0)
            {
                /* wrong character */
                return ret;
            }
            is_hex = 1;
            a = tbuf_cutb(a);
            continue;
        }
        if (a.ptr[0] >= '0' && a.ptr[0] <= '9')
            digit = a.ptr[0] - '0';
        if (a.ptr[0] >= 'A' && a.ptr[0] <= 'F')
            digit = a.ptr[0] - 'A' + 10;
        if (a.ptr[0] >= 'a' && a.ptr[0] <= 'f')
            digit = a.ptr[0] - 'a' + 10;
        if (digit < 0 || (digit > 9 && is_hex == 0))
        {
            /* wrong character */
            return ret;
        }
        if (is_hex)
            ret *= 16;
        else
            ret *= 10;
        ret += digit;
        a = tbuf_cutb(a);
    }
    return ret;
}
