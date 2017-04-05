/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* prvCpssDxChTcamLog.c
*       WARNING!!! this is a generated file, please don't edit it manually
* COMMENTS:
*
* FILE REVISION NUMBER:
*       $Revision: 1 $
*******************************************************************************/
#include <cpss/generic/log/cpssLog.h>
#include <cpss/generic/log/prvCpssLog.h>
#include <cpss/extServices/private/prvCpssBindFunc.h>
#include <cpss/dxCh/dxChxGen/tcam/cpssDxChTcam.h>
#include <cpss/dxCh/dxChxGen/tcam/private/prvCpssDxChTcamLog.h>
#include <cpss/generic/log/prvCpssGenCommonTypesLog.h>
#include <cpss/generic/log/prvCpssGenDbLog.h>


/********* enums *********/

char * prvCpssLogEnum_CPSS_DXCH_TCAM_CLIENT_ENT[]  =
{
    "CPSS_DXCH_TCAM_IPCL_0_E",
    "CPSS_DXCH_TCAM_IPCL_1_E",
    "CPSS_DXCH_TCAM_IPCL_2_E",
    "CPSS_DXCH_TCAM_EPCL_E",
    "CPSS_DXCH_TCAM_TTI_E"
};
PRV_CPSS_LOG_STC_ENUM_ARRAY_SIZE_MAC(CPSS_DXCH_TCAM_CLIENT_ENT);
char * prvCpssLogEnum_CPSS_DXCH_TCAM_RULE_SIZE_ENT[]  =
{
    "CPSS_DXCH_TCAM_RULE_SIZE_10_B_E",
    "CPSS_DXCH_TCAM_RULE_SIZE_20_B_E",
    "CPSS_DXCH_TCAM_RULE_SIZE_30_B_E",
    "CPSS_DXCH_TCAM_RULE_SIZE_40_B_E",
    "CPSS_DXCH_TCAM_RULE_SIZE_50_B_E",
    "CPSS_DXCH_TCAM_RULE_SIZE_60_B_E",
    "CPSS_DXCH_TCAM_RULE_SIZE_80_B_E"
};
PRV_CPSS_LOG_STC_ENUM_ARRAY_SIZE_MAC(CPSS_DXCH_TCAM_RULE_SIZE_ENT);


/********* structure fields log functions *********/

void prvCpssLogParamFuncStc_CPSS_DXCH_TCAM_BLOCK_INFO_STC_PTR
(
    IN CPSS_LOG_LIB_ENT         contextLib,
    IN CPSS_LOG_TYPE_ENT        logType,
    IN GT_CHAR_PTR              namePtr,
    IN void                   * fieldPtr,
    INOUT PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC     * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_START_FIELD_STC_MAC(CPSS_DXCH_TCAM_BLOCK_INFO_STC *, valPtr);
    PRV_CPSS_LOG_UNUSED_MAC(inOutParamInfoPtr);
    PRV_CPSS_LOG_STC_NUMBER_MAC(valPtr, group);
    PRV_CPSS_LOG_STC_NUMBER_MAC(valPtr, hitNum);
    prvCpssLogStcLogEnd(contextLib, logType);
}


/********* parameters log functions *********/

void prvCpssLogParamFunc_CPSS_DXCH_TCAM_CLIENT_ENT(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_SET_PARAM_VAL_MAC(CPSS_DXCH_TCAM_CLIENT_ENT, paramVal);
    PRV_CPSS_LOG_ENUM_MAC(namePtr, paramVal, CPSS_DXCH_TCAM_CLIENT_ENT);
}
void prvCpssLogParamFunc_CPSS_DXCH_TCAM_RULE_SIZE_ENT(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_SET_PARAM_VAL_MAC(CPSS_DXCH_TCAM_RULE_SIZE_ENT, paramVal);
    PRV_CPSS_LOG_ENUM_MAC(namePtr, paramVal, CPSS_DXCH_TCAM_RULE_SIZE_ENT);
}
void prvCpssLogParamFunc_CPSS_DXCH_TCAM_RULE_SIZE_ENT_PTR(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_SET_PARAM_VAL_MAC(CPSS_DXCH_TCAM_RULE_SIZE_ENT*, paramVal);
    if (paramVal == NULL)
    {
        PRV_CPSS_LOG_AND_HISTORY_PARAM_MAC(contextLib, logType, "%s = NULL\n", namePtr);
        return;
    }
    PRV_CPSS_LOG_ENUM_MAC(namePtr, *paramVal, CPSS_DXCH_TCAM_RULE_SIZE_ENT);
}


/********* API fields DB *********/

PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_DXCH_TCAM_BLOCK_INFO_STC_PTR_floorInfoArr = {
     "floorInfoArr", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(CPSS_DXCH_TCAM_BLOCK_INFO_STC)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_DXCH_TCAM_CLIENT_ENT_tcamClient = {
     "tcamClient", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_MAC(CPSS_DXCH_TCAM_CLIENT_ENT)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_DXCH_TCAM_RULE_SIZE_ENT_ruleSize = {
     "ruleSize", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_MAC(CPSS_DXCH_TCAM_RULE_SIZE_ENT)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_DXCH_TCAM_RULE_SIZE_ENT_size = {
     "size", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_MAC(CPSS_DXCH_TCAM_RULE_SIZE_ENT)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_U32_PTR_tcamKeyPtr = {
     "tcamKeyPtr", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(GT_U32)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_U32_PTR_tcamMaskPtr = {
     "tcamMaskPtr", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(GT_U32)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_U32_PTR_tcamPatternPtr = {
     "tcamPatternPtr", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(GT_U32)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_U32_floorIndex = {
     "floorIndex", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_MAC(GT_U32)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_U32_hitNum = {
     "hitNum", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_MAC(GT_U32)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_U32_numOfActiveFloors = {
     "numOfActiveFloors", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_MAC(GT_U32)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_U32_tcamGroup = {
     "tcamGroup", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_MAC(GT_U32)
};
PRV_CPSS_LOG_FUNC_PARAM_STC OUT_CPSS_DXCH_TCAM_BLOCK_INFO_STC_PTR_floorInfoArr = {
     "floorInfoArr", PRV_CPSS_LOG_PARAM_OUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(CPSS_DXCH_TCAM_BLOCK_INFO_STC)
};
PRV_CPSS_LOG_FUNC_PARAM_STC OUT_CPSS_DXCH_TCAM_RULE_SIZE_ENT_PTR_ruleSizePtr = {
     "ruleSizePtr", PRV_CPSS_LOG_PARAM_OUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(CPSS_DXCH_TCAM_RULE_SIZE_ENT)
};
PRV_CPSS_LOG_FUNC_PARAM_STC OUT_CPSS_DXCH_TCAM_RULE_SIZE_ENT_PTR_sizePtr = {
     "sizePtr", PRV_CPSS_LOG_PARAM_OUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(CPSS_DXCH_TCAM_RULE_SIZE_ENT)
};
PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_BOOL_PTR_isHitPtr = {
     "isHitPtr", PRV_CPSS_LOG_PARAM_OUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(GT_BOOL)
};
PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_U32_PTR_hitIndexPtr = {
     "hitIndexPtr", PRV_CPSS_LOG_PARAM_OUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(GT_U32)
};
PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_U32_PTR_numOfActiveFloorsPtr = {
     "numOfActiveFloorsPtr", PRV_CPSS_LOG_PARAM_OUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(GT_U32)
};
PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_U32_PTR_tcamGroupPtr = {
     "tcamGroupPtr", PRV_CPSS_LOG_PARAM_OUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(GT_U32)
};
PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_U32_PTR_tcamKeyPtr = {
     "tcamKeyPtr", PRV_CPSS_LOG_PARAM_OUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(GT_U32)
};
PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_U32_PTR_tcamMaskPtr = {
     "tcamMaskPtr", PRV_CPSS_LOG_PARAM_OUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(GT_U32)
};
PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_U32_PTR_tcamPatternPtr = {
     "tcamPatternPtr", PRV_CPSS_LOG_PARAM_OUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(GT_U32)
};


/********* API prototypes DB *********/

PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChTcamPortGroupClientGroupSet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_PORT_GROUPS_BMP_portGroupsBmp,
    &IN_CPSS_DXCH_TCAM_CLIENT_ENT_tcamClient,
    &IN_GT_U32_tcamGroup,
    &IN_GT_BOOL_enable
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChTcamPortGroupClientGroupGet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_PORT_GROUPS_BMP_portGroupsBmp,
    &IN_CPSS_DXCH_TCAM_CLIENT_ENT_tcamClient,
    &OUT_GT_U32_PTR_tcamGroupPtr,
    &OUT_GT_BOOL_PTR_enablePtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChTcamCpuLookupResultsGet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_PORT_GROUPS_BMP_portGroupsBmp,
    &IN_GT_U32_group,
    &IN_GT_U32_hitNum,
    &OUT_GT_BOOL_PTR_isValidPtr,
    &OUT_GT_BOOL_PTR_isHitPtr,
    &OUT_GT_U32_PTR_hitIndexPtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChTcamPortGroupRuleValidStatusGet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_PORT_GROUPS_BMP_portGroupsBmp,
    &IN_GT_U32_index,
    &OUT_GT_BOOL_PTR_validPtr,
    &OUT_CPSS_DXCH_TCAM_RULE_SIZE_ENT_PTR_ruleSizePtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChTcamIndexRangeHitNumAndGroupSet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_U32_floorIndex,
    &IN_CPSS_DXCH_TCAM_BLOCK_INFO_STC_PTR_floorInfoArr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChTcamIndexRangeHitNumAndGroupGet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_U32_floorIndex,
    &OUT_CPSS_DXCH_TCAM_BLOCK_INFO_STC_PTR_floorInfoArr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChTcamActiveFloorsSet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_U32_numOfActiveFloors
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChTcamActiveFloorsGet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &OUT_GT_U32_PTR_numOfActiveFloorsPtr
};
extern PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChTcamPortGroupCpuLookupTriggerGet_PARAMS[];
extern PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChTcamPortGroupCpuLookupTriggerSet_PARAMS[];
extern PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChTcamPortGroupRuleRead_PARAMS[];
extern PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChTcamPortGroupRuleWrite_PARAMS[];


/********* lib API DB *********/

extern void cpssDxChTcamPortGroupRuleWrite_preLogic(IN va_list  args, OUT PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC * paramDataPtr);
extern void cpssDxChTcamPortGroupRuleRead_preLogic(IN va_list  args, OUT PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC * paramDataPtr);
extern void cpssDxChTcamIndexRangeHitNumAndGroupSet_preLogic(IN va_list  args, OUT PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC * paramDataPtr);
extern void cpssDxChTcamIndexRangeHitNumAndGroupSet_preLogic(IN va_list  args, OUT PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC * paramDataPtr);
extern void cpssDxChTcamPortGroupCpuLookupTriggerSet_preLogic(IN va_list  args, OUT PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC * paramDataPtr);
extern void cpssDxChTcamPortGroupCpuLookupTriggerGet_preLogic(IN va_list  args, OUT PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC * paramDataPtr);

static PRV_CPSS_LOG_FUNC_ENTRY_STC prvCpssDxChTcamLogLibDb[] = {
    {"cpssDxChTcamPortGroupRuleWrite", 7, cpssDxChTcamPortGroupRuleWrite_PARAMS, cpssDxChTcamPortGroupRuleWrite_preLogic},
    {"cpssDxChTcamPortGroupRuleRead", 7, cpssDxChTcamPortGroupRuleRead_PARAMS, cpssDxChTcamPortGroupRuleRead_preLogic},
    {"cpssDxChTcamPortGroupRuleValidStatusSet", 4, prvCpssLogGenDevNumPortGroupsBmpIndexValid_PARAMS, NULL},
    {"cpssDxChTcamPortGroupRuleValidStatusGet", 5, cpssDxChTcamPortGroupRuleValidStatusGet_PARAMS, NULL},
    {"cpssDxChTcamPortGroupClientGroupSet", 5, cpssDxChTcamPortGroupClientGroupSet_PARAMS, NULL},
    {"cpssDxChTcamPortGroupClientGroupGet", 5, cpssDxChTcamPortGroupClientGroupGet_PARAMS, NULL},
    {"cpssDxChTcamIndexRangeHitNumAndGroupSet", 3, cpssDxChTcamIndexRangeHitNumAndGroupSet_PARAMS, cpssDxChTcamIndexRangeHitNumAndGroupSet_preLogic},
    {"cpssDxChTcamIndexRangeHitNumAndGroupGet", 3, cpssDxChTcamIndexRangeHitNumAndGroupGet_PARAMS, cpssDxChTcamIndexRangeHitNumAndGroupSet_preLogic},
    {"cpssDxChTcamActiveFloorsSet", 2, cpssDxChTcamActiveFloorsSet_PARAMS, NULL},
    {"cpssDxChTcamActiveFloorsGet", 2, cpssDxChTcamActiveFloorsGet_PARAMS, NULL},
    {"cpssDxChTcamPortGroupCpuLookupTriggerSet", 5, cpssDxChTcamPortGroupCpuLookupTriggerSet_PARAMS, cpssDxChTcamPortGroupCpuLookupTriggerSet_preLogic},
    {"cpssDxChTcamPortGroupCpuLookupTriggerGet", 5, cpssDxChTcamPortGroupCpuLookupTriggerGet_PARAMS, cpssDxChTcamPortGroupCpuLookupTriggerGet_preLogic},
    {"cpssDxChTcamCpuLookupResultsGet", 7, cpssDxChTcamCpuLookupResultsGet_PARAMS, NULL},
};

/******** DB Access Function ********/
void prvCpssLogParamLibDbGet_CPSS_LOG_LIB_TCAM(
    OUT PRV_CPSS_LOG_FUNC_ENTRY_STC ** logFuncDbPtrPtr,
    OUT GT_U32 * logFuncDbSizePtr
)
{
    *logFuncDbPtrPtr = prvCpssDxChTcamLogLibDb;
    *logFuncDbSizePtr = sizeof(prvCpssDxChTcamLogLibDb) / sizeof(PRV_CPSS_LOG_FUNC_ENTRY_STC);
}

