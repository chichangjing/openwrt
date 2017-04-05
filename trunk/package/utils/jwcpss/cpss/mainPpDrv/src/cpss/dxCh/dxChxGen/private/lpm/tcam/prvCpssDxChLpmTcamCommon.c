/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* prvCpssDxChLpmTcamCommon.c
*
* DESCRIPTION:
*       This file includes functions declarations for controlling the IP
*       tables and structures, and structures definitions for shadow management.
*
* DEPENDENCIES:
*       None.
*
* FILE REVISION NUMBER:
*       $Revision: 90 $
*
********************************************************************************/

#include <cpssCommon/cpssPresteraDefs.h>
#include <cpss/generic/cpssCommonDefs.h>
#include <cpss/extServices/private/prvCpssBindFunc.h>
#include <cpssCommon/private/prvCpssSkipList.h>
#include <cpss/generic/ip/cpssIpTypes.h>
#include <cpss/dxCh/dxChxGen/pcl/cpssDxChPcl.h>
#include <cpss/dxCh/dxChxGen/config/private/prvCpssDxChInfo.h>
#include <cpss/dxCh/dxChxGen/ip/cpssDxChIpTypes.h>
#include <cpss/dxCh/dxChxGen/private/lpm/tcam/prvCpssDxChLpmTcamMgm.h>
#include <cpss/dxCh/dxChxGen/private/lpm/tcam/prvCpssDxChLpmTcamPatTrie.h>
#include <cpss/dxCh/dxChxGen/private/lpm/tcam/prvCpssDxChLpmTcamTypes.h>
#include <cpss/dxCh/dxChxGen/ipLpmEngine/cpssDxChIpLpmTypes.h>
#include <cpss/dxCh/dxChxGen/private/lpm/tcam/prvCpssDxChLpmTcam.h>
#include <cpss/dxCh/dxChxGen/ip/cpssDxChIp.h>
#include <cpss/dxCh/dxChxGen/private/lpm/tcam/prvCpssDxChLpmTcamIpMc.h>
#include <cpss/dxCh/dxChxGen/private/lpm/tcam/prvCpssDxChLpmTcamIpUc.h>
#include <cpss/dxCh/dxChxGen/private/lpm/tcam/prvCpssDxChLpmTcamEntriesHandler.h>
#include <cpss/dxCh/dxChxGen/resourceManager/cpssDxChTcamManager.h>
#include <cpss/dxCh/dxChxGen/private/lpm/tcam/prvCpssDxChLpmTcamDispatcher.h>
#include <cpss/dxCh/dxChxGen/private/lpm/prvCpssDxChLpmUtils.h>

#ifdef OS_MALLOC_MEMORY_LEAKAGE_DBG
static GT_VOID myCpssOsFree
(
    IN GT_VOID* const memblock
)
{
    cpssOsFree_MemoryLeakageDbg(memblock,__FILE__,__LINE__);
}
#endif /*OS_MALLOC_MEMORY_LEAKAGE_DBG*/

/****************************************************************************
* Debug                                                                     *
****************************************************************************/

#define CPSS_DXCH_MAX_VR_ID_CNS                 1024
#define CPSS_DXCH2_MAX_VR_ID_CNS                1
#define CPSS_DXCH3_MAX_VR_ID_CNS                4096
#define CPSS_DXCH_XCAT_MAX_VR_ID_CNS            4096
#define CPSS_DXCH_XCAT_PBR_MAX_VR_ID_CNS        1024
#define CPSS_DXCH2_MULTIPLE_VRS_MAX_VR_ID_CNS   1024 /* 5120 */

