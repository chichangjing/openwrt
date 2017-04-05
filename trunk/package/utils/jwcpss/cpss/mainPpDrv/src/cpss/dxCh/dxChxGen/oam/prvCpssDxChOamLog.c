/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* prvCpssDxChOamLog.c
*       WARNING!!! this is a generated file, please don't edit it manually
* COMMENTS:
*
* FILE REVISION NUMBER:
*       $Revision: 1 $
*******************************************************************************/
#include <cpss/generic/log/cpssLog.h>
#include <cpss/generic/log/prvCpssLog.h>
#include <cpss/extServices/private/prvCpssBindFunc.h>
#include <cpss/dxCh/dxChxGen/networkIf/private/prvCpssDxChNetworkIfLog.h>
#include <cpss/dxCh/dxChxGen/oam/cpssDxChOam.h>
#include <cpss/dxCh/dxChxGen/oam/cpssDxChOamSrv.h>
#include <cpss/dxCh/dxChxGen/oam/private/prvCpssDxChOamLog.h>
#include <cpss/generic/log/prvCpssGenLog.h>
#include <cpss/generic/networkIf/private/prvCpssGenNetworkIfLog.h>


/********* enums *********/

char * prvCpssLogEnum_CPSS_DXCH_OAM_AGING_BITMAP_UPDATE_MODE_ENT[]  =
{
    "CPSS_DXCH_OAM_AGING_BITMAP_UPDATE_MODE_ONLY_FAILURES_E",
    "CPSS_DXCH_OAM_AGING_BITMAP_UPDATE_MODE_ALL_E"
};
PRV_CPSS_LOG_STC_ENUM_ARRAY_SIZE_MAC(CPSS_DXCH_OAM_AGING_BITMAP_UPDATE_MODE_ENT);
char * prvCpssLogEnum_CPSS_DXCH_OAM_EXCEPTION_TYPE_ENT[]  =
{
    "CPSS_DXCH_OAM_EXCEPTION_TYPE_MEG_LEVEL_E",
    "CPSS_DXCH_OAM_EXCEPTION_TYPE_SOURCE_INTERFACE_E",
    "CPSS_DXCH_OAM_EXCEPTION_TYPE_RDI_STATUS_CHANGED_E",
    "CPSS_DXCH_OAM_EXCEPTION_TYPE_KEEPALIVE_AGING_E",
    "CPSS_DXCH_OAM_EXCEPTION_TYPE_KEEPALIVE_EXCESS_E",
    "CPSS_DXCH_OAM_EXCEPTION_TYPE_KEEPALIVE_INVALID_HASH_E",
    "CPSS_DXCH_OAM_EXCEPTION_TYPE_TX_PERIOD_E",
    "CPSS_DXCH_OAM_EXCEPTION_TYPE_SUMMARY_E",
    "CPSS_DXCH_OAM_EXCEPTION_TYPE_LAST_E"
};
PRV_CPSS_LOG_STC_ENUM_ARRAY_SIZE_MAC(CPSS_DXCH_OAM_EXCEPTION_TYPE_ENT);
char * prvCpssLogEnum_CPSS_DXCH_OAM_OPCODE_TYPE_ENT[]  =
{
    "CPSS_DXCH_OAM_OPCODE_TYPE_LM_COUNTED_E",
    "CPSS_DXCH_OAM_OPCODE_TYPE_LM_SINGLE_ENDED_E",
    "CPSS_DXCH_OAM_OPCODE_TYPE_LM_DUAL_ENDED_E",
    "CPSS_DXCH_OAM_OPCODE_TYPE_DM_E",
    "CPSS_DXCH_OAM_OPCODE_TYPE_KEEPALIVE_E"
};
PRV_CPSS_LOG_STC_ENUM_ARRAY_SIZE_MAC(CPSS_DXCH_OAM_OPCODE_TYPE_ENT);
char * prvCpssLogEnum_CPSS_DXCH_OAM_STAGE_TYPE_ENT[]  =
{
    "CPSS_DXCH_OAM_STAGE_TYPE_INGRESS_E",
    "CPSS_DXCH_OAM_STAGE_TYPE_EGRESS_E"
};
PRV_CPSS_LOG_STC_ENUM_ARRAY_SIZE_MAC(CPSS_DXCH_OAM_STAGE_TYPE_ENT);
char * prvCpssLogEnum_CPSS_DXCH_OAM_LM_COUNTING_MODE_ENT[]  =
{
    "CPSS_DXCH_OAM_LM_COUNTING_MODE_RETAIN_E",
    "CPSS_DXCH_OAM_LM_COUNTING_MODE_DISABLE_E",
    "CPSS_DXCH_OAM_LM_COUNTING_MODE_ENABLE_E"
};
PRV_CPSS_LOG_STC_ENUM_ARRAY_SIZE_MAC(CPSS_DXCH_OAM_LM_COUNTING_MODE_ENT);
char * prvCpssLogEnum_CPSS_DXCH_OAM_SOURCE_INTERFACE_CHECK_MODE_ENT[]  =
{
    "CPSS_DXCH_OAM_SOURCE_INTERFACE_CHECK_MODE_NO_MATCH_E",
    "CPSS_DXCH_OAM_SOURCE_INTERFACE_CHECK_MODE_MATCH_E"
};
PRV_CPSS_LOG_STC_ENUM_ARRAY_SIZE_MAC(CPSS_DXCH_OAM_SOURCE_INTERFACE_CHECK_MODE_ENT);
char * prvCpssLogEnum_CPSS_DXCH_OAM_SRV_FRAME_TYPE_ENT[]  =
{
    "CPSS_DXCH_OAM_SRV_FRAME_TYPE_1731_E",
    "CPSS_DXCH_OAM_SRV_FRAME_TYPE_1711_E"
};
PRV_CPSS_LOG_STC_ENUM_ARRAY_SIZE_MAC(CPSS_DXCH_OAM_SRV_FRAME_TYPE_ENT);
char * prvCpssLogEnum_CPSS_DXCH_OAM_SRV_MP_TYPE_ENT[]  =
{
    "CPSS_DXCH_OAM_SRV_MP_TYPE_UP_MEP_E",
    "CPSS_DXCH_OAM_SRV_MP_TYPE_DOWN_MEP_E",
    "CPSS_DXCH_OAM_SRV_MP_TYPE_MIP_E",
    "CPSS_DXCH_OAM_SRV_MP_TYPE_OTHER_E"
};
PRV_CPSS_LOG_STC_ENUM_ARRAY_SIZE_MAC(CPSS_DXCH_OAM_SRV_MP_TYPE_ENT);


/********* structure fields log functions *********/

