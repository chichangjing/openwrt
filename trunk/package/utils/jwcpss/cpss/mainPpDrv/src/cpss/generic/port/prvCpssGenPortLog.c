/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* prvCpssGenPortLog.c
*       WARNING!!! this is a generated file, please don't edit it manually
* COMMENTS:
*
* FILE REVISION NUMBER:
*       $Revision: 1 $
*******************************************************************************/
#include <cpss/generic/log/cpssLog.h>
#include <cpss/generic/log/prvCpssLog.h>
#include <cpss/extServices/private/prvCpssBindFunc.h>
#include <cpss/generic/log/prvCpssGenCommonTypesLog.h>
#include <cpss/generic/port/cpssPortCtrl.h>
#include <cpss/generic/port/cpssPortStat.h>
#include <cpss/generic/port/cpssPortTx.h>
#include <cpss/generic/port/private/prvCpssGenPortLog.h>


/********* enums *********/

char * prvCpssLogEnum_CPSS_PORT_DIRECTION_ENT[]  =
{
    "CPSS_PORT_DIRECTION_RX_E",
    "CPSS_PORT_DIRECTION_TX_E",
    "CPSS_PORT_DIRECTION_BOTH_E"
};
PRV_CPSS_LOG_STC_ENUM_ARRAY_SIZE_MAC(CPSS_PORT_DIRECTION_ENT);
char * prvCpssLogEnum_CPSS_PORT_DUPLEX_ENT[]  =
{
    "CPSS_PORT_FULL_DUPLEX_E",
    "CPSS_PORT_HALF_DUPLEX_E"
};
PRV_CPSS_LOG_STC_ENUM_ARRAY_SIZE_MAC(CPSS_PORT_DUPLEX_ENT);
PRV_CPSS_ENUM_STRING_VALUE_PAIR_STC prvCpssLogEnum_map_CPSS_PORT_EGRESS_CNT_MODE_ENT[]  =
{PRV_CPSS_LOG_ENUM_NAME_AND_VALUE_MAC(CPSS_EGRESS_CNT_PORT_E),
PRV_CPSS_LOG_ENUM_NAME_AND_VALUE_MAC(CPSS_EGRESS_CNT_VLAN_E),
PRV_CPSS_LOG_ENUM_NAME_AND_VALUE_MAC(CPSS_EGRESS_CNT_TC_E),
PRV_CPSS_LOG_ENUM_NAME_AND_VALUE_MAC(CPSS_EGRESS_CNT_DP_E)
};
PRV_CPSS_LOG_STC_ENUM_MAP_ARRAY_SIZE_MAC(CPSS_PORT_EGRESS_CNT_MODE_ENT);
char * prvCpssLogEnum_CPSS_PORT_FLOW_CONTROL_ENT[]  =
{
    "CPSS_PORT_FLOW_CONTROL_DISABLE_E",
    "CPSS_PORT_FLOW_CONTROL_RX_TX_E",
    "CPSS_PORT_FLOW_CONTROL_RX_ONLY_E",
    "CPSS_PORT_FLOW_CONTROL_TX_ONLY_E"
};
PRV_CPSS_LOG_STC_ENUM_ARRAY_SIZE_MAC(CPSS_PORT_FLOW_CONTROL_ENT);
PRV_CPSS_ENUM_STRING_VALUE_PAIR_STC prvCpssLogEnum_map_CPSS_PORT_INTERFACE_MODE_ENT[]  =
{PRV_CPSS_LOG_ENUM_NAME_AND_VALUE_MAC(CPSS_PORT_INTERFACE_MODE_REDUCED_10BIT_E),
PRV_CPSS_LOG_ENUM_NAME_AND_VALUE_MAC(CPSS_PORT_INTERFACE_MODE_REDUCED_GMII_E),
PRV_CPSS_LOG_ENUM_NAME_AND_VALUE_MAC(CPSS_PORT_INTERFACE_MODE_MII_E),
PRV_CPSS_LOG_ENUM_NAME_AND_VALUE_MAC(CPSS_PORT_INTERFACE_MODE_SGMII_E),
PRV_CPSS_LOG_ENUM_NAME_AND_VALUE_MAC(CPSS_PORT_INTERFACE_MODE_XGMII_E),
PRV_CPSS_LOG_ENUM_NAME_AND_VALUE_MAC(CPSS_PORT_INTERFACE_MODE_MGMII_E),
PRV_CPSS_LOG_ENUM_NAME_AND_VALUE_MAC(CPSS_PORT_INTERFACE_MODE_1000BASE_X_E),
PRV_CPSS_LOG_ENUM_NAME_AND_VALUE_MAC(CPSS_PORT_INTERFACE_MODE_GMII_E),
PRV_CPSS_LOG_ENUM_NAME_AND_VALUE_MAC(CPSS_PORT_INTERFACE_MODE_MII_PHY_E),
PRV_CPSS_LOG_ENUM_NAME_AND_VALUE_MAC(CPSS_PORT_INTERFACE_MODE_QX_E),
PRV_CPSS_LOG_ENUM_NAME_AND_VALUE_MAC(CPSS_PORT_INTERFACE_MODE_HX_E),
PRV_CPSS_LOG_ENUM_NAME_AND_VALUE_MAC(CPSS_PORT_INTERFACE_MODE_RXAUI_E),
PRV_CPSS_LOG_ENUM_NAME_AND_VALUE_MAC(CPSS_PORT_INTERFACE_MODE_100BASE_FX_E),
PRV_CPSS_LOG_ENUM_NAME_AND_VALUE_MAC(CPSS_PORT_INTERFACE_MODE_QSGMII_E),
PRV_CPSS_LOG_ENUM_NAME_AND_VALUE_MAC(CPSS_PORT_INTERFACE_MODE_XLG_E),
PRV_CPSS_LOG_ENUM_NAME_AND_VALUE_MAC(CPSS_PORT_INTERFACE_MODE_LOCAL_XGMII_E),
PRV_CPSS_LOG_ENUM_NAME_AND_VALUE_MAC(CPSS_PORT_INTERFACE_MODE_NO_SERDES_PORT_E),
PRV_CPSS_LOG_ENUM_NAME_AND_VALUE_MAC(CPSS_PORT_INTERFACE_MODE_KR_E),
PRV_CPSS_LOG_ENUM_NAME_AND_VALUE_MAC(CPSS_PORT_INTERFACE_MODE_HGL_E),
PRV_CPSS_LOG_ENUM_NAME_AND_VALUE_MAC(CPSS_PORT_INTERFACE_MODE_CHGL_12_E),
PRV_CPSS_LOG_ENUM_NAME_AND_VALUE_MAC(CPSS_PORT_INTERFACE_MODE_ILKN12_E),
PRV_CPSS_LOG_ENUM_NAME_AND_VALUE_MAC(CPSS_PORT_INTERFACE_MODE_SR_LR_E),
PRV_CPSS_LOG_ENUM_NAME_AND_VALUE_MAC(CPSS_PORT_INTERFACE_MODE_ILKN16_E),
PRV_CPSS_LOG_ENUM_NAME_AND_VALUE_MAC(CPSS_PORT_INTERFACE_MODE_ILKN24_E),
PRV_CPSS_LOG_ENUM_NAME_AND_VALUE_MAC(CPSS_PORT_INTERFACE_MODE_ILKN4_E),
PRV_CPSS_LOG_ENUM_NAME_AND_VALUE_MAC(CPSS_PORT_INTERFACE_MODE_ILKN8_E),
PRV_CPSS_LOG_ENUM_NAME_AND_VALUE_MAC(CPSS_PORT_INTERFACE_MODE_XHGS_E),
PRV_CPSS_LOG_ENUM_NAME_AND_VALUE_MAC(CPSS_PORT_INTERFACE_MODE_XHGS_SR_E),
PRV_CPSS_LOG_ENUM_NAME_AND_VALUE_MAC(CPSS_PORT_INTERFACE_MODE_NA_E)
};
PRV_CPSS_LOG_STC_ENUM_MAP_ARRAY_SIZE_MAC(CPSS_PORT_INTERFACE_MODE_ENT);
char * prvCpssLogEnum_CPSS_PORT_INTERLAKEN_COUNTER_ID_ENT[]  =
{
    "CPSS_PORT_INTERLAKEN_COUNTER_ID_RX_BYTE_E",
    "CPSS_PORT_INTERLAKEN_COUNTER_ID_RX_BAD_PKT_E",
    "CPSS_PORT_INTERLAKEN_COUNTER_ID_RX_PKT_E",
    "CPSS_PORT_INTERLAKEN_COUNTER_ID_RX_CRC_ERR_E",
    "CPSS_PORT_INTERLAKEN_COUNTER_ID_TX_BYTE_E",
    "CPSS_PORT_INTERLAKEN_COUNTER_ID_TX_BAD_PKT_E",
    "CPSS_PORT_INTERLAKEN_COUNTER_ID_TX_PKT_E",
    "CPSS_PORT_INTERLAKEN_COUNTER_ID_BLK_TYPE_ERR_E",
    "CPSS_PORT_INTERLAKEN_COUNTER_ID_DIAG_CRC_ERR_E",
    "CPSS_PORT_INTERLAKEN_COUNTER_ID_WORD_SYNC_ERR_E",
    "CPSS_PORT_INTERLAKEN_COUNTER_ID_RX_ALIGNMENT_ERR_E",
    "CPSS_PORT_INTERLAKEN_COUNTER_ID_RX_ALIGNMENT_FAIL_E",
    "CPSS_PORT_INTERLAKEN_COUNTER_ID_RX_BURST_SIZE_ERR_E"
};
PRV_CPSS_LOG_STC_ENUM_ARRAY_SIZE_MAC(CPSS_PORT_INTERLAKEN_COUNTER_ID_ENT);
char * prvCpssLogEnum_CPSS_PORT_MAC_COUNTERS_ENT[]  =
{
    "CPSS_GOOD_OCTETS_RCV_E",
    "CPSS_BAD_OCTETS_RCV_E",
    "CPSS_MAC_TRANSMIT_ERR_E",
    "CPSS_GOOD_PKTS_RCV_E",
    "CPSS_BAD_PKTS_RCV_E",
    "CPSS_BRDC_PKTS_RCV_E",
    "CPSS_MC_PKTS_RCV_E",
    "CPSS_PKTS_64_OCTETS_E",
    "CPSS_PKTS_65TO127_OCTETS_E",
    "CPSS_PKTS_128TO255_OCTETS_E",
    "CPSS_PKTS_256TO511_OCTETS_E",
    "CPSS_PKTS_512TO1023_OCTETS_E",
    "CPSS_PKTS_1024TOMAX_OCTETS_E",
    "CPSS_GOOD_OCTETS_SENT_E",
    "CPSS_GOOD_PKTS_SENT_E",
    "CPSS_EXCESSIVE_COLLISIONS_E",
    "CPSS_MC_PKTS_SENT_E",
    "CPSS_BRDC_PKTS_SENT_E",
    "CPSS_UNRECOG_MAC_CNTR_RCV_E",
    "CPSS_FC_SENT_E",
    "CPSS_GOOD_FC_RCV_E",
    "CPSS_DROP_EVENTS_E",
    "CPSS_UNDERSIZE_PKTS_E",
    "CPSS_FRAGMENTS_PKTS_E",
    "CPSS_OVERSIZE_PKTS_E",
    "CPSS_JABBER_PKTS_E",
    "CPSS_MAC_RCV_ERROR_E",
    "CPSS_BAD_CRC_E",
    "CPSS_COLLISIONS_E",
    "CPSS_LATE_COLLISIONS_E",
    "CPSS_BadFC_RCV_E",
    "CPSS_GOOD_UC_PKTS_RCV_E",
    "CPSS_GOOD_UC_PKTS_SENT_E",
    "CPSS_MULTIPLE_PKTS_SENT_E",
    "CPSS_DEFERRED_PKTS_SENT_E",
    "CPSS_PKTS_1024TO1518_OCTETS_E",
    "CPSS_PKTS_1519TOMAX_OCTETS_E",
    "CPSS_LAST_MAC_COUNTER_NUM_E"
};
PRV_CPSS_LOG_STC_ENUM_ARRAY_SIZE_MAC(CPSS_PORT_MAC_COUNTERS_ENT);
char * prvCpssLogEnum_CPSS_PORT_MAC_TYPE_ENT[]  =
{
    "CPSS_PORT_MAC_TYPE_NOT_EXISTS_E",
    "CPSS_PORT_MAC_TYPE_FE_E",
    "CPSS_PORT_MAC_TYPE_GE_E",
    "CPSS_PORT_MAC_TYPE_XG_E",
    "CPSS_PORT_MAC_TYPE_XLG_E",
    "CPSS_PORT_MAC_TYPE_HGL_E",
    "CPSS_PORT_MAC_TYPE_CG_E",
    "CPSS_PORT_MAC_TYPE_ILKN_E",
    "CPSS_PORT_MAC_TYPE_NOT_APPLICABLE_E"
};
PRV_CPSS_LOG_STC_ENUM_ARRAY_SIZE_MAC(CPSS_PORT_MAC_TYPE_ENT);
char * prvCpssLogEnum_CPSS_PORT_PCS_RESET_MODE_ENT[]  =
{
    "CPSS_PORT_PCS_RESET_MODE_RX_E",
    "CPSS_PORT_PCS_RESET_MODE_TX_E",
    "CPSS_PORT_PCS_RESET_MODE_ALL_E"
};
PRV_CPSS_LOG_STC_ENUM_ARRAY_SIZE_MAC(CPSS_PORT_PCS_RESET_MODE_ENT);
char * prvCpssLogEnum_CPSS_PORT_REF_CLOCK_SOURCE_ENT[]  =
{
    "CPSS_PORT_REF_CLOCK_SOURCE_PRIMARY_E",
    "CPSS_PORT_REF_CLOCK_SOURCE_SECONDARY_E",
    "CPSS_PORT_REF_CLOCK_SOURCE_LAST_E"
};
PRV_CPSS_LOG_STC_ENUM_ARRAY_SIZE_MAC(CPSS_PORT_REF_CLOCK_SOURCE_ENT);
char * prvCpssLogEnum_CPSS_PORT_RX_FC_PROFILE_SET_ENT[]  =
{
    "CPSS_PORT_RX_FC_PROFILE_1_E",
    "CPSS_PORT_RX_FC_PROFILE_2_E",
    "CPSS_PORT_RX_FC_PROFILE_3_E",
    "CPSS_PORT_RX_FC_PROFILE_4_E",
    "CPSS_PORT_RX_FC_PROFILE_5_E",
    "CPSS_PORT_RX_FC_PROFILE_6_E",
    "CPSS_PORT_RX_FC_PROFILE_7_E",
    "CPSS_PORT_RX_FC_PROFILE_8_E"
};
PRV_CPSS_LOG_STC_ENUM_ARRAY_SIZE_MAC(CPSS_PORT_RX_FC_PROFILE_SET_ENT);
char * prvCpssLogEnum_CPSS_PORT_SERDES_TUNING_TYPE_ENT[]  =
{
    "CPSS_PORT_SERDES_TUNING_NONE_E",
    "CPSS_PORT_SERDES_TUNING_RX_ONLY_E",
    "CPSS_PORT_SERDES_TUNING_TRX_E"
};
PRV_CPSS_LOG_STC_ENUM_ARRAY_SIZE_MAC(CPSS_PORT_SERDES_TUNING_TYPE_ENT);
char * prvCpssLogEnum_CPSS_PORT_SPEED_ENT[]  =
{
    "CPSS_PORT_SPEED_10_E",
    "CPSS_PORT_SPEED_100_E",
    "CPSS_PORT_SPEED_1000_E",
    "CPSS_PORT_SPEED_10000_E",
    "CPSS_PORT_SPEED_12000_E",
    "CPSS_PORT_SPEED_2500_E",
    "CPSS_PORT_SPEED_5000_E",
    "CPSS_PORT_SPEED_13600_E",
    "CPSS_PORT_SPEED_20000_E",
    "CPSS_PORT_SPEED_40000_E",
    "CPSS_PORT_SPEED_16000_E",
    "CPSS_PORT_SPEED_15000_E",
    "CPSS_PORT_SPEED_75000_E",
    "CPSS_PORT_SPEED_100G_E",
    "CPSS_PORT_SPEED_50000_E",
    "CPSS_PORT_SPEED_140G_E",
    "CPSS_PORT_SPEED_11800_E",
    "CPSS_PORT_SPEED_47200_E",
    "CPSS_PORT_SPEED_NA_E"
};
PRV_CPSS_LOG_STC_ENUM_ARRAY_SIZE_MAC(CPSS_PORT_SPEED_ENT);
char * prvCpssLogEnum_CPSS_PORT_TX_DROP_PROFILE_SET_ENT[]  =
{
    "CPSS_PORT_TX_DROP_PROFILE_1_E",
    "CPSS_PORT_TX_DROP_PROFILE_2_E",
    "CPSS_PORT_TX_DROP_PROFILE_3_E",
    "CPSS_PORT_TX_DROP_PROFILE_4_E",
    "CPSS_PORT_TX_DROP_PROFILE_5_E",
    "CPSS_PORT_TX_DROP_PROFILE_6_E",
    "CPSS_PORT_TX_DROP_PROFILE_7_E",
    "CPSS_PORT_TX_DROP_PROFILE_8_E",
    "CPSS_PORT_TX_DROP_PROFILE_9_E",
    "CPSS_PORT_TX_DROP_PROFILE_10_E",
    "CPSS_PORT_TX_DROP_PROFILE_11_E",
    "CPSS_PORT_TX_DROP_PROFILE_12_E",
    "CPSS_PORT_TX_DROP_PROFILE_13_E",
    "CPSS_PORT_TX_DROP_PROFILE_14_E",
    "CPSS_PORT_TX_DROP_PROFILE_15_E",
    "CPSS_PORT_TX_DROP_PROFILE_16_E"
};
PRV_CPSS_LOG_STC_ENUM_ARRAY_SIZE_MAC(CPSS_PORT_TX_DROP_PROFILE_SET_ENT);
char * prvCpssLogEnum_CPSS_PORT_TX_DROP_SHAPER_MODE_ENT[]  =
{
    "CPSS_PORT_TX_DROP_SHAPER_BYTE_MODE_E",
    "CPSS_PORT_TX_DROP_SHAPER_PACKET_MODE_E"
};
PRV_CPSS_LOG_STC_ENUM_ARRAY_SIZE_MAC(CPSS_PORT_TX_DROP_SHAPER_MODE_ENT);
char * prvCpssLogEnum_CPSS_PORT_TX_Q_ARB_GROUP_ENT[]  =
{
    "CPSS_PORT_TX_WRR_ARB_GROUP_0_E",
    "CPSS_PORT_TX_WRR_ARB_GROUP_1_E",
    "CPSS_PORT_TX_SP_ARB_GROUP_E"
};
PRV_CPSS_LOG_STC_ENUM_ARRAY_SIZE_MAC(CPSS_PORT_TX_Q_ARB_GROUP_ENT);
char * prvCpssLogEnum_CPSS_PORT_TX_SCHEDULER_PROFILE_SET_ENT[]  =
{
    "CPSS_PORT_TX_SCHEDULER_PROFILE_1_E",
    "CPSS_PORT_TX_SCHEDULER_PROFILE_2_E",
    "CPSS_PORT_TX_SCHEDULER_PROFILE_3_E",
    "CPSS_PORT_TX_SCHEDULER_PROFILE_4_E",
    "CPSS_PORT_TX_SCHEDULER_PROFILE_5_E",
    "CPSS_PORT_TX_SCHEDULER_PROFILE_6_E",
    "CPSS_PORT_TX_SCHEDULER_PROFILE_7_E",
    "CPSS_PORT_TX_SCHEDULER_PROFILE_8_E",
    "CPSS_PORT_TX_SCHEDULER_PROFILE_9_E",
    "CPSS_PORT_TX_SCHEDULER_PROFILE_10_E",
    "CPSS_PORT_TX_SCHEDULER_PROFILE_11_E",
    "CPSS_PORT_TX_SCHEDULER_PROFILE_12_E",
    "CPSS_PORT_TX_SCHEDULER_PROFILE_13_E",
    "CPSS_PORT_TX_SCHEDULER_PROFILE_14_E",
    "CPSS_PORT_TX_SCHEDULER_PROFILE_15_E",
    "CPSS_PORT_TX_SCHEDULER_PROFILE_16_E"
};
PRV_CPSS_LOG_STC_ENUM_ARRAY_SIZE_MAC(CPSS_PORT_TX_SCHEDULER_PROFILE_SET_ENT);
char * prvCpssLogEnum_CPSS_PORT_TX_WRR_MODE_ENT[]  =
{
    "CPSS_PORT_TX_WRR_BYTE_MODE_E",
    "CPSS_PORT_TX_WRR_PACKET_MODE_E"
};
PRV_CPSS_LOG_STC_ENUM_ARRAY_SIZE_MAC(CPSS_PORT_TX_WRR_MODE_ENT);
char * prvCpssLogEnum_CPSS_PORT_TX_WRR_MTU_ENT[]  =
{
    "CPSS_PORT_TX_WRR_MTU_256_E",
    "CPSS_PORT_TX_WRR_MTU_2K_E",
    "CPSS_PORT_TX_WRR_MTU_8K_E",
    "CPSS_PORT_TX_WRR_MTU_64_E",
    "CPSS_PORT_TX_WRR_MTU_128_E",
    "CPSS_PORT_TX_WRR_MTU_512_E",
    "CPSS_PORT_TX_WRR_MTU_1K_E",
    "CPSS_PORT_TX_WRR_MTU_4K_E",
    "CPSS_PORT_TX_WRR_MTU_16K_E",
    "CPSS_PORT_TX_WRR_MTU_32K_E",
    "CPSS_PORT_TX_WRR_MTU_64K_E"
};
PRV_CPSS_LOG_STC_ENUM_ARRAY_SIZE_MAC(CPSS_PORT_TX_WRR_MTU_ENT);
char * prvCpssLogEnum_CPSS_PORT_XGMII_MODE_ENT[]  =
{
    "CPSS_PORT_XGMII_LAN_E",
    "CPSS_PORT_XGMII_WAN_E",
    "CPSS_PORT_XGMII_FIXED_E"
};
PRV_CPSS_LOG_STC_ENUM_ARRAY_SIZE_MAC(CPSS_PORT_XGMII_MODE_ENT);
char * prvCpssLogEnum_CPSS_PORT_XG_FIXED_IPG_ENT[]  =
{
    "CPSS_PORT_XG_FIXED_IPG_8_BYTES_E",
    "CPSS_PORT_XG_FIXED_IPG_12_BYTES_E"
};
PRV_CPSS_LOG_STC_ENUM_ARRAY_SIZE_MAC(CPSS_PORT_XG_FIXED_IPG_ENT);


