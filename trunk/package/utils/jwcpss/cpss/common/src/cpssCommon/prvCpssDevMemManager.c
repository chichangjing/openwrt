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
* prvCpssDevMemManager.c
*
* DESCRIPTION:
*       Device memory manager.
*
* DEPENDENCIES:
*       None.
*
* FILE REVISION NUMBER:
*       $Revision: 15 $
*******************************************************************************/

#include <cpss/extServices/private/prvCpssBindFunc.h>
#include <cpssCommon/private/prvCpssDevMemManager.h>
#include <cpssCommon/cpssBuffManagerPool.h>
#include <cpssCommon/cpssPresteraDefs.h>

#ifdef CPSS_EXMX_IP_LPM_DEBUG_MEM

extern void * cpssOsIpMalloc
(
    IN GT_U32 size
);

extern void cpssOsIpFree
(
    IN void* const memblock
);
#else
#define cpssOsIpMalloc cpssOsMalloc
#define cpssOsIpFree cpssOsFree
#endif

#undef DMM_DEBUG_ENABLE

#define DMM_SET_EPTINESS(partition, index) \
                { partition->emptinessBitArray[index>>5] |=(1<<(index & 0x1F));}

#define DMM_CLEAN_EPTINESS(partition, index) \
              {partition->emptinessBitArray[index>>5] &= ~(1<<(index & 0x1F));}

#define DMM_EXIST_EXACT_FIT(partition, size) \
                   ((partition->tableOfSizePointers[size] != NULL)?\
                            (GT_TRUE):(GT_FALSE))

static GT_U32 maskArray[32] =
   {0x1,0x2,0x4,0x8,0x10,
    0x20,0x40,0x80,0x100,0x200,0x400,0x800,
    0x1000,0x2000,0x4000,0x8000,0x10000,0x20000,
    0x40000,0x80000,0x100000,0x200000,0x400000,
    0x800000,0x1000000,0x2000000,0x4000000,0x8000000,
    0x10000000,0x20000000,0x40000000,0x80000000};


static GT_U32 revMaskArray[32] =
   {0xFFFFFFFF ,0xFFFFFFFE ,0xFFFFFFFC ,0xFFFFFFF8 ,
    0xFFFFFFF0 ,0xFFFFFFE0 ,0xFFFFFFC0 ,0xFFFFFF80 ,
    0xFFFFFF00 ,0xFFFFFE00 ,0xFFFFFC00 ,0xFFFFF800 ,
    0xFFFFF000 ,0xFFFFE000 ,0xFFFFC000 ,0xFFFF8000 ,
    0xFFFF0000 ,0xFFFE0000 ,0xFFFC0000 ,0xFFF80000 ,
    0xFFF00000 ,0xFFE00000 ,0xFFC00000 ,0xFF800000 ,
    0xFF000000 ,0xFE000000 ,0xFC000000 ,0xF8000000 ,
    0xF0000000 ,0xE0000000 ,0xC0000000 ,0x80000000};



static GT_U32 dmmNextNotEmptyAfter
(
    GT_DMM_PARTITION    *partition,
    GT_U32              size
)
{
    GT_U32  theWord,theBit,retVal=0;

    /*Finish current word*/
    theWord = size >> 5;
    theBit = size & 0x1F;
    /*If the rest of the word are zero - skip the next while*/
    if( (partition->emptinessBitArray[theWord] & revMaskArray[theBit] ) !=0 )
    {
        while(GT_TRUE)
        {
            if(theBit == 32)
            {
                break;
            }

            if( (partition->emptinessBitArray[theWord] & maskArray[theBit]) ==0)
            {
                theBit++;
                continue;
            }
            else
            {
                return (theWord << 5) | theBit;
            }
        }
    }

    theWord++;

    /*Find next non zero word*/
    while(GT_TRUE)
    {
        if(theWord == DMM_EMPTINESS_ARRAY_SIZE)
            return 0;

        if( partition->emptinessBitArray[theWord] != 0 )
        {
            break;
        }
        else
        {
            theWord++;
        }
    }

    /*Find in non zero word*/
    theBit = 0;
    while(GT_TRUE)
    {
        if( (partition->emptinessBitArray[theWord] & maskArray[theBit]) == 0)
        {
            theBit++;
            continue;
        }
        else
        {
            retVal = (theWord << 5) | theBit;
            break;
        }
    }

    return retVal;
}


static GT_U32 removeFromSizedDll
(
    GT_DMM_PARTITION    *partition,
    GT_DMM_BLOCK        *node
)
{
    if((node->nextBySizeOrPartitionPtr.nextBySize != NULL) && (node->prevBySize != NULL) )
    { /*Middle of the dll*/
        node->prevBySize->nextBySizeOrPartitionPtr.nextBySize =
            node->nextBySizeOrPartitionPtr.nextBySize;
        node->nextBySizeOrPartitionPtr.nextBySize->prevBySize = node->prevBySize;
        node->prevBySize = node->nextBySizeOrPartitionPtr.nextBySize = NULL;
    }

    else if((node->nextBySizeOrPartitionPtr.nextBySize == NULL) && (node->prevBySize != NULL))
    { /*End of the dll*/
        node->prevBySize->nextBySizeOrPartitionPtr.nextBySize = NULL;
        node->prevBySize = NULL;
    }

    else if((node->nextBySizeOrPartitionPtr.nextBySize != NULL) && (node->prevBySize == NULL))
    { /*Head of the dll*/
        if(SIZE_IN_WORDS(node) <= DMM_MAXIMAL_BLOCK_SIZE_ALLOWED)
        {
            partition->tableOfSizePointers[SIZE_IN_WORDS(node)] =
                node->nextBySizeOrPartitionPtr.nextBySize;
        }
        else
        {
            partition->bigBlocksList = node->nextBySizeOrPartitionPtr.nextBySize;
        }
        node->nextBySizeOrPartitionPtr.nextBySize->prevBySize = NULL;
        node->nextBySizeOrPartitionPtr.nextBySize = NULL;
    }

    else if((node->nextBySizeOrPartitionPtr.nextBySize == NULL) && (node->prevBySize == NULL))
    { /*Single node in the dll*/

        if(SIZE_IN_WORDS(node) <= DMM_MAXIMAL_BLOCK_SIZE_ALLOWED)
        {
            partition->tableOfSizePointers[SIZE_IN_WORDS(node)] = NULL;
            DMM_CLEAN_EPTINESS(partition,SIZE_IN_WORDS(node));
        }
        else
        {
            partition->bigBlocksList = NULL;
        }
    }

    return 0;
}

