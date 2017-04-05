/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* prvCpssDxChMultiPortGroupLog.h
*       WARNING!!! this is a generated file, please don't edit it manually
* COMMENTS:
*
* FILE REVISION NUMBER:
*       $Revision: 1 $
*******************************************************************************/
#ifndef __prvCpssDxChMultiPortGroupLogh
#define __prvCpssDxChMultiPortGroupLogh
#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
#include <cpss/generic/log/cpssLog.h>
#include <cpss/generic/log/prvCpssLog.h>


/********* manually implemented declarations *********/

void prvCpssLogParamFunc_CPSS_DXCH_MULTI_PORT_GROUP_RING_INFO_STC_PTR(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
);
void prvCpssLogParamFuncStc_CPSS_DXCH_MULTI_PORT_GROUP_RING_INFO_STC_PTR
(
    IN CPSS_LOG_LIB_ENT         contextLib,
    IN CPSS_LOG_TYPE_ENT        logType,
    IN GT_CHAR_PTR              namePtr,
    IN void                   * fieldPtr,
    INOUT PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC     * inOutParamInfoPtr
);


/********* enums *********/

PRV_CPSS_LOG_STC_ENUM_DECLARE_MAC(CPSS_DXCH_MULTI_PORT_GROUP_FDB_MODE_ENT);
PRV_CPSS_LOG_STC_ENUM_DECLARE_MAC(CPSS_DXCH_MULTI_PORT_GROUP_LOOKUP_NOT_FOUND_ENT);


/********* structure fields log functions *********/

void prvCpssLogParamFuncStc_CPSS_DXCH_MULTI_PORT_GROUP_LOOKUP_NOT_FOUND_STC_PTR
(
    IN CPSS_LOG_LIB_ENT         contextLib,
    IN CPSS_LOG_TYPE_ENT        logType,
    IN GT_CHAR_PTR              namePtr,
    IN void                   * fieldPtr,
    INOUT PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC     * inOutParamInfoPtr
);


/********* parameters log functions *********/

void prvCpssLogParamFunc_CPSS_DXCH_MULTI_PORT_GROUP_FDB_MODE_ENT(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
);
void prvCpssLogParamFunc_CPSS_DXCH_MULTI_PORT_GROUP_FDB_MODE_ENT_PTR(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
);
void prvCpssLogParamFunc_CPSS_DXCH_MULTI_PORT_GROUP_LOOKUP_NOT_FOUND_STC_PTR(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
);


/********* API fields DB *********/

extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_DXCH_MULTI_PORT_GROUP_FDB_MODE_ENT_fdbMode;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_DXCH_MULTI_PORT_GROUP_LOOKUP_NOT_FOUND_STC_PTR_infoPtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_DXCH_MULTI_PORT_GROUP_RING_INFO_STC_PTR_ringInfoArr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_DXCH_MULTI_PORT_GROUP_SRC_HASH_MODE_ENT_mode;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_DXCH_MULTI_PORT_GROUP_SRC_PORT_HASH_PAIR_STC_PTR_portsArr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_PORT_NUM_PTR_uplinkPortsArr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_U32_numOfPorts;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_U32_numOfUplinkPorts;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_U32_ringInfoNum;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_U32_sourcePortGroupId;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_CPSS_DXCH_MULTI_PORT_GROUP_FDB_MODE_ENT_PTR_fdbModePtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_CPSS_DXCH_MULTI_PORT_GROUP_LOOKUP_NOT_FOUND_STC_PTR_infoPtr;


/********* lib API DB *********/

enum {
    PRV_CPSS_LOG_FUNC_cpssDxChMultiPortGroupConfigSet_E = (CPSS_LOG_LIB_MULTI_PORT_GROUP_E << 16),
    PRV_CPSS_LOG_FUNC_cpssDxChMultiPortGroupTrunkSrcPortHashMappingSet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChMultiPortGroupPortLookupEnableSet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChMultiPortGroupPortLookupEnableGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChMultiPortGroupPortRingEnableSet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChMultiPortGroupPortRingEnableGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChMultiPortGroupLookupNotFoundTableSet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChMultiPortGroupLookupNotFoundTableGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChMultiPortGroupBridgeUnknownDaMaskEnableSet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChMultiPortGroupBridgeUnknownDaMaskEnableGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChMultiPortGroupFdbModeSet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChMultiPortGroupFdbModeGet_E
};

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif /* __prvCpssDxChMultiPortGroupLogh */
