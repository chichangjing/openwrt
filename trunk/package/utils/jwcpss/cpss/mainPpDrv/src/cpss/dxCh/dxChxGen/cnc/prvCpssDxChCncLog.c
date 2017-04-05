/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* prvCpssDxChCncLog.c
*       WARNING!!! this is a generated file, please don't edit it manually
* COMMENTS:
*
* FILE REVISION NUMBER:
*       $Revision: 1 $
*******************************************************************************/
#include <cpss/generic/log/cpssLog.h>
#include <cpss/generic/log/prvCpssLog.h>
#include <cpss/extServices/private/prvCpssBindFunc.h>
#include <cpss/dxCh/dxChxGen/cnc/cpssDxChCnc.h>
#include <cpss/dxCh/dxChxGen/cnc/private/prvCpssDxChCncLog.h>
#include <cpss/generic/log/prvCpssGenCommonTypesLog.h>
#include <cpss/generic/log/prvCpssGenDbLog.h>


/********* enums *********/

char * prvCpssLogEnum_CPSS_DXCH_CNC_BYTE_COUNT_MODE_ENT[]  =
{
    "CPSS_DXCH_CNC_BYTE_COUNT_MODE_L2_E",
    "CPSS_DXCH_CNC_BYTE_COUNT_MODE_L3_E"
};
PRV_CPSS_LOG_STC_ENUM_ARRAY_SIZE_MAC(CPSS_DXCH_CNC_BYTE_COUNT_MODE_ENT);
char * prvCpssLogEnum_CPSS_DXCH_CNC_CLIENT_ENT[]  =
{
    "CPSS_DXCH_CNC_CLIENT_L2L3_INGRESS_VLAN_E",
    "CPSS_DXCH_CNC_CLIENT_INGRESS_PCL_LOOKUP_0_E",
    "CPSS_DXCH_CNC_CLIENT_INGRESS_PCL_LOOKUP_0_1_E",
    "CPSS_DXCH_CNC_CLIENT_INGRESS_PCL_LOOKUP_1_E",
    "CPSS_DXCH_CNC_CLIENT_INGRESS_VLAN_PASS_DROP_E",
    "CPSS_DXCH_CNC_CLIENT_EGRESS_VLAN_PASS_DROP_E",
    "CPSS_DXCH_CNC_CLIENT_EGRESS_QUEUE_PASS_DROP_E",
    "CPSS_DXCH_CNC_CLIENT_EGRESS_PCL_E",
    "CPSS_DXCH_CNC_CLIENT_ARP_TABLE_ACCESS_E",
    "CPSS_DXCH_CNC_CLIENT_TUNNEL_START_E",
    "CPSS_DXCH_CNC_CLIENT_TTI_E",
    "CPSS_DXCH_CNC_CLIENT_INGRESS_SRC_EPORT_E",
    "CPSS_DXCH_CNC_CLIENT_EGRESS_TRG_EPORT_E",
    "CPSS_DXCH_CNC_CLIENT_TTI_PARALLEL_0_E",
    "CPSS_DXCH_CNC_CLIENT_TTI_PARALLEL_1_E",
    "CPSS_DXCH_CNC_CLIENT_INGRESS_PCL_LOOKUP_0_PARALLEL_0_E",
    "CPSS_DXCH_CNC_CLIENT_INGRESS_PCL_LOOKUP_0_PARALLEL_1_E",
    "CPSS_DXCH_CNC_CLIENT_INGRESS_PCL_LOOKUP_0_PARALLEL_2_E",
    "CPSS_DXCH_CNC_CLIENT_INGRESS_PCL_LOOKUP_0_PARALLEL_3_E",
    "CPSS_DXCH_CNC_CLIENT_INGRESS_PCL_LOOKUP_1_PARALLEL_0_E",
    "CPSS_DXCH_CNC_CLIENT_INGRESS_PCL_LOOKUP_1_PARALLEL_1_E",
    "CPSS_DXCH_CNC_CLIENT_INGRESS_PCL_LOOKUP_1_PARALLEL_2_E",
    "CPSS_DXCH_CNC_CLIENT_INGRESS_PCL_LOOKUP_1_PARALLEL_3_E",
    "CPSS_DXCH_CNC_CLIENT_INGRESS_PCL_LOOKUP_2_PARALLEL_0_E",
    "CPSS_DXCH_CNC_CLIENT_INGRESS_PCL_LOOKUP_2_PARALLEL_1_E",
    "CPSS_DXCH_CNC_CLIENT_INGRESS_PCL_LOOKUP_2_PARALLEL_2_E",
    "CPSS_DXCH_CNC_CLIENT_INGRESS_PCL_LOOKUP_2_PARALLEL_3_E",
    "CPSS_DXCH_CNC_CLIENT_EGRESS_PCL_PARALLEL_0_E",
    "CPSS_DXCH_CNC_CLIENT_EGRESS_PCL_PARALLEL_1_E",
    "CPSS_DXCH_CNC_CLIENT_EGRESS_PCL_PARALLEL_2_E",
    "CPSS_DXCH_CNC_CLIENT_EGRESS_PCL_PARALLEL_3_E",
    "CPSS_DXCH_CNC_CLIENT_PACKET_TYPE_PASS_DROP_E",
    "CPSS_DXCH_CNC_CLIENT_TM_PASS_DROP_E",
    "CPSS_DXCH_CNC_CLIENT_TTI_PARALLEL_2_E",
    "CPSS_DXCH_CNC_CLIENT_TTI_PARALLEL_3_E",
    "CPSS_DXCH_CNC_CLIENT_LAST_E"
};
PRV_CPSS_LOG_STC_ENUM_ARRAY_SIZE_MAC(CPSS_DXCH_CNC_CLIENT_ENT);
char * prvCpssLogEnum_CPSS_DXCH_CNC_COUNTER_FORMAT_ENT[]  =
{
    "CPSS_DXCH_CNC_COUNTER_FORMAT_MODE_0_E",
    "CPSS_DXCH_CNC_COUNTER_FORMAT_MODE_1_E",
    "CPSS_DXCH_CNC_COUNTER_FORMAT_MODE_2_E"
};
PRV_CPSS_LOG_STC_ENUM_ARRAY_SIZE_MAC(CPSS_DXCH_CNC_COUNTER_FORMAT_ENT);
char * prvCpssLogEnum_CPSS_DXCH_CNC_COUNTING_ENABLE_UNIT_ENT[]  =
{
    "CPSS_DXCH_CNC_COUNTING_ENABLE_UNIT_TTI_E",
    "CPSS_DXCH_CNC_COUNTING_ENABLE_UNIT_PCL_E"
};
PRV_CPSS_LOG_STC_ENUM_ARRAY_SIZE_MAC(CPSS_DXCH_CNC_COUNTING_ENABLE_UNIT_ENT);
char * prvCpssLogEnum_CPSS_DXCH_CNC_EGRESS_DROP_COUNT_MODE_ENT[]  =
{
    "CPSS_DXCH_CNC_EGRESS_DROP_COUNT_MODE_EGRESS_FILTER_AND_TAIL_DROP_E",
    "CPSS_DXCH_CNC_EGRESS_DROP_COUNT_MODE_EGRESS_FILTER_ONLY_E",
    "CPSS_DXCH_CNC_EGRESS_DROP_COUNT_MODE_TAIL_DROP_ONLY_E"
};
PRV_CPSS_LOG_STC_ENUM_ARRAY_SIZE_MAC(CPSS_DXCH_CNC_EGRESS_DROP_COUNT_MODE_ENT);
char * prvCpssLogEnum_CPSS_DXCH_CNC_EGRESS_QUEUE_CLIENT_MODE_ENT[]  =
{
    "CPSS_DXCH_CNC_EGRESS_QUEUE_CLIENT_MODE_TAIL_DROP_E",
    "CPSS_DXCH_CNC_EGRESS_QUEUE_CLIENT_MODE_CN_E"
};
PRV_CPSS_LOG_STC_ENUM_ARRAY_SIZE_MAC(CPSS_DXCH_CNC_EGRESS_QUEUE_CLIENT_MODE_ENT);
char * prvCpssLogEnum_CPSS_DXCH_CNC_PACKET_TYPE_PASS_DROP_TO_CPU_MODE_ENT[]  =
{
    "CPSS_DXCH_CNC_PACKET_TYPE_PASS_DROP_TO_CPU_MODE_SRC_PORT_E",
    "CPSS_DXCH_CNC_PACKET_TYPE_PASS_DROP_TO_CPU_MODE_CPU_CODE_E"
};
PRV_CPSS_LOG_STC_ENUM_ARRAY_SIZE_MAC(CPSS_DXCH_CNC_PACKET_TYPE_PASS_DROP_TO_CPU_MODE_ENT);
char * prvCpssLogEnum_CPSS_DXCH_CNC_TM_INDEX_MODE_ENT[]  =
{
    "CPSS_DXCH_CNC_TM_INDEX_MODE_0_E",
    "CPSS_DXCH_CNC_TM_INDEX_MODE_1_E",
    "CPSS_DXCH_CNC_TM_INDEX_MODE_2_E",
    "CPSS_DXCH_CNC_TM_INDEX_MODE_3_E"
};
PRV_CPSS_LOG_STC_ENUM_ARRAY_SIZE_MAC(CPSS_DXCH_CNC_TM_INDEX_MODE_ENT);
char * prvCpssLogEnum_CPSS_DXCH_CNC_VLAN_INDEX_MODE_ENT[]  =
{
    "CPSS_DXCH_CNC_VLAN_INDEX_MODE_ORIGINAL_VID_E",
    "CPSS_DXCH_CNC_VLAN_INDEX_MODE_EVID_E",
    "CPSS_DXCH_CNC_VLAN_INDEX_MODE_TAG1_VID_E"
};
PRV_CPSS_LOG_STC_ENUM_ARRAY_SIZE_MAC(CPSS_DXCH_CNC_VLAN_INDEX_MODE_ENT);