static GT_U32 addToSizedDll
(
    GT_DMM_PARTITION    *partition,
    GT_DMM_BLOCK        *node
)
{
    if(SIZE_IN_WORDS(node) <= DMM_MAXIMAL_BLOCK_SIZE_ALLOWED)
    {
        node->nextBySizeOrPartitionPtr.nextBySize = partition->tableOfSizePointers[SIZE_IN_WORDS(node)];

        if(partition->tableOfSizePointers[SIZE_IN_WORDS(node)] != NULL)
        {
            partition->tableOfSizePointers[SIZE_IN_WORDS(node)]->prevBySize =node;
        }
        else
        {
            DMM_SET_EPTINESS(partition,SIZE_IN_WORDS(node));
        }

        partition->tableOfSizePointers[SIZE_IN_WORDS(node)] = node;
        node->prevBySize = NULL;
    }
    else
    {
        node->nextBySizeOrPartitionPtr.nextBySize = partition->bigBlocksList;
        if(partition->bigBlocksList != NULL)
            partition->bigBlocksList->prevBySize = node;
        partition->bigBlocksList = node;
        node->prevBySize = NULL;
    }

    return 0;
}

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
)
{
    GT_DMM_PARTITION    *partition;
    GT_DMM_BLOCK        *newBlock;

    if( ( (sizeOfPartition & 0x3) != 0 ) ||
        ( (startAddress & 0x3) != 0 ) || ( sizeOfPartition == 0 ) ||
        ( (minBlockSize & 0x3) != 0 ) || ( minBlockSize == 0 ) ||
        ( (maxBlockSize >> 2) > DMM_MAXIMAL_BLOCK_SIZE_ALLOWED ))
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    partition = (GT_DMM_PARTITION*)cpssOsIpMalloc(sizeof(GT_DMM_PARTITION));
    if (partition == NULL)
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NO_RESOURCE, LOG_ERROR_NO_MSG);

    /*Arrays must be 0*/
    cpssOsMemSet(partition->tableOfSizePointers,0,
             sizeof(partition->tableOfSizePointers));
    cpssOsMemSet(partition->emptinessBitArray,0,
             sizeof(partition->emptinessBitArray));

    /*No big blocks*/
    partition->bigBlocksList = NULL;

    partition->minBlockSizeInWords = minBlockSize >> 2;
    partition->maxBlockSizeInWords = maxBlockSize >> 2;

    sizeOfPartition = sizeOfPartition >> 2;

    /* Add new block to database */
    newBlock = (GT_DMM_BLOCK*)cpssOsIpMalloc(sizeof (GT_DMM_BLOCK));
    if(newBlock == NULL)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_FAIL, LOG_ERROR_NO_MSG);
    }
    partition->currentBlocks = partition->pickBlocks = 1;
    partition->pickAllocatedBytes = partition->allocatedBytes = 0;

    SET_OFFSET_IN_WORDS(newBlock, (startAddress >> 2));
    SET_DMM_BLOCK_STATUS_FREE(newBlock);
    SET_DMM_BLOCK_STAMP(newBlock, DMM_STAMP_FREE);
    SET_SIZE_IN_WORDS(newBlock, sizeOfPartition);

    /*Dlls with one node*/
    newBlock->prevBySize = newBlock->nextBySizeOrPartitionPtr.nextBySize = NULL;
    newBlock->nextByAddr = newBlock->prevByAddr = NULL;

    addToSizedDll(partition,newBlock);

    *partitionId = (GT_UINTPTR)partition;

    /* pointedBlock points on the only block in the partition */
    partition->pointedBlock = newBlock;

    return GT_OK;
}

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
)
{
    GT_DMM_BLOCK        *allocatedBlock = NULL;
    GT_DMM_BLOCK        *foundBlock = NULL;
    GT_DMM_PARTITION    *partition = (GT_DMM_PARTITION *)partitionId;
    GT_U32              blockSize = size >> 2;

    alignment = alignment; /* warning fix */

    foundBlock = partition->bigBlocksList;

    while ((foundBlock != NULL) && (SIZE_IN_WORDS(foundBlock) < blockSize))
    {
        foundBlock = foundBlock->nextBySizeOrPartitionPtr.nextBySize;
    }

    if (foundBlock == NULL)
    {
        return 0;
    }

    /*Update addressed dll*/
    if ( NULL ==
         ( allocatedBlock = (GT_DMM_BLOCK*)cpssOsIpMalloc(sizeof(GT_DMM_BLOCK))))
    {
        return (0);
    }

    partition->currentBlocks++;
    if (  partition->currentBlocks > partition->pickBlocks )
    {
        partition->pickBlocks = partition->currentBlocks;
    }

    /*Update addressed dll*/
    if (foundBlock->prevByAddr != NULL)
        foundBlock->prevByAddr->nextByAddr = allocatedBlock;
    allocatedBlock->nextByAddr = foundBlock;
    allocatedBlock->prevByAddr = foundBlock->prevByAddr;
    foundBlock->prevByAddr = allocatedBlock;

    SET_OFFSET_IN_WORDS(allocatedBlock, OFFSET_IN_WORDS(foundBlock));
    SET_SIZE_IN_WORDS(allocatedBlock, blockSize);
    SET_DMM_BLOCK_STATUS_ALLOCATED(allocatedBlock);
    SET_DMM_BLOCK_STAMP(allocatedBlock, DMM_STAMP_USED_STATIC);
    allocatedBlock->prevBySize = NULL;

    SET_OFFSET_IN_WORDS(foundBlock, (OFFSET_IN_WORDS(foundBlock)+blockSize));

    /*Remove from old sized dll*/
    removeFromSizedDll(partition,foundBlock);

    SET_SIZE_IN_WORDS(foundBlock, (SIZE_IN_WORDS(foundBlock)-blockSize));

    /*Add to new sized dll*/
    addToSizedDll(partition,foundBlock);


    partition->allocatedBytes += blockSize << 2;
    if (partition->allocatedBytes > partition->pickAllocatedBytes )
        partition->pickAllocatedBytes = partition->allocatedBytes;

    /* pointedBlock points on the allocated block */
    partition->pointedBlock = allocatedBlock;

    allocatedBlock->nextBySizeOrPartitionPtr.partitionPtr = partition;

    return (GT_UINTPTR)allocatedBlock;
}

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
)
{
    GT_DMM_BLOCK        *allocatedBlock = NULL;
    GT_DMM_BLOCK        *foundBlock = NULL;
    GT_DMM_PARTITION    *partition = (GT_DMM_PARTITION *)partitionId;
    GT_U32              blockSize = size >> 2;
    GT_U32              foundBlockSize = 0, tempSize;

    alignment = alignment; /* warning fix */

    if( (blockSize > partition->maxBlockSizeInWords) ||
        (blockSize < partition->minBlockSizeInWords) /*||
        (alignment != 2 )*/ )
    {
        return DMM_BLOCK_NOT_FOUND;
    }

    if(DMM_EXIST_EXACT_FIT(partition,blockSize))
    {
        allocatedBlock = partition->tableOfSizePointers[blockSize];

        removeFromSizedDll(partition,allocatedBlock);

        /*Change status of the allocated block*/
        SET_DMM_BLOCK_STATUS_ALLOCATED(allocatedBlock);
        SET_DMM_BLOCK_STAMP(allocatedBlock, DMM_STAMP_USED);

        partition->allocatedBytes += blockSize << 2;
        if(partition->allocatedBytes > partition->pickAllocatedBytes )
            partition->pickAllocatedBytes = partition->allocatedBytes;

        /* pointedBlock points on the allocated block */
        partition->pointedBlock = allocatedBlock;

        /*  - store partition id in the allocated DMM block.*/
        allocatedBlock->nextBySizeOrPartitionPtr.partitionPtr = partition;

    }
    else
    {
        /*Look for block*/
        tempSize = blockSize + partition->minBlockSizeInWords;
        if( tempSize <= DMM_MAXIMAL_BLOCK_SIZE_ALLOWED )
        {
            if(DMM_EXIST_EXACT_FIT(partition,tempSize))
            {
                foundBlockSize = tempSize;
            }
        }

        if( foundBlockSize == 0)
        {
            tempSize = blockSize << 1;
            if( tempSize <= DMM_MAXIMAL_BLOCK_SIZE_ALLOWED )
            {
                if(DMM_EXIST_EXACT_FIT(partition,tempSize))
                {
                    foundBlockSize = tempSize;
                }
            }
        }

        if( foundBlockSize == 0)
        {
            if( 0 == (foundBlockSize = dmmNextNotEmptyAfter(partition,
                            blockSize + partition->minBlockSizeInWords - 1) ))
            {
                if( partition->bigBlocksList == NULL )
                {
                    foundBlockSize = dmmNextNotEmptyAfter(partition,blockSize);
                }
            }
        }

        if( (foundBlockSize == 0) && (partition->bigBlocksList ==  NULL))
        {   /*If sized block not found and list of big blocks empry -
                out of memory */
            return DMM_BLOCK_NOT_FOUND;
        }

        foundBlock = ( foundBlockSize == 0 )?(partition->bigBlocksList):
                            (partition->tableOfSizePointers[foundBlockSize]);

        /*Update addressed dll*/
        if( NULL ==
            ( allocatedBlock = (GT_DMM_BLOCK*)cpssOsIpMalloc(sizeof(GT_DMM_BLOCK))))
        {
            return DMM_MALLOC_FAIL;
        }

        partition->currentBlocks++;
        if(  partition->currentBlocks > partition->pickBlocks )
        {
            partition->pickBlocks = partition->currentBlocks;
        }

        /*Update addressed dll*/
        if(foundBlock->prevByAddr != NULL)
            foundBlock->prevByAddr->nextByAddr = allocatedBlock;
        allocatedBlock->nextByAddr = foundBlock;
        allocatedBlock->prevByAddr = foundBlock->prevByAddr;
        foundBlock->prevByAddr = allocatedBlock;

        SET_OFFSET_IN_WORDS(allocatedBlock, OFFSET_IN_WORDS(foundBlock));
        SET_SIZE_IN_WORDS(allocatedBlock, blockSize);
        SET_DMM_BLOCK_STATUS_ALLOCATED(allocatedBlock);
        SET_DMM_BLOCK_STAMP(allocatedBlock, DMM_STAMP_USED);

        allocatedBlock->nextBySizeOrPartitionPtr.nextBySize = allocatedBlock->prevBySize = NULL;

        SET_OFFSET_IN_WORDS(foundBlock, (OFFSET_IN_WORDS(foundBlock)+blockSize));

        /*Update sized dll's*/
        if( foundBlockSize != 0 )
        { /*Small block to smal block*/

            /*Remove from old sized dll*/
            removeFromSizedDll(partition,foundBlock);

            SET_SIZE_IN_WORDS(foundBlock, (SIZE_IN_WORDS(foundBlock)-blockSize));

            /*Add to new sized dll*/
            addToSizedDll(partition,foundBlock);
        }
        else if ( (SIZE_IN_WORDS(foundBlock)-blockSize) >
                  DMM_MAXIMAL_BLOCK_SIZE_ALLOWED)
        { /* Big block to big block*/

            /*Just update the size*/
            SET_SIZE_IN_WORDS(foundBlock, (SIZE_IN_WORDS(foundBlock)-blockSize));
        }
        else
        { /* Big block to small block*/

            /*Remove from big blocks dll ( remove the head of the dll)*/
            removeFromSizedDll(partition, partition->bigBlocksList);

            SET_SIZE_IN_WORDS(foundBlock, (SIZE_IN_WORDS(foundBlock)-blockSize));

            /*Add to new sized dll*/
            addToSizedDll(partition,foundBlock);
        }

        partition->allocatedBytes += blockSize << 2;
        if(partition->allocatedBytes > partition->pickAllocatedBytes )
            partition->pickAllocatedBytes = partition->allocatedBytes;

        /* pointedBlock points on the allocated block */
        partition->pointedBlock = allocatedBlock;

        /*  - store partition id in the allocated DMM block.*/
        allocatedBlock->nextBySizeOrPartitionPtr.partitionPtr = partition;

    }

    return (GT_UINTPTR)allocatedBlock;
}


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
)
{
    GT_DMM_BLOCK        *allocatedBlock = NULL;
    GT_DMM_BLOCK        *freeBlock = NULL;
    GT_DMM_BLOCK        *foundBlock = NULL ;
    GT_DMM_PARTITION    *partition = (GT_DMM_PARTITION *)partitionId;
    GT_U32              blockSize = size >> 2;
    GT_U32              foundBlockSize = 0,  givenHwOffsetInWords;
    GT_BOOL             gotoNext = GT_TRUE;

    alignment = alignment; /* warning fix */

    if( (blockSize > partition->maxBlockSizeInWords) ||
        (blockSize < partition->minBlockSizeInWords) /*||
        (alignment != 2 )*/ )
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    givenHwOffsetInWords = hwOffset >> 2;
    foundBlock = partition->pointedBlock;

    /* check which direction to search */
    if (givenHwOffsetInWords < OFFSET_IN_WORDS(foundBlock) )
    {
        gotoNext = GT_FALSE;
    }

    /* start by locating the block which includes this address in two directions
       search fashion */
    while ((foundBlock != NULL) &&
           ((givenHwOffsetInWords < OFFSET_IN_WORDS(foundBlock) ) ||
           (givenHwOffsetInWords >= (OFFSET_IN_WORDS(foundBlock) + SIZE_IN_WORDS(foundBlock)))))
    {
        foundBlock = (gotoNext == GT_TRUE)? foundBlock->nextByAddr:
            foundBlock->prevByAddr;
    }

    /* check if we reached the end of the memory , or this is an already
       allocated space, or the found block is smaller then the needed memory*/
    if ((foundBlock == NULL) || (DMM_BLOCK_STATUS(foundBlock) == DMM_BLOCK_ALLOCATED) ||
        (SIZE_IN_WORDS(foundBlock) < blockSize))
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NO_RESOURCE, LOG_ERROR_NO_MSG);
    }


    /* now treat each one of the spilts */

    /* first case , the wanted offset is the same as the start of this block */
    if (OFFSET_IN_WORDS(foundBlock) == givenHwOffsetInWords)
    {
        /* first check if we have and exact match in size for a block */
        if (SIZE_IN_WORDS(foundBlock) == blockSize)
        {
            allocatedBlock = foundBlock;

            removeFromSizedDll(partition,allocatedBlock);

            /*Change status of the allocated block*/
            SET_DMM_BLOCK_STATUS_ALLOCATED(allocatedBlock);
            SET_DMM_BLOCK_STAMP(allocatedBlock, DMM_STAMP_USED);

            partition->allocatedBytes += blockSize << 2;
            if (partition->allocatedBytes > partition->pickAllocatedBytes )
                partition->pickAllocatedBytes = partition->allocatedBytes;

            /* pointedBlock points on the allocated block */
            partition->pointedBlock = allocatedBlock;

            allocatedBlock->nextBySizeOrPartitionPtr.partitionPtr = partition;
            *hwOffsetHandlePtr = (GT_UINTPTR)allocatedBlock;
            return GT_OK;
        }
        else
        {
            /* we need to split in the middle of the block , the end will be a
               new free block */

            if ( NULL ==
                 ( allocatedBlock = (GT_DMM_BLOCK*)cpssOsIpMalloc(sizeof(GT_DMM_BLOCK))))
            {
                CPSS_LOG_ERROR_AND_RETURN_MAC(GT_OUT_OF_CPU_MEM, LOG_ERROR_NO_MSG);
            }

            partition->currentBlocks++;

            allocatedBlock->nextByAddr = foundBlock;
            allocatedBlock->prevByAddr = foundBlock->prevByAddr;

            if (foundBlock->prevByAddr != NULL)
                foundBlock->prevByAddr->nextByAddr = allocatedBlock;
            foundBlock->prevByAddr = allocatedBlock;

            SET_OFFSET_IN_WORDS(allocatedBlock, OFFSET_IN_WORDS(foundBlock));
            SET_SIZE_IN_WORDS(allocatedBlock, blockSize);
            SET_DMM_BLOCK_STATUS_ALLOCATED(allocatedBlock);
            SET_DMM_BLOCK_STAMP(allocatedBlock, DMM_STAMP_USED);
            allocatedBlock->nextBySizeOrPartitionPtr.nextBySize = allocatedBlock->prevBySize = NULL;

            SET_OFFSET_IN_WORDS(foundBlock, (OFFSET_IN_WORDS(foundBlock)+blockSize));

            /* calculate the new second free block size */
            foundBlockSize = SIZE_IN_WORDS(foundBlock) - blockSize;
        }
    }
    else
    {
        /* the wanted offset is in the middle of the block */
        if (OFFSET_IN_WORDS(foundBlock) + SIZE_IN_WORDS(foundBlock) ==
            givenHwOffsetInWords + blockSize)
        {
            /* the found block ends in the same place the found block ends */
            /* we need to split in middle of the block and the start block will
               be a new free block */

            if ( NULL ==
                 ( allocatedBlock = (GT_DMM_BLOCK*)cpssOsIpMalloc(sizeof(GT_DMM_BLOCK))))
            {
                CPSS_LOG_ERROR_AND_RETURN_MAC(GT_OUT_OF_CPU_MEM, LOG_ERROR_NO_MSG);
            }

            partition->currentBlocks++;

            allocatedBlock->nextByAddr = foundBlock->nextByAddr;
            allocatedBlock->prevByAddr = foundBlock;

            if (foundBlock->nextByAddr != NULL)
                foundBlock->nextByAddr->prevByAddr = allocatedBlock;
            foundBlock->nextByAddr = allocatedBlock;

            SET_OFFSET_IN_WORDS(allocatedBlock, givenHwOffsetInWords);
            SET_SIZE_IN_WORDS(allocatedBlock, blockSize);
            SET_DMM_BLOCK_STATUS_ALLOCATED(allocatedBlock);
            SET_DMM_BLOCK_STAMP(allocatedBlock, DMM_STAMP_USED);
            allocatedBlock->nextBySizeOrPartitionPtr.nextBySize = allocatedBlock->prevBySize = NULL;

            /* calculate the new first free block size */
            foundBlockSize = SIZE_IN_WORDS(foundBlock) - blockSize;
        }
        else
        {
            /* the needed block falls in the middle of the found block, we need
               to split the found block to two free blocks and a new allocated
               block */

            if ( NULL ==
                 ( allocatedBlock = (GT_DMM_BLOCK*)cpssOsIpMalloc(sizeof(GT_DMM_BLOCK))))
            {
                CPSS_LOG_ERROR_AND_RETURN_MAC(GT_OUT_OF_CPU_MEM, LOG_ERROR_NO_MSG);
            }

            if ( NULL ==
                 ( freeBlock = (GT_DMM_BLOCK*)cpssOsIpMalloc(sizeof(GT_DMM_BLOCK))))
            {
                cpssOsIpFree(allocatedBlock);
                CPSS_LOG_ERROR_AND_RETURN_MAC(GT_OUT_OF_CPU_MEM, LOG_ERROR_NO_MSG);
            }

            partition->currentBlocks += 2;

            allocatedBlock->nextByAddr = freeBlock;
            allocatedBlock->prevByAddr = foundBlock;

            freeBlock->nextByAddr = foundBlock->nextByAddr;
            freeBlock->prevByAddr = allocatedBlock;

            if (foundBlock->nextByAddr != NULL)
                foundBlock->nextByAddr->prevByAddr = freeBlock;
            foundBlock->nextByAddr = allocatedBlock;

            /* calculate the new first free block size */

            foundBlockSize = givenHwOffsetInWords - OFFSET_IN_WORDS(foundBlock);

            SET_OFFSET_IN_WORDS(allocatedBlock, givenHwOffsetInWords);
            SET_SIZE_IN_WORDS(allocatedBlock, blockSize);
            SET_DMM_BLOCK_STATUS_ALLOCATED(allocatedBlock);
            SET_DMM_BLOCK_STAMP(allocatedBlock, DMM_STAMP_USED);
            allocatedBlock->nextBySizeOrPartitionPtr.nextBySize = allocatedBlock->prevBySize = NULL;

            SET_OFFSET_IN_WORDS(freeBlock, givenHwOffsetInWords + blockSize);
            SET_SIZE_IN_WORDS(freeBlock,
                              SIZE_IN_WORDS(foundBlock) - blockSize - foundBlockSize);
            SET_DMM_BLOCK_STATUS_FREE(freeBlock);
            SET_DMM_BLOCK_STAMP(freeBlock, DMM_STAMP_FREE);
            freeBlock->nextBySizeOrPartitionPtr.nextBySize = allocatedBlock->prevBySize = NULL;

            /*Add the new free to sized dll*/
            addToSizedDll(partition,freeBlock);
        }
    }

    /* update the foundblock in the DLL */

    /*Remove from old sized dll*/
    removeFromSizedDll(partition,foundBlock);

    /* update it's size */
    SET_SIZE_IN_WORDS(foundBlock, foundBlockSize);

    /*Add to new sized dll*/
    addToSizedDll(partition,foundBlock);



    partition->allocatedBytes += blockSize << 2;
    if (partition->allocatedBytes > partition->pickAllocatedBytes )
        partition->pickAllocatedBytes = partition->allocatedBytes;

    /* pointedBlock points on the allocated block */
    partition->pointedBlock = allocatedBlock;

    allocatedBlock->nextBySizeOrPartitionPtr.partitionPtr = partition;

    *hwOffsetHandlePtr = (GT_UINTPTR)allocatedBlock;
    return GT_OK;
}

