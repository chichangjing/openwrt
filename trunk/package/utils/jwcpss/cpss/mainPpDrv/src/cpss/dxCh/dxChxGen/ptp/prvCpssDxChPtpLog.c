/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* prvCpssDxChPtpLog.c
*       WARNING!!! this is a generated file, please don't edit it manually
* COMMENTS:
*
* FILE REVISION NUMBER:
*       $Revision: 1 $
*******************************************************************************/
#include <cpss/generic/log/cpssLog.h>
#include <cpss/generic/log/prvCpssLog.h>
#include <cpss/extServices/private/prvCpssBindFunc.h>
#include <cpss/dxCh/dxChxGen/ptp/cpssDxChPtp.h>
#include <cpss/dxCh/dxChxGen/ptp/private/prvCpssDxChPtpLog.h>
#include <cpss/generic/log/prvCpssGenLog.h>
#include <cpss/generic/networkIf/private/prvCpssGenNetworkIfLog.h>


/********* enums *********/

char * prvCpssLogEnum_CPSS_DXCH_PTP_INGRESS_CHECKING_MODE_ENT[]  =
{
    "CPSS_DXCH_PTP_INGRESS_CHECKING_MODE_BASIC_E",
    "CPSS_DXCH_PTP_INGRESS_CHECKING_MODE_TRANS_CLK_E",
    "CPSS_DXCH_PTP_INGRESS_CHECKING_MODE_PIGGY_BACK_E",
    "CPSS_DXCH_PTP_INGRESS_CHECKING_MODE_BOUNDRY_CLK_E"
};
PRV_CPSS_LOG_STC_ENUM_ARRAY_SIZE_MAC(CPSS_DXCH_PTP_INGRESS_CHECKING_MODE_ENT);
char * prvCpssLogEnum_CPSS_DXCH_PTP_INGRESS_DOMAIN_MODE_ENT[]  =
{
    "CPSS_DXCH_PTP_INGRESS_DOMAIN_MODE_DISABLE_E",
    "CPSS_DXCH_PTP_INGRESS_DOMAIN_MODE_PTP_V1_E",
    "CPSS_DXCH_PTP_INGRESS_DOMAIN_MODE_PTP_V2_E"
};
PRV_CPSS_LOG_STC_ENUM_ARRAY_SIZE_MAC(CPSS_DXCH_PTP_INGRESS_DOMAIN_MODE_ENT);
char * prvCpssLogEnum_CPSS_DXCH_PTP_TAI_CLOCK_MODE_ENT[]  =
{
    "CPSS_DXCH_PTP_TAI_CLOCK_MODE_DISABLED_E",
    "CPSS_DXCH_PTP_TAI_CLOCK_MODE_OUTPUT_E",
    "CPSS_DXCH_PTP_TAI_CLOCK_MODE_INPUT_E",
    "CPSS_DXCH_PTP_TAI_CLOCK_MODE_INPUT_ADJUST_E"
};
PRV_CPSS_LOG_STC_ENUM_ARRAY_SIZE_MAC(CPSS_DXCH_PTP_TAI_CLOCK_MODE_ENT);
char * prvCpssLogEnum_CPSS_DXCH_PTP_TAI_CLOCK_SELECT_ENT[]  =
{
    "CPSS_DXCH_PTP_TAI_CLOCK_SELECT_PTP_PLL_E",
    "CPSS_DXCH_PTP_TAI_CLOCK_SELECT_CORE_PLL_E"
};
PRV_CPSS_LOG_STC_ENUM_ARRAY_SIZE_MAC(CPSS_DXCH_PTP_TAI_CLOCK_SELECT_ENT);
char * prvCpssLogEnum_CPSS_DXCH_PTP_TAI_NUMBER_ENT[]  =
{
    "CPSS_DXCH_PTP_TAI_NUMBER_0_E",
    "CPSS_DXCH_PTP_TAI_NUMBER_1_E",
    "CPSS_DXCH_PTP_TAI_NUMBER_ALL_E"
};
PRV_CPSS_LOG_STC_ENUM_ARRAY_SIZE_MAC(CPSS_DXCH_PTP_TAI_NUMBER_ENT);
char * prvCpssLogEnum_CPSS_DXCH_PTP_TAI_PCLK_RCVR_CLK_MODE_ENT[]  =
{
    "CPSS_DXCH_PTP_TAI_PCLK_RCVR_CLK_MODE_PCLK_E",
    "CPSS_DXCH_PTP_TAI_PCLK_RCVR_CLK_MODE_RCVR_CLK_E"
};
PRV_CPSS_LOG_STC_ENUM_ARRAY_SIZE_MAC(CPSS_DXCH_PTP_TAI_PCLK_RCVR_CLK_MODE_ENT);
char * prvCpssLogEnum_CPSS_DXCH_PTP_TAI_TOD_TYPE_ENT[]  =
{
    "CPSS_DXCH_PTP_TAI_TOD_TYPE_TRIGGER_GENERATION_E",
    "CPSS_DXCH_PTP_TAI_TOD_TYPE_TRIGGER_GENERATION_MASK_E",
    "CPSS_DXCH_PTP_TAI_TOD_TYPE_LOAD_VALUE_E",
    "CPSS_DXCH_PTP_TAI_TOD_TYPE_CAPTURE_VALUE0_E",
    "CPSS_DXCH_PTP_TAI_TOD_TYPE_CAPTURE_VALUE1_E"
};
PRV_CPSS_LOG_STC_ENUM_ARRAY_SIZE_MAC(CPSS_DXCH_PTP_TAI_TOD_TYPE_ENT);
char * prvCpssLogEnum_CPSS_DXCH_PTP_TOD_COUNTER_FUNC_ENT[]  =
{
    "CPSS_DXCH_PTP_TOD_COUNTER_FUNC_UPDATE_E",
    "CPSS_DXCH_PTP_TOD_COUNTER_FUNC_INCREMENT_E",
    "CPSS_DXCH_PTP_TOD_COUNTER_FUNC_CAPTURE_E",
    "CPSS_DXCH_PTP_TOD_COUNTER_FUNC_GENERATE_E",
    "CPSS_DXCH_PTP_TOD_COUNTER_FUNC_DECREMENT_E",
    "CPSS_DXCH_PTP_TOD_COUNTER_FUNC_GRACEFULLY_INCREMENT_E",
    "CPSS_DXCH_PTP_TOD_COUNTER_FUNC_GRACEFULLY_DECREMENT_E",
    "CPSS_DXCH_PTP_TOD_COUNTER_FUNC_FREQUENCE_UPDATE_E",
    "CPSS_DXCH_PTP_TOD_COUNTER_FUNC_NOP_E"
};
PRV_CPSS_LOG_STC_ENUM_ARRAY_SIZE_MAC(CPSS_DXCH_PTP_TOD_COUNTER_FUNC_ENT);
char * prvCpssLogEnum_CPSS_DXCH_PTP_TSU_PACKET_COUNTER_TYPE_ENT[]  =
{
    "CPSS_DXCH_PTP_TSU_PACKET_COUNTER_TYPE_TOTAL_E",
    "CPSS_DXCH_PTP_TSU_PACKET_COUNTER_TYPE_PTP_V1_E",
    "CPSS_DXCH_PTP_TSU_PACKET_COUNTER_TYPE_PTP_V2_E",
    "CPSS_DXCH_PTP_TSU_PACKET_COUNTER_TYPE_Y1731_E",
    "CPSS_DXCH_PTP_TSU_PACKET_COUNTER_TYPE_NTP_TS_E",
    "CPSS_DXCH_PTP_TSU_PACKET_COUNTER_TYPE_NTP_RX_E",
    "CPSS_DXCH_PTP_TSU_PACKET_COUNTER_TYPE_NTP_TX_E",
    "CPSS_DXCH_PTP_TSU_PACKET_COUNTER_TYPE_WAMP_E",
    "CPSS_DXCH_PTP_TSU_PACKET_COUNTER_TYPE_NONE_ACTION_E",
    "CPSS_DXCH_PTP_TSU_PACKET_COUNTER_TYPE_FORWARD_E",
    "CPSS_DXCH_PTP_TSU_PACKET_COUNTER_TYPE_DROP_E",
    "CPSS_DXCH_PTP_TSU_PACKET_COUNTER_TYPE_CAPTURE_E",
    "CPSS_DXCH_PTP_TSU_PACKET_COUNTER_TYPE_ADD_TIME_E",
    "CPSS_DXCH_PTP_TSU_PACKET_COUNTER_TYPE_ADD_CORR_TIME_E",
    "CPSS_DXCH_PTP_TSU_PACKET_COUNTER_TYPE_CAPTURE_ADD_TIME_E",
    "CPSS_DXCH_PTP_TSU_PACKET_COUNTER_TYPE_CAPTURE_ADD_CORR_TIME_E",
    "CPSS_DXCH_PTP_TSU_PACKET_COUNTER_TYPE_ADD_INGRESS_TIME_E",
    "CPSS_DXCH_PTP_TSU_PACKET_COUNTER_TYPE_CAPTURE_ADD_INGRESS_TIME_E",
    "CPSS_DXCH_PTP_TSU_PACKET_COUNTER_TYPE_CAPTURE_INGRESS_TIME_E"
};
PRV_CPSS_LOG_STC_ENUM_ARRAY_SIZE_MAC(CPSS_DXCH_PTP_TSU_PACKET_COUNTER_TYPE_ENT);
char * prvCpssLogEnum_CPSS_DXCH_PTP_TS_DELAY_TAI_SEL_MODE_ENT[]  =
{
    "CPSS_DXCH_PTP_TS_DELAY_TAI_SEL_MODE_INGRESS_E",
    "CPSS_DXCH_PTP_TS_DELAY_TAI_SEL_MODE_EGRESS_E"
};
PRV_CPSS_LOG_STC_ENUM_ARRAY_SIZE_MAC(CPSS_DXCH_PTP_TS_DELAY_TAI_SEL_MODE_ENT);
char * prvCpssLogEnum_CPSS_DXCH_PTP_TAI_INSTANCE_ENT[]  =
{
    "CPSS_DXCH_PTP_TAI_INSTANCE_GLOBAL_E",
    "CPSS_DXCH_PTP_TAI_INSTANCE_PORT_E",
    "CPSS_DXCH_PTP_TAI_INSTANCE_ALL_E"
};
PRV_CPSS_LOG_STC_ENUM_ARRAY_SIZE_MAC(CPSS_DXCH_PTP_TAI_INSTANCE_ENT);
char * prvCpssLogEnum_CPSS_DXCH_PTP_TRANSPORT_TYPE_ENT[]  =
{
    "CPSS_DXCH_PTP_TRANSPORT_TYPE_ETHERNET_E",
    "CPSS_DXCH_PTP_TRANSPORT_TYPE_UDP_IPV4_E",
    "CPSS_DXCH_PTP_TRANSPORT_TYPE_UDP_IPV6_E"
};
PRV_CPSS_LOG_STC_ENUM_ARRAY_SIZE_MAC(CPSS_DXCH_PTP_TRANSPORT_TYPE_ENT);
char * prvCpssLogEnum_CPSS_DXCH_PTP_TS_ACTION_ENT[]  =
{
    "CPSS_DXCH_PTP_TS_ACTION_NONE_E",
    "CPSS_DXCH_PTP_TS_ACTION_DROP_E",
    "CPSS_DXCH_PTP_TS_ACTION_CAPTURE_E",
    "CPSS_DXCH_PTP_TS_ACTION_ADD_TIME_E",
    "CPSS_DXCH_PTP_TS_ACTION_ADD_CORRECTED_TIME_E",
    "CPSS_DXCH_PTP_TS_ACTION_CAPTURE_ADD_TIME_E",
    "CPSS_DXCH_PTP_TS_ACTION_CAPTURE_ADD_CORRECTED_TIME_E",
    "CPSS_DXCH_PTP_TS_ACTION_ADD_INGRESS_TIME_E",
    "CPSS_DXCH_PTP_TS_ACTION_CAPTURE_INGRESS_TIME_E",
    "CPSS_DXCH_PTP_TS_ACTION_CAPTURE_ADD_INGRESS_TIME_E"
};
PRV_CPSS_LOG_STC_ENUM_ARRAY_SIZE_MAC(CPSS_DXCH_PTP_TS_ACTION_ENT);
char * prvCpssLogEnum_CPSS_DXCH_PTP_TS_PACKET_TYPE_ENT[]  =
{
    "CPSS_DXCH_PTP_TS_PACKET_TYPE_PTP_V1_E",
    "CPSS_DXCH_PTP_TS_PACKET_TYPE_PTP_V2_E",
    "CPSS_DXCH_PTP_TS_PACKET_TYPE_Y1731_E",
    "CPSS_DXCH_PTP_TS_PACKET_TYPE_NTP_TS_E",
    "CPSS_DXCH_PTP_TS_PACKET_TYPE_NTP_RX_E",
    "CPSS_DXCH_PTP_TS_PACKET_TYPE_NTP_TX_E",
    "CPSS_DXCH_PTP_TS_PACKET_TYPE_WAMP_E",
    "CPSS_DXCH_PTP_TS_PACKET_TYPE_RESERVED_E"
};
PRV_CPSS_LOG_STC_ENUM_ARRAY_SIZE_MAC(CPSS_DXCH_PTP_TS_PACKET_TYPE_ENT);
char * prvCpssLogEnum_CPSS_DXCH_PTP_TS_TAG_MODE_ENT[]  =
{
    "CPSS_DXCH_PTP_TS_TAG_MODE_NONE_E",
    "CPSS_DXCH_PTP_TS_TAG_MODE_ALL_NON_EXTENDED_E",
    "CPSS_DXCH_PTP_TS_TAG_MODE_ALL_EXTENDED_E",
    "CPSS_DXCH_PTP_TS_TAG_MODE_NON_EXTENDED_E",
    "CPSS_DXCH_PTP_TS_TAG_MODE_EXTENDED_E",
    "CPSS_DXCH_PTP_TS_TAG_MODE_PIGGYBACK_E",
    "CPSS_DXCH_PTP_TS_TAG_MODE_HYBRID_E"
};
PRV_CPSS_LOG_STC_ENUM_ARRAY_SIZE_MAC(CPSS_DXCH_PTP_TS_TAG_MODE_ENT);
char * prvCpssLogEnum_CPSS_DXCH_PTP_TS_TIMESTAMPING_MODE_ENT[]  =
{
    "CPSS_DXCH_PTP_TS_TIMESTAMPING_MODE_DO_ACTION_E",
    "CPSS_DXCH_PTP_TS_TIMESTAMPING_MODE_MODIFY_TAG_E"
};
PRV_CPSS_LOG_STC_ENUM_ARRAY_SIZE_MAC(CPSS_DXCH_PTP_TS_TIMESTAMPING_MODE_ENT);
char * prvCpssLogEnum_CPSS_DXCH_PTP_TS_UDP_CHECKSUM_UPDATE_MODE_ENT[]  =
{
    "CPSS_DXCH_PTP_TS_UDP_CHECKSUM_UPDATE_MODE_CLEAR_E",
    "CPSS_DXCH_PTP_TS_UDP_CHECKSUM_UPDATE_MODE_RECALCULATE_E",
    "CPSS_DXCH_PTP_TS_UDP_CHECKSUM_UPDATE_MODE_KEEP_E"
};
PRV_CPSS_LOG_STC_ENUM_ARRAY_SIZE_MAC(CPSS_DXCH_PTP_TS_UDP_CHECKSUM_UPDATE_MODE_ENT);


/********* structure fields log functions *********/

