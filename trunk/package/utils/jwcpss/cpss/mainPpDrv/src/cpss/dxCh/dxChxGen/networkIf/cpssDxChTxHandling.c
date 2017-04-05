/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* cpssDxChTxHandling.c
*
* DESCRIPTION:
*       This file implements all needed functions for sending packets of upper
*       layer to the cpu port of the DXCH device (PP's Tx Queues).
*
* FILE REVISION NUMBER:
*       $Revision: 46 $
*
*******************************************************************************/
#define CPSS_LOG_IN_MODULE_ENABLE
#include <cpss/dxCh/dxChxGen/networkIf/private/prvCpssDxChNetworkIfLog.h>
#include <cpss/dxCh/dxChxGen/config/private/prvCpssDxChInfo.h>
#include <cpss/dxCh/dxChxGen/networkIf/cpssDxChNetIf.h>
#include <cpss/dxCh/dxChxGen/cos/private/prvCpssDxChCoS.h>
#include <cpss/driver/interrupts/cpssDrvComIntEvReqQueues.h>

#include <cpss/generic/networkIf/cpssGenNetIfMii.h>
#include <cpss/generic/networkIf/private/prvCpssGenNetworkIfMii.h>

#include <cpssCommon/private/prvCpssMath.h>

#define _25MHZ_CLK_CNS 25000000
#define _1MHZ_CNS       1000000
#define _1GHZ_CNS    1000000000

/*******************************************************************************
* External usage environment parameters
*******************************************************************************/

/*******************************************************************************
* Internal usage environment parameters
*******************************************************************************/

/*******************************************************************************
* prvCpssDxChNetIfSdmaTxGenerator3PacketIntervalsWait
*
* DESCRIPTION:
*       This function imposes a wait of 3 packet intervals.
*
* APPLICABLE DEVICES:
*        xCat3; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum          - Device number.
*       portGroupId     - Port Group ID.
*       txQueue         - The Tx SDMA queue.
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK           - on success.
*       GT_FAIL         - on hardware error
*
* COMMENTS:
*   Waiting (sleeping) for 3 packet generator intervals before updating
*   descriptor & buffer is required because they might be in use. One (1)
*   interval might be not enough due to the descriptor prefetch mechanism.
*
*******************************************************************************/
static GT_STATUS prvCpssDxChNetIfSdmaTxGenerator3PacketIntervalsWait
(
    IN  GT_U8                           devNum,
    IN  GT_U32                          portGroupId,
    IN  GT_U8                           txQueue
)
{
    GT_STATUS   rc;
    GT_U32      regValue;   /* register value */
    GT_U32      sleepTime;  /* sleep time in ms */

    rc = prvCpssHwPpPortGroupReadRegBitMask(
            devNum,
            portGroupId,
            PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->sdmaRegs.
                                    txSdmaPacketGeneratorConfigQueue[txQueue],
            0xFFFFFFFE,
            &regValue);

    if( GT_OK != rc )
    {
        return rc;
    }

    if( (regValue & 0x2) == 0 )
    {
        /* Tx without packet interval, use 1ms sleep */
        sleepTime = 1;
    }
    else
    {
        /* calculate sleep time in ms and add 1 due to rounding */
        if( (0xFFFFFFFF / ((regValue >> 2) & 0x3FFFFFF)) <
                                           (GT_U32)(_1GHZ_CNS/_25MHZ_CLK_CNS) )
        {
            sleepTime = (((regValue >> 2) & 0x3FFFFFF)/_1MHZ_CNS) *
                                           (_1GHZ_CNS/_25MHZ_CLK_CNS)/_1MHZ_CNS;
        }
        else
        {
            sleepTime = (((regValue >> 2) & 0x3FFFFFF) *
                                          (_1GHZ_CNS/_25MHZ_CLK_CNS))/_1MHZ_CNS;
        }

        sleepTime += 1;
        sleepTime *= 3;
    }

    cpssOsTimerWkAfter(sleepTime);

    return GT_OK;
}

/*******************************************************************************
* prvCpssDxChNetIfSdmaTxGeneratorRateCalc
*
* DESCRIPTION:
*       This function calculate the required number of clock ticks to configure
*       in HW for the packet interval.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       rateMode        - transmission rate mode.
*       rateValue       - packets per second for rateMode ==
*                      CPSS_DXCH_NET_TX_GENERATOR_RATE_MODE_GLOBAL_THROUGHPUT_E,
*                         nanoseconds for rateMode ==
*                      CPSS_DXCH_NET_TX_GENERATOR_RATE_MODE_PER_FLOW_INTERVAL_E.
*                 Not relevant to CPSS_DXCH_NET_TX_GENERATOR_RATE_MODE_NO_GAP_E.
*       packetsInChain  - number of packets in the generator chain.
*
* OUTPUTS:
*       clkTicksPtr         - (pointer to) number of clock ticks to configure
*                             in HW for packet gap.
*       actualRateValuePtr  - (pointer to) the actual configured rate value.
*                             Same units as rateValue.
*
* RETURNS:
*       GT_OK           - on success.
*       GT_BAD_PARAM    - on bad rate mode or value.
*       GT_BAD_VALUE    - due to HW configuration limitation.
*
* COMMENTS:
*       Rate can be set while Tx SDMA is enabled.
*
*******************************************************************************/
static GT_STATUS prvCpssDxChNetIfSdmaTxGeneratorRateCalc
(
    IN  CPSS_DXCH_NET_TX_GENERATOR_RATE_MODE_ENT    rateMode,
    IN  GT_U64                                      rateValue,
    IN  GT_U32                                      packetsInChain,
    OUT GT_U32                                      *clkTicksPtr,
    OUT GT_U64                                      *actualRateValuePtr
)
{
    GT_U32  localPacketInChain = packetsInChain ;
    GT_U64  actualRateValue;
    GT_U64  tempVal;

    switch(rateMode)
    {
        case CPSS_DXCH_NET_TX_GENERATOR_RATE_MODE_GLOBAL_THROUGHPUT_E:
            /* cannot differentiate if required gap betwwen packets is less */
            /* then 1 clk tick. */
            if( rateValue.l[1] !=0 || rateValue.l[0] > _25MHZ_CLK_CNS )
            {
                CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
            }

            /* division by zero */
            if( rateValue.l[0] == 0 )
            {
                CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
            }

            *clkTicksPtr = _25MHZ_CLK_CNS/rateValue.l[0];
            actualRateValue.l[0] = _25MHZ_CLK_CNS/(*clkTicksPtr);
            actualRateValue.l[1] = 0;
            *actualRateValuePtr = actualRateValue;
            break;

        case CPSS_DXCH_NET_TX_GENERATOR_RATE_MODE_PER_FLOW_INTERVAL_E:
            /* if there are no packets yet in the queue assume 1 packet to */
            /* intialize the configuration */
            if( 0 == localPacketInChain )
            {
                localPacketInChain = 1;
            }

            /* division by 1,000,000,000 since rate value in ns */
            tempVal = prvCpssMathMul64(
                        (BIT_30-1),
                        (localPacketInChain*(_1GHZ_CNS/_25MHZ_CLK_CNS)));
            if( (tempVal.l[1] < rateValue.l[1]) ||
                ((tempVal.l[1] == rateValue.l[1]) &&
                 (tempVal.l[0]  < rateValue.l[0])) )
            {
                CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_VALUE, LOG_ERROR_NO_MSG);
            }

            tempVal = prvCpssMathDiv64By16(rateValue,
                                           (_1GHZ_CNS/_25MHZ_CLK_CNS));
            tempVal = prvCpssMathDiv64By16(tempVal,
                                           (GT_U16)localPacketInChain);

            *clkTicksPtr = tempVal.l[0];

            if(*clkTicksPtr == 0)
            {
                CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_VALUE, LOG_ERROR_NO_MSG);
            }

            tempVal = prvCpssMathMul64(
                        (*clkTicksPtr),
                        (localPacketInChain*(_1GHZ_CNS/_25MHZ_CLK_CNS)));

            *actualRateValuePtr = tempVal;
            break;

        case CPSS_DXCH_NET_TX_GENERATOR_RATE_MODE_NO_GAP_E:
            break;

        default:
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    return GT_OK;
}

/*******************************************************************************
* prvCpssDxChNetIfSdmaTxGeneratorHwRateSet
*
* DESCRIPTION:
*       This function sets Tx SDMA Generator HW related parameters for packet
*       interval.
*
* APPLICABLE DEVICES:
*        xCat3; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum          - Device number.
*       portGroupId     - Port Group ID.
*       txQueue         - The Tx SDMA queue.
*       rateMode        - transmission rate mode.
*       clkTicks        - clock ticks to configure into HW (actual value).
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK           - on success.
*       GT_BAD_PARAM    - on bad rate mode.
*       GT_FAIL         - on hardware error
*
* COMMENTS:
*       Rate can be set while Tx SDMA is enabled.
*
*******************************************************************************/
static GT_STATUS prvCpssDxChNetIfSdmaTxGeneratorHwRateSet
(
    IN  GT_U8                                       devNum,
    IN  GT_U32                                      portGroupId,
    IN  GT_U8                                       txQueue,
    IN  CPSS_DXCH_NET_TX_GENERATOR_RATE_MODE_ENT    rateMode,
    IN  GT_U32                                      clkTicks
)
{
    GT_STATUS   rc;

    GT_U32      regValue;         /* register value */

    /* Set packet interval enable - bit 1 & packet interval value - bits[31:2]*/
    switch(rateMode)
    {
        case CPSS_DXCH_NET_TX_GENERATOR_RATE_MODE_GLOBAL_THROUGHPUT_E:
        case CPSS_DXCH_NET_TX_GENERATOR_RATE_MODE_PER_FLOW_INTERVAL_E:
            regValue = (1 << 1) + (clkTicks << 2);
            break;

        case CPSS_DXCH_NET_TX_GENERATOR_RATE_MODE_NO_GAP_E:
            regValue = 0x0;
            break;

        default:
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    rc = prvCpssHwPpPortGroupWriteRegBitMask(
            devNum,
            portGroupId,
            PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->sdmaRegs.
                                    txSdmaPacketGeneratorConfigQueue[txQueue],
            0xFFFFFFFE,
            regValue);

    return rc;
}

/*******************************************************************************
* prvCpssDxChNetIfSdmaTxDsaLengthAndDataOffsetGet
*
* DESCRIPTION:
*       This function calculated DSA tag bytes length and the packet data offset
*       based on tagging existence.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1_Diamond.
*
* INPUTS:
*       packetParamsPtr - (pointer to) The internal packet params to be set into
*                       the packet descriptors.
*
* OUTPUTS:
*       dsaTagNumOfBytesPtr - (pointer to) DSA tag length in bytes.
*       dataOffsetPtr       - (pointer to) packet data offset.
*
* RETURNS:
*       GT_OK          - on success, or
*       GT_BAD_PARAM   - on wrong DSA tag type
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS prvCpssDxChNetIfSdmaTxDsaLengthAndDataOffsetGet
(
    IN  CPSS_DXCH_NET_TX_PARAMS_STC *packetParamsPtr,
    OUT GT_U32                      *dsaTagNumOfBytesPtr,
    OUT GT_U32                      *dataOffsetPtr
)
{
    switch(packetParamsPtr->dsaParam.commonParams.dsaTagType)
    {
        case CPSS_DXCH_NET_DSA_1_WORD_TYPE_ENT:
            *dsaTagNumOfBytesPtr = 4;
            break;
        case CPSS_DXCH_NET_DSA_2_WORD_TYPE_ENT:
            *dsaTagNumOfBytesPtr = 8;
            break;
        case CPSS_DXCH_NET_DSA_4_WORD_TYPE_ENT:
            *dsaTagNumOfBytesPtr = 16;
            break;
        default:
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    /* Get the Data after Source MAC Address and Destination MAC Address */
    if( packetParamsPtr->packetIsTagged == GT_TRUE )
    {
        *dataOffsetPtr = PRV_CPSS_DXCH_ETHPRT_HDR_MAC_SIZE_CNS + /*12*/
                         PRV_CPSS_DXCH_ETHPRT_TAGGED_PCKT_FLAG_LEN_CNS;/* 4 */
    }
    else
    {
        *dataOffsetPtr = PRV_CPSS_DXCH_ETHPRT_HDR_MAC_SIZE_CNS; /*12*/
    }

    return GT_OK;
}

/*******************************************************************************
* prvCpssDxChNetIfSdmaTxGeneratorPacketToBufferSet
*
* DESCRIPTION:
*       This function create the requested packet in the generator buffer,
*       updating the descriptor and adding the DSA tag.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1_Diamond.
*
* INPUTS:
*       devNum          - Device number.
*       packetParamsPtr   - (pointer to) The internal packet params to be set
*                         into the packet descriptors.
*       packetDataPtr     - (pointer to) packet data.
*       packetDataLength  - packet data length.
*       txDesc          - (pointer to) The descriptor used for that packet.
*       bufferSize      - the buffer size for the created packet.

*
* OUTPUTS:
*       none.
*
* RETURNS:
*       GT_OK          - on success
*       GT_BAD_PARAM   - on wrong DSA tag type the data buffer is longer than
*                        allowed.
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS prvCpssDxChNetIfSdmaTxGeneratorPacketToBufferSet
(
    IN  GT_U8                       devNum,
    IN  CPSS_DXCH_NET_TX_PARAMS_STC *packetParamsPtr,
    IN  GT_U8                       *packetDataPtr,
    IN  GT_U32                      packetDataLength,
    IN  PRV_CPSS_TX_DESC_STC        *txDesc,
    IN  GT_U32                      bufferSize
)
{
    GT_STATUS   rc;

    GT_U32      dataOffset;         /* packet data offset due to tagging */
    GT_U32      dsaTagNumOfBytes;   /* DSA tag length */

    /* Calculate DSA tag length and packet data offset */
    rc = prvCpssDxChNetIfSdmaTxDsaLengthAndDataOffsetGet(packetParamsPtr,
                                                         &dsaTagNumOfBytes,
                                                         &dataOffset);
    if( GT_OK != rc )
    {
        return rc;
    }

    /* Check packet length does not excceed buffer length */
    if( bufferSize < (packetDataLength + dsaTagNumOfBytes +
                      PRV_CPSS_DXCH_ETHPRT_HDR_MAC_SIZE_CNS - dataOffset) )
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    /* Fill new packet buffer */
    /* Copy Ethernet MACs header */
    cpssOsMemCpy( ((GT_U8*)txDesc + sizeof(PRV_CPSS_TX_DESC_STC)),
                  packetDataPtr,
                  PRV_CPSS_DXCH_ETHPRT_HDR_MAC_SIZE_CNS );

    /* Build & Add DSA tag */
    rc = cpssDxChNetIfDsaTagBuild( devNum,
                                   &packetParamsPtr->dsaParam,
                      ((GT_U8*)txDesc + sizeof(PRV_CPSS_TX_DESC_STC) +
                                   PRV_CPSS_DXCH_ETHPRT_HDR_MAC_SIZE_CNS) );
    if(rc != GT_OK)
    {
        return rc;
    }

    /* Copy rest of packet data */
    cpssOsMemCpy( ((GT_U8*)txDesc + sizeof(PRV_CPSS_TX_DESC_STC) +
                      PRV_CPSS_DXCH_ETHPRT_HDR_MAC_SIZE_CNS + dsaTagNumOfBytes),
                  packetDataPtr + dataOffset,
                  packetDataLength - dataOffset);

    /* Fill new descriptor */
    TX_DESC_RESET_MAC(txDesc);

    /* Set first and last bit */
    TX_DESC_SET_FIRST_BIT_MAC(txDesc,1);
    TX_DESC_SET_LAST_BIT_MAC(txDesc,1);

    /* Set packet byte count */
    TX_DESC_SET_BYTE_CNT_MAC(txDesc,(packetDataLength + dsaTagNumOfBytes +
                          PRV_CPSS_DXCH_ETHPRT_HDR_MAC_SIZE_CNS - dataOffset));

    /* Set ownership bit - owned by DMA */
    TX_DESC_SET_OWN_BIT_MAC(txDesc,TX_DESC_DMA_OWN);

    txDesc->word1 = CPSS_32BIT_LE(txDesc->word1);
    txDesc->word2 = CPSS_32BIT_LE(txDesc->word2);

    return rc;
}

