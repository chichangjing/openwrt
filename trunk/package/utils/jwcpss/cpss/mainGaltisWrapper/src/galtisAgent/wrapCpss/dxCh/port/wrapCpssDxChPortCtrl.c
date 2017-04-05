/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* wrapPhyCpssDxCh.c
*
* DESCRIPTION:
*       Wrapper functions for Phy cpss.dxCh functions
*
* FILE REVISION NUMBER:
*       $Revision: 103 $
*
*******************************************************************************/

/* Common galtis includes */
#include <galtisAgent/wrapUtil/cmdCpssPresteraUtils.h>
#include <cmdShell/cmdDb/cmdBase.h>
#include <cmdShell/common/cmdWrapUtils.h>

/* Feature specific includes. */
#include <cpss/dxCh/dxChxGen/port/cpssDxChPortCtrl.h>
#include <cpss/dxCh/dxChxGen/config/private/prvCpssDxChInfo.h>

/*******************************************************************************
* cpssDxChPortSerdesManualTxConfigSet
*
* DESCRIPTION:
*   Configure specific parameters of serdes TX in HW.
*
* APPLICABLE DEVICES:
*   Lion2; Lion3.
*
* NOT APPLICABLE DEVICES:
*   DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* INPUTS:
*   devNum - physical device number
*   portNum - physical port number
*   laneNum - number of SERDES lane of port (0-> lane 0,...,3 -> lane 3 etc.)
*   serdesTxCfgPtr  - serdes Tx parameters
*
* OUTPUTS:
*   None.
*
* RETURNS:
*   GT_OK                       - on success.
*   GT_BAD_PARAM                - on bad parameters
*   GT_NOT_APPLICABLE_DEVICE    - on not applicable device
*   GT_NOT_INITIALIZED          - if serdes was not intialized
*   GT_OUT_OF_RANGE             - parameter out of range
*
* COMMENTS: 
*   Pay attention: every serdes power up restore CPSS default configurations,
*   so this API should be egaged by application after every serdes power up
*   or use cpssDxChPortSerdesTuningSet.
*
*******************************************************************************/
CMD_STATUS wrCpssDxChPortSerdesManualTxConfigSet
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
    GT_U32                  laneNum;
    CPSS_DXCH_PORT_SERDES_TX_CONFIG_STC  serdesCfg;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum          = (GT_U8)inArgs[0];
    portNum         = (GT_U8)inArgs[1];
    laneNum         = (GT_U32)inArgs[2];

    serdesCfg.txAmp = (GT_U32)inArgs[3];
    serdesCfg.txAmpAdjEn = (GT_BOOL)inArgs[4];
    serdesCfg.emph0 = (GT_U32)inArgs[5];
    serdesCfg.emph1 = (GT_U32)inArgs[6];
    serdesCfg.txAmpShft = (GT_BOOL)inArgs[7];
    
    /* Override Device and Port */
    CONVERT_DEV_PORT_MAC(devNum, portNum);

    /* call cpss api function */
    result = cpssDxChPortSerdesManualTxConfigSet(devNum, portNum, laneNum, 
                                                 &serdesCfg);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChPortSerdesManualRxConfigSet
*
* DESCRIPTION:
*   Configure specific parameters of serdes RX in HW.
*
* APPLICABLE DEVICES:
*   Lion2; Lion3.
*
* NOT APPLICABLE DEVICES:
*   DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* INPUTS:
*   devNum - physical device number
*   portNum - physical port number
*   laneNum - number of SERDES lane of port (0-> lane 0,...,3 -> lane 3 etc.)
*   serdesRxCfgPtr  - serdes Rx parameters
*
* OUTPUTS:
*   None.
*
* RETURNS:
*   GT_OK                       - on success.
*   GT_BAD_PARAM                - on bad parameters
*   GT_NOT_APPLICABLE_DEVICE    - on not applicable device
*   GT_NOT_INITIALIZED          - if serdes was not intialized
*   GT_OUT_OF_RANGE             - parameter out of range
*
* COMMENTS: 
*   Pay attention: every serdes power up restore CPSS default configurations,
*   so this API should be egaged by application after every serdes power up
*   or use cpssDxChPortSerdesTuningSet.
*
*******************************************************************************/
CMD_STATUS wrCpssDxChPortSerdesManualRxConfigSet
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
    GT_U32                  laneNum;
    CPSS_DXCH_PORT_SERDES_RX_CONFIG_STC  serdesCfg;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum          = (GT_U8)inArgs[0];
    portNum         = (GT_U8)inArgs[1];
    laneNum         = (GT_U32)inArgs[2];

    serdesCfg.sqlch = (GT_U32)inArgs[3];
    serdesCfg.ffeRes = (GT_U32)inArgs[4];
    serdesCfg.ffeCap = (GT_U32)inArgs[5];
    /* inArgs[6] - dfeEn obsolete */
    serdesCfg.align90 = (GT_U32)inArgs[7];
    
    /* Override Device and Port */
    CONVERT_DEV_PORT_MAC(devNum, portNum);

    /* call cpss api function */
    result = cpssDxChPortSerdesManualRxConfigSet(devNum, portNum, laneNum, 
                                                 &serdesCfg);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/******************************************************************************
* cpssDxChPortFecModeSet
*
* DESCRIPTION:
*       Configure Forward error correction (FEC) mode of port
*
* APPLICABLE DEVICES:
*        Lion2; Lion3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* INPUTS:
*       devNum    - physical device number
*       portNum   - physical port number
*       mode      - Forward error correction mode
*
* OUTPUTS:
*       None.
*
* RETURNS:
*     GT_OK            - on success.
*     GT_BAD_PARAM     - on bad parameters
*     GT_NOT_INITIALIZED    - if port not configured
*     GT_NOT_SUPPORTED - if FEC not supported on interface mode defined now on port
*     GT_FAIL          - on error
*     GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
******************************************************************************/
CMD_STATUS wrCpssDxChPortFecModeSet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    GT_U8 portNum;
    CPSS_DXCH_PORT_FEC_MODE_ENT mode;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];
    portNum = (GT_U8)inArgs[1];
    mode = (CPSS_DXCH_PORT_FEC_MODE_ENT)inArgs[2];

    /* Override Device and Port */
    CONVERT_DEV_PORT_MAC(devNum, portNum);

    /* call cpss api function */
    result = cpssDxChPortFecModeSet(devNum, portNum, mode);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/******************************************************************************
* cpssDxChPortFecModeGet
*
* DESCRIPTION:
*       Read current Forward error correction (FEC) mode of port
*
* APPLICABLE DEVICES:
*        Lion2; Lion3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* INPUTS:
*       devNum    - physical device number
*       portNum   - physical port number
*
* OUTPUTS:
*       modePtr   - current Forward error correction mode
*
* RETURNS:
*     GT_OK            - on success.
*     GT_BAD_PARAM     - on bad parameters
*     GT_NOT_INITIALIZED    - if port not configured
*     GT_BAD_PTR       - modePtr == NULL
*     GT_NOT_SUPPORTED - if FEC not supported on interface mode defined now on port
*     GT_FAIL          - on error
*     GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
******************************************************************************/
CMD_STATUS wrCpssDxChPortFecModeGet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                   result = GT_OK;
    GT_U8                       devNum;
    GT_U8                       portNum;
    CPSS_DXCH_PORT_FEC_MODE_ENT mode = CPSS_DXCH_PORT_FEC_MODE_DISABLED_E;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    portNum = (GT_U8)inArgs[1];

    /* Override Device and Port */
    CONVERT_DEV_PORT_MAC(devNum, portNum);

    /* call cpss api function */
    result = cpssDxChPortFecModeGet(devNum, portNum, &mode);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", mode);
    return CMD_OK;
}

