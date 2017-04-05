/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* osLinuxMem.c
*
* DESCRIPTION:
*       Linux Operating System wrapper. Memory manipulation facility.
*
* DEPENDENCIES:
*       Linux, CPU independed.
*
* FILE REVISION NUMBER:
*       $Revision: 35 $
*******************************************************************************/
/***** Includes ********************************************************/
/* IMPORTANT : we must remove the declaration of OS_MALLOC_MEMORY_LEAKAGE_DBG */
/* because we not want local calls to osMalloc to be affected by the H file definitions */
#define FORCE_IGNORE_OS_MALLOC_MEMORY_LEAKAGE_DBG

#define _GNU_SOURCE
#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include <gtOs/gtOsMem.h>
#include <gtOs/gtOsSem.h>
#include <gtOs/gtOsGen.h>
#include <gtOs/gtOsIo.h>
#include <gtOs/gtOsExc.h>
#include <gtOs/gtOsSharedMalloc.h>
#include <gtOs/gtOsSharedData.h>
#if __WORDSIZE == 64
# include <sys/mman.h>
#endif
#if defined(USE_VALGRIND) && !defined(SHARED_MEMORY)
#include <valgrind/valgrind.h>
#endif /* USE_VALGRIND */

#ifdef LINUX_SIM
#define EXPLICIT_INCLUDE_TO_SIM_OS_H_FILES
#include <os/simTypesBind.h>
#include <os/simOsProcess.h>

#include <asicSimulation/SDistributed/sdistributed.h>
#include <asicSimulation/SInit/sinit.h>
#endif
#include <gtExtDrv/drivers/prvExtDrvLinuxMapping.h>


/*********  important ****  important  ***  important ****  important **********
NOTE: in this file the OS_MALLOC_MEMORY_LEAKAGE_DBG is NEVER defined
    instead this file must check
    INTERNAL_OS_MALLOC_MEMORY_LEAKAGE_DBG
    instead of
    OS_MALLOC_MEMORY_LEAKAGE_DBG
*********  important ****  important  ***  important ****  important **********/


/***** Function Declaration ********************************************/
#ifdef LINUX_SIM
static GT_VOID* dmaAsSharedMemoryMalloc(IN GT_U32 size);
#endif

/***** External Functions **********************************************/
GT_VOID *osMemPoolsAlloc(GT_U32 size);
GT_VOID *osMemPoolsRealloc(GT_VOID *ptr, GT_U32 size);
GT_VOID osMemPoolFree(GT_VOID *memBlock);
GT_VOID *osStaticMallocFromChunk(GT_U32 size);

extern GT_VOID prvOsCacheDmaMallocOverflowDetectionSupportGet
(
    INOUT GT_U32    *sizePtr
);

extern GT_STATUS osCacheDmaRegisterPtr(
    IN GT_VOID *    allocPtr,
    IN GT_U32       numOfBytes
);
extern void osCacheDmaUnregisterPtr(
    IN GT_VOID *    oldPointer
);
extern GT_STATUS osCacheDmaSystemReset(void);
#ifdef INTERNAL_OS_MALLOC_MEMORY_LEAKAGE_DBG
extern GT_STATUS osMemRegisterPtr(
    IN GT_VOID *    allocPtr,
    IN GT_U32       numOfBytes,
    IN const char*    fileNamePtr,
    IN GT_U32   line
);
extern void osMemUnregisterPtr(
    IN GT_VOID *    oldPointer,
    IN const char*    fileNamePtr,
    IN GT_U32   line
);
#else /*!INTERNAL_OS_MALLOC_MEMORY_LEAKAGE_DBG*/
/* define empty macro to reduce management when not needed */
#define osMemRegisterPtr(a,b,c)
#define osMemUnregisterPtr(a,b)
#endif /*!INTERNAL_OS_MALLOC_MEMORY_LEAKAGE_DBG*/

/***** Global Vars ********************************************/

GT_BOOL  firstInitialization __SHARED_DATA_MAINOS = GT_TRUE;