/*******************************************************************************
* dxChNetIfSdmaTxPacketSend
*
* DESCRIPTION:
*       This function receives packet buffers & parameters from the different
*       core Tx functions. Prepares them for the transmit operation, and
*       enqueues the prepared descriptors to the PP's tx queues. -- SDMA relate
*       function activates Tx SDMA , function doesn't wait for event of
*       "Tx buffer queue" from PP
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1_Diamond.
*
* INPUTS:
*       devNum       - Device number.
*       pcktParamsPtr- The internal packet params to be set into the packet
*                      descriptors.
*       buffList     - The packet data buffers list.
*       buffLenList  - A list of the buffers len in buffList.
*       numOfBufsPtr - Length of buffList.
*       synchronicSend - sent the packets synchronic (not return until the
*                        buffers can be free)
*                       GT_TRUE - send synchronic
*                       GT_FALSE - send asynchronous (not wait for buffers to be
*                                  free)
*        txDescListPtr - The Tx desc. list control structure
*
* OUTPUTS:
*        txDescListPtr - The Tx desc. list control structure
*        txDescListUpdatedPtr - (pointer to) a parameter indicating if the
*                               txDescList was updated and data structure need
*                               to be restored
*
* RETURNS:
*       GT_OK          - on success, or
*       GT_NO_RESOURCE - if there is not enough free elements in the fifo
*                        associated with the Event Request Handle.
*       GT_EMPTY       - if there are not enough descriptors to do the sending.
*       GT_ABORTED     - if command aborted (during shutDown operation)
*       GT_HW_ERROR    - when  synchronicSend = GT_TRUE and after transmission
*                        the last descriptor own bit wasn't changed for long time.
*       GT_BAD_PARAM   - the data buffer is longer than allowed.
*                        buffer data can occupied up to the maximum
*                        number of descriptors defined or queue assigned for
*                        Packet Generator, unavailable for regular transmission.
*       GT_BAD_PTR  - one of the parameters is NULL pointer
*       GT_FAIL otherwise.
*
* COMMENTS:
*           Note: Each buffer should be at least 8 bytes long.
*           first buffer must be at least 24 bytes for tagged packet ,
*           20 for untagged packet
*
*           support both :
*           ===== regular DSA tag  ====
*           ===== extended DSA tag ====
*
*******************************************************************************/
static GT_STATUS dxChNetIfSdmaTxPacketSend
(
    IN GT_U8                             devNum,
    IN CPSS_DXCH_NET_TX_PARAMS_STC      *pcktParamsPtr,
    IN GT_U8                            *buffList[],
    IN GT_U32                            buffLenList[],
    IN GT_U32                           *numOfBufsPtr,
    IN GT_BOOL                           synchronicSend,
    INOUT PRV_CPSS_TX_DESC_LIST_STC     *txDescListPtr,
    OUT GT_BOOL                         *txDescListUpdatedPtr
)
{
    GT_STATUS rc;
    GT_U8 txQueue;              /* The tx queue to send this packet to. */
    PRV_CPSS_TX_DESC_STC *firstDesc;  /* The first descriptor of the          */
                                /* currently transmitted packet.        */
    PRV_CPSS_SW_TX_DESC_STC  *currSwDesc;  /* The current handled descriptor of*/
                                /* the currently transmitted packet.    */
    PRV_CPSS_TX_DESC_STC  tmpTxDesc;  /* Temporary Tx descriptor used for     */
                                /* preparing the real descriptor data.  */
    PRV_CPSS_TX_DESC_STC  tmpFirstTxDesc; /* Temporary Tx descriptor used for */
                                /* preparing the real first descriptor  */
                                /* data.                                */
    GT_U32      descWord1;          /* The first word of the Tx descriptor. */
    GT_UINTPTR  tmpBuffPtr;         /* Holds the real buffer pointer.       */
    GT_U32      txCmdMask;          /* Mask for Tx command register.        */
    GT_U32      i;
    GT_U8       numOfAddedBuffs;
    GT_U32      dataOffset;
    GT_U32      dsaTagNumOfBytes;/* extended DSA tag size */
    GT_U32      numOfBufs;
    GT_32       key;
    GT_U32      portGroupId;/* port group Id for multi-port-groups device support */
    GT_U32      packetLen = 0;  /* packet length including the DSA tag, */
                                /* not including padding if needed */
    GT_U32      byteBlockMask;  /* mask used to calculate if padding needed due to */
                                /* troublesome packet length */
    PRV_CPSS_DXCH_MODULE_CONFIG_STC *moduleCfgPtr;

    /* must be first line before any chance that function may return
      indicate that the descriptors not changed yet */
    *txDescListUpdatedPtr = GT_FALSE;

    /* we will use the 'First active port group' , to represent the whole device.
       that way we allow application to give SDMA memory to single port group instead
       of split it between all active port groups
    */
    portGroupId = PRV_CPSS_NETIF_SDMA_PORT_GROUP_ID_MAC(devNum);

    /* Set to 0, in the function body will be updated to real number of used buffers. */
    numOfBufs = *numOfBufsPtr;
    *numOfBufsPtr = 0;

    /* Set tx parameters */
    txQueue     = pcktParamsPtr->sdmaInfo.txQueue;
    PRV_CPSS_DXCH_DEV_TX_SDMA_QUEUE_NUM_CHECK_MAC(txQueue);

    moduleCfgPtr = PRV_CPSS_DXCH_DEV_MODULE_CFG_MAC(devNum);

    /* Queue is assigned for Packet Generator */
    if(CPSS_TX_SDMA_QUEUE_MODE_PACKET_GENERATOR_E ==
          moduleCfgPtr->
               multiNetIfCfg.txSdmaQueuesConfig[portGroupId][txQueue].queueMode)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    rc = prvCpssDxChNetIfSdmaTxDsaLengthAndDataOffsetGet(pcktParamsPtr,
                                                         &dsaTagNumOfBytes,
                                                         &dataOffset);
    if( GT_OK != rc )
    {
        return rc;
    }

    /* Number of buffers added - 1 for the DSA Tag, 1 for split data after */
    /* the Source MAC Address and Destination MAC Address */
    numOfAddedBuffs = 2;

    if(GT_TRUE == PRV_CPSS_DXCH_ERRATA_GET_MAC(devNum, PRV_CPSS_DXCH_XCAT_TX_CPU_CORRUPT_BUFFER_WA_E))
    {
        /* calc packet length */
        packetLen = dsaTagNumOfBytes + PRV_CPSS_DXCH_ETHPRT_HDR_MAC_SIZE_CNS - dataOffset;
        for( i = 0 ; i < numOfBufs ; i++ )
        {
            packetLen += buffLenList[i];
        }

        /* add 8 byte padding (another buffer is added, numOfAddedBuffs = 3) should */
        /* be done only if packets length is <byteblock>*n+k (1<=k<=8) (including   */
        /* DSA tag), where n and k are integers and <byteblock> is 256 for xCat */
        /* and 512 for DxChLion,                                  */
        /* otherwise (numOfAddedBuffs = 2) and previous flow is preserved.          */
        if((PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_DXCH_LION_E)||
           (PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_DXCH_LION2_E) ||
           (PRV_CPSS_SIP_5_CHECK_MAC(devNum)))
        {
            byteBlockMask = 0x1FF;
        }
        else /* xCat A1 */
        {
            byteBlockMask = 0xFF;
        }

        if( (1 <= (packetLen & byteBlockMask)) && (8 >= (packetLen & byteBlockMask)) )
        {
            numOfAddedBuffs = 3;
        }
    }

    if((numOfBufs + numOfAddedBuffs) > txDescListPtr->maxDescNum)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    /* check available buffers in pool */
    if (cpssBmPoolBufFreeCntGet(txDescListPtr->poolId) < 1)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NO_RESOURCE, LOG_ERROR_NO_MSG);
    }

    /* Check if there are enough descriptors for this packet. */
    if((numOfBufs + numOfAddedBuffs) > txDescListPtr->freeDescNum)
    {
        if(PRV_CPSS_PP_MAC(devNum)->useIsr == GT_FALSE)
        {
            /* the device was NOT bound to the ISR ,
               and the EVENTs are not handled by the cpss */

            /* return 'Empty' to tell the application , to take care of the
               descriptors , because we have not enough descriptor to do the 'TX'
            */
            /* note : we must not call the
                prvCpssDxChNetIfSdmaTxBuffQueueEvIsrHandle , because this
                function wants to use the EVENTs mechanism of the CPSS, by
                releasing the descriptors but also sending the 'cookie' info to
                the events queue handling
            */

            /* Application should call function cpssDxChNetIfSdmaTxBufferQueueDescFreeAndCookieGet
               to release the descriptors ,and get the 'cookie' indication */
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_EMPTY, LOG_ERROR_NO_MSG);
        }
        else /* the device was bound to the ISR */
        {
            /* Call prvCpssDxChNetIfSdmaTxBuffQueueEvIsrHandle to free buffers.
            Must be under interrupt disabled */
            cpssOsSetIntLockUnlock(CPSS_OS_INTR_MODE_LOCK_E, &key);
            rc = prvCpssDxChNetIfSdmaTxBuffQueueEvIsrHandle(devNum, txQueue);
            cpssOsSetIntLockUnlock(CPSS_OS_INTR_MODE_UNLOCK_E, &key);
            if(rc != GT_OK)
                return rc;
            /* Check again if there are enough descriptors for this packet. */
            if((numOfBufs + numOfAddedBuffs) > txDescListPtr->freeDescNum)
                CPSS_LOG_ERROR_AND_RETURN_MAC(GT_EMPTY, LOG_ERROR_NO_MSG);
        }
    }

    /* the descriptors start to change here */
    *txDescListUpdatedPtr = GT_TRUE;

    /* Set descriptor parameters */
    currSwDesc              = txDescListPtr->next2Feed;
    firstDesc               = currSwDesc->txDesc;
    /* Set the cookie in the userData - usually the packet pointer received by
       the application */
    currSwDesc->userData = pcktParamsPtr->cookie;
    currSwDesc->evReqHndl= pcktParamsPtr->sdmaInfo.evReqHndl;

    /***********************************************/
    /* start with the original first buffer's data */
    /***********************************************/
    descWord1 = 0;

    if(pcktParamsPtr->sdmaInfo.recalcCrc == GT_TRUE)
    {
        descWord1 |= (1 << 12);
    }

    /* Set the first descriptor parameters. */
    TX_DESC_RESET_MAC(&tmpFirstTxDesc);

    tmpFirstTxDesc.word1 = descWord1;

    /* Set bit for first buffer of a frame for Temporary Tx descriptor */
    TX_DESC_SET_FIRST_BIT_MAC(&tmpFirstTxDesc,1);
    /* Add Source MAC Address and Destination MAC Address */
    TX_DESC_SET_BYTE_CNT_MAC(&tmpFirstTxDesc,PRV_CPSS_DXCH_ETHPRT_HDR_MAC_SIZE_CNS); /*12*/

    /* update the packet header to the first descriptor */
    rc = cpssOsVirt2Phy((GT_UINTPTR)buffList[0],&tmpBuffPtr);
    if (rc != GT_OK)
    {
        return rc;
    }

    #if __WORDSIZE == 64    /* phyAddr must fit in 32 bit */
        if (0 != (tmpBuffPtr & 0xffffffff00000000L))
        {
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_OUT_OF_RANGE, LOG_ERROR_NO_MSG);
        }
    #endif

    tmpFirstTxDesc.buffPointer = CPSS_32BIT_LE((GT_U32)tmpBuffPtr);

    /***************/
    /* Add DSA Tag */
    /***************/
    currSwDesc = currSwDesc->swNextDesc;

    TX_DESC_RESET_MAC(&tmpTxDesc);
    tmpTxDesc.word1 = descWord1;

    /* Use Short buffer in this case*/
    /* Build the DSA Tag info into the short buffer of the descriptor */
    rc = cpssDxChNetIfDsaTagBuild(devNum,
                                     &pcktParamsPtr->dsaParam,
                                     &currSwDesc->shortBuffer[0]);
    if(rc != GT_OK)
    {
        return rc;
    }

    rc = cpssOsVirt2Phy((GT_UINTPTR)(currSwDesc->shortBuffer),/*OUT*/&tmpBuffPtr);
    if (rc != GT_OK)
    {
        return rc;
    }

    #if __WORDSIZE == 64    /* phyAddr must fit in 32 bit */
        if (0 != (tmpBuffPtr & 0xffffffff00000000L))
        {
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_OUT_OF_RANGE, LOG_ERROR_NO_MSG);
        }
    #endif

    tmpTxDesc.buffPointer = CPSS_32BIT_LE((GT_U32)tmpBuffPtr);

    currSwDesc->userData = pcktParamsPtr->cookie;
    /* Set byte counter to DSA Tag size */
    TX_DESC_SET_BYTE_CNT_MAC(&tmpTxDesc, dsaTagNumOfBytes);
    /* Set ownership bit - owned by DMA */
    TX_DESC_SET_OWN_BIT_MAC(&tmpTxDesc,TX_DESC_DMA_OWN);

    tmpTxDesc.word1 = CPSS_32BIT_LE(tmpTxDesc.word1);
    tmpTxDesc.word2 = CPSS_32BIT_LE(tmpTxDesc.word2);
    TX_DESC_COPY_MAC(currSwDesc->txDesc,&tmpTxDesc);

    /**************************************************/
    /* continue with the original first buffer's data */
    /**************************************************/
    currSwDesc = currSwDesc->swNextDesc;

    TX_DESC_RESET_MAC(&tmpTxDesc);
    tmpTxDesc.word1 = descWord1;

    /* Get the Data */
    rc = cpssOsVirt2Phy((GT_UINTPTR)( ((GT_U8*)buffList[0]) + dataOffset ),/*OUT*/&tmpBuffPtr);
    if (rc != GT_OK)
    {
        return rc;
    }

    #if __WORDSIZE == 64    /* phyAddr must fit in 32 bit */
        if (0 != (tmpBuffPtr & 0xffffffff00000000L))
        {
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_OUT_OF_RANGE, LOG_ERROR_NO_MSG);
        }
    #endif

    tmpTxDesc.buffPointer = CPSS_32BIT_LE((GT_U32)tmpBuffPtr);

    currSwDesc->userData = pcktParamsPtr->cookie;
    /* Set byte counter to buffer size minus first descriptor */
    TX_DESC_SET_BYTE_CNT_MAC(&tmpTxDesc, ( buffLenList[0] - dataOffset ));
    /* Set ownership bit - owned by DMA */
    TX_DESC_SET_OWN_BIT_MAC(&tmpTxDesc,TX_DESC_DMA_OWN);

    /* in case last descriptor don't swap
       need to set: Enable Interrupt bit, Last buffer of frame bit */
    if(numOfBufs != 1)
    {
        tmpTxDesc.word1 = CPSS_32BIT_LE(tmpTxDesc.word1);
        tmpTxDesc.word2 = CPSS_32BIT_LE(tmpTxDesc.word2);

        TX_DESC_COPY_MAC(currSwDesc->txDesc,&tmpTxDesc);

        /*******************************************/
        /* continue with the original next buffers */
        /*******************************************/


        /* Add rest of the buffers (starting from second buffer) */
        for(i = 1; i < numOfBufs; i++)
        {
            currSwDesc = currSwDesc->swNextDesc;

            TX_DESC_RESET_MAC(&tmpTxDesc);
            tmpTxDesc.word1 = descWord1;

            /* Check if the buffers length is larger than (TX_SHORT_BUFF_SIZE)  */
            if(((PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_MAC(devNum) == GT_TRUE) &&
                (buffLenList[i] > TX_SHORT_BUFF_SIZE_16_CNS)) ||
                ((PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_MAC(devNum) == GT_FALSE) &&
                 (buffLenList[i] > TX_SHORT_BUFF_SIZE)))
            {
                rc = cpssOsVirt2Phy((GT_UINTPTR)(buffList[i]),/*OUT*/&tmpBuffPtr);
                if (rc != GT_OK)
                {
                    return rc;
                }

                #if __WORDSIZE == 64    /* phyAddr must fit in 32 bit */
                    if (0 != (tmpBuffPtr & 0xffffffff00000000L))
                    {
                        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_OUT_OF_RANGE, LOG_ERROR_NO_MSG);
                    }
                #endif

            }
            else
            {
                cpssOsMemCpy(currSwDesc->shortBuffer,buffList[i],buffLenList[i]);
                rc = cpssOsVirt2Phy((GT_UINTPTR)(currSwDesc->shortBuffer),&tmpBuffPtr);
                if (rc != GT_OK)
                {
                    return rc;
                }

                #if __WORDSIZE == 64    /* phyAddr must fit in 32 bit */
                    if (0 != (tmpBuffPtr & 0xffffffff00000000L))
                    {
                        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_OUT_OF_RANGE, LOG_ERROR_NO_MSG);
                    }
                #endif

            }

            tmpTxDesc.buffPointer = CPSS_32BIT_LE((GT_U32)tmpBuffPtr);
            currSwDesc->userData = pcktParamsPtr->cookie;
            TX_DESC_SET_BYTE_CNT_MAC(&tmpTxDesc,buffLenList[i]);
            TX_DESC_SET_OWN_BIT_MAC(&tmpTxDesc,TX_DESC_DMA_OWN);

            /* in case last descriptor don't swap  */
            /* need to set: Enable Interrupt bit, Last buffer of frame bit */
            /* In case PRV_CPSS_DXCH_XCAT_TX_CPU_CORRUPT_BUFFER_WA_E is */
            /* to be done (3 == numOfAddedBuffs) the last buffer in the */
            /* received buffer list is not the last one.                */
            if((i != (numOfBufs - 1)) || (3 == numOfAddedBuffs) )
            {
                tmpTxDesc.word1 = CPSS_32BIT_LE(tmpTxDesc.word1);
                tmpTxDesc.word2 = CPSS_32BIT_LE(tmpTxDesc.word2);
                TX_DESC_COPY_MAC(currSwDesc->txDesc,&tmpTxDesc);
            }
        }
    }

    /* padded 8 bytes for PRV_CPSS_DXCH_XCAT_TX_CPU_CORRUPT_BUFFER_WA_E */
    if( 3 == numOfAddedBuffs )
    {
        if(numOfBufs == 1)
        {
            /* swapping the previous descriptor in case only one buffer was */
            /* supplied to the routine */
            tmpTxDesc.word1 = CPSS_32BIT_LE(tmpTxDesc.word1);
            tmpTxDesc.word2 = CPSS_32BIT_LE(tmpTxDesc.word2);
            TX_DESC_COPY_MAC(currSwDesc->txDesc,&tmpTxDesc);
        }

        currSwDesc = currSwDesc->swNextDesc;

        TX_DESC_RESET_MAC(&tmpTxDesc);
        tmpTxDesc.word1 = descWord1;

        /* padding with zeros */
        cpssOsMemSet(currSwDesc->shortBuffer, 0x0, 8);

        rc = cpssOsVirt2Phy((GT_UINTPTR)(currSwDesc->shortBuffer),/*OUT*/&tmpBuffPtr);
        if (rc != GT_OK)
        {
            return rc;
        }

        #if __WORDSIZE == 64    /* phyAddr must fit in 32 bit */
            if (0 != (tmpBuffPtr & 0xffffffff00000000L))
            {
                CPSS_LOG_ERROR_AND_RETURN_MAC(GT_OUT_OF_RANGE, LOG_ERROR_NO_MSG);
            }
        #endif

        tmpTxDesc.buffPointer = CPSS_32BIT_LE((GT_U32)tmpBuffPtr);
        currSwDesc->userData = pcktParamsPtr->cookie;
        TX_DESC_SET_BYTE_CNT_MAC(&tmpTxDesc, 8);
        TX_DESC_SET_OWN_BIT_MAC(&tmpTxDesc,TX_DESC_DMA_OWN);

        /* last descriptor don't swap */
    }

    /**************************************/
    /* Set the last descriptor parameters */
    /**************************************/
    /* Set the bit for Last buffer of a frame */
    TX_DESC_SET_LAST_BIT_MAC(&tmpTxDesc,1);

    /* check if caller want to get the event invocation ,
       NOTE : we must not invoke when sending "synchronic" */
    if((pcktParamsPtr->sdmaInfo.invokeTxBufferQueueEvent == GT_TRUE) &&
       (synchronicSend == GT_FALSE))
    {
        /* Set the bit for Enable Interrupt */
        TX_DESC_SET_INT_BIT_MAC(&tmpTxDesc,1);
    }
    else
        /* Set the bit for Disable Interrupt */
        TX_DESC_SET_INT_BIT_MAC(&tmpTxDesc,0);

    tmpTxDesc.word1 = CPSS_32BIT_LE(tmpTxDesc.word1);
    tmpTxDesc.word2 = CPSS_32BIT_LE(tmpTxDesc.word2);
    TX_DESC_COPY_MAC(currSwDesc->txDesc,&tmpTxDesc);

    txDescListPtr->freeDescNum -= (numOfBufs + numOfAddedBuffs);
    txDescListPtr->next2Feed    = currSwDesc->swNextDesc;
    /* Set the sctual number of used buffers. */
    *numOfBufsPtr = numOfBufs + numOfAddedBuffs;

    /* Make sure that all previous operations where */
    /* executed before changing the own bit of the  */
    /* first descriptor.                            */
    GT_SYNC;

    /* Set the first descriptor own bit to start transmitting.  */
    TX_DESC_SET_OWN_BIT_MAC(&tmpFirstTxDesc,TX_DESC_DMA_OWN);
    tmpFirstTxDesc.word1  = CPSS_32BIT_LE(tmpFirstTxDesc.word1);
    tmpFirstTxDesc.word2  = CPSS_32BIT_LE(tmpFirstTxDesc.word2);
    TX_DESC_COPY_MAC(firstDesc,&tmpFirstTxDesc);

    /* The Enable DMA operation should be done only */
    /* AFTER all desc. operations where completed.  */
    GT_SYNC;

    /* Enable the Tx DMA.   */
    txCmdMask = 1 << txQueue;
    rc = prvCpssHwPpPortGroupWriteRegister(devNum,portGroupId,PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->sdmaRegs.txQCmdReg,txCmdMask);
    if(rc != GT_OK)
        return rc;

    if(synchronicSend == GT_TRUE)
    {
#ifndef ASIC_SIMULATION
        /* DMA procedure may take a long time for Jumbo packets.
          E.g. the TX of 10240 Byte packet may take 411 micro seconds for PEX Gen 1.1.
          PEX Gen 1.1 haz 250 MBytes per second maximal throughput. The TX of
          10240 bytes minimal time will take:
           (10240 + 16 (eDSA size) + 16 (descriptor size)) / 250 M = 41.088 microseconds
          The ARM v7 CPU with 1.3 Giga Core Clock and DDR3-1333 performs
          approximately 15 millions per second loops of OWN Bit check algorithm.
          It's required 41.088 * 15 = 613 loops for ARM CPU and maximal PEX 1.1 throughput.
          Choose approx 500 times more loops to be on safe side for high end CPUs - 312000.
          The "numOfBufs + numOfAddedBuffs"  is 3 or more.
          Use constant 104000 to get 312000 loop count.*/
        GT_U32 loopIndex = (numOfBufs + numOfAddedBuffs) * 104000;
#else
        GT_U32 loopIndex = (numOfBufs + numOfAddedBuffs) * 500;
        GT_U32 sleepTime;/*time to sleep , to allow the 'Asic simulation' process the packet */

        /* allow another task to process it if ready , without 1 millisecond penalty */
        /* this code of sleep 0 Vs sleep 1 boost the performance *20 in enhanced-UT !!! */
        sleepTime = 0;

        tryMore_lbl:
#endif /*ASIC_SIMULATION*/

        /* Wait until PP sent the packet. Or HW error if while block
                   run more then loopIndex times */
        while (loopIndex && (TX_DESC_GET_OWN_BIT_MAC(devNum,currSwDesc->txDesc) == TX_DESC_DMA_OWN))
        {
#ifdef ASIC_SIMULATION
            /* do some sleep allow the simulation process the packet */
            cpssOsTimerWkAfter(sleepTime);
#endif /*ASIC_SIMULATION*/
            loopIndex--;
        }

        if(loopIndex == 0)
        {
#ifdef ASIC_SIMULATION
            if(sleepTime == 0)/* the TX was not completed ? we need to allow more retries with 'sleep (1)'*/
            {
                loopIndex = (numOfBufs + numOfAddedBuffs) * 500;
                sleepTime = 1;
                goto tryMore_lbl;
            }
#endif /*ASIC_SIMULATION*/
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_HW_ERROR, LOG_ERROR_NO_MSG);
        }

    }


    return GT_OK;
}

