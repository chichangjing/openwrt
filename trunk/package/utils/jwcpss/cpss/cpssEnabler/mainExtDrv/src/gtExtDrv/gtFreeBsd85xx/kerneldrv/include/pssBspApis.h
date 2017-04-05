/*******************************************************************************
*                   Copyright 2002, GALILEO TECHNOLOGY, LTD.                   *
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
* (MJKK), GALILEO TECHNOLOGY LTD. (GTL) AND GALILEO TECHNOLOGY, INC. (GTI).    *
********************************************************************************
* pssBspApis.h - bsp APIs
*
* DESCRIPTION:
*       Enable managment of cache memory 
*
* DEPENDENCIES:
*       None.
*       
* FILE REVISION NUMBER:
*       $Revision: 2 $
*
*******************************************************************************/

#ifndef __pssBspApisH
#define __pssBspApisH

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include <stddef.h> /* for size_t */

/* Defines */

#undef  IN
#define IN
#undef  OUT
#define OUT
#undef  INOUT
#define INOUT

/* In c++ compiler typedef void is not allowed therefore define is used */
#define GT_VOID void

typedef char GT_8;
typedef short GT_16;
typedef long GT_32;
typedef unsigned char GT_U8, *GT_U8_PTR;
typedef unsigned short GT_U16;
typedef unsigned long GT_U32;
typedef enum {GT_FALSE = 0, GT_TRUE = 1} GT_BOOL;
typedef unsigned int GT_STATUS;

typedef int 	(*GT_FUNCPTR) (void);	  /* ptr to function returning int   */
typedef void 	(*GT_VOIDFUNCPTR) (void *); /* ptr to function returning void  */
typedef double 	(*GT_DBLFUNCPTR) (void);  /* ptr to function returning double*/
typedef float 	(*GT_FLTFUNCPTR) (void);  /* ptr to function returning float */

#define GT_OK               (0x00)  /* Operation succeeded                   */

/*
 * Typedef: enum bspCacheType_ENT
 *
 * Description:
 *             This type defines used cache types 
 *
 * Fields: 
 *          bspCacheType_InstructionCache_E - cache of commands
 *          bspCacheType_DataCache_E        - cache of data
 *
 * Note:
 *      The enum has to be compatible with GT_MGMT_CACHE_TYPE_ENT.
 *
 */    
typedef enum 
{
    bspCacheType_InstructionCache_E,
    bspCacheType_DataCache_E
} bspCacheType_ENT;

/*
 * Description: Enumeration For PCI interrupt lines.
 *
 * Enumerations:
 *      bspPciInt_PCI_INT_A_E - PCI INT# A
 *      bspPciInt_PCI_INT_B_ - PCI INT# B
 *      bspPciInt_PCI_INT_C - PCI INT# C
 *      bspPciInt_PCI_INT_D - PCI INT# D
 *
 * Assumption:
 *      This enum should be identical to bspPciInt_PCI_INT.
 */
typedef enum
{
    bspPciInt_PCI_INT_A = 1,
    bspPciInt_PCI_INT_B,
    bspPciInt_PCI_INT_C,
    bspPciInt_PCI_INT_D
} bspPciInt_PCI_INT;

/*
 * Typedef: enum bspSmiAccessMode_ENT
 *
 * Description:
 *             PP SMI access mode. 
 *
 * Fields: 
 *          bspSmiAccessMode_Direct_E   - direct access mode (single/parallel)
 *          bspSmiAccessMode_inDirect_E - indirect access mode
 *
 * Note:
 *      The enum has to be compatible with GT_MGMT_CACHE_TYPE_ENT.
 *
 */    
typedef enum 
{
    bspSmiAccessMode_Direct_E,
    bspSmiAccessMode_inDirect_E
} bspSmiAccessMode_ENT;


