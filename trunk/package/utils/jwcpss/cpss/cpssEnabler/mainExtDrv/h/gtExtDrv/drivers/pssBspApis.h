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
*       API's supported by BSP/LSP/FSP
*
* DEPENDENCIES:
*       None.
*
* FILE REVISION NUMBER:
*       $Revision: 17 $
*
*******************************************************************************/

#ifndef __pssBspApisH
#define __pssBspApisH

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include <stddef.h> /* for size_t */

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

/*
 * Typedef: enum bspEthTxMode_ENT
 *
 * Description:
 *             MII transmission mode
 *
 * Fields:
 *          bspEthTxMode_asynch_E - execute asynchroniouse packet send
 *          bspEthTxMode_synch_E  - execute synchroniouse packet send
 *
 * Note:
 *
 */
typedef enum
{
        bspEthTxMode_asynch_E = 0,
        bspEthTxMode_synch_E
} bspEthTxMode_ENT;

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

/*
 * Typedef: GT_BUF_INFO
 *
 * Description:
 *       This structure contains information describing one of buffers
 *       (fragments) they are built Ethernet packet.
 *
 * Fields:
 *          bufVirtPtr  - pointer to buffer
 *          bufPhysAddr - physical address of buffer
 *          bufSize     - buffer memory size
 *          dataSize    - size of actual data placed in buffer
 *          memHandle   - for backword compatibility
 *
 * Note:
 *
 */
typedef struct
{
     GT_U8*     bufVirtPtr;
     unsigned long bufPhysAddr;
     GT_U32     bufSize;
     GT_U32     dataSize;
     GT_U32     memHandle;
} GT_BUF_INFO;

/*
 * Typedef: GT_PKT_INFO
 *
 * Description:
 *      This structure contains information describing Ethernet packet.
 *      The packet can be divided for few buffers (fragments)
 *
 * Fields:
 *          osInfo  - identification magic number;
 *          *pFrags - array of buffers
 *          status  - packet status
 *          pktSize - packet length
 *          numFrags - number of buffers used for this packet
 *          srcIdx  - for backword compatibility
 *          fragIP  - is packet IP fragmented
 *
 * Note:
 *
 */
typedef struct
{
    unsigned long   osInfo;
    GT_BUF_INFO     *pFrags;
    GT_U32          status;
    GT_U16          pktSize;
    GT_U16          numFrags;
    char            srcIdx;
    GT_U32          fragIP;
} GT_PKT_INFO;

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
* bspHsuMalloc
*
* DESCRIPTION:
*       Allocate a free area for HSU usage.
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

void *bspHsuMalloc
(
    IN size_t bytes
);

/*******************************************************************************
* bspHsuFree
*
* DESCRIPTION:
*       free a hsu area back to pool.
*
* INPUTS:
*       size_t bytes - number of bytes to allocate
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       MV_OK   - on success
*       MV_FAIL - on error
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS bspHsuFree
(
 IN void *pBuf
);

/*******************************************************************************
* bspWarmRestart
*
* DESCRIPTION:
*       This routine performs warm restart.
*
* INPUTS:
*       none.
*
* OUTPUTS:
*       none.
*
* RETURNS:
*       MV_OK      - on success.
*       MV_FAIL    - otherwise.
*
* COMMENTS:
*       None.
*
*******************************************************************************/

GT_STATUS bspWarmRestart
(
    GT_VOID
);

/*******************************************************************************
* bspInboundSdmaEnable
*
* DESCRIPTION:
*       This routine enables inbound sdma access.
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

GT_STATUS bspInboundSdmaEnable
(
    GT_VOID
);

/*******************************************************************************
* bspInboundSdmaDisable
*
* DESCRIPTION:
*       This routine disables inbound sdma access.
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

GT_STATUS bspInboundSdmaDisable
(
    GT_VOID
);

/*******************************************************************************
* bspInboundSdmaStateGet
*
* DESCRIPTION:
*       This routine gets the state of inbound sdma access.
*
* INPUTS:
*       none.
*
* OUTPUTS:
*       none.
*
* RETURNS:
*       GT_TRUE    - if sdma is enabled.
*       GT_FALSE   - otherwise.
*
* COMMENTS:
*       None.
*
*******************************************************************************/