/*******************************************************************************
* internal_cpssDxChNetIfSdmaTxPacketSend
*
* DESCRIPTION:
*       This function receives packet buffers & parameters from Application .
*       Prepares them for the transmit operation, and enqueues the prepared
*       descriptors to the PP's tx queues.  -- SDMA relate.
*       function activates Tx SDMA , function doesn't wait for event of
*       "Tx buffer queue" from PP
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1_Diamond.
*
* INPUTS:
*       devNum       - Device number.
*       pcktParamsPtr- The internal packet params to be set into the packet
*                      descriptors.
*       buffList     - The packet data buffers list.
*       buffLenList  - A list of the buffers len in buffList.
*       numOfBufs    - Length of buffList.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK          - on success, or
*       GT_NO_RESOURCE - if there is not enough free elements in the fifo
*                        associated with the Event Request Handle.
*       GT_EMPTY       - if there are not enough descriptors to do the sending.
*       GT_BAD_PARAM   - on bad DSA params or the data buffer is longer
*                        than allowed. Buffer data can occupied up to the
*                        maximum number of descriptors defined.
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_INITIALIZED       - the library was not initialized
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_FAIL otherwise.
*
* COMMENTS:
*    1. Support both regular DSA tag and extended DSA tag.
*    2. Each buffer must be at least 8 bytes long. First buffer must be at
*       least 24 bytes for tagged packet, 20 for untagged packet.
*    3. If returned status is GT_NO_RESOURCE then the application should free
*       the elements in the fifo (associated with the Handle) by calling
*       cpssDxChNetIfTxBufferQueueGet, and send the packet again.
*    4. If returned status is GT_EMPTY and CPSS handling the events
*       of the device then the application should wait and try to send the
*       packet again.
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChNetIfSdmaTxPacketSend
(
    IN GT_U8                                    devNum,
    IN CPSS_DXCH_NET_TX_PARAMS_STC              *pcktParamsPtr,
    IN GT_U8                                    *buffList[],
    IN GT_U32                                   buffLenList[],
    IN GT_U32                                   numOfBufs
)
{
    GT_STATUS rc = GT_OK;
    GT_U8                      txQueue;   /*The tx queue to send this packet to*/
    PRV_CPSS_TX_DESC_LIST_STC *txDescList;/*The Tx desc. list control structure*/
    GT_BOOL   txDescListUpdated;/* do we need to restore descriptors -->
                        dummy parameter not used for 'asynch sending'*/

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    CPSS_NULL_PTR_CHECK_MAC(pcktParamsPtr);
    CPSS_NULL_PTR_CHECK_MAC(buffList);
    CPSS_NULL_PTR_CHECK_MAC(buffLenList);
    PRV_CPSS_DXCH_SDMA_USED_CHECK_MAC(devNum);

    /* Diamond Cut have not SDMA registers */

    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum, CPSS_CH1_DIAMOND_E);


    if(!PRV_CPSS_DXCH_CHECK_SDMA_PACKETS_FROM_CPU_CORRUPT_WA_MAC(devNum))
    {
        /*check that the DB of networkIf library was initialized*/
        PRV_CPSS_DXCH_NETIF_INITIALIZED_CHECK_MAC(devNum);

        txQueue     = pcktParamsPtr->sdmaInfo.txQueue;
        txDescList  = &(PRV_CPSS_PP_MAC(devNum)->intCtrl.txDescList[txQueue]);

        /* asynchronous sending */
        rc = dxChNetIfSdmaTxPacketSend(devNum, pcktParamsPtr, buffList, buffLenList,
                                       &numOfBufs, GT_FALSE, txDescList ,
                                       &txDescListUpdated);
    }
    else
    {
        rc = prvCpssGenNetIfMiiTxModeSet(PRV_CPSS_GEN_NETIF_MII_TX_MODE_ASYNCH_E);
        if (rc != GT_OK)
            return rc;

        rc = prvCpssDxChNetIfMiiPacketTx(devNum, pcktParamsPtr, buffList,
                                         buffLenList, numOfBufs);
    }

    return rc;
}

/*******************************************************************************
* cpssDxChNetIfSdmaTxPacketSend
*
* DESCRIPTION:
*       This function receives packet buffers & parameters from Application .
*       Prepares them for the transmit operation, and enqueues the prepared
*       descriptors to the PP's tx queues.  -- SDMA relate.
*       function activates Tx SDMA , function doesn't wait for event of
*       "Tx buffer queue" from PP
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1_Diamond.
*
* INPUTS:
*       devNum       - Device number.
*       pcktParamsPtr- The internal packet params to be set into the packet
*                      descriptors.
*       buffList     - The packet data buffers list.
*       buffLenList  - A list of the buffers len in buffList.
*       numOfBufs    - Length of buffList.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK          - on success, or
*       GT_NO_RESOURCE - if there is not enough free elements in the fifo
*                        associated with the Event Request Handle.
*       GT_EMPTY       - if there are not enough descriptors to do the sending.
*       GT_BAD_PARAM   - on bad DSA params or the data buffer is longer
*                        than allowed. Buffer data can occupied up to the
*                        maximum number of descriptors defined.
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_INITIALIZED       - the library was not initialized
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_FAIL otherwise.
*
* COMMENTS:
*    1. Support both regular DSA tag and extended DSA tag.
*    2. Each buffer must be at least 8 bytes long. First buffer must be at
*       least 24 bytes for tagged packet, 20 for untagged packet.
*    3. If returned status is GT_NO_RESOURCE then the application should free
*       the elements in the fifo (associated with the Handle) by calling
*       cpssDxChNetIfTxBufferQueueGet, and send the packet again.
*    4. If returned status is GT_EMPTY and CPSS handling the events
*       of the device then the application should wait and try to send the
*       packet again.
*
*******************************************************************************/
GT_STATUS cpssDxChNetIfSdmaTxPacketSend
(
    IN GT_U8                                    devNum,
    IN CPSS_DXCH_NET_TX_PARAMS_STC              *pcktParamsPtr,
    IN GT_U8                                    *buffList[],
    IN GT_U32                                   buffLenList[],
    IN GT_U32                                   numOfBufs
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChNetIfSdmaTxPacketSend);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, pcktParamsPtr, buffList, buffLenList, numOfBufs));

    rc = internal_cpssDxChNetIfSdmaTxPacketSend(devNum, pcktParamsPtr, buffList, buffLenList, numOfBufs);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, pcktParamsPtr, buffList, buffLenList, numOfBufs));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChNetIfSdmaSyncTxPacketSend
*
*       This function receives packet buffers & parameters from Applications .
*       Prepares them for the transmit operation, and enqueues the prepared
*       descriptors to the PP's tx queues.
*       After transmition end all transmitted packets descriptors are freed.
*        -- SDMA relate.
*       function activates Tx SDMA , function wait for PP to finish processing
*       the buffers.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1_Diamond.
*
* INPUTS:
*       devNum       - Device number.
*       pcktParamsPtr- The internal packet params to be set into the packet
*                      descriptors.
*       buffList     - The packet data buffers list.
*       buffLenList  - A list of the buffers len in buffList.
*       numOfBufs    - Length of buffList.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK          - on success, or
*       GT_NO_RESOURCE - if there is not enough free elements in the fifo
*                        associated with the Event Request Handle.
*       GT_EMPTY       - if there are not enough descriptors to do the sending.
*       GT_HW_ERROR    - when after transmission last descriptor own bit wasn't
*                        changed for long time.
*       GT_BAD_PARAM   - on bad DSA params or the data buffer is longer
*                        than allowed. Buffer data can occupied up to the
*                        maximum number of descriptors defined.
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_INITIALIZED       - the library was not initialized
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_FAIL otherwise.
*
* COMMENTS:
*    1. Support both regular DSA tag and extended DSA tag.
*    2. Each buffer must be at least 8 bytes long. First buffer must be at
*       least 24 bytes for tagged packet, 20 for untagged packet.
*    3. If returned status is GT_NO_RESOURCE then the application should free
*       the elements in the fifo (associated with the Handle) by calling
*       cpssDxChNetIfTxBufferQueueGet, and send the packet again.
*    4. If returned status is GT_EMPTY and CPSS handling the events
*       of the device then the application should wait and try to send the
*       packet again.
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChNetIfSdmaSyncTxPacketSend
(
    IN GT_U8                                    devNum,
    IN CPSS_DXCH_NET_TX_PARAMS_STC              *pcktParamsPtr,
    IN GT_U8                                    *buffList[],
    IN GT_U32                                   buffLenList[],
    IN GT_U32                                   numOfBufs
)
{
    GT_STATUS rc0 = GT_OK;
    GT_STATUS rc1 = GT_OK;
    GT_BOOL   txDescListUpdated;/* do we need to restore descriptors */
    GT_U8 txQueue;              /* The tx queue to send this packet to. */
    PRV_CPSS_TX_DESC_LIST_STC *txDescList;   /* The Tx desc. list control structure. */
    PRV_CPSS_SW_TX_DESC_STC    *firstDescPtr;
    GT_U32                      numOfUsedDesc;

    GT_UINTPTR                  phyNext2Feed; /* The physicat address of the next2Feed*/
                                              /* field.                               */
    PRV_CPSS_DXCH_PP_REGS_ADDR_STC *regsAddr;
    GT_U32         portGroupId;/* port group Id for multi-port-groups device support */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    CPSS_NULL_PTR_CHECK_MAC(pcktParamsPtr);
    CPSS_NULL_PTR_CHECK_MAC(buffList);
    CPSS_NULL_PTR_CHECK_MAC(buffLenList);
    PRV_CPSS_DXCH_SDMA_USED_CHECK_MAC(devNum);

    /* Diamond Cut have not SDMA registers */

    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum, CPSS_CH1_DIAMOND_E);


    if(!PRV_CPSS_DXCH_CHECK_SDMA_PACKETS_FROM_CPU_CORRUPT_WA_MAC(devNum))
    {
        /*check that the DB of networkIf library was initialized*/
        PRV_CPSS_DXCH_NETIF_INITIALIZED_CHECK_MAC(devNum);

        /* we will use the 'First active port group' , to represent the whole device.
           that way we allow application to give SDMA memory to single port group instead
           of split it between all active port groups
        */
        portGroupId = PRV_CPSS_NETIF_SDMA_PORT_GROUP_ID_MAC(devNum);

        txQueue     = pcktParamsPtr->sdmaInfo.txQueue;
        txDescList  = &(PRV_CPSS_PP_MAC(devNum)->intCtrl.txDescList[txQueue]);


        /* dxChNetIfSdmaTxPacketSend changes txDescList->next2Feed, keep it here
        and return it back after cleaning. */
        firstDescPtr = txDescList->next2Feed;

        numOfUsedDesc = numOfBufs;

        /* synchronic sending */
        rc0 =  dxChNetIfSdmaTxPacketSend(devNum,pcktParamsPtr,buffList,buffLenList,
                                        &numOfUsedDesc, GT_TRUE, txDescList ,
                                        &txDescListUpdated);

        if (txDescListUpdated == GT_TRUE)
        {
            /* Don't check rc here, need to restore data structure*/

            /* Restore next to feed descriptor. */
            txDescList->next2Feed = firstDescPtr;
            txDescList->freeDescNum += numOfUsedDesc;

            /* Returned the buffers to application. Return the state of descriptors as
             it was before calling internalTxPacketSend. Set txDescList->next2Feed
             to point to the same SW_DESC before calling internalTxPacketSend
             Then return the status to the caller (even in case of error). */
            while(numOfUsedDesc > 0)
            {
                firstDescPtr->txDesc->word1 = 0x0;
                firstDescPtr->txDesc->word2 = 0x0;
                firstDescPtr = firstDescPtr->swNextDesc;
                numOfUsedDesc--;
            }

            /* Set/Restore Tx Current Desc Pointer Register. */
            regsAddr    = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum);
            rc1 = cpssOsVirt2Phy((GT_UINTPTR)(txDescList->next2Feed->txDesc),/*OUT*/&phyNext2Feed);
            if (rc1 != GT_OK)
            {
                return rc1;
            }

            #if __WORDSIZE == 64    /* phyAddr must fit in 32 bit */
                if (0 != (phyNext2Feed & 0xffffffff00000000L))
                {
                    CPSS_LOG_ERROR_AND_RETURN_MAC(GT_OUT_OF_RANGE, LOG_ERROR_NO_MSG);
                }
            #endif

            rc1 = prvCpssHwPpPortGroupWriteRegister(devNum,portGroupId,regsAddr->sdmaRegs.txDmaCdp[txQueue],(GT_U32)phyNext2Feed);
            if(rc1 != GT_OK)
                return rc1;
        }
    }
    else
    {

        rc0 = prvCpssGenNetIfMiiTxModeSet(PRV_CPSS_GEN_NETIF_MII_TX_MODE_SYNCH_E);
        if (rc0 != GT_OK)
            return rc0;

        rc0 = prvCpssDxChNetIfMiiPacketTx(devNum, pcktParamsPtr, buffList,
                                         buffLenList, numOfBufs);
    }

    return rc0;
}

/*******************************************************************************
* cpssDxChNetIfSdmaSyncTxPacketSend
*
*       This function receives packet buffers & parameters from Applications .
*       Prepares them for the transmit operation, and enqueues the prepared
*       descriptors to the PP's tx queues.
*       After transmition end all transmitted packets descriptors are freed.
*        -- SDMA relate.
*       function activates Tx SDMA , function wait for PP to finish processing
*       the buffers.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1_Diamond.
*
* INPUTS:
*       devNum       - Device number.
*       pcktParamsPtr- The internal packet params to be set into the packet
*                      descriptors.
*       buffList     - The packet data buffers list.
*       buffLenList  - A list of the buffers len in buffList.
*       numOfBufs    - Length of buffList.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK          - on success, or
*       GT_NO_RESOURCE - if there is not enough free elements in the fifo
*                        associated with the Event Request Handle.
*       GT_EMPTY       - if there are not enough descriptors to do the sending.
*       GT_HW_ERROR    - when after transmission last descriptor own bit wasn't
*                        changed for long time.
*       GT_BAD_PARAM   - on bad DSA params or the data buffer is longer
*                        than allowed. Buffer data can occupied up to the
*                        maximum number of descriptors defined.
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_INITIALIZED       - the library was not initialized
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_FAIL otherwise.
*
* COMMENTS:
*    1. Support both regular DSA tag and extended DSA tag.
*    2. Each buffer must be at least 8 bytes long. First buffer must be at
*       least 24 bytes for tagged packet, 20 for untagged packet.
*    3. If returned status is GT_NO_RESOURCE then the application should free
*       the elements in the fifo (associated with the Handle) by calling
*       cpssDxChNetIfTxBufferQueueGet, and send the packet again.
*    4. If returned status is GT_EMPTY and CPSS handling the events
*       of the device then the application should wait and try to send the
*       packet again.
*
*******************************************************************************/
GT_STATUS cpssDxChNetIfSdmaSyncTxPacketSend
(
    IN GT_U8                                    devNum,
    IN CPSS_DXCH_NET_TX_PARAMS_STC              *pcktParamsPtr,
    IN GT_U8                                    *buffList[],
    IN GT_U32                                   buffLenList[],
    IN GT_U32                                   numOfBufs
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChNetIfSdmaSyncTxPacketSend);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, pcktParamsPtr, buffList, buffLenList, numOfBufs));

    rc = internal_cpssDxChNetIfSdmaSyncTxPacketSend(devNum, pcktParamsPtr, buffList, buffLenList, numOfBufs);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, pcktParamsPtr, buffList, buffLenList, numOfBufs));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* prvCpssDxChNetIfSdmaTxBuffQueueEvIsrHandle
