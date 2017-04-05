/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* cliMemLib.c
*
* DESCRIPTION:
*       cliMemPool implementation
*
* DEPENDENCIES:
*       None
*
* FILE REVISION NUMBER:
*       $Revision: 1 $
*******************************************************************************/
/***** Includes ********************************************************/
#ifdef VXWORKS
#include <vxWorks.h>
#endif
#include <stdlib.h>
#include <string.h>
#include <gtOs/gtOsGen.h>
#include <gtOs/gtOsSem.h>
#include <gtOs/gtOsIo.h>
#include <cmdShell/lua/cliMemLib.h>

/************* Defines ********************************************************/

#if __WORDSIZE == 64
#define OSMEM_DEFAULT_BLOCK_STEP_SIZE 8
#else
#define OSMEM_DEFAULT_BLOCK_STEP_SIZE 4
#endif

#define OSMEM_DEFAULT_STEPS_LIMIT     128
#define OSMEM_DEFAULT_POOLS_LIMIT     65536

/* Obsolete will be removed in the next version */
/* #define OSMEM_DEFAULT_MEM_INIT_SIZE   2048*1024 */

#define OSMEM_MAX_MEMORY_SIZE_USE 64*1024*1024 /* 64MB */

/* don't disable debug if UTs included in the build! */
#define OSMEMPOOLS_DEBUG
#undef MUTEX_PROTECTION_ENABLED

/*
 * Typedef: struct OSMEM_POOL_FREE_ELEM
 *
 * Description:
 *      This structure represent a node in the pool's free list.
 *
 * Fields:
 *      next_elem     - next node in the memory pool's list.
 *      size          - the size of the memory block (used only when the various
 *                      sizes pool).
 *
 */

typedef struct os_mem_pool_free_elem{
    struct os_mem_pool_free_elem    *next_elem;
    GT_U32                          size;
}OSMEM_POOL_FREE_ELEM,*OSMEM_POOL_FREE_ELEM_PTR;


/*
 * Typedef: struct OSMEM_POOL_ELEM
 *
 * Description:
 *      This structure represent a node followed by a memory block in the memory
 *      linked list managed by the memory pools.
 *
 * Fields:
 *        size -     real required for dynamic allocation size
 *                for example if the required size is 150 bytes but the allocated pool 
 *                length is 256 bytes this  field will be set to 150
 * 
 */

typedef struct os_mem_pool_elem{
    GT_U32                      size;
#ifdef OS_MEM_64_BIT_ALIGNED
    GT_U32                      padding;
#endif
}OSMEM_POOL_ELEM,*OSMEM_POOL_ELEM_PTR;

/*
 * Typedef: struct OSMEM_POOL_HDR
 *
 * Description:
 *      This structure holds the header of a list of memory managed pool for
 *      the osMalloc mechanism.
 *
 * Fields:
 *      poolIndex          - the index in the pool array of this pool.
 *      elem_size          - the size of elements this pool manage (if 0 see
 *                           cliMemPoolsInit).
 *      free_size          - the size of free space in this pool
 *      size               - the size of the memory pool (allocated + free).
 *                           was allocated.
 *      alloc_times        - the amount of times alloc was called to allocated
 *                           memory from this pool.
 *      free_times         - the amount of times free was called to free memory
 *                           from this pool.
 *      malloc_used        - the amount of times this pool was needed to use
 *                           malloc.
 *      chunkIndex         - the index of the chunk from which the next element
 *                           should be allocated from.
 *      first_free_PTR     - pointer to the first free element.
 *
 */

typedef struct {
    GT_U32                      poolIndex;
    GT_U32                      elem_size;
#ifdef OSMEMPOOLS_DEBUG
    GT_U32                      free_size;
    GT_U32                      overAlloc;
    GT_U32                      alloc_times;
    GT_U32                      free_times;
    GT_U32                      malloc_used;
    GT_U32                      size;
#endif

    GT_U32                      chunkIndex;

    struct os_mem_pool_free_elem    *first_free_PTR;
      
}OSMEM_POOL_HDR,*OSMEM_POOL_HDR_PTR;


/*
 * Typedef: struct OSMEM_CHUNK_POINTER
 *
 * Description:
 *      This structure represent a chunk of memory allocated for the memory
 *      manager.
 *
 * Fields:
 *      startAddr   - a pointer to the beginning of the chunk.
 *      dynamicAddr - a pointer to the end of the dynamic memory allocated from
 *                    this chunk.
 *      freeSize    - the current free size in the chunk.
 *
 */

typedef struct os_mem_chunk_pointer{
    GT_CHAR*                    startAddr;
    GT_CHAR*                    dynamicAddr;
    GT_U32                      freeSize;

}OSMEM_CHUNK_POINTER,*OSMEM_CHUNK_POINTER_PTR;

/*
 * Typedef: struct OSMEM_CHUNK_ARRAY
 *
 * Description:
 *      This structure represent the array of chunks for the memory mangaer use
 *
 * Fields:
 *      chunkArray     - the array holding the pointer to the chunks.
 *      chunkSize      - the size of a chunk (one size for all).
 *      lastChunkIndex - a pointer to the beginning of the last allocated chunk.
 *      chunkArraySize - the size of the chunkArray
 *      memAllocFuncPtr - the function to be used in memory allocation.
 *      memAllocMethod - determines the method of the allocation 
 *
 */

