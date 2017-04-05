/*******************************************************************************
*              (c), Copyright 2014, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* prvTgfLog.h
*
* DESCRIPTION:
*       Internal header which defines API for TGF logger
*       It is used only in the tgfEngine
*
* FILE REVISION NUMBER:
*       $Revision: 1 $
*
*******************************************************************************/

#include <cpss/generic/cpssTypes.h>
#include <cpss/extServices/private/prvCpssBindFunc.h>
/* #include <extUtils/trafficEngine/tgfTrafficEngine.h> */
#include <extUtils/trafficEngine/prvTgfLog.h>

static TGF_LOG_BINDINGS_STC bind = {
    NULL, NULL, NULL, NULL, NULL, NULL
};

/*******************************************************************************
* prvTgfLogMessage
*
* DESCRIPTION:
*       This routine writes message to the output interface.
*       This function handles all issues regarding the logging to specific
*       interfaces.
*
* INPUTS:
*       formatStringPtr -   (pointer to) format string.
*       argvPtr         -   (pointer to) vector of output string arguments.
*       argc            -   number of arguments in output string.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK       -   logging has been done successfully.
*       GT_FAIL     -   General failure error. Should never happen.
*
* COMMENTS:
*       It uses OS abstraction layer to output.
*******************************************************************************/
GT_VOID prvTgfLogMessage
(
    IN const GT_CHAR    *formatStringPtr,
    IN const GT_UINTPTR argvPtr[],
    IN GT_U32           argc
)
{
    if (bind.logMessageFunc != NULL)
    {
        bind.logMessageFunc(formatStringPtr, argvPtr, argc);
    }
}

/* TODO */
GT_BOOL tgfEqualVerify
(
    IN GT_UINTPTR   e,
    IN GT_UINTPTR   r,
    IN GT_32        lineNum,
    IN GT_CHAR      *fileNamePtr
)
{
    if (bind.equalVerifyFunc != NULL)
    {
        return bind.equalVerifyFunc(e, r, lineNum, fileNamePtr);
    }
    return GT_FALSE;
}

/*TODO*/
GT_BOOL tgfFailureMsgLog
(
    IN const GT_CHAR    *failureMsgPtr,
    IN const GT_UINTPTR argvPtr[],
    IN GT_U32           argc
)
{
    if (bind.failureMgsLogFunc != NULL)
    {
        return bind.failureMgsLogFunc(failureMsgPtr, argvPtr, argc);
    }
    return GT_FALSE;
}

/*TODO*/
GT_VOID tgfPrintKeepAlive
(
    GT_VOID
)
{
    if (bind.printKeepAliveFunc != NULL)
    {
        bind.printKeepAliveFunc();
    }
}

GT_U32 tgfLogPrintModeFinal
(
    GT_VOID
)
{
    if (bind.logPrintModeFinalFunc != NULL)
    {
        return bind.logPrintModeFinalFunc();
    }
    return 0;
}

GT_U32 tgfLogPrintModeRestore
(
    IN GT_U32       savedmode
)
{
    if (bind.logPrintModeRestoreFunc != NULL)
    {
        return bind.logPrintModeRestoreFunc(savedmode);
    }
    return 0;
}

GT_VOID prvTgfLogBindFunctions
(
    IN TGF_LOG_BINDINGS_STC *bindings
)
{
    /* memcpy??? */
    if (bindings == NULL)
    {
        cpssOsMemSet(&bind, 0, sizeof(bind));
    }
    else
    {
        bind = *bindings;
    }
}
