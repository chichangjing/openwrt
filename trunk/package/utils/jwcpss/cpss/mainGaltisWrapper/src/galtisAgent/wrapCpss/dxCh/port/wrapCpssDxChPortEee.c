/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* wrapCpssDxChPortEee.c
*
* DESCRIPTION:
*       Wrapper functions for CPSS DXCH Port EEE API
*       CPSS DxCh Port's IEEE 802.3az Energy Efficient Ethernet (EEE) Support.
*       NOTEs:
*       1. The device support relevant when the PHY works in EEE slave mode.
*          When the PHY works in EEE master mode, the device is not involved in
*          EEE processing.
*       2. The feature not relevant when port is not 'tri-speed'
*       3. LPI is short for 'Low Power Idle'
*
* FILE REVISION NUMBER:
*       $Revision: 1 $
*
*******************************************************************************/

/* Common galtis includes */
#include <galtisAgent/wrapUtil/cmdCpssPresteraUtils.h>
#include <cmdShell/cmdDb/cmdBase.h>
#include <cmdShell/common/cmdWrapUtils.h>


/* Feature specific includes. */
#include <cpss/dxCh/dxChxGen/port/cpssDxChPortEee.h>
#include <cpss/generic/config/private/prvCpssConfigTypes.h>
#include <cpss/dxCh/dxChxGen/config/private/prvCpssDxChInfo.h>

