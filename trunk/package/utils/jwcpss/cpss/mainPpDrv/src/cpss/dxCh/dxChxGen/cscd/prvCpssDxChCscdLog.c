/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* prvCpssDxChCscdLog.c
*       WARNING!!! this is a generated file, please don't edit it manually
* COMMENTS:
*
* FILE REVISION NUMBER:
*       $Revision: 1 $
*******************************************************************************/
#include <cpss/generic/log/cpssLog.h>
#include <cpss/generic/log/prvCpssLog.h>
#include <cpss/extServices/private/prvCpssBindFunc.h>
#include <cpss/dxCh/dxChxGen/cscd/cpssDxChCscd.h>
#include <cpss/dxCh/dxChxGen/cscd/private/prvCpssDxChCscdLog.h>
#include <cpss/dxCh/dxChxGen/networkIf/private/prvCpssDxChNetworkIfLog.h>
#include <cpss/generic/config/private/prvCpssGenConfigLog.h>
#include <cpss/generic/cos/private/prvCpssGenCosLog.h>
#include <cpss/generic/cscd/private/prvCpssGenCscdLog.h>
#include <cpss/generic/log/prvCpssGenLog.h>
#include <cpss/generic/port/private/prvCpssGenPortLog.h>


/********* enums *********/

char * prvCpssLogEnum_CPSS_DXCH_CSCD_HYPER_G_PORT_MODE_ENT[]  =
{
    "CPSS_DXCH_CSCD_HYPER_G_PORT_MODE_STANDART_E",
    "CPSS_DXCH_CSCD_HYPER_G_PORT_MODE_ACCELERATED_E"
};
PRV_CPSS_LOG_STC_ENUM_ARRAY_SIZE_MAC(CPSS_DXCH_CSCD_HYPER_G_PORT_MODE_ENT);
char * prvCpssLogEnum_CPSS_DXCH_CSCD_INGRESS_PHYSICAL_PORT_ASSIGNMENT_MODE_ENT[]  =
{
    "CPSS_DXCH_CSCD_INGRESS_PHYSICAL_PORT_ASSIGNMENT_MODE_DISABLED_E",
    "CPSS_DXCH_CSCD_INGRESS_PHYSICAL_PORT_ASSIGNMENT_MODE_INTERLAKEN_E",
    "CPSS_DXCH_CSCD_INGRESS_PHYSICAL_PORT_ASSIGNMENT_MODE_DSA_E"
};
PRV_CPSS_LOG_STC_ENUM_ARRAY_SIZE_MAC(CPSS_DXCH_CSCD_INGRESS_PHYSICAL_PORT_ASSIGNMENT_MODE_ENT);
char * prvCpssLogEnum_CPSS_DXCH_CSCD_PORT_CRC_MODE_ENT[]  =
{
    "CPSS_DXCH_CSCD_PORT_CRC_ONE_BYTE_E",
    "CPSS_DXCH_CSCD_PORT_CRC_TWO_BYTES_E",
    "CPSS_DXCH_CSCD_PORT_CRC_THREE_BYTES_E",
    "CPSS_DXCH_CSCD_PORT_CRC_FOUR_BYTES_E"
};
PRV_CPSS_LOG_STC_ENUM_ARRAY_SIZE_MAC(CPSS_DXCH_CSCD_PORT_CRC_MODE_ENT);
char * prvCpssLogEnum_CPSS_DXCH_CSCD_PORT_QOS_DSA_TRUST_MODE_ENT[]  =
{
    "CPSS_DXCH_CSCD_PORT_QOS_DSA_TRUST_MODE_GLOBAL_E",
    "CPSS_DXCH_CSCD_PORT_QOS_DSA_TRUST_MODE_EXTENDED_E"
};
PRV_CPSS_LOG_STC_ENUM_ARRAY_SIZE_MAC(CPSS_DXCH_CSCD_PORT_QOS_DSA_TRUST_MODE_ENT);
char * prvCpssLogEnum_CPSS_DXCH_CSCD_QOS_REMAP_TYPE_ENT[]  =
{
    "CPSS_DXCH_CSCD_QOS_REMAP_DISABLED_E",
    "CPSS_DXCH_CSCD_QOS_REMAP_CNTRL_ONLY_E",
    "CPSS_DXCH_CSCD_QOS_REMAP_DATA_ONLY_E",
    "CPSS_DXCH_CSCD_QOS_REMAP_ALL_E"
};
PRV_CPSS_LOG_STC_ENUM_ARRAY_SIZE_MAC(CPSS_DXCH_CSCD_QOS_REMAP_TYPE_ENT);
char * prvCpssLogEnum_CPSS_DXCH_CSCD_TRUNK_LINK_HASH_ENT[]  =
{
    "CPSS_DXCH_CSCD_TRUNK_LINK_HASH_IS_INGR_PIPE_HASH_E",
    "CPSS_DXCH_CSCD_TRUNK_LINK_HASH_IS_SRC_PORT_E",
    "CPSS_DXCH_CSCD_TRUNK_LINK_HASH_IS_DST_PORT_E",
    "CPSS_DXCH_CSCD_TRUNK_LINK_HASH_IS_DYNAMIC_LOAD_BALANCING_E"
};
PRV_CPSS_LOG_STC_ENUM_ARRAY_SIZE_MAC(CPSS_DXCH_CSCD_TRUNK_LINK_HASH_ENT);
char * prvCpssLogEnum_CPSS_DXCH_DEV_MAP_LOOKUP_MODE_ENT[]  =
{
    "CPSS_DXCH_DEV_MAP_LOOKUP_MODE_TRG_DEV_E",
    "CPSS_DXCH_DEV_MAP_LOOKUP_MODE_TRG_DEV_TRG_PORT_E",
    "CPSS_DXCH_DEV_MAP_LOOKUP_MODE_TRG_DEV_SRC_DEV_E",
    "CPSS_DXCH_DEV_MAP_LOOKUP_MODE_TRG_DEV_SRC_PORT_E",
    "CPSS_DXCH_DEV_MAP_LOOKUP_MODE_TRG_DEV_1BIT_SRC_DEV_SRC_PORT_E",
    "CPSS_DXCH_DEV_MAP_LOOKUP_MODE_TRG_DEV_2BITS_SRC_DEV_SRC_PORT_E",
    "CPSS_DXCH_DEV_MAP_LOOKUP_MODE_TRG_DEV_3BITS_SRC_DEV_SRC_PORT_E",
    "CPSS_DXCH_DEV_MAP_LOOKUP_MODE_TRG_DEV_4BITS_SRC_DEV_SRC_PORT_E",
    "CPSS_DXCH_DEV_MAP_LOOKUP_MODE_TRG_DEV_5BITS_SRC_DEV_SRC_PORT_E"
};
PRV_CPSS_LOG_STC_ENUM_ARRAY_SIZE_MAC(CPSS_DXCH_DEV_MAP_LOOKUP_MODE_ENT);


/********* structure fields log functions *********/

