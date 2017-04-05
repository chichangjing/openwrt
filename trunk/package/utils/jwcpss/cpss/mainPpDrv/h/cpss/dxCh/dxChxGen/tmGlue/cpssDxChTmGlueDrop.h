/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
*
* cpssDxChTmGlueDrop.h
*
* DESCRIPTION:
*       Traffic Manager Glue - TM drop.
*
* FILE REVISION NUMBER:
*       $Revision: 3 $
*
*******************************************************************************/

#ifndef __cpssDxChTmGlueDroph
#define __cpssDxChTmGlueDroph

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include <cpss/generic/cpssTypes.h>

/**********************************************************************
 * typedef: enumeration CPSS_DXCH_TM_GLUE_DROP_RED_PACKET_DROP_MODE_ENT
 *
 * Description:
 *      This mode defines whether to drop all RED packets or 
 *      to drop RED packets according to the Traffic Manager recommendation.
 *
 * Enumerations:
 *   CPSS_DXCH_TM_GLUE_DROP_RED_PACKET_DROP_MODE_DROP_ALL_RED_E -
 *         Drop all RED packets.
 *   CPSS_DXCH_TM_GLUE_DROP_RED_PACKET_DROP_MODE_ASK_TM_RESP_E -
 *         Drop RED packets according to TM response
 *
 * COMMENTS:
 ***********************************************************************/
typedef enum
{
    CPSS_DXCH_TM_GLUE_DROP_RED_PACKET_DROP_MODE_DROP_ALL_RED_E,
    CPSS_DXCH_TM_GLUE_DROP_RED_PACKET_DROP_MODE_ASK_TM_RESP_E
} CPSS_DXCH_TM_GLUE_DROP_RED_PACKET_DROP_MODE_ENT;

/**********************************************************************
 * typedef: struct CPSS_DXCH_TM_GLUE_DROP_MASK_STC
 *
 * Description:
 *      Configuration for TM drop recommendation.
 *
 * Fields:
 *   qTailDropUnmask     - indicates whether to use the Q tail drop 
 *                       recommendation for the dropping decision.
 *                       GT_FALSE = Mask. Do not consider for Drop.
 *                       GT_TRUE  = UnMask. Consider for Drop. 
 *   qWredDropUnmask     - indicates whether to use the QWRED drop 
 *                       recommendation for the dropping decision.
 *                       GT_FALSE = Mask. Do not consider for Drop.
 *                       GT_TRUE  = UnMask. Consider for Drop. 
 *   aTailDropUnmask     - indicates whether to use the A node tail 
 *                       drop recommendation for the dropping decision.
 *                       GT_FALSE = Mask. Do not consider for Drop.
 *                       GT_TRUE  = UnMask. Consider for Drop. 
 *   aWredDropUnmask     - indicates whether to use the A node WRED drop 
 *                       recommendation for the dropping decision.
 *                       GT_FALSE = Mask. Do not consider for Drop.
 *                       GT_TRUE  = UnMask. Consider for Drop. 
 *   bTailDropUnmask     - indicates whether to use the B node tail drop 
 *                       recommendation for the dropping decision.
 *                       GT_FALSE = Mask. Do not consider for Drop.
 *                       GT_TRUE  = UnMask. Consider for Drop. 
 *   bWredDropUnmask     - indicates whether to use a B node WRED drop 
 *                       recommendation for the dropping decision.
 *                       GT_FALSE = Mask. Do not consider for Drop.
 *                       GT_TRUE  = UnMask. Consider for Drop. 
 *   cTailDropUnmask     - indicates whether to use a C node tail drop 
 *                       recommendation for the dropping decision.
 *                       GT_FALSE = Mask. Do not consider for Drop.
 *                       GT_TRUE  = UnMask. Consider for Drop. 
 *   cWredDropUnmask     - indicates whether to use a C node WRED drop 
 *                       recommendation for the dropping decision.
 *                       GT_FALSE = Mask. Do not consider for Drop.
 *                       GT_TRUE  = UnMask. Consider for Drop. 
 *   portTailDropUnmask  - indicates whether to use the Port tail drop 
 *                       recommendation for the dropping decision.
 *                       GT_FALSE = Mask. Do not consider for Drop.
 *                       GT_TRUE  = UnMask. Consider for Drop. 
 *   portWredDropUnmask  - indicates whether to use the Port WRED drop 
 *                       recommendation for the dropping decision.
 *                       GT_FALSE = Mask. Do not consider for Drop.
 *                       GT_TRUE  = UnMask. Consider for Drop. 
 *   outOfResourceDropUnmask  - indicates whether to use the OOR 
 *                       (Out of resources) for the dropping decision.
 *                       GT_FALSE = Mask. Do not consider for Drop.
 *                       GT_TRUE  = UnMask. Consider for Drop. 
 *   redPacketsDropMode  - defines whether to drop all RED packets 
 *                       or to drop RED packets according to the TM recommendation.
 *
 * COMMENTS:
 ***********************************************************************/