void prvCpssLogParamFuncStc_CPSS_DXCH_OAM_ENTRY_STC_PTR
(
    IN CPSS_LOG_LIB_ENT         contextLib,
    IN CPSS_LOG_TYPE_ENT        logType,
    IN GT_CHAR_PTR              namePtr,
    IN void                   * fieldPtr,
    INOUT PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC     * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_START_FIELD_STC_MAC(CPSS_DXCH_OAM_ENTRY_STC *, valPtr);
    PRV_CPSS_LOG_UNUSED_MAC(inOutParamInfoPtr);
    PRV_CPSS_LOG_STC_BOOL_MAC(valPtr, opcodeParsingEnable);
    PRV_CPSS_LOG_STC_BOOL_MAC(valPtr, megLevelCheckEnable);
    PRV_CPSS_LOG_STC_NUMBER_MAC(valPtr, megLevel);
    PRV_CPSS_LOG_STC_NUMBER_MAC(valPtr, packetCommandProfile);
    PRV_CPSS_LOG_STC_NUMBER_MAC(valPtr, cpuCodeOffset);
    PRV_CPSS_LOG_STC_BOOL_MAC(valPtr, sourceInterfaceCheckEnable);
    PRV_CPSS_LOG_STC_ENUM_MAC(valPtr, sourceInterfaceCheckMode, CPSS_DXCH_OAM_SOURCE_INTERFACE_CHECK_MODE_ENT);
    PRV_CPSS_LOG_STC_STC_MAC(valPtr, sourceInterface, CPSS_INTERFACE_INFO_STC);
    PRV_CPSS_LOG_STC_BOOL_MAC(valPtr, lmCounterCaptureEnable);
    PRV_CPSS_LOG_STC_BOOL_MAC(valPtr, dualEndedLmEnable);
    PRV_CPSS_LOG_STC_ENUM_MAC(valPtr, lmCountingMode, CPSS_DXCH_OAM_LM_COUNTING_MODE_ENT);
    PRV_CPSS_LOG_STC_NUMBER_MAC(valPtr, oamPtpOffsetIndex);
    PRV_CPSS_LOG_STC_BOOL_MAC(valPtr, timestampEnable);
    PRV_CPSS_LOG_STC_BOOL_MAC(valPtr, keepaliveAgingEnable);
    PRV_CPSS_LOG_STC_NUMBER_MAC(valPtr, ageState);
    PRV_CPSS_LOG_STC_NUMBER_MAC(valPtr, agingPeriodIndex);
    PRV_CPSS_LOG_STC_NUMBER_MAC(valPtr, agingThreshold);
    PRV_CPSS_LOG_STC_BOOL_MAC(valPtr, hashVerifyEnable);
    PRV_CPSS_LOG_STC_BOOL_MAC(valPtr, lockHashValueEnable);
    PRV_CPSS_LOG_STC_BOOL_MAC(valPtr, protectionLocUpdateEnable);
    PRV_CPSS_LOG_STC_NUMBER_MAC(valPtr, flowHash);
    PRV_CPSS_LOG_STC_BOOL_MAC(valPtr, excessKeepaliveDetectionEnable);
    PRV_CPSS_LOG_STC_NUMBER_MAC(valPtr, excessKeepalivePeriodCounter);
    PRV_CPSS_LOG_STC_NUMBER_MAC(valPtr, excessKeepalivePeriodThreshold);
    PRV_CPSS_LOG_STC_NUMBER_MAC(valPtr, excessKeepaliveMessageCounter);
    PRV_CPSS_LOG_STC_NUMBER_MAC(valPtr, excessKeepaliveMessageThreshold);
    PRV_CPSS_LOG_STC_BOOL_MAC(valPtr, rdiCheckEnable);
    PRV_CPSS_LOG_STC_NUMBER_MAC(valPtr, rdiStatus);
    PRV_CPSS_LOG_STC_BOOL_MAC(valPtr, periodCheckEnable);
    PRV_CPSS_LOG_STC_NUMBER_MAC(valPtr, keepaliveTxPeriod);
    prvCpssLogStcLogEnd(contextLib, logType);
}
void prvCpssLogParamFuncStc_CPSS_DXCH_OAM_EXCEPTION_CONFIG_STC_PTR
(
    IN CPSS_LOG_LIB_ENT         contextLib,
    IN CPSS_LOG_TYPE_ENT        logType,
    IN GT_CHAR_PTR              namePtr,
    IN void                   * fieldPtr,
    INOUT PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC     * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_START_FIELD_STC_MAC(CPSS_DXCH_OAM_EXCEPTION_CONFIG_STC *, valPtr);
    PRV_CPSS_LOG_UNUSED_MAC(inOutParamInfoPtr);
    PRV_CPSS_LOG_STC_ENUM_MAC(valPtr, command, CPSS_PACKET_CMD_ENT);
    PRV_CPSS_LOG_STC_ENUM_MAP_MAC(valPtr, cpuCode, CPSS_NET_RX_CPU_CODE_ENT);
    PRV_CPSS_LOG_STC_BOOL_MAC(valPtr, summaryBitEnable);
    prvCpssLogStcLogEnd(contextLib, logType);
}
void prvCpssLogParamFuncStc_CPSS_DXCH_OAM_SRV_DM_CONFIG_STC_PTR
(
    IN CPSS_LOG_LIB_ENT         contextLib,
    IN CPSS_LOG_TYPE_ENT        logType,
    IN GT_CHAR_PTR              namePtr,
    IN void                   * fieldPtr,
    INOUT PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC     * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_START_FIELD_STC_MAC(CPSS_DXCH_OAM_SRV_DM_CONFIG_STC *, valPtr);
    PRV_CPSS_LOG_UNUSED_MAC(inOutParamInfoPtr);
    PRV_CPSS_LOG_STC_BOOL_MAC(valPtr, countersEnable);
    prvCpssLogStcLogEnd(contextLib, logType);
}
void prvCpssLogParamFuncStc_CPSS_DXCH_OAM_SRV_DM_COUNTERS_STC_PTR
(
    IN CPSS_LOG_LIB_ENT         contextLib,
    IN CPSS_LOG_TYPE_ENT        logType,
    IN GT_CHAR_PTR              namePtr,
    IN void                   * fieldPtr,
    INOUT PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC     * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_START_FIELD_STC_MAC(CPSS_DXCH_OAM_SRV_DM_COUNTERS_STC *, valPtr);
    PRV_CPSS_LOG_UNUSED_MAC(inOutParamInfoPtr);
    PRV_CPSS_LOG_STC_STC_MAC(valPtr, basicCounters, CPSS_DXCH_OAM_SRV_DM_BASIC_COUNTERS_STC);
    prvCpssLogStcLogEnd(contextLib, logType);
}
void prvCpssLogParamFuncStc_CPSS_DXCH_OAM_SRV_LB_CONFIG_STC_PTR
(
    IN CPSS_LOG_LIB_ENT         contextLib,
    IN CPSS_LOG_TYPE_ENT        logType,
    IN GT_CHAR_PTR              namePtr,
    IN void                   * fieldPtr,
    INOUT PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC     * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_START_FIELD_STC_MAC(CPSS_DXCH_OAM_SRV_LB_CONFIG_STC *, valPtr);
    PRV_CPSS_LOG_UNUSED_MAC(inOutParamInfoPtr);
    PRV_CPSS_LOG_STC_BOOL_MAC(valPtr, outOfSequenceEnable);
    PRV_CPSS_LOG_STC_BOOL_MAC(valPtr, invalidTlvEnable);
    PRV_CPSS_LOG_STC_NUMBER_MAC(valPtr, tlvDataCrc32Val);
    prvCpssLogStcLogEnd(contextLib, logType);
}
void prvCpssLogParamFuncStc_CPSS_DXCH_OAM_SRV_LB_COUNTERS_STC_PTR
(
    IN CPSS_LOG_LIB_ENT         contextLib,
    IN CPSS_LOG_TYPE_ENT        logType,
    IN GT_CHAR_PTR              namePtr,
    IN void                   * fieldPtr,
    INOUT PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC     * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_START_FIELD_STC_MAC(CPSS_DXCH_OAM_SRV_LB_COUNTERS_STC *, valPtr);
    PRV_CPSS_LOG_UNUSED_MAC(inOutParamInfoPtr);
    PRV_CPSS_LOG_STC_NUMBER_MAC(valPtr, outOfSequenceCount);
    PRV_CPSS_LOG_STC_NUMBER_MAC(valPtr, totalCheckedDataTlv);
    PRV_CPSS_LOG_STC_NUMBER_MAC(valPtr, totalInvalidDataTlv);
    prvCpssLogStcLogEnd(contextLib, logType);
}
void prvCpssLogParamFuncStc_CPSS_DXCH_OAM_SRV_LM_CONFIG_STC_PTR
(
    IN CPSS_LOG_LIB_ENT         contextLib,
    IN CPSS_LOG_TYPE_ENT        logType,
    IN GT_CHAR_PTR              namePtr,
    IN void                   * fieldPtr,
    INOUT PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC     * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_START_FIELD_STC_MAC(CPSS_DXCH_OAM_SRV_LM_CONFIG_STC *, valPtr);
    PRV_CPSS_LOG_UNUSED_MAC(inOutParamInfoPtr);
    PRV_CPSS_LOG_STC_BOOL_MAC(valPtr, countersEnable);
    prvCpssLogStcLogEnd(contextLib, logType);
}
void prvCpssLogParamFuncStc_CPSS_DXCH_OAM_SRV_LM_COUNTERS_STC_PTR
(
    IN CPSS_LOG_LIB_ENT         contextLib,
    IN CPSS_LOG_TYPE_ENT        logType,
    IN GT_CHAR_PTR              namePtr,
    IN void                   * fieldPtr,
    INOUT PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC     * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_START_FIELD_STC_MAC(CPSS_DXCH_OAM_SRV_LM_COUNTERS_STC *, valPtr);
    PRV_CPSS_LOG_UNUSED_MAC(inOutParamInfoPtr);
    PRV_CPSS_LOG_STC_STC_MAC(valPtr, basicCounters, CPSS_DXCH_OAM_SRV_LM_BASIC_COUNTERS_STC);
    prvCpssLogStcLogEnd(contextLib, logType);
}
void prvCpssLogParamFuncStc_CPSS_DXCH_OAM_SRV_RX_FLOW_FRAME_PARAMS_STC_PTR
(
    IN CPSS_LOG_LIB_ENT         contextLib,
    IN CPSS_LOG_TYPE_ENT        logType,
    IN GT_CHAR_PTR              namePtr,
    IN void                   * fieldPtr,
    INOUT PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC     * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_START_FIELD_STC_MAC(CPSS_DXCH_OAM_SRV_RX_FLOW_FRAME_PARAMS_STC *, valPtr);
    PRV_CPSS_LOG_UNUSED_MAC(inOutParamInfoPtr);
    PRV_CPSS_LOG_STC_ENUM_MAC(valPtr, frameType, CPSS_DXCH_OAM_SRV_FRAME_TYPE_ENT);
    prvCpssLogStcLogStart(contextLib,  logType, "frame");
    PRV_CPSS_LOG_STC_STC_MAC((&valPtr->frame), frame1731, CPSS_DXCH_OAM_SRV_1731_CONFIG_STC);
    prvCpssLogStcLogEnd(contextLib, logType);
    prvCpssLogStcLogEnd(contextLib, logType);
}
void prvCpssLogParamFuncStc_CPSS_DXCH_OAM_SRV_SYS_PARAMS_STC_PTR
(
    IN CPSS_LOG_LIB_ENT         contextLib,
    IN CPSS_LOG_TYPE_ENT        logType,
    IN GT_CHAR_PTR              namePtr,
    IN void                   * fieldPtr,
    INOUT PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC     * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_START_FIELD_STC_MAC(CPSS_DXCH_OAM_SRV_SYS_PARAMS_STC *, valPtr);
    PRV_CPSS_LOG_UNUSED_MAC(inOutParamInfoPtr);
    PRV_CPSS_LOG_STC_TYPE_ARRAY_MAC(valPtr, rxQueueNum, CPSS_DXCH_OAM_RX_SDMA_QUEUE_NUM_CNS, GT_U32);
    PRV_CPSS_LOG_STC_TYPE_ARRAY_MAC(valPtr, txQueueNum, CPSS_DXCH_OAM_TX_SDMA_QUEUE_NUM_CNS, GT_U32);
    PRV_CPSS_LOG_STC_NUMBER_MAC(valPtr, downMepsNum);
    PRV_CPSS_LOG_STC_NUMBER_MAC(valPtr, upMepsNum);
    PRV_CPSS_LOG_STC_NUMBER_MAC(valPtr, rxCcmFlowsNum);
    prvCpssLogStcLogEnd(contextLib, logType);
}
void prvCpssLogParamFuncStc_CPSS_DXCH_OAM_SRV_1731_CONFIG_STC_PTR
(
    IN CPSS_LOG_LIB_ENT         contextLib,
    IN CPSS_LOG_TYPE_ENT        logType,
    IN GT_CHAR_PTR              namePtr,
    IN void                   * fieldPtr,
    INOUT PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC     * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_START_FIELD_STC_MAC(CPSS_DXCH_OAM_SRV_1731_CONFIG_STC *, valPtr);
    PRV_CPSS_LOG_UNUSED_MAC(inOutParamInfoPtr);
    PRV_CPSS_LOG_STC_ENUM_MAC(valPtr, mpType, CPSS_DXCH_OAM_SRV_MP_TYPE_ENT);
    PRV_CPSS_LOG_STC_NUMBER_MAC(valPtr, cfmOffset);
    PRV_CPSS_LOG_STC_STC_MAC(valPtr, transmitParams, CPSS_DXCH_OAM_SRV_TRANSMIT_PARAMS_STC);
    prvCpssLogStcLogEnd(contextLib, logType);
}
void prvCpssLogParamFuncStc_CPSS_DXCH_OAM_SRV_DM_BASIC_COUNTERS_STC_PTR
(
    IN CPSS_LOG_LIB_ENT         contextLib,
    IN CPSS_LOG_TYPE_ENT        logType,
    IN GT_CHAR_PTR              namePtr,
    IN void                   * fieldPtr,
    INOUT PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC     * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_START_FIELD_STC_MAC(CPSS_DXCH_OAM_SRV_DM_BASIC_COUNTERS_STC *, valPtr);
    PRV_CPSS_LOG_UNUSED_MAC(inOutParamInfoPtr);
    PRV_CPSS_LOG_STC_NUMBER_MAC(valPtr, twoWayFrameDelay);
    PRV_CPSS_LOG_STC_NUMBER_MAC(valPtr, oneWayFrameDelayForward);
    PRV_CPSS_LOG_STC_NUMBER_MAC(valPtr, oneWayFrameDelayBackward);
    prvCpssLogStcLogEnd(contextLib, logType);
}
void prvCpssLogParamFuncStc_CPSS_DXCH_OAM_SRV_LM_BASIC_COUNTERS_STC_PTR
(
    IN CPSS_LOG_LIB_ENT         contextLib,
    IN CPSS_LOG_TYPE_ENT        logType,
    IN GT_CHAR_PTR              namePtr,
    IN void                   * fieldPtr,
    INOUT PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC     * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_START_FIELD_STC_MAC(CPSS_DXCH_OAM_SRV_LM_BASIC_COUNTERS_STC *, valPtr);
    PRV_CPSS_LOG_UNUSED_MAC(inOutParamInfoPtr);
    PRV_CPSS_LOG_STC_NUMBER_MAC(valPtr, txFrameCntForward);
    PRV_CPSS_LOG_STC_NUMBER_MAC(valPtr, rxFrameCntForward);
    PRV_CPSS_LOG_STC_NUMBER_MAC(valPtr, txFrameCntBackward);
    PRV_CPSS_LOG_STC_NUMBER_MAC(valPtr, rxFrameCntBackward);
    PRV_CPSS_LOG_STC_NUMBER_MAC(valPtr, farEndFrameLoss);
    PRV_CPSS_LOG_STC_NUMBER_MAC(valPtr, nearEndFrameLoss);
    prvCpssLogStcLogEnd(contextLib, logType);
}
void prvCpssLogParamFuncStc_CPSS_DXCH_OAM_SRV_TRANSMIT_PARAMS_STC_PTR
(
    IN CPSS_LOG_LIB_ENT         contextLib,
    IN CPSS_LOG_TYPE_ENT        logType,
    IN GT_CHAR_PTR              namePtr,
    IN void                   * fieldPtr,
    INOUT PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC     * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_START_FIELD_STC_MAC(CPSS_DXCH_OAM_SRV_TRANSMIT_PARAMS_STC *, valPtr);
    PRV_CPSS_LOG_UNUSED_MAC(inOutParamInfoPtr);
    PRV_CPSS_LOG_STC_BOOL_MAC(valPtr, l2HeaderValid);
    prvCpssLogStcLogStart(contextLib,  logType, "l2Header");
    PRV_CPSS_LOG_STC_TYPE_ARRAY_MAC((&valPtr->l2Header), header, 64, GT_U8);
    PRV_CPSS_LOG_STC_ETH_MAC((&valPtr->l2Header), macAddr);
    prvCpssLogStcLogEnd(contextLib, logType);
    PRV_CPSS_LOG_STC_BOOL_MAC(valPtr, dsaParamValid);
    PRV_CPSS_LOG_STC_STC_MAC(valPtr, dsaParam, CPSS_DXCH_NET_DSA_PARAMS_STC);
    prvCpssLogStcLogEnd(contextLib, logType);
}


