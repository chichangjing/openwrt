/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* prvCpssDxChLpmMc.c
*
* DESCRIPTION:
*       This file includes functions for controlling the LPM Multicast
*       tables and structures.
*
* DEPENDENCIES:
*       None.
*
* FILE REVISION NUMBER:
*       $Revision: 14 $
*
*******************************************************************************/

#include <cpss/dxCh/dxChxGen/private/lpm/ram/prvCpssDxChLpmRamTypes.h>
#include <cpss/dxCh/dxChxGen/private/lpm/ram/prvCpssDxChLpmRamMng.h>
#include <cpssCommon/private/prvCpssDevMemManager.h>
#include <cpss/extServices/private/prvCpssBindFunc.h>
#include <cpss/dxCh/dxChxGen/private/lpm/ram/prvCpssDxChLpmRam.h>
#include <cpss/dxCh/dxChxGen/private/lpm/ram/prvCpssDxChLpmRamUc.h>
#include <cpss/dxCh/dxChxGen/private/lpm/ram/prvCpssDxChLpmRamMc.h>
#include <cpssCommon/private/prvCpssSkipList.h>

/****************************************************************************
* Debug                                                                     *
****************************************************************************/

extern void * cpssOsLpmMalloc
(
    IN GT_U32 size
);

extern void cpssOsLpmFree
(
    IN void* const memblock
);

/***************************************************************************
* defines
****************************************************************************/

/*
 * Typedef: struct SHADOW_IP_MC_GROUP_INSERT_FUNC_PARAMS_STC
 *
 * Description: A struct that holds the parameters for
 *              PRV_CPSS_DXCH_LPM_RAM_MC_GROUP_INSERT_FUNC_PTR
 *
 * Fields:
 *      see PRV_CPSS_DXCH_LPM_RAM_MC_GROUP_INSERT_FUNC_PTR
 */
typedef struct SHADOW_IP_MC_GROUP_INSERT_FUNC_PARAMS_STCT
{
    GT_U32                                          vrId;
    GT_U8                                           *ipGroup;
    GT_U32                                          ipGroupPrefix;
    PRV_CPSS_DXCH_LPM_RAM_BUCKET_SHADOW_STC         *srcBucketPtr;
    PRV_CPSS_DXCH_LPM_RAM_SHADOW_STC                *shadowPtr;
    PRV_CPSS_DXCH_LPM_RAM_TRIE_INSERT_MODE_ENT      insertMode;
    PRV_CPSS_DXCH_LPM_PROTOCOL_STACK_ENT            protocol;
}SHADOW_IP_MC_GROUP_INSERT_FUNC_PARAMS_STC;

/*******************************************************************************
* prvCpssDxChLpmRamCheckAvailableMcMem
*
* DESCRIPTION:
*       Check the if there is availble memory for the Mc Group insertion.
*       (Group Search in LPM)
*
* INPUTS:
*       vrId        - the Vr id to look in.
*       ipDestPtr   - the ip group address.
*       ipPrefix    - the ip Group address prefix length.
*       shadowPtr   - the shadow relevant for the devices asked to act on.
*       protocol    - the protocol
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK   - on success.
*       GT_FAIL - on error.
*
* COMMENTS:
*       None.
*******************************************************************************/
static GT_STATUS prvCpssDxChLpmRamCheckAvailableMcMem
(
    IN GT_U32                                   vrId,
    IN GT_U8                                    *ipDestPtr,
    IN GT_U32                                   ipPrefix,
    IN PRV_CPSS_DXCH_LPM_RAM_SHADOW_STC         *shadowPtr,
    IN PRV_CPSS_DXCH_LPM_PROTOCOL_STACK_ENT     protocol
)
{
    GT_STATUS   retVal = GT_OK;          /* Function return value.   */
    GT_U8       groupAddr[PRV_CPSS_DXCH_LPM_MAX_NUM_OF_OCTETS_FOR_ADDRESS_CNS]; /* the group address */
    PRV_CPSS_DXCH_LPM_RAM_BUCKET_SHADOW_STC *rootBucketPtr;
    PRV_CPSS_DXCH_LPM_RAM_MEM_INFO_STC     **lpmEngineMemPtr;
    GT_U8       numOfOctets;

    lpmEngineMemPtr = shadowPtr->mcSearchMemArrayPtr[protocol];
    rootBucketPtr = shadowPtr->vrRootBucketArray[vrId].rootBucket[protocol];

    if (protocol == PRV_CPSS_DXCH_LPM_PROTOCOL_IPV4_E)
    {
        numOfOctets = PRV_CPSS_DXCH_LPM_NUM_OF_OCTETS_IN_IPV4_PROTOCOL_CNS;
    }
    else    /* PRV_CPSS_DXCH_LPM_PROTOCOL_IPV6_E */
    {
        numOfOctets = PRV_CPSS_DXCH_LPM_NUM_OF_OCTETS_IN_IPV6_PROTOCOL_CNS;
    }
    cpssOsMemCpy(groupAddr,ipDestPtr,sizeof(GT_U8)*numOfOctets);

    /* Check for group IP allocation. */
    retVal =
        prvCpssDxChLpmRamMngAllocAvailableMemCheck(rootBucketPtr,groupAddr,
                                                   ipPrefix,
                                                   PRV_CPSS_DXCH_LPM_RAM_ROOT_BUCKET_WITHOUT_COMPRESSED_2_E,
                                                   lpmEngineMemPtr,
                                                   shadowPtr,
                                                   protocol);
    return retVal;
}


/*******************************************************************************
* checkAvailableMcSearchMemory
*
* DESCRIPTION:
*       Check if there is enough available memory in the RAM to insert a new
*       Multicast IP address.
*
* INPUTS:
*       bucketPtr         - The LPM bucket to check on the LPM insert.
*       vrId              - The virtual router Id.
*       ipDestPtr         - If isUnicast == GT_TRUE, holds the unicast IP address to
*                           be inserted.
*                           If isUnicast == GT_FALSE, holds the multicast group ip
*                           address to be inserted.
*       ipPrefix          - Holds the prefix length of ipDest.
*       ipSrcArr             - Valid only if isUnicast == GT_FALSE, holds the source ip
*                           address of the inserted IP-Mc entry.
*       ipSrcPrefix       - Valid only if isUnicast == GT_FALSE, holds the ip address
*                           prefix length of ipSrcArr.
*       rootBucketFlag    - Indicates the way to deal with a root bucket (if it is).
*       skipMcGroupCheck  - indicates whether to skip the MC group check or not.
*       protocolStack     - type of ip protocol stack to work on.
*       shadowPtr         - the shadow relevant for the devices asked to act on.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK               - If there is enough memory for the insertion.
*       GT_OUT_OF_PP_MEM    - otherwise.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS checkAvailableMcSearchMemory
(
    IN PRV_CPSS_DXCH_LPM_RAM_BUCKET_SHADOW_STC      *bucketPtr,
    IN GT_U32                                       vrId,
    IN GT_U8                                        *ipDestPtr,
    IN GT_U32                                       ipPrefix,
    IN GT_U8                                        ipSrcArr[],
    IN GT_U32                                       ipSrcPrefix,
    IN PRV_CPSS_DXCH_LPM_RAM_ROOT_BUCKET_FLAG_ENT   rootBucketFlag,
    IN GT_BOOL                                      skipMcGroupCheck,
    IN PRV_CPSS_DXCH_LPM_PROTOCOL_STACK_ENT         protocolStack,
    IN PRV_CPSS_DXCH_LPM_RAM_SHADOW_STC             *shadowPtr
)
{
    GT_STATUS   retVal = GT_OK;             /* Function return value.   */
    GT_STATUS   retVal2 = GT_OK;

    PRV_CPSS_DXCH_LPM_RAM_MEM_INFO_STC                  **lpmEngineMemPtr;
    GT_U32 i;

    PRV_CPSS_DXCH_LPM_RAM_OCTETS_TO_BLOCK_MAPPING_STC   tempLpmRamOctetsToBlockMapping[PRV_CPSS_DXCH_LPM_RAM_NUM_OF_MEMORIES_CNS];/* used for reconstruct in case of error */

    lpmEngineMemPtr = shadowPtr->ucSearchMemArrayPtr[protocolStack];
    shadowPtr->neededMemoryListLen = 0;
    shadowPtr->neededMemoryCurIdx = 0;

    /* keep values in case UC reconstruct is needed */
    cpssOsMemCpy(tempLpmRamOctetsToBlockMapping,shadowPtr->lpmRamOctetsToBlockMappingPtr,
                 sizeof(PRV_CPSS_DXCH_LPM_RAM_OCTETS_TO_BLOCK_MAPPING_STC)*shadowPtr->numOfLpmMemories);

    /* Check for source IP allocation. */
    retVal = prvCpssDxChLpmRamMngAllocAvailableMemCheck(bucketPtr,ipSrcArr,ipSrcPrefix,
                                                        rootBucketFlag,lpmEngineMemPtr,
                                                        shadowPtr,
                                                        protocolStack);

    /* Source IP allocation succeeded, check mc-tree needs.       */
    if((retVal == GT_OK) && (skipMcGroupCheck == GT_FALSE))
    {
        retVal =
            prvCpssDxChLpmRamCheckAvailableMcMem(vrId,ipDestPtr,ipPrefix,
                                                 shadowPtr,protocolStack);

        /* if we failed the mc allocation release the uc allocation */
        if (retVal != GT_OK)
        {
            for (i = 0 ; i < shadowPtr->neededMemoryListLen; i++)
            {
                prvCpssDmmFree(shadowPtr->neededMemoryBlocks[i]);
            }

            /* free the allocated/bound RAM memory */
            retVal2 = prvCpssDxChLpmRamMngAllocatedAndBoundMemFree(shadowPtr,protocolStack);
            if (retVal2!=GT_OK)
            {
                return retVal2;
            }
            /* in case of fail we will need to reconstruct to the status we had before the call to prvCpssDxChLpmRamMngAllocAvailableMemCheck */
            cpssOsMemCpy(shadowPtr->lpmRamOctetsToBlockMappingPtr,tempLpmRamOctetsToBlockMapping,
                         sizeof(PRV_CPSS_DXCH_LPM_RAM_OCTETS_TO_BLOCK_MAPPING_STC)*shadowPtr->numOfLpmMemories);

        }

        /* in order not to loose the retVal in case of out of Group memory the
           Revert of the ListMng will be on retVal2 */

        /* release all the checked memory if there was any allocated*/
        retVal2 = prvCpssDxChLpmRamMemFreeListMng(0,PRV_CPSS_DXCH_LPM_RAM_MEM_LIST_FREE_MEM_OP_E,
                                                  &shadowPtr->freeMemListDuringUpdate);
        if(retVal2 != GT_OK)
            return retVal2;
    }

    return retVal;
}

/*******************************************************************************
* prvCpssDxChLpmRamMcDefRouteSet
*
* DESCRIPTION:
*       sets the default MC route for the given VR router.
*
* INPUTS:
*       vrId                  - The vrId to which this tree belongs to.
*       defMcRoutePointerPtr  - A pointer to the default mc route
*       shadowPtr             - the shadow relevant for the devices asked to act on.
*       insertMode            - the insert mode.
*       protocol              - the protocol
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK   - on success.
*       GT_FAIL - on error.
*
* COMMENTS:
*       None.
*******************************************************************************/
static GT_STATUS prvCpssDxChLpmRamMcDefRouteSet
(
    IN  GT_U32                                          vrId,
    IN  PRV_CPSS_DXCH_LPM_ROUTE_ENTRY_POINTER_STC       *defMcRoutePointerPtr,
    IN  PRV_CPSS_DXCH_LPM_RAM_SHADOW_STC                *shadowPtr,
    IN  PRV_CPSS_DXCH_LPM_RAM_TRIE_INSERT_MODE_ENT      insertMode,
    IN  PRV_CPSS_DXCH_LPM_PROTOCOL_STACK_ENT            protocol
)
{
    GT_STATUS retVal;

    GT_U8 groupAddr[PRV_CPSS_DXCH_LPM_MAX_NUM_OF_OCTETS_FOR_ADDRESS_CNS];
    GT_U32 ipGroupPrefix;
    CPSS_DXCH_LPM_NEXT_POINTER_TYPE_ENT rootBucketType; /* the root mc Bucket type */
    PRV_CPSS_DXCH_LPM_RAM_MEM_INFO_STC     **lpmEngineMemPtr; /* the MC lpm
                                            Engine Memory array*/
    GT_BOOL                                  updateOldPtr = GT_FALSE;
    CPSS_DXCH_LPM_NEXT_POINTER_TYPE_ENT defMcRoutePtrType;
    PRV_CPSS_DXCH_LPM_RAM_BUCKET_SHADOW_STC *rootBucketPtr;
    rootBucketPtr =
        shadowPtr->vrRootBucketArray[vrId].rootBucket[protocol];
    rootBucketType =
        shadowPtr->vrRootBucketArray[vrId].rootBucketType[protocol];

    lpmEngineMemPtr = shadowPtr->mcSearchMemArrayPtr[protocol];

    ipGroupPrefix =
        (protocol == PRV_CPSS_DXCH_LPM_PROTOCOL_IPV4_E) ?
        PRV_CPSS_DXCH_LPM_RAM_IPV4_MC_ADDRESS_SPACE_PREFIX_CNS :
        PRV_CPSS_DXCH_LPM_RAM_IPV6_MC_ADDRESS_SPACE_PREFIX_CNS;

    groupAddr[0] =
        (GT_U8)((protocol == PRV_CPSS_DXCH_LPM_PROTOCOL_IPV4_E) ?
        PRV_CPSS_DXCH_LPM_RAM_START_OF_IPV4_MC_ADDRESS_SPACE_CNS :
        PRV_CPSS_DXCH_LPM_RAM_END_OF_IPV6_MC_ADDRESS_SPACE_CNS);

    PRV_CPSS_DXCH_LPM_NEXT_PTR_TYPE_CONVERT_MAC(defMcRoutePointerPtr->routeEntryMethod,defMcRoutePtrType);
     /* Insert the complete ip address to the lpm structure */
    retVal = prvCpssDxChLpmRamMngInsert(rootBucketPtr, groupAddr,
                                        ipGroupPrefix, defMcRoutePointerPtr,
                                        defMcRoutePtrType,
                                        PRV_CPSS_DXCH_LPM_RAM_ROOT_BUCKET_WITHOUT_COMPRESSED_2_E,
                                        lpmEngineMemPtr,&updateOldPtr,&rootBucketType,
                                        shadowPtr, insertMode, GT_FALSE);
    if(retVal == GT_OK)
    {
        /* Now update the ip-mc first table according   */
        /* to the first level of the lpm structure.     */
        shadowPtr->vrRootBucketArray[vrId].rootBucketType[protocol] =
            rootBucketType;

        /* Update the default route entry */
        shadowPtr->vrRootBucketArray[vrId].multicastDefault[protocol] =
            defMcRoutePointerPtr;

        /* in case of shadow update don't touch the hw */
        if (insertMode != PRV_CPSS_DXCH_LPM_RAM_TRIE_INSERT_SDW_ONLY_MODE_E)
        {
            retVal = prvCpssDxChLpmRamMngVrfEntryUpdate(vrId ,protocol, shadowPtr);
        }
    }

    return retVal;
}


