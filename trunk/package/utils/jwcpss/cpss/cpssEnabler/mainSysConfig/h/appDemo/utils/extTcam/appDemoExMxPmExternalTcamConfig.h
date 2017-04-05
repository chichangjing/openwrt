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
* appDemoExMxPmExternalTcamConfig.h
*
* DESCRIPTION:
*       App demo ExMxPm External TCAM configuration API header.
*
* FILE REVISION NUMBER:
*       $Revision: 3 $
*******************************************************************************/

#ifndef __appDemoExMxPmExternalTcamConfigh
#define __appDemoExMxPmExternalTcamConfigh

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include <cpss/exMxPm/exMxPmGen/extTcam/cpssExMxPmExternalTcam.h>
#include <appDemo/sysHwConfig/gtAppDemoSysConfig.h>

/* 3 rule types length in external TCAM - 160, 320 & 640 bits */
#define APP_DEMO_EXTERNAL_TCAM_RULE_LEN_NUM_MAC 3

/* the number of client and key combinations */
#define APP_DEMO_EXTERNAL_TCAM_CLIENT_KEY_COMB_MAC  44

/* end of rule num table flag */
#define APP_DEMO_EXTERNAL_TCAM_EOT_MAC  99

/* for each rule 4 lines are reserved in CSU-0 to support all types of actions  */
#define APP_DEMO_EXTERNAL_TACM_LINES_PER_ACTION_MAC 4

/* 32 blocks in NL TCAM */
#define APP_DEMO_NL_TCAM_NUM_OF_BLOCKS_MAC  32

/* 4096 TCAM lines in each NL block */
#define APP_DEMO_NL_TCAM_LINES_IN_BLOCK_MAC 4096

/* 32 segments in IDT TCAM */
#define APP_DEMO_IDT_TCAM_NUM_OF_SEGMENTS_MAC  32

/* 4096 TCAM lines in each IDT segment */
#define APP_DEMO_IDT_TCAM_LINES_IN_SEGMENT_MAC 4096

/* the maximal number of default values of rule numbers */
/* per <client, key, lookup) that can be overwritten    */
#define APP_DEMO_EXTERNAL_TCAM_MAX_DEFALT_RULE_NUMS_OVERWRITTEN_MAC   50

/* macro for finding rule type index in the management database     */
/* _ruleSize    - rule type as defined by the enumeration           */
/*                CPSS_EXMXPM_EXTERNAL_TCAM_RULE_SIZE_ENT.          */
/* _ruleIndex   - the index in the management database              */
#define APP_DEMO_EXTERNAL_TCAM_GET_RULE_SIZE_INDEX_MAC(_ruleSize, _ruleIndex)   \
    switch(_ruleSize)                                                           \
    {                                                                           \
        case CPSS_EXMXPM_EXTERNAL_TCAM_RULE_SIZE_160_E: _ruleIndex = 0;         \
                                                        break;                  \
        case CPSS_EXMXPM_EXTERNAL_TCAM_RULE_SIZE_320_E: _ruleIndex = 1;         \
                                                        break;                  \
        case CPSS_EXMXPM_EXTERNAL_TCAM_RULE_SIZE_640_E: _ruleIndex = 2;         \
                                                        break;                  \
        default: return GT_BAD_PARAM;                                           \
                 break;                                                         \
    }

/*
 * typedef: struct APP_DEMO_EXTERNAL_TCAM_OVERWRITTEN_DEFAULT_STC
 *
 * Description:
 *      Entry for collecting information of the number of rules requested
 *      for client, key and lookup combination that will overwrite the
 *      default values.
 *
 * Fields:
 *       devNum             - device number
 *       tcamClient         - EXMXPM device External TCAM unit client.
 *       clientKeyType      - The selected key type enumeration per client:
 *                            please refer to Functional Spec for key types number.
 *       lookupIndex        - The number of lookup.
 *       requestedRulesNum  - the number of rules requested (will overwrite the
 *                            default value).
 *       inUse              - entry already in use (GT_TRUE) or not yet used (GT_FALSE)
 *
 * Comment:
 */
typedef struct
{
  GT_U8                                 devNum;
  CPSS_EXMXPM_EXTERNAL_TCAM_CLIENT_ENT  tcamClient;
  GT_U32                                clientKeyType;
  GT_U32                                lookupIndex;
  GT_U32                                requestedRulesNum;
  GT_BOOL                               inUse;
} APP_DEMO_EXTERNAL_TCAM_OVERWRITTEN_DEFAULT_STC;

