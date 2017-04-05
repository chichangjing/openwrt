/*******************************************************************************
*              (c), Copyright 2006, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* wrapCpssDxChPortInterlaken.c
*
* DESCRIPTION:
*       Wrapper functions for
*           cpss/DxCh/DxChxGen/Port/cpssDxChPortInterlaken.h API's
*
*       DEPENDENCIES:
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
#include <cpss/dxCh/dxChxGen/port/cpssDxChPortInterlaken.h>


/*
 * typedef: struct PRV_CMD_DXCH_PORT_INTERLAKEN_COUNTER_WRAP_TABLE_STC
 *
 * Description: Port Interlaken Counter Table Wrapper output string.
 *
 * Fields:
 *      counterName     - counter name
 *      counterId       - counter ID
 *      counterMaxIndex - counter maximal index
 * Comments:
 */
typedef struct
{
    GT_U8* counterName;
    GT_U32 counterId;
    GT_U32 counterMaxIndex;
} PRV_CMD_DXCH_PORT_INTERLAKEN_COUNTER_WRAP_TABLE_STC;

PRV_CMD_DXCH_PORT_INTERLAKEN_COUNTER_WRAP_TABLE_STC
    prvCmdDxChPortInterlakenCounterWrapTab[] =
{
    {(GT_U8*)"RX_BYTE",           CPSS_PORT_INTERLAKEN_COUNTER_ID_RX_BYTE_E, 1},
    {(GT_U8*)"RX_BAD_PKT",        CPSS_PORT_INTERLAKEN_COUNTER_ID_RX_BAD_PKT_E, 1},
    {(GT_U8*)"RX_PKT",            CPSS_PORT_INTERLAKEN_COUNTER_ID_RX_PKT_E, 1},
    {(GT_U8*)"RX_CRC_ERR",        CPSS_PORT_INTERLAKEN_COUNTER_ID_RX_CRC_ERR_E, 1},
    {(GT_U8*)"TX_BYTE",           CPSS_PORT_INTERLAKEN_COUNTER_ID_TX_BYTE_E, 1},
    {(GT_U8*)"TX_BAD_PKT",        CPSS_PORT_INTERLAKEN_COUNTER_ID_TX_BAD_PKT_E, 1},
    {(GT_U8*)"TX_PKT",            CPSS_PORT_INTERLAKEN_COUNTER_ID_TX_PKT_E, 1},
    {(GT_U8*)"RX_ALIGNMENT_ERR",  CPSS_PORT_INTERLAKEN_COUNTER_ID_RX_ALIGNMENT_ERR_E, 1},
    {(GT_U8*)"RX_ALIGNMENT_FAIL", CPSS_PORT_INTERLAKEN_COUNTER_ID_RX_ALIGNMENT_FAIL_E, 1},
    {(GT_U8*)"RX_BURST_SIZE_ERR", CPSS_PORT_INTERLAKEN_COUNTER_ID_RX_BURST_SIZE_ERR_E, 1}
};
GT_U32 prvCmdDxChPortInterlakenCounterWrapTabSize =
    sizeof(prvCmdDxChPortInterlakenCounterWrapTab)
        / sizeof(prvCmdDxChPortInterlakenCounterWrapTab[0]);


PRV_CMD_DXCH_PORT_INTERLAKEN_COUNTER_WRAP_TABLE_STC
    prvCmdDxChPortInterlakenPerLaneCounterWrapTab[] =
{
    {(GT_U8*)"BLK_TYPE_ERR",      CPSS_PORT_INTERLAKEN_COUNTER_ID_BLK_TYPE_ERR_E, 8},
    {(GT_U8*)"DIAG_CRC_ERR",      CPSS_PORT_INTERLAKEN_COUNTER_ID_DIAG_CRC_ERR_E, 8},
    {(GT_U8*)"WORD_SYNC_ERR",     CPSS_PORT_INTERLAKEN_COUNTER_ID_WORD_SYNC_ERR_E, 8},
};
GT_U32 prvCmdDxChPortInterlakenPerLaneCounterWrapTabSize =
    sizeof(prvCmdDxChPortInterlakenPerLaneCounterWrapTab)
        / sizeof(prvCmdDxChPortInterlakenPerLaneCounterWrapTab[0]);