/***************************************************************************
* Externals
****************************************************************************/
GT_U8 cpssDxChDefaultMcIpAddr[2][16] = {{0xE0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
                                  {0xFF,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}};
GT_U32 cpssDxChDefaultMcIpPrefixLen[2] = {4,8};

/* number of rows in the TCAM */
GT_U32  tcamRows = 0;

/***************************************************************************
* Forward function declarations
****************************************************************************/

/*******************************************************************************
* Internal usage environment parameters
*******************************************************************************/


/*******************************************************************************
* Ip core common environment parameters
*******************************************************************************/



/*******************************************************************************
* prvCpssDxChLpmTcamMoveToLocation
*
* DESCRIPTION:
*       Copy single entry from one TCAM location to another TCAM location
*       and to invalidate the entry in the original location
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* NOT APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       fromPtr           - points to the original hardware TCAM location
*       toPtr             - points to the new hardware TCAM location
*       clientCookiePtr   - points to the client cookie associated with the entry
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on error
*
* COMMENTS:
*       None.
*******************************************************************************/
GT_STATUS prvCpssDxChLpmTcamMoveToLocation
(
    IN  CPSS_DXCH_TCAM_MANAGER_TCAM_LOCATION_STC    *fromPtr,
    IN  CPSS_DXCH_TCAM_MANAGER_TCAM_LOCATION_STC    *toPtr,
    IN  GT_VOID                                     *clientCookiePtr
)
{
    GT_STATUS ret = GT_OK;
    PRV_CPSS_DXCH_LPM_TCAM_PAT_TRIE_FUNCS_OBJ_STC    *ipTcamFuncsPtr;
    GT_PTR moveFromTcamRulePtr = (GT_PTR)(fromPtr);
    GT_PTR moveToTcamRulePtr = (GT_PTR)(toPtr);
    PRV_CPSS_DXCH_LPM_TCAM_PAT_TRIE_NODE_STC  *trieNodePtr =
        (PRV_CPSS_DXCH_LPM_TCAM_PAT_TRIE_NODE_STC*)clientCookiePtr;
    ipTcamFuncsPtr = trieNodePtr->ipTcamFuncsPtr;

    /* if source and target are the same, nothing to do */
    if ((fromPtr->row == toPtr->row) && (fromPtr->column == toPtr->column))
    {
        return GT_OK;
    }
    if (trieNodePtr->size == 0)
    {
        /* pbr mc defaults dummy trie node */
        return GT_OK;
    }

    /* mark that ipTcamMovePrefix is called with TCAM locations by setting
      bit 14 in the vrId */
    ret = ipTcamFuncsPtr->ipTcamMovePrefix(trieNodePtr->vrId | BIT_14,
                                           moveFromTcamRulePtr,
                                           moveToTcamRulePtr,
                                           trieNodePtr,
                                           *ipTcamFuncsPtr->workDevListPtr);
    if(ret != GT_OK)
    {
        return ret;
    }
    trieNodePtr->tcamAllocInfo.tokenNode->trieNode = trieNodePtr;

    return ret;
}

/*******************************************************************************
* prvCpssDxChLpmTcamMoveToAnywhere
*
* DESCRIPTION:
*       Move single entry from TCAM location and to invalidate the entry in
*       the original location.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* NOT APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       clientCookiePtr   - points to the client cookie associated with the entry
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on error
*
* COMMENTS:
*       None.
*******************************************************************************/
GT_STATUS prvCpssDxChLpmTcamMoveToAnywhere
(
    IN  GT_VOID         *clientCookiePtr
)
{
    PRV_CPSS_DXCH_LPM_TCAM_PAT_TRIE_NODE_STC    *trieNodePtr;
    GT_STATUS                                   rc = GT_OK;
    GT_U32                                      originalToken = 0;

    trieNodePtr = (PRV_CPSS_DXCH_LPM_TCAM_PAT_TRIE_NODE_STC *)clientCookiePtr;

    originalToken = (GT_U32)(trieNodePtr->tcamAllocInfo.tokenNode->tcamManagerEntryToken);

    rc = prvCpssDxChLpmTcamPatTrieRelocatePrefix(trieNodePtr->ipTcamShadowPtr,trieNodePtr);
    if (rc != GT_OK)
        return rc;

    rc = cpssDxChTcamManagerEntryFree(trieNodePtr->ipTcamShadowPtr->tcamManagerHandlerPtr,
                                      trieNodePtr->ipTcamShadowPtr->clientId,
                                      originalToken);
    return rc;
}

/*******************************************************************************
* prvCpssDxChLpmTcamCheckIfDefaultLocation
*
* DESCRIPTION:
*       Return if an entry from TCAM is a default entry
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* NOT APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       clientCookiePtr   - points to the client cookie associated with the entry
*
* OUTPUTS:
*       isDefaultEntryLocationPtr - (pointer to)
*                                GT_TRUE: clientCookiePtr is a default entry
*                                GT_FALSE:clientCookiePtr is not a default entry
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on error
*
* COMMENTS:
*       None.
*******************************************************************************/
GT_STATUS prvCpssDxChLpmTcamCheckIfDefaultLocation
(
    IN  GT_VOID         *clientCookiePtr,
    OUT GT_BOOL         *isDefaultEntryLocationPtr
)
{
    PRV_CPSS_DXCH_LPM_TCAM_PAT_TRIE_NODE_STC    *trieNodePtr;
    GT_STATUS                                   rc = GT_OK;

    trieNodePtr = (PRV_CPSS_DXCH_LPM_TCAM_PAT_TRIE_NODE_STC *)clientCookiePtr;
    if (trieNodePtr->father==NULL)
    {
        *isDefaultEntryLocationPtr=GT_TRUE;
    }
    else
    {
        *isDefaultEntryLocationPtr=GT_FALSE;
    }
    return rc;

}

/*******************************************************************************
* ipTcamTriePclUtilFillIIpUcMaskAndPatternCheetah
*
* DESCRIPTION:
*       Converts the PSS IP UC Rule to pair of mask and pattern
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* NOT APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       pclHwId         - pcl Hw Id
*       ipAddrPtr          - IP Uc address
*       prefixLen       - Ipv6 Uc prefix length
*       isL4Valid       - is Layer 4 valid
*       ipProtocol      - Layer 4 protocol
*       tcpUdpSourcePort        - tcp/udp port source port.
*       tcpUdpSourcePortMaskLen - the length of bits to use from the tcp/udp
*                                 port.
*       isIpv6          - if True then it's a ipv6 prefix, otherwise ipv4
*
* OUTPUTS:
*       ruleFormatPtr - rule format
*       maskPtr     - PCE mask in CPSS format
*       patternPtr  - PCE pattern in CPSS format
*
* RETURNS :
*       GT_OK           - on success.
*       GT_BAD_PARAM    - Illegal parameter in function called
*
* COMMENTS:
*
*
*******************************************************************************/
static GT_STATUS ipTcamTriePclUtilFillIIpUcMaskAndPatternCheetah
(
    IN  GT_U32                             pclHwId,
    IN  GT_U8                              *ipAddrPtr,
    IN  GT_U32                             prefixLen,
    IN  GT_BOOL                            isL4Valid,
    IN  GT_U8                              ipProtocol,
    IN  GT_U16                             tcpUdpSourcePort,
    IN  GT_U32                             tcpUdpSourcePortMaskLen,
    IN  GT_BOOL                            isIpv6,
    OUT CPSS_DXCH_PCL_RULE_FORMAT_TYPE_ENT *ruleFormatPtr,
    OUT CPSS_DXCH_PCL_RULE_FORMAT_UNT      *maskPtr,
    OUT CPSS_DXCH_PCL_RULE_FORMAT_UNT      *patternPtr
)
{
    /* miscelanous pointers into mask and patteern */
    /* possible mask and pattern formats           */
    CPSS_DXCH_PCL_RULE_FORMAT_STD_IPV6_DIP_STC  *maskIpv6StdPtr;
    CPSS_DXCH_PCL_RULE_FORMAT_STD_IPV6_DIP_STC  *patternIpv6StdPtr;
    CPSS_DXCH_PCL_RULE_FORMAT_STD_IPV4_L4_STC   *maskIpv4StdPtr;
    CPSS_DXCH_PCL_RULE_FORMAT_STD_IPV4_L4_STC   *patternIpv4StdPtr;
    /* common substructures */
    CPSS_DXCH_PCL_RULE_FORMAT_COMMON_STC        *maskCommPtr;
    CPSS_DXCH_PCL_RULE_FORMAT_COMMON_STC        *patternCommPtr;
    CPSS_DXCH_PCL_RULE_FORMAT_COMMON_STD_IP_STC *maskCommStdIpPtr;
    CPSS_DXCH_PCL_RULE_FORMAT_COMMON_STD_IP_STC *patternCommStdIpPtr;
    /* IPV6/V4 DIP */
    GT_IPADDR                                   *maskIpv4DipPtr;
    GT_IPADDR                                   *patternIpv4DipPtr;
    GT_IPV6ADDR                                 *maskIpv6DipPtr;
    GT_IPV6ADDR                                 *patternIpv6DipPtr;
    GT_U8                                       *maskDip;
    GT_U8                                       *patternDip;
    /* IP address length in bytes */
    GT_U32                                       ipAddrLen;
    GT_U32                                       i; /* loop index */
    /* TCP source port */
    GT_U8                                       *maskSrcPort0;
    GT_U8                                       *patternSrcPort0;
    GT_U8                                       *maskSrcPort1;
    GT_U8                                       *patternSrcPort1;

    /* clear data */
    cpssOsMemSet(maskPtr, 0, sizeof(CPSS_DXCH_PCL_RULE_FORMAT_UNT));
    cpssOsMemSet(patternPtr, 0, sizeof(CPSS_DXCH_PCL_RULE_FORMAT_UNT));

    /* miscelanous pointers into mask and patteern */
    maskIpv6StdPtr    = &(maskPtr->ruleStdIpv6Dip);
    patternIpv6StdPtr = &(patternPtr->ruleStdIpv6Dip);

    maskIpv4StdPtr    = &(maskPtr->ruleStdIpv4L4);
    patternIpv4StdPtr = &(patternPtr->ruleStdIpv4L4);

    if (isIpv6 == GT_TRUE)
    {
        /* IPV6 */
        *ruleFormatPtr = CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_STD_IPV6_DIP_E;

        maskCommPtr =  &(maskIpv6StdPtr->common);
        patternCommPtr =  &(patternIpv6StdPtr->common);
        maskCommStdIpPtr = &(maskIpv6StdPtr->commonStdIp);
        patternCommStdIpPtr = &(patternIpv6StdPtr->commonStdIp);
    }
    else
    {
        /* IPV4 */
        *ruleFormatPtr = CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_STD_IPV4_L4_E;

        maskCommPtr = &(maskIpv4StdPtr->common);
        patternCommPtr = &(patternIpv4StdPtr->common);
        maskCommStdIpPtr = &(maskIpv4StdPtr->commonStdIp);
        patternCommStdIpPtr = &(patternIpv4StdPtr->commonStdIp);
    }

    /* pclId */
    maskCommPtr->pclId    = (GT_U16)0xFFFF;
    patternCommPtr->pclId = (GT_U16)pclHwId;

    /* isIp */
    maskCommPtr->isIp    = 1;
    patternCommPtr->isIp = 1;

    /* IPV4 / IPV6 */
    maskCommStdIpPtr->isIpv4    = 1;
    patternCommStdIpPtr->isIpv4 = (GT_U8)((isIpv6 == GT_FALSE) ? 1 : 0);

    /* isL4Valid */
    if (isL4Valid == GT_TRUE)
    {
        maskCommStdIpPtr->isL4Valid    = 1;
        patternCommStdIpPtr->isL4Valid = 1;

        /* ipProtocol */
        maskCommStdIpPtr->ipProtocol    = 0xFF;
        patternCommStdIpPtr->ipProtocol = ipProtocol;
    }

    /* IP addresses */
    maskIpv4DipPtr    = &(maskIpv4StdPtr->dip);
    patternIpv4DipPtr = &(patternIpv4StdPtr->dip);
    maskIpv6DipPtr    = &(maskIpv6StdPtr->dip);
    patternIpv6DipPtr = &(patternIpv6StdPtr->dip);

    maskDip = (isIpv6 == GT_FALSE)
              ? &(maskIpv4DipPtr->arIP[0]) : &(maskIpv6DipPtr->arIP[0]);
    patternDip = (isIpv6 == GT_FALSE)
                 ? &(patternIpv4DipPtr->arIP[0]) : &(patternIpv6DipPtr->arIP[0]);

    ipAddrLen = (isIpv6 == GT_FALSE) ? 4 : 16;

    if (prefixLen > (ipAddrLen * 8))
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    for (i = 0; (i < (prefixLen / 8)); i++)
    {
        patternDip[i] = ipAddrPtr[i];
        maskDip[i]    = 0xFF;
    }

    if ((prefixLen % 8) != 0)
    {
        i = prefixLen / 8;
        maskDip[i]    = (GT_U8)(0xFF << (8 - (prefixLen % 8)));
        patternDip[i] = (GT_U8)(maskDip[i] & ipAddrPtr[i]);
    }

    /* TCP source port */
    if (tcpUdpSourcePortMaskLen > 16)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    /* standard DI key does not contain Tcp Source Port */
    if ((tcpUdpSourcePortMaskLen > 0)
        && (isIpv6 != GT_FALSE))
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    if (isIpv6 == GT_TRUE)
    {
        maskSrcPort0 = NULL;
        patternSrcPort0 = NULL;
        maskSrcPort1 = NULL;
        patternSrcPort1 = NULL;
    }
    else
    {
        maskSrcPort0 = &(maskIpv4StdPtr->l4Byte0) ;
        patternSrcPort0 = &(patternIpv4StdPtr->l4Byte0) ;
        maskSrcPort1 = &(maskIpv4StdPtr->l4Byte1) ;
        patternSrcPort1 = &(patternIpv4StdPtr->l4Byte1) ;

        if (tcpUdpSourcePortMaskLen > 0)
        {
            /* mask */
            i = 0xFFFF & (0xFFFF<< (16 - tcpUdpSourcePortMaskLen));
            *maskSrcPort0 = (GT_U8)((i >> 8) & 0xFF);
            *maskSrcPort1 = (GT_U8)(i & 0xFF);
            /* pattern */
            *patternSrcPort0 = (GT_U8)((tcpUdpSourcePort >> 8) & (*maskSrcPort0));
            *patternSrcPort1 = (GT_U8)(tcpUdpSourcePort & (*maskSrcPort1));
        }
    }

    return GT_OK;
}

/*******************************************************************************
* ipTcamTrieSetPrefixIpCheetah
*
* DESCRIPTION:
*       utility function to Add IP UC prefix for cheetha+.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* NOT APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       vrId                - the virtual router id.
*       ipAddrPtr           - IP Uc address
*       prefixLen           - Ip Uc prefix length
*       ecmpRouteNum        - the ecmp route number to write
*       tcamRulePtr         - the tcam rule to set it in
*       ipTcamShadowPtr     - ip shadow we are working on.
*       pData               - the data to attach to this prefix.
*       isIpv6              - whether this prefix is ipv6 or ipv4
*       devListPtr          - pointer to the device list to preform the actions on.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on error
*
* COMMENTS:
*       If we want to add a default IP group, then prefixLen should be 0.
*******************************************************************************/
static GT_STATUS ipTcamTrieSetPrefixIpCheetah
(
    IN  GT_U32                                    vrId,
    IN  GT_U8                                     *ipAddrPtr,
    IN  GT_U32                                    prefixLen,
    IN  GT_U8                                     ecmpRouteNum,
    IN  GT_PTR                                    tcamRulePtr,
    IN  PRV_CPSS_DXCH_LPM_TCAM_SHADOW_STC         *ipTcamShadowPtr,
    IN  GT_PTR                                    pData,
    IN  GT_BOOL                                   isIpv6,
    IN  PRV_CPSS_DXCH_LPM_SHADOW_DEVS_LIST_STC    *devListPtr
)
{
    GT_STATUS ret = GT_OK;
    GT_U8   *ipShareDevs;
    GT_U32  numOfShareDevs;
    CPSS_DXCH_PCL_ACTION_STC *nextHopEntry;
    GT_U32 i;
    CPSS_DXCH_PCL_RULE_FORMAT_UNT ruleData;
    CPSS_DXCH_PCL_RULE_FORMAT_UNT ruleMask;
    CPSS_DXCH_PCL_RULE_FORMAT_TYPE_ENT ruleFormat;
    GT_U32  offset = 0;
    GT_BOOL isToken;

    /*static TCAM_RULE_ECMP_INFO defaultRuleInfo = {0,GT_FALSE,0,0,0};

    static TCAM_RULE_ECMP_INFO ecmpRulesInfo2Entries[5] =
    {{0,GT_FALSE,0,0,0},{1,GT_TRUE,17,1,1},{0,GT_TRUE,17,0,1},{1,GT_TRUE,6,1,1},
     {0,GT_TRUE,6,0,1}};

    static TCAM_RULE_ECMP_INFO ecmpRulesInfo3Entries[9] =
    {{0,GT_FALSE,0,0,0},{2,GT_TRUE,17,3,2},{2,GT_TRUE,17,2,2},{1,GT_TRUE,17,1,2},
     {0,GT_TRUE,17,0,2},{1,GT_TRUE,6,3,2},{2,GT_TRUE,6,2,2},{1,GT_TRUE,6,1,2},
     {0,GT_TRUE,6,0,2}};

    static TCAM_RULE_ECMP_INFO ecmpRulesInfo4Entries[9] =
    {{0,GT_FALSE,0,0,0},{3,GT_TRUE,17,3,2},{2,GT_TRUE,17,2,2},{1,GT_TRUE,17,1,2},
     {0,GT_TRUE,17,0,2},{3,GT_TRUE,6,3,2},{2,GT_TRUE,6,2,2},{1,GT_TRUE,6,1,2},
     {0,GT_TRUE,6,0,2}};*/

    /* fix warning */
    ecmpRouteNum = ecmpRouteNum;
    /* if BIT 14 in the vrId is set then the tcamRulePtr represents TCAM location
       otherwise it represents token */
    isToken = (vrId & BIT_14) ? GT_FALSE : GT_TRUE;
    vrId &= (~BIT_14);
    ret = prvCpssDxChLpmTcamGetTcamOffset(ipTcamShadowPtr,tcamRulePtr,isToken,&offset);
    if (ret != GT_OK)
    {
        return ret;
    }

    /* continue only if there are devices in the share devices list */
    if (devListPtr->ipShareDevNum == 0)
        return GT_OK;

    ipShareDevs     = devListPtr->ipShareDevs;
    numOfShareDevs  = devListPtr->ipShareDevNum;

    nextHopEntry = (CPSS_DXCH_PCL_ACTION_STC *)pData;

    ret = ipTcamTriePclUtilFillIIpUcMaskAndPatternCheetah(vrId,ipAddrPtr,prefixLen,
                                                          GT_FALSE,0,0,0,isIpv6,
                                                          &ruleFormat,&ruleMask,
                                                          &ruleData);
    if (ret != GT_OK)
    {
        return ret;
    }
    /* go over all the share device and write to the device */
    for (i = 0; i < numOfShareDevs; i++)
    {

        ret = cpssDxChPclRuleSet(
            ipShareDevs[i],ruleFormat,offset, 0 /*ruleOptionsBmp*/,
                                 &ruleMask,&ruleData,nextHopEntry);
        if (ret != GT_OK)
        {
            return ret;
        }

    }

    return ret;
}

/*******************************************************************************
* ipTcamTrieSetPrefixIpv4Cheetah
*
* DESCRIPTION:
*       utility function to Add IP UC prefix.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* NOT APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       vrId                - the virtual router id.
*       ipAddrPtr           - IP Uc address
*       prefixLen           - Ip Uc prefix length
*       ecmpRouteNum        - the ecmp route number to write
*       tcamRulePtr         - the tcam rule to set it in
*       ipTcamShadowPtr     - ip shadow we are working on.
*       pData               - the data to attach to this prefix.
*       devListPtr          - pointer to the device list to preform the actions on.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on error
*
* COMMENTS:
*       If we want to add a default IP group, then prefixLen should be 0.
*******************************************************************************/
static GT_STATUS ipTcamTrieSetPrefixIpv4Cheetah
(
    IN  GT_U32                                  vrId,
    IN  GT_U8                                   *ipAddrPtr,
    IN  GT_U32                                  prefixLen,
    IN  GT_U8                                   ecmpRouteNum,
    IN  GT_PTR                                  tcamRulePtr,
    IN  PRV_CPSS_DXCH_LPM_TCAM_SHADOW_STC       *ipTcamShadowPtr,
    IN  GT_PTR                                  pData,
    IN  PRV_CPSS_DXCH_LPM_SHADOW_DEVS_LIST_STC  *devListPtr
)
{
    return ipTcamTrieSetPrefixIpCheetah(vrId,
                                        ipAddrPtr,
                                        prefixLen,
                                        ecmpRouteNum,
                                        tcamRulePtr,
                                        ipTcamShadowPtr,
                                        pData,
                                        GT_FALSE,
                                        devListPtr);
}

/*******************************************************************************
* ipTcamTrieSetPrefixIpv6Cheetah
*
* DESCRIPTION:
*       utility function to Add IP UC prefix.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* NOT APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       vrId            - the virtual router id.
*       ipAddrPtr       - IP Uc address
*       prefixLen       - Ip Uc prefix length
*       ecmpRouteNum    - the ecmp route number to write
*       tcamRulePtr     - the tcam rule to set it in
*       ipTcamShadowPtr - ip shadow we are working on.
*       pData           - the data to attach to this prefix.
*       devListPtr      - pointer to the device list to preform the actions on.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on error
*
* COMMENTS:
*       If we want to add a default IP group, then prefixLen should be 0.
*******************************************************************************/
static GT_STATUS ipTcamTrieSetPrefixIpv6Cheetah
(
    IN  GT_U32                                 vrId,
    IN  GT_U8                                  *ipAddrPtr,
    IN  GT_U32                                 prefixLen,
    IN  GT_U8                                  ecmpRouteNum,
    IN  GT_PTR                                 tcamRulePtr,
    IN  PRV_CPSS_DXCH_LPM_TCAM_SHADOW_STC      *ipTcamShadowPtr,
    IN  GT_PTR                                 pData,
    IN  PRV_CPSS_DXCH_LPM_SHADOW_DEVS_LIST_STC *devListPtr
)
{
    return ipTcamTrieSetPrefixIpCheetah(vrId,
                                        ipAddrPtr,
                                        prefixLen,
                                        ecmpRouteNum,
                                        tcamRulePtr,
                                        ipTcamShadowPtr,
                                        pData,
                                        GT_TRUE,
                                        devListPtr);
}

/*******************************************************************************
* ipTcamTrieDelPrefixCheetah
*
* DESCRIPTION:
*       utility function to Delete IP prefix PCE from IP table.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* NOT APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       vrId                - the virtual router id.
*       tcamRulePtr         - the tcam rule to delete it from.
*       ipTcamShadowPtr     - ip shadow we are working on.
*       pData               - the data entry that was attached to this prefix.
*       devListPtr          - pointer to the device list to preform the actions on.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on error
*
* COMMENTS:
*       None.
*******************************************************************************/
static GT_STATUS ipTcamTrieDelPrefixCheetah
(
    IN  GT_U32                                     vrId,
    IN  GT_PTR                                     tcamRulePtr,
    IN PRV_CPSS_DXCH_LPM_TCAM_SHADOW_STC           *ipTcamShadowPtr,
    IN  GT_PTR                                     pData,
    IN  PRV_CPSS_DXCH_LPM_SHADOW_DEVS_LIST_STC     *devListPtr
)
{
    GT_STATUS ret = GT_OK;
    GT_U8   *ipShareDevs;
    GT_U32 numOfShareDevs;
    GT_U32  i;
    GT_U32  offset = 0;
    GT_BOOL isToken;

    /* fix warning */
    vrId = vrId;
    pData = pData;

    /* continue only if there are devices in the share devices list */
    if (devListPtr->ipShareDevNum == 0)
        return GT_OK;

    ipShareDevs     = devListPtr->ipShareDevs;
    numOfShareDevs  = devListPtr->ipShareDevNum;
    /* if BIT 14 in the vrId is set then the tcamRulePtr represents TCAM location
       otherwise it represents token */
    isToken = (vrId & BIT_14) ? GT_FALSE : GT_TRUE;
    vrId &= (~BIT_14);
    ret = prvCpssDxChLpmTcamGetTcamOffset(ipTcamShadowPtr,tcamRulePtr,isToken,&offset);
    if (ret != GT_OK)
    {
        return ret;
    }

    /* go over all the share device and write to the device */
    for (i = 0; i < numOfShareDevs; i++)
    {

        ret = cpssDxChPclRuleInvalidate(ipShareDevs[i],CPSS_PCL_RULE_SIZE_STD_E,
                                        offset);
        if (ret != GT_OK)
        {
            return ret;
        }

    }
    return ret;
}

/*******************************************************************************
* ipTcamTrieMovePrefixCheetah
*
* DESCRIPTION:
*       utility function to Update move PCE entry action in IP pCL table.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* NOT APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       vrId                - the virtual router id.
*       moveFromTcamRulePtr - the tcam rule to move from.
*       moveToTcamRulePtr   - the tcam rule to move to .
*       trieNodePtr         - trie node associated with moved rule.
*       protocolStack       - the protocol stack of the prefix
*       devListPtr          - pointer to the device list to preform the actions on.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on error
*
* COMMENTS:
*       None.
*******************************************************************************/
static GT_STATUS ipTcamTrieMovePrefixCheetah
(
    IN  GT_U32                                    vrId,
    IN  GT_PTR                                    moveFromTcamRulePtr,
    IN  GT_PTR                                    moveToTcamRulePtr,
    IN  PRV_CPSS_DXCH_LPM_TCAM_PAT_TRIE_NODE_STC  *trieNodePtr,
    IN  CPSS_IP_PROTOCOL_STACK_ENT                protocolStack,
    IN  PRV_CPSS_DXCH_LPM_SHADOW_DEVS_LIST_STC    *devListPtr
)
{
    GT_STATUS ret = GT_OK;
    GT_U8 ipAddr[16];
    GT_U32 prefixLen;
    GT_BOOL isNotIpV4;

    /* continue only if there are devices in the share devices list */
    if (devListPtr->ipShareDevNum == 0)
        return GT_OK;

    /* get ip address from the rule's node */
    ret = prvCpssDxChLpmTcamPatTrieGetAddrFromNode(trieNodePtr,ipAddr,&prefixLen);
    if (ret != GT_OK)
    {
        return ret;
    }

    /* first write the new rule */
    isNotIpV4 = (protocolStack==CPSS_IP_PROTOCOL_IPV4_E)?GT_FALSE:GT_TRUE;
    ret = ipTcamTrieSetPrefixIpCheetah(vrId,
                                       ipAddr,
                                       prefixLen,
                                       0,
                                       moveToTcamRulePtr,
                                       trieNodePtr->ipTcamShadowPtr,
                                       trieNodePtr->pData,
                                       isNotIpV4,
                                       devListPtr);
    if (ret != GT_OK)
    {
        return ret;
    }

    /* now delete the old rule */
    ret = ipTcamTrieDelPrefixCheetah(vrId,
                                     moveFromTcamRulePtr,
                                     trieNodePtr->ipTcamShadowPtr,
                                     trieNodePtr->pData,
                                     devListPtr);
    if (ret != GT_OK)
    {
        return ret;
    }


    return ret;
}


/*******************************************************************************
* ipTcamTrieMoveIpv4PrefixCheetah
*
* DESCRIPTION:
*       utility function to Update move PCE entry action in IP pCL table.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* NOT APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       vrId                - the virtual router id.
*       moveFromTcamRulePtr - the tcam rule to move from.
*       moveToTcamRulePtr   - the tcam rule to move to .
*       trieNodePtr         - trie node associated with moved rule.
*       devListPtr          - pointer to the device list to preform the actions on.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on error
*
* COMMENTS:
*       None.
*******************************************************************************/
static GT_STATUS ipTcamTrieMoveIpv4PrefixCheetah
(
    IN  GT_U32                                        vrId,
    IN  GT_PTR                                        moveFromTcamRulePtr,
    IN  GT_PTR                                        moveToTcamRulePtr,
    IN  PRV_CPSS_DXCH_LPM_TCAM_PAT_TRIE_NODE_STC      *trieNodePtr,
    IN  PRV_CPSS_DXCH_LPM_SHADOW_DEVS_LIST_STC        *devListPtr
)
{
    return ipTcamTrieMovePrefixCheetah(vrId,
                                       moveFromTcamRulePtr,
                                       moveToTcamRulePtr,
                                       trieNodePtr,
                                       CPSS_IP_PROTOCOL_IPV4_E,
                                       devListPtr);
}


/*******************************************************************************
* ipTcamTrieMoveIpv6PrefixCheetah
*
* DESCRIPTION:
*       utility function to Update move PCE entry action in IP pCL table.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* NOT APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       vrId                - the virtual router id.
*       moveFromTcamRulePtr - the tcam rule to move from.
*       moveToTcamRulePtr   - the tcam rule to move to .
*       trieNodePtr         - trie node associated with moved rule
*       devListPtr          - pointer to the device list to preform the actions on.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on error
*
* COMMENTS:
*       None.
*******************************************************************************/
static GT_STATUS ipTcamTrieMoveIpv6PrefixCheetah
(
    IN  GT_U32                                        vrId,
    IN  GT_PTR                                        moveFromTcamRulePtr,
    IN  GT_PTR                                        moveToTcamRulePtr,
    IN  PRV_CPSS_DXCH_LPM_TCAM_PAT_TRIE_NODE_STC      *trieNodePtr,
    IN  PRV_CPSS_DXCH_LPM_SHADOW_DEVS_LIST_STC        *devListPtr
)
{
    return ipTcamTrieMovePrefixCheetah(vrId,
                                       moveFromTcamRulePtr,
                                       moveToTcamRulePtr,
                                       trieNodePtr,
                                       CPSS_IP_PROTOCOL_IPV6_E,
                                       devListPtr);
}

/*******************************************************************************
* ipTcamTrieUpdateDataCheeta
*
* DESCRIPTION:
*       updates the offset/pceNum (idx) in the data entry given.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* NOT APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       oldDataEntry        - the data entry currently in.
*       dataEntry           - the data entry to set.
*       tcamRulePtr         - the tcam rule needs updating
*       isToken             - GT_TRUE  - tcamRulePtr means token (tcam manager mode).
*                           - GT_FALSE - tcamRulePtr means tcam location (tcam manager mode).
*       ipTcamShadowPtr     - ip shadow we are working on.
*       devListPtr          - pointer to the device list to preform the actions on.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on error
*
* COMMENTS:
*       None.
*******************************************************************************/
static GT_STATUS ipTcamTrieUpdateDataCheeta
(
    IN  GT_PTR                                        oldDataEntry,
    IN  GT_PTR                                        dataEntry,
    IN  GT_PTR                                        tcamRulePtr,
    IN  GT_BOOL                                       isToken,
    IN PRV_CPSS_DXCH_LPM_TCAM_SHADOW_STC              *ipTcamShadowPtr,
    IN  PRV_CPSS_DXCH_LPM_SHADOW_DEVS_LIST_STC        *devListPtr
)
{
    GT_STATUS ret = GT_OK;
    GT_U8   *ipShareDevs;
    GT_U32 numOfShareDevs;
    CPSS_DXCH_PCL_ACTION_STC *nextHopEntry;
    GT_U32  i;
    GT_U32  offset = 0;

    /* fix warning */
    oldDataEntry = oldDataEntry;

    /* continue only if there are devices in the share devices list */
    if (devListPtr->ipShareDevNum == 0)
        return GT_OK;

    ipShareDevs     = devListPtr->ipShareDevs;
    numOfShareDevs  = devListPtr->ipShareDevNum;

    nextHopEntry = (CPSS_DXCH_PCL_ACTION_STC *)dataEntry;
    ret = prvCpssDxChLpmTcamGetTcamOffset(ipTcamShadowPtr,tcamRulePtr,isToken,&offset);
    if (ret != GT_OK)
    {
        return ret;
    }

    /* go over all the share device and write to the device */
    for (i = 0; i < numOfShareDevs; i++)
    {
        ret = cpssDxChPclRuleActionUpdate(ipShareDevs[i],
                                          CPSS_PCL_RULE_SIZE_STD_E,
                                          offset,nextHopEntry);

        if (ret != GT_OK)
        {
            return ret;
        }
    }
    return ret;
}



/*******************************************************************************
* ipTcamTrieUpdateDataCheeta2
*
* DESCRIPTION:
*       updates the offset/pceNum (idx) in the data entry given.
*
* APPLICABLE DEVICES:
*        DxCh2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       oldDataEntry        - the data entry currently in.
*       dataEntry           - the data entry to set.
*       tcamRulePtr         - the tcam rule needs updating
*       isToken             - GT_TRUE  - tcamRulePtr means token (tcam manager mode).
*                           - GT_FALSE - tcamRulePtr means tcam location (tcam manager mode).
*       ipTcamShadowPtr     - ip shadow we are working on.
*       devListPtr          - pointer to the device list to preform the actions on.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on error
*
* COMMENTS:
*       None.
*******************************************************************************/
static GT_STATUS ipTcamTrieUpdateDataCheeta2
(
    IN  GT_PTR                                  oldDataEntry,
    IN  GT_PTR                                  dataEntry,
    IN  GT_PTR                                  tcamRulePtr,
    IN  GT_BOOL                                 isToken,
    IN PRV_CPSS_DXCH_LPM_TCAM_SHADOW_STC        *ipTcamShadowPtr,
    IN  PRV_CPSS_DXCH_LPM_SHADOW_DEVS_LIST_STC  *devListPtr
)
{
    GT_STATUS ret = GT_OK;
    GT_U8   *ipShareDevs;
    GT_U32 numOfShareDevs;
    PRV_CPSS_DXCH_LPM_ROUTE_ENTRY_POINTER_STC *lttEntryPtr;
    GT_U32 i;
    GT_U32  tcamRow =0;
    GT_U32  tcamColumn = 0;
    CPSS_DXCH_TCAM_MANAGER_TCAM_LOCATION_STC    *tcamLocation = NULL ;
    /* fix warning */
    oldDataEntry = oldDataEntry;

    /* continue only if there are devices in the share devices list */
    if (devListPtr->ipShareDevNum == 0)
        return GT_OK;

    ipShareDevs     = devListPtr->ipShareDevs;
    numOfShareDevs  = devListPtr->ipShareDevNum;

    if (isToken == GT_TRUE)
    {
        ret =  prvCpssDxChLpmTcamGetTcamRowColumn(ipTcamShadowPtr,
                                                  tcamRulePtr,
                                                  &tcamRow,
                                                  &tcamColumn);
        if (ret != GT_OK)
        {
            return ret;
        }
    }
    else
    {
        /* tcamRulePtr is alredy contains tcam location:*/
        tcamLocation = (CPSS_DXCH_TCAM_MANAGER_TCAM_LOCATION_STC*)tcamRulePtr;
        tcamRow = tcamLocation->row;
        tcamColumn = tcamLocation->column;
    }

    lttEntryPtr = (PRV_CPSS_DXCH_LPM_ROUTE_ENTRY_POINTER_STC *)dataEntry;

    /* go over all the share device and write to the device */
    for (i = 0; i < numOfShareDevs; i++)
    {

        /* set the lookup translation table entry */
        ret = prvCpssDxChLpmTcamLttWrite(ipShareDevs[i],
                                         tcamRow,
                                         tcamColumn,
                                         lttEntryPtr);
        if (ret != GT_OK)
        {
            return ret;
        }

    }
    return ret;
}


/*******************************************************************************
* ipTcamTrieSetPrefixIpCheetah2PclBasedVr
*
* DESCRIPTION:
*       utility function to Add IP UC prefix for cheetha2 multiple VRs.
*
* APPLICABLE DEVICES:
*        DxCh2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       vrId        - the virtual router id.
*       ipAddrPtr   - IP Uc address
*       prefixLen   - Ip Uc prefix length
*       ecmpRouteNum- the ecmp route number to write
*       tcamRulePtr - the tcam rule to set it in
*       pData       - the data to attach to this prefix.
*       isIpv6      - whether this prefix is ipv6 or ipv4
*       devListPtr  - pointer to the device list to preform the actions on.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on error
*
* COMMENTS:
*       If we want to add a default IP group, then prefixLen should be 0.
*******************************************************************************/
static GT_STATUS ipTcamTrieSetPrefixIpCheetah2PclBasedVr
(
    IN  GT_U32                                  vrId,
    IN  GT_U8                                   *ipAddrPtr,
    IN  GT_U32                                  prefixLen,
    IN  GT_U8                                   ecmpRouteNum,
    IN  PRV_CPSS_DXCH_LPM_TCAM_RULE_NODE_STC    *tcamRulePtr,
    IN  GT_PTR                                  pData,
    IN  GT_BOOL                                 isIpv6,
    IN  PRV_CPSS_DXCH_LPM_SHADOW_DEVS_LIST_STC  *devListPtr
)
{
    GT_STATUS                            ret = GT_OK;
    GT_U8                                *ipShareDevs;
    GT_U32                               numOfShareDevs;
    GT_U32                               i;
    CPSS_DXCH_PCL_RULE_FORMAT_UNT        ruleData;
    CPSS_DXCH_PCL_RULE_FORMAT_UNT        ruleMask;
    CPSS_DXCH_PCL_RULE_FORMAT_TYPE_ENT   ruleFormat;
    CPSS_DXCH_PCL_ACTION_STC             pclAction;
    PRV_CPSS_DXCH_LPM_TCAM_RULE_NODE_STC pclTcamRule;
    /* fix warning */
    ecmpRouteNum = ecmpRouteNum;

    /* continue only if there are devices in the share devices list */
    if (devListPtr->ipShareDevNum == 0)
        return GT_OK;

    ipShareDevs     = devListPtr->ipShareDevs;
    numOfShareDevs  = devListPtr->ipShareDevNum;

    /* set default parameters */
    cpssOsMemSet(&pclAction, 0, sizeof(pclAction));

    pclAction.pktCmd                        = CPSS_PACKET_CMD_FORWARD_E;
    pclAction.mirror.mirrorToRxAnalyzerPort = GT_FALSE;
    pclAction.matchCounter.matchCounterIndex = 5;
    pclAction.matchCounter.enableMatchCount = GT_TRUE;
    pclAction.policer.policerEnable         = GT_FALSE;
    pclAction.ipUcRoute.doIpUcRoute         = GT_FALSE;
    pclAction.egressPolicy                  = GT_FALSE;
    pclAction.qos.ingress.modifyDscp        = CPSS_PACKET_ATTRIBUTE_MODIFY_KEEP_PREVIOUS_E;
    pclAction.qos.ingress.modifyUp          = CPSS_PACKET_ATTRIBUTE_MODIFY_KEEP_PREVIOUS_E;
    pclAction.qos.ingress.profileAssignIndex = GT_FALSE;
    pclAction.vlan.ingress.modifyVlan       = CPSS_PACKET_ATTRIBUTE_ASSIGN_DISABLED_E;
    /* these are only relevant parameters for this case */
    pclAction.redirect.data.routerLttIndex  = tcamRulePtr->redirectOffset;
    pclAction.redirect.redirectCmd          = CPSS_DXCH_PCL_ACTION_REDIRECT_CMD_ROUTER_E;

    /* first write the Lookup translation entry (using the function that does
       only that */
    pclTcamRule = *tcamRulePtr;
    pclTcamRule.offset = pclTcamRule.redirectOffset;
    ret = ipTcamTrieUpdateDataCheeta2(NULL,
                                      pData,
                                      (GT_PTR)(&pclTcamRule),
                                      GT_TRUE,
                                      tcamRulePtr->trieNode->ipTcamShadowPtr,
                                      devListPtr);
    if (ret != GT_OK)
    {
        return ret;
    }

    ret = ipTcamTriePclUtilFillIIpUcMaskAndPatternCheetah(vrId,ipAddrPtr,prefixLen,
                                                          GT_FALSE,0,0,0,isIpv6,
                                                          &ruleFormat,&ruleMask,
                                                          &ruleData);
    if (ret != GT_OK)
    {
        return ret;
    }
    /* go over all the share device and write to the device */
    for (i = 0; i < numOfShareDevs; i++)
    {

        ret = cpssDxChPclRuleSet(
            ipShareDevs[i], ruleFormat, tcamRulePtr->offset, 0 /*ruleOptionsBmp*/,
                                 &ruleMask, &ruleData, &pclAction);
        if (ret != GT_OK)
        {
            return ret;
        }

    }

    return ret;
}

/*******************************************************************************
* prvCpssDxChLpmTcamTrieSetPrefixIpCheetah2
*
* DESCRIPTION:
*       utility function to Add IP UC prefix.
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       vrId            - The virtual's router ID.
*       ipAddrPtr       - IP Uc address
*       prefixLen       - Ip Uc prefix length
*       ecmpRouteNum    - the ecmp route number to write
*       isMcSrc         - if this entry is MC source
*       gIndex          - the G-index to write (relevant if isMcSrc == GT_TRUE)
*       tcamRulePtr     - the tcam rule to set it in
*       ipTcamShadowPtr - ip shadow we are working on.
*       pData           - the data to attach to this prefix.
*       protocolStack   - the type of the prefix.
*       devListPtr      - pointer to the device list to preform the actions on.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       1.  In order to add a default IP group, use prefixLen = 0.
*       2.  Bit 15 in the vrId indicates whether this is shared virtual router
*           or not (if the bit is 1 this is shared virtual router).
*           For shared virtual router:
*             - <match all> entries (meaning perfix len is 0) are not written to
*               the hardware
*             - all other prefixes are written with <don't care> masking for
*               the virtual router field.
*******************************************************************************/
GT_STATUS prvCpssDxChLpmTcamTrieSetPrefixIpCheetah2
(
    IN  GT_U32                                    vrId,
    IN  GT_U8                                     *ipAddrPtr,
    IN  GT_U32                                    prefixLen,
    IN  GT_U8                                     ecmpRouteNum,
    IN  GT_BOOL                                   isMcSrc,
    IN  GT_U16                                    gIndex,
    IN  GT_PTR                                    tcamRulePtr,
    IN PRV_CPSS_DXCH_LPM_TCAM_SHADOW_STC          *ipTcamShadowPtr,
    IN  GT_PTR                                    pData,
    IN  CPSS_IP_PROTOCOL_STACK_ENT                protocolStack,
    IN  PRV_CPSS_DXCH_LPM_SHADOW_DEVS_LIST_STC    *devListPtr
)
{
    GT_STATUS   ret = GT_OK;
    GT_U8       *ipShareDevs;
    GT_U32      numOfShareDevs;
    GT_U32      i,j;
    GT_U8       *arIpAddrPtr;
    GT_U8       *arIpMaskPtr;
    GT_IPADDR   ipv4Addr,ipv4Mask = {0};
    GT_IPV6ADDR ipv6Addr,ipv6Mask;
    GT_U32      maxPrefixLenInBytes;
    CPSS_DXCH_IPV4_PREFIX_STC ipv4Prefix;
    CPSS_DXCH_IPV4_PREFIX_STC ipv4PrefixMask;
    CPSS_DXCH_IPV6_PREFIX_STC ipv6Prefix;
    CPSS_DXCH_IPV6_PREFIX_STC ipv6PrefixMask;
    GT_U32  tcamRow = 0;
    GT_U32  tcamColumn = 0;
    GT_U32      tcamLines = tcamRows;
    GT_BOOL     isSharedVr;
    GT_U32      mcGroupRow = 0;
    GT_U32      mcGroupColumn = 0;
    GT_BOOL isToken;
    CPSS_DXCH_TCAM_MANAGER_TCAM_LOCATION_STC    *tcamLocation = NULL ;
    /* continue only if there are devices in the share devices list */
    if (devListPtr->ipShareDevNum == 0)
        return GT_OK;

    isToken = (vrId & BIT_14) ? GT_FALSE : GT_TRUE;
    vrId &= (~BIT_14);
    if (isToken == GT_TRUE)
    {
        ret =  prvCpssDxChLpmTcamGetTcamRowColumn(ipTcamShadowPtr,
                                                  tcamRulePtr,
                                                  &tcamRow,
                                                  &tcamColumn);
        if (ret != GT_OK)
        {
            return ret;
        }
    }
    else
    {
        /* tcamRulePtr is alredy contains tcam location:*/
        tcamLocation = (CPSS_DXCH_TCAM_MANAGER_TCAM_LOCATION_STC*)tcamRulePtr;
        tcamRow = tcamLocation->row;
        tcamColumn = tcamLocation->column;
    }

    if (ipTcamShadowPtr->tcamManagementMode == PRV_CPSS_DXCH_LPM_TCAM_MANAGEMENT_TCAM_ENTRIES_HANDLER_MODE_E)
    {
        /* devices working with TCAM entries handler are column based TCAM */
        mcGroupRow    = gIndex % tcamLines;
        mcGroupColumn = gIndex / tcamLines;
    }
    else
    {
        /* devices working with TCAM manager are row based TCAM */
        mcGroupRow    = gIndex / 4;
        mcGroupColumn = gIndex % 4;
    }

    /* continue only if there are devices in the share devices list */
    if (devListPtr->ipShareDevNum == 0)
        return GT_OK;

    ipShareDevs     = devListPtr->ipShareDevs;
    numOfShareDevs  = devListPtr->ipShareDevNum;

    /* bit 15 in vrId indicates shared virtual router */
    isSharedVr = ((vrId & BIT_15) ? GT_TRUE : GT_FALSE);

    /* for Cheetah2, for pcl based routing */
    if ((PRV_CPSS_PP_MAC(ipShareDevs[0])->devFamily == CPSS_PP_FAMILY_CHEETAH2_E) && (vrId > 0))
    {
        return ipTcamTrieSetPrefixIpCheetah2PclBasedVr
            (vrId,
             ipAddrPtr,
             prefixLen,
             ecmpRouteNum,
             (PRV_CPSS_DXCH_LPM_TCAM_RULE_NODE_STC*)tcamRulePtr,
             pData,
             (protocolStack == CPSS_IP_PROTOCOL_IPV4_E) ? GT_FALSE : GT_TRUE,
             devListPtr);
    }

    /* for shared virtual routers, <all match> entries are not written to hardware */
    if ((isSharedVr == GT_TRUE) && (prefixLen == 0))
        return GT_OK;

    /* first write the Lookup translation entry (using the function that does
       only that */
    ret = ipTcamTrieUpdateDataCheeta2(NULL,
                                      pData,
                                      tcamRulePtr,
                                      isToken,
                                      ipTcamShadowPtr,
                                      devListPtr);
    if (ret != GT_OK)
    {
        return ret;
    }

    cpssOsMemSet(&ipv4Addr, 0x00, sizeof(ipv4Addr));
    cpssOsMemSet(&ipv6Addr, 0x00, sizeof(ipv6Addr));

    arIpAddrPtr = (protocolStack == CPSS_IP_PROTOCOL_IPV4_E)?
        ipv4Addr.arIP : ipv6Addr.arIP;

    arIpMaskPtr = (protocolStack == CPSS_IP_PROTOCOL_IPV4_E)?
        ipv4Mask.arIP : ipv6Mask.arIP;

    maxPrefixLenInBytes = (protocolStack == CPSS_IP_PROTOCOL_IPV4_E)? 4:16;

    /* copy out the relevant part of the ip address */
    cpssOsMemCpy(arIpAddrPtr,ipAddrPtr,((prefixLen+7) /8) );

    /* zero out the mask */
    cpssOsMemSet(arIpMaskPtr,0,maxPrefixLenInBytes*sizeof(GT_U8));
    /* set the mask */
    cpssOsMemSet(arIpMaskPtr,0xFF,(prefixLen/8));
    if ((prefixLen % 8) != 0)
    {
        arIpMaskPtr[(prefixLen /8)] = (GT_U8)((0xFF << (8 - (prefixLen %8))) & 0xFF);
    }

    /* then write the TCAM rule for the prefix */
    for (i = 0; i < numOfShareDevs; i++)
    {
        if (protocolStack == CPSS_IP_PROTOCOL_IPV4_E)
        {
            ipv4Prefix.vrId = ((isSharedVr == GT_TRUE) ? 0 : vrId);
            ipv4Prefix.ipAddr.u32Ip = ipv4Addr.u32Ip;
            ipv4Prefix.isMcSource = isMcSrc;
            ipv4Prefix.mcGroupIndexRow = mcGroupRow;
            ipv4Prefix.mcGroupIndexColumn = mcGroupColumn;

            ipv4PrefixMask.vrId = ((isSharedVr == GT_TRUE) ? 0 : 0xfff);
            ipv4PrefixMask.ipAddr.u32Ip = ipv4Mask.u32Ip;
            /* you shouldn't mask isMcSource */
            ipv4PrefixMask.isMcSource = GT_TRUE;
            ipv4PrefixMask.mcGroupIndexRow = 0x1fff;
            if (PRV_CPSS_PP_MAC(ipShareDevs[0])->devFamily == CPSS_PP_FAMILY_CHEETAH2_E)
                ipv4PrefixMask.mcGroupIndexColumn = 0x7;
            else
                ipv4PrefixMask.mcGroupIndexColumn = 0x3;
            ret = cpssDxChIpv4PrefixSet(ipShareDevs[i],
                                        tcamRow,
                                        tcamColumn,
                                        &ipv4Prefix,&ipv4PrefixMask);
        }
        else
        {
            ipv6Prefix.vrId = ((isSharedVr == GT_TRUE) ? 0 : vrId);
            for (j = 0 ; j < 4 ; j++)
                ipv6Prefix.ipAddr.u32Ip[j] = ipv6Addr.u32Ip[j];
            ipv6Prefix.isMcSource = isMcSrc;
            ipv6Prefix.mcGroupIndexRow = mcGroupRow;

            ipv6PrefixMask.vrId = ((isSharedVr == GT_TRUE) ? 0 : 0xfff);
            for (j = 0 ; j < 4 ; j++)
                ipv6PrefixMask.ipAddr.u32Ip[j] = ipv6Mask.u32Ip[j];
            /* you shouldn't mask isMcSource */
            ipv6PrefixMask.isMcSource = GT_TRUE;
            ipv6PrefixMask.mcGroupIndexRow = 0xfff;

            ret = cpssDxChIpv6PrefixSet(ipShareDevs[i],tcamRow,
                                        &ipv6Prefix,&ipv6PrefixMask);
        }

        if (ret != GT_OK)
        {
            return ret;
        }
    }

    return ret;
}

/*******************************************************************************
* ipTcamTrieSetPrefixIpv4Cheetah2
*
* DESCRIPTION:
*       utility function to Add IP UC prefix.
*
* APPLICABLE DEVICES:
*        DxCh2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       vrId               - the virtual router id.
*       ipAddrPtr          - IP Uc address
*       prefixLen          - Ip Uc prefix length
*       ecmpRouteNum       - the ecmp route number to write
*       tcamRulePtr        - the tcam rule to set it in
*       ipTcamShadowPtr    - ip shadow we are working on.
*       pData              - the data to attach to this prefix.
*       devListPtr         - pointer to the device list to preform the actions on.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on error
*
* COMMENTS:
*       If we want to add a default IP group, then prefixLen should be 0.
*******************************************************************************/
static GT_STATUS ipTcamTrieSetPrefixIpv4Cheetah2
(
    IN  GT_U32                                    vrId,
    IN  GT_U8                                     *ipAddrPtr,
    IN  GT_U32                                    prefixLen,
    IN  GT_U8                                     ecmpRouteNum,
    IN  GT_PTR                                    tcamRulePtr,
    IN PRV_CPSS_DXCH_LPM_TCAM_SHADOW_STC          *ipTcamShadowPtr,
    IN  GT_PTR                                    pData,
    IN  PRV_CPSS_DXCH_LPM_SHADOW_DEVS_LIST_STC    *devListPtr
)
{
    return prvCpssDxChLpmTcamTrieSetPrefixIpCheetah2(vrId,
                                                     ipAddrPtr,
                                                     prefixLen,
                                                     ecmpRouteNum,
                                                     GT_FALSE,
                                                     0,
                                                     tcamRulePtr,
                                                     ipTcamShadowPtr,
                                                     pData,
                                                     CPSS_IP_PROTOCOL_IPV4_E,
                                                     devListPtr);
}

/*******************************************************************************
* ipTcamTrieSetPrefixIpv6Cheetah2
*
* DESCRIPTION:
*       utility function to Add IP UC prefix.
*
* APPLICABLE DEVICES:
*        DxCh2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       vrId               - the virtual router id.
*       ipAddrPtr          - IP Uc address.
*       prefixLen          - Ip Uc prefix length.
*       ecmpRouteNum       - the ecmp route number to write.
*       tcamRulePtr        - the tcam rule to set it in.
*       ipTcamShadowPtr    - ip shadow we are working on.
*       pData              - the data to attach to this prefix.
*       devListPtr         - pointer to the device list to preform the actions on.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on error
*
* COMMENTS:
*       If we want to add a default IP group, then prefixLen should be 0.
*******************************************************************************/
static GT_STATUS ipTcamTrieSetPrefixIpv6Cheetah2
(
    IN  GT_U32                                    vrId,
    IN  GT_U8                                     *ipAddrPtr,
    IN  GT_U32                                    prefixLen,
    IN  GT_U8                                     ecmpRouteNum,
    IN  GT_PTR                                    tcamRulePtr,
    IN  PRV_CPSS_DXCH_LPM_TCAM_SHADOW_STC         *ipTcamShadowPtr,
    IN  GT_PTR                                    pData,
    IN  PRV_CPSS_DXCH_LPM_SHADOW_DEVS_LIST_STC    *devListPtr
)
{
    return prvCpssDxChLpmTcamTrieSetPrefixIpCheetah2(vrId,
                                                     ipAddrPtr,
                                                     prefixLen,
                                                     ecmpRouteNum,
                                                     GT_FALSE,
                                                     0,
                                                     tcamRulePtr,
                                                     ipTcamShadowPtr,
                                                     pData,
                                                     CPSS_IP_PROTOCOL_IPV6_E,
                                                     devListPtr);
}

/*******************************************************************************
* prvCpssDxChLpmTcamTrieDelPrefixCheetah2
*
* DESCRIPTION:
*       utility function to Delete IP prefix rule from IP table.
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       vrId                - the virtual router id.
*       tcamRulePtr         - the tcam rule to delete it from.
*       ipTcamShadowPtr     - ip shadow we are working on.
*       pData               - the data entry that was attached to this prefix.
*       protocolStack       - the protocol stack of the prefix.
*       devListPtr          - pointer to the device list to preform the actions on.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*******************************************************************************/
GT_STATUS prvCpssDxChLpmTcamTrieDelPrefixCheetah2
(
    IN  GT_U32                                        vrId,
    IN  GT_PTR                                        tcamRulePtr,
    IN  PRV_CPSS_DXCH_LPM_TCAM_SHADOW_STC             *ipTcamShadowPtr,
    IN  GT_PTR                                        pData,
    IN  CPSS_IP_PROTOCOL_STACK_ENT                    protocolStack,
    IN  PRV_CPSS_DXCH_LPM_SHADOW_DEVS_LIST_STC        *devListPtr
)
{
    GT_STATUS ret = GT_OK;
    GT_U8   *ipShareDevs;
    GT_U32 numOfShareDevs;
    GT_U32  i;
    GT_U32 tcamRow = 0;
    GT_U32 tcamColumn = 0;
    GT_BOOL isToken;
    CPSS_DXCH_TCAM_MANAGER_TCAM_LOCATION_STC    *tcamLocation = NULL ;

    /* continue only if there are devices in the share devices list */
    if (devListPtr->ipShareDevNum == 0)
        return GT_OK;

    ipShareDevs     = devListPtr->ipShareDevs;
    numOfShareDevs  = devListPtr->ipShareDevNum;

    isToken = (vrId & BIT_14) ? GT_FALSE : GT_TRUE;
    vrId &= (~BIT_14);
    if (isToken == GT_TRUE)
    {
        ret =  prvCpssDxChLpmTcamGetTcamRowColumn(ipTcamShadowPtr,
                                                  tcamRulePtr,
                                                  &tcamRow,
                                                  &tcamColumn);
        if (ret != GT_OK)
        {
            return ret;
        }
    }
    else
    {
        /* tcamRulePtr is alredy contains tcam location:*/
        tcamLocation = (CPSS_DXCH_TCAM_MANAGER_TCAM_LOCATION_STC*)tcamRulePtr;
        tcamRow = tcamLocation->row;
        tcamColumn = tcamLocation->column;
    }

    /* for Cheetah2, for pcl based routing */
    if ((PRV_CPSS_PP_MAC(ipShareDevs[0])->devFamily == CPSS_PP_FAMILY_CHEETAH2_E) && (vrId > 0))
    {
        return ipTcamTrieDelPrefixCheetah(vrId,
                                          tcamRulePtr,
                                          ipTcamShadowPtr,
                                          pData,
                                          devListPtr);
    }

    /* go over all the share device and invalidate */
    for (i = 0; i < numOfShareDevs; i++)
    {
        if (protocolStack == CPSS_IP_PROTOCOL_IPV4_E)
            ret = cpssDxChIpv4PrefixInvalidate(ipShareDevs[i],
                                               tcamRow,
                                               tcamColumn);
        else
            ret = cpssDxChIpv6PrefixInvalidate(ipShareDevs[i],tcamRow);
        if (ret != GT_OK)
        {
            return ret;
        }

    }
    return ret;
}

/*******************************************************************************
* prvCpssDxChLpmTcamTrieDelIpv4PrefixCheetah2
*
* DESCRIPTION:
*       utility function to Delete IP prefix rule from IP table.
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       vrId                - the virtual router id.
*       tcamRulePtr         - the tcam rule to delete it from.
*       ipTcamShadowPtr     - ip shadow we are working on.
*       pData               - the data entry that was attached to this prefix.
*       devListPtr          - pointer to the device list to preform the actions on.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*******************************************************************************/
GT_STATUS prvCpssDxChLpmTcamTrieDelIpv4PrefixCheetah2
(
    IN  GT_U32                                        vrId,
    IN  GT_PTR                                        tcamRulePtr,
    IN PRV_CPSS_DXCH_LPM_TCAM_SHADOW_STC              *ipTcamShadowPtr,
    IN  GT_PTR                                        pData,
    IN  PRV_CPSS_DXCH_LPM_SHADOW_DEVS_LIST_STC        *devListPtr
)
{
    return prvCpssDxChLpmTcamTrieDelPrefixCheetah2(vrId,
                                                   tcamRulePtr,
                                                   ipTcamShadowPtr,
                                                   pData,
                                                   CPSS_IP_PROTOCOL_IPV4_E,
                                                   devListPtr);
}

/*******************************************************************************
* prvCpssDxChLpmTcamTrieDelIpv6PrefixCheetah2
*
* DESCRIPTION:
*       utility function to Delete IP prefix rule from IP table.
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       vrId                - the virtual router id.
*       tcamRulePtr         - the tcam rule to delete it from.
*       ipTcamShadowPtr     - ip shadow we are working on.
*       pData               - the data entry that was attached to this prefix.
*       devListPtr          - pointer to the device list to preform the actions on.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*******************************************************************************/
GT_STATUS prvCpssDxChLpmTcamTrieDelIpv6PrefixCheetah2
(
    IN  GT_U32                                        vrId,
    IN  GT_PTR                                        tcamRulePtr,
    IN  PRV_CPSS_DXCH_LPM_TCAM_SHADOW_STC             *ipTcamShadowPtr,
    IN  GT_PTR                                        pData,
    IN  PRV_CPSS_DXCH_LPM_SHADOW_DEVS_LIST_STC        *devListPtr
)
{
    return prvCpssDxChLpmTcamTrieDelPrefixCheetah2(vrId,
                                                   tcamRulePtr,
                                                   ipTcamShadowPtr,
                                                   pData,
                                                   CPSS_IP_PROTOCOL_IPV6_E,
                                                   devListPtr);
}

/*******************************************************************************
* ipTcamTrieMovePrefixCheetah2PclBasedVr
*
* DESCRIPTION:
*       utility function to Update move PCE entry action in IP PCL table.
*
* APPLICABLE DEVICES:
*        DxCh2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       vrId                - the virtual router id.
*       moveFromTcamRulePtr - the tcam rule to move from.
*       moveToTcamRulePtr   - the tcam rule to move to .
*       pData               - the data entry that that is attached to this moved prefix.
*       protocolStack       - the protocol stack of the prefix.
*       devListPtr          - pointer to the device list to preform the actions on.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on error
*
* COMMENTS:
*       None.
*******************************************************************************/
static GT_STATUS ipTcamTrieMovePrefixCheetah2PclBasedVr
(
    IN  GT_U32                                 vrId,
    IN  PRV_CPSS_DXCH_LPM_TCAM_RULE_NODE_STC   *moveFromTcamRulePtr,
    IN  PRV_CPSS_DXCH_LPM_TCAM_RULE_NODE_STC   *moveToTcamRulePtr,
    IN  GT_PTR                                 pData,
    IN  CPSS_IP_PROTOCOL_STACK_ENT             protocolStack,
    IN  PRV_CPSS_DXCH_LPM_SHADOW_DEVS_LIST_STC *devListPtr
)
{
    GT_STATUS ret = GT_OK;
    GT_U8 ipAddr[16];
    GT_U32 prefixLen;
    /* continue only if there are devices in the share devices list */
    if (devListPtr->ipShareDevNum == 0)
        return GT_OK;

    /* get ip address from the rule's node */
    ret = prvCpssDxChLpmTcamPatTrieGetAddrFromNode(moveFromTcamRulePtr->trieNode,
                                                   ipAddr,&prefixLen);
    if (ret != GT_OK)
    {
        return (ret);
    }

    /* first write the new rule */
    ret = ipTcamTrieSetPrefixIpCheetah2PclBasedVr(
            vrId,ipAddr,prefixLen,0,moveToTcamRulePtr, pData,
            (protocolStack == CPSS_IP_PROTOCOL_IPV4_E) ? GT_FALSE : GT_TRUE,
            devListPtr);
    if (ret != GT_OK)
    {
        return ret;
    }

    /* now delete the old rule */
    ret = ipTcamTrieDelPrefixCheetah(vrId,
                                    (GT_PTR)moveFromTcamRulePtr,
                                     moveFromTcamRulePtr->trieNode->ipTcamShadowPtr,
                                     pData,
                                     devListPtr);
    if (ret != GT_OK)
    {
        return ret;
    }


    return ret;
}

/*******************************************************************************
* ipTcamTrieMovePrefixCheetah2
*
* DESCRIPTION:
*       utility function to Update move rule entry in IP TCAM table.
*
* APPLICABLE DEVICES:
*        DxCh2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       vrId                - the virtual router id.
*       moveFromTcamRulePtr - the tcam rule to move from.
*       moveToTcamRulePtr   - the tcam rule to move to .
*       trieNodePtr         - trie node associated with moved rule.
*       protocolStack       - the protocol stack of the prefix
*       devListPtr          - pointer to the device list to preform the actions on.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on error
*
* COMMENTS:
*       None.
*******************************************************************************/
static GT_STATUS ipTcamTrieMovePrefixCheetah2
(
    IN  GT_U32                                    vrId,
    IN  GT_PTR                                    moveFromTcamRulePtr,
    IN  GT_PTR                                    moveToTcamRulePtr,
    IN  PRV_CPSS_DXCH_LPM_TCAM_PAT_TRIE_NODE_STC  *trieNodePtr,
    IN  CPSS_IP_PROTOCOL_STACK_ENT                protocolStack,
    IN  PRV_CPSS_DXCH_LPM_SHADOW_DEVS_LIST_STC    *devListPtr
)
{
    GT_STATUS ret = GT_OK;
    GT_U8       *ipShareDevs;
    GT_U8 ipAddr[16];
    GT_U32 prefixLen;
    /* continue only if there are devices in the share devices list */
    if (devListPtr->ipShareDevNum == 0)
        return GT_OK;

    ipShareDevs     = devListPtr->ipShareDevs;

    /* for Cheetah2, for pcl based routing */
    if ((PRV_CPSS_PP_MAC(ipShareDevs[0])->devFamily == CPSS_PP_FAMILY_CHEETAH2_E) && (vrId > 0))
    {
        return ipTcamTrieMovePrefixCheetah2PclBasedVr(vrId,
                                                      (PRV_CPSS_DXCH_LPM_TCAM_RULE_NODE_STC*)moveFromTcamRulePtr,
                                                      (PRV_CPSS_DXCH_LPM_TCAM_RULE_NODE_STC*)moveToTcamRulePtr,
                                                      trieNodePtr->pData,
                                                      protocolStack,
                                                      devListPtr);
    }

    /* get ip address from the rule's node */
    ret = prvCpssDxChLpmTcamPatTrieGetAddrFromNode(trieNodePtr,ipAddr,&prefixLen);
    if (ret != GT_OK)
    {
        return (ret);
    }

    /* first write the new rule */
    ret = prvCpssDxChLpmTcamTrieSetPrefixIpCheetah2(vrId,
                                                    ipAddr,
                                                    prefixLen,
                                                    0,
                                                    GT_FALSE,
                                                    0,
                                                    moveToTcamRulePtr,
                                                    trieNodePtr->ipTcamShadowPtr,
                                                    trieNodePtr->pData,
                                                    protocolStack,
                                                    devListPtr);
    if (ret != GT_OK)
    {
        return (ret);
    }

    /* now delete the old rule */
    ret = prvCpssDxChLpmTcamTrieDelPrefixCheetah2(vrId,
                                                  moveFromTcamRulePtr,
                                                  trieNodePtr->ipTcamShadowPtr,
                                                  trieNodePtr->pData,
                                                  protocolStack,
                                                  devListPtr);
    if (ret != GT_OK)
    {
        return (ret);
    }

    return ret;
}

/*******************************************************************************
* ipTcamTrieMoveIpv4PrefixCheetah2
*
* DESCRIPTION:
*       utility function to Update move rule entry in IP TCAM table.
*
* APPLICABLE DEVICES:
*        DxCh2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       vrId                - the virtual router id.
*       moveFromTcamRulePtr - the tcam rule to move from.
*       moveToTcamRulePtr   - the tcam rule to move to .
*       trieNodePtr         - trie node associated with moved rule
*       devListPtr          - pointer to the device list to preform the actions on.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on error
*
* COMMENTS:
*       None.
*******************************************************************************/
static GT_STATUS ipTcamTrieMoveIpv4PrefixCheetah2
(
    IN  GT_U32                                    vrId,
    IN  GT_PTR                                    moveFromTcamRulePtr,
    IN  GT_PTR                                    moveToTcamRulePtr,
    IN  PRV_CPSS_DXCH_LPM_TCAM_PAT_TRIE_NODE_STC  *trieNodePtr,
    IN  PRV_CPSS_DXCH_LPM_SHADOW_DEVS_LIST_STC    *devListPtr
)
{
    return ipTcamTrieMovePrefixCheetah2(vrId,
                                        moveFromTcamRulePtr,
                                        moveToTcamRulePtr,
                                        trieNodePtr,
                                        CPSS_IP_PROTOCOL_IPV4_E,
                                        devListPtr);
}

/*******************************************************************************
* ipTcamTrieMoveIpv6PrefixCheetah2
*
* DESCRIPTION:
*       utility function to Update move rule entry in IP TCAM table.
*
* APPLICABLE DEVICES:
*        DxCh2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       vrId                - the virtual router id.
*       moveFromTcamRulePtr - the tcam rule to move from.
*       moveToTcamRulePtr   - the tcam rule to move to .
*       trieNodePtr         - trie node associated with moved rule
*       devListPtr          - pointer to the device list to preform the actions on.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on error
*
* COMMENTS:
*       None.
*******************************************************************************/
static GT_STATUS ipTcamTrieMoveIpv6PrefixCheetah2
(
    IN  GT_U32                                        vrId,
    IN  GT_PTR                                        moveFromTcamRulePtr,
    IN  GT_PTR                                        moveToTcamRulePtr,
    IN  PRV_CPSS_DXCH_LPM_TCAM_PAT_TRIE_NODE_STC      *trieNodePtr,
    IN  PRV_CPSS_DXCH_LPM_SHADOW_DEVS_LIST_STC        *devListPtr
)
{
    return ipTcamTrieMovePrefixCheetah2(vrId,
                                        moveFromTcamRulePtr,
                                        moveToTcamRulePtr,
                                        trieNodePtr,
                                        CPSS_IP_PROTOCOL_IPV6_E,
                                        devListPtr);
}

/*******************************************************************************
* ipTcamVrCompFunc
*
* DESCRIPTION:
*       a Vitual router structs compare function.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* NOT APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       elem1Ptr - the first VR struct.
*       elem2Ptr - the second VR struct.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       on of the GT_COMP_RES.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_COMP_RES ipTcamVrCompFunc
(
    IN GT_VOID *elem1Ptr,
    IN GT_VOID *elem2Ptr
)
{
    PRV_CPSS_DXCH_LPM_TCAM_VR_TBL_BLOCK_STC *vr1Ptr;
    PRV_CPSS_DXCH_LPM_TCAM_VR_TBL_BLOCK_STC *vr2Ptr;

    vr1Ptr = elem1Ptr;
    vr2Ptr = elem2Ptr;

    if((vr1Ptr == NULL) && (vr2Ptr == NULL))
        return GT_EQUAL;
    if (vr1Ptr == NULL)
        return GT_SMALLER;
    if (vr2Ptr == NULL)
        return GT_GREATER;

    if(vr1Ptr->vrId == vr2Ptr->vrId)
    {
        return GT_EQUAL;
    }

    if (vr1Ptr->vrId < vr2Ptr->vrId)
        return GT_SMALLER;
    return GT_GREATER;
}

/*******************************************************************************
* prvCpssDxChLpmTcamCheckCapacityForShadow
*
* DESCRIPTION:
*       This function checks the capacity given in lpb db create or capacity update.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* NOT APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       shadowType              - the type of shadow we're building
*       indexesRangePtr         - the range of TCAM indexes availble for this
*                                 LPM DB (see explanation in
*                                 CPSS_DXCH_IP_TCAM_LPM_MANGER_INDEX_RANGE_STC)
*       partitionEnable         - whether to enable partitioning of the TCAM
*                                 according to the tcamLpmManagerCapcaityCfgPtr
*       capacityCfgPtr          - The capcity configuration struct.
*
* OUTPUTS:
*       totalLinesPtr           - total lines needed
*       totalSingleEntriesPtr   - total single entries needed
*       totalQuadEntriesPtr     - total quad entries needed
*
* RETURNS:
*       GT_OK on success, or
*       GT_BAD_PARAM on bad capacityCfg.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS prvCpssDxChLpmTcamCheckCapacityForShadow
(
    IN PRV_CPSS_DXCH_LPM_SHADOW_TYPE_ENT                shadowType,
    IN PRV_CPSS_DXCH_LPM_TCAM_MANGER_INDEX_RANGE_STC    *indexesRangePtr,
    IN GT_BOOL                                          partitionEnable,
    IN  PRV_CPSS_DXCH_LPM_TCAM_MANGER_CAPCITY_CFG_STC   *capacityCfgPtr,
    OUT GT_U32                                          *totalLinesPtr,
    OUT GT_U32                                          *totalSingleEntriesPtr,
    OUT GT_U32                                          *totalQuadEntriesPtr
)
{
    GT_U32 totalLines = 0;
    GT_U32 totalSingleEntries = 0;
    GT_U32 totalQuadEntries = 0;
    GT_U32 totalEntries = 0;

    /*for non xcat we do the check in tcam entries handler*/
    if (!(shadowType == PRV_CPSS_DXCH_LPM_TCAM_XCAT_POLICY_BASED_ROUTING_SHADOW_E ||
          shadowType == PRV_CPSS_DXCH_LPM_TCAM_XCAT_SHADOW_E))
        return GT_OK;

    /* verify that range is valid one */
    if (indexesRangePtr->lastIndex < indexesRangePtr->firstIndex)
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);

    /* check capacity */
    if (partitionEnable == GT_TRUE)
    {
        if ((capacityCfgPtr->numOfIpv4McSourcePrefixes == 0) &&
            (capacityCfgPtr->numOfIpv4Prefixes == 0)         &&
            (capacityCfgPtr->numOfIpv6Prefixes == 0))
        {
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
        }
    }

    /* total number of TCAM lines */
    totalLines = indexesRangePtr->lastIndex - indexesRangePtr->firstIndex +1;

    *totalLinesPtr = totalLines;

    /* get number of needed single and quad entries */
    if (shadowType == PRV_CPSS_DXCH_LPM_TCAM_XCAT_POLICY_BASED_ROUTING_SHADOW_E)
    {
        /*in PBR ipv6 holds a single entry as in ipv4*/
        totalSingleEntries = capacityCfgPtr->numOfIpv4Prefixes + capacityCfgPtr->numOfIpv4McSourcePrefixes + capacityCfgPtr->numOfIpv6Prefixes;
        totalQuadEntries = 0;

        /* verify there was no overflow */
        if ((totalSingleEntries < capacityCfgPtr->numOfIpv4Prefixes) ||
            (totalSingleEntries < capacityCfgPtr->numOfIpv4McSourcePrefixes) ||
            (totalSingleEntries < capacityCfgPtr->numOfIpv6Prefixes) ||
            (totalSingleEntries < (capacityCfgPtr->numOfIpv4Prefixes + capacityCfgPtr->numOfIpv4McSourcePrefixes)) ||
            (totalSingleEntries < (capacityCfgPtr->numOfIpv4Prefixes + capacityCfgPtr->numOfIpv6Prefixes)) ||
            (totalSingleEntries < (capacityCfgPtr->numOfIpv4McSourcePrefixes + capacityCfgPtr->numOfIpv6Prefixes)))
        {
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
        }
    }
    else
    {
        totalSingleEntries = capacityCfgPtr->numOfIpv4Prefixes + capacityCfgPtr->numOfIpv4McSourcePrefixes;
        totalQuadEntries = capacityCfgPtr->numOfIpv6Prefixes;

        /* verify there was no overflow */
        if ((totalSingleEntries < capacityCfgPtr->numOfIpv4Prefixes) ||
            (totalSingleEntries < capacityCfgPtr->numOfIpv4McSourcePrefixes))
        {
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
        }
    }
    *totalSingleEntriesPtr = totalSingleEntries;
    *totalQuadEntriesPtr = totalQuadEntries;

    /* verify there was no overflow */
    if (((totalLines*4) < totalLines) ||
        ((totalQuadEntries*4) < totalQuadEntries))
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    totalEntries = totalSingleEntries + (totalQuadEntries*4);
    if ((totalEntries < totalSingleEntries) ||
        (totalEntries < (totalQuadEntries*4)))
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    /*each line hold 4 single entries*/
    if (totalEntries > (totalLines*4))
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    return GT_OK;
}

