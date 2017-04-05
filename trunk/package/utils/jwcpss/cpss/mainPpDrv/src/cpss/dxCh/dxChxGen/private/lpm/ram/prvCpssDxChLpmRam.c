/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* prvCpssDxChLpmRam.c
*
* DESCRIPTION:
*       the CPSS LPM Engine support.
*
* FILE REVISION NUMBER:
*       $Revision: 26 $
*******************************************************************************/

#include <cpss/dxCh/dxChxGen/private/lpm/ram/prvCpssDxChLpmRamTypes.h>
#include <cpss/dxCh/dxChxGen/private/lpm/ram/prvCpssDxChLpmRam.h>
#include <cpss/dxCh/dxChxGen/private/lpm/ram/prvCpssDxChLpmRamTrie.h>
#include <cpssCommon/private/prvCpssDevMemManager.h>
#include <cpss/extServices/private/prvCpssBindFunc.h>
#include <cpss/dxCh/dxChxGen/private/lpm/ram/prvCpssDxChLpmRamUc.h>
#include <cpss/dxCh/dxChxGen/private/lpm/ram/prvCpssDxChLpmRamMc.h>
#include <cpssCommon/private/prvCpssSkipList.h>
#include <cpss/dxCh/dxChxGen/config/private/prvCpssDxChInfo.h>
#include <cpss/dxCh/dxChxGen/private/lpm/ram/prvCpssDxChLpmRamMng.h>
#include <cpss/dxCh/dxChxGen/systemRecovery/hsu/private/prvCpssDxChHsu.h>
#include <cpss/dxCh/dxChxGen/private/lpm/prvCpssDxChLpmUtils.h>
#include <cpss/dxCh/dxChxGen/lpm/cpssDxChLpm.h>

#ifdef CPSS_DXCH_LPM_DEBUG_MEM
GT_U32 prvCpssDxChLpmRamMemSize = 0;
#endif

void * cpssOsLpmMalloc
(
    IN GT_U32 size
)
{
#ifdef CPSS_DXCH_LPM_DEBUG_MEM
    static GT_U32 k = 0;
    prvCpssDxChLpmRamMemSize += size;
    k++;
#endif

    GT_VOID_PTR ptr = cpssOsMalloc(size);

    if(ptr)
    {
        cpssOsMemSet(ptr,0,size);
    }

    return ptr;
}

void cpssOsLpmFree
(
    IN void* const memblock
)
{
#ifdef CPSS_DXCH_LPM_DEBUG_MEM
    static GT_U32 k = 0;
    GT_U32 size1,size2,size3,size4;

    size1 = size2 = size3 = 0;
    printMemDelta(&size1,&size2,&size3);
    size4 = prvCpssDxChLpmRamMemSize;
#endif
    cpssOsFree(memblock);
#ifdef CPSS_DXCH_LPM_DEBUG_MEM
    size1 = size2 = size3 = 0;
    printMemDelta(&prvCpssDxChLpmRamMemSize,&size2,&size3);
    size4 = size4 - prvCpssDxChLpmRamMemSize;
    k++;
#endif
}

/*******************************************************************************
**                               local defines                                **
*******************************************************************************/
#if 0
#define PRV_CPSS_DXCH_LPM_RAM_HSU_DMM_FREE_LINK_LIST_END_CNS 0xfffffffd
#define PRV_CPSS_DXCH_LPM_RAM_HSU_DMM_PARTITION_END_CNS      0xfffffffe
#endif

/*
 * Typedef: enum PRV_CPSS_DXCH_LPM_RAM_SEARCH_MEM_UPDATE_TYPE_ENT
 *
 * Description:
 *      Indicates how the LPM search structures should be updated
 *
 *
 * Fields:
 *      PRV_CPSS_DXCH_LPM_RAM_SEARCH_MEM_UPDATE_E -
 *                                      don't allocate memory in the HW.
 *      PRV_CPSS_DXCH_LPM_RAM_SEARCH_MEM_UPDATE_AND_ALLOC_E -
 *                                      allocate memory in the HW.
 *      PRV_CPSS_DXCH_LPM_RAM_SEARCH_MEM_OVERWRITE_E -
 *                                      overwrite and allocate memory if needed.
 *
 */
typedef enum
{
    PRV_CPSS_DXCH_LPM_RAM_SEARCH_MEM_UPDATE_E = 0,
    PRV_CPSS_DXCH_LPM_RAM_SEARCH_MEM_UPDATE_AND_ALLOC_E,
    PRV_CPSS_DXCH_LPM_RAM_SEARCH_MEM_OVERWRITE_E
} PRV_CPSS_DXCH_LPM_RAM_SEARCH_MEM_UPDATE_TYPE_ENT;

#if 0
/*
 * Typedef: enum IP_HSU_STAGE_ENT
 *
 * Description: A enum representing an ip coreExMxPm shadow iterartor stage
 *
 * values:
 *      IP_HSU_VR_STAGE_E        - VR router stage
 *      IP_HSU_UC_PREFIX_STAGE_E - ip uc prefix stage
 *      IP_HSU_UC_LPM_STAGE_E    - ip uc lpm stage
 *      IP_HSU_MC_PREFIX_STAGE_E - ip mc prefix stage
 *      IP_HSU_MC_LPM_STAGE_E    - ip mc lpm stage
 *      IP_HSU_DMM_FREE_LIST_E   - dmm free lists stage
 */
typedef enum
{
    IP_HSU_VR_STAGE_E = 0,
    IP_HSU_UC_PREFIX_STAGE_E ,
    IP_HSU_UC_LPM_STAGE_E,
    IP_HSU_MC_PREFIX_STAGE_E,
    IP_HSU_MC_LPM_STAGE_E,
    IP_HSU_DMM_FREE_LIST_E,
    IP_HSU_IP_LAST_E

}IP_HSU_STAGE_ENT;

/*
 * Typedef: struct IP_HSU_ITERATOR_STC
 *
 * Description: A struct that holds an ip coreExMxPm shadow iterartor
 *
 * Fields:
 *      magic        - iterator magic number.
 *      currMemPtr   - the current memory address we're working on.
 *      currMemSize  - the current accumulated memory size.
 *      currIter - the current stage iterator.
 *      currStage - the current stage.
 *      currProtocolStack - the current Protocol stack.
 *      currVrId - the current vrId.
 *      currShadowIdx - shadow index.
 */
typedef struct IP_HSU_ITERATOR_STCT
{
    GT_U32                          magic;
    GT_VOID                         *currMemPtr;
    GT_U32                          currMemSize;
    GT_UINTPTR                      currIter;
    IP_HSU_STAGE_ENT                currStage;
    CPSS_IP_PROTOCOL_STACK_ENT      currProtocolStack;
    GT_U32                          currVrId;
    GT_U32                          currShadowIdx;
}IP_HSU_ITERATOR_STC;

/*
 * Typedef: struct IPV4_HSU_MC_ROUTE_ENTRY_STC
 *
 * Description: A structure to hold multicast route entry info. Used for HSU
 *              Pp synchronization.
 *
 * Fields:
 *      vrId                 - The virtual router identifier.
 *      mcGroup              - MC group IP address
 *      ipSrc                - MC source IP address
 *      ipSrcPrefixlength    - ip source prefix length
 *      mcRoutePointer       - Multicast Route Entry pointer
 *      last                 - weather this is the last entry
 */
typedef struct IPV4_HSU_MC_ROUTE_ENTRY_STCT
{
    GT_U32                                      vrId;
    GT_IPADDR                                   mcGroup;
    GT_IPADDR                                   ipSrc;
    GT_U32                                      ipSrcPrefixlength;
    PRV_CPSS_DXCH_LPM_ROUTE_ENTRY_POINTER_STC   mcRoutePointer;
    GT_BOOL                                     last;
}IPV4_HSU_MC_ROUTE_ENTRY_STC;

/*
 * Typedef: struct IPV6_HSU_MC_ROUTE_ENTRY_STC
 *
 * Description: A structure to hold multicast route entry info. Used for HSU
 *              Pp synchronization.
 *
 * Fields:
 *      vrId              - The virtual router identifier.
 *      mcGroup           - MC group IP address
 *      ipSrc             - MC source IP address
 *      ipSrcPrefixlength - ip source prefix length
 *      mcRoutePointer    - Multicast Route Entry pointer
 *      last              - weather this is the last entry
 *      groupScope        - the group scope.
 */
typedef struct IPV6_HSU_MC_ROUTE_ENTRY_STCT
{
    GT_U32                                      vrId;
    GT_IPV6ADDR                                 mcGroup;
    GT_IPV6ADDR                                 ipSrc;
    GT_U32                                      ipSrcPrefixlength;
    PRV_CPSS_DXCH_LPM_ROUTE_ENTRY_POINTER_STC   mcRoutePointer;
    GT_BOOL                                     last;
    CPSS_IPV6_PREFIX_SCOPE_ENT                  groupScope;
}IPV6_HSU_MC_ROUTE_ENTRY_STC;

/*
 * Typedef: struct DMM_IP_HSU_ENTRY_STC
 *
 * Description: A structure to hold hsu DMM entry info. Used for HSU
 *              Pp synchronization.
 *
 * Fields:
 *      partitionIndex            - dmm partition index.
 *      currentFreeLinkList       - it shows for which memory size this list is intended.
 *      currentDmmBlockAddrInList - current DMM block in the list.
 *      endOfList                 - list was processed but end of list signature was not done.
 *      endOfPartition            - partition was processed but end of partition signature was
 *                                  not done
 *
 */
typedef struct DMM_IP_HSU_ENTRY_STCT
{
    GT_U32                                  partitionIndex;
    GT_U32                                  currentFreeLinkList;
    GT_DMM_BLOCK                            *currentDmmBlockAddrInList;
    GT_BOOL                                 endOfList;
    GT_BOOL                                 endOfPartition;
    GT_DMM_PARTITION                        *partitionArray[8];
}DMM_IP_HSU_ENTRY_STC;
#endif

/*******************************************************************************
**                             forward declarations                           **
*******************************************************************************/

/* Check validity of values of route entry pointer */
static GT_STATUS prvCpssDxChLpmRouteEntryPointerCheck
(
    IN  PRV_CPSS_DXCH_LPM_ROUTE_ENTRY_POINTER_STC  *routeEntryPointerPtr
);

/*******************************************************************************
**                               local functions                              **
*******************************************************************************/

/*******************************************************************************
* deleteShadowStruct
*
* DESCRIPTION:
*       This function deletes existing shadow struct.
*
* INPUTS:
*       shadowPtr          - points to the shadow to delete
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK on success, or
*       GT_FAIL otherwise.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS deleteShadowStruct
(
    IN  PRV_CPSS_DXCH_LPM_RAM_SHADOW_STC *shadowPtr
)
{
    GT_STATUS       rc;
    GT_U32          i,octet;
    PRV_CPSS_DXCH_LPM_PROTOCOL_STACK_ENT protocol;
    GT_U32          numOfOctetsPerProtocol[PRV_CPSS_DXCH_LPM_RAM_NUM_OF_UC_PROTOCOLS_CNS] = {PRV_CPSS_DXCH_LPM_NUM_OF_OCTETS_IN_IPV4_PROTOCOL_CNS,
                                                                                             PRV_CPSS_DXCH_LPM_NUM_OF_OCTETS_IN_IPV6_PROTOCOL_CNS,
                                                                                             PRV_CPSS_DXCH_LPM_NUM_OF_OCTETS_IN_FCOE_PROTOCOL_CNS};
    PRV_CPSS_DXCH_LPM_RAM_MEM_INFO_STC startOfOctetList;
    PRV_CPSS_DXCH_LPM_RAM_MEM_INFO_STC *tempNextMemInfoPtr;

    /* first make sure all virtual routers are deleted */
    for (i = 0 ; i < shadowPtr->vrfTblSize ; i++)
    {
        if (shadowPtr->vrRootBucketArray[i].valid == GT_TRUE)
        {
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_FAIL, LOG_ERROR_NO_MSG);
        }
    }

    /* now free resources used by the shadow struct */

    /* free memory set to vr table array */
    cpssOsLpmFree(shadowPtr->vrRootBucketArray);

    /* free all memory pools allocated */
    for (i = 0 ; i < shadowPtr->numOfLpmMemories ; i++)
    {
        if(shadowPtr->lpmRamStructsMemPoolPtr[i]!=0)
        {
            rc = prvCpssDmmClosePartition(shadowPtr->lpmRamStructsMemPoolPtr[i]);
            if (rc != GT_OK)
            {
                 /* free alloctated arrays used for RAM configuration */
                cpssOsFree(shadowPtr->lpmRamBlocksSizeArrayPtr);
                cpssOsFree(shadowPtr->lpmRamStructsMemPoolPtr);
                cpssOsFree(shadowPtr->lpmRamOctetsToBlockMappingPtr);

                return  rc;
            }
        }
    }

    /* free alloctated arrays used for RAM configuration */
    cpssOsFree(shadowPtr->lpmRamBlocksSizeArrayPtr);
    cpssOsFree(shadowPtr->lpmRamStructsMemPoolPtr);
    cpssOsFree(shadowPtr->lpmRamOctetsToBlockMappingPtr);

    /* go over all linked list per protocol for all octets and free it */
    for (protocol = 0; protocol < PRV_CPSS_DXCH_LPM_PROTOCOL_LAST_E; protocol++)
    {
        for (octet = 0; octet < numOfOctetsPerProtocol[protocol]; octet++)
        {
            startOfOctetList = shadowPtr->lpmMemInfoArray[protocol][octet];
            while(startOfOctetList.nextMemInfoPtr != NULL)
            {
                /* keep a temp pointer to the element in the list we need to free */
                tempNextMemInfoPtr = startOfOctetList.nextMemInfoPtr;
                /* assign a new next element for the startOfOctetList */
                startOfOctetList.nextMemInfoPtr = tempNextMemInfoPtr->nextMemInfoPtr;
                /* free the memory kept in the temp pointer */
                cpssOsFree(tempNextMemInfoPtr);
            }
        }
    }

    /* free memory allocated to swap */
    if (shadowPtr->swapMemoryAddr != 0)
    {
        cpssOsLpmFree((void*)shadowPtr->swapMemoryAddr);
    }

    /* remove add devices from device list */
    if (shadowPtr->shadowDevList.shareDevs != NULL)
    {
        cpssOsLpmFree(shadowPtr->shadowDevList.shareDevs);
    }

    return GT_OK;
}

/*******************************************************************************
* createShadowStruct
*
* DESCRIPTION:
*       This function returns a pointer to a new shadow struct.
*
* INPUTS:
*       shadowType            - the type of shadow to maintain
*       protocolBitmap        - the protocols this shodow supports
*       memoryCfgPtr          - (pointer to) the memory configuration of this LPM DB*
*
* OUTPUTS:
*       shadowPtr             - points to the created shadow struct
*
* RETURNS:
*       GT_OK on success, or
*       GT_FAIL otherwise.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS createShadowStruct
(
    IN  PRV_CPSS_DXCH_LPM_SHADOW_TYPE_ENT   shadowType,
    IN  PRV_CPSS_DXCH_LPM_PROTOCOL_BMP      protocolBitmap,
    IN PRV_CPSS_DXCH_LPM_RAM_CONFIG_STC     *memoryCfgPtr,
    OUT PRV_CPSS_DXCH_LPM_RAM_SHADOW_STC    *shadowPtr
)
{
    GT_U8 numOfDevs;        /* Number of devices in system.                                     */
    GT_STATUS retVal = GT_OK;
    GT_U32 memSize;         /* size of the memory block to be inserted to                       */
                            /* the memory management unit.                                      */
    GT_U32 memSizeBetweenBlocksInBytes; /* the size between each memory block management unit   */
    GT_DMM_BLOCK  *dmmBlockPtr;

    GT_U32      i,j;
    GT_BOOL     isProtocolInitialized[PRV_CPSS_DXCH_LPM_PROTOCOL_LAST_E];
    GT_UINTPTR  structsMemPool[PRV_CPSS_DXCH_LPM_RAM_NUM_OF_MEMORIES_CNS];
    GT_U32      numOfOctetsPerProtocol[PRV_CPSS_DXCH_LPM_PROTOCOL_LAST_E] = {PRV_CPSS_DXCH_LPM_NUM_OF_OCTETS_IN_IPV4_PROTOCOL_CNS,
                                                                             PRV_CPSS_DXCH_LPM_NUM_OF_OCTETS_IN_IPV6_PROTOCOL_CNS,
                                                                             PRV_CPSS_DXCH_LPM_NUM_OF_OCTETS_IN_FCOE_PROTOCOL_CNS};

    numOfDevs = PRV_CPSS_MAX_PP_DEVICES_CNS;

    /* zero out the shadow */
    cpssOsMemSet(shadowPtr,0,sizeof(PRV_CPSS_DXCH_LPM_RAM_SHADOW_STC));
    shadowPtr->shadowType = shadowType;

    /* update the protocols that the shadow needs to support */
    isProtocolInitialized[PRV_CPSS_DXCH_LPM_PROTOCOL_IPV4_E] =
        PRV_CPSS_DXCH_LPM_PROTOCOL_IPV4_STATUS_GET_MAC(protocolBitmap);
    isProtocolInitialized[PRV_CPSS_DXCH_LPM_PROTOCOL_IPV6_E] =
        PRV_CPSS_DXCH_LPM_PROTOCOL_IPV6_STATUS_GET_MAC(protocolBitmap);
    isProtocolInitialized[PRV_CPSS_DXCH_LPM_PROTOCOL_FCOE_E] =
        PRV_CPSS_DXCH_LPM_PROTOCOL_FCOE_STATUS_GET_MAC(protocolBitmap);

    /* reset the to be freed memory list */
    shadowPtr->freeMemListDuringUpdate = NULL;
    shadowPtr->freeMemListEndOfUpdate = NULL;

    /* we keep the numOfBlocks value as the numOfLpmMemories.
       Block and Memory has the same meaning.
       We use numOfLpmMemories due to legacy code */
    shadowPtr->numOfLpmMemories = memoryCfgPtr->numOfBlocks;

    /* allocate needed RAM configuration arrays */
    shadowPtr->lpmRamBlocksSizeArrayPtr = cpssOsMalloc(sizeof(GT_U32)*shadowPtr->numOfLpmMemories);
    if ( shadowPtr->lpmRamBlocksSizeArrayPtr == NULL)
    {
        /* fail to allocate space */
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_OUT_OF_CPU_MEM, LOG_ERROR_NO_MSG);
    }
    shadowPtr->lpmRamStructsMemPoolPtr = cpssOsMalloc(sizeof(GT_UINTPTR) * shadowPtr->numOfLpmMemories);
    if ( shadowPtr->lpmRamStructsMemPoolPtr == NULL)
    {
        /* fail to allocate space */
        cpssOsFree(shadowPtr->lpmRamBlocksSizeArrayPtr);
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_OUT_OF_CPU_MEM, LOG_ERROR_NO_MSG);
    }
    shadowPtr->lpmRamOctetsToBlockMappingPtr = cpssOsMalloc(sizeof(PRV_CPSS_DXCH_LPM_RAM_OCTETS_TO_BLOCK_MAPPING_STC)*shadowPtr->numOfLpmMemories);
    if ( shadowPtr->lpmRamOctetsToBlockMappingPtr == NULL)
    {
        /* fail to allocate space */
        cpssOsFree(shadowPtr->lpmRamBlocksSizeArrayPtr);
        cpssOsFree(shadowPtr->lpmRamStructsMemPoolPtr);
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_OUT_OF_CPU_MEM, LOG_ERROR_NO_MSG);
    }
    /* reset needed RAM configuration arrays */
    cpssOsMemSet(shadowPtr->lpmRamBlocksSizeArrayPtr,0,sizeof(GT_U32)*shadowPtr->numOfLpmMemories);
    cpssOsMemSet(shadowPtr->lpmRamStructsMemPoolPtr,0,sizeof(GT_UINTPTR)*shadowPtr->numOfLpmMemories);
    cpssOsMemSet(shadowPtr->lpmRamOctetsToBlockMappingPtr,0,sizeof(PRV_CPSS_DXCH_LPM_RAM_OCTETS_TO_BLOCK_MAPPING_STC)*shadowPtr->numOfLpmMemories);

    for (i=0;i<shadowPtr->numOfLpmMemories;i++)
    {
         shadowPtr->lpmRamBlocksSizeArrayPtr[i] = memoryCfgPtr->blocksSizeArray[i];
    }

    shadowPtr->lpmRamTotalBlocksSizeIncludingGap = PRV_CPSS_DXCH_LPM_RAM_TOTAL_BLOCKS_SIZE_INCLUDING_GAP_CNS;
    shadowPtr->lpmRamBlocksAllocationMethod = memoryCfgPtr->blocksAllocationMethod;

    /* init the search memory pools */
    for (i = 0; i < shadowPtr->numOfLpmMemories; i++)
    {
        /* Swap area is in the first block. In order to leave space for it, we
           deduct the size of a max LPM bucket */
        if (i == 0)
        {
             /* in the first block there must be space for the swap area */
            if (shadowPtr->lpmRamBlocksSizeArrayPtr[0] < PRV_CPSS_DXCH_LPM_RAM_MAX_SIZE_OF_BUCKET_IN_LPM_LINES_CNS)
            {
                cpssOsFree(shadowPtr->lpmRamBlocksSizeArrayPtr);
                cpssOsFree(shadowPtr->lpmRamStructsMemPoolPtr);
                cpssOsFree(shadowPtr->lpmRamOctetsToBlockMappingPtr);
                CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NO_RESOURCE, LOG_ERROR_NO_MSG);
            }

            memSize = shadowPtr->lpmRamBlocksSizeArrayPtr[0] - PRV_CPSS_DXCH_LPM_RAM_MAX_SIZE_OF_BUCKET_IN_LPM_LINES_CNS;
            /* record the swap memory address (at the end of the structs memory)*/
            dmmBlockPtr = (GT_DMM_BLOCK*)cpssOsLpmMalloc(sizeof(GT_DMM_BLOCK));
            if (dmmBlockPtr == NULL)
            {
                cpssOsFree(shadowPtr->lpmRamBlocksSizeArrayPtr);
                cpssOsFree(shadowPtr->lpmRamStructsMemPoolPtr);
                cpssOsFree(shadowPtr->lpmRamOctetsToBlockMappingPtr);
                CPSS_LOG_ERROR_AND_RETURN_MAC(GT_OUT_OF_CPU_MEM, LOG_ERROR_NO_MSG);
            }

            SET_OFFSET_IN_WORDS(dmmBlockPtr, memSize);

            /* - mark swap dmm block by 0xFFFFFFFE partition Id.*/
    #if __WORDSIZE == 64
            dmmBlockPtr->nextBySizeOrPartitionPtr.partitionPtr = (GT_DMM_PARTITION*)0xFFFFFFFFFFFFFFFE;
    #else
            dmmBlockPtr->nextBySizeOrPartitionPtr.partitionPtr = (GT_DMM_PARTITION*)0xFFFFFFFE;
    #endif

            shadowPtr->swapMemoryAddr = (GT_UINTPTR)dmmBlockPtr;
        }
        else
        {
            memSize = shadowPtr->lpmRamBlocksSizeArrayPtr[i];
        }

        memSizeBetweenBlocksInBytes  = shadowPtr->lpmRamTotalBlocksSizeIncludingGap * PRV_CPSS_DXCH_LPM_RAM_SIZE_OF_LPM_ENTRY_IN_BYTES_CNS;

        if (prvCpssDmmCreatePartition(DMM_MIN_ALLOCATE_SIZE_IN_BYTE_CNS * memSize,
                                      i * memSizeBetweenBlocksInBytes,
                                      DMM_MIN_ALLOCATE_SIZE_IN_BYTE_CNS,
                                      DMM_MIN_ALLOCATE_SIZE_IN_BYTE_CNS * PRV_CPSS_DXCH_LPM_RAM_MAX_SIZE_OF_BUCKET_IN_LPM_LINES_CNS,
                                      &structsMemPool[i]) != GT_OK)
        {

            cpssOsFree(shadowPtr->lpmRamBlocksSizeArrayPtr);
            cpssOsFree(shadowPtr->lpmRamStructsMemPoolPtr);
            cpssOsFree(shadowPtr->lpmRamOctetsToBlockMappingPtr);
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_FAIL, LOG_ERROR_NO_MSG);
        }

        /* keep all memory pool Id's for future use, when binding octets to the blocks */
        shadowPtr->lpmRamStructsMemPoolPtr[i] = structsMemPool[i];
    }

    /* block 0 is used by default entries */
    shadowPtr->lpmRamOctetsToBlockMappingPtr[0].isBlockUsed=GT_TRUE;

    for (i = 0; i < PRV_CPSS_DXCH_LPM_PROTOCOL_LAST_E; i++)
    {
        /* for all the protocols octet 0 is mapped to block 0 */
        shadowPtr->lpmMemInfoArray[i][0].structsMemPool = shadowPtr->lpmRamStructsMemPoolPtr[0];
        shadowPtr->lpmMemInfoArray[i][0].ramIndex = 0;
        shadowPtr->lpmMemInfoArray[i][0].structsBase = 0;
        shadowPtr->lpmMemInfoArray[i][0].nextMemInfoPtr = NULL;

        for (j = 1; j < numOfOctetsPerProtocol[i]; j++)
        {
            /* all blocks except 0 are not mapped */
            shadowPtr->lpmMemInfoArray[i][j].structsMemPool = 0;
            shadowPtr->lpmMemInfoArray[i][j].ramIndex = 0;
            shadowPtr->lpmMemInfoArray[i][j].structsBase = 0;
            shadowPtr->lpmMemInfoArray[i][j].nextMemInfoPtr = NULL;
        }

        /* block 0 is used by all the protocols  - mark bit 0 as 0x1*/
        PRV_CPSS_DXCH_LPM_RAM_OCTET_TO_BLOCK_MAPPING_SET_MAC(shadowPtr,i,0,0);
    }

    /* IPv4 init the lpm level's memory usage */
    /* meaning of i - octet in IP address used for level in LPM search .*/
    for (i = 0; i < PRV_CPSS_DXCH_LPM_NUM_OF_OCTETS_IN_IPV4_PROTOCOL_CNS ; i++)
    {
        /* for the uc search */
        shadowPtr->ucSearchMemArrayPtr[PRV_CPSS_DXCH_LPM_PROTOCOL_IPV4_E][i] =
            &shadowPtr->lpmMemInfoArray[PRV_CPSS_DXCH_LPM_PROTOCOL_IPV4_E][i];

        /* for the mc grp search */
        shadowPtr->mcSearchMemArrayPtr[PRV_CPSS_DXCH_LPM_PROTOCOL_IPV4_E][i] =
            &shadowPtr->lpmMemInfoArray[PRV_CPSS_DXCH_LPM_PROTOCOL_IPV4_E][i];

        /* for the mc src search */
        shadowPtr->mcSearchMemArrayPtr[PRV_CPSS_DXCH_LPM_PROTOCOL_IPV4_E][i+PRV_CPSS_DXCH_LPM_NUM_OF_OCTETS_IN_IPV4_PROTOCOL_CNS] =
            &shadowPtr->lpmMemInfoArray[PRV_CPSS_DXCH_LPM_PROTOCOL_IPV4_E][i];
    }

     /* IPv6 init the lpm level's memory usage */
     /* meaning of i - octet in IP address used for level in LPM search .*/
    for (i = 0; i < PRV_CPSS_DXCH_LPM_NUM_OF_OCTETS_IN_IPV6_PROTOCOL_CNS ; i++)
    {
        /* for the uc search */
        shadowPtr->ucSearchMemArrayPtr[PRV_CPSS_DXCH_LPM_PROTOCOL_IPV6_E][i] =
            &shadowPtr->lpmMemInfoArray[PRV_CPSS_DXCH_LPM_PROTOCOL_IPV6_E][i];

        /* for the mc grp search */
        shadowPtr->mcSearchMemArrayPtr[PRV_CPSS_DXCH_LPM_PROTOCOL_IPV6_E][i] =
            &shadowPtr->lpmMemInfoArray[PRV_CPSS_DXCH_LPM_PROTOCOL_IPV6_E][i];

        /* for the mc src search */
        shadowPtr->mcSearchMemArrayPtr[PRV_CPSS_DXCH_LPM_PROTOCOL_IPV6_E][i+PRV_CPSS_DXCH_LPM_NUM_OF_OCTETS_IN_IPV6_PROTOCOL_CNS] =
            &shadowPtr->lpmMemInfoArray[PRV_CPSS_DXCH_LPM_PROTOCOL_IPV6_E][i];
    }

    /* Allocate the device list */
    shadowPtr->shadowDevList.shareDevs = (GT_U8*)cpssOsLpmMalloc(sizeof(GT_U8) * numOfDevs);

    if (shadowPtr->shadowDevList.shareDevs == NULL)
    {
        cpssOsFree(shadowPtr->lpmRamBlocksSizeArrayPtr);
        cpssOsFree(shadowPtr->lpmRamStructsMemPoolPtr);
        cpssOsFree(shadowPtr->lpmRamOctetsToBlockMappingPtr);
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_OUT_OF_CPU_MEM, LOG_ERROR_NO_MSG);
    }

    /* set the working devlist (which is empty curretly)*/
    shadowPtr->workDevListPtr = &shadowPtr->shadowDevList;

    cpssOsMemCpy(shadowPtr->isProtocolInitialized, isProtocolInitialized, PRV_CPSS_DXCH_LPM_PROTOCOL_LAST_E * sizeof(GT_BOOL));

    shadowPtr->vrfTblSize = PRV_CPSS_DXCH_LPM_RAM_NUM_OF_VIRTUAL_ROUTERS_CNS;
    shadowPtr->vrRootBucketArray =
        cpssOsLpmMalloc(sizeof(PRV_CPSS_DXCH_LPM_RAM_ROOT_BUCKET_STC) * shadowPtr->vrfTblSize);
    if (shadowPtr->vrRootBucketArray == NULL)
    {
        cpssOsFree(shadowPtr->lpmRamBlocksSizeArrayPtr);
        cpssOsFree(shadowPtr->lpmRamStructsMemPoolPtr);
        cpssOsFree(shadowPtr->lpmRamOctetsToBlockMappingPtr);
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_OUT_OF_CPU_MEM, LOG_ERROR_NO_MSG);
    }
    for (i = 0; i < shadowPtr->vrfTblSize; i++)
    {
        shadowPtr->vrRootBucketArray[i].valid = GT_FALSE;
        shadowPtr->vrRootBucketArray[i].rootBucket[PRV_CPSS_DXCH_LPM_PROTOCOL_IPV4_E] = NULL;
        shadowPtr->vrRootBucketArray[i].rootBucket[PRV_CPSS_DXCH_LPM_PROTOCOL_IPV6_E] = NULL;
        shadowPtr->vrRootBucketArray[i].rootBucket[PRV_CPSS_DXCH_LPM_PROTOCOL_FCOE_E] = NULL;
    }

    return retVal;
}

/*******************************************************************************
* prvCpssDxChLpmRamDbCreate
*
* DESCRIPTION:
*       Function Relevant mode : High Level API modes
*       LPM DB is a database that holds LPM shadow information that is shared
*       to one PP or more. The LPM DB manage adding, deleting and searching
*       This function creates LPM DB for a shared LPM managment.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2.
*
* INPUTS:
*       lpmDbPtrPtr       - the LPM DB
*       protocolBitmap    - the protocols that are used in this LPM DB
*       memoryCfgPtr      - (pointer to) the memory configuration of this LPM DB
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on illegal parameter
*       GT_OUT_OF_CPU_MEM        - on failure to allocate memory
*       GT_FAIL                  - on error
*
* COMMENTS:
*       None.
*
*
*******************************************************************************/
GT_STATUS prvCpssDxChLpmRamDbCreate
(
    IN PRV_CPSS_DXCH_LPM_RAM_SHADOWS_DB_STC  **lpmDbPtrPtr,
    IN PRV_CPSS_DXCH_LPM_PROTOCOL_BMP        protocolBitmap,
    IN PRV_CPSS_DXCH_LPM_RAM_CONFIG_STC      *memoryCfgPtr
)
{
    GT_STATUS                            retVal = GT_OK;
    GT_U32                               shadowIdx;
    PRV_CPSS_DXCH_LPM_RAM_SHADOWS_DB_STC *lpmDbPtr;

    /* right now only one shadow configuration can be attached to LPM DB and
       only PRV_CPSS_DXCH_LPM_RAM_LION3_SHADOW_E is supported */
    GT_U32                              numOfShadowCfg = 1;
    PRV_CPSS_DXCH_LPM_SHADOW_TYPE_ENT   shadowType = PRV_CPSS_DXCH_LPM_RAM_LION3_SHADOW_E;

    *lpmDbPtrPtr = (PRV_CPSS_DXCH_LPM_RAM_SHADOWS_DB_STC*)
                    cpssOsLpmMalloc(sizeof(PRV_CPSS_DXCH_LPM_RAM_SHADOWS_DB_STC));
    if (*lpmDbPtrPtr == NULL)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_OUT_OF_CPU_MEM, LOG_ERROR_NO_MSG);
    }

    lpmDbPtr = *lpmDbPtrPtr;

    /* allocate memory */
    lpmDbPtr->shadowArray =
        cpssOsLpmMalloc(sizeof(PRV_CPSS_DXCH_LPM_RAM_SHADOW_STC) * numOfShadowCfg);
    if (lpmDbPtr->shadowArray == NULL)
    {
        cpssOsLpmFree(lpmDbPtr);
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_OUT_OF_CPU_MEM, LOG_ERROR_NO_MSG);
    }

    /*copy out the lpm db info */
    lpmDbPtr->protocolBitmap = protocolBitmap;
    lpmDbPtr->numOfShadowCfg = numOfShadowCfg;

    /* now initlize the shadows */
    for (shadowIdx = 0; (shadowIdx < numOfShadowCfg) && (retVal == GT_OK);
          shadowIdx++)
    {
        retVal = createShadowStruct(shadowType,
                                    protocolBitmap,
                                    memoryCfgPtr,
                                    &lpmDbPtr->shadowArray[shadowIdx]);
    }

    return retVal;
}

/*******************************************************************************
* prvCpssDxChLpmRamDbDelete
*
* DESCRIPTION:
*       Function Relevant mode : High Level API modes
*       LPM DB is a database that holds LPM shadow information that is shared
*       to one PP or more. The LPM DB manage adding, deleting and searching
*       IPv4/6 UC/MC and FCoE prefixes.
*       This function deletes LPM DB for a shared LPM managment.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2.
*
* INPUTS:
*       lpmDbPtr          - the LPM DB
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on illegal parameter
*       GT_OUT_OF_CPU_MEM        - on failure to allocate memory
*       GT_FAIL                  - on error
*
* COMMENTS:
*       Deleting LPM DB is allowed only if no VR present in the LPM DB (since
*       prefixes reside within VR it means that no prefixes present as well).
*
*******************************************************************************/
GT_STATUS prvCpssDxChLpmRamDbDelete
(
    IN PRV_CPSS_DXCH_LPM_RAM_SHADOWS_DB_STC  *lpmDbPtr
)
{
    GT_STATUS                           rc;         /* function return code */
    GT_U32                              i;

    /* make sure the lpm db id exists */
    /* delete all shadows */
    for (i = 0 ; i < lpmDbPtr->numOfShadowCfg ; i++)
    {
        rc = deleteShadowStruct(&lpmDbPtr->shadowArray[i]);
        if (rc != GT_OK)
        {
            return rc;
        }
    }

    /* free resources used by the lpm db */
    if (lpmDbPtr->shadowArray != NULL)
    {
        cpssOsLpmFree(lpmDbPtr->shadowArray);
    }

    cpssOsLpmFree(lpmDbPtr);

    return GT_OK;
}

