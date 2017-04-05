/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* prvCpssMemLib.c
*
* DESCRIPTION:
*       Implementation of a memory management unit, for managing all PP's inte-
*       rnal memory for the use of tables and internal structures manipulation
*       functions.
*
*       This implementation provides the following functions for the user:
*
*       -   memDevPoolCreate(void): Creates a new memory pool and returns it's
*           Id, for further allocations / deallocations.
*
*       -   memDevPoolAdd(poolId,memPtr,memSize): Adds a block to a previously
*           created memory pool.
*
*       -   memDevPoolMalloc(poolId,size,bitAllign): Allocates a block of
*           memory from the given pool. The allocated block is alligned by the
*           desired bit allignment.
*
*       -   memDevPoolFree(poolId,ptr): Frees a previously allocated memory
*           block, returns the number of freed bytes.
*
*       -   memDevPoolStatus(poolId,*allocatedSize,*freeSize): This is a stat.
*           function, which calculates the total free / allocated bytes
*           remaining in / allocated from the given pool.
*
*       Inorder for the user to be able to use the above functionality, the file
*       "prvCpssMemLib.h" must be included.
*
* FILE REVISION NUMBER:
*       $Revision: 4 $
*
*******************************************************************************/

#include <cpssCommon/private/prvCpssMemLib.h>
#include <cpssCommon/private/prvCpssSkipList.h>
#include <cpss/extServices/private/prvCpssBindFunc.h>
#include <cpssCommon/cpssPresteraDefs.h>

#define FREEPTR     0
#define ALLOCATED   1


/* Macro definitoins    */
#define MEM_NODE_SIZE(pData)     ((pData == NULL) ? \
                                 0 : \
                                 (((STRUCT_MEM_DATA*)(pData))->size))
#define MEM_NODE_PTR(pData)      ((pData == NULL) ? \
                                 0 : \
                                 (((STRUCT_MEM_DATA*)(pData))->ptr))
#define MEM_NODE_GT_STATUS(pData)   ((pData == NULL) ? \
                                 FREEPTR : \
                                 (((STRUCT_MEM_DATA*)(pData))->status))


/* Forward functions declarations */
static GT_STATUS mmuGroup(GT_UINTPTR  poolId,STRUCT_MEM_DATA *first,
                          STRUCT_MEM_DATA  *second);

/*******************************************************************************
* mmuNewData
*
* DESCRIPTION:
*       Creates a new skip-list data, to be sent to the skip-list functions as a
*       parameter.
*
* INPUTS:
*       ptr    - the ptr field of the new created struct.
*       size   - the size field of the new created struct.
*       status - the status field of the new created struct.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       A pointer to the new created node.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static STRUCT_MEM_DATA* mmuNewData
(
    IN GT_UINTPTR ptr,
    IN GT_U32 size,
    IN GT_U32 status
)
{
    STRUCT_MEM_DATA*   pNewNode;   /* pointer to the new created node. */

    pNewNode = (STRUCT_MEM_DATA*)cpssOsMalloc(sizeof(STRUCT_MEM_DATA));
    if (pNewNode == NULL)
        return NULL;

    pNewNode->ptr       = ptr;
    pNewNode->size      = size;
    pNewNode->status    = status;

    return pNewNode;
}



/*******************************************************************************
* mmuCompareNodes
*
* DESCRIPTION:
*       Compares two memory nodes data, and returns one of the following
*       results: GT_EQUAL / GT_SMALLER / GT_GREATER.
*
* INPUTS:
*       data1   - A pointer to the first struct to be compared.
*       data2   - A pointer to the second struct to be compared.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_EQUAL / GT_SMALLER / GT_GREATER.
*
* COMMENTS:
*       For full declaration of the compare function see:
*       common/gtSkip_List.h
*
*******************************************************************************/
static GT_COMP_RES mmuCompareNodes
(
    IN STRUCT_MEM_DATA *data1,
    IN STRUCT_MEM_DATA *data2
)
{
    /* Check the null cases     */
    if((data1 == NULL) && (data2 == NULL))
    {
        return GT_EQUAL;
    }
    if (data1 == NULL)
    {
        return GT_SMALLER;
    }
    if (data2 == NULL)
    {
        return GT_GREATER;
    }

    /* Both data's are not NULL */
    if (data1->status != data2->status)
    {
        if (data1->status > data2->status)
        {
            return GT_GREATER;
        }
        else
        {
            return GT_SMALLER;
        }
    }
    else
    {
        if (data1->ptr > data2->ptr)
        {
            return GT_GREATER;
        }

        if (data1->ptr < data2->ptr)
        {
            return GT_SMALLER;
        }
    }
    return GT_EQUAL;
}