/*******************************************************************************
* prvCpssDxChLpmTcamPrepareReservation
*
* DESCRIPTION:
*       This function returns a pointer to a new IP shadow struct.
*
* APPLICABLE DEVICES:
*        xCat; xCat3; Lion; xCat2; Lion2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       shadowType                  - the type of shadow we're building (cheetah+ or cheetah2)
*       partitionEnable             - whether to enable partitioning of the TCAM
*                                     according to tcamLpmManagerCapacityCfgPtr
*       capacityCfgPtr              - The capcity configuration struct.
*       totalPlacesNeeded           - total places needed
*       totalCapacity               - total number of lines possible
*       sizeOfArrayPtr              - points to allocated size of the
*                                     requestedEntriesArrayPtr array
*
* OUTPUTS:
*       reservationTypePtr          - type of reservation needed
*       requestedEntriesArrayPtr    - array of amounts needed per entry type
*       sizeOfArrayPtr              - number of elements filled in the
*                                     requestedEntriesArrayPtr array
*       numberOfUnusedTcamEntriesPtr  - points to unused Tcam entires
*
* RETURNS:
*       GT_OK on success, or
*       GT_BAD_PARAM on bad capacityCfg.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS prvCpssDxChLpmTcamPrepareReservation
(
    IN    PRV_CPSS_DXCH_LPM_SHADOW_TYPE_ENT                   shadowType,
    IN    GT_BOOL                                             partitionEnable,
    IN    PRV_CPSS_DXCH_LPM_TCAM_MANGER_CAPCITY_CFG_STC       *capacityCfgPtr,
    IN    GT_U32                                              totalPlacesNeeded,
    IN    GT_U32                                              totalCapacity,
    OUT   CPSS_DXCH_TCAM_MANAGER_ENTRIES_RESERVATION_TYPE_ENT *reservationTypePtr,
    OUT   CPSS_DXCH_TCAM_MANAGER_ENTRY_AMOUNT_STC             *requestedEntriesArrayPtr,
    INOUT GT_U32                                              *sizeOfArrayPtr,
    OUT   GT_U32                                              *numberOfUnusedTcamEntriesPtr
)
{
    GT_U32  sizeOfArrayUsed = 0;
    *numberOfUnusedTcamEntriesPtr=0;

    /* this function is supported only for xCat shadow types */
    if ((shadowType != PRV_CPSS_DXCH_LPM_TCAM_XCAT_SHADOW_E) &&
        (shadowType != PRV_CPSS_DXCH_LPM_TCAM_XCAT_POLICY_BASED_ROUTING_SHADOW_E))
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_SUPPORTED, LOG_ERROR_NO_MSG);
    }

    /* policy based routing doesn't support MC */
    if (shadowType == PRV_CPSS_DXCH_LPM_TCAM_XCAT_POLICY_BASED_ROUTING_SHADOW_E)
    {
        capacityCfgPtr->numOfIpv4McSourcePrefixes = 0;
    }

    /* in case of partitionEnable == GT_TRUE, there must be some prefixed required */
    if ((partitionEnable == GT_TRUE) &&
        (capacityCfgPtr->numOfIpv4Prefixes == 0) &&
        (capacityCfgPtr->numOfIpv4McSourcePrefixes == 0) &&
        (capacityCfgPtr->numOfIpv6Prefixes == 0))
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    /* prepare reservationTypePtr */
    if (partitionEnable == GT_TRUE)
        *reservationTypePtr = CPSS_DXCH_TCAM_MANAGER_STATIC_ENTRIES_RESERVATION_E;
    else
        *reservationTypePtr = CPSS_DXCH_TCAM_MANAGER_DYNAMIC_ENTRIES_RESERVATION_E;

    /* fill requestedEntriesArrayPtr according to the shadow type and capacityCfgPtr */
    if (shadowType == PRV_CPSS_DXCH_LPM_TCAM_XCAT_POLICY_BASED_ROUTING_SHADOW_E)
    {
        /* on policy based routing mode, each IPv4 & IPv6 prefix resides in SINGLE TCAM entry */

        if (*sizeOfArrayPtr < 1)
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);

        sizeOfArrayUsed = 1;

        requestedEntriesArrayPtr[0].entryType = CPSS_DXCH_TCAM_MANAGER_SINGLE_ENTRY_E;
        requestedEntriesArrayPtr[0].amount    = capacityCfgPtr->numOfIpv4Prefixes + capacityCfgPtr->numOfIpv6Prefixes;
    }
    else /* PRV_CPSS_DXCH_LPM_TCAM_XCAT_SHADOW_E */
    {
        /* on router TCAM routing mode, each IPv4 UC & IPv4 McSrc prefix resides
           in SINGLE TCAM entry and each IPv6 prefix resides in QUAD TCAM entry */

        /* check if single entries are needed */
        if ((capacityCfgPtr->numOfIpv4Prefixes > 0) || (capacityCfgPtr->numOfIpv4McSourcePrefixes > 0))
        {
            if (*sizeOfArrayPtr < (sizeOfArrayUsed+1))
                CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);

            requestedEntriesArrayPtr[sizeOfArrayUsed].entryType = CPSS_DXCH_TCAM_MANAGER_SINGLE_ENTRY_E;
            requestedEntriesArrayPtr[sizeOfArrayUsed].amount    = capacityCfgPtr->numOfIpv4Prefixes + capacityCfgPtr->numOfIpv4McSourcePrefixes;

            sizeOfArrayUsed++;
        }

        /* check if quad entries are needed */
        if (capacityCfgPtr->numOfIpv6Prefixes > 0)
        {
            if (*sizeOfArrayPtr < (sizeOfArrayUsed+1))
                CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);

            requestedEntriesArrayPtr[sizeOfArrayUsed].entryType = CPSS_DXCH_TCAM_MANAGER_QUAD_ENTRY_E;
            requestedEntriesArrayPtr[sizeOfArrayUsed].amount    = capacityCfgPtr->numOfIpv6Prefixes;

            sizeOfArrayUsed++;
        }
    }

    /* in case of partitionEnable == GT_TRUE, if IPv4 prefixes are required then
       all unused entries in the TCAM range are allcoated as IPv4 prefixes */
    if ((partitionEnable == GT_TRUE) &&
        ((capacityCfgPtr->numOfIpv4Prefixes > 0) || (capacityCfgPtr->numOfIpv4McSourcePrefixes > 0)))
    {
        if (totalCapacity*4 > totalPlacesNeeded)
        {
            /* from filling of requestedEntriesArrayPtr, IPv4 prefixes will always
               reside on first array index */
            requestedEntriesArrayPtr[0].amount += (totalCapacity*4 - totalPlacesNeeded);
            *numberOfUnusedTcamEntriesPtr = totalCapacity*4 - totalPlacesNeeded;
        }
    }

    *sizeOfArrayPtr = sizeOfArrayUsed;

    return GT_OK;
}
/*******************************************************************************
* createIpTcamShadowStruct
*
* DESCRIPTION:
*       This function returns a pointer to a new IP shadow struct.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* NOT APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       shadowType - the type of shadow we're building (cheetah+ or cheetah2)
*       indexesRangePtr       - the range of TCAM indexes availble for this
*                               LPM DB (see explanation in
*                               CPSS_DXCH_IP_TCAM_LPM_MANGER_INDEX_RANGE_STC)
*       partitionEnable       - whether to enable partitioning of the TCAM
*                               according to the tcamLpmManagercapacityCfgPtr
*       capacityCfgPtr        - The capcity configuration struct.
*       protocolStack         - the protocol stack to support
*       tcamManagerHandlerPtr - the TCAM manager handler

*
* OUTPUTS:
*       ipShadowOutPtrPtr     - A pointer to the created ip shadow struct.
*
* RETURNS:
*       GT_OK on success, or
*       GT_FAIL otherwise.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS createIpTcamShadowStruct
(
    IN PRV_CPSS_DXCH_LPM_SHADOW_TYPE_ENT                shadowType,
    IN PRV_CPSS_DXCH_LPM_TCAM_MANGER_INDEX_RANGE_STC    *indexesRangePtr,
    IN GT_BOOL                                          partitionEnable,
    IN  PRV_CPSS_DXCH_LPM_TCAM_MANGER_CAPCITY_CFG_STC   *capacityCfgPtr,
    OUT PRV_CPSS_DXCH_LPM_TCAM_SHADOW_STC               **ipShadowOutPtrPtr,
    IN  CPSS_IP_PROTOCOL_STACK_ENT                      protocolStack,
    IN  GT_VOID                                         *tcamManagerHandlerPtr

)
{
    GT_STATUS   ret = GT_OK;
    GT_STATUS   ret2 = GT_OK;
    GT_U32      maxNumOfVr;
    GT_U8       numOfDevs;                           /* Number of devices in system.         */
    PRV_CPSS_DXCH_LPM_TCAM_SHADOW_STC    *retShadow; /* A pointer to the shadow struct to be */
                                                     /* returned.                            */
    CPSS_DXCH_TCAM_MANAGER_RANGE_STC      tcamRange; /* physical tcam range                  */
    GT_CHAR clientName[CPSS_DXCH_TCAM_MANAGER_CLIENT_NAME_NAX_LENGTH_CNS]=  "lpmTcam";
    GT_U32 clientId = 0;
    GT_U32 totalCapacity = 0;
    GT_U32 sizeOfArray = 4;
    GT_U32 totalPlacesNeeded = 0;
    GT_U32 totalPlaceForSingle = 0;
    GT_U32 totalPlaceForQuad = 0;
    GT_U32 numberOfUnusedTcamEntries;
    CPSS_DXCH_TCAM_MANAGER_CLIENT_FUNC_STC  clientFuncPtr;
    CPSS_DXCH_TCAM_MANAGER_TCAM_TYPE_ENT tcamType =  /* tcam type for TCAM manager create    */
        CPSS_DXCH_TCAM_MANAGER_XCAT_AND_ABOVE_ROUTER_TCAM_E;
    CPSS_DXCH_TCAM_MANAGER_ENTRIES_RESERVATION_TYPE_ENT reservationType =
        CPSS_DXCH_TCAM_MANAGER_STATIC_ENTRIES_RESERVATION_E;
    CPSS_DXCH_TCAM_MANAGER_ENTRY_AMOUNT_STC  requestedEntriesArray[4];

    CPSS_DXCH_TCAM_MANAGER_RANGE_STC  reservationRange ={0,0};

    /* check capacity */
    ret = prvCpssDxChLpmTcamCheckCapacityForShadow(shadowType,
                                                   indexesRangePtr,
                                                   partitionEnable,
                                                   capacityCfgPtr,
                                                   &totalCapacity,
                                                   &totalPlaceForSingle,
                                                   &totalPlaceForQuad);

    totalPlacesNeeded = totalPlaceForSingle + totalPlaceForQuad*4;
    if (ret != GT_OK)
    {
        return ret;
    }

    tcamRange.firstLine = indexesRangePtr->firstIndex;
    tcamRange.lastLine  = indexesRangePtr->lastIndex;
    retShadow = NULL;

    numOfDevs = PRV_CPSS_MAX_PP_DEVICES_CNS;

    retShadow = (PRV_CPSS_DXCH_LPM_TCAM_SHADOW_STC *)
                        cpssOsMalloc(sizeof(PRV_CPSS_DXCH_LPM_TCAM_SHADOW_STC));
    if (retShadow == NULL)
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_FAIL, LOG_ERROR_NO_MSG);

    cpssOsMemSet(retShadow,0,sizeof(PRV_CPSS_DXCH_LPM_TCAM_SHADOW_STC));

    /* save shadow type in shadow */
    retShadow->shadowType = shadowType;

    /* Ch2 do not support Multiple Virtual Router by default*/
    if(shadowType == PRV_CPSS_DXCH_LPM_TCAM_CHEETAH2_SHADOW_E)
        retShadow->multipleVirtualRouterSupport = GT_FALSE;
    else
        retShadow->multipleVirtualRouterSupport = GT_TRUE;

    /* save TCAM index range allocated to the shadow */
    retShadow->tcamIndexRange.firstIndex = indexesRangePtr->firstIndex;
    retShadow->tcamIndexRange.lastIndex  = indexesRangePtr->lastIndex;

    /* save the number of rows in the TCAM used by the shadow */
    switch (shadowType)
    {
    case PRV_CPSS_DXCH_LPM_TCAM_CHEETAH_SHADOW_E:
        tcamRows = PRV_CPSS_DXCH_LPM_TCAM_CHEETAH_NUM_OF_RULES_IN_COLUMN_CNS;
        break;
    case PRV_CPSS_DXCH_LPM_TCAM_CHEETAH2_SHADOW_E:
        tcamRows = PRV_CPSS_DXCH_LPM_TCAM_CHEETAH2_NUM_OF_RULES_IN_COLUMN_CNS;
        break;
    case PRV_CPSS_DXCH_LPM_TCAM_CHEETAH3_SHADOW_E:
    case PRV_CPSS_DXCH_LPM_TCAM_XCAT_SHADOW_E:
        tcamRows = PRV_CPSS_DXCH_LPM_TCAM_CHEETAH3_TCAM_NUM_ROWS_CEILING_CNS;
        break;
    case PRV_CPSS_DXCH_LPM_TCAM_XCAT_POLICY_BASED_ROUTING_SHADOW_E:
        tcamRows = PRV_CPSS_DXCH_LPM_TCAM_XCAT_POLICY_TCAM_NUM_OF_RULES_CEILING_CNS;
        break;
    CPSS_COVERITY_NON_ISSUE_BOOKMARK
    /* coverity[dead_error_begin] */default:
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_FAIL, LOG_ERROR_NO_MSG);
    }

    retShadow->shadowDevList.ipShareDevNum = 0;

    /* since now we gonna work on the whole list, set the pointer right*/
    retShadow->workDevListPtr = &retShadow->shadowDevList;

    *ipShadowOutPtrPtr                 = retShadow;
    retShadow->isIpVerIsInitialized[0] = GT_FALSE;
    retShadow->isIpVerIsInitialized[1] = GT_FALSE;

    retShadow->tcamPartitionEnable = partitionEnable;
    retShadow->numOfIpPrefixes[CPSS_IP_PROTOCOL_IPV4_E] =
                                                capacityCfgPtr->numOfIpv4Prefixes;
    retShadow->numOfIpPrefixes[CPSS_IP_PROTOCOL_IPV6_E] =
                                                capacityCfgPtr->numOfIpv6Prefixes;

    retShadow->numOfIpMcSrcAddr = capacityCfgPtr->numOfIpv4McSourcePrefixes;

    /* init prefix counters */
    retShadow->allocatedIpPrefixCounters.numOfAllocatedIpPrefixes[CPSS_IP_PROTOCOL_IPV4_E] = 0;
    retShadow->allocatedIpPrefixCounters.numOfAllocatedIpPrefixes[CPSS_IP_PROTOCOL_IPV6_E] = 0;
    retShadow->allocatedIpPrefixCounters.numOfAllocatedIpv4McSourcePrefixes = 0;

    /* Allocate the device list                                 */
    if ((retShadow->shadowDevList.ipShareDevs =
         (GT_U8*)cpssOsMalloc(sizeof(GT_U8) * numOfDevs)) == NULL)
    {
        cpssOsFree(retShadow);
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_FAIL, LOG_ERROR_NO_MSG);
    }

    /* Init the IP unit sem.                                  */
    /*if (osSemBinCreate("IPSem",1,&(retShadow->ipUnitSem)) != GT_OK)
    {
        return GT_FAIL;
    }*/

    /* allocate the virtual routers skip list*/
    switch (shadowType)
    {
    case PRV_CPSS_DXCH_LPM_TCAM_CHEETAH_SHADOW_E:
        maxNumOfVr = CPSS_DXCH_MAX_VR_ID_CNS;
        retShadow->tcamManagementMode = PRV_CPSS_DXCH_LPM_TCAM_MANAGEMENT_TCAM_ENTRIES_HANDLER_MODE_E;
        break;
    case PRV_CPSS_DXCH_LPM_TCAM_CHEETAH2_SHADOW_E:
        maxNumOfVr = CPSS_DXCH2_MAX_VR_ID_CNS;
        retShadow->tcamManagementMode = PRV_CPSS_DXCH_LPM_TCAM_MANAGEMENT_TCAM_ENTRIES_HANDLER_MODE_E;
        break;
    case PRV_CPSS_DXCH_LPM_TCAM_CHEETAH3_SHADOW_E:
        maxNumOfVr = CPSS_DXCH3_MAX_VR_ID_CNS;
        retShadow->tcamManagementMode = PRV_CPSS_DXCH_LPM_TCAM_MANAGEMENT_TCAM_ENTRIES_HANDLER_MODE_E;
        break;
    case PRV_CPSS_DXCH_LPM_TCAM_XCAT_SHADOW_E:
        maxNumOfVr = CPSS_DXCH_XCAT_MAX_VR_ID_CNS;
        tcamType = CPSS_DXCH_TCAM_MANAGER_XCAT_AND_ABOVE_ROUTER_TCAM_E;
        retShadow->tcamManagementMode = PRV_CPSS_DXCH_LPM_TCAM_MANAGEMENT_TCAM_MANAGER_MODE_E;
        break;
    case PRV_CPSS_DXCH_LPM_TCAM_XCAT_POLICY_BASED_ROUTING_SHADOW_E:
        maxNumOfVr = CPSS_DXCH_XCAT_PBR_MAX_VR_ID_CNS;
        tcamType = CPSS_DXCH_TCAM_MANAGER_XCAT_AND_ABOVE_PCL_TCAM_E;
        retShadow->tcamManagementMode = PRV_CPSS_DXCH_LPM_TCAM_MANAGEMENT_TCAM_MANAGER_MODE_E;
        break;
    CPSS_COVERITY_NON_ISSUE_BOOKMARK
    /* coverity[dead_error_begin] */default:
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_FAIL, LOG_ERROR_NO_MSG);
    }
    retShadow->vrSl = prvCpssSlInit(ipTcamVrCompFunc, maxNumOfVr);
    if (retShadow->vrSl == NULL)
    {
        cpssOsFree(retShadow->shadowDevList.ipShareDevs);
        cpssOsFree(retShadow);
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_OUT_OF_CPU_MEM, LOG_ERROR_NO_MSG);
    }

    if ( retShadow->tcamManagementMode ==
          PRV_CPSS_DXCH_LPM_TCAM_MANAGEMENT_TCAM_MANAGER_MODE_E)
    {
        /* if TCAM manager was not created yet and shadow type is XCAT or XCAT policy-based,
           make new TCAM manager handler */
        if (tcamManagerHandlerPtr == NULL)
        {
        ret = cpssDxChTcamManagerCreate(tcamType, &tcamRange,&tcamManagerHandlerPtr);
        if (GT_OK != ret)
        {
#ifdef OS_MALLOC_MEMORY_LEAKAGE_DBG
            ret2 = prvCpssSlDestroy(retShadow->vrSl, myCpssOsFree);
#else /*! OS_MALLOC_MEMORY_LEAKAGE_DBG */
            ret2 = prvCpssSlDestroy(retShadow->vrSl, cpssOsFree);
#endif /*! OS_MALLOC_MEMORY_LEAKAGE_DBG */
            if (GT_OK != ret2)
            {
                return ret2;
            }
            cpssOsFree(retShadow->shadowDevList.ipShareDevs);
            cpssOsFree(retShadow);
            return ret;
        }
            retShadow->isTcamManagerInternal = GT_TRUE;
        }
        else
        {
            retShadow->isTcamManagerInternal = GT_FALSE;
        }
        retShadow->tcamManagerHandlerPtr = tcamManagerHandlerPtr;
        clientFuncPtr.moveToAnywhereFuncPtr = prvCpssDxChLpmTcamMoveToAnywhere;
        clientFuncPtr.moveToLocationFuncPtr = prvCpssDxChLpmTcamMoveToLocation;
        clientFuncPtr.checkIfDefaultLocationFuncPtr = prvCpssDxChLpmTcamCheckIfDefaultLocation;
        /* make client registration */
        ret =  cpssDxChTcamManagerClientRegister(tcamManagerHandlerPtr,
                                                 clientName,
                                                 &clientFuncPtr,
                                                 &clientId);
        if (GT_OK != ret)
        {
#ifdef OS_MALLOC_MEMORY_LEAKAGE_DBG
            ret2 = prvCpssSlDestroy(retShadow->vrSl, myCpssOsFree);
#else /*! OS_MALLOC_MEMORY_LEAKAGE_DBG */
            ret2 = prvCpssSlDestroy(retShadow->vrSl, cpssOsFree);
#endif /*! OS_MALLOC_MEMORY_LEAKAGE_DBG */
            if (GT_OK != ret2)
            {
                return ret2;
            }
            cpssOsFree(retShadow->shadowDevList.ipShareDevs);
            cpssOsFree(retShadow);
            return ret;
        }
        retShadow->clientId = clientId;

        ret = prvCpssDxChLpmTcamPrepareReservation(shadowType,
                                                   partitionEnable,
                                                   capacityCfgPtr,
                                                   totalPlacesNeeded,
                                                   totalCapacity,
                                                   &reservationType,
                                                   requestedEntriesArray,
                                                   &sizeOfArray,
                                                   &numberOfUnusedTcamEntries);

        if (ret != GT_OK)
        {
#ifdef OS_MALLOC_MEMORY_LEAKAGE_DBG
            ret2 = prvCpssSlDestroy(retShadow->vrSl, myCpssOsFree);
#else /*! OS_MALLOC_MEMORY_LEAKAGE_DBG */
            ret2 = prvCpssSlDestroy(retShadow->vrSl, cpssOsFree);
#endif /*! OS_MALLOC_MEMORY_LEAKAGE_DBG */
            if (GT_OK != ret2)
            {
                return ret2;
            }
            cpssOsFree(retShadow->shadowDevList.ipShareDevs);
            cpssOsFree(retShadow);
            return ret;
        }
        if ( ((protocolStack == CPSS_IP_PROTOCOL_IPV4_E) ||
             (protocolStack == CPSS_IP_PROTOCOL_IPV4V6_E)) &&
              (retShadow->tcamPartitionEnable == GT_TRUE) )
        {
            retShadow->numOfIpPrefixes[CPSS_IP_PROTOCOL_IPV4_E]+=numberOfUnusedTcamEntries;
        }
        reservationRange.firstLine = indexesRangePtr->firstIndex;
        reservationRange.lastLine = indexesRangePtr->lastIndex;

        /* Reserve entries for a client in the TCAM manager*/
        ret =  cpssDxChTcamManagerEntriesReservationSet(tcamManagerHandlerPtr,
                                                        clientId,
                                                        reservationType,
                                                        requestedEntriesArray,
                                                        sizeOfArray,
                                                        &reservationRange);
        if (GT_OK != ret)
        {
#ifdef OS_MALLOC_MEMORY_LEAKAGE_DBG
            ret2 = prvCpssSlDestroy(retShadow->vrSl, myCpssOsFree);
#else /*! OS_MALLOC_MEMORY_LEAKAGE_DBG */
            ret2 = prvCpssSlDestroy(retShadow->vrSl, cpssOsFree);
#endif /*! OS_MALLOC_MEMORY_LEAKAGE_DBG */
            if (GT_OK != ret2)
            {
                return ret2;
            }
            cpssOsFree(retShadow->shadowDevList.ipShareDevs);
            cpssOsFree(retShadow);
            return ret;
        }


    }
    else
    {
        /* now create the TCAM entries handler */
        ret = prvCpssDxChLpmTcamEntriesHandlerCreate(retShadow,indexesRangePtr,partitionEnable,
                                                     capacityCfgPtr,0,protocolStack,tcamRows);
        if (GT_OK != ret)
        {
#ifdef OS_MALLOC_MEMORY_LEAKAGE_DBG
            ret2 = prvCpssSlDestroy(retShadow->vrSl, myCpssOsFree);
#else /*! OS_MALLOC_MEMORY_LEAKAGE_DBG */
            ret2 = prvCpssSlDestroy(retShadow->vrSl, cpssOsFree);
#endif /*! OS_MALLOC_MEMORY_LEAKAGE_DBG */
            if (GT_OK != ret2)
            {
                return ret2;
            }
            cpssOsFree(retShadow->shadowDevList.ipShareDevs);
            cpssOsFree(retShadow);
            return ret;
        }
    }
    /* by default, LPM DB doesn't support using policy based routing MC defaults */
    retShadow->usePbrDefaultMc = GT_FALSE;
    /* new LPM DB has no virtual routers added */
    retShadow->sharedVrExist = GT_FALSE;
    retShadow->sharedVrId = 0;

    return ret;
}

