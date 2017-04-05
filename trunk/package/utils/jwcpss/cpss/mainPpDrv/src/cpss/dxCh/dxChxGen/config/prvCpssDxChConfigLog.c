/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* prvCpssDxChConfigLog.c
*       WARNING!!! this is a generated file, please don't edit it manually
* COMMENTS:
*
* FILE REVISION NUMBER:
*       $Revision: 1 $
*******************************************************************************/
#include <cpss/generic/log/cpssLog.h>
#include <cpss/generic/log/prvCpssLog.h>
#include <cpss/extServices/private/prvCpssBindFunc.h>
#include <cpss/dxCh/dxChxGen/config/cpssDxChCfgInit.h>
#include <cpss/dxCh/dxChxGen/config/private/prvCpssDxChConfigLog.h>
#include <cpss/generic/config/private/prvCpssGenConfigLog.h>
#include <cpss/generic/extMac/private/prvCpssGenExtMacLog.h>
#include <cpss/generic/log/prvCpssGenLog.h>


/********* enums *********/

char * prvCpssLogEnum_CPSS_DXCH_CFG_EGRESS_HASH_SELECTION_FROM_INGRESS_MODE_ENT[]  =
{
    "CPSS_DXCH_CFG_EGRESS_HASH_SELECTION_FROM_INGRESS_MODE_6_LSB_E",
    "CPSS_DXCH_CFG_EGRESS_HASH_SELECTION_FROM_INGRESS_MODE_6_MSB_E",
    "CPSS_DXCH_CFG_EGRESS_HASH_SELECTION_FROM_INGRESS_MODE_12_BITS_E"
};
PRV_CPSS_LOG_STC_ENUM_ARRAY_SIZE_MAC(CPSS_DXCH_CFG_EGRESS_HASH_SELECTION_FROM_INGRESS_MODE_ENT);
char * prvCpssLogEnum_CPSS_DXCH_CFG_INGRESS_DROP_COUNTER_MODE_ENT[]  =
{
    "CPSS_DXCH_CFG_INGRESS_DROP_COUNTER_MODE_ALL_E",
    "CPSS_DXCH_CFG_INGRESS_DROP_COUNTER_MODE_VLAN_E",
    "CPSS_DXCH_CFG_INGRESS_DROP_COUNTER_MODE_PORT_E"
};
PRV_CPSS_LOG_STC_ENUM_ARRAY_SIZE_MAC(CPSS_DXCH_CFG_INGRESS_DROP_COUNTER_MODE_ENT);
char * prvCpssLogEnum_CPSS_DXCH_CFG_TABLES_ENT[]  =
{
    "CPSS_DXCH_CFG_TABLE_VLAN_E",
    "CPSS_DXCH_CFG_TABLE_FDB_E",
    "CPSS_DXCH_CFG_TABLE_PCL_ACTION_E",
    "CPSS_DXCH_CFG_TABLE_PCL_TCAM_E",
    "CPSS_DXCH_CFG_TABLE_ROUTER_NEXT_HOP_E",
    "CPSS_DXCH_CFG_TABLE_ROUTER_LTT_E",
    "CPSS_DXCH_CFG_TABLE_ROUTER_TCAM_E",
    "CPSS_DXCH_CFG_TABLE_ROUTER_ECMP_QOS_E",
    "CPSS_DXCH_CFG_TABLE_TTI_TCAM_E",
    "CPSS_DXCH_CFG_TABLE_MLL_PAIR_E",
    "CPSS_DXCH_CFG_TABLE_POLICER_METERS_E",
    "CPSS_DXCH_CFG_TABLE_POLICER_BILLING_COUNTERS_E",
    "CPSS_DXCH_CFG_TABLE_VIDX_E",
    "CPSS_DXCH_CFG_TABLE_ARP_E",
    "CPSS_DXCH_CFG_TABLE_TUNNEL_START_E",
    "CPSS_DXCH_CFG_TABLE_STG_E",
    "CPSS_DXCH_CFG_TABLE_QOS_PROFILE_E",
    "CPSS_DXCH_CFG_TABLE_MAC_TO_ME_E",
    "CPSS_DXCH_CFG_TABLE_CNC_E",
    "CPSS_DXCH_CFG_TABLE_CNC_BLOCK_E",
    "CPSS_DXCH_CFG_TABLE_TRUNK_E",
    "CPSS_DXCH_CFG_TABLE_LPM_RAM_E",
    "CPSS_DXCH_CFG_TABLE_ROUTER_ECMP_E",
    "CPSS_DXCH_CFG_TABLE_L2_MLL_LTT_E",
    "CPSS_DXCH_CFG_TABLE_EPORT_E",
    "CPSS_DXCH_CFG_TABLE_DEFAULT_EPORT_E",
    "CPSS_DXCH_CFG_TABLE_PHYSICAL_PORT_E",
    "CPSS_DXCH_CFG_TABLE_LAST_E"
};
PRV_CPSS_LOG_STC_ENUM_ARRAY_SIZE_MAC(CPSS_DXCH_CFG_TABLES_ENT);
char * prvCpssLogEnum_CPSS_DXCH_CFG_ROUTING_MODE_ENT[]  =
{
    "CPSS_DXCH_POLICY_BASED_ROUTING_ONLY_E",
    "CPSS_DXCH_TCAM_ROUTER_BASED_E"
};
PRV_CPSS_LOG_STC_ENUM_ARRAY_SIZE_MAC(CPSS_DXCH_CFG_ROUTING_MODE_ENT);


