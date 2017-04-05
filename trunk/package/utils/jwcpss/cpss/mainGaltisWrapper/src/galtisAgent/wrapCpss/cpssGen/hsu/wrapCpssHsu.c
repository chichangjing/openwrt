/*******************************************************************************
*              (c), Copyright 2006, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* wrapCpssHsu.c
*
* DESCRIPTION:
*
* FILE REVISION NUMBER:
*       $Revision: 9 $
*
*******************************************************************************/

/* Common galtis includes */
#include <galtisAgent/wrapUtil/cmdCpssPresteraUtils.h>
#include <cmdShell/cmdDb/cmdBase.h>
#include <cmdShell/common/cmdWrapUtils.h>

/* Feature specific includes. */
#include <cpss/generic/cpssTypes.h>
#include <cpss/generic/systemRecovery/hsu/cpssGenHsu.h>
#include <cpss/generic/systemRecovery/cpssGenSystemRecovery.h>


/*******************************************************************************
* cpssHsuStateSet
*
* DESCRIPTION:
*       This function inform cpss about HSU state
*
*  APPLICABLE DEVICES: ALL
*
* INPUTS:
*       hsuInfoPtr               - pointer to hsu information
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                  - on success.
*       GT_BAD_PTR             - on NULL pointer
*       GT_BAD_PARAM           - on bad hsuState
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssHsuStateSet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    CPSS_SYSTEM_RECOVERY_INFO_STC hsuInfo;
    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    hsuInfo.systemRecoveryState = (GT_U32)inArgs[0];
    hsuInfo.systemRecoveryProcess = CPSS_SYSTEM_RECOVERY_PROCESS_HSU_E;
    switch ((GT_U32)inArgs[1])
    {
    case 0:
        hsuInfo.systemRecoveryMode.continuousAuMessages = GT_TRUE;
        hsuInfo.systemRecoveryMode.continuousFuMessages = GT_TRUE;
        hsuInfo.systemRecoveryMode.continuousRx = GT_TRUE;
        hsuInfo.systemRecoveryMode.continuousTx = GT_TRUE;
        break;
    case 1:
        hsuInfo.systemRecoveryMode.continuousAuMessages = GT_FALSE;
        hsuInfo.systemRecoveryMode.continuousFuMessages = GT_FALSE;
        hsuInfo.systemRecoveryMode.continuousRx = GT_FALSE;
        hsuInfo.systemRecoveryMode.continuousTx = GT_FALSE;
        break;
    default:
        return GT_BAD_PARAM;
    }
    hsuInfo.systemRecoveryMode.haCpuMemoryAccessBlocked = GT_FALSE;
    /* call cpss api function */
    result = cpssSystemRecoveryStateSet(&hsuInfo);
    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d%d", hsuInfo.systemRecoveryState,(GT_U32)inArgs[1]);

    return CMD_OK;
}
/*******************************************************************************
* cpssHsuEventHandleUpdate
*
* DESCRIPTION:
*       This function replace old event  handle created after regular init
*       to new event handle that application got during HSU.
*
*  APPLICABLE DEVICES: ALL
*
* INPUTS:
*       oldHndl               - old event  handle created after regular init.
*       newHndl               - new event handle  created during HSU.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                  - on success.
*
* COMMENTS:
*
*******************************************************************************/
CMD_STATUS wrCpssHsuEventHandleUpdate
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U32 oldEvReqHndl,newEvReqHndl;
    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    oldEvReqHndl = (GT_U32)inArgs[0];
    newEvReqHndl = (GT_U32)inArgs[1];

    result =  cpssHsuEventHandleUpdate(oldEvReqHndl, newEvReqHndl);
    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d%d", oldEvReqHndl, newEvReqHndl);

    return CMD_OK;
}

/*******************************************************************************
* extDrvHsuWarmRestart
*
* DESCRIPTION:
*       Perform warm restart
*
*  APPLICABLE DEVICES: ALL
*
* INPUTS:
*       None
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       None
*
* COMMENTS:
*        None
*
*******************************************************************************/
static CMD_STATUS wrCpssHsuWarmRestart
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;


#ifndef ASIC_SIMULATION
    cpssExtDrvHsuWarmRestart();
#endif
    return CMD_OK;
}


 /*******************************************************************************
* cpssExtDrvSetIntLockUnlock
*
* DESCRIPTION:
*       Lock/unlock interrupts
*
* INPUTS:
*       mode   - interrupt state lock/unlock
*       key    - if mode is INTR_MODE_UNLOCK, lock key returned by
*                preceding interrupt disable call
*
* OUTPUTS:
*       key    - if mode is INTR_MODE_LOCK lock key for the interrupt
*                level
*
* RETURNS:
*       Lock key for the interrupt level
*
* COMMENTS:
*       None
*
*******************************************************************************/
CMD_STATUS wrCpssExtDrvSetIntLockUnlock
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U32 intKey;
    CPSS_OS_INTR_MODE_ENT action;
    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    action = (CPSS_OS_INTR_MODE_ENT)inArgs[0];
    intKey = (GT_U32)inArgs[1];
    if (action == CPSS_OS_INTR_MODE_LOCK_E)
    {
        PRV_CPSS_LOCK_BY_MUTEX;
    }
    result =  cpssExtDrvSetIntLockUnlock(action, (GT_32*)&intKey);
    if (action == CPSS_OS_INTR_MODE_UNLOCK_E)
    {
        PRV_CPSS_UNLOCK_BY_MUTEX;
    }
     /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d",intKey);
    return CMD_OK;
}

/*******************************************************************************
* cpssPpInterruptsDisable
*
* DESCRIPTION:
*       This API disable  PP interrupts
*
*  APPLICABLE DEVICES: ALL
*
* INPUTS:
*       devNum            - The device number.
*       enable            - GT_TRUE  - enable interrupts
*                           GT_FALSE - disable interrupts
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on hardware error
*       GT_NOT_INITIALIZED - if the driver was not initialized
*       GT_HW_ERROR - on hardware error
*
* COMMENTS:
*       None
*
*******************************************************************************/
CMD_STATUS wrCpssPpInterruptsDisable
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];

    result = cpssPpInterruptsDisable(devNum);
    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/**** database initialization **************************************/

static CMD_COMMAND dbCommands[] =
{
        {"cpssHsuStateSet",
         &wrCpssHsuStateSet,
         2, 0},
        {"cpssHsuEventHandlUpdate",
         &wrCpssHsuEventHandleUpdate,
         2, 0},
        {"extDrvHsuWarmRestart",
         &wrCpssHsuWarmRestart,
         0, 0},
        {"cpssExtDrvSetIntLockUnlock",
         &wrCpssExtDrvSetIntLockUnlock,
         2, 0},
        {"cpssPpInterruptsDisable",
         &wrCpssPpInterruptsDisable,
         1, 0},

};

#define numCommands (sizeof(dbCommands) / sizeof(CMD_COMMAND))

/*******************************************************************************
* cmdLibInitCpssGenHsu
*
* DESCRIPTION:
*     Library database initialization function.
*
* INPUTS:
*     none
*
* OUTPUTS:
*     none
*
* RETURNS:
*     GT_OK   - on success.
*     GT_FAIL - on failure.
*
* COMMENTS:
*     none
*
*******************************************************************************/
GT_STATUS cmdLibInitCpssGenHsu
(
    GT_VOID
)
{
    return cmdInitLibrary(dbCommands, numCommands);
}

