/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* wrapCpssDxChBridgeFdbRouting.c
*
* DESCRIPTION:
*       Wrapper functions for cpssDxChBrgFdbRouting.c dxCh functions
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
#include <cpss/dxCh/dxChxGen/bridge/cpssDxChBrgFdbRouting.h>

extern GT_STATUS appDemoBrgFdbIpv6UcUpdateAddrDataLinkedList
(
    IN  GT_U8           devNum,
    IN  GT_U32          addrIndex,
    IN  GT_U32          dataIndex,
    IN  GT_BOOL         deleteElement
);

extern GT_VOID appDemoBrgFdbIpv6AddrDataLinkedListDump
(
    GT_VOID
);

extern GT_STATUS appDemoBrgFdbIpv6UcFreeAddrDataLinkedList
(
    IN GT_U8  devNum
);

/*******************************************************************************
* cpssDxChBrgFdbRoutingPortIpUcEnableSet
*
* DESCRIPTION:
*   Set Enable/Disable using of FDB for Destination IP lookup for IP UC routing.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum     - device number
*       portNum    - port number
*       protocol    - ipv4 or ipv6 or both
*       enable     - GT_TRUE  - enable using of FDB for DIP lookup for IP UC routing,
*                    GT_FALSE - disable using of FDB for DIP lookup for IP UC routing
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success,
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong parameter value
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChBrgFdbRoutingPortIpUcEnableSet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                    result;
    GT_U8                        dev;
    GT_PORT_NUM                  port;
    CPSS_IP_PROTOCOL_STACK_ENT   protocol;
    GT_BOOL                      enable;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    dev      = (GT_U8)inArgs[0];
    port     = (GT_PORT_NUM)inArgs[1];
    protocol = (CPSS_IP_PROTOCOL_STACK_ENT)inArgs[2];
    enable   = (GT_BOOL)inArgs[3];

    /* Override Device and Port */
    CONVERT_DEV_PORT_U32_MAC(dev, port);

    /* call cpss api function */
    result = cpssDxChBrgFdbRoutingPortIpUcEnableSet(dev, port, protocol, enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}

/*******************************************************************************
* cpssDxChBrgFdbRoutingPortIpUcEnableGet
*
* DESCRIPTION:
*   Get Enable/Disable using of FDB for Destination IP lookup for IP UC routing.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum     - device number
*       portNum    - port number
*       protocol    - ipv4 or ipv6
*
* OUTPUTS:
*       enablePtr  - (pointer to)
*                    GT_TRUE  - enable using of FDB for DIP lookup for IP UC routing,
*                    GT_FALSE - disable using of FDB for DIP lookup for IP UC routing
*
* RETURNS:
*       GT_OK                    - on success,
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong parameter value
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChBrgFdbRoutingPortIpUcEnableGet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                    result;
    GT_U8                        dev;
    GT_PORT_NUM                  port;
    CPSS_IP_PROTOCOL_STACK_ENT   protocol;
    GT_BOOL                      enable;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    dev      = (GT_U8)inArgs[0];
    port     = (GT_PORT_NUM)inArgs[1];
    protocol = (CPSS_IP_PROTOCOL_STACK_ENT)inArgs[2];

    /* Override Device and Port */
    CONVERT_DEV_PORT_U32_MAC(dev, port);

    /* call cpss api function */
    result = cpssDxChBrgFdbRoutingPortIpUcEnableGet(dev, port, protocol, &enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", enable);
    return CMD_OK;
}

