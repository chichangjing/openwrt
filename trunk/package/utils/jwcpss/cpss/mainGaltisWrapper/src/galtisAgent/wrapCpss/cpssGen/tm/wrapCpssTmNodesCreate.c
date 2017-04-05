/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* wrapCpssTmNodesCreate.c
*
* DESCRIPTION:
*       Wrapper functions for TM nodes creation APIs.
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
#include <cpss/generic/tm/cpssTmNodesCreate.h>
#include <cpss/generic/tm/cpssTmPublicDefs.h>
#include <cpss/generic/tm/cpssTmNodesRead.h>


/*******************************************************************************
* cpssTmPortCreate
*
* DESCRIPTION:
*       Create Port and download its parameters to HW.
*
* APPLICABLE DEVICES:
*       Bobcat2.
*
* NOT APPLICABLE DEVICES:
*       xCat; Lion; xCat2; DxCh1; DxCh1_Diamond; DxCh2; DxCh3; Lion2; Lion3; Puma2; Puma3; ExMx.
*
* INPUTS:
*       devNum            - Device number.
*       portInd           - Port index.
*       portSpeed         - Port physical bandwidth.
*       paramsPtr         - Port parameters structure pointer.
*       cNodesNum         - Number of C-nodes under port.
*       bNodesNum         - Number of B-nodes under port.
*       aNodesNum         - Number of A-nodes under port.
*       queuesNum         - Number of Queues under port.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_FAIL                  - on hardware error.
*       GT_BAD_PARAM             - on wrong device number or wrong parameter value.
*       GT_OUT_OF_RANGE          - on parameter value out of range.
*       GT_NO_RESOURCE           - on memory allocation fail.
*       GT_ALREADY_EXIST         - on portInd already in use.
*       GT_BAD_SIZE              - on too large port's min/max token.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device.
*
* COMMENTS:
*       1. To indicate that no shaping is needed for port, set
*       cirBw to CPSS_TM_INVAL_CNS, in this case other shaping
*       parameters will not be considered.
*       2. Valid number of nodes per level must be multiple of number nodes
*       on upper level (parent nodes) in case of equal distribution. And
*       in case of unequal distribution all parent nodes except the last
*       one must have the same children range and the last parent node -
*       number of children less that the range. In case of not valid
*       number will be returned GT_BAD_PARAM error code.
*       3. If port will be used for transparent queues, set the c/b/aNodesNum = 1.
*******************************************************************************/
static CMD_STATUS wrCpssTmPortCreate
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8                        devNum;
    GT_U32                       portInd;
    CPSS_TM_PORT_BW_ENT          portSpeed;
    CPSS_TM_PORT_PARAMS_STC      params;
    GT_U32                       cNodesNum;
    GT_U32                       bNodesNum;
    GT_U32                       aNodesNum;
    GT_U32                       queuesNum;
    GT_U32                       ii;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* init with zeros */
    cmdOsMemSet(&params, 0, sizeof(params));

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    portInd = (GT_U32)inArgs[1];
    portSpeed = (CPSS_TM_PORT_BW_ENT)inArgs[2];
    params.cirBw = (GT_32)inArgs[3];
    params.eirBw = (GT_32)inArgs[4];
    params.cbs = (GT_32)inArgs[5];
    params.ebs = (GT_32)inArgs[6];

    for(ii=0; ii<8; ii++) {
        params.quantumArr[ii] = (GT_32)inArgs[7+ii];
    }
    for(ii=0; ii<8; ii++) {
        params.schdModeArr[ii] = (GT_32)inArgs[15+ii];
    }

    params.dropProfileInd = (GT_32)inArgs[23];
    cNodesNum = (GT_U32)inArgs[24];
    bNodesNum = (GT_U32)inArgs[25];
    aNodesNum = (GT_U32)inArgs[26];
    queuesNum = (GT_U32)inArgs[27];

    params.eligiblePrioFuncId = (CPSS_TM_ELIG_FUNC_NODE_ENT)inArgs[28];

    /* call cpss api function */
    result = cpssTmPortCreate(devNum, portInd, portSpeed, &params, cNodesNum, bNodesNum, aNodesNum, queuesNum);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}


/*******************************************************************************
* cpssTmAsymPortCreate
*
* DESCRIPTION:
*       Create Port with assymetric sub-tree and download its parameters to HW.
*
* APPLICABLE DEVICES:
*       Bobcat2.
*
* NOT APPLICABLE DEVICES:
*       xCat; Lion; xCat2; DxCh1; DxCh1_Diamond; DxCh2; DxCh3; Lion2; Lion3; Puma2; Puma3; ExMx.
*
* INPUTS:
*       devNum            - Device number.
*       portInd           - Port index.
*       portSpeed         - Port physical bandwidth.
*       paramsPtr         - Port parameters structure pointer.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_FAIL                  - on hardware error.
*       GT_BAD_PARAM             - on wrong device number or wrong parameter value.
*       GT_OUT_OF_RANGE          - on parameter value out of range.
*       GT_NO_RESOURCE           - on memory allocation fail.
*       GT_ALREADY_EXIST         - on portInd already in use.
*       GT_BAD_SIZE              - on too large port's min/max token.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device.
*
* COMMENTS:
*       To indicate that no shaping is needed for port, set
*       cirBw to CPSS_TM_INVAL_CNS, in this case other shaping
*       parameters will not be considered.
*******************************************************************************/
static CMD_STATUS wrCpssTmAsymPortCreate
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8                        devNum;
    GT_U32                       portInd;
    CPSS_TM_PORT_BW_ENT          portSpeed;
    CPSS_TM_PORT_PARAMS_STC      params;
    GT_U32                       ii;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* init with zeros */
    cmdOsMemSet(&params, 0, sizeof(params));

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    portInd = (GT_U32)inArgs[1];
    portSpeed = (CPSS_TM_PORT_BW_ENT)inArgs[2];

    params.cirBw = (GT_32)inArgs[3];
    params.eirBw = (GT_32)inArgs[4];
    params.cbs = (GT_32)inArgs[5];
    params.ebs = (GT_32)inArgs[6];
    for(ii=0; ii<8; ii++) {
        params.quantumArr[ii] = (GT_32)inArgs[7+ii];
    }
    for(ii=0; ii<8; ii++) {
        params.schdModeArr[ii] = (GT_32)inArgs[15+ii];
    }

    params.dropProfileInd = (GT_32)inArgs[23];
    params.eligiblePrioFuncId = (CPSS_TM_ELIG_FUNC_NODE_ENT)inArgs[24];
    params.numOfChildren = (GT_32)inArgs[25];

    /* call cpss api function */
    result = cpssTmAsymPortCreate(devNum, portInd, portSpeed, &params);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}


