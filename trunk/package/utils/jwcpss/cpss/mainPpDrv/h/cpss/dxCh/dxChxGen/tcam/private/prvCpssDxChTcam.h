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
* prvCpssDxChTcam.h
*
* DESCRIPTION:
*       Common private TCAM declarations.
*
* DEPENDENCIES:
*       None.
*
* FILE REVISION NUMBER:
*       $Revision: 2 $
*
*******************************************************************************/
#ifndef __prvCpssDxChTcamh
#define __prvCpssDxChTcamh

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include <cpss/generic/cpssTypes.h>
#include <cpss/dxCh/dxChxGen/tcam/cpssDxChTcam.h>

#define PRV_CPSS_LION3_TCAM_MULTI_PORT_GROUPS_BMP_CHECK_MAC(devNum,portGroupsBmp)                                               \
    if(PRV_CPSS_IS_MULTI_PORT_GROUPS_DEVICE_MAC(devNum) && (PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_MAC(devNum) == GT_TRUE))  \
    {                                                                                                                           \
        /* only port groups (0,4) supported */                                                                                  \
        if(((portGroupsBmp & 0xF) != 0xF) && (((portGroupsBmp >> 4) & 0xF) != 0xF))                                             \
        {                                                                                                                       \
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);                                                                                                \
        }                                                                                                                       \
        if(portGroupsBmp & 0xF)                                                                                                 \
        {                                                                                                                       \
             portGroupsBmp = portGroupsBmp & 0xF1;                                                                              \
        }                                                                                                                       \
        if((portGroupsBmp >> 4) & 0xF)                                                                                          \
        {                                                                                                                       \
             portGroupsBmp = portGroupsBmp & 0x1F;                                                                              \
        }                                                                                                                       \
    }