/*******************************************************************************
* cpssDxChBrgFdbRoutingUcRefreshEnableSet
*
* DESCRIPTION:
*   Enable/Disable refresh for UC Routing entries
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum     - device number
*       enable     - GT_TRUE - Perform refresh for UC Routing entries
*                    GT_FALSE - Do not perform refresh for UC Routing entries
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success,
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong parameter value
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChBrgFdbRoutingUcRefreshEnableSet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8     devNum;
    GT_BOOL   enable;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    enable = (GT_BOOL)inArgs[1];

    /* call cpss api function */
    result = cpssDxChBrgFdbRoutingUcRefreshEnableSet(devNum, enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChBrgFdbRoutingUcRefreshEnableGet
*
* DESCRIPTION:
*   Get Enable/Disable refresh status for UC Routing entries
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum     - device number
*
* OUTPUTS:
*       enablePtr  - (pointer to)
*                    GT_TRUE  - Perform refresh for UC Routing entries
*                    GT_FALSE - Do not perform refresh for UC Routing entries
*
* RETURNS:
*       GT_OK                    - on success,
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong parameter value
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChBrgFdbRoutingUcRefreshEnableGet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8     devNum;
    GT_BOOL   enable;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];

    /* call cpss api function */
    result = cpssDxChBrgFdbRoutingUcRefreshEnableGet(devNum, &enable);
    if(result != GT_OK)
    {
        galtisOutput(outArgs, result, "%d", -1);
        return CMD_OK;
    }

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", enable);

    return CMD_OK;
}


