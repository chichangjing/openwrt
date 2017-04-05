/*******************************************************************************
*                Copyright 2001, Marvell International Ltd.
* This code contains confidential information of Marvell semiconductor, inc.
* no rights are granted herein under any patent, mask work right or copyright
* of Marvell or any third party.
* Marvell reserves the right at its sole discretion to request that this code
* be immediately returned to Marvell. This code is provided "as is".
* Marvell makes no warranties, express, implied or otherwise, regarding its
* accuracy, completeness or performance.
********************************************************************************
* prvCpssGenNetIfMiiPool.c
*
* DESCRIPTION:
*       Ethernet port buffer management module. These pools are used by the
*       network interface for PP connected to NIC (like DXSAL,DXCH-smi).
*
* FILE REVISION NUMBER:
*       $Revision: 9 $
*******************************************************************************/


/********* include ************************************************************/
#include <cpss/generic/networkIf/cpssGenNetIfMii.h>
#include <cpss/generic/networkIf/private/prvCpssGenNetworkIfMii.h>
#include <cpssCommon/cpssBuffManagerPool.h>
#ifdef SHARED_MEMORY
#include <gtOs/gtOsMem.h>
#include <gtOs/gtOsSharedData.h>
#endif

#ifdef ETH_POOL_DEBUG
#define DBG_INFO(x)     osPrintf x
#else
#define DBG_INFO(x)
#endif

/************ Internal Typedefs ***********************************************/


/*
 * Typedef: struct GT_POOL_BUF_HEAD
 *
 * Description: Defines the header of a buffer in a pool.
 *
 * Fields:
 *      pNext - Pointer to the next free buffer in the pool.
 *
 */
typedef struct BufHead
{
    struct BufHead  *pNext;

}GT_ETH_PORT_POOL_FREE_BUF_HEAD;

/*
 * Typedef: struct gt_pool_head
 *
 * Description: Defines the pool ID
 *
 * Fields:
 *      poolBlock_PTR           - Pointer to the allocated memory block.
 *      alignedBlockStart_PTR   - Pointer to the aligned start address
 *                                of the pool's memory block.
 *      alignedBlockEnd_PTR     - Pointer to the aligned end address
 *                                of the pool's memory block.
 *      listHead_PTR            - Head of the buffers' list.
 *      poolSem                 - the pool semaphore
 *      memBlocSize             - Memory block size.
 *      reqBufferSize           - requested Buffer's size.
 *      neededBufferSize        - Actual buffer's size (after alignment).
 *      numOfBuffers            - Number of buffers in the pool.
 *      numOfFreeBuffers        - number of free buffers in the pool.
 */
typedef struct gt_pool_head
{
    GT_VOID                        *poolBlock_PTR;
    GT_VOID                        *alignedBlockStart_PTR;
    GT_VOID                        *alignedBlockEnd_PTR;
    CPSS_OS_SIG_SEM                 poolSem;
    GT_ETH_PORT_POOL_FREE_BUF_HEAD *listHead_PTR;
    GT_U32                          memBlocSize;
    GT_U32                          reqBufferSize;
    GT_U32                          neededBufferSize;
    GT_U32                          numOfBuffers;
    GT_U32                          numOfFreeBuffers;
}GT_ETH_PORT_POOL_HEAD;

#define ALIGN_4B_MASK_CNS   0x3 /* 00000011 */
#define ALIGN_4B_CNS        4

#ifndef SHARED_MEMORY
/* Array of pools */
static GT_ETH_PORT_POOL_HEAD poolArr[PRV_CPSS_GEN_NETIF_MII_MAX_POOLS_E];
#else
GT_ETH_PORT_POOL_HEAD * poolArrPtr;
#endif

