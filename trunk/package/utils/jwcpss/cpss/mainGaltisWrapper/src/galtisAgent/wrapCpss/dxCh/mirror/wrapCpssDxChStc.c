/*******************************************************************************
*              (c), Copyright 2006, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* wrapCpssDxChStc.c
*
* DESCRIPTION:
*       TODO: Add proper description of this file here
*
*       DEPENDENCIES:
*       None.
*
* FILE REVISION NUMBER:
*       $Revision: 4 $
*
*******************************************************************************/

/* Common galtis includes */
#include <galtisAgent/wrapUtil/cmdCpssPresteraUtils.h>
#include <cmdShell/cmdDb/cmdBase.h>
#include <cmdShell/common/cmdWrapUtils.h>

/* Feature specific includes. */
#include <cpss/generic/cpssTypes.h>
#include <cpss/dxCh/dxChxGen/mirror/cpssDxChStc.h>

/*******************************************************************************
* cpssDxChStcIngressCountModeSet
*
* DESCRIPTION:
*       Set the type of packets subject to Ingress Sampling to CPU.
*
* APPLICABLE DEVICES:  All DXCH devices
*
* INPUTS:
*       devNum    - the device number
*       mode      - CPSS_DXCH_STC_COUNT_ALL_PACKETS_E - All packets without
*                                                         any MAC-level errors.
*                   CPSS_DXCH_STC_COUNT_NON_DROPPED_PACKETS_E -
*                                                       only non-dropped packets.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK         - on success.
*       GT_BAD_PARAM  - wrong device number.
*       GT_HW_ERROR   - on writing to HW error.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChStcIngressCountModeSet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    CPSS_DXCH_STC_COUNT_MODE_ENT mode;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];
    mode = (CPSS_DXCH_STC_COUNT_MODE_ENT)inArgs[1];

    /* call cpss api function */
    result = cpssDxChStcIngressCountModeSet(devNum, mode);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChStcIngressCountModeGet
*
* DESCRIPTION:
*       Get the type of packets that are subject to Ingress Sampling to CPU.
*
* APPLICABLE DEVICES:  All DXCH devices
*
* INPUTS:
*       devNum    - the device number
*
* OUTPUTS:
*       modePtr   - CPSS_DXCH_STC_COUNT_ALL_PACKETS_E = All packets without
*                                                           any MAC-level errors.
*                   CPSS_DXCH_STC_COUNT_NON_DROPPED_PACKETS_E -
*                                                       only non-dropped packets.
*
* RETURNS:
*       GT_OK         - on success.
*       GT_BAD_PARAM  - wrong device number.
*       GT_HW_ERROR   - on writing to HW error.
*       GT_BAD_PTR    - one of the parameters is NULL pointer
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChStcIngressCountModeGet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    CPSS_DXCH_STC_COUNT_MODE_ENT mode;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];

    /* call cpss api function */
    result = cpssDxChStcIngressCountModeGet(devNum, &mode);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", mode);

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChStcReloadModeSet
*
* DESCRIPTION:
*       Set the type of Sampling To CPU (STC) count reload mode.
*
* APPLICABLE DEVICES:  All DXCH devices
*
* INPUTS:
*       devNum    - the device number
*       stcType   - Sampling To CPU (STC) type: ingress or egress
*       mode      - Sampling to CPU (STC) Reload mode
*                   CPSS_DXCH_STC_COUNT_RELOAD_CONTINUOUS_E - contiuous mode
*                   CPSS_DXCH_STC_COUNT_RELOAD_TRIGGERED_E - triggered mode
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK         - on success.
*       GT_BAD_PARAM  - wrong device number,STC type or mode.
*       GT_HW_ERROR   - on writing to HW error.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChStcReloadModeSet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    CPSS_DXCH_STC_TYPE_ENT stcType;
    CPSS_DXCH_STC_COUNT_RELOAD_MODE_ENT mode;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];
    stcType = (CPSS_DXCH_STC_TYPE_ENT)inArgs[1];
    mode = (CPSS_DXCH_STC_COUNT_RELOAD_MODE_ENT)inArgs[2];

    /* call cpss api function */
    result = cpssDxChStcReloadModeSet(devNum, stcType, mode);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChStcReloadModeGet
*
* DESCRIPTION:
*       Get the type of Sampling To CPU (STC) count reload mode.
*
* APPLICABLE DEVICES:  All DXCH devices
*
* INPUTS:
*       devNum    - the device number
*       stcType   - Sampling To CPU (STC) type: ingress or egress
*
* OUTPUTS:
*       modePtr   - (pointer to) Sampling to CPU (STC) Reload mode
*                   CPSS_DXCH_STC_COUNT_RELOAD_CONTINUOUS_E - contiuous mode
*                   CPSS_DXCH_STC_COUNT_RELOAD_TRIGGERED_E - triggered mode
*
* RETURNS:
*       GT_OK         - on success.
*       GT_BAD_PARAM  - wrong device number or STC type.
*       GT_HW_ERROR   - on writing to HW error.
*       GT_BAD_PTR    - one of the parameters is NULL pointer
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChStcReloadModeGet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    CPSS_DXCH_STC_TYPE_ENT stcType;
    CPSS_DXCH_STC_COUNT_RELOAD_MODE_ENT mode;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];
    stcType = (CPSS_DXCH_STC_TYPE_ENT)inArgs[1];

    /* call cpss api function */
    result = cpssDxChStcReloadModeGet(devNum, stcType, &mode);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", mode);

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChStcEnableSet
*
* DESCRIPTION:
*       Global Enable/Disable Sampling To CPU (STC).
*
* APPLICABLE DEVICES:  All DXCH devices
*
* INPUTS:
*       devNum    - the device number
*       stcType   - Sampling To CPU (STC) type: ingress or egress
*       enable    - GT_TRUE = enable Sampling To CPU (STC)
*                   GT_FALSE = disable Sampling To CPU (STC)
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK         - on success.
*       GT_BAD_PARAM  - wrong device number or STC type.
*       GT_HW_ERROR   - on writing to HW error.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChStcEnableSet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    CPSS_DXCH_STC_TYPE_ENT stcType;
    GT_BOOL enable;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];
    stcType = (CPSS_DXCH_STC_TYPE_ENT)inArgs[1];
    enable = (GT_BOOL)inArgs[2];

    /* call cpss api function */
    result = cpssDxChStcEnableSet(devNum, stcType, enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChStcEnableGet
*
* DESCRIPTION:
*       Get the global status of Sampling To CPU (STC) (Enabled/Disabled).
*
* APPLICABLE DEVICES:  All DXCH devices
*
* INPUTS:
*       devNum    - the device number
*       stcType   - Sampling To CPU (STC) type: ingress or egress
*
* OUTPUTS:
*       enablePtr - GT_TRUE = enable Sampling To CPU (STC)
*                   GT_FALSE = disable Sampling To CPU (STC)
*
* RETURNS:
*       GT_OK         - on success.
*       GT_BAD_PARAM  - wrong device number or STC type.
*       GT_HW_ERROR   - on writing to HW error.
*       GT_BAD_PTR    - one of the parameters is NULL pointer
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChStcEnableGet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    CPSS_DXCH_STC_TYPE_ENT stcType;
    GT_BOOL enable;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];
    stcType = (CPSS_DXCH_STC_TYPE_ENT)inArgs[1];

    /* call cpss api function */
    result = cpssDxChStcEnableGet(devNum, stcType, &enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", enable);

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChStcPortLimitSet
*
* DESCRIPTION:
*       Set Sampling to CPU (STC) limit per port.
*       The limit to be loaded into the Count Down Counter.
*       This counter is decremented for each packet received on this port and is
*       subject to sampling according to the setting of STC Count mode.
*       When this counter is decremented from 1 to 0, the packet causing this
*       decrement is sampled to the CPU.
*
* APPLICABLE DEVICES:  All DXCH devices
*
* INPUTS:
*       devNum    - the device number
*       port      - port number.
*       stcType   - Sampling To CPU (STC) type: ingress or egress
*       limit     - Count Down Limit (0 - 0x3FFFFFFF)
*                   when limit value is 1 - every packet is sampled to CPU
*                   when limit value is 0 - there is no sampling
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK         - on success.
*       GT_BAD_PARAM  - wrong device number,port number or STC type.
*       GT_HW_ERROR   - on writing to HW error.
*       GT_BAD_PTR    - one of the parameters is NULL pointer
*       GT_OUT_OF_RANGE - limit is out of range
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChStcPortLimitSet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    GT_PHYSICAL_PORT_NUM port;
    CPSS_DXCH_STC_TYPE_ENT stcType;
    GT_U32 limit;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];
    port = (GT_PHYSICAL_PORT_NUM)inArgs[1];
    stcType = (CPSS_DXCH_STC_TYPE_ENT)inArgs[2];
    limit = (GT_U32)inArgs[3];

    /* Override Device and Port */
    CONVERT_DEV_PHYSICAL_PORT_MAC(devNum, port);

    /* call cpss api function */
    result = cpssDxChStcPortLimitSet(devNum, port, stcType, limit);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChStcPortLimitGet
*
* DESCRIPTION:
*       Get Sampling to CPU (STC) limit per port.
*       The limit to be loaded into the Count Down Counter.
*       This counter is decremented for each packet received on this port and is
*       subject to sampling according to the setting of STC Count mode.
*       When this counter is decremented from 1 to 0, the packet causing this
*       decrement is sampled to the CPU.
*
* APPLICABLE DEVICES:  All DXCH devices
*
* INPUTS:
*       devNum    - the device number
*       port      - port number
*       stcType   - Sampling To CPU (STC) type: ingress or egress
*
* OUTPUTS:
*       limitPtr  - pointer to Count Down Limit (0 - 0x3FFFFFFF)
*                   when limit value is 1 - every packet is sampled to CPU
*                   when limit value is 0 - there is no sampling
*
* RETURNS:
*       GT_OK         - on success.
*       GT_BAD_PARAM  - wrong device number,port number or STC type.
*       GT_HW_ERROR   - on writing to HW error.
*       GT_BAD_PTR    - one of the parameters is NULL pointer
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChStcPortLimitGet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    GT_PHYSICAL_PORT_NUM port;
    CPSS_DXCH_STC_TYPE_ENT stcType;
    GT_U32 limit;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];
    port = (GT_PHYSICAL_PORT_NUM)inArgs[1];
    stcType = (CPSS_DXCH_STC_TYPE_ENT)inArgs[2];

    /* Override Device and Port */
    CONVERT_DEV_PHYSICAL_PORT_MAC(devNum, port);

    /* call cpss api function */
    result = cpssDxChStcPortLimitGet(devNum, port, stcType, &limit);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", limit);

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChStcPortReadyForNewLimitGet
*
* DESCRIPTION:
*       Check whether Sampling to CPU is ready to get new STC Limit Value per
*       port.
*       The function cpssDxChStcPortLimitSet sets new limit value.
*       But only after device finishes handling of new limit value the
*       cpssDxChStcPortLimitSet may be called once more.
*
*
* APPLICABLE DEVICES:  All DXCH devices
*
* INPUTS:
*       devNum    - the device number
*       port      - port number
*       stcType   - Sampling To CPU (STC) type: ingress or egress
*
* OUTPUTS:
*       isReadyPtr - (pointer to) Is Ready value
*                    GT_TRUE = Sampling to CPU ready to get new STC Limit Value
*                    GT_FALSE = Sampling to CPU handles STC Limit Value yet and
*                               is not ready to get new value.
*
* RETURNS:
*       GT_OK         - on success.
*       GT_BAD_PARAM  - wrong device number,port number or STC type.
*       GT_HW_ERROR   - on writing to HW error.
*       GT_BAD_PTR    - one of the parameters is NULL pointer
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChStcPortReadyForNewLimitGet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    GT_PHYSICAL_PORT_NUM port;
    CPSS_DXCH_STC_TYPE_ENT stcType;
    GT_BOOL isReady;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];
    port = (GT_PHYSICAL_PORT_NUM)inArgs[1];
    stcType = (CPSS_DXCH_STC_TYPE_ENT)inArgs[2];

    /* Override Device and Port */
    CONVERT_DEV_PHYSICAL_PORT_MAC(devNum, port);

    /* call cpss api function */
    result = cpssDxChStcPortReadyForNewLimitGet(devNum, port, stcType, &isReady);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", isReady);

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChStcPortCountdownCntrGet
*
* DESCRIPTION:
*       Get STC Countdown Counter per port.
*       This counter is decremented for each packet received on this port and is
*       subject to sampling according to the setting of STC Count mode.
*       When this counter is decremented from 1 to 0, the packet causing this
*       decrement is sampled to the CPU.
*
* APPLICABLE DEVICES:  All DXCH devices
*
* INPUTS:
*       devNum    - the device number
*       port      - port number
*       stcType   - Sampling To CPU (STC) type: ingress or egress
*
* OUTPUTS:
*       cntrPtr   - (pointer to) STC Count down counter. This is the number of
*                   packets left to send/receive in order that a packet will be
*                   sampled to CPU and a new value will be loaded.
*
* RETURNS:
*       GT_OK         - on success.
*       GT_BAD_PARAM  - wrong device number,port number or STC type.
*       GT_HW_ERROR   - on writing to HW error.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChStcPortCountdownCntrGet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    GT_PHYSICAL_PORT_NUM port;
    CPSS_DXCH_STC_TYPE_ENT stcType;
    GT_U32 cntr;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];
    port = (GT_PHYSICAL_PORT_NUM)inArgs[1];
    stcType = (CPSS_DXCH_STC_TYPE_ENT)inArgs[2];

    /* Override Device and Port */
    CONVERT_DEV_PHYSICAL_PORT_MAC(devNum, port);

    /* call cpss api function */
    result = cpssDxChStcPortCountdownCntrGet(devNum, port, stcType, &cntr);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", cntr);

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChStcPortSampledPacketsCntrSet
*
* DESCRIPTION:
*       Set the number of packets Sampled to CPU per port.
*
* APPLICABLE DEVICES:  All DXCH devices
*
* INPUTS:
*       devNum    - the device number
*       port      - port number
*       stcType   - Sampling To CPU (STC) type: ingress or egress
*       cntr      - STC Sampled to CPU packet's counter (0 - 0xFFFF).
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK         - on success.
*       GT_BAD_PARAM  - wrong device number,port number or STC type.
*       GT_HW_ERROR   - on writing to HW error.
*       GT_OUT_OF_RANGE - on wrong cntr
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChStcPortSampledPacketsCntrSet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    GT_PHYSICAL_PORT_NUM port;
    CPSS_DXCH_STC_TYPE_ENT stcType;
    GT_U32 cntr;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];
    port = (GT_PHYSICAL_PORT_NUM)inArgs[1];
    stcType = (CPSS_DXCH_STC_TYPE_ENT)inArgs[2];
    cntr = (GT_U32)inArgs[3];

    /* Override Device and Port */
    CONVERT_DEV_PHYSICAL_PORT_MAC(devNum, port);

    /* call cpss api function */
    result = cpssDxChStcPortSampledPacketsCntrSet(devNum, port, stcType, cntr);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChStcPortSampledPacketsCntrGet
*
* DESCRIPTION:
*       Get the number of packets Sampled to CPU per port.
*
* APPLICABLE DEVICES:  All DXCH devices
*
* INPUTS:
*       devNum    - the device number
*       port      - port number
*       stcType   - Sampling To CPU (STC) type: ingress or egress
*
* OUTPUTS:
*       cntrPtr   - (pointer to) STC Sampled to CPU packet's counter (0 - 0xFFFF).
*
* RETURNS:
*       GT_OK         - on success.
*       GT_BAD_PARAM  - wrong device number,port number or STC type.
*       GT_HW_ERROR   - on writing to HW error.
*       GT_BAD_PTR    - one of the parameters is NULL pointer
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChStcPortSampledPacketsCntrGet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    GT_PHYSICAL_PORT_NUM port;
    CPSS_DXCH_STC_TYPE_ENT stcType;
    GT_U32 cntr;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];
    port = (GT_PHYSICAL_PORT_NUM)inArgs[1];
    stcType = (CPSS_DXCH_STC_TYPE_ENT)inArgs[2];

    /* Override Device and Port */
    CONVERT_DEV_PHYSICAL_PORT_MAC(devNum, port);

    /* call cpss api function */
    result = cpssDxChStcPortSampledPacketsCntrGet(devNum, port, stcType, &cntr);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", cntr);

    return CMD_OK;
}


/*******************************************************************************
* cpssDxChStcEgressAnalyzerIndexSet
*
* DESCRIPTION:
*       Set the egress STC analyzer index.
*
*       feature behavior:
*       If a packet is marked for both egress STC and egress mirroring , the
*       packet is only replicated once, depending on a configurable priority
*       between the mechanisms. The configurable <STC analyzer index> determines
*       the priority of STC compared to mirroring to the analyzer.
*       Note:
*       1. that STC does not actually use this index to determine the target
*          of the mirrored packet.
*       2. the indexes used by egress mirroring engines are in range 0..6.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*      devNum    - device number.
*      enable    - global enable/disable analyzer index for egress STC.
*                  - GT_TRUE - enable analyzer index.
*                  - GT_FALSE - no analyzer index.
*      index     - Analyzer index. (APPLICABLE RANGES: 0..14)
*
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - wrong device.
*       GT_OUT_OF_RANGE          - index is out of range.
*       GT_HW_ERROR              - on HW error.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       function uses same parameters as mirroring functions like
*       cpssDxChMirrorTxGlobalAnalyzerInterfaceIndexSet(devNum,enable,index).
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChStcEgressAnalyzerIndexSet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8       devNum;
    GT_BOOL   enable;
    GT_U32    index;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum =  (GT_U8)inArgs[0];
    enable =  (GT_BOOL)inArgs[1];
    index =  (GT_U32)inArgs[2];

    /* call cpss api function */
    result = cpssDxChStcEgressAnalyzerIndexSet(devNum, enable , index);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}

