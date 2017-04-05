/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* prvCpssDxChTtiLog.h
*       WARNING!!! this is a generated file, please don't edit it manually
* COMMENTS:
*
* FILE REVISION NUMBER:
*       $Revision: 1 $
*******************************************************************************/
#ifndef __prvCpssDxChTtiLogh
#define __prvCpssDxChTtiLogh
#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
#include <cpss/generic/log/cpssLog.h>
#include <cpss/generic/log/prvCpssLog.h>


/********* manually implemented declarations *********/

void prvCpssLogParamFuncStc_CPSS_DXCH_TTI_ACTION_UNT_PTR
(
    IN CPSS_LOG_LIB_ENT         contextLib,
    IN CPSS_LOG_TYPE_ENT        logType,
    IN GT_CHAR_PTR              namePtr,
    IN void                   * fieldPtr,
    INOUT PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC     * inOutParamInfoPtr
);
void prvCpssLogParamFuncStc_CPSS_DXCH_TTI_RULE_UNT_PTR
(
    IN CPSS_LOG_LIB_ENT         contextLib,
    IN CPSS_LOG_TYPE_ENT        logType,
    IN GT_CHAR_PTR              namePtr,
    IN void                   * fieldPtr,
    INOUT PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC     * inOutParamInfoPtr
);


/********* enums *********/

PRV_CPSS_LOG_STC_ENUM_DECLARE_MAC(CPSS_DXCH_TTI_ACTION_TYPE_ENT);
PRV_CPSS_LOG_STC_ENUM_MAP_DECLARE_MAC(CPSS_DXCH_TTI_EXCEPTION_ENT);
PRV_CPSS_LOG_STC_ENUM_DECLARE_MAC(CPSS_DXCH_TTI_KEY_SIZE_ENT);
PRV_CPSS_LOG_STC_ENUM_DECLARE_MAC(CPSS_DXCH_TTI_KEY_TCAM_SEGMENT_MODE_ENT);
PRV_CPSS_LOG_STC_ENUM_DECLARE_MAC(CPSS_DXCH_TTI_KEY_TYPE_ENT);
PRV_CPSS_LOG_STC_ENUM_DECLARE_MAC(CPSS_DXCH_TTI_MAC_MODE_ENT);
PRV_CPSS_LOG_STC_ENUM_DECLARE_MAC(CPSS_DXCH_TTI_MULTICAST_DUPLICATION_MODE_ENT);
PRV_CPSS_LOG_STC_ENUM_DECLARE_MAC(CPSS_DXCH_TTI_OFFSET_TYPE_ENT);
PRV_CPSS_LOG_STC_ENUM_DECLARE_MAC(CPSS_DXCH_TTI_PW_CW_EXCEPTION_ENT);
PRV_CPSS_LOG_STC_ENUM_DECLARE_MAC(CPSS_DXCH_TTI_RULE_TYPE_ENT);
PRV_CPSS_LOG_STC_ENUM_DECLARE_MAC(CPSS_DXCH_TTI_MAC_TO_ME_INCLUDE_SRC_INTERFACE_TYPE_ENT);
PRV_CPSS_LOG_STC_ENUM_DECLARE_MAC(CPSS_DXCH_TTI_MODIFY_DSCP_ENT);
PRV_CPSS_LOG_STC_ENUM_DECLARE_MAC(CPSS_DXCH_TTI_MODIFY_UP_ENT);
PRV_CPSS_LOG_STC_ENUM_DECLARE_MAC(CPSS_DXCH_TTI_MPLS_CMD_ENT);
PRV_CPSS_LOG_STC_ENUM_DECLARE_MAC(CPSS_DXCH_TTI_PASSENGER_PARSING_OF_TRANSIT_MPLS_TUNNEL_MODE_TYPE_ENT);
PRV_CPSS_LOG_STC_ENUM_DECLARE_MAC(CPSS_DXCH_TTI_PASSENGER_TYPE_ENT);
PRV_CPSS_LOG_STC_ENUM_DECLARE_MAC(CPSS_DXCH_TTI_PTP_TRIGGER_TYPE_ENT);
PRV_CPSS_LOG_STC_ENUM_DECLARE_MAC(CPSS_DXCH_TTI_PW_TAG_MODE_ENT);
PRV_CPSS_LOG_STC_ENUM_DECLARE_MAC(CPSS_DXCH_TTI_QOS_MODE_TYPE_ENT);
PRV_CPSS_LOG_STC_ENUM_MAP_DECLARE_MAC(CPSS_DXCH_TTI_REDIRECT_COMMAND_ENT);
PRV_CPSS_LOG_STC_ENUM_DECLARE_MAC(CPSS_DXCH_TTI_TAG1_UP_COMMAND_ENT);
PRV_CPSS_LOG_STC_ENUM_DECLARE_MAC(CPSS_DXCH_TTI_VLAN_COMMAND_ENT);


