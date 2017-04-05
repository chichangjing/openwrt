/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* prvCpssDxChPclManualLog.c
*       Manually implemented CPSS Log type wrappers
* COMMENTS:
*
* FILE REVISION NUMBER:
*       $Revision: 1 $
*******************************************************************************/
#include <cpss/generic/log/cpssLog.h>
#include <cpss/generic/log/prvCpssLog.h>
#include <cpss/extServices/private/prvCpssBindFunc.h>
#include <cpss/dxCh/dxChxGen/pcl/cpssDxChPcl.h>
#include <cpss/dxCh/dxChxGen/pcl/private/prvCpssDxChPclLog.h>
#include <cpss/dxCh/dxChxGen/pcl/private/prvCpssDxChPcl.h>
#include <cpss/generic/log/prvCpssGenLog.h>
#include <cpss/generic/networkIf/private/prvCpssGenNetworkIfLog.h>
#include <cpss/generic/pcl/private/prvCpssGenPclLog.h>
#include <cpss/generic/config/private/prvCpssConfigTypes.h>

/********* enums *********/
static void prvCpssLogParamFunc_GT_U32_ARRAY_sizeSet1(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_SET_PARAM_VAL_MAC(GT_U32*, paramVal);
    inOutParamInfoPtr->paramKey.paramKeyArr[2] = inOutParamInfoPtr->paramKey.paramKeyArr[5];
    prvCpssLogArrayOfParamsHandle(contextLib,logType,namePtr,paramVal,sizeof(GT_U32),inOutParamInfoPtr,prvCpssLogParamFuncStc_GT_U32_PTR);
}

PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_U32_ARRAY_maskArr = {
     "maskArr", PRV_CPSS_LOG_PARAM_OUT_E,  PRV_CPSS_LOG_FUNC_TYPE_ARRAY_MAC(GT_U32)
};
PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_U32_ARRAY_patternArr = {
     "patternArr", PRV_CPSS_LOG_PARAM_OUT_E,  PRV_CPSS_LOG_FUNC_TYPE_ARRAY_MAC(GT_U32)
};
PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_U32_ARRAY_actionArr = {
     "actionArr", PRV_CPSS_LOG_PARAM_OUT_E,  prvCpssLogParamFunc_GT_U32_ARRAY_sizeSet1
};

PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChPclRuleGet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_CPSS_PCL_RULE_SIZE_ENT_ruleSize,
    &IN_GT_U32_ruleIndex,
    &OUT_GT_U32_ARRAY_maskArr,
    &OUT_GT_U32_ARRAY_patternArr,
    &OUT_GT_U32_ARRAY_actionArr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChPclPortGroupRuleGet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_PORT_GROUPS_BMP_portGroupsBmp,
    &IN_CPSS_PCL_RULE_SIZE_ENT_ruleSize,
    &IN_GT_U32_ruleIndex,
    &OUT_GT_U32_ARRAY_maskArr,
    &OUT_GT_U32_ARRAY_patternArr,
    &OUT_GT_U32_ARRAY_actionArr
};
/********* structure fields log functions *********/
void prvCpssLogParamFuncStc_CPSS_DXCH_PCL_ACTION_QOS_UNT_PTR
(
    IN CPSS_LOG_LIB_ENT         contextLib,
    IN CPSS_LOG_TYPE_ENT        logType,
    IN GT_CHAR_PTR              namePtr,
    IN void                   * fieldPtr,
    INOUT PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC     * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_START_FIELD_STC_MAC(CPSS_DXCH_PCL_ACTION_QOS_UNT *, valPtr);
    if (inOutParamInfoPtr->paramKey.paramKeyArr[4] == GT_FALSE) 
    {
        prvCpssLogStcLogStart(contextLib,  logType, "ingress"); 
        PRV_CPSS_LOG_STC_ENUM_MAC((&valPtr->ingress), modifyDscp, CPSS_PACKET_ATTRIBUTE_MODIFY_TYPE_ENT);
        PRV_CPSS_LOG_STC_ENUM_MAC((&valPtr->ingress), modifyUp, CPSS_PACKET_ATTRIBUTE_MODIFY_TYPE_ENT);
        PRV_CPSS_LOG_STC_NUMBER_MAC((&valPtr->ingress), profileIndex);
        PRV_CPSS_LOG_STC_BOOL_MAC((&valPtr->ingress), profileAssignIndex);
        PRV_CPSS_LOG_STC_ENUM_MAC((&valPtr->ingress), profilePrecedence, CPSS_PACKET_ATTRIBUTE_ASSIGN_PRECEDENCE_ENT);
        PRV_CPSS_LOG_STC_ENUM_MAC((&valPtr->ingress), up1Cmd, CPSS_DXCH_PCL_ACTION_INGRESS_UP1_CMD_ENT);
        PRV_CPSS_LOG_STC_NUMBER_MAC((&valPtr->ingress), up1);
        prvCpssLogStcLogEnd(contextLib, logType);
    }
    else
    {
        prvCpssLogStcLogStart(contextLib,  logType, "egress");
        PRV_CPSS_LOG_STC_ENUM_MAC((&valPtr->egress), modifyDscp, CPSS_DXCH_PCL_ACTION_EGRESS_DSCP_EXP_CMD_ENT);
        PRV_CPSS_LOG_STC_NUMBER_MAC((&valPtr->egress), dscp);
        PRV_CPSS_LOG_STC_ENUM_MAC((&valPtr->egress), modifyUp, CPSS_DXCH_PCL_ACTION_EGRESS_TAG0_CMD_ENT);
        PRV_CPSS_LOG_STC_NUMBER_MAC((&valPtr->egress), up);
        PRV_CPSS_LOG_STC_BOOL_MAC((&valPtr->egress), up1ModifyEnable);
        PRV_CPSS_LOG_STC_NUMBER_MAC((&valPtr->egress), up1);
        prvCpssLogStcLogEnd(contextLib, logType);
    }
    prvCpssLogStcLogEnd(contextLib, logType);
}
void prvCpssLogParamFuncStc_CPSS_DXCH_PCL_ACTION_REDIRECT_STC_PTR
(
    IN CPSS_LOG_LIB_ENT         contextLib,
    IN CPSS_LOG_TYPE_ENT        logType,
    IN GT_CHAR_PTR              namePtr,
    IN void                   * fieldPtr,
    INOUT PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC     * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_START_FIELD_STC_MAC(CPSS_DXCH_PCL_ACTION_REDIRECT_STC *, valPtr);
    PRV_CPSS_LOG_UNUSED_MAC(inOutParamInfoPtr);
    PRV_CPSS_LOG_STC_ENUM_MAC(valPtr, redirectCmd, CPSS_DXCH_PCL_ACTION_REDIRECT_CMD_ENT);
    prvCpssLogStcLogStart(contextLib,  logType, "data");
    switch (valPtr->redirectCmd) 
    {
        case CPSS_DXCH_PCL_ACTION_REDIRECT_CMD_OUT_IF_E:
            prvCpssLogStcLogStart(contextLib,  logType, "outIf"); 
            PRV_CPSS_LOG_STC_STC_MAC((&(&valPtr->data)->outIf), outInterface, CPSS_INTERFACE_INFO_STC);
            PRV_CPSS_LOG_STC_BOOL_MAC((&(&valPtr->data)->outIf), vntL2Echo);
            PRV_CPSS_LOG_STC_BOOL_MAC((&(&valPtr->data)->outIf), tunnelStart);
            PRV_CPSS_LOG_STC_NUMBER_MAC((&(&valPtr->data)->outIf), tunnelPtr);
            PRV_CPSS_LOG_STC_ENUM_MAC((&(&valPtr->data)->outIf), tunnelType, CPSS_DXCH_PCL_ACTION_REDIRECT_TUNNEL_TYPE_ENT);
            PRV_CPSS_LOG_STC_NUMBER_MAC((&(&valPtr->data)->outIf), arpPtr);
            PRV_CPSS_LOG_STC_BOOL_MAC((&(&valPtr->data)->outIf), modifyMacDa);
            PRV_CPSS_LOG_STC_BOOL_MAC((&(&valPtr->data)->outIf), modifyMacSa);
            prvCpssLogStcLogEnd(contextLib, logType);
            break;
        case CPSS_DXCH_PCL_ACTION_REDIRECT_CMD_ROUTER_E:
            PRV_CPSS_LOG_STC_NUMBER_MAC((&valPtr->data), routerLttIndex);
            break;
        case CPSS_DXCH_PCL_ACTION_REDIRECT_CMD_VIRT_ROUTER_E:
            PRV_CPSS_LOG_STC_NUMBER_MAC((&valPtr->data), vrfId);
            break;
        case CPSS_DXCH_PCL_ACTION_REDIRECT_CMD_REPLACE_MAC_SA_E:
            prvCpssLogStcLogStart(contextLib,  logType, "modifyMacSa");
            PRV_CPSS_LOG_STC_NUMBER_MAC((&(&valPtr->data)->modifyMacSa), arpPtr);
            PRV_CPSS_LOG_STC_ETH_MAC((&(&valPtr->data)->modifyMacSa), macSa);
            prvCpssLogStcLogEnd(contextLib, logType);
            break;
        case CPSS_DXCH_PCL_ACTION_REDIRECT_CMD_NONE_E:
            prvCpssLogStcLogStart(contextLib,  logType, "noRedirect");
            PRV_CPSS_LOG_STC_NUMBER_MAC((&(&valPtr->data)->noRedirect), arpPtr);
            PRV_CPSS_LOG_STC_BOOL_MAC((&(&valPtr->data)->noRedirect), modifyMacDa);
            prvCpssLogStcLogEnd(contextLib, logType);
            break;
        case CPSS_DXCH_PCL_ACTION_REDIRECT_CMD_LOGICAL_PORT_ASSIGN_E:
            prvCpssLogStcLogStart(contextLib,  logType, "logicalSourceInterface");
            PRV_CPSS_LOG_STC_STC_MAC((&(&valPtr->data)->logicalSourceInterface), logicalInterface, CPSS_INTERFACE_INFO_STC);
            PRV_CPSS_LOG_STC_BOOL_MAC((&(&valPtr->data)->logicalSourceInterface), sourceMeshIdSetEnable);
            PRV_CPSS_LOG_STC_NUMBER_MAC((&(&valPtr->data)->logicalSourceInterface), sourceMeshId);
            PRV_CPSS_LOG_STC_BOOL_MAC((&(&valPtr->data)->logicalSourceInterface), userTagAcEnable);
            prvCpssLogStcLogEnd(contextLib, logType);
            break;
        default:
            break;
    }
    prvCpssLogStcLogEnd(contextLib, logType);
    prvCpssLogStcLogEnd(contextLib, logType);
}
void prvCpssLogParamFuncStc_CPSS_DXCH_PCL_ACTION_VLAN_UNT_PTR
(
    IN CPSS_LOG_LIB_ENT         contextLib,
    IN CPSS_LOG_TYPE_ENT        logType,
    IN GT_CHAR_PTR              namePtr,
    IN void                   * fieldPtr,
    INOUT PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC     * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_START_FIELD_STC_MAC(CPSS_DXCH_PCL_ACTION_VLAN_UNT *, valPtr);
    if (inOutParamInfoPtr->paramKey.paramKeyArr[4] == GT_FALSE) 
    {
        prvCpssLogStcLogStart(contextLib,  logType, "ingress");
        PRV_CPSS_LOG_STC_ENUM_MAC((&valPtr->ingress), modifyVlan, CPSS_PACKET_ATTRIBUTE_ASSIGN_CMD_ENT);
        PRV_CPSS_LOG_STC_BOOL_MAC((&valPtr->ingress), nestedVlan);
        PRV_CPSS_LOG_STC_NUMBER_MAC((&valPtr->ingress), vlanId);
        PRV_CPSS_LOG_STC_ENUM_MAC((&valPtr->ingress), precedence, CPSS_PACKET_ATTRIBUTE_ASSIGN_PRECEDENCE_ENT);
        PRV_CPSS_LOG_STC_ENUM_MAC((&valPtr->ingress), vlanId1Cmd, CPSS_DXCH_PCL_ACTION_INGRESS_VLAN_ID1_CMD_ENT);
        PRV_CPSS_LOG_STC_NUMBER_MAC((&valPtr->ingress), vlanId1);
        prvCpssLogStcLogEnd(contextLib, logType);
    }
    else
    {
        prvCpssLogStcLogStart(contextLib,  logType, "egress");
        PRV_CPSS_LOG_STC_ENUM_MAC((&valPtr->egress), vlanCmd, CPSS_DXCH_PCL_ACTION_EGRESS_TAG0_CMD_ENT);
        PRV_CPSS_LOG_STC_NUMBER_MAC((&valPtr->egress), vlanId);
        PRV_CPSS_LOG_STC_BOOL_MAC((&valPtr->egress), vlanId1ModifyEnable);
        PRV_CPSS_LOG_STC_NUMBER_MAC((&valPtr->egress), vlanId1);
        prvCpssLogStcLogEnd(contextLib, logType);
    }
    prvCpssLogStcLogEnd(contextLib, logType);
}
void prvCpssLogParamFuncStc_CPSS_DXCH_PCL_ACTION_STC_PTR
(
    IN CPSS_LOG_LIB_ENT         contextLib,
    IN CPSS_LOG_TYPE_ENT        logType,
    IN GT_CHAR_PTR              namePtr,
    IN void                   * fieldPtr,
    INOUT PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC     * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_START_FIELD_STC_MAC(CPSS_DXCH_PCL_ACTION_STC *, valPtr);
    PRV_CPSS_LOG_STC_ENUM_MAC(valPtr, pktCmd, CPSS_PACKET_CMD_ENT);
    PRV_CPSS_LOG_STC_BOOL_MAC(valPtr, actionStop);
    PRV_CPSS_LOG_STC_BOOL_MAC(valPtr, bypassBridge);
    PRV_CPSS_LOG_STC_BOOL_MAC(valPtr, bypassIngressPipe);
    PRV_CPSS_LOG_STC_BOOL_MAC(valPtr, egressPolicy);
    inOutParamInfoPtr->paramKey.paramKeyArr[4] = (GT_BOOL)valPtr->egressPolicy;
    PRV_CPSS_LOG_STC_STC_MAC(valPtr, lookupConfig, CPSS_DXCH_PCL_ACTION_LOOKUP_CONFIG_STC);
    PRV_CPSS_LOG_STC_STC_MAC(valPtr, mirror, CPSS_DXCH_PCL_ACTION_MIRROR_STC);
    PRV_CPSS_LOG_STC_STC_MAC(valPtr, matchCounter, CPSS_DXCH_PCL_ACTION_MATCH_COUNTER_STC);
    PRV_CPSS_LOG_STC_STC_MAC(valPtr, qos, CPSS_DXCH_PCL_ACTION_QOS_UNT);
    PRV_CPSS_LOG_STC_STC_MAC(valPtr, redirect, CPSS_DXCH_PCL_ACTION_REDIRECT_STC);
    PRV_CPSS_LOG_STC_STC_MAC(valPtr, policer, CPSS_DXCH_PCL_ACTION_POLICER_STC);
    PRV_CPSS_LOG_STC_STC_MAC(valPtr, vlan, CPSS_DXCH_PCL_ACTION_VLAN_UNT);
    PRV_CPSS_LOG_STC_STC_MAC(valPtr, ipUcRoute, CPSS_DXCH_PCL_ACTION_IP_UC_ROUTE_STC);
    PRV_CPSS_LOG_STC_STC_MAC(valPtr, sourceId, CPSS_DXCH_PCL_ACTION_SOURCE_ID_STC);
    PRV_CPSS_LOG_STC_STC_MAC(valPtr, oam, CPSS_DXCH_PCL_ACTION_OAM_STC);
    PRV_CPSS_LOG_STC_NUMBER_MAC(valPtr, flowId);
    PRV_CPSS_LOG_STC_STC_MAC(valPtr, sourcePort, CPSS_DXCH_PCL_ACTION_SOURCE_PORT_STC);
    PRV_CPSS_LOG_STC_BOOL_MAC(valPtr, setMacToMe);
    PRV_CPSS_LOG_STC_BOOL_MAC(valPtr, channelTypeToOpcodeMapEnable);
    PRV_CPSS_LOG_STC_NUMBER_MAC(valPtr, tmQueueId);
    PRV_CPSS_LOG_STC_BOOL_MAC(valPtr, unknownSaCommandEnable);
    PRV_CPSS_LOG_STC_ENUM_MAC(valPtr, unknownSaCommand, CPSS_PACKET_CMD_ENT);
    prvCpssLogStcLogEnd(contextLib, logType);
}
void prvCpssLogParamFuncStc_CPSS_DXCH_PCL_RULE_FORMAT_UNT_PTR
(
    IN CPSS_LOG_LIB_ENT         contextLib,
    IN CPSS_LOG_TYPE_ENT        logType,
    IN GT_CHAR_PTR              namePtr,
    IN void                   * fieldPtr,
    INOUT PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC     * inOutParamInfoPtr
)
{
    CPSS_DXCH_PCL_RULE_FORMAT_TYPE_ENT ruleFormat;

    PRV_CPSS_LOG_START_FIELD_STC_MAC(CPSS_DXCH_PCL_RULE_FORMAT_UNT *, valPtr);
    ruleFormat = (CPSS_DXCH_PCL_RULE_FORMAT_TYPE_ENT)inOutParamInfoPtr->paramKey.paramKeyArr[5];

    switch (ruleFormat)
    {
        case CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_STD_NOT_IP_E:
            PRV_CPSS_LOG_STC_STC_MAC(valPtr, ruleStdNotIp, CPSS_DXCH_PCL_RULE_FORMAT_STD_NOT_IP_STC);
            break;
        case CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_STD_IP_L2_QOS_E:
            PRV_CPSS_LOG_STC_STC_MAC(valPtr, ruleStdIpL2Qos, CPSS_DXCH_PCL_RULE_FORMAT_STD_IP_L2_QOS_STC);
            break;
        case CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_STD_IPV4_L4_E:
            PRV_CPSS_LOG_STC_STC_MAC(valPtr, ruleStdIpv4L4, CPSS_DXCH_PCL_RULE_FORMAT_STD_IPV4_L4_STC);
            break;
        case CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_STD_IPV6_DIP_E:
            PRV_CPSS_LOG_STC_STC_MAC(valPtr, ruleStdIpv6Dip, CPSS_DXCH_PCL_RULE_FORMAT_STD_IPV6_DIP_STC);
            break;
        case CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_EXT_NOT_IPV6_E:
            PRV_CPSS_LOG_STC_STC_MAC(valPtr, ruleExtNotIpv6, CPSS_DXCH_PCL_RULE_FORMAT_EXT_NOT_IPV6_STC);
            break;
        case CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_EXT_IPV6_L2_E:
            PRV_CPSS_LOG_STC_STC_MAC(valPtr, ruleExtIpv6L2, CPSS_DXCH_PCL_RULE_FORMAT_EXT_IPV6_L2_STC);
            break;
        case CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_EXT_IPV6_L4_E:
            PRV_CPSS_LOG_STC_STC_MAC(valPtr, ruleExtIpv6L4, CPSS_DXCH_PCL_RULE_FORMAT_EXT_IPV6_L4_STC);
            break;
        case CPSS_DXCH_PCL_RULE_FORMAT_EGRESS_STD_NOT_IP_E:
            PRV_CPSS_LOG_STC_STC_MAC(valPtr, ruleEgrStdNotIp, CPSS_DXCH_PCL_RULE_FORMAT_EGRESS_STD_NOT_IP_STC);
            break;
        case CPSS_DXCH_PCL_RULE_FORMAT_EGRESS_STD_IP_L2_QOS_E:
            PRV_CPSS_LOG_STC_STC_MAC(valPtr, ruleEgrStdIpL2Qos, CPSS_DXCH_PCL_RULE_FORMAT_EGRESS_STD_IP_L2_QOS_STC);
            break;
        case CPSS_DXCH_PCL_RULE_FORMAT_EGRESS_STD_IPV4_L4_E:
            PRV_CPSS_LOG_STC_STC_MAC(valPtr, ruleEgrStdIpv4L4, CPSS_DXCH_PCL_RULE_FORMAT_EGRESS_STD_IPV4_L4_STC);
            break;
        case CPSS_DXCH_PCL_RULE_FORMAT_EGRESS_EXT_NOT_IPV6_E:
            PRV_CPSS_LOG_STC_STC_MAC(valPtr, ruleEgrExtNotIpv6, CPSS_DXCH_PCL_RULE_FORMAT_EGRESS_EXT_NOT_IPV6_STC);
            break;
        case CPSS_DXCH_PCL_RULE_FORMAT_EGRESS_EXT_IPV6_L2_E:
            PRV_CPSS_LOG_STC_STC_MAC(valPtr, ruleEgrExtIpv6L2, CPSS_DXCH_PCL_RULE_FORMAT_EGRESS_EXT_IPV6_L2_STC);
            break;
        case CPSS_DXCH_PCL_RULE_FORMAT_EGRESS_EXT_IPV6_L4_E:
            PRV_CPSS_LOG_STC_STC_MAC(valPtr, ruleEgrExtIpv6L4, CPSS_DXCH_PCL_RULE_FORMAT_EGRESS_EXT_IPV6_L4_STC);
            break;
        case CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_STD_UDB_E:
            PRV_CPSS_LOG_STC_STC_MAC(valPtr, ruleIngrStdUdb, CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_STANDARD_UDB_STC);
            break;
        case CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_EXT_UDB_E:
            PRV_CPSS_LOG_STC_STC_MAC(valPtr, ruleIngrExtUdb, CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_EXTENDED_UDB_STC);
            break;
        case CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_STD_IPV4_ROUTED_ACL_QOS_E:
            PRV_CPSS_LOG_STC_STC_MAC(valPtr, ruleStdIpv4RoutedAclQos, CPSS_DXCH_PCL_RULE_FORMAT_STD_IPV4_ROUTED_ACL_QOS_STC);
            break;
        case CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_EXT_IPV4_PORT_VLAN_QOS_E:
            PRV_CPSS_LOG_STC_STC_MAC(valPtr, ruleExtIpv4PortVlanQos, CPSS_DXCH_PCL_RULE_FORMAT_EXT_IPV4_PORT_VLAN_QOS_STC);
            break;
        case CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_ULTRA_IPV6_PORT_VLAN_QOS_E:
            PRV_CPSS_LOG_STC_STC_MAC(valPtr, ruleUltraIpv6PortVlanQos, CPSS_DXCH_PCL_RULE_FORMAT_ULTRA_IPV6_PORT_VLAN_QOS_STC);
            break;
        case CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_ULTRA_IPV6_ROUTED_ACL_QOS_E:
            PRV_CPSS_LOG_STC_STC_MAC(valPtr, ruleUltraIpv6RoutedAclQos, CPSS_DXCH_PCL_RULE_FORMAT_ULTRA_IPV6_ROUTED_ACL_QOS_STC);
            break;
        case CPSS_DXCH_PCL_RULE_FORMAT_EGRESS_EXT_IPV4_RACL_VACL_E:
            PRV_CPSS_LOG_STC_STC_MAC(valPtr, ruleEgrExtIpv4RaclVacl, CPSS_DXCH_PCL_RULE_FORMAT_EGRESS_EXT_IPV4_RACL_VACL_STC);
            break;
        case CPSS_DXCH_PCL_RULE_FORMAT_EGRESS_ULTRA_IPV6_RACL_VACL_E:
            PRV_CPSS_LOG_STC_STC_MAC(valPtr, ruleEgrUltraIpv6RaclVacl, CPSS_DXCH_PCL_RULE_FORMAT_EGRESS_ULTRA_IPV6_RACL_VACL_STC);
            break;
        case CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_UDB_10_E:
        case CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_UDB_20_E:
        case CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_UDB_30_E:
        case CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_UDB_40_E:
        case CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_UDB_50_E:
        case CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_UDB_60_E:
            PRV_CPSS_LOG_STC_STC_MAC(valPtr, ruleIngrUdbOnly, CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_UDB_ONLY_STC);
            break;
        case CPSS_DXCH_PCL_RULE_FORMAT_EGRESS_UDB_10_E:
        case CPSS_DXCH_PCL_RULE_FORMAT_EGRESS_UDB_20_E:
        case CPSS_DXCH_PCL_RULE_FORMAT_EGRESS_UDB_30_E:
        case CPSS_DXCH_PCL_RULE_FORMAT_EGRESS_UDB_40_E:
        case CPSS_DXCH_PCL_RULE_FORMAT_EGRESS_UDB_50_E:
        case CPSS_DXCH_PCL_RULE_FORMAT_EGRESS_UDB_60_E:
            PRV_CPSS_LOG_STC_STC_MAC(valPtr, ruleEgrUdbOnly, CPSS_DXCH_PCL_RULE_FORMAT_EGRESS_UDB_ONLY_STC);
            break;
        default:
            break;
    }
    prvCpssLogStcLogEnd(contextLib, logType);
}
/********* parameters log functions *********/

