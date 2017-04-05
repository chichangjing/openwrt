/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* cpssDxChPortInterlaken.c
*
* DESCRIPTION:
*       CPSS DxCh implementation for Interlaken port configuration.
*
* NOTE:
*
* FILE REVISION NUMBER:
*       $Revision: 16 $
*******************************************************************************/
#define CPSS_LOG_IN_MODULE_ENABLE
#include <cpss/dxCh/dxChxGen/port/private/prvCpssDxChPortLog.h>
#include <cpss/dxCh/dxChxGen/config/private/prvCpssDxChInfo.h>
#include <cpss/dxCh/dxChxGen/port/cpssDxChPortStat.h>
#include <cpss/generic/port/cpssPortStat.h>

#include <cpss/dxCh/dxChxGen/port/cpssDxChPortInterlaken.h>
#include <cpss/dxCh/dxChxGen/port/PizzaArbiter/DynamicPizzaArbiter/prvCpssDxChPortDynamicPizzaArbiter.h>
#include <cpss/dxCh/dxChxGen/port/PizzaArbiter/DynamicPizzaArbiter/prvCpssDxChPortDynamicPAPortSpeedDB.h>

#include <common/siliconIf/mvSiliconIf.h>

/* in Bobcat2 just MAC 64 capable for ILKN interface support */
#define PRV_CPSS_DXCH_BOBCAT2_ILKN_MAC_CNS  64

/* in Bobcat2 ILKN interface support 64 channels */
#define PRV_CPSS_DXCH_BOBCAT2_ILKN_CHANNEL_MAX_CNS  64

#define PRV_CPSS_DXCH_BOBCAT2_ILKN_PR_MEM_SIZE_CNS  20480 /* 64*(2*0xA0) */

#define GT_NA ((GT_U32)-1) 

typedef struct PRV_CPSS_DXCH_PR_MEM_CHUNK_DATA_STCT
{
    GT_U32 offset;
    GT_U32 size;
    GT_BOOL inUse;
    GT_U32  channelId;

}PRV_CPSS_DXCH_PR_MEM_CHUNK_DATA_STC;

/* number of chunks in worst case could be 64*2 one used one free */
static PRV_CPSS_DXCH_PR_MEM_CHUNK_DATA_STC prMemoryChunksArray
                            [PRV_CPSS_DXCH_BOBCAT2_ILKN_CHANNEL_MAX_CNS*2];

/*******************************************************************************
* prvCpssDxChPortIlknPrMemInit
*
* DESCRIPTION:
*       Init internal DB for PR memory management.
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
*
* INPUTS:
*       None
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_NO_RESOURCE           - not enough free memory
*       GT_BAD_STATE             - algorithm failed
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_VOID prvCpssDxChPortIlknPrMemInit
(
    GT_VOID
)
{
    GT_U32  i;

    for(i = 0; i < PRV_CPSS_DXCH_BOBCAT2_ILKN_CHANNEL_MAX_CNS*2; i++)
    {
        prMemoryChunksArray[i].channelId = GT_NA;
        prMemoryChunksArray[i].inUse = GT_FALSE;
        prMemoryChunksArray[i].size = 0;
        prMemoryChunksArray[i].offset = GT_NA;
    }

    prMemoryChunksArray[0].size = PRV_CPSS_DXCH_BOBCAT2_ILKN_PR_MEM_SIZE_CNS;
    prMemoryChunksArray[0].offset = 0;

    return;
}

/*******************************************************************************
* prvCpssDxChPortIlknPrChunkAlloc
*
* DESCRIPTION:
*       Allocate chunk of PR memory.
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
*
* INPUTS:
*       channelId   - channel number;
*       size        - size of required memory.
*
* OUTPUTS:
*       offsetPtr   - offset of chunk in memory
*
* RETURNS:
*       GT_OK                    - on success
*       GT_NO_RESOURCE           - not enough free memory
*       GT_BAD_STATE             - algorithm failed
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS prvCpssDxChPortIlknPrChunkAlloc
(
    IN  GT_U32 channelId,
    IN  GT_U32 size,
    OUT GT_U32 *offsetPtr
)
{
    GT_U32  i, j;
    GT_U32  freeMemChunkSize;
    GT_U32  nextFreeChunk;

    for(i = 0; i < PRV_CPSS_DXCH_BOBCAT2_ILKN_CHANNEL_MAX_CNS*2; i++)
    {
        if((!prMemoryChunksArray[i].inUse) && (prMemoryChunksArray[i].size >= size))
        {
            prMemoryChunksArray[i].inUse = GT_TRUE;
            prMemoryChunksArray[i].channelId = channelId;
            freeMemChunkSize = prMemoryChunksArray[i].size - size;
            prMemoryChunksArray[i].size = size;
            if(i+1 < PRV_CPSS_DXCH_BOBCAT2_ILKN_CHANNEL_MAX_CNS*2)
            {
                if(!prMemoryChunksArray[i+1].inUse)
                {
                    prMemoryChunksArray[i+1].offset = 
                                            prMemoryChunksArray[i].offset + size;
                    prMemoryChunksArray[i+1].size += freeMemChunkSize;

                    break;
                }
                else
                {
                    for(j = i+2, nextFreeChunk = GT_NA; j < PRV_CPSS_DXCH_BOBCAT2_ILKN_CHANNEL_MAX_CNS*2; j++)
                    {
                        if(!prMemoryChunksArray[j].inUse)
                            nextFreeChunk = j;
                    }
                    if(nextFreeChunk != GT_NA)
                    {
                        for(j = nextFreeChunk; nextFreeChunk > i+1; nextFreeChunk--)
                        {
                            cpssOsMemCpy(&(prMemoryChunksArray[nextFreeChunk]),
                                         &(prMemoryChunksArray[nextFreeChunk-1]),
                                           sizeof(PRV_CPSS_DXCH_PR_MEM_CHUNK_DATA_STC));
                        }

                        prMemoryChunksArray[i+1].offset = 
                                                prMemoryChunksArray[i].offset + size;
                        prMemoryChunksArray[i+1].size = freeMemChunkSize;
                        break;
                    }
                    else
                    {/* it can happen only if there is bug in algorithm */
                        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_STATE, LOG_ERROR_NO_MSG);
                    }
                }
            }
            else
            {
                break;
            }
        }
    }

    if(PRV_CPSS_DXCH_BOBCAT2_ILKN_CHANNEL_MAX_CNS*2 == i)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NO_RESOURCE, LOG_ERROR_NO_MSG);
    }

    *offsetPtr = prMemoryChunksArray[i].offset;

    return GT_OK;
}