/*******************************************************************************
* cpssTmPortDropPerCosConfig
*
* DESCRIPTION:
*       Configure Port's Drop per Cos and download its parameters to HW.
*
* APPLICABLE DEVICES:
*       Bobcat2.
*
* NOT APPLICABLE DEVICES:
*       xCat; Lion; xCat2; DxCh1; DxCh1_Diamond; DxCh2; DxCh3; Lion2; Lion3; Puma2; Puma3; ExMx.
*
* INPUTS:
*       devNum            - Device number.
*       portInd           - Port index.
*       paramsPtr         - Port Drop per Cos parameters structure pointer.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_FAIL                  - on hardware error.
*       GT_BAD_PARAM             - on wrong device number or wrong parameter value.
*       GT_OUT_OF_RANGE          - on parameter value out of range.
*       GT_ALREADY_EXIST         - on portInd already in use.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device.
*
* COMMENTS:
*       None.
*******************************************************************************/
static CMD_STATUS wrCpssTmPortDropPerCosConfig
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8                          devNum;
    GT_U32                         portInd;
    CPSS_TM_PORT_DROP_PER_COS_STC  params;
    GT_U32                         ii;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* init with zeros */
    cmdOsMemSet(&params, 0, sizeof(params));

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    portInd = (GT_U32)inArgs[1];
    params.dropCosMap = (GT_32)inArgs[2];
    for(ii=0; ii<8; ii++)
        params.dropProfileIndArr[ii] = (GT_32)inArgs[3+ii];

    /* call cpss api function */
    result = cpssTmPortDropPerCosConfig(devNum, portInd, &params);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}


/*******************************************************************************
* cpssTmQueueToPortCreate
*
* DESCRIPTION:
*       Create path from Queue to Port.
*
* APPLICABLE DEVICES:
*       Bobcat2.
*
* NOT APPLICABLE DEVICES:
*       xCat; Lion; xCat2; DxCh1; DxCh1_Diamond; DxCh2; DxCh3; Lion2; Lion3; Puma2; Puma3; ExMx.
*
* INPUTS:
*       devNum            - Device number.
*       portInd           - Port index.
*       qParamsPtr        - Queue parameters structure pointer.
*       aParamsPtr        - A-Node parameters structure pointer.
*       bParamsPtr        - B-Node parameters structure pointer.
*       cParamsPtr        - C-Node parameters structure pointer.
*
* OUTPUTS:
*       queueIndPtr       - (pointer to) The created Queue index (APPLICABLE RANGES: 0..65535).
*       aNodeIndPtr       - (pointer to) The created A-Node index (APPLICABLE RANGES: 0..16383).
*       bNodeIndPtr       - (pointer to) The created B-Node index (APPLICABLE RANGES: 0..4095).
*       cNodeIndPtr       - (pointer to) The created C-Node index (APPLICABLE RANGES: 0..511).
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_FAIL                  - on hardware error.
*       GT_BAD_PARAM             - on wrong device number or wrong parameter value.
*       GT_OUT_OF_RANGE          - on parameter value out of range.
*       GT_NO_RESOURCE           - on memory allocation fail.
*       GT_BAD_STATE             - on portInd not in use.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device.
*
* COMMENTS:
*       To indicate that no shaping is needed to the queue/node, set
*       shapingProfilePtr to CPSS_TM_INVAL_CNS.
*******************************************************************************/
static CMD_STATUS wrCpssTmQueueToPortCreate
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8                       devNum;
    GT_U32                      portInd;
    CPSS_TM_QUEUE_PARAMS_STC    qParams;
    CPSS_TM_A_NODE_PARAMS_STC   aParams;
    CPSS_TM_B_NODE_PARAMS_STC   bParams;
    CPSS_TM_C_NODE_PARAMS_STC   cParams;
    GT_U32                      queueInd;
    GT_U32                      aNodeInd;
    GT_U32                      bNodeInd;
    GT_U32                      cNodeInd;
    GT_U32                         ii;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* init with zeros */
    cmdOsMemSet(&qParams, 0, sizeof(qParams));
    cmdOsMemSet(&aParams, 0, sizeof(aParams));
    cmdOsMemSet(&bParams, 0, sizeof(bParams));
    cmdOsMemSet(&cParams, 0, sizeof(cParams));

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    portInd = (GT_U32)inArgs[1];

    /* Queue structure */
    qParams.shapingProfilePtr = (GT_32)inArgs[2];
    qParams.quantum = (GT_32)inArgs[3];
    qParams.dropProfileInd = (GT_32)inArgs[4];

    /* A-Node structure */
    aParams.shapingProfilePtr = (GT_32)inArgs[5];
    aParams.quantum = (GT_32)inArgs[6];
    for(ii=0; ii<8; ii++) {
        aParams.schdModeArr[ii] = (GT_32)inArgs[7+ii];
    }
    aParams.dropProfileInd = (GT_32)inArgs[15];

    /* B-Node structure */
    bParams.shapingProfilePtr = (GT_32)inArgs[16];
    bParams.quantum = (GT_32)inArgs[17];
    for(ii=0; ii<8; ii++) {
        bParams.schdModeArr[ii] = (GT_32)inArgs[18+ii];
    }
    bParams.dropProfileInd = (GT_32)inArgs[26];


    /* C-Node structure */
    cParams.shapingProfilePtr = (GT_32)inArgs[27];
    cParams.quantum = (GT_32)inArgs[28];
    for(ii=0; ii<8; ii++) {
        cParams.schdModeArr[ii] = (GT_32)inArgs[29+ii];
    }
    cParams.dropCosMap = (GT_32)inArgs[37];
    for(ii=0; ii<8; ii++) {
        cParams.dropProfileIndArr[ii] = (GT_32)inArgs[38+ii];
    }

    qParams.eligiblePrioFuncId = (CPSS_TM_ELIG_FUNC_QUEUE_ENT)inArgs[46];
    aParams.eligiblePrioFuncId = (CPSS_TM_ELIG_FUNC_NODE_ENT)inArgs[47];
    bParams.eligiblePrioFuncId = (CPSS_TM_ELIG_FUNC_NODE_ENT)inArgs[48];
    cParams.eligiblePrioFuncId = (CPSS_TM_ELIG_FUNC_NODE_ENT)inArgs[49];

    aParams.numOfChildren = (GT_U32)inArgs[50];
    bParams.numOfChildren = (GT_U32)inArgs[51];
    cParams.numOfChildren = (GT_U32)inArgs[52];


    /* call cpss api function */
    result = cpssTmQueueToPortCreate(devNum, portInd, &qParams, &aParams, &bParams, &cParams,
                                     &queueInd, &aNodeInd, &bNodeInd, &cNodeInd);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d%d%d%d", queueInd, aNodeInd, bNodeInd, cNodeInd);

    return CMD_OK;
}