/*******************************************************************************
* prvCpssDxChLpmRamMcGroupSearch
*
* DESCRIPTION:
*       searches the MC tree for the Given Mc Ip Group address and returns
*       the bucket pointer of the lpm stucture attached to it.
*       (Group Search in LPM)
*
* INPUTS:
*       vrId                  - The vrId to which this tree belongs to.
*       ipGroupPtr            - The ip Address to search for.
*       ipGroupPrefix         - The prefix length of ipGroupPtr.
*       shadowPtr             - the shadow relevant for the devices asked to act on.
*       protocol              - the protocol
*
* OUTPUTS:
*       srcBucketPtrPtr       - A pointer to the shadow bucket including the src. IP
*                               prefixes.
*
* RETURNS:
*       GT_OK   - on success.
*       GT_FAIL - on error.
*
* COMMENTS:
*       None.
*******************************************************************************/
static GT_STATUS prvCpssDxChLpmRamMcGroupSearch
(
    IN  GT_U32                                    vrId,
    IN  GT_U8                                     *ipGroupPtr,
    IN  GT_U32                                    ipGroupPrefix,
    OUT PRV_CPSS_DXCH_LPM_RAM_BUCKET_SHADOW_STC   **srcBucketPtrPtr,
    IN  PRV_CPSS_DXCH_LPM_RAM_SHADOW_STC          *shadowPtr,
    IN  PRV_CPSS_DXCH_LPM_PROTOCOL_STACK_ENT      protocol
)
{
    GT_STATUS retVal;
    GT_U8 groupAddr[PRV_CPSS_DXCH_LPM_MAX_NUM_OF_OCTETS_FOR_ADDRESS_CNS], numOfOctets;
    PRV_CPSS_DXCH_LPM_RAM_BUCKET_SHADOW_STC *rootBucketPtr;
    rootBucketPtr = shadowPtr->vrRootBucketArray[vrId].rootBucket[protocol];

    if (protocol == PRV_CPSS_DXCH_LPM_PROTOCOL_IPV4_E)
    {
        numOfOctets = PRV_CPSS_DXCH_LPM_NUM_OF_OCTETS_IN_IPV4_PROTOCOL_CNS;
    }
    else    /* PRV_CPSS_DXCH_LPM_PROTOCOL_IPV6_E */
    {
        numOfOctets = PRV_CPSS_DXCH_LPM_NUM_OF_OCTETS_IN_IPV6_PROTOCOL_CNS;
    }
    cpssOsMemCpy(groupAddr,ipGroupPtr,sizeof(GT_U8)*numOfOctets);

    retVal = prvCpssDxChLpmRamMngSearch(rootBucketPtr,groupAddr,
                                        ipGroupPrefix,(GT_PTR*)srcBucketPtrPtr);
    return retVal;
}

/*******************************************************************************
* cpssDxChLpmRamMcGroupGetNext
*
* DESCRIPTION:
*       This function searchs the trie for the next valid group node.
*       The search is started from the given ipGroupPtr & ipGroupPrefixPtr.
*       (Group Search in LPM)
*
* INPUTS:
*       vrId              - The vrId to which this table belongs to.
*       force             - Indicates whether to find the next valid entry even
*                           if the given ipGroupPtr and ipGroupPrefixPtr are not
*                           valid.
*       ipGroupPtr        - IP address associated with the search start point.
*       ipGroupPrefixPtr  - prefix of ipGroupPtr.
*       shadowPtr         - the shadow relevant for the devices asked to act on.
*       protocol          - the protocol
*
* OUTPUTS:
*       ipGroupPtr        - The ip address associated with the next node.
*       ipGroupPrefixPtr  - The prefix associated with ipGroupPtr.
*       srcBucketPtrPtr   - A pointer to the src lpm base if found, NULL otherwise.
*
* RETURNS:
*       GT_OK   - on success.
*       GT_FAIL - on error.
*
* COMMENTS:
*       1.  If the given node is the last node in the trie then the values of
*           ipGroupPtr and ipGroupPrefixPtr will not be changed, and
*           *srcBucketPtrPtr will get NULL.
*       2.  If force == GT_TRUE and (ipGroupPtr,ipGroupPrefixPtr) is invalid,
*           then this function will find the first valid node.
*
*******************************************************************************/
static GT_STATUS cpssDxChLpmRamMcGroupGetNext
(
    IN    GT_U32                                   vrId,
    IN    GT_BOOL                                  force,
    INOUT GT_U8                                    *ipGroupPtr,
    INOUT GT_U32                                   *ipGroupPrefixPtr,
    OUT   PRV_CPSS_DXCH_LPM_RAM_BUCKET_SHADOW_STC  **srcBucketPtrPtr,
    IN    PRV_CPSS_DXCH_LPM_RAM_SHADOW_STC         *shadowPtr,
    IN    PRV_CPSS_DXCH_LPM_PROTOCOL_STACK_ENT     protocol
)
{
    GT_STATUS retVal;

    PRV_CPSS_DXCH_LPM_RAM_BUCKET_SHADOW_STC *bucketPtr;/* Pointer to the LPM structure */
    GT_U8 groupAddr[PRV_CPSS_DXCH_LPM_MAX_NUM_OF_OCTETS_FOR_ADDRESS_CNS]; /* this is the group address */
    GT_U8 numOfOctets, i;
    GT_U8 mask;
    GT_U8* groupAddrPtr = (GT_U8*)groupAddr;
    GT_U8 remainder;

    if (protocol == PRV_CPSS_DXCH_LPM_PROTOCOL_IPV4_E)
    {
        numOfOctets = PRV_CPSS_DXCH_LPM_NUM_OF_OCTETS_IN_IPV4_PROTOCOL_CNS;
    }
    else    /* PRV_CPSS_DXCH_LPM_PROTOCOL_IPV6_E */
    {
        numOfOctets = PRV_CPSS_DXCH_LPM_NUM_OF_OCTETS_IN_IPV6_PROTOCOL_CNS;
    }
    cpssOsMemCpy(groupAddr,ipGroupPtr,sizeof(GT_U8)*numOfOctets);

    /* truncate only the prefix part out of the ip address */
    remainder = (GT_U8)*ipGroupPrefixPtr;
    for (i = 0; i < numOfOctets; i++)
    {
        if (*ipGroupPrefixPtr == 0)
        {
            mask = 0;
        }
        else if (remainder >= 8)
        {
            mask = 0xFF;
        }
        else
        {
            mask = (GT_U8)(0xFF << (8 - remainder));
        }
        *groupAddrPtr = (GT_U8) (*groupAddrPtr & mask);
        groupAddrPtr++;
        if (remainder >= 8)
        {
            remainder = (GT_U8)(*ipGroupPrefixPtr - 8);
        }
        else
        {
            remainder = 0;
        }
    }

    bucketPtr = shadowPtr->vrRootBucketArray[vrId].rootBucket[protocol];

    if (force == GT_FALSE)
    {
        /* in this case the ip has to be valid */
        retVal = prvCpssDxChLpmRamMngSearch(bucketPtr,groupAddr,
                                            *ipGroupPrefixPtr,
                                            (GT_PTR*)srcBucketPtrPtr);
        if (retVal != GT_OK)
        {
            return retVal;
        }
    }

    retVal = prvCpssDxChLpmRamMngEntryGet(bucketPtr,protocol,groupAddr,
                                          ipGroupPrefixPtr,(GT_PTR*)srcBucketPtrPtr);

    if ((protocol == PRV_CPSS_DXCH_LPM_PROTOCOL_IPV4_E) &&
        (groupAddr[0] >= PRV_CPSS_DXCH_LPM_RAM_START_OF_IPV4_RESERVED_SPACE_ADDRESS_SPACE_CNS))
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_FOUND, LOG_ERROR_NO_MSG);
    }

    cpssOsMemCpy(ipGroupPtr,groupAddr,sizeof(GT_U8)*numOfOctets);

    return (retVal);
}

/*******************************************************************************
* prvCpssDxChLpmRamMcGroupInsert
*
* DESCRIPTION:
*       Insert an Ip Group Address to the ipv4 mc Tree.
*       (Group Search in LPM)
*
* INPUTS:
*       vrId              - The vrId to which this table belongs to.
*       ipGroupPtr        - The ip group Address associated with the new inserted prefix.
*       ipGroupPrefix     - The prefix length of the new inserted prefix.
*       srcBucketPtr      - A pointer to the shadow bucket including the src. IP prefixes.
*       shadowPtr         - the shadow relevant for the devices asked to act on.
*       insertMode        - the insert mode
*       protocol          - the protocol
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK   - on success.
*       GT_FAIL - on error.
*
* COMMENTS:
*       None.
*******************************************************************************/
static GT_STATUS prvCpssDxChLpmRamMcGroupInsert
(
    IN GT_U32                                           vrId,
    IN GT_U8                                            *ipGroupPtr,
    IN GT_U32                                           ipGroupPrefix,
    IN PRV_CPSS_DXCH_LPM_RAM_BUCKET_SHADOW_STC          *srcBucketPtr,
    IN PRV_CPSS_DXCH_LPM_RAM_SHADOW_STC                 *shadowPtr,
    IN PRV_CPSS_DXCH_LPM_RAM_TRIE_INSERT_MODE_ENT       insertMode,
    IN  PRV_CPSS_DXCH_LPM_PROTOCOL_STACK_ENT            protocol
)
{
    GT_STATUS retVal;

    GT_U8 groupAddr[PRV_CPSS_DXCH_LPM_MAX_NUM_OF_OCTETS_FOR_ADDRESS_CNS]; /* this is the group address */
    PRV_CPSS_DXCH_LPM_RAM_BUCKET_SHADOW_STC *rootBucketPtr;       /* the root Mc bucket */
    CPSS_DXCH_LPM_NEXT_POINTER_TYPE_ENT rootBucketType;  /* the root mc Bucket type */
    GT_BOOL                                  updateOldPtr = GT_FALSE;
    PRV_CPSS_DXCH_LPM_RAM_MEM_INFO_STC     **lpmEngineMemPtr; /* the MC lpm Engine Memory
                                                     array*/
    GT_U8 numOfOctets;

    if (protocol == PRV_CPSS_DXCH_LPM_PROTOCOL_IPV4_E)
    {
        numOfOctets = PRV_CPSS_DXCH_LPM_NUM_OF_OCTETS_IN_IPV4_PROTOCOL_CNS;
    }
    else    /* PRV_CPSS_DXCH_LPM_PROTOCOL_IPV6_E */
    {
        numOfOctets = PRV_CPSS_DXCH_LPM_NUM_OF_OCTETS_IN_IPV6_PROTOCOL_CNS;
    }
    cpssOsMemCpy(groupAddr,ipGroupPtr,sizeof(GT_U8)*numOfOctets);

    lpmEngineMemPtr = shadowPtr->mcSearchMemArrayPtr[protocol];

    rootBucketPtr =
        shadowPtr->vrRootBucketArray[vrId].rootBucket[protocol];
    rootBucketType =
        shadowPtr->vrRootBucketArray[vrId].rootBucketType[protocol];

     /* Insert the complete ip address to the lpm structure */
    retVal = prvCpssDxChLpmRamMngInsert(rootBucketPtr, groupAddr,
                                        ipGroupPrefix, srcBucketPtr,
                                        PRV_CPSS_DXCH_LPM_RAM_TRIE_PTR_TYPE_E,
                                        PRV_CPSS_DXCH_LPM_RAM_ROOT_BUCKET_WITHOUT_COMPRESSED_2_E,
                                        lpmEngineMemPtr,&updateOldPtr,&rootBucketType,
                                        shadowPtr, insertMode, GT_FALSE);
    if(retVal == GT_OK)
    {
        /* Now update the ip-mc first table according   */
        /* to the first level of the lpm structure.     */
        shadowPtr->vrRootBucketArray[vrId].rootBucketType[protocol] =
            rootBucketType;

        /* in case of shadow update don't touch the hw */
        if (insertMode != PRV_CPSS_DXCH_LPM_RAM_TRIE_INSERT_SDW_ONLY_MODE_E)
        {
            retVal = prvCpssDxChLpmRamMngVrfEntryUpdate(vrId, protocol, shadowPtr);
        }
    }

    return retVal;
}

