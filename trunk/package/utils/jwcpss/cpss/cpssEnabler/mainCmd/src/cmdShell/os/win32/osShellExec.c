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
* osShellExec.c
*
* DESCRIPTION:
*       win32 microsoft imageHelp library basedf shell
*
* DEPENDENCIES:
*
* FILE REVISION NUMBER:
*       $Revision: 12 $
*******************************************************************************/

/***** Include files ***************************************************/

/* WA to avoid next warning :
   due to include to : #include <windows.h>
    c:\program files\microsoft visual studio\vc98\include\rpcasync.h(45) :
    warning C4115: '_RPC_ASYNC_STATE' : named type definition in parentheses
*/
struct _RPC_ASYNC_STATE;

#include <cmdShell/common/cmdCommon.h>
#include <cmdShell/os/cmdExecute.h>

#include <windows.h>
#include <gtOs/gtGenTypes.h>
#include <imagehlp.h>

typedef GT_STATUS (*SHELL_FUNC)(char* command);
/* global, so must be outside #ifdef IMAGE_HELP_SHELL */
SHELL_FUNC osSlellFunction = NULL;

/* for microsoft compiler only */
#if defined(IMAGE_HELP_SHELL) || defined(APPLICATION_SYMBOL_TABLE)

int osShellPrintf(const char* format, ...);


#define EXECUTE_SCRATCH_PAD_SIZE 128

static GT_U32 execute_scratch_pad[EXECUTE_SCRATCH_PAD_SIZE];

typedef int (*FUNCTION0)(void);
typedef int (*FUNCTION1)(void*);
typedef int (*FUNCTION2)(void*, void*);
typedef int (*FUNCTION3)(void*, void*, void*);
typedef int (*FUNCTION4)(void*, void*, void*, void*);
typedef int (*FUNCTION5)(void*, void*, void*, void*, void*);
typedef int (*FUNCTION6)(void*, void*, void*, void*, void*, void*);
typedef int (*FUNCTION7)(void*, void*, void*, void*, void*, void*, void*);
typedef int (*FUNCTION8)(void*, void*, void*, void*, void*, void*, void*,
    void*);
typedef int (*FUNCTION9)(void*, void*, void*, void*, void*, void*, void*,
    void*, void*);
typedef int (*FUNCTION10)(void*, void*, void*, void*, void*, void*, void*,
    void*, void*, void*);
typedef int (*FUNCTION11)(void*, void*, void*, void*, void*, void*, void*,
    void*, void*, void*, void*);
typedef int (*FUNCTION12)(void*, void*, void*, void*, void*, void*, void*,
    void*, void*, void*, void*, void*);
typedef int (*FUNCTION13)(void*, void*, void*, void*, void*, void*, void*,
    void*, void*, void*, void*, void*, void*);
typedef int (*FUNCTION14)(void*, void*, void*, void*, void*, void*, void*,
    void*, void*, void*, void*, void*, void*, void*);
typedef int (*FUNCTION15)(void*, void*, void*, void*, void*, void*, void*,
    void*, void*, void*, void*, void*, void*, void*, void*);


