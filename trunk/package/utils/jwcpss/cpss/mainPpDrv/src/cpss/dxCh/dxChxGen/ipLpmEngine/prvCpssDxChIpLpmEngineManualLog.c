/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* prvCpssDxChIpLpmEngineManualLog.c
*       Manually implemented CPSS Log type wrappers
* COMMENTS:
*
* FILE REVISION NUMBER:
*       $Revision: 1 $
*******************************************************************************/
#include <cpss/generic/log/cpssLog.h>
#include <cpss/generic/log/prvCpssLog.h>
#include <cpss/extServices/private/prvCpssBindFunc.h>
#include <cpss/dxCh/dxChxGen/ip/private/prvCpssDxChIpLog.h>
#include <cpss/dxCh/dxChxGen/ipLpmEngine/cpssDxChIpLpmTypes.h>
#include <cpss/dxCh/dxChxGen/ipLpmEngine/private/prvCpssDxChIpLpmEngineLog.h>
#include <cpss/dxCh/dxChxGen/ipLpmEngine/cpssDxChIpLpm.h>
#include <cpss/dxCh/dxChxGen/ipLpmEngine/private/cpssDxChPrvIpLpm.h>
#include <cpss/dxCh/dxChxGen/lpm/private/prvCpssDxChLpmLog.h>
#include <cpss/dxCh/dxChxGen/pcl/private/prvCpssDxChPclLog.h>
#include <cpss/generic/log/prvCpssGenLog.h>

/********* enums *********/
/********* structure fields log functions *********/
void prvCpssLogParamFuncStc_CPSS_DXCH_IP_TCAM_ROUTE_ENTRY_INFO_UNT_PTR
(
    IN CPSS_LOG_LIB_ENT         contextLib,
    IN CPSS_LOG_TYPE_ENT        logType,
    IN GT_CHAR_PTR              namePtr,
    IN void                   * fieldPtr,
    INOUT PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC     * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_START_FIELD_STC_MAC(CPSS_DXCH_IP_TCAM_ROUTE_ENTRY_INFO_UNT *, valPtr);
    if (inOutParamInfoPtr->paramKey.paramKeyArr[4] == GT_TRUE) 
    {
        PRV_CPSS_LOG_STC_STC_MAC(valPtr, pclIpUcAction, CPSS_DXCH_PCL_ACTION_STC); 
    }
    else
    {
        PRV_CPSS_LOG_STC_STC_MAC(valPtr, ipLttEntry, CPSS_DXCH_IP_LTT_ENTRY_STC);
    }
    prvCpssLogStcLogEnd(contextLib, logType);
}

/********* parameters log functions *********/
void prvCpssLogParamFunc_CPSS_DXCH_IP_LPM_MEMORY_CONFIG_UNT_PTR(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
)
{
    CPSS_DXCH_IP_LPM_SHADOW_TYPE_ENT shadowType;
    PRV_CPSS_LOG_START_PARAM_STC_MAC(CPSS_DXCH_IP_LPM_MEMORY_CONFIG_UNT*, paramVal);

    prvCpssLogStcLogStart(contextLib, logType, namePtr);

    shadowType = (inOutParamInfoPtr->paramKey.paramKeyArr[1] == PRV_CPSS_LOG_PARAM_IN_E) ?
          ((CPSS_DXCH_IP_LPM_SHADOW_TYPE_ENT)inOutParamInfoPtr->paramKey.paramKeyArr[0]):
          ((inOutParamInfoPtr->paramKey.paramKeyArr[0] == 0) ? CPSS_DXCH_IP_LPM_RAM_LION3_SHADOW_E :
              *(CPSS_DXCH_IP_LPM_SHADOW_TYPE_ENT*)inOutParamInfoPtr->paramKey.paramKeyArr[0]);

    if((shadowType == CPSS_DXCH_IP_LPM_TCAM_CHEETAH_SHADOW_E) ||
       (shadowType == CPSS_DXCH_IP_LPM_TCAM_CHEETAH2_SHADOW_E) ||
       (shadowType == CPSS_DXCH_IP_LPM_TCAM_CHEETAH3_SHADOW_E) ||
       (shadowType == CPSS_DXCH_IP_LPM_TCAM_XCAT_SHADOW_E) ||
       (shadowType == CPSS_DXCH_IP_LPM_TCAM_XCAT_POLICY_BASED_ROUTING_SHADOW_E) ||
       (shadowType == CPSS_DXCH_IP_LPM_RAM_LION3_SHADOW_E))
    {
        if (shadowType < CPSS_DXCH_IP_LPM_RAM_LION3_SHADOW_E) 
        {
            PRV_CPSS_LOG_STC_STC_MAC(paramVal, tcamDbCfg, CPSS_DXCH_IP_LPM_TCAM_CONFIG_STC);
        }
        else
        {
            PRV_CPSS_LOG_STC_STC_MAC(paramVal, ramDbCfg, CPSS_DXCH_LPM_RAM_CONFIG_STC);
        }

    }
    prvCpssLogStcLogEnd(contextLib, logType);
}
void prvCpssLogParamFunc_CPSS_DXCH_IP_LPM_IPV4_UC_PREFIX_STC_PTR(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_START_PARAM_STC_MAC(CPSS_DXCH_IP_LPM_IPV4_UC_PREFIX_STC*, paramVal);
    prvCpssLogArrayOfParamsHandle(contextLib,logType,namePtr,(void*) paramVal, sizeof(CPSS_DXCH_IP_LPM_IPV4_UC_PREFIX_STC), inOutParamInfoPtr,
                                  prvCpssLogParamFuncStc_CPSS_DXCH_IP_LPM_IPV4_UC_PREFIX_STC_PTR);
}
void prvCpssLogParamFunc_CPSS_DXCH_IP_LPM_IPV6_UC_PREFIX_STC_PTR(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_START_PARAM_STC_MAC(CPSS_DXCH_IP_LPM_IPV6_UC_PREFIX_STC*, paramVal);
    prvCpssLogArrayOfParamsHandle(contextLib,logType,namePtr,(void*) paramVal, sizeof(CPSS_DXCH_IP_LPM_IPV6_UC_PREFIX_STC), inOutParamInfoPtr,
                                  prvCpssLogParamFuncStc_CPSS_DXCH_IP_LPM_IPV6_UC_PREFIX_STC_PTR);
}

