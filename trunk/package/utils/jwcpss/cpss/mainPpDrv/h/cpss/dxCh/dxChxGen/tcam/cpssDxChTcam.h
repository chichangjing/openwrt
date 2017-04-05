/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* cpssDxChTcam.h
*
* DESCRIPTION:
*       CPSS TCAM declarations.
*
* DEPENDENCIES:
*       None.
*
* FILE REVISION NUMBER:
*       $Revision: 11 $
*
*******************************************************************************/

#ifndef __cpssDxChTcamh
#define __cpssDxChTcamh

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include <cpss/generic/cpssTypes.h>
#include <cpss/generic/cpssCommonDefs.h>

/* max number of floors in TCAM */
#define CPSS_DXCH_TCAM_MAX_NUM_FLOORS_CNS  12

/* max number of banks in TCAM */
#define CPSS_DXCH_TCAM_MAX_NUM_BANKS_CNS   12

/* max number of rules per bank in TCAM */
#define CPSS_DXCH_TCAM_MAX_NUM_RULES_PER_BANKS_CNS   256

/* max number of blocks in TCAM */
#define CPSS_DXCH_TCAM_MAX_NUM_BLOCKS_CNS   6

/* max number of rules per half floor in TCAM (1536) */
#define CPSS_DXCH_TCAM_MAX_NUM_RULES_PER_HALF_FLOOR_CNS   (CPSS_DXCH_TCAM_MAX_NUM_RULES_PER_BANKS_CNS*(CPSS_DXCH_TCAM_MAX_NUM_BANKS_CNS / 2))

/* max number of rules per floor in TCAM (3072) */
#define CPSS_DXCH_TCAM_MAX_NUM_RULES_PER_FLOOR_CNS   (CPSS_DXCH_TCAM_MAX_NUM_RULES_PER_BANKS_CNS*CPSS_DXCH_TCAM_MAX_NUM_BANKS_CNS)

/* max number of rules in TCAM (36864) */
#define CPSS_DXCH_TCAM_MAX_NUM_RULES_CNS   (CPSS_DXCH_TCAM_MAX_NUM_RULES_PER_BANKS_CNS*CPSS_DXCH_TCAM_MAX_NUM_FLOORS_CNS*CPSS_DXCH_TCAM_MAX_NUM_BANKS_CNS)

/* Old CPSS versions used obsolete name CPSS_DXCH_TCAM_HALF_FLOOR_INFO_STC. New versions use new name CPSS_DXCH_TCAM_BLOCK_INFO_STC. */
#define CPSS_DXCH_TCAM_HALF_FLOOR_INFO_STC CPSS_DXCH_TCAM_BLOCK_INFO_STC

/* macro to validate the value of global line index in TCAM
    use fineTuning.tableSize.tunnelTerm for the check
*/
#define PRV_CPSS_DXCH_TCAM_CHECK_LINE_INDEX_MAC(devNum,index)      \
    if ((index) >= PRV_CPSS_DXCH_PP_MAC(devNum)->fineTuning.tableSize.tunnelTerm) \
    {                                                              \
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);                                       \
    }

/* Converts rule size to number of TCAM banks value */
#define PRV_CPSS_DXCH_TCAM_CONVERT_RULE_SIZE_TO_NUM_OF_BANKS_VAL_MAC(_numOfBanks, _ruleSize)\
    switch (_ruleSize)                                                                      \
    {                                                                                       \
        case CPSS_DXCH_TCAM_RULE_SIZE_10_B_E:                                               \
            _numOfBanks = 1;                                                                \
            break;                                                                          \
        case CPSS_DXCH_TCAM_RULE_SIZE_20_B_E:                                               \
            _numOfBanks = 2;                                                                \
            break;                                                                          \
        case CPSS_DXCH_TCAM_RULE_SIZE_30_B_E:                                               \
            _numOfBanks = 3;                                                                \
            break;                                                                          \
        case CPSS_DXCH_TCAM_RULE_SIZE_40_B_E:                                               \
            _numOfBanks = 4;                                                                \
            break;                                                                          \
        case CPSS_DXCH_TCAM_RULE_SIZE_50_B_E:                                               \
            _numOfBanks = 5;                                                                \
            break;                                                                          \
        case CPSS_DXCH_TCAM_RULE_SIZE_60_B_E:                                               \
            _numOfBanks = 6;                                                                \
            break;                                                                          \
        case CPSS_DXCH_TCAM_RULE_SIZE_80_B_E:                                               \
            _numOfBanks = 8;                                                                \
            break;                                                                          \
        default:                                                                            \
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);                                                            \
    }

