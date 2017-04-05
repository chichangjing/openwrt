/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* prvCpssDxChVirtualTcamLog.c
*       WARNING!!! this is a generated file, please don't edit it manually
* COMMENTS:
*
* FILE REVISION NUMBER:
*       $Revision: 1 $
*******************************************************************************/
#include <cpss/generic/log/cpssLog.h>
#include <cpss/generic/log/prvCpssLog.h>
#include <cpss/extServices/private/prvCpssBindFunc.h>
#include <cpss/dxCh/dxChxGen/virtualTcam/cpssDxChVirtualTcam.h>
#include <cpss/dxCh/dxChxGen/virtualTcam/private/prvCpssDxChVirtualTcamLog.h>
#include <cpss/generic/log/prvCpssGenCommonTypesLog.h>
#include <cpss/generic/log/prvCpssGenDbLog.h>


/********* enums *********/

char * prvCpssLogEnum_CPSS_DXCH_VIRTUAL_TCAM_AVAILABILTY_ENT[]  =
{
    "CPSS_DXCH_VIRTUAL_TCAM_AVAILABILTY_NEVER_E",
    "CPSS_DXCH_VIRTUAL_TCAM_AVAILABILTY_ALREADY_E",
    "CPSS_DXCH_VIRTUAL_TCAM_AVAILABILTY_AFTER_DEFRAG_E",
    "CPSS_DXCH_VIRTUAL_TCAM_AVAILABILTY_FROM_SCRATCH_E"
};
PRV_CPSS_LOG_STC_ENUM_ARRAY_SIZE_MAC(CPSS_DXCH_VIRTUAL_TCAM_AVAILABILTY_ENT);
char * prvCpssLogEnum_CPSS_DXCH_VIRTUAL_TCAM_EQUAL_PRIORITY_RULE_POSITION_ENT[]  =
{
    "CPSS_DXCH_VIRTUAL_TCAM_EQUAL_PRIORITY_RULE_POSITION_FIRST_E",
    "CPSS_DXCH_VIRTUAL_TCAM_EQUAL_PRIORITY_RULE_POSITION_LAST_E"
};
PRV_CPSS_LOG_STC_ENUM_ARRAY_SIZE_MAC(CPSS_DXCH_VIRTUAL_TCAM_EQUAL_PRIORITY_RULE_POSITION_ENT);
char * prvCpssLogEnum_CPSS_DXCH_VIRTUAL_TCAM_RULE_ADDITION_METHOD_ENT[]  =
{
    "CPSS_DXCH_VIRTUAL_TCAM_RULE_ADDITION_METHOD_LOGICAL_INDEX_E",
    "CPSS_DXCH_VIRTUAL_TCAM_RULE_ADDITION_METHOD_PRIORITY_E"
};
PRV_CPSS_LOG_STC_ENUM_ARRAY_SIZE_MAC(CPSS_DXCH_VIRTUAL_TCAM_RULE_ADDITION_METHOD_ENT);
PRV_CPSS_ENUM_STRING_VALUE_PAIR_STC prvCpssLogEnum_map_CPSS_DXCH_VIRTUAL_TCAM_RULE_SIZE_ENT[]  =
{PRV_CPSS_LOG_ENUM_NAME_AND_VALUE_MAC(CPSS_DXCH_VIRTUAL_TCAM_RULE_SIZE_10_B_E),
PRV_CPSS_LOG_ENUM_NAME_AND_VALUE_MAC(CPSS_DXCH_VIRTUAL_TCAM_RULE_SIZE_20_B_E),
PRV_CPSS_LOG_ENUM_NAME_AND_VALUE_MAC(CPSS_DXCH_VIRTUAL_TCAM_RULE_SIZE_30_B_E),
PRV_CPSS_LOG_ENUM_NAME_AND_VALUE_MAC(CPSS_DXCH_VIRTUAL_TCAM_RULE_SIZE_40_B_E),
PRV_CPSS_LOG_ENUM_NAME_AND_VALUE_MAC(CPSS_DXCH_VIRTUAL_TCAM_RULE_SIZE_50_B_E),
PRV_CPSS_LOG_ENUM_NAME_AND_VALUE_MAC(CPSS_DXCH_VIRTUAL_TCAM_RULE_SIZE_60_B_E),
PRV_CPSS_LOG_ENUM_NAME_AND_VALUE_MAC(CPSS_DXCH_VIRTUAL_TCAM_RULE_SIZE_STD_E),
PRV_CPSS_LOG_ENUM_NAME_AND_VALUE_MAC(CPSS_DXCH_VIRTUAL_TCAM_RULE_SIZE_EXT_E)
};
PRV_CPSS_LOG_STC_ENUM_MAP_ARRAY_SIZE_MAC(CPSS_DXCH_VIRTUAL_TCAM_RULE_SIZE_ENT);
char * prvCpssLogEnum_CPSS_DXCH_VIRTUAL_TCAM_RULE_TYPE_ENT[]  =
{
    "CPSS_DXCH_VIRTUAL_TCAM_RULE_TYPE_PCL_E",
    "CPSS_DXCH_VIRTUAL_TCAM_RULE_TYPE_TTI_E"
};
PRV_CPSS_LOG_STC_ENUM_ARRAY_SIZE_MAC(CPSS_DXCH_VIRTUAL_TCAM_RULE_TYPE_ENT);


