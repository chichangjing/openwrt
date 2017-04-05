/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* wrapCpssDxChIp.c
*
* DESCRIPTION:
*       Wrapper functions for Cpss DxChXcat Logical Target Mapping
*
* DEPENDENCIES:
*       None.
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
#include <cpss/extServices/private/prvCpssBindFunc.h>
#include <cpss/generic/cpssTypes.h>
#include <cpss/dxCh/dxChxGen/logicalTarget/cpssDxChLogicalTargetMapping.h>

/*****************************Commands*******************************/

/*******************************************************************************
* cpssDxChLogicalTargetMappingEnableSet
*
* DESCRIPTION:
*       Enable/disable logical port mapping feature on the specified device.
*
* APPLICABLE DEVICES:  DxChXcat and above
*
*
* INPUTS:
*       devNum   - physical device number
*       enable   - GT_TRUE:  enable
*                  GT_FALSE: disable .
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong devNum
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChLogicalTargetMappingEnableSet

(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS    result;

    GT_U8        devNum;
    GT_BOOL      enable;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    enable = (GT_BOOL)inArgs[1];

    /* call cpss api function */
    result = cpssDxChLogicalTargetMappingEnableSet(devNum, enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}


/*******************************************************************************
* cpssDxChLogicalTargetMappingEnableGet
*
* DESCRIPTION:
*       Get the Enable/Disable status logical port mapping feature on the
*       specified  device.
*
* APPLICABLE DEVICES:  DxChXcat and above
*
* INPUTS:
*       devNum    - physical device number
*
* OUTPUTS:
*       enablePtr - Pointer to the enable/disable state.
*                   GT_TRUE : enable,
*                   GT_FALSE: disable
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong devNum
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChLogicalTargetMappingEnableGet

(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS    result;

    GT_U8        devNum;
    GT_BOOL      enablePtr;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];

    /* call cpss api function */
    result = cpssDxChLogicalTargetMappingEnableGet(devNum, &enablePtr);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", enablePtr);
    return CMD_OK;
}


/*******************************************************************************
* cpssDxChLogicalTargetMappingDeviceEnableSet
*
* DESCRIPTION:
*       Enable/disable a target device to be considered as a logical device
*       on the specified device.
*
* APPLICABLE DEVICES:  DxChXcat and above
*
*
* INPUTS:
*       devNum        - physical device number
*       logicalDevNum - logical device number.
*                       Range (24-31)
*       enable        - GT_TRUE:  enable
*                       GT_FALSE: disable .
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong devNum, logicalDevNum
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChLogicalTargetMappingDeviceEnableSet

(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS    result;

    GT_U8        devNum;
    GT_U8        logicalDevNum;
    GT_BOOL      enable;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    logicalDevNum = (GT_U8)inArgs[1];
    enable = (GT_BOOL)inArgs[2];

    /* call cpss api function */
    result = cpssDxChLogicalTargetMappingDeviceEnableSet(devNum,
                                                         logicalDevNum,
                                                         enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}



/*******************************************************************************
* cpssDxChLogicalTargetMappingDeviceEnableGet
*
* DESCRIPTION:
*       Get Enable/disable status of target device to be considered as a logical device
*       on the specified device.
*
* APPLICABLE DEVICES:  DxChXcat and above
*
*
* INPUTS:
*       devNum        - physical device number
*       logicalDevNum - logical device number.
*                       Range (24-31)
*
* OUTPUTS:
*       enablePtr     - Pointer to the  Enable/disable state.
*                       GT_TRUE : enable,
*                       GT_FALSE: disable
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong devNum, logicalDevNum
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChLogicalTargetMappingDeviceEnableGet

(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS    result;

    GT_U8        devNum;
    GT_U8        logicalDevNum;
    GT_BOOL      enablePtr;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    logicalDevNum = (GT_U8)inArgs[1];

    /* call cpss api function */
    result = cpssDxChLogicalTargetMappingDeviceEnableGet(devNum,
                                                         logicalDevNum,
                                                         &enablePtr);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", enablePtr);
    return CMD_OK;
}


/*****************************Tables*******************************/