*
* DESCRIPTION:
*       This routine frees all transmitted packets descriptors. In addition, all
*       user relevant data in Tx End FIFO.  -- SDMA relate.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1_Diamond.
*
* INPUTS:
*       devNum      - The device number the packet was transmitted from
*       queueIdx    - The queue the packet was transmitted upon.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK on success, or
*       GT_NOT_INITIALIZED       - the library was not initialized
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_BAD_STATE             - the CPSS does not handle the ISR for the device
*                                  so function must not be called
*       GT_FAIL otherwise.
*
* COMMENTS:
*       Invoked by ISR routine !!
*       the function is called internally in the CPSS from the driver as a call
*       back , see bind in function hwPpPhase1Part1(...)
*
*******************************************************************************/
GT_STATUS prvCpssDxChNetIfSdmaTxBuffQueueEvIsrHandle
(
    IN GT_U8                devNum,
    IN GT_U8                queueIdx
)
{
    PRV_CPSS_TX_DESC_LIST_STC       *txDescListPtr;
    /* Counts the number of descriptors already freed. */
    GT_U32                          freeDescNum;
    PRV_CPSS_SW_TX_DESC_STC         *descPtr;
    /* Points to the first sw desc. to be sent to the callback function.*/
    PRV_CPSS_SW_TX_DESC_STC         *firstTxDescPtr;
    /* Number of descriptors this packet occupies to be sent to the callback
       function.*/
    GT_U32                          descNum;
    PRV_CPSS_TX_DESC_STC            tmpTxDesc;
    PRV_CPSS_SW_TX_FREE_DATA_STC    *txFreeDataPtr;
    PRV_CPSS_DRV_EVENT_HNDL_STC     *evHndlPtr;
    PRV_CPSS_TX_BUF_QUEUE_FIFO_STC  *fifoPtr;

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    /* Diamond Cut have not SDMA registers */

    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum, CPSS_CH1_DIAMOND_E);

    PRV_CPSS_DXCH_COS_CHECK_TC_MAC(queueIdx);

    /*check that the DB of networkIf library was initialized*/
    PRV_CPSS_DXCH_NETIF_INITIALIZED_CHECK_MAC(devNum);

    txDescListPtr = &(PRV_CPSS_PP_MAC(devNum)->intCtrl.txDescList[queueIdx]);

    if(PRV_CPSS_PP_MAC(devNum)->useIsr == GT_FALSE)
    {
        /* the CPSS not handling the ISR for the device so function must not be called */
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_STATE, LOG_ERROR_NO_MSG);
    }

    descPtr = txDescListPtr->next2Free;

    tmpTxDesc.word1 = descPtr->txDesc->word1;

    /* No more descriptors to release.  */
    while (TX_DESC_GET_OWN_BIT_MAC(devNum,&tmpTxDesc) != TX_DESC_DMA_OWN)
    {
        if (TX_DESC_GET_FIRST_BIT_MAC(devNum,&tmpTxDesc) == 0)
        {
            /* It may happen if all the Tx descriptors are free - this is legal case
               In this case - should not return GT_ERROR */
            if(txDescListPtr->freeDescNum == txDescListPtr->maxDescNum)
            {
                return GT_OK;
            }

            /* should not happen */
            CPSS_LOG_ERROR_AND_RETURN_MAC(((GT_STATUS)GT_ERROR), LOG_ERROR_NO_MSG);
        }

        descNum     = 1;
        firstTxDescPtr = descPtr;
        evHndlPtr   = (PRV_CPSS_DRV_EVENT_HNDL_STC*)descPtr->evReqHndl;

        /* Get the packet's descriptors.        */
        while (TX_DESC_GET_LAST_BIT_MAC(devNum,&tmpTxDesc) == 0x0)
        {
            if (TX_DESC_GET_OWN_BIT_MAC(devNum,&tmpTxDesc) == TX_DESC_DMA_OWN)
            {
                return GT_OK;
            }
            descPtr = descPtr->swNextDesc;
            tmpTxDesc.word1 = descPtr->txDesc->word1;
            descNum++;
        }

        if (TX_DESC_GET_OWN_BIT_MAC(devNum,&tmpTxDesc) == TX_DESC_DMA_OWN)
        {
            return GT_OK;
        }

        /* for not control packets/master/stand alone/device that doesn't need trap2cpu WA */
        freeDescNum = descNum;

        if ((txFreeDataPtr = cpssBmPoolBufGet(txDescListPtr->poolId)) == NULL)
        {
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NO_RESOURCE, LOG_ERROR_NO_MSG);
        }

        /* save the Tx End data in FIFO node */
        txFreeDataPtr->devNum   = devNum;
        txFreeDataPtr->queueNum = queueIdx;
        txFreeDataPtr->userData = firstTxDescPtr->userData;
        txFreeDataPtr->nextPtr  = NULL;

        /* insert the new Tx end node into FIFO */
        fifoPtr = (PRV_CPSS_TX_BUF_QUEUE_FIFO_STC*)evHndlPtr->extDataPtr;

        if (NULL == fifoPtr->tailPtr)
        {
            fifoPtr->tailPtr = txFreeDataPtr;
            fifoPtr->headPtr = txFreeDataPtr;

            /* notify the user process ,or        */
            /* call the application's CB function */
            /* only for first packet in fifo.     */
            prvCpssDrvEvReqNotify(evHndlPtr);
        }
        else
        {
            fifoPtr->tailPtr->nextPtr = txFreeDataPtr;
            fifoPtr->tailPtr = txFreeDataPtr;
        }

        while(freeDescNum > 0)
        {
            firstTxDescPtr->txDesc->word1 = 0x0;

            firstTxDescPtr = firstTxDescPtr->swNextDesc;
            freeDescNum--;
        }

        txDescListPtr->freeDescNum += descNum;
        txDescListPtr->next2Free = firstTxDescPtr;
        if(txDescListPtr->next2Free == txDescListPtr->next2Feed)
            break;

        /* update the desc ponter for next iteration in while loop */
        descPtr = firstTxDescPtr;
        tmpTxDesc.word1 = descPtr->txDesc->word1;
    }

    return GT_OK;
}

/*******************************************************************************
* internal_cpssDxChNetIfSdmaTxQueueEnable
*
* DESCRIPTION:
*       Enable/Disable the specified traffic class queue for TX
*       on all packet processors in the system. -- SDMA relate
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1_Diamond.
*
* INPUTS:
*        devNum    - device number
*        queue     - traffic class queue (APPLICABLE RANGES: 0..7)
*        enable    - GT_TRUE, enable queue
*                    GT_FALSE, disable queue
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - on bad parameters
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_BAD_STATE             - on CPU port is not SDMA
*
* COMMENTS:
*
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChNetIfSdmaTxQueueEnable
(
    IN  GT_U8                           devNum,
    IN  GT_U8                           queue,
    IN  GT_BOOL                         enable
)
{
    GT_U32  regAddr;/* The register address to write to.    */
    GT_U32  data;
    GT_U32  portGroupId;/* port group Id for multi-port-groups device support */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);

    /* Diamond Cut have not SDMA registers */

    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum, CPSS_CH1_DIAMOND_E);

    PRV_CPSS_DXCH_COS_CHECK_TC_MAC(queue);

    if(PRV_CPSS_PP_MAC(devNum)->cpuPortMode == CPSS_NET_CPU_PORT_MODE_SDMA_E &&
        !PRV_CPSS_DXCH_CHECK_SDMA_PACKETS_FROM_CPU_CORRUPT_WA_MAC(devNum))
    {
        /* we will use the 'First active port group' , to represent the whole device.
           that way we allow application to give SDMA memory to single port group instead
           of split it between all active port groups
        */
        portGroupId = PRV_CPSS_NETIF_SDMA_PORT_GROUP_ID_MAC(devNum);

        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->sdmaRegs.txQCmdReg;

        if (enable == GT_TRUE)
        {
            data = (1 << queue);
        }
        else
        {
            data = (1 << (8 + queue));
        }

        return prvCpssHwPpPortGroupWriteRegister (devNum, portGroupId,regAddr, data);
    }
    else if (PRV_CPSS_PP_MAC(devNum)->cpuPortMode == CPSS_NET_CPU_PORT_MODE_SDMA_E &&
             PRV_CPSS_DXCH_CHECK_SDMA_PACKETS_FROM_CPU_CORRUPT_WA_MAC(devNum))
    {
        return GT_OK;
    }

    CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_STATE, LOG_ERROR_NO_MSG);
}

/*******************************************************************************
* cpssDxChNetIfSdmaTxQueueEnable
*
* DESCRIPTION:
*       Enable/Disable the specified traffic class queue for TX
*       on all packet processors in the system. -- SDMA relate
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1_Diamond.
*
* INPUTS:
*        devNum    - device number
*        queue     - traffic class queue (APPLICABLE RANGES: 0..7)
*        enable    - GT_TRUE, enable queue
*                    GT_FALSE, disable queue
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - on bad parameters
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_BAD_STATE             - on CPU port is not SDMA
*
* COMMENTS:
*
*
*******************************************************************************/
GT_STATUS cpssDxChNetIfSdmaTxQueueEnable
(
    IN  GT_U8                           devNum,
    IN  GT_U8                           queue,
    IN  GT_BOOL                         enable
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChNetIfSdmaTxQueueEnable);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, queue, enable));

    rc = internal_cpssDxChNetIfSdmaTxQueueEnable(devNum, queue, enable);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, queue, enable));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChNetIfSdmaTxQueueEnableGet
*
* DESCRIPTION:
*       Get status of the specified traffic class queue for TX
*       packets from CPU. -- SDMA relate
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1_Diamond.
*
* INPUTS:
*        devNum    - device number
*        queue     - traffic class queue
*
* OUTPUTS:
*        enablePtr - GT_TRUE, enable queue
*                    GT_FALSE, disable queue
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - on bad parameters
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_BAD_STATE             - on CPU port is not SDMA
*       GT_BAD_PTR               - on NULL ptr
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChNetIfSdmaTxQueueEnableGet
(
    IN  GT_U8                           devNum,
    IN  GT_U8                           queue,
    OUT GT_BOOL                         *enablePtr
)
{
    GT_STATUS   rc;
    GT_U32      regAddr;/* The register address to write to.    */
    GT_U32      hwData, hwValue;
    GT_U32      portGroupId;/* port group Id for multi-port-groups device support */


    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    /* Diamond Cut have not SDMA registers */
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum, CPSS_CH1_DIAMOND_E);
    PRV_CPSS_DXCH_COS_CHECK_TC_MAC(queue);
    CPSS_NULL_PTR_CHECK_MAC(enablePtr);

    if(PRV_CPSS_PP_MAC(devNum)->cpuPortMode == CPSS_NET_CPU_PORT_MODE_SDMA_E &&
        !PRV_CPSS_DXCH_CHECK_SDMA_PACKETS_FROM_CPU_CORRUPT_WA_MAC(devNum))
    {
        /* we will use the 'First active port group' , to represent the whole device.
           that way we allow application to give SDMA memory to single port group instead
           of split it between all active port groups
        */
        portGroupId = PRV_CPSS_NETIF_SDMA_PORT_GROUP_ID_MAC(devNum);

        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->sdmaRegs.txQCmdReg;

        rc = prvCpssHwPpPortGroupReadRegister (devNum, portGroupId,regAddr, &hwValue);

        if(GT_OK != rc)
        {
            return rc;
        }

        hwData = hwValue & 0xFFFF;

        /* Need check only TxENQ bit to get Tx status.
          The TX SDMA is not active when TxENQ is 0. */
        if((hwData >> queue) & 1)
        {
            *enablePtr = GT_TRUE;
        }
        else
        {
            *enablePtr = GT_FALSE;
        }

        return rc;
    }
    else if (PRV_CPSS_PP_MAC(devNum)->cpuPortMode == CPSS_NET_CPU_PORT_MODE_SDMA_E &&
             PRV_CPSS_DXCH_CHECK_SDMA_PACKETS_FROM_CPU_CORRUPT_WA_MAC(devNum))
    {
        *enablePtr = GT_FALSE;
        return GT_OK;
    }

    CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_STATE, LOG_ERROR_NO_MSG);
}

/*******************************************************************************
* cpssDxChNetIfSdmaTxQueueEnableGet
*
* DESCRIPTION:
*       Get status of the specified traffic class queue for TX
*       packets from CPU. -- SDMA relate
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1_Diamond.
*
* INPUTS:
*        devNum    - device number
*        queue     - traffic class queue
*
* OUTPUTS:
*        enablePtr - GT_TRUE, enable queue
*                    GT_FALSE, disable queue
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - on bad parameters
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_BAD_STATE             - on CPU port is not SDMA
*       GT_BAD_PTR               - on NULL ptr
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChNetIfSdmaTxQueueEnableGet
(
    IN  GT_U8                           devNum,
    IN  GT_U8                           queue,
    OUT GT_BOOL                         *enablePtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChNetIfSdmaTxQueueEnableGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, queue, enablePtr));

    rc = internal_cpssDxChNetIfSdmaTxQueueEnableGet(devNum, queue, enablePtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, queue, enablePtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChNetIfTxBufferQueueGet
*
* DESCRIPTION:
*       This routine returns the caller the TxEnd parameters for a transmitted
*       packet.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       hndl        - Handle got from gtEventBind.
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
*       GT_BAD_PARAM             - one of the parameters value is wrong
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NO_MORE               - no more packets
*       GT_BAD_STATE             - the CPSS not handling the ISR for the device
*                                  so function must not be called
*       GT_NOT_INITIALIZED       - the library was not initialized
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_FAIL otherwise.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChNetIfTxBufferQueueGet
(
    IN  GT_UINTPTR          hndl,
    OUT GT_U8               *devPtr,
    OUT GT_PTR              *cookiePtr,
    OUT GT_U8               *queuePtr,
    OUT GT_STATUS           *statusPtr
)
{
    PRV_CPSS_DRV_EVENT_HNDL_STC     *evHndlPtr;   /* The event handle pointer */
    PRV_CPSS_TX_BUF_QUEUE_FIFO_STC  *fifoPtr;     /* The Handle Tx-End FIFO   */
    GT_U32                          intKey;       /* The interrupt key        */
    PRV_CPSS_SW_TX_FREE_DATA_STC    *txEndDataPtr;/* The Tx shadow data pointer*/

    CPSS_NULL_PTR_CHECK_MAC(devPtr);
    CPSS_NULL_PTR_CHECK_MAC(cookiePtr);
    CPSS_NULL_PTR_CHECK_MAC(queuePtr);
    CPSS_NULL_PTR_CHECK_MAC(statusPtr);

    evHndlPtr = (PRV_CPSS_DRV_EVENT_HNDL_STC*)hndl;

    /* validate event handle data */
    if (NULL == evHndlPtr || NULL == evHndlPtr->extDataPtr)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_FAIL, LOG_ERROR_NO_MSG);
    }

    fifoPtr = (PRV_CPSS_TX_BUF_QUEUE_FIFO_STC*)evHndlPtr->extDataPtr;

    if (NULL == fifoPtr->headPtr)
    {
        /* FIFO is empty */
        return /* it's not error for log */ GT_NO_MORE;
    }

    /* lock section from ISR preemption */
    intKey = 0;
    PRV_CPSS_LOCK_BY_MUTEX;
    cpssExtDrvSetIntLockUnlock(CPSS_OS_INTR_MODE_LOCK_E , (GT_32*)&intKey);

    txEndDataPtr = fifoPtr->headPtr;
    fifoPtr->headPtr  = txEndDataPtr->nextPtr;

    if (NULL == fifoPtr->headPtr)
    {
        /* last element in FIFO */
        fifoPtr->tailPtr = NULL;
    }

    /* extract data from FIFO element */
    *queuePtr   = txEndDataPtr->queueNum;
    *devPtr     = txEndDataPtr->devNum;
    *cookiePtr  = txEndDataPtr->userData;
    *statusPtr  = GT_OK;

    cpssExtDrvSetIntLockUnlock(CPSS_OS_INTR_MODE_UNLOCK_E , (GT_32*)&intKey);
    PRV_CPSS_UNLOCK_BY_MUTEX;

    if(!PRV_CPSS_DXCH_CHECK_SDMA_PACKETS_FROM_CPU_CORRUPT_WA_MAC(*devPtr))
    {
        PRV_CPSS_DXCH_SDMA_USED_CHECK_MAC(*devPtr);

        /*check that the DB of networkIf library was initialized*/
        PRV_CPSS_DXCH_NETIF_INITIALIZED_CHECK_MAC(*devPtr);

        if(PRV_CPSS_PP_MAC(*devPtr)->useIsr == GT_FALSE)
        {
            /* this check can be done only after we have the 'device number' ,
               and now we can check that :
               the CPSS not handling the ISR for the device so function must not
               be called */
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_STATE, LOG_ERROR_NO_MSG);
        }

        /* return the FIFO element buffer to pool */
        if (cpssBmPoolBufFree(PRV_CPSS_PP_MAC(*devPtr)->intCtrl.txDescList[*queuePtr].poolId,
                          txEndDataPtr) != GT_OK)
        {
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_FAIL, LOG_ERROR_NO_MSG);
        }
    }
    else
    {
        /* return the packet header buffer to pool */
        if (prvCpssGenNetIfMiiPoolFreeBuf(PRV_CPSS_GEN_NETIF_MII_TX_POOL_E,
                                            txEndDataPtr) != GT_OK)
        {
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_FAIL, LOG_ERROR_NO_MSG);
        }
    }

    return GT_OK;
}

