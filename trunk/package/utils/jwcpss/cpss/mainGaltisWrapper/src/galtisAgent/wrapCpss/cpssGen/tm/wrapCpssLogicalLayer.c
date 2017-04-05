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
*       Wrapper functions for TM logical layer APIs.
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
#include <cpss/generic/tm/cpssTmLogicalLayer.h>
#include <cpss/generic/tm/cpssTmPublicDefs.h>
#include <cpss/generic/tm/cpssTmNodesRead.h>


#define	CPSS_C_NODE_FORMAT	"TestCNode%ld"
#define	CPSS_B_NODE_FORMAT	"TestBNode%ld"
#define	CPSS_A_NODE_FORMAT	"TestANode%ld"
#define	CPSS_QUEUE_FORMAT	"TestQueue%ld"
#define	CPSS_PORT_FORMAT	"TestPort%ld"

/*******************************************************************************
* cpssTmNamedPortCreate
*
* DESCRIPTION:
*       Create NamedPort and download its parameters to HW. This port can be accessed by its index and by it's name
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
*       portName          - Name for port to be created.
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
*     	GT_BAD_VALUE             - portName is NULL or already occupied.
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
static CMD_STATUS wrCpssTmNamedPortCreate
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
	GT_8						 portName[32];
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
    params.numOfChildren= (GT_32)inArgs[25];
    cNodesNum = (GT_U32)inArgs[26];
    bNodesNum = (GT_U32)inArgs[27];
    aNodesNum = (GT_U32)inArgs[28];
    queuesNum = (GT_U32)inArgs[29];



	cmdOsSprintf((char*)portName,CPSS_PORT_FORMAT,(GT_32)inArgs[30]);
    /* call cpss api function */
    result = cpssTmNamedPortCreate(devNum, portInd, portSpeed, &params, cNodesNum, bNodesNum, aNodesNum, queuesNum, (const GT_8_PTR)portName);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}


/*******************************************************************************
* cpssTmNamedAsymPortCreate
*
* DESCRIPTION:
*       Create Named Port with asymmetric sub-tree and download its parameters to HW. This port can be accessed by its index and by it's name
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
*       portName          - Name for port to be created.
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
*     	GT_BAD_VALUE             - nodeName is NULL or already occupied on for it's level.
*
* COMMENTS:
*       To indicate that no shaping is needed for port, set
*       cirBw to CPSS_TM_INVAL_CNS, in this case other shaping
*       parameters will not be considered.
*******************************************************************************/
static CMD_STATUS wrCpssTmNamedAsymPortCreate
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
	GT_8						 portName[32];
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

	cmdOsSprintf((char*)portName,CPSS_PORT_FORMAT,(GT_32)inArgs[26]);

    /* call cpss api function */
    result = cpssTmNamedAsymPortCreate(devNum, portInd, portSpeed, &params,(const GT_8_PTR)portName);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}


/*******************************************************************************
* cpssTmNamedQueueToAnodeCreate
*
* DESCRIPTION:
*       Create path from Queue to A-node. The created queue has a name and can be accessed also by name or by it's index
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
*       queueName         - Name assigned to created Queue.
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
*     	GT_BAD_VALUE             - nodeName is NULL or already occupied on for it's level.
*
* COMMENTS:
*       To indicate that no shaping is needed to the queue/node, set
*       shapingProfilePtr to CPSS_TM_INVAL_CNS.
*******************************************************************************/
static CMD_STATUS wrCpssTmNamedQueueToAnodeCreate
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
	GT_8							queueName[32];
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
 	cmdOsSprintf((char*)queueName, CPSS_QUEUE_FORMAT, (GT_32)inArgs[6]);

    /* call cpss api function */
    result = cpssTmNamedQueueToAnodeCreate(devNum, aNodeInd,&qParams,(const GT_8_PTR)queueName,&queueInd);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", queueInd);

    return CMD_OK;
}



