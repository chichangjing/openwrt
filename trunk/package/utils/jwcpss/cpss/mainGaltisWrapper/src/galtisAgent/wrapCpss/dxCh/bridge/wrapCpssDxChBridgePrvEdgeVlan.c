
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
* wrapBridgePrvEdgeVlanCpssDxCh.c
*
* DESCRIPTION:
*       Wrapper functions for BridgePrvEdgeVlan cpss.dxCh functions
*
* DEPENDENCIES:
*       None.
*
* FILE REVISION NUMBER:
*       $Revision: 18 $
*
*******************************************************************************/

/* Common galtis includes */
#include <galtisAgent/wrapUtil/cmdCpssPresteraUtils.h>
#include <cmdShell/cmdDb/cmdBase.h>
#include <cmdShell/common/cmdWrapUtils.h>


/* Feature specific includes. */
#include <cpss/generic/cpssTypes.h>
#include <cpss/dxCh/dxChxGen/bridge/cpssDxChBrgPrvEdgeVlan.h>
#include <cpss/dxCh/dxChxGen/config/private/prvCpssDxChInfoEnhanced.h>

/* port number for cpssDxChBrgPrvEdgeVlanPort table */
static GT_U8 brgPrvEdgeVlanPortNum = 0;

/*******************************************************************************
* cpssDxChBrgPrvEdgeVlanEnable
*
* DESCRIPTION:
*       This function enables/disables the Private Edge VLAN on
*       specified device
*
* APPLICABLE DEVICES:  All DxCh devices
*
* INPUTS:
*       devNum - device number
*       enable - GT_TRUE to enable the feature,
*                GT_FALSE to disable the feature
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK            - on success
*       GT_FAIL          - on error
*       GT_HW_ERROR - on hardware error
*       GT_BAD_PARAM     - on bad devNum
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChBrgPrvEdgeVlanEnable

(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8    devNum;
    GT_BOOL  enable;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    enable = (GT_BOOL)inArgs[1];

    /* call cpss api function */
    result = cpssDxChBrgPrvEdgeVlanEnable(devNum, enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}

/*******************************************************************************
* cpssDxChBrgPrvEdgeVlanEnableGet
*
* DESCRIPTION:
*       Get status of the Private Edge VLAN on
*       specified device
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Lion3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum - device number
*
* OUTPUTS:
*       enablePtr - GT_TRUE to enable the feature,
*                   GT_FALSE to disable the feature
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on bad devNum
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_BAD_PTR               - on NULL ptr
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChBrgPrvEdgeVlanEnableGet

(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8    devNum;
    GT_BOOL  enable;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];

    /* call cpss api function */
    result = cpssDxChBrgPrvEdgeVlanEnableGet(devNum, &enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", enable);
    return CMD_OK;
}