#ifndef LINUX_SIM
extern GT_32 gtPpFd;

#define DMA_MEM_MAGIC  0x87654321
static GT_MUTEX freeDmaPoolMtx __SHARED_DATA_MAINOS;
static OSMEM_POOL_FREE_ELEM_PTR freeDmaPool __SHARED_DATA_MAINOS = NULL;
#endif

#ifdef LINUX_SIM
/* a number that indicate the 'key' representing the shared memory */
#define SIM_DMA_AS_SHARED_MEM_KEY_CNS   5678

/* id of the shared memory */
static GT_SH_MEM_ID simDmaAsSharedMemId __SHARED_DATA_MAINOS = 0;

static GT_MUTEX simDmaMutexId __SHARED_DATA_MAINOS;
/* pointer to the start of the shared memory */
static GT_VOID* simDmaAsSharedMemPtr __SHARED_DATA_MAINOS=NULL;

/* size of the shared memory */
static GT_U32 simDmaAsSharedMemSize __SHARED_DATA_MAINOS = 0;

/* offset ued from start of the shared memory  */
static GT_U32 simDmaAsSharedMemOffsetUsed __SHARED_DATA_MAINOS = 0;
#endif /* LINUX_SIM */

SHARED_MEM_STC cdma_mem __SHARED_DATA_MAINOS;
GT_UINTPTR baseAddr __SHARED_DATA_MAINOS = 0;
GT_UINTPTR baseVirtAddr __SHARED_DATA_MAINOS = LINUX_VMA_DMABASE;
GT_U32 dmaSize __SHARED_DATA_MAINOS = 2 * (1024 * 1024); /*default value*/

/***** Public Functions ************************************************/
#define SHARED_MALLOC32_MAC SHARED_MALLOC_MAC
#define SHARED_FREE32_MAC SHARED_FREE_MAC

#ifndef SHARED_MEMORY
/* shrMemSharedMalloc always allocate in first 2Gig address space */
#if __WORDSIZE == 64
/* 32bit works ok, workarounds for 64 bit */
#undef SHARED_MALLOC32_MAC
#undef SHARED_FREE32_MAC

#if defined(MIPS64_CPU) || defined(INTEL64_CPU)
#define ADV64_CPU
#endif

#ifndef ADV64_CPU
/* workaround: allocate in first 2Gig address space */
static GT_VOID* malloc_32bit(unsigned int size)
{
    size_t *ptr;
    ptr = mmap(NULL, size + sizeof(size_t),
        PROT_READ | PROT_WRITE,
        MAP_32BIT | MAP_ANONYMOUS | MAP_PRIVATE,
        0, 0);
    if (ptr)
    {
        *ptr = size;
        ptr++;
    }
    return ptr;
}
static GT_VOID free_32bit(GT_VOID* ptr)
{
    size_t *sptr = (size_t*)ptr;
    if (sptr)
    {
        sptr--;
        munmap(sptr, *sptr + sizeof(size_t));
    }
}
#ifdef ALL_ALLOCS_32BIT
static GT_VOID* realloc_32bit(GT_VOID* ptr, size_t newsize)
{
    size_t *sptr = (size_t*)ptr;
    GT_VOID   *newptr;
    if (ptr == NULL)
        return malloc_32bit(newsize);
    sptr--;
    if (newsize == *sptr)
    {
        return ptr;
    }
    newptr = malloc_32bit(newsize);
    memcpy(newptr, ptr, (*sptr < newsize) ? *sptr : newsize);
    free_32bit(ptr);
    return newptr;
}
#define SHARED_REALLOC32_MAC realloc_32bit
#endif /* ALL_ALLOCS_32BIT */

#define SHARED_MALLOC32_MAC malloc_32bit
#define SHARED_FREE32_MAC free_32bit



#else /* defined(ADV64_CPU) */
/* && !defined SHARED_MEM */
extern int adv64_malloc_32bit;
#include <gtOs/gtOsSharedMemoryRemapper.h>