/*******************************************************************************
* BSP_RX_CALLBACK_FUNCPTR
*
* DESCRIPTION:
*       The prototype of the routine to be called after a packet was received
*
* INPUTS:
*       segmentList     - A list of pointers to the packets segments.
*       segmentLen      - A list of segment length.
*       numOfSegments   - The number of segment in segment list.
*       queueNum        - the received queue number
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_TRUE if it has handled the input packet and no further action should 
*               be taken with it, or
*       GT_FALSE if it has not handled the input packet and normal processing.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
typedef GT_STATUS (*BSP_RX_CALLBACK_FUNCPTR)
(
    IN GT_U8_PTR   segmentList[],
    IN GT_U32      segmentLen[],   
    IN GT_U32      numOfSegments,
    IN GT_U32      queueNum
);

/*******************************************************************************
* BSP_TX_COMPLETE_CALLBACK_FUNCPTR
*
* DESCRIPTION:
*       The prototype of the routine to be called after a packet was received
*
* INPUTS:
*       segmentList     - A list of pointers to the packets segments.
*       numOfSegments   - The number of segment in segment list.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_TRUE if it has handled the input packet and no further action should 
*               be taken with it, or
*       GT_FALSE if it has not handled the input packet and normal processing.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
typedef GT_STATUS (*BSP_TX_COMPLETE_CALLBACK_FUNCPTR)
(
    IN GT_U8_PTR   segmentList[],
    IN GT_U32      numOfSegments
);

/*** reset ***/
/*******************************************************************************
* bspResetInit
*
* DESCRIPTION:
*       This routine calls in init to do system init config for reset.
*
* INPUTS:
*       none.
*
* OUTPUTS:
*       none.
*
* RETURNS:
*       GT_OK      - on success.
*       GT_FAIL    - otherwise.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS bspResetInit
(
    GT_VOID
);


/*******************************************************************************
* bspReset
*
* DESCRIPTION:
*       This routine calls to reset of CPU.
*
* INPUTS:
*       none.
*
* OUTPUTS:
*       none.
*
* RETURNS:
*       GT_OK      - on success.
*       GT_FAIL    - otherwise.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS bspReset
(
    GT_VOID
);

/*** cache ***/
/*******************************************************************************
* bspCacheFlush
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
*       GT_FAIL - othersise.
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS bspCacheFlush
(
    IN bspCacheType_ENT         cacheType, 
    IN void                     *address_PTR, 
    IN size_t                   size
);

/*******************************************************************************
* bspCacheInvalidate
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
*       GT_FAIL - othersise.
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS bspCacheInvalidate 
(
    IN bspCacheType_ENT         cacheType, 
    IN void                     *address_PTR, 
    IN size_t                   size
);

/*** DMA ***/
/*******************************************************************************
* bspDmaWrite
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
GT_STATUS bspDmaWrite
(
    IN  GT_U32  address,
    IN  GT_U32  *buffer,
    IN  GT_U32  length,
    IN  GT_U32  burstLimit
);

/*******************************************************************************
* bspDmaRead
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
GT_STATUS bspDmaRead
(
    IN  GT_U32  address,
    IN  GT_U32  length,
    IN  GT_U32  burstLimit,
    OUT GT_U32  *buffer
);

/*******************************************************************************
* bspCacheDmaMalloc
*
* DESCRIPTION:
*       Allocate a cache free area for DMA devices.
*
* INPUTS:
*       size_t bytes - number of bytes to allocate
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       pointer to allocated data per success
*       NULL - per failure to allocate space
*
* COMMENTS:
*       None
*
*******************************************************************************/
void *bspCacheDmaMalloc
(
    IN size_t bytes
);

