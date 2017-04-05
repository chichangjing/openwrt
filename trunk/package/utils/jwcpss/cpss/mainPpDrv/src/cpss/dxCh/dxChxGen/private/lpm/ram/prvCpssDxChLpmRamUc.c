/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* prvCpssDxChLpmRamUc.c
*
* DESCRIPTION:
*       This file includes functions declarations for controlling the LPM UC
*       tables and structures, and structures definitions for shadow
*       management.
*
* DEPENDENCIES:
*       None.
*
* FILE REVISION NUMBER:
*       $Revision: 8 $
*
*******************************************************************************/

#include <cpss/extServices/os/gtOs/gtGenTypes.h>
#include <cpss/dxCh/dxChxGen/private/lpm/ram/prvCpssDxChLpmRamTypes.h>
#include <cpss/dxCh/dxChxGen/private/lpm/ram/prvCpssDxChLpmRam.h>
#include <cpss/dxCh/dxChxGen/private/lpm/ram/prvCpssDxChLpmRamMng.h>
#include <cpss/extServices/private/prvCpssBindFunc.h>
#include <cpss/dxCh/dxChxGen/private/lpm/ram/prvCpssDxChLpmRamUc.h>
#include <cpssCommon/private/prvCpssDevMemManager.h>
#include <cpss/dxCh/dxChxGen/private/lpm/ram/prvCpssDxChLpmRamMc.h>
#include <cpssCommon/private/prvCpssSkipList.h>
#include <cpss/dxCh/dxChxGen/config/private/prvCpssDxChInfo.h>

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

