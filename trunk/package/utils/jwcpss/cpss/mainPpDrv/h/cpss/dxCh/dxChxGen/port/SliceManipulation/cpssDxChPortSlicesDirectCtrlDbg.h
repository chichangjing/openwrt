/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* cpssDxChDiagPortSlicesDirectCtrl.c
*
* DESCRIPTION:
*       Pizza Aribiter Units Slices Manipulation Interface
*
* FILE REVISION NUMBER:
*       $Revision: 3 $
*******************************************************************************/
#ifndef __CPSS_DXCH_DIAG_SLICES_DIRECT_CTRL_H
#define __CPSS_DXCH_DIAG_SLICES_DIRECT_CTRL_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */


#include <cpss/generic/cpssTypes.h>
#include <cpss/dxCh/dxChxGen/port/cpssDxChPortCtrl.h>  
#include <cpss/dxCh/dxChxGen/config/private/prvCpssDxChInfo.h>

typedef enum 
{
    CPSS_DXCH_DIAG_PA_UNIT_RXDMA_E = 0,
    CPSS_DXCH_DIAG_PA_UNIT_RXDMA_CTU_E, /* 1 */
    CPSS_DXCH_DIAG_PA_UNIT_TXDMA_E,
    CPSS_DXCH_DIAG_PA_UNIT_TXDMA_CTU_E,
    CPSS_DXCH_DIAG_PA_UNIT_BM_E,
    CPSS_DXCH_DIAG_PA_UNIT_TXQ_E,
    CPSS_DXCH_DIAG_PA_UNIT_MAX_E             /* never cross this boundary */
}CPSS_DXCH_DIAG_PIZZA_ARBITER_UNIT_ENT;

/*******************************************************************************
* cpssDxChSlicesNumSet
*
* DESCRIPTION:
*      The function sets number of slices to be used in Pizza Arbiter for specific unit
*
* APPLICABLE DEVICES:
*        Lion2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum           - device number
*       portGroupId      - port group id
*       unitId           - unit id
*       sliceNumToConfig - slices number to be set in pizza arbiter
*
* OUTPUTS:
*
* RETURNS:
*       GT_OK           - on success,
*       GT_BAD_PARAM    - bad devNum, portGroupId
*       GT_HW_ERROR     - hw error
*       GT_OUT_OF_RANGE - number of slice to be set is greater than available
*       GT_ABORTED      - error in internal data structures
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChSlicesNumSet
(
    IN GT_U8                                 devNum,
    IN GT_U32                                portGroupId,
    IN CPSS_DXCH_DIAG_PIZZA_ARBITER_UNIT_ENT unitId,
    IN GT_U32                                sliceNumToConfig
);

/*******************************************************************************
* cpssDxChSlicesNumGet
*
* DESCRIPTION:
*      The function gets number of slices used in Pizza Arbiter for specific unit
*
* APPLICABLE DEVICES:
*        Lion2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum      - device number
*       portGroupId - port group id
*       unitId      - unit id
*
* OUTPUTS:
*       numOfConfiguredSlicesPtr   - (the pointer to) number of slices used by pizza arbiter
*
* RETURNS:
*       GT_OK           - on success,
*       GT_BAD_PARAM    - bad devNum, portGroupId
*       GT_HW_ERROR     - hw error
*       GT_ABORTED      - error in internal data structures
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChSlicesNumGet
(
    IN GT_U8                                 devNum,
    IN GT_U32                                portGroupId,
    IN CPSS_DXCH_DIAG_PIZZA_ARBITER_UNIT_ENT unitId,
    OUT GT_U32                              *numOfConfiguredSlicesPtr
);

/*******************************************************************************
* cpssDxChSliceStateSet
*
* DESCRIPTION:
*      The function sets slice state for specific unit
*
* APPLICABLE DEVICES:
*        Lion2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum      - device number
*       portGroupId - port group id
*       unitId      - unit id
*       sliceNum      - slice number to be set in pizza arbiter
*       localPortNum  - local port number
*       isEnable      - is enable or disable
*
* OUTPUTS:
*
* RETURNS:
*       GT_OK           - on success,
*       GT_BAD_PARAM    - bad devNum, portGroupId
*       GT_HW_ERROR     - hw error
*       GT_OUT_OF_RANGE - number of slice to be set is greater than available
*       GT_ABORTED      - error in internal data structures
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChSliceStateSet
(
    IN GT_U8                                 devNum,
    IN GT_U32                                portGroupId,
    IN CPSS_DXCH_DIAG_PIZZA_ARBITER_UNIT_ENT unitId,
    IN GT_U32                                sliceNum,
    IN GT_U32                                localPortNum,
    IN GT_BOOL                               isEnable
);

/*******************************************************************************
* cpssDxChSliceStateGet
*
* DESCRIPTION:
*      The function gets state of specific slice in specific unit
*
* APPLICABLE DEVICES:
*        Lion2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum      - device number
*       portGroupId - port group id
*       unitId      - unit idex
*       sliceNum    - number of slice which state is desired to get 
*
* OUTPUTS:
*       localPortNumPtr  - (pointer to) local port number
*       isEnablePtr      - (pointer to) whether port is enable or disable
*
* RETURNS:
*       GT_OK           - on success,
*       GT_BAD_PARAM    - bad devNum, portGroupId
*       GT_HW_ERROR     - hw error
*       GT_OUT_OF_RANGE - number of slice to be set is greater than available
*       GT_ABORTED      - error in internal data structures
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChSliceStateGet
(
    IN GT_U8                                 devNum,
    IN GT_U32                                portGroupId,
    IN CPSS_DXCH_DIAG_PIZZA_ARBITER_UNIT_ENT unitId,
    IN GT_U32                                sliceNum,
    OUT GT_U32                               *localPortNumPtr,
    OUT GT_BOOL                              *isEnablePtr
);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif


