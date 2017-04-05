/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* prvCpssDxChLpmUtils.c
*
* DESCRIPTION:
*       private LPM utility functions
*
* FILE REVISION NUMBER:
*       $Revision: 5 $
*******************************************************************************/

#include <cpss/dxCh/dxChxGen/ipLpmEngine/cpssDxChIpLpmTypes.h>
#include <cpss/dxCh/dxChxGen/private/lpm/tcam/prvCpssDxChLpmTcam.h>
#include <cpss/dxCh/dxChxGen/private/lpm/ram/prvCpssDxChLpmRam.h>
#include <cpss/extServices/private/prvCpssBindFunc.h>

/*******************************************************************************
* prvCpssDxChLpmConvertIpProtocolStackToProtocolBitmap
*
* DESCRIPTION:
*       Convert IP protocol stack value to bitmap of protocols
*
* APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       None
*
* INPUTS:
*       protocolStack     - the IP protocol stack
*
* OUTPUTS:
*       protocolBitmapPtr - the protocol bitmap
*
* RETURNS:
*       None
*
* COMMENTS:
*       The function assumes that a validity check was done on protocolStack
*       before calling to this function.
*
*******************************************************************************/
GT_VOID prvCpssDxChLpmConvertIpProtocolStackToProtocolBitmap
(
    IN  CPSS_IP_PROTOCOL_STACK_ENT        protocolStack,
    OUT PRV_CPSS_DXCH_LPM_PROTOCOL_BMP    *protocolBitmapPtr
)
{
    if (protocolStack == CPSS_IP_PROTOCOL_IPV4_E)
    {
        *protocolBitmapPtr = PRV_CPSS_DXCH_LPM_PROTOCOL_IPV4_BIT_E;
    }
    else if (protocolStack == CPSS_IP_PROTOCOL_IPV6_E)
    {
        *protocolBitmapPtr = PRV_CPSS_DXCH_LPM_PROTOCOL_IPV6_BIT_E;
    }
    else   /* protocolStack == CPSS_IP_PROTOCOL_IPV4V6_E */
    {
        *protocolBitmapPtr = PRV_CPSS_DXCH_LPM_PROTOCOL_IPV4_BIT_E |
                             PRV_CPSS_DXCH_LPM_PROTOCOL_IPV6_BIT_E;
    }
}

/*******************************************************************************
* prvCpssDxChLpmConvertProtocolBitmapToIpProtocolStack
*
* DESCRIPTION:
*       Convert bitmap of protocols to IP protocol stack value
*
* APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       None
*
* INPUTS:
*       protocolBitmap   - the protocol bitmap
*
* OUTPUTS:
*       protocolStackPtr - the IP protocol stack
*
* RETURNS:
*       None
*
* COMMENTS:
*       The function assumes that protocolBitmap holds at least one IP protocol
*
*******************************************************************************/
GT_VOID prvCpssDxChLpmConvertProtocolBitmapToIpProtocolStack
(
    IN  PRV_CPSS_DXCH_LPM_PROTOCOL_BMP    protocolBitmap,
    OUT CPSS_IP_PROTOCOL_STACK_ENT        *protocolStackPtr
)
{
    if (PRV_CPSS_DXCH_LPM_PROTOCOL_IPV4_STATUS_GET_MAC(protocolBitmap))
    {
        if (PRV_CPSS_DXCH_LPM_PROTOCOL_IPV6_STATUS_GET_MAC(protocolBitmap))
        {
            *protocolStackPtr = CPSS_IP_PROTOCOL_IPV4V6_E;
        }
        else
        {
            *protocolStackPtr = CPSS_IP_PROTOCOL_IPV4_E;
        }
    }
    else
    {
        *protocolStackPtr = CPSS_IP_PROTOCOL_IPV6_E;
    }
}

