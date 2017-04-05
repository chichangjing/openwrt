/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* prvCpssDxChTrunkLog.c
*       WARNING!!! this is a generated file, please don't edit it manually
* COMMENTS:
*
* FILE REVISION NUMBER:
*       $Revision: 1 $
*******************************************************************************/
#include <cpss/generic/log/cpssLog.h>
#include <cpss/generic/log/prvCpssLog.h>
#include <cpss/extServices/private/prvCpssBindFunc.h>
#include <cpss/dxCh/dxChxGen/trunk/cpssDxChTrunk.h>
#include <cpss/dxCh/dxChxGen/trunk/private/prvCpssDxChTrunkLog.h>
#include <cpss/generic/log/prvCpssGenLog.h>
#include <cpss/generic/trunk/private/prvCpssGenTrunkLog.h>


/********* enums *********/

char * prvCpssLogEnum_CPSS_DXCH_MULTI_PORT_GROUP_SRC_HASH_MODE_ENT[]  =
{
    "CPSS_DXCH_MULTI_PORT_GROUP_SRC_HASH_MODE_MODULO_8_E",
    "CPSS_DXCH_MULTI_PORT_GROUP_SRC_HASH_MODE_MODULO_64_E"
};
PRV_CPSS_LOG_STC_ENUM_ARRAY_SIZE_MAC(CPSS_DXCH_MULTI_PORT_GROUP_SRC_HASH_MODE_ENT);
PRV_CPSS_ENUM_STRING_VALUE_PAIR_STC prvCpssLogEnum_map_CPSS_DXCH_TRUNK_DESIGNATED_TABLE_MODE_ENT[]  =
{PRV_CPSS_LOG_ENUM_NAME_AND_VALUE_MAC(CPSS_DXCH_TRUNK_DESIGNATED_TABLE_USE_INGRESS_HASH_E),
PRV_CPSS_LOG_ENUM_NAME_AND_VALUE_MAC(CPSS_DXCH_TRUNK_DESIGNATED_TABLE_USE_INGRESS_HASH_AND_VID_E),
PRV_CPSS_LOG_ENUM_NAME_AND_VALUE_MAC(CPSS_DXCH_TRUNK_DESIGNATED_TABLE_USE_SOURCE_INFO_E)
};
PRV_CPSS_LOG_STC_ENUM_MAP_ARRAY_SIZE_MAC(CPSS_DXCH_TRUNK_DESIGNATED_TABLE_MODE_ENT);
char * prvCpssLogEnum_CPSS_DXCH_TRUNK_HASH_CLIENT_TYPE_ENT[]  =
{
    "CPSS_DXCH_TRUNK_HASH_CLIENT_TYPE_TRUNK_E",
    "CPSS_DXCH_TRUNK_HASH_CLIENT_TYPE_L2_ECMP_E",
    "CPSS_DXCH_TRUNK_HASH_CLIENT_TYPE_L3_ECMP_E",
    "CPSS_DXCH_TRUNK_HASH_CLIENT_TYPE___LAST___E"
};
PRV_CPSS_LOG_STC_ENUM_ARRAY_SIZE_MAC(CPSS_DXCH_TRUNK_HASH_CLIENT_TYPE_ENT);
char * prvCpssLogEnum_CPSS_DXCH_TRUNK_HASH_MASK_CRC_PARAM_OVERRIDE_ENT[]  =
{
    "CPSS_DXCH_TRUNK_HASH_MASK_CRC_PARAM_OVERRIDE_UDB21_UDB22_BY_FLOW_ID_E",
    "CPSS_DXCH_TRUNK_HASH_MASK_CRC_PARAM_OVERRIDE_MPLS_LABEL0_BY_EVLAN_E",
    "CPSS_DXCH_TRUNK_HASH_MASK_CRC_PARAM_OVERRIDE_MPLS_LABEL1_BY_ORIGINAL_SOURCE_EPORT_E",
    "CPSS_DXCH_TRUNK_HASH_MASK_CRC_PARAM_OVERRIDE_MPLS_LABEL2_BY_LOCAL_SOURCE_EPORT_E",
    "CPSS_DXCH_TRUNK_HASH_MASK_CRC_PARAM_OVERRIDE_IPV6_ADDR_24MSB_BY_24UDBS_E"
};
PRV_CPSS_LOG_STC_ENUM_ARRAY_SIZE_MAC(CPSS_DXCH_TRUNK_HASH_MASK_CRC_PARAM_OVERRIDE_ENT);
char * prvCpssLogEnum_CPSS_DXCH_TRUNK_IPV6_HASH_MODE_ENT[]  =
{
    "CPSS_DXCH_TRUNK_IPV6_HASH_LSB_SIP_DIP_FLOW_E",
    "CPSS_DXCH_TRUNK_IPV6_HASH_MSB_SIP_DIP_FLOW_E",
    "CPSS_DXCH_TRUNK_IPV6_HASH_MSB_LSB_SIP_DIP_FLOW_E",
    "CPSS_DXCH_TRUNK_IPV6_HASH_LSB_SIP_DIP_E"
};
PRV_CPSS_LOG_STC_ENUM_ARRAY_SIZE_MAC(CPSS_DXCH_TRUNK_IPV6_HASH_MODE_ENT);
char * prvCpssLogEnum_CPSS_DXCH_TRUNK_L4_LBH_MODE_ENT[]  =
{
    "CPSS_DXCH_TRUNK_L4_LBH_DISABLED_E",
    "CPSS_DXCH_TRUNK_L4_LBH_LONG_E",
    "CPSS_DXCH_TRUNK_L4_LBH_SHORT_E"
};
PRV_CPSS_LOG_STC_ENUM_ARRAY_SIZE_MAC(CPSS_DXCH_TRUNK_L4_LBH_MODE_ENT);
char * prvCpssLogEnum_CPSS_DXCH_TRUNK_LBH_CRC_MODE_ENT[]  =
{
    "CPSS_DXCH_TRUNK_LBH_CRC_6_MODE_E",
    "CPSS_DXCH_TRUNK_LBH_CRC_16_AND_PEARSON_MODE_E",
    "CPSS_DXCH_TRUNK_LBH_CRC_32_MODE_E"
};
PRV_CPSS_LOG_STC_ENUM_ARRAY_SIZE_MAC(CPSS_DXCH_TRUNK_LBH_CRC_MODE_ENT);
char * prvCpssLogEnum_CPSS_DXCH_TRUNK_LBH_GLOBAL_MODE_ENT[]  =
{
    "CPSS_DXCH_TRUNK_LBH_PACKETS_INFO_E",
    "CPSS_DXCH_TRUNK_LBH_INGRESS_PORT_E",
    "CPSS_DXCH_TRUNK_LBH_PACKETS_INFO_CRC_E"
};
PRV_CPSS_LOG_STC_ENUM_ARRAY_SIZE_MAC(CPSS_DXCH_TRUNK_LBH_GLOBAL_MODE_ENT);
char * prvCpssLogEnum_CPSS_DXCH_TRUNK_LBH_MASK_ENT[]  =
{
    "CPSS_DXCH_TRUNK_LBH_MASK_MAC_DA_E",
    "CPSS_DXCH_TRUNK_LBH_MASK_MAC_SA_E",
    "CPSS_DXCH_TRUNK_LBH_MASK_MPLS_LABEL0_E",
    "CPSS_DXCH_TRUNK_LBH_MASK_MPLS_LABEL1_E",
    "CPSS_DXCH_TRUNK_LBH_MASK_MPLS_LABEL2_E",
    "CPSS_DXCH_TRUNK_LBH_MASK_IPV4_DIP_E",
    "CPSS_DXCH_TRUNK_LBH_MASK_IPV4_SIP_E",
    "CPSS_DXCH_TRUNK_LBH_MASK_IPV6_DIP_E",
    "CPSS_DXCH_TRUNK_LBH_MASK_IPV6_SIP_E",
    "CPSS_DXCH_TRUNK_LBH_MASK_IPV6_FLOW_E",
    "CPSS_DXCH_TRUNK_LBH_MASK_L4_DST_PORT_E",
    "CPSS_DXCH_TRUNK_LBH_MASK_L4_SRC_PORT_E"
};
PRV_CPSS_LOG_STC_ENUM_ARRAY_SIZE_MAC(CPSS_DXCH_TRUNK_LBH_MASK_ENT);
char * prvCpssLogEnum_CPSS_DXCH_TRUNK_MEMBERS_MODE_ENT[]  =
{
    "CPSS_DXCH_TRUNK_MEMBERS_MODE_NATIVE_E",
    "CPSS_DXCH_TRUNK_MEMBERS_MODE_SOHO_EMULATION_E"
};
PRV_CPSS_LOG_STC_ENUM_ARRAY_SIZE_MAC(CPSS_DXCH_TRUNK_MEMBERS_MODE_ENT);


