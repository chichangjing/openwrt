/* re.c: This file contains the regular expression interface routines for
   the ed line editor. */
/*-
 * Copyright (c) 1993 Andrew Moore, Talke Studio.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#include "ed.h"



/* get_compiled_pattern: return pointer to compiled pattern from command
   buffer */
pattern_t *
get_compiled_pattern(ed_data_t *ed)
{
    char *exprs;
    char delimiter;
    int n;

    if ((delimiter = *(ed->ibufp)) == ' ') {
        ed->errmsg = "invalid pattern delimiter";
        return NULL;
    } else if (delimiter == '\n' || *++(ed->ibufp) == '\n' || *(ed->ibufp) == delimiter) {
        if (!ed->expr)
            ed->errmsg = "no previous pattern";
        return ed->expr;
    } else if ((exprs = extract_pattern(ed, delimiter)) == NULL)
        return NULL;
    /* buffer alloc'd && not reserved */
    if (ed->expr && !ed->patlock)
        regfree(ed->expr);
    else if ((ed->expr = (pattern_t *) ED_MALLOC(sizeof(pattern_t))) == NULL) {
        ed->errmsg = "out of memory";
        return NULL;
    }
    ed->patlock = 0;
    n = regcomp(ed->expr, exprs, 0);
    if (n != 0) {
        regerror(n, ed->expr, ed->error, sizeof ed->error);
        ed->errmsg = ed->error;
        ED_FREE(ed->expr);
        return ed->expr = NULL;
    }
    return ed->expr;
}


/* extract_pattern: copy a pattern string from the command buffer; return
   pointer to the copy */
char *
extract_pattern(ed_data_t *ed, int delimiter)
{
    char *nd;
    int len;

    for (nd = ed->ibufp; *nd != delimiter && *nd != '\n'; nd++)
        switch (*nd) {
        default:
            break;
        case '[':
            if ((nd = parse_char_class(++nd)) == NULL) {
                ed->errmsg = "unbalanced brackets ([])";
                return NULL;
            }
            break;
        case '\\':
            if (*++nd == '\n') {
                ed->errmsg = "trailing backslash (\\)";
                return NULL;
            }
            break;
        }
    len = (int)(nd - ed->ibufp);
    REALLOC_C(ed->lhbuf, len + 1, NULL);
    ED_MEMCPY(ed->lhbuf, ed->ibufp, len);
    ed->lhbuf[len] = '\0';
    ed->ibufp = nd;
    return ed->lhbuf;
}


/* parse_char_class: expand a POSIX character class */
char *
parse_char_class(char *s)
{
    int c, d;

    if (*s == '^')
        s++;
    if (*s == ']')
        s++;
    for (; *s != ']' && *s != '\n'; s++)
        if (*s == '[' && ((d = *(s+1)) == '.' || d == ':' || d == '='))
            for (s++, c = *++s; *s != ']' || c != d; s++)
                if ((c = *s) == '\n')
                    return NULL;
    return  (*s == ']') ? s : NULL;
}