/*******************************************************************************
* cpssDxChLogicalTargetMappingTableEntrySet
*
* DESCRIPTION:
*       Set logical target mapping table entry.
*
* APPLICABLE DEVICES: DxChXcat and above.
*
* INPUTS:
*       devNum                       - physical device number.
*       logicalDevNum                - logical device number.
*                                      Range (24-31)
*       logicalPortNum               - logical device number.
*                                      Range (0-63)
*       logicalPortMappingTablePtr   - points to logical Port Mapping  entry
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK           - on success.
*       GT_BAD_PARAM    - on wrong devNum, logicalDevNum, logicalPortNum
*       GT_BAD_PTR      - one of the parameters is NULL pointer
*       GT_OUT_OF_RANGE - on the memebers of virtual port entry struct out of range.
*       GT_HW_ERROR     - on Hardware error.
*
* COMMENTS:
*       None.
*******************************************************************************/
static CMD_STATUS wrCpssDxChLogicalTargetMappingTableEntrySet

(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{

    GT_STATUS                                  result;
    GT_U8                                      devNum;
    GT_U8                                      logicalDevNum;
    GT_U8                                      logicalPortNum;
    CPSS_DXCH_LOGICAL_TARGET_MAPPING_STC       logicalPortMappingTable;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    logicalDevNum = (GT_U8)inArgs[1];
    logicalPortNum = (GT_U8)inArgs[2];

    logicalPortMappingTable.outputInterface.isTunnelStart = (GT_BOOL)inFields[0];
    logicalPortMappingTable.outputInterface.tunnelStartInfo.passengerPacketType =
                                (CPSS_DXCH_TUNNEL_PASSANGER_TYPE_ENT)inFields[1];
    logicalPortMappingTable.outputInterface.tunnelStartInfo.ptr = (GT_U32)inFields[2];
    logicalPortMappingTable.outputInterface.physicalInterface.type =
                                            (CPSS_INTERFACE_TYPE_ENT)inFields[3];
    logicalPortMappingTable.outputInterface.physicalInterface.devPort.hwDevNum = (GT_HW_DEV_NUM)inFields[4];
    logicalPortMappingTable.outputInterface.physicalInterface.devPort.portNum = (GT_PORT_NUM)inFields[5];
    logicalPortMappingTable.outputInterface.physicalInterface.trunkId = (GT_TRUNK_ID)inFields[6];
    if (logicalPortMappingTable.outputInterface.physicalInterface.type == CPSS_INTERFACE_PORT_E )
    {
        CONVERT_DEV_PORT_DATA_MAC(logicalPortMappingTable.outputInterface.physicalInterface.devPort.hwDevNum,
                             logicalPortMappingTable.outputInterface.physicalInterface.devPort.portNum);

    }
    if (logicalPortMappingTable.outputInterface.physicalInterface.type == CPSS_INTERFACE_TRUNK_E )
    {
        CONVERT_TRUNK_ID_TEST_TO_CPSS_MAC(logicalPortMappingTable.outputInterface.physicalInterface.trunkId);
    }
    logicalPortMappingTable.outputInterface.physicalInterface.vidx = (GT_U16)inFields[7];

   /* call cpss api function */
    result = cpssDxChLogicalTargetMappingTableEntrySet(devNum,
                                                       logicalDevNum,
                                                       logicalPortNum,
                                                       &logicalPortMappingTable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}



/*******************************************************************************
* cpssDxChLogicalTargetMappingTableEntryGet
*
* DESCRIPTION:
*       Get logical target mapping table entry.
*
* APPLICABLE DEVICES: DxChXcat and above.
*
* INPUTS:
*       devNum                       - physical device number.
*       logicalDevNum                - logical device number.
*                                      Range (24-31)
*       logicalPortNum               - logical device number.
*                                      Range (0-63)
*
* OUTPUTS:
*       logicalPortMappingTablePtr   - points to logical Port Mapping  entry
*
* RETURNS:
*       GT_OK           - on success.
*       GT_BAD_PARAM    - on wrong devNum, logicalDevNum, logicalPortNum
*       GT_BAD_PTR      - one of the parameters is NULL pointer
*       GT_OUT_OF_RANGE - on the memebers of virtual port entry struct out of range.
*       GT_HW_ERROR     - on Hardware error.
*
* COMMENTS:
*       None.
*******************************************************************************/
static CMD_STATUS wrCpssDxChLogicalTargetMappingTableEntryGetFirst
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{

    GT_STATUS                                  result;
    GT_U8                                      devNum;
    GT_U8                                      logicalDevNum;
    GT_U8                                      logicalPortNum;
    CPSS_DXCH_LOGICAL_TARGET_MAPPING_STC       logicalPortMappingTable;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    logicalDevNum = (GT_U8)inArgs[1];
    logicalPortNum = (GT_U8)inArgs[2];

    cmdOsMemSet(&logicalPortMappingTable, 0,sizeof(CPSS_DXCH_LOGICAL_TARGET_MAPPING_STC));

    result = cpssDxChLogicalTargetMappingTableEntryGet(devNum,
                                                       logicalDevNum,
                                                       logicalPortNum,
                                                       &logicalPortMappingTable);

    if (result != GT_OK)
    {
        galtisOutput(outArgs, result, "%d", -1);
        return CMD_OK;
    }

    inFields[0] = logicalPortMappingTable.outputInterface.isTunnelStart;
    inFields[1] = logicalPortMappingTable.outputInterface.tunnelStartInfo.passengerPacketType;
    inFields[2] = logicalPortMappingTable.outputInterface.tunnelStartInfo.ptr;
    inFields[3] = logicalPortMappingTable.outputInterface.physicalInterface.type;
    CONVERT_BACK_DEV_PORT_DATA_MAC(logicalPortMappingTable.outputInterface.physicalInterface.devPort.hwDevNum,
                                   logicalPortMappingTable.outputInterface.physicalInterface.devPort.portNum);
    inFields[4] = logicalPortMappingTable.outputInterface.physicalInterface.devPort.hwDevNum;
    inFields[5] = logicalPortMappingTable.outputInterface.physicalInterface.devPort.portNum;
    CONVERT_TRUNK_ID_CPSS_TO_TEST_MAC(logicalPortMappingTable.outputInterface.physicalInterface.trunkId);
    inFields[6] = logicalPortMappingTable.outputInterface.physicalInterface.trunkId;
    inFields[7] = logicalPortMappingTable.outputInterface.physicalInterface.vidx;
    inFields[8] = 0;
    /* pack and output table fields */
    fieldOutput("%d%d%d%d%d%d%d%d%d", inFields[0], inFields[1], inFields[2],
                                    inFields[3], inFields[4], inFields[5],
                                    inFields[6], inFields[7], inFields[8] );

    galtisOutput(outArgs, GT_OK, "%f");

    return CMD_OK;
}


/*******************************************************************************
* wrCpssDxChDummyGetNext
*
* DESCRIPTION:
*   The function used to return "No Next Table Entry"
*
* COMMENTS: Generic
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChDummyGetNext
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    galtisOutput(outArgs, GT_OK, "%d", -1);

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChLogicalTargetMappingTableEntrySet
*
* DESCRIPTION:
*       Set logical target mapping table entry.
*
* APPLICABLE DEVICES: DxChXcat and above.
*
* INPUTS:
*       devNum                       - physical device number.
*       logicalDevNum                - logical device number.
*                                      Range (24-31)
*       logicalPortNum               - logical device number.
*                                      Range (0-63)
*       logicalPortMappingTablePtr   - points to logical Port Mapping  entry
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK           - on success.
*       GT_BAD_PARAM    - on wrong devNum, logicalDevNum, logicalPortNum
*       GT_BAD_PTR      - one of the parameters is NULL pointer
*       GT_OUT_OF_RANGE - on the memebers of virtual port entry struct out of range.
*       GT_HW_ERROR     - on Hardware error.
*
* COMMENTS:
*       None.
*******************************************************************************/
static CMD_STATUS wrCpssDxChLogicalTargetMappingTableEntrySet_1

(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{

    GT_STATUS                                  result;
    GT_U8                                      devNum;
    GT_U8                                      logicalDevNum;
    GT_U8                                      logicalPortNum;
    CPSS_DXCH_LOGICAL_TARGET_MAPPING_STC       logicalPortMappingTable;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    logicalDevNum = (GT_U8)inArgs[1];
    logicalPortNum = (GT_U8)inArgs[2];

    logicalPortMappingTable.outputInterface.isTunnelStart = (GT_BOOL)inFields[0];
    logicalPortMappingTable.outputInterface.tunnelStartInfo.passengerPacketType =
                                (CPSS_DXCH_TUNNEL_PASSANGER_TYPE_ENT)inFields[1];
    logicalPortMappingTable.outputInterface.tunnelStartInfo.ptr = (GT_U32)inFields[2];
    logicalPortMappingTable.outputInterface.physicalInterface.type =
                                            (CPSS_INTERFACE_TYPE_ENT)inFields[3];
    logicalPortMappingTable.outputInterface.physicalInterface.devPort.hwDevNum = (GT_HW_DEV_NUM)inFields[4];
    logicalPortMappingTable.outputInterface.physicalInterface.devPort.portNum = (GT_PORT_NUM)inFields[5];
    logicalPortMappingTable.outputInterface.physicalInterface.trunkId = (GT_TRUNK_ID)inFields[6];
    if (logicalPortMappingTable.outputInterface.physicalInterface.type == CPSS_INTERFACE_PORT_E )
    {
        CONVERT_DEV_PORT_DATA_MAC(logicalPortMappingTable.outputInterface.physicalInterface.devPort.hwDevNum,
                             logicalPortMappingTable.outputInterface.physicalInterface.devPort.portNum);

    }
    if (logicalPortMappingTable.outputInterface.physicalInterface.type == CPSS_INTERFACE_TRUNK_E )
    {
        CONVERT_TRUNK_ID_TEST_TO_CPSS_MAC(logicalPortMappingTable.outputInterface.physicalInterface.trunkId);
    }
    logicalPortMappingTable.outputInterface.physicalInterface.vidx = (GT_U16)inFields[7];
    logicalPortMappingTable.outputInterface.physicalInterface.hwDevNum = (GT_HW_DEV_NUM)inFields[8];
    logicalPortMappingTable.egressVlanFilteringEnable = (GT_BOOL)inFields[9];
    logicalPortMappingTable.egressVlanTagStateEnable = (GT_BOOL)inFields[10];
    logicalPortMappingTable.egressVlanTagState = (CPSS_DXCH_BRG_VLAN_PORT_TAG_CMD_ENT)inFields[11];
    logicalPortMappingTable.egressTagTpidIndex = (GT_U32)inFields[12];
    logicalPortMappingTable.assignVid0Enable = (GT_BOOL)inFields[13];
    logicalPortMappingTable.vid0 = (GT_U16)inFields[14];

   /* call cpss api function */
    result = cpssDxChLogicalTargetMappingTableEntrySet(devNum,
                                                       logicalDevNum,
                                                       logicalPortNum,
                                                       &logicalPortMappingTable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}



/*******************************************************************************
* cpssDxChLogicalTargetMappingTableEntryGet
*
* DESCRIPTION:
*       Get logical target mapping table entry.
*
* APPLICABLE DEVICES: DxChXcat and above.
*
* INPUTS:
*       devNum                       - physical device number.
*       logicalDevNum                - logical device number.
*                                      Range (24-31)
*       logicalPortNum               - logical device number.
*                                      Range (0-63)
*
* OUTPUTS:
*       logicalPortMappingTablePtr   - points to logical Port Mapping  entry
*
* RETURNS:
*       GT_OK           - on success.
*       GT_BAD_PARAM    - on wrong devNum, logicalDevNum, logicalPortNum
*       GT_BAD_PTR      - one of the parameters is NULL pointer
*       GT_OUT_OF_RANGE - on the memebers of virtual port entry struct out of range.
*       GT_HW_ERROR     - on Hardware error.
*
* COMMENTS:
*       None.
*******************************************************************************/
static CMD_STATUS wrCpssDxChLogicalTargetMappingTableEntryGetFirst_1
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{

    GT_STATUS                                  result;
    GT_U8                                      devNum;
    GT_U8                                      logicalDevNum;
    GT_U8                                      logicalPortNum;
    CPSS_DXCH_LOGICAL_TARGET_MAPPING_STC       logicalPortMappingTable;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    logicalDevNum = (GT_U8)inArgs[1];
    logicalPortNum = (GT_U8)inArgs[2];

    cmdOsMemSet(&logicalPortMappingTable, 0,sizeof(CPSS_DXCH_LOGICAL_TARGET_MAPPING_STC));

    result = cpssDxChLogicalTargetMappingTableEntryGet(devNum,
                                                       logicalDevNum,
                                                       logicalPortNum,
                                                       &logicalPortMappingTable);

    if (result != GT_OK)
    {
        galtisOutput(outArgs, result, "%d", -1);
        return CMD_OK;
    }

    inFields[0] = logicalPortMappingTable.outputInterface.isTunnelStart;
    inFields[1] = logicalPortMappingTable.outputInterface.tunnelStartInfo.passengerPacketType;
    inFields[2] = logicalPortMappingTable.outputInterface.tunnelStartInfo.ptr;
    inFields[3] = logicalPortMappingTable.outputInterface.physicalInterface.type;
    CONVERT_BACK_DEV_PORT_DATA_MAC(logicalPortMappingTable.outputInterface.physicalInterface.devPort.hwDevNum,
                                   logicalPortMappingTable.outputInterface.physicalInterface.devPort.portNum);
    inFields[4] = logicalPortMappingTable.outputInterface.physicalInterface.devPort.hwDevNum;
    inFields[5] = logicalPortMappingTable.outputInterface.physicalInterface.devPort.portNum;
    CONVERT_TRUNK_ID_CPSS_TO_TEST_MAC(logicalPortMappingTable.outputInterface.physicalInterface.trunkId);
    inFields[6] = logicalPortMappingTable.outputInterface.physicalInterface.trunkId;
    inFields[7] = logicalPortMappingTable.outputInterface.physicalInterface.vidx;
    inFields[8] = 0;
    inFields[9] = logicalPortMappingTable.egressVlanFilteringEnable;
    inFields[10] = logicalPortMappingTable.egressVlanTagStateEnable;
    inFields[11] = logicalPortMappingTable.egressVlanTagState;
    inFields[12] = logicalPortMappingTable.egressTagTpidIndex;
    inFields[13] = logicalPortMappingTable.assignVid0Enable;
    inFields[14] = logicalPortMappingTable.vid0;
    /* pack and output table fields */
    fieldOutput("%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d", inFields[0], inFields[1], inFields[2],
                inFields[3], inFields[4], inFields[5], inFields[6], inFields[7],
                inFields[8], inFields[9], inFields[10], inFields[11], inFields[12],
                inFields[14], inFields[13] );

    galtisOutput(outArgs, GT_OK, "%f");

    return CMD_OK;
}


/*******************************************************************************
* cpssDxChLogicalTargetMappingEgressVlanMemberAccessModeSet
*
* DESCRIPTION:
*      This function sets the mode in which the Logical Port Egress VLAN member
*      table is accessed, according to the amount of VIDs and Logical Ports in
*      the system.
*
* APPLICABLE DEVICES:
*        xCat.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; Lion; xCat2.
*
* INPUTS:
*       devNum  - the device number
*       mode    - value to set.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong input parameters
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       xCat device: applicable starting from revision C0
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChLogicalTargetMappingEgressVlanMemberAccessModeSet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS    result;
    GT_U8        devNum;
    CPSS_DXCH_LOGICAL_TARGET_MAPPING_EGRESS_VLAN_MEMBER_ACCESS_MODE_ENT    mode;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    mode   = (CPSS_DXCH_LOGICAL_TARGET_MAPPING_EGRESS_VLAN_MEMBER_ACCESS_MODE_ENT)inArgs[1];

    /* call cpss api function */
    result = cpssDxChLogicalTargetMappingEgressVlanMemberAccessModeSet(devNum, mode);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}

/*******************************************************************************
* cpssDxChLogicalTargetMappingEgressVlanMemberAccessModeGet
*
* DESCRIPTION:
*      This function gets the mode in which the Logical Port Egress VLAN member
*      table is accessed, according to the amount of VIDs and Logical Ports in
*      the system.
*
* APPLICABLE DEVICES:
*        xCat.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; Lion; xCat2.
*
* INPUTS:
*       devNum  - the device number
*
* OUTPUTS:
*       modePtr - (pointer to) value to get.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong input parameters
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_BAD_PTR      - one of the parameters is NULL pointer
*
* COMMENTS:
*       xCat device: applicable starting from revision C0
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChLogicalTargetMappingEgressVlanMemberAccessModeGet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS    result;
    GT_U8        devNum;
    CPSS_DXCH_LOGICAL_TARGET_MAPPING_EGRESS_VLAN_MEMBER_ACCESS_MODE_ENT modePtr;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];

    /* call cpss api function */
    result = cpssDxChLogicalTargetMappingEgressVlanMemberAccessModeGet(devNum, &modePtr);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", modePtr);
    return CMD_OK;
}

/*******************************************************************************
* cpssDxChLogicalTargetMappingEgressVlanFilteringDropCounterSet
*
* DESCRIPTION:
*      This function sets the number of packets dropped due to Logical Port
*      Egress VLAN Filtering.
*
* APPLICABLE DEVICES:
*        xCat.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; Lion; xCat2.
*
* INPUTS:
*       devNum  - the device number
*       counter - value to set counter.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong input parameters
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       xCat device: applicable starting from revision C0
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChLogicalTargetMappingEgressVlanFilteringDropCounterSet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS   result;
    GT_U8       devNum;
    GT_U32      counter;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum  = (GT_U8)inArgs[0];
    counter = (GT_U32)inArgs[1];

    /* call cpss api function */
    result = cpssDxChLogicalTargetMappingEgressVlanFilteringDropCounterSet(devNum, counter);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}
/*******************************************************************************
* cpssDxChLogicalTargetMappingEgressVlanFilteringDropCounterGet
*
* DESCRIPTION:
*      This function gets the number of packets dropped due to Logical Port
*      Egress VLAN Filtering.
*
* APPLICABLE DEVICES:
*        xCat.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; Lion; xCat2.
*
* INPUTS:
*       devNum     - the device number
*
* OUTPUTS:
*       counterPtr - (pointer to) counter value.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong input parameters
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       xCat device: applicable starting from revision C0
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChLogicalTargetMappingEgressVlanFilteringDropCounterGet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS   result;
    GT_U8       devNum;
    GT_U32      counterPtr;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];

    /* call cpss api function */
    result = cpssDxChLogicalTargetMappingEgressVlanFilteringDropCounterGet(devNum, &counterPtr);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", counterPtr);
    return CMD_OK;
}