/*******************************************************************************
* cpssDxChPortEeeLpiRequestEnableSet
*
* DESCRIPTION:
*       Set port enable/disable transmitting of LPI signaling to the PHY and
*       identification of LPI on reception.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
*
* INPUTS:
*       devNum    - physical device number
*       portNum   - physical port number
*       enable    - GT_TRUE  - enable
*                   GT_FALSE - disable
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong port number or devNum
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChPortEeeLpiRequestEnableSet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_U32   ii = 0;
    GT_STATUS                       result;
    GT_U8                           devNum;
    GT_PHYSICAL_PORT_NUM            portNum;
    GT_BOOL                         enable;

    /* check for valid arguments */
    if (!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[ii++];
    portNum  = (GT_PHYSICAL_PORT_NUM)inArgs[ii++];
    enable  = (GT_BOOL)inArgs[ii++];

    CONVERT_DEV_PHYSICAL_PORT_MAC(devNum, portNum);

    /* call cpss api function */
    result = cpssDxChPortEeeLpiRequestEnableSet(devNum, portNum, enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}

/*******************************************************************************
* cpssDxChPortEeeLpiRequestEnableGet
*
* DESCRIPTION:
*       Get port's enable/disable transmitting of LPI signaling to the PHY and
*       identification of LPI on reception.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
*
* INPUTS:
*       devNum    - physical device number
*       portNum   - physical port number
*
* OUTPUTS:
*       enablePtr   -  (pointer to)
*                       GT_TRUE  - enabled
*                       GT_FALSE - disabled
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong port number or devNum
*       GT_BAD_PTR               - on NULL pointer
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChPortEeeLpiRequestEnableGet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_U32   ii = 0;
    GT_STATUS  result;
    GT_U8      devNum;
    GT_PHYSICAL_PORT_NUM            portNum;
    GT_BOOL    enable;

    /* check for valid arguments */
    if (!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[ii++];
    portNum  = (GT_PHYSICAL_PORT_NUM)inArgs[ii++];

    CONVERT_DEV_PHYSICAL_PORT_MAC(devNum, portNum);

    /* call cpss api function */
    result = cpssDxChPortEeeLpiRequestEnableGet(devNum, portNum, &enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", enable);
    return CMD_OK;
}

/*******************************************************************************
* cpssDxChPortEeeLpiManualModeSet
*
* DESCRIPTION:
*       Set port manual mode (enabled/disabled/forced) for LPI signaling.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
*
* INPUTS:
*       devNum    - physical device number
*       portNum   - physical port number
*       mode      - the manual mode
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong port number or devNum
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChPortEeeLpiManualModeSet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_U32   ii = 0;
    GT_STATUS                       result;
    GT_U8                           devNum;
    GT_PHYSICAL_PORT_NUM            portNum;
    CPSS_DXCH_PORT_EEE_LPI_MANUAL_MODE_ENT mode;

    /* check for valid arguments */
    if (!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[ii++];
    portNum  = (GT_PHYSICAL_PORT_NUM)inArgs[ii++];
    mode  = (CPSS_DXCH_PORT_EEE_LPI_MANUAL_MODE_ENT)inArgs[ii++];

    CONVERT_DEV_PHYSICAL_PORT_MAC(devNum, portNum);

    /* call cpss api function */
    result = cpssDxChPortEeeLpiManualModeSet(devNum, portNum, mode);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}

/*******************************************************************************
* cpssDxChPortEeeLpiManualModeGet
*
* DESCRIPTION:
*       Get port manual mode (enabled/disabled/forced) for LPI signaling.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
*
* INPUTS:
*       devNum    - physical device number
*       portNum   - physical port number
*
* OUTPUTS:
*       modePtr   - (pointer to) the manual mode
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong port number or devNum
*       GT_BAD_PTR               - on NULL pointer
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChPortEeeLpiManualModeGet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_U32   ii = 0;
    GT_STATUS  result;
    GT_U8      devNum;
    GT_PHYSICAL_PORT_NUM            portNum;
    CPSS_DXCH_PORT_EEE_LPI_MANUAL_MODE_ENT mode;

    /* check for valid arguments */
    if (!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[ii++];
    portNum  = (GT_PHYSICAL_PORT_NUM)inArgs[ii++];

    CONVERT_DEV_PHYSICAL_PORT_MAC(devNum, portNum);

    /* call cpss api function */
    result = cpssDxChPortEeeLpiManualModeGet(devNum, portNum, &mode);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", mode);
    return CMD_OK;
}

/*******************************************************************************
* cpssDxChPortEeeLpiTimeLimitsSet
*
* DESCRIPTION:
*       Set port LPI timers.
*       NOTE: The 'time' granularity is 1 uSec for 1000 Mbps and 10 uSec for 100 Mbps.
*       so the function set value 'as is'.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum    - physical device number
*       portNum   - physical port number
*       twLimit   - The minimum time from LPI de-assertion until valid data can be sent .
*                   (APPLICABLE RANGES: 0..4095)
*       liLimit   - The minimum time since going into LPI and allowing LPI de-assert.
*                   (APPLICABLE RANGES: 0..254)
*       tsLimit   - The minimum time from emptying of Tx FIFO and LPI assert.
*                   (APPLICABLE RANGES: 0..255)
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong port number or devNum
*       GT_OUT_OF_RANGE          - on out of range : twLimit , liLimit , tsLimit
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChPortEeeLpiTimeLimitsSet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_U32   ii = 0;
    GT_STATUS                       result;
    GT_U8                           devNum;
    GT_PHYSICAL_PORT_NUM            portNum;
    GT_U32                  twLimit;
    GT_U32                  liLimit;
    GT_U32                  tsLimit;

    /* check for valid arguments */
    if (!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[ii++];
    portNum  = (GT_PHYSICAL_PORT_NUM)inArgs[ii++];
    twLimit  = (GT_U32)inArgs[ii++];
    liLimit  = (GT_U32)inArgs[ii++];
    tsLimit  = (GT_U32)inArgs[ii++];

    CONVERT_DEV_PHYSICAL_PORT_MAC(devNum, portNum);

    /* call cpss api function */
    result = cpssDxChPortEeeLpiTimeLimitsSet(devNum, portNum, twLimit, liLimit, tsLimit);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}

/*******************************************************************************
* cpssDxChPortEeeLpiTimeLimitsGet
*
* DESCRIPTION:
*       Get port LPI timers.
*       NOTE: The 'time' granularity is 1 uSec for 1000 Mbps and 10 uSec for 100 Mbps.
*       so the function get value 'as is'.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum    - physical device number
*       portNum   - physical port number
* OUTPUTS:
*       twLimitPtr - (pointer to) The minimum time from LPI de-assertion until valid data can be sent .
*       liLimitPtr - (pointer to) The minimum time since going into LPI and allowing LPI de-assert.
*       tsLimitPtr - (pointer to) The minimum time from emptying of Tx FIFO and LPI assert.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong port number or devNum
*       GT_BAD_PTR               - on NULL pointer
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChPortEeeLpiTimeLimitsGet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_U32   ii = 0;
    GT_STATUS  result;
    GT_U8      devNum;
    GT_PHYSICAL_PORT_NUM            portNum;
    GT_U32                  twLimit;
    GT_U32                  liLimit;
    GT_U32                  tsLimit;

    /* check for valid arguments */
    if (!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[ii++];
    portNum  = (GT_PHYSICAL_PORT_NUM)inArgs[ii++];

    CONVERT_DEV_PHYSICAL_PORT_MAC(devNum, portNum);

    /* call cpss api function */
    result = cpssDxChPortEeeLpiTimeLimitsGet(devNum, portNum, &twLimit, &liLimit , &tsLimit);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d%d%d", twLimit, liLimit, tsLimit);
    return CMD_OK;
}



/* EEE per port Table */
static GT_PHYSICAL_PORT_NUM   portIndex;
/* look for valid portIndex that hold 'Mac' */
static GT_STATUS getPortWithMac(IN GT_U8    devNum)
{
    GT_U32  portMacNum;
    GT_STATUS rc;

    do{
        rc = prvCpssDxChPortPhysicalPortMapCheckAndConvert(devNum, portIndex, PRV_CPSS_DXCH_PORT_TYPE_MAC_E, &portMacNum);
        if(rc != GT_OK)
        {
            portIndex++;
            continue;
        }

        return GT_OK;
    }while(portIndex < 256);

    return GT_NO_MORE;
}

/*******************************************************************************
* cpssDxChPortEeeLpiStatusGet
*
* DESCRIPTION:
*       Get port LPI status.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum    - physical device number
*       portNum   - physical port number
* OUTPUTS:
*       statusPtr - (pointer to) the status info.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong port number or devNum
*       GT_BAD_PTR               - on NULL pointer
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChPortEeeLpiStatusGetNext
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_U32   ii = 0;
    GT_STATUS                       result;
    GT_U8                           devNum;
    GT_PHYSICAL_PORT_NUM            portNum;
    CPSS_DXCH_PORT_EEE_LPI_STATUS_STC statusInfo;

    /* check for valid arguments */
    if (!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[ii++];

    if (GT_OK != getPortWithMac(devNum))
    {
        /* no more ports with MAC */
        galtisOutput(outArgs, GT_OK, "%d", -1);
        return CMD_OK;
    }

    /* call cpss api function */
    result = cpssDxChPortEeeLpiStatusGet(devNum, portIndex, &statusInfo);

    portNum = portIndex;
    CONVERT_BACK_DEV_PHYSICAL_PORT_MAC(devNum, portNum);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d%d%d%d%d%d",portNum,
        statusInfo.macTxPathLpi,
        statusInfo.macTxPathLpWait,
        statusInfo.macRxPathLpi,
        statusInfo.pcsTxPathLpi,
        statusInfo.pcsRxPathLpi
    );

    portIndex++;

    return CMD_OK;
}

/* get first */
static CMD_STATUS wrCpssDxChPortEeeLpiStatusGetFirst
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    portIndex = 0;
    return wrCpssDxChPortEeeLpiStatusGetNext(inArgs,inFields,numFields,outArgs);
}


