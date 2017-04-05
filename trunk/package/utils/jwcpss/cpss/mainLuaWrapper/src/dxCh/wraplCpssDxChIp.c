/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* wraplCpssDxChIp.c
*
* DESCRIPTION:
*       A lua wrapper for ip address functions
*
* DEPENDENCIES:
*
* COMMENTS:
*
* FILE REVISION NUMBER:
*       $Revision: 22 $
*******************************************************************************/

#include <generic/private/prvWraplGeneral.h>
#include <cpss/generic/config/cpssGenCfg.h>
#include <cpss/dxCh/dxChxGen/config/private/prvCpssDxChInfo.h>
#include <cpss/dxCh/dxChxGen/ip/cpssDxChIp.h>
#include <cpss/dxCh/dxChxGen/ipLpmEngine/cpssDxChIpLpm.h>
#include <dxCh/wraplCpssDxChDeviceInfo.h>
#include <cpssCommon/wraplCpssDeviceInfo.h>
#include <cpssCommon/wrapCpssDebugInfo.h>
#include <cpss/dxCh/dxChxGen/private/lpm/prvCpssDxChLpmTypes.h>

#include <extUtils/luaCLI/luaCLIEngine_genWrapper.h>

#define MAX_VRID 4096

/*******************************************************************************
*   Ip address wrapper definitions                                             *
*******************************************************************************/


/*******************************************************************************
*   Ip address wrapper errors                                                  *
*******************************************************************************/
extern GT_CHAR wrongIpProtocolDefinitionErrorMessage[];
GT_CHAR ipUcRouteEntryReadErrorMessage[]            =
                                            "Could not read ip unicast entry";
GT_CHAR ipUcRouteEntryFromLuaGettingErrorMessage[]  =
                                    "There is no ip unicast entry in lua stack";
GT_CHAR ipUcRouteEntryWriteErrorMessage[]           =
                                            "Could not write ip unicast entry";
GT_CHAR invalidIpUcRouteEntryErrorMessage[]         =
                                            "Next-hop entry is not initialized";
GT_CHAR ipPreffixAddingErrorMessage[]               = "Ip prefix adding error";
GT_CHAR ipPreffixDeletingErrorMessage[]             =
                                                    "Ip prefix deleting error";
GT_CHAR ipPrefixSearchingErrorMessage[]             =
                                                    "Ip prefix searching error";


/***** declarations ********/

use_prv_struct(CPSS_DXCH_IP_UC_ROUTE_ENTRY_STC);
use_prv_print_struct(GT_ETHERADDR);
use_prv_print_struct(GT_IPADDR);
use_prv_print_struct(GT_IPV6ADDR);
use_prv_print_struct(CPSS_DXCH_IP_UC_ROUTE_ENTRY_STC);
use_prv_print_struct(CPSS_DXCH_IP_TCAM_ROUTE_ENTRY_INFO_UNT);

/***** declarations ********/


/*******************************************************************************
* prvCpssDxChIsRoutingPolicyBased
*
* DESCRIPTION:
*       Checks If Policy based routing
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       lpmDBId             - LPM DB number
*
* OUTPUTS:
*       isPolicyBasedPtr  - (pointer to)GT_TRUE - policy based routing, GT_FALSE - other
*
* RETURNS:
*       GT_OK - on sucess, other on fail
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvCpssDxChIsRoutingPolicyBased
(
    IN  GT_U32               lpmDBId,
    OUT GT_BOOL              *isPolicyBasedPtr
)
{
    GT_STATUS                                    status;
    CPSS_DXCH_IP_LPM_SHADOW_TYPE_ENT             shadowType;
    CPSS_IP_PROTOCOL_STACK_ENT                   protocolStack;
    CPSS_DXCH_IP_TCAM_LPM_MANGER_INDEX_RANGE_STC indexesRange;
    CPSS_DXCH_IP_TCAM_LPM_MANGER_CAPCITY_CFG_STC tcamLpmManagerCapcityCfg;
    CPSS_DXCH_IP_LPM_MEMORY_CONFIG_UNT           memoryCfg;

    /* initialize pointers in the structure - should point to real memory */
    cpssOsMemSet(&indexesRange, 0, sizeof(indexesRange));
    cpssOsMemSet(&tcamLpmManagerCapcityCfg, 0, sizeof(tcamLpmManagerCapcityCfg));
    cpssOsMemSet(&memoryCfg, 0, sizeof(memoryCfg));
    memoryCfg.tcamDbCfg.indexesRangePtr = &indexesRange;
    memoryCfg.tcamDbCfg.tcamLpmManagerCapcityCfgPtr = &tcamLpmManagerCapcityCfg;

    status = cpssDxChIpLpmDBConfigGet(
        lpmDBId, &shadowType, &protocolStack, &memoryCfg);
    if (status != GT_OK)
    {
        return status;
    }

    if ((shadowType == CPSS_DXCH_IP_LPM_TCAM_CHEETAH_SHADOW_E) ||
        (shadowType == CPSS_DXCH_IP_LPM_TCAM_XCAT_POLICY_BASED_ROUTING_SHADOW_E))
    {
        *isPolicyBasedPtr = GT_TRUE;
    }
    else
    {
        *isPolicyBasedPtr = GT_FALSE;
    }

    return GT_OK;
}

/*******************************************************************************
* prvCpssDxChMaximumRouterArpIndex
*
* DESCRIPTION:
*       Function Relevant mode : All modes
*
*       Getting of maximum router arp index.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum             - device number
*
* OUTPUTS:
*       routerArpIndexPtr  - router arp index
*       errorMessagePtr    - error message
*
* RETURNS:
*       GT_OK
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvCpssDxChMaximumRouterArpIndex
(
    IN  GT_U8                   devNum,
    OUT GT_U32_PTR              maximumRouterArpIndexPtr,
    OUT GT_CHAR_PTR             *errorMessagePtr
)
{
    GT_STATUS                   status          = GT_OK;
    *errorMessagePtr    = NULL;

    *maximumRouterArpIndexPtr  =
                WRL_PRV_CPSS_DXCH_PP_MAC(devNum)->fineTuning.tableSize.routerArp;

    return status;
}


/*******************************************************************************
* prvCpssDxChMaximumRouterNextHopEntryIndex
*
* DESCRIPTION:
*       Function Relevant mode : All modes
*
*       Getting of maximum router next hop entry index.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum                - device number
*
* OUTPUTS:
*       maximumRouterNextHopEntryIndexPtr
*                             - router next-hop index
*       errorMessagePtr       - error message
*
* RETURNS:
*       GT_OK
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvCpssDxChMaximumRouterNextHopEntryIndex
(
    IN  GT_U8                   devNum,
    OUT GT_U32_PTR              maximumRouterNextHopEntryIndexPtr,
    OUT GT_CHAR_PTR             *errorMessagePtr
)
{
    GT_STATUS                   status          = GT_OK;
    *errorMessagePtr    = NULL;

    *maximumRouterNextHopEntryIndexPtr  =
        WRL_PRV_CPSS_DXCH_PP_MAC(devNum)->fineTuning.tableSize.routerNextHop;

    return status;
}


/*******************************************************************************
* pvrCpssDxChIpUnicastEntryFromLuaGet
*
* DESCRIPTION:
*       Function Relevant mode : All modes
*
*       CPSS wrapper of port members array from lua stack.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*        L                    - lua state
*        L_index              - entry lua stack index
*
* OUTPUTS:
*       isIpUnicastEntryTaken - property, that ip unicast entry was in stack
*       ipUnicastEntry        - ip unicast entry
*       errorMessagePtr       - error message
*
* RETURNS:
*       GT_OK, if success otherwise error code and message
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS pvrCpssDxChIpUnicastEntryFromLuaGet
(
    IN  lua_State               *L,
    IN  GT_32                   L_index,
    OUT GT_BOOL                 *isIpUnicastEntryTaken,
    OUT CPSS_DXCH_IP_UC_ROUTE_ENTRY_STC
                                *ipUnicastEntry,
    OUT GT_CHAR_PTR             *errorMessagePtr
)
{
    GT_STATUS                   status          = GT_OK;

    *errorMessagePtr = NULL;

    if (0 != lua_istable(L, L_index))
    {
        lua_pushvalue(L, L_index);
        prv_lua_to_c_CPSS_DXCH_IP_UC_ROUTE_ENTRY_STC(L, ipUnicastEntry);
        lua_pop(L, 1);
        *isIpUnicastEntryTaken = GT_TRUE;
    }
    else
    {
        *isIpUnicastEntryTaken = GT_FALSE;
    }

    return status;
}


/*******************************************************************************
* pvrCpssDxChIpUnicastEntryOnConditionPush
*
* DESCRIPTION:
*       Function Relevant mode : All modes
*
*       CPSS wrapper of conditional pushing of ip unicast entry pushing to
*       lua stack.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       status                - caller status
*       L                     - lua state
*       routeEntry            - ip unicast entry
*       condition           - pushing condition
*
* OUTPUTS:
*
* RETURNS:
*       1 if string was pushed to stack otherwise 0
*
* COMMENTS:
*
*******************************************************************************/
int pvrCpssDxChIpUnicastEntryOnConditionPush
(
    IN GT_STATUS                status,
    IN lua_State                *L,
    IN CPSS_DXCH_IP_UC_ROUTE_ENTRY_STC
                                *routeEntry,
    IN GT_BOOL                  condition
)
{
    if ((GT_OK == status) && (GT_TRUE == condition))
    {
        prv_c_to_lua_CPSS_DXCH_IP_UC_ROUTE_ENTRY_STC(L, routeEntry);

        return 1;
    }
    else
    {
        return 0;
    }
}


