/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* cpssDxChVirtualTcam.h
*
* DESCRIPTION:
*       The CPSS DXCH High Level Virtual TCAM Manager
*
* FILE REVISION NUMBER:
*       $Revision: 1 $
*******************************************************************************/
#ifndef __cpssDxChVirtualTcam_h
#define __cpssDxChVirtualTcam_h

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include <cpss/generic/cpssTypes.h>
#include <cpss/dxCh/dxChxGen/cpssDxChTypes.h>
#include <cpss/dxCh/dxChxGen/tcam/cpssDxChTcam.h>
#include <cpss/dxCh/dxChxGen/pcl/cpssDxChPcl.h>
#include <cpss/dxCh/dxChxGen/tti/cpssDxChTti.h>

/* maximal amount of Virtual TCAM Managers - Id range 0..31 */
#define CPSS_DXCH_VIRTUAL_TCAM_MNG_MAX_ID_CNS 31

/* maximal amount of vTCAMs - Id range 0..1023 */
#define CPSS_DXCH_VIRTUAL_TCAM_VTCAM_MAX_ID_CNS 1023

/* used to point the last rule place in Virtual TCAM */
#define CPSS_DXCH_VIRTUAL_TCAM_APPEND_CNS 0xFFFFFFFF

/* used to point the place in Virtual TCAM precedes the first rule */
#define CPSS_DXCH_VIRTUAL_TCAM_BEFORE_START_CNS 0xFFFFFFFE

/* used to start iteration of rules in Virtual TCAM */
#define CPSS_DXCH_VIRTUAL_TCAM_START_RULE_ID_CNS 0xFFFFFFFF

/*
 * Typedef: CPSS_DXCH_VIRTUAL_TCAM_RULE_ID
 *
 * Description: Rule ID of virtual TCAM
 *
 *  used as the type for the rule ID
 *
 */
typedef GT_U32 CPSS_DXCH_VIRTUAL_TCAM_RULE_ID;

/*
 * typedef: enum CPSS_DXCH_VIRTUAL_TCAM_RULE_TYPE_ENT
 *
 * Description:
 *      Rule types.
 *
 * Enumerations:
 *
 *     CPSS_DXCH_VIRTUAL_TCAM_RULE_TYPE_PCL_E  - Rule type is PCL
 *     CPSS_DXCH_VIRTUAL_TCAM_RULE_TYPE_TTI_E  - Rule type is TTI
 *
 */
typedef enum
{
    CPSS_DXCH_VIRTUAL_TCAM_RULE_TYPE_PCL_E,
    CPSS_DXCH_VIRTUAL_TCAM_RULE_TYPE_TTI_E
} CPSS_DXCH_VIRTUAL_TCAM_RULE_TYPE_ENT;

/*
 * typedef: enum CPSS_DXCH_VIRTUAL_TCAM_RULE_ADDITION_METHOD_ENT
 *
 * Description:
 *      Rule create types.
 *
 * Enumerations:
 *
 *     CPSS_DXCH_VIRTUAL_TCAM_RULE_ADDITION_METHOD_LOGICAL_INDEX_E   -
 *                  Rule placed by given logical index.
 *                  If logical index of rule1 more than logical index of rule2
 *                  than rule1 will be checked to match after rule2
 *     CPSS_DXCH_VIRTUAL_TCAM_RULE_ADDITION_METHOD_PRIORITY_E        -
 *                  Rule created by unique rule Id and attributes(priority).
 *                  If priority of rule1 less than priority of rule2
 *                  than rule1 will be checked to match before rule2
 *                  If two rules created with the same priority
 *                  the rule created later be checked to match later.
 *
 */
typedef enum
{
    CPSS_DXCH_VIRTUAL_TCAM_RULE_ADDITION_METHOD_LOGICAL_INDEX_E,
    CPSS_DXCH_VIRTUAL_TCAM_RULE_ADDITION_METHOD_PRIORITY_E
} CPSS_DXCH_VIRTUAL_TCAM_RULE_ADDITION_METHOD_ENT;

/*
 * typedef: enum CPSS_DXCH_VIRTUAL_TCAM_RULE_SIZE_ENT
 *
 * Description:
 *      TCAM rule size.
 *
 * Enumerations:
 *
 *      CPSS_DXCH_VIRTUAL_TCAM_RULE_SIZE_10_B_E - TCAM rule size to be used 10 Bytes
 *                              (APPLICABLE DEVICES: Bobcat2, Caelum, Bobcat3)
 *      CPSS_DXCH_VIRTUAL_TCAM_RULE_SIZE_20_B_E - TCAM rule size to be used 20 Bytes
 *                              (APPLICABLE DEVICES: Bobcat2, Caelum, Bobcat3)
 *      CPSS_DXCH_VIRTUAL_TCAM_RULE_SIZE_30_B_E - TCAM rule size to be used 30 Bytes
 *      CPSS_DXCH_VIRTUAL_TCAM_RULE_SIZE_40_B_E - TCAM rule size to be used 40 Bytes
 *                              (APPLICABLE DEVICES: Bobcat2, Caelum, Bobcat3)
 *      CPSS_DXCH_VIRTUAL_TCAM_RULE_SIZE_50_B_E - TCAM rule size to be used 50 Bytes
 *                              (APPLICABLE DEVICES: Bobcat2, Caelum, Bobcat3)
 *      CPSS_DXCH_VIRTUAL_TCAM_RULE_SIZE_60_B_E - TCAM rule size to be used 60 Bytes
 *      CPSS_DXCH_VIRTUAL_TCAM_RULE_SIZE_STD_E - Standard rule size for xCat3 devices.
 *      CPSS_DXCH_VIRTUAL_TCAM_RULE_SIZE_EXT_E - Extended rule size for xCat3 devices.
 *
 */
