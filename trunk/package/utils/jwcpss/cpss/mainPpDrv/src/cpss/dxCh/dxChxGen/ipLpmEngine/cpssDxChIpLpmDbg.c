/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* cpssDxChIpLpmDbg.c
*
* DESCRIPTION:
*       the CPSS DXCH debug .
*
* FILE REVISION NUMBER:
*       $Revision: 54 $
*******************************************************************************/
#define CPSS_LOG_IN_MODULE_ENABLE
#include <cpss/dxCh/dxChxGen/ipLpmEngine/cpssDxChIpLpm.h>
#include <cpss/dxCh/dxChxGen/ipLpmEngine/private/cpssDxChPrvIpLpm.h>
#include <cpss/dxCh/dxChxGen/ipLpmEngine/private/prvCpssDxChIpLpmEngineLog.h>
#include <cpss/dxCh/dxChxGen/private/lpm/tcam/prvCpssDxChLpmTcamDbg.h>
#include <cpss/dxCh/dxChxGen/private/lpm/ram/prvCpssDxChLpmRamDbg.h>
#include <cpss/dxCh/dxChxGen/private/routerTunnelTermTcam/prvCpssDxChRouterTunnelTermTcam.h>
#include <cpss/extServices/private/prvCpssBindFunc.h>
#include <cpss/dxCh/dxChxGen/private/lpm/prvCpssDxChLpmUtils.h>
#include <cpss/generic/config/private/prvCpssConfigTypes.h>
#include <cpss/dxCh/dxChxGen/config/private/prvCpssDxChInfo.h>

/* lpm db skip list ptr */
extern GT_VOID *prvCpssDxChIplpmDbSL;

extern GT_VOID * prvCpssSlSearch
(
    IN GT_VOID        *ctrlPtr,
    IN GT_VOID        *dataPtr
);

/*******************************************************************************
* outputIPAddress
*
* DESCRIPTION:
*       Print Ip address
*
* INPUTS:
*       ipAddrPtr     - ip address
*       bytes2Print   - amount of printed bytes
*       isIpv4        - Ipv4 or Ipv6 address
*
* OUTPUTS:
*       None
*
* COMMENTS:
*
*******************************************************************************/
static GT_VOID outputIPAddress
(
    IN GT_U8 *ipAddrPtr,
    IN GT_U32 bytes2Print,
    IN GT_BOOL isIpv4
)
{
    while(bytes2Print > 1)
    {
        if (GT_TRUE == isIpv4)
        {
            cpssOsPrintf("%03d.%03d.", *ipAddrPtr, *(ipAddrPtr+1));
        }
        else
        {
            cpssOsPrintf("%02X%02X:", *ipAddrPtr, *(ipAddrPtr+1));
        }

        ipAddrPtr+=2;
        bytes2Print-=2;
    }

    if (bytes2Print > 0)
    {
        if (GT_TRUE == isIpv4)
        {
            cpssOsPrintf("%03d", *ipAddrPtr);
        }
        else
        {
            cpssOsPrintf("%02X", *ipAddrPtr);
        }
    }
}

/*******************************************************************************
* prvCpssDxChIpLpmDbgConvertRouteType
*
* DESCRIPTION:
*   Convert CPSS_DXCH_IP_ROUTE_ENTRY_METHOD_ECMP_E to
*   CPSS_DXCH_IP_ROUTE_ENTRY_METHOD_REGULAR_E for RAM based LPM whenever input
*   type is CPSS_DXCH_IP_ROUTE_ENTRY_METHOD_ECMP_E and numOfPaths is 0
*
*
* INPUTS:
*       lpmDBId               - LPM DB id
*       ipLttEntryPtr         - (pointer to) the original IP LTT entry
*
* OUTPUTS:
*       ipLttEntryPtr         - (pointer to) the changed IP LTT entry
*
* RETURNS:
*       GT_OK          - on success
*       GT_NOT_FOUND   - if can't find the lpm DB
*       GT_BAD_PTR     - on null pointer or if shadowType is uninitialized
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS prvCpssDxChIpLpmDbgConvertRouteType
(
    IN    GT_U32                          lpmDBId,
    INOUT CPSS_DXCH_IP_LTT_ENTRY_STC      *ipLttEntryPtr
)
{
    GT_STATUS                                       status;
    CPSS_DXCH_IP_LPM_SHADOW_TYPE_ENT                shadowType;
    CPSS_IP_PROTOCOL_STACK_ENT                      protocolStack;
    CPSS_DXCH_IP_LPM_MEMORY_CONFIG_UNT              memoryCfg;
    CPSS_DXCH_IP_TCAM_LPM_MANGER_INDEX_RANGE_STC    indexesRange;
    CPSS_DXCH_IP_TCAM_LPM_MANGER_CAPCITY_CFG_STC    tcamLpmManagerCapcityCfg;
    GT_PTR                                          tcamManagerHandler;

    memoryCfg.tcamDbCfg.indexesRangePtr = &indexesRange;
    memoryCfg.tcamDbCfg.tcamLpmManagerCapcityCfgPtr = &tcamLpmManagerCapcityCfg;
    memoryCfg.tcamDbCfg.tcamManagerHandlerPtr = &tcamManagerHandler;
    status = cpssDxChIpLpmDBConfigGet(lpmDBId, &shadowType, &protocolStack, &memoryCfg);
    if (status != GT_OK)
    {
        return status;
    }

    if (ipLttEntryPtr == NULL)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PTR, LOG_ERROR_NO_MSG);
    }

    if ((shadowType == CPSS_DXCH_IP_LPM_RAM_LION3_SHADOW_E) &&
        (ipLttEntryPtr->routeType == CPSS_DXCH_IP_ROUTE_ENTRY_METHOD_ECMP_E) &&
        (ipLttEntryPtr->numOfPaths == 0))
    {
        ipLttEntryPtr->routeType = CPSS_DXCH_IP_ROUTE_ENTRY_METHOD_REGULAR_E;
    }

    return GT_OK;
}

/*******************************************************************************
* dumpRouteTcam
*
* DESCRIPTION:
*     This func makes physical router tcam scanning and prints its contents.
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*     dump   -  parameter for debugging purposes
*
* OUTPUTS:
*     None
*
* RETURNS:
*       GT_OK   - on success.
*       GT_BAD_PARAM             - on bad parameter.
*       GT_BAD_PTR               - on NULL pointer
*       GT_NOT_INITIALIZED       - if the driver was not initialized
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_FAIL - on failure.
*
* COMMENTS:
*     None
*
*******************************************************************************/
GT_STATUS dumpRouteTcam
(
    IN GT_BOOL dump
)
{
    return prvCpssDxChLpmTcamDbgDumpRouteTcam(dump);
}

/*******************************************************************************
* cpssDxChIpPatTriePrint
*
* DESCRIPTION:
*     This function prints Patricia trie contents.
*
* APPLICABLE DEVICES:
*        xCat; xCat3; Lion; xCat2; Lion2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*     lpmDbId     - The LPM DB id.
*     vrId        - The virtual router identifier.
*     protocol    - ip protocol
*     prefixType  - uc/mc prefix type
*
* OUTPUTS:
*     None.
*
* RETURNS:
*     GT_OK                    - on success
*     GT_NOT_FOUND             - if lpmDbId or vrId is not found
*     GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChIpPatTriePrint
(
    IN GT_U32                     lpmDbId,
    IN GT_U32                     vrId,
    IN CPSS_IP_PROTOCOL_STACK_ENT protocol,
    IN CPSS_UNICAST_MULTICAST_ENT prefixType
)
{
    PRV_CPSS_DXCH_LPM_SHADOW_STC                    *lpmDbPtr,tmpLpmDb;
    PRV_CPSS_DXCH_LPM_HW_ENT                        lpmHw;

    tmpLpmDb.lpmDbId = lpmDbId;
    lpmDbPtr = prvCpssSlSearch(prvCpssDxChIplpmDbSL,&tmpLpmDb);
    if (lpmDbPtr == NULL)
    {
        /* can't find the lpm DB */
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_FOUND, LOG_ERROR_NO_MSG);
    }

    lpmHw = prvCpssDxChLpmGetHwType(lpmDbPtr->shadowType);
    switch (lpmHw)
    {
        case PRV_CPSS_DXCH_LPM_HW_TCAM_E:
            return prvCpssDxChLpmTcamDbgPatTriePrint((PRV_CPSS_DXCH_LPM_TCAM_SHADOW_STC*)(lpmDbPtr->shadow), vrId,
                                                     protocol, prefixType);

        case PRV_CPSS_DXCH_LPM_HW_RAM_E:
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_APPLICABLE_DEVICE, LOG_ERROR_NO_MSG);

        default:
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }
}

/*******************************************************************************
* cpssDxChIpPatTrieValidityCheck
*
* DESCRIPTION:
*   This function checks Patricia trie validity.
*
* APPLICABLE DEVICES:
*        xCat; xCat3; Lion; xCat2; Lion2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       lpmDBId     - The LPM DB id.
*       vrId        - The virtual router identifier.
*       protocol    - ip protocol
*       prefixType   - uc/mc prefix type
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_STATE             - on bad state in patricia trie
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*******************************************************************************/
GT_STATUS cpssDxChIpPatTrieValidityCheck
(
  IN GT_U32                     lpmDBId,
  IN GT_U32                     vrId,
  IN CPSS_IP_PROTOCOL_STACK_ENT protocol,
  IN CPSS_UNICAST_MULTICAST_ENT prefixType
)
{
    PRV_CPSS_DXCH_LPM_SHADOW_STC        *lpmDbPtr,tmpLpmDb;
    PRV_CPSS_DXCH_LPM_HW_ENT            lpmHw;

    tmpLpmDb.lpmDbId = lpmDBId;
    lpmDbPtr = prvCpssSlSearch(prvCpssDxChIplpmDbSL,&tmpLpmDb);
    if (lpmDbPtr == NULL)
    {
        /* can't find the lpm DB */
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_FOUND, LOG_ERROR_NO_MSG);
    }

    lpmHw = prvCpssDxChLpmGetHwType(lpmDbPtr->shadowType);
    switch (lpmHw)
    {
        case PRV_CPSS_DXCH_LPM_HW_TCAM_E:
            return prvCpssDxChLpmTcamDbgPatTrieValidityCheck((PRV_CPSS_DXCH_LPM_TCAM_SHADOW_STC*)(lpmDbPtr->shadow),
                                                             vrId, protocol,
                                                             prefixType);

        case PRV_CPSS_DXCH_LPM_HW_RAM_E:
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_APPLICABLE_DEVICE, LOG_ERROR_NO_MSG);

        default:
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }
}

/*******************************************************************************
* cpssDxChIpLpmDump
*
* DESCRIPTION:
*   Print hardware LPM dump
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum      - The device number
*       vrId        - The virtual router id
*       protocol    - The IP protocol
*       prefixType  - Unicast or multicast
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*******************************************************************************/
GT_STATUS cpssDxChIpLpmDump
(
    IN GT_U8                            devNum,
    IN GT_U32                           vrId,
    IN CPSS_IP_PROTOCOL_STACK_ENT       protocol,
    IN CPSS_UNICAST_MULTICAST_ENT       prefixType
)
{
    PRV_CPSS_DXCH_LPM_PROTOCOL_STACK_ENT    lpmProtocol;

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
                                          CPSS_CH1_E | CPSS_CH1_DIAMOND_E |
                                          CPSS_CH2_E | CPSS_CH3_E |
                                          CPSS_XCAT_E | CPSS_XCAT3_E | CPSS_LION_E |
                                          CPSS_XCAT2_E | CPSS_LION2_E);
    switch (protocol)
    {
        case CPSS_IP_PROTOCOL_IPV4_E:
            lpmProtocol = PRV_CPSS_DXCH_LPM_PROTOCOL_IPV4_E;
            break;

        case CPSS_IP_PROTOCOL_IPV6_E:
            lpmProtocol = PRV_CPSS_DXCH_LPM_PROTOCOL_IPV6_E;
            break;

        default:
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    if ((prefixType != CPSS_UNICAST_E) && (prefixType != CPSS_MULTICAST_E))
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    return prvCpssDxChLpmRamDbgDump(devNum, vrId, lpmProtocol, prefixType);
}

/*******************************************************************************
* cpssDxChIpLpmDbHwMemPrint
*
* DESCRIPTION:
*   Print DMM partition debug information for a specific LPM DB
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       lpmDbId     - The LPM DB id
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*******************************************************************************/
GT_STATUS cpssDxChIpLpmDbHwMemPrint
(
    IN GT_U32                           lpmDbId
)
{
    PRV_CPSS_DXCH_LPM_SHADOW_STC        *lpmDbPtr,tmpLpmDb;
    PRV_CPSS_DXCH_LPM_HW_ENT            lpmHw;

    tmpLpmDb.lpmDbId = lpmDbId;
    lpmDbPtr = prvCpssSlSearch(prvCpssDxChIplpmDbSL,&tmpLpmDb);
    if (lpmDbPtr == NULL)
    {
        /* can't find the lpm DB */
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_FOUND, LOG_ERROR_NO_MSG);
    }

    lpmHw = prvCpssDxChLpmGetHwType(lpmDbPtr->shadowType);
    switch (lpmHw)
    {
        case PRV_CPSS_DXCH_LPM_HW_TCAM_E:
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_APPLICABLE_DEVICE, LOG_ERROR_NO_MSG);

        case PRV_CPSS_DXCH_LPM_HW_RAM_E:
            return prvCpssDxChLpmRamDbgDbHwMemPrint((PRV_CPSS_DXCH_LPM_RAM_SHADOWS_DB_STC*)(lpmDbPtr->shadow));

        default:
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }
}

/*******************************************************************************
* cpssDxChIpLpmHwValidation
*
* DESCRIPTION:
*   Validate the LPM in HW
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum      - The device number
*       vrId        - The virtual router id
*       protocol    - The IP protocol
*       prefixType  - Unicast or multicast
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*******************************************************************************/
GT_STATUS cpssDxChIpLpmHwValidation
(
    IN GT_U8                            devNum,
    IN GT_U32                           vrId,
    IN CPSS_IP_PROTOCOL_STACK_ENT       protocol,
    IN CPSS_UNICAST_MULTICAST_ENT       prefixType
)
{
    PRV_CPSS_DXCH_LPM_PROTOCOL_STACK_ENT    lpmProtocol;

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
                                          CPSS_CH1_E | CPSS_CH1_DIAMOND_E |
                                          CPSS_CH2_E | CPSS_CH3_E |
                                          CPSS_XCAT_E | CPSS_XCAT3_E | CPSS_LION_E |
                                          CPSS_XCAT2_E | CPSS_LION2_E);
    switch (protocol)
    {
        case CPSS_IP_PROTOCOL_IPV4_E:
            lpmProtocol = PRV_CPSS_DXCH_LPM_PROTOCOL_IPV4_E;
            break;

        case CPSS_IP_PROTOCOL_IPV6_E:
            lpmProtocol = PRV_CPSS_DXCH_LPM_PROTOCOL_IPV6_E;
            break;

        default:
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    if ((prefixType != CPSS_UNICAST_E) && (prefixType != CPSS_MULTICAST_E))
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    return prvCpssDxChLpmRamDbgHwValidation(devNum, vrId, lpmProtocol, prefixType);
}

/*******************************************************************************
* cpssDxChIpLpmShadowValidityCheck
*
* DESCRIPTION:
*   Validate the LPM in shadow
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       lpmDbId         - The LPM DB id
*       vrId            - The virtual router id
*       protocol        - The IP protocol
*       prefixType      - Unicast or multicast
*       returnOnFailure - whether to return on first failure
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*******************************************************************************/
GT_STATUS cpssDxChIpLpmShadowValidityCheck
(
    IN GT_U32                           lpmDbId,
    IN GT_U32                           vrId,
    IN CPSS_IP_PROTOCOL_STACK_ENT       protocol,
    IN CPSS_UNICAST_MULTICAST_ENT       prefixType,
    IN GT_BOOL                          returnOnFailure
)
{
    PRV_CPSS_DXCH_LPM_SHADOW_STC        *lpmDbPtr,tmpLpmDb;
    PRV_CPSS_DXCH_LPM_HW_ENT            lpmHw;
    PRV_CPSS_DXCH_LPM_PROTOCOL_BMP      protocolBitmap;

    PRV_CPSS_DXCH_IP_CHECK_PROTOCOL_STACK_MAC(protocol);

    tmpLpmDb.lpmDbId = lpmDbId;
    lpmDbPtr = prvCpssSlSearch(prvCpssDxChIplpmDbSL,&tmpLpmDb);
    if (lpmDbPtr == NULL)
    {
        /* can't find the lpm DB */
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_FOUND, LOG_ERROR_NO_MSG);
    }

    lpmHw = prvCpssDxChLpmGetHwType(lpmDbPtr->shadowType);
    switch (lpmHw)
    {
        case PRV_CPSS_DXCH_LPM_HW_TCAM_E:
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_APPLICABLE_DEVICE, LOG_ERROR_NO_MSG);

        case PRV_CPSS_DXCH_LPM_HW_RAM_E:
            prvCpssDxChLpmConvertIpProtocolStackToProtocolBitmap(protocol, &protocolBitmap);
            return prvCpssDxChLpmRamDbgShadowValidityCheck((PRV_CPSS_DXCH_LPM_RAM_SHADOWS_DB_STC*)(lpmDbPtr->shadow),
                                                           vrId, protocolBitmap, prefixType, returnOnFailure);

        default:
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }
}

/*******************************************************************************
* cpssDxChIpLpmHwShadowSyncValidityCheck
*
* DESCRIPTION:
*   This function validates synchronization between the shadow and HW of the LPM
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum          - The device number
*       lpmDbId         - The LPM DB id
*       vrId            - The virtual router id
*       protocol        - The IP protocol
*       prefixType      - Unicast or multicast
*       returnOnFailure - whether to return on first failure
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*******************************************************************************/
GT_STATUS cpssDxChIpLpmHwShadowSyncValidityCheck
(
    IN GT_U8                            devNum,
    IN GT_U32                           lpmDbId,
    IN GT_U32                           vrId,
    IN CPSS_IP_PROTOCOL_STACK_ENT       protocol,
    IN CPSS_UNICAST_MULTICAST_ENT       prefixType,
    IN GT_BOOL                          returnOnFailure
)
{
    PRV_CPSS_DXCH_LPM_SHADOW_STC        *lpmDbPtr,tmpLpmDb;
    PRV_CPSS_DXCH_LPM_HW_ENT            lpmHw;
    PRV_CPSS_DXCH_LPM_PROTOCOL_BMP      protocolBitmap;

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
                                          CPSS_CH1_E | CPSS_CH1_DIAMOND_E |
                                          CPSS_CH2_E | CPSS_CH3_E |
                                          CPSS_XCAT_E | CPSS_XCAT3_E | CPSS_LION_E |
                                          CPSS_XCAT2_E | CPSS_LION2_E);
    PRV_CPSS_DXCH_IP_CHECK_PROTOCOL_STACK_MAC(protocol);

    /* run shadow and HW synchronization check, assuming HW validation and shadow validation passed ok */
    tmpLpmDb.lpmDbId = lpmDbId;
    lpmDbPtr = prvCpssSlSearch(prvCpssDxChIplpmDbSL,&tmpLpmDb);
    if (lpmDbPtr == NULL)
    {
        /* can't find the lpm DB */
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_FOUND, LOG_ERROR_NO_MSG);
    }

    lpmHw = prvCpssDxChLpmGetHwType(lpmDbPtr->shadowType);
    switch (lpmHw)
    {
        case PRV_CPSS_DXCH_LPM_HW_TCAM_E:
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_APPLICABLE_DEVICE, LOG_ERROR_NO_MSG);

        case PRV_CPSS_DXCH_LPM_HW_RAM_E:
            prvCpssDxChLpmConvertIpProtocolStackToProtocolBitmap(protocol, &protocolBitmap);
            return prvCpssDxChLpmRamDbgHwShadowSyncValidityCheck(devNum,
                                                                 (PRV_CPSS_DXCH_LPM_RAM_SHADOWS_DB_STC*)(lpmDbPtr->shadow),
                                                                 vrId, protocolBitmap, prefixType, returnOnFailure);

        default:
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }
}

/*******************************************************************************
* cpssDxChIpLpmHwShadowValidityAndSyncCheck
*
* DESCRIPTION:
*   This function Validate the LPM in shadow, Validate the LPM in HW and
*   check synchronization between the shadow and HW of the LPM
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum          - The device number
*       lpmDbId         - The LPM DB id
*       vrId            - The virtual router id
*       protocol        - The IP protocol
*       prefixType      - Unicast or multicast
*       returnOnFailure - whether to return on first failure
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*******************************************************************************/
GT_STATUS cpssDxChIpLpmHwShadowValidityAndSyncCheck
(
    IN GT_U8                            devNum,
    IN GT_U32                           lpmDbId,
    IN GT_U32                           vrId,
    IN CPSS_IP_PROTOCOL_STACK_ENT       protocol,
    IN CPSS_UNICAST_MULTICAST_ENT       prefixType,
    IN GT_BOOL                          returnOnFailure
)
{
    GT_STATUS                           rc=GT_OK;

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
                                          CPSS_CH1_E | CPSS_CH1_DIAMOND_E |
                                          CPSS_CH2_E | CPSS_CH3_E |
                                          CPSS_XCAT_E | CPSS_XCAT3_E | CPSS_LION_E |
                                          CPSS_XCAT2_E | CPSS_LION2_E);
    PRV_CPSS_DXCH_IP_CHECK_PROTOCOL_STACK_MAC(protocol);

    /* run HW validation */
    rc = cpssDxChIpLpmHwValidation(devNum, vrId, protocol, prefixType);
    if (rc != GT_OK)
    {
        cpssOsPrintf("Fail on HW validation\n");
        return rc;
    }
    /* run shadow validation */
    rc = cpssDxChIpLpmShadowValidityCheck(lpmDbId, vrId, protocol, prefixType, returnOnFailure);
    if (rc != GT_OK)
    {
        cpssOsPrintf("Fail on shadow validation\n");
        return rc;
    }

    /* run HW/shadow synchronization */
    rc = cpssDxChIpLpmHwShadowSyncValidityCheck(devNum, lpmDbId, vrId, protocol, prefixType, returnOnFailure);
    if (rc != GT_OK)
    {
        cpssOsPrintf("Fail on HW/shadow synchronization validation\n");
        return rc;
    }

    return rc;
}