/*******************************************************************************
* prvCpssMemDevPoolCreate
*
* DESCRIPTION:
*       Creates a new memory pool and returns its Id.
*
* INPUTS:
*       None.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       The pool Id, or NULL (0) if failed to allocte memory.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_UINTPTR prvCpssMemDevPoolCreate()
{
    GT_VOID *poolId;

    if ((poolId = prvCpssSlInit((FUNCP_SL_COMP)mmuCompareNodes,GT_SL_MAXELEM))
         == NULL)
    {
        cpssOsPrintf("Can't Allocate memory.\n");
        return (GT_UINTPTR)NULL;
    }

    return (GT_UINTPTR)poolId;
}



/*******************************************************************************
* prvCpssMemDevPoolAdd
*
* DESCRIPTION:
*       Adds a new memory block to the given pool.
*
* INPUTS:
*       poolId - The Id of the memory pool to add to.
*       memPtr - a pointer to memory block to be added.
*       memSize- the size (in bytes) of the memory block to be added.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK if the block adding succeeded, GT_FAIL otherwise.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS prvCpssMemDevPoolAdd
(
    IN GT_UINTPTR poolId,
    IN GT_UINTPTR memPtr,
    IN GT_U32 memSize
)
{
    STRUCT_MEM_DATA *memNodeData;
    STRUCT_MEM_DATA *tmpMemDataPtr;
    GT_UINTPTR      iteratorId;
    GT_UINTPTR      tmpIteratorId;
    GT_VOID         *poolPtr;

    if (poolId == 0)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_FAIL, LOG_ERROR_NO_MSG);
    }

    poolPtr = (GT_VOID*)poolId;

    if((memNodeData = mmuNewData(memPtr,memSize,FREEPTR)) == NULL)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_FAIL, LOG_ERROR_NO_MSG);
    }
    iteratorId = 0;
    if (prvCpssSlAdd(poolPtr,memNodeData,&iteratorId) == GT_FAIL)
    {
        cpssOsFree(memNodeData);
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_FAIL, LOG_ERROR_NO_MSG);
    }
    else
    {
        tmpIteratorId = iteratorId;
        tmpMemDataPtr = prvCpssSlGetNext(poolPtr,&tmpIteratorId);
        mmuGroup(poolId,memNodeData,tmpMemDataPtr);
        tmpIteratorId = iteratorId;
        tmpMemDataPtr = prvCpssSlGetPrev(poolPtr,&tmpIteratorId);
        mmuGroup(poolId,tmpMemDataPtr,memNodeData);
    }

    return GT_OK;
}


/*******************************************************************************
* prvCpssMemDevPoolMallocLowest
*
* DESCRIPTION:
*       Allocates a lowest free memory block of given size from the pool.
*
* INPUTS:
*       poolId - the Id of the memory pool to allocae from.
*       size   - the size (in bytes) of the memory pool to be allocated.
*       bitAllign - the memory area allignment in bits.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       The address of the allocated area, or NULL (0) if failed.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_UINTPTR prvCpssMemDevPoolMallocLowest
(
    IN GT_UINTPTR poolId,
    IN GT_U32 size,
    IN GT_U32 bitAllign
)
{
    GT_VOID         *pPool;
    STRUCT_MEM_DATA *allocatedData;
    STRUCT_MEM_DATA *allignFragmentData;
    STRUCT_MEM_DATA *tmpMemData = NULL;
    GT_U32 allign;
    GT_UINTPTR nodePtrId = 0;
    GT_32  nodeSizeTail = 0;        /* Signed !! used for minus compare */
    GT_UINTPTR iteratorId;
    GT_U32 addedForAllignment = 0;  /* additional size allocated for allignment */


    if ((poolId == 0) || (size == 0))
    {
        return 0;
    }

    allign = 1 << bitAllign;

    /* Search free block enough size*/
    pPool = (GT_VOID*)poolId;
    iteratorId = 0;
    while (1)
    {
        tmpMemData = prvCpssSlGetNext(pPool,&iteratorId);
        if (tmpMemData == NULL)
        {
            return 0;
        }
        if (MEM_NODE_GT_STATUS(tmpMemData) > FREEPTR)
        {
            return 0;
        }
        nodePtrId = MEM_NODE_PTR(tmpMemData);

        addedForAllignment = (allign - (nodePtrId % allign)) % allign;
        nodeSizeTail = MEM_NODE_SIZE(tmpMemData) -
                                    (size + addedForAllignment);
        if ( nodeSizeTail >= 0)
        {
            break;
        }
    }

    if((allocatedData = mmuNewData(nodePtrId + addedForAllignment,
                                   size,ALLOCATED)) == NULL)
    {
        return 0;
    }


    /* Modify data of the found free block */
    if (nodeSizeTail != 0)
    {
        tmpMemData->ptr =  nodePtrId + addedForAllignment + size;
        tmpMemData->size = nodeSizeTail;

    }
    else
    {
        tmpMemData = prvCpssSlDelete(pPool,tmpMemData);
        cpssOsFree(tmpMemData);
    }

    /* Add a node representing the allignment overhead, if needed. */
    if (addedForAllignment != 0)
    {
        if((allignFragmentData = mmuNewData(nodePtrId,
                                            addedForAllignment,
                                            FREEPTR)) == NULL)
        {
            cpssOsFree(allocatedData);
            return 0;
        }

        if(prvCpssSlAdd(pPool,allignFragmentData,&iteratorId) == GT_FAIL)
        {
            cpssOsFree(allocatedData);
            cpssOsFree(allignFragmentData);
            return 0;
        }
    }

    if (prvCpssSlAdd(pPool,allocatedData,&iteratorId) == GT_FAIL)
    {
        cpssOsFree(allocatedData);
        return 0;
    }

    return allocatedData->ptr;

}


