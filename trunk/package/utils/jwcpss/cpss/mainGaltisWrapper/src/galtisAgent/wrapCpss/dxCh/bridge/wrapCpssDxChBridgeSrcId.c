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
* wrapBridgeSrcIdCpssDxCh.c
*
* DESCRIPTION:
*       Wrapper functions for BridgeSrcId cpss.dxCh functions
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
#include <cpss/generic/cpssTypes.h>
#include <cpss/dxCh/dxChxGen/bridge/cpssDxChBrgSrcId.h>

/* DxCh max number of entries to be read from tables\classes */
#define  CPSS_DXCH_SOURCE_ID_MAX_ENTRY_CNS 4096
/*******************************************************************************
* cpssDxChBrgSrcIdGroupPortAdd
*
* DESCRIPTION:
*         Add a port to Source ID group. Packets assigned with this Source ID
*         may be forwarded to this port.
*
* APPLICABLE DEVICES:  All DXCH devices
*
* INPUTS:
*       devNum   - device number
*       sourceId - Source ID  number that the port is added to.
*       portNum  - Physical port number.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK            - on success
*       GT_BAD_PARAM     - wrong devNum, portNum, sourceId
*       GT_HW_ERROR      - on hardware error
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChBrgSrcIdGroupPortAdd

(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS  result;

    GT_U8      devNum;
    GT_U32     sourceId;
    GT_PHYSICAL_PORT_NUM      portNum;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    sourceId = (GT_U32)inArgs[1];
    portNum = (GT_PHYSICAL_PORT_NUM)inArgs[2];

    /* Override Device and Port */
    CONVERT_DEV_PHYSICAL_PORT_MAC(devNum, portNum);

    /* call cpss api function */
    result = cpssDxChBrgSrcIdGroupPortAdd(devNum, sourceId, portNum);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}


/*******************************************************************************
* cpssDxChBrgSrcIdGroupPortDelete
*
* DESCRIPTION:
*         Delete a port from Source ID group. Packets assigned with this
*         Source ID will not be forwarded to this port.
*
* APPLICABLE DEVICES:  All DXCH devices
*
* INPUTS:
*       devNum         - device number
*       sourceId       - Source ID  number that the port is added to.
*       portNum        - Physical port number.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK            - on success
*       GT_BAD_PARAM     - wrong devNum, portNum, sourceId
*       GT_HW_ERROR      - on hardware error
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChBrgSrcIdGroupPortDelete

(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS  result;

    GT_U8      devNum;
    GT_U32     sourceId;
    GT_PHYSICAL_PORT_NUM      portNum;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    sourceId = (GT_U32)inArgs[1];
    portNum = (GT_PHYSICAL_PORT_NUM)inArgs[2];

    /* Override Device and Port */
    CONVERT_DEV_PHYSICAL_PORT_MAC(devNum, portNum);

    /* call cpss api function */
    result = cpssDxChBrgSrcIdGroupPortDelete(devNum, sourceId, portNum);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}




/*cpssDxChBrgSrcIdGroupEntry Table*/
static GT_U32   sourceIdCnt;