/********* structure fields log functions *********/

void prvCpssLogParamFuncStc_CPSS_DXCH_TTI_MAC_TO_ME_SRC_INTERFACE_INFO_STC_PTR
(
    IN CPSS_LOG_LIB_ENT         contextLib,
    IN CPSS_LOG_TYPE_ENT        logType,
    IN GT_CHAR_PTR              namePtr,
    IN void                   * fieldPtr,
    INOUT PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC     * inOutParamInfoPtr
);
void prvCpssLogParamFuncStc_CPSS_DXCH_TTI_MAC_VLAN_STC_PTR
(
    IN CPSS_LOG_LIB_ENT         contextLib,
    IN CPSS_LOG_TYPE_ENT        logType,
    IN GT_CHAR_PTR              namePtr,
    IN void                   * fieldPtr,
    INOUT PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC     * inOutParamInfoPtr
);
void prvCpssLogParamFuncStc_CPSS_DXCH_TTI_TRILL_ADJACENCY_STC_PTR
(
    IN CPSS_LOG_LIB_ENT         contextLib,
    IN CPSS_LOG_TYPE_ENT        logType,
    IN GT_CHAR_PTR              namePtr,
    IN void                   * fieldPtr,
    INOUT PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC     * inOutParamInfoPtr
);
void prvCpssLogParamFuncStc_CPSS_DXCH_TTI_ACTION_2_STC_PTR
(
    IN CPSS_LOG_LIB_ENT         contextLib,
    IN CPSS_LOG_TYPE_ENT        logType,
    IN GT_CHAR_PTR              namePtr,
    IN void                   * fieldPtr,
    INOUT PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC     * inOutParamInfoPtr
);
void prvCpssLogParamFuncStc_CPSS_DXCH_TTI_ACTION_STC_PTR
(
    IN CPSS_LOG_LIB_ENT         contextLib,
    IN CPSS_LOG_TYPE_ENT        logType,
    IN GT_CHAR_PTR              namePtr,
    IN void                   * fieldPtr,
    INOUT PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC     * inOutParamInfoPtr
);
void prvCpssLogParamFuncStc_CPSS_DXCH_TTI_ETH_RULE_STC_PTR
(
    IN CPSS_LOG_LIB_ENT         contextLib,
    IN CPSS_LOG_TYPE_ENT        logType,
    IN GT_CHAR_PTR              namePtr,
    IN void                   * fieldPtr,
    INOUT PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC     * inOutParamInfoPtr
);
void prvCpssLogParamFuncStc_CPSS_DXCH_TTI_IPV4_RULE_STC_PTR
(
    IN CPSS_LOG_LIB_ENT         contextLib,
    IN CPSS_LOG_TYPE_ENT        logType,
    IN GT_CHAR_PTR              namePtr,
    IN void                   * fieldPtr,
    INOUT PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC     * inOutParamInfoPtr
);
void prvCpssLogParamFuncStc_CPSS_DXCH_TTI_MIM_RULE_STC_PTR
(
    IN CPSS_LOG_LIB_ENT         contextLib,
    IN CPSS_LOG_TYPE_ENT        logType,
    IN GT_CHAR_PTR              namePtr,
    IN void                   * fieldPtr,
    INOUT PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC     * inOutParamInfoPtr
);
void prvCpssLogParamFuncStc_CPSS_DXCH_TTI_MPLS_RULE_STC_PTR
(
    IN CPSS_LOG_LIB_ENT         contextLib,
    IN CPSS_LOG_TYPE_ENT        logType,
    IN GT_CHAR_PTR              namePtr,
    IN void                   * fieldPtr,
    INOUT PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC     * inOutParamInfoPtr
);
void prvCpssLogParamFuncStc_CPSS_DXCH_TTI_TRILL_RULE_STC_PTR
(
    IN CPSS_LOG_LIB_ENT         contextLib,
    IN CPSS_LOG_TYPE_ENT        logType,
    IN GT_CHAR_PTR              namePtr,
    IN void                   * fieldPtr,
    INOUT PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC     * inOutParamInfoPtr
);
void prvCpssLogParamFuncStc_CPSS_DXCH_TTI_RULE_COMMON_STC_PTR
(
    IN CPSS_LOG_LIB_ENT         contextLib,
    IN CPSS_LOG_TYPE_ENT        logType,
    IN GT_CHAR_PTR              namePtr,
    IN void                   * fieldPtr,
    INOUT PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC     * inOutParamInfoPtr
);