/*******************************************************************************
* cpssDxChBrgPrvEdgeVlanPortEnable
*
* DESCRIPTION:
*       Enable/Disable a specified port to operate in Private Edge VLAN mode.
*
* APPLICABLE DEVICES:  All DxCh devices
*
* INPUTS:
*       devNum    - device number
*       portNum   - physical port number to set state.
*       enable    - GT_TRUE for enabled, GT_FALSE for disabled
*       dstPort   - the destination "uplink" physical port to which all traffic
*                   received on srcPort&srcDev is forwarded.  This parameter
*                   is ignored if disabling the mode.
*       dstHwDev  - the destination "uplink" physical HW device to which all
*                   traffic received on srcPort&srcDev is forwarded.  This
*                   parameter is ignored if disabling the mode.
*       dstTrunk  - the destination is a trunk member. This parameter
*                   is ignored if disabling the mode.
*                   GT_TRUE - dstPort hold value of trunkId , and dstDev is
*                             ignored
*                   GT_FALSE - dstPort hold value of port in device dstDev
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK            - on success
*       GT_FAIL          - on error
*       GT_HW_ERROR      - on hardware error
*       GT_BAD_PARAM     - on bad portNum or devNum or dstPort or dstDev
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChBrgPrvEdgeVlanPortEnable

(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8           devNum;
    GT_PORT_NUM     portNum;
    GT_BOOL         enable;
    GT_PORT_NUM     dstPort;
    GT_HW_DEV_NUM   dstHwDev;
    GT_BOOL  dstTrunk;
    GT_TRUNK_ID trunkId;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    portNum = (GT_PORT_NUM)inArgs[1];
    enable = (GT_BOOL)inArgs[2];
    dstPort = (GT_PORT_NUM)inArgs[3];
    dstHwDev = (GT_HW_DEV_NUM)inArgs[4];
    dstTrunk = (GT_BOOL)inArgs[5];


    /* Override Ingress Device and Port */
    CONVERT_DEV_PORT_U32_MAC(devNum, portNum);

    if (dstTrunk == GT_TRUE)
    {
        trunkId = (GT_TRUNK_ID)dstPort;
        CONVERT_TRUNK_ID_TEST_TO_CPSS_MAC(trunkId);
        dstPort = (GT_U8)trunkId;
    }
    else
    {
        /* Override Uplink Device and Port */
        CONVERT_DEV_PORT_DATA_MAC(dstHwDev, dstPort);
    }

    /* call cpss api function */
    result = cpssDxChBrgPrvEdgeVlanPortEnable(devNum, portNum, enable,
                                            dstPort, dstHwDev, dstTrunk);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}

/*******************************************************************************
* cpssDxChBrgPrvEdgeVlanPortEnableGet
*
* DESCRIPTION:
*       Get enabling/disabling status to operate in Private Edge VLAN mode
*       and destination parameters for a specified port
*
* APPLICABLE DEVICES:  All DxCh Devices
*
* INPUTS:
*       devNum    - device number
*       portNum   - physical port number to set state.
*
* OUTPUTS:
*       enablePtr - pointer to status of enabling/disabling a specified port
*                   to operate in Private Edge VLAN mode.
*                   GT_TRUE for enabled,
*                   GT_FALSE for disabled
*       dstPortPtr  - pointer to the destination "uplink" physical port to which
*                     all trafficreceived on srcPort&srcDev is forwarded.
*                     This parameter is ignored if disabling the mode.
*       dstHwDevPtr - pointer to the destination "uplink" physical HW device to
*                     which all traffic received on srcPort&srcDev is forwarded.
*                     This parameter is ignored if disabling the mode.
*       dstTrunkPtr - pointer to the destination is a trunk member.
*                     This parameter is ignored if disabling the mode.
*                     GT_TRUE - dstPort hold value of trunkId , and dstHwDev is
*                               ignored
*                     GT_FALSE - dstPort hold value of port in device dstHwDev
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on bad portNum or devNum
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChBrgPrvEdgeVlanPortEnableGet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8           devNum;
    GT_PORT_NUM     portNum;
    GT_BOOL         enable;
    GT_PORT_NUM     dstPort;
    GT_HW_DEV_NUM   dstHwDev;
    GT_BOOL         dstTrunk;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum  = (GT_U8)inArgs[0];
    portNum = (GT_PORT_NUM)inArgs[1];

    CONVERT_DEV_PORT_U32_MAC(devNum, portNum);

    /* call cpss api function */
    result = cpssDxChBrgPrvEdgeVlanPortEnableGet(devNum, portNum, &enable,
                                                 &dstPort, &dstHwDev, &dstTrunk);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d%d%d%d", enable, dstPort, dstHwDev, dstTrunk);
    return CMD_OK;

}

