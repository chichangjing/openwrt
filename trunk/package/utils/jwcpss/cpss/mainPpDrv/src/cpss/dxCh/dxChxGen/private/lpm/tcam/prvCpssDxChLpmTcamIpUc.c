/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* prvCpssDxChLpmTcamIpUc.c
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
*       $Revision: 28 $
*
*******************************************************************************/


#include <cpssCommon/cpssPresteraDefs.h>
#include <cpss/generic/cpssCommonDefs.h>
#include <cpss/extServices/private/prvCpssBindFunc.h>
#include <cpssCommon/private/prvCpssSkipList.h>
#include <cpss/generic/ip/cpssIpTypes.h>
#include <cpss/dxCh/dxChxGen/pcl/cpssDxChPcl.h>
#include <cpss/dxCh/dxChxGen/ipLpmEngine/cpssDxChIpLpmTypes.h>
#include <cpss/dxCh/dxChxGen/private/lpm/tcam/prvCpssDxChLpmTcamMgm.h>
#include <cpss/dxCh/dxChxGen/private/lpm/tcam/prvCpssDxChLpmTcamPatTrie.h>
#include <cpss/dxCh/dxChxGen/private/lpm/tcam/prvCpssDxChLpmTcamTypes.h>
#include <cpss/dxCh/dxChxGen/private/lpm/tcam/prvCpssDxChLpmTcamEntriesHandler.h>
#include <cpss/dxCh/dxChxGen/private/lpm/tcam/prvCpssDxChLpmTcamIpUc.h>
#include <cpss/dxCh/dxChxGen/private/lpm/tcam/prvCpssDxChLpmTcam.h>
#include <cpss/dxCh/dxChxGen/resourceManager/cpssDxChTcamManager.h>
#include <cpss/dxCh/dxChxGen/private/lpm/tcam/prvCpssDxChLpmTcamDispatcher.h>
#include <cpss/dxCh/dxChxGen/config/private/prvCpssDxChInfo.h>

#ifdef OS_MALLOC_MEMORY_LEAKAGE_DBG
static GT_VOID myCpssOsFree
(
    IN GT_VOID* const memblock
)
{
    cpssOsFree_MemoryLeakageDbg(memblock,__FILE__,__LINE__);
}
#endif /*OS_MALLOC_MEMORY_LEAKAGE_DBG*/

/***************************************************************************
* Externals
****************************************************************************/

/*******************************************************************************
* Ip core common environment parameters
*******************************************************************************/


