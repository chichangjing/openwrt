
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
* wrapCpssDxChNst.c
*
* DESCRIPTION:
*       Wrapper functions for Nst cpss.dxCh functions
*
* DEPENDENCIES:
*       None.
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
#include <cpss/dxCh/dxChxGen/nst/cpssDxChNst.h>

/*******************************************************************************
* nstMultiPortGroupsBmpGet
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
static void nstMultiPortGroupsBmpGet
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

/* Port Group aqnd Regular version wrapper     */
/* description see in original function header */
static GT_STATUS pg_wrap_cpssDxChNstIngressFrwFilterDropCntrSet
(
    IN  GT_U8       devNum,
    IN GT_U32       ingressCnt
)
{
    GT_BOOL             pgEnable; /* multi port group  enable */
    GT_PORT_GROUPS_BMP  pgBmp;    /* port group BMP           */

    nstMultiPortGroupsBmpGet(devNum, &pgEnable, &pgBmp);

    if (pgEnable == GT_FALSE)
    {
        return cpssDxChNstIngressFrwFilterDropCntrSet(devNum, ingressCnt);
    }
    else
    {
        return cpssDxChNstPortGroupIngressFrwFilterDropCntrSet(devNum, pgBmp, 
                                                               ingressCnt);
    }
}

/* Port Group aqnd Regular version wrapper     */
/* description see in original function header */
static GT_STATUS pg_wrap_cpssDxChNstIngressFrwFilterDropCntrGet
(
    IN  GT_U8       devNum,
    OUT GT_U32      *ingressCntPtr
)
{
    GT_BOOL             pgEnable; /* multi port group  enable */
    GT_PORT_GROUPS_BMP  pgBmp;    /* port group BMP           */

    nstMultiPortGroupsBmpGet(devNum, &pgEnable, &pgBmp);

    if (pgEnable == GT_FALSE)
    {
        return cpssDxChNstIngressFrwFilterDropCntrGet(devNum, ingressCntPtr);
    }
    else
    {
        return cpssDxChNstPortGroupIngressFrwFilterDropCntrGet(devNum, pgBmp, 
                                                               ingressCntPtr);
    }
}
 
/*******************************************************************************
* cpssDxChNstBridgeAccessMatrixCmdSet
*
* DESCRIPTION:
*     Set bridge access matrix entry.
*     The packet is assigned two access levels based on its MAC SA/SIP and MAC
*     DA/DIP. 
*     The device supports up to 8  SA and up to 8 DA levels.
*     The Access Matrix Configuration tables controls which access level 
*     pairs can communicate with each other. Based on the access level pair, 
*     the packet is assigned a command of: FORWARD, HARD DROP, and SOFT DROP.
*
* APPLICABLE DEVICES:  All DxCh2 devices
*
* INPUTS:
*      devNum         - physical device number
*      saAccessLevel  - Source address access level
*      daAccessLevel  - Destination address access level
*      command        - command assigned to a packet
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK              - on success.
*       GT_BAD_PARAM       - on wrong devNum, saAccessLevel, daAccessLevel. 
*       GT_HW_ERROR        - on hardware error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChNstBridgeAccessMatrixCmdSet

(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS               result;

    GT_U8                   devNum;
    GT_U32                  saAccessLevel;
    GT_U32                  daAccessLevel;
    CPSS_PACKET_CMD_ENT     command;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    saAccessLevel = (GT_U32)inArgs[1];
    daAccessLevel = (GT_U32)inArgs[2];
    command = (CPSS_PACKET_CMD_ENT)inArgs[3];

    /* call cpss api function */
    result = cpssDxChNstBridgeAccessMatrixCmdSet(devNum, saAccessLevel, 
                                                daAccessLevel, command);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}

