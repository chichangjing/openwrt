/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* prvCpssGenIpLog.c
*       WARNING!!! this is a generated file, please don't edit it manually
* COMMENTS:
*
* FILE REVISION NUMBER:
*       $Revision: 1 $
*******************************************************************************/
#include <cpss/generic/log/cpssLog.h>
#include <cpss/generic/log/prvCpssLog.h>
#include <cpss/extServices/private/prvCpssBindFunc.h>
#include <cpss/generic/ip/cpssIpTypes.h>
#include <cpss/generic/ip/private/prvCpssGenIpLog.h>


/********* enums *********/

char * prvCpssLogEnum_CPSS_IPV6_MLL_SELECTION_RULE_ENT[]  =
{
    "CPSS_IPV6_MLL_SELECTION_RULE_LOCAL_E",
    "CPSS_IPV6_MLL_SELECTION_RULE_GLOBAL_E"
};
PRV_CPSS_LOG_STC_ENUM_ARRAY_SIZE_MAC(CPSS_IPV6_MLL_SELECTION_RULE_ENT);
char * prvCpssLogEnum_CPSS_IPV6_PREFIX_SCOPE_ENT[]  =
{
    "CPSS_IPV6_PREFIX_SCOPE_LINK_LOCAL_E",
    "CPSS_IPV6_PREFIX_SCOPE_SITE_LOCAL_E",
    "CPSS_IPV6_PREFIX_SCOPE_UNIQUE_LOCAL_E",
    "CPSS_IPV6_PREFIX_SCOPE_GLOBAL_E"
};
PRV_CPSS_LOG_STC_ENUM_ARRAY_SIZE_MAC(CPSS_IPV6_PREFIX_SCOPE_ENT);
char * prvCpssLogEnum_CPSS_IP_CNT_SET_ENT[]  =
{
    "CPSS_IP_CNT_SET0_E",
    "CPSS_IP_CNT_SET1_E",
    "CPSS_IP_CNT_SET2_E",
    "CPSS_IP_CNT_SET3_E",
    "CPSS_IP_CNT_NO_SET_E"
};
PRV_CPSS_LOG_STC_ENUM_ARRAY_SIZE_MAC(CPSS_IP_CNT_SET_ENT);
char * prvCpssLogEnum_CPSS_IP_SITE_ID_ENT[]  =
{
    "CPSS_IP_SITE_ID_INTERNAL_E",
    "CPSS_IP_SITE_ID_EXTERNAL_E"
};
PRV_CPSS_LOG_STC_ENUM_ARRAY_SIZE_MAC(CPSS_IP_SITE_ID_ENT);
char * prvCpssLogEnum_CPSS_IP_UNICAST_MULTICAST_ENT[]  =
{
    "CPSS_IP_UNICAST_E",
    "CPSS_IP_MULTICAST_E"
};
PRV_CPSS_LOG_STC_ENUM_ARRAY_SIZE_MAC(CPSS_IP_UNICAST_MULTICAST_ENT);


/********* parameters log functions *********/

void prvCpssLogParamFunc_CPSS_IPV6_MLL_SELECTION_RULE_ENT(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_SET_PARAM_VAL_MAC(CPSS_IPV6_MLL_SELECTION_RULE_ENT, paramVal);
    PRV_CPSS_LOG_ENUM_MAC(namePtr, paramVal, CPSS_IPV6_MLL_SELECTION_RULE_ENT);
}
void prvCpssLogParamFunc_CPSS_IPV6_MLL_SELECTION_RULE_ENT_PTR(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_SET_PARAM_VAL_MAC(CPSS_IPV6_MLL_SELECTION_RULE_ENT*, paramVal);
    if (paramVal == NULL)
    {
        PRV_CPSS_LOG_AND_HISTORY_PARAM_MAC(contextLib, logType, "%s = NULL\n", namePtr);
        return;
    }
    PRV_CPSS_LOG_ENUM_MAC(namePtr, *paramVal, CPSS_IPV6_MLL_SELECTION_RULE_ENT);
}
void prvCpssLogParamFunc_CPSS_IPV6_PREFIX_SCOPE_ENT(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_SET_PARAM_VAL_MAC(CPSS_IPV6_PREFIX_SCOPE_ENT, paramVal);
    PRV_CPSS_LOG_ENUM_MAC(namePtr, paramVal, CPSS_IPV6_PREFIX_SCOPE_ENT);
}
void prvCpssLogParamFunc_CPSS_IPV6_PREFIX_SCOPE_ENT_PTR(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_SET_PARAM_VAL_MAC(CPSS_IPV6_PREFIX_SCOPE_ENT*, paramVal);
    if (paramVal == NULL)
    {
        PRV_CPSS_LOG_AND_HISTORY_PARAM_MAC(contextLib, logType, "%s = NULL\n", namePtr);
        return;
    }
    PRV_CPSS_LOG_ENUM_MAC(namePtr, *paramVal, CPSS_IPV6_PREFIX_SCOPE_ENT);
}
void prvCpssLogParamFunc_CPSS_IP_CNT_SET_ENT(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_SET_PARAM_VAL_MAC(CPSS_IP_CNT_SET_ENT, paramVal);
    PRV_CPSS_LOG_ENUM_MAC(namePtr, paramVal, CPSS_IP_CNT_SET_ENT);
}
void prvCpssLogParamFunc_CPSS_IP_SITE_ID_ENT(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_SET_PARAM_VAL_MAC(CPSS_IP_SITE_ID_ENT, paramVal);
    PRV_CPSS_LOG_ENUM_MAC(namePtr, paramVal, CPSS_IP_SITE_ID_ENT);
}
void prvCpssLogParamFunc_CPSS_IP_UNICAST_MULTICAST_ENT(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_SET_PARAM_VAL_MAC(CPSS_IP_UNICAST_MULTICAST_ENT, paramVal);
    PRV_CPSS_LOG_ENUM_MAC(namePtr, paramVal, CPSS_IP_UNICAST_MULTICAST_ENT);
}

