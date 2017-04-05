/*******************************************************************************
*              (c), Copyright 2011, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* tbuf.h
*
* DESCRIPTION:
*       tbuf: a library to parse strings
*
* DEPENDENCIES:
*
* COMMENTS:
*
* FILE REVISION NUMBER:
*       $Revision: 2 $
**************************************************************************/

#ifndef __tbuf_h__
#define __tbuf_h__

/* tbuf
 *  A structure to manupulate string content
 *  This struct can be easy passed as parameter by value 
 *  and returned by value
 */
typedef struct tbuf {
    const char *ptr;
    int     len;
} tbuf;

/********************************************************************
* tbuf_init
*
* Description:
*   return empty tbuf
********************************************************************/
tbuf tbuf_init(void);
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
tbuf tbuf_init_s(const char *s);
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
tbuf tbuf_cutb(tbuf d);
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
tbuf tbuf_strip_head(tbuf d);
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
tbuf tbuf_strip_tail(tbuf d);
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
tbuf tbuf_strip(tbuf d);
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
tbuf tbuf_after(tbuf d, tbuf frag);
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
tbuf tbuf_starting_from(tbuf d, tbuf frag);
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
int tbuf_equal(tbuf a, tbuf b);
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
int tbuf_equal_s(tbuf a, const char *s);
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
char* tbuf_strcpy(char *s, tbuf a);

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
unsigned int tbuf_touint(tbuf a);

/********************************************************************
* is_spc
*
* Description:
*   return non-zero if character is space
********************************************************************/
int is_spc(char ch);

#endif /* __tbuf_h__ */