/********* structure fields log functions *********/

void prvCpssLogParamFuncStc_CPSS_DXCH_CNC_COUNTER_STC_PTR
(
    IN CPSS_LOG_LIB_ENT         contextLib,
    IN CPSS_LOG_TYPE_ENT        logType,
    IN GT_CHAR_PTR              namePtr,
    IN void                   * fieldPtr,
    INOUT PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC     * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_START_FIELD_STC_MAC(CPSS_DXCH_CNC_COUNTER_STC *, valPtr);
    PRV_CPSS_LOG_UNUSED_MAC(inOutParamInfoPtr);
    PRV_CPSS_LOG_STC_STC_MAC(valPtr, byteCount, GT_U64);
    PRV_CPSS_LOG_STC_STC_MAC(valPtr, packetCount, GT_U64);
    prvCpssLogStcLogEnd(contextLib, logType);
}


/********* parameters log functions *********/

void prvCpssLogParamFunc_CPSS_DXCH_CNC_BYTE_COUNT_MODE_ENT(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_SET_PARAM_VAL_MAC(CPSS_DXCH_CNC_BYTE_COUNT_MODE_ENT, paramVal);
    PRV_CPSS_LOG_ENUM_MAC(namePtr, paramVal, CPSS_DXCH_CNC_BYTE_COUNT_MODE_ENT);
}
void prvCpssLogParamFunc_CPSS_DXCH_CNC_BYTE_COUNT_MODE_ENT_PTR(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_SET_PARAM_VAL_MAC(CPSS_DXCH_CNC_BYTE_COUNT_MODE_ENT*, paramVal);
    if (paramVal == NULL)
    {
        PRV_CPSS_LOG_AND_HISTORY_PARAM_MAC(contextLib, logType, "%s = NULL\n", namePtr);
        return;
    }
    PRV_CPSS_LOG_ENUM_MAC(namePtr, *paramVal, CPSS_DXCH_CNC_BYTE_COUNT_MODE_ENT);
}
void prvCpssLogParamFunc_CPSS_DXCH_CNC_CLIENT_ENT(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_SET_PARAM_VAL_MAC(CPSS_DXCH_CNC_CLIENT_ENT, paramVal);
    PRV_CPSS_LOG_ENUM_MAC(namePtr, paramVal, CPSS_DXCH_CNC_CLIENT_ENT);
}
void prvCpssLogParamFunc_CPSS_DXCH_CNC_COUNTER_FORMAT_ENT(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_SET_PARAM_VAL_MAC(CPSS_DXCH_CNC_COUNTER_FORMAT_ENT, paramVal);
    PRV_CPSS_LOG_ENUM_MAC(namePtr, paramVal, CPSS_DXCH_CNC_COUNTER_FORMAT_ENT);
}
void prvCpssLogParamFunc_CPSS_DXCH_CNC_COUNTER_FORMAT_ENT_PTR(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_SET_PARAM_VAL_MAC(CPSS_DXCH_CNC_COUNTER_FORMAT_ENT*, paramVal);
    if (paramVal == NULL)
    {
        PRV_CPSS_LOG_AND_HISTORY_PARAM_MAC(contextLib, logType, "%s = NULL\n", namePtr);
        return;
    }
    PRV_CPSS_LOG_ENUM_MAC(namePtr, *paramVal, CPSS_DXCH_CNC_COUNTER_FORMAT_ENT);
}
void prvCpssLogParamFunc_CPSS_DXCH_CNC_COUNTING_ENABLE_UNIT_ENT(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_SET_PARAM_VAL_MAC(CPSS_DXCH_CNC_COUNTING_ENABLE_UNIT_ENT, paramVal);
    PRV_CPSS_LOG_ENUM_MAC(namePtr, paramVal, CPSS_DXCH_CNC_COUNTING_ENABLE_UNIT_ENT);
}
void prvCpssLogParamFunc_CPSS_DXCH_CNC_EGRESS_DROP_COUNT_MODE_ENT(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_SET_PARAM_VAL_MAC(CPSS_DXCH_CNC_EGRESS_DROP_COUNT_MODE_ENT, paramVal);
    PRV_CPSS_LOG_ENUM_MAC(namePtr, paramVal, CPSS_DXCH_CNC_EGRESS_DROP_COUNT_MODE_ENT);
}
void prvCpssLogParamFunc_CPSS_DXCH_CNC_EGRESS_DROP_COUNT_MODE_ENT_PTR(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_SET_PARAM_VAL_MAC(CPSS_DXCH_CNC_EGRESS_DROP_COUNT_MODE_ENT*, paramVal);
    if (paramVal == NULL)
    {
        PRV_CPSS_LOG_AND_HISTORY_PARAM_MAC(contextLib, logType, "%s = NULL\n", namePtr);
        return;
    }
    PRV_CPSS_LOG_ENUM_MAC(namePtr, *paramVal, CPSS_DXCH_CNC_EGRESS_DROP_COUNT_MODE_ENT);
}
void prvCpssLogParamFunc_CPSS_DXCH_CNC_EGRESS_QUEUE_CLIENT_MODE_ENT(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_SET_PARAM_VAL_MAC(CPSS_DXCH_CNC_EGRESS_QUEUE_CLIENT_MODE_ENT, paramVal);
    PRV_CPSS_LOG_ENUM_MAC(namePtr, paramVal, CPSS_DXCH_CNC_EGRESS_QUEUE_CLIENT_MODE_ENT);
}
void prvCpssLogParamFunc_CPSS_DXCH_CNC_EGRESS_QUEUE_CLIENT_MODE_ENT_PTR(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_SET_PARAM_VAL_MAC(CPSS_DXCH_CNC_EGRESS_QUEUE_CLIENT_MODE_ENT*, paramVal);
    if (paramVal == NULL)
    {
        PRV_CPSS_LOG_AND_HISTORY_PARAM_MAC(contextLib, logType, "%s = NULL\n", namePtr);
        return;
    }
    PRV_CPSS_LOG_ENUM_MAC(namePtr, *paramVal, CPSS_DXCH_CNC_EGRESS_QUEUE_CLIENT_MODE_ENT);
}
void prvCpssLogParamFunc_CPSS_DXCH_CNC_PACKET_TYPE_PASS_DROP_TO_CPU_MODE_ENT(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_SET_PARAM_VAL_MAC(CPSS_DXCH_CNC_PACKET_TYPE_PASS_DROP_TO_CPU_MODE_ENT, paramVal);
    PRV_CPSS_LOG_ENUM_MAC(namePtr, paramVal, CPSS_DXCH_CNC_PACKET_TYPE_PASS_DROP_TO_CPU_MODE_ENT);
}
void prvCpssLogParamFunc_CPSS_DXCH_CNC_PACKET_TYPE_PASS_DROP_TO_CPU_MODE_ENT_PTR(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_SET_PARAM_VAL_MAC(CPSS_DXCH_CNC_PACKET_TYPE_PASS_DROP_TO_CPU_MODE_ENT*, paramVal);
    if (paramVal == NULL)
    {
        PRV_CPSS_LOG_AND_HISTORY_PARAM_MAC(contextLib, logType, "%s = NULL\n", namePtr);
        return;
    }
    PRV_CPSS_LOG_ENUM_MAC(namePtr, *paramVal, CPSS_DXCH_CNC_PACKET_TYPE_PASS_DROP_TO_CPU_MODE_ENT);
}
void prvCpssLogParamFunc_CPSS_DXCH_CNC_TM_INDEX_MODE_ENT(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_SET_PARAM_VAL_MAC(CPSS_DXCH_CNC_TM_INDEX_MODE_ENT, paramVal);
    PRV_CPSS_LOG_ENUM_MAC(namePtr, paramVal, CPSS_DXCH_CNC_TM_INDEX_MODE_ENT);
}
void prvCpssLogParamFunc_CPSS_DXCH_CNC_TM_INDEX_MODE_ENT_PTR(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_SET_PARAM_VAL_MAC(CPSS_DXCH_CNC_TM_INDEX_MODE_ENT*, paramVal);
    if (paramVal == NULL)
    {
        PRV_CPSS_LOG_AND_HISTORY_PARAM_MAC(contextLib, logType, "%s = NULL\n", namePtr);
        return;
    }
    PRV_CPSS_LOG_ENUM_MAC(namePtr, *paramVal, CPSS_DXCH_CNC_TM_INDEX_MODE_ENT);
}
void prvCpssLogParamFunc_CPSS_DXCH_CNC_VLAN_INDEX_MODE_ENT(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_SET_PARAM_VAL_MAC(CPSS_DXCH_CNC_VLAN_INDEX_MODE_ENT, paramVal);
    PRV_CPSS_LOG_ENUM_MAC(namePtr, paramVal, CPSS_DXCH_CNC_VLAN_INDEX_MODE_ENT);
}
void prvCpssLogParamFunc_CPSS_DXCH_CNC_VLAN_INDEX_MODE_ENT_PTR(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_SET_PARAM_VAL_MAC(CPSS_DXCH_CNC_VLAN_INDEX_MODE_ENT*, paramVal);
    if (paramVal == NULL)
    {
        PRV_CPSS_LOG_AND_HISTORY_PARAM_MAC(contextLib, logType, "%s = NULL\n", namePtr);
        return;
    }
    PRV_CPSS_LOG_ENUM_MAC(namePtr, *paramVal, CPSS_DXCH_CNC_VLAN_INDEX_MODE_ENT);
}