/*******************************************************************************
* cpssTmTransQueueToPortCreate
*
* DESCRIPTION:
*       Create transparent path from Queue to Port.
*
* APPLICABLE DEVICES:
*       Bobcat2.
*
* NOT APPLICABLE DEVICES:
*       xCat; Lion; xCat2; DxCh1; DxCh1_Diamond; DxCh2; DxCh3; Lion2; Lion3; Puma2; Puma3; ExMx.
*
* INPUTS:
*       devNum            - Device number.
*       portInd           - Port index.
*       qParamsPtr        - Queue parameters structure pointer.
*
* OUTPUTS:
*       queueIndPtr       - (pointer to) The created Queue index (APPLICABLE RANGES: 0..65535).
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_FAIL                  - on hardware error.
*       GT_BAD_PARAM             - on wrong device number or wrong parameter value.
*       GT_OUT_OF_RANGE          - on parameter value out of range.
*       GT_NO_RESOURCE           - on memory allocation fail.
*       GT_BAD_STATE             - on portInd not in use.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device.
*
* COMMENTS:
*       1. This API introduces 'transparent' concept to TM nodes
*       structure that deals with Queues and Ports only. In this case no
*       configuration is needed for A,B,C level nodes, this nodes
*       are created automatically (one C-node, one B-node and one
*       A-node) and they are 'transparent' from the system point of
*       view. Transparent path can be created under symmetric port
*       only. To delete Queue from the structure - use
*       'cpssTmNodesCtlNodeDelete' API with level CPSS_TM_LEVEL_Q_E. To update queue
*       parameters - use 'cpssTmQueueUpdate' API. To delete Port from
*       the structure use 'cpssTmNodesCtlTransPortDelete' API. Applying any
*       other APIs on nodes underlying the port can cause unexpected
*       behavior of the system.
*       2. To indicate that no shaping is needed to the queue, set
*       shapingProfilePtr to CPSS_TM_INVAL_CNS.
*******************************************************************************/
static CMD_STATUS wrCpssTmTransQueueToPortCreate
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                   result;
    GT_U8                       devNum;
    GT_U32                      portInd;
    CPSS_TM_QUEUE_PARAMS_STC    qParams;
    GT_U32                      queueInd;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* init with zeros */
    cmdOsMemSet(&qParams, 0, sizeof(qParams));

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    portInd = (CPSS_TM_LEVEL_ENT)inArgs[1];
    qParams.shapingProfilePtr = (GT_U32)inArgs[2];
    qParams.quantum = (GT_U32)inArgs[3];
    qParams.dropProfileInd = (GT_U32)inArgs[4];
    qParams.eligiblePrioFuncId = (CPSS_TM_ELIG_FUNC_QUEUE_ENT)inArgs[5];


    /* call cpss api function */
    result = cpssTmTransQueueToPortCreate(devNum, portInd, &qParams, &queueInd);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", queueInd);

    return CMD_OK;
}

/*******************************************************************************
* cpssTmQueueToCnodeCreate
*
* DESCRIPTION:
*       Create path from Queue to C-node.
*
* APPLICABLE DEVICES:
*       Bobcat2.
*
* NOT APPLICABLE DEVICES:
*       xCat; Lion; xCat2; DxCh1; DxCh1_Diamond; DxCh2; DxCh3; Lion2; Lion3; Puma2; Puma3; ExMx.
*
* INPUTS:
*       devNum            - Device number.
*       cNodeInd          - C-Node index.
*       qParamsPtr        - Queue parameters structure pointer.
*       aParamsPtr        - A-Node parameters structure pointer.
*       bParamsPtr        - B-Node parameters structure pointer.
*
* OUTPUTS:
*       queueIndPtr       - (pointer to) The created Queue index (APPLICABLE RANGES: 0..65535).
*       aNodeIndPtr       - (pointer to) The created A-Node index (APPLICABLE RANGES: 0..16383).
*       bNodeIndPtr       - (pointer to) The created B-Node index (APPLICABLE RANGES: 0..4095).
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_FAIL                  - on hardware error.
*       GT_BAD_PARAM             - on wrong device number or wrong parameter value.
*       GT_OUT_OF_RANGE          - on parameter value out of range.
*       GT_NO_RESOURCE           - on memory allocation fail.
*       GT_BAD_STATE             - on cNodeIndex not in use.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device.
*
* COMMENTS:
*       To indicate that no shaping is needed to the queue/node, set
*       shapingProfilePtr to CPSS_TM_INVAL_CNS.
*******************************************************************************/
static CMD_STATUS wrCpssTmQueueToCnodeCreate
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                       result;
    GT_U8                           devNum;
    GT_U32                          cNodeInd;
    CPSS_TM_QUEUE_PARAMS_STC        qParams;
    CPSS_TM_A_NODE_PARAMS_STC       aParams;
    CPSS_TM_B_NODE_PARAMS_STC       bParams;
    GT_U32                          queueInd;
    GT_U32                          aNodeInd;
    GT_U32                          bNodeInd;
    GT_U32                          ii;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* init with zeros */
    cmdOsMemSet(&qParams, 0, sizeof(qParams));
    cmdOsMemSet(&aParams, 0, sizeof(aParams));
    cmdOsMemSet(&bParams, 0, sizeof(bParams));


    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    cNodeInd = (GT_U32)inArgs[1];
    qParams.shapingProfilePtr = (GT_U32)inArgs[2];
    qParams.quantum = (GT_U32)inArgs[3];
    qParams.dropProfileInd = (GT_U32)inArgs[4];


    aParams.shapingProfilePtr = (GT_U32)inArgs[5];
    aParams.quantum = (GT_U32)inArgs[6];
    for (ii=0; ii<8; ii++) {
        aParams.schdModeArr[ii] = (GT_U32)inArgs[ii+7];
    }
    aParams.dropProfileInd = (GT_U32)inArgs[15];



    bParams.shapingProfilePtr = (GT_U32)inArgs[16];
    bParams.quantum = (GT_U32)inArgs[17];
    for (ii=0; ii<8; ii++) {
        bParams.schdModeArr[ii] = (GT_U32)inArgs[ii+18];
    }
    bParams.dropProfileInd = (GT_U32)inArgs[26];


    qParams.eligiblePrioFuncId = (CPSS_TM_ELIG_FUNC_QUEUE_ENT)inArgs[27];
    aParams.eligiblePrioFuncId = (CPSS_TM_ELIG_FUNC_NODE_ENT)inArgs[28];
    bParams.eligiblePrioFuncId = (CPSS_TM_ELIG_FUNC_NODE_ENT)inArgs[29];

    aParams.numOfChildren = (GT_U32)inArgs[30];
    bParams.numOfChildren = (GT_U32)inArgs[31];

    /* call cpss api function */
    result = cpssTmQueueToCnodeCreate(devNum, cNodeInd,&qParams,&aParams, &bParams, &queueInd, &aNodeInd, &bNodeInd);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d%d%d", queueInd,aNodeInd, bNodeInd);

    return CMD_OK;
}