/*******************************************************************************
* prvCpssGenNetIfMiiCreatePool
*
* DESCRIPTION:
*       Create a pool of buffers with pre-allocated memory or allocated memory.
*       Align the pool and buffers.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; ExMxPm; ExMx.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       poolIdx         - the pool index
*       reqBufferSize   - Buffer's requested size
*       numOfBuffersPtr - (pointer to)number of requested buffers in the pool
*       poolPtr         - (pointer to)pool memory allocted by user
*       poolSize        - size of the pointed pool
*       mallocFunc      - Function for allocating the buffers
*
* OUTPUTS:
*       numOfBuffersPtr - (pointer to)number of buffers actually initialized in the pool
*
* RETURNS:
*       GT_OK           - For successful operation.
*       GT_BAD_VALUE    - incorrect pool index.
*       GT_NO_RESOURCE  - No memory is available to create the pool.
*       GT_NOT_INITIALIZED - In case of un-initialized package.
*
* COMMENTS:
*       If dynamic allocation, the pool size is calculated from needed buffers
*       and buffer size. If the number of buffer differs from request number,
*       the routine does not fail but returns the actual buffers used.
*
*******************************************************************************/
GT_STATUS prvCpssGenNetIfMiiCreatePool
(
    IN      PRV_CPSS_GEN_NETIF_MII_POOL_TYPE_ENT    poolIdx,
    IN      GT_U32                                  reqBufferSize,
    INOUT   GT_U32                                  *numOfBuffersPtr,
    IN      GT_U8                                   *poolPtr,
    IN      GT_U32                                  poolSize,
    IN      PRV_CPSS_NETIF_MII_RX_BUFF_MALLOC_FUNC  mallocFunc
)
{
    GT_U32                          neededBufferSize;
    GT_U32                          neededPoolSize;
    GT_U32                          indx;
    GT_ETH_PORT_POOL_FREE_BUF_HEAD *buff_PTR;
    GT_ETH_PORT_POOL_HEAD          *head_PTR;
    GT_U8                          *tmp_PTR;

#ifdef SHARED_MEMORY
    GT_STATUS                       rc;

    rc = cpssMultiProcSharedDataLock();
    if (rc != GT_OK)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_FAIL, LOG_ERROR_NO_MSG);
    }
    poolArrPtr = (GT_ETH_PORT_POOL_HEAD*)
        cpssMultiProcGetSharedData(CPSS_SHARED_DATA_ETHPORT_POOLARRAY_PTR_E);
    if (poolArrPtr == NULL)
    {
        poolArrPtr = (GT_ETH_PORT_POOL_HEAD*)
            osMalloc(sizeof (*poolArrPtr) * PRV_CPSS_GEN_NETIF_MII_MAX_POOLS_E);
        rc = cpssMultiProcSetSharedData(
                CPSS_SHARED_DATA_ETHPORT_POOLARRAY_PTR_E,
                (uintptr_t)poolArrPtr);
        if (rc != GT_OK)
        {
            rc = cpssMultiProcSharedDataUnlock();
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_FAIL, LOG_ERROR_NO_MSG); 
        }
    }
    rc = cpssMultiProcSharedDataUnlock();
    if (rc != GT_OK)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_FAIL, LOG_ERROR_NO_MSG);
    }
#endif

    buff_PTR = NULL;

    /* validate pool index */
    if (poolIdx >= PRV_CPSS_GEN_NETIF_MII_MAX_POOLS_E)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_VALUE, LOG_ERROR_NO_MSG);
    }

    /* The numOfBuffersPtr MUST not be zero. */
    if (0 == *numOfBuffersPtr)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_VALUE, LOG_ERROR_NO_MSG);
    }


#ifndef SHARED_MEMORY
    /* set the pool */
    head_PTR = &poolArr[poolIdx];
#else
    head_PTR = &poolArrPtr[poolIdx];
#endif

    /* Align the buffer size */
    neededBufferSize = (reqBufferSize + ALIGN_4B_MASK_CNS) & (~ALIGN_4B_MASK_CNS);

    /* allocate memory chunk for the pool if user did not allocate allready */
    if (NULL == poolPtr)
    {
        if (mallocFunc == NULL)
        {
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_FAIL, LOG_ERROR_NO_MSG);
        }
        neededPoolSize = (*numOfBuffersPtr) * neededBufferSize;
        poolPtr = mallocFunc(neededPoolSize, ALIGN_4B_CNS);
        if (NULL == poolPtr)
        {
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NO_RESOURCE, LOG_ERROR_NO_MSG);
        }
    }
    else
    {
        neededPoolSize = poolSize;
    }

    /* Align the memory pool block if needed */
    if ((ALIGN_4B_MASK_CNS & (GT_UINTPTR)poolPtr) != 0)
    {
        /* align the memory pool block */
        tmp_PTR = poolPtr;
        poolPtr = (GT_U8*)((GT_UINTPTR)(poolPtr + ALIGN_4B_MASK_CNS) &
                            (~((GT_UINTPTR)ALIGN_4B_MASK_CNS)));
        neededPoolSize -= (GT_U32)(poolPtr - tmp_PTR);
    }

    /* update the actual number of buffers in the pool */
    *numOfBuffersPtr = neededPoolSize / neededBufferSize;

    /* validate that we did not run out of buffers */
    if (0 == (*numOfBuffersPtr))
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NO_RESOURCE, LOG_ERROR_NO_MSG);
    }

    /* now we have a valid pool size, number of buffers and buffer size */

    /* create semaphore for the pool get/free routines */
    if (cpssOsSigSemBinCreate(poolIdx == PRV_CPSS_GEN_NETIF_MII_RX_SEG_POOL_E ? "rxPool" : "txPool",
                       1, &head_PTR->poolSem) != GT_OK)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NO_RESOURCE, LOG_ERROR_NO_MSG);
    }

    /* Store the memory block boundaries, block memory address and its size */
    head_PTR->poolBlock_PTR         = poolPtr;
    head_PTR->memBlocSize           = poolSize;
    head_PTR->alignedBlockStart_PTR = poolPtr;
    head_PTR->alignedBlockEnd_PTR   = poolPtr + neededPoolSize - 1;
    head_PTR->reqBufferSize         = reqBufferSize;
    head_PTR->neededBufferSize      = neededBufferSize;
    head_PTR->numOfBuffers          = *numOfBuffersPtr;
    head_PTR->numOfFreeBuffers      = *numOfBuffersPtr;

    /* The first buffer is located in the beginning of the memory block */
    head_PTR->listHead_PTR = (GT_ETH_PORT_POOL_FREE_BUF_HEAD*)poolPtr;

    /* Devide the memory block into buffers and make it as a link list */
    for (indx=0; indx < (*numOfBuffersPtr); indx++)
    {
        buff_PTR = (GT_ETH_PORT_POOL_FREE_BUF_HEAD*)poolPtr;
        poolPtr = poolPtr + neededBufferSize;
        buff_PTR->pNext = (GT_ETH_PORT_POOL_FREE_BUF_HEAD*)poolPtr;
    }

    /* Last buffer is pointing to NULL */
    buff_PTR->pNext = NULL;

    return GT_OK;
}


