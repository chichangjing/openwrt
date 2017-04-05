/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* gtOsMemLib.c
*
* DESCRIPTION:
*       osMemPool implementation
*
* DEPENDENCIES:
*       None
*
* FILE REVISION NUMBER:
*       $Revision: 20 $
*******************************************************************************/

/* IMPORTANT : we must remove the declaration of OS_MALLOC_MEMORY_LEAKAGE_DBG */
/* because we not want local calls to osMalloc to be affected by the H file definitions */
#define FORCE_IGNORE_OS_MALLOC_MEMORY_LEAKAGE_DBG

/***** Includes ********************************************************/
#ifdef _VXWORKS
#include <vxWorks.h>
#endif
#include <string.h>
#include <stdlib.h>
#include <assert.h>

#include <gtOs/gtOsMem.h>
#include <gtOs/gtOsSem.h>
#include <gtOs/gtOsGen.h>
#include <gtOs/gtOsIo.h>
#include <gtOs/gtOsExc.h>
#include <gtOs/gtOsTimer.h>
#include <gtOs/gtOsSharedData.h>

#ifdef ASIC_SIMULATION
# define CHECK_ADDRESS_ON_FREE
#endif

/*********  important ****  important  ***  important ****  important **********
NOTE: in this file the OS_MALLOC_MEMORY_LEAKAGE_DBG is NEVER defined
    instead this file must check
    INTERNAL_OS_MALLOC_MEMORY_LEAKAGE_DBG
    instead of
    OS_MALLOC_MEMORY_LEAKAGE_DBG
*********  important ****  important  ***  important ****  important **********/

/***** Function Declaration ********************************************/
void *osMemPoolsAlloc(GT_U32 size);
void *osMemPoolsRealloc(void *ptr, GT_U32 size);
void osMemPoolFree(void *memBlock);

extern GT_VOID *internalOsMalloc(IN GT_U32   size);
extern GT_VOID *internalOsRealloc(IN GT_VOID * ptr ,IN GT_U32    size);
extern GT_VOID internalOsFree(IN GT_VOID* const memblock);

static GT_STATUS osMemChunkAllocate(GT_VOID);

static GT_STATUS osMemPoolsInit(GT_U32 blockSizeStep, GT_U32 stepsLimit,
                                GT_U32 poolsLimit);
static OSMEM_POOL_HDR *osMemPoolCreate(GT_U32 elemSize,GT_U32 poolIndex);
static GT_VOID* osMemPoolAllocSize(OSMEM_POOL_HDR_PTR poolHdrPtr,GT_U32 size);
static GT_VOID* osMemPoolAllocVarSize(OSMEM_POOL_HDR_PTR poolHdrPtr,GT_U32 size,GT_U32 *outSizePtr);
static GT_VOID *osMemAllocDynamicFromChunk(GT_U32 size,GT_U32 *chunkIndex);
static GT_STATUS osMemPoolClear(OSMEM_POOL_HDR_PTR poolHdrPtr);

#ifdef INTERNAL_OS_MALLOC_MEMORY_LEAKAGE_DBG
GT_STATUS osMemRegisterPtr(
    IN GT_VOID *    allocPtr,
    IN GT_U32       numOfBytes,
    IN const char*    fileNamePtr,
    IN GT_U32   line
);
void osMemUnregisterPtr(
    IN GT_VOID *    oldPointer,
    IN const char*    fileNamePtr,
    IN GT_U32   line
);
extern GT_VOID *osMalloc_MemoryLeakageDbg
(
    IN GT_U32   size,
    IN const char*    fileNamePtr,
    IN GT_U32   line
);
#else /*!INTERNAL_OS_MALLOC_MEMORY_LEAKAGE_DBG*/
/* define empty macro to reduce management when not needed */
#define osMemRegisterPtr(a,b,c,d)
#define osMemUnregisterPtr(a,b,c)
#endif /*!INTERNAL_OS_MALLOC_MEMORY_LEAKAGE_DBG*/


/***** External Functions **********************************************/
extern GT_STATUS mathLog2(IN  GT_U32  num,OUT GT_U32  *result);

/***** Global Vars ********************************************/
static GT_BOOL firstMemInit __SHARED_DATA_MAINOS = GT_TRUE;
static OSMEM_POOL_HDR_PTR* os_mem_pools_array __SHARED_DATA_MAINOS;
static GT_U32 os_mem_pools_array_size __SHARED_DATA_MAINOS;
static GT_MUTEX os_mem_pools_mtx_id __SHARED_DATA_MAINOS;
static GT_U32 *os_mem_pools_size2index __SHARED_DATA_MAINOS;
static OSMEM_CHUNK_ARRAY os_mem_chunks_array __SHARED_DATA_MAINOS;
static GT_U32 os_mem_poolsLimit __SHARED_DATA_MAINOS;

/* poolIndex to mark osStaticMalloc */
#define POOL_INDEX_STATIC_CNS   0xFFFFFFFF


/* heap allocation measurement static variables */
static GT_U32 heap_bytes_allocated __SHARED_DATA_MAINOS = 0;
static GT_U32 mallocCounter __SHARED_DATA_MAINOS = 0;
static GT_U32 mallocFailCounterLimit __SHARED_DATA_MAINOS = 0;
static GT_BOOL mallocFailEnable __SHARED_DATA_MAINOS = GT_FALSE;

/* global state of the engine of recording memory allocations info */
static OSMEM_MEMORY_LEAKAGE_STATE_DBG_ENT globalMemoryLeakageState __SHARED_DATA_MAINOS = OSMEM_MEMORY_LEAKAGE_STATE_DBG_OFF_E;

/* magic number ot try catch damaged DB */
#define MAGIC_NUMBER_CNS 0xa5a5aF15

#define REPORT_CORRUPTED_MEMORY_MAC     \
    osPrintf("memory corruption detected in file [%s] line [%d] \n" , __FILE__ , __LINE__)

/** start of generic DMA manager of malloc and free **/
typedef struct DMA_ALLOCATION_STCT{
    GT_U32  myMagic;/* always need to be MAGIC_NUMBER_CNS */
    GT_PTR  myMemoryPtr;/* pointer to the allocation that we got from 'osCacheDmaMalloc' */
    GT_U32  myMemoryNumOfBytes;/* number of bytes of the allocation in myMemoryPtr */

    /* pointer to next element */
    struct DMA_ALLOCATION_STCT *nextElementPtr;
}DMA_ALLOCATION_STC;

static DMA_ALLOCATION_STC   globalDmaAllocations __SHARED_DATA_MAINOS;
static DMA_ALLOCATION_STC   *globalDmaAllocationsLastPtr __SHARED_DATA_MAINOS = &globalDmaAllocations;
static GT_U32   underSystemReset __SHARED_DATA_MAINOS = 0;

#ifndef SHARED_MEMORY
   /* Workaround for dmaRegisterPtr(), dmaFreeAll(), memRegisterPtr()
    * in case when no osMemLibInit() called
    */
#  define DBG_MALLOC_MAC    malloc
#  define DBG_FREE_MAC      free
#else
#  define DBG_MALLOC_MAC    os_mem_chunks_array.memAllocFuncPtr
#  define DBG_FREE_MAC      os_mem_chunks_array.memFreeFuncPtr
#endif
/** end of generic DMA manager of malloc and free **/


/*
 * Typedef: struct OSMEM_MEMORY_LEAKAGE_DBG_INFO
 *
 * Description:
 *      This structure holds the header for debug for memory leakage.
 *
 * Fields:
 *      fileNamePtr - (pointer to)the file name (or full path name) of the source code calling
 *                   this function. usually used as __FILE__
 *                  when NULL - ignored
 *      line    - line number in the source code file. usually used as __LINE__
 *      memoryLeakageState - state of debug for memory leakage
*/
typedef struct{
    const char*                 fileNamePtr;
    GT_U32                      line;
    OSMEM_MEMORY_LEAKAGE_STATE_DBG_ENT memoryLeakageState;
}OSMEM_MEMORY_LEAKAGE_DBG_INFO;


/** start of generic memory manager of malloc and free **/
typedef struct MEM_ALLOCATION_STCT{
    GT_U32  myMagic;/* always need to be MAGIC_NUMBER_CNS */

    GT_PTR  myMemoryPtr;/* pointer to the allocation that we got from 'osMalloc/osRealloc' */
    GT_U32  myMemoryNumOfBytes;/* number of bytes of the allocation in myMemoryPtr */
    OSMEM_MEMORY_LEAKAGE_DBG_INFO  memoryLeakageDbgInfo;

    /* pointer to next element */
    struct MEM_ALLOCATION_STCT *nextElementPtr;
}MEM_ALLOCATION_STC;

static MEM_ALLOCATION_STC   globalMemAllocations __SHARED_DATA_MAINOS;
#ifdef INTERNAL_OS_MALLOC_MEMORY_LEAKAGE_DBG
static MEM_ALLOCATION_STC   *globalMemAllocationsLastPtr __SHARED_DATA_MAINOS = &globalMemAllocations;
#endif /* INTERNAL_OS_MALLOC_MEMORY_LEAKAGE_DBG */
/** end of generic memory manager of malloc and free **/


/* error in implementation of DMA detection ... cause false alarms ... need debug */
#define ERROR_IN_IMPLEMENTATION 1

#if ERROR_IN_IMPLEMENTATION == 0
/* DMA : state the values to be used by the predefined 'overflow detection' region at the
   end of each 'Cached DMA' */
#define  DMA_NUM_PRE_DEFINED_PATTERNS_CNS   4
static GT_U32 dmaPreDefinedPatternsArr[DMA_NUM_PRE_DEFINED_PATTERNS_CNS] __SHARED_DATA_MAINOS =
    { 0xdeadbeaf  , 0xbeafdead , 0xa5a5a5a5 , 0x5a5a5a5a};
#endif /*ERROR_IN_IMPLEMENTATION == 0*/

/* DMA : number of lines with the dmaPreDefinedPatternsArr[] patterns */
#define DMA_NUM_LINES_OF_PRE_DEFINED_PATTERNS   16

/* DMA : number of bytes that used as 'overflow detection' region at the end of each 'Cached DMA' */
#define  DMA_NUM_BYTES_OVERFLOW_DETECTION_REGION_CNS    ((DMA_NUM_PRE_DEFINED_PATTERNS_CNS*4) * DMA_NUM_LINES_OF_PRE_DEFINED_PATTERNS)
/* DMA : indication that need 'overflow detection' region at the end of each 'Cached DMA'*/
static GT_BOOL dmaOverflowDetectionEnabled __SHARED_DATA_MAINOS = GT_FALSE;

/***** Public Functions ************************************************/

