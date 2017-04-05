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
* appDemoExMxPmExternalUtils.h
*
* DESCRIPTION:
*       App demo ExMxPm External TCAM utilities API header.
*
* FILE REVISION NUMBER:
*       $Revision: 1 $
*******************************************************************************/

#include <cpss/exMxPm/exMxPmGen/extTcam/cpssExMxPmExternalTcam.h>


/*
 * typedef: APP_DEMO_EXTERNAL_TCAM_IDT_REP_OP_ENT
 *
 * Description:
 *      Action of IDT external TCAM repeat instructions.
 * Fields:
 *    APP_DEMO_EXTERNAL_TCAM_IDT_REP_OP_COPY_E        -   copy
 *    APP_DEMO_EXTERNAL_TCAM_IDT_REP_OP_MOVE_E        -   move
 *    APP_DEMO_EXTERNAL_TCAM_IDT_REP_OP_CLEAR_E       -   clear
 *    APP_DEMO_EXTERNAL_TCAM_IDT_REP_OP_VALIDATE_E    -   validate
 *    APP_DEMO_EXTERNAL_TCAM_IDT_REP_OP_INVALIDATE_E  -   invalidate
 *
 * Comment:
 */
typedef enum
{
    APP_DEMO_EXTERNAL_TCAM_IDT_REP_OP_COPY_E,
    APP_DEMO_EXTERNAL_TCAM_IDT_REP_OP_MOVE_E,
    APP_DEMO_EXTERNAL_TCAM_IDT_REP_OP_CLEAR_E,
    APP_DEMO_EXTERNAL_TCAM_IDT_REP_OP_VALIDATE_E,
    APP_DEMO_EXTERNAL_TCAM_IDT_REP_OP_INVALIDATE_E
} APP_DEMO_EXTERNAL_TCAM_IDT_REP_OP_ENT;

/*
 * typedef: APP_DEMO_EXTERNAL_TCAM_IDT_REP_DIRECTION_ENT
 *
 * Description:
 *      Looping direction of addresses used during the repeat action.
 * Fields:
 *    APP_DEMO_EEXTERNAL_TCAM_IDT_REP_DIRECTION_INC_E - increment direction
 *    APP_DEMO_EEXTERNAL_TCAM_IDT_REP_DIRECTION_DEC_E - deccrement direction
 *
 * Comment:
 *
 */
typedef enum
{
    APP_DEMO_EXTERNAL_TCAM_IDT_REP_DIRECTION_INC_E,
    APP_DEMO_EXTERNAL_TCAM_IDT_REP_DIRECTION_DEC_E
} APP_DEMO_EXTERNAL_TCAM_IDT_REP_DIRECTION_ENT;

/*
 * typedef: APP_DEMO_EXTERNAL_TCAM_IDT_AGED_OUT_OP_ENT
 *
 * Description:
 *      Indicate how invalidation of aged out entries is done.
 * Fields:
 *    APP_DEMO_EXTERNAL_TCAM_IDT_AGED_OUT_OP_BY_PP    - aged out entries are
 *                                                      invalidate by the
 *                                                      pp\asic sending invalid
 *                                                      instruction.
 *    APP_DEMO_EXTERNAL_TCAM_IDT_AGED_OUT_OP_BY_TCAM  - invalidation of the aged
 *                                                      entries will be done
 *                                                      automatically by the TCAM
 *                                                      (after read from the age
 *                                                      fifo).
 *
 * Comment:
 */
typedef enum
{
    APP_DEMO_EXTERNAL_TCAM_IDT_AGED_OUT_OP_BY_PP,
    APP_DEMO_EXTERNAL_TCAM_IDT_AGED_OUT_OP_BY_TCAM
} APP_DEMO_EXTERNAL_TCAM_IDT_AGED_OUT_OP_ENT;

/*
 * typedef: struct APP_DEMO_EXTERNAL_TCAM_IDT_REPEAT_CMD_STC
 *
 * Description:
 *      Addresses and control information for the IDT TCAM repeat instruction.
 *
 * Fields:
 *      repeatCount - the number of 80 bit words to be affected.
 *      srcAddr     - source address for the repeat instruction.
 *      dstAddr     - destination address, relevant for copy and move repeat
 *                    instructions.
 *      cmdOpCode   - operation type: copy\move\clear\validate\invalidate
 *      countDir    - count direction, increament or decrement operation
 *                    addresses.
 *
 * Comment:
 */