/*
 * typedef: struct APP_DEMO_EXTERNAL_TCAM_NUM_OF_RULES_STC
 *
 * Description:
 *      Entry for collecting information of the number of rules
 *      for client, key and lookup combination.
 *
 * Fields:
 *       lookupIndex        - The number of lookup.
 *       tcamClient         - EXMXPM device External TCAM unit client.
 *       clientKeyType      - The selected key type enumeration per client:
 *                            please refer to Functional Spec for key types number.
 *       ruleSize            - size of rule entry.
 *       numOfRules         - the number of rules for this combination.
 *
 * Comment:
 */
typedef struct
{
  GT_U32                                    lookupIndex;
  CPSS_EXMXPM_EXTERNAL_TCAM_CLIENT_ENT      tcamClient;
  GT_U32                                    clientKeyType;
  CPSS_EXMXPM_EXTERNAL_TCAM_RULE_SIZE_ENT   ruleSize;
  GT_U32                                    numOfRules;
} APP_DEMO_EXTERNAL_TCAM_NUM_OF_RULES_STC;

/*
 * typedef: struct APP_DEMO_EXTERNAL_TCAM_KEY_LOOKUP_STC
 *
 * Description:
 *      Parameters for the external TCAM management, used for each external TCAM
 *      rule size (160, 320 or 640 bits) and lookup number combination.
 *
 * Fields:
 *       ruleStartIndex     - index of rule in external TCAM where area for rule size
 *                            in the lookup starts.
 *       maxRuleNum         - the maximal number of rules that can reside in the
 *                            specified area.
 *       actionStartIndex   - relative index of control memory line of external
 *                            TCAM actions chunk.
 *
 * Comment:
 */
typedef struct APP_DEMO_EXTERNAL_TCAM_KEY_LOOKUP_STCT
{
  GT_U32    ruleStartIndex;
  GT_U32    maxRuleNum;
  GT_U32    actionStartIndex;
} APP_DEMO_EXTERNAL_TCAM_KEY_LOOKUP_STC;

extern GT_U32 appDemoExternalTcamNumOflookups;
extern APP_DEMO_EXTERNAL_TCAM_NUM_OF_RULES_STC *appDemoNumOfRulesTblPtr;

/*******************************************************************************
* appDemoExMxPmExternalTcamMngInit
*
* DESCRIPTION:
*       Initialize the external TCAM management structure.
*
* APPLICABLE DEVICES:  All ExMxPm devices
*
* INPUTS:
*       devNum          - device number
*
* OUTPUTS:
*       logicalConfigParamsPtr  -   (pointer to) PP logical configuration parameters
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_BAD_PTR               - on null pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       This function allocate the memory for the external TCAM management
*       database and start to initialize it.
*       It calculates the number of lines needed to be allocated from the
*       control memory and used for actions.
*       This function must be called prior to call to appDemoDxPpLogicalInit
*       since it initializes external TCAM relevant parameters in
*       logicalConfigParamsPtr pointed structure.
*       The function does not configure HW -> only the output structure.
*
*******************************************************************************/
GT_STATUS appDemoExMxPmExternalTcamMngInit
(
    IN  GT_U8                        devNum,
    OUT  CPSS_PP_CONFIG_INIT_STC     *logicalConfigParamsPtr
);