/*******************************************************************************
* cpssTmNamedAnodeToBnodeCreate
*
* DESCRIPTION:
*       Create path from A-node to B-node. The created A-node has "persistent" name and can be accessed by it always , even if
*       physical index of this node is changed as a result of reshuffling process.
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
*       nodeName          - name assigned to created A-Node
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
*     	GT_BAD_VALUE             - nodeName is NULL or already occupied on for it's level.
*
* COMMENTS:
*       To indicate that no shaping is needed to the queue/node, set
*       shapingProfilePtr to CPSS_TM_INVAL_CNS.
*******************************************************************************/
static CMD_STATUS wrCpssTmNamedAnodeToBnodeCreate
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
	GT_8							nodeName[32];
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

	cmdOsSprintf((char*)nodeName,CPSS_A_NODE_FORMAT,(GT_32)inArgs[15]);
    /* call cpss api function */
    result = cpssTmNamedAnodeToBnodeCreate(devNum, bNodeInd,&aParams,(const GT_8_PTR)nodeName,&aNodeInd);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d",aNodeInd);

    return CMD_OK;
}


/*******************************************************************************
* cpssTmNamedBnodeToCnodeCreate
*
* DESCRIPTION:
*       Create path from B-node to C-node. The created B-node has "persistent" name and can be accessed by it always , even if
*       physical index of this node is changed as a result of reshuffling process.
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
*       nodeName          - name assigned to created B-Node
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
*     	GT_BAD_VALUE             - nodeName is NULL or already occupied on for it's level.
*
* COMMENTS:
*       To indicate that no shaping is needed to the queue/node, set
*       shapingProfilePtr to CPSS_TM_INVAL_CNS.
*******************************************************************************/
static CMD_STATUS wrCpssTmNamedBnodeToCnodeCreate
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
	GT_8                            nodeName[32];
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

	cmdOsSprintf((char*)nodeName,CPSS_B_NODE_FORMAT,(GT_32)inArgs[15]);

    /* call cpss api function */
    result = cpssTmNamedBnodeToCnodeCreate(devNum, cNodeInd,&bParams,(const GT_8_PTR)nodeName, &bNodeInd);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d",bNodeInd);

    return CMD_OK;
}

/*******************************************************************************
* cpssTmNamedCnodeToPortCreate
*
* DESCRIPTION:
*       Create path from C-node to Port. This C-node can be accessed by its index and by it's name
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
*       nodeName          - name assigned to created C-Node
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
*     	GT_BAD_VALUE             - nodeName is NULL or already occupied on for it's level.
*
* COMMENTS:
*       To indicate that no shaping is needed to the queue/node, set
*       shapingProfilePtr to CPSS_TM_INVAL_CNS.
*******************************************************************************/
static CMD_STATUS wrCpssTmNamedCnodeToPortCreate
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
	GT_8                            nodeName[32];
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

	cmdOsSprintf((char*)nodeName,CPSS_C_NODE_FORMAT,(GT_32)inArgs[23]);
    /* call cpss api function */
    result = cpssTmNamedCnodeToPortCreate(devNum, portInd, &cParams, (const GT_8_PTR)nodeName, &cNodeInd);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d",cNodeInd);

    return CMD_OK;
}


/*******************************************************************************
* cpssTmGetLogicalNodeIndex
*
* DESCRIPTION:
*       Returns physical index of node with given logical name
*
* APPLICABLE DEVICES:
*       Bobcat2.
*
* NOT APPLICABLE DEVICES:
*       xCat; Lion; xCat2; DxCh1; DxCh1_Diamond; DxCh2; DxCh3; Lion2; Lion3; Puma2; Puma3; ExMx.
*
* INPUTS:
*       devNum            - Device number.
*       level             - node level ( Port / C-level / B-level /A-level /Queue
*       nodeName          - name of interesting node
* OUTPUTS:
*       NodeIndPtr        - (pointer to) The index of node with given name (if call successful)
*                            -1 - if call failed
* RETURNS:
*       GT_OK                    - on success.
*       GT_FAIL                  - on hardware error.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device.
*       GT_BAD_PARAM             - on wrong device number or wrong parameter value.
*       GT_NO_RESOURCE           - index not found.
*     	GT_BAD_VALUE             - nodeName is NULL or already occupied on for it's level.
*
* COMMENTS:
******************************************************************************/