GT_BOOL bspInboundSdmaStateGet
(
    GT_VOID
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

/******************************************************************************
* bspDoubleRead
*
* DESCRIPTION:
* This routine will read a 64-bit data  from given address
*
* INPUTS:
*     address - address to write to
*
* OUTPUTS:
*       64-bit data.
*
* RETURNS:
*       GT_OK   - on success,
*       GT_FAIL - othersise.
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS bspPciDoubleRead(GT_U32 address, GT_U64 *dataPtr);

/******************************************************************************
* bspDoubleWrite
*
* DESCRIPTION:
* This routine will write a 64-bit data  to given address
*
* INPUTS:
*     address - address to write to
*     word1 - the first half of double word to write
*     word2 - the second half of double word    to write
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
GT_STATUS bspPciDoubleWrite(GT_U32 address, GT_U32 word1, GT_U32 word2);

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

#ifdef MV_LINUX_BSP
/* bspPciGetResourceStart() and bspPciGetResourceLen() are used in
 * Linux kernel module only. The output value is MV_U64 (!= GT_U64)
 */
#if defined(MV_PPC64) || defined(MIPS64)
typedef unsigned long	MV_U64;
#else
typedef unsigned long long	MV_U64;
#endif
/*******************************************************************************
* bspPciGetResourceStart
*
* DESCRIPTION:
*       This routine performs pci_resource_start.
*       In MIPS64 and INTEL64  this function must be used instead of reading the bar
*       directly.
*
* INPUTS:
*       busNo    - PCI bus number.
*       devSel   - the device devSel.
*       funcNo   - function number.
*       barNo    - Bar Number.
*
* OUTPUTS:
*       ResourceStart - the address of the resource.
*
* RETURNS:
*       MV_OK   - on success,
*       MV_FAIL - othersise.
*
* COMMENTS:
*
*******************************************************************************/

GT_STATUS bspPciGetResourceStart
(
    IN  GT_U32  busNo,
    IN  GT_U32  devSel,
    IN  GT_U32  funcNo,
    IN  GT_U32  barNo,
    OUT MV_U64  *resourceStart
);

/*******************************************************************************
* bspPciGetResourceLen
*
* DESCRIPTION:
*       This routine performs pci_resource_len.
*       In MIPS64 and INTEL64 this function must be used instead of reading the bar
*       directly.
*
* INPUTS:
*       busNo    - PCI bus number.
*       devSel   - the device devSel.
*       funcNo   - function number.
*       barNo    - Bar Number.
*
* OUTPUTS:
*       ResourceLen - the address of the resource.
*
* RETURNS:
*       MV_OK   - on success,
*       MV_FAIL - othersise.
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS bspPciGetResourceLen
(
    IN  GT_U32  busNo,
    IN  GT_U32  devSel,
    IN  GT_U32  funcNo,
    IN  GT_U32  barNo,
    OUT MV_U64  *resourceLen
);
#endif /* MV_LINUX_BSP */

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

/*******************************************************************************
* bspEthMuxSet
*
* DESCRIPTION: Set bitmap of PP ports. Set direction of packets:
*              cpss, Linux, or Raw.
*
* INPUTS:
*       MV_U32 portNum, PortType
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       MV_OK if successful, or
*       MV_FAIL otherwise.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS bspEthMuxSet(
 IN GT_U32 portNum,
 IN GT_U32 portType
);

/*******************************************************************************
 * bspEthMuxGet
 *
 * DESCRIPTION: Get the mux mosde of the port
 *
 * INPUTS:
 *       portNum   - The port number for the action
 *       portType  - Address to store portType information
 *
 * OUTPUTS:
 *       None.
 *
 * RETURNS:
 *       GT_OK always
 *
 * COMMENTS:
 *       None.
 *
 *******************************************************************************/
GT_STATUS bspEthMuxGet
(
 IN GT_U32 portNum,
 OUT GT_U32 *portTypeP
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
* bspEthInit
*
* DESCRIPTION: Init the ethernet HW and HAL
*
* INPUTS:
*       port   - eth port number
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       MV_OK if successful, or
*       MV_FAIL otherwise.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_VOID bspEthInit
(
    GT_U8 port
);

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
* bspEthPortTxModeSet
*
* DESCRIPTION: Set the ethernet port tx mode
*
* INPUTS:
*       if txMode == bspEthTxMode_asynch_E -- don't wait for TX done - free packet when interrupt received
*       if txMode == bspEthTxMode_synch_E  -- wait to TX done and free packet immediately
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       MV_OK if successful
*       MV_NOT_SUPPORTED if input is wrong
*       MV_FAIL if bspTxModeSetOn is zero
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS bspEthPortTxModeSet
(
    bspEthTxMode_ENT    txMode
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
* bspEthPortTxQueue
*
* DESCRIPTION:
*       This function is called after a TxEnd event has been received, it passes
*       the needed information to the Tapi part.
*
* INPUTS:
*       segmentList     - A list of pointers to the packets segments.
*       segmentLen      - A list of segment length.
*       numOfSegments   - The number of segment in segment list.
*       txQueue         - The TX queue.
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
GT_STATUS bspEthPortTxQueue
(
    IN GT_U8_PTR        segmentList[],
    IN GT_U32           segmentLen[],
    IN GT_U32           numOfSegments,
    IN GT_U32           txQueue
);

/*******************************************************************************
* bspEthPortRx
*
* DESCRIPTION:
*       This function is called when a packet has received.
*
* INPUTS:
*       rxQueue         - RX Queue.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       MV_OK if successful, or
*       MV_FAIL otherwise.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_PKT_INFO* bspEthPortRx
(
    IN GT_U32           rxQueue
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
    IN GT_U8_PTR    segmentList[],
    IN GT_U32       numOfSegments,
    IN GT_U32       queueNum
);

/*******************************************************************************
* bspEthCpuCodeToQueue
*
* DESCRIPTION:
*       Binds DSA CPU code to RX queue.
*
* INPUTS:
*       dsaCpuCode - DSA CPU code
*       rxQueue    -  rx queue
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK               - on success
*       GT_FAIL             - on fail
*       GT_NOT_SUPPORTED    - the API is not supported
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS bspEthCpuCodeToQueue
(
    IN GT_U32  dsaCpuCode,
    IN GT_U8   rxQueue
);

/*******************************************************************************
* bspEthPrePendTwoBytesHeaderSet
*
* DESCRIPTION:
*       Enables/Disable pre-pending a two-byte header to all packets arriving
*       to the CPU.
*
* INPUTS:
*        enable    - GT_TRUE  - Two-byte header is pre-pended to packets
*                               arriving to the CPU.
*                    GT_FALSE - Two-byte header is not pre-pended to packets
*                               arriving to the CPU.
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
GT_STATUS bspEthPrePendTwoBytesHeaderSet
(
    IN GT_BOOL enable
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
#ifndef bspIntEnable

/* Some functions are defined as macros for performance boost.
   See pssBspApisInline.h in Linux.  */

GT_STATUS bspIntEnable
(
    IN GT_U32   intMask
);
#endif

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
#ifndef bspIntDisable
GT_STATUS bspIntDisable
(
    IN GT_U32   intMask
);
#endif

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
#ifndef bspVirt2Phys
GT_U32 bspVirt2Phys
(
    IN GT_U32 vAddr
);
#endif

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
#ifndef bspPhys2Virt
GT_U32 bspPhys2Virt
(
    IN GT_U32 pAddr
);
#endif

/*** Dragonite ***/
/*******************************************************************************
* bspDragoniteSWDownload
*
* DESCRIPTION:
*       Download new version of Dragonite firmware to Dragonite MCU
*
* INPUTS:
*       sourcePtr      - Pointer to memory where new version of Dragonite firmware resides.
*       size           - size of firmware to download to ITCM.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK   - on success,
*       GT_FAIL - otherwise.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS bspDragoniteSWDownload
(
    IN  const GT_VOID *sourcePtr,
    IN  GT_U32         size
);

/*******************************************************************************
* bspDragoniteEnableSet
*
* DESCRIPTION: Enable/Disable Dragonite module
*
* INPUTS:
*       enable – GT_TRUE  – Dragonite starts work with parameters set by application
*                GT_FALSE – Dragonite stops function
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK
*
* COMMENTS:
*       call after SW download
*
*******************************************************************************/
GT_STATUS bspDragoniteEnableSet
(
    IN  GT_BOOL enable
);


/*******************************************************************************
* bspDragoniteInit
*
* DESCRIPTION: Initialize Dragonite module
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
*       Application will call this before firmware download
*
*******************************************************************************/
GT_STATUS bspDragoniteInit
(
    GT_VOID
);

/*******************************************************************************
* bspDragoniteSharedMemWrite
*
* DESCRIPTION:
*       Write a given buffer to the given offset in shared memory of Dragonite
*        microcontroller.
*
* INPUTS:
*       offset  - Offset from beginning of shared memory
*       buffer  - The buffer to be written.
*       length  - Length of buffer in bytes.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK       - on success,
*       GT_BAD_PARAM - otherwise.
*
* COMMENTS:
*       Only DTCM is reachable
*
*******************************************************************************/
GT_STATUS bspDragoniteSharedMemWrite
(
    IN  GT_U32         offset,
    IN  const GT_VOID *buffer,
    IN  GT_U32         length
);

/*******************************************************************************
* bspDragoniteSharedMemRead
*
* DESCRIPTION:
*       Read a memory block from a given offset in shared memory of Dragonite
*        microcontroller.
*
* INPUTS:
*       offset  - Offset from beginning of shared memory
*       length  - Length of the memory block to read (in bytes).
*
* OUTPUTS:
*       buffer  - The read data.
*
* RETURNS:
*       GT_OK   - on success,
*       GT_BAD_PARAM - otherwise.
*
* COMMENTS:
*       Only DTCM is reachanble
*
*******************************************************************************/
GT_STATUS bspDragoniteSharedMemRead
(
    IN  GT_U32   offset,
    OUT GT_VOID *buffer,
    IN  GT_U32   length
);

/*******************************************************************************
* bspDragoniteSharedMemoryBaseAddrGet
*
* DESCRIPTION:
*       Get start address of DTCM
*
* INPUTS:
*       None.
*
* OUTPUTS:
*       dtcmPtr - Pointer to beginning of DTCM where communication structures
*                 must be placed
*
* RETURNS:
*       GT_OK
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS bspDragoniteSharedMemoryBaseAddrGet
(
    OUT GT_U32 *dtcmPtr
);

/*******************************************************************************
* bspDragoniteGetIntVec
*
* DESCRIPTION:
*       This routine return the Dragonite interrupt vector.
*
* INPUTS:
*       None
*
* OUTPUTS:
*       intVec - Dragonite interrupt vector.
*
* RETURNS:
*       GT_OK      - on success.
*       GT_FAIL    - otherwise.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS bspDragoniteGetIntVec
(
    OUT GT_U32 *intVec
);

/*******************************************************************************
* bspDragoniteFwCrcCheck
*
* DESCRIPTION:
*       This routine executes Dragonite firmware checksum test
*
* INPUTS:
*       None
*
* OUTPUTS:
*       None
*
* RETURNS:
*       MV_OK      - on success.
*       MV_FAIL    - otherwise.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS bspDragoniteFwCrcCheck
(
    GT_VOID
);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __pssBspApisH */