/*******************************************************************************
* prvCpssDxChLpmRamUcEntryAdd
*
* DESCRIPTION:
*       creates a new or override an exist route entry in the routing
*       structures.
*
* INPUTS:
*       vrId              - The virtual private network identifier.
*       destPtr           - The destination address of this route.
*       prefix            - The number of bits that are actual valid in,
*                           the destPtr.
*       nextHopPointerPtr - The next hop pointer to be associated with the given
*                           (address,prefix).
*       insertMode        - how to insert the prefix
*                           (see PRV_CPSS_DXCH_LPM_RAM_TRIE_INSERT_MODE_ENT)
*       override          - whether this is an override or a new insert.
*       protocolStack     - type of protocol stack to work on.
*       shadowPtr         - the shadow relevant for the devices asked to act on.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success, or
*       GT_BAD_PARAM             - If the vrId was not created yet, or
*       GT_NOT_FOUND             - If the given nextHopId is not valid, or
*       GT_ALREADY_EXIST         - If overide is false and entry already exist, or
*       GT_OUT_OF_CPU_MEM        - If failed to allocate CPU memory, or
*       GT_OUT_OF_PP_MEM         - If failed to allocate PP memory, or
*       GT_FAIL                  - otherwise.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS prvCpssDxChLpmRamUcEntryAdd
(
    IN GT_U32                                       vrId,
    IN GT_U8                                        *destPtr,
    IN GT_U32                                       prefix,
    IN PRV_CPSS_DXCH_LPM_ROUTE_ENTRY_POINTER_STC    *nextHopPointerPtr,
    IN PRV_CPSS_DXCH_LPM_RAM_TRIE_INSERT_MODE_ENT   insertMode,
    IN GT_BOOL                                      override,
    IN PRV_CPSS_DXCH_LPM_PROTOCOL_STACK_ENT         protocolStack,
    IN PRV_CPSS_DXCH_LPM_RAM_SHADOW_STC             *shadowPtr
)
{
    PRV_CPSS_DXCH_LPM_RAM_BUCKET_SHADOW_STC *pBucket;/* the 1st level bucket   */
    CPSS_DXCH_LPM_NEXT_POINTER_TYPE_ENT rootBucketType; /* the 1st level bucket
                                                          type  */
    CPSS_DXCH_LPM_NEXT_POINTER_TYPE_ENT nextBucketType; /* next level bucket
                                                          type  */
    GT_BOOL alreadyExists = GT_FALSE; /* Indicates if the given prefix      */
                                      /* already exists in the LPM structure. */
    GT_PTR  dummy;

    PRV_CPSS_DXCH_LPM_RAM_MEM_INFO_STC     **lpmEngineMemPtr;

    PRV_CPSS_DXCH_LPM_ROUTE_ENTRY_POINTER_STC *allocNextHopPointerPtr=NULL;
    PRV_CPSS_DXCH_LPM_RAM_MEM_LIST_ENTRY_STC  **freeMemList;


    GT_STATUS retVal = GT_OK;
    GT_STATUS retVal2 = GT_OK;
    GT_U32 i;
    GT_U32 maxMemSize;
    GT_U32 shareDevListLen;
    GT_U8  *shareDevsList;  /* List of devices sharing this LPM structure   */

    PRV_CPSS_DXCH_LPM_RAM_OCTETS_TO_BLOCK_MAPPING_STC   tempLpmRamOctetsToBlockMapping[PRV_CPSS_DXCH_LPM_RAM_NUM_OF_MEMORIES_CNS];/* used for reconstruct in case of error */

   /* first check the protocol was initialized */
    if (shadowPtr->isProtocolInitialized[protocolStack] == GT_FALSE)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_INITIALIZED, LOG_ERROR_NO_MSG);
    }

    /* Check if the given virtual router already exists */
    if((vrId >= shadowPtr->vrfTblSize) ||
       (shadowPtr->vrRootBucketArray[vrId].valid == GT_FALSE) ||
       (shadowPtr->vrRootBucketArray[vrId].rootBucket[protocolStack] == NULL))
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    /* verify that unicast is supported for this protocol in the VR */
    if (shadowPtr->vrRootBucketArray[vrId].isUnicastSupported[protocolStack] == GT_FALSE)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_SUPPORTED, LOG_ERROR_NO_MSG);
    }

     /* keep values in case recostruct is needed */
    cpssOsMemCpy(tempLpmRamOctetsToBlockMapping,shadowPtr->lpmRamOctetsToBlockMappingPtr,
                 sizeof(PRV_CPSS_DXCH_LPM_RAM_OCTETS_TO_BLOCK_MAPPING_STC)*shadowPtr->numOfLpmMemories);                              

    shareDevsList   = shadowPtr->workDevListPtr->shareDevs;
    shareDevListLen = shadowPtr->workDevListPtr->shareDevNum;

    for (i = 0; i < shareDevListLen; i++)
    {
        if (nextHopPointerPtr->routeEntryMethod == PRV_CPSS_DXCH_LPM_ENTRY_TYPE_REGULAR_E)
        {
            maxMemSize = PRV_CPSS_DXCH_PP_MAC(shareDevsList[i])->fineTuning.tableSize.routerNextHop;
        }
        else /* PRV_CPSS_DXCH_LPM_ENTRY_TYPE_ECMP_E or PRV_CPSS_DXCH_LPM_ENTRY_TYPE_QOS_E */
        {
            maxMemSize = PRV_CPSS_DXCH_PP_MAC(shareDevsList[i])->fineTuning.tableSize.ecmpQos;
        }
        if (nextHopPointerPtr->routeEntryBaseMemAddr >= maxMemSize)
        {
           CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
        }
    }

    /* if we're in bulk insertion we're allowed to free (reuse) only new
       memory which is not update in the HW */
    freeMemList = (insertMode == PRV_CPSS_DXCH_LPM_RAM_TRIE_INSERT_SDW_MEM_MODE_E)?
                  &shadowPtr->freeMemListDuringUpdate:
                  &shadowPtr->freeMemListEndOfUpdate;

    retVal = prvCpssDxChLpmRamMemFreeListMng(0,PRV_CPSS_DXCH_LPM_RAM_MEM_LIST_RESET_OP_E,
                                             freeMemList);
    if (retVal != GT_OK)
        return (retVal);

    pBucket = shadowPtr->vrRootBucketArray[vrId].rootBucket[protocolStack];
    rootBucketType = shadowPtr->vrRootBucketArray[vrId].rootBucketType[protocolStack];

    /* Check if an entry with the same (address,prefix) already exists,  */
    /* if yes, check the override param.                                 */
    dummy = (GT_PTR)allocNextHopPointerPtr;
    retVal = prvCpssDxChLpmRamMngSearch(pBucket,destPtr,prefix, &dummy);
    allocNextHopPointerPtr = (PRV_CPSS_DXCH_LPM_ROUTE_ENTRY_POINTER_STC *)dummy;
    if(retVal == GT_OK)
    {
        if(override == GT_FALSE)
        {
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_ALREADY_EXIST, LOG_ERROR_NO_MSG);
        }

        alreadyExists = GT_TRUE;
    }
    else
    {
        /* new entry allocate a new nexthop pointer */
        allocNextHopPointerPtr =
            cpssOsLpmMalloc(sizeof(PRV_CPSS_DXCH_LPM_ROUTE_ENTRY_POINTER_STC));

        if (allocNextHopPointerPtr == NULL)
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_OUT_OF_CPU_MEM, LOG_ERROR_NO_MSG);
    }

    /* copy the next hop pointer fields */
    cpssOsMemCpy(allocNextHopPointerPtr,nextHopPointerPtr,
                 sizeof(PRV_CPSS_DXCH_LPM_ROUTE_ENTRY_POINTER_STC));

    lpmEngineMemPtr = shadowPtr->ucSearchMemArrayPtr[protocolStack];
    shadowPtr->neededMemoryListLen = 0;
    shadowPtr->neededMemoryCurIdx = 0;    

    /* if we need to update the shadow only there is no need to check memory
       space */
    if (insertMode != PRV_CPSS_DXCH_LPM_RAM_TRIE_INSERT_SDW_ONLY_MODE_E)
    {
        /* Check memory availability.       */
        retVal = prvCpssDxChLpmRamMngAllocAvailableMemCheck(pBucket,destPtr,prefix,
                                                            PRV_CPSS_DXCH_LPM_RAM_ROOT_BUCKET_WITHOUT_COMPRESSED_2_E,
                                                            lpmEngineMemPtr,
                                                            shadowPtr,
                                                            protocolStack);
        if (retVal != GT_OK)
        {
            if (alreadyExists == GT_FALSE)
                cpssOsLpmFree(allocNextHopPointerPtr);
            return (retVal);
        }
    }


    PRV_CPSS_DXCH_LPM_NEXT_PTR_TYPE_CONVERT_MAC(allocNextHopPointerPtr->routeEntryMethod,nextBucketType);
    /* Insert the complete address to the lpm structure */
    retVal = prvCpssDxChLpmRamMngInsert(pBucket, destPtr, prefix,
                                        allocNextHopPointerPtr,
                                        nextBucketType,
                                        PRV_CPSS_DXCH_LPM_RAM_ROOT_BUCKET_WITHOUT_COMPRESSED_2_E,
                                        lpmEngineMemPtr,&alreadyExists,
                                        &rootBucketType, shadowPtr, insertMode,
                                        GT_FALSE);
    if (retVal != GT_OK)
    {
        shadowPtr->neededMemoryListLen = 0;        
        prvCpssDxChLpmRamMemFreeListMng(0,PRV_CPSS_DXCH_LPM_RAM_MEM_LIST_FREE_MEM_OP_E,
                                        freeMemList);
        /* insert was not successful. Activate rollback and free resources */
        prvCpssDxChLpmRamUcEntryDel(vrId,
                                    destPtr,
                                    prefix,
                                    GT_TRUE,
                                    protocolStack,
                                    shadowPtr,
                                    GT_TRUE);
        if (alreadyExists == GT_FALSE)
            cpssOsLpmFree(allocNextHopPointerPtr);

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

        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_FAIL, LOG_ERROR_NO_MSG);
    }
    if (retVal == GT_OK)
    {
        /* Now update the UC first table according      */
        /* to the first level of the lpm structure.     */
        shadowPtr->vrRootBucketArray[vrId].rootBucketType[protocolStack] =
            rootBucketType;

        /* don't touch the hw in a hot sync process , it's done in the end */
        if (insertMode == PRV_CPSS_DXCH_LPM_RAM_TRIE_INSERT_SDW_MEM_HW_MODE_E)
        {
            retVal = prvCpssDxChLpmRamMngVrfEntryUpdate(vrId, protocolStack, shadowPtr);
            if (retVal != GT_OK)
            {
                shadowPtr->neededMemoryListLen = 0;
                prvCpssDxChLpmRamMemFreeListMng(0,PRV_CPSS_DXCH_LPM_RAM_MEM_LIST_FREE_MEM_OP_E,
                                                freeMemList);
                /* insert was not successful. Activate rollback and free resources */
                prvCpssDxChLpmRamUcEntryDel(vrId,
                                            destPtr,
                                            prefix,
                                            GT_TRUE,
                                            protocolStack,
                                            shadowPtr,
                                            GT_TRUE);
                if (alreadyExists == GT_FALSE)
                    cpssOsLpmFree(allocNextHopPointerPtr);

                /* free the allocated/bound RAM memory */
                retVal2 = prvCpssDxChLpmRamMngAllocatedAndBoundMemFree(shadowPtr,protocolStack);
                if (retVal2!=GT_OK)
                {
                    return retVal2;
                }
                /* in case of fail we will need to recondtruct to the status we had before the call to prvCpssDxChLpmRamMngAllocAvailableMemCheck */
                cpssOsMemCpy(shadowPtr->lpmRamOctetsToBlockMappingPtr,tempLpmRamOctetsToBlockMapping,
                             sizeof(PRV_CPSS_DXCH_LPM_RAM_OCTETS_TO_BLOCK_MAPPING_STC)*shadowPtr->numOfLpmMemories);            

                return retVal;
            }
        }
        else
        {
            /* indicate the VR HW wasn't updated and needs update */
            shadowPtr->vrRootBucketArray[vrId].needsHwUpdate = GT_TRUE;
        }

        /* check that all the pre-allocated memory was used */
        if (shadowPtr->neededMemoryCurIdx != shadowPtr->neededMemoryListLen)
        {
            /* should never! - meaning we didn't use all preallcoated memory*/
            prvCpssDxChLpmRamMemFreeListMng(0,PRV_CPSS_DXCH_LPM_RAM_MEM_LIST_FREE_MEM_OP_E,
                                            freeMemList);

            if (alreadyExists == GT_FALSE)
                cpssOsLpmFree(allocNextHopPointerPtr);
            shadowPtr->neededMemoryListLen = 0;  
                  
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
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_FAIL, LOG_ERROR_NO_MSG);
        }
    }
    shadowPtr->neededMemoryListLen = 0;   
    
    if ((retVal != GT_OK) && (alreadyExists == GT_FALSE))
        cpssOsLpmFree(allocNextHopPointerPtr);
    retVal2 = prvCpssDxChLpmRamMemFreeListMng(0,PRV_CPSS_DXCH_LPM_RAM_MEM_LIST_FREE_MEM_OP_E,
                                              freeMemList);

    if (retVal == GT_OK)
    {
        if (retVal2==GT_OK)
        {
            /* the data was written successfully to HW, we can reset the information regarding the new memoryPool allocations done.
               next call to ADD will set this array with new values of allocated/bound blocks */
            cpssOsMemSet(shadowPtr->allNewNextMemInfoAllocatedPerOctetArrayPtr, 0, sizeof(shadowPtr->allNewNextMemInfoAllocatedPerOctetArrayPtr)); 
        }        
        return retVal2;
    }

    return retVal;
}