/*******************************************************************************
* prvCpssDxChLpmTcamDeleteShadowStruct
*
* DESCRIPTION:
*       This function deletes IP TCAM shadow struct.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* NOT APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       ipTcamShadowPtr       - A pointer to the ip shadow struct.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                 - on success
*       GT_FAIL               - on error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS prvCpssDxChLpmTcamDeleteShadowStruct
(
    IN  PRV_CPSS_DXCH_LPM_TCAM_SHADOW_STC    *ipTcamShadowPtr
)
{
    GT_STATUS   rc = GT_OK;

    /* delete TCAM management module */
    if (ipTcamShadowPtr->tcamManagementMode == PRV_CPSS_DXCH_LPM_TCAM_MANAGEMENT_TCAM_MANAGER_MODE_E)
    {
        /* unregister the LPM DB as TCAM manager client */
        rc = cpssDxChTcamManagerClientUnregister(ipTcamShadowPtr->tcamManagerHandlerPtr,ipTcamShadowPtr->clientId);
        if (rc != GT_OK)
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_FAIL, LOG_ERROR_NO_MSG);

        /* delete the TCAM manager if it was created by the LPM DB */
        if (ipTcamShadowPtr->isTcamManagerInternal == GT_TRUE)
        {
            rc = cpssDxChTcamManagerDelete(ipTcamShadowPtr->tcamManagerHandlerPtr);
            if (rc != GT_OK)
                CPSS_LOG_ERROR_AND_RETURN_MAC(GT_FAIL, LOG_ERROR_NO_MSG);
        }
    }
    else if (ipTcamShadowPtr->tcamManagementMode == PRV_CPSS_DXCH_LPM_TCAM_MANAGEMENT_TCAM_ENTRIES_HANDLER_MODE_E)
    {
        rc = prvCpssDxChLpmTcamEntriesHandlerDelete(ipTcamShadowPtr);
        if (rc != GT_OK)
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_FAIL, LOG_ERROR_NO_MSG);
    }

    /* delete virtual routers skip list */
#ifdef OS_MALLOC_MEMORY_LEAKAGE_DBG
    rc = prvCpssSlDestroy(ipTcamShadowPtr->vrSl,myCpssOsFree);
#else /*! OS_MALLOC_MEMORY_LEAKAGE_DBG */
    rc = prvCpssSlDestroy(ipTcamShadowPtr->vrSl,cpssOsFree);
#endif /*! OS_MALLOC_MEMORY_LEAKAGE_DBG */
    if (rc != GT_OK)
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_FAIL, LOG_ERROR_NO_MSG);

    /* free the device list */
    cpssOsFree(ipTcamShadowPtr->shadowDevList.ipShareDevs);

    /* free TCAM shadow memory */
    cpssOsFree(ipTcamShadowPtr);

    return rc;
}