/********* structure fields log functions *********/

void prvCpssLogParamFuncStc_CPSS_DXCH_MULTI_PORT_GROUP_SRC_PORT_HASH_PAIR_STC_PTR
(
    IN CPSS_LOG_LIB_ENT         contextLib,
    IN CPSS_LOG_TYPE_ENT        logType,
    IN GT_CHAR_PTR              namePtr,
    IN void                   * fieldPtr,
    INOUT PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC     * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_START_FIELD_STC_MAC(CPSS_DXCH_MULTI_PORT_GROUP_SRC_PORT_HASH_PAIR_STC *, valPtr);
    PRV_CPSS_LOG_UNUSED_MAC(inOutParamInfoPtr);
    PRV_CPSS_LOG_STC_NUMBER_MAC(valPtr, srcPort);
    PRV_CPSS_LOG_STC_NUMBER_MAC(valPtr, trunkPort);
    prvCpssLogStcLogEnd(contextLib, logType);
}
void prvCpssLogParamFuncStc_CPSS_DXCH_TRUNK_LBH_CRC_MASK_ENTRY_STC_PTR
(
    IN CPSS_LOG_LIB_ENT         contextLib,
    IN CPSS_LOG_TYPE_ENT        logType,
    IN GT_CHAR_PTR              namePtr,
    IN void                   * fieldPtr,
    INOUT PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC     * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_START_FIELD_STC_MAC(CPSS_DXCH_TRUNK_LBH_CRC_MASK_ENTRY_STC *, valPtr);
    PRV_CPSS_LOG_UNUSED_MAC(inOutParamInfoPtr);
    PRV_CPSS_LOG_STC_NUMBER_MAC(valPtr, l4DstPortMaskBmp);
    PRV_CPSS_LOG_STC_NUMBER_MAC(valPtr, l4SrcPortMaskBmp);
    PRV_CPSS_LOG_STC_NUMBER_MAC(valPtr, ipv6FlowMaskBmp);
    PRV_CPSS_LOG_STC_NUMBER_MAC(valPtr, ipDipMaskBmp);
    PRV_CPSS_LOG_STC_NUMBER_MAC(valPtr, ipSipMaskBmp);
    PRV_CPSS_LOG_STC_NUMBER_MAC(valPtr, macDaMaskBmp);
    PRV_CPSS_LOG_STC_NUMBER_MAC(valPtr, macSaMaskBmp);
    PRV_CPSS_LOG_STC_NUMBER_MAC(valPtr, mplsLabel0MaskBmp);
    PRV_CPSS_LOG_STC_NUMBER_MAC(valPtr, mplsLabel1MaskBmp);
    PRV_CPSS_LOG_STC_NUMBER_MAC(valPtr, mplsLabel2MaskBmp);
    PRV_CPSS_LOG_STC_NUMBER_MAC(valPtr, localSrcPortMaskBmp);
    PRV_CPSS_LOG_STC_NUMBER_MAC(valPtr, udbsMaskBmp);
    PRV_CPSS_LOG_STC_BOOL_MAC(valPtr, symmetricMacAddrEnable);
    PRV_CPSS_LOG_STC_BOOL_MAC(valPtr, symmetricIpv4AddrEnable);
    PRV_CPSS_LOG_STC_BOOL_MAC(valPtr, symmetricIpv6AddrEnable);
    PRV_CPSS_LOG_STC_BOOL_MAC(valPtr, symmetricL4PortEnable);
    prvCpssLogStcLogEnd(contextLib, logType);
}
void prvCpssLogParamFuncStc_CPSS_DXCH_TRUNK_LBH_INPUT_DATA_STC_PTR
(
    IN CPSS_LOG_LIB_ENT         contextLib,
    IN CPSS_LOG_TYPE_ENT        logType,
    IN GT_CHAR_PTR              namePtr,
    IN void                   * fieldPtr,
    INOUT PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC     * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_START_FIELD_STC_MAC(CPSS_DXCH_TRUNK_LBH_INPUT_DATA_STC *, valPtr);
    PRV_CPSS_LOG_UNUSED_MAC(inOutParamInfoPtr);
    PRV_CPSS_LOG_STC_TYPE_ARRAY_MAC(valPtr, l4DstPortArray, 2, GT_U8);
    PRV_CPSS_LOG_STC_TYPE_ARRAY_MAC(valPtr, l4SrcPortArray, 2, GT_U8);
    PRV_CPSS_LOG_STC_TYPE_ARRAY_MAC(valPtr, ipv6FlowArray, 3, GT_U8);
    PRV_CPSS_LOG_STC_TYPE_ARRAY_MAC(valPtr, ipDipArray, 16, GT_U8);
    PRV_CPSS_LOG_STC_TYPE_ARRAY_MAC(valPtr, ipSipArray, 16, GT_U8);
    PRV_CPSS_LOG_STC_TYPE_ARRAY_MAC(valPtr, macDaArray, 6, GT_U8);
    PRV_CPSS_LOG_STC_TYPE_ARRAY_MAC(valPtr, macSaArray, 6, GT_U8);
    PRV_CPSS_LOG_STC_TYPE_ARRAY_MAC(valPtr, mplsLabel0Array, 3, GT_U8);
    PRV_CPSS_LOG_STC_TYPE_ARRAY_MAC(valPtr, mplsLabel1Array, 3, GT_U8);
    PRV_CPSS_LOG_STC_TYPE_ARRAY_MAC(valPtr, mplsLabel2Array, 3, GT_U8);
    PRV_CPSS_LOG_STC_TYPE_ARRAY_MAC(valPtr, localSrcPortArray, 1, GT_U8);
    PRV_CPSS_LOG_STC_TYPE_ARRAY_MAC(valPtr, udbsArray, 32, GT_U8);
    prvCpssLogStcLogEnd(contextLib, logType);
}


/********* parameters log functions *********/

