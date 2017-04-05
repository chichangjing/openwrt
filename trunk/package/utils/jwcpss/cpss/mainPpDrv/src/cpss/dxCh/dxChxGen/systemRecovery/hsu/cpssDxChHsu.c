/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* cpssDxChHsu.c
*
* DESCRIPTION:
*       CPSS DxCh HSU facility API.
*
* FILE REVISION NUMBER:
*       $Revision: 16 $
*******************************************************************************/
#define CPSS_LOG_IN_MODULE_ENABLE
#include <cpss/dxCh/dxChxGen/systemRecovery/hsu/cpssDxChHsu.h>
#include <cpss/dxCh/dxChxGen/systemRecovery/hsu/private/prvCpssDxChHsu.h>
#include <cpssCommon/cpssPresteraDefs.h>
#include <cpss/dxCh/dxChxGen/config/private/prvCpssDxChInfo.h>
#include <cpss/dxCh/dxChxGen/ipLpmEngine/cpssDxChIpLpm.h>
#include <cpss/dxCh/dxChxGen/ipLpmEngine/private/cpssDxChPrvIpLpm.h>
#include <cpssDriver/pp/config/prvCpssDrvPpCfgInit.h>
#include <cpss/dxCh/dxChxGen/bridge/cpssDxChBrgFdb.h>
#include <cpss/dxCh/dxChxGen/networkIf/cpssDxChNetIf.h>
#include <cpss/dxCh/dxChxGen/bridge/private/prvCpssDxChBrgFdbAu.h>
#include <cpss/dxCh/dxChxGen/cpssHwInit/private/prvCpssDxChHwInit.h>
#include <cpss/dxCh/dxChxGen/systemRecovery/private/prvCpssDxChSystemRecoveryLog.h>

static PRV_CPSS_DXCH_HSU_MAIN_FRAME_ITERATOR_STC *frameIterPtr = NULL;
static PRV_CPSS_DXCH_HSU_LPM_DB_ITERATOR_STC     *ipLpmDbsIter = NULL;

/* bitmap of SERDES lanes (bit 0-> lane 0, etc.) where values are not NULL
   serdesLanesBmp[0] - lanes 0-31
   serdesLanesBmp[1] - lanes 32-35 */
static GT_U32 serdesLanesBmp[2] = {0,0};

/*******************************************************************************
* prvCpssDxChHsuExportImportDataHandling
*
* DESCRIPTION:
*       This function handle import/export data.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       actionType             - PRV_CPSS_DXCH_HSU_EXPORT_E - export action
*                                PRV_CPSS_DXCH_HSU_IMPORT_E - import action
*       currentIterPtr         - points to the current iteration.
*       hsuBlockMemSizePtr     - pointer hsu block data size supposed to be exported
*                                in current iteration.
*       hsuBlockMemPtrPtr      - pointer to HSU survived restart memory area
*
* OUTPUTS:
*       currentIterPtr         - points to the current iteration
*       hsuBlockMemSizePtr     - pointer to hsu block data size exported in current iteration.
*       hsuBlockMemPtrPtr      - pointer to HSU survived restart memory area
*       accumSizePtr           - points to accumulated size
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong hsuBlockMemSize, wrong iterator.
*       GT_BAD_PTR               - on NULL pointer
*       GT_FAIL                  - otherwise
*
* COMMENTS:
*       none.
*
*******************************************************************************/
GT_STATUS prvCpssDxChHsuExportImportDataHandling
(
    IN     PRV_CPSS_HSU_ACTION_TYPE_ENT            actionType,
    INOUT  PRV_CPSS_DXCH_HSU_GLOBAL_ITERATOR_STC   *currentIterPtr,
    INOUT  GT_U32                                  *hsuBlockMemSizePtr,
    INOUT  GT_U8                                   **hsuBlockMemPtrPtr,
    OUT    GT_U32                                  *accumSizePtr
)
{
    GT_PTR dst;
    GT_PTR src;
    if ((currentIterPtr->genIterator.currStageMemPtr == NULL) ||
        (currentIterPtr->genIterator.currentStageRemainedSize == 0))
    {
        /* it means that this stage is not needed in current version and */
        /* should be omitted                                             */
        currentIterPtr->currStage++;
        return GT_OK;
    }
    if (actionType == PRV_CPSS_HSU_EXPORT_E)
    {
        dst = *hsuBlockMemPtrPtr;
        src = currentIterPtr->genIterator.currStageMemPtr;
    }
    else
    {
        dst = currentIterPtr->genIterator.currStageMemPtr;
        src = *hsuBlockMemPtrPtr;
    }
    if (*hsuBlockMemSizePtr >= currentIterPtr->genIterator.currentStageRemainedSize)
    {
        cpssOsMemCpy(dst,src,currentIterPtr->genIterator.currentStageRemainedSize);
        *hsuBlockMemSizePtr = *hsuBlockMemSizePtr - currentIterPtr->genIterator.currentStageRemainedSize;
        currentIterPtr->genIterator.currStageMemPtr = NULL;
        currentIterPtr->currStage++;
        *accumSizePtr += currentIterPtr->genIterator.currentStageRemainedSize;
        *hsuBlockMemPtrPtr += currentIterPtr->genIterator.currentStageRemainedSize;
        currentIterPtr->genIterator.currentStageRemainedSize = 0;
    }
    else
    {
        cpssOsMemCpy(dst, src,*hsuBlockMemSizePtr);
        currentIterPtr->genIterator.currStageMemPtr += *hsuBlockMemSizePtr;
        currentIterPtr->genIterator.currentStageRemainedSize = currentIterPtr->genIterator.currentStageRemainedSize - *hsuBlockMemSizePtr;
        *accumSizePtr += *hsuBlockMemSizePtr;
        *hsuBlockMemPtrPtr += *hsuBlockMemSizePtr;
        *hsuBlockMemSizePtr = 0;
    }
    return GT_OK;
}


/*******************************************************************************
* prvCpssDxChHsuInitStageSizeArray
*
* DESCRIPTION:
*       This function inits stage size array for given HSU data block.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum                 - device number
*       dataType               - stageArray data type
* OUTPUTS:
*       stageArrayPtr          - pointer to stage array
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on bad hsu data type
*       GT_BAD_PTR               - on NULL pointer
*
* COMMENTS:
*       none.
*
*******************************************************************************/
GT_STATUS prvCpssDxChHsuInitStageArray
(
    IN   GT_U8                                       devNum,
    IN   PRV_CPSS_DXCH_HSU_STAGE_ARRAY_DATA_TYPE_ENT dataType,
    OUT  PRV_CPSS_DXCH_HSU_STAGE_STC                 stageArrayPtr[]
)
{
    GT_STATUS rc = GT_OK;
    GT_U32 txQueueNumber;
    GT_U32 rxQueueNumber;
    GT_U32 stageIndex;
    PRV_CPSS_TRUNK_DB_INFO_STC *genTrunkDbPtr = PRV_CPSS_DEV_TRUNK_INFO_MAC(devNum);
    PRV_CPSS_TX_DESC_LIST_STC *txDescList = PRV_CPSS_PP_MAC(devNum)->intCtrl.txDescList;
    PRV_CPSS_RX_DESC_LIST_STC *rxDescList = PRV_CPSS_PP_MAC(devNum)->intCtrl.rxDescList;
    GT_U32      i=0;
    GT_U32      maxSerdesLanesNum;/* max number of lanes that can be allocated */
    GT_U32      laneNum=0;
    GT_U32      portGroupId;/* port group Id for multi-port-groups device support */
    CPSS_MULTI_NET_IF_TX_SDMA_QUEUE_STC  sdmaQueuesConfig;


    CPSS_NULL_PTR_CHECK_MAC(stageArrayPtr);
    switch(dataType)
    {
    case PRV_CPSS_DXCH_HSU_STAGE_ARRAY_DATA_TYPE_ALL_E:
        {
            /* init generic device stage*/
            stageArrayPtr[PRV_CPSS_DXCH_HSU_GEN_PP_CONFIG_STAGE_E].stageSize = sizeof(PRV_CPSS_GEN_PP_CONFIG_STC);
            stageArrayPtr[PRV_CPSS_DXCH_HSU_GEN_PP_CONFIG_STAGE_E].stageAddress =(GT_U8*)(&PRV_CPSS_DXCH_PP_MAC(devNum)->genInfo);
            /* init generic trunk array stage*/
            stageArrayPtr[PRV_CPSS_DXCH_HSU_GEN_TRUNK_ARRAY_STAGE_E].stageSize =
                sizeof(PRV_CPSS_TRUNK_ENTRY_INFO_STC) *(genTrunkDbPtr->numberOfTrunks + 1);
            stageArrayPtr[PRV_CPSS_DXCH_HSU_GEN_TRUNK_ARRAY_STAGE_E].stageAddress = (GT_U8*)genTrunkDbPtr->trunksArray;
            /* init generic trunk array all memebers stage*/
            stageArrayPtr[PRV_CPSS_DXCH_HSU_GEN_TRUNK_ALL_MEMBERS_ARRAY_STAGE_E].stageSize =
                sizeof(CPSS_TRUNK_MEMBER_STC)*genTrunkDbPtr->numMembersInTrunk * genTrunkDbPtr->numberOfTrunks;
            stageArrayPtr[PRV_CPSS_DXCH_HSU_GEN_TRUNK_ALL_MEMBERS_ARRAY_STAGE_E].stageAddress =
                (GT_U8*)genTrunkDbPtr->allMembersArray;
            /* init generic trunk local array stage*/
            stageArrayPtr[PRV_CPSS_DXCH_HSU_GEN_TRUNK_ALL_MEMBERS_IS_LOCAL_ARRAY_STAGE_E].stageSize =
                 sizeof(GT_BOOL)*genTrunkDbPtr->numMembersInTrunk * genTrunkDbPtr->numberOfTrunks;
            stageArrayPtr[PRV_CPSS_DXCH_HSU_GEN_TRUNK_ALL_MEMBERS_IS_LOCAL_ARRAY_STAGE_E].stageAddress =
                (GT_U8*)genTrunkDbPtr->allMembersIsLocalArray;
            /* init generic port info  array stage*/
            stageArrayPtr[PRV_CPSS_DXCH_HSU_GEN_PORT_INFO_ARRAY_STAGE_E].stageSize =
                sizeof(PRV_CPSS_PORT_INFO_ARRAY_STC) * (PRV_CPSS_PP_MAC(devNum)->numOfPorts);
            stageArrayPtr[PRV_CPSS_DXCH_HSU_GEN_PORT_INFO_ARRAY_STAGE_E].stageAddress =
                (GT_U8*) PRV_CPSS_PP_MAC(devNum)->phyPortInfoArray;
            /* init bridge stage*/
            stageArrayPtr[PRV_CPSS_DXCH_HSU_BRIDGE_STAGE_E].stageSize = sizeof(PRV_CPSS_DXCH_BRIDGE_INFO_STC);
            stageArrayPtr[PRV_CPSS_DXCH_HSU_BRIDGE_STAGE_E].stageAddress = (GT_U8*)(&PRV_CPSS_DXCH_PP_MAC(devNum)->bridge);
            /* init network interface  stage*/
            stageArrayPtr[PRV_CPSS_DXCH_HSU_NETIF_STAGE_E].stageSize = sizeof(PRV_CPSS_DXCH_NET_INFO_STC);
            stageArrayPtr[PRV_CPSS_DXCH_HSU_NETIF_STAGE_E].stageAddress = (GT_U8*)(&PRV_CPSS_DXCH_PP_MAC(devNum)->netIf);
            /* init policer stage*/
            stageArrayPtr[PRV_CPSS_DXCH_HSU_POLICER_STAGE_E].stageSize = sizeof(PRV_CPSS_DXCH_POLICER_INFO_STC);
            stageArrayPtr[PRV_CPSS_DXCH_HSU_POLICER_STAGE_E].stageAddress = (GT_U8*)(&PRV_CPSS_DXCH_PP_MAC(devNum)->policer);
            /* init port stage*/
            stageArrayPtr[PRV_CPSS_DXCH_HSU_PORT_STAGE_E].stageSize = sizeof(PRV_CPSS_DXCH_PORT_INFO_STC);
            stageArrayPtr[PRV_CPSS_DXCH_HSU_PORT_STAGE_E].stageAddress = (GT_U8*)(&PRV_CPSS_DXCH_PP_MAC(devNum)->port);
            /* init port groups stage*/
            stageArrayPtr[PRV_CPSS_DXCH_HSU_PORT_GROUPS_STAGE_E].stageSize = sizeof(PRV_CPSS_DXCH_PP_PORT_GROUPS_INFO_STC);
            stageArrayPtr[PRV_CPSS_DXCH_HSU_PORT_GROUPS_STAGE_E].stageAddress = (GT_U8*)(&PRV_CPSS_DXCH_PP_MAC(devNum)->portGroupsExtraInfo);

            if(PRV_CPSS_SIP_5_CHECK_MAC(devNum))
            {
                maxSerdesLanesNum = PRV_CPSS_BOBCAT2_SERDES_NUM_CNS;

                /* init serdes array stage*/
                stageArrayPtr[PRV_CPSS_DXCH_HSU_SERDES_BMP_STAGE_E].stageSize =sizeof(GT_U32)*2;
                stageArrayPtr[PRV_CPSS_DXCH_HSU_SERDES_BMP_STAGE_E].stageAddress = (GT_U8*)(&serdesLanesBmp);

                /* calculate lanes stages needed */
                if(PRV_CPSS_DXCH_PP_MAC(devNum)->serdesCfgDbArrPtr!=NULL)
                {
                    for(i=0; i < maxSerdesLanesNum; i++)
                    {
                        if(PRV_CPSS_DXCH_PP_MAC(devNum)->serdesCfgDbArrPtr[i]!=NULL)
                        {
                            /* set bit i to be 1 --> meaning the element was allocated */
                            serdesLanesBmp[i/32] |= (1<<(i%32));
                        }
                    }

                    /* init all lanes stages*/
                    for (stageIndex = PRV_CPSS_DXCH_HSU_SERDES_LANE0_STAGE_E;
                          stageIndex <= PRV_CPSS_DXCH_HSU_SERDES_LANE35_STAGE_E; stageIndex++)
                    {
                        laneNum = (stageIndex-PRV_CPSS_DXCH_HSU_SERDES_LANE0_STAGE_E);

                        if((serdesLanesBmp[laneNum/32] & (1<<(laneNum%32))) == 0)
                        {
                          stageArrayPtr[stageIndex].stageSize = 0;
                          stageArrayPtr[stageIndex].stageAddress = NULL;
                        }
                        else
                        {
                            stageArrayPtr[stageIndex].stageSize = sizeof(CPSS_DXCH_PORT_SERDES_TUNE_STC) * CPSS_DXCH_PORT_SERDES_SPEED_NA_E;
                            stageArrayPtr[stageIndex].stageAddress = (GT_U8*)PRV_CPSS_DXCH_PP_MAC(devNum)->serdesCfgDbArrPtr[laneNum];
                        }
                    }

                }
                else
                {
                    serdesLanesBmp[0]=0; /* there are no allocated serdes lanes */
                    serdesLanesBmp[1]=0; /* there are no allocated serdes lanes */

                     /* no need for all lanes stages*/
                    for (stageIndex = PRV_CPSS_DXCH_HSU_SERDES_LANE0_STAGE_E;
                          stageIndex <= PRV_CPSS_DXCH_HSU_SERDES_LANE35_STAGE_E; stageIndex++)
                    {
                        stageArrayPtr[stageIndex].stageSize = 0;
                        stageArrayPtr[stageIndex].stageAddress = NULL;
                    }
                }
            }
            else
            {
                stageArrayPtr[PRV_CPSS_DXCH_HSU_SERDES_BMP_STAGE_E].stageSize = 0;
                stageArrayPtr[PRV_CPSS_DXCH_HSU_SERDES_BMP_STAGE_E].stageAddress = NULL;

                for (stageIndex = PRV_CPSS_DXCH_HSU_SERDES_LANE0_STAGE_E;
                     stageIndex <= PRV_CPSS_DXCH_HSU_SERDES_LANE35_STAGE_E; stageIndex++)
                {
                     stageArrayPtr[stageIndex].stageSize = 0;
                     stageArrayPtr[stageIndex].stageAddress = NULL;
                }
            }
        }
        case PRV_CPSS_DXCH_HSU_STAGE_ARRAY_DATA_TYPE_RXTX_E:
        {
            /* init all generic RX stages*/
            for (stageIndex = PRV_CPSS_DXCH_HSU_GEN_RX_QUEUE0_STAGE_E;
                  stageIndex <= PRV_CPSS_DXCH_HSU_GEN_RX_QUEUE7_STAGE_E; stageIndex++ )
            {
                rc = prvCpssDxChHsuConvertStageInQueueIndex(stageIndex,&rxQueueNumber);
                if (rc != GT_OK)
                {
                    return rc;
                }

                if (systemRecoveryInfo.systemRecoveryMode.continuousRx == GT_TRUE)
                {
                    stageArrayPtr[stageIndex].stageSize =
                        rxDescList[rxQueueNumber].maxDescNum * sizeof(PRV_CPSS_SW_RX_DESC_STC);
                    stageArrayPtr[stageIndex].stageAddress = (GT_U8*)rxDescList[rxQueueNumber].swRxDescBlock;
                }
                else
                {
                    /* in MSG_DISABLE_MODE there is no export/import for rx descriptor done */
                    stageArrayPtr[stageIndex].stageSize = 0;
                    stageArrayPtr[stageIndex].stageAddress = NULL;
                }
            }
            /* init all generic TX stages*/
            for (stageIndex = PRV_CPSS_DXCH_HSU_GEN_TX_QUEUE0_STAGE_E;
                  stageIndex <= PRV_CPSS_DXCH_HSU_GEN_TX_QUEUE7_STAGE_E; stageIndex++ )
            {
                rc = prvCpssDxChHsuConvertStageInQueueIndex(stageIndex,&txQueueNumber);
                if (rc != GT_OK)
                {
                    return rc;
                }
                if (systemRecoveryInfo.systemRecoveryMode.continuousTx == GT_TRUE)
                {
                    if(PRV_CPSS_SIP_5_CHECK_MAC(devNum))
                    {
                        portGroupId = PRV_CPSS_NETIF_SDMA_PORT_GROUP_ID_MAC(devNum);
                    }
                    else
                    {
                        portGroupId = 0;
                    }

                    sdmaQueuesConfig = PRV_CPSS_DXCH_PP_MAC(devNum)->moduleCfg.multiNetIfCfg.txSdmaQueuesConfig[portGroupId][txQueueNumber];

                    if((PRV_CPSS_SIP_5_CHECK_MAC(devNum))&&
                       (PRV_CPSS_DXCH_PP_MAC(devNum)->moduleCfg.useMultiNetIfSdma==GT_TRUE)&&
                       (sdmaQueuesConfig.queueMode == CPSS_TX_SDMA_QUEUE_MODE_PACKET_GENERATOR_E))
                    {
                        stageArrayPtr[stageIndex].stageSize = sizeof(GT_U32)*txDescList[txQueueNumber].maxDescNum;
                        stageArrayPtr[stageIndex].stageAddress = (GT_U8*)txDescList[txQueueNumber].revPacketIdDb;

                    }
                    else
                    {
                        stageArrayPtr[stageIndex].stageSize =
                            txDescList[txQueueNumber].maxDescNum * sizeof(PRV_CPSS_SW_TX_DESC_STC);
                        stageArrayPtr[stageIndex].stageAddress = (GT_U8*)txDescList[txQueueNumber].swTxDescBlock;
                    }
                    if(PRV_CPSS_PP_MAC(devNum)->intCtrl.txGeneratorPacketIdDb.generatorsTotalDesc!=0)
                    {
                        stageArrayPtr[PRV_CPSS_DXCH_HSU_GEN_TX_GENERATOR_COOKIE_STAGE_E].stageSize = sizeof(GT_U32) * PRV_CPSS_PP_MAC(devNum)->intCtrl.txGeneratorPacketIdDb.generatorsTotalDesc;
                        stageArrayPtr[PRV_CPSS_DXCH_HSU_GEN_TX_GENERATOR_COOKIE_STAGE_E].stageAddress = (GT_U8*)PRV_CPSS_PP_MAC(devNum)->intCtrl.txGeneratorPacketIdDb.cookie;

                        stageArrayPtr[PRV_CPSS_DXCH_HSU_GEN_TX_GENERATOR_FREE_LINKED_LIST_STAGE_E].stageSize = sizeof(GT_U32) * PRV_CPSS_PP_MAC(devNum)->intCtrl.txGeneratorPacketIdDb.generatorsTotalDesc;
                        stageArrayPtr[PRV_CPSS_DXCH_HSU_GEN_TX_GENERATOR_FREE_LINKED_LIST_STAGE_E].stageAddress = (GT_U8*)PRV_CPSS_PP_MAC(devNum)->intCtrl.txGeneratorPacketIdDb.freeLinkedList;
                    }
                    else
                    {
                        stageArrayPtr[PRV_CPSS_DXCH_HSU_GEN_TX_GENERATOR_COOKIE_STAGE_E].stageSize = 0;
                        stageArrayPtr[PRV_CPSS_DXCH_HSU_GEN_TX_GENERATOR_COOKIE_STAGE_E].stageAddress = NULL;

                        stageArrayPtr[PRV_CPSS_DXCH_HSU_GEN_TX_GENERATOR_FREE_LINKED_LIST_STAGE_E].stageSize = 0;
                        stageArrayPtr[PRV_CPSS_DXCH_HSU_GEN_TX_GENERATOR_FREE_LINKED_LIST_STAGE_E].stageAddress = NULL;
                    }
                }
                else
                {
                    /* in MSG_DISABLE_MODE there is no export/import for tx descriptor done */
                    stageArrayPtr[stageIndex].stageSize = 0;
                    stageArrayPtr[stageIndex].stageAddress = NULL;

                    stageArrayPtr[PRV_CPSS_DXCH_HSU_GEN_TX_GENERATOR_COOKIE_STAGE_E].stageSize = 0;
                    stageArrayPtr[PRV_CPSS_DXCH_HSU_GEN_TX_GENERATOR_COOKIE_STAGE_E].stageAddress = NULL;

                    stageArrayPtr[PRV_CPSS_DXCH_HSU_GEN_TX_GENERATOR_FREE_LINKED_LIST_STAGE_E].stageSize = 0;
                    stageArrayPtr[PRV_CPSS_DXCH_HSU_GEN_TX_GENERATOR_FREE_LINKED_LIST_STAGE_E].stageAddress = NULL;


                }
            }
         }
        break;
    default:
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }
    return GT_OK;
}


