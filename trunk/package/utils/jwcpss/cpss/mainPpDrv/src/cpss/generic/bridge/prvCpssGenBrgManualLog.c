/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* prvCpssGenBrgManualLog.c
*       Manually implemented CPSS Log type wrappers
* COMMENTS:
*
* FILE REVISION NUMBER:
*       $Revision: 1 $
*******************************************************************************/
#include <cpss/generic/log/cpssLog.h>
#include <cpss/generic/log/prvCpssLog.h>
#include <cpss/extServices/private/prvCpssBindFunc.h>
#include <cpss/generic/bridge/cpssGenBrgFdb.h>
#include <cpss/generic/bridge/cpssGenBrgGen.h>
#include <cpss/generic/bridge/cpssGenBrgVlanTypes.h>
#include <cpss/generic/bridge/private/prvCpssGenBrgLog.h>
#include <cpss/generic/log/prvCpssGenCommonTypesLog.h>
#include <cpss/generic/log/prvCpssGenPpTypesLog.h>
#include <cpss/generic/log/prvCpssGenLog.h>

/* max length of parameter name (include array element) */
#define PRV_LOG_STRING_NAME_BUFFER_SIZE_CNS 160

/********* enums *********/
/********* structure fields log functions *********/
void prvCpssLogParamFuncStc_CPSS_MAC_ENTRY_EXT_KEY_STC_PTR(
    IN    CPSS_LOG_LIB_ENT                        contextLib,
    IN    CPSS_LOG_TYPE_ENT                       logType,
    IN    GT_CHAR_PTR                             namePtr,
    IN    void                                  * fieldPtr,
    INOUT PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC     * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_START_FIELD_STC_MAC(CPSS_MAC_ENTRY_EXT_KEY_STC *, valPtr);

    PRV_CPSS_LOG_UNUSED_MAC(inOutParamInfoPtr);

    PRV_CPSS_LOG_STC_ENUM_MAC(valPtr, entryType, CPSS_MAC_ENTRY_EXT_TYPE_ENT);
    PRV_CPSS_LOG_STC_NUMBER_MAC(valPtr, vid1);
    switch (PRV_CPSS_LOG_STC_FIELD_VAL_MAC(valPtr, entryType))
    {
        case CPSS_MAC_ENTRY_EXT_TYPE_MAC_ADDR_E:
        case CPSS_MAC_ENTRY_EXT_TYPE_MAC_ADDR_FID_VID1_E:
            PRV_CPSS_LOG_STC_ETH_MAC(valPtr, key.macVlan.macAddr);
            PRV_CPSS_LOG_STC_NUMBER_MAC(valPtr, key.macVlan.vlanId);
            break;

        case CPSS_MAC_ENTRY_EXT_TYPE_IPV4_MCAST_E:
        case CPSS_MAC_ENTRY_EXT_TYPE_IPV4_MCAST_FID_VID1_E:
        case CPSS_MAC_ENTRY_EXT_TYPE_IPV6_MCAST_E:
        case CPSS_MAC_ENTRY_EXT_TYPE_IPV6_MCAST_FID_VID1_E:
            PRV_CPSS_LOG_STC_IPV4_MAC(valPtr, key.ipMcast.sip);
            PRV_CPSS_LOG_STC_IPV4_MAC(valPtr, key.ipMcast.dip);
            PRV_CPSS_LOG_STC_NUMBER_MAC(valPtr, key.ipMcast.vlanId);
            break;

        case CPSS_MAC_ENTRY_EXT_TYPE_IPV4_UC_E:
            PRV_CPSS_LOG_STC_IPV4_MAC(valPtr, key.ipv4Unicast.dip);
            PRV_CPSS_LOG_STC_NUMBER_MAC(valPtr, key.ipv4Unicast.vrfId);
            break;

        case CPSS_MAC_ENTRY_EXT_TYPE_IPV6_UC_ADDR_ENTRY_E:
            PRV_CPSS_LOG_STC_IPV6_MAC(valPtr, key.ipv6Unicast.dip);
            PRV_CPSS_LOG_STC_NUMBER_MAC(valPtr, key.ipv6Unicast.vrfId);
            break;

        case CPSS_MAC_ENTRY_EXT_TYPE_IPV6_UC_DATA_ENTRY_E:
            /* do nothing no key */
            break;
    }

    prvCpssLogStcLogEnd(contextLib, logType);
}

/********* parameters log functions *********/

void prvCpssLogParamFunc_CPSS_MAC_UPDATE_MSG_EXT_STC_PTR(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_START_PARAM_STC_MAC(CPSS_MAC_UPDATE_MSG_EXT_STC *, stcPtr);
    prvCpssLogArrayOfParamsHandle(contextLib,logType,namePtr,(void*) stcPtr, sizeof(CPSS_MAC_UPDATE_MSG_EXT_STC), inOutParamInfoPtr,
                                  prvCpssLogParamFuncStc_CPSS_MAC_UPDATE_MSG_EXT_STC_PTR);
}

/********* api pre-log functions *********/
