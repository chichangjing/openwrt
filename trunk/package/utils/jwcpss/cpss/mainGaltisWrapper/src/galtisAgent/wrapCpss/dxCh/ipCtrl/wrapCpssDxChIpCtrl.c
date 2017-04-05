/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* wrapCpssDxChIpCtrl.c
*
* DESCRIPTION:
*       Wrapper functions for IpCtrl cpss.dxCh functions
*
* DEPENDENCIES:
*       None.
*
* FILE REVISION NUMBER:
*       $Revision: 40 $
*
*******************************************************************************/

/* Common galtis includes */
#include <galtisAgent/wrapUtil/cmdCpssPresteraUtils.h>
#include <cmdShell/cmdDb/cmdBase.h>
#include <cmdShell/common/cmdWrapUtils.h>


/* Feature specific includes. */
#include <cpss/generic/cpssTypes.h>
#include <cpss/dxCh/dxChxGen/ip/cpssDxChIpCtrl.h>
#include <cpss/dxCh/dxChxGen/ip/cpssDxChIpTypes.h>


/*******************************************************************************
* ipCtrlMultiPortGroupsBmpGet
*
* DESCRIPTION:
*       Get the portGroupsBmp for multi port groups device.
*       when 'enabled' --> wrappers will use the APIs
*       with portGroupsBmp parameter
*
* INPUTS:
*       devNum - device number
*
* OUTPUTS:
*       enablePtr - (pointer to)enable / disable the use of APIs with portGroupsBmp parameter.
*       portGroupsBmpPtr - (pointer to)port groups bmp , relevant only when enable = GT_TRUE
*
* RETURNS:
*       NONE
*
* COMMENTS:
*
*******************************************************************************/
static void ipCtrlMultiPortGroupsBmpGet
(
    IN   GT_U8               devNum,
    OUT  GT_BOOL             *enablePtr,
    OUT  GT_PORT_GROUPS_BMP  *portGroupsBmpPtr
)
{
    /* default */
    *enablePtr  = GT_FALSE;
    *portGroupsBmpPtr = CPSS_PORT_GROUP_UNAWARE_MODE_CNS;

    if (0 == PRV_CPSS_IS_DEV_EXISTS_MAC(devNum))
    {
        return;
    }

    utilMultiPortGroupsBmpGet(devNum, enablePtr, portGroupsBmpPtr);
}


/* Port Group and Regular version wrapper     */
/* description see in original function header */
static GT_STATUS pg_wrap_cpssDxChIpPortGroupCntSet
(
    IN  GT_U8                        devNum,
    IN CPSS_IP_CNT_SET_ENT           cntSet,
    IN CPSS_DXCH_IP_COUNTER_SET_STC  *countersPtr
)
{
    GT_BOOL             pgEnable; /* multi port group  enable */
    GT_PORT_GROUPS_BMP  pgBmp;    /* port group BMP           */

    ipCtrlMultiPortGroupsBmpGet(devNum, &pgEnable, &pgBmp);

    if (pgEnable == GT_FALSE)
    {
        return cpssDxChIpCntSet(devNum, cntSet, countersPtr);
    }
    else
    {
        return cpssDxChIpPortGroupCntSet(devNum, pgBmp, cntSet, countersPtr);
    }
}

/* Port Group and Regular version wrapper     */
/* description see in original function header */
static GT_STATUS pg_wrap_cpssDxChIpPortGroupCntGet
(
    IN  GT_U8                         devNum,
    IN  CPSS_IP_CNT_SET_ENT           cntSet,
    OUT CPSS_DXCH_IP_COUNTER_SET_STC  *countersPtr
)
{
    GT_BOOL             pgEnable; /* multi port group  enable */
    GT_PORT_GROUPS_BMP  pgBmp;    /* port group BMP           */

    ipCtrlMultiPortGroupsBmpGet(devNum, &pgEnable, &pgBmp);

    if (pgEnable == GT_FALSE)
    {
        return cpssDxChIpCntGet(devNum, cntSet, countersPtr);
    }
    else
    {
        return cpssDxChIpPortGroupCntGet(devNum, pgBmp, cntSet, countersPtr);
    }
}

/* Port Group and Regular version wrapper     */
/* description see in original function header */
static GT_STATUS pg_wrap_cpssDxChIpDropCntGet
(
    IN  GT_U8     devNum,
    OUT GT_U32    *dropPktsPtr
)
{
    GT_BOOL             pgEnable; /* multi port group  enable */
    GT_PORT_GROUPS_BMP  pgBmp;    /* port group BMP           */

    ipCtrlMultiPortGroupsBmpGet(devNum, &pgEnable, &pgBmp);

    if (pgEnable == GT_FALSE)
    {
        return cpssDxChIpDropCntGet(devNum, dropPktsPtr);
    }
    else
    {
        return cpssDxChIpPortGroupDropCntGet(devNum, pgBmp, dropPktsPtr);
    }
}

/* Port Group and Regular version wrapper     */
/* description see in original function header */
static GT_STATUS pg_wrap_cpssDxChIpDropCntSet
(
    IN  GT_U8     devNum,
    IN GT_U32     dropPkts
)
{
    GT_BOOL             pgEnable; /* multi port group  enable */
    GT_PORT_GROUPS_BMP  pgBmp;    /* port group BMP           */

    ipCtrlMultiPortGroupsBmpGet(devNum, &pgEnable, &pgBmp);

    if (pgEnable == GT_FALSE)
    {
        return cpssDxChIpDropCntSet(devNum, dropPkts);
    }
    else
    {
        return cpssDxChIpPortGroupDropCntSet(devNum, pgBmp, dropPkts);
    }
}


/* Port Group and Regular version wrapper     */
/* description see in original function header */
static GT_STATUS pg_wrap_cpssDxChIpRouterBridgedPacketsExceptionCntGet
(
    IN  GT_U8     devNum,
    OUT GT_U32    *routerBridgedExceptionPktsPtr
)
{
    GT_BOOL             pgEnable; /* multi port group  enable */
    GT_PORT_GROUPS_BMP  pgBmp;    /* port group BMP           */

    ipCtrlMultiPortGroupsBmpGet(devNum, &pgEnable, &pgBmp);

    if (pgEnable == GT_FALSE)
    {
        return cpssDxChIpRouterBridgedPacketsExceptionCntGet(devNum,
                                                             routerBridgedExceptionPktsPtr);
    }
    else
    {
        return cpssDxChIpPortGroupRouterBridgedPacketsExceptionCntGet(devNum,
                                                                      pgBmp,
                                                                      routerBridgedExceptionPktsPtr);
    }
}

/* Port Group and Regular version wrapper     */
/* description see in original function header */
static GT_STATUS pg_wrap_cpssDxChIpRouterBridgedPacketsExceptionCntSet
(
    IN  GT_U8     devNum,
    IN GT_U32     routerBridgedExceptionPkts
)
{
    GT_BOOL             pgEnable; /* multi port group  enable */
    GT_PORT_GROUPS_BMP  pgBmp;    /* port group BMP           */

    ipCtrlMultiPortGroupsBmpGet(devNum, &pgEnable, &pgBmp);

    if (pgEnable == GT_FALSE)
    {
        return cpssDxChIpRouterBridgedPacketsExceptionCntSet(devNum,
                                                             routerBridgedExceptionPkts);
    }
    else
    {
        return cpssDxChIpPortGroupRouterBridgedPacketsExceptionCntSet(devNum,
                                                                      pgBmp,
                                                                      routerBridgedExceptionPkts);
    }
}

/* Port Group and Regular version wrapper     */
/* description see in original function header */
static GT_STATUS pg_wrap_cpssDxChIpPortGroupMultiTargetQueueFullDropCntGet
(
    IN  GT_U8     devNum,
    OUT GT_U32    *dropPktsPtr
)
{
    GT_BOOL             pgEnable; /* multi port group  enable */
    GT_PORT_GROUPS_BMP  pgBmp;    /* port group BMP           */

    ipCtrlMultiPortGroupsBmpGet(devNum, &pgEnable, &pgBmp);

    if (pgEnable == GT_FALSE)
    {
        return cpssDxChIpMultiTargetQueueFullDropCntGet(devNum, dropPktsPtr);
    }
    else
    {
        return cpssDxChIpPortGroupMultiTargetQueueFullDropCntGet(devNum, pgBmp,
                                                                 dropPktsPtr);
    }
}

/* Port Group and Regular version wrapper     */
/* description see in original function header */
static GT_STATUS pg_wrap_cpssDxChIpMultiTargetQueueFullDropCntSet
(
    IN  GT_U8    devNum,
    IN GT_U32    dropPkts
)
{
    GT_BOOL             pgEnable; /* multi port group  enable */
    GT_PORT_GROUPS_BMP  pgBmp;    /* port group BMP           */

    ipCtrlMultiPortGroupsBmpGet(devNum, &pgEnable, &pgBmp);

    if (pgEnable == GT_FALSE)
    {
        return cpssDxChIpMultiTargetQueueFullDropCntSet(devNum, dropPkts);
    }
    else
    {
        return cpssDxChIpPortGroupMultiTargetQueueFullDropCntSet(devNum, pgBmp,
                                                                 dropPkts);
    }
}

/* Port Group and Regular version wrapper     */
/* description see in original function header */
static GT_STATUS pg_wrap_cpssDxChIpMllSkippedEntriesCountersGet
(
    IN  GT_U8     devNum,
    OUT GT_U32    *skipCounterPtr
)
{
    GT_BOOL             pgEnable; /* multi port group  enable */
    GT_PORT_GROUPS_BMP  pgBmp;    /* port group BMP           */

    ipCtrlMultiPortGroupsBmpGet(devNum, &pgEnable, &pgBmp);

    if (pgEnable == GT_FALSE)
    {
        return cpssDxChIpMllSkippedEntriesCountersGet(devNum, skipCounterPtr);
    }
    else
    {
        return cpssDxChIpMllPortGroupSkippedEntriesCountersGet(devNum, pgBmp, skipCounterPtr);
    }
}


/* Port Group and Regular version wrapper     */
/* description see in original function header */
static GT_STATUS pg_wrap_cpssDxChIpMllCntGet
(
    IN  GT_U8     devNum,
    IN GT_U32     mllCntSet,
    OUT GT_U32    *mllOutMCPktsPtr
)
{
    GT_BOOL             pgEnable; /* multi port group  enable */
    GT_PORT_GROUPS_BMP  pgBmp;    /* port group BMP           */

    ipCtrlMultiPortGroupsBmpGet(devNum, &pgEnable, &pgBmp);

    if (pgEnable == GT_FALSE)
    {
        return cpssDxChIpMllCntGet(devNum, mllCntSet, mllOutMCPktsPtr);
    }
    else
    {
        return cpssDxChIpPortGroupMllCntGet(devNum, pgBmp, mllCntSet,
                                            mllOutMCPktsPtr);
    }
}

/* Port Group and Regular version wrapper     */
/* description see in original function header */
static GT_STATUS pg_wrap_cpssDxChIpPortGroupMllCntSet
(
    IN  GT_U8     devNum,
    IN GT_U32     mllCntSet,
    IN GT_U32     mllOutMCPkts
)
{
    GT_BOOL             pgEnable; /* multi port group  enable */
    GT_PORT_GROUPS_BMP  pgBmp;    /* port group BMP           */

    ipCtrlMultiPortGroupsBmpGet(devNum, &pgEnable, &pgBmp);

    if (pgEnable == GT_FALSE)
    {
        return cpssDxChIpMllCntSet(devNum, mllCntSet, mllOutMCPkts);
    }
    else
    {
        return cpssDxChIpPortGroupMllCntSet(devNum, pgBmp, mllCntSet,
                                            mllOutMCPkts);
    }
}

/* Port Group and Regular version wrapper     */
/* description see in original function header */
static GT_STATUS pg_wrap_cpssDxChIpMllSilentDropCntGet
(
    IN  GT_U8     devNum,
    IN  GT_U32    *silentDropPktsPtr
)
{
    GT_BOOL             pgEnable; /* multi port group  enable */
    GT_PORT_GROUPS_BMP  pgBmp;    /* port group BMP           */

    ipCtrlMultiPortGroupsBmpGet(devNum, &pgEnable, &pgBmp);

    if (pgEnable == GT_FALSE)
    {
        return cpssDxChIpMllSilentDropCntGet(devNum, silentDropPktsPtr);
    }
    else
    {
        return cpssDxChIpMllPortGroupSilentDropCntGet(devNum, pgBmp, silentDropPktsPtr);
    }
}


/* Port Group and Regular version wrapper     */
/* description see in original function header */
static GT_STATUS pg_wrap_cpssDxChIpMultiTargetRateShaperSet
(
    IN   GT_U8      devNum,
    IN   GT_BOOL    multiTargetRateShaperEnable,
    IN   GT_U32     windowSize
)
{
    GT_BOOL             pgEnable; /* multi port group  enable */
    GT_PORT_GROUPS_BMP  pgBmp;    /* port group BMP           */

    ipCtrlMultiPortGroupsBmpGet(devNum, &pgEnable, &pgBmp);

    if (pgEnable == GT_FALSE)
    {
        return cpssDxChIpMultiTargetRateShaperSet(devNum,
                                                  multiTargetRateShaperEnable,
                                                  windowSize);
    }
    else
    {
        return cpssDxChIpPortGroupMultiTargetRateShaperSet(devNum, pgBmp,
                                                    multiTargetRateShaperEnable,
                                                    windowSize);
    }
}

/* Port Group and Regular version wrapper     */
/* description see in original function header */
static GT_STATUS pg_wrap_cpssDxChIpMultiTargetRateShaperGet
(
    IN    GT_U8      devNum,
    OUT   GT_BOOL    *multiTargetRateShaperEnablePtr,
    OUT   GT_U32     *windowSizePtr
)
{
    GT_BOOL             pgEnable; /* multi port group  enable */
    GT_PORT_GROUPS_BMP  pgBmp;    /* port group BMP           */

    ipCtrlMultiPortGroupsBmpGet(devNum, &pgEnable, &pgBmp);

    if (pgEnable == GT_FALSE)
    {
        return cpssDxChIpMultiTargetRateShaperGet(devNum,
                                                multiTargetRateShaperEnablePtr,
                                                windowSizePtr);
    }
    else
    {
        return cpssDxChIpPortGroupMultiTargetRateShaperGet(devNum, pgBmp,
                                                multiTargetRateShaperEnablePtr,
                                                windowSizePtr);
    }
}

/* Port Group and Regular version wrapper     */
/* description see in original function header */
static GT_STATUS pg_wrap_cpssDxChIpPortGroupRouterSourceIdSet
(
    IN  GT_U8                           devNum,
    IN  CPSS_IP_UNICAST_MULTICAST_ENT   ucMcSet,
    IN  GT_U32                          sourceId,
    IN  GT_U32                          sourceIdMask
)
{
    GT_BOOL             pgEnable; /* multi port group  enable */
    GT_PORT_GROUPS_BMP  pgBmp;    /* port group BMP           */

    ipCtrlMultiPortGroupsBmpGet(devNum, &pgEnable, &pgBmp);

    if (pgEnable == GT_FALSE)
    {
        return cpssDxChIpRouterSourceIdSet(devNum, ucMcSet, sourceId, sourceIdMask);
    }
    else
    {
        return cpssDxChIpPortGroupRouterSourceIdSet(devNum, pgBmp,
                                                    ucMcSet, sourceId, sourceIdMask);
    }
}

/* Port Group and Regular version wrapper     */
/* description see in original function header */
static GT_STATUS pg_wrap_cpssDxChIpPortGroupRouterSourceIdGet
(
    IN  GT_U8                           devNum,
    IN  CPSS_IP_UNICAST_MULTICAST_ENT   ucMcSet,
    OUT GT_U32                          *sourceIdPtr,
    OUT GT_U32                          *sourceIdMaskPtr
)
{
    GT_BOOL             pgEnable; /* multi port group  enable */
    GT_PORT_GROUPS_BMP  pgBmp;    /* port group BMP           */

    ipCtrlMultiPortGroupsBmpGet(devNum, &pgEnable, &pgBmp);

    if (pgEnable == GT_FALSE)
    {
        return cpssDxChIpRouterSourceIdGet(devNum, ucMcSet, sourceIdPtr, sourceIdMaskPtr);
    }
    else
    {
        return cpssDxChIpPortGroupRouterSourceIdGet(devNum, pgBmp,
                                                    ucMcSet, sourceIdPtr, sourceIdMaskPtr);
    }
}

/*******************************************************************************
* cpssDxChIpSpecialRouterTriggerEnable
*
* DESCRIPTION:
*       Sets the special router trigger enable modes for packets with bridge
*       command other then FORWARD or MIRROR.
*
* APPLICABLE DEVICES:
*       All DxCh2 devices.
*
* INPUTS:
*       devNum              - physical device number
*       bridgeExceptionCmd  - the bridge exception command the packet arrived to
*                             the router with.
*       enableRouterTrigger - enable /disable router trigger.
*
* OUTPUTS:
*       none
*
* RETURNS:
*       GT_OK   - on success
*       GT_NOT_INITIALIZED - The library was not initialized.
*       GT_FAIL - on error
*
* COMMENTS:
*       none.
*
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChIpSpecialRouterTriggerEnable

(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                        result;

    GT_U8                            devNum;
    CPSS_DXCH_IP_BRG_EXCP_CMD_ENT    bridgeExceptionCmd;
    GT_BOOL                          enableRouterTrigger;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    bridgeExceptionCmd = (CPSS_DXCH_IP_BRG_EXCP_CMD_ENT)inArgs[1];
    enableRouterTrigger = (GT_BOOL)inArgs[2];

    /* call cpss api function */
    result = cpssDxChIpSpecialRouterTriggerEnable(devNum, bridgeExceptionCmd,
                                                         enableRouterTrigger);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}

/*******************************************************************************
* cpssDxChIpSpecialRouterTriggerEnableGet
*
* DESCRIPTION:
*       Gets the special router trigger enable modes for packets with bridge
*       command other then FORWARD or MIRROR.
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; xCat3; Lion; Lion2; Lion3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; xCat2.
*
* INPUTS:
*       devNum              - the device number
*       bridgeExceptionCmd  - the bridge exception command the packet arrived to
*                             the router with.
*
* OUTPUTS:
*       enableRouterTriggerPtr - router trigger state (enable/disable).
*
* RETURNS:
*       GT_OK                    - on success
*       GT_NOT_INITIALIZED       - The library was not initialized.
*       GT_FAIL                  - on error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong devNum or bridgeExceptionCmd
*       GT_BAD_PTR               - on NULL ptr
*
* COMMENTS:
*       None.
*
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChIpSpecialRouterTriggerEnableGet

(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                        result;

    GT_U8                            devNum;
    CPSS_DXCH_IP_BRG_EXCP_CMD_ENT    bridgeExceptionCmd;
    GT_BOOL                          enableRouterTrigger;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    bridgeExceptionCmd = (CPSS_DXCH_IP_BRG_EXCP_CMD_ENT)inArgs[1];

    /* call cpss api function */
    result = cpssDxChIpSpecialRouterTriggerEnableGet(devNum, bridgeExceptionCmd,
                                                         &enableRouterTrigger);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", enableRouterTrigger);
    return CMD_OK;
}

/*******************************************************************************
* cpssDxChIpExceptionCommandSet
*
* DESCRIPTION:
*       set a specific excption commnad.
*
* APPLICABLE DEVICES:
*       All DxCh2 devices.
*
* INPUTS:
*       devNum        - physical device number
*       exceptionType  - the exception type
*       protocolStack - weather to set for ipv4/v6 or both
*       command       - the command, for availble commands see
*                       CPSS_DXCH_IP_EXCEPTION_TYPE_ENT.
*
* OUTPUTS:
*       none
*
* RETURNS:
*       GT_OK   - on success
*       GT_NOT_INITIALIZED - The library was not initialized.
*       GT_FAIL - on error
*
* COMMENTS:
*       none.
* GalTis:
*
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChIpExceptionCommandSet

(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                        result;

    GT_U8                            devNum;
    CPSS_DXCH_IP_EXCEPTION_TYPE_ENT  exceptionType;
    CPSS_IP_PROTOCOL_STACK_ENT       protocolStack;
    CPSS_PACKET_CMD_ENT              command;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    exceptionType = (CPSS_DXCH_IP_EXCEPTION_TYPE_ENT)inArgs[1];
    protocolStack = (CPSS_IP_PROTOCOL_STACK_ENT)inArgs[2];
    command = (CPSS_PACKET_CMD_ENT)inArgs[3];

    /* call cpss api function */
    result = cpssDxChIpExceptionCommandSet(devNum, exceptionType,
                                          protocolStack, command);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}
/*******************************************************************************
* cpssDxChIpExceptionCommandGetExt
*
* DESCRIPTION:
*       set a specific exception command.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Lion3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum        - the device number
*       exceptionType  - the exception type
*       protocolStack - whether to set for ipv4/v6 or both
*
*
* OUTPUTS:
*       exceptionCmdPtr       - the command, for availble commands see
*                       CPSS_DXCH_IP_EXCEPTION_TYPE_ENT
*
* RETURNS:
*       GT_OK                    - on success
*       GT_NOT_INITIALIZED       - The library was not initialized.
*       GT_BAD_PARAM             - Illegal parameter in function called
*       GT_FAIL                  - on error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_BAD_PTR               - on NULL pointer
*       GT_BAD_STATE             - on invalid hardware value read
*
* COMMENTS:
*       None.
* GalTis:
*
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChIpExceptionCommandGetExt

(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                        result;
    GT_U8                            devNum;
    CPSS_DXCH_IP_EXCEPTION_TYPE_ENT  exceptionType;
    CPSS_IP_PROTOCOL_STACK_ENT       protocolStack;
    CPSS_PACKET_CMD_ENT              commandPtr;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    exceptionType = (CPSS_DXCH_IP_EXCEPTION_TYPE_ENT)inArgs[1];
    protocolStack = (CPSS_IP_PROTOCOL_STACK_ENT)inArgs[2];

    /* call cpss api function */
    result = cpssDxChIpExceptionCommandGet(devNum, exceptionType,
                                          protocolStack, &commandPtr);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", commandPtr);
    return CMD_OK;
}