/*******************************************************************************
* prvCpssMemDevPoolMallocHighest
*
* DESCRIPTION:
*       Allocates a highest free memory block of given size from the pool.
*
* INPUTS:
*       poolId - the Id of the memory pool to allocae from.
*       size   - the size (in bytes) of the memory pool to be allocated.
*       bitAllign - the memory area allignment in bits.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       The address of the allocated area, or NULL (0) if failed.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_UINTPTR prvCpssMemDevPoolMallocHighest
(
    IN GT_UINTPTR poolId,
    IN GT_U32 size,
    IN GT_U32 bitAllign
)
{
    GT_VOID         *pPool;
    STRUCT_MEM_DATA *allocatedData;
    STRUCT_MEM_DATA *allignFragmentData;
    STRUCT_MEM_DATA *tmpMemData;
    STRUCT_MEM_DATA *lastTmpMemData;
    GT_U32 allign;
    GT_UINTPTR nodePtrId;
    GT_U32  nodeSize;
    GT_UINTPTR iteratorId;
    GT_U32 allignDescend;
    GT_UINTPTR allocatedBase;


    if ((poolId == 0) || (size == 0))
    {
        return 0;
    }

    allign = 1 << bitAllign;

    /* Search free block enough size*/
    pPool = (GT_VOID*)poolId;
    iteratorId = 0;
    lastTmpMemData = NULL;
    while (1)
    {
        tmpMemData = prvCpssSlGetNext(pPool,&iteratorId);
        if (tmpMemData == NULL)
        {
            break;
        }
        if (MEM_NODE_GT_STATUS(tmpMemData) > FREEPTR)
        {
            break;
        }
        nodePtrId = MEM_NODE_PTR(tmpMemData);
        nodeSize = MEM_NODE_SIZE(tmpMemData);

        allocatedBase = nodePtrId + nodeSize - size;

        allignDescend =
            (allign - (allocatedBase % allign)) % allign;

        allocatedBase -= allignDescend;

        if (nodePtrId <= allocatedBase)
        {
            lastTmpMemData = tmpMemData;
        }
    }

    if (lastTmpMemData == NULL)
    {
        return 0;
    }

    /* last node to alloc memory from it */
    tmpMemData = lastTmpMemData;

    /* repeat calculation: all values may be lost in later loop run */
    nodePtrId = MEM_NODE_PTR(tmpMemData);
    nodeSize = MEM_NODE_SIZE(tmpMemData);

    allocatedBase = nodePtrId + nodeSize - size;

    allignDescend =
        (allign - (allocatedBase % allign)) % allign;

    allocatedBase -= allignDescend;

    allocatedData = mmuNewData(allocatedBase, size, ALLOCATED);
    if (allocatedData == NULL)
    {
        return 0;
    }


    /* Modify data of the found free block */
    if (nodePtrId < allocatedBase)
    {
        tmpMemData->ptr = nodePtrId;
        tmpMemData->size = (GT_U32)(allocatedBase - nodePtrId);

    }
    else
    {
        /* nodePtrId == allocatedBase */
        tmpMemData = prvCpssSlDelete(pPool,tmpMemData);
        cpssOsFree(tmpMemData);
    }

    /* Add a node representing the allignment overhead, if needed. */
    if (allignDescend != 0)
    {
        allignFragmentData = mmuNewData(
            (allocatedBase + size), allignDescend, FREEPTR);

        if (allignFragmentData == NULL)
        {
            cpssOsFree(allocatedData);
            return 0;
        }

        if(prvCpssSlAdd(pPool,allignFragmentData,&iteratorId) == GT_FAIL)
        {
            cpssOsFree(allocatedData);
            cpssOsFree(allignFragmentData);
            return 0;
        }
    }

    if (prvCpssSlAdd(pPool,allocatedData,&iteratorId) == GT_FAIL)
    {
        cpssOsFree(allocatedData);
        return 0;
    }

    return allocatedData->ptr;

}

