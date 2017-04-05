/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* libcWrappers.c
*
* DESCRIPTION:
*       This file contains wrapper routines for libc functions which are used
*       by simulation directly to redirect all OS-dependent calls
*       to libhelper.so. 
*       Implementation of wrappers uses libc routines directly.
*
*       Wrappers are used automatically by linker instead of real functions
*       by '--wrap func_name' option for ld.
*       See http://www.cs.cmu.edu/afs/cs.cmu.edu/academic/class/15213-s03/
*           src/interposition/mymalloc.c for retails (LINKTIME approach).
*
*       Wrappers name should be __wrap_{func_name}
*
*       Relevant only for SHARED_MEMORY=1.
*
* FILE REVISION NUMBER:
*       $Revision: 2 $
*
*******************************************************************************/

/************* Includes *******************************************************/
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdarg.h>
#include <assert.h>
#include <errno.h>
#include <stdio.h>

#include <gtOs/gtOs.h>

/* Define this macro to enable wrapper logging */
/*#define LIBC_WRAPPERS_DEBUG_MAC*/

/************* Functions ******************************************************/
/*
stderr
*/
#if 0
/*******************************************************************************
* __wrap_access
*
* DESCRIPTION:
*       Wrapper function for libc access.
*
* INPUTS:
*       Same as for access.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       Same as for access.
*
* COMMENTS:
*       Redirects call to access.
*
*******************************************************************************/
int __wrap_access(const char *pathname, int mode)
{
#ifdef LIBC_WRAPPERS_DEBUG_MAC
    osPrintf("__wrap_access called with pathname=%s, mode=%d\n", pathname, mode);
#endif
    return access(pathname, mode);
}

/*******************************************************************************
* __wrap___assert_fail
*
* DESCRIPTION:
*       Wrapper function for libc __assert_fail.
*
* INPUTS:
*       Same as for __assert_fail.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       Same as for __assert_fail.
*
* COMMENTS:
*       Redirects call to __assert_fail.
*
*******************************************************************************/
void __wrap___assert_fail(__const char *__assertion, __const char *__file,
                           unsigned int __line, __const char *__function)
{
#ifdef LIBC_WRAPPERS_DEBUG_MAC
    osPrintf("__wrap___assert_fail called with __assertion=%s, __file=%s, __line=%d, __function=%s\n", 
             __assertion, __file, __line, __function);
#endif
    __assert_fail(__assertion, __file, __line, __function);
}

/*******************************************************************************
* __wrap___errno_location
*
* DESCRIPTION:
*       Wrapper function for libc __errno_location.
*
* INPUTS:
*       Same as for __errno_location.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       Same as for __errno_location.
*
* COMMENTS:
*       Redirects call to __errno_location.
*
*******************************************************************************/
int *__wrap___errno_location(void)
{
#ifdef LIBC_WRAPPERS_DEBUG_MAC
    osPrintf("__wrap___errno_location called\n");
#endif
    return __errno_location();
}

/*******************************************************************************
* __wrap_exit
*
* DESCRIPTION:
*       Wrapper function for libc exit.
*
* INPUTS:
*       Same as for exit.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       Same as for exit.
*
* COMMENTS:
*       Redirects call to exit.
*
*******************************************************************************/
void __wrap_exit(int status)
{
#ifdef LIBC_WRAPPERS_DEBUG_MAC
    osPrintf("__wrap_exit called with status=%d\n", status);
#endif
    exit(status);
}

/*******************************************************************************
* __wrap_fclose
*
* DESCRIPTION:
*       Wrapper function for libc fclose.
*
* INPUTS:
*       Same as for fclose.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       Same as for fclose.
*
* COMMENTS:
*       Redirects call to fclose.
*
*******************************************************************************/
int __wrap_fclose(FILE *stream)
{
#ifdef LIBC_WRAPPERS_DEBUG_MAC
    osPrintf("__wrap_fclose called with file stream\n");
#endif
    return fclose(stream);
}

