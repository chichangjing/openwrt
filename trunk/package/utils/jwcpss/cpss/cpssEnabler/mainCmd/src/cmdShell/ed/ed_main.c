/* main.c: This file contains the main control and user-interface routines
   for the ed line editor. */
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

/*
 * CREDITS
 *
 *  This program is based on the editor algorithm described in
 *  Brian W. Kernighan and P. J. Plauger's book "Software Tools
 *  in Pascal," Addison-Wesley, 1981.
 *
 *  The buffering algorithm is attributed to Rodney Ruddock of
 *  the University of Guelph, Guelph, Ontario.
 *
 *  The cbc.c encryption code is adapted from
 *  the bdes program by Matt Bishop of Dartmouth College,
 *  Hanover, NH.
 *
 */

#include "ed.h"


/* ed: line editor */
static ed_data_t* init_ed_data(void)
{
    ed_data_t *ed;
    ed = (ed_data_t*)ED_MALLOC(sizeof(*ed));
    if (!ed)
        return NULL;
    ED_MEMSET(ed, 0, sizeof(*ed));
    ed->errmsg = "";
    ed->dps = "*";
    ed->ga_lp = &(ed->buffer_head);
    ed->u_current_addr = -1;
    ed->u_addr_last = -1;
    ed->rows = 22;
    return ed;
}

static void free_ed_data(ed_data_t *ed)
{
    clear_undo_stack(ed);
    if (ed->ustack)
    {
        ED_FREE(ed->ustack);
        ed->ustack = NULL;
        ed->usize = 0;
    }
    clear_active_list(ed);
    if (ed->join_buf)
        ED_FREE(ed->join_buf);

    if (ed->pat) {
        regfree(ed->pat);
        ED_FREE(ed->pat);
        if (ed->expr == ed->pat) {
            ed->expr = NULL;
        }
    }
    if (ed->expr) {
        regfree(ed->expr);
        ED_FREE(ed->expr);
    }
    close_sbuf(ed);
    free_buffers(ed);
    ED_FREE(ed);
}

/*******************************************************************************
* ed_main
*
* DESCRIPTION:
*       ED editor entry point
*
* INPUTS:
*       filename    - file name to edit or NULL
*       prompt      - ED command prompt or NULL
*       iostream    - cmdStreamPTR
*
* OUTPUTS:
*       None
*
* RETURNS:
*       Non-zero on error
*
* COMMENTS:
*       None
*
*******************************************************************************/
int ed_main(char *filename, char *prompt, void* iostream)
{
    int n;
    long status = 0;
    ed_data_t *ed;

    ed = init_ed_data();
    if (!ed)
        return 2;
    ed->iostream = iostream;
    ed->prompt = prompt;

    /*TODO: prompt = .... */
    {
        if (!init_buffers(ed))
        {
            free_ed_data(ed);
            return 2;
        }
        if (filename && is_legal_filename(ed, filename)) {
            if (read_file(ed, filename, 0) < 0)
            {
                iostream_writeLineNL(ed, "can't read input file, assume new");
            }
            if (ED_STRLEN(filename) > sizeof(ed->old_filename) - 1)
            {
                free_ed_data(ed);
                return 2;
            }
            ED_STRCPY(ed->old_filename, filename);
        } else {
            iostream_writeLine(ed, "?\n");
            if (filename)
                ed->errmsg = "invalid filename";
        }
    }
    for (;;) {
        if (status < 0 && ed->garrulous)
        {
            iostream_writeLineNL(ed, ed->errmsg);
        }
        if (ed->prompt) {
            iostream_writeLine(ed, ed->prompt);
        }
        if ((n = get_tty_line(ed)) < 0) {
            status = ERR;
            continue;
        } else if (n == 0) {
            if (ed->modified) {
                iostream_writeLine(ed, "?\n");
                ed->errmsg = "warning: file ed->modified";
                ed->modified = 0;
                status = EMOD;
                continue;
            } else {
                free_ed_data(ed);
                return 0;
            }
        } else if (ed->ibuf[n - 1] != '\n') {
            /* discard line */
            ed->errmsg = "unexpected end-of-file";
            status = ERR;
            continue;
        }
        ed->isglobal = 0;
        if ((status = extract_addr_range(ed)) >= 0 &&
            (status = exec_command(ed)) >= 0)
            if (!status ||
                (status = display_lines(ed, ed->current_addr, ed->current_addr,
                    status)) >= 0)
                continue;
        if (status == EOF)
            break;
        switch (status) {
        case EMOD:
            ed->modified = 0;
            iostream_writeLine(ed, "?\n");      /* give warning */
            ed->errmsg = "warning: file ed->modified";
            break;
        case FATAL:
            if (ed->garrulous)
            {
                iostream_writeLineNL(ed, ed->errmsg);
            }
            free_ed_data(ed);
            return 3;
        default:
            iostream_writeLine(ed, "?\n");
            break;
        }
    }
    free_ed_data(ed);
    return 0;
}