/*******************************************************************************
* prvCpssDxChLpmRamDbConfigGet
*
* DESCRIPTION:
*       Function Relevant mode : High Level API modes
*       LPM DB is a database that holds LPM shadow information that is shared
*       to one PP or more. The LPM DB manages adding, deleting and searching
*       IPv4/6 UC/MC and FCoE prefixes.
*       This function retrieves configuration of LPM DB.
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2.
*
* INPUTS:
*       lpmDbPtr          - (pointer to) the LPM DB
*
* OUTPUTS:
*       protocolBitmapPtr - (pointer to) the protocols that are used in this LPM DB
*       memoryCfgPtr      - (pointer to) the memory configuration of this LPM DB
*
* RETURNS:
*       GT_OK             - on success
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS prvCpssDxChLpmRamDbConfigGet
(
    IN  PRV_CPSS_DXCH_LPM_RAM_SHADOWS_DB_STC    *lpmDbPtr,
    OUT PRV_CPSS_DXCH_LPM_PROTOCOL_BMP          *protocolBitmapPtr,
    OUT PRV_CPSS_DXCH_LPM_RAM_CONFIG_STC        *memoryCfgPtr
)
{
    GT_U32 i=0;

    if (protocolBitmapPtr!=NULL)
    {
        *protocolBitmapPtr = lpmDbPtr->protocolBitmap;
    }
    else
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PTR, LOG_ERROR_NO_MSG);
    }

    if (memoryCfgPtr!=NULL)
    {
        memoryCfgPtr->numOfBlocks = lpmDbPtr->shadowArray[0].numOfLpmMemories;
        for (i=0;i<memoryCfgPtr->numOfBlocks;i++)
        {
            memoryCfgPtr->blocksSizeArray[i] = lpmDbPtr->shadowArray[0].lpmRamBlocksSizeArrayPtr[i];
        }
        memoryCfgPtr->blocksAllocationMethod = lpmDbPtr->shadowArray[0].lpmRamBlocksAllocationMethod;
    }
    else
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PTR, LOG_ERROR_NO_MSG);
    }

    return GT_OK;
}

/*******************************************************************************
* isProtocolSetInBitmap
*
* DESCRIPTION:
*       Check whether the protocol is set in the protocols bitmap
*
* INPUTS:
*       protocolBitmap - bitmap of protocols
*       protocol       - the protocol
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_TRUE  - the protocol bit is set in the bitmap, or
*       GT_FALSE - the protocol bit is not set in the bitmap
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_BOOL isProtocolSetInBitmap
(
    IN PRV_CPSS_DXCH_LPM_PROTOCOL_BMP          protocolBitmap,
    IN PRV_CPSS_DXCH_LPM_PROTOCOL_STACK_ENT    protocol
)
{
    if (protocol == PRV_CPSS_DXCH_LPM_PROTOCOL_IPV4_E)
    {
        return PRV_CPSS_DXCH_LPM_PROTOCOL_IPV4_STATUS_GET_MAC(protocolBitmap);
    }
    if (protocol == PRV_CPSS_DXCH_LPM_PROTOCOL_IPV6_E)
    {
        return PRV_CPSS_DXCH_LPM_PROTOCOL_IPV6_STATUS_GET_MAC(protocolBitmap);
    }
    if (protocol == PRV_CPSS_DXCH_LPM_PROTOCOL_FCOE_E)
    {
        return PRV_CPSS_DXCH_LPM_PROTOCOL_FCOE_STATUS_GET_MAC(protocolBitmap);
    }
    return GT_FALSE;
}

/*******************************************************************************
* updateHwSearchStc
*
* DESCRIPTION:
*       updates the search structures accroding to the parameters passed
*
* INPUTS:
*       updateType     - indicates which and how entries will be updated
*       protocolBitmap - bitmap of protocols to work on.
*       shadowPtr      - the shadow relevant for the devices asked to act on.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK    - on success, or
*       GT_ERROR - If the vrId was not created yet, or
*       GT_FAIL  - otherwise.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS updateHwSearchStc
(
    IN PRV_CPSS_DXCH_LPM_RAM_SEARCH_MEM_UPDATE_TYPE_ENT updateType,
    IN PRV_CPSS_DXCH_LPM_PROTOCOL_BMP                   protocolBitmap,
    IN PRV_CPSS_DXCH_LPM_RAM_SHADOW_STC                 *shadowPtr
)
{
    GT_STATUS                                       retVal = GT_OK;
    PRV_CPSS_DXCH_LPM_RAM_RANGE_SHADOW_STC          tmpRootRange;
    PRV_CPSS_DXCH_LPM_RAM_TRIE_UPDATE_TYPE_ENT      lpmTrieUpdateType;
    GT_U32                                          vrId;
    PRV_CPSS_DXCH_LPM_PROTOCOL_STACK_ENT            currProtocolStack;

    for (currProtocolStack = PRV_CPSS_DXCH_LPM_PROTOCOL_IPV4_E;
          currProtocolStack < PRV_CPSS_DXCH_LPM_PROTOCOL_LAST_E;
          currProtocolStack++)
    {
        if ((isProtocolSetInBitmap(protocolBitmap, currProtocolStack) == GT_FALSE) ||
            (shadowPtr->isProtocolInitialized[currProtocolStack] == GT_FALSE))
        {
            continue;
        }
        for (vrId = 0 ; vrId < shadowPtr->vrfTblSize; vrId++)
        {
            /* go over all valid VR , if it's initilized for this protocol */
            if ((shadowPtr->vrRootBucketArray[vrId].valid == GT_FALSE) ||
                (shadowPtr->vrRootBucketArray[vrId].rootBucket[currProtocolStack] == NULL))
            {
                continue;
            }

            /* if we are in update mode and the vr doesn't need an update , don't
               update */
            if ((updateType != PRV_CPSS_DXCH_LPM_RAM_SEARCH_MEM_OVERWRITE_E) &&
                (shadowPtr->vrRootBucketArray[vrId].needsHwUpdate == GT_FALSE))
            {
                continue;
            }

            retVal = prvCpssDxChLpmRamMemFreeListMng(0,PRV_CPSS_DXCH_LPM_RAM_MEM_LIST_RESET_OP_E,
                                                     &shadowPtr->freeMemListDuringUpdate);
            if (retVal != GT_OK)
                return retVal;

            if (shadowPtr->vrRootBucketArray[vrId].rootBucket[currProtocolStack])
            {
                switch (updateType)
                {
                case PRV_CPSS_DXCH_LPM_RAM_SEARCH_MEM_UPDATE_E:
                    lpmTrieUpdateType = PRV_CPSS_DXCH_LPM_RAM_TRIE_UPDATE_HW_ONLY_E;
                    break;
                case PRV_CPSS_DXCH_LPM_RAM_SEARCH_MEM_UPDATE_AND_ALLOC_E:
                    lpmTrieUpdateType = PRV_CPSS_DXCH_LPM_RAM_TRIE_UPDATE_HW_AND_ALLOC_MEM_E;
                    break;
                case PRV_CPSS_DXCH_LPM_RAM_SEARCH_MEM_OVERWRITE_E:
                    lpmTrieUpdateType = PRV_CPSS_DXCH_LPM_RAM_TRIE_WRITE_HW_AND_ALLOC_MEM_E;
                    break;
                CPSS_COVERITY_NON_ISSUE_BOOKMARK
                /* coverity[dead_error_begin] */
                default:
                    CPSS_LOG_ERROR_AND_RETURN_MAC(GT_FAIL, LOG_ERROR_NO_MSG);
                }

                /* write the changes in the tree */
                tmpRootRange.lowerLpmPtr.nextBucket =
                    shadowPtr->vrRootBucketArray[vrId].rootBucket[currProtocolStack];
                tmpRootRange.pointerType =
                    (GT_U8)(shadowPtr->vrRootBucketArray[vrId].rootBucketType[currProtocolStack]);
                tmpRootRange.next = NULL;
                retVal =
                    prvCpssDxChLpmRamMngBucketTreeWrite(&tmpRootRange,
                                                        shadowPtr->ucSearchMemArrayPtr[currProtocolStack],
                                                        shadowPtr,lpmTrieUpdateType,
                                                        currProtocolStack,vrId);
                if (retVal == GT_OK)
                {
                    /* now write the vr table */
                    retVal = prvCpssDxChLpmRamMngVrfEntryUpdate(vrId, currProtocolStack, shadowPtr);
                    if (retVal == GT_OK)
                    {
                        /* indicate the update was done */
                        shadowPtr->vrRootBucketArray[vrId].needsHwUpdate = GT_FALSE;

                    }
                }
            }

            /* now it's possible to free all the "end of update to be freed
               memory" memory , now that the HW is updated (and the "during
               update to be freed" memory if needed ) */
            retVal = prvCpssDxChLpmRamMemFreeListMng(0,PRV_CPSS_DXCH_LPM_RAM_MEM_LIST_FREE_MEM_OP_E,
                                                     &shadowPtr->freeMemListDuringUpdate);
            if (retVal != GT_OK)
            {
                return retVal;
            }

            retVal = prvCpssDxChLpmRamMemFreeListMng(0,PRV_CPSS_DXCH_LPM_RAM_MEM_LIST_FREE_MEM_OP_E,
                                                     &shadowPtr->freeMemListEndOfUpdate);
            if (retVal != GT_OK)
            {
                return retVal;
            }
        }
    }

    return (retVal);
}

/*******************************************************************************
* prvCpssDxChLpmRamDbDevListAdd
*
* DESCRIPTION:
*       Function Relevant mode : High Level API modes
*       This function adds list of devices to an existing LPM DB.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2.
*
* INPUTS:
*       lpmDbPtr          - the LPM DB
*       devListArray      - array of device ids to add to the LPM DB
*       numOfDevs         - the number of device ids in the array
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on illegal parameter
*       GT_BAD_PTR               - on NULL pointer
*       GT_HW_ERROR              - on hardware error
*       GT_FAIL                  - on error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS prvCpssDxChLpmRamDbDevListAdd
(
    IN PRV_CPSS_DXCH_LPM_RAM_SHADOWS_DB_STC  *lpmDbPtr,
    IN GT_U8                                 devListArray[],
    IN GT_U32                                numOfDevs
)
{
    GT_STATUS                                   retVal = GT_OK;
    GT_U32                                      shadowIdx, devIdx, i;
    static GT_U8                                shadowDevList[PRV_CPSS_MAX_PP_DEVICES_CNS];
    GT_U32                                      numOfShadowDevs=0;
    PRV_CPSS_DXCH_LPM_RAM_SHADOW_DEVS_LIST_STC  addedDevList;
    PRV_CPSS_DXCH_LPM_RAM_SHADOW_DEVS_LIST_STC  *shadowDevListPtr;
    GT_U32                                      totalLpmSize=0,lpmRamNumOfLines=0;


    /* first check the devices we got fit the lpm DB shadows */
    for (devIdx = 0 ; devIdx < numOfDevs; devIdx++)
    {
        PRV_CPSS_DXCH_DEV_CHECK_MAC(devListArray[devIdx]);
        for (shadowIdx = 0 ; shadowIdx < lpmDbPtr->numOfShadowCfg; shadowIdx++)
        {
            /* check if shadow fit to devices*/
            if ( (lpmDbPtr->shadowArray[shadowIdx].shadowType == PRV_CPSS_DXCH_LPM_RAM_LION3_SHADOW_E) &&
                 (!PRV_CPSS_SIP_5_CHECK_MAC(devListArray[devIdx])))
            {
                CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
            }

            totalLpmSize = 0;
            /* calculate the size of the RAM in this LPM DB */
            for (i=0;i<lpmDbPtr->shadowArray[shadowIdx].numOfLpmMemories;i++)
            {
                totalLpmSize += lpmDbPtr->shadowArray[shadowIdx].lpmRamBlocksSizeArrayPtr[i];
            }

            /* check if the device has enough RAM space to contain the LPM DB configuration */
            lpmRamNumOfLines = PRV_CPSS_DXCH_LPM_RAM_GET_NUM_OF_LINES_MAC(PRV_CPSS_DXCH_PP_MAC(devListArray[devIdx])->fineTuning.tableSize.lpmRam);
            if( (lpmRamNumOfLines - PRV_CPSS_DXCH_PP_MAC(devListArray[devIdx])->moduleCfg.ip.maxNumOfPbrEntries) < totalLpmSize)
            {
                /* not enough memory in the device */
                CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NO_RESOURCE, LOG_ERROR_NO_MSG);
            }

            shadowDevListPtr =
                &lpmDbPtr->shadowArray[shadowIdx].shadowDevList;

            /* ok the device fits, see that it doesn't exist already */
            for (i = 0 ; i < shadowDevListPtr->shareDevNum;i++)
            {
                if (shadowDevListPtr->shareDevs[i] == devListArray[devIdx])
                {
                   /* the device exists, can't re-add it */
                   CPSS_LOG_ERROR_AND_RETURN_MAC(GT_ALREADY_EXIST, LOG_ERROR_NO_MSG);
                }
            }
        }
    }

    /* if we reached here, all the added devices are ok, add them */
    for (shadowIdx = 0 ; shadowIdx < lpmDbPtr->numOfShadowCfg; shadowIdx++)
    {
        numOfShadowDevs = 0;
        for (devIdx = 0 ; devIdx < numOfDevs; devIdx++)
        {
            shadowDevList[numOfShadowDevs] = devListArray[devIdx];
            numOfShadowDevs++;
        }
        /* check if we have added device for the shadow , and if there are add
           them */
        if (numOfShadowDevs > 0)
        {
            addedDevList.shareDevNum = numOfShadowDevs;
            addedDevList.shareDevs   = shadowDevList;

            /* change the working devlist*/
            lpmDbPtr->shadowArray[shadowIdx].workDevListPtr = &addedDevList;

            /* write the lpm data to the added devices */
            retVal = updateHwSearchStc(PRV_CPSS_DXCH_LPM_RAM_SEARCH_MEM_OVERWRITE_E,
                                       lpmDbPtr->protocolBitmap,
                                       &lpmDbPtr->shadowArray[shadowIdx]);
            if (retVal != GT_OK)
            {
                return (retVal);
            }

            /* change back the working devlist*/
            lpmDbPtr->shadowArray[shadowIdx].workDevListPtr =
                &lpmDbPtr->shadowArray[shadowIdx].shadowDevList;

            /* now register them */
            shadowDevListPtr = &lpmDbPtr->shadowArray[shadowIdx].shadowDevList;
            for (i = 0; i < numOfShadowDevs; i++)
            {
                shadowDevListPtr->shareDevs[shadowDevListPtr->shareDevNum] =
                    shadowDevList[i];
                shadowDevListPtr->shareDevNum++;
            }
        }
    }
    return GT_OK;
}

/*******************************************************************************
* prvCpssDxChLpmRamDbDevListRemove
*
* DESCRIPTION:
*       Function Relevant mode : High Level API modes
*       This function removes devices from an existing LPM DB.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2.
*
* INPUTS:
*       lpmDbPtr          - the LPM DB
*       devListArray      - array of device ids to remove from the LPM DB
*       numOfDevs         - the number of device ids in the array
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on illegal parameter
*       GT_BAD_PTR               - on NULL pointer
*       GT_HW_ERROR              - on hardware error
*       GT_FAIL                  - on error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       NONE
*
*
*******************************************************************************/
GT_STATUS prvCpssDxChLpmRamDbDevListRemove
(
    IN PRV_CPSS_DXCH_LPM_RAM_SHADOWS_DB_STC  *lpmDbPtr,
    IN GT_U8                                 devListArray[],
    IN GT_U32                                numOfDevs
)
{
    GT_U32                                      shadowIdx, devIdx, i, j;
    PRV_CPSS_DXCH_LPM_RAM_SHADOW_DEVS_LIST_STC  *shadowDevListPtr;
    GT_BOOL                                     devExists;

    /* first check the devices we got are ok */
    for (devIdx = 0 ; devIdx < numOfDevs; devIdx++)
    {
        PRV_CPSS_DXCH_DEV_CHECK_MAC(devListArray[devIdx]);
    }

    /* first check that the devices exist in the shadows */
    for (devIdx = 0 ; devIdx < numOfDevs; devIdx++)
    {
        for (shadowIdx = 0 ; shadowIdx < lpmDbPtr->numOfShadowCfg; shadowIdx++)
        {
            shadowDevListPtr = &lpmDbPtr->shadowArray[shadowIdx].shadowDevList;

            devExists = GT_FALSE;
            for (i = 0 ; i < shadowDevListPtr->shareDevNum;i++)
            {
                if (shadowDevListPtr->shareDevs[i] == devListArray[devIdx])
                {
                    /* found it */
                    devExists = GT_TRUE;
                    break;
                }
            }
            if (devExists == GT_FALSE)
            {
                CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_FOUND, LOG_ERROR_NO_MSG);
            }
        }
    }

    /* now go and remove them from their shadow */
    for (devIdx = 0 ; devIdx < numOfDevs; devIdx++)
    {
        for (shadowIdx = 0 ; shadowIdx < lpmDbPtr->numOfShadowCfg; shadowIdx++)
        {
            shadowDevListPtr = &lpmDbPtr->shadowArray[shadowIdx].shadowDevList;

            /* ok the device fits, see that it doesn't exist already */
            for (i = 0 ; i < shadowDevListPtr->shareDevNum;i++)
            {
                if (shadowDevListPtr->shareDevs[i] == devListArray[devIdx])
                {
                    /* found it - remove it*/
                    for (j = i ; j < shadowDevListPtr->shareDevNum - 1; j++)
                    {
                        shadowDevListPtr->shareDevs[j] =
                            shadowDevListPtr->shareDevs[j+1];
                    }
                    shadowDevListPtr->shareDevNum--;
                }
            }
        }
    }
    return GT_OK;
}

/*******************************************************************************
* prvCpssDxChLpmRamDbDevListGet
*
* DESCRIPTION:
*       Function Relevant mode : High Level API modes
*       This function retrieves the list of devices in an existing LPM DB.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2.
*
* INPUTS:
*       lpmDbPtr          - the LPM DB
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
GT_STATUS prvCpssDxChLpmRamDbDevListGet
(
    IN    PRV_CPSS_DXCH_LPM_RAM_SHADOWS_DB_STC  *lpmDbPtr,
    INOUT GT_U32                                *numOfDevsPtr,
    OUT   GT_U8                                 devListArray[]
)
{
    GT_U32 i;
    PRV_CPSS_DXCH_LPM_RAM_SHADOW_DEVS_LIST_STC *shadowDevListPtr;

    shadowDevListPtr = &lpmDbPtr->shadowArray[0].shadowDevList;

    if (*numOfDevsPtr < shadowDevListPtr->shareDevNum)
    {
        *numOfDevsPtr = shadowDevListPtr->shareDevNum;
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_SIZE, LOG_ERROR_NO_MSG);
    }

    for (i = 0 ; i < shadowDevListPtr->shareDevNum ; i++)
    {
        devListArray[i] = shadowDevListPtr->shareDevs[i];
    }
    *numOfDevsPtr = shadowDevListPtr->shareDevNum;

    return GT_OK;
}



/*******************************************************************************
* removeSupportedProtocolsFromVirtualRouter
*
* DESCRIPTION:
*       This function removes all supported protocols from virtual router for specific shadow.
*
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2.
*
* INPUTS:
*       vrId                - The virtual's router ID.
*       shadowPtr         - the shadow to work on.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK on success, or
*       GT_OUT_OF_CPU_MEM - if failed to allocate CPU memory, or
*       GT_OUT_OF_PP_MEM - if failed to allocate PP memory.
*       GT_BAD_STATE - if the existing VR is not empty.
*
* COMMENTS:
*       The VR must be empty from prefixes in order to be Removed!
*
*******************************************************************************/
static GT_STATUS removeSupportedProtocolsFromVirtualRouter
(
    IN GT_U32                                   vrId,
    IN PRV_CPSS_DXCH_LPM_RAM_SHADOW_STC         *shadowPtr
)
{
    GT_STATUS                                 retVal = GT_OK;
    PRV_CPSS_DXCH_LPM_PROTOCOL_STACK_ENT      protStk;
    GT_UINTPTR                                handle;

    for (protStk = PRV_CPSS_DXCH_LPM_PROTOCOL_IPV4_E; protStk < PRV_CPSS_DXCH_LPM_PROTOCOL_LAST_E; protStk++)
    {
        if (shadowPtr->vrRootBucketArray[vrId].rootBucket[protStk] == NULL)
        {
            continue;
        }
        handle = shadowPtr->vrRootBucketArray[vrId].rootBucket[protStk]->hwBucketOffsetHandle;
        prvCpssDmmFree(handle);
        retVal = prvCpssDxChLpmRamMngRootBucketDelete(shadowPtr, vrId, protStk);
        if (retVal != GT_OK)
        {
            return retVal;
        }
        shadowPtr->vrRootBucketArray[vrId].rootBucket[protStk] = NULL;
    }
    return retVal;
}


/*******************************************************************************
* lpmVirtualRouterSet
*
* DESCRIPTION:
*       This function sets a virtual router in system for specific shadow.
*
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2.
*
* INPUTS:
*       vrId                    - The virtual's router ID.
*       defIpv4UcNextHopPointer - the pointer info of the default IPv4 UC next
*                                 hop for this virtual router.
*       defIpv6UcNextHopPointer - the pointer info of the default IPv6 UC next
*                                 hop for this virtual router.
*       defFcoeNextHopPointer   - the pointer info of the default FCoE next
*                                 hop for this virtual router.
*       defIpv4McRoutePointer   - the pointer info of the default IPv4 MC route
*                                 for this virtual router.
*       defIpv6McRoutePointer   - the pointer info of the default IPv6 MC route
*                                 for this virtual router.
*       protocolBitmap          - types of protocol stack used in this virtual router.
*       ucSupportArr            - Boolean array stating UC support for every protocol stack
*       mcSupportArr            - Boolean array stating MC support for every protocol stack
*       shadowPtr               - the shadow to work on.
*       updateHw                - GT_TRUE : update the VR in the HW
*                                 GT_FALSE : do not update the VR in the HW
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK               - on success
*       GT_OUT_OF_CPU_MEM   - if failed to allocate CPU memory, or
*       GT_OUT_OF_PP_MEM    - if failed to allocate PP memory.
*       GT_ALREADY_EXIST    - if the vr id is already used
*
*******************************************************************************/
static GT_STATUS lpmVirtualRouterSet
(
    IN GT_U32                                       vrId,
    IN PRV_CPSS_DXCH_LPM_ROUTE_ENTRY_POINTER_STC    defIpv4UcNextHopPointer,
    IN PRV_CPSS_DXCH_LPM_ROUTE_ENTRY_POINTER_STC    defIpv6UcNextHopPointer,
    IN PRV_CPSS_DXCH_LPM_ROUTE_ENTRY_POINTER_STC    defFcoeNextHopPointer,
    IN PRV_CPSS_DXCH_LPM_ROUTE_ENTRY_POINTER_STC    defIpv4McRoutePointer,
    IN PRV_CPSS_DXCH_LPM_ROUTE_ENTRY_POINTER_STC    defIpv6McRoutePointer,
    IN PRV_CPSS_DXCH_LPM_PROTOCOL_BMP               protocolBitmap,
    IN GT_BOOL                                      ucSupportArr[PRV_CPSS_DXCH_LPM_RAM_NUM_OF_UC_PROTOCOLS_CNS],
    IN GT_BOOL                                      mcSupportArr[PRV_CPSS_DXCH_LPM_RAM_NUM_OF_MC_PROTOCOLS_CNS],
    IN PRV_CPSS_DXCH_LPM_RAM_SHADOW_STC             *shadowPtr,
    IN GT_BOOL                                      updateHw
)
{
    GT_STATUS                                       retVal;
    PRV_CPSS_DXCH_LPM_PROTOCOL_STACK_ENT            protStk;
    PRV_CPSS_DXCH_LPM_ROUTE_ENTRY_POINTER_STC       *defUcNextHopEntry;
    PRV_CPSS_DXCH_LPM_ROUTE_ENTRY_POINTER_STC       *defReservedNextHopEntry;
    PRV_CPSS_DXCH_LPM_ROUTE_ENTRY_POINTER_STC       *defMcNextHopEntry;
    PRV_CPSS_DXCH_LPM_ROUTE_ENTRY_POINTER_STC       defUcNextHopPointer = {0};
    PRV_CPSS_DXCH_LPM_ROUTE_ENTRY_POINTER_STC       defMcNextHopPointer = {0};
    PRV_CPSS_DXCH_LPM_ROUTE_ENTRY_POINTER_STC       defReservedNextHopPointer = {0};

    /* make compiler silent */
    defUcNextHopPointer.routeEntryBaseMemAddr = defUcNextHopPointer.routeEntryMethod = 0;

    if (shadowPtr->vrRootBucketArray[vrId].valid == GT_TRUE)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_ALREADY_EXIST, LOG_ERROR_NO_MSG);
    }

    shadowPtr->vrRootBucketArray[vrId].valid = GT_TRUE;

    for (protStk = PRV_CPSS_DXCH_LPM_PROTOCOL_IPV4_E;
         protStk < PRV_CPSS_DXCH_LPM_PROTOCOL_LAST_E;
         protStk++)
    {
        defUcNextHopEntry = NULL;
        defReservedNextHopEntry = NULL;
        defMcNextHopEntry = NULL;

        if (isProtocolSetInBitmap(protocolBitmap, protStk) == GT_FALSE)
        {
            if (protStk < PRV_CPSS_DXCH_LPM_RAM_NUM_OF_UC_PROTOCOLS_CNS)
            {
                shadowPtr->vrRootBucketArray[vrId].isUnicastSupported[protStk] = GT_FALSE;
            }
            if (protStk < PRV_CPSS_DXCH_LPM_RAM_NUM_OF_MC_PROTOCOLS_CNS)
            {
                shadowPtr->vrRootBucketArray[vrId].isMulticastSupported[protStk] = GT_FALSE;
            }
            continue;
        }

        if (protStk <= PRV_CPSS_DXCH_LPM_PROTOCOL_IPV6_E)
        {
            if ((ucSupportArr[protStk] == GT_FALSE) && (mcSupportArr[protStk] == GT_FALSE))
            {
                CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
            }
        }
        else
        {
            if (ucSupportArr[protStk] == GT_FALSE)
            {
                CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
            }
        }

        /* init UC structures */
        if (ucSupportArr[protStk] == GT_TRUE)
        {
            shadowPtr->vrRootBucketArray[vrId].isUnicastSupported[protStk] = GT_TRUE;
            if (protStk == PRV_CPSS_DXCH_LPM_PROTOCOL_IPV4_E)
            {
                defUcNextHopPointer = defIpv4UcNextHopPointer;
                defReservedNextHopPointer = defIpv4UcNextHopPointer;
            }
            else if (protStk == PRV_CPSS_DXCH_LPM_PROTOCOL_IPV6_E)
            {
                defUcNextHopPointer = defIpv6UcNextHopPointer;
            }
            else    /* PRV_CPSS_DXCH_LPM_PROTOCOL_FCOE_E */
            {
                defUcNextHopPointer = defFcoeNextHopPointer;
            }

            /* allocate space for the new default */
            defUcNextHopEntry = cpssOsLpmMalloc(sizeof(PRV_CPSS_DXCH_LPM_ROUTE_ENTRY_POINTER_STC));
            if (defUcNextHopEntry == NULL)
            {
                CPSS_LOG_ERROR_AND_RETURN_MAC(GT_OUT_OF_CPU_MEM, LOG_ERROR_NO_MSG);
            }
            cpssOsMemCpy(defUcNextHopEntry, &defUcNextHopPointer,
                         sizeof(PRV_CPSS_DXCH_LPM_ROUTE_ENTRY_POINTER_STC));
            if (protStk == PRV_CPSS_DXCH_LPM_PROTOCOL_IPV4_E)
            {
                defReservedNextHopEntry = cpssOsLpmMalloc(sizeof(PRV_CPSS_DXCH_LPM_ROUTE_ENTRY_POINTER_STC));
                if (defReservedNextHopEntry == NULL)
                {
                    CPSS_LOG_ERROR_AND_RETURN_MAC(GT_OUT_OF_CPU_MEM, LOG_ERROR_NO_MSG);
                }
                cpssOsMemCpy(defReservedNextHopEntry, &defReservedNextHopPointer,
                             sizeof(PRV_CPSS_DXCH_LPM_ROUTE_ENTRY_POINTER_STC));
            }

        }
        else
        {
            shadowPtr->vrRootBucketArray[vrId].isUnicastSupported[protStk] = GT_FALSE;
        }

        /* now init mc stuctures */
        if (protStk <= PRV_CPSS_DXCH_LPM_PROTOCOL_IPV6_E)
        {
            if (mcSupportArr[protStk] == GT_TRUE)
            {
                shadowPtr->vrRootBucketArray[vrId].isMulticastSupported[protStk] = GT_TRUE;
                if (protStk == PRV_CPSS_DXCH_LPM_PROTOCOL_IPV4_E)
                {
                    defMcNextHopPointer = defIpv4McRoutePointer;
                }
                else /* PRV_CPSS_DXCH_LPM_PROTOCOL_IPV6_E */
                {
                    defMcNextHopPointer = defIpv6McRoutePointer;
                }

                /* In the following cases we need to create a new nexthop pointer
                   info structure:
                   1. UC is not supported for this protocol so we didn't create it yet
                   2. UC is supported but the defaults of the UC and MC are different */
                if ((ucSupportArr[protStk] == GT_FALSE) ||
                    (defUcNextHopPointer.routeEntryBaseMemAddr != defMcNextHopPointer.routeEntryBaseMemAddr) ||
                    (defUcNextHopPointer.routeEntryMethod != defMcNextHopPointer.routeEntryMethod))
                {
                    defMcNextHopEntry = cpssOsLpmMalloc(sizeof(PRV_CPSS_DXCH_LPM_ROUTE_ENTRY_POINTER_STC));
                    if (defMcNextHopEntry == NULL)
                    {
                        if (ucSupportArr[protStk] == GT_TRUE)
                        {
                             cpssOsLpmFree(defUcNextHopEntry);
                        }
                        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_OUT_OF_CPU_MEM, LOG_ERROR_NO_MSG);
                    }

                    cpssOsMemCpy(defMcNextHopEntry, &defMcNextHopPointer,
                                 sizeof(PRV_CPSS_DXCH_LPM_ROUTE_ENTRY_POINTER_STC));
                }
                else
                {
                    /* both UC and MC are supported, with the same default NH */
                    defMcNextHopEntry = defUcNextHopEntry;
                }
            }
            else    /* mcSupportArr[protStk] == GT_FALSE */
            {
                shadowPtr->vrRootBucketArray[vrId].isMulticastSupported[protStk] = GT_FALSE;
            }
        }

        /* Create the root bucket */
        retVal = prvCpssDxChLpmRamMngRootBucketCreate(shadowPtr, vrId, protStk,
                                                      defUcNextHopEntry,
                                                      defReservedNextHopEntry,
                                                      defMcNextHopEntry,
                                                      updateHw);
        if (retVal != GT_OK)
        {
            if (ucSupportArr[protStk] == GT_TRUE)
            {
                cpssOsLpmFree(defUcNextHopEntry);
            }
            if ((protStk <= PRV_CPSS_DXCH_LPM_PROTOCOL_IPV6_E) && (mcSupportArr[protStk] == GT_TRUE))
            {
                if ((ucSupportArr[protStk] == GT_FALSE) ||
                    (defUcNextHopPointer.routeEntryBaseMemAddr != defMcNextHopPointer.routeEntryBaseMemAddr) ||
                    (defUcNextHopPointer.routeEntryMethod != defMcNextHopPointer.routeEntryMethod))
                {
                    cpssOsLpmFree(defMcNextHopEntry);
                }
            }
            return retVal;
        }

        /* Don't touch the hw in a HSU process */
        if(updateHw == GT_TRUE)
        {
            /* write the mc and uc vr router table */
            retVal = prvCpssDxChLpmRamMngVrfEntryUpdate(vrId, protStk, shadowPtr);

            if (retVal != GT_OK)
            {
                if (ucSupportArr[protStk] == GT_TRUE)
                {
                    cpssOsLpmFree(defUcNextHopEntry);
                }
                if (mcSupportArr[protStk] == GT_TRUE)
                {
                    if ((ucSupportArr[protStk] == GT_FALSE) ||
                        (defUcNextHopPointer.routeEntryBaseMemAddr != defMcNextHopPointer.routeEntryBaseMemAddr) ||
                        (defUcNextHopPointer.routeEntryMethod != defMcNextHopPointer.routeEntryMethod))
                    {
                        cpssOsLpmFree(defMcNextHopEntry);
                    }
                }
                return retVal;
            }
        }
    }

    return GT_OK;
}

/*******************************************************************************
* lpmVirtualRouterRemove
*
* DESCRIPTION:
*       This function removes a virtual router in system for specific shadow.
*
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2.
*
* INPUTS:
*       vrId                - The virtual's router ID.
*       shadowPtr         - the shadow to work on.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK on success, or
*       GT_OUT_OF_CPU_MEM - if failed to allocate CPU memory, or
*       GT_OUT_OF_PP_MEM - if failed to allocate PP memory.
*       GT_BAD_STATE - if the existing VR is not empty.
*
* COMMENTS:
*       The VR must be empty from prefixes in order to be Removed!
*
*******************************************************************************/
static GT_STATUS lpmVirtualRouterRemove
(
    IN GT_U32                                   vrId,
    IN PRV_CPSS_DXCH_LPM_RAM_SHADOW_STC         *shadowPtr
)
{
    GT_STATUS                                 retVal = GT_OK;

    if (shadowPtr->vrRootBucketArray[vrId].valid == GT_FALSE)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_FOUND, LOG_ERROR_NO_MSG);
    }
    /* this is an existing Vr - delete the allocated structures */
    retVal = removeSupportedProtocolsFromVirtualRouter(vrId,shadowPtr);
    if (retVal != GT_OK)
    {
        return retVal;
    }

    shadowPtr->vrRootBucketArray[vrId].valid = GT_FALSE;

    return retVal;
}



/*******************************************************************************
* prvCpssDxChLpmRamVirtualRouterAdd
*
* DESCRIPTION:
*       Function Relevant mode : High Level API modes
*       This function adds a virtual router for specific LPM DB.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2.
*
* INPUTS:
*       lpmDbPtr          - the LPM DB
*       vrId              - The virtual's router ID.
*       vrConfigPtr       - Configuration of the virtual router
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success, or
*       GT_BAD_PTR               - on NULL pointer
*       GT_OUT_OF_CPU_MEM        - if failed to allocate CPU memory, or
*       GT_ALREADY_EXIST         - if the vr id is already used
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS prvCpssDxChLpmRamVirtualRouterAdd
(
    IN PRV_CPSS_DXCH_LPM_RAM_SHADOWS_DB_STC  *lpmDbPtr,
    IN GT_U32                                vrId,
    IN PRV_CPSS_DXCH_LPM_RAM_VR_CONFIG_STC   *vrConfigPtr
)
{
    GT_STATUS                              retVal = GT_OK;
    PRV_CPSS_DXCH_LPM_RAM_SHADOW_STC       *shadowPtr;
    GT_U32                                 shadowIdx;

    PRV_CPSS_DXCH_LPM_PROTOCOL_BMP         protocolBitmap = 0;
    PRV_CPSS_DXCH_LPM_PROTOCOL_BMP         protocolDifference;
    GT_BOOL                                vrUcSupport[PRV_CPSS_DXCH_LPM_RAM_NUM_OF_UC_PROTOCOLS_CNS];
    GT_BOOL                                vrMcSupport[PRV_CPSS_DXCH_LPM_RAM_NUM_OF_MC_PROTOCOLS_CNS];

    if (vrConfigPtr->supportUcIpv4 == GT_TRUE)
    {
        retVal = prvCpssDxChLpmRouteEntryPointerCheck(&vrConfigPtr->defaultUcIpv4RouteEntry);
        if (retVal != GT_OK)
            return retVal;
    }
    if (vrConfigPtr->supportMcIpv4 == GT_TRUE)
    {
        retVal = prvCpssDxChLpmRouteEntryPointerCheck(&vrConfigPtr->defaultMcIpv4RouteEntry);
        if (retVal != GT_OK)
            return retVal;
    }
    if (vrConfigPtr->supportUcIpv6 == GT_TRUE)
    {
        retVal = prvCpssDxChLpmRouteEntryPointerCheck(&vrConfigPtr->defaultUcIpv6RouteEntry);
        if (retVal != GT_OK)
            return retVal;
    }
    if (vrConfigPtr->supportMcIpv6 == GT_TRUE)
    {
        retVal = prvCpssDxChLpmRouteEntryPointerCheck(&vrConfigPtr->defaultMcIpv6RouteEntry);
        if (retVal != GT_OK)
            return retVal;
    }
    if (vrConfigPtr->supportFcoe == GT_TRUE)
    {
        retVal = prvCpssDxChLpmRouteEntryPointerCheck(&vrConfigPtr->defaultFcoeForwardingEntry);
        if (retVal != GT_OK)
            return retVal;
    }

    vrUcSupport[PRV_CPSS_DXCH_LPM_PROTOCOL_IPV4_E] = (vrConfigPtr->supportUcIpv4) ? GT_TRUE : GT_FALSE;
    vrUcSupport[PRV_CPSS_DXCH_LPM_PROTOCOL_IPV6_E] = (vrConfigPtr->supportUcIpv6) ? GT_TRUE : GT_FALSE;
    vrUcSupport[PRV_CPSS_DXCH_LPM_PROTOCOL_FCOE_E] = (vrConfigPtr->supportFcoe)   ? GT_TRUE : GT_FALSE;
    vrMcSupport[PRV_CPSS_DXCH_LPM_PROTOCOL_IPV4_E] = (vrConfigPtr->supportMcIpv4) ? GT_TRUE : GT_FALSE;
    vrMcSupport[PRV_CPSS_DXCH_LPM_PROTOCOL_IPV6_E] = (vrConfigPtr->supportMcIpv6) ? GT_TRUE : GT_FALSE;

    /* determine the supported protocols */
    if ((vrConfigPtr->supportUcIpv4 == GT_TRUE) || (vrConfigPtr->supportMcIpv4 == GT_TRUE))
    {
        protocolBitmap |= PRV_CPSS_DXCH_LPM_PROTOCOL_IPV4_BIT_E;
    }
    if ((vrConfigPtr->supportUcIpv6 == GT_TRUE) || (vrConfigPtr->supportMcIpv6 == GT_TRUE))
    {
        protocolBitmap |= PRV_CPSS_DXCH_LPM_PROTOCOL_IPV6_BIT_E;
    }
    if (vrConfigPtr->supportFcoe == GT_TRUE)
    {
        protocolBitmap |= PRV_CPSS_DXCH_LPM_PROTOCOL_FCOE_BIT_E;
    }

    /* if initialization has not been done for the requested protocol stack -
    return error */
    protocolDifference = protocolBitmap ^ lpmDbPtr->protocolBitmap;
    if (protocolDifference)
    {
        if (protocolDifference & protocolBitmap)
        {
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_INITIALIZED, LOG_ERROR_NO_MSG);
        }
    }

    for (shadowIdx = 0 ; shadowIdx < lpmDbPtr->numOfShadowCfg; shadowIdx++)
    {
        shadowPtr = &lpmDbPtr->shadowArray[shadowIdx];

        if (vrId >= shadowPtr->vrfTblSize)
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);

        retVal = lpmVirtualRouterSet(vrId,
                                     vrConfigPtr->defaultUcIpv4RouteEntry,
                                     vrConfigPtr->defaultUcIpv6RouteEntry,
                                     vrConfigPtr->defaultFcoeForwardingEntry,
                                     vrConfigPtr->defaultMcIpv4RouteEntry,
                                     vrConfigPtr->defaultMcIpv6RouteEntry,
                                     protocolBitmap,
                                     vrUcSupport,
                                     vrMcSupport,
                                     shadowPtr,
                                     GT_TRUE);
        if (retVal != GT_OK)
        {
            if (retVal != GT_ALREADY_EXIST)
            {
                /* free all allocated resources  */
                removeSupportedProtocolsFromVirtualRouter(vrId,shadowPtr);
            }
            return(retVal);
        }
    }

    return retVal;
}

