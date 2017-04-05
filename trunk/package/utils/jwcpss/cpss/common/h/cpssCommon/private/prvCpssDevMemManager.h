/*******************************************************************************
*         Copyright 2003, MARVELL SEMICONDUCTOR ISRAEL, LTD.                   *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL.                      *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
*                                                                              *
* MARVELL COMPRISES MARVELL TECHNOLOGY GROUP LTD. (MTGL) AND ITS SUBSIDIARIES, *
* MARVELL INTERNATIONAL LTD. (MIL), MARVELL TECHNOLOGY, INC. (MTI), MARVELL    *
* SEMICONDUCTOR, INC. (MSI), MARVELL ASIA PTE LTD. (MAPL), MARVELL JAPAN K.K.  *
* (MJKK), MARVELL SEMICONDUCTOR ISRAEL. (MSIL),  MARVELL TAIWAN, LTD. AND      *
* SYSKONNECT GMBH.                                                             *
********************************************************************************
* prvCpssDevMemManager.h
*
* DESCRIPTION:
*       Device memory manager.
*
* DEPENDENCIES:
*       None.
*
* FILE REVISION NUMBER:
*       $Revision: 10 $
*******************************************************************************/
#ifndef __prvCpssDevMemManager
#define __prvCpssDevMemManager

#include <cpss/extServices/os/gtOs/gtGenTypes.h>

#define DMM_BLOCK_FREE                      0
#define DMM_BLOCK_ALLOCATED                 1
#define DMM_MIN_ALLOCATE_SIZE_IN_BYTE_CNS   4

/* Illegal values constant to distinguish between an error   
   due to CPU memory allocation or DMM block allocation */
#define DMM_MALLOC_FAIL                     0
extern GT_UINTPTR dmmBlockNotFoundIndication;
#define DMM_BLOCK_NOT_FOUND                dmmBlockNotFoundIndication

#define SIZE_IN_WORDS(handler)      (((GT_DMM_BLOCK*)handler)->word1 & 0x1FFFFFF)
#define SET_SIZE_IN_WORDS(handler, size)    \
            if((size) > 0x1FFFFFF)            \
            {                               \
                CPSS_LOG_ERROR_AND_RETURN_MAC(GT_OUT_OF_RANGE, LOG_ERROR_NO_MSG);     \
            }                               \
            else                            \
            {                               \
                handler->word1 = (handler->word1 & 0xFE000000) | (size);     \
            }

#define DMM_BLOCK_STATUS(handler)   ((((GT_DMM_BLOCK*)handler)->word1>>25) & 0x1)
/* following two macro's instead of old one implemented to avoid warning
  "statement not reached" in DIAB WB 2.6
*/
#define SET_DMM_BLOCK_STATUS_FREE(handler)  \
    ((GT_DMM_BLOCK*)handler)->word1 &= ~(1<<25)

#define SET_DMM_BLOCK_STATUS_ALLOCATED(handler) \
    ((GT_DMM_BLOCK*)handler)->word1 |= (1<<25)

#define OFFSET_IN_WORDS(handler)    (((GT_DMM_BLOCK*)handler)->word2 & 0xFFFFFF)
#define SET_OFFSET_IN_WORDS(handler, offset)    \
            if((offset) > 0xFFFFFF)           \
            {                               \
                CPSS_LOG_ERROR_AND_RETURN_MAC(GT_OUT_OF_RANGE, LOG_ERROR_NO_MSG);     \
            }                               \
            else                            \
            {                               \
                handler->word2 = (handler->word2 & 0xFF000000) | (offset);   \
            }

#define DMM_BLOCK_STAMP(handler)    ((((GT_DMM_BLOCK*)handler)->word2>>24) & 0xFF)
/* validity check removed to avoid warning "statement not reached" in DIAB WB 2.6 and
 * because it's internal macro which is used with valid internal pre-defined constants only
 */
#define SET_DMM_BLOCK_STAMP(handler, stamp) \
    handler->word2 = (handler->word2 & 0xFFFFFF) | ((stamp) << 24)

#define DMM_GET_SIZE(handler)  (SIZE_IN_WORDS(handler) << 2)
#define DMM_GET_OFFSET(handler)  (OFFSET_IN_WORDS(handler) << 2)
#define DMM_GET_PARTITION(handler)  ((GT_UINTPTR)(((GT_DMM_BLOCK*)handler)->nextBySizeOrPartitionPtr.partitionPtr))

/*Note : The number must be multiple of 32*/
#define DMM_MAXIMAL_BLOCK_SIZE_ALLOWED  2112
/*This is DMM_MAXIMAL_BLOCK_SIZE_ALLOWED / 32*/
#define DMM_EMPTINESS_ARRAY_SIZE        ((DMM_MAXIMAL_BLOCK_SIZE_ALLOWED/32)+1)   /*33*/