/*******************************************************************************
* prvCpssDxChRouterArpEntryGetAndItsValidityCheck
*
* DESCRIPTION:
*       Function Relevant mode : All modes
*
*       Reading of router arp entry and its validity check.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum                - device number
*       routerArpIndex        - starting router arp index
*       zeroMacAddressIsValid - process zero mac address as valid
*       P_CALLING_FORMAL_DATA - general debug information (environment variables
*                               etc); could be empty
*
* OUTPUTS:
*       arpMacAddrPtr         - mac-address of router arp entry
*       errorMessagePtr       - error message
*
* RETURNS:
*       execution status
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvCpssDxChRouterArpEntryGetAndItsValidityCheck
(
    IN  GT_U8                   devNum,
    IN  GT_U32                  routerArpEntryIndex,
    IN  GT_BOOL                 zeroMacAddressIsValid,
    OUT GT_BOOL                 *isValidRouterArpEntryPtr,
    OUT GT_ETHERADDR            *arpMacAddrPtr,
    OUT GT_CHAR_PTR             *errorMessagePtr
    P_CALLING_FORMAL_DATA
)
{
    GT_BOOL                     isValidArpMacAddress    = GT_TRUE;
    GT_BOOL                     isZeroArpMacAddress     = GT_FALSE;
    GT_STATUS                   status                  = GT_OK;

    CPSS_API_CALL_LOCK();
    status = cpssDxChIpRouterArpAddrRead(devNum, routerArpEntryIndex,
                                         arpMacAddrPtr);
    CPSS_API_CALL_UNLOCK();
    P_CALLING_API(
        cpssDxChIpRouterArpAddrRead,
        PC_NUMBER(devNum,               devNum,             GT_U8,
        PC_NUMBER(routerArpEntryIndex,  routerArpIndex,     GT_U32,
        PC_STRUCT(arpMacAddrPtr,        arpMacAddr,         GT_ETHERADDR,
        PC_LAST_PARAM))),
        PC_STATUS);

    if ((GT_OK == status) && (GT_FALSE == zeroMacAddressIsValid))
    {
        status = prvCpssIsZeroMacAddress(arpMacAddrPtr, &isZeroArpMacAddress,
                                         errorMessagePtr);
        isValidArpMacAddress    = GT_FALSE == isZeroArpMacAddress;
    }

    if ((GT_OK == status) && (GT_TRUE == isValidArpMacAddress))
    {
        *isValidRouterArpEntryPtr = GT_TRUE;
    }
    else
    {
        *isValidRouterArpEntryPtr = GT_FALSE;
    }

    return status;
}


/*******************************************************************************
* prvCpssDxChIpUcRouteEntryGetAndItsValidityCheck
*
* DESCRIPTION:
*       Function Relevant mode : All modes
*
*       Reading of ip unicast route entry and its validity check.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum                - device number
*       ipUcRouteEntryIndex   - starting ip unicast route entry index
*       P_CALLING_FORMAL_DATA - general debug information (environment variables
*                               etc); could be empty
*
* OUTPUTS:
*       isValidIpUcRouteEntryPtr
*                             - readed ip unicast route entry validity
*       ipUcRouteEntryPtr     - readed ip unicast route entry
*       errorMessagePtr       - error message
*
* RETURNS:
*       execution status
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvCpssDxChIpUcRouteEntryGetAndItsValidityCheck
(
    IN  GT_U8                   devNum,
    IN  GT_U32                  ipUcRouteEntryIndex,
    OUT GT_BOOL                 *isValidIpUcRouteEntryPtr,
    OUT CPSS_DXCH_IP_UC_ROUTE_ENTRY_STC
                                *ipUcRouteEntryPtr,
    OUT GT_CHAR_PTR             *errorMessagePtr
    P_CALLING_FORMAL_DATA
)
{
    GT_STATUS                   status                  = GT_OK;

    *errorMessagePtr = NULL;

    *isValidIpUcRouteEntryPtr = GT_TRUE;

    ipUcRouteEntryPtr->type = CPSS_DXCH_IP_UC_ROUTE_ENTRY_E;

    CPSS_API_CALL_LOCK();
    status = cpssDxChIpUcRouteEntriesRead(devNum, ipUcRouteEntryIndex,
                                          ipUcRouteEntryPtr, 1);
    CPSS_API_CALL_UNLOCK();
    P_CALLING_API(
        cpssDxChIpUcRouteEntriesRead,
        PC_NUMBER(devNum,               devNum,                          GT_U8,
        PC_NUMBER(ipUcRouteEntryIndex,  baseRouteEntryIndex,            GT_U32,
        PC_STRUCT(ipUcRouteEntryPtr,    routeEntriesArray,
                                               CPSS_DXCH_IP_UC_ROUTE_ENTRY_STC,
        PC_NUMBER(1,                    numOfRouteEntries,              GT_U32,
        PC_LAST_PARAM)))),
        PC_STATUS);

    if ((GT_OK != status) && (GT_BAD_STATE != status))
    {
        *errorMessagePtr = ipUcRouteEntryReadErrorMessage;
    }

    if (GT_BAD_STATE == status)
    {
        status                      = GT_OK;
        *isValidIpUcRouteEntryPtr   = GT_FALSE;
    }

    if ((GT_OK == status) && (GT_TRUE == *isValidIpUcRouteEntryPtr))
    {
        if (CPSS_DXCH_IP_UC_ROUTE_ENTRY_E == ipUcRouteEntryPtr->type)
        {
            if (CPSS_PACKET_CMD_ROUTE_E ==
                                    ipUcRouteEntryPtr->entry.regularEntry.cmd &&
                                    0 == ipUcRouteEntryPtr->entry.regularEntry.nextHopVlanId)
            {

                *isValidIpUcRouteEntryPtr   = GT_FALSE;
            }
        }
        else
        {
            *isValidIpUcRouteEntryPtr   = GT_FALSE;
        }
    }


    return status;
}


/*******************************************************************************
* prvCpssDxChIpUcRouteEntryValidityCheckOnAllDevices
*
* DESCRIPTION:
*       Function Relevant mode : All modes
*
*       Ip unicast route entry validity check under all devices.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum                - device number
*       ipUcRouteEntryIndex   - starting ip unicast route entry index
*       P_CALLING_FORMAL_DATA - general debug information (environment variables
*                               etc); could be empty
*
* OUTPUTS:
*       isValidIpUcRouteEntryPtr
*                             - readed ip unicast route entry validity
*       errorMessagePtr       - error message
*
* RETURNS:
*       execution status
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvCpssDxChIpUcRouteEntryValidityCheckOnAllDevices
(
    IN  GT_U32                  ipUcRouteEntryIndex,
    OUT GT_BOOL                 *isValidIpUcRouteEntryPtr,
    OUT GT_CHAR_PTR             *errorMessagePtr
    P_CALLING_FORMAL_DATA
)
{
    GT_U8                       devNum                  = 0xFF;
    CPSS_DXCH_IP_UC_ROUTE_ENTRY_STC
                                ipUcRouteEntry;
    GT_STATUS                   status                  = GT_OK;

    *isValidIpUcRouteEntryPtr = GT_FALSE;

    while ((GT_OK == status) && (GT_FALSE == *isValidIpUcRouteEntryPtr) &&
           (GT_OK == prvWrlDevFilterNextDevGet(devNum, &devNum)))
    {
        P_CALLING_API(
            prvWrlDevFilterNextDevGet,
            PC_NUMBER(devNum,                   devNum,                  GT_U8,
            PC_LAST_PARAM),
            PC_STATUS);

        if (prvCpssIsDevExists(devNum))
        {
            status = prvCpssDxChIpUcRouteEntryGetAndItsValidityCheck(
                         devNum, ipUcRouteEntryIndex, isValidIpUcRouteEntryPtr,
                         &ipUcRouteEntry, errorMessagePtr
                         P_CALLING_ACTUAL_DATA);
        }
    }

    return status;
}


/*******************************************************************************
* prvCpssDxChIpPrefixIpUcRouteEntryGet
*
* DESCRIPTION:
*       Function Relevant mode : All modes
*
*       Getting of ip prefix ip uc route entry index.
*
* APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* NOT APPLICABLE DEVICES:
*       None.
*
* INPUTS:
*       vrfId                     - a Virtual Router ID
*       ipProtocolStack           - ip protocol type
*       ipAddrPtr                 - ip v4 or v6 address
*       prefixLen                 - prefix length
*       P_CALLING_FORMAL_DATA     - general debug information (environment
*                                   variables etc); could be empty
*
* OUTPUTS:
*       isIpV4PrefixExistPtr      - ip prefix existance property
*       ipUcRouteEntryIndexPtr    - ip unicast route entry index
*       errorMessagePtr           - error message
*
*
* RETURNS:
*       execution status
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvCpssDxChIpPrefixIpUcRouteEntryGet
(
    IN  GT_U32                      vrfId,
    IN  CPSS_IP_PROTOCOL_STACK_ENT  ipProtocolStack,
    IN  GT_IP_ADDR_TYPE_UNT         *ipAddrPtr,
    IN  GT_U32                      prefixLen,
    OUT GT_BOOL                     *isIpV4PrefixExistPtr,
    OUT GT_U32                      *ipUcRouteEntryIndexPtr,
    OUT GT_CHAR_PTR                 *errorMessagePtr
    P_CALLING_FORMAL_DATA
)
{
    CPSS_DXCH_IP_TCAM_ROUTE_ENTRY_INFO_UNT
                                    nextHopInfo;
    GT_U32                          tcamRowIndex;
    GT_U32                          tcamColumnIndex;
    GT_STATUS                       status          = GT_OK;

    *errorMessagePtr = NULL;

    cpssOsMemSet(&nextHopInfo, 0, sizeof(nextHopInfo));

    CPSS_API_CALL_LOCK();
    switch (ipProtocolStack)
    {
    case CPSS_IP_PROTOCOL_IPV4_E:
        status = cpssDxChIpLpmIpv4UcPrefixSearch(0, vrfId, ipAddrPtr->ipv4Addr,
                                                 prefixLen, &nextHopInfo,
                                                 &tcamRowIndex,
                                                 &tcamColumnIndex);
        P_CALLING_API(
            cpssDxChIpLpmIpv4UcPrefixSearch,
            PC_NUMBER(0,                        lpmDBId,                GT_U32,
            PC_NUMBER(vrfId,                    vrId,                   GT_U32,
            PC_STRUCT(&(ipAddrPtr->ipv4Addr),   ipAddr,              GT_IPADDR,
            PC_NUMBER(prefixLen,                prefixLen,              GT_U32,
            PC_STRUCT(&nextHopInfo,             nextHopInfo,
                                        CPSS_DXCH_IP_TCAM_ROUTE_ENTRY_INFO_UNT,
            PC_NUMBER(tcamRowIndex,             tcamRowIndex,           GT_U32,
            PC_NUMBER(tcamColumnIndex,          tcamColumnIndex,        GT_U32,
            PC_LAST_PARAM))))))),
            PC_STATUS);
        break;

    case CPSS_IP_PROTOCOL_IPV6_E:
        status = cpssDxChIpLpmIpv6UcPrefixSearch(0, vrfId, ipAddrPtr->ipv6Addr,
                                                 prefixLen, &nextHopInfo,
                                                 &tcamRowIndex,
                                                 &tcamColumnIndex);
        P_CALLING_API(
            cpssDxChIpLpmIpv6UcPrefixSearch,
            PC_NUMBER(0,                        lpmDBId,                GT_U32,
            PC_NUMBER(vrfId,                    vrId,                   GT_U32,
            PC_STRUCT(&(ipAddrPtr->ipv6Addr),   ipAddr,            GT_IPV6ADDR,
            PC_NUMBER(prefixLen,                prefixLen,              GT_U32,
            PC_STRUCT(&nextHopInfo,             nextHopInfo,
                                        CPSS_DXCH_IP_TCAM_ROUTE_ENTRY_INFO_UNT,
            PC_NUMBER(tcamRowIndex,             tcamRowIndex,           GT_U32,
            PC_NUMBER(tcamColumnIndex,          tcamColumnIndex,        GT_U32,
            PC_LAST_PARAM))))))),
            PC_STATUS);
        break;

    default:
        status              = GT_BAD_PARAM;
        *errorMessagePtr    = wrongIpProtocolDefinitionErrorMessage;
        break;
    }
    CPSS_API_CALL_UNLOCK();
    if ((GT_OK != status) && (GT_NOT_FOUND != status))
    {
        *errorMessagePtr = ipPrefixSearchingErrorMessage;
    }

    if ((GT_OK == status) || (GT_NOT_FOUND == status))
    {
        *isIpV4PrefixExistPtr   = GT_NOT_FOUND != status;
        *ipUcRouteEntryIndexPtr = nextHopInfo.ipLttEntry.routeEntryBaseIndex;
        status                  = GT_OK;
    }

    return status;
}


/*******************************************************************************
* prvCpssDxChIpV4PrefixValidityCheck
*
* DESCRIPTION:
*       Function Relevant mode : All modes
*
*       Getting of ipv4 prefix ip uc route entry index, if ipv4 prefix is valid.
*
* APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* NOT APPLICABLE DEVICES:
*       None.
*
* INPUTS:
*       nextHopInfoPtr        - next hop info
*       P_CALLING_FORMAL_DATA - general debug information (environment variables
*                               etc); could be empty
*
* OUTPUTS:
*       isIpV4PrefixValidPtr  - ip v4 prefix validity property
*       errorMessagePtr       - error message
*
*
* RETURNS:
*       execution status
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvCpssDxChIpV4PrefixValidityCheck
(
    IN  CPSS_DXCH_IP_TCAM_ROUTE_ENTRY_INFO_UNT
                                *nextHopInfoPtr,
    OUT GT_BOOL                 *isIpV4PrefixValidPtr,
    OUT GT_CHAR_PTR             *errorMessagePtr
    P_CALLING_FORMAL_DATA
)
{
    GT_U32                  ipUcRouteEntryIndex = 0;
    GT_STATUS               status              = GT_OK;

    ipUcRouteEntryIndex = nextHopInfoPtr->ipLttEntry.routeEntryBaseIndex;

    status = prvCpssDxChIpUcRouteEntryValidityCheckOnAllDevices(
                 ipUcRouteEntryIndex, isIpV4PrefixValidPtr, errorMessagePtr
                 P_CALLING_ACTUAL_DATA);

    return status;
}


/*******************************************************************************
* prvCpssDxChValidIpPrefixIpUcRouteEntryGet
*
* DESCRIPTION:
*       Function Relevant mode : All modes
*
*       Getting of ip prefix ip uc route entry index, if ip prefix is valid.
*
* APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* NOT APPLICABLE DEVICES:
*       None.
*
* INPUTS:
*       vrfId                     - a Virtual Router ID
*       ipProtocolStack           - ip protocol type
*       ipAddrPtr                 - ip v4 or v6 address
*       prefixLen                 - prefix length
*       P_CALLING_FORMAL_DATA - general debug information (environment variables
*                               etc); could be empty
*
* OUTPUTS:
*       isIpPrefixValidPtr        - ip prefix validity property
*       ipUcRouteEntryIndexPtr    - ip unicast route entry index
*       errorMessagePtr           - error message
*
*
* RETURNS:
*       execution status
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvCpssDxChValidIpPrefixIpUcRouteEntryGet
(
    IN  GT_U32                      vrfId,
    IN  CPSS_IP_PROTOCOL_STACK_ENT  ipProtocolStack,
    IN  GT_IP_ADDR_TYPE_UNT         *ipAddrPtr,
    IN  GT_U8                       prefixLen,
    OUT GT_BOOL                     *isIpPrefixValidPtr,
    OUT GT_U32                      *ipUcRouteEntryIndexPtr,
    OUT GT_CHAR_PTR                 *errorMessagePtr
    P_CALLING_FORMAL_DATA
)
{
    GT_STATUS                       status      = GT_OK;

    status = prvCpssDxChIpPrefixIpUcRouteEntryGet(vrfId, ipProtocolStack,
                                                  ipAddrPtr, prefixLen,
                                                  isIpPrefixValidPtr,
                                                  ipUcRouteEntryIndexPtr,
                                                  errorMessagePtr
                                                  P_CALLING_ACTUAL_DATA);
    if ((GT_OK != status) && (GT_NOT_FOUND != status))
    {
        *errorMessagePtr = ipPrefixSearchingErrorMessage;
    }

    if ((GT_OK == status) && (GT_TRUE == *isIpPrefixValidPtr))
    {
        status = prvCpssDxChIpUcRouteEntryValidityCheckOnAllDevices(
                     *ipUcRouteEntryIndexPtr, isIpPrefixValidPtr,
                     errorMessagePtr P_CALLING_ACTUAL_DATA);
    }

    return status;
}


/*******************************************************************************
* prvCpssDxChNextValidIpPrefixDataGet
*
* DESCRIPTION:
*       Function Relevant mode : All modes
*
*       Getting of next valid ip prefix its length and its ip unicats route
*       entry index.
*
* APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* NOT APPLICABLE DEVICES:
*       None.
*
* INPUTS:
*       ipProtocolStack               - ip protocol typ
*       ipAddrPtr                     - starting ip v4 or v6 address
*                                       (recomended 0.0.0.0 for v4 or 0::0
*                                       for v6)
*       vrId                          - virtual router ID
*       prefixLenPtr                  - starting prefix length (recomended 0)
*       P_CALLING_FORMAL_DATA         - general debug information (environment
*                                       variables etc); could be empty
*
* OUTPUTS:
*       ipAddrPtr                     - founded prefix ip v4 or v6 address
*       prefixLenPtr                  - founded prefix length
*       vrId                          - founded virtual router ID related to the prefix
*       isIpPrefixFoundPtr            - ip prefix founding property
*       ipUcRouteEntryIndexPtr        - ip unicast route entry index
*       errorMessagePtr               - error message
*
*
* RETURNS:
*       execution status
*
* COMMENTS:
*
*******************************************************************************/