GT_VOID* malloc_32bit_adv64(unsigned int size)
{
  static GT_BOOL firstTime = GT_TRUE;
  size_t *ptr;
  static GT_UINTPTR addr = (GT_UINTPTR)0;
  static GT_UINTPTR end = (GT_UINTPTR)0;
  void *ptr1;

  if (!adv64_malloc_32bit) /* set indirectly by pssBspApis.c and prestera.c .
                              Default is zero */
  {
    ptr1 = malloc(size);
    if (firstTime)
    {
#if 0
      osPrintf("adv64_malloc_32bit is off. Ret-ptr = 0x%lx\n", ptr1);
#endif
      firstTime = GT_FALSE;
    }
    return ptr1;
  }


  if (addr == (GT_UINTPTR)0)
  {
    /* allocate memory at fixed address */
    void *map;
    map = mmap((void*)SHARED_MEMORY_MALLOC_VIRT_ADDR_MAC, 64*1024*1024,
        PROT_READ | PROT_WRITE,
        MAP_FIXED | MAP_ANONYMOUS | MAP_PRIVATE,
        0, 0);
    if (MAP_FAILED == map)
    {
        osPrintf("adv64_malloc_32bit mmap failed: %s\n", strerror(errno));
        addr = !((GT_UINTPTR)0);
        end = addr;
    }
    else
    {
        addr = (GT_UINTPTR)map;
        end = addr + 64*1024*1024;
    }
  }

  if ((end - addr) < size)
  {
      return NULL;
  }

  ptr = (size_t*)addr;
  if (ptr)
  {
    *ptr = size;
    ptr++;
  }

  addr = (GT_UINTPTR)ptr + size;


  if (firstTime)
  {
    osPrintf("adv64_malloc_32bit is on. Ret-ptr = 0x%lx\n", ptr);
    firstTime = GT_FALSE;
  }
  return ptr;
}

#if defined(ALL_ALLOCS_32BIT) || defined(LINUX_SIM)
static GT_VOID free_32bit_adv64(GT_VOID* ptr)
{
  if (!adv64_malloc_32bit)
    free(ptr);

  /* else do nothing */
}
#endif

#ifdef ALL_ALLOCS_32BIT
static GT_VOID* realloc_32bit_adv64(GT_VOID* ptr, size_t newsize)
{
  size_t *sptr = (size_t*)ptr;
  GT_VOID   *newptr;
  if (ptr == NULL)
    return malloc_32bit_adv64(newsize);
  sptr--;
  if (newsize == *sptr)
  {
    return ptr;
  }
  newptr = malloc_32bit_adv64(newsize);
  memcpy(newptr, ptr, (*sptr < newsize) ? *sptr : newsize);
  free_32bit_adv64(ptr);
  return newptr;
}
#define SHARED_REALLOC32_MAC realloc_32bit_adv64
#endif /* ALL_ALLOCS_32BIT */

#define SHARED_MALLOC32_MAC malloc_32bit_adv64
#define SHARED_FREE32_MAC free_32bit_adv64
#endif /* defined(ADV64_CPU) */



/* redefine for 64bit */
#ifdef ALL_ALLOCS_32BIT
#  undef SHARED_MALLOC_MAC
#  undef SHARED_FREE_MAC
#  define SHARED_MALLOC_MAC     SHARED_MALLOC32_MAC
#  define SHARED_FREE_MAC       SHARED_FREE32_MAC
#  ifdef SHARED_REALLOC32_MAC
#    define realloc             SHARED_REALLOC32_MAC
#  endif
#endif

#endif /* __WORDSIZE == 64 */
#endif /* !defined(SHARED_MEMORY) */

