/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* prvCpssGenPpTypesLog.c
*       WARNING!!! this is a generated file, please don't edit it manually
* COMMENTS:
*
* FILE REVISION NUMBER:
*       $Revision: 1 $
*******************************************************************************/
#include <cpss/generic/log/cpssLog.h>
#include <cpss/generic/log/prvCpssLog.h>
#include <cpss/extServices/private/prvCpssBindFunc.h>
#include <cpss/generic/cpssCommonDefs.h>
#include <cpss/generic/cpssTypes.h>
#include <cpss/generic/log/prvCpssGenCommonTypesLog.h>
#include <cpss/generic/log/prvCpssGenPpTypesLog.h>


/********* enums *********/

char * prvCpssLogEnum_CPSS_ADJUST_OPERATION_ENT[]  =
{
    "CPSS_ADJUST_OPERATION_ADD_E",
    "CPSS_ADJUST_OPERATION_SUBSTRUCT_E"
};
PRV_CPSS_LOG_STC_ENUM_ARRAY_SIZE_MAC(CPSS_ADJUST_OPERATION_ENT);
char * prvCpssLogEnum_CPSS_COMPARE_OPERATOR_ENT[]  =
{
    "CPSS_COMPARE_OPERATOR_INVALID_E",
    "CPSS_COMPARE_OPERATOR_LTE",
    "CPSS_COMPARE_OPERATOR_GTE",
    "CPSS_COMPARE_OPERATOR_NEQ"
};
PRV_CPSS_LOG_STC_ENUM_ARRAY_SIZE_MAC(CPSS_COMPARE_OPERATOR_ENT);
char * prvCpssLogEnum_CPSS_DIRECTION_ENT[]  =
{
    "CPSS_DIRECTION_INGRESS_E",
    "CPSS_DIRECTION_EGRESS_E",
    "CPSS_DIRECTION_BOTH_E"
};
PRV_CPSS_LOG_STC_ENUM_ARRAY_SIZE_MAC(CPSS_DIRECTION_ENT);
char * prvCpssLogEnum_CPSS_DROP_MODE_TYPE_ENT[]  =
{
    "CPSS_DROP_MODE_SOFT_E",
    "CPSS_DROP_MODE_HARD_E"
};
PRV_CPSS_LOG_STC_ENUM_ARRAY_SIZE_MAC(CPSS_DROP_MODE_TYPE_ENT);
char * prvCpssLogEnum_CPSS_IP_PROTOCOL_STACK_ENT[]  =
{
    "CPSS_IP_PROTOCOL_IPV4_E",
    "CPSS_IP_PROTOCOL_IPV6_E",
    "CPSS_IP_PROTOCOL_IPV4V6_E"
};
PRV_CPSS_LOG_STC_ENUM_ARRAY_SIZE_MAC(CPSS_IP_PROTOCOL_STACK_ENT);
PRV_CPSS_ENUM_STRING_VALUE_PAIR_STC prvCpssLogEnum_map_CPSS_L4_PROTOCOL_ENT[]  =
{PRV_CPSS_LOG_ENUM_NAME_AND_VALUE_MAC(CPSS_L4_PROTOCOL_TCP_E),
PRV_CPSS_LOG_ENUM_NAME_AND_VALUE_MAC(CPSS_L4_PROTOCOL_UDP_E)
};
PRV_CPSS_LOG_STC_ENUM_MAP_ARRAY_SIZE_MAC(CPSS_L4_PROTOCOL_ENT);
char * prvCpssLogEnum_CPSS_L4_PROTOCOL_PORT_TYPE_ENT[]  =
{
    "CPSS_L4_PROTOCOL_PORT_SRC_E",
    "CPSS_L4_PROTOCOL_PORT_DST_E"
};
PRV_CPSS_LOG_STC_ENUM_ARRAY_SIZE_MAC(CPSS_L4_PROTOCOL_PORT_TYPE_ENT);
char * prvCpssLogEnum_CPSS_PACKET_ATTRIBUTE_ASSIGN_PRECEDENCE_ENT[]  =
{
    "CPSS_PACKET_ATTRIBUTE_ASSIGN_PRECEDENCE_SOFT_E",
    "CPSS_PACKET_ATTRIBUTE_ASSIGN_PRECEDENCE_HARD_E"
};
PRV_CPSS_LOG_STC_ENUM_ARRAY_SIZE_MAC(CPSS_PACKET_ATTRIBUTE_ASSIGN_PRECEDENCE_ENT);
char * prvCpssLogEnum_CPSS_PACKET_CMD_ENT[]  =
{
    "CPSS_PACKET_CMD_FORWARD_E",
    "CPSS_PACKET_CMD_MIRROR_TO_CPU_E",
    "CPSS_PACKET_CMD_TRAP_TO_CPU_E",
    "CPSS_PACKET_CMD_DROP_HARD_E",
    "CPSS_PACKET_CMD_DROP_SOFT_E",
    "CPSS_PACKET_CMD_ROUTE_E",
    "CPSS_PACKET_CMD_ROUTE_AND_MIRROR_E",
    "CPSS_PACKET_CMD_BRIDGE_AND_MIRROR_E",
    "CPSS_PACKET_CMD_BRIDGE_E",
    "CPSS_PACKET_CMD_NONE_E",
    "CPSS_PACKET_CMD_LOOPBACK_E"
};
PRV_CPSS_LOG_STC_ENUM_ARRAY_SIZE_MAC(CPSS_PACKET_CMD_ENT);
char * prvCpssLogEnum_CPSS_UNICAST_MULTICAST_ENT[]  =
{
    "CPSS_UNICAST_E",
    "CPSS_MULTICAST_E",
    "CPSS_UNICAST_MULTICAST_E"
};
PRV_CPSS_LOG_STC_ENUM_ARRAY_SIZE_MAC(CPSS_UNICAST_MULTICAST_ENT);
char * prvCpssLogEnum_CPSS_INTERFACE_TYPE_ENT[]  =
{
    "CPSS_INTERFACE_PORT_E",
    "CPSS_INTERFACE_TRUNK_E",
    "CPSS_INTERFACE_VIDX_E",
    "CPSS_INTERFACE_VID_E",
    "CPSS_INTERFACE_DEVICE_E",
    "CPSS_INTERFACE_FABRIC_VIDX_E",
    "CPSS_INTERFACE_INDEX_E"
};
PRV_CPSS_LOG_STC_ENUM_ARRAY_SIZE_MAC(CPSS_INTERFACE_TYPE_ENT);
char * prvCpssLogEnum_CPSS_PACKET_ATTRIBUTE_ASSIGN_CMD_ENT[]  =
{
    "CPSS_PACKET_ATTRIBUTE_ASSIGN_DISABLED_E",
    "CPSS_PACKET_ATTRIBUTE_ASSIGN_FOR_TAGGED_E",
    "CPSS_PACKET_ATTRIBUTE_ASSIGN_FOR_UNTAGGED_E",
    "CPSS_PACKET_ATTRIBUTE_ASSIGN_FOR_ALL_E"
};
PRV_CPSS_LOG_STC_ENUM_ARRAY_SIZE_MAC(CPSS_PACKET_ATTRIBUTE_ASSIGN_CMD_ENT);
char * prvCpssLogEnum_CPSS_PACKET_ATTRIBUTE_MODIFY_TYPE_ENT[]  =
{
    "CPSS_PACKET_ATTRIBUTE_MODIFY_KEEP_PREVIOUS_E",
    "CPSS_PACKET_ATTRIBUTE_MODIFY_DISABLE_E",
    "CPSS_PACKET_ATTRIBUTE_MODIFY_ENABLE_E"
};
PRV_CPSS_LOG_STC_ENUM_ARRAY_SIZE_MAC(CPSS_PACKET_ATTRIBUTE_MODIFY_TYPE_ENT);
PRV_CPSS_ENUM_STRING_VALUE_PAIR_STC prvCpssLogEnum_map_CPSS_PP_FAMILY_TYPE_ENT[]  =
{PRV_CPSS_LOG_ENUM_NAME_AND_VALUE_MAC(CPSS_PP_FAMILY_START_EXMX_E),
PRV_CPSS_LOG_ENUM_NAME_AND_VALUE_MAC(CPSS_PP_FAMILY_TWISTC_E),
PRV_CPSS_LOG_ENUM_NAME_AND_VALUE_MAC(CPSS_PP_FAMILY_TWISTD_E),
PRV_CPSS_LOG_ENUM_NAME_AND_VALUE_MAC(CPSS_PP_FAMILY_SAMBA_E),
PRV_CPSS_LOG_ENUM_NAME_AND_VALUE_MAC(CPSS_PP_FAMILY_TIGER_E),
PRV_CPSS_LOG_ENUM_NAME_AND_VALUE_MAC(CPSS_PP_FAMILY_END_EXMX_E),
PRV_CPSS_LOG_ENUM_NAME_AND_VALUE_MAC(CPSS_PP_FAMILY_START_DXSAL_E),
PRV_CPSS_LOG_ENUM_NAME_AND_VALUE_MAC(CPSS_PP_FAMILY_SALSA_E),
PRV_CPSS_LOG_ENUM_NAME_AND_VALUE_MAC(CPSS_PP_FAMILY_END_DXSAL_E),
PRV_CPSS_LOG_ENUM_NAME_AND_VALUE_MAC(CPSS_PP_FAMILY_START_DXCH_E),
PRV_CPSS_LOG_ENUM_NAME_AND_VALUE_MAC(CPSS_PP_FAMILY_CHEETAH_E),
PRV_CPSS_LOG_ENUM_NAME_AND_VALUE_MAC(CPSS_PP_FAMILY_CHEETAH2_E),
PRV_CPSS_LOG_ENUM_NAME_AND_VALUE_MAC(CPSS_PP_FAMILY_CHEETAH3_E),
PRV_CPSS_LOG_ENUM_NAME_AND_VALUE_MAC(CPSS_PP_FAMILY_DXCH_XCAT_E),
PRV_CPSS_LOG_ENUM_NAME_AND_VALUE_MAC(CPSS_PP_FAMILY_DXCH_XCAT3_E),
PRV_CPSS_LOG_ENUM_NAME_AND_VALUE_MAC(CPSS_PP_FAMILY_DXCH_LION_E),
PRV_CPSS_LOG_ENUM_NAME_AND_VALUE_MAC(CPSS_PP_FAMILY_DXCH_XCAT2_E),
PRV_CPSS_LOG_ENUM_NAME_AND_VALUE_MAC(CPSS_PP_FAMILY_DXCH_LION2_E),
PRV_CPSS_LOG_ENUM_NAME_AND_VALUE_MAC(CPSS_PP_FAMILY_DXCH_LION3_E),
PRV_CPSS_LOG_ENUM_NAME_AND_VALUE_MAC(CPSS_PP_FAMILY_DXCH_BOBCAT2_E),
PRV_CPSS_LOG_ENUM_NAME_AND_VALUE_MAC(CPSS_PP_FAMILY_DXCH_BOBCAT3_E),
PRV_CPSS_LOG_ENUM_NAME_AND_VALUE_MAC(CPSS_PP_FAMILY_DXCH_ALDRIN_E),
PRV_CPSS_LOG_ENUM_NAME_AND_VALUE_MAC(CPSS_PP_FAMILY_END_DXCH_E),
PRV_CPSS_LOG_ENUM_NAME_AND_VALUE_MAC(CPSS_PP_FAMILY_START_EXMXPM_E),
PRV_CPSS_LOG_ENUM_NAME_AND_VALUE_MAC(CPSS_PP_FAMILY_PUMA_E),
PRV_CPSS_LOG_ENUM_NAME_AND_VALUE_MAC(CPSS_PP_FAMILY_PUMA3_E),
PRV_CPSS_LOG_ENUM_NAME_AND_VALUE_MAC(CPSS_PP_FAMILY_END_EXMXPM_E),
PRV_CPSS_LOG_ENUM_NAME_AND_VALUE_MAC(CPSS_PP_FAMILY_LAST_E),
PRV_CPSS_LOG_ENUM_NAME_AND_VALUE_MAC(CPSS_MAX_FAMILY)
};
PRV_CPSS_LOG_STC_ENUM_MAP_ARRAY_SIZE_MAC(CPSS_PP_FAMILY_TYPE_ENT);
PRV_CPSS_ENUM_STRING_VALUE_PAIR_STC prvCpssLogEnum_map_CPSS_PP_SUB_FAMILY_TYPE_ENT[]  =
{PRV_CPSS_LOG_ENUM_NAME_AND_VALUE_MAC(CPSS_PP_SUB_FAMILY_NONE_E),
PRV_CPSS_LOG_ENUM_NAME_AND_VALUE_MAC(CPSS_PP_SUB_FAMILY_BOBCAT2_BOBK_E),
PRV_CPSS_LOG_ENUM_NAME_AND_VALUE_MAC(CPSS_MAX_SUB_FAMILY),
PRV_CPSS_LOG_ENUM_NAME_AND_VALUE_MAC(CPSS_BAD_SUB_FAMILY)
};
PRV_CPSS_LOG_STC_ENUM_MAP_ARRAY_SIZE_MAC(CPSS_PP_SUB_FAMILY_TYPE_ENT);