/*******************************************************************************
* cpssDxChIpUcRouteAgingModeSet
*
* DESCRIPTION:
*      Sets the global route aging modes.
*
* APPLICABLE DEVICES:
*       All DxCh2 devices.
*
* INPUTS:
*       devNum           - physical device number
*       refreshEnable - Enables the global routing activity refresh mechanism
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChIpUcRouteAgingModeSet

(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS            result;

    GT_U8                devNum;
    GT_BOOL              refreshEnable;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    refreshEnable = (GT_BOOL)inArgs[1];

    /* call cpss api function */
    result = cpssDxChIpUcRouteAgingModeSet(devNum, refreshEnable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}

/*******************************************************************************
* cpssDxChIpUcRouteAgingModeGet
*
* DESCRIPTION:
*      Gets the global route aging modes.
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; xCat3; Lion; Lion2; Lion3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; xCat2.
*
* INPUTS:
*       devNum           - the device number
*
* OUTPUTS:
*       refreshEnablePtr - Enables the global routing activity refresh mechanism
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - Illegal parameter in function called
*       GT_FAIL                  - on error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_BAD_PTR               - on NULL ptr
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChIpUcRouteAgingModeGet

(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS            result;

    GT_U8                devNum;
    GT_BOOL              refreshEnable;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];

    /* call cpss api function */
    result = cpssDxChIpUcRouteAgingModeGet(devNum, &refreshEnable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", refreshEnable);
    return CMD_OK;
}









/*******************************************************************************
* cpssDxChIpRouterSourceIdSet
*
* DESCRIPTION:
*      set the router source id assignmnet.
*
* APPLICABLE DEVICES:
*       All DxCh2 devices.
*
* INPUTS:
*       devNum        - physical device number
*       ucMcSet       - weather to set it for unicast packets or multicast.
*       sourceId      - the assigned source id.
*
* OUTPUTS:
*      none.
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChIpRouterSourceIdSet

(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                       result;

    GT_U8                           devNum;
    CPSS_IP_UNICAST_MULTICAST_ENT   ucMcSet;
    GT_U32                          sourceId;
    GT_U32                          sourceIdMask = 0xFFF;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    ucMcSet = (CPSS_IP_UNICAST_MULTICAST_ENT)inArgs[1];
    sourceId = (GT_U32)inArgs[2];

    /* call cpss api function */
    result = pg_wrap_cpssDxChIpPortGroupRouterSourceIdSet(devNum, ucMcSet, sourceId, sourceIdMask);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}








/*******************************************************************************
* cpssDxChIpRouterSourceIdGet
*
* DESCRIPTION:
*      get the router source id.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Lion3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum        - the device number
*       ucMcSet       - whether to set it for unicast packets or multicast.
*       sourceId      - the assigned source id.
*
* OUTPUTS:
*      None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - Illegal parameter in function called
*       GT_OUT_OF_RANGE          - on out of range parameter
*       GT_FAIL                  - on error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChIpRouterSourceIdGet

(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                       result;

    GT_U8                           devNum;
    CPSS_IP_UNICAST_MULTICAST_ENT   ucMcSet;
    GT_U32                          sourceId;
    GT_U32                          sourceIdMask;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    ucMcSet = (CPSS_IP_UNICAST_MULTICAST_ENT)inArgs[1];

    /* call cpss api function */
    result = pg_wrap_cpssDxChIpPortGroupRouterSourceIdGet(devNum, ucMcSet, &sourceId, &sourceIdMask);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", sourceId);
    return CMD_OK;
}



/*******************************************************************************
* cpssDxChIpRouterSourceIdSet
*
* DESCRIPTION:
*      set the router source id assignmnet.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum        - the device number
*       ucMcSet       - whether to set it for unicast packets or multicast.
*       sourceId      - the assigned source id.
*       sourceIdMask  - the assigned source id mask, 
*                       relevant for ucMcSet CPSS_IP_MULTICAST_E.
*                       (APPLICABLE DEVICES: Bobcat2; Caelum; Bobcat3)
*
* OUTPUTS:
*      None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - Illegal parameter in function called
*       GT_OUT_OF_RANGE          - on out of range parameter
*       GT_FAIL                  - on error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChIpRouterSourceId_1Set

(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                       result;

    GT_U8                           devNum;
    CPSS_IP_UNICAST_MULTICAST_ENT   ucMcSet;
    GT_U32                          sourceIdMask = 0xFFF;
    GT_U32                          sourceId;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    ucMcSet = (CPSS_IP_UNICAST_MULTICAST_ENT)inArgs[1];
    sourceId = (GT_U32)inArgs[2];
    sourceIdMask = (GT_U32)inArgs[3];

    /* call cpss api function */
    result = pg_wrap_cpssDxChIpPortGroupRouterSourceIdSet(devNum, ucMcSet, sourceId, sourceIdMask);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}









/*******************************************************************************
* cpssDxChIpRouterSourceIdGet
*
* DESCRIPTION:
*      get the router source id.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum        - the device number
*       ucMcSet       - whether to set it for unicast packets or multicast.
*
* OUTPUTS:
*       sourceIdPtr   - the assigned source id.
*       sourceIdMaskPtr - the assigned source id mask, 
*                       relevant for ucMcSet CPSS_IP_MULTICAST_E.
*                       (APPLICABLE DEVICES: Bobcat2; Caelum; Bobcat3)
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - Illegal parameter in function called
*       GT_OUT_OF_RANGE          - on out of range parameter
*       GT_FAIL                  - on error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChIpRouterSourceId_1Get

(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                       result;

    GT_U8                           devNum;
    CPSS_IP_UNICAST_MULTICAST_ENT   ucMcSet;
    GT_U32                          sourceId;
    GT_U32                          sourceIdMask;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    ucMcSet = (CPSS_IP_UNICAST_MULTICAST_ENT)inArgs[1];

    /* call cpss api function */
    result = pg_wrap_cpssDxChIpPortGroupRouterSourceIdGet(devNum, ucMcSet, &sourceId, &sourceIdMask);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d, %d", sourceId, sourceIdMask);
    return CMD_OK;
}













/*******************************************************************************
* cpssDxChIpFailRpfCtrlTrafficMultiTargetTCQueueSet
*
* DESCRIPTION:
*      Sets the multi-target TC queue assigned to multi-target Control
*      packets and to RPF Fail packets where the RPF Fail Command is assigned
*      from the MLL entry.
*
* APPLICABLE DEVICES:
*       All DxCh2 devices.
*
* INPUTS:
*       devNum                    - physical device number
*       ctrlMultiTargetTCQueue    - The multi-target TC queue for control
*                                   traffic. There are 4 possible queues.
*       failRpfMultiTargetTCQueue - the multi-target TC queue for fail rpf
*                                   traffic. There are 4 possible queues.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on error
*
* COMMENTS:
*       A packet is considered as a multi-target control if it is a FROM_CPU DSA
*       Tagged with DSA<Use_Vidx> = 1 or a multi-target packet that is also to
*       be mirrored to the CPU
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChIpFailRpfCtrlTrafficMultiTargetTCQueueSet

(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                       result;

    GT_U8                           devNum;
    GT_U32                          ctrlMultiTargetTCQueue;
    GT_U32                          failRpfMultiTargetTCQueue;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    ctrlMultiTargetTCQueue = (GT_U32)inArgs[1];
    failRpfMultiTargetTCQueue = (GT_U32)inArgs[2];

    /* call cpss api function */
    result = cpssDxChIpFailRpfCtrlTrafficMultiTargetTCQueueSet(devNum,
                                               ctrlMultiTargetTCQueue,
                                            failRpfMultiTargetTCQueue);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}

/*******************************************************************************
* cpssDxChIpFailRpfCtrlTrafficMultiTargetTCQueueGet
*
* DESCRIPTION:
*      Gets the multi-target TC queue assigned to multi-target Control
*      packets and to RPF Fail packets where the RPF Fail Command is assigned
*      from the MLL entry.
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; xCat3; Lion; Lion2; Lion3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; xCat2.
*
* INPUTS:
*       devNum                    - the device number
*
* OUTPUTS:
*       ctrlMultiTargetTCQueuePtr    - The multi-target TC queue for control
*                                      traffic. There are 4 possible queues
*       failRpfMultiTargetTCQueuePtr - the multi-target TC queue for fail rpf
*                                      traffic. There are 4 possible queues
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - Illegal parameter in function called
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_BAD_PTR               - on NULL Ptr
*
* COMMENTS:
*       A packet is considered as a multi-target control if it is a FROM_CPU DSA
*       Tagged with DSA<Use_Vidx> = 1 or a multi-target packet that is also to
*       be mirrored to the CPU
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChIpFailRpfCtrlTrafficMultiTargetTCQueueGet

(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                       result;

    GT_U8                           devNum;
    GT_U32                          ctrlMultiTargetTCQueue;
    GT_U32                          failRpfMultiTargetTCQueue;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];

    /* call cpss api function */
    result = cpssDxChIpFailRpfCtrlTrafficMultiTargetTCQueueGet(devNum,
                                               &ctrlMultiTargetTCQueue,
                                            &failRpfMultiTargetTCQueue);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, " %d, %d", ctrlMultiTargetTCQueue,
                                             failRpfMultiTargetTCQueue);
    return CMD_OK;
}

/*******************************************************************************
* cpssDxChIpQosProfileToMultiTargetTCQueueMapSet
*
* DESCRIPTION:
*     Sets the Qos Profile to multi-target TC queue mapping.
*
* APPLICABLE DEVICES:
*       All DxCh2 devices.
*
* INPUTS:
*       devNum             - physical device number
*       qosProfile         - QOS Profile index
*       multiTargeTCQueue  - the multi-target TC queue. There are 4 possible
*                            queues.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChIpQosProfileToMultiTargetTCQueueMapSet

(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                       result;

    GT_U8                           devNum;
    GT_U32                          qosProfile;
    GT_U32                          multiTargeTCQueue;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    qosProfile = (GT_U32)inArgs[1];
    multiTargeTCQueue = (GT_U32)inArgs[2];

    /* call cpss api function */
    result = cpssDxChIpQosProfileToMultiTargetTCQueueMapSet(devNum, qosProfile,
                                                             multiTargeTCQueue);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}

/*******************************************************************************
* cpssDxChIpQosProfileToMultiTargetTCQueueMapGet
*
* DESCRIPTION:
*     Sets the Qos Profile to multi-target TC queue mapping.
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; xCat3; Lion; Lion2; Lion3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; xCat2.
*
* INPUTS:
*       devNum             - the device number
*       qosProfile         - QOS Profile index
*
* OUTPUTS:
*       multiTargetTCQueuePtr - the multi-target TC queue. There are 4 possible
*                               queues.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - Illegal parameter in function called
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChIpQosProfileToMultiTargetTCQueueMapGet

(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                       result;

    GT_U8                           devNum;
    GT_U32                          qosProfile;
    GT_U32                          multiTargeTCQueue;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    qosProfile = (GT_U32)inArgs[1];

    /* call cpss api function */
    result = cpssDxChIpQosProfileToMultiTargetTCQueueMapGet(devNum, qosProfile,
                                                             &multiTargeTCQueue);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", multiTargeTCQueue);
    return CMD_OK;
}






/*******************************************************************************
* cpssDxChIpTcDpToMultiTargetTcQueueMapSet
*
* DESCRIPTION:
*     Sets the packet's {TC,DP} to multi-target TC queue mapping.
*     (It is used for packets received on cascade ports configured to work in extended QoS mode)
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum             - the device number
*       tc                 - traffic class assigned to the packet.
*                            (received on a cascade port configured for extended Global QoS mode)
*                            (APPLICABLE RANGES: 0..7).
*       dp                 - drop precedence assigned to the packet for tail drop.
*                           (received on a cascade port configured for extended Global QoS mode)
*                           (APPLICABLE RANGES: 0..2).
*       multiTargetTCQueue - the multi-target TC queue. (APPLICABLE RANGES: 0..3).
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - Illegal parameter in function called
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChIpTcDpToMultiTargetTcQueueMapSet

(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                       result;

    GT_U8                           devNum;
    GT_U32                          tc;
    CPSS_DP_LEVEL_ENT               dp;
    GT_U32                          multiTargeTCQueue;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum            = (GT_U8)inArgs[0];
    tc                = (GT_U32)inArgs[1];
    dp                = (CPSS_DP_LEVEL_ENT)inArgs[2];
    multiTargeTCQueue = (GT_U32)inArgs[3];

    /* call cpss api function */
    result = cpssDxChIpTcDpToMultiTargetTcQueueMapSet(devNum, tc, dp, multiTargeTCQueue);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}

/*******************************************************************************
* cpssDxChIpTcDpToMultiTargetTCQueueMapGet
*
* DESCRIPTION:
*     Gets the packet's {TC,DP} to multi-target TC queue mapping.
*     (It is used for packets received on cascade ports configured to work in extended QoS mode)
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum             - the device number
*       tc                 - traffic class assigned to the packet.
*                            (received on a cascade port configured for extended Global QoS mode)
*                            (APPLICABLE RANGES: 0..7).
*       dp                 - drop precedence assigned to the packet for tail drop.
*                           (received on a cascade port configured for extended Global QoS mode)
*                           (APPLICABLE RANGES: 0..2).
*
* OUTPUTS:
*       multiTargetTCQueuePtr - the multi-target TC queue. (APPLICABLE RANGES: 0..3).
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - Illegal parameter in function called
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChIpTcDpToMultiTargetTCQueueMapGet

(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                       result;
    GT_U8                           devNum;
    GT_U32                          tc;
    CPSS_DP_LEVEL_ENT               dp;
    GT_U32                          multiTargeTCQueue;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    tc     = (GT_U32)inArgs[1];
    dp     = (CPSS_DP_LEVEL_ENT)inArgs[2];
    /* call cpss api function */
    result = cpssDxChIpTcDpToMultiTargetTcQueueMapGet(devNum, tc, dp, &multiTargeTCQueue);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", multiTargeTCQueue);
    return CMD_OK;
}


/*******************************************************************************
* cpssDxChIpMultiTargetQueueFullDropCntGet
*
* DESCRIPTION:
*      Get the multi target queue full drop packet counter.
*
* APPLICABLE DEVICES:
*       All DxCh2 devices.
*
* INPUTS:
*       devNum      - physical device number
*
* OUTPUTS:
*       dropPktsPtr - the number of counted dropped packets.
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChIpMultiTargetQueueFullDropCntGet

(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                       result;

    GT_U8                           devNum;
    GT_U32                          dropPktsPtr;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];

    /* call cpss api function */
    result = pg_wrap_cpssDxChIpPortGroupMultiTargetQueueFullDropCntGet(devNum,
                                                                  &dropPktsPtr);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", dropPktsPtr);
    return CMD_OK;
}

/*******************************************************************************
* cpssDxChIpMultiTargetQueueFullDropCntSet
*
* DESCRIPTION:
*      set the multi target queue full drop packet counter.
*
* APPLICABLE DEVICES:
*       All DxCh2 devices.
*
* INPUTS:
*       devNum      - physical device number
*       dropPkts    - the counter value to set.
*
* OUTPUTS:
*       none.
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChIpMultiTargetQueueFullDropCntSet

(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                       result;

    GT_U8                           devNum;
    GT_U32                          dropPkts;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    dropPkts = (GT_U32)inArgs[1];

    /* call cpss api function */
    result = pg_wrap_cpssDxChIpMultiTargetQueueFullDropCntSet(devNum, dropPkts);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}

/*******************************************************************************
* cpssDxChIpMultiTargetTCQueueSchedModeSet
*
* DESCRIPTION:
*      sets the multi-target TC queue scheduling mode.
*
* APPLICABLE DEVICES:
*       All DxCh2 devices.
*
* INPUTS:
*       devNum             - physical device number
*       multiTargetTcQueue - the multi-target TC queue. There are 4 possible
*                            queues.
*       schedulingMode     - the scheduling mode.
*       queueWeight        - the queue weight for SDWRR scheduler
*                           (relvant only if schedMode =
*                            CPSS_DXCH_IP_MT_TC_QUEUE_SDWRR_SCHED_MODE_E).
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on error
*
* COMMENTS:
*       none.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChIpMultiTargetTCQueueSchedModeSet

(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                                   result;

    GT_U8                                       devNum;
    GT_U32                                      multiTargetTcQueue;
    CPSS_DXCH_IP_MT_TC_QUEUE_SCHED_MODE_ENT     schedulingMode;
    GT_U32                                      queueWeight;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    multiTargetTcQueue = (GT_U32)inArgs[1];
    schedulingMode = (CPSS_DXCH_IP_MT_TC_QUEUE_SCHED_MODE_ENT)inArgs[2];
    queueWeight = (GT_U32)inArgs[3];

    /* call cpss api function */
    result = cpssDxChIpMultiTargetTCQueueSchedModeSet(devNum,
                                          multiTargetTcQueue,
                                              schedulingMode,
                                                 queueWeight);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}


/*******************************************************************************
* cpssDxChIpMultiTargetTCQueueSchedModeGet
*
* DESCRIPTION:
*      gets the multi-target TC queue scheduling mode.
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; xCat3; Lion; Lion2; Lion3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; xCat2.
*
* INPUTS:
*       devNum             - the device number
*       multiTargetTcQueue - the multi-target TC queue. There are 4 possible
*                            queues.
*
* OUTPUTS:
*       schedulingModePtr     - the scheduling mode.
*       queueWeightPtr        - the queue weight for SDWRR scheduler
*                               (relvant only if schedMode =
*                               CPSS_DXCH_IP_MT_TC_QUEUE_SDWRR_SCHED_MODE_E).
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - Illegal parameter in function called
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChIpMultiTargetTCQueueSchedModeGet

(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                                   result;

    GT_U8                                       devNum;
    GT_U32                                      multiTargetTcQueue;
    CPSS_DXCH_IP_MT_TC_QUEUE_SCHED_MODE_ENT     schedulingMode;
    GT_U32                                      queueWeight;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    multiTargetTcQueue = (GT_U32)inArgs[1];

    /* call cpss api function */
    result = cpssDxChIpMultiTargetTCQueueSchedModeGet(devNum,
                                          multiTargetTcQueue,
                                              &schedulingMode,
                                                 &queueWeight);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d, %d",schedulingMode, queueWeight);
    return CMD_OK;
}

/*******************************************************************************
* cpssDxChIpBridgeServiceEnable
*
* DESCRIPTION:
*      enable/disable a router bridge service.
*
* APPLICABLE DEVICES:
*       All DxCh2 devices.
*
* INPUTS:
*       devNum          - physical device number
*       brgService      - the router bridge service
*       enableDisableMode - Enable/Disable mode of this function (weather
*                           to enable/disable for ipv4/ipv6/arp)
*       enableService   - weather to enable the service for the above more.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on error
*
* COMMENTS:
*       none.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChIpBridgeServiceEnable

(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                                               result;

    GT_U8                                                   devNum;
    CPSS_DXCH_IP_BRG_SERVICE_ENT                            bridgeService;
    CPSS_DXCH_IP_BRG_SERVICE_ENABLE_DISABLE_MODE_ENT        enableDisableMode;
    GT_BOOL                                                 enableService;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    bridgeService = (CPSS_DXCH_IP_BRG_SERVICE_ENT)inArgs[1];
    enableDisableMode =
                    (CPSS_DXCH_IP_BRG_SERVICE_ENABLE_DISABLE_MODE_ENT)inArgs[2];
    enableService = (GT_BOOL)inArgs[3];

    /* call cpss api function */
    result = cpssDxChIpBridgeServiceEnable(devNum, bridgeService,
                                enableDisableMode, enableService);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}

/*******************************************************************************
* cpssDxChIpBridgeServiceEnableGet
*
* DESCRIPTION:
*      enable/disable a router bridge service.
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; xCat3; Lion; Lion2; Lion3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; xCat2.
*
* INPUTS:
*       devNum            - the device number
*       bridgeService     - the router bridge service
*       enableDisableMode - Enable/Disable mode of this function (weather
*                           to enable/disable for ipv4/ipv6/arp)
*
* OUTPUTS:
*       enableServicePtr - weather to enable the service for the above more.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - Illegal parameter in function called
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_BAD_PTR               - on NULL pointer
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChIpBridgeServiceEnableGet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8                                               devNum;
    CPSS_DXCH_IP_BRG_SERVICE_ENT                        bridgeService;
    CPSS_DXCH_IP_BRG_SERVICE_ENABLE_DISABLE_MODE_ENT    enableDisableMode;
    GT_BOOL                                             enableService;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    bridgeService = (CPSS_DXCH_IP_BRG_SERVICE_ENT)inArgs[1];
    enableDisableMode =
                    (CPSS_DXCH_IP_BRG_SERVICE_ENABLE_DISABLE_MODE_ENT)inArgs[2];

    /* call cpss api function */
    result = cpssDxChIpBridgeServiceEnableGet(devNum, bridgeService,
                                            enableDisableMode, &enableService);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", enableService);
    return CMD_OK;
}
/*******************************************************************************
* cpssDxChIpMllBridgeEnable
*
* DESCRIPTION:
*      enable/disable MLL based bridging.
*
* APPLICABLE DEVICES:
*       All DxCh2 devices.
*
* INPUTS:
*       devNum          - physical device number
*       mllBridgeEnable - enable /disable MLL based bridging.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on error
*
* COMMENTS:
*       none.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChIpMllBridgeEnable

(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                result;

    GT_U8                    devNum;
    GT_BOOL                  mllBridgeEnable;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    mllBridgeEnable = (GT_BOOL)inArgs[1];

    /* call cpss api function */
    result = cpssDxChIpMllBridgeEnable(devNum, mllBridgeEnable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}