/*******************************************************************************
* prvCpssDxChHsuGlobalDataBlockSizeGet
*
* DESCRIPTION:
*       This function gets the memory size needed to export the Global HSU
*       data block.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       None.
* OUTPUTS:
*       sizePtr                  - the Global HSU data block size calculated in bytes
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PTR               - on NULL pointer
*       GT_FAIL                  - otherwise
*
* COMMENTS:
*       none.
*
*******************************************************************************/
GT_STATUS prvCpssDxChHsuGlobalDataBlockSizeGet
(
    OUT   GT_U32    *sizePtr
)
{
    GT_STATUS rc = GT_OK;
    GT_U32 devNum;
    GT_U32 genericSize =0;
    GT_U32 rxDataSize = 0;
    GT_U32 txDataSize = 0;
    GT_U32 queueNumber = 0;
    GT_U32 i,numOfAllocatedSerdes=0;
    GT_U32 txGeneratorDataSize = 0;
    GT_U32 maxSerdesLanesNum;/* max number of lanes that can be allocated */
    GT_U32 portGroupId;/* port group Id for multi-port-groups device support */
    CPSS_MULTI_NET_IF_TX_SDMA_QUEUE_STC  sdmaQueuesConfig;

    PRV_CPSS_TX_DESC_LIST_STC *txDescList = NULL;
    PRV_CPSS_RX_DESC_LIST_STC *rxDescList = NULL;
    PRV_CPSS_TRUNK_DB_INFO_STC *genTrunkDbPtr = NULL;
    CPSS_NULL_PTR_CHECK_MAC(sizePtr);
    *sizePtr = 0;
    rc = prvCpssGenGlobalDataSizeGet(&genericSize);
    if (rc != GT_OK)
    {
        return rc;
    }
    for (devNum = 0; devNum < PRV_CPSS_MAX_PP_DEVICES_CNS; devNum++)
    {
        if ( (prvCpssPpConfig[devNum] == NULL) ||
             (PRV_CPSS_DXCH_FAMILY_CHECK_MAC(devNum) == 0) )
        {
            continue;
        }
        rxDataSize = 0;
        txDataSize = 0;
        txDescList  = PRV_CPSS_PP_MAC(devNum)->intCtrl.txDescList;
        rxDescList  = PRV_CPSS_PP_MAC(devNum)->intCtrl.rxDescList;
        genTrunkDbPtr = PRV_CPSS_DEV_TRUNK_INFO_MAC(devNum);

        if(PRV_CPSS_SIP_5_CHECK_MAC(devNum))
        {
            portGroupId = PRV_CPSS_NETIF_SDMA_PORT_GROUP_ID_MAC(devNum);
        }
        else
        {
            portGroupId = 0;
        }

        if (systemRecoveryInfo.systemRecoveryMode.continuousTx == GT_TRUE)
        {
            /* calculate tx data size*/
            for (queueNumber = 0; queueNumber < NUM_OF_TX_QUEUES; queueNumber++)
            {
                sdmaQueuesConfig = PRV_CPSS_DXCH_PP_MAC(devNum)->moduleCfg.multiNetIfCfg.txSdmaQueuesConfig[portGroupId][queueNumber];

               if((PRV_CPSS_SIP_5_CHECK_MAC(devNum))&&
                   (PRV_CPSS_DXCH_PP_MAC(devNum)->moduleCfg.useMultiNetIfSdma==GT_TRUE)&&
                   (sdmaQueuesConfig.queueMode == CPSS_TX_SDMA_QUEUE_MODE_PACKET_GENERATOR_E))
                {
                    txGeneratorDataSize += (sizeof(GT_U32)*txDescList[queueNumber].maxDescNum);

                }
                else
                {
                    txDataSize += txDescList[queueNumber].maxDescNum * sizeof(PRV_CPSS_SW_TX_DESC_STC);
                }
            }
        }
        else
        {
            /* in TX_MSG_DISABLE_MODE TX export/import is not performed */
            txDataSize = 0;
            txGeneratorDataSize=0;
        }

        if (systemRecoveryInfo.systemRecoveryMode.continuousRx == GT_TRUE)
        {
            /* calculate rx data size*/
            for (queueNumber = 0; queueNumber < NUM_OF_RX_QUEUES; queueNumber++)
            {
                rxDataSize += rxDescList[queueNumber].maxDescNum * sizeof(PRV_CPSS_SW_RX_DESC_STC);

            }
        }
        else
        {
            /* in RX_MSG_DISABLE_MODE RX export/import is not performed */
            rxDataSize = 0;
        }
        *sizePtr +=( sizeof(PRV_CPSS_GEN_PP_CONFIG_STC)+
                     rxDataSize + txDataSize +
                     (sizeof(PRV_CPSS_TRUNK_ENTRY_INFO_STC) *(genTrunkDbPtr->numberOfTrunks + 1))+
                     (sizeof(CPSS_TRUNK_MEMBER_STC)*genTrunkDbPtr->numMembersInTrunk *genTrunkDbPtr->numberOfTrunks)+
                     (sizeof(GT_BOOL)*genTrunkDbPtr->numMembersInTrunk * genTrunkDbPtr->numberOfTrunks)+
                     (sizeof(PRV_CPSS_PORT_INFO_ARRAY_STC) * (PRV_CPSS_PP_MAC(devNum)->numOfPorts)) +
                     sizeof(PRV_CPSS_DXCH_BRIDGE_INFO_STC) +  sizeof(PRV_CPSS_DXCH_NET_INFO_STC) +
                     sizeof(PRV_CPSS_DXCH_POLICER_INFO_STC) + sizeof(PRV_CPSS_DXCH_PORT_INFO_STC) +
                     sizeof(PRV_CPSS_DXCH_PP_PORT_GROUPS_INFO_STC));

        if(PRV_CPSS_SIP_5_CHECK_MAC(devNum))
        {
            maxSerdesLanesNum = PRV_CPSS_BOBCAT2_SERDES_NUM_CNS;

            /* serdes BMP size */
            *sizePtr += sizeof(GT_32)*2;

            if(PRV_CPSS_DXCH_PP_MAC(devNum)->serdesCfgDbArrPtr!=NULL)
            {
                for(i=0; i<maxSerdesLanesNum; i++)
                {
                    if(PRV_CPSS_DXCH_PP_MAC(devNum)->serdesCfgDbArrPtr[i] != NULL)
                        numOfAllocatedSerdes++;
                }
                *sizePtr += (sizeof(CPSS_DXCH_PORT_SERDES_TUNE_STC) * CPSS_DXCH_PORT_SERDES_SPEED_NA_E * numOfAllocatedSerdes);
            }

            if (systemRecoveryInfo.systemRecoveryMode.continuousTx == GT_TRUE)
            {
                /* tx generator data size */
                *sizePtr += txGeneratorDataSize;

                /* size of cookie List + size of freeLinkedList */
                *sizePtr += ((sizeof(GT_U32) * PRV_CPSS_PP_MAC(devNum)->intCtrl.txGeneratorPacketIdDb.generatorsTotalDesc)+
                             (sizeof(GT_U32) * PRV_CPSS_PP_MAC(devNum)->intCtrl.txGeneratorPacketIdDb.generatorsTotalDesc));
            }
        }
    }
    /* add system global data size and TLV header to size calculation */
    *sizePtr = *sizePtr + genericSize +  sizeof(CPSS_DXCH_HSU_DATA_BLOCK_HEADER_STC);
    return GT_OK;
}



/*******************************************************************************
* prvCpssDxChHsuLpmDbDataBlockSizeGet
*
* DESCRIPTION:
*       This function gets the memory size needed to export the LPM DB HSU
*       data block.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       None.
* OUTPUTS:
*       sizePtr                  - the LPM DB size calculated in bytes
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PTR               - on NULL pointer
*       GT_FAIL                  - otherwise
*
* COMMENTS:
*       none.
*
*******************************************************************************/
GT_STATUS prvCpssDxChHsuLpmDbDataBlockSizeGet
(
    OUT   GT_U32    *sizePtr
)
{
    GT_U32                        lpmDbId = 0;
    GT_UINTPTR                    slIterator = 0;
    GT_U32                        lpmDbSize = 0;
    GT_STATUS                     rc = GT_OK ;
    CPSS_NULL_PTR_CHECK_MAC(sizePtr);

    *sizePtr = 0;
    while (1)
    {
        rc =  prvCpssDxChIpLpmDbIdGetNext(&lpmDbId,&slIterator);
        if (rc == GT_NO_MORE)
        {
            break;
        }
        if (rc != GT_OK)
        {
            return rc;
        }
        rc = cpssDxChIpLpmDBMemSizeGet(lpmDbId,
                                       &lpmDbSize);
        if (rc != GT_OK)
        {
            return rc;
        }
        *sizePtr=*sizePtr+lpmDbSize;
    }
    /* add TLV header to size calculation */
    *sizePtr = *sizePtr + sizeof(CPSS_DXCH_HSU_DATA_BLOCK_HEADER_STC);
    return GT_OK;
}



/*******************************************************************************
* prvCpssDxChHsuGlobalDataBlockExport
*
* DESCRIPTION:
*       This function exports Global HSU data block to survived restart
*       memory area supplied by application.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       iteratorPtr            - pointer to iterator, to start - set iterator to 0.
*       hsuBlockMemSizePtr     - pointer hsu block data size supposed to be exported
*                                in current iteration. The minimal value is 1k bytes.
*       hsuBlockMemPtr         - pointer to HSU survived restart memory area
*
* OUTPUTS:
*       iteratorPtr            - the iterator - points to the point from which
*                                process will be continued in future iteration.
*       hsuBlockMemSizePtr     - pointer to hsu block data size exported in current iteration.
*       exportCompletePtr      - GT_TRUE -  HSU data export is completed.
*                                GT_FALSE - HSU data export is not completed.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong hsuBlockMemSize, wrong iterator.
*       GT_BAD_PTR               - on NULL pointer
*       GT_FAIL                  - otherwise
*
* COMMENTS:
*       none.
*
*******************************************************************************/
GT_STATUS prvCpssDxChHsuGlobalDataBlockExport
(
    INOUT  GT_UINTPTR                    *iteratorPtr,
    INOUT  GT_U32                        *hsuBlockMemSizePtr,
    IN     GT_U8                         *hsuBlockMemPtr,
    OUT    GT_BOOL                       *exportCompletePtr
)
{
    GT_STATUS                     rc = GT_OK ;

    CPSS_DXCH_HSU_DATA_BLOCK_HEADER_STC *header = NULL;
    PRV_CPSS_DXCH_HSU_GLOBAL_ITERATOR_STC  *currentIterPtr = NULL;
    PRV_CPSS_RX_DESC_LIST_STC *rxDescList = NULL;
    PRV_CPSS_TX_DESC_LIST_STC *txDescList = NULL;
    GT_U8 devNum;
    GT_BOOL enoughMemoryForStage = GT_FALSE;
    PRV_CPSS_DXCH_HSU_GLOBAL_DATA_STAGE_ENT currentStage;
    PRV_CPSS_DXCH_HSU_STAGE_STC stageInfoArray[PRV_CPSS_DXCH_HSU_GLOBAL_LAST_STAGE_E];

    GT_U32 queueNumber;
    PRV_CPSS_SW_RX_DESC_STC* tmpRxDescPtr = NULL;
    PRV_CPSS_SW_TX_DESC_STC* tmpTxDescPtr = NULL;
    GT_U32 currentHsuBlockVersion = 0;
    PRV_CPSS_GEN_PP_CONFIG_STC *genPpConfigPtr;

    GT_U32         portGroupId;/* port group Id for multi-port-groups device support */
    CPSS_MULTI_NET_IF_TX_SDMA_QUEUE_STC  sdmaQueuesConfig;

    CPSS_NULL_PTR_CHECK_MAC(hsuBlockMemPtr);
    CPSS_NULL_PTR_CHECK_MAC(iteratorPtr);
    CPSS_NULL_PTR_CHECK_MAC(exportCompletePtr);
    *exportCompletePtr = GT_FALSE;
    currentIterPtr = (PRV_CPSS_DXCH_HSU_GLOBAL_ITERATOR_STC*)*iteratorPtr;

    /* Upon first iteration TLV header should be saved into hsu memory */
    if (currentIterPtr == NULL) /* first iteration */
    {
        if (*hsuBlockMemSizePtr <= sizeof(CPSS_DXCH_HSU_DATA_BLOCK_HEADER_STC))
        {
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
        }
        /* we need to allocate the iterator */
        currentIterPtr =
            (PRV_CPSS_DXCH_HSU_GLOBAL_ITERATOR_STC*)cpssOsMalloc(sizeof(PRV_CPSS_DXCH_HSU_GLOBAL_ITERATOR_STC));
        if (currentIterPtr == NULL)
        {
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_OUT_OF_CPU_MEM, LOG_ERROR_NO_MSG);
        }
        /*reset it */
        cpssOsMemSet(currentIterPtr,0,sizeof(PRV_CPSS_DXCH_HSU_GLOBAL_ITERATOR_STC));

        /* calculate Global size for export */
        rc = prvCpssDxChHsuGlobalDataBlockSizeGet(&currentIterPtr->hsuBlockGlobalSize);
        if (rc != GT_OK)
        {
            return rc;
        }
        rc = prvCpssDxChHsuDataBlockVersionGet(&currentHsuBlockVersion);
        if (rc != GT_OK)
        {
            return rc;
        }
        header = (CPSS_DXCH_HSU_DATA_BLOCK_HEADER_STC *)hsuBlockMemPtr;
        header->length = currentIterPtr->hsuBlockGlobalSize;
        header->type = CPSS_DXCH_HSU_DATA_TYPE_GLOBAL_E;
        header->version = currentHsuBlockVersion;
        header->reserved1 = 0;
        header->reserved2 = 0;
        hsuBlockMemPtr = (GT_U8*)(++header);
        *hsuBlockMemSizePtr-= sizeof(CPSS_DXCH_HSU_DATA_BLOCK_HEADER_STC);
        currentIterPtr->accumulatedGlobalSize = sizeof(CPSS_DXCH_HSU_DATA_BLOCK_HEADER_STC);
        /* set the starting stage */
        currentIterPtr->currStage = PRV_CPSS_DXCH_HSU_GEN_PP_CONFIG_STAGE_E;
        currentIterPtr->genIterator.currStage = PRV_CPSS_GEN_HSU_CPSS_STAGE_E;
        currentIterPtr->genIterator.magic = PRV_CPSS_DXCH_HSU_ITERATOR_MAGIC_NUMBER_CNC;
        currentIterPtr->genIterator.currStageMemPtr = NULL;
        currentIterPtr->devNum = PRV_CPSS_MAX_PP_DEVICES_CNS;
        *iteratorPtr = (GT_UINTPTR)currentIterPtr;
        currentStage = PRV_CPSS_DXCH_HSU_SYS_GLOBAL_STAGE_E;
    }
    else
    {
        if(currentIterPtr->accumulatedGlobalSize == 0)
        {
            /* it is not first iteration  -- accumulatedGlobalSize should be larger
               than 0. Wrong iterator value possibly was applied */
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
        }

        if (currentIterPtr->genIterator.magic != PRV_CPSS_DXCH_HSU_ITERATOR_MAGIC_NUMBER_CNC)
        {
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
        }
        if (currentIterPtr->accumulatedGlobalSize >= currentIterPtr->hsuBlockGlobalSize)
        {
            *exportCompletePtr = GT_TRUE;
            return GT_OK;
        }
    }

    /* make cpss generic and cpss driver global data export */
    if (currentIterPtr->genStageComplete == GT_FALSE)
    {
        rc = prvCpssGenHsuExportImport(PRV_CPSS_HSU_EXPORT_E,
                                       &currentIterPtr->genIterator,
                                       hsuBlockMemSizePtr,
                                       &hsuBlockMemPtr,
                                       &currentIterPtr->accumulatedGlobalSize,
                                       &currentIterPtr->genStageComplete);
        if (rc != GT_OK)
        {
            return rc;
        }
        if (currentIterPtr->genStageComplete == GT_FALSE)
        {
            return GT_OK;
        }
    }
    /* in this point  generic stage was done and DXCH globals should be processed */
#if 0
    if (systemDataProcessed == GT_FALSE)
    {
        if (currentIterPtr->currStage == PRV_CPSS_DXCH_HSU_SYS_GLOBAL_STAGE_E)
        {
            if (currentIterPtr->genIterator.currStageMemPtr == NULL)
            {
                currentIterPtr->genIterator.currStageMemPtr = (GT_U8*)(&sysGlobalInfo);
                currentIterPtr->genIterator.currentStageRemainedSize = sizeof(PRV_CPSS_DXCH_SYS_GLOBAL_DATA_STC);
            }
            if (*hsuBlockMemSizePtr >= currentIterPtr->genIterator.currentStageRemainedSize )
            {
                enoughMemoryForStage = GT_TRUE;
            }
            else
            {
                enoughMemoryForStage = GT_FALSE;
            }
            rc = prvCpssDxChHsuExportImportDataHandling(PRV_CPSS_HSU_EXPORT_E,
                                                          currentIterPtr,
                                                          hsuBlockMemSizePtr,
                                                          &hsuBlockMemPtr,
                                                          &currentIterPtr->accumulatedGlobalSize);
            if (rc != GT_OK)
            {
                return rc;
            }
            if (enoughMemoryForStage == GT_TRUE)
            {
                /* This stage is over, so preper next stage */
                currentIterPtr->currStage = PRV_CPSS_DXCH_HSU_TRUNK_ARRAY_STAGE_E;
                currentStage = PRV_CPSS_DXCH_HSU_TRUNK_ARRAY_STAGE_E;
            }
            else
            {
                return rc;
            }
        }
        /* At this stage all system globals are handled*/
        systemDataProcessed = GT_TRUE;
    }
