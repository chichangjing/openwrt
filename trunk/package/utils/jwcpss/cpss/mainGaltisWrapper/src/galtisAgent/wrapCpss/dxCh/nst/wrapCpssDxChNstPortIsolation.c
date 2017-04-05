
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
* wrapCpssDxChNstPortIsolation.c
*
* DESCRIPTION:
*       Wrapper functions for NST Port Isolation feature functions
*
* DEPENDENCIES:
*       None.
*
* FILE REVISION NUMBER:
*       $Revision: 17 $
*
*******************************************************************************/

/* Common galtis includes */
#include <galtisAgent/wrapUtil/cmdCpssPresteraUtils.h>
#include <cmdShell/cmdDb/cmdBase.h>
#include <cmdShell/common/cmdWrapUtils.h>


/* Feature specific includes. */
#include <cpss/generic/cpssTypes.h>
#include <cpss/dxCh/dxChxGen/nst/cpssDxChNstPortIsolation.h>

/*******************************************************************************
* cpssDxChNstPortIsolationEnableSet
*
* DESCRIPTION:
*       Function enables/disables the port isolation feature.
*
* APPLICABLE DEVICES:  DxChXcat and above
*
* INPUTS:
*       devNum    - device number
*       enable    - port isolation feature enable/disable
*                   GT_TRUE  - enable
*                   GT_FALSE - disable
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error.
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong devNum
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
CMD_STATUS wrCpssDxChNstPortIsolationEnableSet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS   result;
    GT_U8       devNum;
    GT_BOOL     enable;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    enable = (GT_BOOL)inArgs[1];

    /* call cpss api function */
    result = cpssDxChNstPortIsolationEnableSet(devNum, enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChNstPortIsolationEnableGet
*
* DESCRIPTION:
*       Function gets enabled/disabled state of the port isolation feature.
*
* APPLICABLE DEVICES:  DxChXcat and above
*
* INPUTS:
*       devNum    - device number
*
* OUTPUTS:
*       enablePtr - (pointer to) port isolation feature state
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error.
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong devNum
*       GT_BAD_PTR               - on NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
CMD_STATUS wrCpssDxChNstPortIsolationEnableGet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS   result;
    GT_U8       devNum;
    GT_BOOL     enable;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];

    /* call cpss api function */
    result = cpssDxChNstPortIsolationEnableGet(devNum, &enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", enable);

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChNstPortIsolationTableEntrySet
*
* DESCRIPTION:
*       Function sets port isolation table entry.
*       Each entry represent single source port/device or trunk.
*       Each entry holds bitmap of all local device ports (and CPU port), where
*       for each local port there is a bit marking. If it's a member of source
*       interface (if outgoing traffic from this source interface is allowed to
*       go out at this specific local port).
*
* APPLICABLE DEVICES:  DxChXcat and above
*
* INPUTS:
*       devNum               - device number
*       trafficType          - packets traffic type - L2 or L3
*       srcInterface         - table index is calculated from source interface.
*                              Only portDev and Trunk are supported.
*       cpuPortMember        - port isolation for CPU Port
*                               GT_TRUE - member
*                               GT_FALSE - not member
*       localPortsMembersPtr - (pointer to) port bitmap to be written to the
*                              L2/L3 PI table
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error.
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong devNum, srcInterface or
*                                  localPortsMembersPtr
*       GT_BAD_PTR               - on NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       Traffic is separated for L2 and L3 (means that application may
*       block L2 traffic while allowing L3).
*
*******************************************************************************/
CMD_STATUS wrCpssDxChNstPortIsolationTableEntrySet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                                      result;
    GT_U8                                          devNum;
    CPSS_DXCH_NST_PORT_ISOLATION_TRAFFIC_TYPE_ENT  trafficType;
    CPSS_INTERFACE_INFO_STC                        srcInterface;
    GT_BOOL                                        cpuPortMember;
    CPSS_PORTS_BMP_STC                             localPortsMembers;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    trafficType = (CPSS_DXCH_NST_PORT_ISOLATION_TRAFFIC_TYPE_ENT)inArgs[1];
    cmdOsMemSet(&srcInterface, 0, sizeof(srcInterface));
    srcInterface.type = (CPSS_INTERFACE_TYPE_ENT)inArgs[2];

    switch (inArgs[2])
    {
    case 0:
        srcInterface.devPort.hwDevNum = (GT_HW_DEV_NUM)inArgs[3];
        srcInterface.devPort.portNum = (GT_PORT_NUM)inArgs[4];
        CONVERT_DEV_PORT_DATA_MAC(srcInterface.devPort.hwDevNum ,
                             srcInterface.devPort.portNum);
        break;

    case 1:
        srcInterface.trunkId = (GT_TRUNK_ID)inArgs[5];
        CONVERT_TRUNK_ID_TEST_TO_CPSS_MAC(srcInterface.trunkId);
        break;

    default:
        break;
    }

    cpuPortMember = (GT_BOOL)inArgs[6];
    CPSS_PORTS_BMP_PORT_CLEAR_ALL_MAC(&localPortsMembers);
    localPortsMembers.ports[0] = (GT_U32)inArgs[7];
    localPortsMembers.ports[1] = (GT_U32)inArgs[8];
    CONVERT_DEV_PHYSICAL_PORTS_BMP_MAC(devNum,localPortsMembers);

    /* call cpss api function */
    result = cpssDxChNstPortIsolationTableEntrySet(devNum,
                                                   trafficType,
                                                   srcInterface,
                                                   cpuPortMember,
                                                   &localPortsMembers);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChNstPortIsolationTableEntrySet
*
* DESCRIPTION:
*       Function sets port isolation table entry.
*       Each entry represent single source port/device or trunk.
*       Each entry holds bitmap of all local device ports (and CPU port), where
*       for each local port there is a bit marking. If it's a member of source
*       interface (if outgoing traffic from this source interface is allowed to
*       go out at this specific local port).
*
* APPLICABLE DEVICES:  DxChXcat and above
*
* INPUTS:
*       devNum               - device number
*       trafficType          - packets traffic type - L2 or L3
*       srcInterface         - table index is calculated from source interface.
*                              Only portDev and Trunk are supported.
*       cpuPortMember        - port isolation for CPU Port
*                               GT_TRUE - member
*                               GT_FALSE - not member
*       localPortsMembersPtr - (pointer to) port bitmap to be written to the
*                              L2/L3 PI table
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error.
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong devNum, srcInterface or
*                                  localPortsMembersPtr
*       GT_BAD_PTR               - on NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       Traffic is separated for L2 and L3 (means that application may
*       block L2 traffic while allowing L3).
*
*******************************************************************************/
CMD_STATUS wrCpssDxChNstPortIsolationTableEntrySet1
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                                      result;
    GT_U8                                          devNum;
    CPSS_DXCH_NST_PORT_ISOLATION_TRAFFIC_TYPE_ENT  trafficType;
    CPSS_INTERFACE_INFO_STC                        srcInterface;
    GT_BOOL                                        cpuPortMember;
    CPSS_PORTS_BMP_STC                             localPortsMembers;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    trafficType = (CPSS_DXCH_NST_PORT_ISOLATION_TRAFFIC_TYPE_ENT)inArgs[1];
    cmdOsMemSet(&srcInterface, 0, sizeof(srcInterface));
    srcInterface.type = (CPSS_INTERFACE_TYPE_ENT)inArgs[2];

    switch (inArgs[2])
    {
    case 0:
        srcInterface.devPort.hwDevNum = (GT_HW_DEV_NUM)inArgs[3];
        srcInterface.devPort.portNum = (GT_PORT_NUM)inArgs[4];
        CONVERT_DEV_PORT_DATA_MAC(srcInterface.devPort.hwDevNum ,
                             srcInterface.devPort.portNum);
        break;

    case 1:
        srcInterface.trunkId = (GT_TRUNK_ID)inArgs[5];
        CONVERT_TRUNK_ID_TEST_TO_CPSS_MAC(srcInterface.trunkId);
        break;

    default:
        break;
    }

    cpuPortMember = (GT_BOOL)inArgs[6];
    CPSS_PORTS_BMP_PORT_CLEAR_ALL_MAC(&localPortsMembers);
    localPortsMembers.ports[0] = (GT_U32)inArgs[7];
    localPortsMembers.ports[1] = (GT_U32)inArgs[8];
    localPortsMembers.ports[2] = (GT_U32)inArgs[9];
    localPortsMembers.ports[3] = (GT_U32)inArgs[10];
    localPortsMembers.ports[4] = (GT_U32)inArgs[11];
    localPortsMembers.ports[5] = (GT_U32)inArgs[12];
    localPortsMembers.ports[6] = (GT_U32)inArgs[13];
    localPortsMembers.ports[7] = (GT_U32)inArgs[14];

    CONVERT_DEV_PHYSICAL_PORTS_BMP_MAC(devNum,localPortsMembers);

    /* call cpss api function */
    result = cpssDxChNstPortIsolationTableEntrySet(devNum,
                                                   trafficType,
                                                   srcInterface,
                                                   cpuPortMember,
                                                   &localPortsMembers);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChNstPortIsolationTableEntryGet
*
* DESCRIPTION:
*       Function gets port isolation table entry.
*       Each entry represent single source port/device or trunk.
*       Each entry holds bitmap of all local device ports (and CPU port), where
*       for each local port there is a bit marking if it's a member of source
*       interface (if outgoing traffic from this source interface is allowed to
*       go out at this specific local port).
*
* APPLICABLE DEVICES:  DxChXcat and above
*
* INPUTS:
*       devNum               - device number
*       trafficType          - packets traffic type - L2 or L3
*       srcInterface         - table index is calculated from source interfaces
*                              Only portDev and Trunk are supported.
*
* OUTPUTS:
*       cpuPortMemberPtr     - (pointer to) port isolation for CPU Port
*                                GT_TRUE - member
*                                GT_FALSE - not member
*       localPortsMembersPtr - (pointer to) port bitmap to be written
*                              to the L2/L3 PI table
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error.
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong devNum, srcInterface
*       GT_BAD_PTR               - on NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       Traffic is separated for L2 and L3 (means that application may
*       block L2 traffic while allowing L3).
*
*******************************************************************************/
CMD_STATUS wrCpssDxChNstPortIsolationTableEntryGet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                                      result;
    GT_U8                                          devNum;
    CPSS_DXCH_NST_PORT_ISOLATION_TRAFFIC_TYPE_ENT  trafficType;
    CPSS_INTERFACE_INFO_STC                        srcInterface;
    GT_BOOL                                        cpuPortMember;
    CPSS_PORTS_BMP_STC                             localPortsMembers;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    trafficType = (CPSS_DXCH_NST_PORT_ISOLATION_TRAFFIC_TYPE_ENT)inArgs[1];
    cmdOsMemSet(&srcInterface, 0, sizeof(srcInterface));
    srcInterface.type = (CPSS_INTERFACE_TYPE_ENT)inArgs[2];

    switch (inArgs[2])
    {
    case 0:
        srcInterface.devPort.hwDevNum = (GT_HW_DEV_NUM)inArgs[3];
        srcInterface.devPort.portNum = (GT_PORT_NUM)inArgs[4];
        CONVERT_DEV_PORT_DATA_MAC(srcInterface.devPort.hwDevNum ,
                             srcInterface.devPort.portNum);
        break;

    case 1:
        srcInterface.trunkId = (GT_TRUNK_ID)inArgs[5];
        CONVERT_TRUNK_ID_TEST_TO_CPSS_MAC(srcInterface.trunkId);
        break;

    default:
        break;
    }

    /* call cpss api function */
    result = cpssDxChNstPortIsolationTableEntryGet(devNum,
                                                   trafficType,
                                                   srcInterface,
                                                   &cpuPortMember,
                                                   &localPortsMembers);

    CONVERT_BACK_DEV_PHYSICAL_PORTS_BMP_MAC(devNum,localPortsMembers);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d%d%d", cpuPortMember,
                                            localPortsMembers.ports[0],
                                            localPortsMembers.ports[1]);

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChNstPortIsolationTableEntryGet
*
* DESCRIPTION:
*       Function gets port isolation table entry.
*       Each entry represent single source port/device or trunk.
*       Each entry holds bitmap of all local device ports (and CPU port), where
*       for each local port there is a bit marking if it's a member of source
*       interface (if outgoing traffic from this source interface is allowed to
*       go out at this specific local port).
*
* APPLICABLE DEVICES:  DxChXcat and above
*
* INPUTS:
*       devNum               - device number
*       trafficType          - packets traffic type - L2 or L3
*       srcInterface         - table index is calculated from source interfaces
*                              Only portDev and Trunk are supported.
*
* OUTPUTS:
*       cpuPortMemberPtr     - (pointer to) port isolation for CPU Port
*                                GT_TRUE - member
*                                GT_FALSE - not member
*       localPortsMembersPtr - (pointer to) port bitmap to be written
*                              to the L2/L3 PI table
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error.
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong devNum, srcInterface
*       GT_BAD_PTR               - on NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       Traffic is separated for L2 and L3 (means that application may
*       block L2 traffic while allowing L3).
*
*******************************************************************************/
CMD_STATUS wrCpssDxChNstPortIsolationTableEntryGet1
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                                      result;
    GT_U8                                          devNum;
    CPSS_DXCH_NST_PORT_ISOLATION_TRAFFIC_TYPE_ENT  trafficType;
    CPSS_INTERFACE_INFO_STC                        srcInterface;
    GT_BOOL                                        cpuPortMember;
    CPSS_PORTS_BMP_STC                             localPortsMembers;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    trafficType = (CPSS_DXCH_NST_PORT_ISOLATION_TRAFFIC_TYPE_ENT)inArgs[1];
    cmdOsMemSet(&srcInterface, 0, sizeof(srcInterface));
    cmdOsMemSet(&localPortsMembers, 0, sizeof(localPortsMembers));
    srcInterface.type = (CPSS_INTERFACE_TYPE_ENT)inArgs[2];

    switch (inArgs[2])
    {
    case 0:
        srcInterface.devPort.hwDevNum = (GT_HW_DEV_NUM)inArgs[3];
        srcInterface.devPort.portNum = (GT_PORT_NUM)inArgs[4];
        CONVERT_DEV_PORT_DATA_MAC(srcInterface.devPort.hwDevNum ,
                             srcInterface.devPort.portNum);
        break;

    case 1:
        srcInterface.trunkId = (GT_TRUNK_ID)inArgs[5];
        CONVERT_TRUNK_ID_TEST_TO_CPSS_MAC(srcInterface.trunkId);
        break;

    default:
        break;
    }

    /* call cpss api function */
    result = cpssDxChNstPortIsolationTableEntryGet(devNum,
                                                   trafficType,
                                                   srcInterface,
                                                   &cpuPortMember,
                                                   &localPortsMembers);

    CONVERT_BACK_DEV_PHYSICAL_PORTS_BMP_MAC(devNum,localPortsMembers);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d%d%d%d%d%d%d%d%d", cpuPortMember,
                                                        localPortsMembers.ports[0],
                                                        localPortsMembers.ports[1],
                                                        localPortsMembers.ports[2],
                                                        localPortsMembers.ports[3],
                                                        localPortsMembers.ports[4],
                                                        localPortsMembers.ports[5],
                                                        localPortsMembers.ports[6],
                                                        localPortsMembers.ports[7]);

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChNstPortIsolationPortAdd
*
* DESCRIPTION:
*       Function adds single local port to port isolation table entry.
*       Each entry represent single source port/device or trunk.
*       Adding local port (may be also CPU port 63) to port isolation entry
*       means that traffic which came from srcInterface and wish to egress
*       at the specified local port isn't blocked.
*
* APPLICABLE DEVICES:  DxChXcat and above
*
* INPUTS:
*       devNum        - device number
*       trafficType   - packets traffic type - L2 or L3
*       srcInterface  - table index is calculated from source interface
*                       Only portDev and Trunk are supported.
*       portNum       - local port(include CPU port) to be added to bitmap
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error.
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong devNum, srcInterface, portNum
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
CMD_STATUS wrCpssDxChNstPortIsolationPortAdd
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                                      result;
    GT_U8                                          devNum;
    CPSS_DXCH_NST_PORT_ISOLATION_TRAFFIC_TYPE_ENT  trafficType;
    CPSS_INTERFACE_INFO_STC                        srcInterface;
    GT_PHYSICAL_PORT_NUM                           portNum;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    trafficType = (CPSS_DXCH_NST_PORT_ISOLATION_TRAFFIC_TYPE_ENT)inArgs[1];
    cmdOsMemSet(&srcInterface, 0, sizeof(srcInterface));
    srcInterface.type = (CPSS_INTERFACE_TYPE_ENT)inArgs[2];

    switch (inArgs[2])
    {
    case 0:
        srcInterface.devPort.hwDevNum = (GT_HW_DEV_NUM)inArgs[3];
        srcInterface.devPort.portNum = (GT_PORT_NUM)inArgs[4];
        CONVERT_DEV_PORT_DATA_MAC(srcInterface.devPort.hwDevNum ,
                             srcInterface.devPort.portNum);
        break;

    case 1:
        srcInterface.trunkId = (GT_TRUNK_ID)inArgs[5];
        CONVERT_TRUNK_ID_TEST_TO_CPSS_MAC(srcInterface.trunkId);
        break;

    default:
        break;
    }

    portNum = (GT_PHYSICAL_PORT_NUM)inArgs[6];

    /* Override Device and Port */
    CONVERT_DEV_PHYSICAL_PORT_MAC(devNum, portNum);

    /* call cpss api function */
    result = cpssDxChNstPortIsolationPortAdd(devNum,
                                             trafficType,
                                             srcInterface,
                                             portNum);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChNstPortIsolationPortDelete
*
* DESCRIPTION:
*       Function deletes single local port to port isolation table entry.
*       Each entry represent single source port/device or trunk.
*       Deleting local port (may be also CPU port 63) to port isolation entry
*       means that traffic which came from srcInterface and wish to egress
*       at the specified local port is blocked.
*
* APPLICABLE DEVICES:  DxChXcat and above
*
* INPUTS:
*       devNum        - device number
*       trafficType   - packets traffic type - L2 or L3
*       srcInterface  - table index is calculated from source interface
*                       Only portDev and Trunk are supported.
*       portNum       - local port(include CPU port) to be deleted from bitmap
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error.
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong devNum, srcInterface, portNum
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       In srcInterface parameter only portDev and Trunk are supported.
*
*******************************************************************************/
CMD_STATUS wrCpssDxChNstPortIsolationPortDelete
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                                      result;
    GT_U8                                          devNum;
    CPSS_DXCH_NST_PORT_ISOLATION_TRAFFIC_TYPE_ENT  trafficType;
    CPSS_INTERFACE_INFO_STC                        srcInterface;
    GT_PHYSICAL_PORT_NUM                           portNum;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    trafficType = (CPSS_DXCH_NST_PORT_ISOLATION_TRAFFIC_TYPE_ENT)inArgs[1];
    cmdOsMemSet(&srcInterface, 0, sizeof(srcInterface));
    srcInterface.type = (CPSS_INTERFACE_TYPE_ENT)inArgs[2];

    switch (inArgs[2])
    {
    case 0:
        srcInterface.devPort.hwDevNum = (GT_HW_DEV_NUM)inArgs[3];
        srcInterface.devPort.portNum = (GT_PORT_NUM)inArgs[4];
        CONVERT_DEV_PORT_DATA_MAC(srcInterface.devPort.hwDevNum ,
                             srcInterface.devPort.portNum);
        break;

    case 1:
        srcInterface.trunkId = (GT_TRUNK_ID)inArgs[5];
        CONVERT_TRUNK_ID_TEST_TO_CPSS_MAC(srcInterface.trunkId);
        break;

    default:
        break;
    }

    portNum = (GT_PHYSICAL_PORT_NUM)inArgs[6];

    /* Override Device and Port */
    CONVERT_DEV_PHYSICAL_PORT_MAC(devNum, portNum);

    /* call cpss api function */
    result = cpssDxChNstPortIsolationPortDelete(devNum,
                                                trafficType,
                                                srcInterface,
                                                portNum);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChNstPortIsolationLookupBitsSet
*
* DESCRIPTION:
*       Set the number of bits from the source Interface that are used to
*       index the port isolation table.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum          - device number
*       numberOfPortBits    - number of least significant bits of src port (APPLICABLE RANGES: 0..15)
*       numberOfDeviceBits  - number of least significant bits of src device (APPLICABLE RANGES: 0..15)
*       numberOfTrunkBits   - number of least significant bits of src trunk  (APPLICABLE RANGES: 0..15)
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - wrong device.
*       GT_OUT_OF_RANGE          - on value out of range for one of:
*                                   numberOfPortBits > 15 , numberOfDeviceBits > 15 ,
*                                   numberOfTrunkBits > 15
*       GT_HW_ERROR              - on writing to HW error.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       For example, if the value of this field is 7, and <Port Isolation
*       Lookup Device Bits> is 5, then the index to the port isolation table is
*       {SrcDev[4:0], OrigSRCePort[6:0]}.
*       Note:
*           If <Port Isolation Lookup Port Bits> is 0x0, no bits from the source
*               port are used.
*           If <Port Isolation Lookup Device Bits> is 0x0, no bits from the
*               SrcDev are used.
*
*******************************************************************************/
CMD_STATUS wrCpssDxChNstPortIsolationLookupBitsSet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS   result;
    GT_U8       devNum;
    GT_U32      numberOfPortBits;
    GT_U32      numberOfDeviceBits;
    GT_U32      numberOfTrunkBits;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    numberOfPortBits = (GT_U32)inArgs[1];
    numberOfDeviceBits = (GT_U32)inArgs[2];
    numberOfTrunkBits = (GT_U32)inArgs[3];

    /* call cpss api function */
    result = cpssDxChNstPortIsolationLookupBitsSet(devNum, numberOfPortBits, numberOfDeviceBits, numberOfTrunkBits);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChNstPortIsolationLookupBitsGet
*
* DESCRIPTION:
*       Get the number of bits from the source Interface that are used to
*       index the port isolation table.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum          - device number
*
* OUTPUTS:
*       numberOfPortBitsPtr    - (pointer to)number of least significant bits of src port (APPLICABLE RANGES: 0..15)
*       numberOfDeviceBitsPtr  - (pointer to)number of least significant bits of src device (APPLICABLE RANGES: 0..15)
*       numberOfTrunkBitsPtr   - (pointer to)number of least significant bits of src trunk  (APPLICABLE RANGES: 0..15)
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - wrong device.
*       GT_BAD_PTR               - on NULL pointer.
*       GT_HW_ERROR              - on writing to HW error.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
CMD_STATUS wrCpssDxChNstPortIsolationLookupBitsGet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS   result;
    GT_U8       devNum;
    GT_U32      numberOfPortBits;
    GT_U32      numberOfDeviceBits;
    GT_U32      numberOfTrunkBits;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];

    /* call cpss api function */
    result = cpssDxChNstPortIsolationLookupBitsGet(devNum, &numberOfPortBits, &numberOfDeviceBits, &numberOfTrunkBits);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d%d%d", numberOfPortBits, numberOfDeviceBits, numberOfTrunkBits);

    return CMD_OK;
}

/**** database initialization **************************************/

