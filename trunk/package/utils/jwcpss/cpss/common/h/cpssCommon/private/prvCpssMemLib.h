/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* prvCpssMemLib.h
*
* DESCRIPTION:
*       Implementation of a memory management unit, for managing all PP's inte-
*       rnal memory for the use of tables and internal structures manipulation
*       functions.
*
*       This implementation provides the following functions for the user:
*
*       -   prvCpssMemDevPoolCreate(void): Creates a new memory pool and returns
*           it's Id, for further allocations / deallocations.
*
*       -   prvCpssMemDevPoolAdd(poolId,memPtr,memSize): Adds a block to a
*           previously created memory pool.
*
*       -   prvCpssMemDevPoolMalloc(poolId,size,bitAllign): Allocates a block of
*           memory from the given pool. The allocated block is alligned by the
*           desired bit allignment.
*
*       -   prvCpssMemDevPoolFree(poolId,ptr): Frees a previously allocated
*           memory block, returns the number of freed bytes.
*
*       -   prvCpssMemDevPoolStatus(poolId,*allocatedSize,*freeSize): This is a
*           stat. function, which calculates the total free / allocated bytes
*           remaining in / allocated from the given pool.
*
*       In order that the user will be able to use the above functionality, the
*       file "prvCpssMemLib.h" must be included.
*
* DEPENDENCIES:
*       None.
*
* FILE REVISION NUMBER:
*       $Revision: 2 $
*
*******************************************************************************/

#ifndef __prvCpssMemLibh
#define __prvCpssMemLibh

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include <cpss/extServices/os/gtOs/gtGenTypes.h>


typedef struct memData
{
    GT_U32  status;         /* FREEPTR==0 or ALLOCATED==1 (first search key)       */
    GT_UINTPTR  ptr;            /* pointer to the memory area (second search key)*/
    GT_U32  size;           /* the size of the memory area (in bytes)        */
}STRUCT_MEM_DATA;



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
GT_UINTPTR prvCpssMemDevPoolCreate(void);



/*******************************************************************************
* prvCpssMemDevPoolAdd
*
* DESCRIPTION:
*       Adds a new memory block to the given pool.
*
* INPUTS:
*       poolId - The Id of the memory pool to add to.
*       memPtr - a pointer to memory pool to be added.
*       memSize- the size (in bytes) of the memory pool to be added.
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
);


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
);

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
);

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
);

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
);

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
);



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
    IN GT_U32 *allocatedSize,
    OUT GT_U32 *freeSize
);
/*******************************************************************************
* prvCpssMemDevPoolMove
*
* DESCRIPTION:
*       This function moves a block from ptr1 to ptr2.
*
* INPUTS:
*       poolId  - The poolId to check the status for.
*       ptr1 -    pointer to the src address
*       ptr2 -    pointer to the dst address
*
* OUTPUTS:
*        None.
*
* RETURNS:
*       GT_OK
* COMMENTS:
*       This function will not allocate new buffers and assumes both buffers
*       exist.It will only be possible to move to buffer in ptr2
*       if it has enough place or it will
*       overlap buffer in ptr1.
*
*******************************************************************************/
GT_U32 prvCpssMemDevPoolMove
(
    IN GT_UINTPTR poolId,
    IN GT_U32 ptr1,  /*src*/
    IN GT_U32 ptr2  /*dst*/
);

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
);

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
*       GT_OK or GT_FAIL.
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
) ;

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
) ;

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __prvCpssMemLibh */