/* cpssDxChBrgSrcIdGroupEntrySet
*
* DESCRIPTION:
*        Set entry in Source ID Egress Filtering table.
*
* APPLICABLE DEVICES:  All DXCH devices
*
* INPUTS:
*       devNum         - device number
*       sourceId       - Source ID  number.
*       cpuSrcIdMember - GT_TRUE - CPU is member of of Src ID group.
*                        GT_FALSE - CPU isn't member of of Src ID group.
*       portBitmapPtr  - pointer to the bitmap of ports that are
*                        Source ID Members of specified PP device.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK            - on success
*       GT_BAD_PARAM     - wrong devNum, sourceId, ports bitmap value
*       GT_BAD_PTR       - portsMembersPtr is NULL pointer
*       GT_HW_ERROR      - on hardware error
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChBrgSrcIdGroupEntrySet

(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                        result;

    GT_U8                            devNum;
    GT_U32                           sourceId;
    GT_BOOL                          cpuSrcIdMember;
    CPSS_PORTS_BMP_STC               portsMembers;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];

    sourceId = (GT_U32)inFields[0];
    cpuSrcIdMember = (GT_BOOL)inFields[1];

    CPSS_PORTS_BMP_PORT_CLEAR_ALL_MAC(&portsMembers);
    portsMembers.ports[0] = (GT_U32)inFields[2];
    portsMembers.ports[1] = (GT_U32)inFields[3];
    CONVERT_DEV_PHYSICAL_PORTS_BMP_MAC(devNum,portsMembers);

    /* call cpss api function */
    result = cpssDxChBrgSrcIdGroupEntrySet(devNum, sourceId, cpuSrcIdMember,
                                                                &portsMembers);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}


/*******************************************************************************
* cpssDxChBrgSrcIdGroupEntryGet
*
* DESCRIPTION:
*        Get entry in Source ID Egress Filtering table.
*
* APPLICABLE DEVICES:  All DXCH devices
*
* INPUTS:
*       devNum            - device number
*       sourceId          - Source ID number.
*
* OUTPUTS:
*       cpuSrcIdMemberPtr - GT_TRUE - CPU is member of of Src ID group.
*                           GT_FALSE - CPU isn't member of of Src ID group.
*       portBitmapPtr     - pointer to the bitmap of ports are Source ID Members
*                           of specified PP device.
*
* RETURNS:
*       GT_OK             - on success
*       GT_BAD_PARAM      - wrong devNum, sourceId
*       GT_HW_ERROR       - on hardware error
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChBrgSrcIdGroupEntryGetFirst

(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                       result;

    GT_U8                            devNum;
    GT_BOOL                          cpuSrcIdMemberPtr;
    CPSS_PORTS_BMP_STC               portsMembers;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    sourceIdCnt = 0;

    /* call cpss api function */
    result = cpssDxChBrgSrcIdGroupEntryGet(devNum, sourceIdCnt,
                                            &cpuSrcIdMemberPtr,
                                              &portsMembers);

    if (result != GT_OK)
    {
        galtisOutput(outArgs, result, "%d", -1);
        return CMD_OK;
    }


    if(cpuSrcIdMemberPtr)
    {
        CONVERT_BACK_DEV_PHYSICAL_PORTS_BMP_MAC(devNum,portsMembers);
        inFields[0] = sourceIdCnt;
        inFields[1] = cpuSrcIdMemberPtr;
        inFields[2] = portsMembers.ports[0];
        inFields[3] = portsMembers.ports[1];

        /* pack and output table fields */
        fieldOutput("%d%d%d%d", inFields[0], inFields[1],
                                inFields[2], inFields[3]);

        galtisOutput(outArgs, GT_OK, "%f");
    }
    else
    {
       galtisOutput(outArgs, result, "");
    }
    return CMD_OK;
}

/******************************************************************************/
static CMD_STATUS wrCpssDxChBrgSrcIdGroupEntryGetNext
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                       result;

    GT_U8                            devNum;
    GT_BOOL                          cpuSrcIdMemberPtr;
    CPSS_PORTS_BMP_STC               portsMembers;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    sourceIdCnt++;

    if(sourceIdCnt >= CPSS_DXCH_SOURCE_ID_MAX_ENTRY_CNS)
    {
        galtisOutput(outArgs, GT_OK, "%d", -1);
        return CMD_OK;
    }

    /* call cpss api function */
    result = cpssDxChBrgSrcIdGroupEntryGet(devNum, sourceIdCnt,
                                            &cpuSrcIdMemberPtr,
                                              &portsMembers);

    if (result != GT_OK)
    {
        galtisOutput(outArgs, result, "%d", -1);
        return CMD_OK;
    }

    if(cpuSrcIdMemberPtr)
    {
        CONVERT_BACK_DEV_PHYSICAL_PORTS_BMP_MAC(devNum,portsMembers);
        inFields[0] = sourceIdCnt;
        inFields[1] = cpuSrcIdMemberPtr;
        inFields[2] = portsMembers.ports[0];
        inFields[3] = portsMembers.ports[1];

        /* pack and output table fields */
        fieldOutput("%d%d%d%d", inFields[0], inFields[1],
                                inFields[2], inFields[3]);

        galtisOutput(outArgs, GT_OK, "%f");
    }
    else
    {
       galtisOutput(outArgs, result, "");
    }
    return CMD_OK;
}