/* Converts number of TCAM banks to rule size value */
#define PRV_CPSS_DXCH_TCAM_CONVERT_NUM_OF_BANKS_TO_RULE_SIZE_VAL_MAC(_ruleSize, _numOfBanks)\
    switch (_numOfBanks)                                                                    \
    {                                                                                       \
        case 1:                                                                             \
            _ruleSize = CPSS_DXCH_TCAM_RULE_SIZE_10_B_E;                                    \
            break;                                                                          \
        case 2:                                                                             \
            _ruleSize = CPSS_DXCH_TCAM_RULE_SIZE_20_B_E;                                    \
            break;                                                                          \
        case 3:                                                                             \
            _ruleSize = CPSS_DXCH_TCAM_RULE_SIZE_30_B_E;                                    \
            break;                                                                          \
        case 4:                                                                             \
            _ruleSize = CPSS_DXCH_TCAM_RULE_SIZE_40_B_E;                                    \
            break;                                                                          \
        case 5:                                                                             \
            _ruleSize = CPSS_DXCH_TCAM_RULE_SIZE_50_B_E;                                    \
            break;                                                                          \
        case 6:                                                                             \
            _ruleSize = CPSS_DXCH_TCAM_RULE_SIZE_60_B_E;                                    \
            break;                                                                          \
        case 8:                                                                             \
            _ruleSize = CPSS_DXCH_TCAM_RULE_SIZE_80_B_E;                                    \
            break;                                                                          \
        default:                                                                            \
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);                                                            \
    }

/* Converts rule size to key size value */
#define PRV_CPSS_DXCH_TCAM_CONVERT_RULE_SIZE_TO_KEY_SIZE_VAL_MAC(_keySize, _ruleSize)       \
    switch (_ruleSize)                                                                      \
    {                                                                                       \
        case CPSS_DXCH_TCAM_RULE_SIZE_10_B_E:                                               \
            _keySize = 0;                                                                   \
            break;                                                                          \
        case CPSS_DXCH_TCAM_RULE_SIZE_20_B_E:                                               \
            _keySize = 1;                                                                   \
            break;                                                                          \
        case CPSS_DXCH_TCAM_RULE_SIZE_30_B_E:                                               \
            _keySize = 2;                                                                   \
            break;                                                                          \
        case CPSS_DXCH_TCAM_RULE_SIZE_40_B_E:                                               \
            _keySize = 3;                                                                   \
            break;                                                                          \
        case CPSS_DXCH_TCAM_RULE_SIZE_50_B_E:                                               \
            _keySize = 4;                                                                   \
            break;                                                                          \
        case CPSS_DXCH_TCAM_RULE_SIZE_60_B_E:                                               \
            _keySize = 5;                                                                   \
            break;                                                                          \
        case CPSS_DXCH_TCAM_RULE_SIZE_80_B_E:                                               \
            _keySize = 7;                                                                   \
            break;                                                                          \
        default:                                                                            \
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);                                                            \
    }