/*******************************************************************************
* cpssDxChLogicalTargetMappingEgressMappingEntrySet
*
* DESCRIPTION:
*       Function sets the mapping of any discrete VIDs of the 4K VLAN range,
*       into a continuous range of VIDs.
*       Used to accommodate the cases where the 12-bit VLAN-ID must be mapped
*       into a smaller internal VID index.
*
* APPLICABLE DEVICES:
*        xCat.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; Lion; xCat2.
*
* INPUTS:
*       devNum          - the device number
*       vlanId          - VLAN id, used as index in the Mapping Table.
*                         (APPLICABLE RANGES: 0..4095)
*       vlanIdIndex     - VLAN id index, use as value in the Mapping Table.
*                         (APPLICABLE RANGES: 0..4095)
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong input parameters
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       xCat device: applicable starting from revision C0
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChLogicalTargetMappingEgressMappingEntrySet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS    result;
    GT_U8        devNum;
    GT_U16       vlanId;
    GT_U32       vlanIdIndex;

/* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum      = (GT_U8)inArgs[0];
    vlanId      = (GT_U16)inArgs[1];
    vlanIdIndex = (GT_U32)inArgs[2];

    /* call cpss api function */
    result = cpssDxChLogicalTargetMappingEgressMappingEntrySet(devNum, vlanId, vlanIdIndex);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}

