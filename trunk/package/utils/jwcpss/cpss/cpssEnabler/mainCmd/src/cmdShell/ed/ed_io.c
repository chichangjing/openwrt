/* io.c: This file contains the i/o routines for the ed line editor */
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

#ifdef ENV_MAINCMD
#include <cmdShell/FS/cmdFS.h>
#else
#include <cmdFS.h>
#endif


static long read_stream(ed_data_t *ed, int fd, long n);
static int get_stream_line(ed_data_t *ed, int fd);
static long write_stream(ed_data_t *ed, int fd, long n, long m);
static int put_stream_line(ed_data_t *ed, int fd, const char *s, int len);

/* read_file: read a named file/pipe into the buffer; return line count */
long
read_file(ed_data_t *ed, char *fn, long n)
{
    int fd;
    long size;


    fd = cmdFS.open(fn, GEN_FS_O_RDONLY);
    if (fd < 0) {
        ed->errmsg = "cannot open input file";
        return ERR;
    }
    size = read_stream(ed, fd, n);
    cmdFS.close(fd);
    if (size  < 0)
        return ERR;
    return ed->current_addr - n;
}


/* read_stream: read a stream into the editor buffer; return status */
static long
read_stream(ed_data_t *ed, int fd, long n)
{
    line_t *lp = get_addressed_line_node(ed, n);
    undo_t *up = NULL;
    unsigned long size = 0;
    int o_newline_added = ed->newline_added;
    int appended = (n == ed->addr_last);
    int len;

    for (ed->current_addr = n; (len = get_stream_line(ed, fd)) > 0; size += len) {
        SPL1();
        if (put_sbuf_line(ed, ed->sbuf) == NULL) {
            SPL0();
            return ERR;
        }
        lp = lp->q_forw;
        if (up)
            up->t = lp;
        else if ((up = push_undo_stack(ed, UADD, ed->current_addr,
            ed->current_addr)) == NULL) {
            SPL0();
            return ERR;
        }
        SPL0();
    }
    if (len < 0)
        return ERR;
    if (!size)
        ed->newline_added = 1;
    ed->newline_added = appended ? ed->newline_added : o_newline_added;
    return size;
}


/* get_stream_line: read a line of text from a stream; return line length */
static int
get_stream_line(ed_data_t *ed, int fd)
{
    char c;
    int i = 0;
    int ret = 0;

    while (1)
    {
        ret = cmdFS.read(fd, &c, 1);
        if (ret <= 0 || c == '\n')
            break;
        REALLOC_C(ed->sbuf, i + 1, ERR);
        ed->sbuf[i++] = c;
    }
    REALLOC_C(ed->sbuf, i + 2, ERR);
    if (ret > 0 && c == '\n')
        ed->sbuf[i++] = c;
    else if (i) {
        ed->sbuf[i++] = '\n';
        ed->newline_added = 1;
    }
    ed->sbuf[i] = '\0';
    return (ed->newline_added && i) ? --i : i;
}


/* write_file: write a range of lines to a named file/pipe; return line count */
long
write_file(ed_data_t *ed, char *fn, const char *mode GT_UNUSED, long n, long m)
{
    int fd;
    long size;

    fd = cmdFS.open(fn, GEN_FS_O_WRONLY|GEN_FS_O_CREAT|GEN_FS_O_TRUNC);
    if (fd < 0) {
        ed->errmsg = "cannot open output file";
        return ERR;
    }
    size = write_stream(ed, fd, n, m);
    cmdFS.close(fd);
    if (size < 0)
        return ERR;
    return n ? m - n + 1 : 0;
}


