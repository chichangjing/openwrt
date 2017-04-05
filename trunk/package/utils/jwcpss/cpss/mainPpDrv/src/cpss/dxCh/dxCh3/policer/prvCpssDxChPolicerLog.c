/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* prvCpssDxChPolicerLog.c
*       WARNING!!! this is a generated file, please don't edit it manually
* COMMENTS:
*
* FILE REVISION NUMBER:
*       $Revision: 1 $
*******************************************************************************/
#include <cpss/generic/log/cpssLog.h>
#include <cpss/generic/log/prvCpssLog.h>
#include <cpss/extServices/private/prvCpssBindFunc.h>
#include <cpss/dxCh/dxCh3/policer/cpssDxCh3Policer.h>
#include <cpss/dxCh/dxCh3/policer/private/prvCpssDxChPolicerLog.h>
#include <cpss/dxCh/dxChxGen/policer/cpssDxChPolicer.h>
#include <cpss/generic/cos/private/prvCpssGenCosLog.h>
#include <cpss/generic/log/prvCpssGenLog.h>
#include <cpss/generic/policer/private/prvCpssGenPolicerLog.h>


/********* enums *********/

char * prvCpssLogEnum_CPSS_DXCH3_POLICER_BILLING_CNTR_MODE_ENT[]  =
{
    "CPSS_DXCH3_POLICER_BILLING_CNTR_1_BYTE_E",
    "CPSS_DXCH3_POLICER_BILLING_CNTR_16_BYTES_E",
    "CPSS_DXCH3_POLICER_BILLING_CNTR_PACKET_E"
};
PRV_CPSS_LOG_STC_ENUM_ARRAY_SIZE_MAC(CPSS_DXCH3_POLICER_BILLING_CNTR_MODE_ENT);
char * prvCpssLogEnum_CPSS_DXCH3_POLICER_COLOR_COUNT_MODE_ENT[]  =
{
    "CPSS_DXCH3_POLICER_COLOR_COUNT_CL_E",
    "CPSS_DXCH3_POLICER_COLOR_COUNT_DP_E"
};
PRV_CPSS_LOG_STC_ENUM_ARRAY_SIZE_MAC(CPSS_DXCH3_POLICER_COLOR_COUNT_MODE_ENT);
char * prvCpssLogEnum_CPSS_DXCH3_POLICER_ENTRY_TYPE_ENT[]  =
{
    "CPSS_DXCH3_POLICER_ENTRY_METERING_E",
    "CPSS_DXCH3_POLICER_ENTRY_BILLING_E"
};
PRV_CPSS_LOG_STC_ENUM_ARRAY_SIZE_MAC(CPSS_DXCH3_POLICER_ENTRY_TYPE_ENT);
char * prvCpssLogEnum_CPSS_DXCH3_POLICER_METER_MODE_ENT[]  =
{
    "CPSS_DXCH3_POLICER_METER_MODE_SR_TCM_E",
    "CPSS_DXCH3_POLICER_METER_MODE_TR_TCM_E",
    "CPSS_DXCH3_POLICER_METER_MODE_MEF0_E",
    "CPSS_DXCH3_POLICER_METER_MODE_MEF1_E",
    "CPSS_DXCH3_POLICER_METER_MODE_START_OF_ENVELOPE_E",
    "CPSS_DXCH3_POLICER_METER_MODE_NOT_START_OF_ENVELOPE_E"
};
PRV_CPSS_LOG_STC_ENUM_ARRAY_SIZE_MAC(CPSS_DXCH3_POLICER_METER_MODE_ENT);
char * prvCpssLogEnum_CPSS_DXCH3_POLICER_METER_RESOLUTION_ENT[]  =
{
    "CPSS_DXCH3_POLICER_METER_RESOLUTION_BYTES_E",
    "CPSS_DXCH3_POLICER_METER_RESOLUTION_PACKETS_E"
};
PRV_CPSS_LOG_STC_ENUM_ARRAY_SIZE_MAC(CPSS_DXCH3_POLICER_METER_RESOLUTION_ENT);
char * prvCpssLogEnum_CPSS_DXCH3_POLICER_MNG_CNTR_RESOLUTION_ENT[]  =
{
    "CPSS_DXCH3_POLICER_MNG_CNTR_RESOLUTION_1B_E",
    "CPSS_DXCH3_POLICER_MNG_CNTR_RESOLUTION_16B_E"
};
PRV_CPSS_LOG_STC_ENUM_ARRAY_SIZE_MAC(CPSS_DXCH3_POLICER_MNG_CNTR_RESOLUTION_ENT);
char * prvCpssLogEnum_CPSS_DXCH3_POLICER_MNG_CNTR_SET_ENT[]  =
{
    "CPSS_DXCH3_POLICER_MNG_CNTR_SET0_E",
    "CPSS_DXCH3_POLICER_MNG_CNTR_SET1_E",
    "CPSS_DXCH3_POLICER_MNG_CNTR_SET2_E",
    "CPSS_DXCH3_POLICER_MNG_CNTR_DISABLED_E"
};
PRV_CPSS_LOG_STC_ENUM_ARRAY_SIZE_MAC(CPSS_DXCH3_POLICER_MNG_CNTR_SET_ENT);
char * prvCpssLogEnum_CPSS_DXCH3_POLICER_MNG_CNTR_TYPE_ENT[]  =
{
    "CPSS_DXCH3_POLICER_MNG_CNTR_GREEN_E",
    "CPSS_DXCH3_POLICER_MNG_CNTR_YELLOW_E",
    "CPSS_DXCH3_POLICER_MNG_CNTR_RED_E",
    "CPSS_DXCH3_POLICER_MNG_CNTR_DROP_E"
};
PRV_CPSS_LOG_STC_ENUM_ARRAY_SIZE_MAC(CPSS_DXCH3_POLICER_MNG_CNTR_TYPE_ENT);
char * prvCpssLogEnum_CPSS_DXCH3_POLICER_TS_TAG_COUNTING_MODE_ENT[]  =
{
    "CPSS_DXCH3_POLICER_TS_TAG_COUNTING_MODE_INCLUDE_E",
    "CPSS_DXCH3_POLICER_TS_TAG_COUNTING_MODE_EXCLUDE_E"
};
PRV_CPSS_LOG_STC_ENUM_ARRAY_SIZE_MAC(CPSS_DXCH3_POLICER_TS_TAG_COUNTING_MODE_ENT);
char * prvCpssLogEnum_CPSS_DXCH3_POLICER_TT_PACKET_SIZE_MODE_ENT[]  =
{
    "CPSS_DXCH3_POLICER_TT_PACKET_SIZE_REGULAR_E",
    "CPSS_DXCH3_POLICER_TT_PACKET_SIZE_PASSENGER_E"
};
PRV_CPSS_LOG_STC_ENUM_ARRAY_SIZE_MAC(CPSS_DXCH3_POLICER_TT_PACKET_SIZE_MODE_ENT);
char * prvCpssLogEnum_CPSS_DXCH_POLICER_COUNTING_MODE_ENT[]  =
{
    "CPSS_DXCH_POLICER_COUNTING_DISABLE_E",
    "CPSS_DXCH_POLICER_COUNTING_BILLING_IPFIX_E",
    "CPSS_DXCH_POLICER_COUNTING_POLICY_E",
    "CPSS_DXCH_POLICER_COUNTING_VLAN_E"
};
PRV_CPSS_LOG_STC_ENUM_ARRAY_SIZE_MAC(CPSS_DXCH_POLICER_COUNTING_MODE_ENT);
char * prvCpssLogEnum_CPSS_DXCH_POLICER_ENVELOPE_MAX_SIZE_ENT[]  =
{
    "CPSS_DXCH_POLICER_ENVELOPE_MAX_SIZE_2_E",
    "CPSS_DXCH_POLICER_ENVELOPE_MAX_SIZE_4_E",
    "CPSS_DXCH_POLICER_ENVELOPE_MAX_SIZE_8_E"
};
PRV_CPSS_LOG_STC_ENUM_ARRAY_SIZE_MAC(CPSS_DXCH_POLICER_ENVELOPE_MAX_SIZE_ENT);
char * prvCpssLogEnum_CPSS_DXCH_POLICER_E_ATTR_METER_MODE_ENT[]  =
{
    "CPSS_DXCH_POLICER_E_ATTR_METER_MODE_DISABLED_E",
    "CPSS_DXCH_POLICER_E_ATTR_METER_MODE_EPORT_ENABLED_E",
    "CPSS_DXCH_POLICER_E_ATTR_METER_MODE_EVLAN_ENABLED_E"
};
PRV_CPSS_LOG_STC_ENUM_ARRAY_SIZE_MAC(CPSS_DXCH_POLICER_E_ATTR_METER_MODE_ENT);
char * prvCpssLogEnum_CPSS_DXCH_POLICER_L2_REMARK_MODEL_ENT[]  =
{
    "CPSS_DXCH_POLICER_L2_REMARK_MODEL_TC_E",
    "CPSS_DXCH_POLICER_L2_REMARK_MODEL_UP_E"
};
PRV_CPSS_LOG_STC_ENUM_ARRAY_SIZE_MAC(CPSS_DXCH_POLICER_L2_REMARK_MODEL_ENT);
char * prvCpssLogEnum_CPSS_DXCH_POLICER_MEMORY_CTRL_MODE_ENT[]  =
{
    "CPSS_DXCH_POLICER_MEMORY_CTRL_MODE_0_E",
    "CPSS_DXCH_POLICER_MEMORY_CTRL_MODE_1_E",
    "CPSS_DXCH_POLICER_MEMORY_CTRL_MODE_2_E",
    "CPSS_DXCH_POLICER_MEMORY_CTRL_MODE_3_E",
    "CPSS_DXCH_POLICER_MEMORY_CTRL_MODE_4_E",
    "CPSS_DXCH_POLICER_MEMORY_CTRL_MODE_5_E"
};
PRV_CPSS_LOG_STC_ENUM_ARRAY_SIZE_MAC(CPSS_DXCH_POLICER_MEMORY_CTRL_MODE_ENT);
char * prvCpssLogEnum_CPSS_DXCH_POLICER_METERING_CALC_METHOD_ENT[]  =
{
    "CPSS_DXCH_POLICER_METERING_CALC_METHOD_CIR_ONLY_E",
    "CPSS_DXCH_POLICER_METERING_CALC_METHOD_CIR_AND_CBS_E"
};
PRV_CPSS_LOG_STC_ENUM_ARRAY_SIZE_MAC(CPSS_DXCH_POLICER_METERING_CALC_METHOD_ENT);
char * prvCpssLogEnum_CPSS_DXCH_POLICER_PORT_MODE_ADDR_SELECT_TYPE_ENT[]  =
{
    "CPSS_DXCH_POLICER_PORT_MODE_ADDR_SELECT_TYPE_FULL_E",
    "CPSS_DXCH_POLICER_PORT_MODE_ADDR_SELECT_TYPE_COMPRESSED_E"
};
PRV_CPSS_LOG_STC_ENUM_ARRAY_SIZE_MAC(CPSS_DXCH_POLICER_PORT_MODE_ADDR_SELECT_TYPE_ENT);
char * prvCpssLogEnum_CPSS_DXCH_POLICER_REMARK_TABLE_TYPE_ENT[]  =
{
    "CPSS_DXCH_POLICER_REMARK_TABLE_TYPE_TC_UP_E",
    "CPSS_DXCH_POLICER_REMARK_TABLE_TYPE_DSCP_E",
    "CPSS_DXCH_POLICER_REMARK_TABLE_TYPE_EXP_E"
};
PRV_CPSS_LOG_STC_ENUM_ARRAY_SIZE_MAC(CPSS_DXCH_POLICER_REMARK_TABLE_TYPE_ENT);
char * prvCpssLogEnum_CPSS_DXCH_POLICER_STAGE_METER_MODE_ENT[]  =
{
    "CPSS_DXCH_POLICER_STAGE_METER_MODE_PORT_E",
    "CPSS_DXCH_POLICER_STAGE_METER_MODE_FLOW_E"
};
PRV_CPSS_LOG_STC_ENUM_ARRAY_SIZE_MAC(CPSS_DXCH_POLICER_STAGE_METER_MODE_ENT);
char * prvCpssLogEnum_CPSS_DXCH_POLICER_STAGE_TYPE_ENT[]  =
{
    "CPSS_DXCH_POLICER_STAGE_INGRESS_0_E",
    "CPSS_DXCH_POLICER_STAGE_INGRESS_1_E",
    "CPSS_DXCH_POLICER_STAGE_EGRESS_E"
};
PRV_CPSS_LOG_STC_ENUM_ARRAY_SIZE_MAC(CPSS_DXCH_POLICER_STAGE_TYPE_ENT);
char * prvCpssLogEnum_CPSS_DXCH_POLICER_STORM_TYPE_ENT[]  =
{
    "CPSS_DXCH_POLICER_STORM_TYPE_UC_KNOWN_E",
    "CPSS_DXCH_POLICER_STORM_TYPE_UC_UNKNOWN_E",
    "CPSS_DXCH_POLICER_STORM_TYPE_MC_UNREGISTERED_E",
    "CPSS_DXCH_POLICER_STORM_TYPE_MC_REGISTERED_E",
    "CPSS_DXCH_POLICER_STORM_TYPE_BC_E",
    "CPSS_DXCH_POLICER_STORM_TYPE_TCP_SYN_E"
};
PRV_CPSS_LOG_STC_ENUM_ARRAY_SIZE_MAC(CPSS_DXCH_POLICER_STORM_TYPE_ENT);
char * prvCpssLogEnum_CPSS_DXCH_POLICER_VLAN_CNTR_MODE_ENT[]  =
{
    "CPSS_DXCH_POLICER_VLAN_CNTR_MODE_BYTES_E",
    "CPSS_DXCH_POLICER_VLAN_CNTR_MODE_PACKETS_E"
};
PRV_CPSS_LOG_STC_ENUM_ARRAY_SIZE_MAC(CPSS_DXCH_POLICER_VLAN_CNTR_MODE_ENT);
char * prvCpssLogEnum_CPSS_DXCH3_POLICER_DSA_TAG_COUNTING_MODE_ENT[]  =
{
    "CPSS_DXCH3_POLICER_DSA_TAG_COUNTING_MODE_INCLUDED_E",
    "CPSS_DXCH3_POLICER_DSA_TAG_COUNTING_MODE_COMPENSATED_E"
};
PRV_CPSS_LOG_STC_ENUM_ARRAY_SIZE_MAC(CPSS_DXCH3_POLICER_DSA_TAG_COUNTING_MODE_ENT);
char * prvCpssLogEnum_CPSS_DXCH3_POLICER_LM_CNTR_CAPTURE_MODE_ENT[]  =
{
    "CPSS_DXCH3_POLICER_LM_CNTR_CAPTURE_MODE_COPY_E",
    "CPSS_DXCH3_POLICER_LM_CNTR_CAPTURE_MODE_INSERT_E"
};
PRV_CPSS_LOG_STC_ENUM_ARRAY_SIZE_MAC(CPSS_DXCH3_POLICER_LM_CNTR_CAPTURE_MODE_ENT);
char * prvCpssLogEnum_CPSS_DXCH3_POLICER_NON_CONFORM_CMD_ENT[]  =
{
    "CPSS_DXCH3_POLICER_NON_CONFORM_CMD_NO_CHANGE_E",
    "CPSS_DXCH3_POLICER_NON_CONFORM_CMD_DROP_E",
    "CPSS_DXCH3_POLICER_NON_CONFORM_CMD_REMARK_E",
    "CPSS_DXCH3_POLICER_NON_CONFORM_CMD_REMARK_BY_ENTRY_E"
};
PRV_CPSS_LOG_STC_ENUM_ARRAY_SIZE_MAC(CPSS_DXCH3_POLICER_NON_CONFORM_CMD_ENT);
char * prvCpssLogEnum_CPSS_DXCH_POLICER_BILLING_INDEX_MODE_ENT[]  =
{
    "CPSS_DXCH_POLICER_BILLING_INDEX_MODE_STANDARD_E",
    "CPSS_DXCH_POLICER_BILLING_INDEX_MODE_FLOW_ID_E"
};
PRV_CPSS_LOG_STC_ENUM_ARRAY_SIZE_MAC(CPSS_DXCH_POLICER_BILLING_INDEX_MODE_ENT);
char * prvCpssLogEnum_CPSS_DXCH_POLICER_CMD_ENT[]  =
{
    "CPSS_DXCH_POLICER_CMD_NONE_E",
    "CPSS_DXCH_POLICER_CMD_DROP_RED_E",
    "CPSS_DXCH_POLICER_CMD_QOS_MARK_BY_ENTRY_E",
    "CPSS_DXCH_POLICER_CMD_QOS_PROFILE_MARK_BY_TABLE_E"
};
PRV_CPSS_LOG_STC_ENUM_ARRAY_SIZE_MAC(CPSS_DXCH_POLICER_CMD_ENT);
char * prvCpssLogEnum_CPSS_DXCH_POLICER_MODIFY_DSCP_ENT[]  =
{
    "CPSS_DXCH_POLICER_MODIFY_DSCP_KEEP_PREVIOUS_E",
    "CPSS_DXCH_POLICER_MODIFY_DSCP_DISABLE_E",
    "CPSS_DXCH_POLICER_MODIFY_DSCP_ENABLE_E",
    "CPSS_DXCH_POLICER_MODIFY_DSCP_ENABLE_INNER_E"
};
PRV_CPSS_LOG_STC_ENUM_ARRAY_SIZE_MAC(CPSS_DXCH_POLICER_MODIFY_DSCP_ENT);
char * prvCpssLogEnum_CPSS_DXCH_POLICER_MODIFY_UP_ENT[]  =
{
    "CPSS_DXCH_POLICER_MODIFY_UP_KEEP_PREVIOUS_E",
    "CPSS_DXCH_POLICER_MODIFY_UP_DISABLE_E",
    "CPSS_DXCH_POLICER_MODIFY_UP_ENABLE_E",
    "CPSS_DXCH_POLICER_MODIFY_UP_ENABLE_TAG0_E"
};
PRV_CPSS_LOG_STC_ENUM_ARRAY_SIZE_MAC(CPSS_DXCH_POLICER_MODIFY_UP_ENT);
char * prvCpssLogEnum_CPSS_DXCH_POLICER_REMARK_MODE_ENT[]  =
{
    "CPSS_DXCH_POLICER_REMARK_MODE_L2_E",
    "CPSS_DXCH_POLICER_REMARK_MODE_L3_E"
};
PRV_CPSS_LOG_STC_ENUM_ARRAY_SIZE_MAC(CPSS_DXCH_POLICER_REMARK_MODE_ENT);