/*******************************************************************************
* prvCpssDxChLpmTcamAddIpUcEntry
*
* DESCRIPTION:
*       creates a new or override an exist route entry in the IP routing
*       structures.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       vrId            - The virtual private network identifier.
*       ipDestPtr       - The destination IP address of this route.
*       ipPrefix        - The number of bits that are actual valid in,
*                         the ipDestPtr.
*       nextHopInfoPtr  - the next hop info pointer to associate with this prefix
*       ruleIdxPtr      - if not null then this is the internal rule index for this
*                         prefix (Hsu use)
*       override        - override an existing entry for this mask if it already
*                         exists, or don't override and return error.
*       isBulkInsert    - is this insert is part of a big bulk insertion which
*                         means we do all the operation apart from writing to the HW
*       defragEnable    - whether to defrag entries in case there is no
*                         free place for this entry cause of entries
*                         fragmentation; relevant only if the TCAM entries
*                         handler was created with partitionEnable = GT_FALSE
*       protocolStack   - types of IP we're dealing with.
*       ipTcamShadowPtr - the ip shadow we are working on.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success, or
*       GT_ERROR                 - If the vrId was not created yet, or
*       GT_NOT_FOUND             - If the given nextHopId is not valid, or
*       GT_OUT_OF_CPU_MEM        - If failed to allocate CPU memory, or
*       GT_OUT_OF_PP_MEM         - If failed to allocate PP memory, or
*       GT_FAIL                  - otherwise.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS prvCpssDxChLpmTcamAddIpUcEntry
(
    IN GT_U32                                 vrId,
    IN GT_U8                                  *ipDestPtr,
    IN GT_U32                                 ipPrefix,
    IN PRV_CPSS_DXCH_LPM_ROUTE_ENTRY_INFO_UNT *nextHopInfoPtr,
    IN GT_U32                                 *ruleIdxPtr,
    IN GT_BOOL                                override,
    IN GT_BOOL                                isBulkInsert,
    IN GT_BOOL                                defragEnable,
    IN CPSS_IP_PROTOCOL_STACK_ENT             protocolStack,
    IN  PRV_CPSS_DXCH_LPM_TCAM_SHADOW_STC     *ipTcamShadowPtr
)
{
    /* Pointer the 1st level bucket to which the prefix should be inserted.*/
    PRV_CPSS_DXCH_LPM_TCAM_PAT_TRIE_NODE_STC *pRootTrie;

    /*IP Tcam Routing Pool Manager header.*/
    PRV_CPSS_DXCH_LPM_TCAM_PAT_TRIE_FUNCS_OBJ_STC *ipTcamFuncs;

    PRV_CPSS_DXCH_LPM_TCAM_RULE_NODE_STC *ruleNodePtr = NULL;
    GT_PTR                               tcamRulePtr = NULL;
    /* The next hop entry to be inserted to the LPM structure and the old one.*/
    GT_PTR   ipNextHopEntry = NULL;

    /* Indicates if the given prefix already exists in the LPM structure. */
    GT_BOOL alreadyExists = GT_FALSE;

    GT_U8 i;
    GT_U32 numOfActualRules,oldNumOfRules;


    GT_U8 exactMatchPrefixLen;
    GT_BOOL isExactMatch;
    GT_U32 typeSize;

    GT_STATUS retVal = GT_OK;

    PRV_CPSS_DXCH_LPM_TCAM_VR_TBL_BLOCK_STC *vrEntryPtr,vrEntryTmp;
    PRV_CPSS_DXCH_LPM_TCAM_ENTRIES_HANDLER_ENTRY_TYPE_ENT  entryType;
    CPSS_DXCH_TCAM_MANAGER_ENTRY_AMOUNT_STC         entryTypeAmount;

    GT_U8                                           devNum = 0;
    GT_U32                                          prefixNumber = 1;
    CPSS_IP_UNICAST_MULTICAST_ENT                   ipMode = CPSS_IP_UNICAST_E;
    PRV_CPSS_DXCH_LPM_TCAM_IP_MC_ENTRY_TYPE_ENT     mcEntryType = PRV_CPSS_DXCH_LPM_TCAM_IP_MC_GROUP_E;

    CPSS_NULL_PTR_CHECK_MAC(nextHopInfoPtr);

    cpssOsMemSet(&entryTypeAmount,0,sizeof(entryTypeAmount));

    if (protocolStack == CPSS_IP_PROTOCOL_IPV4_E)
    {
        entryType =PRV_CPSS_DXCH_LPM_TCAM_ENTRIES_HANDLER_IPV4_UC_ENTRY_E;
        entryTypeAmount.entryType=CPSS_DXCH_TCAM_MANAGER_SINGLE_ENTRY_E;
    }
    else
    {
        entryType =PRV_CPSS_DXCH_LPM_TCAM_ENTRIES_HANDLER_IPV6_UC_ENTRY_E;
        if (ipTcamShadowPtr->shadowType == PRV_CPSS_DXCH_LPM_TCAM_XCAT_POLICY_BASED_ROUTING_SHADOW_E)
        {
            entryTypeAmount.entryType=CPSS_DXCH_TCAM_MANAGER_SINGLE_ENTRY_E;
        }
        else
        {
            entryTypeAmount.entryType = CPSS_DXCH_TCAM_MANAGER_QUAD_ENTRY_E;
        }
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

    /* check if this vr is intialized for this ip protocol */
    if (vrEntryPtr->ipUcTrie[protocolStack] == NULL)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_INITIALIZED, LOG_ERROR_NO_MSG);
    }

    /*osSemWait(ipTcamShadowPtr->ipUnitSem,OS_WAIT_FOREVER);*/

    pRootTrie = vrEntryPtr->ipUcTrie[protocolStack];

    ipTcamFuncs = &ipTcamShadowPtr->ipUcTcamFuncsPtrs[protocolStack];

    /* use the ecmp on PCL emulation on cheetah+ , cheetah2 support it in HW */
    numOfActualRules = 1;

    /* Check if an entry with the same (ip,prefix) already exists,  */
    /* if yes, check the override param.                            */
    if(prvCpssDxChLpmTcamPatTrieSearch(pRootTrie,ipDestPtr,ipPrefix,
                                       &oldNumOfRules,&ipNextHopEntry) != NULL)
    {

        if(override == GT_FALSE)
        {
            /*osSemSignal(ipTcamShadowPtr->ipUnitSem);*/
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_ALREADY_EXIST, LOG_ERROR_NO_MSG);

        }

       /* if we don't have the same number of routes we need to delete the old
          one first.
        if (oldNextHopEntry->numOfRouteEntries !=
            ipNextHopEntry->numOfRouteEntries)
        {
            retVal = prvCpssDxChLpmTcamDeleteIpUcEntry(vrId,ipDestPtr,ipPrefix,
                                                       protocolStack);
            if (retVal != GT_OK)
            {
                return (GT_NOT_FOUND);
            }

             now check if there is room
            if (prvCpssDxChLpmTcamMgmCheckAvailbleMem(pIpTcamPool,numOfActualRules)
                != GT_OK)
            {
                return (GT_OUT_OF_PP_MEM);
            }
        }*/
        alreadyExists = GT_TRUE;
    }
    else
    {
        /* this a new prefix , check if there is space */
        /* check IP TCAM counter */
        retVal = prvCpssDxChLpmTcamFreeSpaceCountersCheck(prefixNumber,ipMode,mcEntryType,
                                                          protocolStack,ipTcamShadowPtr);
        if (retVal != GT_OK)
        {
            return retVal;
        }

        entryTypeAmount.amount = 1;
        retVal =  prvCpssDxChLpmTcamDispatchCheckAvailableMem(ipTcamShadowPtr,
                                                              entryType,
                                                              1,
                                                              &entryTypeAmount,
                                                              1);
        if (retVal != GT_OK)
            return retVal;
    }

    /* now allocate the next hop info (if needed) relevant for this shadow */
    if ((ipTcamShadowPtr->shadowType == PRV_CPSS_DXCH_LPM_TCAM_CHEETAH_SHADOW_E) ||
        (ipTcamShadowPtr->shadowType == PRV_CPSS_DXCH_LPM_TCAM_XCAT_POLICY_BASED_ROUTING_SHADOW_E))
    {
        typeSize = sizeof(CPSS_DXCH_PCL_ACTION_STC);
    }
    else
    {
        typeSize = sizeof(PRV_CPSS_DXCH_LPM_ROUTE_ENTRY_POINTER_STC);
    }

    if (ipNextHopEntry == NULL)
    {
        ipNextHopEntry = cpssOsMalloc(typeSize);
        if (ipNextHopEntry == NULL)
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_OUT_OF_CPU_MEM, LOG_ERROR_NO_MSG);
    }

    /* now copy the next hop info */
    if ((ipTcamShadowPtr->shadowType == PRV_CPSS_DXCH_LPM_TCAM_CHEETAH_SHADOW_E) ||
        (ipTcamShadowPtr->shadowType == PRV_CPSS_DXCH_LPM_TCAM_XCAT_POLICY_BASED_ROUTING_SHADOW_E))
    {
        cpssOsMemCpy(ipNextHopEntry,&nextHopInfoPtr->pclIpUcAction,
                     sizeof(CPSS_DXCH_PCL_ACTION_STC));
    }
    else
    {
        switch(nextHopInfoPtr->routeEntry.routeEntryMethod)
        {
            case PRV_CPSS_DXCH_LPM_ENTRY_TYPE_ECMP_E:
            case PRV_CPSS_DXCH_LPM_ENTRY_TYPE_QOS_E:
            case PRV_CPSS_DXCH_LPM_ENTRY_TYPE_REGULAR_E:
                break;
            default:
                CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
        }

        for(i=0; i <  ipTcamShadowPtr->shadowDevList.ipShareDevNum; i++)
        {
            devNum = ipTcamShadowPtr->shadowDevList.ipShareDevs[i];

            if(0 == PRV_CPSS_DXCH_LION_FAMILY_CHECK_MAC(devNum))
            {
                if (nextHopInfoPtr->routeEntry.blockSize > CPSS_DXCH_IP_MAX_ECMP_QOS_GROUP)
                    CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
            }
            else
            {
                if(nextHopInfoPtr->routeEntry.routeEntryMethod == PRV_CPSS_DXCH_LPM_ENTRY_TYPE_QOS_E)
                {
                    if (nextHopInfoPtr->routeEntry.blockSize > CPSS_DXCH_IP_MAX_LION_QOS_GROUP)
                        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
                }
                else
                {
                    if (nextHopInfoPtr->routeEntry.blockSize > CPSS_DXCH_IP_MAX_LION_ECMP_GROUP)
                        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
                }

            }
        }

        switch(nextHopInfoPtr->routeEntry.ipv6McGroupScopeLevel)
        {
            case CPSS_IPV6_PREFIX_SCOPE_LINK_LOCAL_E:
            case CPSS_IPV6_PREFIX_SCOPE_SITE_LOCAL_E:
            case CPSS_IPV6_PREFIX_SCOPE_UNIQUE_LOCAL_E:
            case CPSS_IPV6_PREFIX_SCOPE_GLOBAL_E:
                break;
            default:
                CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
        }

        cpssOsMemCpy(ipNextHopEntry,&nextHopInfoPtr->routeEntry,
                     sizeof(PRV_CPSS_DXCH_LPM_ROUTE_ENTRY_POINTER_STC));
    }

    exactMatchPrefixLen = (GT_U8)((protocolStack == CPSS_IP_PROTOCOL_IPV4_E) ?
                                                                    32 : 128);
    isExactMatch = (ipPrefix == exactMatchPrefixLen)? GT_TRUE : GT_FALSE;

    /* check if we have a rule already */
    if (ruleIdxPtr != NULL)
    {
        retVal = prvCpssDxChLpmTcamDispatchSetEntryHsuInfo(ipTcamShadowPtr,
                                                           entryType,
                                                           *ruleIdxPtr,
                                                           &tcamRulePtr);
        if (retVal != GT_OK)
            return retVal;
        ruleNodePtr = (PRV_CPSS_DXCH_LPM_TCAM_RULE_NODE_STC*)tcamRulePtr;
    }

    for (i = 0; i < numOfActualRules ; i++)
    {
        /* Insert the complete ip address to the lpm structure */
        retVal = prvCpssDxChLpmTcamPatTrieInsert(vrId,
                                                 pRootTrie,
                                                 ipTcamShadowPtr,
                                                 entryType,
                                                 ipDestPtr,
                                                 ipPrefix,
                                                 i,
                                                 ipNextHopEntry,
                                                 isExactMatch,
                                                 isBulkInsert,
                                                 defragEnable,
                                                 ruleNodePtr,
                                                 ipTcamFuncs);

        if (retVal != GT_OK)
        {
            break;
        }
        else
        {
            if (alreadyExists == GT_FALSE)
            {
                ipTcamShadowPtr->allocatedIpPrefixCounters.numOfAllocatedIpPrefixes[protocolStack]++;
            }
        }
    }

    /*osSemSignal(ipTcamShadowPtr->ipUnitSem);*/
    return retVal;
}