/* write_stream: write a range of lines to a stream; return status */
static long
write_stream(ed_data_t *ed, int fd, long n, long m)
{
    line_t *lp = get_addressed_line_node(ed, n);
    unsigned long size = 0;
    char *s;
    int len;

    for (; n && n <= m; n++, lp = lp->q_forw) {
        if ((s = get_sbuf_line(ed, lp)) == NULL)
            return ERR;
        len = lp->len;
        if (n != ed->addr_last || !ed->newline_added)
            s[len++] = '\n';
        if (put_stream_line(ed, fd, s, len) < 0)
            return ERR;
        size += len;
    }
    return size;
}


/* put_stream_line: write a line of text to a stream; return status */
static int
put_stream_line(ed_data_t *ed, int fd, const char *s, int len)
{
    if (cmdFS.write(fd, s, len) < len)
    {
            ed->errmsg = "cannot write file";
            return ERR;
    }
    return 0;
}

/* get_extended_line: get an extended line from stdin */
char *
get_extended_line(ed_data_t *ed, int *sizep, int nonl)
{
    int l, n;
    char *t = ed->ibufp;

    while (*t++ != '\n')
        ;
    if ((l = (int)(t - ed->ibufp)) < 2 || !has_trailing_escape(ed->ibufp, ed->ibufp + l - 1)) {
        *sizep = l;
        return ed->ibufp;
    }
    *sizep = -1;
    REALLOC_C(ed->cvbuf, l, NULL);
    ED_MEMCPY(ed->cvbuf, ed->ibufp, l);
    *(ed->cvbuf + --l - 1) = '\n';  /* strip trailing esc */
    if (nonl) l--;          /* strip newline */
    for (;;) {
        if ((n = get_tty_line(ed)) < 0)
            return NULL;
        else if (n == 0 || ed->ibuf[n - 1] != '\n') {
            ed->errmsg = "unexpected end-of-file";
            return NULL;
        }
        REALLOC_C(ed->cvbuf, l + n, NULL);
        ED_MEMCPY(ed->cvbuf + l, ed->ibuf, n);
        l += n;
        if (n < 2 || !has_trailing_escape(ed->cvbuf, ed->cvbuf + l - 1))
            break;
        *(ed->cvbuf + --l - 1) = '\n';  /* strip trailing esc */
        if (nonl) l--;          /* strip newline */
    }
    REALLOC_C(ed->cvbuf, l + 1, NULL);
    ed->cvbuf[l] = '\0';
    *sizep = l;
    return ed->cvbuf;
}


/* get_tty_line: read a line of text from stdin; return line length */
int
get_tty_line(ed_data_t *ed)
{
    int i = 0;

    i = iostream_readline(ed, ed->ibuf, sizeof(ed->ibuf));
    ed->ibuf[i] = 0;
    ed->ibufp = ed->ibuf;
    if (i < 0)
        return ERR;
    ed->lineno++;
    return i;
}



#define ESCAPES "\a\b\f\n\r\t\v\\"
#define ESCCHARS "abfnrtv\\"

/* put_tty_line: print text to stdout */
int
put_tty_line(ed_data_t *ed, const char *s, int l, long n, int gflag)
{
    char *cp;

    if (gflag & GNP) {
        char s[32];
        ED_SPRINTF(s, "%ld\t", n);
        iostream_writeLine(ed, s);
    }
#define ed_putchar(ch) { char c = ch; iostream_write(ed, &c, 1); }
    for (; l--; s++) {
        if (gflag & GLS) {
            if (31 < *s && *s < 127 && *s != '\\') {
                ed_putchar(*s);
            }
            else {
                ed_putchar('\\');
                if (*s && (cp = ED_STRCHR(ESCAPES, *s)) != NULL) {
                    ed_putchar(ESCCHARS[cp - ESCAPES]);
                }
                else {
                    ed_putchar((((unsigned char) *s & 0300) >> 6) + '0');
                    ed_putchar((((unsigned char) *s & 070) >> 3) + '0');
                    ed_putchar(((unsigned char) *s & 07) + '0');
                }
            }

        } else
            ed_putchar(*s);
    }
    ed_putchar('\n');
#undef ed_putchar
    return 0;
}