/* extract_addr_range: get line addresses from the command buffer until an
   illegal address is seen; return status */
int
extract_addr_range(ed_data_t *ed)
{
    long addr;

    ed->addr_cnt = 0;
    ed->first_addr = ed->second_addr = ed->current_addr;
    while ((addr = next_addr(ed)) >= 0) {
        ed->addr_cnt++;
        ed->first_addr = ed->second_addr;
        ed->second_addr = addr;
        if (*ed->ibufp != ',' && *ed->ibufp != ';')
            break;
        else if (*ed->ibufp++ == ';')
            ed->current_addr = addr;
    }
    if ((ed->addr_cnt = min(ed->addr_cnt, 2)) == 1 || ed->second_addr != addr)
        ed->first_addr = ed->second_addr;
    return (addr == ERR) ? ERR : 0;
}


#define SKIP_BLANKS() while (ED_ISSPACE((unsigned char)*(ed->ibufp)) && *(ed->ibufp) != '\n') ed->ibufp++

#define MUST_BE_FIRST() do {                    \
    if (!first) {                       \
        ed->errmsg = "invalid address";         \
        return ERR;                 \
    }                           \
} while (0)

/*  next_addr: return the next line address in the command buffer */
long
next_addr(ed_data_t *ed)
{
    const char *hd;
    long addr = ed->current_addr;
    long n;
    int first = 1;
    int c;

    SKIP_BLANKS();
    for (hd = ed->ibufp;; first = 0)
        switch (c = *ed->ibufp) {
        case '+':
        case '\t':
        case ' ':
        case '-':
        case '^':
            ed->ibufp++;
            SKIP_BLANKS();
            if (ED_ISDIGIT((unsigned char)*(ed->ibufp))) {
                STRTOL(n, ed->ibufp);
                addr += (c == '-' || c == '^') ? -n : n;
            } else if (!ED_ISSPACE((unsigned char)c))
                addr += (c == '-' || c == '^') ? -1 : 1;
            break;
        case '0': case '1': case '2':
        case '3': case '4': case '5':
        case '6': case '7': case '8': case '9':
            MUST_BE_FIRST();
            STRTOL(addr, ed->ibufp);
            break;
        case '.':
        case '$':
            MUST_BE_FIRST();
            ed->ibufp++;
            addr = (c == '.') ? ed->current_addr : ed->addr_last;
            break;
        case '/':
        case '?':
            MUST_BE_FIRST();
            if ((addr = get_matching_node_addr(ed, 
                get_compiled_pattern(ed), c == '/')) < 0)
                return ERR;
            else if (c == *ed->ibufp)
                ed->ibufp++;
            break;
        case '\'':
            MUST_BE_FIRST();
            ed->ibufp++;
            if ((addr = get_marked_node_addr(ed, *(ed->ibufp++))) < 0)
                return ERR;
            break;
        case '%':
        case ',':
        case ';':
            if (first) {
                ed->ibufp++;
                ed->addr_cnt++;
                ed->second_addr = (c == ';') ? ed->current_addr : 1;
                addr = ed->addr_last;
                break;
            }
            /* FALLTHROUGH */
        default:
            if (ed->ibufp == hd)
                return EOF;
            else if (addr < 0 || ed->addr_last < addr) {
                ed->errmsg = "invalid address";
                return ERR;
            } else
                return addr;
        }
    /* NOTREACHED */
}


/* GET_THIRD_ADDR: get a legal address from the command buffer */
#define GET_THIRD_ADDR(addr) \
{ \
    long ol1, ol2; \
\
    ol1 = ed->first_addr, ol2 = ed->second_addr; \
    if (extract_addr_range(ed) < 0) \
        return ERR; \
    else if (ed->addr_cnt == 0) { \
        ed->errmsg = "destination expected"; \
        return ERR; \
    } else if (ed->second_addr < 0 || ed->addr_last < ed->second_addr) { \
        ed->errmsg = "invalid address"; \
        return ERR; \
    } \
    addr = ed->second_addr; \
    ed->first_addr = ol1, ed->second_addr = ol2; \
}