/********* API fields DB *********/

PRV_CPSS_LOG_FUNC_PARAM_STC INOUT_GT_U32_PTR_indexNumPtr = {
     "indexNumPtr", PRV_CPSS_LOG_PARAM_INOUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(GT_U32)
};
PRV_CPSS_LOG_FUNC_PARAM_STC INOUT_GT_U32_PTR_numOfCounterValuesPtr = {
     "numOfCounterValuesPtr", PRV_CPSS_LOG_PARAM_INOUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(GT_U32)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_DXCH_CNC_BYTE_COUNT_MODE_ENT_countMode = {
     "countMode", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_MAC(CPSS_DXCH_CNC_BYTE_COUNT_MODE_ENT)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_DXCH_CNC_CLIENT_ENT_client = {
     "client", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_MAC(CPSS_DXCH_CNC_CLIENT_ENT)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_DXCH_CNC_CLIENT_ENT_vlanClient = {
     "vlanClient", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_MAC(CPSS_DXCH_CNC_CLIENT_ENT)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_DXCH_CNC_COUNTER_FORMAT_ENT_format = {
     "format", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_MAC(CPSS_DXCH_CNC_COUNTER_FORMAT_ENT)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_DXCH_CNC_COUNTER_STC_PTR_counterPtr = {
     "counterPtr", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(CPSS_DXCH_CNC_COUNTER_STC)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_DXCH_CNC_COUNTING_ENABLE_UNIT_ENT_cncUnit = {
     "cncUnit", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_MAC(CPSS_DXCH_CNC_COUNTING_ENABLE_UNIT_ENT)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_DXCH_CNC_EGRESS_DROP_COUNT_MODE_ENT_mode = {
     "mode", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_MAC(CPSS_DXCH_CNC_EGRESS_DROP_COUNT_MODE_ENT)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_DXCH_CNC_EGRESS_QUEUE_CLIENT_MODE_ENT_mode = {
     "mode", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_MAC(CPSS_DXCH_CNC_EGRESS_QUEUE_CLIENT_MODE_ENT)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_DXCH_CNC_PACKET_TYPE_PASS_DROP_TO_CPU_MODE_ENT_toCpuMode = {
     "toCpuMode", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_MAC(CPSS_DXCH_CNC_PACKET_TYPE_PASS_DROP_TO_CPU_MODE_ENT)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_DXCH_CNC_TM_INDEX_MODE_ENT_indexMode = {
     "indexMode", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_MAC(CPSS_DXCH_CNC_TM_INDEX_MODE_ENT)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_DXCH_CNC_VLAN_INDEX_MODE_ENT_indexMode = {
     "indexMode", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_MAC(CPSS_DXCH_CNC_VLAN_INDEX_MODE_ENT)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_BOOL_updateEnable = {
     "updateEnable", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_MAC(GT_BOOL)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_U32_blockNum = {
     "blockNum", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_MAC(GT_U32)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_U64_indexRangesBmp = {
     "indexRangesBmp", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_MAC(GT_U64)
};
PRV_CPSS_LOG_FUNC_PARAM_STC OUT_CPSS_DXCH_CNC_BYTE_COUNT_MODE_ENT_PTR_countModePtr = {
     "countModePtr", PRV_CPSS_LOG_PARAM_OUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(CPSS_DXCH_CNC_BYTE_COUNT_MODE_ENT)
};
PRV_CPSS_LOG_FUNC_PARAM_STC OUT_CPSS_DXCH_CNC_COUNTER_FORMAT_ENT_PTR_formatPtr = {
     "formatPtr", PRV_CPSS_LOG_PARAM_OUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(CPSS_DXCH_CNC_COUNTER_FORMAT_ENT)
};
PRV_CPSS_LOG_FUNC_PARAM_STC OUT_CPSS_DXCH_CNC_COUNTER_STC_PTR_counterPtr = {
     "counterPtr", PRV_CPSS_LOG_PARAM_OUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(CPSS_DXCH_CNC_COUNTER_STC)
};
PRV_CPSS_LOG_FUNC_PARAM_STC OUT_CPSS_DXCH_CNC_COUNTER_STC_PTR_counterValuesPtr = {
     "counterValuesPtr", PRV_CPSS_LOG_PARAM_OUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(CPSS_DXCH_CNC_COUNTER_STC)
};
PRV_CPSS_LOG_FUNC_PARAM_STC OUT_CPSS_DXCH_CNC_EGRESS_DROP_COUNT_MODE_ENT_PTR_modePtr = {
     "modePtr", PRV_CPSS_LOG_PARAM_OUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(CPSS_DXCH_CNC_EGRESS_DROP_COUNT_MODE_ENT)
};
PRV_CPSS_LOG_FUNC_PARAM_STC OUT_CPSS_DXCH_CNC_EGRESS_QUEUE_CLIENT_MODE_ENT_PTR_modePtr = {
     "modePtr", PRV_CPSS_LOG_PARAM_OUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(CPSS_DXCH_CNC_EGRESS_QUEUE_CLIENT_MODE_ENT)
};
PRV_CPSS_LOG_FUNC_PARAM_STC OUT_CPSS_DXCH_CNC_PACKET_TYPE_PASS_DROP_TO_CPU_MODE_ENT_PTR_toCpuModePtr = {
     "toCpuModePtr", PRV_CPSS_LOG_PARAM_OUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(CPSS_DXCH_CNC_PACKET_TYPE_PASS_DROP_TO_CPU_MODE_ENT)
};
PRV_CPSS_LOG_FUNC_PARAM_STC OUT_CPSS_DXCH_CNC_TM_INDEX_MODE_ENT_PTR_indexModePtr = {
     "indexModePtr", PRV_CPSS_LOG_PARAM_OUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(CPSS_DXCH_CNC_TM_INDEX_MODE_ENT)
};
PRV_CPSS_LOG_FUNC_PARAM_STC OUT_CPSS_DXCH_CNC_VLAN_INDEX_MODE_ENT_PTR_indexModePtr = {
     "indexModePtr", PRV_CPSS_LOG_PARAM_OUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(CPSS_DXCH_CNC_VLAN_INDEX_MODE_ENT)
};
PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_BOOL_PTR_updateEnablePtr = {
     "updateEnablePtr", PRV_CPSS_LOG_PARAM_OUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(GT_BOOL)
};
PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_U32_PTR_inProcessBlocksBmpPtr = {
     "inProcessBlocksBmpPtr", PRV_CPSS_LOG_PARAM_OUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(GT_U32)
};
PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_U32_PTR_indexesArr = {
     "indexesArr", PRV_CPSS_LOG_PARAM_OUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(GT_U32)
};
PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_U32_PTR_portGroupIdArr = {
     "portGroupIdArr", PRV_CPSS_LOG_PARAM_OUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(GT_U32)
};
PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_U64_PTR_indexRangesBmpPtr = {
     "indexRangesBmpPtr", PRV_CPSS_LOG_PARAM_OUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(GT_U64)
};


/********* API prototypes DB *********/

PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChCncClientByteCountModeSet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_CPSS_DXCH_CNC_CLIENT_ENT_client,
    &IN_CPSS_DXCH_CNC_BYTE_COUNT_MODE_ENT_countMode
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChCncClientByteCountModeGet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_CPSS_DXCH_CNC_CLIENT_ENT_client,
    &OUT_CPSS_DXCH_CNC_BYTE_COUNT_MODE_ENT_PTR_countModePtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChCncVlanClientIndexModeSet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_CPSS_DXCH_CNC_CLIENT_ENT_vlanClient,
    &IN_CPSS_DXCH_CNC_VLAN_INDEX_MODE_ENT_indexMode
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChCncVlanClientIndexModeGet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_CPSS_DXCH_CNC_CLIENT_ENT_vlanClient,
    &OUT_CPSS_DXCH_CNC_VLAN_INDEX_MODE_ENT_PTR_indexModePtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChCncCounterClearByReadValueSet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_CPSS_DXCH_CNC_COUNTER_FORMAT_ENT_format,
    &IN_CPSS_DXCH_CNC_COUNTER_STC_PTR_counterPtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChCncCounterClearByReadValueGet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_CPSS_DXCH_CNC_COUNTER_FORMAT_ENT_format,
    &OUT_CPSS_DXCH_CNC_COUNTER_STC_PTR_counterPtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChCncCountingEnableSet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_CPSS_DXCH_CNC_COUNTING_ENABLE_UNIT_ENT_cncUnit,
    &IN_GT_BOOL_enable
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChCncCountingEnableGet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_CPSS_DXCH_CNC_COUNTING_ENABLE_UNIT_ENT_cncUnit,
    &OUT_GT_BOOL_PTR_enablePtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChCncEgressVlanDropCountModeSet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_CPSS_DXCH_CNC_EGRESS_DROP_COUNT_MODE_ENT_mode
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChCncEgressQueueClientModeSet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_CPSS_DXCH_CNC_EGRESS_QUEUE_CLIENT_MODE_ENT_mode
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChCncPacketTypePassDropToCpuModeSet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_CPSS_DXCH_CNC_PACKET_TYPE_PASS_DROP_TO_CPU_MODE_ENT_toCpuMode
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChCncTmClientIndexModeSet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_CPSS_DXCH_CNC_TM_INDEX_MODE_ENT_indexMode
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChCncPortClientEnableSet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_PHYSICAL_PORT_NUM_portNum,
    &IN_CPSS_DXCH_CNC_CLIENT_ENT_client,
    &IN_GT_BOOL_enable
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChCncPortClientEnableGet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_PHYSICAL_PORT_NUM_portNum,
    &IN_CPSS_DXCH_CNC_CLIENT_ENT_client,
    &OUT_GT_BOOL_PTR_enablePtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChCncPortGroupBlockUploadTrigger_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_PORT_GROUPS_BMP_portGroupsBmp,
    &IN_GT_U32_blockNum
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChCncPortGroupBlockClientEnableSet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_PORT_GROUPS_BMP_portGroupsBmp,
    &IN_GT_U32_blockNum,
    &IN_CPSS_DXCH_CNC_CLIENT_ENT_client,
    &IN_GT_BOOL_updateEnable
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChCncPortGroupBlockClientRangesSet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_PORT_GROUPS_BMP_portGroupsBmp,
    &IN_GT_U32_blockNum,
    &IN_CPSS_DXCH_CNC_CLIENT_ENT_client,
    &IN_GT_U64_indexRangesBmp
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChCncPortGroupBlockClientEnableGet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_PORT_GROUPS_BMP_portGroupsBmp,
    &IN_GT_U32_blockNum,
    &IN_CPSS_DXCH_CNC_CLIENT_ENT_client,
    &OUT_GT_BOOL_PTR_updateEnablePtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChCncPortGroupBlockClientRangesGet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_PORT_GROUPS_BMP_portGroupsBmp,
    &IN_GT_U32_blockNum,
    &IN_CPSS_DXCH_CNC_CLIENT_ENT_client,
    &OUT_GT_U64_PTR_indexRangesBmpPtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChCncPortGroupCounterFormatSet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_PORT_GROUPS_BMP_portGroupsBmp,
    &IN_GT_U32_blockNum,
    &IN_CPSS_DXCH_CNC_COUNTER_FORMAT_ENT_format
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChCncPortGroupCounterSet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_PORT_GROUPS_BMP_portGroupsBmp,
    &IN_GT_U32_blockNum,
    &IN_GT_U32_index,
    &IN_CPSS_DXCH_CNC_COUNTER_FORMAT_ENT_format,
    &IN_CPSS_DXCH_CNC_COUNTER_STC_PTR_counterPtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChCncPortGroupCounterGet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_PORT_GROUPS_BMP_portGroupsBmp,
    &IN_GT_U32_blockNum,
    &IN_GT_U32_index,
    &IN_CPSS_DXCH_CNC_COUNTER_FORMAT_ENT_format,
    &OUT_CPSS_DXCH_CNC_COUNTER_STC_PTR_counterPtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChCncPortGroupCounterFormatGet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_PORT_GROUPS_BMP_portGroupsBmp,
    &IN_GT_U32_blockNum,
    &OUT_CPSS_DXCH_CNC_COUNTER_FORMAT_ENT_PTR_formatPtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChCncPortGroupUploadedBlockGet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_PORT_GROUPS_BMP_portGroupsBmp,
    &INOUT_GT_U32_PTR_numOfCounterValuesPtr,
    &IN_CPSS_DXCH_CNC_COUNTER_FORMAT_ENT_format,
    &OUT_CPSS_DXCH_CNC_COUNTER_STC_PTR_counterValuesPtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChCncPortGroupBlockUploadInProcessGet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_PORT_GROUPS_BMP_portGroupsBmp,
    &OUT_GT_U32_PTR_inProcessBlocksBmpPtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChCncBlockUploadTrigger_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_U32_blockNum
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChCncBlockClientEnableSet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_U32_blockNum,
    &IN_CPSS_DXCH_CNC_CLIENT_ENT_client,
    &IN_GT_BOOL_updateEnable
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChCncBlockClientRangesSet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_U32_blockNum,
    &IN_CPSS_DXCH_CNC_CLIENT_ENT_client,
    &IN_GT_U64_indexRangesBmp
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChCncBlockClientEnableGet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_U32_blockNum,
    &IN_CPSS_DXCH_CNC_CLIENT_ENT_client,
    &OUT_GT_BOOL_PTR_updateEnablePtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChCncBlockClientRangesGet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_U32_blockNum,
    &IN_CPSS_DXCH_CNC_CLIENT_ENT_client,
    &OUT_GT_U64_PTR_indexRangesBmpPtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChCncCounterFormatSet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_U32_blockNum,
    &IN_CPSS_DXCH_CNC_COUNTER_FORMAT_ENT_format
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChCncCounterSet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_U32_blockNum,
    &IN_GT_U32_index,
    &IN_CPSS_DXCH_CNC_COUNTER_FORMAT_ENT_format,
    &IN_CPSS_DXCH_CNC_COUNTER_STC_PTR_counterPtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChCncCounterGet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_U32_blockNum,
    &IN_GT_U32_index,
    &IN_CPSS_DXCH_CNC_COUNTER_FORMAT_ENT_format,
    &OUT_CPSS_DXCH_CNC_COUNTER_STC_PTR_counterPtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChCncCounterFormatGet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_U32_blockNum,
    &OUT_CPSS_DXCH_CNC_COUNTER_FORMAT_ENT_PTR_formatPtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChCncOffsetForNatClientSet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_U32_offset
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChCncUploadedBlockGet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &INOUT_GT_U32_PTR_numOfCounterValuesPtr,
    &IN_CPSS_DXCH_CNC_COUNTER_FORMAT_ENT_format,
    &OUT_CPSS_DXCH_CNC_COUNTER_STC_PTR_counterValuesPtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChCncEgressVlanDropCountModeGet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &OUT_CPSS_DXCH_CNC_EGRESS_DROP_COUNT_MODE_ENT_PTR_modePtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChCncEgressQueueClientModeGet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &OUT_CPSS_DXCH_CNC_EGRESS_QUEUE_CLIENT_MODE_ENT_PTR_modePtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChCncPacketTypePassDropToCpuModeGet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &OUT_CPSS_DXCH_CNC_PACKET_TYPE_PASS_DROP_TO_CPU_MODE_ENT_PTR_toCpuModePtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChCncTmClientIndexModeGet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &OUT_CPSS_DXCH_CNC_TM_INDEX_MODE_ENT_PTR_indexModePtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChCncBlockUploadInProcessGet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &OUT_GT_U32_PTR_inProcessBlocksBmpPtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChCncOffsetForNatClientGet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &OUT_GT_U32_PTR_offsetPtr
};
extern PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChCncCounterWraparoundIndexesGet_PARAMS[];
extern PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChCncPortGroupCounterWraparoundIndexesGet_PARAMS[];


/********* lib API DB *********/

extern void cpssDxChCncCounterWraparoundIndexesGet_preLogic(IN va_list  args, OUT PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC * paramDataPtr);
extern void cpssDxChCncUploadedBlockGet_preLogic(IN va_list  args, OUT PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC * paramDataPtr);
extern void cpssDxChCncPortGroupCounterWraparoundIndexesGet_preLogic(IN va_list  args, OUT PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC * paramDataPtr);
extern void cpssDxChCncPortGroupUploadedBlockGet_preLogic(IN va_list  args, OUT PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC * paramDataPtr);

static PRV_CPSS_LOG_FUNC_ENTRY_STC prvCpssDxChCncLogLibDb[] = {
    {"cpssDxChCncBlockClientEnableSet", 4, cpssDxChCncBlockClientEnableSet_PARAMS, NULL},
    {"cpssDxChCncBlockClientEnableGet", 4, cpssDxChCncBlockClientEnableGet_PARAMS, NULL},
    {"cpssDxChCncBlockClientRangesSet", 4, cpssDxChCncBlockClientRangesSet_PARAMS, NULL},
    {"cpssDxChCncBlockClientRangesGet", 4, cpssDxChCncBlockClientRangesGet_PARAMS, NULL},
    {"cpssDxChCncPortClientEnableSet", 4, cpssDxChCncPortClientEnableSet_PARAMS, NULL},
    {"cpssDxChCncPortClientEnableGet", 4, cpssDxChCncPortClientEnableGet_PARAMS, NULL},
    {"cpssDxChCncIngressVlanPassDropFromCpuCountEnableSet", 2, prvCpssLogGenDevNumEnable_PARAMS, NULL},
    {"cpssDxChCncIngressVlanPassDropFromCpuCountEnableGet", 2, prvCpssLogGenDevNumEnablePtr_PARAMS, NULL},
    {"cpssDxChCncVlanClientIndexModeSet", 3, cpssDxChCncVlanClientIndexModeSet_PARAMS, NULL},
    {"cpssDxChCncVlanClientIndexModeGet", 3, cpssDxChCncVlanClientIndexModeGet_PARAMS, NULL},
    {"cpssDxChCncPacketTypePassDropToCpuModeSet", 2, cpssDxChCncPacketTypePassDropToCpuModeSet_PARAMS, NULL},
    {"cpssDxChCncPacketTypePassDropToCpuModeGet", 2, cpssDxChCncPacketTypePassDropToCpuModeGet_PARAMS, NULL},
    {"cpssDxChCncTmClientIndexModeSet", 2, cpssDxChCncTmClientIndexModeSet_PARAMS, NULL},
    {"cpssDxChCncTmClientIndexModeGet", 2, cpssDxChCncTmClientIndexModeGet_PARAMS, NULL},
    {"cpssDxChCncClientByteCountModeSet", 3, cpssDxChCncClientByteCountModeSet_PARAMS, NULL},
    {"cpssDxChCncClientByteCountModeGet", 3, cpssDxChCncClientByteCountModeGet_PARAMS, NULL},
    {"cpssDxChCncEgressVlanDropCountModeSet", 2, cpssDxChCncEgressVlanDropCountModeSet_PARAMS, NULL},
    {"cpssDxChCncEgressVlanDropCountModeGet", 2, cpssDxChCncEgressVlanDropCountModeGet_PARAMS, NULL},
    {"cpssDxChCncCounterClearByReadEnableSet", 2, prvCpssLogGenDevNumEnable_PARAMS, NULL},
    {"cpssDxChCncCounterClearByReadEnableGet", 2, prvCpssLogGenDevNumEnablePtr_PARAMS, NULL},
    {"cpssDxChCncCounterClearByReadValueSet", 3, cpssDxChCncCounterClearByReadValueSet_PARAMS, NULL},
    {"cpssDxChCncCounterClearByReadValueGet", 3, cpssDxChCncCounterClearByReadValueGet_PARAMS, NULL},
    {"cpssDxChCncCounterWraparoundEnableSet", 2, prvCpssLogGenDevNumEnable_PARAMS, NULL},
    {"cpssDxChCncCounterWraparoundEnableGet", 2, prvCpssLogGenDevNumEnablePtr_PARAMS, NULL},
    {"cpssDxChCncCounterWraparoundIndexesGet", 4, cpssDxChCncCounterWraparoundIndexesGet_PARAMS, cpssDxChCncCounterWraparoundIndexesGet_preLogic},
    {"cpssDxChCncCounterSet", 5, cpssDxChCncCounterSet_PARAMS, NULL},
    {"cpssDxChCncCounterGet", 5, cpssDxChCncCounterGet_PARAMS, NULL},
    {"cpssDxChCncBlockUploadTrigger", 2, cpssDxChCncBlockUploadTrigger_PARAMS, NULL},
    {"cpssDxChCncBlockUploadInProcessGet", 2, cpssDxChCncBlockUploadInProcessGet_PARAMS, NULL},
    {"cpssDxChCncUploadedBlockGet", 4, cpssDxChCncUploadedBlockGet_PARAMS, cpssDxChCncUploadedBlockGet_preLogic},
    {"cpssDxChCncCountingEnableSet", 3, cpssDxChCncCountingEnableSet_PARAMS, NULL},
    {"cpssDxChCncCountingEnableGet", 3, cpssDxChCncCountingEnableGet_PARAMS, NULL},
    {"cpssDxChCncCounterFormatSet", 3, cpssDxChCncCounterFormatSet_PARAMS, NULL},
    {"cpssDxChCncCounterFormatGet", 3, cpssDxChCncCounterFormatGet_PARAMS, NULL},
    {"cpssDxChCncEgressQueueClientModeSet", 2, cpssDxChCncEgressQueueClientModeSet_PARAMS, NULL},
    {"cpssDxChCncEgressQueueClientModeGet", 2, cpssDxChCncEgressQueueClientModeGet_PARAMS, NULL},
    {"cpssDxChCncCpuAccessStrictPriorityEnableSet", 2, prvCpssLogGenDevNumEnable_PARAMS, NULL},
    {"cpssDxChCncCpuAccessStrictPriorityEnableGet", 2, prvCpssLogGenDevNumEnablePtr_PARAMS, NULL},
    {"cpssDxChCncPortGroupBlockClientEnableSet", 5, cpssDxChCncPortGroupBlockClientEnableSet_PARAMS, NULL},
    {"cpssDxChCncPortGroupBlockClientEnableGet", 5, cpssDxChCncPortGroupBlockClientEnableGet_PARAMS, NULL},
    {"cpssDxChCncPortGroupBlockClientRangesSet", 5, cpssDxChCncPortGroupBlockClientRangesSet_PARAMS, NULL},
    {"cpssDxChCncPortGroupBlockClientRangesGet", 5, cpssDxChCncPortGroupBlockClientRangesGet_PARAMS, NULL},
    {"cpssDxChCncPortGroupCounterWraparoundIndexesGet", 6, cpssDxChCncPortGroupCounterWraparoundIndexesGet_PARAMS, cpssDxChCncPortGroupCounterWraparoundIndexesGet_preLogic},
    {"cpssDxChCncPortGroupCounterSet", 6, cpssDxChCncPortGroupCounterSet_PARAMS, NULL},
    {"cpssDxChCncPortGroupCounterGet", 6, cpssDxChCncPortGroupCounterGet_PARAMS, NULL},
    {"cpssDxChCncPortGroupBlockUploadTrigger", 3, cpssDxChCncPortGroupBlockUploadTrigger_PARAMS, NULL},
    {"cpssDxChCncPortGroupBlockUploadInProcessGet", 3, cpssDxChCncPortGroupBlockUploadInProcessGet_PARAMS, NULL},
    {"cpssDxChCncPortGroupUploadedBlockGet", 5, cpssDxChCncPortGroupUploadedBlockGet_PARAMS, cpssDxChCncPortGroupUploadedBlockGet_preLogic},
    {"cpssDxChCncPortGroupCounterFormatSet", 4, cpssDxChCncPortGroupCounterFormatSet_PARAMS, NULL},
    {"cpssDxChCncPortGroupCounterFormatGet", 4, cpssDxChCncPortGroupCounterFormatGet_PARAMS, NULL},
    {"cpssDxChCncOffsetForNatClientSet", 2, cpssDxChCncOffsetForNatClientSet_PARAMS, NULL},
    {"cpssDxChCncOffsetForNatClientGet", 2, cpssDxChCncOffsetForNatClientGet_PARAMS, NULL},
};

/******** DB Access Function ********/
void prvCpssLogParamLibDbGet_CPSS_LOG_LIB_CNC(
    OUT PRV_CPSS_LOG_FUNC_ENTRY_STC ** logFuncDbPtrPtr,
    OUT GT_U32 * logFuncDbSizePtr
)
{
    *logFuncDbPtrPtr = prvCpssDxChCncLogLibDb;
    *logFuncDbSizePtr = sizeof(prvCpssDxChCncLogLibDb) / sizeof(PRV_CPSS_LOG_FUNC_ENTRY_STC);
}

