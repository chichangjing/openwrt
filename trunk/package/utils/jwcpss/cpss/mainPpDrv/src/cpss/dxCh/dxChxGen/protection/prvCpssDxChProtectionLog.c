/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* prvCpssDxChProtectionLog.c
*       WARNING!!! this is a generated file, please don't edit it manually
* COMMENTS:
*
* FILE REVISION NUMBER:
*       $Revision: 1 $
*******************************************************************************/
#include <cpss/generic/log/cpssLog.h>
#include <cpss/generic/log/prvCpssLog.h>
#include <cpss/extServices/private/prvCpssBindFunc.h>
#include <cpss/dxCh/dxChxGen/protection/cpssDxChProtection.h>
#include <cpss/dxCh/dxChxGen/protection/private/prvCpssDxChProtectionLog.h>
#include <cpss/generic/log/prvCpssGenCommonTypesLog.h>
#include <cpss/generic/log/prvCpssGenDbLog.h>


/********* enums *********/

char * prvCpssLogEnum_CPSS_DXCH_PROTECTION_LOC_STATUS_ENT[]  =
{
    "CPSS_DXCH_PROTECTION_LOC_NOT_DETECTED_E",
    "CPSS_DXCH_PROTECTION_LOC_DETECTED_E"
};
PRV_CPSS_LOG_STC_ENUM_ARRAY_SIZE_MAC(CPSS_DXCH_PROTECTION_LOC_STATUS_ENT);


/********* parameters log functions *********/

void prvCpssLogParamFunc_CPSS_DXCH_PROTECTION_LOC_STATUS_ENT(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_SET_PARAM_VAL_MAC(CPSS_DXCH_PROTECTION_LOC_STATUS_ENT, paramVal);
    PRV_CPSS_LOG_ENUM_MAC(namePtr, paramVal, CPSS_DXCH_PROTECTION_LOC_STATUS_ENT);
}
void prvCpssLogParamFunc_CPSS_DXCH_PROTECTION_LOC_STATUS_ENT_PTR(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_SET_PARAM_VAL_MAC(CPSS_DXCH_PROTECTION_LOC_STATUS_ENT*, paramVal);
    if (paramVal == NULL)
    {
        PRV_CPSS_LOG_AND_HISTORY_PARAM_MAC(contextLib, logType, "%s = NULL\n", namePtr);
        return;
    }
    PRV_CPSS_LOG_ENUM_MAC(namePtr, *paramVal, CPSS_DXCH_PROTECTION_LOC_STATUS_ENT);
}


/********* API fields DB *********/

PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_DXCH_PROTECTION_LOC_STATUS_ENT_status = {
     "status", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_MAC(CPSS_DXCH_PROTECTION_LOC_STATUS_ENT)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_U32_locTableIndex = {
     "locTableIndex", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_MAC(GT_U32)
};
PRV_CPSS_LOG_FUNC_PARAM_STC OUT_CPSS_DXCH_PROTECTION_LOC_STATUS_ENT_PTR_statusPtr = {
     "statusPtr", PRV_CPSS_LOG_PARAM_OUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(CPSS_DXCH_PROTECTION_LOC_STATUS_ENT)
};
PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_U32_PTR_locTableIndexPtr = {
     "locTableIndexPtr", PRV_CPSS_LOG_PARAM_OUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(GT_U32)
};


/********* API prototypes DB *********/

PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChProtectionPortToLocMappingSet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_PORT_NUM_portNum,
    &IN_GT_U32_locTableIndex
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChProtectionPortToLocMappingGet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_PORT_NUM_portNum,
    &OUT_GT_U32_PTR_locTableIndexPtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChProtectionLocStatusSet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_U32_index,
    &IN_CPSS_DXCH_PROTECTION_LOC_STATUS_ENT_status
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChProtectionLocStatusGet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_U32_index,
    &OUT_CPSS_DXCH_PROTECTION_LOC_STATUS_ENT_PTR_statusPtr
};


/********* lib API DB *********/

static PRV_CPSS_LOG_FUNC_ENTRY_STC prvCpssDxChProtectionLogLibDb[] = {
    {"cpssDxChProtectionEnableSet", 2, prvCpssLogGenDevNumEnable_PARAMS, NULL},
    {"cpssDxChProtectionEnableGet", 2, prvCpssLogGenDevNumEnablePtr_PARAMS, NULL},
    {"cpssDxChProtectionTxEnableSet", 3, prvCpssLogGenDevNumPortNumEnable_PARAMS, NULL},
    {"cpssDxChProtectionTxEnableGet", 3, prvCpssLogGenDevNumPortNumEnablePtr_PARAMS, NULL},
    {"cpssDxChProtectionPortToLocMappingSet", 3, cpssDxChProtectionPortToLocMappingSet_PARAMS, NULL},
    {"cpssDxChProtectionPortToLocMappingGet", 3, cpssDxChProtectionPortToLocMappingGet_PARAMS, NULL},
    {"cpssDxChProtectionLocStatusSet", 3, cpssDxChProtectionLocStatusSet_PARAMS, NULL},
    {"cpssDxChProtectionLocStatusGet", 3, cpssDxChProtectionLocStatusGet_PARAMS, NULL},
    {"cpssDxChProtectionRxExceptionPacketCommandSet", 2, prvCpssLogGenDevNumCommand_PARAMS, NULL},
    {"cpssDxChProtectionRxExceptionPacketCommandGet", 2, prvCpssLogGenDevNumCommandPtr_PARAMS, NULL},
    {"cpssDxChProtectionRxExceptionCpuCodeSet", 2, prvCpssLogGenDevNumCpuCode_PARAMS, NULL},
    {"cpssDxChProtectionRxExceptionCpuCodeGet", 2, prvCpssLogGenDevNumCpuCodePtr_PARAMS, NULL},
};

/******** DB Access Function ********/
void prvCpssLogParamLibDbGet_CPSS_LOG_LIB_PROTECTION(
    OUT PRV_CPSS_LOG_FUNC_ENTRY_STC ** logFuncDbPtrPtr,
    OUT GT_U32 * logFuncDbSizePtr
)
{
    *logFuncDbPtrPtr = prvCpssDxChProtectionLogLibDb;
    *logFuncDbSizePtr = sizeof(prvCpssDxChProtectionLogLibDb) / sizeof(PRV_CPSS_LOG_FUNC_ENTRY_STC);
}