#endif
    /* make export for data per device */
    for (devNum = 0; devNum < PRV_CPSS_MAX_PP_DEVICES_CNS; devNum++)
    {
        if ( (prvCpssPpConfig[devNum] == NULL) ||
             (PRV_CPSS_DXCH_FAMILY_CHECK_MAC(devNum) == 0) )
        {
            continue;
        }
        cpssOsMemSet(stageInfoArray,0,sizeof(stageInfoArray));
        /* init stage data */
        rc = prvCpssDxChHsuInitStageArray(devNum,
                                         PRV_CPSS_DXCH_HSU_STAGE_ARRAY_DATA_TYPE_ALL_E,
                                         stageInfoArray);
       if (rc != GT_OK)
       {
           return rc;
       }


       for( currentStage = PRV_CPSS_DXCH_HSU_GEN_PP_CONFIG_STAGE_E;
             currentStage < PRV_CPSS_DXCH_HSU_GLOBAL_LAST_STAGE_E;
             currentStage++)
       {
            if ( (currentIterPtr->currStage == currentStage) &&
                 ( (currentIterPtr->devNum == devNum) || /* continue handling old device */
                   (currentIterPtr->devNum == PRV_CPSS_MAX_PP_DEVICES_CNS /* new device */)) )
            {
                if (currentIterPtr->genIterator.currStageMemPtr == NULL)
                {
                    currentIterPtr->genIterator.currentStageHsuBlockMemPtr = hsuBlockMemPtr;
                    currentIterPtr->genIterator.currStageMemPtr = stageInfoArray[currentStage].stageAddress;
                    currentIterPtr->genIterator.currentStageRemainedSize = stageInfoArray[currentStage].stageSize;
                    if (currentStage == PRV_CPSS_DXCH_HSU_GEN_PP_CONFIG_STAGE_E)
                    {
                        if (systemRecoveryInfo.systemRecoveryMode.continuousRx == GT_TRUE)
                        {
                            /* make some preparations for rx/tx pointers*/
                            for(queueNumber = 0; queueNumber < NUM_OF_RX_QUEUES; queueNumber++)
                            {
                                rxDescList  = &PRV_CPSS_PP_MAC(devNum)->intCtrl.rxDescList[queueNumber];
                                if (rxDescList->swRxDescBlock == NULL)
                                {
                                    CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_STATE, LOG_ERROR_NO_MSG);
                                }
                                /* save original rx pointers*/
                                tmpRxDescPtr = (PRV_CPSS_SW_RX_DESC_STC*)(rxDescList->swRxDescBlock);
                                currentIterPtr->rxOrigDescArray[queueNumber][0] = (PRV_CPSS_SW_RX_DESC_STC*)tmpRxDescPtr->rxDesc;
                                currentIterPtr->rxOrigDescArray[queueNumber][1] = (PRV_CPSS_SW_RX_DESC_STC*)(rxDescList->next2Return->rxDesc);
                                currentIterPtr->rxOrigDescArray[queueNumber][2] = (PRV_CPSS_SW_RX_DESC_STC*)(rxDescList->next2Receive->rxDesc);
                            }
                        }
                        if (systemRecoveryInfo.systemRecoveryMode.continuousTx == GT_TRUE)
                        {
                            if(PRV_CPSS_SIP_5_CHECK_MAC(devNum))
                            {
                                portGroupId = PRV_CPSS_NETIF_SDMA_PORT_GROUP_ID_MAC(devNum);
                            }
                            else
                            {
                                portGroupId = 0;
                            }
                            for(queueNumber = 0; queueNumber < NUM_OF_TX_QUEUES; queueNumber++)
                            {
                                sdmaQueuesConfig =
                                    PRV_CPSS_DXCH_PP_MAC(devNum)->moduleCfg.multiNetIfCfg.txSdmaQueuesConfig[portGroupId][queueNumber];

                                txDescList  = &PRV_CPSS_PP_MAC(devNum)->intCtrl.txDescList[queueNumber];

                                if((PRV_CPSS_SIP_5_CHECK_MAC(devNum))&&
                                    (PRV_CPSS_DXCH_PP_MAC(devNum)->moduleCfg.useMultiNetIfSdma==GT_TRUE)&&
                                    (sdmaQueuesConfig.queueMode == CPSS_TX_SDMA_QUEUE_MODE_PACKET_GENERATOR_E))
                                {
                                    /* firstDescPtr, freeDescPtr and scratchPadPtr are saved in their relevant stages */
                                    if (txDescList->revPacketIdDb == NULL)
                                    {
                                        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_STATE, LOG_ERROR_NO_MSG);
                                    }
                                    /* save original tx pointers*/
                                    currentIterPtr->txOrigDescArray[queueNumber][0] = (PRV_CPSS_SW_TX_DESC_STC*)(txDescList->revPacketIdDb);

                                }
                                else
                                {
                                    if (txDescList->swTxDescBlock == NULL)
                                    {
                                        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_STATE, LOG_ERROR_NO_MSG);
                                    }
                                    /* save original tx pointers*/
                                    tmpTxDescPtr = (PRV_CPSS_SW_TX_DESC_STC*)(txDescList->swTxDescBlock);
                                    currentIterPtr->txOrigDescArray[queueNumber][0] = (PRV_CPSS_SW_TX_DESC_STC*)(tmpTxDescPtr->txDesc);
                                    currentIterPtr->txOrigDescArray[queueNumber][1] = (PRV_CPSS_SW_TX_DESC_STC*)(txDescList->next2Feed->txDesc);
                                    currentIterPtr->txOrigDescArray[queueNumber][2] = (PRV_CPSS_SW_TX_DESC_STC*)(txDescList->next2Free->txDesc);
                                }
                            }
                        }
                    }
                }
                if (*hsuBlockMemSizePtr >= currentIterPtr->genIterator.currentStageRemainedSize )
                {
                    enoughMemoryForStage = GT_TRUE;
                }
                else
                {
                    enoughMemoryForStage = GT_FALSE;
                }

                rc = prvCpssDxChHsuExportImportDataHandling(PRV_CPSS_HSU_EXPORT_E,
                                                            currentIterPtr,
                                                            hsuBlockMemSizePtr,
                                                            &hsuBlockMemPtr,
                                                            &currentIterPtr->accumulatedGlobalSize);
                if (rc != GT_OK)
                {
                    return rc;
                }
                if (enoughMemoryForStage == GT_FALSE)
                {
                    currentIterPtr->devNum = devNum;
                    return rc;
                }
                else
                {
                    if (currentStage == PRV_CPSS_DXCH_HSU_GEN_PP_CONFIG_STAGE_E)
                    {
                        /* if this place had beeing reached the stage already was fully done*/
                        genPpConfigPtr = (PRV_CPSS_GEN_PP_CONFIG_STC*)(currentIterPtr->genIterator.currentStageHsuBlockMemPtr);
                        if (systemRecoveryInfo.systemRecoveryMode.continuousRx == GT_TRUE)
                        {
                            /* save original rx pointers*/
                            for (queueNumber = 0; queueNumber < NUM_OF_RX_QUEUES; queueNumber++)
                            {
                                genPpConfigPtr->intCtrl.rxDescList[queueNumber].swRxDescBlock = currentIterPtr->rxOrigDescArray[queueNumber][0];
                                genPpConfigPtr->intCtrl.rxDescList[queueNumber].next2Return = currentIterPtr->rxOrigDescArray[queueNumber][1];
                                genPpConfigPtr->intCtrl.rxDescList[queueNumber].next2Receive = currentIterPtr->rxOrigDescArray[queueNumber][2];
                            }
                        }
                        if (systemRecoveryInfo.systemRecoveryMode.continuousTx == GT_TRUE)
                        {
                            /* save original tx pointers*/

                            if(PRV_CPSS_SIP_5_CHECK_MAC(devNum))
                            {
                                portGroupId = PRV_CPSS_NETIF_SDMA_PORT_GROUP_ID_MAC(devNum);
                            }
                            else
                            {
                                portGroupId = 0;
                            }
                            for(queueNumber = 0; queueNumber < NUM_OF_TX_QUEUES; queueNumber++)
                            {
                                sdmaQueuesConfig =
                                    PRV_CPSS_DXCH_PP_MAC(devNum)->moduleCfg.multiNetIfCfg.txSdmaQueuesConfig[portGroupId][queueNumber];

                               if((PRV_CPSS_SIP_5_CHECK_MAC(devNum))&&
                                   (PRV_CPSS_DXCH_PP_MAC(devNum)->moduleCfg.useMultiNetIfSdma==GT_TRUE)&&
                                   (sdmaQueuesConfig.queueMode == CPSS_TX_SDMA_QUEUE_MODE_PACKET_GENERATOR_E))
                                {
                                    genPpConfigPtr->intCtrl.txDescList[queueNumber].revPacketIdDb = (GT_U32 *)currentIterPtr->txOrigDescArray[queueNumber][0];
                                }
                                else
                                {
                                    genPpConfigPtr->intCtrl.txDescList[queueNumber].swTxDescBlock = currentIterPtr->txOrigDescArray[queueNumber][0];
                                    genPpConfigPtr->intCtrl.txDescList[queueNumber].next2Feed = currentIterPtr->txOrigDescArray[queueNumber][1];
                                    genPpConfigPtr->intCtrl.txDescList[queueNumber].next2Free = currentIterPtr->txOrigDescArray[queueNumber][2];
                                }
                            }
                        }
                    }
                }
            }
        }
        /* all data for current device was processed */
        currentIterPtr->devNum =PRV_CPSS_MAX_PP_DEVICES_CNS;
        currentIterPtr->currStage = PRV_CPSS_DXCH_HSU_GEN_PP_CONFIG_STAGE_E;
    }

    if (currentIterPtr->accumulatedGlobalSize == currentIterPtr->hsuBlockGlobalSize)
    {
        *exportCompletePtr = GT_TRUE;
        rc = GT_OK;
    }
    else
    {
        rc = GT_FAIL;
    }
    /* all stages were done -- free iterator */
    cpssOsFree(currentIterPtr);
    *iteratorPtr = 0;

    return  rc;
}

/*******************************************************************************
* internal_cpssDxChHsuBlockSizeGet
*
* DESCRIPTION:
*       This function gets the memory size needed to export the required HSU
*       data block.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       dataType               - hsu data type.
*
* OUTPUTS:
*       sizePtr               - memory size needed to export required hsu data
*                               block. (calculated in bytes)
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PTR               - on NULL pointer
*       GT_FAIL                  - otherwise
*
* COMMENTS:
*       none.
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChHsuBlockSizeGet
(
    IN   CPSS_DXCH_HSU_DATA_TYPE_ENT dataType,
    OUT  GT_U32                        *sizePtr
)
{
    GT_STATUS rc = GT_OK;
    GT_U32 lpmDbSise,globalSize;

    CPSS_NULL_PTR_CHECK_MAC(sizePtr);
    switch(dataType)
    {
    case CPSS_DXCH_HSU_DATA_TYPE_LPM_DB_E:
        rc = prvCpssDxChHsuLpmDbDataBlockSizeGet(sizePtr);
        break;
    case CPSS_DXCH_HSU_DATA_TYPE_GLOBAL_E:
        rc = prvCpssDxChHsuGlobalDataBlockSizeGet(sizePtr);
        break;
    case CPSS_DXCH_HSU_DATA_TYPE_ALL_E:
        rc = prvCpssDxChHsuLpmDbDataBlockSizeGet(&lpmDbSise);
        if (rc != GT_OK)
        {
            return rc;
        }
        rc = prvCpssDxChHsuGlobalDataBlockSizeGet(&globalSize);
        if (rc != GT_OK)
        {
            return rc;
        }
        *sizePtr = lpmDbSise + globalSize;
        break;
    default:
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }
    return rc;
}

/*******************************************************************************
* cpssDxChHsuBlockSizeGet
*
* DESCRIPTION:
*       This function gets the memory size needed to export the required HSU
*       data block.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       dataType               - hsu data type.
*
* OUTPUTS:
*       sizePtr               - memory size needed to export required hsu data
*                               block. (calculated in bytes)
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PTR               - on NULL pointer
*       GT_FAIL                  - otherwise
*
* COMMENTS:
*       none.
*
*******************************************************************************/
GT_STATUS cpssDxChHsuBlockSizeGet
(
    IN   CPSS_DXCH_HSU_DATA_TYPE_ENT dataType,
    OUT  GT_U32                        *sizePtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChHsuBlockSizeGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, dataType, sizePtr));

    rc = internal_cpssDxChHsuBlockSizeGet(dataType, sizePtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, dataType, sizePtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChHsuExport
*
* DESCRIPTION:
*       This function exports required HSU data block to survived restart
*       memory area supplied by application.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       dataType               - hsu data type.
*       iteratorPtr            - pointer to iterator, to start - set iterator to 0.
*       hsuBlockMemSizePtr     - pointer to hsu block data size supposed to be
*                                exported in current iteration.The minimal value
*                                is 1k bytes. The export by one single iteration
*                                is performed when hsuBlockMemSizePtr  points to
*                                CPSS_DXCH_HSU_SINGLE_ITERATION_CNS value.
*                                Exact size calculation is done internally.
*       hsuBlockMemPtr         - pointer to HSU survived restart memory area
*
* OUTPUTS:
*       iteratorPtr            - the iterator - points to the point from which
*                                process will be continued in future iteration.
*       hsuBlockMemSizePtr     - pointer to hsu block data size that was not used
*                                in current iteration.
*       exportCompletePtr      - GT_TRUE -  HSU data export is completed.
*                                GT_FALSE - HSU data export is not completed.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong hsuBlockMemSize, dataType.
*       GT_BAD_PTR               - on NULL pointer
*       GT_FAIL                  - otherwise
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       none.
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChHsuExport
(
    IN     CPSS_DXCH_HSU_DATA_TYPE_ENT dataType,
    INOUT  GT_UINTPTR                  *iteratorPtr,
    INOUT  GT_U32                      *hsuBlockMemSizePtr,
    IN     GT_U8                       *hsuBlockMemPtr,
    OUT    GT_BOOL                     *exportCompletePtr
)
{
    GT_STATUS rc = GT_OK;
    GT_U32 origlBlockMemSize = 0 ;
    GT_BOOL oneIteration = GT_FALSE;

    CPSS_NULL_PTR_CHECK_MAC(iteratorPtr);
    CPSS_NULL_PTR_CHECK_MAC(hsuBlockMemPtr);
    CPSS_NULL_PTR_CHECK_MAC(hsuBlockMemSizePtr);
    CPSS_NULL_PTR_CHECK_MAC(exportCompletePtr);
    if ( (*hsuBlockMemSizePtr != CPSS_DXCH_HSU_SINGLE_ITERATION_CNS) &&
         (*hsuBlockMemSizePtr < BIT_10) )
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }
    if (*hsuBlockMemSizePtr == CPSS_DXCH_HSU_SINGLE_ITERATION_CNS )
    {
        /* single iteration */
        oneIteration = GT_TRUE;
        *iteratorPtr = 0;
    }

    /* all hsu data is processed by iterations */
    if (*iteratorPtr == 0)  /* first iteration */
    {
        /* allocate frame iterator*/
        frameIterPtr =
            (PRV_CPSS_DXCH_HSU_MAIN_FRAME_ITERATOR_STC*)cpssOsMalloc(sizeof(PRV_CPSS_DXCH_HSU_MAIN_FRAME_ITERATOR_STC));
        if (frameIterPtr == NULL)
        {
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_OUT_OF_CPU_MEM, LOG_ERROR_NO_MSG);
        }
        /*reset it */
        cpssOsMemSet(frameIterPtr,0,sizeof(PRV_CPSS_DXCH_HSU_MAIN_FRAME_ITERATOR_STC));
        /* calculate  DB size for export */
        rc = cpssDxChHsuBlockSizeGet(dataType,&frameIterPtr->hsuAllDataSize);
        if (rc != GT_OK)
        {
            cpssOsFree(frameIterPtr);
            return rc;
        }
        frameIterPtr->hsuCurrentPhase = PRV_CPSS_DXCH_HSU_DATA_MAIN_PHASE_LPM_DB_E;
    }
    *exportCompletePtr = GT_FALSE;
    switch(dataType)
    {
    case CPSS_DXCH_HSU_DATA_TYPE_LPM_DB_E:
        rc = prvCpssDxChHsuLpmDbDataBlockExport(iteratorPtr,
                                                hsuBlockMemSizePtr,
                                                hsuBlockMemPtr,
                                                exportCompletePtr);
        break;
    case CPSS_DXCH_HSU_DATA_TYPE_GLOBAL_E:
        rc = prvCpssDxChHsuGlobalDataBlockExport(iteratorPtr,
                                                 hsuBlockMemSizePtr,
                                                 hsuBlockMemPtr,
                                                 exportCompletePtr);

        break;
    case CPSS_DXCH_HSU_DATA_TYPE_ALL_E:

        if (frameIterPtr->hsuCurrentPhase == PRV_CPSS_DXCH_HSU_DATA_MAIN_PHASE_LPM_DB_E)
        {
            origlBlockMemSize = *hsuBlockMemSizePtr;

            rc = prvCpssDxChHsuLpmDbDataBlockExport(iteratorPtr,
                                                    hsuBlockMemSizePtr,
                                                    hsuBlockMemPtr,
                                                    exportCompletePtr);
            if (rc != GT_OK)
            {
                return rc;
            }
            frameIterPtr->hsuAllAccumulatedSize += origlBlockMemSize - *hsuBlockMemSizePtr;
            if (*exportCompletePtr == GT_FALSE)
            {
                if (oneIteration == GT_TRUE)
                {
                    cpssOsFree(frameIterPtr);
                    CPSS_LOG_ERROR_AND_RETURN_MAC(GT_FAIL, LOG_ERROR_NO_MSG);
                }
                return GT_OK;
            }
            else
            {
                /* the stage was finished */
                frameIterPtr->hsuCurrentPhase = PRV_CPSS_DXCH_HSU_DATA_MAIN_PHASE_GLOBAL_E;
                *exportCompletePtr = GT_FALSE;
            }
            hsuBlockMemPtr = (GT_U8*)(hsuBlockMemPtr + (origlBlockMemSize - *hsuBlockMemSizePtr));
        }

        if (frameIterPtr->hsuCurrentPhase == PRV_CPSS_DXCH_HSU_DATA_MAIN_PHASE_GLOBAL_E)
        {
            origlBlockMemSize = *hsuBlockMemSizePtr;
            rc = prvCpssDxChHsuGlobalDataBlockExport(iteratorPtr,
                                                     hsuBlockMemSizePtr,
                                                     hsuBlockMemPtr,
                                                     exportCompletePtr);
            if (rc != GT_OK)
            {
                cpssOsFree(frameIterPtr);
                return rc;
            }
            frameIterPtr->hsuAllAccumulatedSize += (origlBlockMemSize - *hsuBlockMemSizePtr);
            if (*exportCompletePtr == GT_FALSE)
            {
                if (oneIteration == GT_TRUE)
                {
                    cpssOsFree(frameIterPtr);
                    CPSS_LOG_ERROR_AND_RETURN_MAC(GT_FAIL, LOG_ERROR_NO_MSG);
                }
                return GT_OK;
            }
            else
            {
                /* the last stage was finished */
                if (frameIterPtr->hsuAllAccumulatedSize == frameIterPtr->hsuAllDataSize)
                {
                    *iteratorPtr = 0;
                    rc = GT_OK;
                }
                else
                {
                     rc = GT_FAIL;
                }
            }
        }
        break;
    default:
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }
    if ( (*iteratorPtr == 0) && (*exportCompletePtr == GT_TRUE) )
    {
        /* all stages were done -- free iterator */
        cpssOsFree(frameIterPtr);
    }
    return rc;
}

/*******************************************************************************
* cpssDxChHsuExport
*
* DESCRIPTION:
*       This function exports required HSU data block to survived restart
*       memory area supplied by application.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       dataType               - hsu data type.
*       iteratorPtr            - pointer to iterator, to start - set iterator to 0.
*       hsuBlockMemSizePtr     - pointer to hsu block data size supposed to be
*                                exported in current iteration.The minimal value
*                                is 1k bytes. The export by one single iteration
*                                is performed when hsuBlockMemSizePtr  points to
*                                CPSS_DXCH_HSU_SINGLE_ITERATION_CNS value.
*                                Exact size calculation is done internally.
*       hsuBlockMemPtr         - pointer to HSU survived restart memory area
*
* OUTPUTS:
*       iteratorPtr            - the iterator - points to the point from which
*                                process will be continued in future iteration.
*       hsuBlockMemSizePtr     - pointer to hsu block data size that was not used
*                                in current iteration.
*       exportCompletePtr      - GT_TRUE -  HSU data export is completed.
*                                GT_FALSE - HSU data export is not completed.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong hsuBlockMemSize, dataType.
*       GT_BAD_PTR               - on NULL pointer
*       GT_FAIL                  - otherwise
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       none.
*
*******************************************************************************/
GT_STATUS cpssDxChHsuExport
(
    IN     CPSS_DXCH_HSU_DATA_TYPE_ENT dataType,
    INOUT  GT_UINTPTR                  *iteratorPtr,
    INOUT  GT_U32                      *hsuBlockMemSizePtr,
    IN     GT_U8                       *hsuBlockMemPtr,
    OUT    GT_BOOL                     *exportCompletePtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChHsuExport);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, dataType, iteratorPtr, hsuBlockMemSizePtr, hsuBlockMemPtr, exportCompletePtr));

    rc = internal_cpssDxChHsuExport(dataType, iteratorPtr, hsuBlockMemSizePtr, hsuBlockMemPtr, exportCompletePtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, dataType, iteratorPtr, hsuBlockMemSizePtr, hsuBlockMemPtr, exportCompletePtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}


