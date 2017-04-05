/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* cpssTmLogicalLayer.h
*
* DESCRIPTION:
*       API for creation TM nodes with persistent names
*
* FILE REVISION NUMBER:
*       $Revision: 1 $
*******************************************************************************/

#ifndef __cpssTmLogicalLayerh
#define __cpssTmLogicalLayerh

#include <cpss/generic/tm/cpssTmPublicDefs.h>

/***************************************************************************
 * Port Creation 
 ***************************************************************************/

/*******************************************************************************
* cpssTmNamedPortCreate
*
* DESCRIPTION:
*       Create NamedPort and download its parameters to HW. This port can be accessed by its index and by it's name
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum.
*
* NOT APPLICABLE DEVICES:
*       xCat; xCat3; Lion; xCat2; DxCh1; DxCh1_Diamond; DxCh2; DxCh3; Lion2; Bobcat3; Puma2; Puma3; ExMx.
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
GT_STATUS cpssTmNamedPortCreate
(
    IN GT_U8                        devNum,
    IN GT_U32                       portInd,
    IN CPSS_TM_PORT_BW_ENT          portSpeed,
    IN CPSS_TM_PORT_PARAMS_STC      *paramsPtr,
    IN GT_U32                       cNodesNum,
    IN GT_U32                       bNodesNum,
    IN GT_U32                       aNodesNum,
    IN GT_U32                       queuesNum,
	IN const GT_8_PTR               portName
);


/*******************************************************************************
* cpssTmNamedAsymPortCreate
*
* DESCRIPTION:
*       Create Named Port with assymetric sub-tree and download its parameters to HW. This port can be accessed by its index and by it's name
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum.
*
* NOT APPLICABLE DEVICES:
*       xCat; xCat3; Lion; xCat2; DxCh1; DxCh1_Diamond; DxCh2; DxCh3; Lion2; Bobcat3; Puma2; Puma3; ExMx.
* 
* INPUTS:
*       devNum            - Device number.
*       portInd           - Port index.
*       portSpeed         - Port physical bandwidth.
*       paramsPtr         - Port parameters structure pointer.
*       portName          - Name for port to be created.

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
*       To indicate that no shaping is needed for port, set
*       cirBw to CPSS_TM_INVAL_CNS, in this case other shaping 
*       parameters will not be considered.
*******************************************************************************/
GT_STATUS cpssTmNamedAsymPortCreate
(
    IN GT_U8                        devNum,
    IN GT_U32                       portInd,
    IN CPSS_TM_PORT_BW_ENT          portSpeed,
    IN CPSS_TM_PORT_PARAMS_STC      *paramsPtr,
	IN const GT_8_PTR               portName
);






/*******************************************************************************
* cpssTmNamedQueueToAnodeCreate
*
* DESCRIPTION:
*       Create path from Queue to A-node. The created queue has a name and can be accessed also by name or by it's index
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum.
*
* NOT APPLICABLE DEVICES:
*       xCat; xCat3; Lion; xCat2; DxCh1; DxCh1_Diamond; DxCh2; DxCh3; Lion2; Bobcat3; Puma2; Puma3; ExMx.
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
*     	GT_BAD_VALUE             - queueName is NULL or already occupied.
*
* COMMENTS:
*       To indicate that no shaping is needed to the queue/node, set
*       shapingProfilePtr to CPSS_TM_INVAL_CNS.
*******************************************************************************/
GT_STATUS cpssTmNamedQueueToAnodeCreate
(
    IN  GT_U8                       devNum,
    IN  GT_U32                      aNodeInd,
    IN  CPSS_TM_QUEUE_PARAMS_STC    *qParamsPtr,
	IN  const GT_8_PTR              queueName,
    OUT GT_U32                      *queueIndPtr
);