GT_STATUS prvCpssDxChNextValidIpPrefixDataGet
(
    IN    CPSS_IP_PROTOCOL_STACK_ENT    ipProtocolStack,
    INOUT GT_IP_ADDR_TYPE_UNT           *ipAddrPtr,
    INOUT GT_U32_PTR                    prefixLenPtr,
    OUT   GT_BOOL                       *isIpPrefixFoundPtr,
    OUT   GT_U32_PTR                    ipUcRouteEntryIndexPtr,
    OUT   GT_CHAR_PTR                   *errorMessagePtr,
    INOUT GT_U32_PTR                    vrId
    P_CALLING_FORMAL_DATA
)
{
    CPSS_DXCH_IP_TCAM_ROUTE_ENTRY_INFO_UNT
                                        nextHopInfo;
    GT_BOOL                             isIpV4PrefixChecked = GT_FALSE;
    GT_U32                              tcamRowIndex        = 0;
    GT_U32                              tcamColumnIndex     = 0;
    GT_STATUS                           status              = GT_OK;

    *isIpPrefixFoundPtr = GT_TRUE;

    cpssOsMemSet(&nextHopInfo, 0, sizeof(nextHopInfo));

    while (*vrId < (GT_U32)MAX_VRID) {
        *isIpPrefixFoundPtr = GT_TRUE;
        cpssOsMemSet(&nextHopInfo, 0, sizeof(nextHopInfo));
        while ((GT_TRUE == *isIpPrefixFoundPtr) &&
               (GT_FALSE == isIpV4PrefixChecked))
        {
            switch (ipProtocolStack)
            {
            case CPSS_IP_PROTOCOL_IPV4_E:
                CPSS_API_CALL_LOCK();
                status = cpssDxChIpLpmIpv4UcPrefixGetNext(0, *vrId,
                                                          &(ipAddrPtr->ipv4Addr),
                                                          prefixLenPtr,
                                                          &nextHopInfo,
                                                          &tcamRowIndex,
                                                          &tcamColumnIndex);
                CPSS_API_CALL_UNLOCK();
                P_CALLING_API(
                    cpssDxChIpLpmIpv4UcPrefixGetNext,
                    PC_NUMBER(0,                        lpmDBId,            GT_U32,
                    PC_NUMBER(*vrId,                        vrId,               GT_U32,
                    PC_STRUCT(&(ipAddrPtr->ipv4Addr),   ipAddr,          GT_IPADDR,
                    PC_NUMBER(*prefixLenPtr,            prefixLen,          GT_U32,
                    PC_STRUCT(&nextHopInfo,             nextHopInfo,
                                            CPSS_DXCH_IP_TCAM_ROUTE_ENTRY_INFO_UNT,
                    PC_NUMBER(tcamRowIndex,             tcamRowIndex,       GT_U32,
                    PC_NUMBER(tcamColumnIndex,          tcamColumnIndex,    GT_U32,
                    PC_LAST_PARAM))))))),
                    PC_STATUS);
                break;

            case CPSS_IP_PROTOCOL_IPV6_E:
                CPSS_API_CALL_LOCK();
                status = cpssDxChIpLpmIpv6UcPrefixGetNext(0, *vrId,
                                                          &(ipAddrPtr->ipv6Addr),
                                                          prefixLenPtr,
                                                          &nextHopInfo,
                                                          &tcamRowIndex,
                                                          &tcamColumnIndex);
                CPSS_API_CALL_UNLOCK();
                P_CALLING_API(
                    cpssDxChIpLpmIpv6UcPrefixGetNext,
                    PC_NUMBER(0,                        lpmDBId,            GT_U32,
                    PC_NUMBER(*vrId,                        vrId,               GT_U32,
                    PC_STRUCT(&(ipAddrPtr->ipv6Addr),   ipAddr,        GT_IPV6ADDR,
                    PC_NUMBER(*prefixLenPtr,            prefixLen,          GT_U32,
                    PC_STRUCT(&nextHopInfo,             nextHopInfo,
                                            CPSS_DXCH_IP_TCAM_ROUTE_ENTRY_INFO_UNT,
                    PC_NUMBER(tcamRowIndex,             tcamRowIndex,       GT_U32,
                    PC_NUMBER(tcamColumnIndex,          tcamColumnIndex,    GT_U32,
                    PC_LAST_PARAM))))))),
                    PC_STATUS);
                break;

            default:
                status              = GT_BAD_PARAM;
                *errorMessagePtr    = wrongIpProtocolDefinitionErrorMessage;
                break;
            }

            if ((GT_OK == status) || (GT_NOT_FOUND == status))
            {
                *isIpPrefixFoundPtr   = GT_NOT_FOUND != status;
                *ipUcRouteEntryIndexPtr =
                    nextHopInfo.ipLttEntry.routeEntryBaseIndex;
                status                  = GT_OK;
            }

            if ((GT_OK == status) && (GT_TRUE == *isIpPrefixFoundPtr))
            {
                status = prvCpssDxChIpV4PrefixValidityCheck(&nextHopInfo,
                                                            &isIpV4PrefixChecked,
                                                            errorMessagePtr
                                                            P_CALLING_ACTUAL_DATA);
            }

        }
        if (GT_TRUE == *isIpPrefixFoundPtr) {
            break;
        }
        else {
            *vrId = *vrId + 1;
            cpssOsMemSet(ipAddrPtr, 0, sizeof(GT_IP_ADDR_TYPE_UNT));
        }
    }

    return status;
}


