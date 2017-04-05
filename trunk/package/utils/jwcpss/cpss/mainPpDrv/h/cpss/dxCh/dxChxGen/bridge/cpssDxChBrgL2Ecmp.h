/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* cpssDxChBrgL2Ecmp.h
*
* DESCRIPTION:
*       L2 ECMP facility CPSS DxCh implementation.
*
* FILE REVISION NUMBER:
*       $Revision: 10 $
*
*******************************************************************************/
#ifndef __cpssDxChBrgL2Ecmph
#define __cpssDxChBrgL2Ecmph

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include <cpss/generic/cpssTypes.h>
#include <cpss/dxCh/dxChxGen/cpssDxChTypes.h>


 /*
 * Typedef: struct CPSS_DXCH_BRG_L2_ECMP_LTT_ENTRY_STC
 *
 * Description:
 *          L2 ECMP LTT Entry
 *
 * Fields:
 *      ecmpStartIndex  - Determines the start index of the L2 ECMP block.
 *                        (APPLICABLE RANGES: 0..8191)
 *      ecmpNumOfPaths  - The number of paths in the ECMP block:
 *                        0x1 = 1 path, 0x2 = 2 paths and so on.
 *                        (APPLICABLE RANGES: 1..4096)
 *      ecmpEnable      - Determines whether this packet is load balanced over an ECMP group.
 *
 */

typedef struct
{
   GT_U32                                  ecmpStartIndex;
   GT_U32                                  ecmpNumOfPaths;
   GT_BOOL                                 ecmpEnable;

} CPSS_DXCH_BRG_L2_ECMP_LTT_ENTRY_STC;

/*
 * Typedef: struct CPSS_DXCH_BRG_L2_ECMP_ENTRY_STC
 *
 * Description:
 *          L2 ECMP Entry
 *
 * Fields:
 *      targetEport     - target ePort
*       targetHwDevice  - target HW device
 *
 */

typedef struct
{
    GT_PORT_NUM                          targetEport;
    GT_HW_DEV_NUM                        targetHwDevice;
} CPSS_DXCH_BRG_L2_ECMP_ENTRY_STC;

/*******************************************************************************
* cpssDxChBrgL2EcmpEnableSet
*
* DESCRIPTION:
*       Globally enable/disable L2 ECMP (AKA ePort ECMP)
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum      - device number
*       enable      - L2 ECMP status:
*                     GT_TRUE: enable L2 ECMP
*                     GT_FALSE: disable L2 ECMP
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong parameter
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS cpssDxChBrgL2EcmpEnableSet
(
    IN  GT_U8       devNum,
    IN  GT_BOOL     enable
);

/*******************************************************************************
* cpssDxChBrgL2EcmpEnableGet
*
* DESCRIPTION:
*       Get the global enable/disable L2 ECMP (AKA ePort ECMP)
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum      - device number
*
* OUTPUTS:
*       enablePtr   - (pointer to) L2 ECMP  status:
*                     GT_TRUE : L2 ECMP is enabled
*                     GT_FALSE: L2 ECMP is disabled
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - on NULL pointer
*       GT_BAD_PARAM             - on wrong parameter
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS cpssDxChBrgL2EcmpEnableGet
(
    IN  GT_U8       devNum,
    OUT GT_BOOL     *enablePtr
);


/*******************************************************************************
* cpssDxChBrgL2EcmpIndexBaseEportSet
*
* DESCRIPTION:
*       Defines the first ePort number in the L2 ECMP ePort number range.
*       The index to the L2 ECMP LTT is <Target ePort>-<Base ePort>.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum              - device number
*       ecmpIndexBaseEport  - the ePort number
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - on wrong input parameters
*       GT_HW_ERROR              - failed to write to hardware
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS cpssDxChBrgL2EcmpIndexBaseEportSet
(
    IN GT_U8            devNum,
    IN GT_PORT_NUM      ecmpIndexBaseEport
);

/*******************************************************************************
* cpssDxChBrgL2EcmpIndexBaseEportGet
*
* DESCRIPTION:
*       Return the first ePort number in the L2 ECMP ePort number range.
*       The index to the L2 ECMP LTT is <Target ePort>-<Base ePort>.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum      - device number
*
* OUTPUTS:
*       ecmpIndexBaseEportPtr  - (pointer to) the ePort number
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - on wrong input parameters
*       GT_HW_ERROR              - failed to write to hardware
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChBrgL2EcmpIndexBaseEportGet
(
    IN  GT_U8           devNum,
    OUT GT_PORT_NUM     *ecmpIndexBaseEportPtr
);

/*******************************************************************************
* cpssDxChBrgL2EcmpLttTableSet
*
* DESCRIPTION:
*       Set L2 ECMP LTT entry info.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum          - device number
*       index           - the LTT index
*       ecmpLttInfoPtr - (pointer to) L2 ECMP LTT Information
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong parameters
*       GT_OUT_OF_RANGE          - on out of range ecmpStartIndex or ecmpNumOfPaths
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       The index to the LTT is the result of Target ePort - Base ePort.
*       Base ePort may be configured by cpssDxChBrgL2EcmpIndexBaseEportSet.
*
*******************************************************************************/
GT_STATUS cpssDxChBrgL2EcmpLttTableSet
(
    IN GT_U8                                devNum,
    IN GT_U32                               index,
    IN CPSS_DXCH_BRG_L2_ECMP_LTT_ENTRY_STC  *ecmpLttInfoPtr
);