/*******************************************************************************
* cpssDxChIpLpmIpv4UcPrefixAddMany
*
* DESCRIPTION:
*       This debug function tries to add many sequential IPv4 Unicast prefixes and
*       returns the number of prefixes successfully added.
*
* APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       None.
*
* INPUTS:
*       lpmDbId               - the LPM DB id
*       vrId                  - the virtual router id
*       startIpAddr           - the first address to add
*       routeEntryBaseMemAddr - base memory Address (offset) of the route entry
*       numOfPrefixesToAdd    - the number of prefixes to add
*
* OUTPUTS:
*       lastIpAddrAddedPtr    - points to the last prefix successfully
*                               added (NULL to ignore)
*       numOfPrefixesAddedPtr - points to the nubmer of prefixes that were
*                               successfully added (NULL to ignore)
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - on wrong devNum or vrId.
*       GT_HW_ERROR              - on Hardware error.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       This debug function is intended to check the number of the prefixes
*       that can be added. All the prefixes are added with exact match (prefix
*       length 32). The route entry is not written.
*
*******************************************************************************/
GT_STATUS cpssDxChIpLpmIpv4UcPrefixAddMany
(
    IN  GT_U32      lpmDbId,
    IN  GT_U32      vrId,
    IN  GT_IPADDR   startIpAddr,
    IN  GT_U32      routeEntryBaseMemAddr,
    IN  GT_U32      numOfPrefixesToAdd,
    OUT GT_IPADDR   *lastIpAddrAddedPtr,
    OUT GT_U32      *numOfPrefixesAddedPtr
)
{
    GT_STATUS                               rc = GT_OK;
    GT_U32                                  i = 0;
    GT_IPADDR                               currentIpAddr;
    GT_U32                                  numOfPrefixesAdded = 0;
    GT_IPADDR                               lastIpAddrAdded;
    CPSS_DXCH_IP_TCAM_ROUTE_ENTRY_INFO_UNT  nextHopInfo;
    GT_U32                                  prefixLength = 32;
    GT_BOOL                                 override = GT_FALSE;

    currentIpAddr.u32Ip                           = startIpAddr.u32Ip;
    lastIpAddrAdded.u32Ip                         = startIpAddr.u32Ip;

    cpssOsMemSet(&nextHopInfo, 0, sizeof(CPSS_DXCH_IP_TCAM_ROUTE_ENTRY_INFO_UNT));
    nextHopInfo.ipLttEntry.routeType           = CPSS_DXCH_IP_ROUTE_ENTRY_METHOD_ECMP_E;
    nextHopInfo.ipLttEntry.routeEntryBaseIndex = routeEntryBaseMemAddr;
    nextHopInfo.ipLttEntry.numOfPaths          = 0;
    nextHopInfo.ipLttEntry.ipv6MCGroupScopeLevel = CPSS_IPV6_PREFIX_SCOPE_LINK_LOCAL_E;
    rc = prvCpssDxChIpLpmDbgConvertRouteType(lpmDbId, &nextHopInfo.ipLttEntry);
    if (rc != GT_OK)
    {
        return rc;
    }

    for (i = 0 ; i < numOfPrefixesToAdd ; i++)
    {
        /* add the current prefix */
        rc = cpssDxChIpLpmIpv4UcPrefixAdd(lpmDbId,vrId,currentIpAddr,
                                          prefixLength,&nextHopInfo,
                                          override);
        if (rc != GT_OK)
            break;

        numOfPrefixesAdded++;
        lastIpAddrAdded.u32Ip = currentIpAddr.u32Ip;

        /* advance to the next IP address */
        if (currentIpAddr.arIP[3] < 255)
            currentIpAddr.arIP[3]++;
        else
        {
            currentIpAddr.arIP[3] = 0;
            if (currentIpAddr.arIP[2] < 255)
                currentIpAddr.arIP[2]++;
            else
            {
                currentIpAddr.arIP[2] = 0;
                if (currentIpAddr.arIP[1] < 255)
                    currentIpAddr.arIP[1]++;
                else
                {
                    currentIpAddr.arIP[1] = 0;
                    if (currentIpAddr.arIP[0] < 255)
                        currentIpAddr.arIP[0]++;
                    else
                    {
                        rc = GT_NO_MORE;
                        break;
                    }
                }
            }
        }
    }

    /* print result */
    cpssOsPrintf("\nIpv4UcPrefixAddMany results:");
    if (numOfPrefixesAdded == 0)
    {
        cpssOsPrintf("\n    No prefixes were added at all.\n");
    }
    else
    {
        cpssOsPrintf("\n    Prefixes ");
        outputIPAddress(startIpAddr.arIP, PRV_CPSS_DXCH_LPM_NUM_OF_OCTETS_IN_IPV4_PROTOCOL_CNS, GT_TRUE);
        cpssOsPrintf(" - ");
        outputIPAddress(lastIpAddrAdded.arIP, PRV_CPSS_DXCH_LPM_NUM_OF_OCTETS_IN_IPV4_PROTOCOL_CNS, GT_TRUE);
        cpssOsPrintf(" were added (%d/%d)\n",numOfPrefixesAdded,numOfPrefixesToAdd);
    }

    /* update output parameters */
    if (numOfPrefixesAddedPtr != NULL)
        *numOfPrefixesAddedPtr = numOfPrefixesAdded;
    if (lastIpAddrAddedPtr != NULL)
        lastIpAddrAddedPtr->u32Ip = lastIpAddrAdded.u32Ip;

    return rc;
}

/*******************************************************************************
* cpssDxChIpLpmIpv4UcPrefixAddManyRandom
*
* DESCRIPTION:
*       This function tries to add many random IPv4 Unicast prefixes and
*       returns the number of prefixes successfully added.
*
* APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       lpmDbId               - the LPM DB id
*       vrId                  - the virtual router id
*       startIpAddr           - the first address to add
*       routeEntryBaseMemAddr - base memory Address (offset) of the route entry
*       numOfPrefixesToAdd    - the number of prefixes to add
*       isWholeIpRandom       - GT_TRUE: all IP octets calculated by cpssOsRand
*                               GT_FALSE: only 2 LSB octets calculated by cpssOsRand
*
* OUTPUTS:
*       numOfPrefixesAddedPtr - points to the nubmer of prefixes that were
*                               successfully added (NULL to ignore)
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - on wrong devNum or vrId.
*       GT_HW_ERROR              - on Hardware error.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       This debug function is intended to check the number of the prefixes
*       that can be added. All the prefixes are added with exact match (prefix
*       length 32). The route entry is not written.
*
*******************************************************************************/
GT_STATUS cpssDxChIpLpmIpv4UcPrefixAddManyRandom
(
    IN  GT_U32      lpmDbId,
    IN  GT_U32      vrId,
    IN  GT_IPADDR   startIpAddr,
    IN  GT_U32      routeEntryBaseMemAddr,
    IN  GT_U32      numOfPrefixesToAdd,
    IN  GT_BOOL     isWholeIpRandom,
    OUT GT_U32      *numOfPrefixesAddedPtr
)
{
    GT_STATUS                               rc = GT_OK;
    GT_U32                                  i = 0;
    GT_IPADDR                               currentIpAddr;
    GT_U32                                  numOfPrefixesAdded = 0;
    CPSS_DXCH_IP_TCAM_ROUTE_ENTRY_INFO_UNT  nextHopInfo;
    GT_U32                                  prefixLength = 32;
    GT_BOOL                                 override = GT_FALSE;
    GT_U32                                  counter = 0;
    GT_U32                                  maxCounterValue = 0;

    cpssOsMemSet(&nextHopInfo, 0, sizeof(CPSS_DXCH_IP_TCAM_ROUTE_ENTRY_INFO_UNT));
    nextHopInfo.ipLttEntry.routeType           = CPSS_DXCH_IP_ROUTE_ENTRY_METHOD_ECMP_E;
    nextHopInfo.ipLttEntry.routeEntryBaseIndex = routeEntryBaseMemAddr;
    nextHopInfo.ipLttEntry.numOfPaths          = 0;
    nextHopInfo.ipLttEntry.ipv6MCGroupScopeLevel = CPSS_IPV6_PREFIX_SCOPE_LINK_LOCAL_E;
    rc = prvCpssDxChIpLpmDbgConvertRouteType(lpmDbId, &nextHopInfo.ipLttEntry);
    if (rc != GT_OK)
    {
        return rc;
    }

    currentIpAddr.u32Ip                     = startIpAddr.u32Ip;
    currentIpAddr.arIP[3] = (GT_U8)(cpssOsRand() % 256);
    currentIpAddr.arIP[2]= (GT_U8)(cpssOsRand() % 256);

    if (isWholeIpRandom == GT_TRUE)
    {
        currentIpAddr.arIP[1] = (GT_U8)(cpssOsRand() % 256);
        currentIpAddr.arIP[0]= (GT_U8)(cpssOsRand() % 224);
    }
    for (i = 0 ; i < numOfPrefixesToAdd ; i++)
    {
        rc = 100;
        counter = 0;
        while (rc != GT_OK)
        {
            /* add the current prefix */
            rc = cpssDxChIpLpmIpv4UcPrefixAdd(lpmDbId,vrId,currentIpAddr,
                                          prefixLength,&nextHopInfo,
                                          override);
            if (rc != GT_OK)
            {
                if (rc == GT_ALREADY_EXIST)
                {
                    if (counter > 20000)
                        break;
                    currentIpAddr.arIP[3] = (GT_U8)(cpssOsRand() % 256);
                    currentIpAddr.arIP[2]= (GT_U8)(cpssOsRand() % 256);

                    if (isWholeIpRandom == GT_TRUE)
                    {
                        currentIpAddr.arIP[1] = (GT_U8)(cpssOsRand() % 256);
                        currentIpAddr.arIP[0]= (GT_U8)(cpssOsRand() % 224);
                    }
                    counter++;
                    if (counter > maxCounterValue)
                        maxCounterValue = counter;
                }
                else
                {
                    break;
                }
            }
        }
        if (rc != GT_OK)
        {
            break;
        }
        numOfPrefixesAdded++;
        if ((numOfPrefixesAdded % 50) == 0 )
        {
            currentIpAddr.arIP[1] = (GT_U8)(cpssOsRand() % 256);
            currentIpAddr.arIP[0]= (GT_U8)(cpssOsRand() % 224);
        }
        currentIpAddr.arIP[3] = (GT_U8)(cpssOsRand() % 256);
        currentIpAddr.arIP[2]= (GT_U8)(cpssOsRand() % 256);
        if (isWholeIpRandom == GT_TRUE)
        {
            currentIpAddr.arIP[1] = (GT_U8)(cpssOsRand() % 256);
            currentIpAddr.arIP[0]= (GT_U8)(cpssOsRand() % 224);
        }
    }
    /* print result */
    cpssOsPrintf("\nIpv4UcPrefixAddManyRandom results:");
    if (rc != GT_OK)
    {
        cpssOsPrintf("\n error = %x occured on prefix\n",rc);
        outputIPAddress(currentIpAddr.arIP, PRV_CPSS_DXCH_LPM_NUM_OF_OCTETS_IN_IPV4_PROTOCOL_CNS, GT_TRUE);
    }

    /* update output parameters */
    if (numOfPrefixesAddedPtr != NULL)
        *numOfPrefixesAddedPtr = numOfPrefixesAdded;

    if (numOfPrefixesAdded == 0)
    {
        cpssOsPrintf("\n    No prefixes were added at all.\n");
    }
    else
    {
        cpssOsPrintf(" were added %d prefixes from %d prefixes)\n",numOfPrefixesAdded,numOfPrefixesToAdd);
    }
    cpssOsPrintf("maxCounterValue =%d\n",maxCounterValue);
    return rc;
}

/*******************************************************************************
* cpssDxChIpLpmIpv4UcPrefixAddManyByOctet
*
* DESCRIPTION:
*       This function tries to add many sequential IPv4 Unicast prefixes according
*       to octet order selection and returns the number of prefixes successfully added.
*
* APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       lpmDbId               - the LPM DB id
*       vrId                  - the virtual router id
*       startIpAddr           - the first address to add
*       routeEntryBaseMemAddr - base memory Address (offset) of the route entry
*       numOfPrefixesToAdd    - the number of prefixes to add
*       firstOctetToChange    - first octet to change in the prefix address.
*                               value of the octet can be up to 255
*       secondOctetToChange   - second octet to change in the prefix address.
*                               value of the octet can be up to 255
*       thirdOctetToChange    - third octet to change in the prefix address.
*                               value of the octet can be up to 255
*       forthOctetToChange    - forth octet to change in the prefix address.
*                               value of the octet can be up to 255
*
* OUTPUTS:
*       lastIpAddrAddedPtr    - points to the last prefix successfully
*                               added (NULL to ignore)
*       numOfPrefixesAddedPtr - points to the nubmer of prefixes that were
*                               successfully added (NULL to ignore)
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - on wrong devNum or vrId.
*       GT_HW_ERROR              - on Hardware error.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       This debug function is intended to check the number of the prefixes
*       that can be added. All the prefixes are added with exact match (prefix
*       length 32). The route entry is not written.
*
*******************************************************************************/
GT_STATUS cpssDxChIpLpmIpv4UcPrefixAddManyByOctet
(
    IN  GT_U32      lpmDbId,
    IN  GT_U32      vrId,
    IN  GT_IPADDR   startIpAddr,
    IN  GT_U32      routeEntryBaseMemAddr,
    IN  GT_U32      numOfPrefixesToAdd,
    IN  GT_U32      firstOctetToChange,
    IN  GT_U32      secondOctetToChange,
    IN  GT_U32      thirdOctetToChange,
    IN  GT_U32      forthOctetToChange,
    OUT GT_IPADDR   *lastIpAddrAddedPtr,
    OUT GT_U32      *numOfPrefixesAddedPtr
)
{
    GT_STATUS                               rc = GT_OK;
    GT_U32                                  i = 0;
    GT_IPADDR                               currentIpAddr;
    GT_U32                                  numOfPrefixesAdded = 0;
    GT_U32                                  numOfMcPrefixesNotAdded = 0;
    GT_IPADDR                               lastIpAddrAdded;
    CPSS_DXCH_IP_TCAM_ROUTE_ENTRY_INFO_UNT  nextHopInfo;
    GT_U32                                  prefixLength = 32;
    GT_BOOL                                 override = GT_FALSE;
    GT_BOOL                                 octetsArray[PRV_CPSS_DXCH_LPM_NUM_OF_OCTETS_IN_IPV4_PROTOCOL_CNS]={GT_FALSE,GT_FALSE,GT_FALSE,GT_FALSE};/* array to hold the octets already selected */

    currentIpAddr.u32Ip                     = startIpAddr.u32Ip;
    lastIpAddrAdded.u32Ip                   = startIpAddr.u32Ip;

    cpssOsMemSet(&nextHopInfo, 0, sizeof(CPSS_DXCH_IP_TCAM_ROUTE_ENTRY_INFO_UNT));
    nextHopInfo.ipLttEntry.routeType           = CPSS_DXCH_IP_ROUTE_ENTRY_METHOD_ECMP_E;
    nextHopInfo.ipLttEntry.routeEntryBaseIndex = routeEntryBaseMemAddr;
    nextHopInfo.ipLttEntry.numOfPaths          = 0;
    nextHopInfo.ipLttEntry.ipv6MCGroupScopeLevel = CPSS_IPV6_PREFIX_SCOPE_LINK_LOCAL_E;
    rc = prvCpssDxChIpLpmDbgConvertRouteType(lpmDbId, &nextHopInfo.ipLttEntry);
    if (rc != GT_OK)
    {
        return rc;
    }

    if((firstOctetToChange>3) || (secondOctetToChange>3) || (thirdOctetToChange>3) || (forthOctetToChange>3))
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }
    /* make sure octet are not selected twice */
    if(octetsArray[firstOctetToChange]==GT_FALSE)
    {
        octetsArray[firstOctetToChange]=GT_TRUE;
    }
    else
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }
    if(octetsArray[secondOctetToChange]==GT_FALSE)
    {
        octetsArray[secondOctetToChange]=GT_TRUE;
    }
    else
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }
    if(octetsArray[thirdOctetToChange]==GT_FALSE)
    {
        octetsArray[thirdOctetToChange]=GT_TRUE;
    }
    else
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }
    if(octetsArray[forthOctetToChange]==GT_FALSE)
    {
        octetsArray[forthOctetToChange]=GT_TRUE;
    }
    else
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }
    for (i = 0 ; i < numOfPrefixesToAdd ; )
    {
        /* make sure the prefix is not a MC prefix - not in the range 224/4 - 239/4
           therefore the first octate must not start with 0xE */
        if((currentIpAddr.arIP[0] & 0xF0) != 0xE0)
        {
            /* add the current prefix */
            rc = cpssDxChIpLpmIpv4UcPrefixAdd(lpmDbId,vrId,currentIpAddr,
                                              prefixLength,&nextHopInfo,
                                              override);
            if (rc != GT_OK)
                break;

            numOfPrefixesAdded++;
            i++;
        }
        else
        {
            numOfMcPrefixesNotAdded++;
        }


        lastIpAddrAdded.u32Ip = currentIpAddr.u32Ip;

        /* advance to the next IP address */
        if (currentIpAddr.arIP[firstOctetToChange] < 255)
            currentIpAddr.arIP[firstOctetToChange]++;
        else
        {
            currentIpAddr.arIP[firstOctetToChange] = 0;
            if (currentIpAddr.arIP[secondOctetToChange] < 255)
                currentIpAddr.arIP[secondOctetToChange]++;
            else
            {
                currentIpAddr.arIP[secondOctetToChange] = 0;
                if (currentIpAddr.arIP[thirdOctetToChange] < 255)
                    currentIpAddr.arIP[thirdOctetToChange]++;
                else
                {
                    currentIpAddr.arIP[thirdOctetToChange] = 0;
                    if (currentIpAddr.arIP[forthOctetToChange] < 255)
                        currentIpAddr.arIP[forthOctetToChange]++;
                    else
                    {
                        rc = GT_NO_MORE;
                        break;
                    }
                }
            }
        }
    }

    /* print result */
    cpssOsPrintf("\nIpv4UcPrefixAddMany results:");
    if (numOfPrefixesAdded == 0)
    {
        cpssOsPrintf("\n    No prefixes were added at all.\n");
    }
    else
    {
        cpssOsPrintf("\n    Prefixes ");
        outputIPAddress(startIpAddr.arIP, PRV_CPSS_DXCH_LPM_NUM_OF_OCTETS_IN_IPV4_PROTOCOL_CNS, GT_TRUE);
        cpssOsPrintf(" - ");
        outputIPAddress(lastIpAddrAdded.arIP, PRV_CPSS_DXCH_LPM_NUM_OF_OCTETS_IN_IPV4_PROTOCOL_CNS, GT_TRUE);
        cpssOsPrintf(" were added (%d/%d)\n",numOfPrefixesAdded,numOfPrefixesToAdd);
        cpssOsPrintf("    (%d) MC prefixes not added in this range\n",numOfMcPrefixesNotAdded);
    }

    /* update output parameters */
    if (numOfPrefixesAddedPtr != NULL)
        *numOfPrefixesAddedPtr = numOfPrefixesAdded;
    if (lastIpAddrAddedPtr != NULL)
        lastIpAddrAddedPtr->u32Ip = lastIpAddrAdded.u32Ip;

    return rc;
}