/*******************************************************************************
* osBzero
*
* DESCRIPTION:
*       Fills the first nbytes characters of the specified buffer with 0
*
* INPUTS:
*       start  - start address of memory block to be zeroed
*       nbytes - size of block to be set
*
* OUTPUTS:
*       None
*
* RETURNS:
*       None
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_VOID osBzero
(
    IN GT_CHAR * start,
    IN GT_U32 nbytes
)
{
    assert(start != NULL);
#ifdef _VXWORKS
    bzero((GT_CHAR *)start, nbytes);
#else
    memset((GT_VOID *)start, 0,  nbytes);
#endif
    return;
}

/*******************************************************************************
* osMemSet
*
* DESCRIPTION:
*       Stores 'symbol' converted to an unsigned char in each of the elements
*       of the array of unsigned char beginning at 'start', with size 'size'.
*
* INPUTS:
*       start  - start address of memory block for setting
*       symbol - character to store, converted to an unsigned char
*       size   - size of block to be set
*
* OUTPUTS:
*       None
*
* RETURNS:
*       Pointer to set memory block
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_VOID * osMemSet
(
    IN GT_VOID * start,
    IN int    symbol,
    IN GT_U32 size
)
{
    if(start == NULL)
        return start;
#ifdef _VXWORKS
    IS_WRAPPER_OPEN_PTR;
#endif
    return memset(start, symbol, size);
}

/*******************************************************************************
* osMemCpy
*
* DESCRIPTION:
*       Copies 'size' characters from the object pointed to by 'source' into
*       the object pointed to by 'destination'. If copying takes place between
*       objects that overlap, the behavior is undefined.
*
* INPUTS:
*       destination - destination of copy
*       source      - source of copy
*       size        - size of memory to copy
*
* OUTPUTS:
*       None
*
* RETURNS:
*       Pointer to destination
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_VOID * osMemCpy
(
    IN GT_VOID *       destination,
    IN const GT_VOID * source,
    IN GT_U32       size
)
{
    if(destination == NULL || source == NULL)
        return destination;

#ifdef _VXWORKS
    IS_WRAPPER_OPEN_PTR;
#endif
    return memcpy(destination, source, size);
}

/*******************************************************************************
* osMemMove
*
* DESCRIPTION:
*       Copies 'size' characters from the object pointed to by 'source' into
*       the object pointed to by 'destination'. The memory areas may overlap:
*       copying takes place as though the bytes in src are first copied into
*       a temporary array that does  not  overlap src or dest, and the bytes
*       are then copied from the temporary array to dest.
*
* INPUTS:
*       destination - destination of copy
*       source      - source of copy
*       size        - size of memory to copy
*
* OUTPUTS:
*       None
*
* RETURNS:
*       Pointer to destination
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_VOID * osMemMove
(
    IN GT_VOID *       destination,
    IN const GT_VOID * source,
    IN GT_U32          size
)
{
    if(destination == NULL || source == NULL)
        return destination;

#ifdef _VXWORKS
    IS_WRAPPER_OPEN_PTR;
#endif
    return memmove(destination, source, size);
}

/*******************************************************************************
* osMemCmp
*
* DESCRIPTION:
*       Compare 'size' characters from the object pointed to by 'str1' to
*       the object pointed to by 'str2'.
*
* INPUTS:
*       str1 - first memory area
*       str2 - second memory area
*       size - size of memory to compare
*
* OUTPUTS:
*       None
*
* RETURNS:
*       > 0  - if str1 is alfabetic bigger than str2
*       == 0 - if str1 is equal to str2
*       < 0  - if str1 is alfabetic smaller than str2
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_32 osMemCmp
(
    IN const GT_VOID  *str1,
    IN const GT_VOID  *str2,
    IN GT_U32       size
)
{
    assert(str1 != NULL);
    assert(str2 != NULL);

    /* this code prevents undefined results of memcmp in case when:
        pointers are the same: str1 == str2, and str1 points to unaligned memory */
    if(str2 == str1)
    {
        osPrintf("osMemCmp warning: Tried to compare the same memory, size is [%d]. \n", size);
        return 0;
    }

#ifdef _VXWORKS
    IS_WRAPPER_OPEN_PTR;
#endif
    return (GT_32) memcmp(str1, str2, size);
}


/*******************************************************************************
* osMemLibInit
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
GT_STATUS osMemLibInit
(
    IN GT_MEMORY_ALLOC_FUNC memAllocFuncPtr,
    IN GT_MEMORY_FREE_FUNC memFreeFuncPtr,
    IN GT_U32 size,
    IN GT_MEMORY_MANAGE_METHOD_ENT memAllocMethod
)
{
    GT_U32 chunkMaxAmount = 0;
    GT_STATUS retVal = GT_OK;

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
        os_mem_chunks_array.staticIndex = 0;
        os_mem_chunks_array.memAllocMethod = memAllocMethod;
        os_mem_chunks_array.chunkArraySize = chunkMaxAmount;
        os_mem_chunks_array.memAllocFuncPtr = memAllocFuncPtr;
        os_mem_chunks_array.memFreeFuncPtr = memFreeFuncPtr;

        retVal  = osMemChunkAllocate();
        if (retVal != GT_OK)
        {
            return retVal;
        }

        retVal = osMemPoolsInit(OSMEM_DEFAULT_BLOCK_STEP_SIZE,
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
#ifdef _VXWORKS
        /*gtShutdownRegister(osMemPoolsClear);*/
#endif
    }

    firstMemInit = GT_FALSE;

    return retVal;
}

/*******************************************************************************
* osStaticMallocFromChunk
*
* DESCRIPTION:
*       Allocates memory block of specified size, this memory will not be free.
*
* INPUTS:
*       size - bytes to allocate
*
* OUTPUTS:
*       None
*
* RETURNS:
*       Void pointer to the allocated space, or NULL if there is
*       insufficient memory available or calling after first init.
*
* COMMENTS:
*       Usage of this function is only on FIRST initialization.
*
*******************************************************************************/
GT_VOID *osStaticMallocFromChunk
(
    IN GT_U32 size
)
{
    OSMEM_POOL_ELEM_PTR retPtr = NULL;
    OSMEM_CHUNK_POINTER_PTR curChunk;
    GT_U32 allocSize;

    /* Align size to 4 bytes. */
#ifdef OS_MEM_64_BIT_ALIGNED
    size = (size + 7) & 0xFFFFFFF8;
#else
    size = (size + 3) & 0xFFFFFFFC;
#endif

    osMutexLock(os_mem_pools_mtx_id);
    allocSize = size + sizeof (OSMEM_POOL_ELEM);

    if (allocSize > os_mem_chunks_array.chunkSize)
    {
        osPrintf("Warning:Tried to allocate %d which is bigger than the memory chunk size. \n",
                 size);
        osMutexUnlock(os_mem_pools_mtx_id);

        return os_mem_chunks_array.memAllocFuncPtr(size);
    }
    curChunk =
        &os_mem_chunks_array.chunkArray[os_mem_chunks_array.staticIndex];

    while(os_mem_chunks_array.staticIndex < os_mem_chunks_array.lastChunkIndex &&
          curChunk->staticAddr - allocSize < curChunk->dynamicAddr)
    {
        os_mem_chunks_array.staticIndex++;
        curChunk =
        &os_mem_chunks_array.chunkArray[os_mem_chunks_array.staticIndex];
    }

    if (os_mem_chunks_array.staticIndex >= os_mem_chunks_array.lastChunkIndex)
    {
        if (os_mem_chunks_array.memAllocMethod ==
                GT_MEMORY_MANAGE_METHOD_NO_REALLOC_ON_DEMAND_E)
        {
            osMutexUnlock(os_mem_pools_mtx_id);

            osPrintf("Out Of Memory!!\n");
            return NULL;
        }
        if (osMemChunkAllocate() != GT_OK)
        {
            osMutexUnlock(os_mem_pools_mtx_id);
            return NULL;
        }
    }

    curChunk->staticAddr -= allocSize;
    curChunk->freeSize -= allocSize;
    retPtr = (OSMEM_POOL_ELEM_PTR)curChunk->staticAddr;

    retPtr->poolIndex = (GT_UINTPTR)POOL_INDEX_STATIC_CNS;  /* it's more safe to use larger value, it allow more than 255 pools */

    osMutexUnlock(os_mem_pools_mtx_id);

    retPtr++;
    return (retPtr);
}

/*******************************************************************************
* osMemPoolsRealloc
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
GT_VOID *osMemPoolsRealloc
(
    IN GT_VOID * ptr ,
    IN GT_U32    size
)
{
    GT_VOID *allocPtr = NULL;
    GT_U32  oldSize = size;
    GT_U32  poolIndex;
    OSMEM_VAR_POOL_ELEM_PTR allocVarPtr;
    OSMEM_NOPOOLS_HDR_STC *hdrPtr;

    allocPtr = osMemPoolsAlloc(size);
    if (allocPtr == NULL)
    {
        return NULL;
    }

    hdrPtr = (OSMEM_NOPOOLS_HDR_STC*)((GT_UINTPTR)ptr - sizeof(OSMEM_NOPOOLS_HDR_STC));
    /* resolve size of memory pointed by ptr */
    if (hdrPtr->magic != (GT_UINTPTR)hdrPtr) /* should be always true */
    {
        /* this memory is allocated by pool manager */
        poolIndex = (GT_U32)hdrPtr->magic;
        if (poolIndex <= os_mem_pools_array_size)
        {
            oldSize = os_mem_pools_array[poolIndex]->elem_size;

            if (oldSize == 0)
            {
                allocVarPtr = (OSMEM_VAR_POOL_ELEM_PTR)
                              ((GT_VOID*)((GT_UINTPTR)ptr - sizeof(OSMEM_VAR_POOL_ELEM)));
                oldSize = (GT_U32)allocVarPtr->size;
            }
        }
    }
    else
    {
        oldSize = (GT_U32)hdrPtr->size;
    }

    osMemCpy(allocPtr,ptr,size > oldSize ? oldSize : size);
    osMemPoolFree(ptr);
    return allocPtr;
}

/*******************************************************************************
* osMemPoolsClear
*
* DESCRIPTION:
*      returns all the allocated memory from all the pools to free.
*
* INPUTS:
*       none.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK - if success.
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS osMemPoolsClear()
{
    GT_U32 i,dynSize;

    osMutexLock(os_mem_pools_mtx_id);

    for (i = 0; i < os_mem_pools_array_size; ++i)
    {
        osMemPoolClear(os_mem_pools_array[i]);
    }

    for (i = 0; i < os_mem_chunks_array.lastChunkIndex; ++i)
    {
        dynSize = (GT_U32)(os_mem_chunks_array.chunkArray[i].dynamicAddr -
            os_mem_chunks_array.chunkArray[i].startAddr);

        os_mem_chunks_array.chunkArray[i].freeSize += dynSize;

        os_mem_chunks_array.chunkArray[i].dynamicAddr =
            os_mem_chunks_array.chunkArray[i].startAddr;
    }

    osMutexUnlock(os_mem_pools_mtx_id);

    return GT_OK;
}

/*******************************************************************************
* osMemBindMemAllocFunc
*
* DESCRIPTION:
*        Binds the the memory allocation function
*
* INPUTS:
*        memAllocFuncPtr - the function to be used in memory allocation
*                          (refer to osMemLibInit)
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK - operation succeeded
*       GT_FAIL - operation failed
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS osMemBindMemAllocFunc
(
    IN   GT_MEMORY_ALLOC_FUNC    memAllocFuncPtr
)
{
    os_mem_chunks_array.memAllocFuncPtr = memAllocFuncPtr;

    return GT_OK;
}

/***** Private ********************************************************/

