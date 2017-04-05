/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* cpssDxChPortEcn.h
*
* DESCRIPTION:
*         CPSS DxCh Port ECN APIs.
*
*
* FILE REVISION NUMBER:
*       $Revision: 3 $
*
*******************************************************************************/
#ifndef __cpssDxChPortEcn
#define __cpssDxChPortEcn

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include <cpss/generic/cpssTypes.h>
#include <cpss/dxCh/dxChxGen/port/cpssDxChPortTx.h>


/*
 * typedef: enum CPSS_DXCH_PORT_ECN_ENABLERS_STC
 *
 * Description:  ECN marking enablers structure.
 *
 * Fields:
 *      tcDpLimit       - enable/disable ECN marking for {Queue,DP} descriptor/buffer limits.
 *
 *      portLimit       - enable/disable ECN marking for Port descriptor/buffer limits.
 *
 *      tcLimit         - enable/disable ECN marking for Queue descriptor/buffer limits.
 *
 *      sharedPoolLimit - enable/disable ECN marking for Shared Pool descriptor/buffer limits.
 *
 */
typedef struct
{
   GT_BOOL  tcDpLimit;
   GT_BOOL  portLimit;
   GT_BOOL  tcLimit;
   GT_BOOL  sharedPoolLimit;
}CPSS_DXCH_PORT_ECN_ENABLERS_STC;



/*******************************************************************************
* cpssDxChPortEcnMarkingEnableSet
*
* DESCRIPTION:
*       Enable/Disable ECN marking.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum  - device number
*       protocolStack - type of IP stack
*       enable  - GT_TRUE: ECN marking enable
*                 GT_FALSE: ECN marking disable
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on other error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong devNum or protocolStack parameter
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChPortEcnMarkingEnableSet
(
    IN GT_U8                               devNum,
    IN CPSS_IP_PROTOCOL_STACK_ENT          protocolStack,
    IN GT_BOOL                             enable
);

/*******************************************************************************
* cpssDxChPortEcnMarkingEnableGet
*
* DESCRIPTION:
*       Get status of ECN marking.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum  - device number
*       protocolStack - type of IP stack
*
* OUTPUTS:
*       enablePtr  - GT_TRUE: ECN marking enable
*                 GT_FALSE: ECN marking disable
*
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on other error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong devNum or protocolStack parameter
*       GT_BAD_PTR               - on NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChPortEcnMarkingEnableGet
(
    IN GT_U8                               devNum,
    IN CPSS_IP_PROTOCOL_STACK_ENT          protocolStack,
    OUT GT_BOOL                            *enablePtr
);


/*******************************************************************************
* cpssDxChPortEcnMarkingTailDropProfileEnableSet
*
* DESCRIPTION:
*       Enable/Disable ECN marking per profile according to Tail Drop limits.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum  - device number
*       profileSet - tail drop profile ID
*       enablersPtr  - (pointer to) struct of enablers
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on other error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong devNum or profileSet
*       GT_BAD_PTR               - on NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChPortEcnMarkingTailDropProfileEnableSet
(
    IN  GT_U8                               devNum,
    IN  CPSS_PORT_TX_DROP_PROFILE_SET_ENT   profileSet,
    IN  CPSS_DXCH_PORT_ECN_ENABLERS_STC     *enablersPtr
);


/*******************************************************************************
* cpssDxChPortEcnMarkingTailDropProfileEnableGet
*
* DESCRIPTION:
*       Get Enable/Disable ECN marking per profile according to Tail Drop limits.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum  - device number
*       profileSet - tail drop profile ID
*
* OUTPUTS:
*       enablersPtr  - (pointer to) struct of enablers
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on other error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong devNum or profileSet
*       GT_BAD_PTR               - on NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChPortEcnMarkingTailDropProfileEnableGet
(
    IN  GT_U8                               devNum,
    IN  CPSS_PORT_TX_DROP_PROFILE_SET_ENT   profileSet,
    OUT CPSS_DXCH_PORT_ECN_ENABLERS_STC     *enablersPtr
);


#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __cpssDxChPortEcn */