/*******************************************************************************
* __wrap_fgets
*
* DESCRIPTION:
*       Wrapper function for libc fgets.
*
* INPUTS:
*       Same as for fgets.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       Same as for fgets.
*
* COMMENTS:
*       Redirects call to fgets.
*
*******************************************************************************/
char *__wrap_fgets(char *s, int size, FILE *stream)
{
#ifdef LIBC_WRAPPERS_DEBUG_MAC
    osPrintf("__wrap_fgets called with buffer s=%s, size=%d and file stream\n", s, size);
#endif
    return fgets(s, size, stream);
}

/*******************************************************************************
* __wrap_fopen
*
* DESCRIPTION:
*       Wrapper function for libc fopen.
*
* INPUTS:
*       Same as for fopen.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       Same as for fopen.
*
* COMMENTS:
*       Redirects call to fopen.
*
*******************************************************************************/
FILE *__wrap_fopen(const char *path, const char *mode)
{
#ifdef LIBC_WRAPPERS_DEBUG_MAC
    osPrintf("__wrap_fopen called with path=%s, mode=%s\n", path, mode);
#endif
    return fopen(path, mode);
}

/*******************************************************************************
* __wrap_fprintf
*
* DESCRIPTION:
*       Wrapper function for libc fprintf.
*
* INPUTS:
*       Same as for fprintf.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       Same as for fprintf.
*
* COMMENTS:
*       Redirects call to fprintf.
*
*******************************************************************************/
int __wrap_fprintf(FILE *stream, const char *format, ...)
{
#ifdef LIBC_WRAPPERS_DEBUG_MAC
    osPrintf("__wrap_fprintf called with file stream and format=%s\n", format);
#endif
    va_list args;
    int i;

    va_start(args, format);
    i = vfprintf(stream, format, args);
    va_end(args);

    return i;
}

/*******************************************************************************
* __wrap_qsort
*
* DESCRIPTION:
*       Wrapper function for libc qsort.
*
* INPUTS:
*       Same as for qsort.
*
* OUTPUTS:
*       Same as for qsort.
*
* RETURNS:
*       Same as for qsort.
*
* COMMENTS:
*       Redirects call to qsort.
*
*******************************************************************************/
void __wrap_qsort(void *base, size_t nmemb, size_t size,
                  int(*compar)(const void *, const void *))
{
#ifdef LIBC_WRAPPERS_DEBUG_MAC
    osPrintf("__wrap_qsort called with base=%x, nmemb=%d size=%d and pointer to function compar\n", 
             base, nmemb, size);
#endif
    qsort(base, nmemb, size, compar);
}

/*******************************************************************************
* __wrap_sprintf
*
* DESCRIPTION:
*       Wrapper function for libc sprintf.
*
* INPUTS:
*       Same as for sprintf.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       Same as for sprintf.
*
* COMMENTS:
*       Redirects call to osSprintf.
*
*******************************************************************************/
int __wrap_sprintf(char * buffer, const char* format, ...)
{
#ifdef LIBC_WRAPPERS_DEBUG_MAC
    osPrintf("__wrap_sprintf called with format='%s'\n", format);
#endif
    va_list args;
    int i;

    va_start(args, format);
    i = vsprintf(buffer, format, args);
    va_end(args);

    return i;
}

/*******************************************************************************
* __wrap_sscanf
*
* DESCRIPTION:
*       Wrapper function for libc sscanf.
*
* INPUTS:
*       Same as for sscanf.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       Same as for sscanf.
*
* COMMENTS:
*       Redirects call to sscanf.
*
*******************************************************************************/
int __wrap_sscanf(const char *str, const char *format, ...)
{
#ifdef LIBC_WRAPPERS_DEBUG_MAC
    osPrintf("__wrap_sscanf called with str='%s', format='%s'\n", str, format);
#endif
    va_list args;
    int i;

    va_start(args, format);
    i = vsscanf(str, format, args);
    va_end(args);

    return i;
}

