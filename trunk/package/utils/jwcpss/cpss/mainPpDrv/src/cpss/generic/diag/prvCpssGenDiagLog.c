/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* prvCpssGenDiagLog.c
*       WARNING!!! this is a generated file, please don't edit it manually
* COMMENTS:
*
* FILE REVISION NUMBER:
*       $Revision: 1 $
*******************************************************************************/
#include <cpss/generic/log/cpssLog.h>
#include <cpss/generic/log/prvCpssLog.h>
#include <cpss/extServices/private/prvCpssBindFunc.h>
#include <cpss/generic/diag/cpssDiag.h>
#include <cpss/generic/diag/private/prvCpssGenDiagLog.h>
#include <cpss/generic/log/prvCpssGenCommonTypesLog.h>


/********* enums *********/

char * prvCpssLogEnum_CPSS_DIAG_DATA_INTEGRITY_ERROR_CAUSE_TYPE_ENT[]  =
{
    "CPSS_DIAG_DATA_INTEGRITY_ERROR_CAUSE_TYPE_PARITY_E",
    "CPSS_DIAG_DATA_INTEGRITY_ERROR_CAUSE_TYPE_SINGLE_ECC_E",
    "CPSS_DIAG_DATA_INTEGRITY_ERROR_CAUSE_TYPE_MULTIPLE_ECC_E",
    "CPSS_DIAG_DATA_INTEGRITY_ERROR_CAUSE_TYPE_SINGLE_AND_MULTIPLE_ECC_E"
};
PRV_CPSS_LOG_STC_ENUM_ARRAY_SIZE_MAC(CPSS_DIAG_DATA_INTEGRITY_ERROR_CAUSE_TYPE_ENT);
char * prvCpssLogEnum_CPSS_DIAG_DATA_INTEGRITY_ERROR_INJECT_MODE_ENT[]  =
{
    "CPSS_DIAG_DATA_INTEGRITY_ERROR_INJECT_MODE_SINGLE_E",
    "CPSS_DIAG_DATA_INTEGRITY_ERROR_INJECT_MODE_MULTIPLE_E"
};
PRV_CPSS_LOG_STC_ENUM_ARRAY_SIZE_MAC(CPSS_DIAG_DATA_INTEGRITY_ERROR_INJECT_MODE_ENT);
char * prvCpssLogEnum_CPSS_DIAG_DATA_INTEGRITY_MEM_ERROR_PROTECTION_TYPE_ENT[]  =
{
    "CPSS_DIAG_DATA_INTEGRITY_MEM_ERROR_PROTECTION_TYPE_NONE_E",
    "CPSS_DIAG_DATA_INTEGRITY_MEM_ERROR_PROTECTION_TYPE_ECC_E",
    "CPSS_DIAG_DATA_INTEGRITY_MEM_ERROR_PROTECTION_TYPE_PARITY_E"
};
PRV_CPSS_LOG_STC_ENUM_ARRAY_SIZE_MAC(CPSS_DIAG_DATA_INTEGRITY_MEM_ERROR_PROTECTION_TYPE_ENT);
char * prvCpssLogEnum_CPSS_DIAG_PP_MEM_TYPE_ENT[]  =
{
    "CPSS_DIAG_PP_MEM_BUFFER_DRAM_E",
    "CPSS_DIAG_PP_MEM_WIDE_SRAM_E",
    "CPSS_DIAG_PP_MEM_NARROW_SRAM_E",
    "CPSS_DIAG_PP_MEM_INTERNAL_NARROW_SRAM_0_E",
    "CPSS_DIAG_PP_MEM_INTERNAL_NARROW_SRAM_1_E",
    "CPSS_DIAG_PP_MEM_EXTERNAL_NARROW_SRAM_0_E",
    "CPSS_DIAG_PP_MEM_EXTERNAL_NARROW_SRAM_1_E",
    "CPSS_DIAG_PP_MEM_FLOW_DRAM_E",
    "CPSS_DIAG_PP_MEM_MAC_TBL_MEM_E",
    "CPSS_DIAG_PP_MEM_VLAN_TBL_MEM_E"
};
PRV_CPSS_LOG_STC_ENUM_ARRAY_SIZE_MAC(CPSS_DIAG_PP_MEM_TYPE_ENT);
char * prvCpssLogEnum_CPSS_DIAG_PP_REG_TYPE_ENT[]  =
{
    "CPSS_DIAG_PP_REG_INTERNAL_E",
    "CPSS_DIAG_PP_REG_PCI_CFG_E"
};
PRV_CPSS_LOG_STC_ENUM_ARRAY_SIZE_MAC(CPSS_DIAG_PP_REG_TYPE_ENT);
char * prvCpssLogEnum_CPSS_DIAG_TEST_PROFILE_ENT[]  =
{
    "CPSS_DIAG_TEST_RANDOM_E",
    "CPSS_DIAG_TEST_INCREMENTAL_E",
    "CPSS_DIAG_TEST_BIT_TOGGLE_E",
    "CPSS_DIAG_TEST_AA_55_E"
};
PRV_CPSS_LOG_STC_ENUM_ARRAY_SIZE_MAC(CPSS_DIAG_TEST_PROFILE_ENT);
char * prvCpssLogEnum_CPSS_DIAG_DATA_INTEGRITY_TCAM_ARRAY_TYPE_ENT[]  =
{
    "CPSS_DIAG_DATA_INTEGRITY_TCAM_ARRAY_TYPE_X_E",
    "CPSS_DIAG_DATA_INTEGRITY_TCAM_ARRAY_TYPE_Y_E"
};
PRV_CPSS_LOG_STC_ENUM_ARRAY_SIZE_MAC(CPSS_DIAG_DATA_INTEGRITY_TCAM_ARRAY_TYPE_ENT);