/********* parameters log functions *********/

void prvCpssLogParamFunc_CPSS_DXCH_TTI_ACTION_TYPE_ENT(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
);
void prvCpssLogParamFunc_CPSS_DXCH_TTI_ACTION_UNT_PTR(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
);
void prvCpssLogParamFunc_CPSS_DXCH_TTI_EXCEPTION_ENT(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
);
void prvCpssLogParamFunc_CPSS_DXCH_TTI_KEY_SIZE_ENT(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
);
void prvCpssLogParamFunc_CPSS_DXCH_TTI_KEY_SIZE_ENT_PTR(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
);
void prvCpssLogParamFunc_CPSS_DXCH_TTI_KEY_TCAM_SEGMENT_MODE_ENT(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
);
void prvCpssLogParamFunc_CPSS_DXCH_TTI_KEY_TCAM_SEGMENT_MODE_ENT_PTR(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
);
void prvCpssLogParamFunc_CPSS_DXCH_TTI_KEY_TYPE_ENT(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
);
void prvCpssLogParamFunc_CPSS_DXCH_TTI_MAC_MODE_ENT(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
);
void prvCpssLogParamFunc_CPSS_DXCH_TTI_MAC_MODE_ENT_PTR(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
);
void prvCpssLogParamFunc_CPSS_DXCH_TTI_MAC_TO_ME_SRC_INTERFACE_INFO_STC_PTR(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
);
void prvCpssLogParamFunc_CPSS_DXCH_TTI_MAC_VLAN_STC_PTR(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
);
void prvCpssLogParamFunc_CPSS_DXCH_TTI_MULTICAST_DUPLICATION_MODE_ENT(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
);
void prvCpssLogParamFunc_CPSS_DXCH_TTI_MULTICAST_DUPLICATION_MODE_ENT_PTR(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
);
void prvCpssLogParamFunc_CPSS_DXCH_TTI_OFFSET_TYPE_ENT(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
);
void prvCpssLogParamFunc_CPSS_DXCH_TTI_OFFSET_TYPE_ENT_PTR(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
);
void prvCpssLogParamFunc_CPSS_DXCH_TTI_PW_CW_EXCEPTION_ENT(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
);
void prvCpssLogParamFunc_CPSS_DXCH_TTI_RULE_TYPE_ENT(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
);
void prvCpssLogParamFunc_CPSS_DXCH_TTI_RULE_UNT_PTR(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
);
void prvCpssLogParamFunc_CPSS_DXCH_TTI_TRILL_ADJACENCY_STC_PTR(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
);


/********* API fields DB *********/

extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_DXCH_TTI_ACTION_TYPE_ENT_actionType;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_DXCH_TTI_ACTION_UNT_PTR_actionPtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_DXCH_TTI_EXCEPTION_ENT_exceptionType;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_DXCH_TTI_KEY_SIZE_ENT_size;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_DXCH_TTI_KEY_TCAM_SEGMENT_MODE_ENT_segmentMode;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_DXCH_TTI_KEY_TYPE_ENT_keyType;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_DXCH_TTI_MAC_MODE_ENT_macMode;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_DXCH_TTI_MAC_TO_ME_SRC_INTERFACE_INFO_STC_PTR_interfaceInfoPtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_DXCH_TTI_MAC_VLAN_STC_PTR_maskPtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_DXCH_TTI_MAC_VLAN_STC_PTR_valuePtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_DXCH_TTI_MULTICAST_DUPLICATION_MODE_ENT_mode;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_DXCH_TTI_OFFSET_TYPE_ENT_offsetType;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_DXCH_TTI_PW_CW_EXCEPTION_ENT_exceptionType;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_DXCH_TTI_RULE_TYPE_ENT_ruleType;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_DXCH_TTI_RULE_UNT_PTR_maskPtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_DXCH_TTI_RULE_UNT_PTR_patternPtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_DXCH_TTI_TRILL_ADJACENCY_STC_PTR_maskPtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_DXCH_TTI_TRILL_ADJACENCY_STC_PTR_valuePtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_ETHER_MODE_ENT_mode;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_IP_PROTOCOL_STACK_ENT_ipType;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_NET_RX_CPU_CODE_ENT_code;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_TUNNEL_ETHERTYPE_TYPE_ENT_ethertypeType;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_TUNNEL_MULTICAST_TYPE_ENT_protocol;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_ETHERADDR_PTR_addressPtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_ETHERADDR_PTR_maskPtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_U16_udpPort;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_U32_cpuCodeBase;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_U32_ethertype;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_U32_extensionHeaderId;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_U32_extensionHeaderValue;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_U32_lookup;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_U32_maxTrillVersion;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_U32_overrideBitmap;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_U32_pclId;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_CPSS_DXCH_TTI_ACTION_UNT_PTR_actionPtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_CPSS_DXCH_TTI_KEY_SIZE_ENT_PTR_sizePtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_CPSS_DXCH_TTI_KEY_TCAM_SEGMENT_MODE_ENT_PTR_segmentModePtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_CPSS_DXCH_TTI_MAC_MODE_ENT_PTR_macModePtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_CPSS_DXCH_TTI_MAC_TO_ME_SRC_INTERFACE_INFO_STC_PTR_interfaceInfoPtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_CPSS_DXCH_TTI_MAC_VLAN_STC_PTR_maskPtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_CPSS_DXCH_TTI_MAC_VLAN_STC_PTR_valuePtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_CPSS_DXCH_TTI_MULTICAST_DUPLICATION_MODE_ENT_PTR_modePtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_CPSS_DXCH_TTI_OFFSET_TYPE_ENT_PTR_offsetTypePtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_CPSS_DXCH_TTI_RULE_UNT_PTR_maskPtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_CPSS_DXCH_TTI_RULE_UNT_PTR_patternPtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_CPSS_DXCH_TTI_TRILL_ADJACENCY_STC_PTR_maskPtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_CPSS_DXCH_TTI_TRILL_ADJACENCY_STC_PTR_valuePtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_CPSS_ETHER_MODE_ENT_PTR_modePtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_CPSS_NET_RX_CPU_CODE_ENT_PTR_codePtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_ETHERADDR_PTR_addressPtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_ETHERADDR_PTR_maskPtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_U16_PTR_udpPortPtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_U32_PTR_cpuCodeBasePtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_U32_PTR_ethertypePtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_U32_PTR_extensionHeaderValuePtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_U32_PTR_maxTrillVersionPtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_U32_PTR_overrideBitmapPtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_U32_PTR_pclIdPtr;


/********* lib API DB *********/