/*******************************************************************************
* prvCpssDxChLpmRamUcEntryDel
*
* DESCRIPTION:
*       Delete address/prefix unicast address from a practicular virtual router
*
* INPUTS:
*       vrId              - The virtual private network identifier.
*       destPtr           - The destination address of this route.
*       prefix            - The number of bits that are actual valid in destPtr
*       updateHwAndMem    - whether an HW update and memory alloc should take place
*       protocolStack     - type of protocol to work on.
*       shadowPtr         - the shadow relevant for the devices asked to act on.
*       roolBack          - GT_TRUE: rollback is taking place.
*                           GT_FALSE: otherwise.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success, or
*       GT_BAD_PARAM             - if the required vrId is not valid, or
*       GT_FAIL                  - on error.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS prvCpssDxChLpmRamUcEntryDel
(
    IN GT_U32                               vrId,
    IN GT_U8                                *destPtr,
    IN GT_U32                               prefix,
    IN GT_BOOL                              updateHwAndMem,
    IN PRV_CPSS_DXCH_LPM_PROTOCOL_STACK_ENT protocolStack,
    IN PRV_CPSS_DXCH_LPM_RAM_SHADOW_STC     *shadowPtr,
    IN GT_BOOL                              roolBack
)
{
    PRV_CPSS_DXCH_LPM_RAM_POINTER_SHADOW_UNT lpmPtr;/*the LPM structure to      */
                                                    /* delete from.             */
    PRV_CPSS_DXCH_LPM_ROUTE_ENTRY_POINTER_STC *delEntry = NULL;/* Next pointer struct.   */
    GT_PTR dummy;

    PRV_CPSS_DXCH_LPM_RAM_BUCKET_PARENT_WRITE_FUNC_INFO_STC parentWriteFuncInfo;

    PRV_CPSS_DXCH_LPM_RAM_UPDATE_VRF_TABLE_FUNC_PARAMS_STC writeFuncData;

    GT_STATUS retVal = GT_OK;
    GT_STATUS retVal2 = GT_OK;

    CPSS_DXCH_LPM_NEXT_POINTER_TYPE_ENT rootBucketType; /* The bucket type of the 1st level LPM */
                                    /* bucket.                              */
    PRV_CPSS_DXCH_LPM_RAM_MEM_INFO_STC     **lpmEngineMemPtr;

    /* first check that the protocol was initialized */
    if (shadowPtr->isProtocolInitialized[protocolStack] == GT_FALSE)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_INITIALIZED, LOG_ERROR_NO_MSG);
    }

    /* can't delete the default route. only overwrite it */
    if(prefix == 0)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    /* Check if the given virtual router already exists */
    if((vrId >= shadowPtr->vrfTblSize) ||
       (shadowPtr->vrRootBucketArray[vrId].valid == GT_FALSE)||
       (shadowPtr->vrRootBucketArray[vrId].rootBucket[protocolStack] == NULL))
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);

    /* verify that unicast is supported for this protocol in the VR */
    if (shadowPtr->vrRootBucketArray[vrId].isUnicastSupported[protocolStack] == GT_FALSE)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_SUPPORTED, LOG_ERROR_NO_MSG);
    }

    /* update the info for the parent write function */
    parentWriteFuncInfo.bucketParentWriteFunc =
        prvCpssDxChLpmRamUpdateVrTableFuncWrapper;

    writeFuncData.shadowPtr = shadowPtr;
    writeFuncData.protocol = protocolStack;
    writeFuncData.vrId = vrId;
    parentWriteFuncInfo.data = (GT_PTR)(&writeFuncData);

    retVal = prvCpssDxChLpmRamMemFreeListMng(0,PRV_CPSS_DXCH_LPM_RAM_MEM_LIST_RESET_OP_E,
                                             &shadowPtr->freeMemListEndOfUpdate);
    if (retVal != GT_OK)
        return (retVal);

    lpmPtr.nextBucket = shadowPtr->vrRootBucketArray[vrId].rootBucket[protocolStack];

    lpmEngineMemPtr = shadowPtr->ucSearchMemArrayPtr[protocolStack];

    dummy = (GT_PTR)delEntry;
    rootBucketType = shadowPtr->vrRootBucketArray[vrId].rootBucketType[protocolStack];
    retVal = prvCpssDxChLpmRamMngEntryDelete(&lpmPtr,destPtr,prefix,
                                             PRV_CPSS_DXCH_LPM_RAM_ROOT_BUCKET_WITHOUT_COMPRESSED_2_E,
                                             updateHwAndMem,lpmEngineMemPtr,
                                             &rootBucketType,&dummy,
                                             shadowPtr,&parentWriteFuncInfo,
                                             GT_FALSE,roolBack);
    delEntry = (PRV_CPSS_DXCH_LPM_ROUTE_ENTRY_POINTER_STC *)dummy;

    if (retVal == GT_OK)
    {
        /* ok first free the next hop pointer */
        if (roolBack == GT_FALSE)
        {
            cpssOsLpmFree(delEntry);
        }

        /* update the root bucket type */
        shadowPtr->vrRootBucketArray[vrId].rootBucketType[protocolStack] =
            rootBucketType;

        if (updateHwAndMem == GT_TRUE)
        {
            /* Now update the uc first table according   */
            /* to the first level of the lpm structure.     */
            retVal = prvCpssDxChLpmRamMngVrfEntryUpdate(vrId, protocolStack, shadowPtr);
        }
        else
        {
            /* indicate the VR HW wasn't updated and needs update */
            shadowPtr->vrRootBucketArray[vrId].needsHwUpdate = GT_TRUE;
        }
    }

    retVal2 = prvCpssDxChLpmRamMemFreeListMng(0,PRV_CPSS_DXCH_LPM_RAM_MEM_LIST_FREE_MEM_OP_E,
                                              &shadowPtr->freeMemListEndOfUpdate);

    if(retVal == GT_OK)
        return retVal2;

    return retVal;
}