static int callFunction
(
    IN void* funcAddr,
    IN GT_U32* paramArray,
    IN GT_U32  paramAmount
)
{
    switch(paramAmount)
    {
        case 0:
            return ((FUNCTION0)funcAddr)();
        case 1:
            return ((FUNCTION1)funcAddr)((void*)paramArray[0]);
        case 2:
            return ((FUNCTION2)funcAddr)((void*)paramArray[0],
                    (void*)paramArray[1]);
        case 3:
            return ((FUNCTION3)funcAddr)((void*)paramArray[0],
                    (void*)paramArray[1], (void*)paramArray[2]);
        case 4:
            return ((FUNCTION4)funcAddr)((void*)paramArray[0],
                    (void*)paramArray[1], (void*)paramArray[2],
                    (void*)paramArray[3]);
        case 5:
            return ((FUNCTION5)funcAddr)((void*)paramArray[0],
                    (void*)paramArray[1], (void*)paramArray[2],
                    (void*)paramArray[3], (void*)paramArray[4]);
        case 6:
            return ((FUNCTION6)funcAddr)((void*)paramArray[0],
                    (void*)paramArray[1], (void*)paramArray[2],
                    (void*)paramArray[3], (void*)paramArray[4],
                    (void*)paramArray[5]);
        case 7:
            return ((FUNCTION7)funcAddr)((void*)paramArray[0],
                    (void*)paramArray[1], (void*)paramArray[2],
                    (void*)paramArray[3], (void*)paramArray[4],
                    (void*)paramArray[5], (void*)paramArray[6]);
        case 8:
            return ((FUNCTION8)funcAddr)((void*)paramArray[0],
                    (void*)paramArray[1], (void*)paramArray[2],
                    (void*)paramArray[3], (void*)paramArray[4],
                    (void*)paramArray[5], (void*)paramArray[6],
                    (void*)paramArray[7]);
        case 9:
            return ((FUNCTION9)funcAddr)((void*)paramArray[0],
                    (void*)paramArray[1], (void*)paramArray[2],
                    (void*)paramArray[3], (void*)paramArray[4],
                    (void*)paramArray[5], (void*)paramArray[6],
                    (void*)paramArray[7], (void*)paramArray[8]);
        case 10:
            return ((FUNCTION10)funcAddr)((void*)paramArray[0],
                    (void*)paramArray[1], (void*)paramArray[2],
                    (void*)paramArray[3], (void*)paramArray[4],
                    (void*)paramArray[5], (void*)paramArray[6],
                    (void*)paramArray[7], (void*)paramArray[8],
                    (void*)paramArray[9]);
        case 11:
            return ((FUNCTION11)funcAddr)((void*)paramArray[0],
                    (void*)paramArray[1], (void*)paramArray[2],
                    (void*)paramArray[3], (void*)paramArray[4],
                    (void*)paramArray[5], (void*)paramArray[6],
                    (void*)paramArray[7], (void*)paramArray[8],
                    (void*)paramArray[9], (void*)paramArray[10]);
        case 12:
            return ((FUNCTION12)funcAddr)((void*)paramArray[0],
                    (void*)paramArray[1], (void*)paramArray[2],
                    (void*)paramArray[3], (void*)paramArray[4],
                    (void*)paramArray[5], (void*)paramArray[6],
                    (void*)paramArray[7], (void*)paramArray[8],
                    (void*)paramArray[9], (void*)paramArray[10],
                    (void*)paramArray[11]);
        case 13:
            return ((FUNCTION13)funcAddr)((void*)paramArray[0],
                    (void*)paramArray[1], (void*)paramArray[2],
                    (void*)paramArray[3], (void*)paramArray[4],
                    (void*)paramArray[5], (void*)paramArray[6],
                    (void*)paramArray[7], (void*)paramArray[8],
                    (void*)paramArray[9], (void*)paramArray[10],
                    (void*)paramArray[11], (void*)paramArray[12]);
        case 14:
            return ((FUNCTION14)funcAddr)((void*)paramArray[0],
                    (void*)paramArray[1], (void*)paramArray[2],
                    (void*)paramArray[3], (void*)paramArray[4],
                    (void*)paramArray[5], (void*)paramArray[6],
                    (void*)paramArray[7], (void*)paramArray[8],
                    (void*)paramArray[9], (void*)paramArray[10],
                    (void*)paramArray[11], (void*)paramArray[12],
                    (void*)paramArray[13]);
        case 15:
            return ((FUNCTION15)funcAddr)((void*)paramArray[0],
                    (void*)paramArray[1], (void*)paramArray[2],
                    (void*)paramArray[3], (void*)paramArray[4],
                    (void*)paramArray[5], (void*)paramArray[6],
                    (void*)paramArray[7], (void*)paramArray[8],
                    (void*)paramArray[9], (void*)paramArray[10],
                    (void*)paramArray[11], (void*)paramArray[12],
                    (void*)paramArray[13], (void*)paramArray[14]);
        default: break;
    }
    return -1;
}