/*******************************************************************************
* ipTcamSetDefaultPrefixCheetah2
*
* DESCRIPTION:
*       Ch2 with Vr support we need to set some default prefix in Router Tcam
*       for each protocol in use because of uc SIP lookup that is performed
*       and Vr0 wasnt created yet. Otherwise unexpected result encountered.
*
* APPLICABLE DEVICES:
*        DxCh2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devListPtr      - The device list to add.
*       ipTcamShadowPtr - the ip shadow we are working on.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK   - on success,
*       GT_FAIL - otherwise.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS ipTcamSetDefaultPrefixCheetah2
(
    IN  PRV_CPSS_DXCH_LPM_SHADOW_DEVS_LIST_STC *devListPtr,
    IN  PRV_CPSS_DXCH_LPM_TCAM_SHADOW_STC      *ipTcamShadowPtr
)
{
    GT_U32      i,j;
    GT_U8       *ipShareDevs;
    GT_U32      numOfShareDevs;
    GT_U32      lastIpv4EntryIndex = 0/*, numOfIpv4Prefixes*/;
    GT_IPADDR   ipv4Addr = {0}, ipv4Mask = {0};
    GT_U32      lastIpv6EntryIndex = 0/*, numOfIpv6Prefixes*/;
    GT_IPV6ADDR ipv6Addr, ipv6Mask;
    GT_STATUS   retVal = GT_FAIL;
    GT_U32      tcamLines = tcamRows;
    CPSS_DXCH_IPV4_PREFIX_STC ipv4Prefix;
    CPSS_DXCH_IPV4_PREFIX_STC ipv4PrefixMask;
    CPSS_DXCH_IPV6_PREFIX_STC ipv6Prefix;
    CPSS_DXCH_IPV6_PREFIX_STC ipv6PrefixMask;

    /* continue only if there are devices in the share devices list */
    if (devListPtr->ipShareDevNum == 0)
        return GT_OK;

    ipShareDevs     = devListPtr->ipShareDevs;
    numOfShareDevs  = devListPtr->ipShareDevNum;

    if (tcamLines == 0)
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_FAIL, LOG_ERROR_NO_MSG);

    if (ipTcamShadowPtr->isIpVerIsInitialized[CPSS_IP_PROTOCOL_IPV4_E])
    {
        cpssOsMemSet(&ipv4Addr, 0x00, sizeof(ipv4Addr));
        cpssOsMemSet(&ipv4Mask, 0x00, sizeof(ipv4Mask));
    }

    if (ipTcamShadowPtr->isIpVerIsInitialized[CPSS_IP_PROTOCOL_IPV6_E])
    {
        cpssOsMemSet(&ipv6Addr, 0x00, sizeof(ipv6Addr));
        cpssOsMemSet(&ipv6Mask, 0x00, sizeof(ipv6Mask));
    }


    for (i = 0; i < numOfShareDevs; i++)
    {
        if (ipTcamShadowPtr->isIpVerIsInitialized[CPSS_IP_PROTOCOL_IPV4_E])
        {
            ipv4Prefix.vrId = 0;
            ipv4Prefix.ipAddr.u32Ip = ipv4Addr.u32Ip;
            ipv4Prefix.isMcSource = GT_FALSE;
            ipv4Prefix.mcGroupIndexRow = 0;
            ipv4Prefix.mcGroupIndexColumn = 0;

            ipv4PrefixMask.vrId = 0;    /* mask vrId */
            ipv4PrefixMask.ipAddr.u32Ip = ipv4Mask.u32Ip;
            /* you shouldn't mask isMcSource */
            ipv4PrefixMask.isMcSource = GT_TRUE;
            ipv4PrefixMask.mcGroupIndexRow = 0x1fff;
            if (PRV_CPSS_PP_MAC(ipShareDevs[0])->devFamily == CPSS_PP_FAMILY_CHEETAH2_E)
                ipv4PrefixMask.mcGroupIndexColumn = 0x7;
            else
                ipv4PrefixMask.mcGroupIndexColumn = 0x3;

            retVal =  cpssDxChIpv4PrefixSet(ipShareDevs[i],
                                            (lastIpv4EntryIndex % tcamLines),
                                            (lastIpv4EntryIndex / tcamLines),
                                            &ipv4Prefix,&ipv4PrefixMask);
        }

        if (ipTcamShadowPtr->isIpVerIsInitialized[CPSS_IP_PROTOCOL_IPV6_E])
        {
            ipv6Prefix.vrId = 0;
            for (j = 0 ; j < 4 ; j++)
                ipv6Prefix.ipAddr.u32Ip[j] = ipv6Addr.u32Ip[j];
            ipv6Prefix.isMcSource = GT_FALSE;
            ipv6Prefix.mcGroupIndexRow = 0;

            ipv6PrefixMask.vrId = 0;    /* mask vrId */
            for (j = 0 ; j < 4 ; j++)
                ipv6PrefixMask.ipAddr.u32Ip[j] = ipv6Mask.u32Ip[j];
            /* you shouldn't mask isMcSource */
            ipv6PrefixMask.isMcSource = GT_TRUE;
            ipv6PrefixMask.mcGroupIndexRow = 0xfff;

            retVal =  cpssDxChIpv6PrefixSet(ipShareDevs[i], lastIpv6EntryIndex,
                                            &ipv6Prefix,&ipv6PrefixMask);
        }
    }

    return retVal;
}

/*******************************************************************************
* prvCpssDxChLpmTcamAddDevToShadow
*
* DESCRIPTION:
*       add the device to the ip shadow
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Lion2.
*
* NOT APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devListPtr  - The device list to add.
*       ipTcamShadowPtr - the ip shadow we are working on.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success,
*       GT_FAIL                  - otherwise.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS prvCpssDxChLpmTcamAddDevToShadow
(
    IN  PRV_CPSS_DXCH_LPM_SHADOW_DEVS_LIST_STC *devListPtr,
    IN  PRV_CPSS_DXCH_LPM_TCAM_SHADOW_STC      *ipTcamShadowPtr
)
{
    GT_U8               idxInList;      /* Index of the device in the shared*/
                                        /* devices list.                    */
    GT_U8               devNum;
    GT_U32              i;
    GT_STATUS           retVal;
    PRV_CPSS_DXCH_PP_CONFIG_FINE_TUNING_STC *fineTuningPtr;
    CPSS_PP_FAMILY_TYPE_ENT                 devFamily=0;
    PRV_CPSS_DXCH_MODULE_CONFIG_STC         *moduleCfgPtr;/* pointer to the module
                                                configure of the PP's database*/

    if (!ipTcamShadowPtr)
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_FAIL, LOG_ERROR_NO_MSG);

    if(ipTcamShadowPtr->shadowDevList.ipShareDevNum == PRV_CPSS_MAX_PP_DEVICES_CNS)
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_OUT_OF_RANGE, LOG_ERROR_NO_MSG);

    /* first make sure all devices can fit into the lpm DB shadow */
    for (i = 0 ; i < devListPtr->ipShareDevNum ; i++)
    {
        devNum = devListPtr->ipShareDevs[i];
        if (!PRV_CPSS_IS_DEV_EXISTS_MAC(devNum))
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
        fineTuningPtr = &PRV_CPSS_DXCH_PP_MAC(devNum)->fineTuning;
        devFamily = PRV_CPSS_PP_MAC(devNum)->devFamily;

        moduleCfgPtr = &(PRV_CPSS_DXCH_PP_MAC(devNum)->moduleCfg);

        switch (ipTcamShadowPtr->shadowType)
        {
        case PRV_CPSS_DXCH_LPM_TCAM_CHEETAH_SHADOW_E:
            if((devFamily != CPSS_PP_FAMILY_CHEETAH_E) ||
               (ipTcamShadowPtr->tcamIndexRange.lastIndex > fineTuningPtr->tableSize.policyTcamRaws))
                CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
            break;
        case PRV_CPSS_DXCH_LPM_TCAM_XCAT_POLICY_BASED_ROUTING_SHADOW_E:
            if((moduleCfgPtr->ip.routingMode != CPSS_DXCH_POLICY_BASED_ROUTING_ONLY_E)||
               ((devFamily != CPSS_PP_FAMILY_DXCH_XCAT_E) &&
                (devFamily != CPSS_PP_FAMILY_DXCH_XCAT3_E) &&
                (devFamily != CPSS_PP_FAMILY_DXCH_XCAT2_E))||
               (ipTcamShadowPtr->tcamIndexRange.lastIndex > fineTuningPtr->tableSize.policyTcamRaws))
                CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
            break;
        case PRV_CPSS_DXCH_LPM_TCAM_CHEETAH2_SHADOW_E:
            if((devFamily != CPSS_PP_FAMILY_CHEETAH2_E)||
               (ipTcamShadowPtr->tcamIndexRange.lastIndex > fineTuningPtr->tableSize.router))
                CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
            break;
        case PRV_CPSS_DXCH_LPM_TCAM_CHEETAH3_SHADOW_E:
            if((devFamily != CPSS_PP_FAMILY_CHEETAH3_E)||
               (ipTcamShadowPtr->tcamIndexRange.lastIndex > fineTuningPtr->tableSize.router))
                CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
            break;
        case PRV_CPSS_DXCH_LPM_TCAM_XCAT_SHADOW_E:
            if((moduleCfgPtr->ip.routingMode != CPSS_DXCH_TCAM_ROUTER_BASED_E)||
               ((devFamily != CPSS_PP_FAMILY_DXCH_XCAT_E) && (devFamily != CPSS_PP_FAMILY_DXCH_XCAT3_E) && (devFamily != CPSS_PP_FAMILY_DXCH_LION_E) &&
                (devFamily != CPSS_PP_FAMILY_DXCH_LION2_E) && (!PRV_CPSS_SIP_5_CHECK_MAC(devNum)) )||
               (ipTcamShadowPtr->tcamIndexRange.lastIndex > fineTuningPtr->tableSize.router))
                CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
            break;
        default:
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
        }
    }

    /* now add the devices */
    for (i = 0; i < devListPtr->ipShareDevNum ; i++)
    {
        devNum = devListPtr->ipShareDevs[i];

        /* first check that the device wasn't added already */
        for (idxInList = 0;
            ((GT_U32)idxInList < ipTcamShadowPtr->shadowDevList.ipShareDevNum) &&
            (ipTcamShadowPtr->shadowDevList.ipShareDevs[idxInList] != devNum);
            idxInList++);

        /* if it wasn't added - add it */
        if ((GT_U32)idxInList == ipTcamShadowPtr->shadowDevList.ipShareDevNum)
        {
            /* increase the number of devices in the unit's Ip shadow data */
            ipTcamShadowPtr->shadowDevList.ipShareDevNum++;

            /* insert this added device to the last of the shared devices in the */
            /* shadow data */
            ipTcamShadowPtr->shadowDevList.ipShareDevs[idxInList] = devNum;
        }
        else
        {
             /* the device exists, can't re-add it */
             CPSS_LOG_ERROR_AND_RETURN_MAC(GT_ALREADY_EXIST, LOG_ERROR_NO_MSG);
        }
    }

    /* in Ch2 with Vr support we need to set some default in Router Tcam   */
    /* for each protocol in use because of uc SIP lookup that is performed */
    /* and Vr0 wasnt created yet                                           */
    if ((ipTcamShadowPtr->shadowType == PRV_CPSS_DXCH_LPM_TCAM_CHEETAH2_SHADOW_E)&&
        (ipTcamShadowPtr->multipleVirtualRouterSupport == GT_TRUE))
    {
        retVal = ipTcamSetDefaultPrefixCheetah2(devListPtr, ipTcamShadowPtr);
        if (retVal != GT_OK)
            return(retVal);
    }

    /* switch the working dev list to the added one */
    ipTcamShadowPtr->workDevListPtr = devListPtr;

    /* if the LPM DB is set to use PCL default MC rules,
       set them now only on the new added devices */
    if (ipTcamShadowPtr->usePbrDefaultMc == GT_TRUE)
    {
        retVal = prvCpssDxChLpmTcamPbrDefaultMcRuleSet(ipTcamShadowPtr,
                                                       ipTcamShadowPtr->workDevListPtr);
        if (retVal != GT_OK)
            return retVal;
    }

    /* now write the current Virtual routers */
    retVal = prvCpssDxChLpmTcamUpdateHw(ipTcamShadowPtr,
                                        PRV_CPSS_DXCH_LPM_TCAM_OVERWRITE_UC_MC_E,
                                        CPSS_IP_PROTOCOL_IPV4V6_E);

    /* switch back the devlist */
    ipTcamShadowPtr->workDevListPtr = &ipTcamShadowPtr->shadowDevList;


    return retVal;
}


/*******************************************************************************
* ipTcamRegisterIpUcFuncs
*
* DESCRIPTION:
*       register the ip functions per shadow type for the requested ip protocol
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* NOT APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       shadowType      - the type of shadow we're building
*       protocolStack   - types of IP stack used in the future virtual routers.
*       ipTcamShadowPtr - the ip shadow we are working on.
*
* OUTPUTS:
*       None.
* RETURNS:
*       GT_OK               - on success
*       GT_FAIL             - on error
*       GT_OUT_OF_CPU_MEM   - on malloc failed
*
* COMMENTS:
*       None
*
*******************************************************************************/
static GT_STATUS ipTcamRegisterIpUcFuncs
(
    IN PRV_CPSS_DXCH_LPM_SHADOW_TYPE_ENT        shadowType,
    IN CPSS_IP_PROTOCOL_STACK_ENT               protocolStack,
    IN PRV_CPSS_DXCH_LPM_TCAM_SHADOW_STC        *ipTcamShadowPtr
)
{
    PRV_CPSS_DXCH_LPM_TCAM_PAT_TRIE_FUNCS_OBJ_STC *ipFuncs;

    ipFuncs = ipTcamShadowPtr->ipUcTcamFuncsPtrs;
    switch (shadowType)
    {
        case PRV_CPSS_DXCH_LPM_TCAM_CHEETAH_SHADOW_E:
        case PRV_CPSS_DXCH_LPM_TCAM_XCAT_POLICY_BASED_ROUTING_SHADOW_E:
            /* cheetah+ and xCat policy based routing funcs */
            ipFuncs[protocolStack].ipTcamDelPrefix = ipTcamTrieDelPrefixCheetah;
            ipFuncs[protocolStack].ipTcamMovePrefix =
                (protocolStack == CPSS_IP_PROTOCOL_IPV4_E) ?
                ipTcamTrieMoveIpv4PrefixCheetah : ipTcamTrieMoveIpv6PrefixCheetah;
            ipFuncs[protocolStack].ipTcamUpdateDataIdx = ipTcamTrieUpdateDataCheeta;
            ipFuncs[protocolStack].ipTcamSetPrefix =
                (protocolStack == CPSS_IP_PROTOCOL_IPV4_E) ?
                ipTcamTrieSetPrefixIpv4Cheetah:ipTcamTrieSetPrefixIpv6Cheetah;
            break;

    case PRV_CPSS_DXCH_LPM_TCAM_CHEETAH2_SHADOW_E:
    case PRV_CPSS_DXCH_LPM_TCAM_CHEETAH3_SHADOW_E:
    case PRV_CPSS_DXCH_LPM_TCAM_XCAT_SHADOW_E:

            /* cheetah2 funcs */
            ipFuncs[protocolStack].ipTcamDelPrefix =
                (protocolStack == CPSS_IP_PROTOCOL_IPV4_E) ?
                prvCpssDxChLpmTcamTrieDelIpv4PrefixCheetah2 : prvCpssDxChLpmTcamTrieDelIpv6PrefixCheetah2;

            ipFuncs[protocolStack].ipTcamMovePrefix =
                (protocolStack == CPSS_IP_PROTOCOL_IPV4_E) ?
                ipTcamTrieMoveIpv4PrefixCheetah2 : ipTcamTrieMoveIpv6PrefixCheetah2;

            ipFuncs[protocolStack].ipTcamUpdateDataIdx = ipTcamTrieUpdateDataCheeta2;

            ipFuncs[protocolStack].ipTcamSetPrefix =
                (protocolStack == CPSS_IP_PROTOCOL_IPV4_E) ?
                ipTcamTrieSetPrefixIpv4Cheetah2 : ipTcamTrieSetPrefixIpv6Cheetah2;
            break;
        default:
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_FAIL, LOG_ERROR_NO_MSG);
    }

    /* set the working list */
    ipFuncs[protocolStack].workDevListPtr = &ipTcamShadowPtr->workDevListPtr;

    return GT_OK;
}

/*******************************************************************************
* prvCpssDxChLpmTcamInit
*
* DESCRIPTION:
*       Init the Ip unit
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* NOT APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       shadowType              - the type of shadow we're building (cheetah+ or
*                                 cheetah2)
*       rangeIndexsPtr          - the range of TCAM indexes availble for this
*                                 LPM DB (see explanation in
*                                 CPSS_DXCH_IP_TCAM_LPM_MANGER_INDEX_RANGE_STC)
*       partitionEnable         - whether to enable partitioning of the router
*                                 TCAM according to the
*                                 tcamLpmManagercapacityCfgPtr.
*       capacityCfgPtr          - The capcity configuration struct.
*       protocolStack           - the protocol stack to support
*       tcamManagerHandlerPtr   - the TCAM manager handler
*
* OUTPUTS:
*       ipTcamShadowPtrPtr - the ip shadow we created.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_OUT_OF_CPU_MEM        - on malloc failed
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS prvCpssDxChLpmTcamInit
(
    IN  PRV_CPSS_DXCH_LPM_SHADOW_TYPE_ENT                shadowType,
    IN  PRV_CPSS_DXCH_LPM_TCAM_MANGER_INDEX_RANGE_STC    *rangeIndexsPtr,
    IN  GT_BOOL                                          partitionEnable,
    IN  PRV_CPSS_DXCH_LPM_TCAM_MANGER_CAPCITY_CFG_STC    *capacityCfgPtr,
    IN  CPSS_IP_PROTOCOL_STACK_ENT                       protocolStack,
    IN  GT_VOID                                          *tcamManagerHandlerPtr,
    OUT PRV_CPSS_DXCH_LPM_TCAM_SHADOW_STC                **ipTcamShadowPtrPtr
)
{
    GT_STATUS          retVal;
    CPSS_IP_PROTOCOL_STACK_ENT prot;

    /* create the shadow */
    retVal = createIpTcamShadowStruct(shadowType,rangeIndexsPtr,
                                      partitionEnable,capacityCfgPtr,
                                      ipTcamShadowPtrPtr,protocolStack,
                                      tcamManagerHandlerPtr);

    if (retVal != GT_OK)
        return retVal;

    /* if the ip shadow wasn't found/initialized till now return Fail! */
    if (*ipTcamShadowPtrPtr == NULL)
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PTR, LOG_ERROR_NO_MSG);

    retVal = GT_OK;

    for (prot = CPSS_IP_PROTOCOL_IPV4_E; prot < CPSS_IP_PROTOCOL_IPV4V6_E; prot++)
    {
        if ((prot != protocolStack) &&
            (protocolStack != CPSS_IP_PROTOCOL_IPV4V6_E))
            continue;
        /* register ip protocol functions */
        ipTcamRegisterIpUcFuncs(shadowType,prot,*ipTcamShadowPtrPtr);

        retVal = prvCpssDxChLpmTcamRegisterIpMcFuncs(shadowType,prot,*ipTcamShadowPtrPtr);
        if(retVal != GT_OK)
            return retVal;

        /* indicate the ip protocol is initialized. */
        (*ipTcamShadowPtrPtr)->isIpVerIsInitialized[prot] = GT_TRUE;
    }

    return retVal;

}

/*******************************************************************************
* prvCpssDxChLpmTcamCheetah2VrSupportInit
*
* DESCRIPTION:
*       Init the Ip unit for Cheetah2 devices with virtual router support.
*
* APPLICABLE DEVICES:
*        DxCh2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       protocolStack    - the type of protocol stack this LPM DB support.
*       pclTcamCfgPtr    - the pcl tcam configuration
*       routerTcamCfgPtr - the router tcam configuration
*
* OUTPUTS:
*       ipTcamShadowPtrPtr  - the ip shadow we created.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_OUT_OF_CPU_MEM        - on malloc failed
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS prvCpssDxChLpmTcamCheetah2VrSupportInit
(
    IN  CPSS_IP_PROTOCOL_STACK_ENT                              protocolStack,
    IN CPSS_DXCH_IP_TCAM_LPM_MANGER_CHEETAH2_VR_SUPPORT_CFG_STC *pclTcamCfgPtr,
    IN CPSS_DXCH_IP_TCAM_LPM_MANGER_CHEETAH2_VR_SUPPORT_CFG_STC *routerTcamCfgPtr,
    OUT PRV_CPSS_DXCH_LPM_TCAM_SHADOW_STC                       **ipTcamShadowPtrPtr
)
{
    GT_STATUS                   retVal = GT_OK;
    CPSS_IP_PROTOCOL_STACK_ENT  prot;

    /* create the shadow */
    GT_U32      maxNumOfVr;
    GT_U8       numOfDevs;                        /* Number of devices in system.         */
    PRV_CPSS_DXCH_LPM_TCAM_SHADOW_STC *retShadow; /* A pointer to the shadow struct to be */
                                                  /* returned.                            */
    GT_U8       i,j;

    /* parameters checking */
    if (routerTcamCfgPtr->tcamRange.lastIndex -
        routerTcamCfgPtr->tcamRange.firstIndex + 1 >
        PRV_CPSS_DXCH_LPM_TCAM_CHEETAH2_NUM_OF_RULES_IN_COLUMN_CNS)
    {
        /* too few router tcam lines allocated */
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    /* parameters checking - available LTT indexes */
    if (routerTcamCfgPtr->prefixesCfg.numOfIpv4Prefixes +
        routerTcamCfgPtr->prefixesCfg.numOfIpv6Prefixes +
        pclTcamCfgPtr->prefixesCfg.numOfIpv4Prefixes +
        pclTcamCfgPtr->prefixesCfg.numOfIpv6Prefixes >
        (routerTcamCfgPtr->tcamRange.lastIndex -
        routerTcamCfgPtr->tcamRange.firstIndex + 1) * 5)
    {
        /* too many prefixes requested */
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    /* parameters checking */
    if (pclTcamCfgPtr->tcamRange.lastIndex -
        pclTcamCfgPtr->tcamRange.firstIndex + 1 >
        PRV_CPSS_DXCH_LPM_TCAM_CHEETAH_NUM_OF_RULES_IN_COLUMN_CNS)
    {
        /* too many pcl tcam entries allocated */
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    /*
        For Ch2 with Vr support - since uc SIP lookup in Router Tcam always performed
        all prefix types (ipv4/6) must have defaults in this tcam.
        So we require that Vr0 will have the same protocol type as LpmDb and at
        least one prefix (default) in each reqired type
     */
    if (((CPSS_IP_PROTOCOL_IPV4_E   == protocolStack) ||
         (CPSS_IP_PROTOCOL_IPV4V6_E == protocolStack)) &&
        (routerTcamCfgPtr->prefixesCfg.numOfIpv4Prefixes == 0))
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);

    if (((CPSS_IP_PROTOCOL_IPV6_E   == protocolStack) ||
         (CPSS_IP_PROTOCOL_IPV4V6_E == protocolStack)) &&
        (routerTcamCfgPtr->prefixesCfg.numOfIpv6Prefixes == 0))
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);

    retShadow = NULL;

    numOfDevs = PRV_CPSS_MAX_PP_DEVICES_CNS;

    retShadow = (PRV_CPSS_DXCH_LPM_TCAM_SHADOW_STC*)
                        cpssOsMalloc(sizeof(PRV_CPSS_DXCH_LPM_TCAM_SHADOW_STC));
    if (retShadow == NULL)
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_FAIL, LOG_ERROR_NO_MSG);

    cpssOsMemSet(retShadow,0,sizeof(PRV_CPSS_DXCH_LPM_TCAM_SHADOW_STC));

    *ipTcamShadowPtrPtr      = retShadow;
    if (*ipTcamShadowPtrPtr == NULL)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_FAIL, LOG_ERROR_NO_MSG);
    }

    retShadow->shadowType = PRV_CPSS_DXCH_LPM_TCAM_CHEETAH2_SHADOW_E;
    tcamRows = PRV_CPSS_DXCH_LPM_TCAM_CHEETAH2_NUM_OF_RULES_IN_COLUMN_CNS;
    retShadow->shadowDevList.ipShareDevNum = 0;
    retShadow->multipleVirtualRouterSupport = GT_TRUE;

    /* since now we gonna work on the whole list, set the pointer right*/
    retShadow->workDevListPtr = &retShadow->shadowDevList;
    retShadow->isIpVerIsInitialized[0] = GT_FALSE;
    retShadow->isIpVerIsInitialized[1] = GT_FALSE;


    retShadow->numOfIpPrefixes[CPSS_IP_PROTOCOL_IPV4_E] =
                                pclTcamCfgPtr->prefixesCfg.numOfIpv4Prefixes +
                                routerTcamCfgPtr->prefixesCfg.numOfIpv4Prefixes;
    retShadow->numOfIpPrefixes[CPSS_IP_PROTOCOL_IPV6_E] =
                                pclTcamCfgPtr->prefixesCfg.numOfIpv6Prefixes +
                                routerTcamCfgPtr->prefixesCfg.numOfIpv6Prefixes;
    retShadow->numOfIpMcSrcAddr = 0;

    /* Allocate the device list                                 */
    if ((retShadow->shadowDevList.ipShareDevs =
                        (GT_U8*)cpssOsMalloc(sizeof(GT_U8) * numOfDevs)) == NULL)
    {
        cpssOsFree(retShadow);
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_FAIL, LOG_ERROR_NO_MSG);
    }

    /* allocate the virtual routers skip list*/
    maxNumOfVr = CPSS_DXCH2_MULTIPLE_VRS_MAX_VR_ID_CNS;

    retShadow->vrSl = prvCpssSlInit(ipTcamVrCompFunc, maxNumOfVr);
    if (retShadow->vrSl == NULL)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_OUT_OF_CPU_MEM, LOG_ERROR_NO_MSG);
    }


    /* allocate memory for TCAM entries handler */
    retShadow ->tcamEntriesHandler = (PRV_CPSS_DXCH_LPM_TCAM_ENTRIES_HANDLER_STC*)
            cpssOsMalloc(sizeof(PRV_CPSS_DXCH_LPM_TCAM_ENTRIES_HANDLER_STC));

    if (retShadow ->tcamEntriesHandler == NULL)
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_OUT_OF_CPU_MEM, LOG_ERROR_NO_MSG);

    retShadow ->tcamEntriesHandler->tcamShadowType = retShadow ->shadowType;


    for (i=0; i<2; i++)
    {
        for (j=0; j<2; j++)
        {
            (*ipTcamShadowPtrPtr)->tcamEntriesHandler->ipTcamPool.tcamMgm[i][j] = NULL;
        }
    }

    /* now create the memory pools.*/
    retVal = prvCpssDxChLpmTcamMemPoolCheetah2PolicyBasedRoutingInit(pclTcamCfgPtr,
                                                                     routerTcamCfgPtr,
                                                                     protocolStack,
                                                                     (*ipTcamShadowPtrPtr));

    /* if the ip shadow wasn't found/initialized till now return Fail! */
    if ((retVal != GT_OK) || (*ipTcamShadowPtrPtr == NULL))
    {
#ifdef OS_MALLOC_MEMORY_LEAKAGE_DBG
        retVal = prvCpssSlDestroy(retShadow->vrSl, myCpssOsFree);
#else /*! OS_MALLOC_MEMORY_LEAKAGE_DBG */
        retVal = prvCpssSlDestroy(retShadow->vrSl, cpssOsFree);
#endif /*! OS_MALLOC_MEMORY_LEAKAGE_DBG */
        if(retVal != GT_OK)
            return retVal;
        cpssOsFree(retShadow->shadowDevList.ipShareDevs);
        cpssOsFree(retShadow);

        if (*ipTcamShadowPtrPtr == NULL)
        {
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PTR, LOG_ERROR_NO_MSG);
        }

        return retVal;
    }

    retVal = GT_OK;

    for (prot = CPSS_IP_PROTOCOL_IPV4_E; prot < CPSS_IP_PROTOCOL_IPV4V6_E; prot++)
    {
        if ((prot != protocolStack) &&
            (protocolStack != CPSS_IP_PROTOCOL_IPV4V6_E))
            continue;

        /* register ip protocol functions */
        ipTcamRegisterIpUcFuncs(PRV_CPSS_DXCH_LPM_TCAM_CHEETAH2_SHADOW_E,
                                prot,
                                *ipTcamShadowPtrPtr);

        retVal = prvCpssDxChLpmTcamRegisterIpMcFuncs(PRV_CPSS_DXCH_LPM_TCAM_CHEETAH2_SHADOW_E,
                                                     prot,
                                                     *ipTcamShadowPtrPtr);
        if(retVal != GT_OK)
            return retVal;

        /* indicate the ip protocol is initialized. */
        (*ipTcamShadowPtrPtr)->isIpVerIsInitialized[prot] = GT_TRUE;
    }

    return retVal;

}


