/*******************************************************************************
*              Copyright 2001, GALILEO TECHNOLOGY, LTD.
*
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL. NO RIGHTS ARE GRANTED
* HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT OF MARVELL OR ANY THIRD
* PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE DISCRETION TO REQUEST THAT THIS
* CODE BE IMMEDIATELY RETURNED TO MARVELL. THIS CODE IS PROVIDED "AS IS".
* MARVELL MAKES NO WARRANTIES, EXPRESS, IMPLIED OR OTHERWISE, REGARDING ITS
* ACCURACY, COMPLETENESS OR PERFORMANCE. MARVELL COMPRISES MARVELL TECHNOLOGY
* GROUP LTD. (MTGL) AND ITS SUBSIDIARIES, MARVELL INTERNATIONAL LTD. (MIL),
* MARVELL TECHNOLOGY, INC. (MTI), MARVELL SEMICONDUCTOR, INC. (MSI), MARVELL
* ASIA PTE LTD. (MAPL), MARVELL JAPAN K.K. (MJKK), GALILEO TECHNOLOGY LTD. (GTL)
* AND GALILEO TECHNOLOGY, INC. (GTI).
********************************************************************************
* os_lxExecute.c
*
* DESCRIPTION:
*       vxworks extended system wrapper layer
*
*       this module implements pipe io and standard io redirection routines
*
* DEPENDENCIES:
*
* FILE REVISION NUMBER:
*       $Revision: 10 $
*******************************************************************************/

/***** Include files ***************************************************/
/*#include <prestera/os/gtOs.h>*/
#include <sys/ioctl.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#if 1
/* No dynamic libarrys in uCLinux */
#include <dlfcn.h>
#endif
#include <cmdShell/os/cmdExecute.h>

#if 0
typedef struct
{
    char * name_PTR;
    void (*function_PTR)();
} CMD_ENTRY_TYP;
#define FUNC(NAME) extern void NAME(void)
#define COMMAND(NAME) {#NAME, MAME}
FUNC(cpssInitSystem);
static CMD_ENTRY_TYP commands[] =  {COMMAND(cpssInitSystem)};
#endif

typedef int (*FUNC_PTR)();

/*******************************************************************************
* osShellGetFunctionByName
*
* DESCRIPTION:
*       Lookup executable file symbol table for function
*
* INPUTS:
*       funcName - null terminated command string
*
* OUTPUTS:
*       None
*
* RETURNS:
*       NULL        - if function is not found
*       function pointer
*
* COMMENTS:
*       None
*
*******************************************************************************/
#ifndef STATIC_LINKED_APPDEMO
GT_VOIDFUNCPTR osShellGetFunctionByName
(
    IN  const char* funcName
)
{
    return (GT_VOIDFUNCPTR)dlsym(/* RTLD_DEFAULT */ ((void *) 0), (char*)funcName);
}
#else /* defined(STATIC_LINKED_APPDEMO) */
typedef struct {
    const char *name;
    GT_VOIDFUNCPTR funcPtr;
} SYMTBL_ENTRY_STC;

extern SYMTBL_ENTRY_STC __SymbolTable[];
extern int              __SymbolTableLength;

GT_VOIDFUNCPTR osShellGetFunctionByName
(
    IN  const char* funcName
)
{
    int iFirst = 0;
    int iLast = __SymbolTableLength;
    int iMiddle, iRet;

    if (funcName == NULL)
        return (GT_VOIDFUNCPTR)NULL;

    while (iLast >= iFirst)
    {
        /*iMiddle = (iLast + iFirst) / 2;*/
        iMiddle = iFirst + (iLast - iFirst) / 2;
        if (iMiddle >= __SymbolTableLength || iMiddle < 0)
            return (GT_VOIDFUNCPTR)NULL;

        iRet = strcmp(__SymbolTable[iMiddle].name, funcName);

        if (iRet == 0)
            return __SymbolTable[iMiddle].funcPtr;
        if (iRet > 0)
            iLast = iMiddle - 1;
        else
            iFirst = iMiddle + 1;
    }

    return (GT_VOIDFUNCPTR)NULL;
}
#endif