/******************************************************************************
* cpssDxChPortPcsLoopbackModeSet
*
* DESCRIPTION:
*       Configure loopback of specific type on PCS.
*
* APPLICABLE DEVICES:
*        Lion2; Lion3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* INPUTS:
*       devNum    - physical device number
*       portNum   - physical port number
*       mode      - define loopback type or no loopback
*
* OUTPUTS:
*       None.
*
* RETURNS:
*     GT_OK            - on success.
*     GT_BAD_PARAM     - on bad parameters
*     GT_FAIL          - on error
*     GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
******************************************************************************/
CMD_STATUS wrCpssDxChPortPcsLoopbackModeSet
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
    CPSS_DXCH_PORT_PCS_LOOPBACK_MODE_ENT    mode;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum  = (GT_U8)inArgs[0];
    portNum = (GT_U8)inArgs[1];
    mode = (CPSS_DXCH_PORT_PCS_LOOPBACK_MODE_ENT)inArgs[2];

    /* Override Device and Port */
    CONVERT_DEV_PORT_MAC(devNum, portNum);

    /* call cpss api function */
    result = cpssDxChPortPcsLoopbackModeSet(devNum, portNum, mode);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/******************************************************************************
* cpssDxChPortPcsLoopbackModeGet
*
* DESCRIPTION:
*       Configure loopback of specific type on PCS.
*
* APPLICABLE DEVICES:
*        Lion2; Lion3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* INPUTS:
*       devNum    - physical device number
*       portNum   - physical port number
*
* OUTPUTS:
*       modePtr - current loopback type or no loopback
*
* RETURNS:
*     GT_OK            - on success.
*     GT_BAD_PARAM     - on bad parameters
*     GT_BAD_PTR       - modePtr == NULL
*     GT_FAIL          - on error
*     GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
******************************************************************************/
CMD_STATUS wrCpssDxChPortPcsLoopbackModeGet
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
    CPSS_DXCH_PORT_PCS_LOOPBACK_MODE_ENT mode;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum  = (GT_U8)inArgs[0];
    portNum = (GT_U8)inArgs[1];

    /* Override Device and Port */
    CONVERT_DEV_PORT_MAC(devNum, portNum);

    /* call cpss api function */
    result = cpssDxChPortPcsLoopbackModeGet(devNum, portNum, &mode);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", mode);

    return CMD_OK;
}