/* GET_COMMAND_SUFFIX: verify the command suffix in the command buffer */
#define GET_COMMAND_SUFFIX() { \
    int done = 0; \
    do { \
        switch(*ed->ibufp) { \
        case 'p': \
            gflag |= GPR, ed->ibufp++; \
            break; \
        case 'l': \
            gflag |= GLS, ed->ibufp++; \
            break; \
        case 'n': \
            gflag |= GNP, ed->ibufp++; \
            break; \
        default: \
            done++; \
        } \
    } while (!done); \
    if (*ed->ibufp++ != '\n') { \
        ed->errmsg = "invalid command suffix"; \
        return ERR; \
    } \
}


/* sflags */
#define SGG 001     /* complement previous global substitute suffix */
#define SGP 002     /* complement previous print suffix */
#define SGR 004     /* use last regex instead of last pat */
#define SGF 010     /* repeat last substitution */



/* exec_command: execute the next command in command buffer; return print
   request, if any */
int
exec_command(ed_data_t *ed)
{
    pattern_t *tpat;
    char *fnp;
    int gflag = 0;
    int sflags = 0;
    long addr = 0;
    int n = 0;
    int c;

    SKIP_BLANKS();
    switch(c = *ed->ibufp++) {
    case 'a':
        GET_COMMAND_SUFFIX();
        if (!ed->isglobal) clear_undo_stack(ed);
        if (append_lines(ed, ed->second_addr) < 0)
            return ERR;
        break;
    case 'c':
        if (check_addr_range(ed, ed->current_addr, ed->current_addr) < 0)
            return ERR;
        GET_COMMAND_SUFFIX();
        if (!ed->isglobal) clear_undo_stack(ed);
        if (delete_lines(ed, ed->first_addr, ed->second_addr) < 0 ||
            append_lines(ed, ed->current_addr) < 0)
            return ERR;
        break;
    case 'd':
        if (check_addr_range(ed, ed->current_addr, ed->current_addr) < 0)
            return ERR;
        GET_COMMAND_SUFFIX();
        if (!ed->isglobal) clear_undo_stack(ed);
        if (delete_lines(ed, ed->first_addr, ed->second_addr) < 0)
            return ERR;
        else if ((addr = INC_MOD(ed->current_addr, ed->addr_last)) != 0)
            ed->current_addr = addr;
        break;
    case 'e':
        if (ed->modified)
            return EMOD;
        /* FALLTHROUGH */
    case 'E':
        if (ed->addr_cnt > 0) {
            ed->errmsg = "unexpected address";
            return ERR;
        } else if (!ED_ISSPACE((unsigned char)*ed->ibufp)) {
            ed->errmsg = "unexpected command suffix";
            return ERR;
        } else if ((fnp = get_filename(ed)) == NULL)
            return ERR;
        GET_COMMAND_SUFFIX();
        if (delete_lines(ed, 1, ed->addr_last) < 0)
            return ERR;
        clear_undo_stack(ed);
        if (close_sbuf(ed) < 0)
            return ERR;
        else if (open_sbuf(ed) < 0)
            return FATAL;
        if (*fnp && *fnp != '!') ED_STRCPY(ed->old_filename, fnp);
        if (*fnp == '\0' && *ed->old_filename == '\0') {
            ed->errmsg = "no current filename";
            return ERR;
        }
        if (read_file(ed, *fnp ? fnp : ed->old_filename, 0) < 0)
            return ERR;
        clear_undo_stack(ed);
        ed->modified = 0;
        ed->u_current_addr = ed->u_addr_last = -1;
        break;
    case 'f':
        if (ed->addr_cnt > 0) {
            ed->errmsg = "unexpected address";
            return ERR;
        } else if (!ED_ISSPACE((unsigned char)*ed->ibufp)) {
            ed->errmsg = "unexpected command suffix";
            return ERR;
        } else if ((fnp = get_filename(ed)) == NULL)
            return ERR;
        else if (*fnp == '!') {
            ed->errmsg = "invalid redirection";
            return ERR;
        }
        GET_COMMAND_SUFFIX();
        if (*fnp) ED_STRCPY(ed->old_filename, fnp);
        iostream_writeLineNL(ed, strip_escapes(ed, ed->old_filename));
        break;
    case 'g':
    case 'v':
    case 'G':
    case 'V':
        if (ed->isglobal) {
            ed->errmsg = "cannot nest global commands";
            return ERR;
        } else if (check_addr_range(ed, 1, ed->addr_last) < 0)
            return ERR;
        else if (build_active_list(ed, c == 'g' || c == 'G') < 0)
            return ERR;
        else
        {
            n = (c == 'G' || c == 'V');
            if (n != 0)
            {
                GET_COMMAND_SUFFIX();
            }
        }
        ed->isglobal++;
        if (exec_global(ed, n, gflag) < 0)
            return ERR;
        break;
    case 'h':
        if (ed->addr_cnt > 0) {
            ed->errmsg = "unexpected address";
            return ERR;
        }
        GET_COMMAND_SUFFIX();
        if (*ed->errmsg) {
            iostream_writeLineNL(ed, ed->errmsg);
        }
        break;
    case 'H':
        if (ed->addr_cnt > 0) {
            ed->errmsg = "unexpected address";
            return ERR;
        }
        GET_COMMAND_SUFFIX();
        if ((ed->garrulous = 1 - ed->garrulous) && *ed->errmsg)
        {
            iostream_writeLineNL(ed, ed->errmsg);
        }
        break;
    case 'i':
        if (ed->second_addr == 0) {
            ed->errmsg = "invalid address";
            return ERR;
        }
        GET_COMMAND_SUFFIX();
        if (!ed->isglobal) clear_undo_stack(ed);
        if (append_lines(ed, ed->second_addr - 1) < 0)
            return ERR;
        break;
    case 'j':
        if (check_addr_range(ed, ed->current_addr, ed->current_addr + 1) < 0)
            return ERR;
        GET_COMMAND_SUFFIX();
        if (!ed->isglobal) clear_undo_stack(ed);
        if (ed->first_addr != ed->second_addr &&
            join_lines(ed, ed->first_addr, ed->second_addr) < 0)
            return ERR;
        break;
    case 'k':
        c = *ed->ibufp++;
        if (ed->second_addr == 0) {
            ed->errmsg = "invalid address";
            return ERR;
        }
        GET_COMMAND_SUFFIX();
        if (mark_line_node(ed, get_addressed_line_node(ed, ed->second_addr), c) < 0)
            return ERR;
        break;
    case 'l':
        if (check_addr_range(ed, ed->current_addr, ed->current_addr) < 0)
            return ERR;
        GET_COMMAND_SUFFIX();
        if (display_lines(ed, ed->first_addr, ed->second_addr, gflag | GLS) < 0)
            return ERR;
        gflag = 0;
        break;
    case 'm':
        if (check_addr_range(ed, ed->current_addr, ed->current_addr) < 0)
            return ERR;
        GET_THIRD_ADDR(addr);
        if (ed->first_addr <= addr && addr < ed->second_addr) {
            ed->errmsg = "invalid destination";
            return ERR;
        }
        GET_COMMAND_SUFFIX();
        if (!ed->isglobal) clear_undo_stack(ed);
        if (move_lines(ed, addr) < 0)
            return ERR;
        break;
    case 'n':
        if (check_addr_range(ed, ed->current_addr, ed->current_addr) < 0)
            return ERR;
        GET_COMMAND_SUFFIX();
        if (display_lines(ed, ed->first_addr, ed->second_addr, gflag | GNP) < 0)
            return ERR;
        gflag = 0;
        break;
    case 'p':
        if (check_addr_range(ed, ed->current_addr, ed->current_addr) < 0)
            return ERR;
        GET_COMMAND_SUFFIX();
        if (display_lines(ed, ed->first_addr, ed->second_addr, gflag | GPR) < 0)
            return ERR;
        gflag = 0;
        break;
    case 'P':
        if (ed->addr_cnt > 0) {
            ed->errmsg = "unexpected address";
            return ERR;
        }
        GET_COMMAND_SUFFIX();
        ed->prompt = ed->prompt ? NULL : ed->dps;
        break;
    case 'q':
    case 'Q':
        if (ed->addr_cnt > 0) {
            ed->errmsg = "unexpected address";
            return ERR;
        }
        GET_COMMAND_SUFFIX();
        gflag =  (ed->modified && c == 'q') ? EMOD : EOF;
        break;
    case 'r':
        if (!ED_ISSPACE((unsigned char)*ed->ibufp)) {
            ed->errmsg = "unexpected command suffix";
            return ERR;
        } else if (ed->addr_cnt == 0)
            ed->second_addr = ed->addr_last;
        if ((fnp = get_filename(ed)) == NULL)
            return ERR;
        GET_COMMAND_SUFFIX();
        if (!ed->isglobal) clear_undo_stack(ed);
        if (*ed->old_filename == '\0' && *fnp != '!')
            ED_STRCPY(ed->old_filename, fnp);
        if (*fnp == '\0' && *ed->old_filename == '\0') {
            ed->errmsg = "no current filename";
            return ERR;
        }
        if ((addr = read_file(ed, *fnp ? fnp : ed->old_filename, ed->second_addr)) < 0)
            return ERR;
        else if (addr && addr != ed->addr_last)
            ed->modified = 1;
        break;
    case 's':
        do {
            switch(*ed->ibufp) {
            case '\n':
                sflags |=SGF;
                break;
            case 'g':
                sflags |= SGG;
                ed->ibufp++;
                break;
            case 'p':
                sflags |= SGP;
                ed->ibufp++;
                break;
            case 'r':
                sflags |= SGR;
                ed->ibufp++;
                break;
            case '0': case '1': case '2': case '3': case '4':
            case '5': case '6': case '7': case '8': case '9':
                STRTOL(ed->sgnum, ed->ibufp);
                sflags |= SGF;
                ed->sgflag &= ~GSG;     /* override GSG */
                break;
            default:
                if (sflags) {
                    ed->errmsg = "invalid command suffix";
                    return ERR;
                }
            }
        } while (sflags && *ed->ibufp != '\n');
        if (sflags && !ed->pat) {
            ed->errmsg = "no previous substitution";
            return ERR;
        } else if (sflags & SGG)
            ed->sgnum = 0;      /* override numeric arg */
        if (*ed->ibufp != '\n' && *(ed->ibufp + 1) == '\n') {
            ed->errmsg = "invalid pattern delimiter";
            return ERR;
        }
        tpat = ed->pat;
        SPL1();
        if ((!sflags || (sflags & SGR)) &&
            (tpat = get_compiled_pattern(ed)) == NULL) {
            SPL0();
            return ERR;
        } else if (tpat != ed->pat) {
            if (ed->pat) {
                regfree(ed->pat);
                ED_FREE(ed->pat);
                if (ed->expr == ed->pat) {
                    ed->expr = NULL;
                }
            }
            ed->pat = tpat;
            ed->patlock = 1;        /* reserve pattern */
        }
        SPL0();
        if (!sflags && extract_subst_tail(ed, &ed->sgflag, &ed->sgnum) < 0)
            return ERR;
        else if (ed->isglobal)
            ed->sgflag |= GLB;
        else
            ed->sgflag &= ~GLB;
        if (sflags & SGG)
            ed->sgflag ^= GSG;
        if (sflags & SGP)
            ed->sgflag ^= GPR, ed->sgflag &= ~(GLS | GNP);
        do {
            switch(*ed->ibufp) {
            case 'p':
                ed->sgflag |= GPR, ed->ibufp++;
                break;
            case 'l':
                ed->sgflag |= GLS, ed->ibufp++;
                break;
            case 'n':
                ed->sgflag |= GNP, ed->ibufp++;
                break;
            default:
                n++;
            }
        } while (!n);
        if (check_addr_range(ed, ed->current_addr, ed->current_addr) < 0)
            return ERR;
        GET_COMMAND_SUFFIX();
        if (!ed->isglobal) clear_undo_stack(ed);
        if (search_and_replace(ed, ed->pat, ed->sgflag, ed->sgnum) < 0)
            return ERR;
        break;
    case 't':
        if (check_addr_range(ed, ed->current_addr, ed->current_addr) < 0)
            return ERR;
        GET_THIRD_ADDR(addr);
        GET_COMMAND_SUFFIX();
        if (!ed->isglobal) clear_undo_stack(ed);
        if (copy_lines(ed, addr) < 0)
            return ERR;
        break;
    case 'u':
        if (ed->addr_cnt > 0) {
            ed->errmsg = "unexpected address";
            return ERR;
        }
        GET_COMMAND_SUFFIX();
        if (pop_undo_stack(ed) < 0)
            return ERR;
        break;
    case 'w':
    case 'W':
        if ((n = *ed->ibufp) == 'q' || n == 'Q') {
            gflag = EOF;
            ed->ibufp++;
        }
        if (!ED_ISSPACE((unsigned char)*ed->ibufp)) {
            ed->errmsg = "unexpected command suffix";
            return ERR;
        } else if ((fnp = get_filename(ed)) == NULL)
            return ERR;
        if (ed->addr_cnt == 0 && !ed->addr_last)
            ed->first_addr = ed->second_addr = 0;
        else if (check_addr_range(ed, 1, ed->addr_last) < 0)
            return ERR;
        GET_COMMAND_SUFFIX();
        if (*ed->old_filename == '\0' && *fnp != '!')
            ED_STRCPY(ed->old_filename, fnp);
        if (*fnp == '\0' && *ed->old_filename == '\0') {
            ed->errmsg = "no current filename";
            return ERR;
        }
        if ((addr = write_file(ed, *fnp ? fnp : ed->old_filename,
            (c == 'W') ? "a" : "w", ed->first_addr, ed->second_addr)) < 0)
            return ERR;
        else if (addr == ed->addr_last)
            ed->modified = 0;
        else if (ed->modified && n == 'q')
            gflag = EMOD;
        break;
    case 'x':
        if (ed->addr_cnt > 0) {
            ed->errmsg = "unexpected address";
            return ERR;
        }
        GET_COMMAND_SUFFIX();
        ed->errmsg = "crypt unavailable";
        return ERR;
    case 'z':
        if (check_addr_range(ed, ed->first_addr = 1, ed->current_addr + 1) < 0)
            return ERR;
        else if ('0' < *ed->ibufp && *ed->ibufp <= '9')
            STRTOL(ed->rows, ed->ibufp);
        GET_COMMAND_SUFFIX();
        if (display_lines(ed, ed->second_addr, min(ed->addr_last,
            ed->second_addr + ed->rows), gflag) < 0)
            return ERR;
        gflag = 0;
        break;
    case '=':
        GET_COMMAND_SUFFIX();
        {
            char tmpbuf[32];
            ED_SPRINTF(tmpbuf, "%ld\n", ed->addr_cnt ? ed->second_addr : ed->addr_last);
            iostream_writeLine(ed, tmpbuf);
        }
        break;
    case '!':
        ed->errmsg = "not supported";
        return ERR;
    case '\n':
        if (check_addr_range(ed, ed->first_addr = 1, ed->current_addr + 1) < 0
         || display_lines(ed, ed->second_addr, ed->second_addr, 0) < 0)
            return ERR;
        break;
    default:
        ed->errmsg = "unknown command";
        return ERR;
    }
    return gflag;
}