/*******************************************************************************
* cpssDxChIpMllBridgeEnableGet
*
* DESCRIPTION:
*      get state of MLL based bridging.
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; xCat3; Lion; Lion2; Lion3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; xCat2.
*
* INPUTS:
*       devNum             - the device number
*
* OUTPUTS:
*       mllBridgeEnablePtr - enable /disable MLL based bridging.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_BAD_PTR               - on NULL prt
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChIpMllBridgeEnableGet

(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                result;

    GT_U8                    devNum;
    GT_BOOL                  mllBridgeEnable;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];

    /* call cpss api function */
    result = cpssDxChIpMllBridgeEnableGet(devNum, &mllBridgeEnable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", mllBridgeEnable);
    return CMD_OK;
}

/*******************************************************************************
* cpssDxChIpMultiTargetRateShaperSet
*
* DESCRIPTION:
*      set the Multi target Rate shaper params.
*
* APPLICABLE DEVICES:
*       All DxCh2 devices.
*
* INPUTS:
*       devNum     - physical device number
*       multiTargetRateShaperEnable - weather to enable (disable) the Multi
*                                     target Rate shaper
*       windowSize - if enabled then this is the shaper window size.
*
* OUTPUTS:
*       none.
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChIpMultiTargetRateShaperSet

(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                                   result;

    GT_U8                                       devNum;
    GT_BOOL                                     multiTargetRateShaperEnable;
    GT_U32                                      windowSize;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    multiTargetRateShaperEnable = (GT_BOOL)inArgs[1];
    windowSize = (GT_U32)inArgs[2];

    /* call port group api function */
    result = pg_wrap_cpssDxChIpMultiTargetRateShaperSet(devNum,
                                                    multiTargetRateShaperEnable,
                                                    windowSize);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}

/*******************************************************************************
* cpssDxChIpMultiTargetRateShaperGet
*
* DESCRIPTION:
*      set the Multi target Rate shaper params.
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; xCat3; Lion; Lion2; Lion3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; xCat2.
*
* INPUTS:
*       devNum     - the device number
*
* OUTPUTS:
*       multiTargetRateShaperEnablePtr - weather to enable (disable) the Multi
*                                        target Rate shaper
*       windowSizePtr - if enabled then this is the shaper window size
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChIpMultiTargetRateShaperGet

(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                                   result;

    GT_U8                                       devNum;
    GT_BOOL                                     multiTargetRateShaperEnable;
    GT_U32                                      windowSize;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];


    /* call port group api function */
    result = pg_wrap_cpssDxChIpMultiTargetRateShaperGet(devNum,
                                                    &multiTargetRateShaperEnable,
                                                    &windowSize);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d, %d",multiTargetRateShaperEnable, windowSize);
    return CMD_OK;
}

/*******************************************************************************
* cpssDxChIpMultiTargetUcSchedModeSet
*
* DESCRIPTION:
*      set the Multi target/unicast sheduler mode.
*
* APPLICABLE DEVICES:
*       All DxCh2 devices.
*
* INPUTS:
*       devNum    - physical device number
*       ucSPEnable- weather the Unicast uses SP , if GT_FALSE then it means
*                   both the Unicast and multi target uses DSWRR scheduling
*       ucWeight  - if DSWRR scheduler is used (ucSPEnable == GT_FALSE) the this
*                   is the unicast weight.
*       mcWeight  - if DSWRR scheduler is used (ucSPEnable == GT_FALSE) the this
*                   is the multi target weight.
*       schedMtu  - The MTU used by the scheduler
*
* OUTPUTS:
*       none.
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChIpMultiTargetUcSchedModeSet

(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                                   result;

    GT_U8                                       devNum;
    GT_BOOL                                     ucSPEnable;
    GT_U32                                      ucWeight;
    GT_U32                                      mcWeight;
    CPSS_DXCH_IP_MT_UC_SCHED_MTU_ENT            schedMtu;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    ucSPEnable = (GT_BOOL)inArgs[1];
    ucWeight = (GT_U32)inArgs[2];
    mcWeight = (GT_U32)inArgs[3];
    schedMtu = (CPSS_DXCH_IP_MT_UC_SCHED_MTU_ENT)inArgs[4];

    /* call cpss api function */
    result = cpssDxChIpMultiTargetUcSchedModeSet(devNum, ucSPEnable, ucWeight,
                                                           mcWeight, schedMtu);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}

/*******************************************************************************
* cpssDxChIpMultiTargetUcSchedModeGet
*
* DESCRIPTION:
*      get the Multi target/unicast sheduler mode.
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; xCat3; Lion; Lion2; Lion3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; xCat2.
*
* INPUTS:
*       devNum    - the device number
*
* OUTPUTS:
*       ucSPEnablePtr- weather the Unicast uses SP , if GT_FALSE then it means
*                      both the Unicast and multi target uses DSWRR scheduling
*       ucWeightPtr  - if DSWRR scheduler is used (ucSPEnable==GT_FALSE) then this
*                      is the unicast weight (APPLICABLE RANGES: 0..255).
*       mcWeightPtr  - if DSWRR scheduler is used (ucSPEnable==GT_FALSE) then this
*                      is the multi target weight (APPLICABLE RANGES: 0..255).
*       schedMtuPtr  - The MTU used by the scheduler
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - Illegal parameter in function called
*       GT_FAIL                  - on error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_BAD_PTR               - on NULL ptr
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChIpMultiTargetUcSchedModeGet

(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                                   result;

    GT_U8                                       devNum;
    GT_BOOL                                     ucSPEnable;
    GT_U32                                      ucWeight;
    GT_U32                                      mcWeight;
    CPSS_DXCH_IP_MT_UC_SCHED_MTU_ENT            schedMtu;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];

    /* call cpss api function */
    result = cpssDxChIpMultiTargetUcSchedModeGet(devNum, &ucSPEnable, &ucWeight,
                                                          &mcWeight, &schedMtu);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d, %d, %d, %d", ucSPEnable, ucWeight,
                                                            mcWeight, schedMtu);
    return CMD_OK;
}

/*******************************************************************************
* cpssDxChIpArpBcModeSet
*
* DESCRIPTION:
*       set a arp broadcast mode.
*
* APPLICABLE DEVICES:
*       All DxCh2 devices.
*
* INPUTS:
*       devNum       - physical device number
*       arpBcMode    - the arp broadcast command. Possible Commands:
*                      CPSS_PACKET_CMD_NONE_E,CPSS_PACKET_CMD_TRAP_TO_CPU_E,
*                      CPSS_PACKET_CMD_MIRROR_TO_CPU_E
*
* OUTPUTS:
*       none
*
* RETURNS:
*       GT_OK   - on success
*       GT_NOT_INITIALIZED - The library was not initialized.
*       GT_FAIL - on error
*
* COMMENTS:
*       none.
* GalTis:
*
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChIpArpBcModeSet

(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                                   result;

    GT_U8                                       devNum;
    CPSS_PACKET_CMD_ENT                         arpBcMode;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    arpBcMode = (CPSS_PACKET_CMD_ENT)inArgs[1];

    /* call cpss api function */
    result = cpssDxChIpArpBcModeSet(devNum, arpBcMode);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}

/*******************************************************************************
* cpssDxChIpArpBcModeGet
*
* DESCRIPTION:
*       get a arp broadcast mode.
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; xCat3; Lion; Lion2; Lion3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; xCat2.
*
* INPUTS:
*       devNum       - the device number
*
* OUTPUTS:
*       arpBcModePtr - the arp broadcast command. Possible Commands:
*                      CPSS_PACKET_CMD_NONE_E,CPSS_PACKET_CMD_TRAP_TO_CPU_E,
*                      CPSS_PACKET_CMD_MIRROR_TO_CPU_E
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - Illegal parameter in function called
*       GT_NOT_INITIALIZED       - The library was not initialized.
*       GT_FAIL                  - on error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_BAD_PTR               - on NULL pointer
*
* COMMENTS:
*       None.
* GalTis:
*
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChIpArpBcModeGet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8                       devNum;
    CPSS_PACKET_CMD_ENT         arpBcMode;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];

    /* call cpss api function */
    result = cpssDxChIpArpBcModeGet(devNum, &arpBcMode);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", arpBcMode);
    return CMD_OK;
}

/*******************************************************************************
* cpssDxChIpPortRoutingEnable
*
* DESCRIPTION:
*       Enable multicast/unicast IPv4/v6 routing on a port.
*
* APPLICABLE DEVICES:
*       All DxCh2 devices.
*
* INPUTS:
*       devNum        - physical device number
*       portNum       - the port to enable on
*       ucMcEnable    - routing type to enable Unicast/Multicast
*       protocolStack - what type of traffic to enable ipv4 or ipv6 or both.
*       enableRouting - enable IP routing for this port
*
* OUTPUTS:
*       none
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on error
*
* COMMENTS:
*       none
*
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChIpPortRoutingEnable

(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                                       result;

    GT_U8                                           devNum;
    GT_PORT_NUM                                           portNum;
    CPSS_IP_UNICAST_MULTICAST_ENT                   ucMcEnable;
    CPSS_IP_PROTOCOL_STACK_ENT                      protocolStack;
    GT_BOOL                                         enableRouting;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    portNum = (GT_PORT_NUM)inArgs[1];
    ucMcEnable = (CPSS_IP_UNICAST_MULTICAST_ENT)inArgs[2];
    protocolStack = (CPSS_IP_PROTOCOL_STACK_ENT)inArgs[3];
    enableRouting = (GT_BOOL)inArgs[4];

    /* Override Device and Port */
    CONVERT_DEV_PORT_U32_MAC(devNum, portNum);

    /* call cpss api function */
    result = cpssDxChIpPortRoutingEnable(devNum, portNum, ucMcEnable,
                                        protocolStack, enableRouting);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}

/*******************************************************************************
* cpssDxChIpPortRoutingEnableGet
*
* DESCRIPTION:
*       Get status of multicast/unicast IPv4/v6 routing on a port.
*
* APPLICABLE DEVICES: DxCh2 and above.
*
* INPUTS:
*       devNum        - the device number
*       portNum       - the port to enable on
*       ucMcEnable    - routing type to enable Unicast/Multicast
*       protocolStack - what type of traffic to enable ipv4 or ipv6 or both.
*
* OUTPUTS:
*       enableRoutingPtr - (pointer to)enable IP routing for this port
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_BAD_PARAM             - wrong protocolStack
*
* COMMENTS:
*       None
*
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChIpPortRoutingEnableGet

(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                                       result;

    GT_U8                                           devNum;
    GT_PORT_NUM                                           portNum;
    CPSS_IP_UNICAST_MULTICAST_ENT                   ucMcEnable;
    CPSS_IP_PROTOCOL_STACK_ENT                      protocolStack;
    GT_BOOL                                         enableRouting;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    portNum = (GT_PORT_NUM)inArgs[1];
    ucMcEnable = (CPSS_IP_UNICAST_MULTICAST_ENT)inArgs[2];
    protocolStack = (CPSS_IP_PROTOCOL_STACK_ENT)inArgs[3];

    /* Override Device and Port */
    CONVERT_DEV_PORT_U32_MAC(devNum, portNum);

    /* call cpss api function */
    result = cpssDxChIpPortRoutingEnableGet(devNum, portNum, ucMcEnable,
                                        protocolStack, &enableRouting);

    if (result != GT_OK)
    {
        galtisOutput(outArgs, result, "%d", -1);
        return CMD_OK;
    }
    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", enableRouting);
    return CMD_OK;
}

/*******************************************************************************
* cpssDxChIpQosProfileToRouteEntryMapSet
*
* DESCRIPTION:
*       Sets the QoS profile to route entry offset mapping table.
*
* APPLICABLE DEVICES:
*       All DxCh2 devices.
*
* INPUTS:
*       devNum           - physical device number
*       qosProfile       - QOS Profile index
*       routeEntryOffset - The offset in the array of route entries to be
*                          selected for this QOS profile
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK   - on success
*       GT_NOT_INITIALIZED - The library was not initialized.
*       GT_FAIL - on error
*
* COMMENTS:
*       none
*
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChIpQosProfileToRouteEntryMapSet

(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                                   result;

    GT_U8                                       devNum;
    GT_U32                                      qosProfile;
    GT_U32                                      routeEntryOffset;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    qosProfile = (GT_U32)inArgs[1];
    routeEntryOffset = (GT_U32)inArgs[2];

    /* call cpss api function */
    result = cpssDxChIpQosProfileToRouteEntryMapSet(devNum, qosProfile,
                                                      routeEntryOffset);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}

/*******************************************************************************
* cpssDxChIpQosProfileToRouteEntryMapGet
*
* DESCRIPTION:
*       gets the QoS profile to route entry offset mapping table.
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; xCat3; Lion; Lion2; Lion3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; xCat2.
*
* INPUTS:
*       devNum           - the device number
*       qosProfile       - QOS Profile index
*
* OUTPUTS:
*       routeEntryOffsetPtr - The offset in the array of route entries to be
*                             selected for this QOS profile
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - Illegal parameter in function called
*       GT_NOT_INITIALIZED       - The library was not initialized.
*       GT_FAIL                  - on error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_BAD_PTR               - on NULL ptr
*
* COMMENTS:
*       In QoS-based routing, the Route table entry is selected according to the
*       following index calculation:
*       1. in DxCh2, DxCh3, xCat: <Route Entry Index> +
*                               (QoSProfile-to-Route-Block-Offset(QoSProfile) %
*                               (<Number of Paths> + 1)
*       2. in Lion : <Route Entry Index> +
*                    Floor(QoSProfile-to-Route-Block-Offset(QoSProfile) *
*                         (<Number of Paths> + 1) / 8)
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChIpQosProfileToRouteEntryMapGet

(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                                   result;

    GT_U8                                       devNum;
    GT_U32                                      qosProfile;
    GT_U32                                      routeEntryOffset;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    qosProfile = (GT_U32)inArgs[1];

    /* call cpss api function */
    result = cpssDxChIpQosProfileToRouteEntryMapGet(devNum, qosProfile,
                                                      &routeEntryOffset);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", routeEntryOffset);
    return CMD_OK;
}

/*******************************************************************************
* cpssDxChIpRoutingEnable
*
* DESCRIPTION:
*       globally enable/disable routing.
*
* APPLICABLE DEVICES: DxCh2 and above.
*
* INPUTS:
*       devNum        - the device number
*       enableRouting - enable /disable global routing
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_BAD_PARAM             - wrong devNum
*
* COMMENTS:
*       the ASIC defualt is routing enabled.
*
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChIpRoutingEnable

(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS               result;

    GT_U8                   devNum;
    GT_BOOL                 enableRouting;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    enableRouting = (GT_BOOL)inArgs[1];

    /* call cpss api function */
    result = cpssDxChIpRoutingEnable(devNum, enableRouting);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}

/*******************************************************************************
* cpssDxChIpRoutingEnableGet
*
* DESCRIPTION:
*       Get global routing status
*
* APPLICABLE DEVICES: DxCh2 and above.
*
* INPUTS:
*       devNum              - the device number
*
* OUTPUTS:
*       enableRoutingPtr    -  (pointer to)enable /disable global routing
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_BAD_PARAM             - wrong devNum
*       GT_BAD_PTR               - on NULL pointer
*
* COMMENTS:
*       None
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChIpRoutingEnableGet

(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS               result;

    GT_U8                   devNum;
    GT_BOOL                 enableRouting;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];

    /* call cpss api function */
    result = cpssDxChIpRoutingEnableGet(devNum, &enableRouting);

    if (result != GT_OK)
    {
        galtisOutput(outArgs, result, "%d", -1);
        return CMD_OK;
    }
    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", enableRouting);
    return CMD_OK;
}


/*  table cpssDxChIpCnt global variable   */

static CPSS_IP_CNT_SET_ENT    cntSetCnt;

/*
* cpssDxChIpCntGet
*
* DESCRIPTION:
*       Return the IP counter set requested.
*
* APPLICABLE DEVICES:
*       All DxCh2 devices.
*
* INPUTS:
*       cntSet - counters set to retrieve.
*
* OUTPUTS:
*       counters - struct contains the counter values.
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on error
*       GT_NOT_INITIALIZED - The library was not initialized.
*
* COMMENTS:
*       This function doesn't handle counters overflow.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChIpCntGetFirst

(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                      result;

    GT_U8                          devNum;
    CPSS_DXCH_IP_COUNTER_SET_STC   counters;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    cntSetCnt = 0;

    /* call cpss api function */
    result = pg_wrap_cpssDxChIpPortGroupCntGet(devNum, cntSetCnt, &counters);

    if (result != GT_OK)
    {
        galtisOutput(outArgs, result, "%d", -1);
        return CMD_OK;
    }

    inFields[0] = cntSetCnt;
    inFields[1] = counters.inUcPkts;
    inFields[2] = counters.inMcPkts;
    inFields[3] = counters.inUcNonRoutedExcpPkts;
    inFields[4] = counters.inUcNonRoutedNonExcpPkts;
    inFields[5] = counters.inMcNonRoutedExcpPkts;
    inFields[6] = counters.inMcNonRoutedNonExcpPkts;
    inFields[7] = counters.inUcTrappedMirrorPkts;
    inFields[8] = counters.inMcTrappedMirrorPkts;
    inFields[9] = counters.mcRfpFailPkts;
    inFields[10] = counters.outUcRoutedPkts;

    /* pack and output table fields */
    fieldOutput("%d%d%d%d%d%d%d%d%d%d%d", inFields[0],  inFields[1],
                  inFields[2], inFields[3], inFields[4],  inFields[5],
                  inFields[6], inFields[7], inFields[8],  inFields[9],
                  inFields[10]);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%f");

    cntSetCnt++;

    return CMD_OK;
}

/******************************************************************************/
static CMD_STATUS wrCpssDxChIpCntGetNext

(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                      result;

    GT_U8                          devNum;
    CPSS_DXCH_IP_COUNTER_SET_STC   counters;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    if (cntSetCnt > 3)
    {
        galtisOutput(outArgs, GT_OK, "%d", -1);
        return CMD_OK;
    }

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];

    /* call cpss api function */
    result = pg_wrap_cpssDxChIpPortGroupCntGet(devNum, cntSetCnt, &counters);

    if (result != GT_OK)
    {
        galtisOutput(outArgs, result, "%d", -1);
        return CMD_OK;
    }

    inFields[0] = cntSetCnt;
    inFields[1] = counters.inUcPkts;
    inFields[2] = counters.inMcPkts;
    inFields[3] = counters.inUcNonRoutedExcpPkts;
    inFields[4] = counters.inUcNonRoutedNonExcpPkts;
    inFields[5] = counters.inMcNonRoutedExcpPkts;
    inFields[6] = counters.inMcNonRoutedNonExcpPkts;
    inFields[7] = counters.inUcTrappedMirrorPkts;
    inFields[8] = counters.inMcTrappedMirrorPkts;
    inFields[9] = counters.mcRfpFailPkts;
    inFields[10] = counters.outUcRoutedPkts;

    /* pack and output table fields */
    fieldOutput("%d%d%d%d%d%d%d%d%d%d%d", inFields[0],  inFields[1],
                  inFields[2], inFields[3], inFields[4],  inFields[5],
                  inFields[6], inFields[7], inFields[8],  inFields[9],
                  inFields[10]);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%f");

    cntSetCnt++;

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChIpCntSetModeSet
*
* DESCRIPTION:
*      Sets a counter set's bounded inteface and interface mode.
*
* APPLICABLE DEVICES:
*       All DxCh2 devices.
*
* INPUTS:
*       devNum        - the device number
*       cntSet        - the counter set
*       cntSetMode    - the counter set bind mode (interface or Next hop)
*       interfaceModeCfgPtr - if cntSetMode = CPSS_DXCH_IP_CNT_SET_INTERFACE_MODE_E
*                          this is the interface configuration
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChIpCntSetModeSet

(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                                     result;

    GT_U8                                         devNum;
    CPSS_IP_CNT_SET_ENT                           cntSet;
    CPSS_DXCH_IP_CNT_SET_MODE_ENT                 cntSetMode;
    CPSS_DXCH_IP_COUNTER_SET_INTERFACE_CFG_STC    interfaceModeCfgPtr = {0};
    GT_HW_DEV_NUM                                 tmpHwDevNum;
    GT_PORT_NUM                                   tmpPortNum;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];

    cntSet = (CPSS_INTERFACE_TYPE_ENT)inFields[0];
    cntSetMode = (CPSS_INTERFACE_TYPE_ENT)inFields[1];

    interfaceModeCfgPtr.portTrunkCntMode =
                             (CPSS_DXCH_IP_PORT_TRUNK_CNT_MODE_ENT)inFields[2];
    interfaceModeCfgPtr.ipMode = (CPSS_IP_PROTOCOL_STACK_ENT)inFields[3];
    interfaceModeCfgPtr.vlanMode = (CPSS_DXCH_IP_VLAN_CNT_MODE_ENT)inFields[4];

    switch(inFields[2])
    {
    case 1:
        tmpHwDevNum = (GT_HW_DEV_NUM)inFields[5];
        tmpPortNum  = (GT_PORT_NUM)inFields[6];
        CONVERT_DEV_PORT_DATA_MAC(tmpHwDevNum,tmpPortNum);
        interfaceModeCfgPtr.hwDevNum = tmpHwDevNum;
        interfaceModeCfgPtr.portTrunk.port = tmpPortNum;
        break;

    case 2:
        interfaceModeCfgPtr.portTrunk.trunk = (GT_TRUNK_ID)inFields[7];
        CONVERT_TRUNK_ID_TEST_TO_CPSS_MAC(interfaceModeCfgPtr.portTrunk.trunk);
        break;

    default:
        break;
    }

    if(inFields[4])
        interfaceModeCfgPtr.vlanId = (GT_U16)inFields[8];

    /* call cpss api function */
    result = cpssDxChIpCntSetModeSet(devNum, cntSet, cntSetMode,
                                                      &interfaceModeCfgPtr);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}

