/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* prvCpssDxChLpmMng.c
*
* DESCRIPTION:
*       Implementation of the LPM algorithm, for the use of UC and MC engines.
*
* DEPENDENCIES:
*       None
*
* FILE REVISION NUMBER:
*       $Revision: 24 $
*
*******************************************************************************/

#include <cpss/extServices/os/gtOs/gtGenTypes.h>
#include <cpss/dxCh/dxChxGen/private/lpm/ram/prvCpssDxChLpmRamTypes.h>
#include <cpss/extServices/private/prvCpssBindFunc.h>
#include <cpssCommon/private/prvCpssDevMemManager.h>
#include <cpssCommon/cpssPresteraDefs.h>
#include <cpss/dxCh/dxChxGen/private/lpm/ram/prvCpssDxChLpmRam.h>
#include <cpss/dxCh/dxChxGen/private/lpm/ram/prvCpssDxChLpmRamMng.h>
#include <cpss/dxCh/dxChxGen/config/private/prvCpssDxChInfo.h>
#include <cpss/dxCh/dxChxGen/lpm/cpssDxChLpm.h>
#include <cpss/dxCh/dxChxGen/private/lpm/hw/prvCpssDxChLpmHw.h>
#include <cpssCommon/private/prvCpssMath.h>

extern void * cpssOsLpmMalloc
(
    IN GT_U32 size
);

extern void cpssOsLpmFree
(
    IN void* const memblock
);

GT_UINTPTR   dmmBlockNotFoundIndication = (GT_UINTPTR)(-1);

/***************************************************************************
* local defines
****************************************************************************/

/* This macro checks if the given range is the last range   */
/* in this LPM level.                                       */
#define LAST_RANGE_MAC(range)            (range->next == NULL)

/* This macro converts a prefix length to a mask            */
/* representation.                                          */
#define PREFIX_2_MASK_MAC(prefixLen,maxPrefix) \
        (((prefixLen > maxPrefix)|| (prefixLen == 0))? 0 : (1 << (prefixLen-1)))

/* The following macro returns the maximal prefix in the    */
/* next level bucket.                                       */
#define NEXT_BUCKET_PREFIX_MAC(prefix,levelPrefix)  \
        ((prefix <= levelPrefix) ? 0 : (prefix - levelPrefix))

/* The following macro returns the maximal prefix in the    */
/* given level bucket.                                       */
#define BUCKET_PREFIX_AT_LEVEL_MAC(prefix,levelPrefix,level)  \
        ((prefix <= (level * levelPrefix)) ? \
        0 : (prefix - (level * levelPrefix)))

#define MAX_LPM_LEVELS_CNS 17
#define MAX_NUMBER_OF_COMPRESSED_1_RANGES_CNS   5
#define MAX_NUMBER_OF_COMPRESSED_2_RANGES_CNS   10

/*
 * Typedef: enum LPM_BUCKET_UPDATE_MODE_ENT
 *
 * Description:
 *      Indicates what kind of update the bucket will go.
 *
 *
 * Fields:
 *      LPM_BUCKET_UPDATE_NONE_E  - updates nothing.
 *      LPM_BUCKET_UPDATE_HW_AND_MEM_ALLOC_E - updates the HW and mem alloc new
 *                                            memory if needed.
 *      LPM_BUCKET_UPDATE_HW_AND_MEM_RESIZE_E - updates the HW and resize
 *                                             bucket's memory if needed
 *                                            (bulk delete operation).
 *      LPM_BUCKET_UPDATE_MEM_ONLY_E - updates mem alloc only.
 *      LPM_BUCKET_UPDATE_SHADOW_ONLY_E - updates the bucket tree shadow only
 *                                       without memory alloc or HW.
 *      LPM_BUCKET_UPDATE_ROOT_BUCKET_SHADOW_ONLY_E - this is an update for
 *                                     root bucket , it's a shodow update
 *                                     only, no touching of the HW.
 *
 */
typedef enum
{
    LPM_BUCKET_UPDATE_NONE_E = 0,
    LPM_BUCKET_UPDATE_HW_AND_MEM_ALLOC_E,
    LPM_BUCKET_UPDATE_HW_AND_MEM_RESIZE_E,
    LPM_BUCKET_UPDATE_MEM_ONLY_E,
    LPM_BUCKET_UPDATE_SHADOW_ONLY_E,
    LPM_BUCKET_UPDATE_ROOT_BUCKET_SHADOW_ONLY_E

}LPM_BUCKET_UPDATE_MODE_ENT;

/*******************************************************************************
* mask2PrefixLength
*
* DESCRIPTION:
*       This function returns the index of the most significant set bit (1-8),
*       in the given mask.
*
* INPUTS:
*       mask            - The prefix mask to operate on.
*       levelMaxPrefix  - The maximum prefix that can be hold by this mask.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       1 - levelMaxPrefix if (mask != 0), 0 otherwise.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_U32 mask2PrefixLength
(
    IN GT_U32 mask,
    IN GT_U8 levelMaxPrefix
)
{
    GT_U8 i;

    for(i = 0; i < levelMaxPrefix; i++)
    {
        if((mask & (1 << (levelMaxPrefix - 1 - i))) != 0)
            return (levelMaxPrefix - i);
    }
    return 0;
}

/*******************************************************************************
* calcStartEndAddr
*
* DESCRIPTION:
*       This function calcules the start & end address of a prefix.
*
* INPUTS:
*       addr          - the address octet.
*       prefixLen     - the address prefix length
* OUTPUTS:
*       startAddrPtr  - the calculated start address
*       endAddrPtr    - the calculated end address
*
*
* RETURNS:
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS calcStartEndAddr
(
    IN  GT_U8  addr,
    IN  GT_U32 prefixLen,
    OUT GT_U8  *startAddrPtr,
    OUT GT_U8  *endAddrPtr
)
{
    GT_U8  prefixComp;          /* temp var. for calculating startAddr  */
                                /* and endAddr.                         */

    prefixComp = (GT_U8)(((prefixLen > PRV_CPSS_DXCH_LPM_RAM_MAX_LEVEL_LENGTH_CNS) ?
                  0 : (PRV_CPSS_DXCH_LPM_RAM_MAX_LEVEL_LENGTH_CNS - prefixLen)));

    *startAddrPtr = (GT_U8)(addr &
                 (BIT_MASK_MAC(PRV_CPSS_DXCH_LPM_RAM_MAX_LEVEL_LENGTH_CNS) << prefixComp));

    if (endAddrPtr != NULL)
        *endAddrPtr = (GT_U8)(*startAddrPtr | ~(0xFF << prefixComp));

    return GT_OK;
}

/*******************************************************************************
* find1stOverlap
*
* DESCRIPTION:
*       This function traverses the ranges linked list from the low address and
*       stopping at the first overlapping range with the prefix.
*
* INPUTS:
*       bucketPtr   - pointer to bucket, in CPU's memory.
*       startAddr   - prefix start address, lower address covered by the prefix.
*       pPrevPtr    - A pointer to the node before the ovelapping node.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       A pointer to the first overlapping range.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static PRV_CPSS_DXCH_LPM_RAM_RANGE_SHADOW_STC *find1stOverlap
(
    IN  PRV_CPSS_DXCH_LPM_RAM_BUCKET_SHADOW_STC   *bucketPtr,
    IN  GT_U8                                     startAddr,
    OUT PRV_CPSS_DXCH_LPM_RAM_RANGE_SHADOW_STC    **pPrevPtr
)
{
    PRV_CPSS_DXCH_LPM_RAM_RANGE_SHADOW_STC *rangePtr;/* Current range pointer.   */

    if(pPrevPtr != NULL)
        *pPrevPtr = NULL;

    /* if the address we need is after the cashed range, start with the cashed
       range,
       or if the address we need is the same as the cashed ranged, use it only
       if we don't need to provide a prev range */
    if ((bucketPtr->rangeCash != NULL) &&
        ((startAddr > bucketPtr->rangeCash->startAddr) ||
         ((startAddr == bucketPtr->rangeCash->startAddr) &&
          (pPrevPtr == NULL))))
    {
        rangePtr = bucketPtr->rangeCash;
    }
    else
    {
        rangePtr = bucketPtr->rangeList;
    }

    while((rangePtr->next != NULL) &&
          (startAddr >= rangePtr->next->startAddr))
    {
        bucketPtr->rangeCash = rangePtr;/* the range cash always saves the prev*/
        if(pPrevPtr != NULL)
            *pPrevPtr = rangePtr;

        rangePtr = rangePtr->next;
    }

    return rangePtr;
}

/*******************************************************************************
* prvCpssDxChLpmRamMngRangeInNextPointerFormatSet
*
* DESCRIPTION:
*       This sets the given range in next pointer format
*
* INPUTS:
*       rangePtr  - the range to get from.
*       lpmEngineMemPtr - points to a an PRV_CPSS_DXCH_LPM_RAM_MEM_INFO_STC which
*                         holds all the memory information needed for where
*                         and how to treat the search memory.
*
* OUTPUTS:
*       nextPointerArrayPtr - the next pointer
*
* RETURNS:
*       GT_OK on success, GT_FAIL otherwise.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS prvCpssDxChLpmRamMngRangeInNextPointerFormatSet
(
    IN  PRV_CPSS_DXCH_LPM_RAM_RANGE_SHADOW_STC    *rangePtr,
    IN  PRV_CPSS_DXCH_LPM_RAM_MEM_INFO_STC        *lpmEngineMemPtr,
    OUT CPSS_DXCH_LPM_NODE_NEXT_POINTER_STC       *nextPointerArrayPtr
)
{
    GT_STATUS retVal = GT_OK;
    PRV_CPSS_DXCH_LPM_ROUTE_ENTRY_POINTER_STC   *nextHopPtr;
    PRV_CPSS_DXCH_LPM_RAM_BUCKET_SHADOW_STC     *lpmBucketPtr;

    if (rangePtr == NULL)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_FAIL, LOG_ERROR_NO_MSG);
    }
    if ((rangePtr->pointerType == CPSS_DXCH_LPM_ROUTE_ENTRY_NEXT_PTR_TYPE_E) ||
        (rangePtr->pointerType == CPSS_DXCH_LPM_ECMP_ENTRY_PTR_TYPE_E) ||
        (rangePtr->pointerType == CPSS_DXCH_LPM_QOS_ENTRY_PTR_TYPE_E))
    {
        nextHopPtr = rangePtr->lowerLpmPtr.nextHopEntry;
        if (nextHopPtr == NULL)
        {
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_FAIL, LOG_ERROR_NO_MSG);
        }

        nextPointerArrayPtr->pointerType = rangePtr->pointerType;
        nextPointerArrayPtr->pointerData.nextHopOrEcmpPointer.ucRpfCheckEnable =
            nextHopPtr->ucRpfCheckEnable;
        nextPointerArrayPtr->pointerData.nextHopOrEcmpPointer.srcAddrCheckMismatchEnable =
            nextHopPtr->srcAddrCheckMismatchEnable;
        nextPointerArrayPtr->pointerData.nextHopOrEcmpPointer.ipv6McGroupScopeLevel =
            nextHopPtr->ipv6McGroupScopeLevel;
        nextPointerArrayPtr->pointerData.nextHopOrEcmpPointer.entryIndex =
            nextHopPtr->routeEntryBaseMemAddr;
    }
    else
    {
        lpmBucketPtr = rangePtr->lowerLpmPtr.nextBucket;
        if (lpmBucketPtr == NULL)
        {
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_FAIL, LOG_ERROR_NO_MSG);
        }
        /* if the hwBucketOffsetHandle is 0 this is a new bucket that represents
           only a next hop -> thus take the next hop of the first range */
        if (lpmBucketPtr->hwBucketOffsetHandle == 0)
        {
            retVal =
                prvCpssDxChLpmRamMngRangeInNextPointerFormatSet(lpmBucketPtr->rangeList,
                                                                lpmEngineMemPtr,
                                                                nextPointerArrayPtr);
        }
        else
        {
            nextPointerArrayPtr->pointerType = lpmBucketPtr->bucketType;
            nextPointerArrayPtr->pointerData.nextNodePointer.nextPointer =
                PRV_CPSS_DXCH_LPM_RAM_GET_LPM_OFFSET_FROM_DMM_MAC(lpmBucketPtr->hwBucketOffsetHandle) +
                lpmEngineMemPtr->structsBase;
            nextPointerArrayPtr->pointerData.nextNodePointer.range5Index =
                lpmBucketPtr->fifthAddress;
            if ((GT_U32)rangePtr->pointerType == (GT_U32)PRV_CPSS_DXCH_LPM_RAM_TRIE_PTR_TYPE_E)
            {
                /* Set the pointToSipTree bit in case that the next bucket is
                   a root bucket (pointing to MC source tree) */
                nextPointerArrayPtr->pointerData.nextNodePointer.pointToSipTree = GT_TRUE;
            }
            else
            {
                nextPointerArrayPtr->pointerData.nextNodePointer.pointToSipTree = GT_FALSE;
            }
        }
    }
    return retVal;
}

/*******************************************************************************
* insert2Trie
*
* DESCRIPTION:
*       Inserts a next hop entry to the trie structure.
*
* INPUTS:
*       bucketPtr   - Bucket to which the given entry is associated.
*       startAddr   - Start Addr of the address associated with the given
*                     entry.
*       prefix      - The address prefix.
*       trieDepth   - The maximum depth of the trie.
*       nextPtr     - A pointer to the next hop/next_lpm_trie entry to be
*                     inserted to the trie.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK on success, GT_FAIL otherwise.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS insert2Trie
(
    IN PRV_CPSS_DXCH_LPM_RAM_BUCKET_SHADOW_STC  *bucketPtr,
    IN GT_U8                                    startAddr,
    IN GT_U32                                   prefix,
    IN GT_U8                                    trieDepth,
    IN GT_PTR                                   nextPtr
)
{
    GT_U8 addr[1];               /* prefix in GT_U8 representation   */

    addr[0] = (GT_U8)(startAddr & (BIT_MASK_MAC(trieDepth) << (trieDepth - prefix)));

    return prvCpssDxChLpmTrieInsert(&(bucketPtr->trieRoot),
                                    addr,prefix,trieDepth,nextPtr);
}

/*******************************************************************************
* createNewBucket
*
* DESCRIPTION:
*       This function creates a new bucket with a given default next hop route
*       entry.
*
* INPUTS:
*       nextHopPtr - The default route next hop data.
*       trieDepth   - The maximum depth of the trie.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       A pointer to the new created bucket if succeeded, NULL otherwise.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static PRV_CPSS_DXCH_LPM_RAM_BUCKET_SHADOW_STC * createNewBucket
(
    IN PRV_CPSS_DXCH_LPM_ROUTE_ENTRY_POINTER_STC *nextHopPtr,
    IN GT_U8 trieDepth
)
{
    PRV_CPSS_DXCH_LPM_RAM_BUCKET_SHADOW_STC  *pBucket;     /* The bucket to be created.    */
    GT_STATUS                                rc;

    if((pBucket = cpssOsLpmMalloc(sizeof(PRV_CPSS_DXCH_LPM_RAM_BUCKET_SHADOW_STC)))
       == NULL)
        return NULL;

    if((pBucket->rangeList = cpssOsLpmMalloc(sizeof(PRV_CPSS_DXCH_LPM_RAM_RANGE_SHADOW_STC)))
       == NULL)
    {
        cpssOsLpmFree(pBucket);
        pBucket = NULL;

        return NULL;
    }

    /* Initialize the bucket's fields.          */

    pBucket->rangeList->pointerType              = CPSS_DXCH_LPM_ROUTE_ENTRY_NEXT_PTR_TYPE_E;
    pBucket->rangeList->lowerLpmPtr.nextHopEntry = nextHopPtr;

    pBucket->rangeList->startAddr   = 0;
    pBucket->rangeList->mask        = 0;
    pBucket->rangeList->next        = NULL;
    pBucket->bucketType             = CPSS_DXCH_LPM_ROUTE_ENTRY_NEXT_PTR_TYPE_E;
    pBucket->fifthAddress           = 0;
    pBucket->hwBucketOffsetHandle   = 0;
    pBucket->pointingRangeMemAddr   = 0;

    pBucket->rangeCash = NULL;

    pBucket->numOfRanges = 1;
    pBucket->bucketHwUpdateStat = PRV_CPSS_DXCH_LPM_RAM_BUCKET_HW_OK_E;


    /* Insert the default prefix into the       */
    /* Trie of the newly create LPM bucket.     */
    pBucket->trieRoot.pData = NULL;
    rc = insert2Trie(pBucket,0,0,trieDepth,nextHopPtr);
    if (rc != GT_OK)
    {
        /* trie was not inserted successfully*/
        cpssOsLpmFree(pBucket->rangeList);
        cpssOsLpmFree(pBucket);
        return NULL;
    }

    pBucket->trieRoot.father = pBucket->trieRoot.leftSon = NULL;
    pBucket->trieRoot.rightSon = NULL;

    return pBucket;
}

/*******************************************************************************
* splitRange
*
* DESCRIPTION:
*       This function splits a range. According to one of the following possible
*       splits:
*       PRV_CPSS_DXCH_LPM_RAM_SPLIT_OVERWRITE_E, PRV_CPSS_DXCH_LPM_RAM_SPLIT_LOW_SPLIT_E,
*       PRV_CPSS_DXCH_LPM_RAM_SPLIT_HIGH_SPLIT_E and PRV_CPSS_DXCH_LPM_RAM_SPLIT_MID_SPLIT_E
*
* INPUTS:
*       rangePtrPtr     - Pointer to the range to be splitted.
*       startAddr       - The low address of the new range.
*       endAddr         - The high address of the new range.
*       prefixLength    - The length of the dominant prefix of the new range.
*       rangeInfoPtr    - the range information to use for the new range
*                         (or overwrite an old one with this new info).
*       levelPrefix     - The current lpm level prefix.
*       updateOldPtr    - (GT_TRUE) this is an update for an already existing entry.
*
* OUTPUTS:
*       rangePtrPtr         - A pointer to the next range to be checked for split.
*       numOfNewRangesPtr   - Number of new created ranges as a result of the split.
*       pointerTypePtrPtr   - the added/replaced range's pointerType field pointer
*
* RETURNS:
*       GT_OK on success, or
*       GT_OUT_OF_CPU_MEM on lack of cpu memory.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS splitRange
(
    INOUT PRV_CPSS_DXCH_LPM_RAM_RANGE_SHADOW_STC    **rangePtrPtr,
    IN GT_U8                                        startAddr,
    IN GT_U8                                        endAddr,
    IN GT_U32                                       prefixLength,
    IN PRV_CPSS_DXCH_LPM_RAM_RANGE_SHADOW_STC       *rangeInfoPtr,
    IN GT_U8                                        levelPrefix,
    IN GT_BOOL                                      *updateOldPtr,
    OUT GT_U32                                      *numOfNewRangesPtr,
    OUT CPSS_DXCH_LPM_NEXT_POINTER_TYPE_ENT         **pointerTypePtrPtr
)
{
    PRV_CPSS_DXCH_LPM_RAM_RANGE_SHADOW_STC *newRangePtr;  /* Points to the new create */
                            /* range, if such creation is needed.                      */

    PRV_CPSS_DXCH_LPM_RAM_RANGE_SHADOW_STC *curRangePtr = (*rangePtrPtr);    /* Points to rangePtrPtr, for  */
                                            /* easy access.             */
    GT_U8 mask;                /* The mask represented by prefixLength */
                                /* and levelPrefix.                     */
    GT_U8 endRange;            /* Index by which this range ends.      */
    PRV_CPSS_DXCH_LPM_RAM_SPLIT_METHOD_ENT splitMethod;     /* The method by which to split the     */
                                                            /* rangePtrPtr, if needed.              */
    GT_BOOL splittedRangeNeedsHwUpdate = GT_FALSE;


    mask = (GT_U8)PREFIX_2_MASK_MAC(prefixLength,levelPrefix);

    endRange = (GT_U8)(LAST_RANGE_MAC(curRangePtr) ? ((1 << levelPrefix) - 1) :
        ((curRangePtr->next->startAddr) - 1));

    splitMethod = (((startAddr > curRangePtr->startAddr) ? 1 : 0) +
                   ((endAddr < endRange) ? 2 : 0));

    if((*rangePtrPtr)->updateRangeInHw == GT_TRUE)
    {
        /* the range is going to be splitted needs update in hw     */
        /* so all ranges created after splitting should be marked as*/
        /* as needed update in hw                                   */
        /* such situation could be occured during bulk operation    */
        splittedRangeNeedsHwUpdate = GT_TRUE;
    }

    switch (splitMethod)
    {
    case PRV_CPSS_DXCH_LPM_RAM_SPLIT_OVERWRITE_E:
        newRangePtr = curRangePtr;
        newRangePtr->mask |= mask;
        if(*updateOldPtr == GT_TRUE)       /*   Check This  */
        {
            /*osStatFree(newRangePtr->nextPointer.nextPtr.nextHopEntry);*/
            *updateOldPtr = GT_FALSE;
        }

        newRangePtr->pointerType = rangeInfoPtr->pointerType;
        newRangePtr->lowerLpmPtr = rangeInfoPtr->lowerLpmPtr;
        newRangePtr->updateRangeInHw = GT_TRUE;

        *numOfNewRangesPtr = 0;

        /* record the pointer type ptr */
        *pointerTypePtrPtr = &(newRangePtr->pointerType);
        break;

    case PRV_CPSS_DXCH_LPM_RAM_SPLIT_LOW_SPLIT_E:     /* A new node should be added before curRangePtr    */
        if((newRangePtr = cpssOsLpmMalloc(sizeof(PRV_CPSS_DXCH_LPM_RAM_RANGE_SHADOW_STC)))
           == NULL)
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_OUT_OF_CPU_MEM, LOG_ERROR_NO_MSG);

        /* Insert newRangePtr after curRangePtr     */
        newRangePtr->next = curRangePtr->next;
        curRangePtr->next = newRangePtr;

        /* newRangePtr represents the old Range.    */
        newRangePtr->startAddr  = (GT_U8)(endAddr + 1);
        newRangePtr->mask       = curRangePtr->mask;

        newRangePtr->pointerType = curRangePtr->pointerType;
        newRangePtr->lowerLpmPtr = curRangePtr->lowerLpmPtr;

        if (splittedRangeNeedsHwUpdate == GT_TRUE)
        {
            newRangePtr->updateRangeInHw = GT_TRUE;
        }
        else
        {
            /* no new pointer in the old range */
            newRangePtr->updateRangeInHw = GT_FALSE;
        }


        /* curRangePtr represents the new Range.    */
        curRangePtr->mask       |= mask;
        curRangePtr->pointerType = rangeInfoPtr->pointerType;
        curRangePtr->lowerLpmPtr = rangeInfoPtr->lowerLpmPtr;
        /* new pointer so we need to update in the hardware */
        curRangePtr->updateRangeInHw = GT_TRUE;

        *rangePtrPtr       = newRangePtr;
        *numOfNewRangesPtr = 1;

        /* record the pointer type ptr */
        *pointerTypePtrPtr = &(curRangePtr->pointerType);
        break;

    case PRV_CPSS_DXCH_LPM_RAM_SPLIT_HIGH_SPLIT_E:    /* A new node should be added after curRangePtr    */
        if((newRangePtr = cpssOsLpmMalloc(sizeof(PRV_CPSS_DXCH_LPM_RAM_RANGE_SHADOW_STC)))
           == NULL)
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_OUT_OF_CPU_MEM, LOG_ERROR_NO_MSG);

        /* Insert newRangePtr after curRangePtr    */
        newRangePtr->next = curRangePtr->next;
        curRangePtr->next = newRangePtr;

        if (splittedRangeNeedsHwUpdate == GT_TRUE)
        {
            curRangePtr->updateRangeInHw = GT_TRUE;
        }
        else
        {
            /* no new pointer in the old range */
            curRangePtr->updateRangeInHw = GT_FALSE;
        }

        /* Update the fields of newRangePtr     */
        newRangePtr->startAddr  = startAddr;
        newRangePtr->mask       = (GT_U8)(curRangePtr->mask | mask);
        newRangePtr->pointerType = rangeInfoPtr->pointerType;
        newRangePtr->lowerLpmPtr = rangeInfoPtr->lowerLpmPtr;
        /* new pointer so we need to update in the hardware */
        newRangePtr->updateRangeInHw = GT_TRUE;

        *rangePtrPtr       = newRangePtr;
        *numOfNewRangesPtr = 1;

        /* record the pointer type ptr */
        *pointerTypePtrPtr = &(newRangePtr->pointerType);
        break;

    case PRV_CPSS_DXCH_LPM_RAM_SPLIT_MID_SPLIT_E:   /* Two new node should be added, one for the    */
                                                    /* second part of curRangePtr, and one for the  */
                                                    /* range created by the new inserted prefix.    */
        if((newRangePtr = cpssOsLpmMalloc(sizeof(PRV_CPSS_DXCH_LPM_RAM_RANGE_SHADOW_STC)))
           == NULL)
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_OUT_OF_CPU_MEM, LOG_ERROR_NO_MSG);


        if((newRangePtr->next = cpssOsLpmMalloc(sizeof(PRV_CPSS_DXCH_LPM_RAM_RANGE_SHADOW_STC)))
           == NULL)
        {
            cpssOsLpmFree(newRangePtr);
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_OUT_OF_CPU_MEM, LOG_ERROR_NO_MSG);

        }


        /* Insert the two new ranges after curRangePtr     */
        newRangePtr->next->next = curRangePtr->next;
        curRangePtr->next = newRangePtr;
        if (splittedRangeNeedsHwUpdate == GT_TRUE)
        {
            curRangePtr->updateRangeInHw = GT_TRUE;
        }
        else
        {
            /* no new pointer in the old range */
            curRangePtr->updateRangeInHw = GT_FALSE;
        }


        /* Update the fields of the first added range   */
        newRangePtr->startAddr  = startAddr;
        newRangePtr->mask       = (GT_U8)(curRangePtr->mask | mask);

        newRangePtr->pointerType = rangeInfoPtr->pointerType;
        newRangePtr->lowerLpmPtr = rangeInfoPtr->lowerLpmPtr;
        /* new pointer so we need to update in the hardware */
        newRangePtr->updateRangeInHw = GT_TRUE;

        /* record the pointer type ptr */
        *pointerTypePtrPtr = &(newRangePtr->pointerType);

        /* Update the fields of the second added range  */
        newRangePtr             = newRangePtr->next;
        newRangePtr->startAddr  = (GT_U8)(endAddr + 1);
        newRangePtr->mask       = curRangePtr->mask;

        newRangePtr->pointerType = curRangePtr->pointerType;
        newRangePtr->lowerLpmPtr = curRangePtr->lowerLpmPtr;

        if (splittedRangeNeedsHwUpdate == GT_TRUE)
        {
            newRangePtr->updateRangeInHw = GT_TRUE;
        }
        else
        {
            /* the new pointer is the not new just copied, no need to update in Hw*/
            newRangePtr->updateRangeInHw = GT_FALSE;
        }

        *rangePtrPtr = newRangePtr;
        *numOfNewRangesPtr = 2;

        break;
    }

    return GT_OK;
}

/*******************************************************************************
* updateMirrorRange
*
* DESCRIPTION:
*       This function updates just one range in a bucket of the lpm Trie.
*       this function is used only in the case the range points to another
*       bucket.
*
* INPUTS:
*       bucketPtr           - Pointer to the bucket pointed by the range
*       lpmEngineMemPtrPtr  - points to a an PRV_CPSS_DXCH_LPM_RAM_MEM_INFO_STC
*                             which holds all the memory information needed for
*                             where and how to allocate search memory.
*       shadowPtr           - the shadow relevant for the devices asked to act on.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK on success,
*       GT_FAIL - otherwise
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS updateMirrorRange
(
    IN PRV_CPSS_DXCH_LPM_RAM_BUCKET_SHADOW_STC    *bucketPtr,
    IN PRV_CPSS_DXCH_LPM_RAM_MEM_INFO_STC         **lpmEngineMemPtrPtr,
    IN PRV_CPSS_DXCH_LPM_RAM_SHADOW_STC           *shadowPtr
)
{
    GT_U32 i;
    GT_U8  *shareDevsList;  /* List of devices sharing this LPM structure   */
    GT_U32 shareDevListLen;
    GT_STATUS retVal = GT_OK;

    CPSS_DXCH_LPM_NODE_NEXT_POINTER_STC nextPointer;

    PRV_CPSS_DXCH_LPM_RAM_RANGE_SHADOW_STC   range;

    if (bucketPtr->pointingRangeMemAddr == 0)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_FAIL, LOG_ERROR_NO_MSG);
    }

    shareDevsList   = shadowPtr->workDevListPtr->shareDevs;
    shareDevListLen = shadowPtr->workDevListPtr->shareDevNum;

    range.lowerLpmPtr.nextBucket = bucketPtr;
    range.pointerType = bucketPtr->bucketType;

    retVal = prvCpssDxChLpmRamMngRangeInNextPointerFormatSet(&range,
                            lpmEngineMemPtrPtr[0], &nextPointer);

    if (GT_OK != retVal )
    {
        return retVal;
    }

    for (i = 0; i < shareDevListLen; i++)
    {
        retVal =
            cpssDxChLpmNodeNextPointersWrite(shareDevsList[i],
                                             bucketPtr->pointingRangeMemAddr,
                                             1, &nextPointer);
        if (retVal != GT_OK)
        {
            break;
        }
    }

    return (retVal);
}