/*******************************************************************************
* prvCpssDxChLpmTcamDeleteIpUcEntry
*
* DESCRIPTION:
*       Deletes on ip/prefix unicast address from a practicular virtual private
*       network table.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       vrId        - The virtual private network identifier.
*       ipDestPtr   - The destination IP address of this route.
*       ipPrefix    - The number of bits that are actual valid in,
*                     the ipDestPtr. (CIDR like)
*       protocolStack - types of IP we're dealing with.
*       ipTcamShadowPtr - the ip shadow we are working on.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success, or
*       GT_ERROR                 - if the required vrId is not valid, or
*       GT_FAIL                  - on error.
*       GT_NOT_SUPPORTED         - the request is not supported.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS prvCpssDxChLpmTcamDeleteIpUcEntry
(
    IN GT_U32                             vrId,
    IN GT_U8                              *ipDestPtr,
    IN GT_U32                             ipPrefix,
    IN CPSS_IP_PROTOCOL_STACK_ENT         protocolStack,
    IN PRV_CPSS_DXCH_LPM_TCAM_SHADOW_STC  *ipTcamShadowPtr
)
{
    /* Pointer the 1st level bucket to which the prefix should be inserted. */
    PRV_CPSS_DXCH_LPM_TCAM_PAT_TRIE_NODE_STC *pRootTrie;

    /*IP Tcam Routing Pool Manager header.*/
    PRV_CPSS_DXCH_LPM_TCAM_PAT_TRIE_FUNCS_OBJ_STC *ipTcamFuncs;

    GT_PTR delEntry = NULL;/* Next pointer struct.   */

    GT_32 i;
    GT_U32 numOfActualRules;

    GT_STATUS retVal = GT_OK;

    PRV_CPSS_DXCH_LPM_TCAM_VR_TBL_BLOCK_STC *vrEntryPtr,vrEntryTmp;
    PRV_CPSS_DXCH_LPM_TCAM_ENTRIES_HANDLER_ENTRY_TYPE_ENT  entryType;

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

    /* check if this vr is intialized for this ip protocol */
    if (vrEntryPtr->ipUcTrie[protocolStack] == NULL)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_SUPPORTED, LOG_ERROR_NO_MSG);
    }

    /* can't delete the defualt !*/
    if (ipPrefix == 0)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    /*osSemWait(ipTcamShadowPtr->ipUnitSem,OS_WAIT_FOREVER);*/

    pRootTrie = vrEntryPtr->ipUcTrie[protocolStack];

    ipTcamFuncs = &ipTcamShadowPtr->ipUcTcamFuncsPtrs[protocolStack];

    if (prvCpssDxChLpmTcamPatTrieSearch(pRootTrie,ipDestPtr,ipPrefix,
                                        &numOfActualRules,&delEntry) == NULL)
    {
        /*osSemSignal(ipTcamShadowPtr->ipUnitSem);*/
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_FOUND, LOG_ERROR_NO_MSG);
    }

    /* if we did and ecmp on PCL emulation we have to remove all the prefix's
       fellow prefixes */
    for (i = numOfActualRules -1; i >= 0; i--)
    {
        /* delete the complete ip address to the lpm structure */
        if (protocolStack == CPSS_IP_PROTOCOL_IPV4_E)
            entryType = PRV_CPSS_DXCH_LPM_TCAM_ENTRIES_HANDLER_IPV4_UC_ENTRY_E;
        else
            entryType = PRV_CPSS_DXCH_LPM_TCAM_ENTRIES_HANDLER_IPV6_UC_ENTRY_E;

        retVal = prvCpssDxChLpmTcamPatTrieDel(vrId,
                                              pRootTrie,
                                              ipTcamShadowPtr,
                                              entryType,
                                              ipDestPtr,
                                              ipPrefix,
                                              (GT_U8)i,
                                              &delEntry,
                                              ipTcamFuncs);
        if (retVal != GT_OK)
        {
            break;
        }
    }

    if (retVal == GT_OK)
    {
        /*this case is an error !*/
        if (ipTcamShadowPtr->allocatedIpPrefixCounters.numOfAllocatedIpPrefixes[protocolStack] < numOfActualRules)
        {
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_FAIL, LOG_ERROR_NO_MSG);
        }
        ipTcamShadowPtr->allocatedIpPrefixCounters.numOfAllocatedIpPrefixes[protocolStack]-= numOfActualRules;
    }

    /* free the data */
    cpssOsFree(delEntry);
    /*osSemSignal(ipTcamShadowPtr->ipUnitSem);*/
    return retVal;
}