/*******************************************************************************
* wrlCpssDxChIsRoutingPolicyBased
*
* DESCRIPTION:
*       Checks If Policy based routing.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*        L                   - lua state (lpmDBId pushed to lua stack)
*
* OUTPUTS:
*
* RETURNS:
*       1; GT_OK and BOOLEAN isPolicyBased
*       2; error code and error message, if error occures
*
* COMMENTS:
*
*******************************************************************************/
int wrlCpssDxChIsRoutingPolicyBased
(
    IN lua_State            *L
)
{
    GT_U32                  lpmDBId                 = (GT_U32) lua_tonumber(L, 1);
    GT_STATUS               status                  = GT_OK;
    int                     returned_param_count    = 0;
    GT_BOOL                 isPolicyBased           = GT_FALSE;

    status = prvCpssDxChIsRoutingPolicyBased(
        lpmDBId, &isPolicyBased);

    returned_param_count    +=
        prvWrlCpssStatusToLuaPush(status, L, "cpssDxChIpLpmDBConfigGet failed");
    returned_param_count    +=
        pvrCpssBooleanToLuaPush(status, L, isPolicyBased);
    return returned_param_count;
}

/*******************************************************************************
* wrlCpssDxChMaximumRouterArpIndex
*
* DESCRIPTION:
*       Function Relevant mode : All modes
*
*       Getting of maximum router arp index.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       L                     - lua state
*
* OUTPUTS:
*       2; GT_OK and maximum router arp index are pused to lua stack if no
*       errors occurs and such port exists
*       2; error code and error message, if error occures
*
* RETURNS:
*
* COMMENTS:
*
*******************************************************************************/
int wrlCpssDxChMaximumRouterArpIndex
(
    IN lua_State                *L
)
{
    GT_U8                       devNum                  =
                                                    (GT_U8)  lua_tonumber(L, 1);
                                                                    /* devId */
    GT_U32                      maximumRouterArpIndex
                                                        = 0;
    GT_STATUS                   status                  = GT_OK;
    GT_CHAR_PTR                 error_message           = NULL;
    int                         returned_param_count    = 0;

    status = prvCpssDxChMaximumRouterArpIndex(devNum, &maximumRouterArpIndex,
                                                  &error_message);

    returned_param_count    +=
        prvWrlCpssStatusToLuaPush(status, L, error_message);
    returned_param_count    +=
        pvrCpssNumberToLuaPush(status, L, maximumRouterArpIndex);
    return returned_param_count;
}


