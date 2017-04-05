/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* shrMemRemapper.c
*
* DESCRIPTION:
*       This file contains remapping routines which remaps regular libcpss.so
*       shared object (TEXT section is common) to CPSS Shared Library (also
*       BSS and DATA sections are common).
*
* FILE REVISION NUMBER:
*       $Revision: 14 $
*
*******************************************************************************/

/************* Includes *******************************************************/
#define _GNU_SOURCE

#include <sys/mman.h>
#include <sys/types.h>
#include <semaphore.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <gtOs/gtEnvDep.h>
#include <gtOs/gtOsIo.h>
#include <gtOs/gtOsMem.h>
#include <gtOs/gtOsGen.h>
#include <gtOs/gtOsSharedMemoryRemapper.h>
#include <gtExtDrv/drivers/prvExtDrvLinuxMapping.h>

#ifdef OS_NAMED_SEM
#   include <gtOs/gtOsSem.h>
#   define SEM_TYPE GT_SEM
    static GT_STATUS SEM_UNLINK(const char *name)
    {
        GT_SEM sem;
        if (osSemMOpenNamed(name, &sem) != GT_OK)
            return GT_FAIL;
        osSemDelete(sem);
        return GT_OK;
    }
    static GT_STATUS SEM_OPEN(const char *name, SEM_TYPE *semid)
    {
        if(osSemMOpenNamed(name, semid) != GT_OK)
        {
            fprintf(stderr, "osSemMOpenNamed(\"%s\") : can't open/create semaphore\n",
                name);
            return GT_FAIL;
        }
        return GT_OK;
    }
    static GT_STATUS SEM_CLOSE(SEM_TYPE semid)
    {
        return GT_OK;
    }
    static GT_STATUS SEM_WAIT(SEM_TYPE semid)
    {
        return osSemWait(semid, OS_WAIT_FOREVER);
    }
    static GT_STATUS SEM_POST(SEM_TYPE semid)
    {
        return osSemSignal(semid);
    }
    static GT_STATUS SEM_NOT_LOCKED(SEM_TYPE semid)
    {
        GT_STATUS status;
        status = osSemWait(semid, OS_NO_WAIT);
        if (status == GT_OK)
        {
            osSemSignal(semid);
        }
        return status;
    }
#endif
#ifdef POSIX_SEM
#   include <semaphore.h>
#   define  SEM_TYPE sem_t*
    static GT_STATUS SEM_UNLINK(const char *name)
    {
        if(sem_unlink(name) < 0)
        {
            if(errno != ENOENT && 0 != errno)
            {
                fprintf(stderr, "sem_unlink() : %s %s\n", name, strerror(errno));
                return GT_FAIL;
            }
        }
        return GT_OK;
    }
    static GT_STATUS SEM_OPEN(const char *name, SEM_TYPE *semid)
    {
        if((*semid = sem_open(name, O_CREAT, 0666, 1)) == SEM_FAILED)
        {
            fprintf(stderr, "sem_open() : %s\n", strerror(errno));
            return GT_FAIL;
        }
        return GT_OK;
    }
    static GT_STATUS SEM_CLOSE(SEM_TYPE semid)
    {
        if(sem_close(semid) == -1)
        {
            fprintf(stderr, "sem_close() : %s\n", strerror(errno));
            return GT_FAIL;
        }
        return GT_OK;
    }
    static GT_STATUS SEM_WAIT(SEM_TYPE semid)
    {
        if(sem_wait(semid) == -1)
        {
            fprintf(stderr, "sem_wait() : %s\n", strerror(errno));
            return GT_FAIL;
        }
        return GT_OK;
    }
    static GT_STATUS SEM_POST(SEM_TYPE semid)
    {
        if(sem_post(semid) == -1)
        {
            fprintf(stderr, "sem_post() : %s\n", strerror(errno));
            return GT_FAIL;
        }
        return GT_OK;
    }
    static GT_STATUS SEM_NOT_LOCKED(SEM_TYPE semid)
    {
        int sval;
        if (sem_getvalue(semid, &sval) < 0)
        {
            fprintf(stderr, "sem_getvalue() : %s\n", strerror(errno));
            return GT_FAIL;
        }
        return (sval > 0) ?  GT_OK : GT_FAIL;
    }