/*******************************************************************************
* prvCpssDxChHsuLpmDbDataBlockExport
*
* DESCRIPTION:
*       This function exports LPM DB HSU data block to survived restart
*       memory area supplied by application.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       iteratorPtr            - pointer to iterator, to start - set iterator to 0.
*       hsuBlockMemSizePtr     - pointer hsu block data size supposed to be exported
*                                in current iteration. The minimal value is 1k bytes.
*       hsuBlockMemPtr         - pointer to HSU survived restart memory area
*
* OUTPUTS:
*       iteratorPtr            - the iterator - points to the point from which
*                                process will be continued in future iteration.
*       hsuBlockMemSizePtr     - pointer to hsu block data size exported in current iteration.
*       exportCompletePtr      - GT_TRUE -  HSU data export is completed.
*                                GT_FALSE - HSU data export is not completed.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong hsuBlockMemSize.
*       GT_BAD_PTR               - on NULL pointer
*       GT_FAIL                  - otherwise
*
* COMMENTS:
*       none.
*
*******************************************************************************/
GT_STATUS prvCpssDxChHsuLpmDbDataBlockExport
(
    INOUT  GT_UINTPTR                    *iteratorPtr,
    INOUT  GT_U32                        *hsuBlockMemSizePtr,
    IN     GT_U8                         *hsuBlockMemPtr,
    OUT    GT_BOOL                       *exportCompletePtr
)
{
    GT_UINTPTR                    slIterator = 0;
    GT_U32                        lpmDbId = 0;
    GT_U32                        i;
    GT_U32                        supposedHsuBlockSize = 0;
    GT_STATUS                     rc = GT_OK ;
    CPSS_DXCH_HSU_DATA_BLOCK_HEADER_STC *header = NULL;
    GT_U32 currentHsuBlockVersion = 0;
    CPSS_NULL_PTR_CHECK_MAC(hsuBlockMemPtr);
    CPSS_NULL_PTR_CHECK_MAC(iteratorPtr);
    CPSS_NULL_PTR_CHECK_MAC(exportCompletePtr);

    /* Upon first iteration TLV header should be saved into hsu memory */
    if (*iteratorPtr == 0) /* first iteration */
    {
       if(*hsuBlockMemSizePtr <= sizeof(CPSS_DXCH_HSU_DATA_BLOCK_HEADER_STC))
        {
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
        }
        /* allocate lpm dbs iterator */
       ipLpmDbsIter =
           (PRV_CPSS_DXCH_HSU_LPM_DB_ITERATOR_STC*)cpssOsMalloc(sizeof(PRV_CPSS_DXCH_HSU_LPM_DB_ITERATOR_STC));
       if (ipLpmDbsIter == NULL)
       {
           CPSS_LOG_ERROR_AND_RETURN_MAC(GT_OUT_OF_CPU_MEM, LOG_ERROR_NO_MSG);
       }
       /*reset it */
       cpssOsMemSet(ipLpmDbsIter,0,sizeof(PRV_CPSS_DXCH_HSU_LPM_DB_ITERATOR_STC));

        /* calculate Lpm DB size for export */
        rc = prvCpssDxChHsuLpmDbDataBlockSizeGet(&ipLpmDbsIter->lpmDbSize);
        if (rc != GT_OK)
        {
            return rc;
        }
        rc = prvCpssDxChHsuDataBlockVersionGet(&currentHsuBlockVersion);
        if (rc != GT_OK)
        {
            return rc;
        }

        header = (CPSS_DXCH_HSU_DATA_BLOCK_HEADER_STC *)hsuBlockMemPtr;
        header->length = ipLpmDbsIter->lpmDbSize;
        header->type = CPSS_DXCH_HSU_DATA_TYPE_LPM_DB_E;
        header->version = currentHsuBlockVersion;
        header->reserved1 = 0;
        header->reserved2 = 0;
        hsuBlockMemPtr = (GT_U8*)(++header);
        *hsuBlockMemSizePtr-= sizeof(CPSS_DXCH_HSU_DATA_BLOCK_HEADER_STC);
        ipLpmDbsIter->accumulatedLpmDbSize = sizeof(CPSS_DXCH_HSU_DATA_BLOCK_HEADER_STC);
        ipLpmDbsIter->isLpmDbProcessed = cpssOsMalloc(sizeof(GT_BOOL) * PRV_CPSS_MAX_PP_DEVICES_CNS);
        if (ipLpmDbsIter->isLpmDbProcessed == NULL)
        {
            cpssOsFree(ipLpmDbsIter);
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_OUT_OF_CPU_MEM, LOG_ERROR_NO_MSG);
        }
        for (i=0; i < PRV_CPSS_MAX_PP_DEVICES_CNS; i++)
        {
            ipLpmDbsIter->isLpmDbProcessed[i] = GT_FALSE;
        }
    }
    else
    {
        if (ipLpmDbsIter->accumulatedLpmDbSize == 0)
        {
            /* it is not first iteration  -- accumulatedLpmDbSize should be larger
               than 0. Wrong iterator value possibly was applied */
            cpssOsFree(ipLpmDbsIter->isLpmDbProcessed);
            cpssOsFree(ipLpmDbsIter);
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
        }

        if (ipLpmDbsIter->accumulatedLpmDbSize >= ipLpmDbsIter->lpmDbSize)
        {
            *exportCompletePtr = GT_TRUE;
            cpssOsFree(ipLpmDbsIter->isLpmDbProcessed);
            cpssOsFree(ipLpmDbsIter);
            return GT_OK;
        }
    }

    while (1)
    {
        supposedHsuBlockSize = *hsuBlockMemSizePtr;
        rc =  prvCpssDxChIpLpmDbIdGetNext(&lpmDbId,&slIterator);
        if (rc == GT_NO_MORE)
        {
            break;
        }
        if (rc != GT_OK)
        {
            cpssOsFree(ipLpmDbsIter->isLpmDbProcessed);
            cpssOsFree(ipLpmDbsIter);
            return rc;
        }

        if (ipLpmDbsIter->isLpmDbProcessed[lpmDbId] == GT_TRUE)
        {
            continue;
        }

        rc = prvCpssDxChIpLpmDbGetL3(lpmDbId, NULL,hsuBlockMemPtr,hsuBlockMemSizePtr,iteratorPtr);
        if (rc != GT_OK)
        {
            cpssOsFree(ipLpmDbsIter->isLpmDbProcessed);
            cpssOsFree(ipLpmDbsIter);
            return rc;
        }
        if (*iteratorPtr == 0)
        {
            /* export for current lpm db is finished : mark in array*/
            ipLpmDbsIter->isLpmDbProcessed[lpmDbId] = GT_TRUE;
            ipLpmDbsIter->accumulatedLpmDbSize+=(supposedHsuBlockSize - *hsuBlockMemSizePtr);
            /* update  hsuBlockMemPtr for next lpm db */
            hsuBlockMemPtr = hsuBlockMemPtr + (supposedHsuBlockSize - *hsuBlockMemSizePtr);
        }
        else
        {
            /* export is not finished, next iteration needed */
            ipLpmDbsIter->accumulatedLpmDbSize+=(supposedHsuBlockSize - *hsuBlockMemSizePtr);
            return GT_OK;
        }
    }
    /* now all lpm dbs are processed */
    if (ipLpmDbsIter->accumulatedLpmDbSize == ipLpmDbsIter->lpmDbSize)
    {
        *exportCompletePtr = GT_TRUE;
        rc = GT_OK;
    }
    else
    {
        rc = GT_FAIL;
    }
    cpssOsFree(ipLpmDbsIter->isLpmDbProcessed);
    cpssOsFree(ipLpmDbsIter);
    return  rc;
}



/*******************************************************************************
* prvCpssDxChHsuLpmDbDataBlockImport
*
* DESCRIPTION:
*       This function imports LPM DB HSU data block from survived restart
*       memory area supplied by application.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       iteratorPtr            - pointer to iterator, to start - set iterator to 0.
*       hsuBlockMemSizePtr     - pointer hsu block data size supposed to be exported
*                                in current iteration. The minimal value is 1k bytes.
*       hsuBlockMemPtr         - pointer to HSU survived restart memory area
*
* OUTPUTS:
*       iteratorPtr            - the iterator - points to the point from which
*                                process will be continued in future iteration.
*       hsuBlockMemSizePtr     - pointer to hsu block data size exported in current iteration.
*       importCompletePtr      - GT_TRUE -  HSU data import is completed.
*                                GT_FALSE - HSU data import is not completed.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong hsuBlockMemSize.
*       GT_BAD_PTR               - on NULL pointer
*       GT_FAIL                  - otherwise
*
* COMMENTS:
*       none.
*
*******************************************************************************/
GT_STATUS prvCpssDxChHsuLpmDbDataBlockImport
(
    INOUT  GT_UINTPTR                    *iteratorPtr,
    INOUT  GT_U32                        *hsuBlockMemSizePtr,
    IN     GT_U8                         *hsuBlockMemPtr,
    OUT    GT_BOOL                       *importCompletePtr
)
{
    GT_UINTPTR                    slIterator = 0;
    GT_U32                        lpmDbId = 0;
    GT_U32                        supposedHsuBlockSize = 0;
    GT_U32                        i;
    GT_STATUS                     rc = GT_OK ;
    CPSS_DXCH_HSU_DATA_BLOCK_HEADER_STC *header = NULL;
    GT_U32                        currentHsuBlockVersion;

    CPSS_NULL_PTR_CHECK_MAC(hsuBlockMemPtr);
    CPSS_NULL_PTR_CHECK_MAC(iteratorPtr);
    CPSS_NULL_PTR_CHECK_MAC(importCompletePtr);
    CPSS_NULL_PTR_CHECK_MAC(hsuBlockMemSizePtr);

    /* Upon first iteration TLV header should be retrived from hsu memory */
    if (*iteratorPtr == 0) /* first iteration */
    {
        if (*hsuBlockMemSizePtr <= sizeof(CPSS_DXCH_HSU_DATA_BLOCK_HEADER_STC))
        {
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
        }
        header = (CPSS_DXCH_HSU_DATA_BLOCK_HEADER_STC *)hsuBlockMemPtr;
        if (header->type != CPSS_DXCH_HSU_DATA_TYPE_LPM_DB_E)
        {
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_VALUE, LOG_ERROR_NO_MSG);
        }
        if ((header->reserved1 != 0) || (header->reserved2 != 0) )
        {
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_VALUE, LOG_ERROR_NO_MSG);
        }
        rc = prvCpssDxChHsuDataBlockVersionGet(&currentHsuBlockVersion);
        if (rc != GT_OK)
        {
            return rc;
        }

        if (header->version != currentHsuBlockVersion)
        {
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_SUPPORTED, LOG_ERROR_NO_MSG);
        }
        /* allocate lpm dbs iterator */
        ipLpmDbsIter =
            (PRV_CPSS_DXCH_HSU_LPM_DB_ITERATOR_STC*)cpssOsMalloc(sizeof(PRV_CPSS_DXCH_HSU_LPM_DB_ITERATOR_STC));
        if (ipLpmDbsIter == NULL)
        {
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_OUT_OF_CPU_MEM, LOG_ERROR_NO_MSG);
        }
        /*reset it */
        cpssOsMemSet(ipLpmDbsIter,0,sizeof(PRV_CPSS_DXCH_HSU_LPM_DB_ITERATOR_STC));

        /* Retrieve full Lpm DB size for import */
        ipLpmDbsIter->lpmDbSize = header->length;
        hsuBlockMemPtr = (GT_U8*)(++header);
        *hsuBlockMemSizePtr-= sizeof(CPSS_DXCH_HSU_DATA_BLOCK_HEADER_STC);
        ipLpmDbsIter->accumulatedLpmDbSize = sizeof(CPSS_DXCH_HSU_DATA_BLOCK_HEADER_STC);
        ipLpmDbsIter->isLpmDbProcessed = cpssOsMalloc(sizeof(GT_BOOL) * PRV_CPSS_MAX_PP_DEVICES_CNS);
        if (ipLpmDbsIter->isLpmDbProcessed == NULL)
        {
            cpssOsFree(ipLpmDbsIter);
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_OUT_OF_CPU_MEM, LOG_ERROR_NO_MSG);
        }
        for (i=0; i < PRV_CPSS_MAX_PP_DEVICES_CNS; i++)
        {
            ipLpmDbsIter->isLpmDbProcessed[i] = GT_FALSE;
        }
    }
    else
    {
        if (ipLpmDbsIter->accumulatedLpmDbSize == 0)
        {
            /* it is not first iteration  -- accumulatedLpmDbSize should be larger
               than 0. Wrong iterator value possibly was applied */
            cpssOsFree(ipLpmDbsIter->isLpmDbProcessed);
            cpssOsFree(ipLpmDbsIter);
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
        }
        if (ipLpmDbsIter->accumulatedLpmDbSize >= ipLpmDbsIter->lpmDbSize)
        {
            *importCompletePtr = GT_TRUE;
            cpssOsFree(ipLpmDbsIter->isLpmDbProcessed);
            cpssOsFree(ipLpmDbsIter);
            return GT_OK;
        }
    }
    while (1)
    {
        supposedHsuBlockSize = *hsuBlockMemSizePtr;
        rc =  prvCpssDxChIpLpmDbIdGetNext(&lpmDbId,&slIterator);
        if (rc == GT_NO_MORE)
        {
            break;
        }
        if (rc != GT_OK)
        {
            cpssOsFree(ipLpmDbsIter->isLpmDbProcessed);
            cpssOsFree(ipLpmDbsIter);
            return rc;
        }

        if (ipLpmDbsIter->isLpmDbProcessed[lpmDbId] == GT_TRUE)
        {
            continue;
        }

        rc = prvCpssDxChIpLpmDbSetL3(lpmDbId,
                                     hsuBlockMemPtr,
                                     hsuBlockMemSizePtr,
                                     iteratorPtr);
        if (rc != GT_OK)
        {
            cpssOsFree(ipLpmDbsIter->isLpmDbProcessed);
            cpssOsFree(ipLpmDbsIter);
            return rc;
        }
        if (*iteratorPtr == 0)
        {
            /* import for current lpm db is finished : mark in array*/
            ipLpmDbsIter->isLpmDbProcessed[lpmDbId] = GT_TRUE;
            ipLpmDbsIter->accumulatedLpmDbSize += (supposedHsuBlockSize - *hsuBlockMemSizePtr);
            /* update  hsuBlockMemPtr for next lpm db */
            hsuBlockMemPtr = hsuBlockMemPtr + (supposedHsuBlockSize - *hsuBlockMemSizePtr);
        }
        else
        {
            /* import is not finished, next iteration needed */
            ipLpmDbsIter->accumulatedLpmDbSize += (supposedHsuBlockSize - *hsuBlockMemSizePtr);
            return GT_OK;
        }
    }
    /* now all lpm dbs are processed */
    if (ipLpmDbsIter->accumulatedLpmDbSize >= ipLpmDbsIter->lpmDbSize)
    {
        *importCompletePtr = GT_TRUE;
        rc = GT_OK;
    }
    else
    {
        rc = GT_FAIL;
    }
    cpssOsFree(ipLpmDbsIter->isLpmDbProcessed);
    cpssOsFree(ipLpmDbsIter);
    return  rc;
}


/*******************************************************************************
* internal_cpssDxChHsuImport
*
* DESCRIPTION:
*       This function imports required HSU data block from survived restart
*       memory area supplied by application.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       dataType               - hsu data type.
*       iteratorPtr            - pointer to iterator, to start - set iterator to 0.
*       hsuBlockMemSizePtr     - pointer to hsu block data size supposed to be
*                                imported in current iteration.The minimal value
*                                is 1k bytes.
*                                The import by one single iteration is performed
*                                when hsuBlockMemSizePtr  points to
*                                CPSS_DXCH_HSU_SINGLE_ITERATION_CNS value.
*                                The size (saved in export operation) is retrieved
*                                from hsu data block header.
*       hsuBlockMemPtr         - pointer to HSU survived restart memory area.
*
* OUTPUTS:
*       iteratorPtr            - the iterator - points to the point from which
*                                process will be continued in future iteration.
*       hsuBlockMemSizePtr     - pointer to hsu block data size that was not used
*                                in current iteration.
*       importCompletePtr      - GT_TRUE -  HSU data import is completed.
*                                GT_FALSE - HSU data import is not completed.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PTR               - on NULL pointer
*       GT_FAIL                  - otherwise
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       none.
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChHsuImport
(
    IN     CPSS_DXCH_HSU_DATA_TYPE_ENT dataType,
    INOUT  GT_UINTPTR                  *iteratorPtr,
    INOUT  GT_U32                      *hsuBlockMemSizePtr,
    IN     GT_U8                       *hsuBlockMemPtr,
    OUT    GT_BOOL                     *importCompletePtr
)
{
    GT_STATUS rc = GT_OK;
    GT_U32 origlBlockMemSize = 0 ;
    GT_BOOL oneIteration = GT_FALSE;

    CPSS_NULL_PTR_CHECK_MAC(iteratorPtr);
    CPSS_NULL_PTR_CHECK_MAC(hsuBlockMemPtr);
    CPSS_NULL_PTR_CHECK_MAC(hsuBlockMemSizePtr);
    CPSS_NULL_PTR_CHECK_MAC(importCompletePtr);
    if ( (*hsuBlockMemSizePtr != CPSS_DXCH_HSU_SINGLE_ITERATION_CNS) &&
         (*hsuBlockMemSizePtr < BIT_10) )
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }
    if (*hsuBlockMemSizePtr == CPSS_DXCH_HSU_SINGLE_ITERATION_CNS)
    {
        /* single iteration */
        oneIteration = GT_TRUE;
        *iteratorPtr = 0;
    }
    /* all hsu data is processed by iterations */
    if (*iteratorPtr == 0)  /* first iteration */
    {
        /* allocate frame iterator*/
        frameIterPtr =
            (PRV_CPSS_DXCH_HSU_MAIN_FRAME_ITERATOR_STC*)cpssOsMalloc(sizeof(PRV_CPSS_DXCH_HSU_MAIN_FRAME_ITERATOR_STC));
        if (frameIterPtr == NULL)
        {
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_OUT_OF_CPU_MEM, LOG_ERROR_NO_MSG);
        }
        /*reset it */
        cpssOsMemSet(frameIterPtr,0,sizeof(PRV_CPSS_DXCH_HSU_MAIN_FRAME_ITERATOR_STC));

        frameIterPtr->hsuCurrentPhase = PRV_CPSS_DXCH_HSU_DATA_MAIN_PHASE_LPM_DB_E;
    }
    *importCompletePtr = GT_FALSE;
    switch(dataType)
    {
    case CPSS_DXCH_HSU_DATA_TYPE_LPM_DB_E:
        rc = prvCpssDxChHsuLpmDbDataBlockImport(iteratorPtr,
                                                hsuBlockMemSizePtr,
                                                hsuBlockMemPtr,
                                                importCompletePtr);
        break;
    case CPSS_DXCH_HSU_DATA_TYPE_GLOBAL_E:
        rc = prvCpssDxChHsuGlobalDataBlockImport(iteratorPtr,
                                                 hsuBlockMemSizePtr,
                                                 hsuBlockMemPtr,
                                                 importCompletePtr);

        break;
    case CPSS_DXCH_HSU_DATA_TYPE_ALL_E:
        if (frameIterPtr->hsuCurrentPhase == PRV_CPSS_DXCH_HSU_DATA_MAIN_PHASE_LPM_DB_E)
        {
            origlBlockMemSize = *hsuBlockMemSizePtr;
            rc = prvCpssDxChHsuLpmDbDataBlockImport(iteratorPtr,
                                                    hsuBlockMemSizePtr,
                                                    hsuBlockMemPtr,
                                                    importCompletePtr);
            if (rc != GT_OK)
            {
                cpssOsFree(frameIterPtr);
                return rc;
            }
            frameIterPtr->hsuAllAccumulatedSize += origlBlockMemSize - *hsuBlockMemSizePtr;
            if (*importCompletePtr == GT_FALSE)
            {
                if (oneIteration == GT_TRUE)
                {
                    cpssOsFree(frameIterPtr);
                    CPSS_LOG_ERROR_AND_RETURN_MAC(GT_FAIL, LOG_ERROR_NO_MSG);
                }
                return GT_OK;
            }
            else
            {
                /* the stage was finished */
                frameIterPtr->hsuCurrentPhase = PRV_CPSS_DXCH_HSU_DATA_MAIN_PHASE_GLOBAL_E;
                *importCompletePtr = GT_FALSE;
            }
            hsuBlockMemPtr = (GT_U8*)(hsuBlockMemPtr + (origlBlockMemSize - *hsuBlockMemSizePtr));
        }
        if (frameIterPtr->hsuCurrentPhase == PRV_CPSS_DXCH_HSU_DATA_MAIN_PHASE_GLOBAL_E)
        {
            origlBlockMemSize = *hsuBlockMemSizePtr;
            rc = prvCpssDxChHsuGlobalDataBlockImport(iteratorPtr,
                                                     hsuBlockMemSizePtr,
                                                     hsuBlockMemPtr,
                                                     importCompletePtr);
            if (rc != GT_OK)
            {
                cpssOsFree(frameIterPtr);
                return rc;
            }
            frameIterPtr->hsuAllAccumulatedSize += origlBlockMemSize - *hsuBlockMemSizePtr;
            if (*importCompletePtr == GT_FALSE)
            {
                if (oneIteration == GT_TRUE)
                {
                    cpssOsFree(frameIterPtr);
                    CPSS_LOG_ERROR_AND_RETURN_MAC(GT_FAIL, LOG_ERROR_NO_MSG);
                }
                return GT_OK;
            }
            else
            {
                /* the last stage was successfully finished */
                *iteratorPtr = 0;
                rc = GT_OK;
            }
        }
        break;
    default:
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }
    if ( (*iteratorPtr == 0) && (*importCompletePtr == GT_TRUE) )
    {
        /* all stages were done -- free iterator */
        cpssOsFree(frameIterPtr);
    }
    return rc;
}

