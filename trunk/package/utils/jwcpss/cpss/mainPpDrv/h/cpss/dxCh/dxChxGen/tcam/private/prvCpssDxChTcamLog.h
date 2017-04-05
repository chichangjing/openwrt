/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* prvCpssDxChTcamLog.h
*       WARNING!!! this is a generated file, please don't edit it manually
* COMMENTS:
*
* FILE REVISION NUMBER:
*       $Revision: 1 $
*******************************************************************************/
#ifndef __prvCpssDxChTcamLogh
#define __prvCpssDxChTcamLogh
#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
#include <cpss/generic/log/cpssLog.h>
#include <cpss/generic/log/prvCpssLog.h>


/********* manually implemented declarations *********/

void prvCpssLogParamFunc_CPSS_DXCH_TCAM_BLOCK_INFO_STC_PTR(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
);


/********* enums *********/

PRV_CPSS_LOG_STC_ENUM_DECLARE_MAC(CPSS_DXCH_TCAM_CLIENT_ENT);
PRV_CPSS_LOG_STC_ENUM_DECLARE_MAC(CPSS_DXCH_TCAM_RULE_SIZE_ENT);


/********* structure fields log functions *********/

void prvCpssLogParamFuncStc_CPSS_DXCH_TCAM_BLOCK_INFO_STC_PTR
(
    IN CPSS_LOG_LIB_ENT         contextLib,
    IN CPSS_LOG_TYPE_ENT        logType,
    IN GT_CHAR_PTR              namePtr,
    IN void                   * fieldPtr,
    INOUT PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC     * inOutParamInfoPtr
);


/********* parameters log functions *********/

void prvCpssLogParamFunc_CPSS_DXCH_TCAM_CLIENT_ENT(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
);
void prvCpssLogParamFunc_CPSS_DXCH_TCAM_RULE_SIZE_ENT(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
);
void prvCpssLogParamFunc_CPSS_DXCH_TCAM_RULE_SIZE_ENT_PTR(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
);


/********* API fields DB *********/

extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_DXCH_TCAM_BLOCK_INFO_STC_PTR_floorInfoArr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_DXCH_TCAM_CLIENT_ENT_tcamClient;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_DXCH_TCAM_RULE_SIZE_ENT_ruleSize;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_DXCH_TCAM_RULE_SIZE_ENT_size;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_U32_PTR_tcamKeyPtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_U32_PTR_tcamMaskPtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_U32_PTR_tcamPatternPtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_U32_floorIndex;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_U32_hitNum;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_U32_numOfActiveFloors;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_U32_tcamGroup;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_CPSS_DXCH_TCAM_BLOCK_INFO_STC_PTR_floorInfoArr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_CPSS_DXCH_TCAM_RULE_SIZE_ENT_PTR_ruleSizePtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_CPSS_DXCH_TCAM_RULE_SIZE_ENT_PTR_sizePtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_BOOL_PTR_isHitPtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_U32_PTR_hitIndexPtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_U32_PTR_numOfActiveFloorsPtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_U32_PTR_tcamGroupPtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_U32_PTR_tcamKeyPtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_U32_PTR_tcamMaskPtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_U32_PTR_tcamPatternPtr;


/********* lib API DB *********/

enum {
    PRV_CPSS_LOG_FUNC_cpssDxChTcamPortGroupRuleWrite_E = (CPSS_LOG_LIB_TCAM_E << 16),
    PRV_CPSS_LOG_FUNC_cpssDxChTcamPortGroupRuleRead_E,
    PRV_CPSS_LOG_FUNC_cpssDxChTcamPortGroupRuleValidStatusSet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChTcamPortGroupRuleValidStatusGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChTcamPortGroupClientGroupSet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChTcamPortGroupClientGroupGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChTcamIndexRangeHitNumAndGroupSet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChTcamIndexRangeHitNumAndGroupGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChTcamActiveFloorsSet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChTcamActiveFloorsGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChTcamPortGroupCpuLookupTriggerSet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChTcamPortGroupCpuLookupTriggerGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChTcamCpuLookupResultsGet_E
};

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif /* __prvCpssDxChTcamLogh */