typedef enum
{
    CPSS_DXCH_VIRTUAL_TCAM_RULE_SIZE_10_B_E,
    CPSS_DXCH_VIRTUAL_TCAM_RULE_SIZE_20_B_E,
    CPSS_DXCH_VIRTUAL_TCAM_RULE_SIZE_30_B_E,
    CPSS_DXCH_VIRTUAL_TCAM_RULE_SIZE_40_B_E,
    CPSS_DXCH_VIRTUAL_TCAM_RULE_SIZE_50_B_E,
    CPSS_DXCH_VIRTUAL_TCAM_RULE_SIZE_60_B_E,
    CPSS_DXCH_VIRTUAL_TCAM_RULE_SIZE_STD_E = CPSS_DXCH_VIRTUAL_TCAM_RULE_SIZE_30_B_E,
    CPSS_DXCH_VIRTUAL_TCAM_RULE_SIZE_EXT_E = CPSS_DXCH_VIRTUAL_TCAM_RULE_SIZE_60_B_E
} CPSS_DXCH_VIRTUAL_TCAM_RULE_SIZE_ENT;

/*
 * typedef: enum CPSS_DXCH_VIRTUAL_TCAM_AVAILABILTY_ENT
 *
 * Description:
 *      Rule TCAM Availability.
 *
 * Enumerations:
 *
 *      CPSS_DXCH_VIRTUAL_TCAM_AVAILABILTY_NEVER_E        - Free TCAM Memory not available
 *      CPSS_DXCH_VIRTUAL_TCAM_AVAILABILTY_ALREADY_E      - Free TCAM Memory already available
 *      CPSS_DXCH_VIRTUAL_TCAM_AVAILABILTY_AFTER_DEFRAG_E - Free TCAM Memory will be available
 *                                                          after defragmentation
 *      CPSS_DXCH_VIRTUAL_TCAM_AVAILABILTY_FROM_SCRATCH_E - Free TCAM Memory will be available
 *                                                          only from scratch.
 *                                                          meaning that all existing vtcams need to be removed,
 *                                                          before start to adding them and other new ones in
 *                                                          descending 'rule size' vtcams order(60B vtcams before any 50B vtcams ..
 *                                                          before any 20B vtcams  , before any 10B vtcams)
 *
 */
typedef enum
{
    CPSS_DXCH_VIRTUAL_TCAM_AVAILABILTY_NEVER_E,
    CPSS_DXCH_VIRTUAL_TCAM_AVAILABILTY_ALREADY_E,
    CPSS_DXCH_VIRTUAL_TCAM_AVAILABILTY_AFTER_DEFRAG_E,
    CPSS_DXCH_VIRTUAL_TCAM_AVAILABILTY_FROM_SCRATCH_E
} CPSS_DXCH_VIRTUAL_TCAM_AVAILABILTY_ENT;

/*
 * typedef: enum CPSS_DXCH_VIRTUAL_TCAM_EQUAL_PRIORITY_RULE_POSITION_ENT
 *
 * Description:
 *      Position or rule in range of the same priority rules.
 *
 * Enumerations:
 *
 *      CPSS_DXCH_VIRTUAL_TCAM_EQUAL_PRIORITY_RULE_POSITION_FIRST_E     - First in the range
 *      CPSS_DXCH_VIRTUAL_TCAM_EQUAL_PRIORITY_RULE_POSITION_LAST_E      - Last in the range
 *
 */
typedef enum
{
    CPSS_DXCH_VIRTUAL_TCAM_EQUAL_PRIORITY_RULE_POSITION_FIRST_E,
    CPSS_DXCH_VIRTUAL_TCAM_EQUAL_PRIORITY_RULE_POSITION_LAST_E
} CPSS_DXCH_VIRTUAL_TCAM_EQUAL_PRIORITY_RULE_POSITION_ENT;

/*
 * Typedef: struct CPSS_DXCH_VIRTUAL_TCAM_RULE_ATTRIBUTES_STC
 *
 * Description:
 *     This structure describe attributes of the rule.
 *
 * Fields:
 *     priority - used only for priority driven vTcams.
 *
 *  Comments:
 *
 */
typedef struct
{
    GT_U32    priority;
} CPSS_DXCH_VIRTUAL_TCAM_RULE_ATTRIBUTES_STC;

/*
 * Typedef: struct CPSS_DXCH_VIRTUAL_TCAM_INFO_STC
 *
 * Description:
 *     This structure to create virtual TCAM (partition).
 *
 * Fields:
 *       clientGroup           - TCAM Client Group.
 *       hitNumber             - TCAM paralell sublookup number;
 *                              (APPLICABLE RANGES: 0..3);
 *       ruleSize              - size of rules used in virtual TCAM.
 *       autoResize            - GT_TRUE  - resized automatically due to adding/removing rules
 *                               GT_FALSE - resized only when calling "Resize" API.
 *                               Relevant only to priority driven vTCAMs,
 *                               For locical-index driven vTCAMs GT_TRUE causes return GT_BAD_PARAM,.
 *                               Automatic resize option never reduces vTCAM size less
 *                               than guaranteedNumOfRules rules.
 *       guaranteedNumOfRules  - guaranteed amount of rules.
 *       ruleAdditionMethod    - rules addition method.
 *
 *  Comments:
 *
 */
typedef struct
{
    GT_U32                                          clientGroup;
    GT_U32                                          hitNumber;
    CPSS_DXCH_VIRTUAL_TCAM_RULE_SIZE_ENT            ruleSize;
    GT_BOOL                                         autoResize;
    GT_U32                                          guaranteedNumOfRules;
    CPSS_DXCH_VIRTUAL_TCAM_RULE_ADDITION_METHOD_ENT ruleAdditionMethod;
} CPSS_DXCH_VIRTUAL_TCAM_INFO_STC;

