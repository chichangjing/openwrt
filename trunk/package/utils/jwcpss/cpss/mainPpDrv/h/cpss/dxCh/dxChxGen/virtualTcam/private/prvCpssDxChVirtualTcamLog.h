/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* prvCpssDxChVirtualTcamLog.h
*       WARNING!!! this is a generated file, please don't edit it manually
* COMMENTS:
*
* FILE REVISION NUMBER:
*       $Revision: 1 $
*******************************************************************************/
#ifndef __prvCpssDxChVirtualTcamLogh
#define __prvCpssDxChVirtualTcamLogh
#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
#include <cpss/generic/log/cpssLog.h>
#include <cpss/generic/log/prvCpssLog.h>


/********* manually implemented declarations *********/

void prvCpssLogParamFunc_CPSS_DXCH_VIRTUAL_TCAM_CHECK_INFO_STC_PTR(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
);
void prvCpssLogParamFuncStc_CPSS_DXCH_VIRTUAL_TCAM_RULE_ACTION_DATA_STC_PTR
(
    IN CPSS_LOG_LIB_ENT         contextLib,
    IN CPSS_LOG_TYPE_ENT        logType,
    IN GT_CHAR_PTR              namePtr,
    IN void                   * fieldPtr,
    INOUT PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC     * inOutParamInfoPtr
);
void prvCpssLogParamFuncStc_CPSS_DXCH_VIRTUAL_TCAM_RULE_ACTION_TYPE_STC_PTR
(
    IN CPSS_LOG_LIB_ENT         contextLib,
    IN CPSS_LOG_TYPE_ENT        logType,
    IN GT_CHAR_PTR              namePtr,
    IN void                   * fieldPtr,
    INOUT PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC     * inOutParamInfoPtr
);
void prvCpssLogParamFuncStc_CPSS_DXCH_VIRTUAL_TCAM_RULE_DATA_STC_PTR
(
    IN CPSS_LOG_LIB_ENT         contextLib,
    IN CPSS_LOG_TYPE_ENT        logType,
    IN GT_CHAR_PTR              namePtr,
    IN void                   * fieldPtr,
    INOUT PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC     * inOutParamInfoPtr
);
void prvCpssLogParamFuncStc_CPSS_DXCH_VIRTUAL_TCAM_RULE_TYPE_STC_PTR
(
    IN CPSS_LOG_LIB_ENT         contextLib,
    IN CPSS_LOG_TYPE_ENT        logType,
    IN GT_CHAR_PTR              namePtr,
    IN void                   * fieldPtr,
    INOUT PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC     * inOutParamInfoPtr
);


/********* enums *********/

PRV_CPSS_LOG_STC_ENUM_DECLARE_MAC(CPSS_DXCH_VIRTUAL_TCAM_AVAILABILTY_ENT);
PRV_CPSS_LOG_STC_ENUM_DECLARE_MAC(CPSS_DXCH_VIRTUAL_TCAM_EQUAL_PRIORITY_RULE_POSITION_ENT);
PRV_CPSS_LOG_STC_ENUM_DECLARE_MAC(CPSS_DXCH_VIRTUAL_TCAM_RULE_ADDITION_METHOD_ENT);
PRV_CPSS_LOG_STC_ENUM_MAP_DECLARE_MAC(CPSS_DXCH_VIRTUAL_TCAM_RULE_SIZE_ENT);
PRV_CPSS_LOG_STC_ENUM_DECLARE_MAC(CPSS_DXCH_VIRTUAL_TCAM_RULE_TYPE_ENT);


/********* structure fields log functions *********/

void prvCpssLogParamFuncStc_CPSS_DXCH_VIRTUAL_TCAM_CHECK_INFO_STC_PTR
(
    IN CPSS_LOG_LIB_ENT         contextLib,
    IN CPSS_LOG_TYPE_ENT        logType,
    IN GT_CHAR_PTR              namePtr,
    IN void                   * fieldPtr,
    INOUT PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC     * inOutParamInfoPtr
);
void prvCpssLogParamFuncStc_CPSS_DXCH_VIRTUAL_TCAM_INFO_STC_PTR
(
    IN CPSS_LOG_LIB_ENT         contextLib,
    IN CPSS_LOG_TYPE_ENT        logType,
    IN GT_CHAR_PTR              namePtr,
    IN void                   * fieldPtr,
    INOUT PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC     * inOutParamInfoPtr
);
void prvCpssLogParamFuncStc_CPSS_DXCH_VIRTUAL_TCAM_RULE_ATTRIBUTES_STC_PTR
(
    IN CPSS_LOG_LIB_ENT         contextLib,
    IN CPSS_LOG_TYPE_ENT        logType,
    IN GT_CHAR_PTR              namePtr,
    IN void                   * fieldPtr,
    INOUT PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC     * inOutParamInfoPtr
);
void prvCpssLogParamFuncStc_CPSS_DXCH_VIRTUAL_TCAM_USAGE_STC_PTR
(
    IN CPSS_LOG_LIB_ENT         contextLib,
    IN CPSS_LOG_TYPE_ENT        logType,
    IN GT_CHAR_PTR              namePtr,
    IN void                   * fieldPtr,
    INOUT PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC     * inOutParamInfoPtr
);


/********* parameters log functions *********/