/*******************************************************************************
* osMemChunkAllocate
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
static GT_STATUS osMemChunkAllocate(GT_VOID)
{
    if (os_mem_chunks_array.lastChunkIndex ==os_mem_chunks_array.chunkArraySize)
    {
        return GT_OUT_OF_RANGE;
    }

    os_mem_chunks_array.chunkArray[os_mem_chunks_array.lastChunkIndex].startAddr
        = os_mem_chunks_array.memAllocFuncPtr(os_mem_chunks_array.chunkSize);

    if (os_mem_chunks_array.chunkArray[os_mem_chunks_array.lastChunkIndex].startAddr
        == NULL)
    {
        return GT_OUT_OF_CPU_MEM;
    }

    os_mem_chunks_array.chunkArray[os_mem_chunks_array.lastChunkIndex].dynamicAddr =
        os_mem_chunks_array.chunkArray[os_mem_chunks_array.lastChunkIndex].startAddr;

    os_mem_chunks_array.chunkArray[os_mem_chunks_array.lastChunkIndex].staticAddr =
        os_mem_chunks_array.chunkArray[os_mem_chunks_array.lastChunkIndex].startAddr +
        os_mem_chunks_array.chunkSize;

    os_mem_chunks_array.chunkArray[os_mem_chunks_array.lastChunkIndex].freeSize =
        os_mem_chunks_array.chunkSize;

    os_mem_chunks_array.lastChunkIndex++;
    return GT_OK;

}

#ifdef  CHECK_ADDRESS_ON_FREE
static GT_VOID checkMemoryAllocatedFromChunks(GT_VOID *ptr)
{
    GT_U32 i;
    GT_UINTPTR C;
    for (i = 0; i < os_mem_chunks_array.lastChunkIndex; i++)
    {
        C = (GT_UINTPTR)(os_mem_chunks_array.chunkArray[i].startAddr);
        if (((GT_UINTPTR)ptr >= C) &&
                ((GT_UINTPTR)ptr < (C + os_mem_chunks_array.chunkSize)))
        {
            return;
        }
    }
    osFatalError(OS_FATAL_WARNING, "Trying to free memory which is not allocated by osMalloc()");
}
#endif

/*******************************************************************************
* osMemPoolsInit
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
static GT_STATUS osMemPoolsInit
(
    IN GT_U32 blockSizeStep,
    IN GT_U32 stepsLimit,
    IN GT_U32 poolsLimit

)
{
    GT_U32 i, curSize,curInd,log2ofStepsLimit,log2ofPoolsLimit;
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

    /* Create the sync semaphore */
    if (osMutexCreate("osMemPools", &os_mem_pools_mtx_id) != GT_OK)
    {
        return GT_FAIL;
    }
    osMutexLock(os_mem_pools_mtx_id);

    /* calculate the amount of pools we need */
    os_mem_pools_array_size = (stepsLimit / blockSizeStep) +
        (log2ofPoolsLimit - log2ofStepsLimit) + 1 ;

    os_mem_pools_array =
        os_mem_chunks_array.memAllocFuncPtr(sizeof(OSMEM_POOL_HDR_PTR) * (os_mem_pools_array_size + 1));

    os_mem_pools_size2index = os_mem_chunks_array.memAllocFuncPtr(sizeof(*os_mem_pools_size2index) *(poolsLimit+1));

    if (os_mem_pools_array == NULL || os_mem_pools_size2index == NULL)
    {
        osMutexUnlock(os_mem_pools_mtx_id);
        osMutexDelete(os_mem_pools_mtx_id);
        os_mem_pools_mtx_id = (GT_MUTEX)0;
        return GT_FAIL;
    }

    curSize = blockSizeStep;
    for (i = 0; i < os_mem_pools_array_size - 1; ++i)
    {
        os_mem_pools_array[i] = osMemPoolCreate(curSize,i);
        if (os_mem_pools_array[i] == NULL)
        {
            osMutexUnlock(os_mem_pools_mtx_id);
            osMutexDelete(os_mem_pools_mtx_id);
            os_mem_pools_mtx_id = (GT_MUTEX)0;
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

    /* now build the variuos sizes pool */
    os_mem_pools_array[i] = osMemPoolCreate(0,i);

    /* now lets build the index list for fast size to pools indexing */
    curInd = 0;
    for (i = 1; i<= poolsLimit; i++)
    {
        os_mem_pools_size2index[i] = curInd;
        if (i >= stepsLimit)
        {
            if (mathLog2(i,&log2ofStepsLimit) == GT_OK)
            {
                curInd++;
            }
        }
        else if ( (i % blockSizeStep) == 0)
        {
            curInd++;
        }
    }

    osMutexUnlock(os_mem_pools_mtx_id);

    return GT_OK;
}

/*******************************************************************************
* osMemPoolCreate
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
static OSMEM_POOL_HDR *osMemPoolCreate
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

    poolHdrPtr->free_size = 0;
    poolHdrPtr->size = 0;
    poolHdrPtr->overAlloc = 0;
    poolHdrPtr->alloc_times = 0;
    poolHdrPtr->malloc_used = 0;
    poolHdrPtr->free_times = 0;

    return poolHdrPtr;
}

/*******************************************************************************
* osMemPoolsAlloc
*
* DESCRIPTION:
*       Allocates memory block of specified size.
*
* INPUTS:
*       size - bytes to allocate
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
GT_VOID *osMemPoolsAlloc
(
    IN GT_U32 size
)
{
    GT_VOID *memBlock;
    OSMEM_POOL_HDR_PTR curPool = NULL;
    GT_U32  allocSize;/*actual size that the memory manager gave the pointer */

    /* search through the pools for the appropriate size pool */
    if (size > os_mem_poolsLimit)
    {
        if ((size + sizeof (OSMEM_VAR_POOL_ELEM)) > os_mem_chunks_array.chunkSize)
        {
            OSMEM_NOPOOLS_HDR_STC *hdrPtr;
            osPrintf("Warning:Tried to allocate %d which is bigger than the memory chunk size. \n",
                     size);
            hdrPtr = (OSMEM_NOPOOLS_HDR_STC*)os_mem_chunks_array.memAllocFuncPtr(size+sizeof(OSMEM_NOPOOLS_HDR_STC));
            if (hdrPtr == NULL)
            {
                return NULL;
            }
            hdrPtr->magic = (GT_UINTPTR)hdrPtr;
            hdrPtr->size = size;
            hdrPtr++;
            return (GT_VOID*)hdrPtr;
        }
        curPool = os_mem_pools_array[os_mem_pools_array_size-1];
        /*NOTE: curPool->elem_size == 0 for this type of block */
        allocSize = size;
    }
    else
    {
        curPool = os_mem_pools_array[os_mem_pools_size2index[size]];
        allocSize = curPool->elem_size;
    }

    osMutexLock(os_mem_pools_mtx_id);

    /* now allocate the pointer */
    if (curPool->elem_size == 0)
    {
        memBlock = osMemPoolAllocVarSize(curPool,size, &allocSize);
    }
    else
    {
        memBlock = osMemPoolAllocSize(curPool,size);
    }

    /* heap allocation byte counter correction */
    heap_bytes_allocated += allocSize;

    osMutexUnlock(os_mem_pools_mtx_id);

    return memBlock;
}

/*******************************************************************************
* osMemPoolAllocSize
*
* DESCRIPTION:
*      allocates a pointer from a memory pool (for sizes bigger than
*      poolsLimit - see osMemPoolsInit).
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
static GT_VOID* osMemPoolAllocSize
(
    IN OSMEM_POOL_HDR_PTR poolHdrPtr,
    IN GT_U32             size
)
{
    OSMEM_POOL_FREE_ELEM_PTR freePtr = NULL;
    OSMEM_POOL_ELEM_PTR allocPtr = NULL;

    poolHdrPtr->alloc_times++;
    /* if there is a free element in the free list take from there */

    if (poolHdrPtr->first_free_PTR != NULL)
    {
        freePtr = poolHdrPtr->first_free_PTR;

        /* now link out the pieces */
        poolHdrPtr->first_free_PTR = freePtr->next_elem;

        poolHdrPtr->free_size  = poolHdrPtr->free_size -
                                 poolHdrPtr->elem_size;
    }


    allocPtr = (OSMEM_POOL_ELEM_PTR)freePtr;
    /* if we could find any elements in the free list -> take a new element
       from the chunk */

    if (allocPtr == NULL)
    {
        poolHdrPtr->overAlloc += poolHdrPtr->elem_size - size;

        allocPtr = osMemAllocDynamicFromChunk(
            sizeof(OSMEM_POOL_ELEM) + poolHdrPtr->elem_size,
            &poolHdrPtr->chunkIndex);

        if (allocPtr == NULL)
        {
            return NULL;
        }

        poolHdrPtr->malloc_used++;

        poolHdrPtr->size += poolHdrPtr->elem_size;
    }

    /* this variable consists the required allocation size  */
    allocPtr->poolIndex = (GT_UINTPTR)(poolHdrPtr->poolIndex);

    /* return a pointer to the data segment only */
    allocPtr++;
    return (allocPtr);

}

/*******************************************************************************
* osMemPoolAllocVarSize
*
* DESCRIPTION:
*      allocates a pointer from a memory pool (for sizes bigger than
*      poolsLimit - see osMemPoolsInit).
*
* INPUTS:
*       poolId - the pool from which to allocate.
*       size   - the size to allocate
*
* OUTPUTS:
*       outSizePtr - (pointer to)aligned size
*
* RETURNS:
*       the pointer to the newly allocated data.
*
* COMMENTS:
*       None
*
*******************************************************************************/
static GT_VOID* osMemPoolAllocVarSize
(
    IN OSMEM_POOL_HDR_PTR poolHdrPtr,
    IN GT_U32             size,
    OUT GT_U32           *outSizePtr
)
{
    OSMEM_POOL_FREE_ELEM_PTR freePtr = NULL;
    OSMEM_POOL_FREE_ELEM_PTR prvFreePtr = NULL;

    OSMEM_VAR_POOL_ELEM_PTR allocVarPtr = NULL;
    GT_U32 allocSize = 0;

    poolHdrPtr->alloc_times++;
    /* if there is a free element in the free list take from there */

#ifdef OS_MEM_64_BIT_ALIGNED
    size = (size + 7) & 0xfffffff8; /* align 8 bytes */
#else
    size = (size + 3) & 0xfffffffc; /* align 4 bytes */
#endif
    *outSizePtr = size;
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

            poolHdrPtr->free_size  = poolHdrPtr->free_size - size;

        }
    }


    allocVarPtr = (OSMEM_VAR_POOL_ELEM_PTR)freePtr;
    /* if we could find any elements in the free list -> take a new element
       from the chunk */

    if (allocVarPtr == NULL)
    {
        allocSize = sizeof(OSMEM_VAR_POOL_ELEM) + size;

        allocVarPtr = osMemAllocDynamicFromChunk(allocSize,
                                                 &poolHdrPtr->chunkIndex);

        if (allocVarPtr == NULL)
        {
            return NULL;
        }

        allocVarPtr->size = size;

        poolHdrPtr->malloc_used++;

        poolHdrPtr->size += size;
    }
    else
    {
        size = freePtr->size;
        allocVarPtr->size = size;
    }

    /* this variable consists the required allocation size  */
    /* allocVarPtr->poolIndex = size;  - it seems to be an error */
    /* last poolIndex - var size pool */
    allocVarPtr->poolIndex = (GT_UINTPTR)(poolHdrPtr->poolIndex);

    /* return a pointer to the data segment only */
    allocVarPtr++;
    return(allocVarPtr);

}