typedef struct os_mem_chunk_array{
    OSMEM_CHUNK_POINTER_PTR         chunkArray;
    GT_U32                          chunkSize;
    GT_U32                          lastChunkIndex;
    GT_U32                          chunkArraySize;
    CLI_MEMORY_ALLOC_FUNC            memAllocFuncPtr;
    CLI_MEMORY_FREE_FUNC             memFreeFuncPtr;
    CLI_MEMORY_MANAGE_METHOD_ENT     memAllocMethod;

}OSMEM_CHUNK_ARRAY,*OSMEM_CHUNK_ARRAY_PTR;



/***** Function Declaration ********************************************/
static GT_STATUS cliMemChunkAllocate(GT_VOID);

static GT_STATUS cliMemPoolsInit(GT_U32 blockSizeStep, GT_U32 stepsLimit,
                                GT_U32 poolsLimit);
static OSMEM_POOL_HDR *cliMemPoolCreate(GT_U32 elemSize,GT_U32 poolIndex);
static GT_VOID* cliMemPoolAllocSize(OSMEM_POOL_HDR_PTR poolHdrPtr,GT_U32 size);
static GT_VOID *cliMemAllocDynamicFromChunk(GT_U32 size,GT_U32 *chunkIndex);

/***** External Functions **********************************************/
extern GT_STATUS mathLog2(IN  GT_U32  num,OUT GT_U32  *result);

/***** Global Vars ********************************************/

static OSMEM_POOL_HDR_PTR* os_mem_pools_array = NULL;
static GT_U32 os_mem_pools_array_size;
#ifdef MUTEX_PROTECTION_ENABLED
static GT_MUTEX os_mem_pools_mtx_id;
#endif
static GT_U32 *os_mem_pools_size2index;
static OSMEM_CHUNK_ARRAY os_mem_chunks_array;
static GT_U32 os_mem_poolsLimit;

/* heap allocation measurement static variables */
static GT_U32 heap_bytes_allocated = 0;


#undef OSMEMPOOLS_JOURNAL

#ifdef OSMEMPOOLS_JOURNAL
#define LOGSIZE 300
struct {
    int op;
    void* ptr;
    int size;
} logbuf[LOGSIZE];
static int logptr = 0;
#define OSMEMPOOLS_JOURNAL_OP(i) logbuf[logptr].op = (i)
#define OSMEMPOOLS_JOURNAL_PTR(p) logbuf[logptr].ptr = (p)
#define OSMEMPOOLS_JOURNAL_SIZE(s) logbuf[logptr].size = (s)
#define OSMEMPOOLS_JOURNAL_NEWREC() log_newrec()
static const char* log_ops[] = {
    "none  ",
    "malloc",
    "free  "
};
static void log_newrec(void)
{
    logptr++;
    if (logptr >= LOGSIZE)
        logptr = 0;
    OSMEMPOOLS_JOURNAL_OP(0);
    OSMEMPOOLS_JOURNAL_PTR(NULL);
    OSMEMPOOLS_JOURNAL_SIZE(0);
}

void cliMemLibPrintLog(void)
{
    int cnt = 0;
    int i = logptr;
    for (cnt = 0; cnt <LOGSIZE; cnt++,i++)
    {
        if (i >= LOGSIZE)
            i = 0;
        printf("%s %p %d\n",log_ops[logbuf[i].op],
            logbuf[i].ptr,logbuf[i].size); 
    }
}
#else
#define OSMEMPOOLS_JOURNAL_OP(i) 
#define OSMEMPOOLS_JOURNAL_PTR(p) 
#define OSMEMPOOLS_JOURNAL_SIZE(s) 
#define OSMEMPOOLS_JOURNAL_NEWREC() 
#endif /* OSMEMPOOLS_JOURNAL */

/***** Public Functions ************************************************/

