/* undo.c: This file contains the undo routines for the ed line editor */
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


#define USIZE 100               /* undo stack size */
/* push_undo_stack: return pointer to initialized undo node */
undo_t *
push_undo_stack(ed_data_t *ed, int type, long from, long to)
{
    undo_t *t;

    t = ed->ustack;
    if (ed->u_p < ed->usize ||
        (t = (undo_t *) ED_REALLOC(ed->ustack, (ed->usize += USIZE) * sizeof(undo_t))) != NULL) {
        ed->ustack = t;
        ed->ustack[ed->u_p].type = type;
        ed->ustack[ed->u_p].t = get_addressed_line_node(ed, to);
        ed->ustack[ed->u_p].h = get_addressed_line_node(ed, from);
        return ed->ustack + ed->u_p++;
    }
    /* out of memory - release undo stack */
    ed->errmsg = "out of memory";
    clear_undo_stack(ed);
    ED_FREE(ed->ustack);
    ed->ustack = NULL;
    ed->usize = 0;
    return NULL;
}


/* USWAP: swap undo nodes */
#define USWAP(x,y) { \
    undo_t utmp; \
    utmp = x, x = y, y = utmp; \
}


/* pop_undo_stack: undo last change to the editor buffer */
int
pop_undo_stack(ed_data_t *ed)
{
    long n;
    long o_current_addr = ed->current_addr;
    long o_addr_last = ed->addr_last;

    if (ed->u_current_addr == -1 || ed->u_addr_last == -1) {
        ed->errmsg = "nothing to undo";
        return ERR;
    } else if (ed->u_p)
        ed->modified = 1;
    get_addressed_line_node(ed, 0); /* this get_addressed_line_node last! */
    SPL1();
    for (n = ed->u_p; n-- > 0;) {
        switch(ed->ustack[n].type) {
        case UADD:
            REQUE(ed->ustack[n].h->q_back, ed->ustack[n].t->q_forw);
            break;
        case UDEL:
            REQUE(ed->ustack[n].h->q_back, ed->ustack[n].h);
            REQUE(ed->ustack[n].t, ed->ustack[n].t->q_forw);
            break;
        case UMOV:
        case VMOV:
            REQUE(ed->ustack[n - 1].h, ed->ustack[n].h->q_forw);
            REQUE(ed->ustack[n].t->q_back, ed->ustack[n - 1].t);
            REQUE(ed->ustack[n].h, ed->ustack[n].t);
            n--;
            break;
        default:
            /*NOTREACHED*/
            ;
        }
        ed->ustack[n].type ^= 1;
    }
    /* reverse undo stack order */
    for (n = ed->u_p; n-- > (ed->u_p + 1)/ 2;)
        USWAP(ed->ustack[n], ed->ustack[ed->u_p - 1 - n]);
    if (ed->isglobal)
        clear_active_list(ed);
    ed->current_addr = ed->u_current_addr, ed->u_current_addr = o_current_addr;
    ed->addr_last = ed->u_addr_last, ed->u_addr_last = o_addr_last;
    SPL0();
    return 0;
}


/* clear_undo_stack: clear the undo stack */
void
clear_undo_stack(ed_data_t *ed)
{
    line_t *lp, *ep, *tl;

    while (ed->u_p--)
        if (ed->ustack[ed->u_p].type == UDEL) {
            ep = ed->ustack[ed->u_p].t->q_forw;
            for (lp = ed->ustack[ed->u_p].h; lp != ep; lp = tl) {
                unmark_line_node(ed, lp);
                tl = lp->q_forw;
                ED_FREE(lp);
            }
        }
    ed->u_p = 0;
    ed->u_current_addr = ed->current_addr;
    ed->u_addr_last = ed->addr_last;
}
