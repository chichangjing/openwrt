/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* wrapCpssGenPhyVct.c
*
* DESCRIPTION:
*       Wrapper functions for CPSS Marvell Virtual Cable Tester functionality.
*
* FILE REVISION NUMBER:
*       $Revision: 3 $
*
*******************************************************************************/

/* Common galtis includes */
#include <galtisAgent/wrapUtil/cmdCpssPresteraUtils.h>
#include <cmdShell/cmdDb/cmdBase.h>
#include <cmdShell/common/cmdWrapUtils.h>

/* Feature specific includes. */
#include <cpss/generic/phy/cpssGenPhyVct.h>

/*******************************************************************************
* cpssVctCableExtendedStatusGet
*
* DESCRIPTION:
*       This routine returns the extended VCT status per MDI pair/channel pair.
*
* APPLICABLE DEVICES:  All devices
*
* INPUTS:
*       devNum - physical device number
*       port   - physical port number
*
* OUTPUTS:
*       extendedCableStatus - extended virtual cable diagnostic status
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_SUPPORTED         - on non supported device
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       If PHY is CPSS_VCT_PHY_100M_E (FE) cableStatus will have only 2 pairs 
*       relevant. In this case:
*       swap - just pairSwap[0] relevant
*              vctExtendedCableStatus.pairSwap[1] = CPSS_VCT_NOT_APPLICABLE_E
*              vctExtendedCableStatus.pairSwap[2] = CPSS_VCT_NOT_APPLICABLE_E
*              vctExtendedCableStatus.pairSwap[3] = CPSS_VCT_NOT_APPLICABLE_E
*       polarity - just pairPolarity[0] relevant
*              vctExtendedCableStatus.pairPolarity[1] =CPSS_VCT_NOT_APPLICABLE_E
*              vctExtendedCableStatus.pairPolarity[2] =CPSS_VCT_NOT_APPLICABLE_E
*              vctExtendedCableStatus.pairPolarity[3] =CPSS_VCT_NOT_APPLICABLE_E
*       skew - not relevant
*              vctExtendedCableStatus.skew[0,1,2,3].IsValid = GT_FALSE
*       accurateCableLen - not relevant
*              vctExtendedCableStatus.accurateCableLen.isValid = GT_FALSE
*       twoPairDownShift - not relevant
*              extendedCableStatus->twoPairDownShift =
*                                           CPSS_VCT_NOT_APPLICABLE_DOWNSHIFT_E;
*       The API doesn't perform VCT test, but retrieves extended info regarding
*       the above features, which has collected by: cpssVctCableStatusGet API.
*******************************************************************************/
static CMD_STATUS wrCpssVctCableExtendedStatusGet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 dev;
    GT_U8 port;
    CPSS_VCT_CABLE_EXTENDED_STATUS_STC extendedCableStatus;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    cmdOsMemSet(&extendedCableStatus, 0, sizeof(extendedCableStatus));

    dev = (GT_U8)inArgs[0];
    port = (GT_U8)inArgs[1];

    result = cpssVctCableExtendedStatusGet(dev, port, &extendedCableStatus);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d",
    extendedCableStatus.accurateCableLen.isValid[0],
    extendedCableStatus.accurateCableLen.cableLen[0],
    extendedCableStatus.accurateCableLen.isValid[1],
    extendedCableStatus.accurateCableLen.cableLen[1],
    extendedCableStatus.accurateCableLen.isValid[2],
    extendedCableStatus.accurateCableLen.cableLen[2],
    extendedCableStatus.accurateCableLen.isValid[3],
    extendedCableStatus.accurateCableLen.cableLen[3],
    extendedCableStatus.twoPairDownShift,
    extendedCableStatus.vctExtendedCableStatus.isValid,
    extendedCableStatus.vctExtendedCableStatus.pairSwap[0],
    extendedCableStatus.vctExtendedCableStatus.pairSwap[1],
    extendedCableStatus.vctExtendedCableStatus.pairPolarity[0],
    extendedCableStatus.vctExtendedCableStatus.pairPolarity[1],
    extendedCableStatus.vctExtendedCableStatus.pairPolarity[2],
    extendedCableStatus.vctExtendedCableStatus.pairPolarity[3],
    extendedCableStatus.vctExtendedCableStatus.pairSkew.isValid,
    extendedCableStatus.vctExtendedCableStatus.pairSkew.skew[0],
    extendedCableStatus.vctExtendedCableStatus.pairSkew.skew[1],
    extendedCableStatus.vctExtendedCableStatus.pairSkew.skew[2],
    extendedCableStatus.vctExtendedCableStatus.pairSkew.skew[3]
    );

    return CMD_OK;
}