enum {
    PRV_CPSS_LOG_FUNC_cpssDxChTtiMacToMeSet_E = (CPSS_LOG_LIB_TTI_E << 16),
    PRV_CPSS_LOG_FUNC_cpssDxChTtiMacToMeGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChTtiPortLookupEnableSet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChTtiPortLookupEnableGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChTtiPortIpv4OnlyTunneledEnableSet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChTtiPortIpv4OnlyTunneledEnableGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChTtiPortIpv4OnlyMacToMeEnableSet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChTtiPortIpv4OnlyMacToMeEnableGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChTtiIpv4McEnableSet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChTtiIpv4McEnableGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChTtiPortMplsOnlyMacToMeEnableSet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChTtiPortMplsOnlyMacToMeEnableGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChTtiPortMimOnlyMacToMeEnableSet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChTtiPortMimOnlyMacToMeEnableGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChTtiRuleSet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChTtiRuleGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChTtiRuleActionUpdate_E,
    PRV_CPSS_LOG_FUNC_cpssDxChTtiRuleValidStatusSet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChTtiRuleValidStatusGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChTtiMacModeSet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChTtiMacModeGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChTtiPclIdSet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChTtiPclIdGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChTtiExceptionCmdSet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChTtiExceptionCmdGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChTtiExceptionCpuCodeSet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChTtiExceptionCpuCodeGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChTtiBypassHeaderLengthCheckInIpv4TtiHeaderExceptionEnableSet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChTtiBypassHeaderLengthCheckInIpv4TtiHeaderExceptionEnableGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChTtiPortIpTotalLengthDeductionEnableSet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChTtiPortIpTotalLengthDeductionEnableGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChTtiIpTotalLengthDeductionValueSet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChTtiIpTotalLengthDeductionValueGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChTtiEthernetTypeSet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChTtiEthernetTypeGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChTtiTrillCpuCodeBaseSet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChTtiTrillCpuCodeBaseGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChTtiTrillAdjacencyCheckEntrySet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChTtiTrillAdjacencyCheckEntryGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChTtiTrillMaxVersionSet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChTtiTrillMaxVersionGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChTtiPortTrillEnableSet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChTtiPortTrillEnableGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChTtiPortTrillOuterVid0Set_E,
    PRV_CPSS_LOG_FUNC_cpssDxChTtiPortTrillOuterVid0Get_E,
    PRV_CPSS_LOG_FUNC_cpssDxChTtiPortGroupMacToMeSet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChTtiPortGroupMacToMeGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChTtiPortGroupMacModeSet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChTtiPortGroupMacModeGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChTtiPortGroupRuleSet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChTtiPortGroupRuleGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChTtiPortGroupRuleActionUpdate_E,
    PRV_CPSS_LOG_FUNC_cpssDxChTtiPortGroupRuleValidStatusSet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChTtiPortGroupRuleValidStatusGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChTtiPortGroupUserDefinedByteSet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChTtiPortGroupUserDefinedByteGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChTtiPortPassengerOuterIsTag0Or1Set_E,
    PRV_CPSS_LOG_FUNC_cpssDxChTtiPortPassengerOuterIsTag0Or1Get_E,
    PRV_CPSS_LOG_FUNC_cpssDxChTtiPacketTypeKeySizeSet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChTtiPacketTypeKeySizeGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChTtiTcamSegmentModeSet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChTtiTcamSegmentModeGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChTtiMcTunnelDuplicationModeSet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChTtiMcTunnelDuplicationModeGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChTtiMcTunnelDuplicationUdpDestPortSet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChTtiMcTunnelDuplicationUdpDestPortGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChTtiMplsMcTunnelTriggeringMacDaSet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChTtiMplsMcTunnelTriggeringMacDaGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChTtiPwCwExceptionCmdSet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChTtiPwCwExceptionCmdGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChTtiPwCwCpuCodeBaseSet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChTtiPwCwCpuCodeBaseGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChTtiPwCwSequencingSupportEnableSet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChTtiPwCwSequencingSupportEnableGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChTtiIPv6ExtensionHeaderSet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChTtiIPv6ExtensionHeaderGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChTtiSourceIdBitsOverrideSet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChTtiSourceIdBitsOverrideGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChTtiGreExtensionsEnableSet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChTtiGreExtensionsEnableGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChTtiUserDefinedByteSet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChTtiUserDefinedByteGet_E
};

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif /* __prvCpssDxChTtiLogh */
