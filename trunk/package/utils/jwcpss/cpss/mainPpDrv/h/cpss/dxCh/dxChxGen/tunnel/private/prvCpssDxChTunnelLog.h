/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* prvCpssDxChTunnelLog.h
*       WARNING!!! this is a generated file, please don't edit it manually
* COMMENTS:
*
* FILE REVISION NUMBER:
*       $Revision: 1 $
*******************************************************************************/
#ifndef __prvCpssDxChTunnelLogh
#define __prvCpssDxChTunnelLogh
#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
#include <cpss/generic/log/cpssLog.h>
#include <cpss/generic/log/prvCpssLog.h>


/********* manually implemented declarations *********/

void prvCpssLogParamFuncStc_CPSS_DXCH_TUNNEL_START_CONFIG_UNT_PTR
(
    IN CPSS_LOG_LIB_ENT         contextLib,
    IN CPSS_LOG_TYPE_ENT        logType,
    IN GT_CHAR_PTR              namePtr,
    IN void                   * fieldPtr,
    INOUT PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC     * inOutParamInfoPtr
);
void prvCpssLogParamFuncStc_CPSS_DXCH_TUNNEL_TERM_CONFIG_UNT_PTR
(
    IN CPSS_LOG_LIB_ENT         contextLib,
    IN CPSS_LOG_TYPE_ENT        logType,
    IN GT_CHAR_PTR              namePtr,
    IN void                   * fieldPtr,
    INOUT PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC     * inOutParamInfoPtr
);


/********* enums *********/

PRV_CPSS_LOG_STC_ENUM_DECLARE_MAC(CPSS_DXCH_TUNNEL_START_ETHERNET_OVER_X_TAG_MODE_ENT);
PRV_CPSS_LOG_STC_ENUM_DECLARE_MAC(CPSS_DXCH_TUNNEL_TERM_EXCEPTION_ENT);
PRV_CPSS_LOG_STC_ENUM_DECLARE_MAC(CPSS_DXCH_TUNNEL_PASSANGER_TYPE_ENT);
PRV_CPSS_LOG_STC_ENUM_DECLARE_MAC(CPSS_DXCH_TUNNEL_QOS_MODE_ENT);
PRV_CPSS_LOG_STC_ENUM_DECLARE_MAC(CPSS_DXCH_TUNNEL_START_TEMPLATE_DATA_CFG_ENT);
PRV_CPSS_LOG_STC_ENUM_DECLARE_MAC(CPSS_DXCH_TUNNEL_START_TEMPLATE_DATA_SIZE_ENT);
PRV_CPSS_LOG_STC_ENUM_DECLARE_MAC(CPSS_DXCH_TUNNEL_START_UDP_SRC_PORT_ASSIGN_MODE_ENT);
PRV_CPSS_LOG_STC_ENUM_DECLARE_MAC(CPSS_DXCH_TUNNEL_START_FLOW_LABEL_ASSIGN_MODE_ENT);
PRV_CPSS_LOG_STC_ENUM_DECLARE_MAC(CPSS_DXCH_TUNNEL_START_IP_HEADER_PROTOCOL_ENT);
PRV_CPSS_LOG_STC_ENUM_DECLARE_MAC(CPSS_DXCH_TUNNEL_START_MIM_B_DA_ASSIGN_MODE_ENT);
PRV_CPSS_LOG_STC_ENUM_DECLARE_MAC(CPSS_DXCH_TUNNEL_START_MIM_I_SID_ASSIGN_MODE_ENT);
PRV_CPSS_LOG_STC_ENUM_DECLARE_MAC(CPSS_DXCH_TUNNEL_START_MPLS_ETHER_TYPE_ENT);
PRV_CPSS_LOG_STC_ENUM_DECLARE_MAC(CPSS_DXCH_TUNNEL_START_QOS_MARK_MODE_ENT);
PRV_CPSS_LOG_STC_ENUM_DECLARE_MAC(CPSS_DXCH_TUNNEL_START_TTL_MODE_ENT);


/********* structure fields log functions *********/