typedef struct
{
    GT_U32                                       repeatCount;
    GT_U32                                       srcAddr;
    GT_U32                                       dstAddr;
    APP_DEMO_EXTERNAL_TCAM_IDT_REP_OP_ENT        cmdOpCode;
    APP_DEMO_EXTERNAL_TCAM_IDT_REP_DIRECTION_ENT countDir;
} APP_DEMO_EXTERNAL_TCAM_IDT_REPEAT_CMD_STC;


/*******************************************************************************
* appDemoExMxPmExtTcamMgLookup
*
* DESCRIPTION:
*   The function preforms management client lookup in External TCAM device
*
* APPLICABLE DEVICES:  All ExMxPm devices
*
* INPUTS:
*       devNum              - device number
*       lookupConfigPtr     - (pointer to) search scheme parameters
*       ruleSize            - size of rule entry.
*       dataPtr             - (pointer to) data\key for lookup action
*       maxNumOfResults     - the maximal number of results that can
*                             be retrieved.
*
* OUTPUTS:
*       resultIndexPtr      - (pointer to) array of hit\miss results.
*                             24 bit format according to TCAM datasheet
*                             (the 24th bit indicates a hit).
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - on null pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
*******************************************************************************/
GT_STATUS appDemoExMxPmExtTcamMgLookup
(
    IN  GT_U8                                       devNum,
    IN  CPSS_EXMXPM_EXTERNAL_TCAM_CLIENT_CONFIG_STC *lookupConfigPtr,
    IN  CPSS_EXMXPM_EXTERNAL_TCAM_RULE_SIZE_ENT     ruleSize,
    IN  GT_U32                                      *dataPtr,
    IN  GT_U32                                      maxNumOfResults,
    OUT GT_U32                                      *resultIndexPtr
);

/*************************************************************************
* appDemoExMxPmExtIdtTcamRepeat
*
* DESCRIPTION:
*       Execute IDT TCAM repeat operation
*
* APPLICABLE DEVICES:  All ExMxPm devices
*
* INPUTS:
*       devNum          - device number
*       tcamId          - cascaded TCAM Id 0..3
*       repeatCmd       - configuration for the repeat operation.
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
* COMMENTS:
*
*
*******************************************************************************/
GT_STATUS appDemoExMxPmExtIdtTcamRepeatOperationExecute
(
    IN  GT_U8                                        devNum,
    IN  GT_U32                                       tcamId,
    IN  APP_DEMO_EXTERNAL_TCAM_IDT_REPEAT_CMD_STC    repeatCmd
);

/*************************************************************************
* appDemoExMxPmExtIdtTcamNfa
*
* DESCRIPTION:
*       Execute IDT TCAM Next Free Address (NFA) operation
*
* APPLICABLE DEVICES:  All ExMxPm devices
*
* INPUTS:
*       devNum              - device number
*       tcamId              - cascaded TCAM Id 0..3
*       sssRegNum           - selected Search Scheme register.
*       maxNumOfResults     - the maximal number of NFA
*                             results that can be retrieved.
*
* OUTPUTS:
*       nfaResultPtr        - (pointer to) array of NFA results.
*                             24 bit format according to TCAM datasheet
*                             (the 24th bit indicates a "NFA hit").
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - on null pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*
*******************************************************************************/
GT_STATUS appDemoExMxPmExtIdtTcamNfa
(
    IN  GT_U8       devNum,
    IN  GT_U32      tcamId,
    IN  GT_U8       sssRegNum,
    IN  GT_U32      NumOfResults,
    OUT GT_U32      *nfaResultPtr
);

/*************************************************************************
* appDemoExMxPmExternalIdtTcamAgeFifoEntryGet
*
* DESCRIPTION:
*       Retrieve one index of entry to be aged out for each TCAM device
*
* APPLICABLE DEVICES:  All ExMxPm devices
*
* INPUTS:
*       devNum              - device number
*       invalidateMode      - how entry to be aged out should be invalidate.
*       maxNumOfEntries     - the maximal number of aged out indexes that
*                             can be retrieved.
*
* OUTPUTS:
*       agesOutPtr          - (pointer to) array which contains the indexes
*                             of entries to be aged out.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - on null pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*
*******************************************************************************/
GT_STATUS appDemoExMxPmExternalIdtTcamAgeFifoEntryGet
(
    IN  GT_U8                                        devNum,
    IN  APP_DEMO_EXTERNAL_TCAM_IDT_AGED_OUT_OP_ENT   invalidateMode,
    IN  GT_U32                                       maxNumOfEntries,
    OUT GT_U32                                       *agesOutPtr
);