/*******************************************************************************
* getMirrorBucketInfo
*
* DESCRIPTION:
*       This function gets a mirrored bucket info
*
* INPUTS:
*       bucketPtr           - Pointer to the bucket.
*       lpmEngineMemPtrPtr  - points to a an PRV_CPSS_DXCH_LPM_RAM_MEM_INFO_STC
*                             which holds all the memory information needed for
*                             where and how to allocate search memory.
*       newBucketType       - the bucket's new type
*
* OUTPUTS:
*       memSizePtr      - the memory size this bucket needs (in LPM lines)
*       writeOffsetPtr  - the write offset in the bucket where the ranges start
*       fifthStartAddrPtr  - the calculated fifth range address.
*
* RETURNS:
*       GT_OK on success, or
*       GT_FAIL - otherwise
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS getMirrorBucketInfo
(
    IN  PRV_CPSS_DXCH_LPM_RAM_BUCKET_SHADOW_STC   *bucketPtr,
    IN  PRV_CPSS_DXCH_LPM_RAM_MEM_INFO_STC        **lpmEngineMemPtrPtr,
    IN  CPSS_DXCH_LPM_NEXT_POINTER_TYPE_ENT       newBucketType,
    OUT GT_U32                                    *memSizePtr,
    OUT GT_U32                                    *writeOffsetPtr,
    OUT GT_U8                                     *fifthStartAddrPtr
)
{
    /* warnings fix */
    lpmEngineMemPtrPtr = lpmEngineMemPtrPtr;

    switch (newBucketType)
    {
    case (CPSS_DXCH_LPM_COMPRESSED_1_NODE_PTR_TYPE_E):
        *memSizePtr = PRV_CPSS_DXCH_LPM_RAM_BUCKET_BIT_VEC_SIZE_COMPRESSED_1_CNS + bucketPtr->numOfRanges;
        /* One word for the bit-vector + place for next pointers.   */
        *writeOffsetPtr = PRV_CPSS_DXCH_LPM_RAM_BUCKET_BIT_VEC_SIZE_COMPRESSED_1_CNS;
        break;

    case (CPSS_DXCH_LPM_COMPRESSED_2_NODE_PTR_TYPE_E):
        *memSizePtr = PRV_CPSS_DXCH_LPM_RAM_BUCKET_BIT_VEC_SIZE_COMPRESSED_2_CNS + bucketPtr->numOfRanges;
        /* 2 words for the bit-vector + place for next pointers.   */
        *writeOffsetPtr = PRV_CPSS_DXCH_LPM_RAM_BUCKET_BIT_VEC_SIZE_COMPRESSED_2_CNS;

        /* this is the only place where the fifth address is valid - so retrive it:
                             range idx <0>      <1>   <2>   <3>   <4>   <5> */
        *fifthStartAddrPtr = bucketPtr->rangeList->next->next->next->next->next->startAddr;
        break;

    case (CPSS_DXCH_LPM_REGULAR_NODE_PTR_TYPE_E):
        *memSizePtr = PRV_CPSS_DXCH_LPM_RAM_BUCKET_BIT_VEC_SIZE_REGULAR_CNS + bucketPtr->numOfRanges;
        /* 11 words for the bit-vector + place for next pointers.   */
        *writeOffsetPtr = PRV_CPSS_DXCH_LPM_RAM_BUCKET_BIT_VEC_SIZE_REGULAR_CNS;
        break;

    default:
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_FAIL, LOG_ERROR_NO_MSG);
    }

    return (GT_OK);
}

/*******************************************************************************
* getMirrorBucketDataAndUpdateRangesAddress
*
* DESCRIPTION:
*       This function gets a the bucket's shadow data and formats accordingly
*       the bitvetor and the nexy pointer array.
*       it also updates the ranges hw address for the pointed buckets.
*
* INPUTS:
*       rangePtr              - Pointer to the bucket's first range.
*       lpmEngineMemPtrPtr    - points to a an PRV_CPSS_DXCH_LPM_RAM_MEM_INFO_STC
*                               which holds all the memory information needed
*                               for where and how to allocate search memory.
*       newBucketType         - the bucket's new type
*       bucketBaseAddress     - the bucket's base address
*
* OUTPUTS:
*       rangeSelectSecPtr     - range selection section (bitmap)
*       nextPointerArrayPtr   - the next pointer array.
*
* RETURNS:
*       GT_OK on success, or
*       GT_FAIL - otherwise
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS getMirrorBucketDataAndUpdateRangesAddress
(
    IN  PRV_CPSS_DXCH_LPM_RAM_RANGE_SHADOW_STC          *rangePtr,
    IN  PRV_CPSS_DXCH_LPM_RAM_MEM_INFO_STC              **lpmEngineMemPtrPtr,
    IN  CPSS_DXCH_LPM_NEXT_POINTER_TYPE_ENT             newBucketType,
    IN  GT_U32                                          bucketBaseAddress,
    OUT CPSS_DXCH_LPM_NODE_RANGE_SELECTION_UNT          *rangeSelectSecPtr,
    OUT CPSS_DXCH_LPM_NODE_NEXT_POINTER_STC             *nextPointerArrayPtr
)
{
    GT_STATUS retVal = GT_OK;
    GT_U32 bitCount;        /* Will hold the number of sets bits when calcu-*/
                            /* lating the bit vector for regular-buckets.   */
    GT_U32 line;            /* Holds the bit vector line number, when prepa-*/
                            /* ring the bit vector for regular-buckets.     */
    GT_U32 j;
    GT_U32 maxNumOfRanges;

    GT_U8 *rangeStartAddrArray1_4,*rangeStartAddrArray6_9;
    CPSS_DXCH_LPM_NODE_NEXT_POINTER_STC *nextPointerPtr;

    CPSS_DXCH_LPM_REGULAR_NODE_BIT_VECTOR_ENTRY_STC *bitVecEntryPtr;
    GT_U32 bitmapSize = 0;
    nextPointerPtr = nextPointerArrayPtr;
    rangeStartAddrArray6_9 = NULL;

    switch (newBucketType)
    {
    case (CPSS_DXCH_LPM_COMPRESSED_1_NODE_PTR_TYPE_E):
    case (CPSS_DXCH_LPM_COMPRESSED_2_NODE_PTR_TYPE_E):

        if (newBucketType == CPSS_DXCH_LPM_COMPRESSED_1_NODE_PTR_TYPE_E)
        {
            rangeStartAddrArray1_4 = rangeSelectSecPtr->compressed1BitVector.ranges1_4;
            maxNumOfRanges = MAX_NUMBER_OF_COMPRESSED_1_RANGES_CNS;
        }
        else
        {
            rangeStartAddrArray1_4 = rangeSelectSecPtr->compressed2BitVector.ranges1_4;
            rangeStartAddrArray6_9 = rangeSelectSecPtr->compressed2BitVector.ranges6_9;
            maxNumOfRanges = MAX_NUMBER_OF_COMPRESSED_2_RANGES_CNS;
        }

        /* go over the 5 possible ranges and format them for hw writing */
        for (j = 0; j < maxNumOfRanges; j++)
        {
            if (rangePtr == NULL)
            {
                continue;
            }
            /* we don't need it now so remove the flag*/
            rangePtr->updateRangeInHw = GT_FALSE;

            /* update the range's bucket parent range memory address */
            if ((rangePtr->pointerType != CPSS_DXCH_LPM_ROUTE_ENTRY_NEXT_PTR_TYPE_E) &&
                (rangePtr->pointerType != CPSS_DXCH_LPM_ECMP_ENTRY_PTR_TYPE_E) &&
                (rangePtr->pointerType != CPSS_DXCH_LPM_QOS_ENTRY_PTR_TYPE_E))
            {
                if (newBucketType == CPSS_DXCH_LPM_REGULAR_NODE_PTR_TYPE_E)
                    bitmapSize = PRV_CPSS_DXCH_LPM_RAM_BUCKET_BIT_VEC_SIZE_REGULAR_CNS;
                else if (newBucketType == CPSS_DXCH_LPM_COMPRESSED_1_NODE_PTR_TYPE_E)
                    bitmapSize = PRV_CPSS_DXCH_LPM_RAM_BUCKET_BIT_VEC_SIZE_COMPRESSED_1_CNS;
                else /* (newBucketType == CPSS_DXCH_LPM_COMPRESSED_2_NODE_PTR_TYPE_E) */
                    bitmapSize = PRV_CPSS_DXCH_LPM_RAM_BUCKET_BIT_VEC_SIZE_COMPRESSED_2_CNS;

                rangePtr->lowerLpmPtr.nextBucket->pointingRangeMemAddr =
                    bucketBaseAddress + (j + bitmapSize);
            }

            /* we overlook range 0 start address since it's always 0 and there
               is no place for it in the HW */
            if ((j > 0) && (j < 5))
            {
                rangeStartAddrArray1_4[j-1] = rangePtr->startAddr;
            }
            /* here we overlook range 5 start address since it's recorded in the
               pointing pointer */
            else if (j > 5)
            {
                if (rangeStartAddrArray6_9 == NULL)
                    CPSS_LOG_ERROR_AND_RETURN_MAC(GT_FAIL, LOG_ERROR_NO_MSG);

                rangeStartAddrArray6_9[j-6] = rangePtr->startAddr;
            }

            /* format the range */
            retVal = prvCpssDxChLpmRamMngRangeInNextPointerFormatSet(rangePtr,
                                                                     lpmEngineMemPtrPtr[1],
                                                                     nextPointerPtr);
            if (retVal != GT_OK)
            {
                return retVal;
            }

            /* advance the pointers */
            nextPointerPtr++;
            rangePtr = rangePtr->next;
        }
        break;
    case (CPSS_DXCH_LPM_REGULAR_NODE_PTR_TYPE_E):

        bitVecEntryPtr = rangeSelectSecPtr->regularBitVector.bitVectorEntry;

        line = 1;
        j = 0;
        bitCount = 0;
        bitVecEntryPtr[0].rangeCounter = 10;/* First bitvectore counter contains
                                               the offset to the first next hop
                                               pointer.             */
        while (rangePtr != NULL)
        {
            /* we don't need it now so remove the flag*/
            rangePtr->updateRangeInHw = GT_FALSE;

            /* update the range's bucket parent range memory address */
            if ((rangePtr->pointerType != CPSS_DXCH_LPM_ROUTE_ENTRY_NEXT_PTR_TYPE_E) &&
                (rangePtr->pointerType != CPSS_DXCH_LPM_ECMP_ENTRY_PTR_TYPE_E) &&
                (rangePtr->pointerType != CPSS_DXCH_LPM_QOS_ENTRY_PTR_TYPE_E))
            {
                rangePtr->lowerLpmPtr.nextBucket->pointingRangeMemAddr =
                    bucketBaseAddress +
                    (j + PRV_CPSS_DXCH_LPM_RAM_BUCKET_BIT_VEC_SIZE_REGULAR_CNS);
            }

            while (rangePtr->startAddr >= 24 * line)
            {
                bitVecEntryPtr[line].rangeCounter =
                    (GT_U8)(bitVecEntryPtr[line-1].rangeCounter + bitCount - 1);
                bitCount = 0;
                line++;
            }

            retVal = prvCpssDxChLpmRamMngRangeInNextPointerFormatSet(rangePtr,
                                                                     lpmEngineMemPtrPtr[1],
                                                                     nextPointerPtr);
            if (retVal != GT_OK)
            {
                return retVal;
            }

            bitVecEntryPtr[line - 1].bitMap |= (1 << (rangePtr->startAddr % 24));
            j++;
            bitCount++;
            nextPointerPtr++;
            rangePtr = rangePtr->next;
        }

        while (line < PRV_CPSS_DXCH_LPM_RAM_BUCKET_BIT_VEC_SIZE_REGULAR_CNS)
        {
            bitVecEntryPtr[line].rangeCounter =
                (GT_U8)(bitVecEntryPtr[line-1].rangeCounter + bitCount - 1);
            bitCount = 0;
            line++;
        }

        break;

    default:
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_FAIL, LOG_ERROR_NO_MSG);
    }
    return (GT_OK);
}

/*******************************************************************************
* updateMirrorBucket
*
* DESCRIPTION:
*       This function creates a mirrored bucket in CPU's memory, and updates
*       the lpm structures in PP's memory.
*
* INPUTS:
*       bucketPtr             - Pointer to the bucket to create a mirror from.
*       bucketUpdateMode      - Indicates whether to write the table to the device's
*                               Ram, update the memory alloc , or do nothing.
*       isDestTreeRootBucket  - Indicates whether the bucket is the root of the
*                               destination address tree
*       resizeBucket          - Indicates whether the bucket was resized or not
*                               during the insertion / deletion process.
*       forceWriteWholeBucket - force writing of the whole bucket and not just
*                               the ranges where the writeRangeInHw is set.
*       indicateSiblingUpdate - whether to update siblings of buckets (used in
*                               bulk mode)
*       useCompressed2        - whether to use compressed 2 type.
*       lpmEngineMemPtrPtr    - points to a an PRV_CPSS_DXCH_LPM_RAM_MEM_INFO_STC
*                               which holds all the memory information needed
*                               for where and how to allocate search memory.
*       bucketTypePtr         - The bucket type of the mirrored bucket.
*       shadowPtr             - the shadow relevant for the devices asked to act on.
*       parentWriteFuncPtr    - the bucket's parent write function in case there is
*                               a need to update the packet's parent ptr data and
*                               the parent is not a LPM trie range.
*                               (relevant only in delete operations)
*
* OUTPUTS:
*       bucketTypePtr         - The bucket type of the mirrored bucket.
*
* RETURNS:
*       GT_OK on success, or
*       GT_OUT_OF_CPU_MEM - if failed to allocate CPU memory, or
*       GT_OUT_OF_PP_MEM - if failed to allocate PP memory.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS updateMirrorBucket
(
    IN PRV_CPSS_DXCH_LPM_RAM_BUCKET_SHADOW_STC  *bucketPtr,
    IN LPM_BUCKET_UPDATE_MODE_ENT               bucketUpdateMode,
    IN GT_BOOL                                  isDestTreeRootBucket,
    IN GT_BOOL                                  resizeBucket,
    IN GT_BOOL                                  forceWriteWholeBucket,
    IN GT_BOOL                                  indicateSiblingUpdate,
    IN GT_BOOL                                  useCompressed2,
    IN PRV_CPSS_DXCH_LPM_RAM_MEM_INFO_STC       **lpmEngineMemPtrPtr,
    INOUT CPSS_DXCH_LPM_NEXT_POINTER_TYPE_ENT   *bucketTypePtr,
    IN PRV_CPSS_DXCH_LPM_RAM_SHADOW_STC         *shadowPtr,
    IN PRV_CPSS_DXCH_LPM_RAM_BUCKET_PARENT_WRITE_FUNC_INFO_STC *parentWriteFuncPtr
)
{
    GT_STATUS retVal = GT_OK;
    GT_U32 i;
    GT_U32 memSize = 0;/* Size of block to be written to RAM.          */

    GT_UINTPTR oldMemPool = 0;  /* The memory pool from which the old buckets   */
                            /* where allocated.                             */

    GT_U8  *shareDevsList;  /* List of devices sharing this LPM structure   */
    GT_U32 shareDevListLen;

    GT_UINTPTR tempHwAddrHandle;/* Will hold the allocated area in PP's RAM to  */
                                /* which the buckets will be written.           */

    GT_U32 memBlockBase = 0;/* the base address (offset) in the RAM for bucket*/

    PRV_CPSS_DXCH_LPM_RAM_MEM_INFO_STC  *tempNextMemInfoPtr;/* use for going over the list of blocks per octet */
    GT_UINTPTR                           tmpStructsMemPool;/* use for going over the list of blocks per octet */

    /* the fields for writing the bucket to the HW */
    static CPSS_DXCH_LPM_NODE_RANGE_SELECTION_UNT rangeSelectionSec;
    static CPSS_DXCH_LPM_NODE_NEXT_POINTER_STC nextPointersArray[256];

    PRV_CPSS_DXCH_LPM_RAM_RANGE_SHADOW_STC   *pRange;
    GT_U32 tempAddr = 0;     /* Temporary address                */
    GT_U8 fifthStartAddr = 0;/* The fifth start address, in case of LPM  */
                             /* Bucket type-2 compressed.                */

    GT_U32 writeOffset = 0; /* Holds the offset from the bucket pointer from*/
                            /* which we need to start writing in case there */
                            /* is no need to resize                         */
    GT_U32 inBucketWriteOffset;
    PRV_CPSS_DXCH_LPM_RAM_MEM_LIST_ENTRY_STC     **freeMemListPtr;

    GT_BOOL swapMemInUse = GT_FALSE;
    GT_BOOL freeOldMem = GT_TRUE;

    CPSS_DXCH_LPM_NEXT_POINTER_TYPE_ENT newBucBucketType;

    GT_UINTPTR oldHwAddrHandle;
    indicateSiblingUpdate = indicateSiblingUpdate; /* prevent warning */

    shareDevsList   = shadowPtr->workDevListPtr->shareDevs;
    shareDevListLen = shadowPtr->workDevListPtr->shareDevNum;

    /* Determine the bucket type of the new bucket. */
    if(bucketPtr->numOfRanges == 1)
    {
        if (isDestTreeRootBucket)
        {
            newBucBucketType = CPSS_DXCH_LPM_COMPRESSED_1_NODE_PTR_TYPE_E;
        }
        else
        {
            newBucBucketType = CPSS_DXCH_LPM_ROUTE_ENTRY_NEXT_PTR_TYPE_E;
        }
    }
    else if(bucketPtr->numOfRanges <= MAX_NUMBER_OF_COMPRESSED_1_RANGES_CNS)    /* 1-line compressed bucket */
    {
        newBucBucketType = CPSS_DXCH_LPM_COMPRESSED_1_NODE_PTR_TYPE_E;
    }
    else if((bucketPtr->numOfRanges <= MAX_NUMBER_OF_COMPRESSED_2_RANGES_CNS) &&
            (useCompressed2 == GT_TRUE))   /* 2-line compressed bucket */
    {
        newBucBucketType = CPSS_DXCH_LPM_COMPRESSED_2_NODE_PTR_TYPE_E;
    }
    else                                    /* Regular bucket           */
    {
        newBucBucketType = CPSS_DXCH_LPM_REGULAR_NODE_PTR_TYPE_E;
    }

    /* check the case of no hw update */
    if((bucketUpdateMode == LPM_BUCKET_UPDATE_ROOT_BUCKET_SHADOW_ONLY_E)  ||
       (bucketUpdateMode == LPM_BUCKET_UPDATE_NONE_E))
    {
        /* all buckets that are visited during the shadow update stage of
           the bulk operation should be scanned during the hardware update stage;
           therefore don't leave buckets with PRV_CPSS_DXCH_LPM_RAM_BUCKET_HW_OK_E
           mark */
        if (/* (indicateSiblingUpdate == GT_TRUE) && */
            (bucketPtr->bucketHwUpdateStat == PRV_CPSS_DXCH_LPM_RAM_BUCKET_HW_OK_E))
        {
            /*indicate we need to go through this bucket in bulk update in order
              to reach it's siblings for update. */
            bucketPtr->bucketHwUpdateStat =
                PRV_CPSS_DXCH_LPM_RAM_BUCKET_HW_OK_UPDATE_SIBLINGS_E;
        }

        if (bucketUpdateMode == LPM_BUCKET_UPDATE_ROOT_BUCKET_SHADOW_ONLY_E)
        {
            /* for root bucket type we need to update the bucket type */
            *bucketTypePtr = newBucBucketType;
        }

        /* update the bucket type and exit */
        bucketPtr->bucketType = *bucketTypePtr;
        return GT_OK;
    }

    if ((bucketUpdateMode == LPM_BUCKET_UPDATE_HW_AND_MEM_RESIZE_E) &&
        (bucketPtr->bucketHwUpdateStat ==
         PRV_CPSS_DXCH_LPM_RAM_BUCKET_HW_WAS_RESIZED_AND_NEEDS_REWRITING_E))
    {
        /* we are in a bulk operation update (delete) and this bucket
           was already found to be resized , so we refer to it as need to be
           resized since it's memory hasn't been resized yet*/

        resizeBucket = GT_TRUE;
    }

    if (bucketUpdateMode == LPM_BUCKET_UPDATE_SHADOW_ONLY_E)
    {
        /* this means we are in a stage where we just update the shadow
           the only released memory here is that of empty buckets */
        freeMemListPtr = &shadowPtr->freeMemListEndOfUpdate;
    }
    else
    {
        /* check what kind of memory this bucket occupies according to it HW update
           status */
        freeMemListPtr = (bucketPtr->bucketHwUpdateStat ==
                          PRV_CPSS_DXCH_LPM_RAM_BUCKET_HW_WAS_RESIZED_AND_NEEDS_REWRITING_E)?
            &shadowPtr->freeMemListDuringUpdate:
            &shadowPtr->freeMemListEndOfUpdate;
    }

    /* If there is only one range (next hop type), then delete the
       whole bucket */
    if(newBucBucketType == CPSS_DXCH_LPM_ROUTE_ENTRY_NEXT_PTR_TYPE_E)
    {
        if (bucketPtr->bucketType != CPSS_DXCH_LPM_ROUTE_ENTRY_NEXT_PTR_TYPE_E)
        {
            retVal = prvCpssDxChLpmRamMemFreeListMng(bucketPtr->hwBucketOffsetHandle,
                                                     PRV_CPSS_DXCH_LPM_RAM_MEM_LIST_ADD_MEM_OP_E,
                                                     freeMemListPtr);
            if (retVal != GT_OK)
            {
                return retVal;
            }
        }

        bucketPtr->bucketType = CPSS_DXCH_LPM_ROUTE_ENTRY_NEXT_PTR_TYPE_E;
        /* in case of PRV_CPSS_DXCH_LPM_RAM_TRIE_PTR_TYPE_E pointer type
           shouldn't be updated because it represents that it points to a src
           trie (and not the actuall type of the bucket) */
        if ((GT_U32)*bucketTypePtr != (GT_U32)PRV_CPSS_DXCH_LPM_RAM_TRIE_PTR_TYPE_E)
            *bucketTypePtr = CPSS_DXCH_LPM_ROUTE_ENTRY_NEXT_PTR_TYPE_E;
        bucketPtr->fifthAddress = 0;
        bucketPtr->hwBucketOffsetHandle = 0;

        return GT_OK;
    }

    memBlockBase = lpmEngineMemPtrPtr[0]->structsBase;

    retVal = getMirrorBucketInfo(bucketPtr,lpmEngineMemPtrPtr,newBucBucketType,&memSize,
                                 &writeOffset,&fifthStartAddr);
    if (retVal != GT_OK)
    {
        return retVal;
    }

    /* Allocate memory in PP's RAM if it's resized or never been allocated
       before (hotSync)*/
    if((bucketUpdateMode != LPM_BUCKET_UPDATE_SHADOW_ONLY_E) &&
       ((resizeBucket == GT_TRUE) || (bucketPtr->hwBucketOffsetHandle == 0)))
    {
        /* first check if memory was already allocated for these changes */
        /* if yes , use only that memory! */
        if (shadowPtr->neededMemoryListLen > 0)
        {
            i = shadowPtr->neededMemoryCurIdx;

            if ((i >= shadowPtr->neededMemoryListLen) ||
                (memSize != shadowPtr->neededMemoryBlocksSizes[i]))
                CPSS_LOG_ERROR_AND_RETURN_MAC(GT_FAIL, LOG_ERROR_NO_MSG); /* should not happen */

            tempHwAddrHandle = shadowPtr->neededMemoryBlocks[i];
            shadowPtr->neededMemoryCurIdx++;
        }
        else
        {            
            tempNextMemInfoPtr = lpmEngineMemPtrPtr[0];            
            tmpStructsMemPool = lpmEngineMemPtrPtr[0]->structsMemPool;
            /*  this can only happen in lpm Delete, since in an insert 
                the memory is preallocated. 
                this forces use to use the swap memory , which we will
                swap back after using */
            if (lpmEngineMemPtrPtr[0]->structsMemPool==0)
            {
                tempHwAddrHandle = DMM_BLOCK_NOT_FOUND;
            }
            else
            {            
                do
                {                
                    tempHwAddrHandle = prvCpssDmmAllocate(tmpStructsMemPool, 
                                                         DMM_MIN_ALLOCATE_SIZE_IN_BYTE_CNS * memSize,
                                                         DMM_MIN_ALLOCATE_SIZE_IN_BYTE_CNS);

                    if (tempNextMemInfoPtr->nextMemInfoPtr == NULL)
                    {
                        break;
                    }
                    tempNextMemInfoPtr = tempNextMemInfoPtr->nextMemInfoPtr;
                    tmpStructsMemPool = tempNextMemInfoPtr->structsMemPool; 

                } while (tempHwAddrHandle==DMM_BLOCK_NOT_FOUND);
            }

            if (tempHwAddrHandle == DMM_BLOCK_NOT_FOUND) 
            {
                /* we reached a max fragmentation in the PP's memory*/
                /* this can only happen in lpm Delete, since in an insert */
                /* the memory is preallocated. */
                /* this forces use to use the swap memory , which we will*/
                /* swap back after using */
                tempHwAddrHandle = shadowPtr->swapMemoryAddr;

                if (tempHwAddrHandle == 0)
                {
                    CPSS_LOG_ERROR_AND_RETURN_MAC(((GT_STATUS)GT_ERROR), LOG_ERROR_NO_MSG);
                }

                swapMemInUse = GT_TRUE;
            }
            else
            {
                /* we had a CPU Memory allocation error */
                if(tempHwAddrHandle == DMM_MALLOC_FAIL)
                {
                    CPSS_LOG_ERROR_AND_RETURN_MAC(GT_OUT_OF_CPU_MEM, LOG_ERROR_NO_MSG);
                }      
            }
        }
    }                        
    else
    {
        tempHwAddrHandle = bucketPtr->hwBucketOffsetHandle;
    }

    /* update the bucket's hw status */
    if (resizeBucket == GT_TRUE)
    {
        bucketPtr->bucketHwUpdateStat =
            PRV_CPSS_DXCH_LPM_RAM_BUCKET_HW_WAS_RESIZED_AND_NEEDS_REWRITING_E;
    }
    else
    {
        if ((bucketPtr->bucketHwUpdateStat == PRV_CPSS_DXCH_LPM_RAM_BUCKET_HW_OK_E) ||
            (bucketPtr->bucketHwUpdateStat == PRV_CPSS_DXCH_LPM_RAM_BUCKET_HW_OK_UPDATE_SIBLINGS_E))
        {
            /* the bucket wasn't resized , but we reached here so it needs
               re-writing in the HW (some pointers were changed in it) */
            bucketPtr->bucketHwUpdateStat = PRV_CPSS_DXCH_LPM_RAM_BUCKET_HW_NEEDS_UPDATE_E;
        }
    }

    /* Write the tables to PP's RAM for each device that share this LPM table. */

    if ((bucketUpdateMode == LPM_BUCKET_UPDATE_HW_AND_MEM_ALLOC_E) ||
        (bucketUpdateMode == LPM_BUCKET_UPDATE_HW_AND_MEM_RESIZE_E))
    {
        pRange = bucketPtr->rangeList;
        /* Preparation of HW words.     */

        if ((resizeBucket == GT_TRUE) ||
            (forceWriteWholeBucket == GT_TRUE) ||
            (bucketPtr->bucketHwUpdateStat ==
             PRV_CPSS_DXCH_LPM_RAM_BUCKET_HW_WAS_RESIZED_AND_NEEDS_REWRITING_E))
            /*it could be that this bucket was resized but haven't been written
              the the HW*/
        {
            /* first get the bucket base addr for sibling buckets parent
               range mem address calculation */
            if (swapMemInUse == GT_TRUE)
            {
                /* if we're using the swap we will not record it but record the
                   bucket's old memory address */
                tempAddr = (PRV_CPSS_DXCH_LPM_RAM_GET_LPM_OFFSET_FROM_DMM_MAC(bucketPtr->hwBucketOffsetHandle)) +
                    memBlockBase;
            }
            else
            {
                tempAddr = (PRV_CPSS_DXCH_LPM_RAM_GET_LPM_OFFSET_FROM_DMM_MAC(tempHwAddrHandle)) + memBlockBase;
            }

            cpssOsMemSet(&rangeSelectionSec,0,
                         sizeof(CPSS_DXCH_LPM_NODE_RANGE_SELECTION_UNT));
            /* create the bucket data, and update the ranges hw address */
            getMirrorBucketDataAndUpdateRangesAddress(pRange,
                                                      lpmEngineMemPtrPtr,
                                                      newBucBucketType,
                                                      tempAddr,
                                                      &rangeSelectionSec,
                                                      nextPointersArray);

            /* Writing to RAM.*/
            do
            {
                tempAddr = PRV_CPSS_DXCH_LPM_RAM_GET_LPM_OFFSET_FROM_DMM_MAC(tempHwAddrHandle) + memBlockBase;
                for (i = 0; i < shareDevListLen; i++)
                {

                    retVal = cpssDxChLpmNodeWrite(shareDevsList[i],
                                                  tempAddr,
                                                  newBucBucketType,
                                                  &rangeSelectionSec,
                                                  bucketPtr->numOfRanges,
                                                  nextPointersArray);
                    if (retVal != GT_OK)
                    {
                        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_FAIL, LOG_ERROR_NO_MSG);
                    }
                }

                if ((bucketUpdateMode == LPM_BUCKET_UPDATE_HW_AND_MEM_RESIZE_E) ||
                    (swapMemInUse == GT_TRUE))
                {
                    oldHwAddrHandle = bucketPtr->hwBucketOffsetHandle;
                     /* ok since the bucket is updated in the HW, we can now
                        update the shadow */
                    bucketPtr->fifthAddress = fifthStartAddr;
                    bucketPtr->bucketType = (GT_U8)newBucBucketType;

                    /* in case of PRV_CPSS_DXCH_LPM_RAM_TRIE_PTR_TYPE_E we
                       don't update the pointer type since it represents that it
                       points to a src trie. (and not the actuall type of the
                       bucket it points to)*/
                    if ((GT_U32)*bucketTypePtr != (GT_U32)PRV_CPSS_DXCH_LPM_RAM_TRIE_PTR_TYPE_E)
                        *bucketTypePtr = (GT_U8)newBucBucketType;
                    bucketPtr->hwBucketOffsetHandle = tempHwAddrHandle;

                    /* in this mode we immidiatly update the parent range in
                       order to free the bucket's memory for further use .the
                       same goes to if we use the swap memory */
                    if (bucketPtr->pointingRangeMemAddr == 0)
                    {
                        /* if the parentRangeMemAddr = 0 then it means the parent
                           range is not an regular lpm range, thus order it's
                           rewriting using the function ptr*/
                        if (parentWriteFuncPtr->bucketParentWriteFunc(
                            parentWriteFuncPtr->data) != GT_OK)
                        {
                            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_FAIL, LOG_ERROR_NO_MSG);
                        }
                    }
                    else
                    {
                        /* write the bucket's parent range in order to be able to
                           free this bucket's old memory / swap memory
                          (resizing took place)*/
                        if (updateMirrorRange(bucketPtr,
                                              lpmEngineMemPtrPtr,
                                              shadowPtr)
                            != GT_OK)
                        {

                            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_FAIL, LOG_ERROR_NO_MSG);
                        }
                    }

                    /* now it's ok to free this bucket's old memory */
                    if ((freeOldMem == GT_TRUE) &&
                        (bucketPtr->bucketType != CPSS_DXCH_LPM_ROUTE_ENTRY_NEXT_PTR_TYPE_E) &&
                        (bucketPtr->bucketType != CPSS_DXCH_LPM_ECMP_ENTRY_PTR_TYPE_E) &&
                        (bucketPtr->bucketType != CPSS_DXCH_LPM_QOS_ENTRY_PTR_TYPE_E))
                    {
                        oldMemPool = DMM_GET_PARTITION(oldHwAddrHandle);
                        prvCpssDmmFree(oldHwAddrHandle);

                        freeOldMem = GT_FALSE;

                    }
                }

                if (swapMemInUse == GT_FALSE)
                    break;

                /* if we reached here the swap memory is in use , and there is
                   a need to move and rewrite the bucket back where it was */

                if (oldMemPool == 0)
                {
                    /* should never happen since we just freed this mem*/
                    CPSS_LOG_ERROR_AND_RETURN_MAC(GT_FAIL, LOG_ERROR_NO_MSG);
                }

                tempHwAddrHandle = prvCpssDmmAllocate(oldMemPool,
                                                      DMM_MIN_ALLOCATE_SIZE_IN_BYTE_CNS * memSize,
                                                      DMM_MIN_ALLOCATE_SIZE_IN_BYTE_CNS);
                if ((tempHwAddrHandle == DMM_BLOCK_NOT_FOUND)||(tempHwAddrHandle == DMM_MALLOC_FAIL))
                {
                    /* should never happen since we just freed this mem*/
                    CPSS_LOG_ERROR_AND_RETURN_MAC(GT_FAIL, LOG_ERROR_NO_MSG);
                }

                /* indicate we're not using the swap memory anymore*/
                swapMemInUse = GT_FALSE;

            } while (GT_TRUE);
        }
        else
        {
            inBucketWriteOffset = 0;
            while (pRange != NULL)
            {
                /* only if the range needs updating , update it */
                if (pRange->updateRangeInHw == GT_TRUE)
                {
                    retVal =
                        prvCpssDxChLpmRamMngRangeInNextPointerFormatSet(pRange,
                                                                        lpmEngineMemPtrPtr[1],
                                                                        nextPointersArray);
                    if (retVal != GT_OK)
                    {
                        return retVal;
                    }

                    tempAddr = (writeOffset + inBucketWriteOffset) +
                              PRV_CPSS_DXCH_LPM_RAM_GET_LPM_OFFSET_FROM_DMM_MAC(tempHwAddrHandle) + memBlockBase;

                    /* update the range's bucket parent range memory address */
                    if ((pRange->pointerType != CPSS_DXCH_LPM_ROUTE_ENTRY_NEXT_PTR_TYPE_E) &&
                        (pRange->pointerType != CPSS_DXCH_LPM_ECMP_ENTRY_PTR_TYPE_E) &&
                        (pRange->pointerType != CPSS_DXCH_LPM_QOS_ENTRY_PTR_TYPE_E))
                    {
                        pRange->lowerLpmPtr.nextBucket->pointingRangeMemAddr =
                            tempAddr;
                    }

                    for (i = 0; i < shareDevListLen; i++)
                    {
                        retVal =
                            cpssDxChLpmNodeNextPointersWrite(shareDevsList[i],
                                                             tempAddr,
                                                             1,
                                                             nextPointersArray);
                        if (retVal != GT_OK)
                        {
                            return (retVal);
                        }
                    }

                    /*  remove tha flag */
                    pRange->updateRangeInHw = GT_FALSE;
                }

                inBucketWriteOffset++;
                pRange = pRange->next;
            }
        }

        /* the bucket's HW update was done. no need to further update it */
        bucketPtr->bucketHwUpdateStat = PRV_CPSS_DXCH_LPM_RAM_BUCKET_HW_OK_E;
    }

    if ((bucketUpdateMode == LPM_BUCKET_UPDATE_HW_AND_MEM_ALLOC_E) ||
        (bucketUpdateMode == LPM_BUCKET_UPDATE_MEM_ONLY_E))
    {
        /* in cases we deal with memory allocation, check if we need to free the
           old bucket */
        freeOldMem = ((freeOldMem) &&
                      (resizeBucket == GT_TRUE) &&
                      (bucketPtr->bucketType != CPSS_DXCH_LPM_ROUTE_ENTRY_NEXT_PTR_TYPE_E) &&
                      (bucketPtr->bucketType != CPSS_DXCH_LPM_ECMP_ENTRY_PTR_TYPE_E) &&
                      (bucketPtr->bucketType != CPSS_DXCH_LPM_QOS_ENTRY_PTR_TYPE_E)); /* not a new bucket */

        if (freeOldMem)
        {
            /* Free the old bucket.  */
            retVal = prvCpssDxChLpmRamMemFreeListMng(bucketPtr->hwBucketOffsetHandle,
                                                     PRV_CPSS_DXCH_LPM_RAM_MEM_LIST_ADD_MEM_OP_E,
                                                     freeMemListPtr);
            if (retVal != GT_OK)
            {
                return retVal;
            }
        }

        bucketPtr->hwBucketOffsetHandle = tempHwAddrHandle;
    }

    /* in LPM_BUCKET_UPDATE_HW_AND_MEM_RESIZE_E mode , this shadow update part
       already been done */
    if ((bucketUpdateMode == LPM_BUCKET_UPDATE_HW_AND_MEM_ALLOC_E) ||
        (bucketUpdateMode == LPM_BUCKET_UPDATE_MEM_ONLY_E) ||
        (bucketUpdateMode == LPM_BUCKET_UPDATE_SHADOW_ONLY_E))
    {
        /* update the bucket's info info */
        bucketPtr->fifthAddress = fifthStartAddr;
        bucketPtr->bucketType = newBucBucketType;

        /* update the range lower Level bucket type */
        /* in case of PRV_CPSS_DXCH_LPM_RAM_TRIE_PTR_TYPE_E we don't update
           the lower Level bucket type (pointer type) since it represents that
           it points to a src trie. (and not the actuall type of the bucket it
           points to)*/
        if ((GT_U32)*bucketTypePtr != (GT_U32)PRV_CPSS_DXCH_LPM_RAM_TRIE_PTR_TYPE_E)
            *bucketTypePtr = newBucBucketType;
    }

    return GT_OK;
}