/*******************************************************************************
* cpssDxChBrgSrcIdPortDefaultSrcIdSet
*
* DESCRIPTION:
*       Configure Port's Default Source ID.
*       The Source ID is used for source based egress filtering.
*
* APPLICABLE DEVICES:  All DXCH devices
*
* INPUTS:
*       devNum          - device number
*       portNum         - Physical port number
*       defaultSrcId    - Port's Default Source ID
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK             - on success
*       GT_BAD_PARAM      - wrong devNum, portNum, defaultSrcId
*       GT_HW_ERROR       - on hardware error
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChBrgSrcIdPortDefaultSrcIdSet

(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS  result;

    GT_U8      devNum;
    GT_PORT_NUM      portNum;
    GT_U32     defaultSrceId;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    portNum = (GT_PORT_NUM)inArgs[1];
    defaultSrceId = (GT_U32)inArgs[2];

    /* Override Device and Port */
    CONVERT_DEV_PORT_U32_MAC(devNum, portNum);

    /* call cpss api function */
    result = cpssDxChBrgSrcIdPortDefaultSrcIdSet(devNum, portNum,
                                                   defaultSrceId);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}


/*******************************************************************************
* cpssDxChBrgSrcIdPortDefaultSrcIdGet
*
* DESCRIPTION:
*       Get configuration of Port's Default Source ID.
*       The Source ID is used for Source based egress filtering.
*
* APPLICABLE DEVICES:  All DXCH devices
*
* INPUTS:
*       devNum            - device number
*       portNum           - Physical port number
*
* OUTPUTS:
*       defaultSrcIdPtr   - Port's Default Source ID
*
* RETURNS:
*       GT_OK             - on success
*       GT_BAD_PARAM      - wrong devNum, portNum
*       GT_HW_ERROR       - on hardware error
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChBrgSrcIdPortDefaultSrcIdGet

(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS  result;

    GT_U8      devNum;
    GT_PORT_NUM      portNum;
    GT_U32     defaultSrceIdPtr;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    portNum = (GT_PORT_NUM)inArgs[1];

    /* Override Device and Port */
    CONVERT_DEV_PORT_U32_MAC(devNum, portNum);

    /* call cpss api function */
    result = cpssDxChBrgSrcIdPortDefaultSrcIdGet(devNum, portNum,
                                               &defaultSrceIdPtr);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", defaultSrceIdPtr);
    return CMD_OK;
}

/*******************************************************************************
* cpssDxChBrgSrcIdGlobalUcastEgressFilterSet
*
* DESCRIPTION:
*     Enable or disable Source ID egress filter for unicast
*     packets. The Source ID filter is configured by
*     cpssDxChBrgSrcIdGroupPortAdd and cpssDxChBrgSrcIdGroupPortDelete.
*
* APPLICABLE DEVICES:  All DXCH devices
*
* INPUTS:
*       devNum   - device number
*       enable   - GT_TRUE -  enable Source ID filtering on unicast packets.
*                             Unicast packet is dropped if egress port is not
*                             member in the Source ID group.
*                - GT_FALSE - disable Source ID filtering on unicast packets.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK            - on success
*       GT_BAD_PARAM     - wrong devNum
*       GT_HW_ERROR      - on hardware error
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChBrgSrcIdGlobalUcastEgressFilterSet

(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS  result;

    GT_U8      devNum;
    GT_BOOL    enable;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    enable = (GT_BOOL)inArgs[1];

    /* call cpss api function */
    result = cpssDxChBrgSrcIdGlobalUcastEgressFilterSet(devNum, enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}