/********* structure fields log functions *********/

void prvCpssLogParamFuncStc_CPSS_DXCH_CFG_DEV_INFO_STC_PTR
(
    IN CPSS_LOG_LIB_ENT         contextLib,
    IN CPSS_LOG_TYPE_ENT        logType,
    IN GT_CHAR_PTR              namePtr,
    IN void                   * fieldPtr,
    INOUT PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC     * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_START_FIELD_STC_MAC(CPSS_DXCH_CFG_DEV_INFO_STC *, valPtr);
    PRV_CPSS_LOG_UNUSED_MAC(inOutParamInfoPtr);
    PRV_CPSS_LOG_STC_STC_MAC(valPtr, genDevInfo, CPSS_GEN_CFG_DEV_INFO_STC);
    prvCpssLogStcLogEnd(contextLib, logType);
}
void prvCpssLogParamFuncStc_CPSS_DXCH_CFG_GLOBAL_EPORT_STC_PTR
(
    IN CPSS_LOG_LIB_ENT         contextLib,
    IN CPSS_LOG_TYPE_ENT        logType,
    IN GT_CHAR_PTR              namePtr,
    IN void                   * fieldPtr,
    INOUT PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC     * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_START_FIELD_STC_MAC(CPSS_DXCH_CFG_GLOBAL_EPORT_STC *, valPtr);
    PRV_CPSS_LOG_UNUSED_MAC(inOutParamInfoPtr);
    PRV_CPSS_LOG_STC_BOOL_MAC(valPtr, enable);
    PRV_CPSS_LOG_STC_NUMBER_MAC(valPtr, pattern);
    PRV_CPSS_LOG_STC_NUMBER_MAC(valPtr, mask);
    prvCpssLogStcLogEnd(contextLib, logType);
}
void prvCpssLogParamFuncStc_CPSS_DXCH_CFG_HW_ACCESS_OBJ_STC_PTR
(
    IN CPSS_LOG_LIB_ENT         contextLib,
    IN CPSS_LOG_TYPE_ENT        logType,
    IN GT_CHAR_PTR              namePtr,
    IN void                   * fieldPtr,
    INOUT PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC     * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_START_FIELD_STC_MAC(CPSS_DXCH_CFG_HW_ACCESS_OBJ_STC *, valPtr);
    PRV_CPSS_LOG_UNUSED_MAC(inOutParamInfoPtr);
    PRV_CPSS_LOG_STC_PTR_MAC(valPtr, hwAccessTableEntryReadFunc);
    PRV_CPSS_LOG_STC_PTR_MAC(valPtr, hwAccessTableEntryWriteFunc);
    PRV_CPSS_LOG_STC_PTR_MAC(valPtr, hwAccessTableEntryFieldReadFunc);
    PRV_CPSS_LOG_STC_PTR_MAC(valPtr, hwAccessTableEntryFieldWriteFunc);
    prvCpssLogStcLogEnd(contextLib, logType);
}
void prvCpssLogParamFuncStc_CPSS_DXCH_PP_CONFIG_INIT_STC_PTR
(
    IN CPSS_LOG_LIB_ENT         contextLib,
    IN CPSS_LOG_TYPE_ENT        logType,
    IN GT_CHAR_PTR              namePtr,
    IN void                   * fieldPtr,
    INOUT PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC     * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_START_FIELD_STC_MAC(CPSS_DXCH_PP_CONFIG_INIT_STC *, valPtr);
    PRV_CPSS_LOG_UNUSED_MAC(inOutParamInfoPtr);
    PRV_CPSS_LOG_STC_ENUM_MAC(valPtr, routingMode, CPSS_DXCH_CFG_ROUTING_MODE_ENT);
    PRV_CPSS_LOG_STC_NUMBER_MAC(valPtr, maxNumOfPbrEntries);
    prvCpssLogStcLogEnd(contextLib, logType);
}