/*******************************************************************************
* prvCpssDxChLpmConvertIpShadowTypeToLpmShadowType
*
* DESCRIPTION:
*       Convert enum of IP shadow type to LPM shadow type
*
* APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       None
*
* INPUTS:
*       ipShadowType     - the IP shadow type
*
* OUTPUTS:
*       lpmShadowTypePtr - the LPM shadow type
*
* RETURNS:
*       GT_OK         - on success
*       GT_BAD_PARAM  - on wrong parameters
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS prvCpssDxChLpmConvertIpShadowTypeToLpmShadowType
(
    IN  CPSS_DXCH_IP_LPM_SHADOW_TYPE_ENT    ipShadowType,
    OUT PRV_CPSS_DXCH_LPM_SHADOW_TYPE_ENT   *lpmShadowTypePtr
)
{
    switch (ipShadowType)
    {
        case CPSS_DXCH_IP_LPM_TCAM_CHEETAH_SHADOW_E:
            *lpmShadowTypePtr = PRV_CPSS_DXCH_LPM_TCAM_CHEETAH_SHADOW_E;
            break;
        case CPSS_DXCH_IP_LPM_TCAM_CHEETAH2_SHADOW_E:
            *lpmShadowTypePtr = PRV_CPSS_DXCH_LPM_TCAM_CHEETAH2_SHADOW_E;
            break;
        case CPSS_DXCH_IP_LPM_TCAM_CHEETAH3_SHADOW_E:
            *lpmShadowTypePtr = PRV_CPSS_DXCH_LPM_TCAM_CHEETAH3_SHADOW_E;
            break;
        case CPSS_DXCH_IP_LPM_TCAM_XCAT_SHADOW_E:
            *lpmShadowTypePtr = PRV_CPSS_DXCH_LPM_TCAM_XCAT_SHADOW_E;
            break;
        case CPSS_DXCH_IP_LPM_TCAM_XCAT_POLICY_BASED_ROUTING_SHADOW_E:
            *lpmShadowTypePtr = PRV_CPSS_DXCH_LPM_TCAM_XCAT_POLICY_BASED_ROUTING_SHADOW_E;
            break;
        case CPSS_DXCH_IP_LPM_RAM_LION3_SHADOW_E:
            *lpmShadowTypePtr = PRV_CPSS_DXCH_LPM_RAM_LION3_SHADOW_E;
            break;
        default:
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }
    return GT_OK;
}

/*******************************************************************************
* prvCpssDxChLpmConvertLpmShadowTypeToIpShadowType
*
* DESCRIPTION:
*       Convert enum of LPM shadow type to IP shadow type
*
* APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       None
*
* INPUTS:
*       lpmShadowType   - the LPM shadow type
*
* OUTPUTS:
*       ipShadowTypePtr - the IP shadow type
*
* RETURNS:
*       GT_OK         - on success
*       GT_BAD_PARAM  - on wrong parameters
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS prvCpssDxChLpmConvertLpmShadowTypeToIpShadowType
(
    IN  PRV_CPSS_DXCH_LPM_SHADOW_TYPE_ENT    lpmShadowType,
    OUT CPSS_DXCH_IP_LPM_SHADOW_TYPE_ENT     *ipShadowTypePtr
)
{
    switch (lpmShadowType)
    {
        case PRV_CPSS_DXCH_LPM_TCAM_CHEETAH_SHADOW_E:
            *ipShadowTypePtr = CPSS_DXCH_IP_LPM_TCAM_CHEETAH_SHADOW_E;
            break;
        case PRV_CPSS_DXCH_LPM_TCAM_CHEETAH2_SHADOW_E:
            *ipShadowTypePtr = CPSS_DXCH_IP_LPM_TCAM_CHEETAH2_SHADOW_E;
            break;
        case PRV_CPSS_DXCH_LPM_TCAM_CHEETAH3_SHADOW_E:
            *ipShadowTypePtr = CPSS_DXCH_IP_LPM_TCAM_CHEETAH3_SHADOW_E;
            break;
        case PRV_CPSS_DXCH_LPM_TCAM_XCAT_SHADOW_E:
            *ipShadowTypePtr = CPSS_DXCH_IP_LPM_TCAM_XCAT_SHADOW_E;
            break;
        case PRV_CPSS_DXCH_LPM_TCAM_XCAT_POLICY_BASED_ROUTING_SHADOW_E:
            *ipShadowTypePtr = CPSS_DXCH_IP_LPM_TCAM_XCAT_POLICY_BASED_ROUTING_SHADOW_E;
            break;
        case PRV_CPSS_DXCH_LPM_RAM_LION3_SHADOW_E:
            *ipShadowTypePtr = CPSS_DXCH_IP_LPM_RAM_LION3_SHADOW_E;
            break;
        default:
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }
    return GT_OK;
}

/*******************************************************************************
* prvCpssDxChLpmConvertIpLttEntryToLpmRouteEntry
*
* DESCRIPTION:
*       Convert IP LTT entry structure to private LPM route entry structure
*
* APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       None
*
* INPUTS:
*       lpmHwType       - whether the device uses RAM for LPM
*       ipLttEntryPtr   - the IP LTT entry
*
* OUTPUTS:
*       routeEntryPtr   - the LPM route entry
*
* RETURNS:
*       None
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_VOID prvCpssDxChLpmConvertIpLttEntryToLpmRouteEntry
(
    IN  PRV_CPSS_DXCH_LPM_HW_ENT                        lpmHwType,
    IN  CPSS_DXCH_IP_LTT_ENTRY_STC                      *ipLttEntryPtr,
    OUT PRV_CPSS_DXCH_LPM_ROUTE_ENTRY_POINTER_STC       *routeEntryPtr
)
{
    routeEntryPtr->routeEntryBaseMemAddr = ipLttEntryPtr->routeEntryBaseIndex;
    routeEntryPtr->blockSize = ipLttEntryPtr->numOfPaths;

    if (ipLttEntryPtr->routeType == CPSS_DXCH_IP_ROUTE_ENTRY_METHOD_ECMP_E)
    {
        if ((ipLttEntryPtr->numOfPaths > 1) || (lpmHwType == PRV_CPSS_DXCH_LPM_HW_RAM_E))
        {
            routeEntryPtr->routeEntryMethod = PRV_CPSS_DXCH_LPM_ENTRY_TYPE_ECMP_E;
        }
        else
        {
            routeEntryPtr->routeEntryMethod = PRV_CPSS_DXCH_LPM_ENTRY_TYPE_REGULAR_E;
        }
    }
    else if (ipLttEntryPtr->routeType == CPSS_DXCH_IP_ROUTE_ENTRY_METHOD_QOS_E)
    {
        routeEntryPtr->routeEntryMethod = PRV_CPSS_DXCH_LPM_ENTRY_TYPE_QOS_E;
    }
    else
    {
        routeEntryPtr->routeEntryMethod = ipLttEntryPtr->routeType;
    }

    routeEntryPtr->ucRpfCheckEnable = ipLttEntryPtr->ucRPFCheckEnable;
    routeEntryPtr->srcAddrCheckMismatchEnable = ipLttEntryPtr->sipSaCheckMismatchEnable;
    routeEntryPtr->ipv6McGroupScopeLevel = ipLttEntryPtr->ipv6MCGroupScopeLevel;
}

/*******************************************************************************
* prvCpssDxChLpmConvertLpmRouteEntryToIpLttEntry
*
* DESCRIPTION:
*       Convert private LPM route entry structure to IP LTT entry structure
*
* APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       None
*
* INPUTS:
*       lpmHwType       - whether the device uses RAM for LPM
*       routeEntryPtr   - the LPM route entry
*
* OUTPUTS:
*       ipLttEntryPtr   - the IP LTT entry
*
* RETURNS:
*       None
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_VOID prvCpssDxChLpmConvertLpmRouteEntryToIpLttEntry
(
    IN  PRV_CPSS_DXCH_LPM_HW_ENT                        lpmHwType,
    IN  PRV_CPSS_DXCH_LPM_ROUTE_ENTRY_POINTER_STC       *routeEntryPtr,
    OUT CPSS_DXCH_IP_LTT_ENTRY_STC                      *ipLttEntryPtr
)
{
    ipLttEntryPtr->routeEntryBaseIndex = routeEntryPtr->routeEntryBaseMemAddr;
    ipLttEntryPtr->numOfPaths = routeEntryPtr->blockSize;

    if (routeEntryPtr->routeEntryMethod == PRV_CPSS_DXCH_LPM_ENTRY_TYPE_QOS_E)
    {
        ipLttEntryPtr->routeType = CPSS_DXCH_IP_ROUTE_ENTRY_METHOD_QOS_E;
    }
    else if (routeEntryPtr->routeEntryMethod == PRV_CPSS_DXCH_LPM_ENTRY_TYPE_ECMP_E)
    {
        ipLttEntryPtr->routeType = CPSS_DXCH_IP_ROUTE_ENTRY_METHOD_ECMP_E;
    }
    else if (routeEntryPtr->routeEntryMethod == PRV_CPSS_DXCH_LPM_ENTRY_TYPE_REGULAR_E)
    {
        if (lpmHwType == PRV_CPSS_DXCH_LPM_HW_RAM_E)
        {
            ipLttEntryPtr->routeType = CPSS_DXCH_IP_ROUTE_ENTRY_METHOD_REGULAR_E;
        }
        else
        {
            ipLttEntryPtr->routeType = CPSS_DXCH_IP_ROUTE_ENTRY_METHOD_ECMP_E;
        }
    }
    else /* invalid value, but it should be copied as is */
    {
        ipLttEntryPtr->routeType = routeEntryPtr->routeEntryMethod;
    }

    ipLttEntryPtr->ucRPFCheckEnable = routeEntryPtr->ucRpfCheckEnable;
    ipLttEntryPtr->sipSaCheckMismatchEnable = routeEntryPtr->srcAddrCheckMismatchEnable;
    ipLttEntryPtr->ipv6MCGroupScopeLevel = routeEntryPtr->ipv6McGroupScopeLevel;
}