/*******************************************************************************
* cpssDxChBrgFdbRoutingUcAgingEnableSet
*
* DESCRIPTION:
*   Enable/Disable aging for UC Routing entries
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum     - device number
*       enable     - GT_TRUE  - UC routing entries are examined for aging and
*                               an AA message is sent out for an entry that is
*                               aged out. Auto delete is not performed
*                    GT_FALSE - UC routing entries are not examined for aging
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success,
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong parameter value
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChBrgFdbRoutingUcAgingEnableSet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8     devNum;
    GT_BOOL   enable;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    enable = (GT_BOOL)inArgs[1];

    /* call cpss api function */
    result = cpssDxChBrgFdbRoutingUcAgingEnableSet(devNum, enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChBrgFdbRoutingUcAgingEnableGet
*
* DESCRIPTION:
*   Get Enable/Disable aging status for UC Routing entries
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum     - device number
*
* OUTPUTS:
*       enablePtr  - (pointer to)
*                    GT_TRUE  - UC routing entries are examined for aging and
*                               an AA message is sent out for an entry that is
*                               aged out. Auto delete is not performed
*                    GT_FALSE - UC routing entries are not examined for aging
*
* RETURNS:
*       GT_OK                    - on success,
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong parameter value
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChBrgFdbRoutingUcAgingEnableGet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8     devNum;
    GT_BOOL   enable;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];

    /* call cpss api function */
    result = cpssDxChBrgFdbRoutingUcAgingEnableGet(devNum, &enable);
    if(result != GT_OK)
    {
        galtisOutput(outArgs, result, "%d", -1);
        return CMD_OK;
    }

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", enable);

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChBrgFdbRoutingUcTransplantEnableSet
*
* DESCRIPTION:
*   Enable/Disable transplanting for UC Routing entries
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum     - device number
*       enable     - GT_TRUE  - Perform address transplanting for UC Routing entries
*                    GT_FALSE - Do not perform address transplanting for UC Routing entries
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success,
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong parameter value
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChBrgFdbRoutingUcTransplantEnableSet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8     devNum;
    GT_BOOL   enable;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    enable = (GT_BOOL)inArgs[1];

    /* call cpss api function */
    result = cpssDxChBrgFdbRoutingUcTransplantEnableSet(devNum, enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChBrgFdbRoutingUcTransplantEnableGet
*
* DESCRIPTION:
*   Get Enable/Disable transplanting status for UC Routing entries
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum     - device number
*
* OUTPUTS:
*       enablePtr  - (pointer to)
*                    GT_TRUE  - Perform address transplanting for UC Routing entries
*                    GT_FALSE - Do not perform address transplanting for UC Routing entries
*
* RETURNS:
*       GT_OK                    - on success,
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong parameter value
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChBrgFdbRoutingUcTransplantEnableGet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8     devNum;
    GT_BOOL   enable;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];

    /* call cpss api function */
    result = cpssDxChBrgFdbRoutingUcTransplantEnableGet(devNum, &enable);
    if(result != GT_OK)
    {
        galtisOutput(outArgs, result, "%d", -1);
        return CMD_OK;
    }

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", enable);

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChBrgFdbRoutingUcDeleteEnableSet
*
* DESCRIPTION:
*   Enable/Disable delete for UC Routing entries
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum     - device number
*       enable     - GT_TRUE  - Perform address deleting for UC Routing entries
*                    GT_FALSE - Do not perform address deleting for UC Routing entries
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success,
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong parameter value
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChBrgFdbRoutingUcDeleteEnableSet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8     devNum;
    GT_BOOL   enable;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    enable = (GT_BOOL)inArgs[1];

    /* call cpss api function */
    result = cpssDxChBrgFdbRoutingUcDeleteEnableSet(devNum, enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChBrgFdbRoutingUcDeleteEnableGet
*
* DESCRIPTION:
*   Get Enable/Disable delete status for UC Routing entries
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum     - device number
*
* OUTPUTS:
*       enablePtr  - (pointer to)
*                    GT_TRUE  - Perform address deleting for UC Routing entries
*                    GT_FALSE - Do not perform address deleting for UC Routing entries
*
* RETURNS:
*       GT_OK                    - on success,
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong parameter value
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChBrgFdbRoutingUcDeleteEnableGet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8     devNum;
    GT_BOOL   enable;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];

    /* call cpss api function */
    result = cpssDxChBrgFdbRoutingUcDeleteEnableGet(devNum, &enable);
    if(result != GT_OK)
    {
        galtisOutput(outArgs, result, "%d", -1);
        return CMD_OK;
    }

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", enable);

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChBrgFdbRoutingUcAAandTAToCpuSet
*
* DESCRIPTION:
*       The device implements a hardware-based aging or transplanting
*       mechanism. When an entry is aged out or is transplanted, a message can
*       be forwarded to the CPU.
*       This field enables/disables forwarding address aged out messages and
*       transplanted address messages to the Host CPU for UC route FDB entries.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum     - device number
*       enable     - GT_TRUE  - AA and TA messages are not forwarded to the CPU for
*                               UC route entries.
*                    GT_FALSE - AA and TA messages are not forwarded to the CPU for
*                               UC route entries.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success,
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong parameter value
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChBrgFdbRoutingUcAAandTAToCpuSet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8     devNum;
    GT_BOOL   enable;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    enable = (GT_BOOL)inArgs[1];

    /* call cpss api function */
    result = cpssDxChBrgFdbRoutingUcAAandTAToCpuSet(devNum, enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChBrgFdbRoutingUcAAandTAToCpuGet
*
* DESCRIPTION:
*   The device implements a hardware-based aging or transplanting mechanism
*   When an entry is aged out or is transplanted, a message can be forwarded
*   to the CPU.
*   This field  get the enables/disable status of forwarding address aged out
*   messages and transplanted address messages to the Host CPU for UC route FDB entries.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum     - device number
*
* OUTPUTS:
*       enablePtr  - (pointer to)
*                    GT_TRUE  - AA and TA messages are not forwarded to the CPU for
*                               UC route entries.
*                    GT_FALSE - AA and TA messages are not forwarded to the CPU for
*
* RETURNS:
*       GT_OK                    - on success,
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong parameter value
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChBrgFdbRoutingUcAAandTAToCpuGet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8     devNum;
    GT_BOOL   enable;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];

    /* call cpss api function */
    result = cpssDxChBrgFdbRoutingUcAAandTAToCpuGet(devNum, &enable);
    if(result != GT_OK)
    {
        galtisOutput(outArgs, result, "%d", -1);
        return CMD_OK;
    }

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", enable);

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChBrgFdbRoutingNextHopPacketCmdSet
*
* DESCRIPTION:
*       Set Packet Commands for FDB routed packets.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum      - device number
*       nhPacketCmd - Route entry command. supported commands:
*                     CPSS_PACKET_CMD_ROUTE_E,
*                     CPSS_PACKET_CMD_ROUTE_AND_MIRROR_E,
*                     CPSS_PACKET_CMD_TRAP_TO_CPU_E,
*                     CPSS_PACKET_CMD_DROP_SOFT_E ,
*                     CPSS_PACKET_CMD_DROP_HARD_E.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success,
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong parameter value
*       GT_OUT_OF_RANGE          - one of the parameters is out of range
*       GT_BAD_STATE             - the setting not allowed in current state of
*                                  configuration
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChBrgFdbRoutingNextHopPacketCmdSet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS               result;
    GT_U8                   devNum;
    CPSS_PACKET_CMD_ENT     nhPacketCmd;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    nhPacketCmd = (CPSS_PACKET_CMD_ENT)inArgs[1];

    /* call cpss api function */
    result = cpssDxChBrgFdbRoutingNextHopPacketCmdSet(devNum, nhPacketCmd);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}


