/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* cpssTmNodesUpdate.h
*
* DESCRIPTION:
*       TM nodes update APIs
*
* FILE REVISION NUMBER:
*       $Revision: 1 $
*******************************************************************************/

#ifndef __cpssTmNodesUpdateh
#define __cpssTmNodesUpdateh

#include <cpss/generic/tm/cpssTmPublicDefs.h>


/*******************************************************************************
* cpssTmQueueUpdate
*
* DESCRIPTION:
*       Update queue parameters.
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum.
*
* NOT APPLICABLE DEVICES:
*       xCat; xCat3; Lion; xCat2; DxCh1; DxCh1_Diamond; DxCh2; DxCh3; Lion2; Bobcat3; Puma2; Puma3; ExMx.
*
* INPUTS:
*       devNum          - Device number.
*       index           - Queue index.
*       paramsPtr       - Queue parameters structure pointer.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_FAIL                  - on hardware error.
*       GT_BAD_PARAM             - on wrong device number or wrong parameter value.
*       GT_OUT_OF_RANGE          - on parameter value out of range.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device.
*
* COMMENTS:
*       1. When error occurs, the entry is considered inconsistent.
*       2. 'dropProfileInd' parameter will be updated in any case,
*       set it's value to be the same as in DB if you don't want to change it.
*******************************************************************************/
GT_STATUS cpssTmQueueUpdate
(
    IN GT_U8                        devNum,
    IN GT_U32                       index,
    IN CPSS_TM_QUEUE_PARAMS_STC     *paramsPtr
);


/*******************************************************************************
* cpssTmAnodeUpdate
*
* DESCRIPTION:
*       Update A-node parameters.
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum.
*
* NOT APPLICABLE DEVICES:
*       xCat; xCat3; Lion; xCat2; DxCh1; DxCh1_Diamond; DxCh2; DxCh3; Lion2; Bobcat3; Puma2; Puma3; ExMx.
*
* INPUTS:
*       devNum          - Device number.
*       index           - Node index.
*       paramsPtr       - Node parameters structure pointer.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_FAIL                  - on hardware error.
*       GT_BAD_PARAM             - on wrong device number or wrong parameter value.
*       GT_OUT_OF_RANGE          - on parameter value out of range.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device.
*
* COMMENTS:
*       1. When error occurs, the entry is considered inconsistent.
*       2. 'numOfChildren' can't be updated.
*       3. 'dropProfileInd' parameter will be updated in any case,
*       set it's value to be the same as in DB if you don't want to change it.
*******************************************************************************/
GT_STATUS cpssTmAnodeUpdate
(
    IN GT_U8                        devNum,
    IN GT_U32                       index,
    IN CPSS_TM_A_NODE_PARAMS_STC    *paramsPtr
);


/*******************************************************************************
* cpssTmBnodeUpdate
*
* DESCRIPTION:
*       Update B-node parameters.
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum.
*
* NOT APPLICABLE DEVICES:
*       xCat; xCat3; Lion; xCat2; DxCh1; DxCh1_Diamond; DxCh2; DxCh3; Lion2; Bobcat3; Puma2; Puma3; ExMx.
*
* INPUTS:
*       devNum          - Device number.
*       index           - Node index.
*       paramsPtr       - Node parameters structure pointer.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_FAIL                  - on hardware error.
*       GT_BAD_PARAM             - on wrong device number or wrong parameter value.
*       GT_OUT_OF_RANGE          - on parameter value out of range.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device.
*
* COMMENTS:
*       1. When error occurs, the entry is considered inconsistent.
*       2. 'numOfChildren' can't be updated.
*******************************************************************************/
GT_STATUS cpssTmBnodeUpdate
(
    IN GT_U8                        devNum,
    IN GT_U32                       index,
    IN CPSS_TM_B_NODE_PARAMS_STC    *paramsPtr
);


/*******************************************************************************
* cpssTmCnodeUpdate
*
* DESCRIPTION:
*       Update C-node parameters.
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum.
*
* NOT APPLICABLE DEVICES:
*       xCat; xCat3; Lion; xCat2; DxCh1; DxCh1_Diamond; DxCh2; DxCh3; Lion2; Bobcat3; Puma2; Puma3; ExMx.
*
* INPUTS:
*       devNum          - Device number.
*       index           - Node index.
*       paramsPtr       - Node parameters structure pointer.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_FAIL                  - on hardware error.
*       GT_BAD_PARAM             - on wrong device number or wrong parameter value.
*       GT_OUT_OF_RANGE          - on parameter value out of range.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device.
*
* COMMENTS:
*       1. When error occurs, the entry is considered inconsistent.
*       2. 'numOfChildren' can't be updated.
*******************************************************************************/
GT_STATUS cpssTmCnodeUpdate
(
    IN GT_U8                         devNum,
    IN GT_U32                        index,
    IN CPSS_TM_C_NODE_PARAMS_STC     *paramsPtr
);