/*******************************************************************************
* cpssDxChLogicalTargetMappingEgressMappingEntryGet
*
* DESCRIPTION:
*       Function gets the mapping of any discrete VIDs of the 4K VLAN range,
*       into a continuous range of VIDs.
*       Used to accommodate the cases where the 12-bit VLAN-ID must be mapped
*       into a smaller internal VID index.
*
* APPLICABLE DEVICES:
*        xCat.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; Lion; xCat2.
*
* INPUTS:
*       devNum          - the device number
*       vlanId          - VLAN id, used as index in the Mapping Table.
*                         (APPLICABLE RANGES: 0..4095)
*
* OUTPUTS:
*       vlanIdIndexPtr  - (pointer to) VLAN id index, use as value in
*                         the Mapping Table. (APPLICABLE RANGES: 0..4095)
*
* RETURNS:
*       GT_OK                    - on success
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong input parameters
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*
* COMMENTS:
*       xCat device: applicable starting from revision C0
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChLogicalTargetMappingEgressMappingEntryGet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS   result;
    GT_U8       devNum;
    GT_U16      vlanId;
    GT_U32      vlanIdIndexPtr;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    vlanId = (GT_U16)inArgs[1];

    /* call cpss api function */
    result = cpssDxChLogicalTargetMappingEgressMappingEntryGet(devNum, vlanId, &vlanIdIndexPtr);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", vlanIdIndexPtr);
    return CMD_OK;
}

