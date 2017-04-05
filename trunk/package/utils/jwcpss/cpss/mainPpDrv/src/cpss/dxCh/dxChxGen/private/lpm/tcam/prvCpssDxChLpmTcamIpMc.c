/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* prvCpssDxChLpmTcamIpMc.c
*
* DESCRIPTION:
*       This file includes functions declarations for controlling the IP (v6/v4)
*       UC tables and structures, and structures definitions for shadow
*       management.
*
* DEPENDENCIES:
*       None.
*
* FILE REVISION NUMBER:
*       $Revision: 50 $
*
*******************************************************************************/

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
#include <cpss/dxCh/dxChxGen/ip/cpssDxChIp.h>
#include <cpss/dxCh/dxChxGen/ipLpmEngine/cpssDxChIpLpmTypes.h>
#include <cpss/dxCh/dxChxGen/private/lpm/tcam/prvCpssDxChLpmTcam.h>
#include <cpss/dxCh/dxChxGen/private/lpm/tcam/prvCpssDxChLpmTcamIpMc.h>
#include <cpss/dxCh/dxChxGen/private/lpm/tcam/prvCpssDxChLpmTcamEntriesHandler.h>
#include <cpss/dxCh/dxChxGen/resourceManager/cpssDxChTcamManager.h>
#include <cpss/dxCh/dxChxGen/private/lpm/tcam/prvCpssDxChLpmTcamDispatcher.h>

/****************************************************************************
* Debug                                                                     *
****************************************************************************/


/***************************************************************************
* Externals
****************************************************************************/

extern GT_U8 cpssDxChDefaultMcIpAddr[2][16];/***defaultMcIpAddr;*/

extern GT_U32 cpssDxChDefaultMcIpPrefixLen[2];/**defaultMcIpPrefixLen;*/

/* number of rows in the TCAM */
extern GT_U32  tcamRows;

/*******************************************************************************
* Ip core common environment parameters
*******************************************************************************/