/*******************************************************************************
* cpssTmQueueToBnodeCreate
*
* DESCRIPTION:
*       Create path from Queue to B-node.
*
* APPLICABLE DEVICES:
*       Bobcat2.
*
* NOT APPLICABLE DEVICES:
*       xCat; Lion; xCat2; DxCh1; DxCh1_Diamond; DxCh2; DxCh3; Lion2; Lion3; Puma2; Puma3; ExMx.
*
* INPUTS:
*       devNum            - Device number.
*       bNodeInd          - B-Node index.
*       qParamsPtr        - Queue parameters structure pointer.
*       aParamsPtr        - A-Node parameters structure pointer.
*
* OUTPUTS:
*       queueIndPtr       - (pointer to) The created Queue index (APPLICABLE RANGES: 0..65535).
*       aNodeIndPtr       - (pointer to) The created A-Node index (APPLICABLE RANGES: 0..16383).
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_FAIL                  - on hardware error.
*       GT_BAD_PARAM             - on wrong device number or wrong parameter value.
*       GT_OUT_OF_RANGE          - on parameter value out of range.
*       GT_NO_RESOURCE           - on memory allocation fail.
*       GT_BAD_STATE             - on bNodeInd not in use.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device.
*
* COMMENTS:
*       To indicate that no shaping is needed to the queue/node, set
*       shapingProfilePtr to CPSS_TM_INVAL_CNS.
*******************************************************************************/
static CMD_STATUS wrCpssTmQueueToBnodeCreate
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                       result;
    GT_U8                           devNum;
    GT_U32                          bNodeInd;
    CPSS_TM_QUEUE_PARAMS_STC        qParams;
    CPSS_TM_A_NODE_PARAMS_STC       aParams;
    GT_U32                          queueInd;
    GT_U32                          aNodeInd;
    GT_U32                          ii;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* init with zeros */
    cmdOsMemSet(&qParams, 0, sizeof(qParams));
    cmdOsMemSet(&aParams, 0, sizeof(aParams));


    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    bNodeInd = (GT_U32)inArgs[1];
    qParams.shapingProfilePtr = (GT_U32)inArgs[2];
    qParams.quantum = (GT_U32)inArgs[3];
    qParams.dropProfileInd = (GT_U32)inArgs[4];


    aParams.shapingProfilePtr = (GT_U32)inArgs[5];
    aParams.quantum = (GT_U32)inArgs[6];
    for (ii=0; ii<8; ii++) {
        aParams.schdModeArr[ii] = (GT_U32)inArgs[ii+7];
    }
    aParams.dropProfileInd = (GT_U32)inArgs[15];


    qParams.eligiblePrioFuncId = (CPSS_TM_ELIG_FUNC_QUEUE_ENT)inArgs[16];
    aParams.eligiblePrioFuncId = (CPSS_TM_ELIG_FUNC_NODE_ENT)inArgs[17];
    aParams.numOfChildren = (GT_U32)inArgs[18];

    /* call cpss api function */
    result = cpssTmQueueToBnodeCreate(devNum, bNodeInd,&qParams,&aParams, &queueInd, &aNodeInd);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d%d", queueInd,aNodeInd);

    return CMD_OK;
}

/*******************************************************************************
* cpssTmQueueToAnodeCreate
*
* DESCRIPTION:
*       Create path from Queue to A-node.
*
* APPLICABLE DEVICES:
*       Bobcat2.
*
* NOT APPLICABLE DEVICES:
*       xCat; Lion; xCat2; DxCh1; DxCh1_Diamond; DxCh2; DxCh3; Lion2; Lion3; Puma2; Puma3; ExMx.
*
* INPUTS:
*       devNum            - Device number.
*       aNodeInd          - A-Node index.
*       qParamsPtr        - Queue parameters structure pointer.
*
* OUTPUTS:
*       queueIndPtr       - (pointer to) The created Queue index (APPLICABLE RANGES: 0..65535).
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_FAIL                  - on hardware error.
*       GT_BAD_PARAM             - on wrong device number or wrong parameter value.
*       GT_OUT_OF_RANGE          - on parameter value out of range.
*       GT_NO_RESOURCE           - on memory allocation fail.
*       GT_BAD_STATE             - on aNodeInd not in use.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device.
*
* COMMENTS:
*       To indicate that no shaping is needed to the queue/node, set
*       shapingProfilePtr to CPSS_TM_INVAL_CNS.
*******************************************************************************/
static CMD_STATUS wrCpssTmQueueToAnodeCreate
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                       result;
    GT_U8                           devNum;
    GT_U32                          aNodeInd;
    CPSS_TM_QUEUE_PARAMS_STC        qParams;
    GT_U32                          queueInd;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* init with zeros */
    cmdOsMemSet(&qParams, 0, sizeof(qParams));

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    aNodeInd = (GT_U32)inArgs[1];
    qParams.shapingProfilePtr = (GT_U32)inArgs[2];
    qParams.quantum = (GT_U32)inArgs[3];
    qParams.dropProfileInd = (GT_U32)inArgs[4];
    qParams.eligiblePrioFuncId = (CPSS_TM_ELIG_FUNC_QUEUE_ENT)inArgs[5];

    /* call cpss api function */
    result = cpssTmQueueToAnodeCreate(devNum, aNodeInd,&qParams,&queueInd);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", queueInd);

    return CMD_OK;
}