/*******************************************************************************
* prvCpssMemDevPoolMalloc
*
* DESCRIPTION:
*       Allocates a memory block from the pool.
*
* INPUTS:
*       poolId - the Id of the memory pool to allocae from.
*       size   - the size (in bytes) of the memory pool to be allocated.
*       bitAllign - the memory area allignment in bits.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       The address of the allocated area, or NULL (0) if failed.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_UINTPTR prvCpssMemDevPoolMalloc
(
    IN GT_UINTPTR poolId,
    IN GT_U32 size,
    IN GT_U32 bitAllign
)
{
    return prvCpssMemDevPoolMallocLowest(poolId, size, bitAllign);
}

/*******************************************************************************
* prvCpssMemDevPoolFree
*
* DESCRIPTION:
*       Frees a memory block that was previously allocated from the pool.
*
* INPUTS:
*       poolId - the Id of the memory pool.
*       ptr    - pointer to the memory block to be freed.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       The size of the freed area, or 0 if none.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_U32 prvCpssMemDevPoolFree
(
    IN GT_UINTPTR poolId,
    IN GT_UINTPTR ptr
)
{
    STRUCT_MEM_DATA *pFreedData;
    STRUCT_MEM_DATA *tmpMemDataPtr;
    STRUCT_MEM_DATA delData;
    GT_U32 freeSize;
    GT_UINTPTR  iteratorId;
    GT_UINTPTR  tmpIteratorId;
    GT_VOID *poolPtr;

    if (poolId == 0)
    {
        return 0;
    }
    iteratorId = 0;
    poolPtr = (GT_VOID*)poolId;
    delData.status = ALLOCATED;
    delData.ptr    = ptr;

    pFreedData = prvCpssSlDelete(poolPtr,&delData);

    if(pFreedData == NULL)
    {
        return 0;
    }
    else
    {
        freeSize = pFreedData->size;
    }

    pFreedData->status = FREEPTR;
    if (prvCpssSlAdd(poolPtr,pFreedData,&iteratorId) == GT_FAIL)
    {
        cpssOsFree(pFreedData);
        return 0;
    }
    else
    {
        tmpIteratorId = iteratorId;
        tmpMemDataPtr = prvCpssSlGetNext(poolPtr,&tmpIteratorId);
        mmuGroup(poolId,pFreedData,tmpMemDataPtr);
        tmpIteratorId = iteratorId;
        tmpMemDataPtr = prvCpssSlGetPrev(poolPtr,&tmpIteratorId);
        mmuGroup(poolId,tmpMemDataPtr,pFreedData);
    }

    return freeSize;
}


/*******************************************************************************
* prvCpssMemDevPoolClear
*
* DESCRIPTION:
*       Frees a all the memory blocks that were previously allocated from the
*       pool.
*
* INPUTS:
*       poolId - the Id of the memory pool.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK - if succeeded.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS prvCpssMemDevPoolClear
(
    IN GT_UINTPTR poolId
)
{
    GT_VOID   *poolPtr;

    /* NOTICE! - this function hasn't been debuged yet, if you use it make sure
       it works first */
    STRUCT_MEM_DATA *pFreedData;
    GT_U32 memsize = 0;
    GT_UINTPTR mptr = ~((GT_UINTPTR)0);

    if (poolId == 0)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_FAIL, LOG_ERROR_NO_MSG);
    }
    poolPtr = (GT_VOID*)poolId;

    while((pFreedData = prvCpssSlGetNext(poolPtr,NULL)) != NULL)
    {

        pFreedData = prvCpssSlDelete(poolPtr,pFreedData);
        if (pFreedData == NULL)
        {
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_FAIL, LOG_ERROR_NO_MSG);
        }

        memsize += pFreedData->size;
        mptr = (mptr < pFreedData->ptr) ? mptr : pFreedData->ptr ;

    }

    return prvCpssMemDevPoolAdd(poolId,mptr,memsize);;
}