/*
 * Typedef: struct CPSS_DXCH_VIRTUAL_TCAM_CHECK_INFO_STC
 *
 * Description:
 *     This structure to check TCAM memory availability.
 *
 * Fields:
 *       isExist               - for existing vTcam being overriden or resized - GT_TRUE,
 *                               for new vTcam - GT_FALSE.
 *       vTcamId               - virtual TCAM Id, relevant only when isExist == GT_TRUE
 *       vTcamInfo             - virtual TCAM Info (see structure).
 *
 *  Comments:
 *
 */
typedef struct
{
    GT_BOOL                           isExist;
    GT_U32                            vTcamId;
    CPSS_DXCH_VIRTUAL_TCAM_INFO_STC   vTcamInfo;
} CPSS_DXCH_VIRTUAL_TCAM_CHECK_INFO_STC;

/*
 * Typedef: struct CPSS_DXCH_VIRTUAL_TCAM_RULE_ACTION_CONTENT_STC
 *
 * Description:
 *     This structure of rule action content
 *
 * Fields:
 *       ruleType   - rule type: (see enum).
 *       action     - Union for PCL rule or for TTI Action Type.
 *
 *       PCL Rule Action parameters for cpssDxChPclRuleActionUpdate/Get call:
 *       action.pcl.direction   - direction of rule's action.
 *
 *  Comments:
 *      Rule Action update/get implemented by calling
 *          PCL - cpssDxChPclRuleActionUpdate/Get
 *          TTI - cpssDxChTtiRuleActionUpdate/Get
 *
 */
typedef struct
{
    CPSS_DXCH_VIRTUAL_TCAM_RULE_TYPE_ENT       ruleType;
    union
    {
        struct
        {
            CPSS_PCL_DIRECTION_ENT             direction;
        } pcl;
    } action;
} CPSS_DXCH_VIRTUAL_TCAM_RULE_ACTION_TYPE_STC;

/*
 * Typedef: struct CPSS_DXCH_VIRTUAL_TCAM_RULE_ACTION_CONTENT_STC
 *
 * Description:
 *     This structure of rule action content
 *
 * Fields:
 *       ruleType   - rule type: (see enum).
 *       action     - Union for PCL rule or for TTI Rule.
 *       PCL Rule Action parameters for cpssDxChPclRuleActionUpdate call:
 *       action.pcl.actionPtr   - (pointer to)rule action.
 *       TTI Rule Action parameters for cpssDxChTtiRuleActionUpdate call:
 *       action.tti.actionPtr   - (pointer to)rule action.
 *                              (using actionPtr->type2)
 *
 *  Comments:
 *      Rule Action update implemented by calling
 *          PCL - cpssDxChPclRuleActionUpdate/Get
 *          TTI - cpssDxChTtiRuleActionUpdate/Get
 *
 */
typedef struct
{
    union
    {
        struct
        {
            CPSS_DXCH_PCL_ACTION_STC           *actionPtr;
        } pcl;
        struct
        {
            CPSS_DXCH_TTI_ACTION_UNT           *actionPtr;
        } tti;
    } action;
} CPSS_DXCH_VIRTUAL_TCAM_RULE_ACTION_DATA_STC;

/*
 * Typedef: struct CPSS_DXCH_VIRTUAL_TCAM_RULE_CONTENT_STC
 *
 * Description:
 *     This structure of rule content
 *
 * Fields:
 *       ruleType   - rule type: (see enum).
 *       rule       - Union for PCL rule or for TTI Rule.
 *       PCL Rule parameters for cpssDxChPclRuleSet/ParsedGet call:
 *       rule.pcl.ruleFormat  - PCL format of the Rule.
 *
 *       TTI Rule parameters for cpssDxChTtiRuleSet/Get call:
 *       rule.tti.ruleFormat  - TTI format of the Rule.
 *
 *  Comments:
 *      Rule writing/read implemented by calling
 *          PCL - cpssDxChPclRuleSet/ParsedGet
 *          TTI - cpssDxChTtiRuleSet/Get
 *
 */
typedef struct
{
    CPSS_DXCH_VIRTUAL_TCAM_RULE_TYPE_ENT       ruleType;
    union
    {
        struct
        {
            CPSS_DXCH_PCL_RULE_FORMAT_TYPE_ENT ruleFormat;
        } pcl;
        struct
        {
            CPSS_DXCH_TTI_RULE_TYPE_ENT        ruleFormat;
        } tti;
    } rule;
} CPSS_DXCH_VIRTUAL_TCAM_RULE_TYPE_STC;

/*
 * Typedef: struct CPSS_DXCH_VIRTUAL_TCAM_RULE_DATA_STC
 *
 * Description:
 *     This structure of rule data
 *
 * Fields:
 *       valid                - GT_TRUE - Rule is valid, GT_FALSE - invalid.
 *                              Valid status of rule can be updated calling
 *                              cpssDxChVirtualTcamRuleValidStatusSet.
 *
 *       PCL Rule parameters for cpssDxChPclRuleSet/ParsedGet call:
 *       rule.pcl.maskPtr     - (pointer to)rule mask.
 *       rule.pcl.patternPtr  - (pointer to)rule pattern.
 *       rule.pcl.actionPtr   - (pointer to)rule action.
 *
 *       TTI Rule parameters for cpssDxChTtiRuleSet/Get call:
 *       rule.tti.maskPtr     - (pointer to)rule mask.
 *       rule.tti.patternPtr  - (pointer to)rule pattern.
 *       rule.tti.actionPtr   - (pointer to)rule action.
 *                              (using actionPtr->type2)
 *
 *  Comments:
 *      Rule writing/read implemented by calling
 *          PCL - cpssDxChPclRuleSet/ParsedGet
 *          TTI - cpssDxChTtiRuleSet/Get
 *
 */
