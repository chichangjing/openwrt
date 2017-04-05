/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* prvCpssGenTmManualLog.c
*       Manually implemented CPSS Log types wrappers
* COMMENTS:
*
* FILE REVISION NUMBER:
*       $Revision: 1 $
*******************************************************************************/
#include <cpss/generic/log/cpssLog.h>
#include <cpss/generic/log/prvCpssLog.h>
#include <cpss/generic/tm/cpssTmPublicDefs.h>
#include <cpss/generic/tm/private/prvCpssGenTmLog.h>
#include <cpss/generic/log/prvCpssGenDbLog.h>
#include <cpss/generic/tm/cpssTmDrop.h>

PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_U32_ARRAY_quantumArrPtr = {
     "quantumArrPtr", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_ARRAY_MAC(GT_U32)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_BOOL_ARRAY_priosPtr = {
     "priosPtr", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_ARRAY_MAC(GT_BOOL)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_U32_ARRAY_probabilityArr = {
     "probabilityArr", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_ARRAY_MAC(GT_U32)
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssTmPortSchedulingUpdate_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_U32_index,
    &IN_CPSS_TM_ELIG_FUNC_NODE_ENT_eligPrioFuncId,
    &IN_GT_U32_ARRAY_quantumArrPtr,
    &IN_CPSS_TM_SCHD_MODE_ENT_PTR_schdModeArr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssTmTreeDwrrPrioSet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_BOOL_ARRAY_priosPtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssTmDropWredCurveCreate_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_CPSS_TM_LEVEL_ENT_level,
    &IN_GT_U32_cos,
    &IN_GT_U32_ARRAY_probabilityArr,
    &OUT_GT_U32_PTR_curveIndexPtr
};
/********* structure fields log functions *********/


void prvCpssLogParamFuncStc_CPSS_TM_TREE_CHANGE_STC_PTR
(
    IN    CPSS_LOG_LIB_ENT                        contextLib,
    IN    CPSS_LOG_TYPE_ENT                       logType,
    IN    GT_CHAR_PTR                             namePtr,
    IN    void                                  * fieldPtr,
    INOUT PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC     * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_START_FIELD_STC_MAC(CPSS_TM_TREE_CHANGE_STC *, valPtr);
    PRV_CPSS_LOG_UNUSED_MAC(inOutParamInfoPtr);
    PRV_CPSS_LOG_STC_NUMBER_MAC(valPtr, type);
    PRV_CPSS_LOG_STC_NUMBER_MAC(valPtr, index);
    PRV_CPSS_LOG_STC_NUMBER_MAC(valPtr, oldIndex);
    PRV_CPSS_LOG_STC_NUMBER_MAC(valPtr, newIndex);
    PRV_CPSS_LOG_STC_PTR_MAC(valPtr, nextPtr);
    prvCpssLogStcLogEnd(contextLib, logType);
}
void prvCpssLogParamFuncStc_CPSS_TM_ELIG_PRIO_FUNC_STC_PTR
(
    IN CPSS_LOG_LIB_ENT         contextLib,
    IN CPSS_LOG_TYPE_ENT        logType,
    IN GT_CHAR_PTR              namePtr,
    IN void                   * fieldPtr,
    INOUT PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC     * inOutParamInfoPtr
)
{
    GT_BOOL key; /* the union key */

    PRV_CPSS_LOG_START_FIELD_STC_MAC(CPSS_TM_ELIG_PRIO_FUNC_STC *, valPtr);
    key = (GT_BOOL)inOutParamInfoPtr->paramKey.paramKeyArr[4];
    if (key == GT_TRUE) 
    {
        PRV_CPSS_LOG_STC_TYPE_ARRAY_MAC(valPtr, queueEligPrioFunc, 4, CPSS_TM_ELIG_PRIO_FUNC_OUT_STC); 
    }
    else
    {
        PRV_CPSS_LOG_STC_TYPE_ARRAY2_MAC(valPtr, nodeEligPrioFunc, 8, 4,  CPSS_TM_ELIG_PRIO_FUNC_OUT_STC);
    }
    prvCpssLogStcLogEnd(contextLib, logType);
}
void prvCpssLogParamFunc_CPSS_TM_DROP_PROFILE_PARAMS_STC_PTR(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_START_PARAM_STC_MAC(CPSS_TM_DROP_PROFILE_PARAMS_STC*, paramVal);
    prvCpssLogArrayOfParamsHandle(contextLib,logType,namePtr,(void*) paramVal, sizeof(CPSS_TM_DROP_PROFILE_PARAMS_STC), inOutParamInfoPtr,
                              prvCpssLogParamFuncStc_CPSS_TM_DROP_PROFILE_PARAMS_STC_PTR);
}
void prvCpssLogParamFunc_CPSS_TM_SCHD_MODE_ENT_PTR(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_SET_PARAM_VAL_MAC(CPSS_TM_SCHD_MODE_ENT*, paramVal);
    if (paramVal == NULL)
    {
        PRV_CPSS_LOG_AND_HISTORY_PARAM_MAC(contextLib, logType, "%s = NULL\n", namePtr);
        return;
    }
    PRV_CPSS_LOG_ENUM_ARRAY_MAC(contextLib, logType, namePtr, paramVal, CPSS_TM_SCHD_MODE_ENT,inOutParamInfoPtr);
}
void prvCpssLogParamFunc_CPSS_TM_DP_SOURCE_ENT_PTR(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_SET_PARAM_VAL_MAC(CPSS_TM_DP_SOURCE_ENT*, paramVal);
    if (paramVal == NULL)
    {
        PRV_CPSS_LOG_AND_HISTORY_PARAM_MAC(contextLib, logType, "%s = NULL\n", namePtr);
        return;
    }
    PRV_CPSS_LOG_ENUM_ARRAY_MAC(contextLib, logType, namePtr, paramVal, CPSS_TM_DP_SOURCE_ENT,inOutParamInfoPtr);
}

void cpssTmPortSchedulingUpdate_preLogic
(
    IN va_list  args,
    OUT PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC * paramDataPtr
)
{
    /* devNum */
    va_arg(args, GT_U32);
    /* setting the size of quantumArrPtr - constant */
    paramDataPtr->paramKey.paramKeyArr[2] = 8; 
}
void cpssTmEligPrioFuncQueueConfig_preLogic
(
    IN va_list  args,
    OUT PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC * paramDataPtr
)
{
    /* devNum */
    va_arg(args, GT_U32);
    /* setting the CPSS_TM_ELIG_PRIO_FUNC_STC union key - in order to use queueEligPrioFunc */
    paramDataPtr->paramKey.paramKeyArr[4] = GT_TRUE; 
}
void cpssTmEligPrioFuncNodeConfig_preLogic
(
    IN va_list  args,
    OUT PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC * paramDataPtr
)
{
    /* devNum */
    va_arg(args, GT_U32);
    /* setting the CPSS_TM_ELIG_PRIO_FUNC_STC union key - in order to use nodeEligPrioFunc */
    paramDataPtr->paramKey.paramKeyArr[4] = GT_FALSE; 
}
void cpssTmDropWredCurveCreate_preLogic
(
    IN va_list  args,
    OUT PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC * paramDataPtr
)
{
    /* devNum */
    va_arg(args, GT_U32);
    /* setting the size of probabilityArr - constant */
    paramDataPtr->paramKey.paramKeyArr[2] = CPSS_TM_DROP_PROB_ARR_SIZE_CNS; 
}
void cpssTmDropProfileAgingBlockCreate_preLogic
(
    IN va_list  args,
    OUT PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC * paramDataPtr
)
{
    /* devNum */
    va_arg(args, GT_U32);
    /* setting the size of profileStcArr - constant */
    paramDataPtr->paramKey.paramKeyArr[2] = CPSS_TM_AGING_PROFILES_CNS; 
}
void cpssTm2TmEgressDropAqmModeSet_preLogic
(
    IN va_list  args,
    OUT PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC * paramDataPtr
)
{
    CPSS_TM_COLOR_NUM_ENT colorNum; /*enum of colorNum */
    GT_U32 actualColorNum; /* number of colors */

    /* devNum */
    va_arg(args, GT_U32);
    /* level */
    va_arg(args, CPSS_TM_LEVEL_ENT);
    /* colorNum */
    colorNum = va_arg(args, CPSS_TM_COLOR_NUM_ENT);
    switch (colorNum) 
    {
        case CPSS_TM_COLOR_NUM_1_E:
            actualColorNum = 1;
            break;
        case CPSS_TM_COLOR_NUM_2_E:
            actualColorNum = 2;
            break;
        case CPSS_TM_COLOR_NUM_3_E:
            actualColorNum = 3;
            break;
        default:
            actualColorNum = 0;
            break;
    }
    /* setting the size of dpSourcePtr */
    paramDataPtr->paramKey.paramKeyArr[2] = actualColorNum;
}