/*******************************************************************************
*  prvCpssDmmFree
*
* DESCRIPTION:
*      Free device memory block to existing partition.
*
* INPUTS:
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
)
{
    GT_DMM_PARTITION    *partition;
    GT_DMM_BLOCK        *freedBlock, *unusedBlock;
    GT_U32              freedSize;
    GT_U8               leftRigthJoin = 0;

    freedBlock = (GT_DMM_BLOCK *)handler;
    freedSize = SIZE_IN_WORDS(freedBlock) << 2;

    partition = freedBlock->nextBySizeOrPartitionPtr.partitionPtr ;

    if( (handler == 0)  || (partition == 0) ||
        (partition == (GT_DMM_PARTITION*)((GT_UINTPTR)0xFFFFFFFE)) ||
        (DMM_BLOCK_STATUS(freedBlock) != DMM_BLOCK_ALLOCATED) ||
        (DMM_BLOCK_STAMP(freedBlock) != DMM_STAMP_USED) )
    {
        return 0;
    }

    SET_DMM_BLOCK_STATUS_FREE(freedBlock);
    SET_DMM_BLOCK_STAMP(freedBlock, DMM_STAMP_FREE);

    /*Try to join from rigth */
    if( (freedBlock->nextByAddr != NULL) &&
        (DMM_BLOCK_STATUS(freedBlock->nextByAddr) == DMM_BLOCK_FREE) &&
        ((OFFSET_IN_WORDS(freedBlock) + SIZE_IN_WORDS(freedBlock)) ==
         OFFSET_IN_WORDS(freedBlock->nextByAddr)) )
    {
        /*remove joined block from sized dll */
        removeFromSizedDll(partition,freedBlock->nextByAddr);

        SET_SIZE_IN_WORDS(freedBlock,
            SIZE_IN_WORDS(freedBlock) + SIZE_IN_WORDS(freedBlock->nextByAddr));

        leftRigthJoin++;
    }

    /*Try to join from left */
    if( (freedBlock->prevByAddr != NULL) &&
        (DMM_BLOCK_STATUS(freedBlock->prevByAddr) == DMM_BLOCK_FREE) &&
        ((OFFSET_IN_WORDS(freedBlock->prevByAddr) +
          SIZE_IN_WORDS(freedBlock->prevByAddr)) == OFFSET_IN_WORDS(freedBlock)) )
    {
        /*remove joined block from sized dll */
        removeFromSizedDll(partition, freedBlock->prevByAddr);

        SET_SIZE_IN_WORDS(freedBlock->prevByAddr,
            SIZE_IN_WORDS(freedBlock->prevByAddr) + SIZE_IN_WORDS(freedBlock));

        freedBlock = freedBlock->prevByAddr;
        leftRigthJoin++;
    }

    /*Arrange sized DLL*/
    if(leftRigthJoin == 1)
    {
        unusedBlock = freedBlock->nextByAddr;
        if(freedBlock->nextByAddr->nextByAddr != NULL)
            freedBlock->nextByAddr->nextByAddr->prevByAddr = freedBlock;
        freedBlock->nextByAddr = freedBlock->nextByAddr->nextByAddr;

        cpssOsIpFree(unusedBlock);

        partition->currentBlocks--;
    }
    else if (leftRigthJoin == 2)
    {
        unusedBlock = freedBlock->nextByAddr;

        if(freedBlock->nextByAddr->nextByAddr->nextByAddr != NULL)
            freedBlock->nextByAddr->nextByAddr->nextByAddr->prevByAddr =
                freedBlock;
        freedBlock->nextByAddr = freedBlock->nextByAddr->nextByAddr->nextByAddr;

        cpssOsIpFree(unusedBlock->nextByAddr);

        partition->currentBlocks--;

        cpssOsIpFree(unusedBlock);

        partition->currentBlocks--;
    }

    /*Add to new sized dll*/
    addToSizedDll(partition,freedBlock);


    /* pointedBlock points on free block */
    partition->pointedBlock = freedBlock;

    partition->allocatedBytes -= freedSize;
    return freedSize;
}

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
)
{
    GT_DMM_PARTITION *partition = (GT_DMM_PARTITION*)partitionId;

    if(partition->pointedBlock)
    {
        /* was allocated in
            newBlock = (GT_DMM_BLOCK*)cpssOsIpMalloc(sizeof (GT_DMM_BLOCK)); */
        cpssOsIpFree(partition->pointedBlock);
    }

    cpssOsIpFree(partition);

    return GT_OK;
}

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
    IN GT_UINTPTR partitionId,
    OUT GT_U32 *numOfUsedBlocks,
    OUT GT_U32 *numOfAllocatedBytes,
    OUT GT_U32 *numOfFreeBytes,
    OUT GT_U32 *numOfPickAllocatedBytes,
    OUT GT_U32 *numOfPickAllBlocks
)
{
    GT_DMM_PARTITION    *partition = (GT_DMM_PARTITION *)partitionId;
    GT_DMM_BLOCK        *tempBlock;
    GT_U32              numOfAllocatedBlocks = 0;
    GT_U32              numOfFreeBlocks = 0;
    GT_U32              freeMem = 0, allocMem = 0;
    static GT_U32       sizes[DMM_MAXIMAL_BLOCK_SIZE_ALLOWED];
    GT_U32              addrCheck = 0;

    cpssOsMemSet(sizes,0,DMM_MAXIMAL_BLOCK_SIZE_ALLOWED*4);

    /* scan the partition block - start from pointedBlock */


    /*Go left from pointed block */
    tempBlock = partition->pointedBlock;
    addrCheck = OFFSET_IN_WORDS(tempBlock);
    do
    {
        if (addrCheck != OFFSET_IN_WORDS(tempBlock))
        {
            cpssOsPrintf("inconsistency found \n");
        }

        if(DMM_BLOCK_STATUS(tempBlock) == DMM_BLOCK_FREE )
        {
            numOfFreeBlocks++;
            freeMem += (SIZE_IN_WORDS(tempBlock)<<2);
            if(SIZE_IN_WORDS(tempBlock) < DMM_MAXIMAL_BLOCK_SIZE_ALLOWED )
            {
                sizes[SIZE_IN_WORDS(tempBlock)]++;
            }
        }
        else
        {
            numOfAllocatedBlocks++;
            allocMem += (SIZE_IN_WORDS(tempBlock)<<2);
        }

        tempBlock = tempBlock->prevByAddr;
        if (tempBlock != NULL)
        {
            addrCheck -= SIZE_IN_WORDS(tempBlock);
        }
    }while(tempBlock != NULL);

    /*Go rigth from pointed block*/
    tempBlock = partition->pointedBlock->nextByAddr;
    if (tempBlock != NULL)
    {
        addrCheck = OFFSET_IN_WORDS(tempBlock);
    }
    while(tempBlock != NULL)
    {
        if (addrCheck != OFFSET_IN_WORDS(tempBlock))
        {
            cpssOsPrintf("inconsistency found \n");
        }
        if(DMM_BLOCK_STATUS(tempBlock) == DMM_BLOCK_FREE )
        {
            numOfFreeBlocks++;
            freeMem += (SIZE_IN_WORDS(tempBlock)<<2);
            if(SIZE_IN_WORDS(tempBlock) < DMM_MAXIMAL_BLOCK_SIZE_ALLOWED )
            {
                sizes[SIZE_IN_WORDS(tempBlock)]++;
            }
        }
        else
        {
            numOfAllocatedBlocks++;
            allocMem += (SIZE_IN_WORDS(tempBlock)<<2);
        }

        addrCheck += SIZE_IN_WORDS(tempBlock);
        tempBlock = tempBlock->nextByAddr;
    }

    cpssOsPrintf("\n\n");

    *numOfUsedBlocks     = numOfAllocatedBlocks;
    *numOfAllocatedBytes = allocMem;
    *numOfFreeBytes      = freeMem;

    *numOfPickAllocatedBytes = partition->pickAllocatedBytes;
    *numOfPickAllBlocks      = partition->pickBlocks;

    /*
    for( i = 0; i < DMM_MAXIMAL_BLOCK_SIZE_ALLOWED; i++)
    {
        if( sizes[i] == 0 )
            continue;
        osPrintf("\n In Size %d \t %d",i,sizes[i]);

    }
    osPrintf("\n\n Allocated blocks : %d \n Free blocks : %d \
             \n Allocated memory : 0x%X\n Free memory      : 0x%X\n All memory : 0x%X",
             numOfAllocatedBlocks, numOfFreeBlocks, freeMem, allocMem, (freeMem+allocMem));
*/
    return GT_OK;
}


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
)
{
    GT_DMM_PARTITION    *partition = (GT_DMM_PARTITION *)partitionId;
    GT_DMM_BLOCK        *tempBlock;
    GT_U32              numOfAllocatedBlocks = 0;
    GT_U32              numOfFreeBlocks = 0;
    GT_U32              freeMem = 0, allocMem = 0;
    static GT_U32       sizes[DMM_MAXIMAL_BLOCK_SIZE_ALLOWED];
    GT_U32              addrCheck = 0;
    GT_U32              i = 0;
    GT_DMM_BLOCK *tempPtr;

    cpssOsMemSet(sizes,0,DMM_MAXIMAL_BLOCK_SIZE_ALLOWED*4);
    cpssOsPrintf("\n\n\nPrint partition 0X%p memory statistic:\n\n",partitionId);
    /* print free list for this partition */
    cpssOsPrintf(" \nfree lists for small blocks till %d bytes:\n",
                 DMM_MAXIMAL_BLOCK_SIZE_ALLOWED*4);
    for (i= 0; i <= DMM_MAXIMAL_BLOCK_SIZE_ALLOWED; i++)
    {
        if (partition->tableOfSizePointers[i] == NULL)
        {
            continue;
        }
        tempPtr =  partition->tableOfSizePointers[i];
        cpssOsPrintf("%d :",i*4);
        while(tempPtr != NULL)
        {
            cpssOsPrintf("%x ->",OFFSET_IN_WORDS(tempPtr));
            tempPtr = tempPtr->nextBySizeOrPartitionPtr.nextBySize;
        }
        cpssOsPrintf("EOSL\n");
        cpssOsPrintf("----------------------------------------------------\n");
    }
    /* check big free blocks */
    if (partition->bigBlocksList != NULL)
    {
        if(DMM_BLOCK_STATUS(partition->bigBlocksList) != DMM_BLOCK_FREE )
        {
            cpssOsPrintf("!!!!!!!! not free big block\n");
        }
         tempPtr = partition->bigBlocksList;
         cpssOsPrintf("free list for big block\n");
         cpssOsPrintf("%d:",SIZE_IN_WORDS(tempPtr));

         while(tempPtr != NULL)
         {
             cpssOsPrintf("%x ->",OFFSET_IN_WORDS(tempPtr));
             tempPtr = tempPtr->nextBySizeOrPartitionPtr.nextBySize;
         }
         cpssOsPrintf("EOBL\n");
         cpssOsPrintf("-----------------------------------------------------------------\n");

    }

    /* scan the partition block - start from pointedBlock */

    /*Go left from pointed block */
    tempBlock = partition->pointedBlock;
    addrCheck = OFFSET_IN_WORDS(tempBlock);
    while(tempBlock->nextByAddr != NULL)
        tempBlock = tempBlock->nextByAddr;
    addrCheck = OFFSET_IN_WORDS(tempBlock);
    do
    {
        if (addrCheck != OFFSET_IN_WORDS(tempBlock))
        {
            cpssOsPrintf("inconsistency found \n");
        }

        if(DMM_BLOCK_STATUS(tempBlock) == DMM_BLOCK_FREE )
        {
            numOfFreeBlocks++;
            freeMem += (SIZE_IN_WORDS(tempBlock));
            if(SIZE_IN_WORDS(tempBlock) < DMM_MAXIMAL_BLOCK_SIZE_ALLOWED )
            {
                sizes[SIZE_IN_WORDS(tempBlock)]++;
            }
            cpssOsPrintf("Free Block:      addr = 0x%X, size = %d\n",
                         OFFSET_IN_WORDS(tempBlock),
                         SIZE_IN_WORDS(tempBlock));
        }
        else
        {
            numOfAllocatedBlocks++;
            allocMem += (SIZE_IN_WORDS(tempBlock));
            cpssOsPrintf("Allocated Block: addr = 0x%X, size = %d\n",
                         OFFSET_IN_WORDS(tempBlock),
                         SIZE_IN_WORDS(tempBlock));
        }

        tempBlock = tempBlock->prevByAddr;
        if (tempBlock != NULL)
        {
            addrCheck -= SIZE_IN_WORDS(tempBlock);
        }
    }while(tempBlock != NULL);

    /*Go rigth from pointed block*/
   /* tempBlock = partition->pointedBlock->nextByAddr;
    if (tempBlock != NULL)
    {
        addrCheck = OFFSET_IN_WORDS(tempBlock);
    }
    while(tempBlock != NULL)
    {
        if (addrCheck != OFFSET_IN_WORDS(tempBlock))
        {
            cpssOsPrintf("inconsistency found \n");
        }
        if(DMM_BLOCK_STATUS(tempBlock) == DMM_BLOCK_FREE )
        {
            numOfFreeBlocks++;
            freeMem += (SIZE_IN_WORDS(tempBlock)<<2);
            if(SIZE_IN_WORDS(tempBlock) < DMM_MAXIMAL_BLOCK_SIZE_ALLOWED )
            {
                sizes[SIZE_IN_WORDS(tempBlock)]++;
            }
            cpssOsPrintf("Free Block:      addr = 0x%X, size = %d\n",
                         OFFSET_IN_WORDS(tempBlock),
                         SIZE_IN_WORDS(tempBlock)<<2);
        }
        else
        {
            numOfAllocatedBlocks++;
            allocMem += (SIZE_IN_WORDS(tempBlock)<<2);
            cpssOsPrintf("Allocated Block: addr = 0x%X, size = %d\n",
                         OFFSET_IN_WORDS(tempBlock),
                         SIZE_IN_WORDS(tempBlock)<<2);
        }

        addrCheck += SIZE_IN_WORDS(tempBlock);
        tempBlock = tempBlock->nextByAddr;
    }*/

    cpssOsPrintf("\n\n Allocated blocks : %d \n Free blocks : %d \
             \n Allocated memory : 0x%X\n Free memory      : 0x%X\n All memory : 0x%X",
             numOfAllocatedBlocks, numOfFreeBlocks, freeMem, allocMem, (freeMem+allocMem));
    return GT_OK;
}




