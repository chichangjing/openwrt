/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* prvCpssDxChNetworkIfLog.c
*       WARNING!!! this is a generated file, please don't edit it manually
* COMMENTS:
*
* FILE REVISION NUMBER:
*       $Revision: 1 $
*******************************************************************************/
#include <cpss/generic/log/cpssLog.h>
#include <cpss/generic/log/prvCpssLog.h>
#include <cpss/extServices/private/prvCpssBindFunc.h>
#include <cpss/dxCh/dxChxGen/networkIf/cpssDxChNetIfMii.h>
#include <cpss/dxCh/dxChxGen/networkIf/cpssDxChNetIfTypes.h>
#include <cpss/dxCh/dxChxGen/networkIf/private/prvCpssDxChNetworkIfLog.h>
#include <cpss/generic/cos/private/prvCpssGenCosLog.h>
#include <cpss/generic/log/prvCpssGenLog.h>
#include <cpss/generic/networkIf/private/prvCpssGenNetworkIfLog.h>


/********* enums *********/

char * prvCpssLogEnum_CPSS_DXCH_NET_DSA_CMD_ENT[]  =
{
    "CPSS_DXCH_NET_DSA_CMD_TO_CPU_E",
    "CPSS_DXCH_NET_DSA_CMD_FROM_CPU_E",
    "CPSS_DXCH_NET_DSA_CMD_TO_ANALYZER_E",
    "CPSS_DXCH_NET_DSA_CMD_FORWARD_E"
};
PRV_CPSS_LOG_STC_ENUM_ARRAY_SIZE_MAC(CPSS_DXCH_NET_DSA_CMD_ENT);
char * prvCpssLogEnum_CPSS_DXCH_NET_RESOURCE_ERROR_MODE_ENT[]  =
{
    "CPSS_DXCH_NET_RESOURCE_ERROR_MODE_RETRY_E",
    "CPSS_DXCH_NET_RESOURCE_ERROR_MODE_ABORT_E"
};
PRV_CPSS_LOG_STC_ENUM_ARRAY_SIZE_MAC(CPSS_DXCH_NET_RESOURCE_ERROR_MODE_ENT);
char * prvCpssLogEnum_CPSS_DXCH_NET_TO_CPU_FLOW_ID_OR_TT_OFFSET_MODE_ENT[]  =
{
    "CPSS_DXCH_NET_TO_CPU_FLOW_ID_MODE_E",
    "CPSS_DXCH_NET_TO_CPU_TT_OFFSET_MODE_E"
};
PRV_CPSS_LOG_STC_ENUM_ARRAY_SIZE_MAC(CPSS_DXCH_NET_TO_CPU_FLOW_ID_OR_TT_OFFSET_MODE_ENT);
char * prvCpssLogEnum_CPSS_DXCH_NET_TX_GENERATOR_BURST_STATUS_ENT[]  =
{
    "CPSS_DXCH_NET_TX_GENERATOR_BURST_STATUS_IDLE_E",
    "CPSS_DXCH_NET_TX_GENERATOR_BURST_STATUS_RUN_E",
    "CPSS_DXCH_NET_TX_GENERATOR_BURST_STATUS_LIMIT_E",
    "CPSS_DXCH_NET_TX_GENERATOR_BURST_STATUS_TERMINATE_E"
};
PRV_CPSS_LOG_STC_ENUM_ARRAY_SIZE_MAC(CPSS_DXCH_NET_TX_GENERATOR_BURST_STATUS_ENT);
char * prvCpssLogEnum_CPSS_DXCH_NET_TX_GENERATOR_RATE_MODE_ENT[]  =
{
    "CPSS_DXCH_NET_TX_GENERATOR_RATE_MODE_GLOBAL_THROUGHPUT_E",
    "CPSS_DXCH_NET_TX_GENERATOR_RATE_MODE_PER_FLOW_INTERVAL_E",
    "CPSS_DXCH_NET_TX_GENERATOR_RATE_MODE_NO_GAP_E"
};
PRV_CPSS_LOG_STC_ENUM_ARRAY_SIZE_MAC(CPSS_DXCH_NET_TX_GENERATOR_RATE_MODE_ENT);
char * prvCpssLogEnum_CPSS_DXCH_NET_DSA_TYPE_ENT[]  =
{
    "CPSS_DXCH_NET_DSA_1_WORD_TYPE_ENT",
    "CPSS_DXCH_NET_DSA_2_WORD_TYPE_ENT",
    "CPSS_DXCH_NET_DSA_3_WORD_TYPE_ENT",
    "CPSS_DXCH_NET_DSA_4_WORD_TYPE_ENT"
};
PRV_CPSS_LOG_STC_ENUM_ARRAY_SIZE_MAC(CPSS_DXCH_NET_DSA_TYPE_ENT);


/********* structure fields log functions *********/