/*******************************************************************************
* prvCpssDxChPortIlknPrChunkFree
*
* DESCRIPTION:
*       Free chunk of PR memory and also try some defragmentation.
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
*
* INPUTS:
*       channelId - channel number.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_NOT_FOUND             - chunk of channel not found
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS prvCpssDxChPortIlknPrChunkFree
(
    GT_U32 channelId
)
{
    GT_U32  i;
    GT_32   j;

    for(i = 0; i < PRV_CPSS_DXCH_BOBCAT2_ILKN_CHANNEL_MAX_CNS*2; i++)
    {
        if(prMemoryChunksArray[i].channelId == channelId)
        {
            prMemoryChunksArray[i].inUse = GT_FALSE;

            if(i+1 < PRV_CPSS_DXCH_BOBCAT2_ILKN_CHANNEL_MAX_CNS*2)
            {
                /* if next chunk is free lets add it to current and create one big
                   free chunk */
                if(!prMemoryChunksArray[i+1].inUse)
                {
                    prMemoryChunksArray[i].size += prMemoryChunksArray[i+1].size;
                    prMemoryChunksArray[i+1].size = 0;
                    prMemoryChunksArray[i+1].offset = GT_NA;
                }
            }

            if(i > 0)
            {
                for(j = i-1; j >= 0; j--)
                {
                    if((!prMemoryChunksArray[j].inUse) && (prMemoryChunksArray[j].size != 0))
                    {
                        prMemoryChunksArray[j].size += prMemoryChunksArray[i].size;
                        prMemoryChunksArray[i].size = 0;
                        prMemoryChunksArray[i].offset = GT_NA;
                        return GT_OK;
                    }
                }
            }

            return GT_OK;
        }
    }

    /* if requested free memory of not existing channel no problem */
    return GT_OK;
}

/*******************************************************************************
* internal_cpssDxChPortInterlakenCounterGet
*
* DESCRIPTION:
*       Function gets counters of Port Interlaken interface.
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
*
* INPUTS:
*       devNum              - device number
*       portNum             - physical port number.
*       counterId           - counter Id
*       laneNum             - lane number - relevant only for
*                             BLK_TYPE_ERR, DIAG_CRC_ERR, WORD_SYNC_ERR
*                             (APPLICABLE RANGES: 0..7)
*
* OUTPUTS:
*       counterPtr   - (pointer to) 64-bit counter value
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong device
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_BAD_STATE             - on illegal state
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChPortInterlakenCounterGet
(
    IN  GT_U8                                   devNum,
    IN  GT_PHYSICAL_PORT_NUM                    portNum,
    IN  CPSS_PORT_INTERLAKEN_COUNTER_ID_ENT     counterId,
    IN  GT_U32                                  laneNum,
    OUT GT_U64                                  *counterPtr
)
{
    GT_STATUS   rc;                 /* return status        */
    GT_U32      regAddr[2];         /* register address     */
    GT_U32      regMask[2];         /* register mask        */
    GT_U32      regNum;             /* number of registers  */
    GT_U32      i;                  /* loop index           */
    CPSS_DXCH_PORT_MAP_STC  portMap;/* port map struct      */


    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E | CPSS_LION_E | CPSS_XCAT2_E | CPSS_LION2_E   );
    CPSS_NULL_PTR_CHECK_MAC(counterPtr);

    rc = cpssDxChPortPhysicalPortMapGet(devNum, portNum, 1, &portMap);
    if((rc != GT_OK) || (portMap.mappingType != CPSS_DXCH_PORT_MAPPING_TYPE_ILKN_CHANNEL_E))
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    /* check and calculate regNum and regMask */
    switch (counterId)
    {
        case CPSS_PORT_INTERLAKEN_COUNTER_ID_BLK_TYPE_ERR_E:
        case CPSS_PORT_INTERLAKEN_COUNTER_ID_DIAG_CRC_ERR_E:
        case CPSS_PORT_INTERLAKEN_COUNTER_ID_WORD_SYNC_ERR_E:
            if (laneNum > 7)
            {
                CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
            }
            regNum = 1;
            regMask[0] = 0xFFFF;
            regMask[1] = 0;
            break;
        case CPSS_PORT_INTERLAKEN_COUNTER_ID_RX_BYTE_E:
        case CPSS_PORT_INTERLAKEN_COUNTER_ID_RX_BAD_PKT_E:
        case CPSS_PORT_INTERLAKEN_COUNTER_ID_RX_PKT_E:
        case CPSS_PORT_INTERLAKEN_COUNTER_ID_RX_CRC_ERR_E:
        case CPSS_PORT_INTERLAKEN_COUNTER_ID_TX_BYTE_E:
        case CPSS_PORT_INTERLAKEN_COUNTER_ID_TX_BAD_PKT_E:
        case CPSS_PORT_INTERLAKEN_COUNTER_ID_TX_PKT_E:
            regNum = 2;
            regMask[0] = 0xFFFFFFFF;
            regMask[1] = 0xFFFFFFFF;
            break;
        case CPSS_PORT_INTERLAKEN_COUNTER_ID_RX_BURST_SIZE_ERR_E:
            regNum = 2;
            regMask[0] = 0xFFFFFFFF;
            regMask[1] = 0x0FFF;
            break;
        case CPSS_PORT_INTERLAKEN_COUNTER_ID_RX_ALIGNMENT_ERR_E:
        case CPSS_PORT_INTERLAKEN_COUNTER_ID_RX_ALIGNMENT_FAIL_E:
            regNum = 1;
            regMask[0] = 0xFFFF;
            regMask[1] = 0;
            break;
        default: CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    /* default */
    regAddr[1] = PRV_CPSS_SW_PTR_ENTRY_UNUSED;

    /* calculate regAddr */
    switch (counterId)
    {
        case CPSS_PORT_INTERLAKEN_COUNTER_ID_BLK_TYPE_ERR_E:
            regAddr[0] = PRV_DXCH_REG1_UNIT_ILKN_MAC(devNum).ILKN_COUNT_BLK_TYPE_ERR_L[laneNum];
            break;

        case CPSS_PORT_INTERLAKEN_COUNTER_ID_DIAG_CRC_ERR_E:
            regAddr[0] = PRV_DXCH_REG1_UNIT_ILKN_MAC(devNum).ILKN_COUNT_DIAG_CRC_ERR_L[laneNum];
            break;

        case CPSS_PORT_INTERLAKEN_COUNTER_ID_WORD_SYNC_ERR_E:
            regAddr[0] = PRV_DXCH_REG1_UNIT_ILKN_MAC(devNum).ILKN_COUNT_WORD_SYNC_ERR_L[laneNum];
            break;

        case CPSS_PORT_INTERLAKEN_COUNTER_ID_RX_BYTE_E:
            regAddr[0] = PRV_DXCH_REG1_UNIT_ILKN_MAC(devNum).ILKN_COUNT_RX_BYTE_LOW;
            regAddr[1] = PRV_DXCH_REG1_UNIT_ILKN_MAC(devNum).ILKN_COUNT_RX_BYTE_HIGH;
            break;

        case CPSS_PORT_INTERLAKEN_COUNTER_ID_RX_BAD_PKT_E:
            regAddr[0] = PRV_DXCH_REG1_UNIT_ILKN_MAC(devNum).ILKN_COUNT_RX_BAD_PKT_LOW;
            regAddr[1] = PRV_DXCH_REG1_UNIT_ILKN_MAC(devNum).ILKN_COUNT_RX_BAD_PKT_HIGH;
            break;

        case CPSS_PORT_INTERLAKEN_COUNTER_ID_RX_PKT_E:
            regAddr[0] = PRV_DXCH_REG1_UNIT_ILKN_MAC(devNum).ILKN_COUNT_RX_PKT_LOW;
            regAddr[1] = PRV_DXCH_REG1_UNIT_ILKN_MAC(devNum).ILKN_COUNT_RX_PKT_HIGH;
            break;

        case CPSS_PORT_INTERLAKEN_COUNTER_ID_RX_CRC_ERR_E:
            regAddr[0] = PRV_DXCH_REG1_UNIT_ILKN_MAC(devNum).ILKN_COUNT_RX_CRC_ERROR_LOW;
            regAddr[1] = PRV_DXCH_REG1_UNIT_ILKN_MAC(devNum).ILKN_COUNT_RX_CRC_ERROR_HIGH;
            break;

        case CPSS_PORT_INTERLAKEN_COUNTER_ID_TX_BYTE_E:
            regAddr[0] = PRV_DXCH_REG1_UNIT_ILKN_MAC(devNum).ILKN_COUNT_TX_BYTE_LOW;
            regAddr[1] = PRV_DXCH_REG1_UNIT_ILKN_MAC(devNum).ILKN_COUNT_TX_BYTE_HIGH;
            break;

        case CPSS_PORT_INTERLAKEN_COUNTER_ID_TX_BAD_PKT_E:
            regAddr[0] = PRV_DXCH_REG1_UNIT_ILKN_MAC(devNum).ILKN_COUNT_TX_BAD_PKT_LOW;
            regAddr[1] = PRV_DXCH_REG1_UNIT_ILKN_MAC(devNum).ILKN_COUNT_TX_BAD_PKT_HIGH;
            break;

        case CPSS_PORT_INTERLAKEN_COUNTER_ID_TX_PKT_E:
            regAddr[0] = PRV_DXCH_REG1_UNIT_ILKN_MAC(devNum).ILKN_COUNT_TX_PKT_LOW;
            regAddr[1] = PRV_DXCH_REG1_UNIT_ILKN_MAC(devNum).ILKN_COUNT_TX_PKT_HIGH;
            break;

        case CPSS_PORT_INTERLAKEN_COUNTER_ID_RX_BURST_SIZE_ERR_E:
            regAddr[0] = PRV_DXCH_REG1_UNIT_ILKN_MAC(devNum).ILKN_COUNT_RX_BURST_SIZE_ERROR_L;
            regAddr[1] = PRV_DXCH_REG1_UNIT_ILKN_MAC(devNum).ILKN_COUNT_RX_BURST_SIZE_ERROR_H;
            break;

        case CPSS_PORT_INTERLAKEN_COUNTER_ID_RX_ALIGNMENT_ERR_E:
            regAddr[0] = PRV_DXCH_REG1_UNIT_ILKN_MAC(devNum).ILKN_COUNT_RX_ALIGNMENT_ERROR;
            break;

        case CPSS_PORT_INTERLAKEN_COUNTER_ID_RX_ALIGNMENT_FAIL_E:
            regAddr[0] = PRV_DXCH_REG1_UNIT_ILKN_MAC(devNum).ILKN_COUNT_RX_ALIGNMENT_FAILURE;
            break;

        default: 
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }


    /* clear result */
    counterPtr->l[0] = 0;
    counterPtr->l[1] = 0;

    for (i = 0; (i < regNum); i++)
    {
        rc = prvCpssHwPpReadRegBitMask(
            devNum, regAddr[i], regMask[i], &(counterPtr->l[i]));
        if (rc != GT_OK)
        {
            return rc;
        }
    }

    return GT_OK;
}