/*******************************************************************************
* wrlCpssDxChMaximumRouterNextHopEntryIndex
*
* DESCRIPTION:
*       Function Relevant mode : All modes
*
*       Getting of maximum router next hop entry index (wrapper).
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       L                     - lua state
*
* OUTPUTS:
*       2; GT_OK and maximum router arp index are pused to lua stack if no
*       errors occurs and such port exists
*       2; error code and error message, if error occures
*
* RETURNS:
*
* COMMENTS:
*
*******************************************************************************/
int wrlCpssDxChMaximumRouterNextHopEntryIndex
(
    IN lua_State                *L
)
{
    GT_U8                       devNum          = (GT_U8)  lua_tonumber(L, 1);
                                                                    /* devId */
    GT_U32                      maximumRouterNextHopEntryIndex
                                                = 0;
    GT_STATUS                   status          = GT_OK;
    GT_CHAR_PTR                 error_message   = NULL;
    int                         returned_param_count    = 0;

    status = prvCpssDxChMaximumRouterNextHopEntryIndex(
                 devNum, &maximumRouterNextHopEntryIndex, &error_message);

    returned_param_count    +=
        prvWrlCpssStatusToLuaPush(status, L, error_message);
    returned_param_count    +=
        pvrCpssNumberToLuaPush(status, L, maximumRouterNextHopEntryIndex);
    return returned_param_count;
}


/*******************************************************************************
* wrlCpssDxChRouterArpEntryIfValidGet
*
* DESCRIPTION:
*       Function Relevant mode : All modes
*
*       Getting of router arp entry, if it is valid.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*        L                   - lua state (device number and entry index should
*                              be pushed to lua stack)
*
* OUTPUTS:
*
* RETURNS:
*       2; GT_OK and router arp entry is pused to lua stack if no errors
*       occurs and this entry is valid
*       1; GT_OK is pused to lua stack if no errors occurs and valid router arp
*       entry is not exists
*       2; error code and error message, if error occures
*
* COMMENTS:
*
*******************************************************************************/
int wrlCpssDxChRouterArpEntryIfValidGet
(
    IN lua_State            *L
)
{
    GT_U8                               devNum                  =
                                                    (GT_U8)  lua_tonumber(L, 1);
                                                                    /* devId */
    GT_U32                              routerArpEntryIndex     =
                                                    (GT_U32) lua_tonumber(L, 2);
                                                        /* arp entry index */
    GT_BOOL                             isValidRouterArpEntry   = GT_TRUE;
    GT_ETHERADDR                        arpMacAddr;
    GT_STATUS                           status                  = GT_OK;
    GT_CHAR_PTR                         error_message           = NULL;
    int                                 returned_param_count    = 0;
    P_CALLING_CHECKING;

    cpssOsMemSet(&arpMacAddr, 0, sizeof(arpMacAddr));

    status = prvCpssDxChRouterArpEntryGetAndItsValidityCheck
                 (devNum, routerArpEntryIndex, GT_FALSE, &isValidRouterArpEntry,
                  &arpMacAddr, &error_message P_CALLING_ACTUAL_DATA);

    returned_param_count    +=
        prvWrlCpssStatusToLuaPush(status, L, error_message);
    returned_param_count    +=
        pvrCpssNumberToLuaOnConditionPush(status, L, routerArpEntryIndex,
                                          isValidRouterArpEntry);
    returned_param_count    +=
        pvrCpssMacAddrToLuaOnConditionPush(status, L, &arpMacAddr,
                                           isValidRouterArpEntry);
    return returned_param_count;
}


/*******************************************************************************
* wrlCpssDxChNextRouterArpEntryGet
*
* DESCRIPTION:
*       Function Relevant mode : All modes
*
*       Getting of next arp entry.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*        L                   - lua state (device number and entry index should
*                              be pushed to lua stack)
*
* OUTPUTS:
*
* RETURNS:
*       2; GT_OK and next router arp entry is pused to lua stack if no errors
*       occurs and such entry exists
*       1; GT_OK is pused to lua stack if no errors occurs and such entry is not
*       exists
*       2; error code and error message, if error occures
*
* COMMENTS:
*
*******************************************************************************/
int wrlCpssDxChNextRouterArpEntryGet
(
    IN lua_State            *L
)
{
    GT_U8                               devNum                  =
                                                    (GT_U8)  lua_tonumber(L, 1);
                                                                    /* devId */
    GT_U32                              routerArpEntryIndex     =
                                                    (GT_U32) lua_tonumber(L, 2);
                                                        /* arp entry index */
    GT_U32                              maximumRouterArpIndex   = 0;
    GT_BOOL                             routerArpEntryWasFound  = GT_FALSE;
    GT_ETHERADDR                        arpMacAddr;
    GT_STATUS                           status                  = GT_OK;
    GT_CHAR_PTR                         error_message           = NULL;
    int                                 returned_param_count    = 0;
    P_CALLING_CHECKING;

    cpssOsMemSet(&arpMacAddr, 0, sizeof(arpMacAddr));

    status = prvCpssDxChMaximumRouterArpIndex(devNum, &maximumRouterArpIndex,
                                                  &error_message);

    while ((GT_OK == status) && (GT_FALSE == routerArpEntryWasFound) &&
           (routerArpEntryIndex < maximumRouterArpIndex))
    {
        status = prvCpssDxChRouterArpEntryGetAndItsValidityCheck(
                     devNum, routerArpEntryIndex++, GT_FALSE,
                     &routerArpEntryWasFound, &arpMacAddr, &error_message
                     P_CALLING_ACTUAL_DATA);
    }

    if (GT_TRUE == routerArpEntryWasFound)
    {
        routerArpEntryIndex--;
    }

    returned_param_count    +=
        prvWrlCpssStatusToLuaPush(status, L, error_message);
    returned_param_count    +=
        pvrCpssNumberToLuaOnConditionPush(status, L, routerArpEntryIndex,
                                          routerArpEntryWasFound);
    returned_param_count    +=
        pvrCpssMacAddrToLuaOnConditionPush(status, L, &arpMacAddr,
                                           routerArpEntryWasFound);
    return returned_param_count;
}


/*******************************************************************************
* wrlCpssDxChRouterArpEntryIndexGet
*
* DESCRIPTION:
*       Function Relevant mode : All modes
*
*       Getting of next arp entry index with same mac address.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*        L                   - lua state (device number, start entry index and
*                              mac-address should be pushed to lua stack)
*
* OUTPUTS:
*
* RETURNS:
*       2; GT_OK and founded router arp entry index is pused to lua stack if no
*       errors occurs and such entry exists
*       1; GT_OK is pused to lua stack if no errors occurs and such entry is not
*       exists
*       2; error code and error message, if error occures
*
* COMMENTS:
*
*******************************************************************************/
int wrlCpssDxChRouterArpEntryIndexGet
(
    IN lua_State            *L
)
{
    GT_U8                   devNum                          =
                                                    (GT_U8)  lua_tonumber(L, 1);
                                                                    /* devId */
    GT_U32                  routerArpEntryIndex             =
                                                    (GT_U32) lua_tonumber(L, 2);
                                                        /* arp entry index */
    GT_U32                  maximumRouterArpIndex           = 0;
    GT_BOOL                 routerArpEntryWasFound          = GT_FALSE;
    GT_ETHERADDR            etalonMacAddr, checkedArpMacAddr;
    GT_STATUS               status                          = GT_OK;
    GT_CHAR_PTR             error_message                   = NULL;
    int                     returned_param_count            = 0;
    P_CALLING_CHECKING;

    status = pvrCpssMacAddrFromLuaGet(L, 3, &etalonMacAddr, &error_message);

    cpssOsMemSet(&checkedArpMacAddr, 0, sizeof(checkedArpMacAddr));

    if (GT_OK == status)
    {
        status = prvCpssDxChMaximumRouterArpIndex(devNum,
                                                  &maximumRouterArpIndex,
                                                  &error_message);
    }

    while ((GT_OK == status) && (GT_FALSE == routerArpEntryWasFound) &&
           (routerArpEntryIndex < maximumRouterArpIndex))
    {
        status = prvCpssDxChRouterArpEntryGetAndItsValidityCheck(
                     devNum, routerArpEntryIndex++, GT_TRUE,
                     &routerArpEntryWasFound, &checkedArpMacAddr,
                     &error_message P_CALLING_ACTUAL_DATA);

        if ((GT_OK == status) && (GT_TRUE == routerArpEntryWasFound))
        {
            status = prvCpssIsSameMacAddresses(&etalonMacAddr,
                                               &checkedArpMacAddr,
                                               &routerArpEntryWasFound,
                                               &error_message);
        }
    }

    if (GT_TRUE == routerArpEntryWasFound)
    {
        routerArpEntryIndex--;
    }

    returned_param_count    +=
        prvWrlCpssStatusToLuaPush(status, L, error_message);
    returned_param_count    +=
        pvrCpssNumberToLuaOnConditionPush(status, L, routerArpEntryIndex,
                                          routerArpEntryWasFound);
    return returned_param_count;
}