/*******************************************************************************
* cpssTmNamedAnodeToBnodeCreate
*
* DESCRIPTION:
*       Create path from A-node to B-node. The created A-node has "persistent" name and can be accessed by it always , even if 
*       physical index of this node is changed as a result of reshuffling process. 
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum.
*
* NOT APPLICABLE DEVICES:
*       xCat; xCat3; Lion; xCat2; DxCh1; DxCh1_Diamond; DxCh2; DxCh3; Lion2; Bobcat3; Puma2; Puma3; ExMx.
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
GT_STATUS cpssTmNamedAnodeToBnodeCreate
(
    IN  GT_U8                       devNum,
    IN  GT_U32                      bNodeInd,
    IN  CPSS_TM_A_NODE_PARAMS_STC   *aParamsPtr,
	IN  const GT_8_PTR                     nodeName,
    OUT GT_U32                      *aNodeIndPtr
);


/*******************************************************************************
* cpssTmNamedBnodeToCnodeCreate
*
* DESCRIPTION:
*       Create path from B-node to C-node. The created B-node has "persistent" name and can be accessed by it always , even if 
*       physical index of this node is changed as a result of reshuffling process. 
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum.
*
* NOT APPLICABLE DEVICES:
*       xCat; xCat3; Lion; xCat2; DxCh1; DxCh1_Diamond; DxCh2; DxCh3; Lion2; Bobcat3; Puma2; Puma3; ExMx.
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
GT_STATUS cpssTmNamedBnodeToCnodeCreate
(
    IN  GT_U8                       devNum,
    IN  GT_U32                      cNodeInd,
    IN  CPSS_TM_B_NODE_PARAMS_STC   *bParamsPtr,
	IN  const GT_8_PTR              nodeName,
    OUT GT_U32                      *bNodeIndPtr
);

/*******************************************************************************
* cpssTmNamedCnodeToPortCreate
*
* DESCRIPTION:
*       Create path from C-node to Port. This C-node can be accessed by its index and by it's name
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum.
*
* NOT APPLICABLE DEVICES:
*       xCat; xCat3; Lion; xCat2; DxCh1; DxCh1_Diamond; DxCh2; DxCh3; Lion2; Bobcat3; Puma2; Puma3; ExMx.
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
GT_STATUS cpssTmNamedCnodeToPortCreate
(
    IN  GT_U8                       devNum,
    IN  GT_U32                      portInd,
    IN  CPSS_TM_C_NODE_PARAMS_STC   *cParamsPtr,
	IN  const GT_8_PTR              nodeName,
    OUT GT_U32                      *cNodeIndPtr
);

/***************************************************************************
 * mapping functions
 ***************************************************************************/
/*******************************************************************************
* cpssTmGetLogicalNodeIndex
*
* DESCRIPTION:
*       Returns physical index of node with given logical name
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum.
*
* NOT APPLICABLE DEVICES:
*       xCat; xCat3; Lion; xCat2; DxCh1; DxCh1_Diamond; DxCh2; DxCh3; Lion2; Bobcat3; Puma2; Puma3; ExMx.
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
*
* COMMENTS:
******************************************************************************/

GT_STATUS		cpssTmGetLogicalNodeIndex
(
    IN  GT_U8                    devNum,
    IN  CPSS_TM_LEVEL_ENT        level,
 	IN  const GT_8_PTR           nodeName,
	OUT GT_32                    *nodeIndexPtr                        
);
/*******************************************************************************
* cpssTmGetNodeLogicalName
*
* DESCRIPTION:
*       Returns logical name of node with given index
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum.
*
* NOT APPLICABLE DEVICES:
*       xCat; xCat3; Lion; xCat2; DxCh1; DxCh1_Diamond; DxCh2; DxCh3; Lion2; Bobcat3; Puma2; Puma3; ExMx.
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
* COMMENTS:
*******************************************************************************/

GT_STATUS	cpssTmGetNodeLogicalName
(
    IN  GT_U8                    devNum,
    IN  CPSS_TM_LEVEL_ENT        level,
 	IN  GT_U32                   nodeIndex,
	OUT GT_8_PTR                 *nodeNamePtr
);
#endif 	    /* __cpssTmLogicalLayerh */

