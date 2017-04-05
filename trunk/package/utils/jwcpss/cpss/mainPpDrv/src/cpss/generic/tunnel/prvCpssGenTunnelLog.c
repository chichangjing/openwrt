/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* prvCpssGenTunnelLog.c
*       WARNING!!! this is a generated file, please don't edit it manually
* COMMENTS:
*
* FILE REVISION NUMBER:
*       $Revision: 1 $
*******************************************************************************/
#include <cpss/generic/log/cpssLog.h>
#include <cpss/generic/log/prvCpssLog.h>
#include <cpss/extServices/private/prvCpssBindFunc.h>
#include <cpss/generic/tunnel/cpssGenTunnelTypes.h>
#include <cpss/generic/tunnel/private/prvCpssGenTunnelLog.h>


/********* enums *********/

char * prvCpssLogEnum_CPSS_TUNNEL_ETHERTYPE_TYPE_ENT[]  =
{
    "CPSS_TUNNEL_ETHERTYPE_TYPE_IPV4_GRE0_E",
    "CPSS_TUNNEL_ETHERTYPE_TYPE_IPV4_GRE1_E",
    "CPSS_TUNNEL_ETHERTYPE_TYPE_MPLS_UNICAST_E",
    "CPSS_TUNNEL_ETHERTYPE_TYPE_MPLS_MULTICAST_E",
    "CPSS_TUNNEL_ETHERTYPE_TYPE_MIM_E",
    "CPSS_TUNNEL_ETHERTYPE_TYPE_TRILL_E",
    "CPSS_TUNNEL_ETHERTYPE_TYPE_IPV6_GRE0_E",
    "CPSS_TUNNEL_ETHERTYPE_TYPE_IPV6_GRE1_E",
    "CPSS_TUNNEL_ETHERTYPE_TYPE_USER_DEFINED0_E",
    "CPSS_TUNNEL_ETHERTYPE_TYPE_USER_DEFINED1_E",
    "CPSS_TUNNEL_ETHERTYPE_TYPE_USER_DEFINED2_E",
    "CPSS_TUNNEL_ETHERTYPE_TYPE_USER_DEFINED3_E",
    "CPSS_TUNNEL_ETHERTYPE_TYPE_USER_DEFINED4_E",
    "CPSS_TUNNEL_ETHERTYPE_TYPE_USER_DEFINED5_E",
    "CPSS_TUNNEL_ETHERTYPE_TYPE_USER_DEFINED6_E"
};
PRV_CPSS_LOG_STC_ENUM_ARRAY_SIZE_MAC(CPSS_TUNNEL_ETHERTYPE_TYPE_ENT);
char * prvCpssLogEnum_CPSS_TUNNEL_MULTICAST_TYPE_ENT[]  =
{
    "CPSS_TUNNEL_MULTICAST_IPV4_E",
    "CPSS_TUNNEL_MULTICAST_IPV6_E",
    "CPSS_TUNNEL_MULTICAST_MPLS_E",
    "CPSS_TUNNEL_MULTICAST_TRILL_E",
    "CPSS_TUNNEL_MULTICAST_PBB_E"
};
PRV_CPSS_LOG_STC_ENUM_ARRAY_SIZE_MAC(CPSS_TUNNEL_MULTICAST_TYPE_ENT);
char * prvCpssLogEnum_CPSS_TUNNEL_TYPE_ENT[]  =
{
    "CPSS_TUNNEL_IPV4_OVER_IPV4_E",
    "CPSS_TUNNEL_IPV4_OVER_GRE_IPV4_E",
    "CPSS_TUNNEL_IPV6_OVER_IPV4_E",
    "CPSS_TUNNEL_IPV6_OVER_GRE_IPV4_E",
    "CPSS_TUNNEL_IP_OVER_MPLS_E",
    "CPSS_TUNNEL_ETHERNET_OVER_MPLS_E",
    "CPSS_TUNNEL_X_OVER_IPV4_E",
    "CPSS_TUNNEL_X_OVER_GRE_IPV4_E",
    "CPSS_TUNNEL_X_OVER_MPLS_E",
    "CPSS_TUNNEL_IP_OVER_X_E",
    "CPSS_TUNNEL_MAC_IN_MAC_E",
    "CPSS_TUNNEL_TRILL_E",
    "CPSS_TUNNEL_Q_IN_Q_E",
    "CPSS_TUNNEL_GENERIC_IPV4_E",
    "CPSS_TUNNEL_GENERIC_IPV6_E"
};
PRV_CPSS_LOG_STC_ENUM_ARRAY_SIZE_MAC(CPSS_TUNNEL_TYPE_ENT);
char * prvCpssLogEnum_CPSS_TUNNEL_PASSENGER_PACKET_ENT[]  =
{
    "CPSS_TUNNEL_PASSENGER_PACKET_IPV4_E",
    "CPSS_TUNNEL_PASSENGER_PACKET_IPV6_E",
    "CPSS_TUNNEL_PASSENGER_PACKET_ETHERNET_NO_CRC_E",
    "CPSS_TUNNEL_PASSENGER_PACKET_ETHERNET_CRC_E"
};
PRV_CPSS_LOG_STC_ENUM_ARRAY_SIZE_MAC(CPSS_TUNNEL_PASSENGER_PACKET_ENT);


/********* parameters log functions *********/

void prvCpssLogParamFunc_CPSS_TUNNEL_ETHERTYPE_TYPE_ENT(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_SET_PARAM_VAL_MAC(CPSS_TUNNEL_ETHERTYPE_TYPE_ENT, paramVal);
    PRV_CPSS_LOG_ENUM_MAC(namePtr, paramVal, CPSS_TUNNEL_ETHERTYPE_TYPE_ENT);
}
void prvCpssLogParamFunc_CPSS_TUNNEL_MULTICAST_TYPE_ENT(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_SET_PARAM_VAL_MAC(CPSS_TUNNEL_MULTICAST_TYPE_ENT, paramVal);
    PRV_CPSS_LOG_ENUM_MAC(namePtr, paramVal, CPSS_TUNNEL_MULTICAST_TYPE_ENT);
}
void prvCpssLogParamFunc_CPSS_TUNNEL_TYPE_ENT(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_SET_PARAM_VAL_MAC(CPSS_TUNNEL_TYPE_ENT, paramVal);
    PRV_CPSS_LOG_ENUM_MAC(namePtr, paramVal, CPSS_TUNNEL_TYPE_ENT);
}
void prvCpssLogParamFunc_CPSS_TUNNEL_TYPE_ENT_PTR(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_SET_PARAM_VAL_MAC(CPSS_TUNNEL_TYPE_ENT*, paramVal);
    if (paramVal == NULL)
    {
        PRV_CPSS_LOG_AND_HISTORY_PARAM_MAC(contextLib, logType, "%s = NULL\n", namePtr);
        return;
    }
    PRV_CPSS_LOG_ENUM_MAC(namePtr, *paramVal, CPSS_TUNNEL_TYPE_ENT);
}

