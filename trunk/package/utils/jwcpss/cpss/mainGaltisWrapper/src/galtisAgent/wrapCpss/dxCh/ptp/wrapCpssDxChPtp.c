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
* wrapCpssDxChPtp.c
*
* DESCRIPTION:
*       Wrapper functions for PTP cpss.dxCh functions
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
#include <cpss/dxCh/dxChxGen/ptp/cpssDxChPtp.h>

/* support for multi port groups */

/*******************************************************************************
* ptpMultiPortGroupsBmpGet
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
static void ptpMultiPortGroupsBmpGet
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

static GT_STATUS pg_wrap_cpssDxChPtpTodCounterFunctionSet
(
    IN GT_U8                                    devNum,
    IN CPSS_PORT_DIRECTION_ENT                  direction,
    IN CPSS_DXCH_PTP_TOD_COUNTER_FUNC_ENT       function
)
{
    GT_BOOL             pgEnable; /* multi port group  enable */
    GT_PORT_GROUPS_BMP  pgBmp;    /* port group BMP           */
    CPSS_DXCH_PTP_TAI_ID_STC   taiId =
        {CPSS_DXCH_PTP_TAI_INSTANCE_GLOBAL_E, CPSS_DXCH_PTP_TAI_NUMBER_0_E, 0};

    ptpMultiPortGroupsBmpGet(devNum, &pgEnable, &pgBmp);

    if (pgEnable == GT_FALSE)
    {
        return  cpssDxChPtpTodCounterFunctionSet(devNum, direction, &taiId, function);
    }
    else
    {
        return  cpssDxChPtpPortGroupTodCounterFunctionSet(devNum, pgBmp,
                                                          direction, function);
    }
}


static GT_STATUS pg_wrap_cpssDxChPtpTodCounterFunctionGet
(
    IN  GT_U8                                   devNum,
    IN  CPSS_PORT_DIRECTION_ENT                 direction,
    OUT CPSS_DXCH_PTP_TOD_COUNTER_FUNC_ENT      *functionPtr
)
{
    GT_BOOL             pgEnable; /* multi port group  enable */
    GT_PORT_GROUPS_BMP  pgBmp;    /* port group BMP           */
    CPSS_DXCH_PTP_TAI_ID_STC   taiId =
        {CPSS_DXCH_PTP_TAI_INSTANCE_GLOBAL_E, CPSS_DXCH_PTP_TAI_NUMBER_0_E, 0};

    ptpMultiPortGroupsBmpGet(devNum, &pgEnable, &pgBmp);

    if (pgEnable == GT_FALSE)
    {
        return  cpssDxChPtpTodCounterFunctionGet(devNum, direction, &taiId, functionPtr);
    }
    else
    {
        return  cpssDxChPtpPortGroupTodCounterFunctionGet(devNum, pgBmp,
                                                          direction, functionPtr);
    }
}


static GT_STATUS pg_wrap_cpssDxChPtpTodCounterFunctionTriggerSet
(
    IN GT_U8                    devNum,
    IN CPSS_PORT_DIRECTION_ENT  direction
)
{
    GT_BOOL             pgEnable; /* multi port group  enable */
    GT_PORT_GROUPS_BMP  pgBmp;    /* port group BMP           */
    CPSS_DXCH_PTP_TAI_ID_STC   taiId =
        {CPSS_DXCH_PTP_TAI_INSTANCE_GLOBAL_E, CPSS_DXCH_PTP_TAI_NUMBER_0_E, 0};

    ptpMultiPortGroupsBmpGet(devNum, &pgEnable, &pgBmp);

    if (pgEnable == GT_FALSE)
    {
        return  cpssDxChPtpTodCounterFunctionTriggerSet(devNum, direction, &taiId);
    }
    else
    {
        return  cpssDxChPtpPortGroupTodCounterFunctionTriggerSet(devNum, pgBmp,
                                                                 direction);
    }
}


static GT_STATUS pg_wrap_cpssDxChPtpTodCounterFunctionTriggerGet
(
    IN GT_U8                    devNum,
    IN CPSS_PORT_DIRECTION_ENT  direction,
    OUT GT_BOOL                 *enablePtr
)
{
    GT_BOOL             pgEnable; /* multi port group  enable */
    GT_PORT_GROUPS_BMP  pgBmp;    /* port group BMP           */
    CPSS_DXCH_PTP_TAI_ID_STC   taiId =
        {CPSS_DXCH_PTP_TAI_INSTANCE_GLOBAL_E, CPSS_DXCH_PTP_TAI_NUMBER_0_E, 0};

    ptpMultiPortGroupsBmpGet(devNum, &pgEnable, &pgBmp);

    if (pgEnable == GT_FALSE)
    {
        return  cpssDxChPtpTodCounterFunctionTriggerGet(devNum, direction,
                                                        &taiId, enablePtr);
    }
    else
    {
        return  cpssDxChPtpPortGroupTodCounterFunctionTriggerGet(devNum, pgBmp,
                                                                 direction,
                                                                 enablePtr);
    }
}


static GT_STATUS pg_wrap_cpssDxChPtpTodCounterShadowSet
(
    IN GT_U8                        devNum,
    IN CPSS_PORT_DIRECTION_ENT      direction,
    IN CPSS_DXCH_PTP_TOD_COUNT_STC  *todCounterPtr
)
{
    GT_BOOL             pgEnable; /* multi port group  enable */
    GT_PORT_GROUPS_BMP  pgBmp;    /* port group BMP           */

    ptpMultiPortGroupsBmpGet(devNum, &pgEnable, &pgBmp);

    if (pgEnable == GT_FALSE)
    {
        return  cpssDxChPtpTodCounterShadowSet(devNum, direction, todCounterPtr);
    }
    else
    {
        return  cpssDxChPtpPortGroupTodCounterShadowSet(devNum, pgBmp, direction,
                                                        todCounterPtr);
    }
}


static GT_STATUS pg_wrap_cpssDxChPtpTodCounterShadowGet
(
    IN  GT_U8                           devNum,
    IN  CPSS_PORT_DIRECTION_ENT         direction,
    OUT CPSS_DXCH_PTP_TOD_COUNT_STC     *todCounterPtr
)
{
    GT_BOOL             pgEnable; /* multi port group  enable */
    GT_PORT_GROUPS_BMP  pgBmp;    /* port group BMP           */

    ptpMultiPortGroupsBmpGet(devNum, &pgEnable, &pgBmp);

    if (pgEnable == GT_FALSE)
    {
        return  cpssDxChPtpTodCounterShadowGet(devNum, direction, todCounterPtr);
    }
    else
    {
        return  cpssDxChPtpPortGroupTodCounterShadowGet(devNum, pgBmp, direction,
                                                        todCounterPtr);
    }
}

static GT_STATUS pg_wrap_cpssDxChPtpTodCounterGet
(
    IN GT_U8                            devNum,
    IN CPSS_PORT_DIRECTION_ENT          direction,
    OUT CPSS_DXCH_PTP_TOD_COUNT_STC     *todCounterPtr
)
{

    GT_BOOL             pgEnable; /* multi port group  enable */
    GT_PORT_GROUPS_BMP  pgBmp;    /* port group BMP           */

    ptpMultiPortGroupsBmpGet(devNum, &pgEnable, &pgBmp);

    if (pgEnable == GT_FALSE)
    {
        return  cpssDxChPtpTodCounterGet(devNum, direction, todCounterPtr);
    }
    else
    {
        return  cpssDxChPtpPortGroupTodCounterGet(devNum, pgBmp, direction,
                                                  todCounterPtr);
    }
}


/*******************************************************************************
* cpssDxChPtpEtherTypeSet
*
* DESCRIPTION:
*       Configure ethertype0/ethertype1 of PTP over Ethernet packets.
*       A packet is identified as PTP over Ethernet if its EtherType matches
*       one of the configured values.
*
* APPLICABLE DEVICES:  Lion and above.
*
* INPUTS:
*       devNum          - device number
*       etherTypeIndex  - PTP EtherType index, values 0..1
*       etherType       - PTP EtherType0 or EtherType1 according to the index,
*                         values 0..0xFFFF
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong devNum or etherTypeIndex
*       GT_OUT_OF_RANGE          - on wrong etherType
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChPtpEtherTypeSet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS   result;
    GT_U8       devNum;
    GT_U32      etherTypeIndex;
    GT_U32      etherType;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    etherTypeIndex = (GT_U32)inArgs[1];
    etherType = (GT_U32)inArgs[2];

    /* call cpss api function */
    result = cpssDxChPtpEtherTypeSet(devNum, etherTypeIndex, etherType);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChPtpEtherTypeGet
*
* DESCRIPTION:
*       Get ethertypes of PTP over Ethernet packets.
*
* APPLICABLE DEVICES:  Lion and above.
*
* INPUTS:
*       devNum          - device number
*       etherTypeIndex  - PTP EtherType index, values 0..1
*
* OUTPUTS:
*       etherTypePtr    - (pointer to) PTP EtherType0 or EtherType1 according to
*                          the index
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong devNum or etherTypeIndex
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChPtpEtherTypeGet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS   result;
    GT_U8       devNum;
    GT_U32      etherTypeIndex;
    GT_U32      etherType;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    etherTypeIndex = (GT_U32)inArgs[1];

    /* call cpss api function */
    result = cpssDxChPtpEtherTypeGet(devNum, etherTypeIndex, &etherType);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", etherType);

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChPtpUdpDestPortSet
*
* DESCRIPTION:
*       Configure UDP destination port0/port1 of PTP over UDP packets.
*       A packet is identified as PTP over UDP if it is a UDP packet, whose
*       destination port matches one of the configured ports.
*
* APPLICABLE DEVICES:  Lion and above.
*
* INPUTS:
*       devNum          - device number
*       udpPortIndex    - UDP port index , values 0..1
*       udpPortNum      - UDP port1/port0 number according to the index,
*                         values 0..0xFFFF
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong devNum or udpPortIndex
*       GT_OUT_OF_RANGE          - on wrong udpPortNum
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChPtpUdpDestPortsSet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS   result;
    GT_U8       devNum;
    GT_U32      udpPortIndex;
    GT_U32      udpPortNum;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    udpPortIndex = (GT_U32)inArgs[1];
    udpPortNum = (GT_U32)inArgs[2];

    /* call cpss api function */
    result = cpssDxChPtpUdpDestPortsSet(devNum, udpPortIndex, udpPortNum);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChPtpUdpDestPortGet
*
* DESCRIPTION:
*       Get UDP destination port0/port1 of PTP over UDP packets.
*
* APPLICABLE DEVICES:  Lion and above.
*
* INPUTS:
*       devNum          - device number
*       udpPortIndex    - UDP port index , values 0..1
*
* OUTPUTS:
*       udpPortNumPtr     - (pointer to) UDP port0/port1 number, according to
*                           the index
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong devNum or udpPortIndex
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChPtpUdpDestPortsGet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS   result;
    GT_U8       devNum;
    GT_U32      udpPortIndex;
    GT_U32      udpPortNum;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    udpPortIndex = (GT_U32)inArgs[1];

    /* call cpss api function */
    result = cpssDxChPtpUdpDestPortsGet(devNum, udpPortIndex, &udpPortNum);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", udpPortNum);

    return CMD_OK;
}


/*******************************************************************************
* cpssDxChPtpMessageTypeCmdSet
*
* DESCRIPTION:
*       Configure packet command per PTP message type.
*       The message type is extracted from the PTP header.
*
* APPLICABLE DEVICES:  Lion and above.
*
* INPUTS:
*       devNum          - device number
*       messageType     - message type, values 0..15
*       command         - assigned command to the packet.
*                         Acceptable values: FORWARD, MIRROR, TRAP, HARD_DROP and
*                                            SOFT_DROP.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong devNum, messageType or command
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChPtpMessageTypeCmdSet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS               result;
    GT_U8                   devNum;
    GT_PHYSICAL_PORT_NUM    portNum;
    GT_U32                  messageType;
    CPSS_PACKET_CMD_ENT     command;
    GT_U32                  domainNum;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    portNum = (GT_PHYSICAL_PORT_NUM)inArgs[1];
    domainNum = (GT_U32)inArgs[2];
    messageType = (GT_U32)inArgs[3];
    command = (CPSS_PACKET_CMD_ENT)inArgs[4];

    /* Override Device and Port */
    CONVERT_DEV_PHYSICAL_PORT_MAC(devNum, portNum);

    /* call cpss api function */
    result = cpssDxChPtpMessageTypeCmdSet(
        devNum, portNum, domainNum, messageType, command);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChPtpMessageTypeCmdGet
*
* DESCRIPTION:
*       Get packet command per PTP message type.
*
* APPLICABLE DEVICES:  Lion and above.
*
* INPUTS:
*       devNum          - device number
*       messageType     - message type, values 0..15.
*
* OUTPUTS:
*       commandPtr      - (pointer to) assigned command to the packet.
*                         Acceptable values: FORWARD, MIRROR, TRAP, HARD_DROP and
*                                            SOFT_DROP.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong devNum or messageType
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_BAD_STATE             - on bad value found in HW
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChPtpMessageTypeCmdGet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS               result;
    GT_U8                   devNum;
    GT_PHYSICAL_PORT_NUM    portNum;
    GT_U32                  messageType;
    CPSS_PACKET_CMD_ENT     command;
    GT_U32                  domainNum;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    portNum = (GT_PHYSICAL_PORT_NUM)inArgs[1];
    domainNum = (GT_U32)inArgs[2];
    messageType = (GT_U32)inArgs[3];

    /* Override Device and Port */
    CONVERT_DEV_PHYSICAL_PORT_MAC(devNum, portNum);

    /* call cpss api function */
    result = cpssDxChPtpMessageTypeCmdGet(
        devNum, portNum, domainNum, messageType, &command);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", command);

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChPtpCpuCodeBaseSet
*
* DESCRIPTION:
*       Set CPU code base assigned to trapped and mirrored PTP packets.
*       A PTP packet's CPU code is computed as base CPU code + messageType
*       (where message type is taken from PTP header).
*
* APPLICABLE DEVICES:  Lion and above.
*
* INPUTS:
*       devNum          - device number
*       cpuCode         - The base of CPU code assigned to PTP packets mirrored
*                         or trapped to CPU.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong devNum or cpuCode
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChPtpCpuCodeBaseSet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                   result;
    GT_U8                       devNum;
    CPSS_NET_RX_CPU_CODE_ENT    cpuCode;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    cpuCode = (CPSS_NET_RX_CPU_CODE_ENT)inArgs[1];

    /* call cpss api function */
    result = cpssDxChPtpCpuCodeBaseSet(devNum, cpuCode);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChPtpCpuCodeBaseGet
*
* DESCRIPTION:
*       Get CPU code base assigned to trapped and mirrored PTP packets.
*       A PTP packet's CPU code is computed as base CPU code + messageType
*       (where message type is taken from PTP header).
*
* APPLICABLE DEVICES:  Lion and above.
*
* INPUTS:
*       devNum          - device number
*
* OUTPUTS:
*       cpuCodePtr      - (pointer to) The base of CPU code assigned to PTP
*                         packets mirrored or trapped to CPU.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong devNum
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChPtpCpuCodeBaseGet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                   result;
    GT_U8                       devNum;
    CPSS_NET_RX_CPU_CODE_ENT    cpuCode;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];

    /* call cpss api function */
    result = cpssDxChPtpCpuCodeBaseGet(devNum, &cpuCode);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", cpuCode);

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChPtpTodCounterFunctionSet
*
* DESCRIPTION:
*       The function configures type of TOD counter action that will be
*       performed once triggered by cpssDxChPtpTodCounterFunctionTriggerSet()
*       API or by pulse from an external interface.
*
* APPLICABLE DEVICES:  Lion and above.
*
* INPUTS:
*       devNum          - device number
*       direction       - Tx, Rx or both directions
*       function        - One of the four possible TOD counter functions
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong devNum or direction
*                                  or function
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChPtpTodCounterFunctionSet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                           result;
    GT_U8                               devNum;
    CPSS_PORT_DIRECTION_ENT             direction;
    CPSS_DXCH_PTP_TOD_COUNTER_FUNC_ENT  function;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    direction = (CPSS_PORT_DIRECTION_ENT)inArgs[1];
    function = (CPSS_DXCH_PTP_TOD_COUNTER_FUNC_ENT)inArgs[2];

    /* call cpss api function */
    result = pg_wrap_cpssDxChPtpTodCounterFunctionSet(devNum, direction,
                                                      function);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChPtpTodCounterFunctionGet
*
* DESCRIPTION:
*       Get type of TOD counter action that will be performed once triggered by
*       cpssDxChPtpTodCounterFunctionTriggerSet() API or by pulse from an
*       external interface.
*
* APPLICABLE DEVICES:  Lion and above.
*
* INPUTS:
*       devNum          - device number
*       direction       - Tx or Rx
*
* OUTPUTS:
*       functionPtr     - (pointer to) One of the four possible TOD counter
*                          functions
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong devNum or direction
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_BAD_STATE             - on bad value found in HW
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChPtpTodCounterFunctionGet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                           result;
    GT_U8                               devNum;
    CPSS_PORT_DIRECTION_ENT             direction;
    CPSS_DXCH_PTP_TOD_COUNTER_FUNC_ENT  function;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    direction = (CPSS_PORT_DIRECTION_ENT)inArgs[1];

    /* call cpss api function */
    result = pg_wrap_cpssDxChPtpTodCounterFunctionGet(devNum, direction, &function);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", function);

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChPtpTodCounterFunctionTriggerSet
*
* DESCRIPTION:
*       Trigger TOD (Time of Day) counter function accorging to the function set
*       by cpssDxChPtpTodCounterFunctionSet().
*
* APPLICABLE DEVICES:  Lion and above.
*
* INPUTS:
*       devNum          - device number
*       direction       - Tx, Rx or both directions
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_STATE             - when previous TOD triggered action is not finished yet
*       GT_BAD_PARAM             - on wrong devNum or direction
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChPtpTodCounterFunctionTriggerSet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                   result;
    GT_U8                       devNum;
    CPSS_PORT_DIRECTION_ENT     direction;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    direction = (CPSS_PORT_DIRECTION_ENT)inArgs[1];

    /* call cpss api function */
    result = pg_wrap_cpssDxChPtpTodCounterFunctionTriggerSet(devNum, direction);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChPtpTodCounterFunctionTriggerGet
*
* DESCRIPTION:
*       Get status of trigger TOD (Time of Day) counter function.
*
* APPLICABLE DEVICES:  Lion and above.
*
* INPUTS:
*       devNum          - device number
*       direction       - Tx or Rx
*
* OUTPUTS:
*       enablePtr       - (pointer to) GT_TRUE - The trigger is on, the TOD
*                                               function is not finished.
*                         GT_FALSE - The trigger is off, the TOD function is
*                                   finished.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong devNum or direction
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChPtpTodCounterFunctionTriggerGet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                   result;
    GT_U8                       devNum;
    CPSS_PORT_DIRECTION_ENT     direction;
    GT_BOOL                     enable;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    direction = (CPSS_PORT_DIRECTION_ENT)inArgs[1];

    /* call cpss api function */
    result = pg_wrap_cpssDxChPtpTodCounterFunctionTriggerGet(devNum, direction,
                                                             &enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", enable);

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChPtpTodCounterShadowSet
*
* DESCRIPTION:
*       Configure TOD timestamping shadow counter values.
*
* APPLICABLE DEVICES:  Lion and above.
*
* INPUTS:
*       devNum          - device number
*       direction       - Tx, Rx or both directions
*       todCounterPtr   - pointer to TOD counter shadow
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong devNum or direction
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChPtpTodCounterShadowSet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                   result;
    GT_U8                       devNum;
    CPSS_PORT_DIRECTION_ENT     direction;
    CPSS_DXCH_PTP_TOD_COUNT_STC todCounter;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    direction = (CPSS_PORT_DIRECTION_ENT)inArgs[1];
    todCounter.nanoSeconds = (GT_U32)inArgs[2];
    todCounter.seconds.l[0] = (GT_U32)inArgs[3];
    todCounter.seconds.l[1] = (GT_U32)inArgs[4];

    /* call cpss api function */
    result = pg_wrap_cpssDxChPtpTodCounterShadowSet(devNum, direction,
                                                    &todCounter);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChPtpTodCounterShadowGet
*
* DESCRIPTION:
*       Get TOD timestamping shadow counter values.
*
* APPLICABLE DEVICES:  Lion and above.
*
* INPUTS:
*       devNum          - device number
*       direction       - Tx or Rx
*
* OUTPUTS:
*       todCounterPtr      - (pointer to) TOD counter shadow
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong devNum or direction
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChPtpTodCounterShadowGet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                   result;
    GT_U8                       devNum;
    CPSS_PORT_DIRECTION_ENT     direction;
    CPSS_DXCH_PTP_TOD_COUNT_STC todCounter;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    direction = (CPSS_PORT_DIRECTION_ENT)inArgs[1];

    /* call cpss api function */
    result = pg_wrap_cpssDxChPtpTodCounterShadowGet(devNum, direction,
                                                    &todCounter);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%X%X%X", todCounter.nanoSeconds,
                 todCounter.seconds.l[0], todCounter.seconds.l[1]);

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChPtpTodCounterGet
*
* DESCRIPTION:
*       Get TOD timestamping counter value.
*
* APPLICABLE DEVICES:  Lion and above.
*
* INPUTS:
*       devNum          - device number
*       direction       - Tx or Rx
*
* OUTPUTS:
*       todCounterPtr      - (pointer to) TOD counter
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong devNum or direction
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChPtpTodCounterGet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                   result;
    GT_U8                       devNum;
    CPSS_PORT_DIRECTION_ENT     direction;
    CPSS_DXCH_PTP_TOD_COUNT_STC todCounter;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    direction = (CPSS_PORT_DIRECTION_ENT)inArgs[1];

    /* call cpss api function */
    result = pg_wrap_cpssDxChPtpTodCounterGet(devNum, direction, &todCounter);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%X%X%X", todCounter.nanoSeconds,
                 todCounter.seconds.l[0], todCounter.seconds.l[1]);

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChPtpPortTimeStampEnableSet
*
* DESCRIPTION:
*       Enable/Disable port for PTP timestamping.
*
* APPLICABLE DEVICES:  Lion and above.
*
* INPUTS:
*       devNum          - device number
*       direction       - Tx, Rx or both directions
*       portNum         - port number
*                         Note: For Ingress timestamping the trigger is based
*                               on the local source port.
*                               For Egress timestamping the trigger is based
*                               on the local target port.
*       enable          - GT_TRUE - The port is enabled for ingress/egress
*                                   timestamping.
*                         GT_FALSE - The port is disabled for ingress/egress
*                                   timestamping.
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong devNum, direction or portNum.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChPtpPortTimeStampEnableSet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                   result;
    GT_U8                       devNum;
    CPSS_PORT_DIRECTION_ENT     direction;
    GT_PHYSICAL_PORT_NUM                       portNum;
    GT_BOOL                     enable;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    direction = (CPSS_PORT_DIRECTION_ENT)inArgs[1];
    portNum = (GT_PHYSICAL_PORT_NUM)inArgs[2];
    enable = (GT_BOOL)inArgs[3];

    /* Override Device and Port */
    CONVERT_DEV_PHYSICAL_PORT_MAC(devNum, portNum);

    /* call cpss api function */
    result = cpssDxChPtpPortTimeStampEnableSet(devNum, direction, portNum,
                                               enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChPtpPortTimeStampEnableGet
*
* DESCRIPTION:
*       Get port PTP timestamping status (enabled/disabled).
*
* APPLICABLE DEVICES:  Lion and above.
*
* INPUTS:
*       devNum          - device number
*       direction       - Tx or Rx
*       portNum         - port number
*                         Note: For Ingress timestamping the trigger is based
*                               on the local source port.
*                               For Egress timestamping the trigger is based
*                               on the local target port.
* OUTPUTS:
*       enablePtr       - (pointer to) GT_TRUE - The port is enabled for
*                                               ingress/egress timestamping.
*                         GT_FALSE - The port is disabled for ingress/egress
*                                   timestamping.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong devNum, direction or portNum.
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChPtpPortTimeStampEnableGet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                   result;
    GT_U8                       devNum;
    CPSS_PORT_DIRECTION_ENT     direction;
    GT_PHYSICAL_PORT_NUM                       portNum;
    GT_BOOL                     enable;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    direction = (CPSS_PORT_DIRECTION_ENT)inArgs[1];
    portNum = (GT_PHYSICAL_PORT_NUM)inArgs[2];

    /* Override Device and Port */
    CONVERT_DEV_PHYSICAL_PORT_MAC(devNum, portNum);

    /* call cpss api function */
    result = cpssDxChPtpPortTimeStampEnableGet(devNum, direction, portNum,
                                               &enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", enable);

    return CMD_OK;
}


/*******************************************************************************
* cpssDxChPtpEthernetTimeStampEnableSet
*
* DESCRIPTION:
*       Enable/Disable PTP Timestamping over Ethernet packets.
*
* APPLICABLE DEVICES:  Lion and above.
*
* INPUTS:
*       devNum          - device number
*       direction       - Tx, Rx or both directions
*       enable          - GT_TRUE - PTP Timestamping is enabled over Ethernet.
*                         GT_FALSE - PTP Timestamping is disabled over Ethernet.
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong devNum or direction
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChPtpEthernetTimeStampEnableSet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                   result;
    GT_U8                       devNum;
    CPSS_PORT_DIRECTION_ENT     direction;
    GT_BOOL                     enable;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    direction = (CPSS_PORT_DIRECTION_ENT)inArgs[1];
    enable = (GT_BOOL)inArgs[2];

    /* call cpss api function */
    result = cpssDxChPtpEthernetTimeStampEnableSet(devNum, direction, enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}


/*******************************************************************************
* cpssDxChPtpEthernetTimeStampEnableGet
*
* DESCRIPTION:
*       Get PTP Timestamping status over Ethernet packets (enable/disabled).
*
* APPLICABLE DEVICES:  Lion and above.
*
* INPUTS:
*       devNum          - device number
*       direction       - Tx or Rx
* OUTPUTS:
*       enablePtr       - (pointer to) GT_TRUE - PTP Timestamping is enabled
*                                               over Ethernet.
*                         GT_FALSE - PTP Timestamping is disabled over Ethernet.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong devNum or direction
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChPtpEthernetTimeStampEnableGet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                   result;
    GT_U8                       devNum;
    CPSS_PORT_DIRECTION_ENT     direction;
    GT_BOOL                     enable;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    direction = (CPSS_PORT_DIRECTION_ENT)inArgs[1];

    /* call cpss api function */
    result = cpssDxChPtpEthernetTimeStampEnableGet(devNum, direction, &enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", enable);

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChPtpUdpTimeStampEnableSet
*
* DESCRIPTION:
*       Enable/Disable PTP Timestamping over UDP packets.
*
* APPLICABLE DEVICES:  Lion and above.
*
* INPUTS:
*       devNum          - device number
*       direction       - Tx, Rx or both directions
*       protocolStack   - types of IP to set: IPV4 or IPV6.
*       enable          - GT_TRUE - PTP Timestamping is enabled over UDP (IPV4/IPV6).
*                         GT_FALSE - PTP Timestamping is disabled over UDP (IPV4/IPV6).
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong devNum, protocolStack or direction
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChPtpUdpTimeStampEnableSet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                   result;
    GT_U8                       devNum;
    CPSS_PORT_DIRECTION_ENT     direction;
    CPSS_IP_PROTOCOL_STACK_ENT  protocolStack;
    GT_BOOL                     enable;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    direction = (CPSS_PORT_DIRECTION_ENT)inArgs[1];
    protocolStack = (CPSS_IP_PROTOCOL_STACK_ENT)inArgs[2];
    enable = (GT_BOOL)inArgs[3];

    /* call cpss api function */
    result = cpssDxChPtpUdpTimeStampEnableSet(devNum, direction, protocolStack,
                                              enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}


/*******************************************************************************
* cpssDxChPtpUdpTimeStampEnableGet
*
* DESCRIPTION:
*       Get PTP Timestamping status over UDP packets (enable/disabled).
*
* APPLICABLE DEVICES:  Lion and above.
*
* INPUTS:
*       devNum          - device number
*       direction       - Tx or Rx
*       protocolStack   - types of IP to set: IPV4 or IPV6.
* OUTPUTS:
*       enablePtr       - (pointer to) GT_TRUE - PTP Timestamping is enabled
*                                               over UDP (IPV4/IPV6).
*                         GT_FALSE - PTP Timestamping is disabled over UDP (IPV4/IPV6).
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong devNum, protocolStack or direction
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChPtpUdpTimeStampEnableGet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                   result;
    GT_U8                       devNum;
    CPSS_PORT_DIRECTION_ENT     direction;
    CPSS_IP_PROTOCOL_STACK_ENT  protocolStack;
    GT_BOOL                     enable;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    direction = (CPSS_PORT_DIRECTION_ENT)inArgs[1];
    protocolStack = (CPSS_IP_PROTOCOL_STACK_ENT)inArgs[2];

    /* call cpss api function */
    result = cpssDxChPtpUdpTimeStampEnableGet(devNum, direction, protocolStack,
                                              &enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", enable);

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChPtpMessageTypeTimeStampEnableSet
*
* DESCRIPTION:
*       Enable/Disable PTP Timestamping for specific message type of the packet.
*
* APPLICABLE DEVICES:  Lion and above.
*
* INPUTS:
*       devNum          - device number
*       direction       - Tx, Rx or both directions
*       messageType     - message type (0..15).
*       enable          - GT_TRUE - PTP Timestamping is enabled for specific message type.
*                         GT_FALSE - PTP Timestamping is disabled for specific message type.
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong devNum, messageType or direction
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChPtpMessageTypeTimeStampEnableSet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                   result;
    GT_U8                       devNum;
    CPSS_PORT_DIRECTION_ENT     direction;
    GT_U32                      messageType;
    GT_BOOL                     enable;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    direction = (CPSS_PORT_DIRECTION_ENT)inArgs[1];
    messageType = (GT_U32)inArgs[2];
    enable = (GT_BOOL)inArgs[3];

    /* call cpss api function */
    result = cpssDxChPtpMessageTypeTimeStampEnableSet(devNum, direction,
                                                      messageType, enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChPtpMessageTypeTimeStampEnableGet
*
* DESCRIPTION:
*       Get PTP Timestamping status for specific message type of the packet
*       (enable/disabled).
*
* APPLICABLE DEVICES:  Lion and above.
*
* INPUTS:
*       devNum          - device number
*       direction       - Tx or Rx
*       messageType     - message type (0..15).
* OUTPUTS:
*       enablePtr       - (pointer to) GT_TRUE - PTP Timestamping is enabled
*                                               for specific message type.
*                         GT_FALSE - PTP Timestamping is disabled for specific
*                         message type.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong devNum, messageType or direction
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChPtpMessageTypeTimeStampEnableGet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                   result;
    GT_U8                       devNum;
    CPSS_PORT_DIRECTION_ENT     direction;
    GT_U32                      messageType;
    GT_BOOL                     enable;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    direction = (CPSS_PORT_DIRECTION_ENT)inArgs[1];
    messageType = (GT_U32)inArgs[2];

    /* call cpss api function */
    result = cpssDxChPtpMessageTypeTimeStampEnableGet(devNum, direction,
                                                      messageType, &enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", enable);

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChPtpTransportSpecificCheckEnableSet
*
* DESCRIPTION:
*       Enable/Disable PTP Timestamping check of the packet's transport specific value.
*
* APPLICABLE DEVICES:  Lion and above.
*
* INPUTS:
*       devNum      - device number
*       direction   - Tx, Rx or both directions
*       enable      - GT_TRUE - PTP Timestamping check of the packet's transport
*                               specific value is enabled.
*                     GT_FALSE - PTP Timestamping check of the packet's transport
*                                specific value is disabled.
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong devNum or direction
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChPtpTransportSpecificCheckEnableSet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                   result;
    GT_U8                       devNum;
    CPSS_PORT_DIRECTION_ENT     direction;
    GT_BOOL                     enable;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    direction = (CPSS_PORT_DIRECTION_ENT)inArgs[1];
    enable = (GT_BOOL)inArgs[2];

    /* call cpss api function */
    result = cpssDxChPtpTransportSpecificCheckEnableSet(devNum, direction,
                                                        enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChPtpTransportSpecificCheckEnableGet
*
* DESCRIPTION:
*       Get status of PTP Timestamping check of the packet's transport specific
*       value (the check is enabled/disabled).
*
* APPLICABLE DEVICES:  Lion and above.
*
* INPUTS:
*       devNum      - device number
*       direction   - Tx or Rx
* OUTPUTS:
*       enablePtr   - (pointer to) GT_TRUE - PTP Timestamping check of the
*                                  packet's transport specific value is enabled.
*                     GT_FALSE - PTP Timestamping check of the packet's transport
*                                specific value is disabled.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong devNum or direction
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChPtpTransportSpecificCheckEnableGet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                   result;
    GT_U8                       devNum;
    CPSS_PORT_DIRECTION_ENT     direction;
    GT_BOOL                     enable;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    direction = (CPSS_PORT_DIRECTION_ENT)inArgs[1];

    /* call cpss api function */
    result = cpssDxChPtpTransportSpecificCheckEnableGet(devNum, direction,
                                                        &enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", enable);

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChPtpTransportSpecificTimeStampEnableSet
*
* DESCRIPTION:
*       Enable PTP Timestamping for transport specific value of the packet.
*       Note: Relevant only if global configuration determines whether the
*       transport specific field should be checked by
*       cpssDxChPtpTransportSpecificCheckEnableSet().
*
* APPLICABLE DEVICES:  Lion and above.
*
* INPUTS:
*       devNum                  - device number
*       direction               - Tx, Rx or both directions
*       transportSpecificVal    - transport specific value taken from PTP header
*                                 bits [7..4]. Acceptable values (0..15).
*       enable                  - GT_TRUE - PTP Timestamping is enabled for this
*                                           transport specific value.
*                                GT_FALSE - PTP Timestamping is disabled for this
*                                           transport specific value.
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong devNum, transportSpecificVal or
*                                  direction
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChPtpTransportSpecificTimeStampEnableSet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                   result;
    GT_U8                       devNum;
    CPSS_PORT_DIRECTION_ENT     direction;
    GT_U32                      transportSpecificVal;
    GT_BOOL                     enable;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    direction = (CPSS_PORT_DIRECTION_ENT)inArgs[1];
    transportSpecificVal = (GT_U32)inArgs[2];
    enable = (GT_BOOL)inArgs[3];

    /* call cpss api function */
    result = cpssDxChPtpTransportSpecificTimeStampEnableSet(devNum, direction,
                                                            transportSpecificVal,
                                                            enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChPtpTransportSpecificTimeStampEnableGet
*
* DESCRIPTION:
*       Get PTP Timestamping status for specific message type of the packet
*       (enable/disabled).
*
* APPLICABLE DEVICES:  Lion and above.
*
* INPUTS:
*       devNum          - device number
*       direction       - Tx or Rx
*       transportSpecificVal    - transport specific value taken from PTP header
* OUTPUTS:
*       enablePtr       - (pointer to) GT_TRUE - PTP Timestamping is
                                      enabled for this transport specific value.
*                         GT_FALSE - PTP Timestamping is disabled for this
*                                    transport specific value.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong devNum, transportSpecificVal or
*                                  direction
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChPtpTransportSpecificTimeStampEnableGet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                   result;
    GT_U8                       devNum;
    CPSS_PORT_DIRECTION_ENT     direction;
    GT_U32                      transportSpecificVal;
    GT_BOOL                     enable;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    direction = (CPSS_PORT_DIRECTION_ENT)inArgs[1];
    transportSpecificVal = (GT_U32)inArgs[2];

    /* call cpss api function */
    result = cpssDxChPtpTransportSpecificTimeStampEnableGet(devNum, direction,
                                                            transportSpecificVal,
                                                            &enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", enable);

    return CMD_OK;

}

/*******************************************************************************
* cpssDxChPtpTimestampEntryGet
*
* DESCRIPTION:
*       Read current Timestamp entry from the timestamp FIFO queue.
*
* APPLICABLE DEVICES:  Lion and above.
*
* INPUTS:
*       devNum          - device number
*       direction       - Tx, Rx or both directions
* OUTPUTS:
*       entryPtr        - (pointer to) timestamp entry
*
* RETURNS:
*       GT_OK                    - on success
*       GT_NO_MORE               - the action succeeded and there are no more waiting
*                                  Timestamp entries
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong devNum or direction
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChPtpTimestampEntryGet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                           result;
    GT_U8                               devNum;
    CPSS_PORT_DIRECTION_ENT             direction;
    CPSS_DXCH_PTP_TIMESTAMP_ENTRY_STC   entry;
    GT_HW_DEV_NUM               __HwDev;  /* Dummy for converting. */
    GT_PORT_NUM                 __Port; /* Dummy for converting. */

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    direction = (CPSS_PORT_DIRECTION_ENT)inArgs[1];

    /* call cpss api function */
    result = cpssDxChPtpTimestampEntryGet(devNum, direction, &entry);

    /* need to remove casting after changing the devNum and portNum type*/
    CPSS_TBD_BOOKMARK_EARCH

    __HwDev = (GT_U32)devNum;
    __Port = (GT_U32)entry.portNum;
    /* convert port number */
    CONVERT_BACK_DEV_PORT_DATA_MAC(__HwDev,__Port);
    devNum = (GT_U8)__HwDev;
    entry.portNum = (GT_U8)__Port;

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d %d %d %d", entry.portNum, entry.messageType,
                 entry.sequenceId, entry.timeStampVal);

    return CMD_OK;
}


/*============== BobCat2 And Lion3 new API =============*/
/*******************************************************************************
* cpssDxChPtpTodCounterFunctionSet
*
* DESCRIPTION:
*       The function configures type of TOD counter action that will be
*       performed once triggered by cpssDxChPtpTodCounterFunctionTriggerSet()
*       API or by pulse from an external interface.
*
* APPLICABLE DEVICES:  Lion and above.
*
* INPUTS:
*       devNum          - device number
*       direction       - Tx, Rx or both directions
*       taiIdPtr        - (pointer to) TAI Units identification.
*                         (APPLICABLE DEVICES: BobCat2, Lion3.)
*       function        - One of the four possible TOD counter functions
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong devNum or direction
*                                  or function
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChPtpTodCounterFunctionSet_Tai
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                           result;
    GT_U8                               devNum;
    CPSS_PORT_DIRECTION_ENT             direction;
    CPSS_DXCH_PTP_TOD_COUNTER_FUNC_ENT  function;
    CPSS_DXCH_PTP_TAI_ID_STC            taiId;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    direction = (CPSS_PORT_DIRECTION_ENT)inArgs[1];
    taiId.taiInstance = (CPSS_DXCH_PTP_TAI_INSTANCE_ENT)inArgs[2];
    taiId.taiNumber   = (CPSS_DXCH_PTP_TAI_NUMBER_ENT)inArgs[3];
    taiId.portNum     = (GT_PHYSICAL_PORT_NUM)inArgs[4];
    function = (CPSS_DXCH_PTP_TOD_COUNTER_FUNC_ENT)inArgs[5];

    /* call cpss api function */
    result = cpssDxChPtpTodCounterFunctionSet(
        devNum, direction, &taiId, function);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChPtpTodCounterFunctionGet
*
* DESCRIPTION:
*       Get type of TOD counter action that will be performed once triggered by
*       cpssDxChPtpTodCounterFunctionTriggerSet() API or by pulse from an
*       external interface.
*
* APPLICABLE DEVICES:  Lion and above.
*
* INPUTS:
*       devNum          - device number
*       direction       - Tx or Rx
*       taiIdPtr        - (pointer to) TAI Units identification.
*                         (APPLICABLE DEVICES: BobCat2, Lion3.)
*
* OUTPUTS:
*       functionPtr     - (pointer to) One of the four possible TOD counter
*                          functions
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong devNum or direction
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_BAD_STATE             - on bad value found in HW
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChPtpTodCounterFunctionGet_Tai
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                           result;
    GT_U8                               devNum;
    CPSS_PORT_DIRECTION_ENT             direction;
    CPSS_DXCH_PTP_TOD_COUNTER_FUNC_ENT  function;
    CPSS_DXCH_PTP_TAI_ID_STC            taiId;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    direction = (CPSS_PORT_DIRECTION_ENT)inArgs[1];
    taiId.taiInstance = (CPSS_DXCH_PTP_TAI_INSTANCE_ENT)inArgs[2];
    taiId.taiNumber   = (CPSS_DXCH_PTP_TAI_NUMBER_ENT)inArgs[3];
    taiId.portNum     = (GT_PHYSICAL_PORT_NUM)inArgs[4];

    /* call cpss api function */
    result = cpssDxChPtpTodCounterFunctionGet(
        devNum, direction, &taiId, &function);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", function);

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChPtpTodCounterFunctionTriggerSet
*
* DESCRIPTION:
*       Trigger TOD (Time of Day) counter function accorging to the function set
*       by cpssDxChPtpTodCounterFunctionSet().
*
* APPLICABLE DEVICES:  Lion and above.
*
* INPUTS:
*       devNum          - device number
*       direction       - Tx, Rx or both directions
*       taiIdPtr        - (pointer to) TAI Units identification.
*                         (APPLICABLE DEVICES: BobCat2, Lion3.)
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_STATE             - when previous TOD triggered action is not finished yet
*       GT_BAD_PARAM             - on wrong devNum or direction
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChPtpTodCounterFunctionTriggerSet_Tai
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                   result;
    GT_U8                       devNum;
    CPSS_PORT_DIRECTION_ENT     direction;
    CPSS_DXCH_PTP_TAI_ID_STC    taiId;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    direction = (CPSS_PORT_DIRECTION_ENT)inArgs[1];
    taiId.taiInstance = (CPSS_DXCH_PTP_TAI_INSTANCE_ENT)inArgs[2];
    taiId.taiNumber   = (CPSS_DXCH_PTP_TAI_NUMBER_ENT)inArgs[3];
    taiId.portNum     = (GT_PHYSICAL_PORT_NUM)inArgs[4];

    /* call cpss api function */
    result = cpssDxChPtpTodCounterFunctionTriggerSet(
        devNum, direction, &taiId);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChPtpTodCounterFunctionTriggerGet
*
* DESCRIPTION:
*       Get status of trigger TOD (Time of Day) counter function.
*
* APPLICABLE DEVICES:  Lion and above.
*
* INPUTS:
*       devNum          - device number
*       direction       - Tx or Rx
*       taiIdPtr        - (pointer to) TAI Units identification.
*                         (APPLICABLE DEVICES: BobCat2, Lion3.)
*
* OUTPUTS:
*       enablePtr       - (pointer to) GT_TRUE - The trigger is on, the TOD
*                                               function is not finished.
*                         GT_FALSE - The trigger is off, the TOD function is
*                                   finished.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong devNum or direction
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChPtpTodCounterFunctionTriggerGet_Tai
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                   result;
    GT_U8                       devNum;
    CPSS_PORT_DIRECTION_ENT     direction;
    GT_BOOL                     enable;
    CPSS_DXCH_PTP_TAI_ID_STC    taiId;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    direction = (CPSS_PORT_DIRECTION_ENT)inArgs[1];
    taiId.taiInstance = (CPSS_DXCH_PTP_TAI_INSTANCE_ENT)inArgs[2];
    taiId.taiNumber   = (CPSS_DXCH_PTP_TAI_NUMBER_ENT)inArgs[3];
    taiId.portNum     = (GT_PHYSICAL_PORT_NUM)inArgs[4];

    /* call cpss api function */
    result = cpssDxChPtpTodCounterFunctionTriggerGet(
        devNum, direction, &taiId, &enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", enable);

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChPtpTaiClockModeSet
*
* DESCRIPTION:
*       Set TAI (Time Application Interface) Clock Mode.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum          - device number
*       taiIdPtr        - (pointer to) TAI Units identification.
*       clockMode       - clock interface using mode.
*                         Output mode relevant to Global TAI Instance only.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_BAD_PARAM             - on wrong parameter
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChPtpTaiClockModeSet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                           result;
    GT_U8                               devNum;
    CPSS_DXCH_PTP_TAI_ID_STC            taiId;
    CPSS_DXCH_PTP_TAI_CLOCK_MODE_ENT    clockMode;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    taiId.taiInstance = (CPSS_DXCH_PTP_TAI_INSTANCE_ENT)inArgs[1];
    taiId.taiNumber   = (CPSS_DXCH_PTP_TAI_NUMBER_ENT)inArgs[2];
    taiId.portNum     = (GT_PHYSICAL_PORT_NUM)inArgs[3];
    clockMode         = (CPSS_DXCH_PTP_TAI_CLOCK_MODE_ENT)inArgs[4];

    /* call cpss api function */
    result = cpssDxChPtpTaiClockModeSet(
        devNum, &taiId, clockMode);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChPtpTaiClockModeGet
*
* DESCRIPTION:
*       Get TAI (Time Application Interface) Clock Mode.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum          - device number
*       taiIdPtr        - (pointer to) TAI Units identification.
*                         Single unit must be specified.
*
* OUTPUTS:
*       clockModePtr    - pointer to clock interface using mode.
*                         Output mode relevant to Global TAI Instance only.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_BAD_PARAM             - on wrong parameter
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChPtpTaiClockModeGet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                           result;
    GT_U8                               devNum;
    CPSS_DXCH_PTP_TAI_ID_STC            taiId;
    CPSS_DXCH_PTP_TAI_CLOCK_MODE_ENT    clockMode;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    taiId.taiInstance = (CPSS_DXCH_PTP_TAI_INSTANCE_ENT)inArgs[1];
    taiId.taiNumber   = (CPSS_DXCH_PTP_TAI_NUMBER_ENT)inArgs[2];
    taiId.portNum     = (GT_PHYSICAL_PORT_NUM)inArgs[3];

    /* call cpss api function */
    result = cpssDxChPtpTaiClockModeGet(
        devNum, &taiId, &clockMode);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", clockMode);

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChPtpTaiInternalClockGenerateEnableSet
*
* DESCRIPTION:
*       Set TAI (Time Application Interface) Internal Clock Enable.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum          - device number
*       taiIdPtr        - (pointer to) TAI Units identification.
*       enable          - Internal Clock Generate Enable.
*                         GT_TRUE - the internal clock generator
*                         generates a clock signal
*                         GT_FALSE - the internal clock not generated.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_BAD_PARAM             - on wrong parameter
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChPtpTaiInternalClockGenerateEnableSet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                           result;
    GT_U8                               devNum;
    CPSS_DXCH_PTP_TAI_ID_STC            taiId;
    GT_BOOL                             enable;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    taiId.taiInstance = (CPSS_DXCH_PTP_TAI_INSTANCE_ENT)inArgs[1];
    taiId.taiNumber   = (CPSS_DXCH_PTP_TAI_NUMBER_ENT)inArgs[2];
    taiId.portNum     = (GT_PHYSICAL_PORT_NUM)inArgs[3];
    enable            = (GT_BOOL)inArgs[4];

    /* call cpss api function */
    result = cpssDxChPtpTaiInternalClockGenerateEnableSet(
        devNum, &taiId, enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChPtpTaiInternalClockGenerateEnableGet
*
* DESCRIPTION:
*       Get TAI (Time Application Interface) Internal Clock Enable.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum          - device number
*       taiIdPtr        - (pointer to) TAI Units identification.
*                         Single unit must be specified.
*
* OUTPUTS:
*       enablePtr       - pointer to Internal Clock Generate Enable.
*                         GT_TRUE - the internal clock generator
*                         generates a clock
*                         GT_FALSE - the internal clock not generated.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_BAD_PARAM             - on wrong parameter
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChPtpTaiInternalClockGenerateEnableGet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                           result;
    GT_U8                               devNum;
    CPSS_DXCH_PTP_TAI_ID_STC            taiId;
    GT_BOOL                             enable;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    taiId.taiInstance = (CPSS_DXCH_PTP_TAI_INSTANCE_ENT)inArgs[1];
    taiId.taiNumber   = (CPSS_DXCH_PTP_TAI_NUMBER_ENT)inArgs[2];
    taiId.portNum     = (GT_PHYSICAL_PORT_NUM)inArgs[3];

    /* call cpss api function */
    result = cpssDxChPtpTaiInternalClockGenerateEnableGet(
        devNum, &taiId, &enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", enable);

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChPtpTaiPtpPClockDriftAdjustEnableSet
*
* DESCRIPTION:
*       Set TAI (Time Application Interface) Ptp Clock Adjust Enable.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum         - device number
*       taiIdPtr       - (pointer to) TAI Units identification.
*       enable         - Ptp PClock Drift Adjust Enable.
*                        GT_TRUE - the PClk is affected by the <Drift Adjustment> or not.
*                        GT_FALSE - the PClk reflects the frequency of the free running TOD,
*                        without any frequency adjustments.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_BAD_PARAM             - on wrong parameter
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChPtpTaiPtpPClockDriftAdjustEnableSet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                           result;
    GT_U8                               devNum;
    CPSS_DXCH_PTP_TAI_ID_STC            taiId;
    GT_BOOL                             enable;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    taiId.taiInstance = (CPSS_DXCH_PTP_TAI_INSTANCE_ENT)inArgs[1];
    taiId.taiNumber   = (CPSS_DXCH_PTP_TAI_NUMBER_ENT)inArgs[2];
    taiId.portNum     = (GT_PHYSICAL_PORT_NUM)inArgs[3];
    enable            = (GT_BOOL)inArgs[4];

    /* call cpss api function */
    result = cpssDxChPtpTaiPtpPClockDriftAdjustEnableSet(
        devNum, &taiId, enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChPtpTaiPtpPClockDriftAdjustEnableGet
*
* DESCRIPTION:
*       Get TAI (Time Application Interface) Ptp Clock Adjust Enable.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum         - device number
*       taiIdPtr       - (pointer to) TAI Units identification.
*                        Single unit must be specified.
*
* OUTPUTS:
*       enablePtr      - pointer to Ptp PClock Drift Adjust Enable.
*                        GT_TRUE - the PClk is affected by the <Drift Adjustment> or not.
*                        GT_FALSE - the PClk reflects the frequency of the free running TOD,
*                        without any frequency adjustments.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_BAD_PARAM             - on wrong parameter
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChPtpTaiPtpPClockDriftAdjustEnableGet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                           result;
    GT_U8                               devNum;
    CPSS_DXCH_PTP_TAI_ID_STC            taiId;
    GT_BOOL                             enable;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    taiId.taiInstance = (CPSS_DXCH_PTP_TAI_INSTANCE_ENT)inArgs[1];
    taiId.taiNumber   = (CPSS_DXCH_PTP_TAI_NUMBER_ENT)inArgs[2];
    taiId.portNum     = (GT_PHYSICAL_PORT_NUM)inArgs[3];

    /* call cpss api function */
    result = cpssDxChPtpTaiPtpPClockDriftAdjustEnableGet(
        devNum, &taiId, &enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", enable);

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChPtpTaiCaptureOverrideEnableSet
*
* DESCRIPTION:
*       Set TAI (Time Application Interface) Capture Override Enable.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum         - device number
*       taiIdPtr        - (pointer to) TAI Units identification.
*       enable         - Capture Override Enable.
*                        When both TOD Capture registers are valid and capture
*                        trigger arrives:
*                        GT_TRUE  - the TOD Capture Value0 is overwritten.
*                        GT_FALSE - the TOD Capture Value0 unchanged.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_BAD_PARAM             - on wrong parameter
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChPtpTaiCaptureOverrideEnableSet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                           result;
    GT_U8                               devNum;
    CPSS_DXCH_PTP_TAI_ID_STC            taiId;
    GT_BOOL                             enable;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    taiId.taiInstance = (CPSS_DXCH_PTP_TAI_INSTANCE_ENT)inArgs[1];
    taiId.taiNumber   = (CPSS_DXCH_PTP_TAI_NUMBER_ENT)inArgs[2];
    taiId.portNum     = (GT_PHYSICAL_PORT_NUM)inArgs[3];
    enable            = (GT_BOOL)inArgs[4];

    /* call cpss api function */
    result = cpssDxChPtpTaiCaptureOverrideEnableSet(
        devNum, &taiId, enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChPtpTaiCaptureOverrideEnableGet
*
* DESCRIPTION:
*       Get TAI (Time Application Interface) Capture Override Enable
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum         - device number
*       taiIdPtr        - (pointer to) TAI Units identification.
*                        Single unit must be specified.
*
* OUTPUTS:
*       enablePtr      - pointer to Capture Override Enable.
*                        When both TOD Capture registers are valid and capture
*                        trigger arrives:
*                        GT_TRUE  - the TOD Capture Value0 is overwritten.
*                        GT_FALSE - the TOD Capture Value0 unchanged.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_BAD_PARAM             - on wrong parameter
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChPtpTaiCaptureOverrideEnableGet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                           result;
    GT_U8                               devNum;
    CPSS_DXCH_PTP_TAI_ID_STC            taiId;
    GT_BOOL                             enable;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    taiId.taiInstance = (CPSS_DXCH_PTP_TAI_INSTANCE_ENT)inArgs[1];
    taiId.taiNumber   = (CPSS_DXCH_PTP_TAI_NUMBER_ENT)inArgs[2];
    taiId.portNum     = (GT_PHYSICAL_PORT_NUM)inArgs[3];

    /* call cpss api function */
    result = cpssDxChPtpTaiCaptureOverrideEnableGet(
        devNum, &taiId, &enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", enable);

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChPtpTaiInputTriggersCountEnableSet
*
* DESCRIPTION:
*       Set TAI (Time Application Interface) Count Input Triggers Enable.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum         - device number
*       taiIdPtr       - (pointer to) TAI Units identification.
*       enable         - Count Input Triggers Enable.
*                        GT_TRUE  - each incoming trigger is counted.
*                        GT_FALSE - incoming triggers not counted.
*                        see cpssDxChPtpTaiIncomingTriggerCounterSet/Get
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_BAD_PARAM             - on wrong parameter
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChPtpTaiInputTriggersCountEnableSet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                           result;
    GT_U8                               devNum;
    CPSS_DXCH_PTP_TAI_ID_STC            taiId;
    GT_BOOL                             enable;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    taiId.taiInstance = (CPSS_DXCH_PTP_TAI_INSTANCE_ENT)inArgs[1];
    taiId.taiNumber   = (CPSS_DXCH_PTP_TAI_NUMBER_ENT)inArgs[2];
    taiId.portNum     = (GT_PHYSICAL_PORT_NUM)inArgs[3];
    enable            = (GT_BOOL)inArgs[4];

    /* call cpss api function */
    result = cpssDxChPtpTaiInputTriggersCountEnableSet(
        devNum, &taiId, enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChPtpTaiInputTriggersCountEnableGet
*
* DESCRIPTION:
*       Get TAI (Time Application Interface) Count Input Triggers Enable
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum         - device number
*       taiIdPtr       - (pointer to) TAI Units identification.
*                        Single unit must be specified.
*
* OUTPUTS:
*       enablePtr      - pointer to Count Input Triggers Enable.
*                        If enabled, each incoming trigger is counted.
*                        see cpssDxChPtpTaiIncomingTriggerCounterSet/Get
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_BAD_PARAM             - on wrong parameter
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChPtpTaiInputTriggersCountEnableGet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                           result;
    GT_U8                               devNum;
    CPSS_DXCH_PTP_TAI_ID_STC            taiId;
    GT_BOOL                             enable;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    taiId.taiInstance = (CPSS_DXCH_PTP_TAI_INSTANCE_ENT)inArgs[1];
    taiId.taiNumber   = (CPSS_DXCH_PTP_TAI_NUMBER_ENT)inArgs[2];
    taiId.portNum     = (GT_PHYSICAL_PORT_NUM)inArgs[3];

    /* call cpss api function */
    result = cpssDxChPtpTaiInputTriggersCountEnableGet(
        devNum, &taiId, &enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", enable);

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChPtpTaiExternalPulseWidthSet
*
* DESCRIPTION:
*       Set TAI (Time Application Interface) External Pulse Width.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum         - device number
*       taiIdPtr       - (pointer to) TAI Units identification.
*       extPulseWidth  - External Pulse Width.
*                        width of the pulse in nanoseconds of generated output trigger.
*                       (APPLICABLE RANGES: 0..(2^28 - 1).)
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_BAD_PARAM             - on wrong parameter
*       GT_OUT_OF_RANGE          - on bad extPulseWidth
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChPtpTaiExternalPulseWidthSet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                           result;
    GT_U8                               devNum;
    CPSS_DXCH_PTP_TAI_ID_STC            taiId;
    GT_U32                              extPulseWidth;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    taiId.taiInstance = (CPSS_DXCH_PTP_TAI_INSTANCE_ENT)inArgs[1];
    taiId.taiNumber   = (CPSS_DXCH_PTP_TAI_NUMBER_ENT)inArgs[2];
    taiId.portNum     = (GT_PHYSICAL_PORT_NUM)inArgs[3];
    extPulseWidth     = (GT_U32)inArgs[4];

    /* call cpss api function */
    result = cpssDxChPtpTaiExternalPulseWidthSet(
        devNum, &taiId, extPulseWidth);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChPtpTaiExternalPulseWidthGet
*
* DESCRIPTION:
*       Get TAI (Time Application Interface) External Pulse Width
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum         - device number
*       taiIdPtr       - (pointer to) TAI Units identification.
*                        Single unit must be specified.
*
* OUTPUTS:
*       extPulseWidthPtr -  pointer to External Pulse Width.
*                           width of the pulse in nanoseconds of generated output trigger.
*                           (APPLICABLE RANGES: 0..(2^28 - 1).)
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_BAD_PARAM             - on wrong parameter
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChPtpTaiExternalPulseWidthGet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                           result;
    GT_U8                               devNum;
    CPSS_DXCH_PTP_TAI_ID_STC            taiId;
    GT_U32                              extPulseWidth;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    taiId.taiInstance = (CPSS_DXCH_PTP_TAI_INSTANCE_ENT)inArgs[1];
    taiId.taiNumber   = (CPSS_DXCH_PTP_TAI_NUMBER_ENT)inArgs[2];
    taiId.portNum     = (GT_PHYSICAL_PORT_NUM)inArgs[3];

    /* call cpss api function */
    result = cpssDxChPtpTaiExternalPulseWidthGet(
        devNum, &taiId, &extPulseWidth);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", extPulseWidth);

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChPtpTaiTodSet
*
* DESCRIPTION:
*       Set TAI (Time Application Interface) TOD values.
*       The TOD will be updated by triggering an appropriate function.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum          - device number
*       taiIdPtr        - (pointer to) TAI Units identification.
*       todValueType    - type of TOD value.
*                         Valid types are
*                         CPSS_DXCH_PTP_TAI_TOD_TYPE_TRIGGER_GENERATION_E,
*                         CPSS_DXCH_PTP_TAI_TOD_TYPE_TRIGGER_GENERATION_MASK_E,
*                         CPSS_DXCH_PTP_TAI_TOD_TYPE_LOAD_VALUE_E.
*       todValuePtr     - pointer to TOD value.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_BAD_PARAM             - on wrong parameter
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       This API does not activate any triggers, other APIs does it
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChPtpTaiTodSet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                           result;
    GT_U8                               devNum;
    CPSS_DXCH_PTP_TAI_ID_STC            taiId;
    CPSS_DXCH_PTP_TAI_TOD_TYPE_ENT      todValueType;
    CPSS_DXCH_PTP_TOD_COUNT_STC         todValue;
    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum                    = (GT_U8)inArgs[0];
    taiId.taiInstance         = (CPSS_DXCH_PTP_TAI_INSTANCE_ENT)inArgs[1];
    taiId.taiNumber           = (CPSS_DXCH_PTP_TAI_NUMBER_ENT)inArgs[2];
    taiId.portNum             = (GT_PHYSICAL_PORT_NUM)inArgs[3];
    todValueType              = (CPSS_DXCH_PTP_TAI_TOD_TYPE_ENT)inArgs[4];
    todValue.seconds.l[0]     = (GT_U32)inArgs[5];
    todValue.seconds.l[1]     = (GT_U32)inArgs[6];
    todValue.nanoSeconds      = (GT_U32)inArgs[7];
    todValue.fracNanoSeconds  = (GT_U32)inArgs[8];

    /* call cpss api function */
    result = cpssDxChPtpTaiTodSet(
        devNum, &taiId, todValueType, &todValue);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChPtpTaiTodGet
*
* DESCRIPTION:
*       Get TAI (Time Application Interface) TOD values.
*       The TOD was captured by triggering an appropriate function.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum          - device number
*       taiIdPtr        - (pointer to) TAI Units identification.
*                         Single unit must be specified.
*       todValueType    - type of TOD value.
*
* OUTPUTS:
*       todValuePtr     - pointer to TOD value.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_BAD_PARAM             - on wrong parameter
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChPtpTaiTodGet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                           result;
    GT_U8                               devNum;
    CPSS_DXCH_PTP_TAI_ID_STC            taiId;
    CPSS_DXCH_PTP_TAI_TOD_TYPE_ENT      todValueType;
    CPSS_DXCH_PTP_TOD_COUNT_STC         todValue;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum            = (GT_U8)inArgs[0];
    taiId.taiInstance = (CPSS_DXCH_PTP_TAI_INSTANCE_ENT)inArgs[1];
    taiId.taiNumber   = (CPSS_DXCH_PTP_TAI_NUMBER_ENT)inArgs[2];
    taiId.portNum     = (GT_PHYSICAL_PORT_NUM)inArgs[3];
    todValueType      = (CPSS_DXCH_PTP_TAI_TOD_TYPE_ENT)inArgs[4];

    /* call cpss api function */
    result = cpssDxChPtpTaiTodGet(
        devNum, &taiId, todValueType, &todValue);

    /* pack output arguments to galtis string */
    galtisOutput(
        outArgs, result, "%d%d%d%d",
        todValue.seconds.l[0], todValue.seconds.l[1],
        todValue.nanoSeconds, todValue.fracNanoSeconds);

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChPtpTaiOutputTriggerEnableSet
*
* DESCRIPTION:
*       Set TAI (Time Application Interface) Output Trigger Generation Enable.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum          - device number
*       taiIdPtr        - (pointer to) TAI Units identification.
*       enable          -  enable output trigger generation.
*                          GT_TRUE  - enable, GT_FALSE - disable.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_BAD_PARAM             - on wrong parameter
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       use cpssDxChPtpTaiTodSet with
*       CPSS_DXCH_PTP_TAI_TOD_TYPE_TRIGGER_GENERATION_E and
*       the time to trigger generation.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChPtpTaiOutputTriggerEnableSet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                           result;
    GT_U8                               devNum;
    CPSS_DXCH_PTP_TAI_ID_STC            taiId;
    GT_BOOL                             enable;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    taiId.taiInstance = (CPSS_DXCH_PTP_TAI_INSTANCE_ENT)inArgs[1];
    taiId.taiNumber   = (CPSS_DXCH_PTP_TAI_NUMBER_ENT)inArgs[2];
    taiId.portNum     = (GT_PHYSICAL_PORT_NUM)inArgs[3];
    enable            = (GT_BOOL)inArgs[4];

    /* call cpss api function */
    result = cpssDxChPtpTaiOutputTriggerEnableSet(
        devNum, &taiId, enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChPtpTaiOutputTriggerEnableGet
*
* DESCRIPTION:
*       Get TAI (Time Application Interface) Output Trigger Generation Enable.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum          - device number
*       taiIdPtr        - (pointer to) TAI Units identification.
*
* OUTPUTS:
*       enablePtr       - (pointer to) enable output trigger generation.
*                         GT_TRUE  - enable GT_FALSE - disable
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_BAD_PARAM             - on wrong parameter
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChPtpTaiOutputTriggerEnableGet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                           result;
    GT_U8                               devNum;
    CPSS_DXCH_PTP_TAI_ID_STC            taiId;
    GT_BOOL                             enable;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    taiId.taiInstance = (CPSS_DXCH_PTP_TAI_INSTANCE_ENT)inArgs[1];
    taiId.taiNumber   = (CPSS_DXCH_PTP_TAI_NUMBER_ENT)inArgs[2];
    taiId.portNum     = (GT_PHYSICAL_PORT_NUM)inArgs[3];

    /* call cpss api function */
    result = cpssDxChPtpTaiOutputTriggerEnableGet(
        devNum, &taiId, &enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", enable);

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChPtpTaiTodStepSet
*
* DESCRIPTION:
*       Set TAI (Time Application Interface) TOD Step.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum          - device number
*       taiIdPtr        - (pointer to) TAI Units identification.
*       todStepPtr      - pointer to TOD Step.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_BAD_PARAM             - on wrong parameter
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChPtpTaiTodStepSet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                           result;
    GT_U8                               devNum;
    CPSS_DXCH_PTP_TAI_ID_STC            taiId;
    CPSS_DXCH_PTP_TAI_TOD_STEP_STC      todStep;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum                    = (GT_U8)inArgs[0];
    taiId.taiInstance         = (CPSS_DXCH_PTP_TAI_INSTANCE_ENT)inArgs[1];
    taiId.taiNumber           = (CPSS_DXCH_PTP_TAI_NUMBER_ENT)inArgs[2];
    taiId.portNum             = (GT_PHYSICAL_PORT_NUM)inArgs[3];
    todStep.nanoSeconds       = (GT_U32)inArgs[4];
    todStep.fracNanoSeconds   = (GT_U32)inArgs[5];

    /* call cpss api function */
    result = cpssDxChPtpTaiTodStepSet(
        devNum, &taiId, &todStep);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChPtpTaiTodStepGet
*
* DESCRIPTION:
*       Get TAI (Time Application Interface) TOD Step.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum          - device number
*       taiIdPtr        - (pointer to) TAI Units identification.
*                         Single unit must be specified.
*
* OUTPUTS:
*       todStepPtr      - pointer to TOD Step.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_BAD_PARAM             - on wrong parameter
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChPtpTaiTodStepGet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                           result;
    GT_U8                               devNum;
    CPSS_DXCH_PTP_TAI_ID_STC            taiId;
    CPSS_DXCH_PTP_TAI_TOD_STEP_STC      todStep;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum            = (GT_U8)inArgs[0];
    taiId.taiInstance = (CPSS_DXCH_PTP_TAI_INSTANCE_ENT)inArgs[1];
    taiId.taiNumber   = (CPSS_DXCH_PTP_TAI_NUMBER_ENT)inArgs[2];
    taiId.portNum     = (GT_PHYSICAL_PORT_NUM)inArgs[3];

    /* call cpss api function */
    result = cpssDxChPtpTaiTodStepGet(
        devNum, &taiId, &todStep);

    /* pack output arguments to galtis string */
    galtisOutput(
        outArgs, result, "%d%d",
        todStep.nanoSeconds, todStep.fracNanoSeconds);

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChPtpTaiPulseDelaySet
*
* DESCRIPTION:
*       Set TAI (Time Application Interface) External Trigger Propagation Delay.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum          - device number
*       taiIdPtr        - (pointer to) TAI Units identification.
*       nanoSeconds     - nano seconds delay.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_BAD_PARAM             - on wrong parameter
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChPtpTaiPulseDelaySet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                           result;
    GT_U8                               devNum;
    CPSS_DXCH_PTP_TAI_ID_STC            taiId;
    GT_U32                              pulseDelay;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    taiId.taiInstance = (CPSS_DXCH_PTP_TAI_INSTANCE_ENT)inArgs[1];
    taiId.taiNumber   = (CPSS_DXCH_PTP_TAI_NUMBER_ENT)inArgs[2];
    taiId.portNum     = (GT_PHYSICAL_PORT_NUM)inArgs[3];
    pulseDelay        = (GT_U32)inArgs[4];

    /* call cpss api function */
    result = cpssDxChPtpTaiPulseDelaySet(
        devNum, &taiId, pulseDelay);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChPtpTaiPulseDelayGet
*
* DESCRIPTION:
*       Get TAI (Time Application Interface) External Trigger Propagation Delay.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum          - device number
*       taiIdPtr        - (pointer to) TAI Units identification.
*                         Single unit must be specified.
*
* OUTPUTS:
*       nanoSecondsPtr  - (pointer to) nano seconds delay.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_BAD_PARAM             - on wrong parameter
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChPtpTaiPulseDelayGet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                           result;
    GT_U8                               devNum;
    CPSS_DXCH_PTP_TAI_ID_STC            taiId;
    GT_U32                              pulseDelay;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    taiId.taiInstance = (CPSS_DXCH_PTP_TAI_INSTANCE_ENT)inArgs[1];
    taiId.taiNumber   = (CPSS_DXCH_PTP_TAI_NUMBER_ENT)inArgs[2];
    taiId.portNum     = (GT_PHYSICAL_PORT_NUM)inArgs[3];

    /* call cpss api function */
    result = cpssDxChPtpTaiPulseDelayGet(
        devNum, &taiId, &pulseDelay);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", pulseDelay);

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChPtpTaiClockDelaySet
*
* DESCRIPTION:
*       Set TAI (Time Application Interface) External Clock Propagation Delay.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum          - device number
*       taiIdPtr        - (pointer to) TAI Units identification.
*       nanoSeconds     - nano seconds delay.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_BAD_PARAM             - on wrong parameter
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChPtpTaiClockDelaySet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                           result;
    GT_U8                               devNum;
    CPSS_DXCH_PTP_TAI_ID_STC            taiId;
    GT_U32                              clockDelay;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    taiId.taiInstance = (CPSS_DXCH_PTP_TAI_INSTANCE_ENT)inArgs[1];
    taiId.taiNumber   = (CPSS_DXCH_PTP_TAI_NUMBER_ENT)inArgs[2];
    taiId.portNum     = (GT_PHYSICAL_PORT_NUM)inArgs[3];
    clockDelay        = (GT_U32)inArgs[4];

    /* call cpss api function */
    result = cpssDxChPtpTaiClockDelaySet(
        devNum, &taiId, clockDelay);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChPtpTaiClockDelayGet
*
* DESCRIPTION:
*       Get TAI (Time Application Interface) External Clock Propagation Delay.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum          - device number
*       taiIdPtr        - (pointer to) TAI Units identification.
*                         Single unit must be specified.
*
* OUTPUTS:
*       nanoSecondsPtr  - (pointer to) nano seconds delay.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_BAD_PARAM             - on wrong parameter
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChPtpTaiClockDelayGet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                           result;
    GT_U8                               devNum;
    CPSS_DXCH_PTP_TAI_ID_STC            taiId;
    GT_U32                              clockDelay;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    taiId.taiInstance = (CPSS_DXCH_PTP_TAI_INSTANCE_ENT)inArgs[1];
    taiId.taiNumber   = (CPSS_DXCH_PTP_TAI_NUMBER_ENT)inArgs[2];
    taiId.portNum     = (GT_PHYSICAL_PORT_NUM)inArgs[3];

    /* call cpss api function */
    result = cpssDxChPtpTaiClockDelayGet(
        devNum, &taiId, &clockDelay);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", clockDelay);

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChPtpTaiFractionalNanosecondDriftSet
*
* DESCRIPTION:
*       Set TAI (Time Application Interface) Fractional Nanosecond Drift.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum          - device number
*       taiIdPtr        - (pointer to) TAI Units identification.
*       fracNanoSecond  - (pointer to) fractional nano seconds drift.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_BAD_PARAM             - on wrong parameter
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChPtpTaiFractionalNanosecondDriftSet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                           result;
    GT_U8                               devNum;
    CPSS_DXCH_PTP_TAI_ID_STC            taiId;
    GT_32                               fracNanoDrift;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    taiId.taiInstance = (CPSS_DXCH_PTP_TAI_INSTANCE_ENT)inArgs[1];
    taiId.taiNumber   = (CPSS_DXCH_PTP_TAI_NUMBER_ENT)inArgs[2];
    taiId.portNum     = (GT_PHYSICAL_PORT_NUM)inArgs[3];
    fracNanoDrift     = (GT_32)inArgs[4];

    /* call cpss api function */
    result = cpssDxChPtpTaiFractionalNanosecondDriftSet(
        devNum, &taiId, fracNanoDrift);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChPtpTaiFractionalNanosecondDriftGet
*
* DESCRIPTION:
*       Get TAI (Time Application Interface) Fractional Nanosecond Drift.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum              - device number
*       taiIdPtr            - (pointer to) TAI Units identification.
*                             Single unit must be specified.
*
* OUTPUTS:
*       fracNanoSecondPtr   - (pointer to) fractional nano seconds drift.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_BAD_PARAM             - on wrong parameter
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChPtpTaiFractionalNanosecondDriftGet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                           result;
    GT_U8                               devNum;
    CPSS_DXCH_PTP_TAI_ID_STC            taiId;
    GT_32                               fracNanoDrift;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    taiId.taiInstance = (CPSS_DXCH_PTP_TAI_INSTANCE_ENT)inArgs[1];
    taiId.taiNumber   = (CPSS_DXCH_PTP_TAI_NUMBER_ENT)inArgs[2];
    taiId.portNum     = (GT_PHYSICAL_PORT_NUM)inArgs[3];

    /* call cpss api function */
    result = cpssDxChPtpTaiFractionalNanosecondDriftGet(
        devNum, &taiId, &fracNanoDrift);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", fracNanoDrift);

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChPtpTaiPClockCycleSet
*
* DESCRIPTION:
*       Set TAI (Time Application Interface) PClock Cycle.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum          - device number
*       taiIdPtr        - (pointer to) TAI Units identification.
*       nanoSeconds     - nano seconds cycle.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_BAD_PARAM             - on wrong parameter
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChPtpTaiPClockCycleSet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                           result;
    GT_U8                               devNum;
    CPSS_DXCH_PTP_TAI_ID_STC            taiId;
    GT_U32                              pClockCycle;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    taiId.taiInstance = (CPSS_DXCH_PTP_TAI_INSTANCE_ENT)inArgs[1];
    taiId.taiNumber   = (CPSS_DXCH_PTP_TAI_NUMBER_ENT)inArgs[2];
    taiId.portNum     = (GT_PHYSICAL_PORT_NUM)inArgs[3];
    pClockCycle       = (GT_U32)inArgs[4];

    /* call cpss api function */
    result = cpssDxChPtpTaiPClockCycleSet(
        devNum, &taiId, pClockCycle);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChPtpTaiPClockCycleGet
*
* DESCRIPTION:
*       Get TAI (Time Application Interface) PClock Cycle.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum          - device number
*       taiIdPtr        - (pointer to) TAI Units identification.
*                         Single unit must be specified.
*
* OUTPUTS:
*       nanoSecondsPtr  - (pointer to) nano seconds cycle.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_BAD_PARAM             - on wrong parameter
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChPtpTaiPClockCycleGet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                           result;
    GT_U8                               devNum;
    CPSS_DXCH_PTP_TAI_ID_STC            taiId;
    GT_U32                              pClockCycle;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    taiId.taiInstance = (CPSS_DXCH_PTP_TAI_INSTANCE_ENT)inArgs[1];
    taiId.taiNumber   = (CPSS_DXCH_PTP_TAI_NUMBER_ENT)inArgs[2];
    taiId.portNum     = (GT_PHYSICAL_PORT_NUM)inArgs[3];

    /* call cpss api function */
    result = cpssDxChPtpTaiPClockCycleGet(
        devNum, &taiId, &pClockCycle);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", pClockCycle);

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChPtpTaiClockCycleSet
*
* DESCRIPTION:
*       Set TAI (Time Application Interface) Clock Cycle.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum          - device number
*       taiIdPtr        - (pointer to) TAI Units identification.
*       seconds         - seconds part of cycle.
*                         (APPLICABLE RANGES: 0..3.)
*       nanoSeconds     - nano seconds part of cycle.
*                         (APPLICABLE RANGES: 0..(2^29 - 1).)
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_BAD_PARAM             - on wrong parameter
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChPtpTaiClockCycleSet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                           result;
    GT_U8                               devNum;
    CPSS_DXCH_PTP_TAI_ID_STC            taiId;
    GT_U32                              clockCycleSec;
    GT_U32                              clockCycleNano;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    taiId.taiInstance = (CPSS_DXCH_PTP_TAI_INSTANCE_ENT)inArgs[1];
    taiId.taiNumber   = (CPSS_DXCH_PTP_TAI_NUMBER_ENT)inArgs[2];
    taiId.portNum     = (GT_PHYSICAL_PORT_NUM)inArgs[3];
    clockCycleSec     = (GT_U32)inArgs[4];
    clockCycleNano    = (GT_U32)inArgs[5];

    /* call cpss api function */
    result = cpssDxChPtpTaiClockCycleSet(
        devNum, &taiId, clockCycleSec, clockCycleNano);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChPtpTaiClockCycleGet
*
* DESCRIPTION:
*       Get TAI (Time Application Interface) Clock Cycle.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum          - device number
*       taiIdPtr        - (pointer to) TAI Units identification.
*                         Single unit must be specified.
*
* OUTPUTS:
*       secondsPtr      - (pointer to) seconds part of cycle.
*                         (APPLICABLE RANGES: 0..3.)
*       nanoSecondsPtr  - (pointer to) nano seconds part of cycle.
*                         (APPLICABLE RANGES: 0..(2^29 - 1).)
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_BAD_PARAM             - on wrong parameter
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChPtpTaiClockCycleGet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                           result;
    GT_U8                               devNum;
    CPSS_DXCH_PTP_TAI_ID_STC            taiId;
    GT_U32                              clockCycleSec;
    GT_U32                              clockCycleNano;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    taiId.taiInstance = (CPSS_DXCH_PTP_TAI_INSTANCE_ENT)inArgs[1];
    taiId.taiNumber   = (CPSS_DXCH_PTP_TAI_NUMBER_ENT)inArgs[2];
    taiId.portNum     = (GT_PHYSICAL_PORT_NUM)inArgs[3];

    /* call cpss api function */
    result = cpssDxChPtpTaiClockCycleGet(
        devNum, &taiId, &clockCycleSec, &clockCycleNano);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d%d", clockCycleSec, clockCycleNano);

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChPtpTaiTodCaptureStatusSet
*
* DESCRIPTION:
*       Set TAI (Time Application Interface) TOD Capture Status.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum          - device number
*       taiIdPtr        - (pointer to) TAI Units identification.
*       captureIndex    - Capture Index.
*                         (APPLICABLE RANGES: 0..1.)
*       valid           - TOD Capture is valid.
*                         GT_TRUE - valid, GT_FALSE - invalid
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_BAD_PARAM             - on wrong parameter
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChPtpTaiTodCaptureStatusSet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                           result;
    GT_U8                               devNum;
    CPSS_DXCH_PTP_TAI_ID_STC            taiId;
    GT_U32                              captureIndex;
    GT_BOOL                             valid;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    taiId.taiInstance = (CPSS_DXCH_PTP_TAI_INSTANCE_ENT)inArgs[1];
    taiId.taiNumber   = (CPSS_DXCH_PTP_TAI_NUMBER_ENT)inArgs[2];
    taiId.portNum     = (GT_PHYSICAL_PORT_NUM)inArgs[3];
    captureIndex      = (GT_U32)inArgs[4];
    valid             = (GT_BOOL)inArgs[5];

    /* call cpss api function */
    result = cpssDxChPtpTaiTodCaptureStatusSet(
        devNum, &taiId, captureIndex, valid);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChPtpTaiTodCaptureStatusGet
*
* DESCRIPTION:
*       Get TAI (Time Application Interface) TOD Capture Status.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum          - device number
*       taiIdPtr        - (pointer to) TAI Units identification.
*       captureIndex    - Capture Index.
*                         (APPLICABLE RANGES: 0..1.)
*
* OUTPUTS:
*       validPtr         - (pointer to)Capture is valid.
*                          GT_TRUE - valid, GT_FALSE - invalid
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_BAD_PARAM             - on wrong parameter
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChPtpTaiTodCaptureStatusGet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                           result;
    GT_U8                               devNum;
    CPSS_DXCH_PTP_TAI_ID_STC            taiId;
    GT_U32                              captureIndex;
    GT_BOOL                             valid;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    taiId.taiInstance = (CPSS_DXCH_PTP_TAI_INSTANCE_ENT)inArgs[1];
    taiId.taiNumber   = (CPSS_DXCH_PTP_TAI_NUMBER_ENT)inArgs[2];
    taiId.portNum     = (GT_PHYSICAL_PORT_NUM)inArgs[3];
    captureIndex      = (GT_U32)inArgs[4];

    /* call cpss api function */
    result = cpssDxChPtpTaiTodCaptureStatusGet(
        devNum, &taiId, captureIndex, &valid);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", valid);

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChPtpTaiTodUpdateCounterGet
*
* DESCRIPTION:
*       Get TAI (Time Application Interface) TOD Update Counter.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum          - device number
*       taiIdPtr        - (pointer to) TAI Units identification.
*
* OUTPUTS:
*       valuePtr        - (pointer to)TOD Update Counter value.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_BAD_PARAM             - on wrong parameter
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       Counter is Clear On Read. Value incremented at each
*       TOD Update/Increment/Decrement function.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChPtpTaiTodUpdateCounterGet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                           result;
    GT_U8                               devNum;
    CPSS_DXCH_PTP_TAI_ID_STC            taiId;
    GT_U32                              todUpdCnt;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    taiId.taiInstance = (CPSS_DXCH_PTP_TAI_INSTANCE_ENT)inArgs[1];
    taiId.taiNumber   = (CPSS_DXCH_PTP_TAI_NUMBER_ENT)inArgs[2];
    taiId.portNum     = (GT_PHYSICAL_PORT_NUM)inArgs[3];

    /* call cpss api function */
    result = cpssDxChPtpTaiTodUpdateCounterGet(
        devNum, &taiId, &todUpdCnt);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", todUpdCnt);

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChPtpTaiIncomingTriggerCounterSet
*
* DESCRIPTION:
*       Set TAI (Time Application Interface) Incoming Trigger Counter.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum          - device number
*       taiIdPtr        - (pointer to) TAI Units identification.
*       value           - value to set.
*                         (APPLICABLE RANGE: 0..255.)
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_BAD_PARAM             - on wrong parameter
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChPtpTaiIncomingTriggerCounterSet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                           result;
    GT_U8                               devNum;
    CPSS_DXCH_PTP_TAI_ID_STC            taiId;
    GT_U32                              inTrigCnt;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    taiId.taiInstance = (CPSS_DXCH_PTP_TAI_INSTANCE_ENT)inArgs[1];
    taiId.taiNumber   = (CPSS_DXCH_PTP_TAI_NUMBER_ENT)inArgs[2];
    taiId.portNum     = (GT_PHYSICAL_PORT_NUM)inArgs[3];
    inTrigCnt         = (GT_U32)inArgs[4];

    /* call cpss api function */
    result = cpssDxChPtpTaiIncomingTriggerCounterSet(
        devNum, &taiId, inTrigCnt);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChPtpTaiIncomingTriggerCounterGet
*
* DESCRIPTION:
*       Get TAI (Time Application Interface) Incoming Trigger Counter.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum          - device number
*       taiIdPtr        - (pointer to) TAI Units identification.
*
* OUTPUTS:
*       valuePtr        - (pointer to)value.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_BAD_PARAM             - on wrong parameter
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChPtpTaiIncomingTriggerCounterGet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                           result;
    GT_U8                               devNum;
    CPSS_DXCH_PTP_TAI_ID_STC            taiId;
    GT_U32                              inTrigCnt;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    taiId.taiInstance = (CPSS_DXCH_PTP_TAI_INSTANCE_ENT)inArgs[1];
    taiId.taiNumber   = (CPSS_DXCH_PTP_TAI_NUMBER_ENT)inArgs[2];
    taiId.portNum     = (GT_PHYSICAL_PORT_NUM)inArgs[3];

    /* call cpss api function */
    result = cpssDxChPtpTaiIncomingTriggerCounterGet(
        devNum, &taiId, &inTrigCnt);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", inTrigCnt);

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChPtpTaiIncomingClockCounterSet
*
* DESCRIPTION:
*       Set TAI (Time Application Interface) Incoming Clock Counter.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum          - device number
*       taiIdPtr        - (pointer to) TAI Units identification.
*       enable          - enable counting.
*                         GT_TRUE - enable, GT_FALSE - disable.
*       value           - value to set.
*                         (APPLICABLE RANGE: 0..(2^30 - 1).)
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_BAD_PARAM             - on wrong parameter
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChPtpTaiIncomingClockCounterSet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                           result;
    GT_U8                               devNum;
    CPSS_DXCH_PTP_TAI_ID_STC            taiId;
    GT_BOOL                             enable;
    GT_U32                              inClockCnt;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    taiId.taiInstance = (CPSS_DXCH_PTP_TAI_INSTANCE_ENT)inArgs[1];
    taiId.taiNumber   = (CPSS_DXCH_PTP_TAI_NUMBER_ENT)inArgs[2];
    taiId.portNum     = (GT_PHYSICAL_PORT_NUM)inArgs[3];
    enable            = (GT_BOOL)inArgs[4];
    inClockCnt        = (GT_U32)inArgs[5];

    /* call cpss api function */
    result = cpssDxChPtpTaiIncomingClockCounterSet(
        devNum, &taiId, enable, inClockCnt);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChPtpTaiIncomingClockCounterGet
*
* DESCRIPTION:
*       Get TAI (Time Application Interface) Incoming Clock Counter.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum          - device number
*       taiIdPtr        - (pointer to) TAI Units identification.
*
* OUTPUTS:
*       enablePtr       - (pointer to)enable counting.
*                         GT_TRUE - enable, GT_FALSE - disable.
*       valuePtr        - (pointer to)value.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_BAD_PARAM             - on wrong parameter
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChPtpTaiIncomingClockCounterGet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                           result;
    GT_U8                               devNum;
    CPSS_DXCH_PTP_TAI_ID_STC            taiId;
    GT_BOOL                             enable;
    GT_U32                              inClockCnt;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    taiId.taiInstance = (CPSS_DXCH_PTP_TAI_INSTANCE_ENT)inArgs[1];
    taiId.taiNumber   = (CPSS_DXCH_PTP_TAI_NUMBER_ENT)inArgs[2];
    taiId.portNum     = (GT_PHYSICAL_PORT_NUM)inArgs[3];

    /* call cpss api function */
    result = cpssDxChPtpTaiIncomingClockCounterGet(
        devNum, &taiId, &enable, &inClockCnt);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d%d", enable, inClockCnt);

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChPtpTaiFrequencyDriftThesholdsSet
*
* DESCRIPTION:
*       Set TAI (Time Application Interface) Frequency Drift Thesholds.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum             - device number
*       taiIdPtr           - (pointer to) TAI Units identification.
*       interruptThreshold - the drift value threshold causing interrupt.
*                            Notifies about the clock frequency out of sync.
*                           (APPLICABLE RANGE: 0..(2^24 - 1).)
*       adjustThreshold    - the drift value threshold that triggers
*                            the adjustment logic. Prevents minor jitters
*                            from affecting the drift adjustment.
*                           (APPLICABLE RANGE: 0..255.)
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_BAD_PARAM             - on wrong parameter
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChPtpTaiFrequencyDriftThesholdsSet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                           result;
    GT_U8                               devNum;
    CPSS_DXCH_PTP_TAI_ID_STC            taiId;
    GT_U32                              interruptThreshold;
    GT_U32                              adjustThreshold;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    taiId.taiInstance  = (CPSS_DXCH_PTP_TAI_INSTANCE_ENT)inArgs[1];
    taiId.taiNumber    = (CPSS_DXCH_PTP_TAI_NUMBER_ENT)inArgs[2];
    taiId.portNum      = (GT_PHYSICAL_PORT_NUM)inArgs[3];
    interruptThreshold = (GT_U32)inArgs[4];
    adjustThreshold    = (GT_U32)inArgs[5];

    /* call cpss api function */
    result = cpssDxChPtpTaiFrequencyDriftThesholdsSet(
        devNum, &taiId, interruptThreshold, adjustThreshold);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChPtpTaiFrequencyDriftThesholdsGet
*
* DESCRIPTION:
*       Get TAI (Time Application Interface) Frequency Drift Thesholds.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum                - device number
*       taiIdPtr              - (pointer to) TAI Units identification.
*
* OUTPUTS:
*       interruptThresholdPtr - (pointer to)the drift value threshold causing interrupt.
*                               Notifies about the clock frequency out of sync.
*                               (APPLICABLE RANGE: 0..(2^24 - 1).)
*       adjustThresholdPtr    - (pointer to)the drift value threshold that triggers
*                               the adjustment logic. Prevents minor jitters
*                               from affecting the drift adjustment.
*                               (APPLICABLE RANGE: 0..255.)
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_BAD_PARAM             - on wrong parameter
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChPtpTaiFrequencyDriftThesholdsGet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                           result;
    GT_U8                               devNum;
    CPSS_DXCH_PTP_TAI_ID_STC            taiId;
    GT_U32                              interruptThreshold;
    GT_U32                              adjustThreshold;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    taiId.taiInstance = (CPSS_DXCH_PTP_TAI_INSTANCE_ENT)inArgs[1];
    taiId.taiNumber   = (CPSS_DXCH_PTP_TAI_NUMBER_ENT)inArgs[2];
    taiId.portNum     = (GT_PHYSICAL_PORT_NUM)inArgs[3];

    /* call cpss api function */
    result = cpssDxChPtpTaiFrequencyDriftThesholdsGet(
        devNum, &taiId, &interruptThreshold, &adjustThreshold);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d%d", interruptThreshold, adjustThreshold);

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChPtpTaiGracefulStepSet
*
* DESCRIPTION:
*       Set TAI (Time Application Interface) Graceful Step.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum          - device number
*       taiIdPtr        - (pointer to) TAI Units identification.
*       gracefulStep    - Graceful Step in nanoseconds.
*                         (APPLICABLE RANGES: 0..32.)
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_BAD_PARAM             - on wrong parameter
*       GT_OUT_OF_RANGE          - on wrong gracefulStep
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChPtpTaiGracefulStepSet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                           result;
    GT_U8                               devNum;
    CPSS_DXCH_PTP_TAI_ID_STC            taiId;
    GT_U32                              gracefulStep;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    taiId.taiInstance  = (CPSS_DXCH_PTP_TAI_INSTANCE_ENT)inArgs[1];
    taiId.taiNumber    = (CPSS_DXCH_PTP_TAI_NUMBER_ENT)inArgs[2];
    taiId.portNum      = (GT_PHYSICAL_PORT_NUM)inArgs[3];
    gracefulStep       = (GT_U32)inArgs[4];

    /* call cpss api function */
    result = cpssDxChPtpTaiGracefulStepSet(devNum, &taiId, gracefulStep);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChPtpTaiGracefulStepGet
*
* DESCRIPTION:
*       Get TAI (Time Application Interface) Graceful Step.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum          - device number
*       taiIdPtr        - (pointer to) TAI Units identification.
*                         Single unit must be specified.
*
* OUTPUTS:
*       gracefulStepPtr - (pointer to) Graceful Step in nanoseconds.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_BAD_PARAM             - on wrong parameter
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChPtpTaiGracefulStepGet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                           result;
    GT_U8                               devNum;
    CPSS_DXCH_PTP_TAI_ID_STC            taiId;
    GT_U32                              gracefulStep;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    taiId.taiInstance = (CPSS_DXCH_PTP_TAI_INSTANCE_ENT)inArgs[1];
    taiId.taiNumber   = (CPSS_DXCH_PTP_TAI_NUMBER_ENT)inArgs[2];
    taiId.portNum     = (GT_PHYSICAL_PORT_NUM)inArgs[3];

    /* call cpss api function */
    result = cpssDxChPtpTaiGracefulStepGet(devNum, &taiId, &gracefulStep);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", gracefulStep);

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChPtpTsuControlSet
*
* DESCRIPTION:
*       Set TSU (Time Stamp Unit) Control.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum          - device number
*       portNum         - port number.
*       controlPtr      - (pointer to) control structure.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_BAD_PARAM             - on wrong parameter
*       GT_OUT_OF_RANGE          - on out-of-range parameter
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChPtpTsuControlTableSet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                       result;
    GT_U8                           devNum;
    GT_PHYSICAL_PORT_NUM            portNum;
    CPSS_DXCH_PTP_TSU_CONTROL_STC   control;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* to support the not seted/geted members of structures */
    cpssOsMemSet(&control, 0, sizeof(control));

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    /* startPort and numOfPorts arguments ignored for Set */

    portNum                     = (GT_PHYSICAL_PORT_NUM)inFields[0];
    control.unitEnable          = (GT_BOOL)inFields[1];
    control.rxTaiSelect         = (GT_U32)inFields[2];
    control.tsQueOverrideEnable = (GT_BOOL)inFields[3];

    CONVERT_DEV_PHYSICAL_PORT_MAC(devNum, portNum);

    /* call cpss api function */
    result = cpssDxChPtpTsuControlSet(
        devNum, portNum, &control);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}


/*******************************************************************************
* cpssDxChPtpTaiControlGet
*
* DESCRIPTION:
*       Get TSU (Time Stamp Unit) Control.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum          - device number
*       portNum         - port number.
*
* OUTPUTS:
*       controlPtr      - (pointer to) control structure.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_BAD_PARAM             - on wrong parameter
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChPtpTsuControlTableGet_util
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32      numFields,
    OUT GT_8       outArgs[CMD_MAX_BUFFER],
    IN  GT_BOOL   isGetFirst
)
{
    GT_STATUS rc;
    GT_U8     devNum;
    GT_U8     devNum1;
    /* variables initialised only to prevent compiler warnings */
    static GT_PHYSICAL_PORT_NUM     portNum = 0;
    static GT_U8     numOfPorts = 0;
    CPSS_DXCH_PTP_TSU_CONTROL_STC   control;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    devNum = (GT_U8)inArgs[0];
    if (isGetFirst == GT_TRUE)
    {
        devNum1     = devNum;
        portNum     = (GT_PHYSICAL_PORT_NUM)inArgs[1];
        numOfPorts  = (GT_U8)inArgs[2];
        CONVERT_DEV_PHYSICAL_PORT_MAC(devNum1, portNum);
        if (numOfPorts == 0)
        {
            /* no ports more */
            galtisOutput(outArgs, GT_OK, "%d", -1);
            return CMD_OK;
        }
        numOfPorts --;
    }
    else
    {
        if (numOfPorts == 0)
        {
            /* no ports more */
            galtisOutput(outArgs, GT_OK, "%d", -1);
            return CMD_OK;
        }
        numOfPorts --;

        /* find next existing port */
        for (portNum++; (1); portNum++)
        {
            if (portNum >= PRV_CPSS_PP_MAC(devNum)->numOfPorts)
            {
                /* no ports more */
                galtisOutput(outArgs, GT_OK, "%d", -1);
                return CMD_OK;
            }
            if (CPSS_PORTS_BMP_IS_PORT_SET_MAC(
                &(PRV_CPSS_PP_MAC(devNum)->existingPorts), portNum))
            {
                break;
            }
        }
    }


    rc = cpssDxChPtpTsuControlGet(
        devNum, portNum, &control);
    if (rc != GT_OK)
    {
        galtisOutput(outArgs, rc, "cpssDxChPtpTsuControlGet");
        return CMD_OK;
    }

    /* pack and output table fields */
    fieldOutput(
        "%d%d%d%d", portNum, control.unitEnable,
        control.rxTaiSelect, control.tsQueOverrideEnable);
    galtisOutput(outArgs, rc, "%f");

    return CMD_OK;
}

static CMD_STATUS wrCpssDxChPtpTsuControlTableGetFirst
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    return wrCpssDxChPtpTsuControlTableGet_util(
        inArgs, inFields, numFields, outArgs, GT_TRUE/*isGetFirst*/);
}

static CMD_STATUS wrCpssDxChPtpTsuControlTableGetNext
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    return wrCpssDxChPtpTsuControlTableGet_util(
        inArgs, inFields, numFields, outArgs, GT_FALSE/*isGetFirst*/);
}

/* configurable limit of amount of reading of PtpTsuTxTimestampQueue */
/* needed to avoid endless loop due to HW bug                        */
/* default - unlimited                                               */
static GT_U32 ptpTsuTxTimestampQueueTableReadLimit = 0xFFFFFFFF;
/* current amount of reading of PtpTsuTxTimestampQueue               */
static GT_U32 ptpTsuTxTimestampQueueTableReadAmount = 0;


static CMD_STATUS wrUtilPtpTsuTxTimestampQueueTableReadLimitSet
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

    ptpTsuTxTimestampQueueTableReadLimit = (GT_U32)inArgs[0];

    galtisOutput(outArgs, GT_OK, "");
    return CMD_OK;
}


/*******************************************************************************
* cpssDxChPtpTsuTxTimestampQueueRead
*
* DESCRIPTION:
*       Read TSU (Time Stamp Unit) TX Timestamp Queue Entry.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum          - device number
*       portNum         - port number.
*       queueNum        - queue number.
*                         (APPLICABLE RANGES: 0..1.)
*
* OUTPUTS:
*       entryPtr        - (pointer to) TX Timestamp Queue Entry structure.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_BAD_PARAM             - on wrong parameter
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       The Entry deleted from the Queue by reading it.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChPtpTsuTxTimestampQueueTableGet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                                          rc;
    GT_U8                                              devNum;
    GT_PHYSICAL_PORT_NUM                               portNum;
    GT_U32                                             queueNum;
    CPSS_DXCH_PTP_TSU_TX_TIMESTAMP_QUEUE_ENTRY_STC     queuEntry;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    devNum      = (GT_U8)inArgs[0];
    portNum     = (GT_PHYSICAL_PORT_NUM)inArgs[1];
    queueNum    = (GT_U32)inArgs[2];
    CONVERT_DEV_PHYSICAL_PORT_MAC(devNum, portNum);

    if (ptpTsuTxTimestampQueueTableReadAmount
        >= ptpTsuTxTimestampQueueTableReadLimit)
    {
        galtisOutput(outArgs, GT_OK, "%d", -1);
        return CMD_OK;
    }

    ptpTsuTxTimestampQueueTableReadAmount ++;

    rc = cpssDxChPtpTsuTxTimestampQueueRead(
        devNum, portNum, queueNum, &queuEntry);
    if (rc != GT_OK)
    {
        galtisOutput(outArgs, rc, "cpssDxChPtpTsuTxTimestampQueueRead");
        return CMD_OK;
    }

    if (queuEntry.entryValid == GT_FALSE)
    {
        /* no entries more */
        galtisOutput(outArgs, GT_OK, "%d", -1);
        return CMD_OK;
    }

    /* pack and output table fields */
    fieldOutput(
        "%d%d%d%d", queuEntry.entryId, queuEntry.taiSelect,
        queuEntry.todUpdateFlag, queuEntry.timestamp);
    galtisOutput(outArgs, rc, "%f");

    return CMD_OK;
}

static CMD_STATUS wrCpssDxChPtpTsuTxTimestampQueueTableGetFirst
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    ptpTsuTxTimestampQueueTableReadAmount = 0;

    return wrCpssDxChPtpTsuTxTimestampQueueTableGet(
        inArgs, inFields, numFields, outArgs);
}

/*******************************************************************************
* cpssDxChPtpTsuCountersClear
*
* DESCRIPTION:
*       Clear All TSU (Time Stamp Unit) Packet counters.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum          - device number
*       portNum         - port number.

* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong parameter
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChPtpTsuCountersClear
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS   result;
    GT_U8       devNum;
    GT_PHYSICAL_PORT_NUM       portNum;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    portNum     = (GT_PHYSICAL_PORT_NUM)inArgs[1];
    CONVERT_DEV_PHYSICAL_PORT_MAC(devNum, portNum);

    /* call cpss api function */
    result = cpssDxChPtpTsuCountersClear(
        devNum, portNum);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChPtpTsuPacketCouterGet
*
* DESCRIPTION:
*       Get TSU (Time Stamp Unit) packet counter.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum          - device number
*       portNum         - port number.
*       counterType     - counter type (see enum)
*
* OUTPUTS:
*       valuePtr        - (pointer to) counter value.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_BAD_PARAM             - on wrong parameter
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChPtpTsuPacketCouterGet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                                 result;
    GT_U8                                     devNum;
    GT_PHYSICAL_PORT_NUM                                     portNum;
    CPSS_DXCH_PTP_TSU_PACKET_COUNTER_TYPE_ENT counterType;
    GT_U32                                    conterVal;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum     = (GT_U8)inArgs[0];
    portNum    = (GT_PHYSICAL_PORT_NUM)inArgs[1];
    counterType  = (CPSS_DXCH_PTP_TSU_PACKET_COUNTER_TYPE_ENT)inArgs[2];
    CONVERT_DEV_PHYSICAL_PORT_MAC(devNum, portNum);


    /* call cpss api function */
    result = cpssDxChPtpTsuPacketCouterGet(
        devNum, portNum, counterType, &conterVal);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", conterVal);

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChPtpTsuNtpTimeOffsetSet
*
* DESCRIPTION:
*       Set TSU (Time Stamp Unit) NTP Time Offset.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum          - device number
*       portNum         - port number.
*       ntpTimeOffset   - NTP PTP Time Offset measured in seconds.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong parameter
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChPtpTsuNtpTimeOffsetSet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS   result;
    GT_U8       devNum;
    GT_PHYSICAL_PORT_NUM       portNum;
    GT_U32      ntpTimeOffset;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum         = (GT_U8)inArgs[0];
    portNum        = (GT_PHYSICAL_PORT_NUM)inArgs[1];
    ntpTimeOffset  = (GT_U8)inArgs[2];
    CONVERT_DEV_PHYSICAL_PORT_MAC(devNum, portNum);

    /* call cpss api function */
    result = cpssDxChPtpTsuNtpTimeOffsetSet(
        devNum, portNum, ntpTimeOffset);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChPtpTaiNtpTimeOffsetGet
*
* DESCRIPTION:
*       Get TSU (Time Stamp Unit) NTP Time Offset.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum           - device number
*       portNum          - port number.
*
* OUTPUTS:
*       ntpTimeOffsetPtr - (pointer to) NTP PTP Time Offset measured in seconds.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_BAD_PARAM             - on wrong parameter
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChPtpTsuNtpTimeOffsetGet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS   result;
    GT_U8       devNum;
    GT_PHYSICAL_PORT_NUM       portNum;
    GT_U32      ntpTimeOffset;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum         = (GT_U8)inArgs[0];
    portNum        = (GT_PHYSICAL_PORT_NUM)inArgs[1];
    CONVERT_DEV_PHYSICAL_PORT_MAC(devNum, portNum);

    /* call cpss api function */
    result = cpssDxChPtpTsuNtpTimeOffsetGet(
        devNum, portNum, &ntpTimeOffset);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", ntpTimeOffset);

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChPtpTsTagGlobalCfgSet
*
* DESCRIPTION:
*       Set Timestamp Global Configuration.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum              - device number
*       tsTagGlobalCfgPtr   - (pointer to) TS Tag Global Configuration.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_BAD_PARAM             - on wrong parameter
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChPtpTsTagGlobalCfgSet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                           result;
    GT_U8                               devNum;
    CPSS_DXCH_PTP_TS_TAG_GLOBAL_CFG_STC tsTagGlobalCfg;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    cpssOsMemSet(&tsTagGlobalCfg, 0, sizeof(tsTagGlobalCfg));

    /* map input arguments to locals */
    devNum                                 = (GT_U8)inArgs[0];
    tsTagGlobalCfg.tsTagParseEnable        = (GT_BOOL)inArgs[1];
    tsTagGlobalCfg.hybridTsTagParseEnable  = (GT_BOOL)inArgs[2];
    tsTagGlobalCfg.tsTagEtherType          = (GT_U32)inArgs[3];
    tsTagGlobalCfg.hybridTsTagEtherType    = (GT_U32)inArgs[4];

    /* call cpss api function */
    result = cpssDxChPtpTsTagGlobalCfgSet(
        devNum, &tsTagGlobalCfg);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChPtpTsTagGlobalCfgGet
*
* DESCRIPTION:
*       Get Timestamp Global Configuration.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum              - device number
*
* OUTPUTS:
*       tsTagGlobalCfgPtr   - (pointer to) TS Tag Global Configuration.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_BAD_PARAM             - on wrong parameter
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChPtpTsTagGlobalCfgGet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                           result;
    GT_U8                               devNum;
    CPSS_DXCH_PTP_TS_TAG_GLOBAL_CFG_STC tsTagGlobalCfg;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    cpssOsMemSet(&tsTagGlobalCfg, 0, sizeof(tsTagGlobalCfg));

    /* map input arguments to locals */
    devNum                                 = (GT_U8)inArgs[0];

    /* call cpss api function */
    result = cpssDxChPtpTsTagGlobalCfgGet(
        devNum, &tsTagGlobalCfg);

    /* pack output arguments to galtis string */
    galtisOutput(
        outArgs, result, "%d%d%d%d",
        tsTagGlobalCfg.tsTagParseEnable,
        tsTagGlobalCfg.hybridTsTagParseEnable,
        tsTagGlobalCfg.tsTagEtherType,
        tsTagGlobalCfg.hybridTsTagEtherType);

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChPtpTsTagPortCfgSet
*
* DESCRIPTION:
*       Set Timestamp Port Configuration.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum            - device number
*       portNum           - port number.
*       tsTagPortCfgPtr   - (pointer to) TS Tag Port Configuration.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_BAD_PARAM             - on wrong parameter
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChPtpTsTagPortCfgTableSet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                           result;
    GT_U8                               devNum;
        GT_PHYSICAL_PORT_NUM            portNum;
    CPSS_DXCH_PTP_TS_TAG_PORT_CFG_STC   tsTagPortCfg;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* to support the not seted/geted members of structures */
    cpssOsMemSet(&tsTagPortCfg, 0, sizeof(tsTagPortCfg));

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    /* startPort and numOfPorts arguments ignored for Set */

    portNum                        = (GT_PHYSICAL_PORT_NUM)inFields[0];
    tsTagPortCfg.tsReceptionEnable = (GT_BOOL)inFields[1];
    tsTagPortCfg.tsPiggyBackEnable = (GT_BOOL)inFields[2];
    tsTagPortCfg.tsTagMode         = (CPSS_DXCH_PTP_TS_TAG_MODE_ENT)inFields[3];

    CONVERT_DEV_PHYSICAL_PORT_MAC(devNum, portNum);

    /* call cpss api function */
    result = cpssDxChPtpTsTagPortCfgSet(
        devNum, portNum, &tsTagPortCfg);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChPtpTsTagPortCfgGet
*
* DESCRIPTION:
*       Get Timestamp Port Configuration.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum            - device number
*       portNum           - port number.
*
* OUTPUTS:
*       tsTagPortCfgPtr   - (pointer to) TS Tag Port Configuration.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_BAD_PARAM             - on wrong parameter
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChPtpTsTagPortCfgTableGet_util
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32      numFields,
    OUT GT_8       outArgs[CMD_MAX_BUFFER],
    IN  GT_BOOL    isGetFirst
)
{
    GT_STATUS rc;
    GT_U8     devNum;
    GT_U8     devNum1;
    /* variables initialised only to prevent compiler warnings */
    static GT_PHYSICAL_PORT_NUM         portNum = 0;
    static GT_U8                        numOfPorts = 0;
    CPSS_DXCH_PTP_TS_TAG_PORT_CFG_STC   tsTagPortCfg;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    devNum = (GT_U8)inArgs[0];
    if (isGetFirst == GT_TRUE)
    {
        devNum1     = devNum;
        portNum     = (GT_PHYSICAL_PORT_NUM)inArgs[1];
        numOfPorts  = (GT_U8)inArgs[2];
        CONVERT_DEV_PHYSICAL_PORT_MAC(devNum1, portNum);
        if (numOfPorts == 0)
        {
            /* no ports more */
            galtisOutput(outArgs, GT_OK, "%d", -1);
            return CMD_OK;
        }
        numOfPorts --;
    }
    else
    {
        if (numOfPorts == 0)
        {
            /* no ports more */
            galtisOutput(outArgs, GT_OK, "%d", -1);
            return CMD_OK;
        }
        numOfPorts --;

        /* find next existing port */
        for (portNum++; (1); portNum++)
        {
            if (portNum >= PRV_CPSS_PP_MAC(devNum)->numOfPorts)
            {
                /* no ports more */
                galtisOutput(outArgs, GT_OK, "%d", -1);
                return CMD_OK;
            }
            if (CPSS_PORTS_BMP_IS_PORT_SET_MAC(
                &(PRV_CPSS_PP_MAC(devNum)->existingPorts), portNum))
            {
                break;
            }
        }
    }


    rc = cpssDxChPtpTsTagPortCfgGet(
        devNum, portNum, &tsTagPortCfg);
    if (rc != GT_OK)
    {
        galtisOutput(outArgs, rc, "cpssDxChPtpTsTagPortCfgGet");
        return CMD_OK;
    }

    /* pack and output table fields */
    fieldOutput(
        "%d%d%d%d", portNum, tsTagPortCfg.tsReceptionEnable,
        tsTagPortCfg.tsPiggyBackEnable, tsTagPortCfg.tsTagMode);
    galtisOutput(outArgs, rc, "%f");

    return CMD_OK;
}

static CMD_STATUS wrCpssDxChPtpTsTagPortCfgTableGetFirst
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    return wrCpssDxChPtpTsTagPortCfgTableGet_util(
        inArgs, inFields, numFields, outArgs, GT_TRUE/*isGetFirst*/);
}

static CMD_STATUS wrCpssDxChPtpTsTagPortCfgTableGetNext
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    return wrCpssDxChPtpTsTagPortCfgTableGet_util(
        inArgs, inFields, numFields, outArgs, GT_FALSE/*isGetFirst*/);
}

/*******************************************************************************
* cpssDxChPtpTsDelayIngressPortDelaySet
*
* DESCRIPTION:
*       Set Ingress Port Delay Correction.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum              - device number
*       portNum             - port number.
*       ingrPortDelayCorr   - Ingress Port Delay Correction.
*                             (APPLICABLE RANGES: (-10^9)..(10^9-1).)
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_OUT_OF_RANGE          - on out-of-range parameter
*       GT_BAD_PARAM             - on wrong parameter
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChPtpTsDelayIngressPortDelaySet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS   result;
    GT_U8       devNum;
    GT_PHYSICAL_PORT_NUM       portNum;
    GT_32       ingrPortDelayCorr;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum             = (GT_U8)inArgs[0];
    portNum            = (GT_PHYSICAL_PORT_NUM)inArgs[1];
    ingrPortDelayCorr  = (GT_32)inArgs[2];
    CONVERT_DEV_PHYSICAL_PORT_MAC(devNum, portNum);

    /* call cpss api function */
    result = cpssDxChPtpTsDelayIngressPortDelaySet(
        devNum, portNum, ingrPortDelayCorr);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChPtpTsDelayIngressPortDelayGet
*
* DESCRIPTION:
*       Get Port Delay Correction Configuration.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum               - device number
*       portNum              - port number.
*
* OUTPUTS:
*       ingrPortDelayCorrPtr - (pointer to) Ingress Port Delay Correction.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_BAD_PARAM             - on wrong parameter
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChPtpTsDelayIngressPortDelayGet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS   result;
    GT_U8       devNum;
    GT_PHYSICAL_PORT_NUM       portNum;
    GT_32       ingrPortDelayCorr;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum            = (GT_U8)inArgs[0];
    portNum           = (GT_PHYSICAL_PORT_NUM)inArgs[1];
    CONVERT_DEV_PHYSICAL_PORT_MAC(devNum, portNum);

    /* call cpss api function */
    result = cpssDxChPtpTsDelayIngressPortDelayGet(
        devNum, portNum, &ingrPortDelayCorr);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", ingrPortDelayCorr);

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChPtpTsDelayIngressLinkDelaySet
*
* DESCRIPTION:
*       Set Ingress Link Delay.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum              - device number
*       portNum             - port number.
*       domainProfile       - domain profile
*                             (APPLICABLE RANGES: 0..8.)
*       ingressDelay        - Ingress Port Delay.
*                             (APPLICABLE RANGES: -10^9..10^9-1.)
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_OUT_OF_RANGE          - on out-of-range parameter
*       GT_BAD_PARAM             - on wrong parameter
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChPtpTsDelayIngressLinkDelaySet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS   result;
    GT_U8       devNum;
    GT_PHYSICAL_PORT_NUM       portNum;
    GT_U32      domainProfile;
    GT_32       ingrPortDelay;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum             = (GT_U8)inArgs[0];
    portNum            = (GT_PHYSICAL_PORT_NUM)inArgs[1];
    domainProfile      = (GT_U32)inArgs[2];
    ingrPortDelay      = (GT_32)inArgs[3];
    CONVERT_DEV_PHYSICAL_PORT_MAC(devNum, portNum);

    /* call cpss api function */
    result = cpssDxChPtpTsDelayIngressLinkDelaySet(
        devNum, portNum, domainProfile, ingrPortDelay);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChPtpTsDelayIngressLinkDelayGet
*
* DESCRIPTION:
*       Get Ingress Link Delay.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum               - device number
*       portNum              - port number.
*       domainProfile        - domain profile
*                              (APPLICABLE RANGES: 0..8.)
*
* OUTPUTS:
*       ingressDelayPtr      - (pointer to) Ingress Port Delay.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_BAD_PARAM             - on wrong parameter
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChPtpTsDelayIngressLinkDelayGet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS   result;
    GT_U8       devNum;
    GT_PHYSICAL_PORT_NUM       portNum;
    GT_U32      domainProfile;
    GT_32       ingrPortDelay;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum            = (GT_U8)inArgs[0];
    portNum           = (GT_PHYSICAL_PORT_NUM)inArgs[1];
    domainProfile     = (GT_U32)inArgs[2];
    CONVERT_DEV_PHYSICAL_PORT_MAC(devNum, portNum);

    /* call cpss api function */
    result = cpssDxChPtpTsDelayIngressLinkDelayGet(
        devNum, portNum, domainProfile, &ingrPortDelay);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", ingrPortDelay);

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChPtpTsDelayEgressPipeDelaySet
*
* DESCRIPTION:
*       Set Egress Pipe Delay Correction.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum              - device number
*       portNum             - port number.
*       egrPipeDelayCorr    - Egress Pipe Delay Correction.
*                             (APPLICABLE RANGES: (-10^9)..(10^9-1).)
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_OUT_OF_RANGE          - on out-of-range parameter
*       GT_BAD_PARAM             - on wrong parameter
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChPtpTsDelayEgressPipeDelaySet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS   result;
    GT_U8       devNum;
    GT_PHYSICAL_PORT_NUM       portNum;
    GT_32       egrPipeDelayCorr;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum             = (GT_U8)inArgs[0];
    portNum            = (GT_PHYSICAL_PORT_NUM)inArgs[1];
    egrPipeDelayCorr   = (GT_32)inArgs[2];
    CONVERT_DEV_PHYSICAL_PORT_MAC(devNum, portNum);

    /* call cpss api function */
    result = cpssDxChPtpTsDelayEgressPipeDelaySet(
        devNum, portNum, egrPipeDelayCorr);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChPtpTsDelayEgressPipeDelayGet
*
* DESCRIPTION:
*       Get Egress Pipe Delay Correction.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum               - device number
*       portNum              - port number.
*
* OUTPUTS:
*       egrPipeDelayCorrPtr  - (pointer to) Egress Pipe Delay Correction.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_BAD_PARAM             - on wrong parameter
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChPtpTsDelayEgressPipeDelayGet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS   result;
    GT_U8       devNum;
    GT_PHYSICAL_PORT_NUM       portNum;
    GT_32       egrPipeDelayCorr;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum            = (GT_U8)inArgs[0];
    portNum           = (GT_PHYSICAL_PORT_NUM)inArgs[1];
    CONVERT_DEV_PHYSICAL_PORT_MAC(devNum, portNum);

    /* call cpss api function */
    result = cpssDxChPtpTsDelayEgressPipeDelayGet(
        devNum, portNum, &egrPipeDelayCorr);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", egrPipeDelayCorr);

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChPtpTsDelayEgressAsymmetryCorrectionSet
*
* DESCRIPTION:
*       Set Egress Asymmetry Correction.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum              - device number
*       portNum             - port number.
*       domainNum           - domain number.
*                             (APPLICABLE RANGES: 0..8.)
*       egrAsymmetryCorr    - Egress Asymmetry Correction.
*                             (APPLICABLE RANGES: (-10^9)..(10^9-1).)
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_OUT_OF_RANGE          - on out-of-range parameter
*       GT_BAD_PARAM             - on wrong parameter
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChPtpTsDelayEgressAsymmetryCorrectionSet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS   result;
    GT_U8       devNum;
    GT_PHYSICAL_PORT_NUM       portNum;
    GT_U32      domainNum;
    GT_32       egrAsymmetryCorr;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum             = (GT_U8)inArgs[0];
    portNum            = (GT_PHYSICAL_PORT_NUM)inArgs[1];
    domainNum          = (GT_U32)inArgs[2];
    egrAsymmetryCorr   = (GT_32)inArgs[3];
    CONVERT_DEV_PHYSICAL_PORT_MAC(devNum, portNum);

    /* call cpss api function */
    result = cpssDxChPtpTsDelayEgressAsymmetryCorrectionSet(
        devNum, portNum, domainNum, egrAsymmetryCorr);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChPtpTsDelayEgressAsymmetryCorrectionGet
*
* DESCRIPTION:
*       Get Egress Asymmetry Correction.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum               - device number
*       portNum              - port number.
*       domainNum            - domain number.
*                             (APPLICABLE RANGES: 0..8.)
*
* OUTPUTS:
*       egrAsymmetryCorrPtr  - (pointer to) Egress Asymmetry Correction.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_BAD_PARAM             - on wrong parameter
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChPtpTsDelayEgressAsymmetryCorrectionGet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS   result;
    GT_U8       devNum;
    GT_PHYSICAL_PORT_NUM       portNum;
    GT_U32      domainNum;
    GT_32       egrAsymmetryCorr;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum            = (GT_U8)inArgs[0];
    portNum           = (GT_PHYSICAL_PORT_NUM)inArgs[1];
    domainNum          = (GT_U32)inArgs[2];
    CONVERT_DEV_PHYSICAL_PORT_MAC(devNum, portNum);

    /* call cpss api function */
    result = cpssDxChPtpTsDelayEgressAsymmetryCorrectionGet(
        devNum, portNum, domainNum, &egrAsymmetryCorr);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", egrAsymmetryCorr);

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChPtpTsDelayIngressCorrFieldPiggybackEnableSet
*
* DESCRIPTION:
*       Set Ingress Correction Field Piggyback Enable.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum              - device number
*       portNum             - port number.
*       ingrCorrFldPBEnable - Ingress Correction Field Piggyback Enable.
*                             GT_TRUE  - Correction field of PTP header contains
*                                        ingress timestamp.
*                             GT_FALSE - Correction field of PTP header reserved.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong parameter
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChPtpTsDelayIngressCorrFieldPiggybackEnableSet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS   result;
    GT_U8       devNum;
    GT_PHYSICAL_PORT_NUM       portNum;
    GT_BOOL     ingrCorrFldPBEnable;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum              = (GT_U8)inArgs[0];
    portNum             = (GT_PHYSICAL_PORT_NUM)inArgs[1];
    ingrCorrFldPBEnable = (GT_BOOL)inArgs[2];
    CONVERT_DEV_PHYSICAL_PORT_MAC(devNum, portNum);

    /* call cpss api function */
    result = cpssDxChPtpTsDelayIngressCorrFieldPiggybackEnableSet(
        devNum, portNum, ingrCorrFldPBEnable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChPtpTsDelayIngressCorrFieldPiggybackEnableGet
*
* DESCRIPTION:
*       Get Ingress Correction Field Piggyback Enable.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum                 - device number
*       portNum                - port number.
*
* OUTPUTS:
*       ingrCorrFldPBEnablePtr - (pointer to) Ingress Correction Field Piggyback Enable.
*                                GT_TRUE  - Correction field of PTP header contains
*                                           ingress timestamp.
*                                GT_FALSE - Correction field of PTP header reserved.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_BAD_PARAM             - on wrong parameter
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChPtpTsDelayIngressCorrFieldPiggybackEnableGet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS   result;
    GT_U8       devNum;
    GT_PHYSICAL_PORT_NUM       portNum;
    GT_BOOL     ingrCorrFldPBEnable;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum              = (GT_U8)inArgs[0];
    portNum             = (GT_PHYSICAL_PORT_NUM)inArgs[1];
    CONVERT_DEV_PHYSICAL_PORT_MAC(devNum, portNum);

    /* call cpss api function */
    result = cpssDxChPtpTsDelayIngressCorrFieldPiggybackEnableGet(
        devNum, portNum, &ingrCorrFldPBEnable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", ingrCorrFldPBEnable);

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChPtpTsDelayEgressCorrFieldPiggybackEnableSet
*
* DESCRIPTION:
*       Set Egress Correction Field Piggyback Enable.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum              - device number
*       portNum             - port number.
*       egrCorrFldPBEnable  - Egress Correction Field Piggyback Enable.
*                             GT_TRUE  - Correction field of PTP header contains
*                                        ingress timestamp.
*                             GT_FALSE - Correction field of PTP header reserved.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong parameter
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChPtpTsDelayEgressCorrFieldPiggybackEnableSet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS   result;
    GT_U8       devNum;
    GT_PHYSICAL_PORT_NUM       portNum;
    GT_BOOL     egrCorrFldPBEnable;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum              = (GT_U8)inArgs[0];
    portNum             = (GT_PHYSICAL_PORT_NUM)inArgs[1];
    egrCorrFldPBEnable  = (GT_BOOL)inArgs[2];
    CONVERT_DEV_PHYSICAL_PORT_MAC(devNum, portNum);

    /* call cpss api function */
    result = cpssDxChPtpTsDelayEgressCorrFieldPiggybackEnableSet(
        devNum, portNum, egrCorrFldPBEnable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChPtpTsDelayEgressCorrFieldPiggybackEnableGet
*
* DESCRIPTION:
*       Get Egress Correction Field Piggyback Enable.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum                 - device number
*       portNum                - port number.
*
* OUTPUTS:
*       egrCorrFldPBEnablePtr  - (pointer to) Egress Correction Field Piggyback Enable.
*                                GT_TRUE  - Correction field of PTP header contains
*                                           ingress timestamp.
*                                GT_FALSE - Correction field of PTP header reserved.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_BAD_PARAM             - on wrong parameter
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChPtpTsDelayEgressCorrFieldPiggybackEnableGet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS   result;
    GT_U8       devNum;
    GT_PHYSICAL_PORT_NUM       portNum;
    GT_BOOL     egrCorrFldPBEnable;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum              = (GT_U8)inArgs[0];
    portNum             = (GT_PHYSICAL_PORT_NUM)inArgs[1];
    CONVERT_DEV_PHYSICAL_PORT_MAC(devNum, portNum);

    /* call cpss api function */
    result = cpssDxChPtpTsDelayEgressCorrFieldPiggybackEnableGet(
        devNum, portNum, &egrCorrFldPBEnable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", egrCorrFldPBEnable);

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChPtpTsDelayEgressTimeCorrTaiSelectModeSet
*
* DESCRIPTION:
*       Set Egress Time Correction TAI Select Mode.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum                 - device number
*       portNum                - port number.
*       egrTimeCorrTaiSelMode  - Egress Time Correction TAI Select Mode.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong parameter
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChPtpTsDelayEgressTimeCorrTaiSelectModeSet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                                 result;
    GT_U8                                     devNum;
    GT_PHYSICAL_PORT_NUM                                     portNum;
    CPSS_DXCH_PTP_TS_DELAY_TAI_SEL_MODE_ENT   egrTimeCorrTaiSelMode;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum                 = (GT_U8)inArgs[0];
    portNum                = (GT_PHYSICAL_PORT_NUM)inArgs[1];
    egrTimeCorrTaiSelMode  = (CPSS_DXCH_PTP_TS_DELAY_TAI_SEL_MODE_ENT)inArgs[2];
    CONVERT_DEV_PHYSICAL_PORT_MAC(devNum, portNum);

    /* call cpss api function */
    result = cpssDxChPtpTsDelayEgressTimeCorrTaiSelectModeSet(
        devNum, portNum, egrTimeCorrTaiSelMode);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChPtpTsDelayEgressTimeCorrTaiSelectModeGet
*
* DESCRIPTION:
*       Get Egress Time Correction TAI Select Mode.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum                    - device number
*       portNum                   - port number.
*
* OUTPUTS:
*       egrTimeCorrTaiSelModePtr  - (pointer to)Egress Time Correction TAI Select Mode.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_BAD_PARAM             - on wrong parameter
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChPtpTsDelayEgressTimeCorrTaiSelectModeGet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                                 result;
    GT_U8                                     devNum;
    GT_PHYSICAL_PORT_NUM                                     portNum;
    CPSS_DXCH_PTP_TS_DELAY_TAI_SEL_MODE_ENT   egrTimeCorrTaiSelMode;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum              = (GT_U8)inArgs[0];
    portNum             = (GT_PHYSICAL_PORT_NUM)inArgs[1];
    CONVERT_DEV_PHYSICAL_PORT_MAC(devNum, portNum);

    /* call cpss api function */
    result = cpssDxChPtpTsDelayEgressTimeCorrTaiSelectModeGet(
        devNum, portNum, &egrTimeCorrTaiSelMode);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", egrTimeCorrTaiSelMode);

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChPtpTsDelayEgressTimestampTaiSelectSet
*
* DESCRIPTION:
*       Set Egress Timestamp TAI Select.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum              - device number
*       portNum             - port number.
*       egrTsTaiNum         - Egress Timestamp TAI Number.
*                             (APPLICABLE RANGES: 0..1.)
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_OUT_OF_RANGE          - on out-of-range parameter
*       GT_BAD_PARAM             - on wrong parameter
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChPtpTsDelayEgressTimestampTaiSelectSet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS    result;
    GT_U8        devNum;
    GT_PHYSICAL_PORT_NUM        portNum;
    GT_U32       egrTsTaiNum;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum       = (GT_U8)inArgs[0];
    portNum      = (GT_PHYSICAL_PORT_NUM)inArgs[1];
    egrTsTaiNum  = (GT_U32)inArgs[2];
    CONVERT_DEV_PHYSICAL_PORT_MAC(devNum, portNum);

    /* call cpss api function */
    result = cpssDxChPtpTsDelayEgressTimestampTaiSelectSet(
        devNum, portNum, egrTsTaiNum);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChPtpTsDelayEgressTimestampTaiSelectGet
*
* DESCRIPTION:
*       Get Egress Timestamp TAI Select.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum          - device number
*       portNum         - port number.
*
* OUTPUTS:
*       egrTsTaiNumPtr  - (pointer to)Egress Timestamp TAI Number.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_BAD_PARAM             - on wrong parameter
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChPtpTsDelayEgressTimestampTaiSelectGet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS    result;
    GT_U8        devNum;
    GT_PHYSICAL_PORT_NUM        portNum;
    GT_U32       egrTsTaiNum;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum     = (GT_U8)inArgs[0];
    portNum    = (GT_PHYSICAL_PORT_NUM)inArgs[1];
    CONVERT_DEV_PHYSICAL_PORT_MAC(devNum, portNum);

    /* call cpss api function */
    result = cpssDxChPtpTsDelayEgressTimestampTaiSelectGet(
        devNum, portNum, &egrTsTaiNum);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", egrTsTaiNum);

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChPtpTsUdpChecksumUpdateModeSet
*
* DESCRIPTION:
*       Set Timestamp UDP Checksum Update Mode.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum              - device number
*       udpCsUpdModePtr     - (pointer to)Timestamp UDP Checksum Update Mode.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_BAD_PARAM             - on wrong parameter
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChPtpTsUdpChecksumUpdateModeSet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                                     result;
    GT_U8                                         devNum;
    CPSS_DXCH_PTP_TS_UDP_CHECKSUM_UPDATE_MODE_STC modes;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum             = (GT_U8)inArgs[0];
    modes.ptpIpv4Mode  = (CPSS_DXCH_PTP_TS_UDP_CHECKSUM_UPDATE_MODE_ENT)inArgs[1];
    modes.ptpIpv6Mode  = (CPSS_DXCH_PTP_TS_UDP_CHECKSUM_UPDATE_MODE_ENT)inArgs[2];
    modes.ntpIpv4Mode  = (CPSS_DXCH_PTP_TS_UDP_CHECKSUM_UPDATE_MODE_ENT)inArgs[3];
    modes.ntpIpv6Mode  = (CPSS_DXCH_PTP_TS_UDP_CHECKSUM_UPDATE_MODE_ENT)inArgs[4];
    modes.wampIpv4Mode = (CPSS_DXCH_PTP_TS_UDP_CHECKSUM_UPDATE_MODE_ENT)inArgs[5];
    modes.wampIpv6Mode = (CPSS_DXCH_PTP_TS_UDP_CHECKSUM_UPDATE_MODE_ENT)inArgs[6];

    /* call cpss api function */
    result = cpssDxChPtpTsUdpChecksumUpdateModeSet(
        devNum, &modes);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChPtpTsUdpChecksumUpdateModeGet
*
* DESCRIPTION:
*       Get Timestamp UDP Checksum Update Mode.
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum              - device number
*
* OUTPUTS:
*       udpCsUpdModePtr     - (pointer to)Timestamp UDP Checksum Update Mode.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_BAD_PARAM             - on wrong parameter
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChPtpTsUdpChecksumUpdateModeGet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                                     result;
    GT_U8                                         devNum;
    CPSS_DXCH_PTP_TS_UDP_CHECKSUM_UPDATE_MODE_STC modes;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum     = (GT_U8)inArgs[0];

    /* call cpss api function */
    result = cpssDxChPtpTsUdpChecksumUpdateModeGet(
        devNum, &modes);

    /* pack output arguments to galtis string */
    galtisOutput(
        outArgs, result, "%d%d%d%d%d%d%d",
        modes.ptpIpv4Mode, modes.ptpIpv6Mode,
        modes.ntpIpv4Mode, modes.ntpIpv6Mode,
        modes.wampIpv4Mode, modes.wampIpv6Mode);

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChPtpOverEthernetEnableSet
*
* DESCRIPTION:
*       Set enable PTP over Ethernet packet.
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum         - device number
*       enable         - GT_TRUE - enable, GT_FALSE - disable.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong parameter
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChPtpOverEthernetEnableSet
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
    devNum  = (GT_U8)inArgs[0];
    enable  = (GT_BOOL)inArgs[1];

    /* call cpss api function */
    result = cpssDxChPtpOverEthernetEnableSet(
        devNum, enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChPtpOverEthernetEnableGet
*
* DESCRIPTION:
*       Get enable PTP over Ethernet packet.
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum          - device number
*
* OUTPUTS:
*       enablePtr       - (pointer to) GT_TRUE - enable, GT_FALSE - disable.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong parameter
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChPtpOverEthernetEnableGet
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
    devNum     = (GT_U8)inArgs[0];

    /* call cpss api function */
    result = cpssDxChPtpOverEthernetEnableGet(
        devNum, &enable);

    /* pack output arguments to galtis string */
    galtisOutput(
        outArgs, result, "%d", enable);

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChPtpOverUdpEnableSet
*
* DESCRIPTION:
*       Set enable PTP over UDP packet.
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum         - device number
*       enable         - GT_TRUE - enable, GT_FALSE - disable.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong parameter
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChPtpOverUdpEnableSet
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
    devNum  = (GT_U8)inArgs[0];
    enable  = (GT_BOOL)inArgs[1];

    /* call cpss api function */
    result = cpssDxChPtpOverUdpEnableSet(
        devNum, enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChPtpOverUdpEnableGet
*
* DESCRIPTION:
*       Get enable PTP over UDP packet.
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum          - device number
*
* OUTPUTS:
*       enablePtr       - (pointer to) GT_TRUE - enable, GT_FALSE - disable.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong parameter
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChPtpOverUdpEnableGet
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
    devNum     = (GT_U8)inArgs[0];

    /* call cpss api function */
    result = cpssDxChPtpOverUdpEnableGet(
        devNum, &enable);

    /* pack output arguments to galtis string */
    galtisOutput(
        outArgs, result, "%d", enable);

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChPtpIngressExceptionCfgSet
*
* DESCRIPTION:
*       Set PTP packet Ingress Exception Configuration.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum              - device number
*       ingrExceptionCfgPtr  - (pointer to) PTP packet Ingress Exception Configuration.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_BAD_PARAM             - on wrong parameter
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChPtpIngressExceptionCfgSet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                               result;
    GT_U8                                   devNum;
    CPSS_DXCH_PTP_INGRESS_EXCEPTION_CFG_STC cfg;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum                         = (GT_U8)inArgs[0];
    cfg.ptpExceptionCommandEnable  = (GT_BOOL)inArgs[1];
    cfg.ptpExceptionCommand        = (CPSS_PACKET_CMD_ENT)inArgs[2];
    cfg.ptpExceptionCpuCode        = (CPSS_NET_RX_CPU_CODE_ENT)inArgs[3];
    cfg.ptpVersionCheckEnable      = (GT_BOOL)inArgs[4];

    /* call cpss api function */
    result = cpssDxChPtpIngressExceptionCfgSet(
        devNum, &cfg);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChPtpIngressExceptionCfgGet
*
* DESCRIPTION:
*       Get PTP packet Ingress Exception Configuration.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum               - device number
*
* OUTPUTS:
*       ingrExceptionCfgPtr  - (pointer to) PTP packet Ingress Exception Configuration.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_BAD_PARAM             - on wrong parameter
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChPtpIngressExceptionCfgGet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                               result;
    GT_U8                                   devNum;
    CPSS_DXCH_PTP_INGRESS_EXCEPTION_CFG_STC cfg;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum     = (GT_U8)inArgs[0];

    /* call cpss api function */
    result = cpssDxChPtpIngressExceptionCfgGet(
        devNum, &cfg);

    /* pack output arguments to galtis string */
    galtisOutput(
        outArgs, result, "%d%d%d%d",
        cfg.ptpExceptionCommandEnable, cfg.ptpExceptionCommand,
        cfg.ptpExceptionCpuCode, cfg.ptpVersionCheckEnable);

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChPtpEgressExceptionCfgSet
*
* DESCRIPTION:
*       Set PTP packet Egress Exception Configuration.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum              - device number
*       egrExceptionCfgPtr  - (pointer to) PTP packet Egress Exception Configuration.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_BAD_PARAM             - on wrong parameter
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChPtpEgressExceptionCfgSet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                               result;
    GT_U8                                   devNum;
    CPSS_DXCH_PTP_EGRESS_EXCEPTION_CFG_STC  cfg;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum                         = (GT_U8)inArgs[0];
    cfg.invalidPtpPktCmd           = (CPSS_PACKET_CMD_ENT)inArgs[1];
    cfg.invalidOutPiggybackPktCmd  = (CPSS_PACKET_CMD_ENT)inArgs[2];
    cfg.invalidInPiggybackPktCmd   = (CPSS_PACKET_CMD_ENT)inArgs[3];
    cfg.invalidTsPktCmd            = (CPSS_PACKET_CMD_ENT)inArgs[4];

    /* call cpss api function */
    result = cpssDxChPtpEgressExceptionCfgSet(
        devNum, &cfg);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChPtpEgressExceptionCfgGet
*
* DESCRIPTION:
*       Get PTP packet Egress Exception Configuration.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum               - device number
*
* OUTPUTS:
*       egrExceptionCfgPtr  - (pointer to) PTP packet Egress Exception Configuration.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_BAD_PARAM             - on wrong parameter
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChPtpEgressExceptionCfgGet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                               result;
    GT_U8                                   devNum;
    CPSS_DXCH_PTP_EGRESS_EXCEPTION_CFG_STC  cfg;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum     = (GT_U8)inArgs[0];

    /* call cpss api function */
    result = cpssDxChPtpEgressExceptionCfgGet(
        devNum, &cfg);

    /* pack output arguments to galtis string */
    galtisOutput(
        outArgs, result, "%d%d%d%d",
        cfg.invalidPtpPktCmd, cfg.invalidOutPiggybackPktCmd,
        cfg.invalidInPiggybackPktCmd, cfg.invalidTsPktCmd);

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChPtpIngressExceptionCounterGet
*
* DESCRIPTION:
*       Get PTP Ingress Exception packet Counter.
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
*       counterPtr  - (pointer to) PTP Ingress Exception packet Counter.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_BAD_PARAM             - on wrong parameter
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       The counter is Clear On Read.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChPtpIngressExceptionCounterGet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS  result;
    GT_U8      devNum;
    GT_U32     counter;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum     = (GT_U8)inArgs[0];

    /* call cpss api function */
    result = cpssDxChPtpIngressExceptionCounterGet(
        devNum, &counter);

    /* pack output arguments to galtis string */
    galtisOutput(
        outArgs, result, "%d", counter);

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChPtpEgressExceptionCountersGet
*
* DESCRIPTION:
*       Get PTP packet Egress Exception Counters.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum               - device number
*
* OUTPUTS:
*       egrExceptionCntPtr  - (pointer to) PTP packet Egress Exception Counters.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_BAD_PARAM             - on wrong parameter
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       The counters are Clear On Read.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChPtpEgressExceptionCountersGet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                                   result;
    GT_U8                                       devNum;
    CPSS_DXCH_PTP_EGRESS_EXCEPTION_COUNTERS_STC counter;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum     = (GT_U8)inArgs[0];

    /* call cpss api function */
    result = cpssDxChPtpEgressExceptionCountersGet(
        devNum, &counter);

    /* pack output arguments to galtis string */
    galtisOutput(
        outArgs, result, "%d%d%d%d",
        counter.invalidPtpPktCnt, counter.invalidOutPiggybackPktCnt,
        counter.invalidInPiggybackPktCnt, counter.invalidTsPktCnt);

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChPtpTsMessageTypeToQueueIdMapSet
*
* DESCRIPTION:
*       Set PTP Message Type To Queue Id Map.
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum         - device number
*       idMapBmp       - PTP Message Type To Queue Id Map.
*                        16 bits, bit per message type
*                        0 - queue0, 1 - queue1
*                        (APPLICABLE RANGES: 0..0xFFFF.)
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_OUT_OF_RANGE          - on out-of-range parameter
*       GT_BAD_PARAM             - on wrong parameter
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChPtpTsMessageTypeToQueueIdMapSet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS    result;
    GT_U8        devNum;
    GT_U32       idMapBmp;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum    = (GT_U8)inArgs[0];
    idMapBmp  = (GT_U32)inArgs[1];

    /* call cpss api function */
    result = cpssDxChPtpTsMessageTypeToQueueIdMapSet(
        devNum, idMapBmp);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChPtpTsMessageTypeToQueueIdMapGet
*
* DESCRIPTION:
*       Get PTP Message Type To Queue Id Map.
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum        - device number
*
* OUTPUTS:
*       idMapBmpPtr    - (pointer to) PTP Message Type To Queue Id Map.
*                        16 bits, bit per message type
*                        0 - queue0, 1 - queue1
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong parameter
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChPtpTsMessageTypeToQueueIdMapGet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS    result;
    GT_U8        devNum;
    GT_U32       idMapBmp;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum     = (GT_U8)inArgs[0];

    /* call cpss api function */
    result = cpssDxChPtpTsMessageTypeToQueueIdMapGet(
        devNum, &idMapBmp);

    /* pack output arguments to galtis string */
    galtisOutput(
        outArgs, result, "%d", idMapBmp);

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChPtpTsQueuesEntryOverrideEnableSet
*
* DESCRIPTION:
*       Set Timestamp Queues Override Enable.
*       Related to all 2 ingress and 2 egress Timestamp Queues.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum          - device number
*       enable          - when the TS Queue is full the new timestamp
*                         GT_TRUE  - overrides an oldest Queue entry.
*                         GT_FALSE - not queued.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_BAD_PARAM             - on wrong parameter
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       cpssDxChPtpTsuControlSet should configure the same value per port
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChPtpTsQueuesEntryOverrideEnableSet
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
    devNum  = (GT_U8)inArgs[0];
    enable  = (GT_BOOL)inArgs[1];

    /* call cpss api function */
    result = cpssDxChPtpTsQueuesEntryOverrideEnableSet(
        devNum, enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChPtpTsQueuesEntryOverrideEnableGet
*
* DESCRIPTION:
*       Get Timestamp Queues Override Enable.
*       Related to all 2 ingress and 2 egress Timestamp Queues.
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
*       enablePtr       - (pointer to)when the TS Queue is full the new timestamp
*                         GT_TRUE  - overrides an oldest Queue entry.
*                         GT_FALSE - not queued.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_BAD_PARAM             - on wrong parameter
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChPtpTsQueuesEntryOverrideEnableGet
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
    devNum     = (GT_U8)inArgs[0];

    /* call cpss api function */
    result = cpssDxChPtpTsQueuesEntryOverrideEnableGet(
        devNum, &enable);

    /* pack output arguments to galtis string */
    galtisOutput(
        outArgs, result, "%d", enable);

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChPtpTsQueuesSizeSet
*
* DESCRIPTION:
*       Set Timestamp Queues Size.
*       Related to all 2 ingress and 2 egress Timestamp Queues.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum          - device number
*       queueSize       - The size of each PTP Queue.
*                         (APPLICABLE RANGES: 0..256.)
*                         If Timestamp Queues Override is Enable,
*                         the size should be (maximal - 2) == 254.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_BAD_PARAM             - on wrong parameter
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChPtpTsQueuesSizeSet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS    result;
    GT_U8        devNum;
    GT_U32       queueSize;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum    = (GT_U8)inArgs[0];
    queueSize = (GT_U32)inArgs[1];

    /* call cpss api function */
    result = cpssDxChPtpTsQueuesSizeSet(
        devNum, queueSize);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChPtpTsQueuesSizeGet
*
* DESCRIPTION:
*       Get Timestamp Queues Size.
*       Related to all 2 ingress and 2 egress Timestamp Queues.
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
*       queueSizePtr    - (pointer to)The size of each PTP Queue.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_BAD_PARAM             - on wrong parameter
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChPtpTsQueuesSizeGet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS    result;
    GT_U8        devNum;
    GT_U32       queueSize;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum     = (GT_U8)inArgs[0];

    /* call cpss api function */
    result = cpssDxChPtpTsQueuesSizeGet(
        devNum, &queueSize);

    /* pack output arguments to galtis string */
    galtisOutput(
        outArgs, result, "%d", queueSize);

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChPtpTsDebugQueuesEntryIdsClear
*
* DESCRIPTION:
*       Set Timestamp Queues Current Entry Ids Clear.
*       For Debug only.
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
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong parameter
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChPtpTsDebugQueuesEntryIdsClear
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
    devNum    = (GT_U8)inArgs[0];

    /* call cpss api function */
    result = cpssDxChPtpTsDebugQueuesEntryIdsClear(
        devNum);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChPtpTsDebugQueuesEntryIdsGet
*
* DESCRIPTION:
*       Get Timestamp Queues Current Entry IDs.
*       For Debug only.
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
*       queueEntryId0Ptr    - (pointer to)The PTP Queue0 current entry Id.
*       queueEntryId1Ptr    - (pointer to)The PTP Queue1 current entry Id.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_BAD_PARAM             - on wrong parameter
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChPtpTsDebugQueuesEntryIdsGet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS    result;
    GT_U8        devNum;
    GT_U32       queueEntryId0;
    GT_U32       queueEntryId1;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum     = (GT_U8)inArgs[0];

    /* call cpss api function */
    result = cpssDxChPtpTsDebugQueuesEntryIdsGet(
        devNum, &queueEntryId0, &queueEntryId1);

    /* pack output arguments to galtis string */
    galtisOutput(
        outArgs, result, "%d%d", queueEntryId0, queueEntryId1);

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChPtpTsNtpTimeOffsetSet
*
* DESCRIPTION:
*       Set TS NTP Time Offset.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum          - device number
*       ntpTimeOffset   - NTP PTP Time Offset measured in seconds.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong parameter
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       The same value should be configured per port.
*       See cpssDxChPtpTsuNtpTimeOffsetSet.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChPtpTsNtpTimeOffsetSet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS    result;
    GT_U8        devNum;
    GT_U32       ntpTimeOffset;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum        = (GT_U8)inArgs[0];
    ntpTimeOffset = (GT_U32)inArgs[1];

    /* call cpss api function */
    result = cpssDxChPtpTsNtpTimeOffsetSet(
        devNum, ntpTimeOffset);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChPtpTsNtpTimeOffsetGet
*
* DESCRIPTION:
*       Get TS NTP Time Offset.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum           - device number
*
* OUTPUTS:
*       ntpTimeOffsetPtr - (pointer to) NTP PTP Time Offset measured in seconds.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_BAD_PARAM             - on wrong parameter
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChPtpTsNtpTimeOffsetGet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS    result;
    GT_U8        devNum;
    GT_U32       ntpTimeOffset;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum     = (GT_U8)inArgs[0];

    /* call cpss api function */
    result = cpssDxChPtpTsNtpTimeOffsetGet(
        devNum, &ntpTimeOffset);

    /* pack output arguments to galtis string */
    galtisOutput(
        outArgs, result, "%d", ntpTimeOffset);

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChPtpMessageTypeCmdSet
*
* DESCRIPTION:
*       Configure packet command per PTP message type.
*       The message type is extracted from the PTP header.
*
* APPLICABLE DEVICES:
*        Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3.
*
* INPUTS:
*       devNum          - device number
*       messageType     - message type
*                         (APPLICABLE RANGES: 0..15.)
*       domainNum       - domainNum
*                         (APPLICABLE RANGES: 0..4.)
*                         (APPLICABLE DEVICES: Bobcat2; Lion3.)
*       command         - assigned command to the packet.
*                         Acceptable values: FORWARD, MIRROR, TRAP, HARD_DROP and
*                                            SOFT_DROP.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong devNum, messageType or command
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChPtpMessageTypeCmdTableSet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                rc;
    GT_U8                    devNum;
    GT_PHYSICAL_PORT_NUM     portNum;
    GT_U32                   domainNum;
    GT_U32                   messageType;
    CPSS_PACKET_CMD_ENT      command;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum      = (GT_U8)inArgs[0];
    portNum     = (GT_PHYSICAL_PORT_NUM)inArgs[1];
    domainNum   = (GT_U32)inArgs[2];
    messageType = (GT_U32)inFields[0];
    command     = (CPSS_PACKET_CMD_ENT)inFields[1];

    /* call cpss api function */
    rc = cpssDxChPtpMessageTypeCmdSet(
        devNum, portNum, domainNum, messageType, command);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, rc, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChPtpMessageTypeCmdGet
*
* DESCRIPTION:
*       Get packet command per PTP message type.
*
* APPLICABLE DEVICES:
*        Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3.
*
* INPUTS:
*       devNum          - device number
*       domainNum       - domainNum
*                         (APPLICABLE RANGES: 0..4.)
*                         (APPLICABLE DEVICES: Bobcat2; Lion3.)
*       messageType     - message type
*                         (APPLICABLE RANGES: 0..15.)
*
* OUTPUTS:
*       commandPtr      - (pointer to) assigned command to the packet.
*                         Acceptable values: FORWARD, MIRROR, TRAP, HARD_DROP and
*                                            SOFT_DROP.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong devNum or messageType
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_BAD_STATE             - on bad value found in HW
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChPtpMessageTypeCmdTableGet_util
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32      numFields,
    OUT GT_8       outArgs[CMD_MAX_BUFFER],
    IN  GT_BOOL    isGetFirst
)
{
    GT_STATUS                rc;
    GT_U8                    devNum;
    GT_PHYSICAL_PORT_NUM     portNum;
    GT_U32                   domainNum;
    /* variables initialised only to prevent compiler warnings */
    static GT_U32            messageType = 0;
    CPSS_PACKET_CMD_ENT      command;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    devNum      = (GT_U8)inArgs[0];
    portNum     = (GT_PHYSICAL_PORT_NUM)inArgs[1];
    domainNum   = (GT_U32)inArgs[2];

    if (isGetFirst == GT_TRUE)
    {
        messageType = 0;
    }
    else
    {
        messageType ++;
        if (messageType > 15)
        {
            /* no entries more */
            galtisOutput(outArgs, GT_OK, "%d", -1);
            return CMD_OK;
        }
    }

    rc = cpssDxChPtpMessageTypeCmdGet(
        devNum, portNum, domainNum, messageType, &command);
    if (rc != GT_OK)
    {
        galtisOutput(outArgs, rc, "cpssDxChPtpMessageTypeCmdGet");
        return CMD_OK;
    }

    /* pack and output table fields */
    fieldOutput(
        "%d%d", messageType, command);
    galtisOutput(outArgs, rc, "%f");

    return CMD_OK;
}

static CMD_STATUS wrCpssDxChPtpMessageTypeCmdTableGetFirst
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    return wrCpssDxChPtpMessageTypeCmdTableGet_util(
        inArgs, inFields, numFields, outArgs, GT_TRUE/*isGetFirst*/);
}

static CMD_STATUS wrCpssDxChPtpMessageTypeCmdTableGetNext
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    return wrCpssDxChPtpMessageTypeCmdTableGet_util(
        inArgs, inFields, numFields, outArgs, GT_FALSE/*isGetFirst*/);
}

/*******************************************************************************
* cpssDxChPtpTsCfgTableSet
*
* DESCRIPTION:
*       Set Timestamp Configuration Table Entry.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum              - device number
*       entryIndex          - entry index.
*                             (APPLICABLE RANGES: 0..127.)
*       entryPtr            - (pointer to)Timestamp Configuration Table Entry.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_OUT_OF_RANGE          - on out-of-range parameter
*       GT_BAD_PARAM             - on wrong parameter
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChPtpTsCfgTableSet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                      rc;
    GT_U8                          devNum;
    GT_U32                         entryIndex;
    CPSS_DXCH_PTP_TS_CFG_ENTRY_STC entry;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum                            = (GT_U8)inArgs[0];
    entryIndex                        = (GT_U32)inFields[0];
    entry.tsMode                      = (CPSS_DXCH_PTP_TS_TIMESTAMPING_MODE_ENT)inFields[1];
    entry.offsetProfile               = (GT_U32                                )inFields[2];
    entry.OE                          = (GT_BOOL                               )inFields[3];
    entry.tsAction                    = (CPSS_DXCH_PTP_TS_ACTION_ENT           )inFields[4];
    entry.packetFormat                = (CPSS_DXCH_PTP_TS_PACKET_TYPE_ENT      )inFields[5];
    entry.ptpTransport                = (CPSS_DXCH_PTP_TRANSPORT_TYPE_ENT      )inFields[6];
    entry.offset                      = (GT_U32                                )inFields[7];
    entry.ptpMessageType              = (GT_U32                                )inFields[8];
    entry.domain                      = (GT_U32                                )inFields[9];
    entry.ingrLinkDelayEnable         = (GT_BOOL                               )inFields[10];
    entry.packetDispatchingEnable     = (GT_BOOL                               )inFields[11];

    /* call cpss api function */
    rc = cpssDxChPtpTsCfgTableSet(
        devNum, entryIndex, &entry);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, rc, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChPtpTsCfgTableGet
*
* DESCRIPTION:
*       Get Timestamp Configuration Table Entry.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum              - device number
*       entryIndex          - entry index.
*                             (APPLICABLE RANGES: 0..127.)
*
* OUTPUTS:
*       entryPtr            - (pointer to)Timestamp Configuration Table Entry.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_BAD_PARAM             - on wrong parameter
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChPtpTsCfgTableGet_util
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32      numFields,
    OUT GT_8       outArgs[CMD_MAX_BUFFER],
    IN  GT_BOOL    isGetFirst
)
{
    GT_STATUS                      rc;
    GT_U8                          devNum;
    /* variables initialised only to prevent compiler warnings */
    static GT_U32                  entryIndex = 0;
    static GT_U32                  entriesAmount = 0;
    CPSS_DXCH_PTP_TS_CFG_ENTRY_STC entry;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    devNum      = (GT_U8)inArgs[0];

    if (isGetFirst == GT_TRUE)
    {
        entryIndex      = (GT_U8)inArgs[1];
        entriesAmount   = (GT_U32)inArgs[2];
    }
    else
    {
        entryIndex      ++;
        entriesAmount   --;
    }

    if (entriesAmount == 0)
    {
        /* no entries more */
        galtisOutput(outArgs, GT_OK, "%d", -1);
        return CMD_OK;
    }

    rc = cpssDxChPtpTsCfgTableGet(
        devNum, entryIndex, &entry);
    if (rc != GT_OK)
    {
        galtisOutput(outArgs, rc, "cpssDxChPtpTsCfgTableGet");
        return CMD_OK;
    }

    /* pack and output table fields */
    fieldOutput(
        "%d%d%d%d%d%d%d%d%d%d%d%d",
        entryIndex,
        entry.tsMode,
        entry.offsetProfile,
        entry.OE,
        entry.tsAction,
        entry.packetFormat,
        entry.ptpTransport,
        entry.offset,
        entry.ptpMessageType,
        entry.domain,
        entry.ingrLinkDelayEnable,
        entry.packetDispatchingEnable);
    galtisOutput(outArgs, rc, "%f");

    return CMD_OK;
}

static CMD_STATUS wrCpssDxChPtpTsCfgTableGetFirst
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    return wrCpssDxChPtpTsCfgTableGet_util(
        inArgs, inFields, numFields, outArgs, GT_TRUE/*isGetFirst*/);
}

static CMD_STATUS wrCpssDxChPtpTsCfgTableGetNext
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    return wrCpssDxChPtpTsCfgTableGet_util(
        inArgs, inFields, numFields, outArgs, GT_FALSE/*isGetFirst*/);
}

/*******************************************************************************
* cpssDxChPtpTsLocalActionTableSet
*
* DESCRIPTION:
*       Set Timestamp Local Action Table Entry.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum              - device number
*       portNum             - port number.
*       domainNum           - domain number.
*                             (APPLICABLE RANGES: 0..8.)
*       messageType         - message type
*                             (APPLICABLE RANGES: 0..15.)
*       entryPtr            - (pointer to)Timestamp Local Action Table Entry.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_BAD_PARAM             - on wrong parameter
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChPtpTsLocalActionTableSet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                               rc;
    GT_U8                                   devNum;
    GT_PHYSICAL_PORT_NUM                    portNum;
    GT_U32                                  domainNum;
    GT_U32                                  messageType;
    CPSS_DXCH_PTP_TS_LOCAL_ACTION_ENTRY_STC entry;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum                            = (GT_U8)inArgs[0];
    portNum                           = (GT_PHYSICAL_PORT_NUM)inArgs[1];
    domainNum                         = (GT_U8)inArgs[2];
    messageType                       = (GT_U32)inFields[0];
    entry.tsAction                    = (CPSS_DXCH_PTP_TS_ACTION_ENT)inFields[1];
    entry.ingrLinkDelayEnable         = (GT_BOOL)inFields[2];
    entry.packetDispatchingEnable     = (GT_BOOL)inFields[3];

    /* Override Device and Port */
    CONVERT_DEV_PHYSICAL_PORT_MAC(devNum, portNum);

    /* call cpss api function */
    rc = cpssDxChPtpTsLocalActionTableSet(
        devNum, portNum, domainNum, messageType, &entry);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, rc, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChPtpTsLocalActionTableGet
*
* DESCRIPTION:
*       Get Timestamp Local Action Table Entry.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum              - device number
*       portNum             - port number.
*       domainNum           - domain number.
*                             (APPLICABLE RANGES: 0..8.)
*       messageType         - message type
*                             (APPLICABLE RANGES: 0..15.)
*
* OUTPUTS:
*       entryPtr            - (pointer to)Timestamp Local Action Table Entry.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_BAD_PARAM             - on wrong parameter
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChPtpTsLocalActionTableGet_util
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32      numFields,
    OUT GT_8       outArgs[CMD_MAX_BUFFER],
    IN  GT_BOOL    isGetFirst
)
{
    GT_STATUS                               rc;
    GT_U8                                   devNum;
    GT_PHYSICAL_PORT_NUM                    portNum;
    GT_U32                                  domainNum;
    /* variables initialised only to prevent compiler warnings */
    static GT_U32                           messageType = 0;
    CPSS_DXCH_PTP_TS_LOCAL_ACTION_ENTRY_STC entry;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum                            = (GT_U8)inArgs[0];
    portNum                           = (GT_PHYSICAL_PORT_NUM)inArgs[1];
    domainNum                         = (GT_U8)inArgs[2];

    if (isGetFirst == GT_TRUE)
    {
        messageType = 0;
    }
    else
    {
        messageType ++;
    }

    if (messageType > 15)
    {
        /* no entries more */
        galtisOutput(outArgs, GT_OK, "%d", -1);
        return CMD_OK;
    }

    rc = cpssDxChPtpTsLocalActionTableGet(
        devNum, portNum, domainNum, messageType, &entry);
    if (rc != GT_OK)
    {
        galtisOutput(outArgs, rc, "cpssDxChPtpTsLocalActionTableGet");
        return CMD_OK;
    }

    /* pack and output table fields */
    fieldOutput(
        "%d%d%d%d",
        messageType,
        entry.tsAction,
        entry.ingrLinkDelayEnable,
        entry.packetDispatchingEnable);
    galtisOutput(outArgs, rc, "%f");

    return CMD_OK;
}

static CMD_STATUS wrCpssDxChPtpTsLocalActionTableGetFirst
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    return wrCpssDxChPtpTsLocalActionTableGet_util(
        inArgs, inFields, numFields, outArgs, GT_TRUE/*isGetFirst*/);
}

static CMD_STATUS wrCpssDxChPtpTsLocalActionTableGetNext
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    return wrCpssDxChPtpTsLocalActionTableGet_util(
        inArgs, inFields, numFields, outArgs, GT_FALSE/*isGetFirst*/);
}

/*******************************************************************************
* cpssDxChPtpDomainModeSet
*
* DESCRIPTION:
*       Set PTP domain mode.
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum         - device number
*       domainNum      - domain number.
*                        (APPLICABLE RANGES: 0..3.)
*       domainMode     - domain mode
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong parameter
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChPtpDomainModeTableSet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                               rc;
    GT_U8                                   devNum;
    GT_U32                                  domainNum;
    CPSS_DXCH_PTP_INGRESS_DOMAIN_MODE_ENT   domainMode;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum      = (GT_U8)inArgs[0];
    domainNum   = (GT_U32)inFields[0];
    domainMode  = (CPSS_DXCH_PTP_INGRESS_DOMAIN_MODE_ENT)inFields[1];

    /* call cpss api function */
    rc = cpssDxChPtpDomainModeSet(
        devNum, domainNum, domainMode);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, rc, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChPtpDomainModeGet
*
* DESCRIPTION:
*       Get PTP domain mode.
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum          - device number
*       domainNum       - domain number.
*                         (APPLICABLE RANGES: 0..3.)
*
* OUTPUTS:
*       domainModePtr   - (pointer to) domain mode
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong parameter
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChPtpDomainModeTableGet_util
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32      numFields,
    OUT GT_8       outArgs[CMD_MAX_BUFFER],
    IN  GT_BOOL    isGetFirst
)
{
    GT_STATUS                               rc;
    GT_U8                                   devNum;
    /* variables initialised only to prevent compiler warnings */
    static GT_U32                           domainNum = 0;
    CPSS_DXCH_PTP_INGRESS_DOMAIN_MODE_ENT   domainMode;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum                            = (GT_U8)inArgs[0];

    if (isGetFirst == GT_TRUE)
    {
        domainNum = 0;
    }
    else
    {
        domainNum ++;
    }

    if (domainNum > 3)
    {
        /* no entries more */
        galtisOutput(outArgs, GT_OK, "%d", -1);
        return CMD_OK;
    }

    rc = cpssDxChPtpDomainModeGet(
        devNum, domainNum, &domainMode);
    if (rc != GT_OK)
    {
        galtisOutput(outArgs, rc, "cpssDxChPtpDomainModeGet");
        return CMD_OK;
    }

    /* pack and output table fields */
    fieldOutput(
        "%d%d", domainNum, domainMode);
    galtisOutput(outArgs, rc, "%f");

    return CMD_OK;
}

static CMD_STATUS wrCpssDxChPtpDomainModeTableGetFirst
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    return wrCpssDxChPtpDomainModeTableGet_util(
        inArgs, inFields, numFields, outArgs, GT_TRUE/*isGetFirst*/);
}

static CMD_STATUS wrCpssDxChPtpDomainModeTableGetNext
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    return wrCpssDxChPtpDomainModeTableGet_util(
        inArgs, inFields, numFields, outArgs, GT_FALSE/*isGetFirst*/);
}

/*******************************************************************************
* cpssDxChPtpDomainV1IdSet
*
* DESCRIPTION:
*       Set PTP V1 domain Id.
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum         - device number
*       domainNum      - domain number.
*                        (APPLICABLE RANGES: 0..3.)
*       domainIdArr    - domain Id
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong parameter
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       PTP V1 Header contains 128-bit domain Id.
*       4 domain Id values mapped to domainNum 0-3
*       all other domain Id values mapped to default domain
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChPtpDomainV1IdTableSet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS    rc;
    GT_U8        devNum;
    GT_U32       domainNum;
    GT_U32       domainIdArr[4];

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum          = (GT_U8)inArgs[0];
    domainNum       = (GT_U32)inFields[0];
    domainIdArr[0]  = (GT_U32)inFields[1];
    domainIdArr[1]  = (GT_U32)inFields[2];
    domainIdArr[2]  = (GT_U32)inFields[3];
    domainIdArr[3]  = (GT_U32)inFields[4];

    /* call cpss api function */
    rc = cpssDxChPtpDomainV1IdSet(
        devNum, domainNum, domainIdArr);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, rc, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChPtpDomainV1IdGet
*
* DESCRIPTION:
*       Get PTP V1 domain Id.
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum          - device number
*       domainNum       - domain number.
*                         (APPLICABLE RANGES: 0..3.)
*
* OUTPUTS:
*       domainIdArr     - domain Id
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong parameter
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       PTP V1 Header contains 128-bit domain Id.
*       4 domain Id values mapped to domainNum 0-3
*       all other domain Id values mapped to default domain
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChPtpDomainV1IdTableGet_util
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32      numFields,
    OUT GT_8       outArgs[CMD_MAX_BUFFER],
    IN  GT_BOOL    isGetFirst
)
{
    GT_STATUS      rc;
    GT_U8          devNum;
    /* variables initialised only to prevent compiler warnings */
    static GT_U32  domainNum = 0;
    GT_U32         domainIdArr[4];

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum                            = (GT_U8)inArgs[0];

    if (isGetFirst == GT_TRUE)
    {
        domainNum = 0;
    }
    else
    {
        domainNum ++;
    }

    if (domainNum > 3)
    {
        /* no entries more */
        galtisOutput(outArgs, GT_OK, "%d", -1);
        return CMD_OK;
    }

    rc = cpssDxChPtpDomainV1IdGet(
        devNum, domainNum, domainIdArr);
    if (rc != GT_OK)
    {
        galtisOutput(outArgs, rc, "cpssDxChPtpDomainV1IdGet");
        return CMD_OK;
    }

    /* pack and output table fields */
    fieldOutput(
        "%d%d%d%d%d", domainNum,
        domainIdArr[0], domainIdArr[1], domainIdArr[2], domainIdArr[3]);
    galtisOutput(outArgs, rc, "%f");

    return CMD_OK;
}