/*******************************************************************************
* cpssDxChPortInterlakenCounterGet
*
* DESCRIPTION:
*       Function gets counters of Port Interlaken interface.
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
*
* INPUTS:
*       devNum              - device number
*       portNum             - physical port number.
*       counterId           - counter Id
*       laneNum             - lane number - relevant only for
*                             BLK_TYPE_ERR, DIAG_CRC_ERR, WORD_SYNC_ERR
*                             (APPLICABLE RANGES: 0..7)
*
* OUTPUTS:
*       counterPtr   - (pointer to) 64-bit counter value
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong device
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_BAD_STATE             - on illegal state
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChPortInterlakenCounterGet
(
    IN  GT_U8                                   devNum,
    IN  GT_PHYSICAL_PORT_NUM                    portNum,
    IN  CPSS_PORT_INTERLAKEN_COUNTER_ID_ENT     counterId,
    IN  GT_U32                                  laneNum,
    OUT GT_U64                                  *counterPtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChPortInterlakenCounterGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, portNum, counterId, laneNum, counterPtr));

    rc = internal_cpssDxChPortInterlakenCounterGet(devNum, portNum, counterId, laneNum, counterPtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, portNum, counterId, laneNum, counterPtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

GT_STATUS prvCpssDxChPortIlknIFBWResolutionSet
(
    IN GT_U8    devNum,
    IN GT_U32   ilknIfBWMBps,
    IN GT_U32   channelSpeedResMbps,
    OUT GT_U32  *etsBwIFMbpsPtr          /* estimated BW of ilkn IF */
)
{
    GT_STATUS rc;
    GT_U32    estBwByResolutionMbps;
    GT_U32    maxIFbwMbps;

    if (channelSpeedResMbps == 0)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }
    estBwByResolutionMbps = PRV_CPSS_DXCH_BC2_PA_ILKN_TX_FIFO_MAP_TOTAL_FLDS_REG_CNS*channelSpeedResMbps;

    if (estBwByResolutionMbps < ilknIfBWMBps)
    {
        maxIFbwMbps = estBwByResolutionMbps;
    }
    else
    {
        maxIFbwMbps = ilknIfBWMBps;
    }
    rc = prvCpssDxChPortDynamicPAIlknSpeedResolutionSet(devNum,/*OUT*/channelSpeedResMbps,/*OUT*/maxIFbwMbps);
    if (GT_OK != rc)
    {
        return rc;
    }
    *etsBwIFMbpsPtr = maxIFbwMbps; 
    return GT_OK;
}


GT_STATUS prvCpssDxChPortIlknIFBWSet
(
    IN GT_U8    devNum,
    IN GT_U32   ilknIfBWGBps,
    OUT GT_U32 *maxIlknIFSpeedMbpsPtr
)
{
    GT_STATUS rc;
    GT_U32    channelSpeedResMbps;
    GT_U32    ilknIfBWMBps;
    GT_U32    ifCurEstBWMbps;
    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum, CPSS_CH1_E | CPSS_CH1_DIAMOND_E
                                            | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E
                                            | CPSS_LION_E | CPSS_LION2_E | CPSS_XCAT2_E);

    rc = prvCpssDxChPortDynamicPAIlknSpeedResolutionGet(devNum,&channelSpeedResMbps,&ifCurEstBWMbps);
    if (GT_OK != rc)
    {
        return rc;
    }

    ilknIfBWMBps = ilknIfBWGBps*1000;
    rc = prvCpssDxChPortIlknIFBWResolutionSet(devNum,ilknIfBWMBps,channelSpeedResMbps,/*OUT*/maxIlknIFSpeedMbpsPtr);
    if (GT_OK != rc)
    {
        return rc;
    }


    rc = prvCpssDxChPortDynamicPizzaArbiterIfUnitBWSet(devNum, 
                                                        CPSS_DXCH_PA_UNIT_ILKN_TX_FIFO_E,
                                                        ilknIfBWGBps);
    if (GT_OK != rc)
    {
        return rc;
    }
    return GT_OK;
}