/*******************************************************************************
* cliMemLibInit
*
* DESCRIPTION:
*       Initialize the memory management with a user memory function.
*       Can be used to allocate from a dedicated memory pool that remains till
*       HW reset.
*
* INPUTS:
*       memAllocFuncPtr - the function to be used in memory allocation.
*       memFreeFuncPtr - the function to be used in memory allocation.
*       size - The memory size to manage
*       memAllocMethod - determines the method of the allocation
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK   - on success
*       GT_NO_RESOURCE - not enough memory in the system.
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS cliMemLibInit
(
    IN CLI_MEMORY_ALLOC_FUNC memAllocFuncPtr,
    IN CLI_MEMORY_FREE_FUNC memFreeFuncPtr,
    IN GT_U32 size,
    IN CLI_MEMORY_MANAGE_METHOD_ENT memAllocMethod
)
{
    GT_U32 chunkMaxAmount = 0;
    GT_STATUS retVal = GT_OK;

    static GT_BOOL firstMemInit = GT_TRUE;

    if (firstMemInit == GT_TRUE)
    {
        if (size == 0)
        {
            return GT_BAD_PARAM;
        }

        chunkMaxAmount = OSMEM_MAX_MEMORY_SIZE_USE / size;

        os_mem_chunks_array.chunkArray = memAllocFuncPtr(sizeof(OSMEM_CHUNK_POINTER)
                                                            *chunkMaxAmount);
        os_mem_chunks_array.chunkSize = size;
        os_mem_chunks_array.lastChunkIndex = 0;
        os_mem_chunks_array.memAllocMethod = memAllocMethod;
        os_mem_chunks_array.chunkArraySize = chunkMaxAmount;
        os_mem_chunks_array.memAllocFuncPtr = memAllocFuncPtr;
        os_mem_chunks_array.memFreeFuncPtr = memFreeFuncPtr;

        retVal  = cliMemChunkAllocate();
        if (retVal != GT_OK)
        {
            return retVal;
        }

        retVal = cliMemPoolsInit(OSMEM_DEFAULT_BLOCK_STEP_SIZE,
                       OSMEM_DEFAULT_STEPS_LIMIT,
                       OSMEM_DEFAULT_POOLS_LIMIT);
        if (retVal != GT_OK)
        {
            memFreeFuncPtr(os_mem_chunks_array.chunkArray[0].startAddr);
            os_mem_chunks_array.lastChunkIndex = 0;
            memFreeFuncPtr(os_mem_chunks_array.chunkArray);
            os_mem_chunks_array.chunkArray = NULL;
            return retVal;
        }
    }

    firstMemInit = GT_FALSE;

    return retVal;
}

/*******************************************************************************
* cliMemLibReset
*
* DESCRIPTION:
*       Reset memory pools to empty state
*
* INPUTS:
*       None
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - not initialized yet
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS cliMemLibReset(void)
{
    GT_U32 i;
    if (os_mem_pools_array == NULL)
    {
        return GT_FAIL;
    }
    /* reset pools */
    for (i = 0; i < os_mem_pools_array_size; i++)
    {
        OSMEM_POOL_HDR_PTR poolHdrPtr = os_mem_pools_array[i];
        poolHdrPtr->chunkIndex = 0;
        poolHdrPtr->first_free_PTR = NULL;
#ifdef OSMEMPOOLS_DEBUG
        poolHdrPtr->free_size = 0;
        poolHdrPtr->overAlloc = 0;
        poolHdrPtr->alloc_times = 0;
        poolHdrPtr->free_times = 0;
        poolHdrPtr->malloc_used = 0;
        poolHdrPtr->size = 0;
#endif
    }
    /* Reset chunks */
    for (i = 0; i < os_mem_chunks_array.lastChunkIndex; ++i)
    {
        OSMEM_CHUNK_POINTER_PTR chunk = &(os_mem_chunks_array.chunkArray[i]);
        chunk->dynamicAddr = chunk->startAddr;
        chunk->freeSize = os_mem_chunks_array.chunkSize;
#ifdef OS_MEM_64_BIT_ALIGNED
        while (((GT_UINTPTR)(chunk->dynamicAddr)) & 0x07)
        {
            chunk->dynamicAddr++;
            chunk->freeSize--;
        }
#endif
    }
    return GT_OK;
}

#ifdef OSMEMPOOLS_DEBUG
void chk_addr(void* ptr)
{
    int i;
    for (i = 0; i < os_mem_chunks_array.lastChunkIndex; ++i)
    {
        OSMEM_CHUNK_POINTER_PTR chunk = &(os_mem_chunks_array.chunkArray[i]);
        if ((GT_U8*)ptr >= chunk->startAddr && (GT_U8*)ptr < chunk->startAddr+os_mem_chunks_array.chunkSize)
            return;
    }
    printf("Realloc/free for bad address (%p)\n",ptr);
}
#endif
/*******************************************************************************
* cliRealloc
*
* DESCRIPTION:
*       Reallocate memory block of specified size.
*
* INPUTS:
*       ptr  - pointer to previously allocated buffer
*       size - bytes to allocate
*
* OUTPUTS:
*       None
*
* RETURNS:
*       Void pointer to the allocated space, or NULL if there is
*       insufficient memory available
*
* COMMENTS:
*       Usage of this function is NEVER during initialization.
*       Recommended to avoid usage of this function.
*
*******************************************************************************/
GT_VOID *cliRealloc
(
    IN GT_VOID * ptr ,
    IN GT_U32    size
)
{
    GT_VOID *allocPtr = NULL;
    GT_U32  oldSize = size;
    OSMEM_POOL_ELEM_PTR memPoolElem;
    if (os_mem_pools_array == NULL)
    {
        return NULL;
    }

    if (ptr == NULL)
    {
        return cliMalloc(size);
    }
#ifdef OSMEMPOOLS_DEBUG
    chk_addr(ptr);
#endif
    if (size == 0)
    {
        cliFree(ptr);
        return NULL;
    }
    allocPtr = cliMalloc(size);
    if (allocPtr == NULL)
    {
        return NULL;
    }

    memPoolElem = (OSMEM_POOL_ELEM_PTR)((GT_VOID*)((GT_UINTPTR)ptr - sizeof(OSMEM_POOL_ELEM)));
    oldSize = memPoolElem->size;

    memcpy(allocPtr,ptr,size > oldSize ? oldSize : size);
    cliFree(ptr);
    return allocPtr;
}