/*******************************************************************************
* prvCpssDxChLpmRamUcEntriesFlush
*
* DESCRIPTION:
*       flushes the unicast Routing table and stays with the default entry
*       only.
*
* INPUTS:
*       vrId           - The virtual private network identifier.
*       protocolStack  - the protocol to work on.
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
GT_STATUS prvCpssDxChLpmRamUcEntriesFlush
(
    IN GT_U32                               vrId,
    IN PRV_CPSS_DXCH_LPM_PROTOCOL_STACK_ENT protocolStack,
    IN PRV_CPSS_DXCH_LPM_RAM_SHADOW_STC     *shadowPtr
)
{
    GT_STATUS retVal;
    GT_U8 addr[PRV_CPSS_DXCH_LPM_MAX_NUM_OF_OCTETS_FOR_ADDRESS_CNS];   /* The next address & prefix to be */
    GT_U32 prefix = 0;                    /* deleted.                      */
    GT_U8 del[PRV_CPSS_DXCH_LPM_MAX_NUM_OF_OCTETS_FOR_ADDRESS_CNS];   /* addr in GT_U8[] format.   */
    GT_U32 i;

    /* first check that the protocol was initialized */
    if (shadowPtr->isProtocolInitialized[protocolStack] == GT_FALSE)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_INITIALIZED, LOG_ERROR_NO_MSG);
    }

    cpssOsMemSet(addr, 0, sizeof(GT_U8) * PRV_CPSS_DXCH_LPM_MAX_NUM_OF_OCTETS_FOR_ADDRESS_CNS);

    i = 0;
    while(prvCpssDxChLpmRamUcEntryGet(vrId,addr,&prefix,NULL,
                                      protocolStack,shadowPtr) == GT_OK)
    {
        i++;

        cpssOsMemCpy(del, addr, sizeof(GT_U8) * PRV_CPSS_DXCH_LPM_MAX_NUM_OF_OCTETS_FOR_ADDRESS_CNS);

        retVal = prvCpssDxChLpmRamUcEntryDel(vrId,del,prefix,GT_TRUE,
                                             protocolStack,shadowPtr,
                                             GT_FALSE);
        if (retVal != GT_OK)
        {
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_FAIL, LOG_ERROR_NO_MSG);
        }

        cpssOsMemSet(addr, 0, sizeof(GT_U8)* PRV_CPSS_DXCH_LPM_MAX_NUM_OF_OCTETS_FOR_ADDRESS_CNS);

        prefix = 0;
    }

    return GT_OK;
}