typedef struct
{
    GT_BOOL         qTailDropUnmask;
    GT_BOOL         qWredDropUnmask;
    GT_BOOL         aTailDropUnmask;
    GT_BOOL         aWredDropUnmask;
    GT_BOOL         bTailDropUnmask;
    GT_BOOL         bWredDropUnmask;
    GT_BOOL         cTailDropUnmask;
    GT_BOOL         cWredDropUnmask;
    GT_BOOL         portTailDropUnmask;
    GT_BOOL         portWredDropUnmask;
    GT_BOOL         outOfResourceDropUnmask;
    CPSS_DXCH_TM_GLUE_DROP_RED_PACKET_DROP_MODE_ENT redPacketsDropMode;
}CPSS_DXCH_TM_GLUE_DROP_MASK_STC;


/*******************************************************************************
* cpssDxChTmGlueDropQueueProfileIdSet
*
* DESCRIPTION:
*   The function maps every Queue Id to its Profile Id.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat3.
*
* INPUTS:
*       devNum             - device number
*       queueId            - TM queue id 
*                            (APPLICABLE RANGES: 0..16383).
*       profileId          - per queue profile Id
*                            (APPLICABLE RANGES: 0..7).
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_OUT_OF_RANGE          - on wrong profile id
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChTmGlueDropQueueProfileIdSet
(
    IN  GT_U8     devNum,
    IN  GT_U32    queueId,
    IN  GT_U32    profileId
);

/*******************************************************************************
* cpssDxChTmGlueDropQueueProfileIdGet
*
* DESCRIPTION:
*   The function gets the Profile Id of a given QueueId.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat3.
*
* INPUTS:
*       devNum             - device number
*       queueId            - TM queue id 
*                            (APPLICABLE RANGES: 0..16383).
*
* OUTPUTS:
*       profileIdPtr       - (pointer to) profile Id per Queue
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_BAD_PTR               - on NULL pointer
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChTmGlueDropQueueProfileIdGet
(
    IN  GT_U8     devNum,
    IN  GT_U32    queueId,
    OUT GT_U32   *profileIdPtr
);

/*******************************************************************************
* cpssDxChTmGlueDropProfileDropMaskSet
* 
* DESCRIPTION:
*   The function configures the Drop Masking.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat3.
*
* INPUTS:
*       devNum             - device number
*       profileId          - configuration per profile Id
*                            (APPLICABLE RANGES: 0..7).
*       tc                 - TM traffic class 
*                            (APPLICABLE RANGES: 0..15).
*       dropMaskCfgPtr     - (pointer to) drop mask configuration
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChTmGlueDropProfileDropMaskSet
(
    IN  GT_U8                                    devNum,
    IN  GT_U32                                   profileId,
    IN  GT_U32                                   tc,
    IN  CPSS_DXCH_TM_GLUE_DROP_MASK_STC         *dropMaskCfgPtr
);

/*******************************************************************************
* cpssDxChTmGlueDropProfileDropMaskGet
*
* DESCRIPTION:
*   The function gets the Drop Mask configuration.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat3.
*
* INPUTS:
*       devNum             - device number
*       profileId          - configuration per profile Id
*                            (APPLICABLE RANGES: 0..7).
*       tc                 - TM traffic class 
*                            (APPLICABLE RANGES: 0..15).
*
* OUTPUTS:
*       dropMaskCfgPtr     - (pointer to) structure for drop mask configuration
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_BAD_PTR               - on NULL pointer
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChTmGlueDropProfileDropMaskGet
(
    IN  GT_U8                                    devNum,
    IN  GT_U32                                   profileId,
    IN  GT_U32                                   tc,
    OUT CPSS_DXCH_TM_GLUE_DROP_MASK_STC         *dropMaskCfgPtr
);


/*******************************************************************************
* cpssDxChTmGlueDropTcToCosSet
* 
* DESCRIPTION:
*   The function configures TM TC to COS mapping table.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat3.
*
* INPUTS:
*       devNum             - device number
*       tmTc               - TM traffic class
*                            (APPLICABLE RANGES: 0..15).
*       cos                - mapped COS value for TM-TC
*                            (APPLICABLE RANGES: 0..7).
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_OUT_OF_RANGE          - on wrong cos
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChTmGlueDropTcToCosSet
(
    IN  GT_U8     devNum,
    IN  GT_U32    tmTc,
    IN  GT_U32    cos
);

/*******************************************************************************
* cpssDxChTmGlueDropTcToCosGet
*
* DESCRIPTION:
*   The function retrieves the TM TC to COS mapping COS value.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat3.
*
* INPUTS:
*       devNum             - device number
*       tmTc               - TM traffic class
*                            (APPLICABLE RANGES: 0..15).
*
* OUTPUTS:
*       cosPtr             - (pointer to) mapped COS value for TM-TC
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_BAD_PTR               - on NULL pointer
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChTmGlueDropTcToCosGet
(
    IN  GT_U8     devNum,
    IN  GT_U32    tmTc,
    OUT GT_U32   *cosPtr
);


#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __cpssDxChTmGlueDroph */


