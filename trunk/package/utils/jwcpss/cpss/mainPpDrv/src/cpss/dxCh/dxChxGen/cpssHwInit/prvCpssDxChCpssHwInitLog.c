/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* prvCpssDxChCpssHwInitLog.c
*       WARNING!!! this is a generated file, please don't edit it manually
* COMMENTS:
*
* FILE REVISION NUMBER:
*       $Revision: 1 $
*******************************************************************************/
#include <cpss/generic/log/cpssLog.h>
#include <cpss/generic/log/prvCpssLog.h>
#include <cpss/extServices/private/prvCpssBindFunc.h>
#include <cpss/dxCh/dxChxGen/cpssHwInit/cpssDxChHwInit.h>
#include <cpss/dxCh/dxChxGen/cpssHwInit/cpssDxChHwInitLedCtrl.h>
#include <cpss/dxCh/dxChxGen/cpssHwInit/private/prvCpssDxChCpssHwInitLog.h>
#include <cpss/generic/cpssHwInit/private/prvCpssGenCpssHwInitLog.h>
#include <cpss/generic/log/prvCpssGenLog.h>


/********* enums *********/

char * prvCpssLogEnum_CPSS_DXCH_IMPLEMENT_WA_ENT[]  =
{
    "CPSS_DXCH_IMPLEMENT_WA_TRUNK_WRONG_SOURCE_PORT_TO_CPU_WA_E",
    "CPSS_DXCH_IMPLEMENT_WA_SDMA_PKTS_FROM_CPU_STACK_E",
    "CPSS_DXCH_IMPLEMENT_WA_FDB_AU_FIFO_E",
    "CPSS_DXCH_IMPLEMENT_WA_FDB_AU_FU_FROM_NON_SOURCE_PORT_GROUP_E",
    "CPSS_DXCH_IMPLEMENT_WA_SDMA_PKTS_FROM_CPU_STACK_PADDING_E",
    "CPSS_DXCH_IMPLEMENT_WA_RESTRICTED_ADDRESS_FILTERING_E",
    "CPSS_DXCH_IMPLEMENT_WA_RGMII_EDGE_ALIGN_MODE_E",
    "CPSS_DXCH_IMPLEMENT_WA_IP_MC_UNDER_STRESS_E",
    "CPSS_DXCH_IMPLEMENT_WA_GE_PORT_UNIDIRECT_E",
    "CPSS_DXCH_IMPLEMENT_WA_2_5G_SGMII_LINK_UP_E",
    "CPSS_DXCH_IMPLEMENT_WA_SGMII_2_5G_UNDER_TRAFFIC_CFG_E",
    "CPSS_DXCH_IMPLEMENT_WA_SGMII_2_5G_UNDER_TRAFFIC_NETWORKP_CFG_E",
    "CPSS_DXCH_IMPLEMENT_WA_SGMII_2_5G_UNDER_TRAFFIC_STACKP_CFG_E",
    "CPSS_DXCH_IMPLEMENT_WA_SERDES_INTERNAL_REG_ACCESS_E",
    "CPSS_DXCH_IMPLEMENT_WA_IPM_BRIDGE_COPY_GET_DROPPED_E",
    "CPSS_DXCH_IMPLEMENT_WA_NO_ALLIGNMENT_LOCK_E",
    "CPSS_DXCH_IMPLEMENT_WA_MC_BANDWIDTH_RESTRICTION_E",
    "CPSS_DXCH_IMPLEMENT_WA_DISMATCH_IF_LINK_E",
    "CPSS_DXCH_IMPLEMENT_WA_CUT_THROUGH_SLOW_TO_FAST_E",
    "CPSS_DXCH_IMPLEMENT_WA_RXAUI_LINK_E",
    "CPSS_DXCH_IMPLEMENT_WA_TRI_SPEED_PORT_AN_FC_E",
    "CPSS_DXCH_IMPLEMENT_WA_BOBCAT2_REV_A0_40G_NOT_THROUGH_TM_IS_PA_30G_E",
    "CPSS_DXCH_IMPLEMENT_WA_WRONG_MIB_COUNTERS_LINK_DOWN_E",
    "CPSS_DXCH_IMPLEMENT_WA_ROUTER_TCAM_RM_E",
    "CPSS_DXCH_IMPLEMENT_WA_BOBCAT2_PTP_TIMESTAMP_E",
    "CPSS_DXCH_IMPLEMENT_WA_LAST_E"
};
PRV_CPSS_LOG_STC_ENUM_ARRAY_SIZE_MAC(CPSS_DXCH_IMPLEMENT_WA_ENT);
char * prvCpssLogEnum_CPSS_DXCH_LED_CPU_OR_PORT27_ENT[]  =
{
    "CPSS_DXCH_LED_CPU_E",
    "CPSS_DXCH_LED_PORT27_E"
};
PRV_CPSS_LOG_STC_ENUM_ARRAY_SIZE_MAC(CPSS_DXCH_LED_CPU_OR_PORT27_ENT);
char * prvCpssLogEnum_CPSS_DXCH_LED_INDICATION_ENT[]  =
{
    "CPSS_DXCH_LED_INDICATION_PRIMARY_E",
    "CPSS_DXCH_LED_INDICATION_LINK_E",
    "CPSS_DXCH_LED_INDICATION_RX_ACT_E",
    "CPSS_DXCH_LED_INDICATION_TX_ACT_E",
    "CPSS_DXCH_LED_INDICATION_LOCAL_FAULT_OR_PCS_LINK_E",
    "CPSS_DXCH_LED_INDICATION_REMOTE_FAULT_OR_RESERVED_E",
    "CPSS_DXCH_LED_INDICATION_UKN_SEQ_OR_DUPLEX_E",
    "CPSS_DXCH_LED_INDICATION_P_REJ_E",
    "CPSS_DXCH_LED_INDICATION_RX_ERROR_E",
    "CPSS_DXCH_LED_INDICATION_JABBER_E",
    "CPSS_DXCH_LED_INDICATION_FRAGMENT_E",
    "CPSS_DXCH_LED_INDICATION_CRC_ERROR_E",
    "CPSS_DXCH_LED_INDICATION_FC_RX_E",
    "CPSS_DXCH_LED_INDICATION_TX_BAD_CRC_OR_LATE_COL_E",
    "CPSS_DXCH_LED_INDICATION_RX_BUFFER_FULL_OR_BACK_PR_E",
    "CPSS_DXCH_LED_INDICATION_WATCHDOG_EXP_SAMPLE_E",
    "CPSS_DXCH_LED_INDICATION_RX_TX_ACT_E"
};
PRV_CPSS_LOG_STC_ENUM_ARRAY_SIZE_MAC(CPSS_DXCH_LED_INDICATION_ENT);
char * prvCpssLogEnum_CPSS_DXCH_LED_PORT_TYPE_ENT[]  =
{
    "CPSS_DXCH_LED_PORT_TYPE_TRI_SPEED_E",
    "CPSS_DXCH_LED_PORT_TYPE_XG_E"
};
PRV_CPSS_LOG_STC_ENUM_ARRAY_SIZE_MAC(CPSS_DXCH_LED_PORT_TYPE_ENT);
char * prvCpssLogEnum_CPSS_DXCH_PP_SERDES_REF_CLOCK_ENT[]  =
{
    "CPSS_DXCH_PP_SERDES_REF_CLOCK_EXTERNAL_25_SINGLE_ENDED_E",
    "CPSS_DXCH_PP_SERDES_REF_CLOCK_EXTERNAL_125_SINGLE_ENDED_E",
    "CPSS_DXCH_PP_SERDES_REF_CLOCK_EXTERNAL_125_DIFF_E",
    "CPSS_DXCH_PP_SERDES_REF_CLOCK_EXTERNAL_156_25_SINGLE_ENDED_E",
    "CPSS_DXCH_PP_SERDES_REF_CLOCK_EXTERNAL_156_25_DIFF_E",
    "CPSS_DXCH_PP_SERDES_REF_CLOCK_INTERNAL_125_E"
};
PRV_CPSS_LOG_STC_ENUM_ARRAY_SIZE_MAC(CPSS_DXCH_PP_SERDES_REF_CLOCK_ENT);