void prvCpssLogParamFuncStc_CPSS_DXCH_PTP_EGRESS_DOMAIN_ENTRY_STC_PTR
(
    IN CPSS_LOG_LIB_ENT         contextLib,
    IN CPSS_LOG_TYPE_ENT        logType,
    IN GT_CHAR_PTR              namePtr,
    IN void                   * fieldPtr,
    INOUT PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC     * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_START_FIELD_STC_MAC(CPSS_DXCH_PTP_EGRESS_DOMAIN_ENTRY_STC *, valPtr);
    PRV_CPSS_LOG_UNUSED_MAC(inOutParamInfoPtr);
    PRV_CPSS_LOG_STC_BOOL_MAC(valPtr, ptpOverEhernetTsEnable);
    PRV_CPSS_LOG_STC_BOOL_MAC(valPtr, ptpOverUdpIpv4TsEnable);
    PRV_CPSS_LOG_STC_BOOL_MAC(valPtr, ptpOverUdpIpv6TsEnable);
    PRV_CPSS_LOG_STC_NUMBER_MAC(valPtr, messageTypeTsEnableBmp);
    PRV_CPSS_LOG_STC_NUMBER_MAC(valPtr, transportSpecificTsEnableBmp);
    prvCpssLogStcLogEnd(contextLib, logType);
}
void prvCpssLogParamFuncStc_CPSS_DXCH_PTP_EGRESS_EXCEPTION_CFG_STC_PTR
(
    IN CPSS_LOG_LIB_ENT         contextLib,
    IN CPSS_LOG_TYPE_ENT        logType,
    IN GT_CHAR_PTR              namePtr,
    IN void                   * fieldPtr,
    INOUT PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC     * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_START_FIELD_STC_MAC(CPSS_DXCH_PTP_EGRESS_EXCEPTION_CFG_STC *, valPtr);
    PRV_CPSS_LOG_UNUSED_MAC(inOutParamInfoPtr);
    PRV_CPSS_LOG_STC_ENUM_MAC(valPtr, invalidPtpPktCmd, CPSS_PACKET_CMD_ENT);
    PRV_CPSS_LOG_STC_ENUM_MAC(valPtr, invalidOutPiggybackPktCmd, CPSS_PACKET_CMD_ENT);
    PRV_CPSS_LOG_STC_ENUM_MAC(valPtr, invalidInPiggybackPktCmd, CPSS_PACKET_CMD_ENT);
    PRV_CPSS_LOG_STC_ENUM_MAC(valPtr, invalidTsPktCmd, CPSS_PACKET_CMD_ENT);
    prvCpssLogStcLogEnd(contextLib, logType);
}
void prvCpssLogParamFuncStc_CPSS_DXCH_PTP_EGRESS_EXCEPTION_COUNTERS_STC_PTR
(
    IN CPSS_LOG_LIB_ENT         contextLib,
    IN CPSS_LOG_TYPE_ENT        logType,
    IN GT_CHAR_PTR              namePtr,
    IN void                   * fieldPtr,
    INOUT PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC     * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_START_FIELD_STC_MAC(CPSS_DXCH_PTP_EGRESS_EXCEPTION_COUNTERS_STC *, valPtr);
    PRV_CPSS_LOG_UNUSED_MAC(inOutParamInfoPtr);
    PRV_CPSS_LOG_STC_NUMBER_MAC(valPtr, invalidPtpPktCnt);
    PRV_CPSS_LOG_STC_NUMBER_MAC(valPtr, invalidOutPiggybackPktCnt);
    PRV_CPSS_LOG_STC_NUMBER_MAC(valPtr, invalidInPiggybackPktCnt);
    PRV_CPSS_LOG_STC_NUMBER_MAC(valPtr, invalidTsPktCnt);
    prvCpssLogStcLogEnd(contextLib, logType);
}
void prvCpssLogParamFuncStc_CPSS_DXCH_PTP_INGRESS_EXCEPTION_CFG_STC_PTR
(
    IN CPSS_LOG_LIB_ENT         contextLib,
    IN CPSS_LOG_TYPE_ENT        logType,
    IN GT_CHAR_PTR              namePtr,
    IN void                   * fieldPtr,
    INOUT PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC     * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_START_FIELD_STC_MAC(CPSS_DXCH_PTP_INGRESS_EXCEPTION_CFG_STC *, valPtr);
    PRV_CPSS_LOG_UNUSED_MAC(inOutParamInfoPtr);
    PRV_CPSS_LOG_STC_BOOL_MAC(valPtr, ptpExceptionCommandEnable);
    PRV_CPSS_LOG_STC_ENUM_MAC(valPtr, ptpExceptionCommand, CPSS_PACKET_CMD_ENT);
    PRV_CPSS_LOG_STC_ENUM_MAP_MAC(valPtr, ptpExceptionCpuCode, CPSS_NET_RX_CPU_CODE_ENT);
    PRV_CPSS_LOG_STC_BOOL_MAC(valPtr, ptpVersionCheckEnable);
    prvCpssLogStcLogEnd(contextLib, logType);
}
void prvCpssLogParamFuncStc_CPSS_DXCH_PTP_TAI_ID_STC_PTR
(
    IN CPSS_LOG_LIB_ENT         contextLib,
    IN CPSS_LOG_TYPE_ENT        logType,
    IN GT_CHAR_PTR              namePtr,
    IN void                   * fieldPtr,
    INOUT PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC     * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_START_FIELD_STC_MAC(CPSS_DXCH_PTP_TAI_ID_STC *, valPtr);
    PRV_CPSS_LOG_UNUSED_MAC(inOutParamInfoPtr);
    PRV_CPSS_LOG_STC_ENUM_MAC(valPtr, taiInstance, CPSS_DXCH_PTP_TAI_INSTANCE_ENT);
    PRV_CPSS_LOG_STC_ENUM_MAC(valPtr, taiNumber, CPSS_DXCH_PTP_TAI_NUMBER_ENT);
    PRV_CPSS_LOG_STC_NUMBER_MAC(valPtr, portNum);
    prvCpssLogStcLogEnd(contextLib, logType);
}
void prvCpssLogParamFuncStc_CPSS_DXCH_PTP_TAI_TOD_STEP_STC_PTR
(
    IN CPSS_LOG_LIB_ENT         contextLib,
    IN CPSS_LOG_TYPE_ENT        logType,
    IN GT_CHAR_PTR              namePtr,
    IN void                   * fieldPtr,
    INOUT PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC     * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_START_FIELD_STC_MAC(CPSS_DXCH_PTP_TAI_TOD_STEP_STC *, valPtr);
    PRV_CPSS_LOG_UNUSED_MAC(inOutParamInfoPtr);
    PRV_CPSS_LOG_STC_NUMBER_MAC(valPtr, nanoSeconds);
    PRV_CPSS_LOG_STC_NUMBER_MAC(valPtr, fracNanoSeconds);
    prvCpssLogStcLogEnd(contextLib, logType);
}
void prvCpssLogParamFuncStc_CPSS_DXCH_PTP_TIMESTAMP_ENTRY_STC_PTR
(
    IN CPSS_LOG_LIB_ENT         contextLib,
    IN CPSS_LOG_TYPE_ENT        logType,
    IN GT_CHAR_PTR              namePtr,
    IN void                   * fieldPtr,
    INOUT PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC     * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_START_FIELD_STC_MAC(CPSS_DXCH_PTP_TIMESTAMP_ENTRY_STC *, valPtr);
    PRV_CPSS_LOG_UNUSED_MAC(inOutParamInfoPtr);
    PRV_CPSS_LOG_STC_NUMBER_MAC(valPtr, portNum);
    PRV_CPSS_LOG_STC_NUMBER_MAC(valPtr, messageType);
    PRV_CPSS_LOG_STC_NUMBER_MAC(valPtr, sequenceId);
    PRV_CPSS_LOG_STC_NUMBER_MAC(valPtr, timeStampVal);
    prvCpssLogStcLogEnd(contextLib, logType);
}
void prvCpssLogParamFuncStc_CPSS_DXCH_PTP_TOD_COUNT_STC_PTR
(
    IN CPSS_LOG_LIB_ENT         contextLib,
    IN CPSS_LOG_TYPE_ENT        logType,
    IN GT_CHAR_PTR              namePtr,
    IN void                   * fieldPtr,
    INOUT PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC     * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_START_FIELD_STC_MAC(CPSS_DXCH_PTP_TOD_COUNT_STC *, valPtr);
    PRV_CPSS_LOG_UNUSED_MAC(inOutParamInfoPtr);
    PRV_CPSS_LOG_STC_NUMBER_MAC(valPtr, nanoSeconds);
    PRV_CPSS_LOG_STC_STC_MAC(valPtr, seconds, GT_U64);
    PRV_CPSS_LOG_STC_NUMBER_MAC(valPtr, fracNanoSeconds);
    prvCpssLogStcLogEnd(contextLib, logType);
}
void prvCpssLogParamFuncStc_CPSS_DXCH_PTP_TSU_CONTROL_STC_PTR
(
    IN CPSS_LOG_LIB_ENT         contextLib,
    IN CPSS_LOG_TYPE_ENT        logType,
    IN GT_CHAR_PTR              namePtr,
    IN void                   * fieldPtr,
    INOUT PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC     * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_START_FIELD_STC_MAC(CPSS_DXCH_PTP_TSU_CONTROL_STC *, valPtr);
    PRV_CPSS_LOG_UNUSED_MAC(inOutParamInfoPtr);
    PRV_CPSS_LOG_STC_BOOL_MAC(valPtr, unitEnable);
    PRV_CPSS_LOG_STC_NUMBER_MAC(valPtr, rxTaiSelect);
    PRV_CPSS_LOG_STC_BOOL_MAC(valPtr, tsQueOverrideEnable);
    prvCpssLogStcLogEnd(contextLib, logType);
}
void prvCpssLogParamFuncStc_CPSS_DXCH_PTP_TSU_TX_TIMESTAMP_QUEUE_ENTRY_STC_PTR
(
    IN CPSS_LOG_LIB_ENT         contextLib,
    IN CPSS_LOG_TYPE_ENT        logType,
    IN GT_CHAR_PTR              namePtr,
    IN void                   * fieldPtr,
    INOUT PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC     * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_START_FIELD_STC_MAC(CPSS_DXCH_PTP_TSU_TX_TIMESTAMP_QUEUE_ENTRY_STC *, valPtr);
    PRV_CPSS_LOG_UNUSED_MAC(inOutParamInfoPtr);
    PRV_CPSS_LOG_STC_BOOL_MAC(valPtr, entryValid);
    PRV_CPSS_LOG_STC_NUMBER_MAC(valPtr, entryId);
    PRV_CPSS_LOG_STC_NUMBER_MAC(valPtr, taiSelect);
    PRV_CPSS_LOG_STC_NUMBER_MAC(valPtr, todUpdateFlag);
    PRV_CPSS_LOG_STC_NUMBER_MAC(valPtr, timestamp);
    prvCpssLogStcLogEnd(contextLib, logType);
}
void prvCpssLogParamFuncStc_CPSS_DXCH_PTP_TS_CFG_ENTRY_STC_PTR
(
    IN CPSS_LOG_LIB_ENT         contextLib,
    IN CPSS_LOG_TYPE_ENT        logType,
    IN GT_CHAR_PTR              namePtr,
    IN void                   * fieldPtr,
    INOUT PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC     * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_START_FIELD_STC_MAC(CPSS_DXCH_PTP_TS_CFG_ENTRY_STC *, valPtr);
    PRV_CPSS_LOG_UNUSED_MAC(inOutParamInfoPtr);
    PRV_CPSS_LOG_STC_ENUM_MAC(valPtr, tsMode, CPSS_DXCH_PTP_TS_TIMESTAMPING_MODE_ENT);
    PRV_CPSS_LOG_STC_NUMBER_MAC(valPtr, offsetProfile);
    PRV_CPSS_LOG_STC_BOOL_MAC(valPtr, OE);
    PRV_CPSS_LOG_STC_ENUM_MAC(valPtr, tsAction, CPSS_DXCH_PTP_TS_ACTION_ENT);
    PRV_CPSS_LOG_STC_ENUM_MAC(valPtr, packetFormat, CPSS_DXCH_PTP_TS_PACKET_TYPE_ENT);
    PRV_CPSS_LOG_STC_ENUM_MAC(valPtr, ptpTransport, CPSS_DXCH_PTP_TRANSPORT_TYPE_ENT);
    PRV_CPSS_LOG_STC_NUMBER_MAC(valPtr, offset);
    PRV_CPSS_LOG_STC_NUMBER_MAC(valPtr, ptpMessageType);
    PRV_CPSS_LOG_STC_NUMBER_MAC(valPtr, domain);
    PRV_CPSS_LOG_STC_BOOL_MAC(valPtr, ingrLinkDelayEnable);
    PRV_CPSS_LOG_STC_BOOL_MAC(valPtr, packetDispatchingEnable);
    prvCpssLogStcLogEnd(contextLib, logType);
}
void prvCpssLogParamFuncStc_CPSS_DXCH_PTP_TS_EGRESS_TIMESTAMP_QUEUE_ENTRY_STC_PTR
(
    IN CPSS_LOG_LIB_ENT         contextLib,
    IN CPSS_LOG_TYPE_ENT        logType,
    IN GT_CHAR_PTR              namePtr,
    IN void                   * fieldPtr,
    INOUT PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC     * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_START_FIELD_STC_MAC(CPSS_DXCH_PTP_TS_EGRESS_TIMESTAMP_QUEUE_ENTRY_STC *, valPtr);
    PRV_CPSS_LOG_UNUSED_MAC(inOutParamInfoPtr);
    PRV_CPSS_LOG_STC_BOOL_MAC(valPtr, entryValid);
    PRV_CPSS_LOG_STC_BOOL_MAC(valPtr, isPtpExeption);
    PRV_CPSS_LOG_STC_ENUM_MAC(valPtr, packetFormat, CPSS_DXCH_PTP_TS_PACKET_TYPE_ENT);
    PRV_CPSS_LOG_STC_NUMBER_MAC(valPtr, messageType);
    PRV_CPSS_LOG_STC_NUMBER_MAC(valPtr, domainNum);
    PRV_CPSS_LOG_STC_NUMBER_MAC(valPtr, sequenceId);
    PRV_CPSS_LOG_STC_NUMBER_MAC(valPtr, queueEntryId);
    PRV_CPSS_LOG_STC_NUMBER_MAC(valPtr, portNum);
    prvCpssLogStcLogEnd(contextLib, logType);
}
void prvCpssLogParamFuncStc_CPSS_DXCH_PTP_TS_INGRESS_TIMESTAMP_QUEUE_ENTRY_STC_PTR
(
    IN CPSS_LOG_LIB_ENT         contextLib,
    IN CPSS_LOG_TYPE_ENT        logType,
    IN GT_CHAR_PTR              namePtr,
    IN void                   * fieldPtr,
    INOUT PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC     * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_START_FIELD_STC_MAC(CPSS_DXCH_PTP_TS_INGRESS_TIMESTAMP_QUEUE_ENTRY_STC *, valPtr);
    PRV_CPSS_LOG_UNUSED_MAC(inOutParamInfoPtr);
    PRV_CPSS_LOG_STC_BOOL_MAC(valPtr, entryValid);
    PRV_CPSS_LOG_STC_BOOL_MAC(valPtr, isPtpExeption);
    PRV_CPSS_LOG_STC_ENUM_MAC(valPtr, packetFormat, CPSS_DXCH_PTP_TS_PACKET_TYPE_ENT);
    PRV_CPSS_LOG_STC_NUMBER_MAC(valPtr, taiSelect);
    PRV_CPSS_LOG_STC_NUMBER_MAC(valPtr, todUpdateFlag);
    PRV_CPSS_LOG_STC_NUMBER_MAC(valPtr, messageType);
    PRV_CPSS_LOG_STC_NUMBER_MAC(valPtr, domainNum);
    PRV_CPSS_LOG_STC_NUMBER_MAC(valPtr, sequenceId);
    PRV_CPSS_LOG_STC_NUMBER_MAC(valPtr, timestamp);
    PRV_CPSS_LOG_STC_NUMBER_MAC(valPtr, portNum);
    prvCpssLogStcLogEnd(contextLib, logType);
}
void prvCpssLogParamFuncStc_CPSS_DXCH_PTP_TS_LOCAL_ACTION_ENTRY_STC_PTR
(
    IN CPSS_LOG_LIB_ENT         contextLib,
    IN CPSS_LOG_TYPE_ENT        logType,
    IN GT_CHAR_PTR              namePtr,
    IN void                   * fieldPtr,
    INOUT PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC     * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_START_FIELD_STC_MAC(CPSS_DXCH_PTP_TS_LOCAL_ACTION_ENTRY_STC *, valPtr);
    PRV_CPSS_LOG_UNUSED_MAC(inOutParamInfoPtr);
    PRV_CPSS_LOG_STC_ENUM_MAC(valPtr, tsAction, CPSS_DXCH_PTP_TS_ACTION_ENT);
    PRV_CPSS_LOG_STC_BOOL_MAC(valPtr, ingrLinkDelayEnable);
    PRV_CPSS_LOG_STC_BOOL_MAC(valPtr, packetDispatchingEnable);
    prvCpssLogStcLogEnd(contextLib, logType);
}
void prvCpssLogParamFuncStc_CPSS_DXCH_PTP_TS_TAG_GLOBAL_CFG_STC_PTR
(
    IN CPSS_LOG_LIB_ENT         contextLib,
    IN CPSS_LOG_TYPE_ENT        logType,
    IN GT_CHAR_PTR              namePtr,
    IN void                   * fieldPtr,
    INOUT PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC     * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_START_FIELD_STC_MAC(CPSS_DXCH_PTP_TS_TAG_GLOBAL_CFG_STC *, valPtr);
    PRV_CPSS_LOG_UNUSED_MAC(inOutParamInfoPtr);
    PRV_CPSS_LOG_STC_BOOL_MAC(valPtr, tsTagParseEnable);
    PRV_CPSS_LOG_STC_BOOL_MAC(valPtr, hybridTsTagParseEnable);
    PRV_CPSS_LOG_STC_NUMBER_MAC(valPtr, tsTagEtherType);
    PRV_CPSS_LOG_STC_NUMBER_MAC(valPtr, hybridTsTagEtherType);
    prvCpssLogStcLogEnd(contextLib, logType);
}
void prvCpssLogParamFuncStc_CPSS_DXCH_PTP_TS_TAG_PORT_CFG_STC_PTR
(
    IN CPSS_LOG_LIB_ENT         contextLib,
    IN CPSS_LOG_TYPE_ENT        logType,
    IN GT_CHAR_PTR              namePtr,
    IN void                   * fieldPtr,
    INOUT PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC     * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_START_FIELD_STC_MAC(CPSS_DXCH_PTP_TS_TAG_PORT_CFG_STC *, valPtr);
    PRV_CPSS_LOG_UNUSED_MAC(inOutParamInfoPtr);
    PRV_CPSS_LOG_STC_BOOL_MAC(valPtr, tsReceptionEnable);
    PRV_CPSS_LOG_STC_BOOL_MAC(valPtr, tsPiggyBackEnable);
    PRV_CPSS_LOG_STC_ENUM_MAC(valPtr, tsTagMode, CPSS_DXCH_PTP_TS_TAG_MODE_ENT);
    prvCpssLogStcLogEnd(contextLib, logType);
}
void prvCpssLogParamFuncStc_CPSS_DXCH_PTP_TS_UDP_CHECKSUM_UPDATE_MODE_STC_PTR
(
    IN CPSS_LOG_LIB_ENT         contextLib,
    IN CPSS_LOG_TYPE_ENT        logType,
    IN GT_CHAR_PTR              namePtr,
    IN void                   * fieldPtr,
    INOUT PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC     * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_START_FIELD_STC_MAC(CPSS_DXCH_PTP_TS_UDP_CHECKSUM_UPDATE_MODE_STC *, valPtr);
    PRV_CPSS_LOG_UNUSED_MAC(inOutParamInfoPtr);
    PRV_CPSS_LOG_STC_ENUM_MAC(valPtr, ptpIpv4Mode, CPSS_DXCH_PTP_TS_UDP_CHECKSUM_UPDATE_MODE_ENT);
    PRV_CPSS_LOG_STC_ENUM_MAC(valPtr, ptpIpv6Mode, CPSS_DXCH_PTP_TS_UDP_CHECKSUM_UPDATE_MODE_ENT);
    PRV_CPSS_LOG_STC_ENUM_MAC(valPtr, ntpIpv4Mode, CPSS_DXCH_PTP_TS_UDP_CHECKSUM_UPDATE_MODE_ENT);
    PRV_CPSS_LOG_STC_ENUM_MAC(valPtr, ntpIpv6Mode, CPSS_DXCH_PTP_TS_UDP_CHECKSUM_UPDATE_MODE_ENT);
    PRV_CPSS_LOG_STC_ENUM_MAC(valPtr, wampIpv4Mode, CPSS_DXCH_PTP_TS_UDP_CHECKSUM_UPDATE_MODE_ENT);
    PRV_CPSS_LOG_STC_ENUM_MAC(valPtr, wampIpv6Mode, CPSS_DXCH_PTP_TS_UDP_CHECKSUM_UPDATE_MODE_ENT);
    prvCpssLogStcLogEnd(contextLib, logType);
}