/*******************************************************************************
* cpssDxChBrgSrcIdGlobalUcastEgressFilterGet
*
* DESCRIPTION:
*     Get Source ID egress filter configuration for unicast
*     packets. The Source ID filter is configured by
*     cpssDxChBrgSrcIdGroupPortAdd and cpssDxChBrgSrcIdGroupPortDelete.
*
* APPLICABLE DEVICES:  All DXCH devices
*
* INPUTS:
*       devNum   - device number
*
* OUTPUTS:
*       enablePtr   - GT_TRUE -  enable Source ID filtering on unicast packets.
*                             Unicast packet is dropped if egress port is not
*                             member in the Source ID group.
*                   - GT_FALSE - disable Source ID filtering on unicast packets.
*
* RETURNS:
*       GT_OK            - on success
*       GT_BAD_PARAM     - wrong devNum
*       GT_HW_ERROR      - on hardware error
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChBrgSrcIdGlobalUcastEgressFilterGet

(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS  result;

    GT_U8      devNum;
    GT_BOOL    enablePtr;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];

    /* call cpss api function */
    result = cpssDxChBrgSrcIdGlobalUcastEgressFilterGet(devNum, &enablePtr);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", enablePtr);
    return CMD_OK;
}


/*******************************************************************************
* cpssDxChBrgSrcIdGlobalSrcIdAssignModeSet
*
* DESCRIPTION:
*         Set Source ID Assignment mode for non-DSA tagged packets.
*
* APPLICABLE DEVICES:  All DXCH devices exclude DXCH2
*
* INPUTS:
*       devNum    - device number
*       mode      - the assignment mode of the packet Source ID
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK             - on success
*       GT_BAD_PARAM      - wrong devNum, mode
*       GT_NOT_SUPPORTED  - this request isn't supported
*       GT_HW_ERROR       - on hardware error
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChBrgSrcIdGlobalSrcIdAssignModeSet

(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                          result;

    GT_U8                              devNum;
    CPSS_BRG_SRC_ID_ASSIGN_MODE_ENT    mode;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    mode = (CPSS_BRG_SRC_ID_ASSIGN_MODE_ENT)inArgs[1];

    /* call cpss api function */
    result = cpssDxChBrgSrcIdGlobalSrcIdAssignModeSet(devNum, mode);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}


/*******************************************************************************
* cpssDxChBrgSrcIdGlobalSrcIdAssignModeGet
*
* DESCRIPTION:
*         Get Source ID Assignment mode for non-DSA tagged packets.
*
* APPLICABLE DEVICES:  All DXCH devices exclude DXCH2
*
* INPUTS:
*       devNum    - device number
*
* OUTPUTS:
*       modePtr      - the assignment mode of the packet Source ID
*
* RETURNS:
*       GT_OK             - on success
*       GT_BAD_PARAM      - wrong devNum
*       GT_NOT_SUPPORTED  - this request isn't supported
*       GT_HW_ERROR       - on hardware error
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChBrgSrcIdGlobalSrcIdAssignModeGet

(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                          result;

    GT_U8                              devNum;
    CPSS_BRG_SRC_ID_ASSIGN_MODE_ENT    modePtr;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];

    /* call cpss api function */
    result = cpssDxChBrgSrcIdGlobalSrcIdAssignModeGet(devNum, &modePtr);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", modePtr);
    return CMD_OK;
}