/*******************************************************************************
* prvCpssDxChLpmTcamFlushIpUcEntries
*
* DESCRIPTION:
*       flushes the unicast IP Routing table and stays with the default entry
*       only.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       vrId        - The virtual private network identifier.
*       protocolStack - types of IP we're dealing with.
*       ipTcamShadowPtr - the ip shadow we are working on.
*       flushDefault - whether to flush the defualt
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
GT_STATUS prvCpssDxChLpmTcamFlushIpUcEntries
(
    IN GT_U32                             vrId,
    IN CPSS_IP_PROTOCOL_STACK_ENT         protocolStack,
    IN PRV_CPSS_DXCH_LPM_TCAM_SHADOW_STC  *ipTcamShadowPtr,
    IN GT_BOOL                            flushDefault
)
{
    /* Pointer the 1st level bucket to which the prefix should be inserted. */
    PRV_CPSS_DXCH_LPM_TCAM_PAT_TRIE_NODE_STC *pRootTrie;

    /*IP Tcam Routing Pool Manager header.*/
    PRV_CPSS_DXCH_LPM_TCAM_PAT_TRIE_FUNCS_OBJ_STC *ipTcamFuncs;
    PRV_CPSS_DXCH_LPM_TCAM_PAT_TRIE_NODE_STC          *nodePtr = NULL;
    GT_U32      numOfUcPrefixes = 0;
    GT_BOOL     isLastNode = GT_FALSE;

    GT_STATUS retVal = GT_OK;

    PRV_CPSS_DXCH_LPM_TCAM_VR_TBL_BLOCK_STC *vrEntryPtr,vrEntryTmp;
    PRV_CPSS_DXCH_LPM_TCAM_ENTRIES_HANDLER_ENTRY_TYPE_ENT  entryType;

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

    /* check if this vr is intialized for this ip protocol */
    if (vrEntryPtr->ipUcTrie[protocolStack] == NULL)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_SUPPORTED, LOG_ERROR_NO_MSG);
    }

    pRootTrie = vrEntryPtr->ipUcTrie[protocolStack];

    ipTcamFuncs = &ipTcamShadowPtr->ipUcTcamFuncsPtrs[protocolStack];

    if (protocolStack == CPSS_IP_PROTOCOL_IPV4_E)
        entryType = PRV_CPSS_DXCH_LPM_TCAM_ENTRIES_HANDLER_IPV4_UC_ENTRY_E;
    else
        entryType = PRV_CPSS_DXCH_LPM_TCAM_ENTRIES_HANDLER_IPV6_UC_ENTRY_E;

    /* calculate number of prefixes in trie before delete */
    if (pRootTrie != NULL)
    {
        nodePtr = pRootTrie;
        while (isLastNode == GT_FALSE)
        {
            retVal = prvCpssDxChLpmTcamPatTrieGetNextPostOrderTrieNode(&nodePtr,&isLastNode);
            if (retVal != GT_OK)
                return retVal;
            /* count the valid nodes */
            if (nodePtr->pData != NULL)
                numOfUcPrefixes++;
        }
    }
    /* the calculated number of prefixes contains root node -- default */
    if (flushDefault == GT_FALSE)
    {
        numOfUcPrefixes--;
    }
    retVal = prvCpssDxChLpmTcamPatTrieFlush(pRootTrie,
                                            ipTcamShadowPtr,
                                            entryType,
                                            ipTcamFuncs,
#ifdef OS_MALLOC_MEMORY_LEAKAGE_DBG
                                            myCpssOsFree,
#else /*! OS_MALLOC_MEMORY_LEAKAGE_DBG */
                                            cpssOsFree,
#endif /*! OS_MALLOC_MEMORY_LEAKAGE_DBG */
                                            flushDefault);
    if (retVal == GT_OK)
    {
        /*this case is an error !*/
        if (ipTcamShadowPtr->allocatedIpPrefixCounters.numOfAllocatedIpPrefixes[protocolStack] < numOfUcPrefixes)
        {
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_FAIL, LOG_ERROR_NO_MSG);
        }
        ipTcamShadowPtr->allocatedIpPrefixCounters.numOfAllocatedIpPrefixes[protocolStack] -= numOfUcPrefixes;
    }

    return retVal;
}