/*******************************************************************************
* prvCpssDxChLpmTcamAddVirtualRouter
*
* DESCRIPTION:
*       This function adds a new virtual router to the valid virtual routers
*       in system for specific shared shadow devices.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* NOT APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       vrId                        - The virtual's router ID (in cheetah+ this
*                                     will be the pcl id).
*       defIpv4UcNextHopInfoPtr     - The next hop info representing the default IPv4-Uc
*                                     route entry.
*       defIpv6UcNextHopInfoPtr     - The next hop info representing the default IPv6-Uc
*                                     route entry.

*       defUcNextHopRuleIdxArr      - if not NULL, then this is the internal rule idx
*                                     of the defualt NextHop prefix. (array of 2
*                                     for each of the protocol Stacks)
*       defIpv4McRouteLttEntryPtr   - The default IPv4_Mc LTT entry
*       defIpv6McRouteLttEntryPtr   - The default IPv6_Mc LTT entry
*       defMcRouteRuleIdxArr        - if not null then this is the internal Rule idx
*                                     of the default mc route (prefix) (array of 2
*                                     for each of the protocol Stacks)
*       protocolStack               - types of IP stack used in this virtual router.
*       ipTcamShadowPtr             - the ip shadow we are working on.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - if success
*       GT_NOT_FOUND             - if the given next hop Id is not valid.
*       GT_ALREADY_EXIST         - if the virtual router already exists
*       GT_OUT_OF_CPU_MEM        - if failed to allocate CPU memory, or
*       GT_OUT_OF_PP_MEM         - if failed to allocate PP memory.
*       GT_NOT_APPLICABLE_DEVICE - if not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS prvCpssDxChLpmTcamAddVirtualRouter
(
    IN GT_U32                                       vrId,
    IN PRV_CPSS_DXCH_LPM_ROUTE_ENTRY_INFO_UNT       *defIpv4UcNextHopInfoPtr,
    IN PRV_CPSS_DXCH_LPM_ROUTE_ENTRY_INFO_UNT       *defIpv6UcNextHopInfoPtr,
    IN GT_U32                                       defUcNextHopRuleIdxArr[],
    IN PRV_CPSS_DXCH_LPM_ROUTE_ENTRY_POINTER_STC    *defIpv4McRouteLttEntryPtr,
    IN PRV_CPSS_DXCH_LPM_ROUTE_ENTRY_POINTER_STC    *defIpv6McRouteLttEntryPtr,
    IN GT_U32                                       defMcRouteRuleIdxArr[],
    IN CPSS_IP_PROTOCOL_STACK_ENT                   protocolStack,
    IN PRV_CPSS_DXCH_LPM_TCAM_SHADOW_STC            *ipTcamShadowPtr
)
{
    /* Next hop info associated with defaults*/

    GT_PTR  ipNextHopEntry[CPSS_IP_PROTOCOL_IPV4V6_E] = {NULL,NULL};

    PRV_CPSS_DXCH_LPM_ROUTE_ENTRY_POINTER_STC *defMcRoutePtr[CPSS_IP_PROTOCOL_IPV4V6_E] = {NULL,NULL};


    GT_U8                    zeroIpAddr[16] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
    PRV_CPSS_DXCH_LPM_TCAM_PAT_TRIE_NODE_STC    *patTrie;
    GT_STATUS                                   retVal = GT_OK;
    GT_U8                                       i;
    GT_UINTPTR                                  iter;
    PRV_CPSS_DXCH_LPM_TCAM_VR_TBL_BLOCK_STC     vrEntryTmp;
    PRV_CPSS_DXCH_LPM_TCAM_VR_TBL_BLOCK_STC     *vrEntryPtr;
    PRV_CPSS_DXCH_LPM_TCAM_MC_GROUP_SRC_NODE_STC *mcGroupNode[CPSS_IP_PROTOCOL_IPV4V6_E] = {NULL,NULL};
    PRV_CPSS_DXCH_LPM_TCAM_RULE_NODE_STC        *ruleNode = NULL;
    GT_PTR                                      tcamRulePtr = NULL;
    PRV_CPSS_DXCH_LPM_TCAM_ENTRIES_HANDLER_ENTRY_TYPE_ENT  entryType;
    GT_U8                                       devNum = 0;
    GT_U32                                      ipv4PrefixNumber = 0;
    GT_U32                                      ipv6PrefixNumber = 0;
    CPSS_IP_UNICAST_MULTICAST_ENT               ipMode = CPSS_IP_UNICAST_E;
    PRV_CPSS_DXCH_LPM_TCAM_IP_MC_ENTRY_TYPE_ENT mcEntryType = PRV_CPSS_DXCH_LPM_TCAM_IP_MC_GROUP_E;

    switch(protocolStack)
    {
        case CPSS_IP_PROTOCOL_IPV4_E:
        case CPSS_IP_PROTOCOL_IPV6_E:
        case CPSS_IP_PROTOCOL_IPV4V6_E:
            break;
        default:
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    /* verify the LPM DB was initialized for the requested protocol stack */
    if (((protocolStack == CPSS_IP_PROTOCOL_IPV4V6_E)               &&
        ((ipTcamShadowPtr->isIpVerIsInitialized[0] == GT_FALSE)     ||
         (ipTcamShadowPtr->isIpVerIsInitialized[1] == GT_FALSE)))   ||
        ((protocolStack != CPSS_IP_PROTOCOL_IPV4V6_E)               &&
         (ipTcamShadowPtr->isIpVerIsInitialized[protocolStack] == GT_FALSE)))
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_INITIALIZED, LOG_ERROR_NO_MSG);

    /* check if the VR exist */
    vrEntryTmp.vrId = vrId;
    vrEntryPtr = prvCpssSlSearch(ipTcamShadowPtr->vrSl,&vrEntryTmp);
    if (vrEntryPtr != NULL)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_ALREADY_EXIST, LOG_ERROR_NO_MSG);
    }

    if (defIpv4UcNextHopInfoPtr != NULL)
    {
        ipv4PrefixNumber++;
    }

    /*MC should not be counter when we are in Policy based routing tcam where
      MC is not supported*/
    if ((ipTcamShadowPtr->shadowType == PRV_CPSS_DXCH_LPM_TCAM_CHEETAH2_SHADOW_E) ||
        (ipTcamShadowPtr->shadowType == PRV_CPSS_DXCH_LPM_TCAM_CHEETAH3_SHADOW_E) ||
        (ipTcamShadowPtr->shadowType == PRV_CPSS_DXCH_LPM_TCAM_XCAT_SHADOW_E))
    {
        if (defIpv4McRouteLttEntryPtr != NULL)
        {
            ipv4PrefixNumber++;

        }
    }
    /* check tcam prefix counter for free space */
    /* uc ipv4 default and mc ipv4 default are counted by the same counter so it could be
       checked together */
    retVal = prvCpssDxChLpmTcamFreeSpaceCountersCheck(ipv4PrefixNumber,ipMode,mcEntryType,
                                                      CPSS_IP_PROTOCOL_IPV4_E,ipTcamShadowPtr);
    if (retVal != GT_OK)
    {
        return retVal;
    }
    /* to perform correct check of TCAM free place for ipv6 defaults in "no partition"
       mode increment ipv4 counter on ipv4PrefixNumber. After TCAM free place estimation
       for ipv6 defaults decrement this counter back */

    ipTcamShadowPtr->allocatedIpPrefixCounters.numOfAllocatedIpPrefixes[CPSS_IP_PROTOCOL_IPV4_E]+=
        ipv4PrefixNumber;
    /* check Ipv6 defaults*/
    if (defIpv6UcNextHopInfoPtr != NULL)
    {
        ipv6PrefixNumber++;
    }

    /*MC should not be counter when we are in Policy based routing tcam where
      MC is not supported*/
    if ((ipTcamShadowPtr->shadowType == PRV_CPSS_DXCH_LPM_TCAM_CHEETAH2_SHADOW_E) ||
        (ipTcamShadowPtr->shadowType == PRV_CPSS_DXCH_LPM_TCAM_CHEETAH3_SHADOW_E) ||
        (ipTcamShadowPtr->shadowType == PRV_CPSS_DXCH_LPM_TCAM_XCAT_SHADOW_E))
    {
        if (defIpv6McRouteLttEntryPtr != NULL)
        {
            ipv6PrefixNumber++;
        }
    }
    /* check tcam prefix counter for free space */
    retVal = prvCpssDxChLpmTcamFreeSpaceCountersCheck(ipv6PrefixNumber,ipMode,mcEntryType,
                                                      CPSS_IP_PROTOCOL_IPV6_E,ipTcamShadowPtr);
    if (retVal != GT_OK)
    {
        /* decrement ipv4 counter back */
        ipTcamShadowPtr->allocatedIpPrefixCounters.numOfAllocatedIpPrefixes[CPSS_IP_PROTOCOL_IPV4_E]-=
            ipv4PrefixNumber;
        return retVal;
    }
    /* decrement ipv4 counter back */
    ipTcamShadowPtr->allocatedIpPrefixCounters.numOfAllocatedIpPrefixes[CPSS_IP_PROTOCOL_IPV4_E]-=
        ipv4PrefixNumber;

    /* ok create a new one.*/
    vrEntryPtr = cpssOsMalloc(sizeof(PRV_CPSS_DXCH_LPM_TCAM_VR_TBL_BLOCK_STC));
    if (vrEntryPtr == NULL)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_OUT_OF_CPU_MEM, LOG_ERROR_NO_MSG);
    }
    cpssOsMemSet(vrEntryPtr,0,sizeof(PRV_CPSS_DXCH_LPM_TCAM_VR_TBL_BLOCK_STC));
    vrEntryPtr->vrId = vrId;
    vrEntryPtr->tcamEntriesHandler = ipTcamShadowPtr->tcamEntriesHandler;
    for (i = 0 ; i < 2 ; i++)
    {
        vrEntryPtr->ipUcTrie[i] = NULL;
        vrEntryPtr->ipMcTrie[i] = NULL;
    }

    /* choose which kind of next hop info we got and allocate it. */
    /* first allocate the next hop info relevant for this shadow */
    switch (ipTcamShadowPtr->shadowType)
    {
    case PRV_CPSS_DXCH_LPM_TCAM_CHEETAH_SHADOW_E:
    case PRV_CPSS_DXCH_LPM_TCAM_XCAT_POLICY_BASED_ROUTING_SHADOW_E:
        /* for policy based shadow types, defUcNextHopInfoPtr is tested to be
           not NULL in the API functions */
        if (defIpv4UcNextHopInfoPtr != NULL)
        {
            ipNextHopEntry[CPSS_IP_PROTOCOL_IPV4_E] = cpssOsMalloc(sizeof (CPSS_DXCH_PCL_ACTION_STC));
            if (ipNextHopEntry[CPSS_IP_PROTOCOL_IPV4_E] == NULL)
                CPSS_LOG_ERROR_AND_RETURN_MAC(GT_OUT_OF_CPU_MEM, LOG_ERROR_NO_MSG);
            cpssOsMemCpy(ipNextHopEntry[CPSS_IP_PROTOCOL_IPV4_E],
                         &defIpv4UcNextHopInfoPtr->pclIpUcAction,
                         sizeof(CPSS_DXCH_PCL_ACTION_STC));
        }
        if (defIpv6UcNextHopInfoPtr != NULL)
        {
            ipNextHopEntry[CPSS_IP_PROTOCOL_IPV6_E] = cpssOsMalloc(sizeof (CPSS_DXCH_PCL_ACTION_STC));
            if (ipNextHopEntry[CPSS_IP_PROTOCOL_IPV6_E] == NULL)
                CPSS_LOG_ERROR_AND_RETURN_MAC(GT_OUT_OF_CPU_MEM, LOG_ERROR_NO_MSG);
            cpssOsMemCpy(ipNextHopEntry[CPSS_IP_PROTOCOL_IPV6_E],
                         &defIpv6UcNextHopInfoPtr->pclIpUcAction,
                         sizeof(CPSS_DXCH_PCL_ACTION_STC));
        }

        break;

    case PRV_CPSS_DXCH_LPM_TCAM_CHEETAH2_SHADOW_E:
    case PRV_CPSS_DXCH_LPM_TCAM_CHEETAH3_SHADOW_E:
    case PRV_CPSS_DXCH_LPM_TCAM_XCAT_SHADOW_E:
        if (defIpv4UcNextHopInfoPtr != NULL)
        {
            switch(defIpv4UcNextHopInfoPtr->routeEntry.ipv6McGroupScopeLevel)
            {
                case CPSS_IPV6_PREFIX_SCOPE_LINK_LOCAL_E:
                case CPSS_IPV6_PREFIX_SCOPE_SITE_LOCAL_E:
                case CPSS_IPV6_PREFIX_SCOPE_UNIQUE_LOCAL_E:
                case CPSS_IPV6_PREFIX_SCOPE_GLOBAL_E:
                    break;
                default:
                    CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
            }

            for(i=0; i < ipTcamShadowPtr->shadowDevList.ipShareDevNum; i++)
            {
                devNum = ipTcamShadowPtr->shadowDevList.ipShareDevs[i];

                if (0 == PRV_CPSS_DXCH_LION_FAMILY_CHECK_MAC(devNum))
                {
                    if (defIpv4UcNextHopInfoPtr->routeEntry.blockSize > CPSS_DXCH_IP_MAX_ECMP_QOS_GROUP)
                        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
                }
                else
                {
                    if(defIpv4UcNextHopInfoPtr->routeEntry.routeEntryMethod == PRV_CPSS_DXCH_LPM_ENTRY_TYPE_QOS_E)
                    {
                        if (defIpv4UcNextHopInfoPtr->routeEntry.blockSize > CPSS_DXCH_IP_MAX_LION_QOS_GROUP)
                            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
                    }
                    else
                    {
                        if (defIpv4UcNextHopInfoPtr->routeEntry.blockSize > CPSS_DXCH_IP_MAX_LION_ECMP_GROUP)
                            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
                    }

                }

                if (defIpv4UcNextHopInfoPtr->routeEntry.routeEntryBaseMemAddr >= PRV_CPSS_DXCH_PP_MAC(devNum)->fineTuning.tableSize.routerNextHop)
                    CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
            }

            ipNextHopEntry[CPSS_IP_PROTOCOL_IPV4_E] = cpssOsMalloc(sizeof (PRV_CPSS_DXCH_LPM_ROUTE_ENTRY_POINTER_STC));
            if (ipNextHopEntry[CPSS_IP_PROTOCOL_IPV4_E] == NULL)
                CPSS_LOG_ERROR_AND_RETURN_MAC(GT_OUT_OF_CPU_MEM, LOG_ERROR_NO_MSG);
            cpssOsMemCpy(ipNextHopEntry[CPSS_IP_PROTOCOL_IPV4_E],
                         &defIpv4UcNextHopInfoPtr->routeEntry,
                         sizeof(PRV_CPSS_DXCH_LPM_ROUTE_ENTRY_POINTER_STC));
        }
        if (defIpv6UcNextHopInfoPtr != NULL)
        {
            switch(defIpv6UcNextHopInfoPtr->routeEntry.ipv6McGroupScopeLevel)
            {
                case CPSS_IPV6_PREFIX_SCOPE_LINK_LOCAL_E:
                case CPSS_IPV6_PREFIX_SCOPE_SITE_LOCAL_E:
                case CPSS_IPV6_PREFIX_SCOPE_UNIQUE_LOCAL_E:
                case CPSS_IPV6_PREFIX_SCOPE_GLOBAL_E:
                    break;
                default:
                    CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
            }

            for(i=0; i < ipTcamShadowPtr->shadowDevList.ipShareDevNum; i++)
            {
                devNum = ipTcamShadowPtr->shadowDevList.ipShareDevs[i];

                if (0 == PRV_CPSS_DXCH_LION_FAMILY_CHECK_MAC(devNum))
                {
                    if (defIpv6UcNextHopInfoPtr->routeEntry.blockSize > CPSS_DXCH_IP_MAX_ECMP_QOS_GROUP)
                        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
                }
                else
                {
                    if(defIpv6UcNextHopInfoPtr->routeEntry.routeEntryMethod == PRV_CPSS_DXCH_LPM_ENTRY_TYPE_QOS_E)
                    {
                        if (defIpv6UcNextHopInfoPtr->routeEntry.blockSize > CPSS_DXCH_IP_MAX_LION_QOS_GROUP)
                            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
                    }
                    else
                    {
                        if (defIpv6UcNextHopInfoPtr->routeEntry.blockSize > CPSS_DXCH_IP_MAX_LION_ECMP_GROUP)
                            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
                    }

                }

                if (defIpv6UcNextHopInfoPtr->routeEntry.routeEntryBaseMemAddr >= PRV_CPSS_DXCH_PP_MAC(devNum)->fineTuning.tableSize.routerNextHop)
                    CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
            }

            ipNextHopEntry[CPSS_IP_PROTOCOL_IPV6_E] = cpssOsMalloc(sizeof (PRV_CPSS_DXCH_LPM_ROUTE_ENTRY_POINTER_STC));
            if (ipNextHopEntry[CPSS_IP_PROTOCOL_IPV6_E] == NULL)
                CPSS_LOG_ERROR_AND_RETURN_MAC(GT_OUT_OF_CPU_MEM, LOG_ERROR_NO_MSG);
            cpssOsMemCpy(ipNextHopEntry[CPSS_IP_PROTOCOL_IPV6_E],
                         &defIpv6UcNextHopInfoPtr->routeEntry,
                         sizeof(PRV_CPSS_DXCH_LPM_ROUTE_ENTRY_POINTER_STC));
        }

        /* allocate MC for:                                         */
        /* - Cheetah3, Cheetah2 or xCat (not Policy Based Routing) */
        if ((ipTcamShadowPtr->shadowType == PRV_CPSS_DXCH_LPM_TCAM_CHEETAH2_SHADOW_E) ||
            (ipTcamShadowPtr->shadowType == PRV_CPSS_DXCH_LPM_TCAM_CHEETAH3_SHADOW_E) ||
            (ipTcamShadowPtr->shadowType == PRV_CPSS_DXCH_LPM_TCAM_XCAT_SHADOW_E))
        {
            if (defIpv4McRouteLttEntryPtr != NULL)
            {
               for(i=0; i < ipTcamShadowPtr->shadowDevList.ipShareDevNum; i++)
                {
                    devNum = ipTcamShadowPtr->shadowDevList.ipShareDevs[i];

                    if (0 == PRV_CPSS_DXCH_LION_FAMILY_CHECK_MAC(devNum))
                    {
                        if (defIpv4McRouteLttEntryPtr->blockSize > CPSS_DXCH_IP_MAX_ECMP_QOS_GROUP)
                            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
                    }
                    else
                    {
                        if(defIpv4McRouteLttEntryPtr->routeEntryMethod == PRV_CPSS_DXCH_LPM_ENTRY_TYPE_QOS_E)
                        {
                            if (defIpv4McRouteLttEntryPtr->blockSize > CPSS_DXCH_IP_MAX_LION_QOS_GROUP)
                                CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
                        }
                        else
                        {
                            if (defIpv4McRouteLttEntryPtr->blockSize > CPSS_DXCH_IP_MAX_LION_ECMP_GROUP)
                                CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
                        }

                    }

                    if (defIpv4McRouteLttEntryPtr->routeEntryBaseMemAddr >= PRV_CPSS_DXCH_PP_MAC(devNum)->fineTuning.tableSize.routerNextHop)
                        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
                }

                defMcRoutePtr[CPSS_IP_PROTOCOL_IPV4_E] = cpssOsMalloc(sizeof (PRV_CPSS_DXCH_LPM_ROUTE_ENTRY_POINTER_STC));
                if (defMcRoutePtr[CPSS_IP_PROTOCOL_IPV4_E] == NULL)
                    CPSS_LOG_ERROR_AND_RETURN_MAC(GT_OUT_OF_CPU_MEM, LOG_ERROR_NO_MSG);

                /* allocate */
                mcGroupNode[CPSS_IP_PROTOCOL_IPV4_E] =
                    (PRV_CPSS_DXCH_LPM_TCAM_MC_GROUP_SRC_NODE_STC*)cpssOsMalloc(
                    sizeof(PRV_CPSS_DXCH_LPM_TCAM_MC_GROUP_SRC_NODE_STC));
                if ( mcGroupNode[CPSS_IP_PROTOCOL_IPV4_E] == NULL)
                {
                    cpssOsFree(defMcRoutePtr[CPSS_IP_PROTOCOL_IPV4_E]);
                    CPSS_LOG_ERROR_AND_RETURN_MAC(GT_OUT_OF_CPU_MEM, LOG_ERROR_NO_MSG);
                }

                cpssOsMemCpy(defMcRoutePtr[CPSS_IP_PROTOCOL_IPV4_E],
                             defIpv4McRouteLttEntryPtr,
                             sizeof(PRV_CPSS_DXCH_LPM_ROUTE_ENTRY_POINTER_STC));
                 mcGroupNode[CPSS_IP_PROTOCOL_IPV4_E]->lttEntryPtr = defMcRoutePtr[CPSS_IP_PROTOCOL_IPV4_E];
                 mcGroupNode[CPSS_IP_PROTOCOL_IPV4_E]->ipMcSrcTrie = NULL;
                 mcGroupNode[CPSS_IP_PROTOCOL_IPV4_E]->isAPointer2Def = GT_FALSE;
                 mcGroupNode[CPSS_IP_PROTOCOL_IPV4_E]->gIndex = 0;
            }
            if (defIpv6McRouteLttEntryPtr != NULL)
            {
                switch(defIpv6McRouteLttEntryPtr->ipv6McGroupScopeLevel)
                {
                    case CPSS_IPV6_PREFIX_SCOPE_LINK_LOCAL_E:
                    case CPSS_IPV6_PREFIX_SCOPE_SITE_LOCAL_E:
                    case CPSS_IPV6_PREFIX_SCOPE_UNIQUE_LOCAL_E:
                    case CPSS_IPV6_PREFIX_SCOPE_GLOBAL_E:
                        break;
                    default:
                        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
                }
                for(i=0; i < ipTcamShadowPtr->shadowDevList.ipShareDevNum; i++)
                {
                    devNum = ipTcamShadowPtr->shadowDevList.ipShareDevs[i];

                    if (0 == PRV_CPSS_DXCH_LION_FAMILY_CHECK_MAC(devNum))
                    {
                        if (defIpv6McRouteLttEntryPtr->blockSize > CPSS_DXCH_IP_MAX_ECMP_QOS_GROUP)
                            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
                    }
                    else
                    {
                        if(defIpv6McRouteLttEntryPtr->routeEntryMethod == PRV_CPSS_DXCH_LPM_ENTRY_TYPE_QOS_E)
                        {
                            if (defIpv6McRouteLttEntryPtr->blockSize > CPSS_DXCH_IP_MAX_LION_QOS_GROUP)
                                CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
                        }
                        else
                        {
                            if (defIpv6McRouteLttEntryPtr->blockSize > CPSS_DXCH_IP_MAX_LION_ECMP_GROUP)
                                CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
                        }

                    }

                    if (defIpv6McRouteLttEntryPtr->routeEntryBaseMemAddr >= PRV_CPSS_DXCH_PP_MAC(devNum)->fineTuning.tableSize.routerNextHop)
                        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
                }

                defMcRoutePtr[CPSS_IP_PROTOCOL_IPV6_E] = cpssOsMalloc(sizeof (PRV_CPSS_DXCH_LPM_ROUTE_ENTRY_POINTER_STC));
                if (defMcRoutePtr[CPSS_IP_PROTOCOL_IPV6_E] == NULL)
                    CPSS_LOG_ERROR_AND_RETURN_MAC(GT_OUT_OF_CPU_MEM, LOG_ERROR_NO_MSG);

                /* allocate */
                mcGroupNode[CPSS_IP_PROTOCOL_IPV6_E] =
                    (PRV_CPSS_DXCH_LPM_TCAM_MC_GROUP_SRC_NODE_STC*)cpssOsMalloc(
                    sizeof(PRV_CPSS_DXCH_LPM_TCAM_MC_GROUP_SRC_NODE_STC));
                if ( mcGroupNode[CPSS_IP_PROTOCOL_IPV6_E] == NULL)
                {
                    cpssOsFree(defMcRoutePtr[CPSS_IP_PROTOCOL_IPV6_E]);
                    CPSS_LOG_ERROR_AND_RETURN_MAC(GT_OUT_OF_CPU_MEM, LOG_ERROR_NO_MSG);
                }

                cpssOsMemCpy(defMcRoutePtr[CPSS_IP_PROTOCOL_IPV6_E],
                             defIpv6McRouteLttEntryPtr,
                             sizeof(PRV_CPSS_DXCH_LPM_ROUTE_ENTRY_POINTER_STC));
                mcGroupNode[CPSS_IP_PROTOCOL_IPV6_E]->lttEntryPtr = defMcRoutePtr[CPSS_IP_PROTOCOL_IPV6_E];
                mcGroupNode[CPSS_IP_PROTOCOL_IPV6_E]->ipMcSrcTrie = NULL;
                mcGroupNode[CPSS_IP_PROTOCOL_IPV6_E]->isAPointer2Def = GT_FALSE;
                mcGroupNode[CPSS_IP_PROTOCOL_IPV6_E]->gIndex = 0;
            }
        }
        break;
    default:
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_FAIL, LOG_ERROR_NO_MSG);
    }

    /* init the Tries for the appropriate ip version.*/
    for (i = 0; i < 2; i++)
    {
        if ((i == protocolStack) || (protocolStack == CPSS_IP_PROTOCOL_IPV4V6_E))
        {
            if (ipNextHopEntry[i] != NULL)
            {
                retVal = prvCpssDxChLpmTcamPatTrieInit(ipTcamShadowPtr,&patTrie);
                if (retVal != GT_OK)
                    break;
                vrEntryPtr->ipUcTrie[i] = patTrie;
                /* check if we have the rule idx */
                if (defUcNextHopRuleIdxArr != NULL)
                {
                    if (i == CPSS_IP_PROTOCOL_IPV4_E)
                        entryType = PRV_CPSS_DXCH_LPM_TCAM_ENTRIES_HANDLER_IPV4_UC_ENTRY_E;
                    else
                        entryType = PRV_CPSS_DXCH_LPM_TCAM_ENTRIES_HANDLER_IPV6_UC_ENTRY_E;
                    retVal = prvCpssDxChLpmTcamDispatchSetEntryHsuInfo(ipTcamShadowPtr,
                                                                       entryType,
                                                                       defUcNextHopRuleIdxArr[i],
                                                                       &tcamRulePtr);
                    if (retVal != GT_OK)
                        break;
                    ruleNode = (PRV_CPSS_DXCH_LPM_TCAM_RULE_NODE_STC*)tcamRulePtr;
                }

                /* now insert the default nexthops+prefixes */
                if (i == CPSS_IP_PROTOCOL_IPV4_E)
                    entryType = PRV_CPSS_DXCH_LPM_TCAM_ENTRIES_HANDLER_IPV4_UC_ENTRY_E;
                else
                    entryType = PRV_CPSS_DXCH_LPM_TCAM_ENTRIES_HANDLER_IPV6_UC_ENTRY_E;
                retVal =
                    prvCpssDxChLpmTcamPatTrieInsert(vrId,
                                                    patTrie,
                                                    ipTcamShadowPtr,
                                                    entryType,
                                                    zeroIpAddr,
                                                    0,
                                                    0,
                                                    ipNextHopEntry[i],
                                                    GT_FALSE,
                                                    GT_FALSE,
                                                    GT_FALSE /* defragEnable */,ruleNode,
                                                    &ipTcamShadowPtr->ipUcTcamFuncsPtrs[i]);
                if (retVal == GT_OK)
                {
                    ipTcamShadowPtr->allocatedIpPrefixCounters.numOfAllocatedIpPrefixes[i]++;
                }
                else
                {
                    break;
                }
            }

            /* MC is supported for:                                     */
            /* - Cheetah3 or                                            */
            /* - Cheetah2 that doesn't support multiply virtual routers */
            if ((ipTcamShadowPtr->shadowType == PRV_CPSS_DXCH_LPM_TCAM_CHEETAH3_SHADOW_E) ||
                (ipTcamShadowPtr->shadowType == PRV_CPSS_DXCH_LPM_TCAM_XCAT_SHADOW_E) ||
                ((ipTcamShadowPtr->shadowType == PRV_CPSS_DXCH_LPM_TCAM_CHEETAH2_SHADOW_E) && (vrId == 0)))
            {
                if (defMcRoutePtr[i] != NULL)
                {
                    retVal = prvCpssDxChLpmTcamPatTrieInit(ipTcamShadowPtr,&patTrie);
                    if (retVal != GT_OK)
                        break;

                    vrEntryPtr->ipMcTrie[i] = patTrie;

                    ruleNode = NULL;
                    /* check if we have the rule idx */
                    if (defMcRouteRuleIdxArr != NULL)
                    {
                        if (i == CPSS_IP_PROTOCOL_IPV4_E)
                            entryType = PRV_CPSS_DXCH_LPM_TCAM_ENTRIES_HANDLER_IPV4_MC_GROUP_ENTRY_E;
                        else
                            entryType = PRV_CPSS_DXCH_LPM_TCAM_ENTRIES_HANDLER_IPV6_MC_GROUP_ENTRY_E;
                        retVal = prvCpssDxChLpmTcamDispatchSetEntryHsuInfo(ipTcamShadowPtr,
                                                                           entryType,
                                                                           defMcRouteRuleIdxArr[i],
                                                                           &tcamRulePtr);
                        if (retVal != GT_OK)
                            break;
                        ruleNode = (PRV_CPSS_DXCH_LPM_TCAM_RULE_NODE_STC*)tcamRulePtr;
                    }

                    if (i == CPSS_IP_PROTOCOL_IPV4_E)
                        entryType = PRV_CPSS_DXCH_LPM_TCAM_ENTRIES_HANDLER_IPV4_MC_GROUP_ENTRY_E;
                    else
                        entryType = PRV_CPSS_DXCH_LPM_TCAM_ENTRIES_HANDLER_IPV6_MC_GROUP_ENTRY_E;
                    retVal =
                        prvCpssDxChLpmTcamPatTrieInsert(vrId,
                                                        patTrie,
                                                        ipTcamShadowPtr,
                                                        entryType,
                                                        cpssDxChDefaultMcIpAddr[i],
                                                        cpssDxChDefaultMcIpPrefixLen[i],
                                                        0,
                                                        mcGroupNode[i],
                                                        GT_FALSE,
                                                        GT_FALSE,
                                                        GT_FALSE /* defragEnable */,ruleNode,
                                                        &ipTcamShadowPtr->ipMcTcamFuncsPtrs[i]);
                    if (retVal == GT_OK)
                    {
                        ipTcamShadowPtr->allocatedIpPrefixCounters.numOfAllocatedIpPrefixes[i]++;
                    }
                    else
                    {
                        break;
                    }
                }
            }
        }
    }

    if (retVal == GT_OK)
    {
        vrEntryPtr->vrId &= (~BIT_15); /* in the vr skip list the application vr id is used */
        retVal = prvCpssSlAdd(ipTcamShadowPtr->vrSl,vrEntryPtr,&iter);
    }
    else
        cpssOsFree(vrEntryPtr);

    return retVal;
}

