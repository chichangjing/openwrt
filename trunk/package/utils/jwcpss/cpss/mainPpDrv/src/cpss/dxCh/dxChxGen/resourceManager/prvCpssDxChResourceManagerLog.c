/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* prvCpssDxChResourceManagerLog.c
*       WARNING!!! this is a generated file, please don't edit it manually
* COMMENTS:
*
* FILE REVISION NUMBER:
*       $Revision: 1 $
*******************************************************************************/
#include <cpss/generic/log/cpssLog.h>
#include <cpss/generic/log/prvCpssLog.h>
#include <cpss/extServices/private/prvCpssBindFunc.h>
#include <cpss/dxCh/dxChxGen/resourceManager/cpssDxChTcamManagerTypes.h>
#include <cpss/dxCh/dxChxGen/resourceManager/private/prvCpssDxChResourceManagerLog.h>
#include <cpss/generic/log/prvCpssGenCommonTypesLog.h>
#include <cpss/generic/log/prvCpssGenDbLog.h>


/********* enums *********/

char * prvCpssLogEnum_CPSS_DXCH_TCAM_MANAGER_ALLOC_METHOD_ENT[]  =
{
    "CPSS_DXCH_TCAM_MANAGER_ALLOC_MIN_E",
    "CPSS_DXCH_TCAM_MANAGER_ALLOC_MID_E",
    "CPSS_DXCH_TCAM_MANAGER_ALLOC_MAX_E"
};
PRV_CPSS_LOG_STC_ENUM_ARRAY_SIZE_MAC(CPSS_DXCH_TCAM_MANAGER_ALLOC_METHOD_ENT);
char * prvCpssLogEnum_CPSS_DXCH_TCAM_MANAGER_DUMP_TYPE_ENT[]  =
{
    "CPSS_DXCH_TCAM_MANAGER_DUMP_CLIENTS_E",
    "CPSS_DXCH_TCAM_MANAGER_DUMP_ENTRIES_FOR_SPECIFIC_CLIENT_E",
    "CPSS_DXCH_TCAM_MANAGER_DUMP_ENTRIES_FOR_ALL_CLIENTS_E",
    "CPSS_DXCH_TCAM_MANAGER_DUMP_CLIENTS_AND_ENTRIES_FOR_ALL_CLIENTS_E",
    "CPSS_DXCH_TCAM_MANAGER_DUMP_INTERNAL_INFO_E"
};
PRV_CPSS_LOG_STC_ENUM_ARRAY_SIZE_MAC(CPSS_DXCH_TCAM_MANAGER_DUMP_TYPE_ENT);
char * prvCpssLogEnum_CPSS_DXCH_TCAM_MANAGER_ENTRIES_RESERVATION_TYPE_ENT[]  =
{
    "CPSS_DXCH_TCAM_MANAGER_STATIC_ENTRIES_RESERVATION_E",
    "CPSS_DXCH_TCAM_MANAGER_DYNAMIC_ENTRIES_RESERVATION_E"
};
PRV_CPSS_LOG_STC_ENUM_ARRAY_SIZE_MAC(CPSS_DXCH_TCAM_MANAGER_ENTRIES_RESERVATION_TYPE_ENT);
char * prvCpssLogEnum_CPSS_DXCH_TCAM_MANAGER_ENTRY_TYPE_ENT[]  =
{
    "CPSS_DXCH_TCAM_MANAGER_SINGLE_ENTRY_E",
    "CPSS_DXCH_TCAM_MANAGER_DUAL_ENTRY_E",
    "CPSS_DXCH_TCAM_MANAGER_TRIPLE_ENTRY_E",
    "CPSS_DXCH_TCAM_MANAGER_QUAD_ENTRY_E"
};
PRV_CPSS_LOG_STC_ENUM_ARRAY_SIZE_MAC(CPSS_DXCH_TCAM_MANAGER_ENTRY_TYPE_ENT);
char * prvCpssLogEnum_CPSS_DXCH_TCAM_MANAGER_RANGE_UPDATE_METHOD_ENT[]  =
{
    "CPSS_DXCH_TCAM_MANAGER_DO_NOT_MOVE_RANGE_UPDATE_METHOD_E",
    "CPSS_DXCH_TCAM_MANAGER_MOVE_TOP_AND_COMPRESS_RANGE_UPDATE_METHOD_E",
    "CPSS_DXCH_TCAM_MANAGER_MOVE_BOTTOM_AND_COMPRESS_RANGE_UPDATE_METHOD_E",
    "CPSS_DXCH_TCAM_MANAGER_MOVE_MIDDLE_AND_COMPRESS_RANGE_UPDATE_METHOD_E",
    "CPSS_DXCH_TCAM_MANAGER_MOVE_TOP_AND_KEEP_OFFSETS_RANGE_UPDATE_METHOD_E"
};
PRV_CPSS_LOG_STC_ENUM_ARRAY_SIZE_MAC(CPSS_DXCH_TCAM_MANAGER_RANGE_UPDATE_METHOD_ENT);
char * prvCpssLogEnum_CPSS_DXCH_TCAM_MANAGER_TCAM_TYPE_ENT[]  =
{
    "CPSS_DXCH_TCAM_MANAGER_XCAT_AND_ABOVE_ROUTER_TCAM_E",
    "CPSS_DXCH_TCAM_MANAGER_XCAT_AND_ABOVE_PCL_TCAM_E"
};
PRV_CPSS_LOG_STC_ENUM_ARRAY_SIZE_MAC(CPSS_DXCH_TCAM_MANAGER_TCAM_TYPE_ENT);