/*******************************************************************************
* mmuGroup
*
* DESCRIPTION:
*       Gets two nodes and checks if they are sequenced in the memory.
*       if the two nodes can be combined then the first one remains in the
*       skipList and the second one is removed.
*
* INPUTS:
*       poolId - A pointer to memory pool.
*       first  - A pointer to the first memData to be combined.
*       second - A pointer to the second memData to be combined.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK    - if the two nodes where combined,
*       GT_FAIL - otherwise.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS mmuGroup
(
    IN GT_UINTPTR poolId,
    IN STRUCT_MEM_DATA *first,
    IN STRUCT_MEM_DATA *second
)
{
    STRUCT_MEM_DATA *pFreedData;

    if((first == NULL) || (second == NULL))
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_FAIL, LOG_ERROR_NO_MSG);
    }

    if(MEM_NODE_PTR(first) == (GT_UINTPTR)NULL)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_FAIL, LOG_ERROR_NO_MSG);
    }

    if((MEM_NODE_GT_STATUS(first)  != FREEPTR) ||
       (MEM_NODE_GT_STATUS(second) != FREEPTR))
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_FAIL, LOG_ERROR_NO_MSG);
    }

    if (MEM_NODE_PTR(first) + MEM_NODE_SIZE(first) == MEM_NODE_PTR(second))
    {
        (first->size) += MEM_NODE_SIZE(second);
        pFreedData = prvCpssSlDelete((GT_VOID*)poolId,second);
        if(pFreedData != NULL)
        {
            cpssOsFree(pFreedData);
        }
        return GT_OK;
    }
    else
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_FAIL, LOG_ERROR_NO_MSG);
    }

}
/*******************************************************************************
* prvCpssMemDevPoolStatus
*
* DESCRIPTION:
*       This function returns the number of allocated & free bytes in a given
*       memory pool.
*
* INPUTS:
*       poolId  - The poolId to check the status for.
*
* OUTPUTS:
*       allocatedSize   - Number of allocated bytes.
*       freeSize        - Number of free bytes.
*
* RETURNS:
*       GT_OK
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS prvCpssMemDevPoolStatus
(
    IN GT_UINTPTR poolId,
    OUT GT_U32 *allocatedSize,
    OUT GT_U32 *freeSize
)
{
    GT_UINTPTR      iteratorId;
    GT_VOID         *poolPtr;
    STRUCT_MEM_DATA *memData;

    if (poolId == 0)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_FAIL, LOG_ERROR_NO_MSG);
    }

    /* Get first block */
    poolPtr = (GT_VOID*)poolId;
    iteratorId = 0;
    memData = prvCpssSlGetNext(poolPtr,&iteratorId);

    if (memData == NULL)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_FAIL, LOG_ERROR_NO_MSG);
    }

    (*allocatedSize) = (*freeSize) = 0;

    while(memData != NULL)
    {
        if(memData->status == FREEPTR)
        {
            (*freeSize) += memData->size;
        }
        else
        {
            (*allocatedSize) += memData->size;
        }
        memData = prvCpssSlGetNext(poolPtr,&iteratorId);
    }

    return GT_OK;
}