/********* structure fields log functions *********/

void prvCpssLogParamFuncStc_CPSS_PORT_ATTRIBUTES_STC_PTR
(
    IN CPSS_LOG_LIB_ENT         contextLib,
    IN CPSS_LOG_TYPE_ENT        logType,
    IN GT_CHAR_PTR              namePtr,
    IN void                   * fieldPtr,
    INOUT PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC     * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_START_FIELD_STC_MAC(CPSS_PORT_ATTRIBUTES_STC *, valPtr);
    PRV_CPSS_LOG_UNUSED_MAC(inOutParamInfoPtr);
    PRV_CPSS_LOG_STC_BOOL_MAC(valPtr, portLinkUp);
    PRV_CPSS_LOG_STC_ENUM_MAC(valPtr, portSpeed, CPSS_PORT_SPEED_ENT);
    PRV_CPSS_LOG_STC_ENUM_MAC(valPtr, portDuplexity, CPSS_PORT_DUPLEX_ENT);
    prvCpssLogStcLogEnd(contextLib, logType);
}
void prvCpssLogParamFuncStc_CPSS_PORT_EGRESS_CNTR_STC_PTR
(
    IN CPSS_LOG_LIB_ENT         contextLib,
    IN CPSS_LOG_TYPE_ENT        logType,
    IN GT_CHAR_PTR              namePtr,
    IN void                   * fieldPtr,
    INOUT PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC     * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_START_FIELD_STC_MAC(CPSS_PORT_EGRESS_CNTR_STC *, valPtr);
    PRV_CPSS_LOG_UNUSED_MAC(inOutParamInfoPtr);
    PRV_CPSS_LOG_STC_NUMBER_MAC(valPtr, outUcFrames);
    PRV_CPSS_LOG_STC_NUMBER_MAC(valPtr, outMcFrames);
    PRV_CPSS_LOG_STC_NUMBER_MAC(valPtr, outBcFrames);
    PRV_CPSS_LOG_STC_NUMBER_MAC(valPtr, brgEgrFilterDisc);
    PRV_CPSS_LOG_STC_NUMBER_MAC(valPtr, txqFilterDisc);
    PRV_CPSS_LOG_STC_NUMBER_MAC(valPtr, outCtrlFrames);
    PRV_CPSS_LOG_STC_NUMBER_MAC(valPtr, egrFrwDropFrames);
    PRV_CPSS_LOG_STC_NUMBER_MAC(valPtr, mcFifo3_0DropPkts);
    PRV_CPSS_LOG_STC_NUMBER_MAC(valPtr, mcFifo7_4DropPkts);
    prvCpssLogStcLogEnd(contextLib, logType);
}
void prvCpssLogParamFuncStc_CPSS_PORT_MAC_COUNTER_SET_STC_PTR
(
    IN CPSS_LOG_LIB_ENT         contextLib,
    IN CPSS_LOG_TYPE_ENT        logType,
    IN GT_CHAR_PTR              namePtr,
    IN void                   * fieldPtr,
    INOUT PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC     * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_START_FIELD_STC_MAC(CPSS_PORT_MAC_COUNTER_SET_STC *, valPtr);
    PRV_CPSS_LOG_UNUSED_MAC(inOutParamInfoPtr);
    PRV_CPSS_LOG_STC_STC_MAC(valPtr, goodOctetsRcv, GT_U64);
    PRV_CPSS_LOG_STC_STC_MAC(valPtr, badOctetsRcv, GT_U64);
    PRV_CPSS_LOG_STC_STC_MAC(valPtr, macTransmitErr, GT_U64);
    PRV_CPSS_LOG_STC_STC_MAC(valPtr, goodPktsRcv, GT_U64);
    PRV_CPSS_LOG_STC_STC_MAC(valPtr, badPktsRcv, GT_U64);
    PRV_CPSS_LOG_STC_STC_MAC(valPtr, brdcPktsRcv, GT_U64);
    PRV_CPSS_LOG_STC_STC_MAC(valPtr, mcPktsRcv, GT_U64);
    PRV_CPSS_LOG_STC_STC_MAC(valPtr, pkts64Octets, GT_U64);
    PRV_CPSS_LOG_STC_STC_MAC(valPtr, pkts65to127Octets, GT_U64);
    PRV_CPSS_LOG_STC_STC_MAC(valPtr, pkts128to255Octets, GT_U64);
    PRV_CPSS_LOG_STC_STC_MAC(valPtr, pkts256to511Octets, GT_U64);
    PRV_CPSS_LOG_STC_STC_MAC(valPtr, pkts512to1023Octets, GT_U64);
    PRV_CPSS_LOG_STC_STC_MAC(valPtr, pkts1024tomaxOoctets, GT_U64);
    PRV_CPSS_LOG_STC_STC_MAC(valPtr, goodOctetsSent, GT_U64);
    PRV_CPSS_LOG_STC_STC_MAC(valPtr, goodPktsSent, GT_U64);
    PRV_CPSS_LOG_STC_STC_MAC(valPtr, excessiveCollisions, GT_U64);
    PRV_CPSS_LOG_STC_STC_MAC(valPtr, mcPktsSent, GT_U64);
    PRV_CPSS_LOG_STC_STC_MAC(valPtr, brdcPktsSent, GT_U64);
    PRV_CPSS_LOG_STC_STC_MAC(valPtr, unrecogMacCntrRcv, GT_U64);
    PRV_CPSS_LOG_STC_STC_MAC(valPtr, fcSent, GT_U64);
    PRV_CPSS_LOG_STC_STC_MAC(valPtr, goodFcRcv, GT_U64);
    PRV_CPSS_LOG_STC_STC_MAC(valPtr, dropEvents, GT_U64);
    PRV_CPSS_LOG_STC_STC_MAC(valPtr, undersizePkts, GT_U64);
    PRV_CPSS_LOG_STC_STC_MAC(valPtr, fragmentsPkts, GT_U64);
    PRV_CPSS_LOG_STC_STC_MAC(valPtr, oversizePkts, GT_U64);
    PRV_CPSS_LOG_STC_STC_MAC(valPtr, jabberPkts, GT_U64);
    PRV_CPSS_LOG_STC_STC_MAC(valPtr, macRcvError, GT_U64);
    PRV_CPSS_LOG_STC_STC_MAC(valPtr, badCrc, GT_U64);
    PRV_CPSS_LOG_STC_STC_MAC(valPtr, collisions, GT_U64);
    PRV_CPSS_LOG_STC_STC_MAC(valPtr, lateCollisions, GT_U64);
    PRV_CPSS_LOG_STC_STC_MAC(valPtr, badFcRcv, GT_U64);
    PRV_CPSS_LOG_STC_STC_MAC(valPtr, ucPktsRcv, GT_U64);
    PRV_CPSS_LOG_STC_STC_MAC(valPtr, ucPktsSent, GT_U64);
    PRV_CPSS_LOG_STC_STC_MAC(valPtr, multiplePktsSent, GT_U64);
    PRV_CPSS_LOG_STC_STC_MAC(valPtr, deferredPktsSent, GT_U64);
    PRV_CPSS_LOG_STC_STC_MAC(valPtr, pkts1024to1518Octets, GT_U64);
    PRV_CPSS_LOG_STC_STC_MAC(valPtr, pkts1519toMaxOctets, GT_U64);
    prvCpssLogStcLogEnd(contextLib, logType);
}
void prvCpssLogParamFuncStc_CPSS_PORT_MAC_STATUS_STC_PTR
(
    IN CPSS_LOG_LIB_ENT         contextLib,
    IN CPSS_LOG_TYPE_ENT        logType,
    IN GT_CHAR_PTR              namePtr,
    IN void                   * fieldPtr,
    INOUT PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC     * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_START_FIELD_STC_MAC(CPSS_PORT_MAC_STATUS_STC *, valPtr);
    PRV_CPSS_LOG_UNUSED_MAC(inOutParamInfoPtr);
    PRV_CPSS_LOG_STC_BOOL_MAC(valPtr, isPortRxPause);
    PRV_CPSS_LOG_STC_BOOL_MAC(valPtr, isPortTxPause);
    PRV_CPSS_LOG_STC_BOOL_MAC(valPtr, isPortBackPres);
    PRV_CPSS_LOG_STC_BOOL_MAC(valPtr, isPortBufFull);
    PRV_CPSS_LOG_STC_BOOL_MAC(valPtr, isPortSyncFail);
    PRV_CPSS_LOG_STC_BOOL_MAC(valPtr, isPortHiErrorRate);
    PRV_CPSS_LOG_STC_BOOL_MAC(valPtr, isPortAnDone);
    PRV_CPSS_LOG_STC_BOOL_MAC(valPtr, isPortFatalError);
    prvCpssLogStcLogEnd(contextLib, logType);
}
void prvCpssLogParamFuncStc_CPSS_PORT_MODE_SPEED_STC_PTR
(
    IN CPSS_LOG_LIB_ENT         contextLib,
    IN CPSS_LOG_TYPE_ENT        logType,
    IN GT_CHAR_PTR              namePtr,
    IN void                   * fieldPtr,
    INOUT PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC     * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_START_FIELD_STC_MAC(CPSS_PORT_MODE_SPEED_STC *, valPtr);
    PRV_CPSS_LOG_UNUSED_MAC(inOutParamInfoPtr);
    PRV_CPSS_LOG_STC_ENUM_MAP_MAC(valPtr, ifMode, CPSS_PORT_INTERFACE_MODE_ENT);
    PRV_CPSS_LOG_STC_ENUM_MAC(valPtr, speed, CPSS_PORT_SPEED_ENT);
    prvCpssLogStcLogEnd(contextLib, logType);
}
void prvCpssLogParamFuncStc_CPSS_PORT_TX_Q_TAIL_DROP_PROF_TC_PARAMS_PTR
(
    IN CPSS_LOG_LIB_ENT         contextLib,
    IN CPSS_LOG_TYPE_ENT        logType,
    IN GT_CHAR_PTR              namePtr,
    IN void                   * fieldPtr,
    INOUT PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC     * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_START_FIELD_STC_MAC(CPSS_PORT_TX_Q_TAIL_DROP_PROF_TC_PARAMS *, valPtr);
    PRV_CPSS_LOG_UNUSED_MAC(inOutParamInfoPtr);
    PRV_CPSS_LOG_STC_NUMBER_MAC(valPtr, dp0MaxBuffNum);
    PRV_CPSS_LOG_STC_NUMBER_MAC(valPtr, dp1MaxBuffNum);
    PRV_CPSS_LOG_STC_NUMBER_MAC(valPtr, dp2MaxBuffNum);
    PRV_CPSS_LOG_STC_NUMBER_MAC(valPtr, dp0MaxDescrNum);
    PRV_CPSS_LOG_STC_NUMBER_MAC(valPtr, dp1MaxDescrNum);
    PRV_CPSS_LOG_STC_NUMBER_MAC(valPtr, dp2MaxDescrNum);
    PRV_CPSS_LOG_STC_NUMBER_MAC(valPtr, tcMaxBuffNum);
    PRV_CPSS_LOG_STC_NUMBER_MAC(valPtr, tcMaxDescrNum);
    PRV_CPSS_LOG_STC_NUMBER_MAC(valPtr, dp0MaxMCBuffNum);
    PRV_CPSS_LOG_STC_NUMBER_MAC(valPtr, dp1MaxMCBuffNum);
    PRV_CPSS_LOG_STC_NUMBER_MAC(valPtr, dp2MaxMCBuffNum);
    PRV_CPSS_LOG_STC_BOOL_MAC(valPtr, sharedUcAndMcCountersDisable);
    prvCpssLogStcLogEnd(contextLib, logType);
}