/********* structure fields log functions *********/

void prvCpssLogParamFuncStc_CPSS_DXCH_TCAM_MANAGER_CLIENT_FUNC_STC_PTR
(
    IN CPSS_LOG_LIB_ENT         contextLib,
    IN CPSS_LOG_TYPE_ENT        logType,
    IN GT_CHAR_PTR              namePtr,
    IN void                   * fieldPtr,
    INOUT PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC     * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_START_FIELD_STC_MAC(CPSS_DXCH_TCAM_MANAGER_CLIENT_FUNC_STC *, valPtr);
    PRV_CPSS_LOG_UNUSED_MAC(inOutParamInfoPtr);
    PRV_CPSS_LOG_STC_PTR_MAC(valPtr, moveToLocationFuncPtr);
    PRV_CPSS_LOG_STC_PTR_MAC(valPtr, moveToAnywhereFuncPtr);
    PRV_CPSS_LOG_STC_PTR_MAC(valPtr, checkIfDefaultLocationFuncPtr);
    prvCpssLogStcLogEnd(contextLib, logType);
}
void prvCpssLogParamFuncStc_CPSS_DXCH_TCAM_MANAGER_ENTRY_AMOUNT_STC_PTR
(
    IN CPSS_LOG_LIB_ENT         contextLib,
    IN CPSS_LOG_TYPE_ENT        logType,
    IN GT_CHAR_PTR              namePtr,
    IN void                   * fieldPtr,
    INOUT PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC     * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_START_FIELD_STC_MAC(CPSS_DXCH_TCAM_MANAGER_ENTRY_AMOUNT_STC *, valPtr);
    PRV_CPSS_LOG_UNUSED_MAC(inOutParamInfoPtr);
    PRV_CPSS_LOG_STC_ENUM_MAC(valPtr, entryType, CPSS_DXCH_TCAM_MANAGER_ENTRY_TYPE_ENT);
    PRV_CPSS_LOG_STC_NUMBER_MAC(valPtr, amount);
    prvCpssLogStcLogEnd(contextLib, logType);
}
void prvCpssLogParamFuncStc_CPSS_DXCH_TCAM_MANAGER_RANGE_STC_PTR
(
    IN CPSS_LOG_LIB_ENT         contextLib,
    IN CPSS_LOG_TYPE_ENT        logType,
    IN GT_CHAR_PTR              namePtr,
    IN void                   * fieldPtr,
    INOUT PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC     * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_START_FIELD_STC_MAC(CPSS_DXCH_TCAM_MANAGER_RANGE_STC *, valPtr);
    PRV_CPSS_LOG_UNUSED_MAC(inOutParamInfoPtr);
    PRV_CPSS_LOG_STC_NUMBER_MAC(valPtr, firstLine);
    PRV_CPSS_LOG_STC_NUMBER_MAC(valPtr, lastLine);
    prvCpssLogStcLogEnd(contextLib, logType);
}
void prvCpssLogParamFuncStc_CPSS_DXCH_TCAM_MANAGER_TCAM_LOCATION_STC_PTR
(
    IN CPSS_LOG_LIB_ENT         contextLib,
    IN CPSS_LOG_TYPE_ENT        logType,
    IN GT_CHAR_PTR              namePtr,
    IN void                   * fieldPtr,
    INOUT PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC     * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_START_FIELD_STC_MAC(CPSS_DXCH_TCAM_MANAGER_TCAM_LOCATION_STC *, valPtr);
    PRV_CPSS_LOG_UNUSED_MAC(inOutParamInfoPtr);
    PRV_CPSS_LOG_STC_NUMBER_MAC(valPtr, row);
    PRV_CPSS_LOG_STC_NUMBER_MAC(valPtr, column);
    prvCpssLogStcLogEnd(contextLib, logType);
}


