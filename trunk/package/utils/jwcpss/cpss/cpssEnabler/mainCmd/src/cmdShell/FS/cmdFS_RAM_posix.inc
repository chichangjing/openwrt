/****************************************************
* cmdFS_RAM_posix.h
*
* DESCRIPTION:
*       The micro layer for mainCmd environment
*       This file just included in cmdFS_RAM.c
*
* DEPENDENCIES:
*
* COMMENTS:
*
* FILE REVISION NUMBER:
*       $Revision: 4 $
*******************************************************************************/
#define _GNU_SOURCE
#include <inttypes.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#ifndef CMDFS_RAM_TYPES_ONLY

#define CMD_FS_STRCMP(a,b)    strcmp(a,b)
#define CMD_FS_STRDUP(a)      strdup(a)
#define CMD_FS_STRNCPY(d,s,n) strncpy(d,s,n)
#define CMD_FS_BZERO(p,l)     memset(p,0,l)
#define CMD_FS_BCOPY(s,d,l)   memcpy(d,s,l)
#define CMD_FS_MALLOC(l)      malloc(l)
#define CMD_FS_FREE(l)        free(l)
#define CMD_FS_TIME()         ((unsigned)time(NULL))
#define CMD_FS_CONST_CAST(_a,_t) ((_t)((void*)((uintptr_t)_a)))

#if !defined(GT_UNUSED) && defined(__GNUC__)
# define GT_UNUSED __attribute__ ((unused))
#endif
#ifndef GT_UNUSED
# define GT_UNUSED
#endif

#endif /* CMDFS_RAM_TYPES_ONLY */
