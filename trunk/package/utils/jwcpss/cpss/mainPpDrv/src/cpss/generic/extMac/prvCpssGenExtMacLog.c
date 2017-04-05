/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* prvCpssGenExtMacLog.c
*       WARNING!!! this is a generated file, please don't edit it manually
* COMMENTS:
*
* FILE REVISION NUMBER:
*       $Revision: 1 $
*******************************************************************************/
#include <cpss/generic/log/cpssLog.h>
#include <cpss/generic/log/prvCpssLog.h>
#include <cpss/extServices/private/prvCpssBindFunc.h>
#include <cpss/generic/extMac/cpssExtMacDrv.h>
#include <cpss/generic/extMac/private/prvCpssGenExtMacLog.h>


/********* structure fields log functions *********/

void prvCpssLogParamFuncStc_CPSS_MACDRV_OBJ_STC_PTR
(
    IN CPSS_LOG_LIB_ENT         contextLib,
    IN CPSS_LOG_TYPE_ENT        logType,
    IN GT_CHAR_PTR              namePtr,
    IN void                   * fieldPtr,
    INOUT PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC     * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_START_FIELD_STC_MAC(CPSS_MACDRV_OBJ_STC *, valPtr);
    PRV_CPSS_LOG_UNUSED_MAC(inOutParamInfoPtr);
    PRV_CPSS_LOG_STC_PTR_MAC(valPtr, macDrvMacSpeedSetFunc);
    PRV_CPSS_LOG_STC_PTR_MAC(valPtr, macDrvMacSpeedGetFunc);
    PRV_CPSS_LOG_STC_PTR_MAC(valPtr, macDrvMacDuplexANSetFunc);
    PRV_CPSS_LOG_STC_PTR_MAC(valPtr, macDrvMacDuplexANGetFunc);
    PRV_CPSS_LOG_STC_PTR_MAC(valPtr, macDrvMacFlowCntlANSetFunc);
    PRV_CPSS_LOG_STC_PTR_MAC(valPtr, macDrvMacFlowCntlANGetFunc);
    PRV_CPSS_LOG_STC_PTR_MAC(valPtr, macDrvMacSpeedANSetFunc);
    PRV_CPSS_LOG_STC_PTR_MAC(valPtr, macDrvMacSpeedANGetFunc);
    PRV_CPSS_LOG_STC_PTR_MAC(valPtr, macDrvMacFlowCntlSetFunc);
    PRV_CPSS_LOG_STC_PTR_MAC(valPtr, macDrvMacFlowCntlGetFunc);
    PRV_CPSS_LOG_STC_PTR_MAC(valPtr, macDrvMacPeriodFlowCntlSetFunc);
    PRV_CPSS_LOG_STC_PTR_MAC(valPtr, macDrvMacPeriodFlowCntlGetFunc);
    PRV_CPSS_LOG_STC_PTR_MAC(valPtr, macDrvMacBackPrSetFunc);
    PRV_CPSS_LOG_STC_PTR_MAC(valPtr, macDrvMacBackPrGetFunc);
    PRV_CPSS_LOG_STC_PTR_MAC(valPtr, macDrvMacPortlinkGetFunc);
    PRV_CPSS_LOG_STC_PTR_MAC(valPtr, macDrvMacDuplexSetFunc);
    PRV_CPSS_LOG_STC_PTR_MAC(valPtr, macDrvMacDuplexGetFunc);
    PRV_CPSS_LOG_STC_PTR_MAC(valPtr, macDrvMacPortEnableSetFunc);
    PRV_CPSS_LOG_STC_PTR_MAC(valPtr, macDrvMacPortEnableGetFunc);
    PRV_CPSS_LOG_STC_PTR_MAC(valPtr, macDrvMacExcessiveCollisionDropSetFunc);
    PRV_CPSS_LOG_STC_PTR_MAC(valPtr, macDrvMacExcessiveCollisionDropGetFunc);
    PRV_CPSS_LOG_STC_PTR_MAC(valPtr, macDrvMacPaddingEnableSetFunc);
    PRV_CPSS_LOG_STC_PTR_MAC(valPtr, macDrvMacPaddingEnableGetFunc);
    PRV_CPSS_LOG_STC_PTR_MAC(valPtr, macDrvMacPreambleLengthSetFunc);
    PRV_CPSS_LOG_STC_PTR_MAC(valPtr, macDrvMacPreambleLengthGetFunc);
    PRV_CPSS_LOG_STC_PTR_MAC(valPtr, macDrvMacCRCCheckSetFunc);
    PRV_CPSS_LOG_STC_PTR_MAC(valPtr, macDrvMacCRCCheckGetFunc);
    PRV_CPSS_LOG_STC_PTR_MAC(valPtr, macDrvMacMRUSetFunc);
    PRV_CPSS_LOG_STC_PTR_MAC(valPtr, macDrvMacMRUGetFunc);
    prvCpssLogStcLogEnd(contextLib, logType);
}


/********* parameters log functions *********/

void prvCpssLogParamFunc_CPSS_MACDRV_OBJ_STC_PTR(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_START_PARAM_STC_MAC(CPSS_MACDRV_OBJ_STC*, paramVal);
    prvCpssLogParamFuncStc_CPSS_MACDRV_OBJ_STC_PTR(contextLib, logType, namePtr, paramVal, inOutParamInfoPtr);
}

