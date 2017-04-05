/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* prvCpssDxChVirtualTcamManualLog.c
*       Manually implemented CPSS Log type wrappers
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
#include <cpss/generic/log/prvCpssGenLog.h>
#include <cpss/generic/pcl/private/prvCpssGenPclLog.h>
#include <cpss/dxCh/dxChxGen/pcl/private/prvCpssDxChPclLog.h>
#include <cpss/dxCh/dxChxGen/tti/private/prvCpssDxChTtiLog.h>

/********* enums *********/

PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_U8_ARRAY_devListArr = {
     "devListArr", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_ARRAY_MAC(GT_U8)
};

PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChVirtualTcamManagerDevListAdd_PARAMS[] =  {
    &IN_GT_U32_vTcamMngId,
    &IN_GT_U8_ARRAY_devListArr,
    &IN_GT_U32_numOfDevs
};

/********* structure fields log functions *********/
void prvCpssLogParamFuncStc_CPSS_DXCH_VIRTUAL_TCAM_RULE_ACTION_TYPE_STC_PTR
(
    IN CPSS_LOG_LIB_ENT         contextLib,
    IN CPSS_LOG_TYPE_ENT        logType,
    IN GT_CHAR_PTR              namePtr,
    IN void                   * fieldPtr,
    INOUT PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC     * inOutParamInfoPtr
)
{
    CPSS_DXCH_VIRTUAL_TCAM_RULE_TYPE_ENT ruleType;

    PRV_CPSS_LOG_START_FIELD_STC_MAC(CPSS_DXCH_VIRTUAL_TCAM_RULE_ACTION_TYPE_STC *, valPtr);
    PRV_CPSS_LOG_UNUSED_MAC(inOutParamInfoPtr);
    PRV_CPSS_LOG_STC_ENUM_MAC(valPtr, ruleType, CPSS_DXCH_VIRTUAL_TCAM_RULE_TYPE_ENT);
    ruleType = valPtr->ruleType;
    prvCpssLogStcLogStart(contextLib,  logType, "action"); 
    if (ruleType == CPSS_DXCH_VIRTUAL_TCAM_RULE_TYPE_PCL_E) 
    {
        prvCpssLogStcLogStart(contextLib,  logType, "pcl");
        PRV_CPSS_LOG_STC_ENUM_MAC((&(&valPtr->action)->pcl), direction, CPSS_PCL_DIRECTION_ENT);
        prvCpssLogStcLogEnd(contextLib, logType);
    }
    prvCpssLogStcLogEnd(contextLib, logType);
    prvCpssLogStcLogEnd(contextLib, logType);
}
void prvCpssLogParamFuncStc_CPSS_DXCH_VIRTUAL_TCAM_RULE_ACTION_DATA_STC_PTR
(
    IN CPSS_LOG_LIB_ENT         contextLib,
    IN CPSS_LOG_TYPE_ENT        logType,
    IN GT_CHAR_PTR              namePtr,
    IN void                   * fieldPtr,
    INOUT PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC     * inOutParamInfoPtr
)
{
    CPSS_DXCH_VIRTUAL_TCAM_RULE_TYPE_ENT ruleType;

    PRV_CPSS_LOG_START_FIELD_STC_MAC(CPSS_DXCH_VIRTUAL_TCAM_RULE_ACTION_DATA_STC *, valPtr);
    ruleType = (CPSS_DXCH_VIRTUAL_TCAM_RULE_TYPE_ENT)inOutParamInfoPtr->paramKey.paramKeyArr[4];
    prvCpssLogStcLogStart(contextLib,  logType, "action");
    if (ruleType == CPSS_DXCH_VIRTUAL_TCAM_RULE_TYPE_PCL_E) {
        prvCpssLogStcLogStart(contextLib,  logType, "pcl");
        PRV_CPSS_LOG_STC_STC_PTR_MAC((&(&valPtr->action)->pcl), actionPtr, CPSS_DXCH_PCL_ACTION_STC);
        prvCpssLogStcLogEnd(contextLib, logType);
    }
    else
    {
        inOutParamInfoPtr->paramKey.paramKeyArr[5] = CPSS_DXCH_TTI_ACTION_TYPE2_ENT;
        prvCpssLogStcLogStart(contextLib,  logType, "tti");
        PRV_CPSS_LOG_STC_STC_PTR_MAC((&(&valPtr->action)->tti), actionPtr, CPSS_DXCH_TTI_ACTION_UNT);
        prvCpssLogStcLogEnd(contextLib, logType);
    }
    prvCpssLogStcLogEnd(contextLib, logType);
    prvCpssLogStcLogEnd(contextLib, logType);
}
void prvCpssLogParamFuncStc_CPSS_DXCH_VIRTUAL_TCAM_RULE_TYPE_STC_PTR
(
    IN CPSS_LOG_LIB_ENT         contextLib,
    IN CPSS_LOG_TYPE_ENT        logType,
    IN GT_CHAR_PTR              namePtr,
    IN void                   * fieldPtr,
    INOUT PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC     * inOutParamInfoPtr
)
{
    CPSS_DXCH_VIRTUAL_TCAM_RULE_TYPE_ENT ruleType;

    PRV_CPSS_LOG_START_FIELD_STC_MAC(CPSS_DXCH_VIRTUAL_TCAM_RULE_TYPE_STC *, valPtr);
    PRV_CPSS_LOG_UNUSED_MAC(inOutParamInfoPtr);
    PRV_CPSS_LOG_STC_ENUM_MAC(valPtr, ruleType, CPSS_DXCH_VIRTUAL_TCAM_RULE_TYPE_ENT);
    ruleType = valPtr->ruleType;
    prvCpssLogStcLogStart(contextLib,  logType, "rule");
    if (ruleType == CPSS_DXCH_VIRTUAL_TCAM_RULE_TYPE_PCL_E) 
    {
        prvCpssLogStcLogStart(contextLib,  logType, "pcl"); 
        PRV_CPSS_LOG_STC_ENUM_MAC((&(&valPtr->rule)->pcl), ruleFormat, CPSS_DXCH_PCL_RULE_FORMAT_TYPE_ENT);
        prvCpssLogStcLogEnd(contextLib, logType);
    }
    else
    {
        prvCpssLogStcLogStart(contextLib,  logType, "tti");
        PRV_CPSS_LOG_STC_ENUM_MAC((&(&valPtr->rule)->tti), ruleFormat, CPSS_DXCH_TTI_RULE_TYPE_ENT);
        prvCpssLogStcLogEnd(contextLib, logType);
    }
    prvCpssLogStcLogEnd(contextLib, logType);
    prvCpssLogStcLogEnd(contextLib, logType);
}
void prvCpssLogParamFuncStc_CPSS_DXCH_VIRTUAL_TCAM_RULE_DATA_STC_PTR
(
    IN CPSS_LOG_LIB_ENT         contextLib,
    IN CPSS_LOG_TYPE_ENT        logType,
    IN GT_CHAR_PTR              namePtr,
    IN void                   * fieldPtr,
    INOUT PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC     * inOutParamInfoPtr
)
{
    CPSS_DXCH_VIRTUAL_TCAM_RULE_TYPE_ENT ruleType;

    PRV_CPSS_LOG_START_FIELD_STC_MAC(CPSS_DXCH_VIRTUAL_TCAM_RULE_DATA_STC *, valPtr);
    ruleType = (CPSS_DXCH_VIRTUAL_TCAM_RULE_TYPE_ENT)inOutParamInfoPtr->paramKey.paramKeyArr[4];
    PRV_CPSS_LOG_STC_BOOL_MAC(valPtr, valid);
    prvCpssLogStcLogStart(contextLib,  logType, "rule");
    if (ruleType == CPSS_DXCH_VIRTUAL_TCAM_RULE_TYPE_PCL_E) 
    {
        prvCpssLogStcLogStart(contextLib,  logType, "pcl"); 
        PRV_CPSS_LOG_STC_STC_PTR_MAC((&(&valPtr->rule)->pcl), maskPtr, CPSS_DXCH_PCL_RULE_FORMAT_UNT);
        PRV_CPSS_LOG_STC_STC_PTR_MAC((&(&valPtr->rule)->pcl), patternPtr, CPSS_DXCH_PCL_RULE_FORMAT_UNT);
        PRV_CPSS_LOG_STC_STC_PTR_MAC((&(&valPtr->rule)->pcl), actionPtr, CPSS_DXCH_PCL_ACTION_STC);
        prvCpssLogStcLogEnd(contextLib, logType);
    }
    else
    {
        prvCpssLogStcLogStart(contextLib,  logType, "tti");
        inOutParamInfoPtr->paramKey.paramKeyArr[5] = CPSS_DXCH_TTI_ACTION_TYPE2_ENT;
        PRV_CPSS_LOG_STC_STC_PTR_MAC((&(&valPtr->rule)->tti), maskPtr, CPSS_DXCH_TTI_RULE_UNT);
        PRV_CPSS_LOG_STC_STC_PTR_MAC((&(&valPtr->rule)->tti), patternPtr, CPSS_DXCH_TTI_RULE_UNT);
        PRV_CPSS_LOG_STC_STC_PTR_MAC((&(&valPtr->rule)->tti), actionPtr, CPSS_DXCH_TTI_ACTION_UNT);
        prvCpssLogStcLogEnd(contextLib, logType);
    }
    prvCpssLogStcLogEnd(contextLib, logType);
    prvCpssLogStcLogEnd(contextLib, logType);
}
void prvCpssLogParamFunc_CPSS_DXCH_VIRTUAL_TCAM_CHECK_INFO_STC_PTR(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_START_PARAM_STC_MAC(CPSS_DXCH_VIRTUAL_TCAM_CHECK_INFO_STC*, paramVal);
    prvCpssLogArrayOfParamsHandle(contextLib,logType,namePtr,(void*) paramVal, sizeof(CPSS_DXCH_VIRTUAL_TCAM_CHECK_INFO_STC), inOutParamInfoPtr,
                              prvCpssLogParamFuncStc_CPSS_DXCH_VIRTUAL_TCAM_CHECK_INFO_STC_PTR);
}