static GT_U32 wrCpssDxChPortInterlakenCounterTblIndex = 0;
static GT_U32 wrCpssDxChPortInterlakenCounterTblIndexParameter = 0;

/**********************************************************************************/
static CMD_STATUS wrCpssDxChPortInterlakenCounterTblGetNext
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                           result = GT_OK;
    GT_U8                               devNum;
    GT_U8                               portNum;
    GT_U64                              counter;

    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    devNum  = (GT_U8)inArgs[0];
    portNum = (GT_U8)inArgs[1];

    if (wrCpssDxChPortInterlakenCounterTblIndex
        >= prvCmdDxChPortInterlakenCounterWrapTabSize)
    {
        galtisOutput(outArgs, GT_OK, "%d", -1);
        return CMD_OK;
    }

    result = cpssDxChPortInterlakenCounterGet(
        devNum, portNum,
        prvCmdDxChPortInterlakenCounterWrapTab[wrCpssDxChPortInterlakenCounterTblIndex].counterId,
        wrCpssDxChPortInterlakenCounterTblIndexParameter,
        &counter);

    /* check for valid arguments */
    if (result != GT_OK)
    {
        galtisOutput(outArgs, result, "");
        return CMD_OK;
    }

    fieldOutput(
        "%s%d%d%d",
        prvCmdDxChPortInterlakenCounterWrapTab[wrCpssDxChPortInterlakenCounterTblIndex].counterName,
        wrCpssDxChPortInterlakenCounterTblIndexParameter,
        counter.l[0], counter.l[1]);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%f");

    /* next counter */
    wrCpssDxChPortInterlakenCounterTblIndexParameter++;
    if (wrCpssDxChPortInterlakenCounterTblIndexParameter >=
        prvCmdDxChPortInterlakenCounterWrapTab[wrCpssDxChPortInterlakenCounterTblIndex].counterMaxIndex)
    {
        wrCpssDxChPortInterlakenCounterTblIndex++;
        wrCpssDxChPortInterlakenCounterTblIndexParameter = 0;
    }

    return CMD_OK;
}

/**********************************************************************************/
static CMD_STATUS wrCpssDxChPortInterlakenCounterTblGetFirst
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    wrCpssDxChPortInterlakenCounterTblIndex = 0;
    wrCpssDxChPortInterlakenCounterTblIndexParameter = 0;

    return wrCpssDxChPortInterlakenCounterTblGetNext(
        inArgs, inFields, numFields, outArgs);
}


static GT_U32 wrCpssDxChPortInterlakenPerLaneCounterTblIndex = 0;
static GT_U32 wrCpssDxChPortInterlakenPerLaneCounterTblIndexParameter = 0;

/**********************************************************************************/
static CMD_STATUS wrCpssDxChPortInterlakenPerLaneCounterTblGetNext
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                           result = GT_OK;
    GT_U8                               devNum;
    GT_U8                               portNum;
    GT_U64                              counter;

    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    devNum  = (GT_U8)inArgs[0];
    portNum = (GT_U8)inArgs[1];

    if (wrCpssDxChPortInterlakenPerLaneCounterTblIndex
        >= prvCmdDxChPortInterlakenPerLaneCounterWrapTabSize)
    {
        galtisOutput(outArgs, GT_OK, "%d", -1);
        return CMD_OK;
    }

    result = cpssDxChPortInterlakenCounterGet(
        devNum, portNum,
        prvCmdDxChPortInterlakenPerLaneCounterWrapTab[wrCpssDxChPortInterlakenPerLaneCounterTblIndex].counterId,
        wrCpssDxChPortInterlakenPerLaneCounterTblIndexParameter,
        &counter);

    /* check for valid arguments */
    if (result != GT_OK)
    {
        galtisOutput(outArgs, result, "");
        return CMD_OK;
    }

    fieldOutput(
        "%s%d%d%d",
        prvCmdDxChPortInterlakenPerLaneCounterWrapTab[wrCpssDxChPortInterlakenPerLaneCounterTblIndex].counterName,
        wrCpssDxChPortInterlakenPerLaneCounterTblIndexParameter,
        counter.l[0], counter.l[1]);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%f");

    /* next counter */
    wrCpssDxChPortInterlakenPerLaneCounterTblIndexParameter++;
    if (wrCpssDxChPortInterlakenPerLaneCounterTblIndexParameter >=
        prvCmdDxChPortInterlakenPerLaneCounterWrapTab[wrCpssDxChPortInterlakenPerLaneCounterTblIndex].counterMaxIndex)
    {
        wrCpssDxChPortInterlakenPerLaneCounterTblIndex ++;
        wrCpssDxChPortInterlakenPerLaneCounterTblIndexParameter = 0;
    }

    return CMD_OK;
}