/********* structure fields log functions *********/

void prvCpssLogParamFuncStc_CPSS_DXCH_ADDR_DECODE_WINDOW_CONFIG_STC_PTR
(
    IN CPSS_LOG_LIB_ENT         contextLib,
    IN CPSS_LOG_TYPE_ENT        logType,
    IN GT_CHAR_PTR              namePtr,
    IN void                   * fieldPtr,
    INOUT PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC     * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_START_FIELD_STC_MAC(CPSS_DXCH_ADDR_DECODE_WINDOW_CONFIG_STC *, valPtr);
    PRV_CPSS_LOG_UNUSED_MAC(inOutParamInfoPtr);
    PRV_CPSS_LOG_STC_BOOL_MAC(valPtr, enableWindow);
    PRV_CPSS_LOG_STC_NUMBER_MAC(valPtr, baseAddress);
    PRV_CPSS_LOG_STC_NUMBER_MAC(valPtr, windowSize);
    PRV_CPSS_LOG_STC_NUMBER_MAC(valPtr, remapAddress);
    prvCpssLogStcLogEnd(contextLib, logType);
}
void prvCpssLogParamFuncStc_CPSS_DXCH_LED_CLASS_CONF_STC_PTR
(
    IN CPSS_LOG_LIB_ENT         contextLib,
    IN CPSS_LOG_TYPE_ENT        logType,
    IN GT_CHAR_PTR              namePtr,
    IN void                   * fieldPtr,
    INOUT PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC     * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_START_FIELD_STC_MAC(CPSS_DXCH_LED_CLASS_CONF_STC *, valPtr);
    PRV_CPSS_LOG_UNUSED_MAC(inOutParamInfoPtr);
    PRV_CPSS_LOG_STC_NUMBER_MAC(valPtr, classNum);
    PRV_CPSS_LOG_STC_NUMBER_MAC(valPtr, classConf);
    prvCpssLogStcLogEnd(contextLib, logType);
}
void prvCpssLogParamFuncStc_CPSS_DXCH_LED_GROUP_CONF_STC_PTR
(
    IN CPSS_LOG_LIB_ENT         contextLib,
    IN CPSS_LOG_TYPE_ENT        logType,
    IN GT_CHAR_PTR              namePtr,
    IN void                   * fieldPtr,
    INOUT PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC     * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_START_FIELD_STC_MAC(CPSS_DXCH_LED_GROUP_CONF_STC *, valPtr);
    PRV_CPSS_LOG_UNUSED_MAC(inOutParamInfoPtr);
    PRV_CPSS_LOG_STC_NUMBER_MAC(valPtr, groupNum);
    PRV_CPSS_LOG_STC_NUMBER_MAC(valPtr, groupConf);
    prvCpssLogStcLogEnd(contextLib, logType);
}
void prvCpssLogParamFuncStc_CPSS_DXCH_PP_PHASE1_INIT_INFO_STC_PTR
(
    IN CPSS_LOG_LIB_ENT         contextLib,
    IN CPSS_LOG_TYPE_ENT        logType,
    IN GT_CHAR_PTR              namePtr,
    IN void                   * fieldPtr,
    INOUT PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC     * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_START_FIELD_STC_MAC(CPSS_DXCH_PP_PHASE1_INIT_INFO_STC *, valPtr);
    PRV_CPSS_LOG_UNUSED_MAC(inOutParamInfoPtr);
    PRV_CPSS_LOG_STC_NUMBER_MAC(valPtr, devNum);
    PRV_CPSS_LOG_STC_UINTPTR_MAC(valPtr, busBaseAddr);
    PRV_CPSS_LOG_STC_UINTPTR_MAC(valPtr, internalPciBase);
    PRV_CPSS_LOG_STC_UINTPTR_MAC(valPtr, resetAndInitControllerBase);
    PRV_CPSS_LOG_STC_STC_MAC(valPtr, hwAddr, CPSS_HW_ADDR_STC);
    PRV_CPSS_LOG_STC_NUMBER_MAC(valPtr, intVecNum);
    PRV_CPSS_LOG_STC_UINTPTR_MAC(valPtr, intMask);
    PRV_CPSS_LOG_STC_NUMBER_MAC(valPtr, coreClock);
    PRV_CPSS_LOG_STC_ENUM_MAC(valPtr, mngInterfaceType, CPSS_PP_INTERFACE_CHANNEL_ENT);
    PRV_CPSS_LOG_STC_ENUM_MAC(valPtr, ppHAState, CPSS_SYS_HA_MODE_ENT);
    PRV_CPSS_LOG_STC_STC_MAC(valPtr, powerDownPortsBmp, CPSS_PORTS_BMP_STC);
    PRV_CPSS_LOG_STC_NUMBER_MAC(valPtr, numOfPortGroups);
    PRV_CPSS_LOG_STC_STC_PTR_MAC(valPtr, multiPortGroupsInfoPtr, CPSS_PP_PORT_GROUP_INIT_INFO_STC);
    PRV_CPSS_LOG_STC_ENUM_MAC(valPtr, serdesRefClock, CPSS_DXCH_PP_SERDES_REF_CLOCK_ENT);
    PRV_CPSS_LOG_STC_BOOL_MAC(valPtr, initSerdesDefaults);
    PRV_CPSS_LOG_STC_BOOL_MAC(valPtr, isExternalCpuConnected);
    PRV_CPSS_LOG_STC_BOOL_MAC(valPtr, tcamParityCalcEnable);
    PRV_CPSS_LOG_STC_NUMBER_MAC(valPtr, isrAddrCompletionRegionsBmp);
    PRV_CPSS_LOG_STC_NUMBER_MAC(valPtr, appAddrCompletionRegionsBmp);
    PRV_CPSS_LOG_STC_BOOL_MAC(valPtr, enableLegacyVplsModeSupport);
    PRV_CPSS_LOG_STC_NUMBER_MAC(valPtr, maxNumOfPhyPortsToUse);
    prvCpssLogStcLogEnd(contextLib, logType);
}
void prvCpssLogParamFuncStc_CPSS_DXCH_PP_PHASE2_INIT_INFO_STC_PTR
(
    IN CPSS_LOG_LIB_ENT         contextLib,
    IN CPSS_LOG_TYPE_ENT        logType,
    IN GT_CHAR_PTR              namePtr,
    IN void                   * fieldPtr,
    INOUT PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC     * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_START_FIELD_STC_MAC(CPSS_DXCH_PP_PHASE2_INIT_INFO_STC *, valPtr);
    PRV_CPSS_LOG_UNUSED_MAC(inOutParamInfoPtr);
    PRV_CPSS_LOG_STC_NUMBER_MAC(valPtr, newDevNum);
    PRV_CPSS_LOG_STC_STC_MAC(valPtr, netIfCfg, CPSS_NET_IF_CFG_STC);
    PRV_CPSS_LOG_STC_STC_MAC(valPtr, auqCfg, CPSS_AUQ_CFG_STC);
    PRV_CPSS_LOG_STC_BOOL_MAC(valPtr, fuqUseSeparate);
    PRV_CPSS_LOG_STC_STC_MAC(valPtr, fuqCfg, CPSS_AUQ_CFG_STC);
    PRV_CPSS_LOG_STC_BOOL_MAC(valPtr, useSecondaryAuq);
    PRV_CPSS_LOG_STC_BOOL_MAC(valPtr, noTraffic2CPU);
    PRV_CPSS_LOG_STC_NUMBER_MAC(valPtr, netifSdmaPortGroupId);
    PRV_CPSS_LOG_STC_ENUM_MAC(valPtr, auMessageLength, CPSS_AU_MESSAGE_LENGTH_ENT);
    PRV_CPSS_LOG_STC_BOOL_MAC(valPtr, useDoubleAuq);
    PRV_CPSS_LOG_STC_BOOL_MAC(valPtr, useMultiNetIfSdma);
    PRV_CPSS_LOG_STC_STC_MAC(valPtr, multiNetIfCfg, CPSS_MULTI_NET_IF_CFG_STC);
    prvCpssLogStcLogEnd(contextLib, logType);
}