/*******************************************************************************
* cpssDxChLogicalTargetMappingEgressMemberSet
*
* DESCRIPTION:
*       Function sets whether target logical port is a member of a given VLAN.
*
* APPLICABLE DEVICES:
*        xCat.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; Lion; xCat2.
*
* INPUTS:
*       devNum          - the device number
*       vlanIdIndex     - VLAN id index, use as value in the Mapping Table.
*                         (APPLICABLE RANGES: 0..4095)
*       logicalDevNum   - logical target device number. (APPLICABLE RANGES: 0..31)
*       logicalPortNum  - logical target port number. (APPLICABLE RANGES: 0..63)
*       isMember        - whether target logical port is member of a given VLAN.
*                         GT_TRUE: target logical port is member of a given VLAN
*                         GT_FALSE: target logical port isn't member of a given VLAN
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong input parameters
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       xCat device: applicable starting from revision C0
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChLogicalTargetMappingEgressMemberSet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS       result;
    GT_U8           devNum;
    GT_U32          vlanIdIndex;
    GT_HW_DEV_NUM   logicalDevNum;
    GT_PORT_NUM     logicalPortNum;
    GT_BOOL         isMember;

     /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum          = (GT_U8)inArgs[0];
    vlanIdIndex     = (GT_U32)inArgs[1];
    logicalDevNum   = (GT_HW_DEV_NUM)inArgs[2];
    logicalPortNum  = (GT_PORT_NUM)inArgs[3];
    isMember        = (GT_BOOL)inArgs[4];

    /* Override Device and Port */
    CONVERT_DEV_PORT_DATA_MAC(logicalDevNum, logicalPortNum);

    /* call cpss api function */
    result = cpssDxChLogicalTargetMappingEgressMemberSet(devNum, vlanIdIndex, (GT_U8)logicalDevNum, (GT_U8)logicalPortNum, isMember);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}

