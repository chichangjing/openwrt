/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* prvCpssGenPclLog.c
*       WARNING!!! this is a generated file, please don't edit it manually
* COMMENTS:
*
* FILE REVISION NUMBER:
*       $Revision: 1 $
*******************************************************************************/
#include <cpss/generic/log/cpssLog.h>
#include <cpss/generic/log/prvCpssLog.h>
#include <cpss/extServices/private/prvCpssBindFunc.h>
#include <cpss/generic/pcl/cpssPcl.h>
#include <cpss/generic/pcl/private/prvCpssGenPclLog.h>


/********* enums *********/

char * prvCpssLogEnum_CPSS_PCL_DIRECTION_ENT[]  =
{
    "CPSS_PCL_DIRECTION_INGRESS_E",
    "CPSS_PCL_DIRECTION_EGRESS_E"
};
PRV_CPSS_LOG_STC_ENUM_ARRAY_SIZE_MAC(CPSS_PCL_DIRECTION_ENT);
PRV_CPSS_ENUM_STRING_VALUE_PAIR_STC prvCpssLogEnum_map_CPSS_PCL_LOOKUP_NUMBER_ENT[]  =
{PRV_CPSS_LOG_ENUM_NAME_AND_VALUE_MAC(CPSS_PCL_LOOKUP_0_E),
PRV_CPSS_LOG_ENUM_NAME_AND_VALUE_MAC(CPSS_PCL_LOOKUP_1_E),
PRV_CPSS_LOG_ENUM_NAME_AND_VALUE_MAC(CPSS_PCL_LOOKUP_0_0_E),
PRV_CPSS_LOG_ENUM_NAME_AND_VALUE_MAC(CPSS_PCL_LOOKUP_0_1_E),
PRV_CPSS_LOG_ENUM_NAME_AND_VALUE_MAC(CPSS_PCL_LOOKUP_NUMBER_0_E),
PRV_CPSS_LOG_ENUM_NAME_AND_VALUE_MAC(CPSS_PCL_LOOKUP_NUMBER_1_E),
PRV_CPSS_LOG_ENUM_NAME_AND_VALUE_MAC(CPSS_PCL_LOOKUP_NUMBER_2_E)
};
PRV_CPSS_LOG_STC_ENUM_MAP_ARRAY_SIZE_MAC(CPSS_PCL_LOOKUP_NUMBER_ENT);
PRV_CPSS_ENUM_STRING_VALUE_PAIR_STC prvCpssLogEnum_map_CPSS_PCL_RULE_SIZE_ENT[]  =
{PRV_CPSS_LOG_ENUM_NAME_AND_VALUE_MAC(CPSS_PCL_RULE_SIZE_STD_E),
PRV_CPSS_LOG_ENUM_NAME_AND_VALUE_MAC(CPSS_PCL_RULE_SIZE_30_BYTES_E),
PRV_CPSS_LOG_ENUM_NAME_AND_VALUE_MAC(CPSS_PCL_RULE_SIZE_EXT_E),
PRV_CPSS_LOG_ENUM_NAME_AND_VALUE_MAC(CPSS_PCL_RULE_SIZE_60_BYTES_E),
PRV_CPSS_LOG_ENUM_NAME_AND_VALUE_MAC(CPSS_PCL_RULE_SIZE_ULTRA_E),
PRV_CPSS_LOG_ENUM_NAME_AND_VALUE_MAC(CPSS_PCL_RULE_SIZE_80_BYTES_E),
PRV_CPSS_LOG_ENUM_NAME_AND_VALUE_MAC(CPSS_PCL_RULE_SIZE_10_BYTES_E),
PRV_CPSS_LOG_ENUM_NAME_AND_VALUE_MAC(CPSS_PCL_RULE_SIZE_20_BYTES_E),
PRV_CPSS_LOG_ENUM_NAME_AND_VALUE_MAC(CPSS_PCL_RULE_SIZE_40_BYTES_E),
PRV_CPSS_LOG_ENUM_NAME_AND_VALUE_MAC(CPSS_PCL_RULE_SIZE_50_BYTES_E)
};
PRV_CPSS_LOG_STC_ENUM_MAP_ARRAY_SIZE_MAC(CPSS_PCL_RULE_SIZE_ENT);


/********* parameters log functions *********/

void prvCpssLogParamFunc_CPSS_PCL_DIRECTION_ENT(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_SET_PARAM_VAL_MAC(CPSS_PCL_DIRECTION_ENT, paramVal);
    PRV_CPSS_LOG_ENUM_MAC(namePtr, paramVal, CPSS_PCL_DIRECTION_ENT);
}
void prvCpssLogParamFunc_CPSS_PCL_LOOKUP_NUMBER_ENT(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_SET_PARAM_VAL_MAC(CPSS_PCL_LOOKUP_NUMBER_ENT, paramVal);
    PRV_CPSS_LOG_ENUM_MAP_MAC(namePtr, paramVal, CPSS_PCL_LOOKUP_NUMBER_ENT);
}
void prvCpssLogParamFunc_CPSS_PCL_RULE_SIZE_ENT(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_SET_PARAM_VAL_MAC(CPSS_PCL_RULE_SIZE_ENT, paramVal);
    PRV_CPSS_LOG_ENUM_MAP_MAC(namePtr, paramVal, CPSS_PCL_RULE_SIZE_ENT);
}
void prvCpssLogParamFunc_CPSS_PCL_RULE_SIZE_ENT_PTR(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_SET_PARAM_VAL_MAC(CPSS_PCL_RULE_SIZE_ENT*, paramVal);
    if (paramVal == NULL)
    {
        PRV_CPSS_LOG_AND_HISTORY_PARAM_MAC(contextLib, logType, "%s = NULL\n", namePtr);
        return;
    }
    PRV_CPSS_LOG_ENUM_MAP_MAC(namePtr, *paramVal, CPSS_PCL_RULE_SIZE_ENT);
}