static CMD_STATUS wrCpssTmGetLogicalNodeIndex
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                       result;
    GT_U8                           devNum;
    GT_U8                           level;
	GT_8							nodeName[32];
    GT_32                           nodeIndex;

     /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;
	nodeIndex=-1;

     /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    level = (GT_U8)inArgs[1];


	switch (level)
	{
		case 0:
		{
			cmdOsSprintf((char*)nodeName,CPSS_QUEUE_FORMAT,(GT_32)inArgs[2]);
			break;
		}
		case 1:
		{
			cmdOsSprintf((char*)nodeName,CPSS_A_NODE_FORMAT,(GT_32)inArgs[2]);
			break;
		}
		case 2:
		{
			cmdOsSprintf((char*)nodeName,CPSS_B_NODE_FORMAT,(GT_32)inArgs[2]);
			break;
		}
		case 3:
		{
			cmdOsSprintf((char*)nodeName,CPSS_C_NODE_FORMAT,(GT_32)inArgs[2]);
			break;
		}
		case 4:
		{
			cmdOsSprintf((char*)nodeName,CPSS_PORT_FORMAT,(GT_32)inArgs[2]);
			break;
		}
		default:
		{
			cmdOsSprintf((char*)nodeName,"unknown level");
			break;
		}
	}

   result = cpssTmGetLogicalNodeIndex(devNum,level,(const GT_8_PTR)nodeName,&nodeIndex);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, " name=%s index=%d",nodeName, nodeIndex);

    return CMD_OK;
}
/*******************************************************************************
* cpssTmGetNodeLogicalName
*
* DESCRIPTION:
*       Returns logical name of node with given index
*
* APPLICABLE DEVICES:
*       Bobcat2.
*
* NOT APPLICABLE DEVICES:
*       xCat; Lion; xCat2; DxCh1; DxCh1_Diamond; DxCh2; DxCh3; Lion2; Lion3; Puma2; Puma3; ExMx.
*
* INPUTS:
*       devNum            - Device number.
*       level             - node level ( Port / C-level / B-level /A-level /Queue
*       nodeIndex         - physical index of the node in it's layer
* OUTPUTS:
*       nodeNamePtr       - (pointer to) The name of node with given index (if call successful)
*                            NULL if call failed;
* RETURNS:
*       GT_OK                    - on success.
*       GT_FAIL                  - on hardware error.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device.
*       GT_BAD_PARAM             - on wrong device number or wrong parameter value.
*       GT_NO_RESOURCE           - name not found.
*
* COMMENTS:*******************************************************************************/

static CMD_STATUS wrCpssTmGetNodeLogicalName
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                       result;
    GT_U8                           devNum;
    GT_U8                           level;
    GT_U32                          nodeIndex;
	GT_8_PTR						nodeName;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    level = (GT_U8)inArgs[1];
	nodeIndex = (GT_U32)inArgs[2];

    /* call cpss api function */
    result = cpssTmGetNodeLogicalName(devNum,level,nodeIndex,&nodeName);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, " level=%d, index=%d  name=%s",level, nodeIndex, nodeName);

    return CMD_OK;
}


/**** database initialization **************************************/

static CMD_COMMAND dbCommands[] =
{
    {"cpssTmNamedPortCreate",
     &wrCpssTmNamedPortCreate,
     31, 0},
    {"cpssTmNamedAsymPortCreate",
     &wrCpssTmNamedAsymPortCreate,
     27, 0},
    {"cpssTmNamedQueueToAnodeCreate",
    &wrCpssTmNamedQueueToAnodeCreate,
     7, 0},
    {"cpssTmNamedAnodeToBnodeCreate",
    &wrCpssTmNamedAnodeToBnodeCreate,
     16, 0},
    {"cpssTmNamedBnodeToCnodeCreate",
    &wrCpssTmNamedBnodeToCnodeCreate,
     16, 0},
    {"cpssTmNamedCnodeToPortCreate",
    &wrCpssTmNamedCnodeToPortCreate,
     24, 0},
    {"cpssTmGetLogicalNodeIndex",
    &wrCpssTmGetLogicalNodeIndex,
     3, 0},
    {"cpssTmGetNodeLogicalName",
    &wrCpssTmGetNodeLogicalName,
     3, 0},
};

#define numCommands (sizeof(dbCommands) / sizeof(CMD_COMMAND))

/*******************************************************************************
* cmdLibInitCpssTmLogicalLayer
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
GT_STATUS cmdLibInitCpssTmLogicalLayer
(
    GT_VOID
)
{
    return cmdInitLibrary(dbCommands, numCommands);
}