/********* api pre-log functions *********/
void cpssDxChPclRuleSet_preLogic
(
    IN va_list  args,
    OUT PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC * paramDataPtr
)
{
    /* devNum */
    va_arg(args, GT_32);
    /* ruleFormat */
    paramDataPtr->paramKey.paramKeyArr[5] = (CPSS_DXCH_PCL_RULE_FORMAT_TYPE_ENT)va_arg(args, CPSS_DXCH_PCL_RULE_FORMAT_TYPE_ENT);
}
void cpssDxChPclRuleGet_preLogic
(
    IN va_list  args,
    OUT PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC * paramDataPtr
)
{
    CPSS_PCL_RULE_SIZE_ENT ruleSize;
    GT_U32 actualRuleSize; /* rule size in words */
    GT_U32 alignment; /* temporary variable for calling prvCpssDxChPclActionSizeAndAllignmentGet */
    GT_U8 devNum; /* devNum for extracting the size using prvCpssDxChPclActionSizeAndAllignmentGet */

    /* devNum */
    devNum = (GT_U8)va_arg(args, GT_32);
    /* ruleSize */
    ruleSize = va_arg(args, CPSS_PCL_RULE_SIZE_ENT);
    /* convert ruleSize to actualSize */
    switch (ruleSize) 
    {
        case CPSS_PCL_RULE_SIZE_STD_E:
            actualRuleSize = 8;
            break;
        case CPSS_PCL_RULE_SIZE_EXT_E:
            actualRuleSize = 15;
            break;
        case CPSS_PCL_RULE_SIZE_ULTRA_E:
            actualRuleSize = 20;
            break;
        case CPSS_PCL_RULE_SIZE_10_BYTES_E:
            actualRuleSize = 3;
            break;
        case CPSS_PCL_RULE_SIZE_20_BYTES_E:
            actualRuleSize = 5;
            break;
        case CPSS_PCL_RULE_SIZE_40_BYTES_E:
            actualRuleSize = 10;
            break;
        case CPSS_PCL_RULE_SIZE_50_BYTES_E:
            actualRuleSize = 13;
            break;
        default:
            actualRuleSize = 0;
    }
    paramDataPtr->paramKey.paramKeyArr[2] = actualRuleSize;
    if(PRV_CPSS_IS_DEV_EXISTS_MAC(devNum))
    {
        /* getting the size of the action array */
        prvCpssDxChPclActionSizeAndAllignmentGet(devNum,&actualRuleSize,&alignment);
    }
    else
    {
        /* use minimal possible size */
        actualRuleSize = 3;
    }
    paramDataPtr->paramKey.paramKeyArr[5] = actualRuleSize;
}
void cpssDxChPclPortGroupRuleSet_preLogic
(
    IN va_list  args,
    OUT PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC * paramDataPtr
)
{
    /* devNum */
    va_arg(args, GT_32);
    /* portGroupsBmp */
    va_arg(args, GT_PORT_GROUPS_BMP);
    /* ruleFormat */
    paramDataPtr->paramKey.paramKeyArr[5] = (CPSS_DXCH_PCL_RULE_FORMAT_TYPE_ENT)va_arg(args, CPSS_DXCH_PCL_RULE_FORMAT_TYPE_ENT);
}