/***** Private ********************************************************/

/*******************************************************************************
* cliMemChunkAllocate
*
* DESCRIPTION:
*       Allocates another chunk to the chunk array.
*
* INPUTS:
*       None
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK - if success.
*       GT_FAIL - otherwise.
*
* COMMENTS:
*       None
*
*******************************************************************************/
static GT_STATUS cliMemChunkAllocate(GT_VOID)
{
    OSMEM_CHUNK_POINTER_PTR chunk;

    if (os_mem_chunks_array.lastChunkIndex ==os_mem_chunks_array.chunkArraySize)
    {
        return GT_OUT_OF_RANGE;
    }

    chunk = &(os_mem_chunks_array.chunkArray[os_mem_chunks_array.lastChunkIndex]);
    chunk->startAddr = os_mem_chunks_array.memAllocFuncPtr(os_mem_chunks_array.chunkSize);

    if (chunk->startAddr == NULL)
    {
        return GT_OUT_OF_CPU_MEM;
    }

    chunk->dynamicAddr = chunk->startAddr;
    chunk->freeSize = os_mem_chunks_array.chunkSize;
#ifdef OS_MEM_64_BIT_ALIGNED
    while (((GT_UINTPTR)(chunk->dynamicAddr)) & 0x07)
    {
        chunk->dynamicAddr++;
        chunk->freeSize--;
    }
#endif

    os_mem_chunks_array.lastChunkIndex++;
    return GT_OK;

}

/*******************************************************************************
* cliMemPoolsInit
*
* DESCRIPTION:
*      initializes the pools for memory management. it creates pools with sizes
*      starting with blockSizeStep and adding blockSizeStep to the created pool
*      size until it reaches stepsLimit, from then on till poolsLimit it doubles
*      the size every time. (example: if blockSizeStep = 2, stepsLimit = 8,
*      poolsLimit = 32 :the pools that will be created are with element sizes
       of : 2,4,6,8,16,32)
* INPUTS:
*       blockSizeStep - the step in size that should be added for every
*                       sequanctual pool created.
*       stepsLimit - the limit at which sizes stop being add to but doubled
*                    (must be a power of 2)
*       poolsLimit - the biggest size pool that will be created.
*                    (must be a power of 2)
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK - if success.
*       GT_FAIL - otherwise.
*
* COMMENTS:
*       None
*
*******************************************************************************/
static GT_STATUS cliMemPoolsInit
(
    IN GT_U32 blockSizeStep,
    IN GT_U32 stepsLimit,
    IN GT_U32 poolsLimit

)
{
    GT_U32 i, curSize,curInd,log2ofStepsLimit,log2ofPoolsLimit;
    GT_U32 sizeIndex;
    log2ofStepsLimit = 0;
    log2ofPoolsLimit = 0;

    /* some sanity checks first */
    if (blockSizeStep == 0 ||
        stepsLimit == 0 ||
        poolsLimit == 0 ||
        (poolsLimit < stepsLimit) ||
        ((stepsLimit % blockSizeStep) != 0 ) ||
        (mathLog2(stepsLimit,&log2ofStepsLimit) != GT_OK) ||
        (mathLog2(poolsLimit,&log2ofPoolsLimit) != GT_OK))

    {
        return GT_FAIL;
    }

    os_mem_poolsLimit = poolsLimit;

#ifdef MUTEX_PROTECTION_ENABLED
    /* Create the sync semaphore */
    if (osMutexCreate("cliMemPools", &os_mem_pools_mtx_id) != GT_OK)
    {
        return GT_FAIL;
    }
    osMutexLock(os_mem_pools_mtx_id);
#endif

    /* calculate the amount of pools we need */
    os_mem_pools_array_size = (stepsLimit / blockSizeStep) +
        (log2ofPoolsLimit - log2ofStepsLimit) + 1 ;

    os_mem_pools_array =
        os_mem_chunks_array.memAllocFuncPtr(sizeof(OSMEM_POOL_HDR_PTR) * (os_mem_pools_array_size + 1));

    os_mem_pools_size2index = os_mem_chunks_array.memAllocFuncPtr(sizeof(*os_mem_pools_size2index) *(poolsLimit+1));

    if (os_mem_pools_array == NULL || os_mem_pools_size2index == NULL)
    {
#ifdef MUTEX_PROTECTION_ENABLED
        osMutexUnlock(os_mem_pools_mtx_id);
        osMutexDelete(os_mem_pools_mtx_id);
        os_mem_pools_mtx_id = (GT_MUTEX)0;
#endif
        return GT_FAIL;
    }

    curInd = 0;
    curSize = blockSizeStep;
    for (i = 0; i < os_mem_pools_array_size - 1; ++i)
    {
        os_mem_pools_array[i] = cliMemPoolCreate(curSize,i);
        while (curInd <= curSize)
            os_mem_pools_size2index[curInd++] = i;

        if (os_mem_pools_array[i] == NULL)
        {
#ifdef MUTEX_PROTECTION_ENABLED
            osMutexUnlock(os_mem_pools_mtx_id);
            osMutexDelete(os_mem_pools_mtx_id);
            os_mem_pools_mtx_id = (GT_MUTEX)0;
#endif
            return GT_FAIL;
        }

        if (curSize >= stepsLimit)
        {
            curSize *= 2;
        }
        else
        {
            curSize += blockSizeStep;
        }
    }
    os_mem_poolsLimit = curInd-1;

    /* now build the variuos sizes pool */
    os_mem_pools_array[i] = cliMemPoolCreate(0,i);

#ifdef MUTEX_PROTECTION_ENABLED
    osMutexUnlock(os_mem_pools_mtx_id);
#endif

    return GT_OK;
}

