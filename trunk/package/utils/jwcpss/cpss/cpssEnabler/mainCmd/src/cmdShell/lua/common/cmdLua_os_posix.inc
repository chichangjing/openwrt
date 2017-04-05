/****************************************************
* cmdLua_os_mainCmd.inc
*
* DESCRIPTION:
*       The micro layer for mainCmd environment
*       This file just included in luaCLI components
*
* DEPENDENCIES:
*
* COMMENTS:
*
* FILE REVISION NUMBER:
*       $Revision: 3 $
*******************************************************************************/

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <inttypes.h>
#include <stddef.h>
#include <dlfcn.h>

#ifdef USE_CLI_MALLOC
#include <cmdShell/lua/cliMemLib.h>
#define LUACLI_MALLOC(len)          cliMalloc((GT_U32)(len))
#define LUACLI_FREE                 cliFree
#else
#define LUACLI_MALLOC               malloc
#define LUACLI_FREE                 free
#endif
#define LUACLI_MEMCPY               memcpy
#define LUACLI_MEMCMP               memcmp
#define LUACLI_MEMSET               memset
#define LUACLI_SPRINTF              sprintf
#define LUACLI_STRCMP               strcmp
#define LUACLI_STRNCMP              strncmp
#define LUACLI_GETFUNCTIONBYNAME(name)    dlsym((void*)0, name)

/* stub */
#define LUACLI_MUTEX_TYPE           int
#define LUACLI_MUTEX_INITIALIZER    0
#define LUACLI_MUTEX_CREATE(nm,pmtx)   
#define LUACLI_MUTEX_LOCK(mtx)         
#define LUACLI_MUTEX_UNLOCK(mtx)       

#define LUACLI_U32                  unsigned
#define LUACLI_UINTPTR              uintptr_t
#define LUACLI_RETSTATUS            int
