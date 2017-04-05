/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* prvCpssGenPolicerLog.c
*       WARNING!!! this is a generated file, please don't edit it manually
* COMMENTS:
*
* FILE REVISION NUMBER:
*       $Revision: 1 $
*******************************************************************************/
#include <cpss/generic/log/cpssLog.h>
#include <cpss/generic/log/prvCpssLog.h>
#include <cpss/extServices/private/prvCpssBindFunc.h>
#include <cpss/generic/policer/cpssGenPolicerTypes.h>
#include <cpss/generic/policer/private/prvCpssGenPolicerLog.h>


/********* enums *********/

char * prvCpssLogEnum_CPSS_POLICER_MRU_ENT[]  =
{
    "CPSS_POLICER_MRU_1536_E",
    "CPSS_POLICER_MRU_2K_E",
    "CPSS_POLICER_MRU_10K_E"
};
PRV_CPSS_LOG_STC_ENUM_ARRAY_SIZE_MAC(CPSS_POLICER_MRU_ENT);
char * prvCpssLogEnum_CPSS_POLICER_PACKET_SIZE_MODE_ENT[]  =
{
    "CPSS_POLICER_PACKET_SIZE_TUNNEL_PASSENGER_E",
    "CPSS_POLICER_PACKET_SIZE_L3_ONLY_E",
    "CPSS_POLICER_PACKET_SIZE_L2_INCLUDE_E",
    "CPSS_POLICER_PACKET_SIZE_L1_INCLUDE_E"
};
PRV_CPSS_LOG_STC_ENUM_ARRAY_SIZE_MAC(CPSS_POLICER_PACKET_SIZE_MODE_ENT);
char * prvCpssLogEnum_CPSS_POLICER_TB_MODE_ENT[]  =
{
    "CPSS_POLICER_TB_STRICT_E",
    "CPSS_POLICER_TB_LOOSE_E"
};
PRV_CPSS_LOG_STC_ENUM_ARRAY_SIZE_MAC(CPSS_POLICER_TB_MODE_ENT);
char * prvCpssLogEnum_CPSS_POLICER_COLOR_MODE_ENT[]  =
{
    "CPSS_POLICER_COLOR_BLIND_E",
    "CPSS_POLICER_COLOR_AWARE_E"
};
PRV_CPSS_LOG_STC_ENUM_ARRAY_SIZE_MAC(CPSS_POLICER_COLOR_MODE_ENT);


/********* parameters log functions *********/

void prvCpssLogParamFunc_CPSS_POLICER_MRU_ENT(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_SET_PARAM_VAL_MAC(CPSS_POLICER_MRU_ENT, paramVal);
    PRV_CPSS_LOG_ENUM_MAC(namePtr, paramVal, CPSS_POLICER_MRU_ENT);
}
void prvCpssLogParamFunc_CPSS_POLICER_PACKET_SIZE_MODE_ENT(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_SET_PARAM_VAL_MAC(CPSS_POLICER_PACKET_SIZE_MODE_ENT, paramVal);
    PRV_CPSS_LOG_ENUM_MAC(namePtr, paramVal, CPSS_POLICER_PACKET_SIZE_MODE_ENT);
}
void prvCpssLogParamFunc_CPSS_POLICER_PACKET_SIZE_MODE_ENT_PTR(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_SET_PARAM_VAL_MAC(CPSS_POLICER_PACKET_SIZE_MODE_ENT*, paramVal);
    if (paramVal == NULL)
    {
        PRV_CPSS_LOG_AND_HISTORY_PARAM_MAC(contextLib, logType, "%s = NULL\n", namePtr);
        return;
    }
    PRV_CPSS_LOG_ENUM_MAC(namePtr, *paramVal, CPSS_POLICER_PACKET_SIZE_MODE_ENT);
}
void prvCpssLogParamFunc_CPSS_POLICER_TB_MODE_ENT(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_SET_PARAM_VAL_MAC(CPSS_POLICER_TB_MODE_ENT, paramVal);
    PRV_CPSS_LOG_ENUM_MAC(namePtr, paramVal, CPSS_POLICER_TB_MODE_ENT);
}