void prvCpssLogParamFuncStc_CPSS_DXCH_NETIF_MII_INIT_STC_PTR
(
    IN CPSS_LOG_LIB_ENT         contextLib,
    IN CPSS_LOG_TYPE_ENT        logType,
    IN GT_CHAR_PTR              namePtr,
    IN void                   * fieldPtr,
    INOUT PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC     * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_START_FIELD_STC_MAC(CPSS_DXCH_NETIF_MII_INIT_STC *, valPtr);
    PRV_CPSS_LOG_UNUSED_MAC(inOutParamInfoPtr);
    PRV_CPSS_LOG_STC_NUMBER_MAC(valPtr, numOfTxDesc);
    PRV_CPSS_LOG_STC_PTR_MAC(valPtr, txInternalBufBlockPtr);
    PRV_CPSS_LOG_STC_NUMBER_MAC(valPtr, txInternalBufBlockSize);
    PRV_CPSS_LOG_STC_TYPE_ARRAY_MAC(valPtr, bufferPercentage, CPSS_MAX_RX_QUEUE_CNS, GT_U32);
    PRV_CPSS_LOG_STC_NUMBER_MAC(valPtr, rxBufSize);
    PRV_CPSS_LOG_STC_NUMBER_MAC(valPtr, headerOffset);
    PRV_CPSS_LOG_STC_PTR_MAC(valPtr, rxBufBlockPtr);
    PRV_CPSS_LOG_STC_NUMBER_MAC(valPtr, rxBufBlockSize);
    prvCpssLogStcLogEnd(contextLib, logType);
}
void prvCpssLogParamFuncStc_CPSS_DXCH_NET_CPU_CODE_TABLE_ENTRY_STC_PTR
(
    IN CPSS_LOG_LIB_ENT         contextLib,
    IN CPSS_LOG_TYPE_ENT        logType,
    IN GT_CHAR_PTR              namePtr,
    IN void                   * fieldPtr,
    INOUT PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC     * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_START_FIELD_STC_MAC(CPSS_DXCH_NET_CPU_CODE_TABLE_ENTRY_STC *, valPtr);
    PRV_CPSS_LOG_UNUSED_MAC(inOutParamInfoPtr);
    PRV_CPSS_LOG_STC_NUMBER_MAC(valPtr, tc);
    PRV_CPSS_LOG_STC_ENUM_MAC(valPtr, dp, CPSS_DP_LEVEL_ENT);
    PRV_CPSS_LOG_STC_BOOL_MAC(valPtr, truncate);
    PRV_CPSS_LOG_STC_ENUM_MAC(valPtr, cpuRateLimitMode, CPSS_NET_CPU_CODE_RATE_LIMIT_MODE_ENT);
    PRV_CPSS_LOG_STC_NUMBER_MAC(valPtr, cpuCodeRateLimiterIndex);
    PRV_CPSS_LOG_STC_NUMBER_MAC(valPtr, cpuCodeStatRateLimitIndex);
    PRV_CPSS_LOG_STC_NUMBER_MAC(valPtr, designatedDevNumIndex);
    prvCpssLogStcLogEnd(contextLib, logType);
}
void prvCpssLogParamFuncStc_CPSS_DXCH_NET_RX_PARAMS_STC_PTR
(
    IN CPSS_LOG_LIB_ENT         contextLib,
    IN CPSS_LOG_TYPE_ENT        logType,
    IN GT_CHAR_PTR              namePtr,
    IN void                   * fieldPtr,
    INOUT PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC     * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_START_FIELD_STC_MAC(CPSS_DXCH_NET_RX_PARAMS_STC *, valPtr);
    PRV_CPSS_LOG_UNUSED_MAC(inOutParamInfoPtr);
    PRV_CPSS_LOG_STC_STC_MAC(valPtr, dsaParam, CPSS_DXCH_NET_DSA_PARAMS_STC);
    prvCpssLogStcLogEnd(contextLib, logType);
}
void prvCpssLogParamFuncStc_CPSS_DXCH_NET_SDMA_RX_COUNTERS_STC_PTR
(
    IN CPSS_LOG_LIB_ENT         contextLib,
    IN CPSS_LOG_TYPE_ENT        logType,
    IN GT_CHAR_PTR              namePtr,
    IN void                   * fieldPtr,
    INOUT PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC     * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_START_FIELD_STC_MAC(CPSS_DXCH_NET_SDMA_RX_COUNTERS_STC *, valPtr);
    PRV_CPSS_LOG_UNUSED_MAC(inOutParamInfoPtr);
    PRV_CPSS_LOG_STC_NUMBER_MAC(valPtr, rxInPkts);
    PRV_CPSS_LOG_STC_NUMBER_MAC(valPtr, rxInOctets);
    prvCpssLogStcLogEnd(contextLib, logType);
}
void prvCpssLogParamFuncStc_CPSS_DXCH_NET_TX_PARAMS_STC_PTR
(
    IN CPSS_LOG_LIB_ENT         contextLib,
    IN CPSS_LOG_TYPE_ENT        logType,
    IN GT_CHAR_PTR              namePtr,
    IN void                   * fieldPtr,
    INOUT PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC     * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_START_FIELD_STC_MAC(CPSS_DXCH_NET_TX_PARAMS_STC *, valPtr);
    PRV_CPSS_LOG_UNUSED_MAC(inOutParamInfoPtr);
    PRV_CPSS_LOG_STC_BOOL_MAC(valPtr, packetIsTagged);
    PRV_CPSS_LOG_STC_PTR_MAC(valPtr, cookie);
    PRV_CPSS_LOG_STC_STC_MAC(valPtr, sdmaInfo, CPSS_DXCH_NET_SDMA_TX_PARAMS_STC);
    PRV_CPSS_LOG_STC_STC_MAC(valPtr, dsaParam, CPSS_DXCH_NET_DSA_PARAMS_STC);
    prvCpssLogStcLogEnd(contextLib, logType);
}
void prvCpssLogParamFuncStc_CPSS_DXCH_NET_DSA_COMMON_STC_PTR
(
    IN CPSS_LOG_LIB_ENT         contextLib,
    IN CPSS_LOG_TYPE_ENT        logType,
    IN GT_CHAR_PTR              namePtr,
    IN void                   * fieldPtr,
    INOUT PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC     * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_START_FIELD_STC_MAC(CPSS_DXCH_NET_DSA_COMMON_STC *, valPtr);
    PRV_CPSS_LOG_UNUSED_MAC(inOutParamInfoPtr);
    PRV_CPSS_LOG_STC_ENUM_MAC(valPtr, dsaTagType, CPSS_DXCH_NET_DSA_TYPE_ENT);
    PRV_CPSS_LOG_STC_NUMBER_MAC(valPtr, vpt);
    PRV_CPSS_LOG_STC_NUMBER_MAC(valPtr, cfiBit);
    PRV_CPSS_LOG_STC_NUMBER_MAC(valPtr, vid);
    PRV_CPSS_LOG_STC_BOOL_MAC(valPtr, dropOnSource);
    PRV_CPSS_LOG_STC_BOOL_MAC(valPtr, packetIsLooped);
    prvCpssLogStcLogEnd(contextLib, logType);
}
void prvCpssLogParamFuncStc_CPSS_DXCH_NET_SDMA_TX_PARAMS_STC_PTR
(
    IN CPSS_LOG_LIB_ENT         contextLib,
    IN CPSS_LOG_TYPE_ENT        logType,
    IN GT_CHAR_PTR              namePtr,
    IN void                   * fieldPtr,
    INOUT PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC     * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_START_FIELD_STC_MAC(CPSS_DXCH_NET_SDMA_TX_PARAMS_STC *, valPtr);
    PRV_CPSS_LOG_UNUSED_MAC(inOutParamInfoPtr);
    PRV_CPSS_LOG_STC_BOOL_MAC(valPtr, recalcCrc);
    PRV_CPSS_LOG_STC_NUMBER_MAC(valPtr, txQueue);
    PRV_CPSS_LOG_STC_UINTPTR_MAC(valPtr, evReqHndl);
    PRV_CPSS_LOG_STC_BOOL_MAC(valPtr, invokeTxBufferQueueEvent);
    prvCpssLogStcLogEnd(contextLib, logType);
}


/********* parameters log functions *********/