/*******************************************************************************
* internal_cpssDxChPortIlknChannelSpeedResolutionSet
*
* DESCRIPTION:
*       Configure Interlaken channel speed resolution.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum                - physical device number.
*       channelSpeedResMbps - channel speed resolution in Mbps
*
* OUTPUTS:
*       maxIlknIFSpeedMbpsPtr - The total bandwidth the Interlaken channels will support with the given resolution (small resolution == small total bandwidth)
                                 
*
* RETURNS:
*       GT_OK             - on success
*       GT_BAD_PARAM      - on wrong parameter
*       GT_OUT_OF_RANGE   - speed out of range
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*   -  default resultion is 40Mbps
*   -  if resolution is  greater than IF bandwidth, GT_BAD_PARAM is returned
*   -  returns GT_BAD_PARAM if requested resolution is not compatible with any currently existing channels.
*      In that case user shall delete all channels that are incompatible with new resolution and
*      repeat the cpssDxChPortInterlakenChannelSpeedResultionSet();
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChPortIlknChannelSpeedResolutionSet
(
    IN  GT_U8    devNum,
    IN  GT_U32   channelSpeedResMbps,
    OUT GT_U32  *maxIlknIFSpeedMbpsPtr
)
{
    GT_STATUS rc;
    GT_U32    bwGbps;
    GT_U32    ifBWMbps;

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum, CPSS_CH1_E | CPSS_CH1_DIAMOND_E
                                            | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E
                                            | CPSS_LION_E | CPSS_LION2_E | CPSS_XCAT2_E);


    rc = prvCpssDxChPortDynamicPizzaArbiterIfUnitBWGet(devNum,CPSS_DXCH_PA_UNIT_ILKN_TX_FIFO_E,/*OUT*/&bwGbps);
    if (GT_OK != rc)
    {
        return rc;
    }

    ifBWMbps         = bwGbps*1000;
    rc = prvCpssDxChPortIlknIFBWResolutionSet(devNum,ifBWMbps,channelSpeedResMbps,/*OUT*/maxIlknIFSpeedMbpsPtr);
    if (GT_OK != rc)
    {
        return rc;
    }
    return GT_OK;
}

/*******************************************************************************
* cpssDxChPortIlknChannelSpeedResolutionSet
*
* DESCRIPTION:
*       Configure Interlaken channel speed resolution.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum                - physical device number.
*       channelSpeedResMbps - channel speed resolution in Mbps
*
* OUTPUTS:
*       maxIlknIFSpeedMbpsPtr - The total bandwidth the Interlaken channels will support with the given resolution (small resolution == small total bandwidth)
                                 
*
* RETURNS:
*       GT_OK             - on success
*       GT_BAD_PARAM      - on wrong parameter
*       GT_OUT_OF_RANGE   - speed out of range
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*   -  default resultion is 40Mbps
*   -  if resolution is  greater than IF bandwidth, GT_BAD_PARAM is returned
*   -  returns GT_BAD_PARAM if requested resolution is not compatible with any currently existing channels.
*      In that case user shall delete all channels that are incompatible with new resolution and
*      repeat the cpssDxChPortInterlakenChannelSpeedResultionSet();
*
*******************************************************************************/
GT_STATUS cpssDxChPortIlknChannelSpeedResolutionSet
(
    IN  GT_U8    devNum,
    IN  GT_U32   channelSpeedResMbps,
    OUT GT_U32  *maxIlknIFSpeedMbpsPtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChPortIlknChannelSpeedResolutionSet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, channelSpeedResMbps, maxIlknIFSpeedMbpsPtr));

    rc = internal_cpssDxChPortIlknChannelSpeedResolutionSet(devNum, channelSpeedResMbps, maxIlknIFSpeedMbpsPtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, channelSpeedResMbps, maxIlknIFSpeedMbpsPtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChPortIlknChannelSpeedResolutionGet
*
* DESCRIPTION:
*       Get Interlaken channel speed resolution and ilkn IF BW that will be supported.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum                - physical device number.
*
* OUTPUTS:
*       speedResulutionMBpsPtr - channel speed resolution in Mbps
*       maxIlknIFSpeedMbpsPtr  - The total bandwidth the Interlaken channels will support with the given resolution (small resolution == small total bandwidth)
*
* RETURNS:
*       GT_OK             - on success
*       GT_BAD_PARAM      - on wrong parameter
*       GT_OUT_OF_RANGE   - speed out of range
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChPortIlknChannelSpeedResolutionGet
(
    IN  GT_U8    devNum,
    OUT GT_U32  *speedResulutionMBpsPtr,
    OUT GT_U32  *maxIlknIFSpeedMbpsPtr	
)
{
    GT_STATUS rc;

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum, CPSS_CH1_E | CPSS_CH1_DIAMOND_E
                                            | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E
                                            | CPSS_LION_E | CPSS_LION2_E | CPSS_XCAT2_E);

    CPSS_NULL_PTR_CHECK_MAC(speedResulutionMBpsPtr);
    CPSS_NULL_PTR_CHECK_MAC(maxIlknIFSpeedMbpsPtr);

    rc = prvCpssDxChPortDynamicPAIlknSpeedResolutionGet(devNum,/*OUT*/speedResulutionMBpsPtr,/*OUT*/maxIlknIFSpeedMbpsPtr);
    if (GT_OK != rc)
    {
        return rc;
    }
    return GT_OK;
}

/*******************************************************************************
* cpssDxChPortIlknChannelSpeedResolutionGet
*
* DESCRIPTION:
*       Get Interlaken channel speed resolution and ilkn IF BW that will be supported.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum                - physical device number.
*
* OUTPUTS:
*       speedResulutionMBpsPtr - channel speed resolution in Mbps
*       maxIlknIFSpeedMbpsPtr  - The total bandwidth the Interlaken channels will support with the given resolution (small resolution == small total bandwidth)
*
* RETURNS:
*       GT_OK             - on success
*       GT_BAD_PARAM      - on wrong parameter
*       GT_OUT_OF_RANGE   - speed out of range
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChPortIlknChannelSpeedResolutionGet
(
    IN  GT_U8    devNum,
    OUT GT_U32  *speedResulutionMBpsPtr,
    OUT GT_U32  *maxIlknIFSpeedMbpsPtr	
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChPortIlknChannelSpeedResolutionGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, speedResulutionMBpsPtr, maxIlknIFSpeedMbpsPtr));

    rc = internal_cpssDxChPortIlknChannelSpeedResolutionGet(devNum, speedResulutionMBpsPtr, maxIlknIFSpeedMbpsPtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, speedResulutionMBpsPtr, maxIlknIFSpeedMbpsPtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}


/*******************************************************************************
* internal_cpssDxChPortIlknChannelSpeedSet
*
* DESCRIPTION:
*       Configure Interlaken channel speed.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum      - physical device number.
*       portNum     - physical port number of channel.
*       direction   - TX/RX/Both.
*       speed       - channel speed in Mbps, resolution depends on ASIC. Use 0 to
*                       free channel resources.
*
* OUTPUTS:
*       actualSpeedPtr - because HW constrains dectate granularity, real TX speed
*                       could differ from requested. Application must use this
*                       value to know how much of general ILKN interface bandwidth
*                       remain free for TX.
*
* RETURNS:
*       GT_OK             - on success
*       GT_BAD_PARAM      - on wrong parameter
*       GT_NOT_INITIALIZED - ILKN interface not configured properly
*       GT_OUT_OF_RANGE   - speed out of range
*       GT_NO_RESOURCE    - no bandwidth to supply channel speed
*       GT_HW_ERROR       - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*   - Could be engaged only after cpssDxChPortModeSpeedSet pass.
*   - the speed shall be defined in MBps in quantum of channel speed resolution, provided by 
*     cpssDxChPortIlknChannelSpeedResolutionSet()
*   - RX speed reconfiguration can cause fragmentation in Packet Reassembly memory
*       and GT_NO_RESOURCE error when actually there is enough free space, to solve
*       this application will have delete all or part of channels and configure them
*       again in one sequence.
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChPortIlknChannelSpeedSet
(
    IN  GT_U8                   devNum,
    IN  GT_PHYSICAL_PORT_NUM    portNum,
    IN  CPSS_PORT_DIRECTION_ENT direction,
    IN  GT_U32                  speed,
    OUT GT_U32                  *actualSpeedPtr
)
{
    GT_STATUS   rc;         /* return code */
    GT_U32      ilknChannel;/* ILKN channel Id */
    GT_U32      channelOffset, /* offset of channel memory in shared reassembly memory */
                channelThreshold; /* size of a region, allocated to a specific channel
                                    in shared reassembly memory */
    GT_U32      totalIfBw;  /* total ILKN interface bandwidth in Mbps */