/*******************************************************************************
* appDemoExMxPmExternalTcamRuleIndexByLenAndLookupGet
*
* DESCRIPTION:
*       The function gets the rule start index and the maximum number of
*       rules that describe the external TCAM rule area appropriate to
*       a rule size and lookup number combination.
*
* APPLICABLE DEVICES:  All ExMxPm devices
*
* INPUTS:
*       devNum              - device number
*       ruleSize            - size of rule entry.
*       lookupIndex         - the number of lookup.
*
* OUTPUTS:
*       ruleStartIndexPtr   - (pointer to) rule start index number of the area.
*       maxRuleNumPtr       - (pointer to) the maximal number of rules that can
*                             be inserted into the area.
*       linesPerRulePtr     - (pointer to) how many external TCAM lines a rule
*                             of ruleSize type consumes.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_BAD_PTR               - on null pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS appDemoExMxPmExternalTcamRuleIndexByLenAndLookupGet
(
    IN GT_U8                                        devNum,
    IN  CPSS_EXMXPM_EXTERNAL_TCAM_RULE_SIZE_ENT     ruleSize,
    IN  GT_U32                                      lookupIndex,
    OUT GT_U32                                      *ruleStartIndexPtr,
    OUT GT_U32                                      *maxRuleNumPtr,
    OUT GT_U32                                      *linesPerRulePtr
);

/*******************************************************************************
* appDemoExMxPmExternalTcamActionIndexGet
*
* DESCRIPTION:
*       The function gets the line in control memory area allocated for external
*       TCAM actions for a defined rule line in external TCAM.
*
* APPLICABLE DEVICES:  All ExMxPm devices
*
* INPUTS:
*       devNum              - device number
*       ruleSize            - size of rule entry.
*       lookupIndex         - the number of lookup.
*       ruleStartIndex      - rule start index.
*
* OUTPUTS:
*       actionStartIndexPtr - (pointer to) action start index.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_BAD_PTR               - on null pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS appDemoExMxPmExternalTcamActionIndexGet
(
    IN  GT_U8                                       devNum,
    IN  CPSS_EXMXPM_EXTERNAL_TCAM_RULE_SIZE_ENT     ruleSize,
    IN  GT_U32                                      lookupIndex,
    IN  GT_U32                                      ruleStartIndex,
    OUT GT_U32                                      *actionStartIndexPtr
);

/*******************************************************************************
* appDemoExMxPmExternalTcamClientKeyLookupSet
*
* DESCRIPTION:
*       This function sets the value of a number of rules requested for a
*       device, client, key and lookup combination later to be used and
*       overwrite the default system value.
*
* APPLICABLE DEVICES:  All ExMxPm devices
*
* INPUTS:
*       devNum              - device number
*       tcamClient          - EXMXPM device External TCAM unit client.
*       clientKeyType       - The selected key type enumeration per client:
*                             please refer to Functional Spec for key types number.
*       lookupIndex         - The number of lookup.
*       requestedRulesNum   - the number of rules requested (will overwrite the
*                             default value).
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_NO_RESOURCE           - no place to save the request
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       In order to effect the system external TCAM configuration this
*       function must be called before cpssInitSystem!!!
*
*******************************************************************************/
GT_STATUS appDemoExMxPmExternalTcamClientKeyLookupSet
(
  IN GT_U8                                 devNum,
  IN CPSS_EXMXPM_EXTERNAL_TCAM_CLIENT_ENT  tcamClient,
  IN GT_U32                                clientKeyType,
  IN GT_U32                                lookupIndex,
  IN GT_U32                                requestedRulesNum
);

/*******************************************************************************
* appDemoExMxPmExternalTcamClientKeyLookupGet
*
* DESCRIPTION:
*       This function gets the value of a number of rules requested for a
*       device, client, key and lookup combination later to be used and
*       overwrite the default system value.
*
* APPLICABLE DEVICES:  All ExMxPm devices
*
* INPUTS:
*       devNum                  - device number
*       tcamClient              - EXMXPM device External TCAM unit client.
*       clientKeyType           - The selected key type enumeration per client:
*                                 please refer to Functional Spec for key types number.
*       lookupIndex             - The number of lookup.
*
* OUTPUTS:
*       requestedRulesNumPtr    - (pointer to) the number of rules requested.
*       validPtr                - (pointer to) indication whether a match to the
*                                 device, client, key and lookup was found.
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_BAD_PTR               - on null pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS appDemoExMxPmExternalTcamClientKeyLookupGet
(
    IN GT_U8                                 devNum,
    IN CPSS_EXMXPM_EXTERNAL_TCAM_CLIENT_ENT  tcamClient,
    IN GT_U32                                clientKeyType,
    IN GT_U32                                lookupIndex,
    OUT GT_U32                               *requestedRulesNumPtr,
    OUT GT_BOOL                              *validPtr
);

/*******************************************************************************
* appDemoExMxPmExternalTcamClientKeyLookupSetByIndex
*
* DESCRIPTION:
*       This function sets an entry (specified by index) in the database
*       of values to overwrite the default of maximum number of rules for
*       device, client, key and lookup combination.
*
* APPLICABLE DEVICES:  All ExMxPm devices
*
* INPUTS:
*       entryIndex  - entry index in database.
*       entryPtr    - (pointer to) database entry.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_BAD_PTR               - on null pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS appDemoExMxPmExternalTcamClientKeyLookupSetByIndex
(
    IN GT_U32                                           entryIndex,
    IN APP_DEMO_EXTERNAL_TCAM_OVERWRITTEN_DEFAULT_STC   *entryPtr
);

/*******************************************************************************
* appDemoExMxPmExternalTcamClientKeyLookupGetByIndex
*
* DESCRIPTION:
*       This function gets an entry (specified by index) from the database
*       of values to overwrite the default of maximum number of rules for
*       device, client, key and lookup combination.
*
* APPLICABLE DEVICES:  All ExMxPm devices
*
* INPUTS:
*       entryIndex  - entry index in database.
*
* OUTPUTS:
*       entryPtr    - (pointer to) database entry.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_BAD_PTR               - on null pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS appDemoExMxPmExternalTcamClientKeyLookupGetByIndex
(
    IN GT_U32                                           entryIndex,
    OUT APP_DEMO_EXTERNAL_TCAM_OVERWRITTEN_DEFAULT_STC  *entryPtr
);

/*******************************************************************************
* appDemoExMxPmExternalTcamConfig
*
* DESCRIPTION:
*       Configure external TCAM device (by setting appropriate values to external
*       TCAM registers, not ETC registers) based on the external TCAM management
*       database content.
*       This function configure the ruleStartIndex fields in the external TCAM
*       management database.
*
* APPLICABLE DEVICES:  All ExMxPm devices
*
* INPUTS:
*       devNum          - device number
*
* OUTPUTS:
*       None.

* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       Since the external TCAM type used is required this function is called
*       after CPSS database is initialized with external TCAM type.
*
*******************************************************************************/
GT_STATUS appDemoExMxPmExternalTcamConfig
(
    IN  GT_U8                        devNum
);