/*******************************************************************************
* prvCpssDxChLpmRamMcGroupDelete
*
* DESCRIPTION:
*       Deletes an Ip Group Address from the ipv4 mc Tree.
*       (Group Search in LPM)
*
* INPUTS:
*       vrId          - The vrId to which this table belongs to.
*       ipGroupPtr    - The ip group Address associated with the new inserted prefix.
*       ipGroupPrefix - The prefix length of the new inserted prefix.

*       shadowPtr     - the shadow relevant for the devices asked to act on.
*       protocol      - the protocol.
*       roollBack                 - GT_TRUE: rollback is taking place.
*                                  GT_FALSE: otherwise.
*
* OUTPUTS:
*       srcBucketPtrPtr - A pointer to the shadow bucket including the src. IP
*                         prefixes.
*
* RETURNS:
*       GT_OK   - on success.
*       GT_FAIL - on error.
*
* COMMENTS:
*       None.
*******************************************************************************/
static GT_STATUS prvCpssDxChLpmRamMcGroupDelete
(
    IN GT_U32                                     vrId,
    IN GT_U8                                      *ipGroupPtr,
    IN GT_U32                                     ipGroupPrefix,
    OUT PRV_CPSS_DXCH_LPM_RAM_BUCKET_SHADOW_STC   **srcBucketPtrPtr,
    IN PRV_CPSS_DXCH_LPM_RAM_SHADOW_STC           *shadowPtr,
    IN PRV_CPSS_DXCH_LPM_PROTOCOL_STACK_ENT       protocol,
    IN GT_BOOL                                    rollBack
)
{
    GT_STATUS retVal,retVal2;
    GT_U8     numOfOctets;

    PRV_CPSS_DXCH_LPM_RAM_POINTER_SHADOW_UNT lpmPtr; /* Pointer to the LPM structure to */
                                                     /* delete from.                    */

    GT_U8 groupAddr[PRV_CPSS_DXCH_LPM_MAX_NUM_OF_OCTETS_FOR_ADDRESS_CNS]; /* this is the group addres */
    CPSS_DXCH_LPM_NEXT_POINTER_TYPE_ENT rootBucketType; /* the root mc Bucket type */

    PRV_CPSS_DXCH_LPM_RAM_MEM_INFO_STC     **lpmEngineMemPtr; /* the MC lpm Engine Memory
                                                     array*/
    PRV_CPSS_DXCH_LPM_RAM_BUCKET_PARENT_WRITE_FUNC_INFO_STC parentWriteFuncInfo;

    PRV_CPSS_DXCH_LPM_RAM_UPDATE_VRF_TABLE_FUNC_PARAMS_STC writeFuncData;

    /* update the info for the parent write function */
    parentWriteFuncInfo.bucketParentWriteFunc =
        prvCpssDxChLpmRamUpdateVrTableFuncWrapper;

    writeFuncData.shadowPtr = shadowPtr;
    writeFuncData.protocol = protocol;
    writeFuncData.vrId = vrId;

    parentWriteFuncInfo.data = (GT_PTR)(&writeFuncData);

    lpmEngineMemPtr = shadowPtr->mcSearchMemArrayPtr[protocol];

    lpmPtr.nextBucket = shadowPtr->vrRootBucketArray[vrId].rootBucket[protocol];

    if (protocol == PRV_CPSS_DXCH_LPM_PROTOCOL_IPV4_E)
    {
        numOfOctets = PRV_CPSS_DXCH_LPM_NUM_OF_OCTETS_IN_IPV4_PROTOCOL_CNS;
    }
    else    /* PRV_CPSS_DXCH_LPM_PROTOCOL_IPV6_E */
    {
        numOfOctets = PRV_CPSS_DXCH_LPM_NUM_OF_OCTETS_IN_IPV6_PROTOCOL_CNS;
    }
    cpssOsMemCpy(groupAddr, ipGroupPtr, sizeof(GT_U8)* numOfOctets);

    rootBucketType = shadowPtr->vrRootBucketArray[vrId].rootBucketType[protocol];
    retVal = prvCpssDxChLpmRamMngEntryDelete(&lpmPtr,groupAddr,ipGroupPrefix,
                                             PRV_CPSS_DXCH_LPM_RAM_ROOT_BUCKET_WITHOUT_COMPRESSED_2_E,GT_TRUE,
                                             lpmEngineMemPtr,&rootBucketType,
                                             (GT_PTR*)srcBucketPtrPtr,shadowPtr,
                                             &parentWriteFuncInfo, GT_FALSE,rollBack);
    if(retVal == GT_OK)
    {
        shadowPtr->vrRootBucketArray[vrId].rootBucketType[protocol] = rootBucketType;
        retVal = prvCpssDxChLpmRamMngVrfEntryUpdate(vrId, protocol, shadowPtr);
    }

    retVal2 = prvCpssDxChLpmRamMemFreeListMng(0,PRV_CPSS_DXCH_LPM_RAM_MEM_LIST_FREE_MEM_OP_E,
                                              &shadowPtr->freeMemListEndOfUpdate);

    if(retVal==GT_OK)
        return retVal2;

    return retVal;
}


/*******************************************************************************
* type: coreIpMcGroupInsertFunc
*
* DESCRIPTION:
*       wrapper function to CORE_IP_MC_GROUP_INSERT_FUNC_PTR
*
* INPUTS:
*       data - parameters to CORE_IP_MC_GROUP_INSERT_FUNC_PTR
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK   - on success.
*       GT_NO_SUCH - if ipGroup was not found, when updateOld is GT_TRUE
*       GT_OUT_OF_PP_MEM - no more ipm Groups can be added
*       GT_FAIL - on error.
*
* COMMENTS:
*       None.
*******************************************************************************/
static GT_STATUS coreIpMcGroupInsertFuncWrap
(
    IN GT_PTR data
)
{
    SHADOW_IP_MC_GROUP_INSERT_FUNC_PARAMS_STC *params;

    params = (SHADOW_IP_MC_GROUP_INSERT_FUNC_PARAMS_STC*)data;

    if (params->protocol > PRV_CPSS_DXCH_LPM_PROTOCOL_IPV6_E)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_STATE, LOG_ERROR_NO_MSG);
    }

    return (prvCpssDxChLpmRamMcGroupInsert(params->vrId,
                                           params->ipGroup,
                                           params->ipGroupPrefix,
                                           params->srcBucketPtr,
                                           params->shadowPtr,
                                           params->insertMode,
                                           params->protocol));
}

/*******************************************************************************
* prvCpssDxChLpmRamMcDefRollBack
*
* DESCRIPTION:
*       restore original the default MC route for the given VR router.
*
* INPUTS:
*       vrId                  - The vrId to which this tree belongs to.
*       defMcRoutePointerPtr  - A pointer to the default mc route
*       shadowPtr             - the shadow relevant for the devices asked to act on.
*       insertMode            - the insert mode.
*       protocolStack         - the protocol stack.
*       srcBucketPtr          - pointer to the root bucket of LPM structure that holds the  IP-Mc src prefixes.
*       ipSrcArr              - the root address for source base multi tree protocol.
*       rootBucketType        - the bucket type of the root bucket
*       alreadyExists         - indicates if the given Src prefix already exists in the LPM structure.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK   - on success.
*       GT_FAIL - on error.
*
* COMMENTS:
*       None.
*******************************************************************************/
static GT_STATUS prvCpssDxChLpmRamMcDefRollBack
(
    IN    GT_U32                                          vrId,
    IN    PRV_CPSS_DXCH_LPM_ROUTE_ENTRY_POINTER_STC       *defMcRoutePointerPtr,
    IN    PRV_CPSS_DXCH_LPM_RAM_SHADOW_STC                *shadowPtr,
    IN    PRV_CPSS_DXCH_LPM_RAM_TRIE_INSERT_MODE_ENT      insertMode,
    IN    PRV_CPSS_DXCH_LPM_PROTOCOL_STACK_ENT            protocolStack,
    IN    PRV_CPSS_DXCH_LPM_RAM_BUCKET_SHADOW_STC         *srcBucketPtr,
    IN    GT_U8                                           ipSrcArr[],
    IN    CPSS_DXCH_LPM_NEXT_POINTER_TYPE_ENT             rootBucketType,
    IN    GT_BOOL                                         alreadyExists
)
{
    GT_STATUS retVal = GT_OK;
    PRV_CPSS_DXCH_LPM_RAM_ROOT_BUCKET_FLAG_ENT    rootBucketFlag;
    PRV_CPSS_DXCH_LPM_RAM_MEM_INFO_STC            **lpmEngineMemPtr;
    GT_U32                                        ipGroupPrefix;
    GT_U8                                         ipGroup[PRV_CPSS_DXCH_LPM_MAX_NUM_OF_OCTETS_FOR_ADDRESS_CNS];


    ipGroup[0] =
        (GT_U8)((protocolStack == PRV_CPSS_DXCH_LPM_PROTOCOL_IPV4_E) ?
        PRV_CPSS_DXCH_LPM_RAM_START_OF_IPV4_MC_ADDRESS_SPACE_CNS :
        PRV_CPSS_DXCH_LPM_RAM_START_OF_IPV6_MC_ADDRESS_SPACE_CNS);
    ipGroupPrefix =
        (protocolStack == PRV_CPSS_DXCH_LPM_PROTOCOL_IPV4_E) ?
        PRV_CPSS_DXCH_LPM_RAM_IPV4_MC_ADDRESS_SPACE_PREFIX_CNS :
        PRV_CPSS_DXCH_LPM_RAM_IPV6_MC_ADDRESS_SPACE_PREFIX_CNS;


     /* set the default route */
    retVal = prvCpssDxChLpmRamMcDefRouteSet(vrId,defMcRoutePointerPtr,
                                            shadowPtr,insertMode,protocolStack);
    if (retVal != GT_OK)
    {
        return retVal;
    }

     while(1)
     {
        /* get next group */
        retVal = cpssDxChLpmRamMcGroupGetNext(vrId, GT_TRUE, ipGroup,
                                              &ipGroupPrefix,&srcBucketPtr,
                                              shadowPtr,protocolStack);
        /* check if there are no more groups */
        if (retVal == GT_NOT_FOUND)
        {
            retVal = GT_OK;
            break;
        }

        if (retVal != GT_OK)
            return retVal;

        if (srcBucketPtr == NULL)
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_FAIL, LOG_ERROR_NO_MSG);

        /* check if the (G,*) is explicit */
        if (srcBucketPtr->isImplicitGroupDefault == GT_FALSE)
        {
            continue; /* to the next group */
        }

        /* update the new group default in its source trie */

        /* the root type for the src trie can be regular, compressed-1 or compressed-2 */
        rootBucketFlag = PRV_CPSS_DXCH_LPM_RAM_ROOT_BUCKET_WITH_RAM_UPDATE_E;

        /* src lpm search is done in the same way as UC lpm */
        lpmEngineMemPtr = shadowPtr->ucSearchMemArrayPtr[protocolStack];

        /* Insert the complete ip address to the lpm structure of PP */
        retVal = prvCpssDxChLpmRamMngInsert(srcBucketPtr, ipSrcArr, 0, defMcRoutePointerPtr,
                                            CPSS_DXCH_LPM_ROUTE_ENTRY_NEXT_PTR_TYPE_E,
                                            rootBucketFlag, lpmEngineMemPtr,
                                            &alreadyExists,&rootBucketType,
                                            shadowPtr, PRV_CPSS_DXCH_LPM_RAM_TRIE_INSERT_SDW_MEM_HW_MODE_E,
                                            GT_TRUE);
        if (retVal != GT_OK)
            return retVal;
    }
     return GT_OK;
}