/*******************************************************************************
* prvCpssDxChLpmConvertIpVrConfigToTcamVrConfig
*
* DESCRIPTION:
*       Convert IP VR config structure to private TCAM VR config structure
*
* APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* NOT APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3;.
*
* INPUTS:
*       shadowType      - the shadow type
*       ipVrConfigPtr      - the IP VR config structure
*
* OUTPUTS:
*       tcamVrConfigPtr    - the TCAM VR config structure
*
* RETURNS:
*       None
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_VOID prvCpssDxChLpmConvertIpVrConfigToTcamVrConfig
(
    IN  PRV_CPSS_DXCH_LPM_SHADOW_TYPE_ENT        shadowType,
    IN  CPSS_DXCH_IP_LPM_VR_CONFIG_STC           *ipVrConfigPtr,
    OUT PRV_CPSS_DXCH_LPM_TCAM_VR_CONFIG_STC     *tcamVrConfigPtr
)
{
    tcamVrConfigPtr->supportIpv4Uc = ipVrConfigPtr->supportIpv4Uc;
    tcamVrConfigPtr->supportIpv4Mc = ipVrConfigPtr->supportIpv4Mc;
    tcamVrConfigPtr->supportIpv6Uc = ipVrConfigPtr->supportIpv6Uc;
    tcamVrConfigPtr->supportIpv6Mc = ipVrConfigPtr->supportIpv6Mc;
    switch (shadowType)
    {
        case PRV_CPSS_DXCH_LPM_TCAM_CHEETAH_SHADOW_E:
        case PRV_CPSS_DXCH_LPM_TCAM_XCAT_POLICY_BASED_ROUTING_SHADOW_E:
            cpssOsMemCpy(&(tcamVrConfigPtr->defIpv4UcNextHopInfo.pclIpUcAction),
                         &(ipVrConfigPtr->defIpv4UcNextHopInfo.pclIpUcAction),
                         sizeof(CPSS_DXCH_PCL_ACTION_STC));
            cpssOsMemCpy(&(tcamVrConfigPtr->defIpv6UcNextHopInfo.pclIpUcAction),
                         &(ipVrConfigPtr->defIpv6UcNextHopInfo.pclIpUcAction),
                         sizeof(CPSS_DXCH_PCL_ACTION_STC));
            break;

        case PRV_CPSS_DXCH_LPM_TCAM_CHEETAH2_SHADOW_E:
        case PRV_CPSS_DXCH_LPM_TCAM_CHEETAH3_SHADOW_E:
        case PRV_CPSS_DXCH_LPM_TCAM_XCAT_SHADOW_E:
            prvCpssDxChLpmConvertIpLttEntryToLpmRouteEntry(PRV_CPSS_DXCH_LPM_HW_TCAM_E,
                                                           &(ipVrConfigPtr->defIpv4UcNextHopInfo.ipLttEntry),
                                                           &(tcamVrConfigPtr->defIpv4UcNextHopInfo.routeEntry));
            prvCpssDxChLpmConvertIpLttEntryToLpmRouteEntry(PRV_CPSS_DXCH_LPM_HW_TCAM_E,
                                                           &(ipVrConfigPtr->defIpv6UcNextHopInfo.ipLttEntry),
                                                           &(tcamVrConfigPtr->defIpv6UcNextHopInfo.routeEntry));
            break;

        default:    /* prevent compiler warning */
            return;
    }
    prvCpssDxChLpmConvertIpLttEntryToLpmRouteEntry(PRV_CPSS_DXCH_LPM_HW_TCAM_E,
                                                   &(ipVrConfigPtr->defIpv4McRouteLttEntry),
                                                   &(tcamVrConfigPtr->defIpv4McRouteLttEntry));
    prvCpssDxChLpmConvertIpLttEntryToLpmRouteEntry(PRV_CPSS_DXCH_LPM_HW_TCAM_E,
                                                   &(ipVrConfigPtr->defIpv6McRouteLttEntry),
                                                   &(tcamVrConfigPtr->defIpv6McRouteLttEntry));
}