/*******************************************************************************
* cpssDxChIpCntSetModeGet
*
* DESCRIPTION:
*      Gets a counter set's bounded inteface and interface mode.
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; xCat3; Lion; Lion2; Lion3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; xCat2.
*
* INPUTS:
*       devNum        - the device number
*       cntSet        - the counter set
*       cntSetMode    - the counter set bind mode (interface or Next hop)
*
* OUTPUTS:
*       interfaceModeCfgPtr - if cntSetMode = CPSS_DXCH_IP_CNT_SET_INTERFACE_MODE_E
*                             this is the interface configuration
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - Illegal parameter in function called
*       GT_FAIL                  - on error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_BAD_PTR               - on NULL ptr
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChIpCntSetModeGet

(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                                     result;

    GT_U8                                         devNum;
      CPSS_DXCH_IP_CNT_SET_MODE_ENT                 cntSetMode =
                                          CPSS_DXCH_IP_CNT_SET_INTERFACE_MODE_E;
    CPSS_DXCH_IP_COUNTER_SET_INTERFACE_CFG_STC    interfaceModeCfgPtr = {0};
    GT_HW_DEV_NUM                                 tmpHwDevNum;
    GT_PORT_NUM                                   tmpPortNum;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    if (cntSetCnt ==  CPSS_IP_CNT_NO_SET_E)
    {
        galtisOutput(outArgs, GT_OK, "%d", -1);
        return CMD_OK;
    }

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];

    inFields[0] = cntSetCnt ;


    /* call cpss api function */
    result = cpssDxChIpCntSetModeGet(devNum, cntSetCnt, &cntSetMode,
                                                      &interfaceModeCfgPtr);

    inFields[1] = cntSetMode;
    inFields[2] = interfaceModeCfgPtr.portTrunkCntMode;
    inFields[3] = interfaceModeCfgPtr.ipMode;
    inFields[4] = interfaceModeCfgPtr.vlanMode;

    switch(inFields[2])
    {
    case 1:
        tmpHwDevNum = interfaceModeCfgPtr.hwDevNum;
        tmpPortNum = interfaceModeCfgPtr.portTrunk.port;
        CONVERT_BACK_DEV_PORT_DATA_MAC(tmpHwDevNum,tmpPortNum);
        inFields[5] = tmpHwDevNum;
        inFields[6] = tmpPortNum;
        break;

    case 2:
        CONVERT_TRUNK_ID_CPSS_TO_TEST_MAC(interfaceModeCfgPtr.portTrunk.trunk);
        inFields[7] = interfaceModeCfgPtr.portTrunk.trunk;
        break;

    default:
        break;
    }

    if(inFields[4])
        inFields[8] = interfaceModeCfgPtr.vlanId;


    /* pack and output table fields */
    fieldOutput("%d%d%d%d%d%d%d%d%d", inFields[0], inFields[1], inFields[2],
        inFields[3], inFields[4], inFields[5], inFields[6], inFields[7], inFields[8]);

    cntSetCnt++;

    galtisOutput(outArgs, result, "%f");
    return CMD_OK;
}


/*******************************************************************************
* cpssDxChIpCntSetModeGet
*
* DESCRIPTION:
*      Gets a counter set's bounded inteface and interface mode.
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; xCat3; Lion; Lion2; Lion3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; xCat2.
*
* INPUTS:
*       devNum        - the device number
*       cntSet        - the counter set
*       cntSetMode    - the counter set bind mode (interface or Next hop)
*
* OUTPUTS:
*       interfaceModeCfgPtr - if cntSetMode = CPSS_DXCH_IP_CNT_SET_INTERFACE_MODE_E
*                             this is the interface configuration
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - Illegal parameter in function called
*       GT_FAIL                  - on error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_BAD_PTR               - on NULL ptr
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChIpCntSetModeGetFirst

(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
            cntSetCnt = 0;
            return wrCpssDxChIpCntSetModeGet(inArgs,inFields,numFields,outArgs);
}

/*******************************************************************************
* cpssDxChIpCntSetModeGet
*
* DESCRIPTION:
*      Gets a counter set's bounded inteface and interface mode.
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; xCat3; Lion; Lion2; Lion3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; xCat2.
*
* INPUTS:
*       devNum        - the device number
*       cntSet        - the counter set
*       cntSetMode    - the counter set bind mode (interface or Next hop)
*
* OUTPUTS:
*       interfaceModeCfgPtr - if cntSetMode = CPSS_DXCH_IP_CNT_SET_INTERFACE_MODE_E
*                             this is the interface configuration
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - Illegal parameter in function called
*       GT_FAIL                  - on error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_BAD_PTR               - on NULL ptr
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChIpCntSetModeGetNext

(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
           return  wrCpssDxChIpCntSetModeGet(inArgs,inFields,numFields,outArgs);
}

/*******************************************************************************
* cpssDxChIpCntSet
*
* DESCRIPTION:
*       set the requested IP counter set.
*
* APPLICABLE DEVICES:
*       All DxCh2 devices.
*
* INPUTS:
*       devNum    - physical device number
*       cntSet    - counters set to clear.
*       counters  - the counter values to set.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChIpCntSet

(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                           result;

    GT_U8                               devNum;
    CPSS_IP_CNT_SET_ENT                 cntSet;
    CPSS_DXCH_IP_COUNTER_SET_STC        counters;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];

    cntSet = (CPSS_IP_CNT_SET_ENT)inFields[0];

    counters.inUcPkts = (GT_U32)inFields[1];
    counters.inMcPkts = (GT_U32)inFields[2];
    counters.inUcNonRoutedExcpPkts = (GT_U32)inFields[3];
    counters.inUcNonRoutedNonExcpPkts = (GT_U32)inFields[4];
    counters.inMcNonRoutedExcpPkts = (GT_U32)inFields[5];
    counters.inMcNonRoutedNonExcpPkts = (GT_U32)inFields[6];
    counters.inUcTrappedMirrorPkts = (GT_U32)inFields[7];
    counters.inMcTrappedMirrorPkts = (GT_U32)inFields[8];
    counters.mcRfpFailPkts = (GT_U32)inFields[9];
    counters.outUcRoutedPkts = (GT_U32)inFields[10];

    /* call cpss api function */
    result = pg_wrap_cpssDxChIpPortGroupCntSet(devNum, cntSet, &counters);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}

/*******************************************************************************
* cpssDxChIpMllSkippedEntriesCountersGet
*
* DESCRIPTION:
*       Get MLL entries skipped counter.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum      - device number
*
* OUTPUTS:
*       skipCounterPtr   - (pointer to) number of MLL entries skipped.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - on wrong input parameters
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChIpMllSkippedEntriesCountersGet

(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                                   result;

    GT_U8                                       devNum;
    GT_U32                                      skipCounter;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];

    /* call cpss api function */
    result = pg_wrap_cpssDxChIpMllSkippedEntriesCountersGet(devNum, &skipCounter);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", skipCounter);
    return CMD_OK;
}

/*******************************************************************************
* cpssDxChIpSetMllCntInterface
*
* DESCRIPTION:
*      Sets a mll counter set's bounded inteface.
*
* APPLICABLE DEVICES:
*       All DxCh2 devices.
*
* INPUTS:
*       devNum          - physical device number.
*       mllCntSet       - mll counter set
*                         (APPLICABLE RANGES: 0..1)
*       interfaceCfgPtr - the mll counter interface configuration
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChIpSetMllCntInterface

(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                                     result;

    GT_U8                                         devNum;
    GT_U32                                        mllCntSet;
    CPSS_DXCH_IP_COUNTER_SET_INTERFACE_CFG_STC    interfaceCfgPtr;

    GT_HW_DEV_NUM                                 tmpHwDevNum;
    GT_PORT_NUM                                   tmpPortNum;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    cpssOsMemSet(&interfaceCfgPtr,0,sizeof(CPSS_DXCH_IP_COUNTER_SET_INTERFACE_CFG_STC));

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];

    mllCntSet = (GT_U32)inFields[0];

    interfaceCfgPtr.portTrunkCntMode =
                             (CPSS_DXCH_IP_PORT_TRUNK_CNT_MODE_ENT)inFields[1];
    interfaceCfgPtr.ipMode = (CPSS_IP_PROTOCOL_STACK_ENT)inFields[2];
    interfaceCfgPtr.vlanMode = (CPSS_DXCH_IP_VLAN_CNT_MODE_ENT)inFields[3];

    switch(inFields[1])
    {
    case 1:
        /* check if need to update CPSS_DXCH_IP_COUNTER_SET_INTERFACE_CFG_STC to
           support intergace of type: GT_HW_DEV_NUM, GT_PORT_NUM
           and fix the code accordingly to support more devs and ports*/
        CPSS_TBD_BOOKMARK_EARCH

        tmpHwDevNum = (GT_HW_DEV_NUM)inFields[4];
        tmpPortNum  = (GT_PORT_NUM)inFields[5];
        CONVERT_DEV_PORT_DATA_MAC(tmpHwDevNum,tmpPortNum);

        interfaceCfgPtr.hwDevNum = tmpHwDevNum;
        interfaceCfgPtr.portTrunk.port = tmpPortNum;

        break;

    case 2:
        interfaceCfgPtr.portTrunk.trunk = (GT_TRUNK_ID)inFields[6];
        CONVERT_TRUNK_ID_TEST_TO_CPSS_MAC(interfaceCfgPtr.portTrunk.trunk);
        break;

    default:
        break;
    }

    if(inFields[3])
        interfaceCfgPtr.vlanId = (GT_U16)inFields[7];

    /* call cpss api function */
    result = cpssDxChIpSetMllCntInterface(devNum, mllCntSet,
                                                  &interfaceCfgPtr);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}

/*******************************************************************************
* cpssDxChIpMllCntGet
*
* DESCRIPTION:
*      Get the mll counter.
*
* APPLICABLE DEVICES:
*       All DxCh2 devices.
*
* INPUTS:
*       devNum     - physical device number.
*       mllCntSet  - the mll counter set out of the 2
*
* OUTPUTS:
*       mllOutMCPktsPtr - According to the configuration of this cnt set, The
*                      number of routed IP Multicast packets Duplicated by the
*                      MLL Engine and transmitted via this interface
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChIpMllCntGet

(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                                   result;

    GT_U8                                       devNum;
    GT_U32                                      mllCntSet;
    GT_U32                                      mllOutMCPktsPtr;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    mllCntSet = (GT_U32)inArgs[1];

    /* call cpss api function */
    result = pg_wrap_cpssDxChIpMllCntGet(devNum, mllCntSet, &mllOutMCPktsPtr);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", mllOutMCPktsPtr);
    return CMD_OK;
}

/*******************************************************************************
* cpssDxChIpMllCntSet
*
* DESCRIPTION:
*      set an mll counter.
*
* APPLICABLE DEVICES:
*       All DxCh2 devices.
*
* INPUTS:
*       devNum          - physical device number.
*       mllCntSet       - the mll counter set out of the 2
*       mllOutMCPkts    - the counter value to set
*
* OUTPUTS:
*      none.
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChIpMllCntSet

(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                                   result;

    GT_U8                                       devNum;
    GT_U32                                      mllCntSet;
    GT_U32                                      mllOutMCPkts;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    mllCntSet = (GT_U32)inArgs[1];
    mllOutMCPkts = (GT_U32)inArgs[2];

    /* call cpss api function */
    result = pg_wrap_cpssDxChIpPortGroupMllCntSet(devNum, mllCntSet,
                                                  mllOutMCPkts);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}


/*******************************************************************************
* cpssDxChIpMllSilentDropCntGet
*
* DESCRIPTION:
*      Get the silent drops in the MLL priority queues.
*      A silent drop is a drop that is applied to a replica of the packet that
*      was previously replicated in the TTI.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum    - the device number
*
* OUTPUTS:
*       silentDropPktsPtr - (pointer to) the number of counted silent dropped packets.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChIpMllSilentDropCntGet

(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                                   result;

    GT_U8                                       devNum;
    GT_U32                                      silentDropPkts;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];

    /* call cpss api function */
    result = pg_wrap_cpssDxChIpMllSilentDropCntGet(devNum, &silentDropPkts);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", silentDropPkts);
    return CMD_OK;
}

/*******************************************************************************
* cpssDxChIpDropCntSet
*
* DESCRIPTION:
*      set the drop counter.
*
* APPLICABLE DEVICES:
*       All DxCh2 devices.
*
* INPUTS:
*       devNum      - physical device number.
*       dropPkts    - the counter value to set
*
* OUTPUTS:
*       none.
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChIpDropCntSet

(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                                   result;

    GT_U8                                       devNum;
    GT_U32                                      dropPkts;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    dropPkts = (GT_U32)inArgs[1];

    /* call cpss api function */
    result = pg_wrap_cpssDxChIpDropCntSet(devNum, dropPkts);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}

/*******************************************************************************
* cpssDxChIpRouterBridgedPacketsExceptionCntGet
*
* DESCRIPTION:
*      Get exception counter for Special Services for Bridged Traffic.
*      it counts the number of Bridged packets failing any of the following checks:
*        - SIP Filter check for bridged IPv4/6 packets
*        - IP Header Check for bridged IPv4/6 packets
*        - Unicast RPF check for bridged IPv4/6 and ARP packets
*        - Unicast SIP/SA check for bridged IPv4/6 and ARP packets
*
* APPLICABLE DEVICES: DxCh2 and above.
*
* INPUTS:
*       devNum      - the device number
*
* OUTPUTS:
*       routerBridgedExceptionPktsPtr - the number of counted router bridged
*                                       exception  packets.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PTR               - on illegal pointer value
*       GT_FAIL                  - on error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChIpRouterBridgedPacketsExceptionCntGet

(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                                   result;

    GT_U8                                       devNum;
    GT_U32                                      routerBridgedExceptionPkts;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];

    /* call cpss api function */
    result = pg_wrap_cpssDxChIpRouterBridgedPacketsExceptionCntGet(devNum,
                                                                   &routerBridgedExceptionPkts);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", routerBridgedExceptionPkts);
    return CMD_OK;
}


/*******************************************************************************
* wrCpssDxChIpRouterBridgedPacketsExceptionCntSet
*
* DESCRIPTION:
*      Set exception counter for Special Services for Bridged Traffic.
*      it counts the number of Bridged packets failing any of the following checks:
*        - SIP Filter check for bridged IPv4/6 packets
*        - IP Header Check for bridged IPv4/6 packets
*        - Unicast RPF check for bridged IPv4/6 and ARP packets
*        - Unicast SIP/SA check for bridged IPv4/6 and ARP packets
*
* APPLICABLE DEVICES: DxCh2 and above.
*
* INPUTS:
*       devNum                        - the device number
*       routerBridgedExceptionPkts    - the counter value to set
*
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChIpRouterBridgedPacketsExceptionCntSet

(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                                   result;

    GT_U8                                       devNum;
    GT_U32                                      routerBridgedExceptionPkts;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    routerBridgedExceptionPkts = (GT_U32)inArgs[1];

    /* call cpss api function */
    result = pg_wrap_cpssDxChIpRouterBridgedPacketsExceptionCntSet(devNum,
                                                                   routerBridgedExceptionPkts);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}

/*******************************************************************************
* cpssDxChIpSetDropCntMode
*
* DESCRIPTION:
*      Sets the drop counter count mode.
*
* APPLICABLE DEVICES:
*       All DxCh2 devices.
*
* INPUTS:
*       devNum      - physical device number.
*       dropCntMode - the drop counter count mode
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChIpSetDropCntMode

(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                                   result;

    GT_U8                                       devNum;
    CPSS_DXCH_IP_DROP_CNT_MODE_ENT              dropCntMode;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    dropCntMode = (CPSS_DXCH_IP_DROP_CNT_MODE_ENT)inArgs[1];

    /* call cpss api function */
    result = cpssDxChIpSetDropCntMode(devNum, dropCntMode);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}

/*******************************************************************************
* cpssDxChIpDropCntGet
*
* DESCRIPTION:
*      Get the drop counter.
*
* APPLICABLE DEVICES:
*       All DxCh2 devices.
*
* INPUTS:
*       devNum    - physical device number.
*
* OUTPUTS:
*       dropPktsPtr - the number of counted dropped packets according to the
*                     drop counter mode.
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChIpDropCntGet

(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                                   result;

    GT_U8                                       devNum;
    GT_U32                                      dropPktsPtr;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];

    /* call cpss api function */
    result = pg_wrap_cpssDxChIpDropCntGet(devNum, &dropPktsPtr);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", dropPktsPtr);
    return CMD_OK;
}

/*******************************************************************************
* cpssDxChIpMtuProfileSet
*
* DESCRIPTION:
*       Sets the next hop interface MTU profile limit value.
*
* APPLICABLE DEVICES:
*       All DxCh2 devices.
*
* INPUTS:
*       devNum          - physical device number.
*       mtuProfileIndex - the mtu profile index out of the possible 8 (0..7).
*       mtu             - the maximum transmission unit
*
* OUTPUTS:
*       none.
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on error
*
* COMMENTS:
*       None.
*
*******************************************************************************/

static CMD_STATUS wrCpssDxChIpMtuProfileSet

(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                                   result;

    GT_U8                                       devNum;
    GT_U32                                      mtuProfileIndex;
    GT_U32                                      mtu;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    mtuProfileIndex = (GT_U32)inArgs[1];
    mtu = (GT_U32)inArgs[2];

    /* call cpss api function */
    result = cpssDxChIpMtuProfileSet(devNum, mtuProfileIndex, mtu);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}
/*******************************************************************************
* cpssDxChIpMtuProfileGet
*
* DESCRIPTION:
*       Gets the next hop interface MTU profile limit value.
*
* APPLICABLE DEVICES:
*       DxCh2; DxCh3; xCat; xCat3; Lion; Lion2; Lion3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; xCat2.
*
* INPUTS:
*       devNum          - the device number
*       mtuProfileIndex - the mtu profile index out of the possible 8
                            (APPLICABLE RANGES - 0..8).
*
* OUTPUTS:
*       mtuPtr             - the maximum transmission unit
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_BAD_PTR               - on NULL pointer
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChIpMtuProfileGet

(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                                   result;

    GT_U8                                       devNum;
    GT_U32                                      mtuProfileIndex;
    GT_U32                                      mtuPtr;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    mtuProfileIndex = (GT_U32)inArgs[1];

    /* call cpss api function */
    result = cpssDxChIpMtuProfileGet(devNum, mtuProfileIndex, &mtuPtr);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", mtuPtr);
    return CMD_OK;
}

/*******************************************************************************
* cpssDxChIpv6AddrPrefixScopeSet
*
* DESCRIPTION:
*       Defines a prefix of a scope type.
*
* APPLICABLE DEVICES:
*       All DxCh2 devices.
*
* INPUTS:
*       devNum           - physical device number
*       prefix           - an IPv6 address prefix
*       prefixLen        - length of the prefix (0 - 16)
*       addressScope     - type of the address scope spanned by the prefix
*       prefixScopeIndex - index of the new prefix scope entry (0..4)
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK              - on success.
*       GT_FAIL            - on error.
*       GT_BAD_PARAM       - case that the prefix length is out of range.
*       GT_NOT_SUPPORTED - if working with a PP that doesn't support IPv6
*
* COMMENTS:
*       Configures an entry in the prefix look up table
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChIpv6AddrPrefixScopeSet

(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                                   result;

    GT_U8                                       devNum;
    GT_IPV6ADDR                                 prefix;
    GT_U32                                      prefixLen;
    CPSS_IPV6_PREFIX_SCOPE_ENT                  addressScope;
    GT_U32                                      prefixScopeIndex;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    galtisIpv6Addr(&prefix, (GT_U8*)inArgs[1]);
    prefixLen = (GT_U32)inArgs[2];
    addressScope = (CPSS_IPV6_PREFIX_SCOPE_ENT)inArgs[3];
    prefixScopeIndex = (GT_U32)inArgs[4];

    /* call cpss api function */
    result = cpssDxChIpv6AddrPrefixScopeSet(devNum, prefix, prefixLen,
                                       addressScope, prefixScopeIndex);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}

/*******************************************************************************
* cpssDxChIpv6AddrPrefixScopeGet
*
* DESCRIPTION:
*       Get a prefix of a scope type.
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; xCat3; Lion; Lion2; Lion3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; xCat2.
*
* INPUTS:
*       devNum           - the device number
*       prefixScopeIndex - index of the new prefix scope entry (APPLICABLE RANGES: 0..4)
*
* OUTPUTS:
*       prefixPtr        - an IPv6 address prefix
*       prefixLenPtr     - length of the prefix
*       addressScopePtr  - type of the address scope spanned by the prefix
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_FAIL                  - on error.
*       GT_BAD_PARAM             - case that the prefix length is out of range.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static  GT_U32 index;

static CMD_STATUS ipv6AddrPrefixScopeGet

(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    GT_IPV6ADDR prefix;
    GT_U32 prefixLen;
    CPSS_IPV6_PREFIX_SCOPE_ENT addressScope;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];

    /* call cpss api function */
    result = cpssDxChIpv6AddrPrefixScopeGet(devNum, &prefix, &prefixLen, &addressScope, index);


    if(result != CMD_OK)
    {
        galtisOutput(outArgs, GT_OK, "%d", -1);
        return CMD_OK;
    }

    inFields[1] = prefixLen;
    inFields[2] = addressScope;
    inFields[3] = index;

    /* pack output arguments to galtis string */
    fieldOutput("%d%16B%d%d", prefix.arIP, inFields[1],
                                inFields[2],  inFields[3]);

    galtisOutput(outArgs, result, "%f");
    return CMD_OK;
}