/********* structure fields log functions *********/
static void prvCpssLogParamFunc_GT_U32_ARRAY_sizeSet1(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_SET_PARAM_VAL_MAC(GT_U32*, paramVal);
    inOutParamInfoPtr->paramKey.paramKeyArr[0] = inOutParamInfoPtr->paramKey.paramKeyArr[4];
    inOutParamInfoPtr->paramKey.paramKeyArr[1] = inOutParamInfoPtr->paramKey.paramKeyArr[5];
    prvCpssLogArrayOfParamsHandle(contextLib,logType,namePtr,paramVal,sizeof(GT_U32),inOutParamInfoPtr,prvCpssLogParamFuncStc_GT_U32_PTR);
}
static void prvCpssLogParamFunc_GT_U32_ARRAY_sizeSet2(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_SET_PARAM_VAL_MAC(GT_U32*, paramVal);
    inOutParamInfoPtr->paramKey.paramKeyArr[0] = inOutParamInfoPtr->paramKey.paramKeyArr[6];
    inOutParamInfoPtr->paramKey.paramKeyArr[1] = inOutParamInfoPtr->paramKey.paramKeyArr[7];
    prvCpssLogArrayOfParamsHandle(contextLib,logType,namePtr,paramVal,sizeof(GT_U32),inOutParamInfoPtr,prvCpssLogParamFuncStc_GT_U32_PTR);
}

PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_U8_ARRAY_devListArr1 = {
     "devListArr", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_ARRAY_MAC(GT_U8)
};
PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_U8_ARRAY_devListArray = {
     "devListArray", PRV_CPSS_LOG_PARAM_OUT_E,  PRV_CPSS_LOG_FUNC_TYPE_ARRAY_MAC(GT_U8)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_U32_ARRAY_linesToReserveArray = {
     "linesToReserveArray", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_ARRAY_MAC(GT_U32)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_U32_ARRAY_pclIdArray = {
     "pclIdArray", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_ARRAY_MAC(GT_U32)
};
PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_U32_ARRAY_defaultIpv4RuleIndexArray = {
     "defaultIpv4RuleIndexArray", PRV_CPSS_LOG_PARAM_OUT_E,  PRV_CPSS_LOG_FUNC_TYPE_ARRAY_MAC(GT_U32)
};
PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_U32_ARRAY_defaultIpv6RuleIndexArray = {
     "defaultIpv6RuleIndexArray", PRV_CPSS_LOG_PARAM_OUT_E,  prvCpssLogParamFunc_GT_U32_ARRAY_sizeSet1
};
PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_U32_ARRAY_pclIdArray = {
     "pclIdArray", PRV_CPSS_LOG_PARAM_OUT_E,  prvCpssLogParamFunc_GT_U32_ARRAY_sizeSet2
};

PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChIpLpmDBDevListAdd_PARAMS[] =  {
    &IN_GT_U32_lpmDBId,
    &IN_GT_U8_ARRAY_devListArr1,
    &IN_GT_U32_numOfDevs
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChIpLpmDBDevListGet_PARAMS[] =  {
    &IN_GT_U32_lpmDbId,
    &INOUT_GT_U32_PTR_numOfDevsPtr,
    &OUT_GT_U8_ARRAY_devListArray
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChIpLpmTcamLinesReserve_PARAMS[] =  {
    &IN_GT_U32_lpmDBId,
    &IN_GT_U32_ARRAY_linesToReserveArray,
    &IN_GT_U32_numberOfLinesToReserve,
    &IN_CPSS_DXCH_IP_TCAM_LPM_MANGER_CAPCITY_CFG_STC_allocateReserveLinesAs
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChIpLpmPolicyBasedRoutingDefaultMcSet_PARAMS[] =  {
    &IN_GT_U32_lpmDBId,
    &IN_GT_U32_pclIdArrayLen,
    &IN_GT_U32_ARRAY_pclIdArray
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChIpLpmPolicyBasedRoutingDefaultMcGet_PARAMS[] =  {
    &IN_GT_U32_lpmDBId,
    &OUT_GT_BOOL_PTR_defaultMcUsedPtr,
    &OUT_CPSS_IP_PROTOCOL_STACK_ENT_PTR_protocolStackPtr,
    &OUT_GT_U32_ARRAY_defaultIpv4RuleIndexArray,
    &INOUT_GT_U32_PTR_defaultIpv4RuleIndexArrayLenPtr,
    &OUT_GT_U32_ARRAY_defaultIpv6RuleIndexArray,
    &INOUT_GT_U32_PTR_defaultIpv6RuleIndexArrayLenPtr,
    &OUT_GT_U32_ARRAY_pclIdArray,
    &INOUT_GT_U32_PTR_pclIdArrayLenPtr
};

/********* api pre-log functions *********/
void cpssDxChIpLpmDBCreate_preLogic
(
    IN va_list  args,
    OUT PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC * paramDataPtr
)
{
    /* lpmDBId */
    va_arg(args, GT_U32);
    /* shadowType - key for union parsing */
    paramDataPtr->paramKey.paramKeyArr[0] = (GT_UINTPTR)va_arg(args, CPSS_DXCH_IP_LPM_SHADOW_TYPE_ENT);
    paramDataPtr->paramKey.paramKeyArr[1] =  PRV_CPSS_LOG_PARAM_IN_E;
}

void cpssDxChIpLpmDBConfigGet_preLogic
(
    IN va_list  args,
    OUT PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC * paramDataPtr
)
{
    /* lpmDBId */
    va_arg(args, GT_U32);
    /* shadowTypePtr - key for union parsing */
    paramDataPtr->paramKey.paramKeyArr[0] = (GT_UINTPTR)va_arg(args, CPSS_DXCH_IP_LPM_SHADOW_TYPE_ENT *);
    paramDataPtr->paramKey.paramKeyArr[1] =  PRV_CPSS_LOG_PARAM_OUT_E;
}
void cpssDxChIpLpmDBDevListAdd_preLogic
(
    IN va_list  args,
    OUT PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC * paramDataPtr
)
{
    /* lpmDBId */
    va_arg(args, GT_U32);
    /* devListArr */
    va_arg(args, GT_U8*);
    /* numOfDevs */
    paramDataPtr->paramKey.paramKeyArr[2] = (GT_U32)va_arg(args, GT_U32);
}
void cpssDxChIpLpmDBDevListGet_preLogic
(
    IN va_list  args,
    OUT PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC * paramDataPtr
)
{
    GT_U32  *sizePtr; /*pointer to array size*/

    /* lpmDbId */
    va_arg(args, GT_U32);
    /* maxNumOfDevsPtr */
    sizePtr = (GT_U32 *)va_arg(args, GT_U32 *);
    paramDataPtr->paramKey.paramKeyArr[0] = (GT_UINTPTR)sizePtr;
    if (paramDataPtr->paramKey.paramKeyArr[0] != 0)
        paramDataPtr->paramKey.paramKeyArr[1] = (GT_U32)*sizePtr;
    /* sign that the output might be an array of strucutures */
    paramDataPtr->paramKey.paramKeyArr[3] = 1;
}
void cpssDxChIpLpmTcamLinesReserve_preLogic
(
    IN va_list  args,
    OUT PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC * paramDataPtr
)
{
    /* lpmDBId */
    va_arg(args, GT_U32);
    /* linesToReserveArray */
    va_arg(args, GT_U32*);
    /* numberOfLinesToReserve */
    paramDataPtr->paramKey.paramKeyArr[2] = (GT_U32)va_arg(args, GT_U32);
}
void cpssDxChIpLpmPolicyBasedRoutingDefaultMcSet_preLogic
(
    IN va_list  args,
    OUT PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC * paramDataPtr
)
{
    /* lpmDBId */
    va_arg(args, GT_U32);
    /* pclIdArrayLen */
    paramDataPtr->paramKey.paramKeyArr[2] = (GT_U32)va_arg(args, GT_U32);
}
void cpssDxChIpLpmPolicyBasedRoutingDefaultMcGet_preLogic
(
    IN va_list  args,
    OUT PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC * paramDataPtr
)
{
    GT_U32  *sizePtr; /*pointer to array size*/

    /* lpmDbId */
    va_arg(args, GT_U32);
    /* defaultMcUsedPtr */
    va_arg(args, GT_BOOL*);
    /* protocolStackPtr */
    va_arg(args, CPSS_IP_PROTOCOL_STACK_ENT*);
    /* defaultIpv4RuleIndexArray */
    va_arg(args, GT_U32*);
    /* maxDefaultIpv4RuleIndexArrayLenPtr */
    sizePtr = (GT_U32 *)va_arg(args, GT_U32 *);
    paramDataPtr->paramKey.paramKeyArr[0] = (GT_UINTPTR)sizePtr;
    if (paramDataPtr->paramKey.paramKeyArr[0] != 0)
        paramDataPtr->paramKey.paramKeyArr[1] = (GT_U32)*sizePtr;
    /* defaultIpv6RuleIndexArray */
    va_arg(args, GT_U32*);
    /* maxDefaultIpv6RuleIndexArrayLenPtr */
    sizePtr = (GT_U32 *)va_arg(args, GT_U32 *);
    paramDataPtr->paramKey.paramKeyArr[4] = (GT_UINTPTR)sizePtr;
    if (paramDataPtr->paramKey.paramKeyArr[4] != 0)
        paramDataPtr->paramKey.paramKeyArr[5] = (GT_U32)*sizePtr;
    /* pclIdArray */
    va_arg(args, GT_U32*);
    /* maxPclIdArrayLenPtr */
    sizePtr = (GT_U32 *)va_arg(args, GT_U32 *);
    paramDataPtr->paramKey.paramKeyArr[6] = (GT_UINTPTR)sizePtr;
    if (paramDataPtr->paramKey.paramKeyArr[6] != 0)
        paramDataPtr->paramKey.paramKeyArr[7] = (GT_U32)*sizePtr;
    /* sign that the output might be an array of strucutures */
    paramDataPtr->paramKey.paramKeyArr[3] = 1;
}
void cpssDxChIpLpmIpv4UcPrefixAdd_preLogic
(
    IN va_list  args,
    OUT PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC * paramDataPtr
)
{
    GT_U32 lpmDBId;
    GT_BOOL shadowType;
    GT_STATUS rc;

    /* lpmDBId */
    lpmDBId = va_arg(args, GT_U32);
    rc = prvCpssDxChIpLpmLogRouteEntryTypeGet(lpmDBId,&shadowType);
    if (rc == GT_OK) 
    {
        paramDataPtr->paramKey.paramKeyArr[4] = (GT_BOOL)shadowType;
    }
}

void cpssDxChIpLpmIpv4UcPrefixAddBulk_preLogic
(
    IN va_list  args,
    OUT PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC * paramDataPtr
)
{
    GT_U32 lpmDBId;
    GT_BOOL shadowType;
    GT_STATUS rc;

    /* lpmDBId */
    lpmDBId = va_arg(args, GT_U32);
    rc = prvCpssDxChIpLpmLogRouteEntryTypeGet(lpmDBId,&shadowType);
    if (rc == GT_OK) 
    {
        paramDataPtr->paramKey.paramKeyArr[4] = (GT_BOOL)shadowType;
    }
    /* ipv4PrefixArrayLen */
    paramDataPtr->paramKey.paramKeyArr[2] = (GT_U32)va_arg(args, GT_U32);
}
