/*******************************************************************************
*              Copyright 2002, GALILEO TECHNOLOGY, LTD.
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
* wrAppDemoLion2Fabric.c
*
* DESCRIPTION:
*       Galtis wrappers for Application demo of using Lion2
*       device as Switch Fabric connecting
*       Bobcat2 devices.
*
* DEPENDENCIES:
*       None.
*
* FILE REVISION NUMBER:
*       $Revision: 2$
*
*******************************************************************************/

/* Common generic includes */
#include <cpss/generic/cpssTypes.h>
#include <cpss/generic/cos/cpssCosTypes.h>
/* Common galtis includes */
#include <galtisAgent/wrapUtil/cmdCpssPresteraUtils.h>
#include <cmdShell/cmdDb/cmdBase.h>
#include <cmdShell/common/cmdWrapUtils.h>

/***********************************************************************************************/
/* forward declarations */
GT_STATUS appDemoLion2FabricInit
(
    IN GT_U8              lion2FabricDevNum,
    IN GT_PORT_NUM        cpuLinkPortNum
);
GT_STATUS appDemoLion2FabricForwardAddBc2Device
(
    IN  GT_U8           lion2FabricDevNum,
    IN  GT_U32          hwDevId,
    IN  GT_PORT_NUM     linkPortNum
);
GT_STATUS appDemoLion2FabricForwardAddBc2Vidx
(
    IN  GT_U8                 lion2FabricDevNum,
    IN  GT_U32                bc2Vidx,
    IN  CPSS_PORTS_BMP_STC    *targetPortsBmpPtr
);
GT_STATUS appDemoLion2FabricForwardAddBc2Vlan
(
    IN  GT_U8                 lion2FabricDevNum,
    IN  GT_U32                bc2VlanId,
    IN  CPSS_PORTS_BMP_STC    *targetPortsBmpPtr
);
GT_STATUS appDemoLion2FabricCleanUp
(
    IN GT_U8              lion2FabricDevNum
);
GT_STATUS appDemoLion2FabricConfigureQos
(
    IN GT_U8              lion2FabricDevNum,
    IN GT_U32             toCpuTc,
    IN CPSS_DP_LEVEL_ENT  toCpuDp,
    IN GT_U32             toAnalyzerTc,
    IN CPSS_DP_LEVEL_ENT  toAnalyzerDp,
    IN CPSS_DP_LEVEL_ENT  fromCpuDp
);

/***********************************************************************************************/
/* debug CMD_LINE wrappers */
/* appDemoLion2FabricInit                - wrapper not needed*/
/* appDemoLion2FabricForwardAddBc2Device - wrapper not needed*/

GT_STATUS dbgAppDemoLion2FabricForwardAddBc2Vidx
(
    IN  GT_U8                 lion2FabricDevNum,
    IN  GT_U32                bc2Vidx,
    IN  GT_U32                targetPortsBmpW0,
    IN  GT_U32                targetPortsBmpW1,
    IN  GT_U32                targetPortsBmpW2,
    IN  GT_U32                targetPortsBmpW3
)
{
    CPSS_PORTS_BMP_STC        targetPortsBmp;
    CPSS_PORTS_BMP_PORT_CLEAR_ALL_MAC(&targetPortsBmp);
    targetPortsBmp.ports[0] = targetPortsBmpW0;
    targetPortsBmp.ports[1] = targetPortsBmpW1;
    targetPortsBmp.ports[2] = targetPortsBmpW2;
    targetPortsBmp.ports[3] = targetPortsBmpW3;

    return appDemoLion2FabricForwardAddBc2Vidx(
        lion2FabricDevNum, bc2Vidx, &targetPortsBmp);
}

GT_STATUS dbgAppDemoLion2FabricForwardAddBc2Vlan
(
    IN  GT_U8                 lion2FabricDevNum,
    IN  GT_U32                bc2VlanId,
    IN  GT_U32                targetPortsBmpW0,
    IN  GT_U32                targetPortsBmpW1,
    IN  GT_U32                targetPortsBmpW2,
    IN  GT_U32                targetPortsBmpW3
)
{
    CPSS_PORTS_BMP_STC        targetPortsBmp;
    CPSS_PORTS_BMP_PORT_CLEAR_ALL_MAC(&targetPortsBmp);
    targetPortsBmp.ports[0] = targetPortsBmpW0;
    targetPortsBmp.ports[1] = targetPortsBmpW1;
    targetPortsBmp.ports[2] = targetPortsBmpW2;
    targetPortsBmp.ports[3] = targetPortsBmpW3;

    return appDemoLion2FabricForwardAddBc2Vlan(
        lion2FabricDevNum, bc2VlanId, &targetPortsBmp);
}

/***********************************************************************************************/
/* Galtis wrappers */

static CMD_STATUS wrAppDemoLion2FabricInit
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS          result;
    GT_U8              lion2FabricDevNum;
    GT_PORT_NUM        cpuLinkPortNum;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    lion2FabricDevNum = (GT_U8)inArgs[0];
    cpuLinkPortNum    = (GT_PORT_NUM)inArgs[1];

    /* call cpss api function */
    result = appDemoLion2FabricInit(lion2FabricDevNum, cpuLinkPortNum);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}

static CMD_STATUS wrAppDemoLion2FabricForwardAddBc2Device
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS          result;
    GT_U8              lion2FabricDevNum;
    GT_U32             hwDevId;
    GT_PORT_NUM        linkPortNum;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    lion2FabricDevNum = (GT_U8)inArgs[0];
    hwDevId           = (GT_U32)inArgs[1];
    linkPortNum       = (GT_PORT_NUM)inArgs[2];

    /* call cpss api function */
    result = appDemoLion2FabricForwardAddBc2Device(
        lion2FabricDevNum, hwDevId, linkPortNum);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}

