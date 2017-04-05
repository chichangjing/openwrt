/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* prvCpssDxChResourceManagerLog.h
*       WARNING!!! this is a generated file, please don't edit it manually
* COMMENTS:
*
* FILE REVISION NUMBER:
*       $Revision: 1 $
*******************************************************************************/
#ifndef __prvCpssDxChResourceManagerLogh
#define __prvCpssDxChResourceManagerLogh
#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
#include <cpss/generic/log/cpssLog.h>
#include <cpss/generic/log/prvCpssLog.h>


/********* manually implemented declarations *********/

void prvCpssLogParamFunc_CPSS_DXCH_TCAM_MANAGER_ENTRY_AMOUNT_STC_PTR(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
);


/********* enums *********/

PRV_CPSS_LOG_STC_ENUM_DECLARE_MAC(CPSS_DXCH_TCAM_MANAGER_ALLOC_METHOD_ENT);
PRV_CPSS_LOG_STC_ENUM_DECLARE_MAC(CPSS_DXCH_TCAM_MANAGER_DUMP_TYPE_ENT);
PRV_CPSS_LOG_STC_ENUM_DECLARE_MAC(CPSS_DXCH_TCAM_MANAGER_ENTRIES_RESERVATION_TYPE_ENT);
PRV_CPSS_LOG_STC_ENUM_DECLARE_MAC(CPSS_DXCH_TCAM_MANAGER_ENTRY_TYPE_ENT);
PRV_CPSS_LOG_STC_ENUM_DECLARE_MAC(CPSS_DXCH_TCAM_MANAGER_RANGE_UPDATE_METHOD_ENT);
PRV_CPSS_LOG_STC_ENUM_DECLARE_MAC(CPSS_DXCH_TCAM_MANAGER_TCAM_TYPE_ENT);


/********* structure fields log functions *********/

void prvCpssLogParamFuncStc_CPSS_DXCH_TCAM_MANAGER_CLIENT_FUNC_STC_PTR
(
    IN CPSS_LOG_LIB_ENT         contextLib,
    IN CPSS_LOG_TYPE_ENT        logType,
    IN GT_CHAR_PTR              namePtr,
    IN void                   * fieldPtr,
    INOUT PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC     * inOutParamInfoPtr
);
void prvCpssLogParamFuncStc_CPSS_DXCH_TCAM_MANAGER_ENTRY_AMOUNT_STC_PTR
(
    IN CPSS_LOG_LIB_ENT         contextLib,
    IN CPSS_LOG_TYPE_ENT        logType,
    IN GT_CHAR_PTR              namePtr,
    IN void                   * fieldPtr,
    INOUT PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC     * inOutParamInfoPtr
);
void prvCpssLogParamFuncStc_CPSS_DXCH_TCAM_MANAGER_RANGE_STC_PTR
(
    IN CPSS_LOG_LIB_ENT         contextLib,
    IN CPSS_LOG_TYPE_ENT        logType,
    IN GT_CHAR_PTR              namePtr,
    IN void                   * fieldPtr,
    INOUT PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC     * inOutParamInfoPtr
);
void prvCpssLogParamFuncStc_CPSS_DXCH_TCAM_MANAGER_TCAM_LOCATION_STC_PTR
(
    IN CPSS_LOG_LIB_ENT         contextLib,
    IN CPSS_LOG_TYPE_ENT        logType,
    IN GT_CHAR_PTR              namePtr,
    IN void                   * fieldPtr,
    INOUT PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC     * inOutParamInfoPtr
);


/********* parameters log functions *********/

void prvCpssLogParamFunc_CPSS_DXCH_TCAM_MANAGER_ALLOC_METHOD_ENT(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
);
void prvCpssLogParamFunc_CPSS_DXCH_TCAM_MANAGER_CLIENT_FUNC_STC_PTR(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
);
void prvCpssLogParamFunc_CPSS_DXCH_TCAM_MANAGER_DUMP_TYPE_ENT(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
);
void prvCpssLogParamFunc_CPSS_DXCH_TCAM_MANAGER_ENTRIES_RESERVATION_TYPE_ENT(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
);
void prvCpssLogParamFunc_CPSS_DXCH_TCAM_MANAGER_ENTRIES_RESERVATION_TYPE_ENT_PTR(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
);
void prvCpssLogParamFunc_CPSS_DXCH_TCAM_MANAGER_ENTRY_TYPE_ENT(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
);
void prvCpssLogParamFunc_CPSS_DXCH_TCAM_MANAGER_ENTRY_TYPE_ENT_PTR(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
);
void prvCpssLogParamFunc_CPSS_DXCH_TCAM_MANAGER_RANGE_STC_PTR(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
);
void prvCpssLogParamFunc_CPSS_DXCH_TCAM_MANAGER_RANGE_UPDATE_METHOD_ENT(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
);
void prvCpssLogParamFunc_CPSS_DXCH_TCAM_MANAGER_TCAM_LOCATION_STC_PTR(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
);
void prvCpssLogParamFunc_CPSS_DXCH_TCAM_MANAGER_TCAM_TYPE_ENT(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
);
void prvCpssLogParamFunc_CPSS_DXCH_TCAM_MANAGER_TCAM_TYPE_ENT_PTR(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
);