static CMD_STATUS wrCpssDxChPtpDomainV1IdTableGetFirst
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    return wrCpssDxChPtpDomainV1IdTableGet_util(
        inArgs, inFields, numFields, outArgs, GT_TRUE/*isGetFirst*/);
}

static CMD_STATUS wrCpssDxChPtpDomainV1IdTableGetNext
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    return wrCpssDxChPtpDomainV1IdTableGet_util(
        inArgs, inFields, numFields, outArgs, GT_FALSE/*isGetFirst*/);
}

/*******************************************************************************
* cpssDxChPtpDomainV2IdSet
*
* DESCRIPTION:
*       Set PTP V2 domain Id.
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum         - device number
*       domainNum      - domain number.
*                        (APPLICABLE RANGES: 0..3.)
*       domainId       - domain Id
*                        (APPLICABLE RANGES: 0..255.)
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_OUT_OF_RANGE          - on out-of-range parameter
*       GT_BAD_PARAM             - on wrong parameter
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       PTP V2 Header contains 8-bit domain Id.
*       4 domain Id values mapped to domainNum 0-3
*       all other domain Id values mapped to default domain
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChPtpDomainV2IdTableSet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS    rc;
    GT_U8        devNum;
    GT_U32       domainNum;
    GT_U32       domainId;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum          = (GT_U8)inArgs[0];
    domainNum       = (GT_U32)inFields[0];
    domainId        = (GT_U32)inFields[1];

    /* call cpss api function */
    rc = cpssDxChPtpDomainV2IdSet(
        devNum, domainNum, domainId);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, rc, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChPtpDomainV2IdGet