static CPSS_PORTS_BMP_STC  prvEdgeSkipPortBmp;
/*******************************************************************************
* cpssDxChBrgPrvEdgeVlanPortEnableGet
*
* DESCRIPTION:
*       Get enabling/disabling status to operate in Private Edge VLAN mode
*       and destination parameters for a specified port
*
* APPLICABLE DEVICES:  All DxCh Devices
*
* INPUTS:
*       devNum    - device number
*       portNum   - physical port number to set state.
*
* OUTPUTS:
*       enablePtr - pointer to status of enabling/disabling a specified port
*                   to operate in Private Edge VLAN mode.
*                   GT_TRUE for enabled,
*                   GT_FALSE for disabled
*       dstPortPtr  - pointer to the destination "uplink" physical port to which
*                     all trafficreceived on srcPort&srcDev is forwarded.
*                     This parameter is ignored if disabling the mode.
*       dstHwDevPtr   - pointer to the destination "uplink" physical device to
*                     which all traffic received on srcPort&srcDev is forwarded.
*                     This parameter is ignored if disabling the mode.
*       dstTrunkPtr - pointer to the destination is a trunk member.
*                     This parameter is ignored if disabling the mode.
*                     GT_TRUE - dstPort hold value of trunkId , and dstHwDev is
*                               ignored
*                     GT_FALSE - dstPort hold value of port in device dstHwDev
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on bad portNum or devNum
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS util_wrCpssDxChBrgPrvEdgeVlanPortEnableGet
(
    IN     GT_U8   devNum,
    INOUT  GT_U8   *portNumPtr,
    IN     GT_UINTPTR   inFields[CMD_MAX_FIELDS],
    OUT    GT_8    outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS           result;
    GT_PORT_NUM         portNum, portNumGet;
    GT_BOOL             enable = GT_FALSE;
    GT_PORT_NUM         dstPort;
    GT_HW_DEV_NUM       dstHwDev;
    GT_BOOL             dstTrunk;
    GT_PORT_NUM         tmpPortNum;

    /* map input arguments to locals */
    portNum = *portNumPtr;

    while (1)
    {
        portNum = (GT_PORT_NUM)( *portNumPtr);

         /* Need to change 256 to the maxValue of GT_PORT_NUM according to the
        PRV_CPSS_DXCH_PP_HW_MAX_VALUE_OF_E_PORT_MAC(devNum) */
        CPSS_TBD_BOOKMARK_EARCH

        if (portNum >= 255)
        {
            /* all ports alredy treated - end of table */
            galtisOutput(outArgs, GT_OK, "%d", -1);
            return GT_OK;
        }

        /* default values, used when PVE disable */
        dstPort  = 0;
        dstHwDev   = 0;
        dstTrunk = GT_FALSE;
        enable   = GT_FALSE;

        portNumGet = portNum;
        tmpPortNum = portNum;

        /* Override Ingress Device and Port */
        CONVERT_DEV_PORT_U32_MAC(devNum, tmpPortNum);

        if (tmpPortNum != portNum)
        {
            /* port has convertion. Mark real port as already handled */
            CPSS_PORTS_BMP_PORT_SET_MAC(&prvEdgeSkipPortBmp, tmpPortNum);
            portNumGet = tmpPortNum;
        } 
        else if (CPSS_PORTS_BMP_IS_PORT_SET_MAC(&prvEdgeSkipPortBmp, portNum))
        {
            /* output default values for a port, 
               increment port number for the next calls */
            (*portNumPtr) ++;
            break;
        }

        /* call cpss api function */
        result = cpssDxChBrgPrvEdgeVlanPortEnableGet(
            devNum, portNumGet, &enable, &dstPort, &dstHwDev, &dstTrunk);
        if (result == GT_OK)
        {
            /* success, increment port number for the next calls */
            (*portNumPtr) ++;
            break;
        }

        /* this port failed, try the next port */
        (*portNumPtr) ++;
    }

    /* pack output arguments to galtis string */
    fieldOutput(
        "%d%d%d%d%d",
        portNum, enable, dstPort, dstHwDev, dstTrunk);

    galtisOutput(outArgs, GT_OK, "%f");

    return GT_OK;

}