/********* parameters log functions *********/

void cpssDxChVirtualTcamManagerDevListAdd_preLogic
(
    IN va_list  args,
    OUT PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC * paramDataPtr
)
{
    /* vTcamMngId */
    va_arg(args, GT_U32);
    /* devListArr */
    va_arg(args, GT_U32*);
    /* numOfDevs */
    paramDataPtr->paramKey.paramKeyArr[2] = (GT_U32)va_arg(args, GT_U32);
}
void cpssDxChVirtualTcamMemoryAvailabilityCheck_preLogic
(
    IN va_list  args,
    OUT PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC * paramDataPtr
)
{
    /* vTcamMngId */
    va_arg(args, GT_U32);
    /* vTcamAmount */
    paramDataPtr->paramKey.paramKeyArr[2] = (GT_U32)va_arg(args, GT_U32);
}
void cpssDxChVirtualTcamRuleActionUpdate_preLogic
(
    IN va_list  args,
    OUT PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC * paramDataPtr
)
{
    CPSS_DXCH_VIRTUAL_TCAM_RULE_ACTION_TYPE_STC *actionTypePtr;

    /* vTcamMngId */
    va_arg(args, GT_U32);
    /* vTcamId */
    va_arg(args, GT_U32);
    /* ruleId */
    va_arg(args, CPSS_DXCH_VIRTUAL_TCAM_RULE_ID);
    /* actionTypePtr */
    actionTypePtr = (CPSS_DXCH_VIRTUAL_TCAM_RULE_ACTION_TYPE_STC*)va_arg(args, CPSS_DXCH_VIRTUAL_TCAM_RULE_ACTION_TYPE_STC*);
    paramDataPtr->paramKey.paramKeyArr[4] = (CPSS_DXCH_VIRTUAL_TCAM_RULE_TYPE_ENT)actionTypePtr->ruleType;
}
void cpssDxChVirtualTcamRuleWrite_preLogic
(
    IN va_list  args,
    OUT PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC * paramDataPtr
)
{
    CPSS_DXCH_VIRTUAL_TCAM_RULE_TYPE_STC *ruleTypePtr;

    /* vTcamMngId */
    va_arg(args, GT_U32);
    /* vTcamId */
    va_arg(args, GT_U32);
    /* ruleId */
    va_arg(args, CPSS_DXCH_VIRTUAL_TCAM_RULE_ID);
    /* ruleAttributesPtr */
    va_arg(args, CPSS_DXCH_VIRTUAL_TCAM_RULE_ATTRIBUTES_STC*);
    /* ruleTypePtr */
    ruleTypePtr = (CPSS_DXCH_VIRTUAL_TCAM_RULE_TYPE_STC*)va_arg(args, CPSS_DXCH_VIRTUAL_TCAM_RULE_TYPE_STC*);
    if (ruleTypePtr != NULL) 
    {
        paramDataPtr->paramKey.paramKeyArr[4] = (CPSS_DXCH_VIRTUAL_TCAM_RULE_TYPE_ENT)ruleTypePtr->ruleType;
        if (ruleTypePtr->ruleType == CPSS_DXCH_VIRTUAL_TCAM_RULE_TYPE_PCL_E) 
        {
            paramDataPtr->paramKey.paramKeyArr[5] = (CPSS_DXCH_PCL_RULE_FORMAT_TYPE_ENT)ruleTypePtr->rule.pcl.ruleFormat; 
        }
        else
        {
            paramDataPtr->paramKey.paramKeyArr[5] = (CPSS_DXCH_TTI_RULE_TYPE_ENT)ruleTypePtr->rule.tti.ruleFormat; 
        }
    }
    else
    {
        paramDataPtr->paramKey.paramKeyArr[4] = 0;
    }
}
void cpssDxChVirtualTcamRuleRead_preLogic
(
    IN va_list  args,
    OUT PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC * paramDataPtr
)
{
    CPSS_DXCH_VIRTUAL_TCAM_RULE_TYPE_STC *ruleTypePtr;

    /* vTcamMngId */
    va_arg(args, GT_U32);
    /* vTcamId */
    va_arg(args, GT_U32);
    /* ruleId */
    va_arg(args, CPSS_DXCH_VIRTUAL_TCAM_RULE_ID);
    /* ruleTypePtr */
    ruleTypePtr = (CPSS_DXCH_VIRTUAL_TCAM_RULE_TYPE_STC*)va_arg(args, CPSS_DXCH_VIRTUAL_TCAM_RULE_TYPE_STC*);
    if (ruleTypePtr != NULL) 
    {
        paramDataPtr->paramKey.paramKeyArr[4] = (CPSS_DXCH_VIRTUAL_TCAM_RULE_TYPE_ENT)ruleTypePtr->ruleType;
        if (ruleTypePtr->ruleType == CPSS_DXCH_VIRTUAL_TCAM_RULE_TYPE_PCL_E) 
        {
            paramDataPtr->paramKey.paramKeyArr[5] = (CPSS_DXCH_PCL_RULE_FORMAT_TYPE_ENT)ruleTypePtr->rule.pcl.ruleFormat; 
        }
        else
        {
            paramDataPtr->paramKey.paramKeyArr[5] = (CPSS_DXCH_TTI_RULE_TYPE_ENT)ruleTypePtr->rule.tti.ruleFormat; 
        }
    }
    else
    {
        paramDataPtr->paramKey.paramKeyArr[4] = 0;
    }
}