*
* DESCRIPTION:
*       Get PTP V2 domain Id.
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum          - device number
*       domainNum       - domain number.
*                         (APPLICABLE RANGES: 0..3.)
*
* OUTPUTS:
*       domainIdPtr     - (pointer to) domain Id
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong parameter
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       PTP V2 Header contains 8-bit domain Id.
*       4 domain Id values mapped to domainNum 0-3
*       all other domain Id values mapped to default domain
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChPtpDomainV2IdTableGet_util
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32      numFields,
    OUT GT_8       outArgs[CMD_MAX_BUFFER],
    IN  GT_BOOL    isGetFirst
)
{
    GT_STATUS      rc;
    GT_U8          devNum;
    /* variables initialised only to prevent compiler warnings */
    static GT_U32  domainNum = 0;
    GT_U32         domainId;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum                            = (GT_U8)inArgs[0];

    if (isGetFirst == GT_TRUE)
    {
        domainNum = 0;
    }
    else
    {
        domainNum ++;
    }

    if (domainNum > 3)
    {
        /* no entries more */
        galtisOutput(outArgs, GT_OK, "%d", -1);
        return CMD_OK;
    }

    rc = cpssDxChPtpDomainV2IdGet(
        devNum, domainNum, &domainId);
    if (rc != GT_OK)
    {
        galtisOutput(outArgs, rc, "cpssDxChPtpDomainV2IdGet");
        return CMD_OK;
    }

    /* pack and output table fields */
    fieldOutput(
        "%d%d", domainNum, domainId);
    galtisOutput(outArgs, rc, "%f");

    return CMD_OK;
}