/* check_addr_range: return status of address range check */
int
check_addr_range(ed_data_t *ed, long n, long m)
{
    if (ed->addr_cnt == 0) {
        ed->first_addr = n;
        ed->second_addr = m;
    }
    if (ed->first_addr > ed->second_addr || 1 > ed->first_addr ||
        ed->second_addr > ed->addr_last) {
        ed->errmsg = "invalid address";
        return ERR;
    }
    return 0;
}


/* get_matching_node_addr: return the address of the next line matching a
   pattern in a given direction.  wrap around begin/end of editor buffer if
   necessary */
long
get_matching_node_addr(ed_data_t *ed, pattern_t *pat, int dir)
{
    char *s;
    long n = ed->current_addr;
    line_t *lp;

    if (!pat) return ERR;
    do {
            n = dir ? INC_MOD(n, ed->addr_last) : DEC_MOD(n, ed->addr_last);
            if (n != 0) 
            {
                lp = get_addressed_line_node(ed, n);
                if ((s = get_sbuf_line(ed, lp)) == NULL)
                    return ERR;
                if (!regexec(pat, s, 0, NULL, 0))
                    return n;
           }
    } while (n != ed->current_addr);
    ed->errmsg = "no match";
    return  ERR;
}


/* get_filename: return pointer to copy of filename in the command buffer */
char *
get_filename(ed_data_t *ed)
{
    int n;

    if (*ed->ibufp != '\n') {
        SKIP_BLANKS();
        if (*ed->ibufp == '\n') {
            ed->errmsg = "invalid filename";
            return NULL;
        } else if ((ed->ibufp = get_extended_line(ed, &n, 1)) == NULL)
            return NULL;
        else if (*ed->ibufp == '!') {
            ed->errmsg = "not supported";
            return NULL;
        } else if (n > PATH_MAX - 1) {
            ed->errmsg = "filename too long";
            return  NULL;
        }
    }
    for (n = 0; *ed->ibufp != '\n';)
        ed->tmp_filename[n++] = *ed->ibufp++;
    ed->tmp_filename[n] = '\0';
    return is_legal_filename(ed, ed->tmp_filename) ? ed->tmp_filename : NULL;
}