void prvCpssLogParamFunc_CPSS_DXCH_VIRTUAL_TCAM_AVAILABILTY_ENT_PTR(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
);
void prvCpssLogParamFunc_CPSS_DXCH_VIRTUAL_TCAM_EQUAL_PRIORITY_RULE_POSITION_ENT(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
);
void prvCpssLogParamFunc_CPSS_DXCH_VIRTUAL_TCAM_INFO_STC_PTR(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
);
void prvCpssLogParamFunc_CPSS_DXCH_VIRTUAL_TCAM_RULE_ACTION_DATA_STC_PTR(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
);
void prvCpssLogParamFunc_CPSS_DXCH_VIRTUAL_TCAM_RULE_ACTION_TYPE_STC_PTR(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
);
void prvCpssLogParamFunc_CPSS_DXCH_VIRTUAL_TCAM_RULE_ATTRIBUTES_STC_PTR(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
);
void prvCpssLogParamFunc_CPSS_DXCH_VIRTUAL_TCAM_RULE_DATA_STC_PTR(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
);
void prvCpssLogParamFunc_CPSS_DXCH_VIRTUAL_TCAM_RULE_ID(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
);
void prvCpssLogParamFunc_CPSS_DXCH_VIRTUAL_TCAM_RULE_TYPE_STC_PTR(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
);
void prvCpssLogParamFunc_CPSS_DXCH_VIRTUAL_TCAM_USAGE_STC_PTR(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
);


/********* API fields DB *********/

extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_DXCH_VIRTUAL_TCAM_CHECK_INFO_STC_PTR_vTcamCheckInfoArr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_DXCH_VIRTUAL_TCAM_EQUAL_PRIORITY_RULE_POSITION_ENT_position;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_DXCH_VIRTUAL_TCAM_INFO_STC_PTR_vTcamInfoPtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_DXCH_VIRTUAL_TCAM_RULE_ACTION_DATA_STC_PTR_actionDataPtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_DXCH_VIRTUAL_TCAM_RULE_ACTION_TYPE_STC_PTR_actionTypePtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_DXCH_VIRTUAL_TCAM_RULE_ATTRIBUTES_STC_PTR_ruleAttributesPtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_DXCH_VIRTUAL_TCAM_RULE_DATA_STC_PTR_ruleDataPtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_DXCH_VIRTUAL_TCAM_RULE_ID_dstRuleId;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_DXCH_VIRTUAL_TCAM_RULE_ID_ruleId;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_DXCH_VIRTUAL_TCAM_RULE_ID_srcRuleId;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_DXCH_VIRTUAL_TCAM_RULE_TYPE_STC_PTR_ruleTypePtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_BOOL_toInsert;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_U32_ruleId;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_U32_rulePlace;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_U32_sizeInRules;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_U32_vTcamAmount;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_U32_vTcamId;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_U32_vTcamMngId;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_CPSS_DXCH_VIRTUAL_TCAM_AVAILABILTY_ENT_PTR_tcamAvailabilityPtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_CPSS_DXCH_VIRTUAL_TCAM_INFO_STC_PTR_vTcamInfoPtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_CPSS_DXCH_VIRTUAL_TCAM_RULE_ACTION_DATA_STC_PTR_actionDataPtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_CPSS_DXCH_VIRTUAL_TCAM_RULE_DATA_STC_PTR_ruleDataPtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_CPSS_DXCH_VIRTUAL_TCAM_USAGE_STC_PTR_vTcamUsagePtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_U32_PTR_hwIndexPtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_U32_PTR_logicalIndexPtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_U32_PTR_nextRuleIdPtr;


/********* lib API DB *********/

enum {
    PRV_CPSS_LOG_FUNC_cpssDxChVirtualTcamManagerDelete_E = (CPSS_LOG_LIB_VIRTUAL_TCAM_E << 16),
    PRV_CPSS_LOG_FUNC_cpssDxChVirtualTcamManagerCreate_E,
    PRV_CPSS_LOG_FUNC_cpssDxChVirtualTcamManagerDevListAdd_E,
    PRV_CPSS_LOG_FUNC_cpssDxChVirtualTcamManagerDevListRemove_E,
    PRV_CPSS_LOG_FUNC_cpssDxChVirtualTcamRuleWrite_E,
    PRV_CPSS_LOG_FUNC_cpssDxChVirtualTcamRuleRead_E,
    PRV_CPSS_LOG_FUNC_cpssDxChVirtualTcamRuleActionUpdate_E,
    PRV_CPSS_LOG_FUNC_cpssDxChVirtualTcamRuleActionGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChVirtualTcamRuleValidStatusSet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChVirtualTcamRuleValidStatusGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChVirtualTcamRuleDelete_E,
    PRV_CPSS_LOG_FUNC_cpssDxChVirtualTcamRuleMove_E,
    PRV_CPSS_LOG_FUNC_cpssDxChVirtualTcamRulePriorityUpdate_E,
    PRV_CPSS_LOG_FUNC_cpssDxChVirtualTcamDbRuleIdToHwIndexConvert_E,
    PRV_CPSS_LOG_FUNC_cpssDxChVirtualTcamUsageGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChVirtualTcamInfoGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChVirtualTcamCreate_E,
    PRV_CPSS_LOG_FUNC_cpssDxChVirtualTcamRemove_E,
    PRV_CPSS_LOG_FUNC_cpssDxChVirtualTcamResize_E,
    PRV_CPSS_LOG_FUNC_cpssDxChVirtualTcamNextRuleIdGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChVirtualTcamMemoryAvailabilityCheck_E
};

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif /* __prvCpssDxChVirtualTcamLogh */
