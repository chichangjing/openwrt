/*******************************************************************************
*              (c), Copyright 2006, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* wrapCpssSystemRecovery.c
*
* DESCRIPTION:
*
* FILE REVISION NUMBER:
*       $Revision: 5 $
*
*******************************************************************************/

/* Common galtis includes */
#include <galtisAgent/wrapUtil/cmdCpssPresteraUtils.h>
#include <cmdShell/cmdDb/cmdBase.h>
#include <cmdShell/common/cmdWrapUtils.h>

/* Feature specific includes. */
#include <cpss/generic/cpssTypes.h>
#include <cpss/generic/systemRecovery/cpssGenSystemRecovery.h>


/*******************************************************************************
* wrCpssSystemRecoveryStateSet
*
* DESCRIPTION:
*       This function inform cpss about System Recovery state
*
*  APPLICABLE DEVICES: ALL
*
* INPUTS:
*       systemRecoveryInfoPtr               - pointer to system recovery information
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                  - on success.
*       GT_BAD_PTR             - on NULL pointer
*       GT_BAD_PARAM           - on bad system recovery state
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssSystemRecoveryStateSet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    CPSS_SYSTEM_RECOVERY_INFO_STC systemRecovery;
    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    systemRecovery.systemRecoveryState = (CPSS_SYSTEM_RECOVERY_STATE_ENT)inArgs[0];
    systemRecovery.systemRecoveryMode.continuousRx =  (GT_BOOL)inArgs[1];
    systemRecovery.systemRecoveryMode.continuousTx = (GT_BOOL)inArgs[2];
    systemRecovery.systemRecoveryMode.continuousAuMessages = (GT_BOOL)inArgs[3];
    systemRecovery.systemRecoveryMode.continuousFuMessages = (GT_BOOL)inArgs[4];
    systemRecovery.systemRecoveryMode.haCpuMemoryAccessBlocked = (GT_BOOL)inArgs[5];
    systemRecovery.systemRecoveryProcess = CPSS_SYSTEM_RECOVERY_PROCESS_NOT_ACTIVE_E;
    /* call cpss api function */
    result = cpssSystemRecoveryStateSet(&systemRecovery);
    cpssOsPrintf("systemRecoveryState = %d\n",systemRecovery.systemRecoveryState);
    cpssOsPrintf("systemRecoveryMode.continuousRx = %d\n",systemRecovery.systemRecoveryMode.continuousRx);
    cpssOsPrintf("systemRecoveryMode.continuousTx = %d\n",systemRecovery.systemRecoveryMode.continuousTx);
    cpssOsPrintf("systemRecoveryMode.continuousAuMessages = %d\n",systemRecovery.systemRecoveryMode.continuousAuMessages);
    cpssOsPrintf("systemRecoveryMode.continuousFuMessages = %d\n",systemRecovery.systemRecoveryMode.continuousFuMessages);
    cpssOsPrintf("systemRecoveryMode.haCpuMemoryAccessBlocked = %d\n",systemRecovery.systemRecoveryMode.haCpuMemoryAccessBlocked);
    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d%d%d%d", systemRecovery.systemRecoveryState,systemRecovery.systemRecoveryMode.continuousRx,
                 systemRecovery.systemRecoveryMode.continuousAuMessages,systemRecovery.systemRecoveryMode.haCpuMemoryAccessBlocked);

    return CMD_OK;
}