/*******************************************************************************
* prvCpssDxChLpmTcamAddIpMcEntry
*
* DESCRIPTION:
*       To add the multicast routing information for IP datagrams from
*       a particular source and addressed to a particular IP multicast
*       group address.
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; xCat3; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond.
*
* INPUTS:
*       vrId            - The virtual private network identifier.
*       ipGroupPtr      - (pointer to) The IP multicast group address.
*       ipGroupPrefix   - The number of bits that are actual valid in,
*                         the ipGroup. (CIDR like)
*       ipSrcPtr        - (pointer to) the root address for source base multi tree protocol.
*       ipSrcPrefix     - The number of bits that are actual valid in,
*                         the ipSrc. (CIDR like)
*       ipLttEntryPtr   - The Ltt entry pointer to write for this mc route
*       ruleIdxArr      - if not null then this is the internal rule indexes
*                         (G,S) for this prefix (Hsu use)
*       overwrite       - update or override an existing entry, or create a new one.
*       defragEnable    - whether to defrag entries in case there is no
*                         free place for this entry cause of entries
*                         fragmentation; relevant only if the TCAM entries
*                         handler was created with partitionEnable = GT_FALSE
*       protocolStack   - type of ip protocol stack to work on.
*       ipTcamShadowPtr - the shadow pointer we are working on
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_ERROR                 - if the virtual router does not exist, or ipGroupPrefix == 0.
*       GT_FULL                  - if IP TCAM is full.
*       GT_FAIL                  - otherwise.
*       GT_NOT_SUPPORTED         - the request is not supported.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device.
*
* COMMENTS:
*       If tries to override a non existing entry - return error.
*
*******************************************************************************/
GT_STATUS prvCpssDxChLpmTcamAddIpMcEntry
(
    IN GT_U32                                       vrId,
    IN GT_U8                                        *ipGroupPtr,
    IN GT_U32                                       ipGroupPrefix,
    IN GT_U8                                        *ipSrcPtr,
    IN GT_U32                                       ipSrcPrefix,
    IN PRV_CPSS_DXCH_LPM_ROUTE_ENTRY_POINTER_STC    *ipLttEntryPtr,
    IN GT_U32                                       ruleIdxArr[],
    IN GT_BOOL                                      overwrite,
    IN GT_BOOL                                      defragEnable,
    IN CPSS_IP_PROTOCOL_STACK_ENT                   protocolStack,
    IN PRV_CPSS_DXCH_LPM_TCAM_SHADOW_STC            *ipTcamShadowPtr
)
{
    GT_U32                                          i;
    GT_STATUS                                       ret = GT_OK;
    GT_STATUS                                       ret2 = GT_OK;
    PRV_CPSS_DXCH_LPM_TCAM_PAT_TRIE_FUNCS_OBJ_STC   *ipMcTcamFuncs;
    PRV_CPSS_DXCH_LPM_TCAM_PAT_TRIE_NODE_STC        *pRootTrie;
    PRV_CPSS_DXCH_LPM_TCAM_PAT_TRIE_NODE_STC        *pGroupNode,*pSrcNode;
    PRV_CPSS_DXCH_LPM_TCAM_MC_GROUP_SRC_NODE_STC    *mcGroupNode,*mcSrcNode;
    PRV_CPSS_DXCH_LPM_TCAM_MC_GROUP_SRC_NODE_STC    *mcDefGroupNode;
    PRV_CPSS_DXCH_LPM_ROUTE_ENTRY_POINTER_STC       *lclIpLttEntryPtr = NULL;
    PRV_CPSS_DXCH_LPM_TCAM_PAT_TRIE_NODE_STC        *pSrcTrie = NULL;

    PRV_CPSS_DXCH_LPM_TCAM_RULE_NODE_STC            *ruleNodePtr = NULL;
    GT_PTR                                          tcamRulePtr = NULL;

    GT_U32                                          exactMatchPrefixLen;
    GT_BOOL                                         isExactMatch;
    PRV_CPSS_DXCH_LPM_TCAM_RULE_NODE_STC            *zeroRule = NULL;

    /* roll back params */
    GT_BOOL                                         isSrcTrieBeenCreated = GT_FALSE;
    GT_VOID                                         *rollBackFreeArray[4];
    GT_U32                                          rollBackIndex = 0;
    PRV_CPSS_DXCH_LPM_TCAM_VR_TBL_BLOCK_STC         *vrEntryPtr,vrEntryTmp;
    PRV_CPSS_DXCH_LPM_TCAM_ENTRIES_HANDLER_ENTRY_TYPE_ENT grEntryType;
    PRV_CPSS_DXCH_LPM_TCAM_ENTRIES_HANDLER_ENTRY_TYPE_ENT srcEntryType;

    GT_U8                                           *lclIpGroup;
    GT_U32                                          *lclIpGroupPrefix;
    /* indicate the group found have pointer to the default route entry*/
    GT_BOOL                                         groupFoundHaveAPointer2Def = GT_FALSE;

    PRV_CPSS_DXCH_LPM_TCAM_MC_GROUP_SRC_NODE_STC    *mcGroupNodePtr;

    /* group and group prefix used to traverse the MC tree */
    GT_U8                                           traverseIpGroup[16];
    GT_U32                                          traverseIpGroupPrefix;

    GT_U8                                           *ipShareDevs;
    GT_U32                                          numOfShareDevs;
    GT_U8                                           devNum = 0;
    CPSS_DXCH_TCAM_MANAGER_ENTRY_AMOUNT_STC         entryTypeAmount;

    GT_U32                                          mcGroupRow;
    GT_U32                                          mcGroupColumn;
    GT_U32                                          prefixNumber = 0;
    CPSS_IP_UNICAST_MULTICAST_ENT                   ipMode = CPSS_IP_MULTICAST_E;
    PRV_CPSS_DXCH_LPM_TCAM_IP_MC_ENTRY_TYPE_ENT     mcEntryType = PRV_CPSS_DXCH_LPM_TCAM_IP_MC_GROUP_E;

    switch (ipLttEntryPtr->routeEntryMethod)
    {
        case PRV_CPSS_DXCH_LPM_ENTRY_TYPE_ECMP_E:
        case PRV_CPSS_DXCH_LPM_ENTRY_TYPE_QOS_E:
        case PRV_CPSS_DXCH_LPM_ENTRY_TYPE_REGULAR_E:
            break;
        default:
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    switch (ipLttEntryPtr->ipv6McGroupScopeLevel)
    {
        case CPSS_IPV6_PREFIX_SCOPE_LINK_LOCAL_E:
        case CPSS_IPV6_PREFIX_SCOPE_SITE_LOCAL_E:
        case CPSS_IPV6_PREFIX_SCOPE_UNIQUE_LOCAL_E:
        case CPSS_IPV6_PREFIX_SCOPE_GLOBAL_E:
            break;
        default:
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    for(i=0; i <  ipTcamShadowPtr->shadowDevList.ipShareDevNum; i++)
    {
        devNum = ipTcamShadowPtr->shadowDevList.ipShareDevs[i];
        if (0 == PRV_CPSS_DXCH_LION_FAMILY_CHECK_MAC(devNum))
        {
            if (ipLttEntryPtr->blockSize > CPSS_DXCH_IP_MAX_ECMP_QOS_GROUP)
                CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
        }
        else
        {
            if(ipLttEntryPtr->routeEntryMethod == PRV_CPSS_DXCH_LPM_ENTRY_TYPE_QOS_E)
            {
                if (ipLttEntryPtr->blockSize > CPSS_DXCH_IP_MAX_LION_QOS_GROUP)
                    CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
            }
            else
            {
                if (ipLttEntryPtr->blockSize > CPSS_DXCH_IP_MAX_LION_ECMP_GROUP)
                    CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
            }

        }
        if (ipLttEntryPtr->routeEntryBaseMemAddr >= PRV_CPSS_DXCH_PP_MAC(devNum)->fineTuning.tableSize.routerNextHop)
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }


    lclIpGroup = (ipGroupPrefix == 0)?
        cpssDxChDefaultMcIpAddr[protocolStack] : ipGroupPtr;

    lclIpGroupPrefix = (ipGroupPrefix == 0)?
        &cpssDxChDefaultMcIpPrefixLen[protocolStack] : &ipGroupPrefix;

    if (protocolStack == CPSS_IP_PROTOCOL_IPV4_E)
    {
        grEntryType =PRV_CPSS_DXCH_LPM_TCAM_ENTRIES_HANDLER_IPV4_MC_GROUP_ENTRY_E;
        srcEntryType =PRV_CPSS_DXCH_LPM_TCAM_ENTRIES_HANDLER_IPV4_MC_SOURCE_ENTRY_E;
        entryTypeAmount.entryType=CPSS_DXCH_TCAM_MANAGER_SINGLE_ENTRY_E;
    }
    else
    {
        grEntryType = PRV_CPSS_DXCH_LPM_TCAM_ENTRIES_HANDLER_IPV6_MC_GROUP_ENTRY_E;
        srcEntryType = PRV_CPSS_DXCH_LPM_TCAM_ENTRIES_HANDLER_IPV6_MC_SOURCE_ENTRY_E;

        if (ipTcamShadowPtr->shadowType == PRV_CPSS_DXCH_LPM_TCAM_XCAT_POLICY_BASED_ROUTING_SHADOW_E)
        {
            entryTypeAmount.entryType=CPSS_DXCH_TCAM_MANAGER_SINGLE_ENTRY_E;
        }
        else
        {
            entryTypeAmount.entryType = CPSS_DXCH_TCAM_MANAGER_QUAD_ENTRY_E;
        }
    }
    entryTypeAmount.amount = 1;
    /* Check if the given virtual router already exists */
    vrEntryTmp.vrId = vrId;
    vrEntryPtr = prvCpssSlSearch(ipTcamShadowPtr->vrSl,&vrEntryTmp);
    if (vrEntryPtr == NULL)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_FOUND, LOG_ERROR_NO_MSG);
    }

    /* internally, bit 15 in vrId indicates shared virtual router */
    if ((ipTcamShadowPtr->sharedVrExist == GT_TRUE) && (vrId == ipTcamShadowPtr->sharedVrId))
        vrId |= BIT_15;

    if (vrEntryPtr->ipMcTrie[protocolStack] == NULL)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_SUPPORTED, LOG_ERROR_NO_MSG);
    }

    /*osSemWait(ipTcamShadowPtr->ipUnitSem,OS_WAIT_FOREVER);*/
    mcSrcNode = mcGroupNode = mcDefGroupNode = NULL;
    pSrcNode = NULL;

    pRootTrie = vrEntryPtr->ipMcTrie[protocolStack];

    ipMcTcamFuncs = &ipTcamShadowPtr->ipMcTcamFuncsPtrs[protocolStack];

    exactMatchPrefixLen = (GT_U8)((protocolStack == CPSS_IP_PROTOCOL_IPV4_E) ?
                                                                    32 : 128);

    /* search the group  & source */
    pGroupNode = prvCpssDxChLpmTcamPatTrieSearch(pRootTrie,lclIpGroup,
                                                 *lclIpGroupPrefix,NULL,
                                                 (GT_PTR)&mcGroupNode);


    if (pGroupNode != NULL)
        groupFoundHaveAPointer2Def = mcGroupNode->isAPointer2Def;

    if ((pGroupNode != NULL) && (ipSrcPrefix != 0))
    {
        /* the group already exists use the trie from there */
        pSrcTrie = mcGroupNode->ipMcSrcTrie;

        if(pSrcTrie != NULL)
        {
            pSrcNode = prvCpssDxChLpmTcamPatTrieSearch(pSrcTrie,ipSrcPtr,
                                               ipSrcPrefix,NULL,(GT_PTR)&mcSrcNode);
        }
    }

    /* check for override */
    if(((ipSrcPrefix == 0) && (pGroupNode != NULL) && (mcGroupNode->isAPointer2Def !=1)) ||
       (pSrcNode != NULL))
    {
        if ((overwrite == GT_FALSE) &&
            /* overwrite can always be done on default route*/
            (ipGroupPrefix != 0))
        {
            /*osSemSignal(ipTcamShadowPtr->ipUnitSem);*/
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_ALREADY_EXIST, LOG_ERROR_NO_MSG);
        }

        overwrite = GT_TRUE; /* just to make sure this is an override
                               (default route situation)*/

        /* get the current ltt entry */
        if (pSrcNode != NULL)
        {
            lclIpLttEntryPtr = mcSrcNode->lttEntryPtr;
        }
        else
        {
            lclIpLttEntryPtr = mcGroupNode->lttEntryPtr;
        }
    }

    if ( (pGroupNode == NULL) && (ipSrcPrefix == 0))
    {
        /* new group only  is going to be added */
        /* check counter*/
        prefixNumber = 1;
        ipMode = CPSS_IP_MULTICAST_E;
        mcEntryType = PRV_CPSS_DXCH_LPM_TCAM_IP_MC_GROUP_E;
    }else
    if ( (pGroupNode == NULL) && (ipSrcPrefix != 0) )
    {
        /* new G,S is going to be added: 2 prefixes */
        /*check counters */
        prefixNumber = 2;
        ipMode = CPSS_IP_MULTICAST_E;
        mcEntryType = PRV_CPSS_DXCH_LPM_TCAM_IP_MC_GROUP_AND_SOURCE_E;
    }else
    if ((pGroupNode != NULL) && (ipSrcPrefix != 0) && (overwrite == GT_FALSE))
    {
        /* new source is going to be added */
        /*check counter */
        prefixNumber = 1;
        ipMode = CPSS_IP_MULTICAST_E;
        mcEntryType = PRV_CPSS_DXCH_LPM_TCAM_IP_MC_SOURCE_E;
    }
    ret = prvCpssDxChLpmTcamFreeSpaceCountersCheck(prefixNumber,ipMode,mcEntryType,
                                                   protocolStack,ipTcamShadowPtr);
    if (ret != GT_OK)
    {
        return ret;
    }

    /* now check availble space */
    if(pGroupNode == NULL)
    {
        /* this a new group , check if there is space */

        /* for Ch2 device, a group cannot sit in index 0, so if index 0 is available,
           set it as occupied just for this and release it after that */
        if (ipTcamShadowPtr->shadowType == PRV_CPSS_DXCH_LPM_TCAM_CHEETAH2_SHADOW_E)
        {
            ret = prvCpssDxChLpmTcamEntriesHandlerSetRealIdxAsAlloc(vrEntryPtr->tcamEntriesHandler,
                                                                    grEntryType,
                                                                    0,
                                                                    &zeroRule);
            if (ret != GT_OK)
                return ret;
        }
        entryTypeAmount.amount = 1;
        ret =  prvCpssDxChLpmTcamDispatchCheckAvailableMem(ipTcamShadowPtr,
                                                           grEntryType,
                                                           1,
                                                           &entryTypeAmount,
                                                           1);

        if (ipTcamShadowPtr->shadowType == PRV_CPSS_DXCH_LPM_TCAM_CHEETAH2_SHADOW_E)
        {
            /* regardless of the result the allocated rule at index 0 must be released */
            if (zeroRule != NULL)
            {
                ret2 = prvCpssDxChLpmTcamEntriesHandlerFree(vrEntryPtr->tcamEntriesHandler,
                                                            grEntryType,
                                                            zeroRule);
                if (ret2 != GT_OK)
                    return ret2;
            }
        }

        /* check the result of the available memory check */
        if (ret != GT_OK)
        {
            /*osSemSignal(ipTcamShadowPtr->ipUnitSem);*/
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_OUT_OF_PP_MEM, LOG_ERROR_NO_MSG);
        }
    }

    /* move on to the source */
    if ((ipSrcPrefix != 0) && (pSrcNode == NULL))
    {
        ret =  prvCpssDxChLpmTcamDispatchCheckAvailableMem(ipTcamShadowPtr,
                                                           srcEntryType,
                                                           1,
                                                           &entryTypeAmount,
                                                           1);
        if (ret != GT_OK)
        {
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_OUT_OF_PP_MEM, LOG_ERROR_NO_MSG);
        }
    }

    /* ok we are set, now start with the memory allocations */

    /* There is no initialized LPM structure for this ip-group, initialize  */
    /* an LPM structure.*/
    if(pGroupNode == NULL)
    {
        /* create src trie */
        if(prvCpssDxChLpmTcamPatTrieInit(ipTcamShadowPtr,&pSrcTrie) != GT_OK)
        {
            /*osSemSignal(ipTcamShadowPtr->ipUnitSem);*/
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_FAIL, LOG_ERROR_NO_MSG);
        }
        /* Make root node of source trie as dummy node */
        cpssOsMemSet(pSrcTrie->ipAddr,0,sizeof(GT_U8) * PRV_CPSS_DXCH_LPM_TCAM_PAT_TRIE_MAX_PREFIX_LEN_IN_BYTES_CNS);
        pSrcTrie->size = 1;
        isSrcTrieBeenCreated = GT_TRUE;

        /* allocate */
        mcGroupNode = (PRV_CPSS_DXCH_LPM_TCAM_MC_GROUP_SRC_NODE_STC*)cpssOsMalloc(
            sizeof(PRV_CPSS_DXCH_LPM_TCAM_MC_GROUP_SRC_NODE_STC));
        if (mcGroupNode == NULL)
        {
            /*osSemSignal(ipTcamShadowPtr->ipUnitSem);*/
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_OUT_OF_CPU_MEM, LOG_ERROR_NO_MSG);
        }

        rollBackFreeArray[rollBackIndex] = mcGroupNode;
        rollBackIndex++;

        /* if this a G,S entry and there is no G , create one that points to
           the virtual route default mc route */
        if (ipSrcPrefix != 0)
        {
            /* search the defualt group */
            if (prvCpssDxChLpmTcamPatTrieSearch(pRootTrie,
                                                cpssDxChDefaultMcIpAddr[protocolStack],
                                                cpssDxChDefaultMcIpPrefixLen[protocolStack],
                                                NULL,(GT_PTR)&mcDefGroupNode) == NULL)
            {
                /* this means the vr doesn't the default, can't be !!!*/
                /*osSemSignal(ipTcamShadowPtr->ipUnitSem);*/
                CPSS_LOG_ERROR_AND_RETURN_MAC(GT_FAIL, LOG_ERROR_NO_MSG);
            }

            cpssOsMemCpy(mcGroupNode,mcDefGroupNode,
                         sizeof(PRV_CPSS_DXCH_LPM_TCAM_MC_GROUP_SRC_NODE_STC));

            /* indicate this is a fake group */
            mcGroupNode->isAPointer2Def = GT_TRUE;
        }

        mcGroupNode->ipMcSrcTrie = pSrcTrie;
    }

    if (ipSrcPrefix == 0)
    {
        /* if this a group writing only ! then the entry isn't a fake one
           that points to the default */
        mcGroupNode->isAPointer2Def = GT_FALSE;
    }

    /* first allocate the ltt info if needed and copy */
    if (lclIpLttEntryPtr == NULL)
    {
        lclIpLttEntryPtr = cpssOsMalloc(sizeof(PRV_CPSS_DXCH_LPM_ROUTE_ENTRY_POINTER_STC));
        if (lclIpLttEntryPtr == NULL)
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_OUT_OF_CPU_MEM, LOG_ERROR_NO_MSG);
    }

    cpssOsMemCpy(lclIpLttEntryPtr,ipLttEntryPtr,
                 sizeof(PRV_CPSS_DXCH_LPM_ROUTE_ENTRY_POINTER_STC));

    if (ipSrcPrefix != 0)
    {
        if (pSrcNode == NULL)
        {
            /* allocate */
            mcSrcNode = (PRV_CPSS_DXCH_LPM_TCAM_MC_GROUP_SRC_NODE_STC*)cpssOsMalloc(
                sizeof(PRV_CPSS_DXCH_LPM_TCAM_MC_GROUP_SRC_NODE_STC));
            if (mcSrcNode == NULL)
            {
                if (isSrcTrieBeenCreated)
                    prvCpssDxChLpmTcamPatTrieDestroy(ipTcamShadowPtr,pSrcTrie);
                for (i = 0 ; i < rollBackIndex ; i++)
                    cpssOsFree(rollBackFreeArray[i]);

                /*osSemSignal(ipTcamShadowPtr->ipUnitSem);*/
                CPSS_LOG_ERROR_AND_RETURN_MAC(GT_OUT_OF_CPU_MEM, LOG_ERROR_NO_MSG);
            }

            rollBackFreeArray[rollBackIndex] = mcSrcNode;
            rollBackIndex++;

            /* this will indicate that this a src node */
            mcSrcNode->ipMcSrcTrie = NULL;
        }

        /* set the LTT entry we are now writing - of the Source*/
        mcSrcNode->lttEntryPtr = lclIpLttEntryPtr;
    }
    else
    {
        /* set the LTT entry we are now writing - of the Group*/
        mcGroupNode->lttEntryPtr = lclIpLttEntryPtr;
    }

    /* move on to inserting to the lpm trie - first the group
      to get the G-index*/
    if ((ret == GT_OK) &&
        ((pGroupNode == NULL) ||
         /* rewrite also if it's an overwrite of the group.*/
         ((overwrite == GT_TRUE) && (ipSrcPrefix == 0)) ||
         /* rewrite if it's an overwrite of G* that was defined implicitly.*/
         ((groupFoundHaveAPointer2Def == GT_TRUE) && (ipSrcPrefix == 0))))
    {
        /* first check if we have a rule */
        if (ruleIdxArr != NULL)
        {
            ret = prvCpssDxChLpmTcamDispatchSetEntryHsuInfo(ipTcamShadowPtr,
                                                            grEntryType,
                                                            ruleIdxArr[0],
                                                            &tcamRulePtr);
            if (ret != GT_OK)
                return ret;
            ruleNodePtr = (PRV_CPSS_DXCH_LPM_TCAM_RULE_NODE_STC*)tcamRulePtr;
        }
        else
        {
            /* for Ch2 device, a group cannot sit in index 0, so if index 0 is available,
               set it as occupied just for this and release it after that */
            if (ipTcamShadowPtr->shadowType == PRV_CPSS_DXCH_LPM_TCAM_CHEETAH2_SHADOW_E)
            {
                ret = prvCpssDxChLpmTcamEntriesHandlerSetRealIdxAsAlloc(vrEntryPtr->tcamEntriesHandler,
                                                                        grEntryType,
                                                                        0,
                                                                        &zeroRule);
                if (ret != GT_OK)
                    return ret;
            }

        }

        isExactMatch = (*lclIpGroupPrefix == exactMatchPrefixLen)? GT_TRUE : GT_FALSE;
        if (ret == GT_OK)
            ret = prvCpssDxChLpmTcamPatTrieInsert(vrId,
                                                  pRootTrie,
                                                  ipTcamShadowPtr,
                                                  grEntryType,
                                                  lclIpGroup,
                                                  *lclIpGroupPrefix,
                                                  0,
                                                  mcGroupNode,
                                                  isExactMatch,
                                                  GT_FALSE,
                                                  defragEnable,
                                                  ruleNodePtr,
                                                  ipMcTcamFuncs);

        if (ipTcamShadowPtr->shadowType == PRV_CPSS_DXCH_LPM_TCAM_CHEETAH2_SHADOW_E)
        {
            /* now if we set the 0 as allocate , free it*/
            if ((ruleIdxArr == NULL) && (zeroRule != NULL))
            {
                ret = prvCpssDxChLpmTcamEntriesHandlerFree(vrEntryPtr->tcamEntriesHandler,
                                                           grEntryType,
                                                           zeroRule);
                if(ret != GT_OK)
                    return ret;
            }
        }
        if (ret == GT_OK)
        {
            /* increment mc group counter */
            if (pGroupNode == NULL)
            {
                ipTcamShadowPtr->allocatedIpPrefixCounters.numOfAllocatedIpPrefixes[protocolStack]++;
            }
        }
    }

    /* source now */
    if ((ret == GT_OK) && (ipSrcPrefix != 0))
    {
        /* first check if we have a rule */
        ruleNodePtr = NULL;
        if (ruleIdxArr != NULL)
        {
            ret = prvCpssDxChLpmTcamDispatchSetEntryHsuInfo(ipTcamShadowPtr,
                                                            srcEntryType,
                                                            ruleIdxArr[1],
                                                            &tcamRulePtr);
            if (ret != GT_OK)
                return ret;
            ruleNodePtr = (PRV_CPSS_DXCH_LPM_TCAM_RULE_NODE_STC*)tcamRulePtr;
        }
        /* set the g-index , now that we have it*/
        mcSrcNode->gIndex = mcGroupNode->gIndex;

        isExactMatch = (ipSrcPrefix == exactMatchPrefixLen)? GT_TRUE : GT_FALSE;
        if (ret == GT_OK)
        {
            ret = prvCpssDxChLpmTcamPatTrieInsert(vrId,
                                                  pSrcTrie,
                                                  ipTcamShadowPtr,
                                                  srcEntryType,
                                                  ipSrcPtr,
                                                  ipSrcPrefix,
                                                  0,
                                                  mcSrcNode,
                                                  isExactMatch,
                                                  GT_FALSE,
                                                  defragEnable,
                                                  ruleNodePtr,
                                                  ipMcTcamFuncs);
            if (ret == GT_OK)
            {
                if (pSrcNode == NULL)
                {
                    /* increment appropriate source counter */
                    if (protocolStack == CPSS_IP_PROTOCOL_IPV6_E )
                    {
                        ipTcamShadowPtr->allocatedIpPrefixCounters.numOfAllocatedIpPrefixes[protocolStack]++;
                    }
                    else
                    {
                        ipTcamShadowPtr->allocatedIpPrefixCounters.numOfAllocatedIpv4McSourcePrefixes++;
                    }
                }
            }
        }
    }
    if (ret != GT_OK)
    {
        if (isSrcTrieBeenCreated)
            prvCpssDxChLpmTcamPatTrieDestroy(ipTcamShadowPtr,pSrcTrie);
        for (i = 0 ; i < rollBackIndex ; i++)
            cpssOsFree(rollBackFreeArray[i]);
    }

    /* if we overwritten the default MC, we need to overwrite     */
    /* all the implicit (*,G) meaning group defaults that points  */
    /* to the default MC                                          */
    if ((overwrite == GT_TRUE) && (ipSrcPrefix == 0))
    {
        ipShareDevs     = ipTcamShadowPtr->workDevListPtr->ipShareDevs;
        numOfShareDevs  = ipTcamShadowPtr->workDevListPtr->ipShareDevNum;

        /* search the default group */
        if (prvCpssDxChLpmTcamPatTrieSearch(pRootTrie,
                                            cpssDxChDefaultMcIpAddr[protocolStack],
                                            cpssDxChDefaultMcIpPrefixLen[protocolStack],
                                            NULL,(GT_PTR)&mcDefGroupNode) == NULL)
        {
            /* this means the vr doesn't have a default, can't be !!!*/
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_FAIL, LOG_ERROR_NO_MSG);
        }

        /* check if we are dealing with default MC group */
        if ((mcGroupNode == mcDefGroupNode) || cpssOsMemCmp(mcGroupNode,mcDefGroupNode,
                         sizeof(PRV_CPSS_DXCH_LPM_TCAM_MC_GROUP_SRC_NODE_STC)) == 0)
        {
            pRootTrie = vrEntryPtr->ipMcTrie[protocolStack];
            cpssOsMemSet(traverseIpGroup,0,sizeof(GT_U8)*16);
            traverseIpGroupPrefix = 0;

            /* go over all MC groups */
            while ((prvCpssDxChLpmTcamPatTrieGetNext(pRootTrie,traverseIpGroup,
                                                     &traverseIpGroupPrefix,
                                                     (GT_PTR)&mcGroupNodePtr)) != NULL)
            {
                /* if the group is implicit group default overwrite it */
                if (mcGroupNodePtr->isAPointer2Def == GT_TRUE)
                {
                    cpssOsMemCpy(mcGroupNodePtr->lttEntryPtr,lclIpLttEntryPtr,
                                 sizeof(PRV_CPSS_DXCH_LPM_ROUTE_ENTRY_POINTER_STC));

                    if (ipTcamShadowPtr->tcamManagementMode == PRV_CPSS_DXCH_LPM_TCAM_MANAGEMENT_TCAM_ENTRIES_HANDLER_MODE_E)
                    {
                        /* devices working with TCAM entries handler are column based TCAM */
                        mcGroupRow    = mcGroupNodePtr->gIndex % tcamRows;
                        mcGroupColumn = mcGroupNodePtr->gIndex / tcamRows;
                    }
                    else
                    {
                        /* devices working with TCAM manager are row based TCAM */
                        mcGroupRow    = mcGroupNodePtr->gIndex / 4;
                        mcGroupColumn = mcGroupNodePtr->gIndex % 4;
                    }
                    /* go over all the share device and write to the device */
                    for (i = 0; i < numOfShareDevs; i++)
                    {
                        /* set the lookup translation table entry */
                        ret = prvCpssDxChLpmTcamLttWrite(ipShareDevs[i],
                                                         mcGroupRow,
                                                         mcGroupColumn,
                                                         lclIpLttEntryPtr);
                        if (ret != GT_OK)
                        {
                            return ret;
                        }
                    }
                }
            }
        }
    }

    /*osSemSignal(ipTcamShadowPtr->ipUnitSem);*/

    return ret;
}

