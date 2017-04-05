/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* prvCpssGenConfigLog.c
*       WARNING!!! this is a generated file, please don't edit it manually
* COMMENTS:
*
* FILE REVISION NUMBER:
*       $Revision: 1 $
*******************************************************************************/
#include <cpss/generic/log/cpssLog.h>
#include <cpss/generic/log/prvCpssLog.h>
#include <cpss/extServices/private/prvCpssBindFunc.h>
#include <cpss/generic/config/cpssGenCfg.h>
#include <cpss/generic/config/private/prvCpssGenConfigLog.h>
#include <cpss/generic/log/prvCpssGenCommonTypesLog.h>
#include <cpss/generic/log/prvCpssGenPpTypesLog.h>
#include <cpss/generic/networkIf/private/prvCpssGenNetworkIfLog.h>


/********* enums *********/

char * prvCpssLogEnum_CPSS_GEN_CFG_HW_DEV_NUM_MODE_ENT[]  =
{
    "CPSS_GEN_CFG_HW_DEV_NUM_MODE_SINGLE_E",
    "CPSS_GEN_CFG_HW_DEV_NUM_MODE_DUAL_E"
};
PRV_CPSS_LOG_STC_ENUM_ARRAY_SIZE_MAC(CPSS_GEN_CFG_HW_DEV_NUM_MODE_ENT);


/********* structure fields log functions *********/

void prvCpssLogParamFuncStc_CPSS_GEN_CFG_DEV_INFO_STC_PTR
(
    IN CPSS_LOG_LIB_ENT         contextLib,
    IN CPSS_LOG_TYPE_ENT        logType,
    IN GT_CHAR_PTR              namePtr,
    IN void                   * fieldPtr,
    INOUT PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC     * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_START_FIELD_STC_MAC(CPSS_GEN_CFG_DEV_INFO_STC *, valPtr);
    PRV_CPSS_LOG_UNUSED_MAC(inOutParamInfoPtr);
    PRV_CPSS_LOG_STC_NUMBER_MAC(valPtr, devType);
    PRV_CPSS_LOG_STC_NUMBER_MAC(valPtr, revision);
    PRV_CPSS_LOG_STC_ENUM_MAP_MAC(valPtr, devFamily, CPSS_PP_FAMILY_TYPE_ENT);
    PRV_CPSS_LOG_STC_ENUM_MAP_MAC(valPtr, devSubFamily, CPSS_PP_SUB_FAMILY_TYPE_ENT);
    PRV_CPSS_LOG_STC_NUMBER_MAC(valPtr, maxPortNum);
    PRV_CPSS_LOG_STC_NUMBER_MAC(valPtr, numOfVirtPorts);
    PRV_CPSS_LOG_STC_STC_MAC(valPtr, existingPorts, CPSS_PORTS_BMP_STC);
    PRV_CPSS_LOG_STC_ENUM_MAC(valPtr, hwDevNumMode, CPSS_GEN_CFG_HW_DEV_NUM_MODE_ENT);
    PRV_CPSS_LOG_STC_ENUM_MAC(valPtr, cpuPortMode, CPSS_NET_CPU_PORT_MODE_ENT);
    PRV_CPSS_LOG_STC_NUMBER_MAC(valPtr, numOfLedInfPerPortGroup);
    prvCpssLogStcLogEnd(contextLib, logType);
}


/********* parameters log functions *********/

void prvCpssLogParamFunc_CPSS_GEN_CFG_HW_DEV_NUM_MODE_ENT(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_SET_PARAM_VAL_MAC(CPSS_GEN_CFG_HW_DEV_NUM_MODE_ENT, paramVal);
    PRV_CPSS_LOG_ENUM_MAC(namePtr, paramVal, CPSS_GEN_CFG_HW_DEV_NUM_MODE_ENT);
}
void prvCpssLogParamFunc_CPSS_GEN_CFG_HW_DEV_NUM_MODE_ENT_PTR(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_SET_PARAM_VAL_MAC(CPSS_GEN_CFG_HW_DEV_NUM_MODE_ENT*, paramVal);
    if (paramVal == NULL)
    {
        PRV_CPSS_LOG_AND_HISTORY_PARAM_MAC(contextLib, logType, "%s = NULL\n", namePtr);
        return;
    }
    PRV_CPSS_LOG_ENUM_MAC(namePtr, *paramVal, CPSS_GEN_CFG_HW_DEV_NUM_MODE_ENT);
}