/*******************************************************************************
* osMemInit
*
* DESCRIPTION:
*       Initialize the memory management.
*
* INPUTS:
*       size            - memory size to manage.
*       reallocOnDemand - defines whether in the case of memory chunk depleation
*                         a new chunk will be allocated or an out of memory will
*                         be reported.
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
GT_STATUS osMemInit
(
    IN GT_U32 size,
    IN GT_BOOL reallocOnDemand
)
{
    GT_MEMORY_MANAGE_METHOD_ENT memAllocMethod;
    GT_STATUS ret;

    if (reallocOnDemand == GT_TRUE)
    {
        memAllocMethod = GT_MEMORY_MANAGE_METHOD_REALLOC_ON_DEMAND_E;
    }
    else
    {
        memAllocMethod = GT_MEMORY_MANAGE_METHOD_NO_REALLOC_ON_DEMAND_E;
    }

#ifndef LINUX_SIM
    /* Create the simDma semaphore */
    if (osMutexCreate("osFreeDmaPool", &freeDmaPoolMtx) != GT_OK)
    {
        return GT_FAIL;
    }
#endif
#ifdef LINUX_SIM
    /* Create the simDma semaphore */
    if (osMutexCreate("osSimDma", &simDmaMutexId) != GT_OK)
    {
        return GT_FAIL;
    }
    if(sasicgSimulationRole == SASICG_SIMULATION_ROLE_DISTRIBUTED_APPLICATION_SIDE_VIA_BROKER_E)
    {
        /* the application side that work with broker may decide how the broker
           will handle DMA read/write requests:
            1. use shared memory and do read/write by itself.
            2. use socket to send to application the request from read/write DMA
        */

        if(brokerDmaMode == BROKER_NOT_USE_SOCKET)
        {
            /* steal 1/2 of memory for DMA */
            if(NULL == dmaAsSharedMemoryMalloc(size/2))
            {
                return GT_NO_RESOURCE;
            }

            /* send DMA registration info */
            simDistributedRegisterDma(
                    (GT_U32)((GT_UINTPTR)simDmaAsSharedMemPtr),/* the address of the pointer in our memory */
                    simDmaAsSharedMemSize,/* the size of shared memory */
                    SIM_DMA_AS_SHARED_MEM_KEY_CNS,/* the key to retrieve the shared memory */
                    BROKER_DMA_MODE_SHARED_MEMORY);

            /* use other 1/2 of memory for non-DMA */
            size = size / 2;
        }
        else if(brokerDmaMode == BROKER_USE_SOCKET)
        {
            /* send DMA registration info -- we work in socket mode */
            simDistributedRegisterDma(0,/* don't care */
                                      0,/* don't care */
                                      0,/* don't care */
                                      BROKER_DMA_MODE_SOCKET);
        }
        else
        {
            return GT_BAD_STATE;
        }
    }
#endif
    ret = osMemLibInit(
            (GT_MEMORY_ALLOC_FUNC)SHARED_MALLOC_MAC,
            SHARED_FREE_MAC,size,
            memAllocMethod);
    firstInitialization = GT_FALSE;
    return ret;
}

/*******************************************************************************
* osStaticMalloc
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
GT_VOID *osStaticMalloc
(
    IN GT_U32 size
)
{
    void *ptr;
    if (firstInitialization == GT_TRUE)
    {
        return osMalloc(size);
    }
    ptr = osStaticMallocFromChunk(size);
    osMemRegisterPtr(ptr,size,LOCAL_DEBUG_INFO);
    return ptr;
}

/*******************************************************************************
* internalOsMalloc
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
*       insufficient memory available
*
* COMMENTS:
*       Usage of this function is NEVER during initialization.
*
*******************************************************************************/
GT_VOID *internalOsMalloc
(
    IN GT_U32 size
)
{
    IS_WRAPPER_OPEN_PTR;

    if( size == 0) return NULL;

    if (firstInitialization == GT_TRUE)
    {
        OSMEM_NOPOOLS_HDR_STC *hdrPtr;
        hdrPtr = (OSMEM_NOPOOLS_HDR_STC*)SHARED_MALLOC_MAC(size+sizeof(OSMEM_NOPOOLS_HDR_STC));
        if (hdrPtr == NULL)
        {
            return NULL;
        }
        hdrPtr->magic = (GT_UINTPTR)hdrPtr;
        hdrPtr->size = size;
        hdrPtr++;
        return (GT_VOID*)hdrPtr;
    }
    else
    {
        return osMemPoolsAlloc(size);
    }
}

