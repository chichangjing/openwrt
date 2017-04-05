/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* prvCpssDxChL2mllManualLog.c
*       Manually implemented CPSS Log type wrappers
* COMMENTS:
*
* FILE REVISION NUMBER:
*       $Revision: 1 $
*******************************************************************************/
#include <cpss/generic/log/cpssLog.h>
#include <cpss/generic/log/prvCpssLog.h>
#include <cpss/extServices/private/prvCpssBindFunc.h>
#include <cpss/generic/log/prvCpssGenLog.h>
#include <cpss/dxCh/dxChxGen/l2mll/cpssDxChL2Mll.h>
#include <cpss/dxCh/dxChxGen/l2mll/private/prvCpssDxChL2mllLog.h>

/********* enums *********/
/********* structure fields log functions *********/
void prvCpssLogParamFuncStc_CPSS_DXCH_L2MLL_COUNTER_SET_INTERFACE_CFG_STC_PTR
(
    IN CPSS_LOG_LIB_ENT         contextLib,
    IN CPSS_LOG_TYPE_ENT        logType,
    IN GT_CHAR_PTR              namePtr,
    IN void                   * fieldPtr,
    INOUT PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC     * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_START_FIELD_STC_MAC(CPSS_DXCH_L2MLL_COUNTER_SET_INTERFACE_CFG_STC *, valPtr);
    PRV_CPSS_LOG_UNUSED_MAC(inOutParamInfoPtr);
    PRV_CPSS_LOG_STC_ENUM_MAC(valPtr, portTrunkCntMode, CPSS_DXCH_L2MLL_PORT_TRUNK_CNT_MODE_ENT);
    PRV_CPSS_LOG_STC_ENUM_MAC(valPtr, ipMode, CPSS_IP_PROTOCOL_STACK_ENT);
    PRV_CPSS_LOG_STC_ENUM_MAC(valPtr, vlanMode, CPSS_DXCH_L2MLL_VLAN_CNT_MODE_ENT);
    PRV_CPSS_LOG_STC_NUMBER_MAC(valPtr, hwDevNum);
    prvCpssLogStcLogStart(contextLib,  logType, "portTrunk");
    if (valPtr->portTrunkCntMode == CPSS_DXCH_L2MLL_PORT_CNT_MODE_E) 
    {
        PRV_CPSS_LOG_STC_NUMBER_MAC((&valPtr->portTrunk), port); 
    }
    else
    {
        PRV_CPSS_LOG_STC_NUMBER_MAC((&valPtr->portTrunk), trunk);
    }
    prvCpssLogStcLogEnd(contextLib, logType);
    PRV_CPSS_LOG_STC_NUMBER_MAC(valPtr, vlanId);
    prvCpssLogStcLogEnd(contextLib, logType);
}
/********* parameters log functions *********/
/********* api pre-log functions *********/