/*******************************************************************************
* cpssDxChBrgFdbRoutingNextHopPacketCmdGet
*
* DESCRIPTION:
*    Get Packet Commands for FDB routed packets.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum     - device number
*
* OUTPUTS:
*       nhPacketCmdPtr - (pointer to) Route entry command. supported commands:
*                       CPSS_PACKET_CMD_ROUTE_E,
*                       CPSS_PACKET_CMD_ROUTE_AND_MIRROR_E,
*                       CPSS_PACKET_CMD_TRAP_TO_CPU_E,
*                       CPSS_PACKET_CMD_DROP_SOFT_E ,
*                       CPSS_PACKET_CMD_DROP_HARD_E.
* RETURNS:
*       GT_OK                    - on success,
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_VALUE             - wrong hw value
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChBrgFdbRoutingNextHopPacketCmdGet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS               result;
    GT_U8                   devNum;
    CPSS_PACKET_CMD_ENT     nhPacketCmd;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];

    /* call cpss api function */
    result = cpssDxChBrgFdbRoutingNextHopPacketCmdGet(devNum, &nhPacketCmd);
    if(result != GT_OK)
    {
        galtisOutput(outArgs, result, "%d", -1);
        return CMD_OK;
    }

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", nhPacketCmd);

    return CMD_OK;
}

/*******************************************************************************
* wrAppDemoBrgFdbIpv6AddrDataLinkedListDump
*
* DESCRIPTION:
*       Dump FDB IPv6 UC Route entries linked list,
*       that holds AddressIndex to DataIndex mapping
*
* INPUTS:
*       devNum     - the device number from which AU are taken
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK   - on success
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrAppDemoBrgFdbIpv6AddrDataLinkedListDump
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS               result=GT_OK;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

   /* call cpss api function */
    appDemoBrgFdbIpv6AddrDataLinkedListDump();

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* wrAppDemoBrgFdbIpv6UcFreeAddrDataLinkedList
*
* DESCRIPTION:
*       Free FDB IPv6 UC Route entries linked list,
*       that holds AddressIndex to DataIndex mapping
*
* INPUTS:
*       devNum     - the device number from which AU are taken
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong devNum
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_OUT_OF_RANGE          - index is out of range
*       GT_TIMEOUT  - after max number of retries checking if PP ready
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       Only elements that were deleted in flush process will be deleted
*       from the Linked List. If only part of the IPV6 UC entry was deleted
*       (address entry or data entry) then the Linked list will be updated
*       with value PRV_APPDEMO_INVALID_FDB_INDEX_VALUE_CNS indicating the
*       entry is not valid
*
*******************************************************************************/
static CMD_STATUS wrAppDemoBrgFdbIpv6UcFreeAddrDataLinkedList
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS               result;
    GT_U8                   devNum;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];

    /* call cpss api function */
    result = appDemoBrgFdbIpv6UcFreeAddrDataLinkedList(devNum);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}
/*******************************************************************************
* wrAppDemoBrgFdbIpv6UcUpdateAddrDataLinkedList
*
* DESCRIPTION:
*       Update FDB IPv6 UC Route entries linked list,
*       that holds AddressIndex to DataIndex mapping
*
* INPUTS:
*       devNum    - the device number
*       addrIndex - FDB IPv6 UC address index bind to dataIndex
*       dataIndex - FDB IPv6 UC data index bind to addrIndex
*       deleteElement - GT_TRUE: delete the element if found
*                       GT_FALSE: update the element if found or add a new one
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK               - on success,
*       GT_OUT_OF_CPU_MEM   - on allocation fail
*       GT_FAIL             - otherwise.
*
* COMMENTS:
*       adding a mapping of addrIndex and dataIndex to the Linked List must be
*       of valid entries that were added to HW.
*       if the address entry or the data entry is not added to HW this function will fail.
*       the Linked List should reflect the HW state
*
*******************************************************************************/
static CMD_STATUS wrAppDemoBrgFdbIpv6UcUpdateAddrDataLinkedList
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS               result;
    GT_U8                   devNum;
    GT_U32                  addrIndex;
    GT_U32                  dataIndex;
    GT_BOOL                 deleteElement;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    addrIndex = (GT_U32)inArgs[1];
    dataIndex = (GT_U32)inArgs[2];
    deleteElement = (GT_BOOL)inArgs[3];

    /* call cpss api function */
    result = appDemoBrgFdbIpv6UcUpdateAddrDataLinkedList(devNum,addrIndex, dataIndex, deleteElement);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/**** database initialization **************************************/