/*******************************************************************************
* wrlCpssDxChNextInvalidRouterArpEntryGet
*
* DESCRIPTION:
*       Function Relevant mode : All modes
*
*       Getting of next invalid arp entry. Valid enties could be assigned as
*       invalid on overruleUnusedInLuaArpEntries property.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*        L                   - lua state (device number and entry index should
*                              be pushed to lua stack)
*
* OUTPUTS:
*
* RETURNS:
*       2; GT_OK and next router arp entry is pused to lua stack if no errors
*       occurs and such entry exists
*       1; GT_OK is pused to lua stack if no errors occurs and such entry is not
*       exists
*       2; error code and error message, if error occures
*
* COMMENTS:
*
*******************************************************************************/
int wrlCpssDxChNextInvalidRouterArpEntryGet
(
    IN lua_State            *L
)
{
    GT_U8                   devNum                          =
                                                    (GT_U8)  lua_tonumber(L, 1);
                                                                    /* devId */
    GT_U32                  routerArpEntryIndex             =
                                                    (GT_U32) lua_tonumber(L, 2);
                                                        /* arp entry index */
                                /* considering of unused entries as invalid */
    GT_U32                  maximumRouterArpIndex           = 0;
    GT_BOOL                 routerArpEntryValidity          = GT_TRUE;
    GT_BOOL                 invalidRouterArpEntryWasFound
                                                         = GT_FALSE;
    GT_ETHERADDR            arpMacAddr;
    GT_STATUS               status                          = GT_OK;
    GT_CHAR_PTR             error_message                   = NULL;
    int                     returned_param_count            = 0;
    P_CALLING_CHECKING;

    cpssOsMemSet(&arpMacAddr, 0, sizeof(arpMacAddr));

    status = prvCpssDxChMaximumRouterArpIndex(devNum, &maximumRouterArpIndex,
                                              &error_message);

    while ((GT_OK == status) && (GT_FALSE == invalidRouterArpEntryWasFound) &&
           (routerArpEntryIndex < maximumRouterArpIndex))
    {
        status = prvCpssDxChRouterArpEntryGetAndItsValidityCheck(
                     devNum, routerArpEntryIndex++, GT_FALSE,
                     &routerArpEntryValidity, &arpMacAddr, &error_message
                     P_CALLING_ACTUAL_DATA);

        if (GT_OK == status)
        {
            invalidRouterArpEntryWasFound = GT_FALSE == routerArpEntryValidity;
        }
    }

    if ((GT_OK == status) && (GT_TRUE == invalidRouterArpEntryWasFound))
    {
        routerArpEntryIndex--;
    }

    returned_param_count    +=
        prvWrlCpssStatusToLuaPush(status, L, error_message);
    returned_param_count    +=
        pvrCpssNumberToLuaOnConditionPush(status, L, routerArpEntryIndex,
                                          invalidRouterArpEntryWasFound);
    return returned_param_count;
}


/*******************************************************************************
* wrlCpssDxChIpUcRouteEntryRead
*
* DESCRIPTION:
*       Function Relevant mode : All modes
*
*       Reading of ip unicast entry.
*
* APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* NOT APPLICABLE DEVICES:
*       None.
*
* INPUTS:
*       L                 - lua state (ip unicast entry index pushed to lua
*                           stack)
*
* OUTPUTS:
*
* RETURNS:
*       2; GT_OK and ip unicast entry is pused to lua stack if no errors
*       occurs
*       2; error code and error message, if error occures
*
* COMMENTS:
*
*******************************************************************************/
int wrlCpssDxChIpUcRouteEntryRead
(
    IN lua_State            *L
)
{
    GT_U8                   devNum                  =
                                                    (GT_U8)  lua_tonumber(L, 1);
                                                                    /* devId */
    GT_U32                  routeEntryIndex         =
                                                    (GT_U32) lua_tonumber(L, 2);
                                                        /* route entry index */
    GT_BOOL                 routeEntryTypeTaken
                                                    = GT_FALSE;
    CPSS_DXCH_IP_UC_ROUTE_ENTRY_STC
                            routeEntry;
    GT_STATUS               status                  = GT_OK;
    GT_CHAR_PTR             error_message           = NULL;
	GT_U32                  tempVal;
    int                     returned_param_count    = 0;
    P_CALLING_CHECKING;

    status = pvrCpssNumberFromLuaGet(L, 3, &routeEntryTypeTaken,
                                     &tempVal,
                                     &error_message);

    if ((GT_OK == status) && (GT_FALSE == routeEntryTypeTaken))
    {
        routeEntry.type = CPSS_DXCH_IP_UC_ROUTE_ENTRY_E;
    }

    if (GT_OK == status)
    {
        CPSS_API_CALL_LOCK();
        status = cpssDxChIpUcRouteEntriesRead(devNum, routeEntryIndex,
                                              &routeEntry, 1);
        CPSS_API_CALL_UNLOCK();
        P_CALLING_API(
            cpssDxChIpUcRouteEntriesRead,
            PC_NUMBER(devNum,           devNum,                         GT_U8,
            PC_NUMBER(routeEntryIndex,  baseRouteEntryIndex,           GT_U32,
            PC_STRUCT(&routeEntry,      routeEntriesArray,
                                              CPSS_DXCH_IP_UC_ROUTE_ENTRY_STC,
            PC_NUMBER(1,                numOfRouteEntries,
                                                                       GT_U32,
            PC_LAST_PARAM)))),
            PC_STATUS);
        if (GT_OK != status)
        {
            error_message = ipUcRouteEntryReadErrorMessage;
        }
    }

    returned_param_count    +=
        prvWrlCpssStatusToLuaPush(status, L, error_message);
    returned_param_count    +=
        pvrCpssDxChIpUnicastEntryOnConditionPush(status, L, &routeEntry,
                                                 GT_TRUE);
    return returned_param_count;
}


/*******************************************************************************
* wrlCpssDxChIpUcRouteEntryWrite
*
* DESCRIPTION:
*       Function Relevant mode : All modes
*
*       Writing of ip unicast entry.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*        L                - lua state (device number and route entry index
*                           pushed to lua stack)
*
* OUTPUTS:
*
* RETURNS:
*       1; GT_OK if operation succed
*       2; error code and error message, if error occures
*
* COMMENTS:
*
*******************************************************************************/
int wrlCpssDxChIpUcRouteEntryWrite
(
    IN lua_State            *L
)
{
    GT_U8                   devNum          = (GT_U8)  lua_tonumber(L, 1);
                                                                    /* devId */
    GT_U32                  routeEntryIndex = (GT_U32) lua_tonumber(L, 2);
                                                        /* route entry index */
    GT_BOOL                 isRouteEntryTaken
                                            = GT_FALSE;
    CPSS_DXCH_IP_UC_ROUTE_ENTRY_STC
                            routeEntry;
    GT_STATUS               status          = GT_OK;
    GT_CHAR_PTR             error_message   = NULL;
    GT_U32                  tmpHWDevNum;
    P_CALLING_CHECKING;

    status = pvrCpssDxChIpUnicastEntryFromLuaGet(L, 3, &isRouteEntryTaken,
                                                 &routeEntry, &error_message);

    tmpHWDevNum = PRV_CPSS_HW_DEV_NUM_MAC(devNum);
    routeEntry.entry.regularEntry.nextHopInterface.devPort.hwDevNum = tmpHWDevNum;
    if (GT_FALSE == isRouteEntryTaken)
    {
        error_message   = ipUcRouteEntryFromLuaGettingErrorMessage;
    }

    if (GT_OK == status)
    {
        CPSS_API_CALL_LOCK();
        status = cpssDxChIpUcRouteEntriesWrite(devNum, routeEntryIndex,
                                               &routeEntry, 1);
        CPSS_API_CALL_UNLOCK();
        P_CALLING_API(
            cpssDxChIpUcRouteEntriesWrite,
            PC_NUMBER(devNum,           devNum,                          GT_U8,
            PC_NUMBER(routeEntryIndex,  baseRouteEntryIndex,            GT_U32,
            PC_STRUCT(&routeEntry,      routeEntriesArray,
                                               CPSS_DXCH_IP_UC_ROUTE_ENTRY_STC,
            PC_NUMBER(1,                numOfRouteEntries,              GT_U32,
            PC_LAST_PARAM)))),
            PC_STATUS);
        if (GT_OK != status)
        {
            error_message = ipUcRouteEntryWriteErrorMessage;
        }
    }

    return prvWrlCpssStatusToLuaPush(status, L, error_message);
}


/*******************************************************************************
* wrlCpssDxChIpUcRouteEntryValidityCheck
*
* DESCRIPTION:
*       Function Relevant mode : All modes
*
*       Ip unicast route entry validity check under all devices.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       L                 - lua state (ip unicast entry index pushed to lua
*                           stack)
*
* OUTPUTS:
*
* RETURNS:
*       2; GT_OK and ip unicast entry validity is pused to lua stack if no
*       errors occurs
*       2; error code and error message, if error occures
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS wrlCpssDxChIpUcRouteEntryValidityCheck
(
    IN lua_State            *L
)
{
    GT_U32                  ipUcRouteEntryIndex     =
                                                    (GT_U32) lua_tonumber(L, 1);
                                                /* ip uc route entry index */
    GT_BOOL                 isValidIpUcRouteEntry   = GT_FALSE;
    GT_STATUS               status                  = GT_OK;
    GT_CHAR_PTR             error_message           = NULL;
    int                     returned_param_count    = 0;
    P_CALLING_CHECKING;

    status = prvCpssDxChIpUcRouteEntryValidityCheckOnAllDevices(
                 ipUcRouteEntryIndex, &isValidIpUcRouteEntry, &error_message
                 P_CALLING_ACTUAL_DATA);

    returned_param_count    +=
        prvWrlCpssStatusToLuaPush(status, L, error_message);
    returned_param_count    +=
        pvrCpssBooleanToLuaPush(status, L, isValidIpUcRouteEntry);
    return returned_param_count;
}


