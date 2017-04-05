/* ed.h: type and constant definitions for the ed editor. */
/*-
 * Copyright (c) 1993 Andrew Moore
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
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 *  @(#)ed.h,v 1.5 1994/02/01 00:34:39 alm Exp
 * $FreeBSD: src/bin/ed/ed.h,v 1.20.28.1 2011/09/23 00:51:37 kensmith Exp $
 */

/*****************/
/* API functions */
/*****************/

/*******************************************************************************
* ed_main
*
* DESCRIPTION:
*       ED editor entry point
*
* INPUTS:
*       filename    - file name to edit or NULL
*       prompt      - ED command prompt or NULL
*       iostream    - IOStreamPTR
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
int ed_main(char *filename, char *prompt, void* iostream);


/******************/
/* Implementation */
/******************/
#include "ed_env.h"

/* namespace mapping */
#define add_line_node   ed_add_line_node
#define append_lines    ed_append_lines
#define apply_subst_template    ed_apply_subst_template
#define build_active_list   ed_build_active_list
#define check_addr_range    ed_check_addr_range
#define clear_active_list   ed_clear_active_list
#define clear_undo_stack    ed_clear_undo_stack
#define close_sbuf  ed_close_sbuf
#define copy_lines  ed_copy_lines
#define delete_lines    ed_delete_lines
#define display_lines   ed_display_lines
#define dup_line_node   ed_dup_line_node
#define exec_command    ed_exec_command
#define exec_global ed_exec_global
#define extract_addr_range  ed_extract_addr_range
#define extract_pattern ed_extract_pattern
#define extract_subst_tail  ed_extract_subst_tail
#define extract_subst_template  ed_extract_subst_template
#define filter_lines    ed_filter_lines
#define free_buffers    ed_free_buffers
#define get_addressed_line_node ed_get_addressed_line_node
#define get_compiled_pattern    ed_get_compiled_pattern
#define get_extended_line   ed_get_extended_line
#define get_filename    ed_get_filename
#define get_keyword ed_get_keyword
#define get_line_node_addr  ed_get_line_node_addr
#define get_matching_node_addr  ed_get_matching_node_addr
#define get_marked_node_addr    ed_get_marked_node_addr
#define get_sbuf_line   ed_get_sbuf_line
#define get_shell_command   ed_get_shell_command
#define get_tty_line    ed_get_tty_line
#define has_trailing_escape ed_has_trailing_escape
#define hex_to_binary   ed_hex_to_binary
#define init_buffers    ed_init_buffers
#define is_legal_filename   ed_is_legal_filename
#define join_lines  ed_join_lines
#define mark_line_node  ed_mark_line_node
#define move_lines  ed_move_lines
#define next_active_node    ed_next_active_node
#define next_addr   ed_next_addr
#define open_sbuf   ed_open_sbuf
#define parse_char_class    ed_parse_char_class
#define pop_undo_stack  ed_pop_undo_stack
#define push_undo_stack ed_push_undo_stack
#define put_sbuf_line   ed_put_sbuf_line
#define put_tty_line    ed_put_tty_line
#define read_file   ed_read_file
#define search_and_replace  ed_search_and_replace
#define set_active_node ed_set_active_node
#define strip_escapes   ed_strip_escapes
#define substitute_matching_text    ed_substitute_matching_text
#define translit_text   ed_translit_text
#define unmark_line_node    ed_unmark_line_node
#define unset_active_nodes  ed_unset_active_nodes
#define write_file  ed_write_file

#define iostream_readline   ed_iostream_readline
#define iostream_write  ed_iostream_write
#define iostream_writeLine  ed_iostream_writeLine
#define iostream_writeLineNL    ed_iostream_writeLineNL




#if 0
#include <sys/param.h>
#include <errno.h>
#include <limits.h>
#include <stdlib.h>
#include <string.h>
#include <regex.h>
#include <signal.h>
#include <stdio.h>
#include <unistd.h>
#endif
#include "regex/ed_regex.h"

#ifndef EOF
#define EOF     (-1)
#endif
#define ERR     (-2)
#define EMOD        (-3)
#define FATAL       (-4)

#define PATH_MAX 512
#define MINBUFSZ 512        /* minimum buffer size - must be > 0 */
#define IBUFSZ   1024       /* input buffer size, line size */
#define SE_MAX 30       /* max subexpressions in a regular expression */
#define LINECHARS 8192  /* max chars per line */

/* gflags */
#define GLB 001     /* global command */
#define GPR 002     /* print after command */
#define GLS 004     /* list after command */
#define GNP 010     /* enumerate after command */
#define GSG 020     /* global substitute */

typedef regex_t pattern_t;