/*******************************************************************************
* wrCpssDxChBrgPrvEdgeVlanPortTableGetFirst
*
* DESCRIPTION:
*       Get enabling/disabling status to operate in Private Edge VLAN mode
*       and destination parameters for a specified port
*
* APPLICABLE DEVICES:  All DxCh Devices
*
* INPUTS:
*       devNum    - device number
*       portNum   - physical port number to set state.
*
* OUTPUTS:
*       enablePtr - pointer to status of enabling/disabling a specified port
*                   to operate in Private Edge VLAN mode.
*                   GT_TRUE for enabled,
*                   GT_FALSE for disabled
*       dstPortPtr  - pointer to the destination "uplink" physical port to which
*                     all trafficreceived on srcPort&srcDev is forwarded.
*                     This parameter is ignored if disabling the mode.
*       dstDevPtr   - pointer to the destination "uplink" physical device to
*                     which all traffic received on srcPort&srcDev is forwarded.
*                     This parameter is ignored if disabling the mode.
*       dstTrunkPtr - pointer to the destination is a trunk member.
*                     This parameter is ignored if disabling the mode.
*                     GT_TRUE - dstPort hold value of trunkId , and dstDev is
*                               ignored
*                     GT_FALSE - dstPort hold value of port in device dstDev
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on bad portNum or devNum
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS  wrCpssDxChBrgPrvEdgeVlanPortTableGetFirst
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_U8                           devNum;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    brgPrvEdgeVlanPortNum = 0;
    CPSS_PORTS_BMP_PORT_CLEAR_ALL_MAC(&prvEdgeSkipPortBmp);

    util_wrCpssDxChBrgPrvEdgeVlanPortEnableGet(
        devNum, &brgPrvEdgeVlanPortNum,
        inFields, outArgs);

    return CMD_OK;
}

/*******************************************************************************
* wrCpssDxChBrgPrvEdgeVlanPortTableGetNext
*
* DESCRIPTION:
*       Get enabling/disabling status to operate in Private Edge VLAN mode
*       and destination parameters for a specified port
*
* APPLICABLE DEVICES:  All DxCh Devices
*
* INPUTS:
*       devNum    - device number
*       portNum   - physical port number to set state.
*
* OUTPUTS:
*       enablePtr - pointer to status of enabling/disabling a specified port
*                   to operate in Private Edge VLAN mode.
*                   GT_TRUE for enabled,
*                   GT_FALSE for disabled
*       dstPortPtr  - pointer to the destination "uplink" physical port to which
*                     all trafficreceived on srcPort&srcDev is forwarded.
*                     This parameter is ignored if disabling the mode.
*       dstDevPtr   - pointer to the destination "uplink" physical device to
*                     which all traffic received on srcPort&srcDev is forwarded.
*                     This parameter is ignored if disabling the mode.
*       dstTrunkPtr - pointer to the destination is a trunk member.
*                     This parameter is ignored if disabling the mode.
*                     GT_TRUE - dstPort hold value of trunkId , and dstDev is
*                               ignored
*                     GT_FALSE - dstPort hold value of port in device dstDev
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on bad portNum or devNum
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS  wrCpssDxChBrgPrvEdgeVlanPortTableGetNext
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_U8                           devNum;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];

    util_wrCpssDxChBrgPrvEdgeVlanPortEnableGet(
        devNum, &brgPrvEdgeVlanPortNum,
        inFields, outArgs);

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChBrgPrvEdgeVlanPortControlPktsToUplinkSet
*
* DESCRIPTION:
*       Enable/Disable per port forwarding control taffic to Private Edge
*       VLAN Uplink. PVE port can be disabled from trapping or mirroring
*       bridged packets to the CPU. In this case, as long as the packet is not
*       assigned a HARD DROP or SOFT DROP, the packet is unconditionally
*       assigned a FORWARD command with the ingress ports configured
*       PVE destination (PVLAN Uplink).
*
* APPLICABLE DEVICES:  All DXCH2 devices
*
* INPUTS:
*       devNum  - device number
*       port    - physical or CPU port to set.
*       enable  - GT_TRUE  - forward control traffic to PVE Uplink
*                 GT_FALSE - not forward control traffic to PVE Uplink,
*                            trapped or mirrored to CPU packets are sent to
*                            CPU port
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK             - on success
*       GT_BAD_PARAM      - wrong devNum, port
*       GT_HW_ERROR       - on hardware error
*
* COMMENTS:
*
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChBrgPrvEdgeVlanPortControlPktsToUplinkSet

(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8       devNum;
    GT_PORT_NUM       port;
    GT_BOOL     enable;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    port   = (GT_PORT_NUM)inArgs[1];
    enable = (GT_BOOL)inArgs[2];

    CONVERT_DEV_PORT_U32_MAC(devNum, port);

    /* call cpss api function */
    result = cpssDxChBrgPrvEdgeVlanPortControlPktsToUplinkSet(devNum, port, enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}