void prvCpssLogParamFuncStc_CPSS_DXCH_CSCD_QOS_TC_DP_REMAP_INDEX_STC_PTR
(
    IN CPSS_LOG_LIB_ENT         contextLib,
    IN CPSS_LOG_TYPE_ENT        logType,
    IN GT_CHAR_PTR              namePtr,
    IN void                   * fieldPtr,
    INOUT PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC     * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_START_FIELD_STC_MAC(CPSS_DXCH_CSCD_QOS_TC_DP_REMAP_INDEX_STC *, valPtr);
    PRV_CPSS_LOG_UNUSED_MAC(inOutParamInfoPtr);
    PRV_CPSS_LOG_STC_NUMBER_MAC(valPtr, tc);
    PRV_CPSS_LOG_STC_ENUM_MAC(valPtr, dp, CPSS_DP_LEVEL_ENT);
    PRV_CPSS_LOG_STC_BOOL_MAC(valPtr, isStack);
    PRV_CPSS_LOG_STC_ENUM_MAC(valPtr, dsaTagCmd, CPSS_DXCH_NET_DSA_CMD_ENT);
    prvCpssLogStcLogEnd(contextLib, logType);
}
void prvCpssLogParamFuncStc_CPSS_DXCH_CSCD_QOS_TC_REMAP_STC_PTR
(
    IN CPSS_LOG_LIB_ENT         contextLib,
    IN CPSS_LOG_TYPE_ENT        logType,
    IN GT_CHAR_PTR              namePtr,
    IN void                   * fieldPtr,
    INOUT PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC     * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_START_FIELD_STC_MAC(CPSS_DXCH_CSCD_QOS_TC_REMAP_STC *, valPtr);
    PRV_CPSS_LOG_UNUSED_MAC(inOutParamInfoPtr);
    PRV_CPSS_LOG_STC_NUMBER_MAC(valPtr, forwardLocalTc);
    PRV_CPSS_LOG_STC_NUMBER_MAC(valPtr, forwardStackTc);
    PRV_CPSS_LOG_STC_NUMBER_MAC(valPtr, toAnalyzerLocalTc);
    PRV_CPSS_LOG_STC_NUMBER_MAC(valPtr, toAnalyzerStackTc);
    PRV_CPSS_LOG_STC_NUMBER_MAC(valPtr, toCpuLocalTc);
    PRV_CPSS_LOG_STC_NUMBER_MAC(valPtr, toCpuStackTc);
    PRV_CPSS_LOG_STC_NUMBER_MAC(valPtr, fromCpuLocalTc);
    PRV_CPSS_LOG_STC_NUMBER_MAC(valPtr, fromCpuStackTc);
    prvCpssLogStcLogEnd(contextLib, logType);
}


/********* parameters log functions *********/

void prvCpssLogParamFunc_CPSS_DXCH_CSCD_HYPER_G_PORT_MODE_ENT(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_SET_PARAM_VAL_MAC(CPSS_DXCH_CSCD_HYPER_G_PORT_MODE_ENT, paramVal);
    PRV_CPSS_LOG_ENUM_MAC(namePtr, paramVal, CPSS_DXCH_CSCD_HYPER_G_PORT_MODE_ENT);
}
void prvCpssLogParamFunc_CPSS_DXCH_CSCD_HYPER_G_PORT_MODE_ENT_PTR(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_SET_PARAM_VAL_MAC(CPSS_DXCH_CSCD_HYPER_G_PORT_MODE_ENT*, paramVal);
    if (paramVal == NULL)
    {
        PRV_CPSS_LOG_AND_HISTORY_PARAM_MAC(contextLib, logType, "%s = NULL\n", namePtr);
        return;
    }
    PRV_CPSS_LOG_ENUM_MAC(namePtr, *paramVal, CPSS_DXCH_CSCD_HYPER_G_PORT_MODE_ENT);
}
void prvCpssLogParamFunc_CPSS_DXCH_CSCD_INGRESS_PHYSICAL_PORT_ASSIGNMENT_MODE_ENT(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_SET_PARAM_VAL_MAC(CPSS_DXCH_CSCD_INGRESS_PHYSICAL_PORT_ASSIGNMENT_MODE_ENT, paramVal);
    PRV_CPSS_LOG_ENUM_MAC(namePtr, paramVal, CPSS_DXCH_CSCD_INGRESS_PHYSICAL_PORT_ASSIGNMENT_MODE_ENT);
}
void prvCpssLogParamFunc_CPSS_DXCH_CSCD_INGRESS_PHYSICAL_PORT_ASSIGNMENT_MODE_ENT_PTR(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_SET_PARAM_VAL_MAC(CPSS_DXCH_CSCD_INGRESS_PHYSICAL_PORT_ASSIGNMENT_MODE_ENT*, paramVal);
    if (paramVal == NULL)
    {
        PRV_CPSS_LOG_AND_HISTORY_PARAM_MAC(contextLib, logType, "%s = NULL\n", namePtr);
        return;
    }
    PRV_CPSS_LOG_ENUM_MAC(namePtr, *paramVal, CPSS_DXCH_CSCD_INGRESS_PHYSICAL_PORT_ASSIGNMENT_MODE_ENT);
}
void prvCpssLogParamFunc_CPSS_DXCH_CSCD_PORT_CRC_MODE_ENT(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_SET_PARAM_VAL_MAC(CPSS_DXCH_CSCD_PORT_CRC_MODE_ENT, paramVal);
    PRV_CPSS_LOG_ENUM_MAC(namePtr, paramVal, CPSS_DXCH_CSCD_PORT_CRC_MODE_ENT);
}
void prvCpssLogParamFunc_CPSS_DXCH_CSCD_PORT_CRC_MODE_ENT_PTR(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_SET_PARAM_VAL_MAC(CPSS_DXCH_CSCD_PORT_CRC_MODE_ENT*, paramVal);
    if (paramVal == NULL)
    {
        PRV_CPSS_LOG_AND_HISTORY_PARAM_MAC(contextLib, logType, "%s = NULL\n", namePtr);
        return;
    }
    PRV_CPSS_LOG_ENUM_MAC(namePtr, *paramVal, CPSS_DXCH_CSCD_PORT_CRC_MODE_ENT);
}
void prvCpssLogParamFunc_CPSS_DXCH_CSCD_PORT_QOS_DSA_TRUST_MODE_ENT(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_SET_PARAM_VAL_MAC(CPSS_DXCH_CSCD_PORT_QOS_DSA_TRUST_MODE_ENT, paramVal);
    PRV_CPSS_LOG_ENUM_MAC(namePtr, paramVal, CPSS_DXCH_CSCD_PORT_QOS_DSA_TRUST_MODE_ENT);
}
void prvCpssLogParamFunc_CPSS_DXCH_CSCD_PORT_QOS_DSA_TRUST_MODE_ENT_PTR(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_SET_PARAM_VAL_MAC(CPSS_DXCH_CSCD_PORT_QOS_DSA_TRUST_MODE_ENT*, paramVal);
    if (paramVal == NULL)
    {
        PRV_CPSS_LOG_AND_HISTORY_PARAM_MAC(contextLib, logType, "%s = NULL\n", namePtr);
        return;
    }
    PRV_CPSS_LOG_ENUM_MAC(namePtr, *paramVal, CPSS_DXCH_CSCD_PORT_QOS_DSA_TRUST_MODE_ENT);
}
void prvCpssLogParamFunc_CPSS_DXCH_CSCD_QOS_REMAP_TYPE_ENT(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_SET_PARAM_VAL_MAC(CPSS_DXCH_CSCD_QOS_REMAP_TYPE_ENT, paramVal);
    PRV_CPSS_LOG_ENUM_MAC(namePtr, paramVal, CPSS_DXCH_CSCD_QOS_REMAP_TYPE_ENT);
}
void prvCpssLogParamFunc_CPSS_DXCH_CSCD_QOS_REMAP_TYPE_ENT_PTR(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_SET_PARAM_VAL_MAC(CPSS_DXCH_CSCD_QOS_REMAP_TYPE_ENT*, paramVal);
    if (paramVal == NULL)
    {
        PRV_CPSS_LOG_AND_HISTORY_PARAM_MAC(contextLib, logType, "%s = NULL\n", namePtr);
        return;
    }
    PRV_CPSS_LOG_ENUM_MAC(namePtr, *paramVal, CPSS_DXCH_CSCD_QOS_REMAP_TYPE_ENT);
}
void prvCpssLogParamFunc_CPSS_DXCH_CSCD_QOS_TC_DP_REMAP_INDEX_STC_PTR(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_START_PARAM_STC_MAC(CPSS_DXCH_CSCD_QOS_TC_DP_REMAP_INDEX_STC*, paramVal);
    prvCpssLogParamFuncStc_CPSS_DXCH_CSCD_QOS_TC_DP_REMAP_INDEX_STC_PTR(contextLib, logType, namePtr, paramVal, inOutParamInfoPtr);
}
void prvCpssLogParamFunc_CPSS_DXCH_CSCD_QOS_TC_REMAP_STC_PTR(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_START_PARAM_STC_MAC(CPSS_DXCH_CSCD_QOS_TC_REMAP_STC*, paramVal);
    prvCpssLogParamFuncStc_CPSS_DXCH_CSCD_QOS_TC_REMAP_STC_PTR(contextLib, logType, namePtr, paramVal, inOutParamInfoPtr);
}
void prvCpssLogParamFunc_CPSS_DXCH_CSCD_TRUNK_LINK_HASH_ENT(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_SET_PARAM_VAL_MAC(CPSS_DXCH_CSCD_TRUNK_LINK_HASH_ENT, paramVal);
    PRV_CPSS_LOG_ENUM_MAC(namePtr, paramVal, CPSS_DXCH_CSCD_TRUNK_LINK_HASH_ENT);
}
void prvCpssLogParamFunc_CPSS_DXCH_CSCD_TRUNK_LINK_HASH_ENT_PTR(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_SET_PARAM_VAL_MAC(CPSS_DXCH_CSCD_TRUNK_LINK_HASH_ENT*, paramVal);
    if (paramVal == NULL)
    {
        PRV_CPSS_LOG_AND_HISTORY_PARAM_MAC(contextLib, logType, "%s = NULL\n", namePtr);
        return;
    }
    PRV_CPSS_LOG_ENUM_MAC(namePtr, *paramVal, CPSS_DXCH_CSCD_TRUNK_LINK_HASH_ENT);
}
void prvCpssLogParamFunc_CPSS_DXCH_DEV_MAP_LOOKUP_MODE_ENT(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_SET_PARAM_VAL_MAC(CPSS_DXCH_DEV_MAP_LOOKUP_MODE_ENT, paramVal);
    PRV_CPSS_LOG_ENUM_MAC(namePtr, paramVal, CPSS_DXCH_DEV_MAP_LOOKUP_MODE_ENT);
}
void prvCpssLogParamFunc_CPSS_DXCH_DEV_MAP_LOOKUP_MODE_ENT_PTR(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_SET_PARAM_VAL_MAC(CPSS_DXCH_DEV_MAP_LOOKUP_MODE_ENT*, paramVal);
    if (paramVal == NULL)
    {
        PRV_CPSS_LOG_AND_HISTORY_PARAM_MAC(contextLib, logType, "%s = NULL\n", namePtr);
        return;
    }
    PRV_CPSS_LOG_ENUM_MAC(namePtr, *paramVal, CPSS_DXCH_DEV_MAP_LOOKUP_MODE_ENT);
}


