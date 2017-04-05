/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* prvCpssDxChPclLog.h
*       WARNING!!! this is a generated file, please don't edit it manually
* COMMENTS:
*
* FILE REVISION NUMBER:
*       $Revision: 1 $
*******************************************************************************/
#ifndef __prvCpssDxChPclLogh
#define __prvCpssDxChPclLogh
#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
#include <cpss/generic/log/cpssLog.h>
#include <cpss/generic/log/prvCpssLog.h>


/********* manually implemented declarations *********/

void prvCpssLogParamFuncStc_CPSS_DXCH_PCL_ACTION_STC_PTR
(
    IN CPSS_LOG_LIB_ENT         contextLib,
    IN CPSS_LOG_TYPE_ENT        logType,
    IN GT_CHAR_PTR              namePtr,
    IN void                   * fieldPtr,
    INOUT PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC     * inOutParamInfoPtr
);
void prvCpssLogParamFuncStc_CPSS_DXCH_PCL_RULE_FORMAT_UNT_PTR
(
    IN CPSS_LOG_LIB_ENT         contextLib,
    IN CPSS_LOG_TYPE_ENT        logType,
    IN GT_CHAR_PTR              namePtr,
    IN void                   * fieldPtr,
    INOUT PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC     * inOutParamInfoPtr
);
void prvCpssLogParamFuncStc_CPSS_DXCH_PCL_ACTION_QOS_UNT_PTR
(
    IN CPSS_LOG_LIB_ENT         contextLib,
    IN CPSS_LOG_TYPE_ENT        logType,
    IN GT_CHAR_PTR              namePtr,
    IN void                   * fieldPtr,
    INOUT PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC     * inOutParamInfoPtr
);
void prvCpssLogParamFuncStc_CPSS_DXCH_PCL_ACTION_REDIRECT_STC_PTR
(
    IN CPSS_LOG_LIB_ENT         contextLib,
    IN CPSS_LOG_TYPE_ENT        logType,
    IN GT_CHAR_PTR              namePtr,
    IN void                   * fieldPtr,
    INOUT PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC     * inOutParamInfoPtr
);
void prvCpssLogParamFuncStc_CPSS_DXCH_PCL_ACTION_VLAN_UNT_PTR
(
    IN CPSS_LOG_LIB_ENT         contextLib,
    IN CPSS_LOG_TYPE_ENT        logType,
    IN GT_CHAR_PTR              namePtr,
    IN void                   * fieldPtr,
    INOUT PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC     * inOutParamInfoPtr
);


/********* enums *********/

PRV_CPSS_LOG_STC_ENUM_DECLARE_MAC(CPSS_DXCH_PCL_EGRESS_KEY_VID_UP_MODE_ENT);
PRV_CPSS_LOG_STC_ENUM_DECLARE_MAC(CPSS_DXCH_PCL_EGRESS_PKT_TYPE_ENT);
PRV_CPSS_LOG_STC_ENUM_DECLARE_MAC(CPSS_DXCH_PCL_EGRESS_SOURCE_PORT_SELECTION_MODE_ENT);
PRV_CPSS_LOG_STC_ENUM_DECLARE_MAC(CPSS_DXCH_PCL_EGRESS_TARGET_PORT_SELECTION_MODE_ENT);
PRV_CPSS_LOG_STC_ENUM_DECLARE_MAC(CPSS_DXCH_PCL_IPCL01_LOOKUP_TYPE_ENT);
PRV_CPSS_LOG_STC_ENUM_DECLARE_MAC(CPSS_DXCH_PCL_IP_LENGTH_CHECK_MODE_ENT);
PRV_CPSS_LOG_STC_ENUM_DECLARE_MAC(CPSS_DXCH_PCL_OAM_EGRESS_DETECT_MODE_ENT);
PRV_CPSS_LOG_STC_ENUM_DECLARE_MAC(CPSS_DXCH_PCL_OAM_PKT_MEG_LEVEL_MODE_ENT);
PRV_CPSS_LOG_STC_ENUM_DECLARE_MAC(CPSS_DXCH_PCL_OFFSET_TYPE_ENT);
PRV_CPSS_LOG_STC_ENUM_DECLARE_MAC(CPSS_DXCH_PCL_PACKET_TYPE_ENT);
PRV_CPSS_LOG_STC_ENUM_DECLARE_MAC(CPSS_DXCH_PCL_PORT_LOOKUP_CFG_TAB_ACC_MODE_ENT);
PRV_CPSS_LOG_STC_ENUM_DECLARE_MAC(CPSS_DXCH_PCL_RULE_FORMAT_TYPE_ENT);
PRV_CPSS_LOG_STC_ENUM_MAP_DECLARE_MAC(CPSS_DXCH_PCL_RULE_OPTION_ENT);
PRV_CPSS_LOG_STC_ENUM_DECLARE_MAC(CPSS_DXCH_PCL_UDB_OVERRIDE_TYPE_ENT);
PRV_CPSS_LOG_STC_ENUM_DECLARE_MAC(CPSS_DXCH_UDB_ERROR_CMD_ENT);
PRV_CPSS_LOG_STC_ENUM_DECLARE_MAC(CPSS_DXCH_PCL_CFG_TBL_ACCESS_MODE_ENT);
PRV_CPSS_LOG_STC_ENUM_DECLARE_MAC(CPSS_DXCH_PCL_CFG_TBL_DEV_PORT_BASE_ENT);
PRV_CPSS_LOG_STC_ENUM_DECLARE_MAC(CPSS_DXCH_PCL_CFG_TBL_MAX_DEV_PORTS_ENT);
PRV_CPSS_LOG_STC_ENUM_DECLARE_MAC(CPSS_DXCH_PCL_TCAM_SEGMENT_MODE_ENT);
PRV_CPSS_LOG_STC_ENUM_DECLARE_MAC(CPSS_DXCH_PCL_ACTION_EGRESS_DSCP_EXP_CMD_ENT);
PRV_CPSS_LOG_STC_ENUM_DECLARE_MAC(CPSS_DXCH_PCL_ACTION_EGRESS_TAG0_CMD_ENT);
PRV_CPSS_LOG_STC_ENUM_DECLARE_MAC(CPSS_DXCH_PCL_ACTION_INGRESS_UP1_CMD_ENT);
PRV_CPSS_LOG_STC_ENUM_DECLARE_MAC(CPSS_DXCH_PCL_ACTION_INGRESS_VLAN_ID1_CMD_ENT);
PRV_CPSS_LOG_STC_ENUM_DECLARE_MAC(CPSS_DXCH_PCL_ACTION_REDIRECT_CMD_ENT);
PRV_CPSS_LOG_STC_ENUM_DECLARE_MAC(CPSS_DXCH_PCL_ACTION_REDIRECT_TUNNEL_TYPE_ENT);
PRV_CPSS_LOG_STC_ENUM_DECLARE_MAC(CPSS_DXCH_PCL_LOOKUP_CONFIG_INDEX_ENT);
PRV_CPSS_LOG_STC_ENUM_DECLARE_MAC(CPSS_DXCH_PCL_POLICER_ENABLE_ENT);


