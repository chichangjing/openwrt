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
* gtBmPool.h
*
* DESCRIPTION:
*       Operating System wrapper for buffer management pool.
*
* DEPENDENCIES:
*       None.
*
* FILE REVISION NUMBER:
*       $Revision: 9 $
*******************************************************************************/

#ifndef __gtBmPoolh
#define __gtBmPoolh

#ifdef __cplusplus
extern "C" {
#endif

/************* Includes *******************************************************/

#include <gtOs/gtGenTypes.h>


#if (!defined __cmdExtServices_h_) || (defined PSS_PRODUCT)

/************* Define *********************************************************/

#define GT_POOL_NULL_ID  ((GT_POOL_ID)NULL)



/************* Typedef ********************************************************/


/*
 * Typedef: GT_POOL_ID
 *
 * Description:
 *  Define type for pool ID.
 *  The pool ID is allocated and returned to the user by the poolCreate function.
 *  In order to delet pool, allocate buffer or free buffer, the pool ID is given as
 *  parametr to those service functions.
 *
 */
typedef GT_VOID* GT_POOL_ID;

/*
 * Typedef: enum GT_POOL_ALIGNMENT
 *
 * Description:
 *  Define the buffer alignment supported by the module.
 *
 */
typedef enum
{
    GT_1_BYTE_ALIGNMENT  = 0x1, /* 00000001 */
    GT_4_BYTE_ALIGNMENT  = 0x3, /* 00000011 */
    GT_8_BYTE_ALIGNMENT  = 0x7, /* 00000111 */
    GT_16_BYTE_ALIGNMENT = 0xF  /* 00001111 */

}GT_POOL_ALIGNMENT;

#endif /* __cmdExtServices_h_ */

/************* Functions ******************************************************/


/*******************************************************************************
* gtPoolInit
*
* DESCRIPTION:
*           Used to initialize the pool module
*
* INPUTS:
*         SemProtected - Flag indicate if the module should be
*                        semaphore protected
*         maxPool      - Max number of pools supported by this module.
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK          - For successful operation.
*       GT_BAD_VALUE   - In case of bad alignment value
*       GT_INIT_ERROR  - In case re-initialization.
*       GT_NO_RESOURCE - Not enough memory or semaphores.
*
* COMMENTS:
*       Usage of this function is only during FIRST initialization.
*
*******************************************************************************/
GT_STATUS  gtPoolInit
(
   IN   GT_U32             maxPools
);



/*******************************************************************************
* gtPoolCreatePool
*
* DESCRIPTION:
*           Create pool of buffers. All buffers have the same size.
*
* INPUTS:
*           bufferSize   - Buffer's size
*           alignment    - Buffers' alignment
*           numOfBuffers - number of buffer at the pool
*           useSem       - GT_TRUE to use a semaphore for mutual exclusion on
*                         access to the pool.
*                  Note: Semaphore protection is one for all pools.
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
GT_STATUS gtPoolCreatePool
(
    IN   GT_U32            reqBufferSize,
    IN   GT_POOL_ALIGNMENT alignment,
    IN   GT_U32            numOfBuffers,
    IN   GT_BOOL           useSem,
    OUT  GT_POOL_ID       *pPoolId
);

/*******************************************************************************
* gtPoolCreateDmaPool
*
* DESCRIPTION:
*           Create pool of buffers. All buffers have the same size.
*           The buffers are created in a phsycally contigious area.
*
* INPUTS:
*           bufferSize   - Buffer's size
*           alignment    - Buffers' alignment
*           numOfBuffers - number of buffer at the pool
*           useSem       - GT_TRUE to use a semaphore for mutual exclusion on
*                         access to the pool.
*                  Note: Semaphore protection is one for all pools.
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
GT_STATUS gtPoolCreateDmaPool
(
    IN   GT_U32            reqBufferSize,
    IN   GT_POOL_ALIGNMENT alignment,
    IN   GT_U32            numOfBuffers,
    IN   GT_BOOL           useSem,
    OUT  GT_POOL_ID       *pPoolId
);

/*******************************************************************************
* gtPoolReCreatePool
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
GT_STATUS gtPoolReCreatePool
(
    IN  GT_POOL_ID  poolId
);

/*******************************************************************************
* gtPoolDeletePool
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
GT_STATUS gtPoolDeletePool
(
    IN  GT_POOL_ID  poolId
);

/*******************************************************************************
* gtPoolGetBuf
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
GT_VOID* gtPoolGetBuf
(
    IN        GT_POOL_ID  poolId
);


/*******************************************************************************
* gtPoolFreeBuf
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
*       GT_STATUS  //  GT_OK - For successful operation.
*                      GT_BAD_PTR - The returned buffer is not belongs to that pool
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS gtPoolFreeBuf
(
    IN        GT_POOL_ID  poolId,
    IN        GT_VOID*    pBuf
);


/*******************************************************************************
* gtPoolGetBufSize
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
GT_U32 gtPoolGetBufSize
(
    IN        GT_POOL_ID  poolId
);

/*******************************************************************************
* gtPoolExpandPool
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
GT_STATUS gtPoolExpandPool
(
    IN   GT_POOL_ID        pPoolId,
    IN   GT_U32            numOfBuffers
);


/*******************************************************************************
* gtPoolGetBufFreeCnt
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
GT_U32 gtPoolGetBufFreeCnt
(
    IN        GT_POOL_ID  poolId
);


/*******************************************************************************
* gtPoolPrintStats
*
* DESCRIPTION:
*           Print pool's statistics for the requested pool, or for all
*           the pols in case poolID is GT_POOL_NULL_ID.
*
* INPUTS:
*           PoolId      - The pool ID as returned by the create function.
*                         or GT_POOL_NULL_ID to print all pools' information.
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
void gtPoolPrintStats
(
    IN  GT_POOL_ID poolId
);


#ifdef __cplusplus
}
#endif

#endif  /* __gtBmPoolh */
/* Do Not Add Anything Below This Line */