/*******************************************************************************
* cpssDxChBrgSrcIdPortSrcIdAssignModeSet
*
* DESCRIPTION:
*         Set Source ID Assignment mode for non-DSA tagged packets
*         per Ingress Port.
*
* APPLICABLE DEVICES:  All DXCH2 devices
*
* INPUTS:
*       devNum    - device number
*       portNum   - Physical port number
*       mode      - the assignment mode of the packet Source ID
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK             - on success
*       GT_BAD_PARAM      - wrong devNum, portNum, mode
*       GT_HW_ERROR       - on hardware error
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChBrgSrcIdPortSrcIdAssignModeSet

(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                          result;

    GT_U8                              devNum;
    GT_PORT_NUM                              portNum;
    CPSS_BRG_SRC_ID_ASSIGN_MODE_ENT    mode;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    portNum = (GT_PORT_NUM)inArgs[1];
    mode = (CPSS_BRG_SRC_ID_ASSIGN_MODE_ENT)inArgs[2];

    /* Override Device and Port */
    CONVERT_DEV_PORT_U32_MAC(devNum, portNum);

    /* call cpss api function */
    result = cpssDxChBrgSrcIdPortSrcIdAssignModeSet(devNum, portNum, mode);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}


/*******************************************************************************
* cpssDxChBrgSrcIdPortSrcIdAssignModeGet
*
* DESCRIPTION:
*         Get Source ID Assignment mode for non-DSA tagged packets
*         per Ingress Port.
*
* APPLICABLE DEVICES:  All DXCH2 devices
*
* INPUTS:
*       devNum    - device number
*       portNum   - Physical port number
*
* OUTPUTS:
*       modePtr   - the assignment mode of the packet Source ID
*
* RETURNS:
*       GT_OK             - on success
*       GT_BAD_PARAM      - wrong devNum, portNum
*       GT_HW_ERROR       - on hardware error
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChBrgSrcIdPortSrcIdAssignModeGet

(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                          result;

    GT_U8                              devNum;
    GT_PORT_NUM                              portNum;
    CPSS_BRG_SRC_ID_ASSIGN_MODE_ENT    modePtr;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    portNum = (GT_PORT_NUM)inArgs[1];

    /* Override Device and Port */
    CONVERT_DEV_PORT_U32_MAC(devNum, portNum);

    /* call cpss api function */
    result = cpssDxChBrgSrcIdPortSrcIdAssignModeGet(devNum, portNum, &modePtr);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", modePtr);
    return CMD_OK;
}

/*******************************************************************************
* cpssDxChBrgSrcIdPortUcastEgressFilterSet
*
* DESCRIPTION:
*     Per Egress Port enable or disable Source ID egress filter for unicast
*     packets. The Source ID filter is configured by
*     cpssDxChBrgSrcIdGroupPortAdd and cpssDxChBrgSrcIdGroupPortDelete.
*
* APPLICABLE DEVICES:  All DXCH2 devices
*
* INPUTS:
*       devNum   - device number
*       portNum  - Physical port number/all ports wild card
*       enable   - GT_TRUE - enable Source ID filtering on unicast packets
*                            forwarded to this port. Unicast packet is dropped
*                            if egress port is not member in the Source ID group.
*                - GT_FALSE - disable Source ID filtering on unicast packets
*                             forwarded to this port.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK            - on success
*       GT_BAD_PARAM     - wrong devNum, portNum
*       GT_HW_ERROR      - on hardware error
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChBrgSrcIdPortUcastEgressFilterSet

(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                          result;

    GT_U8                              devNum;
    GT_PHYSICAL_PORT_NUM                              portNum;
    GT_BOOL                            enable;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    portNum = (GT_PHYSICAL_PORT_NUM)inArgs[1];
    enable = (GT_BOOL)inArgs[2];

    /* Override Device and Port */
    CONVERT_DEV_PHYSICAL_PORT_MAC(devNum, portNum);

    /* call cpss api function */
    result = cpssDxChBrgSrcIdPortUcastEgressFilterSet(devNum, portNum, enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}