/*******************************************************************************
* osMemAllocDynamicFromChunk
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
static GT_VOID *osMemAllocDynamicFromChunk
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
           curChunk->dynamicAddr + size > curChunk->staticAddr)
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
                    GT_MEMORY_MANAGE_METHOD_NO_REALLOC_ON_DEMAND_E)
        {
            /* we reached out of mem situation! */
            return NULL;
        }
        /* allocate another chunk */
        if (osMemChunkAllocate() != GT_OK)
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
    if (curChunk->dynamicAddr + size > curChunk->staticAddr)
    {
        (*chunkIndex)++;
    }

    return retPtr;
}

/*******************************************************************************
* osMemPoolFree
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
GT_VOID osMemPoolFree
(
    IN GT_VOID    *memBlock
)
{
    OSMEM_POOL_HDR_PTR poolHdrPtr;
    OSMEM_POOL_ELEM_PTR allocPtr;
    OSMEM_VAR_POOL_ELEM_PTR allocVarPtr;
    OSMEM_POOL_FREE_ELEM_PTR freePtr;

    GT_U32 freeSize = 0;

    osMutexLock(os_mem_pools_mtx_id);
#ifdef  CHECK_ADDRESS_ON_FREE
    checkMemoryAllocatedFromChunks(memBlock);
#endif

    /* Note: the structure in memory just before *memBlock can be:
        1) OSMEM_POOL_ELEM_PTR      (4 bytes:              GT_U32 poolIndex)
        2) OSMEM_VAR_POOL_ELEM_PTR  (8 bytes: GT_U32 size, GT_U32 poolIndex) */
    allocPtr = (OSMEM_POOL_ELEM_PTR)((GT_VOID*)((GT_UINTPTR) memBlock - sizeof(OSMEM_POOL_ELEM)));


    if (allocPtr->poolIndex > (GT_UINTPTR)os_mem_pools_array_size)
    {   /* out of range poolIndex */
        if (allocPtr->poolIndex == (GT_UINTPTR)POOL_INDEX_STATIC_CNS)
        {
            osPrintf("Warning:Trying to free a osStaticMalloc pointer !!!\n");

            osMutexUnlock(os_mem_pools_mtx_id);
            return;
        }
        else
        {
            if(os_mem_chunks_array.memFreeFuncPtr == NULL)
            {
                osPrintf("Warning: osMemPoolFree - invalid poolIndex, memory lost !!!\n");
            }
            else
            {
                /* it was allocated in osMemPoolsAlloc(...) for huge size by
                   os_mem_chunks_array.memAllocFuncPtr(...)
                */
                os_mem_chunks_array.memFreeFuncPtr(memBlock);
            }

            osMutexUnlock(os_mem_pools_mtx_id);
            return;
        }
    }

    poolHdrPtr = os_mem_pools_array[allocPtr->poolIndex];

    if (poolHdrPtr->elem_size == 0)
    {
        /* various size pool */
        allocVarPtr = (OSMEM_VAR_POOL_ELEM_PTR)
                      ((GT_VOID*)((GT_UINTPTR)memBlock - sizeof(OSMEM_VAR_POOL_ELEM)));
        freeSize = (GT_U32)allocVarPtr->size;
        freePtr = (OSMEM_POOL_FREE_ELEM_PTR)allocVarPtr;
        freePtr->size = freeSize;


    }
    else
    {
        freePtr = (OSMEM_POOL_FREE_ELEM_PTR)allocPtr;
        freeSize = poolHdrPtr->elem_size;
    }

    if(heap_bytes_allocated < freeSize)
    {
        osFatalError(OS_FATAL_WARNING, "osMemPoolFree(): bad management cause negative heap_bytes_allocated \n");
    }
    else
    {
        /* heap allocation byte counter correction */
        heap_bytes_allocated -= freeSize;
    }

    poolHdrPtr->free_times++;

    /* now deal with the free list ***** */

    freePtr->next_elem = poolHdrPtr->first_free_PTR;
    poolHdrPtr->first_free_PTR = freePtr;

    poolHdrPtr->free_size = poolHdrPtr->free_size + freeSize;

    osMutexUnlock(os_mem_pools_mtx_id);

    return;
}

/*******************************************************************************
* osMemPoolClear
*
* DESCRIPTION:
*      returns all the allocated memory from this pool to free.
*
* INPUTS:
*       poolId - the pool to clear.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK - if success.
*
* COMMENTS:
*       None
*
*******************************************************************************/
static GT_STATUS osMemPoolClear
(
    IN OSMEM_POOL_HDR_PTR poolHdrPtr
)
{
    poolHdrPtr->first_free_PTR = NULL;
    poolHdrPtr->free_size = 0;
    poolHdrPtr->size = 0;
    poolHdrPtr->chunkIndex = 0;

    return GT_OK;
}

/***** Debug ********************************************************/
#ifdef OSMEMPOOLS_DEBUG

/* Global variables for debug */
static GT_U32 heap_start_counter_value __SHARED_DATA_MAINOS = 0;
static GT_32  countDynamic __SHARED_DATA_MAINOS = 0;
static GT_32  countStatic __SHARED_DATA_MAINOS = 0;
static GT_32  countDynamicOver __SHARED_DATA_MAINOS = 0;

/*******************************************************************************
* osMemPrintMemPoolsStat
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
GT_VOID osMemPrintMemPoolsStat
(
    GT_VOID
)
{
    GT_U32 i;
    GT_U32 totalFree, totalAlloc, totalAllocTimes, totalFreeTimes;
    GT_U32 totalMallocUsed, totalOverhead, totalOverAlloc, dynamicSize, staticSize;
    GT_U32 totalStatic, totalDynamic;

    totalFree = 0;
    totalAlloc = 0;
    totalAllocTimes = 0;
    totalFreeTimes = 0;
    totalMallocUsed = 0;
    totalOverhead = 0;
    totalOverAlloc = 0;
    totalStatic = 0;
    totalDynamic = 0;

    osPrintf("osMemPools Status:\n");
    osPrintf("------------------\n\n");
    for (i = 0; i < os_mem_pools_array_size; ++i)
    {
        if (os_mem_pools_array[i]->size > 0)
        {
            osPrintf("Statistics for Pool with Element Size of:       %d\n",
                     os_mem_pools_array[i]->elem_size);
            osPrintf("-------------------------------------------------------------------------\n");
            osPrintf("Free: %d \tUsed: %d\tSize: %d\tOverhead:(Alloc) %d\t (List)%d\n",
                     os_mem_pools_array[i]->free_size,
                     (os_mem_pools_array[i]->size -
                     os_mem_pools_array[i]->free_size),
                     os_mem_pools_array[i]->size,
                     os_mem_pools_array[i]->overAlloc,
                     os_mem_pools_array[i]->alloc_times*
                     sizeof(OSMEM_POOL_ELEM));
            osPrintf("Alloc called: %d \tFree called %d \t Reusage(Per) %d\t Chunk:%d->%d\n\n",
                     os_mem_pools_array[i]->alloc_times,
                     os_mem_pools_array[i]->free_times,
                     ((os_mem_pools_array[i]->alloc_times -
                      os_mem_pools_array[i]->malloc_used)*100) /
                     os_mem_pools_array[i]->alloc_times,
                     os_mem_pools_array[i]->chunkIndex,
                     os_mem_chunks_array.chunkArray[os_mem_pools_array[i]->chunkIndex].freeSize);
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

    osPrintf("Total Free: %d\tTotal Allocated: %d\tTotal OverHead:(Alloc) %d (List) %d\n",
             totalFree,totalAlloc,totalOverAlloc, totalOverhead);
    osPrintf("Total Alloc called: %d\tTotal Free called %d\tTotal malloc used: %d\n",
             totalAllocTimes,totalFreeTimes,totalMallocUsed);
    osPrintf("Total Memory used: %d \n",totalOverhead+totalAlloc);

    totalFree = 0;
    totalAlloc = 0;
    for (i = 0; i < os_mem_chunks_array.lastChunkIndex; ++i)
    {
        osPrintf("\nStatistics for Chunk %d \t Size: %d",
                 i,
                 os_mem_chunks_array.chunkSize);
        osPrintf("-------------------------------------------------------------------------\n");
        dynamicSize = (GT_U32)(os_mem_chunks_array.chunkArray[i].dynamicAddr -
                      os_mem_chunks_array.chunkArray[i].startAddr);
        totalDynamic += dynamicSize;
        staticSize = os_mem_chunks_array.chunkSize -
                     (GT_U32)(os_mem_chunks_array.chunkArray[i].staticAddr -
                      os_mem_chunks_array.chunkArray[i].startAddr);
        totalStatic += staticSize;

        totalFree += os_mem_chunks_array.chunkArray[i].freeSize;
        totalAlloc += os_mem_chunks_array.chunkSize;

        osPrintf("Dynamic: %d(%d)\t Static: %d(%d)\t Free:%d(%d)\n",
                 dynamicSize,
                 (dynamicSize* 100)/os_mem_chunks_array.chunkSize,
                 staticSize,
                 (staticSize* 100)/os_mem_chunks_array.chunkSize,
                 os_mem_chunks_array.chunkArray[i].freeSize,
                 (os_mem_chunks_array.chunkArray[i].freeSize* 100)
                 /os_mem_chunks_array.chunkSize);
    }

    osPrintf("\nTotal Chunk: Dynamic : %d(%d) Static: %d(%d) Free: %d(%d)\n",
             totalDynamic,
             (totalDynamic*100)/totalAlloc,
             totalStatic,
             (totalStatic*100)/totalAlloc,
             totalFree,
             (totalFree*100)/totalAlloc);

    return;
}

/*******************************************************************************
* osMemPrintMemDelta
*
* DESCRIPTION:
*        print memory delta
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
GT_VOID osMemPrintMemDelta
(
    GT_U32* dyn,
    GT_U32 *dynOver,
    GT_U32 *staticAlloc
)
{
    GT_U32 i;
    GT_U32 totalAlloc;
    GT_U32 totalOverhead, staticSize;
    GT_U32 totalStatic;

    totalAlloc = 0;
    totalOverhead = 0;
    totalStatic = 0;

    for (i = 0; i < os_mem_pools_array_size; ++i)
    {
        if (os_mem_pools_array[i]->size > 0)
        {
            totalAlloc += os_mem_pools_array[i]->size - os_mem_pools_array[i]->free_size;
            totalOverhead += os_mem_pools_array[i]->malloc_used*
                     sizeof(OSMEM_POOL_ELEM);
        }
    }

   for (i = 0; i < os_mem_chunks_array.lastChunkIndex; ++i)
   {
       staticSize = os_mem_chunks_array.chunkSize -
                    (GT_U32)(os_mem_chunks_array.chunkArray[i].staticAddr -
                     os_mem_chunks_array.chunkArray[i].startAddr);
       totalStatic += staticSize;
   }
   /*osPrintf("\nDelta: Dynamic : %d dynamic+overhead : %d Static: %d \n",
            totalAlloc - countDynamic,
            totalAlloc+totalOverhead - countDynamicOver,
            totalStatic - countStatic);*/

    if ((dyn != NULL) &&
        (dynOver != NULL) &&
        (staticAlloc != NULL))
    {
        *dyn += totalAlloc - countDynamic;
        *dynOver += totalAlloc+totalOverhead - countDynamicOver;
        *staticAlloc += totalStatic - countStatic;
    }

   countDynamic = totalAlloc;
   countStatic = totalStatic;
   countDynamicOver = totalAlloc+totalOverhead;

    return;
}