/********* API fields DB *********/

PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_CSCD_LINK_TYPE_STC_PTR_cascadeLinkPtr = {
     "cascadeLinkPtr", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(CPSS_CSCD_LINK_TYPE_STC)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_CSCD_PORT_TYPE_ENT_portType = {
     "portType", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_MAC(CPSS_CSCD_PORT_TYPE_ENT)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_DIRECTION_ENT_portDirection = {
     "portDirection", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_MAC(CPSS_DIRECTION_ENT)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_DP_LEVEL_ENT_cpuToCpuDp = {
     "cpuToCpuDp", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_MAC(CPSS_DP_LEVEL_ENT)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_DP_LEVEL_ENT_ctrlDp = {
     "ctrlDp", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_MAC(CPSS_DP_LEVEL_ENT)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_DP_LEVEL_ENT_newDp = {
     "newDp", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_MAC(CPSS_DP_LEVEL_ENT)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_DP_LEVEL_ENT_remapDp = {
     "remapDp", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_MAC(CPSS_DP_LEVEL_ENT)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_DXCH_CSCD_HYPER_G_PORT_MODE_ENT_mode = {
     "mode", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_MAC(CPSS_DXCH_CSCD_HYPER_G_PORT_MODE_ENT)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_DXCH_CSCD_INGRESS_PHYSICAL_PORT_ASSIGNMENT_MODE_ENT_assignmentMode = {
     "assignmentMode", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_MAC(CPSS_DXCH_CSCD_INGRESS_PHYSICAL_PORT_ASSIGNMENT_MODE_ENT)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_DXCH_CSCD_PORT_CRC_MODE_ENT_crcMode = {
     "crcMode", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_MAC(CPSS_DXCH_CSCD_PORT_CRC_MODE_ENT)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_DXCH_CSCD_PORT_QOS_DSA_TRUST_MODE_ENT_portQosDsaMode = {
     "portQosDsaMode", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_MAC(CPSS_DXCH_CSCD_PORT_QOS_DSA_TRUST_MODE_ENT)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_DXCH_CSCD_QOS_REMAP_TYPE_ENT_remapType = {
     "remapType", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_MAC(CPSS_DXCH_CSCD_QOS_REMAP_TYPE_ENT)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_DXCH_CSCD_QOS_TC_DP_REMAP_INDEX_STC_PTR_tcDpRemappingPtr = {
     "tcDpRemappingPtr", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(CPSS_DXCH_CSCD_QOS_TC_DP_REMAP_INDEX_STC)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_DXCH_CSCD_QOS_TC_REMAP_STC_PTR_tcMappingsPtr = {
     "tcMappingsPtr", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(CPSS_DXCH_CSCD_QOS_TC_REMAP_STC)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_DXCH_CSCD_TRUNK_LINK_HASH_ENT_srcPortTrunkHashEn = {
     "srcPortTrunkHashEn", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_MAC(CPSS_DXCH_CSCD_TRUNK_LINK_HASH_ENT)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_DXCH_DEV_MAP_LOOKUP_MODE_ENT_mode = {
     "mode", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_MAC(CPSS_DXCH_DEV_MAP_LOOKUP_MODE_ENT)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_GEN_CFG_HW_DEV_NUM_MODE_ENT_hwDevMode = {
     "hwDevMode", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_MAC(CPSS_GEN_CFG_HW_DEV_NUM_MODE_ENT)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_PORT_DIRECTION_ENT_portDirection = {
     "portDirection", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_MAC(CPSS_PORT_DIRECTION_ENT)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_BOOL_egressAttributesLocallyEn = {
     "egressAttributesLocallyEn", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_MAC(GT_BOOL)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_BOOL_enableOwnDevFltr = {
     "enableOwnDevFltr", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_MAC(GT_BOOL)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_BOOL_isLooped = {
     "isLooped", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_MAC(GT_BOOL)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_HW_DEV_NUM_aggHwDevNum = {
     "aggHwDevNum", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_MAC(GT_HW_DEV_NUM)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_HW_DEV_NUM_remotePhysicalHwDevNum = {
     "remotePhysicalHwDevNum", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_MAC(GT_HW_DEV_NUM)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_HW_DEV_NUM_reservedHwDevNum = {
     "reservedHwDevNum", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_MAC(GT_HW_DEV_NUM)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_HW_DEV_NUM_sourceHwDevNum = {
     "sourceHwDevNum", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_MAC(GT_HW_DEV_NUM)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_HW_DEV_NUM_targetHwDevNum = {
     "targetHwDevNum", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_MAC(GT_HW_DEV_NUM)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_PHYSICAL_PORT_NUM_remotePhysicalPortNum = {
     "remotePhysicalPortNum", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_MAC(GT_PHYSICAL_PORT_NUM)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_PHYSICAL_PORT_NUM_secondaryTargetPort = {
     "secondaryTargetPort", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_MAC(GT_PHYSICAL_PORT_NUM)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_PORT_NUM_aggPortNum = {
     "aggPortNum", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_MAC(GT_PORT_NUM)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_PORT_NUM_portBaseEport = {
     "portBaseEport", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_MAC(GT_PORT_NUM)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_PORT_NUM_trunkBaseEport = {
     "trunkBaseEport", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_MAC(GT_PORT_NUM)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_U32_aggSrcId = {
     "aggSrcId", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_MAC(GT_U32)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_U32_newTc = {
     "newTc", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_MAC(GT_U32)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_U32_physicalPortBase = {
     "physicalPortBase", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_MAC(GT_U32)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_U32_srcDevLsbAmount = {
     "srcDevLsbAmount", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_MAC(GT_U32)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_U32_srcId = {
     "srcId", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_MAC(GT_U32)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_U32_srcPortLsbAmount = {
     "srcPortLsbAmount", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_MAC(GT_U32)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_U32_srcTrunkLsbAmount = {
     "srcTrunkLsbAmount", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_MAC(GT_U32)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_U8_ctrlTc = {
     "ctrlTc", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_MAC(GT_U8)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_U8_remapTc = {
     "remapTc", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_MAC(GT_U8)
};
PRV_CPSS_LOG_FUNC_PARAM_STC OUT_CPSS_CSCD_LINK_TYPE_STC_PTR_cascadeLinkPtr = {
     "cascadeLinkPtr", PRV_CPSS_LOG_PARAM_OUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(CPSS_CSCD_LINK_TYPE_STC)
};
PRV_CPSS_LOG_FUNC_PARAM_STC OUT_CPSS_CSCD_PORT_TYPE_ENT_PTR_portTypePtr = {
     "portTypePtr", PRV_CPSS_LOG_PARAM_OUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(CPSS_CSCD_PORT_TYPE_ENT)
};
PRV_CPSS_LOG_FUNC_PARAM_STC OUT_CPSS_DP_LEVEL_ENT_PTR_cpuToCpuDpPtr = {
     "cpuToCpuDpPtr", PRV_CPSS_LOG_PARAM_OUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(CPSS_DP_LEVEL_ENT)
};
PRV_CPSS_LOG_FUNC_PARAM_STC OUT_CPSS_DP_LEVEL_ENT_PTR_ctrlDpPtr = {
     "ctrlDpPtr", PRV_CPSS_LOG_PARAM_OUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(CPSS_DP_LEVEL_ENT)
};
PRV_CPSS_LOG_FUNC_PARAM_STC OUT_CPSS_DP_LEVEL_ENT_PTR_remapDpPtr = {
     "remapDpPtr", PRV_CPSS_LOG_PARAM_OUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(CPSS_DP_LEVEL_ENT)
};
PRV_CPSS_LOG_FUNC_PARAM_STC OUT_CPSS_DP_LEVEL_ENT_PTR_remappedDpPtr = {
     "remappedDpPtr", PRV_CPSS_LOG_PARAM_OUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(CPSS_DP_LEVEL_ENT)
};
PRV_CPSS_LOG_FUNC_PARAM_STC OUT_CPSS_DXCH_CSCD_HYPER_G_PORT_MODE_ENT_PTR_modePtr = {
     "modePtr", PRV_CPSS_LOG_PARAM_OUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(CPSS_DXCH_CSCD_HYPER_G_PORT_MODE_ENT)
};
PRV_CPSS_LOG_FUNC_PARAM_STC OUT_CPSS_DXCH_CSCD_INGRESS_PHYSICAL_PORT_ASSIGNMENT_MODE_ENT_PTR_assignmentModePtr = {
     "assignmentModePtr", PRV_CPSS_LOG_PARAM_OUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(CPSS_DXCH_CSCD_INGRESS_PHYSICAL_PORT_ASSIGNMENT_MODE_ENT)
};
PRV_CPSS_LOG_FUNC_PARAM_STC OUT_CPSS_DXCH_CSCD_PORT_CRC_MODE_ENT_PTR_crcModePtr = {
     "crcModePtr", PRV_CPSS_LOG_PARAM_OUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(CPSS_DXCH_CSCD_PORT_CRC_MODE_ENT)
};
PRV_CPSS_LOG_FUNC_PARAM_STC OUT_CPSS_DXCH_CSCD_PORT_QOS_DSA_TRUST_MODE_ENT_PTR_portQosDsaTrustModePtr = {
     "portQosDsaTrustModePtr", PRV_CPSS_LOG_PARAM_OUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(CPSS_DXCH_CSCD_PORT_QOS_DSA_TRUST_MODE_ENT)
};
PRV_CPSS_LOG_FUNC_PARAM_STC OUT_CPSS_DXCH_CSCD_QOS_REMAP_TYPE_ENT_PTR_remapTypePtr = {
     "remapTypePtr", PRV_CPSS_LOG_PARAM_OUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(CPSS_DXCH_CSCD_QOS_REMAP_TYPE_ENT)
};
PRV_CPSS_LOG_FUNC_PARAM_STC OUT_CPSS_DXCH_CSCD_QOS_TC_REMAP_STC_PTR_tcMappingsPtr = {
     "tcMappingsPtr", PRV_CPSS_LOG_PARAM_OUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(CPSS_DXCH_CSCD_QOS_TC_REMAP_STC)
};
PRV_CPSS_LOG_FUNC_PARAM_STC OUT_CPSS_DXCH_CSCD_TRUNK_LINK_HASH_ENT_PTR_srcPortTrunkHashEnPtr = {
     "srcPortTrunkHashEnPtr", PRV_CPSS_LOG_PARAM_OUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(CPSS_DXCH_CSCD_TRUNK_LINK_HASH_ENT)
};
PRV_CPSS_LOG_FUNC_PARAM_STC OUT_CPSS_DXCH_DEV_MAP_LOOKUP_MODE_ENT_PTR_modePtr = {
     "modePtr", PRV_CPSS_LOG_PARAM_OUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(CPSS_DXCH_DEV_MAP_LOOKUP_MODE_ENT)
};
PRV_CPSS_LOG_FUNC_PARAM_STC OUT_CPSS_GEN_CFG_HW_DEV_NUM_MODE_ENT_PTR_hwDevModePtr = {
     "hwDevModePtr", PRV_CPSS_LOG_PARAM_OUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(CPSS_GEN_CFG_HW_DEV_NUM_MODE_ENT)
};
PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_BOOL_PTR_egressAttributesLocallyEnPtr = {
     "egressAttributesLocallyEnPtr", PRV_CPSS_LOG_PARAM_OUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(GT_BOOL)
};
PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_BOOL_PTR_enableOwnDevFltrPtr = {
     "enableOwnDevFltrPtr", PRV_CPSS_LOG_PARAM_OUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(GT_BOOL)
};
PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_BOOL_PTR_isLoopedPtr = {
     "isLoopedPtr", PRV_CPSS_LOG_PARAM_OUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(GT_BOOL)
};
PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_HW_DEV_NUM_PTR_aggHwDevNumPtr = {
     "aggHwDevNumPtr", PRV_CPSS_LOG_PARAM_OUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(GT_HW_DEV_NUM)
};
PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_HW_DEV_NUM_PTR_remotePhysicalHwDevNumPtr = {
     "remotePhysicalHwDevNumPtr", PRV_CPSS_LOG_PARAM_OUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(GT_HW_DEV_NUM)
};
PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_HW_DEV_NUM_PTR_reservedHwDevNumPtr = {
     "reservedHwDevNumPtr", PRV_CPSS_LOG_PARAM_OUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(GT_HW_DEV_NUM)
};
PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_PHYSICAL_PORT_NUM_PTR_remotePhysicalPortNumPtr = {
     "remotePhysicalPortNumPtr", PRV_CPSS_LOG_PARAM_OUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(GT_PHYSICAL_PORT_NUM)
};
PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_PHYSICAL_PORT_NUM_PTR_secondaryTargetPortPtr = {
     "secondaryTargetPortPtr", PRV_CPSS_LOG_PARAM_OUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(GT_PHYSICAL_PORT_NUM)
};
PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_PORT_NUM_PTR_aggPortNumPtr = {
     "aggPortNumPtr", PRV_CPSS_LOG_PARAM_OUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(GT_PORT_NUM)
};
PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_PORT_NUM_PTR_portBaseEportPtr = {
     "portBaseEportPtr", PRV_CPSS_LOG_PARAM_OUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(GT_PORT_NUM)
};
PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_PORT_NUM_PTR_trunkBaseEportPtr = {
     "trunkBaseEportPtr", PRV_CPSS_LOG_PARAM_OUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(GT_PORT_NUM)
};
PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_U32_PTR_aggSrcIdPtr = {
     "aggSrcIdPtr", PRV_CPSS_LOG_PARAM_OUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(GT_U32)
};
PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_U32_PTR_physicalPortBasePtr = {
     "physicalPortBasePtr", PRV_CPSS_LOG_PARAM_OUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(GT_U32)
};
PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_U32_PTR_remappedTcPtr = {
     "remappedTcPtr", PRV_CPSS_LOG_PARAM_OUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(GT_U32)
};
PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_U32_PTR_srcDevLsbAmountPtr = {
     "srcDevLsbAmountPtr", PRV_CPSS_LOG_PARAM_OUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(GT_U32)
};
PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_U32_PTR_srcIdPtr = {
     "srcIdPtr", PRV_CPSS_LOG_PARAM_OUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(GT_U32)
};
PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_U32_PTR_srcPortLsbAmountPtr = {
     "srcPortLsbAmountPtr", PRV_CPSS_LOG_PARAM_OUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(GT_U32)
};
PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_U32_PTR_srcTrunkLsbAmountPtr = {
     "srcTrunkLsbAmountPtr", PRV_CPSS_LOG_PARAM_OUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(GT_U32)
};
PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_U8_PTR_ctrlTcPtr = {
     "ctrlTcPtr", PRV_CPSS_LOG_PARAM_OUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(GT_U8)
};
PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_U8_PTR_remapTcPtr = {
     "remapTcPtr", PRV_CPSS_LOG_PARAM_OUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(GT_U8)
};


/********* API prototypes DB *********/

PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChCscdDbRemoteHwDevNumModeSet_PARAMS[] =  {
    &IN_GT_HW_DEV_NUM_hwDevNum,
    &IN_CPSS_GEN_CFG_HW_DEV_NUM_MODE_ENT_hwDevMode
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChCscdDbRemoteHwDevNumModeGet_PARAMS[] =  {
    &IN_GT_HW_DEV_NUM_hwDevNum,
    &OUT_CPSS_GEN_CFG_HW_DEV_NUM_MODE_ENT_PTR_hwDevModePtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChCscdQosTcDpRemapTableSet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_CPSS_DXCH_CSCD_QOS_TC_DP_REMAP_INDEX_STC_PTR_tcDpRemappingPtr,
    &IN_GT_U32_newTc,
    &IN_CPSS_DP_LEVEL_ENT_newDp
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChCscdQosTcDpRemapTableGet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_CPSS_DXCH_CSCD_QOS_TC_DP_REMAP_INDEX_STC_PTR_tcDpRemappingPtr,
    &OUT_GT_U32_PTR_remappedTcPtr,
    &OUT_CPSS_DP_LEVEL_ENT_PTR_remappedDpPtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChCscdDevMapLookupModeSet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_CPSS_DXCH_DEV_MAP_LOOKUP_MODE_ENT_mode
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChCscdDsaSrcDevFilterSet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_BOOL_enableOwnDevFltr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChCscdDevMapTableSet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_HW_DEV_NUM_targetHwDevNum,
    &IN_GT_HW_DEV_NUM_sourceHwDevNum,
    &IN_GT_PORT_NUM_portNum,
    &IN_CPSS_CSCD_LINK_TYPE_STC_PTR_cascadeLinkPtr,
    &IN_CPSS_DXCH_CSCD_TRUNK_LINK_HASH_ENT_srcPortTrunkHashEn,
    &IN_GT_BOOL_egressAttributesLocallyEn
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChCscdDevMapTableGet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_HW_DEV_NUM_targetHwDevNum,
    &IN_GT_HW_DEV_NUM_sourceHwDevNum,
    &IN_GT_PORT_NUM_portNum,
    &OUT_CPSS_CSCD_LINK_TYPE_STC_PTR_cascadeLinkPtr,
    &OUT_CPSS_DXCH_CSCD_TRUNK_LINK_HASH_ENT_PTR_srcPortTrunkHashEnPtr,
    &OUT_GT_BOOL_PTR_egressAttributesLocallyEnPtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChCscdPortLocalDevMapLookupEnableSet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_PHYSICAL_PORT_NUM_portNum,
    &IN_CPSS_DIRECTION_ENT_portDirection,
    &IN_GT_BOOL_enable
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChCscdPortLocalDevMapLookupEnableGet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_PHYSICAL_PORT_NUM_portNum,
    &IN_CPSS_DIRECTION_ENT_portDirection,
    &OUT_GT_BOOL_PTR_enablePtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChCscdHyperGInternalPortModeSet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_PHYSICAL_PORT_NUM_portNum,
    &IN_CPSS_DXCH_CSCD_HYPER_G_PORT_MODE_ENT_mode
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChCscdCentralizedChassisMyPhysicalPortAssignmentModeSet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_PHYSICAL_PORT_NUM_portNum,
    &IN_CPSS_DXCH_CSCD_INGRESS_PHYSICAL_PORT_ASSIGNMENT_MODE_ENT_assignmentMode
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChCscdPortQosDsaModeSet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_PHYSICAL_PORT_NUM_portNum,
    &IN_CPSS_DXCH_CSCD_PORT_QOS_DSA_TRUST_MODE_ENT_portQosDsaMode
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChCscdRemapQosModeSet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_PHYSICAL_PORT_NUM_portNum,
    &IN_CPSS_DXCH_CSCD_QOS_REMAP_TYPE_ENT_remapType
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChCscdPortTypeSet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_PHYSICAL_PORT_NUM_portNum,
    &IN_CPSS_PORT_DIRECTION_ENT_portDirection,
    &IN_CPSS_CSCD_PORT_TYPE_ENT_portType
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChCscdHyperGPortCrcModeSet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_PHYSICAL_PORT_NUM_portNum,
    &IN_CPSS_PORT_DIRECTION_ENT_portDirection,
    &IN_CPSS_DXCH_CSCD_PORT_CRC_MODE_ENT_crcMode
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChCscdPortTypeGet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_PHYSICAL_PORT_NUM_portNum,
    &IN_CPSS_PORT_DIRECTION_ENT_portDirection,
    &OUT_CPSS_CSCD_PORT_TYPE_ENT_PTR_portTypePtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChCscdHyperGPortCrcModeGet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_PHYSICAL_PORT_NUM_portNum,
    &IN_CPSS_PORT_DIRECTION_ENT_portDirection,
    &OUT_CPSS_DXCH_CSCD_PORT_CRC_MODE_ENT_PTR_crcModePtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChCscdFastFailoverPortIsLoopedSet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_PHYSICAL_PORT_NUM_portNum,
    &IN_GT_BOOL_isLooped
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChCscdPortStackAggregationConfigSet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_PHYSICAL_PORT_NUM_portNum,
    &IN_GT_HW_DEV_NUM_aggHwDevNum,
    &IN_GT_PORT_NUM_aggPortNum,
    &IN_GT_U32_aggSrcId
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChCscdCentralizedChassisRemotePhysicalPortMapSet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_PHYSICAL_PORT_NUM_portNum,
    &IN_GT_HW_DEV_NUM_remotePhysicalHwDevNum,
    &IN_GT_PHYSICAL_PORT_NUM_remotePhysicalPortNum
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChCscdCentralizedChassisReservedDevNumSet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_PHYSICAL_PORT_NUM_portNum,
    &IN_GT_HW_DEV_NUM_reservedHwDevNum
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChCscdFastFailoverSecondaryTargetPortMapSet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_PHYSICAL_PORT_NUM_portNum,
    &IN_GT_PHYSICAL_PORT_NUM_secondaryTargetPort
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChCscdCentralizedChassisLineCardDefaultEportBaseSet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_PHYSICAL_PORT_NUM_portNum,
    &IN_GT_PORT_NUM_portBaseEport,
    &IN_GT_PORT_NUM_trunkBaseEport
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChCscdCentralizedChassisMyPhysicalPortBaseSet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_PHYSICAL_PORT_NUM_portNum,
    &IN_GT_U32_physicalPortBase
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChCscdCentralizedChassisSrcIdSet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_PHYSICAL_PORT_NUM_portNum,
    &IN_GT_U32_srcId
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChCscdHyperGInternalPortModeGet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_PHYSICAL_PORT_NUM_portNum,
    &OUT_CPSS_DXCH_CSCD_HYPER_G_PORT_MODE_ENT_PTR_modePtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChCscdCentralizedChassisMyPhysicalPortAssignmentModeGet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_PHYSICAL_PORT_NUM_portNum,
    &OUT_CPSS_DXCH_CSCD_INGRESS_PHYSICAL_PORT_ASSIGNMENT_MODE_ENT_PTR_assignmentModePtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChCscdPortQosDsaModeGet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_PHYSICAL_PORT_NUM_portNum,
    &OUT_CPSS_DXCH_CSCD_PORT_QOS_DSA_TRUST_MODE_ENT_PTR_portQosDsaTrustModePtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChCscdRemapQosModeGet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_PHYSICAL_PORT_NUM_portNum,
    &OUT_CPSS_DXCH_CSCD_QOS_REMAP_TYPE_ENT_PTR_remapTypePtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChCscdFastFailoverPortIsLoopedGet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_PHYSICAL_PORT_NUM_portNum,
    &OUT_GT_BOOL_PTR_isLoopedPtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChCscdPortStackAggregationConfigGet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_PHYSICAL_PORT_NUM_portNum,
    &OUT_GT_HW_DEV_NUM_PTR_aggHwDevNumPtr,
    &OUT_GT_PORT_NUM_PTR_aggPortNumPtr,
    &OUT_GT_U32_PTR_aggSrcIdPtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChCscdCentralizedChassisRemotePhysicalPortMapGet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_PHYSICAL_PORT_NUM_portNum,
    &OUT_GT_HW_DEV_NUM_PTR_remotePhysicalHwDevNumPtr,
    &OUT_GT_PHYSICAL_PORT_NUM_PTR_remotePhysicalPortNumPtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChCscdCentralizedChassisReservedDevNumGet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_PHYSICAL_PORT_NUM_portNum,
    &OUT_GT_HW_DEV_NUM_PTR_reservedHwDevNumPtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChCscdFastFailoverSecondaryTargetPortMapGet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_PHYSICAL_PORT_NUM_portNum,
    &OUT_GT_PHYSICAL_PORT_NUM_PTR_secondaryTargetPortPtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChCscdCentralizedChassisLineCardDefaultEportBaseGet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_PHYSICAL_PORT_NUM_portNum,
    &OUT_GT_PORT_NUM_PTR_portBaseEportPtr,
    &OUT_GT_PORT_NUM_PTR_trunkBaseEportPtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChCscdCentralizedChassisMyPhysicalPortBaseGet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_PHYSICAL_PORT_NUM_portNum,
    &OUT_GT_U32_PTR_physicalPortBasePtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChCscdCentralizedChassisSrcIdGet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_PHYSICAL_PORT_NUM_portNum,
    &OUT_GT_U32_PTR_srcIdPtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChCscdCentralizedChassisMyPhysicalPortMappingDsaSrcLsbAmountSet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_U32_srcDevLsbAmount,
    &IN_GT_U32_srcPortLsbAmount
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChCscdCentralizedChassisMappingDsaSrcLsbAmountSet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_U32_srcTrunkLsbAmount,
    &IN_GT_U32_srcPortLsbAmount,
    &IN_GT_U32_srcDevLsbAmount
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChCscdQosTcRemapTableSet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_U32_tc,
    &IN_CPSS_DXCH_CSCD_QOS_TC_REMAP_STC_PTR_tcMappingsPtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChCscdQosTcRemapTableGet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_U32_tc,
    &OUT_CPSS_DXCH_CSCD_QOS_TC_REMAP_STC_PTR_tcMappingsPtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChCscdCtrlQosSet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_U8_ctrlTc,
    &IN_CPSS_DP_LEVEL_ENT_ctrlDp,
    &IN_CPSS_DP_LEVEL_ENT_cpuToCpuDp
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChCscdRemapDataQosTblSet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_U8_tc,
    &IN_CPSS_DP_LEVEL_ENT_dp,
    &IN_GT_U8_remapTc,
    &IN_CPSS_DP_LEVEL_ENT_remapDp
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChCscdRemapDataQosTblGet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_U8_tc,
    &IN_CPSS_DP_LEVEL_ENT_dp,
    &OUT_GT_U8_PTR_remapTcPtr,
    &OUT_CPSS_DP_LEVEL_ENT_PTR_remapDpPtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChCscdDevMapLookupModeGet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &OUT_CPSS_DXCH_DEV_MAP_LOOKUP_MODE_ENT_PTR_modePtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChCscdDsaSrcDevFilterGet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &OUT_GT_BOOL_PTR_enableOwnDevFltrPtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChCscdCentralizedChassisMyPhysicalPortMappingDsaSrcLsbAmountGet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &OUT_GT_U32_PTR_srcDevLsbAmountPtr,
    &OUT_GT_U32_PTR_srcPortLsbAmountPtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChCscdCentralizedChassisMappingDsaSrcLsbAmountGet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &OUT_GT_U32_PTR_srcTrunkLsbAmountPtr,
    &OUT_GT_U32_PTR_srcPortLsbAmountPtr,
    &OUT_GT_U32_PTR_srcDevLsbAmountPtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChCscdCtrlQosGet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &OUT_GT_U8_PTR_ctrlTcPtr,
    &OUT_CPSS_DP_LEVEL_ENT_PTR_ctrlDpPtr,
    &OUT_CPSS_DP_LEVEL_ENT_PTR_cpuToCpuDpPtr
};


/********* lib API DB *********/

static PRV_CPSS_LOG_FUNC_ENTRY_STC prvCpssDxChCscdLogLibDb[] = {
    {"cpssDxChCscdPortTypeSet", 4, cpssDxChCscdPortTypeSet_PARAMS, NULL},
    {"cpssDxChCscdPortTypeGet", 4, cpssDxChCscdPortTypeGet_PARAMS, NULL},
    {"cpssDxChCscdDevMapTableSet", 7, cpssDxChCscdDevMapTableSet_PARAMS, NULL},
    {"cpssDxChCscdDevMapTableGet", 7, cpssDxChCscdDevMapTableGet_PARAMS, NULL},
    {"cpssDxChCscdRemapQosModeSet", 3, cpssDxChCscdRemapQosModeSet_PARAMS, NULL},
    {"cpssDxChCscdRemapQosModeGet", 3, cpssDxChCscdRemapQosModeGet_PARAMS, NULL},
    {"cpssDxChCscdCtrlQosSet", 4, cpssDxChCscdCtrlQosSet_PARAMS, NULL},
    {"cpssDxChCscdCtrlQosGet", 4, cpssDxChCscdCtrlQosGet_PARAMS, NULL},
    {"cpssDxChCscdRemapDataQosTblSet", 5, cpssDxChCscdRemapDataQosTblSet_PARAMS, NULL},
    {"cpssDxChCscdRemapDataQosTblGet", 5, cpssDxChCscdRemapDataQosTblGet_PARAMS, NULL},
    {"cpssDxChCscdDsaSrcDevFilterSet", 2, cpssDxChCscdDsaSrcDevFilterSet_PARAMS, NULL},
    {"cpssDxChCscdDsaSrcDevFilterGet", 2, cpssDxChCscdDsaSrcDevFilterGet_PARAMS, NULL},
    {"cpssDxChCscdHyperGInternalPortModeSet", 3, cpssDxChCscdHyperGInternalPortModeSet_PARAMS, NULL},
    {"cpssDxChCscdHyperGInternalPortModeGet", 3, cpssDxChCscdHyperGInternalPortModeGet_PARAMS, NULL},
    {"cpssDxChCscdHyperGPortCrcModeSet", 4, cpssDxChCscdHyperGPortCrcModeSet_PARAMS, NULL},
    {"cpssDxChCscdHyperGPortCrcModeGet", 4, cpssDxChCscdHyperGPortCrcModeGet_PARAMS, NULL},
    {"cpssDxChCscdFastFailoverFastStackRecoveryEnableSet", 2, prvCpssLogGenDevNumEnable_PARAMS, NULL},
    {"cpssDxChCscdFastFailoverFastStackRecoveryEnableGet", 2, prvCpssLogGenDevNumEnablePtr_PARAMS, NULL},
    {"cpssDxChCscdFastFailoverSecondaryTargetPortMapSet", 3, cpssDxChCscdFastFailoverSecondaryTargetPortMapSet_PARAMS, NULL},
    {"cpssDxChCscdFastFailoverSecondaryTargetPortMapGet", 3, cpssDxChCscdFastFailoverSecondaryTargetPortMapGet_PARAMS, NULL},
    {"cpssDxChCscdFastFailoverTerminateLocalLoopbackEnableSet", 2, prvCpssLogGenDevNumEnable_PARAMS, NULL},
    {"cpssDxChCscdFastFailoverTerminateLocalLoopbackEnableGet", 2, prvCpssLogGenDevNumEnablePtr_PARAMS, NULL},
    {"cpssDxChCscdFastFailoverPortIsLoopedSet", 3, cpssDxChCscdFastFailoverPortIsLoopedSet_PARAMS, NULL},
    {"cpssDxChCscdFastFailoverPortIsLoopedGet", 3, cpssDxChCscdFastFailoverPortIsLoopedGet_PARAMS, NULL},
    {"cpssDxChCscdQosPortTcRemapEnableSet", 3, prvCpssLogGenDevNumPortNumEnable2_PARAMS, NULL},
    {"cpssDxChCscdQosPortTcRemapEnableGet", 3, prvCpssLogGenDevNumPortNumEnablePtr2_PARAMS, NULL},
    {"cpssDxChCscdQosTcRemapTableSet", 3, cpssDxChCscdQosTcRemapTableSet_PARAMS, NULL},
    {"cpssDxChCscdQosTcRemapTableGet", 3, cpssDxChCscdQosTcRemapTableGet_PARAMS, NULL},
    {"cpssDxChCscdQosTcDpRemapTableSet", 4, cpssDxChCscdQosTcDpRemapTableSet_PARAMS, NULL},
    {"cpssDxChCscdQosTcDpRemapTableGet", 4, cpssDxChCscdQosTcDpRemapTableGet_PARAMS, NULL},
    {"cpssDxChCscdPortBridgeBypassEnableSet", 3, prvCpssLogGenDevNumPortNumEnable_PARAMS, NULL},
    {"cpssDxChCscdPortBridgeBypassEnableGet", 3, prvCpssLogGenDevNumPortNumEnablePtr_PARAMS, NULL},
    {"cpssDxChCscdOrigSrcPortFilterEnableSet", 2, prvCpssLogGenDevNumEnable_PARAMS, NULL},
    {"cpssDxChCscdOrigSrcPortFilterEnableGet", 2, prvCpssLogGenDevNumEnablePtr_PARAMS, NULL},
    {"cpssDxChCscdDevMapLookupModeSet", 2, cpssDxChCscdDevMapLookupModeSet_PARAMS, NULL},
    {"cpssDxChCscdDevMapLookupModeGet", 2, cpssDxChCscdDevMapLookupModeGet_PARAMS, NULL},
    {"cpssDxChCscdPortLocalDevMapLookupEnableSet", 4, cpssDxChCscdPortLocalDevMapLookupEnableSet_PARAMS, NULL},
    {"cpssDxChCscdPortLocalDevMapLookupEnableGet", 4, cpssDxChCscdPortLocalDevMapLookupEnableGet_PARAMS, NULL},
    {"cpssDxChCscdCentralizedChassisModeEnableSet", 3, prvCpssLogGenDevNumPortNumEnable2_PARAMS, NULL},
    {"cpssDxChCscdCentralizedChassisModeEnableGet", 3, prvCpssLogGenDevNumPortNumEnablePtr2_PARAMS, NULL},
    {"cpssDxChCscdCentralizedChassisReservedDevNumSet", 3, cpssDxChCscdCentralizedChassisReservedDevNumSet_PARAMS, NULL},
    {"cpssDxChCscdCentralizedChassisReservedDevNumGet", 3, cpssDxChCscdCentralizedChassisReservedDevNumGet_PARAMS, NULL},
    {"cpssDxChCscdCentralizedChassisSrcIdSet", 3, cpssDxChCscdCentralizedChassisSrcIdSet_PARAMS, NULL},
    {"cpssDxChCscdCentralizedChassisSrcIdGet", 3, cpssDxChCscdCentralizedChassisSrcIdGet_PARAMS, NULL},
    {"cpssDxChCscdDbRemoteHwDevNumModeSet", 2, cpssDxChCscdDbRemoteHwDevNumModeSet_PARAMS, NULL},
    {"cpssDxChCscdDbRemoteHwDevNumModeGet", 2, cpssDxChCscdDbRemoteHwDevNumModeGet_PARAMS, NULL},
    {"cpssDxChCscdCentralizedChassisLineCardDefaultEportBaseSet", 4, cpssDxChCscdCentralizedChassisLineCardDefaultEportBaseSet_PARAMS, NULL},
    {"cpssDxChCscdCentralizedChassisLineCardDefaultEportBaseGet", 4, cpssDxChCscdCentralizedChassisLineCardDefaultEportBaseGet_PARAMS, NULL},
    {"cpssDxChCscdCentralizedChassisMappingDsaSrcLsbAmountSet", 4, cpssDxChCscdCentralizedChassisMappingDsaSrcLsbAmountSet_PARAMS, NULL},
    {"cpssDxChCscdCentralizedChassisMappingDsaSrcLsbAmountGet", 4, cpssDxChCscdCentralizedChassisMappingDsaSrcLsbAmountGet_PARAMS, NULL},
    {"cpssDxChCscdPortMruCheckOnCascadeEnableSet", 3, prvCpssLogGenDevNumPortNumEnable2_PARAMS, NULL},
    {"cpssDxChCscdPortMruCheckOnCascadeEnableGet", 3, prvCpssLogGenDevNumPortNumEnablePtr2_PARAMS, NULL},
    {"cpssDxChCscdCentralizedChassisMyPhysicalPortMappingDsaSrcLsbAmountSet", 3, cpssDxChCscdCentralizedChassisMyPhysicalPortMappingDsaSrcLsbAmountSet_PARAMS, NULL},
    {"cpssDxChCscdCentralizedChassisMyPhysicalPortMappingDsaSrcLsbAmountGet", 3, cpssDxChCscdCentralizedChassisMyPhysicalPortMappingDsaSrcLsbAmountGet_PARAMS, NULL},
    {"cpssDxChCscdCentralizedChassisMyPhysicalPortAssignmentModeSet", 3, cpssDxChCscdCentralizedChassisMyPhysicalPortAssignmentModeSet_PARAMS, NULL},
    {"cpssDxChCscdCentralizedChassisMyPhysicalPortAssignmentModeGet", 3, cpssDxChCscdCentralizedChassisMyPhysicalPortAssignmentModeGet_PARAMS, NULL},
    {"cpssDxChCscdCentralizedChassisMyPhysicalPortBaseSet", 3, cpssDxChCscdCentralizedChassisMyPhysicalPortBaseSet_PARAMS, NULL},
    {"cpssDxChCscdCentralizedChassisMyPhysicalPortBaseGet", 3, cpssDxChCscdCentralizedChassisMyPhysicalPortBaseGet_PARAMS, NULL},
    {"cpssDxChCscdCentralizedChassisMyPhysicalPortAssignSrcDevEnableSet", 3, prvCpssLogGenDevNumPortNumEnable2_PARAMS, NULL},
    {"cpssDxChCscdCentralizedChassisMyPhysicalPortAssignSrcDevEnableGet", 3, prvCpssLogGenDevNumPortNumEnablePtr2_PARAMS, NULL},
    {"cpssDxChCscdPortStackAggregationEnableSet", 3, prvCpssLogGenDevNumPortNumEnable2_PARAMS, NULL},
    {"cpssDxChCscdPortStackAggregationEnableGet", 3, prvCpssLogGenDevNumPortNumEnablePtr2_PARAMS, NULL},
    {"cpssDxChCscdPortStackAggregationConfigSet", 5, cpssDxChCscdPortStackAggregationConfigSet_PARAMS, NULL},
    {"cpssDxChCscdPortStackAggregationConfigGet", 5, cpssDxChCscdPortStackAggregationConfigGet_PARAMS, NULL},
    {"cpssDxChCscdCentralizedChassisRemotePhysicalPortMapEnableSet", 3, prvCpssLogGenDevNumPortNumEnable2_PARAMS, NULL},
    {"cpssDxChCscdCentralizedChassisRemotePhysicalPortMapEnableGet", 3, prvCpssLogGenDevNumPortNumEnablePtr2_PARAMS, NULL},
    {"cpssDxChCscdCentralizedChassisRemotePhysicalPortMapSet", 4, cpssDxChCscdCentralizedChassisRemotePhysicalPortMapSet_PARAMS, NULL},
    {"cpssDxChCscdCentralizedChassisRemotePhysicalPortMapGet", 4, cpssDxChCscdCentralizedChassisRemotePhysicalPortMapGet_PARAMS, NULL},
    {"cpssDxChCscdPortQosDsaModeSet", 3, cpssDxChCscdPortQosDsaModeSet_PARAMS, NULL},
    {"cpssDxChCscdPortQosDsaModeGet", 3, cpssDxChCscdPortQosDsaModeGet_PARAMS, NULL},
    {"cpssDxChCscdHashInDsaEnableSet", 4, prvCpssLogGenDevNumPortNumDirectionEnable_PARAMS, NULL},
    {"cpssDxChCscdHashInDsaEnableGet", 4, prvCpssLogGenDevNumPortNumDirectionEnablePtr_PARAMS, NULL},
};

/******** DB Access Function ********/
void prvCpssLogParamLibDbGet_CPSS_LOG_LIB_CSCD(
    OUT PRV_CPSS_LOG_FUNC_ENTRY_STC ** logFuncDbPtrPtr,
    OUT GT_U32 * logFuncDbSizePtr
)
{
    *logFuncDbPtrPtr = prvCpssDxChCscdLogLibDb;
    *logFuncDbSizePtr = sizeof(prvCpssDxChCscdLogLibDb) / sizeof(PRV_CPSS_LOG_FUNC_ENTRY_STC);
}