static CMD_STATUS wrCpssDxChPtpDomainV2IdTableGetFirst
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    return wrCpssDxChPtpDomainV2IdTableGet_util(
        inArgs, inFields, numFields, outArgs, GT_TRUE/*isGetFirst*/);
}

static CMD_STATUS wrCpssDxChPtpDomainV2IdTableGetNext
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    return wrCpssDxChPtpDomainV2IdTableGet_util(
        inArgs, inFields, numFields, outArgs, GT_FALSE/*isGetFirst*/);
}

/*******************************************************************************
* cpssDxChPtpEgressDomainTableSet
*
* DESCRIPTION:
*       Set Egress Domain Table Entry.
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum              - device number
*       portNum             - port number.
*       domainNum           - domain number.
*                             (APPLICABLE RANGES: 0..4.)
*       entryPtr            - (pointer to) Domain Table Entry.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_BAD_PARAM             - on wrong parameter
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChPtpEgressDomainTableSet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                              rc;
    GT_U8                                  devNum;
    GT_PHYSICAL_PORT_NUM                   portNum;
    GT_U32                                 domainNum;
    CPSS_DXCH_PTP_EGRESS_DOMAIN_ENTRY_STC  entry;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum                               = (GT_U8)inArgs[0];
    portNum                              = (GT_PHYSICAL_PORT_NUM)inArgs[1];
    domainNum                            = (GT_U32)inFields[0];
    entry.ptpOverEhernetTsEnable         = (GT_BOOL)inFields[1];
    entry.ptpOverUdpIpv4TsEnable         = (GT_BOOL)inFields[2];
    entry.ptpOverUdpIpv6TsEnable         = (GT_BOOL)inFields[3];
    entry.messageTypeTsEnableBmp         = (GT_U32)inFields[4];
    entry.transportSpecificTsEnableBmp   = (GT_U32)inFields[5];

    /* Override Device and Port */
    CONVERT_DEV_PHYSICAL_PORT_MAC(devNum, portNum);

    /* call cpss api function */
    rc = cpssDxChPtpEgressDomainTableSet(
        devNum, portNum, domainNum, &entry);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, rc, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChPtpEgressDomainTableGet
*
* DESCRIPTION:
*       Get Egress Domain Table Entry.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum              - device number
*       portNum             - port number.
*       domainNum           - domain number.
*                             (APPLICABLE RANGES: 0..4.)
*
* OUTPUTS:
*       entryPtr            - (pointer to) Egress Domain Table Entry.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_BAD_PARAM             - on wrong parameter
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChPtpEgressDomainTableGet_util
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32      numFields,
    OUT GT_8       outArgs[CMD_MAX_BUFFER],
    IN  GT_BOOL    isGetFirst
)
{
    GT_STATUS                              rc;
    GT_U8                                  devNum;
    GT_PHYSICAL_PORT_NUM                   portNum;
    /* variables initialised only to prevent compiler warnings */
    static GT_U32                          domainNum = 0;
    CPSS_DXCH_PTP_EGRESS_DOMAIN_ENTRY_STC  entry;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum      = (GT_U8)inArgs[0];
    portNum     = (GT_PHYSICAL_PORT_NUM)inArgs[1];

    if (isGetFirst == GT_TRUE)
    {
        domainNum = 0;
    }
    else
    {
        domainNum ++;
    }

    if (domainNum > 4)
    {
        /* no entries more */
        galtisOutput(outArgs, GT_OK, "%d", -1);
        return CMD_OK;
    }

    rc = cpssDxChPtpEgressDomainTableGet(
        devNum, portNum, domainNum, &entry);
    if (rc != GT_OK)
    {
        galtisOutput(outArgs, rc, "cpssDxChPtpEgressDomainTableGet");
        return CMD_OK;
    }

    /* pack and output table fields */
    fieldOutput(
        "%d%d%d%d%d%d", domainNum,
        entry.ptpOverEhernetTsEnable,
        entry.ptpOverUdpIpv4TsEnable,
        entry.ptpOverUdpIpv6TsEnable,
        entry.messageTypeTsEnableBmp,
        entry.transportSpecificTsEnableBmp);
    galtisOutput(outArgs, rc, "%f");

    return CMD_OK;
}