void prvCpssLogParamFunc_CPSS_DXCH_MULTI_PORT_GROUP_SRC_HASH_MODE_ENT(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_SET_PARAM_VAL_MAC(CPSS_DXCH_MULTI_PORT_GROUP_SRC_HASH_MODE_ENT, paramVal);
    PRV_CPSS_LOG_ENUM_MAC(namePtr, paramVal, CPSS_DXCH_MULTI_PORT_GROUP_SRC_HASH_MODE_ENT);
}
void prvCpssLogParamFunc_CPSS_DXCH_TRUNK_DESIGNATED_TABLE_MODE_ENT(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_SET_PARAM_VAL_MAC(CPSS_DXCH_TRUNK_DESIGNATED_TABLE_MODE_ENT, paramVal);
    PRV_CPSS_LOG_ENUM_MAP_MAC(namePtr, paramVal, CPSS_DXCH_TRUNK_DESIGNATED_TABLE_MODE_ENT);
}
void prvCpssLogParamFunc_CPSS_DXCH_TRUNK_DESIGNATED_TABLE_MODE_ENT_PTR(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_SET_PARAM_VAL_MAC(CPSS_DXCH_TRUNK_DESIGNATED_TABLE_MODE_ENT*, paramVal);
    if (paramVal == NULL)
    {
        PRV_CPSS_LOG_AND_HISTORY_PARAM_MAC(contextLib, logType, "%s = NULL\n", namePtr);
        return;
    }
    PRV_CPSS_LOG_ENUM_MAP_MAC(namePtr, *paramVal, CPSS_DXCH_TRUNK_DESIGNATED_TABLE_MODE_ENT);
}
void prvCpssLogParamFunc_CPSS_DXCH_TRUNK_HASH_CLIENT_TYPE_ENT(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_SET_PARAM_VAL_MAC(CPSS_DXCH_TRUNK_HASH_CLIENT_TYPE_ENT, paramVal);
    PRV_CPSS_LOG_ENUM_MAC(namePtr, paramVal, CPSS_DXCH_TRUNK_HASH_CLIENT_TYPE_ENT);
}
void prvCpssLogParamFunc_CPSS_DXCH_TRUNK_HASH_MASK_CRC_PARAM_OVERRIDE_ENT(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_SET_PARAM_VAL_MAC(CPSS_DXCH_TRUNK_HASH_MASK_CRC_PARAM_OVERRIDE_ENT, paramVal);
    PRV_CPSS_LOG_ENUM_MAC(namePtr, paramVal, CPSS_DXCH_TRUNK_HASH_MASK_CRC_PARAM_OVERRIDE_ENT);
}
void prvCpssLogParamFunc_CPSS_DXCH_TRUNK_IPV6_HASH_MODE_ENT(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_SET_PARAM_VAL_MAC(CPSS_DXCH_TRUNK_IPV6_HASH_MODE_ENT, paramVal);
    PRV_CPSS_LOG_ENUM_MAC(namePtr, paramVal, CPSS_DXCH_TRUNK_IPV6_HASH_MODE_ENT);
}
void prvCpssLogParamFunc_CPSS_DXCH_TRUNK_IPV6_HASH_MODE_ENT_PTR(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_SET_PARAM_VAL_MAC(CPSS_DXCH_TRUNK_IPV6_HASH_MODE_ENT*, paramVal);
    if (paramVal == NULL)
    {
        PRV_CPSS_LOG_AND_HISTORY_PARAM_MAC(contextLib, logType, "%s = NULL\n", namePtr);
        return;
    }
    PRV_CPSS_LOG_ENUM_MAC(namePtr, *paramVal, CPSS_DXCH_TRUNK_IPV6_HASH_MODE_ENT);
}
void prvCpssLogParamFunc_CPSS_DXCH_TRUNK_L4_LBH_MODE_ENT(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_SET_PARAM_VAL_MAC(CPSS_DXCH_TRUNK_L4_LBH_MODE_ENT, paramVal);
    PRV_CPSS_LOG_ENUM_MAC(namePtr, paramVal, CPSS_DXCH_TRUNK_L4_LBH_MODE_ENT);
}
void prvCpssLogParamFunc_CPSS_DXCH_TRUNK_L4_LBH_MODE_ENT_PTR(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_SET_PARAM_VAL_MAC(CPSS_DXCH_TRUNK_L4_LBH_MODE_ENT*, paramVal);
    if (paramVal == NULL)
    {
        PRV_CPSS_LOG_AND_HISTORY_PARAM_MAC(contextLib, logType, "%s = NULL\n", namePtr);
        return;
    }
    PRV_CPSS_LOG_ENUM_MAC(namePtr, *paramVal, CPSS_DXCH_TRUNK_L4_LBH_MODE_ENT);
}
void prvCpssLogParamFunc_CPSS_DXCH_TRUNK_LBH_CRC_MASK_ENTRY_STC_PTR(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_START_PARAM_STC_MAC(CPSS_DXCH_TRUNK_LBH_CRC_MASK_ENTRY_STC*, paramVal);
    prvCpssLogParamFuncStc_CPSS_DXCH_TRUNK_LBH_CRC_MASK_ENTRY_STC_PTR(contextLib, logType, namePtr, paramVal, inOutParamInfoPtr);
}
void prvCpssLogParamFunc_CPSS_DXCH_TRUNK_LBH_CRC_MODE_ENT(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_SET_PARAM_VAL_MAC(CPSS_DXCH_TRUNK_LBH_CRC_MODE_ENT, paramVal);
    PRV_CPSS_LOG_ENUM_MAC(namePtr, paramVal, CPSS_DXCH_TRUNK_LBH_CRC_MODE_ENT);
}
void prvCpssLogParamFunc_CPSS_DXCH_TRUNK_LBH_CRC_MODE_ENT_PTR(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_SET_PARAM_VAL_MAC(CPSS_DXCH_TRUNK_LBH_CRC_MODE_ENT*, paramVal);
    if (paramVal == NULL)
    {
        PRV_CPSS_LOG_AND_HISTORY_PARAM_MAC(contextLib, logType, "%s = NULL\n", namePtr);
        return;
    }
    PRV_CPSS_LOG_ENUM_MAC(namePtr, *paramVal, CPSS_DXCH_TRUNK_LBH_CRC_MODE_ENT);
}
void prvCpssLogParamFunc_CPSS_DXCH_TRUNK_LBH_GLOBAL_MODE_ENT(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_SET_PARAM_VAL_MAC(CPSS_DXCH_TRUNK_LBH_GLOBAL_MODE_ENT, paramVal);
    PRV_CPSS_LOG_ENUM_MAC(namePtr, paramVal, CPSS_DXCH_TRUNK_LBH_GLOBAL_MODE_ENT);
}
void prvCpssLogParamFunc_CPSS_DXCH_TRUNK_LBH_GLOBAL_MODE_ENT_PTR(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_SET_PARAM_VAL_MAC(CPSS_DXCH_TRUNK_LBH_GLOBAL_MODE_ENT*, paramVal);
    if (paramVal == NULL)
    {
        PRV_CPSS_LOG_AND_HISTORY_PARAM_MAC(contextLib, logType, "%s = NULL\n", namePtr);
        return;
    }
    PRV_CPSS_LOG_ENUM_MAC(namePtr, *paramVal, CPSS_DXCH_TRUNK_LBH_GLOBAL_MODE_ENT);
}
void prvCpssLogParamFunc_CPSS_DXCH_TRUNK_LBH_INPUT_DATA_STC_PTR(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_START_PARAM_STC_MAC(CPSS_DXCH_TRUNK_LBH_INPUT_DATA_STC*, paramVal);
    prvCpssLogParamFuncStc_CPSS_DXCH_TRUNK_LBH_INPUT_DATA_STC_PTR(contextLib, logType, namePtr, paramVal, inOutParamInfoPtr);
}
void prvCpssLogParamFunc_CPSS_DXCH_TRUNK_LBH_MASK_ENT(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_SET_PARAM_VAL_MAC(CPSS_DXCH_TRUNK_LBH_MASK_ENT, paramVal);
    PRV_CPSS_LOG_ENUM_MAC(namePtr, paramVal, CPSS_DXCH_TRUNK_LBH_MASK_ENT);
}
void prvCpssLogParamFunc_CPSS_DXCH_TRUNK_MEMBERS_MODE_ENT(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_SET_PARAM_VAL_MAC(CPSS_DXCH_TRUNK_MEMBERS_MODE_ENT, paramVal);
    PRV_CPSS_LOG_ENUM_MAC(namePtr, paramVal, CPSS_DXCH_TRUNK_MEMBERS_MODE_ENT);
}