static CMD_STATUS wrAppDemoLion2FabricForwardAddBc2Vidx
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                 result;
    GT_U8                     lion2FabricDevNum;
    GT_U32                    bc2Vidx;
    CPSS_PORTS_BMP_STC        targetPortsBmp;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    lion2FabricDevNum = (GT_U8)inArgs[0];
    bc2Vidx           = (GT_U32)inArgs[1];

    CPSS_PORTS_BMP_PORT_CLEAR_ALL_MAC(&targetPortsBmp);
    targetPortsBmp.ports[0] = (GT_U32)inArgs[2];
    targetPortsBmp.ports[1] = (GT_U32)inArgs[3];
    targetPortsBmp.ports[2] = (GT_U32)inArgs[4];
    targetPortsBmp.ports[3] = (GT_U32)inArgs[5];
    targetPortsBmp.ports[4] = (GT_U32)inArgs[6];
    targetPortsBmp.ports[5] = (GT_U32)inArgs[7];
    targetPortsBmp.ports[6] = (GT_U32)inArgs[8];
    targetPortsBmp.ports[7] = (GT_U32)inArgs[9];

    CONVERT_DEV_PHYSICAL_PORTS_BMP_MAC(lion2FabricDevNum, targetPortsBmp);

    /* call cpss api function */
    result = appDemoLion2FabricForwardAddBc2Vidx(
        lion2FabricDevNum, bc2Vidx, &targetPortsBmp);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}

static CMD_STATUS wrAppDemoLion2FabricForwardAddBc2Vlan
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                 result;
    GT_U8                     lion2FabricDevNum;
    GT_U32                    bc2VlanId;
    CPSS_PORTS_BMP_STC        targetPortsBmp;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    lion2FabricDevNum = (GT_U8)inArgs[0];
    bc2VlanId         = (GT_U32)inArgs[1];

    CPSS_PORTS_BMP_PORT_CLEAR_ALL_MAC(&targetPortsBmp);
    targetPortsBmp.ports[0] = (GT_U32)inArgs[2];
    targetPortsBmp.ports[1] = (GT_U32)inArgs[3];
    targetPortsBmp.ports[2] = (GT_U32)inArgs[4];
    targetPortsBmp.ports[3] = (GT_U32)inArgs[5];
    targetPortsBmp.ports[4] = (GT_U32)inArgs[6];
    targetPortsBmp.ports[5] = (GT_U32)inArgs[7];
    targetPortsBmp.ports[6] = (GT_U32)inArgs[8];
    targetPortsBmp.ports[7] = (GT_U32)inArgs[9];

    CONVERT_DEV_PHYSICAL_PORTS_BMP_MAC(lion2FabricDevNum, targetPortsBmp);

    /* call cpss api function */
    result = appDemoLion2FabricForwardAddBc2Vlan(
        lion2FabricDevNum, bc2VlanId, &targetPortsBmp);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}

static CMD_STATUS wrAppDemoLion2FabricCleanUp
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                 result;
    GT_U8                     lion2FabricDevNum;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    lion2FabricDevNum = (GT_U8)inArgs[0];

    /* call cpss api function */
    result = appDemoLion2FabricCleanUp(
        lion2FabricDevNum);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}

static CMD_STATUS wrAppDemoLion2FabricConfigureQos
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                 result;
    GT_U8                     lion2FabricDevNum;
    GT_U32                    toCpuTc;
    CPSS_DP_LEVEL_ENT         toCpuDp;
    GT_U32                    toAnalyzerTc;
    CPSS_DP_LEVEL_ENT         toAnalyzerDp;
    CPSS_DP_LEVEL_ENT         fromCpuDp;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    lion2FabricDevNum = (GT_U8)inArgs[0];
    toCpuTc           = (GT_U32)inArgs[1];
    toCpuDp           = (CPSS_DP_LEVEL_ENT)inArgs[2];
    toAnalyzerTc      = (GT_U32)inArgs[3];
    toAnalyzerDp      = (CPSS_DP_LEVEL_ENT)inArgs[4];
    fromCpuDp         = (CPSS_DP_LEVEL_ENT)inArgs[5];

    /* call cpss api function */
    result = appDemoLion2FabricConfigureQos(
        lion2FabricDevNum, toCpuTc, toCpuDp, toAnalyzerTc, toAnalyzerDp, fromCpuDp);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}

/**** database initialization **************************************/

static CMD_COMMAND dbCommands[] =
{
    {"appDemoLion2FabricInit",
        &wrAppDemoLion2FabricInit,
        2, 0},
    {"appDemoLion2FabricForwardAddBc2Device",
        &wrAppDemoLion2FabricForwardAddBc2Device,
        3, 0},
    {"appDemoLion2FabricForwardAddBc2Vidx",
        &wrAppDemoLion2FabricForwardAddBc2Vidx,
        10, 0},
    {"appDemoLion2FabricForwardAddBc2Vlan",
        &wrAppDemoLion2FabricForwardAddBc2Vlan,
        10, 0},
    {"appDemoLion2FabricCleanUp",
        &wrAppDemoLion2FabricCleanUp,
        1, 0},
    {"appDemoLion2FabricConfigureQos",
        &wrAppDemoLion2FabricConfigureQos,
        6, 0}
};

#define numCommands (sizeof(dbCommands) / sizeof(CMD_COMMAND))

/*******************************************************************************
* cmdLibInitAppDemoLion2Fabric
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
GT_STATUS cmdLibInitAppDemoLion2Fabric
(
    GT_VOID
)
{
    return cmdInitLibrary(dbCommands, numCommands);
}