/********* structure fields log functions *********/

void prvCpssLogParamFuncStc_CPSS_DXCH3_POLICER_BILLING_ENTRY_STC_PTR
(
    IN CPSS_LOG_LIB_ENT         contextLib,
    IN CPSS_LOG_TYPE_ENT        logType,
    IN GT_CHAR_PTR              namePtr,
    IN void                   * fieldPtr,
    INOUT PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC     * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_START_FIELD_STC_MAC(CPSS_DXCH3_POLICER_BILLING_ENTRY_STC *, valPtr);
    PRV_CPSS_LOG_UNUSED_MAC(inOutParamInfoPtr);
    PRV_CPSS_LOG_STC_STC_MAC(valPtr, greenCntr, GT_U64);
    PRV_CPSS_LOG_STC_STC_MAC(valPtr, yellowCntr, GT_U64);
    PRV_CPSS_LOG_STC_STC_MAC(valPtr, redCntr, GT_U64);
    PRV_CPSS_LOG_STC_ENUM_MAC(valPtr, billingCntrMode, CPSS_DXCH3_POLICER_BILLING_CNTR_MODE_ENT);
    PRV_CPSS_LOG_STC_BOOL_MAC(valPtr, billingCntrAllEnable);
    PRV_CPSS_LOG_STC_ENUM_MAC(valPtr, lmCntrCaptureMode, CPSS_DXCH3_POLICER_LM_CNTR_CAPTURE_MODE_ENT);
    PRV_CPSS_LOG_STC_STC_MAC(valPtr, greenCntrSnapshot, GT_U64);
    PRV_CPSS_LOG_STC_BOOL_MAC(valPtr, greenCntrSnapshotValid);
    PRV_CPSS_LOG_STC_ENUM_MAC(valPtr, packetSizeMode, CPSS_POLICER_PACKET_SIZE_MODE_ENT);
    PRV_CPSS_LOG_STC_ENUM_MAC(valPtr, tunnelTerminationPacketSizeMode, CPSS_DXCH3_POLICER_TT_PACKET_SIZE_MODE_ENT);
    PRV_CPSS_LOG_STC_ENUM_MAC(valPtr, dsaTagCountingMode, CPSS_DXCH3_POLICER_DSA_TAG_COUNTING_MODE_ENT);
    PRV_CPSS_LOG_STC_ENUM_MAC(valPtr, timeStampCountingMode, CPSS_DXCH3_POLICER_TS_TAG_COUNTING_MODE_ENT);
    prvCpssLogStcLogEnd(contextLib, logType);
}
void prvCpssLogParamFuncStc_CPSS_DXCH3_POLICER_MNG_CNTR_ENTRY_STC_PTR
(
    IN CPSS_LOG_LIB_ENT         contextLib,
    IN CPSS_LOG_TYPE_ENT        logType,
    IN GT_CHAR_PTR              namePtr,
    IN void                   * fieldPtr,
    INOUT PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC     * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_START_FIELD_STC_MAC(CPSS_DXCH3_POLICER_MNG_CNTR_ENTRY_STC *, valPtr);
    PRV_CPSS_LOG_UNUSED_MAC(inOutParamInfoPtr);
    PRV_CPSS_LOG_STC_STC_MAC(valPtr, duMngCntr, GT_U64);
    PRV_CPSS_LOG_STC_NUMBER_MAC(valPtr, packetMngCntr);
    prvCpssLogStcLogEnd(contextLib, logType);
}
void prvCpssLogParamFuncStc_CPSS_DXCH_POLICER_BILLING_INDEX_CFG_STC_PTR
(
    IN CPSS_LOG_LIB_ENT         contextLib,
    IN CPSS_LOG_TYPE_ENT        logType,
    IN GT_CHAR_PTR              namePtr,
    IN void                   * fieldPtr,
    INOUT PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC     * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_START_FIELD_STC_MAC(CPSS_DXCH_POLICER_BILLING_INDEX_CFG_STC *, valPtr);
    PRV_CPSS_LOG_UNUSED_MAC(inOutParamInfoPtr);
    PRV_CPSS_LOG_STC_ENUM_MAC(valPtr, billingIndexMode, CPSS_DXCH_POLICER_BILLING_INDEX_MODE_ENT);
    PRV_CPSS_LOG_STC_NUMBER_MAC(valPtr, billingFlowIdIndexBase);
    PRV_CPSS_LOG_STC_NUMBER_MAC(valPtr, billingMinFlowId);
    PRV_CPSS_LOG_STC_NUMBER_MAC(valPtr, billingMaxFlowId);
    prvCpssLogStcLogEnd(contextLib, logType);
}
void prvCpssLogParamFuncStc_CPSS_DXCH_POLICER_BUCKETS_STATE_STC_PTR
(
    IN CPSS_LOG_LIB_ENT         contextLib,
    IN CPSS_LOG_TYPE_ENT        logType,
    IN GT_CHAR_PTR              namePtr,
    IN void                   * fieldPtr,
    INOUT PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC     * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_START_FIELD_STC_MAC(CPSS_DXCH_POLICER_BUCKETS_STATE_STC *, valPtr);
    PRV_CPSS_LOG_UNUSED_MAC(inOutParamInfoPtr);
    PRV_CPSS_LOG_STC_NUMBER_MAC(valPtr, lastTimeUpdate0);
    PRV_CPSS_LOG_STC_NUMBER_MAC(valPtr, lastTimeUpdate1);
    PRV_CPSS_LOG_STC_BOOL_MAC(valPtr, wrapAround0);
    PRV_CPSS_LOG_STC_BOOL_MAC(valPtr, wrapAround1);
    PRV_CPSS_LOG_STC_NUMBER_MAC(valPtr, bucketSize0);
    PRV_CPSS_LOG_STC_NUMBER_MAC(valPtr, bucketSize1);
    PRV_CPSS_LOG_STC_BOOL_MAC(valPtr, bucketSignPositive0);
    PRV_CPSS_LOG_STC_BOOL_MAC(valPtr, bucketSignPositive1);
    prvCpssLogStcLogEnd(contextLib, logType);
}
void prvCpssLogParamFuncStc_CPSS_DXCH_POLICER_COUNTERS_STC_PTR
(
    IN CPSS_LOG_LIB_ENT         contextLib,
    IN CPSS_LOG_TYPE_ENT        logType,
    IN GT_CHAR_PTR              namePtr,
    IN void                   * fieldPtr,
    INOUT PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC     * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_START_FIELD_STC_MAC(CPSS_DXCH_POLICER_COUNTERS_STC *, valPtr);
    PRV_CPSS_LOG_UNUSED_MAC(inOutParamInfoPtr);
    PRV_CPSS_LOG_STC_NUMBER_MAC(valPtr, outOfProfileBytesCnt);
    PRV_CPSS_LOG_STC_NUMBER_MAC(valPtr, inProfileBytesCnt);
    prvCpssLogStcLogEnd(contextLib, logType);
}
void prvCpssLogParamFuncStc_CPSS_DXCH_POLICER_ENTRY_STC_PTR
(
    IN CPSS_LOG_LIB_ENT         contextLib,
    IN CPSS_LOG_TYPE_ENT        logType,
    IN GT_CHAR_PTR              namePtr,
    IN void                   * fieldPtr,
    INOUT PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC     * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_START_FIELD_STC_MAC(CPSS_DXCH_POLICER_ENTRY_STC *, valPtr);
    PRV_CPSS_LOG_UNUSED_MAC(inOutParamInfoPtr);
    PRV_CPSS_LOG_STC_BOOL_MAC(valPtr, policerEnable);
    PRV_CPSS_LOG_STC_ENUM_MAC(valPtr, meterColorMode, CPSS_POLICER_COLOR_MODE_ENT);
    PRV_CPSS_LOG_STC_STC_MAC(valPtr, tbParams, CPSS_DXCH_POLICER_TB_PARAMS_STC);
    PRV_CPSS_LOG_STC_BOOL_MAC(valPtr, counterEnable);
    PRV_CPSS_LOG_STC_NUMBER_MAC(valPtr, counterSetIndex);
    PRV_CPSS_LOG_STC_ENUM_MAC(valPtr, cmd, CPSS_DXCH_POLICER_CMD_ENT);
    PRV_CPSS_LOG_STC_NUMBER_MAC(valPtr, qosProfile);
    PRV_CPSS_LOG_STC_ENUM_MAC(valPtr, modifyDscp, CPSS_PACKET_ATTRIBUTE_MODIFY_TYPE_ENT);
    PRV_CPSS_LOG_STC_ENUM_MAC(valPtr, modifyUp, CPSS_PACKET_ATTRIBUTE_MODIFY_TYPE_ENT);
    prvCpssLogStcLogEnd(contextLib, logType);
}
void prvCpssLogParamFuncStc_CPSS_DXCH_POLICER_HIERARCHICAL_TABLE_ENTRY_STC_PTR
(
    IN CPSS_LOG_LIB_ENT         contextLib,
    IN CPSS_LOG_TYPE_ENT        logType,
    IN GT_CHAR_PTR              namePtr,
    IN void                   * fieldPtr,
    INOUT PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC     * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_START_FIELD_STC_MAC(CPSS_DXCH_POLICER_HIERARCHICAL_TABLE_ENTRY_STC *, valPtr);
    PRV_CPSS_LOG_UNUSED_MAC(inOutParamInfoPtr);
    PRV_CPSS_LOG_STC_BOOL_MAC(valPtr, countingEnable);
    PRV_CPSS_LOG_STC_BOOL_MAC(valPtr, meteringEnable);
    PRV_CPSS_LOG_STC_NUMBER_MAC(valPtr, policerPointer);
    prvCpssLogStcLogEnd(contextLib, logType);
}
void prvCpssLogParamFuncStc_CPSS_DXCH_POLICER_QOS_PARAM_STC_PTR
(
    IN CPSS_LOG_LIB_ENT         contextLib,
    IN CPSS_LOG_TYPE_ENT        logType,
    IN GT_CHAR_PTR              namePtr,
    IN void                   * fieldPtr,
    INOUT PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC     * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_START_FIELD_STC_MAC(CPSS_DXCH_POLICER_QOS_PARAM_STC *, valPtr);
    PRV_CPSS_LOG_UNUSED_MAC(inOutParamInfoPtr);
    PRV_CPSS_LOG_STC_NUMBER_MAC(valPtr, up);
    PRV_CPSS_LOG_STC_NUMBER_MAC(valPtr, dscp);
    PRV_CPSS_LOG_STC_NUMBER_MAC(valPtr, exp);
    PRV_CPSS_LOG_STC_ENUM_MAC(valPtr, dp, CPSS_DP_LEVEL_ENT);
    prvCpssLogStcLogEnd(contextLib, logType);
}
void prvCpssLogParamFuncStc_CPSS_DXCH_POLICER_TB_PARAMS_STC_PTR
(
    IN CPSS_LOG_LIB_ENT         contextLib,
    IN CPSS_LOG_TYPE_ENT        logType,
    IN GT_CHAR_PTR              namePtr,
    IN void                   * fieldPtr,
    INOUT PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC     * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_START_FIELD_STC_MAC(CPSS_DXCH_POLICER_TB_PARAMS_STC *, valPtr);
    PRV_CPSS_LOG_UNUSED_MAC(inOutParamInfoPtr);
    PRV_CPSS_LOG_STC_NUMBER_MAC(valPtr, cir);
    PRV_CPSS_LOG_STC_NUMBER_MAC(valPtr, cbs);
    prvCpssLogStcLogEnd(contextLib, logType);
}
void prvCpssLogParamFuncStc_CPSS_DXCH_POLICER_TRIGGER_ENTRY_STC_PTR
(
    IN CPSS_LOG_LIB_ENT         contextLib,
    IN CPSS_LOG_TYPE_ENT        logType,
    IN GT_CHAR_PTR              namePtr,
    IN void                   * fieldPtr,
    INOUT PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC     * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_START_FIELD_STC_MAC(CPSS_DXCH_POLICER_TRIGGER_ENTRY_STC *, valPtr);
    PRV_CPSS_LOG_UNUSED_MAC(inOutParamInfoPtr);
    PRV_CPSS_LOG_STC_BOOL_MAC(valPtr, meteringEnable);
    PRV_CPSS_LOG_STC_BOOL_MAC(valPtr, countingEnable);
    PRV_CPSS_LOG_STC_NUMBER_MAC(valPtr, policerIndex);
    PRV_CPSS_LOG_STC_BOOL_MAC(valPtr, ucKnownEnable);
    PRV_CPSS_LOG_STC_NUMBER_MAC(valPtr, ucKnownOffset);
    PRV_CPSS_LOG_STC_BOOL_MAC(valPtr, ucUnknownEnable);
    PRV_CPSS_LOG_STC_NUMBER_MAC(valPtr, ucUnknownOffset);
    PRV_CPSS_LOG_STC_BOOL_MAC(valPtr, mcRegisteredEnable);
    PRV_CPSS_LOG_STC_NUMBER_MAC(valPtr, mcRegisteredOffset);
    PRV_CPSS_LOG_STC_BOOL_MAC(valPtr, mcUnregisteredEnable);
    PRV_CPSS_LOG_STC_NUMBER_MAC(valPtr, mcUnregisteredOffset);
    PRV_CPSS_LOG_STC_BOOL_MAC(valPtr, bcEnable);
    PRV_CPSS_LOG_STC_NUMBER_MAC(valPtr, bcOffset);
    PRV_CPSS_LOG_STC_BOOL_MAC(valPtr, tcpSynEnable);
    PRV_CPSS_LOG_STC_NUMBER_MAC(valPtr, tcpSynOffset);
    prvCpssLogStcLogEnd(contextLib, logType);
}


