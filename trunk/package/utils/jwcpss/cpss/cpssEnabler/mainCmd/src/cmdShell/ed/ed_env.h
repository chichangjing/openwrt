#ifndef __ED_ENV_H__
#define __ED_ENV_H__

/* environment specific definitions */
#include <cmdShell/common/cmdExtServices.h>

typedef GT_PTRDIFF ed_off_t;
typedef GT_SIZE_T ed_size_t;

#define ED_MALLOC   cmdOsMalloc
#define ED_FREE     cmdOsFree
#define ED_REALLOC  cmdOsRealloc
#define ED_MEMCPY   cmdOsMemCpy
#define ED_MEMCMP   cmdOsMemCmp
#define ED_MEMSET   cmdOsMemSet
void* ED_MEMMOVE(void* dest, void *src, ed_size_t n);
#define ED_STRLEN   cmdOsStrlen
#define ED_STRCPY   cmdOsStrCpy
#define ED_STRCMP   cmdOsStrCmp
char* ED_STRNCPY(char* dest, const char *src, ed_size_t n);
int ED_STRNCMP(const char* s1, const char *s2, ed_size_t n);
#define ED_STRCHR   cmdOsStrChr
#define ED_SPRINTF  cmdOsSprintf
int ED_ISSPACE(unsigned char ch);
int ED_ISDIGIT(unsigned char ch);
int ED_ISALPHA(unsigned char ch);
int ED_ISALNUM(unsigned char ch);
int ED_ISUPPER(unsigned char ch);
int ED_ISLOWER(unsigned char ch);
int ED_TOLOWER(unsigned char ch);
int ED_TOUPPER(unsigned char ch);
long ED_STRTOL(char *str, char **p);

#endif /* __ED_ENV_H__ */