/********* parameters log functions *********/

void prvCpssLogParamFunc_CPSS_DXCH_PTP_EGRESS_DOMAIN_ENTRY_STC_PTR(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_START_PARAM_STC_MAC(CPSS_DXCH_PTP_EGRESS_DOMAIN_ENTRY_STC*, paramVal);
    prvCpssLogParamFuncStc_CPSS_DXCH_PTP_EGRESS_DOMAIN_ENTRY_STC_PTR(contextLib, logType, namePtr, paramVal, inOutParamInfoPtr);
}
void prvCpssLogParamFunc_CPSS_DXCH_PTP_EGRESS_EXCEPTION_CFG_STC_PTR(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_START_PARAM_STC_MAC(CPSS_DXCH_PTP_EGRESS_EXCEPTION_CFG_STC*, paramVal);
    prvCpssLogParamFuncStc_CPSS_DXCH_PTP_EGRESS_EXCEPTION_CFG_STC_PTR(contextLib, logType, namePtr, paramVal, inOutParamInfoPtr);
}
void prvCpssLogParamFunc_CPSS_DXCH_PTP_EGRESS_EXCEPTION_COUNTERS_STC_PTR(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_START_PARAM_STC_MAC(CPSS_DXCH_PTP_EGRESS_EXCEPTION_COUNTERS_STC*, paramVal);
    prvCpssLogParamFuncStc_CPSS_DXCH_PTP_EGRESS_EXCEPTION_COUNTERS_STC_PTR(contextLib, logType, namePtr, paramVal, inOutParamInfoPtr);
}
void prvCpssLogParamFunc_CPSS_DXCH_PTP_INGRESS_CHECKING_MODE_ENT(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_SET_PARAM_VAL_MAC(CPSS_DXCH_PTP_INGRESS_CHECKING_MODE_ENT, paramVal);
    PRV_CPSS_LOG_ENUM_MAC(namePtr, paramVal, CPSS_DXCH_PTP_INGRESS_CHECKING_MODE_ENT);
}
void prvCpssLogParamFunc_CPSS_DXCH_PTP_INGRESS_CHECKING_MODE_ENT_PTR(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_SET_PARAM_VAL_MAC(CPSS_DXCH_PTP_INGRESS_CHECKING_MODE_ENT*, paramVal);
    if (paramVal == NULL)
    {
        PRV_CPSS_LOG_AND_HISTORY_PARAM_MAC(contextLib, logType, "%s = NULL\n", namePtr);
        return;
    }
    PRV_CPSS_LOG_ENUM_MAC(namePtr, *paramVal, CPSS_DXCH_PTP_INGRESS_CHECKING_MODE_ENT);
}
void prvCpssLogParamFunc_CPSS_DXCH_PTP_INGRESS_DOMAIN_MODE_ENT(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_SET_PARAM_VAL_MAC(CPSS_DXCH_PTP_INGRESS_DOMAIN_MODE_ENT, paramVal);
    PRV_CPSS_LOG_ENUM_MAC(namePtr, paramVal, CPSS_DXCH_PTP_INGRESS_DOMAIN_MODE_ENT);
}
void prvCpssLogParamFunc_CPSS_DXCH_PTP_INGRESS_DOMAIN_MODE_ENT_PTR(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_SET_PARAM_VAL_MAC(CPSS_DXCH_PTP_INGRESS_DOMAIN_MODE_ENT*, paramVal);
    if (paramVal == NULL)
    {
        PRV_CPSS_LOG_AND_HISTORY_PARAM_MAC(contextLib, logType, "%s = NULL\n", namePtr);
        return;
    }
    PRV_CPSS_LOG_ENUM_MAC(namePtr, *paramVal, CPSS_DXCH_PTP_INGRESS_DOMAIN_MODE_ENT);
}
void prvCpssLogParamFunc_CPSS_DXCH_PTP_INGRESS_EXCEPTION_CFG_STC_PTR(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_START_PARAM_STC_MAC(CPSS_DXCH_PTP_INGRESS_EXCEPTION_CFG_STC*, paramVal);
    prvCpssLogParamFuncStc_CPSS_DXCH_PTP_INGRESS_EXCEPTION_CFG_STC_PTR(contextLib, logType, namePtr, paramVal, inOutParamInfoPtr);
}
void prvCpssLogParamFunc_CPSS_DXCH_PTP_TAI_CLOCK_MODE_ENT(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_SET_PARAM_VAL_MAC(CPSS_DXCH_PTP_TAI_CLOCK_MODE_ENT, paramVal);
    PRV_CPSS_LOG_ENUM_MAC(namePtr, paramVal, CPSS_DXCH_PTP_TAI_CLOCK_MODE_ENT);
}
void prvCpssLogParamFunc_CPSS_DXCH_PTP_TAI_CLOCK_MODE_ENT_PTR(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_SET_PARAM_VAL_MAC(CPSS_DXCH_PTP_TAI_CLOCK_MODE_ENT*, paramVal);
    if (paramVal == NULL)
    {
        PRV_CPSS_LOG_AND_HISTORY_PARAM_MAC(contextLib, logType, "%s = NULL\n", namePtr);
        return;
    }
    PRV_CPSS_LOG_ENUM_MAC(namePtr, *paramVal, CPSS_DXCH_PTP_TAI_CLOCK_MODE_ENT);
}
void prvCpssLogParamFunc_CPSS_DXCH_PTP_TAI_CLOCK_SELECT_ENT(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_SET_PARAM_VAL_MAC(CPSS_DXCH_PTP_TAI_CLOCK_SELECT_ENT, paramVal);
    PRV_CPSS_LOG_ENUM_MAC(namePtr, paramVal, CPSS_DXCH_PTP_TAI_CLOCK_SELECT_ENT);
}
void prvCpssLogParamFunc_CPSS_DXCH_PTP_TAI_CLOCK_SELECT_ENT_PTR(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_SET_PARAM_VAL_MAC(CPSS_DXCH_PTP_TAI_CLOCK_SELECT_ENT*, paramVal);
    if (paramVal == NULL)
    {
        PRV_CPSS_LOG_AND_HISTORY_PARAM_MAC(contextLib, logType, "%s = NULL\n", namePtr);
        return;
    }
    PRV_CPSS_LOG_ENUM_MAC(namePtr, *paramVal, CPSS_DXCH_PTP_TAI_CLOCK_SELECT_ENT);
}
void prvCpssLogParamFunc_CPSS_DXCH_PTP_TAI_ID_STC_PTR(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_START_PARAM_STC_MAC(CPSS_DXCH_PTP_TAI_ID_STC*, paramVal);
    prvCpssLogParamFuncStc_CPSS_DXCH_PTP_TAI_ID_STC_PTR(contextLib, logType, namePtr, paramVal, inOutParamInfoPtr);
}
void prvCpssLogParamFunc_CPSS_DXCH_PTP_TAI_NUMBER_ENT(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_SET_PARAM_VAL_MAC(CPSS_DXCH_PTP_TAI_NUMBER_ENT, paramVal);
    PRV_CPSS_LOG_ENUM_MAC(namePtr, paramVal, CPSS_DXCH_PTP_TAI_NUMBER_ENT);
}
void prvCpssLogParamFunc_CPSS_DXCH_PTP_TAI_NUMBER_ENT_PTR(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_SET_PARAM_VAL_MAC(CPSS_DXCH_PTP_TAI_NUMBER_ENT*, paramVal);
    if (paramVal == NULL)
    {
        PRV_CPSS_LOG_AND_HISTORY_PARAM_MAC(contextLib, logType, "%s = NULL\n", namePtr);
        return;
    }
    PRV_CPSS_LOG_ENUM_MAC(namePtr, *paramVal, CPSS_DXCH_PTP_TAI_NUMBER_ENT);
}
void prvCpssLogParamFunc_CPSS_DXCH_PTP_TAI_PCLK_RCVR_CLK_MODE_ENT(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_SET_PARAM_VAL_MAC(CPSS_DXCH_PTP_TAI_PCLK_RCVR_CLK_MODE_ENT, paramVal);
    PRV_CPSS_LOG_ENUM_MAC(namePtr, paramVal, CPSS_DXCH_PTP_TAI_PCLK_RCVR_CLK_MODE_ENT);
}
void prvCpssLogParamFunc_CPSS_DXCH_PTP_TAI_PCLK_RCVR_CLK_MODE_ENT_PTR(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_SET_PARAM_VAL_MAC(CPSS_DXCH_PTP_TAI_PCLK_RCVR_CLK_MODE_ENT*, paramVal);
    if (paramVal == NULL)
    {
        PRV_CPSS_LOG_AND_HISTORY_PARAM_MAC(contextLib, logType, "%s = NULL\n", namePtr);
        return;
    }
    PRV_CPSS_LOG_ENUM_MAC(namePtr, *paramVal, CPSS_DXCH_PTP_TAI_PCLK_RCVR_CLK_MODE_ENT);
}
void prvCpssLogParamFunc_CPSS_DXCH_PTP_TAI_TOD_STEP_STC_PTR(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_START_PARAM_STC_MAC(CPSS_DXCH_PTP_TAI_TOD_STEP_STC*, paramVal);
    prvCpssLogParamFuncStc_CPSS_DXCH_PTP_TAI_TOD_STEP_STC_PTR(contextLib, logType, namePtr, paramVal, inOutParamInfoPtr);
}
void prvCpssLogParamFunc_CPSS_DXCH_PTP_TAI_TOD_TYPE_ENT(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_SET_PARAM_VAL_MAC(CPSS_DXCH_PTP_TAI_TOD_TYPE_ENT, paramVal);
    PRV_CPSS_LOG_ENUM_MAC(namePtr, paramVal, CPSS_DXCH_PTP_TAI_TOD_TYPE_ENT);
}
void prvCpssLogParamFunc_CPSS_DXCH_PTP_TIMESTAMP_ENTRY_STC_PTR(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_START_PARAM_STC_MAC(CPSS_DXCH_PTP_TIMESTAMP_ENTRY_STC*, paramVal);
    prvCpssLogParamFuncStc_CPSS_DXCH_PTP_TIMESTAMP_ENTRY_STC_PTR(contextLib, logType, namePtr, paramVal, inOutParamInfoPtr);
}
void prvCpssLogParamFunc_CPSS_DXCH_PTP_TOD_COUNTER_FUNC_ENT(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_SET_PARAM_VAL_MAC(CPSS_DXCH_PTP_TOD_COUNTER_FUNC_ENT, paramVal);
    PRV_CPSS_LOG_ENUM_MAC(namePtr, paramVal, CPSS_DXCH_PTP_TOD_COUNTER_FUNC_ENT);
}
void prvCpssLogParamFunc_CPSS_DXCH_PTP_TOD_COUNTER_FUNC_ENT_PTR(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_SET_PARAM_VAL_MAC(CPSS_DXCH_PTP_TOD_COUNTER_FUNC_ENT*, paramVal);
    if (paramVal == NULL)
    {
        PRV_CPSS_LOG_AND_HISTORY_PARAM_MAC(contextLib, logType, "%s = NULL\n", namePtr);
        return;
    }
    PRV_CPSS_LOG_ENUM_MAC(namePtr, *paramVal, CPSS_DXCH_PTP_TOD_COUNTER_FUNC_ENT);
}
void prvCpssLogParamFunc_CPSS_DXCH_PTP_TOD_COUNT_STC_PTR(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_START_PARAM_STC_MAC(CPSS_DXCH_PTP_TOD_COUNT_STC*, paramVal);
    prvCpssLogParamFuncStc_CPSS_DXCH_PTP_TOD_COUNT_STC_PTR(contextLib, logType, namePtr, paramVal, inOutParamInfoPtr);
}
void prvCpssLogParamFunc_CPSS_DXCH_PTP_TSU_CONTROL_STC_PTR(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_START_PARAM_STC_MAC(CPSS_DXCH_PTP_TSU_CONTROL_STC*, paramVal);
    prvCpssLogParamFuncStc_CPSS_DXCH_PTP_TSU_CONTROL_STC_PTR(contextLib, logType, namePtr, paramVal, inOutParamInfoPtr);
}
void prvCpssLogParamFunc_CPSS_DXCH_PTP_TSU_PACKET_COUNTER_TYPE_ENT(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_SET_PARAM_VAL_MAC(CPSS_DXCH_PTP_TSU_PACKET_COUNTER_TYPE_ENT, paramVal);
    PRV_CPSS_LOG_ENUM_MAC(namePtr, paramVal, CPSS_DXCH_PTP_TSU_PACKET_COUNTER_TYPE_ENT);
}
void prvCpssLogParamFunc_CPSS_DXCH_PTP_TSU_TX_TIMESTAMP_QUEUE_ENTRY_STC_PTR(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_START_PARAM_STC_MAC(CPSS_DXCH_PTP_TSU_TX_TIMESTAMP_QUEUE_ENTRY_STC*, paramVal);
    prvCpssLogParamFuncStc_CPSS_DXCH_PTP_TSU_TX_TIMESTAMP_QUEUE_ENTRY_STC_PTR(contextLib, logType, namePtr, paramVal, inOutParamInfoPtr);
}
void prvCpssLogParamFunc_CPSS_DXCH_PTP_TS_CFG_ENTRY_STC_PTR(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_START_PARAM_STC_MAC(CPSS_DXCH_PTP_TS_CFG_ENTRY_STC*, paramVal);
    prvCpssLogParamFuncStc_CPSS_DXCH_PTP_TS_CFG_ENTRY_STC_PTR(contextLib, logType, namePtr, paramVal, inOutParamInfoPtr);
}
void prvCpssLogParamFunc_CPSS_DXCH_PTP_TS_DELAY_TAI_SEL_MODE_ENT(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_SET_PARAM_VAL_MAC(CPSS_DXCH_PTP_TS_DELAY_TAI_SEL_MODE_ENT, paramVal);
    PRV_CPSS_LOG_ENUM_MAC(namePtr, paramVal, CPSS_DXCH_PTP_TS_DELAY_TAI_SEL_MODE_ENT);
}
void prvCpssLogParamFunc_CPSS_DXCH_PTP_TS_DELAY_TAI_SEL_MODE_ENT_PTR(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_SET_PARAM_VAL_MAC(CPSS_DXCH_PTP_TS_DELAY_TAI_SEL_MODE_ENT*, paramVal);
    if (paramVal == NULL)
    {
        PRV_CPSS_LOG_AND_HISTORY_PARAM_MAC(contextLib, logType, "%s = NULL\n", namePtr);
        return;
    }
    PRV_CPSS_LOG_ENUM_MAC(namePtr, *paramVal, CPSS_DXCH_PTP_TS_DELAY_TAI_SEL_MODE_ENT);
}
void prvCpssLogParamFunc_CPSS_DXCH_PTP_TS_EGRESS_TIMESTAMP_QUEUE_ENTRY_STC_PTR(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_START_PARAM_STC_MAC(CPSS_DXCH_PTP_TS_EGRESS_TIMESTAMP_QUEUE_ENTRY_STC*, paramVal);
    prvCpssLogParamFuncStc_CPSS_DXCH_PTP_TS_EGRESS_TIMESTAMP_QUEUE_ENTRY_STC_PTR(contextLib, logType, namePtr, paramVal, inOutParamInfoPtr);
}
void prvCpssLogParamFunc_CPSS_DXCH_PTP_TS_INGRESS_TIMESTAMP_QUEUE_ENTRY_STC_PTR(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_START_PARAM_STC_MAC(CPSS_DXCH_PTP_TS_INGRESS_TIMESTAMP_QUEUE_ENTRY_STC*, paramVal);
    prvCpssLogParamFuncStc_CPSS_DXCH_PTP_TS_INGRESS_TIMESTAMP_QUEUE_ENTRY_STC_PTR(contextLib, logType, namePtr, paramVal, inOutParamInfoPtr);
}
void prvCpssLogParamFunc_CPSS_DXCH_PTP_TS_LOCAL_ACTION_ENTRY_STC_PTR(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_START_PARAM_STC_MAC(CPSS_DXCH_PTP_TS_LOCAL_ACTION_ENTRY_STC*, paramVal);
    prvCpssLogParamFuncStc_CPSS_DXCH_PTP_TS_LOCAL_ACTION_ENTRY_STC_PTR(contextLib, logType, namePtr, paramVal, inOutParamInfoPtr);
}
void prvCpssLogParamFunc_CPSS_DXCH_PTP_TS_TAG_GLOBAL_CFG_STC_PTR(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_START_PARAM_STC_MAC(CPSS_DXCH_PTP_TS_TAG_GLOBAL_CFG_STC*, paramVal);
    prvCpssLogParamFuncStc_CPSS_DXCH_PTP_TS_TAG_GLOBAL_CFG_STC_PTR(contextLib, logType, namePtr, paramVal, inOutParamInfoPtr);
}
void prvCpssLogParamFunc_CPSS_DXCH_PTP_TS_TAG_PORT_CFG_STC_PTR(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_START_PARAM_STC_MAC(CPSS_DXCH_PTP_TS_TAG_PORT_CFG_STC*, paramVal);
    prvCpssLogParamFuncStc_CPSS_DXCH_PTP_TS_TAG_PORT_CFG_STC_PTR(contextLib, logType, namePtr, paramVal, inOutParamInfoPtr);
}
void prvCpssLogParamFunc_CPSS_DXCH_PTP_TS_UDP_CHECKSUM_UPDATE_MODE_STC_PTR(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_START_PARAM_STC_MAC(CPSS_DXCH_PTP_TS_UDP_CHECKSUM_UPDATE_MODE_STC*, paramVal);
    prvCpssLogParamFuncStc_CPSS_DXCH_PTP_TS_UDP_CHECKSUM_UPDATE_MODE_STC_PTR(contextLib, logType, namePtr, paramVal, inOutParamInfoPtr);
}