void prvCpssLogParamFunc_CPSS_DXCH_NETIF_MII_INIT_STC_PTR(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_START_PARAM_STC_MAC(CPSS_DXCH_NETIF_MII_INIT_STC*, paramVal);
    prvCpssLogParamFuncStc_CPSS_DXCH_NETIF_MII_INIT_STC_PTR(contextLib, logType, namePtr, paramVal, inOutParamInfoPtr);
}
void prvCpssLogParamFunc_CPSS_DXCH_NET_CPU_CODE_TABLE_ENTRY_STC_PTR(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_START_PARAM_STC_MAC(CPSS_DXCH_NET_CPU_CODE_TABLE_ENTRY_STC*, paramVal);
    prvCpssLogParamFuncStc_CPSS_DXCH_NET_CPU_CODE_TABLE_ENTRY_STC_PTR(contextLib, logType, namePtr, paramVal, inOutParamInfoPtr);
}
void prvCpssLogParamFunc_CPSS_DXCH_NET_DSA_CMD_ENT(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_SET_PARAM_VAL_MAC(CPSS_DXCH_NET_DSA_CMD_ENT, paramVal);
    PRV_CPSS_LOG_ENUM_MAC(namePtr, paramVal, CPSS_DXCH_NET_DSA_CMD_ENT);
}
void prvCpssLogParamFunc_CPSS_DXCH_NET_DSA_PARAMS_STC_PTR(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_START_PARAM_STC_MAC(CPSS_DXCH_NET_DSA_PARAMS_STC*, paramVal);
    prvCpssLogParamFuncStc_CPSS_DXCH_NET_DSA_PARAMS_STC_PTR(contextLib, logType, namePtr, paramVal, inOutParamInfoPtr);
}
void prvCpssLogParamFunc_CPSS_DXCH_NET_RESOURCE_ERROR_MODE_ENT(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_SET_PARAM_VAL_MAC(CPSS_DXCH_NET_RESOURCE_ERROR_MODE_ENT, paramVal);
    PRV_CPSS_LOG_ENUM_MAC(namePtr, paramVal, CPSS_DXCH_NET_RESOURCE_ERROR_MODE_ENT);
}
void prvCpssLogParamFunc_CPSS_DXCH_NET_RESOURCE_ERROR_MODE_ENT_PTR(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_SET_PARAM_VAL_MAC(CPSS_DXCH_NET_RESOURCE_ERROR_MODE_ENT*, paramVal);
    if (paramVal == NULL)
    {
        PRV_CPSS_LOG_AND_HISTORY_PARAM_MAC(contextLib, logType, "%s = NULL\n", namePtr);
        return;
    }
    PRV_CPSS_LOG_ENUM_MAC(namePtr, *paramVal, CPSS_DXCH_NET_RESOURCE_ERROR_MODE_ENT);
}
void prvCpssLogParamFunc_CPSS_DXCH_NET_RX_PARAMS_STC_PTR(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_START_PARAM_STC_MAC(CPSS_DXCH_NET_RX_PARAMS_STC*, paramVal);
    prvCpssLogParamFuncStc_CPSS_DXCH_NET_RX_PARAMS_STC_PTR(contextLib, logType, namePtr, paramVal, inOutParamInfoPtr);
}
void prvCpssLogParamFunc_CPSS_DXCH_NET_SDMA_RX_COUNTERS_STC_PTR(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_START_PARAM_STC_MAC(CPSS_DXCH_NET_SDMA_RX_COUNTERS_STC*, paramVal);
    prvCpssLogParamFuncStc_CPSS_DXCH_NET_SDMA_RX_COUNTERS_STC_PTR(contextLib, logType, namePtr, paramVal, inOutParamInfoPtr);
}
void prvCpssLogParamFunc_CPSS_DXCH_NET_TO_CPU_FLOW_ID_OR_TT_OFFSET_MODE_ENT(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_SET_PARAM_VAL_MAC(CPSS_DXCH_NET_TO_CPU_FLOW_ID_OR_TT_OFFSET_MODE_ENT, paramVal);
    PRV_CPSS_LOG_ENUM_MAC(namePtr, paramVal, CPSS_DXCH_NET_TO_CPU_FLOW_ID_OR_TT_OFFSET_MODE_ENT);
}
void prvCpssLogParamFunc_CPSS_DXCH_NET_TO_CPU_FLOW_ID_OR_TT_OFFSET_MODE_ENT_PTR(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_SET_PARAM_VAL_MAC(CPSS_DXCH_NET_TO_CPU_FLOW_ID_OR_TT_OFFSET_MODE_ENT*, paramVal);
    if (paramVal == NULL)
    {
        PRV_CPSS_LOG_AND_HISTORY_PARAM_MAC(contextLib, logType, "%s = NULL\n", namePtr);
        return;
    }
    PRV_CPSS_LOG_ENUM_MAC(namePtr, *paramVal, CPSS_DXCH_NET_TO_CPU_FLOW_ID_OR_TT_OFFSET_MODE_ENT);
}
void prvCpssLogParamFunc_CPSS_DXCH_NET_TX_GENERATOR_BURST_STATUS_ENT_PTR(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_SET_PARAM_VAL_MAC(CPSS_DXCH_NET_TX_GENERATOR_BURST_STATUS_ENT*, paramVal);
    if (paramVal == NULL)
    {
        PRV_CPSS_LOG_AND_HISTORY_PARAM_MAC(contextLib, logType, "%s = NULL\n", namePtr);
        return;
    }
    PRV_CPSS_LOG_ENUM_MAC(namePtr, *paramVal, CPSS_DXCH_NET_TX_GENERATOR_BURST_STATUS_ENT);
}
void prvCpssLogParamFunc_CPSS_DXCH_NET_TX_GENERATOR_RATE_MODE_ENT(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_SET_PARAM_VAL_MAC(CPSS_DXCH_NET_TX_GENERATOR_RATE_MODE_ENT, paramVal);
    PRV_CPSS_LOG_ENUM_MAC(namePtr, paramVal, CPSS_DXCH_NET_TX_GENERATOR_RATE_MODE_ENT);
}
void prvCpssLogParamFunc_CPSS_DXCH_NET_TX_GENERATOR_RATE_MODE_ENT_PTR(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_SET_PARAM_VAL_MAC(CPSS_DXCH_NET_TX_GENERATOR_RATE_MODE_ENT*, paramVal);
    if (paramVal == NULL)
    {
        PRV_CPSS_LOG_AND_HISTORY_PARAM_MAC(contextLib, logType, "%s = NULL\n", namePtr);
        return;
    }
    PRV_CPSS_LOG_ENUM_MAC(namePtr, *paramVal, CPSS_DXCH_NET_TX_GENERATOR_RATE_MODE_ENT);
}
void prvCpssLogParamFunc_CPSS_DXCH_NET_TX_PARAMS_STC_PTR(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_START_PARAM_STC_MAC(CPSS_DXCH_NET_TX_PARAMS_STC*, paramVal);
    prvCpssLogParamFuncStc_CPSS_DXCH_NET_TX_PARAMS_STC_PTR(contextLib, logType, namePtr, paramVal, inOutParamInfoPtr);
}


/********* API fields DB *********/

