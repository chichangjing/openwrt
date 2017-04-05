/* sub.c: This file contains the substitution routines for the ed
   line editor */
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


/* extract_subst_tail: extract substitution tail from the command buffer */
int
extract_subst_tail(ed_data_t *ed, int *flagp, long *np)
{
    char delimiter;

    *flagp = *np = 0;
    if ((delimiter = *(ed->ibufp)) == '\n') {
        ed->rhbufi = 0;
        *flagp = GPR;
        return 0;
    } else if (extract_subst_template(ed) == NULL)
        return  ERR;
    else if (*(ed->ibufp) == '\n') {
        *flagp = GPR;
        return 0;
    } else if (*(ed->ibufp) == delimiter)
        (ed->ibufp)++;
    if ('1' <= *(ed->ibufp) && *(ed->ibufp) <= '9') {
        STRTOL(*np, ed->ibufp);
        return 0;
    } else if (*(ed->ibufp) == 'g') {
        (ed->ibufp)++;
        *flagp = GSG;
        return 0;
    }
    return 0;
}


/* extract_subst_template: return pointer to copy of substitution template
   in the command buffer */
char *
extract_subst_template(ed_data_t *ed)
{
    int n = 0;
    int i = 0;
    char c;
    char delimiter = *(ed->ibufp)++;

    if (*(ed->ibufp) == '%' && *(ed->ibufp + 1) == delimiter) {
        ed->ibufp++;
        if (!ed->rhbuf)
            ed->errmsg = "no previous substitution";
        return ed->rhbuf;
    }
    while (*(ed->ibufp) != delimiter) {
        REALLOC_C(ed->rhbuf, i + 2, NULL);
        if ((c = ed->rhbuf[i++] = *(ed->ibufp)++) == '\n' && *(ed->ibufp) == '\0') {
            i--, (ed->ibufp)--;
            break;
        } else if (c != '\\')
            ;
        else if ((ed->rhbuf[i++] = *(ed->ibufp)++) != '\n')
            ;
        else if (!ed->isglobal) {
            while ((n = get_tty_line(ed)) == 0 ||
                (n > 0 && ed->ibuf[n - 1] != '\n'))
            { }
            if (n < 0)
                return NULL;
        }
    }
    REALLOC_C(ed->rhbuf, i + 1, NULL);
    ed->rhbuf[ed->rhbufi = i] = '\0';
    return  ed->rhbuf;
}


/* search_and_replace: for each line in a range, change text matching a pattern
   according to a substitution template; return status  */
int
search_and_replace(ed_data_t *ed, pattern_t *pat, int gflag, int kth)
{
    undo_t *up;
    const char *txt;
    const char *eot;
    long lc;
    long xa = ed->current_addr;
    int nsubs = 0;
    line_t *lp;
    int len;

    ed->current_addr = ed->first_addr - 1;
    for (lc = 0; lc <= ed->second_addr - ed->first_addr; lc++) {
        lp = get_addressed_line_node(ed, ++ed->current_addr);
        if ((len = substitute_matching_text(ed, pat, lp, gflag, kth)) < 0)
            return ERR;
        else if (len) {
            up = NULL;
            if (delete_lines(ed, ed->current_addr, ed->current_addr) < 0)
                return ERR;
            txt = ed->rbuf;
            eot = ed->rbuf + len;
            SPL1();
            do {
                if ((txt = put_sbuf_line(ed, txt)) == NULL) {
                    SPL0();
                    return ERR;
                } else if (up)
                    up->t = get_addressed_line_node(ed, ed->current_addr);
                else if ((up = push_undo_stack(ed, UADD,
                    ed->current_addr, ed->current_addr)) == NULL) {
                    SPL0();
                    return ERR;
                }
            } while (txt != eot);
            SPL0();
            nsubs++;
            xa = ed->current_addr;
        }
    }
    ed->current_addr = xa;
    if  (nsubs == 0 && !(gflag & GLB)) {
        ed->errmsg = "no match";
        return ERR;
    } else if ((gflag & (GPR | GLS | GNP)) &&
        display_lines(ed, ed->current_addr, ed->current_addr, gflag) < 0)
        return ERR;
    return 0;
}


/* substitute_matching_text: replace text matched by a pattern according to
   a substitution template; return pointer to the modified text */
int
substitute_matching_text(ed_data_t *ed, pattern_t *pat, line_t *lp, int gflag, int kth)
{
    int off = 0;
    int changed = 0;
    int matchno = 0;
    int i = 0;
    regmatch_t rm[SE_MAX];
    char *txt;
    char *eot;

    if ((txt = get_sbuf_line(ed, lp)) == NULL)
        return ERR;
    eot = txt + lp->len;
    if (!regexec(pat, txt, SE_MAX, rm, 0)) {
        do {
            if (!kth || kth == ++matchno) {
                changed++;
                i = (int)rm[0].rm_so;
                REALLOC_C(ed->rbuf, off + i, ERR);
                ED_MEMCPY(ed->rbuf + off, txt, i);
                off += i;
                if ((off = apply_subst_template(ed, txt, rm, off,
                    (int)pat->re_nsub)) < 0)
                    return ERR;
            } else {
                i = (int)rm[0].rm_eo;
                REALLOC_C(ed->rbuf, off + i, ERR);
                ED_MEMCPY(ed->rbuf + off, txt, i);
                off += i;
            }
            txt += rm[0].rm_eo;
        } while (*txt &&
                        (!changed || ((gflag & GSG) && rm[0].rm_eo)) &&
                !regexec(pat, txt, SE_MAX, rm, REG_NOTBOL));
        i = (int)(eot - txt);
        REALLOC_C(ed->rbuf, off + i + 2, ERR);
        if (i > 0 && !rm[0].rm_eo && (gflag & GSG)) {
            ed->errmsg = "infinite substitution loop";
            return  ERR;
        }
        ED_MEMCPY(ed->rbuf + off, txt, i);
        ED_MEMCPY(ed->rbuf + off + i, "\n", 2);
    }
    return changed ? off + i + 1 : 0;
}


/* apply_subst_template: modify text according to a substitution template;
   return offset to end of modified text */
int
apply_subst_template(ed_data_t *ed, const char *boln, regmatch_t *rm, int off, int re_nsub)
{
    int j = 0;
    int k = 0;
    int n;
    char *sub = ed->rhbuf;

    for (; sub - ed->rhbuf < ed->rhbufi; sub++)
        if (*sub == '&') {
            j = (int)rm[0].rm_so;
            k = (int)rm[0].rm_eo;
            REALLOC_C(ed->rbuf, off + k - j, ERR);
            while (j < k)
                ed->rbuf[off++] = boln[j++];
        } else if (*sub == '\\' && '1' <= *++sub && *sub <= '9' &&
            (n = *sub - '0') <= re_nsub) {
            j = (int)rm[n].rm_so;
            k = (int)rm[n].rm_eo;
            REALLOC_C(ed->rbuf, off + k - j, ERR);
            while (j < k)
                ed->rbuf[off++] = boln[j++];
        } else {
            REALLOC_C(ed->rbuf, off + 1, ERR);
            ed->rbuf[off++] = *sub;
        }
    REALLOC_C(ed->rbuf, off + 1, ERR);
    ed->rbuf[off] = '\0';
    return off;
}