/********* parameters log functions *********/

void prvCpssLogParamFunc_CPSS_DXCH_CFG_DEV_INFO_STC_PTR(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_START_PARAM_STC_MAC(CPSS_DXCH_CFG_DEV_INFO_STC*, paramVal);
    prvCpssLogParamFuncStc_CPSS_DXCH_CFG_DEV_INFO_STC_PTR(contextLib, logType, namePtr, paramVal, inOutParamInfoPtr);
}
void prvCpssLogParamFunc_CPSS_DXCH_CFG_EGRESS_HASH_SELECTION_FROM_INGRESS_MODE_ENT(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_SET_PARAM_VAL_MAC(CPSS_DXCH_CFG_EGRESS_HASH_SELECTION_FROM_INGRESS_MODE_ENT, paramVal);
    PRV_CPSS_LOG_ENUM_MAC(namePtr, paramVal, CPSS_DXCH_CFG_EGRESS_HASH_SELECTION_FROM_INGRESS_MODE_ENT);
}
void prvCpssLogParamFunc_CPSS_DXCH_CFG_EGRESS_HASH_SELECTION_FROM_INGRESS_MODE_ENT_PTR(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_SET_PARAM_VAL_MAC(CPSS_DXCH_CFG_EGRESS_HASH_SELECTION_FROM_INGRESS_MODE_ENT*, paramVal);
    if (paramVal == NULL)
    {
        PRV_CPSS_LOG_AND_HISTORY_PARAM_MAC(contextLib, logType, "%s = NULL\n", namePtr);
        return;
    }
    PRV_CPSS_LOG_ENUM_MAC(namePtr, *paramVal, CPSS_DXCH_CFG_EGRESS_HASH_SELECTION_FROM_INGRESS_MODE_ENT);
}
void prvCpssLogParamFunc_CPSS_DXCH_CFG_GLOBAL_EPORT_STC_PTR(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_START_PARAM_STC_MAC(CPSS_DXCH_CFG_GLOBAL_EPORT_STC*, paramVal);
    prvCpssLogParamFuncStc_CPSS_DXCH_CFG_GLOBAL_EPORT_STC_PTR(contextLib, logType, namePtr, paramVal, inOutParamInfoPtr);
}
void prvCpssLogParamFunc_CPSS_DXCH_CFG_HW_ACCESS_OBJ_STC_PTR(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_START_PARAM_STC_MAC(CPSS_DXCH_CFG_HW_ACCESS_OBJ_STC*, paramVal);
    prvCpssLogParamFuncStc_CPSS_DXCH_CFG_HW_ACCESS_OBJ_STC_PTR(contextLib, logType, namePtr, paramVal, inOutParamInfoPtr);
}
void prvCpssLogParamFunc_CPSS_DXCH_CFG_INGRESS_DROP_COUNTER_MODE_ENT(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_SET_PARAM_VAL_MAC(CPSS_DXCH_CFG_INGRESS_DROP_COUNTER_MODE_ENT, paramVal);
    PRV_CPSS_LOG_ENUM_MAC(namePtr, paramVal, CPSS_DXCH_CFG_INGRESS_DROP_COUNTER_MODE_ENT);
}
void prvCpssLogParamFunc_CPSS_DXCH_CFG_INGRESS_DROP_COUNTER_MODE_ENT_PTR(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_SET_PARAM_VAL_MAC(CPSS_DXCH_CFG_INGRESS_DROP_COUNTER_MODE_ENT*, paramVal);
    if (paramVal == NULL)
    {
        PRV_CPSS_LOG_AND_HISTORY_PARAM_MAC(contextLib, logType, "%s = NULL\n", namePtr);
        return;
    }
    PRV_CPSS_LOG_ENUM_MAC(namePtr, *paramVal, CPSS_DXCH_CFG_INGRESS_DROP_COUNTER_MODE_ENT);
}
void prvCpssLogParamFunc_CPSS_DXCH_CFG_TABLES_ENT(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_SET_PARAM_VAL_MAC(CPSS_DXCH_CFG_TABLES_ENT, paramVal);
    PRV_CPSS_LOG_ENUM_MAC(namePtr, paramVal, CPSS_DXCH_CFG_TABLES_ENT);
}
void prvCpssLogParamFunc_CPSS_DXCH_PP_CONFIG_INIT_STC_PTR(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_START_PARAM_STC_MAC(CPSS_DXCH_PP_CONFIG_INIT_STC*, paramVal);
    prvCpssLogParamFuncStc_CPSS_DXCH_PP_CONFIG_INIT_STC_PTR(contextLib, logType, namePtr, paramVal, inOutParamInfoPtr);
}