/*******************************************************************************
* cpssDxChNstBridgeAccessMatrixCmdGet
*
* DESCRIPTION:
*     Get bridge access matrix entry.
*     The packet is assigned two access levels based on its MAC SA/SIP and MAC
*     DA/DIP. 
*     The device supports up to 8  SA and up to 8 DA levels.
*     The Access Matrix Configuration tables controls which access level 
*     pairs can communicate with each other. Based on the access level pair, 
*     the packet is assigned a command of: FORWARD, HARD DROP, and SOFT DROP.
*
* APPLICABLE DEVICES:  All DxCh2 devices
*
* INPUTS:
*      devNum         - physical device number
*      saAccessLevel  - Source address access level
*      daAccessLevel  - Destination address access level
*
* OUTPUTS:
*      commandPtr     - command assigned to a packet
*
* RETURNS:
*       GT_OK              - on success.
*       GT_BAD_PARAM       - on wrong devNum, saAccessLevel, daAccessLevel. 
*       GT_HW_ERROR        - on hardware error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChNstBridgeAccessMatrixCmdGet

(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS               result;

    GT_U8                   devNum;
    GT_U32                  saAccessLevel;
    GT_U32                  daAccessLevel;
    CPSS_PACKET_CMD_ENT     commandPtr;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    saAccessLevel = (GT_U32)inArgs[1];
    daAccessLevel = (GT_U32)inArgs[2];

    /* call cpss api function */
    result = cpssDxChNstBridgeAccessMatrixCmdGet(devNum, saAccessLevel, 
                                            daAccessLevel, &commandPtr);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", commandPtr);
    return CMD_OK;
}

/*******************************************************************************
* cpssDxChNstDefaultAccessLevelsSet
*
* DESCRIPTION:
*     Set default access levels for  Bridge Access Matrix Configuration table 
*     that controls which access level pairs can communicate with each other.
*
* APPLICABLE DEVICES:  All DxCh2 devices
*
* INPUTS:
*      devNum         - physical device number
*      paramType    -   type of bridge acess matrix global parameter to 
*                       configure default access level .
*      accessLevel  -  default security access level for parameter
* OUTPUTS:
*      None
*
* RETURNS:
*       GT_OK              - on success.
*       GT_BAD_PARAM       - on wrong devNum,paramType,accessLevel
*       GT_HW_ERROR        - on hardware error
*
* COMMENTS:
*      None
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChNstDefaultAccessLevelsSet

(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS               result;

    GT_U8                   devNum;
    CPSS_NST_AM_PARAM_ENT   paramType;
    GT_U32                  accessLevel;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    paramType = (CPSS_NST_AM_PARAM_ENT)inArgs[1];
    accessLevel = (GT_U32)inArgs[2];

    /* call cpss api function */
    result = cpssDxChNstDefaultAccessLevelsSet(devNum, paramType, accessLevel);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}

/*******************************************************************************
* cpssDxChNstDefaultAccessLevelsGet
*
* DESCRIPTION:
*     Get default access levels for  Bridge Access Matrix Configuration table 
*     that controls which access level pairs can communicate with each other.
*
* APPLICABLE DEVICES:  All DxCh2 devices
*
* INPUTS:
*      devNum         - physical device number
*      paramType    -   type of bridge acess matrix global parameter to 
*                       configure default access level .
*      accessLevel  -  default security access level for parameter
* OUTPUTS:
*      None
*
* RETURNS:
*       GT_OK              - on success.
*       GT_BAD_PARAM       - on wrong devNum,paramType
*       GT_HW_ERROR        - on hardware error
*
* COMMENTS:
*      None
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChNstDefaultAccessLevelsGet

(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS               result;

    GT_U8                   devNum;
    CPSS_NST_AM_PARAM_ENT   paramType;
    GT_U32                  accessLevelPtr;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    paramType = (CPSS_NST_AM_PARAM_ENT)inArgs[1];

    /* call cpss api function */
    result = cpssDxChNstDefaultAccessLevelsGet(devNum, paramType, 
                                                 &accessLevelPtr);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", accessLevelPtr);
    return CMD_OK;
}