/*******************************************************************************
* prvCpssDxChLpmTcamGetVirtualRouter
*
* DESCRIPTION:
*       This function gets the virtual router configuration
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* NOT APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       vrId                    - The virtual's router ID (in cheetah+ this
*                                 will be the pcl id).
*       ipTcamShadowPtr         - the ip shadow we are working on.
*
* OUTPUTS:
*       supportIpv4UcPtr           - Whether the virtual router supports IPv4 UC
*       defIpv4UcNextHopInfoPtr    - The next hop info representing the default Ipv4 Uc
*                                    route entry.
*                                    Relevant only when <supportIpv4UcPtr> == GT_TRUE
*       supportIpv6UcPtr           - Whether the virtual router supports IPv6 UC
*       defIpv6UcNextHopInfoPtr    - The next hop info representing the default Ipv6 Uc
*                                    route entry.
*                                    Relevant only when <supportIpv6UcPtr> == GT_TRUE

*       defUcNextHopRuleIdxArr     - if not NULL, then this is the internal rule idx
*                                    of the defualt NextHop prefix. (array of 2
*                                    for each of the protocol Stacks)
*                                    Relevant only when <supportIpv4UcPtr> == GT_TRUE
*                                    or <supportIpv6UcPtr> == GT_TRUE
*       supportIpv4McPtr           - Whether the virtual router supports Ipv4 MC
*       defIpv4McRouteLttEntryPtr  - The default Ipv4 MC LTT entry
*                                    Relevant only when <supportIpv4McPtr> == GT_TRUE
*       supportIpv6McPtr           - Whether the virtual router supports Ipv6 MC
*       defIpv6McRouteLttEntryPtr  - The default Ipv6 MC LTT entry
*                                    Relevant only when <supportIpv6McPtr> == GT_TRUE
*       defMcRouteRuleIdxArr       - if not null then this is the internal Rule idx
*                                    of the default mc route (prefix) (array of 2
*                                    for each of the protocol Stacks)
*                                    Relevant only when <supportIpv4McPtr> == GT_TRUE
*                                    or <supportIpv6McPtr> == GT_TRUE
*
* RETURNS:
*       GT_OK on success, or
*       GT_NOT_FOUND             - if the given next hop Id is not valid.
*       GT_OUT_OF_CPU_MEM        - if failed to allocate CPU memory, or
*       GT_OUT_OF_PP_MEM         - if failed to allocate PP memory.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS prvCpssDxChLpmTcamGetVirtualRouter
(
    IN  GT_U32                                       vrId,
    IN  PRV_CPSS_DXCH_LPM_TCAM_SHADOW_STC            *ipTcamShadowPtr,
    OUT GT_BOOL                                      *supportIpv4UcPtr,
    OUT PRV_CPSS_DXCH_LPM_ROUTE_ENTRY_INFO_UNT       *defIpv4UcNextHopInfoPtr,
    OUT GT_BOOL                                      *supportIpv6UcPtr,
    OUT PRV_CPSS_DXCH_LPM_ROUTE_ENTRY_INFO_UNT       *defIpv6UcNextHopInfoPtr,
    OUT GT_U32                                       defUcNextHopRuleIdxArr[],
    OUT GT_BOOL                                      *supportIpv4McPtr,
    OUT PRV_CPSS_DXCH_LPM_ROUTE_ENTRY_POINTER_STC    *defIpv4McRouteLttEntryPtr,
    OUT GT_BOOL                                      *supportIpv6McPtr,
    OUT PRV_CPSS_DXCH_LPM_ROUTE_ENTRY_POINTER_STC    *defIpv6McRouteLttEntryPtr,
    OUT GT_U32                                       defMcRouteRuleIdxArr[]
)
{
    GT_STATUS                                    retVal = GT_OK;
    PRV_CPSS_DXCH_LPM_TCAM_VR_TBL_BLOCK_STC      vrEntryTmp;
    PRV_CPSS_DXCH_LPM_TCAM_VR_TBL_BLOCK_STC      *vrEntryPtr;
    PRV_CPSS_DXCH_LPM_ROUTE_ENTRY_INFO_UNT       currNextHopInfo;
    PRV_CPSS_DXCH_LPM_ROUTE_ENTRY_POINTER_STC    currMcLttEntryPtr;
    GT_U8                                        ipAddr[16] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
    GT_U32                                       internalRuleIdx[2];
    GT_U32                                       i;

    cpssOsMemSet(&currNextHopInfo,0,sizeof(currNextHopInfo));
    cpssOsMemSet(&currMcLttEntryPtr,0,sizeof(currMcLttEntryPtr));
    cpssOsMemSet(&vrEntryTmp,0,sizeof(vrEntryTmp));


    /* if initialization has not been done for the requested protocol stack -
    return error */
    if ((ipTcamShadowPtr->isIpVerIsInitialized[0] == GT_FALSE) &&
         (ipTcamShadowPtr->isIpVerIsInitialized[1] == GT_FALSE))
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_INITIALIZED, LOG_ERROR_NO_MSG);

    *supportIpv4UcPtr = GT_FALSE;
    *supportIpv6UcPtr = GT_FALSE;
    *supportIpv4McPtr = GT_FALSE;
    *supportIpv6McPtr = GT_FALSE;

    if (defUcNextHopRuleIdxArr != NULL)
        defUcNextHopRuleIdxArr[0] = defUcNextHopRuleIdxArr[1] = 0;
    if (defMcRouteRuleIdxArr != NULL)
        defMcRouteRuleIdxArr[0] = defMcRouteRuleIdxArr[1] = 0;

    /* check if the VR exist */
    vrEntryTmp.vrId = vrId;

    vrEntryPtr = prvCpssSlSearch(ipTcamShadowPtr->vrSl,&vrEntryTmp);
    if (vrEntryPtr == NULL)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_FOUND, LOG_ERROR_NO_MSG);
    }

    if ((vrEntryPtr->ipUcTrie[0] == NULL) && (vrEntryPtr->ipMcTrie[0] == NULL) &&
        (vrEntryPtr->ipUcTrie[1] == NULL) && (vrEntryPtr->ipMcTrie[1] == NULL))
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_FOUND, LOG_ERROR_NO_MSG);

    /* go over both protocol stacks */
    for (i = 0; i < 2; i++)
    {
        /* copy ipv4/v6 defaults */
        if (vrEntryPtr->ipUcTrie[i] != NULL)
        {
            if (ipTcamShadowPtr->isIpVerIsInitialized[i] == GT_FALSE)
                CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_INITIALIZED, LOG_ERROR_NO_MSG);

            retVal = prvCpssDxChLpmTcamSearchIpUcEntry(vrEntryPtr->vrId,
                                                       ipAddr,0,
                                                       &currNextHopInfo,
                                                       internalRuleIdx,
                                                       (CPSS_IP_PROTOCOL_STACK_ENT)i,
                                                       ipTcamShadowPtr);
            if (retVal == GT_NOT_FOUND)
            {
                return GT_OK;
            }
            if (retVal != GT_OK)
            {
                CPSS_LOG_ERROR_AND_RETURN_MAC(GT_FAIL, LOG_ERROR_NO_MSG);
            }
            if (i == CPSS_IP_PROTOCOL_IPV4_E )
            {
                *defIpv4UcNextHopInfoPtr = currNextHopInfo;
                *supportIpv4UcPtr = GT_TRUE;
            }
            if (i == CPSS_IP_PROTOCOL_IPV6_E )
            {
                *defIpv6UcNextHopInfoPtr = currNextHopInfo;
                *supportIpv6UcPtr = GT_TRUE;
            }

            if(defUcNextHopRuleIdxArr != NULL)
            {
                defUcNextHopRuleIdxArr[i] = internalRuleIdx[0];
            }
        }
        if (vrEntryPtr->ipMcTrie[i] != NULL)
        {
            if (ipTcamShadowPtr->isIpVerIsInitialized[i] == GT_FALSE)
                CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_INITIALIZED, LOG_ERROR_NO_MSG);

            /* allocate MC for:                                         */
            /* - Cheetah3, Cheetah2 or xCat (not Policy Based Routing) */
            if ((ipTcamShadowPtr->shadowType == PRV_CPSS_DXCH_LPM_TCAM_CHEETAH2_SHADOW_E) ||
                (ipTcamShadowPtr->shadowType == PRV_CPSS_DXCH_LPM_TCAM_CHEETAH3_SHADOW_E) ||
                (ipTcamShadowPtr->shadowType == PRV_CPSS_DXCH_LPM_TCAM_XCAT_SHADOW_E))
            {
                retVal =
                    prvCpssDxChLpmTcamSearchMcEntry(vrEntryPtr->vrId,
                                                    cpssDxChDefaultMcIpAddr[i],
                                                    0,
                                                    ipAddr,
                                                    0,
                                                    &currMcLttEntryPtr,
                                                    internalRuleIdx,
                                                    (CPSS_IP_PROTOCOL_STACK_ENT)i,
                                                    ipTcamShadowPtr);
                if (retVal == GT_NOT_FOUND)
                {
                    return GT_OK;
                }
                if (retVal != GT_OK)
                {
                    CPSS_LOG_ERROR_AND_RETURN_MAC(GT_FAIL, LOG_ERROR_NO_MSG);
                }
                if (i == CPSS_IP_PROTOCOL_IPV4_E )
                {
                    *defIpv4McRouteLttEntryPtr = currMcLttEntryPtr;
                    *supportIpv4McPtr = GT_TRUE;
                }
                if (i == CPSS_IP_PROTOCOL_IPV6_E )
                {
                    *defIpv6McRouteLttEntryPtr = currMcLttEntryPtr;
                    *supportIpv6McPtr = GT_TRUE;
                }


                if(defMcRouteRuleIdxArr != NULL)
                    defMcRouteRuleIdxArr[i] = internalRuleIdx[0];
            }
        }
    }

    return retVal;
}


/*******************************************************************************
* prvCpssDxChLpmTcamDeleteVirtualRouter
*
* DESCRIPTION:
*       This function deletes an existing virtual router from the vr table, The
*       IP tables (Uc & Mc) are flushed as a result of deletion.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* NOT APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       vrId          - The virtual's router ID to be deleted.
*       ipTcamShadowPtr - the ip shadow we are working on.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK on success, or
*       GT_FAIL if the given vrId does not exist.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS prvCpssDxChLpmTcamDeleteVirtualRouter
(
    IN GT_U32                              vrId,
    IN PRV_CPSS_DXCH_LPM_TCAM_SHADOW_STC   *ipTcamShadowPtr
)
{
    /* Point to the first level bucket in the LPM structure to be deleted.   */
    GT_U8 i;
    PRV_CPSS_DXCH_LPM_TCAM_VR_TBL_BLOCK_STC vrEntryTmp;
    PRV_CPSS_DXCH_LPM_TCAM_VR_TBL_BLOCK_STC *vrEntryPtr;

    vrEntryTmp.vrId = vrId;
    vrEntryPtr = prvCpssSlSearch(ipTcamShadowPtr->vrSl,&vrEntryTmp);
    if(vrEntryPtr == NULL)
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_FOUND, LOG_ERROR_NO_MSG);

    /* Internally, bit 15 in vrId indicates shared virtual router. However this
       function only uses either calls to other CPSS API that expects the original
       vrId or calls to internal functions that doesn't need this indication. */

    for (i = 0; i < 2;i++)
    {
        if (vrEntryPtr->ipUcTrie[i] != NULL)
        {
            /* Flush the IP-Uc table associated witht is vrId.  */
            if (prvCpssDxChLpmTcamFlushIpUcEntries(vrId,i,ipTcamShadowPtr,GT_TRUE)
                != GT_OK)
                CPSS_LOG_ERROR_AND_RETURN_MAC(GT_FAIL, LOG_ERROR_NO_MSG);
            vrEntryPtr->ipUcTrie[i] = NULL;
        }

        if (vrEntryPtr->ipMcTrie[i] != NULL)
        {
            /* Flush the IP-mc table associated witht is vrId.  */
            if (prvCpssDxChLpmTcamFlushIpMcEntries(vrId,i,ipTcamShadowPtr,GT_TRUE)
                != GT_OK)
            {
                CPSS_LOG_ERROR_AND_RETURN_MAC(GT_FAIL, LOG_ERROR_NO_MSG);
            }

            vrEntryPtr->ipMcTrie[i] = NULL;
        }
    }

    if (prvCpssSlDelete(ipTcamShadowPtr->vrSl,vrEntryPtr) == NULL)
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_FAIL, LOG_ERROR_NO_MSG);

    cpssOsFree(vrEntryPtr);

    return GT_OK;
}


/*******************************************************************************
* prvCpssDxChLpmTcamRemoveDevsFromShadow
*
* DESCRIPTION:
*       removes the devices from belonging to the shadow.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* NOT APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devListPtr  - The device list to remove
*       ipTcamShadowPtr - the ip shadow we are working on.
*
* OUTPUTS:
*       None.
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_OUT_OF_CPU_MEM        - on malloc failed
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvCpssDxChLpmTcamRemoveDevsFromShadow
(
    IN PRV_CPSS_DXCH_LPM_SHADOW_DEVS_LIST_STC *devListPtr,
    IN PRV_CPSS_DXCH_LPM_TCAM_SHADOW_STC      *ipTcamShadowPtr
)
{
    GT_U8                   ind,j;
    GT_STATUS               retVal = GT_OK;
    GT_U8                   devNum;
    GT_U32                  i;

    for (i = 0; i < devListPtr->ipShareDevNum ; i++)
    {
        devNum = devListPtr->ipShareDevs[i];

        /* look for the index of devNum in the inner shared device list */
        for (ind = 0; (ind < ipTcamShadowPtr->shadowDevList.ipShareDevNum) &&
            (ipTcamShadowPtr->shadowDevList.ipShareDevs[ind] != devNum);
            ind++);

        /* if we haven't found the device in the shareDevsList then the */
        /* device wasn't initialized for ip yet */
        if (ind == ipTcamShadowPtr->shadowDevList.ipShareDevNum)
        {
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_INITIALIZED, LOG_ERROR_NO_MSG);
        }
        else
        {

            ipTcamShadowPtr->shadowDevList.ipShareDevNum--;

            /* go over the lists of the devices and base addresses and pull */
            /* on index to remove the data of the removed device */
            for (j = ind; j < ipTcamShadowPtr->shadowDevList.ipShareDevNum ; j++)
            {
                ipTcamShadowPtr->shadowDevList.ipShareDevs[j]   =
                ipTcamShadowPtr->shadowDevList.ipShareDevs[j+1];

            }
        }
    }
    return retVal;
}

/*******************************************************************************
* prvCpssDxChLpmTcamGetDevsFromShadow
*
* DESCRIPTION:
*       Get the devices from belonging to the shadow.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       ipTcamShadowPtr   - the ip shadow we are working on.
*       numOfDevsPtr      - points to the size of devListArray
*
* OUTPUTS:
*       numOfDevsPtr      - points to the number of devices retrieved
*       devListArray      - array of device ids in the LPM DB
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_BAD_SIZE              - in case not enough memory was allocated to device list
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       The application should allocate memory for the device list array.
*       If the function returns GT_OK, then number of devices holds
*       the number of devices the function filled.
*       If the function returns GT_BAD_SIZE, then the memory allocated by the
*       application to the device list is not enough. In this case the
*       number of devices will hold the size of array needed.
*
*******************************************************************************/
GT_STATUS prvCpssDxChLpmTcamGetDevsFromShadow
(
    IN    PRV_CPSS_DXCH_LPM_TCAM_SHADOW_STC          *ipTcamShadowPtr,
    OUT   GT_U8                                      devListArray[],
    INOUT GT_U32                                     *numOfDevsPtr
)
{
    GT_U32  i;
    PRV_CPSS_DXCH_LPM_SHADOW_DEVS_LIST_STC *shadowDevListPtr;

    shadowDevListPtr = &ipTcamShadowPtr->shadowDevList;

    if (*numOfDevsPtr < shadowDevListPtr->ipShareDevNum)
    {
        *numOfDevsPtr = shadowDevListPtr->ipShareDevNum;
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_SIZE, LOG_ERROR_NO_MSG);
    }

    for (i = 0 ; i < shadowDevListPtr->ipShareDevNum ; i++)
    {
        devListArray[i] = shadowDevListPtr->ipShareDevs[i];
    }
    *numOfDevsPtr = shadowDevListPtr->ipShareDevNum;

    return GT_OK;
}


/*******************************************************************************
* prvCpssDxChLpmTcamUpdateHw
*
* DESCRIPTION:
*       Perform the actual HW update at the end of a hot sync operation
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* NOT APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       ipTcamShadowPtr - the shadow to write the HW.
*       updateMode      - the update mode.
*       protocolStack   - which protocol stack to update.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS prvCpssDxChLpmTcamUpdateHw
(
    IN PRV_CPSS_DXCH_LPM_TCAM_SHADOW_STC         *ipTcamShadowPtr,
    IN PRV_CPSS_DXCH_LPM_TCAM_UPDATE_TYPE_ENT    updateMode,
    IN CPSS_IP_PROTOCOL_STACK_ENT                protocolStack
)
{
    GT_STATUS ret;
    GT_U8 i;
    GT_UINTPTR iterator = 0;
    GT_BOOL isUpdateOnly = (updateMode == PRV_CPSS_DXCH_LPM_TCAM_UPDATE_UC_E) ? GT_TRUE : GT_FALSE;


    PRV_CPSS_DXCH_LPM_TCAM_VR_TBL_BLOCK_STC *vrEntryPtr;

    while ((vrEntryPtr = prvCpssSlGetNext(ipTcamShadowPtr->vrSl,&iterator)) != NULL)
    {
        /* go over the intialized tries.*/
        for (i = 0; i < 2;i++)
        {
            if ((vrEntryPtr->ipUcTrie[i] == NULL) ||
                ((protocolStack != CPSS_IP_PROTOCOL_IPV4V6_E) &&
                 (protocolStack != i)))
            {
                continue;
            }

            /* internally, bit 15 in vrId indicates shared virtual router */
            if ((ipTcamShadowPtr->sharedVrExist == GT_TRUE) && (vrEntryPtr->vrId == ipTcamShadowPtr->sharedVrId))
                vrEntryPtr->vrId |= BIT_15;

            /* write the UC */
            ret = prvCpssDxChLpmTcamPatTrieWriteToHw(ipTcamShadowPtr,
                                                     vrEntryPtr->vrId,
                                                     vrEntryPtr->ipUcTrie[i],
                                                     &ipTcamShadowPtr->ipUcTcamFuncsPtrs[i],
                                                     isUpdateOnly);
            if (ret != GT_OK)
            {
                return ret;
            }

            /* MC is supported for:                                     */
            /* - Cheetah3 or                                            */
            /* - Cheetah2 that doesn't support multiply virtual routers */
            if ((ipTcamShadowPtr->shadowType == PRV_CPSS_DXCH_LPM_TCAM_CHEETAH3_SHADOW_E) ||
                (ipTcamShadowPtr->shadowType == PRV_CPSS_DXCH_LPM_TCAM_XCAT_SHADOW_E) ||
                ((ipTcamShadowPtr->shadowType == PRV_CPSS_DXCH_LPM_TCAM_CHEETAH2_SHADOW_E) && (vrEntryPtr->vrId == 0)))
            {
                if ((updateMode == PRV_CPSS_DXCH_LPM_TCAM_OVERWRITE_UC_MC_E) && (vrEntryPtr->ipMcTrie[i] != NULL))
                {
                    ret = prvCpssDxChLpmTcamIpMcEntriesWriteToHw(vrEntryPtr,i,
                                                                 ipTcamShadowPtr);

                    if (ret != GT_OK)
                    {
                        return ret;
                    }
                }
            }
        }
    }
    return GT_OK;
}

