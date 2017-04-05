/*******************************************************************************
*                Copyright 2015, MARVELL SEMICONDUCTOR, LTD.                   *
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
* (MJKK), MARVELL ISRAEL LTD. (MSIL).                                          *
********************************************************************************
* noKmDrvCacheMngStub.c
*
* DESCRIPTION:
*       Cache management functions
*       Use mvDmaDrv or HUGETLB feature
*       On Intel CPUs requires 'intel_iommu=off' kernel option
*
* DEPENDENCIES:
*       None.
*
* FILE REVISION NUMBER:
*       $Revision: 1 $
*
*******************************************************************************/
#define _GNU_SOURCE
#include <gtExtDrv/drivers/gtCacheMng.h>

/*******************************************************************************
* extDrvMgmtCacheFlush
*
* DESCRIPTION:
*       Flush to RAM content of cache
*
* INPUTS:
*       type        - type of cache memory data/intraction
*       address_PTR - starting address of memory block to flush
*       size        - size of memory block
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK   - on success,
*       GT_FAIL - otherwise.
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS extDrvMgmtCacheFlush
(
    IN GT_MGMT_CACHE_TYPE_ENT   type, 
    IN void                     *address_PTR, 
    IN size_t                   size
)
{
    (void) type;
    (void) address_PTR;
    (void) size;
    return GT_OK;
}


/*******************************************************************************
* extDrvMgmtCacheInvalidate
*
* DESCRIPTION:
*       Invalidate current content of cache
*
* INPUTS:
*       type        - type of cache memory data/intraction
*       address_PTR - starting address of memory block to flush
*       size        - size of memory block
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK   - on success,
*       GT_FAIL - otherwise.
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS extDrvMgmtCacheInvalidate 
(
    IN GT_MGMT_CACHE_TYPE_ENT   type, 
    IN void                     *address_PTR, 
    IN size_t                   size
)
{
    (void) type;
    (void) address_PTR;
    (void) size;
    return GT_OK;
}

#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
void*  prvExtDrvDmaPtr = NULL;
GT_UINTPTR prvExtDrvDmaPhys = 0;
GT_U32 prvExtDrvDmaLen = 0;

#ifndef MAP_32BIT
#define MAP_32BIT 0x40
#endif

#ifdef INTEL64_DRV
static uint64_t linux_virt_to_phys(uintptr_t vaddr)
{
    int pagemap;
    int64_t paddr = 0;
    uintptr_t pagesize = getpagesize();
    uintptr_t offset;
    uint64_t e;

    offset = (vaddr / pagesize) * sizeof(uint64_t);
    /* https://www.kernel.org/doc/Documentation/vm/pagemap.txt */
    if ((pagemap = open("/proc/self/pagemap", O_RDONLY)) < 0)
        return 0;
    if (lseek(pagemap, offset, SEEK_SET) != offset)
    {
        close(pagemap);
        return 0;
    }
    if (read(pagemap, &e, sizeof(uint64_t)) != sizeof(uint64_t))
    {
        close(pagemap);
        return 0;
    }
    close(pagemap);
    if (e & (1ULL << 63)) { /* page present ? */
        paddr = e & ((1ULL << 54) - 1); /* pfn mask */
        paddr = paddr * getpagesize();
        /* add offset within page */
        paddr |= (vaddr & (pagesize - 1));
    }
    return paddr;
}

/* try HUGETLB feature */
static long hugePagesTotal = -1;
static long hugePagesFree = -1;
static long hugePageSize = -1;
static int scan_proc_meminfo(void)
{
    FILE *f;
    char buf[BUFSIZ];
    unsigned long num;

    f = fopen("/proc/meminfo","r");
    if (!f)
        return 1;
    while (!feof(f))
    {
        if (!fgets(buf,sizeof(buf)-1,f))
            break;
        buf[sizeof(buf)-1] = 0;
        if (sscanf(buf, "HugePages_Total: %lu", &num) == 1)
        {
            hugePagesTotal = num;
            printf("hugePagesTotal=%lu\n",hugePagesTotal);
        }
        if (sscanf(buf, "HugePages_Free: %lu", &num) == 1)
        {
            hugePagesFree = num;
            printf("hugePagesFree=%lu\n",hugePagesFree);
        }
        if (sscanf(buf, "Hugepagesize: %lu", &num) == 1)
        {
            hugePageSize = num * 1024;
            printf("hugePageSize=%lu\n",hugePageSize);
        }
    }
    fclose(f);
    if (hugePagesTotal < 0 || hugePagesFree < 0 || hugePageSize < 0)
        return 1;
    return 0;
}

static int alloc_hugetlb(void)
{
    void *h;
    if (scan_proc_meminfo())
        return 1;
    if (hugePagesFree == 0)
    {
        FILE *f;
        f = fopen("/proc/sys/vm/nr_hugepages","w");
        fprintf(f,"%lu\n", hugePagesTotal + 1);
        fclose(f);
        if (scan_proc_meminfo())
            return 1;
        if (hugePagesFree == 0)
            return 1;
    }

    h = mmap((void*)0x50000000, hugePageSize,
        PROT_READ | PROT_WRITE,
        MAP_FIXED | MAP_ANONYMOUS | MAP_SHARED | MAP_HUGETLB,
        0, 0);
    if (h == MAP_FAILED)
    {
        perror("mmap(MAP_HUGETLB)");
        return 1;
    }
    prvExtDrvDmaPtr = h;
    prvExtDrvDmaLen = hugePageSize;

    /* resolve phys address */
    mlock(prvExtDrvDmaPtr, prvExtDrvDmaLen);
    prvExtDrvDmaPhys = (GT_UINTPTR)linux_virt_to_phys((uintptr_t)prvExtDrvDmaPtr);
    if (prvExtDrvDmaPhys & 0xffffffff00000000L)
    {
        /* Address above 32bit address space, not supported by Marvell PPs */
        munlock(prvExtDrvDmaPtr, prvExtDrvDmaLen);
        munmap(prvExtDrvDmaPtr, prvExtDrvDmaLen);
        prvExtDrvDmaPtr = NULL;
        prvExtDrvDmaLen = 0;
        prvExtDrvDmaPhys = (GT_UINTPTR)0L;
    }
    else
    {
        fprintf(stderr,"hugePage mapped to %p\n", prvExtDrvDmaPtr);
    }

    return 0;
}
#endif