/*******************************************************************************
* appDemoExMxPmExternalTcamRuleStateGet
*
* DESCRIPTION:
*   The function gets rule status in External TCAM device
*
* APPLICABLE DEVICES:
*        Puma2; Puma3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum          - device number
*       tcamId          - cascaded TCAM Id 0..3
*       ruleStartIndex  - index of rule in external TCAM.
*
* OUTPUTS:
*       validPtr        - (pointer to)Rule's valid status:
*                         GT_TRUE - valid rule
*                         GT_FALSE - invalid rule
*       ageEnablePtr    - (pointer to)Rule's aging enable status
*                         Relevant only if aging function is enabled and configured:
*                         GT_TRUE - aging is enabled for rule
*                         GT_FALSE - aging is disabled for rule.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - on null pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
 COMMENTS:
*
*******************************************************************************/
GT_STATUS appDemoExMxPmExternalTcamRuleStateGet
(
    IN  GT_U8                                       devNum,
    IN  GT_U32                                      tcamId,
    IN  GT_U32                                      ruleStartIndex,
    OUT GT_BOOL                                     *validPtr,
    OUT GT_BOOL                                     *ageEnablePtr
);

/*******************************************************************************
* appDemoExMxPmExternalTcamRuleValidStatusSet
*
* DESCRIPTION:
*   The function sets rule status in External TCAM device
*
* APPLICABLE DEVICES:
*        Puma2; Puma3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum          - device number
*       tcamId          - cascaded TCAM Id 0..3
*       ruleStartIndex  - index of rule in external TCAM.
*       ruleSize        - size of rule entry.
*       valid           - Rule's valid status:
*                         GT_TRUE - valid rule
*                         GT_FALSE - invalid rule
*       ageEnable       - Rule's aging enable status
*                         Relevant only if aging function is enabled and configured:
*                         GT_TRUE - aging is enabled for rule
*                         GT_FALSE - aging is disabled for rule.
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
 COMMENTS:
*
*******************************************************************************/
GT_STATUS appDemoExMxPmExternalTcamRuleValidStatusSet
(
    IN  GT_U8                                       devNum,
    IN  GT_U32                                      tcamId,
    IN  GT_U32                                      ruleStartIndex,
    IN  CPSS_EXMXPM_EXTERNAL_TCAM_RULE_SIZE_ENT     ruleSize,
    IN  GT_BOOL                                     valid,
    IN  GT_BOOL                                     ageEnable
);

/*******************************************************************************
* appDemoExMxPmExternalTcamRuleAndActionCopy
*
* DESCRIPTION:
*   The function copies Rule and Action in External TCAM device to the new place
*
* APPLICABLE DEVICES:
*        Puma2; Puma3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum              - device number
*       tcamId              - cascaded TCAM Id 0..3
*       ruleSize            - size of rule entry.
*       actionSize          - action size in bits
*       srcRuleStartIndex   - index of source rule in external TCAM.
*       dstRuleStartIndex   - index of destination rule in external TCAM.
*       srcActionStartIndex - index of source rule action.
*       dstActionStartIndex - index of destination rule action.
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
 COMMENTS:
*
*******************************************************************************/
GT_STATUS appDemoExMxPmExternalTcamRuleAndActionCopy
(
    IN  GT_U8                                       devNum,
    IN  GT_U32                                      tcamId,
    IN  CPSS_EXMXPM_CONTROL_MEMORY_ENT              actionCsu,
    IN  CPSS_EXMXPM_EXTERNAL_TCAM_RULE_SIZE_ENT     ruleSize,
    IN  GT_U32                                      actionSize,
    IN  GT_U32                                      srcRuleStartIndex,
    IN  GT_U32                                      dstRuleStartIndex,
    IN  GT_U32                                      srcActionStartIndex,
    IN  GT_U32                                      dstActionStartIndex
);