/*******************************************************************************
* cliMemPoolCreate
*
* DESCRIPTION:
*      creates a memory pool for malloc managment
*
* INPUTS:
*       elemSize - the size of elements to be allocated from this pool.
*                  in case the size = 0 then this is a various sizes pool.
*       poolIndex - the index of the pool in the pool array.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       the pool id .
*
* COMMENTS:
*       None
*
*******************************************************************************/
static OSMEM_POOL_HDR *cliMemPoolCreate
(
    IN GT_U32 elemSize,
    IN GT_U32 poolIndex
)
{
    OSMEM_POOL_HDR *poolHdrPtr;
    poolHdrPtr = NULL;

    poolHdrPtr = os_mem_chunks_array.memAllocFuncPtr(sizeof(OSMEM_POOL_HDR));

    if (poolHdrPtr == NULL)
    {
        return NULL;
    }

    poolHdrPtr->elem_size = elemSize;
    poolHdrPtr->first_free_PTR = NULL;
    poolHdrPtr->chunkIndex = 0;
    poolHdrPtr->poolIndex = poolIndex;

#ifdef OSMEMPOOLS_DEBUG
    poolHdrPtr->free_size = 0;
    poolHdrPtr->size = 0;
    poolHdrPtr->overAlloc = 0;
    poolHdrPtr->alloc_times = 0;
    poolHdrPtr->malloc_used = 0;
    poolHdrPtr->free_times = 0;
#endif

    return poolHdrPtr;
}

/*******************************************************************************
* cliMalloc
*
* DESCRIPTION:
*       Allocates memory block of specified siz.
*
* INPUTS:
*       none.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       Void pointer to the allocated space, or NULL if there is
*       insufficient memory available.
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_VOID *cliMalloc
(
    IN GT_U32 size
)
{
    GT_VOID *memBlock;
    OSMEM_POOL_HDR_PTR curPool = NULL;

    if (os_mem_pools_array == NULL)
    {
        return NULL;
    }
    OSMEMPOOLS_JOURNAL_NEWREC();
    OSMEMPOOLS_JOURNAL_OP(1);
    OSMEMPOOLS_JOURNAL_SIZE(size);
    /* search through the pools for the appropriate size pool */
    if (size > os_mem_poolsLimit)
    {
        if ((size + sizeof (OSMEM_POOL_ELEM)) > os_mem_chunks_array.chunkSize)
        {
            cmdOsPrintf("Warning:Tried to allocate %d which is bigger than the memory chunk size. \n",
                     size);
            return NULL;
        }
        curPool = os_mem_pools_array[os_mem_pools_array_size-1];
    }
    else
    {
        curPool = os_mem_pools_array[os_mem_pools_size2index[size]];
    }
    /* heap allocation byte counter correction */
    heap_bytes_allocated += curPool->elem_size;

#ifdef MUTEX_PROTECTION_ENABLED
    osMutexLock(os_mem_pools_mtx_id);
#endif

    /* now allocate the pointer */
    memBlock = cliMemPoolAllocSize(curPool,size);
    OSMEMPOOLS_JOURNAL_PTR(memBlock);

#ifdef MUTEX_PROTECTION_ENABLED
    osMutexUnlock(os_mem_pools_mtx_id);
#endif

    return memBlock;
}

