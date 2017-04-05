/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* prvCpssDxChL2mllLog.h
*       WARNING!!! this is a generated file, please don't edit it manually
* COMMENTS:
*
* FILE REVISION NUMBER:
*       $Revision: 1 $
*******************************************************************************/
#ifndef __prvCpssDxChL2mllLogh
#define __prvCpssDxChL2mllLogh
#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
#include <cpss/generic/log/cpssLog.h>
#include <cpss/generic/log/prvCpssLog.h>


/********* manually implemented declarations *********/

void prvCpssLogParamFuncStc_CPSS_DXCH_L2MLL_COUNTER_SET_INTERFACE_CFG_STC_PTR
(
    IN CPSS_LOG_LIB_ENT         contextLib,
    IN CPSS_LOG_TYPE_ENT        logType,
    IN GT_CHAR_PTR              namePtr,
    IN void                   * fieldPtr,
    INOUT PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC     * inOutParamInfoPtr
);


/********* enums *********/

PRV_CPSS_LOG_STC_ENUM_DECLARE_MAC(CPSS_DXCH_L2MLL_PORT_TRUNK_CNT_MODE_ENT);
PRV_CPSS_LOG_STC_ENUM_DECLARE_MAC(CPSS_DXCH_L2MLL_VLAN_CNT_MODE_ENT);
PRV_CPSS_LOG_STC_ENUM_DECLARE_MAC(CPSS_DXCH_L2_MLL_PAIR_ENTRY_SELECTOR_ENT);


/********* structure fields log functions *********/

void prvCpssLogParamFuncStc_CPSS_DXCH_L2_MLL_EXCEPTION_COUNTERS_STC_PTR
(
    IN CPSS_LOG_LIB_ENT         contextLib,
    IN CPSS_LOG_TYPE_ENT        logType,
    IN GT_CHAR_PTR              namePtr,
    IN void                   * fieldPtr,
    INOUT PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC     * inOutParamInfoPtr
);
void prvCpssLogParamFuncStc_CPSS_DXCH_L2_MLL_LTT_ENTRY_STC_PTR
(
    IN CPSS_LOG_LIB_ENT         contextLib,
    IN CPSS_LOG_TYPE_ENT        logType,
    IN GT_CHAR_PTR              namePtr,
    IN void                   * fieldPtr,
    INOUT PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC     * inOutParamInfoPtr
);
void prvCpssLogParamFuncStc_CPSS_DXCH_L2_MLL_PAIR_STC_PTR
(
    IN CPSS_LOG_LIB_ENT         contextLib,
    IN CPSS_LOG_TYPE_ENT        logType,
    IN GT_CHAR_PTR              namePtr,
    IN void                   * fieldPtr,
    INOUT PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC     * inOutParamInfoPtr
);
void prvCpssLogParamFuncStc_CPSS_DXCH_L2_MLL_ENTRY_STC_PTR
(
    IN CPSS_LOG_LIB_ENT         contextLib,
    IN CPSS_LOG_TYPE_ENT        logType,
    IN GT_CHAR_PTR              namePtr,
    IN void                   * fieldPtr,
    INOUT PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC     * inOutParamInfoPtr
);


/********* parameters log functions *********/

void prvCpssLogParamFunc_CPSS_DXCH_L2MLL_COUNTER_SET_INTERFACE_CFG_STC_PTR(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
);
void prvCpssLogParamFunc_CPSS_DXCH_L2_MLL_EXCEPTION_COUNTERS_STC_PTR(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
);
void prvCpssLogParamFunc_CPSS_DXCH_L2_MLL_LTT_ENTRY_STC_PTR(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
);
void prvCpssLogParamFunc_CPSS_DXCH_L2_MLL_PAIR_STC_PTR(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
);


/********* API fields DB *********/

extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_DXCH_L2MLL_COUNTER_SET_INTERFACE_CFG_STC_PTR_interfaceCfgPtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_DXCH_L2_MLL_LTT_ENTRY_STC_PTR_lttEntryPtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_DXCH_L2_MLL_PAIR_STC_PTR_mllPairEntryPtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_BOOL_trapEnable;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_U32_maxVidxIndex;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_U32_mllPointer;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_U32_portBase;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_U32_vidxBase;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_U8_targetDevNum;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_U8_targetPortNum;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_CPSS_DXCH_L2MLL_COUNTER_SET_INTERFACE_CFG_STC_PTR_interfaceCfgPtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_CPSS_DXCH_L2_MLL_EXCEPTION_COUNTERS_STC_PTR_countersPtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_CPSS_DXCH_L2_MLL_LTT_ENTRY_STC_PTR_lttEntryPtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_CPSS_DXCH_L2_MLL_PAIR_STC_PTR_mllPairEntryPtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_BOOL_PTR_trapEnablePtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_U32_PTR_maxVidxIndexPtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_U32_PTR_mllPointer;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_U32_PTR_portBasePtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_U32_PTR_vidxBasePtr;


/********* lib API DB *********/

enum {
    PRV_CPSS_LOG_FUNC_cpssDxChL2MllLookupForAllEvidxEnableSet_E = (CPSS_LOG_LIB_L2_MLL_E << 16),
    PRV_CPSS_LOG_FUNC_cpssDxChL2MllLookupForAllEvidxEnableGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChL2MllLookupMaxVidxIndexSet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChL2MllLookupMaxVidxIndexGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChL2MllTtlExceptionConfigurationSet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChL2MllTtlExceptionConfigurationGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChL2MllExceptionCountersGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChL2MllPortGroupExceptionCountersGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChL2MllCounterGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChL2MllPortGroupCounterGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChL2MllSetCntInterfaceCfg_E,
    PRV_CPSS_LOG_FUNC_cpssDxChL2MllGetCntInterfaceCfg_E,
    PRV_CPSS_LOG_FUNC_cpssDxChL2MllPortGroupMcCntGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChL2MllMcCntGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChL2MllSilentDropCntGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChL2MllPortGroupSilentDropCntGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChL2MllLttEntrySet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChL2MllLttEntryGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChL2MllPairWrite_E,
    PRV_CPSS_LOG_FUNC_cpssDxChL2MllPairRead_E,
    PRV_CPSS_LOG_FUNC_cpssDxChL2MllMultiTargetPortEnableSet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChL2MllMultiTargetPortEnableGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChL2MllMultiTargetPortSet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChL2MllMultiTargetPortGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChL2MllMultiTargetPortBaseSet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChL2MllMultiTargetPortBaseGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChL2MllPortToVidxBaseSet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChL2MllPortToVidxBaseGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChL2MllSourceBasedFilteringConfigurationSet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChL2MllSourceBasedFilteringConfigurationGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChL2MLLVirtualPortToMllMappingTableEntrySet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChL2MLLVirtualPortToMllMappingTableEntryGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChL2MllVidxEnableSet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChL2MllVidxEnableGet_E
};

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif /* __prvCpssDxChL2mllLogh */