/********* structure fields log functions *********/

void prvCpssLogParamFuncStc_CPSS_DXCH_PCL_CFG_TBL_ACCESS_MODE_STC_PTR
(
    IN CPSS_LOG_LIB_ENT         contextLib,
    IN CPSS_LOG_TYPE_ENT        logType,
    IN GT_CHAR_PTR              namePtr,
    IN void                   * fieldPtr,
    INOUT PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC     * inOutParamInfoPtr
);
void prvCpssLogParamFuncStc_CPSS_DXCH_PCL_LOOKUP_CFG_STC_PTR
(
    IN CPSS_LOG_LIB_ENT         contextLib,
    IN CPSS_LOG_TYPE_ENT        logType,
    IN GT_CHAR_PTR              namePtr,
    IN void                   * fieldPtr,
    INOUT PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC     * inOutParamInfoPtr
);
void prvCpssLogParamFuncStc_CPSS_DXCH_PCL_UDB_SELECT_STC_PTR
(
    IN CPSS_LOG_LIB_ENT         contextLib,
    IN CPSS_LOG_TYPE_ENT        logType,
    IN GT_CHAR_PTR              namePtr,
    IN void                   * fieldPtr,
    INOUT PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC     * inOutParamInfoPtr
);
void prvCpssLogParamFuncStc_CPSS_DXCH_PCL_ACTION_IP_UC_ROUTE_STC_PTR
(
    IN CPSS_LOG_LIB_ENT         contextLib,
    IN CPSS_LOG_TYPE_ENT        logType,
    IN GT_CHAR_PTR              namePtr,
    IN void                   * fieldPtr,
    INOUT PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC     * inOutParamInfoPtr
);
void prvCpssLogParamFuncStc_CPSS_DXCH_PCL_ACTION_LOOKUP_CONFIG_STC_PTR
(
    IN CPSS_LOG_LIB_ENT         contextLib,
    IN CPSS_LOG_TYPE_ENT        logType,
    IN GT_CHAR_PTR              namePtr,
    IN void                   * fieldPtr,
    INOUT PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC     * inOutParamInfoPtr
);
void prvCpssLogParamFuncStc_CPSS_DXCH_PCL_ACTION_MATCH_COUNTER_STC_PTR
(
    IN CPSS_LOG_LIB_ENT         contextLib,
    IN CPSS_LOG_TYPE_ENT        logType,
    IN GT_CHAR_PTR              namePtr,
    IN void                   * fieldPtr,
    INOUT PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC     * inOutParamInfoPtr
);
void prvCpssLogParamFuncStc_CPSS_DXCH_PCL_ACTION_MIRROR_STC_PTR
(
    IN CPSS_LOG_LIB_ENT         contextLib,
    IN CPSS_LOG_TYPE_ENT        logType,
    IN GT_CHAR_PTR              namePtr,
    IN void                   * fieldPtr,
    INOUT PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC     * inOutParamInfoPtr
);
void prvCpssLogParamFuncStc_CPSS_DXCH_PCL_ACTION_OAM_STC_PTR
(
    IN CPSS_LOG_LIB_ENT         contextLib,
    IN CPSS_LOG_TYPE_ENT        logType,
    IN GT_CHAR_PTR              namePtr,
    IN void                   * fieldPtr,
    INOUT PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC     * inOutParamInfoPtr
);
void prvCpssLogParamFuncStc_CPSS_DXCH_PCL_ACTION_POLICER_STC_PTR
(
    IN CPSS_LOG_LIB_ENT         contextLib,
    IN CPSS_LOG_TYPE_ENT        logType,
    IN GT_CHAR_PTR              namePtr,
    IN void                   * fieldPtr,
    INOUT PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC     * inOutParamInfoPtr
);
void prvCpssLogParamFuncStc_CPSS_DXCH_PCL_ACTION_SOURCE_ID_STC_PTR
(
    IN CPSS_LOG_LIB_ENT         contextLib,
    IN CPSS_LOG_TYPE_ENT        logType,
    IN GT_CHAR_PTR              namePtr,
    IN void                   * fieldPtr,
    INOUT PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC     * inOutParamInfoPtr
);
void prvCpssLogParamFuncStc_CPSS_DXCH_PCL_ACTION_SOURCE_PORT_STC_PTR
(
    IN CPSS_LOG_LIB_ENT         contextLib,
    IN CPSS_LOG_TYPE_ENT        logType,
    IN GT_CHAR_PTR              namePtr,
    IN void                   * fieldPtr,
    INOUT PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC     * inOutParamInfoPtr
);
void prvCpssLogParamFuncStc_CPSS_DXCH_PCL_RULE_FORMAT_EGRESS_EXT_IPV4_RACL_VACL_STC_PTR
(
    IN CPSS_LOG_LIB_ENT         contextLib,
    IN CPSS_LOG_TYPE_ENT        logType,
    IN GT_CHAR_PTR              namePtr,
    IN void                   * fieldPtr,
    INOUT PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC     * inOutParamInfoPtr
);
void prvCpssLogParamFuncStc_CPSS_DXCH_PCL_RULE_FORMAT_EGRESS_EXT_IPV6_L2_STC_PTR
(
    IN CPSS_LOG_LIB_ENT         contextLib,
    IN CPSS_LOG_TYPE_ENT        logType,
    IN GT_CHAR_PTR              namePtr,
    IN void                   * fieldPtr,
    INOUT PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC     * inOutParamInfoPtr
);
void prvCpssLogParamFuncStc_CPSS_DXCH_PCL_RULE_FORMAT_EGRESS_EXT_IPV6_L4_STC_PTR
(
    IN CPSS_LOG_LIB_ENT         contextLib,
    IN CPSS_LOG_TYPE_ENT        logType,
    IN GT_CHAR_PTR              namePtr,
    IN void                   * fieldPtr,
    INOUT PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC     * inOutParamInfoPtr
);
void prvCpssLogParamFuncStc_CPSS_DXCH_PCL_RULE_FORMAT_EGRESS_EXT_NOT_IPV6_STC_PTR
(
    IN CPSS_LOG_LIB_ENT         contextLib,
    IN CPSS_LOG_TYPE_ENT        logType,
    IN GT_CHAR_PTR              namePtr,
    IN void                   * fieldPtr,
    INOUT PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC     * inOutParamInfoPtr
);
void prvCpssLogParamFuncStc_CPSS_DXCH_PCL_RULE_FORMAT_EGRESS_STD_IPV4_L4_STC_PTR
(
    IN CPSS_LOG_LIB_ENT         contextLib,
    IN CPSS_LOG_TYPE_ENT        logType,
    IN GT_CHAR_PTR              namePtr,
    IN void                   * fieldPtr,
    INOUT PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC     * inOutParamInfoPtr
);
void prvCpssLogParamFuncStc_CPSS_DXCH_PCL_RULE_FORMAT_EGRESS_STD_IP_L2_QOS_STC_PTR
(
    IN CPSS_LOG_LIB_ENT         contextLib,
    IN CPSS_LOG_TYPE_ENT        logType,
    IN GT_CHAR_PTR              namePtr,
    IN void                   * fieldPtr,
    INOUT PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC     * inOutParamInfoPtr
);
void prvCpssLogParamFuncStc_CPSS_DXCH_PCL_RULE_FORMAT_EGRESS_STD_NOT_IP_STC_PTR
(
    IN CPSS_LOG_LIB_ENT         contextLib,
    IN CPSS_LOG_TYPE_ENT        logType,
    IN GT_CHAR_PTR              namePtr,
    IN void                   * fieldPtr,
    INOUT PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC     * inOutParamInfoPtr
);
void prvCpssLogParamFuncStc_CPSS_DXCH_PCL_RULE_FORMAT_EGRESS_UDB_ONLY_STC_PTR
(
    IN CPSS_LOG_LIB_ENT         contextLib,
    IN CPSS_LOG_TYPE_ENT        logType,
    IN GT_CHAR_PTR              namePtr,
    IN void                   * fieldPtr,
    INOUT PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC     * inOutParamInfoPtr
);
void prvCpssLogParamFuncStc_CPSS_DXCH_PCL_RULE_FORMAT_EGRESS_ULTRA_IPV6_RACL_VACL_STC_PTR
(
    IN CPSS_LOG_LIB_ENT         contextLib,
    IN CPSS_LOG_TYPE_ENT        logType,
    IN GT_CHAR_PTR              namePtr,
    IN void                   * fieldPtr,
    INOUT PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC     * inOutParamInfoPtr
);
void prvCpssLogParamFuncStc_CPSS_DXCH_PCL_RULE_FORMAT_EXT_IPV4_PORT_VLAN_QOS_STC_PTR
(
    IN CPSS_LOG_LIB_ENT         contextLib,
    IN CPSS_LOG_TYPE_ENT        logType,
    IN GT_CHAR_PTR              namePtr,
    IN void                   * fieldPtr,
    INOUT PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC     * inOutParamInfoPtr
);
void prvCpssLogParamFuncStc_CPSS_DXCH_PCL_RULE_FORMAT_EXT_IPV6_L2_STC_PTR
(
    IN CPSS_LOG_LIB_ENT         contextLib,
    IN CPSS_LOG_TYPE_ENT        logType,
    IN GT_CHAR_PTR              namePtr,
    IN void                   * fieldPtr,
    INOUT PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC     * inOutParamInfoPtr
);
void prvCpssLogParamFuncStc_CPSS_DXCH_PCL_RULE_FORMAT_EXT_IPV6_L4_STC_PTR
(
    IN CPSS_LOG_LIB_ENT         contextLib,
    IN CPSS_LOG_TYPE_ENT        logType,
    IN GT_CHAR_PTR              namePtr,
    IN void                   * fieldPtr,
    INOUT PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC     * inOutParamInfoPtr
);
void prvCpssLogParamFuncStc_CPSS_DXCH_PCL_RULE_FORMAT_EXT_NOT_IPV6_STC_PTR
(
    IN CPSS_LOG_LIB_ENT         contextLib,
    IN CPSS_LOG_TYPE_ENT        logType,
    IN GT_CHAR_PTR              namePtr,
    IN void                   * fieldPtr,
    INOUT PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC     * inOutParamInfoPtr
);
void prvCpssLogParamFuncStc_CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_EXTENDED_UDB_STC_PTR
(
    IN CPSS_LOG_LIB_ENT         contextLib,
    IN CPSS_LOG_TYPE_ENT        logType,
    IN GT_CHAR_PTR              namePtr,
    IN void                   * fieldPtr,
    INOUT PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC     * inOutParamInfoPtr
);
void prvCpssLogParamFuncStc_CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_STANDARD_UDB_STC_PTR
(
    IN CPSS_LOG_LIB_ENT         contextLib,
    IN CPSS_LOG_TYPE_ENT        logType,
    IN GT_CHAR_PTR              namePtr,
    IN void                   * fieldPtr,
    INOUT PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC     * inOutParamInfoPtr
);
void prvCpssLogParamFuncStc_CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_UDB_ONLY_STC_PTR
(
    IN CPSS_LOG_LIB_ENT         contextLib,
    IN CPSS_LOG_TYPE_ENT        logType,
    IN GT_CHAR_PTR              namePtr,
    IN void                   * fieldPtr,
    INOUT PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC     * inOutParamInfoPtr
);
void prvCpssLogParamFuncStc_CPSS_DXCH_PCL_RULE_FORMAT_STD_IPV4_L4_STC_PTR
(
    IN CPSS_LOG_LIB_ENT         contextLib,
    IN CPSS_LOG_TYPE_ENT        logType,
    IN GT_CHAR_PTR              namePtr,
    IN void                   * fieldPtr,
    INOUT PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC     * inOutParamInfoPtr
);
void prvCpssLogParamFuncStc_CPSS_DXCH_PCL_RULE_FORMAT_STD_IPV4_ROUTED_ACL_QOS_STC_PTR
(
    IN CPSS_LOG_LIB_ENT         contextLib,
    IN CPSS_LOG_TYPE_ENT        logType,
    IN GT_CHAR_PTR              namePtr,
    IN void                   * fieldPtr,
    INOUT PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC     * inOutParamInfoPtr
);
void prvCpssLogParamFuncStc_CPSS_DXCH_PCL_RULE_FORMAT_STD_IPV6_DIP_STC_PTR
(
    IN CPSS_LOG_LIB_ENT         contextLib,
    IN CPSS_LOG_TYPE_ENT        logType,
    IN GT_CHAR_PTR              namePtr,
    IN void                   * fieldPtr,
    INOUT PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC     * inOutParamInfoPtr
);
void prvCpssLogParamFuncStc_CPSS_DXCH_PCL_RULE_FORMAT_STD_IP_L2_QOS_STC_PTR
(
    IN CPSS_LOG_LIB_ENT         contextLib,
    IN CPSS_LOG_TYPE_ENT        logType,
    IN GT_CHAR_PTR              namePtr,
    IN void                   * fieldPtr,
    INOUT PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC     * inOutParamInfoPtr
);
void prvCpssLogParamFuncStc_CPSS_DXCH_PCL_RULE_FORMAT_STD_NOT_IP_STC_PTR
(
    IN CPSS_LOG_LIB_ENT         contextLib,
    IN CPSS_LOG_TYPE_ENT        logType,
    IN GT_CHAR_PTR              namePtr,
    IN void                   * fieldPtr,
    INOUT PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC     * inOutParamInfoPtr
);
void prvCpssLogParamFuncStc_CPSS_DXCH_PCL_RULE_FORMAT_ULTRA_IPV6_PORT_VLAN_QOS_STC_PTR
(
    IN CPSS_LOG_LIB_ENT         contextLib,
    IN CPSS_LOG_TYPE_ENT        logType,
    IN GT_CHAR_PTR              namePtr,
    IN void                   * fieldPtr,
    INOUT PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC     * inOutParamInfoPtr
);
void prvCpssLogParamFuncStc_CPSS_DXCH_PCL_RULE_FORMAT_ULTRA_IPV6_ROUTED_ACL_QOS_STC_PTR
(
    IN CPSS_LOG_LIB_ENT         contextLib,
    IN CPSS_LOG_TYPE_ENT        logType,
    IN GT_CHAR_PTR              namePtr,
    IN void                   * fieldPtr,
    INOUT PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC     * inOutParamInfoPtr
);
void prvCpssLogParamFuncStc_CPSS_DXCH_PCL_RULE_FORMAT_COMMON_EXT_STC_PTR
(
    IN CPSS_LOG_LIB_ENT         contextLib,
    IN CPSS_LOG_TYPE_ENT        logType,
    IN GT_CHAR_PTR              namePtr,
    IN void                   * fieldPtr,
    INOUT PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC     * inOutParamInfoPtr
);
void prvCpssLogParamFuncStc_CPSS_DXCH_PCL_RULE_FORMAT_COMMON_STC_PTR
(
    IN CPSS_LOG_LIB_ENT         contextLib,
    IN CPSS_LOG_TYPE_ENT        logType,
    IN GT_CHAR_PTR              namePtr,
    IN void                   * fieldPtr,
    INOUT PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC     * inOutParamInfoPtr
);
void prvCpssLogParamFuncStc_CPSS_DXCH_PCL_RULE_FORMAT_COMMON_STD_IP_STC_PTR
(
    IN CPSS_LOG_LIB_ENT         contextLib,
    IN CPSS_LOG_TYPE_ENT        logType,
    IN GT_CHAR_PTR              namePtr,
    IN void                   * fieldPtr,
    INOUT PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC     * inOutParamInfoPtr
);
void prvCpssLogParamFuncStc_CPSS_DXCH_PCL_RULE_FORMAT_EGRESS_COMMON_EXT_STC_PTR
(
    IN CPSS_LOG_LIB_ENT         contextLib,
    IN CPSS_LOG_TYPE_ENT        logType,
    IN GT_CHAR_PTR              namePtr,
    IN void                   * fieldPtr,
    INOUT PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC     * inOutParamInfoPtr
);
void prvCpssLogParamFuncStc_CPSS_DXCH_PCL_RULE_FORMAT_EGRESS_COMMON_STC_PTR
(
    IN CPSS_LOG_LIB_ENT         contextLib,
    IN CPSS_LOG_TYPE_ENT        logType,
    IN GT_CHAR_PTR              namePtr,
    IN void                   * fieldPtr,
    INOUT PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC     * inOutParamInfoPtr
);
void prvCpssLogParamFuncStc_CPSS_DXCH_PCL_RULE_FORMAT_EGRESS_COMMON_STD_IP_STC_PTR
(
    IN CPSS_LOG_LIB_ENT         contextLib,
    IN CPSS_LOG_TYPE_ENT        logType,
    IN GT_CHAR_PTR              namePtr,
    IN void                   * fieldPtr,
    INOUT PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC     * inOutParamInfoPtr
);
void prvCpssLogParamFuncStc_CPSS_DXCH_PCL_RULE_FORMAT_EGRESS_IP_COMMON_STC_PTR
(
    IN CPSS_LOG_LIB_ENT         contextLib,
    IN CPSS_LOG_TYPE_ENT        logType,
    IN GT_CHAR_PTR              namePtr,
    IN void                   * fieldPtr,
    INOUT PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC     * inOutParamInfoPtr
);
void prvCpssLogParamFuncStc_CPSS_DXCH_PCL_RULE_FORMAT_EGRESS_UDB60_FIXED_STC_PTR
(
    IN CPSS_LOG_LIB_ENT         contextLib,
    IN CPSS_LOG_TYPE_ENT        logType,
    IN GT_CHAR_PTR              namePtr,
    IN void                   * fieldPtr,
    INOUT PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC     * inOutParamInfoPtr
);
void prvCpssLogParamFuncStc_CPSS_DXCH_PCL_RULE_FORMAT_EGRESS_UDB_REPLACED_STC_PTR
(
    IN CPSS_LOG_LIB_ENT         contextLib,
    IN CPSS_LOG_TYPE_ENT        logType,
    IN GT_CHAR_PTR              namePtr,
    IN void                   * fieldPtr,
    INOUT PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC     * inOutParamInfoPtr
);
void prvCpssLogParamFuncStc_CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_UDB60_FIXED_STC_PTR
(
    IN CPSS_LOG_LIB_ENT         contextLib,
    IN CPSS_LOG_TYPE_ENT        logType,
    IN GT_CHAR_PTR              namePtr,
    IN void                   * fieldPtr,
    INOUT PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC     * inOutParamInfoPtr
);
void prvCpssLogParamFuncStc_CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_UDB_COMMON_STC_PTR
(
    IN CPSS_LOG_LIB_ENT         contextLib,
    IN CPSS_LOG_TYPE_ENT        logType,
    IN GT_CHAR_PTR              namePtr,
    IN void                   * fieldPtr,
    INOUT PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC     * inOutParamInfoPtr
);
void prvCpssLogParamFuncStc_CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_UDB_REPLACED_STC_PTR
(
    IN CPSS_LOG_LIB_ENT         contextLib,
    IN CPSS_LOG_TYPE_ENT        logType,
    IN GT_CHAR_PTR              namePtr,
    IN void                   * fieldPtr,
    INOUT PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC     * inOutParamInfoPtr
);
void prvCpssLogParamFuncStc_CPSS_DXCH_PCL_RULE_FORMAT_IP_COMMON_STC_PTR
(
    IN CPSS_LOG_LIB_ENT         contextLib,
    IN CPSS_LOG_TYPE_ENT        logType,
    IN GT_CHAR_PTR              namePtr,
    IN void                   * fieldPtr,
    INOUT PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC     * inOutParamInfoPtr
);