#define DMM_STAMP_FREE                  0x59
#define DMM_STAMP_USED                  0xC8
#define DMM_STAMP_USED_STATIC           0xC9

struct _gtDmmPartition;
/*
 * Typedef: GT_DMM_BLOCK
 *
 * Description: Device memory block handler.
 *
 * Fields:
 *          partitionPtr - the partiotion pointer that this DMM block was taken
 *                         from.
 *          nextBySize, prevBySize- Pointers to double linked list of all free
 *                                  memory blocks in the same size.
 *          nextByAddr, prevByAdd - Pointers to sorted by address double
 *                                  linked list of all memory blocks.
 *  Following fields packed into fields of 32bit length to save in
 *  memory and to not mess with bit-fields:
 *          word1:
 *              - sizeInWords     - Size of the block in words.
 *              - status          - Status of the block, free or allocated.
 *          word2:
 *              - stamp           - used for check handler validity,
 *                              different for free and allocated blocks
 *              - offsetInWords   - Address of the memory handled
 *
 */
typedef struct dmmBlock
{
    union
    {
        struct _gtDmmPartition *partitionPtr;
        struct  dmmBlock *nextBySize;
    }nextBySizeOrPartitionPtr;
    struct  dmmBlock *prevBySize;
    struct  dmmBlock *nextByAddr, *prevByAddr;
    GT_U32  word1; /* sizeInWords:25(bits 0-24), status:1(bit #25) */
    GT_U32  word2; /* offsetInWords:24(bits 0-23), stamp:8(bits 24-31) */
    /* GT_U32  sizeInWords:25; */
    /* GT_U32  status:1; */
    /* GT_U32  offsetInWords:24; */
    /* GT_U32  stamp:8; */
}GT_DMM_BLOCK;

/*
 * Typedef: GT_DMM_PARTITION
 *
 * Description: Device memory block handler.
 *
 * Fields:
 *          tableOfSizePointers - Array of pointers to linked lists of all free
 *                                  memory blocks in the same size.
 *          bigBlocksList       - Pointer to linked lists of all free
 *                                  memory blocks in size bigger that.
 *          emptinessBitArray   - bit array of DMM_MAXIMAL_BLOCK_SIZE_ALLOWED
 *                                  bits, used for fast search of non empty
 *                                  indexes in tableOfSizePointers.
 *          minBlockSize      - Minimal size can be allocated from the partition
 *          maxBlockSize      - Maximal size can be allocated from the partition
 *          pointedBlock      - block in the partition - for statistics only.
 *                              always point on a single block in the partion
 *                              needed for statistics when no free block in the
 *                              partition, the tableOfSizePointers and the
 *                              bigBlocksList points to no blocks.
 *
 */
typedef struct _gtDmmPartition
{
    GT_DMM_BLOCK    *tableOfSizePointers[DMM_MAXIMAL_BLOCK_SIZE_ALLOWED+1];
    GT_DMM_BLOCK    *bigBlocksList;
    GT_U32          emptinessBitArray[DMM_EMPTINESS_ARRAY_SIZE];
    GT_U32          minBlockSizeInWords;
    GT_U32          maxBlockSizeInWords;
    GT_U32          pickAllocatedBytes;
    GT_U32          allocatedBytes;
    GT_U32          pickBlocks;
    GT_U32          currentBlocks;
    GT_DMM_BLOCK    *pointedBlock;
}GT_DMM_PARTITION;

/*******************************************************************************
*  prvCpssDmmCreatePartition
*
* DESCRIPTION:
*      Create new device memory partition
*
* INPUTS:
*       sizeOfPartition - Ammount of memory added.
*       startAddress    - the offset that memory block will be added with.
*       minBlockSize - Minimal block size can be allocated from this partition.
*       maxBlockSize - Maximal block size can be allocated from this partition.
*
* OUTPUTS:
*       partitionId - ID of the created partition.
*
* RETURNS:
*       GT_OK   - on success
*       GT_BAD_PARAM - on invalid parameters
*       GT_NO_RESOURCE - No memory is available to create the partition.
*
* COMMENTS:
*   None.
*
*******************************************************************************/
GT_STATUS prvCpssDmmCreatePartition
(
    IN GT_U32   sizeOfPartition,
    IN GT_U32   startAddress,
    IN GT_U32   minBlockSize,
    IN GT_U32   maxBlockSize,
    OUT GT_UINTPTR  *partitionId
);

