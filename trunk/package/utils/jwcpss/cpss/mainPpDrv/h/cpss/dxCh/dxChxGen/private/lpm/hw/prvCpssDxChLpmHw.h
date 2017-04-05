/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* prvCpssDxChLpmHw.h
*
* DESCRIPTION:
*       HW memory read/write internal functions
*
* DEPENDENCIES:
*       None.
*
* FILE REVISION NUMBER:
*       $Revision: 4 $
*
*******************************************************************************/
#ifndef __prvCpssDxChLpmHwh
#define __prvCpssDxChLpmHwh

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include <cpss/dxCh/dxChxGen/lpm/cpssDxChLpmTypes.h>
#include <cpss/dxCh/dxChxGen/private/lpm/prvCpssDxChLpmTypes.h>

/*******************************************************************************
* prvCpssDxChLpmHwVrfEntryWrite
*
* DESCRIPTION:
*       Write a VRF table entry
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; Lion2.
*
* INPUTS:
*       devNum              - the device number
*       vrId                - the virtual router id
*       protocol            - the protocol
*       nodeType            - the type of the node
*       rootNodeAddr        - the line number of the root node
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - one of the parameters with bad value
*       GT_OUT_OF_RANGE          - rootNodeAddr is out of range
*       GT_HW_ERROR              - on hardware error
*       GT_FAIL                  - on error
*       GT_NOT_APPLICABLE_DEVICE - on non applicable device
*
* COMMENTS:
*       VRF root nodeType can be only CPSS_DXCH_LPM_REGULAR_NODE_PTR_TYPE_E or
*       CPSS_DXCH_LPM_COMPRESSED_1_NODE_PTR_TYPE_E.
*
*******************************************************************************/
GT_STATUS prvCpssDxChLpmHwVrfEntryWrite
(
    IN  GT_U8                                   devNum,
    IN  GT_U32                                  vrId,
    IN  PRV_CPSS_DXCH_LPM_PROTOCOL_STACK_ENT    protocol,
    IN  CPSS_DXCH_LPM_NEXT_POINTER_TYPE_ENT     nodeType,
    IN  GT_U32                                  rootNodeAddr
);

/*******************************************************************************
* prvCpssDxChLpmHwVrfEntryRead
*
* DESCRIPTION:
*       Read a VRF table entry
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; Lion2.
*
* INPUTS:
*       devNum              - the device number
*       vrId                - the virtual router id
*       protocol            - the protocol
*
* OUTPUTS:
*       nodeTypePtr         - the type of the node
*       rootNodeAddrPtr     - the line number of the root node
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - one of the parameters with bad value
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_HW_ERROR              - on hardware error
*       GT_FAIL                  - on error
*       GT_NOT_APPLICABLE_DEVICE - on non applicable device
*
* COMMENTS:
*       VRF root nodeTypePtr can be only CPSS_DXCH_LPM_REGULAR_NODE_PTR_TYPE_E
*       or CPSS_DXCH_LPM_COMPRESSED_1_NODE_PTR_TYPE_E.
*
*******************************************************************************/
GT_STATUS prvCpssDxChLpmHwVrfEntryRead
(
    IN  GT_U8                                   devNum,
    IN  GT_U32                                  vrId,
    IN  PRV_CPSS_DXCH_LPM_PROTOCOL_STACK_ENT    protocol,
    OUT CPSS_DXCH_LPM_NEXT_POINTER_TYPE_ENT     *nodeTypePtr,
    OUT GT_U32                                  *rootNodeAddrPtr
);

