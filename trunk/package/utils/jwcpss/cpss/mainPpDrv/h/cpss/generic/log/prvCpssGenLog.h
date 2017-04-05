/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* prvCpssGenLog.h
*
* DESCRIPTION:
*       CPSS private generic log definition.
*
* FILE REVISION NUMBER:
*       $Revision: 1 $
*******************************************************************************/
#ifndef __prvCpssGenLogh
#define __prvCpssGenLogh

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include <cpss/generic/cpssTypes.h>
#include <cpss/generic/log/prvCpssLog.h>
#include <cpss/generic/log/prvCpssGenCommonTypesLog.h>
#include <cpss/generic/log/prvCpssGenDbLog.h>
#include <cpss/generic/log/prvCpssGenPpTypesLog.h>

/* Log port bitmap parameter */
#define PRV_CPSS_LOG_STC_PORT_BMP_MAC(valPtr, field) \
    PRV_CPSS_LOG_FUNC_STC_TYPE_ARRAY_MAC(valPtr, field.ports, CPSS_MAX_PORTS_BMP_NUM_CNS, GT_U32)

/* Log mac address field */
#define PRV_CPSS_LOG_STC_ETH_MAC(valPtr, field) \
    prvCpssLogParamFuncStc_GT_ETHERADDR_PTR(contextLib, logType, #field, (void *)&valPtr->field, NULL);

/* Log IPV4 address field */
#define PRV_CPSS_LOG_STC_IPV4_MAC(valPtr, field) \
    prvCpssLogParamFuncStc_GT_IPADDR_PTR(contextLib, logType, #field, (void *)&valPtr->field, NULL);

/* Log IPV6 address parameter */
#define PRV_CPSS_LOG_STC_IPV6_MAC(valPtr, field) \
    prvCpssLogParamFuncStc_GT_IPV6ADDR_PTR(contextLib, logType, #field, (void *)&valPtr->field, NULL);

void prvCpssLogParamFuncStc_GT_PHYSICAL_PORT_NUM_PTR(
    IN CPSS_LOG_LIB_ENT         contextLib,
    IN CPSS_LOG_TYPE_ENT        logType,
    IN GT_CHAR_PTR              namePtr,
    IN void                   * fieldPtr,
    INOUT PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC     * inOutParamInfoPtr
);
void prvCpssLogParamFuncStc_GT_TRUNK_ID_PTR(
    IN CPSS_LOG_LIB_ENT         contextLib,
    IN CPSS_LOG_TYPE_ENT        logType,
    IN GT_CHAR_PTR              namePtr,
    IN void                   * fieldPtr,
    INOUT PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC     * inOutParamInfoPtr
);
void prvCpssLogParamFuncStc_GT_PORT_NUM_PTR(
    IN CPSS_LOG_LIB_ENT         contextLib,
    IN CPSS_LOG_TYPE_ENT        logType,
    IN GT_CHAR_PTR              namePtr,
    IN void                   * fieldPtr,
    INOUT PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC     * inOutParamInfoPtr
);
void prvCpssLogParamFunc_GT_PORT_NUM_ARRAY(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __prvCpssGenLogh */