/*******************************************************************************
* cpssDxChBrgSrcIdPortUcastEgressFilterGet
*
* DESCRIPTION:
*     Get Per Egress Port Source ID egress filter configuration for unicast
*     packets. The Source ID filter is configured by
*     cpssDxChBrgSrcIdGroupPortAdd and cpssDxChBrgSrcIdGroupPortDelete.
*
* APPLICABLE DEVICES:  All DXCH2 devices
*
* INPUTS:
*       devNum   - device number
*       portNum  - Physical port number/all ports wild card
*
* OUTPUTS:
*       enablePtr   - GT_TRUE - enable Source ID filtering on unicast packets
*                            forwarded to this port. Unicast packet is dropped
*                            if egress port is not member in the Source ID group.
*                   - GT_FALSE - disable Source ID filtering on unicast packets
*                             forwarded to this port.
*
* RETURNS:
*       GT_OK            - on success
*       GT_BAD_PARAM     - wrong devNum, portNum
*       GT_HW_ERROR      - on hardware error
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChBrgSrcIdPortUcastEgressFilterGet

(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                          result;

    GT_U8                              devNum;
    GT_PHYSICAL_PORT_NUM                              portNum;
    GT_BOOL                            enablePtr;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    portNum = (GT_PHYSICAL_PORT_NUM)inArgs[1];

    /* Override Device and Port */
    CONVERT_DEV_PHYSICAL_PORT_MAC(devNum, portNum);

    /* call cpss api function */
    result = cpssDxChBrgSrcIdPortUcastEgressFilterGet(devNum, portNum,
                                                           &enablePtr);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", enablePtr);
    return CMD_OK;
}

/*******************************************************************************
* cpssDxChBrgSrcIdPortSrcIdForceEnableSet
*
* DESCRIPTION:
*         Set Source ID Assignment force mode per Ingress Port.
*
* APPLICABLE DEVICES:  DxChXcat and above.
*
* INPUTS:
*       devNum    - device number
*       portNum   - Physical port number, CPU port
*       enable    - enable/disable SourceID force mode
*                     GT_TRUE - Src-ID is taken from port default
*                     GT_FALSE - Src-ID is taken not from port default
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong devNum, portNum
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChBrgSrcIdPortSrcIdForceEnableSet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS   result;
    GT_U8       devNum;
    GT_PORT_NUM       portNum;
    GT_BOOL     enable;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    portNum = (GT_PORT_NUM)inArgs[1];
    enable = (GT_BOOL)inArgs[2];

    /* Override Device and Port */
    CONVERT_DEV_PORT_U32_MAC(devNum, portNum);

    /* call cpss api function */
    result = cpssDxChBrgSrcIdPortSrcIdForceEnableSet(devNum, portNum, enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}

/*******************************************************************************
* cpssDxChBrgSrcIdPortSrcIdForceEnableGet
*
* DESCRIPTION:
*         Get Source ID Assignment force mode per Ingress Port.
*
* APPLICABLE DEVICES:  DxChXcat and above.
*
* INPUTS:
*       devNum    - device number
*       portNum   - port number, CPU port
*
* OUTPUTS:
*       enablePtr - (pointer to) Source ID Assignment force mode state
*                     GT_TRUE - Src-ID is taken from port default
*                     GT_FALSE - Src-ID is taken not from port default
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong devNum, portNum
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - on NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChBrgSrcIdPortSrcIdForceEnableGet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS   result;
    GT_U8       devNum;
    GT_PORT_NUM       portNum;
    GT_BOOL     enable;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    portNum = (GT_PORT_NUM)inArgs[1];

    /* Override Device and Port */
    CONVERT_DEV_PORT_U32_MAC(devNum, portNum);

    /* call cpss api function */
    result = cpssDxChBrgSrcIdPortSrcIdForceEnableGet(devNum, portNum, &enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", enable);
    return CMD_OK;
}