/*******************************************************************************
* cpssDxChHsuImport
*
* DESCRIPTION:
*       This function imports required HSU data block from survived restart
*       memory area supplied by application.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       dataType               - hsu data type.
*       iteratorPtr            - pointer to iterator, to start - set iterator to 0.
*       hsuBlockMemSizePtr     - pointer to hsu block data size supposed to be
*                                imported in current iteration.The minimal value
*                                is 1k bytes.
*                                The import by one single iteration is performed
*                                when hsuBlockMemSizePtr  points to
*                                CPSS_DXCH_HSU_SINGLE_ITERATION_CNS value.
*                                The size (saved in export operation) is retrieved
*                                from hsu data block header.
*       hsuBlockMemPtr         - pointer to HSU survived restart memory area.
*
* OUTPUTS:
*       iteratorPtr            - the iterator - points to the point from which
*                                process will be continued in future iteration.
*       hsuBlockMemSizePtr     - pointer to hsu block data size that was not used
*                                in current iteration.
*       importCompletePtr      - GT_TRUE -  HSU data import is completed.
*                                GT_FALSE - HSU data import is not completed.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PTR               - on NULL pointer
*       GT_FAIL                  - otherwise
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       none.
*
*******************************************************************************/
GT_STATUS cpssDxChHsuImport
(
    IN     CPSS_DXCH_HSU_DATA_TYPE_ENT dataType,
    INOUT  GT_UINTPTR                  *iteratorPtr,
    INOUT  GT_U32                      *hsuBlockMemSizePtr,
    IN     GT_U8                       *hsuBlockMemPtr,
    OUT    GT_BOOL                     *importCompletePtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChHsuImport);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, dataType, iteratorPtr, hsuBlockMemSizePtr, hsuBlockMemPtr, importCompletePtr));

    rc = internal_cpssDxChHsuImport(dataType, iteratorPtr, hsuBlockMemSizePtr, hsuBlockMemPtr, importCompletePtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, dataType, iteratorPtr, hsuBlockMemSizePtr, hsuBlockMemPtr, importCompletePtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* prvCpssDxChHsuGlobalDataBlockImport
*
* DESCRIPTION:
*       This function imports Global HSU data block from survived restart
*       memory area supplied by application.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       iteratorPtr            - pointer to iterator, to start - set iterator to 0.
*       hsuBlockMemSizePtr     - pointer hsu block data size supposed to be exported
*                                in current iteration. The minimal value is 1k bytes.
*       hsuBlockMemPtr         - pointer to HSU survived restart memory area
*
* OUTPUTS:
*       iteratorPtr            - the iterator - points to the point from which
*                                process will be continued in future iteration.
*       hsuBlockMemSizePtr     - pointer to hsu block data size imported in current iteration.
*       importCompletePtr      - GT_TRUE -  HSU data import is completed.
*                                GT_FALSE - HSU data import is not completed.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong hsuBlockMemSize, wrong iterator.
*       GT_BAD_PTR               - on NULL pointer
*       GT_FAIL                  - otherwise
*
* COMMENTS:
*       none.
*
*******************************************************************************/
GT_STATUS prvCpssDxChHsuGlobalDataBlockImport
(
    INOUT  GT_UINTPTR                    *iteratorPtr,
    INOUT  GT_U32                        *hsuBlockMemSizePtr,
    IN     GT_U8                         *hsuBlockMemPtr,
    OUT    GT_BOOL                       *importCompletePtr
)
{
    PRV_CPSS_TRUNK_DB_INFO_STC *genTrunkDbPtr = NULL;
    GT_STATUS   rc = GT_OK ;
    GT_U32 currentHsuBlockVersion = 0;
    CPSS_DXCH_HSU_DATA_BLOCK_HEADER_STC *header = NULL;
    PRV_CPSS_DXCH_HSU_GLOBAL_ITERATOR_STC  *currentIterPtr = NULL;
    PRV_CPSS_DXCH_HSU_GLOBAL_DATA_STAGE_ENT currentStage;
    PRV_CPSS_DXCH_HSU_STAGE_STC stageInfoArray[PRV_CPSS_DXCH_HSU_GLOBAL_LAST_STAGE_E];
    GT_U8 devNum;
    GT_BOOL enoughMemoryForStage = GT_FALSE;
    GT_U32 i;
    GT_U32 port;

    CPSS_NULL_PTR_CHECK_MAC(hsuBlockMemPtr);
    CPSS_NULL_PTR_CHECK_MAC(iteratorPtr);
    CPSS_NULL_PTR_CHECK_MAC(importCompletePtr);

    *importCompletePtr = GT_FALSE;
    currentIterPtr = (PRV_CPSS_DXCH_HSU_GLOBAL_ITERATOR_STC*)*iteratorPtr;
    /* Upon first iteration TLV header should be retrieved from  hsu memory */
    if (currentIterPtr == NULL) /* first iteration */
    {
        if (*hsuBlockMemSizePtr <= sizeof(CPSS_DXCH_HSU_DATA_BLOCK_HEADER_STC))
        {
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
        }
        header = (CPSS_DXCH_HSU_DATA_BLOCK_HEADER_STC *)hsuBlockMemPtr;
        if (header->type != CPSS_DXCH_HSU_DATA_TYPE_GLOBAL_E)
        {
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_VALUE, LOG_ERROR_NO_MSG);
        }
        if ((header->reserved1 != 0) || (header->reserved2 != 0) )
        {
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_VALUE, LOG_ERROR_NO_MSG);
        }
        rc = prvCpssDxChHsuDataBlockVersionGet(&currentHsuBlockVersion);
        if (rc != GT_OK)
        {
            return rc;
        }

        if (header->version != currentHsuBlockVersion)
        {
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_SUPPORTED, LOG_ERROR_NO_MSG);
        }
        /* we need to allocate the iterator */
        currentIterPtr =
            (PRV_CPSS_DXCH_HSU_GLOBAL_ITERATOR_STC*)cpssOsMalloc(sizeof(PRV_CPSS_DXCH_HSU_GLOBAL_ITERATOR_STC));
        if (currentIterPtr == NULL)
        {
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_OUT_OF_CPU_MEM, LOG_ERROR_NO_MSG);
        }
        /*reset it */
        cpssOsMemSet(currentIterPtr,0,sizeof(PRV_CPSS_DXCH_HSU_GLOBAL_ITERATOR_STC));

        /* Retrieve full Lpm DB size for import */
        currentIterPtr->hsuBlockGlobalSize = header->length;
        hsuBlockMemPtr = (GT_U8*)(++header);
        *hsuBlockMemSizePtr-= sizeof(CPSS_DXCH_HSU_DATA_BLOCK_HEADER_STC);
        currentIterPtr->accumulatedGlobalSize = sizeof(CPSS_DXCH_HSU_DATA_BLOCK_HEADER_STC);

        /* set the starting stage */
        currentIterPtr->currStage = PRV_CPSS_DXCH_HSU_GEN_PP_CONFIG_STAGE_E;
        currentIterPtr->genIterator.currStage = PRV_CPSS_GEN_HSU_CPSS_STAGE_E;
        currentIterPtr->genIterator.magic = PRV_CPSS_DXCH_HSU_ITERATOR_MAGIC_NUMBER_CNC;
        currentIterPtr->genIterator.currStageMemPtr = NULL;
        currentIterPtr->devNum = PRV_CPSS_MAX_PP_DEVICES_CNS;
        *iteratorPtr = (GT_UINTPTR)currentIterPtr;
    }
    else
    {
        if (currentIterPtr->accumulatedGlobalSize == 0)
        {
            /* it is not first iteration  -- accumulatedGlobalSize should be larger
               than 0. Wrong iterator value possibly was applied */
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
        }

        if (currentIterPtr->genIterator.magic != PRV_CPSS_DXCH_HSU_ITERATOR_MAGIC_NUMBER_CNC)
        {
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
        }
        if (currentIterPtr->accumulatedGlobalSize >= currentIterPtr->hsuBlockGlobalSize)
        {
            *importCompletePtr = GT_TRUE;
            return GT_OK;
        }
    }
    /* make cpss generic and cpss driver global data export */
    if (currentIterPtr->genStageComplete == GT_FALSE)
    {
        rc = prvCpssGenHsuExportImport(PRV_CPSS_HSU_IMPORT_E,
                                       &currentIterPtr->genIterator,
                                       hsuBlockMemSizePtr,
                                       &hsuBlockMemPtr,
                                       &currentIterPtr->accumulatedGlobalSize,
                                       &currentIterPtr->genStageComplete);
        if (rc != GT_OK)
        {
            return rc;
        }
        if (currentIterPtr->genStageComplete == GT_FALSE)
        {
            return GT_OK;
        }
    }
    /* in this point generic stage was done and DXCH globals should be processed */
#if 0
    if (currentIterPtr->systemDataProcessed == GT_FALSE)
    {
        if ( currentIterPtr->currStage == PRV_CPSS_DXCH_HSU_SYS_GLOBAL_STAGE_E)
        {
            if (currentIterPtr->genIterator.currStageMemPtr == NULL)
            {
                currentIterPtr->genIterator.currStageMemPtr = (GT_U8*)(&sysGlobalInfo);
                currentIterPtr->genIterator.currentStageRemainedSize = sizeof(PRV_CPSS_DXCH_SYS_GLOBAL_DATA_STC);
            }
            if (*hsuBlockMemSizePtr >= currentIterPtr->genIterator.currentStageRemainedSize )
            {
                enoughMemoryForStage = GT_TRUE;
            }
            else
            {
                enoughMemoryForStage = GT_FALSE;
            }

            rc = prvCpssDxChHsuExportImportDataHandling(PRV_CPSS_HSU_IMPORT_E,
                                                          currentIterPtr,
                                                          hsuBlockMemSizePtr,
                                                          &hsuBlockMemPtr,
                                                          &currentIterPtr->accumulatedGlobalSize);
            if (rc != GT_OK)
            {
                return rc;
            }
            if (enoughMemoryForStage == GT_FALSE)
            {
                return rc;
            }
        }
        /* At this stage all system globals are handled*/
        currentIterPtr->systemDataProcessed = GT_TRUE;
    }