/*******************************************************************************
* prvCpssDxChLpmTcamPbrDefaultMcRuleSet
*
* DESCRIPTION:
*       Sets "do-nothing" PCL rule to capture all MC traffic for specific
*       protocol stack.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* NOT APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       ipTcamShadowPtr   - the ip shadow we are working on
*       devListPtr        - list of devices to set the rule at
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK           - on success.
*       GT_BAD_PARAM    - Illegal parameter in function called
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS prvCpssDxChLpmTcamPbrDefaultMcRuleSet
(
    IN  PRV_CPSS_DXCH_LPM_TCAM_SHADOW_STC       *ipTcamShadowPtr,
    IN  PRV_CPSS_DXCH_LPM_SHADOW_DEVS_LIST_STC  *devListPtr
)
{
    GT_STATUS                           retVal = GT_OK;
    GT_IPADDR                           ipv4Addr;
    GT_U32                              ipv4PrefixLen;
    GT_IPV6ADDR                         ipv6Addr;
    GT_U32                              ipv6PrefixLen;
    GT_U8                               *ipAddrPtr;
    GT_U32                              prefixLen;
    GT_BOOL                             isIpv6;
    CPSS_DXCH_PCL_RULE_FORMAT_TYPE_ENT  ruleFormat;
    CPSS_DXCH_PCL_RULE_FORMAT_UNT       ruleMask;
    CPSS_DXCH_PCL_RULE_FORMAT_UNT       rulePattern;
    GT_U32                              i,j;
    GT_U32                              offset;
    GT_PTR                              tcamRulePtr = NULL;

    CPSS_DXCH_PCL_ACTION_STC            doNothingRuleAction;

    cpssOsMemSet(&doNothingRuleAction, 0, sizeof(doNothingRuleAction));
    doNothingRuleAction.egressPolicy = GT_FALSE;
    doNothingRuleAction.pktCmd = CPSS_PACKET_CMD_FORWARD_E;

    /* prepare IP address according to protocol stack */
    cpssOsMemSet(&ipv4Addr, 0, sizeof(ipv4Addr));
    ipv4Addr.arIP[0] = 224;
    ipv4PrefixLen = 4;
    cpssOsMemSet(&ipv6Addr, 0, sizeof(ipv6Addr));
    ipv6Addr.arIP[0] = 0xFF;
    ipv6PrefixLen = 8;

    for (i = 0 ; i < pbrDefaultMcTrieNodePtrArraySize ; i++)
    {
        if (pbrDefaultMcTrieNodePtrArray[i] == NULL)
            continue;

        /* even entries are used to IPv4 while odd one for IPv6 */
        if ((i % 2) == 0)
        {
            ipAddrPtr = ipv4Addr.arIP;
            prefixLen = ipv4PrefixLen;
            isIpv6 = GT_FALSE;
        }
        else /* ipv6 entry */
        {
            ipAddrPtr = ipv6Addr.arIP;
            prefixLen = ipv6PrefixLen;
            isIpv6 = GT_TRUE;
        }

        cpssOsMemSet(&ruleMask,0,sizeof(ruleMask));
        cpssOsMemSet(&rulePattern,0,sizeof(rulePattern));

        /* prepare rule and mask in PCL rule format */
        retVal = ipTcamTriePclUtilFillIIpUcMaskAndPatternCheetah(pbrDefaultMcTrieNodePtrArray[i]->vrId,
                                                                 ipAddrPtr,
                                                                 prefixLen,
                                                                 GT_FALSE/*isL4Valid*/,
                                                                 0/*ipProtocol*/,
                                                                 0/*tcpUdpSourcePort*/,
                                                                 0/*tcpUdpSourcePortMaskLen*/,
                                                                 isIpv6,
                                                                 &ruleFormat,
                                                                 &ruleMask,
                                                                 &rulePattern);
        if (retVal != GT_OK)
            return retVal;

        retVal = prvCpssDxChLpmTcamDispatcherRuleSet(pbrDefaultMcTrieNodePtrArray[i],
                                                     ipTcamShadowPtr,
                                                     &tcamRulePtr);
        if (retVal != GT_OK)
            return retVal;

        retVal = prvCpssDxChLpmTcamGetTcamOffset(ipTcamShadowPtr,
                                            tcamRulePtr,GT_TRUE,
                                            &offset);
        if (retVal != GT_OK)
            return retVal;

        /* now set the rule at all the shared devices */
        for (j = 0; j < devListPtr->ipShareDevNum ; j++)
        {
            retVal = cpssDxChPclRuleSet(
                devListPtr->ipShareDevs[j],ruleFormat,
                offset, 0 /*ruleOptionsBmp*/,
                                        &ruleMask,&rulePattern,&doNothingRuleAction);
            if (retVal != GT_OK)
                return retVal;
        }
    }

    return retVal;
}



/*******************************************************************************
* prvCpssDxChLpmTcamGetTcamOffset
*
* DESCRIPTION:
*       Get  IP TCAM hardware offset
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* NOT APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       ipTcamShadowPtr     - ip shadow we are working on.
*       tcamRulePtr         - pointer to tcam rule
*       isToken             - whether the tcamRulePtr represent TCAM location
*                             or token (relevant only in TCAM manager mode)
*
* OUTPUTS:
*       offsetPtr           - pointer to TCAM rule hardware offset.
*
* RETURNS:
*      GT_OK        - on OK
*      GT_FAIL      - on FAIL
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS prvCpssDxChLpmTcamGetTcamOffset
(
    IN PRV_CPSS_DXCH_LPM_TCAM_SHADOW_STC          *ipTcamShadowPtr,
    IN  GT_PTR                                    tcamRulePtr,
    IN  GT_BOOL                                   isToken,
    OUT GT_U32                                    *offsetPtr
)
{
    GT_STATUS ret = GT_OK;
    CPSS_DXCH_TCAM_MANAGER_TCAM_LOCATION_STC    tcamLocation = {0,0};

    GT_U32      tcamLines = tcamRows;
    GT_U32      tcamRow = 0;
    GT_U32      tcamColumn = 0;
    if (tcamLines == 0)
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_FAIL, LOG_ERROR_NO_MSG);

    if (ipTcamShadowPtr->tcamManagementMode == PRV_CPSS_DXCH_LPM_TCAM_MANAGEMENT_TCAM_ENTRIES_HANDLER_MODE_E)
    {
        *offsetPtr = ((PRV_CPSS_DXCH_LPM_TCAM_RULE_NODE_STC*)tcamRulePtr)->offset;
    }
    else
    {
        if (isToken == GT_TRUE)
        {
            ret = cpssDxChTcamManagerTokenToTcamLocation(ipTcamShadowPtr->tcamManagerHandlerPtr,
                                                         ipTcamShadowPtr->clientId,
                                                         (GT_U32)((GT_UINTPTR)tcamRulePtr),
                                                         &tcamLocation);
            if (ret != GT_OK)
            {
                return ret;
            }
        }
        else
        {
            cpssOsMemCpy(&tcamLocation,tcamRulePtr,sizeof(CPSS_DXCH_TCAM_MANAGER_TCAM_LOCATION_STC));
        }

        tcamRow = tcamLocation.row;
        tcamColumn = tcamLocation.column;
        *offsetPtr = tcamColumn + 4 * tcamRow;
    }
    return GT_OK;

}


/*******************************************************************************
* prvCpssDxChLpmTcamGetTcamRowColumn
*
* DESCRIPTION:
*       Get  IP TCAM hardware location :row and column
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* NOT APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       ipTcamShadowPtr     - ip shadow we are working on.
*       tcamRulePtr         - pointer to tcam rule
*
* OUTPUTS:
*       tcamRowPtr          - pointer to TCAM row.
*       tcamColumnPtr       - pointer to TCAM column.
*
* RETURNS:
*      GT_OK        - on OK
*      GT_FAIL      - on fail
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS prvCpssDxChLpmTcamGetTcamRowColumn
(
    IN PRV_CPSS_DXCH_LPM_TCAM_SHADOW_STC          *ipTcamShadowPtr,
    IN  GT_PTR                                    tcamRulePtr,
    OUT GT_U32                                    *tcamRowPtr,
    OUT GT_U32                                    *tcamColumnPtr
)
{
    GT_STATUS ret = GT_OK;
    CPSS_DXCH_TCAM_MANAGER_TCAM_LOCATION_STC    tcamLocation = {0,0};
    GT_U32      tcamLines = tcamRows;
    if (tcamLines == 0)
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_FAIL, LOG_ERROR_NO_MSG);

    if (ipTcamShadowPtr->tcamManagementMode == PRV_CPSS_DXCH_LPM_TCAM_MANAGEMENT_TCAM_ENTRIES_HANDLER_MODE_E)
    {
        *tcamRowPtr = ((PRV_CPSS_DXCH_LPM_TCAM_RULE_NODE_STC*)tcamRulePtr)->offset%tcamLines;
        *tcamColumnPtr = ((PRV_CPSS_DXCH_LPM_TCAM_RULE_NODE_STC*)tcamRulePtr)->offset/tcamLines;
    }
    else
    {
        ret = cpssDxChTcamManagerTokenToTcamLocation(ipTcamShadowPtr->tcamManagerHandlerPtr,
                                                     ipTcamShadowPtr->clientId,
                                                     (GT_U32)((GT_UINTPTR)tcamRulePtr),
                                                     &tcamLocation);
        if (ret != GT_OK)
        {
            return ret;
        }

        *tcamRowPtr = tcamLocation.row;
        *tcamColumnPtr = tcamLocation.column;
    }
    return GT_OK;
}


/*******************************************************************************
* prvCpssDxChLpmTcamFreeSpaceCountersCheck
*
* DESCRIPTION:
*       This function check whether TCAM has enough free space for
*       added IPv4/Ipv6 prefixes.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* NOT APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       numberOfPrefixes      - number of added prefixes
*       ipMode                - ip uc/mc selection
*       mcEntryType           - mc entry type: relevant only if ipMode is mc.
*       protocolStack         - the protocol stack to support
*       ipTcamShadowPtr       - ip shadow we are working on.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                 - on success.
*       GT_BAD_PARAM          - on wrong number.
*       GT_FULL               - if TCAM is full.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS prvCpssDxChLpmTcamFreeSpaceCountersCheck
(
    IN GT_U32                                          numberOfPrefixes,
    IN CPSS_IP_UNICAST_MULTICAST_ENT                   ipMode,
    IN PRV_CPSS_DXCH_LPM_TCAM_IP_MC_ENTRY_TYPE_ENT     mcEntryType,
    IN CPSS_IP_PROTOCOL_STACK_ENT                      protocolStack,
    IN PRV_CPSS_DXCH_LPM_TCAM_SHADOW_STC               *ipTcamShadowPtr
)
{
    GT_U32 calculatedIpv6Number = 0;
    GT_U32 calculatedIpv4Number = 0;
    GT_U32 calculatedIpv4McSrcNumber = 0;
    GT_U32 numberOfTcamLines = 0;
    GT_U32 tcamColumns = 0;
    GT_U32 tcamEntriesPerIpv6 = 0;
    GT_U32 ipPrefixCounter[CPSS_IP_PROTOCOL_IPV4V6_E];
    GT_U32 ipPrefixlimit[CPSS_IP_PROTOCOL_IPV4V6_E];
    GT_U32 ipv4McSourceCounter = ipTcamShadowPtr->allocatedIpPrefixCounters.numOfAllocatedIpv4McSourcePrefixes;
    GT_U32 ipv4McSourceLimit = ipTcamShadowPtr->numOfIpMcSrcAddr;
    ipPrefixCounter[CPSS_IP_PROTOCOL_IPV4_E] = ipTcamShadowPtr->allocatedIpPrefixCounters.numOfAllocatedIpPrefixes[CPSS_IP_PROTOCOL_IPV4_E];
    ipPrefixlimit[CPSS_IP_PROTOCOL_IPV4_E] = ipTcamShadowPtr->numOfIpPrefixes[CPSS_IP_PROTOCOL_IPV4_E];
    ipPrefixCounter[CPSS_IP_PROTOCOL_IPV6_E] = ipTcamShadowPtr->allocatedIpPrefixCounters.numOfAllocatedIpPrefixes[CPSS_IP_PROTOCOL_IPV6_E];
    ipPrefixlimit[CPSS_IP_PROTOCOL_IPV6_E] = ipTcamShadowPtr->numOfIpPrefixes[CPSS_IP_PROTOCOL_IPV6_E];
    if (numberOfPrefixes == 0)
    {
        return GT_OK;
    }
    if ( (ipTcamShadowPtr->shadowType == PRV_CPSS_DXCH_LPM_TCAM_CHEETAH_SHADOW_E) ||
         (ipTcamShadowPtr->shadowType == PRV_CPSS_DXCH_LPM_TCAM_CHEETAH2_SHADOW_E)||
         (ipTcamShadowPtr->shadowType == PRV_CPSS_DXCH_LPM_TCAM_CHEETAH3_SHADOW_E) )
    {
        /* these cases are handled by tcam entries handler */
        return GT_OK;
    }


    switch (ipMode)
    {
    case CPSS_IP_UNICAST_E:
    case CPSS_IP_MULTICAST_E:
        break;
    default:
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }
    if (ipMode == CPSS_IP_MULTICAST_E)
    {
        switch (mcEntryType)
        {
        case PRV_CPSS_DXCH_LPM_TCAM_IP_MC_GROUP_E:
        case PRV_CPSS_DXCH_LPM_TCAM_IP_MC_SOURCE_E:
        case PRV_CPSS_DXCH_LPM_TCAM_IP_MC_GROUP_AND_SOURCE_E:
            break;
        default:
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
        }
    }

    switch (protocolStack)
    {
    case CPSS_IP_PROTOCOL_IPV4_E:
    case CPSS_IP_PROTOCOL_IPV6_E:
        break;
    default:
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }


    if (ipMode == CPSS_IP_MULTICAST_E )
    {
        if ( (ipTcamShadowPtr->shadowType == PRV_CPSS_DXCH_LPM_TCAM_CHEETAH_SHADOW_E) ||
             (ipTcamShadowPtr->shadowType == PRV_CPSS_DXCH_LPM_TCAM_XCAT_POLICY_BASED_ROUTING_SHADOW_E))
        {
            /* no mc in PBR */
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
        }
    }

    if ( (protocolStack == CPSS_IP_PROTOCOL_IPV6_E ) && (ipMode == CPSS_IP_MULTICAST_E) &&
         (mcEntryType == PRV_CPSS_DXCH_LPM_TCAM_IP_MC_GROUP_AND_SOURCE_E) )
    {
        if (numberOfPrefixes%2 != 0)
        {
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
        }
    }
    if ( (protocolStack == CPSS_IP_PROTOCOL_IPV4_E ) && (ipMode == CPSS_IP_MULTICAST_E) &&
         (mcEntryType == PRV_CPSS_DXCH_LPM_TCAM_IP_MC_GROUP_AND_SOURCE_E) )
    {
        if (numberOfPrefixes%2 != 0)
        {
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
        }
        numberOfPrefixes = numberOfPrefixes/2;
    }

    if (ipTcamShadowPtr->tcamPartitionEnable == GT_TRUE)
    {
        /* check for static partition */
        if ( (protocolStack == CPSS_IP_PROTOCOL_IPV4_E ) && (ipMode == CPSS_IP_MULTICAST_E) &&
             (mcEntryType == PRV_CPSS_DXCH_LPM_TCAM_IP_MC_GROUP_AND_SOURCE_E) )
        {
            /* check TCAM limit for Group */
            if (ipPrefixCounter[CPSS_IP_PROTOCOL_IPV4_E] + numberOfPrefixes > ipPrefixlimit[CPSS_IP_PROTOCOL_IPV4_E])
            {
                CPSS_LOG_ERROR_AND_RETURN_MAC(GT_FULL, LOG_ERROR_NO_MSG);
            }
            /* check TCAM limit for Source */
            if (ipv4McSourceCounter + numberOfPrefixes > ipv4McSourceLimit)
            {
                CPSS_LOG_ERROR_AND_RETURN_MAC(GT_FULL, LOG_ERROR_NO_MSG);
            }
        }
        else
            if ( (protocolStack == CPSS_IP_PROTOCOL_IPV4_E ) && (ipMode == CPSS_IP_MULTICAST_E) &&
                 (mcEntryType == PRV_CPSS_DXCH_LPM_TCAM_IP_MC_SOURCE_E) )
            {
                /* check TCAM limit for Source */
                if (ipv4McSourceCounter + numberOfPrefixes > ipv4McSourceLimit)
                {
                    CPSS_LOG_ERROR_AND_RETURN_MAC(GT_FULL, LOG_ERROR_NO_MSG);
                }
            }
            else
                if (ipPrefixCounter[protocolStack] + numberOfPrefixes > ipPrefixlimit[protocolStack])
                {
                    CPSS_LOG_ERROR_AND_RETURN_MAC(GT_FULL, LOG_ERROR_NO_MSG);
                }
    }
    else
    {
        /* dynamic partition */
        /* check guarantied prefix number */
        switch (ipTcamShadowPtr->shadowType)
        {
        case PRV_CPSS_DXCH_LPM_TCAM_XCAT_SHADOW_E:
            tcamColumns = 4;
            tcamEntriesPerIpv6 = 4;
            break;
        case PRV_CPSS_DXCH_LPM_TCAM_XCAT_POLICY_BASED_ROUTING_SHADOW_E:
            if (ipMode == CPSS_IP_MULTICAST_E)
            {
                CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
            }
            tcamEntriesPerIpv6 = 1;
            tcamColumns = 4;
            break;
        CPSS_COVERITY_NON_ISSUE_BOOKMARK
        /* coverity[dead_error_begin] */default:
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_FAIL, LOG_ERROR_NO_MSG);
        }

        if (protocolStack == CPSS_IP_PROTOCOL_IPV6_E)
        {
            /* it cover IPV6 uc and ipv6 mc cases */
            if (ipPrefixCounter[CPSS_IP_PROTOCOL_IPV6_E]+ numberOfPrefixes > ipPrefixlimit[CPSS_IP_PROTOCOL_IPV6_E])
            {
                /* The guarantied limit for IPV6 will be exceed */
                /* Check quarantied limit for IPv4 and ipv4 mc source and take maximum number between
                 guarantied limit and current counter */
                calculatedIpv4Number = (ipPrefixCounter[CPSS_IP_PROTOCOL_IPV4_E]>=
                                        ipPrefixlimit[CPSS_IP_PROTOCOL_IPV4_E])?
                    ipPrefixCounter[CPSS_IP_PROTOCOL_IPV4_E] : ipPrefixlimit[CPSS_IP_PROTOCOL_IPV4_E];
                calculatedIpv4McSrcNumber =(ipv4McSourceCounter>=ipv4McSourceLimit)?ipv4McSourceCounter:ipv4McSourceLimit;
                calculatedIpv6Number = ipPrefixCounter[CPSS_IP_PROTOCOL_IPV6_E] + numberOfPrefixes;
            }
            else
            {
                return GT_OK;
            }
        }else if (protocolStack == CPSS_IP_PROTOCOL_IPV4_E)
        {
            if ( (ipMode == CPSS_IP_UNICAST_E ) ||
                 ( (ipMode == CPSS_IP_MULTICAST_E ) &&
                   (mcEntryType == PRV_CPSS_DXCH_LPM_TCAM_IP_MC_GROUP_E )  ) )
            {
                /* it covers ipv4 uc + ipv4 mc group insertion */
                if (ipPrefixCounter[CPSS_IP_PROTOCOL_IPV4_E]+ numberOfPrefixes > ipPrefixlimit[CPSS_IP_PROTOCOL_IPV4_E])
                {
                    calculatedIpv6Number = (ipPrefixCounter[CPSS_IP_PROTOCOL_IPV6_E]>=
                                            ipPrefixlimit[CPSS_IP_PROTOCOL_IPV6_E])?
                        ipPrefixCounter[CPSS_IP_PROTOCOL_IPV6_E] : ipPrefixlimit[CPSS_IP_PROTOCOL_IPV6_E];
                    calculatedIpv4McSrcNumber =(ipv4McSourceCounter>=ipv4McSourceLimit)?ipv4McSourceCounter:ipv4McSourceLimit;
                    calculatedIpv4Number = ipPrefixCounter[CPSS_IP_PROTOCOL_IPV4_E] + numberOfPrefixes;
                }
                else
                {
                    return GT_OK;
                }
            }
            else
                if ((ipMode == CPSS_IP_MULTICAST_E) && (mcEntryType == PRV_CPSS_DXCH_LPM_TCAM_IP_MC_SOURCE_E))
                {
                    /* ipv4 mc source insertion */
                    if (ipv4McSourceCounter + numberOfPrefixes > ipv4McSourceLimit)
                    {
                        calculatedIpv4McSrcNumber = ipv4McSourceCounter + numberOfPrefixes;
                        /* Check quarantied limit for IPv4 and IPV6 */
                        calculatedIpv4Number = (ipPrefixCounter[CPSS_IP_PROTOCOL_IPV4_E]>=
                                                ipPrefixlimit[CPSS_IP_PROTOCOL_IPV4_E])?
                            ipPrefixCounter[CPSS_IP_PROTOCOL_IPV4_E] : ipPrefixlimit[CPSS_IP_PROTOCOL_IPV4_E];
                        calculatedIpv6Number = (ipPrefixCounter[CPSS_IP_PROTOCOL_IPV6_E]>=
                                                ipPrefixlimit[CPSS_IP_PROTOCOL_IPV6_E])?
                            ipPrefixCounter[CPSS_IP_PROTOCOL_IPV6_E] : ipPrefixlimit[CPSS_IP_PROTOCOL_IPV6_E];
                    }
                    else
                    {
                       return GT_OK;
                    }
                }
                else
                    if((ipMode == CPSS_IP_MULTICAST_E) && (mcEntryType == PRV_CPSS_DXCH_LPM_TCAM_IP_MC_GROUP_AND_SOURCE_E ))
                    {
                        /* ipv4 mc group and source insertion */
                        if ( (ipv4McSourceCounter < ipv4McSourceLimit) &&
                             (ipPrefixCounter[CPSS_IP_PROTOCOL_IPV4_E] < ipPrefixlimit[CPSS_IP_PROTOCOL_IPV4_E]) )
                        {
                            return GT_OK;
                        }
                        else
                        {
                            /* one of them reach quarantied limit */
                            calculatedIpv4McSrcNumber = (ipv4McSourceCounter + numberOfPrefixes > ipv4McSourceLimit)?
                                ipv4McSourceCounter + numberOfPrefixes:ipv4McSourceLimit;
                            calculatedIpv4Number = (ipPrefixCounter[CPSS_IP_PROTOCOL_IPV4_E] + numberOfPrefixes >
                                                    ipPrefixlimit[CPSS_IP_PROTOCOL_IPV4_E])?
                                ipPrefixCounter[CPSS_IP_PROTOCOL_IPV4_E] + numberOfPrefixes : ipPrefixlimit[CPSS_IP_PROTOCOL_IPV4_E];

                            calculatedIpv6Number = (ipPrefixCounter[CPSS_IP_PROTOCOL_IPV6_E]>=
                                                    ipPrefixlimit[CPSS_IP_PROTOCOL_IPV6_E])?
                                ipPrefixCounter[CPSS_IP_PROTOCOL_IPV6_E] : ipPrefixlimit[CPSS_IP_PROTOCOL_IPV6_E];
                        }
                    }
           }
        /* now estimate whether enough free space in TCAM */
        numberOfTcamLines = ipTcamShadowPtr->tcamIndexRange.lastIndex -
            ipTcamShadowPtr->tcamIndexRange.firstIndex + 1;
        if ((numberOfTcamLines * tcamColumns) >= (calculatedIpv4Number + calculatedIpv4McSrcNumber
                                                  + calculatedIpv6Number*tcamEntriesPerIpv6))
        {
            /* There is free place */
            return GT_OK;
        }
        else
        {
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_FULL, LOG_ERROR_NO_MSG);
        }
    }
    return GT_OK;

}

/*******************************************************************************
* prvCpssDxChLpmTcamLttWrite
*
* DESCRIPTION:
*    Writes a LookUp Translation Table Entry.
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; xCat3; Lion; Lion2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; xCat2; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum        - the device number.
*       lttTtiRow     - the entry's row index in LTT table (equivalent to
*                       the router tcam entry it is attached to)
*       lttTtiColumn  - the entry's column index in LTT table (equivalent to
*                       the router tcam entry it is attached to)
*       lttEntryPtr   - the lookup translation table entry
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK           - if succeeded
*       GT_BAD_PARAM    - on devNum not active or
*                         invalid ipv6MCGroupScopeLevel value.
*       GT_BAD_PTR      - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       Converts an entry of type PRV_CPSS_DXCH_LPM_ROUTE_ENTRY_POINTER_STC to
*       CPSS_DXCH_IP_LTT_ENTRY_STC and calls cpssDxChIpLttWrite.
*
*******************************************************************************/
GT_STATUS prvCpssDxChLpmTcamLttWrite
(
    IN  GT_U8                                          devNum,
    IN  GT_U32                                         lttTtiRow,
    IN  GT_U32                                         lttTtiColumn,
    IN  PRV_CPSS_DXCH_LPM_ROUTE_ENTRY_POINTER_STC      *routeEntryPtr
)
{
    CPSS_DXCH_IP_LTT_ENTRY_STC lttEntry;
    prvCpssDxChLpmConvertLpmRouteEntryToIpLttEntry(PRV_CPSS_DXCH_LPM_HW_TCAM_E,
                                                   routeEntryPtr, &lttEntry);
    return cpssDxChIpLttWrite(devNum, lttTtiRow, lttTtiColumn, &lttEntry);
}

/*******************************************************************************
* prvCpssDxChLpmTcamLttRead
*
* DESCRIPTION:
*    Reads a LookUp Translation Table Entry.
*
* DESCRIPTION:
*    Reads a LookUp Translation Table Entry.
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; xCat3; Lion; Lion2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; xCat2; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum          - the device number.
*       lttTtiRow       - the entry's row index in LTT table (equivalent to
*                         the router tcam entry it is attached to)
*       lttTtiColumn    - the entry's column index in LTT table (equivalent to
*                         the router tcam entry it is attached to)
*
* OUTPUTS:
*       lttEntryPtr     - the lookup translation table entry
*
* RETURNS:
*       GT_OK           - if succeeded
*       GT_BAD_PARAM    - on devNum not active.
*       GT_BAD_PTR      - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       Calls cpssDxChIpLttRead and converts an entry of type
*       PRV_CPSS_DXCH_LPM_ROUTE_ENTRY_POINTER_STC to CPSS_DXCH_IP_LTT_ENTRY_STC.
*
*******************************************************************************/
GT_STATUS prvCpssDxChLpmTcamLttRead
(
    IN  GT_U8                                          devNum,
    IN  GT_U32                                         lttTtiRow,
    IN  GT_U32                                         lttTtiColumn,
    IN  PRV_CPSS_DXCH_LPM_ROUTE_ENTRY_POINTER_STC      *routeEntryPtr
)
{
    CPSS_DXCH_IP_LTT_ENTRY_STC lttEntry;
    GT_STATUS                  status;
    status = cpssDxChIpLttRead(devNum, lttTtiRow, lttTtiColumn, &lttEntry);
    prvCpssDxChLpmConvertIpLttEntryToLpmRouteEntry(PRV_CPSS_DXCH_LPM_HW_TCAM_E,
                                                   &lttEntry, routeEntryPtr);
    return status;
}