#ifndef GM_USED
    GT_U32      txSpeed;    /* TX channel speed alligned by 40Mbps */
#endif
    GT_U32      speedResolutionMbps;/* resolution for speed accordingly to pizza abilities */
    GT_U32      maxIFbwMbps;/* maximal bandwidth of configured interface in Mbps */
    GT_BOOL     enable;     /* was channel enabled */
    PRV_CPSS_PORT_INFO_ARRAY_STC * portInfoPtr; /* ptr to current port state info */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum, CPSS_CH1_E | CPSS_CH1_DIAMOND_E 
                                          | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E 
                                          | CPSS_XCAT3_E | CPSS_LION_E | CPSS_XCAT2_E 
                                          | CPSS_LION2_E   );

    rc = prvCpssDxChPortPhysicalPortMapCheckAndConvert(devNum, portNum, 
                                       PRV_CPSS_DXCH_PORT_TYPE_ILKN_CHANNEL_E, 
                                       &ilknChannel);
    if(GT_OK != rc)
    {
        return rc;
    }

    if(CPSS_DXCH_PORT_MAPPING_INVALID_PORT_CNS == ilknChannel)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    totalIfBw = 0; /* to prevent warning "potentially uninitialized" */
    if (speed > 0)
    {
        CPSS_NULL_PTR_CHECK_MAC(actualSpeedPtr);

        rc = prvCpssDxChPortDynamicPAIlknSpeedResolutionGet(devNum, 
                                                            &speedResolutionMbps, 
                                                            &maxIFbwMbps);
        if(GT_OK != rc)
        {
            return rc;
        }

        if (0 != (speed % speedResolutionMbps))
        {/* not in quantum of speedResolutionMbps */
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
        }

        portInfoPtr = &PRV_CPSS_PP_MAC(devNum)->phyPortInfoArray
                                            [PRV_CPSS_DXCH_BOBCAT2_ILKN_MAC_CNS];
        if(   portInfoPtr->portIfMode != CPSS_PORT_INTERFACE_MODE_ILKN4_E 
           && portInfoPtr->portIfMode != CPSS_PORT_INTERFACE_MODE_ILKN8_E)
        {
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_INITIALIZED, LOG_ERROR_NO_MSG);
        }

        if( portInfoPtr->portSpeed != CPSS_PORT_SPEED_20000_E &&
            portInfoPtr->portSpeed != CPSS_PORT_SPEED_40000_E)
        {
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_INITIALIZED, LOG_ERROR_NO_MSG);
        }

        totalIfBw = (CPSS_PORT_SPEED_20000_E == portInfoPtr->portSpeed) ? 
                                                                    20000 : 40000;
        if(speed > totalIfBw)
        {
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_OUT_OF_RANGE, LOG_ERROR_NO_MSG);
        }

        rc = prvCpssDxChPortDynamicPizzaArbiterIfIlknChannelCheckSupport(devNum, 
                                                                         portNum, 
                                                                         speed);
        if (GT_OK != rc)
        {
            return rc;
        }
    }

    switch(direction)
    {
        case CPSS_PORT_DIRECTION_BOTH_E:
        case CPSS_PORT_DIRECTION_RX_E:
        {
            GT_U32  regAddr;
            GT_U32  i;
            PRV_CPSS_DXCH_PP_REGS_ADDR_VER1_STC *regsAddrPtr =
                                        PRV_CPSS_DXCH_DEV_REGS_VER1_MAC(devNum);

            if(speed > 0)
            {
                for(i = 0; i < PRV_CPSS_DXCH_BOBCAT2_ILKN_CHANNEL_MAX_CNS*2; i++)
                {
                    if(prMemoryChunksArray[i].inUse)
                    {
                        if(prMemoryChunksArray[i].channelId == ilknChannel)
                        {
                            rc = cpssDxChPortIlknChannelEnableGet(devNum,
                                                                  portNum,
                                                                  CPSS_PORT_DIRECTION_RX_E,
                                                                  &enable);
                            if(rc != GT_OK)
                            {
                                return rc;
                            }
                            if(enable)
                            {/* can't change speed of channel while it's enabled 
                                and can pass traffic */
                                CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_STATE, LOG_ERROR_NO_MSG);
                            }
                            else
                            {
                                rc = prvCpssDxChPortIlknPrChunkFree(ilknChannel);
                                if(rc != GT_OK)
                                {
                                    return rc;
                                }
                            }
                        }
                    }
                }

                channelThreshold = 160 + ((speed*10080+(totalIfBw-1))/totalIfBw);
                rc = prvCpssDxChPortIlknPrChunkAlloc(ilknChannel, channelThreshold, 
                                                     &channelOffset);
                if(rc != GT_OK)
                {
                    return rc;
                }
                *actualSpeedPtr = (((channelThreshold - 160)*totalIfBw)+10079)/10080;
            }
            else
            {
                channelOffset = 0;
                channelThreshold = 0;
                rc = prvCpssDxChPortIlknPrChunkFree(ilknChannel);
                if(rc != GT_OK)
                {
                    return rc;
                }
            }
            regAddr = regsAddrPtr->GOP.PR.PRFunctional.channelConfig[ilknChannel];
            rc = prvCpssDrvHwPpPortGroupSetRegField(devNum,
                                                    CPSS_PORT_GROUP_UNAWARE_MODE_CNS,
                                                    regAddr, 0, 30, 
                                                    (channelOffset | (channelThreshold<<15)));
            if(rc != GT_OK)
            {
                return rc;
            }
        }

        if(direction != CPSS_PORT_DIRECTION_BOTH_E)
        {
            break;
        }            

        case CPSS_PORT_DIRECTION_TX_E:
#ifndef GM_USED
            if(speed != 0)
            {
                /* GT_U32 granularity;

                granularity = (CPSS_PORT_SPEED_20000_E == PRV_CPSS_DXCH_PORT_SPEED_MAC(devNum, 
                     PRV_CPSS_DXCH_BOBCAT2_ILKN_MAC_CNS)) ? 20 : 40;
                txSpeed = ((speed%granularity) == 0) ? speed : 
                    (speed-(speed%granularity)+granularity);
                    */
                txSpeed = speed;
                rc = prvCpssDxChPortDynamicPizzaArbiterIfIlknChannelConfigure(devNum, portNum,txSpeed);
                if (rc != GT_OK)
                {
                    return rc;
                }
                *actualSpeedPtr = txSpeed;
            }
            else
            {
				rc = prvCpssDxChPortDynamicPizzaArbiterIfIlknChannelConfigure(devNum, portNum,0);
                if (rc != GT_OK)
                {
                    return rc;
                }
            }