/********* structure fields log functions *********/

void prvCpssLogParamFuncStc_CPSS_DXCH_VIRTUAL_TCAM_CHECK_INFO_STC_PTR
(
    IN CPSS_LOG_LIB_ENT         contextLib,
    IN CPSS_LOG_TYPE_ENT        logType,
    IN GT_CHAR_PTR              namePtr,
    IN void                   * fieldPtr,
    INOUT PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC     * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_START_FIELD_STC_MAC(CPSS_DXCH_VIRTUAL_TCAM_CHECK_INFO_STC *, valPtr);
    PRV_CPSS_LOG_UNUSED_MAC(inOutParamInfoPtr);
    PRV_CPSS_LOG_STC_BOOL_MAC(valPtr, isExist);
    PRV_CPSS_LOG_STC_NUMBER_MAC(valPtr, vTcamId);
    PRV_CPSS_LOG_STC_STC_MAC(valPtr, vTcamInfo, CPSS_DXCH_VIRTUAL_TCAM_INFO_STC);
    prvCpssLogStcLogEnd(contextLib, logType);
}
void prvCpssLogParamFuncStc_CPSS_DXCH_VIRTUAL_TCAM_INFO_STC_PTR
(
    IN CPSS_LOG_LIB_ENT         contextLib,
    IN CPSS_LOG_TYPE_ENT        logType,
    IN GT_CHAR_PTR              namePtr,
    IN void                   * fieldPtr,
    INOUT PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC     * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_START_FIELD_STC_MAC(CPSS_DXCH_VIRTUAL_TCAM_INFO_STC *, valPtr);
    PRV_CPSS_LOG_UNUSED_MAC(inOutParamInfoPtr);
    PRV_CPSS_LOG_STC_NUMBER_MAC(valPtr, clientGroup);
    PRV_CPSS_LOG_STC_NUMBER_MAC(valPtr, hitNumber);
    PRV_CPSS_LOG_STC_ENUM_MAP_MAC(valPtr, ruleSize, CPSS_DXCH_VIRTUAL_TCAM_RULE_SIZE_ENT);
    PRV_CPSS_LOG_STC_BOOL_MAC(valPtr, autoResize);
    PRV_CPSS_LOG_STC_NUMBER_MAC(valPtr, guaranteedNumOfRules);
    PRV_CPSS_LOG_STC_ENUM_MAC(valPtr, ruleAdditionMethod, CPSS_DXCH_VIRTUAL_TCAM_RULE_ADDITION_METHOD_ENT);
    prvCpssLogStcLogEnd(contextLib, logType);
}
void prvCpssLogParamFuncStc_CPSS_DXCH_VIRTUAL_TCAM_RULE_ATTRIBUTES_STC_PTR
(
    IN CPSS_LOG_LIB_ENT         contextLib,
    IN CPSS_LOG_TYPE_ENT        logType,
    IN GT_CHAR_PTR              namePtr,
    IN void                   * fieldPtr,
    INOUT PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC     * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_START_FIELD_STC_MAC(CPSS_DXCH_VIRTUAL_TCAM_RULE_ATTRIBUTES_STC *, valPtr);
    PRV_CPSS_LOG_UNUSED_MAC(inOutParamInfoPtr);
    PRV_CPSS_LOG_STC_NUMBER_MAC(valPtr, priority);
    prvCpssLogStcLogEnd(contextLib, logType);
}
void prvCpssLogParamFuncStc_CPSS_DXCH_VIRTUAL_TCAM_USAGE_STC_PTR
(
    IN CPSS_LOG_LIB_ENT         contextLib,
    IN CPSS_LOG_TYPE_ENT        logType,
    IN GT_CHAR_PTR              namePtr,
    IN void                   * fieldPtr,
    INOUT PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC     * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_START_FIELD_STC_MAC(CPSS_DXCH_VIRTUAL_TCAM_USAGE_STC *, valPtr);
    PRV_CPSS_LOG_UNUSED_MAC(inOutParamInfoPtr);
    PRV_CPSS_LOG_STC_NUMBER_MAC(valPtr, rulesUsed);
    PRV_CPSS_LOG_STC_NUMBER_MAC(valPtr, rulesFree);
    prvCpssLogStcLogEnd(contextLib, logType);
}