/********* parameters log functions *********/

void prvCpssLogParamFunc_CPSS_DXCH_PCL_ACTION_STC_PTR(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
);
void prvCpssLogParamFunc_CPSS_DXCH_PCL_CFG_TBL_ACCESS_MODE_STC_PTR(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
);
void prvCpssLogParamFunc_CPSS_DXCH_PCL_EGRESS_KEY_VID_UP_MODE_ENT(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
);
void prvCpssLogParamFunc_CPSS_DXCH_PCL_EGRESS_KEY_VID_UP_MODE_ENT_PTR(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
);
void prvCpssLogParamFunc_CPSS_DXCH_PCL_EGRESS_PKT_TYPE_ENT(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
);
void prvCpssLogParamFunc_CPSS_DXCH_PCL_EGRESS_SOURCE_PORT_SELECTION_MODE_ENT(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
);
void prvCpssLogParamFunc_CPSS_DXCH_PCL_EGRESS_SOURCE_PORT_SELECTION_MODE_ENT_PTR(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
);
void prvCpssLogParamFunc_CPSS_DXCH_PCL_EGRESS_TARGET_PORT_SELECTION_MODE_ENT(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
);
void prvCpssLogParamFunc_CPSS_DXCH_PCL_EGRESS_TARGET_PORT_SELECTION_MODE_ENT_PTR(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
);
void prvCpssLogParamFunc_CPSS_DXCH_PCL_IPCL01_LOOKUP_TYPE_ENT(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
);
void prvCpssLogParamFunc_CPSS_DXCH_PCL_IPCL01_LOOKUP_TYPE_ENT_PTR(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
);
void prvCpssLogParamFunc_CPSS_DXCH_PCL_IP_LENGTH_CHECK_MODE_ENT(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
);
void prvCpssLogParamFunc_CPSS_DXCH_PCL_IP_LENGTH_CHECK_MODE_ENT_PTR(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
);
void prvCpssLogParamFunc_CPSS_DXCH_PCL_LOOKUP_CFG_STC_PTR(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
);
void prvCpssLogParamFunc_CPSS_DXCH_PCL_OAM_EGRESS_DETECT_MODE_ENT(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
);
void prvCpssLogParamFunc_CPSS_DXCH_PCL_OAM_EGRESS_DETECT_MODE_ENT_PTR(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
);
void prvCpssLogParamFunc_CPSS_DXCH_PCL_OAM_PKT_MEG_LEVEL_MODE_ENT(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
);
void prvCpssLogParamFunc_CPSS_DXCH_PCL_OAM_PKT_MEG_LEVEL_MODE_ENT_PTR(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
);
void prvCpssLogParamFunc_CPSS_DXCH_PCL_OFFSET_TYPE_ENT(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
);
void prvCpssLogParamFunc_CPSS_DXCH_PCL_OFFSET_TYPE_ENT_PTR(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
);
void prvCpssLogParamFunc_CPSS_DXCH_PCL_PACKET_TYPE_ENT(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
);
void prvCpssLogParamFunc_CPSS_DXCH_PCL_PORT_LOOKUP_CFG_TAB_ACC_MODE_ENT(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
);
void prvCpssLogParamFunc_CPSS_DXCH_PCL_PORT_LOOKUP_CFG_TAB_ACC_MODE_ENT_PTR(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
);
void prvCpssLogParamFunc_CPSS_DXCH_PCL_RULE_FORMAT_TYPE_ENT(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
);
void prvCpssLogParamFunc_CPSS_DXCH_PCL_RULE_FORMAT_UNT_PTR(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
);
void prvCpssLogParamFunc_CPSS_DXCH_PCL_RULE_OPTION_ENT(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
);
void prvCpssLogParamFunc_CPSS_DXCH_PCL_UDB_OVERRIDE_TYPE_ENT(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
);
void prvCpssLogParamFunc_CPSS_DXCH_PCL_UDB_SELECT_STC_PTR(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
);
void prvCpssLogParamFunc_CPSS_DXCH_UDB_ERROR_CMD_ENT(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
);
void prvCpssLogParamFunc_CPSS_DXCH_UDB_ERROR_CMD_ENT_PTR(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
);