/*******************************************************************************
* osMalloc
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
*       insufficient memory available
*
* COMMENTS:
*       Usage of this function is NEVER during initialization.
*
*******************************************************************************/
GT_VOID *osMalloc
(
    IN GT_U32 size
)
{
    void *ptr;
#if defined(USE_VALGRIND) && !defined(SHARED_MEMORY)
    if (RUNNING_ON_VALGRIND)
    {
        return malloc(size);
    }
#endif
    ptr = internalOsMalloc(size);

    osMemRegisterPtr(ptr,size,LOCAL_DEBUG_INFO);
    return ptr;
}
/*******************************************************************************
* internalOsRealloc
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
GT_VOID *internalOsRealloc
(
    IN GT_VOID * ptr ,
    IN GT_U32 size
)
{
    OSMEM_NOPOOLS_HDR_STC *oldHdr;
    IS_WRAPPER_OPEN_PTR;

    if (ptr == NULL)
    {
        return osMalloc(size);
    }

    oldHdr = (OSMEM_NOPOOLS_HDR_STC*)((GT_UINTPTR)ptr - sizeof(OSMEM_NOPOOLS_HDR_STC));
    if (firstInitialization == GT_TRUE)
    {
        OSMEM_NOPOOLS_HDR_STC *allocPtr;
#if defined(SHARED_MEMORY) || !defined(LINUX_SIM)
        if (oldHdr->magic != (GT_UINTPTR)oldHdr)
        {
            osFatalError(OS_FATAL_WARNING, "osRealloc(): trying to realloc wrong memory\n");
            return NULL;
        }

#if 0
        osPrintf("Warning: osRealloc before initialization\n");
#endif

        allocPtr = (OSMEM_NOPOOLS_HDR_STC*)SHARED_MALLOC_MAC(size+sizeof(OSMEM_NOPOOLS_HDR_STC));
        if (allocPtr == NULL)
        {
            return NULL;
        }
        osMemCpy(allocPtr+1,ptr,oldHdr->size < size ? oldHdr->size : size);
        SHARED_FREE_MAC(oldHdr);
#else
        allocPtr = realloc(oldHdr, size+sizeof(OSMEM_NOPOOLS_HDR_STC));
        if (allocPtr == NULL)
        {
            return NULL;
        }
#endif
        allocPtr->magic = (GT_UINTPTR)allocPtr;
        allocPtr->size = size;
        return (GT_VOID*)(allocPtr + 1);

    }
    else
    {
        if (oldHdr->magic == (GT_UINTPTR)oldHdr)
        {
            /* realloc memory allocated by malloc() */
            GT_VOID *newPtr = osMalloc(size);
            if (newPtr == NULL)
            {
                return NULL;
            }
            osMemCpy(newPtr, ptr, oldHdr->size < size ? oldHdr->size : size);
#ifdef LINUX_SIM
            SHARED_FREE_MAC(oldHdr);
#endif
            return newPtr;

        }
        return osMemPoolsRealloc(ptr, size);
    }
}
/*******************************************************************************
* osRealloc
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
GT_VOID *osRealloc
(
    IN GT_VOID * ptr ,
    IN GT_U32 size
)
{
    void *newPtr;
#if defined(USE_VALGRIND) && !defined(SHARED_MEMORY)
    if (RUNNING_ON_VALGRIND)
    {
        return realloc(ptr,size);
    }
#endif
    newPtr = internalOsRealloc(ptr,size);

    /* unregister old ptr */
    osMemUnregisterPtr(ptr,LOCAL_DEBUG_INFO);
    /* register new ptr */
    osMemRegisterPtr(newPtr,size,LOCAL_DEBUG_INFO);
    return newPtr;
}