#endif
    /* make export for data per device */
    for (devNum = 0; devNum < PRV_CPSS_MAX_PP_DEVICES_CNS; devNum++)
    {
        if ( (prvCpssPpConfig[devNum] == NULL) ||
             (PRV_CPSS_DXCH_FAMILY_CHECK_MAC(devNum) == 0) )
        {
            continue;
        }
        cpssOsMemSet(stageInfoArray,0,sizeof(stageInfoArray));
        /* init stage data */
        rc = prvCpssDxChHsuInitStageArray(devNum,
                                          PRV_CPSS_DXCH_HSU_STAGE_ARRAY_DATA_TYPE_ALL_E,
                                          stageInfoArray);
        if (rc != GT_OK)
        {
            return rc;
        }

        for( currentStage = PRV_CPSS_DXCH_HSU_GEN_PP_CONFIG_STAGE_E;
             currentStage < PRV_CPSS_DXCH_HSU_GLOBAL_LAST_STAGE_E;
             currentStage++)
        {
            if ( (currentIterPtr->currStage == currentStage) &&
                 ( (currentIterPtr->devNum == devNum) || /* continue handling old device */
                   (currentIterPtr->devNum == PRV_CPSS_MAX_PP_DEVICES_CNS /* new device */)) )
            {
                if (currentIterPtr->genIterator.currStageMemPtr == NULL)
                {
                    currentIterPtr->genIterator.currStageMemPtr = stageInfoArray[currentStage].stageAddress;
                    currentIterPtr->genIterator.currentStageRemainedSize = stageInfoArray[currentStage].stageSize;
                    if(currentStage == PRV_CPSS_DXCH_HSU_GEN_PP_CONFIG_STAGE_E)
                    {
                        /* pp config generic stage is started */
                        /* At this stage save data that shouldn't be overrided by import */
                        genTrunkDbPtr = PRV_CPSS_DEV_TRUNK_INFO_MAC(devNum);
                        currentIterPtr->virtualFunctionsPtr = genTrunkDbPtr->virtualFunctionsPtr;
                        currentIterPtr->trunksArrayPtr = genTrunkDbPtr->trunksArray;
                        currentIterPtr->allMembersArrayPtr = genTrunkDbPtr->allMembersArray;
                        currentIterPtr->allMembersIsLocalArrayPtr = genTrunkDbPtr->allMembersIsLocalArray;
                        currentIterPtr->phyPortInfoArrayPtr = PRV_CPSS_PP_MAC(devNum)->phyPortInfoArray;
                        /* save cpssInitSystem portMacObjPtr values -- application func PTR. These pointers
                           get new values during cpssInitSystem on new image */
                        currentIterPtr->portMacObjPtr = cpssOsMalloc(sizeof(CPSS_MACDRV_OBJ_STC*)*PRV_CPSS_PP_MAC(devNum)->numOfPorts);
                        if (currentIterPtr->portMacObjPtr == NULL)
                        {
                            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_OUT_OF_CPU_MEM, LOG_ERROR_NO_MSG);
                        }
                        cpssOsMemSet(currentIterPtr->portMacObjPtr,0,sizeof(CPSS_MACDRV_OBJ_STC*)*PRV_CPSS_PP_MAC(devNum)->numOfPorts);

                         for (port = 0; port < PRV_CPSS_PP_MAC(devNum)->numOfPorts; port++)
                         {
                             if(!CPSS_PORTS_BMP_IS_PORT_SET_MAC(&PRV_CPSS_PP_MAC(devNum)->existingPorts , port))
                             {
                                 continue;
                             }
                             currentIterPtr->portMacObjPtr[port] = PRV_CPSS_PP_MAC(devNum)->phyPortInfoArray[port].portMacObjPtr;
                         }
                        if ((systemRecoveryInfo.systemRecoveryMode.continuousRx == GT_FALSE)         ||
                             (systemRecoveryInfo.systemRecoveryMode.continuousAuMessages == GT_FALSE)||
                             (systemRecoveryInfo.systemRecoveryMode.continuousTx == GT_FALSE)        ||
                             (systemRecoveryInfo.systemRecoveryMode.continuousFuMessages == GT_FALSE))
                        {
                            /* save all descriptors                                              */
                            /* In this mode rx/tx/au/fu data is initialized after cpssInitSystem */
                            /* and shouldn't be overwritten by import operation                  */
                            currentIterPtr->allDescListPtr = cpssOsMalloc(sizeof(PRV_CPSS_INTERRUPT_CTRL_STC));
                            if (currentIterPtr->allDescListPtr == NULL)
                            {
                                CPSS_LOG_ERROR_AND_RETURN_MAC(GT_OUT_OF_CPU_MEM, LOG_ERROR_NO_MSG);
                            }

                            cpssOsMemCpy(currentIterPtr->allDescListPtr,
                                     &(PRV_CPSS_PP_MAC(devNum)->intCtrl),
                                     sizeof(PRV_CPSS_INTERRUPT_CTRL_STC));

                            currentIterPtr->txGeneratorPacketIdDb.cookie = PRV_CPSS_PP_MAC(devNum)->intCtrl.txGeneratorPacketIdDb.cookie;
                            currentIterPtr->txGeneratorPacketIdDb.freeLinkedList = PRV_CPSS_PP_MAC(devNum)->intCtrl.txGeneratorPacketIdDb.freeLinkedList;
                            currentIterPtr->txGeneratorPacketIdDb.generatorsTotalDesc = PRV_CPSS_PP_MAC(devNum)->intCtrl.txGeneratorPacketIdDb.generatorsTotalDesc;
                            currentIterPtr->txGeneratorPacketIdDb.firstFree = PRV_CPSS_PP_MAC(devNum)->intCtrl.txGeneratorPacketIdDb.firstFree;
                        }

                        currentIterPtr->phyInfoPtr = cpssOsMalloc(sizeof(PRV_CPSS_PHY_INFO_STC));
                        if (currentIterPtr->phyInfoPtr == NULL)
                        {
                            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_OUT_OF_CPU_MEM, LOG_ERROR_NO_MSG);
                        }

                        cpssOsMemCpy(currentIterPtr->phyInfoPtr,
                                     &(PRV_CPSS_PP_MAC(devNum)->phyInfo),
                                     sizeof(PRV_CPSS_PHY_INFO_STC));
                    }
                    if(PRV_CPSS_SIP_5_CHECK_MAC(devNum))
                    {
                        if(currentStage == PRV_CPSS_DXCH_HSU_SERDES_LANE0_STAGE_E)
                        {
                            rc = prvCpssDxChHsuSerdesImportPreparation(devNum);
                            if(rc != GT_OK)
                                return rc;

                            /* update addresses for next stages */
                            rc = prvCpssDxChHsuInitStageArray(devNum,
                                                              PRV_CPSS_DXCH_HSU_STAGE_ARRAY_DATA_TYPE_ALL_E,
                                                              stageInfoArray);
                            if (rc != GT_OK)
                            {
                                return rc;
                            }
                        }
                    }
                }

                if (*hsuBlockMemSizePtr >= currentIterPtr->genIterator.currentStageRemainedSize )
                {
                    enoughMemoryForStage = GT_TRUE;
                }
                else
                {
                    enoughMemoryForStage = GT_FALSE;
                }

                rc = prvCpssDxChHsuExportImportDataHandling(PRV_CPSS_HSU_IMPORT_E,
                                                              currentIterPtr,
                                                              hsuBlockMemSizePtr,
                                                              &hsuBlockMemPtr,
                                                              &currentIterPtr->accumulatedGlobalSize);
                if (rc != GT_OK)
                {
                    return rc;
                }

                if (enoughMemoryForStage == GT_TRUE )
                {
                    /* restore  global pointers that was allocated during cpss init */
                    genTrunkDbPtr = PRV_CPSS_DEV_TRUNK_INFO_MAC(devNum);
                    if(currentStage == PRV_CPSS_DXCH_HSU_GEN_PP_CONFIG_STAGE_E)
                    {
                        /* gen pp config stage is finished */
                        /* some pointers should be set back to cpssIntSystem values */
                        genTrunkDbPtr->trunksArray = currentIterPtr->trunksArrayPtr;
                        genTrunkDbPtr->virtualFunctionsPtr = currentIterPtr->virtualFunctionsPtr;

                        cpssOsMemCpy(&(PRV_CPSS_PP_MAC(devNum)->phyInfo),
                                     currentIterPtr->phyInfoPtr,
                                     sizeof(PRV_CPSS_PHY_INFO_STC));
                        cpssOsFree(currentIterPtr->phyInfoPtr);
                        genTrunkDbPtr->allMembersArray = currentIterPtr->allMembersArrayPtr;
                        genTrunkDbPtr->allMembersIsLocalArray = currentIterPtr->allMembersIsLocalArrayPtr;
                        PRV_CPSS_PP_MAC(devNum)->phyPortInfoArray = currentIterPtr->phyPortInfoArrayPtr;

                        for (port = 0; port < PRV_CPSS_PP_MAC(devNum)->numOfPorts; port++)
                        {
                            if(!CPSS_PORTS_BMP_IS_PORT_SET_MAC(&PRV_CPSS_PP_MAC(devNum)->existingPorts , port))
                            {
                                continue;
                            }
                            PRV_CPSS_PP_MAC(devNum)->phyPortInfoArray[port].portMacObjPtr = currentIterPtr->portMacObjPtr[port];
                        }
                        cpssOsFree(currentIterPtr->portMacObjPtr);

                        if (systemRecoveryInfo.systemRecoveryMode.continuousAuMessages == GT_FALSE)
                        {
                            /* restore AU descriptor list as it was after cpssInit */
                            cpssOsMemCpy(&(PRV_CPSS_PP_MAC(devNum)->intCtrl.auDescCtrl),
                                         &(currentIterPtr->allDescListPtr->auDescCtrl),
                                         sizeof(currentIterPtr->allDescListPtr->auDescCtrl));
                            /* restore secondary AU descriptor list as it was after cpssInit */
                            cpssOsMemCpy(&(PRV_CPSS_PP_MAC(devNum)->intCtrl.secondaryAuDescCtrl),
                                         &(currentIterPtr->allDescListPtr->secondaryAuDescCtrl),
                                         sizeof(currentIterPtr->allDescListPtr->secondaryAuDescCtrl));
                            /* restore  AU WA parameters as it was after cpssInit */
                            cpssOsMemCpy(&(PRV_CPSS_PP_MAC(devNum)->intCtrl.auqDeadLockWa),
                                         &(currentIterPtr->allDescListPtr->auqDeadLockWa),
                                         sizeof(currentIterPtr->allDescListPtr->auqDeadLockWa));
                            /* restore  active AUQ index as it was after cpssInit */
                            cpssOsMemCpy(&(PRV_CPSS_PP_MAC(devNum)->intCtrl.activeAuqIndex),
                                         &(currentIterPtr->allDescListPtr->activeAuqIndex),
                                         sizeof(currentIterPtr->allDescListPtr->activeAuqIndex));
                            /* restore AU1 descriptor list as it was after cpssInit */
                            cpssOsMemCpy(&(PRV_CPSS_PP_MAC(devNum)->intCtrl.au1DescCtrl),
                                         &(currentIterPtr->allDescListPtr->au1DescCtrl),
                                         sizeof(currentIterPtr->allDescListPtr->au1DescCtrl));
                               /* restore secondary AU1 descriptor list as it was after cpssInit */
                            cpssOsMemCpy(&(PRV_CPSS_PP_MAC(devNum)->intCtrl.secondaryAu1DescCtrl),
                                         &(currentIterPtr->allDescListPtr->secondaryAu1DescCtrl),
                                         sizeof(currentIterPtr->allDescListPtr->secondaryAu1DescCtrl));

                        }

                        if (systemRecoveryInfo.systemRecoveryMode.continuousFuMessages == GT_FALSE)
                        {
                            /* restore FU descriptor list as it was after cpssInit */
                            cpssOsMemCpy(&(PRV_CPSS_PP_MAC(devNum)->intCtrl.fuDescCtrl),
                                         &(currentIterPtr->allDescListPtr->fuDescCtrl),
                                         sizeof(currentIterPtr->allDescListPtr->fuDescCtrl));
                        }

                        if (systemRecoveryInfo.systemRecoveryMode.continuousRx == GT_FALSE)
                        {
                            /* restore RX descriptor list as it was after cpssInit */
                            cpssOsMemCpy(&(PRV_CPSS_PP_MAC(devNum)->intCtrl.rxDescList),
                                         &(currentIterPtr->allDescListPtr->rxDescList),
                                         sizeof(currentIterPtr->allDescListPtr->rxDescList));
                        }

                        if (systemRecoveryInfo.systemRecoveryMode.continuousTx == GT_FALSE)
                        {
                            /* restore TX descriptor list as it was after cpssInit */
                            cpssOsMemCpy(&(PRV_CPSS_PP_MAC(devNum)->intCtrl.txDescList),
                                         &(currentIterPtr->allDescListPtr->txDescList),
                                         sizeof(currentIterPtr->allDescListPtr->txDescList));

                            PRV_CPSS_PP_MAC(devNum)->intCtrl.txGeneratorPacketIdDb.cookie =currentIterPtr->txGeneratorPacketIdDb.cookie;
                            PRV_CPSS_PP_MAC(devNum)->intCtrl.txGeneratorPacketIdDb.freeLinkedList = currentIterPtr->txGeneratorPacketIdDb.freeLinkedList;
                            PRV_CPSS_PP_MAC(devNum)->intCtrl.txGeneratorPacketIdDb.generatorsTotalDesc =currentIterPtr->txGeneratorPacketIdDb.generatorsTotalDesc;
                            PRV_CPSS_PP_MAC(devNum)->intCtrl.txGeneratorPacketIdDb.firstFree = currentIterPtr->txGeneratorPacketIdDb.firstFree;
                        }
                        if ((systemRecoveryInfo.systemRecoveryMode.continuousRx == GT_FALSE)         ||
                             (systemRecoveryInfo.systemRecoveryMode.continuousAuMessages == GT_FALSE)||
                             (systemRecoveryInfo.systemRecoveryMode.continuousTx == GT_FALSE)        ||
                             (systemRecoveryInfo.systemRecoveryMode.continuousFuMessages == GT_FALSE))
                        {
                            cpssOsFree(currentIterPtr->allDescListPtr);
                        }
                        if ((systemRecoveryInfo.systemRecoveryMode.continuousRx == GT_TRUE) ||
                            (systemRecoveryInfo.systemRecoveryMode.continuousTx == GT_TRUE))
                        {
                            /* Rx or TX or both descriptor lists data are imported. Make necessary
                            preparations before RX/TX Sw descriptors import */
                            rc = prvCpssDxChHsuRxTxImportPreparation(devNum);
                            if (rc != GT_OK)
                            {
                                return rc;
                            }
                            /* update addresses for next stages */
                            rc = prvCpssDxChHsuInitStageArray(devNum,
                                                              PRV_CPSS_DXCH_HSU_STAGE_ARRAY_DATA_TYPE_RXTX_E,
                                                              stageInfoArray);
                            if (rc != GT_OK)
                            {
                                return rc;
                            }
                        }
                    }
                    if (currentStage == PRV_CPSS_DXCH_HSU_GEN_TRUNK_ALL_MEMBERS_IS_LOCAL_ARRAY_STAGE_E)
                    {
                        /* last stage of trunk import was done*/
                        /* add maping for internal trunk array ptrs */
                        /* loop on all trunks and "attach" each trunk with it's members */
                        for(i = 0 ; i < genTrunkDbPtr->numberOfTrunks ; i++)
                        {
                            genTrunkDbPtr->trunksArray[i + 1].membersPtr =
                                    &(genTrunkDbPtr->allMembersArray[i * genTrunkDbPtr->numMembersInTrunk]);
                            genTrunkDbPtr->trunksArray[i + 1].isLocalMembersPtr =
                                &(genTrunkDbPtr->allMembersIsLocalArray[i * genTrunkDbPtr->numMembersInTrunk]);
                        }
                    }

                    if (currentStage == PRV_CPSS_DXCH_HSU_GEN_TX_QUEUE7_STAGE_E)
                    {
                        /* last stage of RXTX import was complete*/
                        if ((systemRecoveryInfo.systemRecoveryMode.continuousRx == GT_TRUE)||
                            (systemRecoveryInfo.systemRecoveryMode.continuousTx == GT_TRUE))
                        {
                            /* Restore sw rx/tx descriptors chain */
                            rc = prvCpssDxChHsuRestoreRxTxSwDescriptorsChain(devNum);
                            if (rc != GT_OK)
                            {
                                return rc;
                            }
                        }
                    }
                }
                else
                {
                    currentIterPtr->devNum = devNum;
                    return rc;
                }
            }
        }
    }
    /* all data for current device was processed */
    currentIterPtr->devNum =PRV_CPSS_MAX_PP_DEVICES_CNS;
    currentIterPtr->currStage = PRV_CPSS_DXCH_HSU_GEN_PP_CONFIG_STAGE_E;

    if (currentIterPtr->accumulatedGlobalSize >= currentIterPtr->hsuBlockGlobalSize)
    {
        *importCompletePtr = GT_TRUE;
        rc = GT_OK;
    }
    else
    {
        rc = GT_FAIL;
    }
    /* all stages were done -- free iterator */
    cpssOsFree(currentIterPtr);
    *iteratorPtr = 0;

    return  rc;
}

/*******************************************************************************
* prvCpssDxChHsuRestoreRxTxSwDescriptorsChain
*
* DESCRIPTION:
*       This function restores RxTx Sw descriptors chain.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum                - device number.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*
* COMMENTS:
*       none.
*
*******************************************************************************/
GT_STATUS prvCpssDxChHsuRestoreRxTxSwDescriptorsChain
(
    IN     GT_U8                         devNum
)
{
    GT_U32 queueNumber;
    GT_U32 i;
    PRV_CPSS_TX_DESC_LIST_STC *txDescList = NULL;
    PRV_CPSS_SW_TX_DESC_STC *firstSwTxDesc = NULL;
    PRV_CPSS_RX_DESC_LIST_STC *rxDescList = NULL;
    PRV_CPSS_SW_RX_DESC_STC *firstSwRxDesc = NULL;

    GT_U32                               portGroupId;/* port group Id for multi-port-groups device support */
    CPSS_MULTI_NET_IF_TX_SDMA_QUEUE_STC  sdmaQueuesConfig;

    if (systemRecoveryInfo.systemRecoveryMode.continuousTx == GT_TRUE)
    {
        if(PRV_CPSS_SIP_5_CHECK_MAC(devNum))
        {
            portGroupId = PRV_CPSS_NETIF_SDMA_PORT_GROUP_ID_MAC(devNum);
        }
        else
        {
            portGroupId = 0;
        }
        /* restore Tx sw descriptor chain */
        for(queueNumber = 0; queueNumber < NUM_OF_TX_QUEUES; queueNumber++)
        {
            sdmaQueuesConfig = PRV_CPSS_DXCH_PP_MAC(devNum)->moduleCfg.multiNetIfCfg.txSdmaQueuesConfig[portGroupId][queueNumber];

            if((PRV_CPSS_SIP_5_CHECK_MAC(devNum))&&
                   (PRV_CPSS_DXCH_PP_MAC(devNum)->moduleCfg.useMultiNetIfSdma==GT_TRUE)&&
                   (sdmaQueuesConfig.queueMode == CPSS_TX_SDMA_QUEUE_MODE_PACKET_GENERATOR_E))
            {
                /* nothing to do - no SW descriptors pointers to update */
            }
            else
            {
                txDescList  = &PRV_CPSS_PP_MAC(devNum)->intCtrl.txDescList[queueNumber];
                firstSwTxDesc = txDescList->swTxDescBlock;
                for (i = 0; i < txDescList->maxDescNum; i++)
                {
                    if (i == txDescList->maxDescNum -1)
                    {
                        /* it is cyclic :last should point to first */
                        firstSwTxDesc[i].swNextDesc = firstSwTxDesc;
                    }
                    else
                    {
                        firstSwTxDesc[i].swNextDesc = &(firstSwTxDesc[i+1]);
                    }
                    if ((PRV_CPSS_TX_DESC_STC *)txDescList->next2Free == firstSwTxDesc[i].txDesc)
                    {
                        txDescList->next2Free = &(firstSwTxDesc[i]);
                    }
                    if ((PRV_CPSS_TX_DESC_STC *)txDescList->next2Feed == firstSwTxDesc[i].txDesc)
                    {
                        txDescList->next2Feed = &(firstSwTxDesc[i]);
                    }
                }
            }
        }
    }

    if (systemRecoveryInfo.systemRecoveryMode.continuousRx == GT_TRUE)
    {
        /* restore Rx sw descriptor chain */
        for(queueNumber = 0; queueNumber < NUM_OF_RX_QUEUES; queueNumber++)
        {
            rxDescList  = &PRV_CPSS_PP_MAC(devNum)->intCtrl.rxDescList[queueNumber];
            firstSwRxDesc = rxDescList->swRxDescBlock;
            for (i = 0; i < rxDescList->maxDescNum; i++)
            {
                if (i == rxDescList->maxDescNum -1)
                {
                    /* it is cyclic :last should point to first */
                    firstSwRxDesc[i].swNextDesc = firstSwRxDesc;
                }
                else
                {
                    firstSwRxDesc[i].swNextDesc = &(firstSwRxDesc[i+1]);
                }
                if ((PRV_CPSS_RX_DESC_STC *)rxDescList->next2Return == firstSwRxDesc[i].rxDesc)
                {
                    rxDescList->next2Return = &(firstSwRxDesc[i]);
                }
                if ((PRV_CPSS_RX_DESC_STC *)rxDescList->next2Receive == firstSwRxDesc[i].rxDesc)
                {
                    rxDescList->next2Receive = &(firstSwRxDesc[i]);
                }
            }
        }
    }

    return GT_OK;

}
/*******************************************************************************
* prvCpssDxChHsuRxTxImportPreparation
*
* DESCRIPTION:
*       This function prepares for import stage.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum                 - device number
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_OUT_OF_CPU_MEM        - on out of CPU memory
*       GT_NO_RESOURCE           - on NO_RESOURCE under cpssBmPoolCreate
*
* COMMENTS:
*       none.
*
*******************************************************************************/
GT_STATUS prvCpssDxChHsuRxTxImportPreparation
(
    IN   GT_U8                                   devNum
)
{
    PRV_CPSS_TX_DESC_LIST_STC *txDescListPtr = NULL;
    PRV_CPSS_RX_DESC_LIST_STC *rxDescListPtr = NULL;

    GT_U32                               portGroupId;/* port group Id for multi-port-groups device support */
    CPSS_MULTI_NET_IF_TX_SDMA_QUEUE_STC  sdmaQueuesConfig;

    GT_STATUS rc = GT_OK;
    GT_U32 queueNumber;
    GT_U32 generatorsTotalDesc; /* total number of descriptors\buffers */
    if (systemRecoveryInfo.systemRecoveryMode.continuousTx == GT_TRUE)
    {
        if(PRV_CPSS_SIP_5_CHECK_MAC(devNum))
        {
            generatorsTotalDesc = PRV_CPSS_PP_MAC(devNum)->intCtrl.txGeneratorPacketIdDb.generatorsTotalDesc;
            if(generatorsTotalDesc!=0)
            {
                /* allocate packet ID cookies memory */
                PRV_CPSS_PP_MAC(devNum)->intCtrl.txGeneratorPacketIdDb.cookie =
                        (GT_U32*)cpssOsMalloc(sizeof(GT_U32)*generatorsTotalDesc);
                if( NULL == PRV_CPSS_PP_MAC(devNum)->intCtrl.txGeneratorPacketIdDb.cookie)
                {
                    CPSS_LOG_ERROR_AND_RETURN_MAC(GT_OUT_OF_CPU_MEM, LOG_ERROR_NO_MSG);
                }

                /* fill packet ID cookies memory */
                cpssOsMemSet(PRV_CPSS_PP_MAC(devNum)->intCtrl.txGeneratorPacketIdDb.cookie,
                             0xFF,
                             sizeof(GT_U32)*generatorsTotalDesc);

                /* allocate free packet ID linked list */
                PRV_CPSS_PP_MAC(devNum)->intCtrl.txGeneratorPacketIdDb.freeLinkedList =
                        (GT_U32*)cpssOsMalloc(sizeof(GT_U32)*generatorsTotalDesc);
                if( NULL == PRV_CPSS_PP_MAC(devNum)->intCtrl.txGeneratorPacketIdDb.freeLinkedList)
                {
                    CPSS_LOG_ERROR_AND_RETURN_MAC(GT_OUT_OF_CPU_MEM, LOG_ERROR_NO_MSG);
                }
                /* reset */
                cpssOsMemSet(PRV_CPSS_PP_MAC(devNum)->intCtrl.txGeneratorPacketIdDb.freeLinkedList,
                             0,
                             sizeof(GT_U32)*generatorsTotalDesc);
            }

            portGroupId = PRV_CPSS_NETIF_SDMA_PORT_GROUP_ID_MAC(devNum);
        }
        else
        {
            portGroupId = 0;
        }
        /* prepare tx pointers */
        for(queueNumber = 0; queueNumber < NUM_OF_TX_QUEUES; queueNumber++)
        {
            sdmaQueuesConfig = PRV_CPSS_DXCH_PP_MAC(devNum)->moduleCfg.multiNetIfCfg.txSdmaQueuesConfig[portGroupId][queueNumber];

            if((PRV_CPSS_SIP_5_CHECK_MAC(devNum))&&
                   (PRV_CPSS_DXCH_PP_MAC(devNum)->moduleCfg.useMultiNetIfSdma==GT_TRUE)&&
                   (sdmaQueuesConfig.queueMode == CPSS_TX_SDMA_QUEUE_MODE_PACKET_GENERATOR_E))
            {
                /* firstDescPtr, freeDescPtr and scratchPadPtr are saved in their relevant stages.
                   revPacketIdDb is allocated in sdmaTxGeneratorInit. When continuousTx == GT_TRUE
                   the this init is not done incase of system recovery. */

                txDescListPtr  = &PRV_CPSS_PP_MAC(devNum)->intCtrl.txDescList[queueNumber];
                txDescListPtr->revPacketIdDb =
                   (GT_U32*)cpssOsMalloc(sizeof(GT_U32)*txDescListPtr->maxDescNum);
                if( NULL == txDescListPtr->revPacketIdDb)
                {
                    CPSS_LOG_ERROR_AND_RETURN_MAC(GT_OUT_OF_CPU_MEM, LOG_ERROR_NO_MSG);
                }
                cpssOsMemSet(txDescListPtr->revPacketIdDb,0,sizeof(GT_U32) * txDescListPtr->maxDescNum);

            }
            else
            {
                txDescListPtr  = &PRV_CPSS_PP_MAC(devNum)->intCtrl.txDescList[queueNumber];
                txDescListPtr->swTxDescBlock = (PRV_CPSS_SW_TX_DESC_STC *)
                             cpssOsMalloc(sizeof(PRV_CPSS_SW_TX_DESC_STC ) * txDescListPtr->maxDescNum);
                if(txDescListPtr->swTxDescBlock == NULL)
                {
                    CPSS_LOG_ERROR_AND_RETURN_MAC(GT_OUT_OF_CPU_MEM, LOG_ERROR_NO_MSG);
                }
                cpssOsMemSet(txDescListPtr->swTxDescBlock,0,sizeof(PRV_CPSS_SW_TX_DESC_STC ) * txDescListPtr->maxDescNum);
                rc = cpssBmPoolCreate(sizeof(PRV_CPSS_SW_TX_FREE_DATA_STC),
                                      CPSS_BM_POOL_4_BYTE_ALIGNMENT_E,
                                      txDescListPtr->maxDescNum,
                                      &txDescListPtr->poolId);
                if (rc != GT_OK)
                {
                    return rc;
                }
            }
        }
    }
    /* make some preparations for rx pointers*/
    if (systemRecoveryInfo.systemRecoveryMode.continuousRx == GT_TRUE)
    {
        for(queueNumber = 0; queueNumber < NUM_OF_RX_QUEUES; queueNumber++)
        {
            rxDescListPtr  = &PRV_CPSS_PP_MAC(devNum)->intCtrl.rxDescList[queueNumber];
            rxDescListPtr->swRxDescBlock = (PRV_CPSS_SW_RX_DESC_STC*)
                         cpssOsMalloc(sizeof(PRV_CPSS_SW_RX_DESC_STC)*
                                        rxDescListPtr->maxDescNum);
            if(rxDescListPtr->swRxDescBlock == NULL)
            {
                CPSS_LOG_ERROR_AND_RETURN_MAC(GT_OUT_OF_CPU_MEM, LOG_ERROR_NO_MSG);
            }
            cpssOsMemSet(rxDescListPtr->swRxDescBlock,0,sizeof(PRV_CPSS_SW_RX_DESC_STC)*rxDescListPtr->maxDescNum);
        }
    }
   return rc;
}