/*******************************************************************************
* prvCpssDxChLpmRamVirtualRouterDel
*
* DESCRIPTION:
*       Function Relevant mode : High Level API modes
*       This function deletes a virtual router for specific LPM DB.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2.
*
* INPUTS:
*       lpmDbPtr          - the LPM DB
*       vrId              - The virtual's router ID.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success, or
*       GT_NOT_FOUND             - if the LPM DB id or vr id does not found
*       GT_OUT_OF_CPU_MEM        - if failed to allocate CPU memory, or
*       GT_BAD_STATE             - if the existing VR is not empty.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       All prefixes must be previously deleted.
*
*******************************************************************************/
GT_STATUS prvCpssDxChLpmRamVirtualRouterDel
(
    IN PRV_CPSS_DXCH_LPM_RAM_SHADOWS_DB_STC  *lpmDbPtr,
    IN GT_U32                                vrId
)
{
    GT_STATUS                           retVal = GT_OK;
    PRV_CPSS_DXCH_LPM_RAM_SHADOW_STC    *shadowPtr;
    GT_U32                              shadowIdx;

    for (shadowIdx = 0 ; shadowIdx < lpmDbPtr->numOfShadowCfg; shadowIdx++)
    {
        shadowPtr = &lpmDbPtr->shadowArray[shadowIdx];

        /* delete all the IPv4 unicast prefixes for this virtual router */
        if (shadowPtr->vrRootBucketArray[vrId].isUnicastSupported[PRV_CPSS_DXCH_LPM_PROTOCOL_IPV4_E])
        {
                retVal = prvCpssDxChLpmRamIpv4UcPrefixesFlush(lpmDbPtr,vrId);
                if (retVal != GT_OK)
                {
                    return (retVal);
                }
        }

        /* delete all the IPv4 multicast entries for this virtual router */
        if (shadowPtr->vrRootBucketArray[vrId].isMulticastSupported[PRV_CPSS_DXCH_LPM_PROTOCOL_IPV4_E])
        {
            retVal = prvCpssDxChLpmRamIpv4McEntriesFlush(lpmDbPtr,vrId);
            if (retVal != GT_OK)
            {
                return (retVal);
            }
        }

        /* delete all the IPv6 unicast prefixes for this virtual router */
        if (shadowPtr->vrRootBucketArray[vrId].isUnicastSupported[PRV_CPSS_DXCH_LPM_PROTOCOL_IPV6_E])
        {
            retVal = prvCpssDxChLpmRamIpv6UcPrefixesFlush(lpmDbPtr,vrId);
            if (retVal != GT_OK)
            {
                return (retVal);
            }
        }

        /* delete all the IPv6 multicast entries for this virtual router */
        if (shadowPtr->vrRootBucketArray[vrId].isMulticastSupported[PRV_CPSS_DXCH_LPM_PROTOCOL_IPV6_E])
        {
            retVal = prvCpssDxChLpmRamIpv6McEntriesFlush(lpmDbPtr,vrId);
            if (retVal != GT_OK)
            {
                return (retVal);
            }
        }

        /* delete all the FCoE prefixes for this virtual router */
        if (shadowPtr->vrRootBucketArray[vrId].isUnicastSupported[PRV_CPSS_DXCH_LPM_PROTOCOL_FCOE_E])
        {
            retVal = prvCpssDxChLpmRamFcoePrefixesFlush(lpmDbPtr,vrId);
            if (retVal != GT_OK)
            {
                return (retVal);
            }
        }

        retVal = lpmVirtualRouterRemove(vrId, shadowPtr);
        if (retVal != GT_OK)
        {
            return (retVal);
        }
    }

    return retVal;
}

/*******************************************************************************
* prvCpssDxChLpmRamVirtualRouterGet
*
* DESCRIPTION:
*       Function Relevant mode : High Level API modes
*       This function gets configuration of a virtual router for specific LPM DB.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2.
*
* INPUTS:
*       lpmDbPtr          - the LPM DB.
*       vrId              - The virtual's router ID.
*
* OUTPUTS:
*       vrConfigPtr       - Configuration of the virtual router
*
* RETURNS:
*       GT_OK                    - on success, or
*       GT_NOT_FOUND             - if the LPM DB id or vr id does not found
*       GT_BAD_PTR               - on NULL pointer
*       GT_OUT_OF_CPU_MEM        - if failed to allocate CPU memory, or
*       GT_OUT_OF_PP_MEM         - if failed to allocate TCAM memory.
*       GT_BAD_STATE             - if the existing VR is not empty.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       none.
*
*******************************************************************************/
GT_STATUS prvCpssDxChLpmRamVirtualRouterGet
(
    IN  PRV_CPSS_DXCH_LPM_RAM_SHADOWS_DB_STC *lpmDbPtr,
    IN  GT_U32                               vrId,
    OUT PRV_CPSS_DXCH_LPM_RAM_VR_CONFIG_STC  *vrConfigPtr
)
{
    GT_STATUS                                   retVal = GT_OK;
    PRV_CPSS_DXCH_LPM_RAM_SHADOW_STC            *shadowPtr;
    PRV_CPSS_DXCH_LPM_ROUTE_ENTRY_POINTER_STC   *defRoutePointerPtr = NULL;
    PRV_CPSS_DXCH_LPM_ROUTE_ENTRY_POINTER_STC   routeEntry;
    GT_IPV6ADDR                                 ipv6Addr;
    GT_IPADDR                                   ipv4Addr;
    GT_FCID                                     fcid;

    shadowPtr = &lpmDbPtr->shadowArray[0];

    if (shadowPtr->vrRootBucketArray[vrId].valid == GT_FALSE)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_FOUND, LOG_ERROR_NO_MSG);
    }

    /* IPv4 MC */
    if (shadowPtr->vrRootBucketArray[vrId].isMulticastSupported[PRV_CPSS_DXCH_LPM_PROTOCOL_IPV4_E] == GT_TRUE)
    {
        retVal = prvCpssDxChLpmRamMcDefRouteGet(vrId ,&defRoutePointerPtr,
                                                shadowPtr ,PRV_CPSS_DXCH_LPM_PROTOCOL_IPV4_E);
        if ((retVal != GT_OK) && (retVal != GT_NOT_FOUND) && (retVal != GT_NOT_INITIALIZED))
        {
            return retVal;
        }

        if ((defRoutePointerPtr == NULL) || (retVal == GT_NOT_FOUND) || (retVal == GT_NOT_INITIALIZED))
            vrConfigPtr->supportMcIpv4 = GT_FALSE;
        else
        {
            vrConfigPtr->defaultMcIpv4RouteEntry = *defRoutePointerPtr;
            vrConfigPtr->supportMcIpv4 = GT_TRUE;
        }
    }
    else
    {
        vrConfigPtr->supportMcIpv4 = GT_FALSE;
    }

    /* IPv4 UC */
    if (shadowPtr->vrRootBucketArray[vrId].isUnicastSupported[PRV_CPSS_DXCH_LPM_PROTOCOL_IPV4_E] == GT_TRUE)
    {
        cpssOsMemSet(&ipv4Addr, 0, sizeof(ipv4Addr));
        retVal = prvCpssDxChLpmRamIpv4UcPrefixSearch(lpmDbPtr,vrId,ipv4Addr,0,&routeEntry);
        if ((retVal != GT_OK) && (retVal != GT_NOT_FOUND) && (retVal != GT_NOT_INITIALIZED))
        {
            return retVal;
        }

        if ((retVal == GT_NOT_FOUND)  || (retVal == GT_NOT_INITIALIZED))
            vrConfigPtr->supportUcIpv4 = GT_FALSE;
        else
        {
            vrConfigPtr->defaultUcIpv4RouteEntry = routeEntry;
            vrConfigPtr->supportUcIpv4 = GT_TRUE;
        }
    }
    else
    {
        vrConfigPtr->supportUcIpv4 = GT_FALSE;
    }

    /* IPv6 MC */
    if (shadowPtr->vrRootBucketArray[vrId].isMulticastSupported[PRV_CPSS_DXCH_LPM_PROTOCOL_IPV6_E] == GT_TRUE)
    {
        retVal = prvCpssDxChLpmRamMcDefRouteGet(vrId, &defRoutePointerPtr,
                                                shadowPtr, PRV_CPSS_DXCH_LPM_PROTOCOL_IPV6_E);
        if ((retVal != GT_OK) && (retVal != GT_NOT_FOUND) && (retVal != GT_NOT_INITIALIZED))
        {
            return retVal;
        }

        if ((defRoutePointerPtr == NULL) || (retVal == GT_NOT_FOUND) || (retVal == GT_NOT_INITIALIZED))
            vrConfigPtr->supportMcIpv6 = GT_FALSE;
        else
        {
            vrConfigPtr->defaultMcIpv6RouteEntry = *defRoutePointerPtr;
            vrConfigPtr->supportMcIpv6 = GT_TRUE;
        }
    }
    else
    {
        vrConfigPtr->supportMcIpv6 = GT_FALSE;
    }

    /* IPv6 UC */
    if (shadowPtr->vrRootBucketArray[vrId].isUnicastSupported[PRV_CPSS_DXCH_LPM_PROTOCOL_IPV6_E] == GT_TRUE)
    {
        cpssOsMemSet(&ipv6Addr, 0, sizeof(ipv6Addr));
        retVal = prvCpssDxChLpmRamIpv6UcPrefixSearch(lpmDbPtr,vrId,ipv6Addr,0,&routeEntry);
        if ((retVal != GT_OK) && (retVal != GT_NOT_FOUND) && (retVal != GT_NOT_INITIALIZED))
        {
            return retVal;
        }

        if ((retVal == GT_NOT_FOUND)  || (retVal == GT_NOT_INITIALIZED))
            vrConfigPtr->supportUcIpv6 = GT_FALSE;
        else
        {
            vrConfigPtr->defaultUcIpv6RouteEntry = routeEntry;
            vrConfigPtr->supportUcIpv6 = GT_TRUE;
        }
    }
    else
    {
        vrConfigPtr->supportUcIpv6 = GT_FALSE;
    }

    /* FCoE */
    if (shadowPtr->vrRootBucketArray[vrId].isUnicastSupported[PRV_CPSS_DXCH_LPM_PROTOCOL_FCOE_E] == GT_TRUE)
    {
        cpssOsMemSet(&fcid, 0, sizeof(fcid));
        retVal = prvCpssDxChLpmRamFcoePrefixSearch(lpmDbPtr,vrId,fcid,0,&routeEntry);
        if ((retVal != GT_OK) && (retVal != GT_NOT_FOUND) && (retVal != GT_NOT_INITIALIZED))
        {
            return retVal;
        }

        if ((retVal == GT_NOT_FOUND)  || (retVal == GT_NOT_INITIALIZED))
            vrConfigPtr->supportFcoe = GT_FALSE;
        else
        {
            vrConfigPtr->defaultFcoeForwardingEntry = routeEntry;
            vrConfigPtr->supportFcoe = GT_TRUE;
        }
    }
    else
    {
        vrConfigPtr->supportFcoe = GT_FALSE;
    }

    return GT_OK;
}

/*******************************************************************************
* prvCpssDxChLpmRamIpv4UcPrefixAdd
*
* DESCRIPTION:
*       Function Relevant mode : High Level API modes
*       This function adds a new IPv4 prefix to a Virtual Router in a
*       specific LPM DB or overrides an existing existing IPv4 prefix.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2.
*
* INPUTS:
*       lpmDbPtr          - The LPM DB
*       vrId              - The virtual router id.
*       ipAddr            - The destination IP address of this prefix.
*       prefixLen         - The number of bits that are actual valid in the ipAddr.
*       nextHopInfoPtr    - (points to) The next hop pointer to set for this prefix.
*       override          - override the existing entry if it already exists
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success, or
*       GT_OUT_OF_RANGE         -  If prefix length is too big, or
*       GT_ERROR                 - If the vrId was not created yet, or
*       GT_ALREADY_EXIST         - prefix already exist when override is GT_FALSE
*       GT_BAD_PTR               - on NULL pointer
*       GT_OUT_OF_CPU_MEM        - If failed to allocate CPU memory, or
*       GT_OUT_OF_PP_MEM         - If failed to allocate PP memory, or
*       GT_FAIL                  - otherwise.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       To change the default prefix for the VR use prefixLen = 0.
*
*******************************************************************************/
GT_STATUS prvCpssDxChLpmRamIpv4UcPrefixAdd
(
    IN PRV_CPSS_DXCH_LPM_RAM_SHADOWS_DB_STC         *lpmDbPtr,
    IN GT_U32                                       vrId,
    IN GT_IPADDR                                    ipAddr,
    IN GT_U32                                       prefixLen,
    IN PRV_CPSS_DXCH_LPM_ROUTE_ENTRY_POINTER_STC    *nextHopInfoPtr,
    IN GT_BOOL                                      override
)
{
    GT_STATUS           retVal = GT_OK;
    PRV_CPSS_DXCH_LPM_RAM_SHADOW_STC *shadowPtr;
    GT_U32 shadowIdx;

    retVal = prvCpssDxChLpmRouteEntryPointerCheck(nextHopInfoPtr);
    if (retVal != GT_OK)
        return retVal;

    /* if initialization has not been done for the requested protocol stack -
    return error */
    if (PRV_CPSS_DXCH_LPM_PROTOCOL_IPV4_STATUS_GET_MAC(lpmDbPtr->protocolBitmap) == GT_FALSE)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_INITIALIZED, LOG_ERROR_NO_MSG);
    }

    for (shadowIdx = 0 ; shadowIdx < lpmDbPtr->numOfShadowCfg; shadowIdx++)
    {
        shadowPtr = &lpmDbPtr->shadowArray[shadowIdx];
        retVal = prvCpssDxChLpmRamUcEntryAdd(vrId,
                                             ipAddr.arIP,
                                             prefixLen,
                                             nextHopInfoPtr,
                                             PRV_CPSS_DXCH_LPM_RAM_TRIE_INSERT_SDW_MEM_HW_MODE_E,
                                             override,
                                              PRV_CPSS_DXCH_LPM_PROTOCOL_IPV4_E,
                                             shadowPtr);
        if (retVal != GT_OK)
        {
            break;
        }
    }
    return retVal;
}

/*******************************************************************************
* prvCpssDxChLpmRamIpv4UcPrefixBulkAdd
*
* DESCRIPTION:
*       Function Relevant mode : High Level API modes
*       Creates a new or override an existing bulk of IPv4 prefixes in a Virtual
*       Router for the specified LPM DB.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2.
*
* INPUTS:
*       lpmDbPtr              - The LPM DB
*       ipv4PrefixArrayLen    - Length of UC prefix array.
*       ipv4PrefixArrayPtr    - The UC prefix array.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - if all prefixes were successfully added
*       GT_OUT_OF_RANGE         -  if prefix length is too big
*       GT_BAD_PTR               - on NULL pointer
*       GT_FAIL                  - if adding one or more prefixes failed; the
*                                  array will contain return status for each prefix
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS prvCpssDxChLpmRamIpv4UcPrefixBulkAdd
(
    IN PRV_CPSS_DXCH_LPM_RAM_SHADOWS_DB_STC *lpmDbPtr,
    IN GT_U32                               ipv4PrefixArrayLen,
    IN CPSS_DXCH_IP_LPM_IPV4_UC_PREFIX_STC  *ipv4PrefixArrayPtr
)
{
    GT_STATUS           retVal = GT_OK,retVal1=GT_OK,retVal2=GT_OK;
    PRV_CPSS_DXCH_LPM_RAM_SHADOW_STC *shadowPtr;
    GT_U32 shadowIdx;
    GT_U32 i;
    PRV_CPSS_DXCH_LPM_ROUTE_ENTRY_INFO_UNT lpmRouteEntry;
    PRV_CPSS_DXCH_LPM_PROTOCOL_BMP     protocolBitmap=0;

    cpssOsMemSet(&lpmRouteEntry,0,sizeof(lpmRouteEntry));

    for (i = 0 ; i < ipv4PrefixArrayLen; i++)
    {
        if (ipv4PrefixArrayPtr[i].prefixLen > 32)
        {
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_OUT_OF_RANGE, LOG_ERROR_NO_MSG);
        }

        if ((ipv4PrefixArrayPtr[i].ipAddr.arIP[0] >= PRV_CPSS_DXCH_LPM_RAM_START_OF_IPV4_MC_ADDRESS_SPACE_CNS) &&
            (ipv4PrefixArrayPtr[i].ipAddr.arIP[0] <= PRV_CPSS_DXCH_LPM_RAM_END_OF_IPV4_MC_ADDRESS_SPACE_CNS))
        {
            /* Multicast range */
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
        }
    }

    /* if initialization has not been done for the requested protocol stack -
    return error */
    if (PRV_CPSS_DXCH_LPM_PROTOCOL_IPV4_STATUS_GET_MAC(lpmDbPtr->protocolBitmap) == GT_FALSE)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_INITIALIZED, LOG_ERROR_NO_MSG);
    }

    for (shadowIdx = 0 ; shadowIdx < lpmDbPtr->numOfShadowCfg; shadowIdx++)
    {
        shadowPtr = &lpmDbPtr->shadowArray[shadowIdx];
        /* first go over the prefixes and update the shadow */
        for (i = 0 ; i < ipv4PrefixArrayLen ; i++)
        {
            prvCpssDxChLpmConvertIpRouteEntryToLpmRouteEntry(shadowPtr->shadowType,
                                                             &(ipv4PrefixArrayPtr[i].nextHopInfo),
                                                             &lpmRouteEntry);
            retVal = prvCpssDxChLpmRouteEntryPointerCheck(&(lpmRouteEntry.routeEntry));
            if (retVal != GT_OK)
            {
                return retVal;
            }
            ipv4PrefixArrayPtr[i].returnStatus =
                prvCpssDxChLpmRamUcEntryAdd(ipv4PrefixArrayPtr[i].vrId,
                                            ipv4PrefixArrayPtr[i].ipAddr.arIP,
                                            ipv4PrefixArrayPtr[i].prefixLen,
                                            &lpmRouteEntry.routeEntry,
                                            PRV_CPSS_DXCH_LPM_RAM_TRIE_INSERT_SDW_MEM_MODE_E,
                                            ipv4PrefixArrayPtr[i].override,
                                            PRV_CPSS_DXCH_LPM_PROTOCOL_IPV4_E,
                                            shadowPtr);

            if (ipv4PrefixArrayPtr[i].returnStatus != GT_OK)
            {
                retVal2 = GT_FAIL;
            }
        }

        prvCpssDxChLpmConvertIpProtocolStackToProtocolBitmap(PRV_CPSS_DXCH_LPM_PROTOCOL_IPV4_E, &protocolBitmap);
        /* now update the HW */
        retVal1 = updateHwSearchStc(PRV_CPSS_DXCH_LPM_RAM_SEARCH_MEM_UPDATE_E,
                                    protocolBitmap,
                                    shadowPtr);
        if (retVal1 != GT_OK)
        {
            return (retVal1);
        }
    }
    return (retVal2);
}

/*******************************************************************************
* prvCpssDxChLpmRamIpv4UcPrefixDel
*
* DESCRIPTION:
*       Function Relevant mode : High Level API modes
*       Deletes an existing IPv4 prefix in a Virtual Router for the specified
*       LPM DB.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2.
*
* INPUTS:
*       lpmDbPtr        - The LPM DB.
*       vrId            - The virtual router id.
*       ipAddr          - The destination IP address of the prefix.
*       prefixLen       - The number of bits that are actual valid in the ipAddr.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success, or
*       GT_OUT_OF_RANGE         -  If prefix length is too big, or
*       GT_ERROR                 - If the vrId was not created yet, or
*       GT_NO_SUCH               - If the given prefix doesn't exitst in the VR, or
*       GT_FAIL                  - otherwise.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       the default prefix (prefixLen = 0) can't be deleted!
*
*******************************************************************************/
GT_STATUS prvCpssDxChLpmRamIpv4UcPrefixDel
(
    IN PRV_CPSS_DXCH_LPM_RAM_SHADOWS_DB_STC  *lpmDbPtr,
    IN GT_U32                                vrId,
    IN GT_IPADDR                             ipAddr,
    IN GT_U32                                prefixLen
)
{
    GT_STATUS           retVal = GT_OK;
    PRV_CPSS_DXCH_LPM_RAM_SHADOW_STC *shadowPtr;
    GT_U32 shadowIdx;

    /* if initialization has not been done for the requested protocol stack -
    return error */
    if (PRV_CPSS_DXCH_LPM_PROTOCOL_IPV4_STATUS_GET_MAC(lpmDbPtr->protocolBitmap) == GT_FALSE)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_INITIALIZED, LOG_ERROR_NO_MSG);
    }

    for (shadowIdx = 0 ; shadowIdx < lpmDbPtr->numOfShadowCfg; shadowIdx++)
    {
        shadowPtr = &lpmDbPtr->shadowArray[shadowIdx];

        if (shadowPtr->vrRootBucketArray[vrId].valid == GT_FALSE)
        {
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_FOUND, LOG_ERROR_NO_MSG);
        }

        retVal = prvCpssDxChLpmRamUcEntryDel(vrId,
                                             ipAddr.arIP,
                                             prefixLen,
                                             GT_TRUE,
                                             PRV_CPSS_DXCH_LPM_PROTOCOL_IPV4_E,
                                             shadowPtr,
                                             GT_FALSE);
        if (retVal != GT_OK)
        {
            break;
        }
    }
    return retVal;
}

/*******************************************************************************
* prvCpssDxChLpmRamIpv4UcPrefixBulkDel
*
* DESCRIPTION:
*       Function Relevant mode : High Level mode
*       Deletes an existing bulk of Ipv4 prefixes in a Virtual Router for the
*       specified LPM DB.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2.
*
* INPUTS:
*       lpmDbPtr              - The LPM DB
*       ipv4PrefixArrayLen    - Length of UC prefix array.
*       ipv4PrefixArrayPtr    - The UC prefix array.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - if all prefixes were successfully deleted
*       GT_OUT_OF_RANGE         -  if prefix length is too big
*       GT_BAD_PTR               - if NULL pointer
*       GT_FAIL                  - if deleting one or more prefixes failed; the
*                                  array will contain return status for each prefix
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS prvCpssDxChLpmRamIpv4UcPrefixBulkDel
(
    IN PRV_CPSS_DXCH_LPM_RAM_SHADOWS_DB_STC *lpmDbPtr,
    IN GT_U32                               ipv4PrefixArrayLen,
    IN CPSS_DXCH_IP_LPM_IPV4_UC_PREFIX_STC  *ipv4PrefixArrayPtr
)
{
    GT_STATUS           retVal = GT_OK,retVal1;
    GT_U32      i;
    PRV_CPSS_DXCH_LPM_RAM_SHADOW_STC *shadowPtr;
    GT_U32 shadowIdx;
    PRV_CPSS_DXCH_LPM_PROTOCOL_BMP     protocolBitmap=0;

    /* if initialization has not been done for the requested protocol stack -
    return error */
    if (PRV_CPSS_DXCH_LPM_PROTOCOL_IPV4_STATUS_GET_MAC(lpmDbPtr->protocolBitmap) == GT_FALSE)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_INITIALIZED, LOG_ERROR_NO_MSG);
    }

    for (shadowIdx = 0 ; shadowIdx < lpmDbPtr->numOfShadowCfg; shadowIdx++)
    {
        shadowPtr = &lpmDbPtr->shadowArray[shadowIdx];
        for (i = 0 ; i < ipv4PrefixArrayLen ; i++)
        {
            if (ipv4PrefixArrayPtr[i].prefixLen > 32)
            {
                CPSS_LOG_ERROR_AND_RETURN_MAC(GT_OUT_OF_RANGE, LOG_ERROR_NO_MSG);
            }

            if ((ipv4PrefixArrayPtr[i].ipAddr.arIP[0] >= PRV_CPSS_DXCH_LPM_RAM_START_OF_IPV4_MC_ADDRESS_SPACE_CNS) &&
                (ipv4PrefixArrayPtr[i].ipAddr.arIP[0] <= PRV_CPSS_DXCH_LPM_RAM_END_OF_IPV4_MC_ADDRESS_SPACE_CNS))
            {
                /* Multicast range */
                CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
            }

            ipv4PrefixArrayPtr[i].returnStatus =
                prvCpssDxChLpmRamUcEntryDel(ipv4PrefixArrayPtr[i].vrId,
                                            ipv4PrefixArrayPtr[i].ipAddr.arIP,
                                            ipv4PrefixArrayPtr[i].prefixLen,
                                            GT_FALSE,
                                            PRV_CPSS_DXCH_LPM_PROTOCOL_IPV4_E,
                                            shadowPtr,
                                            GT_FALSE);

            if (ipv4PrefixArrayPtr[i].returnStatus != GT_OK)
            {
                retVal = GT_FAIL;
            }
        }

        prvCpssDxChLpmConvertIpProtocolStackToProtocolBitmap(PRV_CPSS_DXCH_LPM_PROTOCOL_IPV4_E, &protocolBitmap);
        /* now update the HW */
        retVal1 = updateHwSearchStc(PRV_CPSS_DXCH_LPM_RAM_SEARCH_MEM_UPDATE_AND_ALLOC_E,
                                    protocolBitmap,
                                    shadowPtr);
        if (retVal1 != GT_OK)
        {
            return (retVal1);
        }
    }
    return (retVal);
}

/*******************************************************************************
* prvCpssDxChLpmRamIpv4UcPrefixesFlush
*
* DESCRIPTION:
*       Function Relevant mode : High Level API modes
*       Flushes the unicast IPv4 Routing table and stays with the default prefix
*       only for a specific LPM DB.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2.
*
* INPUTS:
*       lpmDbPtr      - The LPM DB
*       vrId          - The virtual router identifier.
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
GT_STATUS prvCpssDxChLpmRamIpv4UcPrefixesFlush
(
    IN PRV_CPSS_DXCH_LPM_RAM_SHADOWS_DB_STC  *lpmDbPtr,
    IN GT_U32                                vrId
)
{
    GT_STATUS           retVal = GT_OK;
    PRV_CPSS_DXCH_LPM_RAM_SHADOW_STC *shadowPtr;
    GT_U32 shadowIdx;

    /* if initialization has not been done for the requested protocol stack -
    return error */
    if (PRV_CPSS_DXCH_LPM_PROTOCOL_IPV4_STATUS_GET_MAC(lpmDbPtr->protocolBitmap) == GT_FALSE)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_INITIALIZED, LOG_ERROR_NO_MSG);
    }

    for (shadowIdx = 0 ; shadowIdx < lpmDbPtr->numOfShadowCfg; shadowIdx++)
    {
        shadowPtr = &lpmDbPtr->shadowArray[shadowIdx];
        if (shadowPtr->vrRootBucketArray[vrId].valid == GT_FALSE)
        {
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_FOUND, LOG_ERROR_NO_MSG);
        }
        retVal = prvCpssDxChLpmRamUcEntriesFlush(vrId,
                                                 PRV_CPSS_DXCH_LPM_PROTOCOL_IPV4_E,
                                                 shadowPtr);
        if (retVal != GT_OK)
        {
            break;
        }
    }
    return (retVal);
}

/*******************************************************************************
* prvCpssDxChLpmRamIpv4UcPrefixSearch
*
* DESCRIPTION:
*       Function Relevant mode : High Level API modes
*       This function searches for a given ip-uc address, and returns the next
*       hop pointer information bound to it if found.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2.
*
* INPUTS:
*       lpmDbPtr          - The LPM DB
*       vrId              - The virtual router id.
*       ipAddr            - The destination IP address to look for.
*       prefixLen         - The number of bits that are actual valid in the
*                           ipAddr.
*
* OUTPUTS:
*       nextHopInfoPtr    - If found this is The next hop pointer to for this
*                           prefix.
*
* RETURNS:
*       GT_OK                    - if the required entry was found, or
*       GT_OUT_OF_RANGE         -  if prefix length is too big, or
*       GT_BAD_PTR               - on NULL pointer
*       GT_NOT_FOUND             - if the given ip prefix was not found.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS prvCpssDxChLpmRamIpv4UcPrefixSearch
(
    IN  PRV_CPSS_DXCH_LPM_RAM_SHADOWS_DB_STC        *lpmDbPtr,
    IN  GT_U32                                      vrId,
    IN  GT_IPADDR                                   ipAddr,
    IN  GT_U32                                      prefixLen,
    OUT PRV_CPSS_DXCH_LPM_ROUTE_ENTRY_POINTER_STC   *nextHopInfoPtr
)
{
    GT_STATUS           retVal = GT_OK;
    PRV_CPSS_DXCH_LPM_RAM_SHADOW_STC *shadowPtr;

    /* if initialization has not been done for the requested protocol stack -
    return error */
    if (PRV_CPSS_DXCH_LPM_PROTOCOL_IPV4_STATUS_GET_MAC(lpmDbPtr->protocolBitmap) == GT_FALSE)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_INITIALIZED, LOG_ERROR_NO_MSG);
    }

    /* the search can be done on one shadow */
    shadowPtr = &lpmDbPtr->shadowArray[0];
    if (shadowPtr->vrRootBucketArray[vrId].valid == GT_FALSE)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_FOUND, LOG_ERROR_NO_MSG);
    }
    retVal = prvCpssDxChLpmRamUcEntrySearch(vrId,ipAddr.arIP,prefixLen,
                                            nextHopInfoPtr,
                                            PRV_CPSS_DXCH_LPM_PROTOCOL_IPV4_E,
                                            shadowPtr);

    return (retVal);
}

/*******************************************************************************
* prvCpssDxChLpmRamIpv4UcPrefixGetNext
*
* DESCRIPTION:
*       Function Relevant mode : High Level API modes
*       This function returns an IP-Unicast prefix with larger (ipAddrPtr,prefixLenPtr)
*       than the given one, it used for iterating over the exisiting prefixes.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2.
*
* INPUTS:
*       lpmDbPtr          - The LPM DB.
*       vrId              - The virtual router Id to get the entry from.
*       ipAddrPtr         - The ip address to start the search from.
*       prefixLenPtr      - Prefix length of ipAddr.
*
* OUTPUTS:
*       ipAddrPtr         - The ip address of the found entry.
*       prefixLenPtr      - The prefix length of the found entry.
*       nextHopInfoPtr    - the next hop pointer associated with the found
*                           ipAddr.
*
* RETURNS:
*       GT_OK                    - if the required entry was found, or
*       GT_OUT_OF_RANGE         -  If prefix length is too big, or
*       GT_BAD_PTR               - on NULL pointer
*       GT_NOT_FOUND             - if no more entries are left in the IP table.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       1.  The values of (ipAddrPtr,prefixLenPtr) must be a valid values, it
*           means that they exist in the IP-UC Table, unless this is the first
*           call to this function, then the value of (ipAddrPtr,prefixLenPtr) is
*           (0,0).
*       2.  In order to get route pointer information for (0,0) use the Ipv4
*           UC prefix get function.
*
*******************************************************************************/
GT_STATUS prvCpssDxChLpmRamIpv4UcPrefixGetNext
(
    IN    PRV_CPSS_DXCH_LPM_RAM_SHADOWS_DB_STC          *lpmDbPtr,
    IN    GT_U32                                        vrId,
    INOUT GT_IPADDR                                     *ipAddrPtr,
    INOUT GT_U32                                        *prefixLenPtr,
    OUT   PRV_CPSS_DXCH_LPM_ROUTE_ENTRY_POINTER_STC     *nextHopInfoPtr
)
{
    GT_STATUS                                   retVal = GT_OK;
    PRV_CPSS_DXCH_LPM_RAM_SHADOW_STC            *shadowPtr;
    PRV_CPSS_DXCH_LPM_ROUTE_ENTRY_POINTER_STC   *tmpNextHopInfoPtr;

    /* if initialization has not been done for the requested protocol stack -
    return error */
    if (PRV_CPSS_DXCH_LPM_PROTOCOL_IPV4_STATUS_GET_MAC(lpmDbPtr->protocolBitmap) == GT_FALSE)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_INITIALIZED, LOG_ERROR_NO_MSG);
    }

    /* the search can be done on one shadow */
    shadowPtr = &lpmDbPtr->shadowArray[0];
    if (shadowPtr->vrRootBucketArray[vrId].valid == GT_FALSE)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_FOUND, LOG_ERROR_NO_MSG);
    }
    retVal = prvCpssDxChLpmRamUcEntryGet(vrId,ipAddrPtr->arIP,prefixLenPtr,
                                         &tmpNextHopInfoPtr,
                                         PRV_CPSS_DXCH_LPM_PROTOCOL_IPV4_E,
                                         shadowPtr);
    if (retVal == GT_OK)
    {
        cpssOsMemCpy(nextHopInfoPtr,tmpNextHopInfoPtr,
                     sizeof(PRV_CPSS_DXCH_LPM_ROUTE_ENTRY_POINTER_STC));
    }

    return retVal;
}

/*******************************************************************************
* prvCpssDxChLpmRamIpv4UcPrefixGet
*
* DESCRIPTION:
*       Function Relevant mode : High Level API modes
*       This function gets a given ip address, and returns the next
*       hop pointer information bounded to the longest prefix match.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2.
*
* INPUTS:
*       lpmDbPtr          - The LPM DB
*       vrId              - The virtual router id.
*       ipAddr            - The destination IP address to look for.
*
* OUTPUTS:
*       prefixLenPtr      - Points to the number of bits that are actual valid
*                           in the longest match
*       nextHopPointerPtr - The next hop pointer bounded to the longest match
*
* RETURNS:
*       GT_OK                    - if the required entry was found, or
*       GT_BAD_PTR               - on NULL pointer
*       GT_NOT_FOUND             - if the given ip prefix was not found.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS prvCpssDxChLpmRamIpv4UcPrefixGet
(
    IN  PRV_CPSS_DXCH_LPM_RAM_SHADOWS_DB_STC        *lpmDbPtr,
    IN  GT_U32                                      vrId,
    IN  GT_IPADDR                                   ipAddr,
    OUT GT_U32                                      *prefixLenPtr,
    OUT PRV_CPSS_DXCH_LPM_ROUTE_ENTRY_POINTER_STC   *nextHopPointerPtr
)
{
    GT_STATUS                           retVal = GT_OK;
    PRV_CPSS_DXCH_LPM_RAM_SHADOW_STC    *shadowPtr;

    /* if initialization has not been done for the requested protocol stack -
       return error */
    if (PRV_CPSS_DXCH_LPM_PROTOCOL_IPV4_STATUS_GET_MAC(lpmDbPtr->protocolBitmap) == GT_FALSE)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_INITIALIZED, LOG_ERROR_NO_MSG);
    }

    /* the search can be done on one shadow */
    shadowPtr = &lpmDbPtr->shadowArray[0];
    if (shadowPtr->vrRootBucketArray[vrId].valid == GT_FALSE)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_FOUND, LOG_ERROR_NO_MSG);
    }
    retVal = prvCpssDxChLpmRamUcEntryLpmSearch(vrId,ipAddr.arIP,prefixLenPtr,
                                               nextHopPointerPtr,
                                               PRV_CPSS_DXCH_LPM_PROTOCOL_IPV4_E,
                                               shadowPtr);
    return (retVal);
}

/*******************************************************************************
* prvCpssDxChLpmRamIpv4McEntryAdd
*
* DESCRIPTION:
*       Function Relevant mode : High Level API modes
*       To add the multicast routing information for IP datagrams from
*       a particular source and addressed to a particular IP multicast
*       group address for a specific LPM DB.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2.
*
* INPUTS:
*       lpmDbPtr          - The LPM DB
*       vrId              - The virtual private network identifier.
*       ipGroup           - The IP multicast group address.
*       ipGroupPrefixLen  - The number of bits that are actual valid in,
*                           the ipGroup.
*       ipSrc             - the root address for source base multi tree protocol.
*       ipSrcPrefixLen    - The number of bits that are actual valid in,
*                           the ipSrc.
*       mcRouteEntryPtr   - the mc Route pointer to set for the mc entry.
*       override          - override the existing entry if it already exists
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success, or
*       GT_OUT_OF_RANGE         -  if prefix length is too big
*       GT_ERROR                 - if the virtual router does not exist.
*       GT_NOT_FOUND             - prefix was not found when override is GT_TRUE
*       GT_ALREADY_EXIST         - prefix already exist when override is GT_FALSE
*       GT_BAD_PTR               - on NULL pointer
*       GT_OUT_OF_CPU_MEM        - if failed to allocate CPU memory, or
*       GT_OUT_OF_PP_MEM         - if failed to allocate PP memory, or
*       GT_FAIL                  - otherwise.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       1.  To override the default mc route use ipGroup = 0.
*       2.  If (S,G) MC group is added when (*,G) doesn't exists then implicit
*           (*,G) is added pointing to (*,*) default. Application added (*,G)
*           will override the implicit (*,G).
*
*******************************************************************************/
GT_STATUS prvCpssDxChLpmRamIpv4McEntryAdd
(
    IN PRV_CPSS_DXCH_LPM_RAM_SHADOWS_DB_STC         *lpmDbPtr,
    IN GT_U32                                       vrId,
    IN GT_IPADDR                                    ipGroup,
    IN GT_U32                                       ipGroupPrefixLen,
    IN GT_IPADDR                                    ipSrc,
    IN GT_U32                                       ipSrcPrefixLen,
    IN PRV_CPSS_DXCH_LPM_ROUTE_ENTRY_POINTER_STC    *mcRouteEntryPtr,
    IN GT_BOOL                                      override
)
{
    GT_STATUS           retVal = GT_OK;
    PRV_CPSS_DXCH_LPM_RAM_SHADOW_STC *shadowPtr;
    GT_U32 shadowIdx;

    if ((ipGroupPrefixLen > 4) && (ipGroupPrefixLen < 32))
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    retVal = prvCpssDxChLpmRouteEntryPointerCheck(mcRouteEntryPtr);
    if (retVal != GT_OK)
        return retVal;

    /* if initialization has not been done for the requested protocol stack -
    return error */
    if (PRV_CPSS_DXCH_LPM_PROTOCOL_IPV4_STATUS_GET_MAC(lpmDbPtr->protocolBitmap) == GT_FALSE)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_INITIALIZED, LOG_ERROR_NO_MSG);
    }

    for (shadowIdx = 0 ; shadowIdx < lpmDbPtr->numOfShadowCfg; shadowIdx++)
    {
        shadowPtr = &lpmDbPtr->shadowArray[shadowIdx];
        if (shadowPtr->vrRootBucketArray[vrId].valid == GT_FALSE)
        {
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_FOUND, LOG_ERROR_NO_MSG);
        }
        retVal = prvCpssDxChLpmRamMcEntryAdd(vrId,ipGroup.arIP,ipGroupPrefixLen,
                                             ipSrc.arIP,ipSrcPrefixLen,
                                             mcRouteEntryPtr,
                                             override,
                                             PRV_CPSS_DXCH_LPM_RAM_TRIE_INSERT_SDW_MEM_HW_MODE_E,
                                             PRV_CPSS_DXCH_LPM_PROTOCOL_IPV4_E,
                                             shadowPtr);
        if (retVal != GT_OK)
        {
            break;
        }
    }
    return (retVal);
}