/*******************************************************************************
* prvCpssDxChLpmRamMngCreateNew
*
* DESCRIPTION:
*       This function creates a new LPM structure, with the pair (0,0) as the
*       default route entry.
*
* INPUTS:
*       nextHopPtr          - A pointer to the next hop entry to be set in the
*                             nextHopEntry field.
*       firstLevelPrefixLen - The first lpm level prefix.
*
*
* RETURNS:
*       A pointer to the new created Bucket, or NULL if allocation failed.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
PRV_CPSS_DXCH_LPM_RAM_BUCKET_SHADOW_STC* prvCpssDxChLpmRamMngCreateNew
(
    IN PRV_CPSS_DXCH_LPM_ROUTE_ENTRY_POINTER_STC *nextHopPtr,
    IN GT_U8 firstLevelPrefixLen
)
{
    PRV_CPSS_DXCH_LPM_RAM_BUCKET_SHADOW_STC *pBucket; /* The first level bucket */

    PRV_CPSS_DXCH_LPM_RAM_RANGE_SHADOW_STC   *pRange;

    pBucket = createNewBucket(nextHopPtr,firstLevelPrefixLen);
    if(pBucket == NULL)
        return NULL;

    pRange = pBucket->rangeList;

    /* beacuse it's a new bucket there is a need to update it in the HW */
    pRange->updateRangeInHw = GT_TRUE;

    return pBucket;
}

/*******************************************************************************
* prvCpssDxChLpmRamMngInsert
*
* DESCRIPTION:
*       Insert a new entry to the LPM tables.
*
* INPUTS:
*       bucketPtr         - Pointer to the root bucket.
*       addrCurBytePtr    - The Byte Array represnting the Address associated
*                           with the entry.
*       prefix            - The address prefix.
*       nextPtr           - A pointer to a next hop/next bucket entry.
*       nextPtrType       - The type of nextPtr (can be next hop, ECMP/QoS entry
*                           or bucket)
*       rootBucketFlag    - Indicates the way to deal with a root bucket (if it is).
*       lpmEngineMemPtrPtr- points to a an array of PRV_CPSS_DXCH_LPM_RAM_MEM_INFO_STC
*                           which holds all the memory information needed for where and
*                           how to allocate search memory for each of the lpm levels
*       updateOldPtr      - (GT_TRUE) this is an update for an already existing entry.
*       bucketTypePtr     - The bucket type of the bucket inserted on the root.
*                           Possible values:
*                           REGULAR_BUCKET, COMPRESSED_1, or COMPRESSED_2.
*       shadowPtr         - the shadow relevant for the devices asked to act on.
*       insertMode        - is the function called in the course of Hot Sync,bulk
*                           operation or regular
*       isMcSrcTree       - indicates whether bucketPtr is the toor bucket of a
*                           multicast source tree
*
* OUTPUTS:
*       updateOldPtr      - (GT_TRUE) this is an update for an already existing entry.
*       bucketTypePtr     - The bucket type of the bucket inserted on the root.
*                           Possible values:
*                           REGULAR_BUCKET, COMPRESSED_1, or COMPRESSED_2.
*
* RETURNS:
*       GT_OK on success, or
*       GT_OUT_OF_CPU_MEM - if failed to allocate CPU memory, or
*       GT_OUT_OF_PP_MEM - if failed to allocate PP memory.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS prvCpssDxChLpmRamMngInsert
(
    IN    PRV_CPSS_DXCH_LPM_RAM_BUCKET_SHADOW_STC       *bucketPtr,
    IN    GT_U8                                         *addrCurBytePtr,
    IN    GT_U32                                        prefix,
    IN    GT_PTR                                        nextPtr,
    IN    CPSS_DXCH_LPM_NEXT_POINTER_TYPE_ENT           nextPtrType,
    IN    PRV_CPSS_DXCH_LPM_RAM_ROOT_BUCKET_FLAG_ENT    rootBucketFlag,
    IN    PRV_CPSS_DXCH_LPM_RAM_MEM_INFO_STC            **lpmEngineMemPtrPtr,
    INOUT GT_BOOL                                       *updateOldPtr,
    INOUT CPSS_DXCH_LPM_NEXT_POINTER_TYPE_ENT           *bucketTypePtr,
    IN    PRV_CPSS_DXCH_LPM_RAM_SHADOW_STC              *shadowPtr,
    IN    PRV_CPSS_DXCH_LPM_RAM_TRIE_INSERT_MODE_ENT    insertMode,
    IN    GT_BOOL                                       isMcSrcTree
)
{

    GT_U8  startAddr;           /* The startAddr and endAddr of the     */
    GT_U8  endAddr;             /* given prefix in the current level.   */

    GT_U32 rangePrefixLength;   /* The prefix length represented by the */
                                /* current range's mask field.          */
    GT_U32 newPrefixLength;     /* The new prefix length that the       */
                                /* current range represents.            */
    GT_U32 numOfNewRanges = 0;  /* Number of new created ranges as a    */
                                /* result of insertion.                 */
    PRV_CPSS_DXCH_LPM_RAM_RANGE_SHADOW_STC newRange;/* the new created range from the insert */
    GT_STATUS retVal= GT_OK;           /* functions returned values.           */

    PRV_CPSS_DXCH_LPM_RAM_RANGE_SHADOW_STC **pRange;
    PRV_CPSS_DXCH_LPM_RAM_RANGE_SHADOW_STC *parentRangeToUpdateDueToLowerLevelResizePtr = NULL;
    GT_U32  parentRangeLevel = 99; /* initializing with and invalid level*/
    GT_BOOL useCompressed2; /* indicates wheather to use a compressed 2 type
                               when allocting this bucket memory */
    GT_BOOL indicateSiblingUpdate = GT_FALSE;
    GT_U32  level = 0;
    GT_BOOL goDown;
    GT_BOOL isDestTreeRootBucket;

    LPM_BUCKET_UPDATE_MODE_ENT bucketUpdateMode;

    static GT_BOOL                                  resizeBucket[MAX_LPM_LEVELS_CNS];
    static GT_BOOL                                  doHwUpdate[MAX_LPM_LEVELS_CNS];
    static PRV_CPSS_DXCH_LPM_RAM_BUCKET_SHADOW_STC  *bucketPtrArry[MAX_LPM_LEVELS_CNS];
    static CPSS_DXCH_LPM_NEXT_POINTER_TYPE_ENT      *pointerTypePtrArray[MAX_LPM_LEVELS_CNS];
    static PRV_CPSS_DXCH_LPM_RAM_RANGE_SHADOW_STC   *pRangeArray[MAX_LPM_LEVELS_CNS];

    PRV_CPSS_DXCH_LPM_RAM_BUCKET_SHADOW_STC  **currBucketPtr = bucketPtrArry;
    GT_U8                                    *currAddrByte = addrCurBytePtr;
    GT_U32                                   currPrefix = prefix;
    PRV_CPSS_DXCH_LPM_RAM_MEM_INFO_STC       **currLpmEngineMemPtr = lpmEngineMemPtrPtr;
    CPSS_DXCH_LPM_NEXT_POINTER_TYPE_ENT      **currBucketPtrType = pointerTypePtrArray;
    GT_BOOL                                  *resizeBucketPtr = resizeBucket;
    GT_BOOL                                  *doHwUpdatePtr = doHwUpdate;
    GT_U32                                   i = 0;
    GT_UINTPTR                               rollBackHwBucketOffsetHandle = 0;
    LPM_BUCKET_UPDATE_MODE_ENT               rollBackBucketUpdateMode = LPM_BUCKET_UPDATE_NONE_E;
    GT_BOOL                                  rollBackResizeBucket = GT_FALSE;

    cpssOsMemSet(&newRange,0,sizeof(newRange));

    /* zero the arrays */
    cpssOsMemSet(resizeBucket,GT_FALSE,sizeof(GT_BOOL)*MAX_LPM_LEVELS_CNS);
    cpssOsMemSet(doHwUpdate,GT_FALSE,sizeof(GT_BOOL)*MAX_LPM_LEVELS_CNS);
    cpssOsMemSet(bucketPtrArry,0,
                 sizeof(PRV_CPSS_DXCH_LPM_RAM_BUCKET_SHADOW_STC*)*MAX_LPM_LEVELS_CNS);
    cpssOsMemSet(pRangeArray,0xFF, /*0xFFFFFFFF indicates untouched level*/
                 sizeof(pRangeArray));

    /* set the initial values.*/
    bucketPtrArry[0] = bucketPtr;
    pointerTypePtrArray[0] = bucketTypePtr;
    pRange = pRangeArray;

    /* first do the lpm tree shadow update part */
    while (currBucketPtr >= bucketPtrArry)
    {
        /* we start with the assumption we'll go up the bucket trie */
        goDown = GT_FALSE;

        /* if the range pointer reached NULL means that we finished with this level */
        if (*pRange != NULL)
        {
            calcStartEndAddr(*currAddrByte,currPrefix,&startAddr,&endAddr);

            /* check if we need to continue this level or this a brand new level
               (0xFFFFFFFF) */
            if (*pRange == (PRV_CPSS_DXCH_LPM_RAM_RANGE_SHADOW_STC *)(~((GT_UINTPTR)0)))
            {
                *pRange = find1stOverlap(*currBucketPtr,startAddr,NULL);
                /* If belongs to this level, insert it into trie.    */
                if (currPrefix <= PRV_CPSS_DXCH_LPM_RAM_MAX_LEVEL_LENGTH_CNS)
                {
                    retVal = insert2Trie(*currBucketPtr,startAddr,
                                         currPrefix,PRV_CPSS_DXCH_LPM_RAM_MAX_LEVEL_LENGTH_CNS,nextPtr);
                    if (retVal != GT_OK)
                    {
                        /* failed on last level*/
                        for (i=1; i <= level; i++)
                        {
                            if (*(pointerTypePtrArray[i]) == CPSS_DXCH_LPM_ROUTE_ENTRY_NEXT_PTR_TYPE_E)
                            {
                                *(pointerTypePtrArray[i]) = CPSS_DXCH_LPM_COMPRESSED_1_NODE_PTR_TYPE_E;
                            }
                        }
                        if (shadowPtr->neededMemoryListLen > 0)
                        {
                            for (i = shadowPtr->neededMemoryCurIdx; i < shadowPtr->neededMemoryListLen; i++)
                            {
                                prvCpssDmmFree(shadowPtr->neededMemoryBlocks[i]);
                            }
                        }
                        /* create mask for current range */
                        (*pRange)->mask = (GT_U8)PREFIX_2_MASK_MAC(currPrefix,PRV_CPSS_DXCH_LPM_RAM_MAX_LEVEL_LENGTH_CNS);
                        return retVal;
                    }

                }
            }

            do
            {
                rangePrefixLength = mask2PrefixLength((*pRange)->mask,
                                                      PRV_CPSS_DXCH_LPM_RAM_MAX_LEVEL_LENGTH_CNS);
                if (currPrefix >= rangePrefixLength)
                {
                    if (((*pRange)->pointerType == CPSS_DXCH_LPM_ROUTE_ENTRY_NEXT_PTR_TYPE_E) ||
                        ((*pRange)->pointerType == CPSS_DXCH_LPM_ECMP_ENTRY_PTR_TYPE_E) ||
                        ((*pRange)->pointerType == CPSS_DXCH_LPM_QOS_ENTRY_PTR_TYPE_E) ||
                        ((GT_U32)((*pRange)->pointerType) == (GT_U32)PRV_CPSS_DXCH_LPM_RAM_TRIE_PTR_TYPE_E))
                    {
                        /* Insert the given addr. to the next level.    */
                        if (currPrefix > PRV_CPSS_DXCH_LPM_RAM_MAX_LEVEL_LENGTH_CNS)
                        {
                            /* Create a new bucket with the dominant next_pointer*/
                            /* value of the upper level as the default route.    */
                            newRange.lowerLpmPtr.nextBucket =
                                createNewBucket((*pRange)->lowerLpmPtr.nextHopEntry,
                                                PRV_CPSS_DXCH_LPM_RAM_MAX_LEVEL_LENGTH_CNS);
                            if (newRange.lowerLpmPtr.nextBucket == NULL)
                            {
                                for (i=1; i <= level; i++)
                                {
                                    if (*(pointerTypePtrArray[i]) == CPSS_DXCH_LPM_ROUTE_ENTRY_NEXT_PTR_TYPE_E)
                                    {
                                        *(pointerTypePtrArray[i]) = CPSS_DXCH_LPM_COMPRESSED_1_NODE_PTR_TYPE_E;
                                    }
                                }
                                if (shadowPtr->neededMemoryListLen > 0)
                                {
                                    for (i = shadowPtr->neededMemoryCurIdx; i < shadowPtr->neededMemoryListLen; i++)
                                    {
                                        prvCpssDmmFree(shadowPtr->neededMemoryBlocks[i]);
                                    }
                                }
                                CPSS_LOG_ERROR_AND_RETURN_MAC(GT_OUT_OF_CPU_MEM, LOG_ERROR_NO_MSG);
                            }

                            /* the new range prefix len is as of what was the prefixlen*/
                            newPrefixLength = rangePrefixLength;

                            /* we set the new range pointer type to next hop
                               since we still don't know what kind of bucket it will
                               be (as in createNewBucket) */
                            newRange.pointerType = CPSS_DXCH_LPM_ROUTE_ENTRY_NEXT_PTR_TYPE_E;

                            /* Insert the given entry to the next level.     */
                            currBucketPtr[1] = newRange.lowerLpmPtr.nextBucket;

                            /* indicate we need to proceed down the bucket trie*/
                            goDown = GT_TRUE;
                        }
                        else
                        {
                            /* this is the end level. record in the new range
                               the given next ptr */
                            newRange.pointerType = (GT_U8)nextPtrType;
                            if ((nextPtrType == CPSS_DXCH_LPM_ROUTE_ENTRY_NEXT_PTR_TYPE_E) ||
                                (nextPtrType == CPSS_DXCH_LPM_ECMP_ENTRY_PTR_TYPE_E) ||
                                (nextPtrType == CPSS_DXCH_LPM_QOS_ENTRY_PTR_TYPE_E))
                            {
                                newRange.lowerLpmPtr.nextHopEntry = nextPtr;
                            }
                            else
                            {
                                /* nextPtrType ==
                                   PRV_CPSS_DXCH_LPM_RAM_TRIE_PTR_TYPE_E */
                                newRange.lowerLpmPtr.nextBucket = nextPtr;
                            }

                            /* the new range prefix len is as of what's left of
                               the prefix length */
                            newPrefixLength = currPrefix;
                        }
                        /* Create new ranges if needed, as a result of inserting    */
                        /* the new range on a pre-existing range.                   */
                        retVal = splitRange(pRange,startAddr,endAddr,
                                            newPrefixLength,&newRange,
                                            PRV_CPSS_DXCH_LPM_RAM_MAX_LEVEL_LENGTH_CNS,updateOldPtr,
                                            &numOfNewRanges,
                                            &(currBucketPtrType[1]));
                        if (retVal != GT_OK)
                        {
                            if (currPrefix > PRV_CPSS_DXCH_LPM_RAM_MAX_LEVEL_LENGTH_CNS)
                            {
                                /* delete new bucket that was created just before fail */
                                prvCpssDxChLpmRamMngBucketDelete(newRange.lowerLpmPtr.nextBucket,
                                                                 PRV_CPSS_DXCH_LPM_RAM_MAX_LEVEL_LENGTH_CNS, NULL);
                            }
                            else
                            {
                                /* create mask for current range */
                                (*pRange)->mask = (GT_U8)PREFIX_2_MASK_MAC(currPrefix,PRV_CPSS_DXCH_LPM_RAM_MAX_LEVEL_LENGTH_CNS);
                            }
                            for (i=1; i <= level; i++)
                            {
                                if (*(pointerTypePtrArray[i]) == CPSS_DXCH_LPM_ROUTE_ENTRY_NEXT_PTR_TYPE_E)
                                {
                                    *(pointerTypePtrArray[i]) = CPSS_DXCH_LPM_COMPRESSED_1_NODE_PTR_TYPE_E;
                                }
                            }

                            if (shadowPtr->neededMemoryListLen > 0)
                            {
                                for (i = shadowPtr->neededMemoryCurIdx; i < shadowPtr->neededMemoryListLen; i++)
                                {
                                    prvCpssDmmFree(shadowPtr->neededMemoryBlocks[i]);
                                }
                            }
                            return retVal;
                        }


                        if (numOfNewRanges == 0)
                        {
                            /* An overwrite has been done, need to update the
                               HW.*/
                            *doHwUpdatePtr = GT_TRUE;
                        }
                        else
                        {
                            /* a change happend to the structure of the bucket
                               clear the cash*/
                            (*currBucketPtr)->rangeCash = NULL;

                            /* indicate that this bucket needs a resize*/
                            *resizeBucketPtr = GT_TRUE;

                            /* Add to the amount of ranges */
                            (*currBucketPtr)->numOfRanges = (GT_U16)((*currBucketPtr)->numOfRanges + numOfNewRanges);

                            /* and update the pointing range it needs to be
                               rewritten (if indeed this is the level after
                               that recorded pointing range) */
                            if ((parentRangeToUpdateDueToLowerLevelResizePtr != NULL) &&
                                ((parentRangeLevel +1) == level))
                            {
                                parentRangeToUpdateDueToLowerLevelResizePtr->updateRangeInHw =
                                    GT_TRUE;
                                /* indicate to the upper level that it needs
                                   a HW update*/
                                *(doHwUpdatePtr - 1) = GT_TRUE;
                            }
                        }
                    }
                    else
                    {
                        /* Insert the addr. to the existing next bucket. */
                        currBucketPtr[1] = (*pRange)->lowerLpmPtr.nextBucket;
                        currBucketPtrType[1] = &((*pRange)->pointerType);

                        /* indicate we need to proceed down the bucket trie*/
                        goDown = GT_TRUE;

                        if (currPrefix > PRV_CPSS_DXCH_LPM_RAM_MAX_LEVEL_LENGTH_CNS)
                        {
                            /* in the case the prefix ends at lower level it
                               may cause the a resize in that lower level which
                               will cause this pointed bucket to change
                               location. so we record the range so the lower
                               level could indicate to this level that in needs
                               to rewrite only pointer and not the whole
                               bucket.
                               Note! if the prefix ends in this level
                               or it ended in higher level, there could be a
                               resize in lower leves! */
                            parentRangeToUpdateDueToLowerLevelResizePtr = *pRange;
                            parentRangeLevel = level;
                        }

                        (*pRange)->mask |=
                            PREFIX_2_MASK_MAC(currPrefix,PRV_CPSS_DXCH_LPM_RAM_MAX_LEVEL_LENGTH_CNS);
                    }
                }
                else
                {
                    (*pRange)->mask |=
                        PREFIX_2_MASK_MAC(currPrefix,PRV_CPSS_DXCH_LPM_RAM_MAX_LEVEL_LENGTH_CNS);
                }

                *pRange = (*pRange)->next;

            }while (((*pRange) != NULL) && (endAddr >= (*pRange)->startAddr) &&
                    (goDown == GT_FALSE));

            /* check if we finished with the ranges we need to go over */
            if (((*pRange) != NULL) && (endAddr < (*pRange)->startAddr))
                (*pRange) = NULL;
        }

        /* check which direction we go in the tree */
        if (goDown == GT_TRUE)
        {
            /* we're going down. so advance in the pointer arrays */
            currBucketPtr++;
            currBucketPtrType++;
            doHwUpdatePtr++;
            resizeBucketPtr++;
            pRange++;

            /* and advance in the prefix information */
            currAddrByte++;
            currLpmEngineMemPtr++;
            level++;
            currPrefix = BUCKET_PREFIX_AT_LEVEL_MAC(prefix,PRV_CPSS_DXCH_LPM_RAM_MAX_LEVEL_LENGTH_CNS,
                                                  level);
        }
        else
        {
            /* we're going up, meaning we finished with the current level.
               write it to the HW and move back the pointers in the arrays*/

            /* if the bucket need resizing always update the HW */
            if (*resizeBucketPtr == GT_TRUE)
                *doHwUpdatePtr = GT_TRUE;

            if ((currBucketPtr == bucketPtrArry) && (isMcSrcTree == GT_FALSE))
            {
                isDestTreeRootBucket = GT_TRUE;
            }
            else
            {
                isDestTreeRootBucket = GT_FALSE;
            }

            /* during hot sync we shouldn't touch the Hw or the allocation
               it will be done in the end  */
            if (isDestTreeRootBucket &&
                (rootBucketFlag == PRV_CPSS_DXCH_LPM_RAM_ROOT_BUCKET_WITHOUT_RAM_UPDATE_E))
            {
                bucketUpdateMode = LPM_BUCKET_UPDATE_ROOT_BUCKET_SHADOW_ONLY_E;
                indicateSiblingUpdate =
                    (insertMode == PRV_CPSS_DXCH_LPM_RAM_TRIE_INSERT_SDW_MEM_MODE_E)?
                    GT_TRUE : GT_FALSE;
            }
            else switch (insertMode)
            {
                case PRV_CPSS_DXCH_LPM_RAM_TRIE_INSERT_SDW_ONLY_MODE_E:
                    bucketUpdateMode = LPM_BUCKET_UPDATE_SHADOW_ONLY_E;
                    break;
                case PRV_CPSS_DXCH_LPM_RAM_TRIE_INSERT_SDW_MEM_MODE_E:
                    if (*doHwUpdatePtr == GT_TRUE)
                    {
                        bucketUpdateMode = LPM_BUCKET_UPDATE_MEM_ONLY_E;
                    }
                    else
                    {
                        bucketUpdateMode = LPM_BUCKET_UPDATE_NONE_E;
                        indicateSiblingUpdate = GT_FALSE;
                    }

                    break;
                case PRV_CPSS_DXCH_LPM_RAM_TRIE_INSERT_SDW_MEM_HW_MODE_E:
                    bucketUpdateMode = (*doHwUpdatePtr == GT_TRUE) ?
                                       LPM_BUCKET_UPDATE_HW_AND_MEM_ALLOC_E:
                                       LPM_BUCKET_UPDATE_NONE_E;
                    break;
                 default:
                     for (i=1; i <= level; i++)
                     {
                         if (*(pointerTypePtrArray[i]) == CPSS_DXCH_LPM_ROUTE_ENTRY_NEXT_PTR_TYPE_E)
                         {
                             *(pointerTypePtrArray[i]) = CPSS_DXCH_LPM_COMPRESSED_1_NODE_PTR_TYPE_E;
                         }
                     }

                     if (shadowPtr->neededMemoryListLen > 0)
                     {
                         for (i = shadowPtr->neededMemoryCurIdx; i < shadowPtr->neededMemoryListLen; i++)
                         {
                             prvCpssDmmFree(shadowPtr->neededMemoryBlocks[i]);
                         }
                     }
                    CPSS_LOG_ERROR_AND_RETURN_MAC(GT_FAIL, LOG_ERROR_NO_MSG);
            }

            useCompressed2 =
                (isDestTreeRootBucket &&
                 (rootBucketFlag == PRV_CPSS_DXCH_LPM_RAM_ROOT_BUCKET_WITHOUT_COMPRESSED_2_E))?
                GT_FALSE : GT_TRUE;

            /* save rollback information */
            rollBackHwBucketOffsetHandle = (*currBucketPtr)->hwBucketOffsetHandle;
            rollBackBucketUpdateMode = bucketUpdateMode;
            rollBackResizeBucket = *resizeBucketPtr;

            /* The the shadow structures to the RAM.    */
            retVal = updateMirrorBucket(*currBucketPtr,bucketUpdateMode,
                                        isDestTreeRootBucket,*resizeBucketPtr,GT_FALSE,
                                        indicateSiblingUpdate,
                                        useCompressed2,currLpmEngineMemPtr,
                                        *currBucketPtrType,shadowPtr,NULL);
            if (retVal != GT_OK)
            {
                (*currBucketPtr)->bucketHwUpdateStat = PRV_CPSS_DXCH_LPM_RAM_BUCKET_HW_OK_E;
                for (i=1; i <= level; i++)
                {
                    if (*(pointerTypePtrArray[i]) == CPSS_DXCH_LPM_ROUTE_ENTRY_NEXT_PTR_TYPE_E)
                    {
                        *(pointerTypePtrArray[i]) = CPSS_DXCH_LPM_COMPRESSED_1_NODE_PTR_TYPE_E;
                    }
                }

                if((rollBackBucketUpdateMode != LPM_BUCKET_UPDATE_SHADOW_ONLY_E) &&
                   ((rollBackResizeBucket == GT_TRUE) || (rollBackHwBucketOffsetHandle == 0)))
                {
                    /* hw update was not successful */
                    if (rollBackHwBucketOffsetHandle == (*currBucketPtr)->hwBucketOffsetHandle)
                    {
                        /* remove memory allocation that was done for this bucket on */
                        /* PP memory estimation stage */
                        if (shadowPtr->neededMemoryListLen > 0)
                        {
                            if (shadowPtr->neededMemoryCurIdx > 0)
                            {
                                for (i = shadowPtr->neededMemoryCurIdx -1; i < shadowPtr->neededMemoryListLen; i++)
                                {
                                    prvCpssDmmFree(shadowPtr->neededMemoryBlocks[i]);
                                }
                            }
                        }
                    }
                }
                else
                    if(rollBackResizeBucket == GT_FALSE)
                    {
                        /* it was not successfull update of existing bucket */
                        /* free all preallocated memory memory */
                        if (shadowPtr->neededMemoryListLen > 0)
                        {
                            for (i = shadowPtr->neededMemoryCurIdx; i < shadowPtr->neededMemoryListLen; i++)
                            {
                                prvCpssDmmFree(shadowPtr->neededMemoryBlocks[i]);
                            }
                        }
                    }
                return retVal;
            }

            /* move back in the arrays*/
            currBucketPtr--;
            currBucketPtrType--;
            doHwUpdatePtr--;
            resizeBucketPtr--;

            /* before we go up , indicate this level is finished, and ready
               for a fresh level if needed in the future */
            *pRange = (PRV_CPSS_DXCH_LPM_RAM_RANGE_SHADOW_STC *)(~((GT_UINTPTR)0));
            pRange--;

            /* and move back in the prefix information */
            currAddrByte--;
            currLpmEngineMemPtr--;
            level--;
            currPrefix = BUCKET_PREFIX_AT_LEVEL_MAC(prefix,PRV_CPSS_DXCH_LPM_RAM_MAX_LEVEL_LENGTH_CNS,
                                                    level);
        }
    }

    return GT_OK;
}