/*******************************************************************************
* osMemGetHeapBytesAllocated
*
* DESCRIPTION:
*       returns the number of bytes allocated in heap
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
GT_U32 osMemGetHeapBytesAllocated
(
    GT_VOID
)
{
    return heap_bytes_allocated;
}

/*******************************************************************************
* osMemStartHeapAllocationCounter
*
* DESCRIPTION:
*        set the current value of heap allocated bytes to the allocation counter
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
GT_VOID osMemStartHeapAllocationCounter
(
    GT_VOID
)
{
    heap_start_counter_value = heap_bytes_allocated;
}

/*******************************************************************************
* osMemGetHeapAllocationCounter
*
* DESCRIPTION:
*        returns the delta of current allocated bytes number and the value of allocation counter set by
*        preveous startHeapAllocationCounter() function
*
* INPUTS:
*
*
* OUTPUTS:
*       None.
*
* RETURNS:
*
* COMMENTS:
*
*******************************************************************************/
GT_U32 osMemGetHeapAllocationCounter
(
    GT_VOID
)
{
    return heap_bytes_allocated - heap_start_counter_value;
}


/*******************************************************************************
* osDbgMallocWithDebugInfo
*
* DESCRIPTION:
*       osDbgMallocWithDebugInfo replace osDbgMalloc to get debug info
*
* INPUTS:
*       size - bytes to allocate
*       fileNamePtr - (pointer to)the file name (or full path name) of the source code calling
*                   this function. usually used as __FILE__
*                  when NULL - ignored
*       line    - line number in the source code file. usually used as __LINE__
*
* OUTPUTS:
*       None
*
* RETURNS:
*       Void pointer to the allocated space, or NULL if there is
*       insufficient memory available
*
* COMMENTS:
*       None
*
*******************************************************************************/
void *osDbgMallocWithDebugInfo
(
    IN GT_U32 size,
    IN const char*    fileNamePtr,
    IN GT_U32   line
)
{
    void *ptr;
    if (mallocFailEnable == GT_TRUE)
    {
        mallocCounter++;
        if (mallocFailCounterLimit > 0)
        {
            if (mallocCounter >= mallocFailCounterLimit)
            {
                mallocCounter = 0;
                mallocFailCounterLimit = 0;
                return NULL;
            }
        }

    }

#ifdef INTERNAL_OS_MALLOC_MEMORY_LEAKAGE_DBG
    ptr = osMalloc_MemoryLeakageDbg(size,fileNamePtr,line);
#else /*INTERNAL_OS_MALLOC_MEMORY_LEAKAGE_DBG*/
    ptr = osMalloc(size);
#endif /*INTERNAL_OS_MALLOC_MEMORY_LEAKAGE_DBG*/
    return ptr;
}

/*******************************************************************************
* osDbgMalloc
*
* DESCRIPTION:
*       Allocates memory block of specified size or fail allocation
*       in accordance with checked flags
*
* INPUTS:
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
*       None
*
*******************************************************************************/
void *osDbgMalloc
(
    IN GT_U32 size
)
{
    return osDbgMallocWithDebugInfo(size,LOCAL_DEBUG_INFO);
}



/*******************************************************************************
* osDbgMallocFailSet
*
* DESCRIPTION:
*       Allocates memory block of specified size or fail allocation
*       in accordance with checked flags
*
* INPUTS:
*       size - bytes to allocate
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK  - on success
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS osDbgMallocFailSet
(
    IN GT_BOOL                  failEnable,
    IN GT_U32                   failCounter
)
{
    mallocCounter = 0;
    mallocFailEnable = failEnable;
    if (failEnable == GT_TRUE)
    {
        mallocFailCounterLimit  = failCounter;
    }
    else
    {
        mallocFailCounterLimit = 0;
    }
    return GT_OK;
}


/*******************************************************************************
* osMemGetMallocAllocationCounter
*
* DESCRIPTION:
*        Get malloc allocation counter
*
* INPUTS:
*       allocationNumberCounterPtr - pointer to number of malloc allocations
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK  - on success
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS osMemGetMallocAllocationCounter
(
    IN GT_U32 *allocationNumberCounterPtr
)
{
    *allocationNumberCounterPtr = mallocCounter;
    return GT_OK;
}

#endif /*OSMEMPOOLS_DEBUG*/

/*******************************************************************************
* dmaRegisterPtr
*
* DESCRIPTION:
*        register pointer that was returned by osCacheDmaMalloc(...)
*
* INPUTS:
*       allocPtr - allocated pointer by osCacheDmaMalloc
*       numOfBytes - number of bytes to allocated by osCacheDmaMalloc
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK  - on success
*       GT_OUT_OF_CPU_MEM - on CPU memory run out (malloc)
* COMMENTS:
*       function under locking of os_mem_pools_mtx_id
*******************************************************************************/
static GT_STATUS dmaRegisterPtr(
    IN GT_VOID *    allocPtr,
    IN GT_U32       numOfBytes
)
{
    DMA_ALLOCATION_STC *nextElementPtr = NULL;

    DMA_ALLOCATION_STC *currentPtr;

    /* find empty element */
    {
        currentPtr = &globalDmaAllocations;
        do
        {
            if(currentPtr->myMemoryPtr == NULL)
            {
                /* empty place */
                nextElementPtr = currentPtr;
                break;
            }

            /* update the 'current' to be 'next' */
            currentPtr = currentPtr->nextElementPtr;
        }while(currentPtr);
    }


    if(nextElementPtr == NULL)
    {
        /* new allocation so need new 'element' */
        nextElementPtr = DBG_MALLOC_MAC(sizeof(*nextElementPtr));
        if(nextElementPtr == NULL)
        {
            return GT_OUT_OF_CPU_MEM;
        }

        /* bind the previous element to the new element */
        globalDmaAllocationsLastPtr->nextElementPtr = nextElementPtr;


        /* update last element pointer to the newly allocated element*/
        globalDmaAllocationsLastPtr = nextElementPtr;

        /* terminate the link list */
        nextElementPtr->nextElementPtr = NULL;
    }

    nextElementPtr->myMagic = MAGIC_NUMBER_CNS;
    nextElementPtr->myMemoryPtr    = allocPtr;  /*save the DMA pointer */
    nextElementPtr->myMemoryNumOfBytes = numOfBytes;

#if ERROR_IN_IMPLEMENTATION == 0
    if(dmaOverflowDetectionEnabled == GT_TRUE)
    {
        GT_U32  ii,jj;
        GT_U32  *startOfOverflowDetectionRegionPtr;

        if(numOfBytes <= DMA_NUM_BYTES_OVERFLOW_DETECTION_REGION_CNS)
        {
            /* should not happen */
            return GT_ERROR;
        }

        /* the pointer and numOfBytes holds the 'DMA_NUM_BYTES_OVERFLOW_DETECTION_REGION_CNS'
           for the 'overflow detection' region at the end of each 'Cached DMA' .

            we need to initialize it with the pre-defined values.
        */

        startOfOverflowDetectionRegionPtr = (GT_U32*)((GT_CHAR*)allocPtr + (numOfBytes - DMA_NUM_BYTES_OVERFLOW_DETECTION_REGION_CNS));

        for(ii = 0 ; ii < DMA_NUM_LINES_OF_PRE_DEFINED_PATTERNS ; ii++)
        {
            for(jj = 0 ; jj < DMA_NUM_PRE_DEFINED_PATTERNS_CNS; jj++)
            {
                (*startOfOverflowDetectionRegionPtr)= dmaPreDefinedPatternsArr[jj];
                startOfOverflowDetectionRegionPtr ++;
            }
        }
    }
#endif /*ERROR_IN_IMPLEMENTATION == 0*/


    return GT_OK;
}

/*******************************************************************************
* osCacheDmaRegisterPtr
*
* DESCRIPTION:
*        register pointer that was returned by osCacheDmaMalloc(...)
*
* INPUTS:
*       allocPtr - allocated pointer by osCacheDmaMalloc
*       numOfBytes - number of bytes to allocated by osCacheDmaMalloc
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK  - on success
*       GT_OUT_OF_CPU_MEM - on CPU memory run out (malloc)
* COMMENTS:
*
*******************************************************************************/
GT_STATUS osCacheDmaRegisterPtr(
    IN GT_VOID *    allocPtr,
    IN GT_U32       numOfBytes
)
{
    GT_STATUS   rc;
    osMutexLock(os_mem_pools_mtx_id);
    rc = dmaRegisterPtr(allocPtr,numOfBytes);
    osMutexUnlock(os_mem_pools_mtx_id);

    return rc;
}

/*******************************************************************************
* dmaMemoryFind
*
* DESCRIPTION:
*        find element in DB according to it's DMA pointer
*
* INPUTS:
*       searchForPtr - DMA pointer to look for
*       numOfBytes - number of bytes to allocated by osCacheDmaMalloc
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       pointer to the element (NULL - means 'not found')
* COMMENTS:
*       function under locking of os_mem_pools_mtx_id
*******************************************************************************/
static DMA_ALLOCATION_STC* dmaMemoryFind
(
    IN GT_PTR                   searchForPtr
)
{
    DMA_ALLOCATION_STC *currentPtr;

    currentPtr = &globalDmaAllocations;

    do
    {
        if(currentPtr->myMemoryPtr == searchForPtr)
        {
            /* got match */
            return currentPtr;
        }

        /* update the 'current' to be 'next' */
        currentPtr = currentPtr->nextElementPtr;
    }while(currentPtr);

    return NULL;
}

/*******************************************************************************
* dmaUnregisterPtr
*
* DESCRIPTION:
*        unregister pointer that is free by osCacheDmaFree(...)
*
* INPUTS:
*       oldPointer - pointer to unregister
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       None
* COMMENTS:
*       function under locking of os_mem_pools_mtx_id
*******************************************************************************/
static void dmaUnregisterPtr(
    IN GT_VOID *    oldPointer
)
{
    DMA_ALLOCATION_STC *elementPtr = dmaMemoryFind(oldPointer);

    if(elementPtr == NULL)
    {
        /* not found the PTR to free */
        return ;
    }

    /* NOTE that : elementPtr->myMemoryPtr == oldPointer !!! */

    /* oldPointer is free by osCacheDmaFree(...) */

    elementPtr->myMemoryPtr = NULL;/* indicate that DMA was free , but element
                                      still exists */
    elementPtr->myMemoryNumOfBytes = 0;

    if(elementPtr->myMagic != MAGIC_NUMBER_CNS)
    {
        /* ERROR ! */
        REPORT_CORRUPTED_MEMORY_MAC;
    }


    return;
}