/********* API fields DB *********/

PRV_CPSS_LOG_FUNC_PARAM_STC INOUT_GT_U32_PTR_numOfDisabledMembersPtr = {
     "numOfDisabledMembersPtr", PRV_CPSS_LOG_PARAM_INOUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(GT_U32)
};
PRV_CPSS_LOG_FUNC_PARAM_STC INOUT_GT_U32_PTR_numOfEnabledMembersPtr = {
     "numOfEnabledMembersPtr", PRV_CPSS_LOG_PARAM_INOUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(GT_U32)
};
PRV_CPSS_LOG_FUNC_PARAM_STC INOUT_GT_U32_PTR_numOfMembersPtr = {
     "numOfMembersPtr", PRV_CPSS_LOG_PARAM_INOUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(GT_U32)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_DXCH_TRUNK_DESIGNATED_TABLE_MODE_ENT_mode = {
     "mode", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_MAC(CPSS_DXCH_TRUNK_DESIGNATED_TABLE_MODE_ENT)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_DXCH_TRUNK_HASH_CLIENT_TYPE_ENT_hashEntityType = {
     "hashEntityType", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_MAC(CPSS_DXCH_TRUNK_HASH_CLIENT_TYPE_ENT)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_DXCH_TRUNK_HASH_MASK_CRC_PARAM_OVERRIDE_ENT_fieldType = {
     "fieldType", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_MAC(CPSS_DXCH_TRUNK_HASH_MASK_CRC_PARAM_OVERRIDE_ENT)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_DXCH_TRUNK_IPV6_HASH_MODE_ENT_hashMode = {
     "hashMode", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_MAC(CPSS_DXCH_TRUNK_IPV6_HASH_MODE_ENT)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_DXCH_TRUNK_L4_LBH_MODE_ENT_hashMode = {
     "hashMode", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_MAC(CPSS_DXCH_TRUNK_L4_LBH_MODE_ENT)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_DXCH_TRUNK_LBH_CRC_MASK_ENTRY_STC_PTR_entryPtr = {
     "entryPtr", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(CPSS_DXCH_TRUNK_LBH_CRC_MASK_ENTRY_STC)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_DXCH_TRUNK_LBH_CRC_MODE_ENT_crcMode = {
     "crcMode", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_MAC(CPSS_DXCH_TRUNK_LBH_CRC_MODE_ENT)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_DXCH_TRUNK_LBH_GLOBAL_MODE_ENT_hashMode = {
     "hashMode", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_MAC(CPSS_DXCH_TRUNK_LBH_GLOBAL_MODE_ENT)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_DXCH_TRUNK_LBH_INPUT_DATA_STC_PTR_hashInputDataPtr = {
     "hashInputDataPtr", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(CPSS_DXCH_TRUNK_LBH_INPUT_DATA_STC)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_DXCH_TRUNK_LBH_MASK_ENT_maskedField = {
     "maskedField", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_MAC(CPSS_DXCH_TRUNK_LBH_MASK_ENT)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_DXCH_TRUNK_MEMBERS_MODE_ENT_trunkMembersMode = {
     "trunkMembersMode", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_MAC(CPSS_DXCH_TRUNK_MEMBERS_MODE_ENT)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_PORTS_BMP_STC_PTR_designatedPortsPtr = {
     "designatedPortsPtr", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(CPSS_PORTS_BMP_STC)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_PORTS_BMP_STC_PTR_nonTrunkPortsBmpPtr = {
     "nonTrunkPortsBmpPtr", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(CPSS_PORTS_BMP_STC)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_PORTS_BMP_STC_PTR_nonTrunkPortsPtr = {
     "nonTrunkPortsPtr", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(CPSS_PORTS_BMP_STC)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_TRUNK_DESIGNATED_TABLE_MC_PORT_MODE_ENT_mode = {
     "mode", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_MAC(CPSS_TRUNK_DESIGNATED_TABLE_MC_PORT_MODE_ENT)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_TRUNK_MEMBER_STC_PTR_disabledMembersArray = {
     "disabledMembersArray", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(CPSS_TRUNK_MEMBER_STC)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_TRUNK_MEMBER_STC_PTR_enabledMembersArray = {
     "enabledMembersArray", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(CPSS_TRUNK_MEMBER_STC)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_TRUNK_MEMBER_STC_PTR_memberPtr = {
     "memberPtr", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(CPSS_TRUNK_MEMBER_STC)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_TRUNK_MEMBER_STC_PTR_membersArray = {
     "membersArray", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(CPSS_TRUNK_MEMBER_STC)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_TRUNK_PORT_TRUNK_ID_MANAGEMENT_MODE_ENT_manageMode = {
     "manageMode", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_MAC(CPSS_TRUNK_PORT_TRUNK_ID_MANAGEMENT_MODE_ENT)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_TRUNK_WEIGHTED_MEMBER_STC_PTR_weightedMembersArray = {
     "weightedMembersArray", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(CPSS_TRUNK_WEIGHTED_MEMBER_STC)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_BOOL_isSrcIp = {
     "isSrcIp", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_MAC(GT_BOOL)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_BOOL_memberOfTrunk = {
     "memberOfTrunk", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_MAC(GT_BOOL)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_U32_byteIndex = {
     "byteIndex", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_MAC(GT_U32)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_U32_crcSeed = {
     "crcSeed", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_MAC(GT_U32)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_U32_maxNumberOfTrunks = {
     "maxNumberOfTrunks", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_MAC(GT_U32)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_U32_numMembers = {
     "numMembers", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_MAC(GT_U32)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_U32_numOfDisabledMembers = {
     "numOfDisabledMembers", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_MAC(GT_U32)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_U32_numOfEnabledMembers = {
     "numOfEnabledMembers", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_MAC(GT_U32)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_U32_numOfMembers = {
     "numOfMembers", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_MAC(GT_U32)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_U32_numberOfMembers = {
     "numberOfMembers", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_MAC(GT_U32)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_U32_shiftValue = {
     "shiftValue", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_MAC(GT_U32)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_U8_maskValue = {
     "maskValue", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_MAC(GT_U8)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_U8_saltValue = {
     "saltValue", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_MAC(GT_U8)
};
PRV_CPSS_LOG_FUNC_PARAM_STC OUT_CPSS_DXCH_TRUNK_DESIGNATED_TABLE_MODE_ENT_PTR_modePtr = {
     "modePtr", PRV_CPSS_LOG_PARAM_OUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(CPSS_DXCH_TRUNK_DESIGNATED_TABLE_MODE_ENT)
};
PRV_CPSS_LOG_FUNC_PARAM_STC OUT_CPSS_DXCH_TRUNK_IPV6_HASH_MODE_ENT_PTR_hashModePtr = {
     "hashModePtr", PRV_CPSS_LOG_PARAM_OUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(CPSS_DXCH_TRUNK_IPV6_HASH_MODE_ENT)
};
PRV_CPSS_LOG_FUNC_PARAM_STC OUT_CPSS_DXCH_TRUNK_L4_LBH_MODE_ENT_PTR_hashModePtr = {
     "hashModePtr", PRV_CPSS_LOG_PARAM_OUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(CPSS_DXCH_TRUNK_L4_LBH_MODE_ENT)
};
PRV_CPSS_LOG_FUNC_PARAM_STC OUT_CPSS_DXCH_TRUNK_LBH_CRC_MASK_ENTRY_STC_PTR_entryPtr = {
     "entryPtr", PRV_CPSS_LOG_PARAM_OUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(CPSS_DXCH_TRUNK_LBH_CRC_MASK_ENTRY_STC)
};
PRV_CPSS_LOG_FUNC_PARAM_STC OUT_CPSS_DXCH_TRUNK_LBH_CRC_MODE_ENT_PTR_crcModePtr = {
     "crcModePtr", PRV_CPSS_LOG_PARAM_OUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(CPSS_DXCH_TRUNK_LBH_CRC_MODE_ENT)
};
PRV_CPSS_LOG_FUNC_PARAM_STC OUT_CPSS_DXCH_TRUNK_LBH_GLOBAL_MODE_ENT_PTR_hashModePtr = {
     "hashModePtr", PRV_CPSS_LOG_PARAM_OUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(CPSS_DXCH_TRUNK_LBH_GLOBAL_MODE_ENT)
};
PRV_CPSS_LOG_FUNC_PARAM_STC OUT_CPSS_PORTS_BMP_STC_PTR_designatedPortsPtr = {
     "designatedPortsPtr", PRV_CPSS_LOG_PARAM_OUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(CPSS_PORTS_BMP_STC)
};
PRV_CPSS_LOG_FUNC_PARAM_STC OUT_CPSS_PORTS_BMP_STC_PTR_nonTrunkPortsPtr = {
     "nonTrunkPortsPtr", PRV_CPSS_LOG_PARAM_OUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(CPSS_PORTS_BMP_STC)
};
PRV_CPSS_LOG_FUNC_PARAM_STC OUT_CPSS_TRUNK_DESIGNATED_TABLE_MC_PORT_MODE_ENT_PTR_modePtr = {
     "modePtr", PRV_CPSS_LOG_PARAM_OUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(CPSS_TRUNK_DESIGNATED_TABLE_MC_PORT_MODE_ENT)
};
PRV_CPSS_LOG_FUNC_PARAM_STC OUT_CPSS_TRUNK_MEMBER_STC_PTR_disabledMembersArray = {
     "disabledMembersArray", PRV_CPSS_LOG_PARAM_OUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(CPSS_TRUNK_MEMBER_STC)
};
PRV_CPSS_LOG_FUNC_PARAM_STC OUT_CPSS_TRUNK_MEMBER_STC_PTR_enabledMembersArray = {
     "enabledMembersArray", PRV_CPSS_LOG_PARAM_OUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(CPSS_TRUNK_MEMBER_STC)
};
PRV_CPSS_LOG_FUNC_PARAM_STC OUT_CPSS_TRUNK_MEMBER_STC_PTR_memberPtr = {
     "memberPtr", PRV_CPSS_LOG_PARAM_OUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(CPSS_TRUNK_MEMBER_STC)
};
PRV_CPSS_LOG_FUNC_PARAM_STC OUT_CPSS_TRUNK_MEMBER_STC_PTR_membersArray = {
     "membersArray", PRV_CPSS_LOG_PARAM_OUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(CPSS_TRUNK_MEMBER_STC)
};
PRV_CPSS_LOG_FUNC_PARAM_STC OUT_CPSS_TRUNK_PORT_TRUNK_ID_MANAGEMENT_MODE_ENT_PTR_manageModePtr = {
     "manageModePtr", PRV_CPSS_LOG_PARAM_OUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(CPSS_TRUNK_PORT_TRUNK_ID_MANAGEMENT_MODE_ENT)
};
PRV_CPSS_LOG_FUNC_PARAM_STC OUT_CPSS_TRUNK_TYPE_ENT_PTR_typePtr = {
     "typePtr", PRV_CPSS_LOG_PARAM_OUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(CPSS_TRUNK_TYPE_ENT)
};
PRV_CPSS_LOG_FUNC_PARAM_STC OUT_CPSS_TRUNK_WEIGHTED_MEMBER_STC_PTR_weightedMembersArray = {
     "weightedMembersArray", PRV_CPSS_LOG_PARAM_OUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(CPSS_TRUNK_WEIGHTED_MEMBER_STC)
};
PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_BOOL_PTR_memberOfTrunkPtr = {
     "memberOfTrunkPtr", PRV_CPSS_LOG_PARAM_OUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(GT_BOOL)
};
PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_BOOL_PTR_overridePtr = {
     "overridePtr", PRV_CPSS_LOG_PARAM_OUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(GT_BOOL)
};
PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_U32_PTR_crcSeedPtr = {
     "crcSeedPtr", PRV_CPSS_LOG_PARAM_OUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(GT_U32)
};
PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_U32_PTR_hashIndexPtr = {
     "hashIndexPtr", PRV_CPSS_LOG_PARAM_OUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(GT_U32)
};
PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_U32_PTR_numMembersPtr = {
     "numMembersPtr", PRV_CPSS_LOG_PARAM_OUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(GT_U32)
};
PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_U32_PTR_shiftValuePtr = {
     "shiftValuePtr", PRV_CPSS_LOG_PARAM_OUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(GT_U32)
};
PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_U8_PTR_maskValuePtr = {
     "maskValuePtr", PRV_CPSS_LOG_PARAM_OUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(GT_U8)
};
PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_U8_PTR_saltValuePtr = {
     "saltValuePtr", PRV_CPSS_LOG_PARAM_OUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(GT_U8)
};


/********* API prototypes DB *********/

PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChTrunkHashIndexCalculate_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_CPSS_DXCH_PCL_PACKET_TYPE_ENT_packetType,
    &IN_CPSS_DXCH_TRUNK_LBH_INPUT_DATA_STC_PTR_hashInputDataPtr,
    &IN_CPSS_DXCH_TRUNK_HASH_CLIENT_TYPE_ENT_hashEntityType,
    &IN_GT_U32_numberOfMembers,
    &OUT_GT_U32_PTR_hashIndexPtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChTrunkHashDesignatedTableModeSet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_CPSS_DXCH_TRUNK_DESIGNATED_TABLE_MODE_ENT_mode
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChTrunkHashMaskCrcParamOverrideSet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_CPSS_DXCH_TRUNK_HASH_MASK_CRC_PARAM_OVERRIDE_ENT_fieldType,
    &IN_GT_BOOL_override
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChTrunkHashMaskCrcParamOverrideGet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_CPSS_DXCH_TRUNK_HASH_MASK_CRC_PARAM_OVERRIDE_ENT_fieldType,
    &OUT_GT_BOOL_PTR_overridePtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChTrunkHashIpv6ModeSet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_CPSS_DXCH_TRUNK_IPV6_HASH_MODE_ENT_hashMode
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChTrunkHashL4ModeSet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_CPSS_DXCH_TRUNK_L4_LBH_MODE_ENT_hashMode
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChTrunkHashCrcParametersSet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_CPSS_DXCH_TRUNK_LBH_CRC_MODE_ENT_crcMode,
    &IN_GT_U32_crcSeed
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChTrunkHashGlobalModeSet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_CPSS_DXCH_TRUNK_LBH_GLOBAL_MODE_ENT_hashMode
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChTrunkHashMaskSet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_CPSS_DXCH_TRUNK_LBH_MASK_ENT_maskedField,
    &IN_GT_U8_maskValue
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChTrunkHashMaskGet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_CPSS_DXCH_TRUNK_LBH_MASK_ENT_maskedField,
    &OUT_GT_U8_PTR_maskValuePtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChTrunkHashIpShiftSet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_CPSS_IP_PROTOCOL_STACK_ENT_protocolStack,
    &IN_GT_BOOL_isSrcIp,
    &IN_GT_U32_shiftValue
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChTrunkHashIpShiftGet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_CPSS_IP_PROTOCOL_STACK_ENT_protocolStack,
    &IN_GT_BOOL_isSrcIp,
    &OUT_GT_U32_PTR_shiftValuePtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChTrunkDbIsMemberOfTrunk_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_CPSS_TRUNK_MEMBER_STC_PTR_memberPtr,
    &OUT_GT_TRUNK_ID_PTR_trunkIdPtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChTrunkPortMcEnableSet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_PHYSICAL_PORT_NUM_portNum,
    &IN_CPSS_TRUNK_DESIGNATED_TABLE_MC_PORT_MODE_ENT_mode
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChTrunkDbPortTrunkIdModeSet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_PHYSICAL_PORT_NUM_portNum,
    &IN_CPSS_TRUNK_PORT_TRUNK_ID_MANAGEMENT_MODE_ENT_manageMode
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChTrunkPortTrunkIdSet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_PHYSICAL_PORT_NUM_portNum,
    &IN_GT_BOOL_memberOfTrunk,
    &IN_GT_TRUNK_ID_trunkId
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChTrunkDbPortMcEnableGet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_PHYSICAL_PORT_NUM_portNum,
    &OUT_CPSS_TRUNK_DESIGNATED_TABLE_MC_PORT_MODE_ENT_PTR_modePtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChTrunkDbPortTrunkIdModeGet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_PHYSICAL_PORT_NUM_portNum,
    &OUT_CPSS_TRUNK_PORT_TRUNK_ID_MANAGEMENT_MODE_ENT_PTR_manageModePtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChTrunkPortTrunkIdGet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_PHYSICAL_PORT_NUM_portNum,
    &OUT_GT_BOOL_PTR_memberOfTrunkPtr,
    &OUT_GT_TRUNK_ID_PTR_trunkIdPtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChTrunkPortHashMaskInfoSet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_PORT_NUM_portNum,
    &IN_GT_BOOL_overrideEnable,
    &IN_GT_U32_index
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChTrunkPortHashMaskInfoGet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_PORT_NUM_portNum,
    &OUT_GT_BOOL_PTR_overrideEnablePtr,
    &OUT_GT_U32_PTR_indexPtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChTrunkLearnPrioritySet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_TRUNK_ID_trunkId,
    &IN_CPSS_DXCH_FDB_LEARN_PRIORITY_ENT_learnPriority
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChTrunkNonTrunkPortsAdd_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_TRUNK_ID_trunkId,
    &IN_CPSS_PORTS_BMP_STC_PTR_nonTrunkPortsBmpPtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChTrunkNonTrunkPortsEntrySet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_TRUNK_ID_trunkId,
    &IN_CPSS_PORTS_BMP_STC_PTR_nonTrunkPortsPtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChTrunkCascadeTrunkPortsSet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_TRUNK_ID_trunkId,
    &IN_CPSS_PORTS_BMP_STC_PTR_portsMembersPtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChTrunkMemberAdd_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_TRUNK_ID_trunkId,
    &IN_CPSS_TRUNK_MEMBER_STC_PTR_memberPtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChTrunkMcLocalSwitchingEnableSet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_TRUNK_ID_trunkId,
    &IN_GT_BOOL_enable
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChTrunkDesignatedMemberSet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_TRUNK_ID_trunkId,
    &IN_GT_BOOL_enable,
    &IN_CPSS_TRUNK_MEMBER_STC_PTR_memberPtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChTrunkTableEntrySet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_TRUNK_ID_trunkId,
    &IN_GT_U32_numMembers,
    &IN_CPSS_TRUNK_MEMBER_STC_PTR_membersArray
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChTrunkMembersSet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_TRUNK_ID_trunkId,
    &IN_GT_U32_numOfEnabledMembers,
    &IN_CPSS_TRUNK_MEMBER_STC_PTR_enabledMembersArray,
    &IN_GT_U32_numOfDisabledMembers,
    &IN_CPSS_TRUNK_MEMBER_STC_PTR_disabledMembersArray
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChTrunkCascadeTrunkWithWeightedPortsSet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_TRUNK_ID_trunkId,
    &IN_GT_U32_numOfMembers,
    &IN_CPSS_TRUNK_WEIGHTED_MEMBER_STC_PTR_weightedMembersArray
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChTrunkUserGroupSet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_TRUNK_ID_trunkId,
    &IN_GT_U32_userGroup
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChTrunkDbDisabledMembersGet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_TRUNK_ID_trunkId,
    &INOUT_GT_U32_PTR_numOfDisabledMembersPtr,
    &OUT_CPSS_TRUNK_MEMBER_STC_PTR_disabledMembersArray
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChTrunkDbEnabledMembersGet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_TRUNK_ID_trunkId,
    &INOUT_GT_U32_PTR_numOfEnabledMembersPtr,
    &OUT_CPSS_TRUNK_MEMBER_STC_PTR_enabledMembersArray
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChTrunkCascadeTrunkWithWeightedPortsGet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_TRUNK_ID_trunkId,
    &INOUT_GT_U32_PTR_numOfMembersPtr,
    &OUT_CPSS_TRUNK_WEIGHTED_MEMBER_STC_PTR_weightedMembersArray
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChTrunkLearnPriorityGet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_TRUNK_ID_trunkId,
    &OUT_CPSS_DXCH_FDB_LEARN_PRIORITY_ENT_PTR_learnPriorityPtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChTrunkNonTrunkPortsEntryGet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_TRUNK_ID_trunkId,
    &OUT_CPSS_PORTS_BMP_STC_PTR_nonTrunkPortsPtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChTrunkCascadeTrunkPortsGet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_TRUNK_ID_trunkId,
    &OUT_CPSS_PORTS_BMP_STC_PTR_portsMembersPtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChTrunkDbTrunkTypeGet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_TRUNK_ID_trunkId,
    &OUT_CPSS_TRUNK_TYPE_ENT_PTR_typePtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChTrunkDbMcLocalSwitchingEnableGet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_TRUNK_ID_trunkId,
    &OUT_GT_BOOL_PTR_enablePtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChTrunkDbDesignatedMemberGet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_TRUNK_ID_trunkId,
    &OUT_GT_BOOL_PTR_enablePtr,
    &OUT_CPSS_TRUNK_MEMBER_STC_PTR_memberPtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChTrunkTableEntryGet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_TRUNK_ID_trunkId,
    &OUT_GT_U32_PTR_numMembersPtr,
    &OUT_CPSS_TRUNK_MEMBER_STC_PTR_membersArray
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChTrunkUserGroupGet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_TRUNK_ID_trunkId,
    &OUT_GT_U32_PTR_userGroupPtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChTrunkHashCrcSaltByteSet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_U32_byteIndex,
    &IN_GT_U8_saltValue
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChTrunkHashCrcSaltByteGet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_U32_byteIndex,
    &OUT_GT_U8_PTR_saltValuePtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChTrunkDesignatedPortsEntrySet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_U32_entryIndex,
    &IN_CPSS_PORTS_BMP_STC_PTR_designatedPortsPtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChTrunkDesignatedPortsEntryGet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_U32_entryIndex,
    &OUT_CPSS_PORTS_BMP_STC_PTR_designatedPortsPtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChTrunkHashMaskCrcEntrySet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_U32_index,
    &IN_CPSS_DXCH_TRUNK_LBH_CRC_MASK_ENTRY_STC_PTR_entryPtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChTrunkHashMaskCrcEntryGet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_U32_index,
    &OUT_CPSS_DXCH_TRUNK_LBH_CRC_MASK_ENTRY_STC_PTR_entryPtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChTrunkInit_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_U32_maxNumberOfTrunks,
    &IN_CPSS_DXCH_TRUNK_MEMBERS_MODE_ENT_trunkMembersMode
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChTrunkHashDesignatedTableModeGet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &OUT_CPSS_DXCH_TRUNK_DESIGNATED_TABLE_MODE_ENT_PTR_modePtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChTrunkHashIpv6ModeGet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &OUT_CPSS_DXCH_TRUNK_IPV6_HASH_MODE_ENT_PTR_hashModePtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChTrunkHashL4ModeGet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &OUT_CPSS_DXCH_TRUNK_L4_LBH_MODE_ENT_PTR_hashModePtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChTrunkHashCrcParametersGet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &OUT_CPSS_DXCH_TRUNK_LBH_CRC_MODE_ENT_PTR_crcModePtr,
    &OUT_GT_U32_PTR_crcSeedPtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChTrunkHashGlobalModeGet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &OUT_CPSS_DXCH_TRUNK_LBH_GLOBAL_MODE_ENT_PTR_hashModePtr
};


/********* lib API DB *********/

extern void cpssDxChTrunkTableEntrySet_preLogic(IN va_list  args, OUT PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC * paramDataPtr);
extern void cpssDxChTrunkTableEntryGet_preLogic(IN va_list  args, OUT PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC * paramDataPtr);
extern void cpssDxChTrunkDbEnabledMembersGet_preLogic(IN va_list  args, OUT PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC * paramDataPtr);
extern void cpssDxChTrunkDbEnabledMembersGet_preLogic(IN va_list  args, OUT PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC * paramDataPtr);
extern void cpssDxChTrunkMembersSet_preLogic(IN va_list  args, OUT PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC * paramDataPtr);
extern void cpssDxChTrunkTableEntrySet_preLogic(IN va_list  args, OUT PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC * paramDataPtr);
extern void cpssDxChTrunkDbEnabledMembersGet_preLogic(IN va_list  args, OUT PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC * paramDataPtr);

static PRV_CPSS_LOG_FUNC_ENTRY_STC prvCpssDxChTrunkLogLibDb[] = {
    {"cpssDxChTrunkDbMembersSortingEnableSet", 2, prvCpssLogGenDevNumEnable_PARAMS, NULL},
    {"cpssDxChTrunkDbMembersSortingEnableGet", 2, prvCpssLogGenDevNumEnablePtr_PARAMS, NULL},
    {"cpssDxChTrunkPortTrunkIdSet", 4, cpssDxChTrunkPortTrunkIdSet_PARAMS, NULL},
    {"cpssDxChTrunkPortTrunkIdGet", 4, cpssDxChTrunkPortTrunkIdGet_PARAMS, NULL},
    {"cpssDxChTrunkTableEntrySet", 4, cpssDxChTrunkTableEntrySet_PARAMS, cpssDxChTrunkTableEntrySet_preLogic},
    {"cpssDxChTrunkTableEntryGet", 4, cpssDxChTrunkTableEntryGet_PARAMS, cpssDxChTrunkTableEntryGet_preLogic},
    {"cpssDxChTrunkNonTrunkPortsEntrySet", 3, cpssDxChTrunkNonTrunkPortsEntrySet_PARAMS, NULL},
    {"cpssDxChTrunkNonTrunkPortsEntryGet", 3, cpssDxChTrunkNonTrunkPortsEntryGet_PARAMS, NULL},
    {"cpssDxChTrunkDesignatedPortsEntrySet", 3, cpssDxChTrunkDesignatedPortsEntrySet_PARAMS, NULL},
    {"cpssDxChTrunkDesignatedPortsEntryGet", 3, cpssDxChTrunkDesignatedPortsEntryGet_PARAMS, NULL},
    {"cpssDxChTrunkHashIpModeSet", 2, prvCpssLogGenDevNumEnable_PARAMS, NULL},
    {"cpssDxChTrunkHashIpModeGet", 2, prvCpssLogGenDevNumEnablePtr_PARAMS, NULL},
    {"cpssDxChTrunkHashL4ModeSet", 2, cpssDxChTrunkHashL4ModeSet_PARAMS, NULL},
    {"cpssDxChTrunkHashL4ModeGet", 2, cpssDxChTrunkHashL4ModeGet_PARAMS, NULL},
    {"cpssDxChTrunkHashIpv6ModeSet", 2, cpssDxChTrunkHashIpv6ModeSet_PARAMS, NULL},
    {"cpssDxChTrunkHashIpv6ModeGet", 2, cpssDxChTrunkHashIpv6ModeGet_PARAMS, NULL},
    {"cpssDxChTrunkHashIpAddMacModeSet", 2, prvCpssLogGenDevNumEnable_PARAMS, NULL},
    {"cpssDxChTrunkHashIpAddMacModeGet", 2, prvCpssLogGenDevNumEnablePtr_PARAMS, NULL},
    {"cpssDxChTrunkHashGlobalModeSet", 2, cpssDxChTrunkHashGlobalModeSet_PARAMS, NULL},
    {"cpssDxChTrunkHashGlobalModeGet", 2, cpssDxChTrunkHashGlobalModeGet_PARAMS, NULL},
    {"cpssDxChTrunkHashDesignatedTableModeSet", 2, cpssDxChTrunkHashDesignatedTableModeSet_PARAMS, NULL},
    {"cpssDxChTrunkHashDesignatedTableModeGet", 2, cpssDxChTrunkHashDesignatedTableModeGet_PARAMS, NULL},
    {"cpssDxChTrunkDbEnabledMembersGet", 4, cpssDxChTrunkDbEnabledMembersGet_PARAMS, cpssDxChTrunkDbEnabledMembersGet_preLogic},
    {"cpssDxChTrunkDbDisabledMembersGet", 4, cpssDxChTrunkDbDisabledMembersGet_PARAMS, cpssDxChTrunkDbEnabledMembersGet_preLogic},
    {"cpssDxChTrunkDbIsMemberOfTrunk", 3, cpssDxChTrunkDbIsMemberOfTrunk_PARAMS, NULL},
    {"cpssDxChTrunkDbTrunkTypeGet", 3, cpssDxChTrunkDbTrunkTypeGet_PARAMS, NULL},
    {"cpssDxChTrunkDesignatedMemberSet", 4, cpssDxChTrunkDesignatedMemberSet_PARAMS, NULL},
    {"cpssDxChTrunkDbDesignatedMemberGet", 4, cpssDxChTrunkDbDesignatedMemberGet_PARAMS, NULL},
    {"cpssDxChTrunkMembersSet", 6, cpssDxChTrunkMembersSet_PARAMS, cpssDxChTrunkMembersSet_preLogic},
    {"cpssDxChTrunkCascadeTrunkPortsSet", 3, cpssDxChTrunkCascadeTrunkPortsSet_PARAMS, NULL},
    {"cpssDxChTrunkCascadeTrunkPortsGet", 3, cpssDxChTrunkCascadeTrunkPortsGet_PARAMS, NULL},
    {"cpssDxChTrunkCascadeTrunkWithWeightedPortsSet", 4, cpssDxChTrunkCascadeTrunkWithWeightedPortsSet_PARAMS, cpssDxChTrunkTableEntrySet_preLogic},
    {"cpssDxChTrunkCascadeTrunkWithWeightedPortsGet", 4, cpssDxChTrunkCascadeTrunkWithWeightedPortsGet_PARAMS, cpssDxChTrunkDbEnabledMembersGet_preLogic},
    {"cpssDxChTrunkMemberAdd", 3, cpssDxChTrunkMemberAdd_PARAMS, NULL},
    {"cpssDxChTrunkMemberRemove", 3, cpssDxChTrunkMemberAdd_PARAMS, NULL},
    {"cpssDxChTrunkMemberDisable", 3, cpssDxChTrunkMemberAdd_PARAMS, NULL},
    {"cpssDxChTrunkMemberEnable", 3, cpssDxChTrunkMemberAdd_PARAMS, NULL},
    {"cpssDxChTrunkNonTrunkPortsAdd", 3, cpssDxChTrunkNonTrunkPortsAdd_PARAMS, NULL},
    {"cpssDxChTrunkNonTrunkPortsRemove", 3, cpssDxChTrunkNonTrunkPortsAdd_PARAMS, NULL},
    {"cpssDxChTrunkInit", 3, cpssDxChTrunkInit_PARAMS, NULL},
    {"cpssDxChTrunkHashMplsModeEnableSet", 2, prvCpssLogGenDevNumEnable_PARAMS, NULL},
    {"cpssDxChTrunkHashMplsModeEnableGet", 2, prvCpssLogGenDevNumEnablePtr_PARAMS, NULL},
    {"cpssDxChTrunkHashMaskSet", 3, cpssDxChTrunkHashMaskSet_PARAMS, NULL},
    {"cpssDxChTrunkHashMaskGet", 3, cpssDxChTrunkHashMaskGet_PARAMS, NULL},
    {"cpssDxChTrunkHashIpShiftSet", 4, cpssDxChTrunkHashIpShiftSet_PARAMS, NULL},
    {"cpssDxChTrunkHashIpShiftGet", 4, cpssDxChTrunkHashIpShiftGet_PARAMS, NULL},
    {"cpssDxChTrunkHashCrcParametersSet", 3, cpssDxChTrunkHashCrcParametersSet_PARAMS, NULL},
    {"cpssDxChTrunkHashCrcParametersGet", 3, cpssDxChTrunkHashCrcParametersGet_PARAMS, NULL},
    {"cpssDxChTrunkPortHashMaskInfoSet", 4, cpssDxChTrunkPortHashMaskInfoSet_PARAMS, NULL},
    {"cpssDxChTrunkPortHashMaskInfoGet", 4, cpssDxChTrunkPortHashMaskInfoGet_PARAMS, NULL},
    {"cpssDxChTrunkHashMaskCrcEntrySet", 3, cpssDxChTrunkHashMaskCrcEntrySet_PARAMS, NULL},
    {"cpssDxChTrunkHashMaskCrcEntryGet", 3, cpssDxChTrunkHashMaskCrcEntryGet_PARAMS, NULL},
    {"cpssDxChTrunkHashIndexCalculate", 6, cpssDxChTrunkHashIndexCalculate_PARAMS, NULL},
    {"cpssDxChTrunkHashPearsonValueSet", 3, prvCpssLogGenDevNumIndexValue_PARAMS, NULL},
    {"cpssDxChTrunkHashPearsonValueGet", 3, prvCpssLogGenDevNumIndexValuePtr_PARAMS, NULL},
    {"cpssDxChTrunkMcLocalSwitchingEnableSet", 3, cpssDxChTrunkMcLocalSwitchingEnableSet_PARAMS, NULL},
    {"cpssDxChTrunkDbMcLocalSwitchingEnableGet", 3, cpssDxChTrunkDbMcLocalSwitchingEnableGet_PARAMS, NULL},
    {"cpssDxChTrunkMemberSelectionModeSet", 2, prvCpssLogGenDevNumSelectionMode_PARAMS, NULL},
    {"cpssDxChTrunkMemberSelectionModeGet", 2, prvCpssLogGenDevNumSelectionModePtr_PARAMS, NULL},
    {"cpssDxChTrunkLearnPrioritySet", 3, cpssDxChTrunkLearnPrioritySet_PARAMS, NULL},
    {"cpssDxChTrunkLearnPriorityGet", 3, cpssDxChTrunkLearnPriorityGet_PARAMS, NULL},
    {"cpssDxChTrunkUserGroupSet", 3, cpssDxChTrunkUserGroupSet_PARAMS, NULL},
    {"cpssDxChTrunkUserGroupGet", 3, cpssDxChTrunkUserGroupGet_PARAMS, NULL},
    {"cpssDxChTrunkHashCrcSaltByteSet", 3, cpssDxChTrunkHashCrcSaltByteSet_PARAMS, NULL},
    {"cpssDxChTrunkHashCrcSaltByteGet", 3, cpssDxChTrunkHashCrcSaltByteGet_PARAMS, NULL},
    {"cpssDxChTrunkHashMaskCrcParamOverrideSet", 3, cpssDxChTrunkHashMaskCrcParamOverrideSet_PARAMS, NULL},
    {"cpssDxChTrunkHashMaskCrcParamOverrideGet", 3, cpssDxChTrunkHashMaskCrcParamOverrideGet_PARAMS, NULL},
    {"cpssDxChTrunkPortMcEnableSet", 3, cpssDxChTrunkPortMcEnableSet_PARAMS, NULL},
    {"cpssDxChTrunkDbPortMcEnableGet", 3, cpssDxChTrunkDbPortMcEnableGet_PARAMS, NULL},
    {"cpssDxChTrunkDbPortTrunkIdModeSet", 3, cpssDxChTrunkDbPortTrunkIdModeSet_PARAMS, NULL},
    {"cpssDxChTrunkDbPortTrunkIdModeGet", 3, cpssDxChTrunkDbPortTrunkIdModeGet_PARAMS, NULL},
};

/******** DB Access Function ********/
void prvCpssLogParamLibDbGet_CPSS_LOG_LIB_TRUNK(
    OUT PRV_CPSS_LOG_FUNC_ENTRY_STC ** logFuncDbPtrPtr,
    OUT GT_U32 * logFuncDbSizePtr
)
{
    *logFuncDbPtrPtr = prvCpssDxChTrunkLogLibDb;
    *logFuncDbSizePtr = sizeof(prvCpssDxChTrunkLogLibDb) / sizeof(PRV_CPSS_LOG_FUNC_ENTRY_STC);
}