/*******************************************************************************
* lpmGetSplitMethod
*
* DESCRIPTION:
*     Return the split method of a given range in the lpm tree.
*
* INPUTS:
*       numOfBucketRangesPtr  - Number of bucket ranges before the split.
*       rangePtr              - Pointer to the range to be splitted.
*       startAddr             - The low address of the new range.
*       endAddr               - The high address of the new range.
*       prefixLength          - The length of the dominant prefix of the new range
*       levelPrefix           - The current lpm level prefix.
*
* OUTPUTS:
*       numOfBucketRangesPtr  - Number of bucket ranges after the split.
*       splitMethodPtr        - The mode according to which the range should be split.
*
* RETURNS:
*     GT_OK     - on success.
*
* COMMENTS:
*     None.
*
*******************************************************************************/
static GT_STATUS lpmGetSplitMethod
(
    INOUT   GT_U32                                 *numOfBucketRangesPtr,
    IN      PRV_CPSS_DXCH_LPM_RAM_RANGE_SHADOW_STC *rangePtr,
    IN      GT_U8                                  startAddr,
    IN      GT_U8                                  endAddr,
    IN      GT_U8                                  levelPrefix,
    OUT     PRV_CPSS_DXCH_LPM_RAM_SPLIT_METHOD_ENT *splitMethodPtr
)
{
    GT_U8  endRange;           /* Index by which this range ends.      */

    endRange = (GT_U8)(LAST_RANGE_MAC(rangePtr) ? ((1 << levelPrefix) - 1) :
        ((rangePtr->next->startAddr) - 1));

    *splitMethodPtr = (((startAddr > rangePtr->startAddr) ? 1 : 0) +
                    ((endAddr < endRange) ? 2 : 0));

    switch (*splitMethodPtr)
    {
        case (PRV_CPSS_DXCH_LPM_RAM_SPLIT_HIGH_SPLIT_E):
        case (PRV_CPSS_DXCH_LPM_RAM_SPLIT_LOW_SPLIT_E):
            *numOfBucketRangesPtr += 1;
            break;

        case (PRV_CPSS_DXCH_LPM_RAM_SPLIT_MID_SPLIT_E):
            *numOfBucketRangesPtr += 2;
            break;

        case (PRV_CPSS_DXCH_LPM_RAM_SPLIT_OVERWRITE_E):
            *numOfBucketRangesPtr += 0;
            break;
    }

    return GT_OK;
}

/*******************************************************************************
* lpmCalcNeededMemory
*
* DESCRIPTION:
*       Calc the ammount of needed Pp memory for unicast address insertion.
*
* INPUTS:
*       bucketPtr         - Pointer to the root bucket.
*       addrCurBytePtr    - The Byte Array represnting the Address associated
*                           with the entry.
*       prefix            - The address prefix.
*       rootBucketFlag    - Indicates the way to deal with a root bucket (if it is).
*       lpmEngineMemPtrPtr- points to a an array of PRV_CPSS_DXCH_LPM_RAM_MEM_INFO_STC
*                           which holds all the memory information needed for where and
*                           how to allocate search memory for each of the lpm levels
*       neededMemList     - List holding the memory needed for inserting the given
*                           address.
*       memPoolList       - List of memory pools from which the blocks should be
*                           allocated from.
*       memListLenPtr     - Nuumber of memory blocks in neededMemList.
*
* OUTPUTS:
*       neededMemList     - List holding the memory needed for inserting the given
*                           address (in LPM lines)
*       memPoolList       - List of memory pools from which the blocks should be
*                           allocated from.
*       memListLenPtr     - Number of memory blocks in neededMemList.
*       memListOctetIndexList - List holding the Octet Index of each memory needed for 
*                           inserting the given address
*
* RETURNS:
*       GT_OK               - on success,
*       GT_FAIL             - Otherwise.
*
* COMMENTS:
*       - This function calculates the needed memory only in the tree layers
*         that are are needed to be written to HW (using rootBucketFlag)
*
*******************************************************************************/
static GT_STATUS lpmCalcNeededMemory
(
    IN    PRV_CPSS_DXCH_LPM_RAM_BUCKET_SHADOW_STC       *bucketPtr,
    IN    GT_U8                                         *addrCurBytePtr,
    IN    GT_U32                                        prefix,
    IN    PRV_CPSS_DXCH_LPM_RAM_ROOT_BUCKET_FLAG_ENT    rootBucketFlag,
    IN    PRV_CPSS_DXCH_LPM_RAM_MEM_INFO_STC            **lpmEngineMemPtrPtr,
    INOUT GT_U32                                        *neededMemList[],
    INOUT GT_UINTPTR                                    *memPoolList[],
    INOUT GT_U32                                        *memListLenPtr,
    INOUT GT_U32                                        *memListOctetIndexList[]
)
{
    GT_U8  startAddr;           /* The startAddr and endAddr of the     */
    GT_U8  endAddr;             /* given prefix in the current level.   */

    GT_U32 rangePrefixLength;   /* The prefix length represented by the */
                                /* current range's mask field.          */
    GT_STATUS   retVal = GT_OK; /* functions returned values.           */

    PRV_CPSS_DXCH_LPM_RAM_RANGE_SHADOW_STC   *pRange;         /* The first range that overlaps with   */
                                /* the given address * prefix.               */
    PRV_CPSS_DXCH_LPM_RAM_SPLIT_METHOD_ENT    splitMethod;  /* The method according to which the    */
                                                            /* range should be split.               */
    GT_U32          numOfBucketRanges;  /* Number of ranges in the      */
                                /* current's level's bucket.            */
    GT_U32          currNumOfBucketRanges;  /* Number of ranges in the      */
                                /* current's level's bucket.            */
    PRV_CPSS_DXCH_LPM_RAM_BUCKET_SHADOW_STC *currBucketPtr = bucketPtr;
    GT_U8 *currAddrByte = addrCurBytePtr;
    GT_U32 currPrefix = prefix;
    PRV_CPSS_DXCH_LPM_RAM_MEM_INFO_STC **currLpmEngineMemPtr = lpmEngineMemPtrPtr;
    GT_U32 octetIndex=0;


    while(currPrefix > 0)
    {
        calcStartEndAddr(*currAddrByte,currPrefix,&startAddr,&endAddr);
        if (currBucketPtr == NULL)
        {
            /* This is a new tree level, a 1-line bucket should be created, */
            /* including 2 or 3 lines.                                      */
            if ((startAddr == 0) || (endAddr == 0xFF))
                **neededMemList = 3;
            else
                **neededMemList = 4;

            **memPoolList = currLpmEngineMemPtr[0]->structsMemPool;
            (*memListLenPtr)++;
            **memListOctetIndexList=octetIndex;
            (*memPoolList)--;
            (*neededMemList)--;
            (*memListOctetIndexList)--;

            /* next level doesn't exists as well. so set it to null        */
            currBucketPtr = NULL;
        }
        else
        {
            pRange = find1stOverlap(currBucketPtr,startAddr,NULL);

            numOfBucketRanges = currNumOfBucketRanges = currBucketPtr->numOfRanges;

            /* Go over all levels until the first level to be written to HW.    */
            if (rootBucketFlag == PRV_CPSS_DXCH_LPM_RAM_ROOT_BUCKET_WITHOUT_RAM_UPDATE_E)
            {
                /* Calculate the needed memory of the next level bucket.        */
                if ((pRange->pointerType == CPSS_DXCH_LPM_ROUTE_ENTRY_NEXT_PTR_TYPE_E) ||
                    (pRange->pointerType == CPSS_DXCH_LPM_ECMP_ENTRY_PTR_TYPE_E) ||
                    (pRange->pointerType == CPSS_DXCH_LPM_QOS_ENTRY_PTR_TYPE_E) ||
                    ((GT_U32)(pRange->pointerType) == (GT_U32)PRV_CPSS_DXCH_LPM_RAM_TRIE_PTR_TYPE_E))
                {
                    currBucketPtr = NULL;
                }
                else
                {
                    currBucketPtr = pRange->lowerLpmPtr.nextBucket;
                }
            }
            else do
            {
                rangePrefixLength = mask2PrefixLength(pRange->mask,PRV_CPSS_DXCH_LPM_RAM_MAX_LEVEL_LENGTH_CNS);
                if (currPrefix >= rangePrefixLength)
                {
                    /* the inserted prefix covers all or part of this range */
                    if ((pRange->pointerType == CPSS_DXCH_LPM_ROUTE_ENTRY_NEXT_PTR_TYPE_E) ||
                        (pRange->pointerType == CPSS_DXCH_LPM_ECMP_ENTRY_PTR_TYPE_E) ||
                        (pRange->pointerType == CPSS_DXCH_LPM_QOS_ENTRY_PTR_TYPE_E) ||
                        ((GT_U32)(pRange->pointerType) == (GT_U32)PRV_CPSS_DXCH_LPM_RAM_TRIE_PTR_TYPE_E))
                    {
                        /* the range points to an end of tree (next-hop or
                           src bucket */

                        lpmGetSplitMethod(&numOfBucketRanges,pRange,startAddr,
                                          endAddr,PRV_CPSS_DXCH_LPM_RAM_MAX_LEVEL_LENGTH_CNS,
                                          &splitMethod);

                        if (currPrefix > PRV_CPSS_DXCH_LPM_RAM_MAX_LEVEL_LENGTH_CNS)
                        {
                            /* the prefix needs to be inserted deeper thus in
                            this case we Need to create new tree levels, all of
                            them of type "1-line bucket" including 2 or 3 lines.*/

                            /* (will reach here only once per do-while loop) */
                            currBucketPtr = NULL;
                        }
                    }
                    else
                    {
                        /* the next pointer points to a bucket  */
                        /* (will reach here only once per do-while loop, in
                            the case we need to go deeper. in other words: if
                            the insertion will cause lower levels to be
                            resized - this is the lower level bucket. if we
                            are on the last level to be resized then it can
                            reach this line several times, but it will have no
                            effect) */
                        currBucketPtr = pRange->lowerLpmPtr.nextBucket;
                    }
                }

                pRange = pRange->next;
            }while ((pRange != NULL) && (endAddr >= pRange->startAddr));

            /* check if a resize is needed, if not return */
            if (numOfBucketRanges != currNumOfBucketRanges)
            {
                if (numOfBucketRanges <= MAX_NUMBER_OF_COMPRESSED_1_RANGES_CNS)
                {
                    /* One line bucket: One word for the bit-vector +
                       next pointers. */
                    **neededMemList  = PRV_CPSS_DXCH_LPM_RAM_BUCKET_BIT_VEC_SIZE_COMPRESSED_1_CNS + numOfBucketRanges;
                }
                else if ((numOfBucketRanges <= MAX_NUMBER_OF_COMPRESSED_2_RANGES_CNS) &&
                         (rootBucketFlag != PRV_CPSS_DXCH_LPM_RAM_ROOT_BUCKET_WITHOUT_COMPRESSED_2_E))
                {
                    /* Two lines bucket: 2 words for the bit-vector +
                       next pointers. */
                    **neededMemList  = PRV_CPSS_DXCH_LPM_RAM_BUCKET_BIT_VEC_SIZE_COMPRESSED_2_CNS + numOfBucketRanges;
                }
                else
                {
                    /* Regular bucket: 11 words for the bit-vector +
                       next pointers.*/
                    **neededMemList  = PRV_CPSS_DXCH_LPM_RAM_BUCKET_BIT_VEC_SIZE_REGULAR_CNS + numOfBucketRanges;
                }

                **memPoolList = currLpmEngineMemPtr[0]->structsMemPool;
                (*memListLenPtr)++;
                (*memPoolList)--;
                (*neededMemList)--;
                **memListOctetIndexList=octetIndex;
                (*memListOctetIndexList)--;
            }
        }

        currAddrByte++;
        currPrefix = NEXT_BUCKET_PREFIX_MAC(currPrefix,PRV_CPSS_DXCH_LPM_RAM_MAX_LEVEL_LENGTH_CNS);
        rootBucketFlag = PRV_CPSS_DXCH_LPM_RAM_NOT_A_ROOT_BUCKET_E;
        currLpmEngineMemPtr++;
        octetIndex++;
    }

    return retVal;
}

/*******************************************************************************
* mergeCheck
*
* DESCRIPTION:
*       Returns 1 if the two input ranges can be merged to one range.
*
* INPUTS:
*       leftRangePtr      - The first range to check for merging.
*       rightRangePtr     - The second range to check for merging.
*       levelPrefix       - The current lpm level prefix.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       1 if the two input ranges can be merged to one range.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_U8 mergeCheck
(
    IN PRV_CPSS_DXCH_LPM_RAM_RANGE_SHADOW_STC   *leftRangePtr,
    IN PRV_CPSS_DXCH_LPM_RAM_RANGE_SHADOW_STC   *rightRangePtr,
    IN GT_U8                                    levelPrefix
)
{
    GT_U32  rightStartAddr;      /* The start address of the right range */
                                 /* after taking away the unneeded LSBs  */
                                 /* according to the range's mask.       */
    GT_U32  rightPrefix;         /* Prefix length of the right range.    */
    GT_U32  leftPrefix;          /* Prefix length of the left range.     */

    if ((leftRangePtr == NULL) || (rightRangePtr == NULL))
        return 0;

    if (((leftRangePtr->pointerType != CPSS_DXCH_LPM_ROUTE_ENTRY_NEXT_PTR_TYPE_E) &&
         (leftRangePtr->pointerType != CPSS_DXCH_LPM_ECMP_ENTRY_PTR_TYPE_E) &&
         (leftRangePtr->pointerType != CPSS_DXCH_LPM_QOS_ENTRY_PTR_TYPE_E) &&
         ((GT_U32)(leftRangePtr->pointerType) != (GT_U32)PRV_CPSS_DXCH_LPM_RAM_TRIE_PTR_TYPE_E)) ||
        ((rightRangePtr->pointerType != CPSS_DXCH_LPM_ROUTE_ENTRY_NEXT_PTR_TYPE_E) &&
         (rightRangePtr->pointerType != CPSS_DXCH_LPM_ECMP_ENTRY_PTR_TYPE_E) &&
         (rightRangePtr->pointerType != CPSS_DXCH_LPM_QOS_ENTRY_PTR_TYPE_E) &&
         ((GT_U32)(rightRangePtr->pointerType) != (GT_U32)PRV_CPSS_DXCH_LPM_RAM_TRIE_PTR_TYPE_E)))
        return 0;

    rightPrefix = mask2PrefixLength(rightRangePtr->mask,levelPrefix);
    leftPrefix  = mask2PrefixLength(leftRangePtr->mask,levelPrefix);

    rightStartAddr = (rightRangePtr->startAddr & (0xFFFFFFFF <<
                                               (levelPrefix - rightPrefix)));

    if((leftRangePtr->startAddr >= rightStartAddr) && (leftPrefix == rightPrefix))
        return 1;

    return 0;
}

/*******************************************************************************
* mergeRange
*
* DESCRIPTION:
*       This function merges a range. According to one of the following possible
*       merges:
*       PRV_CPSS_DXCH_LPM_RAM_MERGE_OVERWRITE_E, PRV_CPSS_DXCH_LPM_RAM_MERGE_LOW_E,
*       PRV_CPSS_DXCH_LPM_RAM_MERGE_HIGH_E and PRV_CPSS_DXCH_LPM_RAM_MERGE_MID_E
*
* INPUTS:
*       rangePtr          - Pointer to the range to be merged.
*       startAddr         - The low address of the range to be merged.
*       prefixLength      - The length of the dominant prefix of the range.
*       prevRangePtr      - A pointer to the lower address neighbor of the range to be
*                           merged. NULL if rangePtr is the 1st in the list.
*       levelPrefix       - The current lpm level prefix.
*
* OUTPUTS:
*       rangePtr          - A pointer to the next node in the list.
*       numOfNewRangesPtr - Number of new created ranges as a result of the split.
*
* RETURNS:
*       GT_OK on success, GT_FAIL otherwise.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS mergeRange
(
    INOUT PRV_CPSS_DXCH_LPM_RAM_RANGE_SHADOW_STC    **rangePtr,
    IN    GT_U32                                    startAddr,
    IN    GT_U32                                    prefixLength,
    IN    PRV_CPSS_DXCH_LPM_RAM_RANGE_SHADOW_STC    *prevRangePtr,
    IN    GT_U8                                     levelPrefix,
    OUT   GT_32                                     *numOfNewRangesPtr
)
{
    PRV_CPSS_DXCH_LPM_RAM_RANGE_SHADOW_STC *curRangePtr = (*rangePtr);/* Temp vars*/

    PRV_CPSS_DXCH_LPM_RAM_MERGE_METHOD_ENT mergeMethod;     /* The method by which too perform the  */
                                                            /* ranges merge.                        */
    PRV_CPSS_DXCH_LPM_RAM_RANGE_SHADOW_STC *tmpPtr; /* Temporary range pointer to be used   */
                                /* in range operation.                  */

    /* warnings fix */
    startAddr = startAddr;
    prefixLength = prefixLength;

    mergeMethod = ((mergeCheck(curRangePtr,curRangePtr->next,
                               levelPrefix) ? 1 : 0) +
                   (mergeCheck(prevRangePtr,curRangePtr,levelPrefix) ? 2 : 0));

    switch (mergeMethod)
    {
    case PRV_CPSS_DXCH_LPM_RAM_MERGE_OVERWRITE_E: /* The old range fell       */
                                                  /* exactly on a older range */
        *numOfNewRangesPtr = 0;

        curRangePtr->updateRangeInHw = GT_TRUE;
        break;

    case PRV_CPSS_DXCH_LPM_RAM_MERGE_HIGH_E:  /* Merge the upper part of the range */
        curRangePtr->mask = curRangePtr->next->mask;

        curRangePtr->lowerLpmPtr = curRangePtr->next->lowerLpmPtr;
        curRangePtr->pointerType = curRangePtr->next->pointerType;

        tmpPtr = curRangePtr->next;
        curRangePtr->next = curRangePtr->next->next;
        (*rangePtr) = curRangePtr;
        cpssOsLpmFree(tmpPtr);
        tmpPtr = NULL;
        *numOfNewRangesPtr = -1;

        curRangePtr->updateRangeInHw = GT_TRUE;

        break;

    case PRV_CPSS_DXCH_LPM_RAM_MERGE_LOW_E: /* Merge the lower part of the range */

        prevRangePtr->next = curRangePtr->next;

        cpssOsLpmFree(curRangePtr);
        curRangePtr = NULL;
        (*rangePtr) = prevRangePtr;
        *numOfNewRangesPtr = -1;

        prevRangePtr->updateRangeInHw = GT_TRUE;
        break;

    case PRV_CPSS_DXCH_LPM_RAM_MERGE_MID_E: /* Merge the ranges from both */
                                            /* sides of the range.        */

        prevRangePtr->next = curRangePtr->next->next;

        cpssOsLpmFree(curRangePtr->next);
        curRangePtr->next = NULL;

        cpssOsLpmFree(curRangePtr);
        curRangePtr = NULL;

        (*rangePtr) = prevRangePtr;
        *numOfNewRangesPtr = -2;

        prevRangePtr->updateRangeInHw = GT_TRUE;
        break;

    default:
        break;
    }

    /* the range has expanded or an overwrite was done, need to update in HW */

    return GT_OK;
}

/*******************************************************************************
* getFromTrie
*
* DESCRIPTION:
*       Returns a next hop entry from the trie structure.
*
* INPUTS:
*       bucketPtr    - Bucket to which the given entry is associated.
*       startAddr    - Start Addr of the address to look for in the trie.
*       prefix       - The address prefix.
*       levelPrefix  - The current lpm level prefix.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       nextHopPtr  - A pointer to the next hop entry, if found, NULL otherwise.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_PTR getFromTrie
(
    IN PRV_CPSS_DXCH_LPM_RAM_BUCKET_SHADOW_STC    *bucketPtr,
    IN GT_U8                                      startAddr,
    IN GT_U32                                     prefix,
    IN GT_U8                                      levelPrefix
)
{
    GT_PTR nextPointer = NULL; /* Trie search result.              */
    GT_U8 addr[1];             /* startAddr representation as      */
                               /* GT_U8.                           */

    addr[0] = (GT_U8)(startAddr & (BIT_MASK_MAC(levelPrefix) << (levelPrefix - prefix)));

    if(prvCpssDxChLpmTrieSearch(&(bucketPtr->trieRoot),addr,prefix,levelPrefix,
                                &nextPointer)== NULL)
        return NULL;

    return (nextPointer);
}

/*******************************************************************************
* delFromTrie
*
* DESCRIPTION:
*       This function deletes an entry from the trie structure.
*
* INPUTS:
*       bucketPtr   - A pointer to the bucket to which the deleted address is
*                    is associated.
*       startAddr   - The start address associated with the address to be
*                     deleted.
*       prefix      - The address prefix.
*       trieDepth   - The maximum depth of the trie.
*
* OUTPUTS:
*       delEntryPtr - A pointer to the next hop entry stored in the trie.
*
* RETURNS:
*       GT_OK on success, or GT_FAIL if the given (address,prefix) are not found.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS delFromTrie
(
    IN  PRV_CPSS_DXCH_LPM_RAM_BUCKET_SHADOW_STC     *bucketPtr,
    IN  GT_U8                                       startAddr,
    IN  GT_U32                                      prefix,
    IN  GT_U8                                       trieDepth,
    OUT GT_PTR                                      *delEntryPtr
)
{
    GT_PTR nextPointer;      /* Trie deleted node data pointer.          */
    GT_U8 addr[1];           /* startAddr & prefix representation        */

    addr[0] = (GT_U8)(startAddr & (BIT_MASK_MAC(trieDepth) << (trieDepth - prefix)));

    prvCpssDxChLpmTrieDel(&(bucketPtr->trieRoot),addr,prefix,trieDepth,
                          &nextPointer);

    if(nextPointer == NULL)
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_FAIL, LOG_ERROR_NO_MSG);
    else
    {
        *delEntryPtr = nextPointer;
        return GT_OK;
    }
}

/*******************************************************************************
* prvCpssDxChLpmRamMngBucketDelete
*
* DESCRIPTION:
*       This function deletes an empty bucket structure from memory.
*
* INPUTS:
*       bucketPtr   - A pointer to the bucket to be deleted.
*       levelPrefix - The current lpm level prefix.
*
* OUTPUTS:
*       pNextPtr    - A pointer to the data stored in the Trie.
*
* RETURNS:
*       GT_OK
*       GT_BAD_STATE - if the bucket is not empty (has more then 1 range)
*
* COMMENTS:
*       This function is called only for non-root buckets, which are fully empty
*
*******************************************************************************/
GT_STATUS prvCpssDxChLpmRamMngBucketDelete
(
    IN  PRV_CPSS_DXCH_LPM_RAM_BUCKET_SHADOW_STC *bucketPtr,
    IN  GT_U8                                   levelPrefix,
    OUT GT_PTR                                  *pNextPtr
)
{
    GT_PTR delEntry = 0;         /* Data stored in the Trie   */

    if (bucketPtr->numOfRanges != 1)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_STATE, LOG_ERROR_NO_MSG);
    }

    delFromTrie(bucketPtr,0,0,levelPrefix,&delEntry);

    if(pNextPtr != NULL)
        *pNextPtr = delEntry;

    cpssOsLpmFree(bucketPtr->rangeList);
    bucketPtr->rangeList = NULL;

    cpssOsLpmFree(bucketPtr);
    bucketPtr = NULL;

    return GT_OK;
}