/********* API fields DB *********/

extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_COMPARE_OPERATOR_ENT_compareOperator;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_DXCH_PCL_ACTION_STC_PTR_actionPtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_DXCH_PCL_CFG_TBL_ACCESS_MODE_STC_PTR_accModePtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_DXCH_PCL_EGRESS_KEY_VID_UP_MODE_ENT_vidUpMode;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_DXCH_PCL_EGRESS_PKT_TYPE_ENT_pktType;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_DXCH_PCL_EGRESS_SOURCE_PORT_SELECTION_MODE_ENT_portSelectionMode;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_DXCH_PCL_EGRESS_TARGET_PORT_SELECTION_MODE_ENT_portSelectionMode;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_DXCH_PCL_IPCL01_LOOKUP_TYPE_ENT_lookupType;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_DXCH_PCL_IP_LENGTH_CHECK_MODE_ENT_mode;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_DXCH_PCL_LOOKUP_CFG_STC_PTR_lookupCfgPtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_DXCH_PCL_OAM_EGRESS_DETECT_MODE_ENT_mode;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_DXCH_PCL_OAM_PKT_MEG_LEVEL_MODE_ENT_megLevelMode;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_DXCH_PCL_OFFSET_TYPE_ENT_offsetType;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_DXCH_PCL_PORT_LOOKUP_CFG_TAB_ACC_MODE_ENT_cfgTabAccMode;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_DXCH_PCL_PORT_LOOKUP_CFG_TAB_ACC_MODE_ENT_mode;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_DXCH_PCL_RULE_FORMAT_TYPE_ENT_ruleFormat;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_DXCH_PCL_RULE_FORMAT_UNT_PTR_maskPtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_DXCH_PCL_RULE_FORMAT_UNT_PTR_patternPtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_DXCH_PCL_RULE_OPTION_ENT_ruleOptionsBmp;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_DXCH_PCL_UDB_OVERRIDE_TYPE_ENT_udbOverrideType;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_DXCH_PCL_UDB_SELECT_STC_PTR_udbSelectPtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_DXCH_UDB_ERROR_CMD_ENT_udbErrorCmd;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_INTERFACE_INFO_STC_PTR_interfaceInfoPtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_L4_PROTOCOL_ENT_l4Protocol;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_L4_PROTOCOL_PORT_TYPE_ENT_l4PortType;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_PCL_DIRECTION_ENT_direction;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_PCL_LOOKUP_NUMBER_ENT_lookupNum;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_PCL_RULE_SIZE_ENT_ruleSize;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_BOOL_RBitAssignmentEnable;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_BOOL_enalbe;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_BOOL_mixedRuleSizeMode;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_U16_value;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_U32_channelTypeProfile;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_U32_counterIndex;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_U32_counterValue;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_U32_ethType;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_U32_ipMinOffset;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_U32_ipPayloadMinSize;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_U32_oamProfile;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_U32_rFlagMask;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_U32_rFlagPattern;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_U32_rdiMask;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_U32_rdiPattern;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_U32_ruleDstIndex;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_U32_ruleIndex;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_U32_ruleSrcIndex;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_U32_subLookupNum;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_CPSS_COMPARE_OPERATOR_ENT_PTR_compareOperatorPtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_CPSS_DXCH_PCL_ACTION_STC_PTR_actionPtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_CPSS_DXCH_PCL_CFG_TBL_ACCESS_MODE_STC_PTR_accModePtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_CPSS_DXCH_PCL_EGRESS_KEY_VID_UP_MODE_ENT_PTR_vidUpModePtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_CPSS_DXCH_PCL_EGRESS_SOURCE_PORT_SELECTION_MODE_ENT_PTR_portSelectionModePtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_CPSS_DXCH_PCL_EGRESS_TARGET_PORT_SELECTION_MODE_ENT_PTR_portSelectionModePtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_CPSS_DXCH_PCL_IPCL01_LOOKUP_TYPE_ENT_PTR_lookupTypePtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_CPSS_DXCH_PCL_IP_LENGTH_CHECK_MODE_ENT_PTR_modePtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_CPSS_DXCH_PCL_LOOKUP_CFG_STC_PTR_lookupCfgPtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_CPSS_DXCH_PCL_OAM_EGRESS_DETECT_MODE_ENT_PTR_modePtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_CPSS_DXCH_PCL_OAM_PKT_MEG_LEVEL_MODE_ENT_PTR_megLevelModePtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_CPSS_DXCH_PCL_OFFSET_TYPE_ENT_PTR_offsetTypePtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_CPSS_DXCH_PCL_PORT_LOOKUP_CFG_TAB_ACC_MODE_ENT_PTR_cfgTabAccModePtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_CPSS_DXCH_PCL_PORT_LOOKUP_CFG_TAB_ACC_MODE_ENT_PTR_modePtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_CPSS_DXCH_PCL_RULE_FORMAT_UNT_PTR_maskPtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_CPSS_DXCH_PCL_RULE_FORMAT_UNT_PTR_patternPtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_CPSS_DXCH_PCL_UDB_SELECT_STC_PTR_udbSelectPtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_CPSS_DXCH_UDB_ERROR_CMD_ENT_PTR_udbErrorCmdPtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_CPSS_L4_PROTOCOL_PORT_TYPE_ENT_PTR_l4PortTypePtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_CPSS_PCL_RULE_SIZE_ENT_PTR_ruleSizePtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_BOOL_PTR_RBitAssignmentEnablePtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_BOOL_PTR_isRuleValidPtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_BOOL_PTR_mixedRuleSizeModePtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_U16_PTR_valuePtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_U32_PTR_actionArr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_U32_PTR_ethTypePtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_U32_PTR_groupPtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_U32_PTR_ipMinOffsetPtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_U32_PTR_ipPayloadMinSizePtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_U32_PTR_maskArr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_U32_PTR_patternArr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_U32_PTR_pclCfgTblEntryPtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_U32_PTR_rFlagMaskPtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_U32_PTR_rFlagPatternPtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_U32_PTR_rdiMaskPtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_U32_PTR_rdiPatternPtr;


