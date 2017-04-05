/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* cmdWrapUtils.c
*
* DESCRIPTION:
*       Commander Wrappers utility function implementation
*
* FILE REVISION NUMBER:
*       $Revision: 7 $
*******************************************************************************/

/***** Include files ***************************************************/

#include <cmdShell/common/cmdWrapUtils.h>

/*#ifdef WIN32*/
    #include <stdarg.h>
/*#endif*/

static GT_CHAR iFieldBuffer[CMD_MAX_BUFFER];
static GT_CHAR stringBuffer[CMD_MAX_BUFFER];
static GT_BOOL fieldOutputAppendMode = GT_FALSE;
GT_U32 stringOffset = 0;

/*******************************************************************************
* strDuplicate
*
* DESCRIPTION:
*       Cyclic string duplicator
*
* INPUTS:
*       Null terminated string
*
* OUTPUTS:
*       none
*
* RETURNS:
*       Null terminated string
*
* COMMENTS:
*
*******************************************************************************/
GT_CHAR* strDuplicate(const GT_CHAR *src)
{
    int length = cmdOsStrlen(src) + 1;
    GT_CHAR *string = stringBuffer + stringOffset;

    if ((length + stringOffset) > CMD_MAX_BUFFER)
        return "$$$";
    cmdOsStrCpy(string, src);
    stringOffset += length;
    return string;
}

#ifdef CMD_LUA_CLI
extern int cmdLuaGaltisWrapper; /* flag */
GT_VOID cmdLuaGaltisWrapperOutput(GT_STATUS status, const GT_CHAR *format, va_list ap);
GT_VOID cmdLuaGaltisWrapperFieldOutput(const GT_CHAR *format, va_list ap);
GT_VOID cmdLuaGaltisWrapperFieldSetAppendMode(GT_VOID);
#endif
/*******************************************************************************
* galtisOutput
*
* DESCRIPTION:
*       builds galtis output string
*
* INPUTS:
*       status          - status that will be added to resultString
*       format          - the incoming arguments format (%d%c%x%s...). The %f
*                         can be used for adding a pre-prepared fields output
*                         string (by using fieldOutput function)
*       ...             - list of arguments to put in the resultString. For %f
*                         format no argument is needed
*
* OUTPUTS:
*       resultString    - the result string (the input converted into string)
*
* RETURNS:
*       GT_VOID
*
* COMMENTS:
*       the output looks like: @@@status!!!o1!!!o2!!!o3!!! ... on!!!###
*
*******************************************************************************/
GT_VOID galtisOutput
(
    OUT GT_8 *resultStringU8,
    IN  GT_STATUS status,
    IN  GT_CHAR *format,
    IN ...
)
{
    va_list argP;
    GT_CHAR tmp[100] = "";
    GT_CHAR *p,*sval;
    GT_VOID *array;
    GT_U32 xval;
    GT_32 dval, arsize, idx;
    GT_U64 *big;
    GT_CHAR *resultString = (GT_CHAR*)resultStringU8;
    GT_U32 resultStringLength;

    va_start(argP, format);
    
#ifdef CMD_LUA_CLI
    if (cmdLuaGaltisWrapper)
    {
        cmdLuaGaltisWrapperOutput(status, format, argP);
        return;
    }
#endif

    cmdOsSprintf(resultString, "@@@%x!!!", status); /* command passed */

    /* set argP to point to the first unnamed argument */
    for (p = format; *p; p++) {
        if (*p != '%') {
            cmdOsStrNCat(resultString, p, 1);
            continue;
        }
        switch (*++p) {
        case '0': case '1': case '2': case '3': case '4':
        case '5': case '6': case '7': case '8': case '9':
        case '*':
            if (*p == '*')
            {
                p++;
                arsize = va_arg(argP, GT_32);
            } else {
                arsize = 0;
                while (*p >= '0' && *p <= '9')
                    arsize = (arsize * 10) + (*p++ - '0');
            }
            array = va_arg(argP, GT_VOID*);
            switch (cmdOsToUpper(*p)) {
                case 'B':
                    for (idx = 0; idx < arsize; idx++) {
                        xval = ((GT_U8*)array)[idx];
                        cmdOsSprintf(tmp,"%02x",xval);
                        cmdOsStrCat(resultString,tmp);
                    }
                    cmdOsStrCat(resultString, "!!!");
                    break;
                case 'W':
                    for (idx = 0; idx < arsize; idx++) {
                        xval = ((GT_U16*)array)[idx];
                        cmdOsSprintf(tmp,"%04x",xval);
                        cmdOsStrCat(resultString,tmp);
                    }
                    cmdOsStrCat(resultString, "!!!");
                    break;
                case 'D':
                    for (idx = 0; idx < arsize; idx++) {
                        xval = ((GT_U32*)array)[idx];
                        cmdOsSprintf(tmp,"%08x",xval);
                        cmdOsStrCat(resultString,tmp);
                    }
                    cmdOsStrCat(resultString, "!!!");
                    break;
            }
            break;
        case 'f':
        case 'F':
            /* get the data and point to next parameter in the variable list */
            cmdOsStrCat(resultString, iFieldBuffer);
            cmdOsStrCat(resultString, "!!!");
            /* clear iFieldBuffer */
            iFieldBuffer[0] = 0;
            break;
        case 'x':
        case 'X':
            /* get the data and point to next parameter in the variable list */
            xval = va_arg(argP, GT_U32);
            cmdOsSprintf(tmp, "%x!!!", xval);
            cmdOsStrCat(resultString, tmp);
            break;
        case 'd':
        case 'D':
            /* get the data and point to next parameter in the variable list */
            dval = va_arg(argP, GT_32);
            cmdOsSprintf(tmp, "%x!!!", dval);
            cmdOsStrCat(resultString, tmp);
            break;

        case 'l':
        case 'L':
            big = va_arg(argP, GT_U64*);
            cmdOsSprintf(tmp, "%x!!!%x", big->l[0], big->l[1]);
            cmdOsStrCat(resultString, tmp);
            break;

        case 's':
        case 'S':
            /* get the data and point to next parameter in the variable list */
            for (sval = va_arg(argP, GT_CHAR*); *sval; sval++)
                cmdOsStrNCat(resultString, sval, 1);
            cmdOsStrCat(resultString, "!!!");
            break;
        default:
            break;
        }
    }
    resultStringLength = cmdOsStrlen(resultString);
    if ((resultString[resultStringLength-1] != '!') ||
        (resultString[resultStringLength-2] != '!') ||
        (resultString[resultStringLength-3] != '!'))
    {
        cmdOsStrCat(resultString, "!!!");
    }
    cmdOsStrCat(resultString, "###");

    va_end(argP);
    stringOffset = 0;
}

