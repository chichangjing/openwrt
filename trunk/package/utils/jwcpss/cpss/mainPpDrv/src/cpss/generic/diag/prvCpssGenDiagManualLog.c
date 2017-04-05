/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* prvCpssGenDiagManualLog.c
*       Manually implemented CPSS Log type wrappers
* COMMENTS:
*
* FILE REVISION NUMBER:
*       $Revision: 1 $
*******************************************************************************/
#include <cpss/generic/log/cpssLog.h>
#include <cpss/generic/log/prvCpssLog.h>
#include <cpss/extServices/private/prvCpssBindFunc.h>
#include <cpss/generic/diag/cpssDiag.h>
#include <cpss/generic/bridge/cpssGenBrgFdb.h>
#include <cpss/generic/log/prvCpssGenCommonTypesLog.h>
#include <cpss/generic/log/prvCpssGenPpTypesLog.h>
#include <cpss/generic/log/prvCpssGenLog.h>
#include <cpss/dxCh/dxChxGen/diag/cpssDxChDiagDataIntegrity.h>
#include <cpss/generic/diag/private/prvCpssGenDiagLog.h>

/* max length of parameter name (include array element) */ 
#define PRV_LOG_STRING_NAME_BUFFER_SIZE_CNS 160

/********* enums *********/
/********* structure fields log functions *********/
void prvCpssLogParamFuncStc_CPSS_DIAG_DATA_INTEGRITY_MEMORY_LOCATION_UNT_PTR
(
    IN CPSS_LOG_LIB_ENT         contextLib,
    IN CPSS_LOG_TYPE_ENT        logType,
    IN GT_CHAR_PTR              namePtr,
    IN void                   * fieldPtr,
    INOUT PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC     * inOutParamInfoPtr
)
{
    CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_ENT memType = (CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_ENT)inOutParamInfoPtr->paramKey.paramKeyArr[4];
    PRV_CPSS_LOG_START_FIELD_STC_MAC(CPSS_DIAG_DATA_INTEGRITY_MEMORY_LOCATION_UNT *, valPtr);
    switch (memType)
    {
        case CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_MPPM_DATA_BUFFER_MEMORY_E:
        case CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_MPPM_DATA_BUFFER_MEMORY_ECC_E:
            PRV_CPSS_LOG_STC_STC_MAC(valPtr, mppmMemLocation, CPSS_DIAG_DATA_INTEGRITY_MPPM_MEMORY_LOCATION_STC);
            break;
        case CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_POLICY_TCAM_E :
        case CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_ROUTER_TCAM_E:
            PRV_CPSS_LOG_STC_STC_MAC(valPtr, tcamMemLocation, CPSS_DIAG_DATA_INTEGRITY_TCAM_MEMORY_LOCATION_STC);
            break;
        default:
            PRV_CPSS_LOG_STC_STC_MAC(valPtr, memLocation, CPSS_DIAG_DATA_INTEGRITY_MEMORY_LOCATION_STC);
            break;
    }
    prvCpssLogStcLogEnd(contextLib, logType);
}

/********* parameters log functions *********/

/********* api pre-log functions *********/