/*******************************************************************************
* prvCpssMemDevPoolBlocksStatus
*
* DESCRIPTION:
*       This function return blocks status in the mwmory pool.
*
* INPUTS:
*       poolId  - The poolId to check the status for.
*
* OUTPUTS:
*       numberOfBlocks          - number of allocated blocks.
*       largestConsecutiveBlock - the largest consecutive block.
*       numberOfFreeMemBlocks   - number of free memory blocks.
*
* RETURNS:
*       GT_OK
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS prvCpssMemDevPoolBlocksStatus
(
    IN  GT_UINTPTR poolId,
    OUT GT_U32 *numberOfBlocks,
    OUT GT_U32 *largestConsecutiveBlock,
    OUT GT_U32 *numberOfFreeMemBlocks,
    OUT GT_U32 *freeSize
)
{
    GT_UINTPTR      iteratorId;
    GT_VOID         *poolPtr;
    STRUCT_MEM_DATA *memData;

    if (poolId == 0)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_FAIL, LOG_ERROR_NO_MSG);
    }

    /* Get first block */
    poolPtr = (GT_VOID*)poolId;
    iteratorId = 0;
    memData = prvCpssSlGetNext(poolPtr,&iteratorId);

    if (memData == NULL)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_FAIL, LOG_ERROR_NO_MSG);
    }

    *freeSize = 0;
    *numberOfBlocks = 0;
    *largestConsecutiveBlock = 0;
    *numberOfFreeMemBlocks = 0;

    while(memData != NULL)
    {
        if(memData->status == FREEPTR)
        {
            (*freeSize) += memData->size;
            if ((*largestConsecutiveBlock) < memData->size)
            {
                *largestConsecutiveBlock = memData->size;
            }
            (*numberOfFreeMemBlocks)++;
        }
        else
        {
            (*numberOfBlocks)++;
        }
        memData = prvCpssSlGetNext(poolPtr,&iteratorId);
    }

    return GT_OK;
}