/********* lib API DB *********/

enum {
    PRV_CPSS_LOG_FUNC_cpssDxChPclInit_E = (CPSS_LOG_LIB_PCL_E << 16),
    PRV_CPSS_LOG_FUNC_cpssDxChPclUserDefinedByteSet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPclUserDefinedByteGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPclRuleSet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPclRuleParsedGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPclRuleActionUpdate_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPclRuleActionGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPclRuleInvalidate_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPclRuleValidStatusSet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPclRuleCopy_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPclRuleMatchCounterGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPclRuleMatchCounterSet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPclPortIngressPolicyEnable_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPclPortIngressPolicyEnableGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPclPortLookupCfgTabAccessModeSet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPclPortLookupCfgTabAccessModeGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPclCfgTblAccessModeSet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPclCfgTblAccessModeGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPclCfgTblSet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPclCfgTblGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPclIngressPolicyEnable_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPclIngressPolicyEnableGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPclTcamRuleSizeModeSet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPclTcamRuleSizeModeGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPclUdeEtherTypeSet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPclUdeEtherTypeGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPclTwoLookupsCpuCodeResolution_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPclInvalidUdbCmdSet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPclInvalidUdbCmdGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPclIpLengthCheckModeSet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPclIpLengthCheckModeGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxCh2PclEgressPolicyEnable_E,
    PRV_CPSS_LOG_FUNC_cpssDxCh2PclEgressPolicyEnableGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPclEgressForRemoteTunnelStartEnableSet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPclEgressForRemoteTunnelStartEnableGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPclEgressTunnelStartPacketsCfgTabAccessModeSet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPclEgressTunnelStartPacketsCfgTabAccessModeGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPclEgressKeyFieldsVidUpModeSet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPclEgressKeyFieldsVidUpModeGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxCh2EgressPclPacketTypesSet_E,
    PRV_CPSS_LOG_FUNC_cpssDxCh2EgressPclPacketTypesGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxCh3PclTunnelTermForceVlanModeEnableSet_E,
    PRV_CPSS_LOG_FUNC_cpssDxCh3PclTunnelTermForceVlanModeEnableGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxCh2PclTcpUdpPortComparatorSet_E,
    PRV_CPSS_LOG_FUNC_cpssDxCh2PclTcpUdpPortComparatorGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPclRuleStateGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPclRuleAnyStateGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPclRuleGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPclCfgTblEntryGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPclLookupCfgPortListEnableSet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPclLookupCfgPortListEnableGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPclPortGroupRuleSet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPclPortGroupRuleParsedGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPclPortGroupRuleActionUpdate_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPclPortGroupRuleActionGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPclPortGroupRuleInvalidate_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPclPortGroupRuleValidStatusSet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPclPortGroupRuleAnyStateGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPclPortGroupRuleCopy_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPclPortGroupRuleGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPclPortGroupCfgTblSet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPclPortGroupCfgTblGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPclPortGroupCfgTblEntryGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPclPortGroupTcpUdpPortComparatorSet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPclPortGroupTcpUdpPortComparatorGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPclPortGroupUserDefinedByteSet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPclPortGroupUserDefinedByteGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPclUserDefinedBytesSelectSet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPclUserDefinedBytesSelectGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPclPortGroupEgressKeyFieldsVidUpModeSet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPclPortGroupEgressKeyFieldsVidUpModeGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPclPortGroupLookupCfgPortListEnableSet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPclPortGroupLookupCfgPortListEnableGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPclL3L4ParsingOverMplsEnableSet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPclL3L4ParsingOverMplsEnableGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPclPortGroupOverrideUserDefinedBytesEnableSet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPclPortGroupOverrideUserDefinedBytesEnableGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPclOverrideUserDefinedBytesEnableSet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPclOverrideUserDefinedBytesEnableGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPclLookup0ForRoutedPacketsEnableSet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPclLookup0ForRoutedPacketsEnableGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPclLookup1ForNotRoutedPacketsEnableSet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPclLookup1ForNotRoutedPacketsEnableGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPclTcamEccDaemonEnableSet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPclTcamEccDaemonEnableGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPclIpMinOffsetSet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPclIpMinOffsetGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPclPortIngressLookup0Sublookup1TypeSet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPclPortIngressLookup0Sublookup1TypeGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPclPortListGroupingEnableSet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPclPortListGroupingEnableGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPclPortListPortMappingSet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPclPortListPortMappingGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPclOamChannelTypeProfileToOpcodeMappingSet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPclOamChannelTypeProfileToOpcodeMappingGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPclOamMegLevelModeSet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPclOamMegLevelModeGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPclOamRdiMatchingSet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPclOamRdiMatchingGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPclOamEgressPacketDetectionSet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPclOamEgressPacketDetectionGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPclOamRFlagMatchingSet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPclOamRFlagMatchingGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPclEgressSourcePortSelectionModeSet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPclEgressSourcePortSelectionModeGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPclEgressTargetPortSelectionModeSet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPclEgressTargetPortSelectionModeGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPclSourceIdMaskSet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPclSourceIdMaskGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPclIpPayloadMinSizeSet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPclIpPayloadMinSizeGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPclEgressRxAnalyzerUseOrigVidEnableSet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPclEgressRxAnalyzerUseOrigVidEnableGet_E
};

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif /* __prvCpssDxChPclLogh */