/********* parameters log functions *********/

void prvCpssLogParamFunc_CPSS_PORT_ATTRIBUTES_STC_PTR(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_START_PARAM_STC_MAC(CPSS_PORT_ATTRIBUTES_STC*, paramVal);
    prvCpssLogParamFuncStc_CPSS_PORT_ATTRIBUTES_STC_PTR(contextLib, logType, namePtr, paramVal, inOutParamInfoPtr);
}
void prvCpssLogParamFunc_CPSS_PORT_DIRECTION_ENT(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_SET_PARAM_VAL_MAC(CPSS_PORT_DIRECTION_ENT, paramVal);
    PRV_CPSS_LOG_ENUM_MAC(namePtr, paramVal, CPSS_PORT_DIRECTION_ENT);
}
void prvCpssLogParamFunc_CPSS_PORT_DUPLEX_ENT(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_SET_PARAM_VAL_MAC(CPSS_PORT_DUPLEX_ENT, paramVal);
    PRV_CPSS_LOG_ENUM_MAC(namePtr, paramVal, CPSS_PORT_DUPLEX_ENT);
}
void prvCpssLogParamFunc_CPSS_PORT_DUPLEX_ENT_PTR(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_SET_PARAM_VAL_MAC(CPSS_PORT_DUPLEX_ENT*, paramVal);
    if (paramVal == NULL)
    {
        PRV_CPSS_LOG_AND_HISTORY_PARAM_MAC(contextLib, logType, "%s = NULL\n", namePtr);
        return;
    }
    PRV_CPSS_LOG_ENUM_MAC(namePtr, *paramVal, CPSS_PORT_DUPLEX_ENT);
}
void prvCpssLogParamFunc_CPSS_PORT_EGRESS_CNTR_STC_PTR(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_START_PARAM_STC_MAC(CPSS_PORT_EGRESS_CNTR_STC*, paramVal);
    prvCpssLogParamFuncStc_CPSS_PORT_EGRESS_CNTR_STC_PTR(contextLib, logType, namePtr, paramVal, inOutParamInfoPtr);
}
void prvCpssLogParamFunc_CPSS_PORT_EGRESS_CNT_MODE_ENT(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_SET_PARAM_VAL_MAC(CPSS_PORT_EGRESS_CNT_MODE_ENT, paramVal);
    PRV_CPSS_LOG_ENUM_MAP_MAC(namePtr, paramVal, CPSS_PORT_EGRESS_CNT_MODE_ENT);
}
void prvCpssLogParamFunc_CPSS_PORT_EGRESS_CNT_MODE_ENT_PTR(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_SET_PARAM_VAL_MAC(CPSS_PORT_EGRESS_CNT_MODE_ENT*, paramVal);
    if (paramVal == NULL)
    {
        PRV_CPSS_LOG_AND_HISTORY_PARAM_MAC(contextLib, logType, "%s = NULL\n", namePtr);
        return;
    }
    PRV_CPSS_LOG_ENUM_MAP_MAC(namePtr, *paramVal, CPSS_PORT_EGRESS_CNT_MODE_ENT);
}
void prvCpssLogParamFunc_CPSS_PORT_FLOW_CONTROL_ENT(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_SET_PARAM_VAL_MAC(CPSS_PORT_FLOW_CONTROL_ENT, paramVal);
    PRV_CPSS_LOG_ENUM_MAC(namePtr, paramVal, CPSS_PORT_FLOW_CONTROL_ENT);
}
void prvCpssLogParamFunc_CPSS_PORT_FLOW_CONTROL_ENT_PTR(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_SET_PARAM_VAL_MAC(CPSS_PORT_FLOW_CONTROL_ENT*, paramVal);
    if (paramVal == NULL)
    {
        PRV_CPSS_LOG_AND_HISTORY_PARAM_MAC(contextLib, logType, "%s = NULL\n", namePtr);
        return;
    }
    PRV_CPSS_LOG_ENUM_MAC(namePtr, *paramVal, CPSS_PORT_FLOW_CONTROL_ENT);
}
void prvCpssLogParamFunc_CPSS_PORT_INTERFACE_MODE_ENT(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_SET_PARAM_VAL_MAC(CPSS_PORT_INTERFACE_MODE_ENT, paramVal);
    PRV_CPSS_LOG_ENUM_MAP_MAC(namePtr, paramVal, CPSS_PORT_INTERFACE_MODE_ENT);
}
void prvCpssLogParamFunc_CPSS_PORT_INTERFACE_MODE_ENT_PTR(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_SET_PARAM_VAL_MAC(CPSS_PORT_INTERFACE_MODE_ENT*, paramVal);
    if (paramVal == NULL)
    {
        PRV_CPSS_LOG_AND_HISTORY_PARAM_MAC(contextLib, logType, "%s = NULL\n", namePtr);
        return;
    }
    PRV_CPSS_LOG_ENUM_MAP_MAC(namePtr, *paramVal, CPSS_PORT_INTERFACE_MODE_ENT);
}
void prvCpssLogParamFunc_CPSS_PORT_INTERLAKEN_COUNTER_ID_ENT(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_SET_PARAM_VAL_MAC(CPSS_PORT_INTERLAKEN_COUNTER_ID_ENT, paramVal);
    PRV_CPSS_LOG_ENUM_MAC(namePtr, paramVal, CPSS_PORT_INTERLAKEN_COUNTER_ID_ENT);
}
void prvCpssLogParamFunc_CPSS_PORT_MAC_COUNTERS_ENT(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_SET_PARAM_VAL_MAC(CPSS_PORT_MAC_COUNTERS_ENT, paramVal);
    PRV_CPSS_LOG_ENUM_MAC(namePtr, paramVal, CPSS_PORT_MAC_COUNTERS_ENT);
}
void prvCpssLogParamFunc_CPSS_PORT_MAC_COUNTER_SET_STC_PTR(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_START_PARAM_STC_MAC(CPSS_PORT_MAC_COUNTER_SET_STC*, paramVal);
    prvCpssLogParamFuncStc_CPSS_PORT_MAC_COUNTER_SET_STC_PTR(contextLib, logType, namePtr, paramVal, inOutParamInfoPtr);
}
void prvCpssLogParamFunc_CPSS_PORT_MAC_STATUS_STC_PTR(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_START_PARAM_STC_MAC(CPSS_PORT_MAC_STATUS_STC*, paramVal);
    prvCpssLogParamFuncStc_CPSS_PORT_MAC_STATUS_STC_PTR(contextLib, logType, namePtr, paramVal, inOutParamInfoPtr);
}
void prvCpssLogParamFunc_CPSS_PORT_MAC_TYPE_ENT_PTR(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_SET_PARAM_VAL_MAC(CPSS_PORT_MAC_TYPE_ENT*, paramVal);
    if (paramVal == NULL)
    {
        PRV_CPSS_LOG_AND_HISTORY_PARAM_MAC(contextLib, logType, "%s = NULL\n", namePtr);
        return;
    }
    PRV_CPSS_LOG_ENUM_MAC(namePtr, *paramVal, CPSS_PORT_MAC_TYPE_ENT);
}
void prvCpssLogParamFunc_CPSS_PORT_MODE_SPEED_STC_PTR(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_START_PARAM_STC_MAC(CPSS_PORT_MODE_SPEED_STC*, paramVal);
    prvCpssLogParamFuncStc_CPSS_PORT_MODE_SPEED_STC_PTR(contextLib, logType, namePtr, paramVal, inOutParamInfoPtr);
}
void prvCpssLogParamFunc_CPSS_PORT_PCS_RESET_MODE_ENT(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_SET_PARAM_VAL_MAC(CPSS_PORT_PCS_RESET_MODE_ENT, paramVal);
    PRV_CPSS_LOG_ENUM_MAC(namePtr, paramVal, CPSS_PORT_PCS_RESET_MODE_ENT);
}
void prvCpssLogParamFunc_CPSS_PORT_REF_CLOCK_SOURCE_ENT(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_SET_PARAM_VAL_MAC(CPSS_PORT_REF_CLOCK_SOURCE_ENT, paramVal);
    PRV_CPSS_LOG_ENUM_MAC(namePtr, paramVal, CPSS_PORT_REF_CLOCK_SOURCE_ENT);
}
void prvCpssLogParamFunc_CPSS_PORT_REF_CLOCK_SOURCE_ENT_PTR(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_SET_PARAM_VAL_MAC(CPSS_PORT_REF_CLOCK_SOURCE_ENT*, paramVal);
    if (paramVal == NULL)
    {
        PRV_CPSS_LOG_AND_HISTORY_PARAM_MAC(contextLib, logType, "%s = NULL\n", namePtr);
        return;
    }
    PRV_CPSS_LOG_ENUM_MAC(namePtr, *paramVal, CPSS_PORT_REF_CLOCK_SOURCE_ENT);
}
void prvCpssLogParamFunc_CPSS_PORT_RX_FC_PROFILE_SET_ENT(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_SET_PARAM_VAL_MAC(CPSS_PORT_RX_FC_PROFILE_SET_ENT, paramVal);
    PRV_CPSS_LOG_ENUM_MAC(namePtr, paramVal, CPSS_PORT_RX_FC_PROFILE_SET_ENT);
}
void prvCpssLogParamFunc_CPSS_PORT_RX_FC_PROFILE_SET_ENT_PTR(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_SET_PARAM_VAL_MAC(CPSS_PORT_RX_FC_PROFILE_SET_ENT*, paramVal);
    if (paramVal == NULL)
    {
        PRV_CPSS_LOG_AND_HISTORY_PARAM_MAC(contextLib, logType, "%s = NULL\n", namePtr);
        return;
    }
    PRV_CPSS_LOG_ENUM_MAC(namePtr, *paramVal, CPSS_PORT_RX_FC_PROFILE_SET_ENT);
}
void prvCpssLogParamFunc_CPSS_PORT_SERDES_TUNING_TYPE_ENT(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_SET_PARAM_VAL_MAC(CPSS_PORT_SERDES_TUNING_TYPE_ENT, paramVal);
    PRV_CPSS_LOG_ENUM_MAC(namePtr, paramVal, CPSS_PORT_SERDES_TUNING_TYPE_ENT);
}
void prvCpssLogParamFunc_CPSS_PORT_SERDES_TUNING_TYPE_ENT_PTR(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_SET_PARAM_VAL_MAC(CPSS_PORT_SERDES_TUNING_TYPE_ENT*, paramVal);
    if (paramVal == NULL)
    {
        PRV_CPSS_LOG_AND_HISTORY_PARAM_MAC(contextLib, logType, "%s = NULL\n", namePtr);
        return;
    }
    PRV_CPSS_LOG_ENUM_MAC(namePtr, *paramVal, CPSS_PORT_SERDES_TUNING_TYPE_ENT);
}
void prvCpssLogParamFunc_CPSS_PORT_SPEED_ENT(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_SET_PARAM_VAL_MAC(CPSS_PORT_SPEED_ENT, paramVal);
    PRV_CPSS_LOG_ENUM_MAC(namePtr, paramVal, CPSS_PORT_SPEED_ENT);
}
void prvCpssLogParamFunc_CPSS_PORT_SPEED_ENT_PTR(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_SET_PARAM_VAL_MAC(CPSS_PORT_SPEED_ENT*, paramVal);
    if (paramVal == NULL)
    {
        PRV_CPSS_LOG_AND_HISTORY_PARAM_MAC(contextLib, logType, "%s = NULL\n", namePtr);
        return;
    }
    PRV_CPSS_LOG_ENUM_MAC(namePtr, *paramVal, CPSS_PORT_SPEED_ENT);
}
void prvCpssLogParamFunc_CPSS_PORT_TX_DROP_PROFILE_SET_ENT(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_SET_PARAM_VAL_MAC(CPSS_PORT_TX_DROP_PROFILE_SET_ENT, paramVal);
    PRV_CPSS_LOG_ENUM_MAC(namePtr, paramVal, CPSS_PORT_TX_DROP_PROFILE_SET_ENT);
}
void prvCpssLogParamFunc_CPSS_PORT_TX_DROP_PROFILE_SET_ENT_PTR(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_SET_PARAM_VAL_MAC(CPSS_PORT_TX_DROP_PROFILE_SET_ENT*, paramVal);
    if (paramVal == NULL)
    {
        PRV_CPSS_LOG_AND_HISTORY_PARAM_MAC(contextLib, logType, "%s = NULL\n", namePtr);
        return;
    }
    PRV_CPSS_LOG_ENUM_MAC(namePtr, *paramVal, CPSS_PORT_TX_DROP_PROFILE_SET_ENT);
}
void prvCpssLogParamFunc_CPSS_PORT_TX_DROP_SHAPER_MODE_ENT(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_SET_PARAM_VAL_MAC(CPSS_PORT_TX_DROP_SHAPER_MODE_ENT, paramVal);
    PRV_CPSS_LOG_ENUM_MAC(namePtr, paramVal, CPSS_PORT_TX_DROP_SHAPER_MODE_ENT);
}
void prvCpssLogParamFunc_CPSS_PORT_TX_DROP_SHAPER_MODE_ENT_PTR(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_SET_PARAM_VAL_MAC(CPSS_PORT_TX_DROP_SHAPER_MODE_ENT*, paramVal);
    if (paramVal == NULL)
    {
        PRV_CPSS_LOG_AND_HISTORY_PARAM_MAC(contextLib, logType, "%s = NULL\n", namePtr);
        return;
    }
    PRV_CPSS_LOG_ENUM_MAC(namePtr, *paramVal, CPSS_PORT_TX_DROP_SHAPER_MODE_ENT);
}
void prvCpssLogParamFunc_CPSS_PORT_TX_Q_ARB_GROUP_ENT(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_SET_PARAM_VAL_MAC(CPSS_PORT_TX_Q_ARB_GROUP_ENT, paramVal);
    PRV_CPSS_LOG_ENUM_MAC(namePtr, paramVal, CPSS_PORT_TX_Q_ARB_GROUP_ENT);
}
void prvCpssLogParamFunc_CPSS_PORT_TX_Q_ARB_GROUP_ENT_PTR(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_SET_PARAM_VAL_MAC(CPSS_PORT_TX_Q_ARB_GROUP_ENT*, paramVal);
    if (paramVal == NULL)
    {
        PRV_CPSS_LOG_AND_HISTORY_PARAM_MAC(contextLib, logType, "%s = NULL\n", namePtr);
        return;
    }
    PRV_CPSS_LOG_ENUM_MAC(namePtr, *paramVal, CPSS_PORT_TX_Q_ARB_GROUP_ENT);
}
void prvCpssLogParamFunc_CPSS_PORT_TX_Q_TAIL_DROP_PROF_TC_PARAMS_PTR(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_START_PARAM_STC_MAC(CPSS_PORT_TX_Q_TAIL_DROP_PROF_TC_PARAMS*, paramVal);
    prvCpssLogParamFuncStc_CPSS_PORT_TX_Q_TAIL_DROP_PROF_TC_PARAMS_PTR(contextLib, logType, namePtr, paramVal, inOutParamInfoPtr);
}
void prvCpssLogParamFunc_CPSS_PORT_TX_SCHEDULER_PROFILE_SET_ENT(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_SET_PARAM_VAL_MAC(CPSS_PORT_TX_SCHEDULER_PROFILE_SET_ENT, paramVal);
    PRV_CPSS_LOG_ENUM_MAC(namePtr, paramVal, CPSS_PORT_TX_SCHEDULER_PROFILE_SET_ENT);
}
void prvCpssLogParamFunc_CPSS_PORT_TX_SCHEDULER_PROFILE_SET_ENT_PTR(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_SET_PARAM_VAL_MAC(CPSS_PORT_TX_SCHEDULER_PROFILE_SET_ENT*, paramVal);
    if (paramVal == NULL)
    {
        PRV_CPSS_LOG_AND_HISTORY_PARAM_MAC(contextLib, logType, "%s = NULL\n", namePtr);
        return;
    }
    PRV_CPSS_LOG_ENUM_MAC(namePtr, *paramVal, CPSS_PORT_TX_SCHEDULER_PROFILE_SET_ENT);
}
void prvCpssLogParamFunc_CPSS_PORT_TX_WRR_MODE_ENT(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_SET_PARAM_VAL_MAC(CPSS_PORT_TX_WRR_MODE_ENT, paramVal);
    PRV_CPSS_LOG_ENUM_MAC(namePtr, paramVal, CPSS_PORT_TX_WRR_MODE_ENT);
}
void prvCpssLogParamFunc_CPSS_PORT_TX_WRR_MODE_ENT_PTR(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_SET_PARAM_VAL_MAC(CPSS_PORT_TX_WRR_MODE_ENT*, paramVal);
    if (paramVal == NULL)
    {
        PRV_CPSS_LOG_AND_HISTORY_PARAM_MAC(contextLib, logType, "%s = NULL\n", namePtr);
        return;
    }
    PRV_CPSS_LOG_ENUM_MAC(namePtr, *paramVal, CPSS_PORT_TX_WRR_MODE_ENT);
}
void prvCpssLogParamFunc_CPSS_PORT_TX_WRR_MTU_ENT(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_SET_PARAM_VAL_MAC(CPSS_PORT_TX_WRR_MTU_ENT, paramVal);
    PRV_CPSS_LOG_ENUM_MAC(namePtr, paramVal, CPSS_PORT_TX_WRR_MTU_ENT);
}
void prvCpssLogParamFunc_CPSS_PORT_TX_WRR_MTU_ENT_PTR(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_SET_PARAM_VAL_MAC(CPSS_PORT_TX_WRR_MTU_ENT*, paramVal);
    if (paramVal == NULL)
    {
        PRV_CPSS_LOG_AND_HISTORY_PARAM_MAC(contextLib, logType, "%s = NULL\n", namePtr);
        return;
    }
    PRV_CPSS_LOG_ENUM_MAC(namePtr, *paramVal, CPSS_PORT_TX_WRR_MTU_ENT);
}
void prvCpssLogParamFunc_CPSS_PORT_XGMII_MODE_ENT(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_SET_PARAM_VAL_MAC(CPSS_PORT_XGMII_MODE_ENT, paramVal);
    PRV_CPSS_LOG_ENUM_MAC(namePtr, paramVal, CPSS_PORT_XGMII_MODE_ENT);
}
void prvCpssLogParamFunc_CPSS_PORT_XGMII_MODE_ENT_PTR(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_SET_PARAM_VAL_MAC(CPSS_PORT_XGMII_MODE_ENT*, paramVal);
    if (paramVal == NULL)
    {
        PRV_CPSS_LOG_AND_HISTORY_PARAM_MAC(contextLib, logType, "%s = NULL\n", namePtr);
        return;
    }
    PRV_CPSS_LOG_ENUM_MAC(namePtr, *paramVal, CPSS_PORT_XGMII_MODE_ENT);
}
void prvCpssLogParamFunc_CPSS_PORT_XG_FIXED_IPG_ENT(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_SET_PARAM_VAL_MAC(CPSS_PORT_XG_FIXED_IPG_ENT, paramVal);
    PRV_CPSS_LOG_ENUM_MAC(namePtr, paramVal, CPSS_PORT_XG_FIXED_IPG_ENT);
}
void prvCpssLogParamFunc_CPSS_PORT_XG_FIXED_IPG_ENT_PTR(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_SET_PARAM_VAL_MAC(CPSS_PORT_XG_FIXED_IPG_ENT*, paramVal);
    if (paramVal == NULL)
    {
        PRV_CPSS_LOG_AND_HISTORY_PARAM_MAC(contextLib, logType, "%s = NULL\n", namePtr);
        return;
    }
    PRV_CPSS_LOG_ENUM_MAC(namePtr, *paramVal, CPSS_PORT_XG_FIXED_IPG_ENT);
}