/*******************************************************************************
* prvCpssDxChLpmRamUcEntrySearch
*
* DESCRIPTION:
*       This function searches for a given uc address, and returns the next
*       hop pointer associated with it.
*
* INPUTS:
*       vrId              - The virtual router Id to get the entry from.
*       addrPtr           - The address to search for.
*       prefix            - Prefix length of the address.
*       protocolStack     - protocol stack to work on.
*       shadowPtr         - the shadow relevant for the devices asked to act on.
*
* OUTPUTS:
*       nextHopPointerPtr - The next hop pointer to which the given prefix is
*                           bound.
*
* RETURNS:
*       GT_OK                    - if the required entry was found, or
*       GT_NOT_FOUND             - if the given prefix was not found.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS prvCpssDxChLpmRamUcEntrySearch
(
    IN  GT_U32                                      vrId,
    IN  GT_U8                                       *addrPtr,
    IN  GT_U32                                      prefix,
    OUT PRV_CPSS_DXCH_LPM_ROUTE_ENTRY_POINTER_STC   *nextHopPointerPtr,
    IN  PRV_CPSS_DXCH_LPM_PROTOCOL_STACK_ENT        protocolStack,
    IN  PRV_CPSS_DXCH_LPM_RAM_SHADOW_STC            *shadowPtr
)
{
    GT_STATUS retVal;
    GT_PTR dummy;
    PRV_CPSS_DXCH_LPM_RAM_BUCKET_SHADOW_STC *bucketPtr;

    PRV_CPSS_DXCH_LPM_ROUTE_ENTRY_POINTER_STC *nextHopEntry = NULL;

    if ((vrId >= shadowPtr->vrfTblSize) ||
        (shadowPtr->vrRootBucketArray[vrId].valid == GT_FALSE) ||
        (shadowPtr->vrRootBucketArray[vrId].rootBucket[protocolStack] == NULL))
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_INITIALIZED, LOG_ERROR_NO_MSG);
    }

    /* verify that unicast is supported for this protocol in the VR */
    if (shadowPtr->vrRootBucketArray[vrId].isUnicastSupported[protocolStack] == GT_FALSE)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_SUPPORTED, LOG_ERROR_NO_MSG);
    }

    bucketPtr = shadowPtr->vrRootBucketArray[vrId].rootBucket[protocolStack];

    if (nextHopPointerPtr == NULL)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    dummy = (GT_PTR*)nextHopEntry;
    retVal = prvCpssDxChLpmRamMngSearch(bucketPtr,addrPtr,prefix,&dummy);
    nextHopEntry =(PRV_CPSS_DXCH_LPM_ROUTE_ENTRY_POINTER_STC *)dummy;
    if(retVal != GT_OK)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_FOUND, LOG_ERROR_NO_MSG);
    }

    cpssOsMemCpy(nextHopPointerPtr,nextHopEntry,
                 sizeof(PRV_CPSS_DXCH_LPM_ROUTE_ENTRY_POINTER_STC));

    return GT_OK;
}