/* Converts key size to rule size value */
#define PRV_CPSS_DXCH_TCAM_CONVERT_KEY_SIZE_TO_RULE_SIZE_VAL_MAC(_ruleSize, _keySize)       \
    switch (_keySize)                                                                       \
    {                                                                                       \
        case 0:                                                                             \
            _ruleSize = CPSS_DXCH_TCAM_RULE_SIZE_10_B_E;                                    \
            break;                                                                          \
        case 1:                                                                             \
            _ruleSize = CPSS_DXCH_TCAM_RULE_SIZE_20_B_E;                                    \
            break;                                                                          \
        case 2:                                                                             \
            _ruleSize = CPSS_DXCH_TCAM_RULE_SIZE_30_B_E;                                    \
            break;                                                                          \
        case 3:                                                                             \
            _ruleSize = CPSS_DXCH_TCAM_RULE_SIZE_40_B_E;                                    \
            break;                                                                          \
        case 4:                                                                             \
            _ruleSize = CPSS_DXCH_TCAM_RULE_SIZE_50_B_E;                                    \
            break;                                                                          \
        case 5:                                                                             \
            _ruleSize = CPSS_DXCH_TCAM_RULE_SIZE_60_B_E;                                    \
            break;                                                                          \
        case 7:                                                                             \
            _ruleSize = CPSS_DXCH_TCAM_RULE_SIZE_80_B_E;                                    \
            break;                                                                          \
        default:                                                                            \
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);                                                            \
    }

/* Converts TCAM client to client id value */
#define PRV_CPSS_DXCH_TCAM_CONVERT_CLIENT_TO_CLIENT_ID_VAL_MAC(_clientId, _tcamClient)\
    switch (_tcamClient)                                                              \
    {                                                                                 \
        case CPSS_DXCH_TCAM_IPCL_0_E:                                                 \
            _clientId = 1;                                                            \
            break;                                                                    \
        case CPSS_DXCH_TCAM_IPCL_1_E:                                                 \
            _clientId = 2;                                                            \
            break;                                                                    \
        case CPSS_DXCH_TCAM_IPCL_2_E:                                                 \
            _clientId = 3;                                                            \
            break;                                                                    \
        case CPSS_DXCH_TCAM_EPCL_E:                                                   \
            _clientId = 4;                                                            \
            break;                                                                    \
        case CPSS_DXCH_TCAM_TTI_E:                                                    \
            _clientId = 0;                                                            \
            break;                                                                    \
        default:                                                                      \
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);                                                      \
    }

/* check that TCAM group id is in valid range (APPLICABLE RANGES: Bobcat2, Caelum: 0..2, Bobcat3: 0..4)   */
#define PRV_CPSS_DXCH_TCAM_GROUP_ID_DEV_CHECK_MAC(_devNum,_tcamGroup)                 \
    switch (PRV_CPSS_PP_MAC(_devNum)->devFamily)                                      \
    {                                                                                 \
    case CPSS_PP_FAMILY_DXCH_BOBCAT2_E:                                               \
    case CPSS_PP_FAMILY_DXCH_ALDRIN_E:                                                \
        if (_tcamGroup > 2)                                                           \
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_SUPPORTED, LOG_ERROR_NO_MSG);                                                  \
        break;                                                                        \
    case CPSS_PP_FAMILY_DXCH_BOBCAT3_E:                                               \
        if (_tcamGroup > 4)/*TBD*/                                                    \
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_SUPPORTED, LOG_ERROR_NO_MSG);                                                  \
        break;                                                                        \
    default:                                                                          \
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);                                                          \
    }

/* check TCAM hit number range   */
#define PRV_CPSS_DXCH_TCAM_HIT_NUM_CHECK_MAC(_hitNum)                                 \
    if ((_hitNum < 0) || (_hitNum > 3))                                               \
    {                                                                                 \
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);                                                          \
    }

/* check TCAM index check range */
#define PRV_CPSS_DXCH_TCAM_INDEX_CHECK_MAC(_startIndex,_maxIndex,_hop)                       \
    if ((_startIndex < 0) || (_startIndex > _maxIndex) || ((_startIndex % _hop) != 0))       \
    {                                                                                        \
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);                                                                 \
    }