/********* parameters log functions *********/

void prvCpssLogParamFunc_CPSS_DXCH_ADDR_DECODE_WINDOW_CONFIG_STC_PTR(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_START_PARAM_STC_MAC(CPSS_DXCH_ADDR_DECODE_WINDOW_CONFIG_STC*, paramVal);
    prvCpssLogParamFuncStc_CPSS_DXCH_ADDR_DECODE_WINDOW_CONFIG_STC_PTR(contextLib, logType, namePtr, paramVal, inOutParamInfoPtr);
}
void prvCpssLogParamFunc_CPSS_DXCH_IMPLEMENT_WA_ENT(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_SET_PARAM_VAL_MAC(CPSS_DXCH_IMPLEMENT_WA_ENT, paramVal);
    PRV_CPSS_LOG_ENUM_MAC(namePtr, paramVal, CPSS_DXCH_IMPLEMENT_WA_ENT);
}
void prvCpssLogParamFunc_CPSS_DXCH_LED_CLASS_CONF_STC_PTR(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_START_PARAM_STC_MAC(CPSS_DXCH_LED_CLASS_CONF_STC*, paramVal);
    prvCpssLogParamFuncStc_CPSS_DXCH_LED_CLASS_CONF_STC_PTR(contextLib, logType, namePtr, paramVal, inOutParamInfoPtr);
}
void prvCpssLogParamFunc_CPSS_DXCH_LED_CPU_OR_PORT27_ENT(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_SET_PARAM_VAL_MAC(CPSS_DXCH_LED_CPU_OR_PORT27_ENT, paramVal);
    PRV_CPSS_LOG_ENUM_MAC(namePtr, paramVal, CPSS_DXCH_LED_CPU_OR_PORT27_ENT);
}
void prvCpssLogParamFunc_CPSS_DXCH_LED_CPU_OR_PORT27_ENT_PTR(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_SET_PARAM_VAL_MAC(CPSS_DXCH_LED_CPU_OR_PORT27_ENT*, paramVal);
    if (paramVal == NULL)
    {
        PRV_CPSS_LOG_AND_HISTORY_PARAM_MAC(contextLib, logType, "%s = NULL\n", namePtr);
        return;
    }
    PRV_CPSS_LOG_ENUM_MAC(namePtr, *paramVal, CPSS_DXCH_LED_CPU_OR_PORT27_ENT);
}
void prvCpssLogParamFunc_CPSS_DXCH_LED_GROUP_CONF_STC_PTR(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_START_PARAM_STC_MAC(CPSS_DXCH_LED_GROUP_CONF_STC*, paramVal);
    prvCpssLogParamFuncStc_CPSS_DXCH_LED_GROUP_CONF_STC_PTR(contextLib, logType, namePtr, paramVal, inOutParamInfoPtr);
}
void prvCpssLogParamFunc_CPSS_DXCH_LED_INDICATION_ENT(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_SET_PARAM_VAL_MAC(CPSS_DXCH_LED_INDICATION_ENT, paramVal);
    PRV_CPSS_LOG_ENUM_MAC(namePtr, paramVal, CPSS_DXCH_LED_INDICATION_ENT);
}
void prvCpssLogParamFunc_CPSS_DXCH_LED_INDICATION_ENT_PTR(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_SET_PARAM_VAL_MAC(CPSS_DXCH_LED_INDICATION_ENT*, paramVal);
    if (paramVal == NULL)
    {
        PRV_CPSS_LOG_AND_HISTORY_PARAM_MAC(contextLib, logType, "%s = NULL\n", namePtr);
        return;
    }
    PRV_CPSS_LOG_ENUM_MAC(namePtr, *paramVal, CPSS_DXCH_LED_INDICATION_ENT);
}
void prvCpssLogParamFunc_CPSS_DXCH_LED_PORT_TYPE_ENT(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_SET_PARAM_VAL_MAC(CPSS_DXCH_LED_PORT_TYPE_ENT, paramVal);
    PRV_CPSS_LOG_ENUM_MAC(namePtr, paramVal, CPSS_DXCH_LED_PORT_TYPE_ENT);
}
void prvCpssLogParamFunc_CPSS_DXCH_PP_PHASE1_INIT_INFO_STC_PTR(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_START_PARAM_STC_MAC(CPSS_DXCH_PP_PHASE1_INIT_INFO_STC*, paramVal);
    prvCpssLogParamFuncStc_CPSS_DXCH_PP_PHASE1_INIT_INFO_STC_PTR(contextLib, logType, namePtr, paramVal, inOutParamInfoPtr);
}
void prvCpssLogParamFunc_CPSS_DXCH_PP_PHASE2_INIT_INFO_STC_PTR(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_START_PARAM_STC_MAC(CPSS_DXCH_PP_PHASE2_INIT_INFO_STC*, paramVal);
    prvCpssLogParamFuncStc_CPSS_DXCH_PP_PHASE2_INIT_INFO_STC_PTR(contextLib, logType, namePtr, paramVal, inOutParamInfoPtr);
}