/*******************************************************************************
* cpssVctCableStatusGet
*
* DESCRIPTION:
*       This routine asserts / aborts the state-machine based virtual cable test
*       (VCT) for the requested port. When test completes, it returns the the
*       status per MDI pair, otherwize it will return current test status.
*
* APPLICABLE DEVICES:  All devices
*
* INPUTS:
*       dev       - physical device number
*       port      - physical port number
*       vctAction - The VCT action to perform on this port.
*
* OUTPUTS:
*       cableStatus - virtual cable diagnostic status
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_READY     - when action is CPSS_VCT_GET_RES_E and the test is not 
*                          completed yet.
*       GT_NOT_SUPPORTED         - on non supported device
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       Scenario: 
*       1)First call with CPSS_VCT_START_E to begin the test
*       2)Call the function with CPSS_VCT_GET_RES_E  and wait till GT_OK as 
*         relult.
*         If GT_NOT_READY is returned, call the CPSS_VCT_GET_RES_E again after a  
*         while in order to let the test be completed.
*
*******************************************************************************/
static CMD_STATUS wrCpssVctCableStatusGet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 dev;
    GT_U8 port;
    CPSS_VCT_ACTION_ENT       vctAction;
    CPSS_VCT_CABLE_STATUS_STC cableStatus;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    cmdOsMemSet(&cableStatus, 0, sizeof(cableStatus));

    dev = (GT_U8)inArgs[0];
    port = (GT_U8)inArgs[1];
    vctAction = (CPSS_VCT_ACTION_ENT)inArgs[2];

    result = cpssVctCableStatusGet(dev, port, vctAction, &cableStatus);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d%d%d%d%d%d%d%d%d%d",
    cableStatus.cableStatus[0].testStatus,
    cableStatus.cableStatus[0].errCableLen,
    cableStatus.cableStatus[1].testStatus,
    cableStatus.cableStatus[1].errCableLen,
    cableStatus.cableStatus[2].testStatus,
    cableStatus.cableStatus[2].errCableLen,
    cableStatus.cableStatus[3].testStatus,
    cableStatus.cableStatus[3].errCableLen,
    cableStatus.normalCableLen,
    cableStatus.phyType);

    return CMD_OK;
}

/*******************************************************************************
* cpssVctSetCableDiagConfig
*
* DESCRIPTION:
*       This routine configures VCT diagnostic parameters per port:
*        - cable's length: greater or less then 10 meters.
* APPLICABLE DEVICES:  All devices
* INPUTS:
*       dev       - device number
*       port      - port number
*       configPtr - configuration parameters
*
* OUTPUTS:
*       None
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_NOT_SUPPORTED         - on non supported device
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssVctSetCableDiagConfig
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 dev;
    GT_U8 port;
    CPSS_VCT_CONFIG_STC config;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    dev = (GT_U8)inArgs[0];
    port = (GT_U8)inArgs[1];
    config.length = (GT_BOOL)inArgs[2];

    /* call tapi api function */
    result = cpssVctSetCableDiagConfig(dev, port, &config);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssVctLengthOffsetSet
*
* DESCRIPTION:
*       This function sets the offset used in VCT length calcualtions per port.
*
* APPLICABLE DEVICES:  All devices
*
* INPUTS:
*       dev       - device number
*       port      - port number
*       offset    - offset in milimeters.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_NOT_SUPPORTED         - on non supported device
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       According to the PHYs' release notes the tuned formula to calculate 
*       length in VCT is of the form:
*       A * hwDist - B + offset
*       where:
*       A & B  - constants published in the PHYs' release notes.
*       hwDist - the distance value from HW registers to the fault.
*       offset - fine tuning required due to different "MDI traces" on 
*                different boards.
*       This function should be called after cpssDxChHwPpPhase1Init() and 
*       before runing the VCT.
*
*******************************************************************************/
static CMD_STATUS wrCpssVctLengthOffsetSet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 dev;
    GT_U8 port;
    GT_32 offset;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    dev = (GT_U8)inArgs[0];
    port = (GT_U8)inArgs[1];
    offset = (GT_32)inArgs[2];

    /* call tapi api function */
    result = cpssVctLengthOffsetSet(dev, port, offset);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssVctLengthOffsetGet
*
* DESCRIPTION:
*       This function gets the offset used in VCT length calcualtions per port.
*
* APPLICABLE DEVICES:  All devices
*
* INPUTS:
*       dev       - device number
*       port      - port number
*
* OUTPUTS:
*       offsetPtr - (pointer to)offset in milimeters
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PTR               - on NULL pointer
*       GT_BAD_PARAM             - one of the parameters value is wrong
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       According to the PHYs' release notes the tuned formula to calculate 
*       length in VCT is of the form:
*       A * hwDist - B + offset
*       where:
*       A & B  - constants published in the PHYs' release notes.
*       hwDist - the distance value from HW registers to the fault.
*       offset - fine tuning required due to different "MDI traces" on 
*                different boards.
*
*******************************************************************************/
static CMD_STATUS wrCpssVctLengthOffsetGet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 dev;
    GT_U8 port;
    GT_32 offset;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    dev = (GT_U8)inArgs[0];
    port = (GT_U8)inArgs[1];
    
    result = cpssVctLengthOffsetGet(dev, port, &offset);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", offset);

    return CMD_OK;
}

/**** database initialization **************************************/

static CMD_COMMAND dbCommands[] =
{
    {"cpssVctCableExtendedStatusGet",
        &wrCpssVctCableExtendedStatusGet,
        2, 0},

    {"cpssVctCableStatusGet",
        &wrCpssVctCableStatusGet,
        3, 0},

    {"cpssVctSetCableDiagConfig",
        &wrCpssVctSetCableDiagConfig,
        3, 0},

    {"cpssVctLengthOffsetSet",
        &wrCpssVctLengthOffsetSet,
        3, 0},

    {"cpssVctLengthOffsetGet",
        &wrCpssVctLengthOffsetGet,
        2, 0}
};


#define numCommands (sizeof(dbCommands) / sizeof(CMD_COMMAND))



/*******************************************************************************
* cmdLibInitCpssGenPhyVct
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
GT_STATUS cmdLibInitCpssGenPhyVct
(
    GT_VOID
)
{
    return cmdInitLibrary(dbCommands, numCommands);
}