/*******************************************************************************
* __wrap_strncmp
*
* DESCRIPTION:
*       Wrapper function for libc strncmp.
*
* INPUTS:
*       Same as for strncmp.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       Same as for strncmp.
*
* COMMENTS:
*       Redirects call to strncmp.
*
*******************************************************************************/
int __wrap_strncmp(const char *s1, const char *s2, size_t n)
{
#ifdef LIBC_WRAPPERS_DEBUG_MAC
    osPrintf("__wrap_strncmp called with s1=%s, s2=%s, n=%d\n", s1, s2, n);
#endif
    return strncmp(s1, s2, n);
}

/*******************************************************************************
* __wrap_strncpy
*
* DESCRIPTION:
*       Wrapper function for libc strncpy.
*
* INPUTS:
*       Same as for strncpy.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       Same as for strncpy.
*
* COMMENTS:
*       Redirects call to strncpy.
*
*******************************************************************************/
char *__wrap_strncpy(char *dest, const char *src, size_t n)
{
#ifdef LIBC_WRAPPERS_DEBUG_MAC
    osPrintf("__wrap_strncpy called with src=%s, n=%d\n", src, n);
#endif
    return strncpy(dest, src, n);
}

/*******************************************************************************
* __wrap_strrchr
*
* DESCRIPTION:
*       Wrapper function for libc strrchr.
*
* INPUTS:
*       Same as for strrchr.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       Same as for strrchr.
*
* COMMENTS:
*       Redirects call to strrchr.
*
*******************************************************************************/
char *__wrap_strrchr(const char *s, int c)
{
#ifdef LIBC_WRAPPERS_DEBUG_MAC
    osPrintf("__wrap_strrchr called with s=%s, c=%d\n", s, c);
#endif
    return strrchr(s, c);
}

/*******************************************************************************
* __wrap_strstr
*
* DESCRIPTION:
*       Wrapper function for libc strstr.
*
* INPUTS:
*       Same as for strstr.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       Same as for strstr.
*
* COMMENTS:
*       Redirects call to strstr.
*
*******************************************************************************/
char *__wrap_strstr(const char *haystack, const char *needle)
{
#ifdef LIBC_WRAPPERS_DEBUG_MAC
    osPrintf("__wrap_strstr called with haystack=%s, needle=%s\n", haystack, needle);
#endif
    return strstr(haystack, needle);
}

/*******************************************************************************
* __wrap_strtok
*
* DESCRIPTION:
*       Wrapper function for libc strtok.
*
* INPUTS:
*       Same as for strtok.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       Same as for strtok.
*
* COMMENTS:
*       Redirects call to strtok.
*
*******************************************************************************/
char *__wrap_strtok(char *s, const char *delim)
{
#ifdef LIBC_WRAPPERS_DEBUG_MAC
    osPrintf("__wrap_strtok called with s=%s, delim=%s\n", s, delim);
#endif
    return strtok(s, delim);
}

/*******************************************************************************
* __wrap_system
*
* DESCRIPTION:
*       Wrapper function for libc system.
*
* INPUTS:
*       Same as for system.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       Same as for system.
*
* COMMENTS:
*       Redirects call to system.
*
*******************************************************************************/
int __wrap_system(const char * string)
{
#ifdef LIBC_WRAPPERS_DEBUG_MAC
    osPrintf("__wrap_system called with string=%s\n", string);
#endif
    return system(string);
}

/*******************************************************************************
* __wrap_vsprintf
*
* DESCRIPTION:
*       Wrapper function for libc vsprintf.
*
* INPUTS:
*       Same as for vsprintf.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       Same as for vsprintf.
*
* COMMENTS:
*       Redirects call to vsprintf.
*
*******************************************************************************/
int __wrap_vsprintf(char *str, const char *format, va_list ap)
{
#ifdef LIBC_WRAPPERS_DEBUG_MAC
    osPrintf("__wrap_vsprintf called with format=%s\n", format);
#endif
    return vsprintf(str, format, ap);
}
#endif