/*******************************************************************************
* prvCpssDxChLpmConvertTcamVrConfigToIpVrConfig
*
* DESCRIPTION:
*       Convert private TCAM VR config structure to IP VR config structure
*
* APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* NOT APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3;.
*
* INPUTS:
*       shadowType      - the shadow type
*       tcamVrConfigPtr - the TCAM VR config structure
*
* OUTPUTS:
*       ipVrConfigPtr   - the IP VR config structure
*
* RETURNS:
*       None
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_VOID prvCpssDxChLpmConvertTcamVrConfigToIpVrConfig
(
    IN  PRV_CPSS_DXCH_LPM_SHADOW_TYPE_ENT        shadowType,
    IN  PRV_CPSS_DXCH_LPM_TCAM_VR_CONFIG_STC     *tcamVrConfigPtr,
    OUT CPSS_DXCH_IP_LPM_VR_CONFIG_STC           *ipVrConfigPtr
)
{
    ipVrConfigPtr->supportIpv4Uc = tcamVrConfigPtr->supportIpv4Uc;
    ipVrConfigPtr->supportIpv4Mc = tcamVrConfigPtr->supportIpv4Mc;
    ipVrConfigPtr->supportIpv6Uc = tcamVrConfigPtr->supportIpv6Uc;
    ipVrConfigPtr->supportIpv6Mc = tcamVrConfigPtr->supportIpv6Mc;
    switch (shadowType)
    {
        case PRV_CPSS_DXCH_LPM_TCAM_CHEETAH_SHADOW_E:
        case PRV_CPSS_DXCH_LPM_TCAM_XCAT_POLICY_BASED_ROUTING_SHADOW_E:
            cpssOsMemCpy(&(ipVrConfigPtr->defIpv4UcNextHopInfo.pclIpUcAction),
                         &(tcamVrConfigPtr->defIpv4UcNextHopInfo.pclIpUcAction),
                         sizeof(CPSS_DXCH_PCL_ACTION_STC));
            cpssOsMemCpy(&(ipVrConfigPtr->defIpv6UcNextHopInfo.pclIpUcAction),
                         &(tcamVrConfigPtr->defIpv6UcNextHopInfo.pclIpUcAction),
                         sizeof(CPSS_DXCH_PCL_ACTION_STC));
            break;

        case PRV_CPSS_DXCH_LPM_TCAM_CHEETAH2_SHADOW_E:
        case PRV_CPSS_DXCH_LPM_TCAM_CHEETAH3_SHADOW_E:
        case PRV_CPSS_DXCH_LPM_TCAM_XCAT_SHADOW_E:
            prvCpssDxChLpmConvertLpmRouteEntryToIpLttEntry(PRV_CPSS_DXCH_LPM_HW_TCAM_E,
                                                           &(tcamVrConfigPtr->defIpv4UcNextHopInfo.routeEntry),
                                                           &(ipVrConfigPtr->defIpv4UcNextHopInfo.ipLttEntry));
            prvCpssDxChLpmConvertLpmRouteEntryToIpLttEntry(PRV_CPSS_DXCH_LPM_HW_TCAM_E,
                                                           &(tcamVrConfigPtr->defIpv6UcNextHopInfo.routeEntry),
                                                           &(ipVrConfigPtr->defIpv6UcNextHopInfo.ipLttEntry));
            break;

        default:    /* prevent compiler warning */
            return;
    }
    prvCpssDxChLpmConvertLpmRouteEntryToIpLttEntry(PRV_CPSS_DXCH_LPM_HW_TCAM_E,
                                                   &(tcamVrConfigPtr->defIpv4McRouteLttEntry),
                                                   &(ipVrConfigPtr->defIpv4McRouteLttEntry));
    prvCpssDxChLpmConvertLpmRouteEntryToIpLttEntry(PRV_CPSS_DXCH_LPM_HW_TCAM_E,
                                                   &(tcamVrConfigPtr->defIpv6McRouteLttEntry),
                                                   &(ipVrConfigPtr->defIpv6McRouteLttEntry));
}