/*******************************************************************************
* prvCpssDxChLpmTcamUcRuleOffsetGet
*
* DESCRIPTION:
*       This function searches for a given ip-uc address, and returns the
*       Ip Tcam unicast rule (tcam row and tcam column) associated with it.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       vrId            - The virtual router Id to get the entry from.
*       ipAddrPtr       - The ip address to search for.
*       prefixLength    - Prefix length of ipAddrPtr.
*       protocolStack   - types of IP we're dealing with.
*       ipTcamShadowPtr - the ip shadow we are working on.
*
* OUTPUTS:
*       tcamRowIndexPtr    - pointer to TCAM row index.
*       tcamColumnIndexPtr - pointer to TCAM column index.
*
* RETURNS:
*       GT_OK                    - if the required entry was found.
*       GT_NOT_FOUND             - if the given ip prefix was not found.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device.
*       GT_NOT_SUPPORTED         - the request is not supported.
*       GT_BAD_VALUE             - on bad value
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS prvCpssDxChLpmTcamUcRuleOffsetGet
(
    IN  GT_U32                                      vrId,
    IN  GT_U8                                       *ipAddrPtr,
    IN  GT_U32                                      prefixLength,
    IN  CPSS_IP_PROTOCOL_STACK_ENT                  protocolStack,
    IN  PRV_CPSS_DXCH_LPM_TCAM_SHADOW_STC           *ipTcamShadowPtr,
    OUT GT_U32                                      *tcamRowIndexPtr,
    OUT GT_U32                                      *tcamColumnIndexPtr
)
{
     /* Pointer the 1st level bucket to which the prefix should be inserted. */
    PRV_CPSS_DXCH_LPM_TCAM_PAT_TRIE_NODE_STC *pRootTrie;

    /*IP Tcam Routing Pool Manager header.*/
    PRV_CPSS_DXCH_LPM_TCAM_PAT_TRIE_NODE_STC *resultNode;

    GT_PTR ipNextHopEntry = NULL;/* Next pointer struct.   */

    PRV_CPSS_DXCH_LPM_TCAM_VR_TBL_BLOCK_STC *vrEntryPtr,vrEntryTmp;
    GT_PTR tcamRulePtr = NULL;
    GT_STATUS rc = GT_OK;

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

    /* check if this vr is intialized for this ip protocol */
    if (vrEntryPtr->ipUcTrie[protocolStack] == NULL)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_SUPPORTED, LOG_ERROR_NO_MSG);
    }


    pRootTrie = vrEntryPtr->ipUcTrie[protocolStack];

    resultNode = prvCpssDxChLpmTcamPatTrieSearch(pRootTrie,ipAddrPtr,prefixLength,
                                                 NULL,&ipNextHopEntry);
    if(resultNode == NULL)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_FOUND, LOG_ERROR_NO_MSG);
    }
    rc = prvCpssDxChLpmTcamDispatcherRuleInfoUpdate(PRV_CPSS_DXCH_LPM_TCAM_DISPATCHER_CHECK_RULE_VALIDITY_E,
                                                    ipTcamShadowPtr->tcamManagementMode,
                                                    NULL,
                                                    resultNode,
                                                    NULL);
    if (rc != GT_OK)
    {
        return rc;
    }

    rc = prvCpssDxChLpmTcamDispatcherRuleSet(resultNode,
                                             ipTcamShadowPtr,
                                             &tcamRulePtr);
    if (rc != GT_OK)
    {
        return rc;
    }

    rc =  prvCpssDxChLpmTcamGetTcamRowColumn(ipTcamShadowPtr,
                                             tcamRulePtr,
                                             tcamRowIndexPtr,
                                             tcamColumnIndexPtr);
     return rc;
}