#ifdef  DMM_DEBUG_ENABLE
GT_STATUS dmmDebugStress
(
    GT_U32  sizeOfPartition,
    GT_U32  minBlock,
    GT_U32  maxBlock,
    GT_U8   strategy
)
{
    GT_UINTPTR pId;
    GT_UINTPTR allocatedTable[1024], i, j, stam, mm;
    GT_U32 loopId;
    GT_U32 allocatedBlocks = 1;
    GT_DMM_BLOCK *block;
    GT_U32       *memoryMap;
    GT_U32          existAllocBlocks,existBlocks,prevExistBlocks,existFreeBlocks;

    if( GT_OK != prvCpssDmmCreatePartition(sizeOfPartition,0,minBlock,maxBlock,
                                           &pId))
      return 1;

    memoryMap = cpssOsIpMalloc(sizeOfPartition);
    if (memoryMap == NULL)
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NO_RESOURCE, LOG_ERROR_NO_MSG);
    cpssOsMemSet(memoryMap,0,sizeOfPartition);

    for(loopId = 0; loopId < 20; loopId++)
    {
        for ( i = 0; i < 1024; i++ )
        {
            allocatedTable[i] = prvCpssDmmAllocate(pId,((osRand()%900)+3)*4 , 2);
            if((allocatedTable[i]==DMM_BLOCK_NOT_FOUND)||(allocatedTable[i]==DMM_MALLOC_FAIL))
            {
                cpssOsPrintf("Alloc Fail");
                return 26;
            }


            allocatedBlocks++;
            /*fill all allocated area*/
            for( mm = DMM_GET_OFFSET(allocatedTable[i]) / 4;
                 mm < DMM_GET_OFFSET(allocatedTable[i]) / 4 + DMM_GET_SIZE(allocatedTable[i]) / 4;
                 mm += 4)
            {
                if( memoryMap[mm] != 0 )
                {
                    cpssOsPrintf("PP mem corruption !!!");
                    return 13;
                }

                memoryMap[mm] = DMM_GET_OFFSET(allocatedTable[i]);
            }

            existBlocks = 0;
            block = (GT_DMM_BLOCK*)allocatedTable[i];

            do
            {
                existBlocks++;
                block = block->nextByAddr;
            }while( block != NULL);

            block = (GT_DMM_BLOCK*)allocatedTable[i];
            existBlocks--;
            do
            {
                existBlocks++;
                block = block->prevByAddr;
            }while( block != NULL);
            stam++;

            if((osRand()%2) == 0)
            {
                j = osRand()%(i+1);

                /*Clean and Compare Free area*/
                for( mm = DMM_GET_OFFSET(allocatedTable[j]) / 4;
                     mm < DMM_GET_OFFSET(allocatedTable[j]) / 4+
                                DMM_GET_SIZE(allocatedTable[j]) / 4;
                     mm += 4)
                {
                    if( memoryMap[mm] != DMM_GET_OFFSET(allocatedTable[j]) )
                    {
                        cpssOsPrintf("PP mem corruption !!!");
                        return 13;
                    }

                    memoryMap[mm] = 0;
                }


                if( 0 == prvCpssDmmFree(allocatedTable[j]))
                {
                    cpssOsPrintf("Free Fail");
                    return 26;
                }

                allocatedBlocks--;

            if( (loopId == 0) && ( i == 550 ) )
                stam++;

                allocatedTable[j] = prvCpssDmmAllocate(pId,((osRand()%900)+3)*4,2);
                if((allocatedTable[i]==DMM_BLOCK_NOT_FOUND)||(allocatedTable[i]==DMM_MALLOC_FAIL))
                {
                    cpssOsPrintf("Alloc Fail");
                    return 26;
                }

                allocatedBlocks++;
                /*fill all allocated area*/
                for( mm = DMM_GET_OFFSET(allocatedTable[j]) / 4;
                     mm < DMM_GET_OFFSET(allocatedTable[j]) / 4 + DMM_GET_SIZE(allocatedTable[j]) / 4;
                     mm += 4)
                {
                    if( memoryMap[mm] != 0 )
                    {
                        cpssOsPrintf("PP mem corruption !!!");
                        return 13;
                    }

                    memoryMap[mm] = DMM_GET_OFFSET(allocatedTable[j]);
                }

                existBlocks = 0;
                block = (GT_DMM_BLOCK*)allocatedTable[i];
                do
                {
                    existBlocks++;
                    block = block->nextByAddr;
                }while( block != NULL);

                    block = (GT_DMM_BLOCK*)allocatedTable[i];
                existBlocks--;
                do
                {
                    existBlocks++;
                    block = block->prevByAddr;
                }while( block != NULL);
                stam++;
            }
        }
        i--;

        for ( ; ; i-- )
        {
            /*Clean and Compare Free area*/
            for( mm = DMM_GET_OFFSET(allocatedTable[i]) / 4;
                 mm < DMM_GET_OFFSET(allocatedTable[i]) / 4 + DMM_GET_SIZE(allocatedTable[i]) / 4;
                 mm += 4)
            {
                if( memoryMap[mm] != DMM_GET_OFFSET(allocatedTable[i]) )
                {
                    cpssOsPrintf("PP mem corruption !!!");
                    return 13;
                }

                memoryMap[mm] = 0;
            }

            if( 0 == prvCpssDmmFree(allocatedTable[i]))
            {
                cpssOsPrintf("Free Fail");
                return 26;
            }
            allocatedBlocks--;

                existBlocks = existAllocBlocks = existFreeBlocks = 0;
                block = (GT_DMM_BLOCK*)allocatedTable[i];
                do
                {
                    existBlocks++;
                    if(DMM_BLOCK_STATUS(block) == DMM_BLOCK_FREE)
                    {
                        existFreeBlocks++;
                    }
                    else
                    {
                        existAllocBlocks++;
                    }
                    block = block->nextByAddr;
                }while( block != NULL);

                block = (GT_DMM_BLOCK*)allocatedTable[i];
                existBlocks--;
                if(DMM_BLOCK_STATUS(block) == DMM_BLOCK_FREE)
                {
                    existFreeBlocks--;
                }
                else
                {
                    existAllocBlocks--;
                }
                do
                {
                    existBlocks++;
                    if(DMM_BLOCK_STATUS(block) == DMM_BLOCK_FREE)
                    {
                        existFreeBlocks++;
                    }
                    else
                    {
                        existAllocBlocks++;
                    }
                    block = block->prevByAddr;
                }while( block != NULL);

                if( prevExistBlocks - existBlocks == 2 )
                    stam++;

                prevExistBlocks = existBlocks;

            if(i == 0 )
            {
                /*dmmClosePartition(pId);*/
                break;
            }
        }
    }

    prvCpssDmmClosePartition(pId);
    return 0;
}