/*
 * typedef: enum CPSS_DXCH_TCAM_RULE_SIZE_ENT
 *
 * Description:
 *      TCAM rule size.
 *
 * Enumerations:
 *
 *      CPSS_DXCH_TCAM_RULE_SIZE_10_B_E - TCAM rule size to be used 10 Bytes
 *      CPSS_DXCH_TCAM_RULE_SIZE_20_B_E - TCAM rule size to be used 20 Bytes
 *      CPSS_DXCH_TCAM_RULE_SIZE_30_B_E - TCAM rule size to be used 30 Bytes
 *      CPSS_DXCH_TCAM_RULE_SIZE_40_B_E - TCAM rule size to be used 40 Bytes
 *      CPSS_DXCH_TCAM_RULE_SIZE_50_B_E - TCAM rule size to be used 50 Bytes
 *      CPSS_DXCH_TCAM_RULE_SIZE_60_B_E - TCAM rule size to be used 60 Bytes
 *      CPSS_DXCH_TCAM_RULE_SIZE_80_B_E - TCAM rule size to be used 80 Bytes
 *
 */
typedef enum
{
    CPSS_DXCH_TCAM_RULE_SIZE_10_B_E,
    CPSS_DXCH_TCAM_RULE_SIZE_20_B_E,
    CPSS_DXCH_TCAM_RULE_SIZE_30_B_E,
    CPSS_DXCH_TCAM_RULE_SIZE_40_B_E,
    CPSS_DXCH_TCAM_RULE_SIZE_50_B_E,
    CPSS_DXCH_TCAM_RULE_SIZE_60_B_E,
    CPSS_DXCH_TCAM_RULE_SIZE_80_B_E
} CPSS_DXCH_TCAM_RULE_SIZE_ENT;

/*
 * typedef: enum CPSS_DXCH_TCAM_CLIENT_ENT
 *
 * Description:
 *      TCAM clients.
 *
 * Enumerations:
 *
 *      CPSS_DXCH_TCAM_IPCL_0_E - TCAM client ingress policy 0 (first lookup)
 *      CPSS_DXCH_TCAM_IPCL_1_E - TCAM client ingress policy 1 (second lookup)
 *      CPSS_DXCH_TCAM_IPCL_2_E - TCAM client ingress policy 2 (third lookup)
 *      CPSS_DXCH_TCAM_EPCL_E   - TCAM client egress policy
 *      CPSS_DXCH_TCAM_TTI_E    - TCAM client tunnel termination
 *
 */
typedef enum
{
    CPSS_DXCH_TCAM_IPCL_0_E,
    CPSS_DXCH_TCAM_IPCL_1_E,
    CPSS_DXCH_TCAM_IPCL_2_E,
    CPSS_DXCH_TCAM_EPCL_E,
    CPSS_DXCH_TCAM_TTI_E
} CPSS_DXCH_TCAM_CLIENT_ENT;


/*
 * Typedef: structure CPSS_DXCH_TCAM_BLOCK_INFO_STC
 *
 * Description: Defines the group id and lookup Number for TCAM block
 *              (Bobcat2, Bobcat3: banks 0-5 or 6-11, Caelum: banks 0-1, 2-3, 4-5 .. 10-11 )
 *
 * Fields:
 *       group           -  TCAM group id (APPLICABLE RANGES: Bobcat2, Caelum: 0..2, Bobcat3: 0..4)
 *       hitNum         -  lookup Number (APPLICABLE RANGES: 0..3)
 *
*/
typedef struct{
    GT_U32    group;
    GT_U32    hitNum;
} CPSS_DXCH_TCAM_BLOCK_INFO_STC;