/*******************************************************************************
* cpssTmPortShapingUpdate
*
* DESCRIPTION:
*       Update Port Shaping parameters.
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum.
*
* NOT APPLICABLE DEVICES:
*       xCat; xCat3; Lion; xCat2; DxCh1; DxCh1_Diamond; DxCh2; DxCh3; Lion2; Bobcat3; Puma2; Puma3; ExMx.
*
* INPUTS:
*       devNum          - Device number.
*       index           - Port index.
*       cirBw           - Port CIR bandwidth.
*       eirBw           - Port EIR bandwidth.
*       cbs             - Port CIR burst size.
*       ebs             - Port EIR burst size.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_FAIL                  - on hardware error.
*       GT_BAD_PARAM             - on wrong device number or wrong parameter value.
*       GT_OUT_OF_RANGE          - on parameter value out of range.
*       GT_BAD_SIZE              - on Port's min/max token too large.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device.
*
* COMMENTS:
*       When error occurs, the entry is considered inconsistent.
*******************************************************************************/
GT_STATUS cpssTmPortShapingUpdate
(
    IN GT_U8    devNum,
    IN GT_U32   index,
    IN GT_U32   cirBw,
    IN GT_U32   eirBw,
    IN GT_U32   cbs,
    IN GT_U32   ebs
);


/*******************************************************************************
* cpssTmPortSchedulingUpdate
*
* DESCRIPTION:
*       Update Port Scheduling parameters.
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum.
*
* NOT APPLICABLE DEVICES:
*       xCat; xCat3; Lion; xCat2; DxCh1; DxCh1_Diamond; DxCh2; DxCh3; Lion2; Bobcat3; Puma2; Puma3; ExMx.
*
* INPUTS:
*       devNum          - Device number.
*       index           - Port index.
*       eligPrioFuncId  - Eligible Priority Function pointer.
*       quantumArrPtr   - Port quantum 8 cell array.
*       schdModeArr     - Port RR/DWRR priority pointer for C-level.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_FAIL                  - on hardware error.
*       GT_BAD_PARAM             - on wrong device number or wrong parameter value.
*       GT_OUT_OF_RANGE          - on parameter value out of range.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device.
*
* COMMENTS:
*       When error occurs, the entry is considered inconsistent.
*******************************************************************************/
GT_STATUS cpssTmPortSchedulingUpdate
(
    IN GT_U8    devNum,
    IN GT_U32   index,
    IN CPSS_TM_ELIG_FUNC_NODE_ENT   eligPrioFuncId,
    IN GT_U32   quantumArrPtr[8], /* 8 cells array */
    IN CPSS_TM_SCHD_MODE_ENT  schdModeArr[8]
);


/*******************************************************************************
* cpssTmPortDropUpdate
*
* DESCRIPTION:
*       Update Port Drop parameters.
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum.
*
* NOT APPLICABLE DEVICES:
*       xCat; xCat3; Lion; xCat2; DxCh1; DxCh1_Diamond; DxCh2; DxCh3; Lion2; Bobcat3; Puma2; Puma3; ExMx.
*
* INPUTS:
*       devNum          - Device number.
*       index           - Port index.
*       wredProfileRef  - Port Drop Profile reference.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_FAIL                  - on hardware error.
*       GT_BAD_PARAM             - on wrong device number or wrong parameter value.
*       GT_OUT_OF_RANGE          - on parameter value out of range.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device.
*
* COMMENTS:
*       When error occurs, the entry is considered inconsistent.
*******************************************************************************/
GT_STATUS cpssTmPortDropUpdate
(
    IN GT_U8    devNum,
    IN GT_U32   index,
    IN GT_U32   wredProfileRef
);


/*******************************************************************************
* cpssTmPortDropCosUpdate
*
* DESCRIPTION:
*       Update Port Drop per Cos parameters.
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum.
*
* NOT APPLICABLE DEVICES:
*       xCat; xCat3; Lion; xCat2; DxCh1; DxCh1_Diamond; DxCh2; DxCh3; Lion2; Bobcat3; Puma2; Puma3; ExMx.
*
* INPUTS:
*       devNum          - Device number.
*       index           - Port index.
*       paramsPtr       - Port Drop per Cos parameters structure pointer.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_FAIL                  - on hardware error.
*       GT_BAD_PARAM             - on wrong device number or wrong parameter value.
*       GT_OUT_OF_RANGE          - on parameter value out of range.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device.
*
* COMMENTS:
*       When error occurs, the entry is considered inconsistent.
*******************************************************************************/
GT_STATUS cpssTmPortDropCosUpdate
(
    IN GT_U8    devNum,
    IN GT_U32   index,
    IN CPSS_TM_PORT_DROP_PER_COS_STC   *paramsPtr
);


#endif 	    /* __cpssTmNodesUpdateh */