/*******************************************************************************
* prvCpssDxChLpmRamMngEntryDelete
*
* DESCRIPTION:
*       Delete an entry from the LPM tables.
*
* INPUTS:
*       lpmPtr            - Pointer to the root bucket.
*       addrCurBytePtr    - The Byte Array represnting the Address associated
*                           with the entry.
*       prefix            - The address prefix.
*       rootBucketFlag    - Indicates the way to deal with a root bucket (if it is)
*       updateHwAndMem    - whether an HW update and memory alloc should take place
*       lpmEngineMemPtrPtr- points to a an array of PRV_CPSS_DXCH_LPM_RAM_MEM_INFO_STC
*                           which holds all the memory information needed for where and
*                           how to allocate search memory for each of the lpm levels
*       bucketTypePtr     - The bucket type of the root bucket after deletion.
*                           Possible values:
*                           REGULAR_BUCKET, COMPRESSED_1, COMPRESSED_2, or NEXT_HOP
*       shadowPtr         - the shadow relevant for the devices asked to act on.
*       parentWriteFuncPtr- a bucket's parent write function in case there is
*                           a need to update the packet's parent ptr data and
*                           the parent is not a LPM trie range.
*       isMcSrcTree       - indicates whether bucketPtr is the toor bucket of a
*                           multicast source tree
*       rollBack          - GT_TRUE: rollback is taking place
*                           GT_FALSE: otherwise
*
* OUTPUTS:
*       lpmPtr            - Pointer to the root bucket.
*       bucketTypePtr     - The bucket type of the root bucket after deletion.
*                           Possible values:
*                           REGULAR_BUCKET, COMPRESSED_1, COMPRESSED_2, or NEXT_HOP
*       delEntryPtr       - The next_pointer structure of the entry associated with
*                           the deleted (address,prefix).
*
* RETURNS:
*       GT_OK on success, or
*       GT_NOT_FOUND             - If (address,prefix) not found.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS prvCpssDxChLpmRamMngEntryDelete
(
    INOUT PRV_CPSS_DXCH_LPM_RAM_POINTER_SHADOW_UNT                  *lpmPtr,
    IN    GT_U8                                                     *addrCurBytePtr,
    IN    GT_U32                                                    prefix,
    IN    PRV_CPSS_DXCH_LPM_RAM_ROOT_BUCKET_FLAG_ENT                rootBucketFlag,
    IN    GT_BOOL                                                   updateHwAndMem,
    IN    PRV_CPSS_DXCH_LPM_RAM_MEM_INFO_STC                        **lpmEngineMemPtrPtr,
    INOUT CPSS_DXCH_LPM_NEXT_POINTER_TYPE_ENT                       *bucketTypePtr,
    OUT   GT_PTR                                                    *delEntryPtr,
    IN    PRV_CPSS_DXCH_LPM_RAM_SHADOW_STC                          *shadowPtr,
    IN    PRV_CPSS_DXCH_LPM_RAM_BUCKET_PARENT_WRITE_FUNC_INFO_STC   *parentWriteFuncPtr,
    IN    GT_BOOL                                                   isMcSrcTree,
    IN    GT_BOOL                                                   rollBack
)
{
    GT_U8 startAddr;           /* The startAddr and endAddr of the     */
    GT_U8 endAddr;             /* given prefix in the current level.   */

    GT_U32 rangePrefixLength;   /* The prefix length represented by the */
                                /* current range's mask field.          */
    GT_U32 newPrefixLength;     /* The new prefix length that the       */
                                /* current range represents.            */

    GT_U8  newStartAddr;        /* The startAddr of the range that      */
                                /* should replace the deleted one.      */
    GT_BOOL resizeBucket;       /* Was the bucket resized as a  */
                                /* result of deletion.                  */
    GT_BOOL doHwUpdate;         /* is it needed to update the HW */
    PRV_CPSS_DXCH_LPM_ROUTE_ENTRY_POINTER_STC *newNextHopPtr;/* Next hop representing
                                                           the prefix that
                                                           replaces the deleted
                                                           one. */
    GT_32 numOfNewRanges;       /* Number of new ranges as a result of  */
                                /* range merging.                       */
    LPM_BUCKET_UPDATE_MODE_ENT bucketUpdateMode;/* indicates whether to write
                                                   the bucket to the device's
                                                   RAM.*/
    GT_BOOL useCompressed2; /* indicates wheather to use a compressed 2 type
                               when allocting this bucket memory */
    GT_BOOL indicateSiblingUpdate;
    GT_PTR  nextPtr;
    PRV_CPSS_DXCH_LPM_RAM_TRIE_INSERT_MODE_ENT  insertMode;

    GT_U32 level = 0;
    GT_BOOL updateOld,updateUpperLevel = GT_FALSE;
    GT_STATUS retVal = GT_OK;

    PRV_CPSS_DXCH_LPM_RAM_RANGE_SHADOW_STC **pPrevRange;/* The first range that overlaps with   */
    PRV_CPSS_DXCH_LPM_RAM_RANGE_SHADOW_STC **pRange;    /* address & prefix, and a pointer to  */
                                                        /* the range that comes before it.      */

    static PRV_CPSS_DXCH_LPM_RAM_POINTER_SHADOW_UNT       *lpmPtrArray[MAX_LPM_LEVELS_CNS];

    static PRV_CPSS_DXCH_LPM_RAM_RANGE_SHADOW_STC         *pRangeArray[MAX_LPM_LEVELS_CNS];
    static PRV_CPSS_DXCH_LPM_RAM_RANGE_SHADOW_STC         *pPrvRangeArray[MAX_LPM_LEVELS_CNS];
    static CPSS_DXCH_LPM_NEXT_POINTER_TYPE_ENT            *bucketPtrTypeArray[MAX_LPM_LEVELS_CNS];
    PRV_CPSS_DXCH_LPM_RAM_POINTER_SHADOW_UNT              **currLpmPtr;
    GT_U8                                                 *currAddrByte;
    GT_U32                                                currPrefix;
    CPSS_DXCH_LPM_NEXT_POINTER_TYPE_ENT                   **currBucketPtrType;
    GT_BOOL                                               isDestTreeRootBucket;
    CPSS_DXCH_LPM_NEXT_POINTER_TYPE_ENT                   newNextHopPtrType;
    currAddrByte = addrCurBytePtr;
    currPrefix = prefix;

    cpssOsMemSet(lpmPtrArray,0,sizeof(PRV_CPSS_DXCH_LPM_RAM_POINTER_SHADOW_UNT*)*MAX_LPM_LEVELS_CNS);
    cpssOsMemSet(pRangeArray,0,sizeof(PRV_CPSS_DXCH_LPM_RAM_RANGE_SHADOW_STC*)*MAX_LPM_LEVELS_CNS);
    cpssOsMemSet(pPrvRangeArray,0,sizeof(PRV_CPSS_DXCH_LPM_RAM_RANGE_SHADOW_STC*)*MAX_LPM_LEVELS_CNS);
    cpssOsMemSet(bucketPtrTypeArray,0,sizeof(GT_U8*)*MAX_LPM_LEVELS_CNS);

    bucketPtrTypeArray[0] = bucketTypePtr;
    lpmPtrArray[0] = lpmPtr;
    currLpmPtr = lpmPtrArray;
    pPrevRange = pPrvRangeArray;
    pRange = pRangeArray;
    currBucketPtrType = bucketPtrTypeArray;

    /* first run and record all the neccesary info down the bucket tree */
    for(;;)
    {
        if (*currLpmPtr == NULL)
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_FOUND, LOG_ERROR_NO_MSG); /* the prefix is not in the tree */

        calcStartEndAddr(*currAddrByte,currPrefix,&startAddr,&endAddr);

        *pRange = find1stOverlap((*currLpmPtr)->nextBucket,startAddr,pPrevRange);
        /* The deleted prefix ends in a deeper level,   */
        /* call the delete function recursively.        */
        if ((currPrefix > PRV_CPSS_DXCH_LPM_RAM_MAX_LEVEL_LENGTH_CNS) &&
            ((*pRange)->pointerType != CPSS_DXCH_LPM_ROUTE_ENTRY_NEXT_PTR_TYPE_E) &&
            ((*pRange)->pointerType != CPSS_DXCH_LPM_ECMP_ENTRY_PTR_TYPE_E) &&
            ((*pRange)->pointerType != CPSS_DXCH_LPM_QOS_ENTRY_PTR_TYPE_E) &&
            ((GT_U32)((*pRange)->pointerType) != (GT_U32)PRV_CPSS_DXCH_LPM_RAM_TRIE_PTR_TYPE_E))
        {
            currLpmPtr[1] = &((*pRange)->lowerLpmPtr);
            currBucketPtrType[1] = &((*pRange)->pointerType);
            currAddrByte++;
            lpmEngineMemPtrPtr++;

            currPrefix = NEXT_BUCKET_PREFIX_MAC(currPrefix,PRV_CPSS_DXCH_LPM_RAM_MAX_LEVEL_LENGTH_CNS);

            /* advance the array pointers */
            currLpmPtr++;
            pRange++;
            pPrevRange++;
            currBucketPtrType++;
            level++;
        }
        else
            break;
    }

    /* now were on the lowest level for this prefix, delete the from trie */
    retVal = delFromTrie((*currLpmPtr)->nextBucket,startAddr,currPrefix,
                         PRV_CPSS_DXCH_LPM_RAM_MAX_LEVEL_LENGTH_CNS,delEntryPtr);
    if (retVal != GT_OK)
    {
        if (rollBack == GT_FALSE)
        {
            return retVal;
        }
    }

    while (currLpmPtr >= lpmPtrArray)
    {
        /* reset the flags */
        resizeBucket          = GT_FALSE;
        doHwUpdate            = GT_FALSE;
        indicateSiblingUpdate = GT_FALSE;

        calcStartEndAddr(*currAddrByte,currPrefix,&startAddr,&endAddr);

        do
        {
            /* The deleted prefix ends in a deeper level,   */
            if (currPrefix > PRV_CPSS_DXCH_LPM_RAM_MAX_LEVEL_LENGTH_CNS)
            {
                /* check if the bucket was resized , thus moved in memory */
                if (updateUpperLevel == GT_TRUE)
                {
                    (*pRange)->updateRangeInHw = GT_TRUE;
                    doHwUpdate = GT_TRUE;

                    /* the upper level has been marked for update , remove
                       the signal */
                    updateUpperLevel = GT_FALSE;
                }

                /* Check the type of the next bucket after deletion */
                if (((*pRange)->pointerType == CPSS_DXCH_LPM_ROUTE_ENTRY_NEXT_PTR_TYPE_E) ||
                    ((*pRange)->pointerType == CPSS_DXCH_LPM_ECMP_ENTRY_PTR_TYPE_E) ||
                    ((*pRange)->pointerType == CPSS_DXCH_LPM_QOS_ENTRY_PTR_TYPE_E))
                {
                    /* if it's type "route entry pointer" it means there is no
                       need for that pointed bucket (and it actually been
                       deleted in the lower level) so merge the range */
                    mergeRange(pRange,startAddr,currPrefix,*pPrevRange,
                               PRV_CPSS_DXCH_LPM_RAM_MAX_LEVEL_LENGTH_CNS,&numOfNewRanges);

                    if (numOfNewRanges == 0)
                    {
                        /* An overwrite merge was done (no change in the amount
                           of ranges) extract the appropriate prefix from the
                           trie, and insert it in the same place.*/
                        rangePrefixLength =
                            mask2PrefixLength((*pRange)->mask,
                                              PRV_CPSS_DXCH_LPM_RAM_MAX_LEVEL_LENGTH_CNS);

                        nextPtr = getFromTrie((*currLpmPtr)->nextBucket,
                                              (*pRange)->startAddr,
                                              rangePrefixLength,
                                              PRV_CPSS_DXCH_LPM_RAM_MAX_LEVEL_LENGTH_CNS);

                        PRV_CPSS_DXCH_LPM_NEXT_PTR_TYPE_CONVERT_MAC(((PRV_CPSS_DXCH_LPM_ROUTE_ENTRY_POINTER_STC*)nextPtr)->routeEntryMethod,
                                                                    (*pRange)->pointerType);
                        (*pRange)->lowerLpmPtr.nextHopEntry = nextPtr;

                        doHwUpdate = GT_TRUE;
                    }
                    else
                    {
                        /* update the number of ranges */
                        (*currLpmPtr)->nextBucket->numOfRanges =
                            (GT_U16)((*currLpmPtr)->nextBucket->numOfRanges + numOfNewRanges);

                        /* indicate a resize has happen */
                        resizeBucket = GT_TRUE;

                        /* a change happend to the structure of the bucket
                        (removed ranges) clear the cash, and signal the upper
                        level */
                        (*currLpmPtr)->nextBucket->rangeCash = NULL;

                        updateUpperLevel = GT_TRUE;
                    }
                }
            }
            else
            {
                /* The deleted prefix ends in this level.   */
                rangePrefixLength =
                    mask2PrefixLength((*pRange)->mask, PRV_CPSS_DXCH_LPM_RAM_MAX_LEVEL_LENGTH_CNS);

                if (currPrefix > rangePrefixLength)
                    /* should never happen */
                    if (rollBack == GT_FALSE)
                        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_FAIL, LOG_ERROR_NO_MSG);

                (*pRange)->mask &= ~(PREFIX_2_MASK_MAC(currPrefix,PRV_CPSS_DXCH_LPM_RAM_MAX_LEVEL_LENGTH_CNS));

                if (currPrefix == rangePrefixLength)
                {
                    /* The current range represents the deleted prefix. */

                    if (((*pRange)->pointerType == CPSS_DXCH_LPM_ROUTE_ENTRY_NEXT_PTR_TYPE_E) ||
                        ((*pRange)->pointerType == CPSS_DXCH_LPM_ECMP_ENTRY_PTR_TYPE_E) ||
                        ((*pRange)->pointerType == CPSS_DXCH_LPM_QOS_ENTRY_PTR_TYPE_E) ||
                        ((GT_U32)((*pRange)->pointerType) == (GT_U32)PRV_CPSS_DXCH_LPM_RAM_TRIE_PTR_TYPE_E))
                    {
                        /* if the range's type "route entry pointer" it means
                           there is no need for this range, merge it */

                        mergeRange(pRange,startAddr,currPrefix,*pPrevRange,
                                   PRV_CPSS_DXCH_LPM_RAM_MAX_LEVEL_LENGTH_CNS,&numOfNewRanges);

                        if (numOfNewRanges == 0)
                        {
                           /* An overwrite merge was done (no change in the
                              amount of ranges) extract the appropriate prefix
                              from the trie, and insert it in the same
                              place.*/
                            rangePrefixLength =
                                mask2PrefixLength((*pRange)->mask,
                                                  PRV_CPSS_DXCH_LPM_RAM_MAX_LEVEL_LENGTH_CNS);
                            nextPtr =
                                getFromTrie((*currLpmPtr)->nextBucket,
                                            (*pRange)->startAddr,
                                            rangePrefixLength,
                                            PRV_CPSS_DXCH_LPM_RAM_MAX_LEVEL_LENGTH_CNS);

                            PRV_CPSS_DXCH_LPM_NEXT_PTR_TYPE_CONVERT_MAC(((PRV_CPSS_DXCH_LPM_ROUTE_ENTRY_POINTER_STC*)nextPtr)->routeEntryMethod,
                                                                        (*pRange)->pointerType);
                            (*pRange)->lowerLpmPtr.nextHopEntry = nextPtr;

                            doHwUpdate = GT_TRUE;
                        }
                        else
                        {

                            /* update the amount of ranges */
                            (*currLpmPtr)->nextBucket->numOfRanges =
                                (GT_U16)((*currLpmPtr)->nextBucket->numOfRanges + numOfNewRanges);

                            /* indicate a resize has happen */
                            resizeBucket = GT_TRUE;

                            /* a change happend to the structure of the bucket
                               (removed ranges) clear the cash, and signal the
                               upper level */
                            (*currLpmPtr)->nextBucket->rangeCash = NULL;

                            updateUpperLevel = GT_TRUE;
                        }
                    }
                    else
                    {
                        /* The current range was extended as a result of    */
                        /* previous insertions of larger prefixes.          */
                        newPrefixLength =
                            mask2PrefixLength((*pRange)->mask,
                                              PRV_CPSS_DXCH_LPM_RAM_MAX_LEVEL_LENGTH_CNS);
                        newStartAddr = (GT_U8)(startAddr &
                            (0xFF << (PRV_CPSS_DXCH_LPM_RAM_MAX_LEVEL_LENGTH_CNS - newPrefixLength)));

                        /* Get the prefix that should replace the   */
                        /* deleted prefix.                          */
                        newNextHopPtr = getFromTrie((*currLpmPtr)->nextBucket,
                                                    newStartAddr,
                                                    newPrefixLength,
                                                    PRV_CPSS_DXCH_LPM_RAM_MAX_LEVEL_LENGTH_CNS);

                        /* in case of no HW update it's the same as in hot sync,
                           shadow update only */
                        insertMode = (updateHwAndMem == GT_TRUE)?
                                     PRV_CPSS_DXCH_LPM_RAM_TRIE_INSERT_SDW_MEM_HW_MODE_E:
                                     PRV_CPSS_DXCH_LPM_RAM_TRIE_INSERT_SDW_ONLY_MODE_E;

                        /* Insert the replacing prefix to the next level. this
                           insertion cannot resize any lower level bucket , it
                           simply overwrites using the replacing prefix the
                           ranges the old prefix dominated. thus there is no
                           need to check if the lower levels have been resized
                           for a current range HW update */
                        updateOld = GT_FALSE;

                        PRV_CPSS_DXCH_LPM_NEXT_PTR_TYPE_CONVERT_MAC(newNextHopPtr->routeEntryMethod,newNextHopPtrType);
                        retVal =
                            prvCpssDxChLpmRamMngInsert((*pRange)->lowerLpmPtr.nextBucket,
                                                       currAddrByte + 1,/* actually not relevant*/
                                                       0,newNextHopPtr,
                                                       newNextHopPtrType,
                                                       PRV_CPSS_DXCH_LPM_RAM_NOT_A_ROOT_BUCKET_E,
                                                       lpmEngineMemPtrPtr + 1,
                                                       &updateOld,
                                                       &((*pRange)->pointerType),
                                                       shadowPtr,insertMode,
                                                       isMcSrcTree);
                        if (retVal != GT_OK)
                        {
                            return retVal;
                        }
                    }
                }
            }

            *pPrevRange = (*pRange);
            (*pRange) = (*pRange)->next;

        }while (((*pRange) != NULL) && (endAddr >= (*pRange)->startAddr));

        /* if the bucket need resizing always update the HW */
        if (resizeBucket == GT_TRUE)
            doHwUpdate = GT_TRUE;

        if ((currLpmPtr == lpmPtrArray) && (isMcSrcTree == GT_FALSE))
        {
            isDestTreeRootBucket = GT_TRUE;
        }
        else
        {
            isDestTreeRootBucket = GT_FALSE;
        }
        useCompressed2 = (isDestTreeRootBucket &&
                          (rootBucketFlag == PRV_CPSS_DXCH_LPM_RAM_ROOT_BUCKET_WITHOUT_COMPRESSED_2_E))?
                         GT_FALSE : GT_TRUE;

        if (isDestTreeRootBucket &&
            (rootBucketFlag == PRV_CPSS_DXCH_LPM_RAM_ROOT_BUCKET_WITHOUT_RAM_UPDATE_E))
        {
            bucketUpdateMode = LPM_BUCKET_UPDATE_ROOT_BUCKET_SHADOW_ONLY_E;
            indicateSiblingUpdate = (updateHwAndMem == GT_FALSE) ?
                                    GT_TRUE:GT_FALSE;
        }
        else if (doHwUpdate == GT_FALSE)
        {
            bucketUpdateMode = LPM_BUCKET_UPDATE_NONE_E;
            indicateSiblingUpdate =
            (updateHwAndMem == GT_FALSE)? GT_TRUE:GT_FALSE;
        }
        else if (updateHwAndMem == GT_FALSE)
        {
            bucketUpdateMode = LPM_BUCKET_UPDATE_SHADOW_ONLY_E;
        }
        else
        {
            bucketUpdateMode = LPM_BUCKET_UPDATE_HW_AND_MEM_ALLOC_E;
        }

        /* Write the prepared shadow bucket to the device's RAM. */
        retVal = updateMirrorBucket((*currLpmPtr)->nextBucket,bucketUpdateMode,
                                    isDestTreeRootBucket,resizeBucket,GT_FALSE,
                                    indicateSiblingUpdate,useCompressed2,
                                    lpmEngineMemPtrPtr,*currBucketPtrType,
                                    shadowPtr,parentWriteFuncPtr);
        if (retVal != GT_OK)
        {
            return retVal;
        }

        /* If this level is not a root bucket then delete it if its empty.*/
        if (((currLpmPtr != lpmPtrArray) ||
             (rootBucketFlag == PRV_CPSS_DXCH_LPM_RAM_NOT_A_ROOT_BUCKET_E)) &&
            ((**currBucketPtrType == CPSS_DXCH_LPM_ROUTE_ENTRY_NEXT_PTR_TYPE_E) ||
             (**currBucketPtrType == CPSS_DXCH_LPM_ECMP_ENTRY_PTR_TYPE_E) ||
             (**currBucketPtrType == CPSS_DXCH_LPM_QOS_ENTRY_PTR_TYPE_E)))
        {
            newNextHopPtr =
                (*currLpmPtr)->nextBucket->rangeList->lowerLpmPtr.nextHopEntry;
            retVal = prvCpssDxChLpmRamMngBucketDelete((*currLpmPtr)->nextBucket,
                                                      PRV_CPSS_DXCH_LPM_RAM_MAX_LEVEL_LENGTH_CNS, NULL);
            if (retVal != GT_OK)
            {
                return retVal;
            }
            (*currLpmPtr)->nextHopEntry = newNextHopPtr;
        }

        /* step back with the arrays */
        currLpmPtr--;
        pRange--;
        pPrevRange--;
        currBucketPtrType--;
        currAddrByte--;
        lpmEngineMemPtrPtr--;
        level--;
        currPrefix = BUCKET_PREFIX_AT_LEVEL_MAC(prefix,PRV_CPSS_DXCH_LPM_RAM_MAX_LEVEL_LENGTH_CNS,level);
    }

    return retVal;
}

/*******************************************************************************
* prvCpssDxChLpmRamMngSearch
*
* DESCRIPTION:
*       Search for the existence of a given (address,Prefix) in the given LPM
*       structure.
*
* INPUTS:
*       bucketPtr         - A pointer to the bucket to search in.
*       addrCurBytePtr    - The Byte Array represnting the Address to search for.
*       prefix            - address prefix length.
*
* OUTPUTS:
*       nextHopPtr        - A pointer to the found next hop entry, or NULL if not
*                           found.
*
* RETURNS:
*       GT_OK if found, or
*       GT_NOT_FOUND if not found.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS prvCpssDxChLpmRamMngSearch
(
    IN  PRV_CPSS_DXCH_LPM_RAM_BUCKET_SHADOW_STC  *bucketPtr,
    IN  GT_U8                                    *addrCurBytePtr,
    IN  GT_U32                                   prefix,
    OUT GT_PTR                                   *nextHopPtr
)
{
    GT_U8                                       startAddr;

    GT_U32                                      currPrefix      = prefix;
    PRV_CPSS_DXCH_LPM_RAM_BUCKET_SHADOW_STC     *currBucketPtr  = bucketPtr;
    GT_U8                                       *currAddrByte   = addrCurBytePtr;

    PRV_CPSS_DXCH_LPM_RAM_RANGE_SHADOW_STC      *pRange;

    if(bucketPtr == NULL)
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_FOUND, LOG_ERROR_NO_MSG);

    calcStartEndAddr(*currAddrByte,currPrefix,&startAddr,NULL);

    while (currPrefix > PRV_CPSS_DXCH_LPM_RAM_MAX_LEVEL_LENGTH_CNS)
    {
        pRange = find1stOverlap(currBucketPtr,startAddr,NULL);

        if((pRange->pointerType == CPSS_DXCH_LPM_ROUTE_ENTRY_NEXT_PTR_TYPE_E) ||
           (pRange->pointerType == CPSS_DXCH_LPM_ECMP_ENTRY_PTR_TYPE_E) ||
           (pRange->pointerType == CPSS_DXCH_LPM_QOS_ENTRY_PTR_TYPE_E) ||
           ((GT_U32)(pRange->pointerType) == (GT_U32)PRV_CPSS_DXCH_LPM_RAM_TRIE_PTR_TYPE_E))
        {
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_FOUND, LOG_ERROR_NO_MSG);
        }
        /* search in the next level.    */
        currBucketPtr = pRange->lowerLpmPtr.nextBucket;
        currAddrByte += 1;
        currPrefix = NEXT_BUCKET_PREFIX_MAC(currPrefix,PRV_CPSS_DXCH_LPM_RAM_MAX_LEVEL_LENGTH_CNS);

        calcStartEndAddr(*currAddrByte,currPrefix,&startAddr,NULL);
    }

    *nextHopPtr = getFromTrie(currBucketPtr,startAddr,currPrefix,PRV_CPSS_DXCH_LPM_RAM_MAX_LEVEL_LENGTH_CNS);

    if(*nextHopPtr == NULL)
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_FOUND, LOG_ERROR_NO_MSG);
    else
        return GT_OK;
}

/*******************************************************************************
* prvCpssDxChLpmRamMngDoLpmSearch
*
* DESCRIPTION:
*       Search for the existence of a given (address,Prefix) in the given LPM
*       structure. This is a LPM search - meaning if exact search hasn't found
*       data - it is taken from range.
*
* INPUTS:
*       bucketPtr         - A pointer to the bucket to search in.
*       addrCurBytePtr    - The Byte Array represnting the address to search for.
*       protocolStack     - protocol stack to work on.
*
* OUTPUTS:
*       prefixLenPtr      - Points to the number of bits that are actual valid
*                           in the longest match
*       nextHopPtr        - A pointer to the found next hop entry, or NULL if not
*                           found.
*
* RETURNS:
*       GT_OK                    - if found.
*       GT_NOT_FOUND             - if not found.
*       GT_BAD_PARAM             - on wrong devNum or port.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS prvCpssDxChLpmRamMngDoLpmSearch
(
    IN  PRV_CPSS_DXCH_LPM_RAM_BUCKET_SHADOW_STC  *bucketPtr,
    IN  GT_U8                                    *addrCurBytePtr,
    IN  PRV_CPSS_DXCH_LPM_PROTOCOL_STACK_ENT     protocolStack,
    OUT GT_U32                                   *prefixLenPtr,
    OUT GT_PTR                                   *nextHopPtr
)
{
    GT_U8                                    startAddr;
    GT_U32                                   currPrefix;
    GT_U8                                    prefixLevel = 0;
    GT_U32                                   rangePrefixLength;
    PRV_CPSS_DXCH_LPM_RAM_BUCKET_SHADOW_STC  *currBucketPtr = bucketPtr;
    GT_U8                                    *currAddrByte = addrCurBytePtr;
    PRV_CPSS_DXCH_LPM_RAM_RANGE_SHADOW_STC   *pRange;

    if(bucketPtr == NULL)
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_FOUND, LOG_ERROR_NO_MSG);

    *nextHopPtr = NULL;
    switch (protocolStack)
    {
    case PRV_CPSS_DXCH_LPM_PROTOCOL_IPV4_E:
        currPrefix = 32;
        break;
    case PRV_CPSS_DXCH_LPM_PROTOCOL_IPV6_E:
        currPrefix = 128;
        break;
    case PRV_CPSS_DXCH_LPM_PROTOCOL_FCOE_E:
        currPrefix = 24;
        break;
    default:
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
        break;
    }

    do
    {
        calcStartEndAddr(*currAddrByte,currPrefix,&startAddr,NULL);

        pRange = find1stOverlap(currBucketPtr,startAddr,NULL);

        if ((pRange->pointerType == CPSS_DXCH_LPM_ROUTE_ENTRY_NEXT_PTR_TYPE_E) ||
            (pRange->pointerType == CPSS_DXCH_LPM_ECMP_ENTRY_PTR_TYPE_E) ||
            (pRange->pointerType == CPSS_DXCH_LPM_QOS_ENTRY_PTR_TYPE_E))
        {
            *nextHopPtr = (GT_PTR*)pRange->lowerLpmPtr.nextHopEntry;
            rangePrefixLength = mask2PrefixLength(pRange->mask,PRV_CPSS_DXCH_LPM_RAM_MAX_LEVEL_LENGTH_CNS);
            *prefixLenPtr = prefixLevel*PRV_CPSS_DXCH_LPM_RAM_MAX_LEVEL_LENGTH_CNS + rangePrefixLength;
            break;
        }

        /* search in the next level.    */
        currBucketPtr  = pRange->lowerLpmPtr.nextBucket;
        currAddrByte += 1;
        prefixLevel +=1;
        currPrefix   = NEXT_BUCKET_PREFIX_MAC(currPrefix,PRV_CPSS_DXCH_LPM_RAM_MAX_LEVEL_LENGTH_CNS);

    }while (currPrefix > 0);

    if (currPrefix == 0)
    {
        *nextHopPtr = (GT_PTR*)currBucketPtr->rangeList->lowerLpmPtr.nextHopEntry;
        *prefixLenPtr = 0;
    }

    if(*nextHopPtr == NULL)
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_FOUND, LOG_ERROR_NO_MSG);
    else
        return GT_OK;

}

