/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* prvCpssDxChConfigLog.h
*       WARNING!!! this is a generated file, please don't edit it manually
* COMMENTS:
*
* FILE REVISION NUMBER:
*       $Revision: 1 $
*******************************************************************************/
#ifndef __prvCpssDxChConfigLogh
#define __prvCpssDxChConfigLogh
#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
#include <cpss/generic/log/cpssLog.h>
#include <cpss/generic/log/prvCpssLog.h>


/********* enums *********/

PRV_CPSS_LOG_STC_ENUM_DECLARE_MAC(CPSS_DXCH_CFG_EGRESS_HASH_SELECTION_FROM_INGRESS_MODE_ENT);
PRV_CPSS_LOG_STC_ENUM_DECLARE_MAC(CPSS_DXCH_CFG_INGRESS_DROP_COUNTER_MODE_ENT);
PRV_CPSS_LOG_STC_ENUM_DECLARE_MAC(CPSS_DXCH_CFG_TABLES_ENT);
PRV_CPSS_LOG_STC_ENUM_DECLARE_MAC(CPSS_DXCH_CFG_ROUTING_MODE_ENT);


/********* structure fields log functions *********/

void prvCpssLogParamFuncStc_CPSS_DXCH_CFG_DEV_INFO_STC_PTR
(
    IN CPSS_LOG_LIB_ENT         contextLib,
    IN CPSS_LOG_TYPE_ENT        logType,
    IN GT_CHAR_PTR              namePtr,
    IN void                   * fieldPtr,
    INOUT PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC     * inOutParamInfoPtr
);
void prvCpssLogParamFuncStc_CPSS_DXCH_CFG_GLOBAL_EPORT_STC_PTR
(
    IN CPSS_LOG_LIB_ENT         contextLib,
    IN CPSS_LOG_TYPE_ENT        logType,
    IN GT_CHAR_PTR              namePtr,
    IN void                   * fieldPtr,
    INOUT PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC     * inOutParamInfoPtr
);
void prvCpssLogParamFuncStc_CPSS_DXCH_CFG_HW_ACCESS_OBJ_STC_PTR
(
    IN CPSS_LOG_LIB_ENT         contextLib,
    IN CPSS_LOG_TYPE_ENT        logType,
    IN GT_CHAR_PTR              namePtr,
    IN void                   * fieldPtr,
    INOUT PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC     * inOutParamInfoPtr
);
void prvCpssLogParamFuncStc_CPSS_DXCH_PP_CONFIG_INIT_STC_PTR
(
    IN CPSS_LOG_LIB_ENT         contextLib,
    IN CPSS_LOG_TYPE_ENT        logType,
    IN GT_CHAR_PTR              namePtr,
    IN void                   * fieldPtr,
    INOUT PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC     * inOutParamInfoPtr
);


/********* parameters log functions *********/

void prvCpssLogParamFunc_CPSS_DXCH_CFG_DEV_INFO_STC_PTR(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
);
void prvCpssLogParamFunc_CPSS_DXCH_CFG_EGRESS_HASH_SELECTION_FROM_INGRESS_MODE_ENT(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
);
void prvCpssLogParamFunc_CPSS_DXCH_CFG_EGRESS_HASH_SELECTION_FROM_INGRESS_MODE_ENT_PTR(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
);
void prvCpssLogParamFunc_CPSS_DXCH_CFG_GLOBAL_EPORT_STC_PTR(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
);
void prvCpssLogParamFunc_CPSS_DXCH_CFG_HW_ACCESS_OBJ_STC_PTR(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
);
void prvCpssLogParamFunc_CPSS_DXCH_CFG_INGRESS_DROP_COUNTER_MODE_ENT(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
);
void prvCpssLogParamFunc_CPSS_DXCH_CFG_INGRESS_DROP_COUNTER_MODE_ENT_PTR(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
);
void prvCpssLogParamFunc_CPSS_DXCH_CFG_TABLES_ENT(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
);
void prvCpssLogParamFunc_CPSS_DXCH_PP_CONFIG_INIT_STC_PTR(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
);


/********* API fields DB *********/

extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_DXCH_CFG_EGRESS_HASH_SELECTION_FROM_INGRESS_MODE_ENT_selectionMode;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_DXCH_CFG_GLOBAL_EPORT_STC_PTR_globalPtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_DXCH_CFG_GLOBAL_EPORT_STC_PTR_l2EcmpPtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_DXCH_CFG_HW_ACCESS_OBJ_STC_PTR_cfgAccessObjPtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_DXCH_CFG_INGRESS_DROP_COUNTER_MODE_ENT_mode;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_DXCH_CFG_TABLES_ENT_table;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_DXCH_PP_CONFIG_INIT_STC_PTR_ppConfigPtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_MACDRV_OBJ_STC_PTR_macDrvObjPtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_BOOL_PTR_modifedDsaSrcDevPtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_BOOL_modifedDsaSrcDev;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_BOOL_modifyEnable;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_PORT_NUM_ePort;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_U8_newDevNum;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_U8_oldDevNum;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_CPSS_DXCH_CFG_DEV_INFO_STC_PTR_devInfoPtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_CPSS_DXCH_CFG_EGRESS_HASH_SELECTION_FROM_INGRESS_MODE_ENT_PTR_selectionModePtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_CPSS_DXCH_CFG_GLOBAL_EPORT_STC_PTR_globalPtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_CPSS_DXCH_CFG_GLOBAL_EPORT_STC_PTR_l2EcmpPtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_CPSS_DXCH_CFG_INGRESS_DROP_COUNTER_MODE_ENT_PTR_modePtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_BOOL_PTR_modifyEnablePtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_PORT_NUM_PTR_ePortPtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_PORT_NUM_PTR_portNumPtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_U32_PTR_numEntriesPtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_U8_PTR_nextDevNumPtr;


/********* lib API DB *********/

enum {
    PRV_CPSS_LOG_FUNC_cpssDxChCfgDevRemove_E = (CPSS_LOG_LIB_CONFIG_E << 16),
    PRV_CPSS_LOG_FUNC_cpssDxChCfgDevEnable_E,
    PRV_CPSS_LOG_FUNC_cpssDxChCfgDevEnableGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChCfgNextDevGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChCfgPpLogicalInit_E,
    PRV_CPSS_LOG_FUNC_cpssDxChCfgDsaTagSrcDevPortRoutedPcktModifySet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChCfgDsaTagSrcDevPortRoutedPcktModifyGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChCfgDsaTagSrcDevPortModifySet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChCfgDsaTagSrcDevPortModifyGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChCfgTableNumEntriesGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChCfgReNumberDevNum_E,
    PRV_CPSS_LOG_FUNC_cpssDxChCfgDevInfoGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChCfgBindPortPhymacObject_E,
    PRV_CPSS_LOG_FUNC_cpssDxChCfgPortDefaultSourceEportNumberSet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChCfgPortDefaultSourceEportNumberGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChCfgEgressHashSelectionModeSet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChCfgEgressHashSelectionModeGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChCfgIngressDropCntrModeSet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChCfgIngressDropCntrModeGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChCfgIngressDropCntrSet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChCfgIngressDropCntrGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChCfgGlobalEportSet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChCfgGlobalEportGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChCfgHwAccessObjectBind_E,
    PRV_CPSS_LOG_FUNC_cpssDxChCfgHitlessWriteMethodEnableSet_E,
    PRV_CPSS_LOG_FUNC_cpssPpCfgNextDevGet_E
};

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif /* __prvCpssDxChConfigLogh */