/*******************************************************************************
* cpssDxChNstProtSanityCheckSet
*
* DESCRIPTION:
*      Set packet sanity checks.
*      Sanity Check engine identifies "suspicious" packets and 
*      provides an option for assigning them a Hard Drop packet command.
*
* APPLICABLE DEVICES:  All DxCh2 devices
*
* INPUTS:
*      devNum      - physical device number
*      checkType - the type of sanity check
*      enable    - GT_TRUE  enable packet's sanity check. Packet that not passed 
*                  check will be dropped and treated as security breach event.
*                - GT_FALSE disable packet's sanity check.
* OUTPUTS:
*      None
*
* RETURNS:
*       GT_OK              - on success.
*       GT_BAD_PARAM       - on wrong devNum,checkType
*       GT_HW_ERROR        - on hardware error
*
* COMMENTS:
*      None
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChNstProtSanityCheckSet

(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS               result;

    GT_U8                   devNum;
    CPSS_NST_CHECK_ENT      checkType;
    GT_BOOL                 enable;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    checkType = (CPSS_NST_CHECK_ENT)inArgs[1];
    enable = (GT_BOOL)inArgs[2];

    /* call cpss api function */
    result = cpssDxChNstProtSanityCheckSet(devNum, checkType, enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}

/*******************************************************************************
* cpssDxChNstProtSanityCheckGet
*
* DESCRIPTION:
*      Get packet sanity checks.
*      Sanity Check engine identifies "suspicious" packets and 
*      provides an option for assigning them a Hard Drop packet command.
*
* APPLICABLE DEVICES:  All DxCh2 devices
*
* INPUTS:
*      devNum      - physical device number
*      checkType - the type of sanity check
* OUTPUTS:
*      enablePtr - GT_TRUE  enable packet's sanity check. Packet that not passed 
*                  check will be dropped and treated as security breach event.
*                - GT_FALSE disable packet's sanity check.
*
* RETURNS:
*       GT_OK              - on success.
*       GT_BAD_PARAM       - on wrong devNum,checkType
*       GT_HW_ERROR        - on hardware error
*
* COMMENTS:
*      None
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChNstProtSanityCheckGet

(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS               result;

    GT_U8                   devNum;
    CPSS_NST_CHECK_ENT      checkType;
    GT_BOOL                 enablePtr;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    checkType = (CPSS_NST_CHECK_ENT)inArgs[1];

    /* call cpss api function */
    result = cpssDxChNstProtSanityCheckGet(devNum, checkType, &enablePtr);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", enablePtr);
    return CMD_OK;
}

/*******************************************************************************
* cpssDxChNstPortIngressFrwFilterSet
*
* DESCRIPTION:
*     Set port ingress forwarding filter.
*     For a given ingress port Enable/Disable traffic if it is destinied to:
*     CPU, ingress analyzer, network. 
*      
* APPLICABLE DEVICES:  All DxCh2 devices
*
* INPUTS:
*       devNum      - physical device number
*       port        - port number
*       filterType  - ingress filter type
*       enable      - GT_TRUE - enable ingress forwarding restiriction according
*                     to filterType, filterred packets will be dropped. 
*                   - GT_FALSE - disable ingress forwarding restiriction
*                     according to filterType
* OUTPUTS:
*      None
*
* RETURNS:
*       GT_OK              - on success.
*       GT_BAD_PARAM       - on wrong devNum, port, filterType
*       GT_HW_ERROR        - on hardware error
*
* COMMENTS:
*      None
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChNstPortIngressFrwFilterSet

(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                           result;

    GT_U8                               devNum;
    GT_PORT_NUM                               port;
    CPSS_NST_INGRESS_FRW_FILTER_ENT     filterType;
    GT_BOOL                             enable;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    port = (GT_PORT_NUM)inArgs[1];
    filterType = (CPSS_NST_INGRESS_FRW_FILTER_ENT)inArgs[2];
    enable = (GT_BOOL)inArgs[3];

    /* Override Device and Port */
    CONVERT_DEV_PORT_U32_MAC(devNum, port);

    /* call cpss api function */
    result = cpssDxChNstPortIngressFrwFilterSet(devNum, port, 
                                          filterType, enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}