/* get_shell_command: read a shell command from stdin; return substitution
   status */
int
get_shell_command(ed_data_t *ed)
{
    ed->errmsg = "shell access restricted";
    return ERR;
}


/* append_lines: insert text from stdin to after line n; stop when either a
   single period is read or EOF; return status */
int
append_lines(ed_data_t *ed, long n)
{
    int l;
    const char *lp = ed->ibuf;
    const char *eot;
    undo_t *up = NULL;

    for (ed->current_addr = n;;) {
        if (!ed->isglobal) {
            if ((l = get_tty_line(ed)) < 0)
                return ERR;
            else if (l == 0 || ed->ibuf[l - 1] != '\n') {
                return  l ? EOF : 0;
            }
            lp = ed->ibuf;
        } else if (*(lp = ed->ibufp) == '\0')
            return 0;
        else {
            while (*ed->ibufp++ != '\n')
                ;
            l = (int)(ed->ibufp - lp);
        }
        if (l == 2 && lp[0] == '.' && lp[1] == '\n') {
            return 0;
        }
        eot = lp + l;
        SPL1();
        do {
            if ((lp = put_sbuf_line(ed, lp)) == NULL) {
                SPL0();
                return ERR;
            } else if (up)
                up->t = get_addressed_line_node(ed, ed->current_addr);
            else if ((up = push_undo_stack(ed, UADD, ed->current_addr,
                ed->current_addr)) == NULL) {
                SPL0();
                return ERR;
            }
        } while (lp != eot);
        ed->modified = 1;
        SPL0();
    }
    /* NOTREACHED */
}