/*******************************************************************************
* fieldOutput
*
* DESCRIPTION:
*       builds table field output from input to the internal field output
*       string
*
* INPUTS:
*       format  - the incoming arguments format (%d%c%x%s...)
*       ...     - list of arguments to put in the fields output string
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_VOID
*
* COMMENTS:
*       ???F1???F2???F3??? .. Fn??? !!!###
*
*******************************************************************************/
GT_VOID fieldOutput
(
    IN GT_CHAR* format,
    IN ...
)
{
    va_list argP;
    GT_CHAR tmp[100] = "";
    GT_CHAR *p,*sval;
    GT_VOID *array;
    GT_U32 xval;
    GT_32 dval, arsize, idx;
    GT_U64 *big;
    GT_CHAR *resultString = iFieldBuffer;

    /* set argP to point to the first unnamed argument */
    va_start(argP, format);

#ifdef CMD_LUA_CLI
    if (cmdLuaGaltisWrapper)
    {
        cmdLuaGaltisWrapperFieldOutput(format, argP);
        return;
    }
#endif

    if (fieldOutputAppendMode == GT_FALSE)
    {
        resultString[0] = 0;
    }
    /* to call fieldOutputSetAppendMode any time before this function call */
    fieldOutputAppendMode = GT_FALSE;

    for (p = format; *p; p++) {
        if (*p != '%') {
            cmdOsStrNCat(resultString, p, 1);
            continue;
        }
        switch (*++p) {
        case '0': case '1': case '2': case '3': case '4':
        case '5': case '6': case '7': case '8': case '9':
        case '*':
            if (*p == '*')
            {
                p++;
                arsize = va_arg(argP, GT_32);
            } else {
                arsize = 0;
                while (*p >= '0' && *p <= '9')
                    arsize = (arsize * 10) + (*p++ - '0');
            }
            array = va_arg(argP, GT_VOID*);
            switch (*p) {
                case 'B':
                    for (idx = 0; idx < arsize; idx++) {
                        xval = ((GT_U8*)array)[idx];
                        cmdOsSprintf(tmp,"%02X",xval);
                        cmdOsStrCat(resultString,tmp);
                    }
                    cmdOsStrCat(resultString, "???");
                    break;
                case 'b':
                    for (idx = 0; idx < arsize; idx++) {
                        xval = ((GT_U8*)array)[idx];
                        cmdOsSprintf(tmp,"%02x",xval);
                        cmdOsStrCat(resultString,tmp);
                    }
                    cmdOsStrCat(resultString, "???");
                    break;
                case 'w':
                case 'W':
                    for (idx = 0; idx < arsize; idx++) {
                        xval = ((GT_U16*)array)[idx];
                        cmdOsSprintf(tmp,"%04x",xval);
                        cmdOsStrCat(resultString,tmp);
                    }
                    cmdOsStrCat(resultString, "???");
                    break;
                case 'd':
                case 'D':
                    for (idx = 0; idx < arsize; idx++) {
                        xval = ((GT_U32*)array)[idx];
                        cmdOsSprintf(tmp,"%08x",xval);
                        cmdOsStrCat(resultString,tmp);
                    }
                    cmdOsStrCat(resultString, "???");
                    break;
            }
            break;
        case 'x':
        case 'X':
            /* get the data and point to next parameter in the variable list */
            xval = va_arg(argP, GT_U32);
            cmdOsSprintf(tmp, "%x???", xval);
            cmdOsStrCat(resultString, tmp);
            break;
        case 'd':
        case 'D':
            /* get the data and point to next parameter in the variable list */
            dval = va_arg(argP, GT_32);
            cmdOsSprintf(tmp, "%x???", dval);
            cmdOsStrCat(resultString, tmp);
            break;

        case 'l':
        case 'L':
            big = va_arg(argP, GT_U64*);
            cmdOsSprintf(tmp, "%x???%x", big->l[0], big->l[1]);
            cmdOsStrCat(resultString, tmp);
            break;

        case 's':
        case 'S':
            /* get the data and point to next parameter in the variable list */
            for (sval = va_arg(argP, GT_CHAR*); *sval; sval++)
                cmdOsStrNCat(resultString, sval, 1);
            cmdOsStrCat(resultString, "???");
            break;
        default:
            break;
        }
    }
    va_end(argP);
}