/*******************************************************************************
* cpssDxChNetIfTxBufferQueueGet
*
* DESCRIPTION:
*       This routine returns the caller the TxEnd parameters for a transmitted
*       packet.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       hndl        - Handle got from gtEventBind.
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
*       GT_BAD_PARAM             - one of the parameters value is wrong
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NO_MORE               - no more packets
*       GT_BAD_STATE             - the CPSS not handling the ISR for the device
*                                  so function must not be called
*       GT_NOT_INITIALIZED       - the library was not initialized
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_FAIL otherwise.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChNetIfTxBufferQueueGet
(
    IN  GT_UINTPTR          hndl,
    OUT GT_U8               *devPtr,
    OUT GT_PTR              *cookiePtr,
    OUT GT_U8               *queuePtr,
    OUT GT_STATUS           *statusPtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChNetIfTxBufferQueueGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, hndl, devPtr, cookiePtr, queuePtr, statusPtr));

    rc = internal_cpssDxChNetIfTxBufferQueueGet(hndl, devPtr, cookiePtr, queuePtr, statusPtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, hndl, devPtr, cookiePtr, queuePtr, statusPtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChNetIfSdmaTxBufferQueueDescFreeAndCookieGet
*
* DESCRIPTION:
*       This routine frees all transmitted packets descriptors. In addition, all
*       user relevant data in Tx End FIFO.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1_Diamond.
*
* INPUTS:
*       devNum     - The device number the packet was transmitted from
*       txQueue    - The queue the packet was transmitted upon (APPLICABLE RANGES: 0..7)
*
* OUTPUTS:
*       cookiePtr   - (pointer to) the cookie attached to packet that was send
*                     from this queue
*
* RETURNS:
*       GT_OK - on success
*       GT_BAD_PARAM - on bad device or queue
*       GT_BAD_PTR - on NULL pointer
*       GT_NO_MORE - no more packet cookies to get
*       GT_ERROR - the Tx descriptor is corrupted
*       GT_NOT_INITIALIZED - the library was not initialized
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*  1. returns the caller the cookie attached to the frame that was sent from the
*     device on the specific queue.
*  2. put the descriptors of this sent packet back to the 'Free descriptors' list
*       --> SDMA relate
*  3. this function should be used only when the ISR of the CPSS is not in use,
*     meaning that the intVecNum (given in function cpssDxChHwPpPhase1Init) was
*    set to CPSS_PP_INTERRUPT_VECTOR_NOT_USED_CNS.
*
*   NOTE: the caller is responsible to synchronize the calls to 'Tx send' and
*   this function , since both deal with the 'Tx descriptors'.
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChNetIfSdmaTxBufferQueueDescFreeAndCookieGet
(
    IN GT_U8                devNum,
    IN GT_U8                txQueue,
    OUT GT_PTR             *cookiePtr
)
{
    PRV_CPSS_TX_DESC_LIST_STC       *txDescListPtr;
    /* Counts the number of descriptors already freed. */
    GT_U32                          freeDescNum;
    PRV_CPSS_SW_TX_DESC_STC         *descPtr;
    /* Points to the first sw desc. to be sent to the callback function.*/
    PRV_CPSS_SW_TX_DESC_STC         *firstTxDescPtr;
    /* Number of descriptors this packet occupies to be sent to the callback
       function.*/
    GT_U32                          descNum;
    PRV_CPSS_TX_DESC_STC            tmpTxDesc;

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    CPSS_NULL_PTR_CHECK_MAC(cookiePtr);
    PRV_CPSS_DXCH_SDMA_USED_CHECK_MAC(devNum);

    /* Diamond Cut have not SDMA registers */

    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum, CPSS_CH1_DIAMOND_E);


    if(!PRV_CPSS_DXCH_CHECK_SDMA_PACKETS_FROM_CPU_CORRUPT_WA_MAC(devNum))
    {
        PRV_CPSS_DXCH_NETIF_INITIALIZED_CHECK_MAC(devNum);

        if(txQueue >= 8)
        {
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
        }

        /* the Tx descriptors for this Queue */
        txDescListPtr  = &(PRV_CPSS_PP_MAC(devNum)->intCtrl.txDescList[txQueue]);

        descPtr = txDescListPtr->next2Free;

        tmpTxDesc.word1 = descPtr->txDesc->word1;

        /* No more descriptors to release.  */
        if(TX_DESC_GET_OWN_BIT_MAC(devNum,&tmpTxDesc) == TX_DESC_DMA_OWN)
        {
            return /* it's not error for log */ GT_NO_MORE;
        }

        if(TX_DESC_GET_FIRST_BIT_MAC(devNum,&tmpTxDesc) == 0)
        {
            /*should not happen*/
            CPSS_LOG_ERROR_AND_RETURN_MAC(((GT_STATUS)GT_ERROR), LOG_ERROR_NO_MSG);
        }

        descNum     = 1;
        firstTxDescPtr = descPtr;

        /* Get the packet's descriptors.        */
        while(TX_DESC_GET_LAST_BIT_MAC(devNum,&tmpTxDesc) == 0x0)
        {
            if(TX_DESC_GET_OWN_BIT_MAC(devNum,&tmpTxDesc) == TX_DESC_DMA_OWN)
            {
                /*should not happen*/
                return /* it's not error for log */ GT_NO_MORE;
            }
            descPtr = descPtr->swNextDesc;
            tmpTxDesc.word1 = descPtr->txDesc->word1;
            descNum++;
        }

        if(TX_DESC_GET_OWN_BIT_MAC(devNum,&tmpTxDesc) == TX_DESC_DMA_OWN)
        {
            /*should not happen*/
            return /* it's not error for log */ GT_NO_MORE;
        }

        freeDescNum = descNum;

        /* we have a cookie to return to caller */
        *cookiePtr = firstTxDescPtr->userData;

        while(freeDescNum > 0)
        {
            firstTxDescPtr->txDesc->word1 = 0x0;

            firstTxDescPtr = firstTxDescPtr->swNextDesc;
            freeDescNum--;
        }

        txDescListPtr->freeDescNum += descNum;
        txDescListPtr->next2Free = firstTxDescPtr;
    }
    else
    {
        /* MII doesn't use descriptors */
        *cookiePtr = 0;
    }

    return GT_OK;
}

/*******************************************************************************
* cpssDxChNetIfSdmaTxBufferQueueDescFreeAndCookieGet
*
* DESCRIPTION:
*       This routine frees all transmitted packets descriptors. In addition, all
*       user relevant data in Tx End FIFO.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1_Diamond.
*
* INPUTS:
*       devNum     - The device number the packet was transmitted from
*       txQueue    - The queue the packet was transmitted upon (APPLICABLE RANGES: 0..7)
*
* OUTPUTS:
*       cookiePtr   - (pointer to) the cookie attached to packet that was send
*                     from this queue
*
* RETURNS:
*       GT_OK - on success
*       GT_BAD_PARAM - on bad device or queue
*       GT_BAD_PTR - on NULL pointer
*       GT_NO_MORE - no more packet cookies to get
*       GT_ERROR - the Tx descriptor is corrupted
*       GT_NOT_INITIALIZED - the library was not initialized
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*  1. returns the caller the cookie attached to the frame that was sent from the
*     device on the specific queue.
*  2. put the descriptors of this sent packet back to the 'Free descriptors' list
*       --> SDMA relate
*  3. this function should be used only when the ISR of the CPSS is not in use,
*     meaning that the intVecNum (given in function cpssDxChHwPpPhase1Init) was
*    set to CPSS_PP_INTERRUPT_VECTOR_NOT_USED_CNS.
*
*   NOTE: the caller is responsible to synchronize the calls to 'Tx send' and
*   this function , since both deal with the 'Tx descriptors'.
*
*******************************************************************************/
GT_STATUS cpssDxChNetIfSdmaTxBufferQueueDescFreeAndCookieGet
(
    IN GT_U8                devNum,
    IN GT_U8                txQueue,
    OUT GT_PTR             *cookiePtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChNetIfSdmaTxBufferQueueDescFreeAndCookieGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, txQueue, cookiePtr));

    rc = internal_cpssDxChNetIfSdmaTxBufferQueueDescFreeAndCookieGet(devNum, txQueue, cookiePtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, txQueue, cookiePtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChNetIfSdmaTxFreeDescripotrsNumberGet
*
* DESCRIPTION:
*       This function return the number of free Tx descriptors for given
*       device and txQueue
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1_Diamond.
*
* INPUTS:
*       devNum  - Device number.
*       txQueue - Tx queue number.
*
* OUTPUTS:
*       numberOfFreeTxDescriptorsPtr - pointer to number of free
*                                         descriptors for the given queue.
*
* RETURNS:
*       GT_OK - on success
*       GT_BAD_PARAM - on bad device or queue
*       GT_BAD_PTR - on NULL pointer
*       GT_NOT_INITIALIZED - the library was not initialized
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChNetIfSdmaTxFreeDescripotrsNumberGet
(
    IN GT_U8                devNum,
    IN GT_U8                txQueue,
    OUT GT_U32             *numberOfFreeTxDescriptorsPtr
)
{
    PRV_CPSS_TX_DESC_LIST_STC       *txDescListPtr;

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    CPSS_NULL_PTR_CHECK_MAC(numberOfFreeTxDescriptorsPtr);
    PRV_CPSS_DXCH_SDMA_USED_CHECK_MAC(devNum);

    /* Diamond Cut have not SDMA registers */

    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum, CPSS_CH1_DIAMOND_E);


    PRV_CPSS_DXCH_NETIF_INITIALIZED_CHECK_MAC(devNum);

    if(txQueue >= 8)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    /* the Tx descriptors for this Queue */
    txDescListPtr  = &(PRV_CPSS_PP_MAC(devNum)->intCtrl.txDescList[txQueue]);

    *numberOfFreeTxDescriptorsPtr = txDescListPtr->freeDescNum;

    return GT_OK;
}

/*******************************************************************************
* cpssDxChNetIfSdmaTxFreeDescripotrsNumberGet
*
* DESCRIPTION:
*       This function return the number of free Tx descriptors for given
*       device and txQueue
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1_Diamond.
*
* INPUTS:
*       devNum  - Device number.
*       txQueue - Tx queue number.
*
* OUTPUTS:
*       numberOfFreeTxDescriptorsPtr - pointer to number of free
*                                         descriptors for the given queue.
*
* RETURNS:
*       GT_OK - on success
*       GT_BAD_PARAM - on bad device or queue
*       GT_BAD_PTR - on NULL pointer
*       GT_NOT_INITIALIZED - the library was not initialized
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChNetIfSdmaTxFreeDescripotrsNumberGet
(
    IN GT_U8                devNum,
    IN GT_U8                txQueue,
    OUT GT_U32             *numberOfFreeTxDescriptorsPtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChNetIfSdmaTxFreeDescripotrsNumberGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, txQueue, numberOfFreeTxDescriptorsPtr));

    rc = internal_cpssDxChNetIfSdmaTxFreeDescripotrsNumberGet(devNum, txQueue, numberOfFreeTxDescriptorsPtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, txQueue, numberOfFreeTxDescriptorsPtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChNetIfSdmaTxGeneratorPacketAdd
*
* DESCRIPTION:
*       This function adds a new packet to Tx SDMA working as Packet Generator.
*       This packet will be transmitted by the selected Tx SDMA with previous
*       packets already transmitted by this Packet generator.
*
* APPLICABLE DEVICES:
*        xCat3; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum          - Device number.
*       portGroupsBmp   - bitmap of Port Groups.
*                       NOTEs:
*                       1. for non multi-port groups device this parameter is
*                          IGNORED.
*                       2. for multi-port groups device :
*                       bitmap must be set with at least one bit representing
*                       valid port group(s). If a bit of non valid port group
*                       is set then function returns GT_BAD_PARAM.
*                       value CPSS_PORT_GROUP_UNAWARE_MODE_CNS is supported.
*       packetParamsPtr   - (pointer to) The internal packet params to be set
*                         into the packet descriptors.
*       packetDataPtr     - (pointer to) packet data.
*       packetDataLength  - packet data length. Buffer size configured during
*                           initialization phase must be sufficient for packet
*                           length and the DSA tag that will be added to it.
*
* OUTPUTS:
*       packetIdPtr       - (pointer to) packet identification number, used by
*                         other Packet Generator functions which require access
*                         to this packet.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_NO_RESOURCE           - no available buffer or descriptor.
*       GT_BAD_PARAM             - Tx SDMA is not in Generator mode, bad DSA
*                                  params or the data buffer is longer than
*                                  size configured.
*       GT_BAD_VALUE             - addition of packet will violate the required
*                                  rate configuration.
*       GT_BAD_PTR               - one of the parameters is NULL pointer.
*       GT_NOT_INITIALIZED       - the library was not initialized.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device.
*
* COMMENTS:
*    1. Packet can be added while Tx SDMA is enabled.
*    2. The packet buffer supplied by the application is copied to internal
*       prealloocated generator buffer, therefore application can free or reuse
*       that buffer as soon as function returns.
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChNetIfSdmaTxGeneratorPacketAdd
(
    IN  GT_U8                       devNum,
    IN  GT_PORT_GROUPS_BMP          portGroupsBmp,
    IN  CPSS_DXCH_NET_TX_PARAMS_STC *packetParamsPtr,
    IN  GT_U8                       *packetDataPtr,
    IN  GT_U32                      packetDataLength,
    OUT GT_U32                      *packetIdPtr
)
{
    GT_STATUS   rc;
    GT_U32      portGroupId;    /* port group id */
    GT_U8       txQueue;        /* Tx queue */
    PRV_CPSS_TX_DESC_LIST_STC *txDescList;   /* Tx desc. list */
    PRV_CPSS_TX_DESC_STC    *tmpTxDesc;
    GT_UINTPTR  physicalMemoryPointer;

    CPSS_DXCH_NET_TX_GENERATOR_RATE_MODE_ENT rateMode; /* Generator rate mode */
    GT_U64                          rateValue;  /* Generator rate value */
    GT_U64                          actualRateValue;
                                            /* the real Generator rate value */
    GT_U32                          clkTicks; /* the HW configured clock ticks */
                                              /* to support the rate value */
    GT_U32          placeInQueue; /* the place (numerical) in the queue list */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
        CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E
        | CPSS_XCAT_E | CPSS_LION_E | CPSS_XCAT2_E | CPSS_LION2_E);
    PRV_CPSS_DXCH_NETIF_INITIALIZED_CHECK_MAC(devNum);
    PRV_CPSS_MULTI_PORT_GROUPS_BMP_CHECK_MAC(devNum,portGroupsBmp);

    CPSS_NULL_PTR_CHECK_MAC(packetParamsPtr);
    CPSS_NULL_PTR_CHECK_MAC(packetDataPtr);
    CPSS_NULL_PTR_CHECK_MAC(packetIdPtr);

    cpssOsBzero((GT_VOID*)&actualRateValue, sizeof(actualRateValue));
    cpssOsBzero((GT_VOID*)&rateValue, sizeof(rateValue));


    txQueue = packetParamsPtr->sdmaInfo.txQueue;
    PRV_CPSS_DXCH_DEV_TX_SDMA_QUEUE_NUM_CHECK_MAC(txQueue);

    portGroupId = PRV_CPSS_NETIF_SDMA_PORT_GROUP_ID_MAC(devNum);
    PRV_CPSS_DXCH_DEV_TX_SDMA_GENERATOR_CHECK_MAC(devNum, portGroupId, txQueue);

    txDescList  = &(PRV_CPSS_PP_MAC(devNum)->intCtrl.txDescList[txQueue]);

    /* Check for available descriptor */
    if( 0 == txDescList->freeDescNum )
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NO_RESOURCE, LOG_ERROR_NO_MSG);
    }

    /* Get current rate configuration */
    rc = cpssDxChNetIfSdmaTxGeneratorRateGet(devNum,
                                             portGroupsBmp,
                                             txQueue,
                                             &rateMode,
                                             &rateValue,
                                             &actualRateValue);
    if( GT_OK != rc )
    {
        return rc;
    }

    /* To avoid warning */
    clkTicks = 0;

    /* Check availability of already configured rate can be supported due to */
    /* a new packet addition. */
    rc = prvCpssDxChNetIfSdmaTxGeneratorRateCalc(rateMode,
                                                 rateValue,
                                                 txDescList->maxDescNum -
                                                    txDescList->freeDescNum + 1,
                                                 &clkTicks,
                                                 &actualRateValue);
    if( GT_OK != rc )
    {
        return rc;
    }

    tmpTxDesc = txDescList->freeDescPtr;

    /* Configure and fill new descriptor & buffer */
    rc = prvCpssDxChNetIfSdmaTxGeneratorPacketToBufferSet(devNum,
                                                          packetParamsPtr,
                                                          packetDataPtr,
                                                          packetDataLength,
                                                          tmpTxDesc,
                                                    txDescList->actualBuffSize);
    if( GT_OK != rc )
    {
        return rc;
    }

    /* Point to the first descriptor in the chain as the next one. */
    rc = cpssOsVirt2Phy((GT_UINTPTR)(txDescList->firstDescPtr),
                        /*OUT*/&physicalMemoryPointer);
    if (rc != GT_OK)
    {
        return rc;
    }

    #if __WORDSIZE == 64    /* phyAddr must fit in 32 bit */
        if (0 != (physicalMemoryPointer & 0xffffffff00000000L))
        {
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_OUT_OF_RANGE, LOG_ERROR_NO_MSG);
        }
    #endif

    tmpTxDesc->nextDescPointer = CPSS_32BIT_LE((GT_U32)physicalMemoryPointer);

    /* Update HW rate configuration  */
    rc = prvCpssDxChNetIfSdmaTxGeneratorHwRateSet(devNum,
                                                  portGroupId,
                                                  txQueue,
                                                  rateMode,
                                                  clkTicks);

    if( GT_OK != rc )
    {
        return rc;
    }

    if( txDescList->freeDescNum == txDescList->maxDescNum )
    {
        /* In case this is the first packet added to the queue, enable it */
        /* if previously explicitly enabled by application. */
        if( GT_TRUE == txDescList->userQueueEnabled )
        {
            /* Set Tx SDMA Current Descriptor Pointer to first descriptor in */
            /* queue chain */
            rc = cpssOsVirt2Phy((GT_UINTPTR)(txDescList->firstDescPtr),
                                /*OUT*/&physicalMemoryPointer);
            if (rc != GT_OK)
            {
                return rc;
            }

            #if __WORDSIZE == 64    /* phyAddr must fit in 32 bit */
                if (0 != (physicalMemoryPointer & 0xffffffff00000000L))
                {
                    CPSS_LOG_ERROR_AND_RETURN_MAC(GT_OUT_OF_RANGE, LOG_ERROR_NO_MSG);
                }
            #endif

            rc = prvCpssHwPpPortGroupWriteRegister(
                devNum,
                portGroupId,
                PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
                     sdmaRegs.txDmaCdp[txQueue],(GT_U32)physicalMemoryPointer);

            if(GT_OK != rc)
            {
                return rc;
            }

            rc = cpssDxChNetIfSdmaTxQueueEnable(devNum, txQueue, GT_TRUE);

            if ( GT_OK != rc )
            {
                return rc;
            }
        }
    }
    else
    {
        /* Get physical address of new free descriptor, will be pointet */
        /* by next pointer field of the now last used descriptor. */
        rc = cpssOsVirt2Phy((GT_UINTPTR)tmpTxDesc, /*OUT*/&physicalMemoryPointer);
        if (rc != GT_OK)
        {
            return rc;
        }

        #if __WORDSIZE == 64    /* phyAddr must fit in 32 bit */
            if (0 != (physicalMemoryPointer & 0xffffffff00000000L))
            {
                CPSS_LOG_ERROR_AND_RETURN_MAC(GT_OUT_OF_RANGE, LOG_ERROR_NO_MSG);
            }
        #endif

        /* Get last used descriptor and point with its next to the new one*/
        tmpTxDesc = (PRV_CPSS_TX_DESC_STC*)((GT_U8*)tmpTxDesc -
                    (sizeof(PRV_CPSS_TX_DESC_STC) + txDescList->actualBuffSize));

        tmpTxDesc->nextDescPointer =
                            CPSS_32BIT_LE((GT_U32)physicalMemoryPointer);
    }

    /* Advanced the next free descriptor */
    txDescList->freeDescPtr = (PRV_CPSS_TX_DESC_STC*)
                            ((GT_U8*)(txDescList->freeDescPtr) +
                    (sizeof(PRV_CPSS_TX_DESC_STC) + txDescList->actualBuffSize));

    placeInQueue = txDescList->maxDescNum - txDescList->freeDescNum;

    /* Decrease free descriptors number */
    txDescList->freeDescNum -= 1;

    /* Get next free packet ID */
    *packetIdPtr =
        PRV_CPSS_PP_MAC(devNum)->intCtrl.txGeneratorPacketIdDb.firstFree;

    /* Save queue number - bits [2:0] & place in queue - bits [31:3], */
    /* in packet ID DB */
    PRV_CPSS_PP_MAC(devNum)->intCtrl.txGeneratorPacketIdDb.cookie[*packetIdPtr] =
                              txQueue + (placeInQueue << 3);

    /* Set next free packet ID */
    PRV_CPSS_PP_MAC(devNum)->intCtrl.txGeneratorPacketIdDb.firstFree =
        PRV_CPSS_PP_MAC(devNum)->intCtrl.txGeneratorPacketIdDb.
                                                    freeLinkedList[*packetIdPtr];

    /* Update queue "reverse" packet ID DB, later used when moving last */
    /* packet during removal operation */
    txDescList->revPacketIdDb[placeInQueue] = *packetIdPtr;

    return rc;
}