/* join_lines: replace a range of lines with the joined text of those lines */
int
join_lines(ed_data_t *ed, long from, long to)
{
    char *s;
    int size = 0;
    line_t *bp, *ep;

    ep = get_addressed_line_node(ed, INC_MOD(to, ed->addr_last));
    bp = get_addressed_line_node(ed, from);
    for (; bp != ep; bp = bp->q_forw) {
        if ((s = get_sbuf_line(ed, bp)) == NULL)
            return ERR;
        REALLOC(ed->join_buf, ed->join_bufl, size + bp->len, ERR);
        ED_MEMCPY(ed->join_buf + size, s, bp->len);
        size += bp->len;
    }
    REALLOC(ed->join_buf, ed->join_bufl, size + 2, ERR);
    ED_MEMCPY(ed->join_buf + size, "\n", 2);
    if (delete_lines(ed, from, to) < 0)
        return ERR;
    ed->current_addr = from - 1;
    SPL1();
    if (put_sbuf_line(ed, ed->join_buf) == NULL ||
        push_undo_stack(ed, UADD, ed->current_addr, ed->current_addr) == NULL) {
        SPL0();
        return ERR;
    }
    ed->modified = 1;
    SPL0();
    return 0;
}


/* move_lines: move a range of lines */
int
move_lines(ed_data_t *ed, long addr)
{
    line_t *b1, *a1, *b2, *a2;
    long n = INC_MOD(ed->second_addr, ed->addr_last);
    long p = ed->first_addr - 1;
    int done = (addr == ed->first_addr - 1 || addr == ed->second_addr);

    SPL1();
    if (done) {
        a2 = get_addressed_line_node(ed, n);
        b2 = get_addressed_line_node(ed, p);
        ed->current_addr = ed->second_addr;
    } else if (push_undo_stack(ed, UMOV, p, n) == NULL ||
        push_undo_stack(ed, UMOV, addr, INC_MOD(addr, ed->addr_last)) == NULL) {
        SPL0();
        return ERR;
    } else {
        a1 = get_addressed_line_node(ed, n);
        if (addr < ed->first_addr) {
            b1 = get_addressed_line_node(ed, p);
            b2 = get_addressed_line_node(ed, addr);
                    /* this get_addressed_line_node last! */
        } else {
            b2 = get_addressed_line_node(ed, addr);
            b1 = get_addressed_line_node(ed, p);
                    /* this get_addressed_line_node last! */
        }
        a2 = b2->q_forw;
        REQUE(b2, b1->q_forw);
        REQUE(a1->q_back, a2);
        REQUE(b1, a1);
        ed->current_addr = addr + ((addr < ed->first_addr) ?
            ed->second_addr - ed->first_addr + 1 : 0);
    }
    if (ed->isglobal)
        unset_active_nodes(ed, b2->q_forw, a2);
    ed->modified = 1;
    SPL0();
    return 0;
}