/*******************************************************************************
* bspCacheDmaFree
*
* DESCRIPTION:
*       free a cache free area back to pool.
*
* INPUTS:
*       size_t bytes - number of bytes to allocate
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on error
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS bspCacheDmaFree
(
    IN void *pBuf
);

/*** PCI ***/
/*******************************************************************************
* bspPciConfigWriteReg
*
* DESCRIPTION:
*       This routine write register to the PCI configuration space.
*
* INPUTS:
*       busNo    - PCI bus number.
*       devSel   - the device devSel.
*       funcNo   - function number.
*       regAddr  - Register offset in the configuration space.
*       data     - data to write.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK   - on success,
*       GT_FAIL - othersise.
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS bspPciConfigWriteReg
(
    IN  GT_U32  busNo,
    IN  GT_U32  devSel,
    IN  GT_U32  funcNo,
    IN  GT_U32  regAddr,
    IN  GT_U32  data
);

/*******************************************************************************
* bspPciConfigReadReg
*
* DESCRIPTION:
*       This routine read register from the PCI configuration space.
*
* INPUTS:
*       busNo    - PCI bus number.
*       devSel   - the device devSel.
*       funcNo   - function number.
*       regAddr  - Register offset in the configuration space.
*
* OUTPUTS:
*       data     - the read data.
*
* RETURNS:
*       GT_OK   - on success,
*       GT_FAIL - othersise.
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS bspPciConfigReadReg
(
    IN  GT_U32  busNo,
    IN  GT_U32  devSel,
    IN  GT_U32  funcNo,
    IN  GT_U32  regAddr,
    OUT GT_U32  *data
);

/*******************************************************************************
* bspPciFindDev
*
* DESCRIPTION:
*       This routine returns the next instance of the given device (defined by
*       vendorId & devId).
*
* INPUTS:
*       vendorId - The device vendor Id.
*       devId    - The device Id.
*       instance - The requested device instance.
*
* OUTPUTS:
*       busNo    - PCI bus number.
*       devSel   - the device devSel.
*       funcNo   - function number.
*
* RETURNS:
*       GT_OK   - on success,
*       GT_FAIL - othersise.
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS bspPciFindDev
(
    IN  GT_U16  vendorId,
    IN  GT_U16  devId,
    IN  GT_U32  instance,
    OUT GT_U32  *busNo,
    OUT GT_U32  *devSel,
    OUT GT_U32  *funcNo
);

/*******************************************************************************
* bspPciGetIntVec
*
* DESCRIPTION:
*       This routine return the PCI interrupt vector.
*
* INPUTS:
*       pciInt - PCI interrupt number.
*
* OUTPUTS:
*       intVec - PCI interrupt vector.
*
* RETURNS:
*       GT_OK      - on success.
*       GT_FAIL    - otherwise.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS bspPciGetIntVec
(
    IN  bspPciInt_PCI_INT  pciInt,
    OUT void               **intVec
);

/*******************************************************************************
* bspPciGetIntMask
*
* DESCRIPTION:
*       This routine return the PCI interrupt vector.
*
* INPUTS:
*       pciInt - PCI interrupt number.
*
* OUTPUTS:
*       intMask - PCI interrupt mask.
*
* RETURNS:
*       GT_OK      - on success.
*       GT_FAIL    - otherwise.
*
* COMMENTS:
*       PCI interrupt mask should be used for interrupt disable/enable.
*
*******************************************************************************/
GT_STATUS bspPciGetIntMask
(
    IN  bspPciInt_PCI_INT  pciInt,
    OUT GT_U32             *intMask
);

/*******************************************************************************
* bspPciEnableCombinedAccess
*
* DESCRIPTION:
*       This function enables / disables the Pci writes / reads combining
*       feature.
*       Some system controllers support combining memory writes / reads. When a
*       long burst write / read is required and combining is enabled, the master
*       combines consecutive write / read transactions, if possible, and
*       performs one burst on the Pci instead of two. (see comments)
*
* INPUTS:
*       enWrCombine - GT_TRUE enables write requests combining.
*       enRdCombine - GT_TRUE enables read requests combining.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK               - on sucess,
*       GT_NOT_SUPPORTED    - if the controller does not support this feature,
*       GT_FAIL             - otherwise.
*
* COMMENTS:
*       1.  Example for combined write scenario:
*           The controller is required to write a 32-bit data to address 0x8000,
*           while this transaction is still in progress, a request for a write
*           operation to address 0x8004 arrives, in this case the two writes are
*           combined into a single burst of 8-bytes.
*
*******************************************************************************/
GT_STATUS bspPciEnableCombinedAccess
(
    IN  GT_BOOL     enWrCombine,
    IN  GT_BOOL     enRdCombine
);


