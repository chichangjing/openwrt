/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* prvCpssDxChLog.c
*       WARNING!!! this is a generated file, please don't edit it manually
* COMMENTS:
*
* FILE REVISION NUMBER:
*       $Revision: 1 $
*******************************************************************************/
#include <cpss/generic/log/cpssLog.h>
#include <cpss/generic/log/prvCpssLog.h>
#include <cpss/extServices/private/prvCpssBindFunc.h>
#include <cpss/dxCh/dxChxGen/cpssDxChTypes.h>
#include <cpss/dxCh/dxChxGen/log/private/prvCpssDxChLog.h>
#include <cpss/dxCh/dxChxGen/tunnel/private/prvCpssDxChTunnelLog.h>
#include <cpss/generic/log/prvCpssGenCommonTypesLog.h>
#include <cpss/generic/log/prvCpssGenPpTypesLog.h>


/********* enums *********/

char * prvCpssLogEnum_CPSS_DXCH_ETHERTYPE_TABLE_ENT[]  =
{
    "CPSS_DXCH_ETHERTYPE_TABLE_INGRESS_E",
    "CPSS_DXCH_ETHERTYPE_TABLE_EGRESS_E",
    "CPSS_DXCH_ETHERTYPE_TABLE_EGRESS_LOGICAL_PORT_E",
    "CPSS_DXCH_ETHERTYPE_TABLE_EGRESS_TS_E"
};
PRV_CPSS_LOG_STC_ENUM_ARRAY_SIZE_MAC(CPSS_DXCH_ETHERTYPE_TABLE_ENT);
char * prvCpssLogEnum_CPSS_DXCH_FDB_LEARN_PRIORITY_ENT[]  =
{
    "CPSS_DXCH_FDB_LEARN_PRIORITY_LOW_E",
    "CPSS_DXCH_FDB_LEARN_PRIORITY_HIGH_E"
};
PRV_CPSS_LOG_STC_ENUM_ARRAY_SIZE_MAC(CPSS_DXCH_FDB_LEARN_PRIORITY_ENT);
char * prvCpssLogEnum_CPSS_DXCH_MEMBER_SELECTION_MODE_ENT[]  =
{
    "CPSS_DXCH_MEMBER_SELECTION_MODE_12_BITS_E",
    "CPSS_DXCH_MEMBER_SELECTION_MODE_6_LSB_E",
    "CPSS_DXCH_MEMBER_SELECTION_MODE_6_MSB_E"
};
PRV_CPSS_LOG_STC_ENUM_ARRAY_SIZE_MAC(CPSS_DXCH_MEMBER_SELECTION_MODE_ENT);
char * prvCpssLogEnum_CPSS_DXCH_PAIR_READ_WRITE_FORM_ENT[]  =
{
    "CPSS_DXCH_PAIR_READ_WRITE_FIRST_ONLY_E",
    "CPSS_DXCH_PAIR_READ_WRITE_SECOND_NEXT_POINTER_ONLY_E",
    "CPSS_DXCH_PAIR_READ_WRITE_WHOLE_E"
};
PRV_CPSS_LOG_STC_ENUM_ARRAY_SIZE_MAC(CPSS_DXCH_PAIR_READ_WRITE_FORM_ENT);


/********* structure fields log functions *********/

void prvCpssLogParamFuncStc_CPSS_DXCH_OUTPUT_INTERFACE_STC_PTR
(
    IN CPSS_LOG_LIB_ENT         contextLib,
    IN CPSS_LOG_TYPE_ENT        logType,
    IN GT_CHAR_PTR              namePtr,
    IN void                   * fieldPtr,
    INOUT PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC     * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_START_FIELD_STC_MAC(CPSS_DXCH_OUTPUT_INTERFACE_STC *, valPtr);
    PRV_CPSS_LOG_UNUSED_MAC(inOutParamInfoPtr);
    PRV_CPSS_LOG_STC_BOOL_MAC(valPtr, isTunnelStart);
    prvCpssLogStcLogStart(contextLib,  logType, "tunnelStartInfo");
    PRV_CPSS_LOG_STC_ENUM_MAC((&valPtr->tunnelStartInfo), passengerPacketType, CPSS_DXCH_TUNNEL_PASSANGER_TYPE_ENT);
    PRV_CPSS_LOG_STC_NUMBER_MAC((&valPtr->tunnelStartInfo), ptr);
    prvCpssLogStcLogEnd(contextLib, logType);
    PRV_CPSS_LOG_STC_STC_MAC(valPtr, physicalInterface, CPSS_INTERFACE_INFO_STC);
    prvCpssLogStcLogEnd(contextLib, logType);
}


/********* parameters log functions *********/

void prvCpssLogParamFunc_CPSS_DXCH_ETHERTYPE_TABLE_ENT(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_SET_PARAM_VAL_MAC(CPSS_DXCH_ETHERTYPE_TABLE_ENT, paramVal);
    PRV_CPSS_LOG_ENUM_MAC(namePtr, paramVal, CPSS_DXCH_ETHERTYPE_TABLE_ENT);
}
void prvCpssLogParamFunc_CPSS_DXCH_FDB_LEARN_PRIORITY_ENT(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_SET_PARAM_VAL_MAC(CPSS_DXCH_FDB_LEARN_PRIORITY_ENT, paramVal);
    PRV_CPSS_LOG_ENUM_MAC(namePtr, paramVal, CPSS_DXCH_FDB_LEARN_PRIORITY_ENT);
}
void prvCpssLogParamFunc_CPSS_DXCH_FDB_LEARN_PRIORITY_ENT_PTR(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_SET_PARAM_VAL_MAC(CPSS_DXCH_FDB_LEARN_PRIORITY_ENT*, paramVal);
    if (paramVal == NULL)
    {
        PRV_CPSS_LOG_AND_HISTORY_PARAM_MAC(contextLib, logType, "%s = NULL\n", namePtr);
        return;
    }
    PRV_CPSS_LOG_ENUM_MAC(namePtr, *paramVal, CPSS_DXCH_FDB_LEARN_PRIORITY_ENT);
}
void prvCpssLogParamFunc_CPSS_DXCH_MEMBER_SELECTION_MODE_ENT(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_SET_PARAM_VAL_MAC(CPSS_DXCH_MEMBER_SELECTION_MODE_ENT, paramVal);
    PRV_CPSS_LOG_ENUM_MAC(namePtr, paramVal, CPSS_DXCH_MEMBER_SELECTION_MODE_ENT);
}
void prvCpssLogParamFunc_CPSS_DXCH_MEMBER_SELECTION_MODE_ENT_PTR(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_SET_PARAM_VAL_MAC(CPSS_DXCH_MEMBER_SELECTION_MODE_ENT*, paramVal);
    if (paramVal == NULL)
    {
        PRV_CPSS_LOG_AND_HISTORY_PARAM_MAC(contextLib, logType, "%s = NULL\n", namePtr);
        return;
    }
    PRV_CPSS_LOG_ENUM_MAC(namePtr, *paramVal, CPSS_DXCH_MEMBER_SELECTION_MODE_ENT);
}
void prvCpssLogParamFunc_CPSS_DXCH_PAIR_READ_WRITE_FORM_ENT(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_SET_PARAM_VAL_MAC(CPSS_DXCH_PAIR_READ_WRITE_FORM_ENT, paramVal);
    PRV_CPSS_LOG_ENUM_MAC(namePtr, paramVal, CPSS_DXCH_PAIR_READ_WRITE_FORM_ENT);
}