/* Line node */
typedef struct  line {
    struct line *q_forw;
    struct line *q_back;
    char    *s;     /* address of line in scratch buffer */
    int     len;        /* length of line */
} line_t;


typedef struct undo {

/* type of undo nodes */
#define UADD    0
#define UDEL    1
#define UMOV    2
#define VMOV    3

    int type;           /* command type */
    line_t  *h;         /* head of list */
    line_t  *t;         /* tail of list */
} undo_t;

#define MAXMARK 26          /* max number of marks */
typedef struct ed_data {
    /* ed_main.c */
    void *iostream;
    int garrulous; /* = 0;      if set, print all error messages */
    int isglobal;           /* if set, doing a global command */
    int modified;           /* if set, buffer modified since last write */
    const char *errmsg; /* = "" */
    long rows; /* = 22;     scroll length: ws_row - 2 */
    char ibuf[1024];            /* ed command-line buffer */
    char *ibufp;            /* pointer to ed command-line buffer */
    int patlock; /* = 0    if set, pattern not freed by get_compiled_pattern() */
    long addr_last;
    long current_addr;
    long first_addr;
    long second_addr;
    int lineno;         /* script line number */
    char old_filename[PATH_MAX]; /*  = ""    default filename */
    char tmp_filename[PATH_MAX]; /* */
    const char *prompt;     /* command-line prompt */
    const char *dps; /*  = "*"   default command-line prompt */
    long addr_cnt;
    char *join_buf; /* = NULL join_lines() buffer */
    int join_bufl; /* = 0 join_lines() buffer already allocated */
    line_t  *mark[MAXMARK];         /* line markers */
    int markno;             /* line marker count */

    pattern_t *pat; /*  = NULL; */
    int sgflag; /* = 0; */
    long sgnum; /* = 0; */

    /* ed_buf.c */
    line_t buffer_head;         /* incore buffer */
    unsigned char ctab[256];        /* character translation table */
    line_t *ga_lp; /* = &(ed->buffer_head); */
    long ga_on; /* = 0; */
    int translit_i; /* = 0 */
    /* ed_glbl.c */
    line_t **active_list;       /* list of lines active in a global command */
    long active_last;       /* index of last active line in active_list */
    long active_size;       /* size of active_list */
    long active_ptr;        /* active_list index (non-decreasing) */
    long active_ndx;        /* active_list index (modulo active_last) */
    char ocmd[IBUFSZ];
    /* ed_io.c */
    int newline_added;      /* if set, newline appended to input file */
    char sbuf[IBUFSZ]; /* buffer */
    char cvbuf[IBUFSZ]; /* buffer */
    /* ed_re.c */
    pattern_t *expr; /* = NULL; */
    char error[1024];
    char lhbuf[IBUFSZ]; /* buffer */
    /* ed_sub.c */
    char rhbuf[IBUFSZ];         /* rhs substitution buffer */
    int rhbufi;         /* rhs substitution buffer index */
    char rbuf[IBUFSZ];  /* substitute_matching_text buffer */
    /* ed_undo.c */
    undo_t *ustack; /* = NULL;              undo stack */
    long usize; /* = 0;                 stack size variable */
    long u_p; /*  = 0;                  undo stack pointer */
    long u_current_addr; /* = -1;    if >= 0, undo enabled */
    long u_addr_last; /*  = -1;      if >= 0, undo enabled */
} ed_data_t;

#ifndef max
# define max(a,b) ((a) > (b) ? (a) : (b))
#endif
#ifndef min
# define min(a,b) ((a) < (b) ? (a) : (b))
#endif

#define INC_MOD(l, k)   ((l) + 1 > (k) ? 0 : (l) + 1)
#define DEC_MOD(l, k)   ((l) - 1 < 0 ? (k) : (l) - 1)

/* SPL1: disable some interrupts (requires reliable signals) */
#define SPL1() 

/* SPL0: enable all interrupts; check sigflags (requires reliable signals) */
#define SPL0() 

/* STRTOL: convert a string to long */
#define STRTOL(i, p) { \
    i = ED_STRTOL(p, &p); \
}

#define REALLOC_C(b,i,err) \
if ((unsigned)(i) > sizeof(b)) { \
    ed->errmsg = "out of memory"; \
    SPL0(); \
    return err; \
}
/* REALLOC: assure at least a minimum size for buffer b */
#define REALLOC(b,n,i,err) \
if ((i) > (n)) { \
    int ti = (n); \
    char *ts; \
    SPL1(); \
    if ((b) != NULL) { \
        if ((ts = (char *) ED_REALLOC((b), ti += max((i), MINBUFSZ))) == NULL) { \
            ed->errmsg = "out of memory"; \
            SPL0(); \
            return err; \
        } \
    } else { \
        if ((ts = (char *) ED_MALLOC(ti += max((i), MINBUFSZ))) == NULL) { \
            ed->errmsg = "out of memory"; \
            SPL0(); \
            return err; \
        } \
    } \
    (n) = ti; \
    (b) = ts; \
    SPL0(); \
}