/********* structure fields log functions *********/

void prvCpssLogParamFuncStc_CPSS_RESOURCE_MAPPING_STC_PTR
(
    IN CPSS_LOG_LIB_ENT         contextLib,
    IN CPSS_LOG_TYPE_ENT        logType,
    IN GT_CHAR_PTR              namePtr,
    IN void                   * fieldPtr,
    INOUT PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC     * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_START_FIELD_STC_MAC(CPSS_RESOURCE_MAPPING_STC *, valPtr);
    PRV_CPSS_LOG_UNUSED_MAC(inOutParamInfoPtr);
    PRV_CPSS_LOG_STC_UINTPTR_MAC(valPtr, sramBaseAddress);
    PRV_CPSS_LOG_STC_NUMBER_MAC(valPtr, sramSize);
    prvCpssLogStcLogEnd(contextLib, logType);
}
void prvCpssLogParamFuncStc_CPSS_HW_ADDR_STC_PTR
(
    IN CPSS_LOG_LIB_ENT         contextLib,
    IN CPSS_LOG_TYPE_ENT        logType,
    IN GT_CHAR_PTR              namePtr,
    IN void                   * fieldPtr,
    INOUT PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC     * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_START_FIELD_STC_MAC(CPSS_HW_ADDR_STC *, valPtr);
    PRV_CPSS_LOG_UNUSED_MAC(inOutParamInfoPtr);
    PRV_CPSS_LOG_STC_NUMBER_MAC(valPtr, busNo);
    PRV_CPSS_LOG_STC_NUMBER_MAC(valPtr, devSel);
    PRV_CPSS_LOG_STC_NUMBER_MAC(valPtr, funcNo);
    prvCpssLogStcLogEnd(contextLib, logType);
}


