/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* prvCpssDxChCutThroughLog.c
*       WARNING!!! this is a generated file, please don't edit it manually
* COMMENTS:
*
* FILE REVISION NUMBER:
*       $Revision: 1 $
*******************************************************************************/
#include <cpss/generic/log/cpssLog.h>
#include <cpss/generic/log/prvCpssLog.h>
#include <cpss/extServices/private/prvCpssBindFunc.h>
#include <cpss/dxCh/dxChxGen/cutThrough/cpssDxChCutThrough.h>
#include <cpss/dxCh/dxChxGen/cutThrough/private/prvCpssDxChCutThroughLog.h>
#include <cpss/generic/log/prvCpssGenCommonTypesLog.h>
#include <cpss/generic/log/prvCpssGenDbLog.h>
#include <cpss/generic/port/private/prvCpssGenPortLog.h>


/********* enums *********/

char * prvCpssLogEnum_CPSS_DXCH_CUT_THROUGH_BYPASS_INGRESS_PCL_ENT[]  =
{
    "CPSS_DXCH_CUT_THROUGH_BYPASS_INGRESS_PCL_NO_BYPASS_E",
    "CPSS_DXCH_CUT_THROUGH_BYPASS_INGRESS_PCL_ONE_LOOKUP_E",
    "CPSS_DXCH_CUT_THROUGH_BYPASS_INGRESS_PCL_TWO_LOOKUP_E",
    "CPSS_DXCH_CUT_THROUGH_BYPASS_INGRESS_PCL_FULL_BYPASS_E"
};
PRV_CPSS_LOG_STC_ENUM_ARRAY_SIZE_MAC(CPSS_DXCH_CUT_THROUGH_BYPASS_INGRESS_PCL_ENT);


/********* structure fields log functions *********/

void prvCpssLogParamFuncStc_CPSS_DXCH_CUT_THROUGH_BYPASS_STC_PTR
(
    IN CPSS_LOG_LIB_ENT         contextLib,
    IN CPSS_LOG_TYPE_ENT        logType,
    IN GT_CHAR_PTR              namePtr,
    IN void                   * fieldPtr,
    INOUT PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC     * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_START_FIELD_STC_MAC(CPSS_DXCH_CUT_THROUGH_BYPASS_STC *, valPtr);
    PRV_CPSS_LOG_UNUSED_MAC(inOutParamInfoPtr);
    PRV_CPSS_LOG_STC_BOOL_MAC(valPtr, bypassRouter);
    PRV_CPSS_LOG_STC_BOOL_MAC(valPtr, bypassIngressPolicerStage0);
    PRV_CPSS_LOG_STC_BOOL_MAC(valPtr, bypassIngressPolicerStage1);
    PRV_CPSS_LOG_STC_BOOL_MAC(valPtr, bypassEgressPolicer);
    PRV_CPSS_LOG_STC_BOOL_MAC(valPtr, bypassEgressPcl);
    PRV_CPSS_LOG_STC_ENUM_MAC(valPtr, bypassIngressPcl, CPSS_DXCH_CUT_THROUGH_BYPASS_INGRESS_PCL_ENT);
    prvCpssLogStcLogEnd(contextLib, logType);
}


/********* parameters log functions *********/

void prvCpssLogParamFunc_CPSS_DXCH_CUT_THROUGH_BYPASS_STC_PTR(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_START_PARAM_STC_MAC(CPSS_DXCH_CUT_THROUGH_BYPASS_STC*, paramVal);
    prvCpssLogParamFuncStc_CPSS_DXCH_CUT_THROUGH_BYPASS_STC_PTR(contextLib, logType, namePtr, paramVal, inOutParamInfoPtr);
}


/********* API fields DB *********/

PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_DXCH_CUT_THROUGH_BYPASS_STC_PTR_bypassModePtr = {
     "bypassModePtr", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(CPSS_DXCH_CUT_THROUGH_BYPASS_STC)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_PORT_SPEED_ENT_portSpeed = {
     "portSpeed", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_MAC(CPSS_PORT_SPEED_ENT)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_BOOL_buffersLimitEnable = {
     "buffersLimitEnable", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_MAC(GT_BOOL)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_BOOL_untaggedEnable = {
     "untaggedEnable", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_MAC(GT_BOOL)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_U32_buffersLimit = {
     "buffersLimit", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_MAC(GT_U32)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_U32_etherType0 = {
     "etherType0", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_MAC(GT_U32)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_U32_etherType1 = {
     "etherType1", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_MAC(GT_U32)
};
PRV_CPSS_LOG_FUNC_PARAM_STC OUT_CPSS_DXCH_CUT_THROUGH_BYPASS_STC_PTR_bypassModePtr = {
     "bypassModePtr", PRV_CPSS_LOG_PARAM_OUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(CPSS_DXCH_CUT_THROUGH_BYPASS_STC)
};
PRV_CPSS_LOG_FUNC_PARAM_STC OUT_CPSS_PORT_SPEED_ENT_PTR_portSpeedPtr = {
     "portSpeedPtr", PRV_CPSS_LOG_PARAM_OUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(CPSS_PORT_SPEED_ENT)
};
PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_BOOL_PTR_buffersLimitEnablePtr = {
     "buffersLimitEnablePtr", PRV_CPSS_LOG_PARAM_OUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(GT_BOOL)
};
PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_BOOL_PTR_untaggedEnablePtr = {
     "untaggedEnablePtr", PRV_CPSS_LOG_PARAM_OUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(GT_BOOL)
};
PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_U32_PTR_buffersLimitPtr = {
     "buffersLimitPtr", PRV_CPSS_LOG_PARAM_OUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(GT_U32)
};
PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_U32_PTR_etherType0Ptr = {
     "etherType0Ptr", PRV_CPSS_LOG_PARAM_OUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(GT_U32)
};
PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_U32_PTR_etherType1Ptr = {
     "etherType1Ptr", PRV_CPSS_LOG_PARAM_OUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(GT_U32)
};


/********* API prototypes DB *********/

PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChCutThroughBypassModeSet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_CPSS_DXCH_CUT_THROUGH_BYPASS_STC_PTR_bypassModePtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChCutThroughMemoryRateLimitSet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_PHYSICAL_PORT_NUM_portNum,
    &IN_GT_BOOL_enable,
    &IN_CPSS_PORT_SPEED_ENT_portSpeed
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChCutThroughPortEnableSet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_PHYSICAL_PORT_NUM_portNum,
    &IN_GT_BOOL_enable,
    &IN_GT_BOOL_untaggedEnable
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChCutThroughMemoryRateLimitGet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_PHYSICAL_PORT_NUM_portNum,
    &OUT_GT_BOOL_PTR_enablePtr,
    &OUT_CPSS_PORT_SPEED_ENT_PTR_portSpeedPtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChCutThroughPortEnableGet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_PHYSICAL_PORT_NUM_portNum,
    &OUT_GT_BOOL_PTR_enablePtr,
    &OUT_GT_BOOL_PTR_untaggedEnablePtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChCutThroughPortGroupMaxBuffersLimitSet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_PORT_GROUPS_BMP_portGroupsBmp,
    &IN_GT_BOOL_buffersLimitEnable,
    &IN_GT_U32_buffersLimit
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChCutThroughPortGroupMaxBuffersLimitGet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_PORT_GROUPS_BMP_portGroupsBmp,
    &OUT_GT_BOOL_PTR_buffersLimitEnablePtr,
    &OUT_GT_U32_PTR_buffersLimitPtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChCutThroughVlanEthertypeSet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_U32_etherType0,
    &IN_GT_U32_etherType1
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChCutThroughMinimalPacketSizeSet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_U32_size
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChCutThroughUpEnableSet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_U8_up,
    &IN_GT_BOOL_enable
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChCutThroughUpEnableGet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_U8_up,
    &OUT_GT_BOOL_PTR_enablePtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChCutThroughBypassModeGet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &OUT_CPSS_DXCH_CUT_THROUGH_BYPASS_STC_PTR_bypassModePtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChCutThroughVlanEthertypeGet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &OUT_GT_U32_PTR_etherType0Ptr,
    &OUT_GT_U32_PTR_etherType1Ptr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChCutThroughMinimalPacketSizeGet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &OUT_GT_U32_PTR_sizePtr
};


/********* lib API DB *********/

static PRV_CPSS_LOG_FUNC_ENTRY_STC prvCpssDxChCutThroughLogLibDb[] = {
    {"cpssDxChCutThroughPortEnableSet", 4, cpssDxChCutThroughPortEnableSet_PARAMS, NULL},
    {"cpssDxChCutThroughPortEnableGet", 4, cpssDxChCutThroughPortEnableGet_PARAMS, NULL},
    {"cpssDxChCutThroughUpEnableSet", 3, cpssDxChCutThroughUpEnableSet_PARAMS, NULL},
    {"cpssDxChCutThroughUpEnableGet", 3, cpssDxChCutThroughUpEnableGet_PARAMS, NULL},
    {"cpssDxChCutThroughVlanEthertypeSet", 3, cpssDxChCutThroughVlanEthertypeSet_PARAMS, NULL},
    {"cpssDxChCutThroughVlanEthertypeGet", 3, cpssDxChCutThroughVlanEthertypeGet_PARAMS, NULL},
    {"cpssDxChCutThroughMinimalPacketSizeSet", 2, cpssDxChCutThroughMinimalPacketSizeSet_PARAMS, NULL},
    {"cpssDxChCutThroughMinimalPacketSizeGet", 2, cpssDxChCutThroughMinimalPacketSizeGet_PARAMS, NULL},
    {"cpssDxChCutThroughMemoryRateLimitSet", 4, cpssDxChCutThroughMemoryRateLimitSet_PARAMS, NULL},
    {"cpssDxChCutThroughMemoryRateLimitGet", 4, cpssDxChCutThroughMemoryRateLimitGet_PARAMS, NULL},
    {"cpssDxChCutThroughBypassRouterAndPolicerEnableSet", 2, prvCpssLogGenDevNumEnable_PARAMS, NULL},
    {"cpssDxChCutThroughBypassRouterAndPolicerEnableGet", 2, prvCpssLogGenDevNumEnablePtr_PARAMS, NULL},
    {"cpssDxChCutThroughBypassModeSet", 2, cpssDxChCutThroughBypassModeSet_PARAMS, NULL},
    {"cpssDxChCutThroughBypassModeGet", 2, cpssDxChCutThroughBypassModeGet_PARAMS, NULL},
    {"cpssDxChCutThroughPortGroupMaxBuffersLimitSet", 4, cpssDxChCutThroughPortGroupMaxBuffersLimitSet_PARAMS, NULL},
    {"cpssDxChCutThroughPortGroupMaxBuffersLimitGet", 4, cpssDxChCutThroughPortGroupMaxBuffersLimitGet_PARAMS, NULL},
};

/******** DB Access Function ********/
void prvCpssLogParamLibDbGet_CPSS_LOG_LIB_CUT_THROUGH(
    OUT PRV_CPSS_LOG_FUNC_ENTRY_STC ** logFuncDbPtrPtr,
    OUT GT_U32 * logFuncDbSizePtr
)
{
    *logFuncDbPtrPtr = prvCpssDxChCutThroughLogLibDb;
    *logFuncDbSizePtr = sizeof(prvCpssDxChCutThroughLogLibDb) / sizeof(PRV_CPSS_LOG_FUNC_ENTRY_STC);
}