/*******************************************************************************
* cpssTmQueueToAnodeByIndexCreate
*
* DESCRIPTION:
*       Create path from selected Queue to A-node.
*
* APPLICABLE DEVICES:
*       Bobcat2.
*
* NOT APPLICABLE DEVICES:
*       xCat; xCat3; Lion; xCat2; DxCh1; DxCh1_Diamond; DxCh2; DxCh3; Lion2; Lion3; Puma2; Puma3; ExMx.
*
* INPUTS:
*       devNum            - Device number.
*       aNodeInd          - A-Node parent index.
*       qParamsPtr        - Queue parameters structure pointer.*
*       queueInd          - Index of Queue to init (APPLICABLE RANGES: 0..65535).
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_FAIL                  - on hardware error.
*       GT_BAD_PARAM             - on wrong device number or wrong parameter value.
*       GT_OUT_OF_RANGE          - on parameter value out of range.
*       GT_NO_RESOURCE           - on memory allocation fail.
*       GT_BAD_STATE             - on aNodeInd not in use.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device.
*
* COMMENTS:
*       1.To indicate that no shaping is needed to the queue/node, set
*         shapingProfilePtr to CPSS_TM_INVAL_CNS.
*       2. The selected queue must previously allocated to A-node parent
*******************************************************************************/
static CMD_STATUS wrCpssTmQueueToAnodeByIndexCreate
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                       result;
    GT_U8                           devNum;
    GT_U32                          aNodeInd;
    CPSS_TM_QUEUE_PARAMS_STC        qParams;
    GT_U32                          queueInd;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* init with zeros */
    cmdOsMemSet(&qParams, 0, sizeof(qParams));

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    aNodeInd = (GT_U32)inArgs[1];
    qParams.shapingProfilePtr = (GT_U32)inArgs[2];
    qParams.quantum = (GT_U32)inArgs[3];
    qParams.dropProfileInd = (GT_U32)inArgs[4];
    qParams.eligiblePrioFuncId = (CPSS_TM_ELIG_FUNC_QUEUE_ENT)inArgs[5];
	queueInd = (GT_U32)inArgs[6];

    /* call cpss api function */
    result = cpssTmQueueToAnodeByIndexCreate(devNum, aNodeInd,&qParams, queueInd);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssTmAnodeToPortCreate
*
* DESCRIPTION:
*       Create path from A-node to Port.
*
* APPLICABLE DEVICES:
*       Bobcat2.
*
* NOT APPLICABLE DEVICES:
*       xCat; Lion; xCat2; DxCh1; DxCh1_Diamond; DxCh2; DxCh3; Lion2; Lion3; Puma2; Puma3; ExMx.
*
* INPUTS:
*       devNum            - Device number.
*       portInd           - Port index.
*       aParamsPtr        - A-Node parameters structure pointer.
*       bParamsPtr        - B-Node parameters structure pointer.
*       cParamsPtr        - C-Node parameters structure pointer.
*
* OUTPUTS:
*       aNodeIndPtr       - (pointer to) The created A-Node index (APPLICABLE RANGES: 0..16383).
*       bNodeIndPtr       - (pointer to) The created B-Node index (APPLICABLE RANGES: 0..4095).
*       cNodeIndPtr       - (pointer to) The created C-Node index (APPLICABLE RANGES: 0..511).
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_FAIL                  - on hardware error.
*       GT_BAD_PARAM             - on wrong device number or wrong parameter value.
*       GT_OUT_OF_RANGE          - on parameter value out of range.
*       GT_NO_RESOURCE           - on memory allocation fail.
*       GT_BAD_STATE             - on portInd not in use.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device.
*
* COMMENTS:
*       To indicate that no shaping is needed to the queue/node, set
*       shapingProfilePtr to CPSS_TM_INVAL_CNS.
*******************************************************************************/
static CMD_STATUS wrCpssTmAnodeToPortCreate
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                       result;
    GT_U8                           devNum;
    GT_U32                          portInd;
    CPSS_TM_A_NODE_PARAMS_STC       aParams;
    CPSS_TM_B_NODE_PARAMS_STC       bParams;
    CPSS_TM_C_NODE_PARAMS_STC       cParams;
    GT_U32                          aNodeInd;
    GT_U32                          bNodeInd;
    GT_U32                          cNodeInd;
    GT_U32                          ii;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* init with zeros */
    cmdOsMemSet(&aParams, 0, sizeof(aParams));
    cmdOsMemSet(&bParams, 0, sizeof(bParams));
    cmdOsMemSet(&cParams, 0, sizeof(cParams));


    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    portInd = (GT_U32)inArgs[1];

    aParams.shapingProfilePtr = (GT_U32)inArgs[2];
    aParams.quantum = (GT_U32)inArgs[3];
    for (ii=0; ii<8; ii++) {
        aParams.schdModeArr[ii] = (GT_U32)inArgs[ii+4];
    }
    aParams.dropProfileInd = (GT_U32)inArgs[12];


    bParams.shapingProfilePtr = (GT_U32)inArgs[13];
    bParams.quantum = (GT_U32)inArgs[14];
    for (ii=0; ii<8; ii++) {
        bParams.schdModeArr[ii] = (GT_U32)inArgs[ii+15];
    }
    bParams.dropProfileInd = (GT_U32)inArgs[23];


    cParams.shapingProfilePtr = (GT_U32)inArgs[24];
    cParams.quantum = (GT_U32)inArgs[25];
    for (ii=0; ii<8; ii++) {
        cParams.schdModeArr[ii] = (GT_U32)inArgs[ii+26];
    }
    cParams.dropCosMap = (GT_U32)inArgs[34];
    for (ii=0; ii<8; ii++) {
        cParams.dropProfileIndArr[ii] = (GT_U32)inArgs[ii+35];
    }


    aParams.eligiblePrioFuncId = (CPSS_TM_ELIG_FUNC_NODE_ENT)inArgs[43];
    bParams.eligiblePrioFuncId = (CPSS_TM_ELIG_FUNC_NODE_ENT)inArgs[44];
    cParams.eligiblePrioFuncId = (CPSS_TM_ELIG_FUNC_NODE_ENT)inArgs[45];

    aParams.numOfChildren = (GT_U32)inArgs[46];
    bParams.numOfChildren = (GT_U32)inArgs[47];
    cParams.numOfChildren = (GT_U32)inArgs[48];

    /* call cpss api function */
    result = cpssTmAnodeToPortCreate(devNum, portInd,&aParams,&bParams, &cParams,&aNodeInd, &bNodeInd,&cNodeInd);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d%d%d",aNodeInd, bNodeInd,cNodeInd);

    return CMD_OK;
}