static CMD_COMMAND dbCommands[] =
{
    {"cpssDxChBrgFdbRoutingPortIpUcEnableSet",
        &wrCpssDxChBrgFdbRoutingPortIpUcEnableSet,
        4, 0},

    {"cpssDxChBrgFdbRoutingPortIpUcEnableGet",
        &wrCpssDxChBrgFdbRoutingPortIpUcEnableGet,
        3, 0},

    {"cpssDxChBrgFdbRoutingUcRefreshEnableSet",
        &wrCpssDxChBrgFdbRoutingUcRefreshEnableSet,
        2, 0},

    {"cpssDxChBrgFdbRoutingUcRefreshEnableGet",
        &wrCpssDxChBrgFdbRoutingUcRefreshEnableGet,
        1, 0},

    {"cpssDxChBrgFdbRoutingUcAgingEnableSet",
        &wrCpssDxChBrgFdbRoutingUcAgingEnableSet,
        2, 0},

    {"cpssDxChBrgFdbRoutingUcAgingEnableGet",
        &wrCpssDxChBrgFdbRoutingUcAgingEnableGet,
        1, 0},

    {"cpssDxChBrgFdbRoutingUcTransplantEnableSet",
        &wrCpssDxChBrgFdbRoutingUcTransplantEnableSet,
        2, 0},

    {"cpssDxChBrgFdbRoutingUcTransplantEnableGet",
        &wrCpssDxChBrgFdbRoutingUcTransplantEnableGet,
        1, 0},

    {"cpssDxChBrgFdbRoutingUcDeleteEnableSet",
        &wrCpssDxChBrgFdbRoutingUcDeleteEnableSet,
        2, 0},

    {"cpssDxChBrgFdbRoutingUcDeleteEnableGet",
        &wrCpssDxChBrgFdbRoutingUcDeleteEnableGet,
        1, 0},

    {"cpssDxChBrgFdbRoutingUcAAandTAToCpuSet",
        &wrCpssDxChBrgFdbRoutingUcAAandTAToCpuSet,
        2, 0},

    {"cpssDxChBrgFdbRoutingUcAAandTAToCpuGet",
        &wrCpssDxChBrgFdbRoutingUcAAandTAToCpuGet,
        1, 0},

    {"cpssDxChBrgFdbRoutingNextHopPacketCmdSet",
        &wrCpssDxChBrgFdbRoutingNextHopPacketCmdSet,
        2, 0},

    {"cpssDxChBrgFdbRoutingNextHopPacketCmdGet",
        &wrCpssDxChBrgFdbRoutingNextHopPacketCmdGet,
        1, 0},

    {"appDemoDxChBrgFdbRouteIpv6UcAddrDataIndexDBDump",
        &wrAppDemoBrgFdbIpv6AddrDataLinkedListDump,
        0, 0},

     {"appDemoDxChBrgFdbRouteIpv6UcFreeAddrDataLinkedList",
        &wrAppDemoBrgFdbIpv6UcFreeAddrDataLinkedList,
        1, 0},

      {"appDemoDxChBrgFdbRouteIpv6UcUpdateAddrDataIndexDB",
        &wrAppDemoBrgFdbIpv6UcUpdateAddrDataLinkedList,
        4, 0}
};

#define numCommands (sizeof(dbCommands) / sizeof(CMD_COMMAND))


/*******************************************************************************
* cmdLibInitCpssDxChBridgeFdbRouting
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
GT_STATUS cmdLibInitCpssDxChBridgeFdbRouting
(
    GT_VOID
)
{
    return cmdInitLibrary(dbCommands, numCommands);
}