/*******************************************************************************
* internalOsFree
*
* DESCRIPTION:
*       Deallocates or frees a specified memory block.
*
* INPUTS:
*       memblock - previously allocated memory block to be freed
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
GT_VOID internalOsFree
(
    IN GT_VOID* const memblock
)
{
    OSMEM_NOPOOLS_HDR_STC *oldHdr = NULL;
    assert(memblock != NULL);
    oldHdr = (OSMEM_NOPOOLS_HDR_STC*)((GT_UINTPTR)memblock - sizeof(OSMEM_NOPOOLS_HDR_STC));
    if (firstInitialization == GT_TRUE)
    {
        if (oldHdr->magic != (GT_UINTPTR)oldHdr)
        {
            osFatalError(OS_FATAL_WARNING, "osFree(): bad magic");
        }
        else
        {
#ifdef LINUX_SIM
            SHARED_FREE_MAC(oldHdr);
#endif
        }
    }
    else
    {
        /* check if this free is trying to free something that wasn't
           allocated by owr pools manager */
        if (oldHdr->magic == (GT_UINTPTR)oldHdr)
        {
/*#ifdef OSMEMPOOLS_DEBUG
            osPrintf("Warning:Trying to Free a pointer allocated by malloc after osMemInit !\n");
#endif bothers CLI */

#ifdef LINUX_SIM
            SHARED_FREE_MAC(oldHdr);
#endif
        }
        else
        {
            osMemPoolFree(memblock);
        }
    }
    return;
}

/*******************************************************************************
* osFree
*
* DESCRIPTION:
*       Deallocates or frees a specified memory block.
*
* INPUTS:
*       memblock - previously allocated memory block to be freed
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
GT_VOID osFree
(
    IN GT_VOID* const memblock
)
{
#if defined(USE_VALGRIND) && !defined(SHARED_MEMORY)
    if (RUNNING_ON_VALGRIND)
    {
        free(memblock);
        return;
    }
#endif
    internalOsFree(memblock);
    osMemUnregisterPtr(memblock,LOCAL_DEBUG_INFO);
}

/*******************************************************************************
* osCacheDmaMalloc
*
* DESCRIPTION:
*       Allocate a cache-safe buffer  of specified size for DMA devices
*       and drivers
*
* INPUTS:
*       size - bytes to allocate
*
* OUTPUTS:
*       None
*
* RETURNS:
*       A pointer to the cache-safe buffer, or NULL
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_VOID *osCacheDmaMalloc
(
    IN GT_U32 size
)
{
    GT_VOID *ptr;
    IS_WRAPPER_OPEN_PTR;

#ifndef LINUX_SIM
    if (size == 0)
        return NULL;
#endif

    /* check if need to get modified size to hold support for 'overflow detection' */
    prvOsCacheDmaMallocOverflowDetectionSupportGet(&size);

#ifndef LINUX_SIM
    {
        OSMEM_POOL_FREE_ELEM_PTR freePtr;
        OSMEM_POOL_FREE_ELEM_PTR *prev;
        OSMEM_NOPOOLS_HDR_STC *hdrPtr = NULL;

        osMutexLock(freeDmaPoolMtx);
        freePtr = freeDmaPool;
        prev = &freeDmaPool;
        while (freePtr != NULL)
        {
            if (freePtr->size == size)
                break;
            prev = &(freePtr->next_elem);
            freePtr = freePtr->next_elem;
        }
        if (freePtr)
        {
            /* remove from chain */
            *prev = freePtr->next_elem;
            hdrPtr = (OSMEM_NOPOOLS_HDR_STC*)freePtr;
        }
        osMutexUnlock(freeDmaPoolMtx);
        if (!freePtr)
        {
fprintf(stderr,"Allocating %d bytes for DMA\n", size);
            hdrPtr = (OSMEM_NOPOOLS_HDR_STC*)shrMemSharedMallocIntoBlock(
                    &cdma_mem, gtPpFd, size+sizeof(OSMEM_NOPOOLS_HDR_STC));

        }
        if (hdrPtr == NULL)
        {
fprintf(stderr,"Failed to allocate %d bytes, exiting\n", size);
exit(1);
            return NULL;
        }
        hdrPtr->magic = DMA_MEM_MAGIC;
        hdrPtr->size = size;
        hdrPtr++;
        ptr = hdrPtr;
    }