/*******************************************************************************
* wrCpssSystemRecoveryStateSet
*
* DESCRIPTION:
*       This function inform cpss about System Recovery state
*
*  APPLICABLE DEVICES: ALL
*
* INPUTS:
*       systemRecoveryInfoPtr               - pointer to system recovery information
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                  - on success.
*       GT_BAD_PTR             - on NULL pointer
*       GT_BAD_PARAM           - on bad system recovery state
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssSystemRecoveryStateSet_1
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    CPSS_SYSTEM_RECOVERY_INFO_STC systemRecovery;
    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    systemRecovery.systemRecoveryState = (CPSS_SYSTEM_RECOVERY_STATE_ENT)inArgs[0];
    systemRecovery.systemRecoveryMode.continuousRx =  (GT_BOOL)inArgs[1];
    systemRecovery.systemRecoveryMode.continuousTx = (GT_BOOL)inArgs[2];
    systemRecovery.systemRecoveryMode.continuousAuMessages = (GT_BOOL)inArgs[3];
    systemRecovery.systemRecoveryMode.continuousFuMessages = (GT_BOOL)inArgs[4];
    systemRecovery.systemRecoveryMode.haCpuMemoryAccessBlocked = (GT_BOOL)inArgs[5];
    systemRecovery.systemRecoveryProcess = (CPSS_SYSTEM_RECOVERY_PROCESS_ENT)inArgs[6];
    /* call cpss api function */
    result = cpssSystemRecoveryStateSet(&systemRecovery);
    cpssOsPrintf("systemRecoveryState = %d\n",systemRecovery.systemRecoveryState);
    cpssOsPrintf("systemRecoveryMode.continuousRx = %d\n",systemRecovery.systemRecoveryMode.continuousRx);
    cpssOsPrintf("systemRecoveryMode.continuousTx = %d\n",systemRecovery.systemRecoveryMode.continuousTx);
    cpssOsPrintf("systemRecoveryMode.continuousAuMessages = %d\n",systemRecovery.systemRecoveryMode.continuousAuMessages);
    cpssOsPrintf("systemRecoveryMode.continuousFuMessages = %d\n",systemRecovery.systemRecoveryMode.continuousFuMessages);
    cpssOsPrintf("systemRecoveryMode.haCpuMemoryAccessBlocked = %d\n",systemRecovery.systemRecoveryMode.haCpuMemoryAccessBlocked);
    cpssOsPrintf("systemRecoveryProcess = %d\n",systemRecovery.systemRecoveryProcess);
    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d%d%d%d", systemRecovery.systemRecoveryState,systemRecovery.systemRecoveryMode.continuousRx,
                 systemRecovery.systemRecoveryMode.continuousAuMessages,systemRecovery.systemRecoveryMode.haCpuMemoryAccessBlocked);

    return CMD_OK;
}


/*******************************************************************************
* cpssExtDrvInboundSdmaEnable
*
* DESCRIPTION:
*       Perform inbound sdma enable
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
static CMD_STATUS wrCpssInboundSdmaEnable
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS   result = GT_OK;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

#if ((!defined ASIC_SIMULATION) && (defined GDA8548_DRV))
    result = cpssExtDrvInboundSdmaEnable();
#endif
    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssExtDrvInboundSdmaDisable
*
* DESCRIPTION:
*       Perform inbound sdma enable
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
static CMD_STATUS wrCpssInboundSdmaDisable
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS   result = GT_OK;
    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;
#if ((!defined ASIC_SIMULATION) && (defined GDA8548_DRV))
    result = cpssExtDrvInboundSdmaDisable();
#endif
    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}


/**** database initialization **************************************/

static CMD_COMMAND dbCommands[] =
{
        {"cpssSystemRecoveryStateSet",
         &wrCpssSystemRecoveryStateSet,
         6, 0},
        {"cpssSystemRecoveryStateSet_1",
         &wrCpssSystemRecoveryStateSet_1,
         7, 0},
        {"cpssInboundSdmaEnable",
         &wrCpssInboundSdmaEnable,
         0, 0},
        {"cpssInboundSdmaDisable",
         &wrCpssInboundSdmaDisable,
         0, 0},
};

#define numCommands (sizeof(dbCommands) / sizeof(CMD_COMMAND))

/*******************************************************************************
* cmdLibInitCpssGenSystemRecovery
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
GT_STATUS cmdLibInitCpssGenSystemRecovery
(
    GT_VOID
)
{
    return cmdInitLibrary(dbCommands, numCommands);
}