/********* parameters log functions *********/

void prvCpssLogParamFunc_CPSS_ADJUST_OPERATION_ENT(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_SET_PARAM_VAL_MAC(CPSS_ADJUST_OPERATION_ENT, paramVal);
    PRV_CPSS_LOG_ENUM_MAC(namePtr, paramVal, CPSS_ADJUST_OPERATION_ENT);
}
void prvCpssLogParamFunc_CPSS_ADJUST_OPERATION_ENT_PTR(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_SET_PARAM_VAL_MAC(CPSS_ADJUST_OPERATION_ENT*, paramVal);
    if (paramVal == NULL)
    {
        PRV_CPSS_LOG_AND_HISTORY_PARAM_MAC(contextLib, logType, "%s = NULL\n", namePtr);
        return;
    }
    PRV_CPSS_LOG_ENUM_MAC(namePtr, *paramVal, CPSS_ADJUST_OPERATION_ENT);
}
void prvCpssLogParamFunc_CPSS_COMPARE_OPERATOR_ENT(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_SET_PARAM_VAL_MAC(CPSS_COMPARE_OPERATOR_ENT, paramVal);
    PRV_CPSS_LOG_ENUM_MAC(namePtr, paramVal, CPSS_COMPARE_OPERATOR_ENT);
}
void prvCpssLogParamFunc_CPSS_COMPARE_OPERATOR_ENT_PTR(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_SET_PARAM_VAL_MAC(CPSS_COMPARE_OPERATOR_ENT*, paramVal);
    if (paramVal == NULL)
    {
        PRV_CPSS_LOG_AND_HISTORY_PARAM_MAC(contextLib, logType, "%s = NULL\n", namePtr);
        return;
    }
    PRV_CPSS_LOG_ENUM_MAC(namePtr, *paramVal, CPSS_COMPARE_OPERATOR_ENT);
}
void prvCpssLogParamFunc_CPSS_DIRECTION_ENT(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_SET_PARAM_VAL_MAC(CPSS_DIRECTION_ENT, paramVal);
    PRV_CPSS_LOG_ENUM_MAC(namePtr, paramVal, CPSS_DIRECTION_ENT);
}
void prvCpssLogParamFunc_CPSS_DIRECTION_ENT_PTR(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_SET_PARAM_VAL_MAC(CPSS_DIRECTION_ENT*, paramVal);
    if (paramVal == NULL)
    {
        PRV_CPSS_LOG_AND_HISTORY_PARAM_MAC(contextLib, logType, "%s = NULL\n", namePtr);
        return;
    }
    PRV_CPSS_LOG_ENUM_MAC(namePtr, *paramVal, CPSS_DIRECTION_ENT);
}
void prvCpssLogParamFunc_CPSS_DROP_MODE_TYPE_ENT(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_SET_PARAM_VAL_MAC(CPSS_DROP_MODE_TYPE_ENT, paramVal);
    PRV_CPSS_LOG_ENUM_MAC(namePtr, paramVal, CPSS_DROP_MODE_TYPE_ENT);
}
void prvCpssLogParamFunc_CPSS_DROP_MODE_TYPE_ENT_PTR(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_SET_PARAM_VAL_MAC(CPSS_DROP_MODE_TYPE_ENT*, paramVal);
    if (paramVal == NULL)
    {
        PRV_CPSS_LOG_AND_HISTORY_PARAM_MAC(contextLib, logType, "%s = NULL\n", namePtr);
        return;
    }
    PRV_CPSS_LOG_ENUM_MAC(namePtr, *paramVal, CPSS_DROP_MODE_TYPE_ENT);
}
void prvCpssLogParamFunc_CPSS_INTERFACE_INFO_STC(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_SET_PARAM_VAL_MAC(CPSS_INTERFACE_INFO_STC, paramVal);
    prvCpssLogParamFuncStc_CPSS_INTERFACE_INFO_STC_PTR(contextLib, logType, namePtr, &paramVal, inOutParamInfoPtr);
}
void prvCpssLogParamFunc_CPSS_INTERFACE_INFO_STC_PTR(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_START_PARAM_STC_MAC(CPSS_INTERFACE_INFO_STC*, paramVal);
    prvCpssLogParamFuncStc_CPSS_INTERFACE_INFO_STC_PTR(contextLib, logType, namePtr, paramVal, inOutParamInfoPtr);
}
void prvCpssLogParamFunc_CPSS_IP_PROTOCOL_STACK_ENT(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_SET_PARAM_VAL_MAC(CPSS_IP_PROTOCOL_STACK_ENT, paramVal);
    PRV_CPSS_LOG_ENUM_MAC(namePtr, paramVal, CPSS_IP_PROTOCOL_STACK_ENT);
}
void prvCpssLogParamFunc_CPSS_IP_PROTOCOL_STACK_ENT_PTR(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_SET_PARAM_VAL_MAC(CPSS_IP_PROTOCOL_STACK_ENT*, paramVal);
    if (paramVal == NULL)
    {
        PRV_CPSS_LOG_AND_HISTORY_PARAM_MAC(contextLib, logType, "%s = NULL\n", namePtr);
        return;
    }
    PRV_CPSS_LOG_ENUM_MAC(namePtr, *paramVal, CPSS_IP_PROTOCOL_STACK_ENT);
}
void prvCpssLogParamFunc_CPSS_L4_PROTOCOL_ENT(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_SET_PARAM_VAL_MAC(CPSS_L4_PROTOCOL_ENT, paramVal);
    PRV_CPSS_LOG_ENUM_MAP_MAC(namePtr, paramVal, CPSS_L4_PROTOCOL_ENT);
}
void prvCpssLogParamFunc_CPSS_L4_PROTOCOL_PORT_TYPE_ENT(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_SET_PARAM_VAL_MAC(CPSS_L4_PROTOCOL_PORT_TYPE_ENT, paramVal);
    PRV_CPSS_LOG_ENUM_MAC(namePtr, paramVal, CPSS_L4_PROTOCOL_PORT_TYPE_ENT);
}
void prvCpssLogParamFunc_CPSS_L4_PROTOCOL_PORT_TYPE_ENT_PTR(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_SET_PARAM_VAL_MAC(CPSS_L4_PROTOCOL_PORT_TYPE_ENT*, paramVal);
    if (paramVal == NULL)
    {
        PRV_CPSS_LOG_AND_HISTORY_PARAM_MAC(contextLib, logType, "%s = NULL\n", namePtr);
        return;
    }
    PRV_CPSS_LOG_ENUM_MAC(namePtr, *paramVal, CPSS_L4_PROTOCOL_PORT_TYPE_ENT);
}
void prvCpssLogParamFunc_CPSS_PACKET_ATTRIBUTE_ASSIGN_PRECEDENCE_ENT(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_SET_PARAM_VAL_MAC(CPSS_PACKET_ATTRIBUTE_ASSIGN_PRECEDENCE_ENT, paramVal);
    PRV_CPSS_LOG_ENUM_MAC(namePtr, paramVal, CPSS_PACKET_ATTRIBUTE_ASSIGN_PRECEDENCE_ENT);
}
void prvCpssLogParamFunc_CPSS_PACKET_ATTRIBUTE_ASSIGN_PRECEDENCE_ENT_PTR(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_SET_PARAM_VAL_MAC(CPSS_PACKET_ATTRIBUTE_ASSIGN_PRECEDENCE_ENT*, paramVal);
    if (paramVal == NULL)
    {
        PRV_CPSS_LOG_AND_HISTORY_PARAM_MAC(contextLib, logType, "%s = NULL\n", namePtr);
        return;
    }
    PRV_CPSS_LOG_ENUM_MAC(namePtr, *paramVal, CPSS_PACKET_ATTRIBUTE_ASSIGN_PRECEDENCE_ENT);
}
void prvCpssLogParamFunc_CPSS_PACKET_CMD_ENT(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_SET_PARAM_VAL_MAC(CPSS_PACKET_CMD_ENT, paramVal);
    PRV_CPSS_LOG_ENUM_MAC(namePtr, paramVal, CPSS_PACKET_CMD_ENT);
}
void prvCpssLogParamFunc_CPSS_PACKET_CMD_ENT_PTR(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_SET_PARAM_VAL_MAC(CPSS_PACKET_CMD_ENT*, paramVal);
    if (paramVal == NULL)
    {
        PRV_CPSS_LOG_AND_HISTORY_PARAM_MAC(contextLib, logType, "%s = NULL\n", namePtr);
        return;
    }
    PRV_CPSS_LOG_ENUM_MAC(namePtr, *paramVal, CPSS_PACKET_CMD_ENT);
}
void prvCpssLogParamFunc_CPSS_PORTS_BMP_STC(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_SET_PARAM_VAL_MAC(CPSS_PORTS_BMP_STC, paramVal);
    prvCpssLogParamFuncStc_CPSS_PORTS_BMP_STC_PTR(contextLib, logType, namePtr, &paramVal, inOutParamInfoPtr);
}
void prvCpssLogParamFunc_CPSS_PORTS_BMP_STC_PTR(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_START_PARAM_STC_MAC(CPSS_PORTS_BMP_STC*, paramVal);
    prvCpssLogParamFuncStc_CPSS_PORTS_BMP_STC_PTR(contextLib, logType, namePtr, paramVal, inOutParamInfoPtr);
}
void prvCpssLogParamFunc_CPSS_PP_DEVICE_TYPE(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
)
{
    char * formatPtr = "%s = %d\n";
    PRV_CPSS_LOG_SET_PARAM_VAL_MAC(CPSS_PP_DEVICE_TYPE, paramVal);
    if (inOutParamInfoPtr->formatPtr)
    {
        formatPtr = inOutParamInfoPtr->formatPtr;
    }
    PRV_CPSS_LOG_AND_HISTORY_PARAM_MAC(contextLib, logType, formatPtr, namePtr, paramVal);
}
void prvCpssLogParamFunc_CPSS_PP_DEVICE_TYPE_PTR(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
)
{
    char * formatPtr = "%s = %d\n";
    PRV_CPSS_LOG_SET_PARAM_VAL_MAC(CPSS_PP_DEVICE_TYPE*, paramVal);
    if (paramVal == NULL)
    {
        PRV_CPSS_LOG_AND_HISTORY_PARAM_MAC(contextLib, logType, "%s = NULL\n", namePtr);
        return;
    }
    if (inOutParamInfoPtr->formatPtr)
    {
        formatPtr = inOutParamInfoPtr->formatPtr;
    }
    PRV_CPSS_LOG_AND_HISTORY_PARAM_MAC(contextLib, logType, formatPtr, namePtr, *paramVal);
}
void prvCpssLogParamFunc_CPSS_RESOURCE_MAPPING_STC_PTR(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_START_PARAM_STC_MAC(CPSS_RESOURCE_MAPPING_STC*, paramVal);
    prvCpssLogParamFuncStc_CPSS_RESOURCE_MAPPING_STC_PTR(contextLib, logType, namePtr, paramVal, inOutParamInfoPtr);
}
void prvCpssLogParamFunc_CPSS_UNICAST_MULTICAST_ENT(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_SET_PARAM_VAL_MAC(CPSS_UNICAST_MULTICAST_ENT, paramVal);
    PRV_CPSS_LOG_ENUM_MAC(namePtr, paramVal, CPSS_UNICAST_MULTICAST_ENT);
}
void prvCpssLogParamFunc_GT_ETHERADDR(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_SET_PARAM_VAL_MAC(GT_ETHERADDR, paramVal);
    prvCpssLogParamFuncStc_GT_ETHERADDR_PTR(contextLib, logType, namePtr, &paramVal, inOutParamInfoPtr);
}
void prvCpssLogParamFunc_GT_ETHERADDR_PTR(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_START_PARAM_STC_MAC(GT_ETHERADDR*, paramVal);
    prvCpssLogParamFuncStc_GT_ETHERADDR_PTR(contextLib, logType, namePtr, paramVal, inOutParamInfoPtr);
}
void prvCpssLogParamFunc_GT_HW_DEV_NUM(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
)
{
    char * formatPtr = "%s = %d\n";
    PRV_CPSS_LOG_SET_PARAM_VAL_MAC(GT_HW_DEV_NUM, paramVal);
    if (inOutParamInfoPtr->formatPtr)
    {
        formatPtr = inOutParamInfoPtr->formatPtr;
    }
    PRV_CPSS_LOG_AND_HISTORY_PARAM_MAC(contextLib, logType, formatPtr, namePtr, paramVal);
}
void prvCpssLogParamFunc_GT_HW_DEV_NUM_PTR(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
)
{
    char * formatPtr = "%s = %d\n";
    PRV_CPSS_LOG_SET_PARAM_VAL_MAC(GT_HW_DEV_NUM*, paramVal);
    if (paramVal == NULL)
    {
        PRV_CPSS_LOG_AND_HISTORY_PARAM_MAC(contextLib, logType, "%s = NULL\n", namePtr);
        return;
    }
    if (inOutParamInfoPtr->formatPtr)
    {
        formatPtr = inOutParamInfoPtr->formatPtr;
    }
    PRV_CPSS_LOG_AND_HISTORY_PARAM_MAC(contextLib, logType, formatPtr, namePtr, *paramVal);
}
void prvCpssLogParamFunc_GT_IPADDR(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_SET_PARAM_VAL_MAC(GT_IPADDR, paramVal);
    prvCpssLogParamFuncStc_GT_IPADDR_PTR(contextLib, logType, namePtr, &paramVal, inOutParamInfoPtr);
}
void prvCpssLogParamFunc_GT_IPADDR_PTR(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_START_PARAM_STC_MAC(GT_IPADDR*, paramVal);
    prvCpssLogParamFuncStc_GT_IPADDR_PTR(contextLib, logType, namePtr, paramVal, inOutParamInfoPtr);
}
void prvCpssLogParamFunc_GT_IPV6ADDR(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_SET_PARAM_VAL_MAC(GT_IPV6ADDR, paramVal);
    prvCpssLogParamFuncStc_GT_IPV6ADDR_PTR(contextLib, logType, namePtr, &paramVal, inOutParamInfoPtr);
}
void prvCpssLogParamFunc_GT_IPV6ADDR_PTR(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_START_PARAM_STC_MAC(GT_IPV6ADDR*, paramVal);
    prvCpssLogParamFuncStc_GT_IPV6ADDR_PTR(contextLib, logType, namePtr, paramVal, inOutParamInfoPtr);
}
void prvCpssLogParamFunc_GT_PHYSICAL_PORT_NUM(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
)
{
    char * formatPtr = "%s = %d\n";
    PRV_CPSS_LOG_SET_PARAM_VAL_MAC(GT_PHYSICAL_PORT_NUM, paramVal);
    if (inOutParamInfoPtr->formatPtr)
    {
        formatPtr = inOutParamInfoPtr->formatPtr;
    }
    PRV_CPSS_LOG_AND_HISTORY_PARAM_MAC(contextLib, logType, formatPtr, namePtr, paramVal);
}
void prvCpssLogParamFunc_GT_PHYSICAL_PORT_NUM_PTR(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
)
{
    char * formatPtr = "%s = %d\n";
    PRV_CPSS_LOG_SET_PARAM_VAL_MAC(GT_PHYSICAL_PORT_NUM*, paramVal);
    if (paramVal == NULL)
    {
        PRV_CPSS_LOG_AND_HISTORY_PARAM_MAC(contextLib, logType, "%s = NULL\n", namePtr);
        return;
    }
    if (inOutParamInfoPtr->formatPtr)
    {
        formatPtr = inOutParamInfoPtr->formatPtr;
    }
    PRV_CPSS_LOG_AND_HISTORY_PARAM_MAC(contextLib, logType, formatPtr, namePtr, *paramVal);
}
void prvCpssLogParamFunc_GT_PORT_GROUPS_BMP_PTR(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
)
{
    char * formatPtr = "%s = %d\n";
    PRV_CPSS_LOG_SET_PARAM_VAL_MAC(GT_PORT_GROUPS_BMP*, paramVal);
    if (paramVal == NULL)
    {
        PRV_CPSS_LOG_AND_HISTORY_PARAM_MAC(contextLib, logType, "%s = NULL\n", namePtr);
        return;
    }
    if (inOutParamInfoPtr->formatPtr)
    {
        formatPtr = inOutParamInfoPtr->formatPtr;
    }
    PRV_CPSS_LOG_AND_HISTORY_PARAM_MAC(contextLib, logType, formatPtr, namePtr, *paramVal);
}
void prvCpssLogParamFunc_GT_PORT_NUM(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
)
{
    char * formatPtr = "%s = %d\n";
    PRV_CPSS_LOG_SET_PARAM_VAL_MAC(GT_PORT_NUM, paramVal);
    if (inOutParamInfoPtr->formatPtr)
    {
        formatPtr = inOutParamInfoPtr->formatPtr;
    }
    PRV_CPSS_LOG_AND_HISTORY_PARAM_MAC(contextLib, logType, formatPtr, namePtr, paramVal);
}
void prvCpssLogParamFunc_GT_PORT_NUM_PTR(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
)
{
    char * formatPtr = "%s = %d\n";
    PRV_CPSS_LOG_SET_PARAM_VAL_MAC(GT_PORT_NUM*, paramVal);
    if (paramVal == NULL)
    {
        PRV_CPSS_LOG_AND_HISTORY_PARAM_MAC(contextLib, logType, "%s = NULL\n", namePtr);
        return;
    }
    if (inOutParamInfoPtr->formatPtr)
    {
        formatPtr = inOutParamInfoPtr->formatPtr;
    }
    PRV_CPSS_LOG_AND_HISTORY_PARAM_MAC(contextLib, logType, formatPtr, namePtr, *paramVal);
}
void prvCpssLogParamFunc_GT_TRUNK_ID(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
)
{
    char * formatPtr = "%s = %d\n";
    PRV_CPSS_LOG_SET_PARAM_VAL_MAC(int, paramVal);
    if (inOutParamInfoPtr->formatPtr)
    {
        formatPtr = inOutParamInfoPtr->formatPtr;
    }
    PRV_CPSS_LOG_AND_HISTORY_PARAM_MAC(contextLib, logType, formatPtr, namePtr, paramVal);
}
void prvCpssLogParamFunc_GT_TRUNK_ID_PTR(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
)
{
    char * formatPtr = "%s = %d\n";
    PRV_CPSS_LOG_SET_PARAM_VAL_MAC(GT_TRUNK_ID*, paramVal);
    if (paramVal == NULL)
    {
        PRV_CPSS_LOG_AND_HISTORY_PARAM_MAC(contextLib, logType, "%s = NULL\n", namePtr);
        return;
    }
    if (inOutParamInfoPtr->formatPtr)
    {
        formatPtr = inOutParamInfoPtr->formatPtr;
    }
    PRV_CPSS_LOG_AND_HISTORY_PARAM_MAC(contextLib, logType, formatPtr, namePtr, *paramVal);
}