/*******************************************************************************
* prvCpssMemDevPoolForceAllocated
*
* DESCRIPTION:
*       Converts state of a specified free memory block from the pool
*       to "ALLOCATED".
*
* INPUTS:
*       poolId - the Id of the memory pool to allocae from.
*       ptr    - pointer to the memory block to be allocated.
*       size   - the size (in bytes) of the memory pool to be allocated.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK or GT_FAIL (on error).
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS prvCpssMemDevPoolForceAllocated
(
    IN GT_UINTPTR poolId,
    IN GT_UINTPTR ptr,
    IN GT_U32 size
)
{
    STRUCT_MEM_DATA  *memData = NULL; /* work for memo range discriptor PTR */
    GT_UINTPTR       freeIterator ;   /* iterator for nodes */
    GT_VOID          *pSlHead ;       /* list header PTR */
    STRUCT_MEM_DATA  *memFreeLeft ;   /* "left" part of free range */
                                      /*to remain free */
    STRUCT_MEM_DATA  *memAlloc;       /* "middle" part of free range */
                                      /*to become allocated */
    STRUCT_MEM_DATA  *memFreeRight ;  /* "right" part of free range */
                                      /*to remain free */
    GT_VOID          *tmp ;           /* work */
    GT_STATUS        rc ;             /* return code */

    if ((poolId == 0) || (size == 0))
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_FAIL, LOG_ERROR_NO_MSG);
    }

    /* initial iteration state */
    pSlHead = (GT_VOID*)poolId ;
    freeIterator = 0 ;

    while (1)
    {
        /* search for intersectind free block block */
        memData = (STRUCT_MEM_DATA*)prvCpssSlGetNext(pSlHead, &freeIterator) ;
        if (memData == NULL)
        {
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_FAIL, LOG_ERROR_NO_MSG) ;
        }
        if (memData->status != FREEPTR)
            continue ;

        if (memData->ptr > ptr)
            continue ;

        if ((memData->ptr + memData->size) <= ptr)
            continue ;

        break ;
    }

    /* the intersection block found, but not covers all needed space */
    if ((memData->ptr + memData->size) < (ptr + size))
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_FAIL, LOG_ERROR_NO_MSG) ;
    }

    memFreeLeft = NULL ;
    if (memData->ptr != ptr)
    {
        memFreeLeft = mmuNewData(memData->ptr, (GT_U32)(ptr - memData->ptr), FREEPTR) ;
        if (memFreeLeft == NULL)
        {
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_FAIL, LOG_ERROR_NO_MSG) ;
        }
    }

    memAlloc = mmuNewData(ptr, size, ALLOCATED) ;
    if (memAlloc == NULL)
    {
        if (memFreeLeft != NULL)
            cpssOsFree(memFreeLeft) ;
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_FAIL, LOG_ERROR_NO_MSG) ;
    }

    memFreeRight = NULL ;
    if ((memData->ptr + memData->size) > (ptr + size))
    {
        memFreeRight = mmuNewData((ptr + size),
            (GT_U32)((memData->ptr + memData->size) - (ptr + size)),
            FREEPTR) ;
        if (memFreeRight == NULL)
        {
            if (memFreeLeft != NULL)
                cpssOsFree(memFreeLeft) ;
            cpssOsFree(memAlloc) ;
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_FAIL, LOG_ERROR_NO_MSG) ;
        }
    }

    /* drop an old "free memory" node */
    tmp = prvCpssSlDelete(pSlHead, memData) ;
    if (tmp == NULL)
    {
        if (memFreeLeft != NULL)
            cpssOsFree(memFreeLeft) ;
        cpssOsFree(memAlloc) ;
        if (memFreeRight != NULL)
            cpssOsFree(memFreeRight) ;
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_FAIL, LOG_ERROR_NO_MSG) ;
    }
    cpssOsFree(memData) ;

    /* If fail occures (malloc inside the "skipList functions" returns 0)*/
    /* no attempts to correct memory list will be done. */
    /* There is no reason */
    /* that memory list previous state restore may success. */
    /* Need only to free the memory, allocated here */
    /* that haven't been connected to the list */

    /* add new nodes instead */
    if (memFreeLeft != NULL)
    {
        rc = prvCpssSlAdd(pSlHead, memFreeLeft, &freeIterator) ;
        if (rc != GT_OK)
        {
            cpssOsFree(memFreeLeft) ;
            cpssOsFree(memAlloc) ;
            if (memFreeRight != NULL)
                cpssOsFree(memFreeRight) ;
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_FAIL, LOG_ERROR_NO_MSG) ;
        }
    }

    rc = prvCpssSlAdd(pSlHead, memAlloc, &freeIterator) ;
    if (rc != GT_OK)
    {
        cpssOsFree(memAlloc) ;
        if (memFreeRight != NULL)
            cpssOsFree(memFreeRight) ;
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_FAIL, LOG_ERROR_NO_MSG) ;
    }

    if (memFreeRight != NULL)
    {
        rc = prvCpssSlAdd(pSlHead, memFreeRight, &freeIterator) ;
        if (rc != GT_OK)
        {
            cpssOsFree(memAlloc) ;
            cpssOsFree(memFreeRight) ;
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_FAIL, LOG_ERROR_NO_MSG) ;
        }
    }

    return GT_OK ;
}