/*******************************************************************************
* prvCpssDxChLpmConvertIpVrConfigToRamVrConfig
*
* DESCRIPTION:
*       Convert IP VR config structure to private RAM VR config structure
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       ipVrConfigPtr      - the IP VR config structure
*
* OUTPUTS:
*       ramVrConfigPtr     - the RAM VR config structure
*
* RETURNS:
*       None
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_VOID prvCpssDxChLpmConvertIpVrConfigToRamVrConfig
(
    IN  CPSS_DXCH_IP_LPM_VR_CONFIG_STC           *ipVrConfigPtr,
    OUT PRV_CPSS_DXCH_LPM_RAM_VR_CONFIG_STC      *ramVrConfigPtr
)
{
    ramVrConfigPtr->supportUcIpv4 = ipVrConfigPtr->supportIpv4Uc;
    ramVrConfigPtr->supportMcIpv4 = ipVrConfigPtr->supportIpv4Mc;
    ramVrConfigPtr->supportUcIpv6 = ipVrConfigPtr->supportIpv6Uc;
    ramVrConfigPtr->supportMcIpv6 = ipVrConfigPtr->supportIpv6Mc;
    ramVrConfigPtr->supportFcoe   = GT_FALSE;
    prvCpssDxChLpmConvertIpLttEntryToLpmRouteEntry(PRV_CPSS_DXCH_LPM_HW_RAM_E,
                                                   &(ipVrConfigPtr->defIpv4UcNextHopInfo.ipLttEntry),
                                                   &(ramVrConfigPtr->defaultUcIpv4RouteEntry));
    prvCpssDxChLpmConvertIpLttEntryToLpmRouteEntry(PRV_CPSS_DXCH_LPM_HW_RAM_E,
                                                   &(ipVrConfigPtr->defIpv6UcNextHopInfo.ipLttEntry),
                                                   &(ramVrConfigPtr->defaultUcIpv6RouteEntry));
    prvCpssDxChLpmConvertIpLttEntryToLpmRouteEntry(PRV_CPSS_DXCH_LPM_HW_RAM_E,
                                                   &(ipVrConfigPtr->defIpv4McRouteLttEntry),
                                                   &(ramVrConfigPtr->defaultMcIpv4RouteEntry));
    prvCpssDxChLpmConvertIpLttEntryToLpmRouteEntry(PRV_CPSS_DXCH_LPM_HW_RAM_E,
                                                   &(ipVrConfigPtr->defIpv6McRouteLttEntry),
                                                   &(ramVrConfigPtr->defaultMcIpv6RouteEntry));
}

/*******************************************************************************
* prvCpssDxChLpmConvertRamVrConfigToIpVrConfig
*
* DESCRIPTION:
*       Convert private RAM VR config structure to IP VR config structure
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       ramVrConfigPtr  - the RAM VR config structure
*
* OUTPUTS:
*       ipVrConfigPtr   - the IP VR config structure
*
* RETURNS:
*       None
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_VOID prvCpssDxChLpmConvertRamVrConfigToIpVrConfig
(
    IN  PRV_CPSS_DXCH_LPM_RAM_VR_CONFIG_STC      *ramVrConfigPtr,
    OUT CPSS_DXCH_IP_LPM_VR_CONFIG_STC           *ipVrConfigPtr
)
{
    ipVrConfigPtr->supportIpv4Uc = ramVrConfigPtr->supportUcIpv4;
    ipVrConfigPtr->supportIpv4Mc = ramVrConfigPtr->supportMcIpv4;
    ipVrConfigPtr->supportIpv6Uc = ramVrConfigPtr->supportUcIpv6;
    ipVrConfigPtr->supportIpv6Mc = ramVrConfigPtr->supportMcIpv6;
    prvCpssDxChLpmConvertLpmRouteEntryToIpLttEntry(PRV_CPSS_DXCH_LPM_HW_RAM_E,
                                                   &(ramVrConfigPtr->defaultUcIpv4RouteEntry),
                                                   &(ipVrConfigPtr->defIpv4UcNextHopInfo.ipLttEntry));
    prvCpssDxChLpmConvertLpmRouteEntryToIpLttEntry(PRV_CPSS_DXCH_LPM_HW_RAM_E,
                                                   &(ramVrConfigPtr->defaultUcIpv6RouteEntry),
                                                   &(ipVrConfigPtr->defIpv6UcNextHopInfo.ipLttEntry));
    prvCpssDxChLpmConvertLpmRouteEntryToIpLttEntry(PRV_CPSS_DXCH_LPM_HW_RAM_E,
                                                   &(ramVrConfigPtr->defaultMcIpv4RouteEntry),
                                                   &(ipVrConfigPtr->defIpv4McRouteLttEntry));
    prvCpssDxChLpmConvertLpmRouteEntryToIpLttEntry(PRV_CPSS_DXCH_LPM_HW_RAM_E,
                                                   &(ramVrConfigPtr->defaultMcIpv6RouteEntry),
                                                   &(ipVrConfigPtr->defIpv6McRouteLttEntry));
}

/*******************************************************************************
* prvCpssDxChLpmConvertIpRouteEntryToLpmRouteEntry
*
* DESCRIPTION:
*       Convert IP route entry union to private LPM route entry union
*
* APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3;.
*
* NOT APPLICABLE DEVICES:
*       None
*
* INPUTS:
*       shadowType            - shadow type
*       ipRouteEntryInfoPtr   - IP route entry union
*
* OUTPUTS:
*       lpmRouteEntryInfoPtr  - private LPM route entry union
*
* RETURNS:
*       None
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_VOID prvCpssDxChLpmConvertIpRouteEntryToLpmRouteEntry
(
    IN  PRV_CPSS_DXCH_LPM_SHADOW_TYPE_ENT        shadowType,
    IN  CPSS_DXCH_IP_TCAM_ROUTE_ENTRY_INFO_UNT   *ipRouteEntryInfoPtr,
    OUT PRV_CPSS_DXCH_LPM_ROUTE_ENTRY_INFO_UNT   *lpmRouteEntryInfoPtr
)
{
    switch (shadowType)
    {
        case PRV_CPSS_DXCH_LPM_TCAM_CHEETAH_SHADOW_E:
        case PRV_CPSS_DXCH_LPM_TCAM_XCAT_POLICY_BASED_ROUTING_SHADOW_E:
            cpssOsMemCpy(&(lpmRouteEntryInfoPtr->pclIpUcAction),
                         &(ipRouteEntryInfoPtr->pclIpUcAction),
                         sizeof(CPSS_DXCH_PCL_ACTION_STC));
            break;

        case PRV_CPSS_DXCH_LPM_TCAM_CHEETAH2_SHADOW_E:
        case PRV_CPSS_DXCH_LPM_TCAM_CHEETAH3_SHADOW_E:
        case PRV_CPSS_DXCH_LPM_TCAM_XCAT_SHADOW_E:
            prvCpssDxChLpmConvertIpLttEntryToLpmRouteEntry(PRV_CPSS_DXCH_LPM_HW_TCAM_E,
                                                           &(ipRouteEntryInfoPtr->ipLttEntry),
                                                           &(lpmRouteEntryInfoPtr->routeEntry));
            break;

        case PRV_CPSS_DXCH_LPM_RAM_LION3_SHADOW_E:
            prvCpssDxChLpmConvertIpLttEntryToLpmRouteEntry(PRV_CPSS_DXCH_LPM_HW_RAM_E,
                                                           &(ipRouteEntryInfoPtr->ipLttEntry),
                                                           &(lpmRouteEntryInfoPtr->routeEntry));
            break;

        default:    /* prevent compiler warning */
            break;
    }
}

