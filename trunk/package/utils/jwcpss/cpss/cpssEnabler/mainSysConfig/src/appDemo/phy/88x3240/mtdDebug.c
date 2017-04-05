#include "mtdCopyright.h" 
/* Copyright 2013 Marvell International Ltd, See included file for licensing information */

#ifdef MTD_DEBUG 
#include "mtdApiTypes.h"
#include "stdarg.h"
#include "stdio.h"

MTD_DBG_LEVEL mtd_debug_level = MTD_DBG_ALL_LVL; /* Set default debug level */

/*******************************************************************************
* mtdDbgPrint
*
* DESCRIPTION:
*       .
*
* INPUTS:
*       debug_level - allows control of how much is printed
*               MTD_DBG_OFF_LVL  - prints nothing
*               MTD_DBG_ERR_LVL - for error messages
*               MTD_DBG_ALL_LVL - for informational messages
*
* OUTPUTS:
*       None
*
* RETURNS:
*       None
*
* COMMENTS:
*       Set global mtd_debug_level to MTD_DBG_OFF_LVL to turn off all printing, or
*       MTD_DBG_ERR_LVL to print only errors, or MTD_DBG_ALL_LVL for both errors
*       and informational messages. This can be done during run time or
*       the global default can be changed at compile time to change the
*       run-time behavior.
*
*       Undefine MTD_DEBUG to remove all printing code (may need some modifications
*       on some compilers).
*
*******************************************************************************/
void mtdDbgPrint(FILED *stream, MTD_DBG_LEVEL debug_level, char* format, ...)
{
    va_list argP;
    char dbgStr[1000] = "";

    if (debug_level)
    {
        if (debug_level <= mtd_debug_level)
        {
            va_start(argP, format);

            vsprintf(dbgStr, format, argP);

            fprintf(stream, dbgStr);
        }
    }
}
#else 

/* All printing code removed */

#endif /* DEBUG_MTD */