/*******************************************************************************
* cpssTmAnodeToCnodeCreate
*
* DESCRIPTION:
*       Create path from A-node to C-node.
*
* APPLICABLE DEVICES:
*       Bobcat2.
*
* NOT APPLICABLE DEVICES:
*       xCat; Lion; xCat2; DxCh1; DxCh1_Diamond; DxCh2; DxCh3; Lion2; Lion3; Puma2; Puma3; ExMx.
*
* INPUTS:
*       devNum            - Device number.
*       cNodeInd          - C-Node index.
*       aParamsPtr        - A-Node parameters structure pointer.
*       bParamsPtr        - B-Node parameters structure pointer.
*
* OUTPUTS:
*       aNodeIndPtr       - (pointer to) The created A-Node index (APPLICABLE RANGES: 0..16383).
*       bNodeIndPtr       - (pointer to) The created B-Node index (APPLICABLE RANGES: 0..4095).
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_FAIL                  - on hardware error.
*       GT_BAD_PARAM             - on wrong device number or wrong parameter value.
*       GT_OUT_OF_RANGE          - on parameter value out of range.
*       GT_NO_RESOURCE           - on memory allocation fail.
*       GT_BAD_STATE             - on cNodeInd not in use.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device.
*
* COMMENTS:
*       To indicate that no shaping is needed to the queue/node, set
*       shapingProfilePtr to CPSS_TM_INVAL_CNS.
*******************************************************************************/
static CMD_STATUS wrCpssTmAnodeToCnodeCreate
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                       result;
    GT_U8                           devNum;
    GT_U32                          cNodeInd;
    CPSS_TM_A_NODE_PARAMS_STC       aParams;
    CPSS_TM_B_NODE_PARAMS_STC       bParams;
    GT_U32                          aNodeInd;
    GT_U32                          bNodeInd;
    GT_U32                          ii;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* init with zeros */
    cmdOsMemSet(&aParams, 0, sizeof(aParams));
    cmdOsMemSet(&bParams, 0, sizeof(bParams));

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    cNodeInd = (GT_U32)inArgs[1];

    aParams.shapingProfilePtr = (GT_U32)inArgs[2];
    aParams.quantum = (GT_U32)inArgs[3];
    for (ii=0; ii<8; ii++) {
        aParams.schdModeArr[ii] = (GT_U32)inArgs[ii+4];
    }
    aParams.dropProfileInd = (GT_U32)inArgs[12];


    bParams.shapingProfilePtr = (GT_U32)inArgs[13];
    bParams.quantum = (GT_U32)inArgs[14];
    for (ii=0; ii<8; ii++) {
        bParams.schdModeArr[ii] = (GT_U32)inArgs[ii+15];
    }
    bParams.dropProfileInd = (GT_U32)inArgs[23];


    aParams.eligiblePrioFuncId = (CPSS_TM_ELIG_FUNC_NODE_ENT)inArgs[24];
    bParams.eligiblePrioFuncId = (CPSS_TM_ELIG_FUNC_NODE_ENT)inArgs[25];

    aParams.numOfChildren = (GT_U32)inArgs[26];
    bParams.numOfChildren = (GT_U32)inArgs[27];

    /* call cpss api function */
    result = cpssTmAnodeToCnodeCreate(devNum, cNodeInd,&aParams,&bParams, &aNodeInd, &bNodeInd);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d%d",aNodeInd, bNodeInd);

    return CMD_OK;
}

/*******************************************************************************
* cpssTmAnodeToBnodeCreate
*
* DESCRIPTION:
*       Create path from A-node to B-node.
*
* APPLICABLE DEVICES:
*       Bobcat2.
*
* NOT APPLICABLE DEVICES:
*       xCat; Lion; xCat2; DxCh1; DxCh1_Diamond; DxCh2; DxCh3; Lion2; Lion3; Puma2; Puma3; ExMx.
*
* INPUTS:
*       devNum            - Device number.
*       bNodeInd          - B-Node index.
*       aParamsPtr        - A-Node parameters structure pointer.
*
* OUTPUTS:
*       aNodeIndPtr       - (pointer to) The created A-Node index (APPLICABLE RANGES: 0..16383).
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_FAIL                  - on hardware error.
*       GT_BAD_PARAM             - on wrong device number or wrong parameter value.
*       GT_OUT_OF_RANGE          - on parameter value out of range.
*       GT_NO_RESOURCE           - on memory allocation fail.
*       GT_BAD_STATE             - on bNodeInd not in use.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device.
*
* COMMENTS:
*       To indicate that no shaping is needed to the queue/node, set
*       shapingProfilePtr to CPSS_TM_INVAL_CNS.
*******************************************************************************/
static CMD_STATUS wrCpssTmAnodeToBnodeCreate
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                       result;
    GT_U8                           devNum;
    GT_U32                          bNodeInd;
    CPSS_TM_A_NODE_PARAMS_STC       aParams;
    GT_U32                          aNodeInd;
    GT_U32                          ii;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* init with zeros */
    cmdOsMemSet(&aParams, 0, sizeof(aParams));

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    bNodeInd = (GT_U32)inArgs[1];

    aParams.shapingProfilePtr = (GT_U32)inArgs[2];
    aParams.quantum = (GT_U32)inArgs[3];
    for (ii=0; ii<8; ii++) {
        aParams.schdModeArr[ii] = (GT_U32)inArgs[ii+4];
    }
    aParams.dropProfileInd = (GT_U32)inArgs[12];
    aParams.eligiblePrioFuncId = (CPSS_TM_ELIG_FUNC_NODE_ENT)inArgs[13];
    aParams.numOfChildren = (GT_U32)inArgs[14];

    /* call cpss api function */
    result = cpssTmAnodeToBnodeCreate(devNum, bNodeInd,&aParams,&aNodeInd);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d",aNodeInd);

    return CMD_OK;
}

/*******************************************************************************
* cpssTmAnodeToBnodeWithQueuePoolCreate
*
* DESCRIPTION:
*       Create path from A-node to B-node & allocates user defined queue children pool
*       Not applicable for symmetric port tree creation
*
* APPLICABLE DEVICES:
*       Bobcat2.
*
* NOT APPLICABLE DEVICES:
*       xCat; xCat3; Lion; xCat2; DxCh1; DxCh1_Diamond; DxCh2; DxCh3; Lion2; Lion3; Puma2; Puma3; ExMx.
*
* INPUTS:
*       devNum            - Device number.
*       bNodeInd          - B-Node parent index.
*       aParamsPtr        - A-Node parameters structure pointer.
*       firstQueueInRange - index of first queue in required queue range
*       queueRangeSize    - queue range size
*
* OUTPUTS:
*       aNodeIndPtr       - (pointer to) The created A-Node index (APPLICABLE RANGES: 0..16383).
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_FAIL                  - on hardware error.
*       GT_BAD_PARAM             - on wrong device number or wrong parameter value.
*       GT_OUT_OF_RANGE          - on parameter value out of range.
*       GT_NO_RESOURCE           - on resource allocation fail.
*       GT_BAD_STATE             - on bNodeInd not in use.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device.
*
* COMMENTS:
*       To indicate that no shaping is needed to the queue/node, set
*       shapingProfilePtr to CPSS_TM_INVAL_CNS.
*******************************************************************************/

