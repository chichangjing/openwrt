/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* prvCpssDxChIpManualLog.c
*       Manually implemented CPSS Log type wrappers
* COMMENTS:
*
* FILE REVISION NUMBER:
*       $Revision: 1 $
*******************************************************************************/
#include <cpss/generic/log/cpssLog.h>
#include <cpss/generic/log/prvCpssLog.h>
#include <cpss/extServices/private/prvCpssBindFunc.h>
#include <cpss/dxCh/dxChxGen/ip/private/prvCpssDxChIpLog.h>
#include <cpss/dxCh/dxChxGen/ip/cpssDxChIpNat.h>
#include <cpss/generic/log/prvCpssGenLog.h>

/********* enums *********/
/********* structure fields log functions *********/
void prvCpssLogParamFuncStc_CPSS_DXCH_IP_NAT_ENTRY_UNT_PTR
(
    IN CPSS_LOG_LIB_ENT         contextLib,
    IN CPSS_LOG_TYPE_ENT        logType,
    IN GT_CHAR_PTR              namePtr,
    IN void                   * fieldPtr,
    INOUT PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC     * inOutParamInfoPtr
)
{
    CPSS_IP_NAT_TYPE_ENT natType;
    CPSS_IP_NAT_TYPE_ENT *natTypePtr;

    PRV_CPSS_LOG_START_FIELD_STC_MAC(CPSS_DXCH_IP_NAT_ENTRY_UNT *, valPtr);
    if (inOutParamInfoPtr->paramKey.paramKeyArr[3] == 1) 
    {
        natTypePtr = (CPSS_IP_NAT_TYPE_ENT*)inOutParamInfoPtr->paramKey.paramKeyArr[4];
        natType = *natTypePtr; 
    }
    else
    {
        natType = (CPSS_IP_NAT_TYPE_ENT)inOutParamInfoPtr->paramKey.paramKeyArr[4]; 
    }
    if (natType == CPSS_IP_NAT_TYPE_NAT44_E) 
    {
        PRV_CPSS_LOG_STC_STC_MAC(valPtr, nat44Entry, CPSS_DXCH_IP_NAT44_ENTRY_STC); 
    }
    else
    {
        PRV_CPSS_LOG_STC_STC_MAC(valPtr, nat66Entry, CPSS_DXCH_IP_NAT66_ENTRY_STC);
    }
    prvCpssLogStcLogEnd(contextLib, logType);
}
void prvCpssLogParamFuncStc_CPSS_DXCH_IP_UC_ROUTE_ENTRY_UNT_PTR
(
    IN CPSS_LOG_LIB_ENT         contextLib,
    IN CPSS_LOG_TYPE_ENT        logType,
    IN GT_CHAR_PTR              namePtr,
    IN void                   * fieldPtr,
    INOUT PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC     * inOutParamInfoPtr
)
{
    CPSS_DXCH_IP_UC_ROUTE_ENTRY_TYPE_ENT type;

    PRV_CPSS_LOG_START_FIELD_STC_MAC(CPSS_DXCH_IP_UC_ROUTE_ENTRY_UNT *, valPtr);
    type = (CPSS_DXCH_IP_UC_ROUTE_ENTRY_TYPE_ENT)inOutParamInfoPtr->paramKey.paramKeyArr[4];
    if (type == CPSS_DXCH_IP_UC_ROUTE_ENTRY_E) 
    {
        PRV_CPSS_LOG_STC_STC_MAC(valPtr, regularEntry, CPSS_DXCH_IP_UC_ROUTE_ENTRY_FORMAT_STC); 
    }
    else
    {
        PRV_CPSS_LOG_STC_STC_MAC(valPtr, ecmpRpfCheck, CPSS_DXCH_IP_UC_ECMP_RPF_FORMAT_STC);
    }
    prvCpssLogStcLogEnd(contextLib, logType);
}
void prvCpssLogParamFuncStc_CPSS_DXCH_IP_UC_ROUTE_ENTRY_STC_PTR
(
    IN CPSS_LOG_LIB_ENT         contextLib,
    IN CPSS_LOG_TYPE_ENT        logType,
    IN GT_CHAR_PTR              namePtr,
    IN void                   * fieldPtr,
    INOUT PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC     * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_START_FIELD_STC_MAC(CPSS_DXCH_IP_UC_ROUTE_ENTRY_STC *, valPtr);
    
    PRV_CPSS_LOG_STC_ENUM_MAC(valPtr, type, CPSS_DXCH_IP_UC_ROUTE_ENTRY_TYPE_ENT);
    inOutParamInfoPtr->paramKey.paramKeyArr[4] = (CPSS_DXCH_IP_UC_ROUTE_ENTRY_TYPE_ENT)valPtr->type;
    PRV_CPSS_LOG_STC_STC_MAC(valPtr, entry, CPSS_DXCH_IP_UC_ROUTE_ENTRY_UNT);
    prvCpssLogStcLogEnd(contextLib, logType);
}
void prvCpssLogParamFuncStc_CPSS_DXCH_IP_COUNTER_SET_INTERFACE_CFG_STC_PTR
(
    IN CPSS_LOG_LIB_ENT         contextLib,
    IN CPSS_LOG_TYPE_ENT        logType,
    IN GT_CHAR_PTR              namePtr,
    IN void                   * fieldPtr,
    INOUT PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC     * inOutParamInfoPtr
)
{
    CPSS_DXCH_IP_PORT_TRUNK_CNT_MODE_ENT portTrunkCntMode;

    PRV_CPSS_LOG_START_FIELD_STC_MAC(CPSS_DXCH_IP_COUNTER_SET_INTERFACE_CFG_STC *, valPtr);
    PRV_CPSS_LOG_UNUSED_MAC(inOutParamInfoPtr);
    PRV_CPSS_LOG_STC_ENUM_MAC(valPtr, portTrunkCntMode, CPSS_DXCH_IP_PORT_TRUNK_CNT_MODE_ENT);
    portTrunkCntMode = (CPSS_DXCH_IP_PORT_TRUNK_CNT_MODE_ENT)valPtr->portTrunkCntMode;
    PRV_CPSS_LOG_STC_ENUM_MAC(valPtr, ipMode, CPSS_IP_PROTOCOL_STACK_ENT);
    PRV_CPSS_LOG_STC_ENUM_MAC(valPtr, vlanMode, CPSS_DXCH_IP_VLAN_CNT_MODE_ENT);
    PRV_CPSS_LOG_STC_NUMBER_MAC(valPtr, hwDevNum);
    prvCpssLogStcLogStart(contextLib,  logType, "portTrunk");
    if (portTrunkCntMode == CPSS_DXCH_IP_PORT_CNT_MODE_E) 
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
void prvCpssLogParamFunc_CPSS_DXCH_IP_UC_ROUTE_ENTRY_STC_PTR(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_START_PARAM_STC_MAC(CPSS_DXCH_IP_UC_ROUTE_ENTRY_STC*, paramVal);
    prvCpssLogArrayOfParamsHandle(contextLib,logType,namePtr,(void*) paramVal, sizeof(CPSS_DXCH_IP_UC_ROUTE_ENTRY_STC), inOutParamInfoPtr,
                                  prvCpssLogParamFuncStc_CPSS_DXCH_IP_UC_ROUTE_ENTRY_STC_PTR);
}
/********* parameters log functions *********/
/********* api pre-log functions *********/
void cpssDxChIpUcRouteEntriesWrite_preLogic
(
    IN va_list  args,
    OUT PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC * paramDataPtr
)
{
    /* devNum */
    va_arg(args, GT_U32);
    /* baseRouteEntryIndex */
    va_arg(args, GT_U32);
    /* routeEntriesArray */
    va_arg(args, CPSS_DXCH_IP_UC_ROUTE_ENTRY_STC*);
    /* numOfRouteEntries */
    paramDataPtr->paramKey.paramKeyArr[2] = (GT_U32)va_arg(args, GT_U32);
}
void cpssDxChIpNatEntrySet_preLogic
(
    IN va_list  args,
    OUT PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC * paramDataPtr
)
{
    /* devNum */
    va_arg(args, GT_U32);
    /* natIndex */
    va_arg(args, GT_U32);
    /* natType */
    paramDataPtr->paramKey.paramKeyArr[4] = (CPSS_IP_NAT_TYPE_ENT)va_arg(args, CPSS_IP_NAT_TYPE_ENT);
}
void cpssDxChIpNatEntryGet_preLogic
(
    IN va_list  args,
    OUT PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC * paramDataPtr
)
{
    /* devNum */
    va_arg(args, GT_U32);
    /* natIndex */
    va_arg(args, GT_U32);
    /* natTypePtr */
    paramDataPtr->paramKey.paramKeyArr[4] = (GT_UINTPTR)va_arg(args, CPSS_IP_NAT_TYPE_ENT*);
    paramDataPtr->paramKey.paramKeyArr[3] = 1;
}