/*******************************************************************************
* prvCpssDxChLpmRamUcEntryLpmSearch
*
* DESCRIPTION:
*       This function searches for a given uc address, and returns the next
*       hop pointer associated with it. This is a LPM search - the exact prefix
*       doesn't have to exist in DB.
*
* INPUTS:
*       vrId            - The virtual router Id to get the entry from.
*       addrPtr         - The address to search for.
*       protocolStack   - type of protocol stack to work on.
*       shadowPtr       - the shadow relevant for the devices asked to act on.
*
* OUTPUTS:
*       prefixLenPtr        - Points to the number of bits that are actual valid
*                             in the longest match
*       nextHopPointerPtr   - The next hop pointer to which the given prefix is
*                             bound.
*
* RETURNS:
*       GT_OK                    - if the required entry was found, or
*       GT_NOT_FOUND             - if the given prefix was not found.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS prvCpssDxChLpmRamUcEntryLpmSearch
(
    IN  GT_U32                                      vrId,
    IN  GT_U8                                       *addrPtr,
    OUT GT_U32                                      *prefixLenPtr,
    OUT PRV_CPSS_DXCH_LPM_ROUTE_ENTRY_POINTER_STC   *nextHopPointerPtr,
    IN  PRV_CPSS_DXCH_LPM_PROTOCOL_STACK_ENT        protocolStack,
    IN  PRV_CPSS_DXCH_LPM_RAM_SHADOW_STC            *shadowPtr
)
{
    GT_STATUS retVal;
    GT_PTR dummy;
    PRV_CPSS_DXCH_LPM_RAM_BUCKET_SHADOW_STC *bucketPtr;

    PRV_CPSS_DXCH_LPM_ROUTE_ENTRY_POINTER_STC *nextHopEntry = NULL;

    if ((vrId >= shadowPtr->vrfTblSize) ||
        (shadowPtr->vrRootBucketArray[vrId].valid == GT_FALSE) ||
        (shadowPtr->vrRootBucketArray[vrId].rootBucket[protocolStack] == NULL))
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_INITIALIZED, LOG_ERROR_NO_MSG);
    }

    /* verify that unicast is supported for this protocol in the VR */
    if (shadowPtr->vrRootBucketArray[vrId].isUnicastSupported[protocolStack] == GT_FALSE)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_SUPPORTED, LOG_ERROR_NO_MSG);
    }

    bucketPtr = shadowPtr->vrRootBucketArray[vrId].rootBucket[protocolStack];

    if (nextHopPointerPtr == NULL)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    dummy = (GT_PTR)nextHopEntry;
    retVal = prvCpssDxChLpmRamMngDoLpmSearch(bucketPtr,
                                             addrPtr,
                                             protocolStack,
                                             prefixLenPtr,
                                             &dummy);
    nextHopEntry = (PRV_CPSS_DXCH_LPM_ROUTE_ENTRY_POINTER_STC *)dummy;
    if(retVal != GT_OK)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_FOUND, LOG_ERROR_NO_MSG);
    }

    cpssOsMemCpy(nextHopPointerPtr,nextHopEntry,sizeof(PRV_CPSS_DXCH_LPM_ROUTE_ENTRY_POINTER_STC));

    return GT_OK;
}