/*******************************************************************************
* cpssDxChLogicalTargetMappingEgressMemberGet
*
* DESCRIPTION:
*       Function gets whether target logical port is a member of a given VLAN.
*
* APPLICABLE DEVICES:
*        xCat.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; Lion; xCat2.
*
* INPUTS:
*       devNum          - the device number
*       vlanIdIndex     - VLAN id index, use as value in the Mapping Table.
*                         (APPLICABLE RANGES: 0..4095)
*       logicalDevNum   - logical target device number. (APPLICABLE RANGES: 0..31)
*       logicalPortNum  - logical target port number. (APPLICABLE RANGES: 0..63)
*
* OUTPUTS:
*       isMemberPtr     - (Pointer to) whether target logical port is member
*              of a give VLAN.
*                         GT_TRUE: target logical port is member of a given VLAN
*                         GT_FALSE: target logical port is not member of a given VLAN
*
* RETURNS:
*       GT_OK                    - on success
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong input parameters
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_BAD_PTR               - on NULL pointer
*
* COMMENTS:
*       xCat device: applicable starting from revision C0
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChLogicalTargetMappingEgressMemberEnableGet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS       result;
    GT_U8           devNum;
    GT_U32          vlanIdIndex;
    GT_HW_DEV_NUM   logicalDevNum;
    GT_PORT_NUM     logicalPortNum;
    GT_BOOL         isMemberPtr;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum          = (GT_U8)inArgs[0];
    vlanIdIndex     = (GT_U32)inArgs[1];
    logicalDevNum   = (GT_HW_DEV_NUM)inArgs[2];
    logicalPortNum  = (GT_PORT_NUM)inArgs[3];

    /* Override Device and Port */
    CONVERT_DEV_PORT_DATA_MAC(logicalDevNum, logicalPortNum);

    /* call cpss api function */
    result = cpssDxChLogicalTargetMappingEgressMemberGet(devNum, vlanIdIndex, (GT_U8)logicalDevNum, (GT_U8)logicalPortNum, &isMemberPtr);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", isMemberPtr);
    return CMD_OK;
}

