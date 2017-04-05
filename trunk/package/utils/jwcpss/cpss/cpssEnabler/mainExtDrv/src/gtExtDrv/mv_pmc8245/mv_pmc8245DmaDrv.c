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
* ppc8240DmaDrv.c
*
* DESCRIPTION:
*       Includes DMA functions wrappers implementation for PPC-600 (8240)
*       controller.
*
* DEPENDENCIES:
*       None.
*
* FILE REVISION NUMBER:
*       $Revision: 2 $
*
*******************************************************************************/

#include <gtExtDrv/drivers/gtCacheMng.h>
#include <gtExtDrv/drivers/gtDmaDrv.h>
#include <gtExtDrv/drivers/gtMpc107Dma.h>


/*******************************************************************************
* extDrvDmaWrite
*
* DESCRIPTION:
*       Write a given buffer to the given address using the Dma.
*
* INPUTS:
*       address     - The destination address to write to.
*       buffer      - The buffer to be written.
*       length      - Length of buffer in words.
*       burstLimit  - Number of words to be written on each burst.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK   - on success,
*       GT_FAIL - othersise.
*
* COMMENTS:
*       1.  The given buffer is allways 4 bytes aligned, any further allignment
*           requirements should be handled internally by this function.
*       2.  The given buffer may be allocated from an uncached memory space, and
*           it's to the function to handle the cache flushing.
*       3.  The Prestera Driver assumes that the implementation of the DMA is
*           blocking, otherwise the Driver functionality might be damaged.
*
*******************************************************************************/
GT_STATUS extDrvDmaWrite
(
    IN  GT_UINTPTR  address,
    IN  GT_U32      *buffer,
    IN  GT_U32      length,
    IN  GT_U32      burstLimit
)
{
    GT_U32  sourceAddr;     /* The source address to begin the transfer from*/
    GT_U32  destAddr;       /* Destination address for the data to be       */
                            /* transfered.                                  */
    DMA_ENGINE  dmaEngine;  /* The Dma engine to perform the burst through. */
    DMA_STATUS  dmaStatus;

    GT_U32  numOfBursts = 0;

    /* Set the dma function parameters.     */
    dmaEngine   = DMA_ENG_0;
    sourceAddr  = (GT_U32)buffer;
    destAddr    = (GT_U32)address;

    /*osPrintf("src = 0x%x, dst = 0x%x, length = %d.\n",sourceAddr,destAddr,
             length);*/

    /* Wait until the Dma is Idle.          */
    while(mpc107DmaIsEngineActive(dmaEngine) != DMA_ENGINE_IDLE);

    /* Flush the buffer data    */
    extDrvMgmtCacheFlush(GT_MGMT_DATA_CACHE_E,buffer,length * sizeof(GT_U32));
                       
    numOfBursts = length / burstLimit;

    while ( numOfBursts )
    {
      dmaStatus = mpc107DmaTransfer(dmaEngine,sourceAddr,destAddr,burstLimit * 4);
      if(dmaStatus != DMA_OK)
      {
          return GT_FAIL;
      }

      sourceAddr += (burstLimit * 4);
      destAddr += (burstLimit * 4);
      numOfBursts--;

      /* Wait until the Dma is Idle.          */
      while(mpc107DmaIsEngineActive(dmaEngine) != DMA_ENGINE_IDLE);
    }

    return GT_OK;
}



/*******************************************************************************
* extDrvDmaRead
*
* DESCRIPTION:
*       Read a memory block from a given address.
*
* INPUTS:
*       address     - The address to read from.
*       length      - Length of the memory block to read (in words).
*       burstLimit  - Number of words to be read on each burst.
*
* OUTPUTS:
*       buffer  - The read data.
*
* RETURNS:
*       GT_OK   - on success,
*       GT_FAIL - othersise.
*
* COMMENTS:
*       1.  The given buffer is allways 4 bytes aligned, any further allignment
*           requirements should be handled internally by this function.
*       2.  The given buffer may be allocated from an uncached memory space, and
*           it's to the function to handle the cache flushing.
*       3.  The Prestera Driver assumes that the implementation of the DMA is
*           blocking, otherwise the Driver functionality might be damaged.
*
*******************************************************************************/
GT_STATUS extDrvDmaRead
(
    IN  GT_UINTPTR  address,
    IN  GT_U32      length,
    IN  GT_U32      burstLimit,
    OUT GT_U32      *buffer
)
{
    GT_U32  sourceAddr;     /* The source address to begin the transfer from*/
    GT_U32  destAddr;       /* Destination address for the data to be       */
                            /* transfered.                                  */
    DMA_ENGINE  dmaEngine;  /* The Dma engine to perform the burst through. */
    DMA_STATUS  dmaStatus;

    GT_U32  numOfBursts = 0;
        
    /* Set the dma function parameters.     */
    dmaEngine   = DMA_ENG_1;
    sourceAddr  = (GT_U32)address;
    destAddr    = (GT_U32)buffer;

    /*osPrintf("src = 0x%x, dst = 0x%x, length = %d.\n",sourceAddr,destAddr,
             length);*/

    /* Wait until the Dma is Idle.          */
    while(mpc107DmaIsEngineActive(dmaEngine) != DMA_ENGINE_IDLE);

    extDrvMgmtCacheFlush(GT_MGMT_DATA_CACHE_E,buffer,sizeof(GT_U32) * length);
    extDrvMgmtCacheInvalidate(GT_MGMT_DATA_CACHE_E,buffer,sizeof(GT_U32) * length);

    numOfBursts = length / burstLimit;

    while ( numOfBursts )
    { 
      dmaStatus = mpc107DmaTransfer(dmaEngine,sourceAddr,destAddr,burstLimit * 4);
      if(dmaStatus != DMA_OK)
      {
          return GT_FAIL;
      }

      sourceAddr += (burstLimit * 4);
      destAddr += (burstLimit * 4);
      numOfBursts--;  

      /* Wait until the Dma is Idle.          */
      while(mpc107DmaIsEngineActive(dmaEngine) != DMA_ENGINE_IDLE);
    }

    return GT_OK;
}

