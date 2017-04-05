#include <Copyright.h>
/********************************************************************************
* debug.c
*
* DESCRIPTION:
*       Debug message display routine
*
* DEPENDENCIES:
*       OS Dependent
*
* FILE REVISION NUMBER:
*       $Revision: 3 $
*******************************************************************************/
#include <msApi.h>

#ifdef DEBUG_QD
#ifdef _VXWORKS
#include "vxWorks.h"
#include "logLib.h"
#include "stdarg.h"
#include "stdio.h"
#elif defined(WIN32)
#include "windows.h"
/* #include "wdm.h" */
#elif defined(LINUX)
 #ifdef __KERNEL__
  #include "kernel.h"
 #else
  #include "stdio.h"
  #include "stdarg.h"
 #endif
#endif

/*******************************************************************************
* gtDbgPrint
*
* DESCRIPTION:
*       .
*
* INPUTS:
*       None
*
* OUTPUTS:
*       None
*
* RETURNS:
*
* COMMENTS:
*       None
*
*******************************************************************************/
#if defined(_VXWORKS) || defined(WIN32) || defined(LINUX)
void gtDbgPrint(const char* format, ...)
{
    va_list argP;
    char dbgStr[1000] = "";

    va_start(argP, format);

    vsprintf(dbgStr, format, argP);

#ifdef _VXWORKS
    printf("%s",dbgStr);
/*    logMsg(dbgStr,0,1,2,3,4,5); */
#elif defined(WIN32)
    printf("%s",dbgStr);
/*    DbgPrint(dbgStr);*/
#elif defined(LINUX)
 #ifdef __KERNEL__
    printk("%s",dbgStr);
 #else
    printf("%s",dbgStr);
 #endif
#endif
    return;
}
#else
void gtDbgPrint(char* format, ...)
{
    GT_UNUSED_PARAM(format);
}
#endif
#else /* DEBUG_QD not defined */
void gtDbgPrint(const char* format, ...)
{
    GT_UNUSED_PARAM(format);
}
#endif /* DEBUG_QD */