typedef struct
{
    GT_BOOL                                    valid;
    union
    {
        struct
        {
            CPSS_DXCH_PCL_RULE_FORMAT_UNT      *maskPtr;
            CPSS_DXCH_PCL_RULE_FORMAT_UNT      *patternPtr;
            CPSS_DXCH_PCL_ACTION_STC           *actionPtr;
        } pcl;
        struct
        {
            CPSS_DXCH_TTI_RULE_UNT             *maskPtr;
            CPSS_DXCH_TTI_RULE_UNT             *patternPtr;
            CPSS_DXCH_TTI_ACTION_UNT           *actionPtr;
        } tti;
    } rule;
} CPSS_DXCH_VIRTUAL_TCAM_RULE_DATA_STC;

/*
 * Typedef: struct CPSS_DXCH_VIRTUAL_TCAM_USAGE_STC
 *
 * Description:
 *     This structure to get virtual TCAM current usage.
 *
 * Fields:
 *        rulesUsed   - amount of used rules in virtual TCAM.
 *        rulesFree   - amount of free rules in virtual TCAM.
 *
 *  Comments:
 *
 */
typedef struct
{
    GT_U32 rulesUsed;
    GT_U32 rulesFree;
} CPSS_DXCH_VIRTUAL_TCAM_USAGE_STC;

/*******************************************************************************
* cpssDxChVirtualTcamManagerCreate
*
* DESCRIPTION:
*       Create Virtual TCAM manager.
*
* APPLICABLE DEVICES:
*        xCat3; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2; Lion2.
*
* INPUTS:
*       vTcamMngId       - Virtual TCAM Manager Id
*                          (APPLICABLE RANGES: 0..31)
*
* OUTPUTS:
*       None.
*
* RETURNS :
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_ALREADY_EXIST         - if already exist TCAM Manager with given Id
*       GT_OUT_OF_CPU_MEM        - on CPU memory allocation fail
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChVirtualTcamManagerCreate
(
    IN  GT_U32                              vTcamMngId
);

/*******************************************************************************
* cpssDxChVirtualTcamManagerDelete
*
* DESCRIPTION:
*       Delete Virtual TCAM manager.
*
* APPLICABLE DEVICES:
*        xCat3; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2; Lion2.
*
* INPUTS:
*       vTcamMngId       - Virtual TCAM Manager Id
*                          (APPLICABLE RANGES: 0..31)
*
* OUTPUTS:
*       None.
*
* RETURNS :
*       GT_OK                   - on success
*       GT_BAD_STATE            - on devices still attached to TCAM manager
*       GT_NOT_FOUND            - on TCAM manager not found
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChVirtualTcamManagerDelete
(
    IN  GT_U32                              vTcamMngId
);

/*******************************************************************************
* cpssDxChVirtualTcamManagerDevListAdd
*
* DESCRIPTION:
*   This function adds devices to an existing Virtual TCAM manager.
*   TCAM of one of old devices (if exist) copied to TCAMs the newly added devices.
*
* APPLICABLE DEVICES:
*        xCat3; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2; Lion2.
*
* INPUTS:
*       vTcamMngId       - Virtual TCAM Manager Id
*                          (APPLICABLE RANGES: 0..31)
*       devListArr     - the array of device ids to add to the Virtual TCAM Manager.
*                          (APPLICABLE RANGES: 0..255)
*       numOfDevs      - the number of device ids in the array.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_NOT_FOUND             - if the Virtual TCAM Manager id is not found.
*       GT_BAD_PARAM             - on wrong parameters
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device in devListArr
*       GT_BAD_PTR               - if devListArr is NULL pointer.
*       GT_ALREADY_EXIST         - if device already exist in DB
*
* COMMENTS:
*       NONE
*
*
*******************************************************************************/
GT_STATUS cpssDxChVirtualTcamManagerDevListAdd
(
    IN GT_U32   vTcamMngId,
    IN GT_U8    devListArr[],
    IN GT_U32   numOfDevs
);

/*******************************************************************************
* cpssDxChVirtualTcamManagerDevListRemove
*
* DESCRIPTION:
*   This function removes devices from an existing Virtual TCAM manager.
*   TCAMs of removed devices not updated.
*   Removing last device caused cleaning up all Tcam Manager configurations.
*
* APPLICABLE DEVICES:
*        xCat3; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2; Lion2.
*
* INPUTS:
*       vTcamMngId       - Virtual TCAM Manager Id
*                          (APPLICABLE RANGES: 0..31)
*       devListArr     - the array of device ids to remove from the
*                        Virtual TCAM Manager.
*       numOfDevs      - the number of device ids in the array.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_NOT_FOUND             - if the Virtual TCAM Manager id is not found.
*       GT_BAD_PARAM             - on wrong parameters
*       GT_BAD_PTR               - if devListArr is NULL pointer.
*       GT_NOT_INITIALIZED       - if there are no devices in DB
*
* COMMENTS:
*       NONE
*
*
*******************************************************************************/
GT_STATUS cpssDxChVirtualTcamManagerDevListRemove
(
    IN GT_U32   vTcamMngId,
    IN GT_U8    devListArr[],
    IN GT_U32   numOfDevs
);