GT_STATUS dmmOldDebugStress
(
    GT_U32  sizeOfPartition,
    GT_U32  minBlock,
    GT_U32  maxBlock,
    GT_U8   strategy
)
{
    GT_UINTPTR pId;
    GT_UINTPTR *allocatedTable;
    GT_U32 i, j;
    GT_U32 loopId;
    GT_U32       *memoryMap;

    allocatedTable = cpssOsIpMalloc(sizeof(*allocatedTable)*1000*1024);
    if (allocatedTable == NULL)
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NO_RESOURCE, LOG_ERROR_NO_MSG);
    pId = prvCpssMemDevPoolCreate();
    if( GT_OK != prvCpssMemDevPoolAdd(pId,4,sizeOfPartition ))
      return 1;

    memoryMap = cpssOsIpMalloc(sizeOfPartition);
    if (memoryMap == NULL)
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NO_RESOURCE, LOG_ERROR_NO_MSG);
    cpssOsMemSet(memoryMap,0,sizeOfPartition);

    for(loopId = 0; loopId < 1; loopId++)
    {
        for ( i = 0; i < 1000*1024; i++ )
        {
            if( 0 == (allocatedTable[i] =
                      prvCpssMemDevPoolMalloc(pId,((osRand()%13)+3)*4 , 2)))
            {
                cpssOsPrintf("Alloc Fail");
                return 26;
            }

            if((osRand()%2) == 0)
            {
                j = osRand()%(i+1);

                if( 0 == prvCpssMemDevPoolFree(pId,allocatedTable[j]))
                {
                    cpssOsPrintf("Free Fail");
                    return 26;
                }

                if( 0 == (allocatedTable[j] =
                          prvCpssMemDevPoolMalloc(pId,((osRand()%13)+3)*4,2)))
                {
                    cpssOsPrintf("Alloc Fail");
                    return 26;
                }
            }
        }
        i--;

        for ( ; ; i-- )
        {
            if( 0 == prvCpssMemDevPoolFree(pId,allocatedTable[i]))
            {
                cpssOsPrintf("Free Fail");
                return 26;
            }

            if(i == 0 )
            {
                /*dmmClosePartition(pId); */
                break;
            }
        }
    }

    cpssOsIpFree(allocatedTable);
    cpssOsIpFree(memoryMap);
    return 0;
}