static CMD_STATUS wrCpssDxChPtpEgressDomainTableGetFirst
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    return wrCpssDxChPtpEgressDomainTableGet_util(
        inArgs, inFields, numFields, outArgs, GT_TRUE/*isGetFirst*/);
}

static CMD_STATUS wrCpssDxChPtpEgressDomainTableGetNext
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    return wrCpssDxChPtpEgressDomainTableGet_util(
        inArgs, inFields, numFields, outArgs, GT_FALSE/*isGetFirst*/);
}

/*******************************************************************************
* cpssDxChPtpIngressPacketCheckingModeSet
*
* DESCRIPTION:
*       Set PTP packet Ingress Checking Mode.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum          - device number
*       messageType     - message type
*                         (APPLICABLE RANGES: 0..15.)
*       domainNum       - domainNum
*                         (APPLICABLE RANGES: 0..4.)
*       checkingMode    - PTP packet ingress checking mode.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong devNum, messageType or command
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChPtpIngressPacketCheckingModeTableSet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                                rc;
    GT_U8                                    devNum;
    GT_U32                                   domainNum;
    GT_U32                                   messageType;
    CPSS_DXCH_PTP_INGRESS_CHECKING_MODE_ENT  checkingMode;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum        = (GT_U8)inArgs[0];
    domainNum     = (GT_U32)inArgs[1];
    messageType   = (GT_U32)inFields[0];
    checkingMode  = (CPSS_DXCH_PTP_INGRESS_CHECKING_MODE_ENT)inFields[1];

    /* call cpss api function */
    rc = cpssDxChPtpIngressPacketCheckingModeSet(
        devNum, domainNum, messageType, checkingMode);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, rc, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChPtpIngressPacketCheckingModeGet
*
* DESCRIPTION:
*       Get PTP packet Ingress Checking Mode.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum          - device number
*       domainNum       - domainNum
*                         (APPLICABLE RANGES: 0..4.)
*       messageType     - message type
*                         (APPLICABLE RANGES: 0..15.)
*
* OUTPUTS:
*       checkingModePtr - (pointer to)PTP packet ingress checking mode.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong devNum or messageType
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_BAD_STATE             - on bad value found in HW
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChPtpIngressPacketCheckingModeTableGet_util
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32      numFields,
    OUT GT_8       outArgs[CMD_MAX_BUFFER],
    IN  GT_BOOL    isGetFirst
)
{
    GT_STATUS                                rc;
    GT_U8                                    devNum;
    GT_U32                                   domainNum;
    /* variables initialised only to prevent compiler warnings */
    static GT_U32                            messageType = 0;
    CPSS_DXCH_PTP_INGRESS_CHECKING_MODE_ENT  checkingMode;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum      = (GT_U8)inArgs[0];
    domainNum   = (GT_U32)inArgs[1];

    if (isGetFirst == GT_TRUE)
    {
        messageType = 0;
    }
    else
    {
        messageType ++;
    }

    if (messageType > 15)
    {
        /* no entries more */
        galtisOutput(outArgs, GT_OK, "%d", -1);
        return CMD_OK;
    }

    rc = cpssDxChPtpIngressPacketCheckingModeGet(
        devNum, domainNum, messageType, &checkingMode);
    if (rc != GT_OK)
    {
        galtisOutput(outArgs, rc, "cpssDxChPtpIngressPacketCheckingModeGet");
        return CMD_OK;
    }

    /* pack and output table fields */
    fieldOutput(
        "%d%d", messageType, checkingMode);
    galtisOutput(outArgs, rc, "%f");

    return CMD_OK;
}

static CMD_STATUS wrCpssDxChPtpIngressPacketCheckingModeTableGetFirst
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    return wrCpssDxChPtpIngressPacketCheckingModeTableGet_util(
        inArgs, inFields, numFields, outArgs, GT_TRUE/*isGetFirst*/);
}

static CMD_STATUS wrCpssDxChPtpIngressPacketCheckingModeTableGetNext
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    return wrCpssDxChPtpIngressPacketCheckingModeTableGet_util(
        inArgs, inFields, numFields, outArgs, GT_FALSE/*isGetFirst*/);
}

/*******************************************************************************
* cpssDxChPtpTsIngressTimestampQueueEntryRead
*
* DESCRIPTION:
*       Read Ingress Timestamp Queue Entry.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum               - device number
*       queueNum             - Queue Number
*                              (APPLICABLE RANGES: 0..1.)
*
* OUTPUTS:
*       tsQueueEntryPtr      - (pointer to) Ingress Timestamp Queue Entry.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_BAD_PARAM             - on wrong parameter
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       The Queue read causes deleting the entry from the queue.
*       The next read will retrieve the next entry.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChPtpTsIngressTimestampQueueTableGet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32      numFields,
    OUT GT_8       outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                                            rc;
    GT_U8                                                devNum;
    GT_U32                                               queueNum;
    CPSS_DXCH_PTP_TS_INGRESS_TIMESTAMP_QUEUE_ENTRY_STC   tsQueueEntry;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum     = (GT_U8)inArgs[0];
    queueNum   = (GT_U32)inArgs[1];

    rc = cpssDxChPtpTsIngressTimestampQueueEntryRead(
        devNum, queueNum, &tsQueueEntry);
    if (rc != GT_OK)
    {
        galtisOutput(outArgs, rc, "cpssDxChPtpTsIngressTimestampQueueEntryRead");
        return CMD_OK;
    }

    if (tsQueueEntry.entryValid == GT_FALSE)
    {
        /* no entries more */
        galtisOutput(outArgs, GT_OK, "%d", -1);
        return CMD_OK;
    }

    /* pack and output table fields */
    fieldOutput(
        "%d%d%d%d%d%d%d%d%d",
        tsQueueEntry.isPtpExeption,
        tsQueueEntry.packetFormat,
        tsQueueEntry.taiSelect,
        tsQueueEntry.todUpdateFlag,
        tsQueueEntry.messageType,
        tsQueueEntry.domainNum,
        tsQueueEntry.sequenceId,
        tsQueueEntry.timestamp,
        tsQueueEntry.portNum);
    galtisOutput(outArgs, rc, "%f");

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChPtpTsEgressTimestampQueueEntryRead
*
* DESCRIPTION:
*       Read Egress Timestamp Queue Entry.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum               - device number
*       queueNum             - Queue Number
*                              (APPLICABLE RANGES: 0..1.)
*
* OUTPUTS:
*       tsQueueEntryPtr      - (pointer to) Egress Timestamp Queue Entry.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_BAD_PARAM             - on wrong parameter
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       The Queue read causes deleting the entry from the queue.
*       The next read will retrieve the next entry.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChPtpTsEgressTimestampQueueTableGet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32      numFields,
    OUT GT_8       outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                                            rc;
    GT_U8                                                devNum;
    GT_U32                                               queueNum;
    CPSS_DXCH_PTP_TS_EGRESS_TIMESTAMP_QUEUE_ENTRY_STC    tsQueueEntry;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum     = (GT_U8)inArgs[0];
    queueNum   = (GT_U32)inArgs[1];

    rc = cpssDxChPtpTsEgressTimestampQueueEntryRead(
        devNum, queueNum, &tsQueueEntry);
    if (rc != GT_OK)
    {
        galtisOutput(outArgs, rc, "cpssDxChPtpTsEgressTimestampQueueEntryRead");
        return CMD_OK;
    }

    if (tsQueueEntry.entryValid == GT_FALSE)
    {
        /* no entries more */
        galtisOutput(outArgs, GT_OK, "%d", -1);
        return CMD_OK;
    }

    /* pack and output table fields */
    fieldOutput(
        "%d%d%d%d%d%d%d",
        tsQueueEntry.isPtpExeption,
        tsQueueEntry.packetFormat,
        tsQueueEntry.messageType,
        tsQueueEntry.domainNum,
        tsQueueEntry.sequenceId,
        tsQueueEntry.queueEntryId,
        tsQueueEntry.portNum);
    galtisOutput(outArgs, rc, "%f");

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChPtpTaiPtpPulseIterfaceSet
*
* DESCRIPTION:
*       Set TAI PTP pulse interface.
*
* APPLICABLE DEVICES:
*       Bobcat2.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Lion3.
*
* INPUTS:
*       devNum                  - device number.
*       pulseInterfaceDirection - PTP pulse interface direction.
*       taiNumber               - TAI number selection.
*                                (relevant for PTP output pulse)
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong parameter
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       PTP pulse interface used for generating/receiving discrete pulses
*       that trigger a time-related operation such as Update or Capture.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChPtpTaiPtpPulseIterfaceSet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                           result;
    GT_U8                               devNum;
    CPSS_DIRECTION_ENT                  pulseInterfaceDirection;
    CPSS_DXCH_PTP_TAI_NUMBER_ENT        taiNumber;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    pulseInterfaceDirection = (CPSS_DIRECTION_ENT)inArgs[1];
    taiNumber = (CPSS_DXCH_PTP_TAI_NUMBER_ENT)inArgs[2];

    /* call cpss api function */
    result = cpssDxChPtpTaiPtpPulseIterfaceSet(devNum,
                                               pulseInterfaceDirection,
                                               taiNumber);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChPtpTaiPtpPulseIterfaceGet
*
* DESCRIPTION:
*       Get TAI PTP pulse interface.
*
* APPLICABLE DEVICES:
*       Bobcat2.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Lion3.
*
* INPUTS:
*       devNum                      - device number
*
* OUTPUTS:
*       pulseInterfaceDirectionPtr  - (pointer to) PTP pulse interface direction.
*       taiNumberPtr                - (pointer to) TAI number selection.
*                                     (relevant for PTP output pulse)
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_BAD_PARAM             - on wrong parameter
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       PTP pulse interface used for generating/receiving discrete pulses
*       that trigger a time-related operation such as Update or Capture.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChPtpTaiPtpPulseIterfaceGet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                           result;
    GT_U8                               devNum;
    CPSS_DIRECTION_ENT                  pulseInterfaceDirection;
    CPSS_DXCH_PTP_TAI_NUMBER_ENT        taiNumber;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];

    /* call cpss api function */
    result = cpssDxChPtpTaiPtpPulseIterfaceGet(devNum,
                                               &pulseInterfaceDirection,
                                               &taiNumber);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d%d", pulseInterfaceDirection, taiNumber);

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChPtpTaiPtpClockInterfaceSet
*
* DESCRIPTION:
*       Set TAI PTP clock interface.
*
* APPLICABLE DEVICES:
*       Bobcat2.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Lion3.
*
* INPUTS:
*       devNum                  - device number
*       taiNumber               - TAI Number.
*       clockInterfaceDirection - PTP clock interface direction.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong parameter
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       PTP clock interface used for generating/receiving a periodic pulse
*       such as 1 PPS signal.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChPtpTaiPtpClockInterfaceSet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                           result;
    GT_U8                               devNum;
    CPSS_DXCH_PTP_TAI_NUMBER_ENT        taiNumber;
    CPSS_DIRECTION_ENT                  clockInterfaceDirection;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    taiNumber = (CPSS_DXCH_PTP_TAI_NUMBER_ENT)inArgs[1];
    clockInterfaceDirection = (CPSS_DIRECTION_ENT)inArgs[2];

    /* call cpss api function */
    result = cpssDxChPtpTaiPtpClockInterfaceSet(devNum,
                                                taiNumber,
                                                clockInterfaceDirection);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChPtpTaiPtpClockInterfaceGet
*
* DESCRIPTION:
*       Get TAI PTP clock interface.
*
* APPLICABLE DEVICES:
*       Bobcat2.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Lion3.
*
* INPUTS:
*       devNum                  - device number
*       taiNumber               - TAI number.
*
* OUTPUTS:
*       clockInterfaceDirectionPtr   - (pointer to) PTP clock interface direction.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_BAD_PARAM             - on wrong parameter
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       PTP clock interface used for generating/receiving a periodic pulse
*       such as 1 PPS signal.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChPtpTaiPtpClockInterfaceGet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                           result;
    GT_U8                               devNum;
    CPSS_DXCH_PTP_TAI_NUMBER_ENT        taiNumber;
    CPSS_DIRECTION_ENT                  clockInterfaceDirection;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    taiNumber = (CPSS_DXCH_PTP_TAI_NUMBER_ENT)inArgs[1];

    /* call cpss api function */
    result = cpssDxChPtpTaiPtpClockInterfaceGet(devNum,
                                                taiNumber,
                                                &clockInterfaceDirection);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", clockInterfaceDirection);

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChPtpTaiPClkOutputInterfaceSet
*
* DESCRIPTION:
*       Set TAI PTP PClk or Sync-E recovered clock output interface.
*
* APPLICABLE DEVICES:
*       Bobcat2.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Lion3.
*
* INPUTS:
*       devNum              - device number
*       pclkRcvrClkMode     - clock output interface mode.
*       taiNumber           - TAI number - used as clock’s source for PClk output mode.
*                             (relevant for "PTP" mode)
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong parameter
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChPtpTaiPClkOutputInterfaceSet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                                   result;
    GT_U8                                       devNum;
    CPSS_DXCH_PTP_TAI_PCLK_RCVR_CLK_MODE_ENT    pclkRcvrClkMode;
    CPSS_DXCH_PTP_TAI_NUMBER_ENT                taiNumber;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    pclkRcvrClkMode = (CPSS_DXCH_PTP_TAI_PCLK_RCVR_CLK_MODE_ENT)inArgs[1];
    taiNumber = (CPSS_DXCH_PTP_TAI_NUMBER_ENT)inArgs[2];

    /* call cpss api function */
    result = cpssDxChPtpTaiPClkOutputInterfaceSet(devNum, pclkRcvrClkMode, taiNumber);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChPtpTaiPClkOutputInterfaceGet
*
* DESCRIPTION:
*       Get TAI PTP Pclock or Sync-E recovered clock output interface.
*
* APPLICABLE DEVICES:
*       Bobcat2.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Lion3.
*
* INPUTS:
*       devNum              - device number
*
* OUTPUTS:
*       pclkRcvrClkModePtr  - (pointer to) clock output interface mode.
*       taiNumberPtr        - (pointer to) TAI number - used as clock’s source for PClk output mode.
*                             (relevant for "PTP" mode)
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_BAD_PARAM             - on wrong parameter
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChPtpTaiPClkOutputInterfaceGet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                                   result;
    GT_U8                                       devNum;
    CPSS_DXCH_PTP_TAI_PCLK_RCVR_CLK_MODE_ENT    pclkRcvrClkMode;
    CPSS_DXCH_PTP_TAI_NUMBER_ENT                taiNumber;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];

    /* call cpss api function */
    result = cpssDxChPtpTaiPClkOutputInterfaceGet(devNum,
                                                  &pclkRcvrClkMode,
                                                  &taiNumber);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d%d", pclkRcvrClkMode, taiNumber);

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChPtpTaiInputClockSelectSet
*
* DESCRIPTION:
*       Configures input TAI clock’s selection.
*
* APPLICABLE DEVICES:
*       Bobcat2.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Lion3.
*
* INPUTS:
*       devNum              - device number
*       taiNumber           - TAI number.
*       clockSelect         - input clock selection mode.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong parameter
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChPtpTaiInputClockSelectSet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                                   result;
    GT_U8                                       devNum;
    CPSS_DXCH_PTP_TAI_NUMBER_ENT                taiNumber;
    CPSS_DXCH_PTP_TAI_CLOCK_SELECT_ENT          clockSelect;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    taiNumber = (CPSS_DXCH_PTP_TAI_NUMBER_ENT)inArgs[1];
    clockSelect = (CPSS_DXCH_PTP_TAI_CLOCK_SELECT_ENT)inArgs[2];

    /* call cpss api function */
    result = cpssDxChPtpTaiInputClockSelectSet(devNum, taiNumber, clockSelect);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChPtpTaiInputClockSelectGet
*
* DESCRIPTION:
*       Get input TAI clock’s selection.
*
* APPLICABLE DEVICES:
*       Bobcat2.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Lion3.
*
* INPUTS:
*       devNum                  - device number
*       taiNumber               - TAI number.
*
* OUTPUTS:
*       clockSelectPtr          - (pointer to) input clock selection mode.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_BAD_PARAM             - on wrong parameter
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChPtpTaiInputClockSelectGet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                                   result;
    GT_U8                                       devNum;
    CPSS_DXCH_PTP_TAI_NUMBER_ENT                taiNumber;
    CPSS_DXCH_PTP_TAI_CLOCK_SELECT_ENT          clockSelect;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    taiNumber = (CPSS_DXCH_PTP_TAI_NUMBER_ENT)inArgs[1];

    /* call cpss api function */
    result = cpssDxChPtpTaiInputClockSelectGet(devNum, taiNumber, &clockSelect);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", clockSelect);

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChPtpTaiTodCounterFunctionAllTriggerSet
*
* DESCRIPTION:
*       Triggeres TOD update operation for all TAI units.
*
* APPLICABLE DEVICES:
*       Bobcat2.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Lion3.
*
* INPUTS:
*       devNum                   - device number
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong devNum
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChPtpTaiTodCounterFunctionAllTriggerSet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                                   result;
    GT_U8                                       devNum;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];

    /* call cpss api function */
    result = cpssDxChPtpTaiTodCounterFunctionAllTriggerSet(devNum);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChPtpPortUnitResetSet
*
* DESCRIPTION:
*       Reset/unreset Port PTP unit.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Lion3.
*
* INPUTS:
*       devNum      - physical device number
*       portNum     - physical port number
*       resetTxUnit - GT_TRUE - trasmit unit,
*                     GT_FALSE - receive unit
*                     For Bobcat2 related to both transmit and receive units
*       resetRxUnit   - GT_TRUE - reset PTP unit
*                     GT_FALSE - unreset PTP unit
*                     (APPLICABLE DEVICES: Caelum; Bobcat3)
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK             - on success
*       GT_BAD_PARAM      - on wrong port number or device
*       GT_HW_ERROR       - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChPtpPortUnitResetSet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                       result;
    GT_U8                           devNum;
    GT_PHYSICAL_PORT_NUM            portNum;
    GT_BOOL                         resetTxUnit;
    GT_BOOL                         resetRxUnit;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum       = (GT_U8)inArgs[0];
    portNum      = (GT_PHYSICAL_PORT_NUM)inArgs[1];
    resetTxUnit  = (GT_BOOL)inArgs[2];
    resetRxUnit  = (GT_BOOL)inArgs[3];

    /* Override Device and Port */
    CONVERT_DEV_PHYSICAL_PORT_MAC(devNum, portNum);

    /* call cpss api function */
    result = cpssDxChPtpPortUnitResetSet(
        devNum, portNum, resetTxUnit, resetRxUnit);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChPtpPortUnitResetGet
*
* DESCRIPTION:
*       Get Reset/unreset Port PTP unit state.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Lion3.
*
* INPUTS:
*       devNum         - physical device number
*       portNum        - physical port number
*
* OUTPUTS:
*       resetTxUnitPtr - (pointer to)
*                        GT_TRUE - trasmit unit,
*                        GT_FALSE - receive unit
*                        For Bobcat2 related to both transmit and receive units
*       resetRxUnitPtr - (pointer to)
*                        GT_TRUE - reset PTP unit
*                        GT_FALSE - unreset PTP unit
*                        (APPLICABLE DEVICES: Caelum; Bobcat3)
*
* RETURNS:
*       GT_OK             - on success
*       GT_BAD_PARAM      - on wrong port number or device
*       GT_HW_ERROR       - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChPtpPortUnitResetGet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                       result;
    GT_U8                           devNum;
    GT_PHYSICAL_PORT_NUM            portNum;
    GT_BOOL                         resetTxUnit;
    GT_BOOL                         resetRxUnit;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum       = (GT_U8)inArgs[0];
    portNum      = (GT_PHYSICAL_PORT_NUM)inArgs[1];

    /* Override Device and Port */
    CONVERT_DEV_PHYSICAL_PORT_MAC(devNum, portNum);

    /* call cpss api function */
    result = cpssDxChPtpPortUnitResetGet(
        devNum, portNum, &resetTxUnit, &resetRxUnit);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d%d", resetTxUnit, resetRxUnit);

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChPtpPortTxPipeStatusDelaySet
*
* DESCRIPTION:
*       Set PTP Tx Pipe Status Delay.
*
* APPLICABLE DEVICES:
*        Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Lion3; Bobcat2.
*
* INPUTS:
*       devNum      - physical device number
*       portNum     - physical port number
*       delay       - delay of empty egress pipe (in core clock cycles).
*                     (APPLICABLE RANGE: 0..0xFFFF)
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK             - on success
*       GT_BAD_PARAM      - on wrong port number or device
*       GT_OUT_OF_RANGE   - on out of range parameter value
*       GT_HW_ERROR       - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChPtpPortTxPipeStatusDelaySet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                       result;
    GT_U8                           devNum;
    GT_PHYSICAL_PORT_NUM            portNum;
    GT_U32                          delay;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum       = (GT_U8)inArgs[0];
    portNum      = (GT_PHYSICAL_PORT_NUM)inArgs[1];
    delay        = (GT_U32)inArgs[2];

    /* Override Device and Port */
    CONVERT_DEV_PHYSICAL_PORT_MAC(devNum, portNum);

    /* call cpss api function */
    result = cpssDxChPtpPortTxPipeStatusDelaySet(
        devNum, portNum, delay);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChPtpPortTxPipeStatusDelayGet
*
* DESCRIPTION:
*       Get PTP Tx Pipe Status Delay.
*
* APPLICABLE DEVICES:
*        Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Lion3; Bobcat2.
*
* INPUTS:
*       devNum      - physical device number
*       portNum     - physical port number
*
* OUTPUTS:
*       delayPtr    - (pointer to)delay of empty egress pipe (in core clock cycles).
*
* RETURNS:
*       GT_OK             - on success
*       GT_BAD_PARAM      - on wrong port number or device
*       GT_BAD_PTR        - one of the parameters is NULL pointer
*       GT_HW_ERROR       - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChPtpPortTxPipeStatusDelayGet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                       result;
    GT_U8                           devNum;
    GT_PHYSICAL_PORT_NUM            portNum;
    GT_U32                          delay;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum  = (GT_U8)inArgs[0];
    portNum = (GT_PHYSICAL_PORT_NUM)inArgs[1];

    /* Override Device and Port */
    CONVERT_DEV_PHYSICAL_PORT_MAC(devNum, portNum);

    /* call cpss api function */
    result = cpssDxChPtpPortTxPipeStatusDelayGet(
        devNum, portNum, &delay);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", delay);

    return CMD_OK;
}

/**** database initialization **************************************/

static CMD_COMMAND dbCommands[] =
{
    {"cpssDxChPtpEtherTypeSet",
        &wrCpssDxChPtpEtherTypeSet,
        3, 0},

    {"cpssDxChPtpEtherTypeGet",
        &wrCpssDxChPtpEtherTypeGet,
        2, 0},

    {"cpssDxChPtpUdpDestPortsSet",
        &wrCpssDxChPtpUdpDestPortsSet,
        3, 0},

    {"cpssDxChPtpUdpDestPortsGet",
        &wrCpssDxChPtpUdpDestPortsGet,
        2, 0},

    {"cpssDxChPtpMessageTypeCmdSet",
        &wrCpssDxChPtpMessageTypeCmdSet,
        5, 0},

    {"cpssDxChPtpMessageTypeCmdGet",
        &wrCpssDxChPtpMessageTypeCmdGet,
        4, 0},

    {"cpssDxChPtpCpuCodeBaseSet",
        &wrCpssDxChPtpCpuCodeBaseSet,
        2, 0},

    {"cpssDxChPtpCpuCodeBaseGet",
        &wrCpssDxChPtpCpuCodeBaseGet,
        1, 0},

    {"cpssDxChPtpTodCounterFunctionSet",
        &wrCpssDxChPtpTodCounterFunctionSet,
        3, 0},

    {"cpssDxChPtpTodCounterFunctionGet",
        &wrCpssDxChPtpTodCounterFunctionGet,
        2, 0},

    {"cpssDxChPtpTodCounterFunctionTriggerSet",
        &wrCpssDxChPtpTodCounterFunctionTriggerSet,
        2, 0},

    {"cpssDxChPtpTodCounterFunctionTriggerGet",
        &wrCpssDxChPtpTodCounterFunctionTriggerGet,
        2, 0},

    {"cpssDxChPtpTodCounterShadowSet",
        &wrCpssDxChPtpTodCounterShadowSet,
        5, 0},

    {"cpssDxChPtpTodCounterShadowGet",
        &wrCpssDxChPtpTodCounterShadowGet,
        2, 0},

    {"cpssDxChPtpTodCounterGet",
        &wrCpssDxChPtpTodCounterGet,
        2, 0},

    {"cpssDxChPtpPortTimeStampEnableSet",
        &wrCpssDxChPtpPortTimeStampEnableSet,
        4, 0},

    {"cpssDxChPtpPortTimeStampEnableGet",
        &wrCpssDxChPtpPortTimeStampEnableGet,
        3, 0},

    {"cpssDxChPtpEthernetTimeStampEnableSet",
        &wrCpssDxChPtpEthernetTimeStampEnableSet,
        3, 0},

    {"cpssDxChPtpEthernetTimeStampEnableGet",
        &wrCpssDxChPtpEthernetTimeStampEnableGet,
        2, 0},

    {"cpssDxChPtpUdpTimeStampEnableSet",
        &wrCpssDxChPtpUdpTimeStampEnableSet,
        4, 0},

    {"cpssDxChPtpUdpTimeStampEnableGet",
        &wrCpssDxChPtpUdpTimeStampEnableGet,
        3, 0},

    {"cpssDxChPtpMessageTypeTimeStampEnableSet",
        &wrCpssDxChPtpMessageTypeTimeStampEnableSet,
        4, 0},

    {"cpssDxChPtpMessageTypeTimeStampEnableGet",
        &wrCpssDxChPtpMessageTypeTimeStampEnableGet,
        3, 0},

    {"cpssDxChPtpTransportSpecificCheckEnableSet",
        &wrCpssDxChPtpTransportSpecificCheckEnableSet,
        3, 0},

    {"cpssDxChPtpTransportSpecificCheckEnableGet",
        &wrCpssDxChPtpTransportSpecificCheckEnableGet,
        2, 0},

    {"cpssDxChPtpTransportSpecificTimeStampEnableSet",
        &wrCpssDxChPtpTransportSpecificTimeStampEnableSet,
        4, 0},

    {"cpssDxChPtpTransportSpecificTimeStampEnableGet",
        &wrCpssDxChPtpTransportSpecificTimeStampEnableGet,
        3, 0},

    {"cpssDxChPtpTimestampEntryGet",
        &wrCpssDxChPtpTimestampEntryGet,
        2, 0},

    /* new BobCat2 and Lion3 commands */

    {"cpssDxChPtpTodCounterFunctionSet_Tai",
        &wrCpssDxChPtpTodCounterFunctionSet_Tai,
        6, 0},

    {"cpssDxChPtpTodCounterFunctionGet_Tai",
        &wrCpssDxChPtpTodCounterFunctionGet_Tai,
        5, 0},

    {"cpssDxChPtpTodCounterFunctionTriggerSet_Tai",
        &wrCpssDxChPtpTodCounterFunctionTriggerSet_Tai,
        5, 0},

    {"cpssDxChPtpTodCounterFunctionTriggerGet_Tai",
        &wrCpssDxChPtpTodCounterFunctionTriggerGet_Tai,
        5, 0},

    {"cpssDxChPtpTaiClockModeSet",
        &wrCpssDxChPtpTaiClockModeSet,
        5, 0},

    {"cpssDxChPtpTaiClockModeGet",
        &wrCpssDxChPtpTaiClockModeGet,
        4, 0},

    {"cpssDxChPtpTaiInternalClockGenerateEnableSet",
        &wrCpssDxChPtpTaiInternalClockGenerateEnableSet,
        5, 0},

    {"cpssDxChPtpTaiInternalClockGenerateEnableGet",
        &wrCpssDxChPtpTaiInternalClockGenerateEnableGet,
        4, 0},

    {"cpssDxChPtpTaiPtpPClockDriftAdjustEnableSet",
        &wrCpssDxChPtpTaiPtpPClockDriftAdjustEnableSet,
        5, 0},

    {"cpssDxChPtpTaiPtpPClockDriftAdjustEnableGet",
        &wrCpssDxChPtpTaiPtpPClockDriftAdjustEnableGet,
        4, 0},

    {"cpssDxChPtpTaiCaptureOverrideEnableSet",
        &wrCpssDxChPtpTaiCaptureOverrideEnableSet,
        5, 0},

    {"cpssDxChPtpTaiCaptureOverrideEnableGet",
        &wrCpssDxChPtpTaiCaptureOverrideEnableGet,
        4, 0},

    {"cpssDxChPtpTaiInputTriggersCountEnableSet",
        &wrCpssDxChPtpTaiInputTriggersCountEnableSet,
        5, 0},

    {"cpssDxChPtpTaiInputTriggersCountEnableGet",
        &wrCpssDxChPtpTaiInputTriggersCountEnableGet,
        4, 0},

    {"cpssDxChPtpTaiExternalPulseWidthSet",
        &wrCpssDxChPtpTaiExternalPulseWidthSet,
        5, 0},

    {"cpssDxChPtpTaiExternalPulseWidthGet",
        &wrCpssDxChPtpTaiExternalPulseWidthGet,
        4, 0},

    {"cpssDxChPtpTaiTodSet",
        &wrCpssDxChPtpTaiTodSet,
        9, 0},

    {"cpssDxChPtpTaiTodGet",
        &wrCpssDxChPtpTaiTodGet,
        5, 0},

    {"cpssDxChPtpTaiOutputTriggerEnableSet",
        &wrCpssDxChPtpTaiOutputTriggerEnableSet,
        5, 0},

    {"cpssDxChPtpTaiOutputTriggerEnableGet",
        &wrCpssDxChPtpTaiOutputTriggerEnableGet,
        4, 0},

    {"cpssDxChPtpTaiTodStepSet",
        &wrCpssDxChPtpTaiTodStepSet,
        6, 0},

    {"cpssDxChPtpTaiTodStepGet",
        &wrCpssDxChPtpTaiTodStepGet,
        4, 0},

    {"cpssDxChPtpTaiPulseDelaySet",
        &wrCpssDxChPtpTaiPulseDelaySet,
        5, 0},

    {"cpssDxChPtpTaiPulseDelayGet",
        &wrCpssDxChPtpTaiPulseDelayGet,
        4, 0},

    {"cpssDxChPtpTaiClockDelaySet",
        &wrCpssDxChPtpTaiClockDelaySet,
        5, 0},

    {"cpssDxChPtpTaiClockDelayGet",
        &wrCpssDxChPtpTaiClockDelayGet,
        4, 0},

    {"cpssDxChPtpTaiFractionalNanosecondDriftSet",
        &wrCpssDxChPtpTaiFractionalNanosecondDriftSet,
        5, 0},

    {"cpssDxChPtpTaiFractionalNanosecondDriftGet",
        &wrCpssDxChPtpTaiFractionalNanosecondDriftGet,
        4, 0},

    {"cpssDxChPtpTaiPClockCycleSet",
        &wrCpssDxChPtpTaiPClockCycleSet,
        5, 0},

    {"cpssDxChPtpTaiPClockCycleGet",
        &wrCpssDxChPtpTaiPClockCycleGet,
        4, 0},

    {"cpssDxChPtpTaiClockCycleSet",
        &wrCpssDxChPtpTaiClockCycleSet,
        6, 0},

    {"cpssDxChPtpTaiClockCycleGet",
        &wrCpssDxChPtpTaiClockCycleGet,
        4, 0},

    {"cpssDxChPtpTaiTodCaptureStatusSet",
        &wrCpssDxChPtpTaiTodCaptureStatusSet,
        6, 0},

    {"cpssDxChPtpTaiTodCaptureStatusGet",
        &wrCpssDxChPtpTaiTodCaptureStatusGet,
        5, 0},

    {"cpssDxChPtpTaiTodUpdateCounterGet",
        &wrCpssDxChPtpTaiTodUpdateCounterGet,
        4, 0},

    {"cpssDxChPtpTaiIncomingTriggerCounterSet",
        &wrCpssDxChPtpTaiIncomingTriggerCounterSet,
        5, 0},

    {"cpssDxChPtpTaiIncomingTriggerCounterGet",
        &wrCpssDxChPtpTaiIncomingTriggerCounterGet,
        4, 0},

    {"cpssDxChPtpTaiIncomingClockCounterSet",
        &wrCpssDxChPtpTaiIncomingClockCounterSet,
        6, 0},

    {"cpssDxChPtpTaiIncomingClockCounterGet",
        &wrCpssDxChPtpTaiIncomingClockCounterGet,
        4, 0},

    {"cpssDxChPtpTaiFrequencyDriftThesholdsSet",
        &wrCpssDxChPtpTaiFrequencyDriftThesholdsSet,
        6, 0},

    {"cpssDxChPtpTaiFrequencyDriftThesholdsGet",
        &wrCpssDxChPtpTaiFrequencyDriftThesholdsGet,
        4, 0},

    {"cpssDxChPtpTaiGracefulStepSet",
        &wrCpssDxChPtpTaiGracefulStepSet,
        5, 0},

    {"cpssDxChPtpTaiGracefulStepGet",
        &wrCpssDxChPtpTaiGracefulStepGet,
        4, 0},

    {"cpssDxChPtpTsuControlTableSet",
        &wrCpssDxChPtpTsuControlTableSet,
        3, 4},

    {"cpssDxChPtpTsuControlTableGetFirst",
        &wrCpssDxChPtpTsuControlTableGetFirst,
        3, 0},

    {"cpssDxChPtpTsuControlTableGetNext",
        &wrCpssDxChPtpTsuControlTableGetNext,
        3, 0},

    {"cpssDxChPtpTsuTxTimestampQueueTableGetFirst",
        &wrCpssDxChPtpTsuTxTimestampQueueTableGetFirst,
        3, 0},

    {"cpssDxChPtpTsuTxTimestampQueueTableGetNext",
        &wrCpssDxChPtpTsuTxTimestampQueueTableGet,
        3, 0},

    {"utilPtpTsuTxTimestampQueueTableReadLimitSet",
        &wrUtilPtpTsuTxTimestampQueueTableReadLimitSet,
        1, 0},

    {"cpssDxChPtpTsuCountersClear",
        &wrCpssDxChPtpTsuCountersClear,
        2, 0},

    {"cpssDxChPtpTsuPacketCouterGet",
        &wrCpssDxChPtpTsuPacketCouterGet,
        3, 0},

    {"cpssDxChPtpTsuNtpTimeOffsetSet",
        &wrCpssDxChPtpTsuNtpTimeOffsetSet,
        3, 0},

    {"cpssDxChPtpTsuNtpTimeOffsetGet",
        &wrCpssDxChPtpTsuNtpTimeOffsetGet,
        2, 0},

    {"cpssDxChPtpTsTagGlobalCfgSet",
        &wrCpssDxChPtpTsTagGlobalCfgSet,
        5, 0},

    {"cpssDxChPtpTsTagGlobalCfgGet",
        &wrCpssDxChPtpTsTagGlobalCfgGet,
        1, 0},

    {"cpssDxChPtpTsTagPortCfgTableSet",
        &wrCpssDxChPtpTsTagPortCfgTableSet,
        3, 4},

    {"cpssDxChPtpTsTagPortCfgTableGetFirst",
        &wrCpssDxChPtpTsTagPortCfgTableGetFirst,
        3, 0},

    {"cpssDxChPtpTsTagPortCfgTableGetNext",
        &wrCpssDxChPtpTsTagPortCfgTableGetNext,
        3, 0},

    {"cpssDxChPtpTsDelayIngressPortDelaySet",
        &wrCpssDxChPtpTsDelayIngressPortDelaySet,
        3, 0},

    {"cpssDxChPtpTsDelayIngressPortDelayGet",
        &wrCpssDxChPtpTsDelayIngressPortDelayGet,
        2, 0},

    {"cpssDxChPtpTsDelayIngressLinkDelaySet",
        &wrCpssDxChPtpTsDelayIngressLinkDelaySet,
        4, 0},

    {"cpssDxChPtpTsDelayIngressLinkDelayGet",
        &wrCpssDxChPtpTsDelayIngressLinkDelayGet,
        3, 0},

    {"cpssDxChPtpTsDelayEgressPipeDelaySet",
        &wrCpssDxChPtpTsDelayEgressPipeDelaySet,
        3, 0},

    {"cpssDxChPtpTsDelayEgressPipeDelayGet",
        &wrCpssDxChPtpTsDelayEgressPipeDelayGet,
        2, 0},

    {"cpssDxChPtpTsDelayEgressAsymmetryCorrectionSet",
        &wrCpssDxChPtpTsDelayEgressAsymmetryCorrectionSet,
        4, 0},

    {"cpssDxChPtpTsDelayEgressAsymmetryCorrectionGet",
        &wrCpssDxChPtpTsDelayEgressAsymmetryCorrectionGet,
        3, 0},

    {"cpssDxChPtpTsDelayIngressCorrFieldPiggybackEnSet",
        &wrCpssDxChPtpTsDelayIngressCorrFieldPiggybackEnableSet,
        3, 0},

    {"cpssDxChPtpTsDelayIngressCorrFieldPiggybackEnGet",
        &wrCpssDxChPtpTsDelayIngressCorrFieldPiggybackEnableGet,
        2, 0},

    {"cpssDxChPtpTsDelayEgressCorrFieldPiggybackEnSet",
        &wrCpssDxChPtpTsDelayEgressCorrFieldPiggybackEnableSet,
        3, 0},

    {"cpssDxChPtpTsDelayEgressCorrFieldPiggybackEnGet",
        &wrCpssDxChPtpTsDelayEgressCorrFieldPiggybackEnableGet,
        2, 0},

    {"cpssDxChPtpTsDelayEgressTimeCorrTaiSelectModeSet",
        &wrCpssDxChPtpTsDelayEgressTimeCorrTaiSelectModeSet,
        3, 0},

    {"cpssDxChPtpTsDelayEgressTimeCorrTaiSelectModeGet",
        &wrCpssDxChPtpTsDelayEgressTimeCorrTaiSelectModeGet,
        2, 0},

    {"cpssDxChPtpTsDelayEgressTimestampTaiSelectSet",
        &wrCpssDxChPtpTsDelayEgressTimestampTaiSelectSet,
        3, 0},

    {"cpssDxChPtpTsDelayEgressTimestampTaiSelectGet",
        &wrCpssDxChPtpTsDelayEgressTimestampTaiSelectGet,
        2, 0},

    {"cpssDxChPtpTsUdpChecksumUpdateModeSet",
        &wrCpssDxChPtpTsUdpChecksumUpdateModeSet,
        7, 0},

    {"cpssDxChPtpTsUdpChecksumUpdateModeGet",
        &wrCpssDxChPtpTsUdpChecksumUpdateModeGet,
        1, 0},

    {"cpssDxChPtpOverEthernetEnableSet",
        &wrCpssDxChPtpOverEthernetEnableSet,
        2, 0},

    {"cpssDxChPtpOverEthernetEnableGet",
        &wrCpssDxChPtpOverEthernetEnableGet,
        1, 0},

    {"cpssDxChPtpOverUdpEnableSet",
        &wrCpssDxChPtpOverUdpEnableSet,
        2, 0},

    {"cpssDxChPtpOverUdpEnableGet",
        &wrCpssDxChPtpOverUdpEnableGet,
        1, 0},

    {"cpssDxChPtpIngressExceptionCfgSet",
        &wrCpssDxChPtpIngressExceptionCfgSet,
        5, 0},

    {"cpssDxChPtpIngressExceptionCfgGet",
        &wrCpssDxChPtpIngressExceptionCfgGet,
        1, 0},

    {"cpssDxChPtpEgressExceptionCfgSet",
        &wrCpssDxChPtpEgressExceptionCfgSet,
        5, 0},

    {"cpssDxChPtpEgressExceptionCfgGet",
        &wrCpssDxChPtpEgressExceptionCfgGet,
        1, 0},

    {"cpssDxChPtpIngressExceptionCounterGet",
        &wrCpssDxChPtpIngressExceptionCounterGet,
        1, 0},

    {"cpssDxChPtpEgressExceptionCountersGet",
        &wrCpssDxChPtpEgressExceptionCountersGet,
        1, 0},

    {"cpssDxChPtpTsMessageTypeToQueueIdMapSet",
        &wrCpssDxChPtpTsMessageTypeToQueueIdMapSet,
        2, 0},

    {"cpssDxChPtpTsMessageTypeToQueueIdMapGet",
        &wrCpssDxChPtpTsMessageTypeToQueueIdMapGet,
        1, 0},

    {"cpssDxChPtpTsQueuesEntryOverrideEnableSet",
        &wrCpssDxChPtpTsQueuesEntryOverrideEnableSet,
        2, 0},

    {"cpssDxChPtpTsQueuesEntryOverrideEnableGet",
        &wrCpssDxChPtpTsQueuesEntryOverrideEnableGet,
        1, 0},

    {"cpssDxChPtpTsQueuesSizeSet",
        &wrCpssDxChPtpTsQueuesSizeSet,
        2, 0},

    {"cpssDxChPtpTsQueuesSizeGet",
        &wrCpssDxChPtpTsQueuesSizeGet,
        1, 0},

    {"cpssDxChPtpTsDebugQueuesEntryIdsClear",
        &wrCpssDxChPtpTsDebugQueuesEntryIdsClear,
        1, 0},

    {"cpssDxChPtpTsDebugQueuesEntryIdsGet",
        &wrCpssDxChPtpTsDebugQueuesEntryIdsGet,
        1, 0},

    {"cpssDxChPtpTsNtpTimeOffsetSet",
        &wrCpssDxChPtpTsNtpTimeOffsetSet,
        2, 0},

    {"cpssDxChPtpTsNtpTimeOffsetGet",
        &wrCpssDxChPtpTsNtpTimeOffsetGet,
        1, 0},

    {"cpssDxChPtpMessageTypeCmdTableSet",
        &wrCpssDxChPtpMessageTypeCmdTableSet,
        3, 2},

    {"cpssDxChPtpMessageTypeCmdTableGetFirst",
        &wrCpssDxChPtpMessageTypeCmdTableGetFirst,
        3, 0},

    {"cpssDxChPtpMessageTypeCmdTableGetNext",
        &wrCpssDxChPtpMessageTypeCmdTableGetNext,
        3, 0},

    {"cpssDxChPtpTsCfgTableSet",
        &wrCpssDxChPtpTsCfgTableSet,
        3, 12},

    {"cpssDxChPtpTsCfgTableGetFirst",
        &wrCpssDxChPtpTsCfgTableGetFirst,
        3, 0},

    {"cpssDxChPtpTsCfgTableGetNext",
        &wrCpssDxChPtpTsCfgTableGetNext,
        3, 0},

    {"cpssDxChPtpTsLocalActionTableSet",
        &wrCpssDxChPtpTsLocalActionTableSet,
        3, 4},

    {"cpssDxChPtpTsLocalActionTableGetFirst",
        &wrCpssDxChPtpTsLocalActionTableGetFirst,
        3, 0},

    {"cpssDxChPtpTsLocalActionTableGetNext",
        &wrCpssDxChPtpTsLocalActionTableGetNext,
        3, 0},

    {"cpssDxChPtpDomainModeTableSet",
        &wrCpssDxChPtpDomainModeTableSet,
        1, 2},

    {"cpssDxChPtpDomainModeTableGetFirst",
        &wrCpssDxChPtpDomainModeTableGetFirst,
        1, 0},

    {"cpssDxChPtpDomainModeTableGetNext",
        &wrCpssDxChPtpDomainModeTableGetNext,
        1, 0},

    {"cpssDxChPtpDomainV1IdTableSet",
        &wrCpssDxChPtpDomainV1IdTableSet,
        1, 5},

    {"cpssDxChPtpDomainV1IdTableGetFirst",
        &wrCpssDxChPtpDomainV1IdTableGetFirst,
        1, 0},

    {"cpssDxChPtpDomainV1IdTableGetNext",
        &wrCpssDxChPtpDomainV1IdTableGetNext,
        1, 0},

    {"cpssDxChPtpDomainV2IdTableSet",
        &wrCpssDxChPtpDomainV2IdTableSet,
        1, 2},

    {"cpssDxChPtpDomainV2IdTableGetFirst",
        &wrCpssDxChPtpDomainV2IdTableGetFirst,
        1, 0},

    {"cpssDxChPtpDomainV2IdTableGetNext",
        &wrCpssDxChPtpDomainV2IdTableGetNext,
        1, 0},

    {"cpssDxChPtpEgressDomainTableSet",
        &wrCpssDxChPtpEgressDomainTableSet,
        2, 6},

    {"cpssDxChPtpEgressDomainTableGetFirst",
        &wrCpssDxChPtpEgressDomainTableGetFirst,
        2, 0},

    {"cpssDxChPtpEgressDomainTableGetNext",
        &wrCpssDxChPtpEgressDomainTableGetNext,
        2, 0},

    {"cpssDxChPtpIngressPacketCheckingModeTblSet",
        &wrCpssDxChPtpIngressPacketCheckingModeTableSet,
        2, 2},

    {"cpssDxChPtpIngressPacketCheckingModeTblGetFirst",
        &wrCpssDxChPtpIngressPacketCheckingModeTableGetFirst,
        2, 0},

    {"cpssDxChPtpIngressPacketCheckingModeTblGetNext",
        &wrCpssDxChPtpIngressPacketCheckingModeTableGetNext,
        2, 0},

    {"cpssDxChPtpTsIngressTimestampQueueTableGetFirst",
        &wrCpssDxChPtpTsIngressTimestampQueueTableGet,
        2, 0},

    {"cpssDxChPtpTsIngressTimestampQueueTableGetNext",
        &wrCpssDxChPtpTsIngressTimestampQueueTableGet,
        2, 0},

    {"cpssDxChPtpTsEgressTimestampQueueTableGetFirst",
        &wrCpssDxChPtpTsEgressTimestampQueueTableGet,
        2, 0},

    {"cpssDxChPtpTsEgressTimestampQueueTableGetNext",
        &wrCpssDxChPtpTsEgressTimestampQueueTableGet,
        2, 0},

    {"cpssDxChPtpTaiPtpPulseIterfaceSet",
        &wrCpssDxChPtpTaiPtpPulseIterfaceSet,
        3, 0},

    {"cpssDxChPtpTaiPtpPulseIterfaceGet",
        &wrCpssDxChPtpTaiPtpPulseIterfaceGet,
        1, 0},

    {"cpssDxChPtpTaiPtpClockInterfaceSet",
        &wrCpssDxChPtpTaiPtpClockInterfaceSet,
        3, 0},

    {"cpssDxChPtpTaiPtpClockInterfaceGet",
        &wrCpssDxChPtpTaiPtpClockInterfaceGet,
        2, 0},

    {"cpssDxChPtpTaiPClkOutputInterfaceSet",
        &wrCpssDxChPtpTaiPClkOutputInterfaceSet,
        3, 0},

    {"cpssDxChPtpTaiPClkOutputInterfaceGet",
        &wrCpssDxChPtpTaiPClkOutputInterfaceGet,
        1, 0},

    {"cpssDxChPtpTaiInputClockSelectSet",
        &wrCpssDxChPtpTaiInputClockSelectSet,
        3, 0},

    {"cpssDxChPtpTaiInputClockSelectGet",
        &wrCpssDxChPtpTaiInputClockSelectGet,
        2, 0},

    {"cpssDxChPtpTaiTodCounterFunctionAllTriggerSet",
        &wrCpssDxChPtpTaiTodCounterFunctionAllTriggerSet,
        1, 0},

    {"cpssDxChPtpPortUnitResetSet",
        &wrCpssDxChPtpPortUnitResetSet,
        4, 0},

    {"cpssDxChPtpPortUnitResetGet",
        &wrCpssDxChPtpPortUnitResetGet,
        2, 0},

    {"cpssDxChPtpPortTxPipeStatusDelaySet",
        &wrCpssDxChPtpPortTxPipeStatusDelaySet,
        3, 0},

    {"cpssDxChPtpPortTxPipeStatusDelayGet",
        &wrCpssDxChPtpPortTxPipeStatusDelayGet,
        2, 0}

};

#define numCommands (sizeof(dbCommands) / sizeof(CMD_COMMAND))

/*******************************************************************************
* cmdLibInitCpssDxChPtp
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
CMD_STATUS cmdLibInitCpssDxChPtp
(
    GT_VOID
)
{
    return cmdInitLibrary(dbCommands, numCommands);
}