/*******************************************************************************
* cliMemPoolAllocSize
*
* DESCRIPTION:
*      allocates a pointer from a memory pool (for sizes bigger than
*      poolsLimit - see cliMemPoolsInit).
*
* INPUTS:
*       poolId - the pool from which to allocate.
*       size   - the size to allocate
*
* OUTPUTS:
*       None
*
* RETURNS:
*       the pointer to the newly allocated data.
*
* COMMENTS:
*       None
*
*******************************************************************************/
static GT_VOID* cliMemPoolAllocSize
(
    IN OSMEM_POOL_HDR_PTR poolHdrPtr,
    IN GT_U32             size
)
{
    OSMEM_POOL_FREE_ELEM_PTR freePtr = NULL;
    OSMEM_POOL_FREE_ELEM_PTR prvFreePtr = NULL;

    OSMEM_POOL_ELEM_PTR allocPtr = NULL;
    GT_U32 allocSize = 0;

#ifdef OSMEMPOOLS_DEBUG
    poolHdrPtr->alloc_times++;
#endif

    /* if there is a free element in the free list take from there */

    /* first deal with various sizes pool */
    if (poolHdrPtr->elem_size == 0)
    {
        size = (size + 3) & 0xfffffffc; /* align 4 bytes */
        if (poolHdrPtr->first_free_PTR != NULL)
        {
            freePtr = poolHdrPtr->first_free_PTR;
            while ((freePtr != NULL) && (freePtr->size != size))
            {
                prvFreePtr = freePtr;
                freePtr = freePtr->next_elem;
            }
            if (freePtr != NULL)
            {
                if (prvFreePtr == NULL)
                {
                    poolHdrPtr->first_free_PTR = freePtr->next_elem;
                }
                else
                {
                    prvFreePtr->next_elem = freePtr->next_elem;
                }

#ifdef OSMEMPOOLS_DEBUG
                poolHdrPtr->free_size -= freePtr->size;
#endif

            }
        }

    }
    else if (poolHdrPtr->first_free_PTR != NULL)
    {
        freePtr = poolHdrPtr->first_free_PTR;

        /* now link out the pieces */
        poolHdrPtr->first_free_PTR = freePtr->next_elem;

#ifdef OSMEMPOOLS_DEBUG
        poolHdrPtr->free_size -= poolHdrPtr->elem_size;
#endif
if (size > poolHdrPtr->elem_size) printf("allocating %d from pool %d elem_size %d\n",
size,poolHdrPtr->poolIndex,poolHdrPtr->elem_size);
    }


    allocPtr = (OSMEM_POOL_ELEM_PTR)freePtr;
    /* if we could find any elements in the free list -> take a new element
       from the chunk */

    /* firsr deal with various size pool */
    if (poolHdrPtr->elem_size == 0)
    {
        if (allocPtr == NULL)
        {
            allocSize = sizeof(OSMEM_POOL_ELEM) + size;

            allocPtr = cliMemAllocDynamicFromChunk(allocSize,
                                                     &poolHdrPtr->chunkIndex);

            if (allocPtr == NULL)
            {
                return NULL;
            }

            allocPtr->size = size;

#ifdef OSMEMPOOLS_DEBUG
            poolHdrPtr->malloc_used++;

            poolHdrPtr->size += size;
#endif
        }
        else
        {
            size = freePtr->size;
            allocPtr->size = size;
        }
#ifdef OS_MEM_64_BIT_ALIGNED
        allocPtr->padding = 0;
#endif

        /* return a pointer to the data segment only */
        allocPtr++;
        return(allocPtr);

    }
    else if (allocPtr == NULL)
    {
        allocSize = poolHdrPtr->elem_size;
        poolHdrPtr->overAlloc += poolHdrPtr->elem_size - size;

        allocPtr = cliMemAllocDynamicFromChunk(
            sizeof(OSMEM_POOL_ELEM) + allocSize,
            &poolHdrPtr->chunkIndex);

        if (allocPtr == NULL)
        {
            return NULL;
        }

#ifdef OSMEMPOOLS_DEBUG
        poolHdrPtr->malloc_used++;

        poolHdrPtr->size += poolHdrPtr->elem_size;
#endif
    }

    /* this variable consists the required allocation size  */
    allocPtr->size = size;
#ifdef OS_MEM_64_BIT_ALIGNED
    allocPtr->padding = 0;
#endif

    /* return a pointer to the data segment only */
    allocPtr++;
    return (allocPtr);

}

/*******************************************************************************
* cliMemAllocDynamicFromChunk
*
* DESCRIPTION:
*       Allocates dynamic alloc memory from the given chunk.
*
* INPUTS:
*       size - the size to allocate.
*       chunkIndex - the index of the chunk from which to allocate.
*
* OUTPUTS:
*       chunkIndex - if the memory was allocated from a different chunk then the
*                    given one , then that chunk index is returned.
*
* RETURNS:
*       a pointer to the allocated memory.
*
* COMMENTS:
*       None
*
*******************************************************************************/
static GT_VOID *cliMemAllocDynamicFromChunk
(
    IN    GT_U32 size,
    INOUT GT_U32 *chunkIndex
)
{
    GT_VOID *retPtr = NULL;
    OSMEM_CHUNK_POINTER_PTR curChunk;

#ifdef OS_MEM_64_BIT_ALIGNED
    size = (size + 7) & 0xfffffff8;
#endif
    /* start by using the given chunk */
    curChunk = &(os_mem_chunks_array.chunkArray[*chunkIndex]);

    /* now look for the first allocated chunk with enough memory (could be the
       given one) */
    while (*chunkIndex < os_mem_chunks_array.lastChunkIndex &&
           size > curChunk->freeSize)
    {
        (*chunkIndex)++;
        curChunk = &(os_mem_chunks_array.chunkArray[*chunkIndex]);
    }

    /* if none of the allocated ones hasn't got enough memory allocated a new
       one (if possible) */
    if (*chunkIndex >= os_mem_chunks_array.lastChunkIndex)
    {
        /* check if allowed to allocate more? */
        if (os_mem_chunks_array.memAllocMethod ==
                    CLI_MEMORY_MANAGE_METHOD_NO_REALLOC_ON_DEMAND_E)
        {
            /* we reached out of mem situation! */
            return NULL;
        }
        /* allocate another chunk */
        if (cliMemChunkAllocate() != GT_OK)
        {
            return NULL;
        }
    }
    /* now take the dynamicAddr for the return ptr */
    retPtr = curChunk->dynamicAddr;
    /* and update the chunk */
    curChunk->dynamicAddr += size;
    curChunk->freeSize -= size;

    /* and as a precheck that the next element can or cannot be allocated from
       this chunk */
    if (size > curChunk->freeSize)
    {
        (*chunkIndex)++;
    }

    return retPtr;
}