/******************************************************************************/
static CMD_STATUS wrCpssDxChIpv6AddrPrefixScopeGet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                   result;
    GT_U8                       devNum;
    GT_U32                      index;
    GT_IPV6ADDR                 prefix;
    GT_U32                      prefixLen;
    CPSS_IPV6_PREFIX_SCOPE_ENT  addressScope;

    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    index = (GT_U32)inArgs[1];

    /* call cpss api function */
    result = cpssDxChIpv6AddrPrefixScopeGet(devNum, &prefix, &prefixLen,
                                            &addressScope, index);

    galtisOutput(outArgs, result, "%16B%d%d%d",
                 prefix.arIP, prefixLen, addressScope, index);

    return CMD_OK;
}

/******************************************************************************/
static CMD_STATUS wrCpssDxChIpv6AddrPrefixScopeGetFirst
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    CMD_STATUS res;

    index = 0;
    res = ipv6AddrPrefixScopeGet(inArgs,inFields,numFields,outArgs);
    if (res == CMD_OK)
        index++;
    return res;
}
/******************************************************************************/
static CMD_STATUS wrCpssDxChIpv6AddrPrefixScopeGetNext
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    CMD_STATUS res;

    if(index > 3)
    {
        galtisOutput(outArgs, GT_OK, "%d", -1);
        return CMD_OK;
    }
    res = ipv6AddrPrefixScopeGet(inArgs,inFields,numFields,outArgs);
    if (res == CMD_OK)
        index++;
    return res;
}

/*******************************************************************************
* cpssDxChIpv6UcScopeCommandSet
*
* DESCRIPTION:
*       sets the ipv6 Unicast scope commands.
*
* APPLICABLE DEVICES:
*       All DxCh2 devices.
*
* INPUTS:
*       devNum           - physical device number
*       addressScopeSrc  - IPv6 address scope of source address
*       addressScopeDest - IPv6 address scope of destination address
*       borderCrossed    - GT_TRUE if source site ID is diffrent to destination
*                          site ID
*       scopeCommand     - action to be done on a packet that match the above
*                          scope configuration.
*                          possible commands:
*                           CPSS_PACKET_CMD_ROUTE_E,
*                           CPSS_PACKET_CMD_ROUTE_AND_MIRROR_E,
*                           CPSS_PACKET_CMD_TRAP_TO_CPU_E,
*                           CPSS_PACKET_CMD_DROP_SOFT_E,
*                           CPSS_PACKET_CMD_DROP_HARD_E
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK            - on success.
*       GT_FAIL          - on error.
*       GT_NOT_INITIALIZED - The library was not initialized.
*       GT_NO_RESOURCE - if failed to allocate CPU memory
*       GT_NOT_SUPPORTED - if working with a PP that doesn't support IPv6
*
* COMMENTS:
*
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChIpv6UcScopeCommandSet

(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                                   result;

    GT_U8                                       devNum;
    CPSS_IPV6_PREFIX_SCOPE_ENT                  addressScopeSrc;
    CPSS_IPV6_PREFIX_SCOPE_ENT                  addressScopeDest;
    GT_BOOL                                     borderCrossed;
    CPSS_PACKET_CMD_ENT                         scopeCommand;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    addressScopeSrc = (CPSS_IPV6_PREFIX_SCOPE_ENT)inArgs[1];
    addressScopeDest = (CPSS_IPV6_PREFIX_SCOPE_ENT)inArgs[2];
    borderCrossed = (GT_BOOL)inArgs[3];
    scopeCommand = (CPSS_PACKET_CMD_ENT)inArgs[4];

    /* call cpss api function */
    result = cpssDxChIpv6UcScopeCommandSet(devNum, addressScopeSrc,
                     addressScopeDest, borderCrossed, scopeCommand);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}

/*******************************************************************************
* cpssDxChIpv6UcScopeCommandGet
*
* DESCRIPTION:
*       gets the ipv6 Unicast scope commands.
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; xCat3; Lion; Lion2; Lion3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; xCat2.
*
* INPUTS:
*       devNum           - the device number
*       addressScopeSrc  - IPv6 address scope of source address
*       addressScopeDest - IPv6 address scope of destination address
*       borderCrossed    - GT_TRUE if source site ID is diffrent to destination
*                          site ID
* OUTPUTS:
*       scopeCommandPtr - action to be done on a packet that match the above
*                          scope configuration.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - Illegal parameter in function called
*       GT_FAIL                  - on error.
*       GT_NOT_INITIALIZED       - The library was not initialized.
*       GT_NO_RESOURCE           - if failed to allocate CPU memory
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_BAD_PTR               - on NULL ptr
*
* COMMENTS:
*
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChIpv6UcScopeCommandGet

(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                                   result;

    GT_U8                                       devNum;
    CPSS_IPV6_PREFIX_SCOPE_ENT                  addressScopeSrc;
    CPSS_IPV6_PREFIX_SCOPE_ENT                  addressScopeDest;
    GT_BOOL                                     borderCrossed;
    CPSS_PACKET_CMD_ENT                         scopeCommand;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    addressScopeSrc = (CPSS_IPV6_PREFIX_SCOPE_ENT)inArgs[1];
    addressScopeDest = (CPSS_IPV6_PREFIX_SCOPE_ENT)inArgs[2];
    borderCrossed = (GT_BOOL)inArgs[3];

    /* call cpss api function */
    result = cpssDxChIpv6UcScopeCommandGet(devNum, addressScopeSrc,
                     addressScopeDest, borderCrossed, &scopeCommand);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", scopeCommand);
    return CMD_OK;
}

/*******************************************************************************
* cpssDxChIpv6McScopeCommandSet
*
* DESCRIPTION:
*       sets the ipv6 Multicast scope commands.
*
* APPLICABLE DEVICES:
*       All DxCh2 devices.
*
* INPUTS:
*       devNum           - physical device number
*       addressScopeSrc  - IPv6 address scope of source address
*       addressScopeDest - IPv6 address scope of destination address
*       borderCrossed    - GT_TRUE if source site ID is diffrent to destination
*                          site ID
*       scopeCommand     - action to be done on a packet that match the above
*                          scope configuration.
*                          possible commands:
*                           CPSS_PACKET_CMD_ROUTE_E,
*                           CPSS_PACKET_CMD_ROUTE_AND_MIRROR_E,
*                           CPSS_PACKET_CMD_TRAP_TO_CPU_E,
*                           CPSS_PACKET_CMD_DROP_SOFT_E,
*                           CPSS_PACKET_CMD_DROP_HARD_E,
*                           CPSS_PACKET_CMD_BRIDGE_AND_MIRROR_E,
*                           CPSS_PACKET_CMD_BRIDGE_E
*       mllSelectionRule - rule for choosing MLL for IPv6 Multicast propogation
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK            - on success.
*       GT_FAIL          - on error.
*       GT_NOT_INITIALIZED - The library was not initialized.
*       GT_NO_RESOURCE - if failed to allocate CPU memory
*       GT_NOT_SUPPORTED - if working with a PP that doesn't support IPv6
*
* COMMENTS:
*       None.
*
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChIpv6McScopeCommandSet

(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                                   result;

    GT_U8                                       devNum;
    CPSS_IPV6_PREFIX_SCOPE_ENT                  addressScopeSrc;
    CPSS_IPV6_PREFIX_SCOPE_ENT                  addressScopeDest;
    GT_BOOL                                     borderCrossed;
    CPSS_PACKET_CMD_ENT                         scopeCommand;
    CPSS_IPV6_MLL_SELECTION_RULE_ENT            mllSelectionRule;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    addressScopeSrc = (CPSS_IPV6_PREFIX_SCOPE_ENT)inArgs[1];
    addressScopeDest = (CPSS_IPV6_PREFIX_SCOPE_ENT)inArgs[2];
    borderCrossed = (GT_BOOL)inArgs[3];
    scopeCommand = (CPSS_PACKET_CMD_ENT)inArgs[4];
    mllSelectionRule = (CPSS_IPV6_MLL_SELECTION_RULE_ENT)inArgs[5];

    /* call cpss api function */
    result = cpssDxChIpv6McScopeCommandSet(devNum, addressScopeSrc,
                                   addressScopeDest, borderCrossed,
                                    scopeCommand, mllSelectionRule);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}

/*******************************************************************************
* cpssDxChIpv6McScopeCommandGet
*
* DESCRIPTION:
*       gets the ipv6 Multicast scope commands.
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; xCat3; Lion; Lion2; Lion3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; xCat2.
*
* INPUTS:
*       devNum           - the device number
*       addressScopeSrc  - IPv6 address scope of source address
*       addressScopeDest - IPv6 address scope of destination address
*       borderCrossed    - GT_TRUE if source site ID is diffrent to destination
*                          site ID
* OUTPUTS:
*       scopeCommandPtr     - action to be done on a packet that match the above
*                             scope configuration.
*                             possible commands:
*                             CPSS_PACKET_CMD_ROUTE_E,
*                             CPSS_PACKET_CMD_ROUTE_AND_MIRROR_E,
*                             CPSS_PACKET_CMD_TRAP_TO_CPU_E,
*                             CPSS_PACKET_CMD_DROP_SOFT_E,
*                             CPSS_PACKET_CMD_DROP_HARD_E,
*                             CPSS_PACKET_CMD_BRIDGE_AND_MIRROR_E,
*                             CPSS_PACKET_CMD_BRIDGE_E
*       mllSelectionRulePtr - rule for choosing MLL for IPv6 Multicast propogation
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - Illegal parameter in function called
*       GT_FAIL                  - on error.
*       GT_NOT_INITIALIZED       - The library was not initialized.
*       GT_NO_RESOURCE           - if failed to allocate CPU memory
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_BAD_PTR               - on NULL ptr
*
* COMMENTS:
*       None.
*
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChIpv6McScopeCommandGet

(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                                   result;

    GT_U8                                       devNum;
    CPSS_IPV6_PREFIX_SCOPE_ENT                  addressScopeSrc;
    CPSS_IPV6_PREFIX_SCOPE_ENT                  addressScopeDest;
    GT_BOOL                                     borderCrossed;
    CPSS_PACKET_CMD_ENT                         scopeCommand;
    CPSS_IPV6_MLL_SELECTION_RULE_ENT            mllSelectionRule;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    addressScopeSrc = (CPSS_IPV6_PREFIX_SCOPE_ENT)inArgs[1];
    addressScopeDest = (CPSS_IPV6_PREFIX_SCOPE_ENT)inArgs[2];
    borderCrossed = (GT_BOOL)inArgs[3];

    /* call cpss api function */
    result = cpssDxChIpv6McScopeCommandGet(devNum, addressScopeSrc,
                                   addressScopeDest, borderCrossed,
                                    &scopeCommand, &mllSelectionRule);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d, %d",scopeCommand, mllSelectionRule);
    return CMD_OK;
}

/*******************************************************************************
* cpssDxChIpRouterMacSaBaseSet
*
* DESCRIPTION:
*       Sets 8 MSBs of Router MAC SA Base address on specified device.
*
* APPLICABLE DEVICES: All DxCh devices
*
* INPUTS:
*       dev - device number
*       macPtr - (pointer to)The system Mac address to set.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK           - on success
*       GT_BAD_PARAM    - wrong devNum
*       GT_FAIL         - on error
*       GT_BAD_PTR      - one of the parameters is NULL pointer
*
* COMMENTS:
*
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChIpRouterMacSaBaseSet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8         devNum;
    GT_ETHERADDR  macPtr;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    galtisMacAddr(&macPtr, (GT_U8*)inArgs[1]);

    /* call cpss api function */
    result = cpssDxChIpRouterMacSaBaseSet(devNum, &macPtr);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}

/*******************************************************************************
* cpssDxChIpRouterMacSaBaseGet
*
* DESCRIPTION:
*       Get 8 MSBs of Router MAC SA Base address on specified device.
*
* APPLICABLE DEVICES: All DxCh devices
*
* INPUTS:
*       dev - device number
*
* OUTPUTS:
*       macPtr - (pointer to)The system Mac address to set.
*
* RETURNS:
*       GT_OK           - on success
*       GT_BAD_PARAM    - wrong devNum
*       GT_FAIL         - on error
*       GT_BAD_PTR      - one of the parameters is NULL pointer
*
* COMMENTS:
*
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChIpRouterMacSaBaseGet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8         devNum;
    GT_ETHERADDR  macPtr;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];

    /* call cpss api function */
    result = cpssDxChIpRouterMacSaBaseGet(devNum, &macPtr);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%6b", macPtr.arEther);
    return CMD_OK;
}

/*******************************************************************************
* cpssDxChIpRouterMacSaLsbModeSet
*
* DESCRIPTION:
*       Sets the mode in which the device sets the packet's MAC SA least
*       significant bytes.
*
* APPLICABLE DEVICES: All DxCh devices
*
* INPUTS:
*       devNum          - The system unique device number
*       saLsbMode       - The MAC SA least-significant bit mode
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK            - on success
*       GT_NOT_SUPPORTED - if the feature does not supported by specified device
*       GT_BAD_PARAM     - wrong devNum/saLsbMode.
*
* COMMENTS:
*       The device 5 most significant bytes are set by cpssDxChIpRouterMacSaBaseSet().
*
*       If saLsbMode is Port Mode, the Port library  action
*       cpssDxChPortMacSaLsbSet() sets the per port least significant byte.
*
*       CPSS_MAC_SA_LSB_MODE_ENT -
*       If saLsbMode is Vlan Mode, the Vlan library  action
*       cpssDxChBrgVlanMacPerVlanSet() sets the per vlan least significant byte.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChIpRouterMacSaLsbModeSet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8                       devNum;
    CPSS_MAC_SA_LSB_MODE_ENT    saLsbMode;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    saLsbMode = (CPSS_MAC_SA_LSB_MODE_ENT)inArgs[1];

    /* call cpss api function */
    result = cpssDxChIpRouterMacSaLsbModeSet(devNum, saLsbMode);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}

/*******************************************************************************
* cpssDxChIpRouterMacSaLsbModeGet
*
* DESCRIPTION:
*       Gets the mode in which the device sets the packet's MAC SA least
*       significant bytes.
*
* APPLICABLE DEVICES: All DxCh devices
*
* INPUTS:
*       devNum          - The system unique device number
*
* OUTPUTS:
*       saLsbMode       - (pointer to) The MAC SA least-significant bit mode
*
* RETURNS:
*       GT_OK            - on success
*       GT_NOT_SUPPORTED - if the feature does not supported by specified device
*       GT_BAD_PARAM     - wrong devNum/saLsbMode.
*       GT_BAD_PTR       - one of the parameters is NULL pointer
*
* COMMENTS:
*       None;
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChIpRouterMacSaLsbModeGet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8                       devNum;
    CPSS_MAC_SA_LSB_MODE_ENT    saLsbMode;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    saLsbMode = (CPSS_MAC_SA_LSB_MODE_ENT)inArgs[1];

    /* call cpss api function */
    result = cpssDxChIpRouterMacSaLsbModeGet(devNum, &saLsbMode);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", saLsbMode);
    return CMD_OK;
}

/*******************************************************************************
* cpssDxChIpRouterPortVlanMacSaLsbSet
*
* DESCRIPTION:
*       Sets the 8 LSB Router MAC SA for this VLAN / EGGRESS PORT.
*
* APPLICABLE DEVICES:  All DxCh devices
*
* INPUTS:
*     devNum          - device number.
*     vlanPortId      - VLAN Id or Eggress Port Id
*     saMac           - The 8 bits SA mac value to be written to the SA bits of
*                       routed packet if SA alteration mode is configured to
*                       take LSB according to VLAN id / Eggress Port Id.
*
* OUTPUTS:
*     None.
*
* RETURNS:
*     GT_OK            - on success
*     GT_NOT_SUPPORTED - if the feature does not supported by specified device
*     GT_FAIL          - on error.
*     GT_HW_ERROR      - on hardware error
*     GT_BAD_PARAM     - wrong devNum or vlanId
*
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChIpRouterPortVlanMacSaLsbSet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8       devNum;
    GT_U16      vlanPortId;
    GT_U8       saMac;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    vlanPortId = (GT_U16)inArgs[1];
    saMac = (GT_U8)inArgs[2];

    /* call cpss api function */
    result = cpssDxChIpRouterPortVlanMacSaLsbSet(devNum, vlanPortId, saMac);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}

/*******************************************************************************
* cpssDxChIpRouterPortVlanMacSaLsbGet
*
* DESCRIPTION:
*       Gets the 8 LSB Router MAC SA for this VLAN / EGGRESS PORT.
*
* APPLICABLE DEVICES:  All DxCh devices
*
* INPUTS:
*     devNum          - device number.
*     vlanPortId      - VLAN Id or Eggress Port Id
*
* OUTPUTS:
*     saMac           - (pointer to) The 8 bits SA mac value written to the SA
*                       bits of routed packet if SA alteration mode is configured
*                       to take LSB according to VLAN id / Eggress Port Id.
*
* RETURNS:
*     GT_OK            - on success
*     GT_NOT_SUPPORTED - if the feature does not supported by specified device
*     GT_FAIL          - on error.
*     GT_HW_ERROR      - on hardware error
*     GT_BAD_PARAM     - wrong devNum or vlanId
*     GT_BAD_PTR       - one of the parameters is NULL pointer
*
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChIpRouterPortVlanMacSaLsbGet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8       devNum;
    GT_U16      vlanPortId;
    GT_U8       saMac;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    vlanPortId = (GT_U16)inArgs[1];

    /* call cpss api function */
    result = cpssDxChIpRouterPortVlanMacSaLsbGet(devNum, vlanPortId, &saMac);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", saMac);
    return CMD_OK;
}

/*******************************************************************************
* cpssDxChIpRouterMacSaModifyEnable
*
* DESCRIPTION:
*      Per Egress port bit Enable Routed packets MAC SA Modification
*
* APPLICABLE DEVICES: All DxCh devices
*
* INPUTS:
*       devNum   - The system unique device number
*       portNum  - The port number to set
*       enable   - GT_FALSE: MAC SA Modification of routed packets is disabled
*                  GT_TRUE: MAC SA Modification of routed packets is enabled
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK            - on success
*       GT_NOT_SUPPORTED - if the feature does not supported by specified device
*       GT_BAD_PARAM     - wrong devNum/portNum.
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChIpRouterMacSaModifyEnable
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8      devNum;
    GT_PHYSICAL_PORT_NUM      portNum;
    GT_BOOL    enable;

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
    result = cpssDxChIpRouterMacSaModifyEnable(devNum, portNum, enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}

/*******************************************************************************
* cpssDxChIpRouterMacSaModifyEnableGet
*
* DESCRIPTION:
*      Per Egress port bit Get Routed packets MAC SA Modification State
*
* APPLICABLE DEVICES: All DxCh devices
*
* INPUTS:
*       devNum   - The system unique device number
*       portNum  - The port number to set (0-31)
*
* OUTPUTS:
*       enable   - GT_FALSE: MAC SA Modification of routed packets is disabled
*                  GT_TRUE: MAC SA Modification of routed packets is enabled
*
* RETURNS:
*       GT_OK            - on success
*       GT_NOT_SUPPORTED - if the feature does not supported by specified device
*       GT_BAD_PARAM     - wrong devNum/portNum.
*       GT_BAD_PTR       - one of the parameters is NULL pointer
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChIpRouterMacSaModifyEnableGet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8      devNum;
    GT_PHYSICAL_PORT_NUM      portNum;
    GT_BOOL    enable;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    portNum = (GT_PHYSICAL_PORT_NUM)inArgs[1];

    /* Override Device and Port */
    CONVERT_DEV_PHYSICAL_PORT_MAC(devNum, portNum);

   /* call cpss api function */
    result = cpssDxChIpRouterMacSaModifyEnableGet(devNum, portNum, &enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", enable);
    return CMD_OK;
}
/*******************************************************************************
* cpssDxChIpPortRouterMacSaLsbModeSet
*
* DESCRIPTION:
*       Sets the mode, per port, in which the device sets the packet's MAC SA
*       least significant bytes.
*
* APPLICABLE DEVICES:
*       DxCh3 and above.
*
* INPUTS:
*       devNum          - the device number
*       portNum         - the port number
*       saLsbMode       - The MAC SA least-significant bit mode
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK            - on success
*       GT_NOT_SUPPORTED - if the feature does not supported by specified device
*       GT_BAD_PARAM     - wrong devNum/saLsbMode.
*
* COMMENTS:
*       The device 5 most significant bytes are set by cpssDxChIpRouterMacSaBaseSet().
*
*       If saLsbMode is Port Mode, the Port library  action
*       cpssDxChPortMacSaLsbSet() sets the per port least significant byte.
*
*       CPSS_MAC_SA_LSB_MODE_ENT -
*       If saLsbMode is Vlan Mode, the Vlan library  action
*       cpssDxChBrgVlanMacPerVlanSet() sets the per vlan least significant byte.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChIpPortRouterMacSaLsbModeSet
(
   IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
   IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
   IN  GT_32 numFields,
   OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    GT_PHYSICAL_PORT_NUM portNum;
    CPSS_MAC_SA_LSB_MODE_ENT saLsbMode;


    /* check for valid arguments */
    if (!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];
    portNum = (GT_PHYSICAL_PORT_NUM)inArgs[1];
    saLsbMode = (CPSS_MAC_SA_LSB_MODE_ENT)inArgs[2];

    /* Override Device and Port */
    CONVERT_DEV_PHYSICAL_PORT_MAC(devNum, portNum);

    /* call cpss api function */
    result = cpssDxChIpPortRouterMacSaLsbModeSet(devNum, portNum, saLsbMode);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}