/*******************************************************************************
* prvCpssDxChLpmConvertLpmRouteEntryToIpRouteEntry
*
* DESCRIPTION:
*       Convert private LPM route entry union to IP route entry union
*
* APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3;.
*
* NOT APPLICABLE DEVICES:
*       None
*
* INPUTS:
*       shadowType           - shadow type
*       lpmRouteEntryInfoPtr - private LPM route entry union
*
* OUTPUTS:
*       ipRouteEntryInfoPtr  - IP route entry union
*
* RETURNS:
*       None
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_VOID prvCpssDxChLpmConvertLpmRouteEntryToIpRouteEntry
(
    IN  PRV_CPSS_DXCH_LPM_SHADOW_TYPE_ENT        shadowType,
    IN  PRV_CPSS_DXCH_LPM_ROUTE_ENTRY_INFO_UNT   *lpmRouteEntryInfoPtr,
    OUT CPSS_DXCH_IP_TCAM_ROUTE_ENTRY_INFO_UNT   *ipRouteEntryInfoPtr
)
{
    switch (shadowType)
    {
        case PRV_CPSS_DXCH_LPM_TCAM_CHEETAH_SHADOW_E:
        case PRV_CPSS_DXCH_LPM_TCAM_XCAT_POLICY_BASED_ROUTING_SHADOW_E:
            cpssOsMemCpy(&(ipRouteEntryInfoPtr->pclIpUcAction),
                         &(lpmRouteEntryInfoPtr->pclIpUcAction),
                         sizeof(CPSS_DXCH_PCL_ACTION_STC));
            break;

        case PRV_CPSS_DXCH_LPM_TCAM_CHEETAH2_SHADOW_E:
        case PRV_CPSS_DXCH_LPM_TCAM_CHEETAH3_SHADOW_E:
        case PRV_CPSS_DXCH_LPM_TCAM_XCAT_SHADOW_E:
            prvCpssDxChLpmConvertLpmRouteEntryToIpLttEntry(PRV_CPSS_DXCH_LPM_HW_TCAM_E,
                                                           &(lpmRouteEntryInfoPtr->routeEntry),
                                                           &(ipRouteEntryInfoPtr->ipLttEntry));
            break;

        case PRV_CPSS_DXCH_LPM_RAM_LION3_SHADOW_E:
            prvCpssDxChLpmConvertLpmRouteEntryToIpLttEntry(PRV_CPSS_DXCH_LPM_HW_RAM_E,
                                                           &(lpmRouteEntryInfoPtr->routeEntry),
                                                           &(ipRouteEntryInfoPtr->ipLttEntry));
            break;

        default:    /* prevent compiler warning */
            break;
    }
}