/*******************************************************************************
* prvCpssDxChTcamRuleIndexToEntryNumber
*
* DESCRIPTION:
*       Converts TCAM rule global index to entry number.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum            - the device number
*       index             - global line index in TCAM to write to.
*
* OUTPUTS:
*       entryNumberXPtr   - (pointer to) entry number in TCAM for X format
*       entryNumberYPtr   - (pointer to) entry number in TCAM for Y format
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - on bad parameter, and when the rule cannot start in a given index.
*       GT_BAD_PTR               - on NULL pointer
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS prvCpssDxChTcamRuleIndexToEntryNumber
(
    IN  GT_U8                               devNum,
    IN  GT_U32                              index,
    OUT GT_U32                              *entryNumberXPtr,
    OUT GT_U32                              *entryNumberYPtr
);

/*******************************************************************************
* prvCpssDxChTcamRuleIndexCheck
*
* DESCRIPTION:
*       Checks if an gloabl TCAM index is valid for a TCAM rule's size.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum            - the device number
*       index             - global line index in TCAM to write to.
*       ruleSize          - size of rule
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - on bad parameter, and when the rule cannot start in a given index.
*       GT_BAD_PTR               - on NULL pointer
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS prvCpssDxChTcamRuleIndexCheck
(
    IN  GT_U8                               devNum,
    IN  GT_U32                              index,
    CPSS_DXCH_TCAM_RULE_SIZE_ENT            ruleSize
);

/*******************************************************************************
* prvCpssDxChTcamTtiRuleBaseOffsetGet
*
* DESCRIPTION:
*       Gets TCAM base index for client TTI
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
* INPUTS:
*       devNum - device number
*
* OUTPUTS:
*       None
*
* RETURNS:
*       The offset to add to the TTI rule index in TCAM
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_U32 prvCpssDxChTcamTtiRuleBaseOffsetGet
(
    IN     GT_U8               devNum
);

/*******************************************************************************
* prvCpssDxChTcamPortGroupRuleReadEntry
*
* DESCRIPTION:
*       Reads the TCAM pattern/mask of single bank.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum            - the device number
*       portGroupsBmp     - bitmap of Port Groups.
*                        NOTEs:
*                         1. for non multi-port groups device this parameter is IGNORED.
*                         2. for multi-port groups device :
*                            bitmap must be set with at least one bit representing
*                            valid port group(s). If a bit of non valid port group
*                            is set then function returns GT_BAD_PARAM.
*                            value CPSS_PORT_GROUP_UNAWARE_MODE_CNS is supported.
*       index             - global line index in TCAM to write to.
*
* OUTPUTS:
*       validPtr          - (pointer to) whether TCAM entry is valid or not
*       ruleSizePtr       - (pointer to) size of rule
*       tcamEntryPatternPtr - points to the TCAM rule's pattern.
*       tcamEntryMaskPtr  - points to the TCAM rule's mask.
*                           The rule mask is "AND STYLED ONE".
*                           Mask bit's 0 and pattern bit's 0 means don't care bit
*                           (corresponding bit in the pattern is not used in the TCAM lookup).
*                           Mask bit's 0 and pattern bit's 1 means ALWAYS MISS (full entry miss).
*                           Mask bit's 1 means that corresponding bit in the pattern
*                           is using in the TCAM lookup.

*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - on bad parameter, and when the rule cannot start in a given index.
*       GT_BAD_STATE             - if in TCAM found rule of size different from the specified
*       GT_BAD_PTR               - on NULL pointer
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS prvCpssDxChTcamPortGroupRuleReadEntry
(
    IN  GT_U8                               devNum,
    IN  GT_PORT_GROUPS_BMP                  portGroupsBmp,
    IN  GT_U32                              index,
    OUT GT_BOOL                             *validPtr,
    OUT CPSS_DXCH_TCAM_RULE_SIZE_ENT        *ruleSizePtr,
    OUT GT_U32                              *tcamEntryPatternPtr,
    OUT GT_U32                              *tcamEntryMaskPtr
);

/*******************************************************************************
* prvCpssDxChTcamPortGroupRuleWriteEntry
*
* DESCRIPTION:
*       Writes the TCAM mask and key of a single bank.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum            - the device number
*       portGroupsBmp     - bitmap of Port Groups.
*                        NOTEs:
*                         1. for non multi-port groups device this parameter is IGNORED.
*                         2. for multi-port groups device :
*                            bitmap must be set with at least one bit representing
*                            valid port group(s). If a bit of non valid port group
*                            is set then function returns GT_BAD_PARAM.
*                            value CPSS_PORT_GROUP_UNAWARE_MODE_CNS is supported.
*       index             - global line index in TCAM to write to.
*       valid             - whether TCAM entry is valid or not
*       ruleSize          - size of rule
*       tcamEmtryPatternPtr - points to the TCAM rule's pattern.
*       tcamEntryMaskPtr  - points to the TCAM rule's mask.
*                           The rule mask is "AND STYLED ONE".
*                           Mask bit's 0 and pattern bit's 0 means don't care bit
*                           (corresponding bit in the pattern is not used in the TCAM lookup).
*                           Mask bit's 0 and pattern bit's 1 means ALWAYS MISS (full entry miss).
*                           Mask bit's 1 means that corresponding bit in the pattern
*                           is using in the TCAM lookup.
*
* OUTPUTS:
*       none.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - on bad parameter, and when the rule cannot start in a given index.
*       GT_BAD_PTR               - on NULL pointer
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS prvCpssDxChTcamPortGroupRuleWriteEntry
(
    IN  GT_U8                               devNum,
    IN  GT_PORT_GROUPS_BMP                  portGroupsBmp,
    IN  GT_U32                              index,
    IN  GT_BOOL                             valid,
    IN  CPSS_DXCH_TCAM_RULE_SIZE_ENT        ruleSize,
    IN  GT_U32                              *tcamEntryPatternPtr,
    IN  GT_U32                              *tcamEntryMaskPtr
);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __prvCpssDxChTcamh */