#endif 
            break;

        default:
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    if((0 == speed) && (actualSpeedPtr != NULL))
    {
        *actualSpeedPtr = 0;
    }

    return GT_OK;
}

/*******************************************************************************
* cpssDxChPortIlknChannelSpeedSet
*
* DESCRIPTION:
*       Configure Interlaken channel speed.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum      - physical device number.
*       portNum     - physical port number of channel.
*       direction   - TX/RX/Both.
*       speed       - channel speed in Mbps, resolution depends on ASIC. Use 0 to
*                       free channel resources.
*
* OUTPUTS:
*       actualSpeedPtr - because HW constrains dectate granularity, real TX speed
*                       could differ from requested. Application must use this
*                       value to know how much of general ILKN interface bandwidth
*                       remain free for TX.
*
* RETURNS:
*       GT_OK             - on success
*       GT_BAD_PARAM      - on wrong parameter
*       GT_NOT_INITIALIZED - ILKN interface not configured properly
*       GT_OUT_OF_RANGE   - speed out of range
*       GT_NO_RESOURCE    - no bandwidth to supply channel speed
*       GT_HW_ERROR       - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*   - Could be engaged only after cpssDxChPortModeSpeedSet pass.
*   - the speed shall be defined in MBps in quantum of channel speed resolution, provided by 
*     cpssDxChPortIlknChannelSpeedResolutionSet()
*   - RX speed reconfiguration can cause fragmentation in Packet Reassembly memory
*       and GT_NO_RESOURCE error when actually there is enough free space, to solve
*       this application will have delete all or part of channels and configure them
*       again in one sequence.
*
*******************************************************************************/
GT_STATUS cpssDxChPortIlknChannelSpeedSet
(
    IN  GT_U8                   devNum,
    IN  GT_PHYSICAL_PORT_NUM    portNum,
    IN  CPSS_PORT_DIRECTION_ENT direction,
    IN  GT_U32                  speed,
    OUT GT_U32                  *actualSpeedPtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChPortIlknChannelSpeedSet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, portNum, direction, speed, actualSpeedPtr));

    rc = internal_cpssDxChPortIlknChannelSpeedSet(devNum, portNum, direction, speed, actualSpeedPtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, portNum, direction, speed, actualSpeedPtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChPortIlknChannelSpeedGet
*
* DESCRIPTION:
*       Get Interlaken channel speed.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum      - physical device number.
*       portNum     - physical port number of channel.
*       direction   - TX/RX. Attention: Both not supported!
*
* OUTPUTS:
*       speedPtr    - actual channel speed in Mbps.
*
* RETURNS:
*       GT_OK             - on success
*       GT_BAD_PARAM      - on wrong parameter
*       GT_BAD_PTR        - speedPtr == NULL
*       GT_NOT_INITIALIZED - ILKN interface not configured properly
*       GT_HW_ERROR       - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*   In Bobcat2 ILKN_TXFIFO allows 40Mbps granularity of TX channel speed if
*    bandwidth of ILKN channel is 40Gbps and 20Mbps if total BW is 20Gbps.
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChPortIlknChannelSpeedGet
(
    IN  GT_U8                   devNum,
    IN  GT_PHYSICAL_PORT_NUM    portNum,
    IN  CPSS_PORT_DIRECTION_ENT direction,
    OUT GT_U32                  *speedPtr
)
{
    GT_STATUS   rc;         /* return code */
    GT_U32      ilknChannel;/* ILKN channel Id */
    GT_U32      channelThreshold; /* size of a region, allocated to a specific channel
                                    in shared reassembly memory */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum, CPSS_CH1_E | CPSS_CH1_DIAMOND_E 
                                          | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E 
                                          | CPSS_LION_E | CPSS_XCAT2_E | 
                                          CPSS_LION2_E   );
    CPSS_NULL_PTR_CHECK_MAC(speedPtr);

    rc = prvCpssDxChPortPhysicalPortMapCheckAndConvert(devNum, portNum, 
                                       PRV_CPSS_DXCH_PORT_TYPE_ILKN_CHANNEL_E, 
                                       &ilknChannel);
    if(GT_OK != rc)
    {
        return rc;
    }

    if(CPSS_DXCH_PORT_MAPPING_INVALID_PORT_CNS == ilknChannel)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    if((PRV_CPSS_DXCH_PORT_IFMODE_MAC(devNum, PRV_CPSS_DXCH_BOBCAT2_ILKN_MAC_CNS)
        != CPSS_PORT_INTERFACE_MODE_ILKN4_E) && 
       (PRV_CPSS_DXCH_PORT_IFMODE_MAC(devNum, PRV_CPSS_DXCH_BOBCAT2_ILKN_MAC_CNS)
        != CPSS_PORT_INTERFACE_MODE_ILKN8_E))
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_INITIALIZED, LOG_ERROR_NO_MSG);
    }

    if((PRV_CPSS_DXCH_PORT_SPEED_MAC(devNum, PRV_CPSS_DXCH_BOBCAT2_ILKN_MAC_CNS)
        != CPSS_PORT_SPEED_20000_E) &&
       (PRV_CPSS_DXCH_PORT_SPEED_MAC(devNum, PRV_CPSS_DXCH_BOBCAT2_ILKN_MAC_CNS)
        != CPSS_PORT_SPEED_40000_E))
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_INITIALIZED, LOG_ERROR_NO_MSG);
    }

    switch(direction)
    {
        case CPSS_PORT_DIRECTION_RX_E:
        {
            GT_U32  regAddr;
            PRV_CPSS_DXCH_PP_REGS_ADDR_VER1_STC *regsAddrPtr =
                                        PRV_CPSS_DXCH_DEV_REGS_VER1_MAC(devNum);

            regAddr = regsAddrPtr->GOP.PR.PRFunctional.channelConfig[ilknChannel];
            rc = prvCpssDrvHwPpPortGroupGetRegField(devNum,
                                                    CPSS_PORT_GROUP_UNAWARE_MODE_CNS,
                                                    regAddr, 15, 15, 
                                                    &channelThreshold);
            if(rc != GT_OK)
            {
                return rc;
            }

            if(channelThreshold != 0)
            {
                GT_U32 totalIfBw;

                /* translate channel memory threshold to Mbps */
                totalIfBw = (CPSS_PORT_SPEED_20000_E == PRV_CPSS_DXCH_PORT_SPEED_MAC(devNum, 
                                 PRV_CPSS_DXCH_BOBCAT2_ILKN_MAC_CNS)) ? 20000 : 40000;
                *speedPtr = (((channelThreshold - 160)*totalIfBw)+10079)/10080;
            }
            else
            {
                *speedPtr = 0;
            }
        }

        break;

        case CPSS_PORT_DIRECTION_TX_E:
            {
                GT_BOOL isInitilized;
                rc = prvCpssDxChPortDynamicPAIlknChannelSpeedDBGet(devNum, portNum,
                                                                   &isInitilized,
                                                                   speedPtr);
            }

            break;

        default:
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    return GT_OK;
}