/*******************************************************************************
* prvCpssDxChLpmTcamDeleteIpMcEntry
*
* DESCRIPTION:
*       To delete a particular mc route entry and all its MLLs.
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; xCat3; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond.
*
* INPUTS:
*       vrId            - The virtual private network identifier.
*       ipGroupPtr      - (pointer to)The IP multicast group address.
*       ipGroupPrefix   - The number of bits that are actual valid in,
*                         the ipGroup. (CIDR like)
*       ipSrcPtr        - (pointer to)the root address for source base multi tree protocol.
*       ipSrcPrefix     - The number of bits that are actual valid in,
*                         the ipSrc. (CIDR like)
*       protocolStack  - type of ip protocol stack to work on.
*       ipTcamShadowPtr - the shadow pointer we are working on
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success, or
*       GT_ERROR                 - if the virtual router does not exist, or
*       GT_NOT_FOUND             - if the (ipGroup,prefix) does not exist, or
*       GT_OUT_OF_CPU_MEM if failed to allocate CPU memory, or
*       GT_OUT_OF_PP_MEM if failed to allocate PP memory, or
*       GT_FAIL                  - otherwise.
*       GT_NOT_SUPPORTED         - the request is not supported.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvCpssDxChLpmTcamDeleteIpMcEntry
(
    IN GT_U32                            vrId,
    IN GT_U8                             *ipGroupPtr,
    IN GT_U32                            ipGroupPrefix,
    IN GT_U8                             *ipSrcPtr,
    IN GT_U32                            ipSrcPrefix,
    IN CPSS_IP_PROTOCOL_STACK_ENT        protocolStack,
    IN PRV_CPSS_DXCH_LPM_TCAM_SHADOW_STC *ipTcamShadowPtr
)
{
    GT_STATUS           ret = GT_OK;
    PRV_CPSS_DXCH_LPM_TCAM_PAT_TRIE_FUNCS_OBJ_STC *ipMcTcamFuncs;
    PRV_CPSS_DXCH_LPM_TCAM_PAT_TRIE_NODE_STC     *pRootTrie,*pSrcTrie;
    PRV_CPSS_DXCH_LPM_TCAM_PAT_TRIE_NODE_STC     *pGroupNode;
    PRV_CPSS_DXCH_LPM_TCAM_MC_GROUP_SRC_NODE_STC *mcGroupNode,*mcSrcNode;
    PRV_CPSS_DXCH_LPM_TCAM_MC_GROUP_SRC_NODE_STC *mcDefGroupNode;
    GT_BOOL     deleteEmptyGroup = GT_FALSE;   /* if group is empty and needs to be deleted */


    PRV_CPSS_DXCH_LPM_TCAM_VR_TBL_BLOCK_STC *vrEntryPtr,vrEntryTmp;
    PRV_CPSS_DXCH_LPM_TCAM_ENTRIES_HANDLER_ENTRY_TYPE_ENT  grEntryType;
    PRV_CPSS_DXCH_LPM_TCAM_ENTRIES_HANDLER_ENTRY_TYPE_ENT  srcEntryType;

    GT_U8  *lclIpGroup;
    GT_U32 *lclIpGroupPrefix;
    GT_U32 defaultPrefixLen;

    lclIpGroup = (ipGroupPrefix == 0)?
        cpssDxChDefaultMcIpAddr[protocolStack] : ipGroupPtr;

    lclIpGroupPrefix = (ipGroupPrefix == 0)?
        &cpssDxChDefaultMcIpPrefixLen[protocolStack] : &ipGroupPrefix;


    if (protocolStack == CPSS_IP_PROTOCOL_IPV4_E)
    {
        grEntryType =PRV_CPSS_DXCH_LPM_TCAM_ENTRIES_HANDLER_IPV4_MC_GROUP_ENTRY_E;
        srcEntryType =PRV_CPSS_DXCH_LPM_TCAM_ENTRIES_HANDLER_IPV4_MC_SOURCE_ENTRY_E;
    }
    else
    {
        grEntryType = PRV_CPSS_DXCH_LPM_TCAM_ENTRIES_HANDLER_IPV6_MC_GROUP_ENTRY_E;
        srcEntryType = PRV_CPSS_DXCH_LPM_TCAM_ENTRIES_HANDLER_IPV6_MC_SOURCE_ENTRY_E;
    }

    /* Check if the given virtual router already exists */
    vrEntryTmp.vrId = vrId;
    vrEntryPtr = prvCpssSlSearch(ipTcamShadowPtr->vrSl,&vrEntryTmp);
    if (vrEntryPtr == NULL)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_FOUND, LOG_ERROR_NO_MSG);
    }

    /* internally, bit 15 in vrId indicates shared virtual router */
    if ((ipTcamShadowPtr->sharedVrExist == GT_TRUE) && (vrId == ipTcamShadowPtr->sharedVrId))
        vrId |= BIT_15;

    if (vrEntryPtr->ipMcTrie[protocolStack] == NULL)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_SUPPORTED, LOG_ERROR_NO_MSG);
    }

    mcSrcNode = mcGroupNode = mcDefGroupNode = NULL;

    pRootTrie = vrEntryPtr->ipMcTrie[protocolStack];

    ipMcTcamFuncs = &ipTcamShadowPtr->ipMcTcamFuncsPtrs[protocolStack];

    /* search the group  & source */
    pGroupNode = prvCpssDxChLpmTcamPatTrieSearch(pRootTrie,lclIpGroup,
                                                 *lclIpGroupPrefix,NULL,
                                                 (GT_PTR)&mcGroupNode);
    if (pGroupNode == NULL)
    {
        /*osSemSignal(ipTcamShadowPtr->ipUnitSem);*/
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_FOUND, LOG_ERROR_NO_MSG);
    }

    /* the following means we are trying to remove a fake (pointing to the
       default) group that doesn't really exists - allow it only if there
       are no sources left for it.*/
    if ((ipSrcPrefix == 0) &&
         (mcGroupNode->isAPointer2Def == GT_TRUE) &&
         (prvCpssDxChLpmTcamPatTrieIsEmpty(mcGroupNode->ipMcSrcTrie) == GT_FALSE))
    {
        /*osSemSignal(ipTcamShadowPtr->ipUnitSem);*/
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_STATE, LOG_ERROR_NO_MSG);
    }


    if (ipSrcPrefix == 0)
    {
        /* check if we are try to delete a "real" group that still has sources*/
        if ((mcGroupNode->isAPointer2Def == GT_FALSE) &&
            (prvCpssDxChLpmTcamPatTrieIsEmpty(mcGroupNode->ipMcSrcTrie) == GT_FALSE))
        {
            /* ok it means we have to switch the "real" group to a fake one
               that holds the default */

            /* search the default group*/
            switch(protocolStack)
            {
            case CPSS_IP_PROTOCOL_IPV4_E:
                defaultPrefixLen = cpssDxChDefaultMcIpPrefixLen[CPSS_IP_PROTOCOL_IPV4_E];
                break;
            case CPSS_IP_PROTOCOL_IPV6_E:
                defaultPrefixLen = cpssDxChDefaultMcIpPrefixLen[CPSS_IP_PROTOCOL_IPV6_E];
                break;
            default:
                CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
            }
            if (prvCpssDxChLpmTcamPatTrieSearch(pRootTrie,
                                                lclIpGroup,
                                                defaultPrefixLen,
                                                NULL,
                                                (GT_PTR)&mcDefGroupNode) == NULL)
            {
                /* this means the vr doesn't the default !!!*/
                /*osSemSignal(ipTcamShadowPtr->ipUnitSem);*/
                CPSS_LOG_ERROR_AND_RETURN_MAC(GT_FAIL, LOG_ERROR_NO_MSG);
            }

            mcGroupNode->lttEntryPtr = mcDefGroupNode->lttEntryPtr;
            mcGroupNode->isAPointer2Def = GT_TRUE;

            ret = prvCpssDxChLpmTcamPatTrieInsert(vrId,
                                                  pRootTrie,
                                                  ipTcamShadowPtr,
                                                  grEntryType,
                                                  lclIpGroup,
                                                  *lclIpGroupPrefix,
                                                  0,
                                                  mcGroupNode,
                                                  GT_TRUE/* doesn't matter ,it's an overwrite*/,
                                                  GT_FALSE,
                                                  GT_FALSE /* defragEnable */,
                                                  NULL,
                                                  ipMcTcamFuncs);
        }
        else
        {
            /* the group is empty, mark it to be deleted */
            deleteEmptyGroup = GT_TRUE;
        }
    }
    else
    {
        /* the group already exists use the trie from there */
        pSrcTrie = mcGroupNode->ipMcSrcTrie;

        if(pSrcTrie != NULL)
        {
            ret = prvCpssDxChLpmTcamPatTrieDel(vrId,
                                               pSrcTrie,
                                               ipTcamShadowPtr,
                                               srcEntryType,
                                               ipSrcPtr,
                                               ipSrcPrefix,
                                               0,
                                               (GT_PTR)&mcSrcNode,
                                               ipMcTcamFuncs);

            /*if delete was OK we need to decrement counter */
            if (ret == GT_OK)
            {
                if (protocolStack == CPSS_IP_PROTOCOL_IPV6_E)
                {
                    /*this case is an error !*/
                    if (ipTcamShadowPtr->allocatedIpPrefixCounters.numOfAllocatedIpPrefixes[protocolStack] == 0)
                    {
                        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_FAIL, LOG_ERROR_NO_MSG);
                    }
                    ipTcamShadowPtr->allocatedIpPrefixCounters.numOfAllocatedIpPrefixes[protocolStack] --;
                }
                else
                {
                    if (ipTcamShadowPtr->allocatedIpPrefixCounters.numOfAllocatedIpv4McSourcePrefixes == 0)
                    {
                        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_FAIL, LOG_ERROR_NO_MSG);
                    }
                    ipTcamShadowPtr->allocatedIpPrefixCounters.numOfAllocatedIpv4McSourcePrefixes  --;
                }
            }
        }
        else
        {
            ret = GT_OK;
        }

        if (ret != GT_OK)
        {
            /*osSemSignal(ipTcamShadowPtr->ipUnitSem);*/
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_FOUND, LOG_ERROR_NO_MSG);
        }

        /* now free the node + data*/
        if (mcSrcNode != NULL)
        {
            cpssOsFree(mcSrcNode->lttEntryPtr);
            cpssOsFree(mcSrcNode);
        }

        /* check if this source was last source in he group and the   */
        /* group entry is "fake" one (meaning added implicitly by the */
        /* code); if so, delete the group                             */
        if ((mcGroupNode->isAPointer2Def == GT_TRUE) &&
            (prvCpssDxChLpmTcamPatTrieIsEmpty(mcGroupNode->ipMcSrcTrie) == GT_TRUE))
        {
            deleteEmptyGroup = GT_TRUE;
        }
    }

    /* if the group marked to be deleted, delete it */
    if (deleteEmptyGroup == GT_TRUE)
    {
        /* we are deleting a group with no sources */

        /* first destroy the src trie - it is empty */
        ret = prvCpssDxChLpmTcamPatTrieDestroy(ipTcamShadowPtr, mcGroupNode->ipMcSrcTrie);
        if (ret != GT_OK)
        {
            /*osSemSignal(ipTcamShadowPtr->ipUnitSem);*/
            return(ret);
        }

        ret = prvCpssDxChLpmTcamPatTrieDel(vrId,
                                           pRootTrie,
                                           ipTcamShadowPtr,
                                           grEntryType,
                                           lclIpGroup,
                                           *lclIpGroupPrefix,
                                           0,
                                           NULL,
                                           ipMcTcamFuncs);
        if (ret != GT_OK)
        {
            /*osSemSignal(ipTcamShadowPtr->ipUnitSem);*/
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_FOUND, LOG_ERROR_NO_MSG);
        }
        /*this case is an error !*/
        if (ipTcamShadowPtr->allocatedIpPrefixCounters.numOfAllocatedIpPrefixes[protocolStack] == 0)
        {
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_FAIL, LOG_ERROR_NO_MSG);
        }
        ipTcamShadowPtr->allocatedIpPrefixCounters.numOfAllocatedIpPrefixes[protocolStack]--;

        if (mcGroupNode->isAPointer2Def == GT_FALSE)
        {
            /* this is a real entry delete the data */
            cpssOsFree(mcGroupNode->lttEntryPtr);
        }
        /* now free the node  */
        cpssOsFree(mcGroupNode);
    }

    /*osSemSignal(ipTcamShadowPtr->ipUnitSem);*/
    return(ret);
}