/*******************************************************************************
* wrlCpssDxChNextIpUcRouteEntryGet
*
* DESCRIPTION:
*       Function Relevant mode : All modes
*
*       Getting of next arp entry.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*        L                   - lua state (device number and entry index should
*                              be pushed to lua stack)
*
* OUTPUTS:
*
* RETURNS:
*       2; GT_OK and next ip unicast route entry is pused to lua stack if no
*       errors occurs and such entry exists
*       1; GT_OK is pused to lua stack if no errors occurs and such entry is not
*       exists
*       2; error code and error message, if error occures
*
* COMMENTS:
*
*******************************************************************************/
int wrlCpssDxChNextIpUcRouteEntryGet
(
    IN lua_State            *L
)
{
    GT_U8                               devNum                  =
                                                    (GT_U8)  lua_tonumber(L, 1);
                                                                    /* devId */
    GT_U32                              ipUcRouteEntryIndex     =
                                                    (GT_U32) lua_tonumber(L, 2);
                                                        /* arp entry index */
    GT_U32                              maximumRouterArpIndex   = 0;
    GT_BOOL                             ipUcRouteEntryWasFound  = GT_FALSE;
    CPSS_DXCH_IP_UC_ROUTE_ENTRY_STC     ipUcRouteEntry;
    GT_STATUS                           status                  = GT_OK;
    GT_CHAR_PTR                         error_message           = NULL;
    int                                 returned_param_count    = 0;
    P_CALLING_CHECKING;

    cpssOsMemSet(&ipUcRouteEntry, 0, sizeof(ipUcRouteEntry));

    status = prvCpssDxChMaximumRouterNextHopEntryIndex(devNum, &maximumRouterArpIndex,
                                              &error_message);

    while ((GT_OK == status) && (GT_FALSE == ipUcRouteEntryWasFound) &&
           (ipUcRouteEntryIndex < maximumRouterArpIndex))
    {
        status = prvCpssDxChIpUcRouteEntryGetAndItsValidityCheck(
                     devNum, ipUcRouteEntryIndex++, &ipUcRouteEntryWasFound,
                     &ipUcRouteEntry, &error_message P_CALLING_ACTUAL_DATA);
    }

    if (GT_TRUE == ipUcRouteEntryWasFound)
    {
        ipUcRouteEntryIndex--;
    }

    returned_param_count    +=
        prvWrlCpssStatusToLuaPush(status, L, error_message);
    returned_param_count    +=
        pvrCpssNumberToLuaOnConditionPush(status, L, ipUcRouteEntryIndex,
                                          ipUcRouteEntryWasFound);
    returned_param_count    +=
        pvrCpssDxChIpUnicastEntryOnConditionPush(status, L, &ipUcRouteEntry,
                                                 ipUcRouteEntryWasFound);
    return returned_param_count;
}


/*******************************************************************************
* wrlCpssDxChIpLpmIpUcPrefixExist
*
* DESCRIPTION:
*       Function Relevant mode : All modes
*
*       Checks if ip v4 or v6 prefix exists.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*        L                   - lua state (vrfId, ip protocol, ip address and
*                              its length pushed to lua stack)
*
* OUTPUTS:
*
* RETURNS:
*       2; GT_OK and ip unicast entry validity is pused to lua stack if no
*       errors occurs
*       2; error code and error message, if error occures
*
* COMMENTS:
*
*******************************************************************************/
int wrlCpssDxChIpLpmIpUcPrefixExist
(
    IN lua_State            *L
)
{
    CPSS_IP_PROTOCOL_STACK_ENT  ipProtocolStack = CPSS_IP_PROTOCOL_IPV4_E;
    GT_IP_ADDR_TYPE_UNT     ipAddress;
    GT_U32                  vrfId               = (GT_U32) lua_tonumber(L, 1);
    GT_U32                  prefixLen           = (GT_U8)  lua_tonumber(L, 4);
                                                           /* prefix length */
    GT_BOOL                 isIpV4PrefixExist       = GT_FALSE;
    GT_U32                  ipUcRouteEntryIndex     = 0;
    GT_STATUS               status                  = GT_OK;
    GT_CHAR_PTR             error_message           = NULL;
    int                     returned_param_count    = 0;
    P_CALLING_CHECKING;

    status = pvrCpssIpProtocolFromLuaGet(L, 2, &ipProtocolStack,
                                         &error_message);
    if (GT_OK == status)
    {
        status = pvrCpssIpAddrFromLuaGet(L, 3, ipProtocolStack, &ipAddress,
                                         &error_message);
    }
    else
    {
        cpssOsMemSet(&ipAddress, 0, sizeof(ipAddress));
    }

    if (GT_OK == status)
    {
        status = prvCpssDxChIpPrefixIpUcRouteEntryGet(vrfId,
                                                      ipProtocolStack,
                                                      &ipAddress, prefixLen,
                                                      &isIpV4PrefixExist,
                                                      &ipUcRouteEntryIndex,
                                                      &error_message
                                                      P_CALLING_ACTUAL_DATA);
    }

    returned_param_count    +=
        prvWrlCpssStatusToLuaPush(status, L, error_message);
    returned_param_count    +=
        pvrCpssBooleanToLuaPush(status, L, isIpV4PrefixExist);
    return returned_param_count;
}


/*******************************************************************************
* wrlCpssDxChNextValidIpPrefixDataGet
*
* DESCRIPTION:
*       Function Relevant mode : All modes
*
*       Getting of next valid ip v4 or v6 prefix its length and its ip unicats
*       route entry index.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*        L                   - lua state (ip protocol, prefix ip address and
*                              prefix length should be pushed to lua stack)
*
* OUTPUTS:
*
* RETURNS:
*       2; GT_OK and prefix ip address, prefix length, prefix next-hop entry
*       index
*       1; GT_OK if next valid ip prefix not found and no error occures
*       2; error code and error message, if error occures
*
* COMMENTS:
*
*******************************************************************************/
int wrlCpssDxChNextValidIpPrefixDataGet
(
    IN lua_State            *L
)
{
    CPSS_IP_PROTOCOL_STACK_ENT  ipProtocolStack         =
                                                        CPSS_IP_PROTOCOL_IPV4_E;
    GT_IP_ADDR_TYPE_UNT         ipAddress;
    GT_U32                      prefixLen               =
                                                    (GT_U32) lua_tonumber(L, 3);
                                                            /* prefix length */
    GT_BOOL                     isIpV4PrefixFound       = GT_FALSE;
    GT_U32                      ipUcRouteEntryIndex     = 0;
    GT_STATUS                   status                  = GT_OK;
    GT_CHAR_PTR                 error_message           = NULL;
    GT_U32                      vrId                    = (GT_U32) lua_tonumber(L, 4);
    int                         returned_param_count    = 0;
    P_CALLING_CHECKING;

    status = pvrCpssIpProtocolFromLuaGet(L, 1, &ipProtocolStack,
                                         &error_message);

    if (GT_OK == status)
    {
        status = pvrCpssIpAddrFromLuaGet(L, 2, ipProtocolStack, &ipAddress,
                                         &error_message);
    }
    else
    {
        cpssOsMemSet(&ipAddress, 0, sizeof(ipAddress));
    }

    if (GT_OK == status)
    {
        status = prvCpssDxChNextValidIpPrefixDataGet(ipProtocolStack, &ipAddress,
                                                     &prefixLen,
                                                     &isIpV4PrefixFound,
                                                     &ipUcRouteEntryIndex,
                                                     &error_message,
                                                     &vrId
                                                     P_CALLING_ACTUAL_DATA);
    }

    returned_param_count    +=
        prvWrlCpssStatusToLuaPush(status, L, error_message);

    returned_param_count    +=
        pvrCpssIpAddrToLuaOnConditionPush(status, L, ipProtocolStack,
                                          &ipAddress, isIpV4PrefixFound);
    returned_param_count    +=
        pvrCpssNumberToLuaOnConditionPush(status, L, prefixLen,
                                          isIpV4PrefixFound);
    returned_param_count    +=
        pvrCpssNumberToLuaOnConditionPush(status, L, ipUcRouteEntryIndex,
                                          isIpV4PrefixFound);
    returned_param_count    +=
        pvrCpssNumberToLuaOnConditionPush(status, L, vrId,
                                          isIpV4PrefixFound);
    return returned_param_count;
}