/*******************************************************************************
* prvCpssDxChLpmGetHwType
*
* DESCRIPTION:
*       Convert LPM shadow type to HW type.
*
* APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3;.
*
* NOT APPLICABLE DEVICES:
*       None
*
* INPUTS:
*       lpmShadowType        - shadow type
*
* OUTPUTS:
*       None
*
* RETURNS:
*       the HW type used for LPM (TCAM or RAM)
*
* COMMENTS:
*       None
*
*******************************************************************************/
PRV_CPSS_DXCH_LPM_HW_ENT prvCpssDxChLpmGetHwType
(
    IN  PRV_CPSS_DXCH_LPM_SHADOW_TYPE_ENT lpmShadowType
)
{
    if((lpmShadowType == PRV_CPSS_DXCH_LPM_TCAM_CHEETAH_SHADOW_E) ||
       (lpmShadowType == PRV_CPSS_DXCH_LPM_TCAM_CHEETAH2_SHADOW_E) ||
       (lpmShadowType == PRV_CPSS_DXCH_LPM_TCAM_CHEETAH3_SHADOW_E) ||
       (lpmShadowType == PRV_CPSS_DXCH_LPM_TCAM_XCAT_SHADOW_E) ||
       (lpmShadowType == PRV_CPSS_DXCH_LPM_TCAM_XCAT_POLICY_BASED_ROUTING_SHADOW_E))
    {
        return PRV_CPSS_DXCH_LPM_HW_TCAM_E;
    }

    if (lpmShadowType == PRV_CPSS_DXCH_LPM_RAM_LION3_SHADOW_E)
    {
        return PRV_CPSS_DXCH_LPM_HW_RAM_E;
    }

    return PRV_CPSS_DXCH_LPM_HW_LAST_E;
}