/********* parameters log functions *********/

void prvCpssLogParamFunc_CPSS_DXCH_VIRTUAL_TCAM_AVAILABILTY_ENT_PTR(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_SET_PARAM_VAL_MAC(CPSS_DXCH_VIRTUAL_TCAM_AVAILABILTY_ENT*, paramVal);
    if (paramVal == NULL)
    {
        PRV_CPSS_LOG_AND_HISTORY_PARAM_MAC(contextLib, logType, "%s = NULL\n", namePtr);
        return;
    }
    PRV_CPSS_LOG_ENUM_MAC(namePtr, *paramVal, CPSS_DXCH_VIRTUAL_TCAM_AVAILABILTY_ENT);
}
void prvCpssLogParamFunc_CPSS_DXCH_VIRTUAL_TCAM_EQUAL_PRIORITY_RULE_POSITION_ENT(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_SET_PARAM_VAL_MAC(CPSS_DXCH_VIRTUAL_TCAM_EQUAL_PRIORITY_RULE_POSITION_ENT, paramVal);
    PRV_CPSS_LOG_ENUM_MAC(namePtr, paramVal, CPSS_DXCH_VIRTUAL_TCAM_EQUAL_PRIORITY_RULE_POSITION_ENT);
}
void prvCpssLogParamFunc_CPSS_DXCH_VIRTUAL_TCAM_INFO_STC_PTR(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_START_PARAM_STC_MAC(CPSS_DXCH_VIRTUAL_TCAM_INFO_STC*, paramVal);
    prvCpssLogParamFuncStc_CPSS_DXCH_VIRTUAL_TCAM_INFO_STC_PTR(contextLib, logType, namePtr, paramVal, inOutParamInfoPtr);
}
void prvCpssLogParamFunc_CPSS_DXCH_VIRTUAL_TCAM_RULE_ACTION_DATA_STC_PTR(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_START_PARAM_STC_MAC(CPSS_DXCH_VIRTUAL_TCAM_RULE_ACTION_DATA_STC*, paramVal);
    prvCpssLogParamFuncStc_CPSS_DXCH_VIRTUAL_TCAM_RULE_ACTION_DATA_STC_PTR(contextLib, logType, namePtr, paramVal, inOutParamInfoPtr);
}
void prvCpssLogParamFunc_CPSS_DXCH_VIRTUAL_TCAM_RULE_ACTION_TYPE_STC_PTR(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_START_PARAM_STC_MAC(CPSS_DXCH_VIRTUAL_TCAM_RULE_ACTION_TYPE_STC*, paramVal);
    prvCpssLogParamFuncStc_CPSS_DXCH_VIRTUAL_TCAM_RULE_ACTION_TYPE_STC_PTR(contextLib, logType, namePtr, paramVal, inOutParamInfoPtr);
}
void prvCpssLogParamFunc_CPSS_DXCH_VIRTUAL_TCAM_RULE_ATTRIBUTES_STC_PTR(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_START_PARAM_STC_MAC(CPSS_DXCH_VIRTUAL_TCAM_RULE_ATTRIBUTES_STC*, paramVal);
    prvCpssLogParamFuncStc_CPSS_DXCH_VIRTUAL_TCAM_RULE_ATTRIBUTES_STC_PTR(contextLib, logType, namePtr, paramVal, inOutParamInfoPtr);
}
void prvCpssLogParamFunc_CPSS_DXCH_VIRTUAL_TCAM_RULE_DATA_STC_PTR(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_START_PARAM_STC_MAC(CPSS_DXCH_VIRTUAL_TCAM_RULE_DATA_STC*, paramVal);
    prvCpssLogParamFuncStc_CPSS_DXCH_VIRTUAL_TCAM_RULE_DATA_STC_PTR(contextLib, logType, namePtr, paramVal, inOutParamInfoPtr);
}
void prvCpssLogParamFunc_CPSS_DXCH_VIRTUAL_TCAM_RULE_ID(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
)
{
    char * formatPtr = "%s = %d\n";
    PRV_CPSS_LOG_SET_PARAM_VAL_MAC(CPSS_DXCH_VIRTUAL_TCAM_RULE_ID, paramVal);
    if (inOutParamInfoPtr->formatPtr)
    {
        formatPtr = inOutParamInfoPtr->formatPtr;
    }
    PRV_CPSS_LOG_AND_HISTORY_PARAM_MAC(contextLib, logType, formatPtr, namePtr, paramVal);
}
void prvCpssLogParamFunc_CPSS_DXCH_VIRTUAL_TCAM_RULE_TYPE_STC_PTR(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_START_PARAM_STC_MAC(CPSS_DXCH_VIRTUAL_TCAM_RULE_TYPE_STC*, paramVal);
    prvCpssLogParamFuncStc_CPSS_DXCH_VIRTUAL_TCAM_RULE_TYPE_STC_PTR(contextLib, logType, namePtr, paramVal, inOutParamInfoPtr);
}
void prvCpssLogParamFunc_CPSS_DXCH_VIRTUAL_TCAM_USAGE_STC_PTR(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_START_PARAM_STC_MAC(CPSS_DXCH_VIRTUAL_TCAM_USAGE_STC*, paramVal);
    prvCpssLogParamFuncStc_CPSS_DXCH_VIRTUAL_TCAM_USAGE_STC_PTR(contextLib, logType, namePtr, paramVal, inOutParamInfoPtr);
}


