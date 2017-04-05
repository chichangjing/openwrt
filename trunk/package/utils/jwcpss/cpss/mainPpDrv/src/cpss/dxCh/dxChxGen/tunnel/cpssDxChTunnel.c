/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* cpssDxChTunnel.c
*
* DESCRIPTION:
*       CPSS tunnel implementation.
*
* DEPENDENCIES:
*       None.
*
* FILE REVISION NUMBER:
*       $Revision: 85 $
*
*******************************************************************************/
#define CPSS_LOG_IN_MODULE_ENABLE
#include <cpssCommon/cpssPresteraDefs.h>
#include <cpss/dxCh/dxChxGen/tunnel/cpssDxChTunnel.h>
#include <cpss/dxCh/dxChxGen/tunnel/private/prvCpssDxChTunnelLog.h>
#include <cpss/generic/bridge/private/prvCpssBrgVlanTypes.h>
#include <cpss/dxCh/dxChxGen/cos/private/prvCpssDxChCoS.h>
#include <cpss/dxCh/dxChxGen/config/private/prvCpssDxChInfo.h>
#include <cpss/dxCh/dxChxGen/networkIf/private/prvCpssDxChNetIf.h>
#include <cpss/dxCh/dxChxGen/private/routerTunnelTermTcam/prvCpssDxChRouterTunnelTermTcam.h>
#include <cpss/dxCh/dxChxGen/cpssHwInit/private/prvCpssDxChHwInit.h>

/* the size of tunnel start entry in words */
#define TUNNEL_START_HW_SIZE_CNS       12

/* maximum index for tunnel start entry */
#define TUNNEL_START_MAX_INDEX_CNS   1023

/* the size of tunnel termination TCAM rule key/mask in words */
#define TUNNEL_TERMINATION_TCAM_RULE_SIZE_CNS   PRV_CPSS_DXCH_ROUTER_TUNNEL_TERM_TCAM_LINE_RULE_SIZE_CNS

/* the size of tunnel termination TCAM action in words */
#define TUNNEL_TERMINATION_TCAM_ACTION_SIZE_CNS PRV_CPSS_DXCH_ROUTER_TUNNEL_TERM_TCAM_LINE_ACTION_SIZE_CNS

/* maximum value for time to live (TTL) field */
#define TTL_MAX_CNS                     255
#define MAX_HASH_SHIFT_LEFT_BITS_CNS    8

/* maximum value for MPLS label field */
#define PRV_TUNNEL_START_MPLS_LABEL_CHECK_MAC(label)    \
    if((label) >= BIT_20)                    \
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG)

/* maximum value for MPLS label stop bit field */
#define PRV_TUNNEL_START_MPLS_SBIT_CHECK_MAC(sBit)  \
    if((sBit) > 1)                      \
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG)

/* maximum value for MPLS label exp field */
#define PRV_TUNNEL_START_MPLS_EXP_CHECK_MAC(exp)    \
    if((exp) >= BIT_3)                    \
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG)


/* maximum value for Egress Vlan Service Id field */
#define PRV_TUNNEL_START_VLAN_SERVICE_ID_CHECK_MAC(vlanServiceId)    \
    if((vlanServiceId) >= BIT_24)                    \
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG)

/* the size of tunnel start profile entry in words */
#define TUNNEL_START_PROFILE_HW_SIZE_CNS       17

/* maximum value for trunk interface field */
#define TRUNK_INTERFACE_MAX_CNS     127

/* maximum value for vidx interface field */
#define VIDX_INTERFACE_MAX_CNS      4095

/* maximum value for QOS profile field */
#define QOS_PROFILE_MAX_CNS         127

/* maximum value for policer index field */
#define POLICER_INDEX_MAX_CNS       255

/* maximum value for tunnel termination match counter value */
#define MATCH_COUNTER_INDEX_MAX_CNS     31

/* maximum value for control word index xCat; xCat3  */
#define CONTROL_WORD_INDEX_XCAT_MAX_CNS 7

/* maximum value for control word index Bobcat2; Caelum; Bobcat3  */
#define CONTROL_WORD_INDEX_MAX_CNS      15

/* macro to check line index */
#define LINE_INDEX_CHECK_MAC(devNum,index)                             \
    if(index >= PRV_CPSS_DXCH_PP_MAC(devNum)->fineTuning.tableSize.tunnelStart) \
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG)

#define PRV_TUNNEL_START_QOS_MARK_MODE_CHECK_MAC(mode)                  \
    switch ((mode))                                                     \
    {                                                                   \
    case CPSS_DXCH_TUNNEL_START_QOS_MARK_FROM_ENTRY_E:                  \
    case CPSS_DXCH_TUNNEL_START_QOS_MARK_FROM_PACKET_QOS_PROFILE_E:     \
        break;                                                          \
    default:                                                            \
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);                                            \
    }

#define PRV_TUNNEL_START_TTL_CHECK_MAC(ttl)     \
    if((ttl) > TTL_MAX_CNS)                             \
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG)

#define PRV_TUNNEL_START_AUTO_TUNNEL_OFFSET_CHECK_MAC(offset)   \
    if((offset) > 12)                                           \
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG)

#define PRV_TUNNEL_START_CFI_CHECK_MAC(cfi)   \
    if((cfi) > 1)                             \
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG)


#define PRV_TUNNEL_START_PROFILE_INDEX_CHECK_MAC(profileIndex)   \
    if((profileIndex) >= BIT_3)                                           \
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);

#define PRV_TUNNEL_START_GRE_PROTOCOL_FOR_ETHERNET_CHECK_MAC(greProtocolForEthernet)   \
    if((greProtocolForEthernet) >= BIT_16)                                           \
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);

#define PRV_TUNNEL_START_GRE_FLAGS_AND_VERSION_CHECK_MAC(greFlagsAndVersion)   \
    if((greFlagsAndVersion) >= BIT_16)                                           \
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);

#define PRV_TUNNEL_START_CHECK_TTL_MODE_MAC(mode)                           \
        switch ((mode))                                                      \
        {                                                                    \
        case CPSS_DXCH_TUNNEL_START_TTL_TO_INCOMING_TTL_E:                   \
        case CPSS_DXCH_TUNNEL_START_TTL_TO_SWAP_LABEL_E:                     \
        case CPSS_DXCH_TUNNEL_START_TTL_TO_SWAP_LABEL_MINUS_ONE_E:           \
            break;                                                           \
        default:                                                             \
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);                                             \
        }

#define PRV_TUNNEL_START_ISID_CHECK_MAC(iSid)       \
    if((iSid) >= BIT_24)                           \
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG)

#define PRV_TUNNEL_START_UDP_DST_PORT_CHECK_MAC(udpDstPort) \
    if ((udpDstPort) >= BIT_16)                             \
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);


#define PRV_TUNNEL_START_UDP_SRC_PORT_CHECK_MAC(udpSrcPort) \
    if ((udpSrcPort) >= BIT_16)                             \
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);


#define PRV_TUNNEL_START_CONVERT_MIM_I_SID_MODE_TO_HW_FORMAT_MAC(mode,value) \
    switch ((mode))                                                         \
    {                                                                       \
    case CPSS_DXCH_TUNNEL_START_MIM_I_SID_ASSIGN_FROM_TS_ENTRY_E:           \
        (value) = 0;                                                        \
        break;                                                              \
    case CPSS_DXCH_TUNNEL_START_MIM_I_SID_ASSIGN_FROM_VLAN_ENTRY_E:         \
        (value) = 1;                                                        \
        break;                                                              \
    default:  /* default set some value as the field must not be valid */   \
        (value) = 0;                                                        \
        break;                                                              \
    }

#define PRV_TUNNEL_START_MIM_I_SID_MODE_CHECK_MAC(mode)                 \
    switch ((mode))                                                     \
    {                                                                   \
    case CPSS_DXCH_TUNNEL_START_MIM_I_SID_ASSIGN_FROM_TS_ENTRY_E:       \
    case CPSS_DXCH_TUNNEL_START_MIM_I_SID_ASSIGN_FROM_VLAN_ENTRY_E:     \
        break;                                                          \
    default:                                                            \
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);                                            \
    }

#define PRV_TUNNEL_START_CONVERT_MIM_B_DA_MODE_TO_HW_FORMAT_MAC(mode,value)             \
    switch ((mode))                                                                     \
    {                                                                                   \
    case CPSS_DXCH_TUNNEL_START_MIM_B_DA_ASSIGN_FROM_TS_ENTRY_E:                        \
        (value) = 0;                                                                    \
        break;                                                                          \
    case CPSS_DXCH_TUNNEL_START_MIM_B_DA_ASSIGN_FROM_TS_ENTRY_AND_VLAN_ENTRY_E:         \
        (value) = 1;                                                                    \
        break;                                                                          \
    default:  /* default set some value as the field must not be valid */               \
        (value) = 0;                                                                    \
        break;                                                                          \
    }


#define PRV_TUNNEL_START_MIM_B_DA_MODE_CHECK_MAC(mode)                              \
    switch ((mode))                                                                 \
    {                                                                               \
    case CPSS_DXCH_TUNNEL_START_MIM_B_DA_ASSIGN_FROM_TS_ENTRY_E:                    \
    case CPSS_DXCH_TUNNEL_START_MIM_B_DA_ASSIGN_FROM_TS_ENTRY_AND_VLAN_ENTRY_E:     \
        break;                                                                      \
    default:                                                                        \
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);                                                        \
    }

#define PRV_TUNNEL_START_ITAG_RES_CHECK_MAC(iTagRes)    \
    if((iTagRes) > 15)                                     \
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG)

#define PRV_TUNNEL_START_CONVERT_QOS_MARK_MODE_TO_HW_FORMAT_MAC(mode,value) \
    switch ((mode))                                                         \
    {                                                                       \
    case CPSS_DXCH_TUNNEL_START_QOS_MARK_FROM_ENTRY_E:                      \
        (value) = 1;                                                        \
        break;                                                              \
    case CPSS_DXCH_TUNNEL_START_QOS_MARK_FROM_PACKET_QOS_PROFILE_E:         \
        (value) = 0;                                                        \
        break;                                                              \
    default:  /* default set some value as the field must not be valid */   \
        (value) = 0;                                                        \
        break;                                                              \
    }

#define PRV_TUNNEL_START_IP_HEADER_PROTOCOL_CHECK_MAC(ipHeaderProtocol) \
    switch ((ipHeaderProtocol))                                         \
    {                                                                   \
    case CPSS_DXCH_TUNNEL_START_IP_HEADER_PROTOCOL_IP_E:                \
    case CPSS_DXCH_TUNNEL_START_IP_HEADER_PROTOCOL_GRE_E:               \
    case CPSS_DXCH_TUNNEL_START_IP_HEADER_PROTOCOL_UDP_E:               \
        break;                                                          \
    default:                                                            \
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);                                            \
    }

#define PRV_TUNNEL_START_CONVERT_IP_HEADER_PROTOCOL_TO_HW_FORMAT_MAC(ipHeaderProtocol,value) \
    switch ((ipHeaderProtocol))                                                              \
    {                                                                                        \
    case CPSS_DXCH_TUNNEL_START_IP_HEADER_PROTOCOL_IP_E:                                     \
        (value) = 0;                                                                         \
        break;                                                                               \
    case CPSS_DXCH_TUNNEL_START_IP_HEADER_PROTOCOL_GRE_E:                                    \
        (value) = 1;                                                                         \
        break;                                                                               \
    case CPSS_DXCH_TUNNEL_START_IP_HEADER_PROTOCOL_UDP_E:                                    \
        (value) = 2;                                                                         \
        break;                                                                               \
    default:  /* default set some value as the field must not be valid */                    \
        (value) = 0;                                                                         \
        break;                                                                               \
    }

#define PRV_TUNNEL_START_CONVERT_TTL_MODE_TO_HW_FORMAT_MAC(mode,value)       \
        switch ((mode))                                                      \
        {                                                                    \
        case CPSS_DXCH_TUNNEL_START_TTL_TO_INCOMING_TTL_E:                   \
            (value) = 0;                                                     \
            break;                                                           \
        case CPSS_DXCH_TUNNEL_START_TTL_TO_SWAP_LABEL_E:                     \
            (value) = 1;                                                     \
            break;                                                           \
        case CPSS_DXCH_TUNNEL_START_TTL_TO_SWAP_LABEL_MINUS_ONE_E:           \
            (value) = 2;                                                     \
            break;                                                           \
        default:  /* default set some value as the field must not be valid */\
            (value) = 0;                                                     \
            break;                                                           \
        }

#define PRV_TUNNEL_START_MPLS_ETHER_TYPE_CHECK_MAC(mplsEtherType)            \
        switch ((mplsEtherType))                                             \
        {                                                                    \
        case CPSS_DXCH_TUNNEL_START_MPLS_ETHER_TYPE_UC_E:                    \
        case CPSS_DXCH_TUNNEL_START_MPLS_ETHER_TYPE_MC_E:                    \
            break;                                                           \
        default:                                                             \
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);                                             \
        }

#define PRV_TUNNEL_START_CONVERT_MPLS_ETHER_TYPE_TO_HW_FORMAT_MAC(etherType,value)\
        switch ((etherType))                                                 \
        {                                                                    \
        case CPSS_DXCH_TUNNEL_START_MPLS_ETHER_TYPE_UC_E:                    \
            (value) = 0;                                                     \
            break;                                                           \
        case CPSS_DXCH_TUNNEL_START_MPLS_ETHER_TYPE_MC_E:                    \
            (value) = 1;                                                     \
            break;                                                           \
        default:  /* default set some value as the field must not be valid */\
            (value) = 0;                                                     \
            break;                                                           \
        }

#define PRV_TUNNEL_START_FLOW_LABEL_MODE_CHECK_AND_CONVERT_TO_HW_FORMAT_MAC(flowLabelMode,value)\
        switch ((flowLabelMode))                                                              \
        {                                                                                     \
        case CPSS_DXCH_TUNNEL_START_FLOW_LABEL_ASSIGN_TO_ZERO_E:                              \
            (value) = 0;                                                                      \
            break;                                                                            \
        case CPSS_DXCH_TUNNEL_START_FLOW_LABEL_ASSIGN_TO_PACKET_HASH_VALUE_E:                 \
            (value) = 1;                                                                      \
            break;                                                                            \
        default:                                                                              \
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);                                                              \
            break;                                                                            \
        }


#define PRV_TUNNEL_START_GEN_PROFILE_CHECK_TEMPLATE_DATA_VALUE_AND_CONVERT_TO_HW_FORMAT_MAC(templateValue,hwValue) \
    switch ((templateValue))                                                              \
    {                                                                                     \
    case CPSS_DXCH_TUNNEL_START_TEMPLATE_DATA_CFG_CONST_0_E:                              \
        (hwValue) = 0;                                                                    \
        break;                                                                            \
    case CPSS_DXCH_TUNNEL_START_TEMPLATE_DATA_CFG_CONST_1_E:                              \
        (hwValue) = 1;                                                                    \
        break;                                                                            \
    case CPSS_DXCH_TUNNEL_START_TEMPLATE_DATA_CFG_EPORT_TS_EXT_0_E:                       \
        (hwValue) = 2;                                                                    \
        break;                                                                            \
    case CPSS_DXCH_TUNNEL_START_TEMPLATE_DATA_CFG_EPORT_TS_EXT_1_E:                       \
        (hwValue) = 3;                                                                    \
        break;                                                                            \
    case CPSS_DXCH_TUNNEL_START_TEMPLATE_DATA_CFG_EPORT_TS_EXT_2_E:                       \
        (hwValue) = 4;                                                                    \
        break;                                                                            \
    case CPSS_DXCH_TUNNEL_START_TEMPLATE_DATA_CFG_EPORT_TS_EXT_3_E:                       \
        (hwValue) = 5;                                                                    \
        break;                                                                            \
    case CPSS_DXCH_TUNNEL_START_TEMPLATE_DATA_CFG_EVLAN_I_SID_0_E:                        \
        (hwValue) = 6;                                                                    \
        break;                                                                            \
    case CPSS_DXCH_TUNNEL_START_TEMPLATE_DATA_CFG_EVLAN_I_SID_1_E:                        \
        (hwValue) = 7;                                                                    \
        break;                                                                            \
    case CPSS_DXCH_TUNNEL_START_TEMPLATE_DATA_CFG_EVLAN_I_SID_2_E:                        \
        (hwValue) = 8;                                                                    \
        break;                                                                            \
    case CPSS_DXCH_TUNNEL_START_TEMPLATE_DATA_CFG_SHIFT_HASH_0_E:                         \
        (hwValue) = 9;                                                                    \
        break;                                                                            \
    case CPSS_DXCH_TUNNEL_START_TEMPLATE_DATA_CFG_SHIFT_HASH_1_E:                         \
        (hwValue) = 10;                                                                   \
        break;                                                                            \
    default:                                                                              \
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);                                                              \
        break;                                                                            \
    }

#define PRV_TUNNEL_START_GEN_PROFILE_TEMPLATE_DATA_VALUE_HW_TO_SW_MAC(templateData,hwValue) \
    switch ((hwValue))                                                                    \
    {                                                                                     \
    case 0:                                                                               \
        (templateData) = CPSS_DXCH_TUNNEL_START_TEMPLATE_DATA_CFG_CONST_0_E;              \
        break;                                                                            \
    case 1:                                                                               \
        (templateData) = CPSS_DXCH_TUNNEL_START_TEMPLATE_DATA_CFG_CONST_1_E;              \
        break;                                                                            \
    case 2:                                                                               \
        (templateData) = CPSS_DXCH_TUNNEL_START_TEMPLATE_DATA_CFG_EPORT_TS_EXT_0_E;       \
        break;                                                                            \
    case 3:                                                                               \
        (templateData) = CPSS_DXCH_TUNNEL_START_TEMPLATE_DATA_CFG_EPORT_TS_EXT_1_E;       \
        break;                                                                            \
    case 4:                                                                               \
        (templateData) = CPSS_DXCH_TUNNEL_START_TEMPLATE_DATA_CFG_EPORT_TS_EXT_2_E;       \
        break;                                                                            \
    case 5:                                                                               \
        (templateData) = CPSS_DXCH_TUNNEL_START_TEMPLATE_DATA_CFG_EPORT_TS_EXT_3_E;       \
        break;                                                                            \
    case 6:                                                                               \
        (templateData) = CPSS_DXCH_TUNNEL_START_TEMPLATE_DATA_CFG_EVLAN_I_SID_0_E;        \
        break;                                                                            \
    case 7:                                                                               \
        (templateData) = CPSS_DXCH_TUNNEL_START_TEMPLATE_DATA_CFG_EVLAN_I_SID_1_E;        \
        break;                                                                            \
    case 8:                                                                               \
        (templateData) = CPSS_DXCH_TUNNEL_START_TEMPLATE_DATA_CFG_EVLAN_I_SID_2_E;        \
        break;                                                                            \
    case 9:                                                                               \
        (templateData) = CPSS_DXCH_TUNNEL_START_TEMPLATE_DATA_CFG_SHIFT_HASH_0_E;         \
        break;                                                                            \
    case 10:                                                                              \
        (templateData) = CPSS_DXCH_TUNNEL_START_TEMPLATE_DATA_CFG_SHIFT_HASH_1_E;         \
        break;                                                                            \
    default:                                                                              \
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_STATE, LOG_ERROR_NO_MSG);                                                              \
        break;                                                                            \
    }


#define PRV_TUNNEL_START_GEN_PROFILE_CHECK_TEMPLATE_DATA_SIZE_AND_CONVERT_TO_HW_FORMAT_MAC(templateDataSize,value) \
    switch ((templateDataSize))                                                           \
    {                                                                                     \
    case CPSS_DXCH_TUNNEL_START_TEMPLATE_DATA_SIZE_NONE_E:                                \
        (value) = 0;                                                                      \
        break;                                                                            \
    case CPSS_DXCH_TUNNEL_START_TEMPLATE_DATA_SIZE_4B_E:                                  \
        (value) = 1;                                                                      \
        break;                                                                            \
    case CPSS_DXCH_TUNNEL_START_TEMPLATE_DATA_SIZE_8B_E:                                  \
        (value) = 2;                                                                      \
        break;                                                                            \
    case CPSS_DXCH_TUNNEL_START_TEMPLATE_DATA_SIZE_12B_E:                                 \
        (value) = 3;                                                                      \
        break;                                                                            \
    case CPSS_DXCH_TUNNEL_START_TEMPLATE_DATA_SIZE_16B_E:                                 \
        (value) = 4;                                                                      \
        break;                                                                            \
    default:                                                                              \
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);                                                              \
        break;                                                                            \
    }

#define PRV_TUNNEL_START_GEN_PROFILE_TEMPLATE_DATA_SIZE_HW_TO_SW_MAC(templateDataSize,value) \
    switch ((value))                                                                      \
    {                                                                                     \
    case 0:                                                                               \
        (templateDataSize) = CPSS_DXCH_TUNNEL_START_TEMPLATE_DATA_SIZE_NONE_E;            \
        break;                                                                            \
    case 1:                                                                               \
        (templateDataSize) = CPSS_DXCH_TUNNEL_START_TEMPLATE_DATA_SIZE_4B_E;              \
        break;                                                                            \
    case CPSS_DXCH_TUNNEL_START_TEMPLATE_DATA_SIZE_8B_E:                                  \
        (templateDataSize) = 2;                                                           \
        break;                                                                            \
    case CPSS_DXCH_TUNNEL_START_TEMPLATE_DATA_SIZE_12B_E:                                 \
        (templateDataSize) = 3;                                                           \
        break;                                                                            \
    case CPSS_DXCH_TUNNEL_START_TEMPLATE_DATA_SIZE_16B_E:                                 \
        (templateDataSize) = 4;                                                           \
        break;                                                                            \
    default:                                                                              \
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_STATE, LOG_ERROR_NO_MSG);                                                              \
        break;                                                                            \
    }


#define PRV_TUNNEL_START_GEN_PROFILE_HASH_SHIFT_BITS_CHECK_MAC(hashShiftLeftBitsNum)     \
    if((hashShiftLeftBitsNum) >= MAX_HASH_SHIFT_LEFT_BITS_CNS)                               \
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG)

#define PRV_TUNNEL_START_GEN_PROFILE_CHECK_UDP_SRC_MODE_AND_CONVERT_TO_HW_FORMAT_MAC(udpSrcPortMode, value)\
        switch ((udpSrcPortMode))                                                             \
        {                                                                                     \
        case CPSS_DXCH_TUNNEL_START_UDP_SRC_PORT_ASSIGN_FROM_IP_GEN_TS_ENTRY_E:               \
            (value) = 0;                                                                      \
            break;                                                                            \
        case CPSS_DXCH_TUNNEL_START_UDP_SRC_PORT_ASSIGN_FROM_PACKET_HASH_VALUE_E:             \
            (value) = 1;                                                                      \
            break;                                                                            \
        default:                                                                              \
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);                                                              \
            break;                                                                            \
        }

#define PRV_TUNNEL_START_GEN_PROFILE_UDP_SRC_MODE_HW_TO_SW_MAC(udpSrcPortMode, value)               \
        switch ((value))                                                                            \
        {                                                                                           \
        case 0:                                                                                     \
            (udpSrcPortMode) = CPSS_DXCH_TUNNEL_START_UDP_SRC_PORT_ASSIGN_FROM_IP_GEN_TS_ENTRY_E;   \
            break;                                                                                  \
        case 1:                                                                                     \
            (udpSrcPortMode) = CPSS_DXCH_TUNNEL_START_UDP_SRC_PORT_ASSIGN_FROM_PACKET_HASH_VALUE_E; \
            break;                                                                                  \
        default:                                                                                    \
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_STATE, LOG_ERROR_NO_MSG);                                                                    \
            break;                                                                                  \
        }


/* macro assume : value hold the 'value' and set it into hwFormatArray */
#define LION3_HA_TUNNEL_START_FIELD_VALUE_SET_MAC(_dev,_fieldName) \
    LION3_HA_TUNNEL_START_FIELD_SET_MAC(_dev,hwFormatArray,_fieldName,value)

/* macro assume : hwFormatArray hold the entry and macro fill the value with 'value'*/
#define LION3_HA_TUNNEL_START_FIELD_VALUE_GET_MAC(_dev,_fieldName) \
    LION3_HA_TUNNEL_START_FIELD_GET_MAC(_dev,hwFormatArray,_fieldName,value)

/* macro assume : value hold the 'value' and set it into hwFormatArray */
#define LION3_HA_GEN_TUNNEL_START_PROFILE_FIELD_VALUE_SET_MAC(_dev,_fieldName) \
    LION3_HA_GEN_TUNNEL_START_PROFILE_FIELD_SET_MAC(_dev,hwFormatArray,_fieldName,value)

/* macro assume : hwFormatArray hold the entry and macro fill the value with 'value'*/
#define LION3_HA_GEN_TUNNEL_START_PROFILE_FIELD_VALUE_GET_MAC(_dev,_fieldName) \
    LION3_HA_GEN_TUNNEL_START_PROFILE_FIELD_GET_MAC(_dev,hwFormatArray,_fieldName,value)

/* assign variable to field by the same name that is common to several 'Tunnel Start' formats */
#define GET_FIELD_FROM_USER_MAC(_keyType,_fieldName)    \
    _fieldName = logicFormatPtr->_keyType._fieldName

/* assign 'SW format field' to field by the same name that is common to several 'Tunnel Start' formats */
#define SET_FIELD_TO_USER_MAC(_keyType,_fieldName)    \
    logicFormatPtr->_keyType._fieldName = _fieldName

/* hw to sw for CPSS_DXCH_TUNNEL_START_QOS_MARK_MODE_ENT */
#define HW_TO_SW_QOS_MARK_MODE_MAC(swValue,hwValue) \
    if (hwValue)                       \
        swValue = CPSS_DXCH_TUNNEL_START_QOS_MARK_FROM_ENTRY_E; \
    else                                    \
        swValue = CPSS_DXCH_TUNNEL_START_QOS_MARK_FROM_PACKET_QOS_PROFILE_E


/* hw to sw for CPSS_DXCH_TUNNEL_START_TTL_MODE_ENT */
#define HW_TO_SW_TTL_MODE_MAC(swValue,hwValue)                                  \
    switch (hwValue)                                                   \
    {                                                                  \
        case 0:                                                        \
            swValue = CPSS_DXCH_TUNNEL_START_TTL_TO_INCOMING_TTL_E;    \
            break;                                                     \
        case 1:                                                        \
            swValue = CPSS_DXCH_TUNNEL_START_TTL_TO_SWAP_LABEL_E;      \
            break;                                                     \
        case 2:                                                        \
            swValue = CPSS_DXCH_TUNNEL_START_TTL_TO_SWAP_LABEL_MINUS_ONE_E; \
            break;                                                     \
        default:                                                       \
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_STATE, LOG_ERROR_NO_MSG);                                       \
    }

/* hw to sw for CPSS_DXCH_TUNNEL_START_IP_HEADER_PROTOCOL_ENT */
#define HW_TO_SW_IP_HEADER_PROTOCOL_MAC(swValue,hwValue)               \
    switch (hwValue)                                                   \
    {                                                                  \
        case 0:                                                        \
            swValue = CPSS_DXCH_TUNNEL_START_IP_HEADER_PROTOCOL_IP_E;  \
            break;                                                     \
        case 1:                                                        \
            swValue = CPSS_DXCH_TUNNEL_START_IP_HEADER_PROTOCOL_GRE_E; \
            break;                                                     \
        case 2:                                                        \
            swValue = CPSS_DXCH_TUNNEL_START_IP_HEADER_PROTOCOL_UDP_E; \
            break;                                                     \
        default:                                                       \
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_STATE, LOG_ERROR_NO_MSG);                                       \
    }

#define HW_TO_SW_FLOW_LABEL_MODE_MAC(swValue,hwValue)                                   \
    switch (hwValue)                                                                    \
    {                                                                                   \
        case 0:                                                                         \
            swValue = CPSS_DXCH_TUNNEL_START_FLOW_LABEL_ASSIGN_TO_ZERO_E;               \
            break;                                                                      \
        case 1:                                                                         \
            swValue = CPSS_DXCH_TUNNEL_START_FLOW_LABEL_ASSIGN_TO_PACKET_HASH_VALUE_E;  \
            break;                                                                      \
        default:                                                                        \
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_STATE, LOG_ERROR_NO_MSG);                                                        \
    }

/* hw to sw for CPSS_DXCH_TUNNEL_START_MPLS_ETHER_TYPE_ENT */
#define HW_TO_SW_MPLS_ETHER_TYPE_MAC(swValue,hwValue)                  \
    switch (hwValue)                                                   \
    {                                                                  \
        case 0:                                                        \
            swValue = CPSS_DXCH_TUNNEL_START_MPLS_ETHER_TYPE_UC_E;     \
            break;                                                     \
        case 1:                                                        \
            swValue = CPSS_DXCH_TUNNEL_START_MPLS_ETHER_TYPE_MC_E;     \
            break;                                                     \
        default:                                                       \
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_STATE, LOG_ERROR_NO_MSG);                                       \
    }

/* hw to sw for CPSS_DXCH_TUNNEL_START_MIM_I_SID_ASSIGN_MODE_ENT */
#define HW_TO_SW_I_SID_ASSIGN_MODE_MAC(swValue,hwValue)                      \
    if (hwValue)                                                             \
        swValue = CPSS_DXCH_TUNNEL_START_MIM_I_SID_ASSIGN_FROM_VLAN_ENTRY_E; \
    else                                                                     \
        swValue = CPSS_DXCH_TUNNEL_START_MIM_I_SID_ASSIGN_FROM_TS_ENTRY_E

/* hw to sw for CPSS_DXCH_TUNNEL_START_MIM_B_DA_ASSIGN_MODE_ENT */
#define HW_TO_SW_B_DA_ASSIGN_MODE_MAC(swValue,hwValue)                       \
    if (hwValue)                                                             \
        swValue = CPSS_DXCH_TUNNEL_START_MIM_I_SID_ASSIGN_FROM_VLAN_ENTRY_E; \
    else                                                                     \
        swValue = CPSS_DXCH_TUNNEL_START_MIM_B_DA_ASSIGN_FROM_TS_ENTRY_E



/*
 * typedef: enum LION3_TUNNEL_START_HW_FIELD_TUNNEL_TYPE_ENT
 *
 * Description: Enumeration of HW value for tunnel start types.
 *
 * Enumerations:
 *
 *  LION3_TUNNEL_START_HW_FIELD_TUNNEL_TYPE_MPLS_E - 0x0 = MPLS; MPLS; This Entry is an X-over-MPLS Tunnel Start.;
 *  LION3_TUNNEL_START_HW_FIELD_TUNNEL_TYPE_IPV4_E - 0x1 = IPv4; IPv4; This Entry is an X-over-IPv4 or X-Over-IPv4/GRE Tunnel Start.;
 *  LION3_TUNNEL_START_HW_FIELD_TUNNEL_TYPE_MIM_E - 0x2 = MacInMac; MacInMac; MacInMac.;
 *  LION3_TUNNEL_START_HW_FIELD_TUNNEL_TYPE_TRILL_E - 0x3 = TRILL; TRILL; TRILL;
 *  LION3_TUNNEL_START_HW_FIELD_TUNNEL_TYPE_CAPWAP_E - 0x4 = CAPWAP; CAPWAP; CAPWAP;
 *  LION3_TUNNEL_START_HW_FIELD_TUNNEL_TYPE_IPV6_E - 0x5 = IPv6; IPv6; This Entry is an X-over-IPv6 or X-Over-IPv6/GRE Tunnel Start.;
 *  LION3_TUNNEL_START_HW_FIELD_TUNNEL_TYPE_GENERIC_IPV4_E - 0x6 = Generic IPv4; GenericIPv4; Flexible X-over-IPv4/GRE/UDP;
 *  LION3_TUNNEL_START_HW_FIELD_TUNNEL_TYPE_GENERIC_IPV6_E - 0x7 = GenericIPv6; GenericIPv6; Flexible X-over-IPv6/GRE/UDP;
 */
typedef enum
{
     LION3_TUNNEL_START_HW_FIELD_TUNNEL_TYPE_MPLS_E
    ,LION3_TUNNEL_START_HW_FIELD_TUNNEL_TYPE_IPV4_E
    ,LION3_TUNNEL_START_HW_FIELD_TUNNEL_TYPE_MIM_E
    ,LION3_TUNNEL_START_HW_FIELD_TUNNEL_TYPE_TRILL_E
    ,LION3_TUNNEL_START_HW_FIELD_TUNNEL_TYPE_CAPWAP_E
    ,LION3_TUNNEL_START_HW_FIELD_TUNNEL_TYPE_IPV6_E
    ,LION3_TUNNEL_START_HW_FIELD_TUNNEL_TYPE_GENERIC_IPV4_E
    ,LION3_TUNNEL_START_HW_FIELD_TUNNEL_TYPE_GENERIC_IPV6_E
}LION3_TUNNEL_START_HW_FIELD_TUNNEL_TYPE_ENT;


/* generic tunnel start entry fields array initilized in accordance with fields sequence */
LION3_HA_GEN_TUNNEL_START_PROFILE_TABLE_FIELDS_ENT genTunnelStartProfilesFields[] =
{
    LION3_HA_GEN_TUNNEL_START_PROFILE_TABLE_FIELDS_TEMPLATE_BYTE0_BIT0_E
   ,LION3_HA_GEN_TUNNEL_START_PROFILE_TABLE_FIELDS_TEMPLATE_BYTE0_BIT1_E
   ,LION3_HA_GEN_TUNNEL_START_PROFILE_TABLE_FIELDS_TEMPLATE_BYTE0_BIT2_E
   ,LION3_HA_GEN_TUNNEL_START_PROFILE_TABLE_FIELDS_TEMPLATE_BYTE0_BIT3_E
   ,LION3_HA_GEN_TUNNEL_START_PROFILE_TABLE_FIELDS_TEMPLATE_BYTE0_BIT4_E
   ,LION3_HA_GEN_TUNNEL_START_PROFILE_TABLE_FIELDS_TEMPLATE_BYTE0_BIT5_E
   ,LION3_HA_GEN_TUNNEL_START_PROFILE_TABLE_FIELDS_TEMPLATE_BYTE0_BIT6_E
   ,LION3_HA_GEN_TUNNEL_START_PROFILE_TABLE_FIELDS_TEMPLATE_BYTE0_BIT7_E

   ,LION3_HA_GEN_TUNNEL_START_PROFILE_TABLE_FIELDS_TEMPLATE_BYTE1_BIT0_E
   ,LION3_HA_GEN_TUNNEL_START_PROFILE_TABLE_FIELDS_TEMPLATE_BYTE1_BIT1_E
   ,LION3_HA_GEN_TUNNEL_START_PROFILE_TABLE_FIELDS_TEMPLATE_BYTE1_BIT2_E
   ,LION3_HA_GEN_TUNNEL_START_PROFILE_TABLE_FIELDS_TEMPLATE_BYTE1_BIT3_E
   ,LION3_HA_GEN_TUNNEL_START_PROFILE_TABLE_FIELDS_TEMPLATE_BYTE1_BIT4_E
   ,LION3_HA_GEN_TUNNEL_START_PROFILE_TABLE_FIELDS_TEMPLATE_BYTE1_BIT5_E
   ,LION3_HA_GEN_TUNNEL_START_PROFILE_TABLE_FIELDS_TEMPLATE_BYTE1_BIT6_E
   ,LION3_HA_GEN_TUNNEL_START_PROFILE_TABLE_FIELDS_TEMPLATE_BYTE1_BIT7_E

   ,LION3_HA_GEN_TUNNEL_START_PROFILE_TABLE_FIELDS_TEMPLATE_BYTE2_BIT0_E
   ,LION3_HA_GEN_TUNNEL_START_PROFILE_TABLE_FIELDS_TEMPLATE_BYTE2_BIT1_E
   ,LION3_HA_GEN_TUNNEL_START_PROFILE_TABLE_FIELDS_TEMPLATE_BYTE2_BIT2_E
   ,LION3_HA_GEN_TUNNEL_START_PROFILE_TABLE_FIELDS_TEMPLATE_BYTE2_BIT3_E
   ,LION3_HA_GEN_TUNNEL_START_PROFILE_TABLE_FIELDS_TEMPLATE_BYTE2_BIT4_E
   ,LION3_HA_GEN_TUNNEL_START_PROFILE_TABLE_FIELDS_TEMPLATE_BYTE2_BIT5_E
   ,LION3_HA_GEN_TUNNEL_START_PROFILE_TABLE_FIELDS_TEMPLATE_BYTE2_BIT6_E
   ,LION3_HA_GEN_TUNNEL_START_PROFILE_TABLE_FIELDS_TEMPLATE_BYTE2_BIT7_E

   ,LION3_HA_GEN_TUNNEL_START_PROFILE_TABLE_FIELDS_TEMPLATE_BYTE3_BIT0_E
   ,LION3_HA_GEN_TUNNEL_START_PROFILE_TABLE_FIELDS_TEMPLATE_BYTE3_BIT1_E
   ,LION3_HA_GEN_TUNNEL_START_PROFILE_TABLE_FIELDS_TEMPLATE_BYTE3_BIT2_E
   ,LION3_HA_GEN_TUNNEL_START_PROFILE_TABLE_FIELDS_TEMPLATE_BYTE3_BIT3_E
   ,LION3_HA_GEN_TUNNEL_START_PROFILE_TABLE_FIELDS_TEMPLATE_BYTE3_BIT4_E
   ,LION3_HA_GEN_TUNNEL_START_PROFILE_TABLE_FIELDS_TEMPLATE_BYTE3_BIT5_E
   ,LION3_HA_GEN_TUNNEL_START_PROFILE_TABLE_FIELDS_TEMPLATE_BYTE3_BIT6_E
   ,LION3_HA_GEN_TUNNEL_START_PROFILE_TABLE_FIELDS_TEMPLATE_BYTE3_BIT7_E

   ,LION3_HA_GEN_TUNNEL_START_PROFILE_TABLE_FIELDS_TEMPLATE_BYTE4_BIT0_E
   ,LION3_HA_GEN_TUNNEL_START_PROFILE_TABLE_FIELDS_TEMPLATE_BYTE4_BIT1_E
   ,LION3_HA_GEN_TUNNEL_START_PROFILE_TABLE_FIELDS_TEMPLATE_BYTE4_BIT2_E
   ,LION3_HA_GEN_TUNNEL_START_PROFILE_TABLE_FIELDS_TEMPLATE_BYTE4_BIT3_E
   ,LION3_HA_GEN_TUNNEL_START_PROFILE_TABLE_FIELDS_TEMPLATE_BYTE4_BIT4_E
   ,LION3_HA_GEN_TUNNEL_START_PROFILE_TABLE_FIELDS_TEMPLATE_BYTE4_BIT5_E
   ,LION3_HA_GEN_TUNNEL_START_PROFILE_TABLE_FIELDS_TEMPLATE_BYTE4_BIT6_E
   ,LION3_HA_GEN_TUNNEL_START_PROFILE_TABLE_FIELDS_TEMPLATE_BYTE4_BIT7_E

   ,LION3_HA_GEN_TUNNEL_START_PROFILE_TABLE_FIELDS_TEMPLATE_BYTE5_BIT0_E
   ,LION3_HA_GEN_TUNNEL_START_PROFILE_TABLE_FIELDS_TEMPLATE_BYTE5_BIT1_E
   ,LION3_HA_GEN_TUNNEL_START_PROFILE_TABLE_FIELDS_TEMPLATE_BYTE5_BIT2_E
   ,LION3_HA_GEN_TUNNEL_START_PROFILE_TABLE_FIELDS_TEMPLATE_BYTE5_BIT3_E
   ,LION3_HA_GEN_TUNNEL_START_PROFILE_TABLE_FIELDS_TEMPLATE_BYTE5_BIT4_E
   ,LION3_HA_GEN_TUNNEL_START_PROFILE_TABLE_FIELDS_TEMPLATE_BYTE5_BIT5_E
   ,LION3_HA_GEN_TUNNEL_START_PROFILE_TABLE_FIELDS_TEMPLATE_BYTE5_BIT6_E
   ,LION3_HA_GEN_TUNNEL_START_PROFILE_TABLE_FIELDS_TEMPLATE_BYTE5_BIT7_E

   ,LION3_HA_GEN_TUNNEL_START_PROFILE_TABLE_FIELDS_TEMPLATE_BYTE6_BIT0_E
   ,LION3_HA_GEN_TUNNEL_START_PROFILE_TABLE_FIELDS_TEMPLATE_BYTE6_BIT1_E
   ,LION3_HA_GEN_TUNNEL_START_PROFILE_TABLE_FIELDS_TEMPLATE_BYTE6_BIT2_E
   ,LION3_HA_GEN_TUNNEL_START_PROFILE_TABLE_FIELDS_TEMPLATE_BYTE6_BIT3_E
   ,LION3_HA_GEN_TUNNEL_START_PROFILE_TABLE_FIELDS_TEMPLATE_BYTE6_BIT4_E
   ,LION3_HA_GEN_TUNNEL_START_PROFILE_TABLE_FIELDS_TEMPLATE_BYTE6_BIT5_E
   ,LION3_HA_GEN_TUNNEL_START_PROFILE_TABLE_FIELDS_TEMPLATE_BYTE6_BIT6_E
   ,LION3_HA_GEN_TUNNEL_START_PROFILE_TABLE_FIELDS_TEMPLATE_BYTE6_BIT7_E

   ,LION3_HA_GEN_TUNNEL_START_PROFILE_TABLE_FIELDS_TEMPLATE_BYTE7_BIT0_E
   ,LION3_HA_GEN_TUNNEL_START_PROFILE_TABLE_FIELDS_TEMPLATE_BYTE7_BIT1_E
   ,LION3_HA_GEN_TUNNEL_START_PROFILE_TABLE_FIELDS_TEMPLATE_BYTE7_BIT2_E
   ,LION3_HA_GEN_TUNNEL_START_PROFILE_TABLE_FIELDS_TEMPLATE_BYTE7_BIT3_E
   ,LION3_HA_GEN_TUNNEL_START_PROFILE_TABLE_FIELDS_TEMPLATE_BYTE7_BIT4_E
   ,LION3_HA_GEN_TUNNEL_START_PROFILE_TABLE_FIELDS_TEMPLATE_BYTE7_BIT5_E
   ,LION3_HA_GEN_TUNNEL_START_PROFILE_TABLE_FIELDS_TEMPLATE_BYTE7_BIT6_E
   ,LION3_HA_GEN_TUNNEL_START_PROFILE_TABLE_FIELDS_TEMPLATE_BYTE7_BIT7_E

   ,LION3_HA_GEN_TUNNEL_START_PROFILE_TABLE_FIELDS_TEMPLATE_BYTE8_BIT0_E
   ,LION3_HA_GEN_TUNNEL_START_PROFILE_TABLE_FIELDS_TEMPLATE_BYTE8_BIT1_E
   ,LION3_HA_GEN_TUNNEL_START_PROFILE_TABLE_FIELDS_TEMPLATE_BYTE8_BIT2_E
   ,LION3_HA_GEN_TUNNEL_START_PROFILE_TABLE_FIELDS_TEMPLATE_BYTE8_BIT3_E
   ,LION3_HA_GEN_TUNNEL_START_PROFILE_TABLE_FIELDS_TEMPLATE_BYTE8_BIT4_E
   ,LION3_HA_GEN_TUNNEL_START_PROFILE_TABLE_FIELDS_TEMPLATE_BYTE8_BIT5_E
   ,LION3_HA_GEN_TUNNEL_START_PROFILE_TABLE_FIELDS_TEMPLATE_BYTE8_BIT6_E
   ,LION3_HA_GEN_TUNNEL_START_PROFILE_TABLE_FIELDS_TEMPLATE_BYTE8_BIT7_E

   ,LION3_HA_GEN_TUNNEL_START_PROFILE_TABLE_FIELDS_TEMPLATE_BYTE9_BIT0_E
   ,LION3_HA_GEN_TUNNEL_START_PROFILE_TABLE_FIELDS_TEMPLATE_BYTE9_BIT1_E
   ,LION3_HA_GEN_TUNNEL_START_PROFILE_TABLE_FIELDS_TEMPLATE_BYTE9_BIT2_E
   ,LION3_HA_GEN_TUNNEL_START_PROFILE_TABLE_FIELDS_TEMPLATE_BYTE9_BIT3_E
   ,LION3_HA_GEN_TUNNEL_START_PROFILE_TABLE_FIELDS_TEMPLATE_BYTE9_BIT4_E
   ,LION3_HA_GEN_TUNNEL_START_PROFILE_TABLE_FIELDS_TEMPLATE_BYTE9_BIT5_E
   ,LION3_HA_GEN_TUNNEL_START_PROFILE_TABLE_FIELDS_TEMPLATE_BYTE9_BIT6_E
   ,LION3_HA_GEN_TUNNEL_START_PROFILE_TABLE_FIELDS_TEMPLATE_BYTE9_BIT7_E

   ,LION3_HA_GEN_TUNNEL_START_PROFILE_TABLE_FIELDS_TEMPLATE_BYTE10_BIT0_E
   ,LION3_HA_GEN_TUNNEL_START_PROFILE_TABLE_FIELDS_TEMPLATE_BYTE10_BIT1_E
   ,LION3_HA_GEN_TUNNEL_START_PROFILE_TABLE_FIELDS_TEMPLATE_BYTE10_BIT2_E
   ,LION3_HA_GEN_TUNNEL_START_PROFILE_TABLE_FIELDS_TEMPLATE_BYTE10_BIT3_E
   ,LION3_HA_GEN_TUNNEL_START_PROFILE_TABLE_FIELDS_TEMPLATE_BYTE10_BIT4_E
   ,LION3_HA_GEN_TUNNEL_START_PROFILE_TABLE_FIELDS_TEMPLATE_BYTE10_BIT5_E
   ,LION3_HA_GEN_TUNNEL_START_PROFILE_TABLE_FIELDS_TEMPLATE_BYTE10_BIT6_E
   ,LION3_HA_GEN_TUNNEL_START_PROFILE_TABLE_FIELDS_TEMPLATE_BYTE10_BIT7_E

   ,LION3_HA_GEN_TUNNEL_START_PROFILE_TABLE_FIELDS_TEMPLATE_BYTE11_BIT0_E
   ,LION3_HA_GEN_TUNNEL_START_PROFILE_TABLE_FIELDS_TEMPLATE_BYTE11_BIT1_E
   ,LION3_HA_GEN_TUNNEL_START_PROFILE_TABLE_FIELDS_TEMPLATE_BYTE11_BIT2_E
   ,LION3_HA_GEN_TUNNEL_START_PROFILE_TABLE_FIELDS_TEMPLATE_BYTE11_BIT3_E
   ,LION3_HA_GEN_TUNNEL_START_PROFILE_TABLE_FIELDS_TEMPLATE_BYTE11_BIT4_E
   ,LION3_HA_GEN_TUNNEL_START_PROFILE_TABLE_FIELDS_TEMPLATE_BYTE11_BIT5_E
   ,LION3_HA_GEN_TUNNEL_START_PROFILE_TABLE_FIELDS_TEMPLATE_BYTE11_BIT6_E
   ,LION3_HA_GEN_TUNNEL_START_PROFILE_TABLE_FIELDS_TEMPLATE_BYTE11_BIT7_E

   ,LION3_HA_GEN_TUNNEL_START_PROFILE_TABLE_FIELDS_TEMPLATE_BYTE12_BIT0_E
   ,LION3_HA_GEN_TUNNEL_START_PROFILE_TABLE_FIELDS_TEMPLATE_BYTE12_BIT1_E
   ,LION3_HA_GEN_TUNNEL_START_PROFILE_TABLE_FIELDS_TEMPLATE_BYTE12_BIT2_E
   ,LION3_HA_GEN_TUNNEL_START_PROFILE_TABLE_FIELDS_TEMPLATE_BYTE12_BIT3_E
   ,LION3_HA_GEN_TUNNEL_START_PROFILE_TABLE_FIELDS_TEMPLATE_BYTE12_BIT4_E
   ,LION3_HA_GEN_TUNNEL_START_PROFILE_TABLE_FIELDS_TEMPLATE_BYTE12_BIT5_E
   ,LION3_HA_GEN_TUNNEL_START_PROFILE_TABLE_FIELDS_TEMPLATE_BYTE12_BIT6_E
   ,LION3_HA_GEN_TUNNEL_START_PROFILE_TABLE_FIELDS_TEMPLATE_BYTE12_BIT7_E

   ,LION3_HA_GEN_TUNNEL_START_PROFILE_TABLE_FIELDS_TEMPLATE_BYTE13_BIT0_E
   ,LION3_HA_GEN_TUNNEL_START_PROFILE_TABLE_FIELDS_TEMPLATE_BYTE13_BIT1_E
   ,LION3_HA_GEN_TUNNEL_START_PROFILE_TABLE_FIELDS_TEMPLATE_BYTE13_BIT2_E
   ,LION3_HA_GEN_TUNNEL_START_PROFILE_TABLE_FIELDS_TEMPLATE_BYTE13_BIT3_E
   ,LION3_HA_GEN_TUNNEL_START_PROFILE_TABLE_FIELDS_TEMPLATE_BYTE13_BIT4_E
   ,LION3_HA_GEN_TUNNEL_START_PROFILE_TABLE_FIELDS_TEMPLATE_BYTE13_BIT5_E
   ,LION3_HA_GEN_TUNNEL_START_PROFILE_TABLE_FIELDS_TEMPLATE_BYTE13_BIT6_E
   ,LION3_HA_GEN_TUNNEL_START_PROFILE_TABLE_FIELDS_TEMPLATE_BYTE13_BIT7_E

   ,LION3_HA_GEN_TUNNEL_START_PROFILE_TABLE_FIELDS_TEMPLATE_BYTE14_BIT0_E
   ,LION3_HA_GEN_TUNNEL_START_PROFILE_TABLE_FIELDS_TEMPLATE_BYTE14_BIT1_E
   ,LION3_HA_GEN_TUNNEL_START_PROFILE_TABLE_FIELDS_TEMPLATE_BYTE14_BIT2_E
   ,LION3_HA_GEN_TUNNEL_START_PROFILE_TABLE_FIELDS_TEMPLATE_BYTE14_BIT3_E
   ,LION3_HA_GEN_TUNNEL_START_PROFILE_TABLE_FIELDS_TEMPLATE_BYTE14_BIT4_E
   ,LION3_HA_GEN_TUNNEL_START_PROFILE_TABLE_FIELDS_TEMPLATE_BYTE14_BIT5_E
   ,LION3_HA_GEN_TUNNEL_START_PROFILE_TABLE_FIELDS_TEMPLATE_BYTE14_BIT6_E
   ,LION3_HA_GEN_TUNNEL_START_PROFILE_TABLE_FIELDS_TEMPLATE_BYTE14_BIT7_E

   ,LION3_HA_GEN_TUNNEL_START_PROFILE_TABLE_FIELDS_TEMPLATE_BYTE15_BIT0_E
   ,LION3_HA_GEN_TUNNEL_START_PROFILE_TABLE_FIELDS_TEMPLATE_BYTE15_BIT1_E
   ,LION3_HA_GEN_TUNNEL_START_PROFILE_TABLE_FIELDS_TEMPLATE_BYTE15_BIT2_E
   ,LION3_HA_GEN_TUNNEL_START_PROFILE_TABLE_FIELDS_TEMPLATE_BYTE15_BIT3_E
   ,LION3_HA_GEN_TUNNEL_START_PROFILE_TABLE_FIELDS_TEMPLATE_BYTE15_BIT4_E
   ,LION3_HA_GEN_TUNNEL_START_PROFILE_TABLE_FIELDS_TEMPLATE_BYTE15_BIT5_E
   ,LION3_HA_GEN_TUNNEL_START_PROFILE_TABLE_FIELDS_TEMPLATE_BYTE15_BIT6_E
   ,LION3_HA_GEN_TUNNEL_START_PROFILE_TABLE_FIELDS_TEMPLATE_BYTE15_BIT7_E

   ,LION3_HA_GEN_TUNNEL_START_PROFILE_TABLE_FIELDS_HASH_CIRCULAR_SHIFT_LEFT_E
   ,LION3_HA_GEN_TUNNEL_START_PROFILE_TABLE_FIELDS_TEMPLATE_SIZE_E
   ,LION3_HA_GEN_TUNNEL_START_PROFILE_TABLE_FIELDS_UDP_SRC_PORT_MODE_E
   ,LION3_HA_GEN_TUNNEL_START_PROFILE_TABLE_FIELDS_SERVICE_ID_E
   ,LION3_HA_GEN_TUNNEL_START_PROFILE_TABLE_FIELDS_MAC_DA_MODE_E
   ,LION3_HA_GEN_TUNNEL_START_PROFILE_TABLE_FIELDS_PROFILE_DIP_MODE_E
    ,LION3_HA_GEN_TUNNEL_START_PROFILE_TABLE_FIELDS____LAST_VALUE___E/* used for array size */
};



/*******************************************************************************
* tunnelStartConfigLogic2HwFormat_lion3
*
* DESCRIPTION:
*       Converts a given tunnel start configuration from logic format
*       to hardware format.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum            - physical device number
*       tunnelType        - type of the tunnel; valid options:
*                           CPSS_TUNNEL_X_OVER_IPV4_E
*                           CPSS_TUNNEL_X_OVER_GRE_IPV4_E
*                           CPSS_TUNNEL_X_OVER_MPLS_E
*                           CPSS_TUNNEL_MAC_IN_MAC_E
*                           CPSS_TUNNEL_TRILL_E
*       logicFormatPtr    - points to tunnel start configuration
*
* OUTPUTS:
*       hwFormatArray     - The configuration in HW format (6 words).
*
* RETURNS:
*       GT_OK             - on success.
*       GT_BAD_PARAM      - on bad parameters.
*       GT_BAD_PTR        - on NULL pointer
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS tunnelStartConfigLogic2HwFormat_lion3
(
    IN  GT_U8                               devNum,
    IN  CPSS_TUNNEL_TYPE_ENT                tunnelType,
    IN  CPSS_DXCH_TUNNEL_START_CONFIG_UNT   *logicFormatPtr,
    OUT GT_U32                              *hwFormatArray
)
{
    GT_U32      value;
    GT_U32      hwTunnelType;
    CPSS_DXCH_TUNNEL_START_QOS_MARK_MODE_ENT    sw_upMarkMode;/*SW up mark mode*/
    GT_U32      upMarkMode;         /* UP marking mode in hw format   */
    GT_U32      up;                 /* UP */
    GT_BOOL     tagEnable;
    GT_U16      vlanId;
    GT_ETHERADDR macDa;
    GT_U32      trillHeaderWords[2];

    switch (tunnelType)
    {
        case CPSS_TUNNEL_X_OVER_MPLS_E:
            hwTunnelType = LION3_TUNNEL_START_HW_FIELD_TUNNEL_TYPE_MPLS_E;

            PRV_TUNNEL_START_QOS_MARK_MODE_CHECK_MAC(logicFormatPtr->mplsCfg.upMarkMode);
            PRV_TUNNEL_START_CONVERT_QOS_MARK_MODE_TO_HW_FORMAT_MAC(logicFormatPtr->mplsCfg.upMarkMode,upMarkMode);
            GET_FIELD_FROM_USER_MAC(mplsCfg,up);
            GET_FIELD_FROM_USER_MAC(mplsCfg,tagEnable);
            GET_FIELD_FROM_USER_MAC(mplsCfg,vlanId);
            GET_FIELD_FROM_USER_MAC(mplsCfg,macDa);

            value = logicFormatPtr->mplsCfg.ttl;
            PRV_TUNNEL_START_TTL_CHECK_MAC(value);
            LION3_HA_TUNNEL_START_FIELD_VALUE_SET_MAC(devNum,LION3_HA_TUNNEL_START_TABLE_FIELDS_TTL_E);

            if(logicFormatPtr->mplsCfg.ttl == 0)
            {
                PRV_TUNNEL_START_CONVERT_TTL_MODE_TO_HW_FORMAT_MAC(logicFormatPtr->mplsCfg.ttlMode,value);
                LION3_HA_TUNNEL_START_FIELD_VALUE_SET_MAC(devNum,LION3_HA_TUNNEL_START_TABLE_FIELDS_SWAP_TTL_MODE_E);
            }

            value = BOOL2BIT_MAC(logicFormatPtr->mplsCfg.retainCRC);
            LION3_HA_TUNNEL_START_FIELD_VALUE_SET_MAC(devNum,LION3_HA_TUNNEL_START_TABLE_FIELDS_RETAIN_INNER_CRC_E);

            value = logicFormatPtr->mplsCfg.numLabels - 1;
            LION3_HA_TUNNEL_START_FIELD_VALUE_SET_MAC(devNum,LION3_HA_TUNNEL_START_TABLE_FIELDS_NUMBER_OF_MPLS_LABELS_E);

            value = logicFormatPtr->mplsCfg.label1;
            LION3_HA_TUNNEL_START_FIELD_VALUE_SET_MAC(devNum,LION3_HA_TUNNEL_START_TABLE_FIELDS_LABEL1_E);

            if (PRV_CPSS_SIP_5_15_CHECK_MAC(devNum))
            {
                value = BOOL2BIT_MAC(logicFormatPtr->mplsCfg.pushEliAndElAfterLabel1); 
                LION3_HA_TUNNEL_START_FIELD_VALUE_SET_MAC(devNum,LION3_HA_TUNNEL_START_TABLE_FIELDS_PUSH_ELI_AND_EL_AFTER_LABEL_1_E);
            }

            PRV_TUNNEL_START_QOS_MARK_MODE_CHECK_MAC(logicFormatPtr->mplsCfg.exp1MarkMode);
            PRV_TUNNEL_START_CONVERT_QOS_MARK_MODE_TO_HW_FORMAT_MAC(logicFormatPtr->mplsCfg.exp1MarkMode,value);
            LION3_HA_TUNNEL_START_FIELD_VALUE_SET_MAC(devNum,LION3_HA_TUNNEL_START_TABLE_FIELDS_EXP1_MARKING_MODE_E);

            if (logicFormatPtr->mplsCfg.exp1MarkMode == CPSS_DXCH_TUNNEL_START_QOS_MARK_FROM_ENTRY_E)
            {
                value = logicFormatPtr->mplsCfg.exp1;
                LION3_HA_TUNNEL_START_FIELD_VALUE_SET_MAC(devNum,LION3_HA_TUNNEL_START_TABLE_FIELDS_EXP1_E);
            }

            if(logicFormatPtr->mplsCfg.numLabels >= 2)
            {
                value = logicFormatPtr->mplsCfg.label2;
                LION3_HA_TUNNEL_START_FIELD_VALUE_SET_MAC(devNum,LION3_HA_TUNNEL_START_TABLE_FIELDS_LABEL2_E);

                if (PRV_CPSS_SIP_5_15_CHECK_MAC(devNum))
                {
                    value = BOOL2BIT_MAC(logicFormatPtr->mplsCfg.pushEliAndElAfterLabel2); 
                    LION3_HA_TUNNEL_START_FIELD_VALUE_SET_MAC(devNum,LION3_HA_TUNNEL_START_TABLE_FIELDS_PUSH_ELI_AND_EL_AFTER_LABEL_2_E);
                }

                PRV_TUNNEL_START_QOS_MARK_MODE_CHECK_MAC(logicFormatPtr->mplsCfg.exp2MarkMode);
                PRV_TUNNEL_START_CONVERT_QOS_MARK_MODE_TO_HW_FORMAT_MAC(logicFormatPtr->mplsCfg.exp2MarkMode,value);
                LION3_HA_TUNNEL_START_FIELD_VALUE_SET_MAC(devNum,LION3_HA_TUNNEL_START_TABLE_FIELDS_EXP2_MARKING_MODE_E);

                if (logicFormatPtr->mplsCfg.exp2MarkMode == CPSS_DXCH_TUNNEL_START_QOS_MARK_FROM_ENTRY_E)
                {
                    value = logicFormatPtr->mplsCfg.exp2;
                    LION3_HA_TUNNEL_START_FIELD_VALUE_SET_MAC(devNum,LION3_HA_TUNNEL_START_TABLE_FIELDS_EXP2_E);
                }

                if(logicFormatPtr->mplsCfg.numLabels >= 3)
                {
                    value = logicFormatPtr->mplsCfg.label3;
                    LION3_HA_TUNNEL_START_FIELD_VALUE_SET_MAC(devNum,LION3_HA_TUNNEL_START_TABLE_FIELDS_LABEL3_E);

                    if (PRV_CPSS_SIP_5_15_CHECK_MAC(devNum))
                    {
                        value = BOOL2BIT_MAC(logicFormatPtr->mplsCfg.pushEliAndElAfterLabel3); 
                        LION3_HA_TUNNEL_START_FIELD_VALUE_SET_MAC(devNum,LION3_HA_TUNNEL_START_TABLE_FIELDS_PUSH_ELI_AND_EL_AFTER_LABEL_3_E);
                    }

                    PRV_TUNNEL_START_QOS_MARK_MODE_CHECK_MAC(logicFormatPtr->mplsCfg.exp3MarkMode);
                    PRV_TUNNEL_START_CONVERT_QOS_MARK_MODE_TO_HW_FORMAT_MAC(logicFormatPtr->mplsCfg.exp3MarkMode,value);
                    LION3_HA_TUNNEL_START_FIELD_VALUE_SET_MAC(devNum,LION3_HA_TUNNEL_START_TABLE_FIELDS_EXP3_MARKING_MODE_E);

                    if (logicFormatPtr->mplsCfg.exp3MarkMode == CPSS_DXCH_TUNNEL_START_QOS_MARK_FROM_ENTRY_E)
                    {
                        value = logicFormatPtr->mplsCfg.exp3;
                        LION3_HA_TUNNEL_START_FIELD_VALUE_SET_MAC(devNum,LION3_HA_TUNNEL_START_TABLE_FIELDS_EXP3_E);
                    }
                }
            }

            value = BOOL2BIT_MAC(logicFormatPtr->mplsCfg.setSBit);
            LION3_HA_TUNNEL_START_FIELD_VALUE_SET_MAC(devNum,LION3_HA_TUNNEL_START_TABLE_FIELDS_SET_S_BIT_E);

            if (BOOL2BIT_MAC(logicFormatPtr->mplsCfg.controlWordEnable))
            {
                value = logicFormatPtr->mplsCfg.controlWordIndex + 1;
            }
            else
            {
                value = 0;
            }
            LION3_HA_TUNNEL_START_FIELD_VALUE_SET_MAC(devNum,LION3_HA_TUNNEL_START_TABLE_FIELDS_PW_CONTROL_INDEX_E);

            PRV_TUNNEL_START_MPLS_ETHER_TYPE_CHECK_MAC(logicFormatPtr->mplsCfg.mplsEthertypeSelect);
            PRV_TUNNEL_START_CONVERT_MPLS_ETHER_TYPE_TO_HW_FORMAT_MAC(logicFormatPtr->mplsCfg.mplsEthertypeSelect,value);
            LION3_HA_TUNNEL_START_FIELD_VALUE_SET_MAC(devNum,LION3_HA_TUNNEL_START_TABLE_FIELDS_MPLS_MC_UPSTREAM_ASSIGNED_LABEL_E);

            break;
        case CPSS_TUNNEL_X_OVER_IPV4_E:
        case CPSS_TUNNEL_X_OVER_GRE_IPV4_E:
        case CPSS_TUNNEL_GENERIC_IPV4_E:
            hwTunnelType = LION3_TUNNEL_START_HW_FIELD_TUNNEL_TYPE_GENERIC_IPV4_E;
            PRV_TUNNEL_START_QOS_MARK_MODE_CHECK_MAC(logicFormatPtr->ipv4Cfg.upMarkMode);
            PRV_TUNNEL_START_CONVERT_QOS_MARK_MODE_TO_HW_FORMAT_MAC(logicFormatPtr->ipv4Cfg.upMarkMode,upMarkMode);
            GET_FIELD_FROM_USER_MAC(ipv4Cfg,up);
            GET_FIELD_FROM_USER_MAC(ipv4Cfg,tagEnable);
            GET_FIELD_FROM_USER_MAC(ipv4Cfg,vlanId);
            GET_FIELD_FROM_USER_MAC(ipv4Cfg,macDa);

            /* ttl value */
            value = logicFormatPtr->ipv4Cfg.ttl;
            LION3_HA_TUNNEL_START_FIELD_VALUE_SET_MAC(devNum,LION3_HA_TUNNEL_START_TABLE_FIELDS_TTL_E);

            /* retain CRC */
            if(tunnelType == CPSS_TUNNEL_GENERIC_IPV4_E)
            {
                value = BOOL2BIT_MAC(logicFormatPtr->ipv4Cfg.retainCRC);
            }
            else
            {
                value = 0;
            }
            LION3_HA_TUNNEL_START_FIELD_VALUE_SET_MAC(devNum,LION3_HA_TUNNEL_START_TABLE_FIELDS_RETAIN_INNER_CRC_E);

            /*  ip header protocol */
            if(tunnelType == CPSS_TUNNEL_X_OVER_GRE_IPV4_E)
            {
                /* GRE protocol */
                value = 1;
            }
            if(tunnelType == CPSS_TUNNEL_X_OVER_IPV4_E)
            {
                /* IP protocol */
                value = 0;
            }
            if(tunnelType == CPSS_TUNNEL_GENERIC_IPV4_E)
            {
                PRV_TUNNEL_START_IP_HEADER_PROTOCOL_CHECK_MAC(logicFormatPtr->ipv4Cfg.ipHeaderProtocol);
                PRV_TUNNEL_START_CONVERT_IP_HEADER_PROTOCOL_TO_HW_FORMAT_MAC(logicFormatPtr->ipv4Cfg.ipHeaderProtocol,value);
            }
            LION3_HA_TUNNEL_START_FIELD_VALUE_SET_MAC(devNum,LION3_HA_TUNNEL_START_TABLE_FIELDS_PROTOCOL_E);

            /* gre fields */
            if (value == 1)/* GRE protocol*/
            {
                /*  gre flags and version */
                if (tunnelType == CPSS_TUNNEL_X_OVER_GRE_IPV4_E)
                {
                    /* legacy type: compliance  with RFC 2784 */
                    value = 0;
                }
                if (tunnelType == CPSS_TUNNEL_GENERIC_IPV4_E)
                {
                    PRV_TUNNEL_START_GRE_FLAGS_AND_VERSION_CHECK_MAC(logicFormatPtr->ipv4Cfg.greFlagsAndVersion);

                    value = logicFormatPtr->ipv4Cfg.greFlagsAndVersion;
                }
                LION3_HA_TUNNEL_START_FIELD_VALUE_SET_MAC(devNum,LION3_HA_TUNNEL_START_TABLE_FIELDS_GRE_FLAGS_AND_VER_E);

                /*  GRE Protocol for Ethernet Passenger*/
                if (tunnelType == CPSS_TUNNEL_GENERIC_IPV4_E)
                {
                    PRV_TUNNEL_START_GRE_PROTOCOL_FOR_ETHERNET_CHECK_MAC(logicFormatPtr->ipv4Cfg.greProtocolForEthernet);
                    value = logicFormatPtr->ipv4Cfg.greProtocolForEthernet;
                }
                if (tunnelType == CPSS_TUNNEL_X_OVER_GRE_IPV4_E)
                {
                    /* legacy type: only one GRE protocol  for Ethernet passenger could be suported */
                    value = 0x6558;
                }
                LION3_HA_TUNNEL_START_FIELD_VALUE_SET_MAC(devNum,LION3_HA_TUNNEL_START_TABLE_FIELDS_GRE_PROTOCOL_E);
            }
            else
                if (value == 2) /* UDP protocol*/
                {
                    /* udp dst port */
                    PRV_TUNNEL_START_UDP_DST_PORT_CHECK_MAC(logicFormatPtr->ipv4Cfg.udpDstPort);
                    value = logicFormatPtr->ipv4Cfg.udpDstPort;
                    LION3_HA_TUNNEL_START_FIELD_VALUE_SET_MAC(devNum,LION3_HA_TUNNEL_START_TABLE_FIELDS_UDP_DST_PORT_E);
                    /* udp src port */
                    PRV_TUNNEL_START_UDP_SRC_PORT_CHECK_MAC(logicFormatPtr->ipv4Cfg.udpSrcPort);
                    value = logicFormatPtr->ipv4Cfg.udpSrcPort;
                    LION3_HA_TUNNEL_START_FIELD_VALUE_SET_MAC(devNum,LION3_HA_TUNNEL_START_TABLE_FIELDS_UDP_SRC_PORT_E);
                }

            /* dscp mark mode */
            PRV_TUNNEL_START_QOS_MARK_MODE_CHECK_MAC(logicFormatPtr->ipv4Cfg.dscpMarkMode);
            PRV_TUNNEL_START_CONVERT_QOS_MARK_MODE_TO_HW_FORMAT_MAC(logicFormatPtr->ipv4Cfg.dscpMarkMode,value);
            LION3_HA_TUNNEL_START_FIELD_VALUE_SET_MAC(devNum,LION3_HA_TUNNEL_START_TABLE_FIELDS_DSCP_MARKING_MODE_E);
            /* dscp value */
            if (logicFormatPtr->ipv4Cfg.dscpMarkMode == CPSS_DXCH_TUNNEL_START_QOS_MARK_FROM_ENTRY_E)
            {
                value = logicFormatPtr->ipv4Cfg.dscp;
                LION3_HA_TUNNEL_START_FIELD_VALUE_SET_MAC(devNum,LION3_HA_TUNNEL_START_TABLE_FIELDS_DSCP_E);
            }

            /* DF flag*/
            value = BOOL2BIT_MAC(logicFormatPtr->ipv4Cfg.dontFragmentFlag);
            LION3_HA_TUNNEL_START_FIELD_VALUE_SET_MAC(devNum,LION3_HA_TUNNEL_START_TABLE_FIELDS_DF_FLAG_E);

            /* Auto tunnel flag */
            value = BOOL2BIT_MAC(logicFormatPtr->ipv4Cfg.autoTunnel);
            LION3_HA_TUNNEL_START_FIELD_VALUE_SET_MAC(devNum,LION3_HA_TUNNEL_START_TABLE_FIELDS_AUTO_TUNNEL_FLAG_E);

            /* profile index */
            if (tunnelType == CPSS_TUNNEL_GENERIC_IPV4_E)
            {
                PRV_TUNNEL_START_PROFILE_INDEX_CHECK_MAC(logicFormatPtr->ipv4Cfg.profileIndex);
                value = logicFormatPtr->ipv4Cfg.profileIndex;
            }
            else
            {
                value = 7; /* profile index for bacward compatibility */
            }
            LION3_HA_TUNNEL_START_FIELD_VALUE_SET_MAC(devNum,LION3_HA_TUNNEL_START_TABLE_FIELDS_PROFILE_NUMBER_E);

            /* Auto tunnel offset */
            if(logicFormatPtr->ipv4Cfg.autoTunnel == GT_TRUE)
            {
                value = logicFormatPtr->ipv4Cfg.autoTunnelOffset;
                LION3_HA_TUNNEL_START_FIELD_VALUE_SET_MAC(devNum,LION3_HA_TUNNEL_START_TABLE_FIELDS_AUTO_TUNNEL_OFFSET_E);
            }

            /* dip */
            value =
                (logicFormatPtr->ipv4Cfg.destIp.arIP[0] << 24) |
                (logicFormatPtr->ipv4Cfg.destIp.arIP[1] << 16) |
                (logicFormatPtr->ipv4Cfg.destIp.arIP[2] <<  8) |
                (logicFormatPtr->ipv4Cfg.destIp.arIP[3] <<  0);
            LION3_HA_TUNNEL_START_FIELD_VALUE_SET_MAC(devNum,LION3_HA_TUNNEL_START_TABLE_FIELDS_DIP_E);

            /* sip */
            value =
                (logicFormatPtr->ipv4Cfg.srcIp.arIP[0] << 24) |
                (logicFormatPtr->ipv4Cfg.srcIp.arIP[1] << 16) |
                (logicFormatPtr->ipv4Cfg.srcIp.arIP[2] <<  8) |
                (logicFormatPtr->ipv4Cfg.srcIp.arIP[3] <<  0);
            LION3_HA_TUNNEL_START_FIELD_VALUE_SET_MAC(devNum,LION3_HA_TUNNEL_START_TABLE_FIELDS_SIP_E);

            break;
        case CPSS_TUNNEL_GENERIC_IPV6_E:
            hwTunnelType = LION3_TUNNEL_START_HW_FIELD_TUNNEL_TYPE_GENERIC_IPV6_E;
            PRV_TUNNEL_START_QOS_MARK_MODE_CHECK_MAC(logicFormatPtr->ipv6Cfg.upMarkMode);
            PRV_TUNNEL_START_CONVERT_QOS_MARK_MODE_TO_HW_FORMAT_MAC(logicFormatPtr->ipv6Cfg.upMarkMode,upMarkMode);
            GET_FIELD_FROM_USER_MAC(ipv6Cfg,up);
            GET_FIELD_FROM_USER_MAC(ipv6Cfg,tagEnable);
            GET_FIELD_FROM_USER_MAC(ipv6Cfg,vlanId);
            GET_FIELD_FROM_USER_MAC(ipv6Cfg,macDa);

            /* ttl value */
            value = logicFormatPtr->ipv6Cfg.ttl;
            LION3_HA_TUNNEL_START_FIELD_VALUE_SET_MAC(devNum,LION3_HA_TUNNEL_START_TABLE_FIELDS_TTL_E);
            /* retain CRC */
            value = BOOL2BIT_MAC(logicFormatPtr->ipv6Cfg.retainCRC);
            LION3_HA_TUNNEL_START_FIELD_VALUE_SET_MAC(devNum,LION3_HA_TUNNEL_START_TABLE_FIELDS_RETAIN_INNER_CRC_E);
            /*  ip header protocol */
            PRV_TUNNEL_START_IP_HEADER_PROTOCOL_CHECK_MAC(logicFormatPtr->ipv6Cfg.ipHeaderProtocol);
            PRV_TUNNEL_START_CONVERT_IP_HEADER_PROTOCOL_TO_HW_FORMAT_MAC(logicFormatPtr->ipv6Cfg.ipHeaderProtocol,value);
            LION3_HA_TUNNEL_START_FIELD_VALUE_SET_MAC(devNum,LION3_HA_TUNNEL_START_TABLE_FIELDS_PROTOCOL_E);

            /* gre fields */
            if (value == 1)/* GRE protocol*/
            {
                PRV_TUNNEL_START_GRE_FLAGS_AND_VERSION_CHECK_MAC(logicFormatPtr->ipv6Cfg.greFlagsAndVersion);

                value = logicFormatPtr->ipv6Cfg.greFlagsAndVersion;
                LION3_HA_TUNNEL_START_FIELD_VALUE_SET_MAC(devNum,LION3_HA_TUNNEL_START_TABLE_FIELDS_GRE_FLAGS_AND_VER_E);

                /*  GRE Protocol for Ethernet Passenger*/
                PRV_TUNNEL_START_GRE_PROTOCOL_FOR_ETHERNET_CHECK_MAC(logicFormatPtr->ipv6Cfg.greProtocolForEthernet);
                value = logicFormatPtr->ipv6Cfg.greProtocolForEthernet;
                LION3_HA_TUNNEL_START_FIELD_VALUE_SET_MAC(devNum,LION3_HA_TUNNEL_START_TABLE_FIELDS_GRE_PROTOCOL_E);
            }
            else
                if (value == 2) /* UDP protocol*/
                {
                    /* udp dst port */
                    PRV_TUNNEL_START_UDP_DST_PORT_CHECK_MAC(logicFormatPtr->ipv6Cfg.udpDstPort);
                    value = logicFormatPtr->ipv6Cfg.udpDstPort;
                    LION3_HA_TUNNEL_START_FIELD_VALUE_SET_MAC(devNum,LION3_HA_TUNNEL_START_TABLE_FIELDS_UDP_DST_PORT_E);
                    /* udp src port */
                    PRV_TUNNEL_START_UDP_SRC_PORT_CHECK_MAC(logicFormatPtr->ipv6Cfg.udpSrcPort);
                    value = logicFormatPtr->ipv6Cfg.udpSrcPort;
                    LION3_HA_TUNNEL_START_FIELD_VALUE_SET_MAC(devNum,LION3_HA_TUNNEL_START_TABLE_FIELDS_UDP_SRC_PORT_E);
                }

            /* dscp mark mode */
            PRV_TUNNEL_START_QOS_MARK_MODE_CHECK_MAC(logicFormatPtr->ipv6Cfg.dscpMarkMode);
            PRV_TUNNEL_START_CONVERT_QOS_MARK_MODE_TO_HW_FORMAT_MAC(logicFormatPtr->ipv6Cfg.dscpMarkMode,value);
            LION3_HA_TUNNEL_START_FIELD_VALUE_SET_MAC(devNum,LION3_HA_TUNNEL_START_TABLE_FIELDS_DSCP_MARKING_MODE_E);
            /* dscp value */
            if (logicFormatPtr->ipv6Cfg.dscpMarkMode == CPSS_DXCH_TUNNEL_START_QOS_MARK_FROM_ENTRY_E)
            {
                value = logicFormatPtr->ipv6Cfg.dscp;
                LION3_HA_TUNNEL_START_FIELD_VALUE_SET_MAC(devNum,LION3_HA_TUNNEL_START_TABLE_FIELDS_DSCP_E);
            }

            /* flow label mode  */
            PRV_TUNNEL_START_FLOW_LABEL_MODE_CHECK_AND_CONVERT_TO_HW_FORMAT_MAC(logicFormatPtr->ipv6Cfg.flowLabelMode,value);
            LION3_HA_TUNNEL_START_FIELD_VALUE_SET_MAC(devNum,LION3_HA_TUNNEL_START_TABLE_FIELDS_FLOW_LABEL_MODE_E);

            /* profile index */
            PRV_TUNNEL_START_PROFILE_INDEX_CHECK_MAC(logicFormatPtr->ipv6Cfg.profileIndex);
            value = logicFormatPtr->ipv6Cfg.profileIndex;
            LION3_HA_TUNNEL_START_FIELD_VALUE_SET_MAC(devNum,LION3_HA_TUNNEL_START_TABLE_FIELDS_PROFILE_NUMBER_E);

            {
                GT_U32 wordNum;
                /* ipv6 destination address */
                for(wordNum = 0; wordNum < 4; wordNum++)
                {
                    /* first register comprize LSB of IP address- bits 31:0
                       second register comprize bits 63:32 and so on  */
                    hwFormatArray[4+wordNum] =    (logicFormatPtr->ipv6Cfg.destIp.arIP[((3 - wordNum) * 4)]<< 24)      |
                                                  (logicFormatPtr->ipv6Cfg.destIp.arIP[((3 - wordNum) * 4) + 1] << 16) |
                                                  (logicFormatPtr->ipv6Cfg.destIp.arIP[((3 - wordNum) * 4) + 2] << 8)  |
                                                  (logicFormatPtr->ipv6Cfg.destIp.arIP[((3 - wordNum) * 4) + 3]);
               }

                 /* ipv6 src address */
                for(wordNum = 0; wordNum < 4; wordNum++)
                {
                    /* first register comprize LSB of IP address- bits 31:0
                       second register comprize bits 63:32 and so on  */
                    hwFormatArray[8+wordNum] =    (logicFormatPtr->ipv6Cfg.srcIp.arIP[((3 - wordNum) * 4)]<< 24)      |
                                                  (logicFormatPtr->ipv6Cfg.srcIp.arIP[((3 - wordNum) * 4) + 1] << 16) |
                                                  (logicFormatPtr->ipv6Cfg.srcIp.arIP[((3 - wordNum) * 4) + 2] << 8)  |
                                                  (logicFormatPtr->ipv6Cfg.srcIp.arIP[((3 - wordNum) * 4) + 3]);
               }
            }

        break;
        case CPSS_TUNNEL_MAC_IN_MAC_E:
            hwTunnelType = LION3_TUNNEL_START_HW_FIELD_TUNNEL_TYPE_MIM_E;
            PRV_TUNNEL_START_MIM_I_SID_MODE_CHECK_MAC(logicFormatPtr->mimCfg.iSidAssignMode);
            PRV_TUNNEL_START_MIM_B_DA_MODE_CHECK_MAC(logicFormatPtr->mimCfg.bDaAssignMode);

            PRV_TUNNEL_START_QOS_MARK_MODE_CHECK_MAC(logicFormatPtr->mimCfg.upMarkMode);
            PRV_TUNNEL_START_CONVERT_QOS_MARK_MODE_TO_HW_FORMAT_MAC(logicFormatPtr->mimCfg.upMarkMode,upMarkMode);
            GET_FIELD_FROM_USER_MAC(mimCfg,up);
            GET_FIELD_FROM_USER_MAC(mimCfg,tagEnable);
            GET_FIELD_FROM_USER_MAC(mimCfg,vlanId);
            GET_FIELD_FROM_USER_MAC(mimCfg,macDa);

            value = BOOL2BIT_MAC(logicFormatPtr->mimCfg.retainCrc);
            LION3_HA_TUNNEL_START_FIELD_VALUE_SET_MAC(devNum,LION3_HA_TUNNEL_START_TABLE_FIELDS_RETAIN_INNER_CRC_E);

            PRV_TUNNEL_START_QOS_MARK_MODE_CHECK_MAC(logicFormatPtr->mimCfg.iUpMarkMode);
            PRV_TUNNEL_START_CONVERT_QOS_MARK_MODE_TO_HW_FORMAT_MAC(logicFormatPtr->mimCfg.iUpMarkMode,value);
            LION3_HA_TUNNEL_START_FIELD_VALUE_SET_MAC(devNum,LION3_HA_TUNNEL_START_TABLE_FIELDS_I_UP_MARKING_MODE_E);

            if (logicFormatPtr->mimCfg.iUpMarkMode == CPSS_DXCH_TUNNEL_START_QOS_MARK_FROM_ENTRY_E)
            {
                value = logicFormatPtr->mimCfg.iUp;
                LION3_HA_TUNNEL_START_FIELD_VALUE_SET_MAC(devNum,LION3_HA_TUNNEL_START_TABLE_FIELDS_I_UP_E);
            }

            PRV_TUNNEL_START_QOS_MARK_MODE_CHECK_MAC(logicFormatPtr->mimCfg.iDpMarkMode);
            PRV_TUNNEL_START_CONVERT_QOS_MARK_MODE_TO_HW_FORMAT_MAC(logicFormatPtr->mimCfg.iDpMarkMode,value);
            LION3_HA_TUNNEL_START_FIELD_VALUE_SET_MAC(devNum,LION3_HA_TUNNEL_START_TABLE_FIELDS_I_DEI_MARKING_MODE_E);

            if (logicFormatPtr->mimCfg.iDpMarkMode == CPSS_DXCH_TUNNEL_START_QOS_MARK_FROM_ENTRY_E)
            {
                value = logicFormatPtr->mimCfg.iDp;
                LION3_HA_TUNNEL_START_FIELD_VALUE_SET_MAC(devNum,LION3_HA_TUNNEL_START_TABLE_FIELDS_I_DEI_E);
            }

            value = U32_GET_FIELD_MAC(logicFormatPtr->mimCfg.iTagReserved,3,1);
            LION3_HA_TUNNEL_START_FIELD_VALUE_SET_MAC(devNum,LION3_HA_TUNNEL_START_TABLE_FIELDS_I_TAG_NCA_E);
            value = U32_GET_FIELD_MAC(logicFormatPtr->mimCfg.iTagReserved,2,1);
            LION3_HA_TUNNEL_START_FIELD_VALUE_SET_MAC(devNum,LION3_HA_TUNNEL_START_TABLE_FIELDS_I_TAG_RES1_E);
            value = U32_GET_FIELD_MAC(logicFormatPtr->mimCfg.iTagReserved,0,2);
            LION3_HA_TUNNEL_START_FIELD_VALUE_SET_MAC(devNum,LION3_HA_TUNNEL_START_TABLE_FIELDS_I_TAG_RES2_E);

            PRV_TUNNEL_START_CONVERT_MIM_I_SID_MODE_TO_HW_FORMAT_MAC(logicFormatPtr->mimCfg.iSidAssignMode,value);
            LION3_HA_TUNNEL_START_FIELD_VALUE_SET_MAC(devNum,LION3_HA_TUNNEL_START_TABLE_FIELDS_I_SID_ASSIGN_MODE_E);

            if (logicFormatPtr->mimCfg.iSidAssignMode == CPSS_DXCH_TUNNEL_START_MIM_I_SID_ASSIGN_FROM_TS_ENTRY_E)
            {
                value = logicFormatPtr->mimCfg.iSid;
                LION3_HA_TUNNEL_START_FIELD_VALUE_SET_MAC(devNum,LION3_HA_TUNNEL_START_TABLE_FIELDS_I_SID_E);
            }

            PRV_TUNNEL_START_CONVERT_MIM_B_DA_MODE_TO_HW_FORMAT_MAC(logicFormatPtr->mimCfg.bDaAssignMode,value);
            LION3_HA_TUNNEL_START_FIELD_VALUE_SET_MAC(devNum,LION3_HA_TUNNEL_START_TABLE_FIELDS_B_DA_ASSIGN_MODE_E);
            break;

        case CPSS_TUNNEL_TRILL_E:
            hwTunnelType = LION3_TUNNEL_START_HW_FIELD_TUNNEL_TYPE_TRILL_E;
            PRV_TUNNEL_START_QOS_MARK_MODE_CHECK_MAC(logicFormatPtr->trillCfg.upMarkMode);
            PRV_TUNNEL_START_CONVERT_QOS_MARK_MODE_TO_HW_FORMAT_MAC(logicFormatPtr->trillCfg.upMarkMode,upMarkMode);
            GET_FIELD_FROM_USER_MAC(trillCfg,up);
            GET_FIELD_FROM_USER_MAC(trillCfg,tagEnable);
            GET_FIELD_FROM_USER_MAC(trillCfg,vlanId);
            macDa = logicFormatPtr->trillCfg.nextHopMacDa;

            /* build the 48 bits 'Trill header' */
            trillHeaderWords[0] = 0;
            trillHeaderWords[1] = 0;

            value = (logicFormatPtr->trillCfg.trillHeader.version << 14) |
                    (BOOL2BIT_MAC(logicFormatPtr->trillCfg.trillHeader.mBit) << 11) |
                    (logicFormatPtr->trillCfg.trillHeader.opLength << 6) |
                    logicFormatPtr->trillCfg.trillHeader.hopCount;
            U32_SET_FIELD_IN_ENTRY_MAC(trillHeaderWords,32,16,value);
            U32_SET_FIELD_IN_ENTRY_MAC(trillHeaderWords,16,16,logicFormatPtr->trillCfg.trillHeader.eRbid);
            U32_SET_FIELD_IN_ENTRY_MAC(trillHeaderWords, 0,16,logicFormatPtr->trillCfg.trillHeader.iRbid);

            /* set the field 'TRILL_HEADER' */
            U_ANY_SET_FIELD_BY_ID_MAC(hwFormatArray,
                PRV_CPSS_DXCH_PP_MAC(devNum)->tableFormatInfo[PRV_CPSS_DXCH_TABLE_FORMAT_TUNNEL_START_E].fieldsInfoPtr,
                    LION3_HA_TUNNEL_START_TABLE_FIELDS_TRILL_HEADER_E,
                    trillHeaderWords);

            break;
        default:
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_IMPLEMENTED, LOG_ERROR_NO_MSG);
    }

    /*********************/
    /* set common fields */
    /*********************/

    value = hwTunnelType;
    LION3_HA_TUNNEL_START_FIELD_VALUE_SET_MAC(devNum,LION3_HA_TUNNEL_START_TABLE_FIELDS_TUNNEL_TYPE_E);

    value = upMarkMode;
    LION3_HA_TUNNEL_START_FIELD_VALUE_SET_MAC(devNum,LION3_HA_TUNNEL_START_TABLE_FIELDS_UP_MARKING_MODE_E);

    value = BOOL2BIT_MAC(tagEnable);
    LION3_HA_TUNNEL_START_FIELD_VALUE_SET_MAC(devNum,LION3_HA_TUNNEL_START_TABLE_FIELDS_TAG_ENABLE_E);

    if (tagEnable == GT_TRUE)
    {
        value = vlanId;
        LION3_HA_TUNNEL_START_FIELD_VALUE_SET_MAC(devNum,LION3_HA_TUNNEL_START_TABLE_FIELDS_VID_E);

        HW_TO_SW_QOS_MARK_MODE_MAC(sw_upMarkMode,upMarkMode);
        if(sw_upMarkMode == CPSS_DXCH_TUNNEL_START_QOS_MARK_FROM_ENTRY_E)
        {
            value = up;
            LION3_HA_TUNNEL_START_FIELD_VALUE_SET_MAC(devNum,LION3_HA_TUNNEL_START_TABLE_FIELDS_UP_E);
        }
    }

    /*LION3_HA_TUNNEL_START_TABLE_FIELDS_NEXT_HOP_MAC_DA_E*/
    LION3_HA_TUNNEL_START_FIELD_NEXT_HOP_MAC_DA_SET_MAC(devNum,hwFormatArray,&macDa.arEther[0]);


    return GT_OK;
}


/*******************************************************************************
* tunnelStartProfileConfigHw2LogicFormat
*
* DESCRIPTION:
*       Converts a given tunnel start configuration from hardware format
*       to logic format.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum          - device number
*       hwFormatArray   - The configuration in HW format (6 words).
*
* OUTPUTS:
*       logicFormatPtr  - points to tunnel start configuration
*
* RETURNS:
*       GT_OK           - on success.
*       GT_BAD_PARAM    - on bad parameters.
*       GT_BAD_PTR      - on NULL pointer
*       GT_BAD_STATE    - on invalid tunnel type
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS tunnelStartProfileConfigHw2LogicFormat
(
    IN  GT_U8                                           devNum,
    IN  GT_U32                                          *hwFormatArray,
    OUT CPSS_DXCH_TUNNEL_START_GEN_IP_PROFILE_STC       *logicFormatPtr
)
{
    GT_U32 i;
    GT_U32 value;
    GT_U32 templateDataHw;
    CPSS_NULL_PTR_CHECK_MAC(logicFormatPtr);
    CPSS_NULL_PTR_CHECK_MAC(hwFormatArray);

    /* handle template size */
    LION3_HA_GEN_TUNNEL_START_PROFILE_FIELD_VALUE_GET_MAC(devNum,LION3_HA_GEN_TUNNEL_START_PROFILE_TABLE_FIELDS_TEMPLATE_SIZE_E);
    templateDataHw = value;
    PRV_TUNNEL_START_GEN_PROFILE_TEMPLATE_DATA_SIZE_HW_TO_SW_MAC(logicFormatPtr->templateDataSize,value);

    /* handle template data */
    for (i = 0; i < templateDataHw*4*8; i++)
    {
        LION3_HA_GEN_TUNNEL_START_PROFILE_FIELD_VALUE_GET_MAC(devNum,genTunnelStartProfilesFields[i]);
        PRV_TUNNEL_START_GEN_PROFILE_TEMPLATE_DATA_VALUE_HW_TO_SW_MAC(logicFormatPtr->templateDataBitsCfg[i], value);
    }
    /* handle hash circular shift left */
    LION3_HA_GEN_TUNNEL_START_PROFILE_FIELD_VALUE_GET_MAC(devNum,LION3_HA_GEN_TUNNEL_START_PROFILE_TABLE_FIELDS_HASH_CIRCULAR_SHIFT_LEFT_E);
    logicFormatPtr->hashShiftLeftBitsNumber = value;


    /* handle UDP source port mode */
    LION3_HA_GEN_TUNNEL_START_PROFILE_FIELD_VALUE_GET_MAC(devNum,LION3_HA_GEN_TUNNEL_START_PROFILE_TABLE_FIELDS_UDP_SRC_PORT_MODE_E);
    PRV_TUNNEL_START_GEN_PROFILE_UDP_SRC_MODE_HW_TO_SW_MAC(logicFormatPtr->udpSrcPortMode, value);
    return GT_OK;
}

/*******************************************************************************
* tunnelStartConfigHw2LogicFormat_lion3
*
* DESCRIPTION:
*       Converts a given tunnel start configuration from hardware format
*       to logic format.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum          - physical device number
*       hwFormatArray   - The configuration in HW format (6 words).
*
* OUTPUTS:
*       tunnelTypePtr   - points to tunnel type
*       logicFormatPtr  - points to tunnel start configuration
*
* RETURNS:
*       GT_OK           - on success.
*       GT_BAD_PARAM    - on bad parameters.
*       GT_BAD_PTR      - on NULL pointer
*       GT_BAD_STATE    - on invalid tunnel type
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS tunnelStartConfigHw2LogicFormat_lion3
(
    IN  GT_U8                               devNum,
    IN  GT_U32                              *hwFormatArray,
    OUT CPSS_TUNNEL_TYPE_ENT                *tunnelTypePtr,
    OUT CPSS_DXCH_TUNNEL_START_CONFIG_UNT   *logicFormatPtr
)
{
    CPSS_TUNNEL_TYPE_ENT  swTunnelType; /* SW tunnel type */
    GT_U32      value;
    LION3_TUNNEL_START_HW_FIELD_TUNNEL_TYPE_ENT      hwTunnelType;
    GT_U32      upMarkMode;         /* UP marking mode in hw format   */
    GT_U32      up;                 /* UP */
    GT_BOOL     tagEnable;
    GT_U16      vlanId;
    GT_ETHERADDR    macDa;
    GT_U32      trillHeaderWords[2];

    devNum = devNum;

    LION3_HA_TUNNEL_START_FIELD_VALUE_GET_MAC(devNum,LION3_HA_TUNNEL_START_TABLE_FIELDS_TUNNEL_TYPE_E);
    hwTunnelType = (LION3_TUNNEL_START_HW_FIELD_TUNNEL_TYPE_ENT)value;

    /*********************/
    /* get common fields */
    /*********************/

    LION3_HA_TUNNEL_START_FIELD_VALUE_GET_MAC(devNum,LION3_HA_TUNNEL_START_TABLE_FIELDS_UP_MARKING_MODE_E);
    upMarkMode = value;

    LION3_HA_TUNNEL_START_FIELD_VALUE_GET_MAC(devNum,LION3_HA_TUNNEL_START_TABLE_FIELDS_TAG_ENABLE_E);
    tagEnable = BIT2BOOL_MAC(value);

    LION3_HA_TUNNEL_START_FIELD_VALUE_GET_MAC(devNum,LION3_HA_TUNNEL_START_TABLE_FIELDS_VID_E);
    vlanId = (GT_U16)value;

    LION3_HA_TUNNEL_START_FIELD_VALUE_GET_MAC(devNum,LION3_HA_TUNNEL_START_TABLE_FIELDS_UP_E);
    up = value;

    /*LION3_HA_TUNNEL_START_TABLE_FIELDS_NEXT_HOP_MAC_DA_E*/
    LION3_HA_TUNNEL_START_FIELD_NEXT_HOP_MAC_DA_GET_MAC(devNum,hwFormatArray,&macDa.arEther[0]);


    switch(hwTunnelType)
    {
        case LION3_TUNNEL_START_HW_FIELD_TUNNEL_TYPE_MPLS_E:
            swTunnelType = CPSS_TUNNEL_X_OVER_MPLS_E;

            HW_TO_SW_QOS_MARK_MODE_MAC(logicFormatPtr->mplsCfg.upMarkMode,upMarkMode);
            SET_FIELD_TO_USER_MAC(mplsCfg,up);
            SET_FIELD_TO_USER_MAC(mplsCfg,tagEnable);
            SET_FIELD_TO_USER_MAC(mplsCfg,vlanId);
            SET_FIELD_TO_USER_MAC(mplsCfg,macDa);

            LION3_HA_TUNNEL_START_FIELD_VALUE_GET_MAC(devNum,LION3_HA_TUNNEL_START_TABLE_FIELDS_TTL_E);
            logicFormatPtr->mplsCfg.ttl = value;

            LION3_HA_TUNNEL_START_FIELD_VALUE_GET_MAC(devNum,LION3_HA_TUNNEL_START_TABLE_FIELDS_SWAP_TTL_MODE_E);
            HW_TO_SW_TTL_MODE_MAC(logicFormatPtr->mplsCfg.ttlMode,value);

            LION3_HA_TUNNEL_START_FIELD_VALUE_GET_MAC(devNum,LION3_HA_TUNNEL_START_TABLE_FIELDS_RETAIN_INNER_CRC_E);
            logicFormatPtr->mplsCfg.retainCRC = BIT2BOOL_MAC(value);

            LION3_HA_TUNNEL_START_FIELD_VALUE_GET_MAC(devNum,LION3_HA_TUNNEL_START_TABLE_FIELDS_NUMBER_OF_MPLS_LABELS_E);
            logicFormatPtr->mplsCfg.numLabels = value + 1;

            /* label #1 */

            LION3_HA_TUNNEL_START_FIELD_VALUE_GET_MAC(devNum,LION3_HA_TUNNEL_START_TABLE_FIELDS_LABEL1_E);
            logicFormatPtr->mplsCfg.label1 = value;

            if (PRV_CPSS_SIP_5_15_CHECK_MAC(devNum))
            {
                LION3_HA_TUNNEL_START_FIELD_VALUE_GET_MAC(devNum,LION3_HA_TUNNEL_START_TABLE_FIELDS_PUSH_ELI_AND_EL_AFTER_LABEL_1_E);
                logicFormatPtr->mplsCfg.pushEliAndElAfterLabel1 = BIT2BOOL_MAC(value); 
            }

            LION3_HA_TUNNEL_START_FIELD_VALUE_GET_MAC(devNum,LION3_HA_TUNNEL_START_TABLE_FIELDS_EXP1_MARKING_MODE_E);
            HW_TO_SW_QOS_MARK_MODE_MAC(logicFormatPtr->mplsCfg.exp1MarkMode,value);

            LION3_HA_TUNNEL_START_FIELD_VALUE_GET_MAC(devNum,LION3_HA_TUNNEL_START_TABLE_FIELDS_EXP1_E);
            logicFormatPtr->mplsCfg.exp1 = value;

            /* label #2 */
            if(logicFormatPtr->mplsCfg.numLabels >= 2)
            {
                LION3_HA_TUNNEL_START_FIELD_VALUE_GET_MAC(devNum,LION3_HA_TUNNEL_START_TABLE_FIELDS_LABEL2_E);
                logicFormatPtr->mplsCfg.label2 = value;

                if (PRV_CPSS_SIP_5_15_CHECK_MAC(devNum))
                {
                    LION3_HA_TUNNEL_START_FIELD_VALUE_GET_MAC(devNum,LION3_HA_TUNNEL_START_TABLE_FIELDS_PUSH_ELI_AND_EL_AFTER_LABEL_2_E);
                    logicFormatPtr->mplsCfg.pushEliAndElAfterLabel2 = BIT2BOOL_MAC(value); 
                }

                LION3_HA_TUNNEL_START_FIELD_VALUE_GET_MAC(devNum,LION3_HA_TUNNEL_START_TABLE_FIELDS_EXP2_MARKING_MODE_E);
                HW_TO_SW_QOS_MARK_MODE_MAC(logicFormatPtr->mplsCfg.exp2MarkMode,value);

                LION3_HA_TUNNEL_START_FIELD_VALUE_GET_MAC(devNum,LION3_HA_TUNNEL_START_TABLE_FIELDS_EXP2_E);
                logicFormatPtr->mplsCfg.exp2 = value;

                /* label #3 */
                if(logicFormatPtr->mplsCfg.numLabels >= 3)
                {
                    LION3_HA_TUNNEL_START_FIELD_VALUE_GET_MAC(devNum,LION3_HA_TUNNEL_START_TABLE_FIELDS_LABEL3_E);
                    logicFormatPtr->mplsCfg.label3 = value;

                    if (PRV_CPSS_SIP_5_15_CHECK_MAC(devNum))
                    {
                        LION3_HA_TUNNEL_START_FIELD_VALUE_GET_MAC(devNum,LION3_HA_TUNNEL_START_TABLE_FIELDS_PUSH_ELI_AND_EL_AFTER_LABEL_3_E);
                        logicFormatPtr->mplsCfg.pushEliAndElAfterLabel3 = BIT2BOOL_MAC(value); 
                    }

                    LION3_HA_TUNNEL_START_FIELD_VALUE_GET_MAC(devNum,LION3_HA_TUNNEL_START_TABLE_FIELDS_EXP3_MARKING_MODE_E);
                    HW_TO_SW_QOS_MARK_MODE_MAC(logicFormatPtr->mplsCfg.exp3MarkMode,value);

                    LION3_HA_TUNNEL_START_FIELD_VALUE_GET_MAC(devNum,LION3_HA_TUNNEL_START_TABLE_FIELDS_EXP3_E);
                    logicFormatPtr->mplsCfg.exp3 = value;
                }
            }


            LION3_HA_TUNNEL_START_FIELD_VALUE_GET_MAC(devNum,LION3_HA_TUNNEL_START_TABLE_FIELDS_SET_S_BIT_E);
            logicFormatPtr->mplsCfg.setSBit = BIT2BOOL_MAC(value);

            LION3_HA_TUNNEL_START_FIELD_VALUE_GET_MAC(devNum,LION3_HA_TUNNEL_START_TABLE_FIELDS_PW_CONTROL_INDEX_E);
            logicFormatPtr->mplsCfg.controlWordEnable = (value > 0) ? GT_TRUE : GT_FALSE;
            logicFormatPtr->mplsCfg.controlWordIndex = value - 1;

            LION3_HA_TUNNEL_START_FIELD_VALUE_GET_MAC(devNum,LION3_HA_TUNNEL_START_TABLE_FIELDS_MPLS_MC_UPSTREAM_ASSIGNED_LABEL_E);
            HW_TO_SW_MPLS_ETHER_TYPE_MAC(logicFormatPtr->mplsCfg.mplsEthertypeSelect,value);

            break;
        case LION3_TUNNEL_START_HW_FIELD_TUNNEL_TYPE_GENERIC_IPV4_E:
            swTunnelType = CPSS_TUNNEL_GENERIC_IPV4_E;
            HW_TO_SW_QOS_MARK_MODE_MAC(logicFormatPtr->ipv4Cfg.upMarkMode,upMarkMode);
            SET_FIELD_TO_USER_MAC(ipv4Cfg,up);
            SET_FIELD_TO_USER_MAC(ipv4Cfg,tagEnable);
            SET_FIELD_TO_USER_MAC(ipv4Cfg,vlanId);
            SET_FIELD_TO_USER_MAC(ipv4Cfg,macDa);


            LION3_HA_TUNNEL_START_FIELD_VALUE_GET_MAC(devNum,LION3_HA_TUNNEL_START_TABLE_FIELDS_TTL_E);
            logicFormatPtr->ipv4Cfg.ttl = value;

            LION3_HA_TUNNEL_START_FIELD_VALUE_GET_MAC(devNum,LION3_HA_TUNNEL_START_TABLE_FIELDS_DSCP_MARKING_MODE_E);
            HW_TO_SW_QOS_MARK_MODE_MAC(logicFormatPtr->ipv4Cfg.dscpMarkMode,value);

            LION3_HA_TUNNEL_START_FIELD_VALUE_GET_MAC(devNum,LION3_HA_TUNNEL_START_TABLE_FIELDS_DSCP_E);
            logicFormatPtr->ipv4Cfg.dscp = value;

            LION3_HA_TUNNEL_START_FIELD_VALUE_GET_MAC(devNum,LION3_HA_TUNNEL_START_TABLE_FIELDS_DF_FLAG_E);
            logicFormatPtr->ipv4Cfg.dontFragmentFlag = BIT2BOOL_MAC(value);

            LION3_HA_TUNNEL_START_FIELD_VALUE_GET_MAC(devNum,LION3_HA_TUNNEL_START_TABLE_FIELDS_AUTO_TUNNEL_FLAG_E);
            logicFormatPtr->ipv4Cfg.autoTunnel = BIT2BOOL_MAC(value);

            LION3_HA_TUNNEL_START_FIELD_VALUE_GET_MAC(devNum,LION3_HA_TUNNEL_START_TABLE_FIELDS_AUTO_TUNNEL_OFFSET_E);
            logicFormatPtr->ipv4Cfg.autoTunnelOffset = value;
            /* profile number */
            LION3_HA_TUNNEL_START_FIELD_VALUE_GET_MAC(devNum,LION3_HA_TUNNEL_START_TABLE_FIELDS_PROFILE_NUMBER_E);
            logicFormatPtr->ipv4Cfg.profileIndex = value;
            /* retain crc */
            LION3_HA_TUNNEL_START_FIELD_VALUE_GET_MAC(devNum,LION3_HA_TUNNEL_START_TABLE_FIELDS_RETAIN_INNER_CRC_E);
            logicFormatPtr->ipv4Cfg.retainCRC = BIT2BOOL_MAC(value);

            /* ip header protocol */
            LION3_HA_TUNNEL_START_FIELD_VALUE_GET_MAC(devNum,LION3_HA_TUNNEL_START_TABLE_FIELDS_PROTOCOL_E);
            HW_TO_SW_IP_HEADER_PROTOCOL_MAC(logicFormatPtr->ipv4Cfg.ipHeaderProtocol,value);
            if (logicFormatPtr->ipv4Cfg.ipHeaderProtocol == CPSS_DXCH_TUNNEL_START_IP_HEADER_PROTOCOL_GRE_E)
            {
                /*  GRE flags and version */
                LION3_HA_TUNNEL_START_FIELD_VALUE_GET_MAC(devNum,LION3_HA_TUNNEL_START_TABLE_FIELDS_GRE_FLAGS_AND_VER_E);
                logicFormatPtr->ipv4Cfg.greFlagsAndVersion = value;
                /*  GRE protocol for ethernet passenger*/
                LION3_HA_TUNNEL_START_FIELD_VALUE_GET_MAC(devNum,LION3_HA_TUNNEL_START_TABLE_FIELDS_GRE_PROTOCOL_E);
                logicFormatPtr->ipv4Cfg.greProtocolForEthernet = value;
            }
            if (logicFormatPtr->ipv4Cfg.ipHeaderProtocol == CPSS_DXCH_TUNNEL_START_IP_HEADER_PROTOCOL_UDP_E)
            {
                /* udp dst port */
                LION3_HA_TUNNEL_START_FIELD_VALUE_GET_MAC(devNum,LION3_HA_TUNNEL_START_TABLE_FIELDS_UDP_DST_PORT_E);
                logicFormatPtr->ipv4Cfg.udpDstPort = value;
                /* udp src port */
                LION3_HA_TUNNEL_START_FIELD_VALUE_GET_MAC(devNum,LION3_HA_TUNNEL_START_TABLE_FIELDS_UDP_SRC_PORT_E);
                logicFormatPtr->ipv4Cfg.udpSrcPort = value;
            }

            LION3_HA_TUNNEL_START_FIELD_VALUE_GET_MAC(devNum,LION3_HA_TUNNEL_START_TABLE_FIELDS_DIP_E);
            logicFormatPtr->ipv4Cfg.destIp.arIP[0] = (GT_U8)(value >> 24);
            logicFormatPtr->ipv4Cfg.destIp.arIP[1] = (GT_U8)(value >> 16);
            logicFormatPtr->ipv4Cfg.destIp.arIP[2] = (GT_U8)(value >>  8);
            logicFormatPtr->ipv4Cfg.destIp.arIP[3] = (GT_U8)(value >>  0);

            LION3_HA_TUNNEL_START_FIELD_VALUE_GET_MAC(devNum,LION3_HA_TUNNEL_START_TABLE_FIELDS_SIP_E);
            logicFormatPtr->ipv4Cfg.srcIp.arIP[0] = (GT_U8)(value >> 24);
            logicFormatPtr->ipv4Cfg.srcIp.arIP[1] = (GT_U8)(value >> 16);
            logicFormatPtr->ipv4Cfg.srcIp.arIP[2] = (GT_U8)(value >>  8);
            logicFormatPtr->ipv4Cfg.srcIp.arIP[3] = (GT_U8)(value >>  0);

            break;
        case LION3_TUNNEL_START_HW_FIELD_TUNNEL_TYPE_GENERIC_IPV6_E:
            swTunnelType = CPSS_TUNNEL_GENERIC_IPV6_E;
            HW_TO_SW_QOS_MARK_MODE_MAC(logicFormatPtr->ipv6Cfg.upMarkMode,upMarkMode);
            SET_FIELD_TO_USER_MAC(ipv6Cfg,up);
            SET_FIELD_TO_USER_MAC(ipv6Cfg,tagEnable);
            SET_FIELD_TO_USER_MAC(ipv6Cfg,vlanId);
            SET_FIELD_TO_USER_MAC(ipv6Cfg,macDa);


            LION3_HA_TUNNEL_START_FIELD_VALUE_GET_MAC(devNum,LION3_HA_TUNNEL_START_TABLE_FIELDS_TTL_E);
            logicFormatPtr->ipv6Cfg.ttl = value;

            LION3_HA_TUNNEL_START_FIELD_VALUE_GET_MAC(devNum,LION3_HA_TUNNEL_START_TABLE_FIELDS_DSCP_MARKING_MODE_E);
            HW_TO_SW_QOS_MARK_MODE_MAC(logicFormatPtr->ipv6Cfg.dscpMarkMode,value);

            LION3_HA_TUNNEL_START_FIELD_VALUE_GET_MAC(devNum,LION3_HA_TUNNEL_START_TABLE_FIELDS_DSCP_E);
            logicFormatPtr->ipv6Cfg.dscp = value;

            LION3_HA_TUNNEL_START_FIELD_VALUE_GET_MAC(devNum,LION3_HA_TUNNEL_START_TABLE_FIELDS_FLOW_LABEL_MODE_E);
            HW_TO_SW_FLOW_LABEL_MODE_MAC(logicFormatPtr->ipv6Cfg.flowLabelMode,value);

            /* profile number */
            LION3_HA_TUNNEL_START_FIELD_VALUE_GET_MAC(devNum,LION3_HA_TUNNEL_START_TABLE_FIELDS_PROFILE_NUMBER_E);
            logicFormatPtr->ipv6Cfg.profileIndex = value;
            /* retain crc */
            LION3_HA_TUNNEL_START_FIELD_VALUE_GET_MAC(devNum,LION3_HA_TUNNEL_START_TABLE_FIELDS_RETAIN_INNER_CRC_E);
            logicFormatPtr->ipv6Cfg.retainCRC = BIT2BOOL_MAC(value);

            /* ip header protocol */
            LION3_HA_TUNNEL_START_FIELD_VALUE_GET_MAC(devNum,LION3_HA_TUNNEL_START_TABLE_FIELDS_PROTOCOL_E);
            HW_TO_SW_IP_HEADER_PROTOCOL_MAC(logicFormatPtr->ipv6Cfg.ipHeaderProtocol,value);
            if (logicFormatPtr->ipv6Cfg.ipHeaderProtocol == CPSS_DXCH_TUNNEL_START_IP_HEADER_PROTOCOL_GRE_E)
            {
                /*  GRE flags and version */
                LION3_HA_TUNNEL_START_FIELD_VALUE_GET_MAC(devNum,LION3_HA_TUNNEL_START_TABLE_FIELDS_GRE_FLAGS_AND_VER_E);
                logicFormatPtr->ipv6Cfg.greFlagsAndVersion = value;
                /*  GRE protocol for ethernet passenger*/
                LION3_HA_TUNNEL_START_FIELD_VALUE_GET_MAC(devNum,LION3_HA_TUNNEL_START_TABLE_FIELDS_GRE_PROTOCOL_E);
                logicFormatPtr->ipv6Cfg.greProtocolForEthernet = value;
            }
            if (logicFormatPtr->ipv6Cfg.ipHeaderProtocol == CPSS_DXCH_TUNNEL_START_IP_HEADER_PROTOCOL_UDP_E)
            {
                /* udp dst port */
                LION3_HA_TUNNEL_START_FIELD_VALUE_GET_MAC(devNum,LION3_HA_TUNNEL_START_TABLE_FIELDS_UDP_DST_PORT_E);
                logicFormatPtr->ipv6Cfg.udpDstPort = value;
                /* udp src port */
                LION3_HA_TUNNEL_START_FIELD_VALUE_GET_MAC(devNum,LION3_HA_TUNNEL_START_TABLE_FIELDS_UDP_SRC_PORT_E);
                logicFormatPtr->ipv6Cfg.udpSrcPort = value;
            }

            {
                GT_U32 wordNum;
                /* ipv6 destination address */
                for(wordNum = 0; wordNum < 4; wordNum++)
                {
                    /* first register comprize LSB of IP address- bits 31:0
                       second register comprize bits 63:32 and so on  */
                    logicFormatPtr->ipv6Cfg.destIp.arIP[((3 - wordNum) * 4)]     = (GT_U8)(hwFormatArray[4+wordNum] >> 24);
                    logicFormatPtr->ipv6Cfg.destIp.arIP[((3 - wordNum) * 4) + 1] = (GT_U8)(hwFormatArray[4+wordNum] >> 16);
                    logicFormatPtr->ipv6Cfg.destIp.arIP[((3 - wordNum) * 4) + 2] = (GT_U8)(hwFormatArray[4+wordNum] >> 8);
                    logicFormatPtr->ipv6Cfg.destIp.arIP[((3 - wordNum) * 4) + 3] = (GT_U8)(hwFormatArray[4+wordNum]);
               }

                 /* ipv6 src address */
                for(wordNum = 0; wordNum < 4; wordNum++)
                {
                    /* first register comprize LSB of IP address- bits 31:0
                       second register comprize bits 63:32 and so on  */
                    logicFormatPtr->ipv6Cfg.srcIp.arIP[((3 - wordNum) * 4)]     = (GT_U8)(hwFormatArray[8+wordNum] >> 24);
                    logicFormatPtr->ipv6Cfg.srcIp.arIP[((3 - wordNum) * 4) + 1] = (GT_U8)(hwFormatArray[8+wordNum] >> 16);
                    logicFormatPtr->ipv6Cfg.srcIp.arIP[((3 - wordNum) * 4) + 2] = (GT_U8)(hwFormatArray[8+wordNum] >> 8);
                    logicFormatPtr->ipv6Cfg.srcIp.arIP[((3 - wordNum) * 4) + 3] = (GT_U8)(hwFormatArray[8+wordNum]);
               }
            }

            break;
        case LION3_TUNNEL_START_HW_FIELD_TUNNEL_TYPE_MIM_E:
            swTunnelType = CPSS_TUNNEL_MAC_IN_MAC_E;

            HW_TO_SW_QOS_MARK_MODE_MAC(logicFormatPtr->mimCfg.upMarkMode,upMarkMode);
            SET_FIELD_TO_USER_MAC(mimCfg,up);
            SET_FIELD_TO_USER_MAC(mimCfg,tagEnable);
            SET_FIELD_TO_USER_MAC(mimCfg,vlanId);
            SET_FIELD_TO_USER_MAC(mimCfg,macDa);

            LION3_HA_TUNNEL_START_FIELD_VALUE_GET_MAC(devNum,LION3_HA_TUNNEL_START_TABLE_FIELDS_RETAIN_INNER_CRC_E);
            logicFormatPtr->mimCfg.retainCrc = BIT2BOOL_MAC(value);

            LION3_HA_TUNNEL_START_FIELD_VALUE_GET_MAC(devNum,LION3_HA_TUNNEL_START_TABLE_FIELDS_I_UP_MARKING_MODE_E);
            HW_TO_SW_QOS_MARK_MODE_MAC(logicFormatPtr->mimCfg.iUpMarkMode,value);

            LION3_HA_TUNNEL_START_FIELD_VALUE_GET_MAC(devNum,LION3_HA_TUNNEL_START_TABLE_FIELDS_I_UP_E);
            logicFormatPtr->mimCfg.iUp = value;

            LION3_HA_TUNNEL_START_FIELD_VALUE_GET_MAC(devNum,LION3_HA_TUNNEL_START_TABLE_FIELDS_I_DEI_MARKING_MODE_E);
            HW_TO_SW_QOS_MARK_MODE_MAC(logicFormatPtr->mimCfg.iDpMarkMode,value);

            LION3_HA_TUNNEL_START_FIELD_VALUE_GET_MAC(devNum,LION3_HA_TUNNEL_START_TABLE_FIELDS_I_DEI_E);
            logicFormatPtr->mimCfg.iDp = value;

            logicFormatPtr->mimCfg.iTagReserved = 0;
            LION3_HA_TUNNEL_START_FIELD_VALUE_GET_MAC(devNum,LION3_HA_TUNNEL_START_TABLE_FIELDS_I_TAG_NCA_E);
            U32_SET_FIELD_MAC(logicFormatPtr->mimCfg.iTagReserved,3,1,value);
            LION3_HA_TUNNEL_START_FIELD_VALUE_GET_MAC(devNum,LION3_HA_TUNNEL_START_TABLE_FIELDS_I_TAG_RES1_E);
            U32_SET_FIELD_MAC(logicFormatPtr->mimCfg.iTagReserved,2,1,value);
            LION3_HA_TUNNEL_START_FIELD_VALUE_GET_MAC(devNum,LION3_HA_TUNNEL_START_TABLE_FIELDS_I_TAG_RES2_E);
            U32_SET_FIELD_MAC(logicFormatPtr->mimCfg.iTagReserved,0,2,value);

            LION3_HA_TUNNEL_START_FIELD_VALUE_GET_MAC(devNum,LION3_HA_TUNNEL_START_TABLE_FIELDS_I_SID_ASSIGN_MODE_E);
            HW_TO_SW_I_SID_ASSIGN_MODE_MAC(logicFormatPtr->mimCfg.iSidAssignMode,value);
            LION3_HA_TUNNEL_START_FIELD_VALUE_GET_MAC(devNum,LION3_HA_TUNNEL_START_TABLE_FIELDS_I_SID_E);
            logicFormatPtr->mimCfg.iSid = value;
            LION3_HA_TUNNEL_START_FIELD_VALUE_GET_MAC(devNum,LION3_HA_TUNNEL_START_TABLE_FIELDS_B_DA_ASSIGN_MODE_E);
            HW_TO_SW_B_DA_ASSIGN_MODE_MAC(logicFormatPtr->mimCfg.bDaAssignMode,value);

            break;
        case LION3_TUNNEL_START_HW_FIELD_TUNNEL_TYPE_TRILL_E:
            swTunnelType = CPSS_TUNNEL_TRILL_E;

            HW_TO_SW_QOS_MARK_MODE_MAC(logicFormatPtr->trillCfg.upMarkMode,upMarkMode);
            SET_FIELD_TO_USER_MAC(trillCfg,up);
            SET_FIELD_TO_USER_MAC(trillCfg,tagEnable);
            SET_FIELD_TO_USER_MAC(trillCfg,vlanId);
            logicFormatPtr->trillCfg.nextHopMacDa = macDa;

            /* get the field 'TRILL_HEADER' */
            U_ANY_GET_FIELD_BY_ID_MAC(hwFormatArray,
                PRV_CPSS_DXCH_PP_MAC(devNum)->tableFormatInfo[PRV_CPSS_DXCH_TABLE_FORMAT_TUNNEL_START_E].fieldsInfoPtr,
                    LION3_HA_TUNNEL_START_TABLE_FIELDS_TRILL_HEADER_E,
                    trillHeaderWords);

            /* parse the 48 bits 'Trill header' */
            U32_GET_FIELD_IN_ENTRY_MAC(trillHeaderWords,32,16,value);
            U32_GET_FIELD_IN_ENTRY_MAC(trillHeaderWords,16,16,logicFormatPtr->trillCfg.trillHeader.eRbid);
            U32_GET_FIELD_IN_ENTRY_MAC(trillHeaderWords, 0,16,logicFormatPtr->trillCfg.trillHeader.iRbid);

            logicFormatPtr->trillCfg.trillHeader.version = U32_GET_FIELD_MAC(value,14,2);
            logicFormatPtr->trillCfg.trillHeader.mBit = BIT2BOOL_MAC(U32_GET_FIELD_MAC(value,11,1));
            logicFormatPtr->trillCfg.trillHeader.opLength = U32_GET_FIELD_MAC(value,6,5);
            logicFormatPtr->trillCfg.trillHeader.hopCount = U32_GET_FIELD_MAC(value,0,6);

            break;
        default:
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_IMPLEMENTED, LOG_ERROR_NO_MSG);
    }


    *tunnelTypePtr = swTunnelType;

    return GT_OK;
}


/*******************************************************************************
* tunnelStartProfileConfigLogic2HwFormat
*
* DESCRIPTION:
*       Converts a given tunnel start configuration from logic format
*       to hardware format.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum            - device number
*       logicFormatPtr    - points to genric tunnel start profile configuration
*
* OUTPUTS:
*       hwFormatArray     - The configuration in HW format (17 words).
*
* RETURNS:
*       GT_OK             - on success.
*       GT_BAD_PARAM      - on bad parameters.
*       GT_BAD_PTR        - on NULL pointer
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS tunnelStartProfileConfigLogic2HwFormat
(
    IN  GT_U8                                       devNum,
    IN  CPSS_DXCH_TUNNEL_START_GEN_IP_PROFILE_STC   *logicFormatPtr,
    OUT GT_U32                                      *hwFormatArray
)
{
    GT_U32 i;
    GT_U32 value;
    GT_U32 templateSizeHw;
    CPSS_NULL_PTR_CHECK_MAC(logicFormatPtr);
    CPSS_NULL_PTR_CHECK_MAC(hwFormatArray);
     /* zero out hw format */
    cpssOsMemSet(hwFormatArray,0,sizeof(GT_U32)*TUNNEL_START_PROFILE_HW_SIZE_CNS);

    /* handle template size */
    PRV_TUNNEL_START_GEN_PROFILE_CHECK_TEMPLATE_DATA_SIZE_AND_CONVERT_TO_HW_FORMAT_MAC(logicFormatPtr->templateDataSize, value);
    templateSizeHw = value;
    LION3_HA_GEN_TUNNEL_START_PROFILE_FIELD_VALUE_SET_MAC(devNum,LION3_HA_GEN_TUNNEL_START_PROFILE_TABLE_FIELDS_TEMPLATE_SIZE_E);
    /*templateDataSize in hw format could be 0,1,2,3,4 :respectively 0,4,8,12,16 byte of template data size*/

    /* handle template data */
    for (i = 0; i < templateSizeHw*4*8; i++)
    {
        PRV_TUNNEL_START_GEN_PROFILE_CHECK_TEMPLATE_DATA_VALUE_AND_CONVERT_TO_HW_FORMAT_MAC(logicFormatPtr->templateDataBitsCfg[i], value);
        LION3_HA_GEN_TUNNEL_START_PROFILE_FIELD_VALUE_SET_MAC(devNum,genTunnelStartProfilesFields[i]);
    }
    /* handle hash circular shift left */
    value = logicFormatPtr->hashShiftLeftBitsNumber;
    LION3_HA_GEN_TUNNEL_START_PROFILE_FIELD_VALUE_SET_MAC(devNum,LION3_HA_GEN_TUNNEL_START_PROFILE_TABLE_FIELDS_HASH_CIRCULAR_SHIFT_LEFT_E);

    /* handle UDP source port mode */
    PRV_TUNNEL_START_GEN_PROFILE_CHECK_UDP_SRC_MODE_AND_CONVERT_TO_HW_FORMAT_MAC(logicFormatPtr->udpSrcPortMode, value);
    LION3_HA_GEN_TUNNEL_START_PROFILE_FIELD_VALUE_SET_MAC(devNum,LION3_HA_GEN_TUNNEL_START_PROFILE_TABLE_FIELDS_UDP_SRC_PORT_MODE_E);

    return GT_OK;
}



/*******************************************************************************
* tunnelStartConfigLogic2HwFormat
*
* DESCRIPTION:
*       Converts a given tunnel start configuration from logic format
*       to hardware format.
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond.
*
* INPUTS:
*       devNum            - physical device number
*       tunnelType        - type of the tunnel; valid options:
*                           CPSS_TUNNEL_X_OVER_IPV4_E
*                           CPSS_TUNNEL_X_OVER_GRE_IPV4_E
*                           CPSS_TUNNEL_X_OVER_MPLS_E
*                           CPSS_TUNNEL_MAC_IN_MAC_E (APPLICABLE DEVICES: xCat; xCat3; Lion; xCat2, Lion2, Bobcat2, Caelum, Bobcat3)
*                           CPSS_TUNNEL_TRILL_E (APPLICABLE DEVICES: Bobcat2; Caelum; Bobcat3)
*       logicFormatPtr    - points to tunnel start configuration
*
* OUTPUTS:
*       hwFormatArray     - The configuration in HW format (6 words).
*
* RETURNS:
*       GT_OK             - on success.
*       GT_BAD_PARAM      - on bad parameters.
*       GT_BAD_PTR        - on NULL pointer
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS tunnelStartConfigLogic2HwFormat
(
    IN  GT_U8                               devNum,
    IN  CPSS_TUNNEL_TYPE_ENT                tunnelType,
    IN  CPSS_DXCH_TUNNEL_START_CONFIG_UNT   *logicFormatPtr,
    OUT GT_U32                              *hwFormatArray
)
{
    GT_U32      upMarkMode;         /* UP marking mode in hw format   */
    GT_U32      dscpMarkMode;       /* DSCP marking mode in hw format */
    GT_U32      exp1MarkMode;       /* EXP1 marking mode in hw format */
    GT_U32      exp2MarkMode;       /* EXP2 marking mode in hw format */
    GT_U32      exp3MarkMode;       /* EXP3 marking mode in hw format */
    GT_U32      ttlMode;            /* TTL mode in hw format          */
    GT_U32      iUpMarkMode;        /* iUP marking mode in hw format  */
    GT_U32      iDpMarkMode;        /* iDP marking mode in hw format  */
    GT_U32      controlWordIndex= 0;/* control word index in hw format*/

    CPSS_NULL_PTR_CHECK_MAC(logicFormatPtr);
    CPSS_NULL_PTR_CHECK_MAC(hwFormatArray);

     /* zero out hw format */
    cpssOsMemSet(hwFormatArray,0,sizeof(GT_U32)*TUNNEL_START_HW_SIZE_CNS);

    /********************/
    /* check parameters */
    /********************/

    switch (tunnelType)
    {
    case CPSS_TUNNEL_X_OVER_IPV4_E:
    case CPSS_TUNNEL_X_OVER_GRE_IPV4_E:
    case CPSS_TUNNEL_GENERIC_IPV4_E:
        /********************************/
        /* check IPv4 config parameters */
        /********************************/
        if (logicFormatPtr->ipv4Cfg.tagEnable == GT_TRUE)
        {
            PRV_CPSS_VLAN_VALUE_CHECK_MAC(logicFormatPtr->ipv4Cfg.vlanId);
            PRV_TUNNEL_START_QOS_MARK_MODE_CHECK_MAC(logicFormatPtr->ipv4Cfg.upMarkMode);
            if (logicFormatPtr->ipv4Cfg.upMarkMode == CPSS_DXCH_TUNNEL_START_QOS_MARK_FROM_ENTRY_E)
                PRV_CPSS_DXCH_COS_CHECK_UP_MAC(logicFormatPtr->ipv4Cfg.up);
        }
        PRV_TUNNEL_START_QOS_MARK_MODE_CHECK_MAC(logicFormatPtr->ipv4Cfg.dscpMarkMode);
        if (logicFormatPtr->ipv4Cfg.dscpMarkMode == CPSS_DXCH_TUNNEL_START_QOS_MARK_FROM_ENTRY_E)
            PRV_CPSS_DXCH_COS_CHECK_DSCP_MAC(logicFormatPtr->ipv4Cfg.dscp);
        PRV_TUNNEL_START_TTL_CHECK_MAC(logicFormatPtr->ipv4Cfg.ttl);
        if (logicFormatPtr->ipv4Cfg.autoTunnel == GT_TRUE)
            PRV_TUNNEL_START_AUTO_TUNNEL_OFFSET_CHECK_MAC(logicFormatPtr->ipv4Cfg.autoTunnelOffset);
        if (PRV_CPSS_DXCH3_FAMILY_CHECK_MAC(devNum))
            PRV_TUNNEL_START_CFI_CHECK_MAC(logicFormatPtr->ipv4Cfg.cfi);
        break;
    case CPSS_TUNNEL_GENERIC_IPV6_E:
        /* check ipv6 config parameters*/
        if (logicFormatPtr->ipv6Cfg.tagEnable == GT_TRUE)
        {
            PRV_CPSS_VLAN_VALUE_CHECK_MAC(logicFormatPtr->ipv6Cfg.vlanId);
            PRV_TUNNEL_START_QOS_MARK_MODE_CHECK_MAC(logicFormatPtr->ipv6Cfg.upMarkMode);
            if (logicFormatPtr->ipv6Cfg.upMarkMode == CPSS_DXCH_TUNNEL_START_QOS_MARK_FROM_ENTRY_E)
                PRV_CPSS_DXCH_COS_CHECK_UP_MAC(logicFormatPtr->ipv6Cfg.up);
        }
        PRV_TUNNEL_START_QOS_MARK_MODE_CHECK_MAC(logicFormatPtr->ipv6Cfg.dscpMarkMode);
        if (logicFormatPtr->ipv6Cfg.dscpMarkMode == CPSS_DXCH_TUNNEL_START_QOS_MARK_FROM_ENTRY_E)
            PRV_CPSS_DXCH_COS_CHECK_DSCP_MAC(logicFormatPtr->ipv6Cfg.dscp);
        PRV_TUNNEL_START_TTL_CHECK_MAC(logicFormatPtr->ipv6Cfg.ttl);
        break;
    case CPSS_TUNNEL_X_OVER_MPLS_E:
        /********************************/
        /* check MPLS config parameters */
        /********************************/
        if (logicFormatPtr->mplsCfg.tagEnable == GT_TRUE)
        {
            PRV_CPSS_VLAN_VALUE_CHECK_MAC(logicFormatPtr->mplsCfg.vlanId);
            PRV_TUNNEL_START_QOS_MARK_MODE_CHECK_MAC(logicFormatPtr->mplsCfg.upMarkMode);
            if (logicFormatPtr->mplsCfg.upMarkMode == CPSS_DXCH_TUNNEL_START_QOS_MARK_FROM_ENTRY_E)
                PRV_CPSS_DXCH_COS_CHECK_UP_MAC(logicFormatPtr->mplsCfg.up);
        }
        PRV_TUNNEL_START_TTL_CHECK_MAC(logicFormatPtr->mplsCfg.ttl);
        if ((logicFormatPtr->mplsCfg.ttl == 0)
                && PRV_CPSS_DXCH_XCAT_FAMILY_CHECK_MAC(devNum))
        {
            PRV_TUNNEL_START_CHECK_TTL_MODE_MAC(
                logicFormatPtr->mplsCfg.ttlMode);
        }
        switch (logicFormatPtr->mplsCfg.numLabels)
        {
        case 3:
        case 2:
        case 1:
             /* 3 labels mode is supported for xCat and Lion */
            if (logicFormatPtr->mplsCfg.numLabels >= 3)
            {
                if(! PRV_CPSS_DXCH_XCAT_FAMILY_CHECK_MAC(devNum))
                    CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
                PRV_TUNNEL_START_MPLS_LABEL_CHECK_MAC(logicFormatPtr->mplsCfg.label3);
                PRV_TUNNEL_START_QOS_MARK_MODE_CHECK_MAC(logicFormatPtr->mplsCfg.exp3MarkMode);
                if (logicFormatPtr->mplsCfg.exp3MarkMode == CPSS_DXCH_TUNNEL_START_QOS_MARK_FROM_ENTRY_E)
                    PRV_TUNNEL_START_MPLS_EXP_CHECK_MAC(logicFormatPtr->mplsCfg.exp3);
            }
            if (logicFormatPtr->mplsCfg.numLabels >= 2)
            {
                PRV_TUNNEL_START_MPLS_LABEL_CHECK_MAC(logicFormatPtr->mplsCfg.label2);
                PRV_TUNNEL_START_QOS_MARK_MODE_CHECK_MAC(logicFormatPtr->mplsCfg.exp2MarkMode);
                if (logicFormatPtr->mplsCfg.exp2MarkMode == CPSS_DXCH_TUNNEL_START_QOS_MARK_FROM_ENTRY_E)
                    PRV_TUNNEL_START_MPLS_EXP_CHECK_MAC(logicFormatPtr->mplsCfg.exp2);
            }
            PRV_TUNNEL_START_MPLS_LABEL_CHECK_MAC(logicFormatPtr->mplsCfg.label1);
            PRV_TUNNEL_START_QOS_MARK_MODE_CHECK_MAC(logicFormatPtr->mplsCfg.exp1MarkMode);
            if (logicFormatPtr->mplsCfg.exp1MarkMode == CPSS_DXCH_TUNNEL_START_QOS_MARK_FROM_ENTRY_E)
                PRV_TUNNEL_START_MPLS_EXP_CHECK_MAC(logicFormatPtr->mplsCfg.exp1);
            break;
        default:
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
        }
        if (PRV_CPSS_DXCH3_FAMILY_CHECK_MAC(devNum))
            PRV_TUNNEL_START_CFI_CHECK_MAC(logicFormatPtr->mplsCfg.cfi);

        /* FEr#2785: The passenger Ethernet packet CRC cannot be retained in some cases */
         if(GT_TRUE == PRV_CPSS_DXCH_ERRATA_GET_MAC(devNum,
                                                    PRV_CPSS_DXCH_XCAT_PASSENGER_ETH_PACKET_CRC_CANT_BE_RETAINED_WA_E))
         {
             if (logicFormatPtr->mplsCfg.retainCRC == GT_TRUE)
                 CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_SUPPORTED, LOG_ERROR_NO_MSG);
         }

        controlWordIndex = (logicFormatPtr->mplsCfg.controlWordEnable == GT_TRUE ? logicFormatPtr->mplsCfg.controlWordIndex + 1 : 0);
        if(PRV_CPSS_DXCH_PP_HW_INFO_VPLS_MODE_SUPPORTED_MAC(devNum) == GT_TRUE)
        {
            if (controlWordIndex > CONTROL_WORD_INDEX_XCAT_MAX_CNS)
            {
                CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
            }
        }
        else
        {
            if (controlWordIndex > CONTROL_WORD_INDEX_MAX_CNS)
            {
                CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
            }
        }


        break;

    case CPSS_TUNNEL_MAC_IN_MAC_E:
        /*******************************/
        /* check MIM config parameters */
        /*******************************/
        PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
             CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E);

        if (logicFormatPtr->mimCfg.tagEnable == GT_TRUE)
        {
            PRV_CPSS_VLAN_VALUE_CHECK_MAC(logicFormatPtr->mimCfg.vlanId);
            PRV_TUNNEL_START_QOS_MARK_MODE_CHECK_MAC(logicFormatPtr->mimCfg.upMarkMode);
            if (logicFormatPtr->mimCfg.upMarkMode == CPSS_DXCH_TUNNEL_START_QOS_MARK_FROM_ENTRY_E)
                PRV_CPSS_DXCH_COS_CHECK_UP_MAC(logicFormatPtr->mimCfg.up);
        }
        PRV_TUNNEL_START_ISID_CHECK_MAC(logicFormatPtr->mimCfg.iSid);
        PRV_TUNNEL_START_QOS_MARK_MODE_CHECK_MAC(logicFormatPtr->mimCfg.iUpMarkMode);
        if (logicFormatPtr->mimCfg.iUpMarkMode == CPSS_DXCH_TUNNEL_START_QOS_MARK_FROM_ENTRY_E)
            PRV_CPSS_DXCH_COS_CHECK_UP_MAC(logicFormatPtr->mimCfg.iUp);
        PRV_TUNNEL_START_QOS_MARK_MODE_CHECK_MAC(logicFormatPtr->mimCfg.iDpMarkMode);
        if (logicFormatPtr->mimCfg.iDpMarkMode == CPSS_DXCH_TUNNEL_START_QOS_MARK_FROM_ENTRY_E)
            PRV_CPSS_DXCH_COS_CHECK_UP_MAC(logicFormatPtr->mimCfg.iDp);
        PRV_TUNNEL_START_ITAG_RES_CHECK_MAC(logicFormatPtr->mimCfg.iTagReserved);

        /* FEr#2785: The passenger Ethernet packet CRC cannot be retained in some cases */
         if(GT_TRUE == PRV_CPSS_DXCH_ERRATA_GET_MAC(devNum,
                                                    PRV_CPSS_DXCH_XCAT_PASSENGER_ETH_PACKET_CRC_CANT_BE_RETAINED_WA_E))
         {
             if (logicFormatPtr->mimCfg.retainCrc == GT_TRUE)
                 CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_SUPPORTED, LOG_ERROR_NO_MSG);
         }

        break;
    case CPSS_TUNNEL_TRILL_E:
        /*********************************/
        /* check TRILL config parameters */
        /*********************************/
        PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
             CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E |
             CPSS_LION_E | CPSS_XCAT2_E | CPSS_LION2_E);

            if (logicFormatPtr->trillCfg.tagEnable == GT_TRUE)
            {
                PRV_CPSS_VLAN_VALUE_CHECK_MAC(logicFormatPtr->trillCfg.vlanId);
                PRV_TUNNEL_START_QOS_MARK_MODE_CHECK_MAC(logicFormatPtr->trillCfg.upMarkMode);
                if (logicFormatPtr->trillCfg.upMarkMode == CPSS_DXCH_TUNNEL_START_QOS_MARK_FROM_ENTRY_E)
                    PRV_CPSS_DXCH_COS_CHECK_UP_MAC(logicFormatPtr->trillCfg.up);
            }
        if (logicFormatPtr->trillCfg.trillHeader.version >= BIT_2)
                CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
        if (logicFormatPtr->trillCfg.trillHeader.opLength >= BIT_5)
                CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
        if (logicFormatPtr->trillCfg.trillHeader.hopCount >= BIT_6)
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
        if (logicFormatPtr->trillCfg.trillHeader.eRbid >= BIT_16)
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
        if (logicFormatPtr->trillCfg.trillHeader.iRbid >= BIT_16)
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);

        break;
    default:
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    /*****************************************/
    /* convert paramteres to hardware format */
    /*****************************************/

    if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_MAC(devNum) == GT_TRUE)
    {
        return tunnelStartConfigLogic2HwFormat_lion3(devNum,
            tunnelType,logicFormatPtr,hwFormatArray);
    }



    switch (tunnelType)
    {
    case CPSS_TUNNEL_X_OVER_IPV4_E:
    case CPSS_TUNNEL_X_OVER_GRE_IPV4_E:
        PRV_TUNNEL_START_CONVERT_QOS_MARK_MODE_TO_HW_FORMAT_MAC(logicFormatPtr->ipv4Cfg.upMarkMode,upMarkMode);
        PRV_TUNNEL_START_CONVERT_QOS_MARK_MODE_TO_HW_FORMAT_MAC(logicFormatPtr->ipv4Cfg.dscpMarkMode,dscpMarkMode);

        /* prepare hw format */
        hwFormatArray[0] = 0x1 | /* X-over-IPv4 tunnel */
            ((upMarkMode & 0x1) << 2) |
            ((logicFormatPtr->ipv4Cfg.up & 0x7) << 3) |
            (((logicFormatPtr->ipv4Cfg.tagEnable == GT_TRUE) ? 1 : 0) << 6) |
            ((logicFormatPtr->ipv4Cfg.vlanId & 0xfff) << 7) |
            ((logicFormatPtr->ipv4Cfg.ttl & 0xff) << 19) |
            ((logicFormatPtr->ipv4Cfg.cfi & 0x1) << 27);

        hwFormatArray[1] = GT_HW_MAC_LOW32((&logicFormatPtr->ipv4Cfg.macDa));

        hwFormatArray[2] = GT_HW_MAC_HIGH16((&logicFormatPtr->ipv4Cfg.macDa));

        hwFormatArray[3] = (logicFormatPtr->ipv4Cfg.autoTunnelOffset & 0xf) |
            (((logicFormatPtr->ipv4Cfg.autoTunnel == GT_TRUE) ? 1 : 0) << 4) |
            (((tunnelType == CPSS_TUNNEL_X_OVER_GRE_IPV4_E) ? 1 : 0) << 5) |
            ((logicFormatPtr->ipv4Cfg.dscp & 0x3f) << 6) |
            ((dscpMarkMode & 0x1) << 12) |
            (((logicFormatPtr->ipv4Cfg.dontFragmentFlag == GT_TRUE) ? 1 : 0) << 13);

        hwFormatArray[4] = (logicFormatPtr->ipv4Cfg.destIp.arIP[0] << 24) |
            (logicFormatPtr->ipv4Cfg.destIp.arIP[1] << 16) |
            (logicFormatPtr->ipv4Cfg.destIp.arIP[2] << 8) |
            (logicFormatPtr->ipv4Cfg.destIp.arIP[3]);

        hwFormatArray[5] = (logicFormatPtr->ipv4Cfg.srcIp.arIP[0] << 24) |
            (logicFormatPtr->ipv4Cfg.srcIp.arIP[1] << 16) |
            (logicFormatPtr->ipv4Cfg.srcIp.arIP[2] << 8) |
            (logicFormatPtr->ipv4Cfg.srcIp.arIP[3]);

        break;

    case CPSS_TUNNEL_X_OVER_MPLS_E:
        PRV_TUNNEL_START_CONVERT_QOS_MARK_MODE_TO_HW_FORMAT_MAC(logicFormatPtr->mplsCfg.upMarkMode,upMarkMode);
        PRV_TUNNEL_START_CONVERT_QOS_MARK_MODE_TO_HW_FORMAT_MAC(logicFormatPtr->mplsCfg.exp1MarkMode,exp1MarkMode);
        PRV_TUNNEL_START_CONVERT_QOS_MARK_MODE_TO_HW_FORMAT_MAC(logicFormatPtr->mplsCfg.exp2MarkMode,exp2MarkMode);
        PRV_TUNNEL_START_CONVERT_QOS_MARK_MODE_TO_HW_FORMAT_MAC(logicFormatPtr->mplsCfg.exp3MarkMode,exp3MarkMode);
        PRV_TUNNEL_START_CONVERT_TTL_MODE_TO_HW_FORMAT_MAC(logicFormatPtr->mplsCfg.ttlMode,ttlMode);

        /* prepare hw format */
        hwFormatArray[0] = 0x0 | /* X-over-MPLS tunnel */
            ((upMarkMode & 0x1) << 2) |
            ((logicFormatPtr->mplsCfg.up & 0x7) << 3) |
            (((logicFormatPtr->mplsCfg.tagEnable == GT_TRUE) ? 1 : 0) << 6) |
            ((logicFormatPtr->mplsCfg.vlanId & 0xfff) << 7) |
            ((logicFormatPtr->mplsCfg.ttl & 0xff) << 19) |
            ((logicFormatPtr->mplsCfg.cfi & 0x1) << 27);

        hwFormatArray[1] = GT_HW_MAC_LOW32((&logicFormatPtr->mplsCfg.macDa));

        hwFormatArray[2] = GT_HW_MAC_HIGH16((&logicFormatPtr->mplsCfg.macDa));

        hwFormatArray[3] = ((logicFormatPtr->mplsCfg.label1 & 0xfffff) << 0) |
            ((logicFormatPtr->mplsCfg.exp1 & 0x7) << 20) |
            ((exp1MarkMode & 0x1) << 23) |
            (((logicFormatPtr->mplsCfg.numLabels - 1) & 0x1) << 24) |
            (((logicFormatPtr->mplsCfg.retainCRC == GT_TRUE) ? 1 : 0) << 25) |
            ((((logicFormatPtr->mplsCfg.numLabels - 1) >> 1) & 0x1) << 26) |
            (((logicFormatPtr->mplsCfg.setSBit == GT_TRUE) ? 1 : 0) << 27) |
            ((ttlMode & 0x3) << 28);

        hwFormatArray[4] = ((logicFormatPtr->mplsCfg.label2 & 0xfffff) << 0) |
            ((logicFormatPtr->mplsCfg.exp2 & 0x7) << 20) |
            (exp2MarkMode << 23) |
            ((controlWordIndex & 0x7) << 24);

        hwFormatArray[5] = ((logicFormatPtr->mplsCfg.label3 & 0xfffff) << 0) |
            ((logicFormatPtr->mplsCfg.exp3 & 0x7) << 20) |
            (exp3MarkMode << 23);

        break;

    case CPSS_TUNNEL_MAC_IN_MAC_E:
        PRV_TUNNEL_START_CONVERT_QOS_MARK_MODE_TO_HW_FORMAT_MAC(logicFormatPtr->mimCfg.upMarkMode,upMarkMode);
        PRV_TUNNEL_START_CONVERT_QOS_MARK_MODE_TO_HW_FORMAT_MAC(logicFormatPtr->mimCfg.iUpMarkMode,iUpMarkMode);
        PRV_TUNNEL_START_CONVERT_QOS_MARK_MODE_TO_HW_FORMAT_MAC(logicFormatPtr->mimCfg.iDpMarkMode,iDpMarkMode);

        /* prepare hw format */
        hwFormatArray[0] = 0x2 | /* MIM tunnel */
            ((upMarkMode & 0x1) << 2) |
            ((logicFormatPtr->mimCfg.up & 0x7) << 3) |
            (((logicFormatPtr->mimCfg.tagEnable == GT_TRUE) ? 1 : 0) << 6) |
            ((logicFormatPtr->mimCfg.vlanId & 0xfff) << 7);

        hwFormatArray[1] = GT_HW_MAC_LOW32((&logicFormatPtr->mimCfg.macDa));

        hwFormatArray[2] = GT_HW_MAC_HIGH16((&logicFormatPtr->mimCfg.macDa));

        hwFormatArray[3] = ((logicFormatPtr->mimCfg.iSid & 0xffffff) << 0) |
            ((iUpMarkMode & 0x1) << 24) |
            (((logicFormatPtr->mimCfg.retainCrc == GT_TRUE) ? 1 : 0) << 25) |
            (((logicFormatPtr->mimCfg.iUp) & 0x7) << 26) |
            ((logicFormatPtr->mimCfg.iDp & 0x1) << 29) |
            ((iDpMarkMode & 0x1) << 30);

        hwFormatArray[4] = ((logicFormatPtr->mimCfg.iTagReserved & 0xf) << 0);

        hwFormatArray[5] = 0;

        break;

    CPSS_COVERITY_NON_ISSUE_BOOKMARK
    /* coverity[dead_error_begin] */
    default:
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    return GT_OK;
}


/*******************************************************************************
* tunnelStartConfigHw2LogicFormat
*
* DESCRIPTION:
*       Converts a given tunnel start configuration from hardware format
*       to logic format.
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; xCat3; Lion; xcat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond.
*
* INPUTS:
*       devNum          - physical device number
*       hwFormatArray   - The configuration in HW format (6 words).
*
* OUTPUTS:
*       tunnelTypePtr   - points to tunnel type
*       logicFormatPtr  - points to tunnel start configuration
*
* RETURNS:
*       GT_OK           - on success.
*       GT_BAD_PARAM    - on bad parameters.
*       GT_BAD_PTR      - on NULL pointer
*       GT_BAD_STATE    - on invalid tunnel type
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS tunnelStartConfigHw2LogicFormat
(
    IN  GT_U8                               devNum,
    IN  GT_U32                              *hwFormatArray,
    OUT CPSS_TUNNEL_TYPE_ENT                *tunnelTypePtr,
    OUT CPSS_DXCH_TUNNEL_START_CONFIG_UNT   *logicFormatPtr
)
{
    /* check parameters */
    CPSS_NULL_PTR_CHECK_MAC(hwFormatArray);
    CPSS_NULL_PTR_CHECK_MAC(tunnelTypePtr);
    CPSS_NULL_PTR_CHECK_MAC(logicFormatPtr);
    /* clear the info that should be returned to the caller , because the 'get'
       logic may skip assign of fields that not relevant , like :
       for MPLS when 'num of labels' is less then 3 , then the info about 'label 3'
       is not assigned */
    cpssOsMemSet(logicFormatPtr,0,sizeof(CPSS_DXCH_TUNNEL_START_CONFIG_UNT));

    if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_MAC(devNum) == GT_TRUE)
    {
        return
            tunnelStartConfigHw2LogicFormat_lion3(devNum,
                hwFormatArray,tunnelTypePtr,logicFormatPtr);
    }

    /* get tunnel type */
    switch (hwFormatArray[0] & 0x3)
    {
    case 0:
        *tunnelTypePtr = CPSS_TUNNEL_X_OVER_MPLS_E;
        break;
    case 1:
        if (((hwFormatArray[3] >> 5) & 0x1) == 0)
            *tunnelTypePtr = CPSS_TUNNEL_X_OVER_IPV4_E;
        else
            *tunnelTypePtr = CPSS_TUNNEL_X_OVER_GRE_IPV4_E;
        break;
    case 2:
        /* MIM is supported xCat and Lion */
        if (PRV_CPSS_DXCH_XCAT_FAMILY_CHECK_MAC(devNum))
            *tunnelTypePtr = CPSS_TUNNEL_MAC_IN_MAC_E;
        else
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_STATE, LOG_ERROR_NO_MSG);
        break;
    default:
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_STATE, LOG_ERROR_NO_MSG);
    }

    /* convert to logical format */
    switch (*tunnelTypePtr)
    {
    case CPSS_TUNNEL_X_OVER_IPV4_E:
    case CPSS_TUNNEL_X_OVER_GRE_IPV4_E:
        if (((hwFormatArray[0] >> 6) & 0x1) == 1)
            logicFormatPtr->ipv4Cfg.tagEnable = GT_TRUE;
        else
            logicFormatPtr->ipv4Cfg.tagEnable = GT_FALSE;

        if (logicFormatPtr->ipv4Cfg.tagEnable == GT_TRUE)
        {
            if (((hwFormatArray[0] >> 2) & 0x1) == 1)
                logicFormatPtr->ipv4Cfg.upMarkMode = CPSS_DXCH_TUNNEL_START_QOS_MARK_FROM_ENTRY_E;
            else
                logicFormatPtr->ipv4Cfg.upMarkMode = CPSS_DXCH_TUNNEL_START_QOS_MARK_FROM_PACKET_QOS_PROFILE_E;

            if (logicFormatPtr->ipv4Cfg.upMarkMode == CPSS_DXCH_TUNNEL_START_QOS_MARK_FROM_ENTRY_E)
                logicFormatPtr->ipv4Cfg.up = ((hwFormatArray[0] >> 3) & 0x7);

            logicFormatPtr->ipv4Cfg.vlanId = (GT_U16)((hwFormatArray[0] >> 7) & 0xfff);
        }

        logicFormatPtr->ipv4Cfg.ttl = ((hwFormatArray[0] >> 19) & 0xff);

        /* cfi bit is relevant only for DxCh3; xCat; xCat3; Lion */
        if(PRV_CPSS_DXCH3_FAMILY_CHECK_MAC(devNum))
            logicFormatPtr->ipv4Cfg.cfi = ((hwFormatArray[0] >> 27) & 0x1);

        logicFormatPtr->ipv4Cfg.macDa.arEther[5] = (GT_U8)((hwFormatArray[1] >>  0) & 0xff);
        logicFormatPtr->ipv4Cfg.macDa.arEther[4] = (GT_U8)((hwFormatArray[1] >>  8) & 0xff);
        logicFormatPtr->ipv4Cfg.macDa.arEther[3] = (GT_U8)((hwFormatArray[1] >> 16) & 0xff);
        logicFormatPtr->ipv4Cfg.macDa.arEther[2] = (GT_U8)((hwFormatArray[1] >> 24) & 0xff);
        logicFormatPtr->ipv4Cfg.macDa.arEther[1] = (GT_U8)((hwFormatArray[2] >>  0) & 0xff);
        logicFormatPtr->ipv4Cfg.macDa.arEther[0] = (GT_U8)((hwFormatArray[2] >>  8) & 0xff);

        if (((hwFormatArray[3] >> 4) & 0x1) == 1)
            logicFormatPtr->ipv4Cfg.autoTunnel = GT_TRUE;
        else
            logicFormatPtr->ipv4Cfg.autoTunnel = GT_FALSE;

        if (logicFormatPtr->ipv4Cfg.autoTunnel == GT_TRUE)
        {
            logicFormatPtr->ipv4Cfg.autoTunnelOffset = ((hwFormatArray[3]) & 0xf);
        }

        if (((hwFormatArray[3] >> 12) & 0x1) == 1)
            logicFormatPtr->ipv4Cfg.dscpMarkMode = CPSS_DXCH_TUNNEL_START_QOS_MARK_FROM_ENTRY_E;
        else
            logicFormatPtr->ipv4Cfg.dscpMarkMode = CPSS_DXCH_TUNNEL_START_QOS_MARK_FROM_PACKET_QOS_PROFILE_E;

        if (logicFormatPtr->ipv4Cfg.dscpMarkMode == CPSS_DXCH_TUNNEL_START_QOS_MARK_FROM_ENTRY_E)
        {
            logicFormatPtr->ipv4Cfg.dscp = ((hwFormatArray[3] >> 6) & 0x3f);
        }

        if (((hwFormatArray[3] >> 13) & 0x1) == 1)
            logicFormatPtr->ipv4Cfg.dontFragmentFlag = GT_TRUE;
        else
            logicFormatPtr->ipv4Cfg.dontFragmentFlag = GT_FALSE;

        logicFormatPtr->ipv4Cfg.destIp.arIP[3] = (GT_U8)((hwFormatArray[4] >>  0) & 0xff);
        logicFormatPtr->ipv4Cfg.destIp.arIP[2] = (GT_U8)((hwFormatArray[4] >>  8) & 0xff);
        logicFormatPtr->ipv4Cfg.destIp.arIP[1] = (GT_U8)((hwFormatArray[4] >> 16) & 0xff);
        logicFormatPtr->ipv4Cfg.destIp.arIP[0] = (GT_U8)((hwFormatArray[4] >> 24) & 0xff);

        logicFormatPtr->ipv4Cfg.srcIp.arIP[3] = (GT_U8)((hwFormatArray[5] >>  0) & 0xff);
        logicFormatPtr->ipv4Cfg.srcIp.arIP[2] = (GT_U8)((hwFormatArray[5] >>  8) & 0xff);
        logicFormatPtr->ipv4Cfg.srcIp.arIP[1] = (GT_U8)((hwFormatArray[5] >> 16) & 0xff);
        logicFormatPtr->ipv4Cfg.srcIp.arIP[0] = (GT_U8)((hwFormatArray[5] >> 24) & 0xff);

        break;

    case CPSS_TUNNEL_X_OVER_MPLS_E:
        if (((hwFormatArray[0] >> 6) & 0x1) == 1)
            logicFormatPtr->mplsCfg.tagEnable = GT_TRUE;
        else
            logicFormatPtr->mplsCfg.tagEnable = GT_FALSE;

        if (logicFormatPtr->mplsCfg.tagEnable == GT_TRUE)
        {
            if (((hwFormatArray[0] >> 2) & 0x1) == 1)
                logicFormatPtr->mplsCfg.upMarkMode = CPSS_DXCH_TUNNEL_START_QOS_MARK_FROM_ENTRY_E;
            else
                logicFormatPtr->mplsCfg.upMarkMode = CPSS_DXCH_TUNNEL_START_QOS_MARK_FROM_PACKET_QOS_PROFILE_E;

            if (logicFormatPtr->mplsCfg.upMarkMode == CPSS_DXCH_TUNNEL_START_QOS_MARK_FROM_ENTRY_E)
            {
                logicFormatPtr->mplsCfg.up = ((hwFormatArray[0] >> 3) & 0x7);
            }

            logicFormatPtr->mplsCfg.vlanId = (GT_U16)((hwFormatArray[0] >> 7) & 0xfff);
        }

        logicFormatPtr->mplsCfg.ttl = ((hwFormatArray[0] >> 19) & 0xff);

        if ((logicFormatPtr->mplsCfg.ttl == 0)
            && PRV_CPSS_DXCH_XCAT_FAMILY_CHECK_MAC(devNum))
        {
            switch ((hwFormatArray[3] >> 28) & 0x3)
            {
            case 0:
                logicFormatPtr->mplsCfg.ttlMode = CPSS_DXCH_TUNNEL_START_TTL_TO_INCOMING_TTL_E;
                break;
            case 1:
                logicFormatPtr->mplsCfg.ttlMode = CPSS_DXCH_TUNNEL_START_TTL_TO_SWAP_LABEL_E;
                break;
            case 2:
                logicFormatPtr->mplsCfg.ttlMode = CPSS_DXCH_TUNNEL_START_TTL_TO_SWAP_LABEL_MINUS_ONE_E;
                break;
            default:
                CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_STATE, LOG_ERROR_NO_MSG);
            }
        }

        if (PRV_CPSS_DXCH_XCAT_FAMILY_CHECK_MAC(devNum))
        {
            if (((hwFormatArray[3] >> 27) & 0x1) == 1)
                logicFormatPtr->mplsCfg.setSBit = GT_TRUE;
            else
                logicFormatPtr->mplsCfg.setSBit = GT_FALSE;
        }

        /* cfi bit is relevant only for DxCh3; xCat; xCat3; Lion */
        if(PRV_CPSS_DXCH3_FAMILY_CHECK_MAC(devNum))
        {
            logicFormatPtr->mplsCfg.cfi = ((hwFormatArray[0] >> 27) & 0x1);
        }

        logicFormatPtr->mplsCfg.macDa.arEther[5] = (GT_U8)((hwFormatArray[1] >>  0) & 0xff);
        logicFormatPtr->mplsCfg.macDa.arEther[4] = (GT_U8)((hwFormatArray[1] >>  8) & 0xff);
        logicFormatPtr->mplsCfg.macDa.arEther[3] = (GT_U8)((hwFormatArray[1] >> 16) & 0xff);
        logicFormatPtr->mplsCfg.macDa.arEther[2] = (GT_U8)((hwFormatArray[1] >> 24) & 0xff);
        logicFormatPtr->mplsCfg.macDa.arEther[1] = (GT_U8)((hwFormatArray[2] >>  0) & 0xff);
        logicFormatPtr->mplsCfg.macDa.arEther[0] = (GT_U8)((hwFormatArray[2] >>  8) & 0xff);

        logicFormatPtr->mplsCfg.numLabels = (((hwFormatArray[3] >> 24) & 0x1) |
            (((hwFormatArray[3] >> 26) & 0x1) << 1)) + 1;

        switch (logicFormatPtr->mplsCfg.numLabels)
        {
        case 3:
            /* 3 labels mode is supported for xCat and Lion */
            if(! PRV_CPSS_DXCH_XCAT_FAMILY_CHECK_MAC(devNum))
                CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_STATE, LOG_ERROR_NO_MSG);
            logicFormatPtr->mplsCfg.label3 = ((hwFormatArray[5]) & 0xfffff);
            if (((hwFormatArray[5] >> 23) & 0x1) == 1)
                logicFormatPtr->mplsCfg.exp3MarkMode = CPSS_DXCH_TUNNEL_START_QOS_MARK_FROM_ENTRY_E;
            else
                logicFormatPtr->mplsCfg.exp3MarkMode = CPSS_DXCH_TUNNEL_START_QOS_MARK_FROM_PACKET_QOS_PROFILE_E;
            if (logicFormatPtr->mplsCfg.exp3MarkMode == CPSS_DXCH_TUNNEL_START_QOS_MARK_FROM_ENTRY_E)
                logicFormatPtr->mplsCfg.exp3 = ((hwFormatArray[5] >> 20) & 0x7);
            /* note fall through to check other labels too */
        case 2:
            logicFormatPtr->mplsCfg.label2 = ((hwFormatArray[4]) & 0xfffff);
            if (((hwFormatArray[4] >> 23) & 0x1) == 1)
                logicFormatPtr->mplsCfg.exp2MarkMode = CPSS_DXCH_TUNNEL_START_QOS_MARK_FROM_ENTRY_E;
            else
                logicFormatPtr->mplsCfg.exp2MarkMode = CPSS_DXCH_TUNNEL_START_QOS_MARK_FROM_PACKET_QOS_PROFILE_E;
            if (logicFormatPtr->mplsCfg.exp2MarkMode == CPSS_DXCH_TUNNEL_START_QOS_MARK_FROM_ENTRY_E)
                logicFormatPtr->mplsCfg.exp2 = ((hwFormatArray[4] >> 20) & 0x7);
            /* note fall through to check other labels too */
        case 1:
            logicFormatPtr->mplsCfg.label1 = ((hwFormatArray[3]) & 0xfffff);
            if (((hwFormatArray[3] >> 23) & 0x1) == 1)
                logicFormatPtr->mplsCfg.exp1MarkMode = CPSS_DXCH_TUNNEL_START_QOS_MARK_FROM_ENTRY_E;
            else
                logicFormatPtr->mplsCfg.exp1MarkMode = CPSS_DXCH_TUNNEL_START_QOS_MARK_FROM_PACKET_QOS_PROFILE_E;
            if (logicFormatPtr->mplsCfg.exp1MarkMode == CPSS_DXCH_TUNNEL_START_QOS_MARK_FROM_ENTRY_E)
                logicFormatPtr->mplsCfg.exp1 = ((hwFormatArray[3] >> 20) & 0x7);
            break;
        default:
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_STATE, LOG_ERROR_NO_MSG);
        }

        if (((hwFormatArray[3] >> 25) & 0x1) == 1)
            logicFormatPtr->mplsCfg.retainCRC = GT_TRUE;
        else
            logicFormatPtr->mplsCfg.retainCRC = GT_FALSE;

        if(PRV_CPSS_DXCH_PP_HW_INFO_VPLS_MODE_SUPPORTED_MAC(devNum) == GT_TRUE)
        {
            if ((hwFormatArray[4] >> 24) & 0x7)
            {
                logicFormatPtr->mplsCfg.controlWordEnable = GT_TRUE;
                logicFormatPtr->mplsCfg.controlWordIndex = ((hwFormatArray[4] >> 24) & 0x7) - 1;
            }
        }

        break;

    case CPSS_TUNNEL_MAC_IN_MAC_E:
        if (((hwFormatArray[0] >> 6) & 0x1) == 1)
            logicFormatPtr->mimCfg.tagEnable = GT_TRUE;
        else
            logicFormatPtr->mimCfg.tagEnable = GT_FALSE;

        if (logicFormatPtr->mimCfg.tagEnable == GT_TRUE)
        {
            if (((hwFormatArray[0] >> 2) & 0x1) == 1)
                logicFormatPtr->mimCfg.upMarkMode = CPSS_DXCH_TUNNEL_START_QOS_MARK_FROM_ENTRY_E;
            else
                logicFormatPtr->mimCfg.upMarkMode = CPSS_DXCH_TUNNEL_START_QOS_MARK_FROM_PACKET_QOS_PROFILE_E;

            if (logicFormatPtr->mimCfg.upMarkMode == CPSS_DXCH_TUNNEL_START_QOS_MARK_FROM_ENTRY_E)
                logicFormatPtr->mimCfg.up = ((hwFormatArray[0] >> 3) & 0x7);

            logicFormatPtr->mimCfg.vlanId = (GT_U16)((hwFormatArray[0] >> 7) & 0xfff);
        }

        logicFormatPtr->mimCfg.macDa.arEther[5] = (GT_U8)((hwFormatArray[1] >>  0) & 0xff);
        logicFormatPtr->mimCfg.macDa.arEther[4] = (GT_U8)((hwFormatArray[1] >>  8) & 0xff);
        logicFormatPtr->mimCfg.macDa.arEther[3] = (GT_U8)((hwFormatArray[1] >> 16) & 0xff);
        logicFormatPtr->mimCfg.macDa.arEther[2] = (GT_U8)((hwFormatArray[1] >> 24) & 0xff);
        logicFormatPtr->mimCfg.macDa.arEther[1] = (GT_U8)((hwFormatArray[2] >>  0) & 0xff);
        logicFormatPtr->mimCfg.macDa.arEther[0] = (GT_U8)((hwFormatArray[2] >>  8) & 0xff);

        logicFormatPtr->mimCfg.iSid = ((hwFormatArray[3] >> 0) & 0xffffff);

        if (((hwFormatArray[3] >> 25) & 0x1) == 1)
            logicFormatPtr->mimCfg.retainCrc = GT_TRUE;
        else
            logicFormatPtr->mimCfg.retainCrc = GT_FALSE;

        if (((hwFormatArray[3] >> 24) & 0x1) == 1)
            logicFormatPtr->mimCfg.iUpMarkMode = CPSS_DXCH_TUNNEL_START_QOS_MARK_FROM_ENTRY_E;
        else
            logicFormatPtr->mimCfg.iUpMarkMode = CPSS_DXCH_TUNNEL_START_QOS_MARK_FROM_PACKET_QOS_PROFILE_E;

        if (logicFormatPtr->mimCfg.iUpMarkMode == CPSS_DXCH_TUNNEL_START_QOS_MARK_FROM_ENTRY_E)
            logicFormatPtr->mimCfg.iUp = ((hwFormatArray[3] >> 26) & 0x7);

        if (((hwFormatArray[3] >> 30) & 0x1) == 1)
            logicFormatPtr->mimCfg.iDpMarkMode = CPSS_DXCH_TUNNEL_START_QOS_MARK_FROM_ENTRY_E;
        else
            logicFormatPtr->mimCfg.iDpMarkMode = CPSS_DXCH_TUNNEL_START_QOS_MARK_FROM_PACKET_QOS_PROFILE_E;

        if (logicFormatPtr->mimCfg.iDpMarkMode == CPSS_DXCH_TUNNEL_START_QOS_MARK_FROM_ENTRY_E)
            logicFormatPtr->mimCfg.iDp = ((hwFormatArray[3] >> 29) & 0x1);

        logicFormatPtr->mimCfg.iTagReserved = ((hwFormatArray[4] >> 0) & 0xf);

        break;

    default:
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_STATE, LOG_ERROR_NO_MSG);
    }

    return GT_OK;
}


/*******************************************************************************
* ch2Only_tunnelTermConfigLogic2HwFormat
*
* DESCRIPTION:
*       Converts a given tunnel termination configuration from logic format
*       to hardware format.
*
* APPLICABLE DEVICES:
*        DxCh2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       tunnelType        - type of the tunnel; valid options:
*                           CPSS_TUNNEL_IPV4_OVER_IPV4_E
*                           CPSS_TUNNEL_IPV4_OVER_GRE_IPV4_E
*                           CPSS_TUNNEL_IPV6_OVER_IPV4_E
*                           CPSS_TUNNEL_IPV6_OVER_GRE_IPV4_E
*                           CPSS_TUNNEL_X_OVER_MPLS_E
*       convertMask       - whether to convert config or config mask
*       logicFormatPtr    - points to tunnel termination configuration in
*                           logic format
*
* OUTPUTS:
*       hwFormatArray     - the configuration in HW format (6 words).
*
* RETURNS:
*       GT_OK             - on success.
*       GT_BAD_PARAM      - on bad parameters.
*       GT_BAD_PTR        - on NULL pointer
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS ch2Only_tunnelTermConfigLogic2HwFormat
(
    IN  CPSS_TUNNEL_TYPE_ENT                tunnelType,
    IN  GT_BOOL                             convertMask,
    IN  CPSS_DXCH_TUNNEL_TERM_CONFIG_UNT    *logicFormatPtr,
    OUT GT_U32                              *hwFormatArray
)
{
    GT_BOOL     isXoverMplsTunnel;  /* if tunnel type is X-over-MPLS tunnel */
    GT_U32      ipv4TunnelTypeShift[] = {11,13,12,14}; /* ipv4 tunnel type to word 2 of hwFormatArray */

    CPSS_NULL_PTR_CHECK_MAC(logicFormatPtr);
    CPSS_NULL_PTR_CHECK_MAC(hwFormatArray);

     /* zero out hw format */
    cpssOsMemSet(hwFormatArray,0,sizeof(GT_U32)*TUNNEL_TERMINATION_TCAM_RULE_SIZE_CNS);

    /* classify tunnel type */
    switch (tunnelType)
    {
    case CPSS_TUNNEL_IPV4_OVER_IPV4_E:
    case CPSS_TUNNEL_IPV4_OVER_GRE_IPV4_E:
    case CPSS_TUNNEL_IPV6_OVER_IPV4_E:
    case CPSS_TUNNEL_IPV6_OVER_GRE_IPV4_E:
        isXoverMplsTunnel = GT_FALSE;
        break;
    case CPSS_TUNNEL_X_OVER_MPLS_E:
        isXoverMplsTunnel = GT_TRUE;
        break;
    CPSS_COVERITY_NON_ISSUE_BOOKMARK
    /* coverity[dead_error_begin] */default:
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    /* check ipv4/mpls configuration pararmeters only for config, not for mask */
    if (convertMask == GT_FALSE)
    {
        /* check ipv4 parameters */
        if (isXoverMplsTunnel == GT_FALSE)
        {
            if(logicFormatPtr->ipv4Cfg.srcDev >= BIT_5) CPSS_LOG_ERROR_AND_RETURN_MAC(GT_OUT_OF_RANGE, LOG_ERROR_NO_MSG);

            switch (logicFormatPtr->ipv4Cfg.srcIsTrunk)
            {
            case 0: /* src is not trunk */

                if(logicFormatPtr->ipv4Cfg.srcPortTrunk >= BIT_6) CPSS_LOG_ERROR_AND_RETURN_MAC(GT_OUT_OF_RANGE, LOG_ERROR_NO_MSG);

                break;
            case 1: /* src is trunk */
                if (logicFormatPtr->ipv4Cfg.srcPortTrunk > TRUNK_INTERFACE_MAX_CNS)
                    CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
                break;
            default:
                CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
            }
            PRV_CPSS_VLAN_VALUE_CHECK_MAC(logicFormatPtr->ipv4Cfg.vid);
        }
        /* check mpls parameters */
        else
        {
            if(logicFormatPtr->mplsCfg.srcDev >= BIT_5) CPSS_LOG_ERROR_AND_RETURN_MAC(GT_OUT_OF_RANGE, LOG_ERROR_NO_MSG);

            switch (logicFormatPtr->mplsCfg.srcIsTrunk)
            {
            case 0: /* src is not trunk */
                if(logicFormatPtr->mplsCfg.srcPortTrunk >= BIT_6) CPSS_LOG_ERROR_AND_RETURN_MAC(GT_OUT_OF_RANGE, LOG_ERROR_NO_MSG);

                break;
            case 1: /* src is trunk */
                if (logicFormatPtr->mplsCfg.srcPortTrunk > TRUNK_INTERFACE_MAX_CNS)
                    CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
                break;
            default:
                CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
            }
            PRV_CPSS_VLAN_VALUE_CHECK_MAC(logicFormatPtr->mplsCfg.vid);
            PRV_TUNNEL_START_MPLS_LABEL_CHECK_MAC(logicFormatPtr->mplsCfg.label1);
            PRV_TUNNEL_START_MPLS_SBIT_CHECK_MAC(logicFormatPtr->mplsCfg.sBit1);
            PRV_TUNNEL_START_MPLS_EXP_CHECK_MAC(logicFormatPtr->mplsCfg.exp1);
            PRV_TUNNEL_START_MPLS_LABEL_CHECK_MAC(logicFormatPtr->mplsCfg.label2);
            PRV_TUNNEL_START_MPLS_SBIT_CHECK_MAC(logicFormatPtr->mplsCfg.sBit2);
            PRV_TUNNEL_START_MPLS_EXP_CHECK_MAC(logicFormatPtr->mplsCfg.exp2);
        }
    }

    /* convert X-over-IPv4 tunnel termination config to hw format */
    if (isXoverMplsTunnel == GT_FALSE)
    {
        /* handle word 0 (bits 0-31) */
        hwFormatArray[0] |= (logicFormatPtr->ipv4Cfg.srcPortTrunk & 0x7f) |
            ((logicFormatPtr->ipv4Cfg.srcIsTrunk & 0x1) << 7) |
            ((logicFormatPtr->ipv4Cfg.srcDev & 0x1f) << 8) |
            ((logicFormatPtr->ipv4Cfg.vid & 0xfff) << 14) |
            ((logicFormatPtr->ipv4Cfg.macDa.arEther[5] & 0x3f) << 26);

        /* handle word 1 (bits 32-63) */
        hwFormatArray[1] |= ((logicFormatPtr->ipv4Cfg.macDa.arEther[5] >> 6) & 0x3) |
            (logicFormatPtr->ipv4Cfg.macDa.arEther[4] << 2) |
            (logicFormatPtr->ipv4Cfg.macDa.arEther[3] << 10) |
            (logicFormatPtr->ipv4Cfg.macDa.arEther[2] << 18) |
            ((logicFormatPtr->ipv4Cfg.macDa.arEther[1] & 0x3f) << 26);

        /* handle word 2 (bits 64- 95) */
        hwFormatArray[2] |= ((logicFormatPtr->ipv4Cfg.macDa.arEther[1] >> 6) & 0x3) |
            (logicFormatPtr->ipv4Cfg.macDa.arEther[0] << 2) |
            (logicFormatPtr->ipv4Cfg.srcIp.arIP[3] << 16) |
            (logicFormatPtr->ipv4Cfg.srcIp.arIP[2] << 24);

        /* add ipv4 tunnel type to word 2 */
        hwFormatArray[2] |= (0x1 << ipv4TunnelTypeShift[tunnelType]);

        /* handle word 3 (bits 94-127) */
        hwFormatArray[3] |= (logicFormatPtr->ipv4Cfg.srcIp.arIP[1]) |
            (logicFormatPtr->ipv4Cfg.srcIp.arIP[0] << 8) |
            (0x1 << 31); /* mark TCAM rule as tunnel termination */

        /* handle word 4 (bits 128-159) */
        hwFormatArray[4] |= (logicFormatPtr->ipv4Cfg.destIp.arIP[3]) |
            (logicFormatPtr->ipv4Cfg.destIp.arIP[2] << 8) |
            (logicFormatPtr->ipv4Cfg.destIp.arIP[1] << 16) |
            (logicFormatPtr->ipv4Cfg.destIp.arIP[0] << 24);

        /* word 5 (bits 160-191) - reserved */
    }
    /* convert X-over-MPLS tunnel termination config to hw format */
    else
    {
        /* handle word 0 (bits 0-31) */
        hwFormatArray[0] |= (logicFormatPtr->mplsCfg.srcPortTrunk & 0x7f) |
            ((logicFormatPtr->mplsCfg.srcIsTrunk & 0x1) << 7) |
            ((logicFormatPtr->mplsCfg.srcDev & 0x1f) << 8) |
            ((logicFormatPtr->mplsCfg.vid & 0xfff) << 14) |
            ((logicFormatPtr->mplsCfg.macDa.arEther[5] & 0x3f) << 26);

        /* handle word 1 (bits 32-63) */
        hwFormatArray[1] |= ((logicFormatPtr->mplsCfg.macDa.arEther[5] >> 6) & 0x3) |
            (logicFormatPtr->mplsCfg.macDa.arEther[4] << 2) |
            (logicFormatPtr->mplsCfg.macDa.arEther[3] << 10) |
            (logicFormatPtr->mplsCfg.macDa.arEther[2] << 18) |
            ((logicFormatPtr->mplsCfg.macDa.arEther[1] & 0x3f) << 26);

        /* handle word 2 (bits 64- 95) */
        hwFormatArray[2] |= ((logicFormatPtr->mplsCfg.macDa.arEther[1] >> 6) & 0x3) |
            (logicFormatPtr->mplsCfg.macDa.arEther[0] << 2) |
            (0x1 << 10) | /* tunnel key type == MPLS, can't be masked */
            ((logicFormatPtr->mplsCfg.exp2 & 0x7) << 12) |
            ((logicFormatPtr->mplsCfg.sBit2 & 0x1) << 15) |
            ((logicFormatPtr->mplsCfg.label2 & 0xffff) << 16);

        /* handle word 3 (bits 94-127) */
        hwFormatArray[3] |= ((logicFormatPtr->mplsCfg.label2 >> 16) & 0xf) |
            ((logicFormatPtr->mplsCfg.exp1 & 0x7) << 4) |
            ((logicFormatPtr->mplsCfg.sBit1 & 0x1) << 7) |
            ((logicFormatPtr->mplsCfg.label1 & 0xfffff) << 8) |
            (0x1 << 31); /* mark TCAM rule as tunnel termination */

        /* word 4 (bits 128-159) - reserved */
        /* word 5 (bits 160-191) - reserved */
    }

    return GT_OK;
}


/*******************************************************************************
* ch2Only_tunnelTermConfigHw2LogicFormat
*
* DESCRIPTION:
*       Converts a given tunnel termination configuration from hardware format
*       to logic format.
*
* APPLICABLE DEVICES:
*        DxCh2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       hwFormatArray     - tunnel termination configuration in HW format (6 words).
*
* OUTPUTS:
*       tunnelTypePtr     - points to tunnel type; valid options:
*                           CPSS_TUNNEL_IPV4_OVER_IPV4_E
*                           CPSS_TUNNEL_IPV4_OVER_GRE_IPV4_E
*                           CPSS_TUNNEL_IPV6_OVER_IPV4_E
*                           CPSS_TUNNEL_IPV6_OVER_GRE_IPV4_E
*                           CPSS_TUNNEL_X_OVER_MPLS_E
*       logicFormatPtr    - points to tunnel termination configuration in
*                           logic format
*
* RETURNS:
*       GT_OK             - on success.
*       GT_BAD_PARAM      - on bad parameters.
*       GT_BAD_PTR        - on NULL pointer
*       GT_BAD_STATE      - on invalid tunnel type
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS ch2Only_tunnelTermConfigHw2LogicFormat
(
    IN  GT_U32                              *hwFormatArray,
    OUT CPSS_TUNNEL_TYPE_ENT                *tunnelTypePtr,
    OUT CPSS_DXCH_TUNNEL_TERM_CONFIG_UNT    *logicFormatPtr
)
{
    GT_BOOL     isXoverMplsTunnel;  /* if tunnel type is X-over-MPLS tunnel */

    /* check parameters */
    CPSS_NULL_PTR_CHECK_MAC(hwFormatArray);
    CPSS_NULL_PTR_CHECK_MAC(tunnelTypePtr);
    CPSS_NULL_PTR_CHECK_MAC(logicFormatPtr);

    /* first check that config is for tunnel termination */
    if (((hwFormatArray[3] >> 31) & 0x1) != 1)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_STATE, LOG_ERROR_NO_MSG);
    }

    /* get tunnel type */
    if (((hwFormatArray[2] >> 10) & 0x1) == 1)
    {
        isXoverMplsTunnel = GT_TRUE;
        *tunnelTypePtr = CPSS_TUNNEL_X_OVER_MPLS_E;
    }
    else
    {
        isXoverMplsTunnel = GT_FALSE;
        switch ((hwFormatArray[2] >> 11) & 0xf)
        {
        case 0x1:
            *tunnelTypePtr = CPSS_TUNNEL_IPV4_OVER_IPV4_E;
            break;
        case 0x2:
            *tunnelTypePtr = CPSS_TUNNEL_IPV6_OVER_IPV4_E;
            break;
        case 0x4:
            *tunnelTypePtr = CPSS_TUNNEL_IPV4_OVER_GRE_IPV4_E;
            break;
        case 0x8:
            *tunnelTypePtr = CPSS_TUNNEL_IPV6_OVER_GRE_IPV4_E;
            break;
        default:
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_STATE, LOG_ERROR_NO_MSG);
        }
    }

    /* get X-over-IPv4/GRE_IPv4 tunnel termination configuration */
    if (isXoverMplsTunnel == GT_FALSE)
    {
        logicFormatPtr->ipv4Cfg.srcPortTrunk = ((hwFormatArray[0]) & 0x7f);

        if (((hwFormatArray[0] >> 7) & 0x1) == 1)
            logicFormatPtr->ipv4Cfg.srcIsTrunk = GT_TRUE;
        else
            logicFormatPtr->ipv4Cfg.srcIsTrunk = GT_FALSE;

        logicFormatPtr->ipv4Cfg.srcDev = (GT_U8)((hwFormatArray[0] >> 8) & 0x1f);

        logicFormatPtr->ipv4Cfg.vid = (GT_U16)((hwFormatArray[0] >> 14) & 0xfff);

        logicFormatPtr->ipv4Cfg.macDa.arEther[5] =
            (GT_U8)(((hwFormatArray[1] & 0x3) << 6) | ((hwFormatArray[0] >> 26) & 0x3f));
        logicFormatPtr->ipv4Cfg.macDa.arEther[4] = (GT_U8)((hwFormatArray[1] >>  2) & 0xff);
        logicFormatPtr->ipv4Cfg.macDa.arEther[3] = (GT_U8)((hwFormatArray[1] >> 10) & 0xff);
        logicFormatPtr->ipv4Cfg.macDa.arEther[2] = (GT_U8)((hwFormatArray[1] >> 18) & 0xff);
        logicFormatPtr->ipv4Cfg.macDa.arEther[1] =
            (GT_U8)(((hwFormatArray[2] & 0x3) << 6) | ((hwFormatArray[1] >> 26) & 0x3f));
        logicFormatPtr->ipv4Cfg.macDa.arEther[0] = (GT_U8)((hwFormatArray[2] >>  2) & 0xff);

        logicFormatPtr->ipv4Cfg.srcIp.arIP[3] = (GT_U8)((hwFormatArray[2] >> 16) & 0xff);
        logicFormatPtr->ipv4Cfg.srcIp.arIP[2] = (GT_U8)((hwFormatArray[2] >> 24) & 0xff);
        logicFormatPtr->ipv4Cfg.srcIp.arIP[1] = (GT_U8)((hwFormatArray[3] >>  0) & 0xff);
        logicFormatPtr->ipv4Cfg.srcIp.arIP[0] = (GT_U8)((hwFormatArray[3] >>  8) & 0xff);

        logicFormatPtr->ipv4Cfg.destIp.arIP[3] = (GT_U8)((hwFormatArray[4] >>  0) & 0xff);
        logicFormatPtr->ipv4Cfg.destIp.arIP[2] = (GT_U8)((hwFormatArray[4] >>  8) & 0xff);
        logicFormatPtr->ipv4Cfg.destIp.arIP[1] = (GT_U8)((hwFormatArray[4] >> 16) & 0xff);
        logicFormatPtr->ipv4Cfg.destIp.arIP[0] = (GT_U8)((hwFormatArray[4] >> 24) & 0xff);
    }
    /* get X-over-MPLS tunnel termination configuration */
    else
    {
        logicFormatPtr->mplsCfg.srcPortTrunk = ((hwFormatArray[0]) & 0x7f);

        if (((hwFormatArray[0] >> 7) & 0x1) == 1)
            logicFormatPtr->mplsCfg.srcIsTrunk = GT_TRUE;
        else
            logicFormatPtr->mplsCfg.srcIsTrunk = GT_FALSE;

        logicFormatPtr->mplsCfg.srcDev = (GT_U8)((hwFormatArray[0] >> 8) & 0x1f);

        logicFormatPtr->mplsCfg.vid = (GT_U16)((hwFormatArray[0] >> 14) & 0xfff);

        logicFormatPtr->mplsCfg.macDa.arEther[5] =
            (GT_U8)(((hwFormatArray[1] & 0x3) << 6) | ((hwFormatArray[0] >> 26) & 0x3f));
        logicFormatPtr->mplsCfg.macDa.arEther[4] = (GT_U8)((hwFormatArray[1] >>  2) & 0xff);
        logicFormatPtr->mplsCfg.macDa.arEther[3] = (GT_U8)((hwFormatArray[1] >> 10) & 0xff);
        logicFormatPtr->mplsCfg.macDa.arEther[2] = (GT_U8)((hwFormatArray[1] >> 18) & 0xff);
        logicFormatPtr->mplsCfg.macDa.arEther[1] =
            (GT_U8)(((hwFormatArray[2] & 0x3) << 6) | ((hwFormatArray[1] >> 26) & 0x3f));
        logicFormatPtr->mplsCfg.macDa.arEther[0] = (GT_U8)((hwFormatArray[2] >>  2) & 0xff);

        logicFormatPtr->mplsCfg.exp2 = ((hwFormatArray[2] >> 12) & 0x7);

        logicFormatPtr->mplsCfg.sBit2 = ((hwFormatArray[2] >> 15) & 0x1);

        logicFormatPtr->mplsCfg.label2 =
            (((hwFormatArray[3] & 0xf) << 16) | ((hwFormatArray[2] >> 16) & 0xffff));

        logicFormatPtr->mplsCfg.exp1 = ((hwFormatArray[2] >> 4) & 0x7);

        logicFormatPtr->mplsCfg.sBit1 = ((hwFormatArray[3] >> 7) & 0x1);

        logicFormatPtr->mplsCfg.label1 = ((hwFormatArray[3] >> 8) & 0xfffff);
    }

    return GT_OK;
}


/*******************************************************************************
* ch2Only_tunnelTermActionLogic2HwFormat
*
* DESCRIPTION:
*       Converts a given tunnel termination action from logic format to
*       hardware format.
*
* APPLICABLE DEVICES:
*        DxCh2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       tunnelType        - type of the tunnel; valid options:
*                           CPSS_TUNNEL_IP_OVER_X_E
*                           CPSS_TUNNEL_ETHERNET_OVER_MPLS_E
*       logicFormatPtr    - points to tunnel termination action in logic format
*
* OUTPUTS:
*       hwFormatArray     - tunnel termination action in hardware format (4 words)
*
* RETURNS:
*       GT_OK             - on success.
*       GT_BAD_PARAM      - on bad parameter.
*       GT_BAD_PTR        - on NULL pointer
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS ch2Only_tunnelTermActionLogic2HwFormat
(
    IN  CPSS_TUNNEL_TYPE_ENT                tunnelType,
    IN  CPSS_DXCH_TUNNEL_TERM_ACTION_STC    *logicFormatPtr,
    OUT GT_U32                              *hwFormatArray
)
{
    GT_STATUS   rc=GT_OK;
    GT_32       pktCommand;         /* tunnel termination packet forwarding */
                                    /* command (hw format) */
    GT_U32      userDefinedCpuCode; /* user defined cpu code (hw format) */
    GT_U32      passengerPktType;   /* passenger packet type (hw format) */
    GT_U32      redirect;           /* if to redirect packet */
    GT_U32      egressInterface;    /* egress interface (hw format) */
    GT_U32      vlanPrecedence;     /* VLAN assignment precedence mode (hw format) */
    GT_U32      qosMode;            /* QoS mode (hw format) */
    GT_U32      qosPrecedence;      /* QoS precedence mode (hw format) */
    GT_U32      modifyUP;           /* modift user priority mode (hw format) */
    GT_U32      modifyDSCP;         /* modify DSCP mode (hw format) */

    CPSS_NULL_PTR_CHECK_MAC(logicFormatPtr);
    CPSS_NULL_PTR_CHECK_MAC(hwFormatArray);

     /* zero out hw format */
    cpssOsMemSet(hwFormatArray,0,sizeof(GT_U32)*TUNNEL_TERMINATION_TCAM_ACTION_SIZE_CNS);

    /* check parameters that are not checked later on */

    if ((tunnelType != CPSS_TUNNEL_IP_OVER_X_E) &&
        (tunnelType != CPSS_TUNNEL_ETHERNET_OVER_MPLS_E))
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    switch (logicFormatPtr->egressInterface.type)
    {
    case CPSS_INTERFACE_VIDX_E:
        if (logicFormatPtr->egressInterface.vidx > VIDX_INTERFACE_MAX_CNS)
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
        break;
    case CPSS_INTERFACE_TRUNK_E:
        if (logicFormatPtr->egressInterface.trunkId > TRUNK_INTERFACE_MAX_CNS)
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
        break;
    case CPSS_INTERFACE_PORT_E:
         /* check that devNum value takes only 5 bits and portNum only 6 bits
           according to HW action format */
         if((logicFormatPtr->egressInterface.devPort.hwDevNum >= BIT_5) ||
            (logicFormatPtr->egressInterface.devPort.portNum >= BIT_6))
         {
             CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
         }
        break;
    default:
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    if (logicFormatPtr->tunnelStartIdx > TUNNEL_START_MAX_INDEX_CNS)
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);

    PRV_CPSS_VLAN_VALUE_CHECK_MAC(logicFormatPtr->vlanId);
    PRV_CPSS_DXCH_COS_CHECK_UP_MAC(logicFormatPtr->defaultUP);
    if (logicFormatPtr->qosProfile > QOS_PROFILE_MAX_CNS)
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    if (logicFormatPtr->policerIndex > POLICER_INDEX_MAX_CNS)
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    if (logicFormatPtr->matchCounterIndex > MATCH_COUNTER_INDEX_MAX_CNS)
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);

    /* translate tunnel termination action forwarding command parameter */
    switch (logicFormatPtr->command)
    {
    case CPSS_PACKET_CMD_ROUTE_E:
        pktCommand = 0;
        break;
    case CPSS_PACKET_CMD_MIRROR_TO_CPU_E:
        pktCommand = 1;
        break;
    case CPSS_PACKET_CMD_TRAP_TO_CPU_E:
        pktCommand = 2;
        break;
    case CPSS_PACKET_CMD_DROP_HARD_E:
        pktCommand = 3;
        break;
    case CPSS_PACKET_CMD_DROP_SOFT_E:
        pktCommand = 4;
        break;
    default:
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    /* translate tunnel termination action modify DSCP parameter */
    switch (logicFormatPtr->modifyDSCP)
    {
    case CPSS_PACKET_ATTRIBUTE_MODIFY_KEEP_PREVIOUS_E:
        modifyDSCP = 0;
        break;
    case CPSS_PACKET_ATTRIBUTE_MODIFY_ENABLE_E:
        modifyDSCP = 1;
        break;
    case CPSS_PACKET_ATTRIBUTE_MODIFY_DISABLE_E:
        modifyDSCP = 2;
        break;
    default:
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    /* translate tunnel termination action modify User Priority parameter */
    switch (logicFormatPtr->modifyUP)
    {
    case CPSS_PACKET_ATTRIBUTE_MODIFY_KEEP_PREVIOUS_E:
        modifyUP = 0;
        break;
    case CPSS_PACKET_ATTRIBUTE_MODIFY_ENABLE_E:
        modifyUP = 1;
        break;
    case CPSS_PACKET_ATTRIBUTE_MODIFY_DISABLE_E:
        modifyUP = 2;
        break;
    default:
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    /* get redirect command from tunnel type */
    if (tunnelType == CPSS_TUNNEL_ETHERNET_OVER_MPLS_E)
        redirect = 1;
    else
        redirect = 0;

    /* translate egress interface to hw format from action output interface */
    switch (logicFormatPtr->egressInterface.type)
    {
    case CPSS_INTERFACE_VIDX_E:
        egressInterface = (0x1 << 12) | /* use vidx */
            (logicFormatPtr->egressInterface.vidx & 0x3ff);
        break;
    case CPSS_INTERFACE_TRUNK_E:
        egressInterface = 0x1 | /* trunk type */
            ((logicFormatPtr->egressInterface.trunkId & 0x7f) << 1);
        break;
    case CPSS_INTERFACE_PORT_E:
        egressInterface = ((logicFormatPtr->egressInterface.devPort.portNum & 0x3f) << 1) |
            ((logicFormatPtr->egressInterface.devPort.hwDevNum & 0x1f) << 7);
        break;
    default:
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    /* translate QoS mode from action QoS mode parameter */
    switch (logicFormatPtr->qosMode)
    {
    case CPSS_DXCH_TUNNEL_QOS_KEEP_PREVIOUS_E:
        qosMode = 0;
        break;
    case CPSS_DXCH_TUNNEL_QOS_TRUST_PASS_UP_E:
        qosMode = 1;
        break;
    case CPSS_DXCH_TUNNEL_QOS_TRUST_PASS_DSCP_E:
        qosMode = 2;
        break;
    case CPSS_DXCH_TUNNEL_QOS_TRUST_PASS_UP_DSCP_E:
        qosMode = 3;
        break;
    case CPSS_DXCH_TUNNEL_QOS_UNTRUST_PKT_E:
        qosMode = 4;
        break;
    case CPSS_DXCH_TUNNEL_QOS_TRUST_MPLS_EXP_E:
        qosMode = 5;
        break;
    default:
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    /* translate passenger packet type from action parameter */
    switch (logicFormatPtr->passengerPacketType)
    {
    case CPSS_TUNNEL_PASSENGER_PACKET_IPV4_E:
        passengerPktType = 0;
        break;
    case CPSS_TUNNEL_PASSENGER_PACKET_IPV6_E:
        passengerPktType = 1;
        break;
    case CPSS_TUNNEL_PASSENGER_PACKET_ETHERNET_CRC_E:
        passengerPktType = 2;
        break;
    case CPSS_TUNNEL_PASSENGER_PACKET_ETHERNET_NO_CRC_E:
        passengerPktType = 3;
        break;
    default:
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    /* translate QoS precedence from action parameter */
    switch (logicFormatPtr->qosPrecedence)
    {
    case CPSS_PACKET_ATTRIBUTE_ASSIGN_PRECEDENCE_HARD_E:
        qosPrecedence = 1;
        break;
    case CPSS_PACKET_ATTRIBUTE_ASSIGN_PRECEDENCE_SOFT_E:
        qosPrecedence = 0;
        break;
    default:
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    /* translate VLAN precedence from action parameter */
    switch (logicFormatPtr->vlanPrecedence)
    {
    case CPSS_PACKET_ATTRIBUTE_ASSIGN_PRECEDENCE_HARD_E:
        vlanPrecedence = 1;
        break;
    case CPSS_PACKET_ATTRIBUTE_ASSIGN_PRECEDENCE_SOFT_E:
        vlanPrecedence = 0;
        break;
    default:
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    /* translate user defined cpu code from action parameter */
    /* note that cpu code is relevant only for trap or mirror commands */
    if ((logicFormatPtr->command == CPSS_PACKET_CMD_MIRROR_TO_CPU_E) ||
        (logicFormatPtr->command == CPSS_PACKET_CMD_TRAP_TO_CPU_E))
    {
        if ((logicFormatPtr->userDefinedCpuCode >= CPSS_NET_FIRST_USER_DEFINED_E) &&
            (logicFormatPtr->userDefinedCpuCode <= CPSS_NET_LAST_USER_DEFINED_E))
        {
            rc = prvCpssDxChNetIfCpuToDsaCode(logicFormatPtr->userDefinedCpuCode,
                                         (PRV_CPSS_DXCH_NET_DSA_TAG_CPU_CODE_ENT*)(GT_UINTPTR)&userDefinedCpuCode);
            if(rc != GT_OK)
                return rc;
        }
        else
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }
    else
        userDefinedCpuCode = 0;

    /* now prepare hardware format */
    hwFormatArray[0] = (pktCommand & 0x7) |
        ((userDefinedCpuCode & 0xff) << 3) |
        (((logicFormatPtr->mirrorToIngressAnalyzerEnable == GT_TRUE) ? 1 : 0) << 11) |
        ((qosPrecedence & 0x1) << 12) |
        ((logicFormatPtr->qosProfile & 0x7f) << 14) |
        ((modifyDSCP & 0x3) << 21) |
        ((modifyUP & 0x3) << 23) |
        (((logicFormatPtr->matchCounterEnable == GT_TRUE) ? 1 : 0) << 25) |
        ((logicFormatPtr->matchCounterIndex & 0x1f) << 26);

    hwFormatArray[1] = (redirect & 0x3) |
        ((egressInterface & 0x1fff) << 2) |
        ((vlanPrecedence & 0x1) << 15) |
        (((logicFormatPtr->nestedVlanEnable == GT_TRUE) ? 1 : 0) << 16) |
        ((logicFormatPtr->vlanId & 0xfff) << 19) |
        (((logicFormatPtr->remapDSCP == GT_TRUE) ? 1 : 0) << 31);

    hwFormatArray[2] = ((logicFormatPtr->policerEnable == GT_TRUE) ? 1 : 0) |
        ((logicFormatPtr->policerIndex & 0xff) << 1) |
        (((logicFormatPtr->isTunnelStart == GT_TRUE) ? 1 : 0) << 10) |
        ((logicFormatPtr->tunnelStartIdx & 0x3ff) << 11) |
        ((qosMode & 0x7) << 22) |
        ((passengerPktType & 0x3) << 25) |
        (((logicFormatPtr->copyTtlFromTunnelHeader == GT_TRUE) ? 1 : 0) << 27) |
        ((0x1) << 28) | /* this bit is reserved and must be set to 1 */
        ((logicFormatPtr->defaultUP & 0x7) << 29);

    hwFormatArray[3] = 0;

    return GT_OK;
}


/*******************************************************************************
* ch2Only_tunnelTermActionHw2LogicFormat
*
* DESCRIPTION:
*       Converts a given tunnel termination action from hardware format to
*       logic format.
*
* APPLICABLE DEVICES:
*        DxCh2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       hwFormatArray     - tunnel termination action in hardware format (4 words)
*
* OUTPUTS:
*       tunnelTypePtr     - points to type of the tunnel; valid options:
*                           CPSS_TUNNEL_IP_OVER_X_E
*                           CPSS_TUNNEL_ETHERNET_OVER_MPLS_E
*       logicFormatPtr    - points to tunnel termination action in logic format
*
* RETURNS:
*       GT_OK             - on success.
*       GT_BAD_PARAM      - on bad parameter.
*       GT_BAD_PTR        - on NULL pointer
*       GT_BAD_STATE      - on invalid tunnel type
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS ch2Only_tunnelTermActionHw2LogicFormat
(
    IN  GT_U32                              *hwFormatArray,
    OUT CPSS_TUNNEL_TYPE_ENT                *tunnelTypePtr,
    OUT CPSS_DXCH_TUNNEL_TERM_ACTION_STC    *logicFormatPtr
)
{
    GT_STATUS rc = GT_OK;

    /* check parameters */
    CPSS_NULL_PTR_CHECK_MAC(hwFormatArray);
    CPSS_NULL_PTR_CHECK_MAC(tunnelTypePtr);
    CPSS_NULL_PTR_CHECK_MAC(logicFormatPtr);

    /* get tunnel type */
    switch (hwFormatArray[1] & 0x3)
    {
        case 0:
            *tunnelTypePtr = CPSS_TUNNEL_IP_OVER_X_E;
            break;
        case 1:
            *tunnelTypePtr = CPSS_TUNNEL_ETHERNET_OVER_MPLS_E;
            break;
        default:
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_STATE, LOG_ERROR_NO_MSG);
    }

    switch (hwFormatArray[0] & 0x7)
    {
        case 0:
            logicFormatPtr->command = CPSS_PACKET_CMD_ROUTE_E;
            break;
        case 1:
            logicFormatPtr->command = CPSS_PACKET_CMD_MIRROR_TO_CPU_E;
            break;
        case 2:
            logicFormatPtr->command = CPSS_PACKET_CMD_TRAP_TO_CPU_E;
            break;
        case 3:
            logicFormatPtr->command = CPSS_PACKET_CMD_DROP_HARD_E;
            break;
        case 4:
            logicFormatPtr->command = CPSS_PACKET_CMD_DROP_SOFT_E;
            break;
        default:
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_STATE, LOG_ERROR_NO_MSG);
    }

    rc = prvCpssDxChNetIfDsaToCpuCode(((hwFormatArray[0] >> 3) & 0xff),
                                 &logicFormatPtr->userDefinedCpuCode);
    if(rc != GT_OK)
        return rc;

    if (((hwFormatArray[0] >> 11) & 0x1) == 1)
        logicFormatPtr->mirrorToIngressAnalyzerEnable = GT_TRUE;
    else
        logicFormatPtr->mirrorToIngressAnalyzerEnable = GT_FALSE;

    if (((hwFormatArray[0] >> 12) & 0x1) == 1)
        logicFormatPtr->qosPrecedence = CPSS_PACKET_ATTRIBUTE_ASSIGN_PRECEDENCE_HARD_E;
    else
        logicFormatPtr->qosPrecedence = CPSS_PACKET_ATTRIBUTE_ASSIGN_PRECEDENCE_SOFT_E;

    logicFormatPtr->qosProfile = ((hwFormatArray[0] >> 14) & 0x7f);

    switch ((hwFormatArray[0] >> 21) & 0x3)
    {
        case 0:
            logicFormatPtr->modifyDSCP = CPSS_PACKET_ATTRIBUTE_MODIFY_KEEP_PREVIOUS_E;
            break;
        case 1:
            logicFormatPtr->modifyDSCP = CPSS_PACKET_ATTRIBUTE_MODIFY_ENABLE_E;
            break;
        case 2:
            logicFormatPtr->modifyDSCP = CPSS_PACKET_ATTRIBUTE_MODIFY_DISABLE_E;
            break;
        default:
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_STATE, LOG_ERROR_NO_MSG);
    }

    switch ((hwFormatArray[0] >> 23) & 0x3)
    {
        case 0:
            logicFormatPtr->modifyUP = CPSS_PACKET_ATTRIBUTE_MODIFY_KEEP_PREVIOUS_E;
            break;
        case 1:
            logicFormatPtr->modifyUP = CPSS_PACKET_ATTRIBUTE_MODIFY_ENABLE_E;
            break;
        case 2:
            logicFormatPtr->modifyUP = CPSS_PACKET_ATTRIBUTE_MODIFY_DISABLE_E;
            break;
        default:
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_STATE, LOG_ERROR_NO_MSG);
    }

    if (((hwFormatArray[0] >> 25) & 0x1) == 1)
        logicFormatPtr->matchCounterEnable = GT_TRUE;
    else
        logicFormatPtr->matchCounterEnable = GT_FALSE;

    logicFormatPtr->matchCounterIndex = ((hwFormatArray[0] >> 26) & 0x1f);

    /* if egress interface is vidx */
    if (((hwFormatArray[1] >> 14) & 0x1) == 1)
    {
        logicFormatPtr->egressInterface.type = CPSS_INTERFACE_VIDX_E;
        logicFormatPtr->egressInterface.vidx =
            (GT_U16)((hwFormatArray[1] >> 2) & 0xfff);
    }
    /* if egress interface is trunk or {dev,port} */
    else
    {
        /* if egress interface is trunk */
        if (((hwFormatArray[1] >> 2) & 0x1) == 1)
        {
            logicFormatPtr->egressInterface.type = CPSS_INTERFACE_TRUNK_E;
            logicFormatPtr->egressInterface.trunkId =
                (GT_TRUNK_ID)((hwFormatArray[1] >> 3) & 0x7f);
        }
        /* if egress interface {dev,port} */
        else
        {
            logicFormatPtr->egressInterface.type = CPSS_INTERFACE_PORT_E;
            logicFormatPtr->egressInterface.devPort.hwDevNum =
                (GT_U8)((hwFormatArray[1] >> 9) & 0x1f);
            logicFormatPtr->egressInterface.devPort.portNum =
                (GT_U8)((hwFormatArray[1] >> 3) & 0x3f);
        }
    }

    if (((hwFormatArray[1] >> 15) & 0x1) == 1)
        logicFormatPtr->vlanPrecedence = CPSS_PACKET_ATTRIBUTE_ASSIGN_PRECEDENCE_HARD_E;
    else
        logicFormatPtr->vlanPrecedence = CPSS_PACKET_ATTRIBUTE_ASSIGN_PRECEDENCE_SOFT_E;

    if (((hwFormatArray[1] >> 16) & 0x1) == 1)
        logicFormatPtr->nestedVlanEnable = GT_TRUE;
    else
        logicFormatPtr->nestedVlanEnable = GT_FALSE;

    logicFormatPtr->vlanId = (GT_U16)((hwFormatArray[1] >> 19) & 0xfff);

    if (((hwFormatArray[1] >> 31) & 0x1) == 1)
        logicFormatPtr->remapDSCP = GT_TRUE;
    else
        logicFormatPtr->remapDSCP = GT_FALSE;

    if (((hwFormatArray[2]) & 0x1) == 1)
        logicFormatPtr->policerEnable = GT_TRUE;
    else
        logicFormatPtr->policerEnable = GT_FALSE;

    logicFormatPtr->policerIndex = ((hwFormatArray[2] >> 1) & 0xff);

    if (((hwFormatArray[2] >> 10) & 0x1) == 1)
        logicFormatPtr->isTunnelStart = GT_TRUE;
    else
        logicFormatPtr->isTunnelStart = GT_FALSE;

    logicFormatPtr->tunnelStartIdx = ((hwFormatArray[2] >> 11) & 0x3ff);

    switch ((hwFormatArray[2] >> 22) & 0x7)
    {
        case 0:
            logicFormatPtr->qosMode = CPSS_DXCH_TUNNEL_QOS_KEEP_PREVIOUS_E;
            break;
        case 1:
            logicFormatPtr->qosMode = CPSS_DXCH_TUNNEL_QOS_TRUST_PASS_UP_E;
            break;
        case 2:
            logicFormatPtr->qosMode = CPSS_DXCH_TUNNEL_QOS_TRUST_PASS_DSCP_E;
            break;
        case 3:
            logicFormatPtr->qosMode = CPSS_DXCH_TUNNEL_QOS_TRUST_PASS_UP_DSCP_E;
            break;
        case 4:
            logicFormatPtr->qosMode = CPSS_DXCH_TUNNEL_QOS_UNTRUST_PKT_E;
            break;
        case 5:
            logicFormatPtr->qosMode = CPSS_DXCH_TUNNEL_QOS_TRUST_MPLS_EXP_E;
            break;
        default:
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_STATE, LOG_ERROR_NO_MSG);
    }

    switch ((hwFormatArray[2] >> 25) & 0x3)
    {
        case 0:
            logicFormatPtr->passengerPacketType = CPSS_TUNNEL_PASSENGER_PACKET_IPV4_E;
            break;
        case 1:
            logicFormatPtr->passengerPacketType = CPSS_TUNNEL_PASSENGER_PACKET_IPV6_E;
            break;
        case 2:
            logicFormatPtr->passengerPacketType = CPSS_TUNNEL_PASSENGER_PACKET_ETHERNET_CRC_E;
            break;
        case 3:
            logicFormatPtr->passengerPacketType = CPSS_TUNNEL_PASSENGER_PACKET_ETHERNET_NO_CRC_E;
            break;
        default:
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_STATE, LOG_ERROR_NO_MSG);
    }

    if (((hwFormatArray[2] >> 27) & 0x1) == 1)
        logicFormatPtr->copyTtlFromTunnelHeader = GT_TRUE;
    else
        logicFormatPtr->copyTtlFromTunnelHeader = GT_FALSE;

    logicFormatPtr->defaultUP = ((hwFormatArray[2] >> 29) & 0x7);

    return GT_OK;
}

/*******************************************************************************
* internal_cpssDxChTunnelStartEntrySet
*
* DESCRIPTION:
*       Set a tunnel start entry.
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond.
*
* INPUTS:
*       devNum        - physical device number
*       routerArpTunnelStartLineIndex - line index for the tunnel start entry
*                       in the router ARP / tunnel start /NAT table (for NAT capable devices)
*       tunnelType    - type of the tunnel; valid values:
*                       CPSS_TUNNEL_X_OVER_IPV4_E
*                       CPSS_TUNNEL_X_OVER_GRE_IPV4_E
*                       CPSS_TUNNEL_X_OVER_MPLS_E
*                       CPSS_TUNNEL_MAC_IN_MAC_E (APPLICABLE DEVICES: xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3)
*                       CPSS_TUNNEL_TRILL_E (APPLICABLE DEVICES: Bobcat2; Caelum; Bobcat3)
*       configPtr     - points to tunnel start configuration
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - on bad parameter.
*       GT_BAD_PTR               - on NULL pointer
*       GT_FAIL                  - on failure.
*       GT_OUT_OF_RANGE          - parameter not in valid range.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       NAT entries table (for NAT capable devices), tunnel start entries table
*       and router ARP addresses table reside at the same physical memory.
*       Each line can hold:
*       - 1 NAT entry(for NAT capable devices)
*       - 1 tunnel start entry
*       - 4 router ARP addresses entries
*       NAT entry / Tunnel start entry at index n and router ARP addresses at indexes
*       4*n..4*n+3 share the same memory. For example NAT entry/tunnel start entry at
*       index 100 and router ARP addresses at indexes 400..403 share the same
*       physical memory.
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChTunnelStartEntrySet
(
    IN  GT_U8                               devNum,
    IN  GT_U32                              routerArpTunnelStartLineIndex,
    IN  CPSS_TUNNEL_TYPE_ENT                tunnelType,
    IN  CPSS_DXCH_TUNNEL_START_CONFIG_UNT   *configPtr
)
{
    return cpssDxChTunnelStartPortGroupEntrySet(devNum,
                CPSS_PORT_GROUP_UNAWARE_MODE_CNS,
                routerArpTunnelStartLineIndex,
                tunnelType,
                configPtr);
}

/*******************************************************************************
* cpssDxChTunnelStartEntrySet
*
* DESCRIPTION:
*       Set a tunnel start entry.
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond.
*
* INPUTS:
*       devNum        - physical device number
*       routerArpTunnelStartLineIndex - line index for the tunnel start entry
*                       in the router ARP / tunnel start /NAT table (for NAT capable devices)
*       tunnelType    - type of the tunnel; valid values:
*                       CPSS_TUNNEL_X_OVER_IPV4_E
*                       CPSS_TUNNEL_X_OVER_GRE_IPV4_E
*                       CPSS_TUNNEL_X_OVER_MPLS_E
*                       CPSS_TUNNEL_MAC_IN_MAC_E (APPLICABLE DEVICES: xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3)
*                       CPSS_TUNNEL_TRILL_E (APPLICABLE DEVICES: Bobcat2; Caelum; Bobcat3)
*       configPtr     - points to tunnel start configuration
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - on bad parameter.
*       GT_BAD_PTR               - on NULL pointer
*       GT_FAIL                  - on failure.
*       GT_OUT_OF_RANGE          - parameter not in valid range.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       NAT entries table (for NAT capable devices), tunnel start entries table
*       and router ARP addresses table reside at the same physical memory.
*       Each line can hold:
*       - 1 NAT entry(for NAT capable devices)
*       - 1 tunnel start entry
*       - 4 router ARP addresses entries
*       NAT entry / Tunnel start entry at index n and router ARP addresses at indexes
*       4*n..4*n+3 share the same memory. For example NAT entry/tunnel start entry at
*       index 100 and router ARP addresses at indexes 400..403 share the same
*       physical memory.
*
*******************************************************************************/
GT_STATUS cpssDxChTunnelStartEntrySet
(
    IN  GT_U8                               devNum,
    IN  GT_U32                              routerArpTunnelStartLineIndex,
    IN  CPSS_TUNNEL_TYPE_ENT                tunnelType,
    IN  CPSS_DXCH_TUNNEL_START_CONFIG_UNT   *configPtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChTunnelStartEntrySet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, routerArpTunnelStartLineIndex, tunnelType, configPtr));

    rc = internal_cpssDxChTunnelStartEntrySet(devNum, routerArpTunnelStartLineIndex, tunnelType, configPtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, routerArpTunnelStartLineIndex, tunnelType, configPtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}



/*******************************************************************************
* internal_cpssDxChTunnelStartPortGroupGenProfileTableEntrySet
*
* DESCRIPTION:
*   This API configures Generic Tunnel-start Profile table entry.
*   Generic Tunnel-start Profile table entry serves as logical extension to
*   the Generic IPv4/6 Tunnel-Start when the IPv4/6 tunnel header includes
*   either a GRE or UDP header.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum           - device number
*       profileIndex     - entry index in Generic Tunnel-start Profile table.
*                          (APPLICABLE RANGES: 0...7).
*
*       portGroupsBmp    - bitmap of Port Groups.
*                          NOTEs:
*                          1. for non multi-port groups device this parameter is IGNORED.
*                          2. for multi-port groups device :
*                             bitmap must be set with at least one bit representing
*                             valid port group(s). If a bit of non valid port group
*                             is set then function returns GT_BAD_PARAM.
*                             value CPSS_PORT_GROUP_UNAWARE_MODE_CNS is supported.
*       profileDataPtr   - pointer to Generic Tunnel-start Profile data.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_BAD_PTR               - null pointer
*       GT_OUT_OF_RANGE          - one of the parameters is out of range
*       GT_HW_ERROR              - on hardware error
*       GT_TIMEOUT               - after max number of retries checking if PP ready
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*      The Profile table entry template generates up to 16 bytes of data
*      after the 4B GRE header or after the 8B UDP header.
*      There are 8 profiles available to use. But if application is
*      going to use legacy ip tunnel types, cpss is reserved profile index 7 for legacy support
*      (empty profile: no extension data is added in tunnel header). Also if usage of new ipv4/6
*      tunnel types take place , and these tunnels don't require profile data generation - empty
*      profile should be reserved. In this case, in order to save empty profiles number, it is
*      recommended to utilize profile 7 as well.
*      Empty profile should be configured to prevent insertion of template data into tunnel header:
*      templateDataSize = CPSS_DXCH_TUNNEL_START_TEMPLATE_DATA_SIZE_NONE_E.
*******************************************************************************/
static GT_STATUS internal_cpssDxChTunnelStartPortGroupGenProfileTableEntrySet
(
    IN GT_U8                                            devNum,
    IN GT_PORT_GROUPS_BMP                               portGroupsBmp,
    IN GT_U32                                           profileIndex,
    IN CPSS_DXCH_TUNNEL_START_GEN_IP_PROFILE_STC        *profileDataPtr
)
{
    GT_U32      hwConfigArray[TUNNEL_START_PROFILE_HW_SIZE_CNS] = {0}; /* tunnel start profile config in hardware format */
    GT_STATUS   rc;         /* function return code */
    GT_U32      portGroupId;/*the port group Id - support multi-port-groups device */


    /* check parameters */
    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E |
         CPSS_LION_E | CPSS_XCAT2_E | CPSS_LION2_E);

    PRV_CPSS_MULTI_PORT_GROUPS_BMP_CHECK_MAC(devNum,portGroupsBmp);
    PRV_TUNNEL_START_PROFILE_INDEX_CHECK_MAC(profileIndex);
    CPSS_NULL_PTR_CHECK_MAC(profileDataPtr);

    /* convert tunnel start entry to hw format while checking parameters */
    rc = tunnelStartProfileConfigLogic2HwFormat(devNum,profileDataPtr,hwConfigArray);
    if (rc != GT_OK)
    {
        return rc;
    }
    PRV_CPSS_GEN_PP_START_LOOP_PORT_GROUPS_IN_BMP_MAC(devNum,portGroupsBmp,portGroupId)
    {
    rc = prvCpssDxChPortGroupWriteTableEntry(devNum,
                                             portGroupId,
                                             PRV_CPSS_DXCH_LION3_TABLE_HA_GENERIC_TS_PROFILE_E,
                                             profileIndex,
                                             hwConfigArray);
        if(rc != GT_OK)
        {
            return rc;
        }
    }
    PRV_CPSS_GEN_PP_END_LOOP_PORT_GROUPS_IN_BMP_MAC(devNum,portGroupsBmp,portGroupId)
    return rc;
}

/*******************************************************************************
* cpssDxChTunnelStartPortGroupGenProfileTableEntrySet
*
* DESCRIPTION:
*   This API configures Generic Tunnel-start Profile table entry.
*   Generic Tunnel-start Profile table entry serves as logical extension to
*   the Generic IPv4/6 Tunnel-Start when the IPv4/6 tunnel header includes
*   either a GRE or UDP header.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum           - device number
*       profileIndex     - entry index in Generic Tunnel-start Profile table.
*                          (APPLICABLE RANGES: 0...7).
*
*       portGroupsBmp    - bitmap of Port Groups.
*                          NOTEs:
*                          1. for non multi-port groups device this parameter is IGNORED.
*                          2. for multi-port groups device :
*                             bitmap must be set with at least one bit representing
*                             valid port group(s). If a bit of non valid port group
*                             is set then function returns GT_BAD_PARAM.
*                             value CPSS_PORT_GROUP_UNAWARE_MODE_CNS is supported.
*       profileDataPtr   - pointer to Generic Tunnel-start Profile data.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_BAD_PTR               - null pointer
*       GT_OUT_OF_RANGE          - one of the parameters is out of range
*       GT_HW_ERROR              - on hardware error
*       GT_TIMEOUT               - after max number of retries checking if PP ready
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*      The Profile table entry template generates up to 16 bytes of data
*      after the 4B GRE header or after the 8B UDP header.
*      There are 8 profiles available to use. But if application is
*      going to use legacy ip tunnel types, cpss is reserved profile index 7 for legacy support
*      (empty profile: no extension data is added in tunnel header). Also if usage of new ipv4/6
*      tunnel types take place , and these tunnels don't require profile data generation - empty
*      profile should be reserved. In this case, in order to save empty profiles number, it is
*      recommended to utilize profile 7 as well.
*      Empty profile should be configured to prevent insertion of template data into tunnel header:
*      templateDataSize = CPSS_DXCH_TUNNEL_START_TEMPLATE_DATA_SIZE_NONE_E.
*******************************************************************************/
GT_STATUS cpssDxChTunnelStartPortGroupGenProfileTableEntrySet
(
    IN GT_U8                                            devNum,
    IN GT_PORT_GROUPS_BMP                               portGroupsBmp,
    IN GT_U32                                           profileIndex,
    IN CPSS_DXCH_TUNNEL_START_GEN_IP_PROFILE_STC        *profileDataPtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChTunnelStartPortGroupGenProfileTableEntrySet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, portGroupsBmp, profileIndex, profileDataPtr));

    rc = internal_cpssDxChTunnelStartPortGroupGenProfileTableEntrySet(devNum, portGroupsBmp, profileIndex, profileDataPtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, portGroupsBmp, profileIndex, profileDataPtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChTunnelStartGenProfileTableEntrySet
*
* DESCRIPTION:
*   This API configures Generic Tunnel-start Profile table entry.
*   It serves as logical extension to the Generic IPv4/6 Tunnel-Start
*   when the IPv4/6 tunnel header includes either a GRE or UDP header.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum           - device number
*       profileIndex     - entry index in Generic Tunnel-start Profile table.
*                          (APPLICABLE RANGES: 0...7).
*       profileDataPtr   - pointer to Generic Tunnel-start Profile data.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_BAD_PTR               - null pointer
*       GT_OUT_OF_RANGE          - one of the parameters is out of range
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*      The Profile table entry template generates up to 16 bytes of data
*      after the 4B GRE header or after the 8B UDP header.
*      There are 8 profiles available to use. But if application is
*      going to use legacy ip tunnel types, cpss is reserved profile index 7 for legacy support
*      (empty profile: no extension data is added in tunnel header). Also if usage of new ipv4/6
*      tunnel types take place , and these tunnels don't require profile data generation - empty
*      profile should be reserved. In this case, in order to save empty profiles number, it is
*      recommended to utilize profile 7 as well.
*      Empty profile should be configured to prevent insertion of template data into tunnel header:
*      templateDataSize = CPSS_DXCH_TUNNEL_START_TEMPLATE_DATA_SIZE_NONE_E.
*******************************************************************************/
static GT_STATUS internal_cpssDxChTunnelStartGenProfileTableEntrySet
(
    IN GT_U8                                            devNum,
    IN GT_U32                                           profileIndex,
    IN CPSS_DXCH_TUNNEL_START_GEN_IP_PROFILE_STC        *profileDataPtr
)
{
    return cpssDxChTunnelStartPortGroupGenProfileTableEntrySet(devNum,
                CPSS_PORT_GROUP_UNAWARE_MODE_CNS,
                profileIndex,
                profileDataPtr);

}

/*******************************************************************************
* cpssDxChTunnelStartGenProfileTableEntrySet
*
* DESCRIPTION:
*   This API configures Generic Tunnel-start Profile table entry.
*   It serves as logical extension to the Generic IPv4/6 Tunnel-Start
*   when the IPv4/6 tunnel header includes either a GRE or UDP header.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum           - device number
*       profileIndex     - entry index in Generic Tunnel-start Profile table.
*                          (APPLICABLE RANGES: 0...7).
*       profileDataPtr   - pointer to Generic Tunnel-start Profile data.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_BAD_PTR               - null pointer
*       GT_OUT_OF_RANGE          - one of the parameters is out of range
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*      The Profile table entry template generates up to 16 bytes of data
*      after the 4B GRE header or after the 8B UDP header.
*      There are 8 profiles available to use. But if application is
*      going to use legacy ip tunnel types, cpss is reserved profile index 7 for legacy support
*      (empty profile: no extension data is added in tunnel header). Also if usage of new ipv4/6
*      tunnel types take place , and these tunnels don't require profile data generation - empty
*      profile should be reserved. In this case, in order to save empty profiles number, it is
*      recommended to utilize profile 7 as well.
*      Empty profile should be configured to prevent insertion of template data into tunnel header:
*      templateDataSize = CPSS_DXCH_TUNNEL_START_TEMPLATE_DATA_SIZE_NONE_E.
*******************************************************************************/
GT_STATUS cpssDxChTunnelStartGenProfileTableEntrySet
(
    IN GT_U8                                            devNum,
    IN GT_U32                                           profileIndex,
    IN CPSS_DXCH_TUNNEL_START_GEN_IP_PROFILE_STC        *profileDataPtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChTunnelStartGenProfileTableEntrySet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, profileIndex, profileDataPtr));

    rc = internal_cpssDxChTunnelStartGenProfileTableEntrySet(devNum, profileIndex, profileDataPtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, profileIndex, profileDataPtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}



/*******************************************************************************
* internal_cpssDxChTunnelStartPortGroupGenProfileTableEntryGet
*
* DESCRIPTION:
*   This API retrieves  Generic Tunnel-start Profile table entry content.
*   Generic Tunnel-start Profile table entry serves as logical extension to
*   the Generic IPv4/6 Tunnel-Start when the IPv4/6 tunnel header includes
*   either a GRE or UDP header.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum           - device number.
*       portGroupsBmp    - bitmap of Port Groups.
*                          NOTEs:
*                          1. for non multi-port groups device this parameter is IGNORED.
*                          2. for multi-port groups device :
*                             bitmap must be set with at least one bit representing
*                             valid port group(s). If a bit of non valid port group
*                             is set then function returns GT_BAD_PARAM.
*                             value CPSS_PORT_GROUP_UNAWARE_MODE_CNS is supported.
*       profileIndex     - entry index in Generic Tunnel-start Profile table.
*                          (APPLICABLE RANGES: 0...7).
* OUTPUTS:
*       profileDataPtr   - pointer to Generic Tunnel-start Profile data.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_BAD_PTR               - null pointer
*       GT_OUT_OF_RANGE          - one of the parameters is out of range
*       GT_HW_ERROR              - on hardware error
*       GT_TIMEOUT  - after max number of retries checking if PP ready
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*      The Profile table entry template generates up to 16 bytes of data
*      after the 4B GRE header or after the 8B UDP header.
*      There are 8 profiles available to use. But if application is
*      going to use legacy ip tunnel types, cpss is reserved profile index 7 for legacy support
*      (empty profile: no extension data is added in tunnel header). Also if usage of new ipv4/6
*      tunnel types take place , and these tunnels don't require profile data generation - empty
*      profile should be reserved. In this case, in order to save empty profiles number, it is
*      recommended to utilize profile 7 as well.
*      Empty profile should be configured to prevent insertion of template data into tunnel header:
*      templateDataSize = CPSS_DXCH_TUNNEL_START_TEMPLATE_DATA_SIZE_NONE_E.
*******************************************************************************/
static GT_STATUS internal_cpssDxChTunnelStartPortGroupGenProfileTableEntryGet
(
    IN GT_U8                                            devNum,
    IN GT_PORT_GROUPS_BMP                               portGroupsBmp,
    IN GT_U32                                           profileIndex,
    OUT CPSS_DXCH_TUNNEL_START_GEN_IP_PROFILE_STC       *profileDataPtr
)
{
    GT_U32      hwConfigArray[TUNNEL_START_PROFILE_HW_SIZE_CNS] = {0}; /* tunnel start profile config in hardware format */
    GT_STATUS   rc;         /* function return code */
    GT_U32      portGroupId;/*the port group Id - support multi-port-groups device */


    /* check parameters */
    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E |
         CPSS_LION_E | CPSS_XCAT2_E | CPSS_LION2_E);

    PRV_CPSS_MULTI_PORT_GROUPS_BMP_CHECK_MAC(devNum,portGroupsBmp);
    PRV_TUNNEL_START_PROFILE_INDEX_CHECK_MAC(profileIndex);
    CPSS_NULL_PTR_CHECK_MAC(profileDataPtr);

    if(PRV_CPSS_IS_MULTI_PORT_GROUPS_DEVICE_MAC(devNum))
    {
        /* Get the first active port group */
        PRV_CPSS_MULTI_PORT_GROUPS_BMP_GET_FIRST_ACTIVE_MAC(devNum, portGroupsBmp,
                                                            portGroupId);
    }
    else
    {
        portGroupId = CPSS_PORT_GROUP_UNAWARE_MODE_CNS;
    }


    rc = prvCpssDxChPortGroupReadTableEntry(devNum,
                                            portGroupId,
                                            PRV_CPSS_DXCH_LION3_TABLE_HA_GENERIC_TS_PROFILE_E,
                                            profileIndex,
                                            hwConfigArray);
    if(rc != GT_OK)
    {
        return rc;
    }

    /* convert tunnel start entry to sw format */
    rc = tunnelStartProfileConfigHw2LogicFormat(devNum,hwConfigArray,profileDataPtr);
    return rc;
}

/*******************************************************************************
* cpssDxChTunnelStartPortGroupGenProfileTableEntryGet
*
* DESCRIPTION:
*   This API retrieves  Generic Tunnel-start Profile table entry content.
*   Generic Tunnel-start Profile table entry serves as logical extension to
*   the Generic IPv4/6 Tunnel-Start when the IPv4/6 tunnel header includes
*   either a GRE or UDP header.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum           - device number.
*       portGroupsBmp    - bitmap of Port Groups.
*                          NOTEs:
*                          1. for non multi-port groups device this parameter is IGNORED.
*                          2. for multi-port groups device :
*                             bitmap must be set with at least one bit representing
*                             valid port group(s). If a bit of non valid port group
*                             is set then function returns GT_BAD_PARAM.
*                             value CPSS_PORT_GROUP_UNAWARE_MODE_CNS is supported.
*       profileIndex     - entry index in Generic Tunnel-start Profile table.
*                          (APPLICABLE RANGES: 0...7).
* OUTPUTS:
*       profileDataPtr   - pointer to Generic Tunnel-start Profile data.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_BAD_PTR               - null pointer
*       GT_OUT_OF_RANGE          - one of the parameters is out of range
*       GT_HW_ERROR              - on hardware error
*       GT_TIMEOUT  - after max number of retries checking if PP ready
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*      The Profile table entry template generates up to 16 bytes of data
*      after the 4B GRE header or after the 8B UDP header.
*      There are 8 profiles available to use. But if application is
*      going to use legacy ip tunnel types, cpss is reserved profile index 7 for legacy support
*      (empty profile: no extension data is added in tunnel header). Also if usage of new ipv4/6
*      tunnel types take place , and these tunnels don't require profile data generation - empty
*      profile should be reserved. In this case, in order to save empty profiles number, it is
*      recommended to utilize profile 7 as well.
*      Empty profile should be configured to prevent insertion of template data into tunnel header:
*      templateDataSize = CPSS_DXCH_TUNNEL_START_TEMPLATE_DATA_SIZE_NONE_E.
*******************************************************************************/
GT_STATUS cpssDxChTunnelStartPortGroupGenProfileTableEntryGet
(
    IN GT_U8                                            devNum,
    IN GT_PORT_GROUPS_BMP                               portGroupsBmp,
    IN GT_U32                                           profileIndex,
    OUT CPSS_DXCH_TUNNEL_START_GEN_IP_PROFILE_STC       *profileDataPtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChTunnelStartPortGroupGenProfileTableEntryGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, portGroupsBmp, profileIndex, profileDataPtr));

    rc = internal_cpssDxChTunnelStartPortGroupGenProfileTableEntryGet(devNum, portGroupsBmp, profileIndex, profileDataPtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, portGroupsBmp, profileIndex, profileDataPtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}


/*******************************************************************************
* internal_cpssDxChTunnelStartGenProfileTableEntryGet
*
* DESCRIPTION:
*   This API retrieves  Generic Tunnel-start Profile table entry content.
*   It serves as logical extension to the Generic IPv4/6 Tunnel-Start
*   when the IPv4/6 tunnel header includes either a GRE or UDP header.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum           - device number
*       profileIndex     - entry index in Generic Tunnel-start Profile table.
*                         (APPLICABLE RANGES: 0...7).
*
* OUTPUTS:
*       profileDataPtr   - pointer to Generic Tunnel-start Profile data.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_BAD_PTR               - null pointer
*       GT_OUT_OF_RANGE          - one of the parameters is out of range
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*      The Profile table entry template generates up to 16 bytes of data
*      after the 4B GRE header or after the 8B UDP header.
*      There are 8 profiles available to use. But if application is
*      going to use legacy ip tunnel types, cpss is reserved profile index 7 for legacy support
*      (empty profile: no extension data is added in tunnel header). Also if usage of new ipv4/6
*      tunnel types take place , and these tunnels don't require profile data generation - empty
*      profile should be reserved. In this case, in order to save empty profiles number, it is
*      recommended to utilize profile 7 as well.
*      Empty profile should be configured to prevent insertion of template data into tunnel header:
*      templateDataSize = CPSS_DXCH_TUNNEL_START_TEMPLATE_DATA_SIZE_NONE_E.
*******************************************************************************/
static GT_STATUS internal_cpssDxChTunnelStartGenProfileTableEntryGet
(
    IN GT_U8                                            devNum,
    IN GT_U32                                           profileIndex,
    OUT CPSS_DXCH_TUNNEL_START_GEN_IP_PROFILE_STC       *profileDataPtr
)
{
    return cpssDxChTunnelStartPortGroupGenProfileTableEntryGet(devNum,
                                                               CPSS_PORT_GROUP_UNAWARE_MODE_CNS,
                                                               profileIndex,
                                                               profileDataPtr);
}

/*******************************************************************************
* cpssDxChTunnelStartGenProfileTableEntryGet
*
* DESCRIPTION:
*   This API retrieves  Generic Tunnel-start Profile table entry content.
*   It serves as logical extension to the Generic IPv4/6 Tunnel-Start
*   when the IPv4/6 tunnel header includes either a GRE or UDP header.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum           - device number
*       profileIndex     - entry index in Generic Tunnel-start Profile table.
*                         (APPLICABLE RANGES: 0...7).
*
* OUTPUTS:
*       profileDataPtr   - pointer to Generic Tunnel-start Profile data.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_BAD_PTR               - null pointer
*       GT_OUT_OF_RANGE          - one of the parameters is out of range
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*      The Profile table entry template generates up to 16 bytes of data
*      after the 4B GRE header or after the 8B UDP header.
*      There are 8 profiles available to use. But if application is
*      going to use legacy ip tunnel types, cpss is reserved profile index 7 for legacy support
*      (empty profile: no extension data is added in tunnel header). Also if usage of new ipv4/6
*      tunnel types take place , and these tunnels don't require profile data generation - empty
*      profile should be reserved. In this case, in order to save empty profiles number, it is
*      recommended to utilize profile 7 as well.
*      Empty profile should be configured to prevent insertion of template data into tunnel header:
*      templateDataSize = CPSS_DXCH_TUNNEL_START_TEMPLATE_DATA_SIZE_NONE_E.
*******************************************************************************/
GT_STATUS cpssDxChTunnelStartGenProfileTableEntryGet
(
    IN GT_U8                                            devNum,
    IN GT_U32                                           profileIndex,
    OUT CPSS_DXCH_TUNNEL_START_GEN_IP_PROFILE_STC       *profileDataPtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChTunnelStartGenProfileTableEntryGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, profileIndex, profileDataPtr));

    rc = internal_cpssDxChTunnelStartGenProfileTableEntryGet(devNum, profileIndex, profileDataPtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, profileIndex, profileDataPtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}
/*******************************************************************************
* internal_cpssDxChTunnelStartEntryGet
*
* DESCRIPTION:
*       Get a tunnel start entry.
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond.
*
* INPUTS:
*       devNum        - physical device number
*       routerArpTunnelStartLineIndex - line index for the tunnel start entry
*                       in the router ARP / tunnel start / NAT table (for NAT capable devices)
*
* OUTPUTS:
*       tunnelTypePtr - points to the type of the tunnel, valid values:
*                       CPSS_TUNNEL_X_OVER_IPV4_E,
*                       CPSS_TUNNEL_X_OVER_GRE_IPV4_E,
*                       CPSS_TUNNEL_X_OVER_MPLS_E
*                       CPSS_TUNNEL_MAC_IN_MAC_E (APPLICABLE DEVICES: xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3)
*                       CPSS_TUNNEL_TRILL_E (APPLICABLE DEVICES: Bobcat2; Caelum; Bobcat3)
*       configPtr     - points to tunnel start configuration
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - on bad parameter.
*       GT_BAD_PTR               - on NULL pointer
*       GT_FAIL                  - on failure.
*       GT_OUT_OF_RANGE          - parameter not in valid range.
*       GT_BAD_STATE             - on invalid tunnel type
*       GT_TIMEOUT  - after max number of retries checking if PP ready
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       NAT entries table (for NAT capable devices), tunnel start entries table
*       and router ARP addresses table reside at the same physical memory.
*       Each line can hold:
*       - 1 NAT entry(for NAT capable devices)
*       - 1 tunnel start entry
*       - 4 router ARP addresses entries
*       NAT entry / Tunnel start entry at index n and router ARP addresses at indexes
*       4*n..4*n+3 share the same memory. For example NAT entry/tunnel start entry at
*       index 100 and router ARP addresses at indexes 400..403 share the same
*       physical memory.
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChTunnelStartEntryGet
(
    IN   GT_U8                              devNum,
    IN   GT_U32                             routerArpTunnelStartLineIndex,
    OUT  CPSS_TUNNEL_TYPE_ENT               *tunnelTypePtr,
    OUT  CPSS_DXCH_TUNNEL_START_CONFIG_UNT  *configPtr
)
{
    return cpssDxChTunnelStartPortGroupEntryGet(devNum,
                CPSS_PORT_GROUP_UNAWARE_MODE_CNS,
                routerArpTunnelStartLineIndex,
                tunnelTypePtr,
                configPtr);
}

/*******************************************************************************
* cpssDxChTunnelStartEntryGet
*
* DESCRIPTION:
*       Get a tunnel start entry.
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond.
*
* INPUTS:
*       devNum        - physical device number
*       routerArpTunnelStartLineIndex - line index for the tunnel start entry
*                       in the router ARP / tunnel start / NAT table (for NAT capable devices)
*
* OUTPUTS:
*       tunnelTypePtr - points to the type of the tunnel, valid values:
*                       CPSS_TUNNEL_X_OVER_IPV4_E,
*                       CPSS_TUNNEL_X_OVER_GRE_IPV4_E,
*                       CPSS_TUNNEL_X_OVER_MPLS_E
*                       CPSS_TUNNEL_MAC_IN_MAC_E (APPLICABLE DEVICES: xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3)
*                       CPSS_TUNNEL_TRILL_E (APPLICABLE DEVICES: Bobcat2; Caelum; Bobcat3)
*       configPtr     - points to tunnel start configuration
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - on bad parameter.
*       GT_BAD_PTR               - on NULL pointer
*       GT_FAIL                  - on failure.
*       GT_OUT_OF_RANGE          - parameter not in valid range.
*       GT_BAD_STATE             - on invalid tunnel type
*       GT_TIMEOUT  - after max number of retries checking if PP ready
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       NAT entries table (for NAT capable devices), tunnel start entries table
*       and router ARP addresses table reside at the same physical memory.
*       Each line can hold:
*       - 1 NAT entry(for NAT capable devices)
*       - 1 tunnel start entry
*       - 4 router ARP addresses entries
*       NAT entry / Tunnel start entry at index n and router ARP addresses at indexes
*       4*n..4*n+3 share the same memory. For example NAT entry/tunnel start entry at
*       index 100 and router ARP addresses at indexes 400..403 share the same
*       physical memory.
*
*******************************************************************************/
GT_STATUS cpssDxChTunnelStartEntryGet
(
    IN   GT_U8                              devNum,
    IN   GT_U32                             routerArpTunnelStartLineIndex,
    OUT  CPSS_TUNNEL_TYPE_ENT               *tunnelTypePtr,
    OUT  CPSS_DXCH_TUNNEL_START_CONFIG_UNT  *configPtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChTunnelStartEntryGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, routerArpTunnelStartLineIndex, tunnelTypePtr, configPtr));

    rc = internal_cpssDxChTunnelStartEntryGet(devNum, routerArpTunnelStartLineIndex, tunnelTypePtr, configPtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, routerArpTunnelStartLineIndex, tunnelTypePtr, configPtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChTunnelStartPortGroupEntrySet
*
* DESCRIPTION:
*       Set a tunnel start entry.
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond.
*
* INPUTS:
*       devNum        - physical device number
*       portGroupsBmp - bitmap of Port Groups.
*                      NOTEs:
*                       1. for non multi-port groups device this parameter is IGNORED.
*                       2. for multi-port groups device :
*                          bitmap must be set with at least one bit representing
*                          valid port group(s). If a bit of non valid port group
*                          is set then function returns GT_BAD_PARAM.
*                          value CPSS_PORT_GROUP_UNAWARE_MODE_CNS is supported.
*       routerArpTunnelStartLineIndex - line index for the tunnel start entry
*                       in the router ARP / tunnel start / NAT table (for NAT capable devices)
*       tunnelType    - type of the tunnel; valid values:
*                       CPSS_TUNNEL_X_OVER_IPV4_E
*                       CPSS_TUNNEL_X_OVER_GRE_IPV4_E
*                       CPSS_TUNNEL_X_OVER_MPLS_E
*                       CPSS_TUNNEL_MAC_IN_MAC_E (APPLICABLE DEVICES: xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3)
*                       CPSS_TUNNEL_TRILL_E (APPLICABLE DEVICES: Bobcat2; Caelum; Bobcat3)
*       configPtr     - points to tunnel start configuration
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - on bad parameter.
*       GT_BAD_PTR               - on NULL pointer
*       GT_FAIL                  - on failure.
*       GT_OUT_OF_RANGE          - parameter not in valid range.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       NAT entries table (for NAT capable devices), tunnel start entries table
*       and router ARP addresses table reside at the same physical memory.
*       Each line can hold:
*       - 1 NAT entry(for NAT capable devices)
*       - 1 tunnel start entry
*       - 4 router ARP addresses entries
*       NAT entry / Tunnel start entry at index n and router ARP addresses at indexes
*       4*n..4*n+3 share the same memory. For example NAT entry/tunnel start entry at
*       index 100 and router ARP addresses at indexes 400..403 share the same
*       physical memory.
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChTunnelStartPortGroupEntrySet
(
    IN  GT_U8                               devNum,
    IN  GT_PORT_GROUPS_BMP                  portGroupsBmp,
    IN  GT_U32                              routerArpTunnelStartLineIndex,
    IN  CPSS_TUNNEL_TYPE_ENT                tunnelType,
    IN  CPSS_DXCH_TUNNEL_START_CONFIG_UNT   *configPtr
)
{
    GT_U32      hwConfigArray[TUNNEL_START_HW_SIZE_CNS]; /* tunnel start config in hardware format */
    GT_STATUS   rc;         /* function return code */
    GT_U32      portGroupId;/*the port group Id - support multi-port-groups device */
    PRV_CPSS_DXCH_HA_TS_ARP_NAT_ENTRY_TYPE_ENT entryType;
    /* check parameters */
    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum, CPSS_CH1_E | CPSS_CH1_DIAMOND_E);

    PRV_CPSS_MULTI_PORT_GROUPS_BMP_CHECK_MAC(devNum,portGroupsBmp);
    LINE_INDEX_CHECK_MAC(devNum,routerArpTunnelStartLineIndex);
    CPSS_NULL_PTR_CHECK_MAC(configPtr);

    /* convert tunnel start entry to hw format while checking parameters */
    rc = tunnelStartConfigLogic2HwFormat(devNum,tunnelType,configPtr,hwConfigArray);
    if (rc != GT_OK)
    {
        return rc;
    }

    if (( PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_ENABLED_MAC(devNum)==GT_TRUE ) &&
        (tunnelType == CPSS_TUNNEL_GENERIC_IPV6_E))
    {
        entryType = PRV_CPSS_DXCH_HA_TS_ARP_NAT_ENTRY_TYPE_TUNNEL_START_DOUBLE_E;
    }
    else
    {
        entryType = PRV_CPSS_DXCH_HA_TS_ARP_NAT_ENTRY_TYPE_TUNNEL_START_REGULAR_E;
    }

    PRV_CPSS_GEN_PP_START_LOOP_PORT_GROUPS_IN_BMP_MAC(devNum,portGroupsBmp,portGroupId)
    {
    rc = prvCpssDxChHwHaTunnelStartArpNatTableEntryWrite(devNum,
                                                 portGroupId,
                                                 entryType,
                                                 routerArpTunnelStartLineIndex,
                                                 hwConfigArray);
        if(rc != GT_OK)
        {
            return rc;
        }
    }
    PRV_CPSS_GEN_PP_END_LOOP_PORT_GROUPS_IN_BMP_MAC(devNum,portGroupsBmp,portGroupId)

    return rc;
}

/*******************************************************************************
* cpssDxChTunnelStartPortGroupEntrySet
*
* DESCRIPTION:
*       Set a tunnel start entry.
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond.
*
* INPUTS:
*       devNum        - physical device number
*       portGroupsBmp - bitmap of Port Groups.
*                      NOTEs:
*                       1. for non multi-port groups device this parameter is IGNORED.
*                       2. for multi-port groups device :
*                          bitmap must be set with at least one bit representing
*                          valid port group(s). If a bit of non valid port group
*                          is set then function returns GT_BAD_PARAM.
*                          value CPSS_PORT_GROUP_UNAWARE_MODE_CNS is supported.
*       routerArpTunnelStartLineIndex - line index for the tunnel start entry
*                       in the router ARP / tunnel start / NAT table (for NAT capable devices)
*       tunnelType    - type of the tunnel; valid values:
*                       CPSS_TUNNEL_X_OVER_IPV4_E
*                       CPSS_TUNNEL_X_OVER_GRE_IPV4_E
*                       CPSS_TUNNEL_X_OVER_MPLS_E
*                       CPSS_TUNNEL_MAC_IN_MAC_E (APPLICABLE DEVICES: xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3)
*                       CPSS_TUNNEL_TRILL_E (APPLICABLE DEVICES: Bobcat2; Caelum; Bobcat3)
*       configPtr     - points to tunnel start configuration
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - on bad parameter.
*       GT_BAD_PTR               - on NULL pointer
*       GT_FAIL                  - on failure.
*       GT_OUT_OF_RANGE          - parameter not in valid range.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       NAT entries table (for NAT capable devices), tunnel start entries table
*       and router ARP addresses table reside at the same physical memory.
*       Each line can hold:
*       - 1 NAT entry(for NAT capable devices)
*       - 1 tunnel start entry
*       - 4 router ARP addresses entries
*       NAT entry / Tunnel start entry at index n and router ARP addresses at indexes
*       4*n..4*n+3 share the same memory. For example NAT entry/tunnel start entry at
*       index 100 and router ARP addresses at indexes 400..403 share the same
*       physical memory.
*
*******************************************************************************/
GT_STATUS cpssDxChTunnelStartPortGroupEntrySet
(
    IN  GT_U8                               devNum,
    IN  GT_PORT_GROUPS_BMP                  portGroupsBmp,
    IN  GT_U32                              routerArpTunnelStartLineIndex,
    IN  CPSS_TUNNEL_TYPE_ENT                tunnelType,
    IN  CPSS_DXCH_TUNNEL_START_CONFIG_UNT   *configPtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChTunnelStartPortGroupEntrySet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, portGroupsBmp, routerArpTunnelStartLineIndex, tunnelType, configPtr));

    rc = internal_cpssDxChTunnelStartPortGroupEntrySet(devNum, portGroupsBmp, routerArpTunnelStartLineIndex, tunnelType, configPtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, portGroupsBmp, routerArpTunnelStartLineIndex, tunnelType, configPtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChTunnelStartPortGroupEntryGet
*
* DESCRIPTION:
*       Get a tunnel start entry.
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond.
*
* INPUTS:
*       devNum        - physical device number
*       portGroupsBmp - bitmap of Port Groups.
*                      NOTEs:
*                       1. for non multi-port groups device this parameter is IGNORED.
*                       2. for multi-port groups device :
*                          bitmap must be set with at least one bit representing
*                          valid port group(s). If a bit of non valid port group
*                          is set then function returns GT_BAD_PARAM.
*                          value CPSS_PORT_GROUP_UNAWARE_MODE_CNS is supported.
*                          - read only from first active port group of the bitmap.
*       routerArpTunnelStartLineIndex - line index for the tunnel start entry
*                       in the router ARP / tunnel start / NAT table (for NAT capable devices)
*
* OUTPUTS:
*       tunnelTypePtr - points to the type of the tunnel, valid values:
*                       CPSS_TUNNEL_X_OVER_IPV4_E,
*                       CPSS_TUNNEL_X_OVER_GRE_IPV4_E,
*                       CPSS_TUNNEL_X_OVER_MPLS_E
*                       CPSS_TUNNEL_MAC_IN_MAC_E (APPLICABLE DEVICES: xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3)
*                       CPSS_TUNNEL_TRILL_E (APPLICABLE DEVICES: Bobcat2; Caelum; Bobcat3)
*       configPtr     - points to tunnel start configuration
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - on bad parameter.
*       GT_BAD_PTR               - on NULL pointer
*       GT_FAIL                  - on failure.
*       GT_OUT_OF_RANGE          - parameter not in valid range.
*       GT_BAD_STATE             - on invalid tunnel type
*       GT_TIMEOUT  - after max number of retries checking if PP ready
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       NAT entries table (for NAT capable devices), tunnel start entries table
*       and router ARP addresses table reside at the same physical memory.
*       Each line can hold:
*       - 1 NAT entry(for NAT capable devices)
*       - 1 tunnel start entry
*       - 4 router ARP addresses entries
*       NAT entry / Tunnel start entry at index n and router ARP addresses at indexes
*       4*n..4*n+3 share the same memory. For example NAT entry/tunnel start entry at
*       index 100 and router ARP addresses at indexes 400..403 share the same
*       physical memory.
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChTunnelStartPortGroupEntryGet
(
    IN   GT_U8                              devNum,
    IN   GT_PORT_GROUPS_BMP                 portGroupsBmp,
    IN   GT_U32                             routerArpTunnelStartLineIndex,
    OUT  CPSS_TUNNEL_TYPE_ENT               *tunnelTypePtr,
    OUT  CPSS_DXCH_TUNNEL_START_CONFIG_UNT  *configPtr
)
{
    GT_U32      hwConfigArray[TUNNEL_START_HW_SIZE_CNS]; /* tunnel start config in hardware format */
    GT_STATUS   rc;     /* function return code */
    GT_U32      portGroupId;/*the port group Id - support multi-port-groups device */
    PRV_CPSS_DXCH_HA_TS_ARP_NAT_ENTRY_TYPE_ENT entryType =PRV_CPSS_DXCH_HA_TS_ARP_NAT_ENTRY_TYPE_TUNNEL_START_REGULAR_E;

    /* check parameters */
    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum, CPSS_CH1_E | CPSS_CH1_DIAMOND_E);

    PRV_CPSS_MULTI_PORT_GROUPS_BMP_CHECK_MAC(devNum,portGroupsBmp);
    LINE_INDEX_CHECK_MAC(devNum,routerArpTunnelStartLineIndex);
    CPSS_NULL_PTR_CHECK_MAC(tunnelTypePtr);
    CPSS_NULL_PTR_CHECK_MAC(configPtr);

    if(PRV_CPSS_IS_MULTI_PORT_GROUPS_DEVICE_MAC(devNum))
    {
        /* Get the first active port group */
        PRV_CPSS_MULTI_PORT_GROUPS_BMP_GET_FIRST_ACTIVE_MAC(devNum, portGroupsBmp,
                                                            portGroupId);
    }
    else
    {
        portGroupId = CPSS_PORT_GROUP_UNAWARE_MODE_CNS;
    }

    if ( PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_ENABLED_MAC(devNum)==GT_TRUE )
    {
        if (!(routerArpTunnelStartLineIndex & 0x1))
        {
            entryType = PRV_CPSS_DXCH_HA_TS_ARP_NAT_ENTRY_TYPE_TUNNEL_START_DOUBLE_E;
        }
    }

    /* read tunnel start entry from the hardware */
    rc = prvCpssDxChHwHaTunnelStartArpNatTableEntryRead(devNum,
                                                        portGroupId,
                                                        entryType,
                                                        routerArpTunnelStartLineIndex,
                                                        hwConfigArray);
    if (rc != GT_OK)
        return rc;

    /* convert tunnel entry from hardware format to logic format */
    rc = tunnelStartConfigHw2LogicFormat(devNum,hwConfigArray,tunnelTypePtr,configPtr);

    return rc;
}

/*******************************************************************************
* cpssDxChTunnelStartPortGroupEntryGet
*
* DESCRIPTION:
*       Get a tunnel start entry.
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond.
*
* INPUTS:
*       devNum        - physical device number
*       portGroupsBmp - bitmap of Port Groups.
*                      NOTEs:
*                       1. for non multi-port groups device this parameter is IGNORED.
*                       2. for multi-port groups device :
*                          bitmap must be set with at least one bit representing
*                          valid port group(s). If a bit of non valid port group
*                          is set then function returns GT_BAD_PARAM.
*                          value CPSS_PORT_GROUP_UNAWARE_MODE_CNS is supported.
*                          - read only from first active port group of the bitmap.
*       routerArpTunnelStartLineIndex - line index for the tunnel start entry
*                       in the router ARP / tunnel start / NAT table (for NAT capable devices)
*
* OUTPUTS:
*       tunnelTypePtr - points to the type of the tunnel, valid values:
*                       CPSS_TUNNEL_X_OVER_IPV4_E,
*                       CPSS_TUNNEL_X_OVER_GRE_IPV4_E,
*                       CPSS_TUNNEL_X_OVER_MPLS_E
*                       CPSS_TUNNEL_MAC_IN_MAC_E (APPLICABLE DEVICES: xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3)
*                       CPSS_TUNNEL_TRILL_E (APPLICABLE DEVICES: Bobcat2; Caelum; Bobcat3)
*       configPtr     - points to tunnel start configuration
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - on bad parameter.
*       GT_BAD_PTR               - on NULL pointer
*       GT_FAIL                  - on failure.
*       GT_OUT_OF_RANGE          - parameter not in valid range.
*       GT_BAD_STATE             - on invalid tunnel type
*       GT_TIMEOUT  - after max number of retries checking if PP ready
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       NAT entries table (for NAT capable devices), tunnel start entries table
*       and router ARP addresses table reside at the same physical memory.
*       Each line can hold:
*       - 1 NAT entry(for NAT capable devices)
*       - 1 tunnel start entry
*       - 4 router ARP addresses entries
*       NAT entry / Tunnel start entry at index n and router ARP addresses at indexes
*       4*n..4*n+3 share the same memory. For example NAT entry/tunnel start entry at
*       index 100 and router ARP addresses at indexes 400..403 share the same
*       physical memory.
*
*******************************************************************************/
GT_STATUS cpssDxChTunnelStartPortGroupEntryGet
(
    IN   GT_U8                              devNum,
    IN   GT_PORT_GROUPS_BMP                 portGroupsBmp,
    IN   GT_U32                             routerArpTunnelStartLineIndex,
    OUT  CPSS_TUNNEL_TYPE_ENT               *tunnelTypePtr,
    OUT  CPSS_DXCH_TUNNEL_START_CONFIG_UNT  *configPtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChTunnelStartPortGroupEntryGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, portGroupsBmp, routerArpTunnelStartLineIndex, tunnelTypePtr, configPtr));

    rc = internal_cpssDxChTunnelStartPortGroupEntryGet(devNum, portGroupsBmp, routerArpTunnelStartLineIndex, tunnelTypePtr, configPtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, portGroupsBmp, routerArpTunnelStartLineIndex, tunnelTypePtr, configPtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChIpv4TunnelTermPortSet
*
* DESCRIPTION:
*       Set port enable/disable state for ipv4 tunnel termination.
*
* APPLICABLE DEVICES:
*        DxCh2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum    - physical device number
*       port      - port to enable/disable ipv4 tunnel termination
*       enable    - enable/disable
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - on bad parameter.
*       GT_FAIL                  - on error.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChIpv4TunnelTermPortSet
(
    IN  GT_U8       devNum,
    IN  GT_PHYSICAL_PORT_NUM  port,
    IN  GT_BOOL     enable
)
{
    GT_U32      value;      /* value to write */
    GT_STATUS   rc;         /* function return code */

    /* check parameters */
    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);

    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E | CPSS_LION_E | CPSS_XCAT2_E | CPSS_LION2_E | CPSS_BOBCAT2_E | CPSS_CAELUM_E | CPSS_BOBCAT3_E);

    PRV_CPSS_DXCH_PHY_PORT_CHECK_MAC(devNum,port);

    /* prepare value to write */
    value = (enable == GT_TRUE) ? 1 : 0;

    /* write value to hardware table */
    rc = prvCpssDxChWriteTableEntryField(devNum,
                                         PRV_CPSS_DXCH_TABLE_PORT_VLAN_QOS_E,
                                         port,
                                         1,     /* word 1 */
                                         17,    /* offset */
                                         1,     /* length */
                                         value);

    return rc;
}

/*******************************************************************************
* cpssDxChIpv4TunnelTermPortSet
*
* DESCRIPTION:
*       Set port enable/disable state for ipv4 tunnel termination.
*
* APPLICABLE DEVICES:
*        DxCh2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum    - physical device number
*       port      - port to enable/disable ipv4 tunnel termination
*       enable    - enable/disable
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - on bad parameter.
*       GT_FAIL                  - on error.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChIpv4TunnelTermPortSet
(
    IN  GT_U8       devNum,
    IN  GT_PHYSICAL_PORT_NUM  port,
    IN  GT_BOOL     enable
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChIpv4TunnelTermPortSet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, port, enable));

    rc = internal_cpssDxChIpv4TunnelTermPortSet(devNum, port, enable);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, port, enable));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}


/*******************************************************************************
* internal_cpssDxChIpv4TunnelTermPortGet
*
* DESCRIPTION:
*       Get ipv4 tunnel termination port enable/disable state.
*
* APPLICABLE DEVICES:
*        DxCh2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum        - physical device number
*       port          - port to get ipv4 tunnel termination state
*
* OUTPUTS:
*       enablePtr     - points to ipv4 tunnel termination state
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PTR               - on NULL pointer
*       GT_BAD_PARAM             - on bad parameter.
*       GT_FAIL                  - on error.
*       GT_TIMEOUT  - after max number of retries checking if PP ready
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChIpv4TunnelTermPortGet
(
    IN  GT_U8       devNum,
    IN  GT_PHYSICAL_PORT_NUM  port,
    OUT GT_BOOL     *enablePtr
)
{
    GT_U32      value;      /* hardware value */
    GT_STATUS   rc;         /* function return code */

    /* check parameters */
    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);

    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E | CPSS_LION_E | CPSS_XCAT2_E | CPSS_LION2_E | CPSS_BOBCAT2_E | CPSS_CAELUM_E | CPSS_BOBCAT3_E);

    PRV_CPSS_DXCH_PHY_PORT_CHECK_MAC(devNum,port);
    CPSS_NULL_PTR_CHECK_MAC(enablePtr);

    /* read hardware value */
    rc = prvCpssDxChReadTableEntryField(devNum,
                                        PRV_CPSS_DXCH_TABLE_PORT_VLAN_QOS_E,
                                        port,
                                        1,     /* word 1 */
                                        17,    /* offset */
                                        1,     /* length */
                                        &value);
    if (rc != GT_OK)
        return rc;

    /* decode enable/disable state from hardware value */
    *enablePtr = (value != 0) ? GT_TRUE : GT_FALSE;

    return GT_OK;
}

/*******************************************************************************
* cpssDxChIpv4TunnelTermPortGet
*
* DESCRIPTION:
*       Get ipv4 tunnel termination port enable/disable state.
*
* APPLICABLE DEVICES:
*        DxCh2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum        - physical device number
*       port          - port to get ipv4 tunnel termination state
*
* OUTPUTS:
*       enablePtr     - points to ipv4 tunnel termination state
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PTR               - on NULL pointer
*       GT_BAD_PARAM             - on bad parameter.
*       GT_FAIL                  - on error.
*       GT_TIMEOUT  - after max number of retries checking if PP ready
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChIpv4TunnelTermPortGet
(
    IN  GT_U8       devNum,
    IN  GT_PHYSICAL_PORT_NUM  port,
    OUT GT_BOOL     *enablePtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChIpv4TunnelTermPortGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, port, enablePtr));

    rc = internal_cpssDxChIpv4TunnelTermPortGet(devNum, port, enablePtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, port, enablePtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}


/*******************************************************************************
* internal_cpssDxChMplsTunnelTermPortSet
*
* DESCRIPTION:
*       Set port enable/disable state for mpls tunnel termination.
*
* APPLICABLE DEVICES:
*        DxCh2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum    - physical device number
*       port      - port to enable/disable mpls tunnel termination
*       enable    - enable/disable
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - on bad parameter.
*       GT_FAIL                  - on error.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChMplsTunnelTermPortSet
(
    IN  GT_U8       devNum,
    IN  GT_PHYSICAL_PORT_NUM  port,
    IN  GT_BOOL     enable
)
{
    GT_U32      value;      /* value to write */
    GT_STATUS   rc;         /* function return code */

    /* check parameters */
    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);

    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E | CPSS_LION_E | CPSS_XCAT2_E | CPSS_LION2_E | CPSS_BOBCAT2_E | CPSS_CAELUM_E | CPSS_BOBCAT3_E);

    PRV_CPSS_DXCH_PHY_PORT_CHECK_MAC(devNum,port);

    /* prepare value to write */
    value = (enable == GT_TRUE) ? 1 : 0;

    /* write value to hardware table */
    rc = prvCpssDxChWriteTableEntryField(devNum,
                                         PRV_CPSS_DXCH_TABLE_PORT_VLAN_QOS_E,
                                         port,
                                         1,     /* word 1 */
                                         18,    /* offset */
                                         1,     /* length */
                                         value);

    return rc;
}

/*******************************************************************************
* cpssDxChMplsTunnelTermPortSet
*
* DESCRIPTION:
*       Set port enable/disable state for mpls tunnel termination.
*
* APPLICABLE DEVICES:
*        DxCh2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum    - physical device number
*       port      - port to enable/disable mpls tunnel termination
*       enable    - enable/disable
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - on bad parameter.
*       GT_FAIL                  - on error.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChMplsTunnelTermPortSet
(
    IN  GT_U8       devNum,
    IN  GT_PHYSICAL_PORT_NUM  port,
    IN  GT_BOOL     enable
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChMplsTunnelTermPortSet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, port, enable));

    rc = internal_cpssDxChMplsTunnelTermPortSet(devNum, port, enable);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, port, enable));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}


/*******************************************************************************
* internal_cpssDxChMplsTunnelTermPortGet
*
* DESCRIPTION:
*       Get mpls tunnel termination port enable/disable state.
*
* APPLICABLE DEVICES:
*        DxCh2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum        - physical device number
*       port          - port to get mpls tunnel termination state
*
* OUTPUTS:
*       enablePtr     - points to mpls tunnel termination state
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PTR               - on NULL pointer
*       GT_BAD_PARAM             - on bad parameter.
*       GT_FAIL                  - on error.
*       GT_TIMEOUT  - after max number of retries checking if PP ready
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChMplsTunnelTermPortGet
(
    IN  GT_U8       devNum,
    IN  GT_PHYSICAL_PORT_NUM port,
    OUT GT_BOOL     *enablePtr
)
{
    GT_U32      value;      /* hardware value */
    GT_STATUS   rc;         /* function return code */

    /* check parameters */
    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);

    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E | CPSS_LION_E | CPSS_XCAT2_E | CPSS_LION2_E | CPSS_BOBCAT2_E | CPSS_CAELUM_E | CPSS_BOBCAT3_E);

    PRV_CPSS_DXCH_PHY_PORT_CHECK_MAC(devNum,port);
    CPSS_NULL_PTR_CHECK_MAC(enablePtr);

    /* read hardware value */
    rc = prvCpssDxChReadTableEntryField(devNum,
                                        PRV_CPSS_DXCH_TABLE_PORT_VLAN_QOS_E,
                                        port,
                                        1,     /* word 1 */
                                        18,    /* offset */
                                        1,     /* length */
                                        &value);
    if (rc != GT_OK)
        return rc;

    /* decode enable/disable state from hardware value */
    *enablePtr = (value != 0) ? GT_TRUE : GT_FALSE;

    return GT_OK;
}

/*******************************************************************************
* cpssDxChMplsTunnelTermPortGet
*
* DESCRIPTION:
*       Get mpls tunnel termination port enable/disable state.
*
* APPLICABLE DEVICES:
*        DxCh2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum        - physical device number
*       port          - port to get mpls tunnel termination state
*
* OUTPUTS:
*       enablePtr     - points to mpls tunnel termination state
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PTR               - on NULL pointer
*       GT_BAD_PARAM             - on bad parameter.
*       GT_FAIL                  - on error.
*       GT_TIMEOUT  - after max number of retries checking if PP ready
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChMplsTunnelTermPortGet
(
    IN  GT_U8       devNum,
    IN  GT_PHYSICAL_PORT_NUM port,
    OUT GT_BOOL     *enablePtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChMplsTunnelTermPortGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, port, enablePtr));

    rc = internal_cpssDxChMplsTunnelTermPortGet(devNum, port, enablePtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, port, enablePtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}


/*******************************************************************************
* internal_cpssDxChTunnelTermEntrySet
*
* DESCRIPTION:
*       Set a tunnel termination entry.
*
* APPLICABLE DEVICES:
*        DxCh2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum        - physical device number
*       routerTunnelTermTcamIndex  - index for the tunnel termination entry in
*                       the router / tunnel termination TCAM
*                       (APPLICABLE RANGES: 0...1023)
*       tunnelType    - type of the tunnel; valid options:
*                       CPSS_TUNNEL_IPV4_OVER_IPV4_E
*                       CPSS_TUNNEL_IPV4_OVER_GRE_IPV4_E
*                       CPSS_TUNNEL_IPV6_OVER_IPV4_E
*                       CPSS_TUNNEL_IPV6_OVER_GRE_IPV4_E
*                       CPSS_TUNNEL_IP_OVER_MPLS_E
*                       CPSS_TUNNEL_ETHERNET_OVER_MPLS_E
*       configPtr     - points to tunnel termination configuration
*       configMaskPtr - points to tunnel termination configuration mask. The
*                       mask is AND styled one. Mask bit's 0 means don't care
*                       bit (corresponding bit in the pattern is not used in
*                       the TCAM lookup). Mask bit's 1 means that corresponding
*                       bit in the pattern is used in the TCAM lookup.
*       actionPtr     - points to tunnel termination action
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - on bad parameter.
*       GT_BAD_PTR               - on NULL pointer
*       GT_FAIL                  - on failure.
*       GT_OUT_OF_RANGE          - parameter not in valid range.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       Tunnel termination entries and IP lookup entries both reside in
*       router / tunnel termination TCAM. The router / tunnel termination TCAM
*       contains 1K lines. Each line can hold:
*       - 1 tunnel termination entry
*       - 1 ipv6 addresses
*       - 5 ipv4 addresses
*       Indexes for entries that takes one full line (meaning tunnel termination
*       and ipv6 lookup address) range (0..1023); Indexes for other entires
*       range (0..5119); Those indexes are counted COLUMN BY COLUMN  meaning
*       indexes 0..1023 reside in the first column, indexes 1024..2047 reside
*       in the second column and so on.
*       Therefore, tunnel termination entry at index n share the same TCAM line
*       with ipv6 lookup address at index n and share the same TCAM line with
*       ipv4 lookup addresses at index n, 1024+n, 2048+n, 3072+n and 4096+n.
*       For example, tunnel termination entry at TCAM line 100 share the
*       same TCAM line with ipv6 lookup address at line 100 and also share the
*       same TCAM line with ipv4 lookup addresses at indexes 100, 1124, 2148,
*       3172 and 4196.
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChTunnelTermEntrySet
(
    IN  GT_U8                               devNum,
    IN  GT_U32                              routerTunnelTermTcamIndex,
    IN  CPSS_TUNNEL_TYPE_ENT                tunnelType,
    IN  CPSS_DXCH_TUNNEL_TERM_CONFIG_UNT    *configPtr,
    IN  CPSS_DXCH_TUNNEL_TERM_CONFIG_UNT    *configMaskPtr,
    IN  CPSS_DXCH_TUNNEL_TERM_ACTION_STC    *actionPtr
)
{
    GT_STATUS   rc;                 /* function return code */
    GT_U32      hwTcamKeyArray[TUNNEL_TERMINATION_TCAM_RULE_SIZE_CNS];  /* TCAM key in hw format */
    GT_U32      hwTcamMaskArray[TUNNEL_TERMINATION_TCAM_RULE_SIZE_CNS]; /* TCAM mask in hw format */
    GT_U32      hwTcamActionArray[TUNNEL_TERMINATION_TCAM_ACTION_SIZE_CNS]; /* TCAM action (rule) in hw format */
    CPSS_TUNNEL_TYPE_ENT    configTunnelType;   /* tunnel type used to convert */
                                                /* tunnel config to hw format  */
    CPSS_TUNNEL_TYPE_ENT    actionTunnelType;   /* tunnel type used to convert */
                                                /* tunnel action to hw format  */

    /* check parameters */
    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);

    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E | CPSS_LION_E | CPSS_XCAT2_E | CPSS_LION2_E | CPSS_BOBCAT2_E | CPSS_CAELUM_E | CPSS_BOBCAT3_E);

    PRV_CPSS_DXCH_ROUTER_TUNNEL_TERM_TCAM_CHECK_LINE_INDEX_MAC(devNum,routerTunnelTermTcamIndex);
    CPSS_NULL_PTR_CHECK_MAC(configPtr);
    CPSS_NULL_PTR_CHECK_MAC(configMaskPtr);
    CPSS_NULL_PTR_CHECK_MAC(actionPtr);

    /* classify tunnel type according to the required by the function to */
    /* convert tunnel termination config to hardware format              */
    switch (tunnelType)
    {
    case CPSS_TUNNEL_IPV4_OVER_IPV4_E:
        configTunnelType = CPSS_TUNNEL_IPV4_OVER_IPV4_E;
        break;
    case CPSS_TUNNEL_IPV4_OVER_GRE_IPV4_E:
        configTunnelType = CPSS_TUNNEL_IPV4_OVER_GRE_IPV4_E;
        break;
    case CPSS_TUNNEL_IPV6_OVER_IPV4_E:
        configTunnelType = CPSS_TUNNEL_IPV6_OVER_IPV4_E;
        break;
    case CPSS_TUNNEL_IPV6_OVER_GRE_IPV4_E:
        configTunnelType = CPSS_TUNNEL_IPV6_OVER_GRE_IPV4_E;
        break;
    case CPSS_TUNNEL_IP_OVER_MPLS_E:
    case CPSS_TUNNEL_ETHERNET_OVER_MPLS_E:
        configTunnelType = CPSS_TUNNEL_X_OVER_MPLS_E;
        break;
    CPSS_COVERITY_NON_ISSUE_BOOKMARK
    /* coverity[dead_error_begin] */default:
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    /* convert tunnel termination configuration to hardware format */
    rc = ch2Only_tunnelTermConfigLogic2HwFormat(configTunnelType,GT_FALSE,
                                        configPtr,hwTcamKeyArray);
    if (rc != GT_OK)
        return rc;

    /* convert tunnel termination configuration mask to hardware format */
    rc = ch2Only_tunnelTermConfigLogic2HwFormat(configTunnelType,GT_TRUE,
                                        configMaskPtr,hwTcamMaskArray);
    if (rc != GT_OK)
        return rc;

    /* classify tunnel type according to the required by the function to */
    /* convert tunnel termination action to hardware format              */
    /* No check for wrong cases is needed - done in the switch-case above*/
    if (tunnelType == CPSS_TUNNEL_ETHERNET_OVER_MPLS_E)
    {
        actionTunnelType = CPSS_TUNNEL_ETHERNET_OVER_MPLS_E;
    }
    else
    {
        actionTunnelType = CPSS_TUNNEL_IP_OVER_X_E;
    }

    /* convert tunnel termination action to hardware format */
    rc = ch2Only_tunnelTermActionLogic2HwFormat(actionTunnelType,actionPtr,hwTcamActionArray);
    if (rc != GT_OK)
        return rc;

    /* now write entry to the router / tunnel termination TCAM */
    rc = prvCpssDxChRouterTunnelTermTcamSetLine(devNum,
                                                CPSS_PORT_GROUP_UNAWARE_MODE_CNS,
                                                routerTunnelTermTcamIndex,
                                                hwTcamKeyArray,
                                                hwTcamMaskArray,
                                                hwTcamActionArray);

    return rc;
}

/*******************************************************************************
* cpssDxChTunnelTermEntrySet
*
* DESCRIPTION:
*       Set a tunnel termination entry.
*
* APPLICABLE DEVICES:
*        DxCh2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum        - physical device number
*       routerTunnelTermTcamIndex  - index for the tunnel termination entry in
*                       the router / tunnel termination TCAM
*                       (APPLICABLE RANGES: 0...1023)
*       tunnelType    - type of the tunnel; valid options:
*                       CPSS_TUNNEL_IPV4_OVER_IPV4_E
*                       CPSS_TUNNEL_IPV4_OVER_GRE_IPV4_E
*                       CPSS_TUNNEL_IPV6_OVER_IPV4_E
*                       CPSS_TUNNEL_IPV6_OVER_GRE_IPV4_E
*                       CPSS_TUNNEL_IP_OVER_MPLS_E
*                       CPSS_TUNNEL_ETHERNET_OVER_MPLS_E
*       configPtr     - points to tunnel termination configuration
*       configMaskPtr - points to tunnel termination configuration mask. The
*                       mask is AND styled one. Mask bit's 0 means don't care
*                       bit (corresponding bit in the pattern is not used in
*                       the TCAM lookup). Mask bit's 1 means that corresponding
*                       bit in the pattern is used in the TCAM lookup.
*       actionPtr     - points to tunnel termination action
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - on bad parameter.
*       GT_BAD_PTR               - on NULL pointer
*       GT_FAIL                  - on failure.
*       GT_OUT_OF_RANGE          - parameter not in valid range.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       Tunnel termination entries and IP lookup entries both reside in
*       router / tunnel termination TCAM. The router / tunnel termination TCAM
*       contains 1K lines. Each line can hold:
*       - 1 tunnel termination entry
*       - 1 ipv6 addresses
*       - 5 ipv4 addresses
*       Indexes for entries that takes one full line (meaning tunnel termination
*       and ipv6 lookup address) range (0..1023); Indexes for other entires
*       range (0..5119); Those indexes are counted COLUMN BY COLUMN  meaning
*       indexes 0..1023 reside in the first column, indexes 1024..2047 reside
*       in the second column and so on.
*       Therefore, tunnel termination entry at index n share the same TCAM line
*       with ipv6 lookup address at index n and share the same TCAM line with
*       ipv4 lookup addresses at index n, 1024+n, 2048+n, 3072+n and 4096+n.
*       For example, tunnel termination entry at TCAM line 100 share the
*       same TCAM line with ipv6 lookup address at line 100 and also share the
*       same TCAM line with ipv4 lookup addresses at indexes 100, 1124, 2148,
*       3172 and 4196.
*
*******************************************************************************/
GT_STATUS cpssDxChTunnelTermEntrySet
(
    IN  GT_U8                               devNum,
    IN  GT_U32                              routerTunnelTermTcamIndex,
    IN  CPSS_TUNNEL_TYPE_ENT                tunnelType,
    IN  CPSS_DXCH_TUNNEL_TERM_CONFIG_UNT    *configPtr,
    IN  CPSS_DXCH_TUNNEL_TERM_CONFIG_UNT    *configMaskPtr,
    IN  CPSS_DXCH_TUNNEL_TERM_ACTION_STC    *actionPtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChTunnelTermEntrySet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, routerTunnelTermTcamIndex, tunnelType, configPtr, configMaskPtr, actionPtr));

    rc = internal_cpssDxChTunnelTermEntrySet(devNum, routerTunnelTermTcamIndex, tunnelType, configPtr, configMaskPtr, actionPtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, routerTunnelTermTcamIndex, tunnelType, configPtr, configMaskPtr, actionPtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}


/*******************************************************************************
* internal_cpssDxChTunnelTermEntryGet
*
* DESCRIPTION:
*       Get tunnel termination entry from hardware at a given index.
*
* APPLICABLE DEVICES:
*        DxCh2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum        - physical device number
*       routerTunnelTermTcamIndex  - index for the tunnel termination entry in
*                       the router / tunnel termination TCAM
*                       (APPLICABLE RANGES: 0...1023)
*
* OUTPUTS:
*       validPtr      - points to whether this tunnel termination entry is valid
*       tunnelTypePtr - points to the type of the tunnel; valid options:
*                       CPSS_TUNNEL_IPV4_OVER_IPV4_E
*                       CPSS_TUNNEL_IPV4_OVER_GRE_IPV4_E
*                       CPSS_TUNNEL_IPV6_OVER_IPV4_E
*                       CPSS_TUNNEL_IPV6_OVER_GRE_IPV4_E
*                       CPSS_TUNNEL_IP_OVER_MPLS_E
*                       CPSS_TUNNEL_ETHERNET_OVER_MPLS_E
*       configPtr     - points to tunnel termination configuration
*       configMaskPtr - points to tunnel termination configuration mask. The
*                       mask is AND styled one. Mask bit's 0 means don't care
*                       bit (corresponding bit in the pattern is not used in
*                       the TCAM lookup). Mask bit's 1 means that corresponding
*                       bit in the pattern is used in the TCAM lookup.
*       actionPtr     - points to tunnel termination action
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - on bad parameter.
*       GT_BAD_PTR               - on NULL pointer
*       GT_FAIL                  - on failure.
*       GT_OUT_OF_RANGE          - parameter not in valid range.
*       GT_BAD_STATE             - on invalid tunnel type
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       Tunnel termination entries and IP lookup entries both reside in
*       router / tunnel termination TCAM. The router / tunnel termination TCAM
*       contains 1K lines. Each line can hold:
*       - 1 tunnel termination entry
*       - 1 ipv6 addresses
*       - 5 ipv4 addresses
*       Indexes for entries that takes one full line (meaning tunnel termination
*       and ipv6 lookup address) range (0..1023); Indexes for other entires
*       range (0..5119); Those indexes are counted COLUMN BY COLUMN  meaning
*       indexes 0..1023 reside in the first column, indexes 1024..2047 reside
*       in the second column and so on.
*       Therefore, tunnel termination entry at index n share the same TCAM line
*       with ipv6 lookup address at index n and share the same TCAM line with
*       ipv4 lookup addresses at index n, 1024+n, 2048+n, 3072+n and 4096+n.
*       For example, tunnel termination entry at TCAM line 100 share the
*       same TCAM line with ipv6 lookup address at line 100 and also share the
*       same TCAM line with ipv4 lookup addresses at indexes 100, 1124, 2148,
*       3172 and 4196.
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChTunnelTermEntryGet
(
    IN  GT_U8                               devNum,
    IN  GT_U32                              routerTunnelTermTcamIndex,
    OUT GT_BOOL                             *validPtr,
    OUT CPSS_TUNNEL_TYPE_ENT                *tunnelTypePtr,
    OUT CPSS_DXCH_TUNNEL_TERM_CONFIG_UNT    *configPtr,
    OUT CPSS_DXCH_TUNNEL_TERM_CONFIG_UNT    *configMaskPtr,
    OUT CPSS_DXCH_TUNNEL_TERM_ACTION_STC    *actionPtr
)
{
    GT_U32      hwTcamKeyArray[TUNNEL_TERMINATION_TCAM_RULE_SIZE_CNS];      /* TCAM key in hw format            */
    GT_U32      hwTcamMaskArray[TUNNEL_TERMINATION_TCAM_RULE_SIZE_CNS];     /* TCAM mask in hw format           */
    GT_U32      hwTcamActionArray[TUNNEL_TERMINATION_TCAM_ACTION_SIZE_CNS]; /* TCAM action (rule) in hw format  */
    GT_U32      validArr[5];                                                /* TCAM line valid bits             */
    GT_U32      compareModeArr[5];                                          /* TCAM line compare mode           */
    CPSS_TUNNEL_TYPE_ENT    configTunnelType;       /* tunnel type got from config      */
    CPSS_TUNNEL_TYPE_ENT    configMaskTunnelType;   /* tunnel type got from config mask */
    CPSS_TUNNEL_TYPE_ENT    actionTunnelType;       /* tunnel type got from action      */
    GT_STATUS               rc;                     /* function return code             */
    GT_U32                  i;

    /* check pararmeters */
    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);

    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E | CPSS_LION_E | CPSS_XCAT2_E | CPSS_LION2_E | CPSS_BOBCAT2_E | CPSS_CAELUM_E | CPSS_BOBCAT3_E);

    PRV_CPSS_DXCH_ROUTER_TUNNEL_TERM_TCAM_CHECK_LINE_INDEX_MAC(devNum,routerTunnelTermTcamIndex);
    CPSS_NULL_PTR_CHECK_MAC(configPtr);
    CPSS_NULL_PTR_CHECK_MAC(configMaskPtr);
    CPSS_NULL_PTR_CHECK_MAC(actionPtr);
    CPSS_NULL_PTR_CHECK_MAC(tunnelTypePtr);
    CPSS_NULL_PTR_CHECK_MAC(validPtr);


    /* read tunnel termination configuration and action from hardware */
    rc = prvCpssDxChRouterTunnelTermTcamGetLine(devNum,
                                                CPSS_PORT_GROUP_UNAWARE_MODE_CNS,
                                                routerTunnelTermTcamIndex,
                                                validArr,
                                                compareModeArr,
                                                hwTcamKeyArray,
                                                hwTcamMaskArray,
                                                hwTcamActionArray);
    if (rc != GT_OK)
        return rc;

    /* line holds valid tunnel termination entry if the following applies:
         - all entries are valid
         - the compare mode or all entries is row compare
         - keyArr[3] bit 31 must be 1 (to indicate TT entry and not IPv6 entry) */
    *validPtr = GT_TRUE;
    for (i = 0 ; i < 5 ; i++)
    {
        /* if entry is not valid or is single compare mode, whole line is not valid */
        if ((validArr[i] == 0) || (compareModeArr[i] == 0))
        {
            *validPtr = GT_FALSE;
            break;
        }
    }
    /* if whole line is valid, verify it is indeed TT entry and not IPv6 entry */
    if ((*validPtr == GT_TRUE) && (((hwTcamKeyArray[3] >> 31) & 0x1) != 1))
    {
        *validPtr = GT_FALSE;
    }
    /* if the entry is not valid, no need to continue to decode it */
    if (*validPtr == GT_FALSE)
        return GT_OK;

    /* convert tunnel termination configuration from hw format to logic format */
    rc = ch2Only_tunnelTermConfigHw2LogicFormat(hwTcamKeyArray,&configTunnelType,configPtr);
    if (rc != GT_OK)
        return rc;

    /* convert tunnel termination configuration mask from hw format to logic format */
    rc = ch2Only_tunnelTermConfigHw2LogicFormat(hwTcamMaskArray,&configMaskTunnelType,configMaskPtr);
    if (rc != GT_OK)
        return rc;

    /* convert tunnel termination action from hw format to logic format */
    rc = ch2Only_tunnelTermActionHw2LogicFormat(hwTcamActionArray,&actionTunnelType,actionPtr);
    if (rc != GT_OK)
        return rc;

    /* classify tunnel type */
    if ((configTunnelType == CPSS_TUNNEL_IPV4_OVER_IPV4_E) ||
        (configTunnelType == CPSS_TUNNEL_IPV4_OVER_GRE_IPV4_E) ||
        (configTunnelType == CPSS_TUNNEL_IPV6_OVER_IPV4_E) ||
        (configTunnelType == CPSS_TUNNEL_IPV6_OVER_GRE_IPV4_E))
    {
        *tunnelTypePtr = configTunnelType;
    }
    else if (configTunnelType == CPSS_TUNNEL_X_OVER_MPLS_E)
    {
        if (actionTunnelType == CPSS_TUNNEL_ETHERNET_OVER_MPLS_E)
        {
            *tunnelTypePtr = CPSS_TUNNEL_ETHERNET_OVER_MPLS_E;
        }
        else if (actionTunnelType == CPSS_TUNNEL_IP_OVER_X_E)
        {
            *tunnelTypePtr = CPSS_TUNNEL_IP_OVER_MPLS_E;
        }
        else
        {
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_STATE, LOG_ERROR_NO_MSG);
        }
    }
    else
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_STATE, LOG_ERROR_NO_MSG);
    }

    return rc;
}

/*******************************************************************************
* cpssDxChTunnelTermEntryGet
*
* DESCRIPTION:
*       Get tunnel termination entry from hardware at a given index.
*
* APPLICABLE DEVICES:
*        DxCh2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum        - physical device number
*       routerTunnelTermTcamIndex  - index for the tunnel termination entry in
*                       the router / tunnel termination TCAM
*                       (APPLICABLE RANGES: 0...1023)
*
* OUTPUTS:
*       validPtr      - points to whether this tunnel termination entry is valid
*       tunnelTypePtr - points to the type of the tunnel; valid options:
*                       CPSS_TUNNEL_IPV4_OVER_IPV4_E
*                       CPSS_TUNNEL_IPV4_OVER_GRE_IPV4_E
*                       CPSS_TUNNEL_IPV6_OVER_IPV4_E
*                       CPSS_TUNNEL_IPV6_OVER_GRE_IPV4_E
*                       CPSS_TUNNEL_IP_OVER_MPLS_E
*                       CPSS_TUNNEL_ETHERNET_OVER_MPLS_E
*       configPtr     - points to tunnel termination configuration
*       configMaskPtr - points to tunnel termination configuration mask. The
*                       mask is AND styled one. Mask bit's 0 means don't care
*                       bit (corresponding bit in the pattern is not used in
*                       the TCAM lookup). Mask bit's 1 means that corresponding
*                       bit in the pattern is used in the TCAM lookup.
*       actionPtr     - points to tunnel termination action
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - on bad parameter.
*       GT_BAD_PTR               - on NULL pointer
*       GT_FAIL                  - on failure.
*       GT_OUT_OF_RANGE          - parameter not in valid range.
*       GT_BAD_STATE             - on invalid tunnel type
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       Tunnel termination entries and IP lookup entries both reside in
*       router / tunnel termination TCAM. The router / tunnel termination TCAM
*       contains 1K lines. Each line can hold:
*       - 1 tunnel termination entry
*       - 1 ipv6 addresses
*       - 5 ipv4 addresses
*       Indexes for entries that takes one full line (meaning tunnel termination
*       and ipv6 lookup address) range (0..1023); Indexes for other entires
*       range (0..5119); Those indexes are counted COLUMN BY COLUMN  meaning
*       indexes 0..1023 reside in the first column, indexes 1024..2047 reside
*       in the second column and so on.
*       Therefore, tunnel termination entry at index n share the same TCAM line
*       with ipv6 lookup address at index n and share the same TCAM line with
*       ipv4 lookup addresses at index n, 1024+n, 2048+n, 3072+n and 4096+n.
*       For example, tunnel termination entry at TCAM line 100 share the
*       same TCAM line with ipv6 lookup address at line 100 and also share the
*       same TCAM line with ipv4 lookup addresses at indexes 100, 1124, 2148,
*       3172 and 4196.
*
*******************************************************************************/
GT_STATUS cpssDxChTunnelTermEntryGet
(
    IN  GT_U8                               devNum,
    IN  GT_U32                              routerTunnelTermTcamIndex,
    OUT GT_BOOL                             *validPtr,
    OUT CPSS_TUNNEL_TYPE_ENT                *tunnelTypePtr,
    OUT CPSS_DXCH_TUNNEL_TERM_CONFIG_UNT    *configPtr,
    OUT CPSS_DXCH_TUNNEL_TERM_CONFIG_UNT    *configMaskPtr,
    OUT CPSS_DXCH_TUNNEL_TERM_ACTION_STC    *actionPtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChTunnelTermEntryGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, routerTunnelTermTcamIndex, validPtr, tunnelTypePtr, configPtr, configMaskPtr, actionPtr));

    rc = internal_cpssDxChTunnelTermEntryGet(devNum, routerTunnelTermTcamIndex, validPtr, tunnelTypePtr, configPtr, configMaskPtr, actionPtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, routerTunnelTermTcamIndex, validPtr, tunnelTypePtr, configPtr, configMaskPtr, actionPtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}


/*******************************************************************************
* internal_cpssDxChTunnelTermEntryInvalidate
*
* DESCRIPTION:
*       Invalidate a tunnel termination entry.
*
* APPLICABLE DEVICES:
*        DxCh2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum        - physical device number
*       routerTunnelTermTcamIndex  - index for the tunnel termination entry in
*                       the router / tunnel termination TCAM
*                       (APPLICABLE RANGES: 0...1023)
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - on bad parameter.
*       GT_FAIL                  - on failure.
*       GT_TIMEOUT  - after max number of retries checking if PP ready
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChTunnelTermEntryInvalidate
(
    IN  GT_U8       devNum,
    IN  GT_U32      routerTunnelTermTcamIndex
)
{
    GT_STATUS   rc;     /* function return code */

    /* check parameters */
    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);

    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E | CPSS_LION_E | CPSS_XCAT2_E | CPSS_LION2_E | CPSS_BOBCAT2_E | CPSS_CAELUM_E | CPSS_BOBCAT3_E);

    PRV_CPSS_DXCH_ROUTER_TUNNEL_TERM_TCAM_CHECK_LINE_INDEX_MAC(devNum,routerTunnelTermTcamIndex);

    /* now invalidate the router / tunnel termination TCAM entry */
    rc = prvCpssDxChRouterTunnelTermTcamInvalidateLine(devNum,
                                     CPSS_PORT_GROUP_UNAWARE_MODE_CNS,
                                     routerTunnelTermTcamIndex);

    return rc;
}

/*******************************************************************************
* cpssDxChTunnelTermEntryInvalidate
*
* DESCRIPTION:
*       Invalidate a tunnel termination entry.
*
* APPLICABLE DEVICES:
*        DxCh2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum        - physical device number
*       routerTunnelTermTcamIndex  - index for the tunnel termination entry in
*                       the router / tunnel termination TCAM
*                       (APPLICABLE RANGES: 0...1023)
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - on bad parameter.
*       GT_FAIL                  - on failure.
*       GT_TIMEOUT  - after max number of retries checking if PP ready
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChTunnelTermEntryInvalidate
(
    IN  GT_U8       devNum,
    IN  GT_U32      routerTunnelTermTcamIndex
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChTunnelTermEntryInvalidate);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, routerTunnelTermTcamIndex));

    rc = internal_cpssDxChTunnelTermEntryInvalidate(devNum, routerTunnelTermTcamIndex);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, routerTunnelTermTcamIndex));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}


/*******************************************************************************
* internal_cpssDxChTunnelStartEgressFilteringSet
*
* DESCRIPTION:
*       Set globally whether to subject Tunnel Start packets to egress VLAN
*       filtering and to egress Spanning Tree filtering.
*
* APPLICABLE DEVICES:
*        DxCh2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum      - physical device number
*       enable      - enable / disable
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - on bad parameter.
*       GT_FAIL                  - on error.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChTunnelStartEgressFilteringSet
(
    IN  GT_U8   devNum,
    IN  GT_BOOL enable
)
{
    GT_U32      value;      /* value to write */
    GT_U32      regAddr;    /* register address */
    GT_STATUS   rc;         /* function return code */

    /* check parameters */
    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);

    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E | CPSS_LION_E | CPSS_XCAT2_E | CPSS_LION2_E | CPSS_BOBCAT2_E | CPSS_CAELUM_E | CPSS_BOBCAT3_E);


    /* write enable value */
    value = (enable == GT_TRUE) ? 1 : 0;

    if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_ENABLED_MAC(devNum) == GT_TRUE)
    {
        regAddr = PRV_DXCH_REG1_UNIT_EGF_EFT_MAC(devNum).egrFilterConfigs.egrFiltersEnable;
        rc =  prvCpssHwPpSetRegField(devNum, regAddr, 7, 1, value);
    }
    else
    if(0 == PRV_CPSS_DXCH_PP_HW_INFO_TXQ_REV_1_OR_ABOVE_MAC(devNum))
    {
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->egrTxQConf.txQueueResSharingAndTunnelEgrFltr;
        rc = prvCpssHwPpSetRegField(devNum,regAddr,25,1,value);
    }
    else
    {
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->txqVer1.egr.
            filterConfig.globalEnables;
        rc = prvCpssHwPpSetRegField(devNum,regAddr,8,1,value);
    }

    return rc;
}

/*******************************************************************************
* cpssDxChTunnelStartEgressFilteringSet
*
* DESCRIPTION:
*       Set globally whether to subject Tunnel Start packets to egress VLAN
*       filtering and to egress Spanning Tree filtering.
*
* APPLICABLE DEVICES:
*        DxCh2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum      - physical device number
*       enable      - enable / disable
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - on bad parameter.
*       GT_FAIL                  - on error.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChTunnelStartEgressFilteringSet
(
    IN  GT_U8   devNum,
    IN  GT_BOOL enable
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChTunnelStartEgressFilteringSet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, enable));

    rc = internal_cpssDxChTunnelStartEgressFilteringSet(devNum, enable);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, enable));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}


/*******************************************************************************
* internal_cpssDxChTunnelStartEgressFilteringGet
*
* DESCRIPTION:
*       Get if Tunnel Start packets are globally subject to egress VLAN
*       filtering and to egress Spanning Tree filtering.
*
* APPLICABLE DEVICES:
*        DxCh2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum      - physical device number
*
* OUTPUTS:
*       enablePtr   - points to enable / disable
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - on bad parameter.
*       GT_BAD_PTR               - on NULL pointer
*       GT_FAIL                  - on error.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChTunnelStartEgressFilteringGet
(
    IN  GT_U8   devNum,
    OUT GT_BOOL *enablePtr
)
{
    GT_U32      value;      /* register value */
    GT_U32      regAddr;    /* register address */
    GT_STATUS   rc;         /* function return code */

    /* check parameters */
    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);

    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E | CPSS_LION_E | CPSS_XCAT2_E | CPSS_LION2_E | CPSS_BOBCAT2_E | CPSS_CAELUM_E | CPSS_BOBCAT3_E);

    CPSS_NULL_PTR_CHECK_MAC(enablePtr);

    if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_ENABLED_MAC(devNum) == GT_TRUE)
    {
        regAddr = PRV_DXCH_REG1_UNIT_EGF_EFT_MAC(devNum).egrFilterConfigs.egrFiltersEnable;
        rc = prvCpssHwPpGetRegField(devNum,regAddr,7,1,&value);
    }
    else
    if(0 == PRV_CPSS_DXCH_PP_HW_INFO_TXQ_REV_1_OR_ABOVE_MAC(devNum))
    {
        /* read register value */
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->egrTxQConf.txQueueResSharingAndTunnelEgrFltr;
        rc = prvCpssHwPpGetRegField(devNum,regAddr,25,1,&value);
    }
    else
    {
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->txqVer1.egr.
            filterConfig.globalEnables;
        rc = prvCpssHwPpGetRegField(devNum,regAddr,8,1,&value);
    }

    if (rc != GT_OK)
        return rc;

    *enablePtr = (value != 0) ? GT_TRUE : GT_FALSE;

    return GT_OK;
}

/*******************************************************************************
* cpssDxChTunnelStartEgressFilteringGet
*
* DESCRIPTION:
*       Get if Tunnel Start packets are globally subject to egress VLAN
*       filtering and to egress Spanning Tree filtering.
*
* APPLICABLE DEVICES:
*        DxCh2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum      - physical device number
*
* OUTPUTS:
*       enablePtr   - points to enable / disable
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - on bad parameter.
*       GT_BAD_PTR               - on NULL pointer
*       GT_FAIL                  - on error.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChTunnelStartEgressFilteringGet
(
    IN  GT_U8   devNum,
    OUT GT_BOOL *enablePtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChTunnelStartEgressFilteringGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, enablePtr));

    rc = internal_cpssDxChTunnelStartEgressFilteringGet(devNum, enablePtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, enablePtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}


/*******************************************************************************
* internal_cpssDxChEthernetOverMplsTunnelStartTaggingSet
*
* DESCRIPTION:
*       Set the global Tagging state for the Ethernet passenger packet to
*       be Tagged or Untagged.
*       The Ethernet passenger packet may have a VLAN tag added, removed,
*       or modified prior to its Tunnel Start encapsulation.
*       The Ethernet passenger packet is treated according to the following
*       modification modes:
*         - Transmit the passenger packet without any modifications to its VLAN
*           tagged state (i.e. if it arrived untagged, transmit untagged; if it
*           arrived tagged, transmit tagged)
*         - Transmit the passenger packet with an additional (nested) VLAN
*           tag regardless of whether it was received tagged or untagged
*         - Transmit the passenger packet tagged (i.e. if it arrived untagged,
*           a tag is added; if it arrived tagged it is transmitted tagged with
*           the new VID assignment)
*         - Transmit the passenger packet untagged (i.e. if it arrived tagged
*           it is transmitted untagged; if it arrived untagged it is
*           transmitted untagged)
*       Based on the global Ethernet passenger tag mode and the Ingress Policy
*       Action Nested VLAN Access mode, the following list indicates how the
*       Ethernet passenger is modified prior to its being encapsulated by the
*       tunnel header:
*         - <Tunnel Start tag> is Untagged & <Nested VLAN Access> = 1
*                   Ethernet passenger tagging is not modified, regardless of
*                   whether it is tagged or untagged.
*         - <Tunnel Start tag> is Untagged & <Nested VLAN Access> = 0
*                   Transmit Untagged. NOTE: If the Ethernet passenger packet
*                   is tagged, the tag is removed. If the Ethernet passenger
*                   packet is untagged, the packet is not modified.
*         - <Tunnel Start tag> is Tagged & <Nested VLAN Access> = 1
*                   A new tag is added to the Ethernet passenger packet,
*                   regardless of whether it is tagged or untagged.
*         - <Tunnel Start tag> is Tagged & <Nested VLAN Access> = 0
*                   Transmit Tagged. NOTE: If the Ethernet passenger packet
*                   is untagged, a tag is added. If the Ethernet passenger
*                   packet is tagged, the existing tag VID is set to the
*                   packet VID assignment.
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum      - physical device number
*       enable      - enable / disable
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - on bad parameter.
*       GT_FAIL                  - on error.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       Despite its name, this API is applicable to all Tunnel Starts, not just MPLS.
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChEthernetOverMplsTunnelStartTaggingSet
(
    IN  GT_U8   devNum,
    IN  GT_BOOL enable
)
{
    GT_U32      value;      /* value to write */
    GT_U32      regAddr;    /* register address */
    GT_STATUS   rc;         /* function return code */

    /* check parameters */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum, CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_BOBCAT2_E | CPSS_CAELUM_E | CPSS_BOBCAT3_E);


    /* write enable value */
    value = (enable == GT_TRUE) ? 1 : 0;

    if(0 == PRV_CPSS_DXCH_PP_HW_INFO_TXQ_REV_1_OR_ABOVE_MAC(devNum))
    {
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->egrTxQConf.txQueueResSharingAndTunnelEgrFltr;
        rc = prvCpssHwPpSetRegField(devNum,regAddr,27,1,value);
    }
    else
    {
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->txqVer1.queue.
            distributor.generalConfig;
        rc = prvCpssHwPpSetRegField(devNum,regAddr,12,1,value);
    }

    return rc;
}

/*******************************************************************************
* cpssDxChEthernetOverMplsTunnelStartTaggingSet
*
* DESCRIPTION:
*       Set the global Tagging state for the Ethernet passenger packet to
*       be Tagged or Untagged.
*       The Ethernet passenger packet may have a VLAN tag added, removed,
*       or modified prior to its Tunnel Start encapsulation.
*       The Ethernet passenger packet is treated according to the following
*       modification modes:
*         - Transmit the passenger packet without any modifications to its VLAN
*           tagged state (i.e. if it arrived untagged, transmit untagged; if it
*           arrived tagged, transmit tagged)
*         - Transmit the passenger packet with an additional (nested) VLAN
*           tag regardless of whether it was received tagged or untagged
*         - Transmit the passenger packet tagged (i.e. if it arrived untagged,
*           a tag is added; if it arrived tagged it is transmitted tagged with
*           the new VID assignment)
*         - Transmit the passenger packet untagged (i.e. if it arrived tagged
*           it is transmitted untagged; if it arrived untagged it is
*           transmitted untagged)
*       Based on the global Ethernet passenger tag mode and the Ingress Policy
*       Action Nested VLAN Access mode, the following list indicates how the
*       Ethernet passenger is modified prior to its being encapsulated by the
*       tunnel header:
*         - <Tunnel Start tag> is Untagged & <Nested VLAN Access> = 1
*                   Ethernet passenger tagging is not modified, regardless of
*                   whether it is tagged or untagged.
*         - <Tunnel Start tag> is Untagged & <Nested VLAN Access> = 0
*                   Transmit Untagged. NOTE: If the Ethernet passenger packet
*                   is tagged, the tag is removed. If the Ethernet passenger
*                   packet is untagged, the packet is not modified.
*         - <Tunnel Start tag> is Tagged & <Nested VLAN Access> = 1
*                   A new tag is added to the Ethernet passenger packet,
*                   regardless of whether it is tagged or untagged.
*         - <Tunnel Start tag> is Tagged & <Nested VLAN Access> = 0
*                   Transmit Tagged. NOTE: If the Ethernet passenger packet
*                   is untagged, a tag is added. If the Ethernet passenger
*                   packet is tagged, the existing tag VID is set to the
*                   packet VID assignment.
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum      - physical device number
*       enable      - enable / disable
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - on bad parameter.
*       GT_FAIL                  - on error.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       Despite its name, this API is applicable to all Tunnel Starts, not just MPLS.
*
*******************************************************************************/
GT_STATUS cpssDxChEthernetOverMplsTunnelStartTaggingSet
(
    IN  GT_U8   devNum,
    IN  GT_BOOL enable
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChEthernetOverMplsTunnelStartTaggingSet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, enable));

    rc = internal_cpssDxChEthernetOverMplsTunnelStartTaggingSet(devNum, enable);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, enable));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}


/*******************************************************************************
* internal_cpssDxChEthernetOverMplsTunnelStartTaggingGet
*
* DESCRIPTION:
*       Set the global Tagging state for the Ethernet passenger packet to
*       be Tagged or Untagged.
*       The Ethernet passenger packet may have a VLAN tag added, removed,
*       or modified prior to its Tunnel Start encapsulation.
*       The Ethernet passenger packet is treated according to the following
*       modification modes:
*         - Transmit the passenger packet without any modifications to its VLAN
*           tagged state (i.e. if it arrived untagged, transmit untagged; if it
*           arrived tagged, transmit tagged)
*         - Transmit the passenger packet with an additional (nested) VLAN
*           tag regardless of whether it was received tagged or untagged
*         - Transmit the passenger packet tagged (i.e. if it arrived untagged,
*           a tag is added; if it arrived tagged it is transmitted tagged with
*           the new VID assignment)
*         - Transmit the passenger packet untagged (i.e. if it arrived tagged
*           it is transmitted untagged; if it arrived untagged it is
*           transmitted untagged)
*       Based on the global Ethernet passenger tag mode and the Ingress Policy
*       Action Nested VLAN Access mode, the following list indicates how the
*       Ethernet passenger is modified prior to its being encapsulated by the
*       tunnel header:
*         - <Tunnel Start tag> is Untagged & <Nested VLAN Access> = 1
*                   Ethernet passenger tagging is not modified, regardless of
*                   whether it is tagged or untagged.
*         - <Tunnel Start tag> is Untagged & <Nested VLAN Access> = 0
*                   Transmit Untagged. NOTE: If the Ethernet passenger packet
*                   is tagged, the tag is removed. If the Ethernet passenger
*                   packet is untagged, the packet is not modified.
*         - <Tunnel Start tag> is Tagged & <Nested VLAN Access> = 1
*                   A new tag is added to the Ethernet passenger packet,
*                   regardless of whether it is tagged or untagged.
*         - <Tunnel Start tag> is Tagged & <Nested VLAN Access> = 0
*                   Transmit Tagged. NOTE: If the Ethernet passenger packet
*                   is untagged, a tag is added. If the Ethernet passenger
*                   packet is tagged, the existing tag VID is set to the
*                   packet VID assignment.
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum      - physical device number
*
* OUTPUTS:
*       enablePtr   - points to enable / disable
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - on bad parameter.
*       GT_FAIL                  - on error.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       Despite its name, this API is applicable to all Tunnel Starts, not just MPLS.
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChEthernetOverMplsTunnelStartTaggingGet
(
    IN  GT_U8   devNum,
    OUT GT_BOOL *enablePtr
)
{
    GT_U32      value;      /* register value */
    GT_U32      regAddr;    /* register address */
    GT_STATUS   rc;         /* function return code */

    /* check parameters */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum, CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_BOBCAT2_E | CPSS_CAELUM_E | CPSS_BOBCAT3_E);

    CPSS_NULL_PTR_CHECK_MAC(enablePtr);

    /* read register value */
    if(0 == PRV_CPSS_DXCH_PP_HW_INFO_TXQ_REV_1_OR_ABOVE_MAC(devNum))
    {
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->egrTxQConf.txQueueResSharingAndTunnelEgrFltr;
        rc = prvCpssHwPpGetRegField(devNum,regAddr,27,1,&value);
    }
    else
    {
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->txqVer1.queue.
            distributor.generalConfig;
        rc = prvCpssHwPpGetRegField(devNum,regAddr,12,1,&value);
    }

    if (rc != GT_OK)
        return rc;

    *enablePtr = (value != 0) ? GT_TRUE : GT_FALSE;

    return GT_OK;
}

/*******************************************************************************
* cpssDxChEthernetOverMplsTunnelStartTaggingGet
*
* DESCRIPTION:
*       Set the global Tagging state for the Ethernet passenger packet to
*       be Tagged or Untagged.
*       The Ethernet passenger packet may have a VLAN tag added, removed,
*       or modified prior to its Tunnel Start encapsulation.
*       The Ethernet passenger packet is treated according to the following
*       modification modes:
*         - Transmit the passenger packet without any modifications to its VLAN
*           tagged state (i.e. if it arrived untagged, transmit untagged; if it
*           arrived tagged, transmit tagged)
*         - Transmit the passenger packet with an additional (nested) VLAN
*           tag regardless of whether it was received tagged or untagged
*         - Transmit the passenger packet tagged (i.e. if it arrived untagged,
*           a tag is added; if it arrived tagged it is transmitted tagged with
*           the new VID assignment)
*         - Transmit the passenger packet untagged (i.e. if it arrived tagged
*           it is transmitted untagged; if it arrived untagged it is
*           transmitted untagged)
*       Based on the global Ethernet passenger tag mode and the Ingress Policy
*       Action Nested VLAN Access mode, the following list indicates how the
*       Ethernet passenger is modified prior to its being encapsulated by the
*       tunnel header:
*         - <Tunnel Start tag> is Untagged & <Nested VLAN Access> = 1
*                   Ethernet passenger tagging is not modified, regardless of
*                   whether it is tagged or untagged.
*         - <Tunnel Start tag> is Untagged & <Nested VLAN Access> = 0
*                   Transmit Untagged. NOTE: If the Ethernet passenger packet
*                   is tagged, the tag is removed. If the Ethernet passenger
*                   packet is untagged, the packet is not modified.
*         - <Tunnel Start tag> is Tagged & <Nested VLAN Access> = 1
*                   A new tag is added to the Ethernet passenger packet,
*                   regardless of whether it is tagged or untagged.
*         - <Tunnel Start tag> is Tagged & <Nested VLAN Access> = 0
*                   Transmit Tagged. NOTE: If the Ethernet passenger packet
*                   is untagged, a tag is added. If the Ethernet passenger
*                   packet is tagged, the existing tag VID is set to the
*                   packet VID assignment.
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum      - physical device number
*
* OUTPUTS:
*       enablePtr   - points to enable / disable
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - on bad parameter.
*       GT_FAIL                  - on error.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       Despite its name, this API is applicable to all Tunnel Starts, not just MPLS.
*
*******************************************************************************/
GT_STATUS cpssDxChEthernetOverMplsTunnelStartTaggingGet
(
    IN  GT_U8   devNum,
    OUT GT_BOOL *enablePtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChEthernetOverMplsTunnelStartTaggingGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, enablePtr));

    rc = internal_cpssDxChEthernetOverMplsTunnelStartTaggingGet(devNum, enablePtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, enablePtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChEthernetOverMplsTunnelStartTagModeSet
*
* DESCRIPTION:
*       Set the vlan tag mode of the passanger packet for an
*       Ethernet-over-xxx tunnel start packet.
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum      - physical device number
*       tagMode     - tunnel start ethernet-over-x vlan tag mode
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - on bad parameter.
*       GT_FAIL                  - on error.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device.
*       GT_HW_ERROR              - on hardware error.
*
* COMMENTS:
*       Despite its name, this API is applicable to all Tunnel Starts, not just MPLS.
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChEthernetOverMplsTunnelStartTagModeSet
(
    IN  GT_U8                                               devNum,
    IN  CPSS_DXCH_TUNNEL_START_ETHERNET_OVER_X_TAG_MODE_ENT tagMode
)
{
    GT_U32    regAddr;           /* register address */
    GT_U32    data;              /* reg sub field data */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_BOBCAT2_E | CPSS_CAELUM_E | CPSS_BOBCAT3_E);


    switch (tagMode)
    {
    case CPSS_DXCH_TUNNEL_START_ETHERNET_OVER_X_TAG_MODE_FROM_VLAN_E:
        data = 0;
        break;
    case CPSS_DXCH_TUNNEL_START_ETHERNET_OVER_X_TAG_MODE_FROM_ENTRY_E:
        data = 1;
        break;
    default:
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    if(0 == PRV_CPSS_DXCH_PP_HW_INFO_TXQ_REV_1_OR_ABOVE_MAC(devNum))
    {
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->egrTxQConf.txQueueResSharingAndTunnelEgrFltr;

        return prvCpssHwPpSetRegField(devNum,regAddr,26,1,data);
    }
    else
    {
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->txqVer1.queue.
            distributor.generalConfig;
        return prvCpssHwPpSetRegField(devNum,regAddr,11,1,data);
    }

}

/*******************************************************************************
* cpssDxChEthernetOverMplsTunnelStartTagModeSet
*
* DESCRIPTION:
*       Set the vlan tag mode of the passanger packet for an
*       Ethernet-over-xxx tunnel start packet.
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum      - physical device number
*       tagMode     - tunnel start ethernet-over-x vlan tag mode
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - on bad parameter.
*       GT_FAIL                  - on error.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device.
*       GT_HW_ERROR              - on hardware error.
*
* COMMENTS:
*       Despite its name, this API is applicable to all Tunnel Starts, not just MPLS.
*
*******************************************************************************/
GT_STATUS cpssDxChEthernetOverMplsTunnelStartTagModeSet

(
    IN  GT_U8                                               devNum,
    IN  CPSS_DXCH_TUNNEL_START_ETHERNET_OVER_X_TAG_MODE_ENT tagMode
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChEthernetOverMplsTunnelStartTagModeSet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, tagMode));

    rc = internal_cpssDxChEthernetOverMplsTunnelStartTagModeSet(devNum, tagMode);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, tagMode));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChEthernetOverMplsTunnelStartTagModeGet
*
* DESCRIPTION:
*       Get the vlan tag mode of the passanger packet for an
*       Ethernet-over-xxx tunnel start packet.
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum      - physical device number
*
* OUTPUTS:
*       tagModePtr  - pointer to tunnel start ethernet-over-x vlan tag mode
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_STATE             - on bad state.
*       GT_BAD_PTR               - on NULL pointer.
*       GT_FAIL                  - on error.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device.
*       GT_HW_ERROR              - on hardware error.
*
* COMMENTS:
*       Despite its name, this API is applicable to all Tunnel Starts, not just MPLS.
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChEthernetOverMplsTunnelStartTagModeGet
(
    IN  GT_U8                                                devNum,
    OUT CPSS_DXCH_TUNNEL_START_ETHERNET_OVER_X_TAG_MODE_ENT *tagModePtr
)
{
    GT_U32    regAddr;           /* register address */
    GT_U32    data;              /* reg sub field data */
    GT_STATUS rc;                /* function return code */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_BOBCAT2_E | CPSS_CAELUM_E | CPSS_BOBCAT3_E);

    CPSS_NULL_PTR_CHECK_MAC(tagModePtr);

    if(0 == PRV_CPSS_DXCH_PP_HW_INFO_TXQ_REV_1_OR_ABOVE_MAC(devNum))
    {
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->egrTxQConf.txQueueResSharingAndTunnelEgrFltr;
        rc = prvCpssHwPpGetRegField(devNum,regAddr,26,1,&data);
    }
    else
    {
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->txqVer1.queue.
            distributor.generalConfig;
        rc = prvCpssHwPpGetRegField(devNum,regAddr,11,1,&data);
    }

    if (rc != GT_OK)
        return rc;

    switch (data)
    {
    case 0:
        *tagModePtr = CPSS_DXCH_TUNNEL_START_ETHERNET_OVER_X_TAG_MODE_FROM_VLAN_E;
        break;
    case 1:
        *tagModePtr = CPSS_DXCH_TUNNEL_START_ETHERNET_OVER_X_TAG_MODE_FROM_ENTRY_E;
        break;
    default:
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_STATE, LOG_ERROR_NO_MSG);
    }

    return GT_OK;

}

/*******************************************************************************
* cpssDxChEthernetOverMplsTunnelStartTagModeGet
*
* DESCRIPTION:
*       Get the vlan tag mode of the passanger packet for an
*       Ethernet-over-xxx tunnel start packet.
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum      - physical device number
*
* OUTPUTS:
*       tagModePtr  - pointer to tunnel start ethernet-over-x vlan tag mode
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_STATE             - on bad state.
*       GT_BAD_PTR               - on NULL pointer.
*       GT_FAIL                  - on error.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device.
*       GT_HW_ERROR              - on hardware error.
*
* COMMENTS:
*       Despite its name, this API is applicable to all Tunnel Starts, not just MPLS.
*
*******************************************************************************/
GT_STATUS cpssDxChEthernetOverMplsTunnelStartTagModeGet

(
    IN  GT_U8                                                devNum,
    OUT CPSS_DXCH_TUNNEL_START_ETHERNET_OVER_X_TAG_MODE_ENT *tagModePtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChEthernetOverMplsTunnelStartTagModeGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, tagModePtr));

    rc = internal_cpssDxChEthernetOverMplsTunnelStartTagModeGet(devNum, tagModePtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, tagModePtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChTunnelStartPassengerVlanTranslationEnableSet
*
* DESCRIPTION:
*       Controls Egress Vlan Translation of Ethernet tunnel start passengers.
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* INPUTS:
*       devNum      - physical device number
*       enable      - GT_TRUE: Ethernet tunnel start passengers are egress vlan
*                              translated, regardless of the VlanTranslationEnable
*                              configuration.
*                     GT_FALSE: Ethernet tunnel start passengers are to be egress
*                               vlan translated in accordance to the
*                               VlanTranslationEnable configuration.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - on bad parameter.
*       GT_BAD_PTR               - on NULL pointer.
*       GT_FAIL                  - on error.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device.
*       GT_HW_ERROR              - on hardware error.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChTunnelStartPassengerVlanTranslationEnableSet
(
    IN  GT_U8   devNum,
    IN  GT_BOOL enable
)
{
    GT_U32    regAddr;           /* register address */
    GT_U32    data;              /* reg sub field data */
    GT_U32    startBit;          /* start bit of the field */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E);


    if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_MAC(devNum) == GT_TRUE)
    {
        regAddr = PRV_DXCH_REG1_UNIT_HA_MAC(devNum).haGlobalConfig;
        startBit = 11;
    }
    else
    {
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->haRegs.hdrAltGlobalConfig;
        startBit = 10;
    }

    data = (enable == GT_TRUE) ? 1 : 0;

    return prvCpssHwPpSetRegField(devNum, regAddr, startBit , 1, data);

}

/*******************************************************************************
* cpssDxChTunnelStartPassengerVlanTranslationEnableSet
*
* DESCRIPTION:
*       Controls Egress Vlan Translation of Ethernet tunnel start passengers.
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* INPUTS:
*       devNum      - physical device number
*       enable      - GT_TRUE: Ethernet tunnel start passengers are egress vlan
*                              translated, regardless of the VlanTranslationEnable
*                              configuration.
*                     GT_FALSE: Ethernet tunnel start passengers are to be egress
*                               vlan translated in accordance to the
*                               VlanTranslationEnable configuration.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - on bad parameter.
*       GT_BAD_PTR               - on NULL pointer.
*       GT_FAIL                  - on error.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device.
*       GT_HW_ERROR              - on hardware error.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChTunnelStartPassengerVlanTranslationEnableSet
(
    IN  GT_U8   devNum,
    IN  GT_BOOL enable
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChTunnelStartPassengerVlanTranslationEnableSet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, enable));

    rc = internal_cpssDxChTunnelStartPassengerVlanTranslationEnableSet(devNum, enable);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, enable));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}


/*******************************************************************************
* internal_cpssDxChTunnelStartPassengerVlanTranslationEnableGet
*
* DESCRIPTION:
*       Gets the Egress Vlan Translation of Ethernet tunnel start passengers.
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* INPUTS:
*       devNum      - physical device number
*
* OUTPUTS:
*       enablePtr   - points enable state.
*                     GT_TRUE: Ethernet tunnel start passengers are egress vlan
*                              translated, regardless of the VlanTranslationEnable
*                              configuration.
*                     GT_FALSE: Ethernet tunnel start passengers are to be egress
*                               vlan translated in accordance to the
*                               VlanTranslationEnable configuration.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - on bad parameter.
*       GT_BAD_PTR               - on NULL pointer.
*       GT_FAIL                  - on error.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device.
*       GT_HW_ERROR              - on hardware error.
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChTunnelStartPassengerVlanTranslationEnableGet
(
    IN  GT_U8   devNum,
    OUT GT_BOOL *enablePtr
)
{
    GT_U32      regAddr;    /* register address */
    GT_U32      value;      /* hardware value */
    GT_STATUS   rc;         /* function return code */
    GT_U32    startBit;          /* start bit of the field */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E);

    CPSS_NULL_PTR_CHECK_MAC(enablePtr);

    if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_MAC(devNum) == GT_TRUE)
    {
        regAddr = PRV_DXCH_REG1_UNIT_HA_MAC(devNum).haGlobalConfig;
        startBit = 11;
    }
    else
    {
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->haRegs.hdrAltGlobalConfig;
        startBit = 10;
    }

    rc = prvCpssHwPpGetRegField(devNum, regAddr, startBit, 1, &value);

    if (rc != GT_OK)
        return rc;

    /* decode enable/disable state from hardware value */
    *enablePtr = (value != 0) ? GT_TRUE : GT_FALSE;

    return GT_OK;
}

/*******************************************************************************
* cpssDxChTunnelStartPassengerVlanTranslationEnableGet
*
* DESCRIPTION:
*       Gets the Egress Vlan Translation of Ethernet tunnel start passengers.
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* INPUTS:
*       devNum      - physical device number
*
* OUTPUTS:
*       enablePtr   - points enable state.
*                     GT_TRUE: Ethernet tunnel start passengers are egress vlan
*                              translated, regardless of the VlanTranslationEnable
*                              configuration.
*                     GT_FALSE: Ethernet tunnel start passengers are to be egress
*                               vlan translated in accordance to the
*                               VlanTranslationEnable configuration.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - on bad parameter.
*       GT_BAD_PTR               - on NULL pointer.
*       GT_FAIL                  - on error.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device.
*       GT_HW_ERROR              - on hardware error.
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChTunnelStartPassengerVlanTranslationEnableGet
(
    IN  GT_U8   devNum,
    OUT GT_BOOL *enablePtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChTunnelStartPassengerVlanTranslationEnableGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, enablePtr));

    rc = internal_cpssDxChTunnelStartPassengerVlanTranslationEnableGet(devNum, enablePtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, enablePtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}


/*******************************************************************************
* internal_cpssDxChTunnelTermExceptionCmdSet
*
* DESCRIPTION:
*       Set tunnel termination exception command.
*
* APPLICABLE DEVICES:
*        DxCh2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum        - physical device number
*       exceptionType - tunnel termination exception type to set command for
*       command       - command to set
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_FAIL                  - on error.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       Commands for the different exceptions are:
*
*       CPSS_DXCH_TUNNEL_TERM_IPV4_HEADER_ERROR_E       -
*                               CPSS_PACKET_CMD_TRAP_TO_CPU_E
*                               CPSS_PACKET_CMD_DROP_HARD_E
*
*       CPSS_DXCH_TUNNEL_TERM_IPV4_OPTION_FRAG_ERROR_E  -
*                               CPSS_PACKET_CMD_TRAP_TO_CPU_E
*                               CPSS_PACKET_CMD_DROP_HARD_E
*
*       CPSS_DXCH_TUNNEL_TERM_IPV4_UNSUP_GRE_ERROR_E    -
*                               CPSS_PACKET_CMD_TRAP_TO_CPU_E
*                               CPSS_PACKET_CMD_DROP_HARD_E
*
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChTunnelTermExceptionCmdSet
(
    IN  GT_U8                               devNum,
    IN  CPSS_DXCH_TUNNEL_TERM_EXCEPTION_ENT exceptionType,
    IN  CPSS_PACKET_CMD_ENT                 command
)
{
    GT_STATUS   rc;         /* function return code */
    GT_U32      value;      /* value to write */
    GT_U32      regAddr;    /* register address */
    GT_U32      offset;     /* offset in the register */

    /* check parameters */
    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);

    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E | CPSS_LION_E | CPSS_XCAT2_E | CPSS_LION2_E | CPSS_BOBCAT2_E | CPSS_CAELUM_E | CPSS_BOBCAT3_E);


    /* find register offset according to the exception type */
    switch (exceptionType)
    {
    case CPSS_DXCH_TUNNEL_TERM_IPV4_HEADER_ERROR_E:
        offset = 19;
        break;
    case CPSS_DXCH_TUNNEL_TERM_IPV4_OPTION_FRAG_ERROR_E:
        offset = 20;
        break;
    case CPSS_DXCH_TUNNEL_TERM_IPV4_UNSUP_GRE_ERROR_E:
        offset = 21;
        break;
    default:
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    /* note the commands for all exception types are the same  */
    /* get the value to write according to command             */
    switch (command)
    {
    case CPSS_PACKET_CMD_TRAP_TO_CPU_E:
        value = 0;
        break;
    case CPSS_PACKET_CMD_DROP_HARD_E:
        value = 1;
        break;
    default:
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    /* write command value */
    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->pclRegs.control;
    rc = prvCpssHwPpSetRegField(devNum,regAddr,offset,1,value);

    return rc;
}

/*******************************************************************************
* cpssDxChTunnelTermExceptionCmdSet
*
* DESCRIPTION:
*       Set tunnel termination exception command.
*
* APPLICABLE DEVICES:
*        DxCh2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum        - physical device number
*       exceptionType - tunnel termination exception type to set command for
*       command       - command to set
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_FAIL                  - on error.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       Commands for the different exceptions are:
*
*       CPSS_DXCH_TUNNEL_TERM_IPV4_HEADER_ERROR_E       -
*                               CPSS_PACKET_CMD_TRAP_TO_CPU_E
*                               CPSS_PACKET_CMD_DROP_HARD_E
*
*       CPSS_DXCH_TUNNEL_TERM_IPV4_OPTION_FRAG_ERROR_E  -
*                               CPSS_PACKET_CMD_TRAP_TO_CPU_E
*                               CPSS_PACKET_CMD_DROP_HARD_E
*
*       CPSS_DXCH_TUNNEL_TERM_IPV4_UNSUP_GRE_ERROR_E    -
*                               CPSS_PACKET_CMD_TRAP_TO_CPU_E
*                               CPSS_PACKET_CMD_DROP_HARD_E
*
*
*******************************************************************************/
GT_STATUS cpssDxChTunnelTermExceptionCmdSet
(
    IN  GT_U8                               devNum,
    IN  CPSS_DXCH_TUNNEL_TERM_EXCEPTION_ENT exceptionType,
    IN  CPSS_PACKET_CMD_ENT                 command
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChTunnelTermExceptionCmdSet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, exceptionType, command));

    rc = internal_cpssDxChTunnelTermExceptionCmdSet(devNum, exceptionType, command);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, exceptionType, command));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}


/*******************************************************************************
* internal_cpssDxChTunnelCtrlTtExceptionCmdGet
*
* DESCRIPTION:
*       Get tunnel termination exception command.
*
* APPLICABLE DEVICES:
*        DxCh2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum        - physical device number
*       exceptionType - tunnel termination exception type to set command for
*
* OUTPUTS:
*       commandPtr    - points to the command for the exception type
*
* RETURNS:
*       GT_OK       - on success.
*       GT_BAD_PTR  - on NULL pointer
*       GT_FAIL     - on error.
*
* COMMENTS:
*       Commands for the different exceptions are:
*
*       CPSS_DXCH_TUNNEL_TERM_IPV4_HEADER_ERROR_E       -
*                               CPSS_PACKET_CMD_TRAP_TO_CPU_E
*                               CPSS_PACKET_CMD_DROP_HARD_E
*
*       CPSS_DXCH_TUNNEL_TERM_IPV4_OPTION_FRAG_ERROR_E  -
*                               CPSS_PACKET_CMD_TRAP_TO_CPU_E
*                               CPSS_PACKET_CMD_DROP_HARD_E
*
*       CPSS_DXCH_TUNNEL_TERM_IPV4_UNSUP_GRE_ERROR_E    -
*                               CPSS_PACKET_CMD_TRAP_TO_CPU_E
*                               CPSS_PACKET_CMD_DROP_HARD_E
*
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChTunnelCtrlTtExceptionCmdGet
(
    IN  GT_U8                               devNum,
    IN  CPSS_DXCH_TUNNEL_TERM_EXCEPTION_ENT exceptionType,
    OUT CPSS_PACKET_CMD_ENT                 *commandPtr
)
{
    GT_STATUS   rc;         /* function return code */
    GT_U32      value;      /* register value */
    GT_U32      regAddr;    /* register address */
    GT_U32      offset;     /* offset in the register */

    /* check parameters */
    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);

    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E | CPSS_LION_E | CPSS_XCAT2_E | CPSS_LION2_E | CPSS_BOBCAT2_E | CPSS_CAELUM_E | CPSS_BOBCAT3_E);

    CPSS_NULL_PTR_CHECK_MAC(commandPtr);

    /* find register offset according to the exception type */
    switch (exceptionType)
    {
    case CPSS_DXCH_TUNNEL_TERM_IPV4_HEADER_ERROR_E:
        offset = 19;
        break;
    case CPSS_DXCH_TUNNEL_TERM_IPV4_OPTION_FRAG_ERROR_E:
        offset = 20;
        break;
    case CPSS_DXCH_TUNNEL_TERM_IPV4_UNSUP_GRE_ERROR_E:
        offset = 21;
        break;
    default:
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    /* read register value */
    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->pclRegs.control;
    rc = prvCpssHwPpGetRegField(devNum,regAddr,offset,1,&value);

    if (rc != GT_OK)
        return rc;

    *commandPtr = (value == 0) ? CPSS_PACKET_CMD_TRAP_TO_CPU_E : CPSS_PACKET_CMD_DROP_HARD_E;

    return rc;
}

/*******************************************************************************
* cpssDxChTunnelCtrlTtExceptionCmdGet
*
* DESCRIPTION:
*       Get tunnel termination exception command.
*
* APPLICABLE DEVICES:
*        DxCh2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum        - physical device number
*       exceptionType - tunnel termination exception type to set command for
*
* OUTPUTS:
*       commandPtr    - points to the command for the exception type
*
* RETURNS:
*       GT_OK       - on success.
*       GT_BAD_PTR  - on NULL pointer
*       GT_FAIL     - on error.
*
* COMMENTS:
*       Commands for the different exceptions are:
*
*       CPSS_DXCH_TUNNEL_TERM_IPV4_HEADER_ERROR_E       -
*                               CPSS_PACKET_CMD_TRAP_TO_CPU_E
*                               CPSS_PACKET_CMD_DROP_HARD_E
*
*       CPSS_DXCH_TUNNEL_TERM_IPV4_OPTION_FRAG_ERROR_E  -
*                               CPSS_PACKET_CMD_TRAP_TO_CPU_E
*                               CPSS_PACKET_CMD_DROP_HARD_E
*
*       CPSS_DXCH_TUNNEL_TERM_IPV4_UNSUP_GRE_ERROR_E    -
*                               CPSS_PACKET_CMD_TRAP_TO_CPU_E
*                               CPSS_PACKET_CMD_DROP_HARD_E
*
*
*******************************************************************************/
GT_STATUS cpssDxChTunnelCtrlTtExceptionCmdGet
(
    IN  GT_U8                               devNum,
    IN  CPSS_DXCH_TUNNEL_TERM_EXCEPTION_ENT exceptionType,
    OUT CPSS_PACKET_CMD_ENT                 *commandPtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChTunnelCtrlTtExceptionCmdGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, exceptionType, commandPtr));

    rc = internal_cpssDxChTunnelCtrlTtExceptionCmdGet(devNum, exceptionType, commandPtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, exceptionType, commandPtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChTunnelStartPortIpTunnelTotalLengthOffsetEnableSet
*
* DESCRIPTION:
*       This feature allows overriding the <total length> in the IP header.
*       When the egress port is enabled for this feature, then the new
*       <Total Length> is Old<Total Length> + <IP Tunnel Total Length Offset>.
*
*       This API enables this feature per port.
*
*       For example: when sending Eth-Over-IPv4 to a port connected to MacSec Phy,
*       then total length of the tunnel header need to be increased by 4 bytes
*       because the MacSec adds additional 4 bytes to the passenger packet but
*       is unable to update the tunnel header alone.
*
* APPLICABLE DEVICES:
*        xCat; xCat3; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum        - device number
*       port          - port number
*       enable        - GT_TRUE: Add offset to tunnel total length
*                       GT_FALSE: Don't add offset to tunnel total length
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong value in any of the parameters
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChTunnelStartPortIpTunnelTotalLengthOffsetEnableSet
(
    IN  GT_U8                               devNum,
    IN  GT_PHYSICAL_PORT_NUM                port,
    IN  GT_BOOL                             enable
)
{
    GT_U32      value;      /* value to write */
    GT_STATUS   rc;         /* function return code */
    GT_U32      regAddr;    /* register address */
    GT_U32      bitOffset;  /* offset of nit from start of register */

    /* check parameters */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_LION_E |
         CPSS_XCAT2_E | CPSS_LION2_E );

    if (PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_DXCH_XCAT_E)
    {
        PRV_CPSS_DXCH_XCAT_A3_ONLY_DEV_CHECK_MAC(devNum);
    }

    PRV_CPSS_DXCH_ENHANCED_PHY_PORT_CHECK_MAC(devNum, port);

    /* prepare value to write */
    value = (enable == GT_TRUE) ? 1 : 0;

    if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_ENABLED_MAC(devNum) == GT_TRUE)
    {
        rc = prvCpssDxChWriteTableEntryField(devNum,
                                       PRV_CPSS_DXCH_LION3_TABLE_HA_PHYSICAL_PORT_1_E,
                                       port,
                                       PRV_CPSS_DXCH_TABLE_WORD_INDICATE_FIELD_NAME_CNS,
                                       LION3_HA_PHYSICAL_PORT_TABLE_1_FIELDS_IP_TUNNEL_LENGTH_OFFSET_ENABLE_E, /* field name */
                                       PRV_CPSS_DXCH_TABLES_WORD_INDICATE_AUTO_CALC_LENGTH_CNS,
                                       value);
    }
    else
    {
        /* write register value */
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->haRegs.trunkNumConfigReg[port >> 2];
        bitOffset = (port & 0x3) + 28;
        rc = prvCpssHwPpSetRegField(devNum, regAddr, bitOffset, 1, value);
    }

    return rc;

}

/*******************************************************************************
* cpssDxChTunnelStartPortIpTunnelTotalLengthOffsetEnableSet
*
* DESCRIPTION:
*       This feature allows overriding the <total length> in the IP header.
*       When the egress port is enabled for this feature, then the new
*       <Total Length> is Old<Total Length> + <IP Tunnel Total Length Offset>.
*
*       This API enables this feature per port.
*
*       For example: when sending Eth-Over-IPv4 to a port connected to MacSec Phy,
*       then total length of the tunnel header need to be increased by 4 bytes
*       because the MacSec adds additional 4 bytes to the passenger packet but
*       is unable to update the tunnel header alone.
*
* APPLICABLE DEVICES:
*        xCat; xCat3; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum        - device number
*       port          - port number
*       enable        - GT_TRUE: Add offset to tunnel total length
*                       GT_FALSE: Don't add offset to tunnel total length
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong value in any of the parameters
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChTunnelStartPortIpTunnelTotalLengthOffsetEnableSet
(
    IN  GT_U8                               devNum,
    IN  GT_PHYSICAL_PORT_NUM                port,
    IN  GT_BOOL                             enable
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChTunnelStartPortIpTunnelTotalLengthOffsetEnableSet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, port, enable));

    rc = internal_cpssDxChTunnelStartPortIpTunnelTotalLengthOffsetEnableSet(devNum, port, enable);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, port, enable));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChTunnelStartPortIpTunnelTotalLengthOffsetEnableGet
*
* DESCRIPTION:
*       The function gets status of the feature which allows overriding the
*       <total length> in the IP header.
*       When the egress port is enabled for this feature, then the new
*       <Total Length> is Old<Total Length> + <IP Tunnel Total Length Offset>.
*
*       This API enables this feature per port.
*
*       For example: when sending Eth-Over-IPv4 to a port connected to MacSec Phy,
*       then total length of the tunnel header need to be increased by 4 bytes
*       because the MacSec adds additional 4 bytes to the passenger packet but
*       is unable to update the tunnel header alone.
*
* APPLICABLE DEVICES:
*        xCat; xCat3; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum        - device number
*       port          - port number
*
* OUTPUTS:
*       enablePtr     - points to enable/disable additional offset to tunnel total length
*                       GT_TRUE: Add offset to tunnel total length
*                       GT_FALSE: Don't add offset to tunnel total length
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong value in any of the parameters
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChTunnelStartPortIpTunnelTotalLengthOffsetEnableGet
(
    IN  GT_U8                               devNum,
    IN  GT_PHYSICAL_PORT_NUM                port,
    OUT GT_BOOL                             *enablePtr
)
{
    GT_U32      value;      /* value to write */
    GT_STATUS   rc;         /* function return code */
    GT_U32      regAddr;    /* register address */
    GT_U32      bitOffset;  /* offset of nit from start of register */

    /* check parameters */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_LION_E |
         CPSS_XCAT2_E | CPSS_LION2_E);

    if (PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_DXCH_XCAT_E)
    {
        PRV_CPSS_DXCH_XCAT_A3_ONLY_DEV_CHECK_MAC(devNum);
    }

    PRV_CPSS_DXCH_ENHANCED_PHY_PORT_CHECK_MAC(devNum, port);
    CPSS_NULL_PTR_CHECK_MAC(enablePtr);

    if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_ENABLED_MAC(devNum) == GT_TRUE)
    {
        rc = prvCpssDxChReadTableEntryField(devNum,
                                       PRV_CPSS_DXCH_LION3_TABLE_HA_PHYSICAL_PORT_1_E,
                                       port,
                                       PRV_CPSS_DXCH_TABLE_WORD_INDICATE_FIELD_NAME_CNS,
                                       LION3_HA_PHYSICAL_PORT_TABLE_1_FIELDS_IP_TUNNEL_LENGTH_OFFSET_ENABLE_E, /* field name */
                                       PRV_CPSS_DXCH_TABLES_WORD_INDICATE_AUTO_CALC_LENGTH_CNS,
                                       &value);
    }
    else
    {
        /* read register value */
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->haRegs.trunkNumConfigReg[port >> 2];
        bitOffset = (port & 0x3) + 28;
        rc = prvCpssHwPpGetRegField(devNum, regAddr, bitOffset, 1, &value);
    }

    if (rc != GT_OK)
        return rc;

    /* decode enable/disable state from hardware value */
    *enablePtr = (value != 0) ? GT_TRUE : GT_FALSE;

    return GT_OK;
}

/*******************************************************************************
* cpssDxChTunnelStartPortIpTunnelTotalLengthOffsetEnableGet
*
* DESCRIPTION:
*       The function gets status of the feature which allows overriding the
*       <total length> in the IP header.
*       When the egress port is enabled for this feature, then the new
*       <Total Length> is Old<Total Length> + <IP Tunnel Total Length Offset>.
*
*       This API enables this feature per port.
*
*       For example: when sending Eth-Over-IPv4 to a port connected to MacSec Phy,
*       then total length of the tunnel header need to be increased by 4 bytes
*       because the MacSec adds additional 4 bytes to the passenger packet but
*       is unable to update the tunnel header alone.
*
* APPLICABLE DEVICES:
*        xCat; xCat3; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum        - device number
*       port          - port number
*
* OUTPUTS:
*       enablePtr     - points to enable/disable additional offset to tunnel total length
*                       GT_TRUE: Add offset to tunnel total length
*                       GT_FALSE: Don't add offset to tunnel total length
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong value in any of the parameters
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChTunnelStartPortIpTunnelTotalLengthOffsetEnableGet
(
    IN  GT_U8                               devNum,
    IN  GT_PHYSICAL_PORT_NUM                port,
    OUT GT_BOOL                             *enablePtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChTunnelStartPortIpTunnelTotalLengthOffsetEnableGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, port, enablePtr));

    rc = internal_cpssDxChTunnelStartPortIpTunnelTotalLengthOffsetEnableGet(devNum, port, enablePtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, port, enablePtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}


/*******************************************************************************
* internal_cpssDxChTunnelStartIpTunnelTotalLengthOffsetSet
*
* DESCRIPTION:
*       This API sets the value for <IP Tunnel Total Length Offset>.
*       When the egress port is enabled for this feature, then the new
*       <Total Length> is Old<Total Length> + <IP Tunnel Total Length Offset>.
*
*       For example: when sending Eth-Over-IPv4 to a port connected to MacSec Phy,
*       then total length of the tunnel header need to be increased by 4 bytes
*       because the MacSec adds additional 4 bytes to the passenger packet but
*       is unable to update the tunnel header alone.
*
* APPLICABLE DEVICES:
*        xCat; xCat3; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum            - device number
*       additionToLength  - Ip tunnel total length offset (6 bits)
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong value in any of the parameters
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_OUT_OF_RANGE          - on out of range values
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChTunnelStartIpTunnelTotalLengthOffsetSet
(
    IN  GT_U8                               devNum,
    IN  GT_U32                              additionToLength
)
{
    GT_U32      regAddr;    /* register address */
    GT_STATUS   rc;         /* function return code */

    /* check parameters */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_LION_E |
         CPSS_XCAT2_E | CPSS_LION2_E);

    if (PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_DXCH_XCAT_E)
    {
        PRV_CPSS_DXCH_XCAT_A3_ONLY_DEV_CHECK_MAC(devNum);
    }

    if(additionToLength >= BIT_6)
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_OUT_OF_RANGE, LOG_ERROR_NO_MSG);

    if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_MAC(devNum) == GT_TRUE)
    {
        /* write register value */
        regAddr = PRV_DXCH_REG1_UNIT_HA_MAC(devNum).IPLengthOffset;
        rc = prvCpssHwPpSetRegField(devNum,regAddr,0,6,additionToLength);
    }
    else
    {
        /* write register value */
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->haRegs.hdrAltCpidReg1;
        rc = prvCpssHwPpSetRegField(devNum,regAddr,26,6,additionToLength);
    }

    return rc;
}

/*******************************************************************************
* cpssDxChTunnelStartIpTunnelTotalLengthOffsetSet
*
* DESCRIPTION:
*       This API sets the value for <IP Tunnel Total Length Offset>.
*       When the egress port is enabled for this feature, then the new
*       <Total Length> is Old<Total Length> + <IP Tunnel Total Length Offset>.
*
*       For example: when sending Eth-Over-IPv4 to a port connected to MacSec Phy,
*       then total length of the tunnel header need to be increased by 4 bytes
*       because the MacSec adds additional 4 bytes to the passenger packet but
*       is unable to update the tunnel header alone.
*
* APPLICABLE DEVICES:
*        xCat; xCat3; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum            - device number
*       additionToLength  - Ip tunnel total length offset (6 bits)
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong value in any of the parameters
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_OUT_OF_RANGE          - on out of range values
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChTunnelStartIpTunnelTotalLengthOffsetSet
(
    IN  GT_U8                               devNum,
    IN  GT_U32                              additionToLength
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChTunnelStartIpTunnelTotalLengthOffsetSet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, additionToLength));

    rc = internal_cpssDxChTunnelStartIpTunnelTotalLengthOffsetSet(devNum, additionToLength);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, additionToLength));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChTunnelStartIpTunnelTotalLengthOffsetGet
*
* DESCRIPTION:
*       This API gets the value for <IP Tunnel Total Length Offset>.
*       When the egress port is enabled for this feature, then the new
*       <Total Length> is Old<Total Length> + <IP Tunnel Total Length Offset>.
*
*       For example: when sending Eth-Over-IPv4 to a port connected to MacSec Phy,
*       then total length of the tunnel header need to be increased by 4 bytes
*       because the MacSec adds additional 4 bytes to the passenger packet but
*       is unable to update the tunnel header alone.
*
* APPLICABLE DEVICES:
*        xCat; xCat3; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum               - device number
*
* OUTPUTS:
*       additionToLengthPtr  - (pointer to) Ip tunnel total length offset (6 bits)
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong value in any of the parameters
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChTunnelStartIpTunnelTotalLengthOffsetGet
(
    IN  GT_U8                               devNum,
    OUT GT_U32                              *additionToLengthPtr
)
{
    GT_U32      regAddr;    /* register address */
    GT_STATUS   rc;         /* function return code */

    /* check parameters */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_LION_E |
         CPSS_XCAT2_E | CPSS_LION2_E);

    if (PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_DXCH_XCAT_E)
    {
        PRV_CPSS_DXCH_XCAT_A3_ONLY_DEV_CHECK_MAC(devNum);
    }

    CPSS_NULL_PTR_CHECK_MAC(additionToLengthPtr);

    if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_MAC(devNum) == GT_TRUE)
    {
        /* write register value */
        regAddr = PRV_DXCH_REG1_UNIT_HA_MAC(devNum).IPLengthOffset;
        rc = prvCpssHwPpGetRegField(devNum,regAddr,0,6,additionToLengthPtr);
    }
    else
    {
        /* read register value */
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->haRegs.hdrAltCpidReg1;
        rc = prvCpssHwPpGetRegField(devNum,regAddr,26,6,additionToLengthPtr);
    }

    return rc;
}

/*******************************************************************************
* cpssDxChTunnelStartIpTunnelTotalLengthOffsetGet
*
* DESCRIPTION:
*       This API gets the value for <IP Tunnel Total Length Offset>.
*       When the egress port is enabled for this feature, then the new
*       <Total Length> is Old<Total Length> + <IP Tunnel Total Length Offset>.
*
*       For example: when sending Eth-Over-IPv4 to a port connected to MacSec Phy,
*       then total length of the tunnel header need to be increased by 4 bytes
*       because the MacSec adds additional 4 bytes to the passenger packet but
*       is unable to update the tunnel header alone.
*
* APPLICABLE DEVICES:
*        xCat; xCat3; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum               - device number
*
* OUTPUTS:
*       additionToLengthPtr  - (pointer to) Ip tunnel total length offset (6 bits)
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong value in any of the parameters
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChTunnelStartIpTunnelTotalLengthOffsetGet
(
    IN  GT_U8                               devNum,
    OUT GT_U32                              *additionToLengthPtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChTunnelStartIpTunnelTotalLengthOffsetGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, additionToLengthPtr));

    rc = internal_cpssDxChTunnelStartIpTunnelTotalLengthOffsetGet(devNum, additionToLengthPtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, additionToLengthPtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChTunnelStartMplsPwLabelPushEnableSet
*
* DESCRIPTION:
*       Enable/Disable MPLS PW label push.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum      - device number
*       portNum     - port number
*       enable      - enable/disable
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - wrong device or port.
*       GT_HW_ERROR              - on writing to HW error.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChTunnelStartMplsPwLabelPushEnableSet
(
    IN GT_U8                devNum,
    IN GT_PORT_NUM          portNum,
    IN GT_BOOL              enable
)
{
    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
        CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E |
         CPSS_LION_E | CPSS_XCAT2_E | CPSS_LION2_E);

    PRV_CPSS_DXCH_PORT_CHECK_MAC(devNum,portNum);

    PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_CHECK_MAC(devNum);

    return prvCpssDxChWriteTableEntryField(devNum,
                                   PRV_CPSS_DXCH_LION3_TABLE_HA_EGRESS_EPORT_1_E,
                                   portNum,
                                   PRV_CPSS_DXCH_TABLE_WORD_INDICATE_FIELD_NAME_CNS,
                                   LION3_HA_EPORT_TABLE_1_FIELDS_MPLS_PW_LABEL_PUSH_ENABLE_E, /* field name */
                                   PRV_CPSS_DXCH_TABLES_WORD_INDICATE_AUTO_CALC_LENGTH_CNS,
                                   BOOL2BIT_MAC(enable));
}

/*******************************************************************************
* cpssDxChTunnelStartMplsPwLabelPushEnableSet
*
* DESCRIPTION:
*       Enable/Disable MPLS PW label push.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum      - device number
*       portNum     - port number
*       enable      - enable/disable
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - wrong device or port.
*       GT_HW_ERROR              - on writing to HW error.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChTunnelStartMplsPwLabelPushEnableSet
(
    IN GT_U8                devNum,
    IN GT_PORT_NUM          portNum,
    IN GT_BOOL              enable
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChTunnelStartMplsPwLabelPushEnableSet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, portNum, enable));

    rc = internal_cpssDxChTunnelStartMplsPwLabelPushEnableSet(devNum, portNum, enable);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, portNum, enable));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChTunnelStartMplsPwLabelPushEnableGet
*
* DESCRIPTION:
*       Returns if MPLS PW Label Push is enabled or disabled.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum      - device number
*       portNum     - port number
*
* OUTPUTS:
*       enablePtr   - (pointer to) enable/disable
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - wrong device or port.
*       GT_HW_ERROR              - on writing to HW error.
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChTunnelStartMplsPwLabelPushEnableGet
(
    IN  GT_U8           devNum,
    IN  GT_PORT_NUM     portNum,
    OUT GT_BOOL         *enablePtr
)
{
    GT_U32      hwValue;    /* hardware value */
    GT_STATUS   rc;


    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
        CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E |
         CPSS_LION_E | CPSS_XCAT2_E | CPSS_LION2_E);

    PRV_CPSS_DXCH_PORT_CHECK_MAC(devNum,portNum);
    CPSS_NULL_PTR_CHECK_MAC(enablePtr);

    PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_CHECK_MAC(devNum);

    rc = prvCpssDxChReadTableEntryField(devNum,
                                   PRV_CPSS_DXCH_LION3_TABLE_HA_EGRESS_EPORT_1_E,
                                   portNum,
                                   PRV_CPSS_DXCH_TABLE_WORD_INDICATE_FIELD_NAME_CNS,
                                   LION3_HA_EPORT_TABLE_1_FIELDS_MPLS_PW_LABEL_PUSH_ENABLE_E, /* field name */
                                   PRV_CPSS_DXCH_TABLES_WORD_INDICATE_AUTO_CALC_LENGTH_CNS,
                                   &hwValue);
    if (rc != GT_OK)
    {
        return rc;
    }

   *enablePtr = BIT2BOOL_MAC(hwValue);

    return rc;
}

/*******************************************************************************
* cpssDxChTunnelStartMplsPwLabelPushEnableGet
*
* DESCRIPTION:
*       Returns if MPLS PW Label Push is enabled or disabled.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum      - device number
*       portNum     - port number
*
* OUTPUTS:
*       enablePtr   - (pointer to) enable/disable
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - wrong device or port.
*       GT_HW_ERROR              - on writing to HW error.
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChTunnelStartMplsPwLabelPushEnableGet
(
    IN  GT_U8           devNum,
    IN  GT_PORT_NUM     portNum,
    OUT GT_BOOL         *enablePtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChTunnelStartMplsPwLabelPushEnableGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, portNum, enablePtr));

    rc = internal_cpssDxChTunnelStartMplsPwLabelPushEnableGet(devNum, portNum, enablePtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, portNum, enablePtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChTunnelStartMplsPwLabelSet
*
* DESCRIPTION:
*       Set the MPLS PW label value to push incase that <MPLS PW label push enable> = Enabled
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum      - device number
*       portNum     - port number
*       label       - MPLS label (20 bits)
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - wrong device or port or label.
*       GT_HW_ERROR              - on writing to HW error.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChTunnelStartMplsPwLabelSet
(
    IN GT_U8                devNum,
    IN GT_PORT_NUM          portNum,
    IN GT_U32               label
)
{
    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
        CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E |
         CPSS_LION_E | CPSS_XCAT2_E | CPSS_LION2_E);

    PRV_CPSS_DXCH_PORT_CHECK_MAC(devNum,portNum);
    PRV_TUNNEL_START_MPLS_LABEL_CHECK_MAC(label);

    PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_CHECK_MAC(devNum);

    return prvCpssDxChWriteTableEntryField(devNum,
                                   PRV_CPSS_DXCH_LION3_TABLE_HA_EGRESS_EPORT_1_E,
                                   portNum,
                                   PRV_CPSS_DXCH_TABLE_WORD_INDICATE_FIELD_NAME_CNS,
                                   LION3_HA_EPORT_TABLE_1_FIELDS_MPLS_PW_LABEL_E, /* field name */
                                   PRV_CPSS_DXCH_TABLES_WORD_INDICATE_AUTO_CALC_LENGTH_CNS,
                                   label);
}

/*******************************************************************************
* cpssDxChTunnelStartMplsPwLabelSet
*
* DESCRIPTION:
*       Set the MPLS PW label value to push incase that <MPLS PW label push enable> = Enabled
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum      - device number
*       portNum     - port number
*       label       - MPLS label (20 bits)
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - wrong device or port or label.
*       GT_HW_ERROR              - on writing to HW error.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChTunnelStartMplsPwLabelSet
(
    IN GT_U8                devNum,
    IN GT_PORT_NUM          portNum,
    IN GT_U32               label
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChTunnelStartMplsPwLabelSet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, portNum, label));

    rc = internal_cpssDxChTunnelStartMplsPwLabelSet(devNum, portNum, label);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, portNum, label));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChTunnelStartMplsPwLabelGet
*
* DESCRIPTION:
*       Returns the MPLS PW label value to push incase that <MPLS PW label push enable> = Enabled
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum      - device number
*       portNum     - port number
*
* OUTPUTS:
*       labelPtr    - (pointer to) MPLS label
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - wrong device or port.
*       GT_HW_ERROR              - on writing to HW error.
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChTunnelStartMplsPwLabelGet
(
    IN  GT_U8           devNum,
    IN  GT_PORT_NUM     portNum,
    OUT GT_U32          *labelPtr
)
{
    GT_U32      hwValue;    /* hardware value */
    GT_STATUS   rc;


    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
        CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E |
         CPSS_LION_E | CPSS_XCAT2_E | CPSS_LION2_E);

    PRV_CPSS_DXCH_PORT_CHECK_MAC(devNum,portNum);
    CPSS_NULL_PTR_CHECK_MAC(labelPtr);

    PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_CHECK_MAC(devNum);

    rc = prvCpssDxChReadTableEntryField(devNum,
                                   PRV_CPSS_DXCH_LION3_TABLE_HA_EGRESS_EPORT_1_E,
                                   portNum,
                                   PRV_CPSS_DXCH_TABLE_WORD_INDICATE_FIELD_NAME_CNS,
                                   LION3_HA_EPORT_TABLE_1_FIELDS_MPLS_PW_LABEL_E, /* field name */
                                   PRV_CPSS_DXCH_TABLES_WORD_INDICATE_AUTO_CALC_LENGTH_CNS,
                                   &hwValue);
    if (rc != GT_OK)
    {
        return rc;
    }

   *labelPtr = hwValue;

    return rc;
}

/*******************************************************************************
* cpssDxChTunnelStartMplsPwLabelGet
*
* DESCRIPTION:
*       Returns the MPLS PW label value to push incase that <MPLS PW label push enable> = Enabled
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum      - device number
*       portNum     - port number
*
* OUTPUTS:
*       labelPtr    - (pointer to) MPLS label
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - wrong device or port.
*       GT_HW_ERROR              - on writing to HW error.
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChTunnelStartMplsPwLabelGet
(
    IN  GT_U8           devNum,
    IN  GT_PORT_NUM     portNum,
    OUT GT_U32          *labelPtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChTunnelStartMplsPwLabelGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, portNum, labelPtr));

    rc = internal_cpssDxChTunnelStartMplsPwLabelGet(devNum, portNum, labelPtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, portNum, labelPtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChTunnelStartMplsFlowLabelEnableSet
*
* DESCRIPTION:
*       Enable/Disable MPLS flow label per port.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum      - device number
*       portNum     - port number
*       enable      - enable/disable
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - wrong device or port.
*       GT_HW_ERROR              - on writing to HW error.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChTunnelStartMplsFlowLabelEnableSet
(
    IN GT_U8                devNum,
    IN GT_PORT_NUM          portNum,
    IN GT_BOOL              enable
)
{
    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
        CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E |
         CPSS_LION_E | CPSS_XCAT2_E | CPSS_LION2_E);

    PRV_CPSS_DXCH_PORT_CHECK_MAC(devNum,portNum);

    PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_CHECK_MAC(devNum);

    return prvCpssDxChWriteTableEntryField(devNum,
                                   PRV_CPSS_DXCH_LION3_TABLE_HA_EGRESS_EPORT_1_E,
                                   portNum,
                                   PRV_CPSS_DXCH_TABLE_WORD_INDICATE_FIELD_NAME_CNS,
                                   LION3_HA_EPORT_TABLE_1_FIELDS_PUSH_MPLS_FLOW_LABEL_ENABLE_E, /* field name */
                                   PRV_CPSS_DXCH_TABLES_WORD_INDICATE_AUTO_CALC_LENGTH_CNS,
                                   BOOL2BIT_MAC(enable));
}

/*******************************************************************************
* cpssDxChTunnelStartMplsFlowLabelEnableSet
*
* DESCRIPTION:
*       Enable/Disable MPLS flow label per port.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum      - device number
*       portNum     - port number
*       enable      - enable/disable
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - wrong device or port.
*       GT_HW_ERROR              - on writing to HW error.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChTunnelStartMplsFlowLabelEnableSet
(
    IN GT_U8                devNum,
    IN GT_PORT_NUM          portNum,
    IN GT_BOOL              enable
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChTunnelStartMplsFlowLabelEnableSet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, portNum, enable));

    rc = internal_cpssDxChTunnelStartMplsFlowLabelEnableSet(devNum, portNum, enable);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, portNum, enable));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChTunnelStartMplsFlowLabelEnableGet
*
* DESCRIPTION:
*       Returns if MPLS flow Label is enabled or disabled per port.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum      - device number
*       portNum     - port number
*
* OUTPUTS:
*       enablePtr   - (pointer to) enable/disable
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - wrong device or port.
*       GT_HW_ERROR              - on writing to HW error.
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChTunnelStartMplsFlowLabelEnableGet
(
    IN  GT_U8           devNum,
    IN  GT_PORT_NUM     portNum,
    OUT GT_BOOL         *enablePtr
)
{
    GT_U32      hwValue;    /* hardware value */
    GT_STATUS   rc;


    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
        CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E |
         CPSS_LION_E | CPSS_XCAT2_E | CPSS_LION2_E);

    PRV_CPSS_DXCH_PORT_CHECK_MAC(devNum,portNum);
    CPSS_NULL_PTR_CHECK_MAC(enablePtr);

    PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_CHECK_MAC(devNum);

    rc = prvCpssDxChReadTableEntryField(devNum,
                                   PRV_CPSS_DXCH_LION3_TABLE_HA_EGRESS_EPORT_1_E,
                                   portNum,
                                   PRV_CPSS_DXCH_TABLE_WORD_INDICATE_FIELD_NAME_CNS,
                                   LION3_HA_EPORT_TABLE_1_FIELDS_PUSH_MPLS_FLOW_LABEL_ENABLE_E, /* field name */
                                   PRV_CPSS_DXCH_TABLES_WORD_INDICATE_AUTO_CALC_LENGTH_CNS,
                                   &hwValue);
    if (rc != GT_OK)
    {
        return rc;
    }

   *enablePtr = BIT2BOOL_MAC(hwValue);

    return rc;
}

/*******************************************************************************
* cpssDxChTunnelStartMplsFlowLabelEnableGet
*
* DESCRIPTION:
*       Returns if MPLS flow Label is enabled or disabled per port.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum      - device number
*       portNum     - port number
*
* OUTPUTS:
*       enablePtr   - (pointer to) enable/disable
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - wrong device or port.
*       GT_HW_ERROR              - on writing to HW error.
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChTunnelStartMplsFlowLabelEnableGet
(
    IN  GT_U8           devNum,
    IN  GT_PORT_NUM     portNum,
    OUT GT_BOOL         *enablePtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChTunnelStartMplsFlowLabelEnableGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, portNum, enablePtr));

    rc = internal_cpssDxChTunnelStartMplsFlowLabelEnableGet(devNum, portNum, enablePtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, portNum, enablePtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChTunnelStartMplsFlowLabelTtlSet
*
* DESCRIPTION:
*       Set the MPLS flow label TTL
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum      - device number
*       ttl         - the TTL value
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - wrong device
*       GT_HW_ERROR              - on writing to HW error.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChTunnelStartMplsFlowLabelTtlSet
(
    IN GT_U8                devNum,
    IN GT_U8                ttl
)
{
    GT_U32      regAddr;    /* register address */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
        CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E |
         CPSS_LION_E | CPSS_XCAT2_E | CPSS_LION2_E);

    PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_CHECK_MAC(devNum);

    /* write value */
    regAddr = PRV_DXCH_REG1_UNIT_HA_MAC(devNum).haGlobalConfig1;
    return prvCpssHwPpSetRegField(devNum, regAddr, 1, 8, ttl);
}

/*******************************************************************************
* cpssDxChTunnelStartMplsFlowLabelTtlSet
*
* DESCRIPTION:
*       Set the MPLS flow label TTL
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum      - device number
*       ttl         - the TTL value
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - wrong device
*       GT_HW_ERROR              - on writing to HW error.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChTunnelStartMplsFlowLabelTtlSet
(
    IN GT_U8                devNum,
    IN GT_U8                ttl
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChTunnelStartMplsFlowLabelTtlSet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, ttl));

    rc = internal_cpssDxChTunnelStartMplsFlowLabelTtlSet(devNum, ttl);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, ttl));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChTunnelStartMplsFlowLabelTtlGet
*
* DESCRIPTION:
*       Get the MPLS flow label TTL
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum      - device number
*
* OUTPUTS:
*       ttlPtr      - (pointer to) the TTL value
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - wrong device.
*       GT_HW_ERROR              - on writing to HW error.
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChTunnelStartMplsFlowLabelTtlGet
(
    IN  GT_U8           devNum,
    OUT GT_U8           *ttlPtr
)
{
    GT_U32      regAddr;    /* register address */
    GT_U32      value;      /* field address */
    GT_STATUS   rc;

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
        CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E |
         CPSS_LION_E | CPSS_XCAT2_E | CPSS_LION2_E);

    CPSS_NULL_PTR_CHECK_MAC(ttlPtr);

    PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_CHECK_MAC(devNum);

    /* write value */
    regAddr = PRV_DXCH_REG1_UNIT_HA_MAC(devNum).haGlobalConfig1;
    rc = prvCpssHwPpGetRegField(devNum, regAddr, 1, 8, &value);
    *ttlPtr = (GT_U8)value;
    return rc;
}

/*******************************************************************************
* cpssDxChTunnelStartMplsFlowLabelTtlGet
*
* DESCRIPTION:
*       Get the MPLS flow label TTL
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum      - device number
*
* OUTPUTS:
*       ttlPtr      - (pointer to) the TTL value
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - wrong device.
*       GT_HW_ERROR              - on writing to HW error.
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChTunnelStartMplsFlowLabelTtlGet
(
    IN  GT_U8           devNum,
    OUT GT_U8           *ttlPtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChTunnelStartMplsFlowLabelTtlGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, ttlPtr));

    rc = internal_cpssDxChTunnelStartMplsFlowLabelTtlGet(devNum, ttlPtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, ttlPtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChTunnelStartMplsPwLabelExpSet
*
* DESCRIPTION:
*       Set the EXP value to push to the PW label incase that <MPLS PW label push enable> = Enabled
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum      - device number
*       portNum     - port number
*       exp         - EXP value (APPLICABLE RANGES: 0...7)
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - wrong device or port or exp.
*       GT_HW_ERROR              - on writing to HW error.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChTunnelStartMplsPwLabelExpSet
(
    IN GT_U8                devNum,
    IN GT_PORT_NUM          portNum,
    IN GT_U32               exp
)
{
    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
        CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E |
         CPSS_LION_E | CPSS_XCAT2_E | CPSS_LION2_E);

    PRV_CPSS_DXCH_PORT_CHECK_MAC(devNum,portNum);
    PRV_TUNNEL_START_MPLS_EXP_CHECK_MAC(exp);

    PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_CHECK_MAC(devNum);

    return prvCpssDxChWriteTableEntryField(devNum,
                                   PRV_CPSS_DXCH_LION3_TABLE_HA_EGRESS_EPORT_1_E,
                                   portNum,
                                   PRV_CPSS_DXCH_TABLE_WORD_INDICATE_FIELD_NAME_CNS,
                                   LION3_HA_EPORT_TABLE_1_FIELDS_MPLS_PW_LABEL_EXP_E, /* field name */
                                   PRV_CPSS_DXCH_TABLES_WORD_INDICATE_AUTO_CALC_LENGTH_CNS,
                                   exp);
}

/*******************************************************************************
* cpssDxChTunnelStartMplsPwLabelExpSet
*
* DESCRIPTION:
*       Set the EXP value to push to the PW label incase that <MPLS PW label push enable> = Enabled
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum      - device number
*       portNum     - port number
*       exp         - EXP value (APPLICABLE RANGES: 0...7)
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - wrong device or port or exp.
*       GT_HW_ERROR              - on writing to HW error.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChTunnelStartMplsPwLabelExpSet
(
    IN GT_U8                devNum,
    IN GT_PORT_NUM          portNum,
    IN GT_U32               exp
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChTunnelStartMplsPwLabelExpSet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, portNum, exp));

    rc = internal_cpssDxChTunnelStartMplsPwLabelExpSet(devNum, portNum, exp);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, portNum, exp));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChTunnelStartMplsPwLabelExpGet
*
* DESCRIPTION:
*       Returns the EXP value to push to the PW label incase that <MPLS PW label push enable> = Enabled
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum      - device number
*       portNum     - port number
*
* OUTPUTS:
*       expPtr      - (pointer to) EXP value
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - wrong device or port.
*       GT_HW_ERROR              - on writing to HW error.
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChTunnelStartMplsPwLabelExpGet
(
    IN  GT_U8           devNum,
    IN  GT_PORT_NUM     portNum,
    OUT GT_U32          *expPtr
)
{
    GT_U32      hwValue;    /* hardware value */
    GT_STATUS   rc;


    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
        CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E |
         CPSS_LION_E | CPSS_XCAT2_E | CPSS_LION2_E);

    PRV_CPSS_DXCH_PORT_CHECK_MAC(devNum,portNum);
    CPSS_NULL_PTR_CHECK_MAC(expPtr);

    PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_CHECK_MAC(devNum);

    rc = prvCpssDxChReadTableEntryField(devNum,
                                   PRV_CPSS_DXCH_LION3_TABLE_HA_EGRESS_EPORT_1_E,
                                   portNum,
                                   PRV_CPSS_DXCH_TABLE_WORD_INDICATE_FIELD_NAME_CNS,
                                   LION3_HA_EPORT_TABLE_1_FIELDS_MPLS_PW_LABEL_EXP_E, /* field name */
                                   PRV_CPSS_DXCH_TABLES_WORD_INDICATE_AUTO_CALC_LENGTH_CNS,
                                   &hwValue);
    if (rc != GT_OK)
    {
        return rc;
    }

   *expPtr = hwValue;

    return rc;
}

/*******************************************************************************
* cpssDxChTunnelStartMplsPwLabelExpGet
*
* DESCRIPTION:
*       Returns the EXP value to push to the PW label incase that <MPLS PW label push enable> = Enabled
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum      - device number
*       portNum     - port number
*
* OUTPUTS:
*       expPtr      - (pointer to) EXP value
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - wrong device or port.
*       GT_HW_ERROR              - on writing to HW error.
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChTunnelStartMplsPwLabelExpGet
(
    IN  GT_U8           devNum,
    IN  GT_PORT_NUM     portNum,
    OUT GT_U32          *expPtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChTunnelStartMplsPwLabelExpGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, portNum, expPtr));

    rc = internal_cpssDxChTunnelStartMplsPwLabelExpGet(devNum, portNum, expPtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, portNum, expPtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChTunnelStartMplsPwLabelTtlSet
*
* DESCRIPTION:
*       Set the TTL value to push to the PW label incase that <MPLS PW label push enable> = Enabled
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum      - device number
*       portNum     - port number
*       ttl         - TTL value (APPLICABLE RANGES: 0...255)
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - wrong device or port or ttl.
*       GT_HW_ERROR              - on writing to HW error.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChTunnelStartMplsPwLabelTtlSet
(
    IN GT_U8                devNum,
    IN GT_PORT_NUM          portNum,
    IN GT_U32               ttl
)
{
    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
        CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E |
         CPSS_LION_E | CPSS_XCAT2_E | CPSS_LION2_E);

    PRV_CPSS_DXCH_PORT_CHECK_MAC(devNum,portNum);

    if(ttl > TTL_MAX_CNS)
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);

    PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_CHECK_MAC(devNum);

    return prvCpssDxChWriteTableEntryField(devNum,
                                   PRV_CPSS_DXCH_LION3_TABLE_HA_EGRESS_EPORT_1_E,
                                   portNum,
                                   PRV_CPSS_DXCH_TABLE_WORD_INDICATE_FIELD_NAME_CNS,
                                   LION3_HA_EPORT_TABLE_1_FIELDS_MPLS_PW_LABEL_TTL_E, /* field name */
                                   PRV_CPSS_DXCH_TABLES_WORD_INDICATE_AUTO_CALC_LENGTH_CNS,
                                   ttl);
}

/*******************************************************************************
* cpssDxChTunnelStartMplsPwLabelTtlSet
*
* DESCRIPTION:
*       Set the TTL value to push to the PW label incase that <MPLS PW label push enable> = Enabled
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum      - device number
*       portNum     - port number
*       ttl         - TTL value (APPLICABLE RANGES: 0...255)
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - wrong device or port or ttl.
*       GT_HW_ERROR              - on writing to HW error.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChTunnelStartMplsPwLabelTtlSet
(
    IN GT_U8                devNum,
    IN GT_PORT_NUM          portNum,
    IN GT_U32               ttl
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChTunnelStartMplsPwLabelTtlSet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, portNum, ttl));

    rc = internal_cpssDxChTunnelStartMplsPwLabelTtlSet(devNum, portNum, ttl);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, portNum, ttl));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChTunnelStartMplsPwLabelTtlGet
*
* DESCRIPTION:
*       Returns the TTL value to push to the PW label incase that <MPLS PW label push enable> = Enabled
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum      - device number
*       portNum     - port number
*
* OUTPUTS:
*       ttlPtr      - (pointer to) TTL value
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - wrong device or port.
*       GT_HW_ERROR              - on writing to HW error.
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChTunnelStartMplsPwLabelTtlGet
(
    IN  GT_U8           devNum,
    IN  GT_PORT_NUM     portNum,
    OUT GT_U32          *ttlPtr
)
{
    GT_U32      hwValue;    /* hardware value */
    GT_STATUS   rc;


    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
        CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E |
         CPSS_LION_E | CPSS_XCAT2_E | CPSS_LION2_E);

    PRV_CPSS_DXCH_PORT_CHECK_MAC(devNum,portNum);
    CPSS_NULL_PTR_CHECK_MAC(ttlPtr);

    PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_CHECK_MAC(devNum);

    rc = prvCpssDxChReadTableEntryField(devNum,
                                   PRV_CPSS_DXCH_LION3_TABLE_HA_EGRESS_EPORT_1_E,
                                   portNum,
                                   PRV_CPSS_DXCH_TABLE_WORD_INDICATE_FIELD_NAME_CNS,
                                   LION3_HA_EPORT_TABLE_1_FIELDS_MPLS_PW_LABEL_TTL_E, /* field name */
                                   PRV_CPSS_DXCH_TABLES_WORD_INDICATE_AUTO_CALC_LENGTH_CNS,
                                   &hwValue);
    if (rc != GT_OK)
    {
        return rc;
    }

   *ttlPtr = hwValue;

    return rc;
}

/*******************************************************************************
* cpssDxChTunnelStartMplsPwLabelTtlGet
*
* DESCRIPTION:
*       Returns the TTL value to push to the PW label incase that <MPLS PW label push enable> = Enabled
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum      - device number
*       portNum     - port number
*
* OUTPUTS:
*       ttlPtr      - (pointer to) TTL value
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - wrong device or port.
*       GT_HW_ERROR              - on writing to HW error.
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChTunnelStartMplsPwLabelTtlGet
(
    IN  GT_U8           devNum,
    IN  GT_PORT_NUM     portNum,
    OUT GT_U32          *ttlPtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChTunnelStartMplsPwLabelTtlGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, portNum, ttlPtr));

    rc = internal_cpssDxChTunnelStartMplsPwLabelTtlGet(devNum, portNum, ttlPtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, portNum, ttlPtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChTunnelStartMplsPwControlWordSet
*
* DESCRIPTION:
*       Sets a Pseudo Wire control word
*
* APPLICABLE DEVICES:
*       xCat; xCat3; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum      - device number
*       index       - the PW word index
*                     Bobcat2; Caelum; Bobcat3: (APPLICABLE RANGES: 0..14)
*                     xCat, xCat3: (APPLICABLE RANGES: 0..6)
*       value       - the PW word to write
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - wrong device or illegal PW word index.
*       GT_HW_ERROR              - on writing to HW error.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       xCat device: applicable starting from revision C0
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChTunnelStartMplsPwControlWordSet
(
    IN  GT_U8           devNum,
    IN  GT_U32          index,
    IN  GT_U32          value
)
{
    GT_U32      regAddr;    /* register address */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
        CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E |
        CPSS_LION_E | CPSS_XCAT2_E | CPSS_LION2_E);

    if (PRV_CPSS_DXCH_PP_HW_INFO_VPLS_MODE_SUPPORTED_MAC(devNum)== GT_TRUE)
    {

        if(index > 6)
        {
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
        }

        /* write value to hardware register */
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->haRegs.mplsControlWord[0];

        /* adding the offset of the register                            */
        /* 0x1C000764 + n*4: where n (0-6) represents word for xCat3    */
        /* or                                                           */
        /* 0x0F000764 + n*4: where n (0-6) represents word for xCat C0  */
        /* each register contains 1 entry of 32 bits                    */
        regAddr += index * 4;

        return prvCpssHwPpSetRegField(devNum, regAddr, 0, 32, value);
    }

    PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_CHECK_MAC(devNum);

    if (index >= 15)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    regAddr = PRV_DXCH_REG1_UNIT_HA_MAC(devNum).PWCtrlWord[index];

    return prvCpssHwPpWriteRegister(devNum, regAddr, value);
}

/*******************************************************************************
* cpssDxChTunnelStartMplsPwControlWordSet
*
* DESCRIPTION:
*       Sets a Pseudo Wire control word
*
* APPLICABLE DEVICES:
*       xCat; xCat3; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum      - device number
*       index       - the PW word index
*                     Bobcat2; Caelum; Bobcat3: (APPLICABLE RANGES: 0..14)
*                     xCat, xCat3: (APPLICABLE RANGES: 0..6)
*       value       - the PW word to write
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - wrong device or illegal PW word index.
*       GT_HW_ERROR              - on writing to HW error.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       xCat device: applicable starting from revision C0
*
*******************************************************************************/
GT_STATUS cpssDxChTunnelStartMplsPwControlWordSet
(
    IN  GT_U8           devNum,
    IN  GT_U32          index,
    IN  GT_U32          value
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChTunnelStartMplsPwControlWordSet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, index, value));

    rc = internal_cpssDxChTunnelStartMplsPwControlWordSet(devNum, index, value);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, index, value));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChTunnelStartMplsPwControlWordGet
*
* DESCRIPTION:
*       Gets a Pseudo Wire control word
*
* APPLICABLE DEVICES:
*       xCat; xCat3; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum      - device number
*       index       - the PW word index
*                     Bobcat2; Caelum; Bobcat3: (APPLICABLE RANGES: 0..14)
*                     xCat; xCat3: (APPLICABLE RANGES: 0..6)
*
* OUTPUTS:
*       valuePtr    - (pointer to) the read PW word
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - wrong device or illegal PW word index.
*       GT_HW_ERROR              - on writing to HW error.
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       xCat device: applicable starting from revision C0
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChTunnelStartMplsPwControlWordGet
(
    IN  GT_U8           devNum,
    IN  GT_U32          index,
    OUT GT_U32          *valuePtr
)
{
    GT_U32      regAddr;    /* register address */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
        CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E |
        CPSS_LION_E | CPSS_XCAT2_E | CPSS_LION2_E);

    CPSS_NULL_PTR_CHECK_MAC(valuePtr);

    if (PRV_CPSS_DXCH_PP_HW_INFO_VPLS_MODE_SUPPORTED_MAC(devNum)== GT_TRUE)
    {
        if(index > 6)
        {
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
        }

        /* read value from hardware register */
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->haRegs.mplsControlWord[0];

        /* adding the offset of the register                            */
        /* 0x1C000764 + n*4: where n (0-6) represents word for xCat3    */
        /* or                                                           */
        /* 0x0F000764 + n*4: where n (0-6) represents word for xCat C0  */
        /* each register contains 1 entry of 32 bits                    */
        regAddr += index * 4;

        return prvCpssHwPpGetRegField(devNum, regAddr, 0, 32, valuePtr);

    }

    PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_CHECK_MAC(devNum);

    if (index >= 15)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    regAddr = PRV_DXCH_REG1_UNIT_HA_MAC(devNum).PWCtrlWord[index];

    return prvCpssHwPpReadRegister(devNum, regAddr, valuePtr);
}

/*******************************************************************************
* cpssDxChTunnelStartMplsPwControlWordGet
*
* DESCRIPTION:
*       Gets a Pseudo Wire control word
*
* APPLICABLE DEVICES:
*       xCat; xCat3; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum      - device number
*       index       - the PW word index
*                     Bobcat2; Caelum; Bobcat3: (APPLICABLE RANGES: 0..14)
*                     xCat; xCat3: (APPLICABLE RANGES: 0..6)
*
* OUTPUTS:
*       valuePtr    - (pointer to) the read PW word
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - wrong device or illegal PW word index.
*       GT_HW_ERROR              - on writing to HW error.
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       xCat device: applicable starting from revision C0
*
*******************************************************************************/
GT_STATUS cpssDxChTunnelStartMplsPwControlWordGet
(
    IN  GT_U8           devNum,
    IN  GT_U32          index,
    OUT GT_U32          *valuePtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChTunnelStartMplsPwControlWordGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, index, valuePtr));

    rc = internal_cpssDxChTunnelStartMplsPwControlWordGet(devNum, index, valuePtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, index, valuePtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChTunnelStartMplsPwETreeEnableSet
*
* DESCRIPTION:
*       Enable/Disable E-TREE assignment for a specified PW Control Word.
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum      - device number
*       index       - the PW word index (APPLICABLE RANGES: 0...14)
*       enable      - enable/disable the E-TREE assignment
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - wrong device or illegal PW word index.
*       GT_HW_ERROR              - on writing to HW error.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChTunnelStartMplsPwETreeEnableSet
(
    IN  GT_U8           devNum,
    IN  GT_U32          index,
    IN  GT_BOOL         enable
)
{
    GT_U32    regAddr;           /* register address */
    GT_U32    offset;            /* the offset of the field */
    GT_U32    value;             /* the value to write */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
        CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E |
         CPSS_LION_E | CPSS_XCAT2_E | CPSS_LION2_E);

    PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_CHECK_MAC(devNum);

    if (index >= 15)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    offset = index+1;
    regAddr = PRV_DXCH_REG1_UNIT_HA_MAC(devNum).PWETreeEnable;
    value = (enable == GT_TRUE) ? 1 : 0;

    return prvCpssHwPpSetRegField(devNum, regAddr, offset , 1, value);
}

/*******************************************************************************
* cpssDxChTunnelStartMplsPwETreeEnableSet
*
* DESCRIPTION:
*       Enable/Disable E-TREE assignment for a specified PW Control Word.
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum      - device number
*       index       - the PW word index (APPLICABLE RANGES: 0...14)
*       enable      - enable/disable the E-TREE assignment
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - wrong device or illegal PW word index.
*       GT_HW_ERROR              - on writing to HW error.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChTunnelStartMplsPwETreeEnableSet
(
    IN  GT_U8           devNum,
    IN  GT_U32          index,
    IN  GT_BOOL         enable
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChTunnelStartMplsPwETreeEnableSet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, index, enable));

    rc = internal_cpssDxChTunnelStartMplsPwETreeEnableSet(devNum, index, enable);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, index, enable));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChTunnelStartMplsPwETreeEnableGet
*
* DESCRIPTION:
*       Return whether E-TREE assignment is enabled for a specified PW Control
*       Word.
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum      - device number
*       index       - the PW word index (APPLICABLE RANGES: 0...14)
*
* OUTPUTS:
*       enablePtr   - (pointer to) the E-TREE assignment enabling status
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - wrong device or illegal PW word index.
*       GT_HW_ERROR              - on writing to HW error.
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChTunnelStartMplsPwETreeEnableGet
(
    IN  GT_U8           devNum,
    IN  GT_U32          index,
    OUT GT_BOOL         *enablePtr
)
{
    GT_U32    regAddr;           /* register address */
    GT_U32    offset;            /* the offset of the field */
    GT_U32    value;             /* the read value */
    GT_STATUS rc;

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
        CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E |
         CPSS_LION_E | CPSS_XCAT2_E | CPSS_LION2_E);

    CPSS_NULL_PTR_CHECK_MAC(enablePtr);

    PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_CHECK_MAC(devNum);

    if (index >= 15)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    offset = index+1;
    regAddr = PRV_DXCH_REG1_UNIT_HA_MAC(devNum).PWETreeEnable;

    rc = prvCpssHwPpGetRegField(devNum, regAddr, offset, 1, &value);
    if (rc != GT_OK)
    {
        return rc;
    }

    /* decode enable/disable state from hardware value */
    *enablePtr = (value != 0) ? GT_TRUE : GT_FALSE;

    return GT_OK;
}

/*******************************************************************************
* cpssDxChTunnelStartMplsPwETreeEnableGet
*
* DESCRIPTION:
*       Return whether E-TREE assignment is enabled for a specified PW Control
*       Word.
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum      - device number
*       index       - the PW word index (APPLICABLE RANGES: 0...14)
*
* OUTPUTS:
*       enablePtr   - (pointer to) the E-TREE assignment enabling status
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - wrong device or illegal PW word index.
*       GT_HW_ERROR              - on writing to HW error.
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChTunnelStartMplsPwETreeEnableGet
(
    IN  GT_U8           devNum,
    IN  GT_U32          index,
    OUT GT_BOOL         *enablePtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChTunnelStartMplsPwETreeEnableGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, index, enablePtr));

    rc = internal_cpssDxChTunnelStartMplsPwETreeEnableGet(devNum, index, enablePtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, index, enablePtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChTunnelStartHeaderTpidSelectSet
*
* DESCRIPTION:
*       Function sets index of TPID tunnel-start header
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum         - device number
*       portNum        - port number (CPU port is supported)
*       tpidEntryIndex - TPID table entry index (APPLICABLE RANGES: 0...7)
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error.
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong devNum, portNum, ethMode
*       GT_OUT_OF_RANGE          - illegal tpidEntryIndex (not 0-7)
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       Relevant only when a TS entry is processed (from ingress
*       pipe assignment or from egress ePort assignment).
*       Applicable to all TS types
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChTunnelStartHeaderTpidSelectSet
(
    IN  GT_U8                devNum,
    IN  GT_PORT_NUM          portNum,
    IN  GT_U32               tpidEntryIndex
)
{
    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
          CPSS_CH1_E| CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E |
          CPSS_LION_E | CPSS_XCAT2_E | CPSS_LION2_E);

    PRV_CPSS_DXCH_PORT_CHECK_MAC(devNum,portNum);

    PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_CHECK_MAC(devNum);

    if (tpidEntryIndex >= 8)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_OUT_OF_RANGE, LOG_ERROR_NO_MSG);
    }

    return prvCpssDxChWriteTableEntryField(devNum,
                                           PRV_CPSS_DXCH_LION3_TABLE_HA_EGRESS_EPORT_1_E,
                                           portNum,
                                           PRV_CPSS_DXCH_TABLE_WORD_INDICATE_FIELD_NAME_CNS,
                                           LION3_HA_EPORT_TABLE_1_FIELDS_TS_HEADER_TPID_INDEX_E, /* field name */
                                           PRV_CPSS_DXCH_TABLES_WORD_INDICATE_AUTO_CALC_LENGTH_CNS,
                                           tpidEntryIndex);
}

/*******************************************************************************
* cpssDxChTunnelStartHeaderTpidSelectSet
*
* DESCRIPTION:
*       Function sets index of TPID tunnel-start header
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum         - device number
*       portNum        - port number (CPU port is supported)
*       tpidEntryIndex - TPID table entry index (APPLICABLE RANGES: 0...7)
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error.
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong devNum, portNum, ethMode
*       GT_OUT_OF_RANGE          - illegal tpidEntryIndex (not 0-7)
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       Relevant only when a TS entry is processed (from ingress
*       pipe assignment or from egress ePort assignment).
*       Applicable to all TS types
*
*******************************************************************************/
GT_STATUS cpssDxChTunnelStartHeaderTpidSelectSet
(
    IN  GT_U8                devNum,
    IN  GT_PORT_NUM          portNum,
    IN  GT_U32               tpidEntryIndex
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChTunnelStartHeaderTpidSelectSet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, portNum, tpidEntryIndex));

    rc = internal_cpssDxChTunnelStartHeaderTpidSelectSet(devNum, portNum, tpidEntryIndex);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, portNum, tpidEntryIndex));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChTunnelStartHeaderTpidSelectGet
*
* DESCRIPTION:
*       Function gets index of TPID tunnel-start header
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum     - device number
*       portNum    - port number (CPU port is supported)
*
* OUTPUTS:
*       tpidEntryIndexPtr - (pointer to) TPID table entry index
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error.
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong devNum, portNum, ethMode
*       GT_BAD_PTR               - tpidEntryIndexPtr is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       Relevant only when a TS entry is processed (from ingress
*       pipe assignment or from egress ePort assignment).
*       Applicable to all TS types
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChTunnelStartHeaderTpidSelectGet
(
    IN  GT_U8                devNum,
    IN  GT_PORT_NUM          portNum,
    OUT GT_U32               *tpidEntryIndexPtr
)
{
    GT_STATUS rc = GT_OK;
    GT_U32  hwValue;

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
          CPSS_CH1_E| CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E |
          CPSS_LION_E | CPSS_XCAT2_E | CPSS_LION2_E);

    CPSS_NULL_PTR_CHECK_MAC(tpidEntryIndexPtr);

    PRV_CPSS_DXCH_PORT_CHECK_MAC(devNum,portNum);

    PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_CHECK_MAC(devNum);


    rc = prvCpssDxChReadTableEntryField(devNum,
                                           PRV_CPSS_DXCH_LION3_TABLE_HA_EGRESS_EPORT_1_E,
                                               portNum,
                                           PRV_CPSS_DXCH_TABLE_WORD_INDICATE_FIELD_NAME_CNS,
                                           LION3_HA_EPORT_TABLE_1_FIELDS_TS_HEADER_TPID_INDEX_E, /* field name */
                                           PRV_CPSS_DXCH_TABLES_WORD_INDICATE_AUTO_CALC_LENGTH_CNS,
                                               &hwValue);
    if(rc != GT_OK)
        return rc;

    *tpidEntryIndexPtr = hwValue;

    return rc;
}

/*******************************************************************************
* cpssDxChTunnelStartHeaderTpidSelectGet
*
* DESCRIPTION:
*       Function gets index of TPID tunnel-start header
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum     - device number
*       portNum    - port number (CPU port is supported)
*
* OUTPUTS:
*       tpidEntryIndexPtr - (pointer to) TPID table entry index
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error.
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong devNum, portNum, ethMode
*       GT_BAD_PTR               - tpidEntryIndexPtr is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       Relevant only when a TS entry is processed (from ingress
*       pipe assignment or from egress ePort assignment).
*       Applicable to all TS types
*
*******************************************************************************/
GT_STATUS cpssDxChTunnelStartHeaderTpidSelectGet
(
    IN  GT_U8                devNum,
    IN  GT_PORT_NUM          portNum,
    OUT GT_U32               *tpidEntryIndexPtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChTunnelStartHeaderTpidSelectGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, portNum, tpidEntryIndexPtr));

    rc = internal_cpssDxChTunnelStartHeaderTpidSelectGet(devNum, portNum, tpidEntryIndexPtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, portNum, tpidEntryIndexPtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChTunnelStartEntryExtensionSet
*
* DESCRIPTION:
*       Set tunnel start entry extension value.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum      - device number
*       portNum     - port number
*       tsExtension - tunnel start entry extension value
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - wrong device or port.
*       GT_HW_ERROR              - on writing to HW error.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChTunnelStartEntryExtensionSet
(
    IN GT_U8                devNum,
    IN GT_PORT_NUM          portNum,
    IN GT_U32               tsExtension
)
{
    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
        CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E |
         CPSS_LION_E | CPSS_XCAT2_E | CPSS_LION2_E);

    PRV_CPSS_DXCH_PORT_CHECK_MAC(devNum,portNum);


    PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_CHECK_MAC(devNum);

    return prvCpssDxChWriteTableEntryField(devNum,
                                   PRV_CPSS_DXCH_LION3_TABLE_HA_EGRESS_EPORT_1_E,
                                   portNum,
                                   PRV_CPSS_DXCH_TABLE_WORD_INDICATE_FIELD_NAME_CNS,
                                   LION3_HA_EPORT_TABLE_1_FIELDS_TS_EXT_E, /* field name */
                                   PRV_CPSS_DXCH_TABLES_WORD_INDICATE_AUTO_CALC_LENGTH_CNS,
                                   tsExtension);
}

/*******************************************************************************
* cpssDxChTunnelStartEntryExtensionSet
*
* DESCRIPTION:
*       Set tunnel start entry extension value.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum      - device number
*       portNum     - port number
*       tsExtension - tunnel start entry extension value
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - wrong device or port.
*       GT_HW_ERROR              - on writing to HW error.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChTunnelStartEntryExtensionSet
(
    IN GT_U8                devNum,
    IN GT_PORT_NUM          portNum,
    IN GT_U32               tsExtension
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChTunnelStartEntryExtensionSet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, portNum, tsExtension));

    rc = internal_cpssDxChTunnelStartEntryExtensionSet(devNum, portNum, tsExtension);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, portNum, tsExtension));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChTunnelStartEntryExtensionGet
*
* DESCRIPTION:
*       Get tunnel start entry extension value.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum      - device number
*       portNum     - port number
*
* OUTPUTS:
*       tsExtensionPtr  - (pointer to) tunnel start entry extension value
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - wrong device or port.
*       GT_HW_ERROR              - on writing to HW error.
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChTunnelStartEntryExtensionGet
(
    IN  GT_U8           devNum,
    IN  GT_PORT_NUM     portNum,
    OUT GT_U32          *tsExtensionPtr
)
{
    GT_U32      hwValue;    /* hardware value */
    GT_STATUS   rc;


    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
        CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E |
         CPSS_LION_E | CPSS_XCAT2_E | CPSS_LION2_E);

    PRV_CPSS_DXCH_PORT_CHECK_MAC(devNum,portNum);
    CPSS_NULL_PTR_CHECK_MAC(tsExtensionPtr);

    PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_CHECK_MAC(devNum);

    rc = prvCpssDxChReadTableEntryField(devNum,
                                   PRV_CPSS_DXCH_LION3_TABLE_HA_EGRESS_EPORT_1_E,
                                   portNum,
                                   PRV_CPSS_DXCH_TABLE_WORD_INDICATE_FIELD_NAME_CNS,
                                   LION3_HA_EPORT_TABLE_1_FIELDS_TS_EXT_E, /* field name */
                                   PRV_CPSS_DXCH_TABLES_WORD_INDICATE_AUTO_CALC_LENGTH_CNS,
                                   &hwValue);
    if (rc != GT_OK)
    {
        return rc;
    }

   *tsExtensionPtr = hwValue;

    return rc;
}

/*******************************************************************************
* cpssDxChTunnelStartEntryExtensionGet
*
* DESCRIPTION:
*       Get tunnel start entry extension value.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum      - device number
*       portNum     - port number
*
* OUTPUTS:
*       tsExtensionPtr  - (pointer to) tunnel start entry extension value
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - wrong device or port.
*       GT_HW_ERROR              - on writing to HW error.
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChTunnelStartEntryExtensionGet
(
    IN  GT_U8           devNum,
    IN  GT_PORT_NUM     portNum,
    OUT GT_U32          *tsExtensionPtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChTunnelStartEntryExtensionGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, portNum, tsExtensionPtr));

    rc = internal_cpssDxChTunnelStartEntryExtensionGet(devNum, portNum, tsExtensionPtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, portNum, tsExtensionPtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChTunnelStartEgessVlanTableServiceIdSet
*
* DESCRIPTION:
*       Set egress vlan table service Id.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum        - device number
*       vlanId        - VLAN id, used as index in the Egress Vlan Translation Table.
*                       (APPLICABLE RANGES:0..8191).
*       vlanServiceId - tunnel start entry extension value (APPLICABLE RANGES: 0...FFFFFF)
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - wrong device or port.
*       GT_HW_ERROR              - on writing to HW error.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChTunnelStartEgessVlanTableServiceIdSet
(
    IN GT_U8                devNum,
    IN GT_U16               vlanId,
    IN GT_U32               vlanServiceId
)
{
    PRV_CPSS_DXCH_TABLE_ENT     tableType = PRV_CPSS_DXCH3_TABLE_EGRESS_VLAN_TRANSLATION_E;
    GT_U32                      numOfBits = 24;/*number of bit to write*/

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
        CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E |
         CPSS_LION_E | CPSS_XCAT2_E | CPSS_LION2_E);
    PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_CHECK_MAC(devNum);

    PRV_CPSS_DXCH_VLAN_INDEX_CHECK_MAC(devNum, vlanId);

    PRV_TUNNEL_START_VLAN_SERVICE_ID_CHECK_MAC(vlanServiceId);
    return prvCpssDxChWriteTableEntryField(devNum,
                                           tableType,
                                           vlanId,
                                           PRV_CPSS_DXCH_TABLE_WORD_INDICATE_GLOBAL_BIT_CNS,   /* word num */
                                           12,   /* start bit */
                                           numOfBits,
                                           vlanServiceId);
}

/*******************************************************************************
* cpssDxChTunnelStartEgessVlanTableServiceIdSet
*
* DESCRIPTION:
*       Set egress vlan table service Id.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum        - device number
*       vlanId        - VLAN id, used as index in the Egress Vlan Translation Table.
*                       (APPLICABLE RANGES:0..8191).
*       vlanServiceId - tunnel start entry extension value (APPLICABLE RANGES: 0...FFFFFF)
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - wrong device or port.
*       GT_HW_ERROR              - on writing to HW error.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChTunnelStartEgessVlanTableServiceIdSet
(
    IN GT_U8                devNum,
    IN GT_U16               vlanId,
    IN GT_U32               vlanServiceId
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChTunnelStartEgessVlanTableServiceIdSet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, vlanId, vlanServiceId));

    rc = internal_cpssDxChTunnelStartEgessVlanTableServiceIdSet(devNum, vlanId, vlanServiceId);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, vlanId, vlanServiceId));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}


/*******************************************************************************
* internal_cpssDxChTunnelStartEgessVlanTableServiceIdGet
*
* DESCRIPTION:
*       Get egress vlan table service Id.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum        - device number
*       vlanId        - VLAN id, used as index in the Egress Vlan Translation Table.
*                       (APPLICABLE RANGES:0..8191).
* OUTPUTS:
*       vlanServiceIdPtr  - (pointer to) vlan service Id.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - wrong device or port.
*       GT_HW_ERROR              - on writing to HW error.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChTunnelStartEgessVlanTableServiceIdGet
(
    IN  GT_U8                devNum,
    IN  GT_U16               vlanId,
    OUT GT_U32               *vlanServiceIdPtr
)
{
    PRV_CPSS_DXCH_TABLE_ENT     tableType = PRV_CPSS_DXCH3_TABLE_EGRESS_VLAN_TRANSLATION_E;
    GT_U32                      numOfBits = 24;/*number of bit to write*/
    GT_STATUS                   rc;
    GT_U32                      hwValue;

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
        CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E |
         CPSS_LION_E | CPSS_XCAT2_E | CPSS_LION2_E);
    PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_CHECK_MAC(devNum);

    PRV_CPSS_DXCH_VLAN_INDEX_CHECK_MAC(devNum, vlanId);

    CPSS_NULL_PTR_CHECK_MAC(vlanServiceIdPtr);

    rc = prvCpssDxChReadTableEntryField(devNum,
                                        tableType,
                                        vlanId,
                                        PRV_CPSS_DXCH_TABLE_WORD_INDICATE_GLOBAL_BIT_CNS,   /* word num*/
                                        12,   /* start bit */
                                        numOfBits,
                                        &hwValue);

    *vlanServiceIdPtr = hwValue & 0xFFFFFF;

    return rc;
}

/*******************************************************************************
* cpssDxChTunnelStartEgessVlanTableServiceIdGet
*
* DESCRIPTION:
*       Get egress vlan table service Id.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum        - device number
*       vlanId        - VLAN id, used as index in the Egress Vlan Translation Table.
*                       (APPLICABLE RANGES:0..8191).
* OUTPUTS:
*       vlanServiceIdPtr  - (pointer to) vlan service Id.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - wrong device or port.
*       GT_HW_ERROR              - on writing to HW error.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChTunnelStartEgessVlanTableServiceIdGet
(
    IN  GT_U8                devNum,
    IN  GT_U16               vlanId,
    OUT GT_U32               *vlanServiceIdPtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChTunnelStartEgessVlanTableServiceIdGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, vlanId, vlanServiceIdPtr));

    rc = internal_cpssDxChTunnelStartEgessVlanTableServiceIdGet(devNum, vlanId, vlanServiceIdPtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, vlanId, vlanServiceIdPtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}