/********* parameters log functions *********/

void prvCpssLogParamFunc_CPSS_DXCH_TCAM_MANAGER_ALLOC_METHOD_ENT(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_SET_PARAM_VAL_MAC(CPSS_DXCH_TCAM_MANAGER_ALLOC_METHOD_ENT, paramVal);
    PRV_CPSS_LOG_ENUM_MAC(namePtr, paramVal, CPSS_DXCH_TCAM_MANAGER_ALLOC_METHOD_ENT);
}
void prvCpssLogParamFunc_CPSS_DXCH_TCAM_MANAGER_CLIENT_FUNC_STC_PTR(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_START_PARAM_STC_MAC(CPSS_DXCH_TCAM_MANAGER_CLIENT_FUNC_STC*, paramVal);
    prvCpssLogParamFuncStc_CPSS_DXCH_TCAM_MANAGER_CLIENT_FUNC_STC_PTR(contextLib, logType, namePtr, paramVal, inOutParamInfoPtr);
}
void prvCpssLogParamFunc_CPSS_DXCH_TCAM_MANAGER_DUMP_TYPE_ENT(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_SET_PARAM_VAL_MAC(CPSS_DXCH_TCAM_MANAGER_DUMP_TYPE_ENT, paramVal);
    PRV_CPSS_LOG_ENUM_MAC(namePtr, paramVal, CPSS_DXCH_TCAM_MANAGER_DUMP_TYPE_ENT);
}
void prvCpssLogParamFunc_CPSS_DXCH_TCAM_MANAGER_ENTRIES_RESERVATION_TYPE_ENT(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_SET_PARAM_VAL_MAC(CPSS_DXCH_TCAM_MANAGER_ENTRIES_RESERVATION_TYPE_ENT, paramVal);
    PRV_CPSS_LOG_ENUM_MAC(namePtr, paramVal, CPSS_DXCH_TCAM_MANAGER_ENTRIES_RESERVATION_TYPE_ENT);
}
void prvCpssLogParamFunc_CPSS_DXCH_TCAM_MANAGER_ENTRIES_RESERVATION_TYPE_ENT_PTR(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_SET_PARAM_VAL_MAC(CPSS_DXCH_TCAM_MANAGER_ENTRIES_RESERVATION_TYPE_ENT*, paramVal);
    if (paramVal == NULL)
    {
        PRV_CPSS_LOG_AND_HISTORY_PARAM_MAC(contextLib, logType, "%s = NULL\n", namePtr);
        return;
    }
    PRV_CPSS_LOG_ENUM_MAC(namePtr, *paramVal, CPSS_DXCH_TCAM_MANAGER_ENTRIES_RESERVATION_TYPE_ENT);
}
void prvCpssLogParamFunc_CPSS_DXCH_TCAM_MANAGER_ENTRY_TYPE_ENT(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_SET_PARAM_VAL_MAC(CPSS_DXCH_TCAM_MANAGER_ENTRY_TYPE_ENT, paramVal);
    PRV_CPSS_LOG_ENUM_MAC(namePtr, paramVal, CPSS_DXCH_TCAM_MANAGER_ENTRY_TYPE_ENT);
}
void prvCpssLogParamFunc_CPSS_DXCH_TCAM_MANAGER_ENTRY_TYPE_ENT_PTR(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_SET_PARAM_VAL_MAC(CPSS_DXCH_TCAM_MANAGER_ENTRY_TYPE_ENT*, paramVal);
    if (paramVal == NULL)
    {
        PRV_CPSS_LOG_AND_HISTORY_PARAM_MAC(contextLib, logType, "%s = NULL\n", namePtr);
        return;
    }
    PRV_CPSS_LOG_ENUM_MAC(namePtr, *paramVal, CPSS_DXCH_TCAM_MANAGER_ENTRY_TYPE_ENT);
}
void prvCpssLogParamFunc_CPSS_DXCH_TCAM_MANAGER_RANGE_STC_PTR(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_START_PARAM_STC_MAC(CPSS_DXCH_TCAM_MANAGER_RANGE_STC*, paramVal);
    prvCpssLogParamFuncStc_CPSS_DXCH_TCAM_MANAGER_RANGE_STC_PTR(contextLib, logType, namePtr, paramVal, inOutParamInfoPtr);
}
void prvCpssLogParamFunc_CPSS_DXCH_TCAM_MANAGER_RANGE_UPDATE_METHOD_ENT(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_SET_PARAM_VAL_MAC(CPSS_DXCH_TCAM_MANAGER_RANGE_UPDATE_METHOD_ENT, paramVal);
    PRV_CPSS_LOG_ENUM_MAC(namePtr, paramVal, CPSS_DXCH_TCAM_MANAGER_RANGE_UPDATE_METHOD_ENT);
}
void prvCpssLogParamFunc_CPSS_DXCH_TCAM_MANAGER_TCAM_LOCATION_STC_PTR(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_START_PARAM_STC_MAC(CPSS_DXCH_TCAM_MANAGER_TCAM_LOCATION_STC*, paramVal);
    prvCpssLogParamFuncStc_CPSS_DXCH_TCAM_MANAGER_TCAM_LOCATION_STC_PTR(contextLib, logType, namePtr, paramVal, inOutParamInfoPtr);
}
void prvCpssLogParamFunc_CPSS_DXCH_TCAM_MANAGER_TCAM_TYPE_ENT(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_SET_PARAM_VAL_MAC(CPSS_DXCH_TCAM_MANAGER_TCAM_TYPE_ENT, paramVal);
    PRV_CPSS_LOG_ENUM_MAC(namePtr, paramVal, CPSS_DXCH_TCAM_MANAGER_TCAM_TYPE_ENT);
}
void prvCpssLogParamFunc_CPSS_DXCH_TCAM_MANAGER_TCAM_TYPE_ENT_PTR(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_SET_PARAM_VAL_MAC(CPSS_DXCH_TCAM_MANAGER_TCAM_TYPE_ENT*, paramVal);
    if (paramVal == NULL)
    {
        PRV_CPSS_LOG_AND_HISTORY_PARAM_MAC(contextLib, logType, "%s = NULL\n", namePtr);
        return;
    }
    PRV_CPSS_LOG_ENUM_MAC(namePtr, *paramVal, CPSS_DXCH_TCAM_MANAGER_TCAM_TYPE_ENT);
}