/*******************************************************************************
* prvCpssDxChLpmRamIpv4McEntryDel
*
* DESCRIPTION:
*       Function Relevant mode : High Level API modes
*       To delete a particular mc route entry for a specific LPM DB.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2.
*
* INPUTS:
*       lpmDbPtr          - The LPM DB
*       vrId              - The virtual router identifier.
*       ipGroup           - The IP multicast group address.
*       ipGroupPrefixLen  - The number of bits that are actual valid in,
*                           the ipGroup.
*       ipSrc             - the root address for source base multi tree protocol.
*       ipSrcPrefixLen    - The number of bits that are actual valid in,
*                           the ipSrc.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success, or
*       GT_OUT_OF_RANGE         -  if prefix length is too big, or
*       GT_ERROR                 - if the virtual router does not exist, or
*       GT_NOT_FOUND             - if the (ipGroup,prefix) does not exist, or
*       GT_FAIL                  - otherwise.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       1.  In order to delete the multicast entry and all the src ip addresses
*           associated with it, call this function with ipSrc = ipSrcPrefixLen = 0.
*       2.  If no (*,G) was added but (S,G) were added, then implicit (*,G)
*           that points to (*,*) is added. If (*,G) is added later, it will
*           replace the implicit (*,G).
*           When deleting (*,G), then if there are still (S,G), an implicit (*,G)
*           pointing to (*,*) will be added.
*           When deleting last (S,G) and the (*,G) was implicitly added, then
*           the (*,G) will be deleted as well.
*
*******************************************************************************/
GT_STATUS prvCpssDxChLpmRamIpv4McEntryDel
(
    IN PRV_CPSS_DXCH_LPM_RAM_SHADOWS_DB_STC *lpmDbPtr,
    IN GT_U32                               vrId,
    IN GT_IPADDR                            ipGroup,
    IN GT_U32                               ipGroupPrefixLen,
    IN GT_IPADDR                            ipSrc,
    IN GT_U32                               ipSrcPrefixLen
)
{
    GT_STATUS           retVal = GT_OK;
    PRV_CPSS_DXCH_LPM_RAM_SHADOW_STC *shadowPtr;
    GT_U32 shadowIdx;

    /* if initialization has not been done for the requested protocol stack -
    return error */
    if (PRV_CPSS_DXCH_LPM_PROTOCOL_IPV4_STATUS_GET_MAC(lpmDbPtr->protocolBitmap) == GT_FALSE)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_INITIALIZED, LOG_ERROR_NO_MSG);
    }

    for (shadowIdx = 0 ; shadowIdx < lpmDbPtr->numOfShadowCfg; shadowIdx++)
    {
        shadowPtr = &lpmDbPtr->shadowArray[shadowIdx];
        if (shadowPtr->vrRootBucketArray[vrId].valid == GT_FALSE)
        {
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_FOUND, LOG_ERROR_NO_MSG);
        }
        retVal = prvCpssDxChLpmRamMcEntryDelete(vrId,ipGroup.arIP,ipGroupPrefixLen,
                                                ipSrc.arIP,ipSrcPrefixLen,
                                                PRV_CPSS_DXCH_LPM_PROTOCOL_IPV4_E,
                                                shadowPtr,GT_FALSE,NULL);
        if (retVal != GT_OK)
        {
            break;
        }
    }
    return (retVal);
}

/*******************************************************************************
* prvCpssDxChLpmRamIpv4McEntriesFlush
*
* DESCRIPTION:
*       Function Relevant mode : High Level API modes
*       flushes the multicast IP Routing table and stays with the default entry
*       only for a specific LPM DB.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2.
*
* INPUTS:
*       lpmDbPtr      - The LPM DB
*       vrId          - The virtual router identifier.
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
GT_STATUS prvCpssDxChLpmRamIpv4McEntriesFlush
(
    IN PRV_CPSS_DXCH_LPM_RAM_SHADOWS_DB_STC *lpmDbPtr,
    IN GT_U32                               vrId
)
{
    GT_STATUS           retVal = GT_OK;
    PRV_CPSS_DXCH_LPM_RAM_SHADOW_STC *shadowPtr;
    GT_U32 shadowIdx;

    /* if initialization has not been done for the requested protocol stack -
    return error */
    if (PRV_CPSS_DXCH_LPM_PROTOCOL_IPV4_STATUS_GET_MAC(lpmDbPtr->protocolBitmap) == GT_FALSE)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_INITIALIZED, LOG_ERROR_NO_MSG);
    }

    for (shadowIdx = 0 ; shadowIdx < lpmDbPtr->numOfShadowCfg; shadowIdx++)
    {
        shadowPtr = &lpmDbPtr->shadowArray[shadowIdx];
        if (shadowPtr->vrRootBucketArray[vrId].valid == GT_FALSE)
        {
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_FOUND, LOG_ERROR_NO_MSG);
        }
        retVal = prvCpssDxChLpmRamMcEntriesFlush(vrId,
                                                 PRV_CPSS_DXCH_LPM_PROTOCOL_IPV4_E,
                                                 shadowPtr);
        if (retVal != GT_OK)
        {
            break;
        }
    }
    return (retVal);
}

/*******************************************************************************
* prvCpssDxChLpmRamIpv4McEntryGet
*
* DESCRIPTION:
*       Function Relevant mode : High Level API modes
*       This function returns the muticast (ipSrc,ipGroup) entry, used
*       to find specific multicast adrress entry
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2.
*
* INPUTS:
*       lpmDbPtr          - The LPM DB
*       vrId              - The virtual router Id.
*       ipGroup           - The ip Group address to get the next entry for.
*       ipGroupPrefixLen  - ipGroup prefix length.
*       ipSrc             - The ip Source address to get the next entry for.
*       ipSrcPrefixLen    - ipSrc prefix length.
*
* OUTPUTS:
*       mcRouteEntryPtr   - the mc route entry ptr of the found mc mc route
*
* RETURNS:
*       GT_OK                    - if found, or
*       GT_OUT_OF_RANGE         -  if prefix length is too big, or
*       GT_BAD_PTR               - if NULL pointer, or
*       GT_NOT_FOUND             - if the given address is the last one on the IP-Mc table, or
*       GT_NOT_APPLICABLE_DEVICE - if not applicable device.
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS prvCpssDxChLpmRamIpv4McEntryGet
(
    IN  PRV_CPSS_DXCH_LPM_RAM_SHADOWS_DB_STC        *lpmDbPtr,
    IN  GT_U32                                      vrId,
    IN  GT_IPADDR                                   ipGroup,
    IN  GT_U32                                      ipGroupPrefixLen,
    IN  GT_IPADDR                                   ipSrc,
    IN  GT_U32                                      ipSrcPrefixLen,
    OUT PRV_CPSS_DXCH_LPM_ROUTE_ENTRY_POINTER_STC   *mcRouteEntryPtr
)
{
    GT_STATUS           retVal = GT_OK;
    PRV_CPSS_DXCH_LPM_RAM_SHADOW_STC *shadowPtr;

    /* if initialization has not been done for the requested protocol stack -
    return error */
    if (PRV_CPSS_DXCH_LPM_PROTOCOL_IPV4_STATUS_GET_MAC(lpmDbPtr->protocolBitmap) == GT_FALSE)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_INITIALIZED, LOG_ERROR_NO_MSG);
    }

    if (lpmDbPtr->shadowArray->vrRootBucketArray[vrId].valid == GT_FALSE)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_FOUND, LOG_ERROR_NO_MSG);
    }

    /* it's enough to look at the first shadow */
    shadowPtr = &lpmDbPtr->shadowArray[0];
    retVal = prvCpssDxChLpmRamMcEntrySearch(vrId,ipGroup.arIP,ipGroupPrefixLen,
                                            ipSrc.arIP,ipSrcPrefixLen,
                                            mcRouteEntryPtr,
                                            PRV_CPSS_DXCH_LPM_PROTOCOL_IPV4_E,
                                            shadowPtr);
    return (retVal);
}

/*******************************************************************************
* prvCpssDxChLpmRamIpv4McEntryGetNext
*
* DESCRIPTION:
*       Function Relevant mode : High Level API modes
*       This function returns the next muticast (ipSrcPtr,ipGroupPtr) entry, used
*       to iterate over the exisiting multicast addresses for a specific LPM DB.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2.
*
* INPUTS:
*       lpmDbPtr            - The LPM DB
*       vrId                - The virtual router Id.
*       ipGroupPtr          - The ip Group address to get the next entry for.
*       ipGroupPrefixLenPtr - ipGroupPtr prefix length.
*       ipSrcPtr            - The ip Source address to get the next entry for.
*       ipSrcPrefixLenPtr   - ipSrcPtr prefix length.
*
* OUTPUTS:
*       ipGroupPtr          - The next ip Group address.
*       ipGroupPrefixLenPtr - ipGroupPtr prefix length.
*       ipSrcPtr            - The next ip Source address.
*       ipSrcPrefixLenPtr   - ipSrc prefix length.
*       mcRouteEntryPtr     - the mc route entry ptr of the found mc mc route
*
* RETURNS:
*       GT_OK                    - if found, or
*       GT_OUT_OF_RANGE         -  if prefix length is too big, or
*       GT_BAD_PTR               - if NULL pointer, or
*       GT_NOT_FOUND             - if the given address is the last one on the IP-Mc table, or
*       GT_NOT_APPLICABLE_DEVICE - if not applicable device.
*
* COMMENTS:
*       1.  The values of (ipSrcPtr,ipGroupPtr) must be a valid values, it
*           means that they exist in the IP-Mc Table, unless this is the first
*           call to this function, then the value of (ipSrcPtr,ipGroupPtr) is
*           (0,0).
*       2.  In order to get route pointer information for (0,0) use the Ipv4
*           MC get function.
*
*******************************************************************************/
GT_STATUS prvCpssDxChLpmRamIpv4McEntryGetNext
(
    IN    PRV_CPSS_DXCH_LPM_RAM_SHADOWS_DB_STC      *lpmDbPtr,
    IN    GT_U32                                    vrId,
    INOUT GT_IPADDR                                 *ipGroupPtr,
    INOUT GT_U32                                    *ipGroupPrefixLenPtr,
    INOUT GT_IPADDR                                 *ipSrcPtr,
    INOUT GT_U32                                    *ipSrcPrefixLenPtr,
    OUT   PRV_CPSS_DXCH_LPM_ROUTE_ENTRY_POINTER_STC *mcRouteEntryPtr
)
{
    GT_STATUS           retVal = GT_OK;
    PRV_CPSS_DXCH_LPM_RAM_SHADOW_STC *shadowPtr;

    /* if initialization has not been done for the requested protocol stack -
    return error */
    if (PRV_CPSS_DXCH_LPM_PROTOCOL_IPV4_STATUS_GET_MAC(lpmDbPtr->protocolBitmap) == GT_FALSE)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_INITIALIZED, LOG_ERROR_NO_MSG);
    }

    if (lpmDbPtr->shadowArray->vrRootBucketArray[vrId].valid == GT_FALSE)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_FOUND, LOG_ERROR_NO_MSG);
    }

    /* it's enough to look at the first shadow */
    shadowPtr = &lpmDbPtr->shadowArray[0];
    retVal = prvCpssDxChLpmRamMcEntryGetNext(vrId,ipGroupPtr->arIP,ipGroupPrefixLenPtr,
                                             ipSrcPtr->arIP,
                                             ipSrcPrefixLenPtr,
                                             mcRouteEntryPtr,
                                             PRV_CPSS_DXCH_LPM_PROTOCOL_IPV4_E,
                                             shadowPtr);
    /* group prefix 0 means this is the default entry (*,*) */
    if (*ipGroupPrefixLenPtr == 0)
    {
        ipGroupPtr->u32Ip = 0;
    }
    return (retVal);
}

/*******************************************************************************
* prvCpssDxChLpmRamIpv6UcPrefixAdd
*
* DESCRIPTION:
*       Function Relevant mode : High Level API modes
*       creates a new or override an existing Ipv6 prefix in a Virtual Router
*       for the specified LPM DB.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2.
*
* INPUTS:
*       lpmDbPtr          - The LPM DB
*       vrId              - The virtual router id.
*       ipAddr            - Points to the destination IP address of this prefix.
*       prefixLen         - The number of bits that are actual valid in the ipAddr.
*       nextHopInfoPtr    - Points to the next hop pointer to set for this prefix.
*       override          - override the existing entry if it already exists
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - if success, or
*       GT_OUT_OF_RANGE         -  if prefix length is too big, or
*       GT_ERROR                 - if the vrId was not created yet, or
*       GT_ALREADY_EXIST         - if prefix already exist when override is GT_FALSE, or
*       GT_BAD_PTR               - if NULL pointer, or
*       GT_OUT_OF_CPU_MEM        - if failed to allocate CPU memory, or
*       GT_OUT_OF_PP_MEM         - if failed to allocate PP memory, or
*       GT_NOT_APPLICABLE_DEVICE - if not applicable device, or
*       GT_FAIL                  - otherwise.
*
* COMMENTS:
*       To change the default prefix for the VR use prefixLen = 0.
*
*******************************************************************************/
GT_STATUS prvCpssDxChLpmRamIpv6UcPrefixAdd
(
    IN PRV_CPSS_DXCH_LPM_RAM_SHADOWS_DB_STC         *lpmDbPtr,
    IN GT_U32                                       vrId,
    IN GT_IPV6ADDR                                  ipAddr,
    IN GT_U32                                       prefixLen,
    IN PRV_CPSS_DXCH_LPM_ROUTE_ENTRY_POINTER_STC    *nextHopInfoPtr,
    IN GT_BOOL                                      override
)
{
    GT_STATUS           retVal = GT_OK;
    PRV_CPSS_DXCH_LPM_RAM_SHADOW_STC *shadowPtr;
    GT_U32 shadowIdx;

    retVal = prvCpssDxChLpmRouteEntryPointerCheck(nextHopInfoPtr);
    if (retVal != GT_OK)
        return retVal;

    /* if initialization has not been done for the requested protocol stack -
    return error */
    if (PRV_CPSS_DXCH_LPM_PROTOCOL_IPV6_STATUS_GET_MAC(lpmDbPtr->protocolBitmap) == GT_FALSE)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_INITIALIZED, LOG_ERROR_NO_MSG);
    }

    for (shadowIdx = 0 ; shadowIdx < lpmDbPtr->numOfShadowCfg; shadowIdx++)
    {
        shadowPtr = &lpmDbPtr->shadowArray[shadowIdx];

        retVal = prvCpssDxChLpmRamUcEntryAdd(vrId,
                                             ipAddr.arIP,
                                             prefixLen,
                                             nextHopInfoPtr,
                                             PRV_CPSS_DXCH_LPM_RAM_TRIE_INSERT_SDW_MEM_HW_MODE_E,
                                             override,
                                             PRV_CPSS_DXCH_LPM_PROTOCOL_IPV6_E,
                                             shadowPtr);
        if (retVal != GT_OK)
        {
            break;
        }
    }
    return retVal;
}

/*******************************************************************************
* prvCpssDxChLpmRamIpv6UcPrefixBulkAdd
*
* DESCRIPTION:
*       Function Relevant mode : High Level API modes
*       creates a new or override an existing bulk of Ipv6 prefixes in a Virtual
*       Router for the specified LPM DB.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2.
*
* INPUTS:
*       lpmDbPtr              - The LPM DB
*       ipv6PrefixArrayLen    - Length of UC prefix array.
*       ipv6PrefixArrayPtr    - The UC prefix array.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - if all prefixes were successfully added
*       GT_OUT_OF_RANGE         -  if prefix length is too big
*       GT_BAD_PTR               - on NULL pointer
*       GT_FAIL                  - if adding one or more prefixes failed; the
*                                  array will contain return status for each prefix
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       none.
*
*******************************************************************************/
GT_STATUS prvCpssDxChLpmRamIpv6UcPrefixBulkAdd
(
    IN PRV_CPSS_DXCH_LPM_RAM_SHADOWS_DB_STC     *lpmDbPtr,
    IN GT_U32                                   ipv6PrefixArrayLen,
    IN CPSS_DXCH_IP_LPM_IPV6_UC_PREFIX_STC      *ipv6PrefixArrayPtr
)
{
    GT_STATUS           retVal = GT_OK,retVal1=GT_OK,retVal2=GT_OK;
    PRV_CPSS_DXCH_LPM_RAM_SHADOW_STC *shadowPtr;
    GT_U32 shadowIdx;
    GT_U32 i;
    PRV_CPSS_DXCH_LPM_ROUTE_ENTRY_INFO_UNT lpmRouteEntry;
    PRV_CPSS_DXCH_LPM_PROTOCOL_BMP     protocolBitmap=0;

    cpssOsMemSet(&lpmRouteEntry,0,sizeof(lpmRouteEntry));

    for (i = 0 ; i < ipv6PrefixArrayLen ; i++)
    {
        if (ipv6PrefixArrayPtr[i].prefixLen > 128)
        {
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_OUT_OF_RANGE, LOG_ERROR_NO_MSG);
        }

        if (ipv6PrefixArrayPtr[i].ipAddr.arIP[0] >= PRV_CPSS_DXCH_LPM_RAM_START_OF_IPV6_MC_ADDRESS_SPACE_CNS)
        {
            /* Multicast range */
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
        }
    }

    /* if initialization has not been done for the requested protocol stack -
    return error */
    if (PRV_CPSS_DXCH_LPM_PROTOCOL_IPV6_STATUS_GET_MAC(lpmDbPtr->protocolBitmap) == GT_FALSE)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_INITIALIZED, LOG_ERROR_NO_MSG);
    }

    for (shadowIdx = 0 ; shadowIdx < lpmDbPtr->numOfShadowCfg; shadowIdx++)
    {
        shadowPtr = &lpmDbPtr->shadowArray[shadowIdx];
        /* first go over the prefixes and update the shadow */
        for (i = 0 ; i < ipv6PrefixArrayLen ; i++)
        {
            prvCpssDxChLpmConvertIpRouteEntryToLpmRouteEntry(shadowPtr->shadowType,
                                                             &(ipv6PrefixArrayPtr[i].nextHopInfo),
                                                             &lpmRouteEntry);
            retVal = prvCpssDxChLpmRouteEntryPointerCheck(&(lpmRouteEntry.routeEntry));
            if (retVal != GT_OK)
            {
                return retVal;
            }
            ipv6PrefixArrayPtr[i].returnStatus =
                prvCpssDxChLpmRamUcEntryAdd(ipv6PrefixArrayPtr[i].vrId,
                                            ipv6PrefixArrayPtr[i].ipAddr.arIP,
                                            ipv6PrefixArrayPtr[i].prefixLen,
                                            &lpmRouteEntry.routeEntry,
                                            PRV_CPSS_DXCH_LPM_RAM_TRIE_INSERT_SDW_MEM_MODE_E,
                                            ipv6PrefixArrayPtr[i].override,
                                            PRV_CPSS_DXCH_LPM_PROTOCOL_IPV6_E,
                                            shadowPtr);

            if (ipv6PrefixArrayPtr[i].returnStatus != GT_OK)
            {
                retVal2 = GT_FAIL;
            }
        }

        prvCpssDxChLpmConvertIpProtocolStackToProtocolBitmap(PRV_CPSS_DXCH_LPM_PROTOCOL_IPV6_E, &protocolBitmap);
        /* now update the HW */
        retVal1 = updateHwSearchStc(PRV_CPSS_DXCH_LPM_RAM_SEARCH_MEM_UPDATE_E,
                                    protocolBitmap,
                                    shadowPtr);
        if (retVal1 != GT_OK)
        {
            return (retVal1);
        }
    }
    return (retVal2);
}

/*******************************************************************************
* prvCpssDxChLpmRamIpv6UcPrefixDel
*
* DESCRIPTION:
*       Function Relevant mode : High Level API modes
*       deletes an existing Ipv6 prefix in a Virtual Router for the specified
*       LPM DB.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2.
*
* INPUTS:
*       lpmDbPtr      - The LPM DB
*       vrId          - The virtual router id.
*       ipAddr        - Points to the destination IP address of the prefix.
*       prefixLen     - The number of bits that are actual valid in the ipAddr.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - if success, or
*       GT_OUT_OF_RANGE          - if prefix length is too big, or
*       GT_ERROR                 - if the vrId was not created yet, or
*       GT_BAD_PTR               - if NULL pointer, or
*       GT_NO_SUCH               - if the given prefix doesn't exitst in the VR, or
*       GT_NOT_APPLICABLE_DEVICE - if not applicable device, or
*       GT_FAIL                  - otherwise.
*
* COMMENTS:
*       The default prefix (prefixLen = 0) can't be deleted!
*
*******************************************************************************/
GT_STATUS prvCpssDxChLpmRamIpv6UcPrefixDel
(
    IN PRV_CPSS_DXCH_LPM_RAM_SHADOWS_DB_STC *lpmDbPtr,
    IN GT_U32                               vrId,
    IN GT_IPV6ADDR                          ipAddr,
    IN GT_U32                               prefixLen
)
{
    GT_STATUS           retVal = GT_OK;
    PRV_CPSS_DXCH_LPM_RAM_SHADOW_STC *shadowPtr;
    GT_U32 shadowIdx;

    /* if initialization has not been done for the requested protocol stack -
    return error */
    if (PRV_CPSS_DXCH_LPM_PROTOCOL_IPV6_STATUS_GET_MAC(lpmDbPtr->protocolBitmap) == GT_FALSE)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_INITIALIZED, LOG_ERROR_NO_MSG);
    }

    for (shadowIdx = 0 ; shadowIdx < lpmDbPtr->numOfShadowCfg; shadowIdx++)
    {
        shadowPtr = &lpmDbPtr->shadowArray[shadowIdx];

        retVal = prvCpssDxChLpmRamUcEntryDel(vrId,
                                             ipAddr.arIP,
                                             prefixLen,
                                             GT_TRUE,
                                             PRV_CPSS_DXCH_LPM_PROTOCOL_IPV6_E,
                                             shadowPtr,
                                             GT_FALSE);
        if (retVal != GT_OK)
        {
            break;
        }
    }
    return retVal;
}

/*******************************************************************************
* prvCpssDxChLpmRamIpv6UcPrefixBulkDel
*
* DESCRIPTION:
*       Function Relevant mode : High Level API modes
*       deletes an existing bulk of Ipv6 prefixes in a Virtual Router for the
*       specified LPM DB.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2.
*
* INPUTS:
*       lpmDbPtr              - The LPM DB
*       ipv6PrefixArrayLen    - Length of UC prefix array.
*       ipv6PrefixArrayPtr    - The UC prefix array.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - if all prefixes were successfully deleted
*       GT_OUT_OF_RANGE         -  if prefix length is too big
*       GT_BAD_PTR               - on NULL pointer
*       GT_FAIL                  - if deleting one or more prefixes failed; the
*                                  array will contain return status for each prefix
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       none.
*
*******************************************************************************/
GT_STATUS prvCpssDxChLpmRamIpv6UcPrefixBulkDel
(
    IN PRV_CPSS_DXCH_LPM_RAM_SHADOWS_DB_STC     *lpmDbPtr,
    IN GT_U32                                   ipv6PrefixArrayLen,
    IN CPSS_DXCH_IP_LPM_IPV6_UC_PREFIX_STC      *ipv6PrefixArrayPtr
)
{
    GT_STATUS           retVal = GT_OK,retVal1;
    GT_U32      i;
    PRV_CPSS_DXCH_LPM_RAM_SHADOW_STC *shadowPtr;
    GT_U32 shadowIdx;
    PRV_CPSS_DXCH_LPM_PROTOCOL_BMP     protocolBitmap=0;

    if(ipv6PrefixArrayLen == 0)
        return GT_OK;

    /* if initialization has not been done for the requested protocol stack -
    return error */
    if (PRV_CPSS_DXCH_LPM_PROTOCOL_IPV6_STATUS_GET_MAC(lpmDbPtr->protocolBitmap) == GT_FALSE)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_INITIALIZED, LOG_ERROR_NO_MSG);
    }

    for (shadowIdx = 0 ; shadowIdx < lpmDbPtr->numOfShadowCfg; shadowIdx++)
    {
        shadowPtr = &lpmDbPtr->shadowArray[shadowIdx];
        for (i = 0 ; i < ipv6PrefixArrayLen ; i++)
        {
            if (ipv6PrefixArrayPtr[i].prefixLen > 128)
            {
                CPSS_LOG_ERROR_AND_RETURN_MAC(GT_OUT_OF_RANGE, LOG_ERROR_NO_MSG);
            }

            if (ipv6PrefixArrayPtr[i].ipAddr.arIP[0] >= PRV_CPSS_DXCH_LPM_RAM_START_OF_IPV6_MC_ADDRESS_SPACE_CNS)
            {
                /* Multicast range */
                CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
            }

            ipv6PrefixArrayPtr[i].returnStatus =
                prvCpssDxChLpmRamUcEntryDel(ipv6PrefixArrayPtr[i].vrId,
                                            ipv6PrefixArrayPtr[i].ipAddr.arIP,
                                            ipv6PrefixArrayPtr[i].prefixLen,
                                            GT_FALSE,
                                            PRV_CPSS_DXCH_LPM_PROTOCOL_IPV6_E,
                                            shadowPtr,
                                            GT_FALSE);

            if (ipv6PrefixArrayPtr[i].returnStatus != GT_OK)
            {
                retVal = GT_FAIL;
            }
        }

        prvCpssDxChLpmConvertIpProtocolStackToProtocolBitmap(PRV_CPSS_DXCH_LPM_PROTOCOL_IPV6_E, &protocolBitmap);
        /* now update the HW */
        retVal1 = updateHwSearchStc(PRV_CPSS_DXCH_LPM_RAM_SEARCH_MEM_UPDATE_AND_ALLOC_E,
                                    protocolBitmap,
                                    shadowPtr);
        if (retVal1 != GT_OK)
        {
            return (retVal1);
        }
    }
    return (retVal);
}

/*******************************************************************************
* prvCpssDxChLpmRamIpv6UcPrefixesFlush
*
* DESCRIPTION:
*       Function Relevant mode : High Level API modes
*       flushes the unicast IPv6 Routing table and stays with the default prefix
*       only for a specific LPM DB.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2.
*
* INPUTS:
*       lpmDbPtr      - The LPM DB
*       vrId          - The virtual router identifier.
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
*
*******************************************************************************/
GT_STATUS prvCpssDxChLpmRamIpv6UcPrefixesFlush
(
    IN PRV_CPSS_DXCH_LPM_RAM_SHADOWS_DB_STC *lpmDbPtr,
    IN GT_U32                               vrId
)
{
    GT_STATUS           retVal = GT_OK;
    PRV_CPSS_DXCH_LPM_RAM_SHADOW_STC *shadowPtr;
    GT_U32 shadowIdx;

    /* if initialization has not been done for the requested protocol stack -
    return error */
    if (PRV_CPSS_DXCH_LPM_PROTOCOL_IPV6_STATUS_GET_MAC(lpmDbPtr->protocolBitmap) == GT_FALSE)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_INITIALIZED, LOG_ERROR_NO_MSG);
    }

    for (shadowIdx = 0 ; shadowIdx < lpmDbPtr->numOfShadowCfg; shadowIdx++)
    {
        shadowPtr = &lpmDbPtr->shadowArray[shadowIdx];
        if (shadowPtr->vrRootBucketArray[vrId].valid == GT_FALSE)
        {
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_FOUND, LOG_ERROR_NO_MSG);
        }
        retVal = prvCpssDxChLpmRamUcEntriesFlush(vrId,
                                                 PRV_CPSS_DXCH_LPM_PROTOCOL_IPV6_E,
                                                 shadowPtr);
        if (retVal != GT_OK)
        {
            break;
        }
    }
    return (retVal);
}

/*******************************************************************************
* prvCpssDxChLpmRamIpv6UcPrefixSearch
*
* DESCRIPTION:
*       Function Relevant mode : High Level API modes
*       This function searches for a given ip-uc address, and returns the next
*       hop pointer information bound to it if found.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2.
*
* INPUTS:
*       lpmDbPtr          - The LPM DB
*       vrId              - The virtual router id.
*       ipAddr            - Points to the destination IP address to look for.
*       prefixLen         - The number of bits that are actual valid in the
*                           ipAddr.
*
* OUTPUTS:
*       nextHopInfoPtr    - If found this is The next hop pointer to for this
*                           prefix.
*
* RETURNS:
*       GT_OK                    - if the required entry was found
*       GT_OUT_OF_RANGE          - if prefix length is too big
*       GT_BAD_PTR               - on NULL pointer
*       GT_NOT_FOUND             - if the given ip prefix was not found.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS prvCpssDxChLpmRamIpv6UcPrefixSearch
(
    IN PRV_CPSS_DXCH_LPM_RAM_SHADOWS_DB_STC         *lpmDbPtr,
    IN  GT_U32                                      vrId,
    IN  GT_IPV6ADDR                                 ipAddr,
    IN  GT_U32                                      prefixLen,
    OUT PRV_CPSS_DXCH_LPM_ROUTE_ENTRY_POINTER_STC   *nextHopInfoPtr
)
{
    GT_STATUS           retVal = GT_OK;
    PRV_CPSS_DXCH_LPM_RAM_SHADOW_STC *shadowPtr;

    /* if initialization has not been done for the requested protocol stack -
    return error */
    if (PRV_CPSS_DXCH_LPM_PROTOCOL_IPV6_STATUS_GET_MAC(lpmDbPtr->protocolBitmap) == GT_FALSE)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_INITIALIZED, LOG_ERROR_NO_MSG);
    }

    /* the search can be done on one shadow */
    shadowPtr = &lpmDbPtr->shadowArray[0];
    retVal = prvCpssDxChLpmRamUcEntrySearch(vrId,ipAddr.arIP,prefixLen,
                                            nextHopInfoPtr,
                                          PRV_CPSS_DXCH_LPM_PROTOCOL_IPV6_E,
                                            shadowPtr);
    return (retVal);
}

/*******************************************************************************
* prvCpssDxChLpmRamIpv6UcPrefixGetNext
*
* DESCRIPTION:
*       Function Relevant mode : High Level API modes
*       This function returns an IP-Unicast prefix with larger (ipAddrPtr,prefixLenPtr)
*       than the given one, it used for iterating over the exisiting prefixes.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2.
*
* INPUTS:
*       lpmDbPtr          - The LPM DB
*       vrId              - The virtual router Id to get the entry from.
*       ipAddrPtr         - The ip address to start the search from.
*       prefixLenPtr      - Prefix length of ipAddr.
*
* OUTPUTS:
*       ipAddrPtr         - The ip address of the found entry.
*       prefixLenPtr      - The prefix length of the found entry.
*       nextHopInfoPtr    - the next hop pointer associated with the found
*                           ipAddr.
*
* RETURNS:
*       GT_OK                    - if the required entry was found
*       GT_OUT_OF_RANGE          - if prefix length is too big
*       GT_BAD_PTR               - on NULL pointer
*       GT_NOT_FOUND             - if no more entries are left in the IP table.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       1.  The values of (ipAddrPtr,prefixLenPtr) must be a valid values, it
*           means that they exist in the IP-UC Table, unless this is the first
*           call to this function, then the value of (ipAddrPtr,prefixLenPtr) is
*           (0,0).
*       2.  In order to get route pointer information for (0,0) use the Ipv6
*           UC prefix get function.
*
*******************************************************************************/
GT_STATUS prvCpssDxChLpmRamIpv6UcPrefixGetNext
(
    IN    PRV_CPSS_DXCH_LPM_RAM_SHADOWS_DB_STC      *lpmDbPtr,
    IN    GT_U32                                    vrId,
    INOUT GT_IPV6ADDR                               *ipAddrPtr,
    INOUT GT_U32                                    *prefixLenPtr,
    OUT   PRV_CPSS_DXCH_LPM_ROUTE_ENTRY_POINTER_STC *nextHopInfoPtr
)
{
    GT_STATUS           retVal = GT_OK;
    PRV_CPSS_DXCH_LPM_RAM_SHADOW_STC *shadowPtr;
    PRV_CPSS_DXCH_LPM_ROUTE_ENTRY_POINTER_STC *tmpNextHopInfoPtr;

    /* if initialization has not been done for the requested protocol stack -
    return error */
    if (PRV_CPSS_DXCH_LPM_PROTOCOL_IPV6_STATUS_GET_MAC(lpmDbPtr->protocolBitmap) == GT_FALSE)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_INITIALIZED, LOG_ERROR_NO_MSG);
    }

    /* the search can be done on one shadow */
    shadowPtr = &lpmDbPtr->shadowArray[0];
    if (shadowPtr->vrRootBucketArray[vrId].valid == GT_FALSE)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_FOUND, LOG_ERROR_NO_MSG);
    }
    retVal = prvCpssDxChLpmRamUcEntryGet(vrId,ipAddrPtr->arIP,prefixLenPtr,
                                         &tmpNextHopInfoPtr,
                                         PRV_CPSS_DXCH_LPM_PROTOCOL_IPV6_E,
                                         shadowPtr);
    if (retVal == GT_OK)
    {
        cpssOsMemCpy(nextHopInfoPtr,tmpNextHopInfoPtr,
                     sizeof(PRV_CPSS_DXCH_LPM_ROUTE_ENTRY_POINTER_STC));
    }

    return retVal;
}

/*******************************************************************************
* prvCpssDxChLpmRamIpv6UcPrefixGet
*
* DESCRIPTION:
*       Function Relevant mode : High Level API modes
*       This function gets a given ip address, and returns the next
*       hop pointer information bounded to the longest prefix match.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2.
*
* INPUTS:
*       lpmDbPtr          - The LPM DB
*       vrId              - The virtual router id.
*       ipAddr            - The destination IP address to look for.
*
* OUTPUTS:
*       prefixLenPtr      - Points to the number of bits that are actual valid
*                           in the longest match
*       nextHopInfoPtr    - The next hop pointer bounded to the longest match
*
* RETURNS:
*       GT_OK                    - if the required entry was found, or
*       GT_BAD_PTR               - on NULL pointer
*       GT_NOT_FOUND             - if the given ip prefix was not found.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS prvCpssDxChLpmRamIpv6UcPrefixGet
(
    IN  PRV_CPSS_DXCH_LPM_RAM_SHADOWS_DB_STC        *lpmDbPtr,
    IN  GT_U32                                      vrId,
    IN  GT_IPV6ADDR                                 ipAddr,
    OUT GT_U32                                      *prefixLenPtr,
    OUT PRV_CPSS_DXCH_LPM_ROUTE_ENTRY_POINTER_STC   *nextHopInfoPtr
)
{
    GT_STATUS                       retVal = GT_OK;
    PRV_CPSS_DXCH_LPM_RAM_SHADOW_STC   *shadowPtr;

    /* if initialization has not been done for the requested protocol stack -
       return error */
    if (PRV_CPSS_DXCH_LPM_PROTOCOL_IPV6_STATUS_GET_MAC(lpmDbPtr->protocolBitmap) == GT_FALSE)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_INITIALIZED, LOG_ERROR_NO_MSG);
    }

    /* the search can be done on one shadow */
    shadowPtr = &lpmDbPtr->shadowArray[0];
    retVal = prvCpssDxChLpmRamUcEntryLpmSearch(vrId,ipAddr.arIP,prefixLenPtr,
                                               nextHopInfoPtr,
                                               PRV_CPSS_DXCH_LPM_PROTOCOL_IPV6_E,
                                               shadowPtr);
    return (retVal);
}

