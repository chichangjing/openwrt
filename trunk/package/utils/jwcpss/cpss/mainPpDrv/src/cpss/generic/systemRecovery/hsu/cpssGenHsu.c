/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
*
* cpssGenHsu.c
*
* DESCRIPTION:
*       Generic HSU ApIs
*
* FILE REVISION NUMBER:
*       $Revision: 8 $
*
*******************************************************************************/
#define CPSS_LOG_IN_MODULE_ENABLE
#include <cpss/generic/cpssHwInit/cpssHwInit.h>
#include <cpssDriver/pp/hardware/prvCpssDrvHwCntl.h>
#include <cpssDriver/pp/config/prvCpssDrvPpCfgInit.h>
#include <cpss/generic/config/private/prvCpssConfigTypes.h>
#include <cpss/generic/interrupts/private/prvCpssGenIntDefs.h>
#include <cpss/generic/systemRecovery/hsu/cpssGenHsu.h>
#include <cpss/generic/systemRecovery/hsu/private/prvCpssGenHsu.h>
#include <cpss/generic/version/cpssGenStream.h>
#include <cpss/extServices/cpssExtServices.h>
#include <cpss/generic/systemRecovery/private/prvCpssGenSystemRecoveryLog.h>
#include <cpss/dxCh/dxChxGen/systemRecovery/private/prvCpssDxChSystemRecoveryLog.h>
/* generic stages address array */
GT_U8 *stageAddressArray[PRV_CPSS_GEN_HSU_LAST_STAGE_E]=
{
   (GT_U8*)(&sysGenGlobalInfo),
   (GT_U8*)(&drvGlobalInfo)
};

/* generic stages size array */
GT_U32 stageSizeArray[PRV_CPSS_GEN_HSU_LAST_STAGE_E]=
{
    sizeof(sysGenGlobalInfo.cpssPpHaDevData)+sizeof(sysGenGlobalInfo.prvCpssAfterDbRelease) + sizeof(sysGenGlobalInfo.reInitializationDbPtr),
    sizeof(PRV_CPSS_DRV_GLOBAL_DATA_STC)
};

/* generic stages array */
PRV_CPSS_GEN_HSU_DATA_STAGE_ENT stageArray[PRV_CPSS_GEN_HSU_LAST_STAGE_E]=
{
    PRV_CPSS_GEN_HSU_CPSS_STAGE_E,
    PRV_CPSS_GEN_HSU_DRV_STAGE_E
};