#else
    if(simDmaAsSharedMemSize)
    {
        GT_CHAR* tmpPtr; /* tmp pointer to start of memory to return */
        /* take sem */
        osMutexLock(simDmaMutexId);

        /* check if we have enough space for the new memory request */
        if((simDmaAsSharedMemOffsetUsed + size) > simDmaAsSharedMemSize)
        {
            /* free sem */
            osMutexUnlock(simDmaMutexId);
            return NULL;
        }

        tmpPtr = (GT_CHAR*)simDmaAsSharedMemPtr + simDmaAsSharedMemOffsetUsed;

        simDmaAsSharedMemOffsetUsed += size;

        /* free sem */
        osMutexUnlock(simDmaMutexId);

        ptr = tmpPtr;
    }
    else
    {
        ptr = SHARED_MALLOC32_MAC(size) ;
    }
#endif

    /* register the dma pointer for system reset */
    osCacheDmaRegisterPtr(ptr,size);

    return ptr;
}

/*******************************************************************************
* osCacheDmaFree
*
* DESCRIPTION:
*       Free the buffer acquired with osCacheDmaMalloc()
*
* INPUTS:
*       ptr - pointer to malloc/free buffer
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on error
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS osCacheDmaFree
(
    IN GT_VOID *ptr
)
{
    IS_WRAPPER_OPEN_STATUS;

    /* unregister the ptr */
    (void)osCacheDmaUnregisterPtr(ptr);

#ifndef LINUX_SIM
    {
        OSMEM_NOPOOLS_HDR_STC *oldHdr;
        OSMEM_POOL_FREE_ELEM_PTR freePtr;

        if (ptr == NULL)
        {
            return GT_FAIL;
        }

        oldHdr = (OSMEM_NOPOOLS_HDR_STC*)((GT_UINTPTR)ptr - sizeof(OSMEM_NOPOOLS_HDR_STC));
        if (oldHdr->magic != DMA_MEM_MAGIC)
        {
            return GT_FAIL;
        }
        freePtr = (OSMEM_POOL_FREE_ELEM_PTR)oldHdr;
        freePtr->size = oldHdr->size;
        /* put to chain */
        osMutexLock(freeDmaPoolMtx);
        freePtr->next_elem = freeDmaPool;
        freeDmaPool = freePtr;
        osMutexUnlock(freeDmaPoolMtx);
    }
#endif
#ifdef LINUX_SIM
    if(simDmaAsSharedMemSize)
    {
        /* not supporting free of this memory */
        return GT_OK;
    }
    SHARED_FREE32_MAC(ptr) ;
#endif

  return GT_OK;
}

/*******************************************************************************
* osCacheDmaFreeAll
*
* DESCRIPTION:
*       Free ALL DMA memory
*
* INPUTS:
*       None
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on error
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS osCacheDmaFreeAll(GT_VOID)
{
    IS_WRAPPER_OPEN_STATUS;
#ifndef LINUX_SIM
    /* reset full DMA in one swipe */
    cdma_mem.curr = cdma_mem.base;
    osMutexLock(freeDmaPoolMtx);
    freeDmaPool = NULL;
    osMutexUnlock(freeDmaPoolMtx);
#endif
    /* let the manager clear the DB one by one (will be calling osCacheDmaFree(...))*/
    return osCacheDmaSystemReset();
}