/*******************************************************************************
* prvCpssDxChLpmRamIpv6McEntryAdd
*
* DESCRIPTION:
*       Function Relevant mode : High Level API modes
*       To add the multicast routing information for IP datagrams from
*       a particular source and addressed to a particular IP multicast
*       group address for a specific LPM DB.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2.
*
* INPUTS:
*       lpmDbPtr          - The LPM DB
*       vrId              - The virtual private network identifier.
*       ipGroup           - The IP multicast group address.
*       ipGroupPrefixLen  - The number of bits that are actual valid in,
*                           the ipGroup.
*       ipSrc             - the root address for source base multi tree protocol.
*       ipSrcPrefixLen    - The number of bits that are actual valid in,
*                           the ipSrc.
*       mcRouteEntryPtr   - (pointer to) the mc Route pointer to set for the mc entry.
*       override          - override the existing entry if it already exists
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success, or
*       GT_OUT_OF_RANGE         -  if prefix length is too big, or
*       GT_ERROR                 - if the virtual router does not exist, or
*       GT_NOT_FOUND             - if prefix was not found when override is GT_TRUE, or
*       GT_ALREADY_EXIST         - if prefix already exist when override is GT_FALSE, or
*       GT_BAD_PTR               - if NULL pointer, or
*       GT_OUT_OF_CPU_MEM        - if failed to allocate CPU memory, or
*       GT_OUT_OF_PP_MEM         - if failed to allocate PP memory, or
*       GT_NOT_APPLICABLE_DEVICE - if not applicable device, or
*       GT_FAIL                  - otherwise.
*
* COMMENTS:
*       1.  To override the default mc route use ipGroup = 0.
*       2.  If (S,G) MC group is added when (*,G) doesn't exists then implicit
*           (*,G) is added pointing to (*,*) default. Application added (*,G)
*           will override the implicit (*,G).
*
*******************************************************************************/
GT_STATUS prvCpssDxChLpmRamIpv6McEntryAdd
(
    IN PRV_CPSS_DXCH_LPM_RAM_SHADOWS_DB_STC         *lpmDbPtr,
    IN GT_U32                                       vrId,
    IN GT_IPV6ADDR                                  ipGroup,
    IN GT_U32                                       ipGroupPrefixLen,
    IN GT_IPV6ADDR                                  ipSrc,
    IN GT_U32                                       ipSrcPrefixLen,
    IN PRV_CPSS_DXCH_LPM_ROUTE_ENTRY_POINTER_STC    *mcRouteEntryPtr,
    IN GT_BOOL                                      override
)
{
    GT_STATUS           retVal = GT_OK;
    PRV_CPSS_DXCH_LPM_RAM_SHADOW_STC *shadowPtr;
    GT_U32 shadowIdx;

    if ((ipGroupPrefixLen > 8) && (ipGroupPrefixLen < 128))
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    retVal = prvCpssDxChLpmRouteEntryPointerCheck(mcRouteEntryPtr);
    if (retVal != GT_OK)
        return retVal;

    /* if initialization has not been done for the requested protocol stack -
    return error */
    if (PRV_CPSS_DXCH_LPM_PROTOCOL_IPV6_STATUS_GET_MAC(lpmDbPtr->protocolBitmap) == GT_FALSE)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_INITIALIZED, LOG_ERROR_NO_MSG);
    }

    for (shadowIdx = 0 ; shadowIdx < lpmDbPtr->numOfShadowCfg; shadowIdx++)
    {
        shadowPtr = &lpmDbPtr->shadowArray[shadowIdx];
        retVal = prvCpssDxChLpmRamMcEntryAdd(vrId,ipGroup.arIP,ipGroupPrefixLen,
                                             ipSrc.arIP,ipSrcPrefixLen,
                                             mcRouteEntryPtr,
                                             override,
                                             PRV_CPSS_DXCH_LPM_RAM_TRIE_INSERT_SDW_MEM_HW_MODE_E,
                                             PRV_CPSS_DXCH_LPM_PROTOCOL_IPV6_E,
                                             shadowPtr);
        if (retVal != GT_OK)
        {
            break;
        }
    }
    return (retVal);
}

/*******************************************************************************
* prvCpssDxChLpmRamIpv6McEntryDel
*
* DESCRIPTION:
*       Function Relevant mode : High Level API modes
*       To delete a particular mc route entry for a specific LPM DB.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2.
*
* INPUTS:
*       lpmDbPtr          - The LPM DB
*       vrId              - The virtual router identifier.
*       ipGroup           - The IP multicast group address.
*       ipGroupPrefixLen  - The number of bits that are actual valid in,
*                           the ipGroup.
*       ipSrc             - the root address for source base multi tree protocol.
*       ipSrcPrefixLen    - The number of bits that are actual valid in,
*                           the ipSrc.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success, or
*       GT_OUT_OF_RANGE         -  if prefix length is too big, or
*       GT_ERROR                 - if the virtual router does not exist, or
*       GT_NOT_FOUND             - if the (ipGroup,prefix) does not exist, or
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device, or
*       GT_FAIL                  - otherwise.
*
* COMMENTS:
*       1.  Inorder to delete the multicast entry and all the src ip addresses
*           associated with it, call this function with ipSrc = ipSrcPrefixLen = 0.
*       2.  If no (*,G) was added but (S,G) were added, then implicit (*,G)
*           that points to (*,*) is added. If (*,G) is added later, it will
*           replace the implicit (*,G).
*           When deleting (*,G), then if there are still (S,G), an implicit (*,G)
*           pointing to (*,*) will be added.
*           When deleting last (S,G) and the (*,G) was implicitly added, then
*           the (*,G) will be deleted as well.
*
*******************************************************************************/
GT_STATUS prvCpssDxChLpmRamIpv6McEntryDel
(
    IN PRV_CPSS_DXCH_LPM_RAM_SHADOWS_DB_STC *lpmDbPtr,
    IN GT_U32                               vrId,
    IN GT_IPV6ADDR                          ipGroup,
    IN GT_U32                               ipGroupPrefixLen,
    IN GT_IPV6ADDR                          ipSrc,
    IN GT_U32                               ipSrcPrefixLen
)
{
    GT_STATUS           retVal = GT_OK;
    PRV_CPSS_DXCH_LPM_RAM_SHADOW_STC *shadowPtr;
    GT_U32 shadowIdx;

    /* if initialization has not been done for the requested protocol stack -
    return error */
    if (PRV_CPSS_DXCH_LPM_PROTOCOL_IPV6_STATUS_GET_MAC(lpmDbPtr->protocolBitmap) == GT_FALSE)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_INITIALIZED, LOG_ERROR_NO_MSG);
    }

    for (shadowIdx = 0 ; shadowIdx < lpmDbPtr->numOfShadowCfg; shadowIdx++)
    {
        shadowPtr = &lpmDbPtr->shadowArray[shadowIdx];
        retVal = prvCpssDxChLpmRamMcEntryDelete(vrId,ipGroup.arIP,ipGroupPrefixLen,
                                                ipSrc.arIP,ipSrcPrefixLen,
                                                PRV_CPSS_DXCH_LPM_PROTOCOL_IPV6_E,
                                                shadowPtr,GT_FALSE,NULL);
        if (retVal != GT_OK)
        {
            break;
        }
    }
    return (retVal);
}

/*******************************************************************************
* prvCpssDxChLpmRamIpv6McEntriesFlush
*
* DESCRIPTION:
*       Function Relevant mode : High Level API modes
*       flushes the multicast IP Routing table and stays with the default entry
*       only for a specific LPM DB.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2.
*
* INPUTS:
*       lpmDbPtr      - The LPM DB
*       vrId          - The virtual router identifier.
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
GT_STATUS prvCpssDxChLpmRamIpv6McEntriesFlush
(
    IN PRV_CPSS_DXCH_LPM_RAM_SHADOWS_DB_STC *lpmDbPtr,
    IN GT_U32                               vrId
)
{
    GT_STATUS           retVal = GT_OK;
    PRV_CPSS_DXCH_LPM_RAM_SHADOW_STC *shadowPtr;
    GT_U32 shadowIdx;

    /* if initialization has not been done for the requested protocol stack -
    return error */
    if (PRV_CPSS_DXCH_LPM_PROTOCOL_IPV6_STATUS_GET_MAC(lpmDbPtr->protocolBitmap) == GT_FALSE)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_INITIALIZED, LOG_ERROR_NO_MSG);
    }

    for (shadowIdx = 0 ; shadowIdx < lpmDbPtr->numOfShadowCfg; shadowIdx++)
    {
        shadowPtr = &lpmDbPtr->shadowArray[shadowIdx];
        retVal = prvCpssDxChLpmRamMcEntriesFlush(vrId,
                                                 PRV_CPSS_DXCH_LPM_PROTOCOL_IPV6_E,
                                                 shadowPtr);
        if (retVal != GT_OK)
        {
            break;
        }
    }
    return (retVal);
}

/*******************************************************************************
* prvCpssDxChLpmRamIpv6McEntryGet
*
* DESCRIPTION:
*       Function Relevant mode : High Level API modes
*       This function returns the muticast (ipSrc,ipGroup) entry, used
*       to find specific multicast adrress entry
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2.
*
* INPUTS:
*       lpmDbPtr          - The LPM DB
*       vrId              - The virtual router Id.
*       ipGroup           - The ip Group address to get the next entry for.
*       ipGroupPrefixLen  - ipGroup prefix length.
*       ipSrc             - The ip Source address to get the next entry for.
*       ipSrcPrefixLen    - ipSrc prefix length.
*
* OUTPUTS:
*       mcRouteEntryPtr   - (pointer to) the mc route entry ptr of the found mc mc route
*
* RETURNS:
*       GT_OK                    - if found, or
*       GT_OUT_OF_RANGE         -  if prefix length is too big, or
*       GT_BAD_PTR               - if NULL pointer, or
*       GT_NOT_FOUND             - if the given address is the last one on the IP-Mc table, or
*       GT_NOT_APPLICABLE_DEVICE - if not applicable device.
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS prvCpssDxChLpmRamIpv6McEntryGet
(
    IN  PRV_CPSS_DXCH_LPM_RAM_SHADOWS_DB_STC        *lpmDbPtr,
    IN  GT_U32                                      vrId,
    IN  GT_IPV6ADDR                                 ipGroup,
    IN  GT_U32                                      ipGroupPrefixLen,
    IN  GT_IPV6ADDR                                 ipSrc,
    IN  GT_U32                                      ipSrcPrefixLen,
    OUT PRV_CPSS_DXCH_LPM_ROUTE_ENTRY_POINTER_STC   *mcRouteEntryPtr
)
{
    GT_STATUS                               retVal = GT_OK;
    PRV_CPSS_DXCH_LPM_RAM_SHADOW_STC        *shadowPtr;

    /* if initialization has not been done for the requested protocol stack -
    return error */
    if (PRV_CPSS_DXCH_LPM_PROTOCOL_IPV6_STATUS_GET_MAC(lpmDbPtr->protocolBitmap) == GT_FALSE)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_INITIALIZED, LOG_ERROR_NO_MSG);
    }

    if (lpmDbPtr->shadowArray->vrRootBucketArray[vrId].valid == GT_FALSE)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_FOUND, LOG_ERROR_NO_MSG);
    }

    /* it's enough to look at the first shadow */
    shadowPtr = &lpmDbPtr->shadowArray[0];
    retVal = prvCpssDxChLpmRamMcEntrySearch(vrId,ipGroup.arIP,ipGroupPrefixLen,
                                            ipSrc.arIP,ipSrcPrefixLen,
                                            mcRouteEntryPtr,
                                            PRV_CPSS_DXCH_LPM_PROTOCOL_IPV6_E,
                                            shadowPtr);
    return (retVal);
}

/*******************************************************************************
* prvCpssDxChLpmRamIpv6McEntryGetNext
*
* DESCRIPTION:
*       Function Relevant mode : High Level API modes
*       This function returns the next muticast (ipSrcPtr,ipGroupPtr) entry, used
*       to iterate over the exisiting multicast addresses for a specific LPM DB.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2.
*
* INPUTS:
*       lpmDbPtr            - The LPM DB
*       vrId                - The virtual router Id.
*       ipGroupPtr          - (pointer to) The ip Group address to get the next entry for.
*       ipGroupPrefixLenPtr - (pointer to) the ipGroupPtr prefix length.
*       ipSrcPtr            - (pointer to) The ip Source address to get the next entry for.
*       ipSrcPrefixLenPtr   - (pointer to) ipSrcPtr prefix length.
*
* OUTPUTS:
*       ipGroupPtr          - (pointer to) The next ip Group address.
*       ipGroupPrefixLenPtr - (pointer to) the ipGroupPtr prefix length.
*       ipSrcPtr            - (pointer to) The next ip Source address.
*       ipSrcPrefixLenPtr   - (pointer to) ipSrcPtr prefix length.
*       mcRouteEntryPtr     - (pointer to) the mc route entry ptr of the found mc mc route
*
* RETURNS:
*       GT_OK                    - if found, or
*       GT_OUT_OF_RANGE         -  if prefix length is too big, or
*       GT_BAD_PTR               - if NULL pointer, or
*       GT_NOT_FOUND             - if the given address is the last one on the IP-Mc table, or
*       GT_NOT_APPLICABLE_DEVICE - if not applicable device
*
* COMMENTS:
*       1.  The values of (ipSrcPtr,ipGroupPtr) must be a valid values, it
*           means that they exist in the IP-Mc Table, unless this is the first
*           call to this function, then the value of (ipSrcPtr,ipGroupPtr) is
*           (0,0).
*       2.  In order to get route pointer information for (0,0) use the Ipv6
*           MC get function.
*
*******************************************************************************/
GT_STATUS prvCpssDxChLpmRamIpv6McEntryGetNext
(
    IN    PRV_CPSS_DXCH_LPM_RAM_SHADOWS_DB_STC      *lpmDbPtr,
    IN    GT_U32                                    vrId,
    INOUT GT_IPV6ADDR                               *ipGroupPtr,
    INOUT GT_U32                                    *ipGroupPrefixLenPtr,
    INOUT GT_IPV6ADDR                               *ipSrcPtr,
    INOUT GT_U32                                    *ipSrcPrefixLenPtr,
    OUT   PRV_CPSS_DXCH_LPM_ROUTE_ENTRY_POINTER_STC *mcRouteEntryPtr
)
{
    GT_STATUS           retVal = GT_OK;
    PRV_CPSS_DXCH_LPM_RAM_SHADOW_STC *shadowPtr;

    /* if initialization has not been done for the requested protocol stack -
    return error */
    if (PRV_CPSS_DXCH_LPM_PROTOCOL_IPV6_STATUS_GET_MAC(lpmDbPtr->protocolBitmap) == GT_FALSE)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_INITIALIZED, LOG_ERROR_NO_MSG);
    }

    if (lpmDbPtr->shadowArray->vrRootBucketArray[vrId].valid == GT_FALSE)
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_FOUND, LOG_ERROR_NO_MSG);

    /* it's enough to look at the first shadow */
    shadowPtr = &lpmDbPtr->shadowArray[0];
    retVal = prvCpssDxChLpmRamMcEntryGetNext(vrId,ipGroupPtr->arIP,ipGroupPrefixLenPtr,
                                             ipSrcPtr->arIP,
                                             ipSrcPrefixLenPtr,
                                             mcRouteEntryPtr,
                                             PRV_CPSS_DXCH_LPM_PROTOCOL_IPV6_E,
                                             shadowPtr);
    /* group prefix 0 means this is the default entry (*,*) */
    if (*ipGroupPrefixLenPtr == 0)
    {
        cpssOsMemSet(ipGroupPtr->u32Ip,0,sizeof(GT_U32)*4);
    }
    return (retVal);
}

/*******************************************************************************
* prvCpssDxChLpmRamFcoePrefixAdd
*
* DESCRIPTION:
*       Function Relevant mode : High Level API modes
*       This function adds a new FCoE prefix to a Virtual Router in a
*       specific LPM DB or overrides an existing existing FCoE prefix.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2.
*
* INPUTS:
*       lpmDbPtr          - The LPM DB
*       vrId              - The virtual router id.
*       fcid              - The FC_ID of this prefix.
*       prefixLen         - The number of bits that are actual valid in the FC_ID.
*       nextHopInfoPtr    - (points to) The next hop pointer to set for this prefix.
*       override          - override the existing entry if it already exists
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success, or
*       GT_OUT_OF_RANGE         -  If prefix length is too big, or
*       GT_ERROR                 - If the vrId was not created yet, or
*       GT_ALREADY_EXIST         - prefix already exist when override is GT_FALSE
*       GT_BAD_PTR               - on NULL pointer
*       GT_OUT_OF_CPU_MEM        - If failed to allocate CPU memory, or
*       GT_OUT_OF_PP_MEM         - If failed to allocate PP memory, or
*       GT_FAIL                  - otherwise.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       To change the default prefix for the VR use prefixLen = 0.
*
*******************************************************************************/
GT_STATUS prvCpssDxChLpmRamFcoePrefixAdd
(
    IN  PRV_CPSS_DXCH_LPM_RAM_SHADOWS_DB_STC        *lpmDbPtr,
    IN  GT_U32                                      vrId,
    IN  GT_FCID                                     fcid,
    IN  GT_U32                                      prefixLen,
    IN  PRV_CPSS_DXCH_LPM_ROUTE_ENTRY_POINTER_STC   *nextHopInfoPtr,
    IN  GT_BOOL                                     override
)
{
    *lpmDbPtr = *lpmDbPtr;
    vrId = vrId;
    fcid = fcid;
    prefixLen = prefixLen;
    *nextHopInfoPtr = *nextHopInfoPtr;
    override = override;

    CPSS_TBD_BOOKMARK
    return GT_OK;
}

/*******************************************************************************
* prvCpssDxChLpmRamFcoePrefixDel
*
* DESCRIPTION:
*       Function Relevant mode : High Level API modes
*       Deletes an existing FCoE prefix in a Virtual Router for the specified
*       LPM DB.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2.
*
* INPUTS:
*       lpmDbPtr        - The LPM DB.
*       vrId            - The virtual router id.
*       fcid            - The destination FC_ID address of the prefix.
*       prefixLen       - The number of bits that are actual valid in the FC_ID.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success, or
*       GT_OUT_OF_RANGE         -  If prefix length is too big, or
*       GT_ERROR                 - If the vrId was not created yet, or
*       GT_NO_SUCH               - If the given prefix doesn't exitst in the VR, or
*       GT_FAIL                  - otherwise.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       the default prefix (prefixLen = 0) can't be deleted!
*
*******************************************************************************/
GT_STATUS prvCpssDxChLpmRamFcoePrefixDel
(
    IN  PRV_CPSS_DXCH_LPM_RAM_SHADOWS_DB_STC    *lpmDbPtr,
    IN  GT_U32                                  vrId,
    IN  GT_FCID                                 fcid,
    IN  GT_U32                                  prefixLen
)
{
    *lpmDbPtr = *lpmDbPtr;
    vrId = vrId;
    fcid = fcid;
    prefixLen = prefixLen;

    CPSS_TBD_BOOKMARK
    return GT_OK;
}

#if 0
/*******************************************************************************
* prvCpssDxChLpmRamFcoePrefixBulkAdd
*
* DESCRIPTION:
*       Function Relevant mode : High Level API modes
*       Creates a new or override an existing bulk of FCoE prefixes in a Virtual
*       Router for the specified LPM DB.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2.
*
* INPUTS:
*       lpmDbPtr              - The LPM DB
*       fcidPrefixArrayLen    - Length of FC_ID prefix array.
*       fcidPrefixArrayPtr    - The FC_ID prefix array.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - if all prefixes were successfully added
*       GT_OUT_OF_RANGE         -  if prefix length is too big
*       GT_BAD_PTR               - on NULL pointer
*       GT_FAIL                  - if adding one or more prefixes failed; the
*                                  array will contain return status for each prefix
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS prvCpssDxChLpmRamFcoePrefixBulkAdd
(
    IN  PRV_CPSS_DXCH_LPM_RAM_SHADOWS_DB_STC            *lpmDbPtr,
    IN  GT_U32                                          fcidPrefixArrayLen,
    IN  CPSS_DXCH_FCOE_LPM_PREFIX_BULK_OPERATION_STC    *fcidPrefixArrayPtr
)
{
    CPSS_TBD_BOOKMARK
    return GT_OK;
}

/*******************************************************************************
* prvCpssDxChLpmRamFcoePrefixBulkDel
*
* DESCRIPTION:
*       Function Relevant mode : High Level mode
*       Deletes an existing bulk of FCoE prefixes in a Virtual Router for the
*       specified LPM DB.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2.
*
* INPUTS:
*       lpmDbPtr              - The LPM DB
*       fcidPrefixArrayLen    - Length of FC_ID prefix array.
*       fcidPrefixArrayPtr    - The FC_ID prefix array.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - if all prefixes were successfully deleted
*       GT_OUT_OF_RANGE         -  if prefix length is too big
*       GT_BAD_PTR               - if NULL pointer
*       GT_FAIL                  - if deleting one or more prefixes failed; the
*                                  array will contain return status for each prefix
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS prvCpssDxChLpmRamFcoePrefixBulkDel
(
    IN  PRV_CPSS_DXCH_LPM_RAM_SHADOWS_DB_STC            *lpmDbPtr,
    IN  GT_U32                                          fcidPrefixArrayLen,
    IN  CPSS_DXCH_FCOE_LPM_PREFIX_BULK_OPERATION_STC    *fcidPrefixArrayPtr
)
{
    CPSS_TBD_BOOKMARK
    return GT_OK;
}
#endif

/*******************************************************************************
* prvCpssDxChLpmRamFcoePrefixesFlush
*
* DESCRIPTION:
*       Function Relevant mode : High Level API modes
*       Flushes the FCoE forwarding table and stays with the default prefix
*       only for a specific LPM DB.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2.
*
* INPUTS:
*       lpmDbPtr      - The LPM DB
*       vrId          - The virtual router identifier.
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
*
*******************************************************************************/
GT_STATUS prvCpssDxChLpmRamFcoePrefixesFlush
(
    IN PRV_CPSS_DXCH_LPM_RAM_SHADOWS_DB_STC  *lpmDbPtr,
    IN GT_U32                                vrId
)
{
    *lpmDbPtr = *lpmDbPtr;
    vrId = vrId;

    CPSS_TBD_BOOKMARK
    return GT_OK;
}

/*******************************************************************************
* prvCpssDxChLpmRamFcoePrefixSearch
*
* DESCRIPTION:
*       Function Relevant mode : High Level API modes
*       This function searches for a given FC_ID, and returns the next
*       hop pointer information bound to it if found.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2.
*
* INPUTS:
*       lpmDbPtr          - The LPM DB
*       vrId              - The virtual router id.
*       fcid              - The FC_ID to look for.
*       prefixLen         - The number of bits that are actual valid in the
*                           FC_ID.
*
* OUTPUTS:
*       nextHopInfoPtr    - If found this is The next hop pointer to for this
*                           prefix.
*
* RETURNS:
*       GT_OK                    - the required entry was found, or
*       GT_OUT_OF_RANGE          - the prefix length is too big, or
*       GT_BAD_PTR               - on NULL pointer
*       GT_NOT_FOUND             - the given prefix was not found.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS prvCpssDxChLpmRamFcoePrefixSearch
(
    IN  PRV_CPSS_DXCH_LPM_RAM_SHADOWS_DB_STC        *lpmDbPtr,
    IN  GT_U32                                      vrId,
    IN  GT_FCID                                     fcid,
    IN  GT_U32                                      prefixLen,
    OUT PRV_CPSS_DXCH_LPM_ROUTE_ENTRY_POINTER_STC   *nextHopInfoPtr
)
{
    *lpmDbPtr = *lpmDbPtr;
    vrId = vrId;
    fcid = fcid;
    prefixLen = prefixLen;
    *nextHopInfoPtr = *nextHopInfoPtr;

    CPSS_TBD_BOOKMARK
    return GT_OK;
}

/*******************************************************************************
* prvCpssDxChLpmRamFcoePrefixGetNext
*
* DESCRIPTION:
*       Function Relevant mode : High Level API modes
*       This function returns an FCoE prefix with larger (FC_ID,prefix)
*       than the given one, it used for iterating over the exisiting prefixes.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2.
*
* INPUTS:
*       lpmDbPtr          - The LPM DB.
*       vrId              - The virtual router Id to get the entry from.
*       fcidPtr           - The FC_ID to start the search from.
*       prefixLenPtr      - The number of bits that are actual valid in the
*                           FC_ID.
*
* OUTPUTS:
*       fcidPtr           - The FC_ID of the found entry.
*       prefixLenPtr      - The prefix length of the found entry.
*       nextHopInfoPtr    - the next hop pointer associated with the found FC_ID
*
* RETURNS:
*       GT_OK                    - the required entry was found, or
*       GT_OUT_OF_RANGE          - the prefix length is too big, or
*       GT_BAD_PTR               - on NULL pointer
*       GT_NOT_FOUND             - no more entries are left in the FC_ID table.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       1.  The values of (FC_ID,prefix) must be a valid values, it
*           means that they exist in the forwarding Table, unless this is the
*           first call to this function, then the value of (FC_ID,prefix) is
*           (0,0).
*       2.  In order to get route pointer information for (0,0) use the FC_ID
*           prefix get function.
*
*******************************************************************************/
GT_STATUS prvCpssDxChLpmRamFcoePrefixGetNext
(
    IN    PRV_CPSS_DXCH_LPM_RAM_SHADOWS_DB_STC      *lpmDbPtr,
    IN    GT_U32                                    vrId,
    INOUT GT_FCID                                   *fcidPtr,
    INOUT GT_U32                                    *prefixLenPtr,
    OUT   PRV_CPSS_DXCH_LPM_ROUTE_ENTRY_POINTER_STC *nextHopInfoPtr
)
{
    *lpmDbPtr = *lpmDbPtr;
    vrId = vrId;
    *fcidPtr = *fcidPtr;
    *prefixLenPtr = *prefixLenPtr;
    *nextHopInfoPtr = *nextHopInfoPtr;

    CPSS_TBD_BOOKMARK
    return GT_OK;
}

/*******************************************************************************
* prvCpssDxChLpmRamFcoePrefixGet
*
* DESCRIPTION:
*       Function Relevant mode : High Level API modes
*       This function gets a given FC_ID address, and returns the next
*       hop pointer information bounded to the longest prefix match.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2.
*
* INPUTS:
*       lpmDbPtr          - The LPM DB
*       vrId              - The virtual router id.
*       fcid              - The FC_ID to look for.
*
* OUTPUTS:
*       prefixLenPtr      - Points to the number of bits that are actual valid
*                           in the longest match
*       nextHopInfoPtr    - The next hop pointer bound to the longest match
*
* RETURNS:
*       GT_OK                    - if the required entry was found, or
*       GT_BAD_PTR               - on NULL pointer
*       GT_NOT_FOUND             - if the given prefix was not found.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS prvCpssDxChLpmRamFcoePrefixGet
(
    IN  PRV_CPSS_DXCH_LPM_RAM_SHADOWS_DB_STC        *lpmDbPtr,
    IN  GT_U32                                      vrId,
    IN  GT_FCID                                     fcid,
    OUT GT_U32                                      *prefixLenPtr,
    OUT PRV_CPSS_DXCH_LPM_ROUTE_ENTRY_POINTER_STC   *nextHopInfoPtr
)
{
    *lpmDbPtr = *lpmDbPtr;
    vrId = vrId;
    fcid = fcid;
    *prefixLenPtr = *prefixLenPtr;
    *nextHopInfoPtr = *nextHopInfoPtr;

    CPSS_TBD_BOOKMARK
    return GT_OK;
}

#if 0
/*******************************************************************************
* ipLpmDbGetL3Vr
*
* DESCRIPTION:
*       Retrives the valid virtual routers
*
* INPUTS:
*       ipShadowPtr           - points to the shadow to retrieve from
*       iterPtr               - points to the current iteration
*       dataSizePtr           - points to data size in bytes supposed to be processed
*                               in current iteration
*
* OUTPUTS:
*       tableSizePtr          - points to the table size calculated (in bytes)
*       tablePtrPtr           - the table size info block
*       iterPtr               - points to the current iteration
*       dataSizePtr           - points to data size in bytes left after iteration step.
*
* RETURNS:
*       GT_OK                 - on success
*
* COMMENTS:
*       None
*
*******************************************************************************/
static GT_STATUS ipLpmDbGetL3Vr
(
    IN     PRV_CPSS_DXCH_LPM_RAM_SHADOW_STC     *ipShadowPtr,
    OUT    GT_U32                               *tableSizePtr,
    OUT    GT_VOID                              **tablePtrPtr,
    INOUT  GT_UINTPTR                           *iterPtr,
    INOUT  GT_U32                               *dataSizePtr
)
{
    GT_STATUS retVal = GT_OK;
    GT_U8 ipAddr[PRV_CPSS_DXCH_LPM_NUM_OF_OCTETS_IN_IPV6_PROTOCOL_CNS];
    GT_U32 *vrId;
    PRV_CPSS_DXCH_LPM_ROUTE_ENTRY_POINTER_STC currNh;
    PRV_CPSS_EXMXPM_IPV6MC_PCL_ENTRY_STC ipv6McPclEntry,*ipv6McPclEntryPtr;

    PRV_CPSS_DXCH_LPM_RAM_VR_INFO_STC *vrEntry;
    GT_U32 numOfEntriesLeft = 0;

    GT_U32 numOfEntries = 0;

    if (*iterPtr == 0)
    {
        /* this is the first call , so intilize */
        *iterPtr = (GT_UINTPTR)cpssOsLpmMalloc(sizeof (GT_U32));
        if (*iterPtr == 0)
        {
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_OUT_OF_CPU_MEM, LOG_ERROR_NO_MSG);
        }
        vrId = (GT_U32*)(*iterPtr);
        *vrId = 0;
    }
    else
    {
        vrId = (GT_U32*)(*iterPtr);

    }

    numOfEntriesLeft = *dataSizePtr/(sizeof(PRV_CPSS_DXCH_LPM_RAM_VR_INFO_STC));

    if (numOfEntriesLeft == 0)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NO_RESOURCE, LOG_ERROR_NO_MSG);
    }

    /* set the pointer */
    vrEntry = (PRV_CPSS_DXCH_LPM_RAM_VR_INFO_STC*)(*tablePtrPtr);
    for(;(numOfEntries < numOfEntriesLeft) &&
          (*vrId < ipShadowPtr->vrfTblSize); (*vrId)++)
    {

        if (ipShadowPtr->vrRootBucketArray[*vrId].valid == GT_FALSE)
        {
            continue;
        }

        if (tableSizePtr != NULL)
        {
            *tableSizePtr += sizeof(PRV_CPSS_DXCH_LPM_RAM_VR_INFO_STC);
        }
        if (*tablePtrPtr != NULL)
        {
            vrEntry->vrId = *vrId;
            vrEntry->isLast = GT_FALSE;

            /* check the protocol stack */
            if ((ipShadowPtr->vrRootBucketArray[*vrId].rootBucket[PRV_CPSS_DXCH_LPM_PROTOCOL_IPV4_E] != NULL) &&
                (ipShadowPtr->vrRootBucketArray[*vrId].rootBucket[PRV_CPSS_DXCH_LPM_PROTOCOL_IPV6_E] != NULL))
            {
                vrEntry->protocolStack = CPSS_IP_PROTOCOL_IPV4V6_E;
            }
            else if (ipShadowPtr->vrRootBucketArray[*vrId].rootBucket[PRV_CPSS_DXCH_LPM_PROTOCOL_IPV4_E] != NULL)
            {
                vrEntry->protocolStack = CPSS_IP_PROTOCOL_IPV4_E;
            }
            else
            {
                vrEntry->protocolStack = CPSS_IP_PROTOCOL_IPV6_E;
            }

            /* check IPv4 uc */
            if (ipShadowPtr->vrRootBucketArray[*vrId].rootBucket[PRV_CPSS_DXCH_LPM_PROTOCOL_IPV4_E] != NULL)
            {
                retVal = prvCpssDxChLpmRamUcEntrySearch(*vrId,ipAddr,0,&currNh,
                                                        PRV_CPSS_DXCH_LPM_PROTOCOL_IPV4_E,
                                                        ipShadowPtr);
                if (retVal != GT_OK)
                {
                    CPSS_LOG_ERROR_AND_RETURN_MAC(GT_FAIL, LOG_ERROR_NO_MSG);
                }

                cpssOsMemCpy(&vrEntry->defUcRouteEntrey[PRV_CPSS_DXCH_LPM_PROTOCOL_IPV4_E],
                             &currNh,sizeof(PRV_CPSS_DXCH_LPM_ROUTE_ENTRY_POINTER_STC));
                vrEntry->vrIpUcSupport[PRV_CPSS_DXCH_LPM_PROTOCOL_IPV4_E] = GT_TRUE;
            }
            else
            {
                 vrEntry->vrIpUcSupport[PRV_CPSS_DXCH_LPM_PROTOCOL_IPV4_E] = GT_FALSE;
            }
            /* check ipv4 mc */
            retVal = prvCpssDxChLpmRamMcEntrySearch(*vrId,ipAddr,0,NULL
                                                    ipAddr,0,&currNh,
                                                    PRV_CPSS_DXCH_LPM_PROTOCOL_IPV4_E,
                                                    ipShadowPtr);
            if ((retVal != GT_OK) && (retVal !=GT_NOT_FOUND))
            {
                CPSS_LOG_ERROR_AND_RETURN_MAC(GT_FAIL, LOG_ERROR_NO_MSG);
            }
            if (retVal == GT_NOT_FOUND)
            {
                /* ipv4 mc is disable */
                vrEntry->vrIpMcSupport[PRV_CPSS_DXCH_LPM_PROTOCOL_IPV4_E] = GT_FALSE;
                 cpssOsMemSet(&vrEntry->defMcRouteEntrey[PRV_CPSS_DXCH_LPM_PROTOCOL_IPV4_E],
                         0,sizeof(PRV_CPSS_DXCH_LPM_ROUTE_ENTRY_POINTER_STC));
            }
            else
            {
                vrEntry->vrIpMcSupport[PRV_CPSS_DXCH_LPM_PROTOCOL_IPV4_E] = GT_TRUE;
                 cpssOsMemCpy(&vrEntry->defMcRouteEntrey[PRV_CPSS_DXCH_LPM_PROTOCOL_IPV4_E],
                         &currNh,sizeof(PRV_CPSS_DXCH_LPM_ROUTE_ENTRY_POINTER_STC));
            }

            /* check IPv6 uc */
            if (ipShadowPtr->vrRootBucketArray[*vrId].rootBucket[PRV_CPSS_DXCH_LPM_PROTOCOL_IPV6_E] != NULL)
            {
                retVal = prvCpssDxChLpmRamUcEntrySearch(*vrId,ipAddr,0,&currNh,
                                                        PRV_CPSS_DXCH_LPM_PROTOCOL_IPV6_E,
                                                        ipShadowPtr);
                if (retVal != GT_OK)
                {
                    CPSS_LOG_ERROR_AND_RETURN_MAC(GT_FAIL, LOG_ERROR_NO_MSG);
                }
                cpssOsMemCpy(&vrEntry->defUcRouteEntrey[PRV_CPSS_DXCH_LPM_PROTOCOL_IPV6_E],
                             &currNh,sizeof(PRV_CPSS_DXCH_LPM_ROUTE_ENTRY_POINTER_STC));
                vrEntry->vrIpUcSupport[PRV_CPSS_DXCH_LPM_PROTOCOL_IPV6_E] = GT_TRUE;
            }
            else
            {
                vrEntry->vrIpUcSupport[PRV_CPSS_DXCH_LPM_PROTOCOL_IPV6_E] = GT_FALSE;
            }
            /* check ipv6 mc */

            ipv6McPclEntry.ipGroupPrefix = 0;
            ipv6McPclEntryPtr = prvCpssSlSearch(ipShadowPtr->vrRootBucketArray[*vrId].coreIPv6McGroup2PceIdDb,
                              &ipv6McPclEntry);

            if (ipv6McPclEntryPtr == NULL)
            {
                vrEntry->vrIpMcSupport[PRV_CPSS_DXCH_LPM_PROTOCOL_IPV6_E] = GT_FALSE;
                cpssOsMemSet(&vrEntry->defMcRouteEntrey[PRV_CPSS_DXCH_LPM_PROTOCOL_IPV6_E],
                             0,
                             sizeof(PRV_CPSS_DXCH_LPM_ROUTE_ENTRY_POINTER_STC));
            }
            else
            {
                vrEntry->vrIpMcSupport[PRV_CPSS_DXCH_LPM_PROTOCOL_IPV6_E] = GT_TRUE;
                cpssOsMemCpy(&vrEntry->defMcRouteEntrey[PRV_CPSS_DXCH_LPM_PROTOCOL_IPV6_E],
                             ipv6McPclEntryPtr->lpmBucketPtr->rangeList->lowerLpmPtr.pNextHopEntry,
                             sizeof(PRV_CPSS_DXCH_LPM_ROUTE_ENTRY_POINTER_STC));

                vrEntry->defIpv6McRuleIndex = ipv6McPclEntryPtr->pceId;

                vrEntry->vrIpv6McPclId =
                    ipShadowPtr->vrRootBucketArray[*vrId].vrIpv6McPclId;
            }
            /* advance the pointer */
            vrEntry++;
        }
        numOfEntries++;
    }

    /* check if we finished iterating*/
    if ( (*vrId == ipShadowPtr->vrfTblSize) && (numOfEntriesLeft > numOfEntries))
    {
        if (*tablePtrPtr != NULL)
        {
            vrEntry->isLast = GT_TRUE;
            *tablePtrPtr = (GT_VOID *)(vrEntry + 1);
        }
        else
        {
            /* take in account "last" virtual router entry: */
            *tableSizePtr += sizeof(PRV_CPSS_DXCH_LPM_RAM_VR_INFO_STC);
        }
        numOfEntries++;
        /* free & zero the iterator */
        cpssOsLpmFree((GT_PTR)*iterPtr);
        *iterPtr = 0;

        retVal = GT_OK;
    }
    else
    {
        /* we didn't finish record the ptr */
        *tablePtrPtr = (GT_VOID *)vrEntry;
    }

    /* update the left num of entries */
    numOfEntriesLeft -= numOfEntries;


    *dataSizePtr = *dataSizePtr - numOfEntries *(sizeof(PRV_CPSS_DXCH_LPM_RAM_VR_INFO_STC));

    return (retVal);
}