/*******************************************************************************
* osShellParseCmdLine
*
* DESCRIPTION:
*       parse command line
*
* INPUTS:
*       s - null terminated command string, will be modified
*
* OUTPUTS:
*       cmd
*
* RETURNS:
*       GT_OK       - on success
*       GT_FAIL     - on error
*
* COMMENTS:
*       None
*
*******************************************************************************/
static GT_STATUS osShellParseCmdLine
(
    INOUT   char    *s,
    OUT     char*   *cmd,
    OUT     GT_UINTPTR argv[],
    OUT     GT_32  *argc
)
{
    enum {
        ST_SPACE,
        ST_NUMERIC,
        ST_STRING,
        ST_STRING_Q,
        ST_STRING_HEX0,
        ST_STRING_HEX1
    } state = ST_SPACE;
    char *tok_begin = s;
    char *tok_end = s;
    GT_U32  number = 0;
    

    *argc = 0;
    /* skip leading spaces */
    while (*s == ' ' || *s == '\t')
        s++;

    /* get command name */
    *cmd = s;
    while (s && strchr(" \t\n,", *s) == NULL)
        s++;
    if (*s == 0)
    {
        return GT_OK;
    }
    *s++ = 0;
    
    /* parse parameters */
    for (; *s; s++)
    {
        switch (state)
        {
            case ST_SPACE:
                if (strchr(" \t\n,", *s) != NULL)
                    continue;
                argv[*argc] = 0;
                (*argc)++;
                if (*s == '"')
                {
                    state = ST_STRING;
                    tok_begin = s;
                    tok_end = s;
                    argv[*argc-1] = (GT_UINTPTR)tok_begin;
                }
                else
                {
                    state = ST_NUMERIC;
                    tok_begin = s;
                }
                continue;
            case ST_NUMERIC:
                if (strchr(" \t\n,", *s) != NULL)
                {
                    *s = 0;
                    argv[*argc-1] = strtoul(tok_begin, 0, 0);
                    state = ST_SPACE;
                    continue;
                }
                continue;
            case ST_STRING:
                if (*s == '"')
                {
                    *tok_end = 0;
                    state = ST_SPACE;
                    continue;
                }
                if (*s == '\\')
                {
                    state = ST_STRING_Q;
                    continue;
                }
                *tok_end++ = *s;
                continue;
            case ST_STRING_Q:
                switch (*s)
                {
                    case 'x': state = ST_STRING_HEX0; number = 0; continue;
                    case 'r': *tok_end++ = '\r'; break;
                    case 't': *tok_end++ = '\t'; break;
                    case 'n': *tok_end++ = '\n'; break;
                    default:
                        *tok_end++ = *s;
                }
                state = ST_STRING;
                continue;
            case ST_STRING_HEX0:
                if (strchr("0123456789abcdef", *s) == NULL)
                {
                    *tok_end++ = (char)number;
                    state = ST_STRING;
                    continue;
                }
                number = (*s > '9')
                    ? (*s + 10 - 'a')
                    : (*s - '0');
                state = ST_STRING_HEX1;
                continue;
            case ST_STRING_HEX1:
                if (strchr("0123456789abcdef", *s) != NULL)
                {
                    number *= 16;
                    number += (*s > '9')
                        ? (*s + 10 - 'a')
                        : (*s - '0');
                }
                *tok_end++ = (char)number;
                state = ST_STRING;
                continue;
        }
    }
    if (state == ST_SPACE)
        return GT_OK;

    if (state == ST_NUMERIC)
    {
        argv[*argc-1] = strtoul(tok_begin, 0, 0);
    }
    else
    {
        /* string */
        *tok_end = 0;
    }
    return GT_OK;
}

/*******************************************************************************
* osShellExecute
*
* DESCRIPTION:
*       execute command through OS shell
*
* INPUTS:
*       command - null terminated command string
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK       - on success
*       GT_FAIL     - on error
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS   osShellExecute
(
    IN  char*   command
)
{
    FUNC_PTR func;
    char copy[128];
    char  * cmd;
    GT_UINTPTR argv[32];
    GT_32 argc;
    GT_STATUS rc;

    strncpy(copy, command, 127);
    copy[127] = '\0';
    if (osShellParseCmdLine(copy, &cmd, argv, &argc) != GT_OK)
    {
        return GT_FAIL;
    }

    func = (FUNC_PTR) osShellGetFunctionByName((char*)cmd);
    if (0 == func)
    {
        return GT_FAIL;
    }
    switch (argc)
    {
        case 0:
            rc = (func)();
            break;
        case 1:
            rc = (func)(argv[0]);
            break;
        case 2:
            rc = (func)(argv[0],argv[1]);
            break;
        case 3:
            rc = (func)(argv[0],argv[1],argv[2]);
            break;
        case 4:
            rc = (func)(argv[0],argv[1],argv[2],argv[3]);
            break;
        case 5:
            rc = (func)(argv[0],argv[1],argv[2],argv[3],argv[4]);
            break;
        case 6:
            rc = (func)(argv[0],argv[1],argv[2],argv[3],argv[4],argv[5]);
            break;
        case 7:
            rc = (func)(argv[0],argv[1],argv[2],argv[3],argv[4],argv[5],argv[6]);
            break;
        case 8:
            rc = (func)(argv[0],argv[1],argv[2],argv[3],argv[4],argv[5],argv[6],
                    argv[7]);
            break;
        case 9:
            rc = (func)(argv[0],argv[1],argv[2],argv[3],argv[4],argv[5],argv[6],
                    argv[7],argv[8]);
            break;
        case 10:
            rc = (func)(argv[0],argv[1],argv[2],argv[3],argv[4],argv[5],argv[6],
                    argv[7],argv[8],argv[9]);
            break;
        default:
            return GT_FAIL;
    }
    cmdOsPrintf("return code is %d\n", rc);
    return rc;
}