/**** database initialization **************************************/

static CMD_COMMAND dbCommands[] =
{

    {"cpssDxChBrgSrcIdGroupPortAdd",
        &wrCpssDxChBrgSrcIdGroupPortAdd,
        3, 0},

    {"cpssDxChBrgSrcIdGroupPortDelete",
        &wrCpssDxChBrgSrcIdGroupPortDelete,
        3, 0},

    {"cpssDxChBrgSrcIdGroupEntrySet",
        &wrCpssDxChBrgSrcIdGroupEntrySet,
        1, 4},

    {"cpssDxChBrgSrcIdGroupEntryGetFirst",
        &wrCpssDxChBrgSrcIdGroupEntryGetFirst,
        1, 0},

    {"cpssDxChBrgSrcIdGroupEntryGetNext",
        &wrCpssDxChBrgSrcIdGroupEntryGetNext,
        1, 0},

    {"cpssDxChBrgSrcIdPortDefaultSrcIdSet",
        &wrCpssDxChBrgSrcIdPortDefaultSrcIdSet,
        3, 0},

    {"cpssDxChBrgSrcIdPortDefaultSrcIdGet",
        &wrCpssDxChBrgSrcIdPortDefaultSrcIdGet,
        2, 0},

    {"cpssDxChBrgSrcIdGlobalUcastEgressFilterSet",
        &wrCpssDxChBrgSrcIdGlobalUcastEgressFilterSet,
        2, 0},

    {"cpssDxChBrgSrcIdGlobalUcastEgressFilterGet",
        &wrCpssDxChBrgSrcIdGlobalUcastEgressFilterGet,
        1, 0},

    {"cpssDxChBrgSrcIdGlobalSrcIdAssignModeSet",
        &wrCpssDxChBrgSrcIdGlobalSrcIdAssignModeSet,
        2, 0},

    {"cpssDxChBrgSrcIdGlobalSrcIdAssignModeGet",
        &wrCpssDxChBrgSrcIdGlobalSrcIdAssignModeGet,
        1, 0},

    {"cpssDxChBrgSrcIdGlobalSrcIdAssignModeSet1",
        &wrCpssDxChBrgSrcIdGlobalSrcIdAssignModeSet,
        2, 0},

    {"cpssDxChBrgSrcIdGlobalSrcIdAssignModeGet1",
        &wrCpssDxChBrgSrcIdGlobalSrcIdAssignModeGet,
        1, 0},

    {"cpssDxChBrgSrcIdPortSrcIdAssignModeSet",
        &wrCpssDxChBrgSrcIdPortSrcIdAssignModeSet,
        3, 0},

    {"cpssDxChBrgSrcIdPortSrcIdAssignModeGet",
        &wrCpssDxChBrgSrcIdPortSrcIdAssignModeGet,
        2, 0},

    {"cpssDxChBrgSrcIdPortUcastEgressFilterSet",
        &wrCpssDxChBrgSrcIdPortUcastEgressFilterSet,
        3, 0},

    {"cpssDxChBrgSrcIdPortUcastEgressFilterGet",
        &wrCpssDxChBrgSrcIdPortUcastEgressFilterGet,
        2, 0},

    {"cpssDxChBrgSrcIdPortSrcIdForceEnableSet",
        &wrCpssDxChBrgSrcIdPortSrcIdForceEnableSet,
        3, 0},

    {"cpssDxChBrgSrcIdPortSrcIdForceEnableGet",
        &wrCpssDxChBrgSrcIdPortSrcIdForceEnableGet,
        2, 0}

};

#define numCommands (sizeof(dbCommands) / sizeof(CMD_COMMAND))

/*******************************************************************************
* cmdLibInitCpssDxChBridgeSrcId
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
GT_STATUS cmdLibInitCpssDxChBridgeSrcId
(
    GT_VOID
)
{
    return cmdInitLibrary(dbCommands, numCommands);
}