/*******************************************************************************
* getNextEntry
*
* DESCRIPTION:
*       This function is a recursive function that returns the first next hop
*       after the given (inAddr,Prefix).
*
* INPUTS:
*       bucketPtr         - A pointer to the current bucket to search in.
*       addrPtr           - The Byte Array represnting the Address to search for
*                           a consecutive for.
*       prefixPtr         - The inAddr prefix length.
*
* OUTPUTS:
*       addrPtr           - The address search result.
*       prefixPtr         - the address prefix length.
*       nextPointerPtr    - A pointer to the data stored in the Trie, associated
*                           with the found (address,prefix).
*
* RETURNS:
*       GT_OK if found, or
*       GT_NOT_FOUND on failure.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS getNextEntry
(
    IN PRV_CPSS_DXCH_LPM_RAM_BUCKET_SHADOW_STC    *bucketPtr,
    INOUT GT_U8                                   *addrPtr,
    INOUT GT_U32                                  *prefixPtr,
    OUT GT_PTR                                    *nextPointerPtr
)
{
    GT_U32          prefixComp,*curLvlPrefix;
    GT_U8           tmpAddrArray[MAX_LPM_LEVELS_CNS];
    GT_U32          tmpLvlPrefixArray[MAX_LPM_LEVELS_CNS];
    GT_U8           *curAddr,*curInAddr;
    GT_32           tmpPrefix;
    PRV_CPSS_DXCH_LPM_RAM_BUCKET_SHADOW_STC   *bucketPtrArray[MAX_LPM_LEVELS_CNS];
    PRV_CPSS_DXCH_LPM_RAM_BUCKET_SHADOW_STC   **curBucketPtr;

    GT_U8  searchAddr[1]; /* inAddr in GT_U8 format */
    GT_U32  searchPrefix;

    PRV_CPSS_DXCH_LPM_RAM_RANGE_SHADOW_STC   *pRangeArray[MAX_LPM_LEVELS_CNS];
    PRV_CPSS_DXCH_LPM_RAM_RANGE_SHADOW_STC   **pRange,*tmpRange;   /* Used for going over the current bucket's ranges. */
    GT_STATUS   retVal = GT_NOT_FOUND;
    GT_BOOL     moveUp = GT_FALSE;

    if(bucketPtr == NULL)
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_FOUND, LOG_ERROR_NO_MSG);

    tmpPrefix = *prefixPtr;
    curAddr = tmpAddrArray;
    curInAddr = addrPtr;
    curLvlPrefix = tmpLvlPrefixArray;

    bucketPtrArray[0] = bucketPtr;
    curBucketPtr = bucketPtrArray;
    pRange = pRangeArray;
    while(tmpPrefix >= 0)
    {
        prefixComp = (tmpPrefix > PRV_CPSS_DXCH_LPM_RAM_MAX_LEVEL_LENGTH_CNS) ?
                          0 : (PRV_CPSS_DXCH_LPM_RAM_MAX_LEVEL_LENGTH_CNS - tmpPrefix);

        *curLvlPrefix = (tmpPrefix > PRV_CPSS_DXCH_LPM_RAM_MAX_LEVEL_LENGTH_CNS) ?
            PRV_CPSS_DXCH_LPM_RAM_MAX_LEVEL_LENGTH_CNS : tmpPrefix;

        *curAddr = (GT_U8)(*curInAddr & (BIT_MASK_MAC(PRV_CPSS_DXCH_LPM_RAM_MAX_LEVEL_LENGTH_CNS)
                                                   << prefixComp));

        if (tmpPrefix > PRV_CPSS_DXCH_LPM_RAM_MAX_LEVEL_LENGTH_CNS)
        {
            *pRange = find1stOverlap(*curBucketPtr,*curAddr,NULL);

            if (((*pRange)->pointerType == CPSS_DXCH_LPM_ROUTE_ENTRY_NEXT_PTR_TYPE_E) ||
                ((*pRange)->pointerType == CPSS_DXCH_LPM_ECMP_ENTRY_PTR_TYPE_E) ||
                ((*pRange)->pointerType == CPSS_DXCH_LPM_QOS_ENTRY_PTR_TYPE_E) ||
                ((GT_U32)((*pRange)->pointerType) == (GT_U32)PRV_CPSS_DXCH_LPM_RAM_TRIE_PTR_TYPE_E))
            {
                break;
            }

            curBucketPtr++;

            *curBucketPtr = (*pRange)->lowerLpmPtr.nextBucket;
            pRange++;

        }
        else
        {
            /* this is the level ,break */
            break;
        }

        curAddr++;
        curInAddr++;
        curLvlPrefix++;
        tmpPrefix -= PRV_CPSS_DXCH_LPM_RAM_MAX_LEVEL_LENGTH_CNS;
    }

    /* ok from here on we search for the next entry after the given found/not found
       one */

    while (curBucketPtr >= bucketPtrArray)
    {

        searchAddr[0] = *curAddr;
        searchPrefix = *curLvlPrefix;
        retVal = prvCpssDxChLpmTrieGetNext(&(*curBucketPtr)->trieRoot,
                                           PRV_CPSS_DXCH_LPM_RAM_MAX_LEVEL_LENGTH_CNS,
                                           GT_FALSE,searchAddr,&searchPrefix,nextPointerPtr);
        if (retVal == GT_FAIL)
        {
            /* this means the prefix we gave isn't in the trie, so to look for
               it we need to be more smart ,and work a bit */

            *nextPointerPtr = NULL;
            /* first we're gonna take the first node using the force == GT_TRUE*/
            retVal = prvCpssDxChLpmTrieGetNext(&(*curBucketPtr)->trieRoot,
                                               PRV_CPSS_DXCH_LPM_RAM_MAX_LEVEL_LENGTH_CNS,
                                               GT_TRUE,searchAddr,&searchPrefix,nextPointerPtr);
            /* now we're gonna advance till we have a bigger prefix */
            while (((searchAddr[0] < *curAddr) ||
                    ((searchAddr[0] == *curAddr) &&
                     (searchPrefix < *curLvlPrefix))) &&
                   (*nextPointerPtr != NULL))
            {
                *nextPointerPtr = NULL;
                retVal = prvCpssDxChLpmTrieGetNext(&(*curBucketPtr)->trieRoot,
                                                   PRV_CPSS_DXCH_LPM_RAM_MAX_LEVEL_LENGTH_CNS,
                                                   GT_TRUE,searchAddr,&searchPrefix,nextPointerPtr);
            }
            if (*nextPointerPtr == NULL)
            {
                /* this means we haven't found a next so the search ones should
                   be unchanged to indicate not found */
                searchAddr[0] = *curAddr;
                searchPrefix = *curLvlPrefix;
            }
        }
        if (moveUp == GT_FALSE)
        {

            /* ok we need to go down in the trie, see if the found next is down */
            /* find the next down accroding to the buckets */
            tmpRange = find1stOverlap(*curBucketPtr,*curAddr,NULL);
            while (tmpRange != NULL)
            {
                if ((tmpRange->pointerType == CPSS_DXCH_LPM_ROUTE_ENTRY_NEXT_PTR_TYPE_E) ||
                    (tmpRange->pointerType == CPSS_DXCH_LPM_ECMP_ENTRY_PTR_TYPE_E) ||
                    (tmpRange->pointerType == CPSS_DXCH_LPM_QOS_ENTRY_PTR_TYPE_E) ||
                    ((GT_U32)(tmpRange->pointerType) == (GT_U32)PRV_CPSS_DXCH_LPM_RAM_TRIE_PTR_TYPE_E))
                {
                    tmpRange = tmpRange->next;
                }
                else
                {
                    break;
                }
            }
            if ((tmpRange == NULL) || (searchAddr[0] <= tmpRange->startAddr))
            {
                if ((searchAddr[0] != (*curAddr)) ||
                    (searchPrefix != *curLvlPrefix))
                {
                    /* ok there is a next deeper one in this level ,record it and break */
                    *curAddr = searchAddr[0];
                    *curLvlPrefix = searchPrefix;
                    break;
                }
            }
        }
        else
        {
            /* ok this means we've been down and now we look for next up
               the found trie search could be the right one to go with unless
               there is down bucket between, lets check */
            tmpRange = (*pRange)->next;
            while (tmpRange != NULL)
            {
                if ((tmpRange->pointerType == CPSS_DXCH_LPM_ROUTE_ENTRY_NEXT_PTR_TYPE_E) ||
                    (tmpRange->pointerType == CPSS_DXCH_LPM_ECMP_ENTRY_PTR_TYPE_E) ||
                    (tmpRange->pointerType == CPSS_DXCH_LPM_QOS_ENTRY_PTR_TYPE_E) ||
                    ((GT_U32)(tmpRange->pointerType) == (GT_U32)PRV_CPSS_DXCH_LPM_RAM_TRIE_PTR_TYPE_E))
                {
                    tmpRange = tmpRange->next;
                }
                else
                {
                    break;
                }
            }

            if ((searchAddr[0] != (*curAddr)) ||
                (searchPrefix != *curLvlPrefix))
            {
                /* in this case the trie next is valid next */
                if ((tmpRange != NULL) &&
                    (tmpRange->startAddr < searchAddr[0]))
                {
                    /* the found range leads to a closer next , go down */
                    *curAddr = tmpRange->startAddr;
                    moveUp = GT_FALSE;
                }
                else
                {
                    /* ok the trie one is a closer next,record it and break */
                    *curAddr = searchAddr[0];
                    *curLvlPrefix = searchPrefix;
                    break;
                }
            }
            else
            {
                /*now only the bucket one could be the next , check if it's valid */
                if (tmpRange != NULL)
                {
                    *curAddr = tmpRange->startAddr;
                    moveUp = GT_FALSE;
                }
                /* the else part is that we continue to move up */
            }
        }

        if (moveUp == GT_FALSE)
        {
            /* not found try to move down */
            (*pRange) = find1stOverlap(*curBucketPtr,*curAddr,NULL);
            while ((*pRange) != NULL)
            {
                if (((*pRange)->pointerType == CPSS_DXCH_LPM_ROUTE_ENTRY_NEXT_PTR_TYPE_E) ||
                    ((*pRange)->pointerType == CPSS_DXCH_LPM_ECMP_ENTRY_PTR_TYPE_E) ||
                    ((*pRange)->pointerType == CPSS_DXCH_LPM_QOS_ENTRY_PTR_TYPE_E) ||
                    ((GT_U32)((*pRange)->pointerType) == (GT_U32)PRV_CPSS_DXCH_LPM_RAM_TRIE_PTR_TYPE_E))
                {
                    (*pRange) = (*pRange)->next;
                }
                else
                {
                    break;
                }
            }
            if ((*pRange) == NULL)
            {
                moveUp = GT_TRUE;
            }
            else
            {
                *curAddr = (*pRange)->startAddr;
                *curLvlPrefix = PRV_CPSS_DXCH_LPM_RAM_MAX_LEVEL_LENGTH_CNS;

                curLvlPrefix++;
                curAddr++;
                curBucketPtr++;

                curInAddr++;

                *curBucketPtr = (*pRange)->lowerLpmPtr.nextBucket;

                pRange++;
                *curAddr = 0;
                *curLvlPrefix = 0;
            }
        }

        if (moveUp == GT_TRUE)
        {
            /* no next , move up */

            curLvlPrefix--;
            curAddr--;
            curBucketPtr--;
            pRange--;
            curInAddr--;
        }
    }

    /* if the array was breached that means we didn't found anything */
    if (curBucketPtr < bucketPtrArray)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_FOUND, LOG_ERROR_NO_MSG);
    }

    /* ok found , now build the address / prefix */
    *prefixPtr = 0;
    while (curInAddr >= addrPtr)
    {
        *curInAddr = *curAddr;
        *prefixPtr += *curLvlPrefix;

        curInAddr--;
        curAddr--;
        curLvlPrefix--;
    }

    return GT_OK;
}

/*******************************************************************************
* prvCpssDxChLpmRamMngEntryGet
*
* DESCRIPTION:
*       This function returns the entry indexed 'index', entries are sorted by
*       (address,prefix) key.
*
* INPUTS:
*       bucketPtr   - A pointer to the bucket to search in.
*       protocol    - the protocol
*       addrPtr     - The address associated with the returned next hop pointer.
*       prefixPtr   - The address prefix length.
*
* OUTPUTS:
*       addrPtr     - The address associated with the returned next hop pointer.
*       prefixPtr   - The address prefix length.
*       nextPtr     - A pointer to the found next hop entry, or NULL if not
*                     found.
*
* RETURNS:
*       GT_OK if the required entry was found, or
*       GT_NOT_FOUND - if no more entries where found in the structure.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS prvCpssDxChLpmRamMngEntryGet
(
    IN    PRV_CPSS_DXCH_LPM_RAM_BUCKET_SHADOW_STC  *bucketPtr,
    IN    PRV_CPSS_DXCH_LPM_PROTOCOL_STACK_ENT     protocol,
    INOUT GT_U8                                    *addrPtr,
    INOUT GT_U32                                   *prefixPtr,
    OUT   GT_PTR                                   *nextPtr
)
{
    GT_STATUS retVal = GT_OK;
    GT_U32 octets=0 ,bits=0 ,i=0, maxNumOfOctets=0;

    retVal = getNextEntry(bucketPtr,addrPtr,prefixPtr,nextPtr);
    if(retVal != GT_OK)
        return retVal;

    /* Reset unused octets and bits in addrPtr according to prefixPtr */

    /* octets --> number of full octets in addrPtr according to the prefix
       bits --> number of bits left in the addrPtr according to the prefix
       so if the prefix is 18 then we have 2 full octets data (octet 0 and 1 should not be masked)
       and 2 more bits of data (need to mask bits 0 to 5) taken from octet 3,
       octet 4 to maxNumOfOctets is all zero */

    if (protocol == PRV_CPSS_DXCH_LPM_PROTOCOL_IPV4_E)
    {
        /* in Ipv4 if the prefix is 32 there is nothing to reset */
        if (*prefixPtr == 32)
            return GT_OK;

        maxNumOfOctets = PRV_CPSS_DXCH_LPM_NUM_OF_OCTETS_IN_IPV4_PROTOCOL_CNS;
    }
    else if (protocol == PRV_CPSS_DXCH_LPM_PROTOCOL_IPV6_E)
    {
        /* in Ipv6 if the prefix is 128 there is nothing to reset */
        if (*prefixPtr == 128)
            return GT_OK;

        maxNumOfOctets = PRV_CPSS_DXCH_LPM_NUM_OF_OCTETS_IN_IPV6_PROTOCOL_CNS;
    }
    else    /* PRV_CPSS_DXCH_LPM_PROTOCOL_FCOE_E */
    {
        /* in FCoE if the prefix is 24 there is nothing to reset */
        if (*prefixPtr == 24)
            return GT_OK;

        maxNumOfOctets = 3;
    }

    octets = (*prefixPtr) / 8;
    bits = (*prefixPtr) % 8;

    addrPtr[octets] &= (BIT_MASK_MAC(bits) << (8 - bits));

    for (i = octets+1; i < maxNumOfOctets; i++)
    {
        addrPtr[i] = 0;
    }

    return GT_OK;
}

/*******************************************************************************
* prvCpssDxChLpmRamMngAllocatedAndBoundMemFree
*
* DESCRIPTION:
*       Free memory that was pre allocated or bound in prvCpssDxChLpmRamMngAllocAvailableMemCheck
*       function. used in case of error in the insertion.
*
* INPUTS:
*       shadowPtr       - the shadow relevant for the devices asked to act on.
*       protocolStack     - protocol stack to work on.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK        - operation finish successfully
*       GT_BAD_PARAM - Bad protocolStack input parameter
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS prvCpssDxChLpmRamMngAllocatedAndBoundMemFree
(
    IN PRV_CPSS_DXCH_LPM_RAM_SHADOW_STC             *shadowPtr,
    IN PRV_CPSS_DXCH_LPM_PROTOCOL_STACK_ENT         protocolStack   
)
{
    GT_U32                                  octetIndex;
    PRV_CPSS_DXCH_LPM_RAM_MEM_INFO_STC      *headOfListToFreePtr; /* head of the list of memories we want to free */
    PRV_CPSS_DXCH_LPM_RAM_MEM_INFO_STC      *tempElemToFreePtr;   /* temp pointer used for free operation */    
    GT_U32                                  maxNumOfOctets=0;

    switch (protocolStack)
    {
        case PRV_CPSS_DXCH_LPM_PROTOCOL_IPV4_E:
            maxNumOfOctets = PRV_CPSS_DXCH_LPM_NUM_OF_OCTETS_IN_IPV4_PROTOCOL_CNS;
            break;
        case PRV_CPSS_DXCH_LPM_PROTOCOL_IPV6_E:
            maxNumOfOctets = PRV_CPSS_DXCH_LPM_NUM_OF_OCTETS_IN_IPV6_PROTOCOL_CNS;
            break;
        case PRV_CPSS_DXCH_LPM_PROTOCOL_FCOE_E:
            maxNumOfOctets = PRV_CPSS_DXCH_LPM_NUM_OF_OCTETS_IN_FCOE_PROTOCOL_CNS;
            break;
        default:
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }
    /* free the allocated/bound RAM memory */
    for (octetIndex=0; octetIndex<maxNumOfOctets; octetIndex++)
    {
        headOfListToFreePtr = shadowPtr->allNewNextMemInfoAllocatedPerOctetArrayPtr[octetIndex]; 
        if (headOfListToFreePtr!=NULL)/* A new alloction was done for this octet */
        {
            while (headOfListToFreePtr->nextMemInfoPtr != NULL) 
            {                                
                tempElemToFreePtr = headOfListToFreePtr->nextMemInfoPtr;
                headOfListToFreePtr->nextMemInfoPtr = headOfListToFreePtr->nextMemInfoPtr->nextMemInfoPtr;
                cpssOsFree(tempElemToFreePtr);                                
            }
                        
            /* check if the last element pointer we need to delete is the root of this protocot+octet or not,
               if this is the root then we need to reset the element, if not then we already assign the nextMemInfoPtr
               to be NULL and there is nothing else to do */
            if (headOfListToFreePtr==(&(shadowPtr->lpmMemInfoArray[protocolStack][octetIndex])))
            {
                /* just to reset the values */
                headOfListToFreePtr->ramIndex=0;
                headOfListToFreePtr->structsBase=0;
                headOfListToFreePtr->structsMemPool=0;                
            }           
        }                        
    }               

    cpssOsMemSet(shadowPtr->allNewNextMemInfoAllocatedPerOctetArrayPtr,0,sizeof(shadowPtr->allNewNextMemInfoAllocatedPerOctetArrayPtr));

    return GT_OK;
}

/*******************************************************************************
* prvCpssDxChLpmRamMngAllocAvailableMemCheck
*
* DESCRIPTION:
*       Check if there is enough available memory to insert a new 
*       Unicast or Multicast address. and if there is allocate it
*       for further use in the insertion.
*
* INPUTS:
*       bucketPtr       - The LPM bucket to check on the LPM insert.
*       destArr         - If holds the unicast address to be inserted.
*       prefix          - Holds the prefix length of destArr.
*       rootBucketFlag  - Indicates the way to deal with a root bucket (if it is).
*       lpmEngineMemPtrPtr - points to a an array of PRV_CPSS_DXCH_LPM_RAM_MEM_INFO_STC
*                         which holds all the memory information needed for where and
*                         how to allocate search memory for each of the lpm levels
*       shadowPtr       - the shadow relevant for the devices asked to act on.
*       protocol        - type of ip protocol stack to work on.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - If there is enough memory for the insertion.
*       GT_OUT_OF_PP_MEM         - otherwise.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS prvCpssDxChLpmRamMngAllocAvailableMemCheck
(
    IN PRV_CPSS_DXCH_LPM_RAM_BUCKET_SHADOW_STC      *bucketPtr,
    IN GT_U8                                        destArr[],
    IN GT_U32                                       prefix,
    IN PRV_CPSS_DXCH_LPM_RAM_ROOT_BUCKET_FLAG_ENT   rootBucketFlag,
    IN PRV_CPSS_DXCH_LPM_RAM_MEM_INFO_STC           **lpmEngineMemPtrPtr,
    IN PRV_CPSS_DXCH_LPM_RAM_SHADOW_STC             *shadowPtr,
    IN PRV_CPSS_DXCH_LPM_PROTOCOL_STACK_ENT         protocol
)
{
    GT_U32      neededMemoryBlocksSizes[MAX_LPM_LEVELS_CNS];  /* Holds memory allocation  */
    GT_UINTPTR  memoryPoolList[MAX_LPM_LEVELS_CNS];            /* needs.                   */
    GT_U32      neededMemoryBlocksOctetsIndexs[MAX_LPM_LEVELS_CNS];
    GT_UINTPTR  *neededMemoryBlocks;
    GT_U32      i;
    GT_STATUS   retVal = GT_OK,retVal2=GT_OK;             /* Function return value.   */
    GT_U32      startIdx;
    GT_U32      *neededMemoryBlocksSizesPtr;
    GT_U32      *neededMemoryBlocksOctetsIndexsPtr;
    GT_UINTPTR  *memoryPoolListPtr;
    GT_U32      newFreeBlockIndex; /* index of a new free block */
    PRV_CPSS_DXCH_LPM_RAM_MEM_INFO_STC  *tempNextMemInfoPtr;/* use for going over the list of blocks per octet */

    PRV_CPSS_DXCH_LPM_RAM_MEM_INFO_STC  **allNewNextMemInfoAllocatedPerOctetArrayPtr;/*(pointer to) an array that holds for each octet 
                                                                                        the allocted new element that need to be freed.
                                                                                        Size of the array is 16 for case of IPV6 */                       
    GT_U32      octetIndex; /* the octet we are working on */
    GT_U32      blockStart=0; /* used as a start point for finding a free block */
    PRV_CPSS_DXCH_LPM_RAM_OCTETS_TO_BLOCK_MAPPING_STC   tempLpmRamOctetsToBlockMapping[PRV_CPSS_DXCH_LPM_RAM_NUM_OF_MEMORIES_CNS];/* used for reconstruct in case of error */
    GT_BOOL     justBindAndDontAllocateFreePoolMem = GT_FALSE;

    neededMemoryBlocks  = shadowPtr->neededMemoryBlocks;
    
    allNewNextMemInfoAllocatedPerOctetArrayPtr = shadowPtr->allNewNextMemInfoAllocatedPerOctetArrayPtr;

    /* remember the current list len, to go over only the added ones*/
    startIdx = shadowPtr->neededMemoryListLen;    

    neededMemoryBlocksSizesPtr = &neededMemoryBlocksSizes[MAX_LPM_LEVELS_CNS-1];
    neededMemoryBlocksOctetsIndexsPtr = &neededMemoryBlocksOctetsIndexs[MAX_LPM_LEVELS_CNS-1];
    memoryPoolListPtr = &memoryPoolList[MAX_LPM_LEVELS_CNS-1];

    /* Get needed memory for LPM search insertion.  */
    retVal = lpmCalcNeededMemory(bucketPtr,destArr,prefix,
                                 rootBucketFlag,lpmEngineMemPtrPtr,
                                 &neededMemoryBlocksSizesPtr,
                                 &memoryPoolListPtr,
                                 &shadowPtr->neededMemoryListLen,
                                 &neededMemoryBlocksOctetsIndexsPtr);
    if(retVal != GT_OK)
    {
        return retVal;
    }

    /* keep values in case reconstruct is needed */
    cpssOsMemCpy(tempLpmRamOctetsToBlockMapping,shadowPtr->lpmRamOctetsToBlockMappingPtr,
                 sizeof(PRV_CPSS_DXCH_LPM_RAM_OCTETS_TO_BLOCK_MAPPING_STC)*shadowPtr->numOfLpmMemories);            

    /* reset neededMemoryBlocks before new allocations */
    cpssOsMemSet(neededMemoryBlocks+startIdx, (int)DMM_BLOCK_NOT_FOUND, (sizeof(GT_UINTPTR) * (shadowPtr->neededMemoryListLen-startIdx)));
    
    /* Try to allocate all needed memory.               */
    for(i = startIdx; i < shadowPtr->neededMemoryListLen; i++)
    {
        memoryPoolListPtr++;
        neededMemoryBlocksSizesPtr++;
        neededMemoryBlocksOctetsIndexsPtr++;
        octetIndex = (*neededMemoryBlocksOctetsIndexsPtr);
        if (*memoryPoolListPtr!=0)
        {
            neededMemoryBlocks[i] = 
            prvCpssDmmAllocate(*memoryPoolListPtr,
                               DMM_MIN_ALLOCATE_SIZE_IN_BYTE_CNS * (*neededMemoryBlocksSizesPtr),
                               DMM_MIN_ALLOCATE_SIZE_IN_BYTE_CNS);
        }
        else
        {
            /* no memory pool is bound to the root of the octet list,
               in this case no need to allocate a memory pool struct just
               to bind it to a free pool */
            justBindAndDontAllocateFreePoolMem = GT_TRUE;

        }
        shadowPtr->neededMemoryBlocksSizes[i] = *neededMemoryBlocksSizesPtr;
        /* if the neededMemoryBlocks is 0xFFFFFFFF --> then we did not succeed in allocating the needed memory  */
        if(neededMemoryBlocks[i] == DMM_BLOCK_NOT_FOUND)
        {
            /* check if the octet is bound to more blocks,
               if yes then try to allocte the memory again */

             /* shadowPtr->neededMemoryListLen is the number of blocks
                (just the first in the list of each octet) in neededMemoryBlocks
                --> meaninng number of octets we want to add in the currect prefix */

            /* the search is from the last octet to the first one since
               memoryPoolListPtr was initialized backwards in lpmCalcNeededMemory */
            tempNextMemInfoPtr = lpmEngineMemPtrPtr[octetIndex];
           
            while((tempNextMemInfoPtr->nextMemInfoPtr!= NULL)&&(neededMemoryBlocks[i]==DMM_BLOCK_NOT_FOUND))
            {
                neededMemoryBlocks[i] =
                        prvCpssDmmAllocate(tempNextMemInfoPtr->nextMemInfoPtr->structsMemPool,
                               DMM_MIN_ALLOCATE_SIZE_IN_BYTE_CNS * (*neededMemoryBlocksSizesPtr),
                               DMM_MIN_ALLOCATE_SIZE_IN_BYTE_CNS);

                tempNextMemInfoPtr = tempNextMemInfoPtr->nextMemInfoPtr;
            }
            
            /* could not find an empty space in the current bound blocks - continue looking */
            if ((tempNextMemInfoPtr->nextMemInfoPtr==NULL)&&(neededMemoryBlocks[i]==DMM_BLOCK_NOT_FOUND))
            {
                if(justBindAndDontAllocateFreePoolMem==GT_FALSE)
                {
                    /* allocate a new elemenet block to the list */
                    tempNextMemInfoPtr->nextMemInfoPtr = cpssOsMalloc(sizeof(PRV_CPSS_DXCH_LPM_RAM_MEM_INFO_STC));                
                    if (tempNextMemInfoPtr->nextMemInfoPtr == NULL) 
                    {
                        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_OUT_OF_CPU_MEM, LOG_ERROR_NO_MSG);
                    }    
                }
                
                /* go over all blocks until the memory of the octet can be allocated in the block found */
                while (blockStart < shadowPtr->numOfLpmMemories)
                {            
                    /* find a new free block and bind it to the octet and protocol */
                    retVal = prvCpssDxChLpmRamMngFindValidMemoryBlockForProtocol(shadowPtr,
                                                                             protocol,
                                                                             octetIndex,
                                                                             blockStart,
                                                                             &newFreeBlockIndex);
                    if (retVal != GT_OK)
                    {                        
                        break;
                    }
                    
                    /* allocate the memory needed from the new structsMemPool bound */
                     neededMemoryBlocks[i] =
                        prvCpssDmmAllocate(shadowPtr->lpmRamStructsMemPoolPtr[newFreeBlockIndex],
                               DMM_MIN_ALLOCATE_SIZE_IN_BYTE_CNS * (*neededMemoryBlocksSizesPtr),
                               DMM_MIN_ALLOCATE_SIZE_IN_BYTE_CNS);

                    /* neededMemoryBlocks is not 0 nor 0xFFFFFFFF --> we did succeed in allocating the
                        needed memory from the valid block we found  */
                    if ((neededMemoryBlocks[i] != DMM_BLOCK_NOT_FOUND)&&(neededMemoryBlocks[i] != DMM_MALLOC_FAIL))
                    {
                        if(justBindAndDontAllocateFreePoolMem==GT_TRUE)
                        {
                            tempNextMemInfoPtr->ramIndex = 0; 
                            tempNextMemInfoPtr->structsBase=0;
                            /* bind the new block */
                            tempNextMemInfoPtr->structsMemPool=shadowPtr->lpmRamStructsMemPoolPtr[newFreeBlockIndex];
                            tempNextMemInfoPtr->nextMemInfoPtr=NULL;
                        }
                        else
                        {
                            tempNextMemInfoPtr->nextMemInfoPtr->ramIndex = 0; 
                            tempNextMemInfoPtr->nextMemInfoPtr->structsBase=0;
                            /* bind the new block */
                            tempNextMemInfoPtr->nextMemInfoPtr->structsMemPool=shadowPtr->lpmRamStructsMemPoolPtr[newFreeBlockIndex];
                            tempNextMemInfoPtr->nextMemInfoPtr->nextMemInfoPtr=NULL;
                        }

                        /* mark the block as used */
                        shadowPtr->lpmRamOctetsToBlockMappingPtr[newFreeBlockIndex].isBlockUsed=GT_TRUE;   
                        /*  set the block to be used by the specific octet and protocol*/                     
                        PRV_CPSS_DXCH_LPM_RAM_OCTET_TO_BLOCK_MAPPING_SET_MAC(shadowPtr,protocol,octetIndex,newFreeBlockIndex);                        
                        break;
                    }
                    /* if the neededMemoryBlocks is 0 or 0xFFFFFFFF --> then we did not succeed in allocating the
                        needed memory from the valid block we found, look for another valid block or
                        return an error incase of CPU memory allocation fail  */
                    else
                    {
                        if (neededMemoryBlocks[i] != DMM_MALLOC_FAIL)
                        {
                            blockStart = newFreeBlockIndex + 1;
                        }
                        else
                        {
                            retVal = GT_OUT_OF_CPU_MEM;
                            break;
                        }
                    }
                }
                /* if the neededMemoryBlocks is 0 or 0xFFFFFFFF --> then we did not succeed in allocating the needed memory  */
                if ((neededMemoryBlocks[i] == DMM_BLOCK_NOT_FOUND)||(neededMemoryBlocks[i] == DMM_MALLOC_FAIL))
                {
                    if(justBindAndDontAllocateFreePoolMem==GT_FALSE)
                    {
                        /* need to free the allocated block since we do not have any free block to bind to it */
                        cpssOsFree(tempNextMemInfoPtr->nextMemInfoPtr);
                        tempNextMemInfoPtr->nextMemInfoPtr=NULL;
                    }
                    break;
                }
                /* neededMemoryBlocks is not 0 nor 0xFFFFFFFF --> then we did succeed in allocating the needed memory
                   --> save the allocated pool Id */
                else
                {
                    /* We only save the first element allocated or bound per octet */
                    if ((allNewNextMemInfoAllocatedPerOctetArrayPtr[octetIndex]==NULL)||
                        (allNewNextMemInfoAllocatedPerOctetArrayPtr[octetIndex]->structsMemPool==0))
                    {
                        /* keep the head of the list we need to free incase of an error */
                        allNewNextMemInfoAllocatedPerOctetArrayPtr[octetIndex] = tempNextMemInfoPtr;                                                                      
                    }                   
                }
            }
            else
            {
                /* we had a CPU Memory allocation error */
                if(neededMemoryBlocks[i] == DMM_MALLOC_FAIL)
                {
                    retVal = GT_OUT_OF_CPU_MEM;
                    break;
                }      
            }
        }
        else
        {
            /* we had a CPU Memory allocation error */
            if(neededMemoryBlocks[i] == DMM_MALLOC_FAIL)
            {
                retVal = GT_OUT_OF_CPU_MEM;
                break;
            }
        }

        justBindAndDontAllocateFreePoolMem = GT_FALSE;
    }

    if(i != shadowPtr->neededMemoryListLen)
    {
        /* if we get to this point due to CPU error then leave the
           retVal we got in previous stage else return error due to PP */
        if (retVal!=GT_OUT_OF_CPU_MEM)
        {
            retVal = GT_OUT_OF_PP_MEM; 
        }
        
        /* Allocation failed, free all allocated memory. */
        while(i > 0)
        {
            prvCpssDmmFree(neededMemoryBlocks[i-1]);
            i--;
        }

        shadowPtr->neededMemoryListLen = 0;

        /* free the allocated/bound RAM memory */
        retVal2 = prvCpssDxChLpmRamMngAllocatedAndBoundMemFree(shadowPtr,protocol);
        if (retVal2!=GT_OK)
        {
            return retVal2;
        }
        /* in case of fail we will need to reconstruct to the status we had before the call to prvCpssDxChLpmRamMngAllocAvailableMemCheck */
        cpssOsMemCpy(shadowPtr->lpmRamOctetsToBlockMappingPtr,tempLpmRamOctetsToBlockMapping,
                     sizeof(PRV_CPSS_DXCH_LPM_RAM_OCTETS_TO_BLOCK_MAPPING_STC)*shadowPtr->numOfLpmMemories);                                                          
        
        return retVal;
    }
    return retVal;
}