/*******************************************************************************
* prvCpssDxChLpmRamMcEntryAdd
*
* DESCRIPTION:
*       To add the multicast routing information for IP datagrams from
*       a particular source and addressed to a particular IP multicast
*       group address.
*
* INPUTS:
*       vrId              - The virtual private network identifier.
*       ipGroupPtr        - The IP multicast group address.
*       ipGroupPrefix     - The number of bits that are actual valid in,
*                           the ipGroup. (CIDR like)
*       ipSrcArr             - the root address for source base multi tree protocol.
*       ipSrcPrefix       - The number of bits that are actual valid in,
*                           the ipSrcArr. (CIDR like)
*       mcRoutePointerPtr - the mc route entry pointer.
*       override          - update or override an existing entry, or create a new one.
*       insertMode        - how to insert the new entry.
*       protocolStack     - protocol to work on.
*       shadowPtr         - the shadow relevant for the devices asked to act on.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                     - on success, or
*       GT_BAD_PARAM              - if inserted group is already exist and new ipGroupRuleIndex
*                                   is not equal to current ipGroupRuleIndex, or
*       GT_ERROR                  - if the virtual router does not exist,
*                                   or ipGroupPrefix == 0,
*       GT_FAIL                   - otherwise.
*       GT_NOT_APPLICABLE_DEVICE  - on not applicable device
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS prvCpssDxChLpmRamMcEntryAdd
(
    IN GT_U32                                       vrId,
    IN GT_U8                                        *ipGroupPtr,
    IN GT_U32                                       ipGroupPrefix,
    IN GT_U8                                        ipSrcArr[],
    IN GT_U32                                       ipSrcPrefix,
    IN PRV_CPSS_DXCH_LPM_ROUTE_ENTRY_POINTER_STC    *mcRoutePointerPtr,
    IN GT_BOOL                                      override,
    IN PRV_CPSS_DXCH_LPM_RAM_TRIE_INSERT_MODE_ENT   insertMode,
    IN PRV_CPSS_DXCH_LPM_PROTOCOL_STACK_ENT         protocolStack,
    IN PRV_CPSS_DXCH_LPM_RAM_SHADOW_STC             *shadowPtr
)
{
    CPSS_DXCH_LPM_NEXT_POINTER_TYPE_ENT rootBucketType = CPSS_DXCH_LPM_REGULAR_NODE_PTR_TYPE_E; /* The bucket type of the root bucket*/
                                        /* of the LPM structure that holds  */
                                        /* the IP-Mc src prefixes.          */

    PRV_CPSS_DXCH_LPM_RAM_BUCKET_SHADOW_STC *srcBucket = NULL;/* A pointer to the root bucket of  */
                                        /* LPM structure that holds the     */
                                        /* IP-Mc src prefixes.              */
    PRV_CPSS_DXCH_LPM_ROUTE_ENTRY_POINTER_STC *defMcRoutePointer;/* A pointer to the
                                           default route */
    PRV_CPSS_DXCH_LPM_ROUTE_ENTRY_POINTER_STC *nextHopEntry = NULL;          /* Temporary vars. that hold the new*/

                                        /* entries to be inserted to the LPM*/
                                        /* structure.                       */
    CPSS_DXCH_LPM_NEXT_POINTER_TYPE_ENT nextHopEntryType;
    GT_BOOL updateOld;                  /* The updateOld param. to be sent  */
                                        /* the mcTreeInsert() function.     */

    GT_BOOL alreadyExists = GT_FALSE;   /* Indicates if the given Src prefix*/
                                        /* already exists in the LPM        */
                                        /* structure.                       */
    PRV_CPSS_DXCH_LPM_RAM_ROOT_BUCKET_FLAG_ENT    rootBucketFlag;
    PRV_CPSS_DXCH_LPM_RAM_MEM_INFO_STC     **lpmEngineMemPtr;
    GT_STATUS           retVal,retVal2;
    GT_U8                                   searchIpGroup[PRV_CPSS_DXCH_LPM_MAX_NUM_OF_OCTETS_FOR_ADDRESS_CNS];
    GT_U32                                  searchIpGroupPrefix;
    GT_BOOL                                 tmpIsImplicitGroupDefault = GT_FALSE;
    GT_BOOL                                 isFullGroupMask = GT_FALSE;
    GT_BOOL                                 addNewSrcTree = GT_FALSE;
    PRV_CPSS_DXCH_LPM_ROUTE_ENTRY_POINTER_STC rollBackDefMcRoutePointer = {0};
    PRV_CPSS_DXCH_LPM_ROUTE_ENTRY_POINTER_STC *rollBackNextHopEntryPtr = NULL;

    /* rollback definitions */
    PRV_CPSS_DXCH_LPM_RAM_BUCKET_PARENT_WRITE_FUNC_INFO_STC parentWriteFuncInfo;
    SHADOW_IP_MC_GROUP_INSERT_FUNC_PARAMS_STC writeFuncData;
    PRV_CPSS_DXCH_LPM_RAM_POINTER_SHADOW_UNT lpmPtr;    /* Will hold the pointer to the IP-src LPM  */
                                                        /* structure for delete operations.         */
    CPSS_DXCH_LPM_NEXT_POINTER_TYPE_ENT newBucketType;  /* Holds the LPM first bucket type after    */
                                                        /* delete operations.                       */
    PRV_CPSS_DXCH_LPM_ROUTE_ENTRY_POINTER_STC *delEntry; /* A pointer to the data saved in the trie, */
                                                         /* for IP-src LPM history purposes. And     */
                                                         /* should be deleted.                       */

    PRV_CPSS_DXCH_LPM_RAM_OCTETS_TO_BLOCK_MAPPING_STC   tempLpmRamOctetsToBlockMapping[PRV_CPSS_DXCH_LPM_RAM_NUM_OF_MEMORIES_CNS];/* used for reconstruct in case of error */

    /* rollback settings */
    parentWriteFuncInfo.bucketParentWriteFunc =
        coreIpMcGroupInsertFuncWrap;

    parentWriteFuncInfo.data = (GT_PTR)(&writeFuncData);

    writeFuncData.vrId         = vrId;
    writeFuncData.ipGroup     = ipGroupPtr;
    writeFuncData.ipGroupPrefix = ipGroupPrefix;
    writeFuncData.shadowPtr = shadowPtr;
    writeFuncData.insertMode = PRV_CPSS_DXCH_LPM_RAM_TRIE_INSERT_SDW_MEM_HW_MODE_E;


    /* first check if the protocol was initialized */
    if (shadowPtr->isProtocolInitialized[protocolStack] == GT_FALSE)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_INITIALIZED, LOG_ERROR_NO_MSG);
    }

    if((vrId >= shadowPtr->vrfTblSize) ||
       (shadowPtr->vrRootBucketArray[vrId].valid == GT_FALSE))
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    /* verify that multicast is supported for this protocol in the VR */
    if (shadowPtr->vrRootBucketArray[vrId].isMulticastSupported[protocolStack] == GT_FALSE)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_SUPPORTED, LOG_ERROR_NO_MSG);
    }

    retVal = prvCpssDxChLpmRamMemFreeListMng(0,PRV_CPSS_DXCH_LPM_RAM_MEM_LIST_RESET_OP_E,
                                             &shadowPtr->freeMemListEndOfUpdate);
    if (retVal != GT_OK)
        return (retVal);

    /* get the default route */
    retVal = prvCpssDxChLpmRamMcDefRouteGet(vrId,&defMcRoutePointer,shadowPtr,protocolStack);
    if (retVal != GT_OK)
    {
        /* shouldn't happen, there should always be a default */
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_FAIL, LOG_ERROR_NO_MSG);
    }

    if ((((protocolStack == PRV_CPSS_DXCH_LPM_PROTOCOL_IPV4_E) &&
         (ipGroupPrefix <= PRV_CPSS_DXCH_LPM_RAM_IPV4_MC_ADDRESS_SPACE_PREFIX_CNS)) ||
        ((protocolStack == PRV_CPSS_DXCH_LPM_PROTOCOL_IPV6_E) &&
         (ipGroupPrefix <= PRV_CPSS_DXCH_LPM_RAM_IPV6_MC_ADDRESS_SPACE_PREFIX_CNS))))
    {
        isFullGroupMask = GT_TRUE;
    }

    if ((isFullGroupMask == GT_TRUE) && (ipSrcPrefix == 0))
    {
        /* default can be only overwritten */
        if (override == GT_FALSE)
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_ALREADY_EXIST, LOG_ERROR_NO_MSG);

        /* save original def mc pointer */
        cpssOsMemCpy(&rollBackDefMcRoutePointer ,defMcRoutePointer,
                     sizeof(PRV_CPSS_DXCH_LPM_ROUTE_ENTRY_POINTER_STC));

        /* copy out the def mc pointer */
        cpssOsMemCpy(defMcRoutePointer ,mcRoutePointerPtr,
                     sizeof(PRV_CPSS_DXCH_LPM_ROUTE_ENTRY_POINTER_STC));

        /* set the default route */
        retVal = prvCpssDxChLpmRamMcDefRouteSet(vrId,defMcRoutePointer,
                                                shadowPtr,insertMode,protocolStack);
        if (retVal != GT_OK)
        {
            /*return the def mc pointer */
            cpssOsMemCpy(defMcRoutePointer ,&rollBackDefMcRoutePointer,
                         sizeof(PRV_CPSS_DXCH_LPM_ROUTE_ENTRY_POINTER_STC));

            /* new default set is failed */
            /* return original default   */
            prvCpssDxChLpmRamMcDefRouteSet(vrId,defMcRoutePointer,
                                           shadowPtr,insertMode,protocolStack);
            return retVal;
        }

        /* we overwritten the default MC, now we also have to update all
           the implicit (G,*) that points to the default MC */
        searchIpGroup[0] =
            (GT_U8)((protocolStack == PRV_CPSS_DXCH_LPM_PROTOCOL_IPV4_E) ?
            PRV_CPSS_DXCH_LPM_RAM_START_OF_IPV4_MC_ADDRESS_SPACE_CNS :
            PRV_CPSS_DXCH_LPM_RAM_START_OF_IPV6_MC_ADDRESS_SPACE_CNS);
        searchIpGroupPrefix =
            (protocolStack == PRV_CPSS_DXCH_LPM_PROTOCOL_IPV4_E) ?
            PRV_CPSS_DXCH_LPM_RAM_IPV4_MC_ADDRESS_SPACE_PREFIX_CNS :
            PRV_CPSS_DXCH_LPM_RAM_IPV6_MC_ADDRESS_SPACE_PREFIX_CNS;

        while(1)
        {
            /* get next group */
            retVal = cpssDxChLpmRamMcGroupGetNext(vrId, GT_TRUE, searchIpGroup,
                                                  &searchIpGroupPrefix,
                                                  &srcBucket,shadowPtr,protocolStack);
            /* check if there are no more groups */
            if (retVal == GT_NOT_FOUND)
            {
                retVal = GT_OK;
                break;
            }

            if (retVal != GT_OK)
                return retVal;

            if (srcBucket == NULL)
            {
                /*return the def mc pointer */
                cpssOsMemCpy(defMcRoutePointer ,&rollBackDefMcRoutePointer,
                             sizeof(PRV_CPSS_DXCH_LPM_ROUTE_ENTRY_POINTER_STC));

                /* src default update failed, activate rollback */
                prvCpssDxChLpmRamMcDefRollBack(vrId,defMcRoutePointer,shadowPtr,insertMode,
                                               protocolStack,srcBucket,ipSrcArr,rootBucketType,
                                               alreadyExists);
                CPSS_LOG_ERROR_AND_RETURN_MAC(GT_FAIL, LOG_ERROR_NO_MSG);

            }

            /* check if the (G,*) is explicit */
            if (srcBucket->isImplicitGroupDefault == GT_FALSE)
            {
                continue; /* to the next group */
            }

            /* update the new group default in its source trie */

            /* the root type for the src trie can be regular, compressed-1 or compressed-2 */
            rootBucketFlag = PRV_CPSS_DXCH_LPM_RAM_ROOT_BUCKET_WITH_RAM_UPDATE_E;

            /* src lpm search is done in the same way as UC lpm */
            lpmEngineMemPtr = shadowPtr->ucSearchMemArrayPtr[protocolStack];

            /* Insert the complete ip address to the lpm structure of PP */
            retVal = prvCpssDxChLpmRamMngInsert(srcBucket, ipSrcArr, 0, defMcRoutePointer,
                                                CPSS_DXCH_LPM_ROUTE_ENTRY_NEXT_PTR_TYPE_E,
                                                rootBucketFlag, lpmEngineMemPtr,
                                                &alreadyExists,&rootBucketType,
                                                shadowPtr, PRV_CPSS_DXCH_LPM_RAM_TRIE_INSERT_SDW_MEM_HW_MODE_E,
                                                GT_TRUE);
            if (retVal != GT_OK)
            {
                /*return the def mc pointer */
                cpssOsMemCpy(defMcRoutePointer ,&rollBackDefMcRoutePointer,
                             sizeof(PRV_CPSS_DXCH_LPM_ROUTE_ENTRY_POINTER_STC));

                /* src default update failed, activate rollback */
                prvCpssDxChLpmRamMcDefRollBack(vrId,defMcRoutePointer,shadowPtr,insertMode,
                                               protocolStack,srcBucket,ipSrcArr,rootBucketType,
                                               alreadyExists);
                return retVal;

            }
        }

        return retVal;
    }

    if ((protocolStack == PRV_CPSS_DXCH_LPM_PROTOCOL_IPV4_E) &&
        (ipGroupPrefix < PRV_CPSS_DXCH_LPM_RAM_IPV4_MC_ADDRESS_SPACE_PREFIX_CNS))
    {
        ipGroupPrefix = PRV_CPSS_DXCH_LPM_RAM_IPV4_MC_ADDRESS_SPACE_PREFIX_CNS;
    }
    else if ((protocolStack == PRV_CPSS_DXCH_LPM_PROTOCOL_IPV6_E) &&
             (ipGroupPrefix < PRV_CPSS_DXCH_LPM_RAM_IPV6_MC_ADDRESS_SPACE_PREFIX_CNS))
    {
        ipGroupPrefix = PRV_CPSS_DXCH_LPM_RAM_IPV6_MC_ADDRESS_SPACE_PREFIX_CNS;
    }

    retVal = prvCpssDxChLpmRamMcGroupSearch(vrId,ipGroupPtr,ipGroupPrefix,
                                           &srcBucket,shadowPtr,protocolStack);
    if ((GT_PTR*)srcBucket == (GT_PTR*)defMcRoutePointer)
    {
        /* if srcBucket and defMcRoutePointer are equal it means that we are
           adding (G,*) with group prefix of 0 and source prefix > 0 */

        if ((GT_PTR*)srcBucket != (GT_PTR*)shadowPtr->vrRootBucketArray[vrId].multicastDefault[protocolStack])
        {
            /* the group root bucket points to another bucket */
            updateOld = GT_TRUE;
            addNewSrcTree = GT_FALSE;
            tmpIsImplicitGroupDefault = GT_FALSE;
        }
        else
        {
            /* there is no next bucket for the group root bucket */
            updateOld = GT_FALSE;
            addNewSrcTree = GT_TRUE;
            tmpIsImplicitGroupDefault = GT_TRUE;
        }
    }
    else if(retVal == GT_OK) /* if the group already exists */
    {
        updateOld = GT_TRUE;
        if (ipSrcPrefix == 0)
            /* we add (G,*) to an existing group, therefore the (G,*) will be explicit */
            tmpIsImplicitGroupDefault = GT_FALSE;
        else
            /* we add source to an existing group, therefore the (G,*) implicit state doesn't change */
            tmpIsImplicitGroupDefault = srcBucket->isImplicitGroupDefault;
    }
    else /* the group doesn't exists */
    {
        updateOld = GT_FALSE;
    }

    /* There is no initialized LPM structure for this ip-group, initialize  */
    /* an LPM structure, with the default mc entry as the LPM default entry.*/
    if(updateOld == GT_FALSE)
    {
        srcBucket =
            prvCpssDxChLpmRamMngCreateNew(defMcRoutePointer,
                                          PRV_CPSS_DXCH_LPM_RAM_MAX_LEVEL_LENGTH_CNS);
        if(srcBucket == NULL)
        {
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_OUT_OF_CPU_MEM, LOG_ERROR_NO_MSG);
        }
        if (addNewSrcTree == GT_FALSE)
        {
            if (ipSrcPrefix == 0)
            {
                /* We create a new group and add the group default, therefore the
                   group WILL be explicit default. The (G,*) is currently marked as
                   implicit because it was created implicitly when creating srcBucket
                   but wasn't explicity added yet */
                srcBucket->isImplicitGroupDefault = GT_TRUE;
                tmpIsImplicitGroupDefault = GT_FALSE;
            }
            else
            {
                /* We add source to a new group, therefore an implicit group
                   default will be added */
                srcBucket->isImplicitGroupDefault = GT_TRUE;
                tmpIsImplicitGroupDefault = GT_TRUE;
            }
        }
    }

    /* Check if an entry with the same (ipSrcArr,ipSrcPrefix) already exists;
       if yes, delete the old one and create a new entry. */
    if (addNewSrcTree == GT_FALSE)
    {
        retVal = prvCpssDxChLpmRamMngSearch(srcBucket,ipSrcArr,ipSrcPrefix,
                                            (GT_PTR*)(GT_UINTPTR)&nextHopEntry);
    }
    else
    {
        retVal = GT_NOT_FOUND;
    }

    if (retVal == GT_OK)
    {
        /* save original nextHopEntry  */
        cpssOsMemCpy(&rollBackDefMcRoutePointer ,nextHopEntry,
                     sizeof(PRV_CPSS_DXCH_LPM_ROUTE_ENTRY_POINTER_STC));
    }

    /* check if the MC entry exists and was explicitly added */
    if (((retVal == GT_OK) && (ipSrcPrefix != 0)) ||
        ((retVal == GT_OK) && (ipSrcPrefix == 0) && (srcBucket->isImplicitGroupDefault == GT_FALSE)))
    {
        if (override == GT_FALSE)
        {
            if (updateOld == GT_FALSE)
                prvCpssDxChLpmRamMngBucketDelete(srcBucket,8,NULL);
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_ALREADY_EXIST, LOG_ERROR_NO_MSG);
        }

        alreadyExists = GT_TRUE;
    }
    else
    {
        /* new entry allocate space */
        nextHopEntry = cpssOsLpmMalloc(sizeof(PRV_CPSS_DXCH_LPM_ROUTE_ENTRY_POINTER_STC));
        if (nextHopEntry == NULL)
        {
            if (updateOld == GT_FALSE)
                prvCpssDxChLpmRamMngBucketDelete(srcBucket,8,NULL);
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_OUT_OF_CPU_MEM, LOG_ERROR_NO_MSG);
        }
    }

    /* copy out the mc pointer */
    cpssOsMemCpy(nextHopEntry ,mcRoutePointerPtr,
                 sizeof(PRV_CPSS_DXCH_LPM_ROUTE_ENTRY_POINTER_STC));

    /* the root type for the src trie can be regular, compressed-1 or compressed-2 */
    rootBucketFlag = PRV_CPSS_DXCH_LPM_RAM_ROOT_BUCKET_WITH_RAM_UPDATE_E;

    /* in case of hot sync no need to check and allocate , it's done in the end*/
    if (insertMode != PRV_CPSS_DXCH_LPM_RAM_TRIE_INSERT_SDW_ONLY_MODE_E)
    {
        /* keep values in case UC reconstruct is needed */
        cpssOsMemCpy(tempLpmRamOctetsToBlockMapping,shadowPtr->lpmRamOctetsToBlockMappingPtr,
                     sizeof(PRV_CPSS_DXCH_LPM_RAM_OCTETS_TO_BLOCK_MAPPING_STC)*shadowPtr->numOfLpmMemories);

        /* Check memory availability. */
        retVal = checkAvailableMcSearchMemory(srcBucket,vrId,ipGroupPtr,
                                              ipGroupPrefix,ipSrcArr,ipSrcPrefix,
                                              rootBucketFlag,updateOld,
                                              protocolStack,shadowPtr);
        if (retVal != GT_OK)
        {
            if (updateOld == GT_FALSE)
                prvCpssDxChLpmRamMngBucketDelete(srcBucket,8,NULL);
            if (alreadyExists == GT_FALSE)
                cpssOsLpmFree(nextHopEntry);
            return (retVal);
        }
    }

    rootBucketType = srcBucket->bucketType;

    /* set src lpm search */
    /* src lpm search is done in the same way as UC lpm */
    lpmEngineMemPtr = shadowPtr->ucSearchMemArrayPtr[protocolStack];

    PRV_CPSS_DXCH_LPM_NEXT_PTR_TYPE_CONVERT_MAC(nextHopEntry->routeEntryMethod,nextHopEntryType);
    /* Insert the complete ip address to the lpm structure of PP */
    retVal = prvCpssDxChLpmRamMngInsert(srcBucket, ipSrcArr, ipSrcPrefix, nextHopEntry,
                                        nextHopEntryType,
                                        rootBucketFlag, lpmEngineMemPtr,
                                        &alreadyExists, &rootBucketType,
                                        shadowPtr, insertMode, GT_TRUE);
    if(retVal != GT_OK)
    {
        shadowPtr->neededMemoryListLen = 0;
        prvCpssDxChLpmRamMemFreeListMng(0,PRV_CPSS_DXCH_LPM_RAM_MEM_LIST_FREE_MEM_OP_E,
                                        &shadowPtr->freeMemListEndOfUpdate);
        /* update the info for the parent write function */
        /* get the old bucket type */
        newBucketType = srcBucket->bucketType;
        lpmPtr.nextBucket = srcBucket;
        writeFuncData.srcBucketPtr = srcBucket;
        if ((updateOld == GT_TRUE) && (ipSrcPrefix == 0))
        {
            /* It was G,* insertion */
            /* return previous G,* */
            if (srcBucket->isImplicitGroupDefault == GT_TRUE)
            {
                /* G,* wasn't explicit before, restore default */
                rollBackNextHopEntryPtr = defMcRoutePointer;
            }
            else
            {
                /* restore original nextHopEntry  */
                cpssOsMemCpy(nextHopEntry,&rollBackDefMcRoutePointer,
                             sizeof(PRV_CPSS_DXCH_LPM_ROUTE_ENTRY_POINTER_STC));
                rollBackNextHopEntryPtr = nextHopEntry;
            }


            prvCpssDxChLpmRamMngInsert(srcBucket, ipSrcArr, ipSrcPrefix, rollBackNextHopEntryPtr,
                                        CPSS_DXCH_LPM_ROUTE_ENTRY_NEXT_PTR_TYPE_E,
                                        rootBucketFlag, lpmEngineMemPtr,
                                        &alreadyExists,&rootBucketType,
                                        shadowPtr, PRV_CPSS_DXCH_LPM_RAM_TRIE_INSERT_SDW_MEM_HW_MODE_E,
                                        GT_TRUE);
            if ((updateOld == GT_TRUE) && (ipSrcPrefix == 0) && (srcBucket->isImplicitGroupDefault == GT_FALSE))
            {
                alreadyExists = GT_TRUE;
            }
        }
        else
        /* Delete the given ip-src from the LPM structure   */
            prvCpssDxChLpmRamMngEntryDelete(&lpmPtr,ipSrcArr,ipSrcPrefix,
                                            PRV_CPSS_DXCH_LPM_RAM_ROOT_BUCKET_WITHOUT_COMPRESSED_2_E,
                                            GT_TRUE,lpmEngineMemPtr,&newBucketType,
                                            (GT_PTR*)(GT_UINTPTR)&delEntry,shadowPtr,
                                            &parentWriteFuncInfo,GT_TRUE,GT_TRUE);
        if (updateOld == GT_TRUE)
        {
            prvCpssDxChLpmRamMcGroupInsert(vrId,ipGroupPtr,ipGroupPrefix,
                                           srcBucket,shadowPtr,
                                           PRV_CPSS_DXCH_LPM_RAM_TRIE_INSERT_SDW_MEM_HW_MODE_E,
                                           protocolStack);
        }

        prvCpssDxChLpmRamMemFreeListMng(0,PRV_CPSS_DXCH_LPM_RAM_MEM_LIST_FREE_MEM_OP_E,
                                        &shadowPtr->freeMemListEndOfUpdate);
        if (updateOld == GT_FALSE)
            prvCpssDxChLpmRamMngBucketDelete(srcBucket,8,NULL);
        if (alreadyExists == GT_FALSE)
            cpssOsLpmFree(nextHopEntry);

        if (insertMode != PRV_CPSS_DXCH_LPM_RAM_TRIE_INSERT_SDW_ONLY_MODE_E)
        {
            /* free the allocated/bound RAM memory */
            retVal2 = prvCpssDxChLpmRamMngAllocatedAndBoundMemFree(shadowPtr,protocolStack);
            if (retVal2!=GT_OK)
            {
                return retVal2;
            }
            /* in case of fail we will need to recondtruct to the status we had before the call to prvCpssDxChLpmRamMngAllocAvailableMemCheck */
            cpssOsMemCpy(shadowPtr->lpmRamOctetsToBlockMappingPtr,tempLpmRamOctetsToBlockMapping,
                         sizeof(PRV_CPSS_DXCH_LPM_RAM_OCTETS_TO_BLOCK_MAPPING_STC)*shadowPtr->numOfLpmMemories);
        }

        return retVal;
    }

    /* Insert the MC Group ip address to the lpm structure of PP */
    retVal = prvCpssDxChLpmRamMcGroupInsert(vrId, ipGroupPtr, ipGroupPrefix,
                                            srcBucket, shadowPtr,
                                            insertMode, protocolStack);

    if (retVal != GT_OK)
    {
        shadowPtr->neededMemoryListLen = 0;
        prvCpssDxChLpmRamMemFreeListMng(0,PRV_CPSS_DXCH_LPM_RAM_MEM_LIST_FREE_MEM_OP_E,
                                        &shadowPtr->freeMemListEndOfUpdate);

        /* insert group is failed, activate rollBack */
        prvCpssDxChLpmRamMcEntryDelete(vrId,ipGroupPtr,ipGroupPrefix,ipSrcArr,
                                       ipSrcPrefix,protocolStack,shadowPtr,GT_TRUE,srcBucket);
        if (insertMode != PRV_CPSS_DXCH_LPM_RAM_TRIE_INSERT_SDW_ONLY_MODE_E)
        {
            /* free the allocated/bound RAM memory */
            retVal2 = prvCpssDxChLpmRamMngAllocatedAndBoundMemFree(shadowPtr,protocolStack);
            if (retVal2!=GT_OK)
            {
                return retVal2;
            }
            /* in case of fail we will need to recondtruct to the status we had before the call to prvCpssDxChLpmRamMngAllocAvailableMemCheck */
            cpssOsMemCpy(shadowPtr->lpmRamOctetsToBlockMappingPtr,tempLpmRamOctetsToBlockMapping,
                         sizeof(PRV_CPSS_DXCH_LPM_RAM_OCTETS_TO_BLOCK_MAPPING_STC)*shadowPtr->numOfLpmMemories);
        }
        return retVal;
    }

    /* in case of shadow update only no need to check since we didn't allocate
       nothing*/
    if (insertMode != PRV_CPSS_DXCH_LPM_RAM_TRIE_INSERT_SDW_ONLY_MODE_E)
    {
        /* check that all the pre-allocated memory was used */
        if (shadowPtr->neededMemoryCurIdx != shadowPtr->neededMemoryListLen)
        {
            /* should never! - meaning we didn't use all preallcoated memory
               FATAL ERROR*/

            /* free the allocated/bound RAM memory */
            retVal2 = prvCpssDxChLpmRamMngAllocatedAndBoundMemFree(shadowPtr,protocolStack);
            if (retVal2!=GT_OK)
            {
                return retVal2;
            }
            /* in case of fail we will need to recondtruct to the status we had before the call to prvCpssDxChLpmRamMngAllocAvailableMemCheck */
            cpssOsMemCpy(shadowPtr->lpmRamOctetsToBlockMappingPtr,tempLpmRamOctetsToBlockMapping,
                         sizeof(PRV_CPSS_DXCH_LPM_RAM_OCTETS_TO_BLOCK_MAPPING_STC)*shadowPtr->numOfLpmMemories);

            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_FAIL, LOG_ERROR_NO_MSG);
        }
    }

    /* MC entry was added successfully, set group default implicit state */
    srcBucket->isImplicitGroupDefault = tmpIsImplicitGroupDefault;

    shadowPtr->neededMemoryListLen = 0;

    /* the data was written successfully to HW, we can reset the information regarding the new memoryPool allocations done.
       next call to ADD will set this array with new values of allocated/bound blocks */
    cpssOsMemSet(shadowPtr->allNewNextMemInfoAllocatedPerOctetArrayPtr,0,sizeof(shadowPtr->allNewNextMemInfoAllocatedPerOctetArrayPtr));

    retVal2 = prvCpssDxChLpmRamMemFreeListMng(0,PRV_CPSS_DXCH_LPM_RAM_MEM_LIST_FREE_MEM_OP_E,
                                              &shadowPtr->freeMemListEndOfUpdate);

    if(retVal==GT_OK)
        return retVal2;

    return retVal;
}