/*******************************************************************************
* prvCpssGenGlobalDataSizeGet
*
* DESCRIPTION:
*       This function calculate  size of generic CPSS and driver data
*
* APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3; Puma2; Puma3.
*
* NOT APPLICABLE DEVICES:
*       None.
*
* INPUTS:
*       None.
* OUTPUTS:
*       sizePtr                  -  size calculated in bytes
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PTR               - on NULL pointer
*
* COMMENTS:
*       none.
*
*******************************************************************************/
GT_STATUS prvCpssGenGlobalDataSizeGet
(
    OUT   GT_U32    *sizePtr
)
{
    GT_U32 i;
    CPSS_NULL_PTR_CHECK_MAC(sizePtr);
    *sizePtr = 0;
    for (i = PRV_CPSS_GEN_HSU_CPSS_STAGE_E; i < PRV_CPSS_GEN_HSU_LAST_STAGE_E; i++ )
    {
        *sizePtr+=stageSizeArray[i];
    }
    return GT_OK;
}
/*******************************************************************************
* prvCpssGenHsuExportImport
*
* DESCRIPTION:
*       This function exports/imports Generic HSU data to/from survived restart
*       memory area.
*
* APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3; Puma2; Puma3.
*
* NOT APPLICABLE DEVICES:
*       None.
*
* INPUTS:
*       actionType             - PRV_CPSS_HSU_EXPORT_E - export action
*                                PRV_CPSS_HSU_IMPORT_E - import action
*       currentIterPtr         - points to the current iteration.
*       hsuBlockMemSizePtr     - pointer hsu block data size supposed to be exported
*                                in current iteration.
*       hsuBlockMemPtrPtr      - pointer to HSU survived restart memory area
*
* OUTPUTS:
*       currentIterPtr         - points to the current iteration
*       hsuBlockMemSizePtr     - pointer to hsu block data size exported in current iteration.
*       accumSizePtr           - points to accumulated size
*       actiontCompletePtr     - GT_TRUE - export/import action is complete
*                                GT_FALSE - otherwise
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
GT_STATUS prvCpssGenHsuExportImport
(
    IN     PRV_CPSS_HSU_ACTION_TYPE_ENT            actionType,
    INOUT  PRV_CPSS_HSU_GEN_ITERATOR_STC           *currentIterPtr,
    INOUT  GT_U32                                  *hsuBlockMemSizePtr,
    IN     GT_U8                                   **hsuBlockMemPtrPtr,
    OUT    GT_U32                                  *accumSizePtr,
    OUT    GT_BOOL                                 *actiontCompletePtr
)
{
    GT_STATUS rc;
    PRV_CPSS_GEN_HSU_DATA_STAGE_ENT currentStage;
    GT_BOOL enoughMemoryForStage = GT_FALSE;

    for( currentStage = PRV_CPSS_GEN_HSU_CPSS_STAGE_E;
         currentStage < PRV_CPSS_GEN_HSU_LAST_STAGE_E;
         currentStage++)
    {
        if (currentIterPtr->currStage == stageArray[currentStage])
        {
            if (currentIterPtr->currStageMemPtr == NULL)
            {
                currentIterPtr->currStageMemPtr = stageAddressArray[currentStage];
                currentIterPtr->currentStageRemainedSize = stageSizeArray[currentStage];
            }
            if (*hsuBlockMemSizePtr >= currentIterPtr->currentStageRemainedSize )
            {
                enoughMemoryForStage = GT_TRUE;
            }

            rc = prvCpssGenHsuExportImportDataHandling(actionType,
                                                       currentIterPtr,
                                                       hsuBlockMemSizePtr,
                                                       hsuBlockMemPtrPtr,
                                                       accumSizePtr);
            if (rc != GT_OK)
            {
                return rc;
            }

            if (enoughMemoryForStage == GT_FALSE)
            {
                return rc;
            }
        }
    }
    *actiontCompletePtr = GT_TRUE;

    return  GT_OK;
}


/*******************************************************************************
* prvCpssGenHsuExportImportDataHandling
*
* DESCRIPTION:
*       This function handle import/export generic data
*
* APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3; Puma2; Puma3.
*
* NOT APPLICABLE DEVICES:
*       None.
*
* INPUTS:
*       actionType             - PRV_CPSS_HSU_EXPORT_E - export action
*                                PRV_CPSS_HSU_IMPORT_E - import action
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
GT_STATUS prvCpssGenHsuExportImportDataHandling
(
    IN     PRV_CPSS_HSU_ACTION_TYPE_ENT            actionType,
    INOUT  PRV_CPSS_HSU_GEN_ITERATOR_STC           *currentIterPtr,
    INOUT  GT_U32                                  *hsuBlockMemSizePtr,
    INOUT  GT_U8                                   **hsuBlockMemPtrPtr,
    OUT    GT_U32                                  *accumSizePtr
)
{
    GT_PTR dst;
    GT_PTR src;
    if (actionType == PRV_CPSS_HSU_EXPORT_E)
    {
        dst = *hsuBlockMemPtrPtr;
        src = currentIterPtr->currStageMemPtr;
    }
    else
    {
        dst = currentIterPtr->currStageMemPtr;
        src = *hsuBlockMemPtrPtr;
    }
    if (*hsuBlockMemSizePtr >= currentIterPtr->currentStageRemainedSize )
    {
        cpssOsMemCpy(dst,src,currentIterPtr->currentStageRemainedSize);
        *hsuBlockMemSizePtr = *hsuBlockMemSizePtr - currentIterPtr->currentStageRemainedSize;
        currentIterPtr->currStageMemPtr = NULL;
        currentIterPtr->currStage++;
        *accumSizePtr += currentIterPtr->currentStageRemainedSize;
        *hsuBlockMemPtrPtr += currentIterPtr->currentStageRemainedSize;
        currentIterPtr->currentStageRemainedSize = 0;
    }
    else
    {
        cpssOsMemCpy(dst, src,*hsuBlockMemSizePtr);
        currentIterPtr->currStageMemPtr += *hsuBlockMemSizePtr;
        currentIterPtr->currentStageRemainedSize = currentIterPtr->currentStageRemainedSize - *hsuBlockMemSizePtr;
        *hsuBlockMemPtrPtr += *hsuBlockMemSizePtr;
        *accumSizePtr += *hsuBlockMemSizePtr;
        *hsuBlockMemSizePtr = 0;
    }
    return GT_OK;
}



/*******************************************************************************
* internal_cpssHsuEventHandleUpdate
*
* DESCRIPTION:
*       This function replace old event  handle created after regular init
*       to new event handle that application got during HSU.
*
* APPLICABLE DEVICES: ALL
*
* INPUTS:
*       oldEvReqHndl               - old event  handle created after regular init.
*       newEvReqHndl               - new event handle  created during HSU.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                  - on success.
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS internal_cpssHsuEventHandleUpdate
(
    IN     GT_U32 oldEvReqHndl,
    IN    GT_U32  newEvReqHndl
)
{
    GT_U8 devNum;
    GT_U32 txQueueNumber,descNum;
    PRV_CPSS_TX_DESC_LIST_STC *txDescList = NULL;
    PRV_CPSS_SW_TX_DESC_STC   *firstSwTxDesc = NULL;

    for (devNum = 0; devNum < PRV_CPSS_MAX_PP_DEVICES_CNS; devNum++)
    {
        if (prvCpssPpConfig[devNum] == NULL)
        {
            continue;
        }
        for(txQueueNumber = 0; txQueueNumber < NUM_OF_TX_QUEUES; txQueueNumber++)
        {
            txDescList  = &PRV_CPSS_PP_MAC(devNum)->intCtrl.txDescList[txQueueNumber];
            firstSwTxDesc = txDescList->swTxDescBlock;
            for (descNum = 0; descNum < txDescList->maxDescNum; descNum++)
            {
                if (firstSwTxDesc[descNum].evReqHndl == oldEvReqHndl)
                {
                    firstSwTxDesc[descNum].evReqHndl = newEvReqHndl;
                }
            }
        }
    }
    return GT_OK;
}

/*******************************************************************************
* cpssHsuEventHandleUpdate
*
* DESCRIPTION:
*       This function replace old event  handle created after regular init
*       to new event handle that application got during HSU.
*
* APPLICABLE DEVICES: ALL
*
* INPUTS:
*       oldEvReqHndl               - old event  handle created after regular init.
*       newEvReqHndl               - new event handle  created during HSU.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                  - on success.
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssHsuEventHandleUpdate
(
    IN     GT_U32 oldEvReqHndl,
    IN    GT_U32  newEvReqHndl
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssHsuEventHandleUpdate);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, oldEvReqHndl, newEvReqHndl));

    rc = internal_cpssHsuEventHandleUpdate(oldEvReqHndl, newEvReqHndl);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, oldEvReqHndl, newEvReqHndl));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* prvCpssDxChHsuDataBlockVersionGet
*
* DESCRIPTION:
*       This function gets the current version of HSU data blocks
*
* APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3; Puma2; Puma3.
*
* NOT APPLICABLE DEVICES:
*       ExMx.
*
* INPUTS:
*       None.
* OUTPUTS:
*       versionPtr                - hsu data block version
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PTR               - on NULL pointer
*
* COMMENTS:
*       none.
*
*******************************************************************************/
GT_STATUS prvCpssDxChHsuDataBlockVersionGet
(
    OUT   GT_U32    *versionPtr
)
{
    GT_U32 i = 0;
    GT_U32 j = 0;
    GT_CHAR fullStreamNameStr[] = CPSS_STREAM_NAME_CNS;
    GT_CHAR digitStreamNameStr[] = CPSS_STREAM_NAME_CNS;

    CPSS_NULL_PTR_CHECK_MAC(versionPtr);
    cpssOsMemSet(digitStreamNameStr, 0, sizeof(digitStreamNameStr));
    /* convert version string to string consists only digits */

    while (fullStreamNameStr[i] != '\0')
    {
        if ( (fullStreamNameStr[i] >= '0') && (fullStreamNameStr[i] <= '9') )
        {
            digitStreamNameStr[j] = fullStreamNameStr[i];
            j++;
        }
        i++;
    }
    digitStreamNameStr[j] = '\0';

    /* convert string to integer */
    *versionPtr = cpssOsStrTo32(digitStreamNameStr);
    return GT_OK;
}
