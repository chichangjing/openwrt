/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* cpssDxChNetIfMii.h
*
* DESCRIPTION:
*       Include DxCh network interface API functions for
*        PPs connected to CPU by MII/RGMII Ethernet port but not SDMA
*
* FILE REVISION NUMBER:
*       $Revision: 12 $
*******************************************************************************/
#ifndef __cpssDxChNetIfMiih
#define __cpssDxChNetIfMiih

#ifdef __cplusplus
extern "C" {
#endif

#include <cpss/generic/networkIf/cpssGenNetIfTypes.h>
#include <cpss/dxCh/dxChxGen/networkIf/cpssDxChNetIfTypes.h>
#include <cpss/generic/cpssHwInit/cpssHwInit.h>

/*
 * typedef: structure CPSS_DXCH_NETIF_MII_INIT_STC
 *
 * Description: structure of network interface by MII/RGMII Ethernet port
 *              initialization parameters.
 * Fields:
 *    numOfTxDesc - number of requested TX descriptors. Decriptors dispersed
 *                  evenly among all TX queues.
 *    txInternalBufBlockPtr  - (Pointer to) a block of host memory to be used
 *                       for internal TX buffers.
 *    txInternalBufBlockSize - The raw size in bytes of
 *                       txInternalBufBlock memory.
 *                       Recommended size of txInternalBufBlock is
 *                       (numOfTxDesc / 2) * CPSS_GEN_NETIF_MII_TX_INTERNAL_BUFF_SIZE_CNS).
 *    bufferPercentage   - A table (entry per queue) that describes the buffer
 *                         dispersal among all Rx queues. (APPLICABLE RANGES: 0..100)
 *                         NOTE: The accumulation of all queues should not
 *                                  be more than 100%.
 *    rxBufSize       - The size of the Rx data buffer.
 *    headerOffset    - packet header offset size
 *    rxBufBlockPtr   - (Pointer to) a block of memory to be used for
 *                        allocating Rx packet data buffers.
 *    rxBufBlockSize  - The raw size in byte of rxDataBufBlock.
 *
*/
typedef struct{
    GT_U32    numOfTxDesc;
    GT_U8    *txInternalBufBlockPtr;
    GT_U32    txInternalBufBlockSize;
    GT_U32    bufferPercentage[CPSS_MAX_RX_QUEUE_CNS];
    GT_U32    rxBufSize;
    GT_U32    headerOffset;
    GT_U8    *rxBufBlockPtr;
    GT_U32    rxBufBlockSize;
}CPSS_DXCH_NETIF_MII_INIT_STC;

/*******************************************************************************
* cpssDxChNetIfMiiTxPacketSend
*
* DESCRIPTION:
*       This function receives packet buffers & parameters from Application,
*       prepares them for the transmit operation, and
*       transmits a packet through the ethernet port.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum              - Device number.
*       pcktParamsPtr       - The internal packet params to be set into the packet
*                             descriptors.
*       packetBuffsArrPtr   - (pointer to)The packet data buffers list.
*       buffLenArr          - A list of the buffers len in packetBuffsArrPtr.
*       numOfBufs           - Length of packetBuffsArrPtr.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK on success, or
*       GT_NO_RESOURCE if there is not enough desc. for enqueuing the packet.
*       GT_FAIL otherwise.
*
* COMMENTS:
*       Support both regular and extended DSA Tag.
*
*******************************************************************************/
GT_STATUS cpssDxChNetIfMiiTxPacketSend
(
    IN GT_U8                        devNum,
    IN CPSS_DXCH_NET_TX_PARAMS_STC *pcktParamsPtr,
    IN GT_U8                       *packetBuffsArrPtr[],
    IN GT_U32                       buffLenArr[],
    IN GT_U32                       numOfBufs
);

/*******************************************************************************
* cpssDxChNetIfMiiSyncTxPacketSend
*
* DESCRIPTION:
*       This function receives packet buffers & parameters from Application,
*       prepares them for the transmit operation, and
*       transmits a packet through the ethernet port.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum              - Device number.
*       pcktParamsPtr       - The internal packet params to be set into the packet
*                             descriptors.
*       packetBuffsArrPtr   - (pointer to)The packet data buffers list.
*       buffLenArr          - A list of the buffers len in packetBuffsArrPtr.
*       numOfBufs           - Length of packetBuffsArrPtr.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK on success, or
*       GT_NO_RESOURCE if there is not enough desc. for enqueuing the packet.
*       GT_FAIL otherwise.
*
* COMMENTS:
*       Support both regular and extended DSA Tag.
*
*******************************************************************************/
GT_STATUS cpssDxChNetIfMiiSyncTxPacketSend
(
    IN GT_U8                        devNum,
    IN CPSS_DXCH_NET_TX_PARAMS_STC *pcktParamsPtr,
    IN GT_U8                       *packetBuffsArrPtr[],
    IN GT_U32                       buffLenArr[],
    IN GT_U32                       numOfBufs
);

/*******************************************************************************
* cpssDxChNetIfMiiTxBufferQueueGet
*
* DESCRIPTION:
*       This routine returns the caller the TxEnd parameters for a transmitted
*       packet.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       hndl        - Handle got from cpssEventBind.
*
* OUTPUTS:
*       devPtr      - The device the packet was transmitted from.
*       cookiePtr   - The user cookie handed on the transmit request.
*       queuePtr    - The queue from which this packet was transmitted
*       statusPtr   - GT_OK if packet transmission was successful, GT_FAIL on
*                     packet reject.
*
* RETURNS:
*       GT_OK on success, or
*       GT_FAIL otherwise.
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChNetIfMiiTxBufferQueueGet
(
    IN GT_UINTPTR            hndl,
    OUT GT_U8               *devPtr,
    OUT GT_PTR              *cookiePtr,
    OUT GT_U8               *queuePtr,
    OUT GT_STATUS           *statusPtr
);

/*******************************************************************************
* cpssDxChNetIfMiiRxPacketGet
*
* DESCRIPTION:
*       This function returns packets from PP destined to the CPU port.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum      - The device number throw which packet was received.
*       queueIdx    - The queue from which this packet was received (APPLICABLE RANGES: 0..7).
*       numOfBuffPtr- Num of buffs in packetBuffsArrPtr.
*
* OUTPUTS:
*       numOfBuffPtr        - Num of used buffs in packetBuffsArrPtr.
*       packetBuffsArrPtr   - (pointer to) The received packet buffers list.
*       buffLenArr          - List of buffer lengths for packetBuffsArrPtr.
*       rxParamsPtr         - (Pointer to)information parameters of received packets
*
* RETURNS:
*       GT_NO_MORE  - no more packets on the device/queue
*       GT_OK       - packet got with no error.
*       GT_FAIL     - failed to get the packet
*       GT_HW_ERROR - on hardware error
*       GT_BAD_PTR  - one of the parameters is NULL pointer
*       GT_BAD_PARAM - wrong devNum,queueIdx.
*       GT_DSA_PARSING_ERROR - DSA tag parsing error.
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChNetIfMiiRxPacketGet
(
    IN GT_U8                            devNum,
    IN GT_U8                            queueIdx,
    INOUT GT_U32                        *numOfBuffPtr,
    OUT GT_U8                           *packetBuffsArrPtr[],
    OUT GT_U32                          buffLenArr[],
    OUT CPSS_DXCH_NET_RX_PARAMS_STC     *rxParamsPtr
);

/*******************************************************************************
* cpssDxChNetIfMiiRxBufFree
*
* DESCRIPTION:
*       Frees a list of buffers, that where previously passed to the upper layer
*       in an Rx event.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum      - The device number throw which these buffers where
*                     received.
*       rxQueue     - The Rx queue number throw which these buffers where
*                     received (APPLICABLE RANGES: 0..7).
*       rxBuffList  - List of Rx buffers to be freed.
*       buffListLen - Length of rxBufList.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK on success, or
*       GT_FAIL otherwise.
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChNetIfMiiRxBufFree
(
    IN GT_U8    devNum,
    IN GT_U8    rxQueue,
    IN GT_U8    *rxBuffList[],
    IN GT_U32   buffListLen
);

/*******************************************************************************
* cpssDxChNetIfMiiInit
*
* DESCRIPTION:
*       Initialize the network interface MII/RGMII Ethernet port.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum       - Device number.
*       miiInitPtr   - (Pointer to) initialization parameters.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on fail
*       GT_BAD_PTR               - on NULL pointer.
*       GT_BAD_PARAM             - on wrong device number
*       GT_OUT_OF_CPU_MEM        - on memory allocation failure
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       1. Only single device can be connected to CPU by ethernet port, therefor
*          this API must be called only once.
*       2. The application must call only one of the following APIs per device:
*          cpssDxChNetIfMiiInit - for MII/RGMII Ethernet port networkIf initialization.
*          cpssDxChNetIfInit - for SDMA networkIf initialization.
*          In case more than one of the mentioned above API is called
*          GT_FAIL will be return.
*
*******************************************************************************/
GT_STATUS cpssDxChNetIfMiiInit
(
    IN GT_U8                        devNum,
    IN CPSS_DXCH_NETIF_MII_INIT_STC *miiInitPtr
);


#ifdef __cplusplus
}
#endif

#endif  /* __cpssDxChNetIfh */