/*******************************************************************************
* prvCpssDxChLpmRamMcEntryDelete
*
* DESCRIPTION:
*       To delete a particular mc route
*
* INPUTS:
*       vrId                    - The virtual private network identifier.
*       ipGroupPtr              - The IP multicast group address.
*       ipGroupPrefix           - The number of bits that are actual valid in,
*                                 the ipGroup. (CIDR like)
*       ipSrcArr                 - the root address for source base multi tree protocol.
*       ipSrcPrefix              - The number of bits that are actual valid in,
*                                  the ipSrcArr. (CIDR like)
*       protocolStack            - type of ip protocol stack to work on.
*       shadowPtr                - the shadow relevant for the devices asked to act on.
*       roollBack                - GT_TRUE: rollback is taking place.
*                                  GT_FALSE: otherwise.
*       rollBackSrcBucketPtr     - pointer to src bucket.(used only for rollback)
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success, or
*       GT_ERROR                 - if the virtual router does not exist, or
*       GT_NOT_FOUND             - if the (ipGroup,prefix) does not exist, or
*       GT_OUT_OF_CPU_MEM        - if failed to allocate CPU memory, or
*       GT_OUT_OF_PP_MEM         - if failed to allocate PP memory, or
*       GT_FAIL                  - otherwise.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       In order to delete the all the multicast entries, call this function
*       with ipSrcArr[0] = ipSrcPrefix = 0.
*
*******************************************************************************/
GT_STATUS prvCpssDxChLpmRamMcEntryDelete
(
    IN GT_U32                                   vrId,
    IN GT_U8                                    *ipGroupPtr,
    IN GT_U32                                   ipGroupPrefix,
    IN GT_U8                                    ipSrcArr[],
    IN GT_U32                                   ipSrcPrefix,
    IN PRV_CPSS_DXCH_LPM_PROTOCOL_STACK_ENT     protocolStack,
    IN PRV_CPSS_DXCH_LPM_RAM_SHADOW_STC         *shadowPtr,
    IN GT_BOOL                                  rollBack,
    IN PRV_CPSS_DXCH_LPM_RAM_BUCKET_SHADOW_STC  *rollBackSrcBucketPtr
)
{
    PRV_CPSS_DXCH_LPM_RAM_BUCKET_SHADOW_STC     *srcBucket;
    PRV_CPSS_DXCH_LPM_ROUTE_ENTRY_POINTER_STC   *defMcRoutePointer;
    CPSS_DXCH_LPM_NEXT_POINTER_TYPE_ENT         defMcRoutePointerType;

    PRV_CPSS_DXCH_LPM_RAM_POINTER_SHADOW_UNT lpmPtr; /* Will hold the pointer to the IP-src LPM  */
                                                     /* structure for delete operations.         */

    CPSS_DXCH_LPM_NEXT_POINTER_TYPE_ENT newBucketType;  /* Holds the LPM first bucket type after    */
                                /* delete operations.                       */

    PRV_CPSS_DXCH_LPM_ROUTE_ENTRY_POINTER_STC *delEntry; /* A pointer to the data saved in the trie, */
                                /* for IP-src LPM history purposes. And     */
                                /* should be deleted.                       */

    PRV_CPSS_DXCH_LPM_RAM_MEM_INFO_STC     **lpmEngineMemPtr;
    PRV_CPSS_DXCH_LPM_RAM_ROOT_BUCKET_FLAG_ENT rootBucketFlag;
    GT_STATUS retVal;
    CPSS_DXCH_LPM_NEXT_POINTER_TYPE_ENT rootBucketType;  /* The bucket type of the root bucket*/
                                        /* of the LPM structure that holds  */
                                        /* the IP-Mc src prefixes.          */
    GT_BOOL alreadyExists = GT_FALSE;   /* Indicates if the given Src prefix*/
                                        /* already exists in the LPM        */
                                        /* structure.                       */
    PRV_CPSS_DXCH_LPM_RAM_BUCKET_PARENT_WRITE_FUNC_INFO_STC parentWriteFuncInfo;

    SHADOW_IP_MC_GROUP_INSERT_FUNC_PARAMS_STC writeFuncData;
    GT_BOOL     deleteEmptyGroup = GT_FALSE;   /* if group is empty and need to be deleted */
    GT_BOOL     isFullGroupMask = GT_FALSE;

    /* first check the ip version was initialized */
    if (shadowPtr->isProtocolInitialized[protocolStack] == GT_FALSE)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_INITIALIZED, LOG_ERROR_NO_MSG);
    }

    if ((protocolStack == PRV_CPSS_DXCH_LPM_PROTOCOL_IPV4_E) &&
        (ipGroupPrefix <= PRV_CPSS_DXCH_LPM_RAM_IPV4_MC_ADDRESS_SPACE_PREFIX_CNS))
    {
        isFullGroupMask = GT_TRUE;
        ipGroupPrefix = PRV_CPSS_DXCH_LPM_RAM_IPV4_MC_ADDRESS_SPACE_PREFIX_CNS;
    }
    else if ((protocolStack == PRV_CPSS_DXCH_LPM_PROTOCOL_IPV6_E) &&
             (ipGroupPrefix <= PRV_CPSS_DXCH_LPM_RAM_IPV6_MC_ADDRESS_SPACE_PREFIX_CNS))
    {
        isFullGroupMask = GT_TRUE;
        ipGroupPrefix = PRV_CPSS_DXCH_LPM_RAM_IPV6_MC_ADDRESS_SPACE_PREFIX_CNS;
    }

    /* can't delete the default route. only overwrite it */
    if ((isFullGroupMask == GT_TRUE) && (ipSrcPrefix == 0))
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    if((vrId >= shadowPtr->vrfTblSize) ||
       (shadowPtr->vrRootBucketArray[vrId].valid == GT_FALSE))
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(((GT_STATUS)GT_ERROR), LOG_ERROR_NO_MSG);
    }

    /* verify that multicast is supported for this protocol in the VR */
    if (shadowPtr->vrRootBucketArray[vrId].isMulticastSupported[protocolStack] == GT_FALSE)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_SUPPORTED, LOG_ERROR_NO_MSG);
    }

    /* we can't use compressed 2 as a root type for the src trie */
    rootBucketFlag = PRV_CPSS_DXCH_LPM_RAM_ROOT_BUCKET_WITHOUT_COMPRESSED_2_E;

     /* src lpm search is done in the same way as UC lpm */
    lpmEngineMemPtr = shadowPtr->ucSearchMemArrayPtr[protocolStack];

    retVal = prvCpssDxChLpmRamMemFreeListMng(0,PRV_CPSS_DXCH_LPM_RAM_MEM_LIST_RESET_OP_E,
                                             &shadowPtr->freeMemListEndOfUpdate);
    if (retVal != GT_OK)
        return (retVal);

    if (rollBack == GT_FALSE)
    {
        retVal = prvCpssDxChLpmRamMcGroupSearch(vrId,ipGroupPtr,ipGroupPrefix,
                                                &srcBucket,shadowPtr,protocolStack);
    }
    else
    {
        srcBucket = rollBackSrcBucketPtr;
    }

    /* if the group wasn't found */
    if(retVal != GT_OK)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_FOUND, LOG_ERROR_NO_MSG);
    }

    /* get the default route */
    retVal = prvCpssDxChLpmRamMcDefRouteGet(vrId,&defMcRoutePointer,
                                            shadowPtr,protocolStack);
    if (retVal != GT_OK)
    {
        /* shouldn't happen, there should always be a default */
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_FAIL, LOG_ERROR_NO_MSG);
    }

    /* if we delete (G,*) */
    if(ipSrcPrefix == 0)
    {
        /* if the (G,*) is implicit, it can't be deleted explicitly */
        if (srcBucket->isImplicitGroupDefault == GT_TRUE)
        {
            if (rollBack == GT_FALSE)
            {
                CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_FOUND, LOG_ERROR_NO_MSG);
            }
        }

        /* if there are no sources attached to the group, delete the group */
        if (srcBucket->bucketType == CPSS_DXCH_LPM_ROUTE_ENTRY_NEXT_PTR_TYPE_E)
        {
            /* mark the group as need to be deleted */
            deleteEmptyGroup = GT_TRUE;
        }
        /* there are still sources attached to the group, therefore just */
        /* set the group default next hop to the MC default hext hop     */
        else
        {
            /* get next hop attached to (G,*) */
            retVal = prvCpssDxChLpmRamMngSearch(srcBucket,ipSrcArr,ipSrcPrefix,(GT_PTR*)(GT_UINTPTR)&delEntry);
            if (retVal != GT_OK)
            {
                /* shouldn't happen */
                CPSS_LOG_ERROR_AND_RETURN_MAC(GT_FAIL, LOG_ERROR_NO_MSG);
            }

            /* Note we already know the (G,*) is explicit.
               Replace the next hop with MC default next hop and
               mark the (G,*) as implicit */

            /* set root bucket type */
            rootBucketType = srcBucket->bucketType;

            /* src lpm search is done in the same way as UC lpm */
            lpmEngineMemPtr = shadowPtr->ucSearchMemArrayPtr[protocolStack];

            PRV_CPSS_DXCH_LPM_NEXT_PTR_TYPE_CONVERT_MAC(defMcRoutePointer->routeEntryMethod,defMcRoutePointerType);
            /* Insert the complete ip address to the lpm structure of PP */
            retVal = prvCpssDxChLpmRamMngInsert(srcBucket, ipSrcArr, ipSrcPrefix, defMcRoutePointer,
                                                defMcRoutePointerType,
                                                rootBucketFlag, lpmEngineMemPtr,
                                                &alreadyExists,&rootBucketType,
                                                shadowPtr, PRV_CPSS_DXCH_LPM_RAM_TRIE_INSERT_SDW_MEM_HW_MODE_E,
                                                GT_TRUE);
            if (retVal != GT_OK)
                return retVal;

            retVal = prvCpssDxChLpmRamMcGroupInsert(vrId,ipGroupPtr,ipGroupPrefix,
                                                    srcBucket,shadowPtr,
                                                    PRV_CPSS_DXCH_LPM_RAM_TRIE_INSERT_SDW_MEM_HW_MODE_E,
                                                    protocolStack);
            if (retVal != GT_OK)
                return retVal;

            /* free old next hop */
            cpssOsLpmFree(delEntry);

            /* mark the (G,*) as implicit */
            srcBucket->isImplicitGroupDefault = GT_TRUE;
        }
    }
    /* we delete (G,S) and not (G,*) */
    else
    {
        /* update the info for the parent write function */
        parentWriteFuncInfo.bucketParentWriteFunc =
            coreIpMcGroupInsertFuncWrap;

        parentWriteFuncInfo.data = (GT_PTR)(&writeFuncData);

        writeFuncData.vrId         = vrId;
        writeFuncData.ipGroup     = ipGroupPtr;
        writeFuncData.ipGroupPrefix = ipGroupPrefix;
        writeFuncData.srcBucketPtr = srcBucket;
        writeFuncData.shadowPtr = shadowPtr;
        writeFuncData.insertMode = PRV_CPSS_DXCH_LPM_RAM_TRIE_INSERT_SDW_MEM_HW_MODE_E;

        /* get the old bucket type */
        newBucketType = srcBucket->bucketType;

        lpmPtr.nextBucket = srcBucket;

        /* Delete the given ip-src from the LPM structure   */
        retVal = prvCpssDxChLpmRamMngEntryDelete(&lpmPtr,ipSrcArr,ipSrcPrefix,rootBucketFlag,
                                                 GT_TRUE,lpmEngineMemPtr,&newBucketType,
                                                 (GT_PTR*)(GT_UINTPTR)&delEntry,shadowPtr,
                                                 &parentWriteFuncInfo,GT_TRUE,rollBack);
        if(retVal != GT_OK)
        {
            prvCpssDxChLpmRamMemFreeListMng(0,PRV_CPSS_DXCH_LPM_RAM_MEM_LIST_FREE_MEM_OP_E,
                                            &shadowPtr->freeMemListEndOfUpdate);

            return retVal;
        }

        if(retVal == GT_OK)
        {
            cpssOsLpmFree(delEntry);
        }
        if (rollBack == GT_FALSE)
        {
            retVal = prvCpssDxChLpmRamMcGroupInsert(vrId,ipGroupPtr,ipGroupPrefix,
                                                    srcBucket,shadowPtr,
                                                    PRV_CPSS_DXCH_LPM_RAM_TRIE_INSERT_SDW_MEM_HW_MODE_E,
                                                    protocolStack);
            if (retVal != GT_OK)
            {
                return retVal;
            }
        }
        else
        {
            if (!((srcBucket->bucketType == CPSS_DXCH_LPM_ROUTE_ENTRY_NEXT_PTR_TYPE_E) &&
            (srcBucket->isImplicitGroupDefault == GT_TRUE)))
            {
                retVal = prvCpssDxChLpmRamMcGroupInsert(vrId,ipGroupPtr,ipGroupPrefix,
                                                        srcBucket,shadowPtr,
                                                        PRV_CPSS_DXCH_LPM_RAM_TRIE_INSERT_SDW_MEM_HW_MODE_E,
                                                        protocolStack);
            }
        }

        /* if there are no more sources attached to the group and the (G,*) is
           implicit, then delete the group */
        if ((srcBucket->bucketType == CPSS_DXCH_LPM_ROUTE_ENTRY_NEXT_PTR_TYPE_E) &&
            (srcBucket->isImplicitGroupDefault == GT_TRUE))
        {
            /* mark the group as need to be deleted */
            deleteEmptyGroup = GT_TRUE;
        }
    }

    /* if the group was marked as need to be deleted, delete it */
    if (deleteEmptyGroup == GT_TRUE)
    {
        retVal = prvCpssDxChLpmRamMcGroupDelete(vrId,ipGroupPtr,ipGroupPrefix,
                                                &srcBucket,shadowPtr,protocolStack,rollBack);
        if(retVal != GT_OK)
        {
            prvCpssDxChLpmRamMemFreeListMng(0,PRV_CPSS_DXCH_LPM_RAM_MEM_LIST_FREE_MEM_OP_E,
                                            &shadowPtr->freeMemListEndOfUpdate);
            return retVal;
        }

        retVal = prvCpssDxChLpmRamMngBucketDelete(srcBucket,8,(GT_PTR*)(GT_UINTPTR)&delEntry);

        if(retVal != GT_OK)
        {
            /*No need to check the return value of prvCpssDxChLpmMemFreeListMng
              because in this scenario we will return the error from
              prvCpssExMxPmLpmDeleteBucket*/
            prvCpssDxChLpmRamMemFreeListMng(0,PRV_CPSS_DXCH_LPM_RAM_MEM_LIST_FREE_MEM_OP_E,
                                            &shadowPtr->freeMemListEndOfUpdate);

            return retVal;
        }

        if ((isFullGroupMask == GT_TRUE) && (srcBucket->isImplicitGroupDefault == GT_TRUE))
        {
            retVal = prvCpssDxChLpmRamMcDefRouteSet(vrId, defMcRoutePointer,
                                                    shadowPtr,
                                                    PRV_CPSS_DXCH_LPM_RAM_TRIE_INSERT_SDW_MEM_HW_MODE_E,
                                                    protocolStack);
            if (retVal != GT_OK)
            {
                return retVal;
            }
        }

        /* check if this is the default next hop , if not free it */
        if(delEntry != defMcRoutePointer)
        {
            cpssOsLpmFree(delEntry);
        }
    }

    retVal = prvCpssDxChLpmRamMemFreeListMng(0,PRV_CPSS_DXCH_LPM_RAM_MEM_LIST_FREE_MEM_OP_E,
                                             &shadowPtr->freeMemListEndOfUpdate);
    if (retVal != GT_OK)
    {
        return retVal;
    }

    return GT_OK;
}

