#include "ed_env.h"

void* ED_MEMMOVE(void* dest, void *src, ed_size_t n)
{
    if (dest == NULL || src == NULL)
        return dest;
    if (dest == src)
        return dest;
    if ((GT_UINTPTR)dest < (GT_UINTPTR)src)
        return cmdOsMemCpy(dest,src,(GT_U32)n);
    if ((GT_UINTPTR)dest > (GT_UINTPTR)src + n)
        return cmdOsMemCpy(dest,src,(GT_U32)n);
    for (;n;n--)
        ((char*)dest)[n-1] = ((char*)src)[n-1];
    return dest;
}
char* ED_STRNCPY(char* dest, const char *src, ed_size_t n)
{
    char *ret = dest;
    if (dest == NULL || src == NULL)
        return ret;
    for (;*src && n; n--)
        *dest++ = *src++;
    if (n)
        *dest = 0;
    return ret;
}
int ED_STRNCMP(const char* s1, const char *s2, ed_size_t n)
{
    if (s1 == NULL || s2 == NULL)
        return 0;
    for (;*s1 && *s2 && n; s1++, s2++, n--)
    {
        if (*s1 != *s2)
            break;
    }
    if (*s1 > *s2)
        return 1;
    if (*s1 < *s2)
        return -1;
    return 0;
}

int ED_ISSPACE(unsigned char ch)
{
    return ED_STRCHR(" \t\n\r", ch) == NULL ? 0 : 1;
}

int ED_ISDIGIT(unsigned char ch)
{
    return (ch >= '0' && ch <= '9') ? 1 : 0;
}
int ED_ISALPHA(unsigned char ch)
{
    if (ch >= 'a' && ch <= 'z')
        return 1;
    return (ch >= 'A' && ch <= 'Z') ? 1 : 0;
}
int ED_ISALNUM(unsigned char ch)
{
    if (ch >= 'a' && ch <= 'z')
        return 1;
    if (ch >= 'A' && ch <= 'Z')
        return 1;
    return (ch >= '0' && ch <= '9') ? 1 : 0;
}
int ED_ISUPPER(unsigned char ch)
{
    return (ch >= 'A' && ch <= 'Z') ? 1 : 0;
}
int ED_ISLOWER(unsigned char ch)
{
    return (ch >= 'a' && ch <= 'z') ? 1 : 0;
}
int ED_TOLOWER(unsigned char ch)
{
    if (ch >= 'A' && ch <= 'Z')
        return ch - 'A' + 'a';
    return ch;
}
int ED_TOUPPER(unsigned char ch)
{
    if (ch >= 'a' && ch <= 'z')
        return ch - 'a' + 'A';
    return ch;
}
long ED_STRTOL(char *str, char **p)
{
    long ret = 0;
    while(ED_ISSPACE(*str))
        str++;
    for (; ED_ISDIGIT(*str); str++)
    {
        ret = ret * 10 + ((*str) - '0');
    }
    
    if (p)
        *p = str;
    return ret;
}