PRV_CPSS_LOG_FUNC_PARAM_STC INOUT_GT_U32_PTR_numOfBuffPtr = {
     "numOfBuffPtr", PRV_CPSS_LOG_PARAM_INOUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(GT_U32)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_DXCH_NETIF_MII_INIT_STC_PTR_miiInitPtr = {
     "miiInitPtr", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(CPSS_DXCH_NETIF_MII_INIT_STC)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_DXCH_NET_CPU_CODE_TABLE_ENTRY_STC_PTR_entryInfoPtr = {
     "entryInfoPtr", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(CPSS_DXCH_NET_CPU_CODE_TABLE_ENTRY_STC)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_DXCH_NET_DSA_PARAMS_STC_PTR_dsaInfoPtr = {
     "dsaInfoPtr", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(CPSS_DXCH_NET_DSA_PARAMS_STC)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_DXCH_NET_RESOURCE_ERROR_MODE_ENT_mode = {
     "mode", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_MAC(CPSS_DXCH_NET_RESOURCE_ERROR_MODE_ENT)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_DXCH_NET_TO_CPU_FLOW_ID_OR_TT_OFFSET_MODE_ENT_mode = {
     "mode", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_MAC(CPSS_DXCH_NET_TO_CPU_FLOW_ID_OR_TT_OFFSET_MODE_ENT)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_DXCH_NET_TX_GENERATOR_RATE_MODE_ENT_rateMode = {
     "rateMode", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_MAC(CPSS_DXCH_NET_TX_GENERATOR_RATE_MODE_ENT)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_DXCH_NET_TX_PARAMS_STC_PTR_packetParamsPtr = {
     "packetParamsPtr", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(CPSS_DXCH_NET_TX_PARAMS_STC)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_DXCH_NET_TX_PARAMS_STC_PTR_pcktParamsPtr = {
     "pcktParamsPtr", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(CPSS_DXCH_NET_TX_PARAMS_STC)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_NET_PROT_ENT_protocol = {
     "protocol", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_MAC(CPSS_NET_PROT_ENT)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_NET_TCP_UDP_PACKET_TYPE_ENT_packetType = {
     "packetType", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_MAC(CPSS_NET_TCP_UDP_PACKET_TYPE_ENT)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_BOOL_burstEnable = {
     "burstEnable", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_MAC(GT_BOOL)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_HW_DEV_NUM_designatedHwDevNum = {
     "designatedHwDevNum", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_MAC(GT_HW_DEV_NUM)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_U16_maxDstPort = {
     "maxDstPort", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_MAC(GT_U16)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_U16_minDstPort = {
     "minDstPort", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_MAC(GT_U16)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_U32_PTR_buffLenArr = {
     "buffLenArr", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(GT_U32)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_U32_PTR_buffLenList = {
     "buffLenList", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(GT_U32)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_U32_PTR_rxBuffSizeList = {
     "rxBuffSizeList", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(GT_U32)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_U32_buffListLen = {
     "buffListLen", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_MAC(GT_U32)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_U32_burstPacketsNumber = {
     "burstPacketsNumber", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_MAC(GT_U32)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_U32_dropCntrVal = {
     "dropCntrVal", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_MAC(GT_U32)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_U32_numOfBufs = {
     "numOfBufs", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_MAC(GT_U32)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_U32_packetDataLength = {
     "packetDataLength", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_MAC(GT_U32)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_U32_packetId = {
     "packetId", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_MAC(GT_U32)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_U32_pktLimit = {
     "pktLimit", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_MAC(GT_U32)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_U32_rangeIndex = {
     "rangeIndex", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_MAC(GT_U32)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_U32_rateLimiterIndex = {
     "rateLimiterIndex", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_MAC(GT_U32)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_U32_statisticalRateLimit = {
     "statisticalRateLimit", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_MAC(GT_U32)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_U32_windowResolution = {
     "windowResolution", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_MAC(GT_U32)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_U64_rateValue = {
     "rateValue", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_MAC(GT_U64)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_U8_PTR_PTR_buffList = {
     "buffList", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_PTR_MAC(GT_U8)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_U8_PTR_PTR_packetBuffsArrPtr = {
     "packetBuffsArrPtr", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_PTR_MAC(GT_U8)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_U8_PTR_PTR_rxBuffList = {
     "rxBuffList", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_PTR_MAC(GT_U8)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_U8_PTR_dsaBytesPtr = {
     "dsaBytesPtr", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(GT_U8)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_U8_PTR_packetDataPtr = {
     "packetDataPtr", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(GT_U8)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_U8_queue = {
     "queue", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_MAC(GT_U8)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_U8_queueIdx = {
     "queueIdx", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_MAC(GT_U8)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_U8_rxQueue = {
     "rxQueue", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_MAC(GT_U8)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_U8_txQueue = {
     "txQueue", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_MAC(GT_U8)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_UINTPTR_hndl = {
     "hndl", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_MAC(GT_UINTPTR)
};
PRV_CPSS_LOG_FUNC_PARAM_STC OUT_CPSS_DXCH_NET_CPU_CODE_TABLE_ENTRY_STC_PTR_entryInfoPtr = {
     "entryInfoPtr", PRV_CPSS_LOG_PARAM_OUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(CPSS_DXCH_NET_CPU_CODE_TABLE_ENTRY_STC)
};
PRV_CPSS_LOG_FUNC_PARAM_STC OUT_CPSS_DXCH_NET_DSA_PARAMS_STC_PTR_dsaInfoPtr = {
     "dsaInfoPtr", PRV_CPSS_LOG_PARAM_OUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(CPSS_DXCH_NET_DSA_PARAMS_STC)
};
PRV_CPSS_LOG_FUNC_PARAM_STC OUT_CPSS_DXCH_NET_RESOURCE_ERROR_MODE_ENT_PTR_modePtr = {
     "modePtr", PRV_CPSS_LOG_PARAM_OUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(CPSS_DXCH_NET_RESOURCE_ERROR_MODE_ENT)
};
PRV_CPSS_LOG_FUNC_PARAM_STC OUT_CPSS_DXCH_NET_RX_PARAMS_STC_PTR_rxParamsPtr = {
     "rxParamsPtr", PRV_CPSS_LOG_PARAM_OUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(CPSS_DXCH_NET_RX_PARAMS_STC)
};
PRV_CPSS_LOG_FUNC_PARAM_STC OUT_CPSS_DXCH_NET_SDMA_RX_COUNTERS_STC_PTR_rxCountersPtr = {
     "rxCountersPtr", PRV_CPSS_LOG_PARAM_OUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(CPSS_DXCH_NET_SDMA_RX_COUNTERS_STC)
};
PRV_CPSS_LOG_FUNC_PARAM_STC OUT_CPSS_DXCH_NET_TO_CPU_FLOW_ID_OR_TT_OFFSET_MODE_ENT_PTR_modePtr = {
     "modePtr", PRV_CPSS_LOG_PARAM_OUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(CPSS_DXCH_NET_TO_CPU_FLOW_ID_OR_TT_OFFSET_MODE_ENT)
};
PRV_CPSS_LOG_FUNC_PARAM_STC OUT_CPSS_DXCH_NET_TX_GENERATOR_BURST_STATUS_ENT_PTR_burstStatusPtr = {
     "burstStatusPtr", PRV_CPSS_LOG_PARAM_OUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(CPSS_DXCH_NET_TX_GENERATOR_BURST_STATUS_ENT)
};
PRV_CPSS_LOG_FUNC_PARAM_STC OUT_CPSS_DXCH_NET_TX_GENERATOR_RATE_MODE_ENT_PTR_rateModePtr = {
     "rateModePtr", PRV_CPSS_LOG_PARAM_OUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(CPSS_DXCH_NET_TX_GENERATOR_RATE_MODE_ENT)
};
PRV_CPSS_LOG_FUNC_PARAM_STC OUT_CPSS_NET_PROT_ENT_PTR_protocolPtr = {
     "protocolPtr", PRV_CPSS_LOG_PARAM_OUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(CPSS_NET_PROT_ENT)
};
PRV_CPSS_LOG_FUNC_PARAM_STC OUT_CPSS_NET_SDMA_RX_ERROR_COUNTERS_STC_PTR_rxErrCountPtr = {
     "rxErrCountPtr", PRV_CPSS_LOG_PARAM_OUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(CPSS_NET_SDMA_RX_ERROR_COUNTERS_STC)
};
PRV_CPSS_LOG_FUNC_PARAM_STC OUT_CPSS_NET_TCP_UDP_PACKET_TYPE_ENT_PTR_packetTypePtr = {
     "packetTypePtr", PRV_CPSS_LOG_PARAM_OUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(CPSS_NET_TCP_UDP_PACKET_TYPE_ENT)
};
PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_HW_DEV_NUM_PTR_designatedHwDevNumPtr = {
     "designatedHwDevNumPtr", PRV_CPSS_LOG_PARAM_OUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(GT_HW_DEV_NUM)
};
PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_STATUS_PTR_statusPtr = {
     "statusPtr", PRV_CPSS_LOG_PARAM_OUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(GT_STATUS)
};
PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_U16_PTR_maxDstPortPtr = {
     "maxDstPortPtr", PRV_CPSS_LOG_PARAM_OUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(GT_U16)
};
PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_U16_PTR_minDstPortPtr = {
     "minDstPortPtr", PRV_CPSS_LOG_PARAM_OUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(GT_U16)
};
PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_U32_PTR_buffLenArr = {
     "buffLenArr", PRV_CPSS_LOG_PARAM_OUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(GT_U32)
};
PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_U32_PTR_dropCntrPtr = {
     "dropCntrPtr", PRV_CPSS_LOG_PARAM_OUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(GT_U32)
};
PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_U32_PTR_numberOfFreeTxDescriptorsPtr = {
     "numberOfFreeTxDescriptorsPtr", PRV_CPSS_LOG_PARAM_OUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(GT_U32)
};
PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_U32_PTR_packetCntrPtr = {
     "packetCntrPtr", PRV_CPSS_LOG_PARAM_OUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(GT_U32)
};
PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_U32_PTR_packetIdPtr = {
     "packetIdPtr", PRV_CPSS_LOG_PARAM_OUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(GT_U32)
};
PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_U32_PTR_pktLimitPtr = {
     "pktLimitPtr", PRV_CPSS_LOG_PARAM_OUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(GT_U32)
};
PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_U32_PTR_statisticalRateLimitPtr = {
     "statisticalRateLimitPtr", PRV_CPSS_LOG_PARAM_OUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(GT_U32)
};
PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_U32_PTR_windowResolutionPtr = {
     "windowResolutionPtr", PRV_CPSS_LOG_PARAM_OUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(GT_U32)
};
PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_U64_PTR_actualRateValuePtr = {
     "actualRateValuePtr", PRV_CPSS_LOG_PARAM_OUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(GT_U64)
};
PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_U64_PTR_rateValuePtr = {
     "rateValuePtr", PRV_CPSS_LOG_PARAM_OUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(GT_U64)
};
PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_U8_PTR_PTR_packetBuffsArrPtr = {
     "packetBuffsArrPtr", PRV_CPSS_LOG_PARAM_OUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_PTR_MAC(GT_U8)
};
PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_U8_PTR_devPtr = {
     "devPtr", PRV_CPSS_LOG_PARAM_OUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(GT_U8)
};
PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_U8_PTR_dsaBytesPtr = {
     "dsaBytesPtr", PRV_CPSS_LOG_PARAM_OUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(GT_U8)
};
PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_U8_PTR_protocolPtr = {
     "protocolPtr", PRV_CPSS_LOG_PARAM_OUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(GT_U8)
};
PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_U8_PTR_queuePtr = {
     "queuePtr", PRV_CPSS_LOG_PARAM_OUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(GT_U8)
};


/********* API prototypes DB *********/

PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChNetIfFromCpuDpSet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_CPSS_DP_LEVEL_ENT_dpLevel
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChNetIfMiiInit_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_CPSS_DXCH_NETIF_MII_INIT_STC_PTR_miiInitPtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChNetIfToCpuFlowIdOrTtOffsetModeSet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_CPSS_DXCH_NET_TO_CPU_FLOW_ID_OR_TT_OFFSET_MODE_ENT_mode
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChNetIfCpuCodeIpLinkLocalProtSet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_CPSS_IP_PROTOCOL_STACK_ENT_ipVer,
    &IN_GT_U8_protocol,
    &IN_CPSS_NET_RX_CPU_CODE_ENT_cpuCode
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChNetIfCpuCodeIpLinkLocalProtGet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_CPSS_IP_PROTOCOL_STACK_ENT_ipVer,
    &IN_GT_U8_protocol,
    &OUT_CPSS_NET_RX_CPU_CODE_ENT_PTR_cpuCodePtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChNetIfCpuCodeTableSet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_CPSS_NET_RX_CPU_CODE_ENT_cpuCode,
    &IN_CPSS_DXCH_NET_CPU_CODE_TABLE_ENTRY_STC_PTR_entryInfoPtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChNetIfCpuCodeTableGet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_CPSS_NET_RX_CPU_CODE_ENT_cpuCode,
    &OUT_CPSS_DXCH_NET_CPU_CODE_TABLE_ENTRY_STC_PTR_entryInfoPtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChNetIfPortGroupCpuCodeRateLimiterDropCntrSet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_PORT_GROUPS_BMP_portGroupsBmp,
    &IN_GT_U32_dropCntrVal
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChNetIfPortGroupCpuCodeRateLimiterTableSet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_PORT_GROUPS_BMP_portGroupsBmp,
    &IN_GT_U32_rateLimiterIndex,
    &IN_GT_U32_windowSize,
    &IN_GT_U32_pktLimit
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChNetIfPortGroupCpuCodeRateLimiterPacketCntrGet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_PORT_GROUPS_BMP_portGroupsBmp,
    &IN_GT_U32_rateLimiterIndex,
    &OUT_GT_U32_PTR_packetCntrPtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChNetIfPortGroupCpuCodeRateLimiterTableGet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_PORT_GROUPS_BMP_portGroupsBmp,
    &IN_GT_U32_rateLimiterIndex,
    &OUT_GT_U32_PTR_windowSizePtr,
    &OUT_GT_U32_PTR_pktLimitPtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChNetIfPortGroupCpuCodeRateLimiterWindowResolutionSet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_PORT_GROUPS_BMP_portGroupsBmp,
    &IN_GT_U32_windowResolution
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChNetIfSdmaTxGeneratorDisable_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_PORT_GROUPS_BMP_portGroupsBmp,
    &IN_GT_U8_txQueue
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChNetIfSdmaTxGeneratorRateSet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_PORT_GROUPS_BMP_portGroupsBmp,
    &IN_GT_U8_txQueue,
    &IN_CPSS_DXCH_NET_TX_GENERATOR_RATE_MODE_ENT_rateMode,
    &IN_GT_U64_rateValue,
    &OUT_GT_U64_PTR_actualRateValuePtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChNetIfSdmaTxGeneratorEnable_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_PORT_GROUPS_BMP_portGroupsBmp,
    &IN_GT_U8_txQueue,
    &IN_GT_BOOL_burstEnable,
    &IN_GT_U32_burstPacketsNumber
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChNetIfSdmaTxGeneratorPacketRemove_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_PORT_GROUPS_BMP_portGroupsBmp,
    &IN_GT_U8_txQueue,
    &IN_GT_U32_packetId
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChNetIfSdmaTxGeneratorBurstStatusGet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_PORT_GROUPS_BMP_portGroupsBmp,
    &IN_GT_U8_txQueue,
    &OUT_CPSS_DXCH_NET_TX_GENERATOR_BURST_STATUS_ENT_PTR_burstStatusPtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChNetIfSdmaTxGeneratorRateGet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_PORT_GROUPS_BMP_portGroupsBmp,
    &IN_GT_U8_txQueue,
    &OUT_CPSS_DXCH_NET_TX_GENERATOR_RATE_MODE_ENT_PTR_rateModePtr,
    &OUT_GT_U64_PTR_rateValuePtr,
    &OUT_GT_U64_PTR_actualRateValuePtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChNetIfPortGroupCpuCodeRateLimiterDropCntrGet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_PORT_GROUPS_BMP_portGroupsBmp,
    &OUT_GT_U32_PTR_dropCntrPtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChNetIfPortGroupCpuCodeRateLimiterWindowResolutionGet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_PORT_GROUPS_BMP_portGroupsBmp,
    &OUT_GT_U32_PTR_windowResolutionPtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChNetIfCpuCodeRateLimiterDropCntrSet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_U32_dropCntrVal
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChNetIfCpuCodeDesignatedDeviceTableSet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_U32_index,
    &IN_GT_HW_DEV_NUM_designatedHwDevNum
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChNetIfCpuCodeStatisticalRateLimitsTableSet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_U32_index,
    &IN_GT_U32_statisticalRateLimit
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChNetIfAppSpecificCpuCodeIpProtocolSet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_U32_index,
    &IN_GT_U8_protocol,
    &IN_CPSS_NET_RX_CPU_CODE_ENT_cpuCode
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChNetIfAppSpecificCpuCodeIpProtocolGet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_U32_index,
    &OUT_GT_BOOL_PTR_validPtr,
    &OUT_GT_U8_PTR_protocolPtr,
    &OUT_CPSS_NET_RX_CPU_CODE_ENT_PTR_cpuCodePtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChNetIfCpuCodeDesignatedDeviceTableGet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_U32_index,
    &OUT_GT_HW_DEV_NUM_PTR_designatedHwDevNumPtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChNetIfCpuCodeStatisticalRateLimitsTableGet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_U32_index,
    &OUT_GT_U32_PTR_statisticalRateLimitPtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChNetIfAppSpecificCpuCodeTcpUdpDestPortRangeInvalidate_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_U32_rangeIndex
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChNetIfAppSpecificCpuCodeTcpUdpDestPortRangeSet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_U32_rangeIndex,
    &IN_GT_U16_minDstPort,
    &IN_GT_U16_maxDstPort,
    &IN_CPSS_NET_TCP_UDP_PACKET_TYPE_ENT_packetType,
    &IN_CPSS_NET_PROT_ENT_protocol,
    &IN_CPSS_NET_RX_CPU_CODE_ENT_cpuCode
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChNetIfAppSpecificCpuCodeTcpUdpDestPortRangeGet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_U32_rangeIndex,
    &OUT_GT_BOOL_PTR_validPtr,
    &OUT_GT_U16_PTR_minDstPortPtr,
    &OUT_GT_U16_PTR_maxDstPortPtr,
    &OUT_CPSS_NET_TCP_UDP_PACKET_TYPE_ENT_PTR_packetTypePtr,
    &OUT_CPSS_NET_PROT_ENT_PTR_protocolPtr,
    &OUT_CPSS_NET_RX_CPU_CODE_ENT_PTR_cpuCodePtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChNetIfCpuCodeRateLimiterTableSet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_U32_rateLimiterIndex,
    &IN_GT_U32_windowSize,
    &IN_GT_U32_pktLimit
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChNetIfCpuCodeRateLimiterPacketCntrGet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_U32_rateLimiterIndex,
    &OUT_GT_U32_PTR_packetCntrPtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChNetIfCpuCodeRateLimiterTableGet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_U32_rateLimiterIndex,
    &OUT_GT_U32_PTR_windowSizePtr,
    &OUT_GT_U32_PTR_pktLimitPtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChNetIfCpuCodeRateLimiterWindowResolutionSet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_U32_windowResolution
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChNetIfCpuCodeIeeeReservedMcastProtSet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_U8_protocol,
    &IN_CPSS_NET_RX_CPU_CODE_ENT_cpuCode
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChNetIfCpuCodeIeeeReservedMcastProtGet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_U8_protocol,
    &OUT_CPSS_NET_RX_CPU_CODE_ENT_PTR_cpuCodePtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChNetIfSdmaRxResourceErrorModeSet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_U8_queue,
    &IN_CPSS_DXCH_NET_RESOURCE_ERROR_MODE_ENT_mode
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChNetIfSdmaRxQueueEnable_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_U8_queue,
    &IN_GT_BOOL_enable
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChNetIfSdmaRxResourceErrorModeGet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_U8_queue,
    &OUT_CPSS_DXCH_NET_RESOURCE_ERROR_MODE_ENT_PTR_modePtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChNetIfSdmaRxQueueEnableGet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_U8_queue,
    &OUT_GT_BOOL_PTR_enablePtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChNetIfSdmaRxCountersGet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_U8_queueIdx,
    &OUT_CPSS_DXCH_NET_SDMA_RX_COUNTERS_STC_PTR_rxCountersPtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChNetIfSdmaTxBufferQueueDescFreeAndCookieGet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_U8_txQueue,
    &OUT_GT_PTR_PTR_cookiePtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChNetIfSdmaTxFreeDescripotrsNumberGet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_U8_txQueue,
    &OUT_GT_U32_PTR_numberOfFreeTxDescriptorsPtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChNetIfFromCpuDpGet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &OUT_CPSS_DP_LEVEL_ENT_PTR_dpLevelPtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChNetIfToCpuFlowIdOrTtOffsetModeGet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &OUT_CPSS_DXCH_NET_TO_CPU_FLOW_ID_OR_TT_OFFSET_MODE_ENT_PTR_modePtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChNetIfSdmaRxErrorCountGet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &OUT_CPSS_NET_SDMA_RX_ERROR_COUNTERS_STC_PTR_rxErrCountPtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChNetIfCpuCodeRateLimiterDropCntrGet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &OUT_GT_U32_PTR_dropCntrPtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChNetIfCpuCodeRateLimiterWindowResolutionGet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &OUT_GT_U32_PTR_windowResolutionPtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChNetIfMiiTxBufferQueueGet_PARAMS[] =  {
    &IN_GT_UINTPTR_hndl,
    &OUT_GT_U8_PTR_devPtr,
    &OUT_GT_PTR_PTR_cookiePtr,
    &OUT_GT_U8_PTR_queuePtr,
    &OUT_GT_STATUS_PTR_statusPtr
};
extern PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChNetIfDsaTagBuild_PARAMS[];
extern PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChNetIfDsaTagParse_PARAMS[];
extern PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChNetIfMiiRxBufFree_PARAMS[];
extern PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChNetIfMiiRxPacketGet_PARAMS[];
extern PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChNetIfMiiSyncTxPacketSend_PARAMS[];
extern PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChNetIfMiiTxPacketSend_PARAMS[];
extern PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChNetIfRxBufFree_PARAMS[];
extern PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChNetIfRxBufFreeWithSize_PARAMS[];
extern PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChNetIfSdmaRxPacketGet_PARAMS[];
extern PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChNetIfSdmaSyncTxPacketSend_PARAMS[];
extern PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChNetIfSdmaTxGeneratorPacketAdd_PARAMS[];
extern PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChNetIfSdmaTxGeneratorPacketUpdate_PARAMS[];
extern PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChNetIfSdmaTxPacketSend_PARAMS[];


/********* lib API DB *********/

extern void cpssDxChNetIfSdmaTxPacketSend_preLogic(IN va_list  args, OUT PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC * paramDataPtr);
extern void cpssDxChNetIfSdmaTxPacketSend_preLogic(IN va_list  args, OUT PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC * paramDataPtr);
extern void cpssDxChNetIfSdmaRxPacketGet_preLogic(IN va_list  args, OUT PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC * paramDataPtr);
extern void cpssDxChNetIfRxBufFree_preLogic(IN va_list  args, OUT PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC * paramDataPtr);
extern void cpssDxChNetIfRxBufFree_preLogic(IN va_list  args, OUT PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC * paramDataPtr);
extern void cpssDxChNetIfRxBufFreeWithSize_preLogic(IN va_list  args, OUT PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC * paramDataPtr);
extern void cpssDxChNetIfSdmaRxPacketGet_preLogic(IN va_list  args, OUT PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC * paramDataPtr);
extern void cpssDxChNetIfSdmaTxPacketSend_preLogic(IN va_list  args, OUT PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC * paramDataPtr);
extern void cpssDxChNetIfSdmaTxPacketSend_preLogic(IN va_list  args, OUT PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC * paramDataPtr);
extern void cpssDxChNetIfSdmaTxGeneratorPacketAdd_preLogic(IN va_list  args, OUT PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC * paramDataPtr);
extern void cpssDxChNetIfSdmaTxGeneratorPacketUpdate_preLogic(IN va_list  args, OUT PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC * paramDataPtr);

static PRV_CPSS_LOG_FUNC_ENTRY_STC prvCpssDxChNetworkIfLogLibDb[] = {
    {"cpssDxChNetIfDsaTagParse", 3, cpssDxChNetIfDsaTagParse_PARAMS, NULL},
    {"cpssDxChNetIfDsaTagBuild", 3, cpssDxChNetIfDsaTagBuild_PARAMS, NULL},
    {"cpssDxChNetIfDuplicateEnableSet", 2, prvCpssLogGenDevNumEnable_PARAMS, NULL},
    {"cpssDxChNetIfDuplicateEnableGet", 2, prvCpssLogGenDevNumEnablePtr_PARAMS, NULL},
    {"cpssDxChNetIfPortDuplicateToCpuSet", 3, prvCpssLogGenDevNumPortNumEnable2_PARAMS, NULL},
    {"cpssDxChNetIfPortDuplicateToCpuGet", 3, prvCpssLogGenDevNumPortNumEnablePtr2_PARAMS, NULL},
    {"cpssDxChNetIfAppSpecificCpuCodeTcpSynSet", 2, prvCpssLogGenDevNumEnable_PARAMS, NULL},
    {"cpssDxChNetIfAppSpecificCpuCodeTcpSynGet", 2, prvCpssLogGenDevNumEnablePtr_PARAMS, NULL},
    {"cpssDxChNetIfAppSpecificCpuCodeIpProtocolSet", 4, cpssDxChNetIfAppSpecificCpuCodeIpProtocolSet_PARAMS, NULL},
    {"cpssDxChNetIfAppSpecificCpuCodeIpProtocolInvalidate", 2, prvCpssLogGenDevNumIndex_PARAMS, NULL},
    {"cpssDxChNetIfAppSpecificCpuCodeIpProtocolGet", 5, cpssDxChNetIfAppSpecificCpuCodeIpProtocolGet_PARAMS, NULL},
    {"cpssDxChNetIfAppSpecificCpuCodeTcpUdpDestPortRangeSet", 7, cpssDxChNetIfAppSpecificCpuCodeTcpUdpDestPortRangeSet_PARAMS, NULL},
    {"cpssDxChNetIfAppSpecificCpuCodeTcpUdpDestPortRangeInvalidate", 2, cpssDxChNetIfAppSpecificCpuCodeTcpUdpDestPortRangeInvalidate_PARAMS, NULL},
    {"cpssDxChNetIfAppSpecificCpuCodeTcpUdpDestPortRangeGet", 8, cpssDxChNetIfAppSpecificCpuCodeTcpUdpDestPortRangeGet_PARAMS, NULL},
    {"cpssDxChNetIfCpuCodeIpLinkLocalProtSet", 4, cpssDxChNetIfCpuCodeIpLinkLocalProtSet_PARAMS, NULL},
    {"cpssDxChNetIfCpuCodeIpLinkLocalProtGet", 4, cpssDxChNetIfCpuCodeIpLinkLocalProtGet_PARAMS, NULL},
    {"cpssDxChNetIfCpuCodeIeeeReservedMcastProtSet", 3, cpssDxChNetIfCpuCodeIeeeReservedMcastProtSet_PARAMS, NULL},
    {"cpssDxChNetIfCpuCodeIeeeReservedMcastProtGet", 3, cpssDxChNetIfCpuCodeIeeeReservedMcastProtGet_PARAMS, NULL},
    {"cpssDxChNetIfCpuCodeTableSet", 3, cpssDxChNetIfCpuCodeTableSet_PARAMS, NULL},
    {"cpssDxChNetIfCpuCodeTableGet", 3, cpssDxChNetIfCpuCodeTableGet_PARAMS, NULL},
    {"cpssDxChNetIfCpuCodeStatisticalRateLimitsTableSet", 3, cpssDxChNetIfCpuCodeStatisticalRateLimitsTableSet_PARAMS, NULL},
    {"cpssDxChNetIfCpuCodeStatisticalRateLimitsTableGet", 3, cpssDxChNetIfCpuCodeStatisticalRateLimitsTableGet_PARAMS, NULL},
    {"cpssDxChNetIfCpuCodeDesignatedDeviceTableSet", 3, cpssDxChNetIfCpuCodeDesignatedDeviceTableSet_PARAMS, NULL},
    {"cpssDxChNetIfCpuCodeDesignatedDeviceTableGet", 3, cpssDxChNetIfCpuCodeDesignatedDeviceTableGet_PARAMS, NULL},
    {"cpssDxChNetIfCpuCodeRateLimiterTableSet", 4, cpssDxChNetIfCpuCodeRateLimiterTableSet_PARAMS, NULL},
    {"cpssDxChNetIfCpuCodeRateLimiterTableGet", 4, cpssDxChNetIfCpuCodeRateLimiterTableGet_PARAMS, NULL},
    {"cpssDxChNetIfCpuCodeRateLimiterWindowResolutionSet", 2, cpssDxChNetIfCpuCodeRateLimiterWindowResolutionSet_PARAMS, NULL},
    {"cpssDxChNetIfCpuCodeRateLimiterWindowResolutionGet", 2, cpssDxChNetIfCpuCodeRateLimiterWindowResolutionGet_PARAMS, NULL},
    {"cpssDxChNetIfCpuCodeRateLimiterPacketCntrGet", 3, cpssDxChNetIfCpuCodeRateLimiterPacketCntrGet_PARAMS, NULL},
    {"cpssDxChNetIfCpuCodeRateLimiterDropCntrGet", 2, cpssDxChNetIfCpuCodeRateLimiterDropCntrGet_PARAMS, NULL},
    {"cpssDxChNetIfCpuCodeRateLimiterDropCntrSet", 2, cpssDxChNetIfCpuCodeRateLimiterDropCntrSet_PARAMS, NULL},
    {"cpssDxChNetIfFromCpuDpSet", 2, cpssDxChNetIfFromCpuDpSet_PARAMS, NULL},
    {"cpssDxChNetIfFromCpuDpGet", 2, cpssDxChNetIfFromCpuDpGet_PARAMS, NULL},
    {"cpssDxChNetIfSdmaRxResourceErrorModeSet", 3, cpssDxChNetIfSdmaRxResourceErrorModeSet_PARAMS, NULL},
    {"cpssDxChNetIfSdmaRxResourceErrorModeGet", 3, cpssDxChNetIfSdmaRxResourceErrorModeGet_PARAMS, NULL},
    {"cpssDxChNetIfPortGroupCpuCodeRateLimiterTableSet", 5, cpssDxChNetIfPortGroupCpuCodeRateLimiterTableSet_PARAMS, NULL},
    {"cpssDxChNetIfPortGroupCpuCodeRateLimiterTableGet", 5, cpssDxChNetIfPortGroupCpuCodeRateLimiterTableGet_PARAMS, NULL},
    {"cpssDxChNetIfPortGroupCpuCodeRateLimiterWindowResolutionSet", 3, cpssDxChNetIfPortGroupCpuCodeRateLimiterWindowResolutionSet_PARAMS, NULL},
    {"cpssDxChNetIfPortGroupCpuCodeRateLimiterWindowResolutionGet", 3, cpssDxChNetIfPortGroupCpuCodeRateLimiterWindowResolutionGet_PARAMS, NULL},
    {"cpssDxChNetIfPortGroupCpuCodeRateLimiterPacketCntrGet", 4, cpssDxChNetIfPortGroupCpuCodeRateLimiterPacketCntrGet_PARAMS, NULL},
    {"cpssDxChNetIfPortGroupCpuCodeRateLimiterDropCntrGet", 3, cpssDxChNetIfPortGroupCpuCodeRateLimiterDropCntrGet_PARAMS, NULL},
    {"cpssDxChNetIfPortGroupCpuCodeRateLimiterDropCntrSet", 3, cpssDxChNetIfPortGroupCpuCodeRateLimiterDropCntrSet_PARAMS, NULL},
    {"cpssDxChNetIfToCpuFlowIdOrTtOffsetModeSet", 2, cpssDxChNetIfToCpuFlowIdOrTtOffsetModeSet_PARAMS, NULL},
    {"cpssDxChNetIfToCpuFlowIdOrTtOffsetModeGet", 2, cpssDxChNetIfToCpuFlowIdOrTtOffsetModeGet_PARAMS, NULL},
    {"cpssDxChNetIfInit", 1, prvCpssLogGenDevNum_PARAMS, NULL},
    {"cpssDxChNetIfRemove", 1, prvCpssLogGenDevNum_PARAMS, NULL},
    {"cpssDxChNetIfMiiTxPacketSend", 5, cpssDxChNetIfMiiTxPacketSend_PARAMS, cpssDxChNetIfSdmaTxPacketSend_preLogic},
    {"cpssDxChNetIfMiiSyncTxPacketSend", 5, cpssDxChNetIfMiiSyncTxPacketSend_PARAMS, cpssDxChNetIfSdmaTxPacketSend_preLogic},
    {"cpssDxChNetIfMiiTxBufferQueueGet", 5, cpssDxChNetIfMiiTxBufferQueueGet_PARAMS, NULL},
    {"cpssDxChNetIfMiiRxPacketGet", 6, cpssDxChNetIfMiiRxPacketGet_PARAMS, cpssDxChNetIfSdmaRxPacketGet_preLogic},
    {"cpssDxChNetIfMiiRxBufFree", 4, cpssDxChNetIfMiiRxBufFree_PARAMS, cpssDxChNetIfRxBufFree_preLogic},
    {"cpssDxChNetIfMiiInit", 2, cpssDxChNetIfMiiInit_PARAMS, NULL},
    {"cpssDxChNetIfRxBufFree", 4, cpssDxChNetIfRxBufFree_PARAMS, cpssDxChNetIfRxBufFree_preLogic},
    {"cpssDxChNetIfRxBufFreeWithSize", 5, cpssDxChNetIfRxBufFreeWithSize_PARAMS, cpssDxChNetIfRxBufFreeWithSize_preLogic},
    {"cpssDxChNetIfSdmaRxPacketGet", 6, cpssDxChNetIfSdmaRxPacketGet_PARAMS, cpssDxChNetIfSdmaRxPacketGet_preLogic},
    {"cpssDxChNetIfPrePendTwoBytesHeaderSet", 2, prvCpssLogGenDevNumEnable_PARAMS, NULL},
    {"cpssDxChNetIfPrePendTwoBytesHeaderGet", 2, prvCpssLogGenDevNumEnablePtr_PARAMS, NULL},
    {"cpssDxChNetIfSdmaRxCountersGet", 3, cpssDxChNetIfSdmaRxCountersGet_PARAMS, NULL},
    {"cpssDxChNetIfSdmaRxErrorCountGet", 2, cpssDxChNetIfSdmaRxErrorCountGet_PARAMS, NULL},
    {"cpssDxChNetIfSdmaRxQueueEnable", 3, cpssDxChNetIfSdmaRxQueueEnable_PARAMS, NULL},
    {"cpssDxChNetIfSdmaRxQueueEnableGet", 3, cpssDxChNetIfSdmaRxQueueEnableGet_PARAMS, NULL},
    {"cpssDxChNetIfSdmaTxPacketSend", 5, cpssDxChNetIfSdmaTxPacketSend_PARAMS, cpssDxChNetIfSdmaTxPacketSend_preLogic},
    {"cpssDxChNetIfSdmaSyncTxPacketSend", 5, cpssDxChNetIfSdmaSyncTxPacketSend_PARAMS, cpssDxChNetIfSdmaTxPacketSend_preLogic},
    {"cpssDxChNetIfSdmaTxQueueEnable", 3, cpssDxChNetIfSdmaRxQueueEnable_PARAMS, NULL},
    {"cpssDxChNetIfSdmaTxQueueEnableGet", 3, cpssDxChNetIfSdmaRxQueueEnableGet_PARAMS, NULL},
    {"cpssDxChNetIfTxBufferQueueGet", 5, cpssDxChNetIfMiiTxBufferQueueGet_PARAMS, NULL},
    {"cpssDxChNetIfSdmaTxBufferQueueDescFreeAndCookieGet", 3, cpssDxChNetIfSdmaTxBufferQueueDescFreeAndCookieGet_PARAMS, NULL},
    {"cpssDxChNetIfSdmaTxFreeDescripotrsNumberGet", 3, cpssDxChNetIfSdmaTxFreeDescripotrsNumberGet_PARAMS, NULL},
    {"cpssDxChNetIfSdmaTxGeneratorPacketAdd", 6, cpssDxChNetIfSdmaTxGeneratorPacketAdd_PARAMS, cpssDxChNetIfSdmaTxGeneratorPacketAdd_preLogic},
    {"cpssDxChNetIfSdmaTxGeneratorPacketUpdate", 6, cpssDxChNetIfSdmaTxGeneratorPacketUpdate_PARAMS, cpssDxChNetIfSdmaTxGeneratorPacketUpdate_preLogic},
    {"cpssDxChNetIfSdmaTxGeneratorPacketRemove", 4, cpssDxChNetIfSdmaTxGeneratorPacketRemove_PARAMS, NULL},
    {"cpssDxChNetIfSdmaTxGeneratorRateSet", 6, cpssDxChNetIfSdmaTxGeneratorRateSet_PARAMS, NULL},
    {"cpssDxChNetIfSdmaTxGeneratorRateGet", 6, cpssDxChNetIfSdmaTxGeneratorRateGet_PARAMS, NULL},
    {"cpssDxChNetIfSdmaTxGeneratorEnable", 5, cpssDxChNetIfSdmaTxGeneratorEnable_PARAMS, NULL},
    {"cpssDxChNetIfSdmaTxGeneratorDisable", 3, cpssDxChNetIfSdmaTxGeneratorDisable_PARAMS, NULL},
    {"cpssDxChNetIfSdmaTxGeneratorBurstStatusGet", 4, cpssDxChNetIfSdmaTxGeneratorBurstStatusGet_PARAMS, NULL},
};

/******** DB Access Function ********/
void prvCpssLogParamLibDbGet_CPSS_LOG_LIB_NETWORK_IF(
    OUT PRV_CPSS_LOG_FUNC_ENTRY_STC ** logFuncDbPtrPtr,
    OUT GT_U32 * logFuncDbSizePtr
)
{
    *logFuncDbPtrPtr = prvCpssDxChNetworkIfLogLibDb;
    *logFuncDbSizePtr = sizeof(prvCpssDxChNetworkIfLogLibDb) / sizeof(PRV_CPSS_LOG_FUNC_ENTRY_STC);
}

