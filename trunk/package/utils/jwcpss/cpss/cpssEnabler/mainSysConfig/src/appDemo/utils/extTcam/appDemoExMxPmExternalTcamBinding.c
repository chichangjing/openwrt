/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
********************************************************************************
* appDemoExMxPmExternalTcamBinding.c
*
* DESCRIPTION:
*       App demo ExMxPm External TCAM glue layer.
*
* FILE REVISION NUMBER:
*       $Revision: 0 $
*******************************************************************************/

#include <appDemo/utils/extTcam/appDemoExMxPmExternalTcamBinding.h>


/*
 * appDemoExMxPmExternalTcamWriteRead
 *
 * DESCRIPTION:
 *      App demo glue layer example for binding between
 *      CPSS_EXMXPM_EXTERNAL_TCAM_RULE_WRITE_FUNC callback to
 *      cpssExMxPmExternalTcamRuleWrite external TCAM API.
 *
 * INPUTS:
 *       devNum          - device number 
 *       ruleInfo        - Information required by the external TCAM and
 *                         supplied to the local CPSS client by the 
 *                         application in the same structure format.
 *       dataSize        - Actual number of relevant bits in rule.
 *                         Calculated by the local CPSS client.
 *       valid           - Rule's valid status
 *                         GT_TRUE - valid rule 
 *                         GT_FALSE - invalid rule.
 *       dataFormat      - Indicates whether the rule is supplied in X\Y 
 *                         or pattern\mask format.
 *                         For local CPSS clients always use pattern\mask
 *                         format.
 *       patternOrXArr   - Array of 32-bit words that contains the pattern 
 *                         or X-data, word0 - bits0-31, word1 - bits32-63, ...
 *                         HW format as derived by the local CPSS client.
 *       maskOrYArr      - Array of 32-bit words that contains the mask
 *                         or Y-data, word0 - bits0-31, word1 - bits32-63, ... 
 *                         HW format as derived by the local CPSS client. 
 *
 * OUTPUTS:
 *       None.
 *
 * RETURNS:
 *       GT_OK                    - on success
 *       GT_BAD_PARAM             - on wrong parameters
 *       GT_HW_ERROR              - on hardware error
 *       GT_BAD_PTR               - on null pointer
 *       GT_NOT_APPLICABLE_DEVICE - on not applicable device
 *
 * Comment:
 */
GT_STATUS appDemoExMxPmExternalTcamRuleWrite
(
    IN  GT_U8                                       devNum,
    IN  CPSS_EXMXPM_EXTERNAL_TCAM_RULE_INFO_STC     *ruleInfo,
    IN  GT_U32                                      dataSize,
    IN  GT_BOOL                                     valid,
    IN  CPSS_EXMXPM_EXTERNAL_TCAM_RULE_FORMAT_ENT   dataFormat,
    IN  GT_U32                                      *patternOrXArr,
    IN  GT_U32                                      *maskOrYArr
)
{                           
    return cpssExMxPmExternalTcamRuleWrite(devNum,
                                           ruleInfo->tcamId,
                                           ruleInfo->ruleStartIndex,
                                           ruleInfo->ruleSize,
                                           dataSize,
                                           valid,
                                           ruleInfo->ageEnable,
                                           dataFormat,
                                           patternOrXArr,
                                           maskOrYArr);
}

/*
 * appDemoExMxPmExternalTcamRuleRead
 *
 * DESCRIPTION:
 *      App demo glue layer example for binding between
 *      CPSS_EXMXPM_EXTERNAL_TCAM_RULE_READ_FUNC callback to
 *      cpssExMxPmExternalTcamRuleRead external TCAM API.
 *
 * INPUTS:
 *       devNum          - device number 
 *       ruleInfo        - (pointer to)Information required by the external TCAM and
 *                         supplied to the local CPSS client by the 
 *                         application in the same structure format.
 *       dataSize        - Actual number of relevant bits in rule.
 *                         Calculated by the local CPSS client.
 *       dataFormat      - Indicates whether the rule is returned in X\Y 
 *                         or pattern\mask format.
 *                         For local CPSS clients always use pattern\mask
 *                         format.
 *
 * OUTPUTS:
 *       ruleInfo        - (pointer to)The age field in this structure is OUT for this function.
 *       validPtr        - (pointer to)Rule's valid status
 *                         GT_TRUE - valid rule 
 *                         GT_FALSE - invalid rule.
 *       patternOrXArr   - (pointer to)Array of 32-bit words that contains the pattern 
 *                         or X-data, word0 - bits0-31, word1 - bits32-63, ...
 *                         HW format.
 *       maskOrYArr      - (pointer to)Array of 32-bit words that contains the mask
 *                         or Y-data, word0 - bits0-31, word1 - bits32-63, ... 
 *                         HW format. 
 *
 * RETURNS:
 *       GT_OK                    - on success
 *       GT_BAD_PARAM             - on wrong parameters
 *       GT_HW_ERROR              - on hardware error
 *       GT_BAD_PTR               - on null pointer
 *       GT_NOT_APPLICABLE_DEVICE - on not applicable device
 *
 * Comment:
 */