/********* structure fields log functions *********/

void prvCpssLogParamFuncStc_CPSS_DIAG_DATA_INTEGRITY_MEMORY_LOCATION_STC_PTR
(
    IN CPSS_LOG_LIB_ENT         contextLib,
    IN CPSS_LOG_TYPE_ENT        logType,
    IN GT_CHAR_PTR              namePtr,
    IN void                   * fieldPtr,
    INOUT PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC     * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_START_FIELD_STC_MAC(CPSS_DIAG_DATA_INTEGRITY_MEMORY_LOCATION_STC *, valPtr);
    PRV_CPSS_LOG_UNUSED_MAC(inOutParamInfoPtr);
    PRV_CPSS_LOG_STC_NUMBER_MAC(valPtr, dfxPipeId);
    PRV_CPSS_LOG_STC_NUMBER_MAC(valPtr, dfxClientId);
    PRV_CPSS_LOG_STC_NUMBER_MAC(valPtr, dfxMemoryId);
    prvCpssLogStcLogEnd(contextLib, logType);
}
void prvCpssLogParamFuncStc_CPSS_DIAG_DATA_INTEGRITY_MPPM_MEMORY_LOCATION_STC_PTR
(
    IN CPSS_LOG_LIB_ENT         contextLib,
    IN CPSS_LOG_TYPE_ENT        logType,
    IN GT_CHAR_PTR              namePtr,
    IN void                   * fieldPtr,
    INOUT PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC     * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_START_FIELD_STC_MAC(CPSS_DIAG_DATA_INTEGRITY_MPPM_MEMORY_LOCATION_STC *, valPtr);
    PRV_CPSS_LOG_UNUSED_MAC(inOutParamInfoPtr);
    PRV_CPSS_LOG_STC_NUMBER_MAC(valPtr, portGroupId);
    PRV_CPSS_LOG_STC_NUMBER_MAC(valPtr, mppmId);
    PRV_CPSS_LOG_STC_NUMBER_MAC(valPtr, bankId);
    prvCpssLogStcLogEnd(contextLib, logType);
}
void prvCpssLogParamFuncStc_CPSS_DIAG_DATA_INTEGRITY_TCAM_MEMORY_LOCATION_STC_PTR
(
    IN CPSS_LOG_LIB_ENT         contextLib,
    IN CPSS_LOG_TYPE_ENT        logType,
    IN GT_CHAR_PTR              namePtr,
    IN void                   * fieldPtr,
    INOUT PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC     * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_START_FIELD_STC_MAC(CPSS_DIAG_DATA_INTEGRITY_TCAM_MEMORY_LOCATION_STC *, valPtr);
    PRV_CPSS_LOG_UNUSED_MAC(inOutParamInfoPtr);
    PRV_CPSS_LOG_STC_ENUM_MAC(valPtr, arrayType, CPSS_DIAG_DATA_INTEGRITY_TCAM_ARRAY_TYPE_ENT);
    PRV_CPSS_LOG_STC_NUMBER_MAC(valPtr, ruleIndex);
    prvCpssLogStcLogEnd(contextLib, logType);
}


/********* parameters log functions *********/