/*******************************************************************************
* cpssDxChIpPortRouterMacSaLsbModeGet
*
* DESCRIPTION:
*       Gets the mode, per port, in which the device sets the packet's MAC SA least
*       significant bytes.
*
* APPLICABLE DEVICES:
*       DxCh3 and above.
*
* INPUTS:
*       devNum          - the device number
*       portNum         - the port number
*
* OUTPUTS:
*       saLsbModePtr    - (pointer to) The MAC SA least-significant bit mode
*
* RETURNS:
*       GT_OK            - on success
*       GT_NOT_SUPPORTED - if the feature does not supported by specified device
*       GT_BAD_PARAM     - wrong devNum/saLsbMode.
*       GT_BAD_PTR       - one of the parameters is NULL pointer
*
* COMMENTS:
*       None;
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChIpPortRouterMacSaLsbModeGet
(
   IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
   IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
   IN  GT_32 numFields,
   OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    GT_PHYSICAL_PORT_NUM portNum;
    CPSS_MAC_SA_LSB_MODE_ENT saLsbMode;


    /* check for valid arguments */
    if (!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];
    portNum = (GT_PHYSICAL_PORT_NUM)inArgs[1];

    /* Override Device and Port */
    CONVERT_DEV_PHYSICAL_PORT_MAC(devNum, portNum);

    /* call cpss api function */
    result = cpssDxChIpPortRouterMacSaLsbModeGet(devNum, portNum, &saLsbMode);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", saLsbMode);

    return CMD_OK;
}





/*******************************************************************************
* cpssDxChIpRouterGlobalMacSaSet
*
* DESCRIPTION:
*       Sets full 48-bit Router MAC SA in Global MAC SA table.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*     devNum            - the device number.
*     routerMacSaIndex  - The index into the global MAC SA table.
*     macSaAddrPtr      - The 48 bits MAC SA.
*
* OUTPUTS:
*     None.
*
* RETURNS:
*     GT_OK                    - on success.
*     GT_FAIL                  - on error.
*     GT_HW_ERROR              - on hardware error.
*     GT_BAD_PARAM             - wrong devNum or routerMacSaIndex.
*     GT_NOT_APPLICABLE_DEVICE - on not applicable device.
*
*
* COMMENTS:
*      This API is applicable when per-egress-physical-port MAC SA assignment
*      mode is configured to global :CPSS_SA_LSB_FULL_48_BIT_GLOBAL by API
*      cpssDxChIpPortRouterMacSaLsbModeSet. The routerMacSaIndex is configured
*      by cpssDxChIpRouterGlobalMacSaIndexSet.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChIpRouterGlobalMacSaSet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{

    GT_STATUS   status;

    GT_U8               dev;
    GT_U32              routerMacSaIndex;
    GT_ETHERADDR        macSaAddr;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    dev = (GT_U8)inArgs[0];
    routerMacSaIndex = (GT_U32)inArgs[1];
    galtisMacAddr(&macSaAddr, (GT_U8*)inArgs[2]);

    /* call cpss api function */
    status =  cpssDxChIpRouterGlobalMacSaSet(dev,routerMacSaIndex,&macSaAddr);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, status, "");

    return CMD_OK;
}




/*******************************************************************************
* cpssDxChIpRouterGlobalMacSaGet
*
* DESCRIPTION:
*       Gets full 48-bit Router MAC SA from Global MAC SA table.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*     devNum            - the device number.
*     routerMacSaIndex  - The index into the global MAC SA table.
*
* OUTPUTS:
*     macSaAddrPtr      - The 48 bits MAC SA.
*
* RETURNS:
*     GT_OK                    - on success.
*     GT_FAIL                  - on error.
*     GT_HW_ERROR              - on hardware error.
*     GT_BAD_PARAM             - wrong devNum or routerMacSaIndex.
*     GT_NOT_APPLICABLE_DEVICE - on not applicable device.
*
*
* COMMENTS:
*      This API is applicable when per-egress-physical-port MAC SA assignment
*      mode is configured to global :CPSS_SA_LSB_FULL_48_BIT_GLOBAL by API
*      cpssDxChIpPortRouterMacSaLsbModeSet. The routerMacSaIndex is configured
*      by cpssDxChIpRouterGlobalMacSaIndexSet.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChIpRouterGlobalMacSaGet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{

    GT_STATUS   status;

    GT_U8               dev;
    GT_U32              routerMacSaIndex;
    GT_ETHERADDR        macSaAddr;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    dev = (GT_U8)inArgs[0];
    routerMacSaIndex = (GT_U32)inArgs[1];

    /* call cpss api function */
    status =  cpssDxChIpRouterGlobalMacSaGet(dev,routerMacSaIndex,&macSaAddr);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, status, "%6b", macSaAddr.arEther);

    return CMD_OK;
}


/*******************************************************************************
* cpssDxChIpRouterPortGlobalMacSaIndexSet
*
* DESCRIPTION:
*       Set router mac sa index refered to global MAC SA table.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum           - device number.
*       portNum          - port number.
*       routerMacSaIndex - global MAC SA table index.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - wrong device or port.
*       GT_HW_ERROR              - on writing to HW error.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChIpRouterGlobalMacSaIndexSet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                          result;
    GT_U8                              devNum;
    GT_PORT_NUM                        portNum;
    GT_U32                             routerMacSaIndex;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    portNum = (GT_PORT_NUM)inArgs[1];
    routerMacSaIndex = (GT_U32)inArgs[2];

    /* Override Device and Port */
    CONVERT_DEV_PORT_U32_MAC(devNum, portNum);

    /* call cpss api function */
    result = cpssDxChIpRouterPortGlobalMacSaIndexSet(devNum, portNum, routerMacSaIndex);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChIpRouterGlobalMacSaIndexGet
*
* DESCRIPTION:
*       Get router mac sa index refered to global MAC SA table.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum      - device number.
*       portNum     - port number.
*
* OUTPUTS:
*       routerMacSaIndexPtr  - (pointer to) global MAC SA table index.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - wrong device or port.
*       GT_HW_ERROR              - on writing to HW error.
*       GT_BAD_PTR               - one of the parameters is NULL pointer.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChIpRouterGlobalMacSaIndexGet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                          result;
    GT_U8                              devNum;
    GT_PORT_NUM                        portNum;
    GT_U32                             routerMacSaIndex;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    portNum = (GT_PORT_NUM)inArgs[1];

    /* Override Device and Port */
    CONVERT_DEV_PORT_U32_MAC(devNum, portNum);

    /* call cpss api function */
    result = cpssDxChIpRouterPortGlobalMacSaIndexGet(devNum, portNum, &routerMacSaIndex);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", routerMacSaIndex);
    return CMD_OK;
}

/*******************************************************************************
* cpssDxChIpRouterPortMacSaLsbSet
*
* DESCRIPTION:
*       Sets the 8 LSB Router MAC SA for this EGGRESS PORT.
*
* APPLICABLE DEVICES:
*       DxCh3 and above.
*
* INPUTS:
*     devNum          - the device number
*     portNum         - Eggress Port number
*     saMac           - The 8 bits SA mac value to be written to the SA bits of
*                       routed packet if SA alteration mode is configured to
*                       take LSB according to Eggress Port number.
*
* OUTPUTS:
*     None.
*
* RETURNS:
*     GT_OK            - on success
*     GT_NOT_SUPPORTED - if the feature does not supported by specified device
*     GT_FAIL          - on error.
*     GT_HW_ERROR      - on hardware error
*     GT_BAD_PARAM     - wrong devNum
*
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChIpRouterPortMacSaLsbSet
(
   IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
   IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
   IN  GT_32 numFields,
   OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    GT_PHYSICAL_PORT_NUM portNum;
    GT_U8 saMac;


    /* check for valid arguments */
    if (!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];
    portNum = (GT_PHYSICAL_PORT_NUM)inArgs[1];
    saMac = (GT_U8)inArgs[2];

    /* Override Device and Port */
    CONVERT_DEV_PHYSICAL_PORT_MAC(devNum, portNum);

    /* call cpss api function */
    result = cpssDxChIpRouterPortMacSaLsbSet(devNum, portNum, saMac);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChIpRouterPortMacSaLsbGet
*
* DESCRIPTION:
*       Gets the 8 LSB Router MAC SA for this EGGRESS PORT.
*
* APPLICABLE DEVICES:
*       DxCh3 and above.
*
* INPUTS:
*     devNum          - the device number
*     portNum         - Eggress Port number
*
* OUTPUTS:
*     saMacPtr        - (pointer to) The 8 bits SA mac value written to the SA
*                       bits of routed packet if SA alteration mode is configured
*                       to take LSB according to Eggress Port number.
*
* RETURNS:
*     GT_OK            - on success
*     GT_NOT_SUPPORTED - if the feature does not supported by specified device
*     GT_FAIL          - on error.
*     GT_HW_ERROR      - on hardware error
*     GT_BAD_PARAM     - wrong devNum
*     GT_BAD_PTR       - one of the parameters is NULL pointer
*
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChIpRouterPortMacSaLsbGet
(
   IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
   IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
   IN  GT_32 numFields,
   OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    GT_PHYSICAL_PORT_NUM portNum;
    GT_U8 saMac;


    /* check for valid arguments */
    if (!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];
    portNum = (GT_PHYSICAL_PORT_NUM)inArgs[1];

    /* Override Device and Port */
    CONVERT_DEV_PHYSICAL_PORT_MAC(devNum, portNum);

    /* call cpss api function */
    result = cpssDxChIpRouterPortMacSaLsbGet(devNum, portNum, &saMac);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", saMac);

    return CMD_OK;
}
/*******************************************************************************
* cpssDxChIpRouterVlanMacSaLsbSet
*
* DESCRIPTION:
*       Sets the 8 LSB Router MAC SA for this VLAN.
*
* APPLICABLE DEVICES:
*       DxCh3 and above.
*
* INPUTS:
*     devNum          - the device number
*     vlan            - VLAN Id
*     saMac           - The 8 bits SA mac value to be written to the SA bits of
*                       routed packet if SA alteration mode is configured to
*                       take LSB according to VLAN.
*
* OUTPUTS:
*     None.
*
* RETURNS:
*     GT_OK            - on success
*     GT_NOT_SUPPORTED - if the feature does not supported by specified device
*     GT_FAIL          - on error.
*     GT_HW_ERROR      - on hardware error
*     GT_BAD_PARAM     - wrong devNum or vlanId
*
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChIpRouterVlanMacSaLsbSet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    GT_U16 vlan;
    GT_U8 saMac;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];
    vlan = (GT_U16)inArgs[1];
    saMac = (GT_U8)inArgs[2];

    /* call cpss api function */
    result = cpssDxChIpRouterVlanMacSaLsbSet(devNum, vlan, saMac);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChIpRouterVlanMacSaLsbGet
*
* DESCRIPTION:
*       Gets the 8 LSB Router MAC SA for this VLAN.
*
* APPLICABLE DEVICES:
*       DxCh3 and above.
*
* INPUTS:
*     devNum          - the device number
*     vlan            - VLAN Id
*
* OUTPUTS:
*     saMacPtr        - (pointer to) The 8 bits SA mac value written to the SA
*                       bits of routed packet if SA alteration mode is configured
*                       to take LSB according to VLAN.
*
* RETURNS:
*     GT_OK            - on success
*     GT_NOT_SUPPORTED - if the feature does not supported by specified device
*     GT_FAIL          - on error.
*     GT_HW_ERROR      - on hardware error
*     GT_BAD_PARAM     - wrong devNum or vlanId
*     GT_BAD_PTR       - one of the parameters is NULL pointer
*
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChIpRouterVlanMacSaLsbGet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    GT_U16 vlan;
    GT_U8 saMac;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];
    vlan = (GT_U16)inArgs[1];

    /* call cpss api function */
    result = cpssDxChIpRouterVlanMacSaLsbGet(devNum, vlan, &saMac);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", saMac);

    return CMD_OK;
}
/*******************************************************************************
* cpssDxChIpEcmpUcRpfCheckEnableSet
*
* DESCRIPTION:
*       Globally enables/disables ECMP/QoS unicast RPF check.
*
* APPLICABLE DEVICES:
*       DxCh3 and above.
*
* INPUTS:
*       devNum    - the device number
*       enable    - enable / disable ECMP/Qos unicast RPF check
*
* OUTPUTS:
*       none
*
* RETURNS:
*       GT_OK   - on success
*       GT_NOT_INITIALIZED - The library was not initialized.
*       GT_FAIL - on error
*
* COMMENTS:
*       None.
*
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChIpEcmpUcRpfCheckEnableSet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    GT_BOOL enable;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];
    enable = (GT_BOOL)inArgs[1];

    /* call cpss api function */
    result = cpssDxChIpEcmpUcRpfCheckEnableSet(devNum, enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChIpEcmpUcRpfCheckEnableGet
*
* DESCRIPTION:
*       Gets globally enables/disables ECMP/QoS unicast RPF check state.
*
* APPLICABLE DEVICES:
*       DxCh3 and above.
*
* INPUTS:
*       devNum    - the device number
*       enablePtr - (points to) enable / disable ECMP/Qos unicast RPF check
*
* OUTPUTS:
*       none
*
* RETURNS:
*       GT_OK   - on success
*       GT_NOT_INITIALIZED - The library was not initialized.
*       GT_FAIL - on error
*
* COMMENTS:
*       None.
*
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChIpEcmpUcRpfCheckEnableGet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    GT_BOOL enable;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];

    /* call cpss api function */
    result = cpssDxChIpEcmpUcRpfCheckEnableGet(devNum, &enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d",enable);

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChIpUcRpfModeSet
*
* DESCRIPTION:
*       Defines the uRPF check mode for a given VID.
*
* APPLICABLE DEVICES:  Lion and above
*
* INPUTS:
*       devNum    - the device number
*       vid       - Vlan Id
*       uRpfMode  - CPSS_DXCH_IP_URPF_DISABLE_MODE_E = uRPF check is disabled
*                        for this VID.
*                   CPSS_DXCH_IP_URPF_VLAN_MODE_E= Vlan: If ECMP uRPF is
*                                        globally enabled, then uRPF check is done
*                                        by comparing the packet VID to the VID
*                                        in the additional route entry, otherwise
*                                        it is done using the SIP-Next Hop Entry VID.
*                   CPSS_DXCH_IP_URPF_PORT_MODE_E = uRPF check is done by
*                        comparing the packet source {device,port}/Trunk to
*                              the SIP-Next Hop Entry {device,port}/Trunk.
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong device, vid or uRpfMode
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       If ucRPFCheckEnable field configured in cpssDxChIpLttWrite is enabled
*       then VLAN-based uRPF check is performed, regardless of this configuration.
*       Otherwise, uRPF check is performed for this VID according to this
*       configuration.
*       Port-based uRPF mode is not supported if the address is associated
*       with an ECMP/QoS block of nexthop entries.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChIpUcRpfModeSet
(
 IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
 IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
 IN  GT_32 numFields,
 OUT GT_8  outArgs[CMD_MAX_BUFFER]
 )
{

    GT_STATUS   status;

    GT_U8                           devNum;
    GT_U16                          vid;
    CPSS_DXCH_IP_URPF_MODE_ENT      uRpfMode;

   /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    devNum      = (GT_U8)inArgs[0];
    vid         = (GT_U16)inArgs[1];
    uRpfMode    = (CPSS_DXCH_IP_URPF_MODE_ENT)inArgs[2];

    /* call cpss api function */
    status =  cpssDxChIpUcRpfModeSet(devNum,vid, uRpfMode);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, status, "");

    return CMD_OK;
}


