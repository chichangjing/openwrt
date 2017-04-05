/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* prvCpssDxChTunnelManualLog.c
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
#include <cpss/generic/tunnel/cpssGenTunnelTypes.h>
#include <cpss/dxCh/dxChxGen/tunnel/private/prvCpssDxChTunnelLog.h>
#include <cpss/dxCh/dxChxGen/tunnel/cpssDxChTunnelTypes.h>

/********* enums *********/
/********* structure fields log functions *********/
void prvCpssLogParamFuncStc_CPSS_DXCH_TUNNEL_START_CONFIG_UNT_PTR
(
    IN CPSS_LOG_LIB_ENT         contextLib,
    IN CPSS_LOG_TYPE_ENT        logType,
    IN GT_CHAR_PTR              namePtr,
    IN void                   * fieldPtr,
    INOUT PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC     * inOutParamInfoPtr
)
{
    CPSS_TUNNEL_TYPE_ENT tunnelType;
    CPSS_TUNNEL_TYPE_ENT *tunnelTypePtr;

    PRV_CPSS_LOG_START_FIELD_STC_MAC(CPSS_DXCH_TUNNEL_START_CONFIG_UNT *, valPtr);
    tunnelTypePtr = (CPSS_TUNNEL_TYPE_ENT*)inOutParamInfoPtr->paramKey.paramKeyArr[5];
    if (tunnelTypePtr != NULL) 
    {
        tunnelType = *tunnelTypePtr; 
    }
    else
    {
        tunnelType = (CPSS_TUNNEL_TYPE_ENT)inOutParamInfoPtr->paramKey.paramKeyArr[4]; 
    }
    switch (tunnelType) 
    {
        case CPSS_TUNNEL_X_OVER_IPV4_E:
        case CPSS_TUNNEL_X_OVER_GRE_IPV4_E:
        case CPSS_TUNNEL_GENERIC_IPV4_E:
            PRV_CPSS_LOG_STC_STC_MAC(valPtr, ipv4Cfg, CPSS_DXCH_TUNNEL_START_IPV4_CONFIG_STC); 
            break;
        case CPSS_TUNNEL_GENERIC_IPV6_E:
            PRV_CPSS_LOG_STC_STC_MAC(valPtr, ipv6Cfg, CPSS_DXCH_TUNNEL_START_IPV6_CONFIG_STC);
            break;
        case CPSS_TUNNEL_X_OVER_MPLS_E:
            PRV_CPSS_LOG_STC_STC_MAC(valPtr, mplsCfg, CPSS_DXCH_TUNNEL_START_MPLS_CONFIG_STC);
            break;
        case CPSS_TUNNEL_MAC_IN_MAC_E:
            PRV_CPSS_LOG_STC_STC_MAC(valPtr, mimCfg, CPSS_DXCH_TUNNEL_START_MIM_CONFIG_STC);
            break;
        case CPSS_TUNNEL_TRILL_E:
            PRV_CPSS_LOG_STC_STC_MAC(valPtr, trillCfg, CPSS_DXCH_TUNNEL_START_TRILL_CONFIG_STC);
            break;
        default:
            break;
    }
    prvCpssLogStcLogEnd(contextLib, logType);
}
void prvCpssLogParamFuncStc_CPSS_DXCH_TUNNEL_TERM_CONFIG_UNT_PTR
(
    IN CPSS_LOG_LIB_ENT         contextLib,
    IN CPSS_LOG_TYPE_ENT        logType,
    IN GT_CHAR_PTR              namePtr,
    IN void                   * fieldPtr,
    INOUT PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC     * inOutParamInfoPtr
)
{
    CPSS_TUNNEL_TYPE_ENT tunnelType;
    CPSS_TUNNEL_TYPE_ENT *tunnelTypePtr;

    PRV_CPSS_LOG_START_FIELD_STC_MAC(CPSS_DXCH_TUNNEL_TERM_CONFIG_UNT *, valPtr);
    tunnelTypePtr = (CPSS_TUNNEL_TYPE_ENT*)inOutParamInfoPtr->paramKey.paramKeyArr[5];
    if (tunnelTypePtr != NULL) 
    {
        tunnelType = *tunnelTypePtr; 
    }
    else
    {
        tunnelType = (CPSS_TUNNEL_TYPE_ENT)inOutParamInfoPtr->paramKey.paramKeyArr[4]; 
    }
    switch (tunnelType) 
    {
        case CPSS_TUNNEL_IP_OVER_MPLS_E:
        case CPSS_TUNNEL_ETHERNET_OVER_MPLS_E:
            PRV_CPSS_LOG_STC_STC_MAC(valPtr, mplsCfg, CPSS_DXCH_TUNNEL_TERM_MPLS_CONFIG_STC);
            break;
        default:
            PRV_CPSS_LOG_STC_STC_MAC(valPtr, ipv4Cfg, CPSS_DXCH_TUNNEL_TERM_IPV4_CONFIG_STC);
            break;
    }
    prvCpssLogStcLogEnd(contextLib, logType);
}                 
/********* parameters log functions *********/
/********* api pre-log functions *********/
void cpssDxChTunnelStartEntrySet_preLogic
(
    IN va_list  args,
    OUT PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC * paramDataPtr
)
{
    /* devNum */
    va_arg(args, GT_U32);
    /* routerArpTunnelStartLineIndex */
    va_arg(args, GT_U32);
    /* tunnelType */
    paramDataPtr->paramKey.paramKeyArr[4] = (CPSS_TUNNEL_TYPE_ENT)va_arg(args, CPSS_TUNNEL_TYPE_ENT);
}
void cpssDxChTunnelStartEntryGet_preLogic
(
    IN va_list  args,
    OUT PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC * paramDataPtr
)
{
    /* devNum */
    va_arg(args, GT_U32);
    /* routerArpTunnelStartLineIndex */
    va_arg(args, GT_U32);
    /* tunnelTypePtr */
    paramDataPtr->paramKey.paramKeyArr[5] = (GT_UINTPTR)va_arg(args, CPSS_TUNNEL_TYPE_ENT*);
}
void cpssDxChTunnelStartPortGroupEntrySet_preLogic
(
    IN va_list  args,
    OUT PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC * paramDataPtr
)
{
    /* devNum */
    va_arg(args, GT_U32);
    /* portGroupsBmp */
    va_arg(args, GT_PORT_GROUPS_BMP);
    /* routerArpTunnelStartLineIndex */
    va_arg(args, GT_U32);
    /* tunnelType */
    paramDataPtr->paramKey.paramKeyArr[4] = (CPSS_TUNNEL_TYPE_ENT)va_arg(args, CPSS_TUNNEL_TYPE_ENT);
}
void cpssDxChTunnelStartPortGroupEntryGet_preLogic
(
    IN va_list  args,
    OUT PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC * paramDataPtr
)
{
    /* devNum */
    va_arg(args, GT_U32);
    /* portGroupsBmp */
    va_arg(args, GT_PORT_GROUPS_BMP);
    /* routerArpTunnelStartLineIndex */
    va_arg(args, GT_U32);
    /* tunnelTypePtr */
    paramDataPtr->paramKey.paramKeyArr[5] = (GT_UINTPTR)va_arg(args, CPSS_TUNNEL_TYPE_ENT*);
}
void cpssDxChTunnelTermEntryGet_preLogic
(
    IN va_list  args,
    OUT PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC * paramDataPtr
)
{
    /* devNum */
    va_arg(args, GT_U32);
    /* routerArpTunnelStartLineIndex */
    va_arg(args, GT_U32);
    /* validPtr */
    va_arg(args, GT_BOOL*);
    /* tunnelTypePtr */
    paramDataPtr->paramKey.paramKeyArr[5] = (GT_UINTPTR)va_arg(args, CPSS_TUNNEL_TYPE_ENT*);
}