/*******************************************************************************
* ipLpmDbGetL3UcPrefix
*
* DESCRIPTION:
*       Retrives a specifc device's core ip uc prefix memory Size needed and info
*
* INPUTS:
*       ipShadowPtr           - points to the shadow to retrive from
*       vrId                  - the virtual router id
*       protocolStack         - type of ip protocol stack to work on.
*       iterPtr               - points to the current iteration
*       dataSizePtr           - points to data size in bytes supposed to be processed
*                               in current iteration
* OUTPUTS:
*       tableSizePtr          - points to the table size calculated (in bytes)
*       tablePtrPtr           - points to the table size info block
*       iterPtr               - points to the current iteration
*       dataSizePtr           - points to data size in bytes left after iteration step.
*
* RETURNS:
*       GT_OK                 - on success
*
* COMMENTS:
*       None
*
*******************************************************************************/
static GT_STATUS ipLpmDbGetL3UcPrefix
(
    IN     PRV_CPSS_DXCH_LPM_RAM_SHADOW_STC *ipShadowPtr,
    IN     GT_U32                           vrId,
    IN     CPSS_IP_PROTOCOL_STACK_ENT       protocolStack,
    OUT    GT_U32                           *tableSizePtr,
    OUT    GT_VOID                          **tablePtrPtr,
    INOUT  GT_UINTPTR                       *iterPtr,
    INOUT  GT_U32                           *dataSizePtr
)
{
    GT_STATUS retVal = GT_OK;
    GT_U8 ipAddr[PRV_CPSS_DXCH_LPM_NUM_OF_OCTETS_IN_IPV6_PROTOCOL_CNS];
    GT_U32 ipPrefix;
    PRV_CPSS_DXCH_LPM_ROUTE_ENTRY_POINTER_STC currNh;
    GT_U32 numOfEntries = 0;
    GT_U32 numOfEntriesLeft = 0;
    PRV_CPSS_DXCH_LPM_RAM_IPV6_UC_PREFIX_BULK_OPERATION_STC *ipv6Prefix;
    PRV_CPSS_DXCH_LPM_RAM_IPV4_UC_PREFIX_BULK_OPERATION_STC *ipv4Prefix;
    GT_PTR  dummy;

    cpssOsMemSet(&currNh,0,sizeof(currNh));

    if (*iterPtr == 0)
    {
        /* this is the first call , so intilize */
        *iterPtr = (protocolStack == CPSS_IP_PROTOCOL_IPV4_E) ?
            (GT_UINTPTR)cpssOsLpmMalloc(sizeof (PRV_CPSS_DXCH_LPM_RAM_IPV4_UC_PREFIX_BULK_OPERATION_STC)):
            (GT_UINTPTR)cpssOsLpmMalloc(sizeof (PRV_CPSS_DXCH_LPM_RAM_IPV6_UC_PREFIX_BULK_OPERATION_STC));
        ipPrefix = 0;
    }
    else
    {
        /* get the current */
        if (protocolStack == CPSS_IP_PROTOCOL_IPV4_E)
        {
            ipv4Prefix = (PRV_CPSS_DXCH_LPM_RAM_IPV4_UC_PREFIX_BULK_OPERATION_STC*)(*iterPtr);
            cpssOsMemCpy(ipAddr,ipv4Prefix->ipAddr.arIP,PRV_CPSS_DXCH_LPM_NUM_OF_OCTETS_IN_IPV4_PROTOCOL_CNS*sizeof(GT_U8));
            ipPrefix = ipv4Prefix->prefixLen;

        }
        else
        {
            ipv6Prefix = (PRV_CPSS_DXCH_LPM_RAM_IPV6_UC_PREFIX_BULK_OPERATION_STC*)(*iterPtr);
            cpssOsMemCpy(ipAddr,ipv6Prefix->ipAddr.arIP,PRV_CPSS_DXCH_LPM_NUM_OF_OCTETS_IN_IPV6_PROTOCOL_CNS*sizeof(GT_U8));
            ipPrefix = ipv6Prefix->prefixLen ;
        }
    }


     if (protocolStack == CPSS_IP_PROTOCOL_IPV4_E)
         numOfEntriesLeft = *dataSizePtr/(sizeof(PRV_CPSS_DXCH_LPM_RAM_IPV4_UC_PREFIX_BULK_OPERATION_STC));
     else
         numOfEntriesLeft = *dataSizePtr/(sizeof(PRV_CPSS_DXCH_LPM_RAM_IPV6_UC_PREFIX_BULK_OPERATION_STC));

    if (numOfEntriesLeft == 0)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NO_RESOURCE, LOG_ERROR_NO_MSG);
    }
    /* set the pointer */
    ipv4Prefix = (PRV_CPSS_DXCH_LPM_RAM_IPV4_UC_PREFIX_BULK_OPERATION_STC*)(*tablePtrPtr);
    ipv6Prefix = (PRV_CPSS_DXCH_LPM_RAM_IPV6_UC_PREFIX_BULK_OPERATION_STC*)(*tablePtrPtr);

    retVal =
        prvCpssDxChLpmRamEntryGet(ipShadowPtr->vrRootBucketArray[vrId].rootBucket[protocolStack],
                                  ipAddr,&ipPrefix,&dummy);
    if(retVal == GT_OK)
        currNh = *((PRV_CPSS_DXCH_LPM_ROUTE_ENTRY_POINTER_STC*)dummy);

    while (retVal == GT_OK)
    {
        if (tableSizePtr != NULL)
        {
            *tableSizePtr += (protocolStack == CPSS_IP_PROTOCOL_IPV4_E)?
                          sizeof(PRV_CPSS_DXCH_LPM_RAM_IPV4_UC_PREFIX_BULK_OPERATION_STC):
                sizeof(PRV_CPSS_DXCH_LPM_RAM_IPV6_UC_PREFIX_BULK_OPERATION_STC);
        }
        if (*tablePtrPtr != NULL)
        {
            /* copy the prefix info, start with ip address*/
            if (protocolStack == CPSS_IP_PROTOCOL_IPV4_E)
            {
                cpssOsMemCpy(ipv4Prefix->ipAddr.arIP,ipAddr,PRV_CPSS_DXCH_LPM_NUM_OF_OCTETS_IN_IPV4_PROTOCOL_CNS*sizeof(GT_U8));
                cpssOsMemCpy(&ipv4Prefix->nextHopPointer,&currNh,
                             sizeof(PRV_CPSS_DXCH_LPM_ROUTE_ENTRY_POINTER_STC));
                ipv4Prefix->prefixLen = ipPrefix;
                ipv4Prefix->vrId = vrId;

                /* advance the pointer */
                ipv4Prefix = ipv4Prefix + 1;
            }
            else
            {
                cpssOsMemCpy(ipv6Prefix->ipAddr.arIP,ipAddr,PRV_CPSS_DXCH_LPM_NUM_OF_OCTETS_IN_IPV6_PROTOCOL_CNS*sizeof(GT_U8));
                cpssOsMemCpy(&ipv6Prefix->nextHopPointer,&currNh,
                             sizeof(PRV_CPSS_DXCH_LPM_ROUTE_ENTRY_POINTER_STC));
                ipv6Prefix->prefixLen = ipPrefix;
                ipv6Prefix->vrId = vrId;

                /* advance the pointer */
                ipv6Prefix = ipv6Prefix + 1;
            }
        }
        numOfEntries++;
        if (numOfEntries >= numOfEntriesLeft)
            /* we run out of entries , we'll conitune later */
            break;

        retVal =
            prvCpssDxChLpmRamEntryGet(ipShadowPtr->vrRootBucketArray[vrId].rootBucket[protocolStack],
                                      ipAddr,&ipPrefix,&dummy);

        if(retVal == GT_OK)
            currNh = *((PRV_CPSS_DXCH_LPM_ROUTE_ENTRY_POINTER_STC*)dummy);
    }

    /* check if we finished iterating*/
    if (retVal != GT_OK)
    {
        if (tableSizePtr != NULL)
        {
            /* make space for that the last one with invalid prefix of 255 */
            *tableSizePtr += (protocolStack == CPSS_IP_PROTOCOL_IPV4_E)?
                sizeof(PRV_CPSS_DXCH_LPM_RAM_IPV4_UC_PREFIX_BULK_OPERATION_STC):
                sizeof(PRV_CPSS_DXCH_LPM_RAM_IPV6_UC_PREFIX_BULK_OPERATION_STC);

        }
        if (*tablePtrPtr != NULL)
        {
            /* indicate the last one with invalid prefix of 255 */
            if (protocolStack == CPSS_IP_PROTOCOL_IPV4_E)
            {
                ipv4Prefix->prefixLen = 0xFF;
                /* advance the pointer */
                *tablePtrPtr = (GT_VOID *)(ipv4Prefix + 1);
            }
            else
            {
                ipv6Prefix->prefixLen = 0xFF;
                /* advance the pointer */
                *tablePtrPtr = (GT_VOID *)(ipv6Prefix + 1);
            }
        }
        numOfEntries++;
        /* free & zero the iterator */
        cpssOsLpmFree((GT_PTR)*iterPtr);
        *iterPtr = 0;

        retVal = GT_OK;
    }
    else
    {
        /* we didn't finish , record the currnet prefix. */
        if (protocolStack == CPSS_IP_PROTOCOL_IPV4_E)
        {
            PRV_CPSS_DXCH_LPM_RAM_IPV4_UC_PREFIX_BULK_OPERATION_STC *tmpIpv4Prefix =
                (PRV_CPSS_DXCH_LPM_RAM_IPV4_UC_PREFIX_BULK_OPERATION_STC*)*iterPtr;
            cpssOsMemCpy(tmpIpv4Prefix->ipAddr.arIP,ipAddr,PRV_CPSS_DXCH_LPM_NUM_OF_OCTETS_IN_IPV4_PROTOCOL_CNS*sizeof(GT_U8));
            tmpIpv4Prefix->prefixLen = ipPrefix;

            /* record the ptr */
            *tablePtrPtr = (GT_VOID *)ipv4Prefix;

        }
        else
        {
            PRV_CPSS_DXCH_LPM_RAM_IPV6_UC_PREFIX_BULK_OPERATION_STC *tmpIpv6Prefix =
                (PRV_CPSS_DXCH_LPM_RAM_IPV6_UC_PREFIX_BULK_OPERATION_STC*)*iterPtr;
            cpssOsMemCpy(tmpIpv6Prefix->ipAddr.arIP,ipAddr,PRV_CPSS_DXCH_LPM_NUM_OF_OCTETS_IN_IPV6_PROTOCOL_CNS*sizeof(GT_U8));
            tmpIpv6Prefix->prefixLen = ipPrefix;

            /* record the ptr */
            *tablePtrPtr = (GT_VOID *)ipv6Prefix ;
        }
    }

    /* update the left num of entries */
    numOfEntriesLeft -= numOfEntries;

    if (protocolStack == CPSS_IP_PROTOCOL_IPV4_E)
        *dataSizePtr = *dataSizePtr - numOfEntries * (sizeof(PRV_CPSS_DXCH_LPM_RAM_IPV4_UC_PREFIX_BULK_OPERATION_STC));
    else
        *dataSizePtr = *dataSizePtr - numOfEntries * (sizeof(PRV_CPSS_DXCH_LPM_RAM_IPV6_UC_PREFIX_BULK_OPERATION_STC));

    return(retVal);
}

/*******************************************************************************
* ipLpmDbGetL3UcLpm
*
* DESCRIPTION:
*       Retrives a specifc device's core ip uc prefix memory Size needed and info
*
* INPUTS:
*       ipShadowPtr           - points to the shadow to retrive from
*       vrId                  - the virtual router id
*       protocolStack         - type of ip protocol stack to work on.
*       iterPtr               - points to the current iteration
*       dataSizePtr           - points to data size in bytes supposed to be processed
*
* OUTPUTS:
*       tableSizePtr          - points to the table size calculated (in bytes)
*       tablePtrPtr           - points to the table size info block
*       iterPtr               - points to the current iteration
*       dataSizePtr           - points to data size in bytes left after iteration step.
*
* RETURNS:
*       GT_OK                 - on success
*
* COMMENTS:
*       None
*
*******************************************************************************/
static GT_STATUS ipLpmDbGetL3UcLpm
(
    IN     PRV_CPSS_DXCH_LPM_RAM_SHADOW_STC *ipShadowPtr,
    IN     GT_U32                           vrId,
    IN     CPSS_IP_PROTOCOL_STACK_ENT       protocolStack,
    OUT    GT_U32                           *tableSizePtr,
    OUT    GT_VOID                          **tablePtrPtr,
    INOUT  GT_UINTPTR                       *iterPtr,
    INOUT  GT_U32                           *dataSizePtr
)
{
    GT_STATUS retVal = GT_OK;
    PRV_CPSS_DXCH_LPM_RAM_RANGE_SHADOW_STC tmpRootRange;

    GT_U32 memAllocIndex = 0;
    GT_U32 numOfEntriesLeft = 0;
    PRV_CPSS_DXCH_LPM_RAM_MEM_TRAVERSE_OP_ENT traOp;
    PRV_CPSS_DXCH_LPM_RAM_MEM_ALLOC_INFO_STC *memAllocInfo = NULL;

    if (ipShadowPtr->vrRootBucketArray[vrId].rootBucket[protocolStack] == NULL)
    {
        /* no uc prefixes */
        return GT_OK;
    }
    if (*tablePtrPtr != NULL)
    {
        /* set the params for lpm traverse */
        traOp = PRV_CPSS_DXCH_LPM_RAM_MEM_TRAVERSE_REC_WITH_PCL_E;

        memAllocInfo = (PRV_CPSS_DXCH_LPM_RAM_MEM_ALLOC_INFO_STC*)*tablePtrPtr;
    }
    else
    {
        traOp = PRV_CPSS_DXCH_LPM_RAM_MEM_TRAVERSE_COUNT_WITH_PCL_E;
    }


    numOfEntriesLeft = *dataSizePtr/(sizeof(PRV_CPSS_DXCH_LPM_RAM_MEM_ALLOC_INFO_STC));
    if (numOfEntriesLeft == 0)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NO_RESOURCE, LOG_ERROR_NO_MSG);
    }
    tmpRootRange.lowerLpmPtr.nextBucket =
        ipShadowPtr->vrRootBucketArray[vrId].rootBucket[protocolStack];
    tmpRootRange.pointerType =
        (GT_U8)(ipShadowPtr->vrRootBucketArray[vrId].rootBucketType[protocolStack]);
    tmpRootRange.next = NULL;

    retVal = prvCpssDxChLpmRamMemTraverse(traOp,
                                          memAllocInfo,
                                          &memAllocIndex,
                                          numOfEntriesLeft,
                                          &tmpRootRange,
                                          GT_FALSE,
                                          ipShadowPtr->ipUcSearchMemArrayPtr[protocolStack],
                                          iterPtr);

    /* update the left num of entries */
    numOfEntriesLeft = numOfEntriesLeft - memAllocIndex;

    *dataSizePtr = *dataSizePtr - memAllocIndex * (sizeof(PRV_CPSS_DXCH_LPM_RAM_MEM_ALLOC_INFO_STC));
    if (tableSizePtr != NULL)
    {
        /* calc the lpm memory need */
        *tableSizePtr += memAllocIndex * sizeof(PRV_CPSS_DXCH_LPM_RAM_MEM_ALLOC_INFO_STC);
    }
    if (*tablePtrPtr != NULL)
    {
        /* advance the pointer */
        *tablePtrPtr = (GT_VOID*)(&memAllocInfo[memAllocIndex]);
    }
    return (retVal);
}

/*******************************************************************************
* ipLpmDbGetL3McRoutes
*
* DESCRIPTION:
*       Retrives a specifc device's core ip mc routes memory Size needed and info
*
* INPUTS:
*       ipShadowPtr           - points to the shadow to retrive from
*       vrId                  - the virtual router id
*       protocolStack         - type of ip protocol stack to work on.
*       iterPtr               - points to the current iter
*       dataSizePtr           - points to data size in bytes supposed to be processed
*                               in current iteration
* OUTPUTS:
*       tableSizePtr          - points to the table size calculated (in bytes)
*       tablePtrPtr           - points to the table size info block
*       iterPtr               - points to the current iter
*       dataSizePtr           - points to data size in bytes left after iteration step.
*
* RETURNS:
*       GT_OK                 - on success
*
* COMMENTS:
*       None
*
*******************************************************************************/
static GT_STATUS ipLpmDbGetL3McRoutes
(
    IN     PRV_CPSS_DXCH_LPM_RAM_SHADOW_STC *ipShadowPtr,
    IN     GT_U32                           vrId,
    IN     CPSS_IP_PROTOCOL_STACK_ENT       protocolStack,
    OUT    GT_U32                           *tableSizePtr,
    OUT    GT_VOID                          **tablePtrPtr,
    INOUT  GT_UINTPTR                       *iterPtr,
    INOUT  GT_U32                           *dataSizePtr
)
{
    GT_STATUS retVal = GT_OK;
    GT_U8 groupIp[PRV_CPSS_DXCH_LPM_NUM_OF_OCTETS_IN_IPV6_PROTOCOL_CNS],srcAddr[PRV_CPSS_DXCH_LPM_NUM_OF_OCTETS_IN_IPV6_PROTOCOL_CNS];
    GT_U32 groupPrefix,srcPrefix;
    PRV_CPSS_DXCH_LPM_ROUTE_ENTRY_POINTER_STC mcEntry;
    GT_U32 numOfEntries = 0;
    GT_U32 numOfEntriesLeft = 0;
    IPV6_HSU_MC_ROUTE_ENTRY_STC *ipv6McRoute;
    IPV4_HSU_MC_ROUTE_ENTRY_STC *ipv4McRoute;

    cpssOsMemSet(&mcEntry,0,sizeof(mcEntry));

    if (*iterPtr == 0)
    {
        /* this is the first call , so intilize */
        *iterPtr = (protocolStack == CPSS_IP_PROTOCOL_IPV4_E) ?
               (GT_UINTPTR)cpssOsLpmMalloc(sizeof(IPV4_HSU_MC_ROUTE_ENTRY_STC)):
               (GT_UINTPTR)cpssOsLpmMalloc(sizeof(IPV6_HSU_MC_ROUTE_ENTRY_STC));
        groupPrefix = 0;
    }
    else
    {
        /* get the current */
        if (protocolStack == CPSS_IP_PROTOCOL_IPV4_E)
        {
            ipv4McRoute = (IPV4_HSU_MC_ROUTE_ENTRY_STC*)(*iterPtr);
            cpssOsMemCpy(groupIp,ipv4McRoute->mcGroup.arIP,PRV_CPSS_DXCH_LPM_NUM_OF_OCTETS_IN_IPV4_PROTOCOL_CNS*sizeof(GT_U8));
            cpssOsMemCpy(srcAddr,ipv4McRoute->ipSrc.arIP,PRV_CPSS_DXCH_LPM_NUM_OF_OCTETS_IN_IPV4_PROTOCOL_CNS*sizeof(GT_U8));

            groupPrefix = (ipv4McRoute->mcGroup.u32Ip == 0)?0:32;
        }
        else
        {
            ipv6McRoute = (IPV6_HSU_MC_ROUTE_ENTRY_STC*)(*iterPtr);
            cpssOsMemCpy(groupIp,ipv6McRoute->mcGroup.arIP,PRV_CPSS_DXCH_LPM_NUM_OF_OCTETS_IN_IPV6_PROTOCOL_CNS*sizeof(GT_U8));
            cpssOsMemCpy(srcAddr,ipv6McRoute->ipSrc.arIP,PRV_CPSS_DXCH_LPM_NUM_OF_OCTETS_IN_IPV6_PROTOCOL_CNS*sizeof(GT_U8));

            groupPrefix = ((ipv6McRoute->mcGroup.u32Ip[0] == 0) &&
                           (ipv6McRoute->mcGroup.u32Ip[1] == 0) &&
                           (ipv6McRoute->mcGroup.u32Ip[2] == 0) &&
                           (ipv6McRoute->mcGroup.u32Ip[3] == 0))?0:128;
        }
    }


     if (protocolStack == CPSS_IP_PROTOCOL_IPV4_E)
         numOfEntriesLeft = *dataSizePtr/(sizeof(IPV4_HSU_MC_ROUTE_ENTRY_STC));
     else
         numOfEntriesLeft = *dataSizePtr/(sizeof(IPV6_HSU_MC_ROUTE_ENTRY_STC));

    if (numOfEntriesLeft == 0)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NO_RESOURCE, LOG_ERROR_NO_MSG);
    }
    /* set the ptrs */
    ipv4McRoute = (IPV4_HSU_MC_ROUTE_ENTRY_STC*)*tablePtrPtr;
    ipv6McRoute = (IPV6_HSU_MC_ROUTE_ENTRY_STC*)*tablePtrPtr;

    retVal = prvCpssDxChLpmRamMcEntryGetNext(vrId,groupIp,&groupPrefix,
                                             NULL,srcAddr,&srcPrefix,&mcEntry,
                                             protocolStack,ipShadowPtr);

    while (retVal == GT_OK)
    {
        if (tableSizePtr != NULL)
        {
            *tableSizePtr  +=
                (protocolStack == CPSS_IP_PROTOCOL_IPV4_E)?
                sizeof(IPV4_HSU_MC_ROUTE_ENTRY_STC):
                sizeof(IPV6_HSU_MC_ROUTE_ENTRY_STC);
        }
        if (*tablePtrPtr != NULL)
        {
            if (protocolStack == CPSS_IP_PROTOCOL_IPV4_E)
            {
                cpssOsMemCpy(ipv4McRoute->mcGroup.arIP,groupIp,PRV_CPSS_DXCH_LPM_NUM_OF_OCTETS_IN_IPV4_PROTOCOL_CNS*sizeof(GT_U8));
                cpssOsMemCpy(ipv4McRoute->ipSrc.arIP,srcAddr,PRV_CPSS_DXCH_LPM_NUM_OF_OCTETS_IN_IPV4_PROTOCOL_CNS*sizeof(GT_U8));
                ipv4McRoute->ipSrcPrefixlength = srcPrefix;
                ipv4McRoute->vrId = vrId;
                cpssOsMemCpy(&ipv4McRoute->mcRoutePointer,&mcEntry,
                             sizeof(PRV_CPSS_DXCH_LPM_ROUTE_ENTRY_POINTER_STC));
                ipv4McRoute->last = GT_FALSE;

                ipv4McRoute++;
            }
            else
            {
                cpssOsMemCpy(ipv6McRoute->mcGroup.arIP,groupIp,PRV_CPSS_DXCH_LPM_NUM_OF_OCTETS_IN_IPV6_PROTOCOL_CNS*sizeof(GT_U8));
                cpssOsMemCpy(ipv6McRoute->ipSrc.arIP,srcAddr,PRV_CPSS_DXCH_LPM_NUM_OF_OCTETS_IN_IPV6_PROTOCOL_CNS*sizeof(GT_U8));
                ipv6McRoute->vrId = vrId;
                ipv6McRoute->ipSrcPrefixlength = srcPrefix;
                cpssOsMemCpy(&ipv6McRoute->mcRoutePointer,&mcEntry,
                             sizeof(PRV_CPSS_DXCH_LPM_ROUTE_ENTRY_POINTER_STC));
                ipv6McRoute->last = GT_FALSE;
                ipv6McRoute++;
            }
        }

        numOfEntries++;

        if (numOfEntries >= numOfEntriesLeft)
            /* we run out of entries , we'll continue later */
            break;

        retVal = prvCpssDxChLpmRamMcEntryGetNext(vrId,groupIp,&groupPrefix,NULL,
                                                 srcAddr,&srcPrefix,&mcEntry,
                                                 protocolStack,ipShadowPtr);
    }

    /* check if we finished iterating*/
    if (retVal != GT_OK)
    {
        if (tableSizePtr != NULL)
        {
            /* make space for that the last one with invalid prefix of 255 */
            *tableSizePtr  += (protocolStack == CPSS_IP_PROTOCOL_IPV4_E)?
                sizeof(IPV4_HSU_MC_ROUTE_ENTRY_STC):
                sizeof(IPV6_HSU_MC_ROUTE_ENTRY_STC);
        }

        if (*tablePtrPtr != NULL)
        {
            /* indicate last for mc*/
            if (protocolStack == CPSS_IP_PROTOCOL_IPV4_E)
            {
                ipv4McRoute->last = GT_TRUE;
                *tablePtrPtr = (GT_VOID*)(ipv4McRoute + 1);
            }
            else
            {
                ipv6McRoute->last = GT_TRUE;
                *tablePtrPtr = (GT_VOID*)(ipv6McRoute + 1);
            }
        }
        numOfEntries++;
        /* free & zero the iterator */
        cpssOsLpmFree((GT_PTR)*iterPtr);
        *iterPtr = 0;

        retVal = GT_OK;
    }
    else
    {
        /* we didn't finish , record the currnet prefix. */
        if (protocolStack == CPSS_IP_PROTOCOL_IPV4_E)
        {
            IPV4_HSU_MC_ROUTE_ENTRY_STC* tmpIpv4McRoute =
                (IPV4_HSU_MC_ROUTE_ENTRY_STC*)*iterPtr;
            cpssOsMemCpy(tmpIpv4McRoute->mcGroup.arIP,groupIp,PRV_CPSS_DXCH_LPM_NUM_OF_OCTETS_IN_IPV4_PROTOCOL_CNS*sizeof(GT_U8));
            cpssOsMemCpy(tmpIpv4McRoute->ipSrc.arIP,srcAddr,PRV_CPSS_DXCH_LPM_NUM_OF_OCTETS_IN_IPV4_PROTOCOL_CNS*sizeof(GT_U8));
            /* record the ptr */
            *tablePtrPtr = (GT_VOID *)ipv4McRoute;
        }
        else
        {
            IPV6_HSU_MC_ROUTE_ENTRY_STC* tmpIpv6McRoute =
                (IPV6_HSU_MC_ROUTE_ENTRY_STC*)*iterPtr;
            cpssOsMemCpy(tmpIpv6McRoute->mcGroup.arIP,groupIp,PRV_CPSS_DXCH_LPM_NUM_OF_OCTETS_IN_IPV6_PROTOCOL_CNS*sizeof(GT_U8));
            cpssOsMemCpy(tmpIpv6McRoute->ipSrc.arIP,srcAddr,PRV_CPSS_DXCH_LPM_NUM_OF_OCTETS_IN_IPV6_PROTOCOL_CNS*sizeof(GT_U8));
            /* record the ptr */
            *tablePtrPtr = (GT_VOID *)ipv6McRoute;
        }
    }

    /* update the left num of entries */
    numOfEntriesLeft -= numOfEntries;

     if (protocolStack == CPSS_IP_PROTOCOL_IPV4_E)
         *dataSizePtr = *dataSizePtr - numOfEntries * (sizeof(IPV4_HSU_MC_ROUTE_ENTRY_STC));
     else
         *dataSizePtr = *dataSizePtr - numOfEntries * (sizeof(IPV6_HSU_MC_ROUTE_ENTRY_STC));
    return (retVal);
}

/*******************************************************************************
* ipLpmDbGetL3McLpm
*
* DESCRIPTION:
*       Retrives a specifc device's core ip mc routes memory Size needed and info
*
* INPUTS:
*       ipShadowPtr           - points to the shadow to retrive from
*       vrId                  - the virtual router id
*       protocolStack         - type of ip protocol stack to work on.
*       iterPtr               - points to the current iter
*       dataSizePtr           - points to data size in bytes supposed to be processed
*                               in current iteration step
* OUTPUTS:
*       tableSizePtr          - points to the table size calculated (in bytes)
*       tablePtrPtr           - points to the table size info block
*       iterPtr               - points to the current iter
*       dataSizePtr           - points to data size in bytes left after iteration step.
*
* RETURNS:
*       GT_OK                 - on success
*
* COMMENTS:
*       None
*
*******************************************************************************/
static GT_STATUS ipLpmDbGetL3McLpm
(
    IN     PRV_CPSS_DXCH_LPM_RAM_SHADOW_STC *ipShadowPtr,
    IN     GT_U32                           vrId,
    IN     CPSS_IP_PROTOCOL_STACK_ENT       protocolStack,
    OUT    GT_U32                           *tableSizePtr,
    OUT    GT_VOID                          **tablePtrPtr,
    INOUT  GT_UINTPTR                       *iterPtr,
    INOUT  GT_U32                           *dataSizePtr
)
{
    GT_STATUS retVal = GT_OK;
    PRV_CPSS_EXMXPM_IPV6MC_PCL_ENTRY_STC ipv6McPclEntry,*ipv6McPclEntryPtr;
    GT_U32 memAllocIndex = 0;
    GT_U32 numOfEntriesLeft = 0;
    PRV_CPSS_DXCH_LPM_RAM_MEM_TRAVERSE_OP_ENT traOp;
    PRV_CPSS_DXCH_LPM_RAM_MEM_ALLOC_INFO_STC *memAllocInfo = NULL;

    /* check if mc prefixes are exist */
    if (protocolStack == CPSS_IP_PROTOCOL_IPV4_E )
    {
        if(ipShadowPtr->vrRootBucketArray[vrId].mcRootBucket[CPSS_IP_PROTOCOL_IPV4_E] == NULL)
        {
            /* no ipv4 mc prefixes */
            return GT_OK;
        }
    }
    if (protocolStack == CPSS_IP_PROTOCOL_IPV6_E )
    {
        if(ipShadowPtr->vrRootBucketArray[vrId].mcRootBucket[CPSS_IP_PROTOCOL_IPV6_E] == NULL)
        {
            /* no ipv4 mc prefixes */
            return GT_OK;
        }
    }
    if (*tablePtrPtr != NULL)
    {
        /* set the params for lpm traverse */
        traOp = PRV_CPSS_DXCH_LPM_RAM_MEM_TRAVERSE_REC_WITH_PCL_E;

        memAllocInfo = (PRV_CPSS_DXCH_LPM_RAM_MEM_ALLOC_INFO_STC*)*tablePtrPtr;
    }
    else
    {
        traOp = PRV_CPSS_DXCH_LPM_RAM_MEM_TRAVERSE_COUNT_WITH_PCL_E;
    }
    memAllocIndex = 0;


    numOfEntriesLeft = *dataSizePtr/(sizeof(PRV_CPSS_DXCH_LPM_RAM_MEM_ALLOC_INFO_STC));

    if (numOfEntriesLeft == 0)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NO_RESOURCE, LOG_ERROR_NO_MSG);
    }
    /* move on to mc LPM*/
    retVal = prvCpssDxChLpmRamMcTraverse(traOp,memAllocInfo,
                                         &memAllocIndex,
                                         numOfEntriesLeft,
                                         vrId,ipShadowPtr,iterPtr);
    /* update the left num of entries */
    numOfEntriesLeft = numOfEntriesLeft - memAllocIndex;


    *dataSizePtr = *dataSizePtr - memAllocIndex * (sizeof(PRV_CPSS_DXCH_LPM_RAM_MEM_ALLOC_INFO_STC));

    if (tableSizePtr != NULL)
    {
        /* calc the mc lpm memory need */
        *tableSizePtr  += memAllocIndex * sizeof(PRV_CPSS_DXCH_LPM_RAM_MEM_ALLOC_INFO_STC);
    }
    if (*tablePtrPtr != NULL)
    {
        /* advance the pointer */
        *tablePtrPtr = (GT_VOID*)(&memAllocInfo[memAllocIndex]);
    }
    return (retVal);
}

/*******************************************************************************
* ipLpmDbGetL3DmmFreeLists
*
* DESCRIPTION:
*       Retrives needed dmm free lists memory size  and info.
*
* INPUTS:
*       ipShadowPtr           - points to the shadow to retrive from
*       iterPtr               - points to the current iter
*       dataSizePtr           - points to data size in bytes supposed to be processed
*                               in current iteration step
* OUTPUTS:
*       tableSizePtr          - points to the table size calculated (in bytes)
*       tablePtrPtr           - points to the table size info block
*       iterPtr               - points to the current iter
*       dataSizePtr           - points to data size in bytes left after iteration step.
*
* RETURNS:
*       GT_OK                 - on success
*
* COMMENTS:
*       None
*
*******************************************************************************/
static GT_STATUS ipLpmDbGetL3DmmFreeLists
(
    IN     PRV_CPSS_DXCH_LPM_RAM_SHADOW_STC *ipShadowPtr,
    OUT    GT_U32                           *tableSizePtr,
    OUT    GT_VOID                          **tablePtrPtr,
    INOUT  GT_UINTPTR                       *iterPtr,
    INOUT  GT_U32                           *dataSizePtr
)
{
    GT_U32 numberInternalEntiresLeft = 0;
    GT_U32 numberInternalEntriesUsed = 0;
    DMM_IP_HSU_ENTRY_STC *dmmIterEntry;
    GT_U32 i = 0;
    GT_U32 j = 0;
    GT_U32 *hsuMemPtr;
    GT_DMM_BLOCK *tempPtr;
    if (*iterPtr == 0)
    {
        /* this is the first call , so intilize */
        *iterPtr = (GT_UINTPTR)cpssOsLpmMalloc(sizeof(DMM_IP_HSU_ENTRY_STC));
        if (*iterPtr == 0)
        {
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_OUT_OF_CPU_MEM, LOG_ERROR_NO_MSG);
        }
        dmmIterEntry = (DMM_IP_HSU_ENTRY_STC*)(*iterPtr);
        dmmIterEntry->currentDmmBlockAddrInList = NULL;
        dmmIterEntry->currentFreeLinkList = 0;
        dmmIterEntry->partitionIndex = 0;
        dmmIterEntry->endOfList = 0;
        dmmIterEntry->endOfPartition = 0;
        cpssOsMemSet(dmmIterEntry->partitionArray,0,sizeof(dmmIterEntry->partitionArray));

        /* need to change logic, since for bobcat2 and above we support multi blocks mapping per octet*/
        CPSS_TBD_BOOKMARK

        /*this code need to be checked and the next code should be deleted*/

        for (j = 0, i= 0; (j <8) && (i < ipShadowPtr->numOfLpmMemories); i++, j++;)
        {
           dmmIterEntry->partitionArray[j] = (GT_DMM_PARTITION*)ipShadowPtr->lpmRamStructsMemPoolPtr[i];
        }

        /* init dmm partition array. Partition array should contain only different partition Ids */
        /*for (j = 0, i= 0; (j <8) && (i < ipShadowPtr->numOfLpmMemories); i++)
        {
           if (ipShadowPtr->lpmMemInfoArray[0][i].structsMemPool ==
               ipShadowPtr->lpmMemInfoArray[1][i].structsMemPool)
           {
               dmmIterEntry->partitionArray[j] = (GT_DMM_PARTITION*)ipShadowPtr->lpmMemInfoArray[0][i].structsMemPool;
               j++;
           }
           else
           {
               dmmIterEntry->partitionArray[j] = ((GT_DMM_PARTITION*)ipShadowPtr->lpmMemInfoArray[0][i].structsMemPool);
               dmmIterEntry->partitionArray[j+1] = (GT_DMM_PARTITION*)ipShadowPtr->lpmMemInfoArray[1][i].structsMemPool;
               j = j+2;
           }
        }
        */
    }
    else
    {
        dmmIterEntry = (DMM_IP_HSU_ENTRY_STC*)(*iterPtr);
    }

    numberInternalEntiresLeft = *dataSizePtr/(sizeof(GT_U32));

    if (numberInternalEntiresLeft == 0)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NO_RESOURCE, LOG_ERROR_NO_MSG);
    }
    /* set the pointer */
    hsuMemPtr = (GT_U32*)(*tablePtrPtr);
    i = dmmIterEntry->currentFreeLinkList;
    j = dmmIterEntry->partitionIndex;
    for (; j < 8; j ++)
    {
        if (dmmIterEntry->partitionArray[j] == NULL)
        {
            continue;
        }
        dmmIterEntry->partitionIndex = j;
        if (dmmIterEntry->endOfPartition == GT_FALSE)
        {
            for(;i < DMM_MAXIMAL_BLOCK_SIZE_ALLOWED+1;i++)
            {
                dmmIterEntry->currentFreeLinkList = i;
                if (i == 0)
                {
                    /* first link list in partition is examined. Write partition index */
                    if (numberInternalEntiresLeft > numberInternalEntriesUsed )
                    {
                        if (tableSizePtr != NULL)
                        {
                            *tableSizePtr+= sizeof(GT_U32);
                        }
                        if (*tablePtrPtr != NULL)
                        {
                            *hsuMemPtr = j;
                            hsuMemPtr++;
                        }
                        numberInternalEntriesUsed++;
                    }
                    else
                    {
                        *dataSizePtr = *dataSizePtr - numberInternalEntriesUsed * sizeof(GT_U32);
                        return GT_OK;
                    }
                }
                if (dmmIterEntry->currentDmmBlockAddrInList == NULL)
                {
                    /* new free list is in process */
                    if (dmmIterEntry->partitionArray[j]->tableOfSizePointers[i] == NULL)
                    {
                        /* no export action for empty free list */
                        continue;
                    }
                    else
                    {
                        /* write link list number-name */
                        if (numberInternalEntiresLeft > numberInternalEntriesUsed )
                        {
                            if (tableSizePtr != NULL)
                            {
                                *tableSizePtr+= sizeof(GT_U32);
                            }
                            if (*tablePtrPtr != NULL)
                            {
                                *hsuMemPtr = i;
                                hsuMemPtr++;
                            }
                            numberInternalEntriesUsed++;
                        }
                        else
                        {
                            *dataSizePtr = *dataSizePtr - numberInternalEntriesUsed * sizeof(GT_U32);
                            return GT_OK;
                        }
                        tempPtr = dmmIterEntry->partitionArray[j]->tableOfSizePointers[i];
                    }
                }
                else
                {
                    /*  free list from previuos iteration is not finished yet. Write
                        hw offset of remains link list members */
                    tempPtr = dmmIterEntry->currentDmmBlockAddrInList;
                }
                /* write hw offset of  link list members */
                if (dmmIterEntry->endOfList == GT_FALSE)
                {
                    do
                    {
                        dmmIterEntry->currentDmmBlockAddrInList = tempPtr;
                        if (numberInternalEntiresLeft > numberInternalEntriesUsed )
                        {
                            if (tableSizePtr != NULL)
                            {
                                *tableSizePtr+= sizeof(GT_U32);
                            }
                            if (*tablePtrPtr != NULL)
                            {
                                *hsuMemPtr = PRV_CPSS_DXCH_LPM_RAM_GET_LPM_OFFSET_FROM_DMM_MAC(tempPtr);
                                hsuMemPtr++;
                            }
                            numberInternalEntriesUsed++;
                        }
                        else
                        {
                            *dataSizePtr = *dataSizePtr - numberInternalEntriesUsed * sizeof(GT_U32);
                            return GT_OK;
                        }
                        tempPtr = tempPtr->nextBySizeOrPartitionPtr.nextBySize;
                    }while(tempPtr != NULL);
                }

                /* write end of the list */
                if (numberInternalEntiresLeft > numberInternalEntriesUsed )
                {
                    if (tableSizePtr != NULL)
                    {
                        *tableSizePtr+= sizeof(GT_U32);
                    }
                    if (*tablePtrPtr != NULL)
                    {
                        *hsuMemPtr = PRV_CPSS_DXCH_LPM_RAM_HSU_DMM_FREE_LINK_LIST_END_CNS;
                        hsuMemPtr++;
                    }
                    numberInternalEntriesUsed++;
                    dmmIterEntry->endOfList = GT_FALSE;
                }
                else
                {
                    dmmIterEntry->endOfList = GT_TRUE;
                    *dataSizePtr = *dataSizePtr - numberInternalEntriesUsed * sizeof(GT_U32);
                    return GT_OK;
                }
                dmmIterEntry->currentDmmBlockAddrInList = NULL;
            }
            /* small blocks export was done */
            if (dmmIterEntry->currentFreeLinkList <= DMM_MAXIMAL_BLOCK_SIZE_ALLOWED)
            {
                dmmIterEntry->currentFreeLinkList = DMM_MAXIMAL_BLOCK_SIZE_ALLOWED+1;
            }
            /* now handle big blocks list */

            /* check if big block list is exist */
            if (dmmIterEntry->partitionArray[j]->bigBlocksList != NULL)
            {
                tempPtr = dmmIterEntry->partitionArray[j]->bigBlocksList;
                if (numberInternalEntiresLeft > numberInternalEntriesUsed )
                {
                    if (tableSizePtr != NULL)
                    {
                        *tableSizePtr+= sizeof(GT_U32);
                    }
                    if (*tablePtrPtr != NULL)
                    {
                        *hsuMemPtr = PRV_CPSS_DXCH_LPM_RAM_GET_LPM_SIZE_FROM_DMM_MAC(tempPtr);
                        hsuMemPtr++;
                    }
                    dmmIterEntry->currentFreeLinkList = PRV_CPSS_DXCH_LPM_RAM_GET_LPM_SIZE_FROM_DMM_MAC(tempPtr);
                    numberInternalEntriesUsed++;
                }
                else
                {
                    *dataSizePtr = *dataSizePtr - numberInternalEntriesUsed * sizeof(GT_U32);
                    return GT_OK;
                }
                if (dmmIterEntry->currentDmmBlockAddrInList == NULL)
                {
                    /* first offset in big blocks list */
                    dmmIterEntry->currentDmmBlockAddrInList = dmmIterEntry->partitionArray[j]->bigBlocksList;
                }
                tempPtr = dmmIterEntry->currentDmmBlockAddrInList;
                do
                {
                    dmmIterEntry->currentDmmBlockAddrInList = tempPtr;
                    if (numberInternalEntiresLeft > numberInternalEntriesUsed )
                    {
                        if (tableSizePtr != NULL)
                        {
                            *tableSizePtr+= sizeof(GT_U32);
                        }
                        if (*tablePtrPtr != NULL)
                        {
                            *hsuMemPtr = PRV_CPSS_DXCH_LPM_RAM_GET_LPM_OFFSET_FROM_DMM_MAC(tempPtr);
                            hsuMemPtr++;
                        }
                        numberInternalEntriesUsed++;
                    }
                    else
                    {
                        *dataSizePtr = *dataSizePtr - numberInternalEntriesUsed * sizeof(GT_U32);
                        return GT_OK;
                    }
                    tempPtr = tempPtr->nextBySizeOrPartitionPtr.nextBySize;
                }while(tempPtr != NULL);
            }
            /* prepare free link list indexes for new partition */
            dmmIterEntry->currentDmmBlockAddrInList = NULL;
            i = 0;
            dmmIterEntry->currentFreeLinkList = 0;
        }
        /* partition is done : partition end signature is assigned */
        if (numberInternalEntiresLeft > numberInternalEntriesUsed )
        {
            if (tableSizePtr != NULL)
            {
                *tableSizePtr+= sizeof(GT_U32);
            }
            if (*tablePtrPtr != NULL)
            {
                *hsuMemPtr = PRV_CPSS_DXCH_LPM_RAM_HSU_DMM_PARTITION_END_CNS;
                hsuMemPtr++;
            }
            numberInternalEntriesUsed++;
            dmmIterEntry->endOfPartition = GT_FALSE;
        }
        else
        {
            dmmIterEntry->endOfPartition = GT_TRUE;
            *dataSizePtr = *dataSizePtr - numberInternalEntriesUsed * sizeof(GT_U32);
            return GT_OK;
        }
    }
    *tablePtrPtr = (GT_VOID*)hsuMemPtr;
    /* in case all data was processed */
    /* update the left num of entries */

    numberInternalEntiresLeft -= numberInternalEntriesUsed;

    *dataSizePtr = *dataSizePtr - numberInternalEntriesUsed * sizeof(GT_U32);
    /* free & zero the iterator */
    cpssOsLpmFree((GT_PTR)*iterPtr);
    *iterPtr = 0;

    return GT_OK;
}