/*******************************************************************************
* CPSS_OS_FREE_FUNC
*
* DESCRIPTION:
*       De-allocates or frees a specified memory block.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       memblock - previously allocated memory block to be freed
*
* OUTPUTS:
*       None
*
* RETURNS:
*       None
*
* COMMENTS:
*       Usage of this function is NEVER during initialization.
*
*******************************************************************************/
void prvCpssDxchIpTcamMcGroupSrcNodeStcFreeFunc
(
    IN GT_PTR memblock
)
{
    PRV_CPSS_DXCH_LPM_TCAM_MC_GROUP_SRC_NODE_STC *mcGroupSrcNode;

    mcGroupSrcNode = memblock;

    if (mcGroupSrcNode->ipMcSrcTrie == NULL)
    {
        /* this is a src node , del the data and the node */
        cpssOsFree(mcGroupSrcNode->lttEntryPtr);
        cpssOsFree(memblock);
    }
    else
    {
        /* this is a group node - delete data if not a pointer to default*/
        if (mcGroupSrcNode->isAPointer2Def == GT_FALSE)
        {
            cpssOsFree(mcGroupSrcNode->lttEntryPtr);
        }
        cpssOsFree(memblock);
    }
    return;
}

/*******************************************************************************
* prvCpssDxChLpmTcamFlushIpMcEntries
*
* DESCRIPTION:
*       flushes the multicast IP Routing table and stays with the default entry
*       only.
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; xCat3; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond.
*
* INPUTS:
*       vrId           - The virtual private network identifier.
*       protocolStack  - type of ip protocol stack to work on.
*       ipTcamShadowPtr - the shadow pointer we are working on.
*       flushDefault   - whether to flush the default as well
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_NOT_SUPPORTED         - the request is not supported.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvCpssDxChLpmTcamFlushIpMcEntries
(
    IN GT_U32                            vrId,
    IN CPSS_IP_PROTOCOL_STACK_ENT        protocolStack,
    IN PRV_CPSS_DXCH_LPM_TCAM_SHADOW_STC *ipTcamShadowPtr,
    IN GT_BOOL                           flushDefault
)
{
    GT_STATUS ret;
    GT_U8 ipGroup[16];     /* The IP address to be deleted at each loop*/
                                /* when flushing IP-group addresses.        */

    GT_U32 ipGroupPrefix = 0;   /* The search IP address prefix to be       */
                                /* deleted.                                 */

    PRV_CPSS_DXCH_LPM_TCAM_PAT_TRIE_NODE_STC     *pRootTrie;
    PRV_CPSS_DXCH_LPM_TCAM_MC_GROUP_SRC_NODE_STC *mcGroupNode;
    PRV_CPSS_DXCH_LPM_TCAM_MC_GROUP_SRC_NODE_STC *mcDefGroupNode;

    PRV_CPSS_DXCH_LPM_TCAM_VR_TBL_BLOCK_STC *vrEntryPtr,vrEntryTmp;
    PRV_CPSS_DXCH_LPM_TCAM_PAT_TRIE_FUNCS_OBJ_STC *ipMcTcamFuncs;
    PRV_CPSS_DXCH_LPM_TCAM_ENTRIES_HANDLER_ENTRY_TYPE_ENT  grEntryType;
    PRV_CPSS_DXCH_LPM_TCAM_ENTRIES_HANDLER_ENTRY_TYPE_ENT  srcEntryType;
    PRV_CPSS_DXCH_LPM_TCAM_PAT_TRIE_NODE_STC          *nodePtr = NULL;
    GT_U32      numOfPrefixes = 0;
    GT_BOOL     isLastNode = GT_FALSE;


    if (protocolStack == CPSS_IP_PROTOCOL_IPV4_E)
    {
        grEntryType =PRV_CPSS_DXCH_LPM_TCAM_ENTRIES_HANDLER_IPV4_MC_GROUP_ENTRY_E;
        srcEntryType =PRV_CPSS_DXCH_LPM_TCAM_ENTRIES_HANDLER_IPV4_MC_SOURCE_ENTRY_E;
    }
    else
    {
        grEntryType = PRV_CPSS_DXCH_LPM_TCAM_ENTRIES_HANDLER_IPV6_MC_GROUP_ENTRY_E;
        srcEntryType = PRV_CPSS_DXCH_LPM_TCAM_ENTRIES_HANDLER_IPV6_MC_SOURCE_ENTRY_E;
    }

    ipMcTcamFuncs = &ipTcamShadowPtr->ipMcTcamFuncsPtrs[protocolStack];

    /* Check if the given virtual router already exists */
    vrEntryTmp.vrId = vrId;
    vrEntryPtr = prvCpssSlSearch(ipTcamShadowPtr->vrSl,&vrEntryTmp);
    if (vrEntryPtr == NULL)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_FOUND, LOG_ERROR_NO_MSG);
    }

    /* internally, bit 15 in vrId indicates shared virtual router */
    if ((ipTcamShadowPtr->sharedVrExist == GT_TRUE) && (vrId == ipTcamShadowPtr->sharedVrId))
        vrId |= BIT_15;

    if (vrEntryPtr->ipMcTrie[protocolStack] == NULL)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_SUPPORTED, LOG_ERROR_NO_MSG);
    }

    /*osSemWait(ipTcamShadowPtr->ipUnitSem,OS_WAIT_FOREVER);*/

    mcGroupNode = mcDefGroupNode = NULL;

    pRootTrie = vrEntryPtr->ipMcTrie[protocolStack];

    /* get the group  & source */
    while ((prvCpssDxChLpmTcamPatTrieGetNext(pRootTrie,ipGroup,
                                             &ipGroupPrefix,
                                             (GT_PTR)&mcGroupNode))
           != NULL)
    {

        if(mcGroupNode->ipMcSrcTrie != NULL)
        {
            /* calculate number of prefixes in src tree before delete */
            nodePtr = mcGroupNode->ipMcSrcTrie;
            while (isLastNode == GT_FALSE)
            {
                ret = prvCpssDxChLpmTcamPatTrieGetNextPostOrderTrieNode(&nodePtr,&isLastNode);
                if (ret != GT_OK)
                    return ret;
                /* count the valid nodes */
                if (nodePtr->pData != NULL)
                    numOfPrefixes++;
            }
            ret = prvCpssDxChLpmTcamPatTrieFlush(mcGroupNode->ipMcSrcTrie,
                                                 ipTcamShadowPtr,
                                                 srcEntryType,
                                                 ipMcTcamFuncs,
                                                 prvCpssDxchIpTcamMcGroupSrcNodeStcFreeFunc,
                                                 GT_TRUE);

            if (ret != GT_OK)
            {
                return ret;
            }
            else
            {
                if (protocolStack == CPSS_IP_PROTOCOL_IPV6_E)
                {
                    /*this case is an error !*/
                    if (ipTcamShadowPtr->allocatedIpPrefixCounters.numOfAllocatedIpPrefixes[protocolStack] < numOfPrefixes)
                    {
                        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_FAIL, LOG_ERROR_NO_MSG);
                    }
                    ipTcamShadowPtr->allocatedIpPrefixCounters.numOfAllocatedIpPrefixes[protocolStack] -= numOfPrefixes;
                }
                else
                {
                    if (ipTcamShadowPtr->allocatedIpPrefixCounters.numOfAllocatedIpv4McSourcePrefixes < numOfPrefixes)
                    {
                        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_FAIL, LOG_ERROR_NO_MSG);
                    }
                    ipTcamShadowPtr->allocatedIpPrefixCounters.numOfAllocatedIpv4McSourcePrefixes  -= numOfPrefixes;
                }
            }
            numOfPrefixes = 0;
            isLastNode = GT_FALSE;
        }
    }

    /* calculate number of prefixes in group tree before delete */
    nodePtr = pRootTrie;
    numOfPrefixes = 0;
    if (pRootTrie != NULL)
    {
        while (isLastNode == GT_FALSE)
        {
            ret = prvCpssDxChLpmTcamPatTrieGetNextPostOrderTrieNode(&nodePtr,&isLastNode);
            if (ret != GT_OK)
                return ret;
            /* count the valid nodes */
            if (nodePtr->pData != NULL)
                numOfPrefixes++;
        }
    }
    /* The calculated numOfPrefixes includes default */
    if (flushDefault == GT_FALSE)
    {
        numOfPrefixes--;
    }

    /* all the source trie's have been flushed, now flush the group's trie */

    ret = prvCpssDxChLpmTcamPatTrieFlush(pRootTrie,ipTcamShadowPtr,grEntryType,ipMcTcamFuncs,
                                         prvCpssDxchIpTcamMcGroupSrcNodeStcFreeFunc,
                                         flushDefault);
    if (ret == GT_OK)
    {
        if (ipTcamShadowPtr->allocatedIpPrefixCounters.numOfAllocatedIpPrefixes[protocolStack] == 0)
        {
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_FAIL, LOG_ERROR_NO_MSG);
        }
        ipTcamShadowPtr->allocatedIpPrefixCounters.numOfAllocatedIpPrefixes[protocolStack] -= numOfPrefixes;
    }

    /*osSemSignal(ipTcamShadowPtr->ipUnitSem);*/

    return ret;
}


/*******************************************************************************
* prvCpssDxChLpmTcamIpMcEntriesWriteToHw
*
* DESCRIPTION:
*       writes to the HW the multicast IP Routing table
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; xCat3; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond.
*
* INPUTS:
*       vrEntryPtr      - The virtual router entry pointer
*       protocolStack   - type of ip protocol stack to work on.
*       ipTcamShadowPtr - the shadow pointer we are working on
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
*
*******************************************************************************/
GT_STATUS prvCpssDxChLpmTcamIpMcEntriesWriteToHw
(
    IN PRV_CPSS_DXCH_LPM_TCAM_VR_TBL_BLOCK_STC *vrEntryPtr,
    IN CPSS_IP_PROTOCOL_STACK_ENT              protocolStack,
    IN PRV_CPSS_DXCH_LPM_TCAM_SHADOW_STC       *ipTcamShadowPtr
)
{
    GT_STATUS ret;
    GT_U8 ipGroup[16];     /* The IP address to be deleted at each loop*/
                                /* when flushing IP-group addresses.        */

    GT_U32 ipGroupPrefix = 0;            /* The search IP address prefix to be       */
                                /* deleted.                                 */

    PRV_CPSS_DXCH_LPM_TCAM_PAT_TRIE_NODE_STC     *pRootTrie;

    PRV_CPSS_DXCH_LPM_TCAM_MC_GROUP_SRC_NODE_STC *mcGroupNode;

    PRV_CPSS_DXCH_LPM_TCAM_PAT_TRIE_FUNCS_OBJ_STC *ipMcTcamFuncs;

    ipMcTcamFuncs = &ipTcamShadowPtr->ipMcTcamFuncsPtrs[protocolStack];

    /*osSemWait(ipTcamShadowPtr->ipUnitSem,OS_WAIT_FOREVER);*/

    mcGroupNode = NULL;

    pRootTrie = vrEntryPtr->ipMcTrie[protocolStack];

    /* get the group */
    while ((prvCpssDxChLpmTcamPatTrieGetNext(pRootTrie,ipGroup,
                                             &ipGroupPrefix,
                                             (GT_PTR)&mcGroupNode))
           != NULL)
    {

        if(mcGroupNode->ipMcSrcTrie != NULL)
        {
            /* write the group's sources trie */
            ret = prvCpssDxChLpmTcamPatTrieWriteToHw(ipTcamShadowPtr,
                                                     vrEntryPtr->vrId,
                                                     mcGroupNode->ipMcSrcTrie,
                                                     ipMcTcamFuncs,
                                                     GT_FALSE);

            if (ret != GT_OK)
            {
                return ret;
            }
        }
    }

    /* all the source trie's have been written, now write the group's trie */
    ret = prvCpssDxChLpmTcamPatTrieWriteToHw(ipTcamShadowPtr,
                                             vrEntryPtr->vrId,
                                             pRootTrie,
                                             ipMcTcamFuncs,
                                             GT_FALSE);
    /*osSemSignal(ipTcamShadowPtr->ipUnitSem);*/

    return ret;
}