/*******************************************************************************
* cpssDxChNetIfSdmaTxGeneratorPacketAdd
*
* DESCRIPTION:
*       This function adds a new packet to Tx SDMA working as Packet Generator.
*       This packet will be transmitted by the selected Tx SDMA with previous
*       packets already transmitted by this Packet generator.
*
* APPLICABLE DEVICES:
*        xCat3; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum          - Device number.
*       portGroupsBmp   - bitmap of Port Groups.
*                       NOTEs:
*                       1. for non multi-port groups device this parameter is
*                          IGNORED.
*                       2. for multi-port groups device :
*                       bitmap must be set with at least one bit representing
*                       valid port group(s). If a bit of non valid port group
*                       is set then function returns GT_BAD_PARAM.
*                       value CPSS_PORT_GROUP_UNAWARE_MODE_CNS is supported.
*       packetParamsPtr   - (pointer to) The internal packet params to be set
*                         into the packet descriptors.
*       packetDataPtr     - (pointer to) packet data.
*       packetDataLength  - packet data length. Buffer size configured during
*                           initialization phase must be sufficient for packet
*                           length and the DSA tag that will be added to it.
*
* OUTPUTS:
*       packetIdPtr       - (pointer to) packet identification number, used by
*                         other Packet Generator functions which require access
*                         to this packet.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_NO_RESOURCE           - no available buffer or descriptor.
*       GT_BAD_PARAM             - Tx SDMA is not in Generator mode, bad DSA
*                                  params or the data buffer is longer than
*                                  size configured.
*       GT_BAD_VALUE             - addition of packet will violate the required
*                                  rate configuration.
*       GT_BAD_PTR               - one of the parameters is NULL pointer.
*       GT_NOT_INITIALIZED       - the library was not initialized.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device.
*
* COMMENTS:
*    1. Packet can be added while Tx SDMA is enabled.
*    2. The packet buffer supplied by the application is copied to internal
*       prealloocated generator buffer, therefore application can free or reuse
*       that buffer as soon as function returns.
*
*******************************************************************************/
GT_STATUS cpssDxChNetIfSdmaTxGeneratorPacketAdd
(
    IN  GT_U8                       devNum,
    IN  GT_PORT_GROUPS_BMP          portGroupsBmp,
    IN  CPSS_DXCH_NET_TX_PARAMS_STC *packetParamsPtr,
    IN  GT_U8                       *packetDataPtr,
    IN  GT_U32                      packetDataLength,
    OUT GT_U32                      *packetIdPtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChNetIfSdmaTxGeneratorPacketAdd);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, portGroupsBmp, packetParamsPtr, packetDataPtr, packetDataLength, packetIdPtr));

    rc = internal_cpssDxChNetIfSdmaTxGeneratorPacketAdd(devNum, portGroupsBmp, packetParamsPtr, packetDataPtr, packetDataLength, packetIdPtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, portGroupsBmp, packetParamsPtr, packetDataPtr, packetDataLength, packetIdPtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChNetIfSdmaTxGeneratorPacketUpdate
*
* DESCRIPTION:
*       This function updates already transmitted packet content and parameters.
*
* APPLICABLE DEVICES:
*        xCat3; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum          - Device number.
*       portGroupsBmp   - bitmap of Port Groups.
*                       NOTEs:
*                       1. for non multi-port groups device this parameter is
*                          IGNORED.
*                       2. for multi-port groups device :
*                       bitmap must be set with at least one bit representing
*                       valid port group(s). If a bit of non valid port group
*                       is set then function returns GT_BAD_PARAM.
*                       value CPSS_PORT_GROUP_UNAWARE_MODE_CNS is supported.
*       packetId          - packet identification number assign during packet
*                           addition to the Generator.
*       packetParamsPtr   - (pointer to) The internal packet params to be set
*                           into the packet descriptors.
*       packetDataPtr     - (pointer to) packet data.
*       packetDataLength  - packet data length. Buffer size configured during
*                           initialization phase must be sufficient for packet
*                           length and the DSA tag that will be added to it.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - Tx SDMA is not in Generator mode, packet is
*                                  not transmitted on this Tx SDMA, bad DSA
*                                  params or the data buffer is longer than
*                                  size configured.
*       GT_BAD_PTR               - one of the parameters is NULL pointer.
*       GT_NOT_INITIALIZED       - the library was not initialized.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device.
*
* COMMENTS:
*    1. Packet can be updated while Tx SDMA is enabled.
*    2. The packet buffer supplied by the application is copied to internal
*       prealloocated generator buffer, therefore application can free or reuse
*       that buffer as soon as function returns.
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChNetIfSdmaTxGeneratorPacketUpdate
(
    IN  GT_U8                       devNum,
    IN  GT_PORT_GROUPS_BMP          portGroupsBmp,
    IN  GT_U32                      packetId,
    IN  CPSS_DXCH_NET_TX_PARAMS_STC *packetParamsPtr,
    IN  GT_U8                       *packetDataPtr,
    IN  GT_U32                      packetDataLength

)
{
    GT_STATUS   rc;
    GT_U32      portGroupId;    /* port group id */
    GT_U8       txQueue;        /* Tx queue */
    PRV_CPSS_TX_DESC_LIST_STC txDescList;  /* Tx desc. list */
    PRV_CPSS_TX_DESC_STC    *currTxDesc;    /* current updated descriptor */
    PRV_CPSS_TX_DESC_STC    *prevTxDesc;    /* descriptor previous to the */
                                            /* updated one */
    GT_UINTPTR  physicalMemoryPointer;

    GT_U32        placeInQueue; /* the place (numerical) in the queue list */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
        CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E
        | CPSS_XCAT_E | CPSS_LION_E | CPSS_XCAT2_E | CPSS_LION2_E);
    PRV_CPSS_DXCH_NETIF_INITIALIZED_CHECK_MAC(devNum);
    PRV_CPSS_MULTI_PORT_GROUPS_BMP_CHECK_MAC(devNum,portGroupsBmp);

    CPSS_NULL_PTR_CHECK_MAC(packetParamsPtr);
    CPSS_NULL_PTR_CHECK_MAC(packetDataPtr);

    txQueue = packetParamsPtr->sdmaInfo.txQueue;
    PRV_CPSS_DXCH_DEV_TX_SDMA_QUEUE_NUM_CHECK_MAC(txQueue);

    portGroupId = PRV_CPSS_NETIF_SDMA_PORT_GROUP_ID_MAC(devNum);
    PRV_CPSS_DXCH_DEV_TX_SDMA_GENERATOR_CHECK_MAC(devNum, portGroupId, txQueue);

    txDescList  = PRV_CPSS_PP_MAC(devNum)->intCtrl.txDescList[txQueue];

    if (packetId >= PRV_CPSS_PP_MAC(devNum)->intCtrl.txGeneratorPacketIdDb.generatorsTotalDesc)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    /* Check the packet for update belongs to the txQueue */
    if( (PRV_CPSS_PP_MAC(devNum)->
           intCtrl.txGeneratorPacketIdDb.cookie[packetId] & 0x7) != txQueue )
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    placeInQueue = ((PRV_CPSS_PP_MAC(devNum)->
            intCtrl.txGeneratorPacketIdDb.cookie[packetId] >> 3) & 0x3FFFFFF);

    /* Check the packet within queue used packet range */
    if( placeInQueue >= (txDescList.maxDescNum - txDescList.freeDescNum) )
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    currTxDesc = (PRV_CPSS_TX_DESC_STC*)((GT_U8*)(txDescList.firstDescPtr) +
                  (placeInQueue *
                  (sizeof(PRV_CPSS_TX_DESC_STC) + txDescList.actualBuffSize)));

    if( currTxDesc == txDescList.firstDescPtr )
    {
        /* if the updated descriptor is the first, the previous is the last */
        prevTxDesc = (PRV_CPSS_TX_DESC_STC*)((GT_U8*)(txDescList.freeDescPtr) -
                   (sizeof(PRV_CPSS_TX_DESC_STC) + txDescList.actualBuffSize));
    }
    else
    {
        prevTxDesc = (PRV_CPSS_TX_DESC_STC*)((GT_U8*)currTxDesc -
                   (sizeof(PRV_CPSS_TX_DESC_STC) + txDescList.actualBuffSize));
    }

    /* Copy descriptor to be updated to the scratch pad descriptor */
    txDescList.scratchPadPtr->word1 = currTxDesc->word1;
    txDescList.scratchPadPtr->word2 = currTxDesc->word2;
    /* Point next from the scratch pad to the next of the updated descriptor */
    txDescList.scratchPadPtr->nextDescPointer = currTxDesc->nextDescPointer;
    /* Copy buffer data */
    cpssOsMemCpy( (GT_U8*)txDescList.scratchPadPtr + sizeof(PRV_CPSS_TX_DESC_STC),
                  (GT_U8*)currTxDesc + sizeof(PRV_CPSS_TX_DESC_STC),
                  TX_DESC_GET_BYTE_CNT_MAC(currTxDesc) );

    /* Physical address of the scratch pad */
    rc = cpssOsVirt2Phy((GT_UINTPTR)(txDescList.scratchPadPtr),
                        /*OUT*/&physicalMemoryPointer);
    if (rc != GT_OK)
    {
        return rc;
    }

    #if __WORDSIZE == 64    /* phyAddr must fit in 32 bit */
        if (0 != (physicalMemoryPointer & 0xffffffff00000000L))
        {
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_OUT_OF_RANGE, LOG_ERROR_NO_MSG);
        }
    #endif

    /* Point with previous descriptor to the scratch pad */
    prevTxDesc->nextDescPointer = CPSS_32BIT_LE((GT_U32)physicalMemoryPointer);


    /* Wait 3 packet generator intervals */
    rc = prvCpssDxChNetIfSdmaTxGenerator3PacketIntervalsWait(devNum,
                                                             portGroupId,
                                                             txQueue);
    if( GT_OK != rc )
    {
        return rc;
    }

    /* Configure and fill new descriptor & buffer */
    rc = prvCpssDxChNetIfSdmaTxGeneratorPacketToBufferSet(
                                                    devNum,
                                                    packetParamsPtr,
                                                    packetDataPtr,
                                                    packetDataLength,
                                                    currTxDesc,
                                                    txDescList.actualBuffSize);
    if( GT_OK != rc )
    {
        return rc;
    }

    /* Physical address of the current descriptor */
    rc = cpssOsVirt2Phy((GT_UINTPTR)(currTxDesc),
                        /*OUT*/&physicalMemoryPointer);
    if (rc != GT_OK)
    {
        return rc;
    }

    #if __WORDSIZE == 64    /* phyAddr must fit in 32 bit */
        if (0 != (physicalMemoryPointer & 0xffffffff00000000L))
        {
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_OUT_OF_RANGE, LOG_ERROR_NO_MSG);
        }
    #endif

    /* Move previous next pointer back to current */
    prevTxDesc->nextDescPointer = CPSS_32BIT_LE((GT_U32)physicalMemoryPointer);

    return rc;
}

/*******************************************************************************
* cpssDxChNetIfSdmaTxGeneratorPacketUpdate
*
* DESCRIPTION:
*       This function updates already transmitted packet content and parameters.
*
* APPLICABLE DEVICES:
*        xCat3; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum          - Device number.
*       portGroupsBmp   - bitmap of Port Groups.
*                       NOTEs:
*                       1. for non multi-port groups device this parameter is
*                          IGNORED.
*                       2. for multi-port groups device :
*                       bitmap must be set with at least one bit representing
*                       valid port group(s). If a bit of non valid port group
*                       is set then function returns GT_BAD_PARAM.
*                       value CPSS_PORT_GROUP_UNAWARE_MODE_CNS is supported.
*       packetId          - packet identification number assign during packet
*                           addition to the Generator.
*       packetParamsPtr   - (pointer to) The internal packet params to be set
*                           into the packet descriptors.
*       packetDataPtr     - (pointer to) packet data.
*       packetDataLength  - packet data length. Buffer size configured during
*                           initialization phase must be sufficient for packet
*                           length and the DSA tag that will be added to it.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - Tx SDMA is not in Generator mode, packet is
*                                  not transmitted on this Tx SDMA, bad DSA
*                                  params or the data buffer is longer than
*                                  size configured.
*       GT_BAD_PTR               - one of the parameters is NULL pointer.
*       GT_NOT_INITIALIZED       - the library was not initialized.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device.
*
* COMMENTS:
*    1. Packet can be updated while Tx SDMA is enabled.
*    2. The packet buffer supplied by the application is copied to internal
*       prealloocated generator buffer, therefore application can free or reuse
*       that buffer as soon as function returns.
*
*******************************************************************************/
GT_STATUS cpssDxChNetIfSdmaTxGeneratorPacketUpdate
(
    IN  GT_U8                       devNum,
    IN  GT_PORT_GROUPS_BMP          portGroupsBmp,
    IN  GT_U32                      packetId,
    IN  CPSS_DXCH_NET_TX_PARAMS_STC *packetParamsPtr,
    IN  GT_U8                       *packetDataPtr,
    IN  GT_U32                      packetDataLength

)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChNetIfSdmaTxGeneratorPacketUpdate);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, portGroupsBmp, packetId, packetParamsPtr, packetDataPtr, packetDataLength));

    rc = internal_cpssDxChNetIfSdmaTxGeneratorPacketUpdate(devNum, portGroupsBmp, packetId, packetParamsPtr, packetDataPtr, packetDataLength);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, portGroupsBmp, packetId, packetParamsPtr, packetDataPtr, packetDataLength));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChNetIfSdmaTxGeneratorPacketRemove