/********* API fields DB *********/

PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_DXCH_VIRTUAL_TCAM_CHECK_INFO_STC_PTR_vTcamCheckInfoArr = {
     "vTcamCheckInfoArr", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(CPSS_DXCH_VIRTUAL_TCAM_CHECK_INFO_STC)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_DXCH_VIRTUAL_TCAM_EQUAL_PRIORITY_RULE_POSITION_ENT_position = {
     "position", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_MAC(CPSS_DXCH_VIRTUAL_TCAM_EQUAL_PRIORITY_RULE_POSITION_ENT)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_DXCH_VIRTUAL_TCAM_INFO_STC_PTR_vTcamInfoPtr = {
     "vTcamInfoPtr", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(CPSS_DXCH_VIRTUAL_TCAM_INFO_STC)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_DXCH_VIRTUAL_TCAM_RULE_ACTION_DATA_STC_PTR_actionDataPtr = {
     "actionDataPtr", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(CPSS_DXCH_VIRTUAL_TCAM_RULE_ACTION_DATA_STC)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_DXCH_VIRTUAL_TCAM_RULE_ACTION_TYPE_STC_PTR_actionTypePtr = {
     "actionTypePtr", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(CPSS_DXCH_VIRTUAL_TCAM_RULE_ACTION_TYPE_STC)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_DXCH_VIRTUAL_TCAM_RULE_ATTRIBUTES_STC_PTR_ruleAttributesPtr = {
     "ruleAttributesPtr", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(CPSS_DXCH_VIRTUAL_TCAM_RULE_ATTRIBUTES_STC)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_DXCH_VIRTUAL_TCAM_RULE_DATA_STC_PTR_ruleDataPtr = {
     "ruleDataPtr", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(CPSS_DXCH_VIRTUAL_TCAM_RULE_DATA_STC)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_DXCH_VIRTUAL_TCAM_RULE_ID_dstRuleId = {
     "dstRuleId", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_MAC(CPSS_DXCH_VIRTUAL_TCAM_RULE_ID)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_DXCH_VIRTUAL_TCAM_RULE_ID_ruleId = {
     "ruleId", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_MAC(CPSS_DXCH_VIRTUAL_TCAM_RULE_ID)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_DXCH_VIRTUAL_TCAM_RULE_ID_srcRuleId = {
     "srcRuleId", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_MAC(CPSS_DXCH_VIRTUAL_TCAM_RULE_ID)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_DXCH_VIRTUAL_TCAM_RULE_TYPE_STC_PTR_ruleTypePtr = {
     "ruleTypePtr", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(CPSS_DXCH_VIRTUAL_TCAM_RULE_TYPE_STC)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_BOOL_toInsert = {
     "toInsert", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_MAC(GT_BOOL)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_U32_ruleId = {
     "ruleId", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_MAC(GT_U32)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_U32_rulePlace = {
     "rulePlace", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_MAC(GT_U32)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_U32_sizeInRules = {
     "sizeInRules", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_MAC(GT_U32)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_U32_vTcamAmount = {
     "vTcamAmount", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_MAC(GT_U32)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_U32_vTcamId = {
     "vTcamId", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_MAC(GT_U32)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_U32_vTcamMngId = {
     "vTcamMngId", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_MAC(GT_U32)
};
PRV_CPSS_LOG_FUNC_PARAM_STC OUT_CPSS_DXCH_VIRTUAL_TCAM_AVAILABILTY_ENT_PTR_tcamAvailabilityPtr = {
     "tcamAvailabilityPtr", PRV_CPSS_LOG_PARAM_OUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(CPSS_DXCH_VIRTUAL_TCAM_AVAILABILTY_ENT)
};
PRV_CPSS_LOG_FUNC_PARAM_STC OUT_CPSS_DXCH_VIRTUAL_TCAM_INFO_STC_PTR_vTcamInfoPtr = {
     "vTcamInfoPtr", PRV_CPSS_LOG_PARAM_OUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(CPSS_DXCH_VIRTUAL_TCAM_INFO_STC)
};
PRV_CPSS_LOG_FUNC_PARAM_STC OUT_CPSS_DXCH_VIRTUAL_TCAM_RULE_ACTION_DATA_STC_PTR_actionDataPtr = {
     "actionDataPtr", PRV_CPSS_LOG_PARAM_OUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(CPSS_DXCH_VIRTUAL_TCAM_RULE_ACTION_DATA_STC)
};
PRV_CPSS_LOG_FUNC_PARAM_STC OUT_CPSS_DXCH_VIRTUAL_TCAM_RULE_DATA_STC_PTR_ruleDataPtr = {
     "ruleDataPtr", PRV_CPSS_LOG_PARAM_OUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(CPSS_DXCH_VIRTUAL_TCAM_RULE_DATA_STC)
};
PRV_CPSS_LOG_FUNC_PARAM_STC OUT_CPSS_DXCH_VIRTUAL_TCAM_USAGE_STC_PTR_vTcamUsagePtr = {
     "vTcamUsagePtr", PRV_CPSS_LOG_PARAM_OUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(CPSS_DXCH_VIRTUAL_TCAM_USAGE_STC)
};
PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_U32_PTR_hwIndexPtr = {
     "hwIndexPtr", PRV_CPSS_LOG_PARAM_OUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(GT_U32)
};
PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_U32_PTR_logicalIndexPtr = {
     "logicalIndexPtr", PRV_CPSS_LOG_PARAM_OUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(GT_U32)
};
PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_U32_PTR_nextRuleIdPtr = {
     "nextRuleIdPtr", PRV_CPSS_LOG_PARAM_OUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(GT_U32)
};


/********* API prototypes DB *********/

PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChVirtualTcamManagerDelete_PARAMS[] =  {
    &IN_GT_U32_vTcamMngId
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChVirtualTcamMemoryAvailabilityCheck_PARAMS[] =  {
    &IN_GT_U32_vTcamMngId,
    &IN_GT_U32_vTcamAmount,
    &IN_CPSS_DXCH_VIRTUAL_TCAM_CHECK_INFO_STC_PTR_vTcamCheckInfoArr,
    &OUT_CPSS_DXCH_VIRTUAL_TCAM_AVAILABILTY_ENT_PTR_tcamAvailabilityPtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChVirtualTcamRemove_PARAMS[] =  {
    &IN_GT_U32_vTcamMngId,
    &IN_GT_U32_vTcamId
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChVirtualTcamCreate_PARAMS[] =  {
    &IN_GT_U32_vTcamMngId,
    &IN_GT_U32_vTcamId,
    &IN_CPSS_DXCH_VIRTUAL_TCAM_INFO_STC_PTR_vTcamInfoPtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChVirtualTcamRuleDelete_PARAMS[] =  {
    &IN_GT_U32_vTcamMngId,
    &IN_GT_U32_vTcamId,
    &IN_CPSS_DXCH_VIRTUAL_TCAM_RULE_ID_ruleId
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChVirtualTcamRuleActionUpdate_PARAMS[] =  {
    &IN_GT_U32_vTcamMngId,
    &IN_GT_U32_vTcamId,
    &IN_CPSS_DXCH_VIRTUAL_TCAM_RULE_ID_ruleId,
    &IN_CPSS_DXCH_VIRTUAL_TCAM_RULE_ACTION_TYPE_STC_PTR_actionTypePtr,
    &IN_CPSS_DXCH_VIRTUAL_TCAM_RULE_ACTION_DATA_STC_PTR_actionDataPtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChVirtualTcamRuleActionGet_PARAMS[] =  {
    &IN_GT_U32_vTcamMngId,
    &IN_GT_U32_vTcamId,
    &IN_CPSS_DXCH_VIRTUAL_TCAM_RULE_ID_ruleId,
    &IN_CPSS_DXCH_VIRTUAL_TCAM_RULE_ACTION_TYPE_STC_PTR_actionTypePtr,
    &OUT_CPSS_DXCH_VIRTUAL_TCAM_RULE_ACTION_DATA_STC_PTR_actionDataPtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChVirtualTcamRuleWrite_PARAMS[] =  {
    &IN_GT_U32_vTcamMngId,
    &IN_GT_U32_vTcamId,
    &IN_CPSS_DXCH_VIRTUAL_TCAM_RULE_ID_ruleId,
    &IN_CPSS_DXCH_VIRTUAL_TCAM_RULE_ATTRIBUTES_STC_PTR_ruleAttributesPtr,
    &IN_CPSS_DXCH_VIRTUAL_TCAM_RULE_TYPE_STC_PTR_ruleTypePtr,
    &IN_CPSS_DXCH_VIRTUAL_TCAM_RULE_DATA_STC_PTR_ruleDataPtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChVirtualTcamRuleRead_PARAMS[] =  {
    &IN_GT_U32_vTcamMngId,
    &IN_GT_U32_vTcamId,
    &IN_CPSS_DXCH_VIRTUAL_TCAM_RULE_ID_ruleId,
    &IN_CPSS_DXCH_VIRTUAL_TCAM_RULE_TYPE_STC_PTR_ruleTypePtr,
    &OUT_CPSS_DXCH_VIRTUAL_TCAM_RULE_DATA_STC_PTR_ruleDataPtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChVirtualTcamRuleValidStatusSet_PARAMS[] =  {
    &IN_GT_U32_vTcamMngId,
    &IN_GT_U32_vTcamId,
    &IN_CPSS_DXCH_VIRTUAL_TCAM_RULE_ID_ruleId,
    &IN_GT_BOOL_valid
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChVirtualTcamRulePriorityUpdate_PARAMS[] =  {
    &IN_GT_U32_vTcamMngId,
    &IN_GT_U32_vTcamId,
    &IN_CPSS_DXCH_VIRTUAL_TCAM_RULE_ID_ruleId,
    &IN_GT_U32_priority,
    &IN_CPSS_DXCH_VIRTUAL_TCAM_EQUAL_PRIORITY_RULE_POSITION_ENT_position
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChVirtualTcamRuleValidStatusGet_PARAMS[] =  {
    &IN_GT_U32_vTcamMngId,
    &IN_GT_U32_vTcamId,
    &IN_CPSS_DXCH_VIRTUAL_TCAM_RULE_ID_ruleId,
    &OUT_GT_BOOL_PTR_validPtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChVirtualTcamRuleMove_PARAMS[] =  {
    &IN_GT_U32_vTcamMngId,
    &IN_GT_U32_vTcamId,
    &IN_CPSS_DXCH_VIRTUAL_TCAM_RULE_ID_srcRuleId,
    &IN_CPSS_DXCH_VIRTUAL_TCAM_RULE_ID_dstRuleId
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChVirtualTcamDbRuleIdToHwIndexConvert_PARAMS[] =  {
    &IN_GT_U32_vTcamMngId,
    &IN_GT_U32_vTcamId,
    &IN_GT_U32_ruleId,
    &OUT_GT_U32_PTR_logicalIndexPtr,
    &OUT_GT_U32_PTR_hwIndexPtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChVirtualTcamNextRuleIdGet_PARAMS[] =  {
    &IN_GT_U32_vTcamMngId,
    &IN_GT_U32_vTcamId,
    &IN_GT_U32_ruleId,
    &OUT_GT_U32_PTR_nextRuleIdPtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChVirtualTcamResize_PARAMS[] =  {
    &IN_GT_U32_vTcamMngId,
    &IN_GT_U32_vTcamId,
    &IN_GT_U32_rulePlace,
    &IN_GT_BOOL_toInsert,
    &IN_GT_U32_sizeInRules
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChVirtualTcamInfoGet_PARAMS[] =  {
    &IN_GT_U32_vTcamMngId,
    &IN_GT_U32_vTcamId,
    &OUT_CPSS_DXCH_VIRTUAL_TCAM_INFO_STC_PTR_vTcamInfoPtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChVirtualTcamUsageGet_PARAMS[] =  {
    &IN_GT_U32_vTcamMngId,
    &IN_GT_U32_vTcamId,
    &OUT_CPSS_DXCH_VIRTUAL_TCAM_USAGE_STC_PTR_vTcamUsagePtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChVirtualTcamManagerDevListRemove_PARAMS[] =  {
    &IN_GT_U32_vTcamMngId,
    &IN_GT_U8_PTR_devListArr,
    &IN_GT_U32_numOfDevs
};
extern PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChVirtualTcamManagerDevListAdd_PARAMS[];


/********* lib API DB *********/

extern void cpssDxChVirtualTcamManagerDevListAdd_preLogic(IN va_list  args, OUT PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC * paramDataPtr);
extern void cpssDxChVirtualTcamManagerDevListAdd_preLogic(IN va_list  args, OUT PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC * paramDataPtr);
extern void cpssDxChVirtualTcamRuleWrite_preLogic(IN va_list  args, OUT PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC * paramDataPtr);
extern void cpssDxChVirtualTcamRuleRead_preLogic(IN va_list  args, OUT PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC * paramDataPtr);
extern void cpssDxChVirtualTcamRuleActionUpdate_preLogic(IN va_list  args, OUT PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC * paramDataPtr);
extern void cpssDxChVirtualTcamRuleActionUpdate_preLogic(IN va_list  args, OUT PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC * paramDataPtr);
extern void cpssDxChVirtualTcamMemoryAvailabilityCheck_preLogic(IN va_list  args, OUT PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC * paramDataPtr);

static PRV_CPSS_LOG_FUNC_ENTRY_STC prvCpssDxChVirtualTcamLogLibDb[] = {
    {"cpssDxChVirtualTcamManagerDelete", 1, cpssDxChVirtualTcamManagerDelete_PARAMS, NULL},
    {"cpssDxChVirtualTcamManagerCreate", 1, cpssDxChVirtualTcamManagerDelete_PARAMS, NULL},
    {"cpssDxChVirtualTcamManagerDevListAdd", 3, cpssDxChVirtualTcamManagerDevListAdd_PARAMS, cpssDxChVirtualTcamManagerDevListAdd_preLogic},
    {"cpssDxChVirtualTcamManagerDevListRemove", 3, cpssDxChVirtualTcamManagerDevListRemove_PARAMS, cpssDxChVirtualTcamManagerDevListAdd_preLogic},
    {"cpssDxChVirtualTcamRuleWrite", 6, cpssDxChVirtualTcamRuleWrite_PARAMS, cpssDxChVirtualTcamRuleWrite_preLogic},
    {"cpssDxChVirtualTcamRuleRead", 5, cpssDxChVirtualTcamRuleRead_PARAMS, cpssDxChVirtualTcamRuleRead_preLogic},
    {"cpssDxChVirtualTcamRuleActionUpdate", 5, cpssDxChVirtualTcamRuleActionUpdate_PARAMS, cpssDxChVirtualTcamRuleActionUpdate_preLogic},
    {"cpssDxChVirtualTcamRuleActionGet", 5, cpssDxChVirtualTcamRuleActionGet_PARAMS, cpssDxChVirtualTcamRuleActionUpdate_preLogic},
    {"cpssDxChVirtualTcamRuleValidStatusSet", 4, cpssDxChVirtualTcamRuleValidStatusSet_PARAMS, NULL},
    {"cpssDxChVirtualTcamRuleValidStatusGet", 4, cpssDxChVirtualTcamRuleValidStatusGet_PARAMS, NULL},
    {"cpssDxChVirtualTcamRuleDelete", 3, cpssDxChVirtualTcamRuleDelete_PARAMS, NULL},
    {"cpssDxChVirtualTcamRuleMove", 4, cpssDxChVirtualTcamRuleMove_PARAMS, NULL},
    {"cpssDxChVirtualTcamRulePriorityUpdate", 5, cpssDxChVirtualTcamRulePriorityUpdate_PARAMS, NULL},
    {"cpssDxChVirtualTcamDbRuleIdToHwIndexConvert", 5, cpssDxChVirtualTcamDbRuleIdToHwIndexConvert_PARAMS, NULL},
    {"cpssDxChVirtualTcamUsageGet", 3, cpssDxChVirtualTcamUsageGet_PARAMS, NULL},
    {"cpssDxChVirtualTcamInfoGet", 3, cpssDxChVirtualTcamInfoGet_PARAMS, NULL},
    {"cpssDxChVirtualTcamCreate", 3, cpssDxChVirtualTcamCreate_PARAMS, NULL},
    {"cpssDxChVirtualTcamRemove", 2, cpssDxChVirtualTcamRemove_PARAMS, NULL},
    {"cpssDxChVirtualTcamResize", 5, cpssDxChVirtualTcamResize_PARAMS, NULL},
    {"cpssDxChVirtualTcamNextRuleIdGet", 4, cpssDxChVirtualTcamNextRuleIdGet_PARAMS, NULL},
    {"cpssDxChVirtualTcamMemoryAvailabilityCheck", 4, cpssDxChVirtualTcamMemoryAvailabilityCheck_PARAMS, cpssDxChVirtualTcamMemoryAvailabilityCheck_preLogic},
};

/******** DB Access Function ********/
void prvCpssLogParamLibDbGet_CPSS_LOG_LIB_VIRTUAL_TCAM(
    OUT PRV_CPSS_LOG_FUNC_ENTRY_STC ** logFuncDbPtrPtr,
    OUT GT_U32 * logFuncDbSizePtr
)
{
    *logFuncDbPtrPtr = prvCpssDxChVirtualTcamLogLibDb;
    *logFuncDbSizePtr = sizeof(prvCpssDxChVirtualTcamLogLibDb) / sizeof(PRV_CPSS_LOG_FUNC_ENTRY_STC);
}