static CMD_COMMAND dbCommands[] =
{
    {"cpssDxChNstPortIsolationEnableSet",
        &wrCpssDxChNstPortIsolationEnableSet,
        2, 0},

    {"cpssDxChNstPortIsolationEnableGet",
        &wrCpssDxChNstPortIsolationEnableGet,
        1, 0},

    {"cpssDxChNstPortIsolationTableEntrySet",
        &wrCpssDxChNstPortIsolationTableEntrySet,
        9, 0},

    {"cpssDxChNstPortIsolationTableEntrySet1",
        &wrCpssDxChNstPortIsolationTableEntrySet1,
        15, 0},

    {"cpssDxChNstPortIsolationTableEntryGet",
        &wrCpssDxChNstPortIsolationTableEntryGet,
        6, 0},

    {"cpssDxChNstPortIsolationTableEntryGet1",
        &wrCpssDxChNstPortIsolationTableEntryGet1,
        6, 0},

    {"cpssDxChNstPortIsolationPortAdd",
        &wrCpssDxChNstPortIsolationPortAdd,
        7, 0},

    {"cpssDxChNstPortIsolationPortDelete",
        &wrCpssDxChNstPortIsolationPortDelete,
        7, 0},
    {"cpssDxChNstPortIsolationLookupBitsSet",
        &wrCpssDxChNstPortIsolationLookupBitsSet,
        4, 0},

    {"cpssDxChNstPortIsolationLookupBitsGet",
        &wrCpssDxChNstPortIsolationLookupBitsGet,
        1, 0}
};

#define numCommands (sizeof(dbCommands) / sizeof(CMD_COMMAND))

/*******************************************************************************
* cmdLibInitCpssDxChNstPortIsolation
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
GT_STATUS cmdLibInitCpssDxChNstPortIsolation
(
    GT_VOID
)
{
    return cmdInitLibrary(dbCommands, numCommands);
}