/*******************************************************************************
* prvCpssDxChLpmRamMngBucketTreeWrite
*
* DESCRIPTION:
*       write an lpm bucket tree to the HW, and if neccessary allocate memory
*       for it - assuming there is enough memory
*
* INPUTS:
*       rootRangePtr   - the range which holds the root bucket of the lpm.
*       lpmEngineMemPtrPtr - points to a an array of PRV_CPSS_DXCH_LPM_RAM_MEM_INFO_STC
*                          which holds all the memory information needed for where and
*                         how to allocate search memory for each of the lpm levels
*       shadowPtr      - the shadow relevant for the devices asked to act on.
*       updateType     - is this an update only for the trie, overwrite of
*                        the whole tree , or update and allocate memory.
*       protocolStack  - the protocol Stack (relvant only if updateType ==
*                        PRV_CPSS_DXCH_LPM_RAM_TRIE_UPDATE_HW_AND_ALLOC_MEM_E)
*       vrId           - VR Id (relvant only if updateType ==
*                        PRV_CPSS_DXCH_LPM_RAM_TRIE_UPDATE_HW_AND_ALLOC_MEM_E)
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - If there is enough memory for the insertion.
*       GT_OUT_OF_PP_MEM         - otherwise.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS prvCpssDxChLpmRamMngBucketTreeWrite
(
    IN PRV_CPSS_DXCH_LPM_RAM_RANGE_SHADOW_STC       *rootRangePtr,
    IN PRV_CPSS_DXCH_LPM_RAM_MEM_INFO_STC           **lpmEngineMemPtrPtr,
    IN PRV_CPSS_DXCH_LPM_RAM_SHADOW_STC             *shadowPtr,
    IN PRV_CPSS_DXCH_LPM_RAM_TRIE_UPDATE_TYPE_ENT   updateType,
    IN PRV_CPSS_DXCH_LPM_PROTOCOL_STACK_ENT         protocolStack,
    IN GT_U32                                       vrId
)
{
    GT_STATUS retVal = GT_OK;
    PRV_CPSS_DXCH_LPM_RAM_RANGE_SHADOW_STC       **currRange,*rangeMem[MAX_LPM_LEVELS_CNS];
    PRV_CPSS_DXCH_LPM_RAM_MEM_INFO_STC **currLpmEnginePtr;
    GT_BOOL forceWriteWholeBucket;
    LPM_BUCKET_UPDATE_MODE_ENT bucketUpdateMode;
    GT_BOOL isDestTreeRootBucket, useCompressed2;

    PRV_CPSS_DXCH_LPM_RAM_BUCKET_PARENT_WRITE_FUNC_INFO_STC parentWriteFuncInfo;

    PRV_CPSS_DXCH_LPM_RAM_UPDATE_VRF_TABLE_FUNC_PARAMS_STC writeFuncData;

    /* update the info for the parent write function */
    parentWriteFuncInfo.bucketParentWriteFunc =
        prvCpssDxChLpmRamUpdateVrTableFuncWrapper;

    writeFuncData.shadowPtr = shadowPtr;
    writeFuncData.protocol = protocolStack;
    writeFuncData.vrId = vrId;

    parentWriteFuncInfo.data = (GT_PTR)(&writeFuncData);

    /* intialize the range memory */
    rangeMem[0] = rootRangePtr;

    /* start with the first level */
    currLpmEnginePtr = lpmEngineMemPtrPtr;
    currRange = rangeMem;

    forceWriteWholeBucket =
        (updateType == PRV_CPSS_DXCH_LPM_RAM_TRIE_WRITE_HW_AND_ALLOC_MEM_E)?
        GT_TRUE : GT_FALSE;

    bucketUpdateMode =
        (updateType == PRV_CPSS_DXCH_LPM_RAM_TRIE_UPDATE_HW_AND_ALLOC_MEM_E)?
        LPM_BUCKET_UPDATE_HW_AND_MEM_RESIZE_E:
        LPM_BUCKET_UPDATE_HW_AND_MEM_ALLOC_E;

    while(currRange >= rangeMem)
    {
        if ((*currRange) == NULL)
        {
            /* this means that we finished with this level - move back up a level*/
            currRange--;
            currLpmEnginePtr--;
            if (currRange >= rangeMem)
            {
                if (((updateType == PRV_CPSS_DXCH_LPM_RAM_TRIE_WRITE_HW_AND_ALLOC_MEM_E) ||
                     /* in update mode, write only if this bucket needs update */
                     ((updateType != PRV_CPSS_DXCH_LPM_RAM_TRIE_WRITE_HW_AND_ALLOC_MEM_E) &&
                      (((*currRange)->lowerLpmPtr.nextBucket->bucketHwUpdateStat
                        != PRV_CPSS_DXCH_LPM_RAM_BUCKET_HW_OK_E) ||
                       ((*currRange)->lowerLpmPtr.nextBucket->bucketHwUpdateStat
                        != PRV_CPSS_DXCH_LPM_RAM_BUCKET_HW_OK_UPDATE_SIBLINGS_E)))))
                {
                    isDestTreeRootBucket = (currLpmEnginePtr == lpmEngineMemPtrPtr) ? GT_TRUE : GT_FALSE;
                    useCompressed2 = (isDestTreeRootBucket == GT_TRUE) ? GT_FALSE : GT_TRUE;
                    /* now that we finished the lower levels , write this bucket */
                    retVal =
                        updateMirrorBucket((*currRange)->lowerLpmPtr.nextBucket,
                                           bucketUpdateMode,isDestTreeRootBucket,GT_FALSE,
                                           forceWriteWholeBucket,GT_FALSE,
                                           useCompressed2,currLpmEnginePtr,
                                           &(*currRange)->pointerType,
                                           shadowPtr,&parentWriteFuncInfo);
                    if (retVal != GT_OK)
                    {
                        return retVal;
                    }
                }
                else if ((updateType != PRV_CPSS_DXCH_LPM_RAM_TRIE_WRITE_HW_AND_ALLOC_MEM_E) ||
                         (currRange == rangeMem))
                {
                    /* it could be that it's BUCKET_HW_UPDATE_SIBLINGS we need
                       to reset it */
                    (*currRange)->lowerLpmPtr.nextBucket->bucketHwUpdateStat
                        = PRV_CPSS_DXCH_LPM_RAM_BUCKET_HW_OK_E;
                }

                (*currRange) = (*currRange)->next;
            }
        }
        else if (((*currRange)->pointerType == CPSS_DXCH_LPM_ROUTE_ENTRY_NEXT_PTR_TYPE_E) ||
                 ((*currRange)->pointerType == CPSS_DXCH_LPM_ECMP_ENTRY_PTR_TYPE_E) ||
                 ((*currRange)->pointerType == CPSS_DXCH_LPM_QOS_ENTRY_PTR_TYPE_E) ||
                 /* if we're in "update only" mode then check if this next bucket
                    or it's siblings need update */
                 ((updateType != PRV_CPSS_DXCH_LPM_RAM_TRIE_WRITE_HW_AND_ALLOC_MEM_E) &&
                  ((*currRange)->lowerLpmPtr.nextBucket->bucketHwUpdateStat ==
                   PRV_CPSS_DXCH_LPM_RAM_BUCKET_HW_OK_E)))
        {
            (*currRange) = (*currRange)->next;
        }
        else
        {
            /* this means this range has lower levels. go and explore */
            currRange[1] = (*currRange)->lowerLpmPtr.nextBucket->rangeList;

            currRange++;
            currLpmEnginePtr++;
        }
    }

    return retVal;
}

/*******************************************************************************
* prvCpssDxChLpmRamMngMemTraverse
*
* DESCRIPTION:
*       a tree traverse function to set/retrive all the memory allocations in this
*       lpm bucket tree. it is done in DFS fashion.
*
* INPUTS:
*       traverseOp               - the traverse operation done on the lpm trie.
*       memAllocArrayPtr         - the array that holds the information of memory
*                                  allocations.
*       memAllocArrayIndexPtr    - the index in the array this function should start
*                                  using.
*       memAllocArraySize        - the size of the above array.
*       rootRangePtr             - the range which holds the root bucket of the lpm.
*       setRecoredRootBucketMem  - whether to set/record the root bucket's memory.
*       lpmEngineMemPtrPtr       - points to a an array of PRV_CPSS_DXCH_LPM_RAM_MEM_INFO_STC
*                                  which holds all the memory information needed for where and
*                                  how to allocate search memory for each of the lpm levels
*
* OUTPUTS:
*       stopPointIterPtr         - an iterator that is returned to point where we stopped
*                                  in the lpm tree, so we can continue from there in the
*                                  next call.
*
* RETURNS:
*       GT_OK                    - If there is enough memory for the insertion.
*       GT_OUT_OF_PP_MEM         - otherwise.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       if *stopPointIter ==0 we start at the begining.
*       and if the returned *stopPointIter==0 the we finished with this tree.
*
*******************************************************************************/
GT_STATUS prvCpssDxChLpmRamMngMemTraverse
(
    IN PRV_CPSS_DXCH_LPM_RAM_MEM_TRAVERSE_OP_ENT    traverseOp,
    IN PRV_CPSS_DXCH_LPM_RAM_MEM_ALLOC_INFO_STC     *memAllocArrayPtr,
    IN GT_U32                                       *memAllocArrayIndexPtr,
    IN GT_U32                                       memAllocArraySize,
    IN PRV_CPSS_DXCH_LPM_RAM_RANGE_SHADOW_STC       *rootRangePtr,
    IN GT_BOOL                                      setRecoredRootBucketMem,
    IN PRV_CPSS_DXCH_LPM_RAM_MEM_INFO_STC           **lpmEngineMemPtrPtr,
    OUT GT_UINTPTR                                  *stopPointIterPtr
)
{
    GT_STATUS retVal = GT_OK;
    PRV_CPSS_DXCH_LPM_RAM_TREE_ITERATOR_STC *iterPtr;
    GT_U32 bucketSize;
    GT_UINTPTR hwBucketOffsetHandle = 0;
    PRV_CPSS_DXCH_LPM_RAM_MEM_INFO_STC  *tempNextMemInfoPtr;/* use for going over the list of blocks per octet */

    if ((memAllocArrayIndexPtr == NULL) ||
        (rootRangePtr == NULL) ||
        (stopPointIterPtr == NULL))
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_FAIL, LOG_ERROR_NO_MSG);
    }

    if (((traverseOp != PRV_CPSS_DXCH_LPM_RAM_MEM_TRAVERSE_COUNT_E) &&
         (traverseOp != PRV_CPSS_DXCH_LPM_RAM_MEM_TRAVERSE_COUNT_WITH_PCL_E)) &&
        (memAllocArrayPtr == NULL))
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_FAIL, LOG_ERROR_NO_MSG);
    }

    /* first allocate memory for the iterator if needed */
    if (*stopPointIterPtr == 0)
    {
        iterPtr = (PRV_CPSS_DXCH_LPM_RAM_TREE_ITERATOR_STC*)cpssOsLpmMalloc(sizeof (PRV_CPSS_DXCH_LPM_RAM_TREE_ITERATOR_STC));
        *stopPointIterPtr = (GT_UINTPTR)iterPtr;

        iterPtr->currRangePtr = iterPtr->rangePtrArray;
        iterPtr->currLpmEnginePtr = lpmEngineMemPtrPtr;

        /* intialize the range memory */
        iterPtr->rangePtrArray[0] = rootRangePtr;
    }
    else
    {
        iterPtr = (PRV_CPSS_DXCH_LPM_RAM_TREE_ITERATOR_STC*)*stopPointIterPtr;
    }

    while(iterPtr->currRangePtr >= iterPtr->rangePtrArray)
    {
        if ((*iterPtr->currRangePtr) == NULL)
        {
            /* this means that we finished with this level - move back up a level*/
            iterPtr->currRangePtr--;
            iterPtr->currLpmEnginePtr--;
            if (iterPtr->currRangePtr >= iterPtr->rangePtrArray)
            {
                /* first address the root bucket issue (set/record it or not )*/
                if ((iterPtr->currRangePtr != iterPtr->rangePtrArray) ||
                    (setRecoredRootBucketMem == GT_TRUE))
                {
                    switch(traverseOp)
                    {
                    case PRV_CPSS_DXCH_LPM_RAM_MEM_TRAVERSE_SET_E:
                    case PRV_CPSS_DXCH_LPM_RAM_MEM_TRAVERSE_SET_WITH_PCL_E:
                        /* first check we didn't by mistake have a next hop */
                        if (((*iterPtr->currRangePtr)->pointerType == CPSS_DXCH_LPM_ROUTE_ENTRY_NEXT_PTR_TYPE_E) ||
                            ((*iterPtr->currRangePtr)->pointerType == CPSS_DXCH_LPM_ECMP_ENTRY_PTR_TYPE_E) ||
                            ((*iterPtr->currRangePtr)->pointerType == CPSS_DXCH_LPM_QOS_ENTRY_PTR_TYPE_E))
                        {
                            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_FAIL, LOG_ERROR_NO_MSG);
                        }

                        /* now that we finished the lower levels , use the memory , but
                        first check if the size fits */
                        switch ((*iterPtr->currRangePtr)->lowerLpmPtr.nextBucket->bucketType)
                        {
                        case CPSS_DXCH_LPM_COMPRESSED_1_NODE_PTR_TYPE_E:
                            bucketSize =
                                (*iterPtr->currRangePtr)->lowerLpmPtr.nextBucket->numOfRanges + PRV_CPSS_DXCH_LPM_RAM_BUCKET_BIT_VEC_SIZE_COMPRESSED_1_CNS;
                            break;
                        case CPSS_DXCH_LPM_COMPRESSED_2_NODE_PTR_TYPE_E:
                            bucketSize =
                                (*iterPtr->currRangePtr)->lowerLpmPtr.nextBucket->numOfRanges + PRV_CPSS_DXCH_LPM_RAM_BUCKET_BIT_VEC_SIZE_COMPRESSED_2_CNS;
                            break;
                        case CPSS_DXCH_LPM_REGULAR_NODE_PTR_TYPE_E:
                            bucketSize =
                                (*iterPtr->currRangePtr)->lowerLpmPtr.nextBucket->numOfRanges + PRV_CPSS_DXCH_LPM_RAM_BUCKET_BIT_VEC_SIZE_REGULAR_CNS;

                            break;
                        default:
                            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_FAIL, LOG_ERROR_NO_MSG);
                        }

                        /* check the size */
                        if (memAllocArrayPtr[*memAllocArrayIndexPtr].memSize != bucketSize)
                        {
                            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
                        }

                        /* if already allocated , check for the same place */
                        if (((*iterPtr->currRangePtr)->lowerLpmPtr.nextBucket->hwBucketOffsetHandle != 0) &&
                            (PRV_CPSS_DXCH_LPM_RAM_GET_LPM_OFFSET_FROM_DMM_MAC((*iterPtr->currRangePtr)->lowerLpmPtr.nextBucket->hwBucketOffsetHandle) !=
                             memAllocArrayPtr[*memAllocArrayIndexPtr].memAddr))
                        {
                            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
                        }

                        /* ok the size fits , alloc */
                        retVal = prvCpssDmmAllocateByPtr((*iterPtr->currLpmEnginePtr)->structsMemPool,
                                                         DMM_MIN_ALLOCATE_SIZE_IN_BYTE_CNS*memAllocArrayPtr[*memAllocArrayIndexPtr].memAddr,
                                                         DMM_MIN_ALLOCATE_SIZE_IN_BYTE_CNS*memAllocArrayPtr[*memAllocArrayIndexPtr].memSize,
                                                         DMM_MIN_ALLOCATE_SIZE_IN_BYTE_CNS,
                                                         &hwBucketOffsetHandle);
                        if (retVal != GT_OK)
                        {
                            /* go over all blocks bound to the octet and try to allocate */
                            tempNextMemInfoPtr = (*iterPtr->currLpmEnginePtr);

                            while((retVal!=GT_OK)&&(tempNextMemInfoPtr->nextMemInfoPtr!= NULL)&&(hwBucketOffsetHandle==0)) 
                            {
                                retVal = prvCpssDmmAllocateByPtr(tempNextMemInfoPtr->nextMemInfoPtr->structsMemPool,
                                                         DMM_MIN_ALLOCATE_SIZE_IN_BYTE_CNS*memAllocArrayPtr[*memAllocArrayIndexPtr].memAddr,
                                                         DMM_MIN_ALLOCATE_SIZE_IN_BYTE_CNS*memAllocArrayPtr[*memAllocArrayIndexPtr].memSize,
                                                         DMM_MIN_ALLOCATE_SIZE_IN_BYTE_CNS,
                                                         &hwBucketOffsetHandle);

                                tempNextMemInfoPtr = tempNextMemInfoPtr->nextMemInfoPtr;                
                            }

                            if (retVal != GT_OK)
                                return retVal;
                        }

                        (*iterPtr->currRangePtr)->lowerLpmPtr.nextBucket->hwBucketOffsetHandle = hwBucketOffsetHandle;
                        /* set Hw bucket update status */
                        (*iterPtr->currRangePtr)->lowerLpmPtr.nextBucket->bucketHwUpdateStat =
                            memAllocArrayPtr[*memAllocArrayIndexPtr].bucketHwUpdateStat;

                        break;
                    case PRV_CPSS_DXCH_LPM_RAM_MEM_TRAVERSE_REC_E:
                    case PRV_CPSS_DXCH_LPM_RAM_MEM_TRAVERSE_REC_WITH_PCL_E:
                        /* now that we finished the lower levels , record this bucket
                           memory */
                        memAllocArrayPtr[*memAllocArrayIndexPtr].memAddr =
                            PRV_CPSS_DXCH_LPM_RAM_GET_LPM_OFFSET_FROM_DMM_MAC((*iterPtr->currRangePtr)->lowerLpmPtr.nextBucket->hwBucketOffsetHandle);

                        memAllocArrayPtr[*memAllocArrayIndexPtr].memSize =
                            PRV_CPSS_DXCH_LPM_RAM_GET_LPM_SIZE_FROM_DMM_MAC((*iterPtr->currRangePtr)->lowerLpmPtr.nextBucket->hwBucketOffsetHandle);
                        /* Get Hw bucket update status */
                        memAllocArrayPtr[*memAllocArrayIndexPtr].bucketHwUpdateStat =
                            (*iterPtr->currRangePtr)->lowerLpmPtr.nextBucket->bucketHwUpdateStat;

                        break;
                    case PRV_CPSS_DXCH_LPM_RAM_MEM_TRAVERSE_COUNT_E:
                    case PRV_CPSS_DXCH_LPM_RAM_MEM_TRAVERSE_COUNT_WITH_PCL_E:
                    default:
                        break;
                    }

                    (*memAllocArrayIndexPtr)++;
                }

                (*iterPtr->currRangePtr) = (*iterPtr->currRangePtr)->next;

                /* check if we run out of memory (or quate)*/
                if (*memAllocArrayIndexPtr >= memAllocArraySize)
                {
                    /* this is where we stop now, we'll continue later*/
                    return (retVal);
                }
            }
        }
        else if (((*iterPtr->currRangePtr)->pointerType == CPSS_DXCH_LPM_ROUTE_ENTRY_NEXT_PTR_TYPE_E) ||
                 ((*iterPtr->currRangePtr)->pointerType == CPSS_DXCH_LPM_ECMP_ENTRY_PTR_TYPE_E) ||
                 ((*iterPtr->currRangePtr)->pointerType == CPSS_DXCH_LPM_QOS_ENTRY_PTR_TYPE_E) ||
                 (((GT_U32)((*iterPtr->currRangePtr)->pointerType) == (GT_U32)PRV_CPSS_DXCH_LPM_RAM_TRIE_PTR_TYPE_E) &&
                  (((*iterPtr->currRangePtr)->lowerLpmPtr.nextBucket->bucketType == CPSS_DXCH_LPM_ROUTE_ENTRY_NEXT_PTR_TYPE_E) ||
                   ((*iterPtr->currRangePtr)->lowerLpmPtr.nextBucket->bucketType == CPSS_DXCH_LPM_ECMP_ENTRY_PTR_TYPE_E) ||
                   ((*iterPtr->currRangePtr)->lowerLpmPtr.nextBucket->bucketType == CPSS_DXCH_LPM_QOS_ENTRY_PTR_TYPE_E))))
        {
            (*iterPtr->currRangePtr) = (*iterPtr->currRangePtr)->next;
        }
        else
        {
            /* this means this range has lower levels. go and explore */
            iterPtr->currRangePtr[1] = (*iterPtr->currRangePtr)->lowerLpmPtr.nextBucket->rangeList;

            iterPtr->currRangePtr++;
            iterPtr->currLpmEnginePtr++;

        }
    }

    /* if we reached here it means we finished the tree , so free the iterator
       and make it 0 */
    cpssOsLpmFree(iterPtr);
    *stopPointIterPtr = 0;

    return retVal;
}