/*******************************************************************************
* cpssDxChNstPortIngressFrwFilterGet
*
* DESCRIPTION:
*     Get port ingress forwarding filter.
*     For a given ingress port Enable/Disable traffic if it is destinied to:
*     CPU, ingress analyzer, network. 
*      
* APPLICABLE DEVICES:  All DxCh2 devices
*
* INPUTS:
*       devNum      - physical device number
*       port        - port number
*       filterType  - ingress filter type
* OUTPUTS:
*       enablePtr   - GT_TRUE - enable ingress forwarding restiriction according
*                     to filterType, filterred packets will be dropped. 
*                   - GT_FALSE - disable ingress forwarding restiriction
*                     according to filterType
*
* RETURNS:
*       GT_OK              - on success.
*       GT_BAD_PARAM       - on wrong devNum, port, filterType
*       GT_HW_ERROR        - on hardware error
*
* COMMENTS:
*      None
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChNstPortIngressFrwFilterGet

(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                           result;

    GT_U8                               devNum;
    GT_PORT_NUM                               port;
    CPSS_NST_INGRESS_FRW_FILTER_ENT     filterType;
    GT_BOOL                             enablePtr;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    port = (GT_PORT_NUM)inArgs[1];
    filterType = (CPSS_NST_INGRESS_FRW_FILTER_ENT)inArgs[2];

    /* Override Device and Port */
    CONVERT_DEV_PORT_U32_MAC(devNum, port);

    /* call cpss api function */
    result = cpssDxChNstPortIngressFrwFilterGet(devNum, port, 
                                      filterType, &enablePtr);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", enablePtr);
    return CMD_OK;
}

/*******************************************************************************
* cpssDxChNstPortEgressFrwFilterSet
*
* DESCRIPTION:
*     Set port egress forwarding filter.
*     For a given egress port Enable/Disable traffic if the packet was:
*     sent from CPU with FROM_CPU DSA tag, bridged or policy switched, 
*     routed or policy routed. 
*     
* APPLICABLE DEVICES:  All DxCh2 devices
*
* INPUTS:
*       devNum      - physical device number
*       port        - port number
*       filterType  - egress filter type
*       enable      - GT_TRUE - enable egress forwarding restiriction according 
*                     to filterType, filterred packets will be dropped.  
*                   - GT_FALSE - disable egress forwarding restiriction 
*                     according to filterType.
* OUTPUTS:
*      None
*
* RETURNS:
*       GT_OK              - on success.
*       GT_BAD_PARAM       - on wrong devNum, port, filterType
*       GT_HW_ERROR        - on hardware error
*
* COMMENTS:
*      None
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChNstPortEgressFrwFilterSet

(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                           result;

    GT_U8                               devNum;
    GT_PORT_NUM                               port;
    CPSS_NST_EGRESS_FRW_FILTER_ENT      filterType;
    GT_BOOL                             enable;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    port = (GT_PORT_NUM)inArgs[1];
    filterType = (CPSS_NST_EGRESS_FRW_FILTER_ENT)inArgs[2];
    enable = (GT_BOOL)inArgs[3];

    /* Override Device and Port */
    CONVERT_DEV_PORT_U32_MAC(devNum, port);

    /* call cpss api function */
    result = cpssDxChNstPortEgressFrwFilterSet(devNum, port, 
                                         filterType, enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}

/*******************************************************************************
* cpssDxChNstPortEgressFrwFilterGet
*
* DESCRIPTION:
*     Get port egress forwarding filter.
*     For a given egress port Enable/Disable traffic if the packet was:
*     sent from CPU with FROM_CPU DSA tag, bridged or policy switched, 
*     routed or policy routed. 
*     
* APPLICABLE DEVICES:  All DxCh2 devices
*
* INPUTS:
*       devNum      - physical device number
*       port        - port number
*       filterType  - egress filter type
* OUTPUTS:
*       enablePtr   - GT_TRUE - enable egress forwarding restiriction according 
*                     to filterType, filterred packets will be dropped.  
*                   - GT_FALSE - disable egress forwarding restiriction 
*                     according to filterType.
*
* RETURNS:
*       GT_OK              - on success.
*       GT_BAD_PARAM       - on wrong devNum, port, filterType
*       GT_HW_ERROR        - on hardware error
*
* COMMENTS:
*      None
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChNstPortEgressFrwFilterGet

(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                           result;

    GT_U8                               devNum;
    GT_PORT_NUM                               port;
    CPSS_NST_EGRESS_FRW_FILTER_ENT      filterType;
    GT_BOOL                             enablePtr;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    port = (GT_PORT_NUM)inArgs[1];
    filterType = (CPSS_NST_EGRESS_FRW_FILTER_ENT)inArgs[2];

    /* Override Device and Port */
    CONVERT_DEV_PORT_U32_MAC(devNum, port);

    /* call cpss api function */
    result = cpssDxChNstPortEgressFrwFilterGet(devNum, port, 
                                     filterType, &enablePtr);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", enablePtr);
    return CMD_OK;
}

