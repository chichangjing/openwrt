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
* osVxMem.c
*
* DESCRIPTION:
*       VxWorks Operating System wrapper. Memory manipulation facility.
*
* DEPENDENCIES:
*       VxWorks, CPU independed.
*
* FILE REVISION NUMBER:
*       $Revision: 23 $
*******************************************************************************/

#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <cacheLib.h>
#include <vxWorks.h>

#include <gtOs/gtOsMem.h>
#include <gtOs/gtOsGen.h>
#include <gtOs/gtOsSem.h>
#include <gtOs/gtOsExc.h>

/*#include <../../mainDrv/h/prestera/common/gtMath.h>*/

#ifndef    RTOS_ON_SIM

#if (CPU == PPC604 || CPU == PPC603 || CPU == PPC405 || CPU == PPC860 || CPU == PPC85XX)
#define PHYS_TO_VIR_ADDR_CONVERT(addr) (addr)
#define VIR_TO_PHYS_ADDR_CONVERT(addr) (addr)
#endif

#if (CPU == R4000 || CPU == R3000)
#define PHYS_TO_VIR_ADDR_CONVERT(addr) (addr | 0xA0000000)
#define VIR_TO_PHYS_ADDR_CONVERT(addr) (addr & (~0xA0000000))
#endif

#if (CPU == ARMARCH4 || CPU == ARMARCH5 || CPU == ARMARCH7)
#define PHYS_TO_VIR_ADDR_CONVERT(addr) (addr)
#define VIR_TO_PHYS_ADDR_CONVERT(addr) (addr)
#endif

#else /* defined(RTOS_ON_SIM) */
#define PHYS_TO_VIR_ADDR_CONVERT(addr) (addr)
#define VIR_TO_PHYS_ADDR_CONVERT(addr) (addr)
#endif

/*
    Use the BSP functions to allocate non-cached (bspCachDmaMalloc) in an area totally
    outside of the VxWorks operating system. Especially for HSU usage.
*/

#if (CPU == PPC85XX)
#ifdef WORKBENCH_TOOL_ENV
#define USE_BSP_DMA_MALLOC
#endif
#endif

#ifdef XCAT_DRV
#define USE_BSP_DMA_MALLOC
#endif

#ifdef USE_BSP_DMA_MALLOC
extern int bspCacheDmaFree(void *ptr);
extern int bspCacheDmaMalloc(GT_U32 size);
#endif

/***** Global Vars ********************************************/

GT_BOOL  firstInitialization = GT_TRUE;

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
/***** Static functions ************************************************/

/***** Static variables ************************************************/

/***** Public Functions ************************************************/

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
    ret = osMemLibInit((GT_MEMORY_ALLOC_FUNC)malloc, (GT_MEMORY_FREE_FUNC)free, size, memAllocMethod);

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
    if (firstInitialization == GT_TRUE)
    {
        return osMalloc(size);
    }
    return osStaticMallocFromChunk(size);
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
void *osMalloc
(
    IN GT_U32 size
)
{
    IS_WRAPPER_OPEN_PTR;

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
    else
    {
        return osMemPoolsAlloc(size);
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
void *osRealloc
(
    IN void * ptr ,
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
/*#ifdef OSMEMPOOLS_DEBUG
            osPrintf("Warning:Trying to Free a pointer allocated by malloc after osMemInit !\n");
#endif OSMEMPOOLS_DEBUG bothers CLI*/
            free(oldHdr);
            return newPtr;
        }
        return osMemPoolsRealloc(ptr, size);
    }
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
void osFree
(
    IN void* const memblock
)
{
    OSMEM_NOPOOLS_HDR_STC *oldHdr = NULL;
    assert(memblock != NULL);
    oldHdr = (OSMEM_NOPOOLS_HDR_STC*)((GT_UINTPTR)memblock - sizeof(OSMEM_NOPOOLS_HDR_STC));
    if (firstInitialization == GT_TRUE)
    {
        assert(oldHdr->magic == (GT_UINTPTR)oldHdr);
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

    IS_WRAPPER_OPEN_PTR;

    /* check if need to get modified size to hold support for 'overflow detection' */
    prvOsCacheDmaMallocOverflowDetectionSupportGet(&size);

#ifdef USE_BSP_DMA_MALLOC
    ptr =  (void*)bspCacheDmaMalloc(size);
#else
    ptr =  (void*)cacheDmaMalloc(size);
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
    IN void *ptr
)
{
    IS_WRAPPER_OPEN_STATUS;

    /* unregister the ptr */
    (void)osCacheDmaUnregisterPtr(ptr);

#ifdef USE_BSP_DMA_MALLOC
    if (bspCacheDmaFree(ptr) == OK)
    {
        return GT_OK;
    }
    else
        return GT_FAIL;
#else
    if (cacheDmaFree(ptr) == OK)
    {
        return GT_OK;
    }
    else
        return GT_FAIL;
#endif

}

/*******************************************************************************
* osCacheDmaFreeAll
*
* DESCRIPTION:
*       Free ALL DMA memory (BM only)
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
    *virtAddr = PHYS_TO_VIR_ADDR_CONVERT(phyAddr);
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
    IS_WRAPPER_OPEN_STATUS;
    *phyAddr = VIR_TO_PHYS_ADDR_CONVERT(virtAddr);

    return GT_OK;
}