/*******************************************************************************
* cpssDxChVirtualTcamDefrag
*
* DESCRIPTION:
*       Defragment TCAM manager memory.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       vTcamMngId       - Virtual TCAM Manager Id
*                          (APPLICABLE RANGES: 0..31)
*
* OUTPUTS:
*       None.
*
* RETURNS :
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device in devListArr
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChVirtualTcamDefrag
(
    IN  GT_U32                              vTcamMngId
);

/*******************************************************************************
* cpssDxChVirtualTcamMemoryAvailabilityCheck
*
* DESCRIPTION:
*       Check availability of TCAM manager memory for list of new and existing Virtual TCAMs.
*
* APPLICABLE DEVICES:
*        xCat3; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2; Lion2.
*
* INPUTS:
*       vTcamMngId        - Virtual TCAM Manager Id
*                           (APPLICABLE RANGES: 0..31)
*       vTcamAmount       - amount of Virtual TCAMs in the array.
*       vTcamCheckInfoArr - array of infos for Virtual TCAMs.
*
*
* OUTPUTS:
*       tcamAvailabilityPtr       - (pointer to)TCAM Availability (see enum)
*
* RETURNS :
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_BAD_PTR               - if pointer or array-pointer is NULL.
*       GT_NOT_FOUND             - TCAM manager with given Id not found
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChVirtualTcamMemoryAvailabilityCheck
(
    IN   GT_U32                                 vTcamMngId,
    IN   GT_U32                                 vTcamAmount,
    IN   CPSS_DXCH_VIRTUAL_TCAM_CHECK_INFO_STC  vTcamCheckInfoArr[],
    OUT  CPSS_DXCH_VIRTUAL_TCAM_AVAILABILTY_ENT *tcamAvailabilityPtr
);

/*******************************************************************************
* cpssDxChVirtualTcamCreate
*
* DESCRIPTION:
*       Create Virtual TCAM.
*
* APPLICABLE DEVICES:
*        xCat3; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2; Lion2.
*
* INPUTS:
*       vTcamMngId     - Virtual TCAM Manager Id
*                        (APPLICABLE RANGES: 0..31)
*       vTcamId        - unique Id of  Virtual TCAM
*       vTcamInfoPtr   - (pointer to) Virtual TCAM info structure
*
* OUTPUTS:
*       None.
*
* RETURNS :
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - on null pointer
*       GT_ALREADY_EXIST         - vTcam with given Id exists
*       GT_NO_RESOURCE           - on tree/buffers resource errors
*       GT_FULL                  - the TCAM space is full for current allocation
*                                   request
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device in devListArr
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChVirtualTcamCreate
(
    IN  GT_U32                           vTcamMngId,
    IN  GT_U32                           vTcamId,
    IN  CPSS_DXCH_VIRTUAL_TCAM_INFO_STC   *vTcamInfoPtr
);

/*******************************************************************************
* cpssDxChVirtualTcamRemove
*
* DESCRIPTION:
*       Remove Virtual TCAM.
*
* APPLICABLE DEVICES:
*        xCat3; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2; Lion2.
*
* INPUTS:
*       vTcamMngId   - Virtual TCAM Manager Id
*                      (APPLICABLE RANGES: 0..31)
*       vTcamId      - unique Id of  Virtual TCAM
*
* OUTPUTS:
*       None.
*
* RETURNS :
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_STATE             - vTcam contains valid rules
*       GT_NOT_FOUND             - vTcam with given Id not found
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device in devListArr
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChVirtualTcamRemove
(
    IN  GT_U32                             vTcamMngId,
    IN  GT_U32                             vTcamId
);

/*******************************************************************************
* cpssDxChVirtualTcamResize
*
* DESCRIPTION:
*       Resize Virtual TCAM. Guaranteed amount of rules updated.
*
* APPLICABLE DEVICES:
*        xCat3; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2; Lion2.
*
* INPUTS:
*       vTcamMngId     - Virtual TCAM Manager Id
*                        (APPLICABLE RANGES: 0..31)
*       vTcamId        - unique Id of  Virtual TCAM
*       rulePlace      - place to insert/remove space after it.
*                        for logical-index driven vTCAMs - logical-index,
*                        for priority driven vTCAMs - priority value.
*                        ----------------------------------------
*                        For logical-index driven vTCAMs:
*                        a) toInsert == GT_TRUE
*                             Function adds free space with size <sizeInRules> rules
*                             after logical-index == rulePlace.
*                             Logical indexes of existing rules higher than inserted space
*                             (i.e. rules with old-logical-index >= rulePlace)
*                             are increased by <sizeInRules>.
*                        b) toInsert == GT_FALSE
*                             Function invalidates existing rules with logical indexes
*                             between <rulePlace> and <rulePlace + sizeInRules - 1>.
*                             This space became free. Function decreases logical indexes of
*                             existing rules higher than freed space
*                             (i.e. rules with old-logical-index >= rulePlace) by <sizeInRules>.
*                        ----------------------------------------
*                        For priority driven vTCAMs:
*                        a) toInsert == GT_TRUE
*                             Function adds free space with size <sizeInRules> rules
*                             after all rules with priority == rulePlace.
*                        b) toInsert == GT_FALSE
*                             Function removes and frees space of <sizeInRules> rules
*                             after all rules with priority == rulePlace.
*                             No rules are removed, but rules can be moved in TCAM
*                             to gather free spaces to one chunk .
*                             Rules with given and lower priorities can be compressed
*                             using free spaces between them.
*                        ----------------------------------------
*                        CPSS_DXCH_VIRTUAL_TCAM_APPEND_CNS - used
*                        for logical-index driven vTCAMs only
*                        to append free space (i.e toInsert == GT_TRUE)
*                        CPSS_DXCH_VIRTUAL_TCAM_BEFORE_START_CNS - used
*                        for logical-index driven vTCAMs only
*                        to insert free space before the 0-th rule (i.e toInsert == GT_TRUE)
*       toInsert       - GT_TRUE - insert rule-places, GT_FALSE - remove rule-places
*                        logical index of rules up to inserted space increased,
*                        logical index of rules up to removed space - decreased,
*                        rules inside removed space - removed.
*       sizeInRules    - amount of rule-space to insert/remove
*
* OUTPUTS:
*       None.
*
* RETURNS :
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_FOUND             - vTcam with given Id not found
*       GT_NO_RESOURCE           - on tree/buffers resource errors
*       GT_FULL                  - the TCAM space is full for current allocation
*                                   request
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device in devListArr
*
* COMMENTS:
*       Example1 (logical-index driven vTCAM):
*           Old vTcam size = 200, rulePlace = 50, toInsert = GT_TRUE, sizeInRules = 100
*           New vTam size is 300.
*           Rules 0-49 logical index unchanged.
*           Rules 50-199 logical index increased by 100 to be 150-299.
*           Place for rules 50-149 added, contains no valid rules.
*       Example2 (logical-index driven vTCAM)
*           Old vTcam size = 300, rulePlace = 50, toInsert = GT_FALSE, sizeInRules = 100
*           New vTam size is 200.
*           Rules 0-49 logical index unchanged.
*           Rules 150-299 logical index decreased by 100 to be 50-199.
*           Place of rules 50-149 removed with all rules that contained.
*
*******************************************************************************/
GT_STATUS cpssDxChVirtualTcamResize
(
    IN  GT_U32                                vTcamMngId,
    IN  GT_U32                                vTcamId,
    IN  GT_U32                                rulePlace,
    IN  GT_BOOL                               toInsert,
    IN  GT_U32                                sizeInRules
);