#endif
#ifdef NO_NAMED_SEM
#   define SEM_TYPE int
    static GT_STATUS SEM_UNLINK(const char *name)
    {
        return GT_OK;
    }
    static GT_STATUS SEM_OPEN(const char *name, SEM_TYPE *semid)
    {
        return GT_OK;
    }
    static GT_STATUS SEM_CLOSE(SEM_TYPE semid)
    {
        return GT_OK;
    }
    static GT_STATUS SEM_WAIT(SEM_TYPE semid)
    {
        return GT_OK;
    }
    static GT_STATUS SEM_POST(SEM_TYPE semid)
    {
        return GT_OK;
    }
    static GT_STATUS SEM_NOT_LOCKED(SEM_TYPE semid)
    {
        return GT_OK;
    }
#endif

/*#define MM_DEBUG */

/* memory mapping sections for auto error detection */
typedef struct PRV_MMAP_AREA_INFO_STCT {
    GT_UINTPTR  start;
    GT_SIZE_T   length;
    const char *name;
} PRV_MMAP_AREA_INFO_STC;
static PRV_MMAP_AREA_INFO_STC prvMmapAreaInfo[] = {
    { SHARED_MEMORY_MALLOC_VIRT_ADDR_MAC,OSMEM_MAX_MEMORY_SIZE_USE,"CPSS_SHM_MALLOC" },
#ifndef ASIC_SIMULATION
    { LINUX_VMA_DMABASE,                   0x200000/*2M*/,   "MVPP_DMA" },
    { LINUX_VMA_PP_CONF_BASE,              0x4000000/*64M*/, "MVPPCONF" },
    { LINUX_VMA_PP_DFX_BASE,               0x4000000/*64M*/, "MVPPDFX" },
    { LINUX_VMA_PP_REGS_BASE + 0x00000000, 0x4000000/*64M*/, "MVPPREGS0" },
    { LINUX_VMA_PP_REGS_BASE + 0x04000000, 0x4000000/*64M*/, "MVPPREGS1" },
    { LINUX_VMA_PP_REGS_BASE + 0x08000000, 0x4000000/*64M*/, "MVPPREGS2" },
    { LINUX_VMA_PP_REGS_BASE + 0x0c000000, 0x4000000/*64M*/, "MVPPREGS3" },
    { LINUX_VMA_PP_REGS_BASE + 0x10000000, 0x4000000/*64M*/, "MVPPREGS4" },
    { LINUX_VMA_PP_REGS_BASE + 0x14000000, 0x4000000/*64M*/, "MVPPREGS5" },
    { LINUX_VMA_PP_REGS_BASE + 0x18000000, 0x4000000/*64M*/, "MVPPREGS6" },
    { LINUX_VMA_PP_REGS_BASE + 0x1c000000, 0x4000000/*64M*/, "MVPPREGS7" },
#endif
    { 0, 0, NULL }
};

/*******************************************************************************
* check_overlap
*
* DESCRIPTION:
*       Check if /proc/self/maps region overlaps with CPSS shared lib mappings
*       Display an error message if overlapped
*
* INPUTS:
*       str     - a single line from /proc/self/maps
*       start   - start of a memory region
*       end     - end of a memory region
*
* OUTPUTS:
*       None
*
* RETURNS:
*       None
*
* COMMENTS:
*
*******************************************************************************/
static void check_overlap(
  IN const char *str,
  IN GT_UINTPTR start,
  IN GT_UINTPTR end
)
{
    int i, overlap;
#define A prvMmapAreaInfo[i]
    for (i = 0; A.name; i++)
    {
        overlap = 0;
        if ((start >= A.start) && (start < (A.start + A.length)))
            overlap = 1;
        if ((end > A.start) && (end < (A.start + A.length)))
            overlap = 1;
        if ((A.start >= start) && (A.start < end))
            overlap = 1;
        if ((A.start+A.length) > start && ((A.start+A.length) < end))
            overlap = 1;
        if (overlap)
            fprintf(stderr, "***ERR: shared lib %p-%p possible overlap with %s:\n***     %s",
                    (void*)start, (void*)end,
                    A.name,str);
    }
#undef A
}

