/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* prvCpssDxChCutThroughLog.h
*       WARNING!!! this is a generated file, please don't edit it manually
* COMMENTS:
*
* FILE REVISION NUMBER:
*       $Revision: 1 $
*******************************************************************************/
#ifndef __prvCpssDxChCutThroughLogh
#define __prvCpssDxChCutThroughLogh
#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
#include <cpss/generic/log/cpssLog.h>
#include <cpss/generic/log/prvCpssLog.h>


/********* enums *********/

PRV_CPSS_LOG_STC_ENUM_DECLARE_MAC(CPSS_DXCH_CUT_THROUGH_BYPASS_INGRESS_PCL_ENT);


/********* structure fields log functions *********/

void prvCpssLogParamFuncStc_CPSS_DXCH_CUT_THROUGH_BYPASS_STC_PTR
(
    IN CPSS_LOG_LIB_ENT         contextLib,
    IN CPSS_LOG_TYPE_ENT        logType,
    IN GT_CHAR_PTR              namePtr,
    IN void                   * fieldPtr,
    INOUT PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC     * inOutParamInfoPtr
);


/********* parameters log functions *********/

void prvCpssLogParamFunc_CPSS_DXCH_CUT_THROUGH_BYPASS_STC_PTR(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
);


/********* API fields DB *********/

extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_DXCH_CUT_THROUGH_BYPASS_STC_PTR_bypassModePtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_PORT_SPEED_ENT_portSpeed;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_BOOL_buffersLimitEnable;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_BOOL_untaggedEnable;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_U32_buffersLimit;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_U32_etherType0;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_U32_etherType1;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_CPSS_DXCH_CUT_THROUGH_BYPASS_STC_PTR_bypassModePtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_CPSS_PORT_SPEED_ENT_PTR_portSpeedPtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_BOOL_PTR_buffersLimitEnablePtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_BOOL_PTR_untaggedEnablePtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_U32_PTR_buffersLimitPtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_U32_PTR_etherType0Ptr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_U32_PTR_etherType1Ptr;


/********* lib API DB *********/

enum {
    PRV_CPSS_LOG_FUNC_cpssDxChCutThroughPortEnableSet_E = (CPSS_LOG_LIB_CUT_THROUGH_E << 16),
    PRV_CPSS_LOG_FUNC_cpssDxChCutThroughPortEnableGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChCutThroughUpEnableSet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChCutThroughUpEnableGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChCutThroughVlanEthertypeSet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChCutThroughVlanEthertypeGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChCutThroughMinimalPacketSizeSet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChCutThroughMinimalPacketSizeGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChCutThroughMemoryRateLimitSet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChCutThroughMemoryRateLimitGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChCutThroughBypassRouterAndPolicerEnableSet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChCutThroughBypassRouterAndPolicerEnableGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChCutThroughBypassModeSet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChCutThroughBypassModeGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChCutThroughPortGroupMaxBuffersLimitSet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChCutThroughPortGroupMaxBuffersLimitGet_E
};

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif /* __prvCpssDxChCutThroughLogh */