/*******************************************************************************
* cpssDxChVirtualTcamInfoGet
*
* DESCRIPTION:
*       Get Virtual TCAM info.
*
* APPLICABLE DEVICES:
*        xCat3; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2; Lion2.
*
* INPUTS:
*       vTcamMngId   - Virtual TCAM Manager Id
*                      (APPLICABLE RANGES: 0..31)
*       vTcamId      - unique Id of  Virtual TCAM
*
* OUTPUTS:
*       vTcamInfoPtr    - (pointer to) Virtual TCAM Info structure
*
* RETURNS :
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_BAD_PTR               - on null pointer
*       GT_NOT_FOUND             - vTcam with given Id not found
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChVirtualTcamInfoGet
(
    IN   GT_U32                             vTcamMngId,
    IN   GT_U32                             vTcamId,
    OUT  CPSS_DXCH_VIRTUAL_TCAM_INFO_STC    *vTcamInfoPtr
);

/*******************************************************************************
* cpssDxChVirtualTcamUsageGet
*
* DESCRIPTION:
*       Get Virtual TCAM usage.
*
* APPLICABLE DEVICES:
*        xCat3; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2; Lion2.
*
* INPUTS:
*       vTcamMngId   - Virtual TCAM Manager Id
*                      (APPLICABLE RANGES: 0..31)
*       vTcamId      - unique Id of  Virtual TCAM
*
* OUTPUTS:
*       vTcamUsagePtr   - (pointer to) Virtual TCAM Usage structure
*
* RETURNS :
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_BAD_PTR               - on null pointer
*       GT_NOT_FOUND             - vTcam with given Id not found
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChVirtualTcamUsageGet
(
    IN   GT_U32                             vTcamMngId,
    IN   GT_U32                             vTcamId,
    OUT  CPSS_DXCH_VIRTUAL_TCAM_USAGE_STC   *vTcamUsagePtr
);

/*******************************************************************************
* cpssDxChVirtualTcamNextRuleIdGet
*
* DESCRIPTION:
*        Function gets next existing rule ID.
*
* APPLICABLE DEVICES:
*        xCat3; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2; Lion2.
*
* INPUTS:
*       vTcamMngId        - Virtual TCAM Manager Id
*                           (APPLICABLE RANGES: 0..31)
*       vTcamId           - unique Id of  Virtual TCAM
*       ruleId            - for logical-index driven vTCAM - logical-index
*                           for priority driven vTCAM - ruleId that used to refer existing rule
*                           CPSS_DXCH_VIRTUAL_TCAM_START_RULE_ID_CNS used to start iteration of rules
*
* OUTPUTS:
*       nextRuleIdPtr     -      (pointer to)id of next rule
*
* RETURNS :
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - on null pointer
*       GT_EMPTY                 - on 'START_RULE_ID' indication and no rules exists.
*       GT_NOT_FOUND             - vTcam with given Id not found
*       GT_NO_MORE               - all rule already passed by iteration
*
* COMMENTS:
*      The next rule is once written and not removed rule.
*      It can be valid or invalid.
*      It is the next by lookup order.
*
*******************************************************************************/
GT_STATUS cpssDxChVirtualTcamNextRuleIdGet
(
    IN  GT_U32                                vTcamMngId,
    IN  GT_U32                                vTcamId,
    IN  CPSS_DXCH_VIRTUAL_TCAM_RULE_ID        ruleId,
    OUT CPSS_DXCH_VIRTUAL_TCAM_RULE_ID        *nextRuleIdPtr
);