/*******************************************************************************
* cpssDxChIpLpmIpv4UcPrefixDelManyByOctet
*
* DESCRIPTION:
*       This function tries to delete many sequential IPv4 Unicast prefixes according
*       to octet order selection and returns the number of prefixes successfully deleted.
*
* APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       lpmDbId               - the LPM DB id
*       vrId                  - the virtual router id
*       startIpAddr           - the first address to add
*       numOfPrefixesToDel    - the number of prefixes to delete
*       firstOctetToChange    - first octet to change in the prefix address.
*                               value of the octet can be up to 255
*       secondOctetToChange   - second octet to change in the prefix address.
*                               value of the octet can be up to 255
*       thirdOctetToChange    - third octet to change in the prefix address.
*                               value of the octet can be up to 255
*       forthOctetToChange    - forth octet to change in the prefix address.
*                               value of the octet can be up to 255
*
* OUTPUTS:
*       lastIpAddrDeletedPtr    - points to the last prefix successfully
*                                 deleted (NULL to ignore)
*       numOfPrefixesDeletedPtr - points to the nubmer of prefixes that were
*                                 successfully deleted (NULL to ignore)
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - on wrong devNum or vrId.
*       GT_HW_ERROR              - on Hardware error.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       This debug function is intended to delete the number of the prefixes
*       that was added by cpssDxChIpLpmIpv4UcPrefixAddManyByOctet.
*
*******************************************************************************/
GT_STATUS cpssDxChIpLpmIpv4UcPrefixDelManyByOctet
(
    IN  GT_U32      lpmDbId,
    IN  GT_U32      vrId,
    IN  GT_IPADDR   startIpAddr,
    IN  GT_U32      numOfPrefixesToDel,
    IN  GT_U32      firstOctetToChange,
    IN  GT_U32      secondOctetToChange,
    IN  GT_U32      thirdOctetToChange,
    IN  GT_U32      forthOctetToChange,
    OUT GT_IPADDR   *lastIpAddrDeletedPtr,
    OUT GT_U32      *numOfPrefixesDeletedPtr
)
{
    GT_STATUS                               rc = GT_OK;
    GT_U32                                  i = 0;
    GT_IPADDR                               currentIpAddr;
    GT_U32                                  numOfPrefixesDeleted = 0;
    GT_U32                                  numOfMcPrefixesNotDeleted = 0;
    GT_IPADDR                               lastIpAddrDeleted;
    GT_U32                                  prefixLength = 32;
    GT_BOOL                                 octetsArray[PRV_CPSS_DXCH_LPM_NUM_OF_OCTETS_IN_IPV4_PROTOCOL_CNS]={GT_FALSE,GT_FALSE,GT_FALSE,GT_FALSE};/* array to hold the octets already selected */

    currentIpAddr.u32Ip                     = startIpAddr.u32Ip;
    lastIpAddrDeleted.u32Ip                 = startIpAddr.u32Ip;

    if((firstOctetToChange>3) || (secondOctetToChange>3) || (thirdOctetToChange>3) || (forthOctetToChange>3))
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }
    /* make sure octet are not selected twice */
    if(octetsArray[firstOctetToChange]==GT_FALSE)
    {
        octetsArray[firstOctetToChange]=GT_TRUE;
    }
    else
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }
    if(octetsArray[secondOctetToChange]==GT_FALSE)
    {
        octetsArray[secondOctetToChange]=GT_TRUE;
    }
    else
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }
    if(octetsArray[thirdOctetToChange]==GT_FALSE)
    {
        octetsArray[thirdOctetToChange]=GT_TRUE;
    }
    else
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }
    if(octetsArray[forthOctetToChange]==GT_FALSE)
    {
        octetsArray[forthOctetToChange]=GT_TRUE;
    }
    else
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }
    for (i = 0 ; i < numOfPrefixesToDel ; )
    {
        /* make sure the prefix is not a MC prefix - not in the range 224/4 - 239/4
           therefore the first octate must not start with 0xE */
        if((currentIpAddr.arIP[0] & 0xF0) != 0xE0)
        {
            /* delete the current prefix */
            rc = cpssDxChIpLpmIpv4UcPrefixDel(lpmDbId,vrId,currentIpAddr,prefixLength);
            if (rc != GT_OK)
                break;

            numOfPrefixesDeleted++;
            i++;
        }
        else
        {
            numOfMcPrefixesNotDeleted++;
        }


        lastIpAddrDeleted.u32Ip = currentIpAddr.u32Ip;

        /* advance to the next IP address */
        if (currentIpAddr.arIP[firstOctetToChange] < 255)
            currentIpAddr.arIP[firstOctetToChange]++;
        else
        {
            currentIpAddr.arIP[firstOctetToChange] = 0;
            if (currentIpAddr.arIP[secondOctetToChange] < 255)
                currentIpAddr.arIP[secondOctetToChange]++;
            else
            {
                currentIpAddr.arIP[secondOctetToChange] = 0;
                if (currentIpAddr.arIP[thirdOctetToChange] < 255)
                    currentIpAddr.arIP[thirdOctetToChange]++;
                else
                {
                    currentIpAddr.arIP[thirdOctetToChange] = 0;
                    if (currentIpAddr.arIP[forthOctetToChange] < 255)
                        currentIpAddr.arIP[forthOctetToChange]++;
                    else
                    {
                        rc = GT_NO_MORE;
                        break;
                    }
                }
            }
        }
    }

    /* print result */
    cpssOsPrintf("\nIpv4UcPrefixDelMany results:");
    if (numOfPrefixesDeleted == 0)
    {
        cpssOsPrintf("\n    No prefixes were Deleted at all.\n");
    }
    else
    {
        cpssOsPrintf("\n    Prefixes ");
        outputIPAddress(startIpAddr.arIP, PRV_CPSS_DXCH_LPM_NUM_OF_OCTETS_IN_IPV4_PROTOCOL_CNS, GT_TRUE);
        cpssOsPrintf(" - ");
        outputIPAddress(lastIpAddrDeleted.arIP, PRV_CPSS_DXCH_LPM_NUM_OF_OCTETS_IN_IPV4_PROTOCOL_CNS, GT_TRUE);
        cpssOsPrintf(" were Deleted (%d/%d)\n",numOfPrefixesDeleted,numOfPrefixesToDel);
        cpssOsPrintf("    (%d) MC prefixes not Deleted in this range\n",numOfMcPrefixesNotDeleted);
    }

    /* update output parameters */
    if (numOfPrefixesDeletedPtr != NULL)
        *numOfPrefixesDeletedPtr = numOfPrefixesDeleted;
    if (lastIpAddrDeletedPtr != NULL)
        lastIpAddrDeletedPtr->u32Ip = lastIpAddrDeleted.u32Ip;

    return rc;
}
/*******************************************************************************
* cpssDxChIpLpmIpv4UcPrefixGetManyByOctet
*
* DESCRIPTION:
*       This function tries to get many sequential IPv4 Unicast prefixes (that were
*       added by cpssDxChIpLpmIpv4UcPrefixAddManyByOctet) and returns the number of
*       successfully retrieved prefixes.
*
* APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       lpmDbId               - the LPM DB id
*       vrId                  - the virtual router id
*       startIpAddr           - the first address to add
*       numOfPrefixesToGet    - the number of prefixes to get
*       firstOctetToChange    - first octet to change in the prefix address.
*                               value of the octet can be up to 255
*       secondOctetToChange   - second octet to change in the prefix address.
*                               value of the octet can be up to 255
*       thirdOctetToChange    - third octet to change in the prefix address.
*                               value of the octet can be up to 255
*       forthOctetToChange    - forth octet to change in the prefix address.
*                               value of the octet can be up to 255
*
* OUTPUTS:
*       numOfRetrievedPrefixesPtr - points to the nubmer of prefixes that were
*                                   successfully retrieved (NULL to ignore)
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - on wrong devNum or vrId.
*       GT_HW_ERROR              - on Hardware error.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       This debug function is intended to check the number of the prefixes
*       that were be added.
*******************************************************************************/
GT_STATUS cpssDxChIpLpmIpv4UcPrefixGetManyByOctet
(
    IN  GT_U32      lpmDbId,
    IN  GT_U32      vrId,
    IN  GT_IPADDR   startIpAddr,
    IN  GT_U32      numOfPrefixesToGet,
    IN  GT_U32      firstOctetToChange,
    IN  GT_U32      secondOctetToChange,
    IN  GT_U32      thirdOctetToChange,
    IN  GT_U32      forthOctetToChange,
    OUT GT_U32      *numOfRetrievedPrefixesPtr
)
{
    GT_STATUS                               rc = GT_OK;
    GT_U32                                  i = 0;
    GT_IPADDR                               currentIpAddr;
    GT_U32                                  numOfRetrievedPrefixes = 0;
    GT_U32                                  numOfMcPrefixesNotAdded = 0;
    GT_IPADDR                               lastIpAddrAdded;
    CPSS_DXCH_IP_TCAM_ROUTE_ENTRY_INFO_UNT  nextHopInfo;
    GT_U32                                  prefixLength = 32;
    GT_BOOL                                 octetsArray[PRV_CPSS_DXCH_LPM_NUM_OF_OCTETS_IN_IPV4_PROTOCOL_CNS]={GT_FALSE,GT_FALSE,GT_FALSE,GT_FALSE};/* array to hold the octets already selected */
    GT_U32                                  tcamRowIndex=0;
    GT_U32                                  tcamColumnIndex=0;

    currentIpAddr.u32Ip                     = startIpAddr.u32Ip;
    lastIpAddrAdded.u32Ip                   = startIpAddr.u32Ip;

    cpssOsMemSet(&nextHopInfo, 0, sizeof(CPSS_DXCH_IP_TCAM_ROUTE_ENTRY_INFO_UNT));

    if((firstOctetToChange>3) || (secondOctetToChange>3) || (thirdOctetToChange>3) || (forthOctetToChange>3))
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }
    /* make sure octet are not selected twice */
    if(octetsArray[firstOctetToChange]==GT_FALSE)
    {
        octetsArray[firstOctetToChange]=GT_TRUE;
    }
    else
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }
    if(octetsArray[secondOctetToChange]==GT_FALSE)
    {
        octetsArray[secondOctetToChange]=GT_TRUE;
    }
    else
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }
    if(octetsArray[thirdOctetToChange]==GT_FALSE)
    {
        octetsArray[thirdOctetToChange]=GT_TRUE;
    }
    else
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }
    if(octetsArray[forthOctetToChange]==GT_FALSE)
    {
        octetsArray[forthOctetToChange]=GT_TRUE;
    }
    else
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }
    for (i = 0 ; i < numOfPrefixesToGet ; )
    {
        /* make sure the prefix is not a MC prefix - not in the range 224/4 - 239/4
           therefore the first octate must not start with 0xE */
        if((currentIpAddr.arIP[0] & 0xF0) != 0xE0)
        {
            /* get the current prefix */
            rc =  cpssDxChIpLpmIpv4UcPrefixSearch(lpmDbId,
                                                 vrId,
                                                 currentIpAddr,
                                                 prefixLength,
                                                 &nextHopInfo,
                                                 &tcamRowIndex,
                                                 &tcamColumnIndex);
            if (rc != GT_OK)
                break;

            numOfRetrievedPrefixes++;
            i++;
        }
        else
        {
            numOfMcPrefixesNotAdded++;
        }

        lastIpAddrAdded.u32Ip = currentIpAddr.u32Ip;

        /* advance to the next IP address */
        if (currentIpAddr.arIP[firstOctetToChange] < 255)
            currentIpAddr.arIP[firstOctetToChange]++;
        else
        {
            currentIpAddr.arIP[firstOctetToChange] = 0;
            if (currentIpAddr.arIP[secondOctetToChange] < 255)
                currentIpAddr.arIP[secondOctetToChange]++;
            else
            {
                currentIpAddr.arIP[secondOctetToChange] = 0;
                if (currentIpAddr.arIP[thirdOctetToChange] < 255)
                    currentIpAddr.arIP[thirdOctetToChange]++;
                else
                {
                    currentIpAddr.arIP[thirdOctetToChange] = 0;
                    if (currentIpAddr.arIP[forthOctetToChange] < 255)
                        currentIpAddr.arIP[forthOctetToChange]++;
                    else
                    {
                        rc = GT_NO_MORE;
                        break;
                    }
                }
            }
        }
    }

    /* print result */
    cpssOsPrintf("\nIpv4UcPrefixGetMany results:");
    if (numOfRetrievedPrefixes == 0)
    {
        cpssOsPrintf("\n    No prefixes at all.\n");
    }
    else
    {
        cpssOsPrintf("\n    Prefixes ");
        outputIPAddress(startIpAddr.arIP, PRV_CPSS_DXCH_LPM_NUM_OF_OCTETS_IN_IPV4_PROTOCOL_CNS, GT_TRUE);
        cpssOsPrintf(" - ");
        outputIPAddress(lastIpAddrAdded.arIP, PRV_CPSS_DXCH_LPM_NUM_OF_OCTETS_IN_IPV4_PROTOCOL_CNS, GT_TRUE);
        cpssOsPrintf(" were retrieved (%d/%d)\n",numOfRetrievedPrefixes,numOfPrefixesToGet);
        cpssOsPrintf("    (%d) MC prefixes not added in this range\n",numOfMcPrefixesNotAdded);
    }

    /* update output parameters */
    if (numOfRetrievedPrefixesPtr != NULL)
        *numOfRetrievedPrefixesPtr = numOfRetrievedPrefixes;
    return rc;
}

