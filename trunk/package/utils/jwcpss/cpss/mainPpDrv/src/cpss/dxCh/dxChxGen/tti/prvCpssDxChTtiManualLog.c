/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* prvCpssDxChTtiManualLog.c
*       Manually implemented CPSS Log type wrappers
* COMMENTS:
*
* FILE REVISION NUMBER:
*       $Revision: 1 $
*******************************************************************************/
#include <cpss/generic/log/cpssLog.h>
#include <cpss/generic/log/prvCpssLog.h>
#include <cpss/extServices/private/prvCpssBindFunc.h>
#include <cpss/generic/log/prvCpssGenLog.h>
#include <cpss/dxCh/dxChxGen/tti/cpssDxChTtiTypes.h>
#include <cpss/dxCh/dxChxGen/tti/private/prvCpssDxChTtiLog.h>

/********* enums *********/
/********* structure fields log functions *********/
void prvCpssLogParamFuncStc_CPSS_DXCH_TTI_RULE_UNT_PTR
(
    IN CPSS_LOG_LIB_ENT         contextLib,
    IN CPSS_LOG_TYPE_ENT        logType,
    IN GT_CHAR_PTR              namePtr,
    IN void                   * fieldPtr,
    INOUT PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC     * inOutParamInfoPtr
)
{
    CPSS_DXCH_TTI_RULE_TYPE_ENT ruleType;

    PRV_CPSS_LOG_START_FIELD_STC_MAC(CPSS_DXCH_TTI_RULE_UNT *, valPtr);
    ruleType = (CPSS_DXCH_TTI_RULE_TYPE_ENT)inOutParamInfoPtr->paramKey.paramKeyArr[5];
    switch (ruleType) 
    {
    case CPSS_DXCH_TTI_RULE_IPV4_E:
        PRV_CPSS_LOG_STC_STC_MAC(valPtr, ipv4, CPSS_DXCH_TTI_IPV4_RULE_STC); 
        break;
    case CPSS_DXCH_TTI_RULE_MPLS_E:
        PRV_CPSS_LOG_STC_STC_MAC(valPtr, mpls, CPSS_DXCH_TTI_MPLS_RULE_STC);
        break;
    case CPSS_DXCH_TTI_RULE_ETH_E:
        PRV_CPSS_LOG_STC_STC_MAC(valPtr, eth, CPSS_DXCH_TTI_ETH_RULE_STC);
        break;
    case CPSS_DXCH_TTI_RULE_MIM_E:
        PRV_CPSS_LOG_STC_STC_MAC(valPtr, mim, CPSS_DXCH_TTI_MIM_RULE_STC);
        break;
    case CPSS_DXCH_TTI_RULE_UDB_10_E:
    case CPSS_DXCH_TTI_RULE_UDB_20_E:
    case CPSS_DXCH_TTI_RULE_UDB_30_E:
        PRV_CPSS_LOG_STC_TYPE_ARRAY_MAC(valPtr, udbArray, CPSS_DXCH_TTI_MAX_UDB_CNS, GT_U8);
        break;
    default:
        PRV_CPSS_LOG_STC_STC_MAC(valPtr, trill, CPSS_DXCH_TTI_TRILL_RULE_STC);
        break;
    }
    prvCpssLogStcLogEnd(contextLib, logType);
}
void prvCpssLogParamFuncStc_CPSS_DXCH_TTI_ACTION_UNT_PTR
(
    IN CPSS_LOG_LIB_ENT         contextLib,
    IN CPSS_LOG_TYPE_ENT        logType,
    IN GT_CHAR_PTR              namePtr,
    IN void                   * fieldPtr,
    INOUT PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC     * inOutParamInfoPtr
)
{
    CPSS_DXCH_TTI_ACTION_TYPE_ENT actionType;

    PRV_CPSS_LOG_START_FIELD_STC_MAC(CPSS_DXCH_TTI_ACTION_UNT *, valPtr);
    actionType = (CPSS_DXCH_TTI_ACTION_TYPE_ENT)inOutParamInfoPtr->paramKey.paramKeyArr[5];
    if (actionType == CPSS_DXCH_TTI_ACTION_TYPE1_ENT) 
    {
        PRV_CPSS_LOG_STC_STC_MAC(valPtr, type1, CPSS_DXCH_TTI_ACTION_STC); 
    }
    else
    {
        PRV_CPSS_LOG_STC_STC_MAC(valPtr, type2, CPSS_DXCH_TTI_ACTION_2_STC);
    }
    prvCpssLogStcLogEnd(contextLib, logType);
}
/********* parameters log functions *********/
/********* api pre-log functions *********/
void cpssDxChTtiRuleSet_preLogic
(
    IN va_list  args,
    OUT PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC * paramDataPtr
)
{
    /* devNum */
    va_arg(args, GT_U32);
    /* index */
    va_arg(args, GT_U32);
    /* ruleType */
    paramDataPtr->paramKey.paramKeyArr[4] = (CPSS_DXCH_TTI_RULE_TYPE_ENT)va_arg(args, CPSS_DXCH_TTI_RULE_TYPE_ENT);
    /* patternPtr */
    va_arg(args, CPSS_DXCH_TTI_RULE_UNT*);     
    /* maskPtr */
    va_arg(args, CPSS_DXCH_TTI_RULE_UNT*);     
    /* actionType */
    paramDataPtr->paramKey.paramKeyArr[5] = (CPSS_DXCH_TTI_ACTION_TYPE_ENT)va_arg(args, CPSS_DXCH_TTI_ACTION_TYPE_ENT); 
}
void cpssDxChTtiRuleActionUpdate_preLogic
(
    IN va_list  args,
    OUT PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC * paramDataPtr
)
{
    /* devNum */
    va_arg(args, GT_U32);
    /* index */
    va_arg(args, GT_U32);
    /* actionType */
    paramDataPtr->paramKey.paramKeyArr[5] = (CPSS_DXCH_TTI_ACTION_TYPE_ENT)va_arg(args, CPSS_DXCH_TTI_ACTION_TYPE_ENT);  
}
void cpssDxChTtiPortGroupRuleSet_preLogic
(
    IN va_list  args,
    OUT PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC * paramDataPtr
)
{
    /* devNum */
    va_arg(args, GT_U32);
    /* portGroupsBmp */
    va_arg(args, GT_PORT_GROUPS_BMP);  
    /* index */
    va_arg(args, GT_U32);
    /* ruleType */
    paramDataPtr->paramKey.paramKeyArr[4] = (CPSS_DXCH_TTI_RULE_TYPE_ENT)va_arg(args, CPSS_DXCH_TTI_RULE_TYPE_ENT);
    /* patternPtr */
    va_arg(args, CPSS_DXCH_TTI_RULE_UNT*);     
    /* maskPtr */
    va_arg(args, CPSS_DXCH_TTI_RULE_UNT*);     
    /* actionType */
    paramDataPtr->paramKey.paramKeyArr[5] = (CPSS_DXCH_TTI_ACTION_TYPE_ENT)va_arg(args, CPSS_DXCH_TTI_ACTION_TYPE_ENT); 
}
void cpssDxChTtiPortGroupRuleActionUpdate_preLogic
(
    IN va_list  args,
    OUT PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC * paramDataPtr
)
{
    /* devNum */
    va_arg(args, GT_U32);
    /* portGroupsBmp */
    va_arg(args, GT_PORT_GROUPS_BMP);  
    /* index */
    va_arg(args, GT_U32);
    /* actionType */
    paramDataPtr->paramKey.paramKeyArr[5] = (CPSS_DXCH_TTI_ACTION_TYPE_ENT)va_arg(args, CPSS_DXCH_TTI_ACTION_TYPE_ENT); 
}