/*******************************************************************************
* cpssDxChPortExtendedModeEnableSet
*
* DESCRIPTION:
*       Define which GE and XG MAC ports 9 and 11 of every mini-GOP will use -
*           from local mini-GOP or extended from other mini-GOP
*
* APPLICABLE DEVICES:
*        Lion2; Lion3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* INPUTS:
*       devNum   - physical device number
*       portNum  - physical port number (not CPU port)
*       enable   - extended mode:
*                       GT_TRUE - use extended MAC;
*                       GT_FALSE - use local MAC;
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
*       For now if application interested to implement GE and XG modes of ports 9
*       and 11 of every mini-GOP over extended MAC's, it can call this function
*       at init stage once for port 9 and once for port 11 and enough
*******************************************************************************/
CMD_STATUS wrCpssDxChPortExtendedModeEnableSet
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
    GT_BOOL     enable;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum  = (GT_U8)inArgs[0];
    portNum = (GT_U8)inArgs[1];
    enable = (GT_BOOL)inArgs[2];

    /* Override Device and Port */
    CONVERT_DEV_PORT_MAC(devNum, portNum);

    /* call cpss api function */
    result = cpssDxChPortExtendedModeEnableSet(devNum, portNum, enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChPortExtendedModeEnableGet
*
* DESCRIPTION:
*       Read which GE and XG MAC ports 9 and 11 of every mini-GOP will use -
*           from local mini-GOP or extended from other mini-GOP
*
* APPLICABLE DEVICES:
*        Lion2; Lion3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* INPUTS:
*       devNum   - physical device number
*       portNum  - physical port number (not CPU port)
*
* OUTPUTS:
*       enablePtr - extended mode:
*                       GT_TRUE - use extended MAC;
*                       GT_FALSE - use local MAC;
*
* RETURNS:
*       GT_OK             - on success
*       GT_BAD_PARAM      - on wrong port number or device
*       GT_BAD_PTR        - enablePtr is NULL
*       GT_HW_ERROR       - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
CMD_STATUS wrCpssDxChPortExtendedModeEnableGet
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
    GT_BOOL     enable;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum  = (GT_U8)inArgs[0];
    portNum = (GT_U8)inArgs[1];

    /* Override Device and Port */
    CONVERT_DEV_PORT_MAC(devNum, portNum);

    /* call cpss api function */
    result = cpssDxChPortExtendedModeEnableGet(devNum, portNum, &enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", enable);

    return CMD_OK;
}

/******************************************************************************
* cpssDxChPortSerdesLaneTuningSet
*
* DESCRIPTION:
*       SerDes lane fine tuning values set.
*
* APPLICABLE DEVICES:
*        xCat; Lion; xCat2; Lion2; Lion3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3.
*
* INPUTS:
*       devNum    - physical device number
*       portGroupNum - number of port group (local core), not used for non-multi-core
*       laneNum   - number of SERDES lane where values in tuneValuesPtr appliable
*       serdesFrequency - serdes speed for which to save tuning values
*       tuneValuesPtr   - (ptr to) structure with tuned values
*
* OUTPUTS:
*       None.
*
* RETURNS:
*     GT_OK            - on success.
*     GT_BAD_PARAM     - on bad parameters
*     GT_OUT_OF_RANGE  - one of tune values is out of range
*     GT_FAIL          - on error
*     GT_BAD_PTR       - one of the parameters is NULL pointer
*     GT_NOT_SUPPORTED - HW does not support the requested operation
*     GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*     Set tuning parameters for lane in SW DB, then
*     cpssDxChPortSerdesPowerStatusSet or cpssDxChPortModeSpeedSet will write
*     them to HW.
*
******************************************************************************/
CMD_STATUS wrCpssDxChPortSerdesLaneTuningSet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS rc;
    GT_U8     devNum;
    GT_U32    portGroupNum;
    GT_U32    laneNum;
    CPSS_DXCH_PORT_SERDES_SPEED_ENT serdesFrequency;
    CPSS_DXCH_PORT_SERDES_TUNE_STC tuneValues = {0};

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum          = (GT_U8)inArgs[0];
    portGroupNum    = (GT_U32)inArgs[1];
    laneNum         = (GT_U32)inArgs[2];
    serdesFrequency = (CPSS_DXCH_PORT_SERDES_SPEED_ENT)inArgs[3];

    tuneValues.dfe      = (GT_U32)inFields[0];
    tuneValues.ffeC     = (GT_U32)inFields[1];
    tuneValues.ffeR     = (GT_U32)inFields[2];
    tuneValues.ffeS     = (GT_U32)inFields[3];
    tuneValues.sampler  = (GT_U32)inFields[4];
    tuneValues.sqlch    = (GT_U32)inFields[5];
    tuneValues.txAmp    = (GT_U32)inFields[6];
    tuneValues.txAmpAdj = (GT_U32)inFields[7];
    tuneValues.txEmphAmp = (GT_U32)inFields[8];
    tuneValues.txEmphEn = (GT_BOOL)inFields[9];

    if(PRV_CPSS_DXCH_LION2_B0_AND_ABOVE_CHECK_MAC(devNum))                
    {
        GT_U32  i;

        tuneValues.txEmph1 = (GT_U32)inFields[10];
        tuneValues.align90 = (GT_U32)inFields[11];
        tuneValues.txEmphEn1 = (GT_BOOL)inFields[12];
        tuneValues.txAmpShft = (GT_BOOL)inFields[13];

        for(i = 0; i < CPSS_DXCH_PORT_DFE_VALUES_ARRAY_SIZE_CNS; i++)
        {
            tuneValues.dfeValsArray[i] = (GT_U32)inFields[14+i];
        }
    }

    rc = cpssDxChPortSerdesLaneTuningSet(devNum,
                                         portGroupNum,
                                         laneNum,
                                         serdesFrequency,
                                         &tuneValues);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, rc, "");

    return CMD_OK;   
}