/*******************************************************************************
* dmaUnregisterPtr
*
* DESCRIPTION:
*        unregister pointer that is free by osCacheDmaFree(...)
*
* INPUTS:
*       oldPointer - pointer to unregister
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       None
* COMMENTS:
*
*******************************************************************************/
void osCacheDmaUnregisterPtr(
    IN GT_VOID *    oldPointer
)
{

    osMutexLock(os_mem_pools_mtx_id);
    if(underSystemReset)
    {
        /* we not want handling call from osCacheDmaSystemReset(...) to
            osCacheDmaFree(...) and to
            osCacheDmaUnregisterPtr(...)

            during changing the DB via osCacheDmaSystemReset
        */
    }
    else
    {
        dmaUnregisterPtr(oldPointer);
    }
    osMutexUnlock(os_mem_pools_mtx_id);
}


/*******************************************************************************
* dmaFreeAll
*
* DESCRIPTION:
*        free all DMA elements in the list and their DMA memory
*
* INPUTS:
*       oldPointer - pointer to unregister
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       None
* COMMENTS:
*       function under locking of os_mem_pools_mtx_id
*******************************************************************************/
static GT_VOID dmaFreeAll(GT_VOID)
{
    DMA_ALLOCATION_STC *currentPtr;
    DMA_ALLOCATION_STC *nextPtr;

    currentPtr = &globalDmaAllocations;
    /* skip first element we will treat it after the 'while' */
    currentPtr = currentPtr->nextElementPtr;

    while(currentPtr)
    {
        nextPtr = currentPtr->nextElementPtr;
        /* free the DMA allocation needed by the device */
        if(currentPtr->myMemoryPtr)/* free only if not NULL , because maybe already released */
        {
            osCacheDmaFree(currentPtr->myMemoryPtr);
        }

        if(currentPtr->myMagic != MAGIC_NUMBER_CNS)
        {
            /* error detected !!! */
            REPORT_CORRUPTED_MEMORY_MAC;

            /* we CAN'T go on the link list !!! it is corrupted !!! */
            break;
        }

        /* free the element itself */
#ifdef SHARED_MEMORY
        if(DBG_FREE_MAC != NULL)
#endif
            DBG_FREE_MAC(currentPtr);

        /* update the 'current' to be 'next' */
        currentPtr = nextPtr;
    }

    if(globalDmaAllocations.myMemoryPtr)
    {
        /* free DMA of first element */
        osCacheDmaFree(globalDmaAllocations.myMemoryPtr);
    }

    if(globalDmaAllocations.myMagic != MAGIC_NUMBER_CNS)
    {
        /* error detected !!! */
        REPORT_CORRUPTED_MEMORY_MAC;
    }

    /* restore DB to compilation time values */
    osMemSet(&globalDmaAllocations,0,sizeof(globalDmaAllocations));
    globalDmaAllocationsLastPtr = &globalDmaAllocations;

    return ;
}

/*******************************************************************************
* dmaFreeAll
*
* DESCRIPTION:
*        free all DMA elements in the list and their DMA memory
*
* INPUTS:
*       oldPointer - pointer to unregister
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       None
* COMMENTS:
*
*******************************************************************************/
GT_STATUS osCacheDmaSystemReset(void)
{
    osMutexLock(os_mem_pools_mtx_id);
    underSystemReset = 1;
    dmaFreeAll();
    underSystemReset = 0;
    osMutexUnlock(os_mem_pools_mtx_id);

    return GT_OK;
}

/*******************************************************************************
* osDbgMalloc_MemoryLeakageDbg
*
* DESCRIPTION:
*       osDbgMalloc_MemoryLeakageDbg replace osDbgMalloc to get debug info
*
* INPUTS:
*       size - bytes to allocate
*       fileNamePtr - (pointer to)the file name (or full path name) of the source code calling
*                   this function. usually used as __FILE__
*                  when NULL - ignored
*       line    - line number in the source code file. usually used as __LINE__
*
* OUTPUTS:
*       None
*
* RETURNS:
*       Void pointer to the allocated space, or NULL if there is
*       insufficient memory available
*
* COMMENTS:
*       None
*
*******************************************************************************/
void *osDbgMalloc_MemoryLeakageDbg
(
    IN GT_U32 size,
    IN const char*    fileNamePtr,
    IN GT_U32   line
)
{
    return osDbgMallocWithDebugInfo(size,fileNamePtr,line);
}

#ifdef INTERNAL_OS_MALLOC_MEMORY_LEAKAGE_DBG
/*******************************************************************************
* memRegisterPtr
*
* DESCRIPTION:
*        register pointer that was returned by osMalloc(...)/osRealloc(...)
*
* INPUTS:
*       allocPtr - allocated pointer by osMalloc(...)/osRealloc(...)
*       numOfBytes - number of bytes to allocated by osMalloc(...)/osRealloc(...)
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK  - on success
*       GT_OUT_OF_CPU_MEM - on CPU memory run out (malloc)
* COMMENTS:
*       function under locking of os_mem_pools_mtx_id
*******************************************************************************/
static GT_STATUS memRegisterPtr(
    IN GT_VOID *    allocPtr,
    IN GT_U32       numOfBytes,
    IN const char*    fileNamePtr,
    IN GT_U32   line
)
{
    MEM_ALLOCATION_STC *nextElementPtr = NULL;
    MEM_ALLOCATION_STC *currentPtr;

    /* find empty element */
    {
        currentPtr = &globalMemAllocations;
        do
        {
            if(currentPtr->myMemoryPtr == NULL)
            {
                /* empty place */
                nextElementPtr = currentPtr;
                break;
            }

            /* update the 'current' to be 'next' */
            currentPtr = currentPtr->nextElementPtr;
        }while(currentPtr);
    }


    if(nextElementPtr == NULL)
    {
        /* new allocation so need new 'element' */
        nextElementPtr = DBG_MALLOC_MAC(sizeof(*nextElementPtr));
        if(allocPtr == NULL)
        {
            return GT_OUT_OF_CPU_MEM;
        }

        /* bind the previous element to the new element */
        globalMemAllocationsLastPtr->nextElementPtr = nextElementPtr;

        /* update last element pointer to the newly allocated element*/
        globalMemAllocationsLastPtr = nextElementPtr;

        /* terminate the link list */
        nextElementPtr->nextElementPtr = NULL;
    }

    nextElementPtr->myMagic = MAGIC_NUMBER_CNS;
    nextElementPtr->myMemoryPtr    = allocPtr;  /*save the MEM pointer */
    nextElementPtr->myMemoryNumOfBytes = numOfBytes;
    /* register debug info about the caller */
    nextElementPtr->memoryLeakageDbgInfo.fileNamePtr = fileNamePtr;
    nextElementPtr->memoryLeakageDbgInfo.line = line;
    nextElementPtr->memoryLeakageDbgInfo.memoryLeakageState = globalMemoryLeakageState;

    return GT_OK;
}


/*******************************************************************************
* osMemRegisterPtr
*
* DESCRIPTION:
*        register pointer that was returned by osMalloc(...)/osRealloc(...)(...)
*
* INPUTS:
*       allocPtr - allocated pointer by osMemMalloc
*       numOfBytes - number of bytes to allocated by osMalloc(...)/osRealloc(...)
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK  - on success
*       GT_OUT_OF_CPU_MEM - on CPU memory run out (malloc)
* COMMENTS:
*
*******************************************************************************/
GT_STATUS osMemRegisterPtr(
    IN GT_VOID *    allocPtr,
    IN GT_U32       numOfBytes,
    IN const char*    fileNamePtr,
    IN GT_U32   line
)
{
    GT_STATUS   rc;
    osMutexLock(os_mem_pools_mtx_id);
    rc = memRegisterPtr(allocPtr,numOfBytes,fileNamePtr,line);
    osMutexUnlock(os_mem_pools_mtx_id);

    return rc;
}

/*******************************************************************************
* memMemoryFind
*
* DESCRIPTION:
*        find element in DB according to it's MEM pointer
*
* INPUTS:
*       searchForPtr - MEM pointer to look for
*       numOfBytes - number of bytes to allocated by osMalloc(...)/osRealloc(...)
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       pointer to the element (NULL - means 'not found')
* COMMENTS:
*       function under locking of os_mem_pools_mtx_id
*******************************************************************************/
static MEM_ALLOCATION_STC* memMemoryFind
(
    IN GT_PTR                   searchForPtr
)
{
    MEM_ALLOCATION_STC *currentPtr;

    currentPtr = &globalMemAllocations;

    do
    {
        if(currentPtr->myMemoryPtr == searchForPtr)
        {
            /* got match */
            return currentPtr;
        }

        /* update the 'current' to be 'next' */
        currentPtr = currentPtr->nextElementPtr;
    }while(currentPtr);

    return NULL;
}

/*******************************************************************************
* memUnregisterPtr
*
* DESCRIPTION:
*        unregister pointer that is free by osMalloc(...)/osRealloc(...)
*
* INPUTS:
*       oldPointer - pointer to unregister
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       None
* COMMENTS:
*       function under locking of os_mem_pools_mtx_id
*******************************************************************************/
static void memUnregisterPtr(
    IN GT_VOID *    oldPointer,
    IN const char*    fileNamePtr,
    IN GT_U32   line
)
{
    OSMEM_POOL_ELEM_PTR allocPtr;
    MEM_ALLOCATION_STC *elementPtr = memMemoryFind(oldPointer);

    if(oldPointer == NULL)
    {
        osPrintf("memUnregisterPtr : try to free 'NULL' pointer ?! called from file[%s] line[%d] \n",
            fileNamePtr,line);
        return ;
    }
    else
    if(elementPtr == NULL)
    {
        /* not found the PTR to free */
        osPrintf("memUnregisterPtr : try to free pointer that not allocated ?! called from file[%s] line[%d] \n",
            fileNamePtr,line);
        return ;
    }

    /* logic from osMemPoolFree(...) about 'static' malloc */
    if(firstMemInit == GT_FALSE)/* indication that osMemLibInit(...) was called */
    {
        allocPtr = (OSMEM_POOL_ELEM_PTR)((GT_VOID*)((GT_UINTPTR)oldPointer - sizeof(OSMEM_POOL_ELEM)));
        if (allocPtr->poolIndex > os_mem_pools_array_size)
        {
            if (allocPtr->poolIndex == POOL_INDEX_STATIC_CNS)
            {
                /* function osMemPoolFree(...) did not free the memory */
                if(elementPtr->memoryLeakageDbgInfo.fileNamePtr)
                {
                    osPrintf("memUnregisterPtr : try to free [%d] bytes of 'static pointer' ?! \n"
                        "called from file[%s] line[%d] and allocated from file[%s] line[%d] \n",
                        elementPtr->myMemoryNumOfBytes,
                        fileNamePtr,line,
                        elementPtr->memoryLeakageDbgInfo.fileNamePtr,
                        elementPtr->memoryLeakageDbgInfo.line);
                }
                else
                {
                    osPrintf("memUnregisterPtr : try to free [%d] bytes of 'static pointer' ?! \n",
                        elementPtr->myMemoryNumOfBytes);
                }
            }
            else
            if(os_mem_chunks_array.memFreeFuncPtr == NULL)
            {
                /* function osMemPoolFree(...) did not free the memory */
                if(elementPtr->memoryLeakageDbgInfo.fileNamePtr)
                {
                    osPrintf("memUnregisterPtr : try to free [%d] bytes of pointer without callback function?! \n"
                        "called from file[%s] line[%d] and allocated from file[%s] line[%d] \n",
                        elementPtr->myMemoryNumOfBytes,
                        fileNamePtr,line,
                        elementPtr->memoryLeakageDbgInfo.fileNamePtr,
                        elementPtr->memoryLeakageDbgInfo.line);
                }
                else
                {
                    osPrintf("memUnregisterPtr : try to free [%d] bytes of 'static pointer' ?! \n",
                        elementPtr->myMemoryNumOfBytes);
                }
            }
        }
    }

    if(elementPtr->myMagic != MAGIC_NUMBER_CNS)
    {
        /* error */
        REPORT_CORRUPTED_MEMORY_MAC;
    }

    /* NOTE that : elementPtr->myMemoryPtr == oldPointer !!! */

    /* oldPointer is free by osMemFree(...) */

    elementPtr->myMemoryPtr = NULL;/* indicate that MEM was free , but element
                                      still exists */
    elementPtr->myMemoryNumOfBytes = 0;

    elementPtr->memoryLeakageDbgInfo.fileNamePtr = NULL;
    elementPtr->memoryLeakageDbgInfo.line = 0;
    elementPtr->memoryLeakageDbgInfo.memoryLeakageState = OSMEM_MEMORY_LEAKAGE_STATE_DBG_OFF_E;

    return;
}