/*******************************************************************************
* ipLpmDbGetL3
*
* DESCRIPTION:
*       Retrives a specifc shadow's ip Table memory Size needed and info
*
* INPUTS:
*       lpmDbPtr              - The LPM DB
*       iterationSizePtr      - points to data size in bytes supposed to be processed
*                               in current iteration
*       iterPtr               - points to the iterator, to start - set to 0.
*
* OUTPUTS:
*       tableSizePtr          - points to the table size calculated (in bytes).
*       tablePtr              - points to the table size info block.
*       iterationSizePtr      - points to data size in bytes left after iteration .
*       iterPtr               - points to the iterator, if = 0 then the operation is done.
*
* RETURNS:
*       GT_OK                 - on success
*       GT_BAD_PARAM          - on bad iterPtr
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS ipLpmDbGetL3
(
    IN    PRV_CPSS_DXCH_LPM_RAM_SHADOWS_DB_STC  *lpmDbPtr,
    OUT   GT_U32                                *tableSizePtr,
    OUT   GT_VOID                               *tablePtr,
    INOUT GT_U32                                *iterationSizePtr,
    INOUT GT_UINTPTR                            *iterPtr
)
{
    GT_STATUS                       retVal = GT_OK;
    IP_HSU_ITERATOR_STC             *currentIterPtr;

    /* if HSU process called this function currDataAmount is size in bytes.
       otherwise it is number of entries */
    GT_U32                              *currDataSizePtr = iterationSizePtr;
    GT_U32                              curentDataSizeBeforeStage;
    PRV_CPSS_DXCH_LPM_RAM_SHADOW_STC    *ipShadowPtr;

    currentIterPtr = (IP_HSU_ITERATOR_STC*)*iterPtr;

    if (currentIterPtr == NULL)
    {
        /* we need to allocate the iterator */
        currentIterPtr =
            (IP_HSU_ITERATOR_STC*)cpssOsLpmMalloc(sizeof(IP_HSU_ITERATOR_STC));
        if (currentIterPtr == NULL)
        {
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_OUT_OF_CPU_MEM, LOG_ERROR_NO_MSG);
        }
        /*reset it */
        cpssOsMemSet(currentIterPtr,0,sizeof(IP_HSU_ITERATOR_STC));

        /* set the starting stage */
        currentIterPtr->currStage = IP_HSU_VR_STAGE_E;
        currentIterPtr->magic = PRV_CPSS_EXMXPM_HSU_ITERATOR_MAGIC_NUMBER_CNC;
        *iterPtr = (GT_UINTPTR)currentIterPtr;

        /* init the table size */
        if (tableSizePtr != NULL)
        {
            *tableSizePtr = 0;
        }
    }
    else
    {
        if(currentIterPtr->magic != PRV_CPSS_EXMXPM_HSU_ITERATOR_MAGIC_NUMBER_CNC)
        {
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
        }
    }
    /* set the memory position */
    currentIterPtr->currMemPtr = tablePtr;

    for (;currentIterPtr->currShadowIdx < lpmDbPtr->numOfShadowCfg;
          currentIterPtr->currShadowIdx++)
    {
        ipShadowPtr = &lpmDbPtr->shadowArray[currentIterPtr->currShadowIdx];

        if (currentIterPtr->currStage == IP_HSU_VR_STAGE_E)
        {
            curentDataSizeBeforeStage = *currDataSizePtr;
            retVal = ipLpmDbGetL3Vr(ipShadowPtr,tableSizePtr,
                                    &currentIterPtr->currMemPtr,
                                    &currentIterPtr->currIter,
                                    currDataSizePtr);
            if ((retVal == GT_NO_RESOURCE)&&
                (*currDataSizePtr == curentDataSizeBeforeStage))
            {
                /* the stage is not done: the data size was smaller than entry in shadow */
                return GT_OK;
            }
            /* check if we finished with the phase */
            if (currentIterPtr->currIter == 0)
            {
                /* set the next stage */
                currentIterPtr->currStage = IP_HSU_UC_PREFIX_STAGE_E;
            }

            if ((retVal != GT_OK) || (*currDataSizePtr == 0))
            {
                return(retVal);
            }
        }

        if (currentIterPtr->currStage != IP_HSU_DMM_FREE_LIST_E)
        {
            for (;currentIterPtr->currProtocolStack < 2; currentIterPtr->currProtocolStack++)
            {
                if (ipShadowPtr->isIpVerIsInitialized[currentIterPtr->currProtocolStack] == GT_FALSE)
                {
                    continue;
                }

                while (currentIterPtr->currVrId < ipShadowPtr->vrfTblSize)
                {
                    /* go over all valid VR */
                    if ((ipShadowPtr->vrRootBucketArray[currentIterPtr->currVrId].valid == GT_FALSE) ||
                        (ipShadowPtr->vrRootBucketArray[currentIterPtr->currVrId].rootBucket[currentIterPtr->currProtocolStack] == NULL))
                    {
                        currentIterPtr->currVrId++;
                        continue;
                    }

                    if (currentIterPtr->currStage == IP_HSU_UC_PREFIX_STAGE_E)
                    {
                        curentDataSizeBeforeStage = *currDataSizePtr;
                        retVal = ipLpmDbGetL3UcPrefix(ipShadowPtr,
                                                      currentIterPtr->currVrId,
                                                      currentIterPtr->currProtocolStack,
                                                      tableSizePtr,
                                                      &currentIterPtr->currMemPtr,
                                                      &currentIterPtr->currIter,
                                                      currDataSizePtr);
                        if ((retVal == GT_NO_RESOURCE)&&
                            (*currDataSizePtr == curentDataSizeBeforeStage))
                        {
                            /* the stage is not done: the data size was smaller than entry in shadow */
                            return GT_OK;
                        }

                        /* check if we finished with the phase */
                        if (currentIterPtr->currIter == 0)
                        {
                            /* set the next stage */
                            currentIterPtr->currStage = IP_HSU_UC_LPM_STAGE_E;
                        }

                        if ((retVal != GT_OK) || (*currDataSizePtr == 0))
                        {
                            return(retVal);
                        }
                    }

                    if (currentIterPtr->currStage == IP_HSU_UC_LPM_STAGE_E)
                    {
                        curentDataSizeBeforeStage = *currDataSizePtr;
                        retVal = ipLpmDbGetL3UcLpm(ipShadowPtr,
                                                   currentIterPtr->currVrId,
                                                   currentIterPtr->currProtocolStack,
                                                   tableSizePtr,
                                                   &currentIterPtr->currMemPtr,
                                                   &currentIterPtr->currIter,
                                                   currDataSizePtr);
                        if ((retVal == GT_NO_RESOURCE)&&
                            (*currDataSizePtr == curentDataSizeBeforeStage))
                        {
                            /* the stage is not done: the data size was smaller than entry in shadow */
                            return GT_OK;
                        }
                        /* check if we finished with the phase */
                        if (currentIterPtr->currIter == 0)
                        {
                            /* set the next stage */
                            currentIterPtr->currStage = IP_HSU_MC_PREFIX_STAGE_E;
                        }
                        if ((retVal != GT_OK) || (*currDataSizePtr == 0))
                        {
                            return(retVal);
                        }
                    }

                    if (currentIterPtr->currStage == IP_HSU_MC_PREFIX_STAGE_E)
                    {
                        curentDataSizeBeforeStage = *currDataSizePtr;
                        retVal = ipLpmDbGetL3McRoutes(ipShadowPtr,
                                                      currentIterPtr->currVrId,
                                                      currentIterPtr->currProtocolStack,
                                                      tableSizePtr,
                                                      &currentIterPtr->currMemPtr,
                                                      &currentIterPtr->currIter,
                                                      currDataSizePtr);
                        if ((retVal == GT_NO_RESOURCE)&&
                            (*currDataSizePtr == curentDataSizeBeforeStage))
                        {
                            /* the stage is not done: the data size was smaller than entry in shadow */
                            return GT_OK;
                        }
                        /* check if we finished with the phase */
                        if (currentIterPtr->currIter == 0)
                        {
                            /* set the next stage */
                            currentIterPtr->currStage = IP_HSU_MC_LPM_STAGE_E;
                        }

                        if ((retVal != GT_OK) || (*currDataSizePtr == 0))
                        {
                            return(retVal);
                        }
                    }

                    if (currentIterPtr->currStage == IP_HSU_MC_LPM_STAGE_E)
                    {
                        curentDataSizeBeforeStage = *currDataSizePtr;
                        retVal = ipLpmDbGetL3McLpm(ipShadowPtr,
                                                   currentIterPtr->currVrId,
                                                   currentIterPtr->currProtocolStack,
                                                   tableSizePtr,
                                                   &currentIterPtr->currMemPtr,
                                                   &currentIterPtr->currIter,
                                                   currDataSizePtr);
                        if ((retVal == GT_NO_RESOURCE)&&
                            (*currDataSizePtr == curentDataSizeBeforeStage))
                        {
                            /* the stage is not done: the data size was smaller than entry in shadow */
                            return GT_OK;
                        }
                        /* check if we finished with the phase */
                        if (currentIterPtr->currIter == 0)
                        {
                            /* set the next stage */
                            currentIterPtr->currStage = IP_HSU_UC_PREFIX_STAGE_E;
                            /* this is the end for this vrid */
                            currentIterPtr->currVrId++;
                        }
                        if (retVal != GT_OK)
                        {
                            return(retVal);
                        }
                   }
                }
                /* we finished with this protocol stack zero the vrid */
                currentIterPtr->currVrId = 0;
            }
        }

        /* the last stage is IP_HSU_DMM_FREE_LIST_E for this shadow*/
        currentIterPtr->currStage = IP_HSU_DMM_FREE_LIST_E;
        if (currentIterPtr->currStage == IP_HSU_DMM_FREE_LIST_E)
        {
            curentDataSizeBeforeStage = *currDataSizePtr;
            retVal = ipLpmDbGetL3DmmFreeLists(ipShadowPtr,
                                              tableSizePtr,
                                              &currentIterPtr->currMemPtr,
                                              &currentIterPtr->currIter,
                                              currDataSizePtr);
            if ((retVal == GT_NO_RESOURCE)&&
                (*currDataSizePtr == curentDataSizeBeforeStage))
            {
                /* the stage is not done: the data size was smaller than entry in shadow */
                return GT_OK;
            }
            /* check if we finished with the last phase */
            if (currentIterPtr->currIter == 0)
            {
                /* set the start stage */
                currentIterPtr->currStage = IP_HSU_VR_STAGE_E;
            }

            if ((retVal != GT_OK)||(currentIterPtr->currIter != 0))
            {
                return(retVal);
            }
        }
        /* we finished with the shadow , set the starting stage */
        currentIterPtr->currStage = IP_HSU_VR_STAGE_E;
    }

    /* we reached the end free the iterator */
    cpssOsLpmFree(currentIterPtr);
    *iterPtr = 0;

    return (GT_OK);
}

/*******************************************************************************
* ipLpmDbSetL3Vr
*
* DESCRIPTION:
*       Retrives the valid virtual routers
*
* INPUTS:
*       ipShadowPtr           - points to the shadow to retrieve from
*       tablePtrPtr           - the table size info block
*       iterPtr               - points to the current iter
*       dataSizePtr           - points to data size in bytes supposed to be processed
*                               in current iteration
*
* OUTPUTS:
*       iterPtr               - points to the current iter
*       dataSizePtr           - points to data size in bytes left after iteration step.
*
* RETURNS:
*       GT_OK                 - on success
*
* COMMENTS:
*       None
*
*******************************************************************************/
static GT_STATUS ipLpmDbSetL3Vr
(
    IN     PRV_CPSS_DXCH_LPM_RAM_SHADOW_STC     *ipShadowPtr,
    IN     GT_VOID                              **tablePtrPtr,
    INOUT  GT_UINTPTR                           *iterPtr,
    INOUT  GT_U32                               *dataSizePtr
)
{
    GT_STATUS   retVal = GT_OK;
    GT_U32      numOfEntries = 0;
    GT_U32      numOfEntriesLeft = 0;

    PRV_CPSS_DXCH_LPM_RAM_VR_INFO_STC *vrEntry;

    numOfEntriesLeft = *dataSizePtr/(sizeof(PRV_CPSS_DXCH_LPM_RAM_VR_INFO_STC));

    if (numOfEntriesLeft == 0)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NO_RESOURCE, LOG_ERROR_NO_MSG);
    }

    /* set the pointer */
    vrEntry = (PRV_CPSS_DXCH_LPM_RAM_VR_INFO_STC*)(*tablePtrPtr);

    while ((vrEntry->isLast == GT_FALSE) && (numOfEntries < numOfEntriesLeft))
    {
        /* setting the VR automatically set the default UC and MC prefixes in the lpm DB */
        retVal = lpmVirtualRouterSet(vrEntry->vrId,
                                     vrEntry->defUcRouteEntrey[CPSS_IP_PROTOCOL_IPV4_E],
                                     vrEntry->defUcRouteEntrey[CPSS_IP_PROTOCOL_IPV6_E],
                                     vrEntry->defMcRouteEntrey[CPSS_IP_PROTOCOL_IPV4_E],
                                     vrEntry->defMcRouteEntrey[CPSS_IP_PROTOCOL_IPV6_E],
                                     vrEntry->protocolStack,
                                     vrEntry->vrIpUcSupport,
                                     vrEntry->vrIpMcSupport,
                                     vrEntry->defIpv6McRuleIndex,
                                     vrEntry->vrIpv6McPclId,
                                     ipShadowPtr,
                                     GT_FALSE);
        if (retVal != GT_OK)
        {
            return (retVal);
        }

        /* advance the pointer */
        vrEntry++;

        numOfEntries++;
    }

    /* check if we finished iterating*/
    if ( (vrEntry->isLast == GT_TRUE)&& (numOfEntriesLeft > numOfEntries))
    {
        /* zero the iterator */
        *iterPtr = 0;

        /* set the ptr */
        *tablePtrPtr = (GT_VOID *)(vrEntry + 1);
         numOfEntries++;
    }
    else
    {
         /* just indicate we didn't finish using a fake iterator */
        *iterPtr = 0xff;
        *tablePtrPtr = (GT_VOID *)vrEntry;
    }

    /* update the left num of entries */
    numOfEntriesLeft -= numOfEntries;

    *dataSizePtr = *dataSizePtr - numOfEntries *(sizeof(PRV_CPSS_DXCH_LPM_RAM_VR_INFO_STC));
    return (retVal);
}

/*******************************************************************************
* ipLpmDbSetL3UcPrefix
*
* DESCRIPTION:
*       Set the data needed for core uc prefix shadow reconstruction used
*       after HSU.
*
* INPUTS:
*       ipShadowPtr           - points to the shadow to set in
*       vrId                  - the virtual router id
*       protocolStack         - type of ip protocol stack to work on.
*       tablePtrPtr           - points to the table size info block
*       iterPtr               - points to the current iter
*       dataSizePtr           - points to data size in bytes supposed to be processed
*                               in current iteration
*
* OUTPUTS:
*       iterPtr               - points to the current iter
*       dataSizePtr           - points to data size in bytes left after iteration step.
*
* RETURNS:
*       GT_OK                 - on success
*
* COMMENTS:
*       None
*
*******************************************************************************/
static GT_STATUS ipLpmDbSetL3UcPrefix
(
    IN     PRV_CPSS_DXCH_LPM_RAM_SHADOW_STC *ipShadowPtr,
    IN     GT_U32                           vrId,
    IN     CPSS_IP_PROTOCOL_STACK_ENT       protocolStack,
    IN     GT_VOID                          **tablePtrPtr,
    INOUT  GT_UINTPTR                       *iterPtr,
    INOUT  GT_U32                           *dataSizePtr
)
{
    PRV_CPSS_DXCH_LPM_ROUTE_ENTRY_POINTER_STC *nextHopPointerPtr;
    GT_STATUS retVal = GT_OK;
    GT_U8 *ipAddr;
    GT_U32 ipPrefix;
    GT_U32 numOfEntries = 0;
    GT_U32 numOfEntriesLeft = 0;
    PRV_CPSS_DXCH_LPM_RAM_IPV6_UC_PREFIX_BULK_OPERATION_STC *ipv6Prefix;
    PRV_CPSS_DXCH_LPM_RAM_IPV4_UC_PREFIX_BULK_OPERATION_STC *ipv4Prefix;
    GT_BOOL isLast;

    /* set the pointer */
    ipv4Prefix = (PRV_CPSS_DXCH_LPM_RAM_IPV4_UC_PREFIX_BULK_OPERATION_STC*)(*tablePtrPtr);
    ipv6Prefix = (PRV_CPSS_DXCH_LPM_RAM_IPV6_UC_PREFIX_BULK_OPERATION_STC*)(*tablePtrPtr);

    /* start with uc prefixes */
    isLast = (protocolStack == CPSS_IP_PROTOCOL_IPV4_E)?
             (ipv4Prefix->prefixLen == 0xFF):(ipv6Prefix->prefixLen == 0xFF);


     if (protocolStack == CPSS_IP_PROTOCOL_IPV4_E)
         numOfEntriesLeft = *dataSizePtr/(sizeof(PRV_CPSS_DXCH_LPM_RAM_IPV4_UC_PREFIX_BULK_OPERATION_STC));
     else
         numOfEntriesLeft = *dataSizePtr/(sizeof(PRV_CPSS_DXCH_LPM_RAM_IPV6_UC_PREFIX_BULK_OPERATION_STC));

    if (numOfEntriesLeft == 0)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NO_RESOURCE, LOG_ERROR_NO_MSG);
    }


    while ((!isLast) && (numOfEntries < numOfEntriesLeft))
    {
        /* copy the prefix info, start with ip address*/
        if (protocolStack == CPSS_IP_PROTOCOL_IPV4_E)
        {
            ipAddr = ipv4Prefix->ipAddr.arIP;
            ipPrefix = ipv4Prefix->prefixLen;
            nextHopPointerPtr = &ipv4Prefix->nextHopPointer;
            vrId = ipv4Prefix->vrId;

            /* advance the pointer */
            ipv4Prefix = ipv4Prefix + 1;

            isLast = (ipv4Prefix->prefixLen == 0xFF);

        }
        else
        {
            ipAddr = ipv6Prefix->ipAddr.arIP;
            ipPrefix = (GT_U8)ipv6Prefix->prefixLen;
            nextHopPointerPtr = &ipv6Prefix->nextHopPointer;
            vrId = ipv6Prefix->vrId;

            /* advance the pointer */
            ipv6Prefix = ipv6Prefix + 1;

            isLast = (ipv6Prefix->prefixLen == 0xFF);

        }

        /* The default UC was already defined in the call to ipLpmDbSetL3Vr */
        if (ipPrefix != 0)
        {
            retVal = prvCpssDxChLpmRamUcEntryAdd(vrId,ipAddr,ipPrefix,nextHopPointerPtr,
                                                 PRV_CPSS_DXCH_LPM_RAM_TRIE_INSERT_SDW_ONLY_MODE_E,
                                                 GT_FALSE,protocolStack,ipShadowPtr);
        }

        if (retVal != GT_OK)
        {
            return (retVal);
        }

        numOfEntries++;

    }

    /* check if we finished */
    if ( (isLast == GT_TRUE) && (numOfEntries < numOfEntriesLeft))
    {
        /* advance the pointer */
        *tablePtrPtr = (protocolStack == CPSS_IP_PROTOCOL_IPV4_E)?
            (GT_VOID *)(ipv4Prefix + 1) : (GT_VOID *)(ipv6Prefix + 1);
        numOfEntries++;
        /* just indicate we finished using a fake iterator */
        *iterPtr = 0;
    }
    else
    {
        /* just indicate we didn't finish using a fake iterator */
        *iterPtr = 0xff;

        /* and record the pointer */
        *tablePtrPtr = (protocolStack == CPSS_IP_PROTOCOL_IPV4_E)?
            (GT_VOID *)ipv4Prefix : (GT_VOID *)ipv6Prefix;
    }

    /* update the num of entries left */
    numOfEntriesLeft -= numOfEntries;

    if (protocolStack == CPSS_IP_PROTOCOL_IPV4_E)
        *dataSizePtr = *dataSizePtr - numOfEntries * (sizeof(PRV_CPSS_DXCH_LPM_RAM_IPV4_UC_PREFIX_BULK_OPERATION_STC));
    else
        *dataSizePtr = *dataSizePtr - numOfEntries * (sizeof(PRV_CPSS_DXCH_LPM_RAM_IPV6_UC_PREFIX_BULK_OPERATION_STC));
    return (retVal);
}

/*******************************************************************************
* ipLpmDbSetL3UcLpm
*
* DESCRIPTION:
*       Set the data needed for core uc LPM shadow reconstruction used
*       after HSU.
*
* INPUTS:
*       ipShadowPtr           - points to the shadow to set in
*       vrId                  - the virtual router id
*       protocolStack         - type of ip protocol stack to work on.
*       tablePtrPtr           - points to the table size info block
*       iterPtr               - points to the current iter
*       dataSizePtr           - points to data size in bytes supposed to be processed
*
* OUTPUTS:
*       iterPtr               - points to the current iter
*       dataSizePtr           - points to data size in bytes left after iteration step.
*
* RETURNS:
*       GT_OK                 - on success
*
* COMMENTS:
*       None
*
*******************************************************************************/
static GT_STATUS ipLpmDbSetL3UcLpm
(
    IN     PRV_CPSS_DXCH_LPM_RAM_SHADOW_STC *ipShadowPtr,
    IN     GT_U32                           vrId,
    IN     CPSS_IP_PROTOCOL_STACK_ENT       protocolStack,
    IN     GT_VOID                          **tablePtrPtr,
    INOUT  GT_UINTPTR                       *iterPtr,
    INOUT  GT_U32                           *dataSizePtr
)
{
    GT_STATUS retVal = GT_OK;
    PRV_CPSS_DXCH_LPM_RAM_RANGE_SHADOW_STC tmpRootRange;

    GT_U32 memAllocIndex = 0;
    GT_U32 numOfEntriesLeft = 0;

    PRV_CPSS_DXCH_LPM_RAM_MEM_ALLOC_INFO_STC *memAllocInfo = NULL;

    numOfEntriesLeft = *dataSizePtr/(sizeof(PRV_CPSS_DXCH_LPM_RAM_MEM_ALLOC_INFO_STC));

    if (numOfEntriesLeft == 0)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NO_RESOURCE, LOG_ERROR_NO_MSG);
    }

    /* check uc prefixes */
    if (ipShadowPtr->vrRootBucketArray[vrId].rootBucket[protocolStack] == NULL)
    {
        /* no uc prefixes */
        return GT_OK;
    }
    /* set the pointer for lpm traverse */
    memAllocInfo = (PRV_CPSS_DXCH_LPM_RAM_MEM_ALLOC_INFO_STC*)*tablePtrPtr;

    memAllocIndex = 0;

    tmpRootRange.lowerLpmPtr.nextBucket =
    ipShadowPtr->vrRootBucketArray[vrId].rootBucket[protocolStack];
    tmpRootRange.pointerType =
    (GT_U8)(ipShadowPtr->vrRootBucketArray[vrId].rootBucketType[protocolStack]);
    tmpRootRange.next = NULL;
    retVal = prvCpssDxChLpmRamMemTraverse(PRV_CPSS_DXCH_LPM_RAM_MEM_TRAVERSE_SET_WITH_PCL_E,
                                          memAllocInfo,
                                          &memAllocIndex,
                                          numOfEntriesLeft,
                                          &tmpRootRange,
                                          GT_FALSE,
                                          ipShadowPtr->ipUcSearchMemArrayPtr[protocolStack],
                                          iterPtr);

    /* update the left num of entries */
    numOfEntriesLeft = numOfEntriesLeft - memAllocIndex;

    *dataSizePtr = *dataSizePtr - memAllocIndex * (sizeof(PRV_CPSS_DXCH_LPM_RAM_MEM_ALLOC_INFO_STC));

    /* advance the pointer */
    *tablePtrPtr = (GT_VOID*)(&memAllocInfo[memAllocIndex]);

    return (retVal);
}

/*******************************************************************************
* ipLpmDbSetL3McRoutes
*
* DESCRIPTION:
*       Set the data needed for core shadow reconstruction used after HSU.
*
* INPUTS:
*       ipShadowPtr           - points to the shadow to set in
*       vrId                  - the virtual router id
*       protocolStack         - type of ip protocol stack to work on.
*       tablePtrPtr           - points to the table size info block
*       iterPtr               - points to the current iter
*       dataSizePtr           - points to data size in bytes supposed to be processed
*                               in current iteration
*
* OUTPUTS:
*       iterPtr               - points to the current iter
*       dataSizePtr           - points to data size in bytes left after iteration step.
*
* RETURNS:
*       GT_OK                 - on success
*
* COMMENTS:
*       None
*
*******************************************************************************/
static GT_STATUS ipLpmDbSetL3McRoutes
(
    IN     PRV_CPSS_DXCH_LPM_RAM_SHADOW_STC     *ipShadowPtr,
    IN     GT_U32                               vrId,
    IN     CPSS_IP_PROTOCOL_STACK_ENT           protocolStack,
    IN     GT_VOID                              **tablePtrPtr,
    INOUT  GT_UINTPTR                           *iterPtr,
    INOUT  GT_U32                               *dataSizePtr
)
{
    GT_STATUS                               retVal = GT_OK;
    GT_U8                                   *groupIp = NULL;
    GT_U8                                   *srcAddr = NULL;
    GT_U32                                  groupPrefix,srcPrefix;

    /* regarding group scope, here we give a false one, it will
       updated as part of the traverse function on the ipv6 groups */
    CPSS_IPV6_PREFIX_SCOPE_ENT          groupScope =
                                            CPSS_IPV6_PREFIX_SCOPE_LINK_LOCAL_E;
    CPSS_EXMXPM_IP_MC_ADDRESS_TYPE_ENT  addressType =
                                            CPSS_EXMXPM_IP_MC_REGULAR_ADDRESS_TYPE_E;

    PRV_CPSS_DXCH_LPM_ROUTE_ENTRY_POINTER_STC *mcEntry;

    GT_U32 numOfEntries = 0;
    GT_U32 numOfEntriesLeft = 0;

    IPV6_HSU_MC_ROUTE_ENTRY_STC *ipv6McRoute;
    IPV4_HSU_MC_ROUTE_ENTRY_STC *ipv4McRoute;

    GT_BOOL isLast;

    ipv4McRoute = (IPV4_HSU_MC_ROUTE_ENTRY_STC*)*tablePtrPtr;
    ipv6McRoute = (IPV6_HSU_MC_ROUTE_ENTRY_STC*)*tablePtrPtr;

    isLast = (protocolStack == CPSS_IP_PROTOCOL_IPV4_E)?
             ipv4McRoute->last:ipv6McRoute->last;

     if (protocolStack == CPSS_IP_PROTOCOL_IPV4_E)
         numOfEntriesLeft = *dataSizePtr/(sizeof(IPV4_HSU_MC_ROUTE_ENTRY_STC));
     else
         numOfEntriesLeft = *dataSizePtr/(sizeof(IPV6_HSU_MC_ROUTE_ENTRY_STC));

    if (numOfEntriesLeft == 0)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NO_RESOURCE, LOG_ERROR_NO_MSG);
    }
    if (protocolStack == CPSS_IP_PROTOCOL_IPV4_E)
    {
        groupIp =  (GT_U8 *)cpssOsMalloc(sizeof(GT_U8)*PRV_CPSS_DXCH_LPM_NUM_OF_OCTETS_IN_IPV4_PROTOCOL_CNS);
        srcAddr =  (GT_U8 *)cpssOsMalloc(sizeof(GT_U8)*PRV_CPSS_DXCH_LPM_NUM_OF_OCTETS_IN_IPV4_PROTOCOL_CNS);
    }
    else
    {
        groupIp =  (GT_U8 *)cpssOsMalloc(sizeof(GT_U8)*PRV_CPSS_DXCH_LPM_NUM_OF_OCTETS_IN_IPV6_PROTOCOL_CNS);
        srcAddr =  (GT_U8 *)cpssOsMalloc(sizeof(GT_U8)*PRV_CPSS_DXCH_LPM_NUM_OF_OCTETS_IN_IPV6_PROTOCOL_CNS);
    }
     cpssOsMemSet(groupIp, 0, sizeof(groupIp));
     cpssOsMemSet(srcAddr, 0, sizeof(srcAddr));


    while ((isLast == GT_FALSE) && (numOfEntries < numOfEntriesLeft))
    {
        if (protocolStack == CPSS_IP_PROTOCOL_IPV4_E)
        {
            cpssOsMemCpy(groupIp,ipv4McRoute->mcGroup.arIP,sizeof(GT_U8) * PRV_CPSS_DXCH_LPM_NUM_OF_OCTETS_IN_IPV4_PROTOCOL_CNS);
            groupPrefix = (ipv4McRoute->mcGroup.u32Ip == 0)?0:32;

            cpssOsMemCpy(srcAddr,ipv4McRoute->ipSrc.arIP,sizeof(GT_U8) * PRV_CPSS_DXCH_LPM_NUM_OF_OCTETS_IN_IPV4_PROTOCOL_CNS);
            srcPrefix = ipv4McRoute->ipSrcPrefixlength;

            vrId = ipv4McRoute->vrId;
            mcEntry = &ipv4McRoute->mcRoutePointer;

            ipv4McRoute++;

            isLast = ipv4McRoute->last;
        }
        else
        {
            cpssOsMemCpy(groupIp,ipv6McRoute->mcGroup.arIP,sizeof(GT_U8) * PRV_CPSS_DXCH_LPM_NUM_OF_OCTETS_IN_IPV6_PROTOCOL_CNS);
            groupPrefix = ((ipv6McRoute->mcGroup.u32Ip[0] == 0) &&
                           (ipv6McRoute->mcGroup.u32Ip[1] == 0) &&
                           (ipv6McRoute->mcGroup.u32Ip[2] == 0) &&
                           (ipv6McRoute->mcGroup.u32Ip[3] == 0))?0:128;

            cpssOsMemCpy(srcAddr,ipv6McRoute->ipSrc.arIP,sizeof(GT_U8) * PRV_CPSS_DXCH_LPM_NUM_OF_OCTETS_IN_IPV6_PROTOCOL_CNS);
            srcPrefix = ipv6McRoute->ipSrcPrefixlength;

            vrId = ipv6McRoute->vrId;
            mcEntry = &ipv6McRoute->mcRoutePointer;

            ipv6McRoute++;

            isLast = ipv6McRoute->last;
        }

        /* we are overriding an entry that exists in core*/
        retVal = prvCpssDxChLpmRamMcEntryAdd(vrId,groupIp,groupPrefix,addressType,
                                             0,srcAddr,srcPrefix,
                                             mcEntry,GT_FALSE,
                                             PRV_CPSS_DXCH_LPM_RAM_TRIE_INSERT_SDW_ONLY_MODE_E,
                                             protocolStack,ipShadowPtr);
        if (retVal != GT_OK)
        {
            cpssOsFree(groupIp);
            cpssOsFree(srcAddr);
            return (retVal);
        }

        numOfEntries++;

    }

    /* check if we finished */
    if ( (isLast == GT_TRUE) && (numOfEntries < numOfEntriesLeft) )
    {
        /* just indicate we finished using a fake iterator */
        *iterPtr = 0;

        /* advance the pointer */
        *tablePtrPtr = (protocolStack == CPSS_IP_PROTOCOL_IPV4_E)?
            (GT_VOID *)(ipv4McRoute + 1) : (GT_VOID *)(ipv6McRoute + 1);
        numOfEntries++;
    }
    else
    {
        /* just indicate we didn't finish using a fake iterator */
        *iterPtr = 0xff;

        /* and record the pointer */
        *tablePtrPtr = (protocolStack == CPSS_IP_PROTOCOL_IPV4_E)?
            (GT_VOID *)ipv4McRoute : (GT_VOID *)ipv6McRoute;
    }

    /* update the num of entries left */
    numOfEntriesLeft -= numOfEntries;

     if (protocolStack == CPSS_IP_PROTOCOL_IPV4_E)
         *dataSizePtr = *dataSizePtr - numOfEntries * (sizeof(IPV4_HSU_MC_ROUTE_ENTRY_STC));
     else
         *dataSizePtr = *dataSizePtr - numOfEntries * (sizeof(IPV6_HSU_MC_ROUTE_ENTRY_STC));
    cpssOsFree(groupIp);
    cpssOsFree(srcAddr);
    return (retVal);
}