/*******************************************************************************
* prvCpssDxChHsuSerdesImportPreparation
*
* DESCRIPTION:
*       This function prepares for serdes import stage.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum                 - device number
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_OUT_OF_CPU_MEM        - on out of CPU memory
*       GT_NO_RESOURCE           - on NO_RESOURCE under cpssBmPoolCreate
*
* COMMENTS:
*       none.
*
*******************************************************************************/
GT_STATUS prvCpssDxChHsuSerdesImportPreparation
(
    IN   GT_U8                                   devNum
)
{
    GT_U32      stageIndex;
    GT_U32      maxSerdesLanesNum;/* max number of lanes that can be allocated */
    GT_U32      laneNum=0;

    if(PRV_CPSS_SIP_5_CHECK_MAC(devNum))
    {
        if((serdesLanesBmp[0] != 0)||(serdesLanesBmp[1] != 0))
        {
            /* in the export stage the serdesLanesBmp!=0 this means
              PRV_CPSS_DXCH_PP_MAC(devNum)->serdesCfgDbArrPtr !=NULL prior to system reset
              so we need to allocate memory */
            maxSerdesLanesNum = PRV_CPSS_BOBCAT2_SERDES_NUM_CNS;

            PRV_CPSS_DXCH_PP_MAC(devNum)->serdesCfgDbArrPtr = (CPSS_DXCH_PORT_SERDES_TUNE_STC_PTR*)
                cpssOsMalloc(sizeof(CPSS_DXCH_PORT_SERDES_TUNE_STC_PTR)*maxSerdesLanesNum);

            if(NULL == PRV_CPSS_DXCH_PP_MAC(devNum)->serdesCfgDbArrPtr)
            {
                CPSS_LOG_ERROR_AND_RETURN_MAC(GT_OUT_OF_CPU_MEM, LOG_ERROR_NO_MSG);
            }
            cpssOsMemSet(PRV_CPSS_DXCH_PP_MAC(devNum)->serdesCfgDbArrPtr, 0,
                         sizeof(CPSS_DXCH_PORT_SERDES_TUNE_STC_PTR)*maxSerdesLanesNum);

            /* init all lanes stages*/
            for (stageIndex = PRV_CPSS_DXCH_HSU_SERDES_LANE0_STAGE_E;
                 stageIndex <= PRV_CPSS_DXCH_HSU_SERDES_LANE35_STAGE_E; stageIndex++)
            {
                laneNum = (stageIndex-PRV_CPSS_DXCH_HSU_SERDES_LANE0_STAGE_E);

                if((serdesLanesBmp[laneNum/32] & (1<<(laneNum%32))) != 0)
                {
                    PRV_CPSS_DXCH_PP_MAC(devNum)->serdesCfgDbArrPtr[laneNum] =
                        (CPSS_DXCH_PORT_SERDES_TUNE_STC_PTR)cpssOsMalloc(
                            sizeof(CPSS_DXCH_PORT_SERDES_TUNE_STC)*CPSS_DXCH_PORT_SERDES_SPEED_NA_E);
                    if(NULL == PRV_CPSS_DXCH_PP_MAC(devNum)->serdesCfgDbArrPtr[laneNum])
                    {
                        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_OUT_OF_CPU_MEM, LOG_ERROR_NO_MSG);
                    }
                    cpssOsMemSet(PRV_CPSS_DXCH_PP_MAC(devNum)->serdesCfgDbArrPtr[laneNum], 0,
                                sizeof(CPSS_DXCH_PORT_SERDES_TUNE_STC)*CPSS_DXCH_PORT_SERDES_SPEED_NA_E);
                }
            }
        }
    }
    else
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_SUPPORTED, LOG_ERROR_NO_MSG);
    }

    return GT_OK;
}

/*******************************************************************************
* prvCpssDxChHsuConvertStageInQueueIndex
*
* DESCRIPTION:
*       This function converts stage enum to tx queue index.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       stageIndex             - import/export tx stage
*
* OUTPUTS:
*       queueIndexPtr          - pointer to tx queue index
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on bad hsu data type
*       GT_BAD_PTR               - on NULL pointer
*
* COMMENTS:
*       none.
*
*******************************************************************************/
GT_STATUS prvCpssDxChHsuConvertStageInQueueIndex
(
    IN   PRV_CPSS_DXCH_HSU_GLOBAL_DATA_STAGE_ENT stageIndex,
    OUT  GT_U32                                  *queueIndexPtr
)
{
    CPSS_NULL_PTR_CHECK_MAC(queueIndexPtr);
    switch(stageIndex)
    {
    case PRV_CPSS_DXCH_HSU_GEN_RX_QUEUE0_STAGE_E:
    case PRV_CPSS_DXCH_HSU_GEN_TX_QUEUE0_STAGE_E:
        *queueIndexPtr = 0;
        break;
    case PRV_CPSS_DXCH_HSU_GEN_RX_QUEUE1_STAGE_E:
    case PRV_CPSS_DXCH_HSU_GEN_TX_QUEUE1_STAGE_E:
        *queueIndexPtr = 1;
        break;
    case PRV_CPSS_DXCH_HSU_GEN_RX_QUEUE2_STAGE_E:
    case PRV_CPSS_DXCH_HSU_GEN_TX_QUEUE2_STAGE_E:
        *queueIndexPtr = 2;
        break;
    case PRV_CPSS_DXCH_HSU_GEN_RX_QUEUE3_STAGE_E:
    case PRV_CPSS_DXCH_HSU_GEN_TX_QUEUE3_STAGE_E:
        *queueIndexPtr = 3;
        break;
    case PRV_CPSS_DXCH_HSU_GEN_RX_QUEUE4_STAGE_E:
    case PRV_CPSS_DXCH_HSU_GEN_TX_QUEUE4_STAGE_E:
        *queueIndexPtr = 4;
        break;
    case PRV_CPSS_DXCH_HSU_GEN_RX_QUEUE5_STAGE_E:
    case PRV_CPSS_DXCH_HSU_GEN_TX_QUEUE5_STAGE_E:
        *queueIndexPtr = 5;
        break;
    case PRV_CPSS_DXCH_HSU_GEN_RX_QUEUE6_STAGE_E:
    case PRV_CPSS_DXCH_HSU_GEN_TX_QUEUE6_STAGE_E:
        *queueIndexPtr = 6;
        break;
    case PRV_CPSS_DXCH_HSU_GEN_RX_QUEUE7_STAGE_E:
    case PRV_CPSS_DXCH_HSU_GEN_TX_QUEUE7_STAGE_E:
        *queueIndexPtr = 7;
        break;
    default:
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }
    return GT_OK;
}




