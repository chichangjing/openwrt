/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* prvCpssDxChSystemRecoveryLog.c
*       WARNING!!! this is a generated file, please don't edit it manually
* COMMENTS:
*
* FILE REVISION NUMBER:
*       $Revision: 1 $
*******************************************************************************/
#include <cpss/generic/log/cpssLog.h>
#include <cpss/generic/log/prvCpssLog.h>
#include <cpss/extServices/private/prvCpssBindFunc.h>
#include <cpss/dxCh/dxChxGen/systemRecovery/hsu/cpssDxChHsu.h>
#include <cpss/dxCh/dxChxGen/systemRecovery/private/prvCpssDxChSystemRecoveryLog.h>
#include <cpss/generic/log/prvCpssGenCommonTypesLog.h>
#include <cpss/generic/log/prvCpssGenDbLog.h>
#include <cpss/generic/systemRecovery/private/prvCpssGenSystemRecoveryLog.h>


/********* enums *********/

char * prvCpssLogEnum_CPSS_DXCH_HSU_DATA_TYPE_ENT[]  =
{
    "CPSS_DXCH_HSU_DATA_TYPE_LPM_DB_E",
    "CPSS_DXCH_HSU_DATA_TYPE_GLOBAL_E",
    "CPSS_DXCH_HSU_DATA_TYPE_ALL_E"
};
PRV_CPSS_LOG_STC_ENUM_ARRAY_SIZE_MAC(CPSS_DXCH_HSU_DATA_TYPE_ENT);


/********* parameters log functions *********/

void prvCpssLogParamFunc_CPSS_DXCH_HSU_DATA_TYPE_ENT(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_SET_PARAM_VAL_MAC(CPSS_DXCH_HSU_DATA_TYPE_ENT, paramVal);
    PRV_CPSS_LOG_ENUM_MAC(namePtr, paramVal, CPSS_DXCH_HSU_DATA_TYPE_ENT);
}


/********* API fields DB *********/

PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_DXCH_HSU_DATA_TYPE_ENT_dataType = {
     "dataType", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_MAC(CPSS_DXCH_HSU_DATA_TYPE_ENT)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_SYSTEM_RECOVERY_INFO_STC_PTR_systemRecoveryInfoPtr = {
     "systemRecoveryInfoPtr", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(CPSS_SYSTEM_RECOVERY_INFO_STC)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_U32_newEvReqHndl = {
     "newEvReqHndl", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_MAC(GT_U32)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_U32_oldEvReqHndl = {
     "oldEvReqHndl", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_MAC(GT_U32)
};


/********* API prototypes DB *********/

PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChHsuExport_PARAMS[] =  {
    &IN_CPSS_DXCH_HSU_DATA_TYPE_ENT_dataType,
    &INOUT_GT_UINTPTR_PTR_iteratorPtr,
    &INOUT_GT_U32_PTR_hsuBlockMemSizePtr,
    &IN_GT_U8_PTR_hsuBlockMemPtr,
    &OUT_GT_BOOL_PTR_exportCompletePtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChHsuImport_PARAMS[] =  {
    &IN_CPSS_DXCH_HSU_DATA_TYPE_ENT_dataType,
    &INOUT_GT_UINTPTR_PTR_iteratorPtr,
    &INOUT_GT_U32_PTR_hsuBlockMemSizePtr,
    &IN_GT_U8_PTR_hsuBlockMemPtr,
    &OUT_GT_BOOL_PTR_importCompletePtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChHsuBlockSizeGet_PARAMS[] =  {
    &IN_CPSS_DXCH_HSU_DATA_TYPE_ENT_dataType,
    &OUT_GT_U32_PTR_sizePtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssSystemRecoveryStateSet_PARAMS[] =  {
    &IN_CPSS_SYSTEM_RECOVERY_INFO_STC_PTR_systemRecoveryInfoPtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssHsuEventHandleUpdate_PARAMS[] =  {
    &IN_GT_U32_oldEvReqHndl,
    &IN_GT_U32_newEvReqHndl
};


/********* lib API DB *********/

static PRV_CPSS_LOG_FUNC_ENTRY_STC prvCpssDxChSystemRecoveryLogLibDb[] = {
    {"cpssDxChHsuBlockSizeGet", 2, cpssDxChHsuBlockSizeGet_PARAMS, NULL},
    {"cpssDxChHsuExport", 5, cpssDxChHsuExport_PARAMS, NULL},
    {"cpssDxChHsuImport", 5, cpssDxChHsuImport_PARAMS, NULL},
    {"cpssDxChCatchUpValidityCheckEnable", 1, prvCpssLogGenDevNum_PARAMS, NULL},
    {"cpssDxChCatchUpValidityCheck", 1, prvCpssLogGenDevNum_PARAMS, NULL},
    {"cpssSystemRecoveryStateSet", 1, cpssSystemRecoveryStateSet_PARAMS, NULL},
    {"cpssHsuEventHandleUpdate", 2, cpssHsuEventHandleUpdate_PARAMS, NULL},
};

/******** DB Access Function ********/
void prvCpssLogParamLibDbGet_CPSS_LOG_LIB_SYSTEM_RECOVERY(
    OUT PRV_CPSS_LOG_FUNC_ENTRY_STC ** logFuncDbPtrPtr,
    OUT GT_U32 * logFuncDbSizePtr
)
{
    *logFuncDbPtrPtr = prvCpssDxChSystemRecoveryLogLibDb;
    *logFuncDbSizePtr = sizeof(prvCpssDxChSystemRecoveryLogLibDb) / sizeof(PRV_CPSS_LOG_FUNC_ENTRY_STC);
}