/******************************************************************************
* cpssDxChPortSerdesLaneTuningGet
*
* DESCRIPTION:
*       Get SerDes lane fine tuning values.
*
* APPLICABLE DEVICES:
*        xCat; Lion; xCat2; Lion2; Lion3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3.
*
* INPUTS:
*       devNum    - physical device number
*       portGroupNum - number of port group (local core), not used for non-multi-core
*       laneNum   - number of SERDES lane where values in tuneValuesPtr appliable
*       serdesFrequency - serdes speed for which to save tuning values
*
* OUTPUTS:
*       tuneValuesPtr   - (ptr to) structure with tuned values
*
* RETURNS:
*     GT_OK            - on success.
*     GT_BAD_PARAM     - on bad parameters
*     GT_OUT_OF_RANGE  - one of tune values is out of range
*     GT_FAIL          - on error
*     GT_BAD_PTR       - one of the parameters is NULL pointer
*     GT_NOT_SUPPORTED - HW does not support the requested operation
*     GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*     Get tuning parameters for lane from SW DB.
*
******************************************************************************/
CMD_STATUS wrCpssDxChPortSerdesLaneTuningGet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS rc;
    GT_U8     devNum;
    GT_U32    portGroupNum;
    GT_U32    laneNum;
    CPSS_DXCH_PORT_SERDES_SPEED_ENT serdesFrequency;
    CPSS_DXCH_PORT_SERDES_TUNE_STC tuneValues;
    GT_U32  i;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum          = (GT_U8)inArgs[0];
    portGroupNum    = (GT_U32)inArgs[1];
    laneNum         = (GT_U32)inArgs[2];
    serdesFrequency = (CPSS_DXCH_PORT_SERDES_SPEED_ENT)inArgs[3];

    rc = cpssDxChPortSerdesLaneTuningGet(devNum, portGroupNum, laneNum,
                                         serdesFrequency,
                                         &tuneValues);
    if (rc != GT_OK)
    {
        galtisOutput(outArgs, rc, "cpssDxChPortSerdesTuningGet");
        return CMD_OK;
    }

    inFields[0] = tuneValues.dfe;
    inFields[1] = tuneValues.ffeC;
    inFields[2] = tuneValues.ffeR;
    inFields[3] = tuneValues.ffeS;
    inFields[4] = tuneValues.sampler;
    inFields[5] = tuneValues.sqlch;
    inFields[6] = tuneValues.txAmp;
    inFields[7] = tuneValues.txAmpAdj;
    inFields[8] = tuneValues.txEmphAmp;
    inFields[9] = tuneValues.txEmphEn;

    if(PRV_CPSS_DXCH_LION2_B0_AND_ABOVE_CHECK_MAC(devNum))                
    {
        inFields[10] = tuneValues.txEmph1;
        inFields[11] = tuneValues.align90;
        inFields[12] = tuneValues.txEmphEn1;
        inFields[13] = tuneValues.txAmpShft;
    
        for(i = 0; i < CPSS_DXCH_PORT_DFE_VALUES_ARRAY_SIZE_CNS; i++)
        {
            inFields[14+i] = tuneValues.dfeValsArray[i];
        }
    }
    else
    {
        for(i = 10; i < 14+CPSS_DXCH_PORT_DFE_VALUES_ARRAY_SIZE_CNS; i++)
        {
            inFields[i] = 0;
        }
    }

    /* pack and output table fields */
    fieldOutput("%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d", inFields[0], inFields[1], inFields[2],
                    inFields[3], inFields[4], inFields[5], inFields[6], inFields[7], inFields[8],
                    inFields[9], inFields[10], inFields[11], inFields[12], inFields[13], inFields[14], inFields[15],
                    inFields[16], inFields[17], inFields[18], inFields[19]);
    galtisOutput(outArgs, rc, "%f");

    return CMD_OK;   
}