/**********************************************************************************/
static CMD_STATUS wrCpssDxChPortInterlakenPerLaneCounterTblGetFirst
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    wrCpssDxChPortInterlakenPerLaneCounterTblIndex = 0;
    wrCpssDxChPortInterlakenPerLaneCounterTblIndexParameter = 0;

    return wrCpssDxChPortInterlakenPerLaneCounterTblGetNext(
        inArgs, inFields, numFields, outArgs);
}

/*******************************************************************************
* cpssDxChPortIlknChannelSpeedSet
*
* DESCRIPTION:
*       Configure Interlaken channel speed.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum      - physical device number.
*       portNum     - physical port number of channel.
*       direction   - TX/RX/Both.
*       speed       - channel speed in Mbps, resolution depends on ASIC. Use 0 to
*                       free channel resources.
*
* OUTPUTS:
*       actualSpeedPtr - because HW constrains dectate granularity, real TX speed
*                       could differ from requested. Application must use this
*                       value to know how much of general ILKN interface bandwidth
*                       remain free for TX.
*
* RETURNS:
*       GT_OK             - on success
*       GT_BAD_PARAM      - on wrong parameter
*       GT_NOT_INITIALIZED - ILKN interface not configured properly
*       GT_OUT_OF_RANGE   - speed out of range
*       GT_NO_RESOURCE    - no bandwidth to supply channel speed
*       GT_HW_ERROR       - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*   - Could be engaged only after cpssDxChPortModeSpeedSet pass.
*   - In Bobcat2 ILKN_TXFIFO allows 40Mbps granularity of TX channel speed if
*       bandwidth of ILKN channel is 40Gbps and 20Mbps if total BW is 20Gbps.
*   - RX speed reconfiguration can cause fragmentation in Packet Reassembly memory
*       and GT_NO_RESOURCE error when actually there is enough free space, to solve
*       this application will have delete all or part of channels and configure them
*       again in one sequence.
*
*******************************************************************************/
CMD_STATUS wrCpssDxChPortIlknChannelSpeedSet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS               result;
    GT_U8                   devNum;
    GT_U8                   portNum;
    CPSS_PORT_DIRECTION_ENT direction;
    GT_U32                  speed;
    GT_U32                  actualSpeed;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum      = (GT_U8)inArgs[0];
    portNum     = (GT_U8)inArgs[1];
    direction   = (CPSS_PORT_DIRECTION_ENT)inArgs[2];
    speed       = (GT_U32)inArgs[3];
   
    /* Override Device and Port */
    CONVERT_DEV_PORT_MAC(devNum, portNum);

    /* call cpss api function */
    result = cpssDxChPortIlknChannelSpeedSet(devNum, portNum, direction, speed,
                                             &actualSpeed);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", actualSpeed);

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChPortIlknChannelSpeedGet
*
* DESCRIPTION:
*       Get Interlaken channel speed.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum      - physical device number.
*       portNum     - physical port number of channel.
*       direction   - TX/RX. Attention: Both not supported!
*
* OUTPUTS:
*       speedPtr    - actual channel speed in Mbps.
*
* RETURNS:
*       GT_OK             - on success
*       GT_BAD_PARAM      - on wrong parameter
*       GT_BAD_PTR        - speedPtr == NULL
*       GT_NOT_INITIALIZED - ILKN interface not configured properly
*       GT_HW_ERROR       - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*   In Bobcat2 ILKN_TXFIFO allows 40Mbps granularity of TX channel speed if
*    bandwidth of ILKN channel is 40Gbps and 20Mbps if total BW is 20Gbps.
*
*******************************************************************************/
CMD_STATUS wrCpssDxChPortIlknChannelSpeedGet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS               result;
    GT_U8                   devNum;
    GT_U8                   portNum;
    CPSS_PORT_DIRECTION_ENT direction;
    GT_U32                  actualSpeed;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum      = (GT_U8)inArgs[0];
    portNum     = (GT_U8)inArgs[1];
    direction   = (CPSS_PORT_DIRECTION_ENT)inArgs[2];
   
    /* Override Device and Port */
    CONVERT_DEV_PORT_MAC(devNum, portNum);

    /* call cpss api function */
    result = cpssDxChPortIlknChannelSpeedGet(devNum, portNum, direction, 
                                             &actualSpeed);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", actualSpeed);

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChPortIlknChannelEnableSet
*
* DESCRIPTION:
*       Enable/disable Interlaken channel.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum      - physical device number.
*       portNum     - physical port number of channel.
*       direction   - TX/RX/Both.
*       enable      -   GT_TRUE - "direction" of channel enabled;
*                       GT_FALSE- "direction" channel disabled.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK             - on success
*       GT_BAD_PARAM      - on wrong parameter
*       GT_NO_RESOURCE    - no bandwidth to supply channel speed
*       GT_HW_ERROR       - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
CMD_STATUS wrCpssDxChPortIlknChannelEnableSet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS   result;
    GT_U8       devNum;
    GT_U8       portNum;
    CPSS_PORT_DIRECTION_ENT direction;
    GT_BOOL     enable;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum  = (GT_U8)inArgs[0];
    portNum = (GT_U8)inArgs[1];
    direction = (CPSS_PORT_DIRECTION_ENT)inArgs[2];
    enable = (GT_BOOL)inArgs[3];

    /* Override Device and Port */
    CONVERT_DEV_PORT_MAC(devNum, portNum);

    /* call cpss api function */
    result = cpssDxChPortIlknChannelEnableSet(devNum, portNum, direction, enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChPortIlknChannelEnableGet
*
* DESCRIPTION:
*       Get Enable/disable state of Interlaken channel.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum      - physical device number.
*       portNum     - physical port number of channel.
*       direction   - TX/RX. Attention: Both not supported.
*
* OUTPUTS:
*       enablePtr   -   GT_TRUE - "direction" of channel enabled;
*                       GT_FALSE- "direction" channel disabled.
*
* RETURNS:
*       GT_OK             - on success
*       GT_BAD_PARAM      - on wrong parameter
*       GT_NO_RESOURCE    - no bandwidth to supply channel speed
*       GT_HW_ERROR       - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
CMD_STATUS wrCpssDxChPortIlknChannelEnableGet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS   result;
    GT_U8       devNum;
    GT_U8       portNum;
    CPSS_PORT_DIRECTION_ENT direction;
    GT_BOOL     enable;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum  = (GT_U8)inArgs[0];
    portNum = (GT_U8)inArgs[1];
    direction = (CPSS_PORT_DIRECTION_ENT)inArgs[2];

    /* Override Device and Port */
    CONVERT_DEV_PORT_MAC(devNum, portNum);

    /* call cpss api function */
    result = cpssDxChPortIlknChannelEnableGet(devNum, portNum, direction, &enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", enable);

    return CMD_OK;
}


/*******************************************************************************
* cpssDxChPortIlknChannelSpeedResolutionSet
*
* DESCRIPTION:
*       Configure Interlaken channel speed resolution.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum                - physical device number.
*       channelSpeedResMbps - channel speed resolution in Mbps
*
* OUTPUTS:
*       maxIlknIFSpeedMbpsPtr - The total bandwidth the Interlaken channels will support with the given resolution (small resolution == small total bandwidth)
                                 
*
* RETURNS:
*       GT_OK             - on success
*       GT_BAD_PARAM      - on wrong parameter
*       GT_OUT_OF_RANGE   - speed out of range
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*   -  default resultion is 40Mbps
*   -  if resolution is  greater than IF bandwidth, GT_BAD_PARAM is returned
*   -  returns GT_BAD_PARAM if requested resolution is not compatible with any currently existing channels.
*      In that case user shall delete all channels that are incompatible with new resolution and
*      repeat the cpssDxChPortInterlakenChannelSpeedResultionSet();
*
*******************************************************************************/
CMD_STATUS wrCpssDxChPortIlknChannelSpeedResolutionSet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS   result;
    GT_U8       devNum;
    GT_U32      channelSpeedResMbps;
    GT_U32      maxIlknIFSpeedMbps;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum  = (GT_U8)inArgs[0];
    channelSpeedResMbps = (GT_U32)inArgs[1];

    /* call cpss api function */
    result = cpssDxChPortIlknChannelSpeedResolutionSet(devNum, channelSpeedResMbps,/*OUT*/&maxIlknIFSpeedMbps);
    if (result == GT_OK)
    {
        cpssOsPrintf("\nmaxIlknIFSpeedMbps = %d\n",maxIlknIFSpeedMbps);
    }
    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChPortIlknChannelSpeedResolutionGet
*
* DESCRIPTION:
*       Get Interlaken channel speed resolution and ilkn IF BW that will be supported.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum                - physical device number.
*
* OUTPUTS:
*       speedResulutionMBpsPtr - channel speed resolution in Mbps
*       maxIlknIFSpeedMbpsPtr  - The total bandwidth the Interlaken channels will support with the given resolution (small resolution == small total bandwidth)
*
* RETURNS:
*       GT_OK             - on success
*       GT_BAD_PARAM      - on wrong parameter
*       GT_OUT_OF_RANGE   - speed out of range
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
CMD_STATUS wrCpssDxChPortIlknChannelSpeedResolutionGet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS   result;
    GT_U8       devNum;
    GT_U32      channelSpeedResMbps;
    GT_U32      maxIlknIFSpeedMbps;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum  = (GT_U8)inArgs[0];

    /* call cpss api function */
    result = cpssDxChPortIlknChannelSpeedResolutionGet(devNum, &channelSpeedResMbps, &maxIlknIFSpeedMbps);
    if (result == GT_OK)
    {
        cpssOsPrintf("\nchannel speed resolution = %d",channelSpeedResMbps);
        cpssOsPrintf("\nmaxIlknIFSpeedMbps       = %d", maxIlknIFSpeedMbps);
        cpssOsPrintf("\n");
    }
    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;

}