/********* API fields DB *********/

extern PRV_CPSS_LOG_FUNC_PARAM_STC INOUT_GT_U32_PTR_sizeOfArrayPtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC INOUT_GT_U32_PTR_sizeOfReservedArrayPtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC INOUT_GT_U32_PTR_sizeofAllocatedArrayPtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_DXCH_TCAM_MANAGER_ALLOC_METHOD_ENT_allocMethod;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_DXCH_TCAM_MANAGER_CLIENT_FUNC_STC_PTR_clientFuncPtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_DXCH_TCAM_MANAGER_DUMP_TYPE_ENT_dumpType;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_DXCH_TCAM_MANAGER_ENTRIES_RESERVATION_TYPE_ENT_reservationType;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_DXCH_TCAM_MANAGER_ENTRY_AMOUNT_STC_PTR_requestedEntriesArray;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_DXCH_TCAM_MANAGER_ENTRY_TYPE_ENT_entryType;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_DXCH_TCAM_MANAGER_RANGE_STC_PTR_rangePtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_DXCH_TCAM_MANAGER_RANGE_STC_PTR_reservationRangePtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_DXCH_TCAM_MANAGER_RANGE_UPDATE_METHOD_ENT_rangeUpdateMethod;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_DXCH_TCAM_MANAGER_TCAM_TYPE_ENT_tcamType;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_BOOL_defragEnable;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_BOOL_usageOfSpecificClient;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_CHAR_PTR_clientNameArr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_U32_clientId;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_U32_entryToken;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_U32_firstToken;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_U32_lowerBoundToken;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_U32_secondToken;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_U32_sizeOfArray;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_U32_upperBoundToken;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_VOID_PTR_clientCookiePtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_VOID_PTR_tcamManagerHandlerPtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_CPSS_DXCH_TCAM_MANAGER_ENTRIES_RESERVATION_TYPE_ENT_PTR_reservationTypePtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_CPSS_DXCH_TCAM_MANAGER_ENTRY_AMOUNT_STC_PTR_allocatedEntriesArray;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_CPSS_DXCH_TCAM_MANAGER_ENTRY_AMOUNT_STC_PTR_reservedEntriesArray;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_CPSS_DXCH_TCAM_MANAGER_ENTRY_TYPE_ENT_PTR_entryTypePtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_CPSS_DXCH_TCAM_MANAGER_RANGE_STC_PTR_rangePtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_CPSS_DXCH_TCAM_MANAGER_RANGE_STC_PTR_reservationRangePtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_CPSS_DXCH_TCAM_MANAGER_TCAM_LOCATION_STC_PTR_tcamLocationPtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_CPSS_DXCH_TCAM_MANAGER_TCAM_TYPE_ENT_PTR_tcamTypePtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_COMP_RES_PTR_resultPtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_U32_PTR_allocEntryTokenPtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_U32_PTR_clientIdPtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_VOID_PTR_PTR_clientCookiePtrPtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_VOID_PTR_PTR_tcamManagerHandlerPtrPtr;


/********* lib API DB *********/

enum {
    PRV_CPSS_LOG_FUNC_cpssDxChTcamManagerCreate_E = (CPSS_LOG_LIB_RESOURCE_MANAGER_E << 16),
    PRV_CPSS_LOG_FUNC_cpssDxChTcamManagerConfigGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChTcamManagerDelete_E,
    PRV_CPSS_LOG_FUNC_cpssDxChTcamManagerRangeUpdate_E,
    PRV_CPSS_LOG_FUNC_cpssDxChTcamManagerClientRegister_E,
    PRV_CPSS_LOG_FUNC_cpssDxChTcamManagerClientUnregister_E,
    PRV_CPSS_LOG_FUNC_cpssDxChTcamManagerClientFuncUpdate_E,
    PRV_CPSS_LOG_FUNC_cpssDxChTcamManagerEntriesReservationSet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChTcamManagerEntriesReservationGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChTcamManagerUsageGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChTcamManagerDump_E,
    PRV_CPSS_LOG_FUNC_cpssDxChTcamManagerEntryAllocate_E,
    PRV_CPSS_LOG_FUNC_cpssDxChTcamManagerEntryClientCookieUpdate_E,
    PRV_CPSS_LOG_FUNC_cpssDxChTcamManagerEntryFree_E,
    PRV_CPSS_LOG_FUNC_cpssDxChTcamManagerAvailableEntriesCheck_E,
    PRV_CPSS_LOG_FUNC_cpssDxChTcamManagerTokenToTcamLocation_E,
    PRV_CPSS_LOG_FUNC_cpssDxChTcamManagerTokenCompare_E,
    PRV_CPSS_LOG_FUNC_cpssDxChTcamManagerTokenInfoGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChTcamManagerHsuSizeGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChTcamManagerHsuExport_E,
    PRV_CPSS_LOG_FUNC_cpssDxChTcamManagerHsuImport_E,
    PRV_CPSS_LOG_FUNC_cpssDxChTcamManagerHsuInactiveClientDelete_E
};

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif /* __prvCpssDxChResourceManagerLogh */