*
* DESCRIPTION:
*       This function removes packet from Tx SDMA working as a Packet Generator.
*
* APPLICABLE DEVICES:
*        xCat3; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum          - Device number.
*       portGroupsBmp   - bitmap of Port Groups.
*                       NOTEs:
*                       1. for non multi-port groups device this parameter is
*                          IGNORED.
*                       2. for multi-port groups device :
*                       bitmap must be set with at least one bit representing
*                       valid port group(s). If a bit of non valid port group
*                       is set then function returns GT_BAD_PARAM.
*                       value CPSS_PORT_GROUP_UNAWARE_MODE_CNS is supported.
*       txQueue         - The Tx SDMA queue to remove the packet from.
*       packetId          - packet identification number assign during packet
*                         addition to the Generator.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - Tx SDMA is not in Generator mode or packet
*                                  is not transmitted on this Tx SDMA.
*       GT_BAD_VALUE             - removal of packet will violate the required
*                                  rate configuration.
*       GT_NOT_INITIALIZED       - the library was not initialized.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device.
*
* COMMENTS:
*    Packet can be removed while Tx SDMA is enabled.
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChNetIfSdmaTxGeneratorPacketRemove
(
    IN  GT_U8               devNum,
    IN  GT_PORT_GROUPS_BMP  portGroupsBmp,
    IN  GT_U8               txQueue,
    IN  GT_U32              packetId
)
{
    GT_STATUS   rc;
    GT_U32      portGroupId;    /* port group id */
    PRV_CPSS_TX_DESC_LIST_STC *txDescList;  /* Tx desc. list */
    PRV_CPSS_TX_DESC_STC    *currTxDesc;    /* current descriptor of packet */
                                            /* remove */
    PRV_CPSS_TX_DESC_STC    *prevTxDesc;    /* descriptor previous to the */
                                            /* to removed one */
    PRV_CPSS_TX_DESC_STC    *lastTxDesc;    /* the last descriptor being used */
                                            /* in this queue */
    GT_U32        placeInQueue; /* the place (numerical) in the queue list */
    GT_BOOL                 enable;         /* queue enabling status */
    GT_UINTPTR  physicalMemoryPointer;
    GT_U32      movedPacketId; /* the packet ID of the moved buffer - */
                   /* from chain last place to the place of the removed one. */
    CPSS_DXCH_NET_TX_GENERATOR_RATE_MODE_ENT rateMode; /* Generator rate mode */
    GT_U64                          rateValue;  /* Generator rate value */
    GT_U64                          actualRateValue;
                                            /* the real Generator rate value */
    GT_U32                          clkTicks; /* the HW configured clock ticks */
                                              /* to support the rate value */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
        CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E
        | CPSS_XCAT_E | CPSS_LION_E | CPSS_XCAT2_E | CPSS_LION2_E);
    PRV_CPSS_DXCH_NETIF_INITIALIZED_CHECK_MAC(devNum);
    PRV_CPSS_MULTI_PORT_GROUPS_BMP_CHECK_MAC(devNum,portGroupsBmp);

    PRV_CPSS_DXCH_DEV_TX_SDMA_QUEUE_NUM_CHECK_MAC(txQueue);

    portGroupId = PRV_CPSS_NETIF_SDMA_PORT_GROUP_ID_MAC(devNum);
    PRV_CPSS_DXCH_DEV_TX_SDMA_GENERATOR_CHECK_MAC(devNum,portGroupId,txQueue);

    cpssOsBzero((GT_VOID*)&actualRateValue, sizeof(actualRateValue));
    cpssOsBzero((GT_VOID*)&rateValue, sizeof(rateValue));


    txDescList  = &(PRV_CPSS_PP_MAC(devNum)->intCtrl.txDescList[txQueue]);

    if (packetId >= PRV_CPSS_PP_MAC(devNum)->intCtrl.txGeneratorPacketIdDb.generatorsTotalDesc)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }
    /* Check the packet for update belongs to the txQueue */
    if( (PRV_CPSS_PP_MAC(devNum)->
           intCtrl.txGeneratorPacketIdDb.cookie[packetId] & 0x7) != txQueue )
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    placeInQueue = ((PRV_CPSS_PP_MAC(devNum)->
            intCtrl.txGeneratorPacketIdDb.cookie[packetId] >> 3) & 0x3FFFFFF);

    /* Check the packet within queue used packet range */
    if( placeInQueue >= (txDescList->maxDescNum - txDescList->freeDescNum) )
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    currTxDesc = (PRV_CPSS_TX_DESC_STC*)((GT_U8*)(txDescList->firstDescPtr) +
                  (placeInQueue *
                  (sizeof(PRV_CPSS_TX_DESC_STC) + txDescList->actualBuffSize)));

    lastTxDesc = (PRV_CPSS_TX_DESC_STC*)((GT_U8*)(txDescList->freeDescPtr) -
                   (sizeof(PRV_CPSS_TX_DESC_STC) + txDescList->actualBuffSize));

   /* In case this is the last packet left in the queue chain */
   if( (txDescList->freeDescNum + 1) == txDescList->maxDescNum )
   {
        /* If queue is enabled, disable it */
        rc = cpssDxChNetIfSdmaTxQueueEnableGet(devNum, txQueue, &enable);
        if( GT_OK != rc )
        {
            return rc;
        }

        rc = cpssDxChNetIfSdmaTxQueueEnable(devNum, txQueue, GT_FALSE);
        if( GT_OK != rc )
        {
            return rc;
        }
   }
   else /* more then one descriptor in the queue chain */
   {

    /* Get current rate configuration */
    rc = cpssDxChNetIfSdmaTxGeneratorRateGet(devNum,
                                             portGroupsBmp,
                                             txQueue,
                                             &rateMode,
                                             &rateValue,
                                             &actualRateValue);
    if( GT_OK != rc )
    {
        return rc;
    }

    /* To avoid warning */
    clkTicks = 0;

    /* Check availability of already configured rate can be supported due to */
    /* packet removal. */
    rc = prvCpssDxChNetIfSdmaTxGeneratorRateCalc(rateMode,
                                                 rateValue,
                                                 txDescList->maxDescNum -
                                                    txDescList->freeDescNum - 1,
                                                 &clkTicks,
                                                 &actualRateValue);
    if( GT_OK != rc )
    {
        return rc;
    }

    if( currTxDesc == txDescList->firstDescPtr )
    {
        /* if the removed descriptor is the first, the previous is the last */
        prevTxDesc = (PRV_CPSS_TX_DESC_STC*)((GT_U8*)(txDescList->freeDescPtr) -
                   (sizeof(PRV_CPSS_TX_DESC_STC) + txDescList->actualBuffSize));
    }
    else
    {
        prevTxDesc = (PRV_CPSS_TX_DESC_STC*)((GT_U8*)currTxDesc -
                   (sizeof(PRV_CPSS_TX_DESC_STC) + txDescList->actualBuffSize));
    }

    /* Skip the descriptor to be romoved */
    prevTxDesc->nextDescPointer = currTxDesc->nextDescPointer;

    /* Wait 3 packet generator intervals */
    rc = prvCpssDxChNetIfSdmaTxGenerator3PacketIntervalsWait(devNum,
                                                             portGroupId,
                                                             txQueue);
    if( GT_OK != rc )
    {
        return rc;
    }

    if (currTxDesc != lastTxDesc)
    {

        /* Copy the last descriptor in chain to the place of the removed one */
        currTxDesc->word1 = lastTxDesc->word1;
        currTxDesc->word2 = lastTxDesc->word2;

        /* Copy the last buffer in chain to the place of the removed one */
        cpssOsMemCpy( (GT_U8*)currTxDesc + sizeof(PRV_CPSS_TX_DESC_STC),
                      (GT_U8*)lastTxDesc + sizeof(PRV_CPSS_TX_DESC_STC),
                      TX_DESC_GET_BYTE_CNT_MAC(lastTxDesc) );

        /* Physical address of the current descriptor */
        rc = cpssOsVirt2Phy((GT_UINTPTR)(currTxDesc),
                            /*OUT*/&physicalMemoryPointer);
        if (rc != GT_OK)
        {
            return rc;
        }

        #if __WORDSIZE == 64    /* phyAddr must fit in 32 bit */
            if (0 != (physicalMemoryPointer & 0xffffffff00000000L))
            {
                CPSS_LOG_ERROR_AND_RETURN_MAC(GT_OUT_OF_RANGE, LOG_ERROR_NO_MSG);
            }
        #endif

        /* Return the previous descriptor pointing back to the current descriptor */
        prevTxDesc->nextDescPointer = CPSS_32BIT_LE((GT_U32)physicalMemoryPointer);

        /* Remove the last descriptor (already copied) from the chain */
        prevTxDesc = (PRV_CPSS_TX_DESC_STC*)((GT_U8*)(lastTxDesc) -
                       (sizeof(PRV_CPSS_TX_DESC_STC) + txDescList->actualBuffSize));

        prevTxDesc->nextDescPointer = lastTxDesc->nextDescPointer;

    }

    /* Update HW rate configuration  */
    rc = prvCpssDxChNetIfSdmaTxGeneratorHwRateSet(devNum,
                                                  portGroupId,
                                                  txQueue,
                                                  rateMode,
                                                  clkTicks);
   }

    movedPacketId =
     txDescList->revPacketIdDb[txDescList->maxDescNum-txDescList->freeDescNum-1];

    /* Update the moved last descriptor "reverse" packet ID DB*/
    txDescList->revPacketIdDb[placeInQueue] = movedPacketId;

    /* Update the moved last descriptor packet ID DB*/
    PRV_CPSS_PP_MAC(devNum)->intCtrl.txGeneratorPacketIdDb.
                                            cookie[movedPacketId] =
                                                txQueue + (placeInQueue << 3);
    /* Update the packet ID DB free list */
    PRV_CPSS_PP_MAC(devNum)->intCtrl.txGeneratorPacketIdDb.cookie[packetId] =
                                                                    0xFFFFFFFF;
    PRV_CPSS_PP_MAC(devNum)->
        intCtrl.txGeneratorPacketIdDb.freeLinkedList[packetId] =
               PRV_CPSS_PP_MAC(devNum)->intCtrl.txGeneratorPacketIdDb.firstFree;

    PRV_CPSS_PP_MAC(devNum)->intCtrl.txGeneratorPacketIdDb.firstFree = packetId;

    /* Increase queue free descriptors number */
    txDescList->freeDescNum += 1;

    /* Update next queue free descriptor pointer to previous last */
    txDescList->freeDescPtr = lastTxDesc;

    return rc;
}

/*******************************************************************************
* cpssDxChNetIfSdmaTxGeneratorPacketRemove
*
* DESCRIPTION:
*       This function removes packet from Tx SDMA working as a Packet Generator.
*
* APPLICABLE DEVICES:
*        xCat3; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum          - Device number.
*       portGroupsBmp   - bitmap of Port Groups.
*                       NOTEs:
*                       1. for non multi-port groups device this parameter is
*                          IGNORED.
*                       2. for multi-port groups device :
*                       bitmap must be set with at least one bit representing
*                       valid port group(s). If a bit of non valid port group
*                       is set then function returns GT_BAD_PARAM.
*                       value CPSS_PORT_GROUP_UNAWARE_MODE_CNS is supported.
*       txQueue         - The Tx SDMA queue to remove the packet from.
*       packetId          - packet identification number assign during packet
*                         addition to the Generator.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - Tx SDMA is not in Generator mode or packet
*                                  is not transmitted on this Tx SDMA.
*       GT_BAD_VALUE             - removal of packet will violate the required
*                                  rate configuration.
*       GT_NOT_INITIALIZED       - the library was not initialized.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device.
*
* COMMENTS:
*    Packet can be removed while Tx SDMA is enabled.
*
*******************************************************************************/
GT_STATUS cpssDxChNetIfSdmaTxGeneratorPacketRemove
(
    IN  GT_U8               devNum,
    IN  GT_PORT_GROUPS_BMP  portGroupsBmp,
    IN  GT_U8               txQueue,
    IN  GT_U32              packetId
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChNetIfSdmaTxGeneratorPacketRemove);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, portGroupsBmp, txQueue, packetId));

    rc = internal_cpssDxChNetIfSdmaTxGeneratorPacketRemove(devNum, portGroupsBmp, txQueue, packetId);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, portGroupsBmp, txQueue, packetId));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChNetIfSdmaTxGeneratorRateSet
*
* DESCRIPTION:
*       This function sets Tx SDMA Generator transmission packet rate.
*
* APPLICABLE DEVICES:
*        xCat3; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum          - Device number.
*       portGroupsBmp   - bitmap of Port Groups.
*                       NOTEs:
*                       1. for non multi-port groups device this parameter is
*                          IGNORED.
*                       2. for multi-port groups device :
*                       bitmap must be set with at least one bit representing
*                       valid port group(s). If a bit of non valid port group
*                       is set then function returns GT_BAD_PARAM.
*                       value CPSS_PORT_GROUP_UNAWARE_MODE_CNS is supported.
*       txQueue         - The Tx SDMA queue.
*       rateMode        - transmission rate mode.
*       rateValue       - packets per second for rateMode ==
*                      CPSS_DXCH_NET_TX_GENERATOR_RATE_MODE_GLOBAL_THROUGHPUT_E,
*                         nanoseconds for rateMode ==
*                      CPSS_DXCH_NET_TX_GENERATOR_RATE_MODE_PER_FLOW_INTERVAL_E.
*                 Not relevant to CPSS_DXCH_NET_TX_GENERATOR_RATE_MODE_NO_GAP_E.
*
* OUTPUTS:
*       actualRateValuePtr  - (pointer to) the actual configured rate value.
*                             Same units as rateValue.
*                 Not relevant to CPSS_DXCH_NET_TX_GENERATOR_RATE_MODE_NO_GAP_E.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - Tx SDMA is not in Generator mode, bad rate
*                                  mode or value.
*       GT_BAD_PTR               - one of the parameters is NULL pointer.
*       GT_BAD_VALUE             - rate cannot be configured for Tx SDMA.
*       GT_NOT_INITIALIZED       - the library was not initialized.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device.
*
* COMMENTS:
*       Rate can be set while Tx SDMA is enabled.
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChNetIfSdmaTxGeneratorRateSet
(
    IN  GT_U8                                       devNum,
    IN  GT_PORT_GROUPS_BMP                          portGroupsBmp,
    IN  GT_U8                                       txQueue,
    IN  CPSS_DXCH_NET_TX_GENERATOR_RATE_MODE_ENT    rateMode,
    IN  GT_U64                                      rateValue,
    OUT GT_U64                                      *actualRateValuePtr
)
{
    GT_STATUS   rc;
    GT_U32      portGroupId;      /* port group id */
    PRV_CPSS_TX_DESC_LIST_STC txDescList;   /* Tx desc. list */
    GT_U32      clkTicks = 0;     /* number of clock ticks for HW configuration */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
        CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E
        | CPSS_XCAT_E | CPSS_LION_E | CPSS_XCAT2_E | CPSS_LION2_E);
    PRV_CPSS_DXCH_NETIF_INITIALIZED_CHECK_MAC(devNum);
    PRV_CPSS_MULTI_PORT_GROUPS_BMP_CHECK_MAC(devNum,portGroupsBmp);



    PRV_CPSS_DXCH_DEV_TX_SDMA_QUEUE_NUM_CHECK_MAC(txQueue);

    portGroupId = PRV_CPSS_NETIF_SDMA_PORT_GROUP_ID_MAC(devNum);
    PRV_CPSS_DXCH_DEV_TX_SDMA_GENERATOR_CHECK_MAC(devNum,portGroupId,txQueue);

    txDescList  = PRV_CPSS_PP_MAC(devNum)->intCtrl.txDescList[txQueue];

    if ( CPSS_DXCH_NET_TX_GENERATOR_RATE_MODE_NO_GAP_E != rateMode )
    {
        CPSS_NULL_PTR_CHECK_MAC(actualRateValuePtr);
        rc = prvCpssDxChNetIfSdmaTxGeneratorRateCalc(rateMode,
                                                     rateValue,
                                                     txDescList.maxDescNum -
                                                     txDescList.freeDescNum,
                                                     &clkTicks,
                                                     actualRateValuePtr);
        if( GT_OK != rc )
        {
            return rc;
        }
    }

    /* HW rate configuration  */
    rc = prvCpssDxChNetIfSdmaTxGeneratorHwRateSet(devNum,
                                                  portGroupId,
                                                  txQueue,
                                                  rateMode,
                                                  clkTicks);

    if( GT_OK != rc )
    {
        return rc;
    }

    /* Save configured rate parameters to DB */
    PRV_CPSS_DXCH_PP_MAC(devNum)->netIf.rateMode[portGroupId][txQueue] =
                                                                      rateMode;
    PRV_CPSS_DXCH_PP_MAC(devNum)->netIf.rateValue[portGroupId][txQueue] =
                                                                      rateValue;

    return rc;
}

/*******************************************************************************
* cpssDxChNetIfSdmaTxGeneratorRateSet
*
* DESCRIPTION:
*       This function sets Tx SDMA Generator transmission packet rate.
*
* APPLICABLE DEVICES:
*        xCat3; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum          - Device number.
*       portGroupsBmp   - bitmap of Port Groups.
*                       NOTEs:
*                       1. for non multi-port groups device this parameter is
*                          IGNORED.
*                       2. for multi-port groups device :
*                       bitmap must be set with at least one bit representing
*                       valid port group(s). If a bit of non valid port group
*                       is set then function returns GT_BAD_PARAM.
*                       value CPSS_PORT_GROUP_UNAWARE_MODE_CNS is supported.
*       txQueue         - The Tx SDMA queue.
*       rateMode        - transmission rate mode.
*       rateValue       - packets per second for rateMode ==
*                      CPSS_DXCH_NET_TX_GENERATOR_RATE_MODE_GLOBAL_THROUGHPUT_E,
*                         nanoseconds for rateMode ==
*                      CPSS_DXCH_NET_TX_GENERATOR_RATE_MODE_PER_FLOW_INTERVAL_E.
*                 Not relevant to CPSS_DXCH_NET_TX_GENERATOR_RATE_MODE_NO_GAP_E.
*
* OUTPUTS:
*       actualRateValuePtr  - (pointer to) the actual configured rate value.
*                             Same units as rateValue.
*                 Not relevant to CPSS_DXCH_NET_TX_GENERATOR_RATE_MODE_NO_GAP_E.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - Tx SDMA is not in Generator mode, bad rate
*                                  mode or value.
*       GT_BAD_PTR               - one of the parameters is NULL pointer.
*       GT_BAD_VALUE             - rate cannot be configured for Tx SDMA.
*       GT_NOT_INITIALIZED       - the library was not initialized.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device.
*
* COMMENTS:
*       Rate can be set while Tx SDMA is enabled.
*
*******************************************************************************/
GT_STATUS cpssDxChNetIfSdmaTxGeneratorRateSet
(
    IN  GT_U8                                       devNum,
    IN  GT_PORT_GROUPS_BMP                          portGroupsBmp,
    IN  GT_U8                                       txQueue,
    IN  CPSS_DXCH_NET_TX_GENERATOR_RATE_MODE_ENT    rateMode,
    IN  GT_U64                                      rateValue,
    OUT GT_U64                                      *actualRateValuePtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChNetIfSdmaTxGeneratorRateSet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, portGroupsBmp, txQueue, rateMode, rateValue, actualRateValuePtr));

    rc = internal_cpssDxChNetIfSdmaTxGeneratorRateSet(devNum, portGroupsBmp, txQueue, rateMode, rateValue, actualRateValuePtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, portGroupsBmp, txQueue, rateMode, rateValue, actualRateValuePtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChNetIfSdmaTxGeneratorRateGet
*
* DESCRIPTION:
*       This function gets Tx SDMA Generator transmission packet rate.
*
* APPLICABLE DEVICES:
*        xCat3; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum          - Device number.
*       portGroupsBmp   - bitmap of Port Groups.
*                       NOTEs:
*                       1. for non multi-port groups device this parameter is
*                          IGNORED.
*                       2. for multi-port groups device :
*                       bitmap must be set with at least one bit representing
*                       valid port group(s). If a bit of non valid port group
*                       is set then function returns GT_BAD_PARAM.
*                       value CPSS_PORT_GROUP_UNAWARE_MODE_CNS is supported.
*       txQueue         - The Tx SDMA queue.
*
* OUTPUTS:
*       rateModePtr     - (pointer to) transmission rate mode
*       rateValuePtr    - (pointer to)
*                         packets per second for rateMode ==
*                      CPSS_DXCH_NET_TX_GENERATOR_RATE_MODE_GLOBAL_THROUGHPUT_E,
*                         nanoseconds for rateMode ==
*                      CPSS_DXCH_NET_TX_GENERATOR_RATE_MODE_PER_FLOW_INTERVAL_E.
*                 Not relevant to CPSS_DXCH_NET_TX_GENERATOR_RATE_MODE_NO_GAP_E.
*       actualRateValuePtr  - (pointer to) the actual configured rate value.
*                             Same units as rateValuePtr.
*                 Not relevant to CPSS_DXCH_NET_TX_GENERATOR_RATE_MODE_NO_GAP_E.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - Tx SDMA is not in Generator mode.
*       GT_BAD_PTR               - one of the parameters is NULL pointer.
*       GT_NOT_INITIALIZED       - the library was not initialized.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device.
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChNetIfSdmaTxGeneratorRateGet
(
    IN  GT_U8                                       devNum,
    IN  GT_PORT_GROUPS_BMP                          portGroupsBmp,
    IN  GT_U8                                       txQueue,
    OUT CPSS_DXCH_NET_TX_GENERATOR_RATE_MODE_ENT    *rateModePtr,
    OUT GT_U64                                      *rateValuePtr,
    OUT GT_U64                                      *actualRateValuePtr
)
{
    GT_U32      portGroupId;    /* port group id */
    PRV_CPSS_TX_DESC_LIST_STC txDescList;   /* Tx desc. list */
    GT_U32      clkTicks;         /* number of clock ticks for HW configuration */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
        CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E
        | CPSS_XCAT_E | CPSS_LION_E | CPSS_XCAT2_E | CPSS_LION2_E);
    PRV_CPSS_DXCH_NETIF_INITIALIZED_CHECK_MAC(devNum);
    PRV_CPSS_MULTI_PORT_GROUPS_BMP_CHECK_MAC(devNum,portGroupsBmp);

    CPSS_NULL_PTR_CHECK_MAC(rateModePtr);


    PRV_CPSS_DXCH_DEV_TX_SDMA_QUEUE_NUM_CHECK_MAC(txQueue);

    portGroupId = PRV_CPSS_NETIF_SDMA_PORT_GROUP_ID_MAC(devNum);
    PRV_CPSS_DXCH_DEV_TX_SDMA_GENERATOR_CHECK_MAC(devNum,portGroupId,txQueue);

    txDescList  = PRV_CPSS_PP_MAC(devNum)->intCtrl.txDescList[txQueue];

    *rateModePtr = PRV_CPSS_DXCH_PP_MAC(devNum)->
                                        netIf.rateMode[portGroupId][txQueue];

    if( CPSS_DXCH_NET_TX_GENERATOR_RATE_MODE_NO_GAP_E != *rateModePtr )
    {
        CPSS_NULL_PTR_CHECK_MAC(rateValuePtr);
        CPSS_NULL_PTR_CHECK_MAC(actualRateValuePtr);

        *rateValuePtr = PRV_CPSS_DXCH_PP_MAC(devNum)->
                                        netIf.rateValue[portGroupId][txQueue];

        return prvCpssDxChNetIfSdmaTxGeneratorRateCalc(*rateModePtr,
                                                       *rateValuePtr,
                                                       txDescList.maxDescNum -
                                                        txDescList.freeDescNum,
                                                       &clkTicks,
                                                       actualRateValuePtr);
    }

    return GT_OK;
}