/********* parameters log functions *********/

void prvCpssLogParamFunc_CPSS_DXCH3_POLICER_BILLING_CNTR_MODE_ENT(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_SET_PARAM_VAL_MAC(CPSS_DXCH3_POLICER_BILLING_CNTR_MODE_ENT, paramVal);
    PRV_CPSS_LOG_ENUM_MAC(namePtr, paramVal, CPSS_DXCH3_POLICER_BILLING_CNTR_MODE_ENT);
}
void prvCpssLogParamFunc_CPSS_DXCH3_POLICER_BILLING_CNTR_MODE_ENT_PTR(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_SET_PARAM_VAL_MAC(CPSS_DXCH3_POLICER_BILLING_CNTR_MODE_ENT*, paramVal);
    if (paramVal == NULL)
    {
        PRV_CPSS_LOG_AND_HISTORY_PARAM_MAC(contextLib, logType, "%s = NULL\n", namePtr);
        return;
    }
    PRV_CPSS_LOG_ENUM_MAC(namePtr, *paramVal, CPSS_DXCH3_POLICER_BILLING_CNTR_MODE_ENT);
}
void prvCpssLogParamFunc_CPSS_DXCH3_POLICER_BILLING_ENTRY_STC_PTR(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_START_PARAM_STC_MAC(CPSS_DXCH3_POLICER_BILLING_ENTRY_STC*, paramVal);
    prvCpssLogParamFuncStc_CPSS_DXCH3_POLICER_BILLING_ENTRY_STC_PTR(contextLib, logType, namePtr, paramVal, inOutParamInfoPtr);
}
void prvCpssLogParamFunc_CPSS_DXCH3_POLICER_COLOR_COUNT_MODE_ENT(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_SET_PARAM_VAL_MAC(CPSS_DXCH3_POLICER_COLOR_COUNT_MODE_ENT, paramVal);
    PRV_CPSS_LOG_ENUM_MAC(namePtr, paramVal, CPSS_DXCH3_POLICER_COLOR_COUNT_MODE_ENT);
}
void prvCpssLogParamFunc_CPSS_DXCH3_POLICER_COLOR_COUNT_MODE_ENT_PTR(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_SET_PARAM_VAL_MAC(CPSS_DXCH3_POLICER_COLOR_COUNT_MODE_ENT*, paramVal);
    if (paramVal == NULL)
    {
        PRV_CPSS_LOG_AND_HISTORY_PARAM_MAC(contextLib, logType, "%s = NULL\n", namePtr);
        return;
    }
    PRV_CPSS_LOG_ENUM_MAC(namePtr, *paramVal, CPSS_DXCH3_POLICER_COLOR_COUNT_MODE_ENT);
}
void prvCpssLogParamFunc_CPSS_DXCH3_POLICER_ENTRY_TYPE_ENT_PTR(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_SET_PARAM_VAL_MAC(CPSS_DXCH3_POLICER_ENTRY_TYPE_ENT*, paramVal);
    if (paramVal == NULL)
    {
        PRV_CPSS_LOG_AND_HISTORY_PARAM_MAC(contextLib, logType, "%s = NULL\n", namePtr);
        return;
    }
    PRV_CPSS_LOG_ENUM_MAC(namePtr, *paramVal, CPSS_DXCH3_POLICER_ENTRY_TYPE_ENT);
}
void prvCpssLogParamFunc_CPSS_DXCH3_POLICER_METER_MODE_ENT(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_SET_PARAM_VAL_MAC(CPSS_DXCH3_POLICER_METER_MODE_ENT, paramVal);
    PRV_CPSS_LOG_ENUM_MAC(namePtr, paramVal, CPSS_DXCH3_POLICER_METER_MODE_ENT);
}
void prvCpssLogParamFunc_CPSS_DXCH3_POLICER_METER_RESOLUTION_ENT(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_SET_PARAM_VAL_MAC(CPSS_DXCH3_POLICER_METER_RESOLUTION_ENT, paramVal);
    PRV_CPSS_LOG_ENUM_MAC(namePtr, paramVal, CPSS_DXCH3_POLICER_METER_RESOLUTION_ENT);
}
void prvCpssLogParamFunc_CPSS_DXCH3_POLICER_METER_RESOLUTION_ENT_PTR(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_SET_PARAM_VAL_MAC(CPSS_DXCH3_POLICER_METER_RESOLUTION_ENT*, paramVal);
    if (paramVal == NULL)
    {
        PRV_CPSS_LOG_AND_HISTORY_PARAM_MAC(contextLib, logType, "%s = NULL\n", namePtr);
        return;
    }
    PRV_CPSS_LOG_ENUM_MAC(namePtr, *paramVal, CPSS_DXCH3_POLICER_METER_RESOLUTION_ENT);
}
void prvCpssLogParamFunc_CPSS_DXCH3_POLICER_MNG_CNTR_ENTRY_STC_PTR(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_START_PARAM_STC_MAC(CPSS_DXCH3_POLICER_MNG_CNTR_ENTRY_STC*, paramVal);
    prvCpssLogParamFuncStc_CPSS_DXCH3_POLICER_MNG_CNTR_ENTRY_STC_PTR(contextLib, logType, namePtr, paramVal, inOutParamInfoPtr);
}
void prvCpssLogParamFunc_CPSS_DXCH3_POLICER_MNG_CNTR_RESOLUTION_ENT(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_SET_PARAM_VAL_MAC(CPSS_DXCH3_POLICER_MNG_CNTR_RESOLUTION_ENT, paramVal);
    PRV_CPSS_LOG_ENUM_MAC(namePtr, paramVal, CPSS_DXCH3_POLICER_MNG_CNTR_RESOLUTION_ENT);
}
void prvCpssLogParamFunc_CPSS_DXCH3_POLICER_MNG_CNTR_RESOLUTION_ENT_PTR(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_SET_PARAM_VAL_MAC(CPSS_DXCH3_POLICER_MNG_CNTR_RESOLUTION_ENT*, paramVal);
    if (paramVal == NULL)
    {
        PRV_CPSS_LOG_AND_HISTORY_PARAM_MAC(contextLib, logType, "%s = NULL\n", namePtr);
        return;
    }
    PRV_CPSS_LOG_ENUM_MAC(namePtr, *paramVal, CPSS_DXCH3_POLICER_MNG_CNTR_RESOLUTION_ENT);
}
void prvCpssLogParamFunc_CPSS_DXCH3_POLICER_MNG_CNTR_SET_ENT(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_SET_PARAM_VAL_MAC(CPSS_DXCH3_POLICER_MNG_CNTR_SET_ENT, paramVal);
    PRV_CPSS_LOG_ENUM_MAC(namePtr, paramVal, CPSS_DXCH3_POLICER_MNG_CNTR_SET_ENT);
}
void prvCpssLogParamFunc_CPSS_DXCH3_POLICER_MNG_CNTR_TYPE_ENT(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_SET_PARAM_VAL_MAC(CPSS_DXCH3_POLICER_MNG_CNTR_TYPE_ENT, paramVal);
    PRV_CPSS_LOG_ENUM_MAC(namePtr, paramVal, CPSS_DXCH3_POLICER_MNG_CNTR_TYPE_ENT);
}
void prvCpssLogParamFunc_CPSS_DXCH3_POLICER_TS_TAG_COUNTING_MODE_ENT(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_SET_PARAM_VAL_MAC(CPSS_DXCH3_POLICER_TS_TAG_COUNTING_MODE_ENT, paramVal);
    PRV_CPSS_LOG_ENUM_MAC(namePtr, paramVal, CPSS_DXCH3_POLICER_TS_TAG_COUNTING_MODE_ENT);
}
void prvCpssLogParamFunc_CPSS_DXCH3_POLICER_TS_TAG_COUNTING_MODE_ENT_PTR(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_SET_PARAM_VAL_MAC(CPSS_DXCH3_POLICER_TS_TAG_COUNTING_MODE_ENT*, paramVal);
    if (paramVal == NULL)
    {
        PRV_CPSS_LOG_AND_HISTORY_PARAM_MAC(contextLib, logType, "%s = NULL\n", namePtr);
        return;
    }
    PRV_CPSS_LOG_ENUM_MAC(namePtr, *paramVal, CPSS_DXCH3_POLICER_TS_TAG_COUNTING_MODE_ENT);
}
void prvCpssLogParamFunc_CPSS_DXCH3_POLICER_TT_PACKET_SIZE_MODE_ENT(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_SET_PARAM_VAL_MAC(CPSS_DXCH3_POLICER_TT_PACKET_SIZE_MODE_ENT, paramVal);
    PRV_CPSS_LOG_ENUM_MAC(namePtr, paramVal, CPSS_DXCH3_POLICER_TT_PACKET_SIZE_MODE_ENT);
}
void prvCpssLogParamFunc_CPSS_DXCH3_POLICER_TT_PACKET_SIZE_MODE_ENT_PTR(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_SET_PARAM_VAL_MAC(CPSS_DXCH3_POLICER_TT_PACKET_SIZE_MODE_ENT*, paramVal);
    if (paramVal == NULL)
    {
        PRV_CPSS_LOG_AND_HISTORY_PARAM_MAC(contextLib, logType, "%s = NULL\n", namePtr);
        return;
    }
    PRV_CPSS_LOG_ENUM_MAC(namePtr, *paramVal, CPSS_DXCH3_POLICER_TT_PACKET_SIZE_MODE_ENT);
}
void prvCpssLogParamFunc_CPSS_DXCH_POLICER_BILLING_INDEX_CFG_STC_PTR(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_START_PARAM_STC_MAC(CPSS_DXCH_POLICER_BILLING_INDEX_CFG_STC*, paramVal);
    prvCpssLogParamFuncStc_CPSS_DXCH_POLICER_BILLING_INDEX_CFG_STC_PTR(contextLib, logType, namePtr, paramVal, inOutParamInfoPtr);
}
void prvCpssLogParamFunc_CPSS_DXCH_POLICER_BUCKETS_STATE_STC_PTR(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_START_PARAM_STC_MAC(CPSS_DXCH_POLICER_BUCKETS_STATE_STC*, paramVal);
    prvCpssLogParamFuncStc_CPSS_DXCH_POLICER_BUCKETS_STATE_STC_PTR(contextLib, logType, namePtr, paramVal, inOutParamInfoPtr);
}
void prvCpssLogParamFunc_CPSS_DXCH_POLICER_COUNTERS_STC_PTR(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_START_PARAM_STC_MAC(CPSS_DXCH_POLICER_COUNTERS_STC*, paramVal);
    prvCpssLogParamFuncStc_CPSS_DXCH_POLICER_COUNTERS_STC_PTR(contextLib, logType, namePtr, paramVal, inOutParamInfoPtr);
}
void prvCpssLogParamFunc_CPSS_DXCH_POLICER_COUNTING_MODE_ENT(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_SET_PARAM_VAL_MAC(CPSS_DXCH_POLICER_COUNTING_MODE_ENT, paramVal);
    PRV_CPSS_LOG_ENUM_MAC(namePtr, paramVal, CPSS_DXCH_POLICER_COUNTING_MODE_ENT);
}
void prvCpssLogParamFunc_CPSS_DXCH_POLICER_COUNTING_MODE_ENT_PTR(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_SET_PARAM_VAL_MAC(CPSS_DXCH_POLICER_COUNTING_MODE_ENT*, paramVal);
    if (paramVal == NULL)
    {
        PRV_CPSS_LOG_AND_HISTORY_PARAM_MAC(contextLib, logType, "%s = NULL\n", namePtr);
        return;
    }
    PRV_CPSS_LOG_ENUM_MAC(namePtr, *paramVal, CPSS_DXCH_POLICER_COUNTING_MODE_ENT);
}
void prvCpssLogParamFunc_CPSS_DXCH_POLICER_ENTRY_STC_PTR(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_START_PARAM_STC_MAC(CPSS_DXCH_POLICER_ENTRY_STC*, paramVal);
    prvCpssLogParamFuncStc_CPSS_DXCH_POLICER_ENTRY_STC_PTR(contextLib, logType, namePtr, paramVal, inOutParamInfoPtr);
}
void prvCpssLogParamFunc_CPSS_DXCH_POLICER_ENVELOPE_MAX_SIZE_ENT(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_SET_PARAM_VAL_MAC(CPSS_DXCH_POLICER_ENVELOPE_MAX_SIZE_ENT, paramVal);
    PRV_CPSS_LOG_ENUM_MAC(namePtr, paramVal, CPSS_DXCH_POLICER_ENVELOPE_MAX_SIZE_ENT);
}
void prvCpssLogParamFunc_CPSS_DXCH_POLICER_ENVELOPE_MAX_SIZE_ENT_PTR(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_SET_PARAM_VAL_MAC(CPSS_DXCH_POLICER_ENVELOPE_MAX_SIZE_ENT*, paramVal);
    if (paramVal == NULL)
    {
        PRV_CPSS_LOG_AND_HISTORY_PARAM_MAC(contextLib, logType, "%s = NULL\n", namePtr);
        return;
    }
    PRV_CPSS_LOG_ENUM_MAC(namePtr, *paramVal, CPSS_DXCH_POLICER_ENVELOPE_MAX_SIZE_ENT);
}
void prvCpssLogParamFunc_CPSS_DXCH_POLICER_E_ATTR_METER_MODE_ENT(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_SET_PARAM_VAL_MAC(CPSS_DXCH_POLICER_E_ATTR_METER_MODE_ENT, paramVal);
    PRV_CPSS_LOG_ENUM_MAC(namePtr, paramVal, CPSS_DXCH_POLICER_E_ATTR_METER_MODE_ENT);
}
void prvCpssLogParamFunc_CPSS_DXCH_POLICER_E_ATTR_METER_MODE_ENT_PTR(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_SET_PARAM_VAL_MAC(CPSS_DXCH_POLICER_E_ATTR_METER_MODE_ENT*, paramVal);
    if (paramVal == NULL)
    {
        PRV_CPSS_LOG_AND_HISTORY_PARAM_MAC(contextLib, logType, "%s = NULL\n", namePtr);
        return;
    }
    PRV_CPSS_LOG_ENUM_MAC(namePtr, *paramVal, CPSS_DXCH_POLICER_E_ATTR_METER_MODE_ENT);
}
void prvCpssLogParamFunc_CPSS_DXCH_POLICER_HIERARCHICAL_TABLE_ENTRY_STC_PTR(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_START_PARAM_STC_MAC(CPSS_DXCH_POLICER_HIERARCHICAL_TABLE_ENTRY_STC*, paramVal);
    prvCpssLogParamFuncStc_CPSS_DXCH_POLICER_HIERARCHICAL_TABLE_ENTRY_STC_PTR(contextLib, logType, namePtr, paramVal, inOutParamInfoPtr);
}
void prvCpssLogParamFunc_CPSS_DXCH_POLICER_L2_REMARK_MODEL_ENT(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_SET_PARAM_VAL_MAC(CPSS_DXCH_POLICER_L2_REMARK_MODEL_ENT, paramVal);
    PRV_CPSS_LOG_ENUM_MAC(namePtr, paramVal, CPSS_DXCH_POLICER_L2_REMARK_MODEL_ENT);
}
void prvCpssLogParamFunc_CPSS_DXCH_POLICER_L2_REMARK_MODEL_ENT_PTR(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_SET_PARAM_VAL_MAC(CPSS_DXCH_POLICER_L2_REMARK_MODEL_ENT*, paramVal);
    if (paramVal == NULL)
    {
        PRV_CPSS_LOG_AND_HISTORY_PARAM_MAC(contextLib, logType, "%s = NULL\n", namePtr);
        return;
    }
    PRV_CPSS_LOG_ENUM_MAC(namePtr, *paramVal, CPSS_DXCH_POLICER_L2_REMARK_MODEL_ENT);
}
void prvCpssLogParamFunc_CPSS_DXCH_POLICER_MEMORY_CTRL_MODE_ENT(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_SET_PARAM_VAL_MAC(CPSS_DXCH_POLICER_MEMORY_CTRL_MODE_ENT, paramVal);
    PRV_CPSS_LOG_ENUM_MAC(namePtr, paramVal, CPSS_DXCH_POLICER_MEMORY_CTRL_MODE_ENT);
}
void prvCpssLogParamFunc_CPSS_DXCH_POLICER_MEMORY_CTRL_MODE_ENT_PTR(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_SET_PARAM_VAL_MAC(CPSS_DXCH_POLICER_MEMORY_CTRL_MODE_ENT*, paramVal);
    if (paramVal == NULL)
    {
        PRV_CPSS_LOG_AND_HISTORY_PARAM_MAC(contextLib, logType, "%s = NULL\n", namePtr);
        return;
    }
    PRV_CPSS_LOG_ENUM_MAC(namePtr, *paramVal, CPSS_DXCH_POLICER_MEMORY_CTRL_MODE_ENT);
}
void prvCpssLogParamFunc_CPSS_DXCH_POLICER_METERING_CALC_METHOD_ENT(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_SET_PARAM_VAL_MAC(CPSS_DXCH_POLICER_METERING_CALC_METHOD_ENT, paramVal);
    PRV_CPSS_LOG_ENUM_MAC(namePtr, paramVal, CPSS_DXCH_POLICER_METERING_CALC_METHOD_ENT);
}
void prvCpssLogParamFunc_CPSS_DXCH_POLICER_METERING_CALC_METHOD_ENT_PTR(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_SET_PARAM_VAL_MAC(CPSS_DXCH_POLICER_METERING_CALC_METHOD_ENT*, paramVal);
    if (paramVal == NULL)
    {
        PRV_CPSS_LOG_AND_HISTORY_PARAM_MAC(contextLib, logType, "%s = NULL\n", namePtr);
        return;
    }
    PRV_CPSS_LOG_ENUM_MAC(namePtr, *paramVal, CPSS_DXCH_POLICER_METERING_CALC_METHOD_ENT);
}
void prvCpssLogParamFunc_CPSS_DXCH_POLICER_PORT_MODE_ADDR_SELECT_TYPE_ENT(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_SET_PARAM_VAL_MAC(CPSS_DXCH_POLICER_PORT_MODE_ADDR_SELECT_TYPE_ENT, paramVal);
    PRV_CPSS_LOG_ENUM_MAC(namePtr, paramVal, CPSS_DXCH_POLICER_PORT_MODE_ADDR_SELECT_TYPE_ENT);
}
void prvCpssLogParamFunc_CPSS_DXCH_POLICER_PORT_MODE_ADDR_SELECT_TYPE_ENT_PTR(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_SET_PARAM_VAL_MAC(CPSS_DXCH_POLICER_PORT_MODE_ADDR_SELECT_TYPE_ENT*, paramVal);
    if (paramVal == NULL)
    {
        PRV_CPSS_LOG_AND_HISTORY_PARAM_MAC(contextLib, logType, "%s = NULL\n", namePtr);
        return;
    }
    PRV_CPSS_LOG_ENUM_MAC(namePtr, *paramVal, CPSS_DXCH_POLICER_PORT_MODE_ADDR_SELECT_TYPE_ENT);
}
void prvCpssLogParamFunc_CPSS_DXCH_POLICER_QOS_PARAM_STC_PTR(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_START_PARAM_STC_MAC(CPSS_DXCH_POLICER_QOS_PARAM_STC*, paramVal);
    prvCpssLogParamFuncStc_CPSS_DXCH_POLICER_QOS_PARAM_STC_PTR(contextLib, logType, namePtr, paramVal, inOutParamInfoPtr);
}
void prvCpssLogParamFunc_CPSS_DXCH_POLICER_REMARK_TABLE_TYPE_ENT(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_SET_PARAM_VAL_MAC(CPSS_DXCH_POLICER_REMARK_TABLE_TYPE_ENT, paramVal);
    PRV_CPSS_LOG_ENUM_MAC(namePtr, paramVal, CPSS_DXCH_POLICER_REMARK_TABLE_TYPE_ENT);
}
void prvCpssLogParamFunc_CPSS_DXCH_POLICER_STAGE_METER_MODE_ENT(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_SET_PARAM_VAL_MAC(CPSS_DXCH_POLICER_STAGE_METER_MODE_ENT, paramVal);
    PRV_CPSS_LOG_ENUM_MAC(namePtr, paramVal, CPSS_DXCH_POLICER_STAGE_METER_MODE_ENT);
}
void prvCpssLogParamFunc_CPSS_DXCH_POLICER_STAGE_METER_MODE_ENT_PTR(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_SET_PARAM_VAL_MAC(CPSS_DXCH_POLICER_STAGE_METER_MODE_ENT*, paramVal);
    if (paramVal == NULL)
    {
        PRV_CPSS_LOG_AND_HISTORY_PARAM_MAC(contextLib, logType, "%s = NULL\n", namePtr);
        return;
    }
    PRV_CPSS_LOG_ENUM_MAC(namePtr, *paramVal, CPSS_DXCH_POLICER_STAGE_METER_MODE_ENT);
}
void prvCpssLogParamFunc_CPSS_DXCH_POLICER_STAGE_TYPE_ENT(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_SET_PARAM_VAL_MAC(CPSS_DXCH_POLICER_STAGE_TYPE_ENT, paramVal);
    PRV_CPSS_LOG_ENUM_MAC(namePtr, paramVal, CPSS_DXCH_POLICER_STAGE_TYPE_ENT);
}
void prvCpssLogParamFunc_CPSS_DXCH_POLICER_STORM_TYPE_ENT(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_SET_PARAM_VAL_MAC(CPSS_DXCH_POLICER_STORM_TYPE_ENT, paramVal);
    PRV_CPSS_LOG_ENUM_MAC(namePtr, paramVal, CPSS_DXCH_POLICER_STORM_TYPE_ENT);
}
void prvCpssLogParamFunc_CPSS_DXCH_POLICER_TB_PARAMS_STC_PTR(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_START_PARAM_STC_MAC(CPSS_DXCH_POLICER_TB_PARAMS_STC*, paramVal);
    prvCpssLogParamFuncStc_CPSS_DXCH_POLICER_TB_PARAMS_STC_PTR(contextLib, logType, namePtr, paramVal, inOutParamInfoPtr);
}
void prvCpssLogParamFunc_CPSS_DXCH_POLICER_TRIGGER_ENTRY_STC_PTR(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_START_PARAM_STC_MAC(CPSS_DXCH_POLICER_TRIGGER_ENTRY_STC*, paramVal);
    prvCpssLogParamFuncStc_CPSS_DXCH_POLICER_TRIGGER_ENTRY_STC_PTR(contextLib, logType, namePtr, paramVal, inOutParamInfoPtr);
}
void prvCpssLogParamFunc_CPSS_DXCH_POLICER_VLAN_CNTR_MODE_ENT(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_SET_PARAM_VAL_MAC(CPSS_DXCH_POLICER_VLAN_CNTR_MODE_ENT, paramVal);
    PRV_CPSS_LOG_ENUM_MAC(namePtr, paramVal, CPSS_DXCH_POLICER_VLAN_CNTR_MODE_ENT);
}
void prvCpssLogParamFunc_CPSS_DXCH_POLICER_VLAN_CNTR_MODE_ENT_PTR(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_SET_PARAM_VAL_MAC(CPSS_DXCH_POLICER_VLAN_CNTR_MODE_ENT*, paramVal);
    if (paramVal == NULL)
    {
        PRV_CPSS_LOG_AND_HISTORY_PARAM_MAC(contextLib, logType, "%s = NULL\n", namePtr);
        return;
    }
    PRV_CPSS_LOG_ENUM_MAC(namePtr, *paramVal, CPSS_DXCH_POLICER_VLAN_CNTR_MODE_ENT);
}