/*******************************************************************************
* prvCpssDxChLpmTcamMcRuleOffsetGet
*
* DESCRIPTION:
*       This function searches for the  given (S,G) prefixes and returns the
*       Ip Tcam multicast rules offsets associated with it.
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; xCat3; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond.
*
* INPUTS:
*       vrId            - The virtual private network identifier.
*       ipGroupPtr      - (pointer to)The ip multicast addr. associated with the given mll node.
*       ipGroupPrefixLen - ipGroup prefix length.
*       ipSrcPtr        - (pointer to)The source ip address. associated with the given mll node.
*       ipSrcPrefixLen  - ipSrc prefix length.
*       protocolStack   - type of ip protocol stack to work on.
*       ipTcamShadowPtr - the shadow pointer we are working on
*
* OUTPUTS:
*
*       tcamGroupRowIndexPtr      - pointer to group TCAM row index
*       tcamGroupColumnIndexPtr   - pointer to group TCAM column index
*       tcamSrcRowIndexPtr        - pointer to src TCAM row index
*       tcamSrcColumnIndexPtr     - pointer to src TCAM column index
*
* RETURNS:
*       GT_OK                     -  if successful, or
*       GT_NOT_FOUND              -  if the given prefix was not found.
*       GT_NOT_APPLICABLE_DEVICE  -  on not applicable device
*       GT_NOT_SUPPORTED          - the request is not supported.
*       GT_BAD_VALUE              -  on bad value
*
* COMMENTS:
*
*        None
*******************************************************************************/
GT_STATUS prvCpssDxChLpmTcamMcRuleOffsetGet
(
    IN GT_U32                             vrId,
    IN GT_U8                              *ipGroupPtr,
    IN GT_U32                             ipGroupPrefixLen,
    IN GT_U8                              *ipSrcPtr,
    IN GT_U32                             ipSrcPrefixLen,
    IN  CPSS_IP_PROTOCOL_STACK_ENT        protocolStack,
    IN PRV_CPSS_DXCH_LPM_TCAM_SHADOW_STC  *ipTcamShadowPtr,
    OUT GT_U32                            *tcamGroupRowIndexPtr,
    OUT GT_U32                            *tcamGroupColumnIndexPtr,
    OUT GT_U32                            *tcamSrcRowIndexPtr,
    OUT GT_U32                            *tcamSrcColumnIndexPtr
)
{
    PRV_CPSS_DXCH_LPM_TCAM_PAT_TRIE_NODE_STC         *pRootTrie;
    PRV_CPSS_DXCH_LPM_TCAM_PAT_TRIE_NODE_STC         *pSrcTrie;
    PRV_CPSS_DXCH_LPM_TCAM_PAT_TRIE_NODE_STC         *pSrcGroupNode;
    PRV_CPSS_DXCH_LPM_TCAM_MC_GROUP_SRC_NODE_STC     *mcNode;

    PRV_CPSS_DXCH_LPM_TCAM_VR_TBL_BLOCK_STC *vrEntryPtr,vrEntryTmp;

    GT_U8  *lclIpGroup;
    GT_U32 *lclIpGroupPrefix;
    GT_PTR tcamRulePtr = NULL;
    GT_STATUS rc = GT_OK;

    lclIpGroup = (ipGroupPrefixLen == 0)?
        cpssDxChDefaultMcIpAddr[protocolStack] : ipGroupPtr;

    lclIpGroupPrefix = (ipGroupPrefixLen == 0)?
        &cpssDxChDefaultMcIpPrefixLen[protocolStack] : &ipGroupPrefixLen;

    /* Check if the given virtual router already exists */
    vrEntryTmp.vrId = vrId;
    vrEntryPtr = prvCpssSlSearch(ipTcamShadowPtr->vrSl,&vrEntryTmp);
    if (vrEntryPtr == NULL)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_FOUND, LOG_ERROR_NO_MSG);
    }

    /* internally, bit 15 in vrId indicates shared virtual router */
    if ((ipTcamShadowPtr->sharedVrExist == GT_TRUE) && (vrId == ipTcamShadowPtr->sharedVrId))
        vrId |= BIT_15;

    if (vrEntryPtr->ipMcTrie[protocolStack] == NULL)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_SUPPORTED, LOG_ERROR_NO_MSG);
    }

    mcNode = NULL;
    pSrcGroupNode = NULL;

    pRootTrie = vrEntryPtr->ipMcTrie[protocolStack];

    /* search the group  node */
    pSrcGroupNode = prvCpssDxChLpmTcamPatTrieSearch(pRootTrie,
                                                    lclIpGroup,*lclIpGroupPrefix,
                                                    NULL,(GT_PTR)&mcNode);

    /* we should find a group, at least the default next hop */
    if (pSrcGroupNode == NULL)
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_FOUND, LOG_ERROR_NO_MSG);
    rc = prvCpssDxChLpmTcamDispatcherRuleInfoUpdate(PRV_CPSS_DXCH_LPM_TCAM_DISPATCHER_CHECK_RULE_VALIDITY_E,
                                                    ipTcamShadowPtr->tcamManagementMode,
                                                    NULL,
                                                    pSrcGroupNode,
                                                    NULL);
    if (rc != GT_OK)
    {
        return rc;
    }

    rc = prvCpssDxChLpmTcamDispatcherRuleSet(pSrcGroupNode,
                                             ipTcamShadowPtr,
                                             &tcamRulePtr);
    if (rc != GT_OK)
    {
        return rc;
    }

    rc =  prvCpssDxChLpmTcamGetTcamRowColumn(ipTcamShadowPtr,
                                             tcamRulePtr,
                                             tcamGroupRowIndexPtr,
                                             tcamGroupColumnIndexPtr);
    if (rc != GT_OK)
    {
        return rc;
    }

    if (ipSrcPrefixLen != 0)
    {
        /* we look for (G,S) entry, find the source */
        pSrcTrie = mcNode->ipMcSrcTrie;
        if(pSrcTrie != NULL)
        {
            pSrcGroupNode = prvCpssDxChLpmTcamPatTrieSearch(pSrcTrie,
                                                            ipSrcPtr,ipSrcPrefixLen,
                                                            NULL,(GT_PTR)&mcNode);
        }
        else
        {
            pSrcGroupNode = NULL;
            *tcamSrcRowIndexPtr = 0;
            *tcamSrcColumnIndexPtr = 0;
        }

        /* check if we found a source */
        if (pSrcGroupNode == NULL)
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_FOUND, LOG_ERROR_NO_MSG);
        rc = prvCpssDxChLpmTcamDispatcherRuleInfoUpdate(PRV_CPSS_DXCH_LPM_TCAM_DISPATCHER_CHECK_RULE_VALIDITY_E,
                                                        ipTcamShadowPtr->tcamManagementMode,
                                                        NULL,
                                                        pSrcGroupNode,
                                                        NULL);
        if (rc != GT_OK)
        {
            return rc;
        }

        rc = prvCpssDxChLpmTcamDispatcherRuleSet(pSrcGroupNode,
                                                 ipTcamShadowPtr,
                                                 &tcamRulePtr);
        if (rc != GT_OK)
        {
            return rc;
        }

        rc =  prvCpssDxChLpmTcamGetTcamRowColumn(ipTcamShadowPtr,
                                                 tcamRulePtr,
                                                 tcamSrcRowIndexPtr,
                                                 tcamSrcColumnIndexPtr);
        if (rc != GT_OK)
        {
            return rc;
        }
    }
    else
    {
        *tcamSrcRowIndexPtr = 0;
        *tcamSrcColumnIndexPtr = 0;
    }

    return rc;
}

/*******************************************************************************
* prvCpssDxChLpmTcamSearchMcEntry
*
* DESCRIPTION:
*       This function searches for the route entries associates with a given
*       (S,G), prefixes.
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; xCat3; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond.
*
* INPUTS:
*       vrId            - The virtual private network identifier.
*       ipGroupArr      - The ip multicast addr. associated with the given mll node.
*       ipGroupPrefix   - ipGroup prefix.
*       ipSrcArr        - The source ip address. associated with the given mll node.
*       srcPrefix       - ipSrc prefix.
*       protocolStack   - type of ip protocol stack to work on.
*       ipTcamShadowPtr - the shadow pointer we are working on
*
* OUTPUTS:
*       lttEntryPtr         - pointer to ltt entry struct
*       internalRuleIdxPtr  - if not null then it is filled with internal mangment
*                           pool rule index (Hsu use)
*
* RETURNS:
*       GT_OK if successful, or
*       GT_NOT_FOUND if the given prefix was not found.
*       GT_NOT_SUPPORTED          - the request is not supported.
*       GT_NOT_APPLICABLE_DEVICE  - on not applicable device
*
* COMMENTS:
*       1.  Calling this function with ipGroupPrefix = 0, will return the
*           default Mc route entries.
*
*******************************************************************************/
GT_STATUS prvCpssDxChLpmTcamSearchMcEntry
(
    IN GT_U32                                            vrId,
    IN GT_U8                                             ipGroupArr[],
    IN GT_U32                                            ipGroupPrefix,
    IN GT_U8                                             ipSrcArr[],
    IN GT_U32                                            srcPrefix,
    OUT PRV_CPSS_DXCH_LPM_ROUTE_ENTRY_POINTER_STC        *lttEntryPtr,
    OUT GT_U32                                           *internalRuleIdxPtr,
    IN  CPSS_IP_PROTOCOL_STACK_ENT                       protocolStack,
    IN PRV_CPSS_DXCH_LPM_TCAM_SHADOW_STC                 *ipTcamShadowPtr
)
{
    GT_STATUS rc = GT_OK;
    PRV_CPSS_DXCH_LPM_TCAM_PAT_TRIE_NODE_STC      *pRootTrie;
    PRV_CPSS_DXCH_LPM_TCAM_PAT_TRIE_NODE_STC      *pSrcTrie;
    PRV_CPSS_DXCH_LPM_TCAM_PAT_TRIE_NODE_STC      *pSrcGroupNode;
    PRV_CPSS_DXCH_LPM_TCAM_MC_GROUP_SRC_NODE_STC  *mcNode;
    GT_32                                         ruleIdx;
    PRV_CPSS_DXCH_LPM_TCAM_ENTRIES_HANDLER_ENTRY_TYPE_ENT  grEntryType;
    PRV_CPSS_DXCH_LPM_TCAM_ENTRIES_HANDLER_ENTRY_TYPE_ENT  srcEntryType;


    PRV_CPSS_DXCH_LPM_TCAM_VR_TBL_BLOCK_STC *vrEntryPtr,vrEntryTmp;

    GT_U8  *lclIpGroup;
    GT_U32 *lclIpGroupPrefix;

    lclIpGroup = (ipGroupPrefix == 0)?
        cpssDxChDefaultMcIpAddr[protocolStack] : ipGroupArr;

    lclIpGroupPrefix = (ipGroupPrefix == 0)?
        &cpssDxChDefaultMcIpPrefixLen[protocolStack] : &ipGroupPrefix;

    if (protocolStack == CPSS_IP_PROTOCOL_IPV4_E)
    {
        grEntryType =PRV_CPSS_DXCH_LPM_TCAM_ENTRIES_HANDLER_IPV4_MC_GROUP_ENTRY_E;
        srcEntryType =PRV_CPSS_DXCH_LPM_TCAM_ENTRIES_HANDLER_IPV4_MC_SOURCE_ENTRY_E;
    }
    else
    {
        grEntryType = PRV_CPSS_DXCH_LPM_TCAM_ENTRIES_HANDLER_IPV6_MC_GROUP_ENTRY_E;
        srcEntryType = PRV_CPSS_DXCH_LPM_TCAM_ENTRIES_HANDLER_IPV6_MC_SOURCE_ENTRY_E;
    }


    /* Check if the given virtual router already exists */
    vrEntryTmp.vrId = vrId;
    vrEntryPtr = prvCpssSlSearch(ipTcamShadowPtr->vrSl,&vrEntryTmp);
    if (vrEntryPtr == NULL)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_FOUND, LOG_ERROR_NO_MSG);
    }

    /* internally, bit 15 in vrId indicates shared virtual router */
    if ((ipTcamShadowPtr->sharedVrExist == GT_TRUE) && (vrId == ipTcamShadowPtr->sharedVrId))
        vrId |= BIT_15;

    if (vrEntryPtr->ipMcTrie[protocolStack] == NULL)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_SUPPORTED, LOG_ERROR_NO_MSG);
    }

    mcNode = NULL;
    pSrcGroupNode = NULL;

    pRootTrie = vrEntryPtr->ipMcTrie[protocolStack];

    /* search the group  & source */
    pSrcGroupNode = prvCpssDxChLpmTcamPatTrieSearch(pRootTrie,
                                                    lclIpGroup,*lclIpGroupPrefix,
                                                    NULL,(GT_PTR)&mcNode);

    /* we should find a group, at least the default next hop */
    if (pSrcGroupNode == NULL)
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_FOUND, LOG_ERROR_NO_MSG);

    /* group was found */
    if (internalRuleIdxPtr != NULL)
    {
        /* get hsu infor for the node */
        rc = prvCpssDxChLpmTcamDispatchGetEntryHsuInfo(ipTcamShadowPtr,
                                                       grEntryType,
                                                       pSrcGroupNode,
                                                       &ruleIdx);
        if(rc != GT_OK)
            return rc;

        internalRuleIdxPtr[0] = (GT_U32)ruleIdx;
    }


    /* if we look for (G,S) entry*/
    if (srcPrefix != 0)
    {
        /* we look for (G,S) entry, find the source */
        pSrcTrie = mcNode->ipMcSrcTrie;
        if(pSrcTrie != NULL)
        {
            pSrcGroupNode = prvCpssDxChLpmTcamPatTrieSearch(pSrcTrie,
                                                            ipSrcArr,srcPrefix,
                                                            NULL,(GT_PTR)&mcNode);
        }
        else
        {
            pSrcGroupNode = NULL;
        }

        /* check if we found a source */
        if (pSrcGroupNode == NULL)
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_FOUND, LOG_ERROR_NO_MSG);

        /* source was found */
        if (internalRuleIdxPtr != NULL)
        {
            /* get hsu infor for the node */
            rc = prvCpssDxChLpmTcamDispatchGetEntryHsuInfo(ipTcamShadowPtr,
                                                           srcEntryType,
                                                           pSrcGroupNode,
                                                           &ruleIdx);
            if(rc != GT_OK)
                return rc;

            internalRuleIdxPtr[1] = (GT_U32)ruleIdx;
        }
    }



    cpssOsMemCpy(lttEntryPtr,mcNode->lttEntryPtr,
                 sizeof(PRV_CPSS_DXCH_LPM_ROUTE_ENTRY_POINTER_STC));
    return GT_OK;
}