/*******************************************************************************
*  prvCpssDmmAllocate
*
* DESCRIPTION:
*      Allocate new device memory block to existing partition.
*
* INPUTS:
*       partitionId     - ID of the partition, created by dmmCreatePartition().
*       size            - Ammount of memory requested
*       alignment       - Alignment of memory requested
*                           ( Only word alingment supported )
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       handler - handler/id of the allocated adress in device memory.
*                   Offset and size of the block can be accessed by macros :
*                   DMM_GET_SIZE(handler)
*                   DMM_GET_OFFSET(handler)
*
* COMMENTS:
*   If handler == 0, the operation failed.
*
*******************************************************************************/
GT_UINTPTR prvCpssDmmAllocate
(
    IN GT_UINTPTR   partitionId,
    IN GT_U32       size,
    IN GT_U32       alignment
);

/*******************************************************************************
*  prvCpssDmmStaticAllocate
*
* DESCRIPTION:
*      Allocate new device static (not to be freed) memory block to existing
*      partition.
*
* INPUTS:
*       partitionId     - ID of the partition, created by dmmCreatePartition().
*       size            - Ammount of memory requested
*       alignment       - Alignment of memory requested
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       handler - handler/id of the allocated adress in device memory.
*                   Offset and size of the block can be accessed by macros :
*                   DMM_GET_SIZE(handler)
*                   DMM_GET_OFFSET(handler)
*
* COMMENTS:
*   If handler == 0, the operation failed.
*
*******************************************************************************/
GT_UINTPTR prvCpssDmmStaticAllocate
(
    IN GT_UINTPTR   partitionId,
    IN GT_U32       size,
    IN GT_U32       alignment
);

/*******************************************************************************
*  prvCpssDmmAllocateByPtr
*
* DESCRIPTION:
*      Allocate new device memory block to existing partition by an already
*      given HW ptr.
*
* INPUTS:
*       partitionId     - ID of the partition, created by dmmCreatePartition().
*       hwOffset        - The given hw offset.
*       size            - Ammount of memory requested
*       alignment       - Alignment of memory requested
*
* OUTPUTS:
*       hwOffsetHandlePtr - handler of the allocated adress in device memory.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong blockSize
*       GT_OUT_OF_RANGE          - on value out of range
*       GT_OUT_OF_CPU_MEM        - if failed to allocate CPU memory, or
*
* COMMENTS:
*       Offset and size of the block can be accessed by macros :
*       DMM_GET_SIZE(handler)
*       DMM_GET_OFFSET(handler)
*
*******************************************************************************/
GT_STATUS prvCpssDmmAllocateByPtr
(
    IN  GT_UINTPTR   partitionId,
    IN  GT_U32       hwOffset,
    IN  GT_U32       size,
    IN  GT_U32       alignment,
    OUT GT_UINTPTR   *hwOffsetHandlePtr
);

/*******************************************************************************
*  prvCpssDmmFree
*
* DESCRIPTION:
*      Free device memory block to existing partition.
*
* INPUTS:
*       partitionId - ID of the partition, created by dmmCreatePartition().
*       handler     - handler/id of the allocated by dmmAllocate() adress.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       size - size of the block freed if the status is GT_OK.
*
* COMMENTS:
*   If size == 0, the operation failed.
*
*******************************************************************************/
GT_U32 prvCpssDmmFree
(
    IN GT_UINTPTR   handler
);

/*******************************************************************************
*  prvCpssDmmClosePartition
*
* DESCRIPTION:
*      Close device memory partition
*
* INPUTS:
*       partitionId - ID of the created partition.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK   - on success
*       GT_BAD_PARAM - on invalid parameters
*
* COMMENTS:
*   None.
*
*******************************************************************************/
GT_STATUS prvCpssDmmClosePartition
(
    IN GT_UINTPTR  partitionId
);


/*******************************************************************************
*  prvCpssDmmStatistic
*
* DESCRIPTION:
*      Get the partition statistic
*
* INPUTS:
*       partitionId - ID of the created partition.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on fail.
*       GT_NO_RESOURCE - No memory is available to create the memory map or
*                        allocated table.
*
* COMMENTS:
*   None.
*
*******************************************************************************/
GT_STATUS prvCpssDmmStatistic
(
    IN GT_UINTPTR  partitionId,
    OUT GT_U32 *numOfUsedBlocks,
    OUT GT_U32 *numOfAllocatedBytes,
    OUT GT_U32 *numOfFreeBytes,
    OUT GT_U32 *numOfPickAllocatedBytes,
    OUT GT_U32 *numOfPickAllBlocks
);

/*******************************************************************************
*  prvCpssDmmPartitionPrint
*
* DESCRIPTION:
*      Print partition info
*
* INPUTS:
*       partitionId - ID of the created partition.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on fail.
*       GT_NO_RESOURCE - No memory is available to create the memory map or
*                        allocated table.
*
* COMMENTS:
*   None.
*
*******************************************************************************/
GT_STATUS prvCpssDmmPartitionPrint
(
    IN GT_UINTPTR  partitionId
);


#endif /*__prvCpssDevMemManager*/