/*******************************************************************************
* cpssDxChIpLpmIpv6UcPrefixAddManyByOctet
*
* DESCRIPTION:
*       This function tries to add many sequential IPv6 Unicast prefixes according
*       to octet order selection and returns the number of prefixes successfully added.
*
* APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       lpmDbId               - the LPM DB id
*       vrId                  - the virtual router id
*       startIpAddr           - the first address to add
*       routeEntryBaseMemAddr - base memory Address (offset) of the route entry
*       numOfPrefixesToAdd    - the number of prefixes to add
*       firstOctetToChange    - first octet to change in the prefix address.
*                               value of the octet can be up to 255
*       secondOctetToChange   - second octet to change in the prefix address.
*                               value of the octet can be up to 255
*       thirdOctetToChange    - third octet to change in the prefix address.
*                               value of the octet can be up to 255
*       forthOctetToChange    - forth octet to change in the prefix address.
*                               value of the octet can be up to 255
*       fifthOctetToChange    - fifth octet to change in the prefix address.
*                               value of the octet can be up to 255
*       sixthOctetToChange    - sixth octet to change in the prefix address.
*                               value of the octet can be up to 255
*       seventhOctetToChange  - seventh octet to change in the prefix address.
*                               value of the octet can be up to 255
*       eighthOctetToChange   - eighth octet to change in the prefix address.
*                               value of the octet can be up to 255
*       ninthOctetToChange    - ninth octet to change in the prefix address.
*                               value of the octet can be up to 255
*       tenthOctetToChange    - tenth octet to change in the prefix address.
*                               value of the octet can be up to 255
*       elevenOctetToChange   - eleven octet to change in the prefix address.
*                               value of the octet can be up to 255
*       twelveOctetToChange   - twelve octet to change in the prefix address.
*                               value of the octet can be up to 255
*       thirteenOctetToChange - thirteen octet to change in the prefix address.
*                               value of the octet can be up to 255
*       fourteenOctetToChange - fourteen octet to change in the prefix address.
*                               value of the octet can be up to 255
*       fifteenOctetToChange  - fifteen octet to change in the prefix address.
*                               value of the octet can be up to 255
*       sixteenOctetToChange  - sixteen octet to change in the prefix address.
*                               value of the octet can be up to 255
*
* OUTPUTS:
*       lastIpAddrAddedPtr    - points to the last prefix successfully
*                               added (NULL to ignore)
*       numOfPrefixesAddedPtr - points to the nubmer of prefixes that were
*                               successfully added (NULL to ignore)
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - on wrong devNum or vrId.
*       GT_HW_ERROR              - on Hardware error.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       This debug function is intended to check the number of the prefixes
*       that can be added. All the prefixes are added with exact match (prefix
*       length 128). The route entry is not written.
*
*******************************************************************************/
GT_STATUS cpssDxChIpLpmIpv6UcPrefixAddManyByOctet
(
    IN  GT_U32      lpmDbId,
    IN  GT_U32      vrId,
    IN  GT_IPV6ADDR startIpAddr,
    IN  GT_U32      routeEntryBaseMemAddr,
    IN  GT_U32      numOfPrefixesToAdd,
    IN  GT_U32      firstOctetToChange,
    IN  GT_U32      secondOctetToChange,
    IN  GT_U32      thirdOctetToChange,
    IN  GT_U32      forthOctetToChange,
    IN  GT_U32      fifthOctetToChange,
    IN  GT_U32      sixthOctetToChange,
    IN  GT_U32      seventhOctetToChange,
    IN  GT_U32      eighthOctetToChange,
    IN  GT_U32      ninthOctetToChange,
    IN  GT_U32      tenthOctetToChange,
    IN  GT_U32      elevenOctetToChange,
    IN  GT_U32      twelveOctetToChange,
    IN  GT_U32      thirteenOctetToChange,
    IN  GT_U32      fourteenOctetToChange,
    IN  GT_U32      fifteenOctetToChange,
    IN  GT_U32      sixteenOctetToChange,
    OUT GT_IPV6ADDR *lastIpAddrAddedPtr,
    OUT GT_U32      *numOfPrefixesAddedPtr
)
{
    GT_STATUS                               rc = GT_OK;
    GT_U32                                  i = 0;
    GT_U32                                  j = 0;
    GT_IPV6ADDR                             currentIpAddr;
    GT_U32                                  numOfPrefixesAdded = 0;
    GT_U32                                  numOfMcPrefixesNotAdded = 0;
    GT_IPV6ADDR                             lastIpAddrAdded;
    CPSS_DXCH_IP_TCAM_ROUTE_ENTRY_INFO_UNT  nextHopInfo;
    GT_U32                                  prefixLength = 128;
    GT_BOOL                                 override = GT_FALSE;
    GT_BOOL                                 octetsArray[PRV_CPSS_DXCH_LPM_NUM_OF_OCTETS_IN_IPV6_PROTOCOL_CNS]={GT_FALSE,GT_FALSE,GT_FALSE,GT_FALSE,
                                                            GT_FALSE,GT_FALSE,GT_FALSE,GT_FALSE,
                                                            GT_FALSE,GT_FALSE,GT_FALSE,GT_FALSE,
                                                            GT_FALSE,GT_FALSE,GT_FALSE,GT_FALSE};/* array to hold the octets already selected */

    GT_U32                                  octetsOrderToChangeArray[PRV_CPSS_DXCH_LPM_NUM_OF_OCTETS_IN_IPV6_PROTOCOL_CNS];
    GT_BOOL                                 tcamDefragmentationEnable=GT_FALSE;


    cpssOsMemSet(&nextHopInfo, 0, sizeof(CPSS_DXCH_IP_TCAM_ROUTE_ENTRY_INFO_UNT));
    nextHopInfo.ipLttEntry.routeType           = CPSS_DXCH_IP_ROUTE_ENTRY_METHOD_ECMP_E;
    nextHopInfo.ipLttEntry.routeEntryBaseIndex = routeEntryBaseMemAddr;
    nextHopInfo.ipLttEntry.numOfPaths          = 0;
    nextHopInfo.ipLttEntry.ipv6MCGroupScopeLevel = CPSS_IPV6_PREFIX_SCOPE_LINK_LOCAL_E;
    rc = prvCpssDxChIpLpmDbgConvertRouteType(lpmDbId, &nextHopInfo.ipLttEntry);
    if (rc != GT_OK)
    {
        return rc;
    }

    cpssOsMemSet(&currentIpAddr, 0, sizeof(GT_IPV6ADDR));

    for(j=0;j<4;j++)
    {
        currentIpAddr.u32Ip[j]     = startIpAddr.u32Ip[j];
        lastIpAddrAdded.u32Ip[j]    = startIpAddr.u32Ip[j];
    }


    octetsOrderToChangeArray[0] = firstOctetToChange;
    octetsOrderToChangeArray[1] = secondOctetToChange;
    octetsOrderToChangeArray[2] = thirdOctetToChange;
    octetsOrderToChangeArray[3] = forthOctetToChange;
    octetsOrderToChangeArray[4] = fifthOctetToChange;
    octetsOrderToChangeArray[5] = sixthOctetToChange;
    octetsOrderToChangeArray[6] = seventhOctetToChange;
    octetsOrderToChangeArray[7] = eighthOctetToChange;
    octetsOrderToChangeArray[8] = ninthOctetToChange;
    octetsOrderToChangeArray[9] = tenthOctetToChange;
    octetsOrderToChangeArray[10] = elevenOctetToChange;
    octetsOrderToChangeArray[11] = twelveOctetToChange;
    octetsOrderToChangeArray[12] = thirteenOctetToChange;
    octetsOrderToChangeArray[13] = fourteenOctetToChange;
    octetsOrderToChangeArray[14] = fifteenOctetToChange;
    octetsOrderToChangeArray[15] = sixteenOctetToChange;

    for(j=0;j<PRV_CPSS_DXCH_LPM_NUM_OF_OCTETS_IN_IPV6_PROTOCOL_CNS;j++)
    {
        if(octetsOrderToChangeArray[j]>15)
        {
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
        }
    }

    for(j=0;j<PRV_CPSS_DXCH_LPM_NUM_OF_OCTETS_IN_IPV6_PROTOCOL_CNS;j++)
    {
        /* make sure octet are not selected twice */
        if(octetsArray[octetsOrderToChangeArray[j]]==GT_FALSE)
        {
            octetsArray[octetsOrderToChangeArray[j]]=GT_TRUE;
        }
        else
        {
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
        }
    }


    for (i = 0 ; i < numOfPrefixesToAdd ; )
    {
        /* make sure the prefix is not a MC prefix - not in format ff00::/8
           therefore the first octate must not start with 0xFF */
        if((currentIpAddr.arIP[0]) != 0xFF)
        {

            /* add the current prefix */
            rc = cpssDxChIpLpmIpv6UcPrefixAdd(lpmDbId,
                                              vrId,
                                              currentIpAddr,
                                              prefixLength,
                                              &nextHopInfo,
                                              override,
                                              tcamDefragmentationEnable);
            if (rc != GT_OK)
                break;

            numOfPrefixesAdded++;
            i++;
        }
        else
        {
            numOfMcPrefixesNotAdded++;
        }

        for(j=0;j<4;j++)
        {
            lastIpAddrAdded.u32Ip[j]    = currentIpAddr.u32Ip[j];
        }

        /* advance to the next IP address */
        if (currentIpAddr.arIP[firstOctetToChange] < 255)
            currentIpAddr.arIP[firstOctetToChange]++;
        else
        {
            currentIpAddr.arIP[firstOctetToChange] = 0;
            if (currentIpAddr.arIP[secondOctetToChange] < 255)
                currentIpAddr.arIP[secondOctetToChange]++;
            else
            {
                currentIpAddr.arIP[secondOctetToChange] = 0;
                if (currentIpAddr.arIP[thirdOctetToChange] < 255)
                    currentIpAddr.arIP[thirdOctetToChange]++;
                else
                {
                    currentIpAddr.arIP[thirdOctetToChange] = 0;
                    if (currentIpAddr.arIP[forthOctetToChange] < 255)
                        currentIpAddr.arIP[forthOctetToChange]++;
                    else
                    {
                        currentIpAddr.arIP[forthOctetToChange] = 0;
                        if (currentIpAddr.arIP[fifthOctetToChange] < 255)
                            currentIpAddr.arIP[fifthOctetToChange]++;
                        else
                        {
                            currentIpAddr.arIP[fifthOctetToChange] = 0;
                            if (currentIpAddr.arIP[sixthOctetToChange] < 255)
                                currentIpAddr.arIP[sixthOctetToChange]++;
                            else
                            {
                                currentIpAddr.arIP[sixthOctetToChange] = 0;
                                if (currentIpAddr.arIP[seventhOctetToChange] < 255)
                                    currentIpAddr.arIP[seventhOctetToChange]++;
                                else
                                {
                                    currentIpAddr.arIP[seventhOctetToChange] = 0;
                                    if (currentIpAddr.arIP[eighthOctetToChange] < 255)
                                        currentIpAddr.arIP[eighthOctetToChange]++;
                                    else
                                    {
                                        currentIpAddr.arIP[eighthOctetToChange] = 0;
                                        if (currentIpAddr.arIP[ninthOctetToChange] < 255)
                                            currentIpAddr.arIP[ninthOctetToChange]++;
                                        else
                                        {
                                            currentIpAddr.arIP[ninthOctetToChange] = 0;
                                            if (currentIpAddr.arIP[tenthOctetToChange] < 255)
                                                currentIpAddr.arIP[tenthOctetToChange]++;
                                            else
                                            {
                                                currentIpAddr.arIP[tenthOctetToChange] = 0;
                                                if (currentIpAddr.arIP[elevenOctetToChange] < 255)
                                                    currentIpAddr.arIP[elevenOctetToChange]++;
                                                else
                                                {
                                                    currentIpAddr.arIP[elevenOctetToChange] = 0;
                                                    if (currentIpAddr.arIP[twelveOctetToChange] < 255)
                                                        currentIpAddr.arIP[twelveOctetToChange]++;
                                                    else
                                                    {
                                                        currentIpAddr.arIP[twelveOctetToChange] = 0;
                                                        if (currentIpAddr.arIP[thirteenOctetToChange] < 255)
                                                            currentIpAddr.arIP[thirteenOctetToChange]++;
                                                        else
                                                        {
                                                            currentIpAddr.arIP[thirteenOctetToChange] = 0;
                                                            if (currentIpAddr.arIP[fourteenOctetToChange] < 255)
                                                                currentIpAddr.arIP[fourteenOctetToChange]++;
                                                            else
                                                            {
                                                                currentIpAddr.arIP[fourteenOctetToChange] = 0;
                                                                if (currentIpAddr.arIP[fifteenOctetToChange] < 255)
                                                                    currentIpAddr.arIP[fifteenOctetToChange]++;
                                                                else
                                                                {
                                                                    currentIpAddr.arIP[fifteenOctetToChange] = 0;
                                                                    if (currentIpAddr.arIP[sixteenOctetToChange] < 255)
                                                                        currentIpAddr.arIP[sixteenOctetToChange]++;
                                                                    else
                                                                    {
                                                                        rc = GT_NO_MORE;
                                                                        break;
                                                                    }
                                                                }
                                                            }
                                                        }
                                                    }
                                                }
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }

    /* print result */
    cpssOsPrintf("\nIpv6UcPrefixAddMany results:");
    if (numOfPrefixesAdded == 0)
    {
        cpssOsPrintf("\n    No prefixes were added at all.\n");
    }
    else
    {
        cpssOsPrintf("\n    Prefixes ");
        outputIPAddress(startIpAddr.arIP, PRV_CPSS_DXCH_LPM_NUM_OF_OCTETS_IN_IPV6_PROTOCOL_CNS, GT_FALSE);
        cpssOsPrintf(" - ");
        outputIPAddress(lastIpAddrAdded.arIP, PRV_CPSS_DXCH_LPM_NUM_OF_OCTETS_IN_IPV6_PROTOCOL_CNS, GT_FALSE);
        cpssOsPrintf(" were added (%d/%d)\n",numOfPrefixesAdded,numOfPrefixesToAdd);
        cpssOsPrintf("    (%d) MC prefixes not added in this range\n",numOfMcPrefixesNotAdded);
    }

    /* update output parameters */
    if (numOfPrefixesAddedPtr != NULL)
        *numOfPrefixesAddedPtr = numOfPrefixesAdded;
    if (lastIpAddrAddedPtr != NULL)
    {
        for(j=0;j<4;j++)
        {
            lastIpAddrAddedPtr->u32Ip[j] = lastIpAddrAdded.u32Ip[j];
        }
    }
    return rc;
}

/*******************************************************************************
* cpssDxChIpLpmIpv6UcPrefixDelManyByOctet
*
* DESCRIPTION:
*       This function tries to delete many sequential IPv6 Unicast prefixes according
*       to octet order selection and returns the number of prefixes successfully deleted.
*
* APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       lpmDbId               - the LPM DB id
*       vrId                  - the virtual router id
*       startIpAddr           - the first address to add
*       numOfPrefixesToDel    - the number of prefixes to deleted
*       firstOctetToChange    - first octet to change in the prefix address.
*                               value of the octet can be up to 255
*       secondOctetToChange   - second octet to change in the prefix address.
*                               value of the octet can be up to 255
*       thirdOctetToChange    - third octet to change in the prefix address.
*                               value of the octet can be up to 255
*       forthOctetToChange    - forth octet to change in the prefix address.
*                               value of the octet can be up to 255
*       fifthOctetToChange    - fifth octet to change in the prefix address.
*                               value of the octet can be up to 255
*       sixthOctetToChange    - sixth octet to change in the prefix address.
*                               value of the octet can be up to 255
*       seventhOctetToChange  - seventh octet to change in the prefix address.
*                               value of the octet can be up to 255
*       eighthOctetToChange   - eighth octet to change in the prefix address.
*                               value of the octet can be up to 255
*       ninthOctetToChange    - ninth octet to change in the prefix address.
*                               value of the octet can be up to 255
*       tenthOctetToChange    - tenth octet to change in the prefix address.
*                               value of the octet can be up to 255
*       elevenOctetToChange   - eleven octet to change in the prefix address.
*                               value of the octet can be up to 255
*       twelveOctetToChange   - twelve octet to change in the prefix address.
*                               value of the octet can be up to 255
*       thirteenOctetToChange - thirteen octet to change in the prefix address.
*                               value of the octet can be up to 255
*       fourteenOctetToChange - fourteen octet to change in the prefix address.
*                               value of the octet can be up to 255
*       fifteenOctetToChange  - fifteen octet to change in the prefix address.
*                               value of the octet can be up to 255
*       sixteenOctetToChange  - sixteen octet to change in the prefix address.
*                               value of the octet can be up to 255
*
* OUTPUTS:
*       lastIpAddrDeletedPtr    - points to the last prefix successfully
*                               deleted (NULL to ignore)
*       numOfPrefixesDeletedPtr - points to the nubmer of prefixes that were
*                               successfully deleted (NULL to ignore)
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - on wrong devNum or vrId.
*       GT_HW_ERROR              - on Hardware error.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       This debug function is intended to delete the number of prefixes
*       that was added by cpssDxChIpLpmIpv6UcPrefixAddManyByOctet.
*
*******************************************************************************/
GT_STATUS cpssDxChIpLpmIpv6UcPrefixDelManyByOctet
(
    IN  GT_U32      lpmDbId,
    IN  GT_U32      vrId,
    IN  GT_IPV6ADDR startIpAddr,
    IN  GT_U32      numOfPrefixesToDel,
    IN  GT_U32      firstOctetToChange,
    IN  GT_U32      secondOctetToChange,
    IN  GT_U32      thirdOctetToChange,
    IN  GT_U32      forthOctetToChange,
    IN  GT_U32      fifthOctetToChange,
    IN  GT_U32      sixthOctetToChange,
    IN  GT_U32      seventhOctetToChange,
    IN  GT_U32      eighthOctetToChange,
    IN  GT_U32      ninthOctetToChange,
    IN  GT_U32      tenthOctetToChange,
    IN  GT_U32      elevenOctetToChange,
    IN  GT_U32      twelveOctetToChange,
    IN  GT_U32      thirteenOctetToChange,
    IN  GT_U32      fourteenOctetToChange,
    IN  GT_U32      fifteenOctetToChange,
    IN  GT_U32      sixteenOctetToChange,
    OUT GT_IPV6ADDR *lastIpAddrDeletedPtr,
    OUT GT_U32      *numOfPrefixesDeletedPtr
)
{
    GT_STATUS                               rc = GT_OK;
    GT_U32                                  i = 0;
    GT_U32                                  j = 0;
    GT_IPV6ADDR                             currentIpAddr;
    GT_U32                                  numOfPrefixesDeleted = 0;
    GT_U32                                  numOfMcPrefixesNotDeleted = 0;
    GT_IPV6ADDR                             lastIpAddrDeleted;
    GT_U32                                  prefixLength = 128;
    GT_BOOL                                 octetsArray[PRV_CPSS_DXCH_LPM_NUM_OF_OCTETS_IN_IPV6_PROTOCOL_CNS]={GT_FALSE,GT_FALSE,GT_FALSE,GT_FALSE,
                                                            GT_FALSE,GT_FALSE,GT_FALSE,GT_FALSE,
                                                            GT_FALSE,GT_FALSE,GT_FALSE,GT_FALSE,
                                                            GT_FALSE,GT_FALSE,GT_FALSE,GT_FALSE};/* array to hold the octets already selected */

    GT_U32                                  octetsOrderToChangeArray[PRV_CPSS_DXCH_LPM_NUM_OF_OCTETS_IN_IPV6_PROTOCOL_CNS];

    cpssOsMemSet(&currentIpAddr, 0, sizeof(GT_IPV6ADDR));

    for(j=0;j<4;j++)
    {
        currentIpAddr.u32Ip[j]        = startIpAddr.u32Ip[j];
        lastIpAddrDeleted.u32Ip[j]    = startIpAddr.u32Ip[j];
    }


    octetsOrderToChangeArray[0] = firstOctetToChange;
    octetsOrderToChangeArray[1] = secondOctetToChange;
    octetsOrderToChangeArray[2] = thirdOctetToChange;
    octetsOrderToChangeArray[3] = forthOctetToChange;
    octetsOrderToChangeArray[4] = fifthOctetToChange;
    octetsOrderToChangeArray[5] = sixthOctetToChange;
    octetsOrderToChangeArray[6] = seventhOctetToChange;
    octetsOrderToChangeArray[7] = eighthOctetToChange;
    octetsOrderToChangeArray[8] = ninthOctetToChange;
    octetsOrderToChangeArray[9] = tenthOctetToChange;
    octetsOrderToChangeArray[10] = elevenOctetToChange;
    octetsOrderToChangeArray[11] = twelveOctetToChange;
    octetsOrderToChangeArray[12] = thirteenOctetToChange;
    octetsOrderToChangeArray[13] = fourteenOctetToChange;
    octetsOrderToChangeArray[14] = fifteenOctetToChange;
    octetsOrderToChangeArray[15] = sixteenOctetToChange;

    for(j=0;j<PRV_CPSS_DXCH_LPM_NUM_OF_OCTETS_IN_IPV6_PROTOCOL_CNS;j++)
    {
        if(octetsOrderToChangeArray[j]>15)
        {
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
        }
    }

    for(j=0;j<PRV_CPSS_DXCH_LPM_NUM_OF_OCTETS_IN_IPV6_PROTOCOL_CNS;j++)
    {
        /* make sure octet are not selected twice */
        if(octetsArray[octetsOrderToChangeArray[j]]==GT_FALSE)
        {
            octetsArray[octetsOrderToChangeArray[j]]=GT_TRUE;
        }
        else
        {
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
        }
    }


    for (i = 0 ; i < numOfPrefixesToDel ; )
    {
        /* make sure the prefix is not a MC prefix - not in format ff00::/8
           therefore the first octate must not start with 0xFF */
        if((currentIpAddr.arIP[0]) != 0xFF)
        {

            /* delete the current prefix */
            rc = cpssDxChIpLpmIpv6UcPrefixDel(lpmDbId,
                                              vrId,
                                              currentIpAddr,
                                              prefixLength);

            if (rc != GT_OK)
                break;

            numOfPrefixesDeleted++;
            i++;
        }
        else
        {
            numOfMcPrefixesNotDeleted++;
        }

        for(j=0;j<4;j++)
        {
            lastIpAddrDeleted.u32Ip[j]    = currentIpAddr.u32Ip[j];
        }

        /* advance to the next IP address */
        if (currentIpAddr.arIP[firstOctetToChange] < 255)
            currentIpAddr.arIP[firstOctetToChange]++;
        else
        {
            currentIpAddr.arIP[firstOctetToChange] = 0;
            if (currentIpAddr.arIP[secondOctetToChange] < 255)
                currentIpAddr.arIP[secondOctetToChange]++;
            else
            {
                currentIpAddr.arIP[secondOctetToChange] = 0;
                if (currentIpAddr.arIP[thirdOctetToChange] < 255)
                    currentIpAddr.arIP[thirdOctetToChange]++;
                else
                {
                    currentIpAddr.arIP[thirdOctetToChange] = 0;
                    if (currentIpAddr.arIP[forthOctetToChange] < 255)
                        currentIpAddr.arIP[forthOctetToChange]++;
                    else
                    {
                        currentIpAddr.arIP[forthOctetToChange] = 0;
                        if (currentIpAddr.arIP[fifthOctetToChange] < 255)
                            currentIpAddr.arIP[fifthOctetToChange]++;
                        else
                        {
                            currentIpAddr.arIP[fifthOctetToChange] = 0;
                            if (currentIpAddr.arIP[sixthOctetToChange] < 255)
                                currentIpAddr.arIP[sixthOctetToChange]++;
                            else
                            {
                                currentIpAddr.arIP[sixthOctetToChange] = 0;
                                if (currentIpAddr.arIP[seventhOctetToChange] < 255)
                                    currentIpAddr.arIP[seventhOctetToChange]++;
                                else
                                {
                                    currentIpAddr.arIP[seventhOctetToChange] = 0;
                                    if (currentIpAddr.arIP[eighthOctetToChange] < 255)
                                        currentIpAddr.arIP[eighthOctetToChange]++;
                                    else
                                    {
                                        currentIpAddr.arIP[eighthOctetToChange] = 0;
                                        if (currentIpAddr.arIP[ninthOctetToChange] < 255)
                                            currentIpAddr.arIP[ninthOctetToChange]++;
                                        else
                                        {
                                            currentIpAddr.arIP[ninthOctetToChange] = 0;
                                            if (currentIpAddr.arIP[tenthOctetToChange] < 255)
                                                currentIpAddr.arIP[tenthOctetToChange]++;
                                            else
                                            {
                                                currentIpAddr.arIP[tenthOctetToChange] = 0;
                                                if (currentIpAddr.arIP[elevenOctetToChange] < 255)
                                                    currentIpAddr.arIP[elevenOctetToChange]++;
                                                else
                                                {
                                                    currentIpAddr.arIP[elevenOctetToChange] = 0;
                                                    if (currentIpAddr.arIP[twelveOctetToChange] < 255)
                                                        currentIpAddr.arIP[twelveOctetToChange]++;
                                                    else
                                                    {
                                                        currentIpAddr.arIP[twelveOctetToChange] = 0;
                                                        if (currentIpAddr.arIP[thirteenOctetToChange] < 255)
                                                            currentIpAddr.arIP[thirteenOctetToChange]++;
                                                        else
                                                        {
                                                            currentIpAddr.arIP[thirteenOctetToChange] = 0;
                                                            if (currentIpAddr.arIP[fourteenOctetToChange] < 255)
                                                                currentIpAddr.arIP[fourteenOctetToChange]++;
                                                            else
                                                            {
                                                                currentIpAddr.arIP[fourteenOctetToChange] = 0;
                                                                if (currentIpAddr.arIP[fifteenOctetToChange] < 255)
                                                                    currentIpAddr.arIP[fifteenOctetToChange]++;
                                                                else
                                                                {
                                                                    currentIpAddr.arIP[fifteenOctetToChange] = 0;
                                                                    if (currentIpAddr.arIP[sixteenOctetToChange] < 255)
                                                                        currentIpAddr.arIP[sixteenOctetToChange]++;
                                                                    else
                                                                    {
                                                                        rc = GT_NO_MORE;
                                                                        break;
                                                                    }
                                                                }
                                                            }
                                                        }
                                                    }
                                                }
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }

    /* print result */
    cpssOsPrintf("\nIpv6UcPrefixDelMany results:");
    if (numOfPrefixesDeleted == 0)
    {
        cpssOsPrintf("\n    No prefixes were Deleted at all.\n");
    }
    else
    {
        cpssOsPrintf("\n    Prefixes ");
        outputIPAddress(startIpAddr.arIP, PRV_CPSS_DXCH_LPM_NUM_OF_OCTETS_IN_IPV6_PROTOCOL_CNS, GT_FALSE);
        cpssOsPrintf(" - ");
        outputIPAddress(lastIpAddrDeleted.arIP, PRV_CPSS_DXCH_LPM_NUM_OF_OCTETS_IN_IPV6_PROTOCOL_CNS, GT_FALSE);
        cpssOsPrintf(" were Deleted (%d/%d)\n",numOfPrefixesDeleted,numOfPrefixesToDel);
        cpssOsPrintf("    (%d) MC prefixes not Deleted in this range\n",numOfMcPrefixesNotDeleted);
    }

    /* update output parameters */
    if (numOfPrefixesDeletedPtr != NULL)
        *numOfPrefixesDeletedPtr = numOfPrefixesDeleted;
    if (lastIpAddrDeletedPtr != NULL)
    {
        for(j=0;j<4;j++)
        {
            lastIpAddrDeletedPtr->u32Ip[j] = lastIpAddrDeleted.u32Ip[j];
        }
    }
    return rc;
}

/*******************************************************************************
* cpssDxChIpLpmIpv6UcPrefixGetManyByOctet
*
* DESCRIPTION:
*       This function tries to get many sequential IPv6 Unicast prefixes (that were
*       added by cpssDxChIpLpmIpv6UcPrefixAddManyByOctet) and returns the number of
*       successfully retrieved prefixes.
*
* APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       lpmDbId               - the LPM DB id
*       vrId                  - the virtual router id
*       startIpAddr           - the first address to add
*       numOfPrefixesToGet    - the number of prefixes to get
*       firstOctetToChange    - first octet to change in the prefix address.
*                               value of the octet can be up to 255
*       secondOctetToChange   - second octet to change in the prefix address.
*                               value of the octet can be up to 255
*       thirdOctetToChange    - third octet to change in the prefix address.
*                               value of the octet can be up to 255
*       forthOctetToChange    - forth octet to change in the prefix address.
*                               value of the octet can be up to 255
*       fifthOctetToChange    - fifth octet to change in the prefix address.
*                               value of the octet can be up to 255
*       sixthOctetToChange    - sixth octet to change in the prefix address.
*                               value of the octet can be up to 255
*       seventhOctetToChange  - seventh octet to change in the prefix address.
*                               value of the octet can be up to 255
*       eighthOctetToChange   - eighth octet to change in the prefix address.
*                               value of the octet can be up to 255
*       ninthOctetToChange    - ninth octet to change in the prefix address.
*                               value of the octet can be up to 255
*       tenthOctetToChange    - tenth octet to change in the prefix address.
*                               value of the octet can be up to 255
*       elevenOctetToChange   - eleven octet to change in the prefix address.
*                               value of the octet can be up to 255
*       twelveOctetToChange   - twelve octet to change in the prefix address.
*                               value of the octet can be up to 255
*       thirteenOctetToChange - thirteen octet to change in the prefix address.
*                               value of the octet can be up to 255
*       fourteenOctetToChange - fourteen octet to change in the prefix address.
*                               value of the octet can be up to 255
*       fifteenOctetToChange  - fifteen octet to change in the prefix address.
*                               value of the octet can be up to 255
*       sixteenOctetToChange  - sixteen octet to change in the prefix address.
*                               value of the octet can be up to 255
*
* OUTPUTS:
*       numOfRetrievedPrefixesPtr - points to the nubmer of prefixes that were
*                                   successfully retrieved (NULL to ignore)
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - on wrong devNum or vrId.
*       GT_HW_ERROR              - on Hardware error.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       This debug function is intended to check the number of the prefixes
*       that were be added.
*******************************************************************************/
GT_STATUS cpssDxChIpLpmIpv6UcPrefixGetManyByOctet
(
    IN  GT_U32      lpmDbId,
    IN  GT_U32      vrId,
    IN  GT_IPV6ADDR startIpAddr,
    IN  GT_U32      numOfPrefixesToGet,
    IN  GT_U32      firstOctetToChange,
    IN  GT_U32      secondOctetToChange,
    IN  GT_U32      thirdOctetToChange,
    IN  GT_U32      forthOctetToChange,
    IN  GT_U32      fifthOctetToChange,
    IN  GT_U32      sixthOctetToChange,
    IN  GT_U32      seventhOctetToChange,
    IN  GT_U32      eighthOctetToChange,
    IN  GT_U32      ninthOctetToChange,
    IN  GT_U32      tenthOctetToChange,
    IN  GT_U32      elevenOctetToChange,
    IN  GT_U32      twelveOctetToChange,
    IN  GT_U32      thirteenOctetToChange,
    IN  GT_U32      fourteenOctetToChange,
    IN  GT_U32      fifteenOctetToChange,
    IN  GT_U32      sixteenOctetToChange,
    OUT GT_U32      *numOfRetrievedPrefixesPtr
)
{
    GT_STATUS                               rc = GT_OK;
    GT_U32                                  i = 0;
    GT_U32                                  j = 0;
    GT_IPV6ADDR                             currentIpAddr;
    GT_U32                                  numOfRetrievedPrefixes = 0;
    GT_U32                                  numOfMcPrefixesNotAdded = 0;
    GT_IPV6ADDR                             lastIpAddrAdded;
    CPSS_DXCH_IP_TCAM_ROUTE_ENTRY_INFO_UNT  nextHopInfo;
    GT_U32                                  prefixLength = 128;
    GT_BOOL                                 octetsArray[PRV_CPSS_DXCH_LPM_NUM_OF_OCTETS_IN_IPV6_PROTOCOL_CNS]={GT_FALSE,GT_FALSE,GT_FALSE,GT_FALSE,
                                                            GT_FALSE,GT_FALSE,GT_FALSE,GT_FALSE,
                                                            GT_FALSE,GT_FALSE,GT_FALSE,GT_FALSE,
                                                            GT_FALSE,GT_FALSE,GT_FALSE,GT_FALSE};/* array to hold the octets already selected */

    GT_U32                                  octetsOrderToChangeArray[PRV_CPSS_DXCH_LPM_NUM_OF_OCTETS_IN_IPV6_PROTOCOL_CNS];
    GT_U32                                  tcamRowIndex=0;
    GT_U32                                  tcamColumnIndex=0;

    cpssOsMemSet(&nextHopInfo, 0, sizeof(CPSS_DXCH_IP_TCAM_ROUTE_ENTRY_INFO_UNT));
    cpssOsMemSet(&currentIpAddr, 0, sizeof(GT_IPV6ADDR));

    for(j=0;j<4;j++)
    {
        currentIpAddr.u32Ip[j]     = startIpAddr.u32Ip[j];
        lastIpAddrAdded.u32Ip[j]    = startIpAddr.u32Ip[j];
    }

    octetsOrderToChangeArray[0] = firstOctetToChange;
    octetsOrderToChangeArray[1] = secondOctetToChange;
    octetsOrderToChangeArray[2] = thirdOctetToChange;
    octetsOrderToChangeArray[3] = forthOctetToChange;
    octetsOrderToChangeArray[4] = fifthOctetToChange;
    octetsOrderToChangeArray[5] = sixthOctetToChange;
    octetsOrderToChangeArray[6] = seventhOctetToChange;
    octetsOrderToChangeArray[7] = eighthOctetToChange;
    octetsOrderToChangeArray[8] = ninthOctetToChange;
    octetsOrderToChangeArray[9] = tenthOctetToChange;
    octetsOrderToChangeArray[10] = elevenOctetToChange;
    octetsOrderToChangeArray[11] = twelveOctetToChange;
    octetsOrderToChangeArray[12] = thirteenOctetToChange;
    octetsOrderToChangeArray[13] = fourteenOctetToChange;
    octetsOrderToChangeArray[14] = fifteenOctetToChange;
    octetsOrderToChangeArray[15] = sixteenOctetToChange;

    for(j=0;j<PRV_CPSS_DXCH_LPM_NUM_OF_OCTETS_IN_IPV6_PROTOCOL_CNS;j++)
    {
        if(octetsOrderToChangeArray[j]>15)
        {
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
        }
    }

    for(j=0;j<PRV_CPSS_DXCH_LPM_NUM_OF_OCTETS_IN_IPV6_PROTOCOL_CNS;j++)
    {
        /* make sure octet are not selected twice */
        if(octetsArray[octetsOrderToChangeArray[j]]==GT_FALSE)
        {
            octetsArray[octetsOrderToChangeArray[j]]=GT_TRUE;
        }
        else
        {
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
        }
    }

    for (i = 0 ; i < numOfPrefixesToGet ; )
    {
        /* make sure the prefix is not a MC prefix - not in format ff00::/8
           therefore the first octate must not start with 0xFF */
        if((currentIpAddr.arIP[0]) != 0xFF)
        {

            /* get the current prefix */
            rc =  cpssDxChIpLpmIpv6UcPrefixSearch(lpmDbId,
                                                 vrId,
                                                 currentIpAddr,
                                                 prefixLength,
                                                 &nextHopInfo,
                                                 &tcamRowIndex,
                                                 &tcamColumnIndex);

            if (rc != GT_OK)
                break;

            numOfRetrievedPrefixes++;
            i++;
        }
        else
        {
            numOfMcPrefixesNotAdded++;
        }

        for(j=0;j<4;j++)
        {
            lastIpAddrAdded.u32Ip[j]    = currentIpAddr.u32Ip[j];
        }


        /* advance to the next IP address */
        if (currentIpAddr.arIP[firstOctetToChange] < 255)
            currentIpAddr.arIP[firstOctetToChange]++;
        else
        {
            currentIpAddr.arIP[firstOctetToChange] = 0;
            if (currentIpAddr.arIP[secondOctetToChange] < 255)
                currentIpAddr.arIP[secondOctetToChange]++;
            else
            {
                currentIpAddr.arIP[secondOctetToChange] = 0;
                if (currentIpAddr.arIP[thirdOctetToChange] < 255)
                    currentIpAddr.arIP[thirdOctetToChange]++;
                else
                {
                    currentIpAddr.arIP[thirdOctetToChange] = 0;
                    if (currentIpAddr.arIP[forthOctetToChange] < 255)
                        currentIpAddr.arIP[forthOctetToChange]++;
                    else
                    {
                        currentIpAddr.arIP[forthOctetToChange] = 0;
                        if (currentIpAddr.arIP[fifthOctetToChange] < 255)
                            currentIpAddr.arIP[fifthOctetToChange]++;
                        else
                        {
                            currentIpAddr.arIP[fifthOctetToChange] = 0;
                            if (currentIpAddr.arIP[sixthOctetToChange] < 255)
                                currentIpAddr.arIP[sixthOctetToChange]++;
                            else
                            {
                                currentIpAddr.arIP[sixthOctetToChange] = 0;
                                if (currentIpAddr.arIP[seventhOctetToChange] < 255)
                                    currentIpAddr.arIP[seventhOctetToChange]++;
                                else
                                {
                                    currentIpAddr.arIP[seventhOctetToChange] = 0;
                                    if (currentIpAddr.arIP[eighthOctetToChange] < 255)
                                        currentIpAddr.arIP[eighthOctetToChange]++;
                                    else
                                    {
                                        currentIpAddr.arIP[eighthOctetToChange] = 0;
                                        if (currentIpAddr.arIP[ninthOctetToChange] < 255)
                                            currentIpAddr.arIP[ninthOctetToChange]++;
                                        else
                                        {
                                            currentIpAddr.arIP[ninthOctetToChange] = 0;
                                            if (currentIpAddr.arIP[tenthOctetToChange] < 255)
                                                currentIpAddr.arIP[tenthOctetToChange]++;
                                            else
                                            {
                                                currentIpAddr.arIP[tenthOctetToChange] = 0;
                                                if (currentIpAddr.arIP[elevenOctetToChange] < 255)
                                                    currentIpAddr.arIP[elevenOctetToChange]++;
                                                else
                                                {
                                                    currentIpAddr.arIP[elevenOctetToChange] = 0;
                                                    if (currentIpAddr.arIP[twelveOctetToChange] < 255)
                                                        currentIpAddr.arIP[twelveOctetToChange]++;
                                                    else
                                                    {
                                                        currentIpAddr.arIP[twelveOctetToChange] = 0;
                                                        if (currentIpAddr.arIP[thirteenOctetToChange] < 255)
                                                            currentIpAddr.arIP[thirteenOctetToChange]++;
                                                        else
                                                        {
                                                            currentIpAddr.arIP[thirteenOctetToChange] = 0;
                                                            if (currentIpAddr.arIP[fourteenOctetToChange] < 255)
                                                                currentIpAddr.arIP[fourteenOctetToChange]++;
                                                            else
                                                            {
                                                                currentIpAddr.arIP[fourteenOctetToChange] = 0;
                                                                if (currentIpAddr.arIP[fifteenOctetToChange] < 255)
                                                                    currentIpAddr.arIP[fifteenOctetToChange]++;
                                                                else
                                                                {
                                                                    currentIpAddr.arIP[fifteenOctetToChange] = 0;
                                                                    if (currentIpAddr.arIP[sixteenOctetToChange] < 255)
                                                                        currentIpAddr.arIP[sixteenOctetToChange]++;
                                                                    else
                                                                    {
                                                                        rc = GT_NO_MORE;
                                                                        break;
                                                                    }
                                                                }
                                                            }
                                                        }
                                                    }
                                                }
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }

    /* print result */
    cpssOsPrintf("\nIpv6UcPrefixGetMany results:");
    if (numOfRetrievedPrefixes == 0)
    {
        cpssOsPrintf("\n    No prefixes at all.\n");
    }
    else
    {
        cpssOsPrintf("\n    Prefixes ");
        outputIPAddress(startIpAddr.arIP, PRV_CPSS_DXCH_LPM_NUM_OF_OCTETS_IN_IPV6_PROTOCOL_CNS, GT_FALSE);
        cpssOsPrintf(" - ");
        outputIPAddress(lastIpAddrAdded.arIP, PRV_CPSS_DXCH_LPM_NUM_OF_OCTETS_IN_IPV6_PROTOCOL_CNS, GT_FALSE);
        cpssOsPrintf(" were retrieved (%d/%d)\n",numOfRetrievedPrefixes,numOfPrefixesToGet);
        cpssOsPrintf("    (%d) MC prefixes not added in this range\n",numOfMcPrefixesNotAdded);
    }

    /* update output parameters */
    if (numOfRetrievedPrefixesPtr != NULL)
        *numOfRetrievedPrefixesPtr = numOfRetrievedPrefixes;
    return rc;
}

/*******************************************************************************
* cpssDxChIpLpmIpv4McPrefixAddManyByOctet
*
* DESCRIPTION:
*       This function tries to add many sequential IPv4 Multicast prefixes according
*       to octet order selection and returns the number of prefixes successfully added.
*
* APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       lpmDbId               - the LPM DB id
*       vrId                  - the virtual router id
*       grpStartIpAddr        - the first group address to add
*       srcStartIpAddr        - the first source address to add
*       routeEntryBaseMemAddr - base memory Address (offset) of the route entry
*       numOfPrefixesToAdd    - the number of prefixes to add
*       changeGrpAddr         - GT_TRUE: change the group address
*                               GT_FALSE: don't change the group address
*       grpFirstOctetToChange - first octet to change in the prefix address.
*                               value of the octet can be up to 255
*       grpSecondOctetToChange- second octet to change in the prefix address.
*                               value of the octet can be up to 255
*       grpThirdOctetToChange - third octet to change in the prefix address.
*                               value of the octet can be up to 255
*       grpForthOctetToChange - forth octet to change in the prefix address.
*                               value of the octet can be up to 255
*       changeSrcAddr         - GT_TRUE: change the source address
*                               GT_FALSE: don't change the source address
*       srcFirstOctetToChange - first octet to change in the prefix address.
*                               value of the octet can be up to 255
*       srcSecondOctetToChange- second octet to change in the prefix address.
*                               value of the octet can be up to 255
*       srcThirdOctetToChange - third octet to change in the prefix address.
*                               value of the octet can be up to 255
*       srcForthOctetToChange - forth octet to change in the prefix address.
*                               value of the octet can be up to 255
*
* OUTPUTS:
*       grpLastIpAddrAddedPtr - points to the last group prefix successfully
*                               added (NULL to ignore)
*       srcLastIpAddrAddedPtr - points to the last source prefix successfully
*                               added (NULL to ignore)
*       numOfPrefixesAddedPtr - points to the nubmer of prefixes that were
*                               successfully added (NULL to ignore)
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - on wrong devNum or vrId.
*       GT_HW_ERROR              - on Hardware error.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       This debug function is intended to check the number of the prefixes
*       that can be added. All the prefixes are added with exact match (prefix
*       length 32). The route entry is not written.
*
*******************************************************************************/
GT_STATUS cpssDxChIpLpmIpv4McPrefixAddManyByOctet
(
    IN  GT_U32      lpmDbId,
    IN  GT_U32      vrId,
    IN  GT_IPADDR   grpStartIpAddr,
    IN  GT_IPADDR   srcStartIpAddr,
    IN  GT_U32      routeEntryBaseMemAddr,
    IN  GT_U32      numOfPrefixesToAdd,
    IN  GT_BOOL     changeGrpAddr,
    IN  GT_U32      grpFirstOctetToChange,
    IN  GT_U32      grpSecondOctetToChange,
    IN  GT_U32      grpThirdOctetToChange,
    IN  GT_U32      grpForthOctetToChange,
    IN  GT_BOOL     changeSrcAddr,
    IN  GT_U32      srcFirstOctetToChange,
    IN  GT_U32      srcSecondOctetToChange,
    IN  GT_U32      srcThirdOctetToChange,
    IN  GT_U32      srcForthOctetToChange,
    OUT GT_IPADDR   *grpLastIpAddrAddedPtr,
    OUT GT_IPADDR   *srcLastIpAddrAddedPtr,
    OUT GT_U32      *numOfPrefixesAddedPtr
)
{
    GT_STATUS                               rc = GT_OK;
    GT_U32                                  i = 0;
    GT_U32                                  j = 0;
    GT_IPADDR                               grpCurrentIpAddr;
    GT_IPADDR                               srcCurrentIpAddr;
    GT_U32                                  numOfPrefixesAdded = 0;
    GT_IPADDR                               grpLastIpAddrAdded;
    GT_IPADDR                               srcLastIpAddrAdded;
    CPSS_DXCH_IP_LTT_ENTRY_STC              mcRouteLttEntry;
    GT_U32                                  prefixLength = 32;
    GT_BOOL                                 override = GT_FALSE;
    GT_BOOL                                 octetsArray[PRV_CPSS_DXCH_LPM_NUM_OF_OCTETS_IN_IPV4_PROTOCOL_CNS]={GT_FALSE,GT_FALSE,GT_FALSE,GT_FALSE};/* array to hold the octets already selected */
    GT_U32                                  octetsOrderToChangeArray[PRV_CPSS_DXCH_LPM_NUM_OF_OCTETS_IN_IPV4_PROTOCOL_CNS];
    GT_BOOL                                 tcamDefragmentationEnable=GT_FALSE;

    srcCurrentIpAddr.u32Ip                  = srcStartIpAddr.u32Ip;
    srcLastIpAddrAdded.u32Ip                = srcStartIpAddr.u32Ip;

    grpCurrentIpAddr.u32Ip                  = grpStartIpAddr.u32Ip;
    grpLastIpAddrAdded.u32Ip                = grpStartIpAddr.u32Ip;

    cpssOsMemSet(&mcRouteLttEntry, 0, sizeof(CPSS_DXCH_IP_LTT_ENTRY_STC));
    mcRouteLttEntry.routeType           = CPSS_DXCH_IP_ROUTE_ENTRY_METHOD_ECMP_E;
    mcRouteLttEntry.routeEntryBaseIndex = routeEntryBaseMemAddr;
    mcRouteLttEntry.numOfPaths          = 0;
    mcRouteLttEntry.ipv6MCGroupScopeLevel = CPSS_IPV6_PREFIX_SCOPE_LINK_LOCAL_E;
    rc = prvCpssDxChIpLpmDbgConvertRouteType(lpmDbId, &mcRouteLttEntry);
    if (rc != GT_OK)
    {
        return rc;
    }

    if(((changeGrpAddr==GT_TRUE)&&(changeSrcAddr==GT_TRUE))||
       ((changeGrpAddr==GT_FALSE)&&(changeSrcAddr==GT_FALSE)))
    {
        cpssOsPrintf("changeGrpAddr should be different from changeSrcAddr\n");
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }


    if(changeGrpAddr==GT_TRUE)
    {
        octetsOrderToChangeArray[0] = grpFirstOctetToChange;
        octetsOrderToChangeArray[1] = grpSecondOctetToChange;
        octetsOrderToChangeArray[2] = grpThirdOctetToChange;
        octetsOrderToChangeArray[3] = grpForthOctetToChange;
    }
    if(changeSrcAddr==GT_TRUE)
    {
        octetsOrderToChangeArray[0] = srcFirstOctetToChange;
        octetsOrderToChangeArray[1] = srcSecondOctetToChange;
        octetsOrderToChangeArray[2] = srcThirdOctetToChange;
        octetsOrderToChangeArray[3] = srcForthOctetToChange;
    }

    for(j=0;j<PRV_CPSS_DXCH_LPM_NUM_OF_OCTETS_IN_IPV4_PROTOCOL_CNS;j++)
    {
        if(octetsOrderToChangeArray[j]>3)
        {
            cpssOsPrintf("octetToChange can not be bigger then 3\n");
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
        }
    }

    for(j=0;j<PRV_CPSS_DXCH_LPM_NUM_OF_OCTETS_IN_IPV4_PROTOCOL_CNS;j++)
    {
        /* make sure octet are not selected twice */
        if(octetsArray[octetsOrderToChangeArray[j]]==GT_FALSE)
        {
            octetsArray[octetsOrderToChangeArray[j]]=GT_TRUE;
        }
        else
        {
            cpssOsPrintf("octetsOrderToChangeArray[j]=%d was selected twice\n",octetsOrderToChangeArray[j]);
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
        }
    }

    for (i = 0 ; i < numOfPrefixesToAdd ; )
    {
        /* make sure the grp prefix is a MC prefix - in the range 224/4 - 239/4
           therefore the first octate must start with 0xE
           and the src prefix is not a MC prefix */
        if(((grpCurrentIpAddr.arIP[0] & 0xF0) == 0xE0)&&
           ((srcCurrentIpAddr.arIP[0] & 0xF0) != 0xE0))
        {
            /* add the current prefix */
            rc = cpssDxChIpLpmIpv4McEntryAdd(lpmDbId,
                                             vrId,
                                             grpCurrentIpAddr,
                                             prefixLength,
                                             srcCurrentIpAddr,
                                             prefixLength,
                                             &mcRouteLttEntry,
                                             override,
                                             tcamDefragmentationEnable);
            if (rc != GT_OK)
                break;

            numOfPrefixesAdded++;
            i++;
        }
        else
        {
            cpssOsPrintf("\n grpStartIpAddr= ");
            outputIPAddress(grpCurrentIpAddr.arIP, PRV_CPSS_DXCH_LPM_NUM_OF_OCTETS_IN_IPV4_PROTOCOL_CNS, GT_TRUE);
            cpssOsPrintf(" is not a valid MC address, or srcCurrentIpAddr= ");
            outputIPAddress(srcCurrentIpAddr.arIP, PRV_CPSS_DXCH_LPM_NUM_OF_OCTETS_IN_IPV4_PROTOCOL_CNS, GT_TRUE);
            cpssOsPrintf(" is not a valid source address\n");
            break;
        }


        srcLastIpAddrAdded.u32Ip                = srcCurrentIpAddr.u32Ip;
        grpLastIpAddrAdded.u32Ip                = grpCurrentIpAddr.u32Ip;


        if(changeSrcAddr==GT_TRUE)
        {
            /* advance to the next IP address */
            if (srcCurrentIpAddr.arIP[srcFirstOctetToChange] < 255)
                srcCurrentIpAddr.arIP[srcFirstOctetToChange]++;
            else
            {
                srcCurrentIpAddr.arIP[srcFirstOctetToChange] = 0;
                if (srcCurrentIpAddr.arIP[srcSecondOctetToChange] < 255)
                    srcCurrentIpAddr.arIP[srcSecondOctetToChange]++;
                else
                {
                    srcCurrentIpAddr.arIP[srcSecondOctetToChange] = 0;
                    if (srcCurrentIpAddr.arIP[srcThirdOctetToChange] < 255)
                        srcCurrentIpAddr.arIP[srcThirdOctetToChange]++;
                    else
                    {
                        srcCurrentIpAddr.arIP[srcThirdOctetToChange] = 0;
                        if (srcCurrentIpAddr.arIP[srcForthOctetToChange] < 255)
                            srcCurrentIpAddr.arIP[srcForthOctetToChange]++;
                        else
                        {
                            rc = GT_NO_MORE;
                            break;
                        }
                    }
                }
            }
        }
        else /* changeGrpAddr==GT_TRUE*/
        {
            /* advance to the next IP address */
            if (grpCurrentIpAddr.arIP[grpFirstOctetToChange] < 255)
                grpCurrentIpAddr.arIP[grpFirstOctetToChange]++;
            else
            {
                grpCurrentIpAddr.arIP[grpFirstOctetToChange] = 0;
                if (grpCurrentIpAddr.arIP[grpSecondOctetToChange] < 255)
                    grpCurrentIpAddr.arIP[grpSecondOctetToChange]++;
                else
                {
                    grpCurrentIpAddr.arIP[grpSecondOctetToChange] = 0;
                    if (grpCurrentIpAddr.arIP[grpThirdOctetToChange] < 255)
                        grpCurrentIpAddr.arIP[grpThirdOctetToChange]++;
                    else
                    {
                        grpCurrentIpAddr.arIP[grpThirdOctetToChange] = 0;
                        if (grpCurrentIpAddr.arIP[grpForthOctetToChange] < 255)
                            grpCurrentIpAddr.arIP[grpForthOctetToChange]++;
                        else
                        {
                            rc = GT_NO_MORE;
                            break;
                        }
                    }
                }
            }
        }
    }

    /* print result */
    cpssOsPrintf("\nIpv4McPrefixAddMany results:");
    if (numOfPrefixesAdded == 0)
    {
        cpssOsPrintf("\n    No prefixes were added at all.\n");
    }
    else
    {
        cpssOsPrintf("\n    Group Prefixes ");
        outputIPAddress(grpStartIpAddr.arIP, PRV_CPSS_DXCH_LPM_NUM_OF_OCTETS_IN_IPV4_PROTOCOL_CNS, GT_TRUE);
        cpssOsPrintf(" - ");
        outputIPAddress(grpLastIpAddrAdded.arIP, PRV_CPSS_DXCH_LPM_NUM_OF_OCTETS_IN_IPV4_PROTOCOL_CNS, GT_TRUE);
        cpssOsPrintf("\n    Source Prefixes ");
        outputIPAddress(srcStartIpAddr.arIP, PRV_CPSS_DXCH_LPM_NUM_OF_OCTETS_IN_IPV4_PROTOCOL_CNS, GT_TRUE);
        cpssOsPrintf(" - ");
        outputIPAddress(srcLastIpAddrAdded.arIP, PRV_CPSS_DXCH_LPM_NUM_OF_OCTETS_IN_IPV4_PROTOCOL_CNS, GT_TRUE);
        cpssOsPrintf(" were added (%d/%d)\n",numOfPrefixesAdded,numOfPrefixesToAdd);
    }

    /* update output parameters */
    if (numOfPrefixesAddedPtr != NULL)
        *numOfPrefixesAddedPtr = numOfPrefixesAdded;
    if (grpLastIpAddrAddedPtr != NULL)
        grpLastIpAddrAddedPtr->u32Ip = grpLastIpAddrAdded.u32Ip;
    if (srcLastIpAddrAddedPtr != NULL)
        srcLastIpAddrAddedPtr->u32Ip = srcLastIpAddrAdded.u32Ip;

    return rc;
}
/*******************************************************************************
* cpssDxChIpLpmIpv4McPrefixGetManyByOctet
*
* DESCRIPTION:
*       This function tries to get many sequential IPv4 Multicast prefixes (that were
*       added by cpssDxChIpLpmIpv4McPrefixAddManyByOctet) and returns the number of
*       successfully retrieved prefixes.
*
* APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       lpmDbId               - the LPM DB id
*       vrId                  - the virtual router id
*       grpStartIpAddr        - the first group address to add
*       srcStartIpAddr        - the first source address to add
*       numOfPrefixesToGet    - the number of prefixes to get
*       changeGrpAddr         - GT_TRUE: change the group address
*                               GT_FALSE: don't change the group address
*       grpFirstOctetToChange - first octet to change in the prefix address.
*                               value of the octet can be up to 255
*       grpSecondOctetToChange- second octet to change in the prefix address.
*                               value of the octet can be up to 255
*       grpThirdOctetToChange - third octet to change in the prefix address.
*                               value of the octet can be up to 255
*       grpForthOctetToChange - forth octet to change in the prefix address.
*                               value of the octet can be up to 255
*       changeSrcAddr         - GT_TRUE: change the source address
*                               GT_FALSE: don't change the source address
*       srcFirstOctetToChange - first octet to change in the prefix address.
*                               value of the octet can be up to 255
*       srcSecondOctetToChange- second octet to change in the prefix address.
*                               value of the octet can be up to 255
*       srcThirdOctetToChange - third octet to change in the prefix address.
*                               value of the octet can be up to 255
*       srcForthOctetToChange - forth octet to change in the prefix address.
*                               value of the octet can be up to 255*
* OUTPUTS:
*       numOfRetrievedPrefixesPtr - points to the nubmer of prefixes that were
*                                   successfully retrieved (NULL to ignore)
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - on wrong devNum or vrId.
*       GT_HW_ERROR              - on Hardware error.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       This debug function is intended to check the number of the prefixes
*       that were be added.
*******************************************************************************/
GT_STATUS cpssDxChIpLpmIpv4McPrefixGetManyByOctet
(
    IN  GT_U32      lpmDbId,
    IN  GT_U32      vrId,
    IN  GT_IPADDR   grpStartIpAddr,
    IN  GT_IPADDR   srcStartIpAddr,
    IN  GT_U32      numOfPrefixesToGet,
    IN  GT_BOOL     changeGrpAddr,
    IN  GT_U32      grpFirstOctetToChange,
    IN  GT_U32      grpSecondOctetToChange,
    IN  GT_U32      grpThirdOctetToChange,
    IN  GT_U32      grpForthOctetToChange,
    IN  GT_BOOL     changeSrcAddr,
    IN  GT_U32      srcFirstOctetToChange,
    IN  GT_U32      srcSecondOctetToChange,
    IN  GT_U32      srcThirdOctetToChange,
    IN  GT_U32      srcForthOctetToChange,
    OUT GT_U32      *numOfRetrievedPrefixesPtr
)
{
    GT_STATUS                               rc = GT_OK;
    GT_U32                                  i = 0;
    GT_U32                                  j = 0;
    GT_IPADDR                               grpCurrentIpAddr;
    GT_IPADDR                               srcCurrentIpAddr;
    GT_U32                                  numOfRetrievedPrefixes = 0;
    GT_IPADDR                               grpLastIpAddrAdded;
    GT_IPADDR                               srcLastIpAddrAdded;
    CPSS_DXCH_IP_LTT_ENTRY_STC              mcRouteLttEntry;
    GT_U32                                  prefixLength = 32;
    GT_BOOL                                 octetsArray[PRV_CPSS_DXCH_LPM_NUM_OF_OCTETS_IN_IPV4_PROTOCOL_CNS]={GT_FALSE,GT_FALSE,GT_FALSE,GT_FALSE};/* array to hold the octets already selected */
    GT_U32                                  octetsOrderToChangeArray[PRV_CPSS_DXCH_LPM_NUM_OF_OCTETS_IN_IPV4_PROTOCOL_CNS];
    GT_U32                                  tcamGroupRowIndex=0;
    GT_U32                                  tcamGroupColumnIndex=0;
    GT_U32                                  tcamSrcRowIndex=0;
    GT_U32                                  tcamSrcColumnIndex=0;


    srcCurrentIpAddr.u32Ip                  = srcStartIpAddr.u32Ip;
    srcLastIpAddrAdded.u32Ip                = srcStartIpAddr.u32Ip;

    grpCurrentIpAddr.u32Ip                  = grpStartIpAddr.u32Ip;
    grpLastIpAddrAdded.u32Ip                = grpStartIpAddr.u32Ip;

    cpssOsMemSet(&mcRouteLttEntry, 0, sizeof(CPSS_DXCH_IP_LTT_ENTRY_STC));

    if(((changeGrpAddr==GT_TRUE)&&(changeSrcAddr==GT_TRUE))||
       ((changeGrpAddr==GT_FALSE)&&(changeSrcAddr==GT_FALSE)))
    {
        cpssOsPrintf("changeGrpAddr should be different from changeSrcAddr\n");
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }


    if(changeGrpAddr==GT_TRUE)
    {
        octetsOrderToChangeArray[0] = grpFirstOctetToChange;
        octetsOrderToChangeArray[1] = grpSecondOctetToChange;
        octetsOrderToChangeArray[2] = grpThirdOctetToChange;
        octetsOrderToChangeArray[3] = grpForthOctetToChange;
    }
    if(changeSrcAddr==GT_TRUE)
    {
        octetsOrderToChangeArray[0] = srcFirstOctetToChange;
        octetsOrderToChangeArray[1] = srcSecondOctetToChange;
        octetsOrderToChangeArray[2] = srcThirdOctetToChange;
        octetsOrderToChangeArray[3] = srcForthOctetToChange;
    }

    for(j=0;j<PRV_CPSS_DXCH_LPM_NUM_OF_OCTETS_IN_IPV4_PROTOCOL_CNS;j++)
    {
        if(octetsOrderToChangeArray[j]>3)
        {
            cpssOsPrintf("octetToChange can not be bigger then 3\n");
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
        }
    }

    for(j=0;j<PRV_CPSS_DXCH_LPM_NUM_OF_OCTETS_IN_IPV4_PROTOCOL_CNS;j++)
    {
        /* make sure octet are not selected twice */
        if(octetsArray[octetsOrderToChangeArray[j]]==GT_FALSE)
        {
            octetsArray[octetsOrderToChangeArray[j]]=GT_TRUE;
        }
        else
        {
            cpssOsPrintf("octetsOrderToChangeArray[j]=%d was selected twice\n",octetsOrderToChangeArray[j]);
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
        }
    }

    for (i = 0 ; i < numOfPrefixesToGet ; )
    {
        /* make sure the grp prefix is a MC prefix - in the range 224/4 - 239/4
           therefore the first octate must start with 0xE
           and the src prefix is not a MC prefix */
        if(((grpCurrentIpAddr.arIP[0] & 0xF0) == 0xE0)&&
           ((srcCurrentIpAddr.arIP[0] & 0xF0) != 0xE0))
        {
            /* get the current prefix */
            rc = cpssDxChIpLpmIpv4McEntrySearch(lpmDbId,vrId,
                                               grpCurrentIpAddr,
                                               prefixLength,
                                               srcCurrentIpAddr,
                                               prefixLength,
                                               &mcRouteLttEntry,
                                               &tcamGroupRowIndex,
                                               &tcamGroupColumnIndex,
                                               &tcamSrcRowIndex,
                                               &tcamSrcColumnIndex);

            if (rc != GT_OK)
                break;

            numOfRetrievedPrefixes++;
            i++;
        }
        else
        {
            cpssOsPrintf("\n grpStartIpAddr= ");
            outputIPAddress(grpCurrentIpAddr.arIP, PRV_CPSS_DXCH_LPM_NUM_OF_OCTETS_IN_IPV4_PROTOCOL_CNS, GT_TRUE);
            cpssOsPrintf(" is not a valid MC address, or srcCurrentIpAddr= ");
            outputIPAddress(srcCurrentIpAddr.arIP, PRV_CPSS_DXCH_LPM_NUM_OF_OCTETS_IN_IPV4_PROTOCOL_CNS, GT_TRUE);
            cpssOsPrintf(" is not a valid source address\n");
            break;
        }


        srcLastIpAddrAdded.u32Ip                = srcCurrentIpAddr.u32Ip;
        grpLastIpAddrAdded.u32Ip                = grpCurrentIpAddr.u32Ip;


        if(changeSrcAddr==GT_TRUE)
        {
            /* advance to the next IP address */
            if (srcCurrentIpAddr.arIP[srcFirstOctetToChange] < 255)
                srcCurrentIpAddr.arIP[srcFirstOctetToChange]++;
            else
            {
                srcCurrentIpAddr.arIP[srcFirstOctetToChange] = 0;
                if (srcCurrentIpAddr.arIP[srcSecondOctetToChange] < 255)
                    srcCurrentIpAddr.arIP[srcSecondOctetToChange]++;
                else
                {
                    srcCurrentIpAddr.arIP[srcSecondOctetToChange] = 0;
                    if (srcCurrentIpAddr.arIP[srcThirdOctetToChange] < 255)
                        srcCurrentIpAddr.arIP[srcThirdOctetToChange]++;
                    else
                    {
                        srcCurrentIpAddr.arIP[srcThirdOctetToChange] = 0;
                        if (srcCurrentIpAddr.arIP[srcForthOctetToChange] < 255)
                            srcCurrentIpAddr.arIP[srcForthOctetToChange]++;
                        else
                        {
                            rc = GT_NO_MORE;
                            break;
                        }
                    }
                }
            }
        }
        else /* changeGrpAddr==GT_TRUE*/
        {
            /* advance to the next IP address */
            if (grpCurrentIpAddr.arIP[grpFirstOctetToChange] < 255)
                grpCurrentIpAddr.arIP[grpFirstOctetToChange]++;
            else
            {
                grpCurrentIpAddr.arIP[grpFirstOctetToChange] = 0;
                if (grpCurrentIpAddr.arIP[grpSecondOctetToChange] < 255)
                    grpCurrentIpAddr.arIP[grpSecondOctetToChange]++;
                else
                {
                    grpCurrentIpAddr.arIP[grpSecondOctetToChange] = 0;
                    if (grpCurrentIpAddr.arIP[grpThirdOctetToChange] < 255)
                        grpCurrentIpAddr.arIP[grpThirdOctetToChange]++;
                    else
                    {
                        grpCurrentIpAddr.arIP[grpThirdOctetToChange] = 0;
                        if (grpCurrentIpAddr.arIP[grpForthOctetToChange] < 255)
                            grpCurrentIpAddr.arIP[grpForthOctetToChange]++;
                        else
                        {
                            rc = GT_NO_MORE;
                            break;
                        }
                    }
                }
            }
        }
    }

    /* print result */
    cpssOsPrintf("\nIpv4UcPrefixGetMany results:");
    if (numOfRetrievedPrefixes == 0)
    {
        cpssOsPrintf("\n    No prefixes at all.\n");
    }
    else
    {
        outputIPAddress(grpStartIpAddr.arIP, PRV_CPSS_DXCH_LPM_NUM_OF_OCTETS_IN_IPV4_PROTOCOL_CNS, GT_TRUE);
        cpssOsPrintf(" - ");
        outputIPAddress(grpLastIpAddrAdded.arIP, PRV_CPSS_DXCH_LPM_NUM_OF_OCTETS_IN_IPV4_PROTOCOL_CNS, GT_TRUE);
        cpssOsPrintf("\n    Source Prefixes ");
        outputIPAddress(srcStartIpAddr.arIP, PRV_CPSS_DXCH_LPM_NUM_OF_OCTETS_IN_IPV4_PROTOCOL_CNS, GT_TRUE);
        cpssOsPrintf(" - ");
        outputIPAddress(srcLastIpAddrAdded.arIP, PRV_CPSS_DXCH_LPM_NUM_OF_OCTETS_IN_IPV4_PROTOCOL_CNS, GT_TRUE);
        cpssOsPrintf(" were retrieved (%d/%d)\n",numOfRetrievedPrefixes,numOfPrefixesToGet);
    }

    /* update output parameters */
    if (numOfRetrievedPrefixesPtr != NULL)
        *numOfRetrievedPrefixesPtr = numOfRetrievedPrefixes;

    return rc;
}


/*******************************************************************************
* cpssDxChIpLpmIpv6McPrefixAddManyByOctet
*
* DESCRIPTION:
*       This function tries to add many sequential IPv6 Multicast prefixes according
*       to octet order selection and returns the number of prefixes successfully added.
*
* APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       lpmDbId               - the LPM DB id
*       vrId                  - the virtual router id
*       grpStartIpAddr        - the first group address to add
*       srcStartIpAddr        - the first source address to add
*       routeEntryBaseMemAddr - base memory Address (offset) of the route entry
*       numOfPrefixesToAdd    - the number of prefixes to add
*       changeGrpAddr         - GT_TRUE: change the group address
*                               GT_FALSE: don't change the group address
*       grpFirstOctetToChange - first octet to change in the prefix address.
*                               value of the octet can be up to 255
*       grpSecondOctetToChange- second octet to change in the prefix address.
*                               value of the octet can be up to 255
*       grpThirdOctetToChange - third octet to change in the prefix address.
*                               value of the octet can be up to 255
*       grpForthOctetToChange - forth octet to change in the prefix address.
*                               value of the octet can be up to 255
*       grpFifthOctetToChange - fifth octet to change in the prefix address.
*                               value of the octet can be up to 255
*       grpSixthOctetToChange - sixth octet to change in the prefix address.
*                               value of the octet can be up to 255
*       grpSeventhOctetToChange  - seventh octet to change in the prefix address.
*                               value of the octet can be up to 255
*       grpEighthOctetToChange- eighth octet to change in the prefix address.
*                               value of the octet can be up to 255
*       grpNinthOctetToChange - ninth octet to change in the prefix address.
*                               value of the octet can be up to 255
*       grpTenthOctetToChange - tenth octet to change in the prefix address.
*                               value of the octet can be up to 255
*       grpElevenOctetToChange- eleven octet to change in the prefix address.
*                               value of the octet can be up to 255
*       grpTwelveOctetToChange- twelve octet to change in the prefix address.
*                               value of the octet can be up to 255
*       grpThirteenOctetToChange - thirteen octet to change in the prefix address.
*                               value of the octet can be up to 255
*       grpFourteenOctetToChange - fourteen octet to change in the prefix address.
*                               value of the octet can be up to 255
*       grpFifteenOctetToChange  - fifteen octet to change in the prefix address.
*                               value of the octet can be up to 255
*       grpSixteenOctetToChange  - sixteen octet to change in the prefix address.
*                               value of the octet can be up to 255
*       changeSrcAddr         - GT_TRUE: change the source address
*                               GT_FALSE: don't change the source address
*       srcFirstOctetToChange - first octet to change in the prefix address.
*                               value of the octet can be up to 255
*       srcSecondOctetToChange- second octet to change in the prefix address.
*                               value of the octet can be up to 255
*       srcThirdOctetToChange - third octet to change in the prefix address.
*                               value of the octet can be up to 255
*       srcForthOctetToChange - forth octet to change in the prefix address.
*                               value of the octet can be up to 255
*       srcFifthOctetToChange - fifth octet to change in the prefix address.
*                               value of the octet can be up to 255
*       srcSixthOctetToChange - sixth octet to change in the prefix address.
*                               value of the octet can be up to 255
*       srcSeventhOctetToChange  - seventh octet to change in the prefix address.
*                               value of the octet can be up to 255
*       srcEighthOctetToChange- eighth octet to change in the prefix address.
*                               value of the octet can be up to 255
*       srcNinthOctetToChange - ninth octet to change in the prefix address.
*                               value of the octet can be up to 255
*       srcTenthOctetToChange - tenth octet to change in the prefix address.
*                               value of the octet can be up to 255
*       srcElevenOctetToChange- eleven octet to change in the prefix address.
*                               value of the octet can be up to 255
*       srcTwelveOctetToChange- twelve octet to change in the prefix address.
*                               value of the octet can be up to 255
*       srcThirteenOctetToChange - thirteen octet to change in the prefix address.
*                               value of the octet can be up to 255
*       srcFourteenOctetToChange - fourteen octet to change in the prefix address.
*                               value of the octet can be up to 255
*       srcFifteenOctetToChange  - fifteen octet to change in the prefix address.
*                               value of the octet can be up to 255
*       srcSixteenOctetToChange  - sixteen octet to change in the prefix address.
*                               value of the octet can be up to 255
*
* OUTPUTS:
*       grpLastIpAddrAddedPtr - points to the last group prefix successfully
*                               added (NULL to ignore)
*       srcLastIpAddrAddedPtr - points to the last source prefix successfully
*                               added (NULL to ignore)
*       numOfPrefixesAddedPtr - points to the nubmer of prefixes that were
*                               successfully added (NULL to ignore)
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - on wrong devNum or vrId.
*       GT_HW_ERROR              - on Hardware error.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       This debug function is intended to check the number of the prefixes
*       that can be added. All the prefixes are added with exact match (prefix
*       length 128). The route entry is not written.
*
*******************************************************************************/
GT_STATUS cpssDxChIpLpmIpv6McPrefixAddManyByOctet
(
    IN  GT_U32      lpmDbId,
    IN  GT_U32      vrId,
    IN  GT_IPV6ADDR grpStartIpAddr,
    IN  GT_IPV6ADDR srcStartIpAddr,
    IN  GT_U32      routeEntryBaseMemAddr,
    IN  GT_U32      numOfPrefixesToAdd,
    IN  GT_BOOL     changeGrpAddr,
    IN  GT_U32      grpFirstOctetToChange,
    IN  GT_U32      grpSecondOctetToChange,
    IN  GT_U32      grpThirdOctetToChange,
    IN  GT_U32      grpForthOctetToChange,
    IN  GT_U32      grpFifthOctetToChange,
    IN  GT_U32      grpSixthOctetToChange,
    IN  GT_U32      grpSeventhOctetToChange,
    IN  GT_U32      grpEighthOctetToChange,
    IN  GT_U32      grpNinthOctetToChange,
    IN  GT_U32      grpTenthOctetToChange,
    IN  GT_U32      grpElevenOctetToChange,
    IN  GT_U32      grpTwelveOctetToChange,
    IN  GT_U32      grpThirteenOctetToChange,
    IN  GT_U32      grpFourteenOctetToChange,
    IN  GT_U32      grpFifteenOctetToChange,
    IN  GT_U32      grpSixteenOctetToChange,
    IN  GT_BOOL     changeSrcAddr,
    IN  GT_U32      srcFirstOctetToChange,
    IN  GT_U32      srcSecondOctetToChange,
    IN  GT_U32      srcThirdOctetToChange,
    IN  GT_U32      srcForthOctetToChange,
    IN  GT_U32      srcFifthOctetToChange,
    IN  GT_U32      srcSixthOctetToChange,
    IN  GT_U32      srcSeventhOctetToChange,
    IN  GT_U32      srcEighthOctetToChange,
    IN  GT_U32      srcNinthOctetToChange,
    IN  GT_U32      srcTenthOctetToChange,
    IN  GT_U32      srcElevenOctetToChange,
    IN  GT_U32      srcTwelveOctetToChange,
    IN  GT_U32      srcThirteenOctetToChange,
    IN  GT_U32      srcFourteenOctetToChange,
    IN  GT_U32      srcFifteenOctetToChange,
    IN  GT_U32      srcSixteenOctetToChange,
    OUT GT_IPV6ADDR *grpLastIpAddrAddedPtr,
    OUT GT_IPV6ADDR *srcLastIpAddrAddedPtr,
    OUT GT_U32      *numOfPrefixesAddedPtr
)
{
    GT_STATUS                               rc = GT_OK;
    GT_U32                                  i = 0;
    GT_U32                                  j = 0;
    GT_IPV6ADDR                             grpCurrentIpAddr;
    GT_IPV6ADDR                             srcCurrentIpAddr;
    GT_U32                                  numOfPrefixesAdded = 0;
    GT_IPV6ADDR                             grpLastIpAddrAdded;
    GT_IPV6ADDR                             srcLastIpAddrAdded;
    CPSS_DXCH_IP_LTT_ENTRY_STC              mcRouteLttEntry;
    GT_U32                                  prefixLength = 128;
    GT_BOOL                                 override = GT_FALSE;
    GT_BOOL                                 octetsArray[PRV_CPSS_DXCH_LPM_NUM_OF_OCTETS_IN_IPV6_PROTOCOL_CNS]={GT_FALSE,GT_FALSE,GT_FALSE,GT_FALSE,
                                                         GT_FALSE,GT_FALSE,GT_FALSE,GT_FALSE,
                                                         GT_FALSE,GT_FALSE,GT_FALSE,GT_FALSE,
                                                         GT_FALSE,GT_FALSE,GT_FALSE,GT_FALSE};/* array to hold the octets already selected */

    GT_U32                                  octetsOrderToChangeArray[PRV_CPSS_DXCH_LPM_NUM_OF_OCTETS_IN_IPV6_PROTOCOL_CNS];
    GT_BOOL                                 tcamDefragmentationEnable=GT_FALSE;

    cpssOsMemSet(&mcRouteLttEntry, 0, sizeof(CPSS_DXCH_IP_LTT_ENTRY_STC));
    mcRouteLttEntry.routeType           = CPSS_DXCH_IP_ROUTE_ENTRY_METHOD_ECMP_E;
    mcRouteLttEntry.routeEntryBaseIndex = routeEntryBaseMemAddr;
    mcRouteLttEntry.numOfPaths          = 0;
    mcRouteLttEntry.ipv6MCGroupScopeLevel = CPSS_IPV6_PREFIX_SCOPE_LINK_LOCAL_E;
    rc = prvCpssDxChIpLpmDbgConvertRouteType(lpmDbId, &mcRouteLttEntry);
    if (rc != GT_OK)
    {
        return rc;
    }

    if(((changeGrpAddr==GT_TRUE)&&(changeSrcAddr==GT_TRUE))||
       ((changeGrpAddr==GT_FALSE)&&(changeSrcAddr==GT_FALSE)))
    {
        cpssOsPrintf("changeGrpAddr should be different from changeSrcAddr\n");
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }


    if(changeGrpAddr==GT_TRUE)
    {
        octetsOrderToChangeArray[0] = grpFirstOctetToChange;
        octetsOrderToChangeArray[1] = grpSecondOctetToChange;
        octetsOrderToChangeArray[2] = grpThirdOctetToChange;
        octetsOrderToChangeArray[3] = grpForthOctetToChange;
        octetsOrderToChangeArray[4] = grpFifthOctetToChange;
        octetsOrderToChangeArray[5] = grpSixthOctetToChange;
        octetsOrderToChangeArray[6] = grpSeventhOctetToChange;
        octetsOrderToChangeArray[7] = grpEighthOctetToChange;
        octetsOrderToChangeArray[8] = grpNinthOctetToChange;
        octetsOrderToChangeArray[9] = grpTenthOctetToChange;
        octetsOrderToChangeArray[10] = grpElevenOctetToChange;
        octetsOrderToChangeArray[11] = grpTwelveOctetToChange;
        octetsOrderToChangeArray[12] = grpThirteenOctetToChange;
        octetsOrderToChangeArray[13] = grpFourteenOctetToChange;
        octetsOrderToChangeArray[14] = grpFifteenOctetToChange;
        octetsOrderToChangeArray[15] = grpSixteenOctetToChange;
    }
    if(changeSrcAddr==GT_TRUE)
    {
        octetsOrderToChangeArray[0] = srcFirstOctetToChange;
        octetsOrderToChangeArray[1] = srcSecondOctetToChange;
        octetsOrderToChangeArray[2] = srcThirdOctetToChange;
        octetsOrderToChangeArray[3] = srcForthOctetToChange;
        octetsOrderToChangeArray[4] = srcFifthOctetToChange;
        octetsOrderToChangeArray[5] = srcSixthOctetToChange;
        octetsOrderToChangeArray[6] = srcSeventhOctetToChange;
        octetsOrderToChangeArray[7] = srcEighthOctetToChange;
        octetsOrderToChangeArray[8] = srcNinthOctetToChange;
        octetsOrderToChangeArray[9] = srcTenthOctetToChange;
        octetsOrderToChangeArray[10] = srcElevenOctetToChange;
        octetsOrderToChangeArray[11] = srcTwelveOctetToChange;
        octetsOrderToChangeArray[12] = srcThirteenOctetToChange;
        octetsOrderToChangeArray[13] = srcFourteenOctetToChange;
        octetsOrderToChangeArray[14] = srcFifteenOctetToChange;
        octetsOrderToChangeArray[15] = srcSixteenOctetToChange;
    }

    for(j=0;j<PRV_CPSS_DXCH_LPM_NUM_OF_OCTETS_IN_IPV6_PROTOCOL_CNS;j++)
    {
        if(octetsOrderToChangeArray[j]>15)
        {
            cpssOsPrintf("octetToChange can not be bigger then 15\n");
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
        }
    }

    for(j=0;j<PRV_CPSS_DXCH_LPM_NUM_OF_OCTETS_IN_IPV6_PROTOCOL_CNS;j++)
    {
        /* make sure octet are not selected twice */
        if(octetsArray[octetsOrderToChangeArray[j]]==GT_FALSE)
        {
            octetsArray[octetsOrderToChangeArray[j]]=GT_TRUE;
        }
        else
        {
            cpssOsPrintf("octetsOrderToChangeArray[j]=%d was selected twice\n",octetsOrderToChangeArray[j]);
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
        }
    }

    cpssOsMemSet(&grpCurrentIpAddr, 0, sizeof(GT_IPV6ADDR));
    cpssOsMemSet(&srcCurrentIpAddr, 0, sizeof(GT_IPV6ADDR));

    for(j=0;j<4;j++)
    {
        grpCurrentIpAddr.u32Ip[j]      = grpStartIpAddr.u32Ip[j];
        grpLastIpAddrAdded.u32Ip[j]    = grpStartIpAddr.u32Ip[j];

        srcCurrentIpAddr.u32Ip[j]      = srcStartIpAddr.u32Ip[j];
        srcLastIpAddrAdded.u32Ip[j]    = srcStartIpAddr.u32Ip[j];
    }

   for (i = 0 ; i < numOfPrefixesToAdd ; )
    {
        /* make sure the prefix is not a MC prefix - not in format ff00::/8
           therefore the first octate must not start with 0xFF
           and the src prefix is not a MC prefix */
        if((grpCurrentIpAddr.arIP[0] == 0xFF)&&
           (srcCurrentIpAddr.arIP[0] != 0xFF))
        {
            /* add the current prefix */
            rc = cpssDxChIpLpmIpv6McEntryAdd(lpmDbId,
                                            vrId,
                                            grpCurrentIpAddr,
                                            prefixLength,
                                            srcCurrentIpAddr,
                                            prefixLength,
                                            &mcRouteLttEntry,
                                            override,
                                            tcamDefragmentationEnable);
            if (rc != GT_OK)
                break;

            numOfPrefixesAdded++;
            i++;
        }
        else
        {
            cpssOsPrintf("\n grpStartIpAddr= ");
            outputIPAddress(grpCurrentIpAddr.arIP, PRV_CPSS_DXCH_LPM_NUM_OF_OCTETS_IN_IPV6_PROTOCOL_CNS, GT_FALSE);
            cpssOsPrintf(" is not a valid MC address, or srcCurrentIpAddr= ");
            outputIPAddress(srcCurrentIpAddr.arIP, PRV_CPSS_DXCH_LPM_NUM_OF_OCTETS_IN_IPV6_PROTOCOL_CNS, GT_FALSE);
            cpssOsPrintf(" is not a valid source address\n");
            break;
        }

        for(j=0;j<4;j++)
        {
            grpLastIpAddrAdded.u32Ip[j]    = grpCurrentIpAddr.u32Ip[j];
            srcLastIpAddrAdded.u32Ip[j]    = srcCurrentIpAddr.u32Ip[j];
        }

        if(changeSrcAddr==GT_TRUE)
        {
            /* advance to the next IP address */
            if (srcCurrentIpAddr.arIP[srcFirstOctetToChange] < 255)
                srcCurrentIpAddr.arIP[srcFirstOctetToChange]++;
            else
            {
                srcCurrentIpAddr.arIP[srcFirstOctetToChange] = 0;
                if (srcCurrentIpAddr.arIP[srcSecondOctetToChange] < 255)
                    srcCurrentIpAddr.arIP[srcSecondOctetToChange]++;
                else
                {
                    srcCurrentIpAddr.arIP[srcSecondOctetToChange] = 0;
                    if (srcCurrentIpAddr.arIP[srcThirdOctetToChange] < 255)
                        srcCurrentIpAddr.arIP[srcThirdOctetToChange]++;
                    else
                    {
                        srcCurrentIpAddr.arIP[srcThirdOctetToChange] = 0;
                        if (srcCurrentIpAddr.arIP[srcForthOctetToChange] < 255)
                            srcCurrentIpAddr.arIP[srcForthOctetToChange]++;
                        else
                        {
                            srcCurrentIpAddr.arIP[srcForthOctetToChange] = 0;
                            if (srcCurrentIpAddr.arIP[srcFifthOctetToChange] < 255)
                                srcCurrentIpAddr.arIP[srcFifthOctetToChange]++;
                            else
                            {
                                srcCurrentIpAddr.arIP[srcFifthOctetToChange] = 0;
                                if (srcCurrentIpAddr.arIP[srcSixthOctetToChange] < 255)
                                    srcCurrentIpAddr.arIP[srcSixthOctetToChange]++;
                                else
                                {
                                    srcCurrentIpAddr.arIP[srcSixthOctetToChange] = 0;
                                    if (srcCurrentIpAddr.arIP[srcSeventhOctetToChange] < 255)
                                        srcCurrentIpAddr.arIP[srcSeventhOctetToChange]++;
                                    else
                                    {
                                        srcCurrentIpAddr.arIP[srcSeventhOctetToChange] = 0;
                                        if (srcCurrentIpAddr.arIP[srcEighthOctetToChange] < 255)
                                            srcCurrentIpAddr.arIP[srcEighthOctetToChange]++;
                                        else
                                        {
                                            srcCurrentIpAddr.arIP[srcEighthOctetToChange] = 0;
                                            if (srcCurrentIpAddr.arIP[srcNinthOctetToChange] < 255)
                                                srcCurrentIpAddr.arIP[srcNinthOctetToChange]++;
                                            else
                                            {
                                                srcCurrentIpAddr.arIP[srcNinthOctetToChange] = 0;
                                                if (srcCurrentIpAddr.arIP[srcTenthOctetToChange] < 255)
                                                    srcCurrentIpAddr.arIP[srcTenthOctetToChange]++;
                                                else
                                                {
                                                    srcCurrentIpAddr.arIP[srcTenthOctetToChange] = 0;
                                                    if (srcCurrentIpAddr.arIP[srcElevenOctetToChange] < 255)
                                                        srcCurrentIpAddr.arIP[srcElevenOctetToChange]++;
                                                    else
                                                    {
                                                        srcCurrentIpAddr.arIP[srcElevenOctetToChange] = 0;
                                                        if (srcCurrentIpAddr.arIP[srcTwelveOctetToChange] < 255)
                                                            srcCurrentIpAddr.arIP[srcTwelveOctetToChange]++;
                                                        else
                                                        {
                                                            srcCurrentIpAddr.arIP[srcTwelveOctetToChange] = 0;
                                                            if (srcCurrentIpAddr.arIP[srcThirteenOctetToChange] < 255)
                                                                srcCurrentIpAddr.arIP[srcThirteenOctetToChange]++;
                                                            else
                                                            {
                                                                srcCurrentIpAddr.arIP[srcThirteenOctetToChange] = 0;
                                                                if (srcCurrentIpAddr.arIP[srcFourteenOctetToChange] < 255)
                                                                    srcCurrentIpAddr.arIP[srcFourteenOctetToChange]++;
                                                                else
                                                                {
                                                                    srcCurrentIpAddr.arIP[srcFourteenOctetToChange] = 0;
                                                                    if (srcCurrentIpAddr.arIP[srcFifteenOctetToChange] < 255)
                                                                        srcCurrentIpAddr.arIP[srcFifteenOctetToChange]++;
                                                                    else
                                                                    {
                                                                        srcCurrentIpAddr.arIP[srcFifteenOctetToChange] = 0;
                                                                        if (srcCurrentIpAddr.arIP[srcSixteenOctetToChange] < 255)
                                                                            srcCurrentIpAddr.arIP[srcSixteenOctetToChange]++;
                                                                        else
                                                                        {
                                                                            rc = GT_NO_MORE;
                                                                            break;
                                                                        }
                                                                    }
                                                                }
                                                            }
                                                        }
                                                    }
                                                }
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }

        }
        else /* changeGrpAddr==GT_TRUE*/
        {
            /* advance to the next IP address */
            if (grpCurrentIpAddr.arIP[grpFirstOctetToChange] < 255)
                grpCurrentIpAddr.arIP[grpFirstOctetToChange]++;
            else
            {
                grpCurrentIpAddr.arIP[grpFirstOctetToChange] = 0;
                if (grpCurrentIpAddr.arIP[grpSecondOctetToChange] < 255)
                    grpCurrentIpAddr.arIP[grpSecondOctetToChange]++;
                else
                {
                    grpCurrentIpAddr.arIP[grpSecondOctetToChange] = 0;
                    if (grpCurrentIpAddr.arIP[grpThirdOctetToChange] < 255)
                        grpCurrentIpAddr.arIP[grpThirdOctetToChange]++;
                    else
                    {
                        grpCurrentIpAddr.arIP[grpThirdOctetToChange] = 0;
                        if (grpCurrentIpAddr.arIP[grpForthOctetToChange] < 255)
                            grpCurrentIpAddr.arIP[grpForthOctetToChange]++;
                        else
                        {
                            grpCurrentIpAddr.arIP[grpForthOctetToChange] = 0;
                            if (grpCurrentIpAddr.arIP[grpFifthOctetToChange] < 255)
                                grpCurrentIpAddr.arIP[grpFifthOctetToChange]++;
                            else
                            {
                                grpCurrentIpAddr.arIP[grpFifthOctetToChange] = 0;
                                if (grpCurrentIpAddr.arIP[grpSixthOctetToChange] < 255)
                                    grpCurrentIpAddr.arIP[grpSixthOctetToChange]++;
                                else
                                {
                                    grpCurrentIpAddr.arIP[grpSixthOctetToChange] = 0;
                                    if (grpCurrentIpAddr.arIP[grpSeventhOctetToChange] < 255)
                                        grpCurrentIpAddr.arIP[grpSeventhOctetToChange]++;
                                    else
                                    {
                                        grpCurrentIpAddr.arIP[grpSeventhOctetToChange] = 0;
                                        if (grpCurrentIpAddr.arIP[grpEighthOctetToChange] < 255)
                                            grpCurrentIpAddr.arIP[grpEighthOctetToChange]++;
                                        else
                                        {
                                            grpCurrentIpAddr.arIP[grpEighthOctetToChange] = 0;
                                            if (grpCurrentIpAddr.arIP[grpNinthOctetToChange] < 255)
                                                grpCurrentIpAddr.arIP[grpNinthOctetToChange]++;
                                            else
                                            {
                                                grpCurrentIpAddr.arIP[grpNinthOctetToChange] = 0;
                                                if (grpCurrentIpAddr.arIP[grpTenthOctetToChange] < 255)
                                                    grpCurrentIpAddr.arIP[grpTenthOctetToChange]++;
                                                else
                                                {
                                                    grpCurrentIpAddr.arIP[grpTenthOctetToChange] = 0;
                                                    if (grpCurrentIpAddr.arIP[grpElevenOctetToChange] < 255)
                                                        grpCurrentIpAddr.arIP[grpElevenOctetToChange]++;
                                                    else
                                                    {
                                                        grpCurrentIpAddr.arIP[grpElevenOctetToChange] = 0;
                                                        if (grpCurrentIpAddr.arIP[grpTwelveOctetToChange] < 255)
                                                            grpCurrentIpAddr.arIP[grpTwelveOctetToChange]++;
                                                        else
                                                        {
                                                            grpCurrentIpAddr.arIP[grpTwelveOctetToChange] = 0;
                                                            if (grpCurrentIpAddr.arIP[grpThirteenOctetToChange] < 255)
                                                                grpCurrentIpAddr.arIP[grpThirteenOctetToChange]++;
                                                            else
                                                            {
                                                                grpCurrentIpAddr.arIP[grpThirteenOctetToChange] = 0;
                                                                if (grpCurrentIpAddr.arIP[grpFourteenOctetToChange] < 255)
                                                                    grpCurrentIpAddr.arIP[grpFourteenOctetToChange]++;
                                                                else
                                                                {
                                                                    grpCurrentIpAddr.arIP[grpFourteenOctetToChange] = 0;
                                                                    if (grpCurrentIpAddr.arIP[grpFifteenOctetToChange] < 255)
                                                                        grpCurrentIpAddr.arIP[grpFifteenOctetToChange]++;
                                                                    else
                                                                    {
                                                                        grpCurrentIpAddr.arIP[grpFifteenOctetToChange] = 0;
                                                                        if (grpCurrentIpAddr.arIP[grpSixteenOctetToChange] < 255)
                                                                            grpCurrentIpAddr.arIP[grpSixteenOctetToChange]++;
                                                                        else
                                                                        {
                                                                            rc = GT_NO_MORE;
                                                                            break;
                                                                        }
                                                                    }
                                                                }
                                                            }
                                                        }
                                                    }
                                                }
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }

    /* print result */
    cpssOsPrintf("\nIpv6McPrefixAddMany results:");
    if (numOfPrefixesAdded == 0)
    {
        cpssOsPrintf("\n    No prefixes were added at all.\n");
    }
    else
    {
        cpssOsPrintf("\n    Group Prefixes ");
        outputIPAddress(grpStartIpAddr.arIP, PRV_CPSS_DXCH_LPM_NUM_OF_OCTETS_IN_IPV6_PROTOCOL_CNS, GT_FALSE);
        cpssOsPrintf(" - ");
        outputIPAddress(grpLastIpAddrAdded.arIP, PRV_CPSS_DXCH_LPM_NUM_OF_OCTETS_IN_IPV6_PROTOCOL_CNS, GT_FALSE);
        cpssOsPrintf("\n    Source Prefixes ");
        outputIPAddress(srcStartIpAddr.arIP, PRV_CPSS_DXCH_LPM_NUM_OF_OCTETS_IN_IPV6_PROTOCOL_CNS, GT_FALSE);
        cpssOsPrintf(" - ");
        outputIPAddress(srcLastIpAddrAdded.arIP, PRV_CPSS_DXCH_LPM_NUM_OF_OCTETS_IN_IPV6_PROTOCOL_CNS, GT_FALSE);
        cpssOsPrintf(" were added (%d/%d)\n",numOfPrefixesAdded,numOfPrefixesToAdd);
    }

    /* update output parameters */
    if (numOfPrefixesAddedPtr != NULL)
        *numOfPrefixesAddedPtr = numOfPrefixesAdded;
    if (grpLastIpAddrAddedPtr != NULL)
    {
        for(j=0;j<4;j++)
        {
            grpLastIpAddrAddedPtr->u32Ip[j] = grpLastIpAddrAdded.u32Ip[j];
        }
    }

    if (srcLastIpAddrAddedPtr != NULL)
    {
        for(j=0;j<4;j++)
        {
            srcLastIpAddrAddedPtr->u32Ip[j] = srcLastIpAddrAdded.u32Ip[j];
        }
    }

    return rc;
}
/*******************************************************************************
* cpssDxChIpLpmIpv6McPrefixGetManyByOctet
*
* DESCRIPTION:
*       This function tries to get many sequential IPv6 Multicast prefixes (that were
*       added by cpssDxChIpLpmIpv6McPrefixAddManyByOctet) and returns the number of
*       successfully retrieved prefixes.
*
* APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       lpmDbId               - the LPM DB id
*       vrId                  - the virtual router id
*       grpStartIpAddr        - the first group address to add
*       srcStartIpAddr        - the first source address to add
*       numOfPrefixesToGet    - the number of prefixes to get
*       changeGrpAddr         - GT_TRUE: change the group address
*                               GT_FALSE: don't change the group address
*       grpFirstOctetToChange - first octet to change in the prefix address.
*                               value of the octet can be up to 255
*       grpSecondOctetToChange- second octet to change in the prefix address.
*                               value of the octet can be up to 255
*       grpThirdOctetToChange - third octet to change in the prefix address.
*                               value of the octet can be up to 255
*       grpForthOctetToChange - forth octet to change in the prefix address.
*                               value of the octet can be up to 255
*       grpFifthOctetToChange - fifth octet to change in the prefix address.
*                               value of the octet can be up to 255
*       grpSixthOctetToChange - sixth octet to change in the prefix address.
*                               value of the octet can be up to 255
*       grpSeventhOctetToChange  - seventh octet to change in the prefix address.
*                               value of the octet can be up to 255
*       grpEighthOctetToChange- eighth octet to change in the prefix address.
*                               value of the octet can be up to 255
*       grpNinthOctetToChange - ninth octet to change in the prefix address.
*                               value of the octet can be up to 255
*       grpTenthOctetToChange - tenth octet to change in the prefix address.
*                               value of the octet can be up to 255
*       grpElevenOctetToChange- eleven octet to change in the prefix address.
*                               value of the octet can be up to 255
*       grpTwelveOctetToChange- twelve octet to change in the prefix address.
*                               value of the octet can be up to 255
*       grpThirteenOctetToChange - thirteen octet to change in the prefix address.
*                               value of the octet can be up to 255
*       grpFourteenOctetToChange - fourteen octet to change in the prefix address.
*                               value of the octet can be up to 255
*       grpFifteenOctetToChange  - fifteen octet to change in the prefix address.
*                               value of the octet can be up to 255
*       grpSixteenOctetToChange  - sixteen octet to change in the prefix address.
*                               value of the octet can be up to 255
*       changeSrcAddr         - GT_TRUE: change the source address
*                               GT_FALSE: don't change the source address
*       srcFirstOctetToChange - first octet to change in the prefix address.
*                               value of the octet can be up to 255
*       srcSecondOctetToChange- second octet to change in the prefix address.
*                               value of the octet can be up to 255
*       srcThirdOctetToChange - third octet to change in the prefix address.
*                               value of the octet can be up to 255
*       srcForthOctetToChange - forth octet to change in the prefix address.
*                               value of the octet can be up to 255
*       srcFifthOctetToChange - fifth octet to change in the prefix address.
*                               value of the octet can be up to 255
*       srcSixthOctetToChange - sixth octet to change in the prefix address.
*                               value of the octet can be up to 255
*       srcSeventhOctetToChange  - seventh octet to change in the prefix address.
*                               value of the octet can be up to 255
*       srcEighthOctetToChange- eighth octet to change in the prefix address.
*                               value of the octet can be up to 255
*       srcNinthOctetToChange - ninth octet to change in the prefix address.
*                               value of the octet can be up to 255
*       srcTenthOctetToChange - tenth octet to change in the prefix address.
*                               value of the octet can be up to 255
*       srcElevenOctetToChange- eleven octet to change in the prefix address.
*                               value of the octet can be up to 255
*       srcTwelveOctetToChange- twelve octet to change in the prefix address.
*                               value of the octet can be up to 255
*       srcThirteenOctetToChange - thirteen octet to change in the prefix address.
*                               value of the octet can be up to 255
*       srcFourteenOctetToChange - fourteen octet to change in the prefix address.
*                               value of the octet can be up to 255
*       srcFifteenOctetToChange  - fifteen octet to change in the prefix address.
*                               value of the octet can be up to 255
*       srcSixteenOctetToChange  - sixteen octet to change in the prefix address.
*                               value of the octet can be up to 255
* OUTPUTS:
*       numOfRetrievedPrefixesPtr - points to the nubmer of prefixes that were
*                                   successfully retrieved (NULL to ignore)
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - on wrong devNum or vrId.
*       GT_HW_ERROR              - on Hardware error.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       This debug function is intended to check the number of the prefixes
*       that were be added.
*******************************************************************************/
GT_STATUS cpssDxChIpLpmIpv6McPrefixGetManyByOctet
(
    IN  GT_U32      lpmDbId,
    IN  GT_U32      vrId,
    IN  GT_IPV6ADDR grpStartIpAddr,
    IN  GT_IPV6ADDR srcStartIpAddr,
    IN  GT_U32      numOfPrefixesToGet,
    IN  GT_BOOL     changeGrpAddr,
    IN  GT_U32      grpFirstOctetToChange,
    IN  GT_U32      grpSecondOctetToChange,
    IN  GT_U32      grpThirdOctetToChange,
    IN  GT_U32      grpForthOctetToChange,
    IN  GT_U32      grpFifthOctetToChange,
    IN  GT_U32      grpSixthOctetToChange,
    IN  GT_U32      grpSeventhOctetToChange,
    IN  GT_U32      grpEighthOctetToChange,
    IN  GT_U32      grpNinthOctetToChange,
    IN  GT_U32      grpTenthOctetToChange,
    IN  GT_U32      grpElevenOctetToChange,
    IN  GT_U32      grpTwelveOctetToChange,
    IN  GT_U32      grpThirteenOctetToChange,
    IN  GT_U32      grpFourteenOctetToChange,
    IN  GT_U32      grpFifteenOctetToChange,
    IN  GT_U32      grpSixteenOctetToChange,
    IN  GT_BOOL     changeSrcAddr,
    IN  GT_U32      srcFirstOctetToChange,
    IN  GT_U32      srcSecondOctetToChange,
    IN  GT_U32      srcThirdOctetToChange,
    IN  GT_U32      srcForthOctetToChange,
    IN  GT_U32      srcFifthOctetToChange,
    IN  GT_U32      srcSixthOctetToChange,
    IN  GT_U32      srcSeventhOctetToChange,
    IN  GT_U32      srcEighthOctetToChange,
    IN  GT_U32      srcNinthOctetToChange,
    IN  GT_U32      srcTenthOctetToChange,
    IN  GT_U32      srcElevenOctetToChange,
    IN  GT_U32      srcTwelveOctetToChange,
    IN  GT_U32      srcThirteenOctetToChange,
    IN  GT_U32      srcFourteenOctetToChange,
    IN  GT_U32      srcFifteenOctetToChange,
    IN  GT_U32      srcSixteenOctetToChange,
    OUT GT_U32      *numOfRetrievedPrefixesPtr
)
{
    GT_STATUS                               rc = GT_OK;
    GT_U32                                  i = 0;
    GT_U32                                  j = 0;
    GT_IPV6ADDR                             grpCurrentIpAddr;
    GT_IPV6ADDR                             srcCurrentIpAddr;
    GT_U32                                  numOfRetrievedPrefixes = 0;
    GT_IPV6ADDR                             grpLastIpAddrAdded;
    GT_IPV6ADDR                             srcLastIpAddrAdded;
    CPSS_DXCH_IP_LTT_ENTRY_STC              mcRouteLttEntry;
    GT_U32                                  prefixLength = 128;
    GT_BOOL                                 octetsArray[PRV_CPSS_DXCH_LPM_NUM_OF_OCTETS_IN_IPV6_PROTOCOL_CNS]={GT_FALSE,GT_FALSE,GT_FALSE,GT_FALSE,
                                                            GT_FALSE,GT_FALSE,GT_FALSE,GT_FALSE,
                                                            GT_FALSE,GT_FALSE,GT_FALSE,GT_FALSE,
                                                            GT_FALSE,GT_FALSE,GT_FALSE,GT_FALSE};/* array to hold the octets already selected */

    GT_U32                                  octetsOrderToChangeArray[PRV_CPSS_DXCH_LPM_NUM_OF_OCTETS_IN_IPV6_PROTOCOL_CNS];
    GT_U32                                  tcamGroupRowIndex=0;
    GT_U32                                  tcamSrcRowIndex=0;

    cpssOsMemSet(&mcRouteLttEntry, 0, sizeof(CPSS_DXCH_IP_LTT_ENTRY_STC));

    if(((changeGrpAddr==GT_TRUE)&&(changeSrcAddr==GT_TRUE))||
       ((changeGrpAddr==GT_FALSE)&&(changeSrcAddr==GT_FALSE)))
    {
        cpssOsPrintf("changeGrpAddr should be different from changeSrcAddr\n");
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    if(changeGrpAddr==GT_TRUE)
    {
        octetsOrderToChangeArray[0] = grpFirstOctetToChange;
        octetsOrderToChangeArray[1] = grpSecondOctetToChange;
        octetsOrderToChangeArray[2] = grpThirdOctetToChange;
        octetsOrderToChangeArray[3] = grpForthOctetToChange;
        octetsOrderToChangeArray[4] = grpFifthOctetToChange;
        octetsOrderToChangeArray[5] = grpSixthOctetToChange;
        octetsOrderToChangeArray[6] = grpSeventhOctetToChange;
        octetsOrderToChangeArray[7] = grpEighthOctetToChange;
        octetsOrderToChangeArray[8] = grpNinthOctetToChange;
        octetsOrderToChangeArray[9] = grpTenthOctetToChange;
        octetsOrderToChangeArray[10] = grpElevenOctetToChange;
        octetsOrderToChangeArray[11] = grpTwelveOctetToChange;
        octetsOrderToChangeArray[12] = grpThirteenOctetToChange;
        octetsOrderToChangeArray[13] = grpFourteenOctetToChange;
        octetsOrderToChangeArray[14] = grpFifteenOctetToChange;
        octetsOrderToChangeArray[15] = grpSixteenOctetToChange;
    }
    if(changeSrcAddr==GT_TRUE)
    {
        octetsOrderToChangeArray[0] = srcFirstOctetToChange;
        octetsOrderToChangeArray[1] = srcSecondOctetToChange;
        octetsOrderToChangeArray[2] = srcThirdOctetToChange;
        octetsOrderToChangeArray[3] = srcForthOctetToChange;
        octetsOrderToChangeArray[4] = srcFifthOctetToChange;
        octetsOrderToChangeArray[5] = srcSixthOctetToChange;
        octetsOrderToChangeArray[6] = srcSeventhOctetToChange;
        octetsOrderToChangeArray[7] = srcEighthOctetToChange;
        octetsOrderToChangeArray[8] = srcNinthOctetToChange;
        octetsOrderToChangeArray[9] = srcTenthOctetToChange;
        octetsOrderToChangeArray[10] = srcElevenOctetToChange;
        octetsOrderToChangeArray[11] = srcTwelveOctetToChange;
        octetsOrderToChangeArray[12] = srcThirteenOctetToChange;
        octetsOrderToChangeArray[13] = srcFourteenOctetToChange;
        octetsOrderToChangeArray[14] = srcFifteenOctetToChange;
        octetsOrderToChangeArray[15] = srcSixteenOctetToChange;
    }

    for(j=0;j<PRV_CPSS_DXCH_LPM_NUM_OF_OCTETS_IN_IPV6_PROTOCOL_CNS;j++)
    {
        if(octetsOrderToChangeArray[j]>15)
        {
            cpssOsPrintf("octetToChange can not be bigger then 15\n");
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
        }
    }

    for(j=0;j<PRV_CPSS_DXCH_LPM_NUM_OF_OCTETS_IN_IPV6_PROTOCOL_CNS;j++)
    {
        /* make sure octet are not selected twice */
        if(octetsArray[octetsOrderToChangeArray[j]]==GT_FALSE)
        {
            octetsArray[octetsOrderToChangeArray[j]]=GT_TRUE;
        }
        else
        {
            cpssOsPrintf("octetsOrderToChangeArray[j]=%d was selected twice\n",octetsOrderToChangeArray[j]);
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
        }
    }

    cpssOsMemSet(&grpCurrentIpAddr, 0, sizeof(GT_IPV6ADDR));
    cpssOsMemSet(&srcCurrentIpAddr, 0, sizeof(GT_IPV6ADDR));

    for(j=0;j<4;j++)
    {
        grpCurrentIpAddr.u32Ip[j]      = grpStartIpAddr.u32Ip[j];
        grpLastIpAddrAdded.u32Ip[j]    = grpStartIpAddr.u32Ip[j];

        srcCurrentIpAddr.u32Ip[j]      = srcStartIpAddr.u32Ip[j];
        srcLastIpAddrAdded.u32Ip[j]    = srcStartIpAddr.u32Ip[j];
    }

    for (i = 0 ; i < numOfPrefixesToGet ; )
    {
        /* make sure the prefix is a MC prefix - in format ff00::/8
           therefore the first octate must start with 0xFF,
           and the src prefix is not a MC prefix */
        if((grpCurrentIpAddr.arIP[0] == 0xFF)&&
           (srcCurrentIpAddr.arIP[0] != 0xFF))
        {

            /* get the current prefix */
            rc =  cpssDxChIpLpmIpv6McEntrySearch(lpmDbId,
                                                 vrId,
                                                 grpCurrentIpAddr,
                                                 prefixLength,
                                                 srcCurrentIpAddr,
                                                 prefixLength,
                                                 &mcRouteLttEntry,
                                                 &tcamGroupRowIndex,
                                                 &tcamSrcRowIndex);
             if (rc != GT_OK)
                    break;

                numOfRetrievedPrefixes++;
                i++;
        }
        else
        {
            cpssOsPrintf("\n grpStartIpAddr= ");
            outputIPAddress(grpCurrentIpAddr.arIP, PRV_CPSS_DXCH_LPM_NUM_OF_OCTETS_IN_IPV6_PROTOCOL_CNS, GT_FALSE);
            cpssOsPrintf(" is not a valid MC address, or srcCurrentIpAddr= ");
            outputIPAddress(srcCurrentIpAddr.arIP, PRV_CPSS_DXCH_LPM_NUM_OF_OCTETS_IN_IPV6_PROTOCOL_CNS, GT_FALSE);
            cpssOsPrintf(" is not a valid source address\n");
            break;
        }

        for(j=0;j<4;j++)
        {
            grpLastIpAddrAdded.u32Ip[j]    = grpCurrentIpAddr.u32Ip[j];
            srcLastIpAddrAdded.u32Ip[j]    = srcCurrentIpAddr.u32Ip[j];
        }

        if(changeSrcAddr==GT_TRUE)
        {
            /* advance to the next IP address */
            if (srcCurrentIpAddr.arIP[srcFirstOctetToChange] < 255)
                srcCurrentIpAddr.arIP[srcFirstOctetToChange]++;
            else
            {
                srcCurrentIpAddr.arIP[srcFirstOctetToChange] = 0;
                if (srcCurrentIpAddr.arIP[srcSecondOctetToChange] < 255)
                    srcCurrentIpAddr.arIP[srcSecondOctetToChange]++;
                else
                {
                    srcCurrentIpAddr.arIP[srcSecondOctetToChange] = 0;
                    if (srcCurrentIpAddr.arIP[srcThirdOctetToChange] < 255)
                        srcCurrentIpAddr.arIP[srcThirdOctetToChange]++;
                    else
                    {
                        srcCurrentIpAddr.arIP[srcThirdOctetToChange] = 0;
                        if (srcCurrentIpAddr.arIP[srcForthOctetToChange] < 255)
                            srcCurrentIpAddr.arIP[srcForthOctetToChange]++;
                        else
                        {
                            srcCurrentIpAddr.arIP[srcForthOctetToChange] = 0;
                            if (srcCurrentIpAddr.arIP[srcFifthOctetToChange] < 255)
                                srcCurrentIpAddr.arIP[srcFifthOctetToChange]++;
                            else
                            {
                                srcCurrentIpAddr.arIP[srcFifthOctetToChange] = 0;
                                if (srcCurrentIpAddr.arIP[srcSixthOctetToChange] < 255)
                                    srcCurrentIpAddr.arIP[srcSixthOctetToChange]++;
                                else
                                {
                                    srcCurrentIpAddr.arIP[srcSixthOctetToChange] = 0;
                                    if (srcCurrentIpAddr.arIP[srcSeventhOctetToChange] < 255)
                                        srcCurrentIpAddr.arIP[srcSeventhOctetToChange]++;
                                    else
                                    {
                                        srcCurrentIpAddr.arIP[srcSeventhOctetToChange] = 0;
                                        if (srcCurrentIpAddr.arIP[srcEighthOctetToChange] < 255)
                                            srcCurrentIpAddr.arIP[srcEighthOctetToChange]++;
                                        else
                                        {
                                            srcCurrentIpAddr.arIP[srcEighthOctetToChange] = 0;
                                            if (srcCurrentIpAddr.arIP[srcNinthOctetToChange] < 255)
                                                srcCurrentIpAddr.arIP[srcNinthOctetToChange]++;
                                            else
                                            {
                                                srcCurrentIpAddr.arIP[srcNinthOctetToChange] = 0;
                                                if (srcCurrentIpAddr.arIP[srcTenthOctetToChange] < 255)
                                                    srcCurrentIpAddr.arIP[srcTenthOctetToChange]++;
                                                else
                                                {
                                                    srcCurrentIpAddr.arIP[srcTenthOctetToChange] = 0;
                                                    if (srcCurrentIpAddr.arIP[srcElevenOctetToChange] < 255)
                                                        srcCurrentIpAddr.arIP[srcElevenOctetToChange]++;
                                                    else
                                                    {
                                                        srcCurrentIpAddr.arIP[srcElevenOctetToChange] = 0;
                                                        if (srcCurrentIpAddr.arIP[srcTwelveOctetToChange] < 255)
                                                            srcCurrentIpAddr.arIP[srcTwelveOctetToChange]++;
                                                        else
                                                        {
                                                            srcCurrentIpAddr.arIP[srcTwelveOctetToChange] = 0;
                                                            if (srcCurrentIpAddr.arIP[srcThirteenOctetToChange] < 255)
                                                                srcCurrentIpAddr.arIP[srcThirteenOctetToChange]++;
                                                            else
                                                            {
                                                                srcCurrentIpAddr.arIP[srcThirteenOctetToChange] = 0;
                                                                if (srcCurrentIpAddr.arIP[srcFourteenOctetToChange] < 255)
                                                                    srcCurrentIpAddr.arIP[srcFourteenOctetToChange]++;
                                                                else
                                                                {
                                                                    srcCurrentIpAddr.arIP[srcFourteenOctetToChange] = 0;
                                                                    if (srcCurrentIpAddr.arIP[srcFifteenOctetToChange] < 255)
                                                                        srcCurrentIpAddr.arIP[srcFifteenOctetToChange]++;
                                                                    else
                                                                    {
                                                                        srcCurrentIpAddr.arIP[srcFifteenOctetToChange] = 0;
                                                                        if (srcCurrentIpAddr.arIP[srcSixteenOctetToChange] < 255)
                                                                            srcCurrentIpAddr.arIP[srcSixteenOctetToChange]++;
                                                                        else
                                                                        {
                                                                            rc = GT_NO_MORE;
                                                                            break;
                                                                        }
                                                                    }
                                                                }
                                                            }
                                                        }
                                                    }
                                                }
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
        else /* changeGrpAddr==GT_TRUE*/
        {
            /* advance to the next IP address */
            if (grpCurrentIpAddr.arIP[grpFirstOctetToChange] < 255)
                grpCurrentIpAddr.arIP[grpFirstOctetToChange]++;
            else
            {
                grpCurrentIpAddr.arIP[grpFirstOctetToChange] = 0;
                if (grpCurrentIpAddr.arIP[grpSecondOctetToChange] < 255)
                    grpCurrentIpAddr.arIP[grpSecondOctetToChange]++;
                else
                {
                    grpCurrentIpAddr.arIP[grpSecondOctetToChange] = 0;
                    if (grpCurrentIpAddr.arIP[grpThirdOctetToChange] < 255)
                        grpCurrentIpAddr.arIP[grpThirdOctetToChange]++;
                    else
                    {
                        grpCurrentIpAddr.arIP[grpThirdOctetToChange] = 0;
                        if (grpCurrentIpAddr.arIP[grpForthOctetToChange] < 255)
                            grpCurrentIpAddr.arIP[grpForthOctetToChange]++;
                        else
                        {
                            grpCurrentIpAddr.arIP[grpForthOctetToChange] = 0;
                            if (grpCurrentIpAddr.arIP[grpFifthOctetToChange] < 255)
                                grpCurrentIpAddr.arIP[grpFifthOctetToChange]++;
                            else
                            {
                                grpCurrentIpAddr.arIP[grpFifthOctetToChange] = 0;
                                if (grpCurrentIpAddr.arIP[grpSixthOctetToChange] < 255)
                                    grpCurrentIpAddr.arIP[grpSixthOctetToChange]++;
                                else
                                {
                                    grpCurrentIpAddr.arIP[grpSixthOctetToChange] = 0;
                                    if (grpCurrentIpAddr.arIP[grpSeventhOctetToChange] < 255)
                                        grpCurrentIpAddr.arIP[grpSeventhOctetToChange]++;
                                    else
                                    {
                                        grpCurrentIpAddr.arIP[grpSeventhOctetToChange] = 0;
                                        if (grpCurrentIpAddr.arIP[grpEighthOctetToChange] < 255)
                                            grpCurrentIpAddr.arIP[grpEighthOctetToChange]++;
                                        else
                                        {
                                            grpCurrentIpAddr.arIP[grpEighthOctetToChange] = 0;
                                            if (grpCurrentIpAddr.arIP[grpNinthOctetToChange] < 255)
                                                grpCurrentIpAddr.arIP[grpNinthOctetToChange]++;
                                            else
                                            {
                                                grpCurrentIpAddr.arIP[grpNinthOctetToChange] = 0;
                                                if (grpCurrentIpAddr.arIP[grpTenthOctetToChange] < 255)
                                                    grpCurrentIpAddr.arIP[grpTenthOctetToChange]++;
                                                else
                                                {
                                                    grpCurrentIpAddr.arIP[grpTenthOctetToChange] = 0;
                                                    if (grpCurrentIpAddr.arIP[grpElevenOctetToChange] < 255)
                                                        grpCurrentIpAddr.arIP[grpElevenOctetToChange]++;
                                                    else
                                                    {
                                                        grpCurrentIpAddr.arIP[grpElevenOctetToChange] = 0;
                                                        if (grpCurrentIpAddr.arIP[grpTwelveOctetToChange] < 255)
                                                            grpCurrentIpAddr.arIP[grpTwelveOctetToChange]++;
                                                        else
                                                        {
                                                            grpCurrentIpAddr.arIP[grpTwelveOctetToChange] = 0;
                                                            if (grpCurrentIpAddr.arIP[grpThirteenOctetToChange] < 255)
                                                                grpCurrentIpAddr.arIP[grpThirteenOctetToChange]++;
                                                            else
                                                            {
                                                                grpCurrentIpAddr.arIP[grpThirteenOctetToChange] = 0;
                                                                if (grpCurrentIpAddr.arIP[grpFourteenOctetToChange] < 255)
                                                                    grpCurrentIpAddr.arIP[grpFourteenOctetToChange]++;
                                                                else
                                                                {
                                                                    grpCurrentIpAddr.arIP[grpFourteenOctetToChange] = 0;
                                                                    if (grpCurrentIpAddr.arIP[grpFifteenOctetToChange] < 255)
                                                                        grpCurrentIpAddr.arIP[grpFifteenOctetToChange]++;
                                                                    else
                                                                    {
                                                                        grpCurrentIpAddr.arIP[grpFifteenOctetToChange] = 0;
                                                                        if (grpCurrentIpAddr.arIP[grpSixteenOctetToChange] < 255)
                                                                            grpCurrentIpAddr.arIP[grpSixteenOctetToChange]++;
                                                                        else
                                                                        {
                                                                            rc = GT_NO_MORE;
                                                                            break;
                                                                        }
                                                                    }
                                                                }
                                                            }
                                                        }
                                                    }
                                                }
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }

    /* print result */
    cpssOsPrintf("\nIpv6McPrefixGetMany results:");
    if (numOfRetrievedPrefixes == 0)
    {
        cpssOsPrintf("\n    No prefixes at all.\n");
    }
    else
    {

        cpssOsPrintf("\n    Group Prefixes ");
        outputIPAddress(grpStartIpAddr.arIP, PRV_CPSS_DXCH_LPM_NUM_OF_OCTETS_IN_IPV6_PROTOCOL_CNS, GT_FALSE);
        cpssOsPrintf(" - ");
        outputIPAddress(grpLastIpAddrAdded.arIP, PRV_CPSS_DXCH_LPM_NUM_OF_OCTETS_IN_IPV6_PROTOCOL_CNS, GT_FALSE);
        cpssOsPrintf("\n    Source Prefixes ");
        outputIPAddress(srcStartIpAddr.arIP, PRV_CPSS_DXCH_LPM_NUM_OF_OCTETS_IN_IPV6_PROTOCOL_CNS, GT_FALSE);
        cpssOsPrintf(" - ");
        outputIPAddress(srcLastIpAddrAdded.arIP, PRV_CPSS_DXCH_LPM_NUM_OF_OCTETS_IN_IPV6_PROTOCOL_CNS, GT_FALSE);
        cpssOsPrintf(" were retrieved (%d/%d)\n",numOfRetrievedPrefixes,numOfPrefixesToGet);
    }

    /* update output parameters */
    if (numOfRetrievedPrefixesPtr != NULL)
        *numOfRetrievedPrefixesPtr = numOfRetrievedPrefixes;
    return rc;
}

/*******************************************************************************
* internal_cpssDxChIpLpmDbgHwOctetPerBlockPrint
*
* DESCRIPTION:
*   Print Octet per Block debug information for a specific LPM DB
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       lpmDbId     - The LPM DB id
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_NOT_FOUND             - if can't find the lpm DB
*
* COMMENTS:
*       None.
*******************************************************************************/
static GT_STATUS internal_cpssDxChIpLpmDbgHwOctetPerBlockPrint
(
    IN GT_U32                           lpmDbId
)
{
    PRV_CPSS_DXCH_LPM_SHADOW_STC        *lpmDbPtr,tmpLpmDb;
    PRV_CPSS_DXCH_LPM_HW_ENT            lpmHw;

    tmpLpmDb.lpmDbId = lpmDbId;
    lpmDbPtr = prvCpssSlSearch(prvCpssDxChIplpmDbSL,&tmpLpmDb);
    if (lpmDbPtr == NULL)
    {
        /* can't find the lpm DB */
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_FOUND, LOG_ERROR_NO_MSG);
    }

    lpmHw = prvCpssDxChLpmGetHwType(lpmDbPtr->shadowType);
    switch (lpmHw)
    {
        case PRV_CPSS_DXCH_LPM_HW_TCAM_E:
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_APPLICABLE_DEVICE, LOG_ERROR_NO_MSG);

        case PRV_CPSS_DXCH_LPM_HW_RAM_E:
            prvCpssDxChLpmDbgHwOctetPerBlockPrint((PRV_CPSS_DXCH_LPM_RAM_SHADOWS_DB_STC*)(lpmDbPtr->shadow));
            break;

        default:
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }
    return GT_OK;
}
/*******************************************************************************
* cpssDxChIpLpmDbgHwOctetPerBlockPrint
*
* DESCRIPTION:
*   Print Octet per Block debug information for a specific LPM DB
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       lpmDbId     - The LPM DB id
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_NOT_FOUND             - if can't find the lpm DB
*
* COMMENTS:
*       None.
*******************************************************************************/
GT_STATUS cpssDxChIpLpmDbgHwOctetPerBlockPrint
(
    IN GT_U32                           lpmDbId
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChIpLpmDbgHwOctetPerBlockPrint);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, lpmDbId));

    rc = internal_cpssDxChIpLpmDbgHwOctetPerBlockPrint(lpmDbId);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, lpmDbId));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChIpLpmDbgHwOctetPerProtocolPrint
*
* DESCRIPTION:
*   Print Octet per Protocol debug information for a specific LPM DB
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       lpmDbId     - The LPM DB id
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_NOT_FOUND             - if can't find the lpm DB
*
* COMMENTS:
*       None.
*******************************************************************************/
static GT_STATUS internal_cpssDxChIpLpmDbgHwOctetPerProtocolPrint
(
    IN GT_U32                           lpmDbId
)
{
    PRV_CPSS_DXCH_LPM_SHADOW_STC        *lpmDbPtr,tmpLpmDb;
    PRV_CPSS_DXCH_LPM_HW_ENT            lpmHw;

    tmpLpmDb.lpmDbId = lpmDbId;
    lpmDbPtr = prvCpssSlSearch(prvCpssDxChIplpmDbSL,&tmpLpmDb);
    if (lpmDbPtr == NULL)
    {
        /* can't find the lpm DB */
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_FOUND, LOG_ERROR_NO_MSG);
    }

    lpmHw = prvCpssDxChLpmGetHwType(lpmDbPtr->shadowType);
    switch (lpmHw)
    {
        case PRV_CPSS_DXCH_LPM_HW_TCAM_E:
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_APPLICABLE_DEVICE, LOG_ERROR_NO_MSG);

        case PRV_CPSS_DXCH_LPM_HW_RAM_E:
            prvCpssDxChLpmDbgHwOctetPerProtocolPrint((PRV_CPSS_DXCH_LPM_RAM_SHADOWS_DB_STC*)(lpmDbPtr->shadow));
            break;
        default:
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }
    return GT_OK;
}

/*******************************************************************************
* cpssDxChIpLpmDbgHwOctetPerProtocolPrint
*
* DESCRIPTION:
*   Print Octet per Protocol debug information for a specific LPM DB
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       lpmDbId     - The LPM DB id
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_NOT_FOUND             - if can't find the lpm DB
*
* COMMENTS:
*       None.
*******************************************************************************/
GT_STATUS cpssDxChIpLpmDbgHwOctetPerProtocolPrint
(
    IN GT_U32                           lpmDbId
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChIpLpmDbgHwOctetPerProtocolPrint);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, lpmDbId));

    rc = internal_cpssDxChIpLpmDbgHwOctetPerProtocolPrint(lpmDbId);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, lpmDbId));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