static CMD_STATUS wrCpssTmAnodeToBnodeWithQueuePoolCreate
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                       result;
    GT_U8                           devNum;
    GT_U32                          bNodeInd;
    CPSS_TM_A_NODE_PARAMS_STC       aParams;
    GT_U32                          aNodeInd;
    GT_U32                          ii;
    GT_U32                          firstQueueInRange;
    GT_U32                          queueRangeSize;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* init with zeros */
    cmdOsMemSet(&aParams, 0, sizeof(aParams));

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    bNodeInd = (GT_U32)inArgs[1];

    aParams.shapingProfilePtr = (GT_U32)inArgs[2];
    aParams.quantum = (GT_U32)inArgs[3];
    for (ii=0; ii<8; ii++) {
        aParams.schdModeArr[ii] = (GT_U32)inArgs[ii+4];
    }
    aParams.dropProfileInd = (GT_U32)inArgs[12];
    aParams.eligiblePrioFuncId = (CPSS_TM_ELIG_FUNC_NODE_ENT)inArgs[13];
    aParams.numOfChildren = (GT_U32)inArgs[14];
	firstQueueInRange = (GT_U32)inArgs[15];
	queueRangeSize = (GT_U32)inArgs[16];

    /* call cpss api function */
    result = cpssTmAnodeToBnodeWithQueuePoolCreate(devNum, bNodeInd,&aParams, firstQueueInRange, queueRangeSize ,&aNodeInd);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d",aNodeInd);

    return CMD_OK;
}

/*******************************************************************************
* cpssTmBnodeToPortCreate
*
* DESCRIPTION:
*       Create path from B-node to Port.
*
* APPLICABLE DEVICES:
*       Bobcat2.
*
* NOT APPLICABLE DEVICES:
*       xCat; Lion; xCat2; DxCh1; DxCh1_Diamond; DxCh2; DxCh3; Lion2; Lion3; Puma2; Puma3; ExMx.
*
* INPUTS:
*       devNum            - Device number.
*       portInd           - Port index.
*       bParamsPtr        - B-Node parameters structure pointer.
*       cParamsPtr        - C-Node parameters structure pointer.
*
* OUTPUTS:
*       bNodeIndPtr       - (pointer to) The created B-Node index (APPLICABLE RANGES: 0..4095).
*       cNodeIndPtr       - (pointer to) The created C-Node index (APPLICABLE RANGES: 0..511).
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_FAIL                  - on hardware error.
*       GT_BAD_PARAM             - on wrong device number or wrong parameter value.
*       GT_OUT_OF_RANGE          - on parameter value out of range.
*       GT_NO_RESOURCE           - on memory allocation fail.
*       GT_BAD_STATE             - on portInd not in use.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device.
*
* COMMENTS:
*       To indicate that no shaping is needed to the queue/node, set
*       shapingProfilePtr to CPSS_TM_INVAL_CNS.
*******************************************************************************/
static CMD_STATUS wrCpssTmBnodeToPortCreate
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                       result;
    GT_U8                           devNum;
    GT_U32                          portInd;
    CPSS_TM_B_NODE_PARAMS_STC       bParams;
    CPSS_TM_C_NODE_PARAMS_STC       cParams;
    GT_U32                          bNodeInd;
    GT_U32                          cNodeInd;
    GT_U32                          ii;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* init with zeros */
    cmdOsMemSet(&bParams, 0, sizeof(bParams));
    cmdOsMemSet(&cParams, 0, sizeof(cParams));


    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    portInd = (GT_U32)inArgs[1];

    bParams.shapingProfilePtr = (GT_U32)inArgs[2];
    bParams.quantum = (GT_U32)inArgs[3];
    for (ii=0; ii<8; ii++) {
        bParams.schdModeArr[ii] = (GT_U32)inArgs[ii+4];
    }
    bParams.dropProfileInd = (GT_U32)inArgs[12];


    cParams.shapingProfilePtr = (GT_U32)inArgs[13];
    cParams.quantum = (GT_U32)inArgs[14];
    for (ii=0; ii<8; ii++) {
        cParams.schdModeArr[ii] = (GT_U32)inArgs[ii+15];
    }
    cParams.dropCosMap = (GT_U32)inArgs[23];
    for (ii=0; ii<8; ii++) {
        cParams.dropProfileIndArr[ii] = (GT_U32)inArgs[ii+24];
    }

    bParams.eligiblePrioFuncId = (CPSS_TM_ELIG_FUNC_NODE_ENT)inArgs[32];
    cParams.eligiblePrioFuncId = (CPSS_TM_ELIG_FUNC_NODE_ENT)inArgs[33];

    bParams.numOfChildren = (GT_U32)inArgs[34];
    cParams.numOfChildren = (GT_U32)inArgs[35];


    /* call cpss api function */
    result = cpssTmBnodeToPortCreate(devNum, portInd,&bParams, &cParams, &bNodeInd,&cNodeInd);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d%d",bNodeInd,cNodeInd);

    return CMD_OK;
}