/*******************************************************************************
* appDemoExMxPmExternalTcamCalcIndexConvertParams
*
* DESCRIPTION:
*       Calculate the hit index conversion params for a specified lookup and
*       key type.
*
* APPLICABLE DEVICES:  All ExMxPm devices
*
* INPUTS:
*       devNum          - device number
*       lookupIndex     - the lookup number
*       ruleSize        - rule size in external TCAM
*
* OUTPUTS:
*       indexConvertPtr - (pointer to) hit index conversion params.

* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS appDemoExMxPmExternalTcamCalcIndexConvertParams
(
    IN  GT_U8                                            devNum,
    IN  GT_U32                                           lookupIndex,
    IN  CPSS_EXMXPM_EXTERNAL_TCAM_RULE_SIZE_ENT          ruleSize,
    OUT  CPSS_EXMXPM_EXTERNAL_TCAM_HIT_INDEX_CONVERT_STC *indexConvertPtr
);

/*******************************************************************************
* appDemoExMxPmExternalTcamControllerConfig
*
* DESCRIPTION:
*       Configure external TCAM controller (ETC). ETC configuration involves
*       setting the Client Search and Base Address Conversion tables.
*
* APPLICABLE DEVICES:  All ExMxPm devices
*
* INPUTS:
*       devNum          - device number
*
* OUTPUTS:
*       None.

* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       This function is called after the external TCAM was configured
*       (and therefore the ruleStartIndex fields in the external TCAM
*       management database were configured).
*
*******************************************************************************/
GT_STATUS appDemoExMxPmExternalTcamControllerConfig
(
    IN  GT_U8                        devNum
);

/*******************************************************************************
* appDemoExMxPmExternalTcamPclConfig
*
* DESCRIPTION:
*       Configure PCL engine for external TCAM.
*
* APPLICABLE DEVICES:  All ExMxPm devices
*
* INPUTS:
*       devNum          - device number
*
* OUTPUTS:
*       None.

* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS appDemoExMxPmExternalTcamPclConfig
(
    IN  GT_U8                        devNum
);

/*******************************************************************************
* appDemoExMxPmExternalTcamRuleAndActionIndexGet
*
* DESCRIPTION:
*       The function used for conversion relative indexes of rules to absolute
*       Rule Start Index and Action Table entry index as used in API.
*
* APPLICABLE DEVICES:  All ExMxPm devices
*
* INPUTS:
*       devNum              - device number
*       ruleSize            - size of rule entry.
*       lookupIndex         - the number of lookup (0 - single, 1 - double).
*       ruleRelativeIndex   - rule relative index in the range dedicated
*                             for rules with such ruleSize and lookupIndex
*                             measured in rules of such ruleSize.
*
* OUTPUTS:
*       ruleStartIndexPtr   - (pointer to) rule start index.
*       actionStartIndexPtr - (pointer to) action start index.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_BAD_PTR               - on null pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS appDemoExMxPmExternalTcamRuleAndActionIndexGet
(
    IN  GT_U8                                       devNum,
    IN  CPSS_EXMXPM_EXTERNAL_TCAM_RULE_SIZE_ENT     ruleSize,
    IN  GT_U32                                      lookupIndex,
    IN  GT_U32                                      ruleRelativeIndex,
    OUT GT_U32                                      *ruleStartIndexPtr,
    OUT GT_U32                                      *actionStartIndexPtr
);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __appDemoExMxPmExternalTcamConfigh */