/********* API fields DB *********/

PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_DXCH_CFG_EGRESS_HASH_SELECTION_FROM_INGRESS_MODE_ENT_selectionMode = {
     "selectionMode", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_MAC(CPSS_DXCH_CFG_EGRESS_HASH_SELECTION_FROM_INGRESS_MODE_ENT)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_DXCH_CFG_GLOBAL_EPORT_STC_PTR_globalPtr = {
     "globalPtr", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(CPSS_DXCH_CFG_GLOBAL_EPORT_STC)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_DXCH_CFG_GLOBAL_EPORT_STC_PTR_l2EcmpPtr = {
     "l2EcmpPtr", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(CPSS_DXCH_CFG_GLOBAL_EPORT_STC)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_DXCH_CFG_HW_ACCESS_OBJ_STC_PTR_cfgAccessObjPtr = {
     "cfgAccessObjPtr", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(CPSS_DXCH_CFG_HW_ACCESS_OBJ_STC)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_DXCH_CFG_INGRESS_DROP_COUNTER_MODE_ENT_mode = {
     "mode", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_MAC(CPSS_DXCH_CFG_INGRESS_DROP_COUNTER_MODE_ENT)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_DXCH_CFG_TABLES_ENT_table = {
     "table", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_MAC(CPSS_DXCH_CFG_TABLES_ENT)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_DXCH_PP_CONFIG_INIT_STC_PTR_ppConfigPtr = {
     "ppConfigPtr", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(CPSS_DXCH_PP_CONFIG_INIT_STC)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_MACDRV_OBJ_STC_PTR_macDrvObjPtr = {
     "macDrvObjPtr", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(CPSS_MACDRV_OBJ_STC)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_BOOL_PTR_modifedDsaSrcDevPtr = {
     "modifedDsaSrcDevPtr", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(GT_BOOL)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_BOOL_modifedDsaSrcDev = {
     "modifedDsaSrcDev", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_MAC(GT_BOOL)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_BOOL_modifyEnable = {
     "modifyEnable", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_MAC(GT_BOOL)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_PORT_NUM_ePort = {
     "ePort", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_MAC(GT_PORT_NUM)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_U8_newDevNum = {
     "newDevNum", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_MAC(GT_U8)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_U8_oldDevNum = {
     "oldDevNum", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_MAC(GT_U8)
};
PRV_CPSS_LOG_FUNC_PARAM_STC OUT_CPSS_DXCH_CFG_DEV_INFO_STC_PTR_devInfoPtr = {
     "devInfoPtr", PRV_CPSS_LOG_PARAM_OUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(CPSS_DXCH_CFG_DEV_INFO_STC)
};
PRV_CPSS_LOG_FUNC_PARAM_STC OUT_CPSS_DXCH_CFG_EGRESS_HASH_SELECTION_FROM_INGRESS_MODE_ENT_PTR_selectionModePtr = {
     "selectionModePtr", PRV_CPSS_LOG_PARAM_OUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(CPSS_DXCH_CFG_EGRESS_HASH_SELECTION_FROM_INGRESS_MODE_ENT)
};
PRV_CPSS_LOG_FUNC_PARAM_STC OUT_CPSS_DXCH_CFG_GLOBAL_EPORT_STC_PTR_globalPtr = {
     "globalPtr", PRV_CPSS_LOG_PARAM_OUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(CPSS_DXCH_CFG_GLOBAL_EPORT_STC)
};
PRV_CPSS_LOG_FUNC_PARAM_STC OUT_CPSS_DXCH_CFG_GLOBAL_EPORT_STC_PTR_l2EcmpPtr = {
     "l2EcmpPtr", PRV_CPSS_LOG_PARAM_OUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(CPSS_DXCH_CFG_GLOBAL_EPORT_STC)
};
PRV_CPSS_LOG_FUNC_PARAM_STC OUT_CPSS_DXCH_CFG_INGRESS_DROP_COUNTER_MODE_ENT_PTR_modePtr = {
     "modePtr", PRV_CPSS_LOG_PARAM_OUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(CPSS_DXCH_CFG_INGRESS_DROP_COUNTER_MODE_ENT)
};
PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_BOOL_PTR_modifyEnablePtr = {
     "modifyEnablePtr", PRV_CPSS_LOG_PARAM_OUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(GT_BOOL)
};
PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_PORT_NUM_PTR_ePortPtr = {
     "ePortPtr", PRV_CPSS_LOG_PARAM_OUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(GT_PORT_NUM)
};
PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_PORT_NUM_PTR_portNumPtr = {
     "portNumPtr", PRV_CPSS_LOG_PARAM_OUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(GT_PORT_NUM)
};
PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_U32_PTR_numEntriesPtr = {
     "numEntriesPtr", PRV_CPSS_LOG_PARAM_OUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(GT_U32)
};
PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_U8_PTR_nextDevNumPtr = {
     "nextDevNumPtr", PRV_CPSS_LOG_PARAM_OUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(GT_U8)
};


/********* API prototypes DB *********/

PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChCfgHwAccessObjectBind_PARAMS[] =  {
    &IN_CPSS_DXCH_CFG_HW_ACCESS_OBJ_STC_PTR_cfgAccessObjPtr,
    &IN_GT_BOOL_bind
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChCfgEgressHashSelectionModeSet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_CPSS_DXCH_CFG_EGRESS_HASH_SELECTION_FROM_INGRESS_MODE_ENT_selectionMode
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChCfgGlobalEportSet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_CPSS_DXCH_CFG_GLOBAL_EPORT_STC_PTR_globalPtr,
    &IN_CPSS_DXCH_CFG_GLOBAL_EPORT_STC_PTR_l2EcmpPtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChCfgIngressDropCntrModeSet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_CPSS_DXCH_CFG_INGRESS_DROP_COUNTER_MODE_ENT_mode,
    &IN_GT_PORT_NUM_portNum,
    &IN_GT_U16_vlan
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChCfgTableNumEntriesGet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_CPSS_DXCH_CFG_TABLES_ENT_table,
    &OUT_GT_U32_PTR_numEntriesPtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChCfgPpLogicalInit_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_CPSS_DXCH_PP_CONFIG_INIT_STC_PTR_ppConfigPtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChCfgDsaTagSrcDevPortModifyGet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_BOOL_PTR_modifedDsaSrcDevPtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChCfgDsaTagSrcDevPortModifySet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_BOOL_modifedDsaSrcDev
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChCfgBindPortPhymacObject_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_PHYSICAL_PORT_NUM_portNum,
    &IN_CPSS_MACDRV_OBJ_STC_PTR_macDrvObjPtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChCfgDsaTagSrcDevPortRoutedPcktModifySet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_PHYSICAL_PORT_NUM_portNum,
    &IN_GT_BOOL_modifyEnable
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChCfgPortDefaultSourceEportNumberSet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_PHYSICAL_PORT_NUM_portNum,
    &IN_GT_PORT_NUM_ePort
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChCfgDsaTagSrcDevPortRoutedPcktModifyGet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_PHYSICAL_PORT_NUM_portNum,
    &OUT_GT_BOOL_PTR_modifyEnablePtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChCfgPortDefaultSourceEportNumberGet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_PHYSICAL_PORT_NUM_portNum,
    &OUT_GT_PORT_NUM_PTR_ePortPtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChCfgDevInfoGet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &OUT_CPSS_DXCH_CFG_DEV_INFO_STC_PTR_devInfoPtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChCfgEgressHashSelectionModeGet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &OUT_CPSS_DXCH_CFG_EGRESS_HASH_SELECTION_FROM_INGRESS_MODE_ENT_PTR_selectionModePtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChCfgGlobalEportGet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &OUT_CPSS_DXCH_CFG_GLOBAL_EPORT_STC_PTR_globalPtr,
    &OUT_CPSS_DXCH_CFG_GLOBAL_EPORT_STC_PTR_l2EcmpPtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChCfgIngressDropCntrModeGet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &OUT_CPSS_DXCH_CFG_INGRESS_DROP_COUNTER_MODE_ENT_PTR_modePtr,
    &OUT_GT_PORT_NUM_PTR_portNumPtr,
    &OUT_GT_U16_PTR_vlanPtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChCfgNextDevGet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &OUT_GT_U8_PTR_nextDevNumPtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChCfgReNumberDevNum_PARAMS[] =  {
    &IN_GT_U8_oldDevNum,
    &IN_GT_U8_newDevNum
};


/********* lib API DB *********/

static PRV_CPSS_LOG_FUNC_ENTRY_STC prvCpssDxChConfigLogLibDb[] = {
    {"cpssDxChCfgDevRemove", 1, prvCpssLogGenDevNum_PARAMS, NULL},
    {"cpssDxChCfgDevEnable", 2, prvCpssLogGenDevNumEnable_PARAMS, NULL},
    {"cpssDxChCfgDevEnableGet", 2, prvCpssLogGenDevNumEnablePtr_PARAMS, NULL},
    {"cpssDxChCfgNextDevGet", 2, cpssDxChCfgNextDevGet_PARAMS, NULL},
    {"cpssDxChCfgPpLogicalInit", 2, cpssDxChCfgPpLogicalInit_PARAMS, NULL},
    {"cpssDxChCfgDsaTagSrcDevPortRoutedPcktModifySet", 3, cpssDxChCfgDsaTagSrcDevPortRoutedPcktModifySet_PARAMS, NULL},
    {"cpssDxChCfgDsaTagSrcDevPortRoutedPcktModifyGet", 3, cpssDxChCfgDsaTagSrcDevPortRoutedPcktModifyGet_PARAMS, NULL},
    {"cpssDxChCfgDsaTagSrcDevPortModifySet", 2, cpssDxChCfgDsaTagSrcDevPortModifySet_PARAMS, NULL},
    {"cpssDxChCfgDsaTagSrcDevPortModifyGet", 2, cpssDxChCfgDsaTagSrcDevPortModifyGet_PARAMS, NULL},
    {"cpssDxChCfgTableNumEntriesGet", 3, cpssDxChCfgTableNumEntriesGet_PARAMS, NULL},
    {"cpssDxChCfgReNumberDevNum", 2, cpssDxChCfgReNumberDevNum_PARAMS, NULL},
    {"cpssDxChCfgDevInfoGet", 2, cpssDxChCfgDevInfoGet_PARAMS, NULL},
    {"cpssDxChCfgBindPortPhymacObject", 3, cpssDxChCfgBindPortPhymacObject_PARAMS, NULL},
    {"cpssDxChCfgPortDefaultSourceEportNumberSet", 3, cpssDxChCfgPortDefaultSourceEportNumberSet_PARAMS, NULL},
    {"cpssDxChCfgPortDefaultSourceEportNumberGet", 3, cpssDxChCfgPortDefaultSourceEportNumberGet_PARAMS, NULL},
    {"cpssDxChCfgEgressHashSelectionModeSet", 2, cpssDxChCfgEgressHashSelectionModeSet_PARAMS, NULL},
    {"cpssDxChCfgEgressHashSelectionModeGet", 2, cpssDxChCfgEgressHashSelectionModeGet_PARAMS, NULL},
    {"cpssDxChCfgIngressDropCntrModeSet", 4, cpssDxChCfgIngressDropCntrModeSet_PARAMS, NULL},
    {"cpssDxChCfgIngressDropCntrModeGet", 4, cpssDxChCfgIngressDropCntrModeGet_PARAMS, NULL},
    {"cpssDxChCfgIngressDropCntrSet", 2, prvCpssLogGenDevNumCounter_PARAMS, NULL},
    {"cpssDxChCfgIngressDropCntrGet", 2, prvCpssLogGenDevNumCounterPtr_PARAMS, NULL},
    {"cpssDxChCfgGlobalEportSet", 3, cpssDxChCfgGlobalEportSet_PARAMS, NULL},
    {"cpssDxChCfgGlobalEportGet", 3, cpssDxChCfgGlobalEportGet_PARAMS, NULL},
    {"cpssDxChCfgHwAccessObjectBind", 2, cpssDxChCfgHwAccessObjectBind_PARAMS, NULL},
    {"cpssDxChCfgHitlessWriteMethodEnableSet", 2, prvCpssLogGenDevNumEnable_PARAMS, NULL},
    {"cpssPpCfgNextDevGet", 2, cpssDxChCfgNextDevGet_PARAMS, NULL},
};

/******** DB Access Function ********/
void prvCpssLogParamLibDbGet_CPSS_LOG_LIB_CONFIG(
    OUT PRV_CPSS_LOG_FUNC_ENTRY_STC ** logFuncDbPtrPtr,
    OUT GT_U32 * logFuncDbSizePtr
)
{
    *logFuncDbPtrPtr = prvCpssDxChConfigLogLibDb;
    *logFuncDbSizePtr = sizeof(prvCpssDxChConfigLogLibDb) / sizeof(PRV_CPSS_LOG_FUNC_ENTRY_STC);
}