/*******************************************************************************
* prvCpssDxChLpmTcamGetMcEntry
*
* DESCRIPTION:
*   This function returns the next muticast (ipSrc,ipGroup) entry, used
*   to iterate over the exisiting multicast addresses for a specific LPM DB.
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; xCat3; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond.
*
* INPUTS:
*       vrId                  - The virtual router Id.
*       ipGroupArr            - The ip Group address to get the next entry for.
*       groupPrefixPtr        - the ipGroup prefix Length.
*       ipSrcArr              - The ip Source address to get the next entry for.
*       srcPrefixPtr          - ipSrc prefix length.
*       protocolStack         - type of ip protocol stack to work on.
*       ipTcamShadowPtr       - the shadow pointer we are working on
*
* OUTPUTS:
*       ipGroupArr            - The next ip Group address.
*       groupPrefixPtr        - the ipGroup prefix Length.
*       ipSrcArr              - The next ip Source address.
*       srcPrefixPtr          - ipSrc prefix length.
*       lttEntryPtr           - the LTT entry pointing to the MC route entry
*                               associated with this MC route.
*       internalRuleIdxArray  - if not null then it is array of 2 internal rule
*                               indexes (group and source); HSU use
*
* RETURNS:
*       GT_OK if found, or
*       GT_NOT_FOUND - If the given address is the last one on the IP-Mc table.
*       GT_NOT_SUPPORTED          - the request is not supported.
*       GT_NOT_APPLICABLE_DEVICE  - on not applicable device
*
* COMMENTS:
*       1.  The value of ipGroupPtr must be a valid value, it
*           means that it exists in the IP-Mc Table, unless this is the first
*           call to this function, then it's value is 0/0
*
*******************************************************************************/
GT_STATUS prvCpssDxChLpmTcamGetMcEntry
(
    IN    GT_U32                                         vrId,
    INOUT GT_U8                                          ipGroupArr[],
    INOUT GT_U32                                         *groupPrefixPtr,
    INOUT GT_U8                                          ipSrcArr[],
    INOUT GT_U32                                         *srcPrefixPtr,
    OUT   PRV_CPSS_DXCH_LPM_ROUTE_ENTRY_POINTER_STC      *lttEntryPtr,
    OUT   GT_U32                                         internalRuleIdxArray[],
    IN    CPSS_IP_PROTOCOL_STACK_ENT                     protocolStack,
    IN    PRV_CPSS_DXCH_LPM_TCAM_SHADOW_STC              *ipTcamShadowPtr
)
{
    PRV_CPSS_DXCH_LPM_TCAM_PAT_TRIE_NODE_STC      *pRootTrie;
    PRV_CPSS_DXCH_LPM_TCAM_PAT_TRIE_NODE_STC      *pGroupNode;
    PRV_CPSS_DXCH_LPM_TCAM_PAT_TRIE_NODE_STC      *pSrcNode = NULL;
    PRV_CPSS_DXCH_LPM_TCAM_MC_GROUP_SRC_NODE_STC  *mcNode;
    GT_BOOL newGroup = GT_FALSE;
    PRV_CPSS_DXCH_LPM_TCAM_ENTRIES_HANDLER_ENTRY_TYPE_ENT  grEntryType;
    PRV_CPSS_DXCH_LPM_TCAM_ENTRIES_HANDLER_ENTRY_TYPE_ENT  srcEntryType;

    PRV_CPSS_DXCH_LPM_TCAM_VR_TBL_BLOCK_STC *vrEntryPtr,vrEntryTmp;
    GT_STATUS           rc = GT_OK;
    GT_32 ruleIdx;

    if (protocolStack == CPSS_IP_PROTOCOL_IPV4_E)
    {
        grEntryType =PRV_CPSS_DXCH_LPM_TCAM_ENTRIES_HANDLER_IPV4_MC_GROUP_ENTRY_E;
        srcEntryType =PRV_CPSS_DXCH_LPM_TCAM_ENTRIES_HANDLER_IPV4_MC_SOURCE_ENTRY_E;
    }
    else
    {
        grEntryType = PRV_CPSS_DXCH_LPM_TCAM_ENTRIES_HANDLER_IPV6_MC_GROUP_ENTRY_E;
        srcEntryType = PRV_CPSS_DXCH_LPM_TCAM_ENTRIES_HANDLER_IPV6_MC_SOURCE_ENTRY_E;
    }

    if (internalRuleIdxArray != NULL)
    {
        internalRuleIdxArray[0] = internalRuleIdxArray[1] = 0;
    }

    /* Check if the given virtual router already exists */
    vrEntryTmp.vrId = vrId;
    vrEntryPtr = prvCpssSlSearch(ipTcamShadowPtr->vrSl,&vrEntryTmp);
    if (vrEntryPtr == NULL)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_FOUND, LOG_ERROR_NO_MSG);
    }

    /* internally, bit 15 in vrId indicates shared virtual router */
    if ((ipTcamShadowPtr->sharedVrExist == GT_TRUE) && (vrId == ipTcamShadowPtr->sharedVrId))
        vrId |= BIT_15;

    if (vrEntryPtr->ipMcTrie[protocolStack] == NULL)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_SUPPORTED, LOG_ERROR_NO_MSG);
    }

    mcNode = NULL;
    pSrcNode = NULL;

    pRootTrie = vrEntryPtr->ipMcTrie[protocolStack];

    if (*groupPrefixPtr == 0)
    {
        /* search the defualt group */
        pGroupNode = prvCpssDxChLpmTcamPatTrieGetNext(pRootTrie,ipGroupArr,groupPrefixPtr,
                                                      (GT_PTR)&mcNode);

        /* we should find a group, at least the default next hop */
        if (pGroupNode == NULL)
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_FOUND, LOG_ERROR_NO_MSG);

        newGroup        = GT_TRUE;

        /* set the source to 0/0 */
        if (protocolStack == CPSS_IP_PROTOCOL_IPV4_E)
            cpssOsMemSet(ipSrcArr,0,sizeof(GT_U8)*4);
        else
            cpssOsMemSet(ipSrcArr,0,sizeof(GT_U8)*16);

        (*srcPrefixPtr)  = 0;

    }
    else
    {
        pGroupNode = prvCpssDxChLpmTcamPatTrieSearch(pRootTrie,ipGroupArr,
                                                     *groupPrefixPtr,NULL,
                                                     (GT_PTR)&mcNode);

        if (pGroupNode == NULL)
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_FOUND, LOG_ERROR_NO_MSG);
    }

    /* Search for the IP-src part of the address.   */
    while(1)
    {

        /* incase we recived in the src (0,0) the default entry, it means this
           is not a valid G,* , which means we need to treat as looking for G,S*/
        if ((newGroup == GT_FALSE) ||
            (mcNode->isAPointer2Def == GT_TRUE))
        {
            if(mcNode->ipMcSrcTrie != NULL)
            {
                /* search next source */
                pSrcNode = prvCpssDxChLpmTcamPatTrieGetNext(mcNode->ipMcSrcTrie,ipSrcArr,
                                                            srcPrefixPtr,(GT_PTR)&mcNode);
            }
            else
            {
                pSrcNode = NULL;
            }
        }
        else
        {
            /* return the entry associated with IP-src (0,0)   */
            cpssOsMemCpy(lttEntryPtr,mcNode->lttEntryPtr,
                 sizeof(PRV_CPSS_DXCH_LPM_ROUTE_ENTRY_POINTER_STC));

            /* this is a group , no src node */
            pSrcNode = NULL;
            break;
        }

        /* No ip-src addresses left for this group, move to the next group. */
        if(pSrcNode == NULL)
        {
            /* search the group  & source */
            pGroupNode = prvCpssDxChLpmTcamPatTrieGetNext(pRootTrie,ipGroupArr,groupPrefixPtr,
                                                          (GT_PTR)&mcNode);

            /* we should find a group, at least the default next hop */
            if (pGroupNode == NULL)
                CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_FOUND, LOG_ERROR_NO_MSG);

            newGroup        = GT_TRUE;

            if (protocolStack == CPSS_IP_PROTOCOL_IPV4_E)
                cpssOsMemSet(ipSrcArr,0,sizeof(GT_U8)*4);
            else
                cpssOsMemSet(ipSrcArr,0,sizeof(GT_U8)*16);

            (*srcPrefixPtr)  = 0;

        }
        else    /* A new entry was found.   */
        {
            cpssOsMemCpy(lttEntryPtr,mcNode->lttEntryPtr,
                 sizeof(PRV_CPSS_DXCH_LPM_ROUTE_ENTRY_POINTER_STC));

            break;
        }
    }

    if (internalRuleIdxArray != NULL)
    {
        if (pGroupNode != NULL)
        {
            rc = prvCpssDxChLpmTcamDispatchGetEntryHsuInfo(ipTcamShadowPtr,
                                                           grEntryType,
                                                           pGroupNode,
                                                           &ruleIdx);
            if (rc != GT_OK)
                return rc;

            internalRuleIdxArray[0] = (GT_U32)ruleIdx;
        }

        if (pSrcNode != NULL)
        {
            rc = prvCpssDxChLpmTcamDispatchGetEntryHsuInfo(ipTcamShadowPtr,
                                                           srcEntryType,
                                                           pSrcNode,
                                                           &ruleIdx);
            if (rc != GT_OK)
                return rc;

            internalRuleIdxArray[1] = (GT_U32)ruleIdx;
        }
    }

    return rc;
}

/*******************************************************************************
* ipMcTcamTrieUpdateData
*
* DESCRIPTION:
*       updates the offset/pceNum (idx) in the data entry given.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
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
static GT_STATUS ipMcTcamTrieUpdateData
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
    GT_U32  numOfShareDevs;
    PRV_CPSS_DXCH_LPM_ROUTE_ENTRY_POINTER_STC *lttEntryPtr;
    GT_U32 i;
    PRV_CPSS_DXCH_LPM_TCAM_MC_GROUP_SRC_NODE_STC *mcGroupSrcNode;
    GT_U32 tcamRow = 0;
    GT_U32 tcamColumn = 0;
    GT_U32 offset = 0;
    CPSS_DXCH_TCAM_MANAGER_TCAM_LOCATION_STC    *tcamLocation = NULL ;

    /* fix warning */
    oldDataEntry = oldDataEntry;

    /* continue only if there are devices in the share devices list */
    if (devListPtr->ipShareDevNum == 0)
        return GT_OK;

    if (oldDataEntry) {} /* to avoid warning */
    ipShareDevs     = devListPtr->ipShareDevs;
    numOfShareDevs  = devListPtr->ipShareDevNum;

    mcGroupSrcNode = (PRV_CPSS_DXCH_LPM_TCAM_MC_GROUP_SRC_NODE_STC*)dataEntry;

    lttEntryPtr = mcGroupSrcNode->lttEntryPtr;
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
    ret = prvCpssDxChLpmTcamGetTcamOffset(ipTcamShadowPtr,tcamRulePtr,GT_TRUE,&offset);
    if (ret != GT_OK)
    {
        return ret;
    }

    /* set the gIndex in case of group*/
    if (mcGroupSrcNode->ipMcSrcTrie != NULL)
    {
        mcGroupSrcNode->gIndex = (GT_U16)offset;
    }

    return ret;
}

