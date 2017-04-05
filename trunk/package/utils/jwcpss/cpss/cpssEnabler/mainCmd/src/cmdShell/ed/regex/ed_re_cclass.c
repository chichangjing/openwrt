#include "ed_regex.h"

/* character-class table */
static struct cclass {
	const char *name;
	char *chars;
} cclasses[] = {
	{ "alnum",	"ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789"},
	{ "alpha",	"ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz"},
	{ "blank",	" \t"},
	{ "cntrl",	"\007\b\t\n\v\f\r\1\2\3\4\5\6\16\17\20\21\22\23\24\25\26\27\30\31\32\33\34\35\36\37\177"},
	{ "digit",	"0123456789"},
	{ "graph",	"ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789!\"#$%&'()*+,-./:;<=>?@[\\]^_`{|}~"},
	{ "lower",	"abcdefghijklmnopqrstuvwxyz"},
	{ "print",	"ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789!\"#$%&'()*+,-./:;<=>?@[\\]^_`{|}~ "},
	{ "punct",	"!\"#$%&'()*+,-./:;<=>?@[\\]^_`{|}~"},
	{ "space",	"\t\n\v\f\r "},
	{ "upper",	"ABCDEFGHIJKLMNOPQRSTUVWXYZ"},
	{ "xdigit",	"0123456789ABCDEFabcdef"},
	{ NULL,		NULL }
};

int ed_re_iswctype(ed_re_wint_t wc, ed_re_wctype_t desc)
{
    if (ED_STRCHR(desc, wc))
        return 1;
    return 0;
}

ed_re_wctype_t ed_re_wctype(const char *name)
{
    struct cclass *c = cclasses;
    for (; c->name; c++)
    {
        if (ED_STRCMP(name, c->name) == 0)
            return (ed_re_wctype_t)(c->chars);
    }
    return (ed_re_wctype_t)NULL;
}