/**** database initialization **************************************/

static CMD_COMMAND dbCommands[] =
{
    /* commandName,                                    funcReference                                      funcArgs funcFields */
    { "cpssDxChPortInterlakenCounterTblGetFirst",      &wrCpssDxChPortInterlakenCounterTblGetFirst,         2,      0},
    { "cpssDxChPortInterlakenCounterTblGetNext",       &wrCpssDxChPortInterlakenCounterTblGetNext,          2,      0},
    { "cpssDxChPortInterlakenPerLaneCountTblGetFirst", &wrCpssDxChPortInterlakenPerLaneCounterTblGetFirst,  2,      0},
    { "cpssDxChPortInterlakenPerLaneCountTblGetNext",  &wrCpssDxChPortInterlakenPerLaneCounterTblGetNext,   2,      0},
    { "cpssDxChPortIlknChannelSpeedSet",               &wrCpssDxChPortIlknChannelSpeedSet,                  4,      0},
    { "cpssDxChPortIlknChannelSpeedGet",               &wrCpssDxChPortIlknChannelSpeedGet,                  3,      0},
    { "cpssDxChPortIlknChannelEnableSet",              &wrCpssDxChPortIlknChannelEnableSet,                 4,      0},
    { "cpssDxChPortIlknChannelEnableGet",              &wrCpssDxChPortIlknChannelEnableGet,                 3,      0},
    { "cpssDxChPortIlknChannelSpeedResolutionSet",     &wrCpssDxChPortIlknChannelSpeedResolutionSet,        2,      0},
    { "cpssDxChPortIlknChannelSpeedResolutionGet",     &wrCpssDxChPortIlknChannelSpeedResolutionGet,        1,      0}
};

#define numCommands (sizeof(dbCommands) / sizeof(CMD_COMMAND))

/*******************************************************************************
* cmdLibInitCpssDxChPortInterlaken
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
GT_STATUS cmdLibInitCpssDxChPortInterlaken
(
    GT_VOID
)
{
    return cmdInitLibrary(dbCommands, numCommands);
}