/*******************************************************************************
* prvCpssDxChLpmTcamSearchIpUcEntry
*
* DESCRIPTION:
*       This function searches for a given ip-uc address, and returns the next
*       hop/s associated with it.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       vrId        - The virtual router Id to get the entry from.
*       ipAddrPtr   - The ip address to search for.
*       prefix      - Prefix length of ipAddrPtr.
*       protocolStack - types of IP we're dealing with.
*       ipTcamShadowPtr - the ip shadow we are working on.
*
* OUTPUTS:
*       nextHopInfoPtr      - The next hop info to which the given prefix is bound.
*       internalRuleIdxPtr  - if not null then it is filled with internal mangment
*                             pool rule index (Hsu use)
*
* RETURNS:
*       GT_OK                    - if the required entry was found, or
*       GT_NOT_FOUND             - if the given ip prefix was not found.
*       GT_NOT_SUPPORTED         - the request is not supported.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS prvCpssDxChLpmTcamSearchIpUcEntry
(
    IN  GT_U32                                      vrId,
    IN  GT_U8                                       *ipAddrPtr,
    IN  GT_U32                                      prefix,
    OUT PRV_CPSS_DXCH_LPM_ROUTE_ENTRY_INFO_UNT      *nextHopInfoPtr,
    OUT GT_U32                                      *internalRuleIdxPtr,
    IN  CPSS_IP_PROTOCOL_STACK_ENT                  protocolStack,
    IN  PRV_CPSS_DXCH_LPM_TCAM_SHADOW_STC           *ipTcamShadowPtr
)
{
     /* Pointer the 1st level bucket to which the prefix should be inserted. */
    PRV_CPSS_DXCH_LPM_TCAM_PAT_TRIE_NODE_STC *pRootTrie;
    GT_STATUS                                   rc;
    GT_32 ruleIdx;
    PRV_CPSS_DXCH_LPM_TCAM_PAT_TRIE_NODE_STC *resultNode;

    GT_PTR ipNextHopEntry = NULL;/* Next pointer struct.   */

    PRV_CPSS_DXCH_LPM_TCAM_VR_TBL_BLOCK_STC *vrEntryPtr,vrEntryTmp;
    PRV_CPSS_DXCH_LPM_TCAM_ENTRIES_HANDLER_ENTRY_TYPE_ENT  entryType;

    if (protocolStack == CPSS_IP_PROTOCOL_IPV4_E)
    {
        entryType =PRV_CPSS_DXCH_LPM_TCAM_ENTRIES_HANDLER_IPV4_UC_ENTRY_E;
    }
    else
    {
        entryType =PRV_CPSS_DXCH_LPM_TCAM_ENTRIES_HANDLER_IPV6_UC_ENTRY_E;
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

    /* check if this vr is intialized for this ip protocol */
    if (vrEntryPtr->ipUcTrie[protocolStack] == NULL)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_SUPPORTED, LOG_ERROR_NO_MSG);
    }


    pRootTrie = vrEntryPtr->ipUcTrie[protocolStack];

    resultNode = prvCpssDxChLpmTcamPatTrieSearch(pRootTrie,ipAddrPtr,prefix,
                                                 NULL,&ipNextHopEntry);
    if(resultNode == NULL)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_FOUND, LOG_ERROR_NO_MSG);
    }

    /* check what kind of pointer was that */
    if ((ipTcamShadowPtr->shadowType == PRV_CPSS_DXCH_LPM_TCAM_CHEETAH_SHADOW_E) ||
        (ipTcamShadowPtr->shadowType == PRV_CPSS_DXCH_LPM_TCAM_XCAT_POLICY_BASED_ROUTING_SHADOW_E))
    {
        cpssOsMemCpy(&nextHopInfoPtr->pclIpUcAction,ipNextHopEntry,
                     sizeof(CPSS_DXCH_PCL_ACTION_STC));
    }
    else
    {
        cpssOsMemCpy(&nextHopInfoPtr->routeEntry,ipNextHopEntry,
                     sizeof(PRV_CPSS_DXCH_LPM_ROUTE_ENTRY_POINTER_STC));
    }

    if (internalRuleIdxPtr != NULL)
    {
        /* get hsu infor for the node */
        rc = prvCpssDxChLpmTcamDispatchGetEntryHsuInfo(ipTcamShadowPtr,
                                                       entryType,
                                                       resultNode,
                                                       &ruleIdx);
        if(rc != GT_OK)
            return rc;

        *internalRuleIdxPtr = (GT_U32)ruleIdx;
    }
    return GT_OK;
}