/*******************************************************************************
* prvCpssDxChLpmRamMcEntriesFlush
*
* DESCRIPTION:
*       flushes the multicast IP Routing table and stays with the default entry
*       only.
*
* INPUTS:
*       vrId           - The virtual private network identifier.
*       protocolStack  - protocol to work on.
*       shadowPtr      - the shadow relevant for the devices asked to act on.
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
GT_STATUS prvCpssDxChLpmRamMcEntriesFlush
(
    IN GT_U32                               vrId,
    IN PRV_CPSS_DXCH_LPM_PROTOCOL_STACK_ENT protocolStack,
    IN PRV_CPSS_DXCH_LPM_RAM_SHADOW_STC     *shadowPtr
)
{
    GT_U8 ipGroup[PRV_CPSS_DXCH_LPM_MAX_NUM_OF_OCTETS_FOR_ADDRESS_CNS]; /* The Multicast group IP address to be deleted at each loop */
    GT_U32 ipGroupPrefix;   /* The Multicast group IP address prefix to be deleted. */
    GT_U8 ipSrc[PRV_CPSS_DXCH_LPM_MAX_NUM_OF_OCTETS_FOR_ADDRESS_CNS];   /* The source IP address to be deleted */
    GT_U32 ipSrcPrefix;     /* The source IP address prefix to be deleted */

    GT_STATUS retVal = GT_OK;

    /* first check the ip version was initialized */
    if (shadowPtr->isProtocolInitialized[protocolStack] == GT_FALSE)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_INITIALIZED, LOG_ERROR_NO_MSG);
    }

    if((vrId >= shadowPtr->vrfTblSize) ||
       (shadowPtr->vrRootBucketArray[vrId].valid == GT_FALSE))
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(((GT_STATUS)GT_ERROR), LOG_ERROR_NO_MSG);
    }

    while(1)
    {
        if (protocolStack == PRV_CPSS_DXCH_LPM_PROTOCOL_IPV4_E)
        {
            ipGroup[0] = PRV_CPSS_DXCH_LPM_RAM_START_OF_IPV4_MC_ADDRESS_SPACE_CNS;
            ipGroupPrefix = PRV_CPSS_DXCH_LPM_RAM_IPV4_MC_ADDRESS_SPACE_PREFIX_CNS;
        }
        else
        {
            ipGroup[0] = PRV_CPSS_DXCH_LPM_RAM_START_OF_IPV6_MC_ADDRESS_SPACE_CNS;
            ipGroupPrefix = PRV_CPSS_DXCH_LPM_RAM_IPV6_MC_ADDRESS_SPACE_PREFIX_CNS;
        }
        cpssOsMemSet(&ipGroup[1], 0, sizeof(GT_U8) * (PRV_CPSS_DXCH_LPM_MAX_NUM_OF_OCTETS_FOR_ADDRESS_CNS - 1));
        cpssOsMemSet(ipSrc, 0, sizeof(GT_U8) * PRV_CPSS_DXCH_LPM_MAX_NUM_OF_OCTETS_FOR_ADDRESS_CNS);
        ipSrcPrefix = 0;

        /* get next entry */
        retVal = prvCpssDxChLpmRamMcEntryGetNext(vrId,ipGroup,&ipGroupPrefix,
                                                 ipSrc,&ipSrcPrefix,NULL,
                                                 protocolStack,shadowPtr);

        /* check if there are no more entries */
        if (retVal == GT_NOT_FOUND)
        {
            retVal = GT_OK;
            break;
        }

        if (retVal != GT_OK)
            return retVal;

        /* delete the enrty found */
        retVal = prvCpssDxChLpmRamMcEntryDelete(vrId,ipGroup,ipGroupPrefix,ipSrc,
                                                ipSrcPrefix,protocolStack,shadowPtr,GT_FALSE,NULL);

        if (retVal != GT_OK)
            return retVal;
    }

    return retVal;
}