/**** database initialization **************************************/

static CMD_COMMAND dbCommands[] =
{
    {"cpssDxChPortEeeLpiRequestEnableSet",
        &wrCpssDxChPortEeeLpiRequestEnableSet,
        3, 0},
    {"cpssDxChPortEeeLpiRequestEnableGet",
        &wrCpssDxChPortEeeLpiRequestEnableGet,
        2, 0},

    {"cpssDxChPortEeeLpiManualModeSet",
        &wrCpssDxChPortEeeLpiManualModeSet,
        3, 0},
    {"cpssDxChPortEeeLpiManualModeGet",
        &wrCpssDxChPortEeeLpiManualModeGet,
        2, 0},

    {"cpssDxChPortEeeLpiTimeLimitsSet",
        &wrCpssDxChPortEeeLpiTimeLimitsSet,
        5, 0},
    {"cpssDxChPortEeeLpiTimeLimitsGet",
        &wrCpssDxChPortEeeLpiTimeLimitsGet,
        2, 0},

    /**********/
    /* tables */
    /**********/
    /* table : cpssDxChPortEeeLpiStatus */
            {"cpssDxChPortEeeLpiStatusGetFirst",
                &wrCpssDxChPortEeeLpiStatusGetFirst,
                1, /*dev*/
                0},/*portNum,macTxPathLpi,macTxPathLpWait,macRxPathLpi,pcsTxPathLpi,pcsRxPathLpi*/

            {"cpssDxChPortEeeLpiStatusGetNext",
                &wrCpssDxChPortEeeLpiStatusGetNext,
                1, /*dev*/
                0}/*portNum,macTxPathLpi,macTxPathLpWait,macRxPathLpi,pcsTxPathLpi,pcsRxPathLpi*/

};

#define numCommands (sizeof(dbCommands) / sizeof(CMD_COMMAND))

/*******************************************************************************
* cmdLibInitCpssDxChPortEee
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
GT_STATUS cmdLibInitCpssDxChPortEee
(
    GT_VOID
)
{
    return cmdInitLibrary(dbCommands, numCommands);
}