GT_STATUS appDemoExMxPmExternalTcamRuleRead
(
    IN    GT_U8                                       devNum,
    INOUT CPSS_EXMXPM_EXTERNAL_TCAM_RULE_INFO_STC     *ruleInfo,
    IN    GT_U32                                      dataSize,
    OUT   GT_BOOL                                     *validPtr,
    IN    CPSS_EXMXPM_EXTERNAL_TCAM_RULE_FORMAT_ENT   dataFormat,
    OUT   GT_U32                                      *patternOrXArr,
    OUT   GT_U32                                      *maskOrYArr
)
{
    return cpssExMxPmExternalTcamRuleRead(devNum,
                                          ruleInfo->tcamId,
                                          ruleInfo->ruleStartIndex,
                                          ruleInfo->ruleSize,
                                          dataSize,
                                          dataFormat, 
                                          validPtr,
                                          &(ruleInfo->ageEnable),
                                          patternOrXArr,
                                          maskOrYArr);
}
 /*
 * appDemoExMxPmExternalTcamActionWrite
 *
 * DESCRIPTION:
 *      App demo glue layer example for binding between
 *      CPSS_EXMXPM_EXTERNAL_TCAM_ACTION_Write_FUNC callback to
 *      cpssExMxPmExternalTcamActionWrite external TCAM API.
 *
 * INPUTS:
 *       devNum          - device number 
 *       actionInfoPtr   - Information required by the external TCAM and
 *                         supplied to the local CPSS client by the 
 *                         application in the same structure format.
 *       actionSize      - Action size in bits, calculated by the local CPSS client.
 *       actionDataPtr   - HW format of action to write derived by the local CPSS 
 *                         client.
 *
 * OUTPUTS:
 *       None. 
 *
 * RETURNS:
 *       GT_OK                    - on success
 *       GT_BAD_PARAM             - on wrong parameters
 *       GT_HW_ERROR              - on hardware error
 *       GT_BAD_PTR               - on null pointer
 *       GT_NOT_APPLICABLE_DEVICE - on not applicable device
 *
 * Comment:
 */
GT_STATUS appDemoExMxPmExternalTcamActionWrite
(
    IN  GT_U8                                       devNum,
    IN  CPSS_EXMXPM_EXTERNAL_TCAM_ACTION_INFO_STC   *actionInfoPtr,  
    IN  GT_U32                                      actionSize,
    IN  GT_U32                                      *actionDataPtr
)
{
    return cpssExMxPmExternalTcamActionWrite(devNum,
                                             actionInfoPtr->memoryType,
                                             actionInfoPtr->actionStartIndex,
                                             actionSize,
                                             actionDataPtr);
}

/*
 * appDemoExMxPmExternalTcamActionRead
 *
 * DESCRIPTION:
 *      App demo glue layer example for binding between
 *      CPSS_EXMXPM_EXTERNAL_TCAM_ACTION_READ_FUNC callback to
 *      cpssExMxPmExternalTcamActionRead external TCAM API.
 *
 * INPUTS:
 *       devNum          - device number 
 *       actionInfoPtr   - Information required by the external TCAM and
 *                         supplied to the local CPSS client by the 
 *                         application in the same structure format.
 *       actionSize      - Action size in bits, calculated by the local CPSS client.
 *
 * OUTPUTS:
 *       actionDataPtr   - HW format of action retrieved. 
 *
 * RETURNS:
 *       GT_OK                    - on success
 *       GT_BAD_PARAM             - on wrong parameters
 *       GT_HW_ERROR              - on hardware error
 *       GT_BAD_PTR               - on null pointer
 *       GT_NOT_APPLICABLE_DEVICE - on not applicable device
 *
 * Comment:
 */
GT_STATUS appDemoExMxPmExternalTcamActionRead
(
    IN  GT_U8                                       devNum,
    IN  CPSS_EXMXPM_EXTERNAL_TCAM_ACTION_INFO_STC   *actionInfoPtr,  
    IN  GT_U32                                      actionSize,
    OUT GT_U32                                      *actionDataPtr
)
{
        return cpssExMxPmExternalTcamActionRead(devNum,
                                                actionInfoPtr->memoryType,
                                                actionInfoPtr->actionStartIndex,
                                                actionSize,
                                                actionDataPtr);
}