/*******************************************************************************
* cpssDxChLogicalTargetMappingEgressMemberTableClear
*
* DESCRIPTION:
*       Function clears Logical Port Egress VLAN member table.
*
* APPLICABLE DEVICES:
*        xCat.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; Lion; xCat2.
*
* INPUTS:
*       devNum          - the device number
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong input parameters
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       xCat device: applicable starting from revision C0
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChLogicalTargetMappingEgressMemberTableClear
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS    result;
    GT_U8        devNum;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];

    /* call cpss api function */
    result = cpssDxChLogicalTargetMappingEgressMemberTableClear(devNum);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}

/**** database initialization **************************************/

static CMD_COMMAND dbCommands[] =
{
    {"cpssDxChLogicalTargetMappingEnableSet",
        &wrCpssDxChLogicalTargetMappingEnableSet,
        2, 0},
    {"cpssDxChLogicalTargetMappingEnableGet",
        &wrCpssDxChLogicalTargetMappingEnableGet,
        1, 0},
    {"cpssDxChLogicalTargetMappingDeviceEnableSet",
        &wrCpssDxChLogicalTargetMappingDeviceEnableSet,
        3, 0},
    {"cpssDxChLogicalTargetMappingDeviceEnableGet",
        &wrCpssDxChLogicalTargetMappingDeviceEnableGet,
        2, 0},
    {"cpssDxChLogicalTargetMappingTableSet",
        &wrCpssDxChLogicalTargetMappingTableEntrySet,
        3, 9},
    {"cpssDxChLogicalTargetMappingTableGetFirst",
        &wrCpssDxChLogicalTargetMappingTableEntryGetFirst,
        3, 0},
    {"cpssDxChLogicalTargetMappingTableGetNext",
        &wrCpssDxChDummyGetNext,
        1, 0},
    {"cpssDxChLogicalTargetMappingTable_1Set",
        &wrCpssDxChLogicalTargetMappingTableEntrySet_1,
        3, 15},
    {"cpssDxChLogicalTargetMappingTable_1GetFirst",
        &wrCpssDxChLogicalTargetMappingTableEntryGetFirst_1,
        3, 0},
    {"cpssDxChLogicalTargetMappingTable_1GetNext",
        &wrCpssDxChDummyGetNext,
        1, 0},
    {"cpssDxChLogicTarMapEgrVlanMemberAccessModeSet",
        &wrCpssDxChLogicalTargetMappingEgressVlanMemberAccessModeSet,
        2, 0},
    {"cpssDxChLogicTarMapEgrVlanMemberAccessModeGet",
        &wrCpssDxChLogicalTargetMappingEgressVlanMemberAccessModeGet,
        1, 0},
    {"cpssDxChLogicTarMapEgrVlanFilteringDropCntSet",
        &wrCpssDxChLogicalTargetMappingEgressVlanFilteringDropCounterSet,
        2, 0},
    {"cpssDxChLogicTarMapEgrVlanFilteringDropCntGet",
        &wrCpssDxChLogicalTargetMappingEgressVlanFilteringDropCounterGet,
        1, 0},
    {"cpssDxChLogicalTargetMappingEgressMappingEntrySet",
        &wrCpssDxChLogicalTargetMappingEgressMappingEntrySet,
        3, 0},
    {"cpssDxChLogicalTargetMappingEgressMappingEntryGet",
        &wrCpssDxChLogicalTargetMappingEgressMappingEntryGet,
        2, 0},
    {"cpssDxChLogicalTargetMappingEgressMemberEnableSet",
        &wrCpssDxChLogicalTargetMappingEgressMemberSet,
        5, 0},
    {"cpssDxChLogicalTargetMappingEgressMemberEnableGet",
        &wrCpssDxChLogicalTargetMappingEgressMemberEnableGet,
        4, 0},
    {"cpssDxChLogicalTargetMappingEgrMemberTableClear",
        &wrCpssDxChLogicalTargetMappingEgressMemberTableClear,
        1, 0}
};


#define numCommands (sizeof(dbCommands) / sizeof(CMD_COMMAND))



/*******************************************************************************
* cmdLibInitCpssDxChLogicalTargetMapping
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
GT_STATUS cmdLibInitCpssDxChLogicalTargetMapping
(
    GT_VOID
)
{
    return cmdInitLibrary(dbCommands, numCommands);
}