/*******************************************************************************
* cpssDxChNstIngressFrwFilterDropCntrSet
*
* DESCRIPTION:
*     Set the global ingress forwarding restriction drop packet counter.
*     This counter counts the number of packets dropped due to Ingress forward
*     restrictions.
*
* APPLICABLE DEVICES:  All DxCh2 devices
*
* INPUTS:
*       devNum      - physical device number
*       ingressCnt  - the counter value
*
* OUTPUTS:
*
* RETURNS:
*       GT_OK              - on success.
*       GT_BAD_PARAM       - on wrong devNum
*       GT_HW_ERROR        - on hardware error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChNstIngressFrwFilterDropCntrSet

(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                           result;

    GT_U8                               devNum;
    GT_U32                              ingressCnt;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    ingressCnt = (GT_U32)inArgs[1];

    /* call cpss api function */
    result = pg_wrap_cpssDxChNstIngressFrwFilterDropCntrSet(devNum, ingressCnt);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}

/*******************************************************************************
* cpssDxChNstIngressFrwFilterDropCntrGet
*
* DESCRIPTION:
*     Reads the global ingress forwarding restriction drop packet counter.
*     This counter counts the number of packets dropped due to Ingress forward
*     restrictions.
*
* APPLICABLE DEVICES:  All DxCh2 devices
*
* INPUTS:
*       devNum      - physical device number
*
* OUTPUTS:
*      ingressCntPtr - the counter value
*
* RETURNS:
*       GT_OK              - on success.
*       GT_BAD_PARAM       - on wrong devNum
*       GT_HW_ERROR        - on hardware error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChNstIngressFrwFilterDropCntrGet

(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                           result;

    GT_U8                               devNum;
    GT_U32                              ingressCntPtr;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];

    /* call cpss api function */
    result = pg_wrap_cpssDxChNstIngressFrwFilterDropCntrGet(devNum,
                                                            &ingressCntPtr);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", ingressCntPtr);
    return CMD_OK;
}

/*******************************************************************************
* cpssDxChNstRouterAccessMatrixCmdSet
*
* DESCRIPTION:
*     Set Routers access matrix entry.
*     The packet is assigned two access levels based on its SIP and DIP. 
*     The device supports up to 8  SIP and up to 8 DIP levels.
*     The Access Matrix Configuration tables controls which access level 
*     pairs can communicate with each other. Based on the access level pair, 
*     the packet is assigned a command of: FORWARD, HARD DROP, and SOFT DROP.
*
* APPLICABLE DEVICES:  All DxCh2 devices
*
* INPUTS:
*      devNum          - physical device number
*      sipAccessLevel  - the sip access level
*      dipAccessLevel  - the dip access level
*      command         - command assigned to a packet
*
* OUTPUTS:
*
* RETURNS:
*       GT_OK              - on success.
*       GT_BAD_PARAM       - on wrong devNum, saAccessLevel, daAccessLevel. 
*       GT_HW_ERROR        - on hardware error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChNstRouterAccessMatrixCmdSet

(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                           result;

    GT_U8                               devNum;
    GT_U32                              sipAccessLevel;
    GT_U32                              dipAccessLevel;
    CPSS_PACKET_CMD_ENT                 command;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    sipAccessLevel = (GT_U32)inArgs[1];
    dipAccessLevel = (GT_U32)inArgs[2];
    command = (CPSS_PACKET_CMD_ENT)inArgs[3];

    /* call cpss api function */
    result = cpssDxChNstRouterAccessMatrixCmdSet(devNum, sipAccessLevel, 
                                                dipAccessLevel, command);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}