/********* API fields DB *********/

PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_DP_LEVEL_ENT_confLevel = {
     "confLevel", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_MAC(CPSS_DP_LEVEL_ENT)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_DROP_MODE_TYPE_ENT_dropRedMode = {
     "dropRedMode", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_MAC(CPSS_DROP_MODE_TYPE_ENT)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_DROP_MODE_TYPE_ENT_dropType = {
     "dropType", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_MAC(CPSS_DROP_MODE_TYPE_ENT)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_DXCH3_POLICER_BILLING_CNTR_MODE_ENT_cntrMode = {
     "cntrMode", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_MAC(CPSS_DXCH3_POLICER_BILLING_CNTR_MODE_ENT)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_DXCH3_POLICER_BILLING_ENTRY_STC_PTR_billingCntrPtr = {
     "billingCntrPtr", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(CPSS_DXCH3_POLICER_BILLING_ENTRY_STC)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_DXCH3_POLICER_COLOR_COUNT_MODE_ENT_mode = {
     "mode", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_MAC(CPSS_DXCH3_POLICER_COLOR_COUNT_MODE_ENT)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_DXCH3_POLICER_METERING_ENTRY_STC_PTR_entryArr = {
     "entryArr", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(CPSS_DXCH3_POLICER_METERING_ENTRY_STC)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_DXCH3_POLICER_METERING_ENTRY_STC_PTR_entryPtr = {
     "entryPtr", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(CPSS_DXCH3_POLICER_METERING_ENTRY_STC)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_DXCH3_POLICER_METER_MODE_ENT_meterMode = {
     "meterMode", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_MAC(CPSS_DXCH3_POLICER_METER_MODE_ENT)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_DXCH3_POLICER_METER_RESOLUTION_ENT_resolution = {
     "resolution", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_MAC(CPSS_DXCH3_POLICER_METER_RESOLUTION_ENT)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_DXCH3_POLICER_METER_TB_PARAMS_UNT_PTR_tbInParamsPtr = {
     "tbInParamsPtr", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(CPSS_DXCH3_POLICER_METER_TB_PARAMS_UNT)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_DXCH3_POLICER_MNG_CNTR_ENTRY_STC_PTR_mngCntrPtr = {
     "mngCntrPtr", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(CPSS_DXCH3_POLICER_MNG_CNTR_ENTRY_STC)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_DXCH3_POLICER_MNG_CNTR_RESOLUTION_ENT_cntrResolution = {
     "cntrResolution", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_MAC(CPSS_DXCH3_POLICER_MNG_CNTR_RESOLUTION_ENT)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_DXCH3_POLICER_MNG_CNTR_SET_ENT_cntrSet = {
     "cntrSet", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_MAC(CPSS_DXCH3_POLICER_MNG_CNTR_SET_ENT)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_DXCH3_POLICER_MNG_CNTR_SET_ENT_mngCntrSet = {
     "mngCntrSet", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_MAC(CPSS_DXCH3_POLICER_MNG_CNTR_SET_ENT)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_DXCH3_POLICER_MNG_CNTR_TYPE_ENT_mngCntrType = {
     "mngCntrType", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_MAC(CPSS_DXCH3_POLICER_MNG_CNTR_TYPE_ENT)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_DXCH3_POLICER_TS_TAG_COUNTING_MODE_ENT_timestampTagMode = {
     "timestampTagMode", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_MAC(CPSS_DXCH3_POLICER_TS_TAG_COUNTING_MODE_ENT)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_DXCH3_POLICER_TT_PACKET_SIZE_MODE_ENT_ttPacketSizeMode = {
     "ttPacketSizeMode", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_MAC(CPSS_DXCH3_POLICER_TT_PACKET_SIZE_MODE_ENT)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_DXCH_POLICER_BILLING_INDEX_CFG_STC_PTR_billingIndexCfgPtr = {
     "billingIndexCfgPtr", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(CPSS_DXCH_POLICER_BILLING_INDEX_CFG_STC)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_DXCH_POLICER_COUNTERS_STC_PTR_countersPtr = {
     "countersPtr", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(CPSS_DXCH_POLICER_COUNTERS_STC)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_DXCH_POLICER_COUNTING_MODE_ENT_mode = {
     "mode", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_MAC(CPSS_DXCH_POLICER_COUNTING_MODE_ENT)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_DXCH_POLICER_ENTRY_STC_PTR_entryPtr = {
     "entryPtr", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(CPSS_DXCH_POLICER_ENTRY_STC)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_DXCH_POLICER_ENVELOPE_MAX_SIZE_ENT_maxSize = {
     "maxSize", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_MAC(CPSS_DXCH_POLICER_ENVELOPE_MAX_SIZE_ENT)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_DXCH_POLICER_E_ATTR_METER_MODE_ENT_mode = {
     "mode", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_MAC(CPSS_DXCH_POLICER_E_ATTR_METER_MODE_ENT)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_DXCH_POLICER_HIERARCHICAL_TABLE_ENTRY_STC_PTR_entryPtr = {
     "entryPtr", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(CPSS_DXCH_POLICER_HIERARCHICAL_TABLE_ENTRY_STC)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_DXCH_POLICER_L2_REMARK_MODEL_ENT_model = {
     "model", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_MAC(CPSS_DXCH_POLICER_L2_REMARK_MODEL_ENT)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_DXCH_POLICER_MEMORY_CTRL_MODE_ENT_mode = {
     "mode", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_MAC(CPSS_DXCH_POLICER_MEMORY_CTRL_MODE_ENT)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_DXCH_POLICER_METERING_CALC_METHOD_ENT_meteringCalcMethod = {
     "meteringCalcMethod", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_MAC(CPSS_DXCH_POLICER_METERING_CALC_METHOD_ENT)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_DXCH_POLICER_PORT_MODE_ADDR_SELECT_TYPE_ENT_type = {
     "type", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_MAC(CPSS_DXCH_POLICER_PORT_MODE_ADDR_SELECT_TYPE_ENT)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_DXCH_POLICER_QOS_PARAM_STC_PTR_qosParamPtr = {
     "qosParamPtr", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(CPSS_DXCH_POLICER_QOS_PARAM_STC)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_DXCH_POLICER_REMARK_TABLE_TYPE_ENT_remarkTableType = {
     "remarkTableType", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_MAC(CPSS_DXCH_POLICER_REMARK_TABLE_TYPE_ENT)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_DXCH_POLICER_STAGE_METER_MODE_ENT_mode = {
     "mode", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_MAC(CPSS_DXCH_POLICER_STAGE_METER_MODE_ENT)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_DXCH_POLICER_STORM_TYPE_ENT_stormType = {
     "stormType", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_MAC(CPSS_DXCH_POLICER_STORM_TYPE_ENT)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_DXCH_POLICER_TB_PARAMS_STC_PTR_tbInParamsPtr = {
     "tbInParamsPtr", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(CPSS_DXCH_POLICER_TB_PARAMS_STC)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_DXCH_POLICER_TRIGGER_ENTRY_STC_PTR_entryPtr = {
     "entryPtr", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(CPSS_DXCH_POLICER_TRIGGER_ENTRY_STC)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_DXCH_POLICER_VLAN_CNTR_MODE_ENT_mode = {
     "mode", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_MAC(CPSS_DXCH_POLICER_VLAN_CNTR_MODE_ENT)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_PACKET_CMD_ENT_cmdTrigger = {
     "cmdTrigger", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_MAC(CPSS_PACKET_CMD_ENT)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_POLICER_MRU_ENT_mru = {
     "mru", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_MAC(CPSS_POLICER_MRU_ENT)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_POLICER_PACKET_SIZE_MODE_ENT_mode = {
     "mode", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_MAC(CPSS_POLICER_PACKET_SIZE_MODE_ENT)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_POLICER_PACKET_SIZE_MODE_ENT_packetSize = {
     "packetSize", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_MAC(CPSS_POLICER_PACKET_SIZE_MODE_ENT)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_POLICER_TB_MODE_ENT_mode = {
     "mode", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_MAC(CPSS_POLICER_TB_MODE_ENT)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_BOOL_cbsPbsCalcOnFail = {
     "cbsPbsCalcOnFail", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_MAC(GT_BOOL)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_BOOL_couplingFlag0 = {
     "couplingFlag0", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_MAC(GT_BOOL)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_U32_bucketIndex = {
     "bucketIndex", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_MAC(GT_U32)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_U32_cirPirAllowedDeviation = {
     "cirPirAllowedDeviation", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_MAC(GT_U32)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_U32_cntrValue = {
     "cntrValue", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_MAC(GT_U32)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_U32_envelopeSize = {
     "envelopeSize", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_MAC(GT_U32)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_U32_greenQosTableRemarkIndex = {
     "greenQosTableRemarkIndex", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_MAC(GT_U32)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_U32_interval = {
     "interval", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_MAC(GT_U32)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_U32_maxEnvelopeSize = {
     "maxEnvelopeSize", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_MAC(GT_U32)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_U32_maxRate = {
     "maxRate", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_MAC(GT_U32)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_U32_meterEntryBurstSize = {
     "meterEntryBurstSize", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_MAC(GT_U32)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_U32_meterEntryRate = {
     "meterEntryRate", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_MAC(GT_U32)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_U32_plrIndexMask = {
     "plrIndexMask", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_MAC(GT_U32)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_U32_policerIndex = {
     "policerIndex", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_MAC(GT_U32)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_U32_qosProfileIndex = {
     "qosProfileIndex", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_MAC(GT_U32)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_U32_redQosTableRemarkIndex = {
     "redQosTableRemarkIndex", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_MAC(GT_U32)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_U32_remarkParamValue = {
     "remarkParamValue", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_MAC(GT_U32)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_U32_startAddress = {
     "startAddress", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_MAC(GT_U32)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_U32_startEntryIndex = {
     "startEntryIndex", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_MAC(GT_U32)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_U32_stopAddress = {
     "stopAddress", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_MAC(GT_U32)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_U32_yellowQosTableRemarkIndex = {
     "yellowQosTableRemarkIndex", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_MAC(GT_U32)
};
PRV_CPSS_LOG_FUNC_PARAM_STC OUT_CPSS_DROP_MODE_TYPE_ENT_PTR_dropTypePtr = {
     "dropTypePtr", PRV_CPSS_LOG_PARAM_OUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(CPSS_DROP_MODE_TYPE_ENT)
};
PRV_CPSS_LOG_FUNC_PARAM_STC OUT_CPSS_DXCH3_POLICER_BILLING_CNTR_MODE_ENT_PTR_cntrModePtr = {
     "cntrModePtr", PRV_CPSS_LOG_PARAM_OUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(CPSS_DXCH3_POLICER_BILLING_CNTR_MODE_ENT)
};
PRV_CPSS_LOG_FUNC_PARAM_STC OUT_CPSS_DXCH3_POLICER_BILLING_ENTRY_STC_PTR_billingCntrPtr = {
     "billingCntrPtr", PRV_CPSS_LOG_PARAM_OUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(CPSS_DXCH3_POLICER_BILLING_ENTRY_STC)
};
PRV_CPSS_LOG_FUNC_PARAM_STC OUT_CPSS_DXCH3_POLICER_COLOR_COUNT_MODE_ENT_PTR_modePtr = {
     "modePtr", PRV_CPSS_LOG_PARAM_OUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(CPSS_DXCH3_POLICER_COLOR_COUNT_MODE_ENT)
};
PRV_CPSS_LOG_FUNC_PARAM_STC OUT_CPSS_DXCH3_POLICER_ENTRY_TYPE_ENT_PTR_entryTypePtr = {
     "entryTypePtr", PRV_CPSS_LOG_PARAM_OUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(CPSS_DXCH3_POLICER_ENTRY_TYPE_ENT)
};
PRV_CPSS_LOG_FUNC_PARAM_STC OUT_CPSS_DXCH3_POLICER_METERING_ENTRY_STC_PTR_entryArr = {
     "entryArr", PRV_CPSS_LOG_PARAM_OUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(CPSS_DXCH3_POLICER_METERING_ENTRY_STC)
};
PRV_CPSS_LOG_FUNC_PARAM_STC OUT_CPSS_DXCH3_POLICER_METERING_ENTRY_STC_PTR_entryPtr = {
     "entryPtr", PRV_CPSS_LOG_PARAM_OUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(CPSS_DXCH3_POLICER_METERING_ENTRY_STC)
};
PRV_CPSS_LOG_FUNC_PARAM_STC OUT_CPSS_DXCH3_POLICER_METER_RESOLUTION_ENT_PTR_resolutionPtr = {
     "resolutionPtr", PRV_CPSS_LOG_PARAM_OUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(CPSS_DXCH3_POLICER_METER_RESOLUTION_ENT)
};
PRV_CPSS_LOG_FUNC_PARAM_STC OUT_CPSS_DXCH3_POLICER_METER_TB_PARAMS_UNT_PTR_tbOutParamsPtr = {
     "tbOutParamsPtr", PRV_CPSS_LOG_PARAM_OUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(CPSS_DXCH3_POLICER_METER_TB_PARAMS_UNT)
};
PRV_CPSS_LOG_FUNC_PARAM_STC OUT_CPSS_DXCH3_POLICER_METER_TB_PARAMS_UNT_PTR_tbParamsArr = {
     "tbParamsArr", PRV_CPSS_LOG_PARAM_OUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(CPSS_DXCH3_POLICER_METER_TB_PARAMS_UNT)
};
PRV_CPSS_LOG_FUNC_PARAM_STC OUT_CPSS_DXCH3_POLICER_METER_TB_PARAMS_UNT_PTR_tbParamsPtr = {
     "tbParamsPtr", PRV_CPSS_LOG_PARAM_OUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(CPSS_DXCH3_POLICER_METER_TB_PARAMS_UNT)
};
PRV_CPSS_LOG_FUNC_PARAM_STC OUT_CPSS_DXCH3_POLICER_MNG_CNTR_ENTRY_STC_PTR_mngCntrPtr = {
     "mngCntrPtr", PRV_CPSS_LOG_PARAM_OUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(CPSS_DXCH3_POLICER_MNG_CNTR_ENTRY_STC)
};
PRV_CPSS_LOG_FUNC_PARAM_STC OUT_CPSS_DXCH3_POLICER_MNG_CNTR_RESOLUTION_ENT_PTR_cntrResolutionPtr = {
     "cntrResolutionPtr", PRV_CPSS_LOG_PARAM_OUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(CPSS_DXCH3_POLICER_MNG_CNTR_RESOLUTION_ENT)
};
PRV_CPSS_LOG_FUNC_PARAM_STC OUT_CPSS_DXCH3_POLICER_TS_TAG_COUNTING_MODE_ENT_PTR_timestampTagModePtr = {
     "timestampTagModePtr", PRV_CPSS_LOG_PARAM_OUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(CPSS_DXCH3_POLICER_TS_TAG_COUNTING_MODE_ENT)
};
PRV_CPSS_LOG_FUNC_PARAM_STC OUT_CPSS_DXCH3_POLICER_TT_PACKET_SIZE_MODE_ENT_PTR_ttPacketSizeModePtr = {
     "ttPacketSizeModePtr", PRV_CPSS_LOG_PARAM_OUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(CPSS_DXCH3_POLICER_TT_PACKET_SIZE_MODE_ENT)
};
PRV_CPSS_LOG_FUNC_PARAM_STC OUT_CPSS_DXCH_POLICER_BILLING_INDEX_CFG_STC_PTR_billingIndexCfgPtr = {
     "billingIndexCfgPtr", PRV_CPSS_LOG_PARAM_OUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(CPSS_DXCH_POLICER_BILLING_INDEX_CFG_STC)
};
PRV_CPSS_LOG_FUNC_PARAM_STC OUT_CPSS_DXCH_POLICER_BUCKETS_STATE_STC_PTR_bucketsStatePtr = {
     "bucketsStatePtr", PRV_CPSS_LOG_PARAM_OUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(CPSS_DXCH_POLICER_BUCKETS_STATE_STC)
};
PRV_CPSS_LOG_FUNC_PARAM_STC OUT_CPSS_DXCH_POLICER_COUNTERS_STC_PTR_countersPtr = {
     "countersPtr", PRV_CPSS_LOG_PARAM_OUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(CPSS_DXCH_POLICER_COUNTERS_STC)
};
PRV_CPSS_LOG_FUNC_PARAM_STC OUT_CPSS_DXCH_POLICER_COUNTING_MODE_ENT_PTR_modePtr = {
     "modePtr", PRV_CPSS_LOG_PARAM_OUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(CPSS_DXCH_POLICER_COUNTING_MODE_ENT)
};
PRV_CPSS_LOG_FUNC_PARAM_STC OUT_CPSS_DXCH_POLICER_ENTRY_STC_PTR_entryPtr = {
     "entryPtr", PRV_CPSS_LOG_PARAM_OUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(CPSS_DXCH_POLICER_ENTRY_STC)
};
PRV_CPSS_LOG_FUNC_PARAM_STC OUT_CPSS_DXCH_POLICER_ENVELOPE_MAX_SIZE_ENT_PTR_maxSizePtr = {
     "maxSizePtr", PRV_CPSS_LOG_PARAM_OUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(CPSS_DXCH_POLICER_ENVELOPE_MAX_SIZE_ENT)
};
PRV_CPSS_LOG_FUNC_PARAM_STC OUT_CPSS_DXCH_POLICER_E_ATTR_METER_MODE_ENT_PTR_modePtr = {
     "modePtr", PRV_CPSS_LOG_PARAM_OUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(CPSS_DXCH_POLICER_E_ATTR_METER_MODE_ENT)
};
PRV_CPSS_LOG_FUNC_PARAM_STC OUT_CPSS_DXCH_POLICER_HIERARCHICAL_TABLE_ENTRY_STC_PTR_entryPtr = {
     "entryPtr", PRV_CPSS_LOG_PARAM_OUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(CPSS_DXCH_POLICER_HIERARCHICAL_TABLE_ENTRY_STC)
};
PRV_CPSS_LOG_FUNC_PARAM_STC OUT_CPSS_DXCH_POLICER_L2_REMARK_MODEL_ENT_PTR_modelPtr = {
     "modelPtr", PRV_CPSS_LOG_PARAM_OUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(CPSS_DXCH_POLICER_L2_REMARK_MODEL_ENT)
};
PRV_CPSS_LOG_FUNC_PARAM_STC OUT_CPSS_DXCH_POLICER_MEMORY_CTRL_MODE_ENT_PTR_modePtr = {
     "modePtr", PRV_CPSS_LOG_PARAM_OUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(CPSS_DXCH_POLICER_MEMORY_CTRL_MODE_ENT)
};
PRV_CPSS_LOG_FUNC_PARAM_STC OUT_CPSS_DXCH_POLICER_METERING_CALC_METHOD_ENT_PTR_meteringCalcMethodPtr = {
     "meteringCalcMethodPtr", PRV_CPSS_LOG_PARAM_OUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(CPSS_DXCH_POLICER_METERING_CALC_METHOD_ENT)
};
PRV_CPSS_LOG_FUNC_PARAM_STC OUT_CPSS_DXCH_POLICER_PORT_MODE_ADDR_SELECT_TYPE_ENT_PTR_typePtr = {
     "typePtr", PRV_CPSS_LOG_PARAM_OUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(CPSS_DXCH_POLICER_PORT_MODE_ADDR_SELECT_TYPE_ENT)
};
PRV_CPSS_LOG_FUNC_PARAM_STC OUT_CPSS_DXCH_POLICER_QOS_PARAM_STC_PTR_qosParamPtr = {
     "qosParamPtr", PRV_CPSS_LOG_PARAM_OUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(CPSS_DXCH_POLICER_QOS_PARAM_STC)
};
PRV_CPSS_LOG_FUNC_PARAM_STC OUT_CPSS_DXCH_POLICER_STAGE_METER_MODE_ENT_PTR_modePtr = {
     "modePtr", PRV_CPSS_LOG_PARAM_OUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(CPSS_DXCH_POLICER_STAGE_METER_MODE_ENT)
};
PRV_CPSS_LOG_FUNC_PARAM_STC OUT_CPSS_DXCH_POLICER_TB_PARAMS_STC_PTR_tbOutParamsPtr = {
     "tbOutParamsPtr", PRV_CPSS_LOG_PARAM_OUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(CPSS_DXCH_POLICER_TB_PARAMS_STC)
};
PRV_CPSS_LOG_FUNC_PARAM_STC OUT_CPSS_DXCH_POLICER_TB_PARAMS_STC_PTR_tbParamsPtr = {
     "tbParamsPtr", PRV_CPSS_LOG_PARAM_OUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(CPSS_DXCH_POLICER_TB_PARAMS_STC)
};
PRV_CPSS_LOG_FUNC_PARAM_STC OUT_CPSS_DXCH_POLICER_TRIGGER_ENTRY_STC_PTR_entryPtr = {
     "entryPtr", PRV_CPSS_LOG_PARAM_OUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(CPSS_DXCH_POLICER_TRIGGER_ENTRY_STC)
};
PRV_CPSS_LOG_FUNC_PARAM_STC OUT_CPSS_DXCH_POLICER_VLAN_CNTR_MODE_ENT_PTR_modePtr = {
     "modePtr", PRV_CPSS_LOG_PARAM_OUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(CPSS_DXCH_POLICER_VLAN_CNTR_MODE_ENT)
};
PRV_CPSS_LOG_FUNC_PARAM_STC OUT_CPSS_POLICER_PACKET_SIZE_MODE_ENT_PTR_packetSizePtr = {
     "packetSizePtr", PRV_CPSS_LOG_PARAM_OUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(CPSS_POLICER_PACKET_SIZE_MODE_ENT)
};
PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_BOOL_PTR_cbsPbsCalcOnFailPtr = {
     "cbsPbsCalcOnFailPtr", PRV_CPSS_LOG_PARAM_OUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(GT_BOOL)
};
PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_BOOL_PTR_couplingFlag0Ptr = {
     "couplingFlag0Ptr", PRV_CPSS_LOG_PARAM_OUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(GT_BOOL)
};
PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_U32_PTR_cirPirAllowedDeviationPtr = {
     "cirPirAllowedDeviationPtr", PRV_CPSS_LOG_PARAM_OUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(GT_U32)
};
PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_U32_PTR_cntrValuePtr = {
     "cntrValuePtr", PRV_CPSS_LOG_PARAM_OUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(GT_U32)
};
PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_U32_PTR_entryAddrPtr = {
     "entryAddrPtr", PRV_CPSS_LOG_PARAM_OUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(GT_U32)
};
PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_U32_PTR_envelopeSizePtr = {
     "envelopeSizePtr", PRV_CPSS_LOG_PARAM_OUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(GT_U32)
};
PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_U32_PTR_greenQosTableRemarkIndexPtr = {
     "greenQosTableRemarkIndexPtr", PRV_CPSS_LOG_PARAM_OUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(GT_U32)
};
PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_U32_PTR_intervalPtr = {
     "intervalPtr", PRV_CPSS_LOG_PARAM_OUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(GT_U32)
};
PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_U32_PTR_plrIndexMaskPtr = {
     "plrIndexMaskPtr", PRV_CPSS_LOG_PARAM_OUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(GT_U32)
};
PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_U32_PTR_redQosTableRemarkIndexPtr = {
     "redQosTableRemarkIndexPtr", PRV_CPSS_LOG_PARAM_OUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(GT_U32)
};
PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_U32_PTR_startAddressPtr = {
     "startAddressPtr", PRV_CPSS_LOG_PARAM_OUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(GT_U32)
};
PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_U32_PTR_stopAddressPtr = {
     "stopAddressPtr", PRV_CPSS_LOG_PARAM_OUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(GT_U32)
};
PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_U32_PTR_yellowQosTableRemarkIndexPtr = {
     "yellowQosTableRemarkIndexPtr", PRV_CPSS_LOG_PARAM_OUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(GT_U32)
};


/********* API prototypes DB *********/

PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChPolicerDropRedModeSet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_CPSS_DROP_MODE_TYPE_ENT_dropRedMode
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxCh3PolicerBillingCountersModeSet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_CPSS_DXCH3_POLICER_BILLING_CNTR_MODE_ENT_cntrMode
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChPolicerEgressL2RemarkModelSet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_CPSS_DXCH_POLICER_L2_REMARK_MODEL_ENT_model
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChPolicerMemorySizeModeSet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_CPSS_DXCH_POLICER_MEMORY_CTRL_MODE_ENT_mode
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChPolicerMeteringCalcMethodSet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_CPSS_DXCH_POLICER_METERING_CALC_METHOD_ENT_meteringCalcMethod,
    &IN_GT_U32_cirPirAllowedDeviation,
    &IN_GT_BOOL_cbsPbsCalcOnFail
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChPolicerEgressQosRemarkingEntrySet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_CPSS_DXCH_POLICER_REMARK_TABLE_TYPE_ENT_remarkTableType,
    &IN_GT_U32_remarkParamValue,
    &IN_CPSS_DP_LEVEL_ENT_confLevel,
    &IN_CPSS_DXCH_POLICER_QOS_PARAM_STC_PTR_qosParamPtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChPolicerEgressQosRemarkingEntryGet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_CPSS_DXCH_POLICER_REMARK_TABLE_TYPE_ENT_remarkTableType,
    &IN_GT_U32_remarkParamValue,
    &IN_CPSS_DP_LEVEL_ENT_confLevel,
    &OUT_CPSS_DXCH_POLICER_QOS_PARAM_STC_PTR_qosParamPtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChPolicerCountingWriteBackCacheFlush_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_CPSS_DXCH_POLICER_STAGE_TYPE_ENT_stage
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxCh3PolicerDropTypeSet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_CPSS_DXCH_POLICER_STAGE_TYPE_ENT_stage,
    &IN_CPSS_DROP_MODE_TYPE_ENT_dropType
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxCh3PolicerCountingColorModeSet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_CPSS_DXCH_POLICER_STAGE_TYPE_ENT_stage,
    &IN_CPSS_DXCH3_POLICER_COLOR_COUNT_MODE_ENT_mode
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxCh3PolicerEntryMeterParamsCalculate_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_CPSS_DXCH_POLICER_STAGE_TYPE_ENT_stage,
    &IN_CPSS_DXCH3_POLICER_METER_MODE_ENT_meterMode,
    &IN_CPSS_DXCH3_POLICER_METER_TB_PARAMS_UNT_PTR_tbInParamsPtr,
    &OUT_CPSS_DXCH3_POLICER_METER_TB_PARAMS_UNT_PTR_tbOutParamsPtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxCh3PolicerMeterResolutionSet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_CPSS_DXCH_POLICER_STAGE_TYPE_ENT_stage,
    &IN_CPSS_DXCH3_POLICER_METER_RESOLUTION_ENT_resolution
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxCh3PolicerManagementCntrsResolutionSet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_CPSS_DXCH_POLICER_STAGE_TYPE_ENT_stage,
    &IN_CPSS_DXCH3_POLICER_MNG_CNTR_SET_ENT_cntrSet,
    &IN_CPSS_DXCH3_POLICER_MNG_CNTR_RESOLUTION_ENT_cntrResolution
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxCh3PolicerManagementCntrsResolutionGet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_CPSS_DXCH_POLICER_STAGE_TYPE_ENT_stage,
    &IN_CPSS_DXCH3_POLICER_MNG_CNTR_SET_ENT_cntrSet,
    &OUT_CPSS_DXCH3_POLICER_MNG_CNTR_RESOLUTION_ENT_PTR_cntrResolutionPtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxCh3PolicerManagementCountersSet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_CPSS_DXCH_POLICER_STAGE_TYPE_ENT_stage,
    &IN_CPSS_DXCH3_POLICER_MNG_CNTR_SET_ENT_mngCntrSet,
    &IN_CPSS_DXCH3_POLICER_MNG_CNTR_TYPE_ENT_mngCntrType,
    &IN_CPSS_DXCH3_POLICER_MNG_CNTR_ENTRY_STC_PTR_mngCntrPtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxCh3PolicerManagementCountersGet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_CPSS_DXCH_POLICER_STAGE_TYPE_ENT_stage,
    &IN_CPSS_DXCH3_POLICER_MNG_CNTR_SET_ENT_mngCntrSet,
    &IN_CPSS_DXCH3_POLICER_MNG_CNTR_TYPE_ENT_mngCntrType,
    &OUT_CPSS_DXCH3_POLICER_MNG_CNTR_ENTRY_STC_PTR_mngCntrPtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChPolicerPacketSizeModeForTimestampTagSet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_CPSS_DXCH_POLICER_STAGE_TYPE_ENT_stage,
    &IN_CPSS_DXCH3_POLICER_TS_TAG_COUNTING_MODE_ENT_timestampTagMode
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxCh3PolicerPacketSizeModeForTunnelTermSet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_CPSS_DXCH_POLICER_STAGE_TYPE_ENT_stage,
    &IN_CPSS_DXCH3_POLICER_TT_PACKET_SIZE_MODE_ENT_ttPacketSizeMode
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChPolicerFlowIdCountingCfgSet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_CPSS_DXCH_POLICER_STAGE_TYPE_ENT_stage,
    &IN_CPSS_DXCH_POLICER_BILLING_INDEX_CFG_STC_PTR_billingIndexCfgPtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChPolicerCountingModeSet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_CPSS_DXCH_POLICER_STAGE_TYPE_ENT_stage,
    &IN_CPSS_DXCH_POLICER_COUNTING_MODE_ENT_mode
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChPolicerEAttributesMeteringModeSet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_CPSS_DXCH_POLICER_STAGE_TYPE_ENT_stage,
    &IN_CPSS_DXCH_POLICER_E_ATTR_METER_MODE_ENT_mode
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChPolicerPortModeAddressSelectSet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_CPSS_DXCH_POLICER_STAGE_TYPE_ENT_stage,
    &IN_CPSS_DXCH_POLICER_PORT_MODE_ADDR_SELECT_TYPE_ENT_type
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChPolicerStageMeterModeSet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_CPSS_DXCH_POLICER_STAGE_TYPE_ENT_stage,
    &IN_CPSS_DXCH_POLICER_STAGE_METER_MODE_ENT_mode
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChPolicerVlanCountingModeSet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_CPSS_DXCH_POLICER_STAGE_TYPE_ENT_stage,
    &IN_CPSS_DXCH_POLICER_VLAN_CNTR_MODE_ENT_mode
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChPolicerVlanCountingPacketCmdTriggerSet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_CPSS_DXCH_POLICER_STAGE_TYPE_ENT_stage,
    &IN_CPSS_PACKET_CMD_ENT_cmdTrigger,
    &IN_GT_BOOL_enable
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChPolicerVlanCountingPacketCmdTriggerGet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_CPSS_DXCH_POLICER_STAGE_TYPE_ENT_stage,
    &IN_CPSS_PACKET_CMD_ENT_cmdTrigger,
    &OUT_GT_BOOL_PTR_enablePtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxCh3PolicerPacketSizeModeSet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_CPSS_DXCH_POLICER_STAGE_TYPE_ENT_stage,
    &IN_CPSS_POLICER_PACKET_SIZE_MODE_ENT_packetSize
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChPolicerPortStormTypeIndexSet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_CPSS_DXCH_POLICER_STAGE_TYPE_ENT_stage,
    &IN_GT_PHYSICAL_PORT_NUM_portNum,
    &IN_CPSS_DXCH_POLICER_STORM_TYPE_ENT_stormType,
    &IN_GT_U32_index
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChPolicerPortStormTypeIndexGet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_CPSS_DXCH_POLICER_STAGE_TYPE_ENT_stage,
    &IN_GT_PHYSICAL_PORT_NUM_portNum,
    &IN_CPSS_DXCH_POLICER_STORM_TYPE_ENT_stormType,
    &OUT_GT_U32_PTR_indexPtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxCh3PolicerPortMeteringEnableSet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_CPSS_DXCH_POLICER_STAGE_TYPE_ENT_stage,
    &IN_GT_PHYSICAL_PORT_NUM_portNum,
    &IN_GT_BOOL_enable
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxCh3PolicerPortMeteringEnableGet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_CPSS_DXCH_POLICER_STAGE_TYPE_ENT_stage,
    &IN_GT_PHYSICAL_PORT_NUM_portNum,
    &OUT_GT_BOOL_PTR_enablePtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChPolicerVlanCntrSet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_CPSS_DXCH_POLICER_STAGE_TYPE_ENT_stage,
    &IN_GT_U16_vid,
    &IN_GT_U32_cntrValue
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChPolicerVlanCntrGet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_CPSS_DXCH_POLICER_STAGE_TYPE_ENT_stage,
    &IN_GT_U16_vid,
    &OUT_GT_U32_PTR_cntrValuePtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxCh3PolicerMeteringEntryRefresh_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_CPSS_DXCH_POLICER_STAGE_TYPE_ENT_stage,
    &IN_GT_U32_entryIndex
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxCh3PolicerBillingEntrySet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_CPSS_DXCH_POLICER_STAGE_TYPE_ENT_stage,
    &IN_GT_U32_entryIndex,
    &IN_CPSS_DXCH3_POLICER_BILLING_ENTRY_STC_PTR_billingCntrPtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxCh3PolicerMeteringEntrySet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_CPSS_DXCH_POLICER_STAGE_TYPE_ENT_stage,
    &IN_GT_U32_entryIndex,
    &IN_CPSS_DXCH3_POLICER_METERING_ENTRY_STC_PTR_entryPtr,
    &OUT_CPSS_DXCH3_POLICER_METER_TB_PARAMS_UNT_PTR_tbParamsPtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChPolicerTriggerEntrySet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_CPSS_DXCH_POLICER_STAGE_TYPE_ENT_stage,
    &IN_GT_U32_entryIndex,
    &IN_CPSS_DXCH_POLICER_TRIGGER_ENTRY_STC_PTR_entryPtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxCh3PolicerBillingEntryGet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_CPSS_DXCH_POLICER_STAGE_TYPE_ENT_stage,
    &IN_GT_U32_entryIndex,
    &IN_GT_BOOL_reset,
    &OUT_CPSS_DXCH3_POLICER_BILLING_ENTRY_STC_PTR_billingCntrPtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChPolicerTokenBucketMaxRateSet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_CPSS_DXCH_POLICER_STAGE_TYPE_ENT_stage,
    &IN_GT_U32_entryIndex,
    &IN_GT_U32_bucketIndex,
    &IN_GT_U32_meterEntryRate,
    &IN_GT_U32_meterEntryBurstSize,
    &IN_GT_U32_maxRate
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChPolicerTokenBucketMaxRateGet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_CPSS_DXCH_POLICER_STAGE_TYPE_ENT_stage,
    &IN_GT_U32_entryIndex,
    &IN_GT_U32_bucketIndex,
    &IN_GT_U32_meterEntryRate,
    &IN_GT_U32_meterEntryBurstSize,
    &OUT_GT_U32_PTR_maxRatePtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxCh3PolicerMeteringEntryGet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_CPSS_DXCH_POLICER_STAGE_TYPE_ENT_stage,
    &IN_GT_U32_entryIndex,
    &OUT_CPSS_DXCH3_POLICER_METERING_ENTRY_STC_PTR_entryPtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChPolicerTriggerEntryGet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_CPSS_DXCH_POLICER_STAGE_TYPE_ENT_stage,
    &IN_GT_U32_entryIndex,
    &OUT_CPSS_DXCH_POLICER_TRIGGER_ENTRY_STC_PTR_entryPtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChPolicerPolicyCntrSet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_CPSS_DXCH_POLICER_STAGE_TYPE_ENT_stage,
    &IN_GT_U32_index,
    &IN_GT_U32_cntrValue
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChPolicerPolicyCntrGet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_CPSS_DXCH_POLICER_STAGE_TYPE_ENT_stage,
    &IN_GT_U32_index,
    &OUT_GT_U32_PTR_cntrValuePtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxCh3PolicerMruSet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_CPSS_DXCH_POLICER_STAGE_TYPE_ENT_stage,
    &IN_GT_U32_mruSize
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChPolicerBucketsCurrentStateGet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_CPSS_DXCH_POLICER_STAGE_TYPE_ENT_stage,
    &IN_GT_U32_policerIndex,
    &OUT_CPSS_DXCH_POLICER_BUCKETS_STATE_STC_PTR_bucketsStatePtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxCh3PolicerQosRemarkingEntrySet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_CPSS_DXCH_POLICER_STAGE_TYPE_ENT_stage,
    &IN_GT_U32_qosProfileIndex,
    &IN_GT_U32_greenQosTableRemarkIndex,
    &IN_GT_U32_yellowQosTableRemarkIndex,
    &IN_GT_U32_redQosTableRemarkIndex
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChPolicerQosProfileToPriorityMapSet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_CPSS_DXCH_POLICER_STAGE_TYPE_ENT_stage,
    &IN_GT_U32_qosProfileIndex,
    &IN_GT_U32_priority
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxCh3PolicerQosRemarkingEntryGet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_CPSS_DXCH_POLICER_STAGE_TYPE_ENT_stage,
    &IN_GT_U32_qosProfileIndex,
    &OUT_GT_U32_PTR_greenQosTableRemarkIndexPtr,
    &OUT_GT_U32_PTR_yellowQosTableRemarkIndexPtr,
    &OUT_GT_U32_PTR_redQosTableRemarkIndexPtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChPolicerQosProfileToPriorityMapGet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_CPSS_DXCH_POLICER_STAGE_TYPE_ENT_stage,
    &IN_GT_U32_qosProfileIndex,
    &OUT_GT_U32_PTR_priorityPtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChPolicerMeteringEntryEnvelopeSet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_CPSS_DXCH_POLICER_STAGE_TYPE_ENT_stage,
    &IN_GT_U32_startEntryIndex,
    &IN_GT_U32_envelopeSize,
    &IN_GT_BOOL_couplingFlag0,
    &IN_CPSS_DXCH3_POLICER_METERING_ENTRY_STC_PTR_entryArr,
    &OUT_CPSS_DXCH3_POLICER_METER_TB_PARAMS_UNT_PTR_tbParamsArr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChPolicerMeteringEntryEnvelopeGet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_CPSS_DXCH_POLICER_STAGE_TYPE_ENT_stage,
    &IN_GT_U32_startEntryIndex,
    &IN_GT_U32_maxEnvelopeSize,
    &OUT_GT_U32_PTR_envelopeSizePtr,
    &OUT_GT_BOOL_PTR_couplingFlag0Ptr,
    &OUT_CPSS_DXCH3_POLICER_METERING_ENTRY_STC_PTR_entryArr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChPolicerMeterTableFlowBasedIndexConfigSet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_CPSS_DXCH_POLICER_STAGE_TYPE_ENT_stage,
    &IN_GT_U32_threshold,
    &IN_CPSS_DXCH_POLICER_ENVELOPE_MAX_SIZE_ENT_maxSize
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxCh3PolicerDropTypeGet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_CPSS_DXCH_POLICER_STAGE_TYPE_ENT_stage,
    &OUT_CPSS_DROP_MODE_TYPE_ENT_PTR_dropTypePtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxCh3PolicerCountingColorModeGet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_CPSS_DXCH_POLICER_STAGE_TYPE_ENT_stage,
    &OUT_CPSS_DXCH3_POLICER_COLOR_COUNT_MODE_ENT_PTR_modePtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxCh3PolicerErrorGet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_CPSS_DXCH_POLICER_STAGE_TYPE_ENT_stage,
    &OUT_CPSS_DXCH3_POLICER_ENTRY_TYPE_ENT_PTR_entryTypePtr,
    &OUT_GT_U32_PTR_entryAddrPtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxCh3PolicerMeterResolutionGet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_CPSS_DXCH_POLICER_STAGE_TYPE_ENT_stage,
    &OUT_CPSS_DXCH3_POLICER_METER_RESOLUTION_ENT_PTR_resolutionPtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChPolicerPacketSizeModeForTimestampTagGet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_CPSS_DXCH_POLICER_STAGE_TYPE_ENT_stage,
    &OUT_CPSS_DXCH3_POLICER_TS_TAG_COUNTING_MODE_ENT_PTR_timestampTagModePtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxCh3PolicerPacketSizeModeForTunnelTermGet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_CPSS_DXCH_POLICER_STAGE_TYPE_ENT_stage,
    &OUT_CPSS_DXCH3_POLICER_TT_PACKET_SIZE_MODE_ENT_PTR_ttPacketSizeModePtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChPolicerFlowIdCountingCfgGet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_CPSS_DXCH_POLICER_STAGE_TYPE_ENT_stage,
    &OUT_CPSS_DXCH_POLICER_BILLING_INDEX_CFG_STC_PTR_billingIndexCfgPtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChPolicerCountingModeGet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_CPSS_DXCH_POLICER_STAGE_TYPE_ENT_stage,
    &OUT_CPSS_DXCH_POLICER_COUNTING_MODE_ENT_PTR_modePtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChPolicerEAttributesMeteringModeGet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_CPSS_DXCH_POLICER_STAGE_TYPE_ENT_stage,
    &OUT_CPSS_DXCH_POLICER_E_ATTR_METER_MODE_ENT_PTR_modePtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChPolicerPortModeAddressSelectGet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_CPSS_DXCH_POLICER_STAGE_TYPE_ENT_stage,
    &OUT_CPSS_DXCH_POLICER_PORT_MODE_ADDR_SELECT_TYPE_ENT_PTR_typePtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChPolicerStageMeterModeGet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_CPSS_DXCH_POLICER_STAGE_TYPE_ENT_stage,
    &OUT_CPSS_DXCH_POLICER_STAGE_METER_MODE_ENT_PTR_modePtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChPolicerVlanCountingModeGet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_CPSS_DXCH_POLICER_STAGE_TYPE_ENT_stage,
    &OUT_CPSS_DXCH_POLICER_VLAN_CNTR_MODE_ENT_PTR_modePtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxCh3PolicerPacketSizeModeGet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_CPSS_DXCH_POLICER_STAGE_TYPE_ENT_stage,
    &OUT_CPSS_POLICER_PACKET_SIZE_MODE_ENT_PTR_packetSizePtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxCh3PolicerErrorCounterGet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_CPSS_DXCH_POLICER_STAGE_TYPE_ENT_stage,
    &OUT_GT_U32_PTR_cntrValuePtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxCh3PolicerMruGet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_CPSS_DXCH_POLICER_STAGE_TYPE_ENT_stage,
    &OUT_GT_U32_PTR_mruSizePtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChPolicerMeterTableFlowBasedIndexConfigGet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_CPSS_DXCH_POLICER_STAGE_TYPE_ENT_stage,
    &OUT_GT_U32_PTR_thresholdPtr,
    &OUT_CPSS_DXCH_POLICER_ENVELOPE_MAX_SIZE_ENT_PTR_maxSizePtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChPolicerEntryMeterParamsCalculate_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_CPSS_DXCH_POLICER_TB_PARAMS_STC_PTR_tbInParamsPtr,
    &OUT_CPSS_DXCH_POLICER_TB_PARAMS_STC_PTR_tbOutParamsPtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChPolicerPacketSizeModeSet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_CPSS_POLICER_PACKET_SIZE_MODE_ENT_mode
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChPolicerTokenBucketModeSet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_CPSS_POLICER_TB_MODE_ENT_mode,
    &IN_CPSS_POLICER_MRU_ENT_mru
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChPolicerPortGroupManagementCountersSet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_PORT_GROUPS_BMP_portGroupsBmp,
    &IN_CPSS_DXCH_POLICER_STAGE_TYPE_ENT_stage,
    &IN_CPSS_DXCH3_POLICER_MNG_CNTR_SET_ENT_mngCntrSet,
    &IN_CPSS_DXCH3_POLICER_MNG_CNTR_TYPE_ENT_mngCntrType,
    &IN_CPSS_DXCH3_POLICER_MNG_CNTR_ENTRY_STC_PTR_mngCntrPtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChPolicerPortGroupManagementCountersGet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_PORT_GROUPS_BMP_portGroupsBmp,
    &IN_CPSS_DXCH_POLICER_STAGE_TYPE_ENT_stage,
    &IN_CPSS_DXCH3_POLICER_MNG_CNTR_SET_ENT_mngCntrSet,
    &IN_CPSS_DXCH3_POLICER_MNG_CNTR_TYPE_ENT_mngCntrType,
    &OUT_CPSS_DXCH3_POLICER_MNG_CNTR_ENTRY_STC_PTR_mngCntrPtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChPolicerPortGroupMeteringEntryRefresh_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_PORT_GROUPS_BMP_portGroupsBmp,
    &IN_CPSS_DXCH_POLICER_STAGE_TYPE_ENT_stage,
    &IN_GT_U32_entryIndex
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChPolicerPortGroupBillingEntrySet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_PORT_GROUPS_BMP_portGroupsBmp,
    &IN_CPSS_DXCH_POLICER_STAGE_TYPE_ENT_stage,
    &IN_GT_U32_entryIndex,
    &IN_CPSS_DXCH3_POLICER_BILLING_ENTRY_STC_PTR_billingCntrPtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChPolicerPortGroupMeteringEntrySet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_PORT_GROUPS_BMP_portGroupsBmp,
    &IN_CPSS_DXCH_POLICER_STAGE_TYPE_ENT_stage,
    &IN_GT_U32_entryIndex,
    &IN_CPSS_DXCH3_POLICER_METERING_ENTRY_STC_PTR_entryPtr,
    &OUT_CPSS_DXCH3_POLICER_METER_TB_PARAMS_UNT_PTR_tbParamsPtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChPolicerPortGroupBillingEntryGet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_PORT_GROUPS_BMP_portGroupsBmp,
    &IN_CPSS_DXCH_POLICER_STAGE_TYPE_ENT_stage,
    &IN_GT_U32_entryIndex,
    &IN_GT_BOOL_reset,
    &OUT_CPSS_DXCH3_POLICER_BILLING_ENTRY_STC_PTR_billingCntrPtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChPolicerPortGroupMeteringEntryGet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_PORT_GROUPS_BMP_portGroupsBmp,
    &IN_CPSS_DXCH_POLICER_STAGE_TYPE_ENT_stage,
    &IN_GT_U32_entryIndex,
    &OUT_CPSS_DXCH3_POLICER_METERING_ENTRY_STC_PTR_entryPtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChPolicerPortGroupPolicyCntrSet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_PORT_GROUPS_BMP_portGroupsBmp,
    &IN_CPSS_DXCH_POLICER_STAGE_TYPE_ENT_stage,
    &IN_GT_U32_index,
    &IN_GT_U32_cntrValue
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChPolicerPortGroupPolicyCntrGet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_PORT_GROUPS_BMP_portGroupsBmp,
    &IN_CPSS_DXCH_POLICER_STAGE_TYPE_ENT_stage,
    &IN_GT_U32_index,
    &OUT_GT_U32_PTR_cntrValuePtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChPolicerPortGroupBucketsCurrentStateGet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_PORT_GROUPS_BMP_portGroupsBmp,
    &IN_CPSS_DXCH_POLICER_STAGE_TYPE_ENT_stage,
    &IN_GT_U32_policerIndex,
    &OUT_CPSS_DXCH_POLICER_BUCKETS_STATE_STC_PTR_bucketsStatePtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChPolicerPortGroupErrorGet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_PORT_GROUPS_BMP_portGroupsBmp,
    &IN_CPSS_DXCH_POLICER_STAGE_TYPE_ENT_stage,
    &OUT_CPSS_DXCH3_POLICER_ENTRY_TYPE_ENT_PTR_entryTypePtr,
    &OUT_GT_U32_PTR_entryAddrPtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChPolicerPortGroupErrorCounterGet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_PORT_GROUPS_BMP_portGroupsBmp,
    &IN_CPSS_DXCH_POLICER_STAGE_TYPE_ENT_stage,
    &OUT_GT_U32_PTR_cntrValuePtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChPolicerCountersSet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_U32_counterSetIndex,
    &IN_CPSS_DXCH_POLICER_COUNTERS_STC_PTR_countersPtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChPolicerCountersGet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_U32_counterSetIndex,
    &OUT_CPSS_DXCH_POLICER_COUNTERS_STC_PTR_countersPtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChPolicerEntrySet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_U32_entryIndex,
    &IN_CPSS_DXCH_POLICER_ENTRY_STC_PTR_entryPtr,
    &OUT_CPSS_DXCH_POLICER_TB_PARAMS_STC_PTR_tbParamsPtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChPolicerEntryGet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_U32_entryIndex,
    &OUT_CPSS_DXCH_POLICER_ENTRY_STC_PTR_entryPtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChPolicerHierarchicalTableEntrySet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_U32_index,
    &IN_CPSS_DXCH_POLICER_HIERARCHICAL_TABLE_ENTRY_STC_PTR_entryPtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChPolicerHierarchicalTableEntryGet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_U32_index,
    &OUT_CPSS_DXCH_POLICER_HIERARCHICAL_TABLE_ENTRY_STC_PTR_entryPtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxCh3PolicerMeteringAutoRefreshIntervalSet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_U32_interval
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChPolicerSecondStageIndexMaskSet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_U32_plrIndexMask
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxCh3PolicerMeteringAutoRefreshRangeSet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_U32_startAddress,
    &IN_GT_U32_stopAddress
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxCh3PolicerBillingCountersModeGet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &OUT_CPSS_DXCH3_POLICER_BILLING_CNTR_MODE_ENT_PTR_cntrModePtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChPolicerEgressL2RemarkModelGet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &OUT_CPSS_DXCH_POLICER_L2_REMARK_MODEL_ENT_PTR_modelPtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChPolicerMemorySizeModeGet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &OUT_CPSS_DXCH_POLICER_MEMORY_CTRL_MODE_ENT_PTR_modePtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChPolicerMeteringCalcMethodGet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &OUT_CPSS_DXCH_POLICER_METERING_CALC_METHOD_ENT_PTR_meteringCalcMethodPtr,
    &OUT_GT_U32_PTR_cirPirAllowedDeviationPtr,
    &OUT_GT_BOOL_PTR_cbsPbsCalcOnFailPtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxCh3PolicerMeteringAutoRefreshIntervalGet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &OUT_GT_U32_PTR_intervalPtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChPolicerSecondStageIndexMaskGet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &OUT_GT_U32_PTR_plrIndexMaskPtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxCh3PolicerMeteringAutoRefreshRangeGet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &OUT_GT_U32_PTR_startAddressPtr,
    &OUT_GT_U32_PTR_stopAddressPtr
};


/********* lib API DB *********/

extern void cpssDxCh3PolicerMeteringEntrySet_preLogic(IN va_list  args, OUT PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC * paramDataPtr);
extern void cpssDxCh3PolicerEntryMeterParamsCalculate_preLogic(IN va_list  args, OUT PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC * paramDataPtr);
extern void cpssDxChPolicerPortGroupMeteringEntrySet_preLogic(IN va_list  args, OUT PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC * paramDataPtr);
extern void cpssDxChPolicerMeteringEntryEnvelopeSet_preLogic(IN va_list  args, OUT PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC * paramDataPtr);
extern void cpssDxChPolicerMeteringEntryEnvelopeGet_preLogic(IN va_list  args, OUT PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC * paramDataPtr);

static PRV_CPSS_LOG_FUNC_ENTRY_STC prvCpssDxChPolicerLogLibDb[] = {
    {"cpssDxChPolicerInit", 1, prvCpssLogGenDevNum_PARAMS, NULL},
    {"cpssDxChPolicerPacketSizeModeSet", 2, cpssDxChPolicerPacketSizeModeSet_PARAMS, NULL},
    {"cpssDxChPolicerDropRedModeSet", 2, cpssDxChPolicerDropRedModeSet_PARAMS, NULL},
    {"cpssDxChPolicerPacketSizeModeForTunnelTermSet", 2, cpssDxChPolicerPacketSizeModeSet_PARAMS, NULL},
    {"cpssDxChPolicerTokenBucketModeSet", 3, cpssDxChPolicerTokenBucketModeSet_PARAMS, NULL},
    {"cpssDxChPolicerEntrySet", 4, cpssDxChPolicerEntrySet_PARAMS, NULL},
    {"cpssDxChPolicerEntryGet", 3, cpssDxChPolicerEntryGet_PARAMS, NULL},
    {"cpssDxChPolicerEntryInvalidate", 2, prvCpssLogGenDevNumEntryIndex_PARAMS, NULL},
    {"cpssDxChPolicerEntryMeterParamsCalculate", 3, cpssDxChPolicerEntryMeterParamsCalculate_PARAMS, NULL},
    {"cpssDxChPolicerCountersGet", 3, cpssDxChPolicerCountersGet_PARAMS, NULL},
    {"cpssDxChPolicerCountersSet", 3, cpssDxChPolicerCountersSet_PARAMS, NULL},
    {"cpssDxCh3PolicerMeteringEnableSet", 3, prvCpssLogGenDevNumStageEnable_PARAMS, NULL},
    {"cpssDxCh3PolicerMeteringEnableGet", 3, prvCpssLogGenDevNumStageEnablePtr_PARAMS, NULL},
    {"cpssDxCh3PolicerBillingCountingEnableSet", 2, prvCpssLogGenDevNumEnable_PARAMS, NULL},
    {"cpssDxCh3PolicerBillingCountingEnableGet", 2, prvCpssLogGenDevNumEnablePtr_PARAMS, NULL},
    {"cpssDxChPolicerCountingModeSet", 3, cpssDxChPolicerCountingModeSet_PARAMS, NULL},
    {"cpssDxChPolicerCountingModeGet", 3, cpssDxChPolicerCountingModeGet_PARAMS, NULL},
    {"cpssDxCh3PolicerBillingCountersModeSet", 2, cpssDxCh3PolicerBillingCountersModeSet_PARAMS, NULL},
    {"cpssDxCh3PolicerBillingCountersModeGet", 2, cpssDxCh3PolicerBillingCountersModeGet_PARAMS, NULL},
    {"cpssDxCh3PolicerPacketSizeModeSet", 3, cpssDxCh3PolicerPacketSizeModeSet_PARAMS, NULL},
    {"cpssDxCh3PolicerPacketSizeModeGet", 3, cpssDxCh3PolicerPacketSizeModeGet_PARAMS, NULL},
    {"cpssDxCh3PolicerMeterResolutionSet", 3, cpssDxCh3PolicerMeterResolutionSet_PARAMS, NULL},
    {"cpssDxCh3PolicerMeterResolutionGet", 3, cpssDxCh3PolicerMeterResolutionGet_PARAMS, NULL},
    {"cpssDxCh3PolicerDropTypeSet", 3, cpssDxCh3PolicerDropTypeSet_PARAMS, NULL},
    {"cpssDxCh3PolicerDropTypeGet", 3, cpssDxCh3PolicerDropTypeGet_PARAMS, NULL},
    {"cpssDxCh3PolicerCountingColorModeSet", 3, cpssDxCh3PolicerCountingColorModeSet_PARAMS, NULL},
    {"cpssDxCh3PolicerCountingColorModeGet", 3, cpssDxCh3PolicerCountingColorModeGet_PARAMS, NULL},
    {"cpssDxCh3PolicerManagementCntrsResolutionSet", 4, cpssDxCh3PolicerManagementCntrsResolutionSet_PARAMS, NULL},
    {"cpssDxCh3PolicerManagementCntrsResolutionGet", 4, cpssDxCh3PolicerManagementCntrsResolutionGet_PARAMS, NULL},
    {"cpssDxCh3PolicerPacketSizeModeForTunnelTermSet", 3, cpssDxCh3PolicerPacketSizeModeForTunnelTermSet_PARAMS, NULL},
    {"cpssDxCh3PolicerPacketSizeModeForTunnelTermGet", 3, cpssDxCh3PolicerPacketSizeModeForTunnelTermGet_PARAMS, NULL},
    {"cpssDxCh3PolicerMeteringAutoRefreshScanEnableSet", 2, prvCpssLogGenDevNumEnable_PARAMS, NULL},
    {"cpssDxCh3PolicerMeteringAutoRefreshScanEnableGet", 2, prvCpssLogGenDevNumEnablePtr_PARAMS, NULL},
    {"cpssDxCh3PolicerMeteringAutoRefreshRangeSet", 3, cpssDxCh3PolicerMeteringAutoRefreshRangeSet_PARAMS, NULL},
    {"cpssDxCh3PolicerMeteringAutoRefreshRangeGet", 3, cpssDxCh3PolicerMeteringAutoRefreshRangeGet_PARAMS, NULL},
    {"cpssDxCh3PolicerMeteringAutoRefreshIntervalSet", 2, cpssDxCh3PolicerMeteringAutoRefreshIntervalSet_PARAMS, NULL},
    {"cpssDxCh3PolicerMeteringAutoRefreshIntervalGet", 2, cpssDxCh3PolicerMeteringAutoRefreshIntervalGet_PARAMS, NULL},
    {"cpssDxCh3PolicerMeteringEntryRefresh", 3, cpssDxCh3PolicerMeteringEntryRefresh_PARAMS, NULL},
    {"cpssDxCh3PolicerPortMeteringEnableSet", 4, cpssDxCh3PolicerPortMeteringEnableSet_PARAMS, NULL},
    {"cpssDxCh3PolicerPortMeteringEnableGet", 4, cpssDxCh3PolicerPortMeteringEnableGet_PARAMS, NULL},
    {"cpssDxCh3PolicerMruSet", 3, cpssDxCh3PolicerMruSet_PARAMS, NULL},
    {"cpssDxCh3PolicerMruGet", 3, cpssDxCh3PolicerMruGet_PARAMS, NULL},
    {"cpssDxCh3PolicerErrorGet", 4, cpssDxCh3PolicerErrorGet_PARAMS, NULL},
    {"cpssDxCh3PolicerErrorCounterGet", 3, cpssDxCh3PolicerErrorCounterGet_PARAMS, NULL},
    {"cpssDxCh3PolicerManagementCountersSet", 5, cpssDxCh3PolicerManagementCountersSet_PARAMS, NULL},
    {"cpssDxCh3PolicerManagementCountersGet", 5, cpssDxCh3PolicerManagementCountersGet_PARAMS, NULL},
    {"cpssDxCh3PolicerMeteringEntrySet", 5, cpssDxCh3PolicerMeteringEntrySet_PARAMS, cpssDxCh3PolicerMeteringEntrySet_preLogic},
    {"cpssDxCh3PolicerMeteringEntryGet", 4, cpssDxCh3PolicerMeteringEntryGet_PARAMS, NULL},
    {"cpssDxCh3PolicerEntryMeterParamsCalculate", 5, cpssDxCh3PolicerEntryMeterParamsCalculate_PARAMS, cpssDxCh3PolicerEntryMeterParamsCalculate_preLogic},
    {"cpssDxCh3PolicerBillingEntrySet", 4, cpssDxCh3PolicerBillingEntrySet_PARAMS, NULL},
    {"cpssDxCh3PolicerBillingEntryGet", 5, cpssDxCh3PolicerBillingEntryGet_PARAMS, NULL},
    {"cpssDxCh3PolicerQosRemarkingEntrySet", 6, cpssDxCh3PolicerQosRemarkingEntrySet_PARAMS, NULL},
    {"cpssDxCh3PolicerQosRemarkingEntryGet", 6, cpssDxCh3PolicerQosRemarkingEntryGet_PARAMS, NULL},
    {"cpssDxChPolicerEgressQosRemarkingEntrySet", 5, cpssDxChPolicerEgressQosRemarkingEntrySet_PARAMS, NULL},
    {"cpssDxChPolicerEgressQosRemarkingEntryGet", 5, cpssDxChPolicerEgressQosRemarkingEntryGet_PARAMS, NULL},
    {"cpssDxChPolicerStageMeterModeSet", 3, cpssDxChPolicerStageMeterModeSet_PARAMS, NULL},
    {"cpssDxChPolicerStageMeterModeGet", 3, cpssDxChPolicerStageMeterModeGet_PARAMS, NULL},
    {"cpssDxChPolicerMeteringOnTrappedPktsEnableSet", 3, prvCpssLogGenDevNumStageEnable_PARAMS, NULL},
    {"cpssDxChPolicerMeteringOnTrappedPktsEnableGet", 3, prvCpssLogGenDevNumStageEnablePtr_PARAMS, NULL},
    {"cpssDxChPolicerPortStormTypeIndexSet", 5, cpssDxChPolicerPortStormTypeIndexSet_PARAMS, NULL},
    {"cpssDxChPolicerPortStormTypeIndexGet", 5, cpssDxChPolicerPortStormTypeIndexGet_PARAMS, NULL},
    {"cpssDxChPolicerEgressL2RemarkModelSet", 2, cpssDxChPolicerEgressL2RemarkModelSet_PARAMS, NULL},
    {"cpssDxChPolicerEgressL2RemarkModelGet", 2, cpssDxChPolicerEgressL2RemarkModelGet_PARAMS, NULL},
    {"cpssDxChPolicerEgressQosUpdateEnableSet", 2, prvCpssLogGenDevNumEnable_PARAMS, NULL},
    {"cpssDxChPolicerEgressQosUpdateEnableGet", 2, prvCpssLogGenDevNumEnablePtr_PARAMS, NULL},
    {"cpssDxChPolicerVlanCntrSet", 4, cpssDxChPolicerVlanCntrSet_PARAMS, NULL},
    {"cpssDxChPolicerVlanCntrGet", 4, cpssDxChPolicerVlanCntrGet_PARAMS, NULL},
    {"cpssDxChPolicerVlanCountingPacketCmdTriggerSet", 4, cpssDxChPolicerVlanCountingPacketCmdTriggerSet_PARAMS, NULL},
    {"cpssDxChPolicerVlanCountingPacketCmdTriggerGet", 4, cpssDxChPolicerVlanCountingPacketCmdTriggerGet_PARAMS, NULL},
    {"cpssDxChPolicerVlanCountingModeSet", 3, cpssDxChPolicerVlanCountingModeSet_PARAMS, NULL},
    {"cpssDxChPolicerVlanCountingModeGet", 3, cpssDxChPolicerVlanCountingModeGet_PARAMS, NULL},
    {"cpssDxChPolicerPolicyCntrSet", 4, cpssDxChPolicerPolicyCntrSet_PARAMS, NULL},
    {"cpssDxChPolicerPolicyCntrGet", 4, cpssDxChPolicerPolicyCntrGet_PARAMS, NULL},
    {"cpssDxChPolicerMemorySizeModeSet", 2, cpssDxChPolicerMemorySizeModeSet_PARAMS, NULL},
    {"cpssDxChPolicerMemorySizeModeGet", 2, cpssDxChPolicerMemorySizeModeGet_PARAMS, NULL},
    {"cpssDxChPolicerCountingWriteBackCacheFlush", 2, cpssDxChPolicerCountingWriteBackCacheFlush_PARAMS, NULL},
    {"cpssDxChPolicerMeteringCalcMethodSet", 4, cpssDxChPolicerMeteringCalcMethodSet_PARAMS, NULL},
    {"cpssDxChPolicerMeteringCalcMethodGet", 4, cpssDxChPolicerMeteringCalcMethodGet_PARAMS, NULL},
    {"cpssDxChPolicerCountingTriggerByPortEnableSet", 3, prvCpssLogGenDevNumStageEnable_PARAMS, NULL},
    {"cpssDxChPolicerCountingTriggerByPortEnableGet", 3, prvCpssLogGenDevNumStageEnablePtr_PARAMS, NULL},
    {"cpssDxChPolicerPortModeAddressSelectSet", 3, cpssDxChPolicerPortModeAddressSelectSet_PARAMS, NULL},
    {"cpssDxChPolicerPortModeAddressSelectGet", 3, cpssDxChPolicerPortModeAddressSelectGet_PARAMS, NULL},
    {"cpssDxChPolicerTriggerEntrySet", 4, cpssDxChPolicerTriggerEntrySet_PARAMS, NULL},
    {"cpssDxChPolicerTriggerEntryGet", 4, cpssDxChPolicerTriggerEntryGet_PARAMS, NULL},
    {"cpssDxChPolicerPortGroupMeteringEntryRefresh", 4, cpssDxChPolicerPortGroupMeteringEntryRefresh_PARAMS, NULL},
    {"cpssDxChPolicerPortGroupErrorGet", 5, cpssDxChPolicerPortGroupErrorGet_PARAMS, NULL},
    {"cpssDxChPolicerPortGroupErrorCounterGet", 4, cpssDxChPolicerPortGroupErrorCounterGet_PARAMS, NULL},
    {"cpssDxChPolicerPortGroupManagementCountersSet", 6, cpssDxChPolicerPortGroupManagementCountersSet_PARAMS, NULL},
    {"cpssDxChPolicerPortGroupManagementCountersGet", 6, cpssDxChPolicerPortGroupManagementCountersGet_PARAMS, NULL},
    {"cpssDxChPolicerPortGroupMeteringEntrySet", 6, cpssDxChPolicerPortGroupMeteringEntrySet_PARAMS, cpssDxChPolicerPortGroupMeteringEntrySet_preLogic},
    {"cpssDxChPolicerPortGroupMeteringEntryGet", 5, cpssDxChPolicerPortGroupMeteringEntryGet_PARAMS, NULL},
    {"cpssDxChPolicerPortGroupBillingEntrySet", 5, cpssDxChPolicerPortGroupBillingEntrySet_PARAMS, NULL},
    {"cpssDxChPolicerPortGroupBillingEntryGet", 6, cpssDxChPolicerPortGroupBillingEntryGet_PARAMS, NULL},
    {"cpssDxChPolicerPortGroupPolicyCntrSet", 5, cpssDxChPolicerPortGroupPolicyCntrSet_PARAMS, NULL},
    {"cpssDxChPolicerPortGroupPolicyCntrGet", 5, cpssDxChPolicerPortGroupPolicyCntrGet_PARAMS, NULL},
    {"cpssDxChPolicerTrappedPacketsBillingEnableSet", 3, prvCpssLogGenDevNumStageEnable_PARAMS, NULL},
    {"cpssDxChPolicerTrappedPacketsBillingEnableGet", 3, prvCpssLogGenDevNumStageEnablePtr_PARAMS, NULL},
    {"cpssDxChPolicerLossMeasurementCounterCaptureEnableSet", 3, prvCpssLogGenDevNumStageEnable_PARAMS, NULL},
    {"cpssDxChPolicerLossMeasurementCounterCaptureEnableGet", 3, prvCpssLogGenDevNumStageEnablePtr_PARAMS, NULL},
    {"cpssDxChPolicerEAttributesMeteringModeSet", 3, cpssDxChPolicerEAttributesMeteringModeSet_PARAMS, NULL},
    {"cpssDxChPolicerEAttributesMeteringModeGet", 3, cpssDxChPolicerEAttributesMeteringModeGet_PARAMS, NULL},
    {"cpssDxChPolicerFlowIdCountingCfgSet", 3, cpssDxChPolicerFlowIdCountingCfgSet_PARAMS, NULL},
    {"cpssDxChPolicerFlowIdCountingCfgGet", 3, cpssDxChPolicerFlowIdCountingCfgGet_PARAMS, NULL},
    {"cpssDxChPolicerHierarchicalTableEntrySet", 3, cpssDxChPolicerHierarchicalTableEntrySet_PARAMS, NULL},
    {"cpssDxChPolicerHierarchicalTableEntryGet", 3, cpssDxChPolicerHierarchicalTableEntryGet_PARAMS, NULL},
    {"cpssDxChPolicerSecondStageIndexMaskSet", 2, cpssDxChPolicerSecondStageIndexMaskSet_PARAMS, NULL},
    {"cpssDxChPolicerSecondStageIndexMaskGet", 2, cpssDxChPolicerSecondStageIndexMaskGet_PARAMS, NULL},
    {"cpssDxChPolicerPortStormTypeInFlowModeEnableSet", 3, prvCpssLogGenDevNumStageEnable_PARAMS, NULL},
    {"cpssDxChPolicerPortStormTypeInFlowModeEnableGet", 3, prvCpssLogGenDevNumStageEnablePtr_PARAMS, NULL},
    {"cpssDxChPolicerPacketSizeModeForTimestampTagSet", 3, cpssDxChPolicerPacketSizeModeForTimestampTagSet_PARAMS, NULL},
    {"cpssDxChPolicerPacketSizeModeForTimestampTagGet", 3, cpssDxChPolicerPacketSizeModeForTimestampTagGet_PARAMS, NULL},
    {"cpssDxChPolicerFlowBasedMeteringToFloodedOnlyEnableSet", 2, prvCpssLogGenDevNumEnable_PARAMS, NULL},
    {"cpssDxChPolicerFlowBasedMeteringToFloodedOnlyEnableGet", 2, prvCpssLogGenDevNumEnablePtr_PARAMS, NULL},
    {"cpssDxChPolicerFlowBasedBillingToFloodedOnlyEnableSet", 2, prvCpssLogGenDevNumEnable_PARAMS, NULL},
    {"cpssDxChPolicerFlowBasedBillingToFloodedOnlyEnableGet", 2, prvCpssLogGenDevNumEnablePtr_PARAMS, NULL},
    {"cpssDxChPolicerMeteringEntryEnvelopeSet", 7, cpssDxChPolicerMeteringEntryEnvelopeSet_PARAMS, cpssDxChPolicerMeteringEntryEnvelopeSet_preLogic},
    {"cpssDxChPolicerMeteringEntryEnvelopeGet", 7, cpssDxChPolicerMeteringEntryEnvelopeGet_PARAMS, cpssDxChPolicerMeteringEntryEnvelopeGet_preLogic},
    {"cpssDxChPolicerMeterTableFlowBasedIndexConfigSet", 4, cpssDxChPolicerMeterTableFlowBasedIndexConfigSet_PARAMS, NULL},
    {"cpssDxChPolicerMeterTableFlowBasedIndexConfigGet", 4, cpssDxChPolicerMeterTableFlowBasedIndexConfigGet_PARAMS, NULL},
    {"cpssDxChPolicerTokenBucketMaxRateSet", 7, cpssDxChPolicerTokenBucketMaxRateSet_PARAMS, NULL},
    {"cpssDxChPolicerTokenBucketMaxRateGet", 7, cpssDxChPolicerTokenBucketMaxRateGet_PARAMS, NULL},
    {"cpssDxChPolicerQosProfileToPriorityMapSet", 4, cpssDxChPolicerQosProfileToPriorityMapSet_PARAMS, NULL},
    {"cpssDxChPolicerQosProfileToPriorityMapGet", 4, cpssDxChPolicerQosProfileToPriorityMapGet_PARAMS, NULL},
    {"cpssDxChPolicerPortGroupBucketsCurrentStateGet", 5, cpssDxChPolicerPortGroupBucketsCurrentStateGet_PARAMS, NULL},
    {"cpssDxChPolicerBucketsCurrentStateGet", 4, cpssDxChPolicerBucketsCurrentStateGet_PARAMS, NULL},
};

/******** DB Access Function ********/
void prvCpssLogParamLibDbGet_CPSS_LOG_LIB_POLICER(
    OUT PRV_CPSS_LOG_FUNC_ENTRY_STC ** logFuncDbPtrPtr,
    OUT GT_U32 * logFuncDbSizePtr
)
{
    *logFuncDbPtrPtr = prvCpssDxChPolicerLogLibDb;
    *logFuncDbSizePtr = sizeof(prvCpssDxChPolicerLogLibDb) / sizeof(PRV_CPSS_LOG_FUNC_ENTRY_STC);
}