/*******************************************************************************
* cliFree
*
* DESCRIPTION:
*      frees a pointer from a memory pool
*
* INPUTS:
*       memBlock - the pointer which was allocated.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       none
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_VOID cliFree
(
    IN GT_VOID    *memBlock
)
{
    OSMEM_POOL_HDR_PTR poolHdrPtr;
    OSMEM_POOL_ELEM_PTR allocPtr;
    OSMEM_POOL_FREE_ELEM_PTR freePtr;

    GT_U32 freeSize = 0;

    if (os_mem_pools_array == NULL)
    {
        return;
    }
    if (memBlock == NULL)
    {
        return;
    }
    OSMEMPOOLS_JOURNAL_NEWREC();
    OSMEMPOOLS_JOURNAL_OP(2);
    OSMEMPOOLS_JOURNAL_PTR(memBlock);
#ifdef OSMEMPOOLS_DEBUG
    chk_addr(memBlock);
#endif
#ifdef MUTEX_PROTECTION_ENABLED
    osMutexLock(os_mem_pools_mtx_id);
#endif

    allocPtr = (OSMEM_POOL_ELEM_PTR)((GT_VOID*)((GT_UINTPTR)memBlock - sizeof(OSMEM_POOL_ELEM)));
    freeSize = allocPtr->size;
    OSMEMPOOLS_JOURNAL_SIZE(freeSize);
    if (freeSize > os_mem_poolsLimit)
    {
        poolHdrPtr = os_mem_pools_array[os_mem_pools_array_size-1];
    }
    else
    {
        poolHdrPtr = os_mem_pools_array[os_mem_pools_size2index[freeSize]];
    }

    freePtr = (OSMEM_POOL_FREE_ELEM_PTR)allocPtr;
    if (poolHdrPtr->elem_size != 0)
    {
        freeSize = poolHdrPtr->elem_size;
    }
    freePtr->size = freeSize;

    /* heap allocation byte counter correction */
    heap_bytes_allocated -= freeSize;

#ifdef OSMEMPOOLS_DEBUG
    poolHdrPtr->free_times++;
#endif

   /* now deal with the free list ***** */

    freePtr->next_elem = poolHdrPtr->first_free_PTR;
    poolHdrPtr->first_free_PTR = freePtr;

#ifdef OSMEMPOOLS_DEBUG
    poolHdrPtr->free_size = poolHdrPtr->free_size + freeSize;
#endif

#ifdef MUTEX_PROTECTION_ENABLED
    osMutexUnlock(os_mem_pools_mtx_id);
#endif

    return;
}

#if 0
void dump_d(GT_U8* ptr)
{
   int i;
   printf("ptr=%p",ptr);
   ptr -= 16;
   for (i = 0; i < 32; i++)
   {
        if (i %16 == 0) printf("\n%p: ",ptr+i);
        printf(" %02x",ptr[i]);
   }
   printf("\n");
}
GT_VOID cliFree_s
(
    IN GT_VOID    *memBlock,
    IN GT_U32     size
)
{
    OSMEM_POOL_ELEM_PTR allocPtr;

    if (os_mem_pools_array == NULL)
    {
        return;
    }
    if (memBlock == NULL)
    {
        return;
    }
#ifdef OSMEMPOOLS_DEBUG
    chk_addr(memBlock);
#endif
    allocPtr = (OSMEM_POOL_ELEM_PTR)((GT_VOID*)((GT_UINTPTR)memBlock - sizeof(OSMEM_POOL_ELEM)));
    if (allocPtr->size != size)
    {
        if (size <= os_mem_poolsLimit || size > allocPtr->size)
        {
            printf("Freeing memory %p size=%d expected=%d\n", memBlock, allocPtr->size, size);
            printf("allocPtr=%p  sizeof(OSMEM_POOL_ELEM)=%d\n",allocPtr,sizeof(OSMEM_POOL_ELEM));
            dump_d(memBlock);
            sleep(1);
            return;
        }
    }
    cliFree(memBlock);
}
#endif

/***** Debug ********************************************************/
#ifdef OSMEMPOOLS_DEBUG