/* copy_lines: copy a range of lines; return status */
int
copy_lines(ed_data_t *ed, long addr)
{
    line_t *lp, *np = get_addressed_line_node(ed, ed->first_addr);
    undo_t *up = NULL;
    long n = ed->second_addr - ed->first_addr + 1;
    long m = 0;

    ed->current_addr = addr;
    if (ed->first_addr <= addr && addr < ed->second_addr) {
        n =  addr - ed->first_addr + 1;
        m = ed->second_addr - addr;
    }
    for (; n > 0; n=m, m=0, np = get_addressed_line_node(ed, ed->current_addr + 1))
        for (; n-- > 0; np = np->q_forw) {
            SPL1();
            if ((lp = dup_line_node(ed, np)) == NULL) {
                SPL0();
                return ERR;
            }
            add_line_node(ed, lp);
            if (up)
                up->t = lp;
            else if ((up = push_undo_stack(ed, UADD, ed->current_addr,
                ed->current_addr)) == NULL) {
                SPL0();
                return ERR;
            }
            ed->modified = 1;
            SPL0();
        }
    return 0;
}


/* delete_lines: delete a range of lines */
int
delete_lines(ed_data_t *ed, long from, long to)
{
    line_t *n, *p;

    SPL1();
    if (push_undo_stack(ed, UDEL, from, to) == NULL) {
        SPL0();
        return ERR;
    }
    n = get_addressed_line_node(ed, INC_MOD(to, ed->addr_last));
    p = get_addressed_line_node(ed, from - 1);
                    /* this get_addressed_line_node last! */
    if (ed->isglobal)
        unset_active_nodes(ed, p->q_forw, n);
    REQUE(p, n);
    ed->addr_last -= to - from + 1;
    ed->current_addr = from - 1;
    ed->modified = 1;
    SPL0();
    return 0;
}


