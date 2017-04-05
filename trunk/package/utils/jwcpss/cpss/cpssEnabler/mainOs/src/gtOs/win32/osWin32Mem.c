/*******************************************************************************
*              Copyright 2001, GALILEO TECHNOLOGY, LTD.
*
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL. NO RIGHTS ARE GRANTED
* HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT OF MARVELL OR ANY THIRD
* PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE DISCRETION TO REQUEST THAT THIS
* CODE BE IMMEDIATELY RETURNED TO MARVELL. THIS CODE IS PROVIDED "AS IS".
* MARVELL MAKES NO WARRANTIES, EXPRESS, IMPLIED OR OTHERWISE, REGARDING ITS
* ACCURACY, COMPLETENESS OR PERFORMANCE. MARVELL COMPRISES MARVELL TECHNOLOGY
* GROUP LTD. (MTGL) AND ITS SUBSIDIARIES, MARVELL INTERNATIONAL LTD. (MIL),
* MARVELL TECHNOLOGY, INC. (MTI), MARVELL SEMICONDUCTOR, INC. (MSI), MARVELL
* ASIA PTE LTD. (MAPL), MARVELL JAPAN K.K. (MJKK), GALILEO TECHNOLOGY LTD. (GTL)
* AND GALILEO TECHNOLOGY, INC. (GTI).
********************************************************************************
* osWin32Mem.c
*
* DESCRIPTION:
*       Win32 Operating System Simulation. Memory manipulation facility.
*
* DEPENDENCIES:
*       Win32, CPU independed.
*
* FILE REVISION NUMBER:
*       $Revision: 24 $
*******************************************************************************/
/* IMPORTANT : we must remove the declaration of OS_MALLOC_MEMORY_LEAKAGE_DBG */
/* because we not want local calls to osMalloc to be affected by the H file definitions */
#define FORCE_IGNORE_OS_MALLOC_MEMORY_LEAKAGE_DBG

#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include <gtOs/gtOsMem.h>
#include <gtOs/gtOsGen.h>
#include <gtOs/gtOsSem.h>
#include <gtOs/gtOsExc.h>

/*********  important ****  important  ***  important ****  important **********
NOTE: in this file the OS_MALLOC_MEMORY_LEAKAGE_DBG is NEVER defined
    instead this file must check
    INTERNAL_OS_MALLOC_MEMORY_LEAKAGE_DBG
    instead of
    OS_MALLOC_MEMORY_LEAKAGE_DBG
*********  important ****  important  ***  important ****  important **********/

/* include simulation H files for shared memory issues */
/* next macro defined so the file of <os/simOsProcess.h> will not #error us */
#define EXPLICIT_INCLUDE_TO_SIM_OS_H_FILES
#include <os/simTypesBind.h>
#include <os/simOsProcess.h>
#include <asicSimulation/SDistributed/sdistributed.h>
#include <asicSimulation/SInit/sinit.h>

/***** External Functions **********************************************/
extern int osPrintf(const char* format, ...);
void *osMemPoolsAlloc(GT_U32 size);
void *osMemPoolsRealloc(void *ptr, GT_U32 size);
void osMemPoolFree(void *memBlock);
void *osStaticMallocFromChunk(GT_U32 size);

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
/***** Static functions ************************************************/

/***** Static variables ************************************************/
/* heap allocation measurement static variables */

GT_BOOL  firstInitialization = GT_TRUE;

/***** Public Functions ************************************************/
/* a number that indicate the 'key' representing the shared memory */
#define SIM_DMA_AS_SHARED_MEM_KEY_CNS   5678

/* id of the shared memory */
static GT_SH_MEM_ID simDmaAsSharedMemId = 0;

static GT_MUTEX simDmaMutexId;
/* pointer to the start of the shared memory */
static void* simDmaAsSharedMemPtr=NULL;

/* size of the shared memory */
static GT_U32 simDmaAsSharedMemSize = 0;

/* offset ued from start of the shared memory  */
static GT_U32 simDmaAsSharedMemOffsetUsed = 0;

/* start protect win32 calles that uses internally semaphore ,
    this to avoid deadlock when a task in inside win32 function that takes
    semaphore and we 'lock' this task from other task that may also call win32 for
    same/other function and will be locked because win32 semaphore is not released */
/*
protect next:
    ClearCommError , ReadFile , WriteFile

*/
#define PROTECT_WIN32_CALL_MAC(win32FuncWithParameters) \
    scibAccessLock();                                   \
    win32FuncWithParameters;                            \
    scibAccessUnlock()


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
static void* dmaAsSharedMemoryMalloc
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

#if __WORDSIZE == 64
static GT_VOID* malloc64_wrapper(unsigned int size)
{
    return malloc((size_t)size);
}
#endif

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
            simDistributedRegisterDma((GT_U32)simDmaAsSharedMemPtr,/* the address of the pointer in our memory */
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

#if __WORDSIZE == 64
    ret = osMemLibInit(malloc64_wrapper,free,size, memAllocMethod);
#else
    ret = osMemLibInit(malloc,free,size, memAllocMethod);
#endif
    firstInitialization = GT_FALSE;
    return ret;
}