/*******************************************************************************
* cpssDxChVirtualTcamRuleWrite
*
* DESCRIPTION:
*       Write Rule to Virtual TCAM.
*
* APPLICABLE DEVICES:
*        xCat3; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2; Lion2.
*
* INPUTS:
*       vTcamMngId        - Virtual TCAM Manager Id
*                           (APPLICABLE RANGES: 0..31)
*       vTcamId           - unique Id of  Virtual TCAM
*       ruleId            - for logical-index driven vTCAM - logical-index
*                           for priority driven vTCAM - ruleId that used to refer existing rule
*                           if rule with given rule Id already exists it overridden only
*                           when the same priotity specified, otherwize GT_BAD_PARAM returned.
*       ruleAttributesPtr - (pointer to)rule attributes (for priority driven vTCAM - priority)
*       ruleTypePtr   -     (pointer to)type of contents of rule
*       ruleDataPtr   -     (pointer to)data of contents of rule
*
* OUTPUTS:
*       None
*
* RETURNS :
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - on null pointer
*       GT_NOT_FOUND             - vTcam with given Id not found
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device in devListArr
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChVirtualTcamRuleWrite
(
    IN  GT_U32                                      vTcamMngId,
    IN  GT_U32                                      vTcamId,
    IN  CPSS_DXCH_VIRTUAL_TCAM_RULE_ID              ruleId,
    IN  CPSS_DXCH_VIRTUAL_TCAM_RULE_ATTRIBUTES_STC  *ruleAttributesPtr,
    IN  CPSS_DXCH_VIRTUAL_TCAM_RULE_TYPE_STC        *ruleTypePtr,
    IN  CPSS_DXCH_VIRTUAL_TCAM_RULE_DATA_STC        *ruleDataPtr
);

/*******************************************************************************
* cpssDxChVirtualTcamRuleRead
*
* DESCRIPTION:
*       Read Rule from Virtual TCAM.
*
* APPLICABLE DEVICES:
*        xCat3; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2; Lion2.
*
* INPUTS:
*       vTcamMngId        - Virtual TCAM Manager Id
*                           (APPLICABLE RANGES: 0..31)
*       vTcamId           - unique Id of  Virtual TCAM
*       ruleId            - for logical-index driven vTCAM - logical-index
*                           for priority driven vTCAM - ruleId that used to refer existing rule
*       ruleTypePtr   -     (pointer to)type of contents of rule
*
* OUTPUTS:
*       ruleDataPtr   -      (pointer to)data of contents of rule
*
* RETURNS :
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - on null pointer
*       GT_NOT_FOUND             - vTcam with given Id not found
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device in devListArr
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChVirtualTcamRuleRead
(
    IN  GT_U32                                      vTcamMngId,
    IN  GT_U32                                      vTcamId,
    IN  CPSS_DXCH_VIRTUAL_TCAM_RULE_ID              ruleId,
    IN  CPSS_DXCH_VIRTUAL_TCAM_RULE_TYPE_STC        *ruleTypePtr,
    OUT CPSS_DXCH_VIRTUAL_TCAM_RULE_DATA_STC        *ruleDataPtr
);

/*******************************************************************************
* cpssDxChVirtualTcamRuleActionUpdate
*
* DESCRIPTION:
*       Update Rule Action in Virtual TCAM.
*
* APPLICABLE DEVICES:
*        xCat3; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2; Lion2.
*
* INPUTS:
*       vTcamMngId      - Virtual TCAM Manager Id
*                         (APPLICABLE RANGES: 0..31)
*       vTcamId         - unique Id of  Virtual TCAM
*       ruleId          - for logical-index driven vTCAM - logical-index
*                         for priority driven vTCAM - ruleId that used to refer existing rule
*       actionTypePtr   - (pointer to)type of contents of action
*       actionDataPtr   - (pointer to)data of contents of action
*
* OUTPUTS:
*       None.
*
* RETURNS :
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - on null pointer
*       GT_NOT_FOUND             - vTcam with given Id or rule with rileId not found
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device in devListArr
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChVirtualTcamRuleActionUpdate
(
    IN  GT_U32                                        vTcamMngId,
    IN  GT_U32                                        vTcamId,
    IN  CPSS_DXCH_VIRTUAL_TCAM_RULE_ID                ruleId,
    IN  CPSS_DXCH_VIRTUAL_TCAM_RULE_ACTION_TYPE_STC   *actionTypePtr,
    IN  CPSS_DXCH_VIRTUAL_TCAM_RULE_ACTION_DATA_STC   *actionDataPtr
);

/*******************************************************************************
* cpssDxChVirtualTcamRuleActionGet
*
* DESCRIPTION:
*       Get Rule Action in Virtual TCAM.
*
* APPLICABLE DEVICES:
*        xCat3; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2; Lion2.
*
* INPUTS:
*       vTcamMngId      - Virtual TCAM Manager Id
*                         (APPLICABLE RANGES: 0..31)
*       vTcamId         - unique Id of  Virtual TCAM
*       ruleId          - for logical-index driven vTCAM - logical-index
*                         for priority driven vTCAM - ruleId that used to refer existing rule
*       actionTypePtr   - (pointer to)type of contents of action
*
* OUTPUTS:
*       actionDataPtr   - (pointer to)data of contents of action
*
* RETURNS :
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - on null pointer
*       GT_NOT_FOUND             - vTcam with given Id or rule with rileId not found
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device in devListArr
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChVirtualTcamRuleActionGet
(
    IN  GT_U32                                        vTcamMngId,
    IN  GT_U32                                        vTcamId,
    IN  CPSS_DXCH_VIRTUAL_TCAM_RULE_ID                ruleId,
    IN  CPSS_DXCH_VIRTUAL_TCAM_RULE_ACTION_TYPE_STC   *actionTypePtr,
    OUT CPSS_DXCH_VIRTUAL_TCAM_RULE_ACTION_DATA_STC   *actionDataPtr
);

/*******************************************************************************
* cpssDxChVirtualTcamRuleValidStatusSet
*
* DESCRIPTION:
*       Set Rule Valid Status in Virtual TCAM.
*
* APPLICABLE DEVICES:
*        xCat3; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2; Lion2.
*
* INPUTS:
*       vTcamMngId       - Virtual TCAM Manager Id
*                          (APPLICABLE RANGES: 0..31)
*       vTcamId          - unique Id of  Virtual TCAM
*       ruleId           - for logical-index driven vTCAM - logical-index
*                          for priority driven vTCAM - ruleId that used to refer existing rule
*       valid            - GT_TRUE - valid, GT_FALSE - invalid
*
* OUTPUTS:
*       None.
*
* RETURNS :
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_FOUND             - vTcam with given Id or rule with rileId not found
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device in devListArr
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChVirtualTcamRuleValidStatusSet
(
    IN   GT_U32                             vTcamMngId,
    IN   GT_U32                             vTcamId,
    IN   CPSS_DXCH_VIRTUAL_TCAM_RULE_ID     ruleId,
    IN   GT_BOOL                            valid
);

/*******************************************************************************
* cpssDxChVirtualTcamRuleValidStatusGet
*
* DESCRIPTION:
*       Get Rule Valid Status in Virtual TCAM.
*
* APPLICABLE DEVICES:
*        xCat3; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2; Lion2.
*
* INPUTS:
*       vTcamMngId       - Virtual TCAM Manager Id
*                          (APPLICABLE RANGES: 0..31)
*       vTcamId          - unique Id of  Virtual TCAM
*       ruleId           - for logical-index driven vTCAM - logical-index
*                          for priority driven vTCAM - ruleId that used to refer existing rule
*
* OUTPUTS:
*       validPtr         - (pointer to)GT_TRUE - valid, GT_FALSE - invalid
*
* RETURNS :
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_BAD_PTR               - on null pointer
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_FOUND             - vTcam with given Id or rule with rileId not found
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device in devListArr
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChVirtualTcamRuleValidStatusGet
(
    IN   GT_U32                             vTcamMngId,
    IN   GT_U32                             vTcamId,
    IN   CPSS_DXCH_VIRTUAL_TCAM_RULE_ID     ruleId,
    OUT  GT_BOOL                            *validPtr
);

/*******************************************************************************
* cpssDxChVirtualTcamRuleDelete
*
* DESCRIPTION:
*       Delete Rule From internal DB and Invalidate it in Virtual TCAM.
*
* APPLICABLE DEVICES:
*        xCat3; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2; Lion2.
*
* INPUTS:
*       vTcamMngId       - Virtual TCAM Manager Id
*                          (APPLICABLE RANGES: 0..31)
*       vTcamId          - unique Id of  Virtual TCAM
*       ruleId           - for logical-index driven vTCAM - logical-index
*                          for priority driven vTCAM - ruleId that used to refer existing rule
*
* OUTPUTS:
*       None.
*
* RETURNS :
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_FOUND             - vTcam with given Id or rule with rileId not found
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device in devListArr
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChVirtualTcamRuleDelete
(
    IN   GT_U32                             vTcamMngId,
    IN   GT_U32                             vTcamId,
    IN   CPSS_DXCH_VIRTUAL_TCAM_RULE_ID     ruleId
);

/*******************************************************************************
* cpssDxChVirtualTcamRuleMove
*
* DESCRIPTION:
*       Move Rule in Virtual TCAM.
*
* APPLICABLE DEVICES:
*        xCat3; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2; Lion2.
*
* INPUTS:
*       vTcamMngId       - Virtual TCAM Manager Id
*                          (APPLICABLE RANGES: 0..31)
*       vTcamId          - unique Id of  Virtual TCAM
*       srcRuleId        - source rule id: for logical-index driven vTCAM - logical-index
*                          for priority driven vTCAM - ruleId that used to refer existing rule
*       dstRuleId        - destination rule id: for logical-index driven vTCAM - logical-index
*                          for priority driven vTCAM - ruleId that used to refer existing rule
*
* OUTPUTS:
*       None.
*
* RETURNS :
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_FOUND             - vTcam with given Id or rule with rileId not found
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device in devListArr
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChVirtualTcamRuleMove
(
    IN   GT_U32                             vTcamMngId,
    IN   GT_U32                             vTcamId,
    IN   CPSS_DXCH_VIRTUAL_TCAM_RULE_ID     srcRuleId,
    IN   CPSS_DXCH_VIRTUAL_TCAM_RULE_ID     dstRuleId
);

/*******************************************************************************
* cpssDxChVirtualTcamRulePriorityUpdate
*
* DESCRIPTION:
*       Update rule priority (for priority driven vTCAM).
*
* APPLICABLE DEVICES:
*        xCat3; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2; Lion2.
*
* INPUTS:
*       vTcamMngId       - Virtual TCAM Manager Id
*                          (APPLICABLE RANGES: 0..31)
*       vTcamId          - unique Id of  Virtual TCAM
*       ruleId           - rule id: that used to refer existing rule
*       priority         - the new priority of the rule
*       position         - position in range of rule of the
*                          specified priority (see enum)
*
* OUTPUTS:
*       None.
*
* RETURNS :
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_FULL                  - when the TCAM (not only the vTcam) is FULL
*                                  and moving the rule will require the entry to
*                                  be absent from the HW for some time.
*                                  (the function can not guarantee loss less hits
*                                   on that rule)
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_FOUND             - vTcam with given Id or rule with rileId not found
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device in devListArr
*
* COMMENTS:
*
*
*******************************************************************************/
GT_STATUS cpssDxChVirtualTcamRulePriorityUpdate
(
    IN   GT_U32                                                    vTcamMngId,
    IN   GT_U32                                                    vTcamId,
    IN   CPSS_DXCH_VIRTUAL_TCAM_RULE_ID                            ruleId,
    IN   GT_U32                                                    priority,
    IN   CPSS_DXCH_VIRTUAL_TCAM_EQUAL_PRIORITY_RULE_POSITION_ENT   position
);