/*******************************************************************************
* cpssDxChPortRefClockSourceOverrideEnableSet
*
* DESCRIPTION:
*       Enables/disables reference clock source override for port create.
*
* APPLICABLE DEVICES:
*       xCat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum          - physical device number
*       portNum         - physical port number
*       overrideEnable  - override
*                           GT_TRUE - use preconfigured refClock source;
*                           GT_FALSE - use default refClock source;
*       refClockSource  - reference clock source.
*                         Not relevant when overrideEnable is false.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK             - on success
*       GT_BAD_PARAM      - on wrong port number, device, refClockSource
*       GT_HW_ERROR       - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None
*******************************************************************************/
CMD_STATUS wrCpssDxChPortRefClockSourceOverrideEnableSet
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
    GT_BOOL                         overrideEnable;
    CPSS_PORT_REF_CLOCK_SOURCE_ENT  refClockSource;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum  = (GT_U8)inArgs[0];
    portNum = (GT_PHYSICAL_PORT_NUM)inArgs[1];
    overrideEnable = (GT_BOOL)inArgs[2];
    refClockSource = (CPSS_PORT_REF_CLOCK_SOURCE_ENT)inArgs[3];

    /* Override Device and Port */
    CONVERT_DEV_PHYSICAL_PORT_MAC(devNum, portNum);

    /* call cpss api function */
    result = cpssDxChPortRefClockSourceOverrideEnableSet(devNum, portNum, overrideEnable, refClockSource);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;

}

/*******************************************************************************
* cpssDxChPortRefClockSourceOverrideEnableGet
*
* DESCRIPTION:
*       Gets status of reference clock source override for port create.
*
* APPLICABLE DEVICES:
*       xCat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum          - physical device number
*       portNum         - physical port number
*
* OUTPUTS:
*       overrideEnablePtr  - (pointer to) override enable status
*       refClockSourcePtr  - (pointer to) reference clock source.
*
* RETURNS:
*       GT_OK             - on success
*       GT_BAD_PARAM      - on wrong port number, device
*       GT_BAD_PTR        - on NULL pointer
*       GT_HW_ERROR       - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None
*******************************************************************************/
CMD_STATUS wrCpssDxChPortRefClockSourceOverrideEnableGet
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
    GT_BOOL                         overrideEnable;
    CPSS_PORT_REF_CLOCK_SOURCE_ENT  refClockSource;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum  = (GT_U8)inArgs[0];
    portNum = (GT_PHYSICAL_PORT_NUM)inArgs[1];

    /* Override Device and Port */
    CONVERT_DEV_PHYSICAL_PORT_MAC(devNum, portNum);

    /* call cpss api function */
    result = cpssDxChPortRefClockSourceOverrideEnableGet(devNum, portNum, &overrideEnable, &refClockSource);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d%d", overrideEnable, refClockSource);

    return CMD_OK;

}


/*******************************************************************************
* wrCpssDxChGetNext
*
* DESCRIPTION:
*
*   This function is called for ending table printing.
*
* APPLICABLE DEVICES:  All DxCh devices
*
* INPUTS:
*       None.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK        - on success
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChGetNext

(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)

{
    galtisOutput(outArgs, GT_OK, "%d", -1);
    return CMD_OK;

}


/******************************************************************************
* cpssDxChPortResourceTmBandwidthSet
*
* DESCRIPTION:
*       Define system bandwith through TM port. Following this configuration
*       TM port resources will be allocated.
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat3.
*
* INPUTS:
*       devNum          - physical device number
*       tmBandwidthMbps - system TM bandwidth
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK            - on success.
*       GT_BAD_PARAM     - on bad parameters
*       GT_FAIL          - on error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       Note: 
*           - API should not be called under traffic
*           - zero bandwidth will release resources of TM for non TM ports
******************************************************************************/
CMD_STATUS wrCpssDxChPortResourceTmBandwidthSet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS   result;
    GT_U8       devNum;
    GT_U32      tmBandwidthMbps;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum  = (GT_U8)inArgs[0];
    tmBandwidthMbps = (GT_U32)inArgs[1];

    /* call cpss api function */
    result = cpssDxChPortResourceTmBandwidthSet(devNum, tmBandwidthMbps);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}