/*******************************************************************************
* prvCpssGenNetIfMiiPoolGetBuf
*
* DESCRIPTION:
*       Return pointer to a buffer from the requested pool
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; ExMxPm; ExMx.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       poolIdx - The pool index.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_VOID* - Pointer to the new allocated buffer. NULL is returned in case
*                  no buffer is not available.
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_VOID * prvCpssGenNetIfMiiPoolGetBuf
(
    IN  PRV_CPSS_GEN_NETIF_MII_POOL_TYPE_ENT poolIdx
)
{
    GT_ETH_PORT_POOL_FREE_BUF_HEAD *buf_PTR;
    GT_ETH_PORT_POOL_HEAD          *head_PTR;

    if(poolIdx >= PRV_CPSS_GEN_NETIF_MII_MAX_POOLS_E)
    {
        return NULL;
    }

#ifndef SHARED_MEMORY
    /* set the pool */
    head_PTR = &poolArr[poolIdx];
#else
    head_PTR = &poolArrPtr[poolIdx];
#endif

    /* lock section, wait for ever */
    cpssOsSigSemWait(head_PTR->poolSem, 0);

    /* Take the buffer from the head of the list */
    buf_PTR = head_PTR->listHead_PTR;

    /* if the list is not empty, update the header */
    if (buf_PTR != NULL)
    {
        head_PTR->listHead_PTR = buf_PTR->pNext;

#ifdef ETH_POOL_DEBUG
        if (head_PTR->numOfFreeBuffers == 0)
            osPrintf("List and counter not in sync, pool= %d\n", poolIdx);
#endif

        /* Update number of free buffers */
        head_PTR->numOfFreeBuffers--;
    }
#ifdef ETH_POOL_DEBUG
    else
    {
        if (head_PTR->numOfFreeBuffers != 0)
            osPrintf("List and counter not in sync, pool= %d cnt= %d\n",
                     poolIdx, head_PTR->numOfFreeBuffers);
    }
#endif

    /* unlock section */
    cpssOsSigSemSignal(head_PTR->poolSem);

    return buf_PTR;
}


/*******************************************************************************
* prvCpssGenNetIfMiiPoolFreeBuf
*
* DESCRIPTION:
*       Free a buffer back to its pool.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; ExMxPm; ExMx.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       poolIdx - The pool index.
*       bufPtr  - Pointer to buffer to be freed
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK               - For successful operation.
*       GT_BAD_PTR          - The returned buffer does not belong to this pool
*       GT_NOT_INITIALIZED  - In case of un-initialized package.
*       GT_BAD_VALUE        - incorrect pool index.
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS prvCpssGenNetIfMiiPoolFreeBuf
(
    IN  PRV_CPSS_GEN_NETIF_MII_POOL_TYPE_ENT   poolIdx,
    IN  GT_VOID                                *bufPtr
)
{
    GT_ETH_PORT_POOL_FREE_BUF_HEAD  *retBuf_PTR;
    GT_ETH_PORT_POOL_HEAD           *head_PTR;

    if (poolIdx >= PRV_CPSS_GEN_NETIF_MII_MAX_POOLS_E)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_VALUE, LOG_ERROR_NO_MSG);
    }

#ifndef SHARED_MEMORY
    /* set the pool */
    head_PTR = &poolArr[poolIdx];
#else
    head_PTR = &poolArrPtr[poolIdx];
#endif

    /* pool boundaries check */
    if ((GT_U8*)bufPtr < (GT_U8*)head_PTR->alignedBlockStart_PTR ||
        (GT_U8*)bufPtr > (GT_U8*)head_PTR->alignedBlockEnd_PTR)
    {
        DBG_INFO(("free: boundry fail, pool= %d\n",poolIdx));
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PTR, LOG_ERROR_NO_MSG);
    }

    /* buffer alignment check */
    if (((GT_U8*)bufPtr - (GT_U8*)head_PTR->alignedBlockStart_PTR) %
        head_PTR->neededBufferSize != 0)
    {
        DBG_INFO(("free: block align fail, pool= %d\n",poolIdx));
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PTR, LOG_ERROR_NO_MSG);
    }

    /* lock section, wait for ever */
    cpssOsSigSemWait(head_PTR->poolSem, 0);

    /* Insert the returned buffer to the head of the list */
    retBuf_PTR = bufPtr;
    retBuf_PTR->pNext = head_PTR->listHead_PTR;
    head_PTR->listHead_PTR = retBuf_PTR;

    /* Update number of free buffers */
    head_PTR->numOfFreeBuffers++;

#ifdef ETH_POOL_DEBUG
    if (head_PTR->numOfFreeBuffers > head_PTR->numOfBuffers)
        osPrintf("Too many buffers, pool= %d, free= %d max= %d\n",
                 poolIdx, head_PTR->numOfFreeBuffers, head_PTR->numOfBuffers);
#endif

    /* unlock section */
    cpssOsSigSemSignal(head_PTR->poolSem);

    return GT_OK;
}


/*******************************************************************************
* prvCpssGenNetIfMiiPoolGetBufSize
*
* DESCRIPTION:
*       Get the buffer size.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; ExMxPm; ExMx.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       poolIdx - The pool index.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_U32  - the buffer size.
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_U32 prvCpssGenNetIfMiiPoolGetBufSize
(
    IN  PRV_CPSS_GEN_NETIF_MII_POOL_TYPE_ENT   poolIdx
)
{
    if (poolIdx >= PRV_CPSS_GEN_NETIF_MII_MAX_POOLS_E)
        return 0;
    else
#ifndef SHARED_MEMORY
    /* set the pool */
    return poolArr[poolIdx].reqBufferSize;
#else
    return  poolArrPtr[poolIdx].reqBufferSize;
#endif
}

#ifdef ETH_POOL_DEBUG
/*******************************************************************************
* appDemoEthPortPoolPrintfInfo
*
* DESCRIPTION:
*       Helper function to print all pool (rx/tx) information
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; ExMxPm; ExMx.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       None
*
* OUTPUTS:
*       None
*
* RETURNS:
*       None.
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_U32 prvCpssGenNetIfMiiPoolPrintfInfo(GT_VOID)
{
    GT_ETH_PORT_POOL_HEAD    * head_PTR;
    GT_U32            i;
    static GT_BOOL    first = GT_TRUE;

    if (first == GT_TRUE)
    {
        first = GT_FALSE;

        cpssOsPrintf("     |=========== Requested ===========| |=============Aligned ============|\n");
        cpssOsPrintf("Pool start    end      size     BuffSize start    end      size     BuffSize\n");

        for (i = 0 ; i < PRV_CPSS_GEN_NETIF_MII_MAX_POOLS_E ; i++)
        {
            head_PTR= &(poolArr[i]);

            cpssOsPrintf("%-4d %-8X %-8X %-8d %-8d %-8X %-8X %-8d %-8d\n",
                          i,
                          (GT_U32)(head_PTR->poolBlock_PTR),
                          (GT_U32)(head_PTR->poolBlock_PTR) + head_PTR->memBlocSize - 1,
                          head_PTR->memBlocSize,
                          head_PTR->reqBufferSize,
                          (GT_U32)(head_PTR->alignedBlockStart_PTR),
                          (GT_U32)(head_PTR->alignedBlockEnd_PTR),
                          head_PTR->numOfBuffers * head_PTR->neededBufferSize,
                          head_PTR->neededBufferSize);
        }
    }

    cpssOsPrintf("Pool Buffers Free\n");
    for (i = 0 ; i < PRV_CPSS_GEN_NETIF_MII_MAX_POOLS_E ; i++)
    {
        head_PTR= &(poolArr[i]);
        cpssOsPrintf("%-4d %-7d %-7d\n",
                        i,
                        head_PTR->numOfBuffers,
                        head_PTR->numOfFreeBuffers);
    }
    return GT_OK;
}
#endif