/********* parameters log functions *********/

void prvCpssLogParamFunc_CPSS_DXCH_OAM_AGING_BITMAP_UPDATE_MODE_ENT(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_SET_PARAM_VAL_MAC(CPSS_DXCH_OAM_AGING_BITMAP_UPDATE_MODE_ENT, paramVal);
    PRV_CPSS_LOG_ENUM_MAC(namePtr, paramVal, CPSS_DXCH_OAM_AGING_BITMAP_UPDATE_MODE_ENT);
}
void prvCpssLogParamFunc_CPSS_DXCH_OAM_AGING_BITMAP_UPDATE_MODE_ENT_PTR(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_SET_PARAM_VAL_MAC(CPSS_DXCH_OAM_AGING_BITMAP_UPDATE_MODE_ENT*, paramVal);
    if (paramVal == NULL)
    {
        PRV_CPSS_LOG_AND_HISTORY_PARAM_MAC(contextLib, logType, "%s = NULL\n", namePtr);
        return;
    }
    PRV_CPSS_LOG_ENUM_MAC(namePtr, *paramVal, CPSS_DXCH_OAM_AGING_BITMAP_UPDATE_MODE_ENT);
}
void prvCpssLogParamFunc_CPSS_DXCH_OAM_ENTRY_STC_PTR(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_START_PARAM_STC_MAC(CPSS_DXCH_OAM_ENTRY_STC*, paramVal);
    prvCpssLogParamFuncStc_CPSS_DXCH_OAM_ENTRY_STC_PTR(contextLib, logType, namePtr, paramVal, inOutParamInfoPtr);
}
void prvCpssLogParamFunc_CPSS_DXCH_OAM_EXCEPTION_CONFIG_STC_PTR(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_START_PARAM_STC_MAC(CPSS_DXCH_OAM_EXCEPTION_CONFIG_STC*, paramVal);
    prvCpssLogParamFuncStc_CPSS_DXCH_OAM_EXCEPTION_CONFIG_STC_PTR(contextLib, logType, namePtr, paramVal, inOutParamInfoPtr);
}
void prvCpssLogParamFunc_CPSS_DXCH_OAM_EXCEPTION_TYPE_ENT(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_SET_PARAM_VAL_MAC(CPSS_DXCH_OAM_EXCEPTION_TYPE_ENT, paramVal);
    PRV_CPSS_LOG_ENUM_MAC(namePtr, paramVal, CPSS_DXCH_OAM_EXCEPTION_TYPE_ENT);
}
void prvCpssLogParamFunc_CPSS_DXCH_OAM_OPCODE_TYPE_ENT(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_SET_PARAM_VAL_MAC(CPSS_DXCH_OAM_OPCODE_TYPE_ENT, paramVal);
    PRV_CPSS_LOG_ENUM_MAC(namePtr, paramVal, CPSS_DXCH_OAM_OPCODE_TYPE_ENT);
}
void prvCpssLogParamFunc_CPSS_DXCH_OAM_SRV_DM_CONFIG_STC_PTR(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_START_PARAM_STC_MAC(CPSS_DXCH_OAM_SRV_DM_CONFIG_STC*, paramVal);
    prvCpssLogParamFuncStc_CPSS_DXCH_OAM_SRV_DM_CONFIG_STC_PTR(contextLib, logType, namePtr, paramVal, inOutParamInfoPtr);
}
void prvCpssLogParamFunc_CPSS_DXCH_OAM_SRV_DM_COUNTERS_STC_PTR(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_START_PARAM_STC_MAC(CPSS_DXCH_OAM_SRV_DM_COUNTERS_STC*, paramVal);
    prvCpssLogParamFuncStc_CPSS_DXCH_OAM_SRV_DM_COUNTERS_STC_PTR(contextLib, logType, namePtr, paramVal, inOutParamInfoPtr);
}
void prvCpssLogParamFunc_CPSS_DXCH_OAM_SRV_LB_CONFIG_STC_PTR(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_START_PARAM_STC_MAC(CPSS_DXCH_OAM_SRV_LB_CONFIG_STC*, paramVal);
    prvCpssLogParamFuncStc_CPSS_DXCH_OAM_SRV_LB_CONFIG_STC_PTR(contextLib, logType, namePtr, paramVal, inOutParamInfoPtr);
}
void prvCpssLogParamFunc_CPSS_DXCH_OAM_SRV_LB_COUNTERS_STC_PTR(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_START_PARAM_STC_MAC(CPSS_DXCH_OAM_SRV_LB_COUNTERS_STC*, paramVal);
    prvCpssLogParamFuncStc_CPSS_DXCH_OAM_SRV_LB_COUNTERS_STC_PTR(contextLib, logType, namePtr, paramVal, inOutParamInfoPtr);
}
void prvCpssLogParamFunc_CPSS_DXCH_OAM_SRV_LM_CONFIG_STC_PTR(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_START_PARAM_STC_MAC(CPSS_DXCH_OAM_SRV_LM_CONFIG_STC*, paramVal);
    prvCpssLogParamFuncStc_CPSS_DXCH_OAM_SRV_LM_CONFIG_STC_PTR(contextLib, logType, namePtr, paramVal, inOutParamInfoPtr);
}
void prvCpssLogParamFunc_CPSS_DXCH_OAM_SRV_LM_COUNTERS_STC_PTR(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_START_PARAM_STC_MAC(CPSS_DXCH_OAM_SRV_LM_COUNTERS_STC*, paramVal);
    prvCpssLogParamFuncStc_CPSS_DXCH_OAM_SRV_LM_COUNTERS_STC_PTR(contextLib, logType, namePtr, paramVal, inOutParamInfoPtr);
}
void prvCpssLogParamFunc_CPSS_DXCH_OAM_SRV_RX_FLOW_FRAME_PARAMS_STC_PTR(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_START_PARAM_STC_MAC(CPSS_DXCH_OAM_SRV_RX_FLOW_FRAME_PARAMS_STC*, paramVal);
    prvCpssLogParamFuncStc_CPSS_DXCH_OAM_SRV_RX_FLOW_FRAME_PARAMS_STC_PTR(contextLib, logType, namePtr, paramVal, inOutParamInfoPtr);
}
void prvCpssLogParamFunc_CPSS_DXCH_OAM_SRV_SYS_PARAMS_STC_PTR(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_START_PARAM_STC_MAC(CPSS_DXCH_OAM_SRV_SYS_PARAMS_STC*, paramVal);
    prvCpssLogParamFuncStc_CPSS_DXCH_OAM_SRV_SYS_PARAMS_STC_PTR(contextLib, logType, namePtr, paramVal, inOutParamInfoPtr);
}
void prvCpssLogParamFunc_CPSS_DXCH_OAM_STAGE_TYPE_ENT(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_SET_PARAM_VAL_MAC(CPSS_DXCH_OAM_STAGE_TYPE_ENT, paramVal);
    PRV_CPSS_LOG_ENUM_MAC(namePtr, paramVal, CPSS_DXCH_OAM_STAGE_TYPE_ENT);
}