/*******************************************************************************
* cpssDxChNstRouterAccessMatrixCmdGet
*
* DESCRIPTION:
*     Get Routers access matrix entry.
*     The packet is assigned two access levels based on its SIP and DIP. 
*     The device supports up to 8  SIP and up to 8 DIP levels.
*     The Access Matrix Configuration tables controls which access level 
*     pairs can communicate with each other. Based on the access level pair, 
*     the packet is assigned a command of: FORWARD, HARD DROP, and SOFT DROP.
*
* APPLICABLE DEVICES:  All DxCh2 devices
*
* INPUTS:
*      devNum         - physical device number
*      sipAccessLevel  - the sip access level
*      dipAccessLevel  - the dip access level
*
* OUTPUTS:
*      commandPtr     - command assigned to a packet
*
* RETURNS:
*       GT_OK              - on success.
*       GT_BAD_PARAM       - on wrong devNum, saAccessLevel, daAccessLevel. 
*       GT_HW_ERROR        - on hardware error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChNstRouterAccessMatrixCmdGet

(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                           result;

    GT_U8                               devNum;
    GT_U32                              sipAccessLevel;
    GT_U32                              dipAccessLevel;
    CPSS_PACKET_CMD_ENT                 commandPtr;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    sipAccessLevel = (GT_U32)inArgs[1];
    dipAccessLevel = (GT_U32)inArgs[2];

    /* call cpss api function */
    result = cpssDxChNstRouterAccessMatrixCmdGet(devNum, sipAccessLevel, 
                                            dipAccessLevel, &commandPtr);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", commandPtr);
    return CMD_OK;
}


/**** database initialization **************************************/

static CMD_COMMAND dbCommands[] =
{
    {"cpssDxChNstBridgeAccessMatrixCmdSet",
        &wrCpssDxChNstBridgeAccessMatrixCmdSet,
        4, 0},

    {"cpssDxChNstBridgeAccessMatrixCmdGet",
        &wrCpssDxChNstBridgeAccessMatrixCmdGet,
        3, 0},

    {"cpssDxChNstDefaultAccessLevelsSet",
        &wrCpssDxChNstDefaultAccessLevelsSet,
        3, 0},

    {"cpssDxChNstDefaultAccessLevelsGet",
        &wrCpssDxChNstDefaultAccessLevelsGet,
        2, 0},

    {"cpssDxChNstProtSanityCheckSet",
        &wrCpssDxChNstProtSanityCheckSet,
        3, 0},

    {"cpssDxChNstProtSanityCheckGet",
        &wrCpssDxChNstProtSanityCheckGet,
        2, 0},

    {"cpssDxChNstPortIngressFrwFilterSet",
        &wrCpssDxChNstPortIngressFrwFilterSet,
        4, 0},

    {"cpssDxChNstPortIngressFrwFilterGet",
        &wrCpssDxChNstPortIngressFrwFilterGet,
        3, 0},

    {"cpssDxChNstPortEgressFrwFilterSet",
        &wrCpssDxChNstPortEgressFrwFilterSet,
        4, 0},

    {"cpssDxChNstPortEgressFrwFilterGet",
        &wrCpssDxChNstPortEgressFrwFilterGet,
        3, 0},

    {"cpssDxChNstIngressFrwFilterDropCntrSet",
        &wrCpssDxChNstIngressFrwFilterDropCntrSet,
        2, 0},

    {"cpssDxChNstIngressFrwFilterDropCntrGet",
        &wrCpssDxChNstIngressFrwFilterDropCntrGet,
        1, 0},

    {"cpssDxChNstRouterAccessMatrixCmdSet",
        &wrCpssDxChNstRouterAccessMatrixCmdSet,
        4, 0},

    {"cpssDxChNstRouterAccessMatrixCmdGet",
        &wrCpssDxChNstRouterAccessMatrixCmdGet,
        3, 0}

};

#define numCommands (sizeof(dbCommands) / sizeof(CMD_COMMAND))

/*******************************************************************************
* cmdLibInitCpssDxChNst
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
GT_STATUS cmdLibInitCpssDxChNst
(
    GT_VOID
)
{
    return cmdInitLibrary(dbCommands, numCommands);
}