/*******************************************************************************
* cpssDxChBrgPrvEdgeVlanPortControlPktsToUplinkGet
*
* DESCRIPTION:
*       Get Enable state per Port forwarding control taffic to Private Edge
*       VLAN Uplink. PVE port can be disabled from trapping or mirroring
*       bridged packets to the CPU. In this case, as long as the packet is not
*       assigned a HARD DROP or SOFT DROP, the packet is unconditionally
*       assigned a FORWARD command with the ingress ports configured
*       PVE destination (PVLAN Uplink).
*
* APPLICABLE DEVICES:  All DXCH2 devices
*
* INPUTS:
*       devNum  - device number
*       port    - physical or CPU port to set.
*
* OUTPUTS:
*       enablePtr GT_TRUE  - forward control traffic to PVE Uplink
*                 GT_FALSE - not forward control traffic to PVE Uplink,
*                            trapped or mirrored to CPU packets are sent to
*                            CPU port
*
* RETURNS:
*       GT_OK             - on success
*       GT_BAD_PARAM      - wrong devNum, port
*       GT_HW_ERROR       - on hardware error
*
* COMMENTS:
*
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChBrgPrvEdgeVlanPortControlPktsToUplinkGet

(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8       devNum;
    GT_PORT_NUM       port;
    GT_BOOL     enable;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    port   = (GT_PORT_NUM)inArgs[1];

    CONVERT_DEV_PORT_U32_MAC(devNum, port);

    /* call cpss api function */
    result = cpssDxChBrgPrvEdgeVlanPortControlPktsToUplinkGet(devNum, port, &enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", enable);
    return CMD_OK;
}

/**** database initialization **************************************/

static CMD_COMMAND dbCommands[] =
{
    {"cpssDxChBrgPrvEdgeVlanEnable",
        &wrCpssDxChBrgPrvEdgeVlanEnable,
        2, 0},

    {"cpssDxChBrgPrvEdgeVlanEnableGet",
        &wrCpssDxChBrgPrvEdgeVlanEnableGet,
        1, 0},

    {"cpssDxChBrgPrvEdgeVlanPortEnable",
        &wrCpssDxChBrgPrvEdgeVlanPortEnable,
        6, 0},

    {"cpssDxChBrgPrvEdgeVlanPortEnableGet",
        &wrCpssDxChBrgPrvEdgeVlanPortEnableGet,
        2, 0},

    {"cpssDxChBrgPrvEdgeVlanPortGetFirst",
        &wrCpssDxChBrgPrvEdgeVlanPortTableGetFirst,
        1, 0},

    {"cpssDxChBrgPrvEdgeVlanPortGetNext",
        &wrCpssDxChBrgPrvEdgeVlanPortTableGetNext,
        1, 0},

    {"cpssDxChBrgPrvEdgeVlanPortControlPktsToUplinkSet",
        &wrCpssDxChBrgPrvEdgeVlanPortControlPktsToUplinkSet,
        3, 0},

    {"cpssDxChBrgPrvEdgeVlanPortControlPktsToUplinkGet",
        &wrCpssDxChBrgPrvEdgeVlanPortControlPktsToUplinkGet,
        2, 0},

};

#define numCommands (sizeof(dbCommands) / sizeof(CMD_COMMAND))

/*******************************************************************************
* cmdLibInitCpssDxChBridgePrvEdgeVlan
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
GT_STATUS cmdLibInitCpssDxChBridgePrvEdgeVlan
(
    GT_VOID
)
{
    return cmdInitLibrary(dbCommands, numCommands);
}


