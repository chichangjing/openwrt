/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* prvCpssDxChTcamManualLog.c
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
#include <cpss/dxCh/dxChxGen/tcam/cpssDxChTcam.h>
#include <cpss/dxCh/dxChxGen/tcam/private/prvCpssDxChTcamLog.h>

/********* enums *********/
/* helper function for calculating rule size */
static GT_U32 ruleSizeConvert
(   
    IN CPSS_DXCH_TCAM_RULE_SIZE_ENT   ruleSize
)
{
    GT_U32 actualRuleSize; /* in words */

    switch (ruleSize) 
    {
        case CPSS_DXCH_TCAM_RULE_SIZE_10_B_E:
            actualRuleSize = 3;
            break;
        case CPSS_DXCH_TCAM_RULE_SIZE_20_B_E:
            actualRuleSize = 5;
            break;
        case CPSS_DXCH_TCAM_RULE_SIZE_30_B_E:
            actualRuleSize = 8;
            break;
        case CPSS_DXCH_TCAM_RULE_SIZE_40_B_E:
            actualRuleSize = 10;
            break;
        case CPSS_DXCH_TCAM_RULE_SIZE_50_B_E:
            actualRuleSize = 13;
            break;
        case CPSS_DXCH_TCAM_RULE_SIZE_60_B_E:
            actualRuleSize = 15;
            break;
        case CPSS_DXCH_TCAM_RULE_SIZE_80_B_E:
            actualRuleSize = 20;
            break;
        default:
            actualRuleSize = 0;
    }
    return actualRuleSize;
}

/********* structure fields log functions *********/
static void prvCpssLogParamFunc_GT_U32_ARRAY_sizeSet(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
)
{
    CPSS_DXCH_TCAM_RULE_SIZE_ENT* ruleSizePtr;

    PRV_CPSS_LOG_SET_PARAM_VAL_MAC(GT_U32*, paramVal);
    ruleSizePtr = (CPSS_DXCH_TCAM_RULE_SIZE_ENT*)inOutParamInfoPtr->paramKey.paramKeyArr[0];
    if (ruleSizePtr != NULL) 
    {
        inOutParamInfoPtr->paramKey.paramKeyArr[2] = (GT_U32)ruleSizeConvert(*ruleSizePtr);
    }
    else
    {
        inOutParamInfoPtr->paramKey.paramKeyArr[2] = 0;
    }
    prvCpssLogArrayOfParamsHandle(contextLib,logType,namePtr,paramVal,sizeof(GT_U32),inOutParamInfoPtr,prvCpssLogParamFuncStc_GT_U32_PTR);
}

PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_U32_ARRAY_tcamPatternPtr = {
     "tcamPatternPtr", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_ARRAY_MAC(GT_U32)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_U32_ARRAY_tcamMaskPtr = {
     "tcamMaskPtr", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_ARRAY_MAC(GT_U32)
};
PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_U32_ARRAY_tcamPatternPtr = {
     "tcamPatternPtr", PRV_CPSS_LOG_PARAM_OUT_E, prvCpssLogParamFunc_GT_U32_ARRAY_sizeSet  
};
PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_U32_ARRAY_tcamMaskPtr = {
     "tcamMaskPtr", PRV_CPSS_LOG_PARAM_OUT_E,  PRV_CPSS_LOG_FUNC_TYPE_ARRAY_MAC(GT_U32)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_U32_ARRAY_tcamKeyPtr = {
     "tcamKeyPtr", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_ARRAY_MAC(GT_U32)
};
PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_U32_ARRAY_tcamKeyPtr = {
     "tcamKeyPtr", PRV_CPSS_LOG_PARAM_OUT_E,  prvCpssLogParamFunc_GT_U32_ARRAY_sizeSet
};

PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChTcamPortGroupRuleWrite_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_PORT_GROUPS_BMP_portGroupsBmp,
    &IN_GT_U32_index,
    &IN_GT_BOOL_valid,
    &IN_CPSS_DXCH_TCAM_RULE_SIZE_ENT_ruleSize,
    &IN_GT_U32_ARRAY_tcamPatternPtr,
    &IN_GT_U32_ARRAY_tcamMaskPtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChTcamPortGroupRuleRead_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_PORT_GROUPS_BMP_portGroupsBmp,
    &IN_GT_U32_index,
    &OUT_GT_BOOL_PTR_validPtr,
    &OUT_CPSS_DXCH_TCAM_RULE_SIZE_ENT_PTR_ruleSizePtr,
    &OUT_GT_U32_ARRAY_tcamPatternPtr,
    &OUT_GT_U32_ARRAY_tcamMaskPtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChTcamPortGroupCpuLookupTriggerSet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_PORT_GROUPS_BMP_portGroupsBmp,
    &IN_GT_U32_group,
    &IN_CPSS_DXCH_TCAM_RULE_SIZE_ENT_size,
    &IN_GT_U32_ARRAY_tcamKeyPtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChTcamPortGroupCpuLookupTriggerGet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_PORT_GROUPS_BMP_portGroupsBmp,
    &IN_GT_U32_group,
    &OUT_CPSS_DXCH_TCAM_RULE_SIZE_ENT_PTR_sizePtr,
    &OUT_GT_U32_ARRAY_tcamKeyPtr
};
/********* parameters log functions *********/
void prvCpssLogParamFunc_CPSS_DXCH_TCAM_BLOCK_INFO_STC_PTR(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_START_PARAM_STC_MAC(CPSS_DXCH_TCAM_BLOCK_INFO_STC*, paramVal);
    prvCpssLogArrayOfParamsHandle(contextLib,logType,namePtr,(void*) paramVal, sizeof(CPSS_DXCH_TCAM_BLOCK_INFO_STC), inOutParamInfoPtr,
                                  prvCpssLogParamFuncStc_CPSS_DXCH_TCAM_BLOCK_INFO_STC_PTR);
}

void cpssDxChTcamIndexRangeHitNumAndGroupSet_preLogic
(
    IN va_list  args,
    OUT PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC * paramDataPtr
)
{
    /* devNum */
    va_arg(args, GT_U32);
    /* setting the size of floorInfoArr - constant */
    paramDataPtr->paramKey.paramKeyArr[2] = CPSS_DXCH_TCAM_MAX_NUM_BLOCKS_CNS; 
}
void cpssDxChTcamPortGroupRuleWrite_preLogic
(
    IN va_list  args,
    OUT PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC * paramDataPtr
)
{
    CPSS_DXCH_TCAM_RULE_SIZE_ENT ruleSize;
    GT_U32 actualRuleSize; /* in words */

    /* devNum */
    va_arg(args, GT_U32);
    /* portGroupsBmp */
    va_arg(args, GT_PORT_GROUPS_BMP);
    /* index */
    va_arg(args, GT_U32);
    /* valid */
    va_arg(args, GT_BOOL);
    /* ruleSize */
    ruleSize = (CPSS_DXCH_TCAM_RULE_SIZE_ENT)va_arg(args, CPSS_DXCH_TCAM_RULE_SIZE_ENT);
    actualRuleSize = ruleSizeConvert(ruleSize);
    paramDataPtr->paramKey.paramKeyArr[2] = actualRuleSize; 
}
void cpssDxChTcamPortGroupRuleRead_preLogic
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
    /* validPtr */
    va_arg(args, GT_BOOL*);
    /* ruleSizePtr */
    paramDataPtr->paramKey.paramKeyArr[0] = (GT_UINTPTR)va_arg(args, CPSS_DXCH_TCAM_RULE_SIZE_ENT*); 
}
void cpssDxChTcamPortGroupCpuLookupTriggerSet_preLogic
(
    IN va_list  args,
    OUT PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC * paramDataPtr
)
{
    CPSS_DXCH_TCAM_RULE_SIZE_ENT ruleSize;
    GT_U32 actualRuleSize; /* in words */

    /* devNum */
    va_arg(args, GT_U32);
    /* portGroupsBmp */
    va_arg(args, GT_PORT_GROUPS_BMP);
    /* group */
    va_arg(args, GT_U32);
    /* size */
    ruleSize = (CPSS_DXCH_TCAM_RULE_SIZE_ENT)va_arg(args, CPSS_DXCH_TCAM_RULE_SIZE_ENT);
    actualRuleSize = ruleSizeConvert(ruleSize);
    paramDataPtr->paramKey.paramKeyArr[2] = actualRuleSize; 
}
void cpssDxChTcamPortGroupCpuLookupTriggerGet_preLogic
(
    IN va_list  args,
    OUT PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC * paramDataPtr
)
{
    /* devNum */
    va_arg(args, GT_U32);
    /* portGroupsBmp */
    va_arg(args, GT_PORT_GROUPS_BMP);
    /* group */
    va_arg(args, GT_U32);
    /* sizePtr */
    paramDataPtr->paramKey.paramKeyArr[0] = (GT_UINTPTR)va_arg(args, CPSS_DXCH_TCAM_RULE_SIZE_ENT*); 
}