/*******************************************************************************
* ipMcTcamTrieSetPrefixIp
*
* DESCRIPTION:
*       utility function to Add IP UC prefix.
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; xCat3; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond.
*
* INPUTS:
*       vrId                - the virtual router id.
*       ipAddrPtr           - IP Uc address
*       prefixLen           - Ip Uc prefix length
*       ecmpRouteNum        - the ecmp route number to write
*       tcamRulePtr         - the tcam rule to set it in
*       ipTcamShadowPtr     - ip shadow we are working on.
*       pData               - the data to attach to this prefix.
*       protocolStack       - the type of the prefix.
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
static GT_STATUS ipMcTcamTrieSetPrefixIp
(
    IN  GT_U32                                    vrId,
    IN  GT_U8                                     *ipAddrPtr,
    IN  GT_U32                                    prefixLen,
    IN  GT_U8                                     ecmpRouteNum,
    IN  GT_PTR                                    tcamRulePtr,
    IN PRV_CPSS_DXCH_LPM_TCAM_SHADOW_STC          *ipTcamShadowPtr,
    IN  GT_PTR                                    pData,
    IN  CPSS_IP_PROTOCOL_STACK_ENT                protocolStack,
    IN  PRV_CPSS_DXCH_LPM_SHADOW_DEVS_LIST_STC    *devListPtr
)
{
    GT_STATUS   ret = GT_OK;
    PRV_CPSS_DXCH_LPM_TCAM_MC_GROUP_SRC_NODE_STC *mcGroupSrcNode;
    GT_U16      gIndex = 0;
    GT_U32      offset = 0;
    GT_BOOL     isMcSrc = GT_FALSE;
    GT_BOOL     isMcDefault = GT_FALSE;

    /* continue only if there are devices in the share devices list */
    if (devListPtr->ipShareDevNum == 0)
        return GT_OK;

    mcGroupSrcNode = (PRV_CPSS_DXCH_LPM_TCAM_MC_GROUP_SRC_NODE_STC*)pData;

    /* check if the prefix added is MC default entry; upon creating VR,
       no MC source trie is created for the default MC group */
    if (((protocolStack == CPSS_IP_PROTOCOL_IPV4_E) && ((ipAddrPtr[0] & 0xF0) == 0xE0) && (prefixLen == 4)) ||
        ((protocolStack == CPSS_IP_PROTOCOL_IPV6_E) && (ipAddrPtr[0] == 0xFF) && (prefixLen == 8)))
    {
        isMcDefault = GT_TRUE;
    }

    if ((mcGroupSrcNode->ipMcSrcTrie == NULL) && (isMcDefault == GT_FALSE))
    {
        /* this is source entry - use the gindex */
        gIndex = mcGroupSrcNode->gIndex;
        isMcSrc = GT_TRUE;
    }
    else
    {
        ret = prvCpssDxChLpmTcamGetTcamOffset(ipTcamShadowPtr,tcamRulePtr,GT_TRUE,&offset);
        if (ret != GT_OK)
        {
            return ret;
        }

        /* set the gIndex - this is a group*/
        mcGroupSrcNode->gIndex = (GT_U16)offset;
        isMcSrc = GT_FALSE;
    }

    ret = prvCpssDxChLpmTcamTrieSetPrefixIpCheetah2(vrId,
                                                    ipAddrPtr,
                                                    prefixLen,
                                                    ecmpRouteNum,
                                                    isMcSrc,
                                                    gIndex,
                                                    tcamRulePtr,
                                                    ipTcamShadowPtr,
                                                    (GT_PTR)mcGroupSrcNode->lttEntryPtr,
                                                    protocolStack,
                                                    devListPtr);
    return ret;
}

/*******************************************************************************
* ipMcTcamTrieSetPrefixIpv4
*
* DESCRIPTION:
*       utility function to Add IP UC prefix.
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; xCat3; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond.
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
static GT_STATUS ipMcTcamTrieSetPrefixIpv4
(
    IN  GT_U32                                     vrId,
    IN  GT_U8                                      *ipAddrPtr,
    IN  GT_U32                                     prefixLen,
    IN  GT_U8                                      ecmpRouteNum,
    IN  GT_PTR                                     tcamRulePtr,
    IN PRV_CPSS_DXCH_LPM_TCAM_SHADOW_STC           *ipTcamShadowPtr,
    IN  GT_PTR                                     pData,
    IN  PRV_CPSS_DXCH_LPM_SHADOW_DEVS_LIST_STC     *devListPtr
)
{
    return ipMcTcamTrieSetPrefixIp(vrId,
                                   ipAddrPtr,
                                   prefixLen,
                                   ecmpRouteNum,
                                   tcamRulePtr,
                                   ipTcamShadowPtr,
                                   pData,
                                   CPSS_IP_PROTOCOL_IPV4_E, devListPtr);
}

/*******************************************************************************
* ipMcTcamTrieSetPrefixIpv6
*
* DESCRIPTION:
*       utility function to Add IP UC prefix.
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; xCat3; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond.
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
static GT_STATUS ipMcTcamTrieSetPrefixIpv6
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
    return ipMcTcamTrieSetPrefixIp(vrId,
                                   ipAddrPtr,
                                   prefixLen,
                                   ecmpRouteNum,
                                   tcamRulePtr,
                                   ipTcamShadowPtr,
                                   pData,
                                   CPSS_IP_PROTOCOL_IPV6_E,
                                   devListPtr);
}

/*******************************************************************************
* ipMcTcamTrieMovePrefix
*
* DESCRIPTION:
*       utility function to Update move rule entry in IP TCAM table.
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; xCat3; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond.
*
* INPUTS:
*       vrId                    - the virtual router id.
*       moveFromTcamRulePtr     - the tcam rule to move from.
*       moveToTcamRulePtr       - the tcam rule to move to
*       trieNodePtr             - trie node associated with moved rule
*       protocolStack           - the protocol stack of the prefix.
*       devListPtr              - pointer to the device list to preform the actions on.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on error
*
* COMMENTS:
*       In case of moving a MC group, then if there are sources associated with
*       the group index, then the group index in the sources must be updated
*       with the new index of the group. Updating the sources and the group
*       can't be done in atomic operation. Therefore the following sequence will
*       be used:
*         - a copy of all sources will be made with the new group index
*         - the group will be set in the new location
*         - the group will be deleted from the old location
*         - the original sources will be deleted
*       After this sequence, the sources trie will be updated to use the newly
*       set sources.
*******************************************************************************/
static GT_STATUS ipMcTcamTrieMovePrefix
(
    IN  GT_U32                                        vrId,
    IN  GT_PTR                                        moveFromTcamRulePtr,
    IN  GT_PTR                                        moveToTcamRulePtr,
    IN  PRV_CPSS_DXCH_LPM_TCAM_PAT_TRIE_NODE_STC      *trieNodePtr,
    IN  CPSS_IP_PROTOCOL_STACK_ENT                    protocolStack,
    IN  PRV_CPSS_DXCH_LPM_SHADOW_DEVS_LIST_STC        *devListPtr
)
{
    GT_STATUS   ret = GT_OK;
    GT_STATUS   ret2 = GT_OK;
    GT_U8       ipAddr[16];
    GT_U32      prefixLen;
    GT_U32      numOfSources = 0;
    GT_BOOL     isLastNode;
    PRV_CPSS_DXCH_LPM_TCAM_PAT_TRIE_NODE_STC    *srcTrieRootPtr = NULL;
    PRV_CPSS_DXCH_LPM_TCAM_PAT_TRIE_NODE_STC    *srcTrieArray = NULL;
    PRV_CPSS_DXCH_LPM_TCAM_PAT_TRIE_NODE_STC    *nodePtr = NULL;
    PRV_CPSS_DXCH_LPM_TCAM_PAT_TRIE_NODE_STC    *boundingSonPtr = NULL;
    GT_U32      i,j;
    GT_U32      newGroupIndex;
    PRV_CPSS_DXCH_LPM_TCAM_ENTRIES_HANDLER_ENTRY_TYPE_ENT   entryType;
    GT_PTR                                              lowerBound;
    GT_PTR                                              upperBound;
    GT_PTR                                              allocatedTcamRulePtr;
    GT_PTR                                              tcamRulePtr;
    PRV_CPSS_DXCH_LPM_TCAM_MGM_ALLOC_METHOD_ENT         tcamHandlerAllocMethod;
    PRV_CPSS_DXCH_LPM_TCAM_SHADOW_STC                   *ipTcamShadowPtr;
    CPSS_DXCH_TCAM_MANAGER_ENTRY_AMOUNT_STC             requestedEntries;

    /* continue only if there are devices in the share devices list */
    if (devListPtr->ipShareDevNum == 0)
        return GT_OK;

    ipTcamShadowPtr = trieNodePtr->ipTcamShadowPtr;
    srcTrieRootPtr = ((PRV_CPSS_DXCH_LPM_TCAM_MC_GROUP_SRC_NODE_STC*)(trieNodePtr->pData))->ipMcSrcTrie;

    /* for group entry, count the number of sources associated with the group */
    if (srcTrieRootPtr != NULL)
    {
        /* scan the source trie in post order DFS mode */
        nodePtr = srcTrieRootPtr;
        isLastNode = GT_FALSE;
        while (isLastNode == GT_FALSE)
        {
            ret = prvCpssDxChLpmTcamPatTrieGetNextPostOrderTrieNode(&nodePtr,&isLastNode);
            if (ret != GT_OK)
                return ret;

            /* count the valid nodes */
            if (nodePtr->pData != NULL)
                numOfSources++;
        }
    }

    /* in case of moving group with sources, allocate TCAM locations from the
       beginning of the TCAM for each source according to post order DFS */
    if ((srcTrieRootPtr != NULL) && (numOfSources != 0))
    {
        /* check if the TCAM doesn't have enough available memory for the temporary sources */
        if (protocolStack == CPSS_IP_PROTOCOL_IPV4_E)
        {
            requestedEntries.entryType = CPSS_DXCH_TCAM_MANAGER_SINGLE_ENTRY_E;
            requestedEntries.amount = numOfSources;
            entryType = PRV_CPSS_DXCH_LPM_TCAM_ENTRIES_HANDLER_IPV4_MC_SOURCE_ENTRY_E;
        }
        else
        {
            requestedEntries.entryType = CPSS_DXCH_TCAM_MANAGER_QUAD_ENTRY_E;
            requestedEntries.amount = numOfSources;
            entryType = PRV_CPSS_DXCH_LPM_TCAM_ENTRIES_HANDLER_IPV6_MC_SOURCE_ENTRY_E;
        }

        ret = prvCpssDxChLpmTcamDispatchCheckAvailableMem(ipTcamShadowPtr,entryType,numOfSources,
                                                          &requestedEntries,1);
        if (ret != GT_OK)
            return ret;

        /* Note that even if check available memory indicate that there is
           enough memory for the sources, the allocations may still fail. For
           example in case of IPv6 sources, it is not guaranteed the the
           available space will be in full lines and not be fragmented. Also
           for IPv4 the IPv6 line to be free is still not allocated so not counted
           during check available memory, however it can't be used for allocations */

        /* allocate trie node array for the new sources */
        srcTrieArray = cpssOsMalloc(numOfSources * sizeof(PRV_CPSS_DXCH_LPM_TCAM_PAT_TRIE_NODE_STC));
        if (srcTrieArray == NULL)
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_OUT_OF_CPU_MEM, LOG_ERROR_NO_MSG);

        tcamHandlerAllocMethod = PRV_CPSS_DXCH_LPM_TCAM_MGM_ALLOC_MIN_E;
        allocatedTcamRulePtr = NULL;
        lowerBound = NULL;
        upperBound = NULL;

        /* get the group index for the new group location */
        ret = prvCpssDxChLpmTcamGetTcamOffset(ipTcamShadowPtr,moveToTcamRulePtr,GT_TRUE,&newGroupIndex);
        if (ret != GT_OK)
        {
            cpssOsFree(srcTrieArray);
            return ret;
        }

        /* scan the source trie in DFS mode */
        nodePtr = srcTrieRootPtr;
        isLastNode = GT_FALSE;
        i = 0;
        while (isLastNode == GT_FALSE)
        {
            ret = prvCpssDxChLpmTcamPatTrieGetNextPostOrderTrieNode(&nodePtr,&isLastNode);
            if (ret != GT_OK)
            {
                /* this function can't fail */
                return ret;
            }

            /* for each valid node, copy the trie node to temp src trie,
               allocate TCAM location and assign the new group index */
            if (nodePtr->pData != NULL)
            {
                /* copy current trie */
                cpssOsMemCpy(&srcTrieArray[i],nodePtr,sizeof(PRV_CPSS_DXCH_LPM_TCAM_PAT_TRIE_NODE_STC));

                /* allocate token info */
                PRV_CPSS_DXCH_LPM_TCAM_ALLOCATE_TOKEN_NODE_MAC(ipTcamShadowPtr->tcamManagementMode,&srcTrieArray[i]);

                /* allocate TCAM location; use origianl source trie node as the
                   cookie of the new TCAM location */
                lowerBound = allocatedTcamRulePtr;
                ret = prvCpssDxChLpmTcamDispatchAllocate(ipTcamShadowPtr,entryType,
                                                         lowerBound,upperBound,
                                                         tcamHandlerAllocMethod,
                                                         GT_FALSE, /*defragEnable*/
                                                         nodePtr,
                                                         &allocatedTcamRulePtr);
                if (ret != GT_OK)
                {
                    /* free TCAM locations already allocated and free token info */
                    for (j = 0 ; j < i ; j++)
                    {
                        ret2 = prvCpssDxChLpmTcamDispatchFree(ipTcamShadowPtr,entryType,&srcTrieArray[j]);
                        if (ret2 != GT_OK)
                        {
                            /* failure during recovery; unstable state */
                            return ret2;
                        }
                        PRV_CPSS_DXCH_LPM_TCAM_FREE_TOKEN_NODE_MAC(ipTcamShadowPtr->tcamManagementMode,&srcTrieArray[j]);
                    }
                    /* for last trie node, just free toekn info as TCAM location allocation failed */
                    PRV_CPSS_DXCH_LPM_TCAM_FREE_TOKEN_NODE_MAC(ipTcamShadowPtr->tcamManagementMode,&srcTrieArray[i]);
                    cpssOsFree(srcTrieArray);
                    return ret;
                }
                /* update the trie node in the rule info to original source */
                ret = prvCpssDxChLpmTcamDispatcherRuleInfoUpdate(PRV_CPSS_DXCH_LPM_TCAM_DISPATCHER_RULE_UPDATE_TRIE_E,
                                                                 ipTcamShadowPtr->tcamManagementMode,
                                                                 nodePtr,
                                                                 &srcTrieArray[i],
                                                                 NULL);
                if (ret != GT_OK)
                {
                    /* this fucntion can't fail with this rule action */
                    return ret;
                }
                /* update allocated rule with new allocated TCAM location */
                ret = prvCpssDxChLpmTcamDispatcherRuleInfoUpdate(PRV_CPSS_DXCH_LPM_TCAM_DISPATCHER_ALLOCATED_RULE_UPDATE_E,
                                                                 ipTcamShadowPtr->tcamManagementMode,
                                                                 NULL,
                                                                 &srcTrieArray[i],
                                                                 allocatedTcamRulePtr);
                if (ret != GT_OK)
                {
                    /* this fucntion can't fail with this rule action */
                    return ret;
                }

                /* set the new group index to the new group trie node */
                ((PRV_CPSS_DXCH_LPM_TCAM_MC_GROUP_SRC_NODE_STC*)(srcTrieArray[i].pData))->gIndex = (GT_U16)newGroupIndex;
                i++;
            }
        }

        /* now write sources to the new places with the new group index */
        for (i = 0 ; i < numOfSources ; i++)
        {
            /* get ip address from the rule's node */
            ret = prvCpssDxChLpmTcamPatTrieGetAddrFromNode(&srcTrieArray[i],ipAddr,&prefixLen);
            if (ret != GT_OK)
            {
                /* this function can't fail */
                return ret;
            }

            ret = prvCpssDxChLpmTcamDispatcherRuleSet(&srcTrieArray[i],ipTcamShadowPtr,&tcamRulePtr);
            if (ret != GT_OK)
            {
                /* this function can't fail */
                return ret;
            }

            ret = ipMcTcamTrieSetPrefixIp(vrId,
                                          ipAddr,
                                          prefixLen,
                                          0,
                                          tcamRulePtr,
                                          ipTcamShadowPtr,
                                          srcTrieArray[i].pData,
                                          protocolStack,
                                          devListPtr);
            if (ret != GT_OK)
            {
                for (j = 0 ; j < numOfSources ; j++)
                {
                    /* till this trie node, the MC source was written to TCAM, delete it */
                    if (j < i)
                    {
                        ret2 = prvCpssDxChLpmTcamDispatcherRuleSet(&srcTrieArray[i],ipTcamShadowPtr,&tcamRulePtr);
                        if (ret2 != GT_OK)
                        {
                            /* failure during recovery; unstable state */
                            return ret2;
                        }
                        ret2 = prvCpssDxChLpmTcamTrieDelPrefixCheetah2(vrId,
                                                                       tcamRulePtr,
                                                                       ipTcamShadowPtr,
                                                                       srcTrieArray[i].pData,
                                                                       protocolStack,
                                                                       devListPtr);
                        if (ret2 != GT_OK)
                        {
                            /* failure during recovery; unstable state */
                            return ret2;
                        }
                    }
                    /* for all trie nodes, free token */
                    ret2 = prvCpssDxChLpmTcamDispatchFree(ipTcamShadowPtr,entryType,&srcTrieArray[j]);
                    if (ret2 != GT_OK)
                    {
                        /* failure during recovery; unstable state */
                        return ret2;
                    }
                    /* for all trie nodes, free token info too */
                    PRV_CPSS_DXCH_LPM_TCAM_FREE_TOKEN_NODE_MAC(ipTcamShadowPtr->tcamManagementMode,&srcTrieArray[j]);
                }
                cpssOsFree(srcTrieArray);
                return ret;
            }
        }
    }

    /* get ip address from the rule's node */
    ret = prvCpssDxChLpmTcamPatTrieGetAddrFromNode(trieNodePtr,ipAddr,&prefixLen);
    if (ret != GT_OK)
    {
        /* this function can't fail */
        return ret;
    }

    /* write the new rule */
    ret = ipMcTcamTrieSetPrefixIp(vrId,
                                  ipAddr,
                                  prefixLen,
                                  0,
                                  moveToTcamRulePtr,
                                  trieNodePtr->ipTcamShadowPtr,
                                  trieNodePtr->pData,
                                  protocolStack,
                                  devListPtr);
    if (ret != GT_OK)
    {
        /* in case of fail during writing new group entry that has associated
           sources, delete the temporary sources too */
        if ((srcTrieRootPtr != NULL) && (numOfSources != 0))
        {
            for (j = 0 ; j < numOfSources ; j++)
            {
                /* delete the MC source from the TCAM */
                ret2 = prvCpssDxChLpmTcamDispatcherRuleSet(&srcTrieArray[j],ipTcamShadowPtr,&tcamRulePtr);
                if (ret2 != GT_OK)
                {
                    /* failure during recovery; unstable state */
                    return ret2;
                }
                ret2 = prvCpssDxChLpmTcamTrieDelPrefixCheetah2(vrId,
                                                               tcamRulePtr,
                                                               ipTcamShadowPtr,
                                                               srcTrieArray[j].pData,
                                                               protocolStack,
                                                               devListPtr);
                if (ret2 != GT_OK)
                {
                    /* failure during recovery; unstable state */
                    return ret2;
                }
                /* free token and token info */
                if (protocolStack == CPSS_IP_PROTOCOL_IPV4_E)
                    entryType = PRV_CPSS_DXCH_LPM_TCAM_ENTRIES_HANDLER_IPV4_MC_SOURCE_ENTRY_E;
                else
                    entryType = PRV_CPSS_DXCH_LPM_TCAM_ENTRIES_HANDLER_IPV6_MC_SOURCE_ENTRY_E;
                ret2 = prvCpssDxChLpmTcamDispatchFree(ipTcamShadowPtr,entryType,&srcTrieArray[j]);
                if (ret2 != GT_OK)
                {
                    /* failure during recovery; unstable state */
                    return ret2;
                }
                PRV_CPSS_DXCH_LPM_TCAM_FREE_TOKEN_NODE_MAC(ipTcamShadowPtr->tcamManagementMode,&srcTrieArray[j]);
            }
            cpssOsFree(srcTrieArray);
        }
        return ret;
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
        /* failure during clean up of old entries; TCAM has multiple
           entries for same prefix; unstable state */
        return ret;
    }

    /* in case of group with associated sources, invalidate sources that are
       associated with old group index */
    if ((srcTrieRootPtr != NULL) && (numOfSources != 0))
    {
        if (protocolStack == CPSS_IP_PROTOCOL_IPV4_E)
            entryType = PRV_CPSS_DXCH_LPM_TCAM_ENTRIES_HANDLER_IPV4_MC_SOURCE_ENTRY_E;
        else
            entryType = PRV_CPSS_DXCH_LPM_TCAM_ENTRIES_HANDLER_IPV6_MC_SOURCE_ENTRY_E;

        /* scan the source trie in DFS mode */
        nodePtr = srcTrieRootPtr;
        isLastNode = GT_FALSE;
        i = 0;
        while (isLastNode == GT_FALSE)
        {
            ret = prvCpssDxChLpmTcamPatTrieGetNextPostOrderTrieNode(&nodePtr,&isLastNode);
            if (ret != GT_OK)
            {
                /* this function can't fail */
                return ret;
            }

            /* for valid node invalidate source associated with the old group
               index, free its token and update node information */
            if (nodePtr->pData != NULL)
            {
                /* delete the old rule */
                ret = prvCpssDxChLpmTcamDispatcherRuleSet(nodePtr,ipTcamShadowPtr,&tcamRulePtr);
                if (ret != GT_OK)
                {
                    /* this function can't fail */
                    return ret;
                }

                ret = prvCpssDxChLpmTcamTrieDelPrefixCheetah2(vrId,
                                                              tcamRulePtr,
                                                              ipTcamShadowPtr,
                                                              nodePtr->pData,
                                                              protocolStack,
                                                              devListPtr);
                if (ret != GT_OK)
                {
                    /* failure during clean up of old entries; TCAM has multiple
                       entries for same prefix; unstable state */
                    return ret;
                }

                /* copy token information from temp node and new group index */
                ret = prvCpssDxChLpmTcamDispatchFree(ipTcamShadowPtr,entryType,nodePtr);
                if (ret != GT_OK)
                {
                    /* failure during clean up of old entries; TCAM has multiple
                       entries for same prefix; unstable state */
                    return ret;
                }
                PRV_CPSS_DXCH_LPM_TCAM_FREE_TOKEN_NODE_MAC(ipTcamShadowPtr->tcamManagementMode,nodePtr);
                /* note that new TCAM location was allocated with nodePtr as the
                   cookie, so there is no need to update cookie */
                cpssOsMemCpy(nodePtr,&srcTrieArray[i],sizeof(PRV_CPSS_DXCH_LPM_TCAM_PAT_TRIE_NODE_STC));
                i++;
            }
            /* for invalid node, update the bounding son (the TCAM locations
               of the sons have been changed) */
            else
            {
                /* update bounding son */
                ret = prvCpssDxChLpmTcamPatTrieGetBoundingSon(nodePtr,ipTcamShadowPtr,&boundingSonPtr);
                if (ret != GT_OK)
                {
                    /* failure during clean up of old entries; TCAM has multiple
                       entries for same prefix; unstable state */
                    return ret;
                }

                if (boundingSonPtr != NULL)
                {
                    ret = prvCpssDxChLpmTcamDispatcherRuleInfoUpdate(PRV_CPSS_DXCH_LPM_TCAM_DISPATCHER_RULE_COPY_E,
                                                                     ipTcamShadowPtr->tcamManagementMode,
                                                                     boundingSonPtr,
                                                                     nodePtr,
                                                                     NULL);
                    if (ret != GT_OK)
                    {
                        /* this fucntion can't fail with this rule action */
                        return ret;
                    }
                }
            }
        }

        /* free sources node array; note that the tcam information memory is now
           use by the updates sources; the tcam information memory used by the
           trie sources was freed during the traverse */
        cpssOsFree(srcTrieArray);
    }

    return ret;
}