/* The offset parameter of mmap for mvDmaDrv
 * if non-zero then map DMA for PCI device
 */
off_t mvDmaDrvOffset = 0;
static void try_map_mvDmaDrv(void)
{
    int fd;
    FILE *f;
    char buf[BUFSIZ];

    fd = open("/dev/mvDmaDrv", O_RDWR);
    if (fd >= 0)
    {
        prvExtDrvDmaLen = 2*1024*1024;
        prvExtDrvDmaPtr = mmap(NULL, prvExtDrvDmaLen,
            PROT_READ | PROT_WRITE,
            MAP_32BIT | MAP_SHARED,
            fd, mvDmaDrvOffset);
        if (prvExtDrvDmaPtr == MAP_FAILED)
        {
            perror("mmap(mvDmaDrv)");
            prvExtDrvDmaPtr = NULL;
            close(fd);
            return;
        }
        else
        {
            fprintf(stderr,"mvDmaDrv mapped to %p\n",prvExtDrvDmaPtr);
        }
    }

    prvExtDrvDmaPhys = 0;
    f = fopen("/proc/self/maps","r");
    if (!f)
        return;
    while (!feof(f))
    {
        unsigned long long start, end, offset;
        char attr1, attr2, attr3, attr4;
        int k;
        if (!fgets(buf,sizeof(buf)-1,f))
            break;
        buf[sizeof(buf)-1] = 0;
        /* b65c7000-b67c7000 rw-s 11200000 00:05 2088       /dev/mvDmaDrv */
        k = sscanf(buf, "%llx-%llx %c%c%c%c %llx",
                &start, &end, &attr1, &attr2, &attr3, &attr4, &offset);
        if (k == 7)
        {
            if (start == (uintptr_t)prvExtDrvDmaPtr)
            {
                prvExtDrvDmaPhys = (GT_UINTPTR)offset;
                break;
            }
        }
    }
    fclose(f);
}

/*******************************************************************************
* check_dma
*
* DESCRIPTION:
*       Check if DMA block already allocated/mapped to userspace
*       If no then allocate/map
*
* INPUTS:
*       None.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       None
*
*******************************************************************************/
static void check_dma(void)
{
    if (prvExtDrvDmaPtr != NULL)
        return;
    /* try to map from mvDmaDrv */
    try_map_mvDmaDrv();
#ifdef INTEL64_DRV
    if (prvExtDrvDmaPtr == NULL)
    {
        /* try to allocate hugetlb */
        alloc_hugetlb();
    }
#endif
fprintf(stderr, "dmaLen=%d\n", prvExtDrvDmaLen);
fprintf(stderr, "dmaPhys=%p\n", (void*)prvExtDrvDmaPhys);
}

/*******************************************************************************
* extDrvGetDmaBase
*
* DESCRIPTION:
*       Get the base address of the DMA area need for Virt2Phys and Phys2Virt
*           translaton
*
* INPUTS:
*       None.
*
* OUTPUTS:
*       dmaBase     - he base address of the DMA area.
*
* RETURNS:
*       GT_OK   - on success,
*       GT_FAIL - otherwise.
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS extDrvGetDmaBase
(
	OUT GT_UINTPTR * dmaBase
)
{
    /* TODO */
    /*  *dmaBase = (GT_UINTPTR)base; */
    check_dma();
    *dmaBase = (GT_UINTPTR)prvExtDrvDmaPhys;
    return GT_OK;
}

/*******************************************************************************
* extDrvGetDmaVirtBase
*
* DESCRIPTION:
*       Get the base address of the DMA area in userspace
*
* INPUTS:
*       None.
*
* OUTPUTS:
*       dmaVirtBase     - the base address of the DMA area in userspace.
*
* RETURNS:
*       GT_OK   - on success,
*       GT_FAIL - otherwise.
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS extDrvGetDmaVirtBase
(
	OUT GT_UINTPTR *dmaVirtBase
)
{
    /* TODO */
    /* *dmaVirtBase = LINUX_VMA_DMABASE; */
    check_dma();
    *dmaVirtBase = (GT_UINTPTR)prvExtDrvDmaPtr;
    return GT_OK;
}

/*******************************************************************************
* extDrvGetDmaSize
*
* DESCRIPTION:
*       Get the size of the DMA area
*
* INPUTS:
*       None.
*
* OUTPUTS:
*       dmaSize     - The size of the DMA area.
*
* RETURNS:
*       GT_OK   - on success,
*       GT_FAIL - otherwise.
*
* COMMENTS:
*
*******************************************************************************/
extern GT_STATUS extDrvGetDmaSize
(
	OUT GT_U32 * dmaSize
)
{
    /* TODO */
    *dmaSize = prvExtDrvDmaLen;
    return GT_OK;
}
