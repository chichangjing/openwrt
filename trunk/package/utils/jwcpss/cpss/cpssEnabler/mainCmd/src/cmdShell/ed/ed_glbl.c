/* glob.c: This file contains the global command routines for the ed line
   editor */
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


/* build_active_list:  add line matching a pattern to the global-active list */
int
build_active_list(ed_data_t *ed, int isgcmd)
{
    pattern_t *pat;
    line_t *lp;
    long n;
    char *s;
    char delimiter;

    if ((delimiter = *(ed->ibufp)) == ' ' || delimiter == '\n') {
        ed->errmsg = "invalid pattern delimiter";
        return ERR;
    } else if ((pat = get_compiled_pattern(ed)) == NULL)
        return ERR;
    else if (*(ed->ibufp) == delimiter)
        ed->ibufp++;
    clear_active_list(ed);
    lp = get_addressed_line_node(ed, ed->first_addr);
    for (n = ed->first_addr; n <= ed->second_addr; n++, lp = lp->q_forw) {
        if ((s = get_sbuf_line(ed, lp)) == NULL)
            return ERR;
        if (!regexec(pat, s, 0, NULL, 0) == isgcmd &&
            set_active_node(ed, lp) < 0)
            return ERR;
    }
    return 0;
}


/* exec_global: apply command list in the command buffer to the active
   lines in a range; return command status */
long
exec_global(ed_data_t *ed, int interact, int gflag)
{
    line_t *lp = NULL;
    int status;
    int n;
    char *cmd = NULL;

    if (!interact)
    {
        if (!ED_STRCMP(ed->ibufp, "\n"))
            cmd = "p\n";        /* null cmd-list == `p' */
        else if ((cmd = get_extended_line(ed, &n, 0)) == NULL)
            return ERR;
    }
    clear_undo_stack(ed);
    while ((lp = next_active_node(ed)) != NULL) {
        if ((ed->current_addr = get_line_node_addr(ed, lp)) < 0)
            return ERR;
        if (interact) {
            /* print ed->current_addr; get a command in global syntax */
            if (display_lines(ed, ed->current_addr, ed->current_addr, gflag) < 0)
                return ERR;
            while ((n = get_tty_line(ed)) > 0 &&
                ed->ibuf[n - 1] != '\n')
            {
            }
            if (n < 0)
                return ERR;
            else if (n == 0) {
                ed->errmsg = "unexpected end-of-file";
                return ERR;
            } else if (n == 1 && !ED_STRCMP(ed->ibuf, "\n"))
                continue;
            else if (n == 2 && !ED_STRCMP(ed->ibuf, "&\n")) {
                if (cmd == NULL) {
                    ed->errmsg = "no previous command";
                    return ERR;
                } else cmd = ed->ocmd;
            } else if ((cmd = get_extended_line(ed, &n, 0)) == NULL)
                return ERR;
            else {
                REALLOC_C(ed->ocmd, n + 1, ERR);
                ED_MEMCPY(ed->ocmd, cmd, n + 1);
                cmd = ed->ocmd;
            }

        }
        ed->ibufp = cmd;
        for (; *(ed->ibufp);)
            if ((status = extract_addr_range(ed)) < 0 ||
                (status = exec_command(ed)) < 0 ||
                (status > 0 && (status = display_lines(ed, 
                ed->current_addr, ed->current_addr, status)) < 0))
                return status;
    }
    return 0;
}

/* set_active_node: add a line node to the global-active list */
int
set_active_node(ed_data_t *ed, line_t *lp)
{
    if (ed->active_last + 1 > ed->active_size) {
        int ti = ed->active_size;
        line_t **ts;
        SPL1();
        if ((ts = (line_t **) ED_REALLOC(ed->active_list,
            (ti += MINBUFSZ) * sizeof(line_t **))) == NULL) {
            ed->errmsg = "out of memory";
            SPL0();
            return ERR;
        }
        ed->active_size = ti;
        ed->active_list = ts;
        SPL0();
    }
    ed->active_list[ed->active_last++] = lp;
    return 0;
}


/* unset_active_nodes: remove a range of lines from the global-active list */
void
unset_active_nodes(ed_data_t *ed, line_t *np, line_t *mp)
{
    line_t *lp;
    long i;

    for (lp = np; lp != mp; lp = lp->q_forw)
        for (i = 0; i < ed->active_last; i++)
            if (ed->active_list[ed->active_ndx] == lp) {
                ed->active_list[ed->active_ndx] = NULL;
                ed->active_ndx = INC_MOD(ed->active_ndx, ed->active_last - 1);
                break;
            } else  ed->active_ndx = INC_MOD(ed->active_ndx, ed->active_last - 1);
}


/* next_active_node: return the next global-active line node */
line_t *
next_active_node(ed_data_t *ed)
{
    while (ed->active_ptr < ed->active_last && ed->active_list[ed->active_ptr] == NULL)
        ed->active_ptr++;
    return (ed->active_ptr < ed->active_last) ? ed->active_list[ed->active_ptr++] : NULL;
}


/* clear_active_list: clear the global-active list */
void
clear_active_list(ed_data_t *ed)
{
    SPL1();
    ed->active_size = ed->active_last = ed->active_ptr = ed->active_ndx = 0;
    if (ed->active_list)
        ED_FREE(ed->active_list);
    ed->active_list = NULL;
    SPL0();
}