/*******************************************************************************
* cpssDxChNetIfSdmaTxGeneratorRateGet
*
* DESCRIPTION:
*       This function gets Tx SDMA Generator transmission packet rate.
*
* APPLICABLE DEVICES:
*        xCat3; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum          - Device number.
*       portGroupsBmp   - bitmap of Port Groups.
*                       NOTEs:
*                       1. for non multi-port groups device this parameter is
*                          IGNORED.
*                       2. for multi-port groups device :
*                       bitmap must be set with at least one bit representing
*                       valid port group(s). If a bit of non valid port group
*                       is set then function returns GT_BAD_PARAM.
*                       value CPSS_PORT_GROUP_UNAWARE_MODE_CNS is supported.
*       txQueue         - The Tx SDMA queue.
*
* OUTPUTS:
*       rateModePtr     - (pointer to) transmission rate mode
*       rateValuePtr    - (pointer to)
*                         packets per second for rateMode ==
*                      CPSS_DXCH_NET_TX_GENERATOR_RATE_MODE_GLOBAL_THROUGHPUT_E,
*                         nanoseconds for rateMode ==
*                      CPSS_DXCH_NET_TX_GENERATOR_RATE_MODE_PER_FLOW_INTERVAL_E.
*                 Not relevant to CPSS_DXCH_NET_TX_GENERATOR_RATE_MODE_NO_GAP_E.
*       actualRateValuePtr  - (pointer to) the actual configured rate value.
*                             Same units as rateValuePtr.
*                 Not relevant to CPSS_DXCH_NET_TX_GENERATOR_RATE_MODE_NO_GAP_E.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - Tx SDMA is not in Generator mode.
*       GT_BAD_PTR               - one of the parameters is NULL pointer.
*       GT_NOT_INITIALIZED       - the library was not initialized.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device.
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChNetIfSdmaTxGeneratorRateGet
(
    IN  GT_U8                                       devNum,
    IN  GT_PORT_GROUPS_BMP                          portGroupsBmp,
    IN  GT_U8                                       txQueue,
    OUT CPSS_DXCH_NET_TX_GENERATOR_RATE_MODE_ENT    *rateModePtr,
    OUT GT_U64                                      *rateValuePtr,
    OUT GT_U64                                      *actualRateValuePtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChNetIfSdmaTxGeneratorRateGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, portGroupsBmp, txQueue, rateModePtr, rateValuePtr, actualRateValuePtr));

    rc = internal_cpssDxChNetIfSdmaTxGeneratorRateGet(devNum, portGroupsBmp, txQueue, rateModePtr, rateValuePtr, actualRateValuePtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, portGroupsBmp, txQueue, rateModePtr, rateValuePtr, actualRateValuePtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChNetIfSdmaTxGeneratorEnable
*
* DESCRIPTION:
*       This function enables selected Tx SDMA Generator.
*
* APPLICABLE DEVICES:
*        xCat3; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum          - Device number.
*       portGroupsBmp   - bitmap of Port Groups.
*                       NOTEs:
*                       1. for non multi-port groups device this parameter is
*                          IGNORED.
*                       2. for multi-port groups device :
*                       bitmap must be set with at least one bit representing
*                       valid port group(s). If a bit of non valid port group
*                       is set then function returns GT_BAD_PARAM.
*                       value CPSS_PORT_GROUP_UNAWARE_MODE_CNS is supported.
*       txQueue         - The Tx SDMA queue to enable.
*       burstEnable     - GT_TRUE for packets burst generation,
*                       - GT_FALSE for continuous packets generation.
*       burstPacketsNumber - Number of packets in burst.
*                            Relevant only if burstEnable == GT_TRUE.
*                            (APPLICABLE RANGES: 0..512M-1)
*
* OUTPUTS:
*       none.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - Tx SDMA is not in Generator mode or burst
*                                  size.
*       GT_BAD_PTR               - one of the parameters is NULL pointer.
*       GT_NOT_INITIALIZED       - the library was not initialized.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device.
*
* COMMENTS:
*       If queue is empty, configuration is done beside the enabling which will
*       be implicitly performed after the first packet will be added to the
*       queue.
*       On each HW queue enabling the Tx SDMA Current Descriptor Pointer
*       register will set to point to the first descriptor in chain list.
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChNetIfSdmaTxGeneratorEnable
(
    IN  GT_U8                           devNum,
    IN  GT_PORT_GROUPS_BMP              portGroupsBmp,
    IN  GT_U8                           txQueue,
    IN  GT_BOOL                         burstEnable,
    IN  GT_U32                          burstPacketsNumber
)
{
    GT_STATUS   rc;
    GT_U32      portGroupId;    /* port group id */
    GT_U32      regValue;       /* register value */
    PRV_CPSS_TX_DESC_LIST_STC *txDescList;   /* Tx desc. list */
    GT_BOOL     queueEnableState;   /* the current HW enabling queue status*/
    GT_UINTPTR  physicalMemoryPointer;

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
        CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E
        | CPSS_XCAT_E | CPSS_LION_E | CPSS_XCAT2_E | CPSS_LION2_E);
    PRV_CPSS_DXCH_NETIF_INITIALIZED_CHECK_MAC(devNum);
    PRV_CPSS_MULTI_PORT_GROUPS_BMP_CHECK_MAC(devNum,portGroupsBmp);

    PRV_CPSS_DXCH_DEV_TX_SDMA_QUEUE_NUM_CHECK_MAC(txQueue);

    portGroupId = PRV_CPSS_NETIF_SDMA_PORT_GROUP_ID_MAC(devNum);
    PRV_CPSS_DXCH_DEV_TX_SDMA_GENERATOR_CHECK_MAC(devNum,portGroupId,txQueue);

    txDescList  = &(PRV_CPSS_PP_MAC(devNum)->intCtrl.txDescList[txQueue]);

    if( GT_TRUE == burstEnable )
    {
        if( burstPacketsNumber >= BIT_29 )
        {
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
        }

        regValue = 1 + ( burstPacketsNumber << 1);
    }
    else
    {
        regValue = 0x0;
    }

    rc = prvCpssHwPpPortGroupWriteRegBitMask(
            devNum,
            portGroupId,
            PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->sdmaRegs.
                                    txSdmaPacketCountConfigQueue[txQueue],
            0x3FFFFFFF,
            regValue);

    if( GT_OK != rc )
    {
        return rc;
    }

    /* Indication for explicit enabling request */
    txDescList->userQueueEnabled = GT_TRUE;

    /* Enabling a queue is prohibited if the queue is empty, and will be done */
    /* later implicitly when the first packet is added to the queue. */
    if( txDescList->freeDescNum == txDescList->maxDescNum )
    {
        return GT_OK;
    }

    /* No need to re-enabled, already enabled queue */
    rc = cpssDxChNetIfSdmaTxQueueEnableGet(devNum, txQueue, &queueEnableState);
    if( GT_OK != rc || GT_TRUE == queueEnableState )
    {
        return rc;
    }

    /* Set Tx SDMA Current Descriptor Pointer to first descriptor in chain */
    rc = cpssOsVirt2Phy((GT_UINTPTR)(txDescList->firstDescPtr),
                        /*OUT*/&physicalMemoryPointer);
    if (rc != GT_OK)
    {
        return rc;
    }

    #if __WORDSIZE == 64    /* phyAddr must fit in 32 bit */
        if (0 != (physicalMemoryPointer & 0xffffffff00000000L))
        {
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_OUT_OF_RANGE, LOG_ERROR_NO_MSG);
        }
    #endif

    rc = prvCpssHwPpPortGroupWriteRegister(
            devNum,
            portGroupId,
            PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
                     sdmaRegs.txDmaCdp[txQueue],(GT_U32)physicalMemoryPointer);

    if(GT_OK != rc)
    {
        return rc;
    }

    return cpssDxChNetIfSdmaTxQueueEnable(devNum, txQueue, GT_TRUE);
}

/*******************************************************************************
* cpssDxChNetIfSdmaTxGeneratorEnable
*
* DESCRIPTION:
*       This function enables selected Tx SDMA Generator.
*
* APPLICABLE DEVICES:
*        xCat3; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum          - Device number.
*       portGroupsBmp   - bitmap of Port Groups.
*                       NOTEs:
*                       1. for non multi-port groups device this parameter is
*                          IGNORED.
*                       2. for multi-port groups device :
*                       bitmap must be set with at least one bit representing
*                       valid port group(s). If a bit of non valid port group
*                       is set then function returns GT_BAD_PARAM.
*                       value CPSS_PORT_GROUP_UNAWARE_MODE_CNS is supported.
*       txQueue         - The Tx SDMA queue to enable.
*       burstEnable     - GT_TRUE for packets burst generation,
*                       - GT_FALSE for continuous packets generation.
*       burstPacketsNumber - Number of packets in burst.
*                            Relevant only if burstEnable == GT_TRUE.
*                            (APPLICABLE RANGES: 0..512M-1)
*
* OUTPUTS:
*       none.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - Tx SDMA is not in Generator mode or burst
*                                  size.
*       GT_BAD_PTR               - one of the parameters is NULL pointer.
*       GT_NOT_INITIALIZED       - the library was not initialized.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device.
*
* COMMENTS:
*       If queue is empty, configuration is done beside the enabling which will
*       be implicitly performed after the first packet will be added to the
*       queue.
*       On each HW queue enabling the Tx SDMA Current Descriptor Pointer
*       register will set to point to the first descriptor in chain list.
*
*******************************************************************************/
GT_STATUS cpssDxChNetIfSdmaTxGeneratorEnable
(
    IN  GT_U8                           devNum,
    IN  GT_PORT_GROUPS_BMP              portGroupsBmp,
    IN  GT_U8                           txQueue,
    IN  GT_BOOL                         burstEnable,
    IN  GT_U32                          burstPacketsNumber
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChNetIfSdmaTxGeneratorEnable);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, portGroupsBmp, txQueue, burstEnable, burstPacketsNumber));

    rc = internal_cpssDxChNetIfSdmaTxGeneratorEnable(devNum, portGroupsBmp, txQueue, burstEnable, burstPacketsNumber);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, portGroupsBmp, txQueue, burstEnable, burstPacketsNumber));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChNetIfSdmaTxGeneratorDisable
*
* DESCRIPTION:
*       This function disables selected Tx SDMA Generator.
*
* APPLICABLE DEVICES:
*        xCat3; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum          - Device number.
*       portGroupsBmp   - bitmap of Port Groups.
*                       NOTEs:
*                       1. for non multi-port groups device this parameter is
*                          IGNORED.
*                       2. for multi-port groups device :
*                       bitmap must be set with at least one bit representing
*                       valid port group(s). If a bit of non valid port group
*                       is set then function returns GT_BAD_PARAM.
*                       value CPSS_PORT_GROUP_UNAWARE_MODE_CNS is supported.
*       txQueue         - The Tx SDMA queue to enable.
*
* OUTPUTS:
*       none.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - Tx SDMA is not in Generator mode.
*       GT_BAD_PTR               - one of the parameters is NULL pointer.
*       GT_NOT_INITIALIZED       - the library was not initialized.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device.
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChNetIfSdmaTxGeneratorDisable
(
    IN  GT_U8                           devNum,
    IN  GT_PORT_GROUPS_BMP              portGroupsBmp,
    IN  GT_U8                           txQueue
)
{
    GT_U32      portGroupId;    /* port group id */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
        CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E
        | CPSS_XCAT_E | CPSS_LION_E | CPSS_XCAT2_E | CPSS_LION2_E);
    PRV_CPSS_DXCH_NETIF_INITIALIZED_CHECK_MAC(devNum);
    PRV_CPSS_MULTI_PORT_GROUPS_BMP_CHECK_MAC(devNum,portGroupsBmp);

    PRV_CPSS_DXCH_DEV_TX_SDMA_QUEUE_NUM_CHECK_MAC(txQueue);

    portGroupId = PRV_CPSS_NETIF_SDMA_PORT_GROUP_ID_MAC(devNum);
    PRV_CPSS_DXCH_DEV_TX_SDMA_GENERATOR_CHECK_MAC(devNum,portGroupId,txQueue);

    /* Indication for explicit disabling request */
    PRV_CPSS_PP_MAC(devNum)->intCtrl.txDescList[txQueue].userQueueEnabled =
                                                                       GT_FALSE;

    return cpssDxChNetIfSdmaTxQueueEnable(devNum, txQueue, GT_FALSE);
}

/*******************************************************************************
* cpssDxChNetIfSdmaTxGeneratorDisable
*
* DESCRIPTION:
*       This function disables selected Tx SDMA Generator.
*
* APPLICABLE DEVICES:
*        xCat3; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum          - Device number.
*       portGroupsBmp   - bitmap of Port Groups.
*                       NOTEs:
*                       1. for non multi-port groups device this parameter is
*                          IGNORED.
*                       2. for multi-port groups device :
*                       bitmap must be set with at least one bit representing
*                       valid port group(s). If a bit of non valid port group
*                       is set then function returns GT_BAD_PARAM.
*                       value CPSS_PORT_GROUP_UNAWARE_MODE_CNS is supported.
*       txQueue         - The Tx SDMA queue to enable.
*
* OUTPUTS:
*       none.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - Tx SDMA is not in Generator mode.
*       GT_BAD_PTR               - one of the parameters is NULL pointer.
*       GT_NOT_INITIALIZED       - the library was not initialized.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device.
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChNetIfSdmaTxGeneratorDisable
(
    IN  GT_U8                           devNum,
    IN  GT_PORT_GROUPS_BMP              portGroupsBmp,
    IN  GT_U8                           txQueue
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChNetIfSdmaTxGeneratorDisable);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, portGroupsBmp, txQueue));

    rc = internal_cpssDxChNetIfSdmaTxGeneratorDisable(devNum, portGroupsBmp, txQueue);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, portGroupsBmp, txQueue));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChNetIfSdmaTxGeneratorBurstStatusGet
*
* DESCRIPTION:
*       This function gets Tx SDMA Generator burst counter status.
*
* APPLICABLE DEVICES:
*        xCat3; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum          - Device number.
*       portGroupsBmp   - bitmap of Port Groups.
*                       NOTEs:
*                       1. for non multi-port groups device this parameter is
*                          IGNORED.
*                       2. for multi-port groups device :
*                       bitmap must be set with at least one bit representing
*                       valid port group(s). If a bit of non valid port group
*                       is set then function returns GT_BAD_PARAM.
*                       value CPSS_PORT_GROUP_UNAWARE_MODE_CNS is supported.
*       txQueue         - The Tx SDMA queue to enable.
*
* OUTPUTS:
*       burstStatusPtr  - (pointer to) Burst counter status.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - Tx SDMA is not in Generator mode.
*       GT_BAD_PTR               - one of the parameters is NULL pointer.
*       GT_NOT_INITIALIZED       - the library was not initialized.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device.
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChNetIfSdmaTxGeneratorBurstStatusGet
(
    IN  GT_U8                                       devNum,
    IN  GT_PORT_GROUPS_BMP                          portGroupsBmp,
    IN  GT_U8                                       txQueue,
    OUT CPSS_DXCH_NET_TX_GENERATOR_BURST_STATUS_ENT *burstStatusPtr
)
{
    GT_STATUS   rc;
    GT_U32      portGroupId;    /* port group id */
    GT_U32      fieldValue;     /* field value */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
        CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E
        | CPSS_XCAT_E | CPSS_LION_E | CPSS_XCAT2_E | CPSS_LION2_E);
    PRV_CPSS_DXCH_NETIF_INITIALIZED_CHECK_MAC(devNum);
    PRV_CPSS_MULTI_PORT_GROUPS_BMP_CHECK_MAC(devNum,portGroupsBmp);

    CPSS_NULL_PTR_CHECK_MAC(burstStatusPtr);

    PRV_CPSS_DXCH_DEV_TX_SDMA_QUEUE_NUM_CHECK_MAC(txQueue);

    portGroupId = PRV_CPSS_NETIF_SDMA_PORT_GROUP_ID_MAC(devNum);
    PRV_CPSS_DXCH_DEV_TX_SDMA_GENERATOR_CHECK_MAC(devNum,portGroupId,txQueue);

    rc = prvCpssHwPpPortGroupGetRegField(
            devNum,
            portGroupId,
            PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->sdmaRegs.
                                    txSdmaPacketCountConfigQueue[txQueue],
            30, 2, &fieldValue);

    if( GT_OK != rc )
    {
        return rc;
    }

    switch(fieldValue)
    {
        case 0: *burstStatusPtr =
                            CPSS_DXCH_NET_TX_GENERATOR_BURST_STATUS_IDLE_E;
                break;
        case 1: *burstStatusPtr =
                            CPSS_DXCH_NET_TX_GENERATOR_BURST_STATUS_RUN_E;
                break;
        case 2: *burstStatusPtr =
                            CPSS_DXCH_NET_TX_GENERATOR_BURST_STATUS_LIMIT_E;
                break;
        case 3: *burstStatusPtr =
                            CPSS_DXCH_NET_TX_GENERATOR_BURST_STATUS_TERMINATE_E;
                break;
        default:
                CPSS_LOG_ERROR_AND_RETURN_MAC(GT_FAIL, LOG_ERROR_NO_MSG);
                break;
    }

    return GT_OK;
}

/*******************************************************************************
* cpssDxChNetIfSdmaTxGeneratorBurstStatusGet
*
* DESCRIPTION:
*       This function gets Tx SDMA Generator burst counter status.
*
* APPLICABLE DEVICES:
*        xCat3; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum          - Device number.
*       portGroupsBmp   - bitmap of Port Groups.
*                       NOTEs:
*                       1. for non multi-port groups device this parameter is
*                          IGNORED.
*                       2. for multi-port groups device :
*                       bitmap must be set with at least one bit representing
*                       valid port group(s). If a bit of non valid port group
*                       is set then function returns GT_BAD_PARAM.
*                       value CPSS_PORT_GROUP_UNAWARE_MODE_CNS is supported.
*       txQueue         - The Tx SDMA queue to enable.
*
* OUTPUTS:
*       burstStatusPtr  - (pointer to) Burst counter status.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - Tx SDMA is not in Generator mode.
*       GT_BAD_PTR               - one of the parameters is NULL pointer.
*       GT_NOT_INITIALIZED       - the library was not initialized.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device.
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChNetIfSdmaTxGeneratorBurstStatusGet
(
    IN  GT_U8                                       devNum,
    IN  GT_PORT_GROUPS_BMP                          portGroupsBmp,
    IN  GT_U8                                       txQueue,
    OUT CPSS_DXCH_NET_TX_GENERATOR_BURST_STATUS_ENT *burstStatusPtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChNetIfSdmaTxGeneratorBurstStatusGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, portGroupsBmp, txQueue, burstStatusPtr));

    rc = internal_cpssDxChNetIfSdmaTxGeneratorBurstStatusGet(devNum, portGroupsBmp, txQueue, burstStatusPtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, portGroupsBmp, txQueue, burstStatusPtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