/********* API fields DB *********/

PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_DXCH_OAM_AGING_BITMAP_UPDATE_MODE_ENT_mode = {
     "mode", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_MAC(CPSS_DXCH_OAM_AGING_BITMAP_UPDATE_MODE_ENT)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_DXCH_OAM_ENTRY_STC_PTR_entryPtr = {
     "entryPtr", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(CPSS_DXCH_OAM_ENTRY_STC)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_DXCH_OAM_EXCEPTION_CONFIG_STC_PTR_exceptionConfigPtr = {
     "exceptionConfigPtr", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(CPSS_DXCH_OAM_EXCEPTION_CONFIG_STC)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_DXCH_OAM_EXCEPTION_TYPE_ENT_exceptionType = {
     "exceptionType", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_MAC(CPSS_DXCH_OAM_EXCEPTION_TYPE_ENT)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_DXCH_OAM_OPCODE_TYPE_ENT_opcodeType = {
     "opcodeType", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_MAC(CPSS_DXCH_OAM_OPCODE_TYPE_ENT)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_DXCH_OAM_SRV_DM_CONFIG_STC_PTR_dmConfigPtr = {
     "dmConfigPtr", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(CPSS_DXCH_OAM_SRV_DM_CONFIG_STC)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_DXCH_OAM_SRV_LB_CONFIG_STC_PTR_lbConfigPtr = {
     "lbConfigPtr", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(CPSS_DXCH_OAM_SRV_LB_CONFIG_STC)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_DXCH_OAM_SRV_LM_CONFIG_STC_PTR_lmConfigPtr = {
     "lmConfigPtr", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(CPSS_DXCH_OAM_SRV_LM_CONFIG_STC)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_DXCH_OAM_SRV_RX_FLOW_FRAME_PARAMS_STC_PTR_frameParamsPtr = {
     "frameParamsPtr", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(CPSS_DXCH_OAM_SRV_RX_FLOW_FRAME_PARAMS_STC)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_DXCH_OAM_SRV_SYS_PARAMS_STC_PTR_sysParamsPtr = {
     "sysParamsPtr", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(CPSS_DXCH_OAM_SRV_SYS_PARAMS_STC)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_DXCH_OAM_STAGE_TYPE_ENT_stage = {
     "stage", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_MAC(CPSS_DXCH_OAM_STAGE_TYPE_ENT)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_NET_RX_CPU_CODE_ENT_baseCpuCode = {
     "baseCpuCode", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_MAC(CPSS_NET_RX_CPU_CODE_ENT)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_BOOL_clearOnRead = {
     "clearOnRead", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_MAC(GT_BOOL)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_BOOL_clearOnReadEnable = {
     "clearOnReadEnable", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_MAC(GT_BOOL)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_BOOL_profileEnable = {
     "profileEnable", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_MAC(GT_BOOL)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_ETHERADDR_macAddr = {
     "macAddr", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_MAC(GT_ETHERADDR)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_PTR_cookiePtr = {
     "cookiePtr", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_MAC(GT_PTR)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_U32_baseFlowId = {
     "baseFlowId", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_MAC(GT_U32)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_U32_channelTypeId = {
     "channelTypeId", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_MAC(GT_U32)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_U32_cpuCodeLsBits = {
     "cpuCodeLsBits", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_MAC(GT_U32)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_U32_cpuCodeOffset = {
     "cpuCodeOffset", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_MAC(GT_U32)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_U32_flowId = {
     "flowId", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_MAC(GT_U32)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_U32_hashFirstBit = {
     "hashFirstBit", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_MAC(GT_U32)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_U32_hashLastBit = {
     "hashLastBit", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_MAC(GT_U32)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_U32_macAddIndex = {
     "macAddIndex", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_MAC(GT_U32)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_U32_opcodeIndex = {
     "opcodeIndex", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_MAC(GT_U32)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_U32_opcodeValue = {
     "opcodeValue", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_MAC(GT_U32)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_U64_agingPeriodValue = {
     "agingPeriodValue", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_MAC(GT_U64)
};
PRV_CPSS_LOG_FUNC_PARAM_STC OUT_CPSS_DXCH_OAM_AGING_BITMAP_UPDATE_MODE_ENT_PTR_modePtr = {
     "modePtr", PRV_CPSS_LOG_PARAM_OUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(CPSS_DXCH_OAM_AGING_BITMAP_UPDATE_MODE_ENT)
};
PRV_CPSS_LOG_FUNC_PARAM_STC OUT_CPSS_DXCH_OAM_ENTRY_STC_PTR_entryPtr = {
     "entryPtr", PRV_CPSS_LOG_PARAM_OUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(CPSS_DXCH_OAM_ENTRY_STC)
};
PRV_CPSS_LOG_FUNC_PARAM_STC OUT_CPSS_DXCH_OAM_EXCEPTION_CONFIG_STC_PTR_exceptionConfigPtr = {
     "exceptionConfigPtr", PRV_CPSS_LOG_PARAM_OUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(CPSS_DXCH_OAM_EXCEPTION_CONFIG_STC)
};
PRV_CPSS_LOG_FUNC_PARAM_STC OUT_CPSS_DXCH_OAM_SRV_DM_CONFIG_STC_PTR_dmConfigPtr = {
     "dmConfigPtr", PRV_CPSS_LOG_PARAM_OUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(CPSS_DXCH_OAM_SRV_DM_CONFIG_STC)
};
PRV_CPSS_LOG_FUNC_PARAM_STC OUT_CPSS_DXCH_OAM_SRV_DM_COUNTERS_STC_PTR_dmCountersPtr = {
     "dmCountersPtr", PRV_CPSS_LOG_PARAM_OUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(CPSS_DXCH_OAM_SRV_DM_COUNTERS_STC)
};
PRV_CPSS_LOG_FUNC_PARAM_STC OUT_CPSS_DXCH_OAM_SRV_LB_CONFIG_STC_PTR_lbConfigPtr = {
     "lbConfigPtr", PRV_CPSS_LOG_PARAM_OUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(CPSS_DXCH_OAM_SRV_LB_CONFIG_STC)
};
PRV_CPSS_LOG_FUNC_PARAM_STC OUT_CPSS_DXCH_OAM_SRV_LB_COUNTERS_STC_PTR_lbCountersPtr = {
     "lbCountersPtr", PRV_CPSS_LOG_PARAM_OUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(CPSS_DXCH_OAM_SRV_LB_COUNTERS_STC)
};
PRV_CPSS_LOG_FUNC_PARAM_STC OUT_CPSS_DXCH_OAM_SRV_LM_CONFIG_STC_PTR_lmConfigPtr = {
     "lmConfigPtr", PRV_CPSS_LOG_PARAM_OUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(CPSS_DXCH_OAM_SRV_LM_CONFIG_STC)
};
PRV_CPSS_LOG_FUNC_PARAM_STC OUT_CPSS_DXCH_OAM_SRV_LM_COUNTERS_STC_PTR_lmCountersPtr = {
     "lmCountersPtr", PRV_CPSS_LOG_PARAM_OUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(CPSS_DXCH_OAM_SRV_LM_COUNTERS_STC)
};
PRV_CPSS_LOG_FUNC_PARAM_STC OUT_CPSS_DXCH_OAM_SRV_RX_FLOW_FRAME_PARAMS_STC_PTR_frameParamsPtr = {
     "frameParamsPtr", PRV_CPSS_LOG_PARAM_OUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(CPSS_DXCH_OAM_SRV_RX_FLOW_FRAME_PARAMS_STC)
};
PRV_CPSS_LOG_FUNC_PARAM_STC OUT_CPSS_NET_RX_CPU_CODE_ENT_PTR_baseCpuCodePtr = {
     "baseCpuCodePtr", PRV_CPSS_LOG_PARAM_OUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(CPSS_NET_RX_CPU_CODE_ENT)
};
PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_BOOL_PTR_profileEnablePtr = {
     "profileEnablePtr", PRV_CPSS_LOG_PARAM_OUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(GT_BOOL)
};
PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_ETHERADDR_PTR_macAddrPtr = {
     "macAddrPtr", PRV_CPSS_LOG_PARAM_OUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(GT_ETHERADDR)
};
PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_U32_PTR_baseFlowIdPtr = {
     "baseFlowIdPtr", PRV_CPSS_LOG_PARAM_OUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(GT_U32)
};
PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_U32_PTR_channelTypeIdPtr = {
     "channelTypeIdPtr", PRV_CPSS_LOG_PARAM_OUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(GT_U32)
};
PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_U32_PTR_counterValuePtr = {
     "counterValuePtr", PRV_CPSS_LOG_PARAM_OUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(GT_U32)
};
PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_U32_PTR_cpuCodeLsBitsPtr = {
     "cpuCodeLsBitsPtr", PRV_CPSS_LOG_PARAM_OUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(GT_U32)
};
PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_U32_PTR_cpuCodeOffsetPtr = {
     "cpuCodeOffsetPtr", PRV_CPSS_LOG_PARAM_OUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(GT_U32)
};
PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_U32_PTR_entryBmpPtr = {
     "entryBmpPtr", PRV_CPSS_LOG_PARAM_OUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(GT_U32)
};
PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_U32_PTR_groupStatusArr = {
     "groupStatusArr", PRV_CPSS_LOG_PARAM_OUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(GT_U32)
};
PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_U32_PTR_hashFirstBitPtr = {
     "hashFirstBitPtr", PRV_CPSS_LOG_PARAM_OUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(GT_U32)
};
PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_U32_PTR_hashLastBitPtr = {
     "hashLastBitPtr", PRV_CPSS_LOG_PARAM_OUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(GT_U32)
};
PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_U32_PTR_macAddIndexPtr = {
     "macAddIndexPtr", PRV_CPSS_LOG_PARAM_OUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(GT_U32)
};
PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_U32_PTR_opcodeValuePtr = {
     "opcodeValuePtr", PRV_CPSS_LOG_PARAM_OUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(GT_U32)
};
PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_U64_PTR_agingPeriodValuePtr = {
     "agingPeriodValuePtr", PRV_CPSS_LOG_PARAM_OUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(GT_U64)
};


/********* API prototypes DB *********/

PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChOamSrvSystemInit_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_CPSS_DXCH_OAM_SRV_SYS_PARAMS_STC_PTR_sysParamsPtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChOamAgingBitmapUpdateModeSet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_CPSS_DXCH_OAM_STAGE_TYPE_ENT_stage,
    &IN_CPSS_DXCH_OAM_AGING_BITMAP_UPDATE_MODE_ENT_mode
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChOamExceptionConfigSet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_CPSS_DXCH_OAM_STAGE_TYPE_ENT_stage,
    &IN_CPSS_DXCH_OAM_EXCEPTION_TYPE_ENT_exceptionType,
    &IN_CPSS_DXCH_OAM_EXCEPTION_CONFIG_STC_PTR_exceptionConfigPtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChOamExceptionStatusGet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_CPSS_DXCH_OAM_STAGE_TYPE_ENT_stage,
    &IN_CPSS_DXCH_OAM_EXCEPTION_TYPE_ENT_exceptionType,
    &IN_GT_U32_entryIndex,
    &OUT_GT_U32_PTR_entryBmpPtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChOamExceptionConfigGet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_CPSS_DXCH_OAM_STAGE_TYPE_ENT_stage,
    &IN_CPSS_DXCH_OAM_EXCEPTION_TYPE_ENT_exceptionType,
    &OUT_CPSS_DXCH_OAM_EXCEPTION_CONFIG_STC_PTR_exceptionConfigPtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChOamExceptionCounterGet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_CPSS_DXCH_OAM_STAGE_TYPE_ENT_stage,
    &IN_CPSS_DXCH_OAM_EXCEPTION_TYPE_ENT_exceptionType,
    &OUT_GT_U32_PTR_counterValuePtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChOamOpcodeSet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_CPSS_DXCH_OAM_STAGE_TYPE_ENT_stage,
    &IN_CPSS_DXCH_OAM_OPCODE_TYPE_ENT_opcodeType,
    &IN_GT_U32_opcodeIndex,
    &IN_GT_U32_opcodeValue
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChOamOpcodeGet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_CPSS_DXCH_OAM_STAGE_TYPE_ENT_stage,
    &IN_CPSS_DXCH_OAM_OPCODE_TYPE_ENT_opcodeType,
    &IN_GT_U32_opcodeIndex,
    &OUT_GT_U32_PTR_opcodeValuePtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChOamCpuCodeBaseSet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_CPSS_DXCH_OAM_STAGE_TYPE_ENT_stage,
    &IN_CPSS_NET_RX_CPU_CODE_ENT_baseCpuCode
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChOamKeepaliveForPacketCommandEnableSet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_CPSS_DXCH_OAM_STAGE_TYPE_ENT_stage,
    &IN_CPSS_PACKET_CMD_ENT_command,
    &IN_GT_BOOL_enable
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChOamDualEndedLmPacketCommandSet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_CPSS_DXCH_OAM_STAGE_TYPE_ENT_stage,
    &IN_CPSS_PACKET_CMD_ENT_command,
    &IN_GT_U32_cpuCodeLsBits
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChOamKeepaliveForPacketCommandEnableGet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_CPSS_DXCH_OAM_STAGE_TYPE_ENT_stage,
    &IN_CPSS_PACKET_CMD_ENT_command,
    &OUT_GT_BOOL_PTR_enablePtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChOamEnableSet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_CPSS_DXCH_OAM_STAGE_TYPE_ENT_stage,
    &IN_GT_BOOL_enable
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChOamTableBaseFlowIdSet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_CPSS_DXCH_OAM_STAGE_TYPE_ENT_stage,
    &IN_GT_U32_baseFlowId
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChOamEntrySet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_CPSS_DXCH_OAM_STAGE_TYPE_ENT_stage,
    &IN_GT_U32_entryIndex,
    &IN_CPSS_DXCH_OAM_ENTRY_STC_PTR_entryPtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChOamAgingPeriodEntrySet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_CPSS_DXCH_OAM_STAGE_TYPE_ENT_stage,
    &IN_GT_U32_entryIndex,
    &IN_GT_U64_agingPeriodValue
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChOamEntryGet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_CPSS_DXCH_OAM_STAGE_TYPE_ENT_stage,
    &IN_GT_U32_entryIndex,
    &OUT_CPSS_DXCH_OAM_ENTRY_STC_PTR_entryPtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChOamAgingPeriodEntryGet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_CPSS_DXCH_OAM_STAGE_TYPE_ENT_stage,
    &IN_GT_U32_entryIndex,
    &OUT_GT_U64_PTR_agingPeriodValuePtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChOamHashBitSelectionSet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_CPSS_DXCH_OAM_STAGE_TYPE_ENT_stage,
    &IN_GT_U32_hashFirstBit,
    &IN_GT_U32_hashLastBit
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChOamOpcodeProfilePacketCommandEntrySet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_CPSS_DXCH_OAM_STAGE_TYPE_ENT_stage,
    &IN_GT_U32_opcode,
    &IN_GT_U32_profile,
    &IN_CPSS_PACKET_CMD_ENT_command,
    &IN_GT_U32_cpuCodeOffset
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChOamOpcodeProfilePacketCommandEntryGet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_CPSS_DXCH_OAM_STAGE_TYPE_ENT_stage,
    &IN_GT_U32_opcode,
    &IN_GT_U32_profile,
    &OUT_CPSS_PACKET_CMD_ENT_PTR_commandPtr,
    &OUT_GT_U32_PTR_cpuCodeOffsetPtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChOamAgingBitmapUpdateModeGet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_CPSS_DXCH_OAM_STAGE_TYPE_ENT_stage,
    &OUT_CPSS_DXCH_OAM_AGING_BITMAP_UPDATE_MODE_ENT_PTR_modePtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChOamCpuCodeBaseGet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_CPSS_DXCH_OAM_STAGE_TYPE_ENT_stage,
    &OUT_CPSS_NET_RX_CPU_CODE_ENT_PTR_baseCpuCodePtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChOamDualEndedLmPacketCommandGet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_CPSS_DXCH_OAM_STAGE_TYPE_ENT_stage,
    &OUT_CPSS_PACKET_CMD_ENT_PTR_commandPtr,
    &OUT_GT_U32_PTR_cpuCodeLsBitsPtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChOamEnableGet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_CPSS_DXCH_OAM_STAGE_TYPE_ENT_stage,
    &OUT_GT_BOOL_PTR_enablePtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChOamTableBaseFlowIdGet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_CPSS_DXCH_OAM_STAGE_TYPE_ENT_stage,
    &OUT_GT_U32_PTR_baseFlowIdPtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChOamHashBitSelectionGet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_CPSS_DXCH_OAM_STAGE_TYPE_ENT_stage,
    &OUT_GT_U32_PTR_hashFirstBitPtr,
    &OUT_GT_U32_PTR_hashLastBitPtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChOamPortGroupEntrySet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_PORT_GROUPS_BMP_portGroupsBmp,
    &IN_CPSS_DXCH_OAM_STAGE_TYPE_ENT_stage,
    &IN_GT_U32_entryIndex,
    &IN_CPSS_DXCH_OAM_ENTRY_STC_PTR_entryPtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChOamPortGroupEntryGet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_PORT_GROUPS_BMP_portGroupsBmp,
    &IN_CPSS_DXCH_OAM_STAGE_TYPE_ENT_stage,
    &IN_GT_U32_entryIndex,
    &OUT_CPSS_DXCH_OAM_ENTRY_STC_PTR_entryPtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChOamSrvLocalInterfaceMacIndexSet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_PORT_NUM_portNum,
    &IN_GT_BOOL_isPhysicalPort,
    &IN_GT_U32_macAddIndex
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChOamSrvLocalInterfaceMacIndexGet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_PORT_NUM_portNum,
    &IN_GT_BOOL_isPhysicalPort,
    &OUT_GT_U32_PTR_macAddIndexPtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChOamLmOffsetTableSet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_U32_entryIndex,
    &IN_GT_U32_offset
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChOamLmOffsetTableGet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_U32_entryIndex,
    &OUT_GT_U32_PTR_offsetPtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChOamSrvLoopbackStatisticalCountersClear_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_U32_flowId
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChOamSrvDmConfigSet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_U32_flowId,
    &IN_CPSS_DXCH_OAM_SRV_DM_CONFIG_STC_PTR_dmConfigPtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChOamSrvLoopbackConfigSet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_U32_flowId,
    &IN_CPSS_DXCH_OAM_SRV_LB_CONFIG_STC_PTR_lbConfigPtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChOamSrvLmConfigSet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_U32_flowId,
    &IN_CPSS_DXCH_OAM_SRV_LM_CONFIG_STC_PTR_lmConfigPtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChOamSrvLoopbackStatisticalCountersGet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_U32_flowId,
    &IN_GT_BOOL_clearOnRead,
    &IN_GT_U32_timeout,
    &OUT_CPSS_DXCH_OAM_SRV_LB_COUNTERS_STC_PTR_lbCountersPtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChOamSrvDmStatisticalCountersGet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_U32_flowId,
    &IN_GT_BOOL_clearOnRead,
    &OUT_CPSS_DXCH_OAM_SRV_DM_COUNTERS_STC_PTR_dmCountersPtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChOamSrvLmStatisticalCountersGet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_U32_flowId,
    &IN_GT_BOOL_clearOnReadEnable,
    &OUT_CPSS_DXCH_OAM_SRV_LM_COUNTERS_STC_PTR_lmCountersPtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChOamSrvRxFlowEntrySet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_U32_flowId,
    &IN_GT_PTR_cookiePtr,
    &IN_CPSS_DXCH_OAM_SRV_RX_FLOW_FRAME_PARAMS_STC_PTR_frameParamsPtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChOamSrvDmConfigGet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_U32_flowId,
    &OUT_CPSS_DXCH_OAM_SRV_DM_CONFIG_STC_PTR_dmConfigPtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChOamSrvLoopbackConfigGet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_U32_flowId,
    &OUT_CPSS_DXCH_OAM_SRV_LB_CONFIG_STC_PTR_lbConfigPtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChOamSrvLmConfigGet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_U32_flowId,
    &OUT_CPSS_DXCH_OAM_SRV_LM_CONFIG_STC_PTR_lmConfigPtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChOamSrvRxFlowEntryGet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_U32_flowId,
    &OUT_GT_PTR_PTR_cookiePtr,
    &OUT_CPSS_DXCH_OAM_SRV_RX_FLOW_FRAME_PARAMS_STC_PTR_frameParamsPtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChOamSrvLocalMacAddressSet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_U32_index,
    &IN_GT_ETHERADDR_macAddr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChOamSrvLocalMacAddressGet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_U32_index,
    &OUT_GT_ETHERADDR_PTR_macAddrPtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChOamMplsCwChannelTypeProfileSet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_U32_profileIndex,
    &IN_GT_U32_channelTypeId,
    &IN_GT_BOOL_profileEnable
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChOamMplsCwChannelTypeProfileGet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_U32_profileIndex,
    &OUT_GT_U32_PTR_channelTypeIdPtr,
    &OUT_GT_BOOL_PTR_profileEnablePtr
};
extern PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChOamExceptionGroupStatusGet_PARAMS[];


/********* lib API DB *********/

extern void cpssDxChOamExceptionGroupStatusGet_preLogic(IN va_list  args, OUT PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC * paramDataPtr);

static PRV_CPSS_LOG_FUNC_ENTRY_STC prvCpssDxChOamLogLibDb[] = {
    {"cpssDxChOamEntrySet", 4, cpssDxChOamEntrySet_PARAMS, NULL},
    {"cpssDxChOamPortGroupEntrySet", 5, cpssDxChOamPortGroupEntrySet_PARAMS, NULL},
    {"cpssDxChOamEntryGet", 4, cpssDxChOamEntryGet_PARAMS, NULL},
    {"cpssDxChOamPortGroupEntryGet", 5, cpssDxChOamPortGroupEntryGet_PARAMS, NULL},
    {"cpssDxChOamEnableSet", 3, cpssDxChOamEnableSet_PARAMS, NULL},
    {"cpssDxChOamEnableGet", 3, cpssDxChOamEnableGet_PARAMS, NULL},
    {"cpssDxChOamTableBaseFlowIdSet", 3, cpssDxChOamTableBaseFlowIdSet_PARAMS, NULL},
    {"cpssDxChOamTableBaseFlowIdGet", 3, cpssDxChOamTableBaseFlowIdGet_PARAMS, NULL},
    {"cpssDxChOamOpcodeProfileDedicatedMcProfileEnableSet", 3, cpssDxChOamEnableSet_PARAMS, NULL},
    {"cpssDxChOamOpcodeProfileDedicatedMcProfileEnableGet", 3, cpssDxChOamEnableGet_PARAMS, NULL},
    {"cpssDxChOamExceptionConfigSet", 4, cpssDxChOamExceptionConfigSet_PARAMS, NULL},
    {"cpssDxChOamExceptionConfigGet", 4, cpssDxChOamExceptionConfigGet_PARAMS, NULL},
    {"cpssDxChOamExceptionCounterGet", 4, cpssDxChOamExceptionCounterGet_PARAMS, NULL},
    {"cpssDxChOamExceptionGroupStatusGet", 4, cpssDxChOamExceptionGroupStatusGet_PARAMS, cpssDxChOamExceptionGroupStatusGet_preLogic},
    {"cpssDxChOamOpcodeSet", 5, cpssDxChOamOpcodeSet_PARAMS, NULL},
    {"cpssDxChOamOpcodeGet", 5, cpssDxChOamOpcodeGet_PARAMS, NULL},
    {"cpssDxChOamAgingPeriodEntrySet", 4, cpssDxChOamAgingPeriodEntrySet_PARAMS, NULL},
    {"cpssDxChOamAgingPeriodEntryGet", 4, cpssDxChOamAgingPeriodEntryGet_PARAMS, NULL},
    {"cpssDxChOamOpcodeProfilePacketCommandEntrySet", 6, cpssDxChOamOpcodeProfilePacketCommandEntrySet_PARAMS, NULL},
    {"cpssDxChOamOpcodeProfilePacketCommandEntryGet", 6, cpssDxChOamOpcodeProfilePacketCommandEntryGet_PARAMS, NULL},
    {"cpssDxChOamTimeStampEtherTypeSet", 2, prvCpssLogGenDevNumEtherType_PARAMS, NULL},
    {"cpssDxChOamTimeStampEtherTypeGet", 2, prvCpssLogGenDevNumEtherTypePtr_PARAMS, NULL},
    {"cpssDxChOamTimeStampParsingEnableSet", 2, prvCpssLogGenDevNumEnable_PARAMS, NULL},
    {"cpssDxChOamTimeStampParsingEnableGet", 2, prvCpssLogGenDevNumEnablePtr_PARAMS, NULL},
    {"cpssDxChOamLmOffsetTableSet", 3, cpssDxChOamLmOffsetTableSet_PARAMS, NULL},
    {"cpssDxChOamLmOffsetTableGet", 3, cpssDxChOamLmOffsetTableGet_PARAMS, NULL},
    {"cpssDxChOamCpuCodeBaseSet", 3, cpssDxChOamCpuCodeBaseSet_PARAMS, NULL},
    {"cpssDxChOamCpuCodeBaseGet", 3, cpssDxChOamCpuCodeBaseGet_PARAMS, NULL},
    {"cpssDxChOamDualEndedLmPacketCommandSet", 4, cpssDxChOamDualEndedLmPacketCommandSet_PARAMS, NULL},
    {"cpssDxChOamDualEndedLmPacketCommandGet", 4, cpssDxChOamDualEndedLmPacketCommandGet_PARAMS, NULL},
    {"cpssDxChOamExceptionStatusGet", 5, cpssDxChOamExceptionStatusGet_PARAMS, NULL},
    {"cpssDxChOamAgingDaemonEnableSet", 3, cpssDxChOamEnableSet_PARAMS, NULL},
    {"cpssDxChOamAgingDaemonEnableGet", 3, cpssDxChOamEnableGet_PARAMS, NULL},
    {"cpssDxChOamPduCpuCodeSet", 2, prvCpssLogGenDevNumCpuCode_PARAMS, NULL},
    {"cpssDxChOamPduCpuCodeGet", 2, prvCpssLogGenDevNumCpuCodePtr_PARAMS, NULL},
    {"cpssDxChOamMplsCwChannelTypeProfileSet", 4, cpssDxChOamMplsCwChannelTypeProfileSet_PARAMS, NULL},
    {"cpssDxChOamMplsCwChannelTypeProfileGet", 4, cpssDxChOamMplsCwChannelTypeProfileGet_PARAMS, NULL},
    {"cpssDxChOamAgingBitmapUpdateModeSet", 3, cpssDxChOamAgingBitmapUpdateModeSet_PARAMS, NULL},
    {"cpssDxChOamAgingBitmapUpdateModeGet", 3, cpssDxChOamAgingBitmapUpdateModeGet_PARAMS, NULL},
    {"cpssDxChOamHashBitSelectionSet", 4, cpssDxChOamHashBitSelectionSet_PARAMS, NULL},
    {"cpssDxChOamHashBitSelectionGet", 4, cpssDxChOamHashBitSelectionGet_PARAMS, NULL},
    {"cpssDxChOamKeepaliveForPacketCommandEnableSet", 4, cpssDxChOamKeepaliveForPacketCommandEnableSet_PARAMS, NULL},
    {"cpssDxChOamKeepaliveForPacketCommandEnableGet", 4, cpssDxChOamKeepaliveForPacketCommandEnableGet_PARAMS, NULL},
    {"cpssDxChOamSrvLoopbackConfigSet", 3, cpssDxChOamSrvLoopbackConfigSet_PARAMS, NULL},
    {"cpssDxChOamSrvLoopbackConfigGet", 3, cpssDxChOamSrvLoopbackConfigGet_PARAMS, NULL},
    {"cpssDxChOamSrvLoopbackStatisticalCountersGet", 5, cpssDxChOamSrvLoopbackStatisticalCountersGet_PARAMS, NULL},
    {"cpssDxChOamSrvLoopbackStatisticalCountersClear", 2, cpssDxChOamSrvLoopbackStatisticalCountersClear_PARAMS, NULL},
    {"cpssDxChOamSrvDmConfigSet", 3, cpssDxChOamSrvDmConfigSet_PARAMS, NULL},
    {"cpssDxChOamSrvDmConfigGet", 3, cpssDxChOamSrvDmConfigGet_PARAMS, NULL},
    {"cpssDxChOamSrvDmStatisticalCountersGet", 4, cpssDxChOamSrvDmStatisticalCountersGet_PARAMS, NULL},
    {"cpssDxChOamSrvDmStatisticalCountersClear", 2, cpssDxChOamSrvLoopbackStatisticalCountersClear_PARAMS, NULL},
    {"cpssDxChOamSrvLmConfigSet", 3, cpssDxChOamSrvLmConfigSet_PARAMS, NULL},
    {"cpssDxChOamSrvLmConfigGet", 3, cpssDxChOamSrvLmConfigGet_PARAMS, NULL},
    {"cpssDxChOamSrvLmStatisticalCountersGet", 4, cpssDxChOamSrvLmStatisticalCountersGet_PARAMS, NULL},
    {"cpssDxChOamSrvLmStatisticalCountersClear", 2, cpssDxChOamSrvLoopbackStatisticalCountersClear_PARAMS, NULL},
    {"cpssDxChOamSrvLocalMacAddressSet", 3, cpssDxChOamSrvLocalMacAddressSet_PARAMS, NULL},
    {"cpssDxChOamSrvLocalMacAddressGet", 3, cpssDxChOamSrvLocalMacAddressGet_PARAMS, NULL},
    {"cpssDxChOamSrvLocalInterfaceMacIndexSet", 4, cpssDxChOamSrvLocalInterfaceMacIndexSet_PARAMS, NULL},
    {"cpssDxChOamSrvLocalInterfaceMacIndexGet", 4, cpssDxChOamSrvLocalInterfaceMacIndexGet_PARAMS, NULL},
    {"cpssDxChOamSrvRxFlowEntrySet", 4, cpssDxChOamSrvRxFlowEntrySet_PARAMS, NULL},
    {"cpssDxChOamSrvRxFlowEntryGet", 4, cpssDxChOamSrvRxFlowEntryGet_PARAMS, NULL},
    {"cpssDxChOamSrvSystemInit", 2, cpssDxChOamSrvSystemInit_PARAMS, NULL},
};

/******** DB Access Function ********/
void prvCpssLogParamLibDbGet_CPSS_LOG_LIB_OAM(
    OUT PRV_CPSS_LOG_FUNC_ENTRY_STC ** logFuncDbPtrPtr,
    OUT GT_U32 * logFuncDbSizePtr
)
{
    *logFuncDbPtrPtr = prvCpssDxChOamLogLibDb;
    *logFuncDbSizePtr = sizeof(prvCpssDxChOamLogLibDb) / sizeof(PRV_CPSS_LOG_FUNC_ENTRY_STC);
}