/* Defines the bmp banks power up. up to 12 banks*/
typedef GT_U32  GT_FLOOR_BANK_BMP;
/*******************************************************************************
* cpssDxChTcamPortGroupRuleWrite
*
* DESCRIPTION:
*       Writes the TCAM mask and key. Writing operation is preformed on all
*       bits in the line.
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
*       tcamPatternPtr    - points to the TCAM rule's pattern.
*       tcamMaskPtr       - points to the TCAM rule's mask.
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
GT_STATUS cpssDxChTcamPortGroupRuleWrite
(
    IN  GT_U8                               devNum,
    IN  GT_PORT_GROUPS_BMP                  portGroupsBmp,
    IN  GT_U32                              index,
    IN  GT_BOOL                             valid,
    IN  CPSS_DXCH_TCAM_RULE_SIZE_ENT        ruleSize,
    IN  GT_U32                              *tcamPatternPtr,
    IN  GT_U32                              *tcamMaskPtr
);

/*******************************************************************************
* cpssDxChTcamPortGroupRuleRead
*
* DESCRIPTION:
*       Reads the TCAM mask and key.
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
*       tcamPatternPtr    - points to the TCAM rule's pattern.
*       tcamMaskPtr       - points to the TCAM rule's mask.
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
*       GT_BAD_PTR               - on NULL pointer
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS cpssDxChTcamPortGroupRuleRead
(
    IN  GT_U8                               devNum,
    IN  GT_PORT_GROUPS_BMP                  portGroupsBmp,
    IN  GT_U32                              index,
    OUT GT_BOOL                             *validPtr,
    OUT CPSS_DXCH_TCAM_RULE_SIZE_ENT        *ruleSizePtr,
    OUT GT_U32                              *tcamPatternPtr,
    OUT GT_U32                              *tcamMaskPtr
);

/*******************************************************************************
* cpssDxChTcamPortGroupRuleValidStatusSet
*
* DESCRIPTION:
*     Validates/Invalidates the TCAM rule.
*        The validation of the rule is performed by next steps:
*        1. Retrieve the content of the rule from PP TCAM
*        2. Write content back to TCAM with Valid indication set.
*           The function does not check content of the rule before
*           write it back to TCAM
*        The invalidation of the rule is performed by next steps:
*        1. Retrieve the content of the rule from PP TCAM
*        2. Write content back to TCAM with Invalid indication set.
*        If the given the rule found already in needed valid state
*        no write done. If the given the rule found with size
*        different from the given rule-size an error code returned.
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
*
* OUTPUTS:
*       none.
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
*      None.
*
*******************************************************************************/
GT_STATUS cpssDxChTcamPortGroupRuleValidStatusSet
(
    IN  GT_U8                               devNum,
    IN  GT_PORT_GROUPS_BMP                  portGroupsBmp,
    IN  GT_U32                              index,
    IN  GT_BOOL                             valid
);

/*******************************************************************************
* cpssDxChTcamPortGroupRuleValidStatusGet
*
* DESCRIPTION:
*     Gets validity of the TCAM rule.
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
*      None.
*
*******************************************************************************/
GT_STATUS cpssDxChTcamPortGroupRuleValidStatusGet
(
    IN  GT_U8                               devNum,
    IN  GT_PORT_GROUPS_BMP                  portGroupsBmp,
    IN  GT_U32                              index,
    OUT GT_BOOL                             *validPtr,
    OUT CPSS_DXCH_TCAM_RULE_SIZE_ENT        *ruleSizePtr
);