/*** SMI ***/
/*******************************************************************************
* bspSmiInitDriver
*
* DESCRIPTION:
*       Init the TWSI interface 
*
* INPUTS:
*       None.
*
* OUTPUTS:
*       smiAccessMode - direct/indirect mode
*
* RETURNS:
*       GT_OK               - on success
*       GT_ERROR   - on hardware error
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS bspSmiInitDriver
(
    bspSmiAccessMode_ENT  *smiAccessMode
);

/*******************************************************************************
* bspSmiReadReg
*
* DESCRIPTION:
*       Reads a register from SMI slave.
*
* INPUTS:
*       devSlvId - Slave Device ID
*       actSmiAddr - actual smi addr to use (relevant for SX PPs)
*       regAddr - Register address to read from.
*
* OUTPUTS:
*       valuePtr     - Data read from register.
*
* RETURNS:
*       GT_OK               - on success
*       GT_ERROR   - on hardware error
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS bspSmiReadReg
(               
    IN  GT_U32  devSlvId,
    IN  GT_U32  actSmiAddr,
    IN  GT_U32  regAddr,
    OUT GT_U32 *valuePtr
);

/*******************************************************************************
* bspSmiWriteReg
*
* DESCRIPTION:
*       Writes a register to an SMI slave.
*
* INPUTS:
*       devSlvId - Slave Device ID
*       actSmiAddr - actual smi addr to use (relevant for SX PPs)
*       regAddr - Register address to read from.
*       value   - data to be written.
*
* OUTPUTS:
*        None,
*
* RETURNS:
*       GT_OK               - on success
*       GT_ERROR   - on hardware error
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS bspSmiWriteReg
(
    IN GT_U32 devSlvId,
    IN GT_U32 actSmiAddr,
    IN GT_U32 regAddr,
    IN GT_U32 value
);


/*** TWSI ***/
/*******************************************************************************
* bspTwsiInitDriver
*
* DESCRIPTION:
*       Init the TWSI interface 
*
* INPUTS:
*       None.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK               - on success
*       GT_ERROR   - on hardware error
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS bspTwsiInitDriver
(
    GT_VOID
);

/*******************************************************************************
* bspTwsiWaitNotBusy
*
* DESCRIPTION:
*       Wait for TWSI interface not BUSY
*
* INPUTS:
*       None.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK               - on success
*       GT_ERROR   - on hardware error
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS bspTwsiWaitNotBusy
(
    GT_VOID
);

/*******************************************************************************
* bspTwsiMasterReadTrans
*
* DESCRIPTION:
*       do TWSI interface Transaction 
*
* INPUTS:
*    devId - I2c slave ID                               
*    pData - Pointer to array of chars (address / data)
*    len   - pData array size (in chars).              
*    stop  - Indicates if stop bit is needed.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK               - on success
*       GT_ERROR   - on hardware error
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS bspTwsiMasterReadTrans
(
    IN GT_U8           devId,       /* I2c slave ID                              */ 
    IN GT_U8           *pData,      /* Pointer to array of chars (address / data)*/
    IN GT_U8           len,         /* pData array size (in chars).              */
    IN GT_BOOL         stop         /* Indicates if stop bit is needed in the end  */
);

/*******************************************************************************
* bspTwsiMasterWriteTrans
*
* DESCRIPTION:
*       do TWSI interface Transaction 
*
* INPUTS:
*    devId - I2c slave ID                               
*    pData - Pointer to array of chars (address / data)
*    len   - pData array size (in chars).              
*    stop  - Indicates if stop bit is needed.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK               - on success
*       GT_ERROR   - on hardware error
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS bspTwsiMasterWriteTrans
(
    IN GT_U8           devId,       /* I2c slave ID                              */ 
    IN GT_U8           *pData,      /* Pointer to array of chars (address / data)*/
    IN GT_U8           len,         /* pData array size (in chars).              */
    IN GT_BOOL         stop         /* Indicates if stop bit is needed in the end  */
);