/********* API fields DB *********/

PRV_CPSS_LOG_FUNC_PARAM_STC INOUT_GT_U32_PTR_configDevDataBufferSizePtr = {
     "configDevDataBufferSizePtr", PRV_CPSS_LOG_PARAM_INOUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(GT_U32)
};
PRV_CPSS_LOG_FUNC_PARAM_STC INOUT_void_PTR_configDevDataBufferPtr = {
     "configDevDataBufferPtr", PRV_CPSS_LOG_PARAM_INOUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(void)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_DXCH_ADDR_DECODE_WINDOW_CONFIG_STC_PTR_windowConfigPtr = {
     "windowConfigPtr", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(CPSS_DXCH_ADDR_DECODE_WINDOW_CONFIG_STC)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_DXCH_IMPLEMENT_WA_ENT_PTR_waArr = {
     "waArr", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(CPSS_DXCH_IMPLEMENT_WA_ENT)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_DXCH_IMPLEMENT_WA_ENT_wa = {
     "wa", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_MAC(CPSS_DXCH_IMPLEMENT_WA_ENT)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_DXCH_LED_CLASS_CONF_STC_PTR_ledClassConfPtr = {
     "ledClassConfPtr", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(CPSS_DXCH_LED_CLASS_CONF_STC)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_DXCH_LED_CPU_OR_PORT27_ENT_indicatedPort = {
     "indicatedPort", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_MAC(CPSS_DXCH_LED_CPU_OR_PORT27_ENT)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_DXCH_LED_GROUP_CONF_STC_PTR_ledGroupConfPtr = {
     "ledGroupConfPtr", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(CPSS_DXCH_LED_GROUP_CONF_STC)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_DXCH_LED_INDICATION_ENT_indication = {
     "indication", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_MAC(CPSS_DXCH_LED_INDICATION_ENT)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_DXCH_LED_PORT_TYPE_ENT_portType = {
     "portType", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_MAC(CPSS_DXCH_LED_PORT_TYPE_ENT)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_DXCH_PP_PHASE1_INIT_INFO_STC_PTR_ppPhase1ParamsPtr = {
     "ppPhase1ParamsPtr", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(CPSS_DXCH_PP_PHASE1_INIT_INFO_STC)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_DXCH_PP_PHASE2_INIT_INFO_STC_PTR_ppPhase2ParamsPtr = {
     "ppPhase2ParamsPtr", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(CPSS_DXCH_PP_PHASE2_INIT_INFO_STC)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_HW_PP_RESET_SKIP_TYPE_ENT_skipType = {
     "skipType", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_MAC(CPSS_HW_PP_RESET_SKIP_TYPE_ENT)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_LED_CLASS_MANIPULATION_STC_PTR_classParamsPtr = {
     "classParamsPtr", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(CPSS_LED_CLASS_MANIPULATION_STC)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_LED_CONF_STC_PTR_ledConfPtr = {
     "ledConfPtr", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(CPSS_LED_CONF_STC)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_LED_GROUP_CONF_STC_PTR_groupParamsPtr = {
     "groupParamsPtr", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(CPSS_LED_GROUP_CONF_STC)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_PP_DEVICE_TYPE_devType = {
     "devType", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_MAC(CPSS_PP_DEVICE_TYPE)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_REG_VALUE_INFO_STC_PTR_initDataListPtr = {
     "initDataListPtr", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(CPSS_REG_VALUE_INFO_STC)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_RESOURCE_MAPPING_STC_PTR_resourceMappingPtr = {
     "resourceMappingPtr", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(CPSS_RESOURCE_MAPPING_STC)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_BOOL_invertEnable = {
     "invertEnable", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_MAC(GT_BOOL)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_BOOL_linkChangeOverride = {
     "linkChangeOverride", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_MAC(GT_BOOL)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_BOOL_loadFromEeprom = {
     "loadFromEeprom", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_MAC(GT_BOOL)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_BOOL_skipEnable = {
     "skipEnable", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_MAC(GT_BOOL)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_U32_PTR_additionalInfoBmpArr = {
     "additionalInfoBmpArr", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(GT_U32)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_U32_classNum = {
     "classNum", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_MAC(GT_U32)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_U32_configDevDataBufferSize = {
     "configDevDataBufferSize", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_MAC(GT_U32)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_U32_groupNum = {
     "groupNum", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_MAC(GT_U32)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_U32_initDataListLen = {
     "initDataListLen", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_MAC(GT_U32)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_U32_ledClassNum = {
     "ledClassNum", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_MAC(GT_U32)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_U32_ledControl = {
     "ledControl", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_MAC(GT_U32)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_U32_ledGroupNum = {
     "ledGroupNum", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_MAC(GT_U32)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_U32_ledInterfaceNum = {
     "ledInterfaceNum", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_MAC(GT_U32)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_U32_numOfWa = {
     "numOfWa", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_MAC(GT_U32)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_U32_period = {
     "period", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_MAC(GT_U32)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_U32_position = {
     "position", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_MAC(GT_U32)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_U32_windowNumber = {
     "windowNumber", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_MAC(GT_U32)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_void_PTR_configDevDataBufferPtr = {
     "configDevDataBufferPtr", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(void)
};
PRV_CPSS_LOG_FUNC_PARAM_STC OUT_CPSS_DXCH_ADDR_DECODE_WINDOW_CONFIG_STC_PTR_windowConfigPtr = {
     "windowConfigPtr", PRV_CPSS_LOG_PARAM_OUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(CPSS_DXCH_ADDR_DECODE_WINDOW_CONFIG_STC)
};
PRV_CPSS_LOG_FUNC_PARAM_STC OUT_CPSS_DXCH_LED_CPU_OR_PORT27_ENT_PTR_indicatedPortPtr = {
     "indicatedPortPtr", PRV_CPSS_LOG_PARAM_OUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(CPSS_DXCH_LED_CPU_OR_PORT27_ENT)
};
PRV_CPSS_LOG_FUNC_PARAM_STC OUT_CPSS_DXCH_LED_INDICATION_ENT_PTR_indicationPtr = {
     "indicationPtr", PRV_CPSS_LOG_PARAM_OUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(CPSS_DXCH_LED_INDICATION_ENT)
};
PRV_CPSS_LOG_FUNC_PARAM_STC OUT_CPSS_HW_PP_INIT_STAGE_ENT_PTR_initStagePtr = {
     "initStagePtr", PRV_CPSS_LOG_PARAM_OUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(CPSS_HW_PP_INIT_STAGE_ENT)
};
PRV_CPSS_LOG_FUNC_PARAM_STC OUT_CPSS_LED_CLASS_MANIPULATION_STC_PTR_classParamsPtr = {
     "classParamsPtr", PRV_CPSS_LOG_PARAM_OUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(CPSS_LED_CLASS_MANIPULATION_STC)
};
PRV_CPSS_LOG_FUNC_PARAM_STC OUT_CPSS_LED_CONF_STC_PTR_ledConfPtr = {
     "ledConfPtr", PRV_CPSS_LOG_PARAM_OUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(CPSS_LED_CONF_STC)
};
PRV_CPSS_LOG_FUNC_PARAM_STC OUT_CPSS_LED_GROUP_CONF_STC_PTR_groupParamsPtr = {
     "groupParamsPtr", PRV_CPSS_LOG_PARAM_OUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(CPSS_LED_GROUP_CONF_STC)
};
PRV_CPSS_LOG_FUNC_PARAM_STC OUT_CPSS_PP_DEVICE_TYPE_PTR_deviceTypePtr = {
     "deviceTypePtr", PRV_CPSS_LOG_PARAM_OUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(CPSS_PP_DEVICE_TYPE)
};
PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_BOOL_PTR_inSyncTrigPtr = {
     "inSyncTrigPtr", PRV_CPSS_LOG_PARAM_OUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(GT_BOOL)
};
PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_BOOL_PTR_invertEnablePtr = {
     "invertEnablePtr", PRV_CPSS_LOG_PARAM_OUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(GT_BOOL)
};
PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_BOOL_PTR_isImplemented = {
     "isImplemented", PRV_CPSS_LOG_PARAM_OUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(GT_BOOL)
};
PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_BOOL_PTR_linkChangeOverridePtr = {
     "linkChangeOverridePtr", PRV_CPSS_LOG_PARAM_OUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(GT_BOOL)
};
PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_BOOL_PTR_skipEnablePtr = {
     "skipEnablePtr", PRV_CPSS_LOG_PARAM_OUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(GT_BOOL)
};
PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_HW_DEV_NUM_PTR_hwDevNumPtr = {
     "hwDevNumPtr", PRV_CPSS_LOG_PARAM_OUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(GT_HW_DEV_NUM)
};
PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_U32_PTR_byteAlignmentPtr = {
     "byteAlignmentPtr", PRV_CPSS_LOG_PARAM_OUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(GT_U32)
};
PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_U32_PTR_coreClkDbPtr = {
     "coreClkDbPtr", PRV_CPSS_LOG_PARAM_OUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(GT_U32)
};
PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_U32_PTR_coreClkHwPtr = {
     "coreClkHwPtr", PRV_CPSS_LOG_PARAM_OUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(GT_U32)
};
PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_U32_PTR_descSizePtr = {
     "descSizePtr", PRV_CPSS_LOG_PARAM_OUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(GT_U32)
};
PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_U32_PTR_periodPtr = {
     "periodPtr", PRV_CPSS_LOG_PARAM_OUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(GT_U32)
};
PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_U32_PTR_positionPtr = {
     "positionPtr", PRV_CPSS_LOG_PARAM_OUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(GT_U32)
};


/********* API prototypes DB *********/

PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChHwPpPhase1Init_PARAMS[] =  {
    &IN_CPSS_DXCH_PP_PHASE1_INIT_INFO_STC_PTR_ppPhase1ParamsPtr,
    &OUT_CPSS_PP_DEVICE_TYPE_PTR_deviceTypePtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChHwRxBufAlignmentGet_PARAMS[] =  {
    &IN_CPSS_PP_DEVICE_TYPE_devType,
    &OUT_GT_U32_PTR_byteAlignmentPtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChHwAuDescSizeGet_PARAMS[] =  {
    &IN_CPSS_PP_DEVICE_TYPE_devType,
    &OUT_GT_U32_PTR_descSizePtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChPpHwImplementWaGet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_CPSS_DXCH_IMPLEMENT_WA_ENT_wa,
    &OUT_GT_BOOL_PTR_isImplemented
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChLedStreamCpuOrPort27ModeSet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_CPSS_DXCH_LED_CPU_OR_PORT27_ENT_indicatedPort
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChHwPpPhase2Init_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_CPSS_DXCH_PP_PHASE2_INIT_INFO_STC_PTR_ppPhase2ParamsPtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChHwPpSoftResetSkipParamSet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_CPSS_HW_PP_RESET_SKIP_TYPE_ENT_skipType,
    &IN_GT_BOOL_skipEnable
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChHwPpSoftResetSkipParamGet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_CPSS_HW_PP_RESET_SKIP_TYPE_ENT_skipType,
    &OUT_GT_BOOL_PTR_skipEnablePtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssHwInitResourceMappingSet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_CPSS_RESOURCE_MAPPING_STC_PTR_resourceMappingPtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChHwInterruptCoalescingSet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_BOOL_enable,
    &IN_GT_U32_period,
    &IN_GT_BOOL_linkChangeOverride
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChHwPpStartInit_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_BOOL_loadFromEeprom,
    &IN_CPSS_REG_VALUE_INFO_STC_PTR_initDataListPtr,
    &IN_GT_U32_initDataListLen
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChCfgHwDevNumSet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_HW_DEV_NUM_hwDevNum
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChLedStreamPortClassPolarityInvertEnableSet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_PHYSICAL_PORT_NUM_portNum,
    &IN_GT_U32_classNum,
    &IN_GT_BOOL_invertEnable
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChLedStreamPortClassPolarityInvertEnableGet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_PHYSICAL_PORT_NUM_portNum,
    &IN_GT_U32_classNum,
    &OUT_GT_BOOL_PTR_invertEnablePtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChLedStreamPortPositionSet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_PHYSICAL_PORT_NUM_portNum,
    &IN_GT_U32_position
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChLedStreamPortPositionGet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_PHYSICAL_PORT_NUM_portNum,
    &OUT_GT_U32_PTR_positionPtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChLedStreamPortGroupClassManipulationSet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_PORT_GROUPS_BMP_portGroupsBmp,
    &IN_GT_U32_ledInterfaceNum,
    &IN_CPSS_DXCH_LED_PORT_TYPE_ENT_portType,
    &IN_GT_U32_classNum,
    &IN_CPSS_LED_CLASS_MANIPULATION_STC_PTR_classParamsPtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChLedStreamPortGroupClassManipulationGet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_PORT_GROUPS_BMP_portGroupsBmp,
    &IN_GT_U32_ledInterfaceNum,
    &IN_CPSS_DXCH_LED_PORT_TYPE_ENT_portType,
    &IN_GT_U32_classNum,
    &OUT_CPSS_LED_CLASS_MANIPULATION_STC_PTR_classParamsPtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChLedStreamPortGroupGroupConfigSet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_PORT_GROUPS_BMP_portGroupsBmp,
    &IN_GT_U32_ledInterfaceNum,
    &IN_CPSS_DXCH_LED_PORT_TYPE_ENT_portType,
    &IN_GT_U32_groupNum,
    &IN_CPSS_LED_GROUP_CONF_STC_PTR_groupParamsPtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChLedStreamPortGroupGroupConfigGet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_PORT_GROUPS_BMP_portGroupsBmp,
    &IN_GT_U32_ledInterfaceNum,
    &IN_CPSS_DXCH_LED_PORT_TYPE_ENT_portType,
    &IN_GT_U32_groupNum,
    &OUT_CPSS_LED_GROUP_CONF_STC_PTR_groupParamsPtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChLedStreamPortGroupConfigSet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_PORT_GROUPS_BMP_portGroupsBmp,
    &IN_GT_U32_ledInterfaceNum,
    &IN_CPSS_LED_CONF_STC_PTR_ledConfPtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChLedStreamPortGroupConfigGet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_PORT_GROUPS_BMP_portGroupsBmp,
    &IN_GT_U32_ledInterfaceNum,
    &OUT_CPSS_LED_CONF_STC_PTR_ledConfPtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChLedStreamClassAndGroupConfig_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_U32_ledControl,
    &IN_CPSS_DXCH_LED_CLASS_CONF_STC_PTR_ledClassConfPtr,
    &IN_GT_U32_ledClassNum,
    &IN_CPSS_DXCH_LED_GROUP_CONF_STC_PTR_ledGroupConfPtr,
    &IN_GT_U32_ledGroupNum
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChLedStreamClassManipulationSet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_U32_ledInterfaceNum,
    &IN_CPSS_DXCH_LED_PORT_TYPE_ENT_portType,
    &IN_GT_U32_classNum,
    &IN_CPSS_LED_CLASS_MANIPULATION_STC_PTR_classParamsPtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChLedStreamClassManipulationGet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_U32_ledInterfaceNum,
    &IN_CPSS_DXCH_LED_PORT_TYPE_ENT_portType,
    &IN_GT_U32_classNum,
    &OUT_CPSS_LED_CLASS_MANIPULATION_STC_PTR_classParamsPtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChLedStreamGroupConfigSet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_U32_ledInterfaceNum,
    &IN_CPSS_DXCH_LED_PORT_TYPE_ENT_portType,
    &IN_GT_U32_groupNum,
    &IN_CPSS_LED_GROUP_CONF_STC_PTR_groupParamsPtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChLedStreamGroupConfigGet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_U32_ledInterfaceNum,
    &IN_CPSS_DXCH_LED_PORT_TYPE_ENT_portType,
    &IN_GT_U32_groupNum,
    &OUT_CPSS_LED_GROUP_CONF_STC_PTR_groupParamsPtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChLedStreamConfigSet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_U32_ledInterfaceNum,
    &IN_CPSS_LED_CONF_STC_PTR_ledConfPtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChLedStreamDirectModeEnableSet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_U32_ledInterfaceNum,
    &IN_GT_BOOL_enable
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChLedStreamClassIndicationSet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_U32_ledInterfaceNum,
    &IN_GT_U32_classNum,
    &IN_CPSS_DXCH_LED_INDICATION_ENT_indication
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChLedStreamClassIndicationGet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_U32_ledInterfaceNum,
    &IN_GT_U32_classNum,
    &OUT_CPSS_DXCH_LED_INDICATION_ENT_PTR_indicationPtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChLedStreamConfigGet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_U32_ledInterfaceNum,
    &OUT_CPSS_LED_CONF_STC_PTR_ledConfPtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChLedStreamDirectModeEnableGet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_U32_ledInterfaceNum,
    &OUT_GT_BOOL_PTR_enablePtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChHwPpAddressDecodeWindowConfigSet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_U32_windowNumber,
    &IN_CPSS_DXCH_ADDR_DECODE_WINDOW_CONFIG_STC_PTR_windowConfigPtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChHwPpAddressDecodeWindowConfigGet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_U32_windowNumber,
    &OUT_CPSS_DXCH_ADDR_DECODE_WINDOW_CONFIG_STC_PTR_windowConfigPtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssPpConfigDevDataImport_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_void_PTR_configDevDataBufferPtr,
    &IN_GT_U32_configDevDataBufferSize
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssPpConfigDevDataExport_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &INOUT_void_PTR_configDevDataBufferPtr,
    &INOUT_GT_U32_PTR_configDevDataBufferSizePtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChLedStreamCpuOrPort27ModeGet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &OUT_CPSS_DXCH_LED_CPU_OR_PORT27_ENT_PTR_indicatedPortPtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChHwPpInitStageGet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &OUT_CPSS_HW_PP_INIT_STAGE_ENT_PTR_initStagePtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChLedStreamSyncLedsEnableGet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &OUT_GT_BOOL_PTR_enablePtr,
    &OUT_GT_BOOL_PTR_inSyncTrigPtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChHwInterruptCoalescingGet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &OUT_GT_BOOL_PTR_enablePtr,
    &OUT_GT_U32_PTR_periodPtr,
    &OUT_GT_BOOL_PTR_linkChangeOverridePtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChCfgHwDevNumGet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &OUT_GT_HW_DEV_NUM_PTR_hwDevNumPtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChHwCoreClockGet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &OUT_GT_U32_PTR_coreClkDbPtr,
    &OUT_GT_U32_PTR_coreClkHwPtr
};
extern PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChHwPpImplementWaInit_PARAMS[];


/********* lib API DB *********/

extern void cpssDxChHwPpStartInit_preLogic(IN va_list  args, OUT PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC * paramDataPtr);
extern void cpssDxChHwPpImplementWaInit_preLogic(IN va_list  args, OUT PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC * paramDataPtr);

static PRV_CPSS_LOG_FUNC_ENTRY_STC prvCpssDxChCpssHwInitLogLibDb[] = {
    {"cpssDxChHwPpPhase1Init", 2, cpssDxChHwPpPhase1Init_PARAMS, NULL},
    {"cpssDxChHwPpPhase2Init", 2, cpssDxChHwPpPhase2Init_PARAMS, NULL},
    {"cpssDxChHwPpStartInit", 4, cpssDxChHwPpStartInit_PARAMS, cpssDxChHwPpStartInit_preLogic},
    {"cpssDxChHwPpImplementWaInit", 4, cpssDxChHwPpImplementWaInit_PARAMS, cpssDxChHwPpImplementWaInit_preLogic},
    {"cpssDxChPpHwImplementWaGet", 3, cpssDxChPpHwImplementWaGet_PARAMS, NULL},
    {"cpssDxChHwAuDescSizeGet", 2, cpssDxChHwAuDescSizeGet_PARAMS, NULL},
    {"cpssDxChHwRxDescSizeGet", 2, cpssDxChHwAuDescSizeGet_PARAMS, NULL},
    {"cpssDxChHwTxDescSizeGet", 2, cpssDxChHwAuDescSizeGet_PARAMS, NULL},
    {"cpssDxChHwRxBufAlignmentGet", 2, cpssDxChHwRxBufAlignmentGet_PARAMS, NULL},
    {"cpssDxChHwPpSoftResetTrigger", 1, prvCpssLogGenDevNum_PARAMS, NULL},
    {"cpssDxChHwPpSoftResetSkipParamSet", 3, cpssDxChHwPpSoftResetSkipParamSet_PARAMS, NULL},
    {"cpssDxChHwPpSoftResetSkipParamGet", 3, cpssDxChHwPpSoftResetSkipParamGet_PARAMS, NULL},
    {"cpssDxChHwPpInitStageGet", 2, cpssDxChHwPpInitStageGet_PARAMS, NULL},
    {"cpssDxChCfgHwDevNumSet", 2, cpssDxChCfgHwDevNumSet_PARAMS, NULL},
    {"cpssDxChCfgHwDevNumGet", 2, cpssDxChCfgHwDevNumGet_PARAMS, NULL},
    {"cpssDxChHwCoreClockGet", 3, cpssDxChHwCoreClockGet_PARAMS, NULL},
    {"cpssDxChHwInterruptCoalescingSet", 4, cpssDxChHwInterruptCoalescingSet_PARAMS, NULL},
    {"cpssDxChHwInterruptCoalescingGet", 4, cpssDxChHwInterruptCoalescingGet_PARAMS, NULL},
    {"cpssDxChHwInitLion2GeBackwardCompatibility", 0, NULL, NULL},
    {"cpssDxChHwPpAddressDecodeWindowConfigSet", 3, cpssDxChHwPpAddressDecodeWindowConfigSet_PARAMS, NULL},
    {"cpssDxChHwPpAddressDecodeWindowConfigGet", 3, cpssDxChHwPpAddressDecodeWindowConfigGet_PARAMS, NULL},
    {"cpssDxChLedStreamClassAndGroupConfig", 6, cpssDxChLedStreamClassAndGroupConfig_PARAMS, NULL},
    {"cpssDxChLedStreamHyperGStackTxQStatusEnableSet", 2, prvCpssLogGenDevNumEnable_PARAMS, NULL},
    {"cpssDxChLedStreamHyperGStackTxQStatusEnableGet", 2, prvCpssLogGenDevNumEnablePtr_PARAMS, NULL},
    {"cpssDxChLedStreamCpuOrPort27ModeSet", 2, cpssDxChLedStreamCpuOrPort27ModeSet_PARAMS, NULL},
    {"cpssDxChLedStreamCpuOrPort27ModeGet", 2, cpssDxChLedStreamCpuOrPort27ModeGet_PARAMS, NULL},
    {"cpssDxChLedStreamClassManipulationSet", 5, cpssDxChLedStreamClassManipulationSet_PARAMS, NULL},
    {"cpssDxChLedStreamClassManipulationGet", 5, cpssDxChLedStreamClassManipulationGet_PARAMS, NULL},
    {"cpssDxChLedStreamGroupConfigSet", 5, cpssDxChLedStreamGroupConfigSet_PARAMS, NULL},
    {"cpssDxChLedStreamGroupConfigGet", 5, cpssDxChLedStreamGroupConfigGet_PARAMS, NULL},
    {"cpssDxChLedStreamConfigSet", 3, cpssDxChLedStreamConfigSet_PARAMS, NULL},
    {"cpssDxChLedStreamConfigGet", 3, cpssDxChLedStreamConfigGet_PARAMS, NULL},
    {"cpssDxChLedStreamClassIndicationSet", 4, cpssDxChLedStreamClassIndicationSet_PARAMS, NULL},
    {"cpssDxChLedStreamClassIndicationGet", 4, cpssDxChLedStreamClassIndicationGet_PARAMS, NULL},
    {"cpssDxChLedStreamDirectModeEnableSet", 3, cpssDxChLedStreamDirectModeEnableSet_PARAMS, NULL},
    {"cpssDxChLedStreamDirectModeEnableGet", 3, cpssDxChLedStreamDirectModeEnableGet_PARAMS, NULL},
    {"cpssDxChLedStreamPortGroupConfigSet", 4, cpssDxChLedStreamPortGroupConfigSet_PARAMS, NULL},
    {"cpssDxChLedStreamPortGroupConfigGet", 4, cpssDxChLedStreamPortGroupConfigGet_PARAMS, NULL},
    {"cpssDxChLedStreamPortGroupClassManipulationSet", 6, cpssDxChLedStreamPortGroupClassManipulationSet_PARAMS, NULL},
    {"cpssDxChLedStreamPortGroupClassManipulationGet", 6, cpssDxChLedStreamPortGroupClassManipulationGet_PARAMS, NULL},
    {"cpssDxChLedStreamPortGroupGroupConfigSet", 6, cpssDxChLedStreamPortGroupGroupConfigSet_PARAMS, NULL},
    {"cpssDxChLedStreamPortGroupGroupConfigGet", 6, cpssDxChLedStreamPortGroupGroupConfigGet_PARAMS, NULL},
    {"cpssDxChLedStreamSyncLedsEnableSet", 2, prvCpssLogGenDevNumEnable_PARAMS, NULL},
    {"cpssDxChLedStreamSyncLedsEnableGet", 3, cpssDxChLedStreamSyncLedsEnableGet_PARAMS, NULL},
    {"cpssDxChLedStreamPortFlexTriSpeedIndicationEnableSet", 3, prvCpssLogGenDevNumPortNumEnable2_PARAMS, NULL},
    {"cpssDxChLedStreamPortFlexTriSpeedIndicationEnableGet", 3, prvCpssLogGenDevNumPortNumEnablePtr2_PARAMS, NULL},
    {"cpssDxChLedStreamPortPositionSet", 3, cpssDxChLedStreamPortPositionSet_PARAMS, NULL},
    {"cpssDxChLedStreamPortPositionGet", 3, cpssDxChLedStreamPortPositionGet_PARAMS, NULL},
    {"cpssDxChLedStreamPortClassPolarityInvertEnableSet", 4, cpssDxChLedStreamPortClassPolarityInvertEnableSet_PARAMS, NULL},
    {"cpssDxChLedStreamPortClassPolarityInvertEnableGet", 4, cpssDxChLedStreamPortClassPolarityInvertEnableGet_PARAMS, NULL},
    {"cpssHwInitResourceMappingSet", 2, cpssHwInitResourceMappingSet_PARAMS, NULL},
    {"cpssPpInterruptsDisable", 1, prvCpssLogGenDevNum_PARAMS, NULL},
    {"cpssPpConfigDevDataImport", 3, cpssPpConfigDevDataImport_PARAMS, NULL},
    {"cpssPpConfigDevDataExport", 3, cpssPpConfigDevDataExport_PARAMS, NULL},
};

/******** DB Access Function ********/
void prvCpssLogParamLibDbGet_CPSS_LOG_LIB_HW_INIT(
    OUT PRV_CPSS_LOG_FUNC_ENTRY_STC ** logFuncDbPtrPtr,
    OUT GT_U32 * logFuncDbSizePtr
)
{
    *logFuncDbPtrPtr = prvCpssDxChCpssHwInitLogLibDb;
    *logFuncDbSizePtr = sizeof(prvCpssDxChCpssHwInitLogLibDb) / sizeof(PRV_CPSS_LOG_FUNC_ENTRY_STC);
}