/*******************************************************************************
* prvCpssDxChLpmRamMcEntryGetNext
*
* DESCRIPTION:
*       This function returns the next muticast (ipSrc,ipGroup) entry.
*
* INPUTS:
*       vrId    - The virtual router Id.
*       ipGroupPtr - The ip Group address to get the next entry for.
*       ipGroupPrefixPtr - ipGroup prefix length.
*       ipSrcPtr   - The ip Source address to get the next entry for.
*       ipSrcPrefixPtr - ipSrc prefix length.
*       protocolStack  - type of ip protocol stack to work on.
*       shadowPtr      - the shadow relevant for the devices asked to act on.
*
* OUTPUTS:
*       ipGroupPtr         - The next ip Group address.
*       ipGroupPrefixPtr   - ipGroup prefix length.
*       ipSrcPtr           - The next ip Source address. (must be 16 x GT_U8!)
*       ipSrcPrefixPtr     - ipSrc prefix length.
*       mcRoutePointerPtr- the mc route entry (NH) pointer.
*
* RETURNS:
*       GT_OK if found, or
*       GT_NOT_FOUND - If the given address is the last one on the IP-Mc table.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       1.  The values of (ipGroup,ipGroupPrefix) must be a valid values, it
*           means that they exist in the IP-Mc Table, unless this is the first
*           call to this function, then the value of (ipGroupPrefix) is (0).
*
*******************************************************************************/
GT_STATUS prvCpssDxChLpmRamMcEntryGetNext
(
    IN    GT_U32                                    vrId,
    INOUT GT_U8                                     *ipGroupPtr,
    INOUT GT_U32                                    *ipGroupPrefixPtr,
    INOUT GT_U8                                     *ipSrcPtr,
    INOUT GT_U32                                    *ipSrcPrefixPtr,
    OUT   PRV_CPSS_DXCH_LPM_ROUTE_ENTRY_POINTER_STC *mcRoutePointerPtr,
    IN    PRV_CPSS_DXCH_LPM_PROTOCOL_STACK_ENT      protocolStack,
    IN    PRV_CPSS_DXCH_LPM_RAM_SHADOW_STC          *shadowPtr
)
{
    PRV_CPSS_DXCH_LPM_RAM_BUCKET_SHADOW_STC *bucketPtr;   /* Holds a pointer to the shadow root*/
                                /* bucket of the ip-src LPM structure.      */
    PRV_CPSS_DXCH_LPM_ROUTE_ENTRY_POINTER_STC *nextHopEntry = NULL;

    GT_BOOL newGroup;           /* Indicates whether this is the first time */
                                /* a certain group is searched.             */

    PRV_CPSS_DXCH_LPM_ROUTE_ENTRY_POINTER_STC *defMcRoutePointer;
    GT_STATUS retVal;

    /* first check the ip version was initialized */
    if (shadowPtr->isProtocolInitialized[protocolStack] == GT_FALSE)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_INITIALIZED, LOG_ERROR_NO_MSG);
    }

    newGroup        = GT_FALSE;

    if ((protocolStack == PRV_CPSS_DXCH_LPM_PROTOCOL_IPV4_E) && (*ipGroupPrefixPtr == 0))
    {
        ipGroupPtr[0] = PRV_CPSS_DXCH_LPM_RAM_START_OF_IPV4_MC_ADDRESS_SPACE_CNS;
    }

    if ((protocolStack == PRV_CPSS_DXCH_LPM_PROTOCOL_IPV6_E) && (*ipGroupPrefixPtr == 0))
    {
        ipGroupPtr[0] = PRV_CPSS_DXCH_LPM_RAM_START_OF_IPV6_MC_ADDRESS_SPACE_CNS;
    }

    if ((((protocolStack == PRV_CPSS_DXCH_LPM_PROTOCOL_IPV4_E) &&
          (ipGroupPtr[0] == PRV_CPSS_DXCH_LPM_RAM_START_OF_IPV4_MC_ADDRESS_SPACE_CNS) &&
          (*ipGroupPrefixPtr <= PRV_CPSS_DXCH_LPM_RAM_IPV4_MC_ADDRESS_SPACE_PREFIX_CNS)) ||
         ((protocolStack == PRV_CPSS_DXCH_LPM_PROTOCOL_IPV6_E) &&
          (ipGroupPtr[0] == PRV_CPSS_DXCH_LPM_RAM_START_OF_IPV6_MC_ADDRESS_SPACE_CNS) &&
          (*ipGroupPrefixPtr <= PRV_CPSS_DXCH_LPM_RAM_IPV6_MC_ADDRESS_SPACE_PREFIX_CNS))) &&
        (*ipSrcPrefixPtr == 0))
    {
        *ipGroupPrefixPtr =
            (protocolStack == PRV_CPSS_DXCH_LPM_PROTOCOL_IPV4_E) ?
            PRV_CPSS_DXCH_LPM_RAM_IPV4_MC_ADDRESS_SPACE_PREFIX_CNS :
            PRV_CPSS_DXCH_LPM_RAM_IPV6_MC_ADDRESS_SPACE_PREFIX_CNS;

        /* search for 224 (IPv4) or 255 (IPv6) */
        retVal = prvCpssDxChLpmRamMcGroupSearch(vrId,ipGroupPtr,*ipGroupPrefixPtr,
                                                &bucketPtr,shadowPtr,protocolStack);
        if ((retVal != GT_OK) || (bucketPtr == NULL))
        {
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_FOUND, LOG_ERROR_NO_MSG);
        }

        if ((GT_PTR*)bucketPtr == (GT_PTR*)shadowPtr->vrRootBucketArray[vrId].multicastDefault[protocolStack])
        {
            /* if we reached here than we don't have a 224/4 (IPv4) or 255/8 (IPv6) + src.
               For 224/4 (IPv4) or 255/8 (IPv6) + src prvCpssDxChLpmRamMcGroupSearch
               set bucketPtr to be different than the default nexthop */
            retVal = cpssDxChLpmRamMcGroupGetNext(vrId, GT_TRUE, ipGroupPtr,
                                              ipGroupPrefixPtr,
                                              &bucketPtr,
                                              shadowPtr,protocolStack);
            if ((retVal != GT_OK) || (bucketPtr == NULL))
            {
                CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_FOUND, LOG_ERROR_NO_MSG);
            }
        }
        newGroup = GT_TRUE;

        if (protocolStack == PRV_CPSS_DXCH_LPM_PROTOCOL_IPV4_E)
        {
            cpssOsMemSet(ipSrcPtr,0,sizeof(GT_U8)*PRV_CPSS_DXCH_LPM_NUM_OF_OCTETS_IN_IPV4_PROTOCOL_CNS);
        }
        else
        {
            cpssOsMemSet(ipSrcPtr,0,sizeof(GT_U8)*PRV_CPSS_DXCH_LPM_NUM_OF_OCTETS_IN_IPV6_PROTOCOL_CNS);
        }

        (*ipSrcPrefixPtr)  = 0;
    }
    else
    {
        retVal = prvCpssDxChLpmRamMcGroupSearch(vrId,ipGroupPtr,*ipGroupPrefixPtr,
                                                &bucketPtr,shadowPtr,protocolStack);
        if(retVal != GT_OK)
        {
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_FOUND, LOG_ERROR_NO_MSG);
        }
    }

    /* Search for the IP-src part of the address.   */
    while(1)
    {
        /* return the entry associated with IP-src (0,0)   */
        if(newGroup == GT_TRUE)
        {
            prvCpssDxChLpmRamMngSearch(bucketPtr, ipSrcPtr, 0, (GT_PTR*)(GT_UINTPTR)&nextHopEntry);
            retVal = GT_OK;
        }

        /* get the default group for comparison  */
        if (prvCpssDxChLpmRamMcDefRouteGet(vrId,&defMcRoutePointer,
                                           shadowPtr,protocolStack) != GT_OK)
            /* can't happen - it means we have no default */
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_FAIL, LOG_ERROR_NO_MSG);

        /* incase we recived in the src (0,0) the default entry, it means this
           is not a valid G,* , which means we need to treat as looking for G,S*/
        if ((newGroup == GT_FALSE) ||
            ((nextHopEntry != NULL) &&
             (nextHopEntry == defMcRoutePointer)))
        {
            retVal = prvCpssDxChLpmRamMngEntryGet(bucketPtr, protocolStack, ipSrcPtr, ipSrcPrefixPtr,
                                                  (GT_PTR*)(GT_UINTPTR)&nextHopEntry);
        }

        /* No ip-src addresses left for this group, move to the next group. */
        if(retVal != GT_OK)
        {
            retVal =
                cpssDxChLpmRamMcGroupGetNext(vrId,GT_FALSE, ipGroupPtr,
                                             ipGroupPrefixPtr,
                                             &bucketPtr,
                                             shadowPtr,protocolStack);
            if ((retVal != GT_OK) || (bucketPtr == NULL))
            {
                CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_FOUND, LOG_ERROR_NO_MSG);
            }

            newGroup        = GT_TRUE;

            if (protocolStack == PRV_CPSS_DXCH_LPM_PROTOCOL_IPV4_E)
            {
                cpssOsMemSet(ipSrcPtr,0,sizeof(GT_U8)*PRV_CPSS_DXCH_LPM_NUM_OF_OCTETS_IN_IPV4_PROTOCOL_CNS);
            }
            else
            {
                cpssOsMemSet(ipSrcPtr,0,sizeof(GT_U8)*PRV_CPSS_DXCH_LPM_NUM_OF_OCTETS_IN_IPV6_PROTOCOL_CNS);
            }

            (*ipSrcPrefixPtr)  = 0;
            newGroup        = GT_TRUE;
        }
        else    /* A new entry was found.   */
        {
            /* if needed copy the mcRoute (NH) */
            if (mcRoutePointerPtr != NULL)
            {
                cpssOsMemCpy(mcRoutePointerPtr,nextHopEntry,
                             sizeof(PRV_CPSS_DXCH_LPM_ROUTE_ENTRY_POINTER_STC));
            }
            return GT_OK;
        }
    }
}