/*******************************************************************************
* prvCpssDxChLpmHwEcmpEntryWrite
*
* DESCRIPTION:
*       Write an ECMP entry
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; xCat2; DxCh2; DxCh3; xCat; xCat3; Lion; Lion2.
*
* INPUTS:
*       devNum          - the device number
*       ecmpEntryIndex  - the index of the entry in the ECMP table
*       ecmpEntryPtr    - (pointer to) the ECMP entry
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong devNum
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS prvCpssDxChLpmHwEcmpEntryWrite
(
    IN GT_U8                                devNum,
    IN GT_U32                               ecmpEntryIndex,
    IN PRV_CPSS_DXCH_LPM_ECMP_ENTRY_STC     *ecmpEntryPtr
);

/*******************************************************************************
* prvCpssDxChLpmHwEcmpEntryRead
*
* DESCRIPTION:
*       Read an ECMP entry
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; xCat2; DxCh2; DxCh3; xCat; xCat3; Lion; Lion2.
*
* INPUTS:
*       devNum          - the device number
*       ecmpEntryIndex  - the index of the entry in the ECMP table
*
* OUTPUTS:
*       ecmpEntryPtr    - (pointer to) the ECMP entry
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong devNum
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS prvCpssDxChLpmHwEcmpEntryRead
(
    IN  GT_U8                               devNum,
    IN  GT_U32                              ecmpEntryIndex,
    OUT PRV_CPSS_DXCH_LPM_ECMP_ENTRY_STC    *ecmpEntryPtr
);
/*******************************************************************************
* prvCpssDxChLpmLeafEntryCalcLpmHwIndexFromSwIndex
*
* DESCRIPTION:
*       Calculate HW Index for an LPM leaf entry
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; xCat2; Lion; Lion2.
*
* INPUTS:
*       devNum                   - the device number
*       leafIndex                - leaf index within the LPM PBR block
*
* OUTPUTS:
*       lpmIndexPtr              - (pointer to) leaf index within the LPM PBR block in HW representation 
*
* RETURNS:
*       GT_OK                    - on success
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvCpssDxChLpmLeafEntryCalcLpmHwIndexFromSwIndex
(
    IN GT_U8            devNum,
    IN GT_U32           leafIndex,
    OUT GT_U32          *lpmIndexPtr
);

/*******************************************************************************
* prvCpssDxChLpmLeafEntryCalcLpmSwIndexFromHwIndex
*
* DESCRIPTION:
*       Calculate SW Index for an LPM leaf HW index
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; xCat2; Lion; Lion2.
*
* INPUTS:
*       devNum                   - the device number
*       leafHwIndex              - leaf HW index within the LPM PBR block in HW representation 
* 
* OUTPUTS:
*       leafSwIndexPtr           - (pointer to)leaf index within the LPM PBR block SW representation
*
* RETURNS:
*       GT_OK                    - on success
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_VOID prvCpssDxChLpmLeafEntryCalcLpmSwIndexFromHwIndex
(
    IN GT_U8            devNum,    
    IN GT_U32           leafHwIndex,
    OUT GT_U32          *leafSwIndexPtr
);

/*******************************************************************************
* prvCpssDxChLpmNextPointerDataBuild
*
* DESCRIPTION:
*       Local function for LPM bucket next pointer HW format build.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; Lion2.
*
* INPUTS:
*       dev                 - dev number
*       nextPointerData     - the next pointer data.
*
* OUTPUTS:
*       hwDataPtr           - node data in HW format
*
* RETURNS:
*       GT_OK          - on success
*       GT_BAD_PARAM   - if any parameter is wrong
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvCpssDxChLpmNextPointerDataBuild
(
    IN  GT_U8                                       dev,
    IN  CPSS_DXCH_LPM_NODE_NEXT_POINTER_STC         nextPointerData,
    OUT GT_U32                                      *hwDataPtr
);

/*******************************************************************************
* prvCpssDxChLpmNextPointerDataDecode
*
* DESCRIPTION:
*       Local function for LPM bucket next pointer HW format build
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; Lion2.
*
* INPUTS:
*       hwDataPtr           - node data in HW format
*
* OUTPUTS:
*       nextPointerDataPtr - (pointer to) next pointer data.
*
* RETURNS:
*       GT_OK              - on success
*       GT_BAD_PARAM       - on any parameter is wrong
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS prvCpssDxChLpmNextPointerDataDecode
(
    IN  GT_U32                                      *hwDataPtr,
    OUT CPSS_DXCH_LPM_NODE_NEXT_POINTER_STC         *nextPointerDataPtr
);
#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __prvCpssDxChLpmHwh */