/*******************************************************************************
* cpssDxChTcamPortGroupClientGroupSet
*
* DESCRIPTION:
*       Enable/Disable client per TCAM group.
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
*       tcamClient        - TCAM client.
*       tcamGroup         - TCAM group id (APPLICABLE RANGES: Bobcat2, Caelum: 0..2, Bobcat3: 0..4)
*       enable            - GT_TRUE: TCAM client is enabled
*                           GT_FALSE: TCAM client is disabled
*
* OUTPUTS:
*       none.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - on bad parameter.
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_OUT_OF_RANGE          - parameter not in valid range.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChTcamPortGroupClientGroupSet

(
    IN  GT_U8                               devNum,
    IN  GT_PORT_GROUPS_BMP                  portGroupsBmp,
    IN  CPSS_DXCH_TCAM_CLIENT_ENT           tcamClient,
    IN  GT_U32                              tcamGroup,
    IN  GT_BOOL                             enable
);

/*******************************************************************************
* cpssDxChTcamPortGroupClientGroupGet
*
* DESCRIPTION:
*       Gets status (Enable/Disable) for client per TCAM group.
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
*       tcamClient        - TCAM client.
*
* OUTPUTS:
*       tcamGroupPtr      - (pointer to) TCAM group id (APPLICABLE RANGES: Bobcat2, Caelum: 0..2, Bobcat3: 0..4)
*       enablePtr         - (pointer to) TCAM client status.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - on bad parameter.
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_OUT_OF_RANGE          - parameter not in valid range.
*       GT_BAD_PTR               - on NULL pointer
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChTcamPortGroupClientGroupGet

(
    IN  GT_U8                               devNum,
    IN  GT_PORT_GROUPS_BMP                  portGroupsBmp,
    IN  CPSS_DXCH_TCAM_CLIENT_ENT           tcamClient,
    OUT GT_U32                              *tcamGroupPtr,
    OUT GT_BOOL                             *enablePtr
);


/*******************************************************************************
* cpssDxChTcamIndexRangeHitNumAndGroupSet
*
* DESCRIPTION:
*       Select for each TCAM index which TCAM group and lookup number
*       is served by TCAM.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum              - the device number
*       floorIndex          - floor index (APPLICABLE RANGES: 0..11)
*       floorInfoArr        - holds group id and lookup Number for each half floor.
*                           Index 0 represent banks 0-5 and index 1 represent  banks  6-11.
*                              APPLICABLE DEVICES: Bobcat2, Bobcat3.
*                           Index 0 represent banks 0-1, index 1 represent  banks  2-3,
*                           index 2 represent banks 4-5, index 3 represent  banks  6-7,
*                           index 4 represent banks 8-9, index 5 represent  banks  10-11.
*                              APPLICABLE DEVICES: Caelum.
*
*
* OUTPUTS:
*       none.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - on bad parameter.
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_OUT_OF_RANGE          - parameter not in valid range.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChTcamIndexRangeHitNumAndGroupSet
(
    IN GT_U8            devNum,
    IN GT_U32           floorIndex,
    IN CPSS_DXCH_TCAM_BLOCK_INFO_STC          floorInfoArr[CPSS_DXCH_TCAM_MAX_NUM_BLOCKS_CNS]
);

/*******************************************************************************
* cpssDxChTcamIndexRangeHitNumAndGroupGet
*
* DESCRIPTION:
*       Select for each TCAM index which TCAM group and lookup number
*       is served by TCAM.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum                  - the device number
*       floorIndex              - floor index (APPLICABLE RANGES: 0..11)
*
*
* OUTPUTS:
*       floorInfoArr               - group id and lookup Number
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - on bad parameter.
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_OUT_OF_RANGE          - parameter not in valid range.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChTcamIndexRangeHitNumAndGroupGet
(
    IN GT_U8            devNum,
    IN GT_U32           floorIndex,
    OUT CPSS_DXCH_TCAM_BLOCK_INFO_STC          floorInfoArr[CPSS_DXCH_TCAM_MAX_NUM_BLOCKS_CNS]
);
/*******************************************************************************
* cpssDxChTcamActiveFloorsSet
*
* DESCRIPTION:
*       Function sets number of active floors at TCAM array. The non-active
*       floors will be the upper floors and will be at power down mode.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum             - the device number
*       numOfActiveFloors  - number of active floors (APPLICABLE RANGES: 0..12)
*
* OUTPUTS:
*       none.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - on bad parameter.
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_OUT_OF_RANGE          - parameter not in valid range.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChTcamActiveFloorsSet
(
    IN  GT_U8           devNum,
    IN  GT_U32          numOfActiveFloors
);

/*******************************************************************************
* cpssDxChTcamActiveFloorsGet
*
* DESCRIPTION:
*       Function gets number of active floors at TCAM array. The non-active
*       floors will be the upper floors and will be at power down mode.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum          - the device number
*
*
* OUTPUTS:
*       numOfActiveFloorsPtr - number of active floors
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - on bad parameter.
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_OUT_OF_RANGE          - parameter not in valid range.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChTcamActiveFloorsGet
(
    IN   GT_U8           devNum,
    OUT  GT_U32          *numOfActiveFloorsPtr
);

/*******************************************************************************
* cpssDxChTcamPortGroupCpuLookupTriggerSet
*
* DESCRIPTION:
*       Sets lookup data and triggers CPU lookup in the TCAM.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum              - the device number
*       portGroupsBmp       - bitmap of Port Groups.
*                           NOTEs:
*                           1. for non multi-port groups device this parameter is IGNORED.
*                           2. for multi-port groups device :
*                              bitmap must be set with at least one bit representing
*                              valid port group(s). If a bit of non valid port group
*                              is set then function returns GT_BAD_PARAM.
*                              value CPSS_PORT_GROUP_UNAWARE_MODE_CNS is supported.
*       group               - TCAM group id (APPLICABLE RANGES: Bobcat2, Caelum: 0..2, Bobcat3: 0..4)
*       size                - key size in TCAM
*       tcamKeyPtr          - (pointer to) key array in the TCAM
*
* OUTPUTS:
*       none.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - on bad parameter.
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_OUT_OF_RANGE          - parameter not in valid range.
*       GT_BAD_PTR               - on NULL pointer
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChTcamPortGroupCpuLookupTriggerSet
(
    IN  GT_U8                               devNum,
    IN  GT_PORT_GROUPS_BMP                  portGroupsBmp,
    IN  GT_U32                              group,
    IN  CPSS_DXCH_TCAM_RULE_SIZE_ENT        size,
    IN  GT_U32                             *tcamKeyPtr
);

/*******************************************************************************
* cpssDxChTcamPortGroupCpuLookupTriggerGet
*
* DESCRIPTION:
*       Gets lookup data and key size from CPU lookup in the TCAM.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum              - the device number
*       portGroupsBmp       - bitmap of Port Groups.
*                           NOTEs:
*                           1. for non multi-port groups device this parameter is IGNORED.
*                           2. for multi-port groups device :
*                              bitmap must be set with at least one bit representing
*                              valid port group(s). If a bit of non valid port group
*                              is set then function returns GT_BAD_PARAM.
*                              value CPSS_PORT_GROUP_UNAWARE_MODE_CNS is supported.
*       group               - TCAM group id (APPLICABLE RANGES: Bobcat2, Caelum: 0..2, Bobcat3: 0..4)
*
* OUTPUTS:
*       sizePtr             - (pointer to) key size in TCAM
*       tcamKeyPtr          - (pointer to) key array in the TCAM
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - on bad parameter.
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_OUT_OF_RANGE          - parameter not in valid range.
*       GT_BAD_STATE             - if in TCAM found rule with different sizes
*       GT_BAD_PTR               - on NULL pointer
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChTcamPortGroupCpuLookupTriggerGet
(
    IN  GT_U8                               devNum,
    IN  GT_PORT_GROUPS_BMP                  portGroupsBmp,
    IN  GT_U32                              group,
    OUT CPSS_DXCH_TCAM_RULE_SIZE_ENT       *sizePtr,
    OUT GT_U32                             *tcamKeyPtr
);

/*******************************************************************************
* cpssDxChTcamCpuLookupResultsGet
*
* DESCRIPTION:
*       Gets CPU lookup results.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum              - the device number
*       group               - TCAM group id (APPLICABLE RANGES: Bobcat2, Caelum: 0..2, Bobcat3: 0..4)
*       hitNum              - hit number in TCAM (APPLICABLE RANGES: 0..3)
*       portGroupsBmp       -
*
* OUTPUTS:
*       isValidPtr          - (pointer to) whether results are valid for prev trigger set.
*       isHitPtr            - (pointer to) whether there was hit in the TCAM. Valid only when isValidPtr == GT_TRUE
*       hitIndexPtr         - (pointer to) global hit index in the TCAM. Valid only when isHitPtr == GT_TRUE
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - on bad parameter.
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_BAD_STATE             - if CPU lookup results are not valid
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChTcamCpuLookupResultsGet
(
    IN  GT_U8                               devNum,
    IN  GT_PORT_GROUPS_BMP                  portGroupsBmp,
    IN  GT_U32                              group,
    IN  GT_U32                              hitNum,
    OUT GT_BOOL                            *isValidPtr,
    OUT GT_BOOL                            *isHitPtr,
    OUT GT_U32                             *hitIndexPtr
);


#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __cpssDxChTcamh */