/*******************************************************************************
* prvCpssMemDevPoolGetSnapshot
*
* DESCRIPTION:
*       This function return memory contigous block infos,
*       sorted by base address.
*
* INPUTS:
*       poolId  - The poolId to check the status for.
*       maxMemoBlocksNum - size of array to get snaphot in blocks
*
* OUTPUTS:
*       memoBlocksNumPtr actual amount of memory blocks
*       blokArarrayPtr   array of memory blocks infos
*
* RETURNS:
*       GT_OK on success
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS prvCpssMemDevPoolGetSnapshot
(
    IN  GT_UINTPTR       poolId,
    IN  GT_U32           maxMemoBlocksNum,
    OUT GT_U32           *memoBlocksNumPtr,
    OUT STRUCT_MEM_DATA  *blokArarrayPtr
)
{
    GT_UINTPTR       freeIterator ;  /* iterator for free memory nodes */
    GT_UINTPTR       usedIterator ;  /* iterator for allocated memory nodes */
    STRUCT_MEM_DATA  *freeMemData;   /* work PTR to free memory rangre */
    STRUCT_MEM_DATA  *usedMemData;   /* work PTR to allocated memory rangre */
    STRUCT_MEM_DATA  *memData;       /* work PTR to memory rangre */
    GT_BOOL          endFree ;       /* no more free memory ranges */
    GT_BOOL          endUsed ;       /* no more allocated memory ranges */
    GT_BOOL          nextFree ;      /* scope the next free memory range */
    GT_BOOL          nextUsed ;      /* scope the next allocated memory range*/
    GT_VOID          *pSlHead ;      /* list header PTR */

    if ((poolId == 0) || (blokArarrayPtr == NULL)
        || (memoBlocksNumPtr == NULL))
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG) ;
    }

    /* to avoid "not initialized variable use" warning */
    freeMemData = NULL ;
    usedMemData = NULL ;
    memData     = NULL ;

    /* initial iteration state */
    pSlHead = (GT_VOID*)poolId ;
    freeIterator = 0 ;
    usedIterator = 0 ;
    endFree = GT_FALSE ;
    endUsed = GT_FALSE ;
    nextFree = GT_TRUE ;
    nextUsed = GT_TRUE ;
    *memoBlocksNumPtr = 0 ;

    while (1)
    {
        /* next free block */
        if (nextFree != GT_FALSE)
        {
            while (1)
            {
                freeMemData = (STRUCT_MEM_DATA*)prvCpssSlGetNext(pSlHead,
                                                                 &freeIterator);
                if (freeMemData == NULL)
                {
                    endFree = GT_TRUE ;
                    break ;
                }
                if (freeMemData->status == FREEPTR)
                    break ;
            }
        }

        /* next used block */
        if (nextUsed != GT_FALSE)
        {
            while (1)
            {
                usedMemData = (STRUCT_MEM_DATA*)prvCpssSlGetNext(pSlHead,
                                                                 &usedIterator);
                if (usedMemData == NULL)
                {
                    endUsed = GT_TRUE ;
                    break ;
                }
                if (usedMemData->status == ALLOCATED)
                    break ;
            }
        }

        /* end of all memo blocks */
        if ((endFree != GT_FALSE) && (endUsed != GT_FALSE))
            break ;

        /* found both types of memo blocks */
        if ((endFree == GT_FALSE) && (endUsed == GT_FALSE))
        {
            if ((GT_UINTPTR)(freeMemData->ptr) <= (GT_UINTPTR)(usedMemData->ptr))
            {
                memData = freeMemData ;
                nextFree = GT_TRUE  ;
                nextUsed = GT_FALSE ;
            }
            else
            {
                memData = usedMemData ;
                nextFree = GT_FALSE  ;
                nextUsed = GT_TRUE ;
            }
        }

        /* found only free memo blocks */
        if ((endFree == GT_FALSE) && (endUsed != GT_FALSE))
        {
            memData = freeMemData ;
            nextFree = GT_TRUE  ;
            nextUsed = GT_FALSE ;
        }

        /* found only used memo blocks */
        if ((endFree != GT_FALSE) && (endUsed == GT_FALSE))
        {
            memData = usedMemData ;
            nextFree = GT_FALSE  ;
            nextUsed = GT_TRUE ;
        }

        blokArarrayPtr[*memoBlocksNumPtr].ptr    = memData->ptr ;
        blokArarrayPtr[*memoBlocksNumPtr].size   = memData->size ;
        blokArarrayPtr[*memoBlocksNumPtr].status =
            (memData->status == FREEPTR) ? 0 : 1 ;

        /* increase amount of fullfilled */
        (*memoBlocksNumPtr) ++ ;

        if (*memoBlocksNumPtr >= maxMemoBlocksNum)
            break ;
    }

    return GT_OK;
}



#ifdef MEMLIB_DEBUG

GT_STATUS printMemNode(STRUCT_MEM_DATA *data)
{
     if ((data == NULL) || ((long)data & 3))
     {
         cpssOsPrintf("MemNode wrong Addr: %08X\n", data);
         return GT_OK;
     }
     cpssOsPrintf("MemNode ptr = 0x%04x, size = 0x%04X, status = %d.\n",
       data->ptr,data->size, data->status);

    return GT_OK;
}

void dumpMemoList(void* cntlPtr)
{
        GT_UINTPTR i = 0;
        void* memNode;

        for (i = 0, memNode = prvCpssSlGetNext(cntlPtr, &i);
            (memNode != NULL);
            memNode = slGetNext(cntlPtr, &i))
        {
            printMemNode((STRUCT_MEM_DATA *)memNode);
        }
        cpssOsPrintf("========================================\n");
}

#endif /* MEMLIB_DEBUG */