/*******************************************************************************
* ipMcTcamTrieMoveIpv4Prefix
*
* DESCRIPTION:
*       utility function to Update move rule entry in IP TCAM table.
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; xCat3; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond.
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
static GT_STATUS ipMcTcamTrieMoveIpv4Prefix
(
    IN  GT_U32                                        vrId,
    IN  GT_PTR                                        moveFromTcamRulePtr,
    IN  GT_PTR                                        moveToTcamRulePtr,
    IN  PRV_CPSS_DXCH_LPM_TCAM_PAT_TRIE_NODE_STC      *trieNodePtr,
    IN  PRV_CPSS_DXCH_LPM_SHADOW_DEVS_LIST_STC        *devListPtr
)
{
    return ipMcTcamTrieMovePrefix(vrId,
                                  moveFromTcamRulePtr,
                                  moveToTcamRulePtr,
                                  trieNodePtr,
                                  CPSS_IP_PROTOCOL_IPV4_E,
                                  devListPtr);
}

/*******************************************************************************
* ipMcTcamTrieMoveIpv6Prefix
*
* DESCRIPTION:
*       utility function to Update move rule entry in IP TCAM table.
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; xCat3; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond.
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
static GT_STATUS ipMcTcamTrieMoveIpv6Prefix
(
    IN  GT_U32                                        vrId,
    IN  GT_PTR                                        moveFromTcamRulePtr,
    IN  GT_PTR                                        moveToTcamRulePtr,
    IN  PRV_CPSS_DXCH_LPM_TCAM_PAT_TRIE_NODE_STC      *trieNodePtr,
    IN  PRV_CPSS_DXCH_LPM_SHADOW_DEVS_LIST_STC        *devListPtr
)
{
    return ipMcTcamTrieMovePrefix(vrId,
                                  moveFromTcamRulePtr,
                                  moveToTcamRulePtr,
                                  trieNodePtr,
                                  CPSS_IP_PROTOCOL_IPV6_E,
                                  devListPtr);
}

/*******************************************************************************
* prvCpssDxChLpmTcamRegisterIpMcFuncs
*
* DESCRIPTION:
*       register the relevant ip functions for that ip protocol
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; xCat3; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond.
*
* INPUTS:
*       shadowType      - the shadow type (cheetah+ or cheetah2)
*       protocolStack   - types of IP stack used in the future virtual routers.
*       ipTcamShadowPtr - the shadow pointer we are working on
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
*       None
*
*******************************************************************************/
GT_STATUS prvCpssDxChLpmTcamRegisterIpMcFuncs
(
    IN PRV_CPSS_DXCH_LPM_SHADOW_TYPE_ENT       shadowType,
    IN CPSS_IP_PROTOCOL_STACK_ENT              protocolStack,
    IN PRV_CPSS_DXCH_LPM_TCAM_SHADOW_STC       *ipTcamShadowPtr
)
{

    if ((shadowType == PRV_CPSS_DXCH_LPM_TCAM_CHEETAH_SHADOW_E) ||
        (shadowType == PRV_CPSS_DXCH_LPM_TCAM_XCAT_POLICY_BASED_ROUTING_SHADOW_E))
    {
        /* no support needed */
        return GT_OK;
    }

    /* cheetah2/cheetah3 funcs */
    ipTcamShadowPtr->ipMcTcamFuncsPtrs[protocolStack].ipTcamDelPrefix =
            (protocolStack == CPSS_IP_PROTOCOL_IPV4_E)?
            prvCpssDxChLpmTcamTrieDelIpv4PrefixCheetah2:
            prvCpssDxChLpmTcamTrieDelIpv6PrefixCheetah2;

    ipTcamShadowPtr->ipMcTcamFuncsPtrs[protocolStack].ipTcamMovePrefix =
        (protocolStack == CPSS_IP_PROTOCOL_IPV4_E)?
        ipMcTcamTrieMoveIpv4Prefix:
        ipMcTcamTrieMoveIpv6Prefix;

    ipTcamShadowPtr->ipMcTcamFuncsPtrs[protocolStack].ipTcamUpdateDataIdx =
        ipMcTcamTrieUpdateData;

    ipTcamShadowPtr->ipMcTcamFuncsPtrs[protocolStack].ipTcamSetPrefix =
        (protocolStack == CPSS_IP_PROTOCOL_IPV4_E)?
        ipMcTcamTrieSetPrefixIpv4:ipMcTcamTrieSetPrefixIpv6;

    /* set the working list */
    ipTcamShadowPtr->ipMcTcamFuncsPtrs[protocolStack].workDevListPtr =
        &ipTcamShadowPtr->workDevListPtr;

    return GT_OK;
}