/*******************************************************************************
* cpssDxChIpUcRpfModeGet
*
* DESCRIPTION:
*       Read uRPF check mode for a given VID.
*
* APPLICABLE DEVICES:  Lion and above
*
* INPUTS:
*       devNum    - the device number
*       vid       - Vlan Id
*
* OUTPUTS:
*       uRpfModePtr  -  CPSS_DXCH_IP_URPF_DISABLE_MODE_E = uRPF check is disabled
*                           for this VID.
*                       CPSS_DXCH_IP_URPF_VLAN_MODE_E= Vlan: If ECMP uRPF is
*                                        globally enabled, then uRPF check is done
*                                        by comparing the packet VID to the VID
*                                        in the additional route entry, otherwise
*                                        it is done using the SIP-Next Hop Entry VID.
*                       CPSS_DXCH_IP_URPF_PORT_MODE_E = uRPF check is done by
*                           comparing the packet source {device,port}/Trunk to
*                           the SIP-Next Hop Entry {device,port}/Trunk.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong device
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*
* COMMENTS:
*       If ucRPFCheckEnable field configured in cpssDxChIpLttWrite is enabled
*       then VLAN-based uRPF check is performed, regardless of this configuration.
*       Otherwise, uRPF check is performed for this VID according to this
*       configuration.
*       Port-based uRPF mode is not supported if the address is associated
*       with an ECMP/QoS block of nexthop entries.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChIpUcRpfModeGet
(
 IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
 IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
 IN  GT_32 numFields,
 OUT GT_8  outArgs[CMD_MAX_BUFFER]
 )
{

    GT_STATUS   status;

    GT_U8                           devNum;
    GT_U16                          vid;
    CPSS_DXCH_IP_URPF_MODE_ENT      uRpfMode;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    devNum      = (GT_U8)inArgs[0];
    vid         = (GT_U16)inArgs[1];

    /* call cpss api function */
    status =  cpssDxChIpUcRpfModeGet(devNum,vid, &uRpfMode);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, status, "%d", uRpfMode);

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChIpPortSipSaEnableSet
*
* DESCRIPTION:
*       Enable SIP/SA check for packets received from the given port.
*
* APPLICABLE DEVICES:  Lion and above
*
* INPUTS:
*       devNum    - the device number
*       portNum   - the port number (including CPU port)
*       enable    - GT_FALSE: disable SIP/SA check on the port
*                   GT_TRUE:  enable SIP/SA check on the port
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong device or port
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       SIP/SA check is triggered only if both this flag and
*       the sipSaCheckMismatchEnable field configured in cpssDxChIpLttWrite
*       are enabled.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChIpPortSipSaEnableSet
(
 IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
 IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
 IN  GT_32 numFields,
 OUT GT_8  outArgs[CMD_MAX_BUFFER]
 )
{

    GT_STATUS   status;

    GT_U8                        devNum;
    GT_PORT_NUM                  portNum;
    GT_BOOL                      enable;

   /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    devNum      = (GT_U8)inArgs[0];
    portNum     = (GT_PORT_NUM)inArgs[1];
    enable      = (GT_BOOL)inArgs[2];

    /* Override Device and Port */
    CONVERT_DEV_PORT_U32_MAC(devNum, portNum);

    /* call cpss api function */
    status =  cpssDxChIpPortSipSaEnableSet(devNum,portNum, enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, status, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChIpPortSipSaEnableGet
*
* DESCRIPTION:
*       Return the SIP/SA check status for packets received from the given port.
*
* APPLICABLE DEVICES:  Lion and above
*
* INPUTS:
*       devNum    - the device number
*       portNum   - the port number (including CPU port)

* OUTPUTS:
*       enablePtr - GT_FALSE: SIP/SA check on the port is disabled
*                   GT_TRUE:  SIP/SA check on the port is enabled
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong device or port
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*
* COMMENTS:
*       SIP/SA check is triggered only if both this flag and
*       the sipSaCheckMismatchEnable field configured in cpssDxChIpLttWrite
*       are enabled.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChIpPortSipSaEnableGet
(
 IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
 IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
 IN  GT_32 numFields,
 OUT GT_8  outArgs[CMD_MAX_BUFFER]
 )
{

    GT_STATUS   status;

    GT_U8                        devNum;
    GT_PORT_NUM                        portNum;
    GT_BOOL                      enable;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    devNum      = (GT_U8)inArgs[0];
    portNum     = (GT_PORT_NUM)inArgs[1];

    /* Override Device and Port */
    CONVERT_DEV_PORT_U32_MAC(devNum, portNum);

    /* call cpss api function */
    status =  cpssDxChIpPortSipSaEnableGet(devNum,portNum, &enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, status, "%d", enable);

    return CMD_OK;
}


/*******************************************************************************
* cpssDxChIpTcamEccDaemonEnableSet
*
* DESCRIPTION:
*       Enables or disables Router TCAM ECC Daemon.
*
* APPLICABLE DEVICES: All DxCh3 devices.
*
* INPUTS:
*       devNum        - device number
*       enalbe        - GT_TRUE  - Router TCAM ECC Daemon enable
*                       GT_FALSE - Router TCAM ECC Daemon disable
* OUTPUTS:
*      None
*
* RETURNS :
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong parameter
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_HW_ERROR              - on hardware error
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChIpTcamEccDaemonEnableSet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    GT_BOOL enalbe;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    enalbe = (GT_BOOL)inArgs[1];

    /* call cpss api function */
    result = cpssDxChIpTcamEccDaemonEnableSet(devNum, enalbe);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChIpTcamEccDaemonEnableGet
*
* DESCRIPTION:
*       Gets status (Enabled/Disabled) of Router TCAM ECC Daemon.
*
* APPLICABLE DEVICES: All DxCh3 devices.
*
* INPUTS:
*       devNum        - device number
*
* OUTPUTS:
*       enablePtr     - (pointer to) Router TCAM ECC Daemon status:
*                       GT_TRUE  - Router TCAM ECC Daemon enable
*                       GT_FALSE - Router TCAM ECC Daemon disable
*
* RETURNS :
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong parameter
*       GT_BAD_PTR               - null pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_HW_ERROR              - on hardware error
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChIpTcamEccDaemonEnableGet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    GT_BOOL enable;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];

    /* call cpss api function */
    result = cpssDxChIpTcamEccDaemonEnableGet(devNum, &enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", enable);

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChIpRouterSourceIdOverrideEnableSet
*
* DESCRIPTION:
*     Enable/Disable overriding of source id by routing engine.
*
* APPLICABLE DEVICES:
*        Lion2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion3.
*
* INPUTS:
*       devNum   - the device number
*       enable   - GT_TRUE  - enable override
*                  GT_FALSE - disable override
*
* OUTPUTS:
*      None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - Illegal parameter in function called
*       GT_FAIL                  - on error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       Lion2 device: applicable starting from revision B1
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChIpRouterSourceIdOverrideEnableSet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    GT_BOOL enable;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    enable = (GT_BOOL)inArgs[1];

    /* call cpss api function */
    result = cpssDxChIpRouterSourceIdOverrideEnableSet(devNum, enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChIpRouterSourceIdOverrideEnableGet
*
* DESCRIPTION:
*     Get Enable/Disable status of overriding of source id
*     by routing engine.
*
* APPLICABLE DEVICES:
*        Lion2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion3.
*
* INPUTS:
*       devNum    - the device number
*
* OUTPUTS:
*       enablePtr - (pointer to)enable:
*                    GT_TRUE  - enable override
*                    GT_FALSE - disable override
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - Illegal parameter in function called
*       GT_BAD_PTR               - on NULL pointer
*       GT_FAIL                  - on error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       Lion2 device: applicable starting from revision B1
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChIpRouterSourceIdOverrideEnableGet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    GT_BOOL enable;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    enable = GT_FALSE;

    /* call cpss api function */
    result = cpssDxChIpRouterSourceIdOverrideEnableGet(devNum, &enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", enable);

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChIpHeaderErrorMaskSet
*
* DESCRIPTION:
*       Mask or unmask an IP header error.
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum              - the device number
*       ipHeaderErrorType   - the IP header error type
*       protocolStack       - whether to mask/unmask the error for IPv4, IPv6 or
*                             both
*       prefixType          - whether to mask/unmask the error for unicast,
*                             multicast or both
*       mask                - GT_TRUE: mask the error
*                             GT_FALSE: unmask the error
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_NOT_INITIALIZED       - The library was not initialized.
*       GT_BAD_PARAM             - Illegal parameter in function called
*       GT_FAIL                  - on error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       If the error is masked, then an IPv4/6 UC/MC header exception will not
*       be triggered for the relevant header error.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChIpHeaderErrorMaskSet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    CPSS_DXCH_IP_HEADER_ERROR_ENT        ipHeaderErrorType;
    CPSS_IP_PROTOCOL_STACK_ENT           protocolStack;
    CPSS_UNICAST_MULTICAST_ENT           prefixType;
    GT_BOOL                              mask;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    ipHeaderErrorType = (CPSS_DXCH_IP_HEADER_ERROR_ENT)inArgs[1];
    protocolStack = (CPSS_IP_PROTOCOL_STACK_ENT)inArgs[2];
    prefixType = (CPSS_UNICAST_MULTICAST_ENT)inArgs[3];
    mask = (GT_BOOL)inArgs[4];

    /* call cpss api function */
    result = cpssDxChIpHeaderErrorMaskSet(devNum, ipHeaderErrorType, protocolStack, prefixType, mask);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChIpHeaderErrorMaskGet
*
* DESCRIPTION:
*       Get the masking status of an IP header error.
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum              - the device number
*       ipHeaderErrorType   - the IP header error type
*       protocolStack       - whether to check the masking status for IPv4 or IPv6
*       prefixType          - whether to check the masking status for unicast or
*                             multicast
*
* OUTPUTS:
*       maskPtr             - GT_TRUE: mask the error
*                             GT_FALSE: unmask the error
*
* RETURNS:
*       GT_OK                    - on success
*       GT_NOT_INITIALIZED       - The library was not initialized.
*       GT_BAD_PARAM             - Illegal parameter in function called
*       GT_FAIL                  - on error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       If the error is masked, then an IPv4/6 UC/MC header exception will not
*       be triggered for the relevant header error.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChIpHeaderErrorMaskGet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    CPSS_DXCH_IP_HEADER_ERROR_ENT        ipHeaderErrorType;
    CPSS_IP_PROTOCOL_STACK_ENT           protocolStack;
    CPSS_UNICAST_MULTICAST_ENT           prefixType;
    GT_BOOL                              mask;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    ipHeaderErrorType = (CPSS_DXCH_IP_HEADER_ERROR_ENT)inArgs[1];
    protocolStack = (CPSS_IP_PROTOCOL_STACK_ENT)inArgs[2];
    prefixType = (CPSS_UNICAST_MULTICAST_ENT)inArgs[3];

    /* call cpss api function */
    result = cpssDxChIpHeaderErrorMaskGet(devNum, ipHeaderErrorType, protocolStack, prefixType, &mask);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", mask);

    return CMD_OK;
}

/*******************************************************************************
* wrCpssDxChIpUcRoutingVid1AssignEnableSet
*
* DESCRIPTION:
*       Enable/disable VID1 assignment by the unicast routing
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; xCat2; DxCh2; DxCh3; xCat; xCat3; Lion; Lion2.
*
* INPUTS:
*       devNum    - the device number
*       enable    - enable/disable VID1 assignment by the unicast routing
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong devNum
*
* COMMENTS:
*       When VID1 assignment is enabled QoS attributes can't be set by the
*       router and the following fields in the unicast nexthop entries are not
*       applicable:
*       qosProfileMarkingEnable, qosProfileIndex, qosPrecedence, modifyUp,
*       modifyDscp.
*       When VID1 assignment is disabled QoS attributes are applicable and
*       nextHopVlanId1 field in the unicast nexthop entries is not applicable.
*       It's recommended to use this API before configuring the nexthop entries.
*       Using this API when nexthops are already configured can cause unexpected
*       results.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChIpUcRoutingVid1AssignEnableSet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    GT_BOOL enable;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    enable = (GT_BOOL)inArgs[1];

    /* call cpss api function */
    result = cpssDxChIpUcRoutingVid1AssignEnableSet(devNum, enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* wrCpssDxChIpUcRoutingVid1AssignEnableGet
*
* DESCRIPTION:
*       Get the enabling status of VID1 assignment by the unicast routing
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; xCat2; DxCh2; DxCh3; xCat; xCat3; Lion; Lion2.
*
* INPUTS:
*       devNum    - the device number
*
* OUTPUTS:
*       enablePtr - (pointer to) VID1 unicast routing assignment enabling status
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong devNum
*
* COMMENTS:
*       When VID1 assignment is enabled QoS attributes can't be set by the
*       router and the following fields in the unicast nexthop entries are not
*       applicable:
*       qosProfileMarkingEnable, qosProfileIndex, qosPrecedence, modifyUp,
*       modifyDscp.
*       When VID1 assignment is disabled QoS attributes are applicable and
*       nextHopVlanId1 field in the unicast nexthop entries is not applicable.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChIpUcRoutingVid1AssignEnableGet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    GT_BOOL enable;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];

    /* call cpss api function */
    result = cpssDxChIpUcRoutingVid1AssignEnableGet(devNum, &enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", enable);

    return CMD_OK;
}


/*******************************************************************************
* cpssDxChIpMllMultiTargetShaperBaselineSet
*
* DESCRIPTION:
*       Relevant for L2 and L3 MLL.
*       Set Token Bucket Baseline.
*       The Token Bucket Baseline is the "zero" level of the token bucket.
*       When the token bucket fill level < Baseline,
*       the respective packet is not served.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; xCat2; Lion; Lion2.
*
* INPUTS:
*       devNum   - device number.
*       baseline - Token Bucket Baseline value in bytes
*                   (APPLICABLE RANGES: 0..0xFFFFFF).
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - on wrong device number
*       GT_OUT_OF_RANGE          - on out of range baseline
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       Token Bucket Baseline must be configured as follows:
*        1. At least MTU (the maximum expected packet size in the system).
*        2. When packet based shaping is enabled, the following used as
*           shaper's MTU:
*           cpssDxChIpMllMultiTargetShaperMtuSet.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChIpMllMultiTargetShaperBaselineSet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    GT_U32 baseline;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    baseline = (GT_U32)inArgs[1];

    /* call cpss api function */
    result = cpssDxChIpMllMultiTargetShaperBaselineSet(devNum, baseline);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChIpMllMultiTargetShaperBaselineGet
*
* DESCRIPTION:
*       Relevant for L2 and L3 MLL.
*       Get Token Bucket Baseline.
*       The Token Bucket Baseline is the "zero" level of the token bucket.
*       When the token bucket fill level < Baseline, the respective packet
*       is not served.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; xCat2; Lion; Lion2.
*
* INPUTS:
*       devNum  - device number.
*
* OUTPUTS:
*       baselinePtr - (pointer to) Token Bucket Baseline value in bytes.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - on wrong device number
*       GT_BAD_PTR               - on NULL pointer
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChIpMllMultiTargetShaperBaselineGet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    GT_U32 baseline;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];

    /* call cpss api function */
    result = cpssDxChIpMllMultiTargetShaperBaselineGet(devNum, &baseline);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", baseline);

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChIpMllMultiTargetShaperMtuSet
*
* DESCRIPTION:
*       Relevant for L2 and L3 MLL.
*       Set the packet length in bytes for updating the shaper token bucket.
*       Valid when <Token Bucket Mode>=Packets.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; xCat2; Lion; Lion2.
*
* INPUTS:
*       devNum   - physical device number
*       mtu      - MTU for egress rate shaper
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - on wrong device number, mtu
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       <Token Bucket Mode> is configured in cpssDxChIpMllMultiTargetShaperConfigurationSet
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChIpMllMultiTargetShaperMtuSet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    GT_U32 mtu;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    mtu = (GT_U32)inArgs[1];

    /* call cpss api function */
    result = cpssDxChIpMllMultiTargetShaperMtuSet(devNum, mtu);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}


/*******************************************************************************
* cpssDxChIpMllMultiTargetShaperMtuGet
*
* DESCRIPTION:
*       Relevant for L2 and L3 MLL.
*       Get the packet length in bytes for updating the shaper token bucket.
*       Valid when <Token Bucket Mode>=Packets.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; xCat2; Lion; Lion2.
*
* INPUTS:
*       devNum   - physical device number
*
* OUTPUTS:
*       mtuPtr   -  pointer to MTU for egress rate shaper
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - on wrong device number
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_BAD_STATE             - on invalid hardware value read
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       <Token Bucket Mode> is configured in cpssDxChIpMllMultiTargetShaperConfigurationSet
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChIpMllMultiTargetShaperMtuGet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    GT_U32 baseline;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];

    /* call cpss api function */
    result = cpssDxChIpMllMultiTargetShaperMtuGet(devNum, &baseline);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", baseline);

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChIpMllMultiTargetShaperTokenBucketModeSet
*
* DESCRIPTION:
*       Relevant for L2 and L3 MLL.
*       Set Token Bucket Mode Mll shaper.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; xCat2; Lion; Lion2.
*
* INPUTS:
*      devNum           - device number.
*      tokenBucketMode  -   Defines the packet length used to decrease the shaper token bucket.
*                           In Byte mode, the token bucket rate shaper is decreased according to the
*                           packet length as received by the switch.
*                           In Packet mode, the token bucket rate shaper is decreased by the packet
*                           length specified in cpssDxChIpMllMultiTargetShaperMtuSet.
* OUTPUTS:
*      None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PTR               - on NULL pointer
*       GT_BAD_PARAM             - on wrong device number
*       GT_OUT_OF_RANGE          - on out of range value
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*        None
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChIpMllMultiTargetShaperTokenBucketModeSet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    CPSS_PORT_TX_DROP_SHAPER_MODE_ENT            tokenBucketMode;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    tokenBucketMode = (CPSS_PORT_TX_DROP_SHAPER_MODE_ENT)inArgs[1];

    /* call cpss api function */
    result = cpssDxChIpMllMultiTargetShaperTokenBucketModeSet(devNum, tokenBucketMode);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}


/*******************************************************************************
* cpssDxChIpMllMultiTargetShaperTokenBucketModeGet
*
* DESCRIPTION:
*       Relevant for L2 and L3 MLL.
*       Get Token Bucket Mode Mll shaper.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; xCat2; Lion; Lion2.
*
* INPUTS:
*      devNum     - device number.
*
* OUTPUTS:
*      tokenBucketModePtr  - (pointer to)Defines the packet length used to decrease the shaper token bucket.
*                           In Byte mode, the token bucket rate shaper is decreased according to the
*                           packet length as received by the switch.
*                           In Packet mode, the token bucket rate shaper is decreased by the packet
*                           length specified in cpssDxChIpMllMultiTargetShaperMtuSet.
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PTR               - on NULL pointer
*       GT_BAD_PARAM             - on wrong device number
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       none.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChIpMllMultiTargetShaperTokenBucketModeGet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    CPSS_PORT_TX_DROP_SHAPER_MODE_ENT   tokenBucketMode;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];

    /* call cpss api function */
    result = cpssDxChIpMllMultiTargetShaperTokenBucketModeGet(devNum, &tokenBucketMode);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", tokenBucketMode);

    return CMD_OK;
}

/*******************************************************************************
*  cpssDxChIpMllMultiTargetShaperEnableSet
*
* DESCRIPTION:
*       Relevant for L2 and L3 MLL.
*       Enable/Disable Token Bucket rate shaping.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; xCat2; Lion; Lion2.
*
* INPUTS:
*       devNum  - physical device number
*       enable  - GT_TRUE, enable Shaping
*                 GT_FALSE, disable Shaping
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - on wrong port number or device
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChIpMllMultiTargetShaperEnableSet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    GT_BOOL enable;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    enable = (GT_BOOL)inArgs[1];

    /* call cpss api function */
    result = cpssDxChIpMllMultiTargetShaperEnableSet(devNum, enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}


/*******************************************************************************
* cpssDxChIpMllMultiTargetShaperEnableGet
*
* DESCRIPTION:
*       Relevant for L2 and L3 MLL.
*       Get Enable/Disable Token Bucket rate shaping status
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; xCat2; Lion; Lion2.
*
* INPUTS:
*       devNum  - physical device number
*
* OUTPUTS:
*       enablePtr - Pointer to Token Bucket rate shaping status.
*                   - GT_TRUE, enable Shaping
*                   - GT_FALSE, disable Shaping
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - on wrong port number or device
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters in NULL pointer.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChIpMllMultiTargetShaperEnableGet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    GT_BOOL enable;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];

    /* call cpss api function */
    result = cpssDxChIpMllMultiTargetShaperEnableGet(devNum, &enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", enable);

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChIpMllMultiTargetShaperConfigurationSet
*
* DESCRIPTION:
*       Relevant for L2 and L3 MLL.
*       Set configuration for Mll shaper.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; xCat2; Lion; Lion2.
*
* INPUTS:
*      devNum           - device number.
*      maxBucketSize    - Maximum bucket size in bytes. The field resolution is 4096 bytes.
*                         The actual burst size is approximately
*                         <MaxBucketSize>*4096 - <Token Bucket Base Line>
*                         When the CPU writes to this field, the value is also written
*                         to the Token Bucket counter.
*                         <MaxBucketSize>*4096 must be set to be greater than both <Tokens>
*                         and <Token Bucket Base Line>.
*                         0 means 4k and 0xFFF means 16M
*                         The bucket size ranges from 4 KB to 16 MB, in steps of 4K.
*                         (APPLICABLE RANGES: 0...0xFFF)
*      maxRatePtr       - Requested Rate in Kbps or packets per second
*                         according to shaper mode.
* OUTPUTS:
*      maxRatePtr   - (pointer to) the actual Rate value in Kbps or packets per second.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PTR               - on NULL pointer
*       GT_BAD_PARAM             - on wrong device number
*       GT_OUT_OF_RANGE          - on out of range value
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*        When packet based shaping is enabled, the following used as shaper's MTU:
*        see:
*        cpssDxChIpMllMultiTargetShaperMtuSet
*        cpssDxChIpMllMultiTargetShaperBaselineSet.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChIpMllMultiTargetShaperConfigurationSet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                            result;
    GT_U8                                devNum;
    GT_U32                               maxBucketSize;
    GT_U32                               maxRate;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    maxBucketSize   = (GT_U32)inArgs[1];
    maxRate = (GT_U32)inArgs[2];

    /* call cpss api function */
    result = cpssDxChIpMllMultiTargetShaperConfigurationSet(devNum,
                                                            maxBucketSize,
                                                            &maxRate);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChIpMllMultiTargetShaperConfigurationGet
*
* DESCRIPTION:
*       Relevant for L2 and L3 MLL.
*       Get configuration for shaper.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; xCat2; Lion; Lion2.
*
* INPUTS:
*      devNum     - device number.
*
* OUTPUTS:
*     maxBucketSizePtr    - (pointer to) Maximum bucket size in bytes. The field resolution is 4096 bytes.
*                         The actual burst size is approximately
*                         <MaxBucketSize>*4096 - <Token Bucket Base Line>
*                         When the CPU writes to this field, the value is also written
*                         to the Token Bucket counter.
*                         <MaxBucketSize>*4096 must be set to be greater than both <Tokens>
*                         and <Token Bucket Base Line>.
*                         0 equal 4k all 12'bFFF equal 16M
*                         The bucket size ranges from 4 KB to 16 MB, in steps of 4K.
*                         (APPLICABLE RANGES: 0...0xFFF)
*      maxRatePtr          - (pointer to) the actual Rate value in Kbps or packets per second.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PTR               - on NULL pointer
*       GT_BAD_PARAM             - on wrong device number
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       none.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChIpMllMultiTargetShaperConfigurationGet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                           result;
    GT_U8                               devNum;
    GT_U32                              maxBucketSize;
    GT_U32                              maxRate;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];

    /* call cpss api function */
    result = cpssDxChIpMllMultiTargetShaperConfigurationGet(devNum, &maxBucketSize,&maxRate);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d%d",
                 maxBucketSize,
                 maxRate);

    return CMD_OK;
}


/*******************************************************************************
* cpssDxChIpMllMultiTargetShaperIntervalConfigurationSet
*
* DESCRIPTION:
*       Relevant for L2 and L3 MLL.
*       Set configuration for Mll shaper Interval.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; xCat2; Lion; Lion2.
*
* INPUTS:
*      devNum                               - device number.
*      tokenBucketIntervalSlowUpdateRatio   - Increases the token bucket update interval to
*                                                <TokenBucketIntervalSlowUpdateRatio>+1> * <TokenBucketUpdateInterval> *
*                                                (64 or 1024 depends on <TokenBucketIntervalUpdateRatio>) core clock cycles.
*                                                Enabled by setting SlowRateEn = GT_TRUE.
*                                                (APPLICABLE RANGES: 1...16)
*
*       tokenBucketUpdateInterval           -   Defines the token update interval for the egress rate shapers.
*                                         The update interval = <TokenBucketIntervalSlowUpdateRatio>+1> *
*                                        <TokenBucketUpdateInterval> * (64 or 1024 depends on
*                                        <TokenBucketIntervalUpdateRatio>)*2^(Port/PriorityTokenBucketEntry<TBIntervalUpdateRatio>) core clock cycles.
*                                        Upon every interval expiration, a configured amount of tokens is added to the
*                                        token bucket. The amount of tokens is configured in Port/PriorityTokenBucketEntry<Tokens>.
*                                        NOTE:
*                                        - This field must not be 0.
*                                          (APPLICABLE RANGES: 1...15)
*      tokenBucketIntervalUpdateRatio  - Token Bucket Interval Update Ratio : 64 or 1024
*
* OUTPUTS:
*      None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PTR               - on NULL pointer
*       GT_BAD_PARAM             - on wrong device number
*       GT_OUT_OF_RANGE          - on out of range value
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChIpMllMultiTargetShaperIntervalConfigurationSet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                            result;
    GT_U8                                devNum;
    GT_U32                                          tokenBucketIntervalSlowUpdateRatio;
    GT_U32                                          tokenBucketUpdateInterval;
    CPSS_DXCH_MULTI_TARGET_SHAPER_GRANULARITY_ENT   tokenBucketIntervalUpdateRatio;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    tokenBucketIntervalSlowUpdateRatio   = (GT_U32)inArgs[1];
    tokenBucketUpdateInterval = (GT_U32)inArgs[2];
    tokenBucketIntervalUpdateRatio = (GT_U32)inArgs[3];

    /* call cpss api function */
    result = cpssDxChIpMllMultiTargetShaperIntervalConfigurationSet(devNum,
                                                            tokenBucketIntervalSlowUpdateRatio,
                                                            tokenBucketUpdateInterval,
                                                            tokenBucketIntervalUpdateRatio);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}