/* returns amount of parameters, -1 on parse error */
static int parceCmd
(
    IN  char    *command,
    IN  GT_U32  parmArraySize, /* in words */
    OUT char    *cmdName,
    OUT GT_U32  *parmArray
)
{
    GT_U8* p = (GT_U8*)command;
    GT_U8* arrEnd = (GT_U8*)parmArray + (parmArraySize * sizeof(GT_U32));
    int i, val;
    int paramAmount;
    GT_BOOL done;
    GT_BOOL isAddress;

    val = 0; /* to prevent compiler warning */
    paramAmount = 0;

    for (i = 0; (*p > ' '); p++, i++)
    {
        cmdName[i] = *p;
    }
    cmdName[i] = 0;

    while (1)
    {
        /* skip blanks and tabs and no more then one ',' */
        for (; ((*p > 0) && (*p <= ' ')); p++) {};
        if (*p == 0)
        {
            return paramAmount;
        }
        if ((*p == ',') && (paramAmount != 0))
        {
            p++;
            for (; ((*p > 0) && (*p <= ' ')); p++) {};
            if (*p == 0)
            {
                return paramAmount;
            }
        }

        isAddress = GT_FALSE;
        done = FALSE;

        /* case of string, 0x22 is a Quote code */
        if ((! done) && (*p == 0x22))
        {
            done = TRUE;
            p++;
            /* search for the second quote or end of string */
            for (i = 0; ((p[i] > 0) && (p[i] != 0x22)); i++) {};

            arrEnd -= (i + 1);
            strncpy(arrEnd, p, i);
            arrEnd[i] = 0;
            val = (GT_U32)arrEnd;
            /* new position - after closing quote or on '\0' */
            p += (p[i] == 0) ? i : (i + 1);
        }

        if ((! done) && (*p == '@'))
        {
            /* the next number is address in execute_scratch_pad */
            isAddress = GT_TRUE;
            p ++;
        }

        /* case of hex number */
        if ((! done) && (p[0] == '0') && ((p[1] == 'x') || (p[1] == 'X')))
        {
            done = TRUE;
            val = 0;
            p += 2;

            for (; ((*p > ' ') && (*p != ',')); p++)
            {
                val *= 16;

                if ((*p >= '0') && (*p <= '9'))
                {
                    val += *p - '0';
                    continue;
                }

                if ((*p >= 'a') && (*p <= 'f'))
                {
                    val += *p - 'a' + 10;
                    continue;
                }

                if ((*p >= 'A') && (*p <= 'F'))
                {
                    val += *p - 'A' + 10;
                    continue;
                }

                return -1;
            }
        }

        /* decimal number */
        if (! done)
        {
            val = 0;
            done = TRUE;

            for (; ((*p > ' ') && (*p != ',')); p++)
            {
                val *= 10;

                if ((*p >= '0') && (*p <= '9'))
                {
                    val += *p - '0';
                    continue;
                }

                return -1;
            }
        }

        if (isAddress != GT_FALSE)
        {
            if (val >= EXECUTE_SCRATCH_PAD_SIZE)
            {
                /* error */
                return -1;
            }
            val = (int)&(execute_scratch_pad[val]);
        }

        parmArray[paramAmount] = val;
        paramAmount ++;
    }

    /* dummy, never reached */
    return 0;
}

int execute_scratch_pad_clear(void)
{
    memset(execute_scratch_pad, 0, sizeof(execute_scratch_pad));
    return 0;
}

int execute_scratch_pad_set(int index, int value)
{
    if (index >= EXECUTE_SCRATCH_PAD_SIZE)
    {
        /* error */
        return -1;
    }
    execute_scratch_pad[index] = value;
    return 0;
}

int execute_scratch_pad_dump(int index, int amount)
{
    int i;

    if ((index+(amount-1)) >= EXECUTE_SCRATCH_PAD_SIZE)
    {
        /* error */
        return -1;
    }

    for (i = 0; (i < amount); i++)
    {
        osShellPrintf(" %08X ", execute_scratch_pad[index + i]);
        if (((i + 1) % 8) == 0)
        {
            osShellPrintf("\n");
        }
    }
    osShellPrintf("\n");

    return 0;
}

static GT_STATUS imageHelpShell
(
    IN  char*   command
)
{
    char funcName[128];
    GT_U32 paramArray[128]; /* 512 bytes for strings */
    int  paramAmount;
    GT_VOIDFUNCPTR  funcPtr;
    GT_STATUS rc;

    if ((command == NULL) || (command[0] == 0))
    {
        return GT_OK;
    }

    paramAmount = parceCmd(
        command,
        (sizeof(paramArray) / sizeof(paramArray[0])),
        funcName, paramArray);
    if (0 > paramAmount)
    {
        return GT_FAIL;
    }

    funcPtr = osShellGetFunctionByName(funcName);
    if (!funcPtr)
    {
        return GT_FAIL;
    }

    rc = callFunction(
        (void*)funcPtr, paramArray, paramAmount);

    osShellPrintf("return code is %d\r\n", rc);

    return GT_OK;
}