/*******************************************************************************
* ipLpmDbSetL3McLpm
*
* DESCRIPTION:
*       Set the data needed for core shadow reconstruction used after HSU.
*
* INPUTS:
*       ipShadowPtr           - points to the shadow to set in
*       vrId                  - the virtual router id
*       protocolStack         - type of ip protocol stack to work on.
*       tablePtrPtr           - points to the table size info block
*       iterPtr               - points to the current iter
*       dataSizePtr           - points to data size in bytes supposed to be processed
*
* OUTPUTS:
*       iterPtr               - points to the current iter
*       dataSizePtr           - points to data size in bytes left after iteration step.
*
* RETURNS:
*       GT_OK                 - on success
*
* COMMENTS:
*       None
*
*******************************************************************************/
static GT_STATUS ipLpmDbSetL3McLpm
(
    IN     PRV_CPSS_DXCH_LPM_RAM_SHADOW_STC     *ipShadowPtr,
    IN     GT_U32                               vrId,
    IN     CPSS_IP_PROTOCOL_STACK_ENT           protocolStack,
    IN     GT_VOID                              **tablePtrPtr,
    INOUT  GT_UINTPTR                           *iterPtr,
    INOUT  GT_U32                               *dataSizePtr
)
{
    GT_STATUS                                   retVal = GT_OK;
    PRV_CPSS_EXMXPM_IPV6MC_PCL_ENTRY_STC        ipv6McPclEntry,*ipv6McPclEntryPtr;
    GT_U32                                      memAllocIndex = 0;
    GT_U32                                      numOfEntriesLeft = 0;
    PRV_CPSS_DXCH_LPM_RAM_MEM_ALLOC_INFO_STC    *memAllocInfo = NULL;
    /* set the pointer for lpm traverse */

    /* check if mc prefixes are exist */
    if (protocolStack == CPSS_IP_PROTOCOL_IPV4_E )
    {
        if(ipShadowPtr->vrRootBucketArray[vrId].mcRootBucket[CPSS_IP_PROTOCOL_IPV4_E] == NULL)
        {
            /* no ipv4 mc prefixes */
            return GT_OK;
        }
    }
    if (protocolStack == CPSS_IP_PROTOCOL_IPV6_E )
    {
        if(ipShadowPtr->vrRootBucketArray[vrId].mcRootBucket[CPSS_IP_PROTOCOL_IPV6_E] == NULL)
        {
            /* no ipv4 mc prefixes */
            return GT_OK;
        }
    }
    memAllocInfo = (PRV_CPSS_DXCH_LPM_RAM_MEM_ALLOC_INFO_STC*)*tablePtrPtr;
    memAllocIndex = 0;

    numOfEntriesLeft = *dataSizePtr/(sizeof(PRV_CPSS_DXCH_LPM_RAM_MEM_ALLOC_INFO_STC));

    if (numOfEntriesLeft == 0)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NO_RESOURCE, LOG_ERROR_NO_MSG);
    }
    /* move on to mc LPM*/
    retVal = prvCpssDxChLpmRamMcTraverse(PRV_CPSS_DXCH_LPM_RAM_MEM_TRAVERSE_SET_WITH_PCL_E,
                                         memAllocInfo,&memAllocIndex,
                                         numOfEntriesLeft,vrId,ipShadowPtr,
                                         iterPtr);
    /* advance the pointer */
    *tablePtrPtr = (GT_VOID*)(&memAllocInfo[memAllocIndex]);

    /* update the left num of entries */
    numOfEntriesLeft = numOfEntriesLeft - memAllocIndex;

    *dataSizePtr = *dataSizePtr - memAllocIndex * (sizeof(PRV_CPSS_DXCH_LPM_RAM_MEM_ALLOC_INFO_STC));

    return (retVal);
}

/*******************************************************************************
* ipLpmDbSetL3DmmFreeLists
*
* DESCRIPTION:
*       Set needed dmm free lists in dmm manager .
*
* INPUTS:
*       ipShadowPtr           - points to the shadow to retrive from
*       iterPtr               - points to the current iter
*       dataSizePtr           - points to data size in bytes supposed to be processed
*                               in current iteration step
* OUTPUTS:
*       tablePtrPtr           - points to the table size info block
*       iterPtr               - points to the current iter
*       dataSizePtr           - points to data size in bytes left after iteration step.
*
* RETURNS:
*       GT_OK                 - on success
*
* COMMENTS:
*       None
*
*******************************************************************************/
static GT_STATUS ipLpmDbSetL3DmmFreeLists
(
    IN     PRV_CPSS_DXCH_LPM_RAM_SHADOW_STC *ipShadowPtr,
    OUT    GT_VOID                          **tablePtrPtr,
    INOUT  GT_UINTPTR                       *iterPtr,
    INOUT  GT_U32                           *dataSizePtr
)
{
    GT_U32 numberInternalEntiresLeft = 0;
    GT_U32 numberInternalEntriesUsed = 0;
    DMM_IP_HSU_ENTRY_STC *dmmIterEntry;
    GT_DMM_BLOCK *tempDmmPtr = NULL;
    GT_DMM_BLOCK *foundDmmBlock = NULL;
    GT_DMM_BLOCK *tempNextBySize = NULL;
    GT_DMM_BLOCK *tempPrevBySize = NULL;
    GT_DMM_BLOCK *tempCurrentNextBySize = NULL;
    GT_BOOL updateSmallFreeListArray = GT_FALSE;
    GT_BOOL updateBigFreeList = GT_FALSE;
    GT_U32 tempListName = 0;
    GT_U32 i = 0;
    GT_U32 j = 0;
    GT_U32 *hsuMemPtr;
    GT_U32 data;

    if (*iterPtr == 0)
    {
        /* this is the first call , so intilize */
        *iterPtr = (GT_UINTPTR)cpssOsLpmMalloc(sizeof(DMM_IP_HSU_ENTRY_STC));
        if (*iterPtr == 0)
        {
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_OUT_OF_CPU_MEM, LOG_ERROR_NO_MSG);
        }
        dmmIterEntry = (DMM_IP_HSU_ENTRY_STC*)(*iterPtr);
        dmmIterEntry->currentDmmBlockAddrInList = NULL;
        dmmIterEntry->currentFreeLinkList = 0xffffffff;
        dmmIterEntry->partitionIndex = 0;
        dmmIterEntry->endOfList = 0;
        dmmIterEntry->endOfPartition = GT_TRUE;
        cpssOsMemSet(dmmIterEntry->partitionArray,0,sizeof(dmmIterEntry->partitionArray));

        /* need to change logic, since for bobcat2 and above we support multi blocks mapping per octet*/
        CPSS_TBD_BOOKMARK

        /*this code need to be checked and the next code should be deleted*/

        for (j = 0, i= 0; (j <8) && (i < ipShadowPtr->numOfLpmMemories); i++, j++;)
        {
           dmmIterEntry->partitionArray[j] = (GT_DMM_PARTITION*)ipShadowPtr->lpmRamStructsMemPoolPtr[i];
        }

        /* init dmm partition array. Partition array should contain only different partition Ids */
        /*for (j = 0, i= 0; (j <8) && (i < ipShadowPtr->numOfLpmMemories); i++)
        {
           if (ipShadowPtr->lpmMemInfoArray[0][i].structsMemPool ==
               ipShadowPtr->lpmMemInfoArray[1][i].structsMemPool)
           {
               dmmIterEntry->partitionArray[j] = (GT_DMM_PARTITION*)ipShadowPtr->lpmMemInfoArray[0][i].structsMemPool;
               j++;
           }
           else
           {
               dmmIterEntry->partitionArray[j] = (GT_DMM_PARTITION*)ipShadowPtr->lpmMemInfoArray[0][i].structsMemPool;
               dmmIterEntry->partitionArray[j+1] = (GT_DMM_PARTITION*)ipShadowPtr->lpmMemInfoArray[1][i].structsMemPool;
               j = j+2;
           }
        }
        */
    }
    else
    {
        dmmIterEntry = (DMM_IP_HSU_ENTRY_STC*)(*iterPtr);
    }

    numberInternalEntiresLeft = *dataSizePtr/(sizeof(GT_U32));

    if (numberInternalEntiresLeft == 0)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NO_RESOURCE, LOG_ERROR_NO_MSG);
    }
    /* set the pointer */

    hsuMemPtr = (GT_U32*)(*tablePtrPtr);
    j = dmmIterEntry->partitionIndex;
    for (; j < 8; j ++)
    {
        if (dmmIterEntry->partitionArray[j] == NULL)
        {
            continue;
        }

        /* partition index in the begining of each partition data */
        /* first reading of new partition block */
        if (dmmIterEntry->endOfPartition == GT_TRUE)
        {
            dmmIterEntry->partitionIndex = j;
            if (numberInternalEntiresLeft > numberInternalEntriesUsed )
            {
                data = *hsuMemPtr;
                hsuMemPtr++;
                numberInternalEntriesUsed++;
                dmmIterEntry->endOfPartition = GT_FALSE;
            }
            else
            {
                *dataSizePtr = *dataSizePtr - numberInternalEntriesUsed * sizeof(GT_U32);
                return GT_OK;
            }
            if (dmmIterEntry->partitionIndex != data)
            {
                /* export and import are not synchronized */
                CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_STATE, LOG_ERROR_NO_MSG);
            }
        }
        while (1)
        {
            if (numberInternalEntiresLeft > numberInternalEntriesUsed )
            {
                data = *hsuMemPtr;
                hsuMemPtr++;
                numberInternalEntriesUsed++;
            }
            else
            {
                *dataSizePtr = *dataSizePtr - numberInternalEntriesUsed * sizeof(GT_U32);
                return GT_OK;
            }
            if (data == PRV_CPSS_DXCH_LPM_RAM_HSU_DMM_PARTITION_END_CNS)
            {
                /* partition is done */
                dmmIterEntry->endOfPartition = GT_TRUE;
                dmmIterEntry->currentFreeLinkList = 0xffffffff;
                break;
            }

            if (data == PRV_CPSS_DXCH_LPM_RAM_HSU_DMM_FREE_LINK_LIST_END_CNS)
            {
                /* this list was finished */
                dmmIterEntry->currentFreeLinkList = 0xffffffff;
                continue;
            }
            /* if iteration mode is activated it is importent to understand when
               link list name is read and when hw offset is read */
            if (dmmIterEntry->currentFreeLinkList == 0xffffffff)
            {
                dmmIterEntry->currentFreeLinkList = data;
                dmmIterEntry->currentDmmBlockAddrInList = NULL;
            }
            else
            {
                tempListName = dmmIterEntry->currentFreeLinkList;
                /* hw offset was read -- set it in existing list */
                if (dmmIterEntry->currentDmmBlockAddrInList == NULL)
                {
                    /* first hw offset in this list */
                    if (tempListName <= DMM_MAXIMAL_BLOCK_SIZE_ALLOWED)
                    {
                        updateSmallFreeListArray = GT_TRUE;
                        dmmIterEntry->currentDmmBlockAddrInList = dmmIterEntry->partitionArray[dmmIterEntry->partitionIndex]->tableOfSizePointers[tempListName];
                    }
                    else
                    {
                        /* big blocks */
                        updateBigFreeList = GT_TRUE;
                        dmmIterEntry->currentDmmBlockAddrInList = dmmIterEntry->partitionArray[dmmIterEntry->partitionIndex]->bigBlocksList;
                    }
                }
                /* find the element with received hw offset in current list */

                if (dmmIterEntry->currentDmmBlockAddrInList == NULL)
                {
                    CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_STATE, LOG_ERROR_NO_MSG);
                }
                /* except of case when  first hw offset in the list was read
                  currentDmmBlockAddrInList is already processed so we start
                  from the next. If next dmm block == NULL, it is bad state:
                  list is done but end of list still wasn't arrived  */

                if ( (updateSmallFreeListArray == GT_TRUE) || (updateBigFreeList == GT_TRUE) )
                {
                    tempDmmPtr = dmmIterEntry->currentDmmBlockAddrInList;
                }
                else
                {
                    tempDmmPtr = dmmIterEntry->currentDmmBlockAddrInList->nextBySizeOrPartitionPtr.nextBySize;
                }
                if (tempDmmPtr == NULL)
                {
                    CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_STATE, LOG_ERROR_NO_MSG);
                }
                foundDmmBlock = NULL;
                while (tempDmmPtr != NULL)
                {
                    if (PRV_CPSS_DXCH_LPM_RAM_GET_LPM_OFFSET_FROM_DMM_MAC(tempDmmPtr) == data)
                    {
                        foundDmmBlock = tempDmmPtr;
                        break;
                    }
                    tempDmmPtr = tempDmmPtr->nextBySizeOrPartitionPtr.nextBySize;
                }
                if (foundDmmBlock == NULL)
                {
                    CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_STATE, LOG_ERROR_NO_MSG);
                }

                if (foundDmmBlock->prevBySize == NULL)
                {
                    /* the found block with correct hw offset is first in the list */
                    /* it is the right place - no move is needed */
                    updateSmallFreeListArray = GT_FALSE;
                    updateBigFreeList = GT_FALSE;
                    dmmIterEntry->currentDmmBlockAddrInList = foundDmmBlock;
                    continue;
                }
                if ( (updateSmallFreeListArray == GT_FALSE) && (updateBigFreeList == GT_FALSE) )
                {
                    /* after first iteration in list is done and first currentDmmBlockAddrInList
                       moved on its constant place */
                    if (foundDmmBlock->prevBySize == dmmIterEntry->currentDmmBlockAddrInList)
                    {
                        /* the foundDmmBlock block is not first in the list. This is the
                           case when founded block should be located just after previously
                           found block. So if this block points by prevBySize on the
                           currentDmmBlockAddrInList --  it is the right place - no move is needed */
                        dmmIterEntry->currentDmmBlockAddrInList = foundDmmBlock;
                        continue;
                    }
                }
                /* the block with the same hw offset is found */
                tempDmmPtr = dmmIterEntry->currentDmmBlockAddrInList;
                tempCurrentNextBySize = tempDmmPtr->nextBySizeOrPartitionPtr.nextBySize;
                tempNextBySize = foundDmmBlock->nextBySizeOrPartitionPtr.nextBySize;
                tempPrevBySize = foundDmmBlock->prevBySize;

                if ( (updateSmallFreeListArray == GT_TRUE) || (updateBigFreeList == GT_TRUE) )
                {
                    /* set this block as first in the list */
                    if (updateSmallFreeListArray == GT_TRUE)
                    {
                        dmmIterEntry->partitionArray[j]->tableOfSizePointers[tempListName] = foundDmmBlock;
                    }
                    else
                    {
                        dmmIterEntry->partitionArray[j]->bigBlocksList = foundDmmBlock;
                    }
                    foundDmmBlock->prevBySize = NULL;
                    foundDmmBlock->nextBySizeOrPartitionPtr.nextBySize = tempDmmPtr;
                    tempDmmPtr->prevBySize = foundDmmBlock;
                    updateSmallFreeListArray = GT_FALSE;
                    updateBigFreeList = GT_FALSE;
                }
                else
                {
                    /* set this block after current block in the list */
                    foundDmmBlock->prevBySize = tempDmmPtr;
                    foundDmmBlock->nextBySizeOrPartitionPtr.nextBySize = tempDmmPtr->nextBySizeOrPartitionPtr.nextBySize;
                    tempDmmPtr->nextBySizeOrPartitionPtr.nextBySize = foundDmmBlock;
                    tempCurrentNextBySize->prevBySize = foundDmmBlock;
                }
                if (tempNextBySize != NULL)
                {
                    /* last element in the list */
                    tempNextBySize->prevBySize = tempPrevBySize;
                }
                tempPrevBySize->nextBySizeOrPartitionPtr.nextBySize = tempNextBySize;
                dmmIterEntry->currentDmmBlockAddrInList = foundDmmBlock;
            }
        }
    }
    /* in case all data was processed */
    /* update the left num of entries */

    numberInternalEntiresLeft -= numberInternalEntriesUsed;

    *dataSizePtr = *dataSizePtr - numberInternalEntriesUsed * sizeof(GT_U32);
    /* free & zero the iterator */
    cpssOsLpmFree((GT_PTR)*iterPtr);
    *iterPtr = 0;

    return GT_OK;
}

/*******************************************************************************
* ipLpmDbSetL3
*
* DESCRIPTION:
*       Set the data needed for core IP shadow reconstruction used after HSU
*
* INPUTS:
*       lpmDbPtr              - The LPM DB
*       tablePtr              - points to the table size info block
*       iterationSizePtr      - points to data size in bytes supposed to be processed
*                               in current iteration
*       iterPtr               - points to the iterator, to start - set to 0.
*
* OUTPUTS:
*       iterationSizePtr      - points to data size in bytes left after iteration step.
*       iterPtr               - points to the iterator, if = 0 then the operation is done.
*
* RETURNS:
*       GT_OK                 - on success
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS ipLpmDbSetL3
(
    IN    PRV_CPSS_DXCH_LPM_RAM_SHADOWS_DB_STC  *lpmDbPtr,
    IN    GT_VOID                               *tablePtr,
    INOUT GT_U32                                *iterationSizePtr,
    INOUT GT_UINTPTR                            *iterPtr
)
{
    GT_STATUS retVal = GT_OK;
    IP_HSU_ITERATOR_STC *currentIterPtr;
    PRV_CPSS_DXCH_LPM_RAM_SHADOW_STC    *ipShadowPtr;
    GT_U32                              *currDataSizePtr = iterationSizePtr;
    GT_U32                              curentDataSizeBeforeStage;

    currentIterPtr = (IP_HSU_ITERATOR_STC*)*iterPtr;

    if (currentIterPtr == NULL)
    {
        /* we need to allocate the iterator */
        currentIterPtr =
        (IP_HSU_ITERATOR_STC*)cpssOsLpmMalloc(sizeof(IP_HSU_ITERATOR_STC));
        if (currentIterPtr == NULL)
        {
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_OUT_OF_CPU_MEM, LOG_ERROR_NO_MSG);
        }
        /*reset it */
        cpssOsMemSet(currentIterPtr,0,sizeof(IP_HSU_ITERATOR_STC));
        /* set the starting stage */
        currentIterPtr->currStage = IP_HSU_VR_STAGE_E;
       *iterPtr = (GT_UINTPTR)currentIterPtr;
    }
     /* set the memory position */
        currentIterPtr->currMemPtr = tablePtr;
    for (; currentIterPtr->currShadowIdx < lpmDbPtr->numOfShadowCfg;
        currentIterPtr->currShadowIdx++)
    {
        ipShadowPtr = &lpmDbPtr->shadowArray[currentIterPtr->currShadowIdx];

        if (currentIterPtr->currStage == IP_HSU_VR_STAGE_E)
        {
            curentDataSizeBeforeStage = *currDataSizePtr;
            retVal = ipLpmDbSetL3Vr(ipShadowPtr,
                                    &currentIterPtr->currMemPtr,
                                    &currentIterPtr->currIter,
                                    currDataSizePtr);
            if ((retVal == GT_NO_RESOURCE)&&
                (*currDataSizePtr == curentDataSizeBeforeStage))
            {
                /* the stage is not done: the data size was smaller than entry in shadow */
                return GT_OK;
            }
            /* check if we finished with the phase */
            if (currentIterPtr->currIter == 0)
            {
                /* set the next stage */
                currentIterPtr->currStage = IP_HSU_UC_PREFIX_STAGE_E;
            }

            if ((retVal != GT_OK) || (*currDataSizePtr == 0))
            {
                return (retVal);
            }
        }
        if (currentIterPtr->currStage != IP_HSU_DMM_FREE_LIST_E)
        {
            for (; currentIterPtr->currProtocolStack < 2; currentIterPtr->currProtocolStack++)
            {
                if (ipShadowPtr->isIpVerIsInitialized[currentIterPtr->currProtocolStack] == GT_FALSE)
                {
                    continue;
                }

                while (currentIterPtr->currVrId < ipShadowPtr->vrfTblSize)
                {
                    /* go over all valid VR */
                    if ((ipShadowPtr->vrRootBucketArray[currentIterPtr->currVrId].valid == GT_FALSE) ||
                        (ipShadowPtr->vrRootBucketArray[currentIterPtr->currVrId].rootBucket == NULL))
                    {
                        currentIterPtr->currVrId++;
                        continue;
                    }

                    if (currentIterPtr->currStage == IP_HSU_UC_PREFIX_STAGE_E)
                    {
                        curentDataSizeBeforeStage = *currDataSizePtr;
                        retVal = ipLpmDbSetL3UcPrefix(ipShadowPtr,
                                                      currentIterPtr->currVrId,
                                                      currentIterPtr->currProtocolStack,
                                                      &currentIterPtr->currMemPtr,
                                                      &currentIterPtr->currIter,
                                                      currDataSizePtr);
                        if ((retVal == GT_NO_RESOURCE)&&
                            (*currDataSizePtr == curentDataSizeBeforeStage))
                        {
                            /* the stage is not done: the data size was smaller than entry in shadow */
                            return GT_OK;
                        }
                        /* check if we finished with the phase */
                        if (currentIterPtr->currIter == 0)
                        {
                            /* set the next stage */
                            currentIterPtr->currStage = IP_HSU_UC_LPM_STAGE_E;
                        }

                        if ((retVal != GT_OK) || (*currDataSizePtr == 0))
                        {
                            return (retVal);
                        }
                    }

                    if (currentIterPtr->currStage == IP_HSU_UC_LPM_STAGE_E)
                    {
                        curentDataSizeBeforeStage = *currDataSizePtr;
                        retVal = ipLpmDbSetL3UcLpm(ipShadowPtr,
                                                   currentIterPtr->currVrId,
                                                   currentIterPtr->currProtocolStack,
                                                   &currentIterPtr->currMemPtr,
                                                   &currentIterPtr->currIter,
                                                   currDataSizePtr);
                        if ((retVal == GT_NO_RESOURCE)&&
                            (*currDataSizePtr == curentDataSizeBeforeStage))
                        {
                            /* the stage is not done: the data size was smaller than entry in shadow */
                            return GT_OK;
                        }

                        /* check if we finished with the phase */
                        if (currentIterPtr->currIter == 0)
                        {
                            /* set the next stage */
                            currentIterPtr->currStage = IP_HSU_MC_PREFIX_STAGE_E;
                        }

                        if ((retVal != GT_OK) || (*currDataSizePtr == 0))
                        {
                            return (retVal);
                        }

                    }

                    if (currentIterPtr->currStage == IP_HSU_MC_PREFIX_STAGE_E)
                    {
                        curentDataSizeBeforeStage = *currDataSizePtr;
                        retVal = ipLpmDbSetL3McRoutes(ipShadowPtr,
                                                      currentIterPtr->currVrId,
                                                      currentIterPtr->currProtocolStack,
                                                      &currentIterPtr->currMemPtr,
                                                      &currentIterPtr->currIter,
                                                      currDataSizePtr);
                        if ((retVal == GT_NO_RESOURCE)&&
                            (*currDataSizePtr == curentDataSizeBeforeStage))
                        {
                            /* the stage is not done: the data size was smaller than entry in shadow */
                            return GT_OK;
                        }
                        /* check if we finished with the phase */
                        if (currentIterPtr->currIter == 0)
                        {
                            /* set the next stage */
                            currentIterPtr->currStage = IP_HSU_MC_LPM_STAGE_E;
                        }

                        if ((retVal != GT_OK) || (*currDataSizePtr == 0))
                        {
                            return (retVal);
                        }
                    }
                    if (currentIterPtr->currStage == IP_HSU_MC_LPM_STAGE_E)
                    {
                        curentDataSizeBeforeStage = *currDataSizePtr;
                        retVal = ipLpmDbSetL3McLpm(ipShadowPtr,
                                                   currentIterPtr->currVrId,
                                                   currentIterPtr->currProtocolStack,
                                                   &currentIterPtr->currMemPtr,
                                                   &currentIterPtr->currIter,
                                                   currDataSizePtr);
                        if ((retVal == GT_NO_RESOURCE)&&
                            (*currDataSizePtr == curentDataSizeBeforeStage))
                        {
                            /* the stage is not done: the data size was smaller than entry in shadow */
                            return GT_OK;
                        }
                        /* check if we finished with the phase */
                        if (currentIterPtr->currIter == 0)
                        {
                            /* set the next stage */
                            currentIterPtr->currStage = IP_HSU_UC_PREFIX_STAGE_E;

                            /* we finished with this vrid go to the next */
                            currentIterPtr->currVrId++;
                        }
                        if (retVal != GT_OK)
                        {
                            return (retVal);
                        }
                    }
                }
                /* we finished with this protocol stack zero the vrid */
                currentIterPtr->currVrId = 0;
            }

        }

        /* the last stage is IP_HSU_DMM_FREE_LIST_E */
        currentIterPtr->currStage = IP_HSU_DMM_FREE_LIST_E;
        if (currentIterPtr->currStage == IP_HSU_DMM_FREE_LIST_E)
        {
            curentDataSizeBeforeStage = *currDataSizePtr;
            retVal = ipLpmDbSetL3DmmFreeLists(ipShadowPtr,
                                              &currentIterPtr->currMemPtr,
                                              &currentIterPtr->currIter,
                                              currDataSizePtr);
            if ((retVal == GT_NO_RESOURCE)&&
                (*currDataSizePtr == curentDataSizeBeforeStage))
            {
                /* the stage is not done: the data size was smaller than entry in shadow */
                return GT_OK;
            }
            /* check if we finished with the phase */
            if (currentIterPtr->currIter == 0)
            {
                /* set the next stage */
                currentIterPtr->currStage = IP_HSU_UC_PREFIX_STAGE_E;
            }
            if ((retVal != GT_OK) || ( currentIterPtr->currIter != 0))
            {
                /*osSemSignal(ipShadowPtr->ipUnitSem);*/
                return(retVal);
            }
        }
        /* we finished with the shadow , set the starting stage */
        currentIterPtr->currStage = IP_HSU_VR_STAGE_E;
    }
    /* we reached the end free the iterator */
    cpssOsLpmFree(currentIterPtr);
    *iterPtr = 0;

    return (GT_OK);
}

/*******************************************************************************
* cpssExMxPmIpLpmDbMemSizeGet
*
* DESCRIPTION:
*       Function Relevant mode : High Level API modes
*       This function calculates the  memory size needed to export the Lpm DB
*       (used for HSU and unit Hotsync)
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2.
*
* INPUTS:
*       lpmDbPtr              - The LPM DB
*
* OUTPUTS:
*       lpmDbSizePtr          - pointer to lpmDb size calculated in bytes.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PTR               - on NULL pointer
*       GT_FAIL                  - otherwise
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       none.
*
*******************************************************************************/
GT_STATUS cpssExMxPmIpLpmDbMemSizeGet
(
    IN  PRV_CPSS_DXCH_LPM_RAM_SHADOWS_DB_STC    *lpmDbPtr,
    OUT GT_U32                                  *lpmDbSizePtr
)
{
    GT_UINTPTR iterPtr = 0;
    GT_U32 iterationSise = 0xffffffff;/* perform size calculating in one iteration*/
    return ipLpmDbGetL3(lpmDbPtr,lpmDbSizePtr,NULL,&iterationSise,&iterPtr);
}

/*******************************************************************************
* cpssExMxPmIpLpmDbExport
*
* DESCRIPTION:
*       Function Relevant mode : High Level API modes
*       This function exports the Lpm DB into the preallocated memory,
*       (used for HSU and unit Hotsync)
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2.
*
* INPUTS:
*       lpmDbPtr              - The LPM DB
*       lpmDbMemBlockSizePtr  - pointer to block data size supposed to be exported
*                               in current iteration.
*       lpmDbMemBlockPtr      - pointer to allocated for lpm DB memory area
*       iterPtr               - the iterator, to start - set to 0.
*
* OUTPUTS:
*       lpmDbMemBlockSizePtr  - pointer to block data size that was not used
*                               in current iteration.
*       iterPtr               - the iterator, if = 0 then the operation is done.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PTR               - on NULL pointer
*       GT_FAIL                  - otherwise
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       Change behaving: from this version treatment of lpmDbMemBlockPtr was changed.
*       Now assumption is that it points on concrete block allocated only for
*       this iteration and not on big contiguous memory block for whole export/import
*       operation. User working with contiguous memory should give exact pointer
*       for next iteration: previous pointer + calculated iteration size.
*******************************************************************************/
GT_STATUS cpssExMxPmIpLpmDbExport
(
    IN     PRV_CPSS_DXCH_LPM_RAM_SHADOWS_DB_STC *lpmDbPtr,
    INOUT  GT_U32                               *lpmDbMemBlockSizePtr,
    IN     GT_U32                               *lpmDbMemBlockPtr,
    INOUT  GT_UINTPTR                           *iterPtr
)
{
    return ipLpmDbGetL3(lpmDbPtr,NULL,lpmDbMemBlockPtr,lpmDbMemBlockSizePtr,iterPtr);
}

/*******************************************************************************
* cpssExMxPmIpLpmDbImport
*
* DESCRIPTION:
*       Function Relevant mode : High Level API modes

*       This function imports the Lpm DB recived and reconstruct it,
*       (used for HSU and unit Hotsync)
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2.
*
* INPUTS:
*       lpmDbPtr              - The LPM DB
*       lpmDbMemBlockSizePtr  - pointer to block data size supposed to be imported
*                               in current iteration.
*       lpmDbMemBlockPtr      - pointer to allocated for lpm DB memory area
*       iterPtr               - the iterator, to start - set to 0.
*
* OUTPUTS:
*       lpmDbMemBlockSizePtr  - pointer to block data size that was not used
*                               in current iteration.
*       iterPtr               - the iterator, if = 0 then the operation is done.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PTR               - on NULL pointer
*       GT_FAIL                  - otherwise
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       Change behaving: from this version treatment of lpmDbMemBlockPtr was changed.
*       Now assumption is that it points on concrete block allocated only for
*       this iteration and not on big contiguous memory block for whole export/import
*       operation. User working with contiguous memory should give exact pointer
*       for next iteration: previous pointer + calculated iteration size.
*******************************************************************************/
GT_STATUS cpssExMxPmIpLpmDbImport
(
    IN    PRV_CPSS_DXCH_LPM_RAM_SHADOWS_DB_STC  *lpmDbPtr,
    INOUT GT_U32                                *lpmDbMemBlockSizePtr,
    IN    GT_U32                                *lpmDbMemBlockPtr,
    INOUT GT_UINTPTR                            *iterPtr
)
{
    return ipLpmDbSetL3(lpmDbPtr,lpmDbMemBlockPtr,lpmDbMemBlockSizePtr,iterPtr);
}
#endif

/*******************************************************************************
* prvCpssDxChLpmRamMemFreeListMng
*
* DESCRIPTION:
*       This function is used to collect all Pp Narrow Sram memory free
*       operations inorder to be freed at the end of these operations.
*
* INPUTS:
*       ppMemAddr - the address in the HW (the device memory pool) to record.
*       operation - the operation (see
*                   PRV_CPSS_DXCH_LPM_RAM_MEM_LIST_RESET_OP_E,
*                   PRV_CPSS_DXCH_LPM_RAM_MEM_LIST_ADD_MEM_OP_E,
*                   PRV_CPSS_DXCH_LPM_RAM_MEM_LIST_FREE_MEM_OP_E
*       memBlockListPtrPtr - (pointer to) the memory block list to act upon.
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
GT_STATUS prvCpssDxChLpmRamMemFreeListMng
(
    IN  GT_UINTPTR                                  ppMemAddr,
    IN  PRV_CPSS_DXCH_LPM_RAM_MEM_LIST_OP_ENT       operation,
    IN  PRV_CPSS_DXCH_LPM_RAM_MEM_LIST_ENTRY_STC    **memBlockListPtrPtr
)
{
    PRV_CPSS_DXCH_LPM_RAM_MEM_LIST_ENTRY_STC  *ppMemEntry,*tmpPtr; /* Pp memory list entry.    */
    GT_STATUS                   retVal = GT_OK;     /* Functions return value.      */

    switch(operation)
    {
    case (PRV_CPSS_DXCH_LPM_RAM_MEM_LIST_RESET_OP_E):
        /* make sure it an empty list */
        if (*memBlockListPtrPtr != NULL)
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_STATE, LOG_ERROR_NO_MSG);

        break;

    case (PRV_CPSS_DXCH_LPM_RAM_MEM_LIST_ADD_MEM_OP_E):
        ppMemEntry = cpssOsLpmMalloc(sizeof(PRV_CPSS_DXCH_LPM_RAM_MEM_LIST_ENTRY_STC));
        if (ppMemEntry == NULL)
        {
            retVal = GT_OUT_OF_CPU_MEM;
        }
        else
        {
            ppMemEntry->memAddr  = ppMemAddr;
            ppMemEntry->next = *memBlockListPtrPtr;
            *memBlockListPtrPtr = ppMemEntry;

        }
        break;

    case (PRV_CPSS_DXCH_LPM_RAM_MEM_LIST_FREE_MEM_OP_E):
        ppMemEntry = *memBlockListPtrPtr;
        while (ppMemEntry != NULL)
        {
                if (prvCpssDmmFree(ppMemEntry->memAddr) == 0)
                {
                    CPSS_LOG_ERROR_AND_RETURN_MAC(GT_FAIL, LOG_ERROR_NO_MSG);
                }

                tmpPtr = ppMemEntry;
                ppMemEntry = ppMemEntry->next;

                cpssOsLpmFree(tmpPtr);
        }
        *memBlockListPtrPtr = NULL;
        break;
    }

    return retVal;
}

/*******************************************************************************
* prvCpssDxChLpmRamUpdateVrTableFuncWrapper
*
* DESCRIPTION:
*       This function is a wrapper to PRV_CPSS_DXCH_LPM_RAM_UPDATE_VRF_TABLE_FUNC_PTR
*
* INPUTS:
*       data - the parmeters for PRV_CPSS_DXCH_LPM_RAM_UPDATE_VRF_TABLE_FUNC_PTR
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK on success, or
*       GT_OUT_OF_CPU_MEM        - if failed to allocate CPU memory, or
*       GT_OUT_OF_PP_MEM         - if failed to allocate PP memory.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS prvCpssDxChLpmRamUpdateVrTableFuncWrapper
(
    IN  GT_PTR                  data
)
{
    PRV_CPSS_DXCH_LPM_RAM_UPDATE_VRF_TABLE_FUNC_PARAMS_STC *params;

    params = (PRV_CPSS_DXCH_LPM_RAM_UPDATE_VRF_TABLE_FUNC_PARAMS_STC*)data;

    return prvCpssDxChLpmRamMngVrfEntryUpdate(params->vrId, params->protocol,
                                              params->shadowPtr);
}

/*******************************************************************************
* prvCpssDxChLpmRouteEntryPointerCheck
*
* DESCRIPTION:
*       Check validity of values of route entry pointer
*
* INPUTS:
*       routeEntryPointerPtr      - route entry pointer to check
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                     - on success
*       GT_BAD_PARAM              - on bad parameters
*       GT_NOT_APPLICABLE_DEVICE  - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS prvCpssDxChLpmRouteEntryPointerCheck
(
    IN  PRV_CPSS_DXCH_LPM_ROUTE_ENTRY_POINTER_STC  *routeEntryPointerPtr
)
{
    CPSS_NULL_PTR_CHECK_MAC(routeEntryPointerPtr);

    if ((routeEntryPointerPtr->routeEntryMethod != PRV_CPSS_DXCH_LPM_ENTRY_TYPE_ECMP_E) &&
        (routeEntryPointerPtr->routeEntryMethod != PRV_CPSS_DXCH_LPM_ENTRY_TYPE_QOS_E) &&
        (routeEntryPointerPtr->routeEntryMethod != PRV_CPSS_DXCH_LPM_ENTRY_TYPE_REGULAR_E))
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    /* validate the ipv6 MC group scope level */
    switch (routeEntryPointerPtr->ipv6McGroupScopeLevel)
    {
        case CPSS_IPV6_PREFIX_SCOPE_LINK_LOCAL_E:
        case CPSS_IPV6_PREFIX_SCOPE_SITE_LOCAL_E:
        case CPSS_IPV6_PREFIX_SCOPE_UNIQUE_LOCAL_E:
        case CPSS_IPV6_PREFIX_SCOPE_GLOBAL_E:
            break;

        default:
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    /* note that since LPM DB can exist without any devices added to it, there
       is no way to know how many route entries or ECMP/QoS entries are
       available in specific device; therefore neither the route entries base
       address nor the size of the ECMP/QoS block value can't be checked */

    return GT_OK;
}

#if 0
/*******************************************************************************
* prvCpssExMxPmLpmDbIdGetNext
*
* DESCRIPTION:
*       This function retrieve next LPM DB ID from LPM DBs Skip List
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2.
*
* INPUTS:
*       slIteratorPtr - The iterator Id that was returned from the last call to
*                       this function.
* OUTPUTS:
*       lpmDbId       - retrieved LPM DB ID
*       slIteratorPtr - The iteration Id to be sent in the next call to this
*                       function to get the next data.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PTR               - on NULL pointer
*       GT_NO_MORE               - on absence of elements in skip list
*
* COMMENTS:
*       none.
*
*******************************************************************************/
GT_STATUS prvCpssExMxPmLpmDbIdGetNext
(
    OUT     GT_U32      *lpmDbIdPtr,
    INOUT   GT_UINTPTR  *slIteratorPtr
)
{
    PRV_CPSS_DXCH_LPM_RAM_LPM_DB_STC *lpmDbPtr = NULL;
    CPSS_NULL_PTR_CHECK_MAC(slIteratorPtr);
    CPSS_NULL_PTR_CHECK_MAC(lpmDbIdPtr);

    lpmDbPtr = (PRV_CPSS_DXCH_LPM_RAM_LPM_DB_STC *)prvCpssSlGetNext(lpmDbSL,slIteratorPtr);
    if (lpmDbPtr == NULL)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NO_MORE, LOG_ERROR_NO_MSG);
    }
    *lpmDbIdPtr = lpmDbPtr->lpmDBId;
    return GT_OK;
}
#endif