/*******************************************************************************
* shmDoCheckOverlap
*
* DESCRIPTION:
*       Read /proc/self/maps for a possible overlaps with CPSS shared lib mappings
*
* INPUTS:
*       None
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK   - Operation succeeded
*       GT_FAIL - Operaton failed
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS shmDoCheckOverlap(void)
{
    FILE *map;
    char buf[4096];
    char attr1, attr2, attr3;

    /* Open special file with section's info.
     * This file is handled by Linux kernel. */
    if( (map = fopen("/proc/self/maps", "r")) == NULL)
    {
        fprintf(stderr, "fopen() : %s\n", strerror(errno));
        return GT_FAIL;
    }

    /* Read it line-by-line */
    while(fgets(buf, sizeof(buf), map) != NULL)
    {
        unsigned long long start, done;/* long will be 64bit on 64bit OS */

        /* Parse varables from line */
        sscanf(buf, "%llx-%llx %c%c%c", &start, &done, &attr1, &attr2, &attr3);

        check_overlap(buf, start, done);
    }

    /* Close /proc/self/maps */
    if (fclose(map) == EOF)
    {
        fprintf(stderr, "fclose() : %s\n", strerror(errno));
        return GT_FAIL;
    }

    return GT_OK;
}

/************* Functions ******************************************************/
/************* Static Functions ***********************************************/

/*******************************************************************************
* findAdressesOfSections
*
* DESCRIPTION:
*       Find termination adress of the shared section into /proc/self/maps file.
*
* INPUTS:
*       cpss_sectPtr  - pointer to structure with leading adress of the section
*       mainos_sectPtr  - pointer to structure with leading adress of the section
*
* OUTPUTS:
*       cpss_sectPtr   -
*       mainos_sectPtr - pointer to structure for leading and termination adress
*                  of the section
*
* RETURNS:
*       GT_OK   - Operation succeeded
*       GT_FAIL - Operaton failed
*
* COMMENTS:
*       Uses SHARED_DATA_ADDR_CNS SHARED_MAINOS_DATA_ADDR_CNS
*       (fixed well-known address) as a key for searching required
*       section by start address. Only end address is calculated here.
*
*******************************************************************************/
static GT_STATUS findAdressesOfSections(
    INOUT SECTION_STC *cpss_sectPtr,
    INOUT SECTION_STC *mainos_sectPtr
)
{
    FILE *map;
    char buf[4096];
    char attr1, attr2, attr3;
    SECTION_STC *sectPtr = cpss_sectPtr;

    /* Init structure -fill by zeroes */
    if(memset(cpss_sectPtr, 0, sizeof(SECTION_STC)) == NULL)
    {
        fprintf(stderr, "memset() : %s\n", strerror(errno));
        return GT_FAIL;
    }
    if(memset(mainos_sectPtr, 0, sizeof(SECTION_STC)) == NULL)
    {
        fprintf(stderr, "memset() : %s\n", strerror(errno));
        return GT_FAIL;
    }

    /* Open special file with section's info.
     * This file is handled by Linux kernel. */
    if( (map = fopen("/proc/self/maps", "r")) == NULL)
    {
        fprintf(stderr, "fopen() : %s\n", strerror(errno));
        return GT_FAIL;
    }

    /* Read it line-by-line */
    while(fgets(buf, sizeof(buf), map) != NULL)
    {
        unsigned long long start, done;/* long will be 64bit on 64bit OS */

        /* Parse varables from line */
        sscanf(buf, "%llx-%llx %c%c%c", &start, &done, &attr1, &attr2, &attr3);

        /* check_overlap(buf, start, done); */
        /* Start address matches the key? */
        if(start == SHARED_DATA_ADDR_CNS /* || start == SHARED_MAINOS_DATA_ADDR_CNS */)
        {
            sectPtr = cpss_sectPtr;
            sectPtr->startPtr = (GT_VOID *)((GT_UINTPTR)start);
            sectPtr->donePtr  = (GT_VOID *)((GT_UINTPTR)done);
        } else if(start == SHARED_MAINOS_DATA_ADDR_CNS)
        {
            sectPtr = mainos_sectPtr;
            sectPtr->startPtr = (GT_VOID *)((GT_UINTPTR)start);
            sectPtr->donePtr  = (GT_VOID *)((GT_UINTPTR)done);
        }
        /* !! NOTE - we merge consecutive sections 'cause compiler
            can split them into parts.
            Warning: unrelated sections shouldn't be consecutive!!! */
        else if((GT_VOID *)((GT_UINTPTR)start) == sectPtr->donePtr)
        {
            /* Don't merge with guider section (with no any rights) */
            if(attr1!='-' || attr2!='-' || attr3!='-')
                sectPtr->donePtr  = (GT_VOID *)((GT_UINTPTR)done);
        }
    }

    /* Close /proc/self/maps */
    if (fclose(map) == EOF)
    {
        fprintf(stderr, "fclose() : %s\n", strerror(errno));
        return GT_FAIL;
    }

    return GT_OK;
}