/*******************************************************************************
* cpssDxChIpMllMultiTargetShaperIntervalConfigurationGet
*
* DESCRIPTION:
*       Relevant for L2 and L3 MLL.
*       Get configuration for shaper Interval.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; xCat2; Lion; Lion2.
*
* INPUTS:
*      devNum     - device number.
*
* OUTPUTS:
*      tokenBucketIntervalSlowUpdateRatioPtr   - (pointer to) Increases the token bucket update interval to
*                                                <TokenBucketIntervalSlowUpdateRatio>+1> * <TokenBucketUpdateInterval> *
*                                                (64 or 1024 depends on <TokenBucketIntervalUpdateRatio>) core clock cycles.
*                                                Enabled by setting SlowRateEn = GT_TRUE.
*                                                (APPLICABLE RANGES: 1...16)
*
*       tokenBucketUpdateIntervalPtr    -   (pointer to) the token update interval for the egress rate shapers.
*                                         The update interval = <TokenBucketIntervalSlowUpdateRatio>+1> *
*                                        <TokenBucketUpdateInterval> * (64 or 1024 depends on
*                                        <TokenBucketIntervalUpdateRatio>)*2^(Port/PriorityTokenBucketEntry<TBIntervalUpdateRatio>) core clock cycles.
*                                        Upon every interval expiration, a configured amount of tokens is added to the
*                                        token bucket. The amount of tokens is configured in Port/PriorityTokenBucketEntry<Tokens>.
*                                        NOTE:
*                                        - This field must not be 0.
*                                          (APPLICABLE RANGES: 1...15)
*      tokenBucketIntervalUpdateRatioPtr  - (pointer to) Token Bucket Interval Update Ratio : 64 or 1024
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PTR               - on NULL pointer
*       GT_BAD_PARAM             - on wrong device number
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       none.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChIpMllMultiTargetShaperIntervalConfigurationGet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                           result;
    GT_U8                               devNum;
    GT_U32                                          tokenBucketIntervalSlowUpdateRatio;
    GT_U32                                          tokenBucketUpdateInterval;
    CPSS_DXCH_MULTI_TARGET_SHAPER_GRANULARITY_ENT   tokenBucketIntervalUpdateRatio;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];

    /* call cpss api function */
    result = cpssDxChIpMllMultiTargetShaperIntervalConfigurationGet(devNum,
                                                            &tokenBucketIntervalSlowUpdateRatio,
                                                            &tokenBucketUpdateInterval,
                                                            &tokenBucketIntervalUpdateRatio);
    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d%d%d",
                 tokenBucketIntervalSlowUpdateRatio,
                 tokenBucketUpdateInterval,
                 tokenBucketIntervalUpdateRatio);

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChIpPbrBypassRouterTriggerRequirementsEnableSet
*
* DESCRIPTION:
*       Enable/disable bypassing the router triggering requirements for policy
*       based routing packets
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; xCat2; Lion; Lion2.
*
* INPUTS:
*       devNum     - device number
*       enable     - enable bypassing the router triggering requirements for PBR
*                    packets:
*                    GT_FALSE: Policy based routing uses the normal router
*                              triggering requirements
*                    GT_TRUE:  Policy based routing bypasses the router triggering
*                              requirement for the FDB DA entry <DA Route> to be
*                              set for UC routing
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - on wrong device number
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChIpPbrBypassRouterTriggerRequirementsEnableSet
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
    result = cpssDxChIpPbrBypassRouterTriggerRequirementsEnableSet(devNum, enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChIpPbrBypassRouterTriggerRequirementsEnableGet
*
* DESCRIPTION:
*       Get the enabling status of bypassing the router triggering requirements
*       for policy based routing packets
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; xCat2; Lion; Lion2.
*
* INPUTS:
*       devNum     - device number
*
* OUTPUTS:
*       enablePtr  - the router triggering requirements enabling status for PBR
*                    packets:
*                    GT_FALSE: Policy based routing uses the normal router
*                              triggering requirements
*                    GT_TRUE:  Policy based routing bypasses the router triggering
*                              requirement for the FDB DA entry <DA Route> to be
*                              set for UC routing
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PTR               - on NULL pointer
*       GT_BAD_PARAM             - on wrong device number
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChIpPbrBypassRouterTriggerRequirementsEnableGet
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
    result = cpssDxChIpPbrBypassRouterTriggerRequirementsEnableGet(devNum, &enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", enable);

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChIpVlanMrstBitmapSet
*
* DESCRIPTION:
*       Set the next-hop MSRT state bitmap per eVLAN.
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; xCat2; Lion; Lion2.
*
* INPUTS:
*       devNum     - device number
*       vlanId     - VLAN ID
*       mrstBmp    - Multicast Routing Shared Tree ID 64 bit bitmap
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - on wrong input parameter
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChIpVlanMrstBitmapSet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8     devNum;
    GT_U16    vlanId;
    GT_U64    mrstBmp;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    vlanId = (GT_U16)inArgs[1];
    mrstBmp.l[0] = (GT_U32)inArgs[2];
    mrstBmp.l[1] = (GT_U32)inArgs[3];

    /* call cpss api function */
    result = cpssDxChIpVlanMrstBitmapSet(devNum, vlanId , mrstBmp);
    
    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChIpVlanMrstBitmapGet
*
* DESCRIPTION:
*       Get the next-hop MSRT state bitmap per eVLAN.
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; xCat2; Lion; Lion2.
*
* INPUTS:
*       devNum - device number
*       vlanId - VLAN ID
*
* OUTPUTS:
*       mrstBmpPtr - pointer to Multicast Routing Shared Tree ID 64 bit bitmap
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - on wrong input parameter
*       GT_BAD_PTR               - on NULL pointer
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChIpVlanMrstBitmapGet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8     devNum;
    GT_U16    vlanId;
    GT_U64    mrstBmp;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    vlanId = (GT_U16)inArgs[1];

    /* call cpss api function */
    result = cpssDxChIpVlanMrstBitmapGet(devNum, vlanId , &mrstBmp);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d%d", mrstBmp.l[0], mrstBmp.l[1]);

    return CMD_OK;
}

/**** database initialization **************************************/

static CMD_COMMAND dbCommands[] =
{
    {"cpssDxChIpSpecialRouterTriggerEnable",
        &wrCpssDxChIpSpecialRouterTriggerEnable,
        3, 0},

    {"cpssDxChIpSpecialRouterTriggerEnableGet",
        &wrCpssDxChIpSpecialRouterTriggerEnableGet,
        2, 0},

    {"cpssDxChIpExceptionCommandSet",
        &wrCpssDxChIpExceptionCommandSet,
        4, 0},

    {"cpssDxChIpExceptionCommandGetExt",
        &wrCpssDxChIpExceptionCommandGetExt,
        3, 0},

    {"cpssDxChIpUcRouteAgingModeSet",
        &wrCpssDxChIpUcRouteAgingModeSet,
        2, 0},

    {"cpssDxChIpUcRouteAgingModeGet",
        &wrCpssDxChIpUcRouteAgingModeGet,
        1, 0},

    {"cpssDxChIpRouterSourceIdSet",
        &wrCpssDxChIpRouterSourceIdSet,
        3, 0},

    {"cpssDxChIpRouterSourceIdGet",
        &wrCpssDxChIpRouterSourceIdGet,
        2, 0},

    {"cpssDxChIpRouterSourceId_1Set",
        &wrCpssDxChIpRouterSourceId_1Set,
        4, 0},

    {"cpssDxChIpRouterSourceId_1Get",
        &wrCpssDxChIpRouterSourceId_1Get,
        2, 0},

    {"cpssDxChIpFailRpfCtrlTrafficMultiTargetTCQueueSet",
        &wrCpssDxChIpFailRpfCtrlTrafficMultiTargetTCQueueSet,
        3, 0},

    {"cpssDxChIpFailRpfCtrlTrafficMultiTargetTCQueueGet",
        &wrCpssDxChIpFailRpfCtrlTrafficMultiTargetTCQueueGet,
        1, 0},

    {"cpssDxChIpQosProfileToMultiTargetTCQueueMapSet",
        &wrCpssDxChIpQosProfileToMultiTargetTCQueueMapSet,
        3, 0},

    {"cpssDxChIpQosProfileToMultiTargetTCQueueMapGet",
        &wrCpssDxChIpQosProfileToMultiTargetTCQueueMapGet,
        2, 0},

    {"cpssDxChIpTcDpToMultiTargetTcQueueMapSet",
        &wrCpssDxChIpTcDpToMultiTargetTcQueueMapSet,
        4, 0},

    {"cpssDxChIpTcDpToMultiTargetTcQueueMapGet",
        &wrCpssDxChIpTcDpToMultiTargetTCQueueMapGet,
        3, 0},

    {"cpssDxChIpMultiTargetQueueFullDropCntGet",
        &wrCpssDxChIpMultiTargetQueueFullDropCntGet,
        1, 0},

    {"cpssDxChIpMultiTargetQueueFullDropCntSet",
        &wrCpssDxChIpMultiTargetQueueFullDropCntSet,
        2, 0},

    {"cpssDxChIpMultiTargetTCQueueSchedModeSet",
        &wrCpssDxChIpMultiTargetTCQueueSchedModeSet,
        4, 0},

    {"cpssDxChIpMultiTargetTCQueueSchedModeGet",
        &wrCpssDxChIpMultiTargetTCQueueSchedModeGet,
        2, 0},

    {"cpssDxChIpBridgeServiceEnable",
        &wrCpssDxChIpBridgeServiceEnable,
        4, 0},

    {"cpssDxChIpBridgeServiceEnableGet",
        &wrCpssDxChIpBridgeServiceEnableGet,
        3, 0},

    {"cpssDxChIpMllBridgeEnable",
        &wrCpssDxChIpMllBridgeEnable,
        2, 0},

    {"cpssDxChIpMllBridgeEnableGet",
        &wrCpssDxChIpMllBridgeEnableGet,
        1, 0},

    {"cpssDxChIpMultiTargetRateShaperSet",
        &wrCpssDxChIpMultiTargetRateShaperSet,
        3, 0},

    {"cpssDxChIpMultiTargetRateShaperGet",
        &wrCpssDxChIpMultiTargetRateShaperGet,
        1, 0},

    {"cpssDxChIpMultiTargetUcSchedModeSet",
        &wrCpssDxChIpMultiTargetUcSchedModeSet,
        5, 0},

    {"cpssDxChIpMultiTargetUcSchedModeGet",
        &wrCpssDxChIpMultiTargetUcSchedModeGet,
        1, 0},

    {"cpssDxChIpArpBcModeSet",
        &wrCpssDxChIpArpBcModeSet,
        2, 0},

    {"cpssDxChIpArpBcModeGet",
        &wrCpssDxChIpArpBcModeGet,
        1, 0},

    {"cpssDxChIpPortRoutingEnable",
        &wrCpssDxChIpPortRoutingEnable,
        5, 0},

    {"cpssDxChIpPortRoutingEnableGet",
        &wrCpssDxChIpPortRoutingEnableGet,
        4, 0},

    {"cpssDxChIpQosProfileToRouteEntryMapSet",
        &wrCpssDxChIpQosProfileToRouteEntryMapSet,
        3, 0},

    {"cpssDxChIpQosProfileToRouteEntryMapGet",
        &wrCpssDxChIpQosProfileToRouteEntryMapGet,
        2, 0},

    {"cpssDxChIpRoutingEnable",
        &wrCpssDxChIpRoutingEnable,
        2, 0},

    {"cpssDxChIpRoutingEnableGet",
        &wrCpssDxChIpRoutingEnableGet,
        1, 0},

    {"cpssDxChIpCntGetFirst",
        &wrCpssDxChIpCntGetFirst,
        1, 0},

    {"cpssDxChIpCntGetNext",
        &wrCpssDxChIpCntGetNext,
        1, 0},

    {"cpssDxChIpCntSetModeSet",
        &wrCpssDxChIpCntSetModeSet,
        1, 9},

    {"cpssDxChIpCntSetModeGetFirst",
        &wrCpssDxChIpCntSetModeGetFirst,
        1, 0},
      {"cpssDxChIpCntSetModeGetNext",
        &wrCpssDxChIpCntSetModeGetNext,
        1, 0},   

    {"cpssDxChIpCntSet",
        &wrCpssDxChIpCntSet,
        1, 11},

    {"cpssDxChIpMllSkippedEntriesCountersGet",
        &wrCpssDxChIpMllSkippedEntriesCountersGet,
        1, 0},

    {"cpssDxChIpMllCntInterfaceSet",
        &wrCpssDxChIpSetMllCntInterface,
        1, 8},

    {"cpssDxChIpMllCntGet",
        &wrCpssDxChIpMllCntGet,
        2, 0},

    {"cpssDxChIpMllCntSet",
        &wrCpssDxChIpMllCntSet,
        3, 0},

    {"cpssDxChIpMllSilentDropCntGet",
        &wrCpssDxChIpMllSilentDropCntGet,
        1, 0},

    {"cpssDxChIpDropCntSet",
        &wrCpssDxChIpDropCntSet,
        2, 0},

    {"cpssDxChIpSetDropCntMode",
        &wrCpssDxChIpSetDropCntMode,
        2, 0},

    {"cpssDxChIpDropCntGet",
        &wrCpssDxChIpDropCntGet,
        1, 0},

    {"cpssDxChIpMtuProfileSet",
        &wrCpssDxChIpMtuProfileSet,
        3, 0},

    {"cpssDxChIpMtuProfileGet",
        &wrCpssDxChIpMtuProfileGet,
        2, 0},

    {"cpssDxChIpv6AddrPrefixScopeSet",
        &wrCpssDxChIpv6AddrPrefixScopeSet,
        5, 0},

    {"cpssDxChIpv6AddrPrefixScopeGet",
        &wrCpssDxChIpv6AddrPrefixScopeGet,
        2, 0},

    {"cpssDxChIpv6AddrPrefixScopeGetFirst",
        &wrCpssDxChIpv6AddrPrefixScopeGetFirst,
        1, 0},

    {"cpssExMxPmIpv6AddrPrefixScopeGetNext",
        &wrCpssDxChIpv6AddrPrefixScopeGetNext,
        1, 0},

    {"cpssDxChIpv6UcScopeCommandSet",
        &wrCpssDxChIpv6UcScopeCommandSet,
        5, 0},

    {"cpssDxChIpv6UcScopeCommandGet",
        &wrCpssDxChIpv6UcScopeCommandGet,
        4, 0},

    {"cpssDxChIpv6McScopeCommandSet",
        &wrCpssDxChIpv6McScopeCommandSet,
        6, 0},

    {"cpssDxChIpv6McScopeCommandGet",
        &wrCpssDxChIpv6McScopeCommandGet,
        4, 0},

    {"cpssDxChIpRouterMacSaBaseSet",
        &wrCpssDxChIpRouterMacSaBaseSet,
        2, 0},

    {"cpssDxChIpRouterMacSaBaseGet",
        &wrCpssDxChIpRouterMacSaBaseGet,
        1, 0},

    {"cpssDxChIpRouterMacSaLsbModeSet",
        &wrCpssDxChIpRouterMacSaLsbModeSet,
        2, 0},

    {"cpssDxChIpRouterMacSaLsbModeGet",
        &wrCpssDxChIpRouterMacSaLsbModeGet,
        1, 0},


    {"cpssDxChIpRouterPortVlanMacSaLsbSet",
        &wrCpssDxChIpRouterPortVlanMacSaLsbSet,
        3, 0},

    {"cpssDxChIpRouterPortVlanMacSaLsbGet",
        &wrCpssDxChIpRouterPortVlanMacSaLsbGet,
        2, 0},

    {"cpssDxChIpRouterMacSaModifyEnable",
        &wrCpssDxChIpRouterMacSaModifyEnable,
        3, 0},

    {"cpssDxChIpRouterMacSaModifyEnableGet",
        &wrCpssDxChIpRouterMacSaModifyEnableGet,
        2, 0},

   {"cpssDxChIpPortRouterMacSaLsbModeSet",
        &wrCpssDxChIpPortRouterMacSaLsbModeSet,
        3, 0},
    {"cpssDxChIpPortRouterMacSaLsbModeGet",
        &wrCpssDxChIpPortRouterMacSaLsbModeGet,
        2, 0},
    {"cpssDxChIpRouterGlobalMacSaSet",
        &wrCpssDxChIpRouterGlobalMacSaSet,
        3, 0},
    {"cpssDxChIpRouterGlobalMacSaGet",
        &wrCpssDxChIpRouterGlobalMacSaGet,
        2, 0},
    {"cpssDxChIpRouterGlobalMacSaIndexSet",
        &wrCpssDxChIpRouterGlobalMacSaIndexSet,
        3, 0},
    {"cpssDxChIpRouterGlobalMacSaIndexGet",
        &wrCpssDxChIpRouterGlobalMacSaIndexGet,
        2, 0},

    {"cpssDxChIpRouterPortMacSaLsbSet",
        &wrCpssDxChIpRouterPortMacSaLsbSet,
        3, 0},
    {"cpssDxChIpRouterPortMacSaLsbGet",
        &wrCpssDxChIpRouterPortMacSaLsbGet,
        2, 0},

    {"cpssDxChIpRouterVlanMacSaLsbSet",
         &wrCpssDxChIpRouterVlanMacSaLsbSet,
         3, 0},
    {"cpssDxChIpRouterVlanMacSaLsbGet",
         &wrCpssDxChIpRouterVlanMacSaLsbGet,
         2, 0},
    {"cpssDxChIpEcmpUcRpfCheckEnableSet",
         &wrCpssDxChIpEcmpUcRpfCheckEnableSet,
         2, 0},
    {"cpssDxChIpEcmpUcRpfCheckEnableGet",
         &wrCpssDxChIpEcmpUcRpfCheckEnableGet,
         1, 0},
    {"cpssDxChIpExceptionCommandSetExt",
        &wrCpssDxChIpExceptionCommandSet,
        4, 0},

    {"cpssDxChIpUcRpfVlanModeSet",
    &wrCpssDxChIpUcRpfModeSet,
    3, 0},

    {"cpssDxChIpUcRpfVlanModeGet",
    &wrCpssDxChIpUcRpfModeGet,
    2, 0},

    {"cpssDxChIpUcRpfVlanModeSet_C001",
    &wrCpssDxChIpUcRpfModeSet,
    3, 0},

    {"cpssDxChIpUcRpfVlanModeGet_C001",
    &wrCpssDxChIpUcRpfModeGet,
    2, 0},

    {"cpssDxChIpPortSipSaEnableSet",
    &wrCpssDxChIpPortSipSaEnableSet,
    3, 0},

    {"cpssDxChIpPortSipSaEnableGet",
    &wrCpssDxChIpPortSipSaEnableGet,
    2, 0},
    {"cpssDxChIpRouterBridgedPacketsExceptionCntSet",
        &wrCpssDxChIpRouterBridgedPacketsExceptionCntSet,
        2, 0},
    {"cpssDxChIpRouterBridgedPacketsExceptionCntGet",
        &wrCpssDxChIpRouterBridgedPacketsExceptionCntGet,
        1, 0},
    {"cpssDxChIpTcamEccDaemonEnableSet",
         &wrCpssDxChIpTcamEccDaemonEnableSet,
         2, 0},
    {"cpssDxChIpTcamEccDaemonEnableGet",
         &wrCpssDxChIpTcamEccDaemonEnableGet,
         1, 0},
    {"cpssDxChIpRouterSourceIdOverrideEnableSet",
         &wrCpssDxChIpRouterSourceIdOverrideEnableSet,
         2, 0},
    {"cpssDxChIpRouterSourceIdOverrideEnableGet",
         &wrCpssDxChIpRouterSourceIdOverrideEnableGet,
         1, 0},

    {"cpssDxChIpHeaderErrorMaskSet",
        &wrCpssDxChIpHeaderErrorMaskSet,
        5, 0},

    {"cpssDxChIpHeaderErrorMaskGet",
        &wrCpssDxChIpHeaderErrorMaskGet,
        4, 0},

    {"cpssDxChIpUcRoutingVid1AssignEnableSet",
        &wrCpssDxChIpUcRoutingVid1AssignEnableSet,
        2, 0},

    {"cpssDxChIpUcRoutingVid1AssignEnableGet",
        &wrCpssDxChIpUcRoutingVid1AssignEnableGet,
        1, 0},

    {"cpssDxChIpMllMultiTargetShaperBaselineSet",
        &wrCpssDxChIpMllMultiTargetShaperBaselineSet,
        2, 0},
    {"cpssDxChIpMllMultiTargetShaperBaselineGet",
        &wrCpssDxChIpMllMultiTargetShaperBaselineGet,
        1, 0},
    {"cpssDxChIpMllMultiTargetShaperMtuSet",
        &wrCpssDxChIpMllMultiTargetShaperMtuSet,
        2, 0},
    {"cpssDxChIpMllMultiTargetShaperMtuGet",
        &wrCpssDxChIpMllMultiTargetShaperMtuGet,
        1, 0},

    {"cpssDxChIpMllMultiTargetShaperTokenBucketModeSet",
        &wrCpssDxChIpMllMultiTargetShaperTokenBucketModeSet,
        2, 0},
    {"cpssDxChIpMllMultiTargetShaperTokenBucketModeGet",
        &wrCpssDxChIpMllMultiTargetShaperTokenBucketModeGet,
        1, 0},

    {"cpssDxChIpMllMultiTargetShaperEnableSet",
        &wrCpssDxChIpMllMultiTargetShaperEnableSet,
        2, 0},
    {"cpssDxChIpMllMultiTargetShaperEnableGet",
        &wrCpssDxChIpMllMultiTargetShaperEnableGet,
        1, 0},
    {"cpssDxChIpMllMultiTargetShaperConfigurationSet",
        &wrCpssDxChIpMllMultiTargetShaperConfigurationSet,
        3, 0},
    {"cpssDxChIpMllMultiTargetShaperConfigurationGet",
        &wrCpssDxChIpMllMultiTargetShaperConfigurationGet,
        1, 0},
    {"cpssDxChIpMllMultiTargetShaperIntervalConfigSet",
        &wrCpssDxChIpMllMultiTargetShaperIntervalConfigurationSet,
        4, 0},
    {"cpssDxChIpMllMultiTargetShaperIntervalConfigGet",
        &wrCpssDxChIpMllMultiTargetShaperIntervalConfigurationGet,
        1, 0},

    {"cpssDxChIpPbrBypassRouterTriggerEnableSet",
        &wrCpssDxChIpPbrBypassRouterTriggerRequirementsEnableSet,
        2, 0},
    {"cpssDxChIpPbrBypassRouterTriggerEnableGet",
        &wrCpssDxChIpPbrBypassRouterTriggerRequirementsEnableGet,
        1, 0},

    {"cpssDxChIpVlanMrstBitmapSet",
        &wrCpssDxChIpVlanMrstBitmapSet,
        4, 0},
    {"cpssDxChIpVlanMrstBitmapGet",
        &wrCpssDxChIpVlanMrstBitmapGet,
        2, 0}

};

#define numCommands (sizeof(dbCommands) / sizeof(CMD_COMMAND))

/*******************************************************************************
* cmdLibInitCpssDxChIpCtrl
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
GT_STATUS cmdLibInitCpssDxChIpCtrl
(
    GT_VOID
)
{
    return cmdInitLibrary(dbCommands, numCommands);
}