/*******************************************************************************
* cpssDxChVirtualTcamDbRuleIdToHwIndexConvert
*
* DESCRIPTION:
*       debug function - convert rule Id to logical index and to hardware index.
*       purpose is to allow application to know where the CPSS inserted the rule
*       in to the hardware.
*       this is 'DB' (DataBase) operation that not access the hardware.
*       NOTE: the function will return failure if the ruleId was not inserted.
*
* APPLICABLE DEVICES:
*        xCat3; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2; Lion2.
*
* INPUTS:
*       vTcamMngId        - Virtual TCAM Manager Id
*                           (APPLICABLE RANGES: 0..31)
*       vTcamId           - unique Id of  Virtual TCAM
*       ruleId            - ruleId to find.
*
* OUTPUTS:
*       logicalIndexPtr  - (pointer to) the logical index.  (can be NULL)
*       hwIndexPtr       - (pointer to) the hardware index. (can be NULL)
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PTR               - if both logicalIndexPtr and hwIndexPtr are NULL
*       GT_BAD_PARAM             - on wrong parameter
*       GT_FAIL                  - on error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChVirtualTcamDbRuleIdToHwIndexConvert
(
    IN  GT_U32                                      vTcamMngId,
    IN  GT_U32                                      vTcamId,
    IN  CPSS_DXCH_VIRTUAL_TCAM_RULE_ID              ruleId,
    OUT  GT_U32                                     *logicalIndexPtr,
    OUT  GT_U32                                     *hwIndexPtr
);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __cpssDxChVirtualTcam_h */