void prvCpssLogParamFuncStc_CPSS_DXCH_TUNNEL_START_GEN_IP_PROFILE_STC_PTR
(
    IN CPSS_LOG_LIB_ENT         contextLib,
    IN CPSS_LOG_TYPE_ENT        logType,
    IN GT_CHAR_PTR              namePtr,
    IN void                   * fieldPtr,
    INOUT PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC     * inOutParamInfoPtr
);
void prvCpssLogParamFuncStc_CPSS_DXCH_TUNNEL_TERM_ACTION_STC_PTR
(
    IN CPSS_LOG_LIB_ENT         contextLib,
    IN CPSS_LOG_TYPE_ENT        logType,
    IN GT_CHAR_PTR              namePtr,
    IN void                   * fieldPtr,
    INOUT PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC     * inOutParamInfoPtr
);
void prvCpssLogParamFuncStc_CPSS_DXCH_TUNNEL_START_IPV4_CONFIG_STC_PTR
(
    IN CPSS_LOG_LIB_ENT         contextLib,
    IN CPSS_LOG_TYPE_ENT        logType,
    IN GT_CHAR_PTR              namePtr,
    IN void                   * fieldPtr,
    INOUT PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC     * inOutParamInfoPtr
);
void prvCpssLogParamFuncStc_CPSS_DXCH_TUNNEL_START_IPV6_CONFIG_STC_PTR
(
    IN CPSS_LOG_LIB_ENT         contextLib,
    IN CPSS_LOG_TYPE_ENT        logType,
    IN GT_CHAR_PTR              namePtr,
    IN void                   * fieldPtr,
    INOUT PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC     * inOutParamInfoPtr
);
void prvCpssLogParamFuncStc_CPSS_DXCH_TUNNEL_START_MIM_CONFIG_STC_PTR
(
    IN CPSS_LOG_LIB_ENT         contextLib,
    IN CPSS_LOG_TYPE_ENT        logType,
    IN GT_CHAR_PTR              namePtr,
    IN void                   * fieldPtr,
    INOUT PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC     * inOutParamInfoPtr
);
void prvCpssLogParamFuncStc_CPSS_DXCH_TUNNEL_START_MPLS_CONFIG_STC_PTR
(
    IN CPSS_LOG_LIB_ENT         contextLib,
    IN CPSS_LOG_TYPE_ENT        logType,
    IN GT_CHAR_PTR              namePtr,
    IN void                   * fieldPtr,
    INOUT PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC     * inOutParamInfoPtr
);
void prvCpssLogParamFuncStc_CPSS_DXCH_TUNNEL_START_TEMPLATE_DATA_CFG_ENT_PTR
(
    IN CPSS_LOG_LIB_ENT         contextLib,
    IN CPSS_LOG_TYPE_ENT        logType,
    IN GT_CHAR_PTR              namePtr,
    IN void                   * fieldPtr,
    INOUT PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC     * inOutParamInfoPtr
);
void prvCpssLogParamFuncStc_CPSS_DXCH_TUNNEL_START_TRILL_CONFIG_STC_PTR
(
    IN CPSS_LOG_LIB_ENT         contextLib,
    IN CPSS_LOG_TYPE_ENT        logType,
    IN GT_CHAR_PTR              namePtr,
    IN void                   * fieldPtr,
    INOUT PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC     * inOutParamInfoPtr
);
void prvCpssLogParamFuncStc_CPSS_DXCH_TUNNEL_TERM_IPV4_CONFIG_STC_PTR
(
    IN CPSS_LOG_LIB_ENT         contextLib,
    IN CPSS_LOG_TYPE_ENT        logType,
    IN GT_CHAR_PTR              namePtr,
    IN void                   * fieldPtr,
    INOUT PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC     * inOutParamInfoPtr
);
void prvCpssLogParamFuncStc_CPSS_DXCH_TUNNEL_TERM_MPLS_CONFIG_STC_PTR
(
    IN CPSS_LOG_LIB_ENT         contextLib,
    IN CPSS_LOG_TYPE_ENT        logType,
    IN GT_CHAR_PTR              namePtr,
    IN void                   * fieldPtr,
    INOUT PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC     * inOutParamInfoPtr
);
void prvCpssLogParamFuncStc_CPSS_DXCH_TUNNEL_START_TRILL_HEADER_STC_PTR
(
    IN CPSS_LOG_LIB_ENT         contextLib,
    IN CPSS_LOG_TYPE_ENT        logType,
    IN GT_CHAR_PTR              namePtr,
    IN void                   * fieldPtr,
    INOUT PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC     * inOutParamInfoPtr
);


/********* parameters log functions *********/

void prvCpssLogParamFunc_CPSS_DXCH_TUNNEL_START_CONFIG_UNT_PTR(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
);
void prvCpssLogParamFunc_CPSS_DXCH_TUNNEL_START_ETHERNET_OVER_X_TAG_MODE_ENT(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
);
void prvCpssLogParamFunc_CPSS_DXCH_TUNNEL_START_ETHERNET_OVER_X_TAG_MODE_ENT_PTR(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
);
void prvCpssLogParamFunc_CPSS_DXCH_TUNNEL_START_GEN_IP_PROFILE_STC_PTR(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
);
void prvCpssLogParamFunc_CPSS_DXCH_TUNNEL_TERM_ACTION_STC_PTR(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
);
void prvCpssLogParamFunc_CPSS_DXCH_TUNNEL_TERM_CONFIG_UNT_PTR(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
);
void prvCpssLogParamFunc_CPSS_DXCH_TUNNEL_TERM_EXCEPTION_ENT(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
);


/********* API fields DB *********/

extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_DXCH_TUNNEL_START_CONFIG_UNT_PTR_configPtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_DXCH_TUNNEL_START_ETHERNET_OVER_X_TAG_MODE_ENT_tagMode;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_DXCH_TUNNEL_START_GEN_IP_PROFILE_STC_PTR_profileDataPtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_DXCH_TUNNEL_TERM_ACTION_STC_PTR_actionPtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_DXCH_TUNNEL_TERM_CONFIG_UNT_PTR_configMaskPtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_DXCH_TUNNEL_TERM_CONFIG_UNT_PTR_configPtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_DXCH_TUNNEL_TERM_EXCEPTION_ENT_exceptionType;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_TUNNEL_TYPE_ENT_tunnelType;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_U32_additionToLength;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_U32_label;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_U32_routerArpTunnelStartLineIndex;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_U32_routerTunnelTermTcamIndex;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_U32_tsExtension;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_U32_ttl;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_U32_vlanServiceId;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_U8_ttl;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_CPSS_DXCH_TUNNEL_START_CONFIG_UNT_PTR_configPtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_CPSS_DXCH_TUNNEL_START_ETHERNET_OVER_X_TAG_MODE_ENT_PTR_tagModePtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_CPSS_DXCH_TUNNEL_START_GEN_IP_PROFILE_STC_PTR_profileDataPtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_CPSS_DXCH_TUNNEL_TERM_ACTION_STC_PTR_actionPtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_CPSS_DXCH_TUNNEL_TERM_CONFIG_UNT_PTR_configMaskPtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_CPSS_DXCH_TUNNEL_TERM_CONFIG_UNT_PTR_configPtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_CPSS_TUNNEL_TYPE_ENT_PTR_tunnelTypePtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_U32_PTR_additionToLengthPtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_U32_PTR_labelPtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_U32_PTR_tsExtensionPtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_U32_PTR_ttlPtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_U32_PTR_vlanServiceIdPtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_U8_PTR_ttlPtr;


/********* lib API DB *********/

enum {
    PRV_CPSS_LOG_FUNC_cpssDxChTunnelStartEntrySet_E = (CPSS_LOG_LIB_TUNNEL_E << 16),
    PRV_CPSS_LOG_FUNC_cpssDxChTunnelStartPortGroupGenProfileTableEntrySet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChTunnelStartGenProfileTableEntrySet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChTunnelStartPortGroupGenProfileTableEntryGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChTunnelStartGenProfileTableEntryGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChTunnelStartEntryGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChTunnelStartPortGroupEntrySet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChTunnelStartPortGroupEntryGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChIpv4TunnelTermPortSet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChIpv4TunnelTermPortGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChMplsTunnelTermPortSet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChMplsTunnelTermPortGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChTunnelTermEntrySet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChTunnelTermEntryGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChTunnelTermEntryInvalidate_E,
    PRV_CPSS_LOG_FUNC_cpssDxChTunnelStartEgressFilteringSet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChTunnelStartEgressFilteringGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChEthernetOverMplsTunnelStartTaggingSet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChEthernetOverMplsTunnelStartTaggingGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChEthernetOverMplsTunnelStartTagModeSet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChEthernetOverMplsTunnelStartTagModeGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChTunnelStartPassengerVlanTranslationEnableSet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChTunnelStartPassengerVlanTranslationEnableGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChTunnelTermExceptionCmdSet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChTunnelCtrlTtExceptionCmdGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChTunnelStartPortIpTunnelTotalLengthOffsetEnableSet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChTunnelStartPortIpTunnelTotalLengthOffsetEnableGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChTunnelStartIpTunnelTotalLengthOffsetSet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChTunnelStartIpTunnelTotalLengthOffsetGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChTunnelStartMplsPwLabelPushEnableSet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChTunnelStartMplsPwLabelPushEnableGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChTunnelStartMplsPwLabelSet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChTunnelStartMplsPwLabelGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChTunnelStartMplsFlowLabelEnableSet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChTunnelStartMplsFlowLabelEnableGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChTunnelStartMplsFlowLabelTtlSet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChTunnelStartMplsFlowLabelTtlGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChTunnelStartMplsPwLabelExpSet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChTunnelStartMplsPwLabelExpGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChTunnelStartMplsPwLabelTtlSet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChTunnelStartMplsPwLabelTtlGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChTunnelStartMplsPwControlWordSet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChTunnelStartMplsPwControlWordGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChTunnelStartMplsPwETreeEnableSet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChTunnelStartMplsPwETreeEnableGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChTunnelStartHeaderTpidSelectSet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChTunnelStartHeaderTpidSelectGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChTunnelStartEntryExtensionSet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChTunnelStartEntryExtensionGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChTunnelStartEgessVlanTableServiceIdSet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChTunnelStartEgessVlanTableServiceIdGet_E
};

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif /* __prvCpssDxChTunnelLogh */
