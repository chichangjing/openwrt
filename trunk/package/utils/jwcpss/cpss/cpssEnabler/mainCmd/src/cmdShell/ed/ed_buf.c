/* buf.c: This file contains the scratch-file buffer routines for the
   ed line editor. */
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

/* get_sbuf_line: get a line of text from the scratch file; return pointer
   to the text */
char *
get_sbuf_line(ed_data_t *ed, line_t *lp)
{
    if (lp == &(ed->buffer_head))
        return NULL;
    return lp->s;
}


/* put_sbuf_line: write a line of text to the scratch file and add a line node
   to the editor buffer;  return a pointer to the end of the text */
const char *
put_sbuf_line(ed_data_t *ed, const char *cs)
{
    line_t *lp;
    int len;
    const char *s;

    if ((lp = (line_t *) ED_MALLOC(sizeof(line_t))) == NULL) {
        ed->errmsg = "out of memory";
        return NULL;
    }
    /* assert: cs is '\n' terminated */
    for (s = cs; *s != '\n'; s++)
        ;
    if (s - cs >= LINECHARS) {
        ed->errmsg = "line too long";
        ED_FREE(lp);
        return NULL;
    }
    len = (int)(s - cs);
    if ((lp->s = ED_MALLOC(len+1)) == NULL) {
        ed->errmsg = "out of memory";
        ED_FREE(lp);
        return NULL;
    }
    ED_MEMCPY(lp->s, cs, len);
    lp->s[len] = 0;
    lp->len = len;
    add_line_node(ed, lp);
    return ++s;
}


/* add_line_node: add a line node in the editor buffer after the current line */
void
add_line_node(ed_data_t *ed, line_t *lp)
{
    line_t *cp;

    cp = get_addressed_line_node(ed, ed->current_addr);             /* this get_addressed_line_node last! */
    INSQUE(lp, cp);
    ed->addr_last++;
    ed->current_addr++;
}


/* get_line_node_addr: return line number of pointer */
long
get_line_node_addr(ed_data_t *ed, line_t *lp)
{
    line_t *cp = &(ed->buffer_head);
    long n = 0;

    while (cp != lp && (cp = cp->q_forw) != &(ed->buffer_head))
        n++;
    if (n && cp == &(ed->buffer_head)) {
        ed->errmsg = "invalid address";
        return ERR;
     }
     return n;
}


/* get_addressed_line_node: return pointer to a line node in the editor buffer */
line_t *
get_addressed_line_node(ed_data_t *ed, long n)
{
#define lp ed->ga_lp
#define on ed->ga_on

    SPL1();
    if (n > on)
        if (n <= (on + ed->addr_last) >> 1)
            for (; on < n; on++)
                lp = lp->q_forw;
        else {
            lp = ed->buffer_head.q_back;
            for (on = ed->addr_last; on > n; on--)
                lp = lp->q_back;
        }
    else
        if (n >= on >> 1)
            for (; on > n; on--)
                lp = lp->q_back;
        else {
            lp = &(ed->buffer_head);
            for (on = 0; on < n; on++)
                lp = lp->q_forw;
        }
    SPL0();
    return lp;
#undef lp
#undef on
}

/* open_sbuf: open scratch file */
int
open_sbuf(ed_data_t *ed)
{
    (void)ed;
    return 0;
}


/* close_sbuf: close scratch file */
int
close_sbuf(ed_data_t *ed)
{
    (void)ed;
    return 0;
}

/* init_buffers: open scratch buffer; initialize line queue */
int
init_buffers(ed_data_t *ed)
{
    int i = 0;

    if (open_sbuf(ed) < 0)
        return 0;
    REQUE(&(ed->buffer_head), &(ed->buffer_head));
    for (i = 0; i < 256; i++)
        ed->ctab[i] = i;
    ed->translit_i = 0;
    return 1;
}

void
free_buffers(ed_data_t *ed)
{
    line_t *l, *n;
    l = ed->buffer_head.q_forw;
    while (l != &(ed->buffer_head))
    {
        n = l->q_forw;
        if (l->s)
            ED_FREE(l->s);
        ED_FREE(l);
        l = n;
    }
    if (l->s)
        ED_FREE(l);
    REQUE(&(ed->buffer_head), &(ed->buffer_head));
    ed->ga_lp = &(ed->buffer_head);
}

/* translit_text: translate characters in a string */
char *
translit_text(ed_data_t *ed, char *s, int len, int from, int to)
{
#define i ed->translit_i

    unsigned char *us;

    ed->ctab[i] = i;            /* restore table to initial state */
    ed->ctab[i = from] = to;
    for (us = (unsigned char *) s; len-- > 0; us++)
        *us = ed->ctab[*us];
    return s;
#undef i
}