/*******************************************************************************
* cpssDxChBrgL2EcmpLttTableGet
*
* DESCRIPTION:
*       Get L2 ECMP LTT entry info.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum          - device number
*       index           - the LTT index
*
* OUTPUTS:
*       ecmpLttInfoPtr - (pointer to) L2 ECMP LTT Information
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong parameters
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       The index to the LTT is the result of Target ePort - Base ePort.
*       Base ePort may be configured by cpssDxChBrgL2EcmpIndexBaseEportSet.
*
*******************************************************************************/
GT_STATUS cpssDxChBrgL2EcmpLttTableGet
(
    IN  GT_U8                                devNum,
    IN  GT_U32                               index,
    OUT CPSS_DXCH_BRG_L2_ECMP_LTT_ENTRY_STC  *ecmpLttInfoPtr
);

/*******************************************************************************
* cpssDxChBrgL2EcmpTableSet
*
* DESCRIPTION:
*       Set L2 ECMP entry info.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum          - device number
*       index           - the ECMP entry index
*       ecmpEntryPtr    - (pointer to) L2 ECMP entry.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong parameters
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*      Index according to index from L2 ECMP LTT
*
*******************************************************************************/
GT_STATUS cpssDxChBrgL2EcmpTableSet
(
    IN GT_U8                                devNum,
    IN GT_U32                               index,
    IN CPSS_DXCH_BRG_L2_ECMP_ENTRY_STC     *ecmpEntryPtr
);

/*******************************************************************************
* cpssDxChBrgL2EcmpTableGet
*
* DESCRIPTION:
*       Get L2 ECMP entry info.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum          - device number
*       index           - the ECMP entry index
*
* OUTPUTS:
*       ecmpEntryPtr    - (pointer to) L2 ECMP entry.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong parameters
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       Index according to index from L2 ECMP LTT
*
*******************************************************************************/
GT_STATUS cpssDxChBrgL2EcmpTableGet
(
    IN  GT_U8                                devNum,
    IN  GT_U32                               index,
    OUT CPSS_DXCH_BRG_L2_ECMP_ENTRY_STC     *ecmpEntryPtr
);

/*******************************************************************************
* cpssDxChBrgL2EcmpMemberSelectionModeSet
*
* DESCRIPTION:
*       Set mode used to calculate the Index of the secondary ePort (L2 ECMP member)
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum          - device number
*       selectionMode   - member selection mode
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - on wrong input parameters
*       GT_HW_ERROR              - failed to write to hardware
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS cpssDxChBrgL2EcmpMemberSelectionModeSet
(
    IN GT_U8                                    devNum,
    IN CPSS_DXCH_MEMBER_SELECTION_MODE_ENT      selectionMode
);

/*******************************************************************************
* cpssDxChBrgL2EcmpMemberSelectionModeGet
*
* DESCRIPTION:
*       Get mode used to calculate the Index of the secondary ePort (L2 ECMP member)
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum              - device number
*
* OUTPUTS:
*       selectionModePtr   - (pointer to) member selection mode
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - on wrong input parameters
*       GT_HW_ERROR              - failed to write to hardware
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChBrgL2EcmpMemberSelectionModeGet
(
    IN  GT_U8                                   devNum,
    OUT CPSS_DXCH_MEMBER_SELECTION_MODE_ENT     *selectionModePtr
);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __cpssDxChBrgL2Ecmph */