/********* API fields DB *********/

PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_DIRECTION_ENT_clockInterfaceDirection = {
     "clockInterfaceDirection", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_MAC(CPSS_DIRECTION_ENT)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_DIRECTION_ENT_pulseInterfaceDirection = {
     "pulseInterfaceDirection", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_MAC(CPSS_DIRECTION_ENT)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_DXCH_PTP_EGRESS_DOMAIN_ENTRY_STC_PTR_entryPtr = {
     "entryPtr", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(CPSS_DXCH_PTP_EGRESS_DOMAIN_ENTRY_STC)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_DXCH_PTP_EGRESS_EXCEPTION_CFG_STC_PTR_egrExceptionCfgPtr = {
     "egrExceptionCfgPtr", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(CPSS_DXCH_PTP_EGRESS_EXCEPTION_CFG_STC)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_DXCH_PTP_INGRESS_CHECKING_MODE_ENT_checkingMode = {
     "checkingMode", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_MAC(CPSS_DXCH_PTP_INGRESS_CHECKING_MODE_ENT)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_DXCH_PTP_INGRESS_DOMAIN_MODE_ENT_domainMode = {
     "domainMode", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_MAC(CPSS_DXCH_PTP_INGRESS_DOMAIN_MODE_ENT)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_DXCH_PTP_INGRESS_EXCEPTION_CFG_STC_PTR_ingrExceptionCfgPtr = {
     "ingrExceptionCfgPtr", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(CPSS_DXCH_PTP_INGRESS_EXCEPTION_CFG_STC)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_DXCH_PTP_TAI_CLOCK_MODE_ENT_clockMode = {
     "clockMode", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_MAC(CPSS_DXCH_PTP_TAI_CLOCK_MODE_ENT)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_DXCH_PTP_TAI_CLOCK_SELECT_ENT_clockSelect = {
     "clockSelect", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_MAC(CPSS_DXCH_PTP_TAI_CLOCK_SELECT_ENT)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_DXCH_PTP_TAI_ID_STC_PTR_taiIdPtr = {
     "taiIdPtr", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(CPSS_DXCH_PTP_TAI_ID_STC)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_DXCH_PTP_TAI_NUMBER_ENT_taiNumber = {
     "taiNumber", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_MAC(CPSS_DXCH_PTP_TAI_NUMBER_ENT)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_DXCH_PTP_TAI_PCLK_RCVR_CLK_MODE_ENT_pclkRcvrClkMode = {
     "pclkRcvrClkMode", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_MAC(CPSS_DXCH_PTP_TAI_PCLK_RCVR_CLK_MODE_ENT)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_DXCH_PTP_TAI_TOD_STEP_STC_PTR_todStepPtr = {
     "todStepPtr", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(CPSS_DXCH_PTP_TAI_TOD_STEP_STC)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_DXCH_PTP_TAI_TOD_TYPE_ENT_todValueType = {
     "todValueType", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_MAC(CPSS_DXCH_PTP_TAI_TOD_TYPE_ENT)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_DXCH_PTP_TOD_COUNTER_FUNC_ENT_function = {
     "function", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_MAC(CPSS_DXCH_PTP_TOD_COUNTER_FUNC_ENT)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_DXCH_PTP_TOD_COUNT_STC_PTR_todCounterPtr = {
     "todCounterPtr", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(CPSS_DXCH_PTP_TOD_COUNT_STC)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_DXCH_PTP_TOD_COUNT_STC_PTR_todValuePtr = {
     "todValuePtr", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(CPSS_DXCH_PTP_TOD_COUNT_STC)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_DXCH_PTP_TSU_CONTROL_STC_PTR_controlPtr = {
     "controlPtr", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(CPSS_DXCH_PTP_TSU_CONTROL_STC)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_DXCH_PTP_TSU_PACKET_COUNTER_TYPE_ENT_counterType = {
     "counterType", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_MAC(CPSS_DXCH_PTP_TSU_PACKET_COUNTER_TYPE_ENT)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_DXCH_PTP_TS_CFG_ENTRY_STC_PTR_entryPtr = {
     "entryPtr", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(CPSS_DXCH_PTP_TS_CFG_ENTRY_STC)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_DXCH_PTP_TS_DELAY_TAI_SEL_MODE_ENT_egrTimeCorrTaiSelMode = {
     "egrTimeCorrTaiSelMode", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_MAC(CPSS_DXCH_PTP_TS_DELAY_TAI_SEL_MODE_ENT)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_DXCH_PTP_TS_LOCAL_ACTION_ENTRY_STC_PTR_entryPtr = {
     "entryPtr", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(CPSS_DXCH_PTP_TS_LOCAL_ACTION_ENTRY_STC)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_DXCH_PTP_TS_TAG_GLOBAL_CFG_STC_PTR_tsTagGlobalCfgPtr = {
     "tsTagGlobalCfgPtr", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(CPSS_DXCH_PTP_TS_TAG_GLOBAL_CFG_STC)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_DXCH_PTP_TS_TAG_PORT_CFG_STC_PTR_tsTagPortCfgPtr = {
     "tsTagPortCfgPtr", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(CPSS_DXCH_PTP_TS_TAG_PORT_CFG_STC)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_DXCH_PTP_TS_UDP_CHECKSUM_UPDATE_MODE_STC_PTR_udpCsUpdModePtr = {
     "udpCsUpdModePtr", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(CPSS_DXCH_PTP_TS_UDP_CHECKSUM_UPDATE_MODE_STC)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_32_egrAsymmetryCorr = {
     "egrAsymmetryCorr", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_MAC(GT_32)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_32_egrPipeDelayCorr = {
     "egrPipeDelayCorr", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_MAC(GT_32)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_32_fracNanoSecond = {
     "fracNanoSecond", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_MAC(GT_32)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_32_ingressDelay = {
     "ingressDelay", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_MAC(GT_32)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_32_ingressDelayCorr = {
     "ingressDelayCorr", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_MAC(GT_32)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_BOOL_egrCorrFldPBEnable = {
     "egrCorrFldPBEnable", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_MAC(GT_BOOL)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_BOOL_ingrCorrFldPBEnable = {
     "ingrCorrFldPBEnable", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_MAC(GT_BOOL)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_BOOL_resetRxUnit = {
     "resetRxUnit", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_MAC(GT_BOOL)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_BOOL_resetTxUnit = {
     "resetTxUnit", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_MAC(GT_BOOL)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_U32_PTR_domainIdArr = {
     "domainIdArr", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(GT_U32)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_U32_adjustThreshold = {
     "adjustThreshold", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_MAC(GT_U32)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_U32_captureIndex = {
     "captureIndex", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_MAC(GT_U32)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_U32_delay = {
     "delay", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_MAC(GT_U32)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_U32_domainId = {
     "domainId", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_MAC(GT_U32)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_U32_domainIndex = {
     "domainIndex", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_MAC(GT_U32)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_U32_domainProfile = {
     "domainProfile", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_MAC(GT_U32)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_U32_egrTsTaiNum = {
     "egrTsTaiNum", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_MAC(GT_U32)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_U32_etherTypeIndex = {
     "etherTypeIndex", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_MAC(GT_U32)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_U32_extPulseWidth = {
     "extPulseWidth", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_MAC(GT_U32)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_U32_gracefulStep = {
     "gracefulStep", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_MAC(GT_U32)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_U32_idMapBmp = {
     "idMapBmp", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_MAC(GT_U32)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_U32_interruptThreshold = {
     "interruptThreshold", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_MAC(GT_U32)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_U32_messageType = {
     "messageType", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_MAC(GT_U32)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_U32_nanoSeconds = {
     "nanoSeconds", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_MAC(GT_U32)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_U32_ntpTimeOffset = {
     "ntpTimeOffset", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_MAC(GT_U32)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_U32_queueNum = {
     "queueNum", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_MAC(GT_U32)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_U32_queueSize = {
     "queueSize", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_MAC(GT_U32)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_U32_seconds = {
     "seconds", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_MAC(GT_U32)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_U32_transportSpecificVal = {
     "transportSpecificVal", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_MAC(GT_U32)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_U32_udpPortIndex = {
     "udpPortIndex", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_MAC(GT_U32)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_U32_udpPortNum = {
     "udpPortNum", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_MAC(GT_U32)
};
PRV_CPSS_LOG_FUNC_PARAM_STC OUT_CPSS_DIRECTION_ENT_PTR_clockInterfaceDirectionPtr = {
     "clockInterfaceDirectionPtr", PRV_CPSS_LOG_PARAM_OUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(CPSS_DIRECTION_ENT)
};
PRV_CPSS_LOG_FUNC_PARAM_STC OUT_CPSS_DIRECTION_ENT_PTR_pulseInterfaceDirectionPtr = {
     "pulseInterfaceDirectionPtr", PRV_CPSS_LOG_PARAM_OUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(CPSS_DIRECTION_ENT)
};
PRV_CPSS_LOG_FUNC_PARAM_STC OUT_CPSS_DXCH_PTP_EGRESS_DOMAIN_ENTRY_STC_PTR_entryPtr = {
     "entryPtr", PRV_CPSS_LOG_PARAM_OUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(CPSS_DXCH_PTP_EGRESS_DOMAIN_ENTRY_STC)
};
PRV_CPSS_LOG_FUNC_PARAM_STC OUT_CPSS_DXCH_PTP_EGRESS_EXCEPTION_CFG_STC_PTR_egrExceptionCfgPtr = {
     "egrExceptionCfgPtr", PRV_CPSS_LOG_PARAM_OUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(CPSS_DXCH_PTP_EGRESS_EXCEPTION_CFG_STC)
};
PRV_CPSS_LOG_FUNC_PARAM_STC OUT_CPSS_DXCH_PTP_EGRESS_EXCEPTION_COUNTERS_STC_PTR_egrExceptionCntPtr = {
     "egrExceptionCntPtr", PRV_CPSS_LOG_PARAM_OUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(CPSS_DXCH_PTP_EGRESS_EXCEPTION_COUNTERS_STC)
};
PRV_CPSS_LOG_FUNC_PARAM_STC OUT_CPSS_DXCH_PTP_INGRESS_CHECKING_MODE_ENT_PTR_checkingModePtr = {
     "checkingModePtr", PRV_CPSS_LOG_PARAM_OUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(CPSS_DXCH_PTP_INGRESS_CHECKING_MODE_ENT)
};
PRV_CPSS_LOG_FUNC_PARAM_STC OUT_CPSS_DXCH_PTP_INGRESS_DOMAIN_MODE_ENT_PTR_domainModePtr = {
     "domainModePtr", PRV_CPSS_LOG_PARAM_OUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(CPSS_DXCH_PTP_INGRESS_DOMAIN_MODE_ENT)
};
PRV_CPSS_LOG_FUNC_PARAM_STC OUT_CPSS_DXCH_PTP_INGRESS_EXCEPTION_CFG_STC_PTR_ingrExceptionCfgPtr = {
     "ingrExceptionCfgPtr", PRV_CPSS_LOG_PARAM_OUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(CPSS_DXCH_PTP_INGRESS_EXCEPTION_CFG_STC)
};
PRV_CPSS_LOG_FUNC_PARAM_STC OUT_CPSS_DXCH_PTP_TAI_CLOCK_MODE_ENT_PTR_clockModePtr = {
     "clockModePtr", PRV_CPSS_LOG_PARAM_OUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(CPSS_DXCH_PTP_TAI_CLOCK_MODE_ENT)
};
PRV_CPSS_LOG_FUNC_PARAM_STC OUT_CPSS_DXCH_PTP_TAI_CLOCK_SELECT_ENT_PTR_clockSelectPtr = {
     "clockSelectPtr", PRV_CPSS_LOG_PARAM_OUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(CPSS_DXCH_PTP_TAI_CLOCK_SELECT_ENT)
};
PRV_CPSS_LOG_FUNC_PARAM_STC OUT_CPSS_DXCH_PTP_TAI_NUMBER_ENT_PTR_taiNumberPtr = {
     "taiNumberPtr", PRV_CPSS_LOG_PARAM_OUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(CPSS_DXCH_PTP_TAI_NUMBER_ENT)
};
PRV_CPSS_LOG_FUNC_PARAM_STC OUT_CPSS_DXCH_PTP_TAI_PCLK_RCVR_CLK_MODE_ENT_PTR_pclkRcvrClkModePtr = {
     "pclkRcvrClkModePtr", PRV_CPSS_LOG_PARAM_OUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(CPSS_DXCH_PTP_TAI_PCLK_RCVR_CLK_MODE_ENT)
};
PRV_CPSS_LOG_FUNC_PARAM_STC OUT_CPSS_DXCH_PTP_TAI_TOD_STEP_STC_PTR_todStepPtr = {
     "todStepPtr", PRV_CPSS_LOG_PARAM_OUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(CPSS_DXCH_PTP_TAI_TOD_STEP_STC)
};
PRV_CPSS_LOG_FUNC_PARAM_STC OUT_CPSS_DXCH_PTP_TIMESTAMP_ENTRY_STC_PTR_entryPtr = {
     "entryPtr", PRV_CPSS_LOG_PARAM_OUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(CPSS_DXCH_PTP_TIMESTAMP_ENTRY_STC)
};
PRV_CPSS_LOG_FUNC_PARAM_STC OUT_CPSS_DXCH_PTP_TOD_COUNTER_FUNC_ENT_PTR_functionPtr = {
     "functionPtr", PRV_CPSS_LOG_PARAM_OUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(CPSS_DXCH_PTP_TOD_COUNTER_FUNC_ENT)
};
PRV_CPSS_LOG_FUNC_PARAM_STC OUT_CPSS_DXCH_PTP_TOD_COUNT_STC_PTR_todCounterPtr = {
     "todCounterPtr", PRV_CPSS_LOG_PARAM_OUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(CPSS_DXCH_PTP_TOD_COUNT_STC)
};
PRV_CPSS_LOG_FUNC_PARAM_STC OUT_CPSS_DXCH_PTP_TOD_COUNT_STC_PTR_todValuePtr = {
     "todValuePtr", PRV_CPSS_LOG_PARAM_OUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(CPSS_DXCH_PTP_TOD_COUNT_STC)
};
PRV_CPSS_LOG_FUNC_PARAM_STC OUT_CPSS_DXCH_PTP_TSU_CONTROL_STC_PTR_controlPtr = {
     "controlPtr", PRV_CPSS_LOG_PARAM_OUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(CPSS_DXCH_PTP_TSU_CONTROL_STC)
};
PRV_CPSS_LOG_FUNC_PARAM_STC OUT_CPSS_DXCH_PTP_TSU_TX_TIMESTAMP_QUEUE_ENTRY_STC_PTR_entryPtr = {
     "entryPtr", PRV_CPSS_LOG_PARAM_OUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(CPSS_DXCH_PTP_TSU_TX_TIMESTAMP_QUEUE_ENTRY_STC)
};
PRV_CPSS_LOG_FUNC_PARAM_STC OUT_CPSS_DXCH_PTP_TS_CFG_ENTRY_STC_PTR_entryPtr = {
     "entryPtr", PRV_CPSS_LOG_PARAM_OUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(CPSS_DXCH_PTP_TS_CFG_ENTRY_STC)
};
PRV_CPSS_LOG_FUNC_PARAM_STC OUT_CPSS_DXCH_PTP_TS_DELAY_TAI_SEL_MODE_ENT_PTR_egrTimeCorrTaiSelModePtr = {
     "egrTimeCorrTaiSelModePtr", PRV_CPSS_LOG_PARAM_OUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(CPSS_DXCH_PTP_TS_DELAY_TAI_SEL_MODE_ENT)
};
PRV_CPSS_LOG_FUNC_PARAM_STC OUT_CPSS_DXCH_PTP_TS_EGRESS_TIMESTAMP_QUEUE_ENTRY_STC_PTR_tsQueueEntryPtr = {
     "tsQueueEntryPtr", PRV_CPSS_LOG_PARAM_OUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(CPSS_DXCH_PTP_TS_EGRESS_TIMESTAMP_QUEUE_ENTRY_STC)
};
PRV_CPSS_LOG_FUNC_PARAM_STC OUT_CPSS_DXCH_PTP_TS_INGRESS_TIMESTAMP_QUEUE_ENTRY_STC_PTR_tsQueueEntryPtr = {
     "tsQueueEntryPtr", PRV_CPSS_LOG_PARAM_OUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(CPSS_DXCH_PTP_TS_INGRESS_TIMESTAMP_QUEUE_ENTRY_STC)
};
PRV_CPSS_LOG_FUNC_PARAM_STC OUT_CPSS_DXCH_PTP_TS_LOCAL_ACTION_ENTRY_STC_PTR_entryPtr = {
     "entryPtr", PRV_CPSS_LOG_PARAM_OUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(CPSS_DXCH_PTP_TS_LOCAL_ACTION_ENTRY_STC)
};
PRV_CPSS_LOG_FUNC_PARAM_STC OUT_CPSS_DXCH_PTP_TS_TAG_GLOBAL_CFG_STC_PTR_tsTagGlobalCfgPtr = {
     "tsTagGlobalCfgPtr", PRV_CPSS_LOG_PARAM_OUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(CPSS_DXCH_PTP_TS_TAG_GLOBAL_CFG_STC)
};
PRV_CPSS_LOG_FUNC_PARAM_STC OUT_CPSS_DXCH_PTP_TS_TAG_PORT_CFG_STC_PTR_tsTagPortCfgPtr = {
     "tsTagPortCfgPtr", PRV_CPSS_LOG_PARAM_OUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(CPSS_DXCH_PTP_TS_TAG_PORT_CFG_STC)
};
PRV_CPSS_LOG_FUNC_PARAM_STC OUT_CPSS_DXCH_PTP_TS_UDP_CHECKSUM_UPDATE_MODE_STC_PTR_udpCsUpdModePtr = {
     "udpCsUpdModePtr", PRV_CPSS_LOG_PARAM_OUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(CPSS_DXCH_PTP_TS_UDP_CHECKSUM_UPDATE_MODE_STC)
};
PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_32_PTR_egrAsymmetryCorrPtr = {
     "egrAsymmetryCorrPtr", PRV_CPSS_LOG_PARAM_OUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(GT_32)
};
PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_32_PTR_egrPipeDelayCorrPtr = {
     "egrPipeDelayCorrPtr", PRV_CPSS_LOG_PARAM_OUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(GT_32)
};
PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_32_PTR_fracNanoSecondPtr = {
     "fracNanoSecondPtr", PRV_CPSS_LOG_PARAM_OUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(GT_32)
};
PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_32_PTR_ingressDelayCorrPtr = {
     "ingressDelayCorrPtr", PRV_CPSS_LOG_PARAM_OUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(GT_32)
};
PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_32_PTR_ingressDelayPtr = {
     "ingressDelayPtr", PRV_CPSS_LOG_PARAM_OUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(GT_32)
};
PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_BOOL_PTR_egrCorrFldPBEnablePtr = {
     "egrCorrFldPBEnablePtr", PRV_CPSS_LOG_PARAM_OUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(GT_BOOL)
};
PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_BOOL_PTR_ingrCorrFldPBEnablePtr = {
     "ingrCorrFldPBEnablePtr", PRV_CPSS_LOG_PARAM_OUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(GT_BOOL)
};
PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_BOOL_PTR_resetRxUnitPtr = {
     "resetRxUnitPtr", PRV_CPSS_LOG_PARAM_OUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(GT_BOOL)
};
PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_BOOL_PTR_resetTxUnitPtr = {
     "resetTxUnitPtr", PRV_CPSS_LOG_PARAM_OUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(GT_BOOL)
};
PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_U32_PTR_adjustThresholdPtr = {
     "adjustThresholdPtr", PRV_CPSS_LOG_PARAM_OUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(GT_U32)
};
PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_U32_PTR_delayPtr = {
     "delayPtr", PRV_CPSS_LOG_PARAM_OUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(GT_U32)
};
PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_U32_PTR_domainIdArr = {
     "domainIdArr", PRV_CPSS_LOG_PARAM_OUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(GT_U32)
};
PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_U32_PTR_domainIdPtr = {
     "domainIdPtr", PRV_CPSS_LOG_PARAM_OUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(GT_U32)
};
PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_U32_PTR_egrTsTaiNumPtr = {
     "egrTsTaiNumPtr", PRV_CPSS_LOG_PARAM_OUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(GT_U32)
};
PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_U32_PTR_extPulseWidthPtr = {
     "extPulseWidthPtr", PRV_CPSS_LOG_PARAM_OUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(GT_U32)
};
PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_U32_PTR_gracefulStepPtr = {
     "gracefulStepPtr", PRV_CPSS_LOG_PARAM_OUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(GT_U32)
};
PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_U32_PTR_idMapBmpPtr = {
     "idMapBmpPtr", PRV_CPSS_LOG_PARAM_OUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(GT_U32)
};
PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_U32_PTR_interruptThresholdPtr = {
     "interruptThresholdPtr", PRV_CPSS_LOG_PARAM_OUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(GT_U32)
};
PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_U32_PTR_nanoSecondsPtr = {
     "nanoSecondsPtr", PRV_CPSS_LOG_PARAM_OUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(GT_U32)
};
PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_U32_PTR_ntpTimeOffsetPtr = {
     "ntpTimeOffsetPtr", PRV_CPSS_LOG_PARAM_OUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(GT_U32)
};
PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_U32_PTR_queueEntryId0Ptr = {
     "queueEntryId0Ptr", PRV_CPSS_LOG_PARAM_OUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(GT_U32)
};
PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_U32_PTR_queueEntryId1Ptr = {
     "queueEntryId1Ptr", PRV_CPSS_LOG_PARAM_OUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(GT_U32)
};
PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_U32_PTR_queueSizePtr = {
     "queueSizePtr", PRV_CPSS_LOG_PARAM_OUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(GT_U32)
};
PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_U32_PTR_secondsPtr = {
     "secondsPtr", PRV_CPSS_LOG_PARAM_OUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(GT_U32)
};
PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_U32_PTR_udpPortNumPtr = {
     "udpPortNumPtr", PRV_CPSS_LOG_PARAM_OUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(GT_U32)
};


/********* API prototypes DB *********/

PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChPtpTaiPtpPulseIterfaceSet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_CPSS_DIRECTION_ENT_pulseInterfaceDirection,
    &IN_CPSS_DXCH_PTP_TAI_NUMBER_ENT_taiNumber
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChPtpEgressExceptionCfgSet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_CPSS_DXCH_PTP_EGRESS_EXCEPTION_CFG_STC_PTR_egrExceptionCfgPtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChPtpIngressExceptionCfgSet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_CPSS_DXCH_PTP_INGRESS_EXCEPTION_CFG_STC_PTR_ingrExceptionCfgPtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChPtpTaiClockModeSet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_CPSS_DXCH_PTP_TAI_ID_STC_PTR_taiIdPtr,
    &IN_CPSS_DXCH_PTP_TAI_CLOCK_MODE_ENT_clockMode
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChPtpTaiTodStepSet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_CPSS_DXCH_PTP_TAI_ID_STC_PTR_taiIdPtr,
    &IN_CPSS_DXCH_PTP_TAI_TOD_STEP_STC_PTR_todStepPtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChPtpTaiTodSet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_CPSS_DXCH_PTP_TAI_ID_STC_PTR_taiIdPtr,
    &IN_CPSS_DXCH_PTP_TAI_TOD_TYPE_ENT_todValueType,
    &IN_CPSS_DXCH_PTP_TOD_COUNT_STC_PTR_todValuePtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChPtpTaiTodGet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_CPSS_DXCH_PTP_TAI_ID_STC_PTR_taiIdPtr,
    &IN_CPSS_DXCH_PTP_TAI_TOD_TYPE_ENT_todValueType,
    &OUT_CPSS_DXCH_PTP_TOD_COUNT_STC_PTR_todValuePtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChPtpTaiFractionalNanosecondDriftSet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_CPSS_DXCH_PTP_TAI_ID_STC_PTR_taiIdPtr,
    &IN_GT_32_fracNanoSecond
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChPtpTaiInternalClockGenerateEnableSet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_CPSS_DXCH_PTP_TAI_ID_STC_PTR_taiIdPtr,
    &IN_GT_BOOL_enable
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChPtpTaiIncomingClockCounterSet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_CPSS_DXCH_PTP_TAI_ID_STC_PTR_taiIdPtr,
    &IN_GT_BOOL_enable,
    &IN_GT_U32_value
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChPtpTaiTodCaptureStatusSet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_CPSS_DXCH_PTP_TAI_ID_STC_PTR_taiIdPtr,
    &IN_GT_U32_captureIndex,
    &IN_GT_BOOL_valid
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChPtpTaiTodCaptureStatusGet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_CPSS_DXCH_PTP_TAI_ID_STC_PTR_taiIdPtr,
    &IN_GT_U32_captureIndex,
    &OUT_GT_BOOL_PTR_validPtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChPtpTaiExternalPulseWidthSet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_CPSS_DXCH_PTP_TAI_ID_STC_PTR_taiIdPtr,
    &IN_GT_U32_extPulseWidth
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChPtpTaiGracefulStepSet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_CPSS_DXCH_PTP_TAI_ID_STC_PTR_taiIdPtr,
    &IN_GT_U32_gracefulStep
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChPtpTaiFrequencyDriftThesholdsSet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_CPSS_DXCH_PTP_TAI_ID_STC_PTR_taiIdPtr,
    &IN_GT_U32_interruptThreshold,
    &IN_GT_U32_adjustThreshold
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChPtpTaiPulseDelaySet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_CPSS_DXCH_PTP_TAI_ID_STC_PTR_taiIdPtr,
    &IN_GT_U32_nanoSeconds
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChPtpTaiClockCycleSet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_CPSS_DXCH_PTP_TAI_ID_STC_PTR_taiIdPtr,
    &IN_GT_U32_seconds,
    &IN_GT_U32_nanoSeconds
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChPtpTaiIncomingTriggerCounterSet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_CPSS_DXCH_PTP_TAI_ID_STC_PTR_taiIdPtr,
    &IN_GT_U32_value
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChPtpTaiClockModeGet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_CPSS_DXCH_PTP_TAI_ID_STC_PTR_taiIdPtr,
    &OUT_CPSS_DXCH_PTP_TAI_CLOCK_MODE_ENT_PTR_clockModePtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChPtpTaiTodStepGet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_CPSS_DXCH_PTP_TAI_ID_STC_PTR_taiIdPtr,
    &OUT_CPSS_DXCH_PTP_TAI_TOD_STEP_STC_PTR_todStepPtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChPtpTaiFractionalNanosecondDriftGet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_CPSS_DXCH_PTP_TAI_ID_STC_PTR_taiIdPtr,
    &OUT_GT_32_PTR_fracNanoSecondPtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChPtpTaiInternalClockGenerateEnableGet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_CPSS_DXCH_PTP_TAI_ID_STC_PTR_taiIdPtr,
    &OUT_GT_BOOL_PTR_enablePtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChPtpTaiIncomingClockCounterGet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_CPSS_DXCH_PTP_TAI_ID_STC_PTR_taiIdPtr,
    &OUT_GT_BOOL_PTR_enablePtr,
    &OUT_GT_U32_PTR_valuePtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChPtpTaiExternalPulseWidthGet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_CPSS_DXCH_PTP_TAI_ID_STC_PTR_taiIdPtr,
    &OUT_GT_U32_PTR_extPulseWidthPtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChPtpTaiGracefulStepGet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_CPSS_DXCH_PTP_TAI_ID_STC_PTR_taiIdPtr,
    &OUT_GT_U32_PTR_gracefulStepPtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChPtpTaiFrequencyDriftThesholdsGet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_CPSS_DXCH_PTP_TAI_ID_STC_PTR_taiIdPtr,
    &OUT_GT_U32_PTR_interruptThresholdPtr,
    &OUT_GT_U32_PTR_adjustThresholdPtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChPtpTaiPulseDelayGet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_CPSS_DXCH_PTP_TAI_ID_STC_PTR_taiIdPtr,
    &OUT_GT_U32_PTR_nanoSecondsPtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChPtpTaiClockCycleGet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_CPSS_DXCH_PTP_TAI_ID_STC_PTR_taiIdPtr,
    &OUT_GT_U32_PTR_secondsPtr,
    &OUT_GT_U32_PTR_nanoSecondsPtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChPtpTaiTodUpdateCounterGet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_CPSS_DXCH_PTP_TAI_ID_STC_PTR_taiIdPtr,
    &OUT_GT_U32_PTR_valuePtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChPtpTaiPtpClockInterfaceSet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_CPSS_DXCH_PTP_TAI_NUMBER_ENT_taiNumber,
    &IN_CPSS_DIRECTION_ENT_clockInterfaceDirection
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChPtpTaiInputClockSelectSet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_CPSS_DXCH_PTP_TAI_NUMBER_ENT_taiNumber,
    &IN_CPSS_DXCH_PTP_TAI_CLOCK_SELECT_ENT_clockSelect
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChPtpTaiPtpClockInterfaceGet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_CPSS_DXCH_PTP_TAI_NUMBER_ENT_taiNumber,
    &OUT_CPSS_DIRECTION_ENT_PTR_clockInterfaceDirectionPtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChPtpTaiInputClockSelectGet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_CPSS_DXCH_PTP_TAI_NUMBER_ENT_taiNumber,
    &OUT_CPSS_DXCH_PTP_TAI_CLOCK_SELECT_ENT_PTR_clockSelectPtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChPtpTaiPClkOutputInterfaceSet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_CPSS_DXCH_PTP_TAI_PCLK_RCVR_CLK_MODE_ENT_pclkRcvrClkMode,
    &IN_CPSS_DXCH_PTP_TAI_NUMBER_ENT_taiNumber
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChPtpTsTagGlobalCfgSet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_CPSS_DXCH_PTP_TS_TAG_GLOBAL_CFG_STC_PTR_tsTagGlobalCfgPtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChPtpTsUdpChecksumUpdateModeSet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_CPSS_DXCH_PTP_TS_UDP_CHECKSUM_UPDATE_MODE_STC_PTR_udpCsUpdModePtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChPtpTodCounterFunctionTriggerSet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_CPSS_PORT_DIRECTION_ENT_direction,
    &IN_CPSS_DXCH_PTP_TAI_ID_STC_PTR_taiIdPtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChPtpTodCounterFunctionSet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_CPSS_PORT_DIRECTION_ENT_direction,
    &IN_CPSS_DXCH_PTP_TAI_ID_STC_PTR_taiIdPtr,
    &IN_CPSS_DXCH_PTP_TOD_COUNTER_FUNC_ENT_function
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChPtpTodCounterFunctionGet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_CPSS_PORT_DIRECTION_ENT_direction,
    &IN_CPSS_DXCH_PTP_TAI_ID_STC_PTR_taiIdPtr,
    &OUT_CPSS_DXCH_PTP_TOD_COUNTER_FUNC_ENT_PTR_functionPtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChPtpTodCounterFunctionTriggerGet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_CPSS_PORT_DIRECTION_ENT_direction,
    &IN_CPSS_DXCH_PTP_TAI_ID_STC_PTR_taiIdPtr,
    &OUT_GT_BOOL_PTR_enablePtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChPtpTodCounterShadowSet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_CPSS_PORT_DIRECTION_ENT_direction,
    &IN_CPSS_DXCH_PTP_TOD_COUNT_STC_PTR_todCounterPtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChPtpUdpTimeStampEnableSet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_CPSS_PORT_DIRECTION_ENT_direction,
    &IN_CPSS_IP_PROTOCOL_STACK_ENT_protocolStack,
    &IN_GT_BOOL_enable
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChPtpUdpTimeStampEnableGet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_CPSS_PORT_DIRECTION_ENT_direction,
    &IN_CPSS_IP_PROTOCOL_STACK_ENT_protocolStack,
    &OUT_GT_BOOL_PTR_enablePtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChPtpEthernetTimeStampEnableSet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_CPSS_PORT_DIRECTION_ENT_direction,
    &IN_GT_BOOL_enable
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChPtpPortTimeStampEnableSet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_CPSS_PORT_DIRECTION_ENT_direction,
    &IN_GT_PHYSICAL_PORT_NUM_portNum,
    &IN_GT_BOOL_enable
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChPtpPortTimeStampEnableGet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_CPSS_PORT_DIRECTION_ENT_direction,
    &IN_GT_PHYSICAL_PORT_NUM_portNum,
    &OUT_GT_BOOL_PTR_enablePtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChPtpMessageTypeTimeStampEnableSet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_CPSS_PORT_DIRECTION_ENT_direction,
    &IN_GT_U32_messageType,
    &IN_GT_BOOL_enable
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChPtpMessageTypeTimeStampEnableGet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_CPSS_PORT_DIRECTION_ENT_direction,
    &IN_GT_U32_messageType,
    &OUT_GT_BOOL_PTR_enablePtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChPtpTransportSpecificTimeStampEnableSet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_CPSS_PORT_DIRECTION_ENT_direction,
    &IN_GT_U32_transportSpecificVal,
    &IN_GT_BOOL_enable
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChPtpTransportSpecificTimeStampEnableGet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_CPSS_PORT_DIRECTION_ENT_direction,
    &IN_GT_U32_transportSpecificVal,
    &OUT_GT_BOOL_PTR_enablePtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChPtpTimestampEntryGet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_CPSS_PORT_DIRECTION_ENT_direction,
    &OUT_CPSS_DXCH_PTP_TIMESTAMP_ENTRY_STC_PTR_entryPtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChPtpTodCounterShadowGet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_CPSS_PORT_DIRECTION_ENT_direction,
    &OUT_CPSS_DXCH_PTP_TOD_COUNT_STC_PTR_todCounterPtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChPtpEthernetTimeStampEnableGet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_CPSS_PORT_DIRECTION_ENT_direction,
    &OUT_GT_BOOL_PTR_enablePtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChPtpTsuControlSet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_PHYSICAL_PORT_NUM_portNum,
    &IN_CPSS_DXCH_PTP_TSU_CONTROL_STC_PTR_controlPtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChPtpTsuPacketCouterGet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_PHYSICAL_PORT_NUM_portNum,
    &IN_CPSS_DXCH_PTP_TSU_PACKET_COUNTER_TYPE_ENT_counterType,
    &OUT_GT_U32_PTR_valuePtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChPtpTsDelayEgressTimeCorrTaiSelectModeSet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_PHYSICAL_PORT_NUM_portNum,
    &IN_CPSS_DXCH_PTP_TS_DELAY_TAI_SEL_MODE_ENT_egrTimeCorrTaiSelMode
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChPtpTsTagPortCfgSet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_PHYSICAL_PORT_NUM_portNum,
    &IN_CPSS_DXCH_PTP_TS_TAG_PORT_CFG_STC_PTR_tsTagPortCfgPtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChPtpTsDelayEgressPipeDelaySet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_PHYSICAL_PORT_NUM_portNum,
    &IN_GT_32_egrPipeDelayCorr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChPtpTsDelayIngressPortDelaySet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_PHYSICAL_PORT_NUM_portNum,
    &IN_GT_32_ingressDelayCorr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChPtpTsDelayEgressCorrFieldPiggybackEnableSet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_PHYSICAL_PORT_NUM_portNum,
    &IN_GT_BOOL_egrCorrFldPBEnable
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChPtpTsDelayIngressCorrFieldPiggybackEnableSet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_PHYSICAL_PORT_NUM_portNum,
    &IN_GT_BOOL_ingrCorrFldPBEnable
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChPtpPortUnitResetSet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_PHYSICAL_PORT_NUM_portNum,
    &IN_GT_BOOL_resetTxUnit,
    &IN_GT_BOOL_resetRxUnit
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChPtpPortTxPipeStatusDelaySet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_PHYSICAL_PORT_NUM_portNum,
    &IN_GT_U32_delay
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChPtpEgressDomainTableSet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_PHYSICAL_PORT_NUM_portNum,
    &IN_GT_U32_domainIndex,
    &IN_CPSS_DXCH_PTP_EGRESS_DOMAIN_ENTRY_STC_PTR_entryPtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChPtpMessageTypeCmdSet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_PHYSICAL_PORT_NUM_portNum,
    &IN_GT_U32_domainIndex,
    &IN_GT_U32_messageType,
    &IN_CPSS_PACKET_CMD_ENT_command
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChPtpMessageTypeCmdGet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_PHYSICAL_PORT_NUM_portNum,
    &IN_GT_U32_domainIndex,
    &IN_GT_U32_messageType,
    &OUT_CPSS_PACKET_CMD_ENT_PTR_commandPtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChPtpEgressDomainTableGet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_PHYSICAL_PORT_NUM_portNum,
    &IN_GT_U32_domainIndex,
    &OUT_CPSS_DXCH_PTP_EGRESS_DOMAIN_ENTRY_STC_PTR_entryPtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChPtpTsDelayEgressAsymmetryCorrectionSet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_PHYSICAL_PORT_NUM_portNum,
    &IN_GT_U32_domainProfile,
    &IN_GT_32_egrAsymmetryCorr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChPtpTsDelayIngressLinkDelaySet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_PHYSICAL_PORT_NUM_portNum,
    &IN_GT_U32_domainProfile,
    &IN_GT_32_ingressDelay
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChPtpTsLocalActionTableSet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_PHYSICAL_PORT_NUM_portNum,
    &IN_GT_U32_domainProfile,
    &IN_GT_U32_messageType,
    &IN_CPSS_DXCH_PTP_TS_LOCAL_ACTION_ENTRY_STC_PTR_entryPtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChPtpTsLocalActionTableGet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_PHYSICAL_PORT_NUM_portNum,
    &IN_GT_U32_domainProfile,
    &IN_GT_U32_messageType,
    &OUT_CPSS_DXCH_PTP_TS_LOCAL_ACTION_ENTRY_STC_PTR_entryPtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChPtpTsDelayEgressAsymmetryCorrectionGet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_PHYSICAL_PORT_NUM_portNum,
    &IN_GT_U32_domainProfile,
    &OUT_GT_32_PTR_egrAsymmetryCorrPtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChPtpTsDelayIngressLinkDelayGet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_PHYSICAL_PORT_NUM_portNum,
    &IN_GT_U32_domainProfile,
    &OUT_GT_32_PTR_ingressDelayPtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChPtpTsDelayEgressTimestampTaiSelectSet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_PHYSICAL_PORT_NUM_portNum,
    &IN_GT_U32_egrTsTaiNum
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChPtpTsuNtpTimeOffsetSet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_PHYSICAL_PORT_NUM_portNum,
    &IN_GT_U32_ntpTimeOffset
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChPtpTsuTxTimestampQueueRead_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_PHYSICAL_PORT_NUM_portNum,
    &IN_GT_U32_queueNum,
    &OUT_CPSS_DXCH_PTP_TSU_TX_TIMESTAMP_QUEUE_ENTRY_STC_PTR_entryPtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChPtpTsuControlGet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_PHYSICAL_PORT_NUM_portNum,
    &OUT_CPSS_DXCH_PTP_TSU_CONTROL_STC_PTR_controlPtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChPtpTsDelayEgressTimeCorrTaiSelectModeGet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_PHYSICAL_PORT_NUM_portNum,
    &OUT_CPSS_DXCH_PTP_TS_DELAY_TAI_SEL_MODE_ENT_PTR_egrTimeCorrTaiSelModePtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChPtpTsTagPortCfgGet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_PHYSICAL_PORT_NUM_portNum,
    &OUT_CPSS_DXCH_PTP_TS_TAG_PORT_CFG_STC_PTR_tsTagPortCfgPtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChPtpTsDelayEgressPipeDelayGet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_PHYSICAL_PORT_NUM_portNum,
    &OUT_GT_32_PTR_egrPipeDelayCorrPtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChPtpTsDelayIngressPortDelayGet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_PHYSICAL_PORT_NUM_portNum,
    &OUT_GT_32_PTR_ingressDelayCorrPtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChPtpTsDelayEgressCorrFieldPiggybackEnableGet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_PHYSICAL_PORT_NUM_portNum,
    &OUT_GT_BOOL_PTR_egrCorrFldPBEnablePtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChPtpTsDelayIngressCorrFieldPiggybackEnableGet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_PHYSICAL_PORT_NUM_portNum,
    &OUT_GT_BOOL_PTR_ingrCorrFldPBEnablePtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChPtpPortUnitResetGet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_PHYSICAL_PORT_NUM_portNum,
    &OUT_GT_BOOL_PTR_resetTxUnitPtr,
    &OUT_GT_BOOL_PTR_resetRxUnitPtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChPtpPortTxPipeStatusDelayGet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_PHYSICAL_PORT_NUM_portNum,
    &OUT_GT_U32_PTR_delayPtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChPtpTsDelayEgressTimestampTaiSelectGet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_PHYSICAL_PORT_NUM_portNum,
    &OUT_GT_U32_PTR_egrTsTaiNumPtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChPtpTsuNtpTimeOffsetGet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_PHYSICAL_PORT_NUM_portNum,
    &OUT_GT_U32_PTR_ntpTimeOffsetPtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChPtpPortGroupTodCounterFunctionTriggerSet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_PORT_GROUPS_BMP_portGroupsBmp,
    &IN_CPSS_PORT_DIRECTION_ENT_direction
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChPtpPortGroupTodCounterFunctionSet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_PORT_GROUPS_BMP_portGroupsBmp,
    &IN_CPSS_PORT_DIRECTION_ENT_direction,
    &IN_CPSS_DXCH_PTP_TOD_COUNTER_FUNC_ENT_function
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChPtpPortGroupTodCounterShadowSet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_PORT_GROUPS_BMP_portGroupsBmp,
    &IN_CPSS_PORT_DIRECTION_ENT_direction,
    &IN_CPSS_DXCH_PTP_TOD_COUNT_STC_PTR_todCounterPtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChPtpPortGroupTodCounterFunctionGet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_PORT_GROUPS_BMP_portGroupsBmp,
    &IN_CPSS_PORT_DIRECTION_ENT_direction,
    &OUT_CPSS_DXCH_PTP_TOD_COUNTER_FUNC_ENT_PTR_functionPtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChPtpPortGroupTodCounterShadowGet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_PORT_GROUPS_BMP_portGroupsBmp,
    &IN_CPSS_PORT_DIRECTION_ENT_direction,
    &OUT_CPSS_DXCH_PTP_TOD_COUNT_STC_PTR_todCounterPtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChPtpPortGroupTodCounterFunctionTriggerGet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_PORT_GROUPS_BMP_portGroupsBmp,
    &IN_CPSS_PORT_DIRECTION_ENT_direction,
    &OUT_GT_BOOL_PTR_enablePtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChPtpDomainModeSet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_U32_domainIndex,
    &IN_CPSS_DXCH_PTP_INGRESS_DOMAIN_MODE_ENT_domainMode
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChPtpDomainV2IdSet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_U32_domainIndex,
    &IN_GT_U32_domainId
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChPtpIngressPacketCheckingModeSet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_U32_domainIndex,
    &IN_GT_U32_messageType,
    &IN_CPSS_DXCH_PTP_INGRESS_CHECKING_MODE_ENT_checkingMode
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChPtpIngressPacketCheckingModeGet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_U32_domainIndex,
    &IN_GT_U32_messageType,
    &OUT_CPSS_DXCH_PTP_INGRESS_CHECKING_MODE_ENT_PTR_checkingModePtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChPtpDomainModeGet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_U32_domainIndex,
    &OUT_CPSS_DXCH_PTP_INGRESS_DOMAIN_MODE_ENT_PTR_domainModePtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChPtpDomainV2IdGet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_U32_domainIndex,
    &OUT_GT_U32_PTR_domainIdPtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChPtpTsCfgTableSet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_U32_entryIndex,
    &IN_CPSS_DXCH_PTP_TS_CFG_ENTRY_STC_PTR_entryPtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChPtpTsCfgTableGet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_U32_entryIndex,
    &OUT_CPSS_DXCH_PTP_TS_CFG_ENTRY_STC_PTR_entryPtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChPtpEtherTypeSet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_U32_etherTypeIndex,
    &IN_GT_U32_etherType
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChPtpEtherTypeGet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_U32_etherTypeIndex,
    &OUT_GT_U32_PTR_etherTypePtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChPtpTsMessageTypeToQueueIdMapSet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_U32_idMapBmp
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChPtpTsNtpTimeOffsetSet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_U32_ntpTimeOffset
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChPtpTsEgressTimestampQueueEntryRead_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_U32_queueNum,
    &OUT_CPSS_DXCH_PTP_TS_EGRESS_TIMESTAMP_QUEUE_ENTRY_STC_PTR_tsQueueEntryPtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChPtpTsIngressTimestampQueueEntryRead_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_U32_queueNum,
    &OUT_CPSS_DXCH_PTP_TS_INGRESS_TIMESTAMP_QUEUE_ENTRY_STC_PTR_tsQueueEntryPtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChPtpTsQueuesSizeSet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_U32_queueSize
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChPtpUdpDestPortsSet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_U32_udpPortIndex,
    &IN_GT_U32_udpPortNum
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChPtpUdpDestPortsGet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_U32_udpPortIndex,
    &OUT_GT_U32_PTR_udpPortNumPtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChPtpTaiPtpPulseIterfaceGet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &OUT_CPSS_DIRECTION_ENT_PTR_pulseInterfaceDirectionPtr,
    &OUT_CPSS_DXCH_PTP_TAI_NUMBER_ENT_PTR_taiNumberPtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChPtpEgressExceptionCfgGet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &OUT_CPSS_DXCH_PTP_EGRESS_EXCEPTION_CFG_STC_PTR_egrExceptionCfgPtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChPtpEgressExceptionCountersGet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &OUT_CPSS_DXCH_PTP_EGRESS_EXCEPTION_COUNTERS_STC_PTR_egrExceptionCntPtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChPtpIngressExceptionCfgGet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &OUT_CPSS_DXCH_PTP_INGRESS_EXCEPTION_CFG_STC_PTR_ingrExceptionCfgPtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChPtpTaiPClkOutputInterfaceGet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &OUT_CPSS_DXCH_PTP_TAI_PCLK_RCVR_CLK_MODE_ENT_PTR_pclkRcvrClkModePtr,
    &OUT_CPSS_DXCH_PTP_TAI_NUMBER_ENT_PTR_taiNumberPtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChPtpTsTagGlobalCfgGet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &OUT_CPSS_DXCH_PTP_TS_TAG_GLOBAL_CFG_STC_PTR_tsTagGlobalCfgPtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChPtpTsUdpChecksumUpdateModeGet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &OUT_CPSS_DXCH_PTP_TS_UDP_CHECKSUM_UPDATE_MODE_STC_PTR_udpCsUpdModePtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChPtpTsMessageTypeToQueueIdMapGet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &OUT_GT_U32_PTR_idMapBmpPtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChPtpTsNtpTimeOffsetGet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &OUT_GT_U32_PTR_ntpTimeOffsetPtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChPtpTsDebugQueuesEntryIdsGet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &OUT_GT_U32_PTR_queueEntryId0Ptr,
    &OUT_GT_U32_PTR_queueEntryId1Ptr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChPtpTsQueuesSizeGet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &OUT_GT_U32_PTR_queueSizePtr
};
extern PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChPtpDomainV1IdGet_PARAMS[];
extern PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChPtpDomainV1IdSet_PARAMS[];


/********* lib API DB *********/

extern void cpssDxChPtpDomainV1IdSet_preLogic(IN va_list  args, OUT PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC * paramDataPtr);
extern void cpssDxChPtpDomainV1IdSet_preLogic(IN va_list  args, OUT PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC * paramDataPtr);

static PRV_CPSS_LOG_FUNC_ENTRY_STC prvCpssDxChPtpLogLibDb[] = {
    {"cpssDxChPtpEtherTypeSet", 3, cpssDxChPtpEtherTypeSet_PARAMS, NULL},
    {"cpssDxChPtpEtherTypeGet", 3, cpssDxChPtpEtherTypeGet_PARAMS, NULL},
    {"cpssDxChPtpUdpDestPortsSet", 3, cpssDxChPtpUdpDestPortsSet_PARAMS, NULL},
    {"cpssDxChPtpUdpDestPortsGet", 3, cpssDxChPtpUdpDestPortsGet_PARAMS, NULL},
    {"cpssDxChPtpMessageTypeCmdSet", 5, cpssDxChPtpMessageTypeCmdSet_PARAMS, NULL},
    {"cpssDxChPtpMessageTypeCmdGet", 5, cpssDxChPtpMessageTypeCmdGet_PARAMS, NULL},
    {"cpssDxChPtpCpuCodeBaseSet", 2, prvCpssLogGenDevNumCpuCode_PARAMS, NULL},
    {"cpssDxChPtpCpuCodeBaseGet", 2, prvCpssLogGenDevNumCpuCodePtr_PARAMS, NULL},
    {"cpssDxChPtpTodCounterFunctionSet", 4, cpssDxChPtpTodCounterFunctionSet_PARAMS, NULL},
    {"cpssDxChPtpTodCounterFunctionGet", 4, cpssDxChPtpTodCounterFunctionGet_PARAMS, NULL},
    {"cpssDxChPtpTodCounterFunctionTriggerSet", 3, cpssDxChPtpTodCounterFunctionTriggerSet_PARAMS, NULL},
    {"cpssDxChPtpTodCounterFunctionTriggerGet", 4, cpssDxChPtpTodCounterFunctionTriggerGet_PARAMS, NULL},
    {"cpssDxChPtpTodCounterShadowSet", 3, cpssDxChPtpTodCounterShadowSet_PARAMS, NULL},
    {"cpssDxChPtpTodCounterShadowGet", 3, cpssDxChPtpTodCounterShadowGet_PARAMS, NULL},
    {"cpssDxChPtpTodCounterGet", 3, cpssDxChPtpTodCounterShadowGet_PARAMS, NULL},
    {"cpssDxChPtpPortTimeStampEnableSet", 4, cpssDxChPtpPortTimeStampEnableSet_PARAMS, NULL},
    {"cpssDxChPtpPortTimeStampEnableGet", 4, cpssDxChPtpPortTimeStampEnableGet_PARAMS, NULL},
    {"cpssDxChPtpEthernetTimeStampEnableSet", 3, cpssDxChPtpEthernetTimeStampEnableSet_PARAMS, NULL},
    {"cpssDxChPtpEthernetTimeStampEnableGet", 3, cpssDxChPtpEthernetTimeStampEnableGet_PARAMS, NULL},
    {"cpssDxChPtpUdpTimeStampEnableSet", 4, cpssDxChPtpUdpTimeStampEnableSet_PARAMS, NULL},
    {"cpssDxChPtpUdpTimeStampEnableGet", 4, cpssDxChPtpUdpTimeStampEnableGet_PARAMS, NULL},
    {"cpssDxChPtpMessageTypeTimeStampEnableSet", 4, cpssDxChPtpMessageTypeTimeStampEnableSet_PARAMS, NULL},
    {"cpssDxChPtpMessageTypeTimeStampEnableGet", 4, cpssDxChPtpMessageTypeTimeStampEnableGet_PARAMS, NULL},
    {"cpssDxChPtpTransportSpecificCheckEnableSet", 3, cpssDxChPtpEthernetTimeStampEnableSet_PARAMS, NULL},
    {"cpssDxChPtpTransportSpecificCheckEnableGet", 3, cpssDxChPtpEthernetTimeStampEnableGet_PARAMS, NULL},
    {"cpssDxChPtpTransportSpecificTimeStampEnableSet", 4, cpssDxChPtpTransportSpecificTimeStampEnableSet_PARAMS, NULL},
    {"cpssDxChPtpTransportSpecificTimeStampEnableGet", 4, cpssDxChPtpTransportSpecificTimeStampEnableGet_PARAMS, NULL},
    {"cpssDxChPtpTimestampEntryGet", 3, cpssDxChPtpTimestampEntryGet_PARAMS, NULL},
    {"cpssDxChPtpPortGroupTodCounterFunctionSet", 4, cpssDxChPtpPortGroupTodCounterFunctionSet_PARAMS, NULL},
    {"cpssDxChPtpPortGroupTodCounterFunctionGet", 4, cpssDxChPtpPortGroupTodCounterFunctionGet_PARAMS, NULL},
    {"cpssDxChPtpPortGroupTodCounterFunctionTriggerSet", 3, cpssDxChPtpPortGroupTodCounterFunctionTriggerSet_PARAMS, NULL},
    {"cpssDxChPtpPortGroupTodCounterFunctionTriggerGet", 4, cpssDxChPtpPortGroupTodCounterFunctionTriggerGet_PARAMS, NULL},
    {"cpssDxChPtpPortGroupTodCounterShadowSet", 4, cpssDxChPtpPortGroupTodCounterShadowSet_PARAMS, NULL},
    {"cpssDxChPtpPortGroupTodCounterShadowGet", 4, cpssDxChPtpPortGroupTodCounterShadowGet_PARAMS, NULL},
    {"cpssDxChPtpPortGroupTodCounterGet", 4, cpssDxChPtpPortGroupTodCounterShadowGet_PARAMS, NULL},
    {"cpssDxChPtpTaiClockModeSet", 3, cpssDxChPtpTaiClockModeSet_PARAMS, NULL},
    {"cpssDxChPtpTaiClockModeGet", 3, cpssDxChPtpTaiClockModeGet_PARAMS, NULL},
    {"cpssDxChPtpTaiInternalClockGenerateEnableSet", 3, cpssDxChPtpTaiInternalClockGenerateEnableSet_PARAMS, NULL},
    {"cpssDxChPtpTaiInternalClockGenerateEnableGet", 3, cpssDxChPtpTaiInternalClockGenerateEnableGet_PARAMS, NULL},
    {"cpssDxChPtpTaiPtpPClockDriftAdjustEnableSet", 3, cpssDxChPtpTaiInternalClockGenerateEnableSet_PARAMS, NULL},
    {"cpssDxChPtpTaiPtpPClockDriftAdjustEnableGet", 3, cpssDxChPtpTaiInternalClockGenerateEnableGet_PARAMS, NULL},
    {"cpssDxChPtpTaiCaptureOverrideEnableSet", 3, cpssDxChPtpTaiInternalClockGenerateEnableSet_PARAMS, NULL},
    {"cpssDxChPtpTaiCaptureOverrideEnableGet", 3, cpssDxChPtpTaiInternalClockGenerateEnableGet_PARAMS, NULL},
    {"cpssDxChPtpTaiInputTriggersCountEnableSet", 3, cpssDxChPtpTaiInternalClockGenerateEnableSet_PARAMS, NULL},
    {"cpssDxChPtpTaiInputTriggersCountEnableGet", 3, cpssDxChPtpTaiInternalClockGenerateEnableGet_PARAMS, NULL},
    {"cpssDxChPtpTaiExternalPulseWidthSet", 3, cpssDxChPtpTaiExternalPulseWidthSet_PARAMS, NULL},
    {"cpssDxChPtpTaiExternalPulseWidthGet", 3, cpssDxChPtpTaiExternalPulseWidthGet_PARAMS, NULL},
    {"cpssDxChPtpTaiTodSet", 4, cpssDxChPtpTaiTodSet_PARAMS, NULL},
    {"cpssDxChPtpTaiTodGet", 4, cpssDxChPtpTaiTodGet_PARAMS, NULL},
    {"cpssDxChPtpTaiOutputTriggerEnableSet", 3, cpssDxChPtpTaiInternalClockGenerateEnableSet_PARAMS, NULL},
    {"cpssDxChPtpTaiOutputTriggerEnableGet", 3, cpssDxChPtpTaiInternalClockGenerateEnableGet_PARAMS, NULL},
    {"cpssDxChPtpTaiTodStepSet", 3, cpssDxChPtpTaiTodStepSet_PARAMS, NULL},
    {"cpssDxChPtpTaiTodStepGet", 3, cpssDxChPtpTaiTodStepGet_PARAMS, NULL},
    {"cpssDxChPtpTaiPulseDelaySet", 3, cpssDxChPtpTaiPulseDelaySet_PARAMS, NULL},
    {"cpssDxChPtpTaiPulseDelayGet", 3, cpssDxChPtpTaiPulseDelayGet_PARAMS, NULL},
    {"cpssDxChPtpTaiClockDelaySet", 3, cpssDxChPtpTaiPulseDelaySet_PARAMS, NULL},
    {"cpssDxChPtpTaiClockDelayGet", 3, cpssDxChPtpTaiPulseDelayGet_PARAMS, NULL},
    {"cpssDxChPtpTaiFractionalNanosecondDriftSet", 3, cpssDxChPtpTaiFractionalNanosecondDriftSet_PARAMS, NULL},
    {"cpssDxChPtpTaiFractionalNanosecondDriftGet", 3, cpssDxChPtpTaiFractionalNanosecondDriftGet_PARAMS, NULL},
    {"cpssDxChPtpTaiPClockCycleSet", 3, cpssDxChPtpTaiPulseDelaySet_PARAMS, NULL},
    {"cpssDxChPtpTaiPClockCycleGet", 3, cpssDxChPtpTaiPulseDelayGet_PARAMS, NULL},
    {"cpssDxChPtpTaiClockCycleSet", 4, cpssDxChPtpTaiClockCycleSet_PARAMS, NULL},
    {"cpssDxChPtpTaiClockCycleGet", 4, cpssDxChPtpTaiClockCycleGet_PARAMS, NULL},
    {"cpssDxChPtpTaiTodCaptureStatusSet", 4, cpssDxChPtpTaiTodCaptureStatusSet_PARAMS, NULL},
    {"cpssDxChPtpTaiTodCaptureStatusGet", 4, cpssDxChPtpTaiTodCaptureStatusGet_PARAMS, NULL},
    {"cpssDxChPtpTaiTodUpdateCounterGet", 3, cpssDxChPtpTaiTodUpdateCounterGet_PARAMS, NULL},
    {"cpssDxChPtpTaiIncomingTriggerCounterSet", 3, cpssDxChPtpTaiIncomingTriggerCounterSet_PARAMS, NULL},
    {"cpssDxChPtpTaiIncomingTriggerCounterGet", 3, cpssDxChPtpTaiTodUpdateCounterGet_PARAMS, NULL},
    {"cpssDxChPtpTaiIncomingClockCounterSet", 4, cpssDxChPtpTaiIncomingClockCounterSet_PARAMS, NULL},
    {"cpssDxChPtpTaiIncomingClockCounterGet", 4, cpssDxChPtpTaiIncomingClockCounterGet_PARAMS, NULL},
    {"cpssDxChPtpTaiFrequencyDriftThesholdsSet", 4, cpssDxChPtpTaiFrequencyDriftThesholdsSet_PARAMS, NULL},
    {"cpssDxChPtpTaiFrequencyDriftThesholdsGet", 4, cpssDxChPtpTaiFrequencyDriftThesholdsGet_PARAMS, NULL},
    {"cpssDxChPtpTaiGracefulStepSet", 3, cpssDxChPtpTaiGracefulStepSet_PARAMS, NULL},
    {"cpssDxChPtpTaiGracefulStepGet", 3, cpssDxChPtpTaiGracefulStepGet_PARAMS, NULL},
    {"cpssDxChPtpTsuControlSet", 3, cpssDxChPtpTsuControlSet_PARAMS, NULL},
    {"cpssDxChPtpTsuControlGet", 3, cpssDxChPtpTsuControlGet_PARAMS, NULL},
    {"cpssDxChPtpTsuTxTimestampQueueRead", 4, cpssDxChPtpTsuTxTimestampQueueRead_PARAMS, NULL},
    {"cpssDxChPtpTsuCountersClear", 2, prvCpssLogGenDevNumPortNum_PARAMS, NULL},
    {"cpssDxChPtpTsuPacketCouterGet", 4, cpssDxChPtpTsuPacketCouterGet_PARAMS, NULL},
    {"cpssDxChPtpTsuNtpTimeOffsetSet", 3, cpssDxChPtpTsuNtpTimeOffsetSet_PARAMS, NULL},
    {"cpssDxChPtpTsuNtpTimeOffsetGet", 3, cpssDxChPtpTsuNtpTimeOffsetGet_PARAMS, NULL},
    {"cpssDxChPtpTsTagGlobalCfgSet", 2, cpssDxChPtpTsTagGlobalCfgSet_PARAMS, NULL},
    {"cpssDxChPtpTsTagGlobalCfgGet", 2, cpssDxChPtpTsTagGlobalCfgGet_PARAMS, NULL},
    {"cpssDxChPtpTsTagPortCfgSet", 3, cpssDxChPtpTsTagPortCfgSet_PARAMS, NULL},
    {"cpssDxChPtpTsTagPortCfgGet", 3, cpssDxChPtpTsTagPortCfgGet_PARAMS, NULL},
    {"cpssDxChPtpTsDelayIngressPortDelaySet", 3, cpssDxChPtpTsDelayIngressPortDelaySet_PARAMS, NULL},
    {"cpssDxChPtpTsDelayIngressPortDelayGet", 3, cpssDxChPtpTsDelayIngressPortDelayGet_PARAMS, NULL},
    {"cpssDxChPtpTsDelayIngressLinkDelaySet", 4, cpssDxChPtpTsDelayIngressLinkDelaySet_PARAMS, NULL},
    {"cpssDxChPtpTsDelayIngressLinkDelayGet", 4, cpssDxChPtpTsDelayIngressLinkDelayGet_PARAMS, NULL},
    {"cpssDxChPtpTsDelayEgressPipeDelaySet", 3, cpssDxChPtpTsDelayEgressPipeDelaySet_PARAMS, NULL},
    {"cpssDxChPtpTsDelayEgressPipeDelayGet", 3, cpssDxChPtpTsDelayEgressPipeDelayGet_PARAMS, NULL},
    {"cpssDxChPtpTsDelayEgressAsymmetryCorrectionSet", 4, cpssDxChPtpTsDelayEgressAsymmetryCorrectionSet_PARAMS, NULL},
    {"cpssDxChPtpTsDelayEgressAsymmetryCorrectionGet", 4, cpssDxChPtpTsDelayEgressAsymmetryCorrectionGet_PARAMS, NULL},
    {"cpssDxChPtpTsDelayIngressCorrFieldPiggybackEnableSet", 3, cpssDxChPtpTsDelayIngressCorrFieldPiggybackEnableSet_PARAMS, NULL},
    {"cpssDxChPtpTsDelayIngressCorrFieldPiggybackEnableGet", 3, cpssDxChPtpTsDelayIngressCorrFieldPiggybackEnableGet_PARAMS, NULL},
    {"cpssDxChPtpTsDelayEgressCorrFieldPiggybackEnableSet", 3, cpssDxChPtpTsDelayEgressCorrFieldPiggybackEnableSet_PARAMS, NULL},
    {"cpssDxChPtpTsDelayEgressCorrFieldPiggybackEnableGet", 3, cpssDxChPtpTsDelayEgressCorrFieldPiggybackEnableGet_PARAMS, NULL},
    {"cpssDxChPtpTsDelayEgressTimeCorrTaiSelectModeSet", 3, cpssDxChPtpTsDelayEgressTimeCorrTaiSelectModeSet_PARAMS, NULL},
    {"cpssDxChPtpTsDelayEgressTimeCorrTaiSelectModeGet", 3, cpssDxChPtpTsDelayEgressTimeCorrTaiSelectModeGet_PARAMS, NULL},
    {"cpssDxChPtpTsDelayEgressTimestampTaiSelectSet", 3, cpssDxChPtpTsDelayEgressTimestampTaiSelectSet_PARAMS, NULL},
    {"cpssDxChPtpTsDelayEgressTimestampTaiSelectGet", 3, cpssDxChPtpTsDelayEgressTimestampTaiSelectGet_PARAMS, NULL},
    {"cpssDxChPtpTsCfgTableSet", 3, cpssDxChPtpTsCfgTableSet_PARAMS, NULL},
    {"cpssDxChPtpTsCfgTableGet", 3, cpssDxChPtpTsCfgTableGet_PARAMS, NULL},
    {"cpssDxChPtpTsLocalActionTableSet", 5, cpssDxChPtpTsLocalActionTableSet_PARAMS, NULL},
    {"cpssDxChPtpTsLocalActionTableGet", 5, cpssDxChPtpTsLocalActionTableGet_PARAMS, NULL},
    {"cpssDxChPtpTsUdpChecksumUpdateModeSet", 2, cpssDxChPtpTsUdpChecksumUpdateModeSet_PARAMS, NULL},
    {"cpssDxChPtpTsUdpChecksumUpdateModeGet", 2, cpssDxChPtpTsUdpChecksumUpdateModeGet_PARAMS, NULL},
    {"cpssDxChPtpOverEthernetEnableSet", 2, prvCpssLogGenDevNumEnable_PARAMS, NULL},
    {"cpssDxChPtpOverEthernetEnableGet", 2, prvCpssLogGenDevNumEnablePtr_PARAMS, NULL},
    {"cpssDxChPtpOverUdpEnableSet", 2, prvCpssLogGenDevNumEnable_PARAMS, NULL},
    {"cpssDxChPtpOverUdpEnableGet", 2, prvCpssLogGenDevNumEnablePtr_PARAMS, NULL},
    {"cpssDxChPtpDomainModeSet", 3, cpssDxChPtpDomainModeSet_PARAMS, NULL},
    {"cpssDxChPtpDomainModeGet", 3, cpssDxChPtpDomainModeGet_PARAMS, NULL},
    {"cpssDxChPtpDomainV1IdSet", 3, cpssDxChPtpDomainV1IdSet_PARAMS, cpssDxChPtpDomainV1IdSet_preLogic},
    {"cpssDxChPtpDomainV1IdGet", 3, cpssDxChPtpDomainV1IdGet_PARAMS, cpssDxChPtpDomainV1IdSet_preLogic},
    {"cpssDxChPtpDomainV2IdSet", 3, cpssDxChPtpDomainV2IdSet_PARAMS, NULL},
    {"cpssDxChPtpDomainV2IdGet", 3, cpssDxChPtpDomainV2IdGet_PARAMS, NULL},
    {"cpssDxChPtpEgressDomainTableSet", 4, cpssDxChPtpEgressDomainTableSet_PARAMS, NULL},
    {"cpssDxChPtpEgressDomainTableGet", 4, cpssDxChPtpEgressDomainTableGet_PARAMS, NULL},
    {"cpssDxChPtpIngressExceptionCfgSet", 2, cpssDxChPtpIngressExceptionCfgSet_PARAMS, NULL},
    {"cpssDxChPtpIngressExceptionCfgGet", 2, cpssDxChPtpIngressExceptionCfgGet_PARAMS, NULL},
    {"cpssDxChPtpIngressPacketCheckingModeSet", 4, cpssDxChPtpIngressPacketCheckingModeSet_PARAMS, NULL},
    {"cpssDxChPtpIngressPacketCheckingModeGet", 4, cpssDxChPtpIngressPacketCheckingModeGet_PARAMS, NULL},
    {"cpssDxChPtpIngressExceptionCounterGet", 2, prvCpssLogGenDevNumCounterPtr_PARAMS, NULL},
    {"cpssDxChPtpEgressExceptionCfgSet", 2, cpssDxChPtpEgressExceptionCfgSet_PARAMS, NULL},
    {"cpssDxChPtpEgressExceptionCfgGet", 2, cpssDxChPtpEgressExceptionCfgGet_PARAMS, NULL},
    {"cpssDxChPtpEgressExceptionCountersGet", 2, cpssDxChPtpEgressExceptionCountersGet_PARAMS, NULL},
    {"cpssDxChPtpTsIngressTimestampQueueEntryRead", 3, cpssDxChPtpTsIngressTimestampQueueEntryRead_PARAMS, NULL},
    {"cpssDxChPtpTsEgressTimestampQueueEntryRead", 3, cpssDxChPtpTsEgressTimestampQueueEntryRead_PARAMS, NULL},
    {"cpssDxChPtpTsMessageTypeToQueueIdMapSet", 2, cpssDxChPtpTsMessageTypeToQueueIdMapSet_PARAMS, NULL},
    {"cpssDxChPtpTsMessageTypeToQueueIdMapGet", 2, cpssDxChPtpTsMessageTypeToQueueIdMapGet_PARAMS, NULL},
    {"cpssDxChPtpTsQueuesEntryOverrideEnableSet", 2, prvCpssLogGenDevNumEnable_PARAMS, NULL},
    {"cpssDxChPtpTsQueuesEntryOverrideEnableGet", 2, prvCpssLogGenDevNumEnablePtr_PARAMS, NULL},
    {"cpssDxChPtpTsQueuesSizeSet", 2, cpssDxChPtpTsQueuesSizeSet_PARAMS, NULL},
    {"cpssDxChPtpTsQueuesSizeGet", 2, cpssDxChPtpTsQueuesSizeGet_PARAMS, NULL},
    {"cpssDxChPtpTsDebugQueuesEntryIdsClear", 1, prvCpssLogGenDevNum_PARAMS, NULL},
    {"cpssDxChPtpTsDebugQueuesEntryIdsGet", 3, cpssDxChPtpTsDebugQueuesEntryIdsGet_PARAMS, NULL},
    {"cpssDxChPtpTsNtpTimeOffsetSet", 2, cpssDxChPtpTsNtpTimeOffsetSet_PARAMS, NULL},
    {"cpssDxChPtpTsNtpTimeOffsetGet", 2, cpssDxChPtpTsNtpTimeOffsetGet_PARAMS, NULL},
    {"cpssDxChPtpTaiPtpPulseIterfaceSet", 3, cpssDxChPtpTaiPtpPulseIterfaceSet_PARAMS, NULL},
    {"cpssDxChPtpTaiPtpPulseIterfaceGet", 3, cpssDxChPtpTaiPtpPulseIterfaceGet_PARAMS, NULL},
    {"cpssDxChPtpTaiPtpClockInterfaceSet", 3, cpssDxChPtpTaiPtpClockInterfaceSet_PARAMS, NULL},
    {"cpssDxChPtpTaiPtpClockInterfaceGet", 3, cpssDxChPtpTaiPtpClockInterfaceGet_PARAMS, NULL},
    {"cpssDxChPtpTaiPClkOutputInterfaceSet", 3, cpssDxChPtpTaiPClkOutputInterfaceSet_PARAMS, NULL},
    {"cpssDxChPtpTaiPClkOutputInterfaceGet", 3, cpssDxChPtpTaiPClkOutputInterfaceGet_PARAMS, NULL},
    {"cpssDxChPtpTaiInputClockSelectSet", 3, cpssDxChPtpTaiInputClockSelectSet_PARAMS, NULL},
    {"cpssDxChPtpTaiInputClockSelectGet", 3, cpssDxChPtpTaiInputClockSelectGet_PARAMS, NULL},
    {"cpssDxChPtpTaiTodCounterFunctionAllTriggerSet", 1, prvCpssLogGenDevNum_PARAMS, NULL},
    {"cpssDxChPtpPortUnitResetSet", 4, cpssDxChPtpPortUnitResetSet_PARAMS, NULL},
    {"cpssDxChPtpPortUnitResetGet", 4, cpssDxChPtpPortUnitResetGet_PARAMS, NULL},
    {"cpssDxChPtpPortTxPipeStatusDelaySet", 3, cpssDxChPtpPortTxPipeStatusDelaySet_PARAMS, NULL},
    {"cpssDxChPtpPortTxPipeStatusDelayGet", 3, cpssDxChPtpPortTxPipeStatusDelayGet_PARAMS, NULL},
};

/******** DB Access Function ********/
void prvCpssLogParamLibDbGet_CPSS_LOG_LIB_PTP(
    OUT PRV_CPSS_LOG_FUNC_ENTRY_STC ** logFuncDbPtrPtr,
    OUT GT_U32 * logFuncDbSizePtr
)
{
    *logFuncDbPtrPtr = prvCpssDxChPtpLogLibDb;
    *logFuncDbSizePtr = sizeof(prvCpssDxChPtpLogLibDb) / sizeof(PRV_CPSS_LOG_FUNC_ENTRY_STC);
}