/*** Ethernet Driver ***/
/*******************************************************************************
* bspEthPortRxInit
*
* DESCRIPTION: Init the ethernet port Rx interface
*
* INPUTS:
*       rxBufPoolSize   - buffer pool size
*       rxBufPool_PTR   - the address of the pool
*       rxBufSize       - the buffer requested size
*       numOfRxBufs_PTR - number of requested buffers, and actual buffers created
*       headerOffset    - packet header offset size
*       rxQNum          - the number of RX queues
*       rxQbufPercentage- the buffer percentage dispersal for all queues
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK if successful, or
*       GT_FAIL otherwise.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS bspEthPortRxInit
(
    IN GT_U32           rxBufPoolSize,
    IN GT_U8_PTR        rxBufPool_PTR,
    IN GT_U32           rxBufSize,
    INOUT GT_U32        *numOfRxBufs_PTR,
    IN GT_U32           headerOffset,
    IN GT_U32           rxQNum,
    IN GT_U32           rxQbufPercentage[]
);

/*******************************************************************************
* bspEthPortTxInit
*
* DESCRIPTION: Init the ethernet port Tx interface
*
* INPUTS:
*       numOfTxBufs - number of requested buffers
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK if successful, or
*       GT_FAIL otherwise.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS bspEthPortTxInit
(
    IN GT_U32           numOfTxBufs
);

/*******************************************************************************
* bspEthPortEnable
*
* DESCRIPTION: Enable the ethernet port interface
*
* INPUTS:
*       None.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK if successful, or
*       GT_FAIL otherwise.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS bspEthPortEnable
(
    GT_VOID
);

/*******************************************************************************
* bspEthPortDisable
*
* DESCRIPTION: Disable the ethernet port interface
*
* INPUTS:
*       None.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK if successful, or
*       GT_FAIL otherwise.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS bspEthPortDisable
(
    GT_VOID
);


/*******************************************************************************
* bspEthPortTx
*
* DESCRIPTION:
*       This function is called after a TxEnd event has been received, it passes
*       the needed information to the Tapi part.
*
* INPUTS:
*       segmentList     - A list of pointers to the packets segments.
*       segmentLen      - A list of segment length.
*       numOfSegments   - The number of segment in segment list.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK if successful, or
*       GT_FAIL otherwise.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS bspEthPortTx
(
    IN GT_U8_PTR        segmentList[],
    IN GT_U32           segmentLen[],   
    IN GT_U32           numOfSegments
);

/*******************************************************************************
* bspEthInputHookAdd
*
* DESCRIPTION:
*       This bind the user Rx callback
*
* INPUTS:
*       userRxFunc - the user Rx callback function
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK if successful, or
*       GT_FAIL otherwise.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS bspEthInputHookAdd
(
    IN BSP_RX_CALLBACK_FUNCPTR    userRxFunc
);

/*******************************************************************************
* bspEthTxCompleteHookAdd
*
* DESCRIPTION:
*       This bind the user Tx complete callback
*
* INPUTS:
*       userTxFunc - the user Tx callback function
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK if successful, or
*       GT_FAIL otherwise.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS bspEthTxCompleteHookAdd
(
    IN BSP_TX_COMPLETE_CALLBACK_FUNCPTR    userTxFunc
);

/*******************************************************************************
* bspEthRxPacketFree
*
* DESCRIPTION:
*       This routine frees the received Rx buffer. 
*
* INPUTS:
*       segmentList     - A list of pointers to the packets segments.
*       numOfSegments   - The number of segment in segment list.
*       queueNum        - Receive queue number
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK if successful, or
*       GT_FAIL otherwise.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS bspEthRxPacketFree
(
    IN GT_U8_PTR        segmentList[],
    IN GT_U32           numOfSegments,
    IN GT_U32           queueNum
);

/*******************************************************************************
* bspIntConnect
*
* DESCRIPTION:
*       Connect a specified C routine to a specified interrupt vector.
*
* INPUTS:
*       vector    - interrupt vector number to attach to
*       routine   - routine to be called
*       parameter - parameter to be passed to routine
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
GT_STATUS bspIntConnect
(
    IN  GT_U32           vector,
    IN  GT_VOIDFUNCPTR   routine,
    IN  GT_U32           parameter
);

/*******************************************************************************
* extDrvIntEnable
*
* DESCRIPTION:
*       Enable corresponding interrupt bits
*
* INPUTS:
*       intMask - new interrupt bits
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on error
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS bspIntEnable
(
    IN GT_U32   intMask
);

/*******************************************************************************
* extDrvIntDisable
*
* DESCRIPTION:
*       Disable corresponding interrupt bits.
*
* INPUTS:
*       intMask - new interrupt bits
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on error
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS bspIntDisable
(
    IN GT_U32   intMask
);



/*******************************************************************************
* bspIntLock
*
* DESCRIPTION:
*       Lock interrupts
*
* INPUTS:
*      None
*
* OUTPUTS:
*      None
*
* RETURNS:
*       Lock key for the interrupt level
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_U32 bspIntLock(GT_VOID);

/*******************************************************************************
* bspIntUnlock
*
* DESCRIPTION:
*       Unlock interrupts
*
* INPUTS:
*       Lock key for the interrupt level
*
* OUTPUTS:
*       None
*
* RETURNS:
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_U32 bspIntUnlock
(
    IN   GT_U32 key
);

/*******************************************************************************
* bspVirt2Phys
*
* DESCRIPTION:
*       Converts virtual address to physical.
*
* INPUTS:
*       vAddr - virtual address
*
* OUTPUTS:
*       None
*
* RETURNS:
*       physical address on success
*       0 - on error
*
* COMMENTS:
*
*******************************************************************************/
GT_U32 bspVirt2Phys
(
	IN GT_U32 vAddr
);

/*******************************************************************************
* bspPhys2Virt
*
* DESCRIPTION:
*       Converts physical address to virtual.
*
* INPUTS:
*       pAddr  - physical address
*
* OUTPUTS:
*       None
*
* RETURNS:
*       virtual address on success
*       0 - on error
*
* COMMENTS:
*
*******************************************************************************/
GT_U32 bspPhys2Virt
(
	IN GT_U32 pAddr
);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __pssBspApisH */
