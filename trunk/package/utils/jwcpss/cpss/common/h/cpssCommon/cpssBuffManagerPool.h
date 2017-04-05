/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* cpssBuffManagerPool.h
*
* DESCRIPTION:
*       Operating System wrapper for buffer management pool.
*
* FILE REVISION NUMBER:
*       $Revision: 2 $
*
*******************************************************************************/
#ifndef __cpssBuffManagerPoolh
#define __cpssBuffManagerPoolh

#ifdef __cplusplus
extern "C" {
#endif

/************* Includes *******************************************************/
#include <cpss/extServices/os/gtOs/gtGenTypes.h>

/************* Define *********************************************************/

#define CPSS_BM_POOL_NULL_ID  ((CPSS_BM_POOL_ID)NULL)



/************* Typedef ********************************************************/


/*
 * Typedef: CPSS_BM_POOL_ID
 *
 * Description:
 *  Define type for pool ID.
 *  The pool ID is allocated and returned to the user by the poolCreate function.
 *  In order to delet pool, allocate buffer or free buffer, the pool ID is given as
 *  parametr to those service functions.
 *
 */
typedef GT_VOID* CPSS_BM_POOL_ID;

/*
 * Typedef: enum GT_POOL_ALIGNMENT
 *
 * Description:
 *  Define the buffer alignment supported by the module.
 *
 */
typedef enum
{
    CPSS_BM_POOL_1_BYTE_ALIGNMENT_E  = 0x1, /* 00000001 */
    CPSS_BM_POOL_4_BYTE_ALIGNMENT_E  = 0x3, /* 00000011 */
    CPSS_BM_POOL_8_BYTE_ALIGNMENT_E  = 0x7, /* 00000111 */
    CPSS_BM_POOL_16_BYTE_ALIGNMENT_E = 0xF  /* 00001111 */

}CPSS_BM_POOL_ALIGNMENT_ENT;



/*******************************************************************************
* cpssBmPoolCreate
*
* DESCRIPTION:
*           Create pool of buffers. All buffers have the same size.
*
* INPUTS:
*           bufferSize   - Buffer's size
*           alignment    - Buffers' alignment
*           numOfBuffers - number of buffer at the pool
*
* OUTPUTS:
*           pPoolId       - Return the pool ID
* RETURNS:
*       GT_OK - For successful operation.
*       GT_BAD_VALUE - Request for zero number of buffers or
*                      buffer's size is smaller than the size of
*                      pointer (usually 4 bytes)
*       GT_BAD_PTR - Bad pointer for the pPoolId
*       GT_NO_RESOURCE - No memory is available to create the pool.
*       GT_NOT_INITIALIZED - In case of un-initialized package.
*
* COMMENTS:
*       Usage of this function is only during FIRST initialization.
*
*******************************************************************************/
GT_STATUS cpssBmPoolCreate
(
    IN   GT_U32                         reqBufferSize,
    IN   CPSS_BM_POOL_ALIGNMENT_ENT     alignment,
    IN   GT_U32                         numOfBuffers,
    OUT  CPSS_BM_POOL_ID                *pPoolId
);

/*******************************************************************************
* cpssBmPoolCreateDma
*
* DESCRIPTION:
*           Create pool of buffers. All buffers have the same size.
*           The buffers are created in a phsycally contigious area.
*
* INPUTS:
*           bufferSize   - Buffer's size
*           alignment    - Buffers' alignment
*           numOfBuffers - number of buffer at the pool
*
* OUTPUTS:
*           pPoolId       - Return the pool ID
* RETURNS:
*       GT_OK - For successful operation.
*       GT_BAD_VALUE - Request for zero number of buffers or
*                      buffer's size is smaller than the size of
*                      pointer (usually 4 bytes)
*       GT_BAD_PTR - Bad pointer for the pPoolId
*       GT_NO_RESOURCE - No memory is available to create the pool.
*       GT_NOT_INITIALIZED - In case of un-initialized package.
*
* COMMENTS:
*       Usage of this function is only during FIRST initialization.
*
*******************************************************************************/
GT_STATUS cpssBmPoolCreateDma
(
    IN   GT_U32                     reqBufferSize,
    IN   CPSS_BM_POOL_ALIGNMENT_ENT alignment,
    IN   GT_U32                     numOfBuffers,
    OUT  CPSS_BM_POOL_ID            *pPoolId
);

/*******************************************************************************
* cpssBmPoolReCreate
*
* DESCRIPTION:
*           Create pool of buffers. All buffers have the same size.
*
* INPUTS:
*           poolId    - Id of the pool to be re-created.

* OUTPUTS:
*           None.
* RETURNS:
*       GT_OK        - For successful operation.
*       GT_BAD_VALUE - Bad poolId
*       GT_NOT_INITIALIZED - In case of un-initialized package.
*
* COMMENTS:
*       Usage of this function is only during FIRST initialization.
*
*******************************************************************************/
GT_STATUS cpssBmPoolReCreate
(
    IN  CPSS_BM_POOL_ID  poolId
);

/*******************************************************************************
* cpssBmPoolDelete
*
* DESCRIPTION:
*           Delete a pool and free all the memory occupied by that pool.
*           The operation is failed if not all the buffers belong to that pool
*           have been freed.
* INPUTS:
*           PoolId - The pool ID as returned by the create function.
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK - For successful operation.
*       GT_BAD_STATE - Not all buffers belong to that pool have
*                      been freed.
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS cpssBmPoolDelete
(
    IN  CPSS_BM_POOL_ID  poolId
);

/*******************************************************************************
* cpssBmPoolBufGet
*
* DESCRIPTION:
*           Return pointer to a buffer from the requested pool
*
* INPUTS:
*           PoolId - The pool ID as returned by the create function.
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_VOID* - Pointer to the new allocated buffer. NULL is returned in case
*               no buffer is not available.
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_VOID *cpssBmPoolBufGet
(
    IN  CPSS_BM_POOL_ID poolId
);

/*******************************************************************************
* cpssBmPoolBufFree
*
* DESCRIPTION:
*           Free a buffer back to its pool.
*
* INPUTS:
*           PoolId - The pool ID as returned by the create function.
*           pBuf   - Pointer to buffer to be freed
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK - For successful operation.
*       GT_BAD_PTR - The returned buffer is not belongs to that pool
*       GT_NOT_INITIALIZED - In case of un-initialized package.
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS cpssBmPoolBufFree
(
    IN  CPSS_BM_POOL_ID poolId,
    IN  GT_VOID   *pBuf
);

/*******************************************************************************
* cpssBmPoolBufSizeGet
*
* DESCRIPTION:
*           Get the buffer size.
*
* INPUTS:
*           PoolId - The pool ID as returned by the create function.
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
GT_U32 cpssBmPoolBufSizeGet
(
    IN        CPSS_BM_POOL_ID  poolId
);

/*******************************************************************************
* cpssBmPoolExpand
*
* DESCRIPTION:
*           Expand a pool of buffers. All buffers have the same size.
*
* INPUTS:
*           PoolId - The pool ID as returned by the create function.
*           forDma       - is physically contigious.
*           numOfBuffers - number of buffer to add to the pool
*
* OUTPUTS:
*       none.
* RETURNS:
*       GT_OK - For successful operation.
*       GT_BAD_VALUE - Request for zero number of buffers or
*                      buffer's size is smaller than the size of
*                      pointer (usually 4 bytes)
*       GT_BAD_PTR - Bad pointer for the pPoolId
*       GT_NO_RESOURCE - No memory is available to create the pool.
*       GT_NOT_INITIALIZED - In case of un-initialized package.
*
* COMMENTS:
*       not used for Dma buffers!
*
*******************************************************************************/
GT_STATUS cpssBmPoolExpand
(
    IN   CPSS_BM_POOL_ID        poolId,
    IN   GT_U32            numOfBuffers
);


/*******************************************************************************
* cpssBmPoolBufFreeCntGet
*
* DESCRIPTION:
*           Get the free buffer count.
*
* INPUTS:
*           PoolId - The pool ID as returned by the create function.
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_U32  - the free buffer count.
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_U32 cpssBmPoolBufFreeCntGet
(
    IN        CPSS_BM_POOL_ID  poolId
);

/************ Private Functions ************************************************/

/*******************************************************************************
* cpssBmPoolPrintStats
*
* DESCRIPTION:
*           Print pool's statistics for the requested pool, or for all
*           the pools in case poolID is CPSS_BM_POOL_NULL_ID.
*
* INPUTS:
*           PoolId      - The pool ID as returned by the create function.
*                         or CPSS_BM_POOL_NULL_ID to print all pools' information.
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
void cpssBmPoolPrintStats
(
    IN  CPSS_BM_POOL_ID poolId
);


#ifdef __cplusplus
}
#endif

#endif  /* __cpssBuffManagerPoolh */