/*******************************************************************************
* memUnregisterPtr
*
* DESCRIPTION:
*        unregister pointer that is free by osMalloc(...)/osRealloc(...)
*
* INPUTS:
*       oldPointer - pointer to unregister
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       None
* COMMENTS:
*
*******************************************************************************/
void osMemUnregisterPtr(
    IN GT_VOID *    oldPointer,
    IN const char*    fileNamePtr,
    IN GT_U32   line
)
{

    osMutexLock(os_mem_pools_mtx_id);
    memUnregisterPtr(oldPointer,fileNamePtr,line);
    osMutexUnlock(os_mem_pools_mtx_id);
}
#endif /*INTERNAL_OS_MALLOC_MEMORY_LEAKAGE_DBG*/

typedef enum{
    MEM_REGISTERED_ACTION_DUMP_STATUS_ON_E      =   (1<<0),         /* dump those with 'on'             */
    MEM_REGISTERED_ACTION_DUMP_STATUS_OFF_E     =   (1<<1),        /* dump those with 'off'            */
    MEM_REGISTERED_ACTION_DUMP_STATUS_ANY_E     =   (MEM_REGISTERED_ACTION_DUMP_STATUS_ON_E |
                                                     MEM_REGISTERED_ACTION_DUMP_STATUS_OFF_E),  /* dump all                         */
    MEM_REGISTERED_ACTION_SET_ALL_ON_TO_OFF_E   =   (1<<2),      /* change all with 'on' to 'off'    */

    MEM_REGISTERED_ACTION___LAST__E/* must be last --- not to be used */
}MEM_REGISTERED_ACTION_ENT;

/*******************************************************************************
* memRegisteredPtrAction
*
* DESCRIPTION:
*        actions on the pointers that not free by osFree
*
* INPUTS:
*       action - the type of action
*            dump those with 'on'
*            dump those with 'off'
*            dump all
*            change all with 'on' to 'off'
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       None
* COMMENTS:
*       function under locking of os_mem_pools_mtx_id
*******************************************************************************/
GT_VOID memRegisteredPtrAction(
    IN MEM_REGISTERED_ACTION_ENT action
)
{
    MEM_ALLOCATION_STC *currentPtr;

    OSMEM_MEMORY_LEAKAGE_DBG_INFO *memoryLeakageDbgInfoPtr;
    GT_U32      sumNumBytes = 0,sumNumElements = 0;
    GT_U32      sumNumElementsWithoutFileName = 0;

    currentPtr = &globalMemAllocations;

    if(globalMemAllocations.myMagic == 0 &&
       globalMemAllocations.myMemoryPtr == NULL &&
       globalMemAllocations.myMemoryNumOfBytes == 0)
    {
        globalMemAllocations.myMagic = MAGIC_NUMBER_CNS;
    }

    do
    {
        memoryLeakageDbgInfoPtr = &currentPtr->memoryLeakageDbgInfo;

        if(currentPtr->myMagic != MAGIC_NUMBER_CNS)
        {
            REPORT_CORRUPTED_MEMORY_MAC;
            /* we CAN'T go on the link list !!! it is corrupted !!! */
            break;
        }
        else
        if(currentPtr->myMemoryPtr && currentPtr->myMemoryNumOfBytes)
        {
            switch(action)
            {
                case MEM_REGISTERED_ACTION_DUMP_STATUS_ON_E:         /* dump those with 'on'             */
                case MEM_REGISTERED_ACTION_DUMP_STATUS_OFF_E:        /* dump those with 'off'            */
                case MEM_REGISTERED_ACTION_DUMP_STATUS_ANY_E:        /* dump all                         */
                    if(
                       (
                        /* print status on */
                        ((action & MEM_REGISTERED_ACTION_DUMP_STATUS_ON_E) &&
                        (memoryLeakageDbgInfoPtr->memoryLeakageState == OSMEM_MEMORY_LEAKAGE_STATE_DBG_ON_E))

                        ||
                        /* print status off */
                        ((action & MEM_REGISTERED_ACTION_DUMP_STATUS_OFF_E) &&
                        (memoryLeakageDbgInfoPtr->memoryLeakageState == OSMEM_MEMORY_LEAKAGE_STATE_DBG_OFF_E))
                        ))
                    {
                        sumNumBytes += currentPtr->myMemoryNumOfBytes;
                        sumNumElements ++;

                        if(memoryLeakageDbgInfoPtr->fileNamePtr)
                        {
                            osPrintf("not free memory that was alloc/realloc of [%d] bytes from file[%s] line[%d] \n"
                                ,currentPtr->myMemoryNumOfBytes
                                ,memoryLeakageDbgInfoPtr->fileNamePtr
                                ,memoryLeakageDbgInfoPtr->line
                                );

                            if((sumNumElements % 50))
                            {
                                /* let time for WIN32 to be able to print all without loss */
                                osTimerWkAfter(50);
                            }
                        }
                        else
                        {
                            /* no file name ?! */
                            sumNumElementsWithoutFileName++;
                        }
                    }

                break;
                case MEM_REGISTERED_ACTION_SET_ALL_ON_TO_OFF_E:      /* change all with 'on' to 'off'    */
                    memoryLeakageDbgInfoPtr->memoryLeakageState = OSMEM_MEMORY_LEAKAGE_STATE_DBG_OFF_E;
                break;
                default:
                    break;
            }
        }

        /* update the 'current' to be 'next' */
        currentPtr = currentPtr->nextElementPtr;
    }while(currentPtr);

    switch(action)
    {
        case MEM_REGISTERED_ACTION_DUMP_STATUS_ON_E:         /* dump those with 'on'             */
        case MEM_REGISTERED_ACTION_DUMP_STATUS_OFF_E:        /* dump those with 'off'            */
        case MEM_REGISTERED_ACTION_DUMP_STATUS_ANY_E:        /* dump all                         */
            if(sumNumBytes)
            {
                osPrintf("not free memory that was alloc/realloc total of [%d] bytes in [%d] elements \n",
                    sumNumBytes,sumNumElements);
                if(sumNumElementsWithoutFileName)
                {
                    osPrintf("[%d] elements without file name \n",
                        sumNumElementsWithoutFileName);
                }
            }
            break;
        default:
            break;
    }



    return ;
}

/*******************************************************************************
* osMallocMemoryLeakageDbgAction
*
* DESCRIPTION:
*        debug tool relate to 'Memory leakage detection'
*        relevant only when compilation with : OS_MALLOC_MEMORY_LEAKAGE_DBG
*        Set state of debug for memory leakage: start/stop.
*
* INPUTS:
*       memoryLeakageState - state of debug for memory leakage:
*                           start/stop.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK  - on success
*
* COMMENTS:
*       use function osMallocMemoryLeakageDbgPrintStandingAllocated to dump all the
*       allocated pointer that are still allocated (not free) that were allocated
*       between last 'start' and the consecutive 'stop'
*******************************************************************************/
GT_STATUS osMallocMemoryLeakageDbgAction
(
    IN OSMEM_MEMORY_LEAKAGE_STATE_DBG_ENT  memoryLeakageState
)
{
    if(memoryLeakageState != OSMEM_MEMORY_LEAKAGE_STATE_DBG_OFF_E &&
       memoryLeakageState != OSMEM_MEMORY_LEAKAGE_STATE_DBG_ON_E)
    {
        return GT_BAD_PARAM;
    }

    if(memoryLeakageState == OSMEM_MEMORY_LEAKAGE_STATE_DBG_ON_E &&
       globalMemoryLeakageState != memoryLeakageState)
    {
        /* we start new session of recording , so clear previous recording */
        osMutexLock(os_mem_pools_mtx_id);
        memRegisteredPtrAction(MEM_REGISTERED_ACTION_SET_ALL_ON_TO_OFF_E);
        osMutexUnlock(os_mem_pools_mtx_id);
    }

    /* set global mode */
    globalMemoryLeakageState = memoryLeakageState;

    return GT_OK;
}

/*******************************************************************************
* osMallocMemoryLeakageDbgPrintStandingAllocated
*
* DESCRIPTION:
*       debug tool relate to 'Memory leakage detection'
*       relevant only when compilation with : OS_MALLOC_MEMORY_LEAKAGE_DBG
*       print the info about the allocated pointers that were not free in the
*       time between 'start' and 'Stop'
*
* INPUTS:
*       None.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK  - on success
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS osMallocMemoryLeakageDbgPrintStandingAllocated
(
    GT_VOID
)
{
    osMutexLock(os_mem_pools_mtx_id);
    memRegisteredPtrAction(MEM_REGISTERED_ACTION_DUMP_STATUS_ON_E);
    osMutexUnlock(os_mem_pools_mtx_id);

    return GT_OK;
}

#ifdef INTERNAL_OS_MALLOC_MEMORY_LEAKAGE_DBG
/*******************************************************************************
* osMalloc_MemoryLeakageDbg
*
* DESCRIPTION:
*       osMalloc_MemoryLeakageDbg replace osMalloc to get debug info
*
* INPUTS:
*       size - bytes to allocate
*       fileNamePtr - (pointer to)the file name (or full path name) of the source code calling
*                   this function. usually used as __FILE__
*                  when NULL - ignored
*       line    - line number in the source code file. usually used as __LINE__
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
*
*******************************************************************************/
GT_VOID *osMalloc_MemoryLeakageDbg
(
    IN GT_U32   size,
    IN const char*    fileNamePtr,
    IN GT_U32   line
)
{
    void *ptr;
    ptr = internalOsMalloc(size);

    osMemRegisterPtr(ptr,size,fileNamePtr,line);
    return ptr;
}