/* display_lines: print a range of lines to stdout */
int
display_lines(ed_data_t *ed, long from, long to, int gflag)
{
    line_t *bp;
    line_t *ep;
    char *s;

    if (!from) {
        ed->errmsg = "invalid address";
        return ERR;
    }
    ep = get_addressed_line_node(ed, INC_MOD(to, ed->addr_last));
    bp = get_addressed_line_node(ed, from);
    for (; bp != ep; bp = bp->q_forw) {
        if ((s = get_sbuf_line(ed, bp)) == NULL)
            return ERR;
        if (put_tty_line(ed, s, bp->len, ed->current_addr = from++, gflag) < 0)
            return ERR;
    }
    return 0;
}


/* mark_line_node: set a line node ed->mark */
int
mark_line_node(ed_data_t *ed, line_t *lp, int n)
{
    if (!ED_ISLOWER((unsigned char)n)) {
        ed->errmsg = "invalid ed->mark character";
        return ERR;
    } else if (ed->mark[n - 'a'] == NULL)
        ed->markno++;
    ed->mark[n - 'a'] = lp;
    return 0;
}


/* get_marked_node_addr: return address of a marked line */
long
get_marked_node_addr(ed_data_t *ed, int n)
{
    if (!ED_ISLOWER((unsigned char)n)) {
        ed->errmsg = "invalid ed->mark character";
        return ERR;
    }
    return get_line_node_addr(ed, ed->mark[n - 'a']);
}


/* unmark_line_node: clear line node mark */
void
unmark_line_node(ed_data_t *ed, line_t *lp)
{
    int i;

    for (i = 0; ed->markno && i < MAXMARK; i++)
        if (ed->mark[i] == lp) {
            ed->mark[i] = NULL;
            ed->markno--;
        }
}


/* dup_line_node: return a pointer to a copy of a line node */
line_t *
dup_line_node(ed_data_t *ed, line_t *lp)
{
    line_t *np;

    if ((np = (line_t *) ED_MALLOC(sizeof(line_t))) == NULL) {
        iostream_writeLine(ed, "ED_MALLOC() failed\n");
        ed->errmsg = "out of memory";
        return NULL;
    }
    np->s = ED_MALLOC(ED_STRLEN(lp->s)+1);
    ED_STRCPY(np->s, lp->s);
    np->len = lp->len;
    return np;
}


/* has_trailing_escape:  return the parity of escapes preceding a character
   in a string */
int
has_trailing_escape(char *s, char *t)
{
    return (s == t || *(t - 1) != '\\') ? 0 : !has_trailing_escape(s, t - 1);
}


/* strip_escapes: return copy of escaped string of at most length PATH_MAX */
char *
strip_escapes(ed_data_t *ed, char *s)
{
    unsigned i = 0;

    while (i < sizeof(ed->tmp_filename) - 1 /* Worry about a possible trailing escape */
           && (ed->tmp_filename[i++] = (*s == '\\') ? *++s : *s))
        s++;
    return ed->tmp_filename;
}


/* is_legal_filename: return a legal filename */
int
is_legal_filename(ed_data_t *ed, char *s)
{
    if (*s == '!') {
        ed->errmsg = "shell access restricted";
        return 0;
    }
    return 1;
}