/*******************************************************************************
* mapFile
*
* DESCRIPTION:
*       Maps shared buffer into processes address space instead of BSS/DATA
*       section. Also the first client have to fulfill initial content of
*       buffer by data from BSS/DATA.
*
* INPUTS:
*       namePtr       - name of file
*       sectPtr       - pointer to the BSS/DATA section for which
*                       mapping is processed for
*       isFirstClient - is client is the first CPSS client(Enabler) on not.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK   - Operation succeeded
*       GT_FAIL - Operaton failed
*
* COMMENTS:
*       This is the core function of Shared Memory approach.
*
*******************************************************************************/
static GT_STATUS mapFile
(
    IN char         *namePtr,
    IN SECTION_STC  *sectPtr,
    IN GT_BOOL      isFirstClient
)
{
    GT_VOID_PTR ptr;
    GT_32 fd;
    void* (*mmapPtr)(void *start, size_t length, int prot , int flags, int fd,
       off_t offset);

    if(sectPtr == NULL || sectPtr->startPtr == NULL)
        return GT_OK;

    if(isFirstClient)
    {
        /* Create shared buffer file */
        /* NOTE - here we use open('dev/shm/XXX') instaead of shm_open('XXX')
         * 'cause this function is likely not supported by ucLibC
        */
        fd = open(namePtr, O_RDWR | O_CREAT, 0666);
        if (fd < 0)
        {
            fprintf(stderr, "open() : %s\n", strerror(errno));
            return GT_FAIL;
        }

        /* Truncate shared buffer file to required length
         * (to the actual section's length) */
        if(ftruncate(fd, sectPtr->donePtr - sectPtr->startPtr) == -1)
        {
            fprintf(stderr, "ftruncate() : %s\n", strerror(errno));
            return GT_FAIL;
        }

        /* Write original content of BSS/DATA section into shared buffer */
        if(write(fd, sectPtr->startPtr, sectPtr->donePtr - sectPtr->startPtr) == -1)
        {
            fprintf(stderr, "write() : %s\n", strerror(errno));
            return GT_FAIL;
        }
    }
    else /* Non-first client */
    {
        /* Just open already created buffer */
        fd = open(namePtr, O_RDWR, 0666);
        if(fd < 0)
        {
            if(errno == ENOENT)
            {
                fprintf(stderr, "open() : file %s not exist, run Enabler first\n", namePtr);
                return GT_FAIL;
            }
            fprintf(stderr, "open() : %s\n", strerror(errno));
            return GT_FAIL;
        }
    }

    /* force dynamic linker to resolve mmap address */
    mmap(0,0,0,0,0,0);

    /* save the mmap address for later usage */
    mmapPtr = mmap;

    /* Shutdown old (private) BSS/DATA section */
    if(munmap(sectPtr->startPtr, sectPtr->donePtr - sectPtr->startPtr) == -1)
    {
        fprintf(stderr, "munmap() : %s\n", strerror(errno));
        return GT_FAIL;
    }

    /* !! NOTE - here is the popentially danger place -
        we havn't access to detached section !! */

    /* !! NOTE - address of 'mmap' function should be already resolved to avoid access
     * to untached section. */
    ptr = mmapPtr(sectPtr->startPtr, sectPtr->donePtr - sectPtr->startPtr,
               PROT_READ | PROT_WRITE /* | PROT_EXEC */,
               MAP_FIXED | MAP_SHARED, fd, 0);

    if(!ptr)
    {
        fprintf(stderr, "mmap() : %s\n", strerror(errno));
        return GT_FAIL;
    }

    /* Shared file descriptor is not required for now.
     * We can close it.
     * !!! Note - we still can use the mapped section into that file. */
    if(close(fd) == -1)
    {
        fprintf(stderr, "close() : %s\n", strerror(errno));
        return GT_FAIL;
    }

    return GT_OK;
}

