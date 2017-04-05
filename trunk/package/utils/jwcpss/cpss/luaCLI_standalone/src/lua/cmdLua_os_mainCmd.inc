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

#include <cmdShell/common/cmdCommon.h>
#include <cmdShell/os/cmdExecute.h>
#ifdef USE_CLI_MALLOC
#include <cmdShell/lua/cliMemLib.h>
#endif
#include <cpss/extServices/private/prvCpssBindFunc.h>
#include <gtOs/gtOsSem.h>
#include <gtOs/gtOsStr.h>

#ifdef USE_CLI_MALLOC
#define LUACLI_MALLOC(len)          cliMalloc((GT_U32)(len))
#define LUACLI_MALLOC_F             cliMalloc
#define LUACLI_FREE                 cliFree
#else
#define LUACLI_MALLOC(len)          cmdOsMalloc((GT_U32)(len))
#define LUACLI_MALLOC_F             cmdOsMalloc
#define LUACLI_FREE                 cmdOsFree
#endif
#define LUACLI_MEMCPY(d,s,l)        cmdOsMemCpy((d),(s),(GT_U32)(l))
#define LUACLI_MEMCMP               cmdOsMemCmp
#define LUACLI_MEMSET               cmdOsMemSet
#define LUACLI_SPRINTF              cmdOsSprintf
#define LUACLI_STRCMP               cpssOsStrCmp
#define LUACLI_STRNCMP(a,b,l)       cpssOsStrNCmp((a),(b),(GT_U32)(l))
#define LUACLI_GETFUNCTIONBYNAME    osShellGetFunctionByName

#define LUACLI_MUTEX_TYPE           CPSS_OS_MUTEX
#define LUACLI_MUTEX_INITIALIZER    ((CPSS_OS_MUTEX)0)
#define LUACLI_MUTEX_CREATE(nm,pmtx)   cmdOsMutexCreate(nm,pmtx)
#define LUACLI_MUTEX_LOCK(mtx)         cmdOsMutexLock(mtx)
#define LUACLI_MUTEX_UNLOCK(mtx)       cmdOsMutexUnlock(mtx)

#define LUACLI_U32                  GT_U32
#define LUACLI_UINTPTR              GT_UINTPTR
#define LUACLI_RETSTATUS            GT_STATUS