/*******************************************************************************
* prvCpssDxChLpmRamUcEntryGet
*
* DESCRIPTION:
*       This function returns an Unicast entry with larger (address,prefix) than
*       the given one.
*
* INPUTS:
*       vrId              - The virtual router Id to get the entry from.
*       addrPtr           - The address to start the search from.
*       prefixPtr         - Prefix length of addrPtr.
*       protocolStack     - type of protocol stack to work on.
*       shadowPtr         - the shadow relevant for the devices asked to act on.
*
* OUTPUTS:
*       addrPtr                 - The address of the found entry.
*       prefixPtr               - The prefix length of the found entry.
*       nextHopPointerPtrPtr    - A pointer to the next hop pointer
*
* RETURNS:
*       GT_OK                    - if the required entry was found, or
*       GT_NOT_FOUND             - if no more entries are left in the LPM shadow
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS prvCpssDxChLpmRamUcEntryGet
(
    IN    GT_U32                                    vrId,
    INOUT GT_U8                                     *addrPtr,
    INOUT GT_U32                                    *prefixPtr,
    OUT   PRV_CPSS_DXCH_LPM_ROUTE_ENTRY_POINTER_STC **nextHopPointerPtrPtr,
    IN    PRV_CPSS_DXCH_LPM_PROTOCOL_STACK_ENT      protocolStack,
    IN    PRV_CPSS_DXCH_LPM_RAM_SHADOW_STC          *shadowPtr
)
{
    GT_STATUS retVal;
    GT_PTR dummy;
    PRV_CPSS_DXCH_LPM_RAM_BUCKET_SHADOW_STC *bucketPtr;
    PRV_CPSS_DXCH_LPM_ROUTE_ENTRY_POINTER_STC *tmpNextHopPointerPtr=NULL;

    if (shadowPtr->isProtocolInitialized[protocolStack] == GT_FALSE)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_INITIALIZED, LOG_ERROR_NO_MSG);
    }

    bucketPtr = shadowPtr->vrRootBucketArray[vrId].rootBucket[protocolStack];

    dummy = (GT_PTR)tmpNextHopPointerPtr;
    retVal = prvCpssDxChLpmRamMngEntryGet(bucketPtr,protocolStack,addrPtr,prefixPtr,(GT_PTR*)&dummy);
    if (retVal != GT_OK)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_FOUND, LOG_ERROR_NO_MSG);
    }
    /* Since UC and MC share the same tree, check that the entry is not MC */
    if ((protocolStack == PRV_CPSS_DXCH_LPM_PROTOCOL_IPV4_E) &&
        (*addrPtr >= PRV_CPSS_DXCH_LPM_RAM_START_OF_IPV4_MC_ADDRESS_SPACE_CNS) &&
        (*addrPtr <= PRV_CPSS_DXCH_LPM_RAM_END_OF_IPV4_MC_ADDRESS_SPACE_CNS))
    {
        /* Skip the mulicast range - jump to the reserved range */
        *addrPtr = PRV_CPSS_DXCH_LPM_RAM_START_OF_IPV4_RESERVED_SPACE_ADDRESS_SPACE_CNS;
        *prefixPtr = PRV_CPSS_DXCH_LPM_RAM_IPV4_RESERVED_ADDRESS_SPACE_PREFIX_CNS;
        return prvCpssDxChLpmRamUcEntryGet(vrId, addrPtr, prefixPtr,
                                           nextHopPointerPtrPtr, protocolStack,
                                           shadowPtr);
    }
    if ((protocolStack == PRV_CPSS_DXCH_LPM_PROTOCOL_IPV6_E) &&
        (*addrPtr >= PRV_CPSS_DXCH_LPM_RAM_START_OF_IPV6_MC_ADDRESS_SPACE_CNS))
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_FOUND, LOG_ERROR_NO_MSG);
    }

    tmpNextHopPointerPtr = (PRV_CPSS_DXCH_LPM_ROUTE_ENTRY_POINTER_STC *)dummy;
    if (nextHopPointerPtrPtr != NULL)
    {
        *nextHopPointerPtrPtr = tmpNextHopPointerPtr;
    }

    return GT_OK;
}