/*******************************************************************************
* prvCpssDxChLpmRamMcEntrySearch
*
* DESCRIPTION:
*       This function searches for the route entries associates with a given
*       (S,G), prefixes.
*
* INPUTS:
*       vrId                  - The virtual router Id.
*       ipGroupPtr            - The ip Group address to get the next entry for.
*       ipGroupPrefix         - ipGroupPtr prefix length.
*       ipSrcArr              - The ip Source address to get the next entry for.
*       ipSrcPrefix           - ipSrcArr prefix length.
*       protocolStack         - the protocol to work on.
*       shadowPtr             - the shadow relevant for the devices asked to act on.
*
* OUTPUTS:
*       ipNextHopEntryPtr     - A pointer to a next hop struct including the HW
*                               pointer
*
* RETURNS:
*       GT_OK if successful, or
*       GT_NOT_FOUND if the given prefix was not found.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       Calling this function with ipGroupPrefix = 0, will return the default
*       Mc route entries.
*
*******************************************************************************/
GT_STATUS prvCpssDxChLpmRamMcEntrySearch
(
    IN GT_U32                                       vrId,
    IN GT_U8                                        *ipGroupPtr,
    IN GT_U32                                       ipGroupPrefix,
    IN GT_U8                                        ipSrcArr[],
    IN GT_U32                                       ipSrcPrefix,
    OUT PRV_CPSS_DXCH_LPM_ROUTE_ENTRY_POINTER_STC   *ipNextHopEntryPtr,
    IN CPSS_IP_PROTOCOL_STACK_ENT                   protocolStack,
    IN PRV_CPSS_DXCH_LPM_RAM_SHADOW_STC             *shadowPtr
)
{
    PRV_CPSS_DXCH_LPM_RAM_BUCKET_SHADOW_STC *srcBucket;
    PRV_CPSS_DXCH_LPM_ROUTE_ENTRY_POINTER_STC *nextHopEntry;/* Next hop entry stored in the */
                                                          /* Mc-src lpm structure.        */
    /* first check the ip version was initialized */
    if (shadowPtr->isProtocolInitialized[protocolStack] == GT_FALSE)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_INITIALIZED, LOG_ERROR_NO_MSG);
    }

    /* verify that multicast is supported for this protocol in the VR */
    if (shadowPtr->vrRootBucketArray[vrId].isMulticastSupported[protocolStack] == GT_FALSE)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_SUPPORTED, LOG_ERROR_NO_MSG);
    }

    if (ipNextHopEntryPtr == NULL)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    if (((ipGroupPrefix == PRV_CPSS_DXCH_LPM_RAM_IPV4_MC_ADDRESS_SPACE_PREFIX_CNS) &&
         (protocolStack == CPSS_IP_PROTOCOL_IPV4_E)) ||
        ((ipGroupPrefix == PRV_CPSS_DXCH_LPM_RAM_IPV6_MC_ADDRESS_SPACE_PREFIX_CNS) &&
         (protocolStack == CPSS_IP_PROTOCOL_IPV6_E)))
    {
        if (GT_OK !=
            prvCpssDxChLpmRamMcDefRouteGet(vrId,&nextHopEntry,
                                           shadowPtr,protocolStack))
        {
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_FOUND, LOG_ERROR_NO_MSG);
        }
    }
    else
    {
        if (prvCpssDxChLpmRamMcGroupSearch(vrId,ipGroupPtr,ipGroupPrefix,
                                           &srcBucket,shadowPtr,protocolStack)
            != GT_OK)
        {
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_FOUND, LOG_ERROR_NO_MSG);
        }

        if (prvCpssDxChLpmRamMngSearch(srcBucket,ipSrcArr,ipSrcPrefix,(GT_PTR*)(GT_UINTPTR)&nextHopEntry) != GT_OK)
        {
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_FOUND, LOG_ERROR_NO_MSG);
        }
    }

    cpssOsMemCpy(ipNextHopEntryPtr,nextHopEntry,
                 sizeof(PRV_CPSS_DXCH_LPM_ROUTE_ENTRY_POINTER_STC));
    return GT_OK;
}

/*******************************************************************************
* prvCpssDxChLpmRamMcDefRouteGet
*
* DESCRIPTION:
*       retrieves the default MC route for the given VR router.
*
* INPUTS:
*       vrId                    - The vrId to which this tree belongs to.
*       shadowPtr               - the shadow relevant for the devices asked to act on.
*       protocol                - the protocol
*
* OUTPUTS:
*       defMcRoutePointerPtr    - A pointer to the default mc route
*
* RETURNS:
*       GT_OK                 - on success.
*       GT_FAIL               - on error.
*
* COMMENTS:
*       None.
*******************************************************************************/
GT_STATUS prvCpssDxChLpmRamMcDefRouteGet
(
    IN  GT_U32                                    vrId,
    OUT PRV_CPSS_DXCH_LPM_ROUTE_ENTRY_POINTER_STC **defMcRoutePointerPtr,
    IN  PRV_CPSS_DXCH_LPM_RAM_SHADOW_STC          *shadowPtr,
    IN  PRV_CPSS_DXCH_LPM_PROTOCOL_STACK_ENT      protocol
)
{
    *defMcRoutePointerPtr =
        shadowPtr->vrRootBucketArray[vrId].multicastDefault[protocol];

    return GT_OK;
}

/*******************************************************************************
* prvCpssDxChLpmRamMcTraverse
*
* DESCRIPTION:
*       a tree traverse function to set/retrive/count all the memory allocations
*       in this mc , and it's lpm src tree. it is done in DFS fashion.
*
* INPUTS:
*       traverseOp            - the traverse operation needed.
*       memAllocArrayPtr      - the array that holds the information of memory
*                               allocations.
*       memAllocArrayIndexPtr - the index in the array this function should start
*                               using.
*       memAllocArraySize     - the size of the above array.
*       vrId                  - The vrId to which this table belongs to.
*       shadowPtr             - the shadow relevant for the devices asked to act on.
*       protocol              - the protocol
*
* OUTPUTS:
*       stopPointIterPtr      - an iterator that is returned to point where we stopped
*                               in the lpm tree, so we can continue from there in the
*                               next call.
*
* RETURNS:
*       GT_OK                 - If there is enough memory for the insertion.
*       GT_OUT_OF_PP_MEM      - otherwise.
*
* COMMENTS:
*       if *stopPointIter ==0 we start at the begining.
*       and if the returned *stopPointIter==0 the we finished with this tree.
*
*******************************************************************************/
GT_STATUS prvCpssDxChLpmRamMcTraverse
(
    IN PRV_CPSS_DXCH_LPM_RAM_MEM_TRAVERSE_OP_ENT    traverseOp,
    IN PRV_CPSS_DXCH_LPM_RAM_MEM_ALLOC_INFO_STC     *memAllocArrayPtr,
    IN GT_U32                                       *memAllocArrayIndexPtr,
    IN GT_U32                                       memAllocArraySize,
    IN GT_U32                                       vrId,
    IN PRV_CPSS_DXCH_LPM_RAM_SHADOW_STC             *shadowPtr,
    OUT GT_UINTPTR                                  *stopPointIterPtr,
    IN PRV_CPSS_DXCH_LPM_PROTOCOL_STACK_ENT         protocol
)
{
    GT_STATUS retVal;
    PRV_CPSS_DXCH_LPM_RAM_RANGE_SHADOW_STC tmpRange;

    tmpRange.lowerLpmPtr.nextBucket =
        shadowPtr->vrRootBucketArray[vrId].rootBucket[protocol];
    tmpRange.pointerType =
        shadowPtr->vrRootBucketArray[vrId].rootBucketType[protocol];
    tmpRange.next = NULL;
    retVal = prvCpssDxChLpmRamMngMemTraverse(traverseOp,memAllocArrayPtr,
                                             memAllocArrayIndexPtr,
                                             memAllocArraySize,&tmpRange,GT_FALSE,
                                             shadowPtr->mcSearchMemArrayPtr[protocol],
                                             stopPointIterPtr);
    return retVal;
}