/*******************************************************************************
* prvCpssDxChLpmRamMngRootBucketCreate
*
* DESCRIPTION:
*       Create a shadow root bucket for a specific virtual router/forwarder Id
*       and protocol, and write it to the HW.
*
* INPUTS:
*       shadowPtr                   - the shadow to work on
*       vrId                        - The virtual router/forwarder ID
*       protocol                    - the protocol
*       defUcNextHopEntryPtr        - the default unicast nexthop
*       defReservedNextHopEntryPtr  - the default reserved range nexthop
*       defMcNextHopEntryPtr        - the default multicast nexthop
*       updateHw                    - whether to update the HW
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                   - on success
*       GT_BAD_PARAM            - on bad input parameters
*       GT_OUT_OF_CPU_MEM       - no memory
*       GT_FAIL                 - on other failure
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS prvCpssDxChLpmRamMngRootBucketCreate
(
    IN PRV_CPSS_DXCH_LPM_RAM_SHADOW_STC             *shadowPtr,
    IN GT_U32                                       vrId,
    IN PRV_CPSS_DXCH_LPM_PROTOCOL_STACK_ENT         protocol,
    IN PRV_CPSS_DXCH_LPM_ROUTE_ENTRY_POINTER_STC    *defUcNextHopEntryPtr,
    IN PRV_CPSS_DXCH_LPM_ROUTE_ENTRY_POINTER_STC    *defReservedNextHopEntryPtr,
    IN PRV_CPSS_DXCH_LPM_ROUTE_ENTRY_POINTER_STC    *defMcNextHopEntryPtr,
    IN GT_BOOL                                      updateHw
)
{
    PRV_CPSS_DXCH_LPM_RAM_BUCKET_SHADOW_STC         *newRootBucket;
    PRV_CPSS_DXCH_LPM_RAM_MEM_INFO_STC              **lpmEngineMemPtrPtr;
    PRV_CPSS_DXCH_LPM_RAM_RANGE_SHADOW_STC          *rangeListPtr;
    PRV_CPSS_DXCH_LPM_RAM_RANGE_SHADOW_STC          *secondRangePtr = NULL;
    PRV_CPSS_DXCH_LPM_RAM_RANGE_SHADOW_STC          *thirdRangePtr = NULL;
    CPSS_DXCH_LPM_NODE_RANGE_SELECTION_UNT          rangeSelection;
    CPSS_DXCH_LPM_NODE_NEXT_POINTER_STC             nextPointerArray[3];
    GT_U8                                           *shareDevsList;
    GT_U32                                          shareDevListLen, devNum;
    GT_U32                                          memSize, lineOffset;
    GT_U8                                           prefixLength;
    GT_STATUS                                       retVal = GT_OK;
    GT_PTR                                          nextHopEntry = NULL;
    GT_U32                                          secondRangePrefixLength = 0;
    PRV_CPSS_DXCH_LPM_RAM_MEM_INFO_STC              *tempNextMemInfoPtr;/* use for going over the list of blocks per octet */
    GT_U32                                          blockStart = 0; /* used as a start point for finding a free block */
    GT_U32                                          newFreeBlockIndex; /* index of a new free block */

    if ((protocol == PRV_CPSS_DXCH_LPM_PROTOCOL_FCOE_E) && (defMcNextHopEntryPtr != NULL))
    {
        /* FCoE is unicast only */
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }
    if ((defUcNextHopEntryPtr == NULL) && (defMcNextHopEntryPtr == NULL))
    {
        /* there must be at least one default */
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    if (protocol != PRV_CPSS_DXCH_LPM_PROTOCOL_FCOE_E)
    {
        /* we need to add a nexthop to both UC and MC range. So if one range is
           invalid it will get a nexthop anyway. This is only a dummy nexthop
           that will never be hit. */
        if (defUcNextHopEntryPtr == NULL)
        {
            defUcNextHopEntryPtr = defMcNextHopEntryPtr;
            if (protocol == PRV_CPSS_DXCH_LPM_PROTOCOL_IPV4_E)
            {
                defReservedNextHopEntryPtr = defMcNextHopEntryPtr;
            }
        }
        if (defMcNextHopEntryPtr == NULL)
        {
            defMcNextHopEntryPtr = defUcNextHopEntryPtr;
        }
    }

    /* Create the root bucket. It's created with startAddr 0, which is the start
       of the UC address space */
    newRootBucket = prvCpssDxChLpmRamMngCreateNew(defUcNextHopEntryPtr,
                                                  PRV_CPSS_DXCH_LPM_RAM_MAX_LEVEL_LENGTH_CNS);
    if (newRootBucket == NULL)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_OUT_OF_CPU_MEM, LOG_ERROR_NO_MSG);
    }

    rangeListPtr = newRootBucket->rangeList;
    PRV_CPSS_DXCH_LPM_NEXT_PTR_TYPE_CONVERT_MAC(defUcNextHopEntryPtr->routeEntryMethod,rangeListPtr->pointerType);
    if (protocol != PRV_CPSS_DXCH_LPM_PROTOCOL_FCOE_E)
    {
        /* Add a range for MC */
        secondRangePtr = cpssOsLpmMalloc(sizeof(PRV_CPSS_DXCH_LPM_RAM_RANGE_SHADOW_STC));
        if (secondRangePtr == NULL)
        {
            cpssOsLpmFree(rangeListPtr);
            cpssOsLpmFree(newRootBucket);
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_OUT_OF_CPU_MEM, LOG_ERROR_NO_MSG);
        }
        rangeListPtr->next = secondRangePtr;
        if (protocol == PRV_CPSS_DXCH_LPM_PROTOCOL_IPV4_E)
        {
            newRootBucket->numOfRanges = 3;
            secondRangePtr->startAddr = PRV_CPSS_DXCH_LPM_RAM_START_OF_IPV4_MC_ADDRESS_SPACE_CNS;
            secondRangePrefixLength = PRV_CPSS_DXCH_LPM_RAM_IPV4_MC_ADDRESS_SPACE_PREFIX_CNS;
        }
        else    /* PRV_CPSS_DXCH_LPM_PROTOCOL_IPV6_E */
        {
            newRootBucket->numOfRanges = 2;
            secondRangePtr->startAddr = PRV_CPSS_DXCH_LPM_RAM_START_OF_IPV6_MC_ADDRESS_SPACE_CNS;
            secondRangePrefixLength = PRV_CPSS_DXCH_LPM_RAM_IPV6_MC_ADDRESS_SPACE_PREFIX_CNS;
        }
        retVal = prvCpssMathPowerOf2((GT_U8)(secondRangePrefixLength - 1), &secondRangePtr->mask);
        if (retVal != GT_OK)
        {
            cpssOsLpmFree(secondRangePtr);
            cpssOsLpmFree(rangeListPtr);
            cpssOsLpmFree(newRootBucket);
            return retVal;
        }
        PRV_CPSS_DXCH_LPM_NEXT_PTR_TYPE_CONVERT_MAC(defMcNextHopEntryPtr->routeEntryMethod,secondRangePtr->pointerType);
        secondRangePtr->lowerLpmPtr.nextHopEntry = defMcNextHopEntryPtr;
        if (protocol == PRV_CPSS_DXCH_LPM_PROTOCOL_IPV6_E)
        {
            secondRangePtr->next = NULL;
        }
        secondRangePtr->updateRangeInHw = GT_TRUE;

        /* update the trie */
        retVal = insert2Trie(newRootBucket, secondRangePtr->startAddr, secondRangePrefixLength,
                             PRV_CPSS_DXCH_LPM_RAM_MAX_LEVEL_LENGTH_CNS,
                             defMcNextHopEntryPtr);
        if (retVal != GT_OK)
        {
            cpssOsLpmFree(secondRangePtr);
            cpssOsLpmFree(rangeListPtr);
            cpssOsLpmFree(newRootBucket);
            return retVal;
        }
        if (protocol == PRV_CPSS_DXCH_LPM_PROTOCOL_IPV4_E)
        {
            /* Add a range for the reserved address space */
            thirdRangePtr = cpssOsLpmMalloc(sizeof(PRV_CPSS_DXCH_LPM_RAM_RANGE_SHADOW_STC));
            if (thirdRangePtr == NULL)
            {
                cpssOsLpmFree(secondRangePtr);
                cpssOsLpmFree(rangeListPtr);
                cpssOsLpmFree(newRootBucket);
                CPSS_LOG_ERROR_AND_RETURN_MAC(GT_OUT_OF_CPU_MEM, LOG_ERROR_NO_MSG);
            }
            secondRangePtr->next = thirdRangePtr;
            thirdRangePtr->startAddr = PRV_CPSS_DXCH_LPM_RAM_START_OF_IPV4_RESERVED_SPACE_ADDRESS_SPACE_CNS;
            prefixLength = PRV_CPSS_DXCH_LPM_RAM_IPV4_RESERVED_ADDRESS_SPACE_PREFIX_CNS;
            retVal = prvCpssMathPowerOf2((GT_U8)(prefixLength - 1), &thirdRangePtr->mask);
            if (retVal != GT_OK)
            {
                /* delete the second range (the one that represents MC) */
                delFromTrie(newRootBucket, secondRangePtr->startAddr, secondRangePrefixLength,
                            PRV_CPSS_DXCH_LPM_RAM_MAX_LEVEL_LENGTH_CNS, &nextHopEntry);

                cpssOsLpmFree(thirdRangePtr);
                cpssOsLpmFree(secondRangePtr);
                cpssOsLpmFree(rangeListPtr);
                cpssOsLpmFree(newRootBucket);
                return retVal;
            }
            PRV_CPSS_DXCH_LPM_NEXT_PTR_TYPE_CONVERT_MAC(defReservedNextHopEntryPtr->routeEntryMethod,thirdRangePtr->pointerType);
            thirdRangePtr->lowerLpmPtr.nextHopEntry = defReservedNextHopEntryPtr;
            thirdRangePtr->next = NULL;
            thirdRangePtr->updateRangeInHw = GT_TRUE;

            /* update the trie */
            retVal = insert2Trie(newRootBucket, thirdRangePtr->startAddr, prefixLength,
                                 PRV_CPSS_DXCH_LPM_RAM_MAX_LEVEL_LENGTH_CNS,
                                 defReservedNextHopEntryPtr);
            if (retVal != GT_OK)
            {
                /* delete the second range (the one that represents MC) */
                delFromTrie(newRootBucket, secondRangePtr->startAddr, secondRangePrefixLength,
                            PRV_CPSS_DXCH_LPM_RAM_MAX_LEVEL_LENGTH_CNS, &nextHopEntry);
                cpssOsLpmFree(thirdRangePtr);
                cpssOsLpmFree(secondRangePtr);
                cpssOsLpmFree(rangeListPtr);
                cpssOsLpmFree(newRootBucket);
                return retVal;
            }

        }
    }

    newRootBucket->bucketType = CPSS_DXCH_LPM_COMPRESSED_1_NODE_PTR_TYPE_E;

    if (updateHw == GT_TRUE)
    {
        /* both unicast and multicast use ucSearchMemArrayPtr */
        lpmEngineMemPtrPtr = shadowPtr->ucSearchMemArrayPtr[protocol];

        /* set the new bucket's memory size: one line for the bit vector + line(s)
           for the range(s) */
        if (protocol == PRV_CPSS_DXCH_LPM_PROTOCOL_FCOE_E)
        {
            /* one range (unicast) */
            memSize = PRV_CPSS_DXCH_LPM_RAM_BUCKET_BIT_VEC_SIZE_COMPRESSED_1_CNS + 1;
        }
        else if (protocol == PRV_CPSS_DXCH_LPM_PROTOCOL_IPV4_E)
        {
            /* 3 ranges (unicast, multicast, reserved) */
            memSize = PRV_CPSS_DXCH_LPM_RAM_BUCKET_BIT_VEC_SIZE_COMPRESSED_1_CNS + 3;
        }
        else /* PRV_CPSS_DXCH_LPM_PROTOCOL_IPV6_E */
        {
            /* 2 ranges (unicast, multicast) */
            memSize = PRV_CPSS_DXCH_LPM_RAM_BUCKET_BIT_VEC_SIZE_COMPRESSED_1_CNS + 2;
        }

        newRootBucket->hwBucketOffsetHandle =
            prvCpssDmmAllocate(lpmEngineMemPtrPtr[0]->structsMemPool,
                               DMM_MIN_ALLOCATE_SIZE_IN_BYTE_CNS * memSize,
                               DMM_MIN_ALLOCATE_SIZE_IN_BYTE_CNS);

        if (newRootBucket->hwBucketOffsetHandle == DMM_BLOCK_NOT_FOUND)
        {
            /* go over all blocks bound to the octet and try to allocate */
            tempNextMemInfoPtr = lpmEngineMemPtrPtr[0];

            while((tempNextMemInfoPtr->nextMemInfoPtr != NULL) && (newRootBucket->hwBucketOffsetHandle == DMM_BLOCK_NOT_FOUND)) 
            {
                newRootBucket->hwBucketOffsetHandle = 
                                            prvCpssDmmAllocate(tempNextMemInfoPtr->nextMemInfoPtr->structsMemPool,
                                                               DMM_MIN_ALLOCATE_SIZE_IN_BYTE_CNS * memSize,
                                                               DMM_MIN_ALLOCATE_SIZE_IN_BYTE_CNS);
                tempNextMemInfoPtr = tempNextMemInfoPtr->nextMemInfoPtr;                
            }

            /* could not find an empty space in the current bound blocks - continue looking */
            if ((tempNextMemInfoPtr->nextMemInfoPtr == NULL) && (newRootBucket->hwBucketOffsetHandle == DMM_BLOCK_NOT_FOUND))
            {
                /* allocate a new elemenet block to the list */
                tempNextMemInfoPtr->nextMemInfoPtr = cpssOsMalloc(sizeof(PRV_CPSS_DXCH_LPM_RAM_MEM_INFO_STC));                
                if (tempNextMemInfoPtr->nextMemInfoPtr == NULL) 
                {
                    retVal = GT_OUT_OF_CPU_MEM;
                    goto delete_ranges;
                }    
                
                /* go over all blocks until the memory of the octet can be allocated in the block found */
                while (blockStart < shadowPtr->numOfLpmMemories)
                {
                    /* find a new free block and bind it to the octet and protocol */
                    retVal = prvCpssDxChLpmRamMngFindValidMemoryBlockForProtocol(shadowPtr,
                                                                             protocol,
                                                                             0,
                                                                             blockStart,
                                                                             &newFreeBlockIndex);
                    if (retVal != GT_OK)
                    {
                        break;
                    }
                    
                    /* allocate the memory needed from the new structsMemPool bound */
                     newRootBucket->hwBucketOffsetHandle =
                        prvCpssDmmAllocate(shadowPtr->lpmRamStructsMemPoolPtr[newFreeBlockIndex],
                                           DMM_MIN_ALLOCATE_SIZE_IN_BYTE_CNS * memSize,
                                           DMM_MIN_ALLOCATE_SIZE_IN_BYTE_CNS);

                    /* neededMemoryBlocks is not 0 nor 0xFFFFFFFF --> we did succeed in allocating the
                       needed memory from the valid block we found */
                    if ((newRootBucket->hwBucketOffsetHandle != DMM_BLOCK_NOT_FOUND) && (newRootBucket->hwBucketOffsetHandle != DMM_MALLOC_FAIL))
                    {
                        tempNextMemInfoPtr->nextMemInfoPtr->ramIndex = 0; 
                        tempNextMemInfoPtr->nextMemInfoPtr->structsBase = 0;
                        /* bind the new block */
                        tempNextMemInfoPtr->nextMemInfoPtr->structsMemPool = shadowPtr->lpmRamStructsMemPoolPtr[newFreeBlockIndex];
                        tempNextMemInfoPtr->nextMemInfoPtr->nextMemInfoPtr = NULL;

                        /* mark the block as used */
                        shadowPtr->lpmRamOctetsToBlockMappingPtr[newFreeBlockIndex].isBlockUsed = GT_TRUE;   
                        /*  set the block to be used by the specific octet and protocol*/                     
                        PRV_CPSS_DXCH_LPM_RAM_OCTET_TO_BLOCK_MAPPING_SET_MAC(shadowPtr,protocol,0,newFreeBlockIndex);                        
                        break;
                    }
                    /* if the neededMemoryBlocks is 0 or 0xFFFFFFFF --> then we did not succeed in allocating the
                        needed memory from the valid block we found, look for another valid block or
                        return an error incase of CPU memory allocation fail  */
                    else
                    {
                        if (newRootBucket->hwBucketOffsetHandle != DMM_MALLOC_FAIL)
                        {
                            blockStart = newFreeBlockIndex + 1;
                        }
                        else
                        {
                            break;
                        }
                    }
                }
                /* if the neededMemoryBlocks is 0 or 0xFFFFFFFF --> then we did not succeed in allocating the needed memory  */
                if ((newRootBucket->hwBucketOffsetHandle == DMM_BLOCK_NOT_FOUND) || (newRootBucket->hwBucketOffsetHandle == DMM_MALLOC_FAIL))
                {
                    if (newRootBucket->hwBucketOffsetHandle == DMM_BLOCK_NOT_FOUND)
                    {
                        retVal = GT_OUT_OF_PP_MEM; 
                    }
                    else
                    {
                        retVal = GT_OUT_OF_CPU_MEM;
                    }
                    /* need to free the allocated block since we do not have any free block to bind to it */
                    cpssOsFree(tempNextMemInfoPtr->nextMemInfoPtr);
                    tempNextMemInfoPtr->nextMemInfoPtr = NULL;
                }
            }
            else
            {
                /* we had a CPU Memory allocation error */
                if(newRootBucket->hwBucketOffsetHandle == DMM_MALLOC_FAIL)
                {
                    retVal = GT_OUT_OF_CPU_MEM;
                }      
            }

delete_ranges:
            if (retVal != GT_OK)
            {
                if (protocol == PRV_CPSS_DXCH_LPM_PROTOCOL_IPV4_E)
                {
                    /* delete the third range (the one that represents reserved) */
                    delFromTrie(newRootBucket, thirdRangePtr->startAddr, PRV_CPSS_DXCH_LPM_RAM_IPV4_RESERVED_ADDRESS_SPACE_PREFIX_CNS,
                                PRV_CPSS_DXCH_LPM_RAM_MAX_LEVEL_LENGTH_CNS, &nextHopEntry);

                    cpssOsLpmFree(thirdRangePtr);
                }
                if (protocol != PRV_CPSS_DXCH_LPM_PROTOCOL_FCOE_E)
                {
                    /* delete the second range (the one that represents MC) */
                    delFromTrie(newRootBucket, secondRangePtr->startAddr, secondRangePrefixLength,
                                PRV_CPSS_DXCH_LPM_RAM_MAX_LEVEL_LENGTH_CNS, &nextHopEntry);
                    cpssOsLpmFree(secondRangePtr);
                }
                
                cpssOsLpmFree(rangeListPtr);
                cpssOsLpmFree(newRootBucket);
                return retVal;
            }
        }

        lineOffset = PRV_CPSS_DXCH_LPM_RAM_GET_LPM_OFFSET_FROM_DMM_MAC(
            newRootBucket->hwBucketOffsetHandle) + lpmEngineMemPtrPtr[0]->structsBase;
        shareDevsList = shadowPtr->workDevListPtr->shareDevs;
        shareDevListLen = shadowPtr->workDevListPtr->shareDevNum;
        cpssOsMemSet(&rangeSelection, 0,
                     sizeof(CPSS_DXCH_LPM_NODE_RANGE_SELECTION_UNT));
        retVal = getMirrorBucketDataAndUpdateRangesAddress(rangeListPtr,
                                                           lpmEngineMemPtrPtr,
                                                           newRootBucket->bucketType,
                                                           lineOffset,
                                                           &rangeSelection,
                                                           nextPointerArray);
        if (retVal != GT_OK)
        {
            prvCpssDmmFree(newRootBucket->hwBucketOffsetHandle);

            if (protocol == PRV_CPSS_DXCH_LPM_PROTOCOL_IPV4_E)
            {
                /* delete the third range (the one that represents reserved) */
                delFromTrie(newRootBucket, thirdRangePtr->startAddr, PRV_CPSS_DXCH_LPM_RAM_IPV4_RESERVED_ADDRESS_SPACE_PREFIX_CNS,
                            PRV_CPSS_DXCH_LPM_RAM_MAX_LEVEL_LENGTH_CNS, &nextHopEntry);

                cpssOsLpmFree(thirdRangePtr);
            }
            if (protocol != PRV_CPSS_DXCH_LPM_PROTOCOL_FCOE_E)
            {
                /* delete the second range (the one that represents MC) */
                delFromTrie(newRootBucket, secondRangePtr->startAddr, secondRangePrefixLength,
                            PRV_CPSS_DXCH_LPM_RAM_MAX_LEVEL_LENGTH_CNS, &nextHopEntry);
                cpssOsLpmFree(secondRangePtr);
            }
            cpssOsLpmFree(rangeListPtr);
            cpssOsLpmFree(newRootBucket);
            return retVal;
        }

        for (devNum = 0; devNum < shareDevListLen; devNum++)
        {
            retVal = cpssDxChLpmNodeWrite(shareDevsList[devNum],
                                          lineOffset,
                                          newRootBucket->bucketType,
                                          &rangeSelection,
                                          newRootBucket->numOfRanges,
                                          nextPointerArray);
            if (retVal != GT_OK)
            {
                prvCpssDmmFree(newRootBucket->hwBucketOffsetHandle);

                if (protocol == PRV_CPSS_DXCH_LPM_PROTOCOL_IPV4_E)
                {
                    /* delete the third range (the one that represents reserved) */
                    delFromTrie(newRootBucket, thirdRangePtr->startAddr, PRV_CPSS_DXCH_LPM_RAM_IPV4_RESERVED_ADDRESS_SPACE_PREFIX_CNS,
                                PRV_CPSS_DXCH_LPM_RAM_MAX_LEVEL_LENGTH_CNS, &nextHopEntry);

                    cpssOsLpmFree(thirdRangePtr);
                }
                if (protocol != PRV_CPSS_DXCH_LPM_PROTOCOL_FCOE_E)
                {
                    /* delete the second range (the one that represents MC) */
                    delFromTrie(newRootBucket, secondRangePtr->startAddr, secondRangePrefixLength,
                                PRV_CPSS_DXCH_LPM_RAM_MAX_LEVEL_LENGTH_CNS, &nextHopEntry);
                    cpssOsLpmFree(secondRangePtr);
                }
                cpssOsLpmFree(rangeListPtr);
                cpssOsLpmFree(newRootBucket);
                return retVal;
            }
        }
    }

    shadowPtr->vrRootBucketArray[vrId].rootBucket[protocol] = newRootBucket;
    shadowPtr->vrRootBucketArray[vrId].rootBucketType[protocol] =
                                newRootBucket->bucketType;
    shadowPtr->vrRootBucketArray[vrId].valid = GT_TRUE;
    shadowPtr->vrRootBucketArray[vrId].needsHwUpdate = GT_FALSE;
    if (protocol != PRV_CPSS_DXCH_LPM_PROTOCOL_FCOE_E)
    {
        shadowPtr->vrRootBucketArray[vrId].multicastDefault[protocol] =
            defMcNextHopEntryPtr;
    }

    return GT_OK;
}

/*******************************************************************************
* prvCpssDxChLpmRamMngRootBucketDelete
*
* DESCRIPTION:
*       This function deletes the root bucket structure from memory.
*
* INPUTS:
*       shadowPtr               - the shadow to work on
*       vrId                    - The virtual router/forwarder ID
*       protocol                - the protocol
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK        - on success
*       GT_BAD_STATE - if the bucket is not empty
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS prvCpssDxChLpmRamMngRootBucketDelete
(
    IN PRV_CPSS_DXCH_LPM_RAM_SHADOW_STC             *shadowPtr,
    IN GT_U32                                       vrId,
    IN PRV_CPSS_DXCH_LPM_PROTOCOL_STACK_ENT         protocol
)
{
    PRV_CPSS_DXCH_LPM_RAM_BUCKET_SHADOW_STC         *rootBucketPtr;
    GT_U32                                          expectedNumOfRanges;
    GT_U8                                           mcStartAddr = 0;
    GT_U8                                           reservedStartAddr = 0;
    GT_U32                                          mcPrefixLength = 0;
    GT_U32                                          reservedPrefixLength = 0;
    GT_PTR                                          firstNextHopEntry = NULL;
    GT_PTR                                          secondNextHopEntry = NULL;
    GT_PTR                                          thirdNextHopEntry = NULL;

    if (protocol != PRV_CPSS_DXCH_LPM_PROTOCOL_FCOE_E)
    {
        if (protocol == PRV_CPSS_DXCH_LPM_PROTOCOL_IPV4_E)
        {
            expectedNumOfRanges = 3;
            mcStartAddr = PRV_CPSS_DXCH_LPM_RAM_START_OF_IPV4_MC_ADDRESS_SPACE_CNS;
            mcPrefixLength = PRV_CPSS_DXCH_LPM_RAM_IPV4_MC_ADDRESS_SPACE_PREFIX_CNS;
            reservedStartAddr = PRV_CPSS_DXCH_LPM_RAM_START_OF_IPV4_RESERVED_SPACE_ADDRESS_SPACE_CNS;
            reservedPrefixLength = PRV_CPSS_DXCH_LPM_RAM_IPV4_RESERVED_ADDRESS_SPACE_PREFIX_CNS;
        }
        else    /* PRV_CPSS_DXCH_LPM_PROTOCOL_IPV6_E */
        {
            expectedNumOfRanges = 2;
            mcStartAddr = PRV_CPSS_DXCH_LPM_RAM_START_OF_IPV6_MC_ADDRESS_SPACE_CNS;
            mcPrefixLength = PRV_CPSS_DXCH_LPM_RAM_IPV6_MC_ADDRESS_SPACE_PREFIX_CNS;
        }
    }
    else
    {
        expectedNumOfRanges = 1;
    }

    rootBucketPtr = shadowPtr->vrRootBucketArray[vrId].rootBucket[protocol];
    if (rootBucketPtr->numOfRanges != expectedNumOfRanges)
    {
        /* the root bucket is not empty */
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_STATE, LOG_ERROR_NO_MSG);
    }

    if (rootBucketPtr->numOfRanges == 3)
    {
        /* delete the third range (the one that represents the reserved address space) */
        delFromTrie(rootBucketPtr, reservedStartAddr, reservedPrefixLength,
                    PRV_CPSS_DXCH_LPM_RAM_MAX_LEVEL_LENGTH_CNS, &thirdNextHopEntry);
        if (thirdNextHopEntry != NULL)
        {
            cpssOsLpmFree(thirdNextHopEntry);
        }
        cpssOsLpmFree(rootBucketPtr->rangeList->next->next);
        rootBucketPtr->numOfRanges--;
    }

    if (rootBucketPtr->numOfRanges == 2)
    {
        /* delete the second range (the one that represents MC) */
        delFromTrie(rootBucketPtr, mcStartAddr, mcPrefixLength,
                    PRV_CPSS_DXCH_LPM_RAM_MAX_LEVEL_LENGTH_CNS, &secondNextHopEntry);
        if ((secondNextHopEntry != NULL) && (secondNextHopEntry != thirdNextHopEntry))
        {
            cpssOsLpmFree(secondNextHopEntry);
        }
        cpssOsLpmFree(rootBucketPtr->rangeList->next);
    }

    /* Now delete the UC range */
    delFromTrie(rootBucketPtr, 0, 0, PRV_CPSS_DXCH_LPM_RAM_MAX_LEVEL_LENGTH_CNS, &firstNextHopEntry);
    if ((firstNextHopEntry != NULL) && (firstNextHopEntry != secondNextHopEntry) &&
        (firstNextHopEntry != thirdNextHopEntry))
    {
        cpssOsLpmFree(firstNextHopEntry);
    }

    cpssOsLpmFree(rootBucketPtr->rangeList);
    cpssOsLpmFree(rootBucketPtr);

    return GT_OK;
}

/*******************************************************************************
* prvCpssDxChLpmRamMngVrfEntryUpdate
*
* DESCRIPTION:
*       perform an update of the VRF table
*
* INPUTS:
*
*       vrId              - the vrId of the updated VR
*       protocol          - the protocol
*       shadowPtr         - the shadow relevant for the devices asked to act on.
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
GT_STATUS prvCpssDxChLpmRamMngVrfEntryUpdate
(
    IN GT_U32                                       vrId,
    IN PRV_CPSS_DXCH_LPM_PROTOCOL_STACK_ENT         protocol,
    IN PRV_CPSS_DXCH_LPM_RAM_SHADOW_STC             *shadowPtr
)
{
    PRV_CPSS_DXCH_LPM_RAM_ROOT_BUCKET_STC   *vrRootBucket;
    CPSS_DXCH_LPM_NEXT_POINTER_TYPE_ENT     rootBucketType;
    GT_U32                                  headOfTrie;
    GT_U8                                   *shareDevsList;
    GT_U32                                  numOfSharedDevs;
    GT_U32                                  dev;
    GT_STATUS                               retVal;
    GT_U8                                   rootLevelMemory;

    /* rootLevelMemory is 0 for now, need to be changed */
    CPSS_TBD_BOOKMARK
    rootLevelMemory = 0;

    shareDevsList   = shadowPtr->workDevListPtr->shareDevs;
    numOfSharedDevs = shadowPtr->workDevListPtr->shareDevNum;
    vrRootBucket    = &(shadowPtr->vrRootBucketArray[vrId]);

    if (vrRootBucket->rootBucket[protocol] != NULL)
    {
        rootBucketType = vrRootBucket->rootBucketType[protocol];
        headOfTrie =
            PRV_CPSS_DXCH_LPM_RAM_GET_LPM_OFFSET_FROM_DMM_MAC(
                vrRootBucket->rootBucket[protocol]->hwBucketOffsetHandle) +
                      shadowPtr->lpmMemInfoArray[protocol][rootLevelMemory].structsBase;
        for (dev = 0; dev < numOfSharedDevs; dev++)
        {
            retVal = prvCpssDxChLpmHwVrfEntryWrite(shareDevsList[dev],
                                                   vrId,
                                                   protocol,
                                                   rootBucketType,
                                                   headOfTrie);
            if (retVal != GT_OK)
            {
                return retVal;
            }
        }
    }

    return GT_OK;
}

/*******************************************************************************
* prvCpssDxChLpmRamMngFindValidMemoryBlockForProtocol
*
* DESCRIPTION:
*       Search for a memory block.
*
* INPUTS:
*       shadowPtr       - (pointer to) the shadow information
*       protocol        - the protocol
*       octetIndex      - the octetIndex to whom we want to bind the found block 
*       blockStart      - start searching from this block
*
* OUTPUTS:
*       blockIndexPtr   - (pointer to) the index of the first empty block
*
* RETURNS:
*       GT_OK           - on success
*       GT_NOT_FOUND    - an available block was not found
*
* COMMENTS:
*       We try to associate to a given protocol a block that will not 
*       violate full wire speed.
*       If we did not find a block and the allocation mode is
*       PRV_CPSS_DXCH_LPM_RAM_BLOCKS_ALLOCATION_METHOD_DYNAMIC_WITH_BLOCK_SHARING_E
*       we do allocate a block that may cause full wire speed violation.
*
*******************************************************************************/
GT_STATUS prvCpssDxChLpmRamMngFindValidMemoryBlockForProtocol
(
    IN  PRV_CPSS_DXCH_LPM_RAM_SHADOW_STC        *shadowPtr,
    IN  PRV_CPSS_DXCH_LPM_PROTOCOL_STACK_ENT    protocol,
    IN  GT_U32                                  octetIndex,
    IN  GT_U32                                  blockStart,
    OUT GT_U32                                  *blockIndexPtr
)
{
    GT_U32 i;  
    GT_BOOL potentialBlockToUseThatViolateWireSpeedFound=GT_FALSE; 
    GT_U32  potentialBlockToUseThatViolateWireSpeedIndex=0;

    /* first we try to allocated to octetIndex the block with the same index */
    if (shadowPtr->lpmRamOctetsToBlockMappingPtr[octetIndex].isBlockUsed==GT_FALSE)
    {
        *blockIndexPtr=octetIndex;
        return GT_OK;
    }

    for (i = blockStart; i < shadowPtr->numOfLpmMemories; i++) 
    {
        /* try to find an empty block */
        if (shadowPtr->lpmRamOctetsToBlockMappingPtr[i].isBlockUsed==GT_FALSE)
        {
            *blockIndexPtr=i;
            return GT_OK;
        }
    }

    for (i = blockStart; i < shadowPtr->numOfLpmMemories; i++) 
    {        
        /* if the block is used BUT not by the specific input protocol - not violate full wire speed */
        if (PRV_CPSS_DXCH_LPM_RAM_IS_BLOCK_USED_BY_PROTOCOL_GET_MAC(shadowPtr,protocol,i)==GT_FALSE)
        {
            *blockIndexPtr = i;
            return GT_OK;
        }    
       
        if((shadowPtr->lpmRamBlocksAllocationMethod==PRV_CPSS_DXCH_LPM_RAM_BLOCKS_ALLOCATION_METHOD_DYNAMIC_WITH_BLOCK_SHARING_E)&&
           (potentialBlockToUseThatViolateWireSpeedFound==GT_FALSE))
        {
            /* if the block is used by the specific input protocol but not by the specific input octet
               - violate full wire speed */                
            if (PRV_CPSS_DXCH_LPM_RAM_IS_BLOCK_USED_BY_OCTET_IN_PROTOCOL_GET_MAC(shadowPtr,protocol,octetIndex,i)==GT_FALSE)
            {
                potentialBlockToUseThatViolateWireSpeedIndex = i;                    
                potentialBlockToUseThatViolateWireSpeedFound=GT_TRUE;
            }    
        }
    }
    if(potentialBlockToUseThatViolateWireSpeedFound==GT_TRUE)
    {
         *blockIndexPtr = potentialBlockToUseThatViolateWireSpeedIndex; 
          return GT_OK;
    }

    CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_FOUND, LOG_ERROR_NO_MSG);
}