/******************************************************************************
* cpssDxChPortResourceTmBandwidthGet
*
* DESCRIPTION:
*       Get system bandwith through TM port. Following this configuration
*       TM port resources will be allocated.
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat3.
*
* INPUTS:
*       devNum          - physical device number
*
* OUTPUTS:
*       tmBandwidthMbpsPtr - (pointer to) system TM bandwidth
*
* RETURNS:
*       GT_OK            - on success.
*       GT_BAD_PARAM     - on bad parameters
*       GT_FAIL          - on error
*       GT_BAD_PTR       - on NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
******************************************************************************/
CMD_STATUS wrCpssDxChPortResourceTmBandwidthGet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS   result;
    GT_U8       devNum;
    GT_U32      tmBandwidthMbps;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum  = (GT_U8)inArgs[0];
    tmBandwidthMbps = (GT_U32)inArgs[1];

    /* call cpss api function */
    result = cpssDxChPortResourceTmBandwidthGet(devNum, &tmBandwidthMbps);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", tmBandwidthMbps);

    return CMD_OK;

}

/**** database initialization **************************************/

static CMD_COMMAND dbCommands[] =
{
    {"cpssDxChPortSerdesLaneTuningEntrySet",
        &wrCpssDxChPortSerdesLaneTuningSet,
        4, 20},

    {"cpssDxChPortSerdesLaneTuningEntryGetFirst",
        &wrCpssDxChPortSerdesLaneTuningGet,
        4, 0},

    {"cpssDxChPortSerdesLaneTuningEntryGetNext",
        &wrCpssDxChGetNext,
        4, 0},

    {"cpssDxChPortFecModeSet",
        &wrCpssDxChPortFecModeSet,
        3, 0},
    
    {"cpssDxChPortFecModeGet",
        &wrCpssDxChPortFecModeGet,
        2, 0},
    
    {"cpssDxChPortSerdesManualTxConfigSet",
         &wrCpssDxChPortSerdesManualTxConfigSet,
         8, 0},

    {"cpssDxChPortSerdesManualRxConfigSet",
         &wrCpssDxChPortSerdesManualRxConfigSet,
         8, 0},

    {"cpssDxChPortPcsLoopbackModeSet",
        &wrCpssDxChPortPcsLoopbackModeSet,
        3, 0},

    {"cpssDxChPortPcsLoopbackModeGet",
        &wrCpssDxChPortPcsLoopbackModeGet,
        2, 0},

    {"cpssDxChPortExtendedModeEnableSet",
        &wrCpssDxChPortExtendedModeEnableSet,
        3, 0},

    {"cpssDxChPortExtendedModeEnableGet",
        &wrCpssDxChPortExtendedModeEnableGet,
        2, 0},

    {"cpssDxChPortRefClockSourceOverrideEnableSet",
        &wrCpssDxChPortRefClockSourceOverrideEnableSet,
        4, 0},

    {"cpssDxChPortRefClockSourceOverrideEnableGet",
        &wrCpssDxChPortRefClockSourceOverrideEnableGet,
        2, 0},

    {"cpssDxChPortResourceTmBandwidthSet",
        &wrCpssDxChPortResourceTmBandwidthSet,
        2, 0},

    {"cpssDxChPortResourceTmBandwidthGet",
        &wrCpssDxChPortResourceTmBandwidthGet,
        1, 0},

};

#define numCommands (sizeof(dbCommands) / sizeof(CMD_COMMAND))

/*******************************************************************************
* cmdLibInitCpssDxChPortCtrl
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
GT_STATUS cmdLibInitCpssDxChPortCtrl
(
    GT_VOID
)
{
    return cmdInitLibrary(dbCommands, numCommands);
}