/*******************************************************************************
* cliMemPrintMemPoolsStat
*
* DESCRIPTION:
*        print statistics of all pools
*
* INPUTS:
*
*
* OUTPUTS:
*       None.
*
* RETURNS:
*
*
* COMMENTS:
*
*******************************************************************************/
GT_VOID cliMemPrintMemPoolsStat
(
    GT_VOID
)
{
    GT_U32 i;
    GT_U32 totalFree, totalAlloc, totalAllocTimes, totalFreeTimes;
    GT_U32 totalMallocUsed, totalOverhead, totalOverAlloc, dynamicSize;
    GT_U32 totalDynamic;
    
    totalFree = 0;
    totalAlloc = 0;
    totalAllocTimes = 0;
    totalFreeTimes = 0;
    totalMallocUsed = 0;
    totalOverhead = 0;
    totalOverAlloc = 0;
    totalDynamic = 0;

    cmdOsPrintf("cliMemPools Status:\n");
    cmdOsPrintf("------------------\n\n");
    for (i = 0; i < os_mem_pools_array_size; ++i)
    {
        if (os_mem_pools_array[i]->size > 0)
        {
            cmdOsPrintf("Statistics for Pool with Element Size of:       %d\n",
                     os_mem_pools_array[i]->elem_size);
            cmdOsPrintf("-------------------------------------------------------------------------\n");
            cmdOsPrintf("Free: %d \tUsed: %d\tSize: %d\tOverhead:(Alloc) %d\t (List)%d\n",
                     os_mem_pools_array[i]->free_size,
                     (os_mem_pools_array[i]->size -
                     os_mem_pools_array[i]->free_size),
                     os_mem_pools_array[i]->size,
                     os_mem_pools_array[i]->overAlloc,
                     os_mem_pools_array[i]->alloc_times*
                     sizeof(OSMEM_POOL_ELEM));
            cmdOsPrintf("Alloc called: %d \tFree called %d \t Reusage(Per) %d\t Chunk:%d->%d  first_free=%p\n\n",
                     os_mem_pools_array[i]->alloc_times,
                     os_mem_pools_array[i]->free_times,
                     ((os_mem_pools_array[i]->alloc_times -
                      os_mem_pools_array[i]->malloc_used)*100) /
                     os_mem_pools_array[i]->alloc_times,
                     os_mem_pools_array[i]->chunkIndex,
                     os_mem_chunks_array.chunkArray[os_mem_pools_array[i]->chunkIndex].freeSize,
                     os_mem_pools_array[i]->first_free_PTR);
            totalFree += os_mem_pools_array[i]->free_size;
            totalAlloc += os_mem_pools_array[i]->size;
            totalAllocTimes += os_mem_pools_array[i]->alloc_times;
            totalFreeTimes += os_mem_pools_array[i]->free_times;
            totalMallocUsed += os_mem_pools_array[i]->malloc_used;
            totalOverhead += os_mem_pools_array[i]->malloc_used*
                     sizeof(OSMEM_POOL_ELEM);
            totalOverAlloc += os_mem_pools_array[i]->overAlloc;
        }
    }

    cmdOsPrintf("Total Free: %d\tTotal Allocated: %d\tTotal OverHead:(Alloc) %d (List) %d\n",
             totalFree,totalAlloc,totalOverAlloc, totalOverhead);
    cmdOsPrintf("Total Alloc called: %d\tTotal Free called %d\tTotal malloc used: %d\n",
             totalAllocTimes,totalFreeTimes,totalMallocUsed);
    cmdOsPrintf("Total Memory used: %d \n",totalOverhead+totalAlloc);

    totalFree = 0;
    totalAlloc = 0;
    for (i = 0; i < os_mem_chunks_array.lastChunkIndex; ++i)
    {
        cmdOsPrintf("\nStatistics for Chunk %d \t Size: %d",
                 i,
                 os_mem_chunks_array.chunkSize);
        cmdOsPrintf("-------------------------------------------------------------------------\n");
        dynamicSize = (GT_U32)(os_mem_chunks_array.chunkArray[i].dynamicAddr -
                      os_mem_chunks_array.chunkArray[i].startAddr);
        totalDynamic += dynamicSize;

        totalFree += os_mem_chunks_array.chunkArray[i].freeSize;
        totalAlloc += os_mem_chunks_array.chunkSize;

        cmdOsPrintf("Addr: %p  Dynamic: %d(%d)\t Free:%d(%d)\n",
                 os_mem_chunks_array.chunkArray[i].startAddr,
                 dynamicSize,
                 (dynamicSize* 100)/os_mem_chunks_array.chunkSize,
                 os_mem_chunks_array.chunkArray[i].freeSize,
                 (os_mem_chunks_array.chunkArray[i].freeSize* 100)
                 /os_mem_chunks_array.chunkSize);
    }

    cmdOsPrintf("\nTotal Chunk: Dynamic : %d(%d) Free: %d(%d)\n",
             totalDynamic,
             (totalDynamic*100)/totalAlloc,
             totalFree,
             (totalFree*100)/totalAlloc);

    return;
}


#endif /*OSMEMPOOLS_DEBUG*/