/********* API fields DB *********/

PRV_CPSS_LOG_FUNC_PARAM_STC INOUT_GT_U32_PTR_sizeOfArrayPtr = {
     "sizeOfArrayPtr", PRV_CPSS_LOG_PARAM_INOUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(GT_U32)
};
PRV_CPSS_LOG_FUNC_PARAM_STC INOUT_GT_U32_PTR_sizeOfReservedArrayPtr = {
     "sizeOfReservedArrayPtr", PRV_CPSS_LOG_PARAM_INOUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(GT_U32)
};
PRV_CPSS_LOG_FUNC_PARAM_STC INOUT_GT_U32_PTR_sizeofAllocatedArrayPtr = {
     "sizeofAllocatedArrayPtr", PRV_CPSS_LOG_PARAM_INOUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(GT_U32)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_DXCH_TCAM_MANAGER_ALLOC_METHOD_ENT_allocMethod = {
     "allocMethod", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_MAC(CPSS_DXCH_TCAM_MANAGER_ALLOC_METHOD_ENT)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_DXCH_TCAM_MANAGER_CLIENT_FUNC_STC_PTR_clientFuncPtr = {
     "clientFuncPtr", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(CPSS_DXCH_TCAM_MANAGER_CLIENT_FUNC_STC)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_DXCH_TCAM_MANAGER_DUMP_TYPE_ENT_dumpType = {
     "dumpType", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_MAC(CPSS_DXCH_TCAM_MANAGER_DUMP_TYPE_ENT)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_DXCH_TCAM_MANAGER_ENTRIES_RESERVATION_TYPE_ENT_reservationType = {
     "reservationType", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_MAC(CPSS_DXCH_TCAM_MANAGER_ENTRIES_RESERVATION_TYPE_ENT)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_DXCH_TCAM_MANAGER_ENTRY_AMOUNT_STC_PTR_requestedEntriesArray = {
     "requestedEntriesArray", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(CPSS_DXCH_TCAM_MANAGER_ENTRY_AMOUNT_STC)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_DXCH_TCAM_MANAGER_ENTRY_TYPE_ENT_entryType = {
     "entryType", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_MAC(CPSS_DXCH_TCAM_MANAGER_ENTRY_TYPE_ENT)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_DXCH_TCAM_MANAGER_RANGE_STC_PTR_rangePtr = {
     "rangePtr", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(CPSS_DXCH_TCAM_MANAGER_RANGE_STC)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_DXCH_TCAM_MANAGER_RANGE_STC_PTR_reservationRangePtr = {
     "reservationRangePtr", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(CPSS_DXCH_TCAM_MANAGER_RANGE_STC)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_DXCH_TCAM_MANAGER_RANGE_UPDATE_METHOD_ENT_rangeUpdateMethod = {
     "rangeUpdateMethod", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_MAC(CPSS_DXCH_TCAM_MANAGER_RANGE_UPDATE_METHOD_ENT)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_DXCH_TCAM_MANAGER_TCAM_TYPE_ENT_tcamType = {
     "tcamType", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_MAC(CPSS_DXCH_TCAM_MANAGER_TCAM_TYPE_ENT)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_BOOL_defragEnable = {
     "defragEnable", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_MAC(GT_BOOL)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_BOOL_usageOfSpecificClient = {
     "usageOfSpecificClient", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_MAC(GT_BOOL)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_CHAR_PTR_clientNameArr = {
     "clientNameArr", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(GT_CHAR)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_U32_clientId = {
     "clientId", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_MAC(GT_U32)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_U32_entryToken = {
     "entryToken", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_MAC(GT_U32)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_U32_firstToken = {
     "firstToken", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_MAC(GT_U32)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_U32_lowerBoundToken = {
     "lowerBoundToken", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_MAC(GT_U32)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_U32_secondToken = {
     "secondToken", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_MAC(GT_U32)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_U32_sizeOfArray = {
     "sizeOfArray", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_MAC(GT_U32)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_U32_upperBoundToken = {
     "upperBoundToken", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_MAC(GT_U32)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_VOID_PTR_clientCookiePtr = {
     "clientCookiePtr", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(GT_VOID)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_VOID_PTR_tcamManagerHandlerPtr = {
     "tcamManagerHandlerPtr", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(GT_VOID)
};
PRV_CPSS_LOG_FUNC_PARAM_STC OUT_CPSS_DXCH_TCAM_MANAGER_ENTRIES_RESERVATION_TYPE_ENT_PTR_reservationTypePtr = {
     "reservationTypePtr", PRV_CPSS_LOG_PARAM_OUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(CPSS_DXCH_TCAM_MANAGER_ENTRIES_RESERVATION_TYPE_ENT)
};
PRV_CPSS_LOG_FUNC_PARAM_STC OUT_CPSS_DXCH_TCAM_MANAGER_ENTRY_AMOUNT_STC_PTR_allocatedEntriesArray = {
     "allocatedEntriesArray", PRV_CPSS_LOG_PARAM_OUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(CPSS_DXCH_TCAM_MANAGER_ENTRY_AMOUNT_STC)
};
PRV_CPSS_LOG_FUNC_PARAM_STC OUT_CPSS_DXCH_TCAM_MANAGER_ENTRY_AMOUNT_STC_PTR_reservedEntriesArray = {
     "reservedEntriesArray", PRV_CPSS_LOG_PARAM_OUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(CPSS_DXCH_TCAM_MANAGER_ENTRY_AMOUNT_STC)
};
PRV_CPSS_LOG_FUNC_PARAM_STC OUT_CPSS_DXCH_TCAM_MANAGER_ENTRY_TYPE_ENT_PTR_entryTypePtr = {
     "entryTypePtr", PRV_CPSS_LOG_PARAM_OUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(CPSS_DXCH_TCAM_MANAGER_ENTRY_TYPE_ENT)
};
PRV_CPSS_LOG_FUNC_PARAM_STC OUT_CPSS_DXCH_TCAM_MANAGER_RANGE_STC_PTR_rangePtr = {
     "rangePtr", PRV_CPSS_LOG_PARAM_OUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(CPSS_DXCH_TCAM_MANAGER_RANGE_STC)
};
PRV_CPSS_LOG_FUNC_PARAM_STC OUT_CPSS_DXCH_TCAM_MANAGER_RANGE_STC_PTR_reservationRangePtr = {
     "reservationRangePtr", PRV_CPSS_LOG_PARAM_OUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(CPSS_DXCH_TCAM_MANAGER_RANGE_STC)
};
PRV_CPSS_LOG_FUNC_PARAM_STC OUT_CPSS_DXCH_TCAM_MANAGER_TCAM_LOCATION_STC_PTR_tcamLocationPtr = {
     "tcamLocationPtr", PRV_CPSS_LOG_PARAM_OUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(CPSS_DXCH_TCAM_MANAGER_TCAM_LOCATION_STC)
};
PRV_CPSS_LOG_FUNC_PARAM_STC OUT_CPSS_DXCH_TCAM_MANAGER_TCAM_TYPE_ENT_PTR_tcamTypePtr = {
     "tcamTypePtr", PRV_CPSS_LOG_PARAM_OUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(CPSS_DXCH_TCAM_MANAGER_TCAM_TYPE_ENT)
};
PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_COMP_RES_PTR_resultPtr = {
     "resultPtr", PRV_CPSS_LOG_PARAM_OUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(GT_COMP_RES)
};
PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_U32_PTR_allocEntryTokenPtr = {
     "allocEntryTokenPtr", PRV_CPSS_LOG_PARAM_OUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(GT_U32)
};
PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_U32_PTR_clientIdPtr = {
     "clientIdPtr", PRV_CPSS_LOG_PARAM_OUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(GT_U32)
};
PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_VOID_PTR_PTR_clientCookiePtrPtr = {
     "clientCookiePtrPtr", PRV_CPSS_LOG_PARAM_OUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_PTR_MAC(GT_VOID)
};
PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_VOID_PTR_PTR_tcamManagerHandlerPtrPtr = {
     "tcamManagerHandlerPtrPtr", PRV_CPSS_LOG_PARAM_OUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_PTR_MAC(GT_VOID)
};


/********* API prototypes DB *********/

PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChTcamManagerCreate_PARAMS[] =  {
    &IN_CPSS_DXCH_TCAM_MANAGER_TCAM_TYPE_ENT_tcamType,
    &IN_CPSS_DXCH_TCAM_MANAGER_RANGE_STC_PTR_rangePtr,
    &OUT_GT_VOID_PTR_PTR_tcamManagerHandlerPtrPtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChTcamManagerDelete_PARAMS[] =  {
    &IN_GT_VOID_PTR_tcamManagerHandlerPtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChTcamManagerDump_PARAMS[] =  {
    &IN_GT_VOID_PTR_tcamManagerHandlerPtr,
    &IN_CPSS_DXCH_TCAM_MANAGER_DUMP_TYPE_ENT_dumpType,
    &IN_GT_U32_clientId
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChTcamManagerRangeUpdate_PARAMS[] =  {
    &IN_GT_VOID_PTR_tcamManagerHandlerPtr,
    &IN_CPSS_DXCH_TCAM_MANAGER_RANGE_STC_PTR_rangePtr,
    &IN_CPSS_DXCH_TCAM_MANAGER_RANGE_UPDATE_METHOD_ENT_rangeUpdateMethod
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChTcamManagerClientUnregister_PARAMS[] =  {
    &IN_GT_VOID_PTR_tcamManagerHandlerPtr,
    &IN_GT_U32_clientId
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChTcamManagerClientFuncUpdate_PARAMS[] =  {
    &IN_GT_VOID_PTR_tcamManagerHandlerPtr,
    &IN_GT_U32_clientId,
    &IN_CPSS_DXCH_TCAM_MANAGER_CLIENT_FUNC_STC_PTR_clientFuncPtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChTcamManagerEntriesReservationSet_PARAMS[] =  {
    &IN_GT_VOID_PTR_tcamManagerHandlerPtr,
    &IN_GT_U32_clientId,
    &IN_CPSS_DXCH_TCAM_MANAGER_ENTRIES_RESERVATION_TYPE_ENT_reservationType,
    &IN_CPSS_DXCH_TCAM_MANAGER_ENTRY_AMOUNT_STC_PTR_requestedEntriesArray,
    &IN_GT_U32_sizeOfArray,
    &IN_CPSS_DXCH_TCAM_MANAGER_RANGE_STC_PTR_reservationRangePtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChTcamManagerAvailableEntriesCheck_PARAMS[] =  {
    &IN_GT_VOID_PTR_tcamManagerHandlerPtr,
    &IN_GT_U32_clientId,
    &IN_CPSS_DXCH_TCAM_MANAGER_ENTRY_AMOUNT_STC_PTR_requestedEntriesArray,
    &IN_GT_U32_sizeOfArray
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChTcamManagerEntryAllocate_PARAMS[] =  {
    &IN_GT_VOID_PTR_tcamManagerHandlerPtr,
    &IN_GT_U32_clientId,
    &IN_CPSS_DXCH_TCAM_MANAGER_ENTRY_TYPE_ENT_entryType,
    &IN_GT_U32_lowerBoundToken,
    &IN_GT_U32_upperBoundToken,
    &IN_CPSS_DXCH_TCAM_MANAGER_ALLOC_METHOD_ENT_allocMethod,
    &IN_GT_BOOL_defragEnable,
    &IN_GT_VOID_PTR_clientCookiePtr,
    &OUT_GT_U32_PTR_allocEntryTokenPtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChTcamManagerEntryFree_PARAMS[] =  {
    &IN_GT_VOID_PTR_tcamManagerHandlerPtr,
    &IN_GT_U32_clientId,
    &IN_GT_U32_entryToken
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChTcamManagerEntryClientCookieUpdate_PARAMS[] =  {
    &IN_GT_VOID_PTR_tcamManagerHandlerPtr,
    &IN_GT_U32_clientId,
    &IN_GT_U32_entryToken,
    &IN_GT_VOID_PTR_clientCookiePtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChTcamManagerTokenInfoGet_PARAMS[] =  {
    &IN_GT_VOID_PTR_tcamManagerHandlerPtr,
    &IN_GT_U32_clientId,
    &IN_GT_U32_entryToken,
    &OUT_CPSS_DXCH_TCAM_MANAGER_ENTRY_TYPE_ENT_PTR_entryTypePtr,
    &OUT_GT_VOID_PTR_PTR_clientCookiePtrPtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChTcamManagerTokenToTcamLocation_PARAMS[] =  {
    &IN_GT_VOID_PTR_tcamManagerHandlerPtr,
    &IN_GT_U32_clientId,
    &IN_GT_U32_entryToken,
    &OUT_CPSS_DXCH_TCAM_MANAGER_TCAM_LOCATION_STC_PTR_tcamLocationPtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChTcamManagerTokenCompare_PARAMS[] =  {
    &IN_GT_VOID_PTR_tcamManagerHandlerPtr,
    &IN_GT_U32_clientId,
    &IN_GT_U32_firstToken,
    &IN_GT_U32_secondToken,
    &OUT_GT_COMP_RES_PTR_resultPtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChTcamManagerEntriesReservationGet_PARAMS[] =  {
    &IN_GT_VOID_PTR_tcamManagerHandlerPtr,
    &IN_GT_U32_clientId,
    &OUT_CPSS_DXCH_TCAM_MANAGER_ENTRIES_RESERVATION_TYPE_ENT_PTR_reservationTypePtr,
    &OUT_CPSS_DXCH_TCAM_MANAGER_ENTRY_AMOUNT_STC_PTR_reservedEntriesArray,
    &INOUT_GT_U32_PTR_sizeOfArrayPtr,
    &OUT_CPSS_DXCH_TCAM_MANAGER_RANGE_STC_PTR_reservationRangePtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChTcamManagerHsuExport_PARAMS[] =  {
    &IN_GT_VOID_PTR_tcamManagerHandlerPtr,
    &INOUT_GT_UINTPTR_PTR_iteratorPtr,
    &INOUT_GT_U32_PTR_hsuBlockMemSizePtr,
    &IN_GT_U8_PTR_hsuBlockMemPtr,
    &OUT_GT_BOOL_PTR_exportCompletePtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChTcamManagerHsuImport_PARAMS[] =  {
    &IN_GT_VOID_PTR_tcamManagerHandlerPtr,
    &INOUT_GT_UINTPTR_PTR_iteratorPtr,
    &INOUT_GT_U32_PTR_hsuBlockMemSizePtr,
    &IN_GT_U8_PTR_hsuBlockMemPtr,
    &OUT_GT_BOOL_PTR_importCompletePtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChTcamManagerConfigGet_PARAMS[] =  {
    &IN_GT_VOID_PTR_tcamManagerHandlerPtr,
    &OUT_CPSS_DXCH_TCAM_MANAGER_TCAM_TYPE_ENT_PTR_tcamTypePtr,
    &OUT_CPSS_DXCH_TCAM_MANAGER_RANGE_STC_PTR_rangePtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChTcamManagerHsuSizeGet_PARAMS[] =  {
    &IN_GT_VOID_PTR_tcamManagerHandlerPtr,
    &OUT_GT_U32_PTR_sizePtr
};
extern PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChTcamManagerClientRegister_PARAMS[];
extern PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChTcamManagerUsageGet_PARAMS[];


/********* lib API DB *********/

extern void cpssDxChTcamManagerEntriesReservationSet_preLogic(IN va_list  args, OUT PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC * paramDataPtr);
extern void cpssDxChTcamManagerEntriesReservationGet_preLogic(IN va_list  args, OUT PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC * paramDataPtr);
extern void cpssDxChTcamManagerUsageGet_preLogic(IN va_list  args, OUT PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC * paramDataPtr);
extern void cpssDxChTcamManagerAvailableEntriesCheck_preLogic(IN va_list  args, OUT PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC * paramDataPtr);

static PRV_CPSS_LOG_FUNC_ENTRY_STC prvCpssDxChResourceManagerLogLibDb[] = {
    {"cpssDxChTcamManagerCreate", 3, cpssDxChTcamManagerCreate_PARAMS, NULL},
    {"cpssDxChTcamManagerConfigGet", 3, cpssDxChTcamManagerConfigGet_PARAMS, NULL},
    {"cpssDxChTcamManagerDelete", 1, cpssDxChTcamManagerDelete_PARAMS, NULL},
    {"cpssDxChTcamManagerRangeUpdate", 3, cpssDxChTcamManagerRangeUpdate_PARAMS, NULL},
    {"cpssDxChTcamManagerClientRegister", 4, cpssDxChTcamManagerClientRegister_PARAMS, NULL},
    {"cpssDxChTcamManagerClientUnregister", 2, cpssDxChTcamManagerClientUnregister_PARAMS, NULL},
    {"cpssDxChTcamManagerClientFuncUpdate", 3, cpssDxChTcamManagerClientFuncUpdate_PARAMS, NULL},
    {"cpssDxChTcamManagerEntriesReservationSet", 6, cpssDxChTcamManagerEntriesReservationSet_PARAMS, cpssDxChTcamManagerEntriesReservationSet_preLogic},
    {"cpssDxChTcamManagerEntriesReservationGet", 6, cpssDxChTcamManagerEntriesReservationGet_PARAMS, cpssDxChTcamManagerEntriesReservationGet_preLogic},
    {"cpssDxChTcamManagerUsageGet", 7, cpssDxChTcamManagerUsageGet_PARAMS, cpssDxChTcamManagerUsageGet_preLogic},
    {"cpssDxChTcamManagerDump", 3, cpssDxChTcamManagerDump_PARAMS, NULL},
    {"cpssDxChTcamManagerEntryAllocate", 9, cpssDxChTcamManagerEntryAllocate_PARAMS, NULL},
    {"cpssDxChTcamManagerEntryClientCookieUpdate", 4, cpssDxChTcamManagerEntryClientCookieUpdate_PARAMS, NULL},
    {"cpssDxChTcamManagerEntryFree", 3, cpssDxChTcamManagerEntryFree_PARAMS, NULL},
    {"cpssDxChTcamManagerAvailableEntriesCheck", 4, cpssDxChTcamManagerAvailableEntriesCheck_PARAMS, cpssDxChTcamManagerAvailableEntriesCheck_preLogic},
    {"cpssDxChTcamManagerTokenToTcamLocation", 4, cpssDxChTcamManagerTokenToTcamLocation_PARAMS, NULL},
    {"cpssDxChTcamManagerTokenCompare", 5, cpssDxChTcamManagerTokenCompare_PARAMS, NULL},
    {"cpssDxChTcamManagerTokenInfoGet", 5, cpssDxChTcamManagerTokenInfoGet_PARAMS, NULL},
    {"cpssDxChTcamManagerHsuSizeGet", 2, cpssDxChTcamManagerHsuSizeGet_PARAMS, NULL},
    {"cpssDxChTcamManagerHsuExport", 5, cpssDxChTcamManagerHsuExport_PARAMS, NULL},
    {"cpssDxChTcamManagerHsuImport", 5, cpssDxChTcamManagerHsuImport_PARAMS, NULL},
    {"cpssDxChTcamManagerHsuInactiveClientDelete", 1, cpssDxChTcamManagerDelete_PARAMS, NULL},
};

/******** DB Access Function ********/
void prvCpssLogParamLibDbGet_CPSS_LOG_LIB_RESOURCE_MANAGER(
    OUT PRV_CPSS_LOG_FUNC_ENTRY_STC ** logFuncDbPtrPtr,
    OUT GT_U32 * logFuncDbSizePtr
)
{
    *logFuncDbPtrPtr = prvCpssDxChResourceManagerLogLibDb;
    *logFuncDbSizePtr = sizeof(prvCpssDxChResourceManagerLogLibDb) / sizeof(PRV_CPSS_LOG_FUNC_ENTRY_STC);
}