/************* Public Functions ***********************************************/

/*******************************************************************************
* shrAddNameSuffix
*
* DESCRIPTION:
*       Add suffix to name (getenv("CPSS_SHR_INSTANCE_NUM"))
*
* INPUTS:
*       name
*
* OUTPUTS:
*       buffer
*
* RETURNS:
*   pointer to buffer
*
* COMMENTS:
*
*******************************************************************************/
char* shrAddNameSuffix(const char* name, char* buffer, int bufsize)
{
    const char *e;
    char  uid[64];

    if (name == NULL || buffer == NULL)
    {
        return NULL;
    }
    strncpy(buffer,name,bufsize-1);

    /* add UID as string to buffer*/
    sprintf(uid,"%d", getuid());
    strncat(buffer, uid, bufsize-1);

    /* add CPSS_SHR_INSTANCE_NUM string to buffer */
    e = getenv("CPSS_SHR_INSTANCE_NUM");
    if (e != NULL)
    {
        strncat(buffer, e, bufsize-1);
    }
    buffer[bufsize-1] = 0;
    return buffer;
}

/*******************************************************************************
* shrMemUnlinkShmObj
*
* DESCRIPTION:
*       Unlink shared files and semaphore.
*
* INPUTS:
*       None
*
* OUTPUTS:
*       None
*
* RETURNS:
*   GT_OK   - Operation succeeded
*   GT_FAIL - Operaton failed
*
* COMMENTS:
*       This action is required for detect-first-client-by-file-existence
*       method.
*
*******************************************************************************/
GT_STATUS shrMemUnlinkShmObj(GT_VOID)
{
    char buf[256];
    GT_STATUS st = GT_OK;

    /* Remove named semaphore for clean-up */
    if (SEM_UNLINK(shrAddNameSuffix(SHARED_MEMORY_SEM_CNS,buf,sizeof(buf))) != GT_OK)
        st = GT_FAIL;

    /* Unlink shared buffer */
#define DO_RM(name) \
    shrAddNameSuffix(name,buf,sizeof(buf)); \
    if(unlink(buf) < 0) \
    { \
        if(errno != ENOENT && 0 != errno) \
        { \
            fprintf(stderr, "unlink() : %s %s\n", buf, strerror(errno)); \
            st = GT_FAIL; \
        } \
    }

    DO_RM(SHARED_MEMORY_DATA_CNS);
    DO_RM(SHARED_MEMORY_MAINOS_DATA_CNS);
    DO_RM("/dev/shm/CPSS_SHM_MALLOC");
#undef DO_RM

    return st;
}