GT_STATUS dmmNewDebugStress
(
    GT_U32  sizeOfPartition,
    GT_U32  minBlock,
    GT_U32  maxBlock,
    GT_U8   strategy
)
{
    GT_UINTPTR pId;
    GT_UINTPTR *allocatedTable;
    GT_U32 i, j, stam;
    GT_U32 loopId;
    GT_U32 *memoryMap;

    allocatedTable = cpssOsIpMalloc(sizeof(*allocatedTable)*1000*1024);
    if (allocatedTable == NULL)
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NO_RESOURCE, LOG_ERROR_NO_MSG);
    if( GT_OK != prvCpssDmmCreatePartition(sizeOfPartition,0,minBlock,maxBlock,
                                           &pId))
      return 1;

    memoryMap = cpssOsIpMalloc(sizeOfPartition);
    if (memoryMap == NULL)
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NO_RESOURCE, LOG_ERROR_NO_MSG);
    cpssOsMemSet(memoryMap,0,sizeOfPartition);

    for(loopId = 0; loopId < 1; loopId++)
    {
        for ( i = 0; i < 1000*1024; i++ )
        {
            allocatedTable[i] = prvCpssDmmAllocate(pId,((osRand()%13)+3)*4 , 2);
            if((allocatedTable[i]==DMM_BLOCK_NOT_FOUND)||(allocatedTable[i]==DMM_MALLOC_FAIL))            
            {
                cpssOsPrintf("Alloc Fail");
                return 26;
            }

            if((osRand()%2) == 0)
            {
                j = osRand()%(i+1);

                if( 0 == prvCpssDmmFree(allocatedTable[j]))
                {
                    cpssOsPrintf("Free Fail");
                    return 26;
                }

                if( i == 537334 )
                    stam++;

                allocatedTable[j] = prvCpssDmmAllocate(pId,((osRand()%13)+3)*4,2);
                if((allocatedTable[i]==DMM_BLOCK_NOT_FOUND)||(allocatedTable[i]==DMM_MALLOC_FAIL))
                {
                    cpssOsPrintf("Alloc Fail");
                    return 26;
                }
            }
        }
        i--;

        for ( ; ; i-- )
        {
            if( 0 == prvCpssDmmFree(allocatedTable[i]))
            {
                cpssOsPrintf("Free Fail");
                return 26;
            }
            if(i == 0 )
            {
                /*prvCpssDmmClosePartition(pId); */
                break;
            }
        }
    }

    prvCpssDmmClosePartition(pId);
    cpssOsIpFree(allocatedTable);
    cpssOsIpFree(memoryMap);
    return 0;
}

#endif /*DMM_DEBUG_ENABLE*/