/*******************************************************************************
* fieldOutputSetAppendMode
*
* DESCRIPTION:
*       causes fieldOutput to continue the output string
*
* INPUTS:
*       None
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_VOID
*
* COMMENTS:
*
*******************************************************************************/
GT_VOID fieldOutputSetAppendMode
(
    GT_VOID
)
{
#ifdef CMD_LUA_CLI
    if (cmdLuaGaltisWrapper)
    {
        cmdLuaGaltisWrapperFieldSetAppendMode();
        return;
    }
#endif
    
    fieldOutputAppendMode = GT_TRUE;
}

/*******************************************************************************
* genericTableGetLast
*
* DESCRIPTION:
*   The generic table "GetLast" function.
*   Used for one-line tables as GetNext function that notifies Galtis GUI
*   that the table has no lines more.
*
* APPLICABLE DEVICES:  All Devices
*
* INPUTS:
*       standard arguments of Galtis wrapper
*
* OUTPUTS:
*       standard arguments of Galtis wrapper
*       None

* RETURNS:
*       GT_OK          - on success any case
*
* COMMENTS:
*       NONE
*
*
*
*******************************************************************************/
CMD_STATUS genericTableGetLast
(
    IN  GT_UINTPTR inArgs[] GT_UNUSED,
    IN  GT_UINTPTR inFields[] GT_UNUSED,
    IN  GT_32 numFields GT_UNUSED,
    OUT GT_8  outArgs[]
)
{
    galtisOutput(outArgs, GT_OK, "%d", -1);
    return CMD_OK;
}

/*******************************************************************************
* genericDummyCommand
*
* DESCRIPTION:
*   The generic dummy command.
*   Only notifies Galtis GUI the successfull execution of the command
*   Used for tables as EndSet function that at the case when
*   all real API calls done in SetFirst and SetNext functions.
*
* APPLICABLE DEVICES:  All Devices
*
* INPUTS:
*       standard arguments of Galtis wrapper
*
* OUTPUTS:
*       standard arguments of Galtis wrapper
*       None

* RETURNS:
*       GT_OK          - on success any case
*
* COMMENTS:
*       NONE
*
*
*
*******************************************************************************/
CMD_STATUS genericDummyCommand
(
    IN  GT_UINTPTR inArgs[] GT_UNUSED,
    IN  GT_UINTPTR inFields[] GT_UNUSED,
    IN  GT_32 numFields GT_UNUSED,
    OUT GT_8  outArgs[]
)
{
    galtisOutput(outArgs, GT_OK, "");
    return CMD_OK;
}