/* REQUE: link pred before succ */
#define REQUE(pred, succ) (pred)->q_forw = (succ), (succ)->q_back = (pred)

/* INSQUE: insert elem in circular queue after pred */
#define INSQUE(elem, pred) \
{ \
    REQUE((elem), (pred)->q_forw); \
    REQUE((pred), elem); \
}

/* REMQUE: remove_lines elem from circular queue */
#define REMQUE(elem) REQUE((elem)->q_back, (elem)->q_forw);

/* NUL_TO_NEWLINE: overwrite ASCII NULs with newlines */
#define NUL_TO_NEWLINE(s, l) translit_text(s, l, '\0', '\n')

/* NEWLINE_TO_NUL: overwrite newlines with ASCII NULs */
#define NEWLINE_TO_NUL(s, l) translit_text(s, l, '\n', '\0')

/* Local Function Declarations */
void add_line_node(ed_data_t *ed, line_t *);
int append_lines(ed_data_t *ed, long);
int apply_subst_template(ed_data_t *ed, const char *, regmatch_t *, int, int);
int build_active_list(ed_data_t *ed, int);
int check_addr_range(ed_data_t *ed, long, long);
void clear_active_list(ed_data_t *ed);
void clear_undo_stack(ed_data_t *ed);
int close_sbuf(ed_data_t *ed);
int copy_lines(ed_data_t *ed, long);
int delete_lines(ed_data_t *ed, long, long);
int display_lines(ed_data_t *ed, long, long, int);
line_t *dup_line_node(ed_data_t *ed, line_t *);
int exec_command(ed_data_t *ed);
long exec_global(ed_data_t *ed, int, int);
int extract_addr_range(ed_data_t *ed);
char *extract_pattern(ed_data_t *ed, int);
int extract_subst_tail(ed_data_t *ed, int *, long *);
char *extract_subst_template(ed_data_t *ed);
int filter_lines(ed_data_t *ed, long, long, char *);
void free_buffers(ed_data_t *ed);
line_t *get_addressed_line_node(ed_data_t *ed, long);
pattern_t *get_compiled_pattern(ed_data_t *ed);
char *get_extended_line(ed_data_t *ed, int *, int);
char *get_filename(ed_data_t *ed);
int get_keyword(ed_data_t *ed);
long get_line_node_addr(ed_data_t *ed, line_t *);
long get_matching_node_addr(ed_data_t *ed, pattern_t *, int);
long get_marked_node_addr(ed_data_t *ed, int);
char *get_sbuf_line(ed_data_t *ed, line_t *);
int get_shell_command(ed_data_t *ed);
int get_tty_line(ed_data_t *ed);
int has_trailing_escape(char *, char *);
int hex_to_binary(int, int);
int init_buffers(ed_data_t *ed);
int is_legal_filename(ed_data_t *ed, char *);
int join_lines(ed_data_t *ed, long, long);
int mark_line_node(ed_data_t *ed, line_t *, int);
int move_lines(ed_data_t *ed, long);
line_t *next_active_node(ed_data_t *ed);
long next_addr(ed_data_t *ed);
int open_sbuf(ed_data_t *ed);
char *parse_char_class(char *);
int pop_undo_stack(ed_data_t *ed);
undo_t *push_undo_stack(ed_data_t *ed, int, long, long);
const char *put_sbuf_line(ed_data_t *ed, const char *);
int put_tty_line(ed_data_t *ed, const char *, int, long, int);
long read_file(ed_data_t *ed, char *, long);
int search_and_replace(ed_data_t *ed, pattern_t *, int, int);
int set_active_node(ed_data_t *ed, line_t *);
char *strip_escapes(ed_data_t *ed, char *);
int substitute_matching_text(ed_data_t *ed, pattern_t *, line_t *, int, int);
char *translit_text(ed_data_t *ed, char *, int, int, int);
void unmark_line_node(ed_data_t *ed, line_t *);
void unset_active_nodes(ed_data_t *ed, line_t *, line_t *);
long write_file(ed_data_t *ed, char *, const char *, long, long);

int iostream_readline(ed_data_t *ed, char *bufferPtr, int bufferLength);
int iostream_write(ed_data_t *ed, const void *bufferPtr, int bufferLength);
int iostream_writeLine(ed_data_t *ed, const char *s);
int iostream_writeLineNL(ed_data_t *ed, const char *s);

