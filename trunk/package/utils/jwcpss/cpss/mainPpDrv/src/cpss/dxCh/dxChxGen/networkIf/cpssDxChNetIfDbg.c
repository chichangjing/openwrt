/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* cpssDxChNetIfDbg.c
*
* DESCRIPTION:
*       Debug utilities for network interface.
*
* FILE REVISION NUMBER:
*       $Revision: 2 $
*
*******************************************************************************/

#include <cpss/dxCh/dxChxGen/config/private/prvCpssDxChInfo.h>

#include <cpss/dxCh/dxChxGen/networkIf/private/prvCpssDxChNetIfDbg.h>

/*******************************************************************************
* cpssDxChNetIfSdmaTxGeneratorMemCheck
*
* DESCRIPTION:
*       This function verify that descriptors and buffers memory is arranged
*       to support optimal performance. 
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum          - Device number.
*       txQueue         - The Tx SDMA queue to enable.
*
* OUTPUTS:
*       checkResultPtr  - (pointer to) memory check result status 
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PTR               - one of the parameters is NULL pointer.
*       GT_NOT_INITIALIZED       - the library was not initialized.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device.
*
* COMMENTS:
*       memory check can be performed while queue is enabled.
*  
*******************************************************************************/
GT_STATUS cpssDxChNetIfSdmaTxGeneratorMemCheck
(
    IN  GT_U8                   devNum,
    IN  GT_U8                   txQueue,
    OUT CPSS_TX_SDMA_GENERATOR_MEM_CHECK_ENT *checkResultPtr
)
{
    GT_U32      portGroupId;    /* port group id */
    PRV_CPSS_TX_DESC_LIST_STC   txDescList;   /* Tx desc. list */
    PRV_CPSS_TX_DESC_STC        *currDescPtr;   /* current descriptor pointer */
    PRV_CPSS_TX_DESC_STC        *nextDescPtr;   /* next descriptor pointer */
    GT_UINTPTR  virtAddr;       /* virtual address */
    GT_U32 sizeOfDescAndBuff;   /* The amount of memory (in bytes) that a     */
                                /* single descriptor and adjacent buffer will */
                                /* occupy, including the alignment.           */
    GT_U32   loopCounter;       /* to avoid infinite loop */
    
    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
        CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E
        | CPSS_XCAT_E | CPSS_XCAT3_E | CPSS_LION_E | CPSS_XCAT2_E | CPSS_LION2_E);
    PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_CHECK_MAC(devNum);
    PRV_CPSS_DXCH_NETIF_INITIALIZED_CHECK_MAC(devNum);

    CPSS_NULL_PTR_CHECK_MAC(checkResultPtr);

    PRV_CPSS_DXCH_DEV_TX_SDMA_QUEUE_NUM_CHECK_MAC(txQueue);

    portGroupId = PRV_CPSS_NETIF_SDMA_PORT_GROUP_ID_MAC(devNum);
    PRV_CPSS_DXCH_DEV_TX_SDMA_GENERATOR_CHECK_MAC(devNum,portGroupId,txQueue);

    txDescList  = (PRV_CPSS_PP_MAC(devNum)->intCtrl.txDescList[txQueue]);

    /* Nothing to check if queue is empty */
    if( txDescList.freeDescNum == txDescList.maxDescNum )
    {
        *checkResultPtr = CPSS_TX_SDMA_GENERATOR_MEM_CHECK_EMPTY_QUEUE_E;
        return GT_OK;
    }

    sizeOfDescAndBuff = sizeof(PRV_CPSS_TX_DESC_STC) + 
                                                    txDescList.actualBuffSize; 

    currDescPtr = txDescList.firstDescPtr;

    cpssOsPhy2Virt((GT_UINTPTR)(CPSS_32BIT_LE(currDescPtr->nextDescPointer)), 
                                &virtAddr);
    nextDescPtr = (PRV_CPSS_TX_DESC_STC*)virtAddr;

    /* In case only one buffer in chain */
    if( txDescList.maxDescNum - txDescList.freeDescNum == 1 )
    {
        cpssOsPhy2Virt((GT_UINTPTR)(CPSS_32BIT_LE(currDescPtr->buffPointer)), 
                                    &virtAddr);

        /* Check buffer pointer for coupling descriptor */
        if( (GT_U8*)virtAddr != (GT_U8*)(currDescPtr) + sizeof(PRV_CPSS_TX_DESC_STC) )
        {
            *checkResultPtr = CPSS_TX_SDMA_GENERATOR_MEM_CHECK_BAD_BUFF_PTR_E;
            return GT_OK;
        }

        /* Check correct next descriptor address -         */
        /* for only one descriptor it must point to itself */
        if( nextDescPtr != currDescPtr )
        {
            *checkResultPtr = CPSS_TX_SDMA_GENERATOR_MEM_CHECK_BAD_NEXT_DESC_E;
            return GT_OK;
        }

        *checkResultPtr = CPSS_TX_SDMA_GENERATOR_MEM_CHECK_PASS_E;
        return GT_OK;
    }

    loopCounter = 0;
    while( nextDescPtr != txDescList.firstDescPtr )
    {
        cpssOsPhy2Virt((GT_UINTPTR)(CPSS_32BIT_LE(currDescPtr->buffPointer)), 
                                    &virtAddr);
        /* Check buffer pointer for coupling descriptor */
        if( (GT_U8*)virtAddr != (GT_U8*)(currDescPtr) + sizeof(PRV_CPSS_TX_DESC_STC) )
        {
            *checkResultPtr = CPSS_TX_SDMA_GENERATOR_MEM_CHECK_BAD_BUFF_PTR_E;
            return GT_OK;
        }

        /* Check correct next descriptor address */
        if( nextDescPtr != (PRV_CPSS_TX_DESC_STC*)
                                ((GT_U8*)(currDescPtr) + sizeOfDescAndBuff) )
        {
            *checkResultPtr = CPSS_TX_SDMA_GENERATOR_MEM_CHECK_BAD_NEXT_DESC_E;
            return GT_OK;
        }
        
        currDescPtr = nextDescPtr;
        cpssOsPhy2Virt((GT_UINTPTR)(CPSS_32BIT_LE(currDescPtr->nextDescPointer)), 
                                &virtAddr);
        nextDescPtr = (PRV_CPSS_TX_DESC_STC*)virtAddr; 

        loopCounter++;
        if( loopCounter > txDescList.maxDescNum )
        {
            *checkResultPtr = CPSS_TX_SDMA_GENERATOR_MEM_CHECK_DESC_EXCEED_E;
            return GT_OK;
        }
    }

    *checkResultPtr = CPSS_TX_SDMA_GENERATOR_MEM_CHECK_PASS_E;
    return GT_OK;
}