/*******************************************************************************
* prvCpssDxChSystemRecoveryDisableModeHandle
*
* DESCRIPTION:
*       Handling au and fu queues,Rx SDMA to provide smooth reprogramming in new immage.
*       It is applicable to system recovery mode when application is not interested
*       in getting Rx/Au/Fu messages during system recovery process.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       None.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvCpssDxChSystemRecoveryDisableModeHandle
(
    GT_VOID
)
{
    GT_STATUS                        rc;
    CPSS_MAC_ENTRY_EXT_KEY_STC       macEntry;
    CPSS_MAC_ENTRY_EXT_STC           fdbEntry;
    GT_U16                           currentVid = 0;
    GT_U16                           currentVidMask = 0;
    CPSS_FDB_ACTION_MODE_ENT         actionMode = CPSS_FDB_ACTION_AGE_WITHOUT_REMOVAL_E;
    CPSS_MAC_ACTION_MODE_ENT         triggerMode = CPSS_ACT_AUTO_E;
    GT_BOOL                          actionEnable = GT_FALSE;
    GT_U32                           fdbNumOfEntries;
    GT_BOOL                          valid;
    GT_U32                           index = 0;
    GT_BOOL                          skip = GT_FALSE;
    GT_BOOL                          *naToCpuPerPortPtr = NULL;
    GT_BOOL                          naToCpuLearnFail = GT_FALSE;
    GT_BOOL                          naTag1VLANassignment = GT_FALSE;
    GT_BOOL                          *naStormPreventPortPtr = NULL;
    GT_BOOL                          naToCpuAaTa = GT_FALSE;
    GT_BOOL                          spAaMsgToCpu = GT_FALSE;
    GT_U8                            devNum       = 0;
    GT_U32                           numOfMsgUntilQueueEnd = 0;
    GT_U32                           numOfProcessedAuFuMsg;
    GT_PORT_GROUPS_BMP               portGroupsBmp;
    GT_PORT_GROUPS_BMP               completedPortGroupsBmp;
    GT_PORT_GROUPS_BMP               succeededPortGroupsBmp;
    GT_BOOL                          fdbUploadState = GT_FALSE;
    GT_U32                           actDev = 0;
    GT_U32                           actDevMask = 0;
    GT_U32                           actIsTrunk = 0;
    GT_U32                           actIsTrunkMask = 0;
    GT_U32                           actTrunkPort = 0;
    GT_U32                           actTrunkPortMask = 0;
    GT_BOOL                          actFinished;
    GT_BOOL                          maskAuFuMsg2CpuOnNonLocal = GT_FALSE;
    GT_U32                           i;
    GT_U32                           portGroupId,portGroupIndex;
    GT_U32                           qa_counter = 0;
    GT_U32                           trigCounter;
    GT_U32                           numberOfFdbEntriesToAdd;
    GT_BOOL                          *deleteEntryPtr = NULL;
    GT_U32                           numOfAuQueuesToReprogram;
    GT_U32                           regAddr = 0;
    GT_UINTPTR                       phyAddr = 0;


    fdbNumOfEntries = PRV_CPSS_DXCH_PP_MAC(devNum)->fineTuning.tableSize.fdb;

    for (devNum = 0; devNum < PRV_CPSS_MAX_PP_DEVICES_CNS; devNum++)
    {
        if ( (prvCpssPpConfig[devNum] == NULL) ||
             (PRV_CPSS_DXCH_FAMILY_CHECK_MAC(devNum) == 0) )
        {
            continue;
        }
        if (systemRecoveryInfo.systemRecoveryMode.continuousRx == GT_FALSE)
        {
            /* handle RX SDMA for hsu message disable mode */
            rc =  cpssDxChNetIfRemove(devNum);
            if (rc != GT_OK)
            {
                return rc;
            }
        }

        if((PRV_CPSS_HW_IF_PCI_COMPATIBLE_MAC(devNum)) &&
         (PRV_CPSS_DXCH_PP_MAC(devNum)->errata.info_PRV_CPSS_DXCH_XCAT_FDB_AU_FIFO_CORRUPT_WA_E.
                         enabled == GT_FALSE))
        {
            if (systemRecoveryInfo.systemRecoveryMode.continuousAuMessages == GT_FALSE)
            {
                /* handle AU/FU queue*/
                naToCpuPerPortPtr = (GT_BOOL*)cpssOsMalloc(sizeof(GT_BOOL)* PRV_CPSS_PP_MAC(devNum)->numOfPorts);
                if (naToCpuPerPortPtr == NULL)
                {
                    CPSS_LOG_ERROR_AND_RETURN_MAC(GT_OUT_OF_CPU_MEM, LOG_ERROR_NO_MSG);
                }
                naStormPreventPortPtr = (GT_BOOL*)cpssOsMalloc(sizeof(GT_BOOL)* PRV_CPSS_PP_MAC(devNum)->numOfPorts);
                if (naStormPreventPortPtr == NULL)
                {
                    cpssOsFree(naToCpuPerPortPtr);
                    CPSS_LOG_ERROR_AND_RETURN_MAC(GT_OUT_OF_CPU_MEM, LOG_ERROR_NO_MSG);
                }


                cpssOsMemSet(naToCpuPerPortPtr,0,sizeof(GT_BOOL)* PRV_CPSS_PP_MAC(devNum)->numOfPorts);
                cpssOsMemSet(naStormPreventPortPtr,0,sizeof(GT_BOOL)* PRV_CPSS_PP_MAC(devNum)->numOfPorts);
                cpssOsMemSet(&macEntry,0,sizeof(CPSS_MAC_ENTRY_EXT_KEY_STC));

                macEntry.entryType                          = CPSS_MAC_ENTRY_EXT_TYPE_MAC_ADDR_E;
                macEntry.key.macVlan.vlanId             = 0;
                macEntry.key.macVlan.macAddr.arEther[0] = 0x0;
                macEntry.key.macVlan.macAddr.arEther[1] = 0x1A;
                macEntry.key.macVlan.macAddr.arEther[2] = 0xFF;
                macEntry.key.macVlan.macAddr.arEther[3] = 0xFF;
                macEntry.key.macVlan.macAddr.arEther[4] = 0xFF;
                macEntry.key.macVlan.macAddr.arEther[5] = 0xFF;

                /*Disable sending NA update messages to the CPU per port*/

                for(i=0; i < PRV_CPSS_PP_MAC(devNum)->numOfPorts; i++)
                {
                    /* skip not existed ports */
                    if (! PRV_CPSS_PHY_PORT_IS_EXIST_MAC(devNum, i))
                        continue;

                    /* at first save staus per port*/
                    rc = cpssDxChBrgFdbNaToCpuPerPortGet(devNum,(GT_U8)i,&naToCpuPerPortPtr[i]);
                    if (rc != GT_OK)
                    {
                        cpssOsFree(naToCpuPerPortPtr);
                        cpssOsFree(naStormPreventPortPtr);
                        return rc;
                    }
                    rc =  cpssDxChBrgFdbNaStormPreventGet(devNum, (GT_U8)i,&naStormPreventPortPtr[i]);
                    if (rc != GT_OK)
                    {
                        cpssOsFree(naToCpuPerPortPtr);
                        cpssOsFree(naStormPreventPortPtr);
                        return rc;
                    }


                    rc =  cpssDxChBrgFdbNaToCpuPerPortSet(devNum,(GT_U8)i,GT_FALSE);
                    if (rc != GT_OK)
                    {
                        cpssOsFree(naToCpuPerPortPtr);
                        cpssOsFree(naStormPreventPortPtr);
                        return rc;
                    }
                    rc =  cpssDxChBrgFdbNaStormPreventSet(devNum, (GT_U8)i,GT_FALSE);
                    if (rc != GT_OK)
                    {
                        cpssOsFree(naToCpuPerPortPtr);
                        cpssOsFree(naStormPreventPortPtr);
                        return rc;
                    }
                }
                /* save status (enabled/disabled) of sending NA messages to the CPU
                   indicating that the device cannot learn a new SA */
                rc = cpssDxChBrgFdbNaMsgOnChainTooLongGet(devNum,&naToCpuLearnFail);
                if (rc != GT_OK)
                {
                    cpssOsFree(naToCpuPerPortPtr);
                    cpssOsFree(naStormPreventPortPtr);
                    return rc;
                }
                /* disable sending NA messages to the CPU
                   indicating that the device cannot learn a new SA */
                rc = cpssDxChBrgFdbNaMsgOnChainTooLongSet(devNum,GT_FALSE);
                if (rc != GT_OK)
                {
                    cpssOsFree(naToCpuPerPortPtr);
                    cpssOsFree(naStormPreventPortPtr);
                    return rc;
                }
                /* Get the status of Tag1 VLAN Id assignment in vid1 field of the NA AU
                  message */
                if (PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_DXCH_XCAT2_E)
                {
                    rc = cpssDxChBrgFdbNaMsgVid1EnableGet(devNum,&naTag1VLANassignment);
                    if (rc != GT_OK)
                    {
                        cpssOsFree(naToCpuPerPortPtr);
                        cpssOsFree(naStormPreventPortPtr);
                        return rc;
                    }
                    /* Disable sending of Tag1 VLAN Id assignment in vid1 field of the NA AU
                      message */
                    rc = cpssDxChBrgFdbNaMsgVid1EnableSet(devNum,GT_FALSE);
                    if (rc != GT_OK)
                    {
                        cpssOsFree(naToCpuPerPortPtr);
                        cpssOsFree(naStormPreventPortPtr);
                        return rc;
                    }
                }

                /* save status of AA and AT messages */
                rc = cpssDxChBrgFdbAAandTAToCpuGet(devNum,&naToCpuAaTa);
                if (rc != GT_OK)
                {
                    cpssOsFree(naToCpuPerPortPtr);
                    cpssOsFree(naStormPreventPortPtr);
                    return rc;
                }

               /* Disable AA and AT messages */
                rc =  cpssDxChBrgFdbAAandTAToCpuSet(devNum,GT_FALSE);
                if (rc != GT_OK)
                {
                    cpssOsFree(naToCpuPerPortPtr);
                    cpssOsFree(naStormPreventPortPtr);
                    return rc;
                }

                /* save Sp AA message to CPU status*/
                rc =  cpssDxChBrgFdbSpAaMsgToCpuGet(devNum,&spAaMsgToCpu);
                if (rc != GT_OK)
                {
                    cpssOsFree(naToCpuPerPortPtr);
                    cpssOsFree(naStormPreventPortPtr);
                    return rc;
                }

                /* Disable sending AA messages to the CPU indicating that the
                device aged-out storm prevention FDB entry */
                rc =  cpssDxChBrgFdbSpAaMsgToCpuSet(devNum,GT_FALSE);
                if (rc != GT_OK)
                {
                    cpssOsFree(naToCpuPerPortPtr);
                    cpssOsFree(naStormPreventPortPtr);
                    return rc;
                }

                /* check all AU/FU queues on the device*/
                /* calculate how many messages remains till the end of the queue                */
                /* in order to do that move sw pointer to hw pointer                            */
                /* also fill the queue by the same number (as remained messages) of QA queries. */
                /* all workarounds connected with secondary AUQ should be finished before HSU   */
                if (PRV_CPSS_DXCH_PP_MAC(devNum)->moduleCfg.useDoubleAuq == GT_TRUE)
                {
                    numOfAuQueuesToReprogram = 2;
                }
                else
                {
                    numOfAuQueuesToReprogram = 1;
                }
                PRV_CPSS_GEN_PP_START_LOOP_PORT_GROUPS_MAC(devNum,portGroupId)
                {
                    portGroupsBmp = (1 << portGroupId);
                    for (i = 0; i < numOfAuQueuesToReprogram; i++)
                    {
                        if (i == 1)
                        {
                            PRV_CPSS_PP_MAC(devNum)->intCtrl.activeAuqIndex[portGroupId] ^= 1;
                        }
                        rc =  auqFuqClearUnreadMessages(devNum ,portGroupId,
                                MESSAGE_QUEUE_PRIMARY_AUQ_E,&numOfProcessedAuFuMsg,&numOfMsgUntilQueueEnd);
                        if(rc != GT_OK)
                        {
                            cpssOsFree(naToCpuPerPortPtr);
                            cpssOsFree(naStormPreventPortPtr);
                            return rc;
                        }
                        if (numOfMsgUntilQueueEnd == 0)
                        {
                            /* It means that queue is FULL. In this case FIFO also could be full */
                            /* New queue of 64 messages is defined                               */
                            /* define queue size */
                            PRV_CPSS_AU_DESC_CTRL_STC   *descCtrlPtr = NULL;

                            descCtrlPtr = (PRV_CPSS_AUQ_INDEX_MAC(devNum, portGroupId) == 0) ?
                                &(PRV_CPSS_PP_MAC(devNum)->intCtrl.auDescCtrl[portGroupId]) :
                                &(PRV_CPSS_PP_MAC(devNum)->intCtrl.au1DescCtrl[portGroupId]);
                            descCtrlPtr->blockSize = 64;
                            descCtrlPtr->currDescIdx = 0;
                            regAddr =PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->globalRegs.auQControl;
                            rc = prvCpssHwPpPortGroupSetRegField(devNum,portGroupId,regAddr,0,31,64);
                            if (rc != GT_OK)
                            {
                                cpssOsFree(naToCpuPerPortPtr);
                                cpssOsFree(naStormPreventPortPtr);
                                return rc;
                            }
                            regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->globalRegs.auQBaseAddr;
                            /* read physical AUQ address from PP */
                            rc =  prvCpssHwPpPortGroupReadRegister(devNum,portGroupId,regAddr,(GT_U32*)(&phyAddr));
                            if (rc != GT_OK)
                            {
                                cpssOsFree(naToCpuPerPortPtr);
                                cpssOsFree(naStormPreventPortPtr);
                                return rc;
                            }
                            /* define queue base address */
                            rc = prvCpssHwPpPortGroupWriteRegister(devNum,portGroupId,regAddr,(GT_U32)phyAddr);
                            if (rc != GT_OK)
                            {
                                cpssOsFree(naToCpuPerPortPtr);
                                cpssOsFree(naStormPreventPortPtr);
                                return rc;
                            }
                            cpssOsTimerWkAfter(1);

                            /* now check status of new created queue */
                            rc =  auqFuqClearUnreadMessages(devNum ,portGroupId,
                                    MESSAGE_QUEUE_PRIMARY_AUQ_E,&numOfProcessedAuFuMsg,&numOfMsgUntilQueueEnd);
                            if(rc != GT_OK)
                            {
                                cpssOsFree(naToCpuPerPortPtr);
                                cpssOsFree(naStormPreventPortPtr);
                                return rc;
                            }
                        }
                        for (index = 0; index < numOfMsgUntilQueueEnd; index++)
                        {
                            qa_counter = 0;
                            do
                            {
                                rc =  cpssDxChBrgFdbPortGroupQaSend( devNum, portGroupsBmp,&macEntry);
                                if(rc != GT_OK)
                                {
         #ifdef ASIC_SIMULATION
                                    cpssOsTimerWkAfter(1);
         #endif
                                    qa_counter++;
                                    if(qa_counter > 20)
                                    {
                                        cpssOsFree(naToCpuPerPortPtr);
                                        cpssOsFree(naStormPreventPortPtr);
                                        return rc;
                                    }
                                }
                            } while (rc != GT_OK);

                            /* verify that action is completed */
                            completedPortGroupsBmp = 0;
                            while ((completedPortGroupsBmp & portGroupsBmp)!= portGroupsBmp)
                            {
                                rc = cpssDxChBrgFdbPortGroupFromCpuAuMsgStatusGet(devNum,portGroupsBmp,&completedPortGroupsBmp,
                                                                                  &succeededPortGroupsBmp);
                                if(rc != GT_OK)
                                {
                                    cpssOsFree(naToCpuPerPortPtr);
                                    cpssOsFree(naStormPreventPortPtr);
                                    return rc;
                                }
                            }
                        }
                    }
                }
                PRV_CPSS_GEN_PP_END_LOOP_PORT_GROUPS_MAC(devNum,portGroupId)
            }

            /* FU handling */
            if (systemRecoveryInfo.systemRecoveryMode.continuousFuMessages == GT_FALSE)
            {

                if(PRV_CPSS_DXCH_PP_MAC(devNum)->moduleCfg.fuqUseSeparate == GT_TRUE)
                {
                    GT_U32 uploadNumber = 0;
                    deleteEntryPtr = (GT_BOOL *)cpssOsMalloc(sizeof(GT_BOOL) * fdbNumOfEntries);
                    if (deleteEntryPtr == NULL)
                    {
                        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_OUT_OF_CPU_MEM, LOG_ERROR_NO_MSG);
                    }
                    rc = cpssDxChBrgFdbUploadEnableGet(devNum,&fdbUploadState);
                    if(rc != GT_OK)
                    {
                        cpssOsFree(deleteEntryPtr);
                        return rc;
                    }

                    /* Enable/Disable reading FDB entries via AU messages to the CPU*/
                    rc =  cpssDxChBrgFdbUploadEnableSet(devNum, GT_TRUE);
                    if(rc != GT_OK)
                    {
                        cpssOsFree(deleteEntryPtr);
                        return rc;
                    }

                    /* configure FDB to upload only this specific entry*/

                    /* save vid and vid mask */
                    rc =  cpssDxChBrgFdbActionActiveVlanGet(devNum,&currentVid,&currentVidMask);
                    if (rc != GT_OK)
                    {
                        cpssOsFree(deleteEntryPtr);
                        return rc;
                    }

                    /* set current vid and vid mask */
                    rc =  cpssDxChBrgFdbActionActiveVlanSet(devNum,0,0xfff);
                    if (rc != GT_OK)
                    {
                        cpssOsFree(deleteEntryPtr);
                        return rc;
                    }
                    /* save current action dev */
                    rc =  cpssDxChBrgFdbActionActiveDevGet(devNum,&actDev,&actDevMask);
                    if (rc != GT_OK)
                    {
                        cpssOsFree(deleteEntryPtr);
                        return rc;
                    }
                    /* set new action device */
                    rc = cpssDxChBrgFdbActionActiveDevSet(devNum,30,0x1f);
                    if (rc != GT_OK)
                    {
                        cpssOsFree(deleteEntryPtr);
                        return rc;
                    }
                    /* save current action interface */
                    rc = cpssDxChBrgFdbActionActiveInterfaceGet(devNum,&actIsTrunk,&actIsTrunkMask,
                                                                &actTrunkPort,&actTrunkPortMask);

                    if (rc != GT_OK)
                    {
                        cpssOsFree(deleteEntryPtr);
                        return rc;
                    }
                    /* set new action interface */
                    rc =  cpssDxChBrgFdbActionActiveInterfaceSet(devNum,0,0,62,0x3f);
                    if (rc != GT_OK)
                    {
                        cpssOsFree(deleteEntryPtr);
                        return rc;
                    }
                    /* save action trigger mode */
                    rc = cpssDxChBrgFdbMacTriggerModeGet(devNum,&triggerMode);
                    if (rc != GT_OK)
                    {
                        cpssOsFree(deleteEntryPtr);
                        return rc;
                    }
                    /* set new action trigger mode */
                    rc = cpssDxChBrgFdbMacTriggerModeSet(devNum,CPSS_ACT_TRIG_E);
                    if (rc != GT_OK)
                    {
                        cpssOsFree(deleteEntryPtr);
                        return rc;
                    }

                    /* save action mode */
                    rc =  cpssDxChBrgFdbActionModeGet(devNum,&actionMode);
                    if (rc != GT_OK)
                    {
                        cpssOsFree(deleteEntryPtr);
                        return rc;
                    }

                    /* set fdb upload action mode */
                    rc = cpssDxChBrgFdbActionModeSet(devNum,CPSS_FDB_ACTION_AGE_WITHOUT_REMOVAL_E);
                    if (rc != GT_OK)
                    {
                        cpssOsFree(deleteEntryPtr);
                        return rc;
                    }
                    /* save action enable state*/
                    rc = cpssDxChBrgFdbActionsEnableGet(devNum,&actionEnable);
                    if (rc != GT_OK)
                    {
                        cpssOsFree(deleteEntryPtr);
                        return rc;
                    }
                    /* set fdb action state */
                    rc = cpssDxChBrgFdbActionsEnableSet(devNum,GT_TRUE);
                    if (rc != GT_OK)
                    {
                        cpssOsFree(deleteEntryPtr);
                        return rc;
                    }

                    if(CPSS_PP_FAMILY_DXCH_LION2_E == PRV_CPSS_PP_MAC(devNum)->devFamily)
                    {
                        /* save maskAuFuMsg2CpuOnNonLocal state*/
                        rc = prvCpssDxChBrgFdbAuFuMessageToCpuOnNonLocalMaskEnableGet(devNum,&maskAuFuMsg2CpuOnNonLocal);
                        if (rc != GT_OK)
                        {
                            return rc;
                        }
                        /* set maskAuFuMsg2CpuOnNonLocal state*/
                        rc = prvCpssDxChBrgFdbAuFuMessageToCpuOnNonLocalMaskEnableSet(devNum,GT_FALSE);
                    }

                    cpssOsMemSet(&fdbEntry,0,sizeof(CPSS_MAC_ENTRY_EXT_STC));

                    /* fill very specific fdb entry  */
                    fdbEntry.key.entryType = CPSS_MAC_ENTRY_EXT_TYPE_MAC_ADDR_E;
                    fdbEntry.key.key.macVlan.vlanId = 0;
                    fdbEntry.key.key.macVlan.macAddr.arEther[0] = 0;
                    fdbEntry.key.key.macVlan.macAddr.arEther[1] = 0x15;
                    fdbEntry.key.key.macVlan.macAddr.arEther[2] = 0x14;
                    fdbEntry.key.key.macVlan.macAddr.arEther[3] = 0x13;
                    fdbEntry.key.key.macVlan.macAddr.arEther[4] = 0x12;
                    fdbEntry.key.key.macVlan.macAddr.arEther[5] = 0x11;
                    fdbEntry.dstInterface.devPort.hwDevNum = 30;
                    fdbEntry.dstInterface.devPort.portNum = 62;
                    fdbEntry.dstInterface.type = CPSS_INTERFACE_PORT_E;

                    PRV_CPSS_GEN_PP_START_LOOP_PORT_GROUPS_MAC(devNum,portGroupId)
                    {
                        cpssOsMemSet(deleteEntryPtr,0,sizeof(GT_BOOL)* fdbNumOfEntries);
                        portGroupsBmp = (1 << portGroupId);

                        /* calculate number of messages till end of FUQ */
                        rc =  auqFuqClearUnreadMessages(devNum ,portGroupId,
                                MESSAGE_QUEUE_PRIMARY_FUQ_E,&numOfProcessedAuFuMsg,&numOfMsgUntilQueueEnd);
                        if(rc != GT_OK)
                        {
                            cpssOsFree(deleteEntryPtr);
                            return rc;
                        }


                        /* Calculate numberOfFdbEntriesToAdd */
                        if (PRV_CPSS_PP_MAC(devNum)->intCtrl.fuDescCtrl[portGroupId].blockSize > fdbNumOfEntries)
                        {
                            numberOfFdbEntriesToAdd = fdbNumOfEntries;
                        }
                        else
                        {
                            numberOfFdbEntriesToAdd = numOfMsgUntilQueueEnd;
                        }
                        uploadNumber = numOfMsgUntilQueueEnd/fdbNumOfEntries;
                        if ((numOfMsgUntilQueueEnd%fdbNumOfEntries) != 0)
                        {
                            uploadNumber++;
                        }
                        /* First scan of the FDB: find how many entries to add to the FDB */
                        for (index = 0; index < fdbNumOfEntries; index++)
                        {
                            /* call cpss api function */
                            rc = cpssDxChBrgFdbPortGroupMacEntryStatusGet(devNum, portGroupsBmp, index, &valid, &skip);
                            if (rc != GT_OK)
                            {
                                cpssOsFree(deleteEntryPtr);
                                return rc;
                            }
                            if ((valid == GT_TRUE) && (skip == GT_FALSE))
                            {
                                numberOfFdbEntriesToAdd--;
                            }
                            if (numberOfFdbEntriesToAdd == 0)
                            {
                                break;
                            }
                        }
                        /* Second scan of the FDB: add entries */
                        for (index = 0; index < fdbNumOfEntries; index++)
                        {
                            if (numberOfFdbEntriesToAdd == 0)
                            {
                                break;
                            }
                            rc = cpssDxChBrgFdbPortGroupMacEntryStatusGet(devNum,portGroupsBmp, index, &valid, &skip);
                            if (rc != GT_OK)
                            {
                                cpssOsFree(deleteEntryPtr);
                                return rc;
                            }
                            if ((valid == GT_FALSE) || (skip == GT_TRUE))
                            {
                                /* write the entry to the FDB */
                                rc = cpssDxChBrgFdbPortGroupMacEntryWrite(devNum, portGroupsBmp, index, GT_FALSE, &fdbEntry);
                                if (rc != GT_OK)
                                {
                                    cpssOsFree(deleteEntryPtr);
                                    return rc;
                                }
                                deleteEntryPtr[index] = GT_TRUE;
                                numberOfFdbEntriesToAdd--;
                            }
                        }

                        /* fill all FUQs on the device*/
                        for (i = 0; i < uploadNumber; i++)
                        {
                            /*  force the upload trigger */
                            rc =  cpssDxChBrgFdbMacTriggerToggle(devNum);
                            if(rc != GT_OK)
                            {
                                cpssOsFree(deleteEntryPtr);
                                return rc;
                            }
                            /* verify that action is completed */
                            actFinished = GT_FALSE;
                            trigCounter = 0;
                            while (actFinished == GT_FALSE)
                            {
                                rc = cpssDxChBrgFdbTrigActionStatusGet(devNum,&actFinished);
                                if(rc != GT_OK)
                                {
                                    cpssOsFree(deleteEntryPtr);
                                    return rc;
                                }
                                if (actFinished == GT_FALSE)
                                {
                                    trigCounter++;
                                }
                                else
                                {
                                    trigCounter = 0;
                                    break;
                                }
                                if (trigCounter > 500)
                                {
                                    PRV_CPSS_GEN_PP_START_LOOP_PORT_GROUPS_MAC(devNum,portGroupIndex)
                                    {
                                        if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_MAC(devNum) == GT_TRUE)
                                        {
                                            regAddr = PRV_DXCH_REG1_UNIT_FDB_MAC(devNum).FDBCore.FDBAction.FDBActionGeneral;
                                        }
                                        else
                                        {
                                            regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->bridgeRegs.macTblAction0;
                                        }
                                        rc =  prvCpssHwPpPortGroupSetRegField(devNum,portGroupIndex,
                                                                                 regAddr,
                                                                                 1, 1, 0);
                                        if (rc != GT_OK)
                                        {
                                            cpssOsFree(deleteEntryPtr);
                                            return rc;
                                        }

                                        /* check if need to break loop on device that supports single FDB unit instance */
                                        PRV_CPSS_DXCH_FDB_SINGLE_INSTANCE_FDB_BREAK_MAC(devNum);
                                    }
                                    PRV_CPSS_GEN_PP_END_LOOP_PORT_GROUPS_MAC(devNum,portGroupIndex)
                                }
                            }
                        }
                        /* check if FUQ is full */
                        completedPortGroupsBmp = 0;
                        trigCounter = 0;
                        while (1)
                        {
                            rc = cpssDxChBrgFdbPortGroupQueueFullGet(devNum,portGroupsBmp,CPSS_DXCH_FDB_QUEUE_TYPE_FU_E,&completedPortGroupsBmp);
                            if (rc != GT_OK)
                            {
                                cpssOsFree(deleteEntryPtr);
                                return rc;
                            }
                            if((completedPortGroupsBmp & portGroupsBmp)== portGroupsBmp)
                            {
                                break;
                            }
                            else
                            {
                                if (trigCounter > 6)
                                {
                                    /* can't fill queue*/
                                    CPSS_LOG_ERROR_AND_RETURN_MAC(GT_FAIL, LOG_ERROR_NO_MSG);
                                }
                                cpssOsTimerWkAfter(1);
                                trigCounter++;
                            }

                        }

                        /* now ALL queus are full . Restore configuration */
                        /* restore FDB configuration */
                        for (index = 0; index < fdbNumOfEntries; index++)
                        {
                            if (deleteEntryPtr[index] == GT_TRUE)
                            {
                                /* delete the entry */
                                portGroupsBmp = (1 << portGroupId);
                                rc = prvCpssDxChPortGroupWriteTableEntryField(devNum,
                                                                              portGroupId,
                                                                              PRV_CPSS_DXCH_TABLE_FDB_E,
                                                                              index,
                                                                              0,
                                                                              1,
                                                                              1,
                                                                              1);
                                if(rc != GT_OK)
                                {
                                    cpssOsFree(deleteEntryPtr);
                                    return rc;
                                }
                            }
                        }
                    }
                    PRV_CPSS_GEN_PP_END_LOOP_PORT_GROUPS_MAC(devNum,portGroupId)
                    cpssOsFree(deleteEntryPtr);
                    /* restore saved vid and vid mask */
                    rc =  cpssDxChBrgFdbActionActiveVlanSet(devNum,currentVid,currentVidMask);
                    if (rc != GT_OK)
                    {
                        return rc;
                    }
                    /* restore saved action device */
                    rc =  cpssDxChBrgFdbActionActiveDevSet(devNum,actDev,actDevMask);
                    if (rc != GT_OK)
                    {
                        return rc;
                    }
                    /* restore saved action interface */
                    rc = cpssDxChBrgFdbActionActiveInterfaceSet(devNum,actIsTrunk,actIsTrunkMask,
                                                                actTrunkPort,actTrunkPortMask);
                    if (rc != GT_OK)
                    {
                        return rc;
                    }

                    /* restore saved action trigger mode */
                    rc = cpssDxChBrgFdbMacTriggerModeSet(devNum,triggerMode);
                    if (rc != GT_OK)
                    {
                        return rc;
                    }

                    /* restore  saved action mode */
                    rc = cpssDxChBrgFdbActionModeSet(devNum,actionMode);
                    if (rc != GT_OK)
                    {
                        return rc;
                    }
                    /* restore saved action enable/disable mode */
                    rc = cpssDxChBrgFdbActionsEnableSet(devNum,actionEnable);
                    if (rc != GT_OK)
                    {
                        return rc;
                    }
                    /* restore fdbUploadState enable/disable state */
                    rc = cpssDxChBrgFdbUploadEnableSet(devNum,fdbUploadState);
                    if(rc != GT_OK)
                    {
                        return rc;
                    }
                    if(CPSS_PP_FAMILY_DXCH_LION2_E == PRV_CPSS_PP_MAC(devNum)->devFamily)
                    {
                        /* restore  maskAuFuMsg2CpuOnNonLocal state*/
                        rc = prvCpssDxChBrgFdbAuFuMessageToCpuOnNonLocalMaskEnableSet(devNum,maskAuFuMsg2CpuOnNonLocal);
                        if(rc != GT_OK)
                        {
                            return rc;
                        }
                    }
                }
            }
            if (systemRecoveryInfo.systemRecoveryMode.continuousAuMessages == GT_FALSE)
            {
                /* Restore configuration as it was before auq handling */

               /*Restore sending NA update messages to the CPU per port*/

                for(i=0; i < PRV_CPSS_PP_MAC(devNum)->numOfPorts; i++)
                {
                    /* skip not existed ports */
                    if (! PRV_CPSS_PHY_PORT_IS_EXIST_MAC(devNum, i))
                        continue;

                    rc =  cpssDxChBrgFdbNaToCpuPerPortSet(devNum,(GT_U8)i,naToCpuPerPortPtr[i]);
                    if (rc != GT_OK)
                    {
                        cpssOsFree(naToCpuPerPortPtr);
                        cpssOsFree(naStormPreventPortPtr);
                        return rc;
                    }
                    rc =  cpssDxChBrgFdbNaStormPreventSet(devNum,(GT_U8)i,naStormPreventPortPtr[i]);
                    if (rc != GT_OK)
                    {
                        cpssOsFree(naToCpuPerPortPtr);
                        cpssOsFree(naStormPreventPortPtr);
                        return rc;
                    }
                }
                cpssOsFree(naToCpuPerPortPtr);
                cpssOsFree(naStormPreventPortPtr);
                /*Restore sending NA messages to the CPU indicating that the device
                cannot learn a new SA. */

                rc = cpssDxChBrgFdbNaMsgOnChainTooLongSet(devNum,naToCpuLearnFail);
                if (rc != GT_OK)
                {
                    return rc;
                }

                /* Reatore the status of Tag1 VLAN Id assignment in vid1 field of the NA AU
                  message */
                if (PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_DXCH_XCAT2_E)
                {
                    rc = cpssDxChBrgFdbNaMsgVid1EnableSet(devNum,naTag1VLANassignment);
                    if (rc != GT_OK)
                    {
                        return rc;
                    }
                }

                /* restore sending to CPU status of AA and TA messages*/
                rc = cpssDxChBrgFdbAAandTAToCpuSet(devNum,naToCpuAaTa);
                if (rc != GT_OK)
                {
                    return rc;
                }

                /* restore Sp AA message to CPU status*/
                rc =  cpssDxChBrgFdbSpAaMsgToCpuSet(devNum,spAaMsgToCpu);
                if (rc != GT_OK)
                {
                    return rc;
                }
            }
        }
    }/* end of for*/
    return GT_OK;
}