/*******************************************************************************
* osPhy2Virt
*
* DESCRIPTION:
*       Converts physical address to virtual.
*
* INPUTS:
*       phyAddr  - physical address
*
* OUTPUTS:
*       virtAddr - virtual address
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on error
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS osPhy2Virt
(
    IN  GT_UINTPTR phyAddr,
    OUT GT_UINTPTR *virtAddr
)
{
    IS_WRAPPER_OPEN_STATUS;

#ifndef LINUX_SIM
    if (baseAddr == 0xffffffff)
    {
        osPrintf("osPhy2Virt:ERROR,baseAddr(0x%0x)\n", (int)baseAddr);
        return GT_FAIL;
        }

    if (phyAddr >= baseAddr && phyAddr < baseAddr + dmaSize)
    {
        *virtAddr = baseVirtAddr + (phyAddr - baseAddr);
        return GT_OK;
    }
    else
    {
        fprintf(stderr, "osPhy2Virt:ERROR,phyAddr(0x%0x)\n", (int)phyAddr);
        *virtAddr = 0xffffffff;
        osPrintf("osPhy2Virt:ERROR\n");
        return GT_FAIL;
    }
#else
    *virtAddr = phyAddr;
    return GT_OK;
#endif
}

/*******************************************************************************
* osVirt2Phy
*
* DESCRIPTION:
*       Converts virtual address to physical.
*
* INPUTS:
*       virtAddr - virtual address
*
* OUTPUTS:
*       phyAddr  - physical address
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on error
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS osVirt2Phy
(
    IN  GT_UINTPTR virtAddr,
    OUT GT_UINTPTR *phyAddr
)
{
    IS_WRAPPER_OPEN_STATUS;

#ifndef LINUX_SIM
    if (baseAddr == 0xffffffff)
    {
        /*ofPrintf(stderr, "osVirt2Phy:ERROR,baseAddr(0x%0x)\n", (int)baseAddr);*/
        return GT_FAIL;
    }

    if(virtAddr >= baseVirtAddr &&
       virtAddr < (baseVirtAddr+dmaSize))
    {
        *phyAddr = baseAddr + (virtAddr - baseVirtAddr);
        /*osPrintf("\nosVirt2Phy: *phyAddr 0x%X\n",*phyAddr);*/
        return GT_OK;
    }
    else
    {
        fprintf(stderr, "osVirt2Phy:ERROR,virtAddr(0x%0x)\n", (int)virtAddr);
        *phyAddr = 0xffffffff;
        /*osPrintf("osVirt2Phy:ERROR2\n");*/
        return GT_FAIL;
    }
#else
    *phyAddr = virtAddr;
    return GT_OK;
#endif
}

/***** Private ********************************************************/

#ifdef LINUX_SIM
/*******************************************************************************
* dmaAsSharedMemoryMalloc
*
* DESCRIPTION:
*       Allocates shared memory block of specified size, this shared memory will
*       be used for 'DMA memory'
*
* INPUTS:
*       size - bytes to allocate
*
* OUTPUTS:
*       None
*
* RETURNS:
*       pointer to the shared memory (on NULL on fail)
*
* COMMENTS:
*       Usage of this function is only on FIRST initialization.
*       used only when distributed simulation of application side is working
*       with broker , in mode of DMA as shared memory(shared between the
*       broker and the application).
*
*******************************************************************************/
static GT_VOID* dmaAsSharedMemoryMalloc
(
    IN GT_U32 size
)
{
    GT_STATUS   rc;

    rc = simOsSharedMemGet((GT_SH_MEM_KEY)SIM_DMA_AS_SHARED_MEM_KEY_CNS,size,0,&simDmaAsSharedMemId);
    if(rc != GT_OK)
    {
        return NULL;
    }

    /* Now we attach (map) the shared memory to our process memory */
    simDmaAsSharedMemPtr = simOsSharedMemAttach(simDmaAsSharedMemId,
            NULL ,/* NULL means we attach it to no specific memory , we let OS to decide */
            0);/*no specific flags*/

    if(simDmaAsSharedMemPtr)
    {
        simDmaAsSharedMemSize = size;
    }

    return simDmaAsSharedMemPtr;
}
#endif /* LINUX_SIM */