/*******************************************************************************
* prvCpssDxChLpmTcamGetIpUcEntry
*
* DESCRIPTION:
*       This function returns an IP-Unicast entry with larger (ip,prefix) than
*       the given one.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       vrId            - The virtual router Id to get the entry from.
*       ipAddrPtr       - The ip address to start the search from.
*       prefixPtr       - Prefix length of ipAddrPtr.
*       protocolStack   - types of IP we're dealing with.
*       ipTcamShadowPtr - the ip shadow we are working on.
*
* OUTPUTS:
*       ipAddrPtr           - The ip address of the found entry.
*       prefixPtr           - The prefix length of the found entry.
*       nextHopInfoPtr      - The next hop info to which the found prefix is bound.
*       internalRuleIdxPtr  - if not null then it is filled with internal mangment
*                         pool rule index (Hsu use)
*
* RETURNS:
*       GT_OK                    - if the required entry was found, or
*       GT_NOT_FOUND             - if no more entries are left in the IP table.
*       GT_NOT_SUPPORTED         - the request is not supported.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS prvCpssDxChLpmTcamGetIpUcEntry
(
    IN GT_U32                                   vrId,
    INOUT GT_U8                                 *ipAddrPtr,
    INOUT GT_U32                                *prefixPtr,
    IN CPSS_IP_PROTOCOL_STACK_ENT               protocolStack,
    OUT PRV_CPSS_DXCH_LPM_ROUTE_ENTRY_INFO_UNT  *nextHopInfoPtr,
    OUT GT_U32                                  *internalRuleIdxPtr,
    IN  PRV_CPSS_DXCH_LPM_TCAM_SHADOW_STC       *ipTcamShadowPtr
)
{
    PRV_CPSS_DXCH_LPM_TCAM_PAT_TRIE_NODE_STC *pRootTrie,*resultNode;         /* Pointer the 1st level bucket to which*/
                                    /* the prefix should be inserted.       */
    GT_U32 prefixlen;

    GT_PTR ipNextHopEntry = NULL;/* Next pointer struct.   */
    GT_32 ruleIdx;

    GT_STATUS retVal = GT_OK;

    PRV_CPSS_DXCH_LPM_TCAM_VR_TBL_BLOCK_STC *vrEntryPtr,vrEntryTmp;
    PRV_CPSS_DXCH_LPM_TCAM_ENTRIES_HANDLER_ENTRY_TYPE_ENT  entryType;

    if (protocolStack == CPSS_IP_PROTOCOL_IPV4_E)
    {
        entryType =PRV_CPSS_DXCH_LPM_TCAM_ENTRIES_HANDLER_IPV4_UC_ENTRY_E;
    }
    else
    {
        entryType =PRV_CPSS_DXCH_LPM_TCAM_ENTRIES_HANDLER_IPV6_UC_ENTRY_E;
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

    /* check if this vr is intialized for this ip protocol */
    if (vrEntryPtr->ipUcTrie[protocolStack] == NULL)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_SUPPORTED, LOG_ERROR_NO_MSG);
    }

    /*osSemWait(ipTcamShadowPtr->ipUnitSem,OS_WAIT_FOREVER);*/

    pRootTrie = vrEntryPtr->ipUcTrie[protocolStack];

    prefixlen = *prefixPtr;

    resultNode = prvCpssDxChLpmTcamPatTrieGetNext(pRootTrie,ipAddrPtr,&prefixlen,
                                                  &ipNextHopEntry);

    *prefixPtr = prefixlen;
    if((resultNode != NULL) && (nextHopInfoPtr != NULL))
    {
        /* check what kind of pointer was that and copy the next hop info*/
        if ((ipTcamShadowPtr->shadowType == PRV_CPSS_DXCH_LPM_TCAM_CHEETAH_SHADOW_E) ||
            (ipTcamShadowPtr->shadowType == PRV_CPSS_DXCH_LPM_TCAM_XCAT_POLICY_BASED_ROUTING_SHADOW_E))
        {
            cpssOsMemCpy(&nextHopInfoPtr->pclIpUcAction,ipNextHopEntry,
                         sizeof(CPSS_DXCH_PCL_ACTION_STC));
        }
        else
        {
            cpssOsMemCpy(&nextHopInfoPtr->routeEntry,ipNextHopEntry,
                         sizeof(PRV_CPSS_DXCH_LPM_ROUTE_ENTRY_POINTER_STC));
        }

        if (internalRuleIdxPtr != NULL)
        {
            /* get hsu infor for the node */
            retVal = prvCpssDxChLpmTcamDispatchGetEntryHsuInfo(ipTcamShadowPtr,
                                                               entryType,
                                                               resultNode,
                                                               &ruleIdx);
            if(retVal != GT_OK)
                return retVal;

            *internalRuleIdxPtr = (GT_U32)ruleIdx;
        }
    }

    if (resultNode == NULL)
        retVal = GT_NOT_FOUND;

    return retVal;
}