/*******************************************************************************
* wrlCpssDxChIpLpmIpUcPrefixAdd
*
* DESCRIPTION:
*       Function Relevant mode : All modes
*
*       Creates a new or override an existing ip v4 or v6 prefix in specified
*       Virtual Router for the specified LPM DB.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*        L                   - lua state (vrf-id, ip protocol, prefix ip address
*                              and prefix length should be pushed to lua stack)
*
* OUTPUTS:
*
* RETURNS:
*       1; GT_OK if operation succed
*       2; error code and error message, if error occures
*
* COMMENTS:
*
*******************************************************************************/
int wrlCpssDxChIpLpmIpUcPrefixAdd
(
    IN lua_State                *L
)
{
    CPSS_IP_PROTOCOL_STACK_ENT  ipProtocolStack         =
                                                        CPSS_IP_PROTOCOL_IPV4_E;
    GT_IP_ADDR_TYPE_UNT         ipAddress;
    GT_U32                      vrfId                   =
                                                    (GT_U32) lua_tonumber(L, 1);
    GT_U32                      prefixLen               =
                                                    (GT_U8)  lua_tonumber(L, 4);
                                                            /* prefix length */
    GT_BOOL                     isValidIpUcRouteEntry   = GT_TRUE;
    GT_U32                      ipUcRouteEntryIndex     =
                                                   (GT_U32)  lua_tonumber(L, 5);
                                                            /* prefix length */
    GT_BOOL                     isEArch              =
                                                    (GT_BOOL) lua_toboolean(L,6);
                                                        /* true if the device is bc2 */
    CPSS_DXCH_IP_TCAM_ROUTE_ENTRY_INFO_UNT
                                nextHopEntry;
    GT_STATUS                   status                  = GT_OK;
    GT_CHAR_PTR                 error_message           = NULL;
    P_CALLING_CHECKING;

    cpssOsMemSet(&nextHopEntry, 0, sizeof(nextHopEntry));

    status = pvrCpssIpProtocolFromLuaGet(L, 2, &ipProtocolStack,
                                         &error_message);

    if (GT_OK == status)
    {
        status = pvrCpssIpAddrFromLuaGet(L, 3, ipProtocolStack, &ipAddress,
                                         &error_message);
    }
    else
    {
        cpssOsMemSet(&ipAddress, 0, sizeof(ipAddress));
    }

    if (GT_OK == status)
    {
        status = prvCpssDxChIpUcRouteEntryValidityCheckOnAllDevices(
                     ipUcRouteEntryIndex, &isValidIpUcRouteEntry,
                     &error_message P_CALLING_ACTUAL_DATA);
        if ((GT_OK == status) && (GT_FALSE == isValidIpUcRouteEntry))
        {
            status          = GT_BAD_PARAM;
            error_message = invalidIpUcRouteEntryErrorMessage;
        }
    }

    /* keep all value of ucNextHop.ipLttEntry = 0 set the rout index        */
    nextHopEntry.ipLttEntry.routeEntryBaseIndex = ipUcRouteEntryIndex;
    if (isEArch == GT_TRUE) {
        nextHopEntry.ipLttEntry.routeType = CPSS_DXCH_IP_ROUTE_ENTRY_METHOD_REGULAR_E;
    }


    if (GT_OK == status)
    {
        switch (ipProtocolStack)
        {
        case CPSS_IP_PROTOCOL_IPV4_E:
            CPSS_API_CALL_LOCK();
            status = cpssDxChIpLpmIpv4UcPrefixAdd(0, vrfId, ipAddress.ipv4Addr,
                                                  prefixLen, &nextHopEntry,
                                                  GT_TRUE);
            CPSS_API_CALL_UNLOCK();
            P_CALLING_API(
                cpssDxChIpLpmIpv4UcPrefixAdd,
                PC_NUMBER(0,                        lpmDBId,            GT_U32,
                PC_NUMBER(vrfId,                    vrId,               GT_U32,
                PC_STRUCT(&(ipAddress.ipv4Addr),    ipAddr,          GT_IPADDR,
                PC_NUMBER(prefixLen,                prefixLen,          GT_U32,
                PC_STRUCT(&nextHopEntry,            nextHopInfo,
                                        CPSS_DXCH_IP_TCAM_ROUTE_ENTRY_INFO_UNT,
                PC_BOOL  (GT_TRUE,                  override,
                PC_LAST_PARAM)))))),
                PC_STATUS);
            break;

        case CPSS_IP_PROTOCOL_IPV6_E:
            CPSS_API_CALL_LOCK();
            status = cpssDxChIpLpmIpv6UcPrefixAdd(0, vrfId, ipAddress.ipv6Addr,
                                                  prefixLen, &nextHopEntry,
                                                  GT_TRUE, GT_TRUE);
            CPSS_API_CALL_UNLOCK();
            P_CALLING_API(
                cpssDxChIpLpmIpv6UcPrefixAdd,
                PC_NUMBER(0,                        lpmDBId,            GT_U32,
                PC_NUMBER(vrfId,                    vrId,               GT_U32,
                PC_STRUCT(&(ipAddress.ipv6Addr),    ipAddr,        GT_IPV6ADDR,
                PC_NUMBER(prefixLen,                prefixLen,          GT_U32,
                PC_STRUCT(&nextHopEntry,            nextHopInfo,
                                        CPSS_DXCH_IP_TCAM_ROUTE_ENTRY_INFO_UNT,
                PC_BOOL  (GT_TRUE,                  override,
                PC_BOOL  (GT_TRUE,                  defragmentationEnable,
                PC_LAST_PARAM))))))),
                PC_STATUS);
            break;

        default:
            status              = GT_BAD_PARAM;
            error_message       = wrongIpProtocolDefinitionErrorMessage;
            break;
        }

        if (GT_OK != status)
        {
            error_message = ipPreffixAddingErrorMessage;
        }
    }

    return prvWrlCpssStatusToLuaPush(status, L, error_message);
}



/*******************************************************************************
* wrlCpssDxChIpLpmIpUcPrefixDel
*
* DESCRIPTION:
*       Function Relevant mode : All modes
*
*       Deletes an existing Ipv6 prefix in a Virtual Router for the specified
*       LPM DB.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*        L                   - lua state (vrfId, ip protocol, prefix ip address
*                              and prefix length should be pushed to lua stack)
*
* OUTPUTS:
*
* RETURNS:
*       1; GT_OK if operation succed
*       2; error code and error message, if error occures
*
* COMMENTS:
*
*******************************************************************************/
int wrlCpssDxChIpLpmIpUcPrefixDel
(
    IN lua_State                *L
)
{
    CPSS_IP_PROTOCOL_STACK_ENT  ipProtocolStack   =
                                                    CPSS_IP_PROTOCOL_IPV4_E;
    GT_IP_ADDR_TYPE_UNT         ipAddress;
    GT_U32                      vrfId            = (GT_U32) lua_tonumber(L, 1);
    GT_U32                      prefixLen         = (GT_U8)  lua_tonumber(L, 4);
                                                        /* prefix length */
    GT_STATUS                   status            = GT_OK;
    GT_CHAR_PTR                 error_message     = NULL;
    P_CALLING_CHECKING;

    status = pvrCpssIpProtocolFromLuaGet(L, 2, &ipProtocolStack,
                                         &error_message);

    if (GT_OK == status)
    {
        status = pvrCpssIpAddrFromLuaGet(L, 3, ipProtocolStack, &ipAddress,
                                         &error_message);
    }
    else
    {
        cpssOsMemSet(&ipAddress, 0, sizeof(ipAddress));
    }

    if (GT_OK == status)
    {
        switch (ipProtocolStack)
        {
        case CPSS_IP_PROTOCOL_IPV4_E:
            CPSS_API_CALL_LOCK();
            status = cpssDxChIpLpmIpv4UcPrefixDel(0, vrfId, ipAddress.ipv4Addr,
                                                  prefixLen);
            CPSS_API_CALL_UNLOCK();
            P_CALLING_API(
                cpssDxChIpLpmIpv4UcPrefixDel,
                PC_NUMBER(0,                        lpmDBId,            GT_U32,
                PC_NUMBER(vrfId,                    vrId,               GT_U32,
                PC_STRUCT(&(ipAddress.ipv4Addr),    ipAddr,          GT_IPADDR,
                PC_NUMBER(prefixLen,                prefixLen,          GT_U32,
                PC_LAST_PARAM)))),
                PC_STATUS);
            break;

        case CPSS_IP_PROTOCOL_IPV6_E:
            CPSS_API_CALL_LOCK();
            status = cpssDxChIpLpmIpv6UcPrefixDel(0, vrfId, ipAddress.ipv6Addr,
                                                  prefixLen);
            CPSS_API_CALL_UNLOCK();
            P_CALLING_API(
                cpssDxChIpLpmIpv6UcPrefixDel,
                PC_NUMBER(0,                        lpmDBId,            GT_U32,
                PC_NUMBER(vrfId,                    vrId,               GT_U32,
                PC_STRUCT(&(ipAddress.ipv6Addr),    ipAddr,        GT_IPV6ADDR,
                PC_NUMBER(prefixLen,                prefixLen,          GT_U32,
                PC_LAST_PARAM)))),
                PC_STATUS);
            break;

        default:
            status              = GT_BAD_PARAM;
            error_message       = wrongIpProtocolDefinitionErrorMessage;
            break;
        }

        if (GT_OK != status)
        {
            error_message = ipPreffixDeletingErrorMessage;
        }
    }

    return prvWrlCpssStatusToLuaPush(status, L, error_message);
}