/*******************************************************************************
* cpssTmBnodeToCnodeCreate
*
* DESCRIPTION:
*       Create path from B-node to C-node.
*
* APPLICABLE DEVICES:
*       Bobcat2.
*
* NOT APPLICABLE DEVICES:
*       xCat; Lion; xCat2; DxCh1; DxCh1_Diamond; DxCh2; DxCh3; Lion2; Lion3; Puma2; Puma3; ExMx.
*
* INPUTS:
*       devNum            - Device number.
*       cNodeInd          - C-Node index.
*       bParamsPtr        - B-Node parameters structure pointer.
*
* OUTPUTS:
*       bNodeIndPtr       - (pointer to) The created B-Node index (APPLICABLE RANGES: 0..4095).
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_FAIL                  - on hardware error.
*       GT_BAD_PARAM             - on wrong device number or wrong parameter value.
*       GT_OUT_OF_RANGE          - on parameter value out of range.
*       GT_NO_RESOURCE           - on memory allocation fail.
*       GT_BAD_STATE             - on cNodeInd not in use.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device.
*
* COMMENTS:
*       To indicate that no shaping is needed to the queue/node, set
*       shapingProfilePtr to CPSS_TM_INVAL_CNS.
*******************************************************************************/
static CMD_STATUS wrCpssTmBnodeToCnodeCreate
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                       result;
    GT_U8                           devNum;
    GT_U32                          cNodeInd;
    CPSS_TM_B_NODE_PARAMS_STC       bParams;
    GT_U32                          bNodeInd;
    GT_U32                          ii;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* init with zeros */
    cmdOsMemSet(&bParams, 0, sizeof(bParams));



    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    cNodeInd = (GT_U32)inArgs[1];

    bParams.shapingProfilePtr = (GT_U32)inArgs[2];
    bParams.quantum = (GT_U32)inArgs[3];
    for (ii=0; ii<8; ii++) {
        bParams.schdModeArr[ii] = (GT_U32)inArgs[ii+4];
    }
    bParams.dropProfileInd = (GT_U32)inArgs[12];
    bParams.eligiblePrioFuncId = (CPSS_TM_ELIG_FUNC_NODE_ENT)inArgs[13];
    bParams.numOfChildren = (GT_U32)inArgs[14];


    /* call cpss api function */
    result = cpssTmBnodeToCnodeCreate(devNum, cNodeInd,&bParams, &bNodeInd);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d",bNodeInd);

    return CMD_OK;
}

/*******************************************************************************
* cpssTmBnodeToCnodeCreate
*
* DESCRIPTION:
*       Create path from C-node to Port.
*
* APPLICABLE DEVICES:
*       Bobcat2.
*
* NOT APPLICABLE DEVICES:
*       xCat; Lion; xCat2; DxCh1; DxCh1_Diamond; DxCh2; DxCh3; Lion2; Lion3; Puma2; Puma3; ExMx.
*
* INPUTS:
*       devNum            - Device number.
*       portInd           - Port index.
*       cParamsPtr        - C-Node parameters structure pointer.
*
* OUTPUTS:
*       cNodeIndPtr       - (pointer to) The created C-Node index (APPLICABLE RANGES: 0..511).
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_FAIL                  - on hardware error.
*       GT_BAD_PARAM             - on wrong device number or wrong parameter value.
*       GT_OUT_OF_RANGE          - on parameter value out of range.
*       GT_NO_RESOURCE           - on memory allocation fail.
*       GT_BAD_STATE             - on portInd not in use.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device.
*
* COMMENTS:
*       To indicate that no shaping is needed to the queue/node, set
*       shapingProfilePtr to CPSS_TM_INVAL_CNS.
*******************************************************************************/
static CMD_STATUS wrCpssTmCnodeToPortCreate
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                       result;
    GT_U8                           devNum;
    GT_U32                          portInd;
    CPSS_TM_C_NODE_PARAMS_STC       cParams;
    GT_U32                          cNodeInd;
    GT_U32                          ii;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* init with zeros */
    cmdOsMemSet(&cParams, 0, sizeof(cParams));


    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    portInd = (GT_U32)inArgs[1];

    cParams.shapingProfilePtr = (GT_U32)inArgs[2];
    cParams.quantum = (GT_U32)inArgs[3];
    for (ii=0; ii<8; ii++) {
        cParams.schdModeArr[ii] = (GT_U32)inArgs[ii+4];
    }
    cParams.dropCosMap = (GT_U32)inArgs[12];
    for (ii=0; ii<8; ii++) {
        cParams.dropProfileIndArr[ii] = (GT_U32)inArgs[ii+13];
    }
    cParams.eligiblePrioFuncId = (CPSS_TM_ELIG_FUNC_NODE_ENT)inArgs[21];
    cParams.numOfChildren = (GT_U32)inArgs[22];

    /* call cpss api function */
    result = cpssTmCnodeToPortCreate(devNum, portInd,&cParams, &cNodeInd);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d",cNodeInd);

    return CMD_OK;
}
/**** database initialization **************************************/

static CMD_COMMAND dbCommands[] =
{
    {"cpssTmPortCreate",
     &wrCpssTmPortCreate,
     29, 0},
    {"cpssTmAsymPortCreate",
     &wrCpssTmAsymPortCreate,
     26, 0},
    {"cpssTmPortDropPerCosConfig",
     &wrCpssTmPortDropPerCosConfig,
     11, 0},
    {"cpssTmQueueToPortCreate",
     &wrCpssTmQueueToPortCreate,
     53, 0},
    {"cpssTmTransQueueToPortCreate",
    &wrCpssTmTransQueueToPortCreate,
     6, 0},
    {"cpssTmQueueToCnodeCreate",
    &wrCpssTmQueueToCnodeCreate,
     32, 0},
    {"cpssTmQueueToBnodeCreate",
    &wrCpssTmQueueToBnodeCreate,
     19, 0},
    {"cpssTmQueueToAnodeCreate",
    &wrCpssTmQueueToAnodeCreate,
     6, 0},
    {"cpssTmQueueToAnodeByIndexCreate",
    &wrCpssTmQueueToAnodeByIndexCreate,
     7, 0},
    {"cpssTmAnodeToPortCreate",
    &wrCpssTmAnodeToPortCreate,
     49, 0},
    {"cpssTmAnodeToCnodeCreate",
    &wrCpssTmAnodeToCnodeCreate,
     28, 0},
    {"cpssTmAnodeToBnodeCreate",
    &wrCpssTmAnodeToBnodeCreate,
     15, 0},
    {"cpssTmAnodeToBnodeWithQueuePoolCreate",
    &wrCpssTmAnodeToBnodeWithQueuePoolCreate,
     17, 0},
    {"cpssTmBnodeToPortCreate",
    &wrCpssTmBnodeToPortCreate,
     36, 0},
    {"cpssTmBnodeToCnodeCreate",
    &wrCpssTmBnodeToCnodeCreate,
     15, 0},
    {"cpssTmCnodeToPortCreate",
    &wrCpssTmCnodeToPortCreate,
     23, 0}
};

#define numCommands (sizeof(dbCommands) / sizeof(CMD_COMMAND))

/*******************************************************************************
* cmdLibInitCpssTmNodesCreate
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
GT_STATUS cmdLibInitCpssTmNodesCreate
(
    GT_VOID
)
{
    return cmdInitLibrary(dbCommands, numCommands);
}