/*******************************************************************************
*   osRealloc_MemoryLeakageDbg
*
* DESCRIPTION:
*       osRealloc_MemoryLeakageDbg replace osRealloc to get debug info
*
* INPUTS:
*       ptr  - pointer to previously allocated buffer
*       size - bytes to allocate
*       fileNamePtr - (pointer to)the file name (or full path name) of the source code calling
*                   this function. usually used as __FILE__
*                  when NULL - ignored
*       line    - line number in the source code file. usually used as __LINE__
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
GT_VOID *osRealloc_MemoryLeakageDbg
(
    IN GT_VOID * ptr ,
    IN GT_U32    size,
    IN const char*    fileNamePtr,
    IN GT_U32   line
)
{
    void *newPtr;
    newPtr = internalOsRealloc(ptr,size);

    /* unregister old ptr */
    osMemUnregisterPtr(ptr,fileNamePtr,line);
    /* register new ptr */
    osMemRegisterPtr(newPtr,size,fileNamePtr,line);
    return newPtr;
}
/*******************************************************************************
* osFree_MemoryLeakageDbg
*
* DESCRIPTION:
*       osFree_MemoryLeakageDbg replace osFree to get debug info
*
* INPUTS:
*       memblock - previously allocated memory block to be freed
*       fileNamePtr - (pointer to)the file name (or full path name) of the source code calling
*                   this function. usually used as __FILE__
*                  when NULL - ignored
*       line    - line number in the source code file. usually used as __LINE__
*
* OUTPUTS:
*       None
*
* RETURNS:
*       None
*
* COMMENTS:
*       Usage of this function is NEVER during initialization.
*
*******************************************************************************/
GT_VOID osFree_MemoryLeakageDbg
(
    IN GT_VOID* const memblock,
    IN const char*    fileNamePtr,
    IN GT_U32   line
)
{
    internalOsFree(memblock);
    osMemUnregisterPtr(memblock,fileNamePtr,line);
}
#endif /*INTERNAL_OS_MALLOC_MEMORY_LEAKAGE_DBG*/


/*******************************************************************************
* dmaOverflowDetectionGet
*
* DESCRIPTION:
*       check if the DMA memory detected with overflow error.
*
*       this function allow the application to check via function
*       osCacheDmaOverflowDetectionErrorGet(...) to check if any of the 'Cached DMA'
*       caused 'overflow'
*
* INPUTS:
*       allocPtr - allocated pointer by osCacheDmaMalloc
*       numOfBytes - number of bytes to allocated by osCacheDmaMalloc
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_TRUE   - on error
*       GT_FALSE  - no error
* COMMENTS:
*       None
*
*******************************************************************************/
static GT_BOOL dmaOverflowDetectionGet
(
    IN GT_PTR   allocPtr ,
    IN GT_U32   numOfBytes
)
{
#if ERROR_IN_IMPLEMENTATION == 0
    GT_U32  ii,jj;
    GT_U32  *startOfOverflowDetectionRegionPtr;


    if(numOfBytes <= DMA_NUM_BYTES_OVERFLOW_DETECTION_REGION_CNS)
    {
        osPrintf("ERROR : dmaOverflowDetectionGet : numOfBytes[%d] <= [%d] \n" ,
            numOfBytes , DMA_NUM_BYTES_OVERFLOW_DETECTION_REGION_CNS);

        /* should not happen */
        return GT_TRUE;
    }

    /* the pointer and numOfBytes holds the 'DMA_NUM_BYTES_OVERFLOW_DETECTION_REGION_CNS'
       for the 'overflow detection' region at the end of each 'Cached DMA' .

        we need to initialize it with the pre-defined values.
    */

    startOfOverflowDetectionRegionPtr = (GT_U32*)((GT_CHAR*)allocPtr + (numOfBytes - DMA_NUM_BYTES_OVERFLOW_DETECTION_REGION_CNS));

    for(ii = 0 ; ii < DMA_NUM_LINES_OF_PRE_DEFINED_PATTERNS ; ii++)
    {
        for(jj = 0 ; jj < DMA_NUM_PRE_DEFINED_PATTERNS_CNS; jj++)
        {
            if((*startOfOverflowDetectionRegionPtr) != dmaPreDefinedPatternsArr[jj])
            {
                osPrintf("ERROR : dmaOverflowDetectionGet : ii=[%d] ,  jj=[%d] , read value[0x%8.8x] , expected [0x%8.8x] \n" ,
                    ii , jj , (*startOfOverflowDetectionRegionPtr) , dmaPreDefinedPatternsArr[jj]);


                osPrintf("dump start and end of memory \n");
                osPrintf("dump start memory \n");
                startOfOverflowDetectionRegionPtr = (GT_U32*)((GT_CHAR*)allocPtr);
                for(ii = 0 ; ii < 10 ; ii++)
                {
                    osPrintf("%2.2d : [0x%8.8x] [0x%8.8x] [0x%8.8x] [0x%8.8x] \n",
                        ii ,
                        startOfOverflowDetectionRegionPtr[0],
                        startOfOverflowDetectionRegionPtr[1],
                        startOfOverflowDetectionRegionPtr[2],
                        startOfOverflowDetectionRegionPtr[3]);

                    startOfOverflowDetectionRegionPtr += 4;
                }
                osPrintf("dump start memory -- ended \n");

                osPrintf("dump end memory \n");
                startOfOverflowDetectionRegionPtr = (GT_U32*)((GT_CHAR*)allocPtr + (numOfBytes - (16*10)));
                for(ii = 0 ; ii < 10 ; ii++)
                {
                    osPrintf("%2.2d : [0x%8.8x] [0x%8.8x] [0x%8.8x] [0x%8.8x] \n",
                        (numOfBytes / 16) + ii ,
                        startOfOverflowDetectionRegionPtr[0],
                        startOfOverflowDetectionRegionPtr[1],
                        startOfOverflowDetectionRegionPtr[2],
                        startOfOverflowDetectionRegionPtr[3]);

                    startOfOverflowDetectionRegionPtr += 4;
                }
                osPrintf("dump end memory -- ended \n");

                /* error detected */
                return GT_TRUE;
            }

            startOfOverflowDetectionRegionPtr++;
        }
    }
#endif /*ERROR_IN_IMPLEMENTATION == 0*/
    return GT_FALSE;
}

/*******************************************************************************
* prvOsCacheDmaMallocOverflowDetectionSupportGet
*
* DESCRIPTION:
*       the function checks if need to update the 'size' with number of bytes
*       used for 'overflow detection' region at the end of each 'Cached DMA'
*
* INPUTS:
*       sizePtr - (pointer to) the size of allocation given by the application.
*
* OUTPUTS:
*       sizePtr - (pointer to) the update size of needed allocation in case that
*                 needed extra bytes for 'overflow detection' region.
*
* RETURNS:
*       None.
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_VOID prvOsCacheDmaMallocOverflowDetectionSupportGet
(
    INOUT GT_U32    *sizePtr
)
{
#if ERROR_IN_IMPLEMENTATION == 0
    if(dmaOverflowDetectionEnabled == GT_TRUE)
    {
        (*sizePtr) += DMA_NUM_BYTES_OVERFLOW_DETECTION_REGION_CNS;
    }
#endif /*ERROR_IN_IMPLEMENTATION == 0*/
    return;
}


/*******************************************************************************
* osCacheDmaOverflowDetectionEnableSet
*
* DESCRIPTION:
*       enable the 'Cache DMA' manager to add 'overflow detection' region at
*       the end of each 'Cached DMA' (on top of the actual needed size) and to
*       initialize this region with 'predefined pattern' words.
*
*       this function allow the application to check via function
*       osCacheDmaOverflowDetectionErrorGet(...) to check if any of the 'Cached DMA'
*       caused 'overflow'
*
* INPUTS:
*       enable - GT_TRUE - enable the detection.
*                GT_FALSE - disable the detection.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK   - on success
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS osCacheDmaOverflowDetectionEnableSet
(
    IN GT_BOOL  enable
)
{
#if ERROR_IN_IMPLEMENTATION == 0
    dmaOverflowDetectionEnabled = enable;
#endif /*ERROR_IN_IMPLEMENTATION == 0*/
    return GT_OK;
}

/*******************************************************************************
* osCacheDmaOverflowDetectionErrorGet
*
* DESCRIPTION:
*       the function checks ALL if any of the 'Cached DMA' memories caused 'overflow'
*       and the function will return the pointers to the 'Cached DMA' memories with errors.
*
* INPUTS:
*       maxNumErrorsToGet - max number of errors that dmaMemoryWithErrorsArr[] can hold.
*                       NOTE: value 0 means that application not care about which
*                       memory caused the problem.
*       dmaMemoryWithErrorsArr[] - array of pointers to get the 'Cached DMA' memories with errors.
*                       NOTE : 1. this array can be NULL when maxNumErrorsToGet == 0
*                              2. this array must hold space for maxNumErrorsToGet 'Cached DMA' memories.
*                              3. if maxNumErrorsToGet is smaller then actual number of memories with errors,
*                                 then the rest of the memories are not filled into this array.
*
* OUTPUTS:
*       actualNumErrorsPtr - (pointer to) the actual number of 'Cached DMA' memories
*                           that caused 'overflow'.
*                       can be NULL if application not care about the number of errors.
*       dmaMemoryWithErrorsArr[] - array of pointers to the 'Cached DMA' memories with errors.
*
* RETURNS:
*       GT_OK - no memory caused error.
*       GT_ERROR - at least one of the 'Cached DMA' memories with errors.
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS osCacheDmaOverflowDetectionErrorGet
(
    IN    GT_U32  maxNumErrorsToGet,
    OUT   GT_U32  *actualNumErrorsPtr,
    INOUT   void*   dmaMemoryWithErrorsArr[]
)
{
    GT_STATUS   rc = GT_OK;
    GT_BOOL     gotError;
    DMA_ALLOCATION_STC *currentPtr;
    DMA_ALLOCATION_STC *nextPtr;
    GT_U32      currentErrorIndex = 0;

    if(dmaOverflowDetectionEnabled == GT_FALSE)
    {
        if(actualNumErrorsPtr)
        {
            *actualNumErrorsPtr = 0;
        }
        /* no errors can be detected ! */
        return GT_OK;
    }

    currentPtr = &globalDmaAllocations;

    while(currentPtr)
    {
        nextPtr = currentPtr->nextElementPtr;
        gotError = GT_FALSE;

        if(currentPtr->myMagic != MAGIC_NUMBER_CNS)
        {
            /* error */
            gotError = GT_TRUE;
            REPORT_CORRUPTED_MEMORY_MAC;
        }
        else
        if(currentPtr->myMemoryPtr)
        {
            /* check if this 'Cached DMA' memory caused 'overflow' */
            gotError = dmaOverflowDetectionGet(currentPtr->myMemoryPtr,currentPtr->myMemoryNumOfBytes);
        }

        if(gotError == GT_TRUE)
        {
            rc = GT_ERROR;

            /* check if application allocated space to get the memory indication */
            if((currentErrorIndex < maxNumErrorsToGet) && dmaMemoryWithErrorsArr)
            {
                /* let application info */
                dmaMemoryWithErrorsArr[currentErrorIndex] = currentPtr->myMemoryPtr;
            }

            /* increment the number of errors */
            currentErrorIndex++;

            if(currentPtr->myMagic != MAGIC_NUMBER_CNS)
            {
                /* we CAN'T go on the link list !!! it is corrupted !!! */
                break;
            }
        }

        /* update the 'current' to be 'next' */
        currentPtr = nextPtr;
    }

    if(actualNumErrorsPtr)
    {
        *actualNumErrorsPtr = currentErrorIndex;
    }

    return rc;
}