/*******************************************************************************
* cpssDxChPortIlknChannelSpeedGet
*
* DESCRIPTION:
*       Get Interlaken channel speed.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum      - physical device number.
*       portNum     - physical port number of channel.
*       direction   - TX/RX. Attention: Both not supported!
*
* OUTPUTS:
*       speedPtr    - actual channel speed in Mbps.
*
* RETURNS:
*       GT_OK             - on success
*       GT_BAD_PARAM      - on wrong parameter
*       GT_BAD_PTR        - speedPtr == NULL
*       GT_NOT_INITIALIZED - ILKN interface not configured properly
*       GT_HW_ERROR       - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*   In Bobcat2 ILKN_TXFIFO allows 40Mbps granularity of TX channel speed if
*    bandwidth of ILKN channel is 40Gbps and 20Mbps if total BW is 20Gbps.
*
*******************************************************************************/
GT_STATUS cpssDxChPortIlknChannelSpeedGet
(
    IN  GT_U8                   devNum,
    IN  GT_PHYSICAL_PORT_NUM    portNum,
    IN  CPSS_PORT_DIRECTION_ENT direction,
    OUT GT_U32                  *speedPtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChPortIlknChannelSpeedGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, portNum, direction, speedPtr));

    rc = internal_cpssDxChPortIlknChannelSpeedGet(devNum, portNum, direction, speedPtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, portNum, direction, speedPtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

GT_STATUS cpssDxChPortIlknChannelSpeedSetDbg
(
    IN  GT_U8                   devNum,
    IN  GT_PHYSICAL_PORT_NUM    portNum,
    IN  CPSS_PORT_DIRECTION_ENT direction,
    IN  GT_U32                  speed    
)
{
    GT_U32  actualSpeed;
    GT_STATUS rc;

    rc = cpssDxChPortIlknChannelSpeedSet(devNum, portNum, direction, speed,
                                         &actualSpeed);

    cpssOsPrintf("rc=%d,actualSpeed=%d\n", rc, actualSpeed);

    return rc;
}

GT_STATUS cpssDxChPortIlknChannelSpeedGetDbg
(
    IN  GT_U8                   devNum,
    IN  GT_PHYSICAL_PORT_NUM    portNum,
    IN  CPSS_PORT_DIRECTION_ENT direction
)
{
    GT_U32  actualSpeed;
    GT_STATUS rc;

    rc = cpssDxChPortIlknChannelSpeedGet(devNum, portNum, direction, &actualSpeed);

    cpssOsPrintf("rc=%d,actualSpeed=%d\n", rc, actualSpeed);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChPortIlknChannelEnableSet
*
* DESCRIPTION:
*       Enable/disable Interlaken channel.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum      - physical device number.
*       portNum     - physical port number of channel.
*       direction   - TX/RX/Both.
*       enable      -   GT_TRUE - "direction" of channel enabled;
*                       GT_FALSE- "direction" channel disabled.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK             - on success
*       GT_BAD_PARAM      - on wrong parameter
*       GT_NOT_INITIALIZED - channel speed not defined yet
*       GT_HW_ERROR       - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       Pay attention: can't enable channel which speed not configured yet.
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChPortIlknChannelEnableSet
(
    IN  GT_U8                   devNum,
    IN  GT_PHYSICAL_PORT_NUM    portNum,
    IN  CPSS_PORT_DIRECTION_ENT direction,
    IN  GT_BOOL                 enable
)
{
    GT_STATUS   rc;         /* return code */
    GT_U32      ilknChannel;/* ILKN channel Id */
    GT_U32      regAddr;    /* address of register */
    GT_U32      chSpeed;    /* speed of channel in given direction */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum, CPSS_CH1_E | CPSS_CH1_DIAMOND_E 
                                          | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E 
                                          | CPSS_LION_E | CPSS_XCAT2_E | 
                                          CPSS_LION2_E   );

    rc = prvCpssDxChPortPhysicalPortMapCheckAndConvert(devNum, portNum, 
                                       PRV_CPSS_DXCH_PORT_TYPE_ILKN_CHANNEL_E, 
                                       &ilknChannel);
    if(GT_OK != rc)
    {
        return rc;
    }

    if(CPSS_DXCH_PORT_MAPPING_INVALID_PORT_CNS == ilknChannel)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }
    
    if(enable)
    {
        if((CPSS_PORT_DIRECTION_BOTH_E == direction) || (CPSS_PORT_DIRECTION_RX_E == 
                                                                        direction))
        {
            rc = cpssDxChPortIlknChannelSpeedGet(devNum, portNum,
                                                 CPSS_PORT_DIRECTION_RX_E,
                                                 &chSpeed);
            if(rc != GT_OK)
            {
                return rc;
            }
    
            if(0 == chSpeed)
            {
                CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_INITIALIZED, LOG_ERROR_NO_MSG);
            }
        }
    
        if((CPSS_PORT_DIRECTION_BOTH_E == direction) || (CPSS_PORT_DIRECTION_TX_E == 
                                                                        direction))
        {
            rc = cpssDxChPortIlknChannelSpeedGet(devNum, portNum,
                                                 CPSS_PORT_DIRECTION_TX_E,
                                                 &chSpeed);
            if(rc != GT_OK)
            {
                return rc;
            }
    
            if(0 == chSpeed)
            {
                CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_INITIALIZED, LOG_ERROR_NO_MSG);
            }
        }
    }

    switch(direction)
    {
        case CPSS_PORT_DIRECTION_BOTH_E:
        case CPSS_PORT_DIRECTION_RX_E:
            regAddr = PRV_CPSS_DXCH_DEV_REGS_VER1_MAC(devNum)->GOP.PR.PRFunctional.
                                                    channelConfig[ilknChannel];
        
            rc = prvCpssDrvHwPpPortGroupSetRegField(devNum,
                                                CPSS_PORT_GROUP_UNAWARE_MODE_CNS,
                                                regAddr, 31, 1, BOOL2BIT_MAC(!enable));
            if(GT_OK != rc)
            {
                return rc;
            }

        if(direction != CPSS_PORT_DIRECTION_BOTH_E)
        {
            break;
        }            

        case CPSS_PORT_DIRECTION_TX_E:
            regAddr = PRV_CPSS_DXCH_DEV_REGS_VER1_MAC(devNum)->SIP_ILKN_TXFIFO.
                                ilknTxFIFOGlobalConfig.portChannelEnable[ilknChannel];
            rc = prvCpssDrvHwPpPortGroupSetRegField(devNum,
                                                CPSS_PORT_GROUP_UNAWARE_MODE_CNS,
                                                regAddr, 0, 1, BOOL2BIT_MAC(enable));
            if(GT_OK != rc)
            {
                return rc;
            }
            break;

        default:
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    return GT_OK;
}

/*******************************************************************************
* cpssDxChPortIlknChannelEnableSet
*
* DESCRIPTION:
*       Enable/disable Interlaken channel.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum      - physical device number.
*       portNum     - physical port number of channel.
*       direction   - TX/RX/Both.
*       enable      -   GT_TRUE - "direction" of channel enabled;
*                       GT_FALSE- "direction" channel disabled.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK             - on success
*       GT_BAD_PARAM      - on wrong parameter
*       GT_NOT_INITIALIZED - channel speed not defined yet
*       GT_HW_ERROR       - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       Pay attention: can't enable channel which speed not configured yet.
*
*******************************************************************************/
GT_STATUS cpssDxChPortIlknChannelEnableSet
(
    IN  GT_U8                   devNum,
    IN  GT_PHYSICAL_PORT_NUM    portNum,
    IN  CPSS_PORT_DIRECTION_ENT direction,
    IN  GT_BOOL                 enable
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChPortIlknChannelEnableSet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, portNum, direction, enable));

    rc = internal_cpssDxChPortIlknChannelEnableSet(devNum, portNum, direction, enable);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, portNum, direction, enable));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChPortIlknChannelEnableGet
*
* DESCRIPTION:
*       Get Enable/disable state of Interlaken channel.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum      - physical device number.
*       portNum     - physical port number of channel.
*       direction   - TX/RX. Attention: Both not supported.
*
* OUTPUTS:
*       enablePtr   -   GT_TRUE - "direction" of channel enabled;
*                       GT_FALSE- "direction" channel disabled.
*
* RETURNS:
*       GT_OK             - on success
*       GT_BAD_PARAM      - on wrong parameter
*       GT_NO_RESOURCE    - no bandwidth to supply channel speed
*       GT_HW_ERROR       - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChPortIlknChannelEnableGet
(
    IN  GT_U8                   devNum,
    IN  GT_PHYSICAL_PORT_NUM    portNum,
    IN  CPSS_PORT_DIRECTION_ENT direction,
    OUT GT_BOOL                 *enablePtr
)
{
    GT_STATUS   rc;         /* return code */
    GT_U32      ilknChannel;/* ILKN channel Id */
    GT_U32      regAddr;    /* address of register */
    GT_U32      value;      /* enable field value */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum, CPSS_CH1_E | CPSS_CH1_DIAMOND_E 
                                          | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E 
                                          | CPSS_LION_E | CPSS_XCAT2_E | 
                                          CPSS_LION2_E   );
    CPSS_NULL_PTR_CHECK_MAC(enablePtr);

    rc = prvCpssDxChPortPhysicalPortMapCheckAndConvert(devNum, portNum, 
                                       PRV_CPSS_DXCH_PORT_TYPE_ILKN_CHANNEL_E, 
                                       &ilknChannel);
    if(GT_OK != rc)
    {
        return rc;
    }

    if(CPSS_DXCH_PORT_MAPPING_INVALID_PORT_CNS == ilknChannel)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }
    
    switch(direction)
    {
        case CPSS_PORT_DIRECTION_RX_E:
            regAddr = PRV_CPSS_DXCH_DEV_REGS_VER1_MAC(devNum)->GOP.PR.PRFunctional.
                                                                channelConfig[ilknChannel];
        
            rc = prvCpssDrvHwPpPortGroupGetRegField(devNum,
                                                CPSS_PORT_GROUP_UNAWARE_MODE_CNS,
                                                regAddr, 31, 1, &value);
            if(GT_OK != rc)
            {
                return rc;
            }

            *enablePtr = !BIT2BOOL_MAC(value);

            break;

        case CPSS_PORT_DIRECTION_TX_E:
            regAddr = PRV_CPSS_DXCH_DEV_REGS_VER1_MAC(devNum)->SIP_ILKN_TXFIFO.
                                ilknTxFIFOGlobalConfig.portChannelEnable[ilknChannel];
            rc = prvCpssDrvHwPpPortGroupGetRegField(devNum,
                                                CPSS_PORT_GROUP_UNAWARE_MODE_CNS,
                                                regAddr, 0, 1, &value);
            if(GT_OK != rc)
            {
                return rc;
            }

            *enablePtr = BIT2BOOL_MAC(value);

            break;

        default:
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    return GT_OK;
}

/*******************************************************************************
* cpssDxChPortIlknChannelEnableGet
*
* DESCRIPTION:
*       Get Enable/disable state of Interlaken channel.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum      - physical device number.
*       portNum     - physical port number of channel.
*       direction   - TX/RX. Attention: Both not supported.
*
* OUTPUTS:
*       enablePtr   -   GT_TRUE - "direction" of channel enabled;
*                       GT_FALSE- "direction" channel disabled.
*
* RETURNS:
*       GT_OK             - on success
*       GT_BAD_PARAM      - on wrong parameter
*       GT_NO_RESOURCE    - no bandwidth to supply channel speed
*       GT_HW_ERROR       - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChPortIlknChannelEnableGet
(
    IN  GT_U8                   devNum,
    IN  GT_PHYSICAL_PORT_NUM    portNum,
    IN  CPSS_PORT_DIRECTION_ENT direction,
    OUT GT_BOOL                 *enablePtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChPortIlknChannelEnableGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, portNum, direction, enablePtr));

    rc = internal_cpssDxChPortIlknChannelEnableGet(devNum, portNum, direction, enablePtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, portNum, direction, enablePtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}


GT_STATUS ilknDbgPrint
(
    GT_U8 devNum
)
{
    GT_STATUS   rc;
    GT_U32  regAddr, value;
    GT_U32  ilknChannel;
    PRV_CPSS_DXCH_PP_REGS_ADDR_VER1_STC *regsAddrPtr =
                                PRV_CPSS_DXCH_DEV_REGS_VER1_MAC(devNum);

    cpssOsPrintf("channel config:\n");
    cpssOsPrintf("regAddr channelOffset channelThreshold rxEnable, txEnable(txEnRegAddr)\n");
    for(ilknChannel = 0; ilknChannel < 64; ilknChannel++)
    {
        regAddr = regsAddrPtr->GOP.PR.PRFunctional.channelConfig[ilknChannel];
        rc = prvCpssDrvHwPpPortGroupReadRegister(devNum,
                                                CPSS_PORT_GROUP_UNAWARE_MODE_CNS,
                                                regAddr,
                                                &value);
        if(rc != GT_OK)
        {
            return rc;
        }
        cpssOsPrintf("0x%x 0x%x 0x%x %d ", regAddr, U32_GET_FIELD_MAC(value, 0, 15), 
                                            U32_GET_FIELD_MAC(value, 15, 15), 
                                !(BIT2BOOL_MAC(U32_GET_FIELD_MAC(value, 31, 1))));

        regAddr = PRV_CPSS_DXCH_DEV_REGS_VER1_MAC(devNum)->SIP_ILKN_TXFIFO.
                            ilknTxFIFOGlobalConfig.portChannelEnable[ilknChannel];
        rc = prvCpssDrvHwPpPortGroupGetRegField(devNum,
                                            CPSS_PORT_GROUP_UNAWARE_MODE_CNS,
                                            regAddr, 0, 1, &value);
        if(GT_OK != rc)
        {
            return rc;
        }
        cpssOsPrintf("%d(0x%x)\n", value, regAddr);
    }

    return  GT_OK;
}


GT_STATUS dbgInterlakenRegSet
(
    IN GT_U8  devNum,
    IN GT_U32 portGroup,
    IN GT_U32 regAddr,
    IN GT_U32 mask,
    IN GT_U32 data
)
{
    return genInterlakenRegSet(devNum, portGroup, regAddr, data, mask);
}

GT_STATUS dbgInterlakenRegGet
(
    IN GT_U8  devNum,
    IN GT_U32 portGroup,
    IN GT_U32 regAddr,
    IN GT_U32 mask
)
{
    GT_STATUS rc;
    GT_U32 data;


    rc = genInterlakenRegGet(devNum, portGroup, regAddr, &data, mask);
    if(GT_OK != rc)
    {
        return rc;
    }

    cpssOsPrintf("cpssExMxPmPortInterlakenRegGet: data = 0x%x\r\n", data);

    return GT_OK;
}

