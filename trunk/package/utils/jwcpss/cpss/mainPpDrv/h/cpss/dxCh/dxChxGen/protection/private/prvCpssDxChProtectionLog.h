/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* prvCpssDxChProtectionLog.h
*       WARNING!!! this is a generated file, please don't edit it manually
* COMMENTS:
*
* FILE REVISION NUMBER:
*       $Revision: 1 $
*******************************************************************************/
#ifndef __prvCpssDxChProtectionLogh
#define __prvCpssDxChProtectionLogh
#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
#include <cpss/generic/log/cpssLog.h>
#include <cpss/generic/log/prvCpssLog.h>


/********* enums *********/

PRV_CPSS_LOG_STC_ENUM_DECLARE_MAC(CPSS_DXCH_PROTECTION_LOC_STATUS_ENT);


/********* parameters log functions *********/

void prvCpssLogParamFunc_CPSS_DXCH_PROTECTION_LOC_STATUS_ENT(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
);
void prvCpssLogParamFunc_CPSS_DXCH_PROTECTION_LOC_STATUS_ENT_PTR(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
);


/********* API fields DB *********/

extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_DXCH_PROTECTION_LOC_STATUS_ENT_status;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_U32_locTableIndex;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_CPSS_DXCH_PROTECTION_LOC_STATUS_ENT_PTR_statusPtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_U32_PTR_locTableIndexPtr;


/********* lib API DB *********/

enum {
    PRV_CPSS_LOG_FUNC_cpssDxChProtectionEnableSet_E = (CPSS_LOG_LIB_PROTECTION_E << 16),
    PRV_CPSS_LOG_FUNC_cpssDxChProtectionEnableGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChProtectionTxEnableSet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChProtectionTxEnableGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChProtectionPortToLocMappingSet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChProtectionPortToLocMappingGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChProtectionLocStatusSet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChProtectionLocStatusGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChProtectionRxExceptionPacketCommandSet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChProtectionRxExceptionPacketCommandGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChProtectionRxExceptionCpuCodeSet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChProtectionRxExceptionCpuCodeGet_E
};

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif /* __prvCpssDxChProtectionLogh */
