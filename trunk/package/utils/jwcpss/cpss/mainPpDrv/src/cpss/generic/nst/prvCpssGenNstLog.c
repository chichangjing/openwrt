/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* prvCpssGenNstLog.c
*       WARNING!!! this is a generated file, please don't edit it manually
* COMMENTS:
*
* FILE REVISION NUMBER:
*       $Revision: 1 $
*******************************************************************************/
#include <cpss/generic/log/cpssLog.h>
#include <cpss/generic/log/prvCpssLog.h>
#include <cpss/extServices/private/prvCpssBindFunc.h>
#include <cpss/generic/nst/cpssNstTypes.h>
#include <cpss/generic/nst/private/prvCpssGenNstLog.h>


/********* enums *********/

char * prvCpssLogEnum_CPSS_NST_AM_PARAM_ENT[]  =
{
    "CPSS_NST_AM_SA_AUTO_LEARNED_E",
    "CPSS_NST_AM_DA_AUTO_LEARNED_E",
    "CPSS_NST_AM_SA_UNKNOWN_E",
    "CPSS_NST_AM_DA_UNKNOWN_E"
};
PRV_CPSS_LOG_STC_ENUM_ARRAY_SIZE_MAC(CPSS_NST_AM_PARAM_ENT);
char * prvCpssLogEnum_CPSS_NST_CHECK_ENT[]  =
{
    "CPSS_NST_CHECK_TCP_SYN_DATA_E",
    "CPSS_NST_CHECK_TCP_OVER_MAC_MC_BC_E",
    "CPSS_NST_CHECK_TCP_FLAG_ZERO_E",
    "CPSS_NST_CHECK_TCP_FLAGS_FIN_URG_PSH_E",
    "CPSS_NST_CHECK_TCP_FLAGS_SYN_FIN_E",
    "CPSS_NST_CHECK_TCP_FLAGS_SYN_RST_E",
    "CPSS_NST_CHECK_TCP_UDP_PORT_ZERO_E",
    "CPSS_NST_CHECK_TCP_ALL_E",
    "CPSS_NST_CHECK_FRAG_IPV4_ICMP_E",
    "CPSS_NST_CHECK_ARP_MAC_SA_MISMATCH_E"
};
PRV_CPSS_LOG_STC_ENUM_ARRAY_SIZE_MAC(CPSS_NST_CHECK_ENT);
char * prvCpssLogEnum_CPSS_NST_EGRESS_FRW_FILTER_ENT[]  =
{
    "CPSS_NST_EGRESS_FRW_FILTER_FROM_CPU_E",
    "CPSS_NST_EGRESS_FRW_FILTER_BRIDGED_E",
    "CPSS_NST_EGRESS_FRW_FILTER_ROUTED_E",
    "CPSS_NST_EGRESS_FRW_FILTER_MPLS_E",
    "CPSS_NST_EGRESS_FRW_FILTER_L2VPN_E"
};
PRV_CPSS_LOG_STC_ENUM_ARRAY_SIZE_MAC(CPSS_NST_EGRESS_FRW_FILTER_ENT);
char * prvCpssLogEnum_CPSS_NST_INGRESS_FRW_FILTER_ENT[]  =
{
    "CPSS_NST_INGRESS_FRW_FILTER_TO_NETWORK_E",
    "CPSS_NST_INGRESS_FRW_FILTER_TO_CPU_E",
    "CPSS_NST_INGRESS_FRW_FILTER_TO_ANALYZER_E"
};
PRV_CPSS_LOG_STC_ENUM_ARRAY_SIZE_MAC(CPSS_NST_INGRESS_FRW_FILTER_ENT);


/********* parameters log functions *********/

void prvCpssLogParamFunc_CPSS_NST_AM_PARAM_ENT(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_SET_PARAM_VAL_MAC(CPSS_NST_AM_PARAM_ENT, paramVal);
    PRV_CPSS_LOG_ENUM_MAC(namePtr, paramVal, CPSS_NST_AM_PARAM_ENT);
}
void prvCpssLogParamFunc_CPSS_NST_CHECK_ENT(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_SET_PARAM_VAL_MAC(CPSS_NST_CHECK_ENT, paramVal);
    PRV_CPSS_LOG_ENUM_MAC(namePtr, paramVal, CPSS_NST_CHECK_ENT);
}
void prvCpssLogParamFunc_CPSS_NST_EGRESS_FRW_FILTER_ENT(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_SET_PARAM_VAL_MAC(CPSS_NST_EGRESS_FRW_FILTER_ENT, paramVal);
    PRV_CPSS_LOG_ENUM_MAC(namePtr, paramVal, CPSS_NST_EGRESS_FRW_FILTER_ENT);
}
void prvCpssLogParamFunc_CPSS_NST_INGRESS_FRW_FILTER_ENT(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_SET_PARAM_VAL_MAC(CPSS_NST_INGRESS_FRW_FILTER_ENT, paramVal);
    PRV_CPSS_LOG_ENUM_MAC(namePtr, paramVal, CPSS_NST_INGRESS_FRW_FILTER_ENT);
}