/*******************************************************************************
* shrMemDoShmMap
*
* DESCRIPTION:
*       Do sharing data for all processes.
*
* INPUTS:
*       isFirstClient - is client is the first CPSS client (CPSS Enabler)
*       or not.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK   - Operation succeeded
*       GT_FAIL - Operaton failed
*
* COMMENTS:
*       Performs major shared memory initializations.
*       It searches BSS/DATA section and remaps it to shared space.
*
*******************************************************************************/
GT_STATUS shrMemDoShmMap(GT_BOOL isFirstClient)
{
    SEM_TYPE    semid;
    SECTION_STC cpss_section;
    SECTION_STC mainos_section;
    char buf[256];

    /* Open named semaphore to guard remapping code from other clients. */
    if (SEM_OPEN(shrAddNameSuffix(SHARED_MEMORY_SEM_CNS, buf, sizeof(buf)), &semid) != GT_OK)
        return GT_FAIL;

    if (isFirstClient)
    {
        if (SEM_NOT_LOCKED(semid) != GT_OK)
        {
            /* it seems there is a deadlock */
            fprintf(stderr,
                    "Warning: The shared mem semaphore seems to be locked\n"
                    "Only one appDemo*_shared process will run in the same time\n"
                    "For cleanup please remove the following files:\n"
                    "    " SHARED_MEMORY_DATA_CNS "$CPSS_SHR_INSTANCE_NUM\n"
                    "    /dev/shm/CPSS_SHM_MALLOC$CPSS_SHR_INSTANCE_NUM\n"
                    "    " SHARED_MEMORY_MAINOS_DATA_CNS "$CPSS_SHR_INSTANCE_NUM\n"
                    "    /dev/shm/sem.CPSS_SHM_SEM$CPSS_SHR_INSTANCE_NUM\n");
        }
    }
    /* Now searching for section we should remap.
     * Only one section with known begin address should be searched.
     * We have to know end address of it to map whole section. */
    if(findAdressesOfSections(&cpss_section,&mainos_section) == GT_FAIL)
    {
        fprintf(stderr, "findAdressesOfSections() : error\n");
        goto closeSemAndFail;
    }
    /* handle error if sections are not found */
    if ((cpss_section.startPtr == NULL) || (mainos_section.startPtr == NULL))
    {
        FILE *map;
        if( (map = fopen("/proc/self/maps", "r")) != NULL)
        {
            while (!feof(map))
            {
                int r;
                r = fread(buf, 1, sizeof(buf), map);
                if (r <= 0)
                    break;
                fwrite(buf, 1, r, stderr);
            }
            fclose(map);
            fflush(stderr);
        }
    }
    if (cpss_section.startPtr == NULL)
    {
        fprintf(stderr, "error: CPSS_SHM_DATA not found, must start at address %p\n",
                (void*)((GT_UINTPTR)SHARED_DATA_ADDR_CNS));
    }
    if (mainos_section.startPtr == NULL)
    {
        fprintf(stderr, "error: MAINOS_SHM_DATA not found, must start at address 0x%lx\n",
                (void*)((GT_UINTPTR)SHARED_MAINOS_DATA_ADDR_CNS));
    }
    if ((cpss_section.startPtr == NULL) || (mainos_section.startPtr == NULL))
    {
        goto closeSemAndFail;
    }

    /* Lock semaphore */
    if(SEM_WAIT(semid) != GT_OK)
        goto closeSemAndFail;

#ifdef MM_DEBUG
    /* Output sections BEFORE remapping. */
    shrMemPrintMapsDebugInfo();
#endif

    /* Map shared buffer into processes address space.
     * The first client also have to create it before mapping. */
    if(mapFile(shrAddNameSuffix(SHARED_MEMORY_DATA_CNS,buf,sizeof(buf)), &cpss_section, isFirstClient) == GT_FAIL)
    {
        SEM_POST(semid);
        fprintf(stderr, "mapFile(CPSS_SHM_DATA) : error\n");
        goto closeSemAndFail;
    }

    if(mapFile(shrAddNameSuffix(SHARED_MEMORY_MAINOS_DATA_CNS,buf,sizeof(buf)), &mainos_section, isFirstClient) == GT_FAIL)
    {
        SEM_POST(semid);
        fprintf(stderr, "mapFile(MAINOS_SHM_DATA) : error\n");
        goto closeSemAndFail;
    }

    if(SEM_POST(semid) != GT_OK)
        goto closeSemAndFail;

    if(SEM_CLOSE(semid) != GT_OK)
        return GT_FAIL;


#ifdef MM_DEBUG
    shrMemPrintMapsDebugInfo();
#endif

    return GT_OK;

closeSemAndFail:
    SEM_CLOSE(semid);
    return GT_FAIL;

}


/*******************************************************************************
* shrMemPrintMapsDebugInfo
*
* DESCRIPTION:
*       Ouputs /proc/self/maps to stderr for debug purpose
*
* INPUTS:
*       None.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       None.
*
* COMMENTS:
*       Should be used only for debugging on SHARED_MEMORY=1
*
*******************************************************************************/
GT_VOID shrMemPrintMapsDebugInfo(GT_VOID)
{
    FILE* fp;
    int num;
    char buffer[4*1024];

    fp = fopen("/proc/self/maps", "r");
    num = fread(buffer, 1, sizeof(buffer), fp);
    fclose(fp);
    fwrite(buffer, 1, num, stderr);
}