/*******************************************************************************
* osStaticAlloc
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
void *osStaticMalloc
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
    IN GT_U32   size
)
{
    if( size == 0) return NULL;

    if (firstInitialization == GT_TRUE)
    {
        /* during first init allocate from the OS */
        OSMEM_NOPOOLS_HDR_STC *hdrPtr;
        hdrPtr = (OSMEM_NOPOOLS_HDR_STC*)malloc(size+sizeof(OSMEM_NOPOOLS_HDR_STC));
        if (hdrPtr == NULL)
        {
            return NULL;
        }
        hdrPtr->magic = (GT_UINTPTR)hdrPtr;
        hdrPtr->size = size;
        hdrPtr++;
        return (GT_VOID*)hdrPtr;
    }

    return osMemPoolsAlloc(size);
}

/*******************************************************************************
*   internalOsRealloc
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
    IN GT_U32    size
)
{
    OSMEM_NOPOOLS_HDR_STC *oldHdr;

    if (ptr == NULL)
    {
        return osMalloc(size);
    }

    oldHdr = (OSMEM_NOPOOLS_HDR_STC*)((GT_UINTPTR)ptr - sizeof(OSMEM_NOPOOLS_HDR_STC));
    if (firstInitialization == GT_TRUE)
    {
        OSMEM_NOPOOLS_HDR_STC *allocPtr;
        if (oldHdr->magic != (GT_UINTPTR)oldHdr)
        {
            osFatalError(OS_FATAL_WARNING, "osRealloc(): bad magic");
        }
        allocPtr = realloc(oldHdr, size+sizeof(OSMEM_NOPOOLS_HDR_STC));
        if (allocPtr == NULL)
        {
            return NULL;
        }
        allocPtr->magic = (GT_UINTPTR)allocPtr;
        allocPtr->size = size;
        return (GT_VOID*)(allocPtr + 1);
    }
    if (oldHdr->magic == (GT_UINTPTR)oldHdr)
    {
        /* realloc memory allocated by malloc() */
        GT_VOID *newPtr = osMalloc(size);
        if (newPtr == NULL)
        {
            return NULL;
        }
        osMemCpy(newPtr, ptr, oldHdr->size < size ? oldHdr->size : size);
/*#ifdef OSMEMPOOLS_DEBUG
        osPrintf("Warning:Trying to Free a pointer allocated by malloc after osMemInit !\n");
#endif OSMEMPOOLS_DEBUG bothers CLI*/
        free(oldHdr);
        return newPtr;
    }
    return osMemPoolsRealloc(ptr, size);
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
        free(oldHdr);
    }
    else
    {
        /* check if this free is trying to free something that wasn't
           allocated by owr pools manager */
        if (oldHdr->magic == (GT_UINTPTR)oldHdr)
        {
/*#ifdef OSMEMPOOLS_DEBUG
            osPrintf("Warning:Trying to Free a pointer allocated by malloc after osMemInit !\n");
#endif OSMEMPOOLS_DEBUG bothers CLI*/
            free(oldHdr);
        }
        else
        {
            osMemPoolFree(memblock);
        }
    }
    return;
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
*       None
*
*******************************************************************************/
void *osMalloc
(
    IN GT_U32 size
)
{
    void *ptr;
    ptr = internalOsMalloc(size);

    osMemRegisterPtr(ptr,size,LOCAL_DEBUG_INFO);
    return ptr;
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
*       None
*
*******************************************************************************/
void *osRealloc
(
    IN void * ptr ,
    IN GT_U32 size
)
{
    void *newPtr;
    newPtr = internalOsRealloc(ptr,size);

    /* unregister old ptr */
    osMemUnregisterPtr(ptr,LOCAL_DEBUG_INFO);
    /* register new ptr */
    osMemRegisterPtr(newPtr,size,LOCAL_DEBUG_INFO);
    return newPtr;
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
*       None
*
*******************************************************************************/
void osFree
(
    IN void* const memblock
)
{
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
void *osCacheDmaMalloc
(
    IN GT_U32 size
)
{
    void * ptr;

    /* check if need to get modified size to hold support for 'overflow detection' */
    prvOsCacheDmaMallocOverflowDetectionSupportGet(&size);

    if(simDmaAsSharedMemSize)
    {
        char* tmpPtr; /* tmp pointer to start of memory to return */
        /* take sem */
        osMutexLock(simDmaMutexId);

        /* check if we have enough space for the new memory request */
        if((simDmaAsSharedMemOffsetUsed + size) > simDmaAsSharedMemSize)
        {
            /* free sem */
            osMutexUnlock(simDmaMutexId);
            return NULL;
        }

        tmpPtr = (char*)simDmaAsSharedMemPtr + simDmaAsSharedMemOffsetUsed;

        simDmaAsSharedMemOffsetUsed += size;

        /* free sem */
        osMutexUnlock(simDmaMutexId);

        ptr = tmpPtr;
    }
    else
    {
        ptr =  malloc(size); /*TODO: os_mem_chunks_array.memAllocFuncPtr(size); */
    }

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
    IN void *ptr
)
{
    /* unregister the ptr */
    (void)osCacheDmaUnregisterPtr(ptr);

    if(simDmaAsSharedMemSize)
    {
        /* not supporting free of this memory */
        return GT_OK;
    }


    free (ptr);
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
    *virtAddr = phyAddr;
    return GT_OK;
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
    *phyAddr = virtAddr;
    return GT_OK;
}