void prvCpssLogParamFunc_CPSS_DIAG_DATA_INTEGRITY_ERROR_CAUSE_TYPE_ENT(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_SET_PARAM_VAL_MAC(CPSS_DIAG_DATA_INTEGRITY_ERROR_CAUSE_TYPE_ENT, paramVal);
    PRV_CPSS_LOG_ENUM_MAC(namePtr, paramVal, CPSS_DIAG_DATA_INTEGRITY_ERROR_CAUSE_TYPE_ENT);
}
void prvCpssLogParamFunc_CPSS_DIAG_DATA_INTEGRITY_ERROR_CAUSE_TYPE_ENT_PTR(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_SET_PARAM_VAL_MAC(CPSS_DIAG_DATA_INTEGRITY_ERROR_CAUSE_TYPE_ENT*, paramVal);
    if (paramVal == NULL)
    {
        PRV_CPSS_LOG_AND_HISTORY_PARAM_MAC(contextLib, logType, "%s = NULL\n", namePtr);
        return;
    }
    PRV_CPSS_LOG_ENUM_MAC(namePtr, *paramVal, CPSS_DIAG_DATA_INTEGRITY_ERROR_CAUSE_TYPE_ENT);
}
void prvCpssLogParamFunc_CPSS_DIAG_DATA_INTEGRITY_ERROR_INJECT_MODE_ENT(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_SET_PARAM_VAL_MAC(CPSS_DIAG_DATA_INTEGRITY_ERROR_INJECT_MODE_ENT, paramVal);
    PRV_CPSS_LOG_ENUM_MAC(namePtr, paramVal, CPSS_DIAG_DATA_INTEGRITY_ERROR_INJECT_MODE_ENT);
}
void prvCpssLogParamFunc_CPSS_DIAG_DATA_INTEGRITY_ERROR_INJECT_MODE_ENT_PTR(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_SET_PARAM_VAL_MAC(CPSS_DIAG_DATA_INTEGRITY_ERROR_INJECT_MODE_ENT*, paramVal);
    if (paramVal == NULL)
    {
        PRV_CPSS_LOG_AND_HISTORY_PARAM_MAC(contextLib, logType, "%s = NULL\n", namePtr);
        return;
    }
    PRV_CPSS_LOG_ENUM_MAC(namePtr, *paramVal, CPSS_DIAG_DATA_INTEGRITY_ERROR_INJECT_MODE_ENT);
}
void prvCpssLogParamFunc_CPSS_DIAG_DATA_INTEGRITY_MEMORY_LOCATION_UNT_PTR(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_START_PARAM_STC_MAC(CPSS_DIAG_DATA_INTEGRITY_MEMORY_LOCATION_UNT*, paramVal);
    prvCpssLogParamFuncStc_CPSS_DIAG_DATA_INTEGRITY_MEMORY_LOCATION_UNT_PTR(contextLib, logType, namePtr, paramVal, inOutParamInfoPtr);
}
void prvCpssLogParamFunc_CPSS_DIAG_DATA_INTEGRITY_MEM_ERROR_PROTECTION_TYPE_ENT_PTR(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_SET_PARAM_VAL_MAC(CPSS_DIAG_DATA_INTEGRITY_MEM_ERROR_PROTECTION_TYPE_ENT*, paramVal);
    if (paramVal == NULL)
    {
        PRV_CPSS_LOG_AND_HISTORY_PARAM_MAC(contextLib, logType, "%s = NULL\n", namePtr);
        return;
    }
    PRV_CPSS_LOG_ENUM_MAC(namePtr, *paramVal, CPSS_DIAG_DATA_INTEGRITY_MEM_ERROR_PROTECTION_TYPE_ENT);
}
void prvCpssLogParamFunc_CPSS_DIAG_PP_MEM_TYPE_ENT(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_SET_PARAM_VAL_MAC(CPSS_DIAG_PP_MEM_TYPE_ENT, paramVal);
    PRV_CPSS_LOG_ENUM_MAC(namePtr, paramVal, CPSS_DIAG_PP_MEM_TYPE_ENT);
}
void prvCpssLogParamFunc_CPSS_DIAG_PP_REG_TYPE_ENT(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_SET_PARAM_VAL_MAC(CPSS_DIAG_PP_REG_TYPE_ENT, paramVal);
    PRV_CPSS_LOG_ENUM_MAC(namePtr, paramVal, CPSS_DIAG_PP_REG_TYPE_ENT);
}
void prvCpssLogParamFunc_CPSS_DIAG_TEST_PROFILE_ENT(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_SET_PARAM_VAL_MAC(CPSS_DIAG_TEST_PROFILE_ENT, paramVal);
    PRV_CPSS_LOG_ENUM_MAC(namePtr, paramVal, CPSS_DIAG_TEST_PROFILE_ENT);
}