/*******************************************************************************
* cpssDxChStcEgressAnalyzerIndexGet
*
* DESCRIPTION:
*       Get the egress STC analyzer index.
*
*       feature behavior:
*       If a packet is marked for both egress STC and egress mirroring , the
*       packet is only replicated once, depending on a configurable priority
*       between the mechanisms. The configurable <STC analyzer index> determines
*       the priority of STC compared to mirroring to the analyzer.
*       Note:
*       1. that STC does not actually use this index to determine the target
*          of the mirrored packet.
*       2. the indexes used by egress mirroring engines are in range 0..6.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*      devNum    - device number.
*
*
* OUTPUTS:
*      enablePtr    - (pointer to) global enable/disable analyzer index for egress STC.
*                  - GT_TRUE - enable analyzer index.
*                  - GT_FALSE - no analyzer index.
*      indexPtr     - (pointer to) Analyzer destination interface index.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - wrong device.
*       GT_OUT_OF_RANGE          - index is out of range.
*       GT_BAD_PTR               - on NULL pointer.
*       GT_HW_ERROR              - on HW error.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       function uses same parameters as mirroring functions like
*       cpssDxChMirrorTxGlobalAnalyzerInterfaceIndexGet(devNum,enablePtr,indexPtr).
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChStcEgressAnalyzerIndexGet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8       devNum;
    GT_BOOL   enable;
    GT_U32    index;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum =  (GT_U8)inArgs[0];

    /* call cpss api function */
    result = cpssDxChStcEgressAnalyzerIndexGet(devNum, &enable ,&index );

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d%d", enable , index);
    return CMD_OK;
}


/**** database initialization **************************************/



static CMD_COMMAND dbCommands[] =
{
        {"cpssDxChStcIngressCountModeSet",
         &wrCpssDxChStcIngressCountModeSet,
         2, 0},
        {"cpssDxChStcIngressCountModeGet",
         &wrCpssDxChStcIngressCountModeGet,
         1, 0},
        {"cpssDxChStcReloadModeSet",
         &wrCpssDxChStcReloadModeSet,
         3, 0},
        {"cpssDxChStcReloadModeGet",
         &wrCpssDxChStcReloadModeGet,
         2, 0},
        {"cpssDxChStcEnableSet",
         &wrCpssDxChStcEnableSet,
         3, 0},
        {"cpssDxChStcEnableGet",
         &wrCpssDxChStcEnableGet,
         2, 0},
        {"cpssDxChStcPortLimitSet",
         &wrCpssDxChStcPortLimitSet,
         4, 0},
        {"cpssDxChStcPortLimitGet",
         &wrCpssDxChStcPortLimitGet,
         3, 0},
        {"cpssDxChStcPortReadyForNewLimitGet",
         &wrCpssDxChStcPortReadyForNewLimitGet,
         3, 0},
        {"cpssDxChStcPortCountdownCntrGet",
         &wrCpssDxChStcPortCountdownCntrGet,
         3, 0},
        {"cpssDxChStcPortSampledPacketsCntrSet",
         &wrCpssDxChStcPortSampledPacketsCntrSet,
         4, 0},
        {"cpssDxChStcPortSampledPacketsCntrGet",
         &wrCpssDxChStcPortSampledPacketsCntrGet,
         3, 0},
        {"cpssDxChStcEgressAnalyzerIndexSet",
         &wrCpssDxChStcEgressAnalyzerIndexSet,
         3, 0},
        {"cpssDxChStcEgressAnalyzerIndexGet",
         &wrCpssDxChStcEgressAnalyzerIndexGet,
         1, 0}
};

#define numCommands (sizeof(dbCommands) / sizeof(CMD_COMMAND))

/*******************************************************************************
* cmdLibInitCpssDxChStc
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
GT_STATUS cmdLibInitCpssDxChStc
(
    GT_VOID
)
{
    return cmdInitLibrary(dbCommands, numCommands);
}