#endif /* defined(IMAGE_HELP_SHELL) || defined(APPLICATION_SYMBOL_TABLE) */

#if defined(IMAGE_HELP_SHELL) && !defined(APPLICATION_SYMBOL_TABLE)
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
GT_VOIDFUNCPTR osShellGetFunctionByName
(
    IN  const char* funcName
)
{
    /* prototype workaround */
    typedef BOOL (__stdcall *SYM_GET_SYM_BY_NAME_PTR)
    (
        HANDLE hProcess,
        const char* Name,
        PIMAGEHLP_SYMBOL Symbol
    );
    SYM_GET_SYM_BY_NAME_PTR _SymGetSymFromName =
            (SYM_GET_SYM_BY_NAME_PTR)SymGetSymFromName;

    IMAGEHLP_SYMBOL symbol;

    symbol.SizeOfStruct = sizeof(IMAGEHLP_SYMBOL);
    symbol.MaxNameLength = 0;

    if (! _SymGetSymFromName(
        GetCurrentProcess(), funcName, &symbol))
    {
        return NULL;
    }
    return (GT_VOIDFUNCPTR)symbol.Address;
}

void imageHelpShellInit()
{
    char appExePath[1024];              /* Application executable path */
    char * cmdLinePtr = &appExePath[0]; /* Pointer to application path */
    int lastSlashIndex = 0;             /* Last slash in application path */

    /* Get full application executable path including application name */
    GetModuleFileName(NULL, cmdLinePtr, 1024);

    /* Search for the last slash symbol */
    cmdLinePtr = strchr(cmdLinePtr, '\\');
    while(cmdLinePtr != NULL)
    {
        lastSlashIndex = cmdLinePtr - &appExePath[0];
        cmdLinePtr = strchr(++cmdLinePtr, '\\');
    }

    /* Remove executable name from string */
    appExePath[lastSlashIndex] = 0;

    /* init imageHelp library */
    SymInitialize(
        GetCurrentProcess(),
        appExePath /*UserSearchPath*/, TRUE /*fInvadeProcess*/);

    /* bind osCmdShell to this shell  */
    osSlellFunction = imageHelpShell;
}


#endif /* defined(IMAGE_HELP_SHELL) && !defined(APPLICATION_SYMBOL_TABLE) */

#ifdef APPLICATION_SYMBOL_TABLE

#include <string.h>
#include <stdlib.h>

typedef struct {
    const char *name;
    GT_VOIDFUNCPTR funcPtr;
} SYMTBL_ENTRY_STC;

extern SYMTBL_ENTRY_STC __SymbolTable[];
static int __SymbolTableLength = 0;


/*******************************************************************************
* qsortRecordCompareFunc
*
* DESCRIPTION:
*                Compares two CMD_COMMAND records by command name
*                Required by qsort()
*
* INPUTS:
*                a  - pointer to record
*                b  - pointer to record
*
* OUTPUTS:
*       none
*
* RETURNS:
*        integer less than, equal to, or greater than zero if
*        a is less than, equal to, or greater than b
*
* COMMENTS:
*       none
*
*******************************************************************************/
static int qsortRecordCompareFunc(
        const void* a,
        const void* b
)
{
    return strcmp(
            ((const SYMTBL_ENTRY_STC*)a)->name,
            ((const SYMTBL_ENTRY_STC*)b)->name);
}

void imageHelpShellInit()
{
    /* calculate length of symbol table */
    while (__SymbolTable[__SymbolTableLength].name)
        __SymbolTableLength++;
    /* sort table to speedup search */
    qsort(__SymbolTable, __SymbolTableLength, sizeof(__SymbolTable[0]), qsortRecordCompareFunc);

    /* bind osCmdShell to this shell  */
    osSlellFunction = imageHelpShell;
}

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

#endif /* APPLICATION_SYMBOL_TABLE */
